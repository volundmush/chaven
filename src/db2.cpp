#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#pragma warning (disable : 4800)
#endif
#endif

#include "merc.h"
#include "db.h"
#include "tables.h"
#include "lookup.h"

#if defined(__cplusplus)
extern "C" {
#endif

  extern int flag_lookup args((const char *name, const struct flag_type *flag_table));
  char *fwrite_flag args((long flags, char buf[]));

  bool load_vessel_file args((const char *fname, int number));
  bool load_port_file args((const char *fname, int number));
  int map_lookup_int args((char *name));

  int maxSubraces;
  struct subrace_type *subrace_table;

  int maxVessels;
  int maxPorts;

  /*
* Snarf a mob section.  new style
*/
  void load_mobiles(FILE *fp) {
    MOB_INDEX_DATA *pMobIndex;
    char *temp;
    if (!area_last) /* OLC */
    {
      bug("Load_mobiles: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;
      int iHash;

      letter = fread_letter(fp);
      if (letter != '#') {
        char bufa[256];
        sprintf(bufa, "Load_mobiles: # not found: %c", letter);
        bug(bufa, 0);
        bug("Load_mobiles: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      //        bool fBootDb = FALSE;
      if (get_mob_index(vnum) != NULL) {
        bug("Load_mobiles: vnum %d duplicated.", vnum);
        exit(1);
      }
      //      fBootDb = TRUE;

      pMobIndex = (MOB_INDEX_DATA *)alloc_perm(sizeof(*pMobIndex));
      pMobIndex->vnum = vnum;
      pMobIndex->area = area_last; /* OLC */
      newmobs++;
      pMobIndex->player_name = fread_string(fp);
      pMobIndex->short_descr = fread_string(fp);
      pMobIndex->long_descr = fread_string(fp);
      pMobIndex->description = fread_string(fp);

      temp = fread_string(fp);
      pMobIndex->race = race_lookup(temp);

      /*
* This is only for when converting from old race/subrace
* format to the new one.  It takes the old race of the mob
* and finds the subrace it corresponds to in the new system
* then it sets the mobs new race to the parent of the subrace.
*/
      char *temp2;
      temp2 = fread_string(fp);

      free_string(temp2);
      free_string(temp);

      pMobIndex->long_descr[0] = UPPER(pMobIndex->long_descr[0]);
      pMobIndex->description[0] = UPPER(pMobIndex->description[0]);

      /*
long act = 0, aff = 0;
act                  = fread_flag( fp );
aff          	     = fread_flag( fp );

bitvector_to_array(pMobIndex->act, act);
bitvector_to_array(pMobIndex->affected_by, aff);
*/
      set_fread_flag(fp, pMobIndex->act);
      SET_FLAG(pMobIndex->act, ACT_IS_NPC);

      set_fread_flag(fp, pMobIndex->affected_by);

      pMobIndex->pShop = NULL;
      pMobIndex->ttl = fread_number(fp);
      pMobIndex->intelligence = fread_number(fp);
      pMobIndex->weapon_type = fread_number(fp);
      pMobIndex->maxwounds = fread_number(fp);
      pMobIndex->level = fread_number(fp);

      /* read flags and add in data from the race table */
      pMobIndex->off_flags = fread_flag(fp);

      /* vital statistics */
      pMobIndex->start_pos = position_lookup(fread_word(fp));
      pMobIndex->default_pos = position_lookup(fread_word(fp));
      pMobIndex->sex = sex_lookup(fread_word(fp));

      pMobIndex->wealth = fread_number(fp);

      temp = fread_string(fp);
      if (str_cmp(temp, ""))
      free_string(temp);

      pMobIndex->form = fread_flag(fp);
      pMobIndex->parts = fread_flag(fp);
      /* size */
      pMobIndex->material = str_dup(fread_word(fp));

      for (;;) {
        letter = fread_letter(fp);

        if (letter == 'F') {
          char *word;
          long vector = 0;
          SET vect_set;
          SET_INIT(vect_set);

          word = fread_word(fp);
          if (!str_prefix(word, "act") || !str_prefix(word, "aff"))
          set_fread_flag(fp, vect_set);
          else
          vector = fread_flag(fp);

          if (!str_prefix(word, "act"))
          SET_REM_SET(pMobIndex->act, vect_set);
          else if (!str_prefix(word, "aff"))
          SET_REM_SET(pMobIndex->affected_by, vect_set);
          else if (!str_prefix(word, "off"))
          REMOVE_BIT(pMobIndex->off_flags, vector);
          else if (!str_prefix(word, "for"))
          REMOVE_BIT(pMobIndex->form, vector);
          else if (!str_prefix(word, "par"))
          REMOVE_BIT(pMobIndex->parts, vector);
          else {
            bug("Flag remove: flag not found.", 0);
            exit(1);
          }
        }
        else if (letter == 'D') {
          int point = fread_number(fp);
          int value = fread_number(fp);
          pMobIndex->disciplines[point] = value;
        }
        else if (letter == 'M') {
          PROG_LIST *pMprog;
          char *word;
          int trigger = 0;

          pMprog = (PROG_LIST *)alloc_perm(sizeof(*pMprog));
          word = fread_word(fp);
          if ((trigger = flag_lookup(word, mprog_flags)) == NO_FLAG) {
            bug("MOBprogs: invalid trigger.", 0);
            exit(1);
          }
          SET_BIT(pMobIndex->mprog_flags, trigger);
          pMprog->trig_type = trigger;
          pMprog->vnum = fread_number(fp);
          pMprog->trig_phrase = fread_string(fp);
          pMprog->next = pMobIndex->mprogs;
          pMobIndex->mprogs = pMprog;
        }
        else {
          ungetc(letter, fp);
          break;
        }
      }

      iHash = vnum % MAX_KEY_HASH;
      pMobIndex->next = mob_index_hash[iHash];
      mob_index_hash[iHash] = pMobIndex;
      top_mob_index++;
      top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob; /* OLC */
      //        assign_area_vnum( vnum );                                  /* OLC
      //        */
    }

    return;
  }

  /*
* Snarf an obj section. new style
*/
  void load_objects(FILE *fp) {
    OBJ_INDEX_DATA *pObjIndex;
    //   bool fBootDb;
    int newobjs = 0;
    if (!area_last) /* OLC */
    {
      bug("Load_objects: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;
      int iHash;

      letter = fread_letter(fp);
      if (letter != '#') {
        bug("Load_objects: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      // fBootDb = FALSE;
      if (get_obj_index(vnum) != NULL) {
        bug("Load_objects: vnum %d duplicated.", vnum);
        exit(1);
      }
      // fBootDb = TRUE;

      pObjIndex = (OBJ_INDEX_DATA *)alloc_perm(sizeof(*pObjIndex));
      pObjIndex->vnum = vnum;
      pObjIndex->area = area_last; /* OLC */
      pObjIndex->reset_num = 0;
      newobjs++;
      pObjIndex->name = fread_string(fp);
      pObjIndex->short_descr = fread_string(fp);
      pObjIndex->description = fread_string(fp);
      pObjIndex->material = fread_string(fp);
      pObjIndex->wear_string = fread_string(fp);
      CHECK_POS(pObjIndex->item_type, item_lookup(fread_word(fp)), "item_type");
      pObjIndex->extra_flags = fread_flag(fp);
      pObjIndex->wear_flags = fread_flag(fp);
      pObjIndex->rot_timer = fread_number(fp);

      if (pObjIndex->rot_timer <= 0)
      pObjIndex->rot_timer = -1;

      switch (pObjIndex->item_type) {
      case ITEM_WEAPON:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = attack_lookup(fread_word(fp));
        pObjIndex->value[4] = fread_flag(fp);
        break;
      case ITEM_CONTAINER:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_flag(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;
      case ITEM_JEWELRY:
      case ITEM_CLOTHING:
      case ITEM_KEY:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        pObjIndex->value[5] = fread_number(fp);
        break;

      case ITEM_RANGED:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        pObjIndex->value[5] = fread_number(fp);
        break;

      case ITEM_BLOODCRYSTAL:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_ARTIFACT:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_FLASHLIGHT:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_PERFUME:
      case ITEM_GAME:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_PHONE:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_DRUGS:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_BABY:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_flag(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_KEYRING:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        pObjIndex->value[2] = fread_number(fp);
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;

      case ITEM_DRINK_CON:
      case ITEM_FOUNTAIN:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_number(fp);
        CHECK_POS(pObjIndex->value[2], liq_lookup(fread_word(fp)), "liq_lookup");
        pObjIndex->value[3] = fread_number(fp);
        pObjIndex->value[4] = fread_number(fp);
        break;
      case ITEM_POTION:
        pObjIndex->value[0] = fread_number(fp);
        pObjIndex->value[1] = fread_flag(fp);
        pObjIndex->value[2] = fread_flag(fp);
        pObjIndex->value[3] = fread_flag(fp);
        pObjIndex->value[4] = fread_flag(fp);
        pObjIndex->value[5] = fread_flag(fp);
        break;
      default:
        pObjIndex->value[0] = fread_flag(fp);
        pObjIndex->value[1] = fread_flag(fp);
        pObjIndex->value[2] = fread_flag(fp);
        pObjIndex->value[3] = fread_flag(fp);
        pObjIndex->value[4] = fread_flag(fp);
        pObjIndex->value[5] = fread_flag(fp);
        break;
      }

      pObjIndex->load_chance = fread_number(fp);
      // pObjIndex->load_chance		= 100;
      pObjIndex->level = fread_number(fp);
      pObjIndex->size = fread_number(fp);
      pObjIndex->cost = fread_number(fp);

      /* condition */
      pObjIndex->condition =
      fread_number(fp); // Hack by Palin, converting to new armor hp system.
      /*
switch (letter)
{
case ('P') :		pObjIndex->condition = 100; break;
case ('G') :		pObjIndex->condition =  90; break;
case ('A') :		pObjIndex->condition =  75; break;
case ('W') :		pObjIndex->condition =  50; break;
case ('D') :		pObjIndex->condition =  25; break;
case ('B') :		pObjIndex->condition =  10; break;
case ('R') :		pObjIndex->condition =   0; break;
default:			pObjIndex->condition = 100;
break;
}
*/
      pObjIndex->fuzzy = fread_number(fp);

      for (;;) {
        char letter;

        letter = fread_letter(fp);

        if (letter == 'A') {
          AFFECT_DATA *paf;

          paf = (AFFECT_DATA *)alloc_perm(sizeof(*paf));
          paf->where = TO_OBJECT;
          paf->type = -1;
          paf->level = fread_number(fp); // pObjIndex->level;
          // paf->level              = pObjIndex->level;
          paf->duration = -1;
          paf->location = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->bitvector = 0;
          paf->next = pObjIndex->affected;
          pObjIndex->affected = paf;
          top_affect++;
        }
        else if (letter == 'B') {
          pObjIndex->buff = fread_number(fp);
          pObjIndex->faction = fread_number(fp);
        }
        else if (letter == 'F') {
          AFFECT_DATA *paf;

          paf = (AFFECT_DATA *)alloc_perm(sizeof(*paf));
          letter = fread_letter(fp);
          switch (letter) {
          case 'A':
            paf->where = TO_AFFECTS;
            break;
          default:
            bug("Load_objects: Bad where on flag set.", 0);
            exit(1);
          }
          paf->type = -1;
          paf->level = pObjIndex->level;
          paf->duration = -1;
          paf->location = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->bitvector = fread_flag(fp);
          paf->next = pObjIndex->affected;
          pObjIndex->affected = paf;
          top_affect++;
        }

        else if (letter == 'E') {
          EXTRA_DESCR_DATA *ed;

          ed = (EXTRA_DESCR_DATA *)alloc_perm(sizeof(*ed));
          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = pObjIndex->extra_descr;
          pObjIndex->extra_descr = ed;
          top_ed++;
        }
        else if (letter == 'O') {
          PROG_LIST *pOprog;
          char *word;
          int trigger = 0;

          pOprog = (PROG_LIST *)alloc_perm(sizeof(*pOprog));
          word = fread_word(fp);
          if (!(trigger = flag_lookup(word, oprog_flags))) {
            bug("OBJprogs: invalid trigger.", 0);
            exit(1);
          }
          SET_BIT(pObjIndex->oprog_flags, trigger);
          pOprog->trig_type = trigger;
          pOprog->vnum = fread_number(fp);
          pOprog->trig_phrase = fread_string(fp);
          pOprog->next = pObjIndex->oprogs;
          pObjIndex->oprogs = pOprog;
        }

        else {
          ungetc(letter, fp);
          break;
        }
      }

      iHash = vnum % MAX_KEY_HASH;
      pObjIndex->next = obj_index_hash[iHash];
      obj_index_hash[iHash] = pObjIndex;
      top_obj_index++;
      top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj; /* OLC */
      assign_area_vnum(vnum);                                   /* OLC */
    }

    return;
  }

  /*****************************************************************************
Name:	        convert_objects
Purpose:	Converts all old format objects to new format
Called by:	boot_db (db.c).
Note:          Loops over all resets to find the level of the mob
loaded before the object to determine the level of
the object.
It might be better to update the levels in load_resets().
This function is not pretty.. Sorry about that :)
Author:        Hugin
****************************************************************************/
  void convert_objects(void) {
    int vnum;
    AREA_DATA *pArea;
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pMob = NULL;
    OBJ_INDEX_DATA *pObj;
    ROOM_INDEX_DATA *pRoom;

    if (newobjs == top_obj_index)
    return; /* all objects in new format */

    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;
      for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
        if (!(pRoom = get_room_index(vnum)))
        continue;

        for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
          switch (pReset->command) {
          case 'M':
            if (!(pMob = get_mob_index(pReset->arg1)))
            bug("Convert_objects: 'M': bad vnum %d.", pReset->arg1);
            break;

          case 'O':
            if (!(pObj = get_obj_index(pReset->arg1))) {
              bug("Convert_objects: 'O': bad vnum %d.", pReset->arg1);
              break;
            }

            // if ( pObj->new_format )
            continue;

            if (!pMob) {
              bug("Convert_objects: 'O': No mob reset yet.", 0);
              break;
            }

            pObj->level = pObj->level < 1 ? pMob->level - 2
            : UMIN(pObj->level, pMob->level - 2);
            break;

          case 'P': {
              OBJ_INDEX_DATA *pObj, *pObjTo;

              if (!(pObj = get_obj_index(pReset->arg1))) {
                bug("Convert_objects: 'P': bad vnum %d.", pReset->arg1);
                break;
              }

              // if ( pObj->new_format )
              continue;

              if (!(pObjTo = get_obj_index(pReset->arg3))) {
                bug("Convert_objects: 'P': bad vnum %d.", pReset->arg3);
                break;
              }

              pObj->level = pObj->level < 1 ? pObjTo->level
              : UMIN(pObj->level, pObjTo->level);
            } break;

          case 'G':
          case 'E':
            if (!(pObj = get_obj_index(pReset->arg1))) {
              bug("Convert_objects: 'E' or 'G': bad vnum %d.", pReset->arg1);
              break;
            }

            if (!pMob) {
              bug("Convert_objects: 'E' or 'G': null mob for vnum %d.", pReset->arg1);
              break;
            }

            // if ( pObj->new_format )
            continue;

            if (pMob->pShop) {
              switch (pObj->item_type) {
              default:
                pObj->level = UMAX(0, pObj->level);
                break;
              case ITEM_POTION:
                pObj->level = UMAX(5, pObj->level);
                break;
              case ITEM_ARMOR:
              case ITEM_WEAPON:
                pObj->level = UMAX(10, pObj->level);
                break;
              }
            }
            else
            pObj->level =
            pObj->level < 1 ? pMob->level : UMIN(pObj->level, pMob->level);
            break;
          } /* switch ( pReset->command ) */
        }
      }
    }

    /* do the conversion: */
    /*
for ( pArea = area_first; pArea ; pArea = pArea->next )
for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
if ( (pObj = get_obj_index( vnum )) )
if ( !pObj->new_format )
convert_object( pObj );
*/
    return;
  }

  /*****************************************************************************
Name:		convert_object
Purpose:	Converts an old_format obj to new_format
Called by:	convert_objects (db2.c).
Note:          Dug out of create_obj (db.c)
Author:        Hugin
****************************************************************************/
  void convert_object(OBJ_INDEX_DATA *pObjIndex) {
    int level;
    int number, type; /* for dice-conversion */

    return;
    // if ( !pObjIndex || pObjIndex->new_format ) return;

    level = pObjIndex->level;

    pObjIndex->level = UMAX(0, pObjIndex->level); /* just to be sure */
    pObjIndex->cost = 10 * level;

    switch (pObjIndex->item_type) {
    default:
      bug("Obj_convert: vnum %d bad type.", pObjIndex->item_type);
      break;

    case ITEM_LIGHT:
    case ITEM_PERFUME:
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_KEYRING:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_FLASHLIGHT:
    case ITEM_CLOTHING:
    case ITEM_COMPONENT:
    case ITEM_STAKE:
    case ITEM_LIGHTER:
    case ITEM_BLOODCRYSTAL:
    case ITEM_ARTIFACT:

      break;

    case ITEM_WEAPON:

      /*
* The conversion below is based on the values generated
* in one_hit() (fight.c).  Since I don't want a lvl 50
* weapon to do 15d3 damage, the min value will be below
* the one in one_hit, and to make up for it, I've made
* the max value higher.
* (I don't want 15d2 because this will hardly ever roll
* 15 or 30, it will only roll damage close to 23.
* I can't do 4d8+11, because one_hit there is no dice-
* bounus value to set...)
*
* The conversion below gives:

level:   dice      min      max      mean
1:     1d8      1( 2)    8( 7)     5( 5)
2:     2d5      2( 3)   10( 8)     6( 6)
3:     2d5      2( 3)   10( 8)     6( 6)
5:     2d6      2( 3)   12(10)     7( 7)
10:     4d5      4( 5)   20(14)    12(10)
20:     5d5      5( 7)   25(21)    15(14)
30:     5d7      5(10)   35(29)    20(20)
50:     5d11     5(15)   55(44)    30(30)

*/

      number = UMIN(level / 4 + 1, 5);
      type = (level + 7) / number;

      pObjIndex->value[1] = number;
      pObjIndex->value[2] = type;
      break;

    case ITEM_ARMOR:
      pObjIndex->value[0] = level / 5 + 3;
      pObjIndex->value[1] = pObjIndex->value[0];
      pObjIndex->value[2] = pObjIndex->value[0];
      break;

    case ITEM_POTION:
      break;

    case ITEM_MONEY:
      pObjIndex->value[0] = pObjIndex->cost;
      break;
    }

    ++newobjs;

    return;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

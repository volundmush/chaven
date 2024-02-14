/* OLC_SAVE.C
* This takes care of saving all the .are information.
* Notes:
* -If a good syntax checker is used for setting vnum ranges of areas
*  then it would become possible to just cycle through vnums instead
*  of using the iHash stuff and checking that the room or reset or
*  mob etc is part of that area.
*/
#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include "merc.h"
#include "olc.h"
#include "tables.h"
#include <ctype.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

#if defined(__cplusplus)
extern "C" {
#endif

#define DIF(a, b) (~((~a) | (b)))
  void do_function args((CHAR_DATA * ch, DO_FUN *do_fun, char *argument));

  /*
*  Verbose writes reset data in plain english into the comments
*  section of the resets.  It makes areas considerably larger but
*  may aid in debugging.
*/
  /* #define VERBOSE */

  char *xml_help(const char *str) {
    const char *point;
    static char buffer[MSL];
    int x;

    if (str) {
      sprintf(buffer, "\t\t\t<Line>");
      x = 9;
      for (point = str; *point; point++) {
        if (*point == '`') {
          point++;
          continue;
        }

        if (*point == '~')
        continue;

        if (*point == '\n') {
          strcat(buffer, "</Line>\n\t\t\t<Line>");
          x += 17;
          continue;
        }

        if (*point == '<') {
          strcat(buffer, "&lt;");
          x += 4;
          continue;
        }

        if (*point == '>') {
          strcat(buffer, "&gt;");
          x += 4;
          continue;
        }

        if (*point == '&') {
          strcat(buffer, "&amp;");
          x += 5;
          continue;
        }

        if (*point == '\r')
        continue;

        buffer[x] = *point;
        buffer[++x] = '\0';
      }
      strcat(buffer, "</Line>");
      buffer[x + 8] = '\0';
      return buffer;
    }
    return '\0';
  }

  /*****************************************************************************
Name:		fix_string
Purpose:	Returns a string without \r and ~.
****************************************************************************/
  char *fix_string(const char *str) {
    static char strfix[MAX_STRING_LENGTH * 4];
    int i;
    int o;

    if (str == NULL)
    return '\0';

    for (o = i = 0; str[i + o] != '\0'; i++) {
      if (str[i + o] == '\r' || str[i + o] == '~')
      o++;
      strfix[i] = str[i + o];
    }
    strfix[i] = '\0';
    return strfix;
  }

  /*****************************************************************************
Name:		save_area_list
Purpose:	Saves the listing of files to be loaded at startup.
Called by:	do_asave(olc_save.c).
****************************************************************************/
  void save_area_list() {
    FILE *fp;
    extern HELP_AREA *had_list;
    HELP_AREA *ha;

    if ((fp = fopen("area.lst", "w")) == NULL) {
      bug("Save_area_list: fopen", 0);
      perror("area.lst");
    }
    else {
      /*
* Add any help files that need to be loaded at
* startup to this section.
*/
      // fprintf( fp, "social.are\n" );    /* ROM OLC */

      for (ha = had_list; ha; ha = ha->next)
      if (ha->area == NULL)
      fprintf(fp, "%s\n", ha->filename);

      for (AreaList::iterator it = area_list.begin(); it != area_list.end();
      ++it) {
        fprintf(fp, "%s\n", (*it)->file_name);
      }

      fprintf(fp, "$\n");
      fclose(fp);
    }

    return;
  }

  /*
* ROM OLC
* Used in save_mobile and save_object below.  Writes
* flags on the form fread_flag reads.
*
* buf[] must hold at least 32+1 characters.
*
* -- Hugin
*/
  char *fwrite_flag(long flags, char buf[]) {
    char offset;
    char *cp;

    buf[0] = '\0';

    if (flags == 0) {
      strcpy(buf, "0");
      return buf;
    }

    /* 32 -- number of bits in a long */

    for (offset = 0, cp = buf; offset < 32; offset++)
    if (flags & ((long)1 << offset)) {
      if (offset <= 'Z' - 'A')
      *(cp++) = 'A' + offset;
      else
      *(cp++) = 'a' + offset - ('Z' - 'A' + 1);
    }

    *cp = '\0';

    return buf;
  }

  void save_groups(FILE *fp, AREA_DATA *pArea) {
    return;
    GROUP_INDEX_DATA *pGroup;
    int i;

    fprintf(fp, "#GROUPS\n");

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
      if ((pGroup = get_group_index(i)) != NULL) {
        fprintf(fp, "#%d\n", i);
        for (int x = 0; x < 6; x++)
        fprintf(fp, "%d %d %d ", pGroup->member_vnum[x], pGroup->member_position[x], pGroup->member_bonus[x]);
        fprintf(fp, "\n");
      }
    }
    fprintf(fp, "#0\n\n");
    return;
  }

  void save_mobprogs(FILE *fp, AREA_DATA *pArea) {
    return;
    PROG_CODE *pMprog;
    int i;

    fprintf(fp, "#MOBPROGS\n");

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
      if ((pMprog = get_prog_index(i, PRG_MPROG)) != NULL) {
        fprintf(fp, "#%d\n", i);
        fprintf(fp, "%s~\n", fix_string(pMprog->code));
      }
    }

    fprintf(fp, "#0\n\n");
    return;
  }

  /*****************************************************************************
Name:          save_mobile
Purpose:       Save one mobile to file, new format -- Hugin
Called by:     save_mobiles (below).
****************************************************************************/
  // Add the following 39 lines

  void save_objprogs(FILE *fp, AREA_DATA *pArea) {
    return;
    PROG_CODE *pOprog;
    int i;

    fprintf(fp, "#OBJPROGS\n");

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
      if ((pOprog = get_prog_index(i, PRG_OPROG)) != NULL) {
        fprintf(fp, "#%d\n", i);
        fprintf(fp, "%s~\n", fix_string(pOprog->code));
      }
    }

    fprintf(fp, "#0\n\n");
    return;
  }

  void save_roomprogs(FILE *fp, AREA_DATA *pArea) {
    return;
    PROG_CODE *pRprog;
    int i;

    fprintf(fp, "#ROOMPROGS\n");

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
      if ((pRprog = get_prog_index(i, PRG_RPROG)) != NULL) {
        fprintf(fp, "#%d\n", i);
        fprintf(fp, "%s~\n", fix_string(pRprog->code));
      }
    }

    fprintf(fp, "#0\n\n");
    return;
  }

  /*****************************************************************************
Name:		save_mobile
Purpose:	Save one mobile to file, new format -- Hugin
Called by:	save_mobiles (below).
****************************************************************************/
  void save_mobile(FILE *fp, MOB_INDEX_DATA *pMobIndex) {
    char buf[MAX_STRING_LENGTH];
    SET set_temp;
    SET_INIT(set_temp);

    if (pMobIndex == NULL || !pMobIndex || pMobIndex->vnum < 0 || pMobIndex->race < 0)
    return;

    fprintf(fp, "#%d\n", pMobIndex->vnum);
    fprintf(fp, "%s~\n", pMobIndex->player_name);
    fprintf(fp, "%s~\n", pMobIndex->short_descr);
    fprintf(fp, "%s~\n", fix_string(pMobIndex->long_descr));
    fprintf(fp, "%s~\n", fix_string(pMobIndex->description));
    fprintf(fp, "%s~\n", race_table[pMobIndex->race].name);
    fprintf(fp, "%s~\n", "");
    fprintf(fp, "%s\n", set_print_flag(pMobIndex->act));
    fprintf(fp, "%s\n", set_print_flag(pMobIndex->affected_by));
    fprintf(fp, "%d\n", pMobIndex->ttl);
    fprintf(fp, "%d\n", pMobIndex->intelligence);
    fprintf(fp, "%d\n", pMobIndex->weapon_type);
    fprintf(fp, "%d\n", pMobIndex->maxwounds);
    fprintf(fp, "%d ", pMobIndex->level);
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->off_flags, buf));
    fprintf(fp, "%s %s %s %ld\n", position_table[pMobIndex->start_pos].short_name, position_table[pMobIndex->default_pos].short_name, sex_table[pMobIndex->sex].name, pMobIndex->wealth);
    fprintf(fp, "%s~\n", "");
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->form, buf));
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->parts, buf));

    fprintf(fp, "unknown\n");

    for (int b = 0; b < MAX_DIS; b++) {
      if (pMobIndex->disciplines[b] > 0)
      fprintf(fp, "D %d %d\n", b, pMobIndex->disciplines[b]);
    }

    if (!SET_IS_ZERO(set_temp))
    fprintf(fp, "F act %s\n", set_print_flag(set_temp));
    /*
SET_DIF(subrace_table[pMobIndex->sub_race].aff, pMobIndex->affected_by, set_temp); if( !SET_IS_ZERO(set_temp) ) fprintf( fp, "F aff %s\n", set_print_flag(set_temp) );
*/
    /*
if ((temp = DIF(subrace_table[pMobIndex->sub_race].aff, pMobIndex->affected_by))) fprintf( fp, "F aff %s\n", fwrite_flag(temp, buf)
);
*/

    fprintf(fp, "\n");
    return;
  }

  void save_mobile_new(FILE *fp, MOB_INDEX_DATA *pMobIndex) {
    char buf[MAX_STRING_LENGTH];
    SET set_temp;
    SET_INIT(set_temp);

    fprintf(fp, "#%d\n", pMobIndex->vnum);
    fprintf(fp, "Name %s~\n", pMobIndex->player_name);
    fprintf(fp, "SDesc %s~\n", pMobIndex->short_descr);
    fprintf(fp, "LDesc %s~\n", fix_string(pMobIndex->long_descr));
    fprintf(fp, "Descr %s~\n", fix_string(pMobIndex->description));
    fprintf(fp, "Race %s~\n", race_table[pMobIndex->race].name);
    fprintf(fp, "Act %s\n", set_print_flag(pMobIndex->act));
    fprintf(fp, "AfBy %s\n", set_print_flag(pMobIndex->affected_by));
    fprintf(fp, "Grp %d\n", pMobIndex->group);
    fprintf(fp, "Lvl %d\n", pMobIndex->level);
    fprintf(fp, "Off %s\n", fwrite_flag(pMobIndex->off_flags, buf));
    fprintf(fp, "Pos %s %s\n", position_table[pMobIndex->start_pos].short_name, position_table[pMobIndex->default_pos].short_name);
    fprintf(fp, "Sex %s\n", sex_table[pMobIndex->sex].name);
    fprintf(fp, "Wealth %ld\n", pMobIndex->wealth);
    fprintf(fp, "Frm %s\n", fwrite_flag(pMobIndex->form, buf));
    fprintf(fp, "Prt %s\n", fwrite_flag(pMobIndex->parts, buf));

    fprintf(fp, "Mat unknown\n");

    for (int b = 0; b < MAX_DIS; b++) {
      if (pMobIndex->disciplines[b] > 0)
      fprintf(fp, "D %d %d\n", b, pMobIndex->disciplines[b]);
    }

    /*
SET_DIF(subrace_table[pMobIndex->sub_race].aff, pMobIndex->affected_by, set_temp); if( !SET_IS_ZERO(set_temp) ) fprintf( fp, "F aff %s\n", set_print_flag(set_temp) );
*/
    /*
if ((temp = DIF(subrace_table[pMobIndex->sub_race].aff, pMobIndex->affected_by))) fprintf( fp, "F aff %s\n", fwrite_flag(temp, buf)
);
*/

    fprintf(fp, "\n");
    return;
  }

  /*****************************************************************************
Name:		save_mobiles
Purpose:	Save #MOBILES secion of an area file.
Called by:	save_area(olc_save.c).
Notes:         Changed for ROM OLC.
****************************************************************************/
  void save_mobiles(FILE *fp, AREA_DATA *pArea) {
    int iHash;
    MOB_INDEX_DATA *pMob;

    fprintf(fp, "#MOBILES\n");

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pMob = mob_index_hash[iHash]; pMob; pMob = pMob->next) {
        if (pMob->area == pArea)
        save_mobile(fp, pMob);
      }
    }
    /*
for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
{
if ( (pMob = get_mob_index( i )) )
save_mobile( fp, pMob );
}
*/
    fprintf(fp, "#0\n\n\n\n");
    return;
  }

  /*****************************************************************************
Name:		save_object
Purpose:	Save one object to file.
new ROM format saving -- Hugin
Called by:	save_objects (below).
****************************************************************************/
  void save_object(FILE *fp, OBJ_INDEX_DATA *pObjIndex) {
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;
    char buf[MAX_STRING_LENGTH];

    fprintf(fp, "#%d\n", pObjIndex->vnum);
    fprintf(fp, "%s~\n", pObjIndex->name);
    fprintf(fp, "%s~\n", pObjIndex->short_descr);
    fprintf(fp, "%s~\n", fix_string(pObjIndex->description));
    fprintf(fp, "%s~\n", pObjIndex->material);
    fprintf(fp, "%s~\n", pObjIndex->wear_string);
    fprintf(fp, "%s ", item_name(pObjIndex->item_type));
    fprintf(fp, "%s ", fwrite_flag(pObjIndex->extra_flags, buf));
    fprintf(fp, "%s\n", fwrite_flag(pObjIndex->wear_flags, buf));
    fprintf(fp, "%d\n", (pObjIndex->rot_timer > 100) ? -1 : pObjIndex->rot_timer);
    /*
*  Using fwrite_flag to write most values gives a strange
*  looking area file, consider making a case for each
*  item type later.
*/

    switch (pObjIndex->item_type) {
    default:
      fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[0], buf));
      fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[1], buf));
      fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[2], buf));
      fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[3], buf));
      fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[4], buf));
      fprintf(fp, "%s\n", fwrite_flag(pObjIndex->value[5], buf));
      break;

    case ITEM_DRINK_CON:
    case ITEM_FOUNTAIN:
      fprintf(fp, "%d %d '%s' %d %d\n", pObjIndex->value[0], pObjIndex->value[1], liq_table[pObjIndex->value[2]].liq_name, pObjIndex->value[3], pObjIndex->value[4]);
      break;

    case ITEM_KEYRING:
      fprintf(fp, "%d ", pObjIndex->value[0]);
      fprintf(fp, "%d 0 0 0\n", pObjIndex->value[1]);
      break;

    case ITEM_CONTAINER:
      fprintf(fp, "%d %s %d %d %d\n", pObjIndex->value[0], fwrite_flag(pObjIndex->value[1], buf), pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
      break;
    case ITEM_CLOTHING:
    case ITEM_JEWELRY:
    case ITEM_KEY:
    case ITEM_CORPSE_NPC:
      fprintf(fp, "%d %d %d %d %d 0\n", pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
      break;

    case ITEM_RANGED:
      fprintf(fp, "%d %d %d %d 0 0\n", pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2], pObjIndex->value[3]);
      break;

    case ITEM_BLOODCRYSTAL:
      fprintf(fp, "%d %d %d %d %d\n", pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
      break;
    case ITEM_ARTIFACT:
      fprintf(fp, "%d %d 0 0 0\n", pObjIndex->value[0], pObjIndex->value[1]);
      break;

    case ITEM_PHONE:
      fprintf(fp, "%d %d %d %d %d\n", pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
      break;

    case ITEM_DRUGS:
      fprintf(fp, "%d %d 0 0 0\n", pObjIndex->value[0], pObjIndex->value[1]);
      break;

    case ITEM_BABY:
      fprintf(fp, "%d %s %d %d %d\n", pObjIndex->value[0], fwrite_flag(pObjIndex->value[1], buf), pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
      break;

    case ITEM_FLASHLIGHT:
      fprintf(fp, "%d 0 0 0 0\n", pObjIndex->value[0]);
      break;

    case ITEM_PERFUME:
    case ITEM_GAME:
      fprintf(fp, "%d 0 0 0 0\n", pObjIndex->value[0]);
      break;

    case ITEM_WEAPON:
      fprintf(fp, "%d %d %d %s %s\n", pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2], "none", fwrite_flag(pObjIndex->value[4], buf));
      break;

    case ITEM_POTION:
      fprintf(fp, "%d '%s' '%s' '%s' '%s'\n", pObjIndex->value[0] > 0 ? pObjIndex->value[0] : 0, "", "", "", "");
      break;
    }

    fprintf(fp, "%d ", pObjIndex->load_chance);
    fprintf(fp, "%d ", pObjIndex->level);
    fprintf(fp, "%d ", pObjIndex->size);
    fprintf(fp, "%d ", pObjIndex->cost);

    fprintf(fp, "%d ", pObjIndex->condition);
    fprintf(fp, "%d\n", pObjIndex->fuzzy);

    /*
if ( pObjIndex->condition >  90 ) letter = 'P';
else if ( pObjIndex->condition >  75 ) letter = 'G';
else if ( pObjIndex->condition >  50 ) letter = 'A';
else if ( pObjIndex->condition >  25 ) letter = 'W';
else if ( pObjIndex->condition >  10 ) letter = 'D';
else if ( pObjIndex->condition >   0 ) letter = 'B';
else                                   letter = 'R';
{
fprintf( fp, "%c ", letter );
fprintf( fp, "%d\n", pObjIndex->fuzzy);
}
//	fprintf(fp, "%c\n",letter);
*/

    if (pObjIndex->buff != 0 || pObjIndex->faction > 0) {
      fprintf(fp, "B\n%d %d\n", pObjIndex->buff, pObjIndex->faction);
    }

    for (pAf = pObjIndex->affected; pAf; pAf = pAf->next) {
      if (pAf->where == TO_OBJECT || pAf->bitvector == 0)
      fprintf(fp, "A\n%d %d %d\n", pAf->level, pAf->location, pAf->modifier);
      else {
        fprintf(fp, "F\n");

        switch (pAf->where) {
        case TO_AFFECTS:
          fprintf(fp, "A ");
          break;
        case TO_IMMUNE:
          fprintf(fp, "I ");
          break;
        case TO_RESIST:
          fprintf(fp, "R ");
          break;
        case TO_VULN:
          fprintf(fp, "V ");
          break;
        default:
          bug("olc_save: Invalid Affect->where", 0);
          break;
        }

        fprintf(fp, "%d %d %s\n", pAf->location, pAf->modifier, fwrite_flag(pAf->bitvector, buf));
      }
    }

    for (pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next) {
      if (safe_strlen(pEd->keyword) > 1 && safe_strlen(pEd->description) > 4)
      fprintf(fp, "E\n%s~\n%s~\n", pEd->keyword, fix_string(pEd->description));
    }

    return;
  }

  /*****************************************************************************
Name:		save_objects
Purpose:	Save #OBJECTS section of an area file.
Called by:	save_area(olc_save.c).
Notes:         Changed for ROM OLC.
****************************************************************************/
  void save_objects(FILE *fp, AREA_DATA *pArea) {
    int iHash;
    OBJ_INDEX_DATA *pObj;

    fprintf(fp, "#OBJECTS\n");

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pObj = obj_index_hash[iHash]; pObj; pObj = pObj->next) {
        if (pObj->area == pArea)
        save_object(fp, pObj);
      }
    }
    /*
for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
{
if ( (pObj = get_obj_index( i )) )
save_object( fp, pObj );
}
*/

    fprintf(fp, "#0\n\n\n\n");
    return;
  }

  /*****************************************************************************
Name:		save_rooms
Purpose:	Save #ROOMS section of an area file.
Called by:	save_area(olc_save.c).
****************************************************************************/
  void save_rooms(FILE *fp, AREA_DATA *pArea) {
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA *pExit;
    int iHash;
    int door;

    char buf[MSL];

    fprintf(fp, "#ROOMS\n");
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == pArea) { // old areas don't always respond well to
          // losing rooms - Discordance
          // Kill rooms with no exits.
          if (pRoomIndex->area->vnum >= HAVEN_TOWN_VNUM && pRoomIndex->area->vnum <= HELL_FOREST_VNUM) {
            // No more saving unwanted rooms without exits - Discordance
            if (pRoomIndex->exit[DIR_NORTH] == NULL && pRoomIndex->exit[DIR_EAST] == NULL && pRoomIndex->exit[DIR_SOUTH] == NULL && pRoomIndex->exit[DIR_WEST] == NULL && pRoomIndex->exit[DIR_NORTHWEST] == NULL && pRoomIndex->exit[DIR_NORTHEAST] == NULL && pRoomIndex->exit[DIR_SOUTHEAST] == NULL && pRoomIndex->exit[DIR_SOUTHWEST] == NULL && pRoomIndex->exit[DIR_UP] == NULL && pRoomIndex->exit[DIR_DOWN] == NULL && pRoomIndex->vnum != 18999 && (pRoomIndex->vnum < INIT_FIGHTROOM || pRoomIndex->vnum > INIT_FIGHTROOM + 30)) {
              sprintf(buf, "KILLROOM: No exits: %d.", pRoomIndex->vnum);
              log_string(buf);
              continue;
            }
          }
          // Kill underground rooms with no connections or description
          if (pRoomIndex->area->vnum >= HAVEN_TOWN_VNUM && pRoomIndex->area->vnum <= HELL_FOREST_VNUM) {
            if ((pRoomIndex->exit[DIR_NORTH] == NULL || pRoomIndex->exit[DIR_NORTH]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_NORTHEAST] == NULL || pRoomIndex->exit[DIR_NORTHEAST]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_EAST] == NULL || pRoomIndex->exit[DIR_EAST]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_SOUTHEAST] == NULL || pRoomIndex->exit[DIR_SOUTHEAST]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_SOUTH] == NULL || pRoomIndex->exit[DIR_SOUTH]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_SOUTHWEST] == NULL || pRoomIndex->exit[DIR_SOUTHWEST]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_WEST] == NULL || pRoomIndex->exit[DIR_WEST]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_NORTHWEST] == NULL || pRoomIndex->exit[DIR_NORTHWEST]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_UP] == NULL || pRoomIndex->exit[DIR_UP]->wall == WALL_BRICK) && (pRoomIndex->exit[DIR_DOWN] == NULL || pRoomIndex->exit[DIR_DOWN]->wall == WALL_BRICK) && pRoomIndex->z < 0 && safe_strlen(pRoomIndex->description) < 3) {
              sprintf(buf, "KILLROOM: Underground, no doors, no desc: %d.", pRoomIndex->vnum);
              log_string(buf);

              kill_room(pRoomIndex);
              continue;
            }
          }
          if (pRoomIndex->area->vnum >= 16 && pRoomIndex->area->vnum <= 28) {
            if (prop_from_room(pRoomIndex) == NULL) {
              if (pRoomIndex->z < 0) {
                sprintf(buf, "KILLROOM: Underground, no property: %d.", pRoomIndex->vnum);
                log_string(buf);

                kill_room(pRoomIndex);
                continue;
              }
              if (pRoomIndex->z > 1) {
                bool touching = FALSE;
                for (int i = 0; i < 10; i++) {
                  if (pRoomIndex->exit[i] != NULL && pRoomIndex->exit[i]->u1.to_room != NULL && prop_from_room(pRoomIndex->exit[i]->u1.to_room) != NULL)
                  touching = TRUE;
                }
                if (touching == FALSE) {
                  sprintf(buf, "KILLROOM: Air room not touching a property: %d.", pRoomIndex->vnum);
                  log_string(buf);

                  kill_room(pRoomIndex);
                  continue;
                }
              }
            }
          }
          // Kill air rooms with nothing but air around them.
          if (pRoomIndex->area->vnum >= HAVEN_TOWN_VNUM && pRoomIndex->area->vnum <= HELL_FOREST_VNUM && pRoomIndex->sector_type == SECT_AIR && pRoomIndex->z > 1) {
            if ((pRoomIndex->exit[DIR_NORTH] == NULL || pRoomIndex->exit[DIR_NORTH]->u1.to_room == NULL || pRoomIndex->exit[DIR_NORTH]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_NORTHEAST] == NULL || pRoomIndex->exit[DIR_NORTHEAST]->u1.to_room == NULL || pRoomIndex->exit[DIR_NORTHEAST]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_EAST] == NULL || pRoomIndex->exit[DIR_EAST]->u1.to_room == NULL || pRoomIndex->exit[DIR_EAST]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_SOUTHEAST] == NULL || pRoomIndex->exit[DIR_SOUTHEAST]->u1.to_room == NULL || pRoomIndex->exit[DIR_SOUTHEAST]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_SOUTH] == NULL || pRoomIndex->exit[DIR_SOUTH]->u1.to_room == NULL || pRoomIndex->exit[DIR_SOUTH]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_SOUTHWEST] == NULL || pRoomIndex->exit[DIR_SOUTHWEST]->u1.to_room == NULL || pRoomIndex->exit[DIR_SOUTHWEST]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_WEST] == NULL || pRoomIndex->exit[DIR_WEST]->u1.to_room == NULL || pRoomIndex->exit[DIR_WEST]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_NORTHWEST] == NULL || pRoomIndex->exit[DIR_NORTHWEST]->u1.to_room == NULL || pRoomIndex->exit[DIR_NORTHWEST]->u1.to_room->sector_type ==
                  SECT_AIR) && (pRoomIndex->exit[DIR_UP] == NULL || pRoomIndex->exit[DIR_UP]->u1.to_room == NULL || pRoomIndex->exit[DIR_UP]->u1.to_room->sector_type == SECT_AIR) && (pRoomIndex->exit[DIR_DOWN] == NULL || pRoomIndex->exit[DIR_DOWN]->u1.to_room == NULL || pRoomIndex->exit[DIR_DOWN]->u1.to_room->sector_type ==
                  SECT_AIR)) {
              sprintf(buf, "KILLROOM: Air room not touching nonair: %d.", pRoomIndex->vnum);
              log_string(buf);

              kill_room(pRoomIndex);
              continue;
            }
          }

          // Saving starts here
          fprintf(fp, "#%d\n", pRoomIndex->vnum);
          fprintf(fp, "%s~\n", pRoomIndex->name);
          fprintf(fp, "%s~\n", fix_string(pRoomIndex->description));
          fprintf(fp, "0 ");
          fprintf(fp, "%d ", pRoomIndex->room_flags);
          fprintf(fp, "%d\n", pRoomIndex->sector_type);

          int x = 0;
          for (pEd = pRoomIndex->extra_descr; pEd && x < 20; pEd = pEd->next) {
            char tmp[MSL];
            sprintf(tmp, "temporary1000temp");
            if (safe_strlen(pEd->keyword) > 1 && str_cmp(pEd->keyword, "!sleepers") && str_cmp(pEd->keyword, tmp)) {
              sprintf(tmp, "%s", pEd->keyword);
              fprintf(fp, "E\n%s~\n%s~\n", pEd->keyword, fix_string(pEd->description));
            }
            x++;
          }
          x = 0;
          for (pEd = pRoomIndex->places; pEd && x < 10; pEd = pEd->next) {
            char tmp[MSL];
            sprintf(tmp, "temporary1000temp");
            if (safe_strlen(pEd->keyword) > 1 && str_cmp(pEd->keyword, tmp)) {
              sprintf(tmp, "%s", pEd->keyword);
              fprintf(fp, "P\n%s~\n%s~\n", pEd->keyword, fix_string(pEd->description));
            }
            x++;
          }

          fprintf(fp, "G %d %d %d %d %d\n", pRoomIndex->locx, pRoomIndex->locy, pRoomIndex->size, pRoomIndex->entryx, pRoomIndex->entryy);
          fprintf(fp, "C %d %d %d\n", pRoomIndex->x, pRoomIndex->y, pRoomIndex->z);
          fprintf(fp, "T %d\n", pRoomIndex->time);
          if (prop_from_room(pRoomIndex) != NULL)
          fprintf(fp, "A %d %d %d %d\n", pRoomIndex->encroachment, pRoomIndex->security, pRoomIndex->toughness, pRoomIndex->decor);

          for (door = 0; door < 10; door++) {
            if (pRoomIndex->features != NULL && pRoomIndex->features[door] != NULL && pRoomIndex->features[door]->type > 0)
            fprintf(fp, "F %d %d\n%s~\n%s~\n%s~\n", pRoomIndex->features[door]->type, pRoomIndex->features[door]->position, pRoomIndex->features[door]->names, pRoomIndex->features[door]->shortdesc, pRoomIndex->features[door]->desc);
          }

          for (door = 0; door < MAX_DIR; door++) /* I hate this! */
          {
            if ((pExit = pRoomIndex->exit[door]) && pExit->u1.to_room && pExit->u1.to_room->vnum > 0) {
              int locks = 0;

              /* HACK : TO PREVENT EX_LOCKED etc without EX_ISDOOR
to stop booting the mud */

              if (IS_SET(pExit->rs_flags, EX_CLOSED) || IS_SET(pExit->rs_flags, EX_LOCKED) || IS_SET(pExit->rs_flags, EX_NOPASS) || IS_SET(pExit->rs_flags, EX_SEETHRU) || IS_SET(pExit->rs_flags, EX_HARD) || IS_SET(pExit->rs_flags, EX_NOCLOSE) || IS_SET(pExit->rs_flags, EX_NOLOCK))
              SET_BIT(pExit->rs_flags, EX_ISDOOR);

              /* THIS SUCKS but it's backwards compatible */
              /* NOTE THAT EX_NOCLOSE NOLOCK etc aren't being saved */
              if (IS_SET(pExit->rs_flags, EX_ISDOOR) && (!IS_SET(pExit->rs_flags, EX_CURTAINS)) && (!IS_SET(pExit->rs_flags, EX_NOPASS)))
              locks = 1;
              if (IS_SET(pExit->rs_flags, EX_ISDOOR) && (IS_SET(pExit->rs_flags, EX_CURTAINS)) && (!IS_SET(pExit->rs_flags, EX_NOPASS)))
              locks = 2;
              if (IS_SET(pExit->rs_flags, EX_ISDOOR) && (!IS_SET(pExit->rs_flags, EX_CURTAINS)) && (IS_SET(pExit->rs_flags, EX_NOPASS)))
              locks = 3;
              if (IS_SET(pExit->rs_flags, EX_ISDOOR) && (IS_SET(pExit->rs_flags, EX_CURTAINS)) && (IS_SET(pExit->rs_flags, EX_NOPASS)))
              locks = 4;

              if (IS_SET(pExit->rs_flags, EX_HIDDEN))
              locks = locks + 5;

              if (!IS_SET(pExit->rs_flags, EX_ISDOOR)) {

                if (IS_SET(pExit->rs_flags, EX_HIDDEN) && (IS_SET(pExit->rs_flags, EX_CURTAINS)))
                locks = 11;
                else if (IS_SET(pExit->rs_flags, EX_CURTAINS))
                locks = 10;
              }

              fprintf(fp, "D%d\n", pExit->orig_door);

              fprintf(fp, "%s~\n", fix_string(pExit->description));
              fprintf(fp, "%s~\n", pExit->keyword);

              fprintf(fp, "%d %d %d\n", locks, pExit->key, pExit->u1.to_room->vnum);

              if (IS_SET(pExit->rs_flags, EX_HELLGATE))
              fprintf(fp, "%d %d %d %d %d %d %d %d %d\n", pExit->jump, pExit->climb, pExit->fall, pExit->wall, pExit->wallcondition, pExit->doorbroken, 0, 0, 0);
              else
              fprintf(fp, "%d %d %d %d %d %d %d %d %d\n", pExit->jump, pExit->climb, pExit->fall, pExit->wall, 0, 0, 0, 0, 0);
            }
          }

          fprintf(fp, "M %d H %d\n", pRoomIndex->timezone, pRoomIndex->level);

          fprintf(fp, "W %s~\n", fix_string(pRoomIndex->shroud));
          fprintf(fp, "Y %s~\n", pRoomIndex->subarea); // Discordance
          // fprintf( fp, "X %s~\n", fix_string(pRoomIndex->player_description )
          // ); fprintf( fp, "Z %s~\n", fix_string(pRoomIndex->player_shroud ) );

          if (!IS_NULLSTR(pRoomIndex->owner))
          fprintf(fp, "O %s~\n", pRoomIndex->owner);

          fprintf(fp, "S\n");
        }
      }
    }
    fprintf(fp, "#0\n\n\n\n");
    return;
  }

  /*
* This function is obsolete.  It it not needed but has been left here
* for historical reasons.  It is used currently for the same reason.
*
* I don't think it's obsolete in ROM -- Hugin.
*/
  void save_door_resets(FILE *fp, AREA_DATA *pArea) {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pExit;
    int door;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == pArea) {
          for (door = 0; door < MAX_DIR; door++) {
            if ((pExit = pRoomIndex->exit[door]) && pExit->u1.to_room && (IS_SET(pExit->rs_flags, EX_CLOSED) || IS_SET(pExit->rs_flags, EX_LOCKED)))
#if defined(VERBOSE)
            fprintf(fp, "D 0 %d %d %d The %s door of %s is %s\n", pRoomIndex->vnum, pExit->orig_door, IS_SET(pExit->rs_flags, EX_LOCKED) ? 2 : 1, dir_name[pExit->orig_door][0], pRoomIndex->name, IS_SET(pExit->rs_flags, EX_LOCKED) ? "closed and locked" : "closed");
#endif
#if !defined(VERBOSE)
            fprintf(fp, "D 0 %d %d %d\n", pRoomIndex->vnum, pExit->orig_door, IS_SET(pExit->rs_flags, EX_LOCKED) ? 2 : 1);
#endif
          }
        }
      }
    }
    return;
  }

  /*****************************************************************************
Name:		save_resets
Purpose:	Saves the #RESETS section of an area file.
Called by:	save_area(olc_save.c)
****************************************************************************/
  void save_resets(FILE *fp, AREA_DATA *pArea) {
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    ROOM_INDEX_DATA *pRoom;
    char buf[MAX_STRING_LENGTH];
    int iHash;

    fprintf(fp, "#RESETS\n");

    save_door_resets(fp, pArea);

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next) {
        if (pRoom->area == pArea) {
          for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
            switch (pReset->command) {
            default:
              bug("Save_resets: bad command %c.", pReset->command);
              break;

#if defined(VERBOSE)

            case 'M':
              pLastMob = get_mob_index(pReset->arg1);
              fprintf(fp, "M 0 %d %d %d %d Load %s\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4, pLastMob->short_descr);
              break;
            case 'Q':
              fprintf(fp, "Q 0 %d %d %d %d loaded to %s\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4, pRoom->name);
              break;
            case 'O':
              OBJ_INDEX_DATA *pLastObj = get_obj_index(pReset->arg1);
              pRoom = get_room_index(pReset->arg3);
              fprintf(fp, "O 0 %d 0 %d %s loaded to %s\n", pReset->arg1, pReset->arg3, pLastObj->short_descr, capitalize(pLastObj->short_descr), pRoom->name);
              break;

            case 'P':
              OBJ_INDEX_DATA *pLastObj = get_obj_index(pReset->arg1);
              fprintf(fp, "P 0 %d %d %d %d %s put inside %s\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4, capitalize(get_obj_index(pReset->arg1)->short_descr), pLastObj->short_descr);
              break;

            case 'G':
              fprintf(fp, "G 0 %d 0 %s is given to %s\n", pReset->arg1, capitalize(get_obj_index(pReset->arg1)->short_descr), pLastMob ? pLastMob->short_descr : "!NO_MOB!");
              if (!pLastMob) {
                sprintf(buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name);
                bug(buf, 0);
              }
              break;

            case 'E':
              fprintf(fp, "E 0 %d 0 %d %s is loaded %s of %s\n", pReset->arg1, pReset->arg3, capitalize(get_obj_index(pReset->arg1)->short_descr), flag_string(wear_loc_strings, pReset->arg3), pLastMob ? pLastMob->short_descr : "!NO_MOB!");
              if (!pLastMob) {
                sprintf(buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name);
                bug(buf, 0);
              }
              break;

            case 'D':
              break;

            case 'R':
              pRoom = get_room_index(pReset->arg1);
              fprintf(fp, "R 0 %d %d Randomize %s\n", pReset->arg1, pReset->arg2, pRoom->name);
              break;
            }
#endif
#if !defined(VERBOSE)
          case 'M':
            pLastMob = get_mob_index(pReset->arg1);
            fprintf(fp, "M 0 %d %d %d %d\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4);
            break;
          case 'Q':
            fprintf(fp, "Q 0 %d %d %d %d loaded to %s\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4, pRoom->name);
            break;
          case 'O':
            pRoom = get_room_index(pReset->arg3);
            fprintf(fp, "O 0 %d 0 %d\n", pReset->arg1, pReset->arg3);
            break;

          case 'P':
            fprintf(fp, "P 0 %d %d %d %d\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4);
            break;

          case 'G':
            fprintf(fp, "G 0 %d 0\n", pReset->arg1);
            if (!pLastMob) {
              sprintf(buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name);
              bug(buf, 0);
            }
            break;

          case 'E':
            fprintf(fp, "E 0 %d 0 %d\n", pReset->arg1, pReset->arg3);
            if (!pLastMob) {
              sprintf(buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name);
              bug(buf, 0);
            }
            break;

          case 'D':
            break;

          case 'R':
            pRoom = get_room_index(pReset->arg1);
            fprintf(fp, "R 0 %d %d\n", pReset->arg1, pReset->arg2);
            break;
          }
#endif
        }
      } /* End if correct area */
    }   /* End for pRoom */
  } /* End for iHash */
  fprintf(fp, "S\n\n\n\n");
  return;
}

/*****************************************************************************
Name:		save_shops
Purpose:	Saves the #SHOPS section of an area file.
Called by:	save_area(olc_save.c)
****************************************************************************/
void save_shops(FILE *fp, AREA_DATA *pArea) {
  SHOP_DATA *pShopIndex;
  MOB_INDEX_DATA *pMobIndex;
  int iTrade;
  int iHash;

  fprintf(fp, "#SHOPS\n");

  for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
    for (pMobIndex = mob_index_hash[iHash]; pMobIndex;
    pMobIndex = pMobIndex->next) {
      if (pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop) {
        pShopIndex = pMobIndex->pShop;

        fprintf(fp, "%d ", pShopIndex->keeper);
        for (iTrade = 0; iTrade < MAX_TRADE; iTrade++) {
          if (pShopIndex->buy_type[iTrade] != 0) {
            fprintf(fp, "%d ", pShopIndex->buy_type[iTrade]);
          }
          else
          fprintf(fp, "0 ");
        }
        fprintf(fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell);
        fprintf(fp, "%d %d", pShopIndex->open_hour, pShopIndex->close_hour);
        fprintf(fp, " %d %d\n", pShopIndex->owner, pShopIndex->proceed);
      }
    }
  }

  fprintf(fp, "0\n\n\n\n");
  return;
}

void save_helps() {
  HELP_DATA *pHelp;
  FILE *fp;

  if (!(fp = fopen("help.are", "w"))) {
    bug("Open_help: fopen", 0);
    perror("help.are");
  }

  fprintf(fp, "#HELPS\n");

  for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
    if (pHelp->del)
    continue;

    fprintf(fp, "Keyword %s~\n", pHelp->keyword);
    if (pHelp->see_also[0] == ',')
    fprintf(fp, "SeeAlso ~\n");
    else
    fprintf(fp, "SeeAlso %s~\n", pHelp->see_also);
    fprintf(fp, "Level %d\n", pHelp->level);
    fprintf(fp, "Online %d\n", pHelp->online);
    fprintf(fp, "Type %d\n", pHelp->type);
    fprintf(fp, "Text %s~\n", fix_string(pHelp->text));
    fprintf(fp, "End\n\n");
  }

  fprintf(fp, "#END\n\n#$\n");
  fclose(fp);
  return;
}

/*****************************************************************************
Name:		save_area
Purpose:	Save an area, note that this format is new.
Called by:	do_asave(olc_save.c).
****************************************************************************/
void save_area(AREA_DATA *pArea, bool backup) {
  FILE *fp;

  if (backup == FALSE) {
    if (!(fp = fopen(pArea->file_name, "w"))) {
      bug("Open_area: fopen", 0);
      perror(pArea->file_name);
    }
  }
  else {
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "back1/%s", pArea->file_name);
    else if (time_info.day % 6 == 0)
    sprintf(buf, "back2/%s", pArea->file_name);
    else if (time_info.day % 5 == 0)
    sprintf(buf, "back3/%s", pArea->file_name);
    else if (time_info.day % 4 == 0)
    sprintf(buf, "back4/%s", pArea->file_name);
    else if (time_info.day % 3 == 0)
    sprintf(buf, "back5/%s", pArea->file_name);
    else if (time_info.day % 2 == 0)
    sprintf(buf, "back6/%s", pArea->file_name);
    else
    sprintf(buf, "back7/%s", pArea->file_name);

    if (!(fp = fopen(buf, "w"))) {
      bug("Open_area: fopen", 0);
      perror(pArea->file_name);
    }
  }
  if (fp == NULL)
  return;

  fprintf(fp, "#AREADATA\n");
  fprintf(fp, "Name %s~\n", pArea->name);
  fprintf(fp, "VNUMs %d %d\n", pArea->min_vnum, pArea->max_vnum);
  fprintf(fp, "Completed %d\n", pArea->area_completed);
  fprintf(fp, "World %d\n", pArea->world);
  fprintf(fp, "Range %d %d %d %d\n", pArea->minx, pArea->miny, pArea->maxx, pArea->maxy);
  fprintf(fp, "End\n\n\n\n");

  save_mobiles(fp, pArea);
  save_objects(fp, pArea);
  save_rooms(fp, pArea);
  save_resets(fp, pArea);
  save_shops(fp, pArea);
  //    save_mobprogs( fp, pArea );
  //    save_groups( fp, pArea );
  //    save_objprogs( fp, pArea );
  //    save_roomprogs( fp, pArea );

  if (pArea->helps && pArea->helps->first)
  save_helps();

  fprintf(fp, "#$\n");

  fclose(fp);

  if (backup == FALSE) {
    save_area(pArea, TRUE);
  }
  return;
}

/*****************************************************************************
Name:		do_asave
Purpose:	Entry point for saving area data.
Called by:	interpreter(interp.c)
****************************************************************************/
_DOFUN(do_asave) {
  char arg1[MAX_INPUT_LENGTH];
  AREA_DATA *pArea;
  int value;
  AreaList::iterator it;

  smash_tilde(argument);
  strcpy(arg1, argument);

  if (arg1[0] == '\0') {
    if (ch) {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  asave <vnum>   - saves a particular area\n\r", ch);
      send_to_char("  asave list     - saves the area.lst file\n\r", ch);
      send_to_char("  asave area     - saves the area being edited\n\r", ch);
      send_to_char("  asave changed  - saves all changed zones\n\r", ch);
      send_to_char("  asave world    - saves the world! (db dump)\n\r", ch);
      send_to_char("  asave helps	- saves the help files\n\r", ch);
      send_to_char("\n\r", ch);
    }

    return;
  }

  /* Snarf the value (which need not be numeric). */
  value = atoi(arg1);
  if (!(pArea = get_area_data(value)) && is_number(arg1)) {
    if (ch)
    send_to_char("That area does not exist.\n\r", ch);
    return;
  }

  /* Save area of given vnum. */
  /* ------------------------ */
  if (is_number(arg1)) {
    if (ch && !check_security(ch, pArea)) {
      send_to_char("You are not a builder for this area.\n\r", ch);
      return;
    }

    save_area_list();
    save_area(pArea, FALSE);
    return;
  }

  /* Save the world, only authorized areas. */
  /* -------------------------------------- */
  if (!str_cmp("world", arg1)) {
    if (ch->level < 108) {
      send_to_char("We aren't doing that atm.", ch);
      return;
    }

    save_area_list();
    for (it = area_list.begin(); it != area_list.end(); ++it) {
      /* Builder must be assigned this area. */
      if (ch && !check_security(ch, *it) && !IS_ADMIN(ch))
      continue;

      save_area(*it, FALSE);
      REMOVE_BIT((*it)->area_flags, AREA_CHANGED);
    }

    if (ch)
    send_to_char("You saved the world.\n\r", ch);

    // save_other_helps( NULL );

    return;
  }

  /* Save changed areas, only authorized areas. */
  /* ------------------------------------------ */
  if ((!str_cmp("changed", arg1)) || (!str_cmp("auto", arg1))) {
    char buf[MAX_INPUT_LENGTH];

    save_area_list();

    if ((ch) && (!str_cmp("changed", arg1)))
    send_to_char("Saved zones:\n\r", ch);
    else
    log_string("Saved zones:");

    sprintf(buf, "None.\n\r");

    for (it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;

      /* Builder must be assigned this area. */
      if (ch && !check_security(ch, pArea))
      continue;

      /* Save changed areas. */
      if (IS_SET(pArea->area_flags, AREA_CHANGED)) {
        save_area(pArea, FALSE);
        sprintf(buf, "%24s - '%s'", pArea->name, pArea->file_name);
        if ((ch) && (!str_cmp(arg1, "changed"))) {
          send_to_char(buf, ch);
          send_to_char("\n\r", ch);
        }
        else
        log_string(buf);
        REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
      }
    }

    // save_other_helps( ch );

    if (!str_cmp(buf, "None.\n\r")) {
      if ((ch) && (!str_cmp(arg1, "changed")))
      send_to_char(buf, ch);
      else
      log_string("None.");
    }
    return;
  }

  /* Save the area.lst file. */
  /* ----------------------- */
  if (!str_cmp(arg1, "list")) {
    save_area_list();
    return;
  }

  /* Save area being edited, if authorized. */
  /* -------------------------------------- */
  if (!str_cmp(arg1, "area")) {
    if (!ch || !ch->desc)
    return;

    /* Is character currently editing. */
    if (ch->desc->editor == ED_NONE) {
      send_to_char("You are not editing an area, therefore an area vnum is required.\n\r", ch);
      return;
    }

    /* Find the area to save. */
    switch (ch->desc->editor) {
    case ED_AREA:
      pArea = (AREA_DATA *)ch->desc->pEdit;
      break;
    case ED_ROOM:
      pArea = ch->in_room->area;
      break;
    case ED_OBJECT:
      pArea = ((OBJ_INDEX_DATA *)ch->desc->pEdit)->area;
      break;
    case ED_MOBILE:
      pArea = ((MOB_INDEX_DATA *)ch->desc->pEdit)->area;
      break;
    case ED_HELP:
      send_to_char("Grabando area : ", ch);
      // save_other_helps( ch );
      return;
    default:
      pArea = ch->in_room->area;
      break;
    }

    if (!check_security(ch, pArea)) {
      send_to_char("You are not a builder for this area.\n\r", ch);
      return;
    }

    save_area_list();
    save_area(pArea, FALSE);
    REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
    send_to_char("Area saved.\n\r", ch);
    return;
  }

  if (!str_cmp("helps", arg1)) {
    save_helps();
    send_to_char("Helps Saved.\n\r", ch);
    return;
  }

  if (!str_cmp("corpses", arg1)) {
    save_corpses();
    send_to_char("Corpses Saved.\n\r", ch);
    return;
  }

  /* Show correct syntax. */
  /* -------------------- */
  if (ch)
  do_asave(ch, "");

  return;
}

_DOFUN(do_savehelps) {
  save_helps();
  send_to_char("Helps Saved.\n\r", ch);
  return;
}
// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

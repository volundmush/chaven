#if defined (_WIN32)
#	if defined (_DEBUG)
#		pragma warning (disable : 4786)
#	endif
#endif

#ifndef WIN32
#include <sys/stat.h>
#endif


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <map>
#include "merc.h"
#include "olc.h"
#include "gsn.h"
#include "recycle.h"
#include "lookup.h"
#include "global.h"
#include <math.h>
#include <dirent.h>

#if defined(__cplusplus)
extern "C" {
#endif

  char *const where_name[] = {
    "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "worn   ", "held   ", "held   ", };


  /* for  keeping track of the player count */
  int max_on = 0;
  char *last_on_names[10];
  char *last_on_times[25];
  /*
* Local functions.
*/
  char *format_obj_to_char args((OBJ_DATA * obj, CHAR_DATA *ch, bool fShort));
  void show_list_to_char args((OBJ_DATA * list, CHAR_DATA *ch, bool fShort, bool fShowNothing, char *arg));
  void show_char_to_char_0 args((CHAR_DATA * victim, CHAR_DATA *ch, int range));
  void show_char_to_char args((CharList * list, CHAR_DATA *ch, int moving));
  bool check_blind args((CHAR_DATA * ch));
  bool check_for_color args((char *word));
  bool is_guildleader args((CHAR_DATA * ch));
  bool does_cover args((OBJ_DATA * obj, int selection));
  bool does_conceal args((OBJ_DATA * obj_over, OBJ_DATA *obj_under));
  char *get_focused args((CHAR_DATA * ch, CHAR_DATA *victim, bool xray));
  int get_bust_mod args((CHAR_DATA * ch));
  bool is_whoinvis args((CHAR_DATA * ch));
  bool has_shapewear_boobs args((CHAR_DATA * ch));
  bool has_shapewear_height args((CHAR_DATA * ch));
  char *draw_horizontal_line args((int span));
  char *animal_skin args((CHAR_DATA * ch));
  bool show_char_location_to_char args((CHAR_DATA * ch, CHAR_DATA *victim, char arg1[MSL]));
  void show_multilocation args((CHAR_DATA * ch, CHAR_DATA *victim, int location));
  char *solechar args((CHAR_DATA * ch));
  bool torso_exposed args((CHAR_DATA * ch));
  int get_redesclocation args((char *arg1));
  void append_focused args((CHAR_DATA * ch, int number, char *replacement));
  void replace_focused args((CHAR_DATA * ch, int number, char *replacement));
  char *standing args((CHAR_DATA * ch, int number));
  char *lower_standing args((CHAR_DATA * ch, int number));
  char *roomtitle args((ROOM_INDEX_DATA * room, bool capital));
  int attract_lifeforce args((CHAR_DATA * ch));

  /* Formats on obj to display to char*/
  char *format_obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch, bool fShort) {
    static char buf[MAX_STRING_LENGTH];
    char *pdesc;
    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0')) || (obj->description == NULL || obj->description[0] == '\0'))
    return buf;

    // Commented out for spacing and new styling - Discordance
    // if ( IS_OBJ_STAT(obj, ITEM_HAND) )
    //	strcat( buf, "(`rHand`x) ");
    // if ( IS_OBJ_STAT(obj, ITEM_ORDER) )
    //	strcat( buf, "(`rOrder`x) ");
    // if ( IS_OBJ_STAT(obj, ITEM_TEMPLE) )
    //	strcat( buf, "(`rTemple`x) ");
    // if ( IS_OBJ_STAT(obj, ITEM_INVIS) )
    // strcat( buf, "(`mInvis`x) ");
    // if ( IS_OBJ_STAT(obj, ITEM_HIDDEN)  )
    // strcat( buf, "(`mHidden`x) ");

    pdesc = get_extra_descr_obj("all", obj->extra_descr, obj);

    strcpy(buf, " ");

    if (obj->faction == -1) {
      strcat(buf, "(`GB`x) ");
    }
    else if (obj->faction == -2 && get_skill(ch, SKILL_DEMONOLOGY) >= 1) {
      strcat(buf, "(`cK`x) ");
    }
    else if (obj->faction > 0) {
      strcat(buf, "(`cS`x) ");
    }
    else if (IS_SET(obj->extra_flags, ITEM_ARMORED)) {
      strcat(buf, "`x(`cA`x) ");
    }
    else if (pdesc != NULL && safe_strlen(pdesc) > 5) {
      strcat(buf, "`x(`cD`x) ");
    }
    else if (obj->item_type == ITEM_CONTAINER) {
      strcat(buf, "`x(`cC`x) ");
    }
    else if (obj->cost >= 10000 && obj->item_type != ITEM_JEWELRY) {
      strcat(buf, "`x(`cE`x) ");
    }
    else if (obj->cost >= 25000) {
      strcat(buf, "`x(`cE`x) ");
    }
    else {
      strcat(buf, "`x    ");
    }

    if (!fShort && obj->item_type == ITEM_CLOTHING) {

      if (obj->condition <= 5)
      strcat(buf, "`x(`yDisgusting`x) ");
      else if (obj->condition <= 25)
      strcat(buf, "`x(`yFilthy`x) ");
      else if (obj->condition < 50)
      strcat(buf, "`x(Dirty) ");

      if (obj->value[4] > 10)
      strcat(buf, "`x(`BWet`x) ");
      else if (obj->value[4] > 0)
      strcat(buf, "`x(`BDamp`x) ");

      if (obj->value[3] > 250)
      strcat(buf, "`x(`rBlood`x drenched) ");
      else if (obj->value[3] > 100)
      strcat(buf, "`x(`rBlood`x spattered) ");
      else if (obj->value[3] > 30)
      strcat(buf, "`x(`RBlood`x speckled) ");
    }

    if (!fShort && (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_RANGED)) {
      if (obj->value[3] > 250)
      strcat(buf, "`x(`RD`rr`Ri`rp`Rp`ri`Rn`rg `Rb`rl`Ro`ro`Rd`x) ");
      else if (obj->value[3] > 100)
      strcat(buf, "`x(`rBlood`x spattered) ");
      else if (obj->value[3] > 30)
      strcat(buf, "`x(`RBlood`x speckled) ");
    }
    if (!fShort && obj->item_type == ITEM_JEWELRY) {
      if (obj->value[4] > 250)
      strcat(buf, "`x(`rBlood`x drenched) ");
      else if (obj->value[4] > 100)
      strcat(buf, "`x(`rBlood`x spattered) ");
      else if (obj->value[4] > 30)
      strcat(buf, "`x(`RBlood`x speckled) ");
    }

    if (ch->on != NULL && ch->on == obj)
    strcat(buf, "`x(`WOn`x) ");

    if (fShort) {
      if (obj->short_descr != NULL)
      strcat(buf, lowercase_clause(obj->short_descr));
    }
    else {
      if (obj->adjust_string != NULL && safe_strlen(obj->adjust_string) > 2)
      strcat(buf, lowercase_clause(obj->adjust_string));
      else {
        if (obj->description != NULL)
        strcat(buf, lowercase_clause(obj->description));
      }
    }

    // Faction symbols
    // if ( IS_OBJ_STAT(obj, ITEM_HAND) )
    //	strcat( buf, " emblazoned with a fist");
    // if ( IS_OBJ_STAT(obj, ITEM_ORDER) )
    //	strcat( buf, " emblazoned with a sword and scroll");
    // if ( IS_OBJ_STAT(obj, ITEM_TEMPLE) )
    //	strcat( buf, " emblazoned with a sun");

    return buf;
  }

  /*
* Show a list to a character.
* Can coalesce duplicated items.
*/
  void show_list_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing, char *arg) {
    char buf[MAX_STRING_LENGTH];
    Buffer output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if (ch->desc == NULL)
    return;

    if (ch == NULL || list == NULL)
    return;

    if (list->next_content == list) {
      send_to_char(format_obj_to_char(list, ch, FALSE), ch);
      return;
    }

    count = 0;
    for (obj = list; obj != NULL; obj = obj->next_content)
    count++;

    prgpstrShow = (char **)alloc_mem(count * sizeof(char *));
    prgnShow = (int *)alloc_mem(count * sizeof(int));
    nShow = 0;

    /*
    * Format the list of objects.
    */
    for (obj = list; obj != NULL; obj = obj->next_content) {
      if (obj->carried_by != NULL && obj->carried_by == ch && (is_big(obj)))
      continue;

      if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && (arg == NULL || (arg != NULL && is_name(arg, obj->name)))) {
        pstrShow = format_obj_to_char(obj, ch, FALSE);

        fCombine = FALSE;

        if (IS_NPC(ch) || IS_FLAG(ch->comm, COMM_COMBINE)) {
          /*
          * Look for duplicates, case sensitive.
          * Matches tend to be near end so run loop backwords.
          */
          for (iShow = nShow - 1; iShow >= 0; iShow--) {
            if (!strcmp(prgpstrShow[iShow], pstrShow)) {
              prgnShow[iShow]++;
              fCombine = TRUE;
              break;
            }
          }
        }

        /*
        * Couldn't combine, or didn't want to.
        */
        if (!fCombine) {
          prgpstrShow[nShow] = str_dup(pstrShow);
          prgnShow[nShow] = 1;
          nShow++;
        }
      }
    }

    /*
    * Output the formatted list.
    */
    for (iShow = 0; iShow < nShow; iShow++) {
      if (prgpstrShow[iShow][0] == '\0') {
        free_string(prgpstrShow[iShow]);
        continue;
      }

      if (IS_NPC(ch) || IS_FLAG(ch->comm, COMM_COMBINE)) {
        if (prgnShow[iShow] != 1) {
          sprintf(buf, "(%2d) ", prgnShow[iShow]);
          output.strcat(buf);
        }
        else {
          sprintf(buf, "%s", "      ");
          output.strcat(buf);
        }
      }
      output.strcat(prgpstrShow[iShow]);
      output.strcat("\n\r");
      free_string(prgpstrShow[iShow]);
    }

    if (fShowNothing && nShow == 0) {
      if (IS_NPC(ch) || IS_FLAG(ch->comm, COMM_COMBINE))
      send_to_char("     ", ch);
      send_to_char("Nothing.\n\r", ch);
    }

    page_to_char(output, ch);

    /*
    * Clean up.
    */
    free_mem(prgpstrShow, count * sizeof(char *));
    free_mem(prgnShow, count * sizeof(int));
    // log_string("MEMCHECK: Show List.");
    return;
  }

  char *const consider_colours[] = {"`D", "`x", "`G", "`g", "`x", "`c", "`C"};

  char *distant_string(CHAR_DATA *ch) {
    if (ch->in_room != NULL) {
      if (ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_UNDERWATER || ch->in_room->sector_type == SECT_WATER || mist_level(ch->in_room) >= 2) {
        if (!is_animal(ch)) {
          if (mist_level(ch->in_room) >= 2)
          return "A shape in the mist";
          else if (ch->in_room->sector_type == SECT_FOREST)
          return "A shape glimpsed between the trees";
          else if (ch->in_room->sector_type == SECT_WATER || ch->in_room->sector_type == SECT_UNDERWATER)
          return "A shape in the water";
          return "A shape";
        }
        else {
          if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <= ANIMAL_SMALL)
          return "A small animal";
          else if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >=
              ANIMAL_MANSIZED)
          return "A large animal";
          else
          return "An animal";
        }
      }
    }
    if (IS_NPC(ch)) {
      if (ch->pIndexData->vnum == 160)
      return "A car";
      else if (ch->pIndexData->vnum == 161)
      return "A motorcycle";
      else if (ch->pIndexData->vnum == 162)
      return "A horse";
    }
    if (is_animal(ch)) {
      if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <= ANIMAL_TINY)
      return "A tiny animal";
      else if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <= ANIMAL_SMALL)
      return "A small animal";
      else if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >= ANIMAL_LARGE)
      return "A very large animal";
      else if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >=
          ANIMAL_MANSIZED)
      return "A large animal";
      else
      return "An animal";
    }

    return "A figure";
  }

  bool is_mover(CHAR_DATA *ch) {
    if (ch->recent_moved > 0)
    return TRUE;
    return FALSE;
  }

  // function for outputting distance and intro prefix affect tags
  // created to avoid duplication - Disco 9/17/2018
  char *distance_header(CHAR_DATA *ch, CHAR_DATA *victim, int range) {
    char buf[MAX_STRING_LENGTH];
    bool dreamviewer = FALSE;
    if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
    dreamviewer = TRUE;

    strcpy(buf, "");

    if (ch->in_room != victim->in_room && !dreamviewer) {
      if (in_world(ch) == in_world(victim) && in_haven(ch->in_room) == in_haven(victim->in_room)) {
        strcat(buf, "`G[");
        strcat(buf, room_distance(ch, victim->in_room));
        strcat(buf, "`G]`x ");
      }
    }
    if (range == DISTANCE_FAR) {
      strcat(buf, distant_string(victim));
      if (in_fight(victim) && victim->cfighting != NULL && same_fight(victim, victim->cfighting)) {
        if (get_dist(victim->x, victim->y, victim->cfighting->x, victim->cfighting->y) <= 5)
        strcat(buf, " is fighting ");
        else
        strcat(buf, " is shooting at ");
        strcat(buf, PERS(victim->cfighting, ch));
      }
      else if (is_mover(victim)) {
        strcat(buf, " moving ");
        strcat(buf, relwalking[get_reldirection(victim->facing, ch->facing)]);
      }
      else {
        strcat(buf, " ");
        strcat(buf, relfacing[get_reldirection(victim->facing, ch->facing)]);
      }
      strcat(buf, ".\n\r");
      buf[0] = UPPER(buf[0]);
      return strdup(buf);
    }

    if (IS_FLAG(victim->comm, COMM_AFK))
    strcat(buf, "[`RAFK`x] ");
    if (!IS_NPC(victim) && victim->desc == NULL)
    strcat(buf, "[`YLinkdead`x] ");
    else if (!is_dreaming(victim) && is_helpless(victim))
    strcat(buf, "[`YHelpless`x] ");
    if (IS_FLAG(victim->act, PLR_SHROUD))
    strcat(buf, "(`DNightmare`x) ");
    if (!IS_NPC(victim) && is_helpless(victim) && victim->pcdata->victimize_difficult_time > current_time)
    strcat(buf, "(`rDifficult Prisoner`x) ");
    if (IS_FLAG(victim->comm, COMM_RUNNING))
    strcat(buf, "(`RStory Running`x)");
    if (victim->invis_level > LEVEL_HERO && IS_FLAG(victim->act, PLR_ROOMINVIS))
    strcat(buf, "(`mWizi`x)");
    if (IS_FLAG(victim->act, PLR_HIDE))
    strcat(buf, "(`DHidden`x) ");
    if (!IS_NPC(victim) && victim->pcdata->institute_action == INSTITUTE_EXPERIMENT)
    strcat(buf, "(`YExperimenting`x) ");
    if (!IS_NPC(victim) && victim->pcdata->institute_action == INSTITUTE_TREAT)
    strcat(buf, "(`YTreating`x) ");
    if (!IS_NPC(victim) && victim->pcdata->institute_action == INSTITUTE_TRAUMA)
    strcat(buf, "(`YTraumatizing`x) ");
    if (!IS_NPC(victim) && victim->pcdata->institute_action == INSTITUTE_TEACH)
    strcat(buf, "(`YTeaching`x) ");

    if (safe_strlen(buf) > 2) {
      return strdup(buf);
    }

    return strdup("");
  }

  void show_char_to_char_0(CHAR_DATA *victim, CHAR_DATA *ch, int range) {
    char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH], header[MSL];
    buf[0] = '\0';

    strcpy(header, distance_header(ch, victim, range));
    strcat(buf, header);

    if (range == DISTANCE_FAR) {
      send_to_char(buf, ch);
      return;
    }

    /* Race Flag */
    /*
    if (victim->position == victim->start_pos && victim->long_descr[0] != '\0' )
    {
    strcat( buf, victim->long_descr );
    send_to_char( buf, ch );
    return;
    }
    */
    if (IS_NPC(victim))
    strcat(buf, victim->long_descr);
    else {
      strcat(buf, PERS(victim, ch));
    }
    strcat(buf, battleflags(victim, ch));

    /*
    else if(is_gm(victim))
    {
    strcat(buf, "A lingering, invisible spirit floats here.\n\r");
    send_to_char( buf, ch );
    return;
    }
    else
    strcat( buf, PERS( victim, ch ) );
    */

    if (victim->wounds == 3 && !is_undead(victim) && !in_hospital(victim))
    strcat(buf, " is bleeding to death.");
    else if (victim->wounds > 3)
    strcat(buf, " is dead...");
    else if (IS_FLAG(victim->comm, COMM_HOSTILE))
    strcat(buf, " looks hostile.");
    else if (in_fight(victim) && victim->cfighting != NULL && same_fight(victim, victim->cfighting)) {
      if (get_dist(victim->x, victim->y, victim->cfighting->x, victim->cfighting->y) <= 5)
      strcat(buf, " is fighting ");
      else
      strcat(buf, " is shooting at ");
      strcat(buf, PERS(victim->cfighting, ch));
      strcat(buf, ".");
    }
    else if (!IS_NPC(victim) && victim->pcdata->title[0] != '\0' && safe_strlen(victim->pcdata->title) > 10) {
      strcat(buf, victim->pcdata->title);
    }
    else if (!IS_NPC(victim) && victim->pcdata->sleeping > 0) {
      if (!is_undead(victim) || get_skill(victim, SKILL_OBFUSCATE) > 0)
      strcat(buf, " is asleep.");
      else
      strcat(buf, " is dead.");
    }
    else if (!IS_NPC(victim) && safe_strlen(victim->pcdata->place) > 2) {
      strcat(buf, " is at ");
      strcat(buf, victim->pcdata->place);
    }
    else {
      switch (victim->position) {
      case POS_DEAD:
        strcat(buf, " is DEAD!!");
        break;
      case POS_MORTAL:
        strcat(buf, " is mortally wounded.");
        break;
      case POS_INCAP:
        strcat(buf, " is incapacitated.");
        break;
      case POS_STUNNED:
        strcat(buf, " is lying here stunned.");
        break;
      case POS_SLEEPING:
        if (victim->on != NULL) {
          sprintf(message, " is sleeping %s %s.", IS_SET(victim->on->value[2], SLEEP_AT)
          ? "at" : IS_SET(victim->on->value[2], SLEEP_ON) ? "on" : "in", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        strcat(buf, " is sleeping here.");
        break;
      case POS_RESTING:
        if (victim->on != NULL) {
          sprintf(message, " is sprawled %s %s.", IS_SET(victim->on->value[2], REST_AT)
          ? "at" : IS_SET(victim->on->value[2], REST_ON) ? "on" : "in", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        strcat(buf, " is sprawled here.");
        break;
      case POS_SITTING:
        if (victim->on != NULL) {
          sprintf(message, " is sitting %s %s.", IS_SET(victim->on->value[2], SIT_AT)
          ? "at" : IS_SET(victim->on->value[2], SIT_ON) ? "on" : "in", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        strcat(buf, " is sitting here.");
        break;
      case POS_STANDING:
        if (victim->on != NULL) {
          sprintf(message, " is standing %s %s.", IS_SET(victim->on->value[2], STAND_AT)
          ? "at" : IS_SET(victim->on->value[2], STAND_ON) ? "on" : "in", victim->on->short_descr);
          strcat(buf, message);
        }
        else if (!IS_NPC(victim) && safe_strlen(victim->pcdata->title) > 4)
        strcat(buf, victim->pcdata->title);
        else if (IS_FLAG(victim->act, PLR_BOUNDFEET) && !is_ghost(victim))
        strcat(buf, " is tied up here.");
        else if (!IS_NPC(victim) && victim->pcdata->sleeping > 0) {
          if (!is_undead(victim))
          strcat(buf, " is sleeping here.");
          else
          strcat(buf, "lies dead here.");
        }
        else if (!IS_NPC(victim))
        strcat(buf, " is standing here.");
        break;
      case POS_FIGHTING:
        break;
      }
    }

    if (in_fight(ch)) {
      char temp[MSL];

      if (has_caff(ch, CAFF_BEWILDER)) {
        int randx = number_range(0, 150);
        int randy = number_range(0, 150);
        sprintf(temp, " (X:%d Y:%d D:%d)", randx, randy, get_dist(ch->x, ch->y, randx, randy));
      }
      else if (relative_z(ch, victim->in_room) == 0)
      sprintf(temp, " (X:%d Y:%d D:%d)", relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_distance(ch, victim, FALSE));
      else
      sprintf(temp, " (X:%d Y:%d D:%d(%d))", relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_distance(ch, victim, FALSE), relative_z(ch, victim->in_room));

      strcat(buf, temp);
    }

    strcat(buf, "\n\r");
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    return;
  }

  char *get_exposed_locations(CHAR_DATA *ch) {

    char buf[MSL];
    //    char * word = str_dup(" ");
    char word[MSL];
    char last[MSL];
    bool hasplaced = FALSE;
    bool blast = FALSE;
    int j, ref = 0;
    sprintf(word, " ");
    if (is_ghost(ch)) {
      ch->pcdata->blood[0] = 0;
      ch->pcdata->dirt[0] = 0;
    }

    int bloodtemp = ch->pcdata->blood[0];
    int dirttemp = ch->pcdata->dirt[0];
    ch->pcdata->blood[0] = 0;
    ch->pcdata->dirt[0] = 0;

    if (!is_covered(ch, COVERS_LOWER_FACE)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_LOWER_FACE)
        ref = j;
      }
      if (ch->pcdata->blood[ref] > 100) {
        if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " spattered in blood and very dirty ");
          strcat(word, buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " spattered in blood and dirty ");
          strcat(word, buf);
        }
        else {
          sprintf(buf, " blood spattered ");
          strcat(word, buf);
        }
      }
      else if (ch->pcdata->blood[ref] > 40) {
        if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty and speckled with blood ");
          strcat(word, buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " speckled with blood and dirty ");
          strcat(word, buf);
        }
        else {
          sprintf(buf, " blood speckled ");
          strcat(word, buf);
        }
      }
      else if (ch->pcdata->dirt[ref] > 100) {
        sprintf(buf, " very dirty ");
        strcat(word, buf);
      }
      else if (ch->pcdata->dirt[ref] > 40) {
        sprintf(buf, " slightly dirty ");
        strcat(word, buf);
      }
      sprintf(buf, "face");
      strcat(word, buf);
      hasplaced = TRUE;
      blast = FALSE;
    }
    else {
      sprintf(last, " ");
    }
    if (!is_covered(ch, COVERS_NECK)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_NECK)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "neck");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");

        sprintf(buf, "neck");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_LOWER_CHEST) && !is_covered(ch, COVERS_BREASTS) && !is_covered(ch, COVERS_UPPER_CHEST)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_BREASTS)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "chest");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "chest");
        strcat(last, buf);
        blast = TRUE;
      }
      if (ch->sex == SEX_FEMALE) {
        if (!hasplaced) {
          sprintf(buf, "`168breasts`x");
          strcat(word, buf);
          hasplaced = TRUE;
          blast = FALSE;
        }
        else {
          if (blast) {
            sprintf(buf, ",%s", last);
            strcat(word, buf);
          }
          if (ch->pcdata->blood[ref] > 100) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " spattered in blood and very dirty ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " spattered in blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood spattered ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->blood[ref] > 40) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " very dirty and speckled with blood ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " speckled with blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood speckled ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " slightly dirty ");
            sprintf(last, "%s", buf);
          }
          else
          sprintf(last, " ");
          sprintf(buf, "`168breasts`x");
          strcat(last, buf);
          blast = TRUE;
        }
      }
    }
    else {
      if (!is_covered(ch, COVERS_LOWER_CHEST)) {
        for (j = 0; j < MAX_COVERS; j++) {
          if (cover_table[j] == COVERS_LOWER_CHEST)
          ref = j;
        }
        if (!hasplaced) {
          sprintf(buf, "stomach");
          strcat(word, buf);
          hasplaced = TRUE;
          blast = FALSE;
        }
        else {
          if (blast) {
            sprintf(buf, ",%s", last);
            strcat(word, buf);
          }
          if (ch->pcdata->blood[ref] > 100) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " spattered in blood and very dirty ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " spattered in blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood spattered ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->blood[ref] > 40) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " very dirty and speckled with blood ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " speckled with blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood speckled ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " slightly dirty ");
            sprintf(last, "%s", buf);
          }
          else
          sprintf(last, " ");
          sprintf(buf, "stomach");
          strcat(last, buf);
          blast = TRUE;
        }
      }
      if (!is_covered(ch, COVERS_UPPER_CHEST) && ch->sex == SEX_MALE) {
        for (j = 0; j < MAX_COVERS; j++) {
          if (cover_table[j] == COVERS_UPPER_CHEST)
          ref = j;
        }
        if (!hasplaced) {
          sprintf(buf, "the top of his chest");
          strcat(word, buf);
          hasplaced = TRUE;
          blast = FALSE;
        }
        else {
          if (blast) {
            sprintf(buf, ",%s", last);
            strcat(word, buf);
          }
          if (ch->pcdata->blood[ref] > 100) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " spattered in blood and very dirty ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " spattered in blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood spattered ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->blood[ref] > 40) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " very dirty and speckled with blood ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " speckled with blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood speckled ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " slightly dirty ");
            sprintf(last, "%s", buf);
          }
          else
          sprintf(last, " ");
          sprintf(buf, "the top of his chest");
          strcat(last, buf);
          blast = TRUE;
        }
      }
      if (!is_covered(ch, COVERS_UPPER_CHEST) && ch->sex == SEX_FEMALE && is_covered(ch, COVERS_BREASTS)) {
        for (j = 0; j < MAX_COVERS; j++) {
          if (cover_table[j] == COVERS_UPPER_CHEST)
          ref = j;
        }
        if (!hasplaced) {
          sprintf(buf, "the top of her breasts");
          strcat(word, buf);
          hasplaced = TRUE;
          blast = FALSE;
        }
        else {
          if (blast) {
            sprintf(buf, ",%s", last);
            strcat(word, buf);
          }
          if (ch->pcdata->blood[ref] > 100) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " spattered in blood and very dirty ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " spattered in blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood spattered ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->blood[ref] > 40) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " very dirty and speckled with blood ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " speckled with blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood speckled ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " slightly dirty ");
            sprintf(last, "%s", buf);
          }
          else
          sprintf(last, " ");
          sprintf(buf, "top of her breasts");
          strcat(last, buf);
          blast = TRUE;
        }
      }
      else if (!is_covered(ch, COVERS_BREASTS) && ch->sex == SEX_FEMALE) {
        for (j = 0; j < MAX_COVERS; j++) {
          if (cover_table[j] == COVERS_BREASTS)
          ref = j;
        }
        if (!hasplaced) {
          sprintf(buf, "breasts");
          strcat(word, buf);
          hasplaced = TRUE;
          blast = FALSE;
        }
        else {
          if (blast) {
            sprintf(buf, ",%s", last);
            strcat(word, buf);
          }
          if (ch->pcdata->blood[ref] > 100) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " spattered in blood and very dirty ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " spattered in blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood spattered ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->blood[ref] > 40) {
            if (ch->pcdata->dirt[ref] > 100) {
              sprintf(buf, " very dirty and speckled with blood ");
              sprintf(last, "%s", buf);
            }
            else if (ch->pcdata->dirt[ref] > 40) {
              sprintf(buf, " speckled with blood and dirty ");
              sprintf(last, "%s", buf);
            }
            else {
              sprintf(buf, " blood speckled ");
              sprintf(last, "%s", buf);
            }
          }
          else if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " slightly dirty ");
            sprintf(last, "%s", buf);
          }
          else
          sprintf(last, " ");
          sprintf(buf, "breasts");
          strcat(last, buf);
          blast = TRUE;
        }
      }
    }
    if (!is_covered(ch, COVERS_LOWER_BACK) && !is_covered(ch, COVERS_UPPER_BACK)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_UPPER_BACK)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "back");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "back");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    else if (!is_covered(ch, COVERS_LOWER_BACK)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_LOWER_BACK)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "lower back");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "lower back");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    else if (!is_covered(ch, COVERS_UPPER_BACK)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_UPPER_BACK)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "upper back");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "upper back");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_GROIN) && ch->shape != SHAPE_MERMAID) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_GROIN)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "`168groin`x");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "`168groin`x");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_ARSE) && ch->shape != SHAPE_MERMAID) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_ARSE)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "`168buttocks`x");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "`168buttocks`x");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_LOWER_LEGS) && !is_covered(ch, COVERS_THIGHS) && ch->shape != SHAPE_MERMAID) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_LOWER_LEGS)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "legs");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "legs");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_FEET) && ch->shape != SHAPE_MERMAID) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_FEET)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "feet");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "feet");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_LOWER_ARMS) && !is_covered(ch, COVERS_UPPER_ARMS)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_LOWER_ARMS)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "arms");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "arms");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    else if (!is_covered(ch, COVERS_LOWER_ARMS)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_LOWER_ARMS)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "forearms");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "forearms");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    else if (!is_covered(ch, COVERS_UPPER_ARMS)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_UPPER_ARMS)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "upper arms");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "upper arms");
        strcat(last, buf);
        blast = TRUE;
      }
    }
    if (!is_covered(ch, COVERS_HANDS)) {
      for (j = 0; j < MAX_COVERS; j++) {
        if (cover_table[j] == COVERS_HANDS)
        ref = j;
      }
      if (!hasplaced) {
        sprintf(buf, "hands");
        strcat(word, buf);
        hasplaced = TRUE;
        blast = FALSE;
      }
      else {
        if (blast) {
          sprintf(buf, ",%s", last);
          strcat(word, buf);
        }
        if (ch->pcdata->blood[ref] > 100) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " spattered in blood and very dirty ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " spattered in blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood spattered ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->blood[ref] > 40) {
          if (ch->pcdata->dirt[ref] > 100) {
            sprintf(buf, " very dirty and speckled with blood ");
            sprintf(last, "%s", buf);
          }
          else if (ch->pcdata->dirt[ref] > 40) {
            sprintf(buf, " speckled with blood and dirty ");
            sprintf(last, "%s", buf);
          }
          else {
            sprintf(buf, " blood speckled ");
            sprintf(last, "%s", buf);
          }
        }
        else if (ch->pcdata->dirt[ref] > 100) {
          sprintf(buf, " very dirty ");
          sprintf(last, "%s", buf);
        }
        else if (ch->pcdata->dirt[ref] > 40) {
          sprintf(buf, " slightly dirty ");
          sprintf(last, "%s", buf);
        }
        else
        sprintf(last, " ");
        sprintf(buf, "hands");
        strcat(last, buf);
        blast = TRUE;
      }
    }

    ch->pcdata->blood[0] = bloodtemp;
    ch->pcdata->dirt[0] = dirttemp;

    if (blast) {
      sprintf(buf, ", and%s.", last);
      strcat(word, buf);
    }
    if (hasplaced)
    return str_dup(word);
    else
    return " clothing.";
  }

  char *get_detailed_locations(CHAR_DATA *ch) {

    char buf[MSL];
    //    char * word = str_dup(" ");
    char word[MSL];
    bool placed = FALSE;
    sprintf(word, " ");

    if ((safe_strlen(ch->pcdata->detail_under[0]) > 3 && !is_covered(ch, cover_table[0])) || safe_strlen(ch->pcdata->detail_over[0]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "hands");
      else
      sprintf(buf, ", hands");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[1]) > 3 && !is_covered(ch, cover_table[1])) || safe_strlen(ch->pcdata->detail_over[1]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "forearms");
      else
      sprintf(buf, ", forearms");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[2]) > 3 && !is_covered(ch, cover_table[2])) || safe_strlen(ch->pcdata->detail_over[2]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "upper arms");
      else
      sprintf(buf, ", upper arms");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[3]) > 3 && !is_covered(ch, cover_table[3])) || safe_strlen(ch->pcdata->detail_over[3]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "feet");
      else
      sprintf(buf, ", feet");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[4]) > 3 && !is_covered(ch, cover_table[4])) || safe_strlen(ch->pcdata->detail_over[4]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "lower legs");
      else
      sprintf(buf, ", lower legs");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[6]) > 3 && !is_covered(ch, cover_table[6])) || safe_strlen(ch->pcdata->detail_over[6]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "thighs");
      else
      sprintf(buf, ", thighs");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[7]) > 3 && !is_covered(ch, cover_table[7])) || safe_strlen(ch->pcdata->detail_over[7]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "groin");
      else
      sprintf(buf, ", groin`x");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[8]) > 3 && !is_covered(ch, cover_table[8])) || safe_strlen(ch->pcdata->detail_over[8]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "rear");
      else
      sprintf(buf, ", rear");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[9]) > 3 && !is_covered(ch, cover_table[9])) || safe_strlen(ch->pcdata->detail_over[9]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "lower back");
      else
      sprintf(buf, ", lower back");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[10]) > 3 && !is_covered(ch, cover_table[10])) || safe_strlen(ch->pcdata->detail_over[10]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "upper back");
      else
      sprintf(buf, ", upper back");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[11]) > 3 && !is_covered(ch, cover_table[11])) || safe_strlen(ch->pcdata->detail_over[11]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "stomach");
      else
      sprintf(buf, ", stomach");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[12]) > 3 && !is_covered(ch, cover_table[12])) || safe_strlen(ch->pcdata->detail_over[12]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "chest");
      else
      sprintf(buf, ", chest");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[13]) > 3 && !is_covered(ch, cover_table[13])) || safe_strlen(ch->pcdata->detail_over[13]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "upper chest");
      else
      sprintf(buf, ", upper chest");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[14]) > 3 && !is_covered(ch, cover_table[14])) || safe_strlen(ch->pcdata->detail_over[14]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "neck");
      else
      sprintf(buf, ", neck");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[15]) > 3 && !is_covered(ch, cover_table[15])) || safe_strlen(ch->pcdata->detail_over[15]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "face");
      else
      sprintf(buf, ", face");
      strcat(word, buf);
      placed = TRUE;
    }
    if ((safe_strlen(ch->pcdata->detail_under[16]) > 3 && !is_covered(ch, cover_table[16])) || safe_strlen(ch->pcdata->detail_over[16]) > 3) {
      if (placed == FALSE)
      sprintf(buf, "hair");
      else
      sprintf(buf, ", hair");
      strcat(word, buf);
      placed = TRUE;
    }

    strcat(word, ".");
    return str_dup(word);
  }

  int get_wordwrap(CHAR_DATA *ch) {
    if (ch->linewidth < 10 || ch->linewidth > 1000)
    return 80;

    return ch->linewidth;
  }

  int local_fame_level(CHAR_DATA *ch) {
    if (ch->race == RACE_CELEBRITY || ch->race == RACE_PILLAR)
    return 3;
    if (ch->race == RACE_IMPORTANT)
    return 2;
    if (ch->race == RACE_LOCAL || ch->race == RACE_DEPUTY)
    return 1;
    if(is_ffamily(ch))
    return 1;
    return 0;
  }
  int can_see_local_fame(CHAR_DATA *ch) {
    if (ch->race == RACE_LOCAL || ch->race == RACE_IMPORTANT)
    return TRUE;
    if (ch->race == RACE_CELEBRITY || ch->race == RACE_PILLAR)
    return TRUE;
    if (ch->race == RACE_DEPUTY)
    return TRUE;
    if (ch->race == RACE_FACULTY)
    return TRUE;

    if(is_ffamily(ch))
    return TRUE;

    if (ch->played / 3600 > 150)
    return TRUE;

    return FALSE;
  }

  char *fame_insert(CHAR_DATA *ch) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    sprintf(buf, "%s is ", (ch->sex == SEX_MALE) ? "He" : "She");

    if (get_skill(ch, SKILL_FAMOUS) > 0) {
      strcat(buf, "a celebrity known for ");
      strcat(buf, ch->pcdata->fame);
      strcat(buf, ".`x");
    }
    else if (local_fame_level(ch) == 1) {
      strcat(buf, "known locally for ");
      strcat(buf, ch->pcdata->fame);
      strcat(buf, ".`x");
    }
    else if (local_fame_level(ch) == 2) {
      strcat(buf, "well known locally for ");
      strcat(buf, ch->pcdata->fame);
      strcat(buf, ".`x");
    }
    else if (local_fame_level(ch) == 3) {
      strcat(buf, "famous locally for ");
      strcat(buf, ch->pcdata->fame);
      strcat(buf, ".`x");
    }
    else
    return "";

    return buf;
  }
  char *wilds_fame_insert(CHAR_DATA *ch) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    sprintf(buf, "%s is ", (ch->sex == SEX_MALE) ? "He" : "She");

    if (ch->pcdata->wilds_fame_level == 1) {
      strcat(buf, "known in the Wilds for ");
      strcat(buf, ch->pcdata->wilds_fame);
      strcat(buf, ".`x");
    }
    else if (ch->pcdata->wilds_fame_level == 2) {
      strcat(buf, "famous in the Wilds for ");
      strcat(buf, ch->pcdata->wilds_fame);
      strcat(buf, ".`x");
    }
    else if (ch->pcdata->wilds_fame_level == 3) {
      strcat(buf, "a legend in the Wilds for ");
      strcat(buf, ch->pcdata->wilds_fame);
      strcat(buf, ".`x");
    }
    else
    return "";

    return buf;
  }

  char *other_fame_insert(CHAR_DATA *ch) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    sprintf(buf, "%s is ", (ch->sex == SEX_MALE) ? "He" : "She");

    if (ch->pcdata->other_fame_level == 1) {
      strcat(buf, "known in the Other for ");
      strcat(buf, ch->pcdata->other_fame);
      strcat(buf, ".`x");
    }
    else if (ch->pcdata->other_fame_level == 2) {
      strcat(buf, "famous in the Other for ");
      strcat(buf, ch->pcdata->other_fame);
      strcat(buf, ".`x");
    }
    else if (ch->pcdata->other_fame_level == 3) {
      strcat(buf, "a legend in the Other for ");
      strcat(buf, ch->pcdata->other_fame);
      strcat(buf, ".`x");
    }
    else
    return "";

    return buf;
  }

  char *godrealm_fame_insert(CHAR_DATA *ch) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    sprintf(buf, "%s is ", (ch->sex == SEX_MALE) ? "He" : "She");
    // Lazy 'fix' for the ugly everyone having godrealms fame 3 bug - Discordance
    if (ch->pcdata->godrealm_fame[0] != '\0') 
    {
      if (ch->pcdata->godrealm_fame_level == 1) {
        strcat(buf, "known in the Godrealms for ");
        strcat(buf, ch->pcdata->godrealm_fame);
        strcat(buf, ".`x");
      }
      else if (ch->pcdata->godrealm_fame_level == 2) {
        strcat(buf, "famous in the Godrealms for ");
        strcat(buf, ch->pcdata->godrealm_fame);
        strcat(buf, ".`x");
      }
      else if (ch->pcdata->godrealm_fame_level == 3) {
        strcat(buf, "a legend in the Godrealms for ");
        strcat(buf, ch->pcdata->godrealm_fame);
        strcat(buf, ".`x");
      }
      else
      return "";
    }
    else
    return strdup("");

    return buf;
  }

  char *hell_fame_insert(CHAR_DATA *ch) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    sprintf(buf, "%s is ", (ch->sex == SEX_MALE) ? "He" : "She");

    if (ch->pcdata->hell_fame_level == 1) {
      strcat(buf, "known in Hell for ");
      strcat(buf, ch->pcdata->hell_fame);
      strcat(buf, ".`x");
    }
    else if (ch->pcdata->hell_fame_level == 2) {
      strcat(buf, "famous in Hell for ");
      strcat(buf, ch->pcdata->hell_fame);
      strcat(buf, ".`x");
    }
    else if (ch->pcdata->hell_fame_level == 3) {
      strcat(buf, "a legend in Hell for ");
      strcat(buf, ch->pcdata->hell_fame);
      strcat(buf, ".`x");
    }
    else
    return "";

    return buf;
  }


  int attract_lifeforce(CHAR_DATA *ch) {
    int lf = get_lifeforce(ch, FALSE, NULL);
    lf = UMIN(lf, 120);
    lf = UMAX(lf, 80);
    if (lf > 100) {
      lf -= 100;
      lf /= 3;
      lf += 100;
    }
    return lf;
  }

  char *shroud_desc(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return "";

    if ((available_donated(ch) >= 750 || ch->race == RACE_DREAMCHILD) && safe_strlen(ch->pcdata->caura) > 2) {
      if (get_tier(ch) > 3)
      return ch->pcdata->caura;

      char arg[MSL];
      remove_color(arg, ch->pcdata->caura);

      if (get_tier(ch) == 3) {
        char tmp[MSL];
        sprintf(tmp, "%s", ch->pcdata->caura);
        if (tmp[0] == '`') {
          char buf[MSL];
          sprintf(buf, "%c%c%s`x", tmp[0], tmp[1], arg);
          return str_dup(buf);
        }
        else {
          char buf[MSL];
          sprintf(buf, "`x%s`x", arg);
          return str_dup(buf);
        }
      }
      char buf[MSL];
      sprintf(buf, "`x%s`x", arg);
      return str_dup(buf);
    }

    if (get_skill(ch, SKILL_OBFUSCATE) > 0) {
      return "";
    }
    switch (get_app_race(ch)) {
    case RACE_LOCAL:
      return "";
      break;
    case RACE_VISITOR:
      return "";
      break;
    case RACE_STUDENT:
      return "";
      break;
    case RACE_SOLDIER:
      if (ch->sex == SEX_MALE)
      return "He is wearing leather bandoliers of ammunition.`x";
      else
      return "She is wearing leather bandoliers of ammunition.`x";
      break;
    case RACE_TIMESWEPT:
      return "";
      /*
      if(ch->sex == SEX_MALE)
      return "`rTime moves slower in the air around his form.`x";
      else
      return "`rTime moves slower in the air around her form.`x";
      */
      break;
    case RACE_WILDLING:
      if (ch->sex == SEX_MALE)
      return "He has warpaint under his eyes and is wearing a bow over his shoulder.`x";
      else
      return "She has warpaint under her eyes and is wearing a bow over her shoulder.`x";
      break;
    case RACE_BROWN:
      return "";
      break;
    case RACE_IMPORTANT:
      return "";
      break;
    case RACE_DABBLER:
      if (ch->sex == SEX_MALE)
      return "He wears a golden astrological charm hung around his neck.`x";
      else
      return "She wears a golden astrological charm hung around her neck.`x";
      break;
    case RACE_DEPUTY:
      if (ch->sex == SEX_MALE)
      return "He has a shiny silver sheriff's badge just over his heart.`x";
      else
      return "She has a shiny silver sheriff's badge just over her heart.`x";
      break;
    case RACE_FACULTY:
      if (ch->sex == SEX_MALE)
      return "His serpentine shadow moves of its own accord.`x";
      else
      return "Her serpentine shadow moves of its own accord.`x";
      break;
    case RACE_NEWVAMPIRE:
      if (ch->sex == SEX_MALE)
      return "A faint mist of blood surrounds him.`x";
      else
      return "A faint mist of blood surrounds her.`x";
      break;
    case RACE_NEWWEREWOLF:
      if (ch->sex == SEX_MALE)
      return "He has thick fur running along his arms and upper back.`x";
      else
      return "She has thick fur running along her arms and upper back.`x";
      break;
    case RACE_NEWFAEBORN:
      if (ch->sex == SEX_MALE)
      return "He has pointed ears and wears a mantle of leaves.`x";
      else
      return "She has pointed ears and wears a mantle of leaves.`x";
      break;
    case RACE_NEWDEMONBORN:
      if (ch->sex == SEX_MALE)
      return "He has small black horns.`x";
      else
      return "She has small black horns.`x";
      break;
    case RACE_NEWDEMIGOD:
      if (ch->sex == SEX_MALE)
      return "He's wearing a rich red cloak.`x";
      else
      return "She's wearing a rich red cloak.`x";
      break;
    case RACE_NEWANGELBORN:
      if (ch->sex == SEX_MALE)
      return "He has small white bird-like wings.`x";
      else
      return "She has small white bird-like wings.`x";
      break;
    case RACE_GIFTED:
      if (ch->sex == SEX_MALE)
      return "He's wearing an open black robe.`x";
      else
      return "She's wearing an open black robe.`x";
      break;
    case RACE_SFORCES:
      if (ch->sex == SEX_MALE)
      return "He is wearing a black tactical combat harness.`x";
      else
      return "She is wearing a black tactical combat harness.`x";
      break;
    case RACE_CELEBRITY:
      if (ch->sex == SEX_MALE)
      return "Tiny stars twinkle around him.`x";
      else
      return "Tiny stars twinkle around her.`x";
      break;
    case RACE_PILLAR:
      if (ch->sex == SEX_MALE)
      return "He is wearing a large golden key around his neck.`x";
      else
      return "She is wearing a large golden key around her neck.`x";
      break;
    case RACE_VETVAMPIRE:
      if (ch->sex == SEX_MALE)
      return "`RHe is wearing a cloak of flowing blood.`x";
      else
      return "`RShe is wearing a cloak of flowing blood.`x";
      break;
    case RACE_VETWEREWOLF:
      if (ch->sex == SEX_MALE)
      return "`DHe has thick fur on his arms and back and black claws for fingernails.`x";
      else
      return "`DShe has thick fur on his arms and back and black claws for fingernails.`x";
      break;
    case RACE_VETFAEBORN:
      if (ch->sex == SEX_MALE)
      return "`gHe has small antlers and wears a mantle of leaves.`x";
      else
      return "`gShe has small antlers and wears a mantle of leaves.`x";
      break;
    case RACE_VETDEMONBORN:
      if (ch->sex == SEX_MALE)
      return "`RHe has black horns and a red lizard-like tail.`x";
      else
      return "`RShe has black horns and a red lizard-like tail.`x";
      break;
    case RACE_VETANGELBORN:
      if (ch->sex == SEX_MALE)
      return "`WHe has large white bird-like wings.`x";
      else
      return "`RShe has large white bird-like wings.`x";
      break;
    case RACE_VETDEMIGOD:
      if (ch->sex == SEX_MALE)
      return "`YHe is wearing a cloak of woven gold.`x";
      else
      return "`yShe is wearing a cloak of woven gold.`x";
      break;
    case RACE_VETGIFTED:
      if (ch->sex == SEX_MALE)
      return "`WHe has glowing all-white eyes that trail smoke.`x";
      else
      return "`WShe has glowing all-white eyes that trail smoke.`x";
      break;
    case RACE_OLDVAMPIRE:
      if (ch->sex == SEX_MALE)
      return "`RHe wears a cloak of flowing blood and has long white fangs.`x";
      else
      return "`RShe wears a cloak of flowing blood and has long white fangs.`x";
      break;
    case RACE_OLDWEREWOLF:
      if (ch->sex == SEX_MALE)
      return "`DHe has `Yferal golden eyes`D with crescent moon pupils and is covered in thick fur.`x";
      else
      return "`DShe has `Yferal golden eyes`D with crescent moon pupils and is covered in thick fur.`x";
      break;
    case RACE_OLDFAEBORN:
      if (ch->sex == SEX_MALE)
      return "`gHe has large `ystag-horns`g and wears a sweeping `Gcloak`g of leaves.`x";
      else
      return "`gShe has large `ystag-horns`g and wears a sweeping `Gcloak`g of leaves`x";
      break;
    case RACE_OLDDEMONBORN:
      if (ch->sex == SEX_MALE)
      return "`RHe has large black horns, a red lizard-like tail and bat-like wings.`x";
      else
      return "`RShe has large black horns, a red lizard-like tail and bat-like wings`x";
      break;
    case RACE_OLDANGELBORN:
      if (ch->sex == SEX_MALE)
      return "`WHe has large angelic wings and wears a `Yg`yl`Yo`yw`Yi`yn`Yg `yg`Yo`yl`Yd`ye`Yn `yc`Yi`yr`Yc`yl`Ye`yt.`x";
      else
      return "`WShe has large angelic wings and wears a `Yg`yl`Yo`yw`Yi`yn`Yg `yg`Yo`yl`Yd`ye`Yn `yc`Yi`yr`Yc`yl`Ye`yt.`x";
      break;
    case RACE_OLDDEMIGOD:
      if (ch->sex == SEX_MALE)
      return "`YHe is clad in golden armor from the neck down, with a `Rb`rr`Ri`rl`Rl`ri`Ra`rn`Rt `rr`Re`rd `Rc`rl`Ro`ra`Rk`x sweeping out behind`x";
      else
      return "`YShe is clad in golden armor from the neck down, with a `Rb`rr`Ri`rl`Rl`ri`Ra`rn`Rt `rr`Re`rd `Rc`rl`Ro`ra`Rk`x sweeping out behind`x";
      break;
    case RACE_ANCIENTVAMPIRE:
      if (ch->sex == SEX_MALE)
      return "`DHe appears to be in perpetual shadow, except for his `rblood`R-`rred`D eyes and gleaming white fangs.`x";
      else
      return "`DShe appears to be in perpetual shadow, except for her `rblood`R-`rred`D eyes and gleaming white fangs.`x";
      break;
    case RACE_ANCIENTWEREWOLF:
      if (ch->sex == SEX_MALE)
      return "`WHe is covered in white fur and his `Yferal gold eyes`W have crescent moon-shaped `Dpupils.`x";
      else
      return "`WShe is covered in ethereal fur and her `Yferal gold eyes`W have crescent moon-shaped `Dpupils.`x";
      break;
    case RACE_DEMIDEMON:
      if (ch->sex == SEX_MALE)
      return "`RHe has large sweeping horns, a lizard-like tail and huge bat-like wings all made from `rf`Rl`Ya`Rm`re.`x";
      else
      return "`RShe has large sweeping horns, a lizard-like tail and huge bat-like wings all made from `rf`Rl`Ya`Rm`re.`x";
      break;
    case RACE_FREEANGEL:
      if (ch->sex == SEX_MALE)
      return "`WHe gives off an intense silver light from his whole body, has large angelic wings and wears a `Yg`yl`Yo`yw`Yi`yn`Yg `yg`Yo`yl`Yd`ye`Yn `yc`Yi`yr`Yc`yl`Ye`yt.`x";
      else
      return "`WShe gives off an intense silver light from her whole body, has large angelic wings and wears a `Yg`yl`Yo`yw`Yi`yn`Yg `yg`Yo`yl`Yd`ye`Yn `yc`Yi`yr`Yc`yl`Ye`yt.`x";
      break;
    case RACE_GODCHILD:
      if (ch->sex == SEX_MALE)
      return "`YHe is clad in golden armor from the neck down, with a `Rb`rr`Ri`rl`Rl`ri`Ra`rn`Rt `rr`Re`rd `Rc`rl`Ro`ra`Rk`Y sweeping out behind, tossed by an invisible wind. Upon his head is a circlet of blazing, ever-shifting `Rr`ru`Rb`ri`Re`rs`Y.`x";
      else
      return "`YShe is clad in golden armor from the neck down, with a `Rb`rr`Ri`rl`Rl`ri`Ra`rn`Rt `rr`Re`rd `Rc`rl`Ro`ra`Rk`Y sweeping out behind, tossed by an invisible wind. Upon her head is a circlet of blazing, ever-shifting `Rr`ru`Rb`ri`Re`rs`Y.`x";
      break;
    case RACE_FAECHILD:
      if (ch->sex == SEX_MALE)
      return "`gHe has a set of majestic `Yg`yo`Yl`yd`Ye`yn `Ya`yn`Yt`yl`Ye`yr`Ys`g and a cloak of `cp`mr`Ci`Ms`Wm`Ma`Ct`mi`cc `rl`ci`Wg`ch`rt `gflowing out behind him.`x";
      else
      return "`gShe has a set of majestic `Yg`yo`Yl`yd`Ye`yn `Ya`yn`Yt`yl`Ye`yr`Ys`g and a cloak of `cp`mr`Ci`Ms`Wm`Ma`Ct`mi`cc `rl`ci`Wg`ch`rt `gflowing out behind her.`x";
      break;
    case RACE_WIGHT:
      if (ch->sex == SEX_MALE)
      return "He has ghostly all-white eyes.`x";
      else
      return "She has ghostly all-white eyes.`x";
      break;
    case RACE_NORMAL:
      return "";
      break;
    }
    return "";
  }

  const char *cup_size[] = {
    "small A cup breasts", "A cup breasts", "large A cup breasts",
    "small B cup breasts", "B cup breasts", "large B cup breasts",
    "small C cup breasts", "C cup breasts", "large C cup breasts",
    "small D cup breasts", "D cup breasts", "large D cup breasts",
    "small E cup breasts", "E cup breasts", "large E cup breasts",
    "small F cup breasts", "F cup breasts", "large F cup breasts",
    "small G cup breasts", "G cup breasts", "large G cup breasts",
    "small H cup breasts", "H cup breasts", "large H cup breasts",
    "small I cup breasts", "I cup breasts", "large I cup breasts",
    "small J cup breasts", "J cup breasts", "large J cup breasts",
    "small K cup breasts", "K cup breasts", "large K cup breasts"
  };

  int get_height_mod(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int iWear;
    int value = 0;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if (iWear == WEAR_HOLD)
      continue;
      if (iWear == WEAR_HOLD_2)
      continue;

      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (obj->item_type != ITEM_CLOTHING)
      continue;

      if (obj->value[2] > 0 && obj->value[2] < 21)
      value = obj->value[2];
    }

    if (value == 0)
    return 0;
    else if (value < 4)
    return 1;
    else if (value < 6)
    return 2;
    else if (value < 8)
    return 3;
    else
    return value - 5;
    return 0;
  }
  int get_bust_mod(CHAR_DATA *ch) {
    int mod = 0;
    OBJ_DATA *obj;
    int iWear;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if (iWear == WEAR_HOLD)
      continue;
      if (iWear == WEAR_HOLD_2)
      continue;

      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_JEWELRY)
      continue;

      if (obj->value[2] > 20 && obj->value[2] < 41) {
        mod -= 30;
        mod += obj->value[2];
      }
    }
    return mod;
  }

  char *get_eyes(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (is_animal(victim) && get_animal_genus(victim, ANIMAL_ACTIVE) == GENUS_SWARM)
    return "eyes";

    if (!is_covered(victim, COVERS_EYES) && safe_strlen_color(victim->pcdata->eyes) > 2) {
      return strdup(victim->pcdata->eyes);
    }
    else if (!is_covered(victim, COVERS_EYES)) {
      return get_natural_eyes(victim);
    }
    else
    return "covered eyes";
  }
  char *get_skin(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (victim->shape != SHAPE_HUMAN && victim->shape != SHAPE_MERMAID)
    return animal_skin(victim);

    if (get_skill(victim, SKILL_ARMORED) > 0 && torso_exposed(victim)) {
      char buf[MSL];
      sprintf(buf, "hardened, porcelain-like %s", victim->pcdata->skin);
      return str_dup(buf);
    }
    return victim->pcdata->skin;
  }
  char *get_hair(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (victim->shape != SHAPE_HUMAN && victim->shape != SHAPE_MERMAID)
    return get_natural_hair(victim);

    if (!is_covered(victim, COVERS_HAIR))
    return get_basic_hair(victim);
    else
    return "covered hair";
  }

  int get_height_inches(CHAR_DATA *ch) {
    int height = ch->pcdata->height_inches + get_height_mod(ch);
    height = height % 12;
    return height;
  }
  int get_height_feet(CHAR_DATA *ch) {
    int height = ch->pcdata->height_feet;
    if ((ch->pcdata->height_inches + get_height_mod(ch)) >= 12)
    height++;
    return height;
  }

  int get_bust(CHAR_DATA *ch) {
    int bust = ch->pcdata->bust;
    bust += get_bust_mod(ch);
    if (bust > 33)
    bust = 33;
    if (bust < 0)
    bust = 0;
    return bust - 1;
  }

  bool is_imprintable(CHAR_DATA *ch) {

    if (!is_super(ch) && event_dominance == 1)
    return TRUE;

    if (get_skill(ch, SKILL_ANGELBORN) > 0)
    return TRUE;

    if (is_angelborn(ch))
    return TRUE;

    return FALSE;
  }

  //called for use with age_names; needs to be merged - Disco
  int get_age_descriptor(CHAR_DATA *victim) {
    int age = 0;
    int modifier = 0;

    age = get_age(victim);

    age = age + modifier;
    // age category table
    if      (age < 20)  {return  1;}
    else if (age < 24)  {return  2;}
    else if (age < 27)  {return  3;}
    else if (age < 30)  {return  4;}
    else if (age < 34)  {return  5;}
    else if (age < 37)  {return  6;}
    else if (age < 40)  {return  7;}
    else if (age < 44)  {return  8;}
    else if (age < 47)  {return  9;}
    else if (age < 50)  {return 10;}
    else if (age < 54)  {return 11;}
    else if (age < 57)  {return 12;}
    else if (age < 60)  {return 13;}
    else if (age < 64)  {return 14;}
    else if (age < 67)  {return 15;}
    else if (age < 70)  {return 16;}
    else if (age < 74)  {return 17;}
    else if (age < 77)  {return 18;}
    else if (age < 80)  {return 19;}
    else if (age < 84)  {return 20;}
    else if (age < 87)  {return 21;}
    else if (age < 90)  {return 22;}
    else if (age < 94)  {return 23;}
    else if (age < 97)  {return 24;}
    else if (age < 100) {return 25;}
    else                {return 26;}
  }

  char *format_column(int width, char header[MSL], char field[MSL], int type) {

    double average_width, fraction, whole;
    int column1, column2, limit;
    char content[MSL], buf[MSL];

    // Finding column sizes
    average_width = (width - 4) / 3;
    fraction = modf(average_width, &whole);
    column1 = (int)floor(average_width);
    if (fraction == 0) {
      column2 = column1;
    }
    else {
      column2 = column1 + (width - 4 - (column1 * 3));
    }

    // Inputing
    strcpy(buf, header);
    if (type == 1) {
      limit = column1 - safe_strlen_color(buf);
    }
    else {
      limit = column2 - safe_strlen_color(buf);
    }
    sprintf(content, "%s", buf);
    sprintf(buf, "%s", field);
    if (safe_strlen_color(buf) > limit - 1) {
      strncpy(buf, buf, limit - 1);
      if (buf[limit - 2] == '`') {
        limit = limit + 2;
      }
      else if (buf[limit - 1] == '`') {
        limit = limit + 1;
      }

      buf[limit - 1] = '\0';
    }
    strcat(content, buf);

    return str_dup(content);
  }

  // Puts text in an ascii box - Discordance
  char *boxify(CHAR_DATA *ch, int width, char source[MSL]) {
    char buf[MSL], box[MSL], xbuf[MAX_STRING_LENGTH];
    int i = 0, j = 0, end_of_line, spacing;
    char *frame;
    char *rdesc;
    static char xbuf2[MAX_STRING_LENGTH];
    char spacer[MSL] = "";

    frame = str_dup("");
    rdesc = str_dup("");
    strcpy(buf, "");
    strcpy(xbuf, "");
    strcpy(spacer, "");
    strcpy(box, "");

    // Upper border
    strcpy(buf, "`g,");
    frame = draw_horizontal_line(width - 2);
    strcat(buf, frame);
    strcat(buf, ",`x\n\r");
    strcat(box, buf);

    if (!width) {
      strcpy(xbuf2, source);
      return xbuf2;
    }

    xbuf[0] = xbuf2[0] = '\0';

    rdesc = str_dup(source);

    strcpy(xbuf, "`g|`x ");
    for (;;) {
      end_of_line = width - 4;
      j = end_of_line;
      for (i = 0; i < end_of_line; i++) {
        if (*(rdesc + i) == '`') {
          end_of_line += 2;
          i++;
        }

        if (!*(rdesc + i))
        break;

        if (*(rdesc + i) == '\r')
        end_of_line = i;
        j = end_of_line;
      }
      if (i < end_of_line) {
        break;
      }
      if (*(rdesc + i - 1) != '\r') {
        for (i = (xbuf[0] ? (end_of_line - 1) : (end_of_line - 4)); i; i--) {
          if (*(rdesc + i) == ' ')
          break;
        }
        if (i) {
          *(rdesc + i) = 0;
          strcat(xbuf, rdesc);
          printf_to_char(ch, "I1: %d\n\r", i);
          printf_to_char(ch, "J1: %d\n\r", j);
          printf_to_char(ch, "End1: %d\n\r", end_of_line);
          spacing = width - 4 - safe_strlen_color(rdesc);
          printf_to_char(ch, "rdesc1: %d\n\r", safe_strlen_color(rdesc));
          printf_to_char(ch, "Spacing1: %d\n\r", spacing);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(xbuf, buf);
          strcat(xbuf, " `g|`x\n\r`g|`x ");
          rdesc += i + 1;
          while (*rdesc == ' ')
          rdesc++;
        }
        else {
          bug("Wrap String: No spaces", 0);
          *(rdesc + (end_of_line - 2)) = 0;
          strcat(xbuf, rdesc);
          printf_to_char(ch, "I2: %d\n\r", i);
          printf_to_char(ch, "J2: %d\n\r", j);
          printf_to_char(ch, "End2: %d\n\r", end_of_line);
          spacing = width - 4 - safe_strlen_color(rdesc);
          printf_to_char(ch, "rdesc2: %d\n\r", safe_strlen_color(rdesc));
          printf_to_char(ch, "Spacing2: %d\n\r", spacing);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(xbuf, buf);
          strcat(xbuf, "-`g|`x\n\r`g|`x ");
          rdesc += end_of_line - 1;
        }
      }
      else {
        *(rdesc + i - 1) = 0;
        strcat(xbuf, rdesc);
        printf_to_char(ch, "I3: %d\n\r", i);
        printf_to_char(ch, "J3: %d\n\r", j);
        printf_to_char(ch, "End3: %d\n\r", end_of_line);
        spacing = width - 4 - safe_strlen_color(rdesc);
        printf_to_char(ch, "width3: %d\n\r", width);
        printf_to_char(ch, "rdesc3: %d\n\r", safe_strlen_color(rdesc));
        printf_to_char(ch, "Spacing3: %d\n\r", spacing);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(xbuf, buf);
        strcat(xbuf, " `g|`x\n\r`g|`x ");
        rdesc += i;
        while (*rdesc == ' ')
        rdesc++;
        // spacing=width-2-safe_strlen_color(rdesc);
        // printf_to_char(ch, "rdesc3a: %d\n\r", safe_strlen_color(rdesc));
        // printf_to_char(ch, "Spacing3a: %d\n\r", spacing);
        // sprintf(buf, "%*s", spacing, spacer);
        // strcat(xbuf, buf);
        strcat(xbuf, " `g|`x\n\r`g|`x ");
      }
    }
    while (*(rdesc + i) && (*(rdesc + i) == ' ' || *(rdesc + i) == '\n' || *(rdesc + i) == '\r'))
    i--;
    *(rdesc + i + 1) = 0;
    strcat(xbuf, rdesc);
    if (xbuf[safe_strlen_color(xbuf) - 2] != '\n') {
      printf_to_char(ch, "I4: %d\n\r", i);
      printf_to_char(ch, "J4: %d\n\r", j);
      printf_to_char(ch, "End4: %d\n\r", end_of_line);
      spacing = width - 4 - safe_strlen_color(rdesc);
      printf_to_char(ch, "rdesc4: %d\n\r", safe_strlen_color(rdesc));
      printf_to_char(ch, "Spacing4: %d\n\r", spacing);
      sprintf(buf, "%*s", spacing, spacer);
      strcat(xbuf, buf);
      strcat(xbuf, "`g|`x\n\r");
    }
    strcpy(xbuf2, xbuf);
    strcat(box, xbuf2);

    // Lower border
    strcpy(buf, "`g|");
    strcat(buf, frame);
    strcat(buf, "|`x\n\r");
    strcat(box, buf);

    frame = str_dup("");
    rdesc = str_dup("");
    free_string(frame);
    free_string(rdesc);

    return str_dup(box);
  }

  int where_length(char *string) {
    char buf[MSL];
    sprintf(buf, "%s", string);
    int count = 1;
    for (int i = 0; i < 100; i++) {
      if (buf[i] == '\0' || buf[i] == '\n')
      return UMAX(9, UMIN(count, 23));
      if (buf[i] == '>') {
        return UMIN(count, 23);
      }
      count++;
    }
    return 9;
  }

  int max_where(CHAR_DATA *victim) {
    int max = 0;
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++)
    // for ( iWear = MAX_WEAR-1; iWear >= 0; iWear-- )
    {
      if ((obj = get_eq_char(victim, iWear)) != NULL && (can_see_wear(victim, iWear))) {
        if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0') {
          if (where_length(obj->wear_temp) > max)
          max = where_length(obj->wear_temp);
        }
        else if (obj->wear_string != NULL && obj->wear_string[0] != '\0') {
          if (where_length(obj->wear_string) > max)
          max = where_length(obj->wear_string);
        }
        /*
        else
        {
        if(where_length(where_name[iWear]) > max)
        max = where_length(where_name[iWear]);
        }
        */
      }
    }
    return max;
  }

  char *where_pad(char *string, int limit) {
    char input[MSL];
    sprintf(input, "%s", string);
    char output[MSL];
    char word[MSL];
    output[0] = '\0';
    output[0] = 0;
    bool spacefill = FALSE;
    for (int i = 0; i < limit; i++) {
      if (input[i] == '\0' || input[i] == '\n' || spacefill == TRUE) {
        strcat(output, " ");
        spacefill = TRUE;
      }
      else {
        sprintf(word, "%c", input[i]);
        strcat(output, word);
      }
    }
    return str_dup(output);
  }

  char *punctuation_spacing(char string[MSL]) {
    char buf[MSL];

    if (string[0] != '.' && string[0] != ',' && string[0] != '!' && string[0] != '?' && string[0] != ';' && string[0] != '\'') {
      buf[0] = ' ';
      strcpy(buf + 1, string);
    }
    else {
      strcpy(buf, string);
    }

    return str_dup(buf);
  }

  char *equip_string(CHAR_DATA *ch, CHAR_DATA *victim) {
    char string[MSL];
    char buf[MAX_STRING_LENGTH];
    strcpy(string, "");
    strcpy(buf, "");
    OBJ_DATA *obj;
    bool found = FALSE;
    int pad = max_where(victim);

    // equipment and clothing
    if ((obj = get_eqr_char(victim, WEAR_HOLD)) != NULL && can_see_obj(ch, obj) && (can_see_wear(victim, WEAR_HOLD) || (has_xray(ch) && victim != ch && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
      if (!found) {
        strcat(string, "\n\r");
        sprintf(buf, "%s is using:\n\r\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
        strcat(string, buf);
        found = TRUE;
      }

      if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0') {
        if (obj->wear_temp[0] == '(') {
          remove_color(buf, obj->wear_temp);
          strcat(string, where_pad(buf, pad));
          strcat(string, format_obj_to_char(obj, ch, FALSE));
        }
        else {
          strcat(string, format_obj_to_char(obj, ch, FALSE));
          remove_color(buf, obj->wear_temp);
          strcat(string, punctuation_spacing(buf));
        }
      }
      else
      {
        strcat(string, format_obj_to_char(obj, ch, FALSE));
      }

      strcat(string, "\n\r");
    }
    if ((obj = get_eq_char(victim, WEAR_HOLD_2)) != NULL && can_see_obj(ch, obj) && (can_see_wear(victim, WEAR_HOLD_2) || (has_xray(ch) && victim != ch && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
      if (!found) {
        strcat(string, "\n\r");
        sprintf(buf, "%s is using:\n\r\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
        strcat(string, buf);
        found = TRUE;
      }

      if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0') {
        if (obj->wear_temp[0] == '(') {
          remove_color(buf, obj->wear_temp);
          strcat(string, where_pad(buf, pad));
          strcat(string, format_obj_to_char(obj, ch, FALSE));
        }
        else {
          strcat(string, format_obj_to_char(obj, ch, FALSE));
          remove_color(buf, obj->wear_temp);
          strcat(string, punctuation_spacing(buf));
        }
      }
      else
      {
        strcat(string, format_obj_to_char(obj, ch, FALSE));
      }

      strcat(string, "\n\r");
    }
    int iWear;
    strcat(string, "\n\n\r");
    for (iWear = MAX_WEAR - 1; iWear >= 0; iWear--) {
      strcpy(buf, "");
      if (iWear == WEAR_HOLD)
      continue;
      if (iWear == WEAR_HOLD_2)
      continue;

      if ((obj = get_eq_char(victim, iWear)) != NULL && (obj->item_type != ITEM_JEWELRY && obj->item_type != ITEM_CONTAINER) && can_see_obj(ch, obj) && (can_see_wear(victim, iWear) || (has_xray(ch) && victim != ch && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
        if (!found) {
          strcat(string, "\n\r");
          sprintf(buf, "%s is using:\n\r\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
          found = TRUE;
        }

        if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0') {
          if (obj->wear_temp[0] == '(') {
            remove_color(buf, obj->wear_temp);
            strcat(string, where_pad(buf, pad));
            strcat(string, format_obj_to_char(obj, ch, FALSE));
          }
          else {
            strcat(string, format_obj_to_char(obj, ch, FALSE));
            remove_color(buf, obj->wear_temp);
            strcat(string, punctuation_spacing(buf));
          }
        }
        else if (obj->wear_string != NULL && obj->wear_string[0] != '\0') {
          if (obj->wear_string[0] == '(') {
            remove_color(buf, obj->wear_string);
            strcat(string, where_pad(buf, pad));
            strcat(string, format_obj_to_char(obj, ch, FALSE));
          }
          else
          strcat(string, format_obj_to_char(obj, ch, FALSE));
        }
        else {
          strcat(string, format_obj_to_char(obj, ch, FALSE));
          // remove_color(buf, where_name[iWear]);
          // strcat(string, where_pad(buf, pad));
          // strcat(string, punctuation_spacing(buf));
        }

        strcat(string, "\n\r");
      }
    }
    strcat(string, "\n\r");
    for (iWear = MAX_WEAR - 1; iWear >= 0; iWear--) {
      if (iWear == WEAR_HOLD)
      continue;
      if (iWear == WEAR_HOLD_2)
      continue;

      if ((obj = get_eq_char(victim, iWear)) != NULL && (obj->item_type == ITEM_JEWELRY || obj->item_type == ITEM_CONTAINER) && can_see_obj(ch, obj) && (can_see_wear(victim, iWear) || (has_xray(ch) && victim != ch && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
        if (!found) {
          strcat(string, "\n\r");
          sprintf(buf, "%s is using:\n\r\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
          found = TRUE;
        }

        if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0') {
          if (obj->wear_temp[0] == '(') {
            remove_color(buf, obj->wear_temp);
            strcat(string, where_pad(buf, pad));
            strcat(string, format_obj_to_char(obj, ch, FALSE));
          }
          else {
            strcat(string, format_obj_to_char(obj, ch, FALSE));
            remove_color(buf, obj->wear_temp);
            strcat(string, punctuation_spacing(buf));
          }
        }
        else if (obj->wear_string != NULL && obj->wear_string[0] != '\0') {
          if (obj->wear_string[0] == '(') {
            remove_color(buf, obj->wear_string);
            strcat(string, where_pad(buf, pad));
            strcat(string, format_obj_to_char(obj, ch, FALSE));
          }
          else
          strcat(string, format_obj_to_char(obj, ch, FALSE));
        }
        else {
          strcat(string, format_obj_to_char(obj, ch, FALSE));
          // remove_color(buf, where_name[iWear]);
          // strcat(string, where_pad(buf, pad));
          // strcat(string, punctuation_spacing(buf));
        }
        strcat(string, "\n\r");
      }
    }

    // strcat(string, buf);
    strcat(string, "\n\r");
    for (obj = victim->carrying; obj != NULL; obj = obj->next_content) {
      if (!can_see_obj(ch, obj))
      continue;

      if (obj->wear_loc == WEAR_NONE && obj->size >= 25 && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {

        if (!found) {
          strcat(string, "\n\r");
          sprintf(buf, "%s is using:\n\r\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
          found = TRUE;
        }

        if (obj->wear_loc == WEAR_NONE && obj->size >= 25 && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
          strcat(string, format_obj_to_char(obj, ch, FALSE));

          if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0' && (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_RANGED)) {
            remove_color(buf, obj->wear_temp);
            strcat(string, where_pad(buf, pad));
          }
          else if (obj->wear_string != NULL && obj->wear_string[0] != '\0' && (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_RANGED)) {
            remove_color(buf, obj->wear_string);
            strcat(string, where_pad(buf, pad));
          }
          else if (obj->item_type == ITEM_WEAPON) {
            strcat(string, where_pad(", sheathed", pad));
          }
          else {
            strcat(string, where_pad(", carried", pad));
          }
          // strcat(string, format_obj_to_char( obj, ch, FALSE ));
          strcat(string, "\n\r");
        }
      }
      if (has_xray(ch) && victim != ch && obj->wear_loc == WEAR_NONE && obj->size < 25 && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->item_type != ITEM_TRASH) {

        strcat(string, format_obj_to_char(obj, ch, FALSE));

        if (obj->item_type == ITEM_WEAPON) {
          strcat(string, where_pad(", sheathed", pad));
        }
        else {
          strcat(string, where_pad(", pocketed", pad));
        }
        // strcat(string, format_obj_to_char( obj, ch, FALSE ));
        strcat(string, "\n\r");
      }
    }
    return str_dup(string);
  }

  char *number_to_text(int number) {
    switch (number) {
    case 0:
      return "No";
      break;
    case 1:
      return "One";
      break;
    case 2:
      return "Two";
      break;
    case 3:
      return "Three";
      break;
    case 4:
      return "Four";
      break;
    case 5:
      return "Five";
      break;
    case 6:
      return "Six";
      break;
    case 7:
      return "Seven";
      break;
    default:
      return "Unknown number";
      break;
    }
  }

  bool can_see_details(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->pcdata->dream_identity_timer > 0 && safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)) >
        1 && safe_strlen(ch->pcdata->identity_world) > 1 && safe_strlen(dream_detail(victim, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)) > 1)
    return FALSE;
    else
    return TRUE;
  }

  // Looking at characters
  // show_char_to_char needs a little fixing up yet, but should at least not
  // display weird stuff - Discordance need to address showing some additional
  // fields on shifters and maybe order of more rare fields.
  void show_char_to_char_1(CHAR_DATA *victim, CHAR_DATA *ch, int type, bool photo) {
    char buf[MAX_STRING_LENGTH], attractstring[MSL], dirtystring[MSL];
    OBJ_DATA *obj;
    int i;
    int pointer = 0;
    char string[MSL];

    if (victim->in_room == NULL)
    return;

    // Reorganized and added new categories to be more accurate; needs to be a separate function at some point - Discordance
    char *const age_names[] = {
      "", "late teens", "early twenties", "mid-twenties", "late twenties", "early thirties", "mid-thirties", "late thirties", "early forties", "mid-forties", "late forties", "early fifties", "mid-fifties", "late fifties", "early sixties", "mid-sixties", "late sixties", "early seventies", "mid-seventies", "late seventies", "early eighties", "mid-eighties", "late eighties", "early nineties", "mid-nineties", "late nineties", "hundreds"
    };

    string[0] = '\0';

    // These are incase strcat is used prior to the variable being set -
    // Discordance
    strcpy(string, "");
    strcpy(buf, "");
    strcpy(attractstring, "");
    strcpy(dirtystring, "");

    // NPCs first instead of strewn without
    if (IS_NPC(victim)) {
      sprintf(buf, "%s\n\r", victim->long_descr);
      strcat(string, buf);
      strcat(string, victim->description);
      strcat(string, "\n\r");
      if(victim->faction != 0)
      {
        FACTION_TYPE *mfac = clan_lookup(victim->faction);
        if(mfac != NULL)
        {
          sprintf(buf, "This is a soldier of %s, wearing a %s symbol.\n\r", mfac->name, mfac->symbol);
          strcat(string, buf);
        }
      }
      if (victim->valuable == TRUE && photo == FALSE) {
        for (int i = 0; i < 33; i++) {
          if (victim->pIndexData->vnum == monster_table[i].vnum) {
            if (get_demon_lvl(i) <= get_skill(ch, SKILL_DEMONOLOGY) * 125) {
              sprintf(buf, "You think their %s will probably be worth something in an alchemical lab.\n\r", monster_table[i].object);
              strcat(string, buf);
            }
          }
        }
      }
      if (photo == FALSE)
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      else
      to_photo_message(ch, string);

    }
    // Storyrunners and Immortals unless they're looking at themselves
    else if (is_gm(victim) && ch != victim && photo == FALSE) {
      printf_to_char(ch, "They are the spirit known as %s.\n\r", victim->name);
      return;
    }
    else if (higher_power(victim) ) {
      printf_to_char(ch, "They are the spirit known as %s.\n\r", victim->name);
      return;
    }
    else if (is_animal(victim)) {
      show_animal_to_char(victim, ch, type, photo);
      return;
    }
    else if (is_cloaked(victim)) {
      if (photo == TRUE)
      return;
      remember_detail(ch, victim);
      send_to_char("This is very clearly a figure of average height, and approximately average build. Their eyes are about average, as is their height, they look to be of about average age and are definitely wearing an average outfit.\n\r", ch);
      return;
    }
    // everyone else
    else {
      if (ch->race == RACE_ANCIENTVAMPIRE || ch->race == RACE_OLDVAMPIRE) {
        if (get_skill(victim, SKILL_VIRGIN) >= 1 && victim->pcdata->virginity_lost == 0) {
          sprintf(buf, "%s's a `Wvirgin`x.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
      }
      pointer = get_age_descriptor(victim);
      string[0] = '\0';
      // Guest line
      if (IS_FLAG(victim->act, PLR_GUEST) && (get_guesttrust(ch, victim) < 1 || IS_IMMORTAL(ch)) && victim->pcdata->guest_type == GUEST_PLOT) {
        sprintf(buf, "(Guest of %s)\n\r", victim->pcdata->guest_of);
        strcat(string, buf);
      }
      // sets up intros depending on form
      strcpy(buf, "This is ");
      if (is_ghost(victim)) {
        strcat(buf, "the ghost of ");
      }
      if (is_masked(victim) && (victim->shape == SHAPE_HUMAN || victim->shape == SHAPE_MERMAID)) {
        strcat(buf, lowercase_clause(
        mask_intro(victim, victim->pcdata->maskednumber)));
      }
      else if (!IS_NPC(ch) && !IS_NPC(victim) && ch->pcdata->dream_identity_timer > 0 && safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)) > 1 && safe_strlen(ch->pcdata->identity_world) > 1 && safe_strlen(dream_detail(victim, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)) > 1)
      strcat(buf, lowercase_clause(dream_detail(
      victim, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)));
      else
      strcat(buf, lowercase_clause(get_intro(victim)));

      if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD) || (!IS_NPC(victim) && victim->pcdata->spectre == 2)) {
        strcat(string, buf);
        strcat(string, "\n");
        strcat(string, get_default_dreamdesc(victim));

        if (photo == FALSE) {
          page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
        }
        else {
          to_photo_message(ch, string);
        }

        return;
      }

      if (can_see_details(ch, victim)) {
        // addresses boobs and age category
        if (get_bust(victim) > -1 && !guestmonster(victim)) {
          if (is_masked(victim))
          sprintf(buf, "%s with %s", buf, cup_size[get_bust(victim)]);
          else
          sprintf(buf, "%s in %s %s with %s", buf, (victim->sex == SEX_MALE) ? "his" : "her", age_names[pointer], cup_size[get_bust(victim)]);
        }
        else if (!is_masked(victim) && !guestmonster(victim))
        sprintf(buf, "%s in %s %s", buf, (victim->sex == SEX_MALE) ? "his" : "her", age_names[pointer]);

        if (ch != victim) // && type == LOOK_GLANCE)
        {
          strcat(buf, " ");
          strcat(buf, relfacing[get_reldirection(victim->facing, ch->facing)]);
        }
        /*
        else if(type == LOOK_LOOK)
        {
        if(is_masked(victim))
        sprintf(buf, "%s standing at about %d'%d\"",  buf, get_height_feet(victim), get_height_inches(victim)); else sprintf(buf, "%s
        standing at about %d'%d\"", buf, get_height_feet(victim), get_height_inches(victim));
        }*/

        // addresses imprintable tag
        if (!is_masked(victim) && photo == FALSE) {
          if (seems_suggestible(victim) && seems_suffer_sensitive(victim)) {
            strcat(buf, ". (`WDesire`D&`rSuffering`x Sensitive`x)`x\n\r");
          }
          else if (seems_suggestible(victim)) {
            strcat(buf, ". (`WDesire Sensitive`x)`x\n\r");
          }
          else if (seems_suffer_sensitive(victim)) {
            strcat(buf, ". (`rSuffering Sensitive`x)`x\n\r");
          }
          else {
            strcat(buf, ".`x\n\r");
          }
        }
        else {
          strcat(buf, ".`x\n\r");
        }
        strcat(string, buf);
        // auras

        if (safe_strlen(victim->pcdata->maim) > 3) {
          sprintf(buf, "`R%s %s.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->maim);
          strcat(string, buf);
        }
        if (victim->modifier == MODIFIER_DISFIGURED && !is_masked(victim)) {
          sprintf(buf, "%s is terribly disfigured.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
        if (victim->modifier == MODIFIER_CRIPPLED) {
          sprintf(buf, "%s is seriously crippled.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
        if (victim->modifier == MODIFIER_INJURY) {
          sprintf(buf, "%s appears injured.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
        if (is_ill(victim) && victim->pcdata->lf_modifier <= -5) {
          sprintf(buf, "It looks like %s has a cold.`x\n\r", (victim->sex == SEX_MALE) ? "he" : "she");
          strcat(string, buf);
        }
        if (victim->modifier == MODIFIER_CATATONIC) {
          sprintf(buf, "%s moves with unusual lethargy.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
        if (victim->modifier == MODIFIER_UNLIVING) {
          sprintf(buf, "%s skin seems unnaturally smooth and pale.`x\n\r", (victim->sex == SEX_MALE) ? "His" : "Her");
          strcat(string, buf);
        }
        if (in_haven(ch->in_room) && can_shroud(ch) && event_cleanse != 1 && cardinal(victim) && sinmatch(ch, victim)) {
          sprintf(buf, "`rRed ethereal smoke is trailing from %s eyes.`x\n\r", (victim->sex == SEX_MALE) ? "his" : "her");
          strcat(string, buf);
        }
        if (ch->diminish_lt > 0 && ch->diminish_lt < 100 && ch->diminish_till > current_time) {
          if (ch->diminish_vis == 1 || (ch->diminish_vis == 3 && in_fight(ch))) {
            sprintf(buf, "`WThey seem to only be fighting at around %d percent effectiveness.`x\n\r", (ch->diminish_lt / 10) * 10);
            strcat(string, buf);
          }
        }

        if (in_haven(ch->in_room) && photo == FALSE && can_shroud(ch) && event_cleanse != 1 && !seems_under_understanding(victim, ch) && !seems_under_limited(victim, ch) && !guestmonster(victim) && !higher_power(victim)) {
          sprintf(buf, "%s aura has a faint `rred`x ", (victim->sex == SEX_MALE) ? "His" : "Her");
          strcat(string, buf);

          if (can_shroud(ch) && event_cleanse != 1 && (!seems_super(victim) || get_skill(victim, SKILL_OBFUSCATE) > 0) && newbie_level(victim) < 10) {
            if (newbie_level(victim) == 1)
            sprintf(buf, "and `ggreen`x glow.\n\r");
            else if (newbie_level(victim) == 2)
            sprintf(buf, "and `cblue-green`x glow.\n\r");
            else if (newbie_level(victim) == 3)
            sprintf(buf, "and `Bblue`x glow.\n\r");

            strcat(string, buf);
          }
          else {
            strcat(string, "glow.\n\r");
          }
        }
        else if (in_haven(ch->in_room) && photo == FALSE && can_shroud(ch) && event_cleanse != 1 && !seems_under_understanding(victim, ch) && seems_under_limited(victim, ch) && !guestmonster(victim) && !higher_power(victim)) {
          sprintf(buf, "%s aura has a faint `yorange`x ", (victim->sex == SEX_MALE) ? "His" : "Her");
          strcat(string, buf);

          if (can_shroud(ch) && event_cleanse != 1 && photo == FALSE && (!seems_super(victim) || get_skill(victim, SKILL_OBFUSCATE) > 0) && newbie_level(victim) < 10) {
            if (newbie_level(victim) == 1)
            sprintf(buf, "and `ggreen`x glow.\n\r");
            else if (newbie_level(victim) == 2)
            sprintf(buf, "and `cblue-green`x glow.\n\r");
            else if (newbie_level(victim) == 3)
            sprintf(buf, "and `Bblue`x glow.\n\r");

            strcat(string, buf);
          }
          else {
            strcat(string, "glow.\n\r");
          }
        }

        else if (in_haven(ch->in_room) && can_shroud(ch) && photo == FALSE && event_cleanse != 1 && (!seems_super(victim) || get_skill(victim, SKILL_OBFUSCATE) > 0) && newbie_level(victim) < 10 && !guestmonster(victim) && !higher_power(victim)) {
          if (newbie_level(victim) == 1)
          sprintf(buf, "%s aura has a faint `ggreen`x glow.\n\r", (victim->sex == SEX_MALE) ? "His" : "Her");
          else if (newbie_level(victim) == 2)
          sprintf(buf, "%s aura has a faint `cblue-green`x glow.\n\r", (victim->sex == SEX_MALE) ? "His" : "Her");
          else if (newbie_level(victim) == 3)
          sprintf(buf, "%s aura has a faint `Bblue`x glow.\n\r", (victim->sex == SEX_MALE) ? "His" : "Her");

          strcat(string, buf);
        }
        if (photo == FALSE && (victim->shape == SHAPE_HUMAN || victim->shape == SHAPE_MERMAID)) {
          sprintf(attractstring, "You would judge %s to be a %s%0.1f`x out of 10", (victim->sex == SEX_MALE) ? "him" : "her", attract_color(get_attract(victim, ch)), (float)get_attract(victim, ch) / 10);
        }
        else if(photo == TRUE && !IS_NPC(victim) && victim->pcdata->photo_attract > 0)
        {
          sprintf(attractstring, "You would judge %s to be a %s%0.1f`x out of 10", (victim->sex == SEX_MALE) ? "him" : "her", attract_color(victim->pcdata->photo_attract), (float)victim->pcdata->photo_attract / 10);
        }

        // this handles dirty and bloody lines
        if (victim->pcdata->blood[0] >= 100 || victim->pcdata->dirt[0] >= 200) {
          // adds conjunction for attractiveness
          if (victim->sex == SEX_MALE)
          strcat(dirtystring, " and he ");
          else
          strcat(dirtystring, " and she ");
          if (victim->pcdata->blood[0] >= 100) {
            if (victim->pcdata->blood[0] >= 300) {
              if (victim->pcdata->dirt[0] >= 400)
              strcat(dirtystring, "is drenched in `rblood`x and very `ydirty`x");
              else if (victim->pcdata->dirt[0] >= 300)
              strcat(dirtystring, "is drenched in `rblood`x and `ydirty`x");
              else if (victim->pcdata->dirt[0] >= 200)
              strcat(dirtystring, "is drenched in `rblood`x and a little `ydirty`x");
              else
              strcat(dirtystring, "is drenched in `rblood`x");
            }
            else if (victim->pcdata->blood[0] >= 200) {
              if (victim->pcdata->dirt[0] >= 400)
              strcat(dirtystring, "is spattered with `rblood`x and very `ydirty`x");
              else if (victim->pcdata->dirt[0] >= 300)
              strcat(dirtystring, "is spattered with `rblood`x and `ydirty`x");
              else if (victim->pcdata->dirt[0] >= 200)
              strcat(dirtystring, "is spattered with `rblood`x and a little `ydirty`x");
              else
              strcat(dirtystring, "is spattered with `rblood`x");
            }
            else {
              if (victim->pcdata->dirt[0] >= 400)
              strcat(dirtystring, "is lightly spattered with `rblood`x and very `ydirty`x");
              else if (victim->pcdata->dirt[0] >= 300)
              strcat(dirtystring, "is lightly spattered with `rblood`x and `ydirty`x");
              else if (victim->pcdata->dirt[0] >= 200)
              strcat(
              dirtystring, "is lightly spattered with `rblood`x and a little `ydirty`x");
              else
              strcat(dirtystring, "is lightly spattered with `rblood`x");
            }
          }
          else {
            if (victim->pcdata->dirt[0] >= 400)
            strcat(dirtystring, "is very `ydirty`x");
            else if (victim->pcdata->dirt[0] >= 300)
            strcat(dirtystring, "is `ydirty`x");
            else if (victim->pcdata->dirt[0] >= 200)
            strcat(dirtystring, "is a little `ydirty`x");
          }
          strcat(dirtystring, ".\n\r");
        }
        else {
          strcat(attractstring, ".\n\r");
        }

        if (!is_animal(victim) && !is_hybrid(victim) && !is_masked(victim) && !guestmonster(victim))
        sprintf(buf, "%s%s", attractstring, dirtystring);
        else
        sprintf(buf, "%s", dirtystring);

        strcat(string, buf);

        if (!is_masked(victim) && !guestmonster(victim) && photo == FALSE && !higher_power(victim)) {
          if (victim->played / 3600 < 50) {
            sprintf(buf, "They seem like a newcomer to Haven.\n\r");
            strcat(string, buf);
          }
          else if (social_standing(victim) >= 75) {
            sprintf(buf, "They seem %s.\n\r", lower_standing(victim, social_standing(victim)));
            strcat(string, buf);
          }
        }

        if (!IS_NPC(victim) && victim->pcdata->guard_number > 0 && victim->pcdata->guard_expiration > current_time) {
          if (victim->pcdata->guard_number == 1)
          sprintf(buf, "`DA bodyguard is rarely far away from %s.`x ", (victim->sex == SEX_MALE) ? "him" : "her");
          else if (victim->pcdata->guard_number == 2)
          sprintf(buf, "`DA pair of bodyguards are rarely far away from %s.`x ", (victim->sex == SEX_MALE) ? "him" : "her");
          else
          sprintf(buf, "`D%s bodyguards are rarely far away from %s.`x ", number_to_text(victim->pcdata->guard_number), (victim->sex == SEX_MALE) ? "him" : "her");
          strcat(string, buf);
        }

        // if(type == LOOK_GLANCE)
        //{
        if (is_masked(victim))
        sprintf(buf, "%s is %d'%d\" and has ", (victim->sex == SEX_MALE) ? "He" : "She", get_height_feet(victim), get_height_inches(victim));
        else
        sprintf(buf, "%s is %d'%d\" and has ", (victim->sex == SEX_MALE) ? "He" : "She", get_height_feet(victim), get_height_inches(victim));

        strcat(string, buf);
        //}

        if (!all_covered(victim) && !is_animal(victim))
        //&& type == LOOK_GLANCE)
        {
          // combined hair, eyes, and skin into one line - Discordance
          if (IS_AFFECTED(victim, AFF_BURSTVESSEL) && !is_covered(victim, COVERS_EYES)) {
            if (is_possessed(victim) && photo == FALSE && ch != victim && (get_skill(ch, SKILL_CLAIRVOYANCE) >= 1 || get_skill(ch, SKILL_PERCEPTION) >= 2))
            sprintf(buf, "%s, %s, and pale bloodshot %s. ", get_skin(ch, victim), get_hair(ch, victim), get_eyes(ch, victim));
            else
            sprintf(buf, "%s, %s, and bloodshot %s. ", get_skin(ch, victim), get_hair(ch, victim), get_eyes(ch, victim));
          }
          else {
            if (is_possessed(victim) && ch != victim && photo == FALSE && !is_covered(victim, COVERS_EYES) && (get_skill(ch, SKILL_CLAIRVOYANCE) >= 1 || get_skill(ch, SKILL_PERCEPTION) >= 2))
            sprintf(buf, "%s, %s, and pale %s. ", get_skin(ch, victim), get_hair(ch, victim), get_eyes(ch, victim));
            else
            sprintf(buf, "%s, %s, and %s. ", get_skin(ch, victim), get_hair(ch, victim), get_eyes(ch, victim));
          }

          strcat(string, buf);

          if (get_skill(ch, SKILL_PERCEPTION) >= 3 && photo == FALSE && !IS_NPC(victim)) {
            if (victim->pcdata->attract[ATTRACT_MAKEUP] == 1)
            sprintf(buf, "\n%s is wearing a small amount of makeup.", (victim->sex == SEX_MALE) ? "He" : "She");
            else if (victim->pcdata->attract[ATTRACT_MAKEUP] == 2)
            sprintf(buf, "\n%s is wearing a moderate amount of makeup.", (victim->sex == SEX_MALE) ? "He" : "She");
            else if (victim->pcdata->attract[ATTRACT_MAKEUP] == 3)
            sprintf(buf, "\n%s is wearing a lot of makeup.", (victim->sex == SEX_MALE) ? "He" : "She");

            if (victim->pcdata->attract[ATTRACT_MAKEUP] >= 1 && victim->pcdata->attract[ATTRACT_MAKEUP] <= 3)
            strcat(string, buf);
          }
          strcat(string, "\n\r");
        }
        if (guestmonster(victim) && safe_strlen(victim->pcdata->dream_origin) > 1) {
          for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
          it != FantasyVect.end(); ++it) {
            if ((*it)->valid == FALSE)
            continue;
            if (!str_cmp(victim->pcdata->dream_origin, (*it)->name)) {
              for (int i = 0; i < 200; i++) {
                if (!str_cmp(ch->name, (*it)->participants[i]) && (*it)->participant_exp[i] >= 100) {
                  sprintf(buf, "You recognize it from the world of %s.\n\r", (*it)->name);
                  strcat(string, buf);
                }
              }
            }
          }
        }
        if (victim->race == RACE_DREAMCHILD && photo == FALSE && safe_strlen(victim->pcdata->dream_origin) > 1) {
          for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
          it != FantasyVect.end(); ++it) {
            if ((*it)->valid == FALSE)
            continue;
            if (!str_cmp(victim->pcdata->dream_origin, (*it)->name)) {
              for (int i = 0; i < 200; i++) {
                if (!str_cmp(ch->name, (*it)->participants[i]) && (*it)->participant_exp[i] >= 100) {
                  for (int j = 0; j < 200; j++) {
                    if (!str_cmp(victim->name, (*it)->participants[j])) {
                      if (safe_strlen((*it)->participant_fames[j]) < 2)
                      sprintf(buf, "%s reminds you of the world of %s.\n\r", (victim->sex == SEX_MALE) ? "He" : "She", (*it)->name);
                      else
                      sprintf(buf, "%s is known for %s(%s).\n\r", (victim->sex == SEX_MALE) ? "He" : "She", (*it)->participant_fames[j], (*it)->name);
                      strcat(string, buf);
                    }
                  }
                }
              }
            }
          }
        }
        if(photo == FALSE)
        {
          if (!is_masked(victim) && !guestmonster(victim)) {
            if ((local_fame_level(victim) > 0 && can_see_local_fame(ch) && photo == FALSE) || get_skill(victim, SKILL_FAMOUS) > 0) {
              if (ch->race != RACE_FACULTY) {
                sprintf(buf, "%s ", fame_insert(victim));
                strcat(string, buf);
              }
            }
            else if (can_see_local_fame(ch)) {
              if (victim->pcdata->job_type_one == JOB_FULLEMPLOY) {
                PROP_TYPE *workplace =
                prop_from_room(get_room_index(victim->pcdata->job_room_one));
                if (workplace != NULL && workplace->type == PROP_SHOP) {
                  sprintf(buf, "%s is known for working at %s as %s%s. ", (victim->sex == SEX_MALE) ? "He" : "She", from_color(workplace->propname), smart_a_or_an(victim->pcdata->job_title_one), victim->pcdata->job_title_one);
                }
                else {
                  sprintf(buf, "%s is known for working from home as %s%s. ", (victim->sex == SEX_MALE) ? "He" : "She", smart_a_or_an(victim->pcdata->job_title_one), victim->pcdata->job_title_one);
                }
                strcat(string, buf);
              }
              else if (victim->race == RACE_FACULTY && !institute_room(victim->in_room) && safe_strlen(victim->pcdata->job_title_one) > 1) {
                sprintf(buf, "%s is known for working in the institute as %s%s. ", (victim->sex == SEX_MALE) ? "He" : "She", smart_a_or_an(victim->pcdata->job_title_one), victim->pcdata->job_title_one);
                strcat(string, buf);
              }
            }
            if (victim->pcdata->wilds_fame_level > 0 && safe_strlen(ch->pcdata->wilds_fame) > 2 && (in_world(ch) == WORLD_WILDS || ch->pcdata->wilds_fame_level > 0 || ch->pcdata->wexp >= 250)) {
              sprintf(buf, "%s ", wilds_fame_insert(victim));
              strcat(string, buf);
            }
            if (victim->pcdata->other_fame_level > 0 && safe_strlen(ch->pcdata->other_fame) > 2 && (in_world(ch) == WORLD_OTHER || ch->pcdata->other_fame_level > 0 || ch->pcdata->oexp >= 250)) {
              sprintf(buf, "%s ", other_fame_insert(victim));
              strcat(string, buf);
            }
            if (victim->pcdata->godrealm_fame_level > 0 && safe_strlen(ch->pcdata->godrealm_fame) > 2 && (in_world(ch) == WORLD_GODREALM || ch->pcdata->godrealm_fame_level > 0 || ch->pcdata->gexp >= 250)) {
              sprintf(buf, "%s ", godrealm_fame_insert(victim));
              strcat(string, buf);
            }
            if (victim->pcdata->hell_fame_level > 0 && safe_strlen(ch->pcdata->hell_fame) > 2 && (in_world(ch) == WORLD_HELL || ch->pcdata->hell_fame_level > 0 || ch->pcdata->hexp >= 250)) {
              sprintf(buf, "%s ", hell_fame_insert(victim));
              strcat(string, buf);
            }
            if ((ch->pcdata->hexp >= 250 || ch->faction != 0) && get_loc("District 82")->base_faction_core != 0 && clan_lookup(get_loc("District 82")->base_faction_core) != NULL && clan_lookup(get_loc("District 82")->base_faction_core)->stasis ==
                0 && !str_cmp(victim->name, clan_lookup(get_loc("District 82")->base_faction_core)
                  ->leader)) {
              sprintf(buf, "\n\r%s is a border lord of `088Hell`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->hexp >= 250 || ch->faction != 0) && get_loc("District 82")->base_faction_sect != 0 && clan_lookup(get_loc("District 82")->base_faction_sect) != NULL && clan_lookup(get_loc("District 82")->base_faction_sect)->stasis ==
                0 && !str_cmp(victim->name, clan_lookup(get_loc("District 82")->base_faction_sect)
                  ->leader)) {
              sprintf(buf, "\n\r%s is a border lord of `088Hell`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->hexp >= 250 || ch->faction != 0) && get_loc("District 82")->base_faction_cult != 0 && clan_lookup(get_loc("District 82")->base_faction_cult) != NULL && clan_lookup(get_loc("District 82")->base_faction_cult)->stasis ==
                0 && !str_cmp(victim->name, clan_lookup(get_loc("District 82")->base_faction_cult)
                  ->leader)) {
              sprintf(buf, "\n\r%s is a border lord of `088Hell`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }

            if ((ch->pcdata->oexp >= 250 || ch->faction != 0) && get_loc("Lauriea")->base_faction_core != 0 && clan_lookup(get_loc("Lauriea")->base_faction_core) != NULL && clan_lookup(get_loc("Lauriea")->base_faction_core)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Lauriea")->base_faction_core)->leader)) {
              sprintf(buf, "\n\r%s is a border lord of the `114Fae`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->oexp >= 250 || ch->faction != 0) && get_loc("Lauriea")->base_faction_sect != 0 && clan_lookup(get_loc("Lauriea")->base_faction_sect) != NULL && clan_lookup(get_loc("Lauriea")->base_faction_sect)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Lauriea")->base_faction_sect)->leader)) {
              sprintf(buf, "\n\r%s is a border lord of the `114Fae`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->oexp >= 250 || ch->faction != 0) && get_loc("Lauriea")->base_faction_cult != 0 && clan_lookup(get_loc("Lauriea")->base_faction_cult) != NULL && clan_lookup(get_loc("Lauriea")->base_faction_cult)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Lauriea")->base_faction_cult)->leader)) {
              sprintf(buf, "\n\r%s is a border lord of the `114Fae`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }

            if ((ch->pcdata->gexp >= 250 || ch->faction != 0) && get_loc("Rhagost")->base_faction_core != 0 && clan_lookup(get_loc("Rhagost")->base_faction_core) != NULL && clan_lookup(get_loc("Rhagost")->base_faction_core)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Rhagost")->base_faction_core)->leader)) {
              sprintf(buf, "\n\r%s is a border lord of the `152Godrealms`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->gexp >= 250 || ch->faction != 0) && get_loc("Rhagost")->base_faction_sect != 0 && clan_lookup(get_loc("Rhagost")->base_faction_sect) != NULL && clan_lookup(get_loc("Rhagost")->base_faction_sect)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Rhagost")->base_faction_sect)->leader)) {
              sprintf(buf, "\n\r%s is a border lord of the `152Godrealms`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->gexp >= 250 || ch->faction != 0) && get_loc("Rhagost")->base_faction_cult != 0 && clan_lookup(get_loc("Rhagost")->base_faction_cult) != NULL && clan_lookup(get_loc("Rhagost")->base_faction_cult)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Rhagost")->base_faction_cult)->leader)) {
              sprintf(buf, "\n\r%s is a border lord of the `152Godrealms`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }

            if ((ch->pcdata->wexp >= 250 || ch->faction != 0) && get_loc("Navorost")->base_faction_core != 0 && clan_lookup(get_loc("Navorost")->base_faction_core) != NULL && clan_lookup(get_loc("Navorost")->base_faction_core)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Navorost")->base_faction_core)->leader)) {
              sprintf(
              buf, "\n\r%s is a border lord of the `226W`227i`228l`227d`226s`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->wexp >= 250 || ch->faction != 0) && get_loc("Navorost")->base_faction_sect != 0 && clan_lookup(get_loc("Navorost")->base_faction_sect) != NULL && clan_lookup(get_loc("Navorost")->base_faction_sect)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Navorost")->base_faction_sect)->leader)) {
              sprintf(
              buf, "\n\r%s is a border lord of the `226W`227i`228l`227d`226s`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            if ((ch->pcdata->wexp >= 250 || ch->faction != 0) && get_loc("Navorost")->base_faction_cult != 0 && clan_lookup(get_loc("Navorost")->base_faction_cult) != NULL && clan_lookup(get_loc("Navorost")->base_faction_cult)->stasis == 0 && !str_cmp(
                  victim->name, clan_lookup(get_loc("Navorost")->base_faction_cult)->leader)) {
              sprintf(
              buf, "\n\r%s is a border lord of the `226W`227i`228l`227d`226s`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }

            if (IS_FLAG(victim->act, PLR_VILLAIN)) {
              sprintf(buf, "People nearby seem to be acting cautiously, as if afraid of %s.`x\n\r", (victim->sex == SEX_MALE) ? "him" : "her");
              strcat(string, buf);
            }
            if (has_weakness(ch, victim) && (is_gm(ch) || ch->faction != 0)) {
              sprintf(buf, "%s is rumored to be psychically protected.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }

            if (victim->race == RACE_BROWN && ch->race == RACE_BROWN) {
              sprintf(buf, "You think you recognize them from campus.\n\r");
              strcat(string, buf);
            }
          }
          // pregnancy descriptions
          if (victim->pcdata->impregnated != 0 || victim->pcdata->egg_daddy != 0)
          //&& type == LOOK_GLANCE)
          {
            int max_length = get_pregnancy_minlength(victim) + 3;
            int weeks_early, weeks_mid, weeks_late;
            weeks_early = max_length / 4;
            weeks_mid = (max_length / 4) * 2;
            weeks_late = (max_length / 4) * 3;
            if (victim->pcdata->impregnated + (3600 * 24 * 7 * weeks_late) <
                current_time) {
              sprintf(buf, "%s is in the late stages of pregnancy.`x ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            else if (victim->pcdata->impregnated + (3600 * 24 * 7 * weeks_mid) <
                current_time) {
              sprintf(buf, "%s is in the middle stages of pregnancy.`x ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }
            else if (victim->pcdata->impregnated + (3600 * 24 * 7 * weeks_early) <
                current_time) {
              bool covered = FALSE;
              for (i = 0; i < 25; i++) {
                if ((obj = get_eq_char(victim, i)) != NULL) {
                  if (does_cover(obj, COVERS_LOWER_CHEST)) {
                    covered = TRUE;
                  }
                }
              }
              if (covered == FALSE) {
                sprintf(buf, "%s is in the early stages of pregnancy.`x ", (victim->sex == SEX_MALE) ? "He" : "She");
                strcat(string, buf);
              }
            }
          }
        }
        // marks
        for (i = 0; i < 4; i++) {
          if (!IS_NPC(victim) && victim->pcdata->mark_timer[i] > 0 && safe_strlen(victim->pcdata->mark[i]) > 2) {
            sprintf(buf, "%s ", victim->pcdata->mark[i]);
            strcat(string, buf);
          }
        }
        if (IS_FLAG(victim->affected_by, AFF_STAKED)) {
          sprintf(buf, "A stake has been lodged in %s heart. ", (victim->sex == SEX_MALE) ? "his" : "her");
          strcat(string, buf);
        }

        // critically wounded
        if (victim->wounds == 3) {
          sprintf(buf, "`RThey are bleeding to death. `x ");
          strcat(string, buf);
        }
        if (!str_cmp(victim->name, ch->pcdata->enthralled) && (has_blackeyes(victim) || !is_abom(victim))) {
          sprintf(buf, "`RThey are your master. `x ");
          strcat(string, buf);
        }
        if (!str_cmp(victim->name, ch->pcdata->enraptured) && (has_blackeyes(victim) || !is_abom(victim))) {
          sprintf(buf, "`RThey are your adored. `x ");
          strcat(string, buf);
        }

        if (ch->recent_moved <= -150 && victim->recent_moved <= -150 && victim->wounds > 0 && victim->wounds < 3) {
          if (get_skill(ch, SKILL_MEDICINE) >= 4) {
            if (victim->wounds == 1)
            sprintf(buf, "They seem mildly injured, recovery will probably take %d hours.\n\r", victim->heal_timer / 1440);
            else if (victim->wounds == 2)
            sprintf(buf, "They seem severely injured, recovery will probably take %d hours.\n\r", victim->heal_timer / 1440);
            strcat(string, buf);
          }
          else if (get_skill(ch, SKILL_MEDICINE) >= 3) {
            if (victim->wounds == 1)
            sprintf(buf, "They seem mildly injured.\n\r");
            else if (victim->wounds == 2)
            sprintf(buf, "They seem severely injured.\n\r");
            strcat(string, buf);
          }
        }

        if (get_tier(ch) >= 3 && get_tier(victim) < 3 && photo == FALSE && victim->skills[SKILL_MENTALDISCIPLINE] <= 0 && victim->pcdata->attract[ATTRACT_PROM] < 100) {
          if (alt_count(victim) <= 1 && victim->lf_taken <= 25 && (victim->pcdata->intel >= 2500 || victim->pcdata->heroic >= 50)) {
            sprintf(buf, "`161They look tasty.`x\n\r");
            strcat(string, buf);
          }
        }

        if(photo == FALSE && !str_cmp(ch->pcdata->fixation_name, victim->name) && ch->pcdata->fixation_timeout > 0)
        {
          sprintf(buf, "You are fixed on them to level %d(%d charges left).\n\r", ch->pcdata->fixation_level, ch->pcdata->fixation_charges);
          strcat(string, buf);
        }

        if (get_skill(ch, SKILL_SENSEWEAKNESS) > 0 && photo == FALSE) {
          if (IS_AFFECTED(victim, AFF_WEAKEN) || IS_AFFECTED(victim, AFF_PENALTY)) {
            sprintf(buf, "You sense a mystical weakness. ");
            strcat(string, buf);
          }
          if (victim->wounds == 1) {
            sprintf(buf, "You sense a faint weakness caused by injury. ");
            strcat(string, buf);
          }
          if (victim->wounds >= 2) {
            sprintf(buf, "You sense a potent weakness caused by injury. ");
            strcat(string, buf);
          }
          if (is_ill(victim)) {
            sprintf(buf, "You sense a weakness cause by illness. ");
            strcat(string, buf);
          }
          if (victim->lf_taken >= 500) {
            sprintf(buf, "You sense a weakness cause by trauma. ");
            strcat(string, buf);
          }
          if (is_weakness(ch, victim)) {
            sprintf(buf, "You sense they are a psychological weakness. ");
            strcat(string, buf);
          }
          if (victim->pcdata->attract[ATTRACT_PROM] >= 1000) {
            sprintf(buf, "You sense that sex is their weakness. ");
            strcat(string, buf);
          }
        }
        if (IS_FLAG(victim->comm, COMM_BLINDFOLD)) {
          sprintf(buf, "`D%s`D is blindfolded. `x\n\r", PERS(victim, ch));
          strcat(string, buf);
        }
        if (IS_FLAG(victim->comm, COMM_GAG)) {
          sprintf(buf, "`D%s`D is gagged. `x\n\r", PERS(victim, ch));
          strcat(string, buf);
        }
        if (IS_FLAG(victim->act, PLR_BOUND) && IS_FLAG(victim->act, PLR_BOUNDFEET)) {
          sprintf(buf, "`c%s`c is bound hand and foot. `x\n\r", PERS(victim, ch));
          strcat(string, buf);
        }
        if (IS_FLAG(victim->act, PLR_BOUND) && !IS_FLAG(victim->act, PLR_BOUNDFEET)) {
          sprintf(buf, "`c%s's`c hands are bound. `x\n\r", PERS(victim, ch));
          strcat(string, buf);
        }
        if (!IS_FLAG(victim->act, PLR_BOUND) && IS_FLAG(victim->act, PLR_BOUNDFEET)) {
          sprintf(buf, "`c%s`c is shackled. `x\n\r", PERS(victim, ch));
          strcat(string, buf);
        }

        if (is_possessed(victim) && ch != victim && photo == FALSE && (get_skill(ch, SKILL_CLAIRVOYANCE) >= 2 || IS_FLAG(get_possesser(victim)->comm, COMM_PRIVATE)) && ch != victim) {
          sprintf(buf, "You see `W%s`x inside them. ", PERS(get_possesser(victim), ch));
          strcat(string, buf);
        }

        // shroud desc
        if ((victim->shape == SHAPE_HUMAN || victim->shape == SHAPE_MERMAID)) {
          if (IS_FLAG(victim->act, PLR_SHROUD) && !guestmonster(victim)) {
            sprintf(buf, "%s  ", shroud_desc(victim));
            strcat(string, buf);
          }
        }

        if (is_smelly(victim) && photo == FALSE) {
          sprintf(buf, "%s is giving off quite a `ystench`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }

        // wetness lines
        if (victim->pcdata->wetness > 35) {
          sprintf(buf, "%s is soaking `Bwet`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
        else if (victim->pcdata->wetness > 15) {
          sprintf(buf, "%s is `Bwet`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }
        else if (victim->pcdata->wetness > 0) {
          sprintf(buf, "%s is damp`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
          strcat(string, buf);
        }

        // Know supernatural stat lines
        if (!is_animal(victim) && photo == FALSE) {
          if (ch != victim && is_vampire(ch) && biteable(ch, victim, FALSE)) {
            sprintf(buf, "`r%s is biteable.`x ", (victim->sex == SEX_MALE) ? "He" : "She");
            strcat(string, buf);
          }
          if (get_skill(ch, SKILL_KNOWVAMP) > 0 && seems_vampire(victim) && get_skill(victim, SKILL_OBFUSCATE) == 0) {
            sprintf(buf, "%s is a `Dvampire`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
            strcat(string, buf);
          }
          if(college_staff(ch, FALSE) && college_student(victim, FALSE))
          {
            sprintf(buf, "This is %s %s %s. ", (victim->sex == SEX_MALE) ? "Mr" : "Miss", victim->name, victim->pcdata->last_name);
            strcat(string, buf);
          }
          else if(college_staff(victim, FALSE) && college_student(ch, FALSE))
          {
            sprintf(buf, "This is %s %s %s. ", (victim->sex == SEX_MALE) ? "Mr" : "Miss", victim->name, victim->pcdata->last_name);
            strcat(string, buf);
          }
          else if(clinic_staff(ch, FALSE) && clinic_patient(victim))
          {
            sprintf(buf, "This is %s %s %s. ", (victim->sex == SEX_MALE) ? "Mr" : "Miss", victim->name, victim->pcdata->last_name);
            strcat(string, buf);
          }
          else if(clinic_staff(victim, FALSE) && clinic_patient(ch))
          {
            sprintf(buf, "This is %s %s %s. ", (victim->sex == SEX_MALE) ? "Mr" : "Miss", victim->name, victim->pcdata->last_name);
            strcat(string, buf);
          }


          if(photo == FALSE)
          {

            if(college_staff(ch, FALSE) && college_student(victim, FALSE) && get_skill(victim, SKILL_ENDOWMENT) > 0)
            {
              sprintf(buf, "%s family has made an endowment to the institute. ", (victim->sex == SEX_MALE) ? "His" : "Her");
              strcat(string, buf);
            }
            if (get_skill(ch, SKILL_KNOWWERE) > 0 && seems_werewolf(victim) && get_skill(victim, SKILL_OBFUSCATE) == 0) {
              sprintf(buf, "%s is a `ywerewolf`x. ", (victim->sex == SEX_MALE) ? "He" : "She");
              strcat(string, buf);
            }

            if (get_skill(ch, SKILL_KNOWMAGE) > 0 && seems_super(victim) && !IS_FLAG(victim->act, PLR_SHROUD)) {
              sprintf(buf, "%s ", shroud_desc(victim));
              strcat(string, buf);
            }
            else if (get_skill(ch, SKILL_TOUCHED) > 0 && !is_super(ch) && seems_super(victim) && !IS_AFFECTED(victim, AFF_SEENSUPER) && number_percent() % 13 == 0) {
              if (number_percent() % 2 == 0 || (under_understanding(victim, ch) && number_percent() % 7 != 1))
              sprintf(buf, "%s ", shroud_desc(victim));
              else
              sprintf(buf, "%s ", shroud_desc(random_person()));

              strcat(string, buf);
              AFFECT_DATA af;
              af.where = TO_AFFECTS;
              af.type = 0;
              af.level = 10;
              af.duration = 12 * 60 * 4;
              af.location = APPLY_NONE;
              af.modifier = 0;
              af.caster = NULL;
              af.weave = FALSE;
              af.bitvector = AFF_SEENSUPER;
              affect_to_char(ch, &af);
            }
          }
        }
        // body locations line
        if (!is_animal(victim) && !guestmonster(victim)) {
          sprintf(buf, "You can see %s%s ", (victim->sex == SEX_MALE) ? "his" : "her", get_exposed_locations(victim));
          strcat(string, buf);

          if (safe_strlen(get_detailed_locations(victim)) > 3) {
            sprintf(buf, "More detail is visible for %s%s ", (victim->sex == SEX_MALE) ? "his" : "her", get_detailed_locations(victim));
            strcat(string, buf);
          }

          if (is_vampire(victim) && base_lifeforce(victim) <= 8000) {
            sprintf(buf, "%s seems gaunt and unhealthy.", (victim->sex == SEX_MALE) ? "He" : "She");
            strcat(string, buf);
          }
          if (!IS_NPC(victim) && victim->pcdata->ill_time > current_time) {
            sprintf(buf, "%s seems gaunt and unhealthy.", (victim->sex == SEX_MALE) ? "He" : "She");
            strcat(string, buf);
          }
        }

        if (!IS_NPC(victim) && victim->race == RACE_WIGHT) {
          if (victim->pcdata->decay_stage == 1) {
            sprintf(buf, "%s seems pale.", (victim->sex == SEX_MALE) ? "He" : "She");
            strcat(string, buf);
          }
          else if (victim->pcdata->decay_stage > 1) {
            sprintf(buf, "%s seems gaunt and unhealthy", (victim->sex == SEX_MALE) ? "He" : "She");
            strcat(string, buf);

            if (victim->pcdata->decay_stage >= 2 && !is_covered(victim, COVERS_HANDS)) {
              sprintf(buf, ", `y%s hands are spotted with decaying flesh`x", (victim->sex == SEX_MALE) ? "his" : "her");
              strcat(string, buf);
            }
            if (victim->pcdata->decay_stage >= 3 && !is_covered(victim, COVERS_THIGHS)) {
              sprintf(buf, ", `y%s hands are spotted with decaying flesh`x", (victim->sex == SEX_MALE) ? "his" : "her");
              strcat(string, buf);
            }
            if (victim->pcdata->decay_stage >= 3 && !is_covered(victim, COVERS_UPPER_ARMS)) {
              sprintf(buf, ", `y%s arms are spotted with decaying flesh`x", (victim->sex == SEX_MALE) ? "his" : "her");
              strcat(string, buf);
            }
            if (victim->pcdata->decay_stage >= 4 && !is_covered(victim, COVERS_BREASTS)) {
              sprintf(buf, ", `y%s torso is spotted with decaying flesh`x", (victim->sex == SEX_MALE) ? "his" : "her");
              strcat(string, buf);
            }
            if (victim->pcdata->decay_stage >= 5 && !is_covered(victim, COVERS_LOWER_FACE)) {
              sprintf(buf, ", `y%s face is spotted with decaying flesh`x", (victim->sex == SEX_MALE) ? "his" : "her");
              strcat(string, buf);
            }
          }
        }
      }
      if (type == LOOK_LOOK) {
        strcat(string, "\n\n\r");
        if (!IS_NPC(ch) && !IS_NPC(victim) && ch->pcdata->dream_identity_timer > 0 && safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)) > 1 && safe_strlen(ch->pcdata->identity_world) > 1 && safe_strlen(dream_detail(victim, ch->pcdata->identity_world, DREAM_DETAIL_DESC)) > 1)
        strcat(string, dream_detail(victim, ch->pcdata->identity_world, DREAM_DETAIL_DESC));
        else
        strcat(string, get_focused(ch, victim, FALSE));
      }

      // equipment and clothing
      strcat(string, equip_string(ch, victim));

      // BOXINGCODE
      // log_string("BOXING");
      //  page_to_char(text_block_to_box(wrap_string(string, get_wordwrap(ch)-4), //  "`c|`x ", get_wordwrap(ch)), ch);

      // char lstring[MSL];
      // sprintf(lstring, "slen: %ld", safe_strlen(string));
      // log_string(lstring);

      if (photo == FALSE)
      page_to_char(string, ch);
      //      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      else
      to_photo_message(ch, string);
      return;
    }
  }

  void introduce_char_to_char(CHAR_DATA *victim, CHAR_DATA *ch) {
    char buf[MAX_STRING_LENGTH], attractstring[MSL], dirtystring[MSL];
    int pointer = 0;
    char string[MSL];

    if (victim->in_room == NULL)
    return;

    // Reorganized and added new categories to be more accurate; needs to be a separate function at some point - Discordance
    char *const age_names[] = {
      "", "late teens",
      "early twenties", "mid-twenties", "late twenties",
      "early thirties", "mid-thirties", "late thirties",
      "early forties", "mid-forties", "late forties",
      "early fifties", "mid-fifties", "late fifties",
      "early sixties", "mid-sixties", "late sixties",
      "early seventies", "mid-seventies", "late seventies",
      "early eighties", "mid-eighties", "late eighties",
      "early nineties", "mid-nineties", "late nineties",
      "hundreds"
    };

    // These are incase strcat is used prior to the variable being set -
    // Discordance
    strcpy(string, "");
    strcpy(buf, "");
    strcpy(attractstring, "");
    strcpy(dirtystring, "");

    pointer = get_age_descriptor(victim);

    string[0] = '\0';

    strcat(buf, lowercase_clause(victim->pcdata->intro_desc));

    if (get_bust(victim) > -1 && !guestmonster(victim)) {
      if (is_masked(victim))
      sprintf(buf, "%s with %s", buf, cup_size[get_bust(victim)]);
      else
      sprintf(buf, "%s in %s %s with %s", buf, (victim->sex == SEX_MALE) ? "his" : "her", age_names[pointer], cup_size[get_bust(victim)]);
    }
    else if (!is_masked(victim) && !guestmonster(victim))
    sprintf(buf, "%s in %s %s", buf, (victim->sex == SEX_MALE) ? "his" : "her", age_names[pointer]);

    if (seems_suggestible(victim) && seems_suffer_sensitive(victim)) {
      strcat(buf, ". (`WDesire`D&`rSuffering`x Sensitive`x)`x\n\r");
    }
    else if (seems_suggestible(victim)) {
      strcat(buf, ". (`WDesire Sensitive`x)`x\n\r");
    }
    else if (seems_suffer_sensitive(victim)) {
      strcat(buf, ". (`rSuffering Sensitive`x)`x\n\r");
    }
    else {
      strcat(buf, ".`x\n\r");
    }
    strcat(string, buf);
    // auras

    if (safe_strlen(victim->pcdata->maim) > 3) {
      sprintf(buf, "`R%s %s.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->maim);
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_DISFIGURED && !is_masked(victim)) {
      sprintf(buf, "%s is terribly disfigured.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_CRIPPLED) {
      sprintf(buf, "%s is seriously crippled.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_INJURY) {
      sprintf(buf, "%s appears injured.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
      strcat(string, buf);
    }
    if (is_ill(victim) && victim->pcdata->lf_modifier <= -5) {
      sprintf(buf, "It looks like %s has a cold.`x\n\r", (victim->sex == SEX_MALE) ? "he" : "she");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_CATATONIC) {
      sprintf(buf, "%s moves with unusual lethargy.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_UNLIVING) {
      sprintf(buf, "%s skin seems unnaturally smooth and pale.`x\n\r", (victim->sex == SEX_MALE) ? "His" : "Her");
      strcat(string, buf);
    }
    if (in_haven(ch->in_room) && can_shroud(ch) && event_cleanse != 1 && !seems_under_understanding(victim, ch) && !seems_under_limited(victim, ch) && !guestmonster(victim)) {
      sprintf(buf, "%s aura has a faint `rred`x ", (victim->sex == SEX_MALE) ? "His" : "Her");
      strcat(string, buf);

      if (can_shroud(ch) && event_cleanse != 1 && (!seems_super(victim) || get_skill(victim, SKILL_OBFUSCATE) > 0) && newbie_level(victim) < 10) {
        if (newbie_level(victim) == 1)
        sprintf(buf, "and `ggreen`x glow.\n\r");
        else if (newbie_level(victim) == 2)
        sprintf(buf, "and `cblue-green`x glow.\n\r");
        else if (newbie_level(victim) == 3)
        sprintf(buf, "and `Bblue`x glow.\n\r");

        strcat(string, buf);
      }
      else {
        strcat(string, "glow.\n\r");
      }
    }
    else if (in_haven(ch->in_room) && can_shroud(ch) && event_cleanse != 1 && !seems_under_understanding(victim, ch) && seems_under_limited(victim, ch) && !guestmonster(victim)) {
      sprintf(buf, "%s aura has a faint `yorange`x ", (victim->sex == SEX_MALE) ? "His" : "Her");
      strcat(string, buf);

      if (can_shroud(ch) && event_cleanse != 1 && (!seems_super(victim) || get_skill(victim, SKILL_OBFUSCATE) > 0) && newbie_level(victim) < 10) {
        if (newbie_level(victim) == 1)
        sprintf(buf, "and `ggreen`x glow.\n\r");
        else if (newbie_level(victim) == 2)
        sprintf(buf, "and `cblue-green`x glow.\n\r");
        else if (newbie_level(victim) == 3)
        sprintf(buf, "and `Bblue`x glow.\n\r");

        strcat(string, buf);
      }
      else {
        strcat(string, "glow.\n\r");
      }
    }

    sprintf(attractstring, "You would judge %s to be a %s%0.1f`x out of 10", (victim->sex == SEX_MALE) ? "him" : "her", attract_color(get_attract(victim, ch)), (float)get_attract(victim, ch) / 10);

    if (victim->played / 3600 < 50) {
      sprintf(buf, "They seem like a newcomer to Haven.\n\r");
      strcat(string, buf);
    }
    else if (social_standing(victim) >= 75) {
      sprintf(buf, "They seem %s.\n\r", lower_standing(victim, social_standing(victim)));
      strcat(string, buf);
    }
    sprintf(buf, "%s is %d'%d\" and has ", (victim->sex == SEX_MALE) ? "He" : "She", get_height_feet(victim), get_height_inches(victim));

    strcat(string, buf);
    if (is_possessed(victim) && ch != victim && !is_covered(victim, COVERS_EYES) && (get_skill(ch, SKILL_CLAIRVOYANCE) >= 1 || get_skill(ch, SKILL_PERCEPTION) >= 2))
    sprintf(buf, "%s, %s, and pale %s. ", get_skin(ch, victim), get_hair(ch, victim), get_eyes(ch, victim));
    else
    sprintf(buf, "%s, %s, and %s. ", get_skin(ch, victim), get_hair(ch, victim), get_eyes(ch, victim));
    strcat(string, buf);

    strcat(string, "\n\r");
    if ((local_fame_level(victim) > 0 && can_see_local_fame(ch)) || get_skill(victim, SKILL_FAMOUS) > 0) {
      if (ch->race != RACE_FACULTY) {
        sprintf(buf, "%s ", fame_insert(victim));
        strcat(string, buf);
      }
    }
    else if (can_see_local_fame(ch)) {
      if (victim->pcdata->job_type_one == JOB_FULLEMPLOY) {
        PROP_TYPE *workplace =
        prop_from_room(get_room_index(victim->pcdata->job_room_one));
        if (workplace != NULL && workplace->type == PROP_SHOP) {
          sprintf(buf, "%s is known for working at %s as %s%s. ", (victim->sex == SEX_MALE) ? "He" : "She", from_color(workplace->propname), smart_a_or_an(victim->pcdata->job_title_one), victim->pcdata->job_title_one);
        }
        else {
          sprintf(buf, "%s is known for working from home as %s%s. ", (victim->sex == SEX_MALE) ? "He" : "She", smart_a_or_an(victim->pcdata->job_title_one), victim->pcdata->job_title_one);
        }
        strcat(string, buf);
      }
      else if (victim->race == RACE_FACULTY && !institute_room(victim->in_room) && safe_strlen(victim->pcdata->job_title_one) > 1) {
        sprintf(buf, "%s is known for working in the institute as %s%s. ", (victim->sex == SEX_MALE) ? "He" : "She", smart_a_or_an(victim->pcdata->job_title_one), victim->pcdata->job_title_one);
        strcat(string, buf);
      }
    }
    if (victim->pcdata->wilds_fame_level > 0 && (in_world(ch) == WORLD_WILDS || ch->pcdata->wilds_fame_level > 0 || ch->pcdata->wexp >= 250)) {
      sprintf(buf, "%s ", wilds_fame_insert(victim));
      strcat(string, buf);
    }
    if (victim->pcdata->other_fame_level > 0 && (in_world(ch) == WORLD_OTHER || ch->pcdata->other_fame_level > 0 || ch->pcdata->oexp >= 250)) {
      sprintf(buf, "%s ", other_fame_insert(victim));
      strcat(string, buf);
    }
    if (victim->pcdata->godrealm_fame_level > 0 && (in_world(ch) == WORLD_GODREALM || ch->pcdata->godrealm_fame_level > 0 || ch->pcdata->gexp >= 250)) {
      sprintf(buf, "%s ", godrealm_fame_insert(victim));
      strcat(string, buf);
    }
    if (victim->pcdata->hell_fame_level > 0 && (in_world(ch) == WORLD_HELL || ch->pcdata->hell_fame_level > 0 || ch->pcdata->hexp >= 250)) {
      sprintf(buf, "%s ", hell_fame_insert(victim));
      strcat(string, buf);
    }
    if (IS_FLAG(victim->act, PLR_VILLAIN)) {
      sprintf(
      buf, "People nearby seem to be acting cautiously, as if afraid of %s.`x\n\r", (victim->sex == SEX_MALE) ? "him" : "her");
      strcat(string, buf);
    }
    if (has_weakness(ch, victim) && (is_gm(ch) || ch->faction != 0)) {
      sprintf(buf, "%s is rumored to be psychically protected.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
      strcat(string, buf);
    }

    if (victim->race == RACE_BROWN && ch->race == RACE_BROWN) {
      sprintf(buf, "You think you recognize them from campus.\n\r");
      strcat(string, buf);
    }
    if (victim->pcdata->impregnated != 0)
    //&& type == LOOK_GLANCE)
    {
      int max_length = get_pregnancy_minlength(victim) + 3;
      int weeks_mid, weeks_late;
      weeks_mid = (max_length / 4) * 2;
      weeks_late = (max_length / 4) * 3;
      if (victim->pcdata->impregnated + (3600 * 24 * 7 * weeks_late) <
          current_time) {
        sprintf(buf, "%s is in the late stages of pregnancy.`x ", (victim->sex == SEX_MALE) ? "He" : "She");
        strcat(string, buf);
      }
      else if (victim->pcdata->impregnated + (3600 * 24 * 7 * weeks_mid) <
          current_time) {
        sprintf(buf, "%s is in the middle stages of pregnancy.`x ", (victim->sex == SEX_MALE) ? "He" : "She");
        strcat(string, buf);
      }
    }
    //                strcat(string, "\n");
    //                strcat(string, get_default_dreamdesc(victim));

    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    return;
  }

  bool can_see_wear(CHAR_DATA *ch, int iWear) {
    switch (iWear) {
    default:
      return TRUE;
      break;
    case WEAR_BODY_24:
      if ((get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_24)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_23:
      if ((get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_23))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_23)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_22:
      if ((get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_22))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_22))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_22)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_21:
      if ((get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_21))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_21))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_21))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_21)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_20:
      if ((get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_20))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_20))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_20))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_20))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_20)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_19:
      if ((get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_19))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_19))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_19))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_19))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_19))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_19)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_18:
      if ((get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_18))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_18))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_18))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_18))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_18))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_18))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_18)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_17:
      if ((get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_17))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_17)))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_16:
      if ((get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_16))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_16))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_15:
      if ((get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_15))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_15))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_14:
      if ((get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_14))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_14))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_13:
      if ((get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_13))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_13))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_12:
      if ((get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_12))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_12))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_11:
      if ((get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_11))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_11))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_10:
      if ((get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_10))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_10))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_9:
      if ((get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_9))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_9))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_8:
      if ((get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_8))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_8))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_7:
      if ((get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_7))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_7))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_6:
      if ((get_eq_char(ch, WEAR_BODY_7) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_7), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_6))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_6))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_5:
      if ((get_eq_char(ch, WEAR_BODY_6) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_6), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_7) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_7), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_5))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_5))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_4:
      if ((get_eq_char(ch, WEAR_BODY_5) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_5), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_6) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_6), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_7) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_7), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_4))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_4))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_3:
      if ((get_eq_char(ch, WEAR_BODY_4) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_4), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_5) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_5), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_6) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_6), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_7) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_7), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_3))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_3))))
      return TRUE;
      else
      return FALSE;
      break;
    case WEAR_BODY_2:
      if ((get_eq_char(ch, WEAR_BODY_3) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_3), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_4) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_4), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_5) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_5), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_6) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_6), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_7) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_7), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_2))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_2))))
      return TRUE;
      else {
        return FALSE;
      }
      break;
    case WEAR_BODY_1:
      if ((get_eq_char(ch, WEAR_BODY_2) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_2), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_3) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_3), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_4) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_4), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_5) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_5), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_6) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_6), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_7) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_7), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_8) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_8), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_9) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_9), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_10) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_10), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_11) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_11), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_12) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_12), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_13) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_13), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_14) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_14), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_15) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_15), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_16) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_16), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_17) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_17), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_18) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_18), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_19) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_19), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_20) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_20), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_21) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_21), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_22) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_22), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_23) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_23), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_24) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_24), get_eq_char(ch, WEAR_BODY_1))) && (get_eq_char(ch, WEAR_BODY_25) == NULL || !does_conceal(get_eq_char(ch, WEAR_BODY_25), get_eq_char(ch, WEAR_BODY_1))))
      return TRUE;
      else {
        return FALSE;
      }
      break;
    }
    return TRUE;
  }

  bool compare_characters(CHAR_DATA *a, CHAR_DATA *b, CHAR_DATA *ch) {
    return get_attract(b, ch) < get_attract(a, ch);
  }

  void show_char_to_char(CharList *list, CHAR_DATA *ch, int moving) {

    if (ch->in_room != NULL && in_fight(ch))
    displaypois(ch);

    try {

      std::vector<CHAR_DATA*> sortedList(list->begin(), list->end());

      // Sort the copy using the custom comparison function
      std::sort(sortedList.begin(), sortedList.end(), [ch](CHAR_DATA *a, CHAR_DATA *b) {
        return compare_characters(a, b, ch);
      });

      CHAR_DATA *last_char = nullptr;
      CHAR_DATA *rch;

      for (std::vector<CHAR_DATA*>::iterator it = sortedList.begin(); it != sortedList.end(); ++it) {
        CHAR_DATA *rch = *it;

        rch = *it;

        if (rch == ch)
        continue;

        if(rch->in_room == NULL)
        continue;

        if (is_cloaked(rch) && public_room(rch->in_room) && !IS_IMMORTAL(ch))
        continue;

        if(higher_power(rch) && !IS_IMMORTAL(ch))
        continue;

        if (moving == 1 && IS_FLAG(rch->act, PLR_SHADOW) && get_skill(ch, SKILL_PERCEPTION) <= get_skill(rch, SKILL_STEALTH))
        continue;

        if (last_char && (get_attract(last_char, ch) - get_attract(rch, ch)) > 10) {
          send_to_char("\n\r", ch);
        }

        if (can_see(ch, rch)) {
          show_char_to_char_0(rch, ch, 0);
          last_char = rch;
        }

      }
    } catch (const std::exception& e) {
      bug("Error doing room look.", 0);
      send_to_char("There was an issue displaying the room.\n\r", ch);
    }

    return;
  }

  int place_pop(ROOM_INDEX_DATA *room, char *place) {
    int pop = 0;
    CHAR_DATA *person;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      person = *it;
      if (person == NULL || person->in_room == NULL || IS_NPC(person))
      continue;

      if (safe_strlen(person->pcdata->place) > 2 && !str_cmp(place, person->pcdata->place))
      pop++;
    }
    return pop;
  }

  void show_crowded_char_to_char(CharList *list, CHAR_DATA *ch) {
    std::vector<CHAR_DATA*> sortedList(list->begin(), list->end());

    // Sort the copy using the custom comparison function
    std::sort(sortedList.begin(), sortedList.end(), [ch](CHAR_DATA *a, CHAR_DATA *b) {
      return compare_characters(a, b, ch);
    });

    CHAR_DATA *last_char = nullptr;
    CHAR_DATA *rch;

    for (std::vector<CHAR_DATA*>::iterator it = sortedList.begin(); it != sortedList.end(); ++it) {
      CHAR_DATA *rch = *it;

      rch = *it;

      if (rch == ch)
      continue;

      if (is_cloaked(rch) && public_room(rch->in_room) && !IS_IMMORTAL(ch))
      continue;


      if(higher_power(rch) && !IS_IMMORTAL(ch))
      continue;


      if (last_char && (get_attract(last_char, ch) - get_attract(rch, ch)) > 10) {
        send_to_char("\n\r", ch);
      }


      if (can_see(ch, rch) && same_place(ch, rch) && safe_strlen(ch->pcdata->place) > 1) {
        show_char_to_char_0(rch, ch, 0);
        last_char = rch;

      }
    }
    char *dynamicplaces[20];
    int dynamicpop[20];
    for (int i = 0; i < 20; i++) {
      dynamicplaces[i] = str_dup("");
      dynamicpop[i] = 0;
    }
    CHAR_DATA *person;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      person = *it;
      if (person == NULL || person->in_room == NULL || IS_NPC(person))
      continue;

      if (safe_strlen(person->pcdata->place) > 1) {
        bool placefound = FALSE;
        for (int i = 0; i < 20; i++) {
          if (!str_cmp(person->pcdata->place, dynamicplaces[i]))
          placefound = TRUE;
        }
        if (placefound == FALSE) {
          bool placeplaced = FALSE;
          for (int i = 0; i < 20; i++) {
            if (safe_strlen(dynamicplaces[i]) < 2 && placeplaced == FALSE) {
              free_string(dynamicplaces[i]);
              dynamicplaces[i] = str_dup(person->pcdata->place);
              placeplaced = TRUE;
              dynamicpop[i] = place_pop(ch->in_room, person->pcdata->place);
            }
          }
        }
      }
    }
    for (int i = 0; i < 20; i++) {
      if (safe_strlen(dynamicplaces[i]) > 1 && (str_cmp(ch->pcdata->place, dynamicplaces[i]) || safe_strlen(ch->pcdata->place) < 2)) {
        if (dynamicpop[i] > 1)
        printf_to_char(ch, "%d people are at %s.\n\r", dynamicpop[i], dynamicplaces[i]);
        else
        printf_to_char(ch, "1 person is at %s.\n\r", dynamicplaces[i]);
      }
    }
    return;
  }

  bool is_asleep(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->pcdata->sleeping > 0)
    return TRUE;

    return FALSE;
  }

  bool check_blind(CHAR_DATA *ch) {
    if (!IS_NPC(ch) && IS_FLAG(ch->act, PLR_HOLYLIGHT))
    return TRUE;

    if (IS_NPC(ch))
    return TRUE;

    if (IS_AFFECTED(ch, AFF_BLIND)) {
      send_to_char("You can't see a thing!\n\r", ch);
      return FALSE;
    }

    if (IS_FLAG(ch->comm, COMM_BLINDFOLD))
    return FALSE;

    if (is_asleep(ch) && !is_dreaming(ch))
    return FALSE;

    return TRUE;
  }

  bool is_blind(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_BLIND))
    return TRUE;

    if (has_caff(ch, CAFF_BLIND))
    return TRUE;

    if (has_caff(ch, CAFF_TEAR))
    return TRUE;

    if (has_caff(ch, CAFF_STASIS)) {
      if (ch->fight_fast == TRUE) {
        if (ch->attack_timer > 0 && ch->move_timer > 0)
        return TRUE;
      }
      else {
        if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK) && IS_FLAG(ch->fightflag, FIGHT_NOMOVE))
        return TRUE;
      }
    }

    if (is_asleep(ch))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_BLINDFOLD) && !is_ghost(ch))
    return TRUE;

    return FALSE;
  }

  bool is_deaf(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_DEAF))
    return TRUE;

    if (is_asleep(ch))
    return TRUE;

    return FALSE;
  }

  /* changes your scroll */
  _DOFUN(do_scroll) {
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      if (ch->lines == 0)
      send_to_char("You do not page long messages.\n\r", ch);
      else {
        sprintf(buf, "You currently display %d lines per page.\n\r", ch->lines + 2);
        send_to_char(buf, ch);
      }
      return;
    }

    if (!is_number(arg)) {
      send_to_char("You must provide a number.\n\r", ch);
      return;
    }

    lines = atoi(arg);

    if (lines == 0) {
      send_to_char("Paging disabled.\n\r", ch);
      ch->lines = 0;
      return;
    }

    if (lines < 10 || lines > 100) {
      send_to_char("You must provide a reasonable number.\n\r", ch);
      return;
    }

    sprintf(buf, "Scroll set to %d lines.\n\r", lines);
    send_to_char(buf, ch);
    ch->lines = lines - 2;
  }

  _DOFUN(do_length) { do_function(ch, &do_scroll, argument); }
  _DOFUN(do_width) {
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      sprintf(buf, "You currently display %d characters per line.\n\r", ch->linewidth);
      send_to_char(buf, ch);
      return;
    }

    if (!is_number(arg)) {
      send_to_char("You must provide a number.\n\r", ch);
      return;
    }

    lines = atoi(arg);

    if (lines < 80 || lines > 320) {
      send_to_char("Acceptable widths are between 80 and 320 characters.\n\r", ch);
      return;
    }

    sprintf(buf, "Linewidth set to %d characters.\n\r", lines);
    send_to_char(buf, ch);
    ch->linewidth = lines;
  }

  /* RT does socials */
  _DOFUN(do_socials) {
    printf_to_char(ch, "Influence             : %d\n\r", ch->pcdata->influence);
    printf_to_char(ch, "Scheme Influence      : %d\n\r", ch->pcdata->scheme_influence);
    printf_to_char(ch, "Supernatural Influence: %d\n\r", ch->pcdata->super_influence);
    printf_to_char(ch, "Society Influence     : %d\n\r", ch->pcdata->faction_influence);

    printf_to_char(ch, "You are considered %s.\n\r", standing(ch, social_standing(ch)));
    printf_to_char(ch, "You are generally considered %s.\n\r", standing(ch, ch->pcdata->base_standing));
    /*
    if(ch->pcdata->social_behave < -30)
    send_to_char("Your recent behavior has made people think a little worse of you.\n\r", ch);
    else if(ch->pcdata->social_behave > 30)
    send_to_char("Your recent behavior has made people think a little better of you.\n\r", ch);
    */
    if (ch->pcdata->social_praise < -1000)
    send_to_char("There are negative rumors about you swirling around.\n\r", ch);
    else if (ch->pcdata->social_praise > 1000)
    send_to_char("There are positive rumors about you swirling around.\n\r", ch);

    return;

    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++) {
      sprintf(buf, "%-12s", social_table[iSocial].name);
      send_to_char(buf, ch);
      if (++col % 6 == 0)
      send_to_char("\n\r", ch);
    }

    if (col % 6 != 0)
    send_to_char("\n\r", ch);
    return;
  }

  /* RT Commands to replace news, motd, imotd, etc from ROM */

  _DOFUN(do_policy) {
    if (!str_cmp(argument, "theme"))
    do_function(ch, &do_help, "policy theme");
    else if (!str_cmp(argument, "pk"))
    do_function(ch, &do_help, "policy pk");
    else if (!str_cmp(argument, "immortals"))
    do_function(ch, &do_help, "policy immortals");
    else if (!str_cmp(argument, "reports"))
    do_function(ch, &do_help, "policy reports");
    else if (!str_cmp(argument, "permdeath"))
    do_function(ch, &do_help, "policy permdeath");
    else
    send_to_char("Syntax: policy (theme/immortals/pk/permdeath/reports)\n\r", ch);
  }

  _DOFUN(do_motd) { do_function(ch, &do_help, "motd"); }
  _DOFUN(do_rules) { do_function(ch, &do_help, "Rules"); }
  _DOFUN(do_staff) { do_function(ch, &do_help, "Staff"); }

  _DOFUN(do_imotd) { do_function(ch, &do_help, "imotd"); }

  /* RT this following section holds all the auto commands from ROM, as well as replacements for config */

  _DOFUN(do_autolist) {
    /* lists most player flags */
    if (IS_NPC(ch))
    return;

    send_to_char("   action     status\n\r", ch);
    send_to_char("---------------------\n\r", ch);

    send_to_char("compact mode   ", ch);
    if (IS_FLAG(ch->comm, COMM_COMPACT))
    send_to_char("ON\n\r", ch);
    else
    send_to_char("OFF\n\r", ch);

    send_to_char("prompt         ", ch);
    if (IS_FLAG(ch->comm, COMM_PROMPT))
    send_to_char("ON\n\r", ch);
    else
    send_to_char("OFF\n\r", ch);

    send_to_char("combine items  ", ch);
    if (IS_FLAG(ch->comm, COMM_COMBINE))
    send_to_char("ON\n\r", ch);
    else
    send_to_char("OFF\n\r", ch);

    if (IS_FLAG(ch->act, PLR_NOFOLLOW))
    send_to_char("You do not welcome followers.\n\r", ch);
    else
    send_to_char("You accept followers.\n\r", ch);
  }

  _DOFUN(do_brief) {
    if (IS_FLAG(ch->comm, COMM_BRIEF)) {
      send_to_char("Full descriptions activated.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_BRIEF);
    }
    else {
      send_to_char("Short descriptions activated.\n\r", ch);
      SET_FLAG(ch->comm, COMM_BRIEF);
    }
  }

  _DOFUN(do_compact) {
    if (IS_FLAG(ch->comm, COMM_COMPACT)) {
      send_to_char("Compact mode removed.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_COMPACT);
    }
    else {
      send_to_char("Compact mode set.\n\r", ch);
      SET_FLAG(ch->comm, COMM_COMPACT);
    }
  }

  _DOFUN(do_prompt) {
    char buf[MAX_STRING_LENGTH];
    if (ch->pcdata->ci_editing == 18) {
      ch->pcdata->ci_absorb = 1;
      string_append(ch, &ch->pcdata->ci_desc);
      send_to_char("Write the alternative encounter prompt.\n\r", ch);
      return;
    }


    if (argument[0] == '\0') {
      if (IS_FLAG(ch->comm, COMM_PROMPT)) {
        send_to_char("You will no longer see prompts.\n\r", ch);
        REMOVE_FLAG(ch->comm, COMM_PROMPT);
      }
      else {
        send_to_char("You will now see prompts.\n\r", ch);
        SET_FLAG(ch->comm, COMM_PROMPT);
      }
      return;
    }

    if (!strcmp(argument, "all"))
    strcpy(buf, "[%d%h`x/`g%H`xCN %D%e`x/`g%E`xSP %A%a `x(`W%F`x)] %p`x");
    else if (!strcmp(argument, "starter")) {
      strcpy(buf, "[%d%h/`g%H`xDF `c%L`xST `R%l`xWN `B%g`xLG (`W%F`x)] %p`x");
    }
    else {
      if (safe_strlen(argument) > 100)
      argument[100] = '\0';
      strcpy(buf, argument);
      smash_tilde(buf);
      if (str_suffix("%c", buf))
      strcat(buf, " ");
    }

    free_string(ch->prompt);
    ch->prompt = str_dup(buf);
    sprintf(buf, "Prompt set to %s\n\r", ch->prompt);
    if (str_cmp(argument, "starter"))
    send_to_char(buf, ch);
    return;
  }

  _DOFUN(do_combine) {
    if (IS_FLAG(ch->comm, COMM_COMBINE)) {
      send_to_char("Long inventory selected.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_COMBINE);
    }
    else {
      send_to_char("Combined inventory selected.\n\r", ch);
      SET_FLAG(ch->comm, COMM_COMBINE);
    }
  }

  _DOFUN(do_nofollow) {
    if (IS_NPC(ch)) {
      return;
    }

    send_to_char("You stop people following you.\n\r", ch);
    die_follower(ch);
    WAIT_STATE(ch, PULSE_PER_SECOND * 5);
  }

  bool show_char_obj_to_char(CHAR_DATA *ch, CHAR_DATA *victim, char arg[MSL]) {
    int iWear;
    OBJ_DATA *obj;
    bool found = FALSE;

    if (arg[0] == '\0') {
      return FALSE;
    }

    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(victim, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(victim, iWear) || (has_xray(ch) && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
        if (is_name(arg, obj->name)) {
          show_obj_to_char(obj, ch, FALSE, FALSE);
          send_to_char("\n\r", ch);
          if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->value[1], CONT_CLOSED) && victim == ch) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }
          if ((obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC) && victim == ch) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }

          send_to_char("\n\r", ch);
          found = TRUE;
        }
      }
    }
    for (obj = victim->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && (obj->size >= 25) && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (is_name(arg, obj->name)) {
          show_obj_to_char(obj, ch, FALSE, FALSE);
          send_to_char("\n\r", ch);
          if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->value[1], CONT_CLOSED) && victim == ch) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }
          if ((obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC) && victim == ch) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }

          send_to_char("\n\r", ch);
          found = TRUE;
        }
      }
    }
    return found;
  }

  void ghostwalker_check(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL) {
        victim = d->character;

        if(IS_NPC(victim))
        continue;

        if (victim->pcdata->ghost_room == ch->in_room->vnum && is_ghost(victim) && IS_FLAG(victim->act, PLR_GHOSTWALKING))
        printf_to_char(ch, "The comatose form of %s is here.\n\r", victim->name);
        if (victim->pcdata->ghost_room == ch->in_room->vnum && victim->pcdata->spectre > 0 && IS_FLAG(victim->act, PLR_SHROUD))
        printf_to_char(ch, "The sleeping form of %s is here.\n\r", victim->name);
      }
    }
  }

  bool has_extradesc(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    if (room->extra_descr == NULL)
    return FALSE;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (str_cmp(ed->keyword, "!keys") && str_cmp(ed->keyword, "dark") && str_cmp(ed->keyword, "temporary") && str_cmp(ed->keyword, "!sleepers") && str_cmp(ed->keyword, "!srtemporary") && str_cmp(ed->keyword, "acutesight") && str_cmp(ed->keyword, "acutehearing") && str_cmp(ed->keyword, "acutesmell") && str_cmp(ed->keyword, "+taste") && str_cmp(ed->keyword, "+owner") && str_cmp(ed->keyword, "+ringtone") && str_cmp(ed->keyword, "!denial") && str_cmp(ed->keyword, "!bugs") && str_cmp(ed->keyword, "!srhidden") && str_cmp(ed->keyword, "!hidden") && str_cmp(ed->keyword, "!roomtitle") && str_cmp(ed->keyword, "!atmosphere")) {
        return TRUE;
      }
    }
    return FALSE;
  }
  bool has_places(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    for (ed = room->places; ed; ed = ed->next) {
      return TRUE;
    }
    return FALSE;
  }
  bool has_cars(ROOM_INDEX_DATA *room) {
    for (int i = 0; i < 10; i++) {
      if (room->vehicle_cost[i] > 0)
      return TRUE;
    }
    return FALSE;
  }

  void show_places_char_to_char(CharList *list, CHAR_DATA *ch, char *place) {
    for (CharList::iterator it = list->begin(); it != list->end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == ch)
      continue;
      if (IS_NPC(rch))
      continue;

      if (is_cloaked(rch) && public_room(rch->in_room))
      continue;

      if ((can_see(ch, rch) && is_name(place, rch->pcdata->place))) {
        //|| (ch->pcdata->dream_room==rch->pcdata->dream_room && is_name(place, //rch->pcdata->dreamplace))) { //dream places
        show_char_to_char_0(rch, ch, 0);
      }
    }

    return;
  }

  int get_sunpos(ROOM_INDEX_DATA *room) {
    tm *ptm;
    time_t east_time;

    east_time = current_time - 14400;
    east_time -= 60;
    ptm = gmtime(&east_time);

    int phase = sunphase(room);

    if (phase == 2) {
      if (ptm->tm_mon == 0)
      return DIR_SOUTHEAST;
      else
      return DIR_EAST;
    }
    if (phase == 3) {
      if (ptm->tm_mon == 0)
      return DIR_SOUTH;
      else if (ptm->tm_mon == 11 || ptm->tm_mon == 1)
      return DIR_SOUTHEAST;
      else
      return DIR_EAST;
    }
    if (phase == 4) {
      if (ptm->tm_mon == 0)
      return DIR_SOUTH;
      else
      return DIR_UP;
    }
    if (phase == 5) {
      if (ptm->tm_mon == 0)
      return DIR_SOUTH;
      else if (ptm->tm_mon == 11 || ptm->tm_mon == 1)
      return DIR_SOUTHWEST;
      else
      return DIR_WEST;
    }
    if (phase == 6) {
      if (ptm->tm_mon == 0)
      return DIR_SOUTHWEST;
      else
      return DIR_WEST;
    }
    return -1;
  }

  int distance_from_town(ROOM_INDEX_DATA *room) {
    int minx = 0;
    int miny = 0;
    int maxx = 71;
    int maxy = 71;

    if (room->area->world == WORLD_OTHER)
    return get_dist(room->x, room->y, (room->area->minx + room->area->maxx) / 2, (room->area->miny + room->area->maxy) / 2) * 2;
    if (room->area->world == WORLD_WILDS)
    return get_dist(room->x, room->y, (room->area->minx + room->area->maxx) / 2, (room->area->miny + room->area->maxy) / 2) * 5;
    if (room->area->world == WORLD_GODREALM)
    return get_dist(room->x, room->y, (room->area->minx + room->area->maxx) / 2, (room->area->miny + room->area->maxy) / 2) * 2;
    if (room->area->world == WORLD_HELL)
    return get_dist(room->x, room->y, (room->area->minx + room->area->maxx) / 2, (room->area->miny + room->area->maxy) / 2) * 2;

    if (room->area->world != WORLD_EARTH)
    return -1;

    if (room->z < -500 || room->z > 10000)
    return -1;

    if (room->area->minx == -1 && room->area->maxx == -1)
    return -1;

    if (room->x <= maxx && room->x >= minx && room->y <= maxy && room->y >= miny)
    return -1;

    if (room->x >= maxx && room->y >= maxy)
    return get_dist(room->x, room->y, maxx, maxy);
    else if (room->x >= maxx && room->y <= miny)
    return get_dist(room->x, room->y, maxx, miny);
    else if (room->x <= minx && room->y >= maxy)
    return get_dist(room->x, room->y, minx, maxy);
    else if (room->x <= minx && room->y <= miny)
    return get_dist(room->x, room->y, minx, miny);
    else if (room->x >= maxx)
    return get_dist(room->x, room->y, maxx, room->y);
    else if (room->x <= minx)
    return get_dist(room->x, room->y, minx, room->y);
    else if (room->y >= maxy)
    return get_dist(room->x, room->y, room->x, maxy);
    else if (room->y <= miny)
    return get_dist(room->x, room->y, room->x, miny);

    return -1;
  }

  // Cleaned up subroutine that respects width preference - Discordance
  void show_room_to_char(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    char buf[MSL];
    char page[MSL];
    char nocol[MSL];
    ROOM_INDEX_DATA *current_room;

    if (is_dreaming(ch)) {
      current_room = get_room_index(ch->pcdata->dream_room);

      if (ch->pcdata->dream_room > 0) {
        FANTASY_TYPE *fant;
        if ((fant = in_fantasy(ch)) != NULL) {
          if (part_of_fantasy(ch, fant)) {
            for (int x = 0; x < 25; x++) {
              if (!str_cmp(ch->name, fant->participants[x]) && fant->participant_blind[x] == TRUE) {
                send_to_char("You can't see right now.\n\r", ch);
                return;
              }
            }
          }
        }
      }
      else {
        if (safe_strlen(ch->pcdata->dream_environment) > 2) {
          send_to_char(ch->pcdata->dream_environment, ch);
        }
        else if (safe_strlen(ch->pcdata->dream_link->pcdata->dream_environment) >
            2) {
          send_to_char(ch->pcdata->dream_link->pcdata->dream_environment, ch);
        }
        else {
          send_to_char("A vast grey void.\n\r", ch);
        }

        printf_to_char(ch, "\n%s\n\r", PERS(ch->pcdata->dream_link, ch));
        return;
      }
    }
    else {
      current_room = room;
    }

    // Setting variable defaults
    strcpy(buf, "");
    strcpy(page, "");
    // default descriptions for areas
    if (room->description == str_dup("")) {
      strcat(page, stock_description(ch, room)); // this is in world.c
      strcat(page, "\n\r");
    }
    // room specific descriptions
    else {
      if (IS_FLAG(ch->act, PLR_SHROUD)) {
        // if(room->player_shroud != NULL && safe_strlen(room->player_shroud) > 2) {
        // sprintf(buf, "%s`x\n\r", room->player_shroud);
        // strcat(page, buf);
        //}
        if (room->shroud != NULL && safe_strlen(room->shroud) > 2) {
          sprintf(buf, "%s`x\n\r", room->shroud);
          strcat(page, buf);
        }
        // else if(room->player_description != NULL && // safe_strlen(room->player_description) > 2) { sprintf(buf, // "%s`x\n\r",room->player_description); strcat(page, buf);
        //}
        else if (room->description != NULL) {
          sprintf(buf, "%s`x\n\r", room->description);
          strcat(page, buf);
        }
      }
      else {
        // if(room->player_description != NULL && safe_strlen(room->player_description)
        // > 2) { sprintf(buf, "%s`x\n\r",room->player_description); strcat(page, // buf);
        //}
        if (room->description != NULL) {
          sprintf(buf, "%s`x\n\r", room->description);
          strcat(page, buf);
        }
      }
    }

    /*
    for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();it != DomainVect.end(); ++it) {
      for (int i = 0; i < 50; i++) {
        if ((*it)->bigshrines[i] == ch->in_room->vnum) {
          sprintf(buf, "There is a shrine here.\n\r");
          strcat(page, buf);
        }
        if ((*it)->medshrines[i] == ch->in_room->vnum) {
          sprintf(buf, "There is a shrine here.\n\r");
          strcat(page, buf);
        }
        if ((*it)->smallshrines[i] == ch->in_room->vnum) {
          sprintf(buf, "There is a shrine here.\n\r");
          strcat(page, buf);
        }
      }
    }
    */
    if(IS_IMMORTAL(ch))
    printf_to_char(ch, "Light level: %d\n\r", light_level(ch->in_room));

    if(in_lodge(ch->in_room) && crisis_prologue == 1)
    {
      send_to_char("`243The tv screens show nothing but static, there is no cell signal or internet.\nOutside the window it is entirely black save for\nodd flashes of red and purple light.\nA nearby discarded newspaper shows the date as '5th September, 2037'.\nYou have a hard time remembering how you got here or who anyone else is.`x\n\n\r", ch);
    }

    CHAR_DATA *hp;
    for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING) {
        continue;
      }

      hp = CH(d);
      if(hp->in_room == ch->in_room && higher_power(hp) && hp->possessing == NULL)
      {
        if(strlen(hp->pcdata->eidilon_ambient) > 3)
        {
          sprintf(buf, "%s\n\r", hp->pcdata->eidilon_ambient);
          strcat(page, buf);
        }
        else
        {
          sprintf(buf, "You sense an unworldly presence.\n\r");
          strcat(page, buf);
        }
      }
    }

    // temporary descriptions
    if (current_room->extra_descr != NULL && get_extra_descr("!srtemporary", current_room->extra_descr) != NULL) {
      sprintf(buf, "\n%s", get_extra_descr("!srtemporary", current_room->extra_descr));
      strcat(page, buf);
    }

    if (current_room->extra_descr != NULL && get_extra_descr("temporary", current_room->extra_descr) != NULL) {
      sprintf(buf, "\n%s", get_extra_descr("temporary", current_room->extra_descr));
      strcat(page, buf);
    }

    strcat(page, "\n\r");
    strcat(page, displaysun(ch));

    if (!IS_SET(current_room->room_flags, ROOM_INDOORS)) {
      // Sun position
      int sunpos = get_sunpos(current_room);
      int phase = sunphase(current_room);
      if (sunpos > -1) {
        if (phase == 2) {
          sprintf(buf, "In the distance %s the `rda`yw`Yn`yin`rg `Ys`Ru`Yn`x crests the horizon. ", relspacial[get_reldirection(sunpos, ch->facing)]);
          strcat(page, buf);
        }
        if (phase == 3 || phase == 5) {
          sprintf(buf, "In the distance %s the sun hangs in the sky. ", relspacial[get_reldirection(sunpos, ch->facing)]);
          strcat(page, buf);
        }
        if (phase == 4) {
          if (sunpos == DIR_UP) {
            sprintf(buf, "The sun hangs in the air far overhead. ");
          }
          else {
            sprintf(buf, "In the distance %s the sun hangs in the sky. ", relspacial[get_reldirection(sunpos, ch->facing)]);
          }
          strcat(page, buf);
        }
        if (phase == 6) {
          sprintf(buf, "In the distance %s the `rsetting s`yu`rn`x, sinks below the horizon. ", relspacial[get_reldirection(sunpos, ch->facing)]);
          strcat(page, buf);
        }
      }
    }

    // Room facing
    if (current_room->exit[ch->facing] != NULL && can_see_room_distance(ch, current_room->exit[ch->facing]->u1.to_room, DISTANCE_NEAR)) {
      ROOM_INDEX_DATA *displayroom = current_room->exit[ch->facing]->u1.to_room;
      if (current_room->area->world != displayroom->area->world)
      displayroom = cardinal_room(current_room, ch->facing);
      remove_color(nocol, displayroom->name);
      sprintf(buf, "Ahead of you is %s. ", nocol);
      strcat(page, buf);
    }

    if (!is_dreaming(ch)) {
      if (!IS_SET(current_room->room_flags, ROOM_INDOORS)) {
        // Nearby water
        if (can_hear_ocean(ch) && current_room->sector_type != SECT_WATER && current_room->sector_type != SECT_UNDERWATER && current_room->sector_type != SECT_SHALLOW) {
          sprintf(buf, "You can hear the sound of waves %s. ", relspacial[get_reldirection(
          roomdirection(get_roomx(current_room), get_roomy(current_room), 57, 35), ch->facing)]);
          strcat(page, buf);
        }
        if (distance_from_town(current_room) > 1 && current_room->z >= 0 && !IS_SET(current_room->room_flags, ROOM_INDOORS) && !is_helpless(ch)) {
          int dtotown = distance_from_town(current_room);
          int miles = 0;
          dtotown = dtotown * 25;
          int differential = dtotown / 10;
          dtotown += differential / 2;
          dtotown = dtotown - (dtotown % differential);
          if (dtotown >= 1320)
          miles = dtotown * 100 / 5280;
          if (miles > 0)
          sprintf(
          buf, "You estimate you're about %.2f miles from the edge of town. ", ((double)(miles)) / 100);
          else
          sprintf(buf, "You estimate you're about %d feet from the edge of town. ", dtotown);
          strcat(page, buf);
        }

        // added some stuff for shop names instead of addresses - Discordance
        if (current_room->sector_type == SECT_STREET || current_room->sector_type == SECT_ALLEY || current_room->sector_type == SECT_SIDEWALK) {
          for (int dir = 0; dir < 10; dir++) {
            if (dir == DIR_UP || dir == DIR_DOWN)
            continue;
            if (current_room->exit[dir] != NULL && room_prop(current_room->exit[dir]->u1.to_room) != NULL && current_room->exit[dir]->u1.to_room->vnum ==
                room_prop(current_room->exit[dir]->u1.to_room)->firstroom) {
              if (room_prop(current_room->exit[dir]->u1.to_room)->type == PROP_HOUSE) {
                sprintf(buf, "%s is %s. ", room_prop(current_room->exit[dir]->u1.to_room)->address, relspacial[get_reldirection(dir, ch->facing)]);
                strcat(page, buf);
              }
              else {
                sprintf(buf, "%s is %s. ", room_prop(current_room->exit[dir]->u1.to_room)->propname, relspacial[get_reldirection(dir, ch->facing)]);
                strcat(page, buf);
              }
            }
          }
        }

        // Clock tower position
        // This should refer to a global timekeeping function eventually
        if (charlineofsight_landmark(ch, get_room_index(8402))) {
          if (get_dist(get_roomx(current_room), get_roomy(current_room), 48, 9) <= 50) {
            tm *ptm;
            time_t east_time;

            east_time = current_time;
            ptm = gmtime(&east_time);
            int minutes = ptm->tm_min;
            minutes += 2;
            minutes = minutes - (minutes % 5);
            int hours = get_hour(current_room);
            if (minutes == 60) {
              minutes = 0;
              hours++;
            }
            if (hours > 12) {
              hours = hours - 12;
            }

            if (hours == 0)
            hours = 12;

            if (sunphase(current_room) == 0) {
              sprintf(buf, "In the distance %s you see the `Willuminated face`x of the clock tower, reading %d:%02d. ", relspacial[get_reldirection(
              roomdirection(get_roomx(current_room), get_roomy(current_room), 48, 9), ch->facing)], hours, minutes);
              strcat(page, buf);
            }
            else {
              sprintf(buf, "In the distance %s you see the face of the clock tower, reading %d:%02d. ", relspacial[get_reldirection(
              roomdirection(get_roomx(current_room), get_roomy(current_room), 48, 9), ch->facing)], hours, minutes);
              strcat(page, buf);
            }
          }
          else if (get_dist(get_roomx(current_room), get_roomy(current_room), 48, 9) <= 75 || get_skill(ch, SKILL_ACUTESIGHT) > 0) {
            if (sunphase(current_room) == 0) {
              sprintf(buf, "Far off in the distance %s you see the `Willuminated face`x of the clock tower. ", relspacial[get_reldirection(
              roomdirection(get_roomx(current_room), get_roomy(current_room), 48, 9), ch->facing)]);
              strcat(page, buf);
            }
            else {
              sprintf(buf, "Far off in the distance %s you see the clock tower. ", relspacial[get_reldirection(
              roomdirection(get_roomx(current_room), get_roomy(current_room), 48, 9), ch->facing)]);
              strcat(page, buf);
            }
          }
        }
        if (charlineofsight_landmark(ch, get_room_index(10156))) {
          bool nightlight = FALSE;
          if (sunphase(get_room_index(10156)) == 0 || sunphase(get_room_index(10156)) == 1 || sunphase(get_room_index(10156)) == 7)
          nightlight = TRUE;
          if (get_dist(get_roomx(current_room), get_roomy(current_room), 58, 25) <= (50 + get_skill(ch, SKILL_ACUTESIGHT) * 25) || (nightlight == TRUE && mist_level(get_room_index(10249)) < 3 && get_dist(get_roomx(current_room), get_roomy(current_room), 58, 25) <= (100 + get_skill(ch, SKILL_ACUTESIGHT) * 50))) {
            if (nightlight == TRUE || mist_level(get_room_index(1468)) >= 2) {
              sprintf(buf, "In the distance %s you see the `Ysweeping beam`x of the lighthouse.", relspacial[get_reldirection(
              roomdirection(get_roomx(current_room), get_roomy(current_room), 58, 25), ch->facing)]);
              strcat(page, buf);
            }
            else {
              sprintf(buf, "In the distance %s you can see the lighthouse sitting atop the bluffs.", relspacial[get_reldirection(
              roomdirection(get_roomx(current_room), get_roomy(current_room), 58, 25), ch->facing)]);
              strcat(page, buf);
            }
          }
        }
      }
    }
    sprintf(buf, "\n\r");
    strcat(page, buf);
    page_to_char(wrap_string(page, get_wordwrap(ch)), ch);

    return;
  }

  bool has_dynamic_places(ROOM_INDEX_DATA *room) {
    CHAR_DATA *person;
    for (CharList::iterator it = room->people->begin(); it != room->people->end(); ++it) {
      person = *it;
      if (person == NULL || person->in_room == NULL || IS_NPC(person) || person->pcdata->place == NULL) {
        continue;
      }

      if (safe_strlen(person->pcdata->place) > 1) {
        EXTRA_DESCR_DATA *ed;
        bool staticplace = FALSE;
        for (ed = room->places; ed; ed = ed->next) {
          if (!str_cmp(person->pcdata->place, ed->keyword)) {
            staticplace = TRUE;
          }
        }
        if (staticplace == FALSE) {
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool bugged_room(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("!bugs", ed->keyword) && safe_strlen(ed->description) > 2) {
        return TRUE;
      }
    }
    if (!ed) {
      return FALSE;
    }

    return FALSE;
  }

  _DOFUN(do_glance) {
    if (!str_cmp(argument, "")) {
      if (IS_FLAG(ch->comm, COMM_BRIEF)) {
        do_function(ch, &do_look, "auto");
      }
      else {
        SET_FLAG(ch->comm, COMM_BRIEF);
        do_function(ch, &do_look, "auto");
        REMOVE_FLAG(ch->comm, COMM_BRIEF);
      }
      return;
    }
    char arg1[MSL];
    char arg2[MSL];
    CHAR_DATA *victim;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if ((victim = get_char_room(ch, NULL, arg1)) != NULL) {
      if (!show_char_location_to_char(ch, victim, arg2)) {
        if (!show_char_obj_to_char(ch, victim, arg2)) {
          show_char_to_char_1(victim, ch, LOOK_GLANCE, FALSE);
        }
      }
      return;
    }
    else if ((victim = get_char_distance(ch, arg1, 3)) != NULL) {
      if (!show_char_location_to_char(ch, victim, arg2)) {
        if (!show_char_obj_to_char(ch, victim, arg2)) {
          show_char_to_char_1(victim, ch, LOOK_GLANCE, FALSE);
        }
      }
      return;
    }
    send_to_char("I don't see them here.\n\r", ch);
  }
  
  // Split subroutine from do_look mess - Discordance 11-1-2015
  void show_dreamroom_to_char( CHAR_DATA *ch, ROOM_INDEX_DATA *droom) {
    char buf[MSL];

    send_to_char(droom->name, ch);
    if (in_fantasy(ch) != NULL && part_of_fantasy(ch, in_fantasy(ch))) {
      sprintf(buf, " `c{`x%s`c}`x", in_fantasy(ch)->name);
      send_to_char(buf, ch);
      FANTASY_TYPE *fant = in_fantasy(ch);
      for (int x = 0; x < 100; x++) {
        if (fant->rooms[x] == ch->pcdata->dream_room && fant->safe_room[x] == 1) {
          send_to_char("`WThis area is safe.`x\n\r", ch);
        }
      }
    }
    else if (room_fantasy(droom) != NULL) {
      sprintf(buf, " `r{`x%s`r}`x", room_fantasy(droom)->name);
      send_to_char(buf, ch);
    }
    send_to_char("\n\r", ch);
    send_to_char(droom->description, ch);
    if (has_places(droom)) {
      EXTRA_DESCR_DATA *ed;
      for (ed = droom->places; ed; ed = ed->next) {
        if (!IS_NPC(ch) && !str_cmp(ch->pcdata->dreamplace, ed->keyword)) {
          send_to_char("\n\n\r", ch);
          send_to_char(get_extra_descr(ed->keyword, droom->places), ch);
        }
      }
    }

    if (room_fantasy(droom) != NULL) {
      send_to_char("\n\r", ch);
      FANTASY_TYPE *fant = room_fantasy(droom);
      bool first = TRUE;
      for (int i = 0; i < 200; i++) {
        if (fant->exits[i] == droom->vnum) {
          if (first == TRUE) {
            printf_to_char(ch, "Exits: %s(%s)", fant->exit_name[i], fant->exit_alias[i]);
            first = FALSE;
          }
          else {
            printf_to_char(ch, ", %s(%s)", fant->exit_name[i], fant->exit_alias[i]);
          }
        }
      }
      send_to_char("\n\r", ch);
    }
    if (has_extradesc(droom)) {
      EXTRA_DESCR_DATA *ed;
      sprintf(buf, "Extra: ");
      for (ed = droom->extra_descr; ed; ed = ed->next) {
        if (str_cmp(ed->keyword, "!keys") && str_cmp(ed->keyword, "dark") && str_cmp(ed->keyword, "temporary") && str_cmp(ed->keyword, "!sleepers") && str_cmp(ed->keyword, "!srtemporary") && str_cmp(ed->keyword, "acutesight") && str_cmp(ed->keyword, "acutehearing") && str_cmp(ed->keyword, "acutesmell") && str_cmp(ed->keyword, "!denial") && str_cmp(ed->keyword, "!bugs") && str_cmp(ed->keyword, "!srhidden") && str_cmp(ed->keyword, "!hidden") && str_cmp(ed->keyword, "!roomtitle") && str_cmp(ed->keyword, "!atmosphere")) {
          strcat(buf, ed->keyword);
          if (ed->next) {
            strcat(buf, ", ");
          }
        }
      }
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
    }
    if (has_places(droom)) {
      EXTRA_DESCR_DATA *ed;
      char buf1[MSL];
      sprintf(buf1, "Places: ");
      for (ed = droom->places; ed; ed = ed->next) {
        strcat(buf1, ed->keyword);
        if (ed->next) {
          strcat(buf1, ", ");
        }
      }
      strcat(buf1, "\n\r");
      send_to_char(buf1, ch);
    }

    char *dynamicplaces[20];
    for (int i = 0; i < 20; i++) {
      dynamicplaces[i] = str_dup("");
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *person;
      DESCRIPTOR_DATA *d = *it;
      person = CH(d);

      if (person == NULL) {
        continue;
      }
      if (IS_NPC(person) || !is_dreaming(person)) {
        continue;
      }
      if (person == ch) {
        continue;
      }
      if (goddreamer(person)) {
        continue;
      }
      if (person->pcdata->dream_room != droom->vnum) {
        continue;
      }

      if (safe_strlen(person->pcdata->dreamplace) > 1) {
        bool placefound = FALSE;
        for (int i = 0; i < 20; i++) {
          if (!str_cmp(person->pcdata->dreamplace, dynamicplaces[i])) {
            placefound = TRUE;
          }
        }
        if (placefound == FALSE) {
          bool placeplaced = FALSE;
          for (int i = 0; i < 20; i++) {
            if (safe_strlen(dynamicplaces[i]) < 2 && placeplaced == FALSE) {
              free_string(dynamicplaces[i]);
              dynamicplaces[i] = str_dup(person->pcdata->place);
              placeplaced = TRUE;
            }
          }
        }
      }
    }
    if (safe_strlen(dynamicplaces[0]) > 1) {
      char buf2[MSL];
      sprintf(buf2, "Dynamic Places: ");
      for (int i = 0; i < 20; i++) {
        if (safe_strlen(dynamicplaces[i]) > 1 && (str_cmp(ch->pcdata->place, dynamicplaces[i]) || safe_strlen(ch->pcdata->dreamplace) < 2)) {
          printf_to_char(ch, "People are at %s.\n\r", dynamicplaces[i]);
        }
      }
    }

    send_to_char("\n\n\r", ch);

    for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL) {
        continue;
      }
      if (IS_NPC(vch) || !is_dreaming(vch)) {
        continue;
      }
      if (vch == ch) {
        continue;
      }
      if (goddreamer(vch)) {
        continue;
      }

      if (vch->pcdata->dream_room == droom->vnum) {
        if (!IS_NPC(vch) && dream_slave(vch) && vch->pcdata->victimize_difficult_time > current_time) {
          printf_to_char(ch, "%s (`rDifficult Prisoner`x)", dream_name(vch));
        }
        else if (safe_strlen(vch->pcdata->dreamtitle) > 2) {
          printf_to_char(ch, "%s %s\n\r", dream_name(vch), vch->pcdata->dreamtitle);
        }
        else if (safe_strlen(vch->pcdata->dreamplace) > 2) {
          printf_to_char(ch, "%s is at %s\n\r", dream_name(vch), vch->pcdata->dreamplace);
        }
        else {
          printf_to_char(ch, "%s, %s\n\r", dream_name(vch), dream_introduction(vch));
        }
      }
    }
    return;
  }

  bool desire_room(ROOM_INDEX_DATA *room, CHAR_DATA *viewer)
  {
    CHAR_DATA *character;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING) {
        continue;
      }

      character = CH(d);
      if(is_gm(character))
      continue;
      if(character == viewer || character->pcdata->account == viewer->pcdata->account)
      continue;
      if(character->in_room == room && seems_suggestible(character) && !IS_FLAG(character->act, PLR_SHROUD))
      return TRUE;
    }
    return FALSE;
  }

  bool suffering_room(ROOM_INDEX_DATA *room, CHAR_DATA *viewer)
  {
    CHAR_DATA *character;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING) {
        continue;
      }

      character = CH(d);
      if(is_gm(character))
      continue;
      if(character == viewer || character->pcdata->account == viewer->pcdata->account)
      continue;

      if(character->in_room == room && seems_suffer_sensitive(character) && !IS_FLAG(character->act, PLR_SHROUD))
      return TRUE;
    }
    return FALSE;
  }
  bool fate_room(ROOM_INDEX_DATA *room, CHAR_DATA *viewer)
  {
    CHAR_DATA *character;
    for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING) {
        continue;
      }

      character = CH(d);
      if(is_gm(character))
      continue;
      if(character == viewer || character->pcdata->account == viewer->pcdata->account)
      continue;

      if(character->in_room == room && seems_fatesensitive(character) && !IS_FLAG(character->act, PLR_SHROUD))
      return TRUE;
    }
    return FALSE;
  }



  _DOFUN(do_look) {
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *current_room;
    OBJ_DATA *obj;
    char *pdesc;
    int door, i;
    int number, count;

    if (ch->desc == NULL) {
      return;
    }
    if (ch->in_room == NULL) {
      return;
    }

    if (is_dreaming(ch)) {
      if (ch->pcdata->dream_room == 0 && ch->pcdata->dream_link != NULL) {
        if (!str_cmp(argument, "")) {
          if (safe_strlen(ch->pcdata->dream_environment) > 2) {
            send_to_char(ch->pcdata->dream_environment, ch);
          }
          else if (safe_strlen(ch->pcdata->dream_link->pcdata->dream_environment) > 2) {
            send_to_char(ch->pcdata->dream_link->pcdata->dream_environment, ch);
          }
          else {
            send_to_char("A vast grey void.\n\r", ch);
          }

          printf_to_char(ch, "\n%s\n\r", PERS(ch->pcdata->dream_link, ch));
          return;
        }
        else {
          char temp[MSL];
          remove_color(temp, PERS(ch->pcdata->dream_link, ch));
          if (is_name(argument, temp)) {
            printf_to_char(ch, "This is %s.\n%s\n\r", ch->pcdata->dream_link->pcdata->dream_intro, ch->pcdata->dream_link->pcdata->dream_description);
            return;
          }
          else if (!str_cmp(argument, "self") || !str_cmp(argument, "me") || is_name(argument, ch->name) || is_name(argument, ch->pcdata->dream_intro)) {
            printf_to_char(ch, "This is %s.\n%s\n\r", ch->pcdata->dream_intro, ch->pcdata->dream_description);
            return;
          }

          send_to_char("You don't see them here.\n\r", ch);
          return;
        }
        return;
      }
      current_room = get_room_index(ch->pcdata->dream_room);
      if (!str_cmp(argument, "")) {
        show_dreamroom_to_char(ch, current_room);
        return;
      }
    }
    else {
      current_room = ch->in_room;
    }

    if (is_asleep(ch) && !is_dreaming(ch) && !IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You can't see anything, you're sleeping!\n\r", ch);
      return;
    }
    if (!check_blind(ch) && !is_dreaming(ch) && !IS_FLAG(ch->act, PLR_SHROUD)) {
      return;
    }
    if (gravesite(current_room) && gravename(ch, argument)) {
      show_grave(ch, argument);
      return;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    number = number_argument(arg1, arg3);
    count = 0;

    if (!str_cmp(arg1, "wardrobe") && current_room->area->vnum == 3 && current_room->vnum % 5 == 3) {
      do_function(ch, &do_stash, "");
      return;
    }

    if (!str_cmp(arg1, "photos")) {
      if (ch->pcdata->photo_msgs == NULL)
      return;
      if (safe_strlen(ch->pcdata->photo_msgs) > MSL) {
        send_to_char("Photo buffer is full.\n\r", ch);
        return;
      }

      page_to_char(ch->pcdata->photo_msgs, ch);
      return;
    }

    if (arg1[0] == '\0' || !str_cmp(arg1, "auto")) {
      if (current_room == NULL) {
        return;
      }
      /* 'look' or 'look auto' */
      if (is_dreaming(ch)) {
        sprintf(buf, "%s", current_room->name);
      }
      else if (IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
        sprintf(buf, "`DDeep in the Nightmare`x");
      }
      else if (current_room->sector_type == SECT_FOREST && (safe_strlen(current_room->name) < 2 || !str_cmp(current_room->name, "The Forest") || !str_cmp(current_room->name, "`GT`gh`Ge `GF`go`Dr`ge`Gs`gt`x")) && !battleground(current_room)) {

        tm *ptm;
        ptm = gmtime(&current_time);
        int month = ptm->tm_mon;
        if (in_world(ch) == WORLD_WILDS) {
          month += 5;
        }
        else if (in_world(ch) == WORLD_OTHER) {
          month += 1;
        }
        else if (in_world(ch) == WORLD_GODREALM) {
          month -= 2;
        }

        if (month < 0) {
          month += 12;
        }
        if (month > 11) {
          month -= 12;
        }

        if (month == 10 || month == 9 || month == 8) {
          sprintf(buf, "`gIn `gT`yh`ge `gF`yo`Dr`ye`gs`yt`x");
        }
        else if (month == 11 || month == 0 || month == 1) {
          sprintf(buf, "`WIn `WT`wh`We `WF`wo`yr`we`Ws`wt`x");
        }
        else if (month == 2 || month == 3 || month == 4) {
          sprintf(buf, "`GIn `GT`gh`Ge `GF`go`Mr`ge`Gs`gt`x");
        }
        else {
          sprintf(buf, "`GIn `GT`gh`Ge `GF`go`Dr`ge`Gs`gt`x");
        }
      }
      else {
        sprintf(buf, "%s", roomtitle(current_room, TRUE));
      }
      send_to_char(buf, ch);

      if ((IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_FLAG(ch->act, PLR_HOLYLIGHT)))) {
        sprintf(buf, " `r[`RRoom %d: %d, %d, %d`r]", current_room->vnum, current_room->x, current_room->y, current_room->z);
        send_to_char(buf, ch);
      }
      if (current_room->encroachment > 0 && IS_IMMORTAL(ch)) {
        sprintf(buf, "[`g%d`x]", current_room->encroachment);
        send_to_char(buf, ch);
      }
      if (room_level(current_room) > 0 && IS_IMMORTAL(ch)) {
        sprintf(buf, "[`R%d`x]", room_level(current_room));
        send_to_char(buf, ch);
      }
      if (is_air(current_room) && IS_IMMORTAL(ch)) {
        sprintf(buf, "(`CAir`x)");
        send_to_char(buf, ch);
      }

      if (IS_FLAG(ch->act, PLR_SHROUD)) {
        sprintf(buf, " `x[`DNightmare`x]");
        send_to_char(buf, ch);
      }
      if (!IS_NPC(ch) && ch->pcdata->institute_action == INSTITUTE_EXPERIMENT) {
        send_to_char(" (`YExperimenting`x)", ch);
      }
      if (!IS_NPC(ch) && ch->pcdata->institute_action == INSTITUTE_TREAT) {
        send_to_char(" (`YTreating`x)", ch);
      }
      if (!IS_NPC(ch) && ch->pcdata->institute_action == INSTITUTE_TRAUMA) {
        send_to_char(" (`YTraumatizing`x)", ch);
      }
      if (!IS_NPC(ch) && ch->pcdata->institute_action == INSTITUTE_TEACH) {
        send_to_char(" (`YTeaching`x)", ch);
      }
      if (IS_FLAG(ch->comm, COMM_PRIVATE)) {
        send_to_char(" [`rPrivate`x]", ch);
      }
      if (!IS_NPC(ch) && ch->pcdata->rp_logging == 1) {
        send_to_char(" [`gLogging`x]", ch);
      }

      if (is_animal(ch)) {
        sprintf(buf, " `x[`y%s`x]", get_animal_species(ch, ANIMAL_ACTIVE));
        send_to_char(buf, ch);
      }
      if (is_manifesting(ch)) {
        sprintf(buf, " `x[`WManifesting`x]");
        send_to_char(buf, ch);
      }
      if (is_visible(ch)) {
        sprintf(buf, " `x[`WVisible`x]");
        send_to_char(buf, ch);
      }
      if (event_cleanse == 0 && ch->pcdata->litup == 1) {
        sprintf(buf, " `x[`WLight up`x]");
        send_to_char(buf, ch);
      }

      if (in_fantasy(ch) != NULL && part_of_fantasy(ch, in_fantasy(ch))) {
        sprintf(buf, " `c{`x%s`c}`x", in_fantasy(ch)->name);
        send_to_char(buf, ch);
      }
      else if (room_fantasy(current_room) != NULL) {
        sprintf(buf, " `r{`x%s`r}`x", room_fantasy(current_room)->name);
        send_to_char(buf, ch);
      }

      if (!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_SENSING_ARTIFACT && ch->pcdata->patrol_target != NULL && ch->pcdata->patrol_timer > 0) {
        if (ch->in_room == ch->pcdata->patrol_target->in_room) {
          printf_to_char(ch, "You sense an awakening cursed object on %s.\n\r", PERS(ch->pcdata->patrol_target, ch));
        }
        else {
          printf_to_char(
          ch, "You sense the awakening of a cursed object to the %s.\n\r", relspacial[get_reldirection(
          roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(ch->pcdata->patrol_target->in_room), get_roomy(ch->pcdata->patrol_target->in_room)), ch->facing)]);
        }
      }
      if (!IS_NPC(ch) && ch->pcdata->patrol_timer > 0 && (ch->pcdata->patrol_status == PATROL_SENSING_GHOST || ch->pcdata->patrol_status == PATROL_GHOST_FOCUS)) {
        if (ch->in_room == ch->pcdata->patrol_room) {
          send_to_char("\nYou sense a rising malevolent spirit in this area.\n\r", ch);
        }
        else {
          printf_to_char(
          ch, "\nYou sense a rising malevolent to the %s.\n\r", relspacial[get_reldirection(
          roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(ch->pcdata->patrol_room), get_roomy(ch->pcdata->patrol_room)), ch->facing)]);
        }
      }
      if (is_water(current_room)) {
        if (is_underwater(current_room))
        sprintf(buf, " `x[`BUnderwater`x]");
        else if (deep_water(ch))
        sprintf(buf, " `x[`BDeep Water`x]");
        else
        sprintf(buf, " `x[`BShallow Water`x]");

        send_to_char(buf, ch);
      }
      if (crowded_room(current_room)) {
        sprintf(buf, " `x[`CCrowded`x]");
        send_to_char(buf, ch);
      }
      if(desire_room(current_room, ch)) {
        sprintf(buf, " `x[`WDesires`x]");
        send_to_char(buf, ch);
      }
      if(suffering_room(current_room, ch)) {
        sprintf(buf, " `x[`rSuffers`x]");
        send_to_char(buf, ch);
      }
      if(fate_room(current_room, ch)) {
        sprintf(buf, " `x[`034Fate`x]");
        send_to_char(buf, ch);
      }

      if (IS_FLAG(ch->act, PLR_HIDE)) {
        sprintf(buf, " `x[`DHidden`x]");
        send_to_char(buf, ch);
      }



      send_to_char("`x\n\r", ch);
      int llevel = light_level(current_room);
      if (llevel >= 90)
      send_to_char("`231The area is very bright.`x\n\r", ch);
      else if (llevel >= 75)
      send_to_char("`226The area is bright.`x\n\r", ch);
      else if (llevel >= 50)
      send_to_char("`178The area is dim.`x\n\r", ch);
      else if (llevel >= 20)
      send_to_char("`038The area is quite dark.`x\n\r", ch);
      else if (llevel >= 0)
      send_to_char("`243The area is dark.`x\n\r", ch);
      else
      send_to_char("`236The area is pitch black.`x\n\r", ch);

      if (!IS_SET(current_room->room_flags, ROOM_UNLIT) && !IS_SET(current_room->room_flags, ROOM_DARK) && current_room->sector_type ==
          SECT_STREET) // Street Lights - Disco 10/30/2017
      {
        if (is_dark_outside()) {
          send_to_char("The `Yglow`x of old street lights spills onto the pavement.`x\n\r", ch);
        }
        else {
          send_to_char("The street lights are cold and dead.`x\n\r", ch);
        }
      }
      else if (is_dark(current_room)) {
        send_to_char("It is `Ddark`x.\n\r", ch);
      }

      if(is_gm(ch) && sandbox_room(current_room))
      send_to_char("This is a sandbox room, you can redesign it with the 'decorate' command.\n\r", ch);
      else if(get_gm(current_room, FALSE) == NULL && sandbox_room(current_room))
      send_to_char("This is a sandbox room, you can redesign it with the 'decorate' command.\n\r", ch);

      if (str_cmp(crisis_atmosphere, ""))
      printf_to_char(ch, "%s\n\r", crisis_atmosphere);
      if (battleground(ch->in_room) && activeoperation != NULL && safe_strlen(activeoperation->atmosphere) > 2)
      printf_to_char(ch, "%s\n\r", activeoperation->atmosphere);

      if (get_skill(ch, SKILL_HACKING) + get_skill(ch, SKILL_ENGINEERING) > 4 && bugged_room(current_room))
      send_to_char("`cYou pick up wireless signals consistent with spy cameras.`x\n\r", ch);

      if (IS_SET(ch->in_room->room_flags, ROOM_LANDLINE) && has_place(ch->in_room, "payphone")) {
        send_to_char("`WThere is a payphone nearby.`x\n\r", ch);
      }
      else if (IS_SET(ch->in_room->room_flags, ROOM_LANDLINE)) {
        send_to_char("`WThere is a landline phone nearby.`x\n\r", ch);
      }

      if (in_medical_facility(ch)) {
        send_to_char("`WThis is a medical facility.`x\n\r", ch);
      }

      if (IS_SET(ch->in_room->room_flags, ROOM_SIGNALBOOST)) {
        send_to_char("`WThere is especially good cell reception here.`x\n\r", ch);
      }
      if (private_school_room(ch->in_room)) {
        send_to_char("`WThis room seems private.`x\n\r", ch);
      }

      if (current_room->vnum == ROOM_INDEX_GENESIS) {
        genesis_display(ch);
        return;
      }
      if (in_haven(current_room) && time_info.bloodstorm == 1) {
        if (in_lodge(current_room) || (institute_room(current_room) && current_room->z < 0))
        send_to_char("`rA mystical storm rages outside.\n\r", ch);
        else
        send_to_char("`RA mystical storm rages painfully around you.\n\r", ch);
      }
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
        if (mirror_room(current_room))
        send_to_char("A black void continues in all directions, split only by a shining white rectangle hanging in mid-air.\n\r", ch);
        else
        send_to_char("A black void continues in all directions, even the ground beneath you seems to lack solidity.\n\r", ch);
      }
      else if (!is_dark(current_room) || can_see_dark(ch)) {
        if (public_room(current_room) && !is_dreaming(ch)) {
          send_to_char("`WThis area seems well populated`x.\n\r", ch);
        }
        if (IS_SET(current_room->room_flags, ROOM_CAMPSITE))
        send_to_char("`gThere's a campsite here.`x\n\r", ch);
        if (IS_SET(current_room->room_flags, ROOM_ANIMALHOME))
        send_to_char("`yAn animal has made its home here.`x\n\r", ch);
        if (IS_SET(current_room->room_flags, ROOM_LIGHTON) &&  crisis_blackout == 0)
        send_to_char("`YThe lights are on.`x\n\r", ch);
        if (IS_SET(current_room->room_flags, ROOM_DIRTROAD) && current_room->sector_type == SECT_STREET)
        send_to_char("`yIt's a dirt road.`x\n\r", ch);
        if (in_prop(ch) != NULL && !is_dreaming(ch)) {
          if (current_room->encroachment >= 850)
          send_to_char("`gThe area is almost entirely overgrown.`x\n\r", ch);
          else if (current_room->encroachment >= 700)
          send_to_char("`gThe area is heavily overgrown.`x\n\r", ch);
          else if (current_room->encroachment >= 500)
          send_to_char("`gInvading roots and vines have started to damage the structure.`x\n\r", ch);
          int decor = get_decor(current_room);
          if (decor == 0)
          send_to_char("It has cheap decor.\n\r", ch);
          else if (decor == 1)
          send_to_char("It has average decor.\n\r", ch);
          else if (decor == 2)
          send_to_char("`cIt has expensive decor.`x\n\r", ch);
          else
          send_to_char("`WIt has extravagant decor.`x\n\r", ch);
          if (IS_SET(current_room->room_flags, ROOM_BEDROOM))
          send_to_char("It is a bedroom. ", ch);
          if (IS_SET(current_room->room_flags, ROOM_BATHROOM))
          send_to_char("It is a bathroom. ", ch);
          if (IS_SET(current_room->room_flags, ROOM_KITCHEN))
          send_to_char("It is a kitchen. ", ch);
          send_to_char("\n\r", ch);
        }
        else {
          if (current_room->sector_type == SECT_STREET && !IS_SET(current_room->room_flags, ROOM_DIRTROAD)) {
            if (current_room->encroachment >= 850)
            send_to_char("`gThe road is a webwork of pot holes and cracks where vines break through the asphalt.`x\n\r", ch);
            else if (current_room->encroachment >= 700)
            send_to_char("`gThe road is littered with cracks where vines break through the asphalt.`x\n\r", ch);
            else if (current_room->encroachment >= 500)
            send_to_char("`gThere's a few cracks in the road, greenery visible beneath.`x\n\r", ch);
          }
          else if (current_room->sector_type == SECT_STREET) {
            if (current_room->encroachment >= 850)
            send_to_char("`gThe road is a webwork of pot holes and tree roots.`x\n\r", ch);
            else if (current_room->encroachment >= 700)
            send_to_char("`gThe road is heavily marred by pot holes and tree roots.`x\n\r", ch);
            else if (current_room->encroachment >= 500)
            send_to_char("`gThere's a few tree roots pushing up through the surface of the road.`x\n\r", ch);
          }
          else {
            if (current_room->encroachment >= 850)
            send_to_char("`gThe area is extremely overgrown.`x\n\r", ch);
            else if (current_room->encroachment >= 700)
            send_to_char("`gThe area is heavily overgrown.`x\n\r", ch);
            else if (current_room->encroachment >= 500)
            send_to_char("`gThe area is starting to become overgrown.`x\n\r", ch);
          }
        }

        if (arg1[0] == '\0' || (!IS_NPC(ch) && !IS_FLAG(ch->comm, COMM_BRIEF))) {
          send_to_char("  ", ch);
          show_room_to_char(ch, current_room);
        }

        if (current_room->extra_descr != NULL && get_extra_descr("!sleepers", current_room->extra_descr) != NULL) {
          printf_to_char(ch, "\n%s\n\r", get_extra_descr("!sleepers", current_room->extra_descr));
        }

        for (i = 0; i < 10; i++) {
          if (current_room->features[i] != NULL && current_room->features[i]->type > 0)
          printf_to_char(ch, "%s\n\r", current_room->features[i]->shortdesc);
        }

        if (get_skill(ch, SKILL_ACUTESMELL) > 0 && ch->pcdata->sniffing == TRUE && !IS_FLAG(ch->act, PLR_SHROUD)) {
          bool firstsmell = TRUE;
          for (int i = 0; i < 3; i++) {
            if (safe_strlen(current_room->smell_desc[i]) > 2) {
              if (firstsmell == TRUE) {
                send_to_char("`yYou smell:`x\n\r", ch);
              }
              printf_to_char(ch, "%s leading %s.\n\r", current_room->smell_desc[i], reldir_name[get_reldirection(
              current_room->smell_dir[i], ch->facing)][0]);
              firstsmell = FALSE;
            }
          }
        }

        if (room_jeopardy(current_room)) {
          send_to_char("\n`RThis room is at risk of being recycled because of the inactivity of it's author, if you want to save it you can adopt it by typing '`Wadopt`R'`x\n\n\r", ch);
        }

        if (current_room->extra_descr != NULL) {

          if (get_skill(ch, SKILL_NIGHTVISION) > 1 && get_extra_descr("dark", current_room->extra_descr) != NULL)
          send_to_char(get_extra_descr("dark", current_room->extra_descr), ch);
          if (get_skill(ch, SKILL_ACUTESIGHT) > 0 && get_extra_descr("acutesight", current_room->extra_descr) != NULL)
          send_to_char(get_extra_descr("acutesight", current_room->extra_descr), ch);
          if (get_skill(ch, SKILL_ACUTEHEAR) > 0 && get_extra_descr("acutehearing", current_room->extra_descr) != NULL)
          send_to_char(
          get_extra_descr("acutehearing", current_room->extra_descr), ch);
          if (get_skill(ch, SKILL_ACUTESMELL) > 0 && get_extra_descr("acutesmell", current_room->extra_descr) != NULL)
          send_to_char(get_extra_descr("acutesmell", current_room->extra_descr), ch);
        }

        ghostwalker_check(ch);
        if (has_places(current_room)) {
          EXTRA_DESCR_DATA *ed;
          for (ed = current_room->places; ed; ed = ed->next) {
            if (!IS_NPC(ch) && !str_cmp(ch->pcdata->place, ed->keyword)) {
              send_to_char("\n\n\r", ch);
              send_to_char(get_extra_descr(ed->keyword, current_room->places), ch);
            }
          }
        }
        if (gravesite(current_room) && gravename(ch, ch->pcdata->place)) {
          send_to_char("\n\n\r", ch);
          show_grave(ch, ch->pcdata->place);
        }
        send_to_char("\n\r", ch);

        if (has_extradesc(current_room)) {
          EXTRA_DESCR_DATA *ed;
          char buf1[MSL];
          sprintf(buf1, "Extra: ");
          for (ed = current_room->extra_descr; ed; ed = ed->next) {
            if (str_cmp(ed->keyword, "!keys") && str_cmp(ed->keyword, "dark") && str_cmp(ed->keyword, "temporary") && str_cmp(ed->keyword, "!sleepers") && str_cmp(ed->keyword, "!srtemporary") && str_cmp(ed->keyword, "acutesight") && str_cmp(ed->keyword, "acutehearing") && str_cmp(ed->keyword, "acutesmell") && str_cmp(ed->keyword, "!denial") && str_cmp(ed->keyword, "!bugs") && str_cmp(ed->keyword, "!srhidden") && str_cmp(ed->keyword, "!hidden") && str_cmp(ed->keyword, "!roomtitle") && str_cmp(ed->keyword, "!atmosphere")) {
              strcat(buf1, ed->keyword);
              if (ed->next) {
                strcat(buf1, ", ");
              }
            }
          }
          strcat(buf1, "\n\r");
          send_to_char(buf1, ch);
        }

        if (has_places(current_room)) {
          EXTRA_DESCR_DATA *ed;
          char buf1[MSL];
          sprintf(buf1, "Places: ");
          for (ed = current_room->places; ed; ed = ed->next) {
            strcat(buf1, ed->keyword);
            if (ed->next) {
              strcat(buf1, ", ");
            }
          }
          strcat(buf1, "\n\r");
          send_to_char(buf1, ch);
        }
        if (has_dynamic_places(current_room)) {
          char *dynamicplaces[20];
          for (int i = 0; i < 20; i++) {
            dynamicplaces[i] = str_dup("");
          }
          CHAR_DATA *person;
          for (CharList::iterator it = current_room->people->begin();
          it != current_room->people->end(); ++it) {
            person = *it;
            if (person == NULL || person->in_room == NULL || IS_NPC(person)) {
              continue;
            }

            if (safe_strlen(person->pcdata->place) > 1) {
              EXTRA_DESCR_DATA *ed;
              bool staticplace = FALSE;
              for (ed = current_room->places; ed; ed = ed->next) {
                if (!str_cmp(person->pcdata->place, ed->keyword)) {
                  staticplace = TRUE;
                }
              }
              if (staticplace == FALSE) {
                bool placefound = FALSE;
                for (int i = 0; i < 20; i++) {
                  if (!str_cmp(person->pcdata->place, dynamicplaces[i])) {
                    placefound = TRUE;
                  }
                }
                if (placefound == FALSE) {
                  bool placeplaced = FALSE;
                  for (int i = 0; i < 20; i++) {
                    if (safe_strlen(dynamicplaces[i]) < 2 && placeplaced == FALSE) {
                      free_string(dynamicplaces[i]);
                      dynamicplaces[i] = str_dup(person->pcdata->place);
                      placeplaced = TRUE;
                    }
                  }
                }
              }
            }
          }
          char buf2[MSL];
          sprintf(buf2, "Dynamic Places: ");
          for (int i = 0; i < 20; i++) {
            if (safe_strlen(dynamicplaces[i]) > 2) {
              strcat(buf2, dynamicplaces[i]);
              if (i < 19 && safe_strlen(dynamicplaces[i + 1]) > 2) {
                strcat(buf2, ", ");
              }
            }
          }
          strcat(buf2, "\n\r");
          send_to_char(buf2, ch);
        }
        if (has_cars(current_room)) {
          char buf1[MSL];
          sprintf(buf1, "Vehicles: ");
          bool notfirst = FALSE;
          for (int y = 0; y < 10; y++) {
            if (current_room->vehicle_cost[y] > 0) {
              if (notfirst) {
                strcat(buf1, ", ");
              }
              strcat(buf1, current_room->vehicle_names[y]);
              notfirst = TRUE;
            }
          }
          strcat(buf1, "\n\r");
          send_to_char(buf1, ch);
        }
        if (plaqueroom(current_room)) {
          send_to_char("There are plaques here. (Look plaques)\n\r", ch);
        }
        if (graveroom(current_room)) {
          send_to_char("There are graves here. (Look graves)\n\r", ch);
        }

        if (IS_SET(current_room->room_flags, ROOM_INDOORS)) {
        }
        else {
        }
        if (!IS_NPC(ch)) {
        }
      }

      if(!IS_NPC(ch) && (current_room->vnum == 1356 || current_room->vnum == 1494 || current_room->vnum == 3335 || current_room->vnum == 9597))
      college_roster(ch, current_room);

      if (!IS_NPC(ch)) {
        send_to_char("\n\r", ch);
        do_function(ch, &do_exits, "auto");
      }

      show_list_to_char(current_room->contents, ch, FALSE, FALSE, NULL);
      send_to_char("\n\r", ch);
      // Listing occupants
      if (!in_fight(ch)) {
        if (is_dreaming(ch)) {
          if (ch->pcdata->dream_room > 0) {
            for (DescList::iterator it = descriptor_list.begin();
            it != descriptor_list.end(); ++it) {
              CHAR_DATA *vch;
              DESCRIPTOR_DATA *d = *it;
              vch = CH(d);
              if (vch == NULL) {
                continue;
              }
              if (IS_NPC(vch) || !is_dreaming(vch)) {
                continue;
              }
              if (vch == ch) {
                continue;
              }
              if (goddreamer(vch)) {
                continue;
              }
              if (vch->pcdata->dream_room == current_room->vnum) {
                printf_to_char(ch, "%s\n\r", dream_name(vch));
              }
            }
          }
          else {
            char temp[MSL];
            remove_color(temp, PERS(ch->pcdata->dream_link, ch));
            if (is_name(argument, temp)) {
              printf_to_char(ch, "This is %s.\n%s\n\r", ch->pcdata->dream_link->pcdata->dream_intro, ch->pcdata->dream_link->pcdata->dream_description);
              return;
            }
            else if (is_name(argument, "self") || is_name(argument, ch->name) || is_name(argument, ch->pcdata->dream_intro)) {
              printf_to_char(ch, "This is %s.\n%s\n\r", ch->pcdata->dream_intro, ch->pcdata->dream_description);
              return;
            }

            send_to_char("You don't see them here.\n\r", ch);
            return;
          }
        }
        else if (crowded_room(current_room)) {
          show_crowded_char_to_char(current_room->people, ch);
        }
        else if (!str_cmp(arg1, "auto")) {
          show_char_to_char(current_room->people, ch, 1);
        }
        else {
          show_char_to_char(current_room->people, ch, 0);
        }
        remote_view(ch);
      }
      else {
        scan_fight(ch, FALSE);
      }

      if (current_room != NULL && current_room->vnum >= 19000 && current_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && in_haven(get_room_index(ch->pcdata->travel_to)) && in_haven(get_room_index(ch->pcdata->travel_from)) && in_haven(ch->your_car->in_room) && current_room->vnum != ch->your_car->in_room->vnum) {
        ROOM_INDEX_DATA *orig = current_room;
        char_from_room(ch);
        char_to_room(ch, ch->your_car->in_room);
        do_function(ch, &do_look, "");
        char_from_room(ch);
        char_to_room(ch, orig);
      }

      for (door = 0; door < MAX_DIR; door++) {
        if ((pexit = current_room->exit[door]) != NULL) {
        }
      }
      return;
    }

    door = -1;
    if (!IS_FLAG(ch->comm, COMM_CARDINAL) || ch->played / 3600 <= 20 || in_fight(ch)) {
      if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
      door = 0;
      else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
      door = 1;
      else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
      door = 2;
      else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
      door = 3;
      else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
      door = 4;
      else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
      door = 5;
      else if (!str_cmp(arg1, "ne") || !str_cmp(arg1, "northeast"))
      door = 6;
      else if (!str_cmp(arg1, "nw") || !str_cmp(arg1, "northwest"))
      door = 7;
      else if (!str_cmp(arg1, "se") || !str_cmp(arg1, "southeast"))
      door = 8;
      else if (!str_cmp(arg1, "sw") || !str_cmp(arg1, "southwest"))
      door = 9;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) || ch->played / 3600 <= 20) {
      for (int j = 0; j < 10; j++) {
        if (!str_cmp(arg1, reldir_name[j][0]) || !str_cmp(arg1, reldir_name[j][1])) {
          door = get_absoldirection(j, ch->facing);
        }
      }
    }
    if (door != -1) {
      if (in_fight(ch) && ch->facing != door)
      ch->debuff += 5;

      ch->facing = door;
      do_function(ch, &do_look, "auto");
      return;
    }

    if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in") || !str_cmp(arg1, "on")) {
      /* 'look in' */
      if (arg2[0] == '\0') {
        send_to_char("Look in what?\n\r", ch);
        return;
      }

      if ((obj = get_obj_here(ch, NULL, arg2)) == NULL) {
        send_to_char("You do not see that here.\n\r", ch);
        return;
      }

      switch (obj->item_type) {
      default:
        send_to_char("That is not a container.\n\r", ch);
        break;

      case ITEM_DRINK_CON:
        if (obj->value[1] <= 0) {
          send_to_char("It is empty.\n\r", ch);
          break;
        }

        sprintf(buf, "It's %sfilled with liquid.\n\r", obj->value[1] < obj->value[0] / 4
        ? "less than half-" : obj->value[1] < 3 * obj->value[0] / 4 ? "about half-" : "more than half-");

        send_to_char(buf, ch);
        break;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        if (IS_SET(obj->value[1], CONT_CLOSED)) {
          send_to_char("It is closed.\n\r", ch);
          break;
        }
        else {
          act("$p holds:", ch, obj, NULL, TO_CHAR);
          show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
        }
        break;
      }
      return;
    }

    // Looking at characters
    if (is_dreaming(ch)) {
      if ((victim = get_char_dream(ch, arg1)) != NULL && victim->pcdata->dream_room == ch->pcdata->dream_room && !goddreamer(victim)) {
        if (safe_strlen(dream_fame(victim)) > 1)
        printf_to_char(ch, "This is %s.\nThey are known for %s.\n%s\n\n%s\n\r", dream_introduction(victim), dream_fame(victim), dream_description(victim), dream_eq(victim));
        else
        printf_to_char(ch, "This is %s.\n%s\n\n%s\n\r", dream_introduction(victim), dream_description(victim), dream_eq(victim));
        return;
      }
      else if (!str_cmp(arg1, "self") || !str_cmp(arg1, "me")) {
        if (safe_strlen(dream_fame(ch)) > 1)
        printf_to_char(ch, "This is %s.\nThey are known for %s.\n%s\n\n%s\n\r", dream_introduction(ch), dream_fame(ch), dream_description(ch), dream_eq(ch));
        else
        printf_to_char(ch, "This is %s.\n%s\n\n%s\n\r", dream_introduction(ch), dream_description(ch), dream_eq(ch));
        return;
      }
    }
    else if ((victim = get_char_room(ch, NULL, arg1)) != NULL) {
      if (!show_char_location_to_char(ch, victim, arg2)) {
        if (!show_char_obj_to_char(ch, victim, arg2))
        show_char_to_char_1(victim, ch, LOOK_LOOK, FALSE);
      }
      return;
    }
    else if ((victim = get_char_distance(ch, arg1, 3)) != NULL) {
      if (!show_char_location_to_char(ch, victim, arg2)) {
        if (!show_char_obj_to_char(ch, victim, arg2))
        show_char_to_char_1(victim, ch, LOOK_LOOK, FALSE);
      }
      return;
    }
    for (i = 0; i < 10; i++) {
      if (current_room->features[i] != NULL && current_room->features[i]->type > 0 && is_name(arg1, current_room->features[i]->names)) {
        printf_to_char(ch, "%s\n\r", current_room->features[i]->desc);
        return;
      }
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj) && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) { /* player can see object */
        if (is_name(arg3, obj->name) && count + 1 == number) {
          show_obj_to_char(obj, ch, FALSE, TRUE);
          if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->value[1], CONT_CLOSED)) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }
          if (obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }
          return;
        }
        else {
          pdesc = get_extra_descr_obj(arg3, obj->extra_descr, obj);
          if (pdesc != NULL) {
            if (++count == number && isalpha(arg3[0])) {
              send_to_char(pdesc, ch);
              return;
            }
            else {
              continue;
            }
          }
          pdesc = get_extra_descr_obj(arg3, obj->pIndexData->extra_descr, obj);
          if (pdesc != NULL) {
            if (++count == number && isalpha(arg3[0])) {
              send_to_char(pdesc, ch);
              return;
            }
            else {
              continue;
            }
          }
        }
      }
    }

    for (obj = current_room->contents; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj)) {
        /*
        pdesc = get_extra_descr( arg3, obj->extra_descr );
        if ( pdesc != NULL )
        if (++count == number)
        {
        send_to_char( pdesc, ch );
        return;
        }

        pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
        if ( pdesc != NULL )
        if (++count == number)
        {
        send_to_char( pdesc, ch );
        return;
        }
        */
        if (is_name(arg3, obj->name))
        if (++count == number) {
          show_obj_to_char(obj, ch, FALSE, TRUE);

          if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->value[1], CONT_CLOSED)) {
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE, NULL);
          }

          return;
        }
      }
    }

    pdesc = get_extra_descr(arg3, current_room->extra_descr);
    if (pdesc != NULL) {
      if (!str_cmp(arg3, "!keys"))
      return;
      if (!str_cmp(arg3, "temporary"))
      return;
      if (!str_cmp(arg3, "!srtemporary"))
      return;
      if (!str_cmp(arg3, "!srhidden"))
      return;
      if (!str_cmp(arg3, "!atmosphere"))
      return;
      if (!str_cmp(arg3, "!hidden"))
      return;
      if (!str_cmp(arg3, "dark") && get_skill(ch, SKILL_NIGHTVISION) < 2)
      return;
      if (!str_cmp(arg3, "acutesight") && get_skill(ch, SKILL_ACUTESIGHT) < 1)
      return;
      if (!str_cmp(arg3, "acutehearing") && get_skill(ch, SKILL_ACUTEHEAR) < 1)
      return;
      if (!str_cmp(arg3, "acutesmell") && get_skill(ch, SKILL_ACUTESMELL) < 1)
      return;
      if (!str_cmp(arg3, "!sleepers"))
      return;
      if (!str_cmp(arg3, "!denial"))
      return;
      if (!str_cmp(arg3, "!bugs"))
      return;
      if (!str_cmp(arg3, "!roomtitle"))
      return;

      if (++count == number) {
        send_to_char(pdesc, ch);
        return;
      }
    }

    // dream or realworld place desc check
    if (is_dreaming(ch)) {
      ROOM_INDEX_DATA *droom;
      droom = get_room_index(ch->pcdata->dream_room);
      pdesc = get_extra_descr(arg3, droom->places);
    }
    else {
      pdesc = get_extra_descr(arg3, current_room->places);
    }

    if (pdesc != NULL) {
      if (++count == number) {
        send_to_char(pdesc, ch);
        show_places_char_to_char(current_room->people, ch, arg3); // BOOKMARK
        return;
      }
    }
    for (int y = 0; y < 10; y++) {
      if (is_name(arg3, from_color(current_room->vehicle_names[y])) && current_room->vehicle_cost[y] > 0) {
        if (current_room->vehicle_typeone[y] == CAR_HORSE)
        printf_to_char(ch, "%s\nIt is %s %s\n%s`x\n", current_room->vehicle_names[y], carqualityname(current_room->vehicle_cost[y], current_room->vehicle_typeone[y], current_room->vehicle_typetwo[y]), cartypename(current_room->vehicle_typeone[y], current_room->vehicle_typetwo[y]), current_room->vehicle_descs[y]);
        else
        printf_to_char(ch, "%s\nIt is %s %s\n%s\n\t`W|`R%s`W|`x\n", current_room->vehicle_names[y], carqualityname(current_room->vehicle_cost[y], current_room->vehicle_typeone[y], current_room->vehicle_typetwo[y]), cartypename(current_room->vehicle_typeone[y], current_room->vehicle_typetwo[y]), current_room->vehicle_descs[y], current_room->vehicle_lplates[y]);
        return;
      }
    }
    if (plaqueroom(current_room) && !str_cmp(arg3, "plaques")) {
      show_plaques(ch);
      return;
    }
    if (graveroom(current_room) && !str_cmp(arg3, "graves")) {
      show_graves(ch);
      return;
    }
    CHAR_DATA *person;
    for (CharList::iterator it = current_room->people->begin();
    it != current_room->people->end(); ++it) {
      person = *it;
      if (person == NULL || person->in_room == NULL || IS_NPC(person))
      continue;

      if (is_name(arg3, person->pcdata->place) && safe_strlen(arg3) > 1) {
        show_places_char_to_char(current_room->people, ch, arg3);
        return;
      }
    }

    if (count > 0 && count != number) {
      if (count == 1)
      sprintf(buf, "You only see one %s here.\n\r", arg3);
      else
      sprintf(buf, "You only see %d of those here.\n\r", count);

      send_to_char(buf, ch);
      return;
    }

    if (!str_cmp(arg1, ""))
    do_function(ch, &do_look, "auto");
    else
    send_to_char("You do not see that here.\n\r", ch);

    return;
  }

  _DOFUN(do_examine) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      printf_to_char(ch, "Decor: %d, Toughness: %d, Security: %d\n\r", get_decor(ch->in_room), get_toughness(ch->in_room), get_security(ch->in_room));
      act("$n examines the room.", ch, NULL, NULL, TO_ROOM);
      return;
      send_to_char("Examine what?\n\r", ch);
      return;
    }

    do_function(ch, &do_look, arg);

    if ((obj = get_obj_here(ch, NULL, arg)) != NULL) {
      switch (obj->item_type) {
      default:
        break;

      case ITEM_JUKEBOX:
        do_function(ch, &do_play, "list");
        break;

      case ITEM_MONEY:
        if (obj->value[1] == 0)
        sprintf(buf, "Odd...there's no money in the pile.\n\r");
        else if (obj->value[1] == 1) {
          sprintf(buf, "Wow. One dollar.\n\r");
        }
        else {
          sprintf(buf, "There are %d dollars in the pile.\n\r", obj->value[1]);
        }
        break;
      case ITEM_CLOTHING:
        printf_to_char(ch, "Height Mod: %d\n\r", ci_heightmod(obj->value[2]));
        printf_to_char(ch, "Bust Mod: %d\n\r", ci_bustmod(obj->value[2]));
      case ITEM_JEWELRY:
        printf_to_char(ch, "Layer: %d\n\r", obj->level);
        printf_to_char(
        ch, "Names: %s\nShortstring: %s\nLongstring: %s\nWear String: %s\n\r", obj->name, obj->short_descr, obj->description, obj->wear_string);
        printf_to_char(ch, "Covers: %s\nZips: %s\nLayer: %d\n\r", get_cover_list(obj->value[0]), get_cover_list(obj->value[1]), obj->level);
        break;

      case ITEM_DRINK_CON:
      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        //	    sprintf(buf,"in %s",argument);
        //	    do_function(ch, &do_look, buf );
        break;
      case ITEM_PORTAL:
        break;
      }
    }

    return;
  }

  char *get_fac(CHAR_DATA *ch) {
    if (ch->faction != 0 && clan_lookup(ch->faction) != NULL)
    return clan_lookup(ch->faction)->name;
    return "None";
  }

  char *get_inj(CHAR_DATA *ch) {
    int val = ch->wounds;
    if (IS_AFFECTED(ch, AFF_BITTEN) && val > 0)
    val--;
    switch (val) {
    case 1:
      return "You are mildly wounded.";
    case 2:
      return "You are severely wounded.";
    case 3:
      return "You are critically wounded.";
    default:
      return "You aren't wounded.";
    }
  }

  int total_disc(CHAR_DATA *ch) {
    int i, total = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (ch->disciplines[discipline_table[i].vnum] > 0) {
        total += ch->disciplines[discipline_table[i].vnum];
      }
    }
    return total;
  }

  int total_skills(CHAR_DATA *ch) {
    int i, total = 0;
    for (i = 0; i < SKILL_USED; i++) {
      total += skillpointcost(ch, skill_table[i].vnum);
    }
    return total;
  }

  char *const tier_names[] = {"None", "One",  "Two", "Three", "Four", "Five", "Six"};

  char *datesuffix(int val) {
    if (val == 1)
    return "st";
    if (val == 2)
    return "nd";
    if (val == 3)
    return "rd";

    if (val == 21)
    return "st";
    if (val == 22)
    return "nd";
    if (val == 23)
    return "rd";

    if (val == 31)
    return "st";
    if (val == 32)
    return "nd";
    if (val == 33)
    return "rd";
    return "th";
  }


  char * get_day_suffix(int day) {
    if (day >= 11 && day <= 13) {
      return "th";
    }
    switch (day % 10) {
    case 1:
      return "st";
    case 2:
      return "nd";
    case 3:
      return "rd";
    default:
      return "th";
    }
  }

  // New and wider score with perfect spacing and some format changes - Discordance
  _DOFUN(do_score) {
    char spacer[] = "";
    char archetype[MSL], string[MSL], name[MSL];
    char border[MSL], fields[MSL], data[MSL];

    CHAR_DATA *target;

    int spacing, i = 0;

    char *const month_names[] = {
      "January", "February", "March", 
      "April", "May", "June", "July",
      "August", "September", "October",
      "November", "December"
    };

    if (IS_IMMORTAL(ch)) {
      one_argument(argument, name);
      if (name[0] != '\0') {
        if ((target = get_char_world(ch, name)) == NULL) {
          send_to_char("That player is not currently logged on.\n\r", ch);
          return;
        }
      }
      else {
        target = ch;
      }
    }
    else {
      target = ch;
    }

    if (IS_NPC(target))
    return;

    if (!IS_FLAG(target->comm, COMM_STORY) && target != ch) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    double value = target->money;
    double amount = target->pcdata->total_money;

    FACTION_TYPE *fac = clan_lookup(target->faction);

    value /= 100;
    amount /= 100;

    if (event_cleanse == 1)
    strcpy(archetype, "`YHuman`x");
    else {
      strcpy(archetype, race_table[target->race].who_name);
    }

    // defines color for score
    strcpy(border, "`g");
    strcpy(fields, "`c");
    strcpy(data, "`x");

    // Line 0 - Spacing
    send_to_char("\n\r", ch);
    // Line 1 - Border
    printf_to_char(ch, "%s,______________________________________________________________________________,`x\n\r", border); // Line 2 - Credit, Hours, Super
    printf_to_char(ch, "%s| ", border);
    sprintf(string, "%sCredit", fields);
    printf_to_char(ch, "%s", string);
    spacing = 31 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sHours%s: %s%d", fields, border, data, (target->played + (int)(current_time - target->logon)) / 3600);
    printf_to_char(ch, "%s", string);
    spacing = 40 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sSuper", fields);
    printf_to_char(ch, "%s", string);
    spacing = 6 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);
    // Line 3 - Credit data, Super data
    printf_to_char(ch, "%s| ", border);
    if (ch != target) {
      sprintf(string, " %d", UMIN(1000, available_donated(target)));
    }
    else {
      sprintf(string, " %d", available_donated(target));
    }
    printf_to_char(ch, "%s%s", data, string);
    spacing = 72 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%s[%s%s%s]", border, data, (is_super(target)) ? "X" : " ", border);
    printf_to_char(ch, "%s", string);
    spacing = 5 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);
    // Line 4 - Name, Sex, Tier Level
    printf_to_char(ch, "%s| ", border);
    sprintf(string, "%s       Name%s: %s%s %s", fields, border, data, target->name, target->pcdata->last_name);
    printf_to_char(ch, "%s", string);
    spacing = 33 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sSex%s: %s%s", fields, border, data, (target->sex == SEX_MALE) ? "Male" : "Female");
    printf_to_char(ch, "%s", string);
    spacing = 19 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sConcept Tier%s: %s%s", fields, border, data, tier_names[get_tier(target)]);
    printf_to_char(ch, "%s", string);
    spacing = 25 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);

    // Line 5 - Age, Concept Box Border
    printf_to_char(ch, "%s| ", border);
    sprintf(string, "%s        Age%s: %s%d", fields, border, data, get_real_age(target));
    printf_to_char(ch, "%s", string);
    spacing = 52 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%s,----------------------,", border);
    printf_to_char(ch, "%s", string);
    spacing = 25 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);
    // Line 6 - Birthdate, Archetype
    printf_to_char(ch, "%s|    ", border);
    if (get_age(target) != get_real_age(target)) {
      sprintf(string, "%sApparent%s: %s%d", fields, border, data, get_age(target));
      printf_to_char(ch, "%s", string);
      spacing = 39 - safe_strlen_color(string);
    }
    else {
      spacing = 39;
    }
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sArchetype %s| ", fields, border);
    printf_to_char(ch, "%s", string);
    spacing = 12 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%s%s", data, archetype);
    printf_to_char(ch, "%s", string);
    spacing = 21 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s| |`x\n\r", border);
    // Line 7 - Death, Modifier
    printf_to_char(ch, "%s| ", border);
    sprintf(string, "%s       Born%s: %s%s %d%s, %d", fields, border, data, month_names[target->pcdata->birth_month - 1], target->pcdata->birth_day, datesuffix(target->pcdata->birth_day), target->pcdata->birth_year);
    printf_to_char(ch, "%s", string);
    spacing = 43 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sModifier %s| ", fields, border);
    printf_to_char(ch, "%s", string);
    spacing = 11 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%s%s", data, modifier_table[target->modifier].name);
    printf_to_char(ch, "%s", string);
    spacing = 21 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s| |`x\n\r", border);
    // Line 8 - Border
    printf_to_char(ch, "%s| ", border);
    if (target->pcdata->doom_date > 1) {
      time_t doomtime;
      doomtime = target->pcdata->doom_date;

      struct tm *timeinfo;
      timeinfo = localtime(&doomtime);

      char month[20];
      strftime(month, sizeof(month), "%B", timeinfo);

      int day = timeinfo->tm_mday;
      int year = 1900 + timeinfo->tm_year;

      sprintf(string, "%s       Dies%s:`x %s %d%s, %d", fields, border, month, day, get_day_suffix(day), year);

      printf_to_char(ch, "%s", string);
      spacing = 52 - safe_strlen_color(string);
    }
    else {
      spacing = 52;
    }

    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%s'----------------------'", border);
    printf_to_char(ch, "%s", string);
    printf_to_char(ch, "%s |`x\n\r", border);

    // Line 8b - Secrecy
    if (is_vampire(target)) {
      printf_to_char(ch, "%s| ", border);
      sprintf(string, "%sSired%s:`x %s %d%2s, %d     `cSecrecy`g:`x %d", fields, border, month_names[target->pcdata->sire_month - 1], target->pcdata->sire_day, datesuffix(target->pcdata->sire_day), target->pcdata->sire_year, char_secrecy(target, NULL) / 10);
    }
    else
    {
      printf_to_char(ch, "%s| ", border);
      spacing = 57;
      printf_to_char(ch, "%*s", spacing, spacer);

      sprintf(string, "%sSecrecy%s: %s%d", fields, border, data, char_secrecy(target, NULL) / 10);
    }
    printf_to_char(ch, "%s", string);
    spacing = 20 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);

    // Line 9 - Spacing
    // printf_to_char(ch,"%s| ", border);
    // spacing=76;
    // printf_to_char(ch, "%*s", spacing, spacer);
    // printf_to_char(ch, "%s |`x\n\r", border);

    // Line 10 - Association, Territory
    printf_to_char(ch, "%s| ", border);
    sprintf(string, "%sAssociation%s: %s%s", fields, border, data, get_subfac_name(target->pcdata->association));
    printf_to_char(ch, "%s", string);
    spacing = 38 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    if (safe_strlen(ch->pcdata->home_territory) > 0) {
      sprintf(string, "%sTerritory%s: %s%-s", fields, border, data, target->pcdata->home_territory);
    }
    else {
      sprintf(string, "%sTerritory%s: %sNone", fields, border, data);
    }
    printf_to_char(ch, "%s", string);
    spacing = 38 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s |`x\n\r", border);

    // Line 11 - Faction, Faction Rank
    if (target->faction != 0 && clan_lookup(target->faction) != NULL) {
      printf_to_char(ch, "%s|     ", border);

      for (i = 0; i < 100 && str_cmp(target->name, fac->member_names[i]); i++) {}
      if (i < 100) {
        printf_to_char(ch, "%sSociety%s:`x %s", fields, border, get_fac(target));
        spacing = 30 - safe_strlen(get_fac(target));
        printf_to_char(ch, "%*s", spacing, spacer);
        printf_to_char(ch, "%sRank%s:`x %s", fields, border, fac->ranks[fac->member_position[i]]);
        spacing = 28 - safe_strlen(fac->ranks[fac->member_position[i]]);
        printf_to_char(ch, "%*s", spacing, spacer);
        printf_to_char(ch, "%s|`x\n\r", border);
      }
    }

    // Line 12 - Spacing
    printf_to_char(ch, "%s| ", border);
    spacing = 76;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s |`x\n\r", border);

    // Line 13 - Cash, Banked fund labels
    printf_to_char(ch, "%s| ", border);
    spacing = 13;
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sCash", fields);
    printf_to_char(ch, "%s", string);
    spacing = 25 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sBanked", fields);
    printf_to_char(ch, "%s", string);
    spacing = 28 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%sInfluence", fields);
    printf_to_char(ch, "%s", string);
    spacing = 10 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s |`x\n\r", border);
    // Line 14 - Cash, Banked fund values; influence
    printf_to_char(ch, "%s| ", border);
    spacing = 3;
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "`G$`x%.2f", value);
    printf_to_char(ch, "%sCurrency%s:`x %s", fields, border, string);
    spacing = 25 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "`G$`x%.2f", amount);
    printf_to_char(ch, "%s%s", data, string);
    spacing = 27 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%d", target->pcdata->influence);
    printf_to_char(ch, "%s%s`x %s", fields, border, string);
    spacing = 10 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s |`x\n\r", border);

    // Line 15 - Spacing
    printf_to_char(ch, "%s| ", border);
    spacing = 76;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s |`x\n\r", border);

    // Line 16 - Fatgiue, Lifeforce, and Defense
    printf_to_char(ch, "%s| ", border);
    spacing = 4;
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%d", UMAX(0, get_energy(target)));
    printf_to_char(ch, "%sFatigue%s:`x %s", fields, border, string);
    spacing = 13 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    if (get_display_lfmod(target) > 0) {
      printf(string, "%d%% (+%d)", get_display_lifeforce(target), get_display_lfmod(target));
    }
    else if (get_display_lfmod(target) < 0) {
      sprintf(string, "%d%% (%d)", get_display_lifeforce(target), get_display_lfmod(target));
    }
    else {
      sprintf(string, "%d%%", get_display_lifeforce(target));
    }
    printf_to_char(ch, "%sLife Force%s:`x %s", fields, border, string);
    spacing = 18 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    sprintf(string, "%d %s/`x %d", target->hit, border, max_hp(target));
    printf_to_char(ch, "%sDefenses%s:`x %s", fields, border, string);
    spacing = 10 - safe_strlen_color(string);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s |`x\n\r", border);
    // Line 17 - Wounds
    printf_to_char(ch, "%s|", border);
    spacing = 6;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sWounds%s:`x %-s", fields, border, get_inj(target));
    spacing = 64 - safe_strlen_color(get_inj(target));
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);
    // Line 18 - Willpower
    if (is_ghost(target)) {
      printf_to_char(ch, "%s|", border);
      spacing = 6;
      printf_to_char(ch, "%*s", spacing, spacer);
      sprintf(string, "%d", target->pcdata->ghost_pool);
      printf_to_char(ch, "%sWillpower%s:`x %-s", fields, border, string);
      spacing = 61 - safe_strlen_color(string);
      printf_to_char(ch, "%*s", spacing, spacer);
      printf_to_char(ch, "%s|`x\n\r", border);
    }
    // Line 19 - Experience chart labels
    printf_to_char(ch, "%s|", border);
    spacing = 6;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sSubdue %s[%s%s%s]", fields, border, data, (IS_FLAG(target->comm, COMM_SUBDUE)) ? "X" : " ", border);
    if (under_limited(target, target)) {
      printf_to_char(ch, "%s[%s%s%s] %sSanctuary", border, data, "/", border, fields);
    }
    else {
      printf_to_char(ch, "%s[%s%s%s] %sSanctuary", border, data, (under_understanding(target, target)) ? "X" : " ", border, fields);
    }
    spacing = 26;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sExperience", fields);
    spacing = 13;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|`x\n\r", border);
    // Line 20 - Subdue, Understanding statusl experience chart labels
    printf_to_char(ch, "%s|", border);
    spacing = 1;
    printf_to_char(ch, "%*s", spacing, spacer);
    // sprintf(string, "%d", total_disc(target));
    printf_to_char(ch, "%sUnconscious %s[%s%s%s]", fields, border, data, (ch->pcdata->sleeping > 0) ? "X" : " ", border);
    printf_to_char(ch, "%s[%s%s%s] %sHelpless", border, data, (is_helpless(ch)) ? "X" : " ", border, fields);
    spacing = 16;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sAvailable     %sSpent  %s/  %sCap       %s|`x\n\r", fields, fields, border, fields, border);
    // Line 21 - Unconscious, helpess status; experience chart border
    printf_to_char(ch, "%s|", border);
    spacing = 4;
    printf_to_char(ch, "%*s", spacing, spacer);
    // sprintf(string, "%d", total_skills(target));
    printf_to_char(ch, "%sWhoinvis %s[%s%s%s]", fields, border, data, (IS_FLAG(target->comm, COMM_WHOINVIS)) ? "X" : " ", border);
    printf_to_char(ch, "%s[%s%s%s] %sWhereinvis", border, data, (!IS_FLAG(target->comm, COMM_WHEREVIS) && !IS_FLAG(target->comm, COMM_WHEREANON))
    ? "X" : " ", border, fields);
    spacing = 13;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s,--------------------------------, |`x\n\r", border);
    // Line 22 - Bound, Blinfolded status; Combat experience
    printf_to_char(ch, "%s|", border);
    spacing = 7;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sBound %s[%s%s%s]", fields, border, data, (IS_FLAG(target->act, PLR_BOUND)) ? "X" : " ", border);
    printf_to_char(ch, "%s[%s%s%s] %sBlindfolded", border, data, (IS_FLAG(target->comm, COMM_BLINDFOLD)) ? "X" : " ", border, fields);
    spacing = 5;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sCombat %s|`x %8d %s| `x%8d %s/`x %-8d", fields, border, available_exp(target), border, target->spentexp + target->spentnewexp, border, target->pcdata->exp_cap);
    printf_to_char(ch, "%s | |`x\n\r", border);
    // Line 23 - Whoinvis, whereinvis status; roleplay experience
    printf_to_char(ch, "%s|", border);
    spacing = 6;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sGagged %s[%s%s%s]", fields, border, data, (IS_FLAG(target->comm, COMM_GAG)) ? "X" : " ", border);
    printf_to_char(ch, "%s[%s%s%s] %sMute", border, data, (is_mute(target)) ? "X" : " ", border, fields);
    spacing = 10;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%sRoleplay %s|`x %8d %s| `x%8d %s/`x %-8d", fields, border, available_rpexp(target), border, target->spentrpexp + target->spentnewrpexp, border, target->pcdata->rpexp_cap);
    printf_to_char(ch, "%s | |`x\n\r", border);
    // Line 24 - experience chart spacing
    printf_to_char(ch, "%s|", border);
    spacing = 1;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s,-------------------------------------,`x", border);
    spacing = 3;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s|--------------------------------| |`x\n\r", border);

    // Line 24b - Regular Karma, Personal, Dream experience and Wilds
    printf_to_char(ch, "%s| | ", border);
    printf_to_char(ch, "%sKarma%s: %s%6d     %sPersonal%s: %s%6d  %s|", fields, border, data, available_karma(target), fields, border, data, available_pkarma(target), border);
    printf_to_char(ch, "   %s|     %sDream%s: %s%8d", border, fields, border, data, target->pcdata->dexp);
    spacing = 11;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s | |`x\n\r", border);

    // Line 25 - Spent Karma, Banked, Adventure Experience
    printf_to_char(ch, "%s| | ", border);
    printf_to_char(ch, "%sSpent%s: %s%6d       %sBanked%s: %s%6d  %s|", fields, border, data, target->spentkarma + target->spentpkarma, fields, border, data, available_karmabank(target), border);
    printf_to_char(ch, "   %s|     %sWorld%s: %s%8d", border, fields, border, data, worlds_experience(ch, -1));
    spacing = 11;
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s | |`x\n\r", border);

    // Line 26 - karma / experience chart border
    printf_to_char(ch, "%s|", border);
    printf_to_char(ch, "%s '-------------------------------------'   '--------------------------------' |`x\n\r", border);

    // line 27 - Bottom border
    printf_to_char(ch, "%s|______________________________________________________________________________|`x\n\r", border);

    // CLEAR! - Disco 12/8/20 warding off MXP bleed QQ
    /*
    if(ch->desc != NULL && ch->desc->mxp == TRUE) {
    printf_to_char(ch, "`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q");
    }
    */
  }

  _DOFUN(do_oldscore) {

    Buffer outbuf;
    char buf[MSL];
    char tempbuf[MSL];

    char *const month_names[] = {
      "January", "Febuary", "March", 
      "April", "May", "June", "July",
      "August", "September", "October",
      "November", "December"
    };

    if (IS_NPC(ch))
    return;

    // The upper line of the score sheet.
    outbuf.strcat("`B####################################################################`x\n\r");
    sprintf(tempbuf, " `WName :`x %10s %-12s  `WAge:`x%3d      `WGender:`x %-6s", ch->name, ch->pcdata->last_name, get_real_age(ch), (ch->sex == SEX_MALE) ? "Male" : "Female");

    sprintf(buf, "`B#`x %-77.77s`B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    if (has_con(ch, SCON_POLICE) || ch->pcdata->police_timer > 0) {
      sprintf(tempbuf, " `RYou are wanted by the police`x");

      sprintf(buf, "`B#`x %-69.69s`B#`x\n\r", tempbuf);
      outbuf.strcat(buf);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
      outbuf.strcat(buf);
    }

    if (event_cleanse == 1)
    sprintf(tempbuf, " `WArchtype :`x `YHuman`x              `WFaction:`x %-10s", get_fac(ch));
    else
    sprintf(tempbuf, " `WArchtype :`x %s`x              `WFaction:`x %-10s", race_table[ch->race].who_name, get_fac(ch));

    sprintf(buf, "`B#`x %-79.79s`B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    //    if(safe_strlen(ch->pcdata->storyrunner) > 1)
    //    {
    //	    sprintf(tempbuf, "   `WSR Status :`x %s`x `WAwards:`x %d", //(gm_active(ch)) ? "Active" : "Inactive", ch->pcdata->awards);
    //            sprintf( buf, "`B#`x %-70.70s     `B#`x\n\r", tempbuf );
    //            outbuf.strcat( buf );
    //    }
    //    else
    //    {
    //            sprintf(tempbuf, " `WAwards:`x %d",  ch->pcdata->awards);
    //            sprintf( buf, "`B#`x%-70.70s`B#`x\n\r", tempbuf );
    //            outbuf.strcat( buf );
    //    }

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "           You were born on the %d%s of %s, %d.", ch->pcdata->birth_day, datesuffix(ch->pcdata->birth_day), month_names[ch->pcdata->birth_month - 1], ch->pcdata->birth_year);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    if (is_vampire(ch)) {
      sprintf(tempbuf, "             And died on the %d%s of %s, %d.", ch->pcdata->sire_day, datesuffix(ch->pcdata->sire_day), month_names[ch->pcdata->sire_month - 1], ch->pcdata->sire_year);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
      outbuf.strcat(buf);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", tempbuf);
      outbuf.strcat(buf);

      sprintf(tempbuf, " Regular Blood: %d, Virgin: %d, Faeborn: %d", ch->pcdata->heldblood[5], ch->pcdata->heldblood[0], ch->pcdata->heldblood[1]);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", tempbuf);
      outbuf.strcat(buf);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
      outbuf.strcat(buf);

      sprintf(tempbuf, " Angelborn: %d, Demonborn: %d, Demigod, %d", ch->pcdata->heldblood[2], ch->pcdata->heldblood[3], ch->pcdata->heldblood[4]);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", tempbuf);
      outbuf.strcat(buf);
    }
    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "`W   Wounds :`x %-27.27s`x `WFatigue   : `x%-3d", get_inj(ch), UMAX(0, get_energy(ch)));

    sprintf(buf, "`B#`x %-70.70s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "                                        `WInfluence : `x%-4d", UMAX(0, ch->pcdata->influence));

    sprintf(buf, "`B#`x %-64.64s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(
    tempbuf, "`W   Experience Cap :`x %7d`x             `WKarma     : `x%-5d`x       ", ch->pcdata->exp_cap, ch->karma);
    //    sprintf( buf, "`B#`x %-64.64s `B#`x\n\r", " " );
    //    outbuf.strcat( buf );

    sprintf(buf, "`B#`x %-68.68s         `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(
    tempbuf, "`W                             `WCommunity Credit     : `x%-5d`x    ", available_donated(ch));

    sprintf(buf, "`B#`x %-68.68s         `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WDisciplines`W:`x%3d                      `WStats:`x%3d", total_disc(ch), total_skills(ch));

    sprintf(buf, "`B#`x %-64s           `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    double value = ch->money;
    value /= 100;
    double amount = ch->pcdata->total_money;
    amount /= 100;

    sprintf(tempbuf, "   `WCash`W: `x$%-8.2f                      `WBanked: `x$%-9.2f", value, amount);

    sprintf(buf, "`B#`x %-64s       `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WExperience`W:`x       %7d            `WRP XP`W:`x       %7d", ch->exp, ch->rpexp);

    sprintf(buf, "`B#`x %-64s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WSpent Experience`W:`x %7d            `WSpent RP XP`W:`x %7d", ch->spentexp, ch->spentrpexp);

    sprintf(buf, "`B#`x %-64s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WDefense`W:`x %4d/%-4d                   `WHours`W:`x %d", ch->hit, max_hp(ch), (ch->played + (int)(current_time - ch->logon)) / 3600);

    sprintf(buf, "`B#`x %-68.68s         `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "`W                                        Whoinvis :`x %s`x ", (IS_FLAG(ch->comm, COMM_WHOINVIS)) ? "On" : "Off");

    sprintf(buf, "`B#`x %-58.58s           `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);
    sprintf(tempbuf, "`W                                        Subdue   :`x %s`x ", (IS_FLAG(ch->comm, COMM_SUBDUE)) ? "On" : "Off");

    sprintf(buf, "`B#`x %-58.58s             `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WLocation`W: `x%s", ch->in_room->area->name);
    sprintf(buf, "`B#`x %-64s       `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    outbuf.strcat("`B####################################################################`x\n\r");

    send_to_char(outbuf, ch);
    return;
  }

  char *const day_name[] = {"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};

  char *const month_names[] = {
    "January", "Febuary", "March",
    "April", "May", "June", "July",
    "August", "September", "October",
    "November", "December"
  };

  char *const moon_real[] = {
    "new", "waxing crescent", "first quarter",
    "waxing gibbous", "full", "waning gibbous",
    "last quarter", "waning crescent"
  };

  int moon_pointer(int strDay, int strMonth, int strYear, CHAR_DATA *ch) {
    int point;
    int Yr = strYear + 1900;
    int Mr = strMonth + 1;
    int Dr = strDay;

    if (get_hour(NULL) >= 17)
    Dr++;

    int YY = Yr - int((12 - Mr) / 10);
    int MM = Mr + 9;
    if (MM >= 12)
    MM = MM - 12;

    long K1 = int(365.25 * (YY + 4712));
    long K2 = int(30.6 * MM + .5);
    long K3 = int(int((YY / 100) + 49) * .75) - 38;

    long Jr = K1 + K2 + Dr + 59;
    if (Jr > 2299160)
    Jr = Jr - K3;

    float Vr = (Jr - 2451550.1) / 29.530588853;
    Vr = Vr - int(Vr);
    if (Vr < 0)
    Vr = Vr + 1;
    float IP = Vr;

    float AG = IP * 29.53;

    if (int(AG) == 5 && Dr == 1) {
      int TDr = 0;
      YY = Yr - int((12 - Mr) / 10);
      MM = Mr + 9;
      if (MM >= 12)
      MM = MM - 12;

      K1 = int(365.25 * (YY + 4712));
      K2 = int(30.6 * MM + .5);
      K3 = int(int((YY / 100) + 49) * .75) - 38;

      Jr = K1 + K2 + TDr + 59;
      if (Jr > 2299160)
      Jr = Jr - K3;

      Vr = (Jr - 2451550.1) / 29.530588853;
      Vr = Vr - int(Vr);
      if (Vr < 0)
      Vr = Vr + 1;
      IP = Vr;

      float tAG = IP * 29.53;
      if (int(tAG) == 4)
      AG = 4;
    }

    switch (int(AG)) {
    case 0:
    case 29:
      point = 0;
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      point = 1;
      break;
    case 7:
      point = 2;
      break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
      point = 3;
      break;
    case 14:
      point = 4;
      break;
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      point = 5;
      break;
    case 22:
      point = 6;
      break;
    default:
      point = 7;
      break;
    }
    return point;
  }

  _DOFUN(do_time) {
    //    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    //    char *suf;
    //    int day;
    char *mod_time;
    time_t com_time;
    tm *ptm;
    time_t east_time;
    char *local_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    com_time = east_time + (ch->pcdata->jetlag * 3600);
    mod_time = str_dup(ctime(&com_time));

    //    day     = time_info.day;

    /*
    if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";
    */
    sprintf(buf, "The time is %sYour time is %s", (char *)ctime(&east_time), mod_time);

    send_to_char(buf, ch);

    if (ch->in_room != NULL && !in_haven(ch->in_room) && ch->in_room->timezone != 0) {
      east_time = current_time;
      ptm = gmtime(&east_time);
      com_time = east_time + (ch->in_room->timezone * 3600);
      local_time = str_dup(ctime(&com_time));

      sprintf(buf, "Local time is %s", local_time);
      send_to_char(buf, ch);
    }

    printf_to_char(
    ch, "\nThere is a %s moon.\n\r", moon_real[moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL)]);

    if (IS_IMMORTAL(ch))
    printf_to_char(ch, "Code hour %d, %d\n\r", get_hour(ch->in_room), get_last_hour(ch->in_room));

    return;
  }

  _DOFUN(do_helpseealso) {
    HELP_DATA *help;
    char key[MAX_INPUT_LENGTH];
    char *keylist;
    bool found = FALSE;
    char argall[MSL], argone[MSL], argc[MSL];
    int level, count = 1, number = 1;
    char nohelp[MSL];

    Buffer exact, prefix;

    if (!str_cmp(argument, "")) {
      send_to_char(help_inferno, ch);
      return;
    }

    strcpy(nohelp, argument);

    /* Get the number of the help to find if there is one */
    if (argument[0] != '\0')
    number = number_argument(argument, argument);

    if (*argument == '\0')
    strcpy(argument, "summary");

    /* 'argc' is used to check again prefixes before checking argall */
    one_argument(argument, argc);

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
      argument = one_argument(argument, argone);
      if (argall[0] != '\0')
      strcat(argall, " ");
      strcat(argall, argone);
    }
    for (help = help_first; help; help = help->next) {
      level = (help->level < 0) ? -1 * help->level - 1 : help->level;

      if (level > get_trust(ch))
      continue;

      if (help->del == TRUE)
      continue;

      for (keylist = help->keyword;;) {
        keylist = one_argument(keylist, key);
        if (key[0] == '\0' || !str_prefix(argc, key))
        break;
      }

      if (key[0] == '\0')
      continue;

      if ((!str_cmp(argall, help->keyword) && number == 1) || (number > 1 && (count + 1) == number && !found)) {
        string_append(ch, &help->see_also);
      }
    }
  }

  _DOFUN(do_helptext) {
    HELP_DATA *help;
    char key[MAX_INPUT_LENGTH];
    char *keylist;
    bool found = FALSE;
    char argall[MSL], argone[MSL], argc[MSL];
    int level, count = 1, number = 1;
    char nohelp[MSL];

    Buffer exact, prefix;

    if (!str_cmp(argument, "")) {
      return;
    }

    strcpy(nohelp, argument);

    /* Get the number of the help to find if there is one */
    if (argument[0] != '\0')
    number = number_argument(argument, argument);

    if (*argument == '\0')
    strcpy(argument, "summary");

    /* 'argc' is used to check again prefixes before checking argall */
    one_argument(argument, argc);

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
      argument = one_argument(argument, argone);
      if (argall[0] != '\0')
      strcat(argall, " ");
      strcat(argall, argone);
    }
    for (help = help_first; help; help = help->next) {
      level = (help->level < 0) ? -1 * help->level - 1 : help->level;

      if (level > get_trust(ch))
      continue;

      if (help->del == TRUE)
      continue;

      for (keylist = help->keyword;;) {
        keylist = one_argument(keylist, key);
        if (key[0] == '\0' || !str_prefix(argc, key))
        break;
      }

      if (key[0] == '\0')
      continue;

      if ((!str_cmp(argall, help->keyword) && number == 1) || (number > 1 && (count + 1) == number && !found)) {

        if (help->text[0] == '.')
        string_append(ch, &help->text + 1);
        else
        string_append(ch, &help->text);
      }
    }
  }

  _DOFUN(do_helpkeyword) {
    HELP_DATA *help;
    bool found = FALSE;
    char argone[MSL];
    int level, count = 1, number = 1;

    Buffer exact, prefix;

    if (!str_cmp(argument, "")) {
      send_to_char(help_inferno, ch);
      return;
    }

    /* Get the number of the help to find if there is one */
    if (argument[0] != '\0')
    number = number_argument(argument, argument);

    argument = one_argument(argument, argone);

    if (safe_strlen(argument) < 2) {
      send_to_char("Rename it to what?\n\r", ch);
      return;
    }

    for (help = help_first; help; help = help->next) {
      level = (help->level < 0) ? -1 * help->level - 1 : help->level;

      if (level > get_trust(ch))
      continue;

      if (help->del == TRUE)
      continue;

      if ((!str_cmp(argone, help->keyword) && number == 1) || (number > 1 && (count + 1) == number && !found)) {
        free_string(help->keyword);
        help->keyword = str_dup(argument);
        printf_to_char(ch, "Help renamed to %s.", argument);
      }
    }
  }

  _DOFUN(do_helpnew) {
    HELP_DATA *pHelp;

    if (argument[0] == '\0') {
      send_to_char("Syntax: hedit create [keyword(s)]\n\r", ch);
      return;
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
      if (!str_cmp(argument, pHelp->keyword)) {
        if (!pHelp->del) {
          send_to_char("That help already exists.\n\r", ch);
          return;
        }
        else {
          pHelp->del = FALSE;
          free_string(pHelp->text);
          pHelp->text = str_dup("");
          free_string(pHelp->see_also);
          pHelp->see_also = str_dup("");
          send_to_char("New Help Created.\n\r", ch);
          string_append(ch, &pHelp->text);
          return;
        }
      }
    }

    pHelp = new_help();
    pHelp->keyword = str_dup(argument);
    pHelp->next = help_first;
    help_first = pHelp;

    send_to_char("New Help Entry Created.\n\r", ch);

    string_append(ch, &pHelp->text);
  }

  _DOFUN(do_helpdelete) {
    HELP_DATA *help;
    char key[MAX_INPUT_LENGTH];
    char *keylist;
    bool found = FALSE;
    char argall[MSL], argone[MSL], argc[MSL];
    int level, count = 1, number = 1;
    char nohelp[MSL];

    Buffer exact, prefix;

    if (!str_cmp(argument, "")) {
      send_to_char(help_inferno, ch);
      return;
    }

    strcpy(nohelp, argument);

    /* Get the number of the help to find if there is one */
    if (argument[0] != '\0')
    number = number_argument(argument, argument);

    if (*argument == '\0')
    strcpy(argument, "summary");

    /* 'argc' is used to check again prefixes before checking argall */
    one_argument(argument, argc);

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
      argument = one_argument(argument, argone);
      if (argall[0] != '\0')
      strcat(argall, " ");
      strcat(argall, argone);
    }
    for (help = help_first; help; help = help->next) {
      level = (help->level < 0) ? -1 * help->level - 1 : help->level;

      if (level > get_trust(ch))
      continue;

      if (help->del == TRUE)
      continue;

      for (keylist = help->keyword;;) {
        keylist = one_argument(keylist, key);
        if (key[0] == '\0' || !str_prefix(argc, key))
        break;
      }

      if (key[0] == '\0')
      continue;

      if ((!str_cmp(argall, help->keyword) && number == 1) || (number > 1 && (count + 1) == number && !found)) {
        help->del = TRUE;
        send_to_char("Help file deleted.\n\r", ch);
      }
    }
  }

  _DOFUN(do_helpsearch) {

    HELP_DATA *help;
    int level = 0;
    char buf[MSL];
    strcpy(buf, "");
    for (help = help_first; help; help = help->next) {
      level = (help->level < 0) ? -1 * help->level - 1 : help->level;

      if (level > get_trust(ch))
      continue;
      if (strcasestr(help->keyword, argument)) {
        strcat(buf, " ");
        strcat(buf, help->keyword);
        strcat(buf, ", ");
      }
      else if (strcasestr(help->text, argument)) {
        strcat(buf, " ");
        strcat(buf, help->keyword);
        strcat(buf, ", ");
      }
    }
    printf_to_char(ch, "Search results:\n%s\n\r", buf);
  }

  _DOFUN(do_help) {
    HELP_DATA *help;
    char key[MAX_INPUT_LENGTH];
    char *keylist;
    bool found = FALSE, fPrefix = FALSE;
    char argall[MSL], argone[MSL], argc[MSL], buf[MSL];
    int level, count = 1, number = 1;
    char nohelp[MSL];

    Buffer exact, prefix;

    if (!str_cmp(argument, "")) {
      if (ch->in_room->vnum == ROOM_INDEX_GENESIS) {
        send_to_char("Welcome to character generation.\nTo get help use `gnewbie (message)`x, or `gquestion (player) (message)`x What players are\nonline can be seen with the `gwho`x command.\n\nIf you're interested in making a supernaturally active character you should look\nat `gsociety list`x and `gsociety info (society name)`x societies are secrective\ngroups who work to manipulate the world both physically and psychically.\n\nIf you'd like to look at pre-generated characters you can play, try `ghelp roster`x,\n`groster list`x, `groster info (number)`x\n\nIf you're interested in playing in the high school, college or asylum sphere look at `ghelp institute`x\n\nIf you'd like to dive right into creating a custom character, type `gcreate`x.\n\r", ch);
      }
      else
      send_to_char(help_inferno, ch);
      return;
    }

    strcpy(nohelp, argument);

    /* Get the number of the help to find if there is one */
    if (argument[0] != '\0')
    number = number_argument(argument, argument);

    if (*argument == '\0')
    strcpy(argument, "summary");

    /* 'argc' is used to check again prefixes before checking argall */
    one_argument(argument, argc);

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
      argument = one_argument(argument, argone);
      if (argall[0] != '\0')
      strcat(argall, " ");
      strcat(argall, argone);
    }

    for (help = help_first; help; help = help->next) {
      level = (help->level < 0) ? -1 * help->level - 1 : help->level;

      if (level > get_trust(ch))
      continue;

      if (help->del == TRUE)
      continue;

      for (keylist = help->keyword;;) {
        keylist = one_argument(keylist, key);
        if (key[0] == '\0' || !str_prefix(argc, key))
        break;
      }

      if (key[0] == '\0')
      continue;

      if ((!str_cmp(argall, help->keyword) && number == 1) || (number > 1 && (count + 1) == number && !found)) {
        /* the keyword is exactly like argument */
        if (found)
        exact.strcat("`r==========`R==========`b==========`B==========`g==========`G==========`x\n\r\n\r");

        switch (help->type) {
        default:
        case 0:
          exact.strcat("`cTitle`g: `W");
          break;
        case 1:
          exact.strcat("`cImmortal`g: `W");
          break;
        case 2:
          exact.strcat("`cSkill`g: `W");
          break;
        case 3:
          exact.strcat("`cKeyword`g: `W");
          break;
        case 4:
          exact.strcat("`cClass`g: `W");
          break;
        case 5:
          exact.strcat("`cRace`g: `W");
          break;
        case 6:
          exact.strcat("`cWeave`g: `W");
          break;
        case 7:
          exact.strcat("`cForm`g: `W");
          break;
        }
        exact.strcat(help->keyword);
        exact.strcat("`x\n\r");
        exact.strcat("\n\r");

        if (help->text[0] == '.')
        exact.strcat(help->text + 1);
        else
        exact.strcat(help->text);
        found = TRUE;

        if (help->see_also != NULL)
        if (safe_strlen(help->see_also) > 1) {
          exact.strcat("\n\r`cSee Also`g: `W");
          exact.strcat(help->see_also);
          exact.strcat("`x");
        }

        exact.strcat("\n\r");

        count++;
      }
      else {
        /* prefix matches, but not whole argument */
        if (!fPrefix) {
          prefix.strcat("\n\r\n\r`g============================================================`x\n\rMore than one help matching that description was found.\n\r");
        }

        count++;
        sprintf(buf, "`c%2d`g: `W%s`x", count, help->keyword);
        prefix.strcat(buf);
        prefix.strcat("\n\r");
        fPrefix = TRUE;
      }
    }

    if (!found && count == 1) {
      send_to_char("No help on that word.  Did you mean one of these?", ch);

      for (help = help_first; help; help = help->next) {
        level = (help->level < 0) ? -1 * help->level - 1 : help->level;

        if (level > get_trust(ch))
        continue;

        if (levenshtein_distance(help->keyword, argall) < 5 && levenshtein_distance(help->keyword, argall) > 0) {
          printf_to_char(ch, ", %s", help->keyword);
        }
      }
      send_to_char("?\n\r", ch);
      append_file(ch, HELP_FILE, nohelp);
    }
    else {
      exact.strcat(prefix);
      if (count > 2) {
        sprintf(buf, "\n\r`c%d`x keywords matching your query were found.\n\rUse `csyntax`g: `Whelp `g(`Wpos #`g)`W.%s`x to select any help files from this list.\n\r", count - 1, argall);
        exact.strcat(buf);
      }
      page_to_char(exact, ch);
    }

    return;
  }

  char *const disc_ranks[] = {"Capable", "Combatant", "Contender", "Mighty", "Strong", "Warrior", "Powerful", "Powerful"};
  char *const stat_ranks[] = {"Proficient", "Skilled", "Accomplished", "Talented  ", "Gifted", "Brilliant", "Virtuoso"};

  char *const fac_ranks[] = {
    "Initiate", "Novice", "Soldier",
    "Knight", "Lieutenant", "Lt. Captain",
    "Captain", "Lord General", "LH General",
    "LS General"
  };

  char *standing(CHAR_DATA *ch, int number) {
    if (ch->played / 3600 < 50)
    return "Newcomer";
    if (number < 75)
    return "Common";
    else if (number < 125)
    return "Social";
    else if (number < 150)
    return "Well-liked";
    else if (number < 200)
    return "Influential";
    else if (number < 250)
    return "Prominent";
    else
    return "Renowned";
  }
  char *lower_standing(CHAR_DATA *ch, int number) {
    if (ch->played / 3600 < 50)
    return "newcomer";
    if (number < 75)
    return "common";
    else if (number < 125)
    return "social";
    else if (number < 150)
    return "well-liked";
    else if (number < 200)
    return "influential";
    else if (number < 250)
    return "prominent";
    else
    return "renowned";
  }

  int social_standing(CHAR_DATA *ch) {
    int base = ch->pcdata->base_standing;
    int behave = ch->pcdata->social_behave / 2;
    behave = UMAX(-20, behave);
    behave = UMIN(20, behave);
    base += behave;
    PROFILE_TYPE *prof = profile_lookup(ch->name);
    if(prof != NULL)
    {
      int pmod = prof->host_rating*prof->host_rating;
      pmod = pmod + 1000;
      base = base * pmod / 1500;
    }

    return base;
  }

  char *get_stat_rank(CHAR_DATA *ch) {
    int val = total_skills(ch);

    if (val < 25)
    return stat_ranks[0];
    else if (val < 35)
    return stat_ranks[1];
    else if (val < 45)
    return stat_ranks[2];
    else if (val < 60)
    return stat_ranks[3];
    else if (val < 75)
    return stat_ranks[4];
    else if (val < 100)
    return stat_ranks[5];

    return stat_ranks[6];
  }
  char *get_disc_rank(CHAR_DATA *ch) {
    int val = total_disc(ch);

    if (val < 80)
    return disc_ranks[0];
    else if (val < 100)
    return disc_ranks[1];
    else if (val < 120)
    return disc_ranks[2];
    else if (val < 140)
    return disc_ranks[3];
    else if (val < 160)
    return disc_ranks[4];
    else if (val < 180)
    return disc_ranks[5];
    else if (val < 200)
    return disc_ranks[6];

    return disc_ranks[7];
  }

  char *get_social_rank(CHAR_DATA *ch) {
    return standing(ch, social_standing(ch));
  }

  char *get_fac_rank(CHAR_DATA *ch) {
    int i;
    FACTION_TYPE *fac = clan_lookup(ch->faction);

    if (fac == NULL)
    return "";

    for (i = 0; i < 100 && str_cmp(ch->name, fac->member_names[i]); i++) {
    }
    return fac_ranks[fac->member_rank[i]];
  }

  char *get_whorank(CHAR_DATA *ch) {
    // Added check for CRank for immortals and rearranged the statement with else
    // if and else.  I also made it so if a character has less than the donation
    // required to use a custom rank, but have one, it'll always show. -
    // Discordance
    if (IS_IMMORTAL(ch) && safe_strlen(ch->pcdata->crank) > 1) {
      return ch->pcdata->crank;
    }
    else if (IS_IMMORTAL(ch)) {
      return "Immortal";
    }
    else if (is_gm(ch)) {
      return "Story Runner";
    }
    else if (safe_strlen(ch->pcdata->crank) > 1 && available_donated(ch) >= 2000 && ch->pcdata->rank_type == 4) {
      return ch->pcdata->crank;
    }
    else if (higher_power(ch)) {
      return "Eidolon";
    }
    else {
      if (ch->pcdata->rank_type == 0)
      return get_social_rank(ch);
      else if (ch->pcdata->rank_type == 1)
      return get_stat_rank(ch);
      else if (ch->pcdata->rank_type == 2)
      return get_disc_rank(ch);
      else if (ch->pcdata->rank_type == 3)
      return get_fac_rank(ch);
      else if (ch->pcdata->rank_type == 5) {
        if (ch->race == RACE_FACULTY)
        return str_dup("`WFaculty`x");
        else
        {
          if(clinic_patient(ch))
          return str_dup("`225Commited`x");
          int croll = college_group(ch, FALSE);
          if(croll == COLLEGE_GOTH)
          return str_dup("`052Goth`x");
          if(croll == COLLEGE_JOCK)
          return str_dup("`009Jock`x");
          if(croll == COLLEGE_OVERACHIEVER)
          return str_dup("`051Nerd`x");
          if(croll == COLLEGE_PREP)
          return str_dup("`226Prep`x");
          if(croll == COLLEGE_BADKID)
          return str_dup("`242Delinquent`x");
          if(croll == COLLEGE_THEATRE)
          return str_dup("`218TheatreKid`x");
        }
      }
      else
      return get_social_rank(ch);
    }
    return "";
  }

  _DOFUN(do_rank) {

    if (!str_cmp(argument, "social")) {
      ch->pcdata->rank_type = 0;
      send_to_char("`cYou now display your social rank`g.\n\r", ch);
      return;
    }
    else if (!str_cmp(argument, "stats")) {
      ch->pcdata->rank_type = 1;
      send_to_char("`cYou now display your stats rank`g.\n\r", ch);
      return;
    }
    else if (!str_cmp(argument, "disciplines") && ch->race != RACE_CIVILIAN) {
      ch->pcdata->rank_type = 2;
      send_to_char("`cYou now display your disciplines rank`g.\n\r", ch);
      return;
    }
    else if (!str_cmp(argument, "Society") && ch->faction != 0) {
      ch->pcdata->rank_type = 3;
      send_to_char("`cYou now display your society rank`g.\n\r", ch);
      return;
    }
    else if (!str_cmp(argument, "institute") && (college_student(ch, FALSE) || ch->race == RACE_FACULTY)) {
      ch->pcdata->rank_type = 5;
      send_to_char("`cYou now display your institute rank`g.\n\r", ch);
      return;
    }
    else if (!str_cmp(argument, "custom") && available_donated(ch) >= 2000) {
      // Added check to see if a custom rank was set first - Discordance
      if (safe_strlen(ch->pcdata->crank) < 1) {
        send_to_char("Set a custom rank with `c syntax`g: Wcredits rank `g(`Wdesired rank`g)`x first.\n\r", ch);
        return;
      }
      ch->pcdata->rank_type = 4;
      send_to_char("You now display your custom rank.\n\r", ch);
      return;
    }
    printf_to_char(ch, "`cSocial     `g:`x %s\n\r", get_social_rank(ch));
    printf_to_char(ch, "`cStats      `g:`x %s\n\r", get_stat_rank(ch));
    if (ch->race != RACE_CIVILIAN)
    printf_to_char(ch, "`cDisciplines`g:`x %s\n\r", get_disc_rank(ch));
    if (ch->faction != 0)
    printf_to_char(ch, "`cSociety    `g:`x %s\n\r", get_fac_rank(ch));

    send_to_char("\n`cSyntax`g: `WRank `g(`WSocial`g/`WStats`g/`WDisciplines`g/`WSociety`g/`WInstitute`g)`x\n\r", ch);
  }

  // For scaling ascii borders for UI elements - Discordance
  char *draw_horizontal_line(int span) {
    char buf[MSL];
    int i;

    if (span > 320) {
      return str_dup("Error");
    }

    strcpy(buf, "");

    for (i = 0; i < span; i++) {
      sprintf(buf, "%s_", buf);
    }

    return str_dup(buf);
  }

  bool immaccount(CHAR_DATA *ch) {
    if (ch->pcdata->account == NULL)
    return FALSE;

    return immaccount_name(ch->pcdata->account_name);
  }

  bool immaccount_name(char *argument) {

    return FALSE;
  }

  bool multiplayer_abom(CHAR_DATA *ch) {
    if (immaccount(ch))
    return FALSE;
    int count = 0;
    int abomcount = 0;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character != NULL) {
        if (IS_FLAG(d->character->act, PLR_SPYSHIELD))
        continue;
        if (IS_FLAG(d->character->act, PLR_GUEST))
        continue;

        if (immaccount(d->character))
        continue;

        if (ch->pcdata->account != NULL && d->character->pcdata->account != NULL) {
          if (!str_cmp(ch->pcdata->account->name, d->character->pcdata->account->name) && !is_gm(d->character)) {
            count++;
            if (is_abom(d->character))
            abomcount++;
          }
        }
        else if (!str_cmp(ch->desc->host, d->host)) {
          if (!is_gm(d->character)) {
            count++;
            if (is_abom(d->character))
            abomcount++;
          }
        }
      }
    }
    if (abomcount > 0 && count > 1)
    return TRUE;

    return FALSE;
  }

  bool multiplayer_super(CHAR_DATA *ch) {
    if (immaccount(ch))
    return FALSE;
    int count = 0;
    int supercount = 0;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character) {
        if (IS_FLAG(d->character->act, PLR_SPYSHIELD))
        continue;
        if (IS_FLAG(d->character->act, PLR_GUEST))
        continue;
        if (higher_power(d->character))
        continue;
        if (immaccount(d->character))
        continue;

        if (ch->pcdata->account != NULL && d->character->pcdata->account != NULL) {
          if (!str_cmp(ch->pcdata->account->name, d->character->pcdata->account->name) && !is_gm(d->character)) {
            count++;
            if (is_super(d->character) && d->character->race != RACE_FACULTY)
            supercount++;
          }
        }
        else if (!str_cmp(ch->desc->host, d->host)) {
          if (!is_gm(d->character)) {
            count++;
            if (is_super(d->character) && d->character->race != RACE_FACULTY)
            supercount++;
          }
        }
      }
    }
    if (supercount > 1)
    return TRUE;

    return FALSE;
  }

  bool multiplayer_three(CHAR_DATA *ch) {
    if (immaccount(ch))
    return FALSE;
    int count = 0;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character) {
        if (IS_FLAG(d->character->act, PLR_SPYSHIELD))
        continue;
        if (IS_FLAG(d->character->act, PLR_GUEST))
        continue;
        if (higher_power(d->character))
        continue;
        if (immaccount(d->character))
        continue;

        if (ch->pcdata->account != NULL && d->character->pcdata->account != NULL) {
          if (!str_cmp(ch->pcdata->account->name, d->character->pcdata->account->name) && !is_gm(d->character))
          count++;
        }
        else {
          if (!str_cmp(ch->desc->host, d->host)) {
            if (!is_gm(d->character)) {
              count++;
            }
          }
        }
      }
    }
    if (count > 2)
    return TRUE;

    return FALSE;
  }

  bool multiplayer_account(CHAR_DATA *ch) {
    if (immaccount(ch))
    return FALSE;

    if (staff_char(ch))
    return FALSE;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character) {
        if (IS_FLAG(d->character->act, PLR_SPYSHIELD))
        continue;
        if (higher_power(d->character))
        continue;
        if (immaccount(d->character))
        continue;

        if (staff_char(d->character))
        continue;
        if (!str_cmp(ch->desc->host, d->host)) {
          if (ch->pcdata->account != NULL && d->character->pcdata->account != NULL) {
            if (str_cmp(ch->pcdata->account->name, d->character->pcdata->account->name))
            return TRUE;
          }
        }
      }
    }
    return FALSE;
  }

  int cheat_number(CHAR_DATA *ch, int number) {
    int num = 1;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character) {
        if (IS_FLAG(d->character->act, PLR_SPYSHIELD))
        continue;

        if (ch->desc->host != NULL && d->host != NULL) {
          if (!str_cmp(ch->desc->host, d->host)) {
            return num;
          }
        }
        num++;
      }
    }
    return num;
  }


  bool where_banned(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    // checking where ban list
    for (int i = 0; i < 50; i++) {
      for (int n = 0; n < 25; n++) {
        if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
          if (!str_cmp(ch->pcdata->account->characters[n], victim->pcdata->nowhere_characters[i])) {
            return TRUE;
          }
        }
      }
    }

    for (int i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->nowhere_accounts[i])) {
        return TRUE;
      }
    }

    return FALSE;
  }

  _DOFUN(do_who) {
    Buffer immbuf(2048), mortbuf(2048), gmbuf(2048);
    CHAR_DATA *target;
    char buf[MSL];
    char buf4[MSL];
    char buf9[MSL];
    char tbuf[MSL];
    char invis1[MSL];
    char invis2[MSL];
    int nMatch = 0;
    int vMatch = 0;
    int i;
    char spacer[] = "";
    int spacing = 0, limit = 0;
    char abridged_title[MSL];
    char rank[MSL];
    char *frame;

    buf[0] = '\0';
    int cheatnumber = 1;
    if (!str_cmp(argument, "recent")) {
      return;
      send_to_char("\n\r`W      \t\t\t\t    Recent Who `x\n\r\n\r", ch);
      for (i = 0; i < 10; i++) {
        send_to_char(last_on_names[i], ch);
      }
      for (i = 0; i < 25; i++) {
        send_to_char(last_on_times[i], ch);
      }
      return;
    }

    immbuf.printf("\n\r`WWho List`x\n\r");
    // immbuf.printf("`w                   __     __ `x\n\r"); immbuf.printf("`w
    // / /    / /  _____,-,__   __  ______   ,______ `x\n\r"); immbuf.printf("`w /
    // /----/ / / ,---) / | | / / / /___)) / ;---) )`x\n\r"); immbuf.printf("`w /
    // ,----, / / /   / /  | |/ / / /-----'/ /   / / `x\n\r"); immbuf.printf("`w
    // /_/    /_/  '=====.{    \\__/ (_(____  /_/   /_/  `x\n\r");

    // immbuf.printf("`w                    .  .                     `x\n\r");
    // immbuf.printf("`w                    |__| ,-. .   , ,-.  ,--. `x\n\r");
    // immbuf.printf("`w                    |  | ,-|  \\ /  |--' |  | `x\n\r");
    // immbuf.printf("`w                    '  ' '-^   '   '--  '  ' `x\n\r");
    // immbuf.printf("`g--------------------------------------------------------------------------------`x");

    sprintf(buf, "\n\r `c");
    // strcat (buf, MXP_TAG ("color fore=coral"));
    strcat(buf, "Immortals`g:`x\n\r");
    // strcat (buf, MXP_TAG ("/color"));

    strcat(buf, "`g,");
    frame = draw_horizontal_line(ch->linewidth - 2);
    strcat(buf, frame);
    strcat(buf, ",`x\n\r");
    // strcat( buf, // "`g,______________________________________________________________________________,`x\n\r");
    immbuf.strcat(buf);

    sprintf(buf, "\n\r `cEntities`g:`x\n\r");
    strcat(buf, "`g,");
    strcat(buf, frame);
    strcat(buf, ",`x\n\r");
    // strcat( buf, // "`g,______________________________________________________________________________,`x\n\r");
    gmbuf.strcat(buf);

    sprintf(buf, "\n\r `cPlayers`g:`x\n\r");
    strcat(buf, "`g,");
    strcat(buf, frame);
    strcat(buf, ",`x\n\r");
    // strcat( buf, // "`g,______________________________________________________________________________,`x\n\r");
    mortbuf.strcat(buf);

    buf[0] = '\0';
    if (!strcmp(argument, "all") && IS_IMMORTAL(ch)) {
      DIR *od;
      struct dirent *dir;
      DESCRIPTOR_DATA d;
      d.original = NULL;
      od = opendir(PLAYER_DIR);
      if (od) {
        while ((dir = readdir(od)) != NULL) {
          if (!strcasestr(dir->d_name, ".") && strcmp(dir->d_name, "GroundObjects")) {
            if (!load_char_obj(&d, dir->d_name)) {
              continue;
            }
            target = d.character;

            printf_to_char(ch, "%s\n", target->name);
            free_char(target);
          }
        }
        closedir(od);
      }
      return;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;

      if ((d->connected != CON_PLAYING && d->connected != CON_CREATION))
      continue;

      target = CH(d);
      nMatch++;

      if ((d->connected != CON_PLAYING && d->connected != CON_CREATION) || (IS_IMMORTAL(target) && target->invis_level > ch->level && (IS_FLAG(target->act, PLR_WHOINVIS) && !higher_power(target)))) {
        continue;
      }

      if(where_banned(ch, target))
      continue;

      if (!IS_IMMORTAL(ch) && IS_FLAG(target->comm, COMM_WHOINVIS) & !higher_power(target))
      continue;

      if (!same_player(ch, target) && spammer(target) && !IS_IMMORTAL(ch))
      continue;

      if (IS_FLAG(target->comm, COMM_WHOINVIS) && !story_on(target))
      continue;

      if (!IS_IMMORTAL(ch) && IS_FLAG(target->act, PLR_BOUNDFEET))
      continue;

      if (!IS_IMMORTAL(ch) && IS_FLAG(target->act, PLR_BOUND))
      continue;

      if (IS_FLAG(target->act, PLR_SPYSHIELD))
      continue;

      vMatch++;

      if (target->invis_level > 0 && IS_FLAG(target->act, PLR_WHOINVIS)) {
        strcpy(invis1, "`C(`w");
        strcpy(invis2, "`C)`x ");
        sprintf(buf4, "%sW%d%s", invis1, target->invis_level, invis2);
      }
      else if (IS_FLAG(target->comm, COMM_WHOINVIS) && IS_IMMORTAL(ch)) {
        strcpy(invis1, "`C(`w");
        strcpy(invis2, "`C)`x ");
        sprintf(buf4, "%sW%d%s", invis1, target->invis_level, invis2);
      }
      else
      strcpy(buf4, "");

      if (IS_IMMORTAL(ch)) {
        char buf44[MSL];
        if (multiplayer_abom(target)) {
          sprintf(buf44, "(`DAbom-%d`x)%s", cheat_number(target, cheatnumber), buf4);
          sprintf(buf4, "%s", buf44);
        }
        if (multiplayer_super(target)) {
          sprintf(buf44, "(`rSuper-%d`x)%s", cheat_number(target, cheatnumber), buf4);
          sprintf(buf4, "%s", buf44);
        }
        if (multiplayer_three(target)) {
          sprintf(buf44, "(`yThree-%d`x)%s", cheat_number(target, cheatnumber), buf4);
          sprintf(buf4, "%s", buf44);
        }
        if (multiplayer_account(target) && strcmp(target->pcdata->account->name, ch->pcdata->account->name)) {
          sprintf(buf44, "(`RAccount-%d`x)%s", cheat_number(target, cheatnumber), buf4);
          sprintf(buf4, "%s", buf44);
        }
        if (target->pcdata->account != NULL && IS_FLAG(target->pcdata->account->flags, ACCOUNT_SPAMMER)) {
          sprintf(buf44, "(`rSpammer`x)%s", buf4);
          sprintf(buf4, "%s", buf44);
        }
        if (target->pcdata->account != NULL && IS_FLAG(target->pcdata->account->flags, ACCOUNT_SHH)) {
          sprintf(buf44, "(`rShh`x)%s", buf4);
          sprintf(buf4, "%s", buf44);
        }
        if (target->pcdata->account != NULL && IS_FLAG(target->pcdata->account->flags, ACCOUNT_SHADOWBAN)) {
          sprintf(buf44, "(`DShadowban`x)%s", buf4);
          sprintf(buf4, "%s", buf44);
        }

        if (is_griefer(target)) {
          sprintf(buf44, "(`rGriefer`x)%s", buf4);
          sprintf(buf4, "%s", buf44);
        }
      }
      cheatnumber++;
      // Centering ranks
      if (safe_strlen_color(get_whorank(target)) & 1) {
        spacing = (int)floor((12 - safe_strlen_color(get_whorank(target))) / 2);
        sprintf(rank, "`x%*s", spacing, spacer);
        sprintf(tbuf, "%s`x", get_whorank(target));
        strcat(rank, tbuf);
        spacing = (int)floor((12 - safe_strlen_color(get_whorank(target))) / 2) + 1;
        sprintf(tbuf, "%*s", spacing, spacer);
        strcat(rank, tbuf);
      }
      else {
        spacing = (int)floor((12 - safe_strlen_color(get_whorank(target))) / 2);
        sprintf(rank, "`x%*s", spacing, spacer);
        sprintf(tbuf, "%s`x", get_whorank(target));
        strcat(rank, tbuf);
        spacing = (int)floor((12 - safe_strlen_color(get_whorank(target))) / 2);
        sprintf(tbuf, "%*s`x", spacing, spacer);
        strcat(rank, tbuf);
      }

      // determining border edge after title
      limit = ch->linewidth - 4 - safe_strlen_color(NAME(target)) - safe_strlen_color(rank);
      if (IS_FLAG(target->comm, COMM_AFK))
      limit = limit - 6;
      if (IS_FLAG(target->comm, COMM_QUIET))
      limit = limit - 8;
      if (target->level == 0)
      limit = limit - 11;

      // this lists what players have SRs trusted to on the who list
      if ((is_gm(ch) || higher_power(ch)) && !is_gm(target)) {
        if (get_gmtrust(ch, target) >= 3 && pact_holder(target))
        sprintf(buf9, "[`rPact`x] ");
        else if (IS_IMMORTAL(ch) && get_gmtrust(ch, target) != 3)
        sprintf(buf9, "[`cT%d`x] ", get_gmtrust(ch, target));
        else if (!IS_IMMORTAL(ch) && get_gmtrust(ch, target) != 0)
        sprintf(buf9, "[`cT%d`x] ", get_gmtrust(ch, target));
        else
        strcpy(buf9, "");
      }
      // this lists trust for SRs on the who list
      else if (!is_gm(ch) && (is_gm(target) || higher_power(target)) && !IS_IMMORTAL(target)) {
        if (get_gmtrust(target, ch) != 0)
        sprintf(buf9, "[`cT%d`x] ", get_gmtrust(target, ch));
        else
        strcpy(buf9, "");
      }
      else
      strcpy(buf9, "");

      limit = limit - safe_strlen_color(buf9); // Accounts for trust tag
      limit = limit - safe_strlen_color(buf4); // Accounts for whoinvis tag

      if (same_player(ch, target) || (!spammer(target) && target->pcdata->account != NULL && IS_FLAG(target->pcdata->account->flags, ACCOUNT_SHH) == FALSE))
      strcpy(abridged_title, target->pcdata->whotitle);
      else
      strcpy(abridged_title, "");

      if (safe_strlen_color(abridged_title) > limit) {
        strncpy(abridged_title, abridged_title, limit);
        // abridged_title[limit+1]='\0';
        abridged_title[limit] = '\0';
      }

      sprintf(buf, "`g|`x%s %s%s%s%s%s%s%s", rank, buf4, buf9, IS_FLAG(target->comm, COMM_AFK) ? "[`RAFK`x] " : "", IS_FLAG(target->comm, COMM_QUIET) ? "[`mQUIET`x] " : "", target->level == 0 ? "[`YCreating`x] " : "", NAME(target), abridged_title);

      if (limit > 0) {
        spacing = limit - safe_strlen_color(abridged_title);
      }
      else {
        spacing = 0;
      }

      sprintf(tbuf, "%*s ", spacing, spacer);
      strcat(buf, tbuf);

      // CLEAR! - Disco 12/8/20 warding off MXP bleed
      if (ch->desc != NULL && ch->desc->mxp == TRUE) {
        strcat(buf, "`q");
      }

      sprintf(tbuf, "`g|`x\n\r");
      strcat(buf, tbuf);

      // sprintf(tbuf, "WLen: %d CLen: %d Spacing: %d Limit: %d  Offset: %d\n\r", // safe_strlen_color(target->pcdata->whotitle), safe_strlen_color(abridged_title), // spacing, limit, offset); strcat(buf, tbuf);

      if (IS_IMMORTAL(target) && target->level >= LEVEL_IMMORTAL) {
        immbuf.strcat(buf);
      }
      else if (IS_FLAG(target->act, PLR_GM) || higher_power(target)) {
        gmbuf.strcat(buf);
      }
      else {
        mortbuf.strcat(buf);
      }
    }
    strcpy(buf, "");
    strcat(buf, "`g|");
    strcat(buf, frame);
    strcat(buf, "|`x\n\r");
    // sprintf( buf, // "`g|______________________________________________________________________________|\n\r");
    immbuf.strcat(buf);
    gmbuf.strcat(buf);
    mortbuf.strcat(buf);
    page_to_char(immbuf, ch);
    page_to_char(gmbuf, ch);
    sprintf(buf, "\n\r `cCharacters online`g:`x %d", nMatch);
    spacing = ch->linewidth - safe_strlen_color(buf);
    mortbuf.strcat(buf);
    sprintf(tbuf, "`cCharacters visible`g:`x %d\n\r", vMatch);
    spacing = spacing - safe_strlen_color(tbuf);
    sprintf(buf, "%*s", spacing, spacer);
    strcat(buf, tbuf);
    mortbuf.strcat(buf);
    // sprintf(buf,      "`cCharacters visible`g:`x %d\n\r", vMatch);
    // mortbuf.strcat(buf);
    /*
    frame=str_dup("");
    free_string(frame);
    */
    page_to_char(mortbuf, ch);
  }

  _DOFUN(do_inventory) {
    send_to_char("`cYou are carrying`g:`x\n\r", ch);
    show_list_to_char(ch->carrying, ch, FALSE, TRUE, NULL);
    return;
  }

  _DOFUN(do_equipment) {
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char("`cYou are using`g:`x\n\r", ch);
    found = FALSE;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL) {
      send_to_char(where_name[WEAR_HOLD], ch);
      send_to_char(format_obj_to_char(obj, ch, FALSE), ch);
      send_to_char("\n\r", ch);
    }
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL) {
      send_to_char(where_name[WEAR_HOLD_2], ch);
      send_to_char(format_obj_to_char(obj, ch, FALSE), ch);
      send_to_char("\n\r", ch);
    }

    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL) {
        continue;
      }
      if (iWear == WEAR_HOLD) {
        continue;
      }
      if (iWear == WEAR_HOLD_2) {
        continue;
      }
      send_to_char(where_name[iWear], ch);

      send_to_char(format_obj_to_char(obj, ch, FALSE), ch);
      send_to_char("\n\r", ch);
      found = TRUE;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && obj->size >= 25 && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {

        if (obj->item_type == ITEM_WEAPON) {
          send_to_char("(sheathed)", ch);
        }
        else {
          send_to_char("(carried)", ch);
        }

        send_to_char(format_obj_to_char(obj, ch, FALSE), ch);
        send_to_char("\n\r", ch);
      }
    }
    if (!found) {
      send_to_char("Nothing.\n\r", ch);
    }

    return;
  }

  // Whotitle Function by Kuval.
  void set_whotitle(CHAR_DATA *ch, char *whotitle) {
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
      bug("Set_whotitle: NPC.", 0);
      return;
    }

    if (whotitle[0] != '.' && whotitle[0] != ',' && whotitle[0] != '!' && whotitle[0] != '?' && whotitle[0] != ';' && whotitle[0] != '\'') {
      buf[0] = ' ';
      strcpy(buf + 1, whotitle);
    }
    else {
      strcpy(buf, whotitle);
    }

    free_string(ch->pcdata->whotitle);
    ch->pcdata->whotitle = str_dup(buf);
    return;
  }

  void set_title(CHAR_DATA *ch, char *title) {
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
      bug("Set_title: NPC.", 0);
      return;
    }

    if (title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' && title[0] != ';' && title[0] != '\'') {
      buf[0] = ' ';
      strcpy(buf + 1, title);
    }
    else {
      strcpy(buf, title);
    }

    if (is_dreaming(ch)) {
      free_string(ch->pcdata->dreamtitle);
      ch->pcdata->dreamtitle = str_dup(buf);
    }
    else {
      free_string(ch->pcdata->title);
      ch->pcdata->title = str_dup(buf);
    }
    return;
  }

  // Whotitle, by Kuval.
  _DOFUN(do_whotitle) {
    if (IS_NPC(ch))
    return;

    if (ch->pcdata->ci_editing == 8) {
      ch->pcdata->ci_absorb = 1;
      free_string(ch->pcdata->ci_name);
      ch->pcdata->ci_name = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Change your whotitle to what?\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTITLE)) {
      send_to_char("You can't set your title.\n\r", ch);
      return;
    }

    if (safe_strlen_color(argument) > 100)
    argument[100] = '\0';

    smash_tilde(argument);
    set_whotitle(ch, argument);
    send_to_char("Whotitle has been changed.\n\r", ch);
  }

  _DOFUN(do_title) {
    if (IS_NPC(ch))
    return;

    smash_vector(argument);

    if (argument[0] == '\0') {
      send_to_char("Title cleared\n\r", ch);
    }
    // hack to get around command priority for room list vs roomtitle
    if (!str_cmp(argument, "list")) {
      do_function(ch, &do_roomlist, "");
      return;
    }
    if (safe_strlen_color(argument) + safe_strlen_color(ch->name) > 100)
    argument[100] = '\0';

    smash_tilde(argument);
    set_title(ch, argument);
    send_to_char("Ok.\n\r", ch);
  }

  _DOFUN(do_maskintro) { do_function(ch, do_mask, argument); }

  _DOFUN(do_intro) {
    if (IS_NPC(ch))
    return;

    if (ch->pcdata->ci_editing == 19) {
      ch->pcdata->ci_absorb = 1;
      string_append(ch, &ch->pcdata->ci_desc);
      send_to_char("Write the message that will display at the start of the patrol event.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || ch->in_room->vnum != ROOM_INDEX_GENESIS) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOINTRO)) {
        send_to_char("You can't set an intro.\n\r", ch);
        return;
      }
    }
    if (argument[0] == '\0') {
      send_to_char("Change your intro to what?\n\r", ch);
      return;
    }
    if(ch->pcdata->summary_stage == SUM_STAGE_INTRO)
    {
      CHAR_DATA *target = ch->pcdata->summary_target;
      if(target == NULL)
      {
        return;
      }
      if(safe_strlen(argument) < 5 || safe_strlen(argument) > 200)
      {
        send_to_char("Your intro must be between 5 and 200 characters.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->summary_intro);
      ch->pcdata->summary_intro = str_dup(argument);
      if(ch->pcdata->summary_type == SUMMARY_CONFLICT_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATING_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATED_ATT)
      {
        process_emote(ch, NULL, EMOTE_EMOTE, argument);
        ch->pcdata->summary_stage = SUM_STAGE_CONTENT;
        target->pcdata->summary_stage = SUM_STAGE_CONTENT;
        send_to_char("You can now write the content of your conflict, use content (emote text) to do so.\n\r", ch);
        send_to_char("You can now write the content of your conflict, use content (emote text) to do so.\n\r", target);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_OTHER)
      {
        if(strlen(target->pcdata->summary_intro) > 2)
        {
          process_emote(ch, NULL, EMOTE_EMOTE, argument);
          ch->pcdata->summary_stage = SUM_STAGE_CONTENT;
          target->pcdata->summary_stage = SUM_STAGE_CONTENT;
          send_to_char("You can now write the content of your scene, use content (emote text) to do so.\n\r", ch);
          send_to_char("You can now write the content of your scene, use content (emote text) to do so.\n\r", target);
          return;
        }
        else
        {
          send_to_char("Intro Set.\n\r", ch);
          return;
        }
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIMIZER)
      {
        process_emote(ch, NULL, EMOTE_EMOTE, argument);
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        target->pcdata->summary_stage = SUM_STAGE_INTRO;
        send_to_char("You can now write you own introduction pose in response, use intro (emote text) to do so.\n\r", target);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIM)
      {
        process_emote(ch, NULL, EMOTE_EMOTE, argument);
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        target->pcdata->summary_stage = SUM_STAGE_CONTENT;
        send_to_char("You can now write the content of the scene, use content (emote text) to do so. You can also set if you would like to take blood, use blood (yes/no) to do so.\n\r", target);
        return;
      }

    }

    if (safe_strlen_color(argument) < 5) {
      send_to_char("Change your intro to what?\n\r", ch);
      return;
    }

    if (strcasestr(argument, "teen")) {
      send_to_char("It's redundant to include age in your intro.\n\r", ch);
      return;
    }

    if (safe_strlen_color(argument) > 50)
    argument[50] = '\0';

    int len = safe_strlen(argument);
    if (ispunct(argument[len]))
    argument[len] = '\0';

    smash_tilde(argument);

    if (ch->shape == SHAPE_MERMAID) {
      free_string(ch->pcdata->mermaidintro);
      ch->pcdata->mermaidintro = str_dup(from_color(argument));
    }
    else {
      free_string(ch->pcdata->intro_desc);
      ch->pcdata->intro_desc = str_dup(from_color(argument));
    }

    send_to_char("Ok.\n\r", ch);
  }

  _DOFUN(do_remember) {
    char arg1[MIL], name[MIL];
    CHAR_DATA *victim = NULL;
    char buf[MSL];

    argument = one_argument(argument, arg1);
    one_argument(argument, name);

    if (arg1[0] == '\0' || name[0] == '\0') {
      send_to_char("Syntax: remember (target) (name)\n\r", ch);
      return;
    }
    if (check_for_color(name)) {
      send_to_char("You cannot have color in the persons name!\n\r", ch);
      return;
    }

    if (safe_strlen(name) > 20) {
      name[20] = '\0';
    }

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("You don't see them here.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      return;
    }
    if (is_cloaked(victim)) {
      send_to_char("Hmm, what, what were you doing?\n\r", ch);
      return;
    }
    if (victim == ch) {
      send_to_char("You already know your own name...hopefully.\n\r", ch);
      return;
    }

    sprintf(buf, "You will now remember %s", PERS(victim, ch));
    add_intro(ch, victim->id, victim->sex, capitalize(name));
    printf_to_char(ch, "%s as %s.\n\r", buf, PERS(victim, ch));
  }

  // History command
  _DOFUN(do_history) {

    if (argument[0] == '\0') {
      string_append(ch, &ch->pcdata->history);
      alter_character(ch);
      return;
    }

    return;
  }

  int location_by_name(char *arg1) {
    int location = -1;
    if (!str_cmp(arg1, "hands"))
    location = 0;
    else if (!str_cmp(arg1, "lowerarms"))
    location = 1;
    else if (!str_cmp(arg1, "upperarms"))
    location = 2;
    else if (!str_cmp(arg1, "feet"))
    location = 3;
    else if (!str_cmp(arg1, "lowerlegs") || !str_cmp(arg1, "calves"))
    location = 4;
    else if (!str_cmp(arg1, "forehead"))
    location = 5;
    else if (!str_cmp(arg1, "thighs"))
    location = 6;
    else if (!str_cmp(arg1, "groin"))
    location = 7;
    else if (!str_cmp(arg1, "arse") || !str_cmp(arg1, "buttocks") || !str_cmp(arg1, "rear") || !str_cmp(arg1, "ass"))
    location = 8;
    else if (!str_cmp(arg1, "lowerback"))
    location = 9;
    else if (!str_cmp(arg1, "upperback"))
    location = 10;
    else if (!str_cmp(arg1, "lowerchest"))
    location = 11;
    else if (!str_cmp(arg1, "breasts"))
    location = 12;
    else if (!str_cmp(arg1, "upperchest"))
    location = 13;
    else if (!str_cmp(arg1, "neck"))
    location = 14;
    else if (!str_cmp(arg1, "lowerface"))
    location = 15;
    else if (!str_cmp(arg1, "hair"))
    location = 16;
    else if (!str_cmp(arg1, "eyes"))
    location = 17;
    else if (!str_cmp(arg1, "smell"))
    location = 18;
    else if (!str_cmp(arg1, "everywhere") || !str_cmp(arg1, "self"))
    location = 19;

    return location;
  }

  char *name_by_location(int location) {
    if (location == 0)
    return "hands";
    else if (location == 1)
    return "lower arms";
    else if (location == 2)
    return "upper arms";
    else if (location == 3)
    return "feet";
    else if (location == 4)
    return "lower legs";
    else if (location == 5)
    return "forehead";
    else if (location == 6)
    return "thighs";
    else if (location == 7)
    return "groin";
    else if (location == 8)
    return "buttocks";
    else if (location == 9)
    return "lower back";
    else if (location == 10)
    return "upper back";
    else if (location == 11)
    return "lower chest";
    else if (location == 12)
    return "breasts";
    else if (location == 13)
    return "upper chest";
    else if (location == 14)
    return "neck";
    else if (location == 15)
    return "lower face";
    else if (location == 16)
    return "hair";
    else if (location == 17)
    return "eyes";
    else if (location == 18)
    return "smell";
    else if (location == 19)
    return "self";
    else
    return "unknown";
  }

  void display_orders(CHAR_DATA *ch) {
    int i, j, min = 2000, lastmin = -2000;
    for (j = 0; j < 20; j++) {
      if (ch->pcdata->focused_order[COVERS_ALL] < min && ch->pcdata->focused_order[COVERS_ALL] > lastmin)
      min = ch->pcdata->focused_order[COVERS_ALL];
      for (i = 0; i < MAX_COVERS + 1; i++) {

        if (ch->pcdata->focused_order[i] < min && ch->pcdata->focused_order[i] > lastmin)
        min = ch->pcdata->focused_order[i];
      }
      if (ch->pcdata->focused_order[COVERS_ALL] == min)
      printf_to_char(ch, "%12s: [%d]\n\r", capitalize(name_by_location(COVERS_ALL)), ch->pcdata->focused_order[COVERS_ALL]);

      for (i = 0; i < MAX_COVERS + 1; i++) {
        if (ch->pcdata->focused_order[i] == min)
        printf_to_char(ch, "%12s: [%d]\n\r", capitalize(name_by_location(i)), ch->pcdata->focused_order[i]);
      }
      lastmin = min;
      min = 2000;
    }
  }

  _DOFUN(do_focused) {
    char arg1[MSL];
    char arg2[MSL];
    int location = 1;

    if (ch->pcdata->desclock > 0) {
      send_to_char("You can't do that yet.\n\r", ch);
      return;
    }

    // dead characters should remain the way they were when they died - Discordance
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You are as the day you died.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "order")) {
      argument = one_argument_nouncap(argument, arg2);
      location = location_by_name(arg2);
      if (location == -1) {
        display_orders(ch);
        send_to_char("describe order (location) (number)\n\r", ch);
        return;
      }
      if (!is_number(argument)) {
        send_to_char("describe order (location) (number)\n\r", ch);
        return;
      }
      ch->pcdata->focused_order[location] = atoi(argument);
      send_to_char("Done.\n\r", ch);
      alter_character(ch);
      return;
    }
    if (!str_cmp(arg1, "file")) {
      string_append(ch, &ch->pcdata->file);
      alter_character(ch);
      return;
    }
    if (!str_cmp(arg1, "classified")) {
      string_append(ch, &ch->pcdata->classified);
      alter_character(ch);
      return;
    }
    if (!str_cmp(arg1, "pact")) {
      string_append(ch, &ch->pcdata->demon_pact);
      alter_character(ch);
      return;
    }
    if (!str_cmp(arg1, "goals")) {
      string_append(ch, &ch->pcdata->char_goals);
      alter_character(ch);
      return;
    }
    if (!str_cmp(arg1, "fears")) {
      string_append(ch, &ch->pcdata->char_fears);
      alter_character(ch);
      return;
    }
    if (!str_cmp(arg1, "secrets")) {
      if (is_roster_char(ch) && ch->played / 3600 < 50) {
        send_to_char("You can't view that yet.\n\r", ch);
        return;
      }
      string_append(ch, &ch->pcdata->char_secrets);
      alter_character(ch);
      return;
    }

    if (!str_cmp(arg1, "replace")) {
      argument = one_argument_nouncap(argument, arg2);
      location = get_redesclocation(arg2);
      if (location == -10) {
        send_to_char("No such location.\n\r", ch);
        return;
      }
      replace_focused(ch, location, argument);
      alter_character(ch);
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "append")) {
      argument = one_argument_nouncap(argument, arg2);
      location = get_redesclocation(arg2);
      if (location == -10) {
        send_to_char("No such location.\n\r", ch);
        return;
      }
      alter_character(ch);
      append_focused(ch, location, argument);
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "me") || !str_cmp(arg1, "self") || !str_cmp(arg1, "everywhere")) {
      alter_character(ch);
      string_append(ch, &ch->description);
      return;
    }
    else if (!str_cmp(arg1, "taste")) {
      alter_character(ch);
      string_append(ch, &ch->pcdata->taste);
      return;
    }
    else if (!str_cmp(arg1, "wolf")) {
      string_append(ch, &ch->pcdata->wolfdesc);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "Radioaction")) {
      string_append(ch, &ch->pcdata->radio_action);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "wolfchangeto")) {
      string_append(ch, &ch->pcdata->wolf_change_to);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "wolfchangefrom")) {
      string_append(ch, &ch->pcdata->wolf_change_from);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "merchangeto")) {
      string_append(ch, &ch->pcdata->mermaid_change_to);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "merchangefrom")) {
      string_append(ch, &ch->pcdata->mermaid_change_from);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "makeuplight")) {
      string_append(ch, &ch->pcdata->makeup_light);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "makeupmedium")) {
      string_append(ch, &ch->pcdata->makeup_medium);
      alter_character(ch);
      return;
    }
    else if(!str_cmp(arg1, "makeupheavy")) {
      string_append(ch, &ch->pcdata->makeup_heavy);
      alter_character(ch);
      return;
    }
    else if (!str_cmp(arg1, "tail")) {
      string_append(ch, &ch->pcdata->mermaiddesc);
      alter_character(ch);
      return;
    }
    else if ((location = location_by_name(arg1)) > -1) {

    }
    else {
      send_to_char("Syntax: describe (location)\nValid locations: self, hands, lowerarms, upperarms, feet, lowerlegs, thighs, groin, arse, lowerback, upperback, lowerchest, breasts, upperchest, neck, lowerface, forehead, hair, eyes, tail, smell, taste, wolf, radioaction, wolfchangeto, wolfchangefrom, merchangeto, merchangefrom, makeuplight, makeupmedium, makeupheavy\n\r", ch);
      return;
    }
    alter_character(ch);
    string_append(ch, &ch->pcdata->focused_descs[location]);
  }

  _DOFUN(do_detail) {
    char arg1[MSL];
    int location = 1;

    if (ch->pcdata->desclock > 0) {
      send_to_char("You can't do that yet.\n\r", ch);
      return;
    }

    // dead characters should remain the way they were when they died - Discordance
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You are as the day you died.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);
    location = location_by_name(arg1);
    if (location == -1 || location >= COVERS_SMELL) {
      send_to_char("Syntax: detail (location) (over/under)\nValid locations: self, hands, lowerarms, upperarms, feet, lowerlegs, thighs, groin, arse, lowerback, upperback, lowerchest, breasts, upperchest, neck, lowerface, forehead, hair, eyes.\n\r", ch);
      return;
    }
    alter_character(ch);
    if (!str_cmp(argument, "over")) {
      string_append(ch, &ch->pcdata->detail_over[location]);
    }
    else if (!str_cmp(argument, "under")) {
      string_append(ch, &ch->pcdata->detail_under[location]);
    }
    else {
      send_to_char("Syntax: detail (location) (over/under)\nValid locations: self, hands, lowerarms, upperarms, feet, lowerlegs, thighs,  groin, arse, lowerback, upperback, lowerchest, breasts, upperchest, neck, lowerface, forehead, hair, eyes.\n\r", ch);
      return;
    }
  }

  _DOFUN(do_victimdescribe) {
    CHAR_DATA *victim;
    char arg1[MSL];
    int location = 0;
    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      send_to_char("Not on NPCS.\n\r", ch);
      return;
    }

    if (!is_helpless(victim) && victim != ch) {
      send_to_char("Might help if you tie them up first.\n\r", ch);
      return;
    }

    captive_command(ch, victim);

    if (!str_cmp(argument, "me") || !str_cmp(argument, "self") || !str_cmp(argument, "them")) {
      string_append(ch, &victim->description);
      return;
    }
    else if (!str_cmp(argument, "hands"))
    location = 0;
    else if (!str_cmp(argument, "lowerarms"))
    location = 1;
    else if (!str_cmp(argument, "upperarms"))
    location = 2;
    else if (!str_cmp(argument, "feet"))
    location = 3;
    else if (!str_cmp(argument, "lowerlegs"))
    location = 4;
    else if (!str_cmp(argument, "forehead"))
    location = 5;
    else if (!str_cmp(argument, "thighs"))
    location = 6;
    else if (!str_cmp(argument, "groin"))
    location = 7;
    else if (!str_cmp(argument, "arse"))
    location = 8;
    else if (!str_cmp(argument, "lowerback"))
    location = 9;
    else if (!str_cmp(argument, "upperback"))
    location = 10;
    else if (!str_cmp(argument, "lowerchest"))
    location = 11;
    else if (!str_cmp(argument, "breasts"))
    location = 12;
    else if (!str_cmp(argument, "upperchest"))
    location = 13;
    else if (!str_cmp(argument, "neck"))
    location = 14;
    else if (!str_cmp(argument, "lowerface"))
    location = 15;
    else if (!str_cmp(argument, "hair"))
    location = 16;
    else if (!str_cmp(argument, "eyes"))
    location = 17;
    else if (!str_cmp(argument, "smell"))
    location = 18;
    else if (!str_cmp(argument, "wolf")) {
      string_append(ch, &victim->pcdata->wolfdesc);
      return;
    }
    else {
      send_to_char("Syntax is :victimdescribe (target) (location)\nValid locations: them, hands, lowerarms, upperarms, feet, lowerlegs, thighs, groin, arse, lowerback, upperback, lowerchest, breasts, upperchest, neck, lowerface, forehead, hair, eyes, smell, wolf.\n\r", ch);
      send_to_char("Each desc must start with a numerical value, 0-50\n\r", ch);
      return;
    }

    string_append(ch, &victim->pcdata->focused_descs[location]);
  }

  _DOFUN(do_trim) {
    char arg[MSL];

    if (ch->in_room->area->vnum != 3) {
      send_to_char("You have to be at home to do that.\n\r", ch);
      return;
    }

    remove_color(arg, argument);

    if (arg[0] == '\0' || safe_strlen(arg) < 2) {
      send_to_char("Trim to what?\n\r", ch);
      return;
    }

    free_string(ch->pcdata->pubic_hair);
    ch->pcdata->pubic_hair = str_dup(arg);

    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_descedit) {
    char buf[MSL];
    if (ch->pcdata->ci_editing == 1 || ch->pcdata->ci_editing == 2 || ch->pcdata->ci_editing == 4 || ch->pcdata->ci_editing == 5 || ch->pcdata->ci_editing == 6 || ch->pcdata->ci_editing == 9 || ch->pcdata->ci_editing == 10 || ch->pcdata->ci_editing == 11 || ch->pcdata->ci_editing == 12 || ch->pcdata->ci_editing == 16 || ch->pcdata->ci_editing == 21 || ch->pcdata->ci_editing == 23) {
      string_append(ch, &ch->pcdata->ci_desc);
      return;
    }

    if (!str_cmp(argument, "me") || !str_cmp(argument, "self")) {

      sprintf(buf, "Description:\n\r%s", ch->description);
      send_to_char(buf, ch);

      if (argument[0] == '\0') {
        string_append(ch, &ch->description);
        return;
      }

    }
    else
    do_function(ch, do_focused, argument);
    return;
  }

  _DOFUN(do_password) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if (IS_NPC(ch))
    return;

    if (safe_strlen(argument) < 3) {
      send_to_char("Your password must be at least 3 characters.\n\r", ch);
      return;
    }

    if (safe_strlen(ch->pcdata->account->pwd) < 2) {
      pwdnew = crypt(argument, ch->pcdata->account->name);
      for (p = pwdnew; *p != '\0'; p++) {
        if (*p == '~') {
          send_to_char("New password not acceptable, try again.\n\r", ch);
          return;
        }
      }

      free_string(ch->pcdata->account->pwd);
      ch->pcdata->account->pwd = str_dup(pwdnew);
      save_char_obj(ch, FALSE, FALSE);
      send_to_char("Ok.\n\r", ch);
      return;
    }

    /*
    * Can't use one_argument here because it smashes case.
    * So we just steal all its code.  Bleagh.
    */
    pArg = arg1;
    while (isspace(*argument))
    argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

    while (*argument != '\0') {
      if (*argument == cEnd) {
        argument++;
        break;
      }
      *pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while (isspace(*argument))
    argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

    while (*argument != '\0') {
      if (*argument == cEnd) {
        argument++;
        break;
      }
      *pArg++ = *argument++;
    }
    *pArg = '\0';
    if (arg1[0] == '\0' || arg2[0] == '\0') {
      send_to_char("Syntax: password (old) (new).\n\r", ch);
      return;
    }

    if (ch->pcdata->account->pwd != NULL && safe_strlen(ch->pcdata->account->pwd) > 1 && strcmp(crypt(arg1, ch->pcdata->account->pwd), ch->pcdata->account->pwd)) {
      WAIT_STATE(ch, 40);
      send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
      return;
    }

    if (safe_strlen(arg2) < 5) {
      send_to_char("New password must be at least five characters long.\n\r", ch);
      return;
    }

    /*
    * No tilde allowed because of player file format.
    */
    pwdnew = crypt(arg2, ch->pcdata->account->name);
    for (p = pwdnew; *p != '\0'; p++) {
      if (*p == '~') {
        send_to_char("New password not acceptable, try again.\n\r", ch);
        return;
      }
    }

    free_string(ch->pcdata->account->pwd);
    ch->pcdata->account->pwd = str_dup(pwdnew);
    save_char_obj(ch, FALSE, FALSE);
    send_to_char("Ok.\n\r", ch);
    return;
  }

  /**
  * Function: do_quotes
  * Descr   : Outputs an ascii file "quote.#" to the player.  The number
  *         : (#) is determined at random, based on how many quote files
  *	   : are stored in the QUOTE_DIR directory.
  * Returns : (void)
  * Syntax  : (none)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
  void do_quotes(CHAR_DATA *ch) {
    int i;
    char buf[MAX_STRING_LENGTH];
    FILE *fp;

    if (num_quotes <= 0) {
      send_to_char("All good things come to an end.", ch);
      return;
    }

    i = number_range(1, num_quotes);
    sprintf(buf, "%squote.%d", QUOTE_DIR, i);

    if ((fp = fopen(buf, "r")) == NULL) {
      log_string("No quote files found.");
      send_to_char("All good things must end...", ch);
      return;
    }

    send_to_char("\n\r", ch);
    while (!feof(fp)) {
      /* Be sure fgets read some more data, we don't want to send the last line twice. */
      if (fgets(buf, MAX_STRING_LENGTH, fp) != NULL)
      send_to_char(buf, ch);
    }

    send_to_char("\n\r", ch);
    fclose(fp);
  }

  _DOFUN(do_deposit) {
    char arg[MIL];
    char arg1[MIL];
    char buf[MIL];
    long amount = 0;

    argument = one_argument(argument, arg);

    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("Your account was closed when you died.\n\r", ch);
      return;
    }

    if (arg[0] == '\0') {
      send_to_char("Syntax: deposit (amount)\n\r", ch);
      return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK) && ch->in_room->sector_type != SECT_BANK) {
      send_to_char("You can't access your account from here.\n\r", ch);
      return;
    }

    if (is_number(arg)) {
      amount = atoi(arg);
      argument = one_argument(argument, arg1);
    }
    else {
      send_to_char("The amount must be a number.\n\r", ch);
      return;
    }
    amount *= 100;
    // Check to make sure a person has enough of the specified coin.
    if (amount > ch->money) {
      send_to_char("You don't have enough.\n\r", ch);
      return;
    }
    if (IS_AFFECTED(ch, AFF_DISCREDIT)) {
      send_to_char("The ATM returns an error.\n\r", ch);
      return;
    }

    if (amount < 0 || amount > ch->money) {
      send_to_char("A positive amount please.\n\r", ch);
      return;
    }

    sprintf(buf, "Thank you.  You just deposted %ld dollars.\n\r", amount / 100);
    send_to_char(buf, ch);
    ch->money -= amount;
    ch->pcdata->total_money += amount;

    return;
  }

  // Adjusted colors for consistency and added payday information - Discordance
  _DOFUN(do_balance) {
    char *monthstr;
    char spacer[] = "";
    char buf[MSL];
    int spacing, monthint, day;
    time_t payday;
    tm *ptm;

    payday = ch->pcdata->last_paid + 604000;
    payday = UMAX(payday, ch->pcdata->account->paycooldown + (3600 * 24 * 4));
    ptm = gmtime(&payday);
    day = ptm->tm_mday;

    ptm = gmtime(&payday);
    monthint = ptm->tm_mon;

    if (monthint > 11) {
      monthint = 0;
    }

    if (monthint == 0)
    monthstr = str_dup("January");
    else if (monthint == 1)
    monthstr = str_dup("February");
    else if (monthint == 2)
    monthstr = str_dup("March");
    else if (monthint == 3)
    monthstr = str_dup("April");
    else if (monthint == 4)
    monthstr = str_dup("May");
    else if (monthint == 5)
    monthstr = str_dup("June");
    else if (monthint == 6)
    monthstr = str_dup("July");
    else if (monthint == 7)
    monthstr = str_dup("August");
    else if (monthint == 8)
    monthstr = str_dup("September");
    else if (monthint == 9)
    monthstr = str_dup("October");
    else if (monthint == 10)
    monthstr = str_dup("November");
    else if (monthint == 11)
    monthstr = str_dup("December");
    else
    monthstr = str_dup("Error");

    send_to_char("     `gCash on hand`x: ", ch);
    sprintf(buf, "`G$`x%d.%02d`x", dollars(ch->money), cents(ch->money));
    spacing = 14 - safe_strlen_color(buf);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\r", buf);

    send_to_char("     `gBank Balance`x: ", ch);
    sprintf(buf, "`G$`x%d.%02d`x", dollars(ch->pcdata->total_money), cents(ch->pcdata->total_money));
    spacing = 14 - safe_strlen_color(buf);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\n\r", buf);

    if (ch->pcdata->total_credit > 0) {
      send_to_char("`gNew Player Credit`x: ", ch);
      sprintf(buf, "`G$`x%d.%02d`x", dollars(ch->pcdata->total_credit), cents(ch->pcdata->total_credit));
      spacing = 14 - safe_strlen_color(buf);
      printf_to_char(ch, "%*s", spacing, spacer);
      printf_to_char(ch, "%s\n\n\r", buf);
    }

    send_to_char("           `gPayday`x: ", ch);
    sprintf(buf, "%s %d%s", monthstr, day, datesuffix(day));
    spacing = 14 - safe_strlen_color(buf);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s", buf);

    printf_to_char(ch, "\n\n`gEstimated Pay`x: `G$`x%d.00\n\r", estimated_pay(ch));
    printf_to_char(ch, "`gEstimated Lifestyle Expenses`x: `G$`x%d.00\n\r", estimated_expenses(ch));
    printf_to_char(ch, "`gEstimated Vehicular Expenses`x: `G$`x%d.00\n\r", garage_charge(ch));
    return;
  }

  _DOFUN(do_withdraw) {
    char arg[MIL];
    char arg1[MIL];
    long amount = 0;
    char buf[MIL];
    argument = one_argument(argument, arg);

    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("Your account was closed when you died.\n\r", ch);
      return;
    }

    if (arg[0] == '\0') {
      send_to_char("Syntax: withdraw (amount)\n\r", ch);
      return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK) && ch->in_room->sector_type != SECT_BANK) {
      send_to_char("You can't access your account from here.\n\r", ch);
      return;
    }

    if (is_number(arg)) {
      amount = atoi(arg);
      argument = one_argument(argument, arg1);
    }
    else {
      send_to_char("The amount must be a number.\n\r", ch);
      return;
    }
    if (IS_AFFECTED(ch, AFF_DISCREDIT)) {
      send_to_char("The ATM returns an error.\n\r", ch);
      return;
    }
    amount *= 100;
    if (amount > ch->pcdata->total_money || amount <= 0) {
      send_to_char("You don't have enough money in the bank.\n\r", ch);
      return;
    }

    sprintf(buf, "Thank you.  You just withdrew %ld dollars.\n\r", amount / 100);
    send_to_char(buf, ch);

    ch->money += amount;
    ch->pcdata->total_money -= amount;
    return;
  }

  /* Display command.		Author: Michael MacLeod (Kuval) */

  _DOFUN(do_display) {
    char buf[MAX_STRING_LENGTH];

    sprintf(buf, "\n`g[`WName`x        `g]`x - %s\n", ch->name);
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WWho Rank    `g]`x - %s\n", get_whorank(ch));
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WWho Title   `g]`x - %s%s\n", ch->name, ch->pcdata->whotitle);
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WRoom Title  `g]`x - %s%s\n", ch->pcdata->intro_desc, ch->pcdata->title);
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WTalk        `g]`x - %s says, %s, \"Word.\"\n", capitalize(ch->pcdata->intro_desc), ch->pcdata->talk);
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WWalk        `g]`x - %s %s in from the right.\n", capitalize(ch->pcdata->intro_desc), movement_message(ch, FALSE, FALSE, 0));
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WIntro       `g]`x - %s\n", ch->pcdata->intro_desc);
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WNightmare Desc `g]`x - %s\n", shroud_desc(ch));
    send_to_char(buf, ch);

    sprintf(buf, "`g[`WCharDesc    `g]`x - \n");
    send_to_char(buf, ch);
    get_focused(ch, ch, TRUE);

    if (IS_IMMORTAL(ch)) {
      sprintf(buf, "`g[`WShort Desc  `g]`x - %s\n", ch->short_descr);
      send_to_char(buf, ch);

      sprintf(buf, "`g[`WPoofin      `g]`x - %s\n", ch->pcdata->bamfin);
      send_to_char(buf, ch);

      sprintf(buf, "`g[`WPoofout     `g]`x - %s\n", ch->pcdata->bamfout);
      send_to_char(buf, ch);
    }

    return;
  }

  void checkinform(CHAR_DATA *ch) { return; }

  _DOFUN(do_inform) { return; }

  _DOFUN(do_dlook) {
    char buf[MSL];
    CHAR_DATA *victim;

    if (argument[0] == '\0') {
      send_to_char("Who where?\n\r", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (victim->description[0] != '\0') {
      send_to_char(victim->description, ch);
    }
    else {
      act("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
    }

    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);

    return;
  }

  _DOFUN(do_jetlag) {
    int t_offset;

    if (IS_NPC(ch))
    return;

    if (argument[0] == '\0') {
      printf_to_char(ch, "\n\rYour jetlag is set to: %d.\n\r", ch->pcdata->jetlag);
      return;
    }

    if (!is_number(argument)) {
      printf_to_char(
      ch, "\n\rYou can only add or subtract numbers from the time!\n\r");
      return;
    }

    t_offset = atoi(argument);
    ch->pcdata->jetlag = t_offset;
    printf_to_char(ch, "\n\rYour jetlag is changed to %d.\n\r", t_offset);
    return;
  }

    /*	Command   : do_mail, by Kuval [Michael MacLeod]		*
     * 	Use       : Get a characters email address online.	*
     *    Date added: 12/28/00					*/
    /*
    _DOFUN(do_mail)
    {

    CHAR_DATA *victim;
    char buf[MSL];

    if( argument[0] == '\0')
    {
      send_to_char( "Syntax: email [character]\n\r", ch);
      return;
    }

    if((victim = get_char_world(ch, argument)) == NULL)
    {
      send_to_char("That player is not currently logged on. I will make this info
      available offline in the near future.\n\r", ch); return;
    }

    sprintf(buf, "Their e-mail address is: %s.\n\r", victim->pcdata->email );
    send_to_char(buf, ch);
    return;

    return;

    }

    */

  char *solechar(CHAR_DATA *ch) {
    if (ch->pcdata->account == NULL)
    return "Unknown";

    if (ch->pcdata->account->focusedcount < 75 && ch->played / 3600 > ch->pcdata->account->focusedcount / 2)
    return "No";

    if (!str_cmp(
          ch->pcdata->account->characters[ch->pcdata->account->focusedchar], ch->name))
    return "Yes";

    return "No";
  }
  /**
  * This command controls what stats are printed out in
  * a character's scoresheet.
  */

  _DOFUN(do_finger) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    int dtime;
    CHAR_DATA *victim;

    char string[MSL];
    char spacer[] = "";
    char wikipage[MSL];
    char border[MSL], fields[MSL], data[MSL];
    int spacing;
    char endcap[MSL];

    // defines color
    strcpy(border, "`g");
    strcpy(fields, "`c");
    strcpy(data, "`x");

    if (argument[0] == '\0') {
      send_to_char("\n\rFinger usage:\n\r\n\rfinger (target)\n\r", ch);
      return;
    }

    d.original = NULL;
    if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, argument)) {
        printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(argument));
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
      stat(buf, &sb);
      victim = d.character;
    }

    if (IS_NPC(victim)) {
      send_to_char("\n\rYou can't finger mobiles!\n\r", ch);
      if (!online)
      free_char(victim);

      return;
    }

    if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)) {
      send_to_char("One word, No.\n\r", ch);
      if (!online)
      free_char(victim);

      return;
    }

    if (online && is_whoinvis(victim) && str_cmp(argument, victim->name)) {
      printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(argument));

      return;
    }
    /* Disabling wiki links for general release - Disco
    if (is_gm(victim)) {
      sprintf(wikipage, "http://havenrpg.net/wiki/pmwiki.php/StoryRunners/%s", victim->name);
    }
    else if (higher_power(victim)) {
      sprintf(wikipage, "http://havenrpg.net/wiki/pmwiki.php/HigherPower/%s", victim->name);
    }
    else {
      char *dupLastName;
      char compLastName[MSL];
      char arg2[MSL], arg3[MSL], arg4[MSL];
      int i, j, cap;

      // breaking down lastname into words
      dupLastName = strdup(victim->pcdata->last_name);
      dupLastName = one_argument_nouncap(dupLastName, arg2);
      dupLastName = one_argument_nouncap(dupLastName, arg3);
      dupLastName = one_argument_nouncap(dupLastName, arg4);
      free_string(dupLastName);

      // reassembling lastname without spaces
      arg2[0] = toupper(arg2[0]);
      strcpy(compLastName, arg2);
      if (safe_strlen(arg3) > 1) {
        arg3[0] = toupper(arg3[0]);
        strcat(compLastName, arg3);
        if (safe_strlen(arg4) > 1) {
          arg4[0] = toupper(arg4[0]);
          strcat(compLastName, arg4);
        }
      }

      // purging apostraphes
      cap = safe_strlen(compLastName);
      for (i = 0; i < cap; i++) {
        if (compLastName[i] == 39) {
          cap = cap - 1;
          for (j = i; j < cap; j++) {
            compLastName[j] = compLastName[j + 1];
          }
        }
      }
      compLastName[cap] = '\0';

      sprintf(wikipage, "http://havenrpg.net/wiki/pmwiki.php/Characters/%s%s", victim->name, compLastName);
    }
    */

    char no_color[MSL];         // for determining line length for truncation
    char where[MSL], when[MSL]; // for truncating without altering stored strings

    // Top border
    printf_to_char(ch, "%s,______________________________________________________________________________,`x\n\r", border);
    // Account
    if (IS_IMMORTAL(ch)) {
      sprintf(string, "%s| %sAccnt%s: %s%s", border, fields, border, data, (victim->pcdata->account == NULL) ? "Unavailable" : victim->pcdata->account->name);
      printf_to_char(ch, "%s", string);
      sprintf(endcap, "%s|`x", border);
      spacing = 80 - safe_strlen_color(string) - safe_strlen_color(endcap);
      printf_to_char(ch, "%*s", spacing, spacer);
      printf_to_char(ch, "%s\n\r", endcap);
    }
    // Name
    sprintf(string, "%s| %s Name%s: %s%s %s", border, fields, border, data, victim->name, victim->pcdata->last_name);
    printf_to_char(ch, "%s", string);
    sprintf(endcap, "%s|`x", border);
    spacing = 80 - safe_strlen_color(string) - safe_strlen_color(endcap);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\r", endcap);
    // Time
    strcpy(when, victim->pcdata->ftime);
    sprintf(string, "%s| %s Time%s: %s%s", border, fields, border, data, when);
    // Time truncation
    remove_color(no_color, when);
    if (safe_strlen_color(no_color) > 78) {
      when[77] = '\0'; // truncating
      sprintf(string, "%s| %s Time%s: %s%s", border, fields, border, data, when);
    }

    printf_to_char(ch, "%s", string);
    sprintf(endcap, "%s|`x", border);
    spacing = 80 - safe_strlen_color(string) - safe_strlen_color(endcap);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\r", endcap);
    // Place
    strcpy(where, victim->pcdata->floc);
    sprintf(string, "%s| %sPlace%s: %s%s", border, fields, border, data, where);
    // Place truncation
    remove_color(no_color, where);
    if (safe_strlen_color(no_color) > 78) {
      where[77] = '\0'; // truncating
      sprintf(string, "%s| %sPlace%s: %s%s", border, fields, border, data, where);
    }

    printf_to_char(ch, "%s", string);
    sprintf(endcap, "%s|`x", border);
    spacing = 80 - safe_strlen_color(string) - safe_strlen_color(endcap);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\r", endcap);
    /*
    //Hours
    sprintf(string, "%s| %s    Hours%s: %s%d", border, fields, border, data, victim->played/3600); printf_to_char(ch, "%s", string); sprintf(endcap, "%s|`x", border); spacing=80-safe_strlen_color(string)-safe_strlen_color(endcap);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\r", endcap);
    */
    // Wiki Page
    /* Disabling wiki stuff for general release
    sprintf(string, "%s| %s Page%s: %s%s", border, fields, border, data, wikipage);
    printf_to_char(ch, "%s", string);
    sprintf(endcap, "%s|`x", border);
    spacing = 80 - safe_strlen_color(string) - safe_strlen_color(endcap);
    printf_to_char(ch, "%*s", spacing, spacer);
    printf_to_char(ch, "%s\n\r", endcap);
    */
    // Idle For
    if (online && (ch->in_room == victim->in_room || can_see_char_distance(ch, victim, DISTANCE_MEDIUM))) {
      dtime = (int)difftime(current_time, victim->idle);
      sprintf(string, "%s| %s Idle%s: %s%d %s %d %s", border, fields, border, data, dtime / 60, (dtime / 60 == 1) ? "minute" : "minutes", dtime % 60, (dtime % 60 == 1) ? "second" : "seconds");
      printf_to_char(ch, "%s", string);
      sprintf(endcap, "%s|`x", border);
      spacing = 80 - safe_strlen_color(string) - safe_strlen_color(endcap);
      printf_to_char(ch, "%*s", spacing, spacer);
      printf_to_char(ch, "%s\n\r", endcap);
    }
    // Bottom border
    printf_to_char(ch, "%s|______________________________________________________________________________|`x\n\r", border);
    // Description
    if (IS_FLAG(victim->act, PLR_GM)) {
      if (safe_strlen_color(victim->pcdata->account->sr_fdesc) > 2) {
        sprintf(string, "\n\r%s`x\n\r", victim->pcdata->account->sr_fdesc);
        printf_to_char(ch, "%s", string);
      }
    }
    else if (higher_power(victim)) {
      if (safe_strlen_color(victim->pcdata->fdesc) > 2) {
        sprintf(string, "\n\r%s`x\n\r", victim->pcdata->fdesc);
        printf_to_char(ch, "%s", string);
      }
    }

    send_to_char(outbuf, ch);

    if (!online)
    free_char(victim);
    return;
  }

  _DOFUN(do_fingerset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg1);
    strcpy(arg2, argument);

    if (IS_NPC(ch)) {
      return;
    }

    if (arg1[0] == '\0') {
      send_to_char("Syntax is:\n\rfingerset time (best contact time)\n\rfingerset location (your location)\n\rfingerset description (short char desc)\n\r", ch);
      return;
    }

    if (!str_prefix(arg1, "time")) {
      if (arg2[0] == '\0') {
        send_to_char("When are you on the most?\n\r", ch);
        return;
      }
      if (safe_strlen(arg2) > 70) {
        send_to_char("Field too long. Truncated.\r\n", ch);
        arg2[69] = '\0';
      }

      free_string(ch->pcdata->ftime);
      ch->pcdata->ftime = str_dup(arg2);
      printf_to_char(ch, "Your time on finger is now %s.\n\r", ch->pcdata->ftime);
    }

    if (!str_prefix(arg1, "place")) {
      if (arg2[0] == '\0') {
        send_to_char("Where do you live?\n\r", ch);
        return;
      }
      if (safe_strlen(arg2) > 70) {
        send_to_char("Field too long. Truncated.\r\n", ch);
        arg2[69] = '\0';
      }
      free_string(ch->pcdata->floc);
      ch->pcdata->floc = str_dup(arg2);
      printf_to_char(ch, "Your place on finger is now %s.\n\r", ch->pcdata->floc);
    }

    /*
    if( !str_prefix(arg1, "website"))
    {
      if( arg2[0] == '\0' )
      {
        send_to_char("What is your SR's wikipage.\n\r", ch );
        return;
      }

      free_string(ch->pcdata->account->sr_website);
      ch->pcdata->account->sr_website = str_dup(arg2);
      printf_to_char(ch, "Your website on finger is now %s.\n\r", ch->pcdata->account->sr_website);
    }

    if( !str_prefix(arg1, "logs"))
    {
      send_to_char("List URLs for any logs or story summaries of your SRing.\n\r", ch); string_append(ch, &ch->pcdata->account->sr_logs);
    }
    */

    if (!str_prefix(arg1, "description")) {
      if (IS_FLAG(ch->act, PLR_GM)) {
        string_append(ch, &ch->pcdata->account->sr_fdesc);
      }
      else if (higher_power(ch)) {
        string_append(ch, &ch->pcdata->fdesc);
      }
      else {
        send_to_char("Only SRs can set descriptions in their finger information.\n\r", ch);
      }
      return;

      if (arg2[0] == '\0') {
        send_to_char("What do you want your description to be?\n\r", ch);
        return;
      }

      free_string(ch->pcdata->fdesc);
      ch->pcdata->fdesc = str_dup(arg2);
      printf_to_char(ch, "Your description on finger is now %s.\n\r", ch->pcdata->fdesc);
    }

    return;
  }

  _DOFUN(print_level_value) {
    char buf[MSL];
    int number = 0;
    int result = 2000;
    int i = 0;

    if (!is_number(argument)) {
      send_to_char("That isn't a number, stupid.\n\r", ch);
      return;
    }

    number = atoi(argument);
    sprintf(buf, "Your original number was: %d\n\r", number);
    send_to_char(buf, ch);

    for (i = 0; number > 0; ++i) {
      result += 2 * i;
      number -= result;
    }

    sprintf(buf, "Your level is: %d\n\r", i);
    send_to_char(buf, ch);
    return;
  }

  bool is_covered(CHAR_DATA *ch, int sel) {
    OBJ_DATA *obj;
    int iWear;
    bool found = FALSE;

    if (IS_FLAG(ch->comm, COMM_BLINDFOLD) && sel == COVERS_EYES)
    return TRUE;

    if (in_bath(ch) || deep_water(ch) || in_stream(ch)) {
      if (sel == COVERS_BREASTS)
      return TRUE;
      if (sel == COVERS_LOWER_CHEST)
      return TRUE;
      if (sel == COVERS_LOWER_BACK)
      return TRUE;
      if (sel == COVERS_ARSE)
      return TRUE;
      if (sel == COVERS_GROIN)
      return TRUE;
      if (sel == COVERS_THIGHS)
      return TRUE;
      if (sel == COVERS_LOWER_LEGS)
      return TRUE;
      if (sel == COVERS_FEET)
      return TRUE;
    }

    for (int i = 0; i < MAX_COVERS; i++) {
      if (cover_table[i] == sel && ch->pcdata->exposed[i] == 111)
      return FALSE;
    }

    for (iWear = 0; iWear < MAX_WEAR && !found; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (does_cover(obj, sel))
      return TRUE;
    }
    return FALSE;
  }

  bool is_transperant(OBJ_DATA *obj) {
    if (obj->item_type != ITEM_CLOTHING)
    return FALSE;
    if (obj->value[0] == 0)
    return TRUE;

    if (obj->value[4] < 10)
    return FALSE;

    if (is_name("white", obj->short_descr) || is_name("sheer", obj->short_descr) || is_name("translucent", obj->short_descr) || is_name("cream", obj->short_descr) || is_name("ivory", obj->short_descr)) {
      if (is_name("leather", obj->short_descr) || is_name("suede", obj->short_descr) || is_name("wool", obj->short_descr) || is_name("satin", obj->short_descr) || is_name("fur", obj->short_descr) || is_name("cashmere", obj->short_descr) || is_name("fleece", obj->short_descr) || is_name("plyester", obj->short_descr))
      return FALSE;

      if (IS_SET(obj->extra_flags, ITEM_ARMORED))
      return FALSE;

      if (is_name("cotton", obj->short_descr) || is_name("t-shirt", obj->short_descr))
      return TRUE;

      if (is_name("jacket", obj->short_descr) || is_name("coat", obj->short_descr))
      return FALSE;

      return TRUE;
    }

    return FALSE;
  }

  bool does_cover(OBJ_DATA *obj, int selection) {
    int i;

    if (obj->item_type != ITEM_CLOTHING)
    return FALSE;
    if (obj->value[0] == 0)
    return FALSE;

    if (is_transperant(obj))
    return FALSE;

    if (obj->value[4] > 10 && (is_name("white", obj->short_descr) || is_name("white,", obj->short_descr) || is_name("gauzy", obj->short_descr) || is_name("gauzy,", obj->short_descr) || is_name("sheer", obj->short_descr) || is_name("sheer,", obj->short_descr) || is_name("gossamer", obj->short_descr) || is_name("gossamer,", obj->short_descr)))
    return FALSE;

    int value = obj->value[0];
    for (i = 0; i < MAX_COVERS; i++) {
      if (selection == cover_table[i]) {
        if (value >= selection)
        return TRUE;
        else
        return FALSE;
      }
      else {
        if (value >= cover_table[i])
        value -= cover_table[i];
      }
    }
    return FALSE;
  }

  bool does_cover_wetless(OBJ_DATA *obj, int selection) {
    int i;
    if (obj->item_type != ITEM_CLOTHING)
    return FALSE;
    if (obj->value[0] == 0)
    return FALSE;

    int value = obj->value[0];
    for (i = 0; i < MAX_COVERS; i++) {
      if (selection == cover_table[i]) {
        if (value >= selection)
        return TRUE;
        else
        return FALSE;
      }
      else {
        if (value >= cover_table[i])
        value -= cover_table[i];
      }
    }
    return FALSE;
  }
  bool does_undercover(OBJ_DATA *obj, int selection) {
    int i;
    if (obj->item_type == ITEM_JEWELRY) {
      if (obj->value[3] == 0)
      return FALSE;
      int value = obj->value[3];
      for (i = 0; i < MAX_COVERS; i++) {
        if (selection == cover_table[i]) {
          if (value >= selection)
          return TRUE;
          else
          return FALSE;
        }
        else {
          if (value >= cover_table[i])
          value -= cover_table[i];
        }
      }
      return FALSE;
    }
    if (obj->item_type == ITEM_CONTAINER && selection == COVERS_LOWER_BACK)
    return TRUE;
    if (obj->item_type != ITEM_CLOTHING)
    return FALSE;
    if (obj->value[0] == 0)
    return FALSE;
    int value = obj->value[0];
    for (i = 0; i < MAX_COVERS; i++) {
      if (selection == cover_table[i]) {
        if (value >= selection)
        return TRUE;
        else
        return FALSE;
      }
      else {
        if (value >= cover_table[i])
        value -= cover_table[i];
      }
    }
    return FALSE;
  }
  bool does_conceal(OBJ_DATA *obj_over, OBJ_DATA *obj_under) {
    int i;

    if (obj_over == NULL) {
      return FALSE;
    }
    if (obj_under == NULL) {
      return TRUE;
    }

    if (obj_under->exposed == 111) {
      return FALSE;
    }
    if (obj_under->value[0] == 0 && obj_under->item_type == ITEM_CLOTHING) {
      return FALSE;
    }
    if (obj_under->value[3] == 0 && obj_under->item_type == ITEM_JEWELRY) {
      return FALSE;
    }
    if (obj_over->item_type != ITEM_CLOTHING) {
      return FALSE;
    }

    for (i = 0; i < MAX_COVERS; i++) {
      if (does_undercover(obj_under, cover_table[i]) && !does_cover(obj_over, cover_table[i])) {
        return FALSE;
      }
    }

    return TRUE;
  }

  char *get_focused(CHAR_DATA *ch, CHAR_DATA *victim, bool xray) {
    int i, j, len;
    int min = 2000;
    int lastmin = -2000;
    char string[MSL];
    string[0] = '\0';
    char buf[MSL];

    if (IS_NPC(victim)) {
      return victim->description;
    }
    if (has_xray(ch))
    xray = TRUE;

    for (j = 0; j < 20; j++) {
      if (victim->pcdata->focused_order[COVERS_ALL] < min && victim->pcdata->focused_order[COVERS_ALL] > lastmin)
      min = victim->pcdata->focused_order[COVERS_ALL];
      for (i = 0; i < MAX_COVERS + 1; i++) {
        if ((cover_table[i] == COVERS_ARSE || cover_table[i] == COVERS_GROIN || cover_table[i] == COVERS_THIGHS || cover_table[i] == COVERS_LOWER_LEGS || cover_table[i] == COVERS_FEET) && victim->shape == SHAPE_MERMAID)
        continue;

        if (victim->pcdata->focused_order[i] < min && victim->pcdata->focused_order[i] > lastmin)
        min = victim->pcdata->focused_order[i];
      }

      if (victim->pcdata->focused_order[COVERS_ALL] == min && str_cmp(victim->description, "") && safe_strlen(victim->description) > 0) {
        sprintf(buf, "%s", victim->description);
        len = safe_strlen(buf);
        if (buf[len - 2] == '\n')
        buf[len - 2] = 0;
        strcat(string, buf);
      }

      for (i = 0; i < MAX_COVERS + 1; i++) {
        if ((cover_table[i] == COVERS_ARSE || cover_table[i] == COVERS_GROIN || cover_table[i] == COVERS_THIGHS || cover_table[i] == COVERS_LOWER_LEGS || cover_table[i] == COVERS_FEET) && victim->shape == SHAPE_MERMAID)
        continue;

        if (i == COVERS_SMELL && (safe_strlen(victim->pcdata->focused_descs[i]) > 2 && ch->in_room == victim->in_room && (get_skill(ch, SKILL_ACUTESMELL) > 0 || ch == victim))) {
          if (victim->pcdata->focused_order[i] == min && str_cmp(victim->pcdata->focused_descs[i], "") && (!is_covered(victim, cover_table[i]) || xray == TRUE)) {
            sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
            len = safe_strlen(buf);
            if (buf[len - 2] == '\n')
            buf[len - 2] = 0;
            strcat(string, buf);
          }
        }
        else if (i != COVERS_SMELL) {
          if (victim->pcdata->focused_order[i] == min && str_cmp(victim->pcdata->focused_descs[i], "") && (!is_covered(victim, cover_table[i]) || xray == TRUE)) {
            sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
            len = safe_strlen(buf);
            if (buf[len - 2] == '\n')
            buf[len - 2] = 0;
            strcat(string, buf);
          }
          if (victim->pcdata->focused_order[i] == min && !is_covered(victim, cover_table[i]) && victim->pcdata->brandlocation == i && victim->pcdata->branddate > 0) {
            sprintf(buf, "%s has a symbol of %s on %s %s. ", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->brandstring, (victim->sex == SEX_MALE) ? "his" : "her", name_by_location(i));
            strcat(string, buf);
          }
          if (victim->pcdata->focused_order[i] == min && !is_covered(victim, cover_table[i]) && str_cmp(victim->pcdata->scars[i], "")) {
            sprintf(buf, "%s ", victim->pcdata->scars[i]);
            strcat(string, buf);
          }
        }
      }
      lastmin = min;
      min = 2000;
    }

    if (safe_strlen(victim->pcdata->mermaiddesc) > 2 && victim->shape == SHAPE_MERMAID) {
      sprintf(buf, " %s\n", victim->pcdata->mermaiddesc);
      strcat(string, buf);
    }

    if(victim->pcdata->attract[ATTRACT_MAKEUP] == 1 && safe_strlen(victim->pcdata->makeup_light) > 2)
    {
      sprintf(buf, " %s\n", victim->pcdata->makeup_light);
      strcat(string, buf);
    }
    if(victim->pcdata->attract[ATTRACT_MAKEUP] == 2 && safe_strlen(victim->pcdata->makeup_medium) > 2)
    {
      sprintf(buf, " %s\n", victim->pcdata->makeup_medium);
      strcat(string, buf);
    }
    if(victim->pcdata->attract[ATTRACT_MAKEUP] == 3 && safe_strlen(victim->pcdata->makeup_heavy) > 2)
    {
      sprintf(buf, " %s\n", victim->pcdata->makeup_heavy);
      strcat(string, buf);
    }

    if (safe_strlen(victim->pcdata->scent) > 2 && ch->in_room == victim->in_room && (get_skill(ch, SKILL_ACUTESMELL) > 0 || (same_place(ch, victim)) || ch == victim)) {
      sprintf(buf, " %s\n", victim->pcdata->scent);
      strcat(string, buf);
    }

    char *result = str_dup(string);
    return result;
  }

  char *mad_text(CHAR_DATA *ch) {
    char buf[MSL];
    int x = -1;
    for (int i = 0; i < 50; i++) {
      x = number_range(0, 49);
      if (safe_strlen(ch->pcdata->speed_names[x]) < 2)
      x = -1;
    }
    if (x < 0)
    return "";
    switch (number_percent() % 7) {
    case 0:
      sprintf(buf, "%s: You should kill yourself.\n\r", ch->pcdata->speed_names[x]);
      break;
    case 1:
      sprintf(buf, "%s: I know what you did.\n\r", ch->pcdata->speed_names[x]);
      break;
    case 2:
      sprintf(buf, "%s: I hate you.\n\r", ch->pcdata->speed_names[x]);
      break;
    case 3:
      sprintf(buf, "%s: I wish you would die.\n\r", ch->pcdata->speed_names[x]);
      break;
    case 4:
      sprintf(buf, "%s: You're pathetic.\n\r", ch->pcdata->speed_names[x]);
      break;
    case 5:
      sprintf(buf, "%s: I never really liked you.\n\r", ch->pcdata->speed_names[x]);
      break;
    default:
      if (ch->sex == SEX_FEMALE)
      sprintf(buf, "%s: Whore.\n\r", ch->pcdata->speed_names[x]);
      else
      sprintf(buf, "%s: Coward.\n\r", ch->pcdata->speed_names[x]);
      break;
    }
    return str_dup(buf);
  }

  char *mad_texts(CHAR_DATA *ch) {
    char otext[MSL];
    strcpy(otext, mad_text(ch));
    if (number_percent() % 3 == 0)
    strcat(otext, mad_text(ch));
    if (number_percent() % 2 == 0)
    strcat(otext, mad_text(ch));
    return str_dup(otext);
  }

  const char *round_augments[] = {
    "normal",      "advanced",           "silver", "gold",   "wooden", "incendiary",  "ferromagnetic iron", "bone",   "quartz", "obsidian", "radioactive", "armor piercing"};
  const char *melee_augments[] = {
    "none",        "enhanced",           "silver", "gold",   "wood", "incendiary",  "ferromagnetic iron", "bone",   "quartz", "obsidian", "radioactive", "armor piercing"};

  void show_obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch, bool photo, bool texts) {
    char buf[MSL];
    char otext[MSL];
    strcpy(otext, "");
    strcat(otext, obj->description);
    strcat(otext, "\n\r");
    char *pdesc;
    EXTRA_DESCR_DATA *ed;

    if (IS_SET(obj->extra_flags, ITEM_RELIC) && obj->carried_by == ch) {
      sprintf(buf, "Cost: Priceless\n\r");
      strcat(otext, buf);
    }
    else if (obj->item_type != ITEM_BABY) {

      sprintf(buf, "Cost: %d.%02d\n\r", dollars(obj->cost), cents(obj->cost));
      strcat(otext, buf);

      if (obj->condition < 100) {
        sprintf(buf, "Condition: %d\n\r", obj->condition);
        strcat(otext, buf);
      }
    }

    if (is_practice_arm(obj)) {
      strcat(otext, "It's made for practicing.\n\r");
    }

    if (IS_SET(obj->extra_flags, ITEM_FOCUS) && obj->carried_by == ch)
    strcat(otext, "It has runes carved into it.\n\r");

    if (IS_SET(obj->extra_flags, ITEM_CURSED) && obj->carried_by == ch && get_skill(ch, SKILL_DEMONOLOGY) >= 4)
    strcat(otext, "It is cursed.\n\r");

    if (IS_SET(obj->extra_flags, ITEM_COLLAR) && obj->carried_by == ch && is_angelborn(ch))
    strcat(otext, "It is an angelic collar.\n\r");

    if (IS_SET(obj->extra_flags, ITEM_FOCUS) && get_skill(ch, SKILL_DEMONOLOGY) >= 2)
    strcat(otext, "It is an arcane focus.\n\r");

    if (obj->buff > 0 && obj->carried_by == ch && IS_SET(obj->extra_flags, ITEM_ARMORED)) {
      sprintf(buf, "It looks like it could offer enhanced protection and physical augmentation for another %d hits.\n\r", obj->buff);
      strcat(otext, buf);
    }
    else if (obj->buff > 0 && obj->carried_by == ch) {
      if (obj->item_type == ITEM_ARMOR) {
        sprintf(buf, "It looks like it could offer enhanced protection against another %d hits.\n\r", obj->buff);
        strcat(otext, buf);
      }
      else if (IS_SET(obj->extra_flags, ITEM_AMMOCHANGED)) {
        if (obj->item_type == ITEM_RANGED) {
          if (obj->value[2] == AMMO_AUGMENTED) {
            sprintf(buf, "[`rInactive`x] It looks like it has about %d advanced rounds left.\n\r", obj->buff);
            strcat(otext, buf);
          }
          else if (obj->value[2] == AMMO_PARALYTIC || obj->value[2] == AMMO_POISON) {
            sprintf(buf, "[`rInactive`x] It looks like it has some sort of poison applied to it.\n\r");
            strcat(otext, buf);
          }
          else {
            sprintf(buf, "[`rInactive`x] It looks like it has about %d %s rounds left.\n\r", obj->buff, round_augments[obj->value[2]]);
            strcat(otext, buf);
          }
        }
        else {
          if (obj->value[2] == AMMO_AUGMENTED) {
            sprintf(buf, "[`rInactive`x] It looks like the weapon's enhancements will last about another %d attacks.\n\r", obj->buff);
            strcat(otext, buf);
          }
          else if (obj->value[2] == AMMO_PARALYTIC || obj->value[2] == AMMO_POISON) {
            sprintf(buf, "[`rInactive`x] It looks like it has some sort of poison applied to it.\n\r");
            strcat(otext, buf);
          }
          else {
            sprintf(buf, "[`rInactive`x] It looks like the weapons's %s augmentation will last another %d attacks.\n\r", melee_augments[obj->value[2]], obj->buff);
            strcat(otext, buf);
          }
        }
      }
      else {
        if (obj->item_type == ITEM_RANGED) {
          if (obj->value[2] == AMMO_AUGMENTED) {
            sprintf(buf, "It looks like it has about %d advanced rounds left.\n\r", obj->buff);
            strcat(otext, buf);
          }
          else if (obj->value[2] == AMMO_PARALYTIC || obj->value[2] == AMMO_POISON) {
            sprintf(
            buf, "It looks like it has some sort of poison applied to it.\n\r");
            strcat(otext, buf);
          }
          else {
            sprintf(buf, "It looks like it has about %d %s rounds left.\n\r", obj->buff, round_augments[obj->value[2]]);
            strcat(otext, buf);
          }
        }
        else {
          if (obj->value[2] == AMMO_AUGMENTED) {
            sprintf(buf, "It looks like the weapon's enhancements will last about another %d attacks.\n\r", obj->buff);
            strcat(otext, buf);
          }
          else if (obj->value[2] == AMMO_PARALYTIC || obj->value[2] == AMMO_POISON) {
            sprintf(
            buf, "It looks like it has some sort of poison applied to it.\n\r");
            strcat(otext, buf);
          }
          else {
            sprintf(buf, "It looks like the weapons's %s augmentation will last another %d attacks.\n\r", melee_augments[obj->value[2]], obj->buff);
            strcat(otext, buf);
          }
        }
      }
    }
    if (obj->item_type == ITEM_PHONE && (texts == TRUE)) {
      bool signalboost = false;
      if (IS_SET(obj->extra_flags, ITEM_OFF)) {
        strcat(otext, "It is switched off.\n\r");
      }

      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+signalboost", ed->keyword)) {
          signalboost = true;
          break;
        }
      }
      if (signalboost == true && get_skill(ch, SKILL_ENGINEERING) >= 2) {
        strcat(otext, "It is signalboosted.\n\r");
      }
      sprintf(buf, "Phone Number: %d\n\r", obj->value[0]);
      strcat(otext, buf);

      if (obj->carried_by != NULL && !IS_NPC(obj->carried_by) && obj->carried_by->pcdata->connected_to != NULL && obj->carried_by->pcdata->connection_stage == CONNECT_RINGING && get_phone(obj->carried_by->pcdata->connected_to) != NULL) {
        int calnum = get_phone(obj->carried_by->pcdata->connected_to)->value[0];
        sprintf(buf, "Calling: %d\n\r", calnum);
        strcat(otext, buf);
      }
      if (str_cmp(obj->material, "") && !IS_AFFECTED(ch, AFF_MAD) && !IS_FLAG(ch->act, PLR_SHROUD)) {
        sprintf(buf, "%s\n\r", obj->material);
        strcat(otext, buf);
        if (photo == FALSE)
        triggercheck(ch, obj->material);
      }
      if (IS_AFFECTED(ch, AFF_MAD) && photo == FALSE && obj->carried_by != NULL && obj->carried_by == ch) {
        strcat(otext, mad_texts(ch));
      }
    }
    if (obj->item_type == ITEM_DRINK_CON && obj->value[0] > 0) {
      int fullness = 100 * obj->value[1] / obj->value[0];
      sprintf(buf, "It looks about %d percent full.\n\r", fullness);
      strcat(otext, buf);
    }
    if (obj->pIndexData->vnum == 36) {
      if (obj->level > 0 && obj->level <= get_skill(ch, SKILL_DEMONOLOGY) * 10) {
        sprintf(
        buf, "That looks to be worth about $%d on the supernatural market.\n\r", obj->level * number_range(7, 13));
        strcat(otext, buf);
      }
    }
    if (obj->pIndexData->vnum == 38) {
      if (obj->level > 0 && obj->level <= get_skill(ch, SKILL_DEMONOLOGY) * 10) {
        sprintf(buf, "That looks to be worth about $%d to an alchemical lab.\n\r", obj->level * number_range(7, 13));
        strcat(otext, buf);
      }
    }
    if (obj->pIndexData->vnum == 74 && (!is_gm(ch) || IS_IMMORTAL(ch))) {
      int compass = compass_direction(ch->in_room);
      if (compass == -1 || is_ghost(ch)) {
        strcat(otext, "The needle is slowly spinning in circles.\n\r");
      }
      else {
        sprintf(buf, "The needle is pointing %s.\n\r", dir_name[compass][0]);
        strcat(otext, buf);
      }
    }

    if (obj->item_type == ITEM_FLASHLIGHT) {
      if (IS_SET(obj->extra_flags, ITEM_OFF))
      strcat(otext, "It is switched off.\n\r");
      else
      strcat(otext, "It is switched on.\n\r");

      sprintf(buf, "It has about %d minutes of batteries left.\n\r", obj->value[0] / 12);
      strcat(otext, buf);
    }
    if (obj->item_type == ITEM_BLOODCRYSTAL) {
      sprintf(buf, "`rBlood`x\n\rRegular: %d  Virgin: %d  Faeborn: %d  Angelborn: %d  Demonborn: %d  Demigod: %d\n\r", obj->level, obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4]);
      strcat(otext, buf);
    }
    if (obj->item_type == ITEM_UMBRELLA) {
      if (IS_SET(obj->extra_flags, ITEM_OFF))
      strcat(otext, "It is closed.\n\r");
      else
      strcat(otext, "It is open.\n\r");
    }
    if (IS_OBJ_STAT(obj, ITEM_SHROUD)) {
      sprintf(buf, "This item is a nightmare charm.\n\r");
      strcat(otext, buf);
    }
    if (obj->pIndexData->vnum == ITEM_GASMASK) {
      strcat(otext, "This is a gas mask.\n\r");
    }
    if (obj->item_type == ITEM_CORPSE_NPC && get_skill(ch, SKILL_FORENSICS) > 0) {
      EXTRA_DESCR_DATA *ed;
      bool tastefound = FALSE;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+forensic", ed->keyword)) {
          strcat(otext, ed->description);
          tastefound = TRUE;
        }
      }
      if (tastefound == FALSE) {
        for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
          if (is_name("+forensic", ed->keyword)) {
            strcat(otext, ed->description);
            tastefound = TRUE;
          }
        }
      }
    }
    if (obj->faction == -1) {
      sprintf(buf, "It has the symbol of Haven Sheriff's Department on it.\n\r");
      strcat(otext, buf);
    }
    else if (obj->faction == -2 && get_skill(ch, SKILL_DEMONOLOGY) >= 1) {
      sprintf(
      buf, "It is configured to act as a mystical key to the goblin market.\n\r");
      strcat(otext, buf);
    }
    else if (obj->faction > -1 && clan_lookup(obj->faction) != NULL && safe_strlen(clan_lookup(obj->faction)->symbol) > 3) {
      sprintf(buf, "It has a symbol of %s`x.\n\r", lowercase_clause(clan_lookup(obj->faction)->symbol));
      strcat(otext, buf);
    }

    pdesc = get_extra_descr_obj("all", obj->extra_descr, obj);
    if (pdesc != NULL) {
      strcat(otext, pdesc);
      if (photo == FALSE)
      triggercheck(ch, pdesc);
    }
    else {
      pdesc = get_extra_descr_obj("all", obj->pIndexData->extra_descr, obj);
      if (pdesc != NULL) {
        strcat(otext, pdesc);
        if (photo == FALSE)
        triggercheck(ch, pdesc);
      }
    }
    if (photo == FALSE)
    page_to_char(otext, ch);
    else
    to_photo_message(ch, otext);
  }

  _DOFUN(do_show) {
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg[MSL];

    argument = one_argument_nouncap(argument, arg);

    if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (show_char_location_to_char(victim, ch, arg)) {
      printf_to_char(victim, "%s shows you their %s.\n\r", PERS(ch, victim), arg);
      printf_to_char(ch, "You show %s your %s.\n\r", PERS(victim, ch), arg);
      return;
    }
    if ((obj = get_obj_wear(ch, arg, TRUE)) == NULL) {
      if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }
    }

    act("You show $N $a $p.", ch, obj, victim, TO_CHAR);
    act("$n shows you $a $p.", ch, obj, victim, TO_VICT);

    show_obj_to_char(obj, victim, FALSE, TRUE);
  }

  bool is_nudity(ROOM_INDEX_DATA *room) {
    CHAR_DATA *to;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      to = *it;

      if (IS_NPC(to) || to->desc == NULL)
      continue;

      if (!is_covered(to, COVERS_GROIN))
      return TRUE;

      if (!is_covered(to, COVERS_BREASTS) && to->sex == SEX_FEMALE)
      return TRUE;
    }

    return FALSE;
  }

  bool is_raining(ROOM_INDEX_DATA *room) {

    if (room != NULL && room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        if (prop->weather == PROPWEATHER_RAIN)
        return TRUE;
        if (prop->weather != 0)
        return FALSE;
      }
    }
    if (room == NULL || in_haven(room)) {
      if (raining == 1)
      return TRUE;
    }
    else {
      if (room->area->vnum == HELL_FOREST_VNUM && hell_raining == 1)
      return TRUE;
      if (room->area->vnum == OTHER_FOREST_VNUM && other_raining == 1)
      return TRUE;
      if (room->area->vnum == WILDS_FOREST_VNUM && wilds_raining == 1)
      return TRUE;
      if (room->area->vnum == GODREALM_FOREST_VNUM && godrealm_raining == 1)
      return TRUE;
      if (room->raining == 1)
      return TRUE;
    }

    return FALSE;
  }
  bool is_hailing(ROOM_INDEX_DATA *room) {
    if (room != NULL && room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        if (prop->weather == PROPWEATHER_HAIL)
        return TRUE;
        if (prop->weather != 0)
        return FALSE;
      }
    }

    if (room == NULL || in_haven(room)) {
      if (hailing == 1)
      return TRUE;
    }
    else {
      if (room->area->vnum == HELL_FOREST_VNUM && hell_hailing == 1)
      return TRUE;
      if (room->area->vnum == OTHER_FOREST_VNUM && other_hailing == 1)
      return TRUE;
      if (room->area->vnum == WILDS_FOREST_VNUM && wilds_hailing == 1)
      return TRUE;
      if (room->area->vnum == GODREALM_FOREST_VNUM && godrealm_hailing == 1)
      return TRUE;
      if (room->hailing == 1)
      return TRUE;
    }

    return FALSE;
  }
  bool is_snowing(ROOM_INDEX_DATA *room) {
    if (room != NULL && room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        if (prop->weather == PROPWEATHER_SNOW)
        return TRUE;
        if (prop->weather != 0)
        return FALSE;
      }
    }

    if (room == NULL || in_haven(room)) {
      if (snowing == 1)
      return TRUE;
    }
    else {
      if (room->area->vnum == HELL_FOREST_VNUM && hell_snowing == 1)
      return TRUE;
      if (room->area->vnum == OTHER_FOREST_VNUM && other_snowing == 1)
      return TRUE;
      if (room->area->vnum == WILDS_FOREST_VNUM && wilds_snowing == 1)
      return TRUE;
      if (room->area->vnum == GODREALM_FOREST_VNUM && godrealm_snowing == 1)
      return TRUE;
      if (room->snowing == 1)
      return TRUE;
    }

    return FALSE;
  }

  int mist_level(ROOM_INDEX_DATA *room) {
    if(time_info.local_mist_level > 3 || time_info.local_mist_level < 0)
    time_info.local_mist_level = 0;

    if (room == NULL)
    return time_info.local_mist_level;

    if (room->sector_type == SECT_UNDERWATER)
    return 0;

    int level;

    if (in_haven(room)) {
      if (crisis_mist == 1)
      return 3;
      level = time_info.local_mist_level;
      if (room->y >= 115)
      level = UMAX(level, 2);
      if (room->y <= -45)
      level = UMAX(level, 2);
      if (room->x >= 85)
      level = UMAX(level, 2);
      else if (room->x >= 80)
      level = UMAX(level, 1);
      if (room->x <= -45)
      level = UMAX(level, 2);
      if (room->area->vnum == OUTER_NORTH_FOREST)
      level = UMAX(level, 1);
      if (room->area->vnum == OUTER_SOUTH_FOREST)
      level = UMAX(level, 1);
      if (room->area->vnum == OUTER_WEST_FOREST)
      level = UMAX(level, 1);
    }
    else
    level = room->mist_level;

    if (room->area->vnum == HELL_FOREST_VNUM) {
      if (room->y >= 4000025)
      level = UMAX(level, 2);
    }
    if (room->area->vnum == WILDS_FOREST_VNUM) {
      if (room->y <= 2000007 && room->x >= 2000023)
      level = UMAX(level, 2);
    }
    if (room->area->vnum == GODREALM_FOREST_VNUM) {
      if (room->y <= 3000005)
      level = UMAX(level, 2);
    }
    if (room->area->vnum == OTHER_FOREST_VNUM) {
      if (room->y >= 1000023 && room->x >= 1000023)
      level = UMAX(level, 2);
    }

    return level;
  }

  int temperature(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return time_info.local_temp;

    int temp = 0;

    if (in_haven(room))
    temp = time_info.local_temp;

    if (room->area->vnum == HELL_FOREST_VNUM)
    temp = hell_temperature;
    else if (room->area->vnum == OTHER_FOREST_VNUM)
    temp = other_temperature;
    else if (room->area->vnum == WILDS_FOREST_VNUM)
    temp = wilds_temperature;
    else if (room->area->vnum == GODREALM_FOREST_VNUM)
    temp = godrealm_temperature;
    else {
      if (room->temperature != 0)
      temp = room->temperature;
      else
      temp = time_info.local_temp;
    }

    if (room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        if (prop->tempfrozen != 0)
        return prop->tempfrozen;
        temp += prop->tempshift;
      }
    }

    return temp;
  }

  int cloud_cover(ROOM_INDEX_DATA *room) {
    if (room != NULL && room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        if (prop->weather == PROPWEATHER_CLEAR)
        return 0;
        if (prop->weather != 0)
        return 80;
      }
    }

    if (room == NULL)
    return time_info.local_cover_total;

    if (in_haven(room))
    return time_info.local_cover_total;

    if (room->area->vnum == HELL_FOREST_VNUM)
    return hell_cloud_cover;
    if (room->area->vnum == OTHER_FOREST_VNUM)
    return other_cloud_cover;
    if (room->area->vnum == WILDS_FOREST_VNUM)
    return wilds_cloud_cover;
    if (room->area->vnum == GODREALM_FOREST_VNUM)
    return godrealm_cloud_cover;

    return room->cloud_cover;
  }

  int cloud_density(ROOM_INDEX_DATA *room) {
    if (room != NULL && room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        if (prop->weather == PROPWEATHER_CLEAR)
        return 0;
        if (prop->weather == PROPWEATHER_CLOUDY)
        return 50;
        if (prop->weather != 0)
        return 80;
      }
    }

    if (room == NULL)
    return time_info.local_density_total;

    if (in_haven(room))
    return time_info.local_density_total;

    if (room->area->vnum == HELL_FOREST_VNUM)
    return hell_cloud_density;
    if (room->area->vnum == OTHER_FOREST_VNUM)
    return other_cloud_density;
    if (room->area->vnum == WILDS_FOREST_VNUM)
    return wilds_cloud_density;
    if (room->area->vnum == GODREALM_FOREST_VNUM)
    return godrealm_cloud_density;

    return room->cloud_density;
  }

  _DOFUN(do_whereinvis) {
    if (IS_FLAG(ch->comm, COMM_WHEREVIS)) {
      if (IS_FLAG(ch->comm, COMM_WHEREANON)) {
        REMOVE_FLAG(ch->comm, COMM_WHEREVIS);
        REMOVE_FLAG(ch->comm, COMM_WHEREANON);
        send_to_char("`cYou are now invisible on the where list`x.\n\r", ch);
      }
      else {
        SET_FLAG(ch->comm, COMM_WHEREANON);
        send_to_char("`cYou are now anonymous on the where list`x.\n\r", ch);
      }
    }
    else {
      SET_FLAG(ch->comm, COMM_WHEREVIS);
      send_to_char("`cYou are now visible on the where list`x.\n\r", ch);
    }

    return;
  }

  _DOFUN(do_whoinvis) {
    if (IS_FLAG(ch->comm, COMM_WHOINVIS)) {
      REMOVE_FLAG(ch->comm, COMM_WHOINVIS);
      send_to_char("You are now visible on the wholist.\n\r", ch);
      return;
    }
    else {
      SET_FLAG(ch->comm, COMM_WHOINVIS);
      send_to_char("You are now invisible on the wholist.\n\r", ch);
      return;
    }
  }
  char *const sun_phases[] = {
    "`Dnight`x", "`dbefore `ydawn`x", "`Wd`Ya`yw`Yn`x",
    "`Ym`Wo`Yr`Wn`Yi`Wn`Yg`x", "`Ynoon`x", "`yafternoon`x",
    "`rd`yu`rs`yk`x", "`Dafter `rdusk`x"
  };

#define TIME_DAWN 0
#define TIME_NOON 1
#define TIME_DUSK 2

  int sunminute(int month, int pointer) {
    switch (month) {
    case 1:
      if (pointer == TIME_DAWN)
      return 18;
      else if (pointer == TIME_NOON)
      return 35;
      else if (pointer == TIME_DUSK)
      return 53;
      break;
    case 2:
      if (pointer == TIME_DAWN)
      return 51;
      else if (pointer == TIME_NOON)
      return 40;
      else if (pointer == TIME_DUSK)
      return 30;
      break;
    case 3:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 34;
      else if (pointer == TIME_DUSK)
      return 3;
      break;
    case 4:
      if (pointer == TIME_DAWN)
      return 17;
      else if (pointer == TIME_NOON)
      return 26;
      else if (pointer == TIME_DUSK)
      return 35;
      break;
    case 5:
      if (pointer == TIME_DAWN)
      return 38;
      else if (pointer == TIME_NOON)
      return 22;
      else if (pointer == TIME_DUSK)
      return 6;
      break;
    case 6:
      if (pointer == TIME_DAWN)
      return 24;
      else if (pointer == TIME_NOON)
      return 26;
      else if (pointer == TIME_DUSK)
      return 29;
      break;
    case 7:
      if (pointer == TIME_DAWN)
      return 37;
      else if (pointer == TIME_NOON)
      return 32;
      else if (pointer == TIME_DUSK)
      return 26;
      break;
    case 8:
      if (pointer == TIME_DAWN)
      return 6;
      else if (pointer == TIME_NOON)
      return 30;
      else if (pointer == TIME_DUSK)
      return 53;
      break;
    case 9:
      if (pointer == TIME_DAWN)
      return 36;
      else if (pointer == TIME_NOON)
      return 21;
      else if (pointer == TIME_DUSK)
      return 4;
      break;
    case 10:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 11;
      else if (pointer == TIME_DUSK)
      return 15;
      break;
    case 11:
      if (pointer == TIME_DAWN)
      return 43;
      else if (pointer == TIME_NOON)
      return 10;
      else if (pointer == TIME_DUSK)
      return 37;
      break;
    case 12:
      if (pointer == TIME_DAWN)
      return 13;
      else if (pointer == TIME_NOON)
      return 21;
      else if (pointer == TIME_DUSK)
      return 29;
      break;
    }
    return 0;
  }

  int sunhour(int month, int pointer) {
    switch (month) {
    case 1:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 11;
      else if (pointer == TIME_DUSK)
      return 16;
      break;
    case 2:
      if (pointer == TIME_DAWN)
      return 6;
      else if (pointer == TIME_NOON)
      return 11;
      else if (pointer == TIME_DUSK)
      return 17;
      break;
    case 3:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 19;
      break;
    case 4:
      if (pointer == TIME_DAWN)
      return 6;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 19;
      break;
    case 5:
      if (pointer == TIME_DAWN)
      return 5;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 20;
      break;
    case 6:
      if (pointer == TIME_DAWN)
      return 5;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 20;
      break;
    case 7:
      if (pointer == TIME_DAWN)
      return 5;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 20;
      break;
    case 8:
      if (pointer == TIME_DAWN)
      return 6;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 19;
      break;
    case 9:
      if (pointer == TIME_DAWN)
      return 6;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 19;
      break;
    case 10:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 12;
      else if (pointer == TIME_DUSK)
      return 18;
      break;
    case 11:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 11;
      else if (pointer == TIME_DUSK)
      return 18;
      break;
    case 12:
      if (pointer == TIME_DAWN)
      return 7;
      else if (pointer == TIME_NOON)
      return 11;
      else if (pointer == TIME_DUSK)
      return 17;
      break;
    }
    return 5;
  }

  int sunphase(ROOM_INDEX_DATA *room) {
    int month;
    int hour;
    int minute;

    hour = get_hour(room);
    month = get_month() + 1;
    minute = get_minute();

    if (room != NULL && room->area->vnum == WILDS_FOREST_VNUM)
    month += 5;
    else if (room != NULL && room->area->vnum == GODREALM_FOREST_VNUM)
    month -= 2;
    else if (room != NULL && room->area->vnum == OTHER_FOREST_VNUM)
    month += 1;

    if (month <= 0)
    month += 12;
    if (month > 12)
    month -= 12;

    if (hour < sunhour(month, TIME_DAWN) - 1 || (hour == sunhour(month, TIME_DAWN) - 1 && minute < sunminute(month, TIME_DAWN)) || hour > sunhour(month, TIME_DUSK) + 1 || (hour == sunhour(month, TIME_DUSK) + 1 && minute > sunminute(month, TIME_DUSK)))
    return 0;
    else if (hour < sunhour(month, TIME_DAWN) || (hour == sunhour(month, TIME_DAWN) && minute < sunminute(month, TIME_DAWN)))
    return 1;
    else if (hour < sunhour(month, TIME_DAWN) + 1 || (hour == sunhour(month, TIME_DAWN) + 1 && minute < sunminute(month, TIME_DAWN)))
    return 2;
    else if (hour < sunhour(month, TIME_NOON) || (hour == sunhour(month, TIME_NOON) && minute < sunminute(month, TIME_NOON)))
    return 3;
    else if (hour < sunhour(month, TIME_NOON) + 1 || (hour == sunhour(month, TIME_NOON) && minute < sunminute(month, TIME_NOON)))
    return 4;
    else if (hour < sunhour(month, TIME_DUSK) - 1 || (hour == sunhour(month, TIME_DUSK) - 1 && minute < sunminute(month, TIME_DUSK)))
    return 5;
    else if (hour < sunhour(month, TIME_DUSK) || (hour == sunhour(month, TIME_DUSK) && minute < sunminute(month, TIME_DUSK)))
    return 6;
    else if (hour < sunhour(month, TIME_DUSK) + 1 || (hour == sunhour(month, TIME_DUSK) + 1 && minute < sunminute(month, TIME_DUSK)))
    return 7;

    return 0;
  }

  int sunphase_delayed(ROOM_INDEX_DATA *room) {
    int month;
    int hour;
    int minute;

    hour = get_last_hour(room);
    month = get_month() + 1;
    minute = get_minute();

    if (room != NULL && room->area->vnum == WILDS_FOREST_VNUM)
    month += 5;
    else if (room != NULL && room->area->vnum == GODREALM_FOREST_VNUM)
    month -= 2;
    else if (room != NULL && room->area->vnum == OTHER_FOREST_VNUM)
    month += 1;

    if (month <= 0)
    month += 12;
    if (month > 12)
    month -= 12;

    if (hour < sunhour(month, TIME_DAWN) - 1 || (hour == sunhour(month, TIME_DAWN) - 1 && minute < sunminute(month, TIME_DAWN)) || hour > sunhour(month, TIME_DUSK) + 1 || (hour == sunhour(month, TIME_DUSK) + 1 && minute > sunminute(month, TIME_DUSK)))
    return 0;
    else if (hour < sunhour(month, TIME_DAWN) || (hour == sunhour(month, TIME_DAWN) && minute < sunminute(month, TIME_DAWN)))
    return 1;
    else if (hour < sunhour(month, TIME_DAWN) + 1 || (hour == sunhour(month, TIME_DAWN) + 1 && minute < sunminute(month, TIME_DAWN)))
    return 2;
    else if (hour < sunhour(month, TIME_NOON) || (hour == sunhour(month, TIME_NOON) && minute < sunminute(month, TIME_NOON)))
    return 3;
    else if (hour < sunhour(month, TIME_NOON) + 1 || (hour == sunhour(month, TIME_NOON) && minute < sunminute(month, TIME_NOON)))
    return 4;
    else if (hour < sunhour(month, TIME_DUSK) - 1 || (hour == sunhour(month, TIME_DUSK) - 1 && minute < sunminute(month, TIME_DUSK)))
    return 5;
    else if (hour < sunhour(month, TIME_DUSK) || (hour == sunhour(month, TIME_DUSK) && minute < sunminute(month, TIME_DUSK)))
    return 6;
    else if (hour < sunhour(month, TIME_DUSK) + 1 || (hour == sunhour(month, TIME_DUSK) + 1 && minute < sunminute(month, TIME_DUSK)))
    return 7;

    return 0;
  }

  char *displaysun_room(ROOM_INDEX_DATA *room) {
    char color[MSL];
    char buf[MSL];
    char page[MSL];

    strcpy(page, "");
    strcpy(buf, "");

    int phase = sunphase(room);
    int cel = temperature(room) - 32;
    cel *= 5;
    cel /= 9;
    int indoortemp = temperature(room);

    if (indoortemp > 68)
    indoortemp -= 10;
    else if (indoortemp < 53)
    indoortemp = UMIN(53, indoortemp + 35);

    int thing = indoortemp % 5;
    indoortemp -= thing;

    int indoorcel = indoortemp - 32;
    indoorcel *= 5;
    indoorcel /= 9;

    int tempcol = temperature(room);
    // color picker - Discordance
    if (tempcol > 100) {
      strcpy(color, "`R");
    }
    else if (tempcol > 95) {
      strcpy(color, "`r");
    }
    else if (tempcol > 90) {
      strcpy(color, "`M");
    }
    else if (tempcol > 85) {
      strcpy(color, "`m");
    }
    else if (tempcol > 80) {
      strcpy(color, "`Y");
    }
    else if (tempcol > 75) {
      strcpy(color, "`y");
    }
    else if (tempcol > 70) {
      strcpy(color, "`G");
    }
    else if (tempcol > 65) {
      strcpy(color, "`g");
    }
    else if (tempcol > 60) {
      strcpy(color, "`C");
    }
    else if (tempcol > 50) {
      strcpy(color, "`c");
    }
    else if (tempcol > 40) {
      strcpy(color, "`B");
    }
    else if (tempcol > 30) {
      strcpy(color, "`b");
    }
    else if (tempcol > 20) {
      strcpy(color, "`W");
    }
    else if (tempcol > 10) {
      strcpy(color, "`d");
    }
    else {
      strcpy(color, "`D");
    }

    if (!can_see_outside(room)) {
      sprintf(buf, "It is ");
      strcat(page, buf);
      sprintf(buf, "about %s%d`xF(%s%d`xC) degrees.  ", color, indoortemp, color, indoorcel);
      strcat(page, buf);
    }
    else {
      sprintf(buf, "It is %s, ", sun_phases[phase]);
      strcat(page, buf);
      sprintf(buf, "about %s%d`xF(%s%d`xC) degrees, ", color, temperature(room), color, cel);
      strcat(page, buf);
      if (cloud_density(room) == 0) {
        sprintf(buf, "and there are clear skies.  ");
      }
      else if (cloud_density(room) < 20) {
        if (cloud_cover(room) < 30) {
          sprintf(buf, "and there are a few wispy white clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(room) < 75) {
          sprintf(buf, "and the sky is partly covered by wispy white clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by wispy white clouds.  ");
          strcat(page, buf);
        }
      }
      else if (cloud_density(room) < 40) {
        if (cloud_cover(room) < 30) {
          sprintf(buf, "and there are a few thin white clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(room) < 75) {
          sprintf(buf, "and the sky is partly covered by thin white clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by thin white clouds.  ");
          strcat(page, buf);
        }
      }
      else if (cloud_density(room) < 60) {
        if (cloud_cover(room) < 30) {
          sprintf(buf, "and there are a few grey clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(room) < 75) {
          sprintf(buf, "and the sky is partly covered by grey clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by grey clouds.  ");
          strcat(page, buf);
        }
      }
      else if (cloud_density(room) < 80) {
        if (cloud_cover(room) < 30) {
          sprintf(buf, "and there are a few dark grey clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(room) < 75) {
          sprintf(buf, "and the sky is partly covered by dark grey clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by dark grey clouds.  ");
          strcat(page, buf);
        }
      }
      else {
        if (cloud_cover(room) < 30) {
          sprintf(buf, "and there are a few dark grey stormclouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(room) < 75) {
          sprintf(buf, "and the sky is partly covered by dark grey stormclouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by dark grey stormclouds.  ");
          strcat(page, buf);
        }
      }

      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        if (is_raining(room)) {
          sprintf(buf, "It's `Braining`x outside.  ");
          strcat(page, buf);
        }
        if (is_hailing(room)) {
          sprintf(buf, "It's `Chailing`x outside.  ");
          strcat(page, buf);
        }
        if (is_snowing(room)) {
          sprintf(buf, "It's `Wsnowing`x outside.  ");
          strcat(page, buf);
        }
      }
      else {
        if (is_raining(room)) {
          sprintf(buf, "It's `Braining.`x  ");
          strcat(page, buf);
        }
        if (is_hailing(room)) {
          sprintf(buf, "It's `Chailing.`x  ");
          strcat(page, buf);
        }
        if (is_snowing(room)) {
          sprintf(buf, "It's `Wsnowing.`x  ");
          strcat(page, buf);
        }
      }

      if (mist_level(room) >= 3) {
        sprintf(buf, "`WThe area is wreathed in mist.`x  ");
        strcat(page, buf);
      }
      else if (mist_level(room) == 2) {
        sprintf(buf, "Waist high `Wmist`x flows through the area.`x  ");
        strcat(page, buf);
      }
      else if (mist_level(room) == 1) {
        sprintf(buf, "Ankle high `Wmist`x flows through the area.`x  ");
        strcat(page, buf);
      }
    }

    if (can_see_outside(room)) {
      if ((phase == 0 || phase == 1 || phase == 7)) {
        tm *ptm;
        time_t east_time;

        east_time = current_time;
        ptm = gmtime(&east_time);

        sprintf(buf, "There is a %s moon.  ", moon_real[moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL)]);
        strcat(page, buf);
      }
    }

    return str_dup(page);
  }

  char *displaysun(CHAR_DATA *ch) {
    char color[MSL];
    char buf[MSL];
    char page[MSL];

    strcpy(page, "");
    strcpy(buf, "");

    int phase = sunphase(ch->in_room);
    int cel = temperature(ch->in_room) - 32;
    cel *= 5;
    cel /= 9;
    int indoortemp = temperature(ch->in_room);

    if (indoortemp > 68)
    indoortemp -= 10;
    else if (indoortemp < 53)
    indoortemp = UMIN(53, indoortemp + 35);

    int thing = indoortemp % 5;
    indoortemp -= thing;
    int indoorcel = indoortemp - 32;
    indoorcel *= 5;
    indoorcel /= 9;

    int tempcol = temperature(ch->in_room);
    // color picker - Discordance
    if (tempcol > 100) {
      strcpy(color, "`R");
    }
    else if (tempcol > 95) {
      strcpy(color, "`r");
    }
    else if (tempcol > 90) {
      strcpy(color, "`M");
    }
    else if (tempcol > 85) {
      strcpy(color, "`m");
    }
    else if (tempcol > 80) {
      strcpy(color, "`Y");
    }
    else if (tempcol > 75) {
      strcpy(color, "`y");
    }
    else if (tempcol > 70) {
      strcpy(color, "`G");
    }
    else if (tempcol > 65) {
      strcpy(color, "`g");
    }
    else if (tempcol > 60) {
      strcpy(color, "`C");
    }
    else if (tempcol > 50) {
      strcpy(color, "`c");
    }
    else if (tempcol > 40) {
      strcpy(color, "`B");
    }
    else if (tempcol > 30) {
      strcpy(color, "`b");
    }
    else if (tempcol > 20) {
      strcpy(color, "`W");
    }
    else if (tempcol > 10) {
      strcpy(color, "`d");
    }
    else {
      strcpy(color, "`D");
    }

    if (!can_see_outside(ch->in_room)) {
      if (is_vampire(ch) || IS_AFFECTED(ch, AFF_SUNBLESS) || get_skill(ch, SKILL_DAYASPECT) > 0 || get_skill(ch, SKILL_SOLARBLESS) > 0) {
        sprintf(buf, "It is %s, and ", sun_phases[phase]);
      }
      else {
        sprintf(buf, "It is ");
      }
      strcat(page, buf);
      sprintf(buf, "about %s%d`xF(%s%d`xC) degrees.  ", color, indoortemp, color, indoorcel);
      strcat(page, buf);
      if (get_roomz(ch->in_room) >= 0) {
        if (is_raining(ch->in_room)) {
          sprintf(buf, "It's `Braining`x outside.  ");
          strcat(page, buf);
        }
        if (is_hailing(ch->in_room)) {
          sprintf(buf, "It's `Chailing`x outside.  ");
          strcat(page, buf);
        }
      }
    }
    else {
      sprintf(buf, "It is %s, ", sun_phases[phase]);
      strcat(page, buf);
      sprintf(buf, "about %s%d`xF(%s%d`xC) degrees, ", color, temperature(ch->in_room), color, cel);
      strcat(page, buf);
      if (cloud_density(ch->in_room) == 0) {
        sprintf(buf, "and there are clear skies.  ");
      }
      else if (cloud_density(ch->in_room) < 20) {
        if (cloud_cover(ch->in_room) < 30) {
          sprintf(buf, "and there are a few wispy white clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(ch->in_room) < 75) {
          sprintf(buf, "and the sky is partly covered by wispy white clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by wispy white clouds.  ");
          strcat(page, buf);
        }
      }
      else if (cloud_density(ch->in_room) < 40) {
        if (cloud_cover(ch->in_room) < 30) {
          sprintf(buf, "and there are a few thin white clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(ch->in_room) < 75) {
          sprintf(buf, "and the sky is partly covered by thin white clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by thin white clouds.  ");
          strcat(page, buf);
        }
      }
      else if (cloud_density(ch->in_room) < 60) {
        if (cloud_cover(ch->in_room) < 30) {
          sprintf(buf, "and there are a few grey clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(ch->in_room) < 75) {
          sprintf(buf, "and the sky is partly covered by grey clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by grey clouds.  ");
          strcat(page, buf);
        }
      }
      else if (cloud_density(ch->in_room) < 80) {
        if (cloud_cover(ch->in_room) < 30) {
          sprintf(buf, "and there are a few dark grey clouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(ch->in_room) < 75) {
          sprintf(buf, "and the sky is partly covered by dark grey clouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by dark grey clouds.  ");
          strcat(page, buf);
        }
      }
      else {
        if (cloud_cover(ch->in_room) < 30) {
          sprintf(buf, "and there are a few dark grey stormclouds in the sky.  ");
          strcat(page, buf);
        }
        else if (cloud_cover(ch->in_room) < 75) {
          sprintf(buf, "and the sky is partly covered by dark grey stormclouds.  ");
          strcat(page, buf);
        }
        else {
          sprintf(buf, "and the sky is covered by dark grey stormclouds.  ");
          strcat(page, buf);
        }
      }

      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        if (is_raining(ch->in_room)) {
          sprintf(buf, "It's `Braining`x outside.  ");
          strcat(page, buf);
        }
        if (is_hailing(ch->in_room)) {
          sprintf(buf, "It's `Chailing`x outside.  ");
          strcat(page, buf);
        }
        if (is_snowing(ch->in_room)) {
          sprintf(buf, "It's `Wsnowing`x outside.  ");
          strcat(page, buf);
        }
      }
      else {
        if (is_raining(ch->in_room)) {
          sprintf(buf, "It's `Braining.`x  ");
          strcat(page, buf);
        }
        if (is_hailing(ch->in_room)) {
          sprintf(buf, "It's `Chailing.`x  ");
          strcat(page, buf);
        }
        if (is_snowing(ch->in_room)) {
          sprintf(buf, "It's `Wsnowing.`x  ");
          strcat(page, buf);
        }
      }

      if (mist_level(ch->in_room) >= 3) {
        sprintf(buf, "`WThe area is wreathed in mist.`x  ");
        strcat(page, buf);
      }
      else if (mist_level(ch->in_room) == 2) {
        sprintf(buf, "Waist high `Wmist`x flows through the area.`x  ");
        strcat(page, buf);
      }
      else if (mist_level(ch->in_room) == 1) {
        sprintf(buf, "Ankle high `Wmist`x flows through the area.`x  ");
        strcat(page, buf);
      }
    }

    if (can_see_outside(ch->in_room) || is_werewolf(ch) || IS_AFFECTED(ch, AFF_MOONBLESS) || get_skill(ch, SKILL_NIGHTASPECT) > 0 || get_skill(ch, SKILL_LUNARBLESS) > 0) {
      if ((phase == 0 || phase == 1 || phase == 7)) {
        tm *ptm;
        time_t east_time;

        east_time = current_time;
        ptm = gmtime(&east_time);

        sprintf(
        buf, "There is a %s moon.  ", moon_real[moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, ch)]);
        strcat(page, buf);
      }
    }

    return str_dup(page);
  }

  _DOFUN(do_weather) {
    char page[MSL];

    sprintf(page, "%s\n\r", displaysun(ch));
    page_to_char(wrap_string(page, get_wordwrap(ch)), ch);

    return;
  }

  int get_minute(void) {
    tm *ptm;
    time_t east_time;
    east_time = current_time;
    ptm = gmtime(&east_time);
    return ptm->tm_min;
  }

  // Commented out what appears to be subtracting an hour, presumably for daylight
  // savings - Discordance
  int get_hour(ROOM_INDEX_DATA *room) {
    int hour;

    tm *ptm;
    time_t east_time;

    //    east_time = current_time + 7200;
    east_time = current_time - 14400;
    east_time -= 3600;
    // Uncomment above for daylight savings
    ptm = gmtime(&east_time);

    hour = ptm->tm_hour;
    if (room != NULL && !in_haven(room) && room->timezone != 0)
    hour += room->timezone;

    if (room != NULL && room->area->vnum >= 19 && room->area->vnum <= 28) {
      PROP_TYPE *prop;
      if ((prop = prop_from_room(room)) != NULL) {
        hour += prop->timeshift;
        if (prop->timefrozen != 0)
        hour = prop->timefrozen;
      }
    }

    if (hour > 23)
    hour -= 24;
    if (hour < 0)
    hour += 24;

    return hour;
  }
  int get_last_hour(ROOM_INDEX_DATA *room) {
    int hour;

    tm *ptm;
    time_t east_time;

    //    east_time = current_time + 7200;
    east_time = current_time - 14400;
    east_time -= 3600;
    // Uncomment above for daylight savings
    east_time -= 60;
    ptm = gmtime(&east_time);

    hour = ptm->tm_hour;
    if (room != NULL && !in_haven(room) && room->timezone != 0)
    hour += room->timezone;

    return hour;
  }
  int get_day() {
    int hour;

    tm *ptm;
    time_t east_time;

    //    east_time = current_time + 7200;
    east_time = current_time - 14400;
    east_time -= 3600;
    // Uncomment above for daylight savings
    ptm = gmtime(&east_time);

    hour = ptm->tm_mday;
    return hour;
  }

  int get_weekday() {
    int hour;

    tm *ptm;
    time_t east_time;

    //    east_time = current_time + 7200;
    east_time = current_time - 14400;
    east_time -= 3600;
    // Uncomment above for daylight savings
    ptm = gmtime(&east_time);

    hour = ptm->tm_wday;
    return hour;
  }

  int get_month() {
    int hour;

    tm *ptm;
    time_t east_time;

    //    east_time = current_time + 7200;

    east_time = current_time - 14400;
    east_time -= 3600;
    ptm = gmtime(&east_time);

    hour = ptm->tm_mon;
    return hour;
  }

  int get_last_day() {
    int hour;

    tm *ptm;
    time_t east_time;

    //    east_time = current_time + 7200;
    east_time = current_time - 14400;
    east_time -= 3600;
    east_time -= 60;
    ptm = gmtime(&east_time);

    hour = ptm->tm_mday;
    return hour;
  }

  bool has_xray(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_HOLYLIGHT))
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_ageless) {

    if ((IS_NPC(ch) || get_skill(ch, SKILL_AGELESS) <= 0)) {
      send_to_char("You have to buy the ageless stat first.\n\r", ch);
      return;
    }

    if (!is_number(argument)) {
      send_to_char("Syntax: Ageless (number)\n\r", ch);
      return;
    }

    if (ch->pcdata->apparant_age > 0 && ch->played / 3600 > 5 && ch->in_room->vnum != ROOM_INDEX_GENESIS) {
      send_to_char("You already set that.\n\r", ch);
      return;
    }
    if (atoi(argument) < min_aage(ch)) {
      printf_to_char(ch, "You can't seem younger than %d.\n\r", min_aage(ch));
      return;
    }
    if (atoi(argument) > max_aage(ch)) {
      printf_to_char(ch, "You can't seem older than %d.\n\r", max_aage(ch));
      return;
    }

    ch->pcdata->apparant_age = atoi(argument);
    if (atoi(argument) == 0)
    send_to_char("You now appear to be your regular age.\n\r", ch);
    else
    printf_to_char(ch, "You now appear to be %d years old.\n\r", ch->pcdata->apparant_age);
  }

  _DOFUN(do_fame) {
    char arg1[MSL];
    char original[MSL];
    strcpy(original, argument);
    argument = one_argument_nouncap(argument, arg1);

    free_string(ch->pcdata->fame);
    ch->pcdata->fame = str_dup(original);
    printf_to_char(ch, "Your character is now famous for; %s\n\r", argument);
  }

  _DOFUN(do_art) {
    char arg1[MSL];
    int num;
    char buf[MSL];
    argument = one_argument_nouncap(argument, arg1);

    num = atoi(arg1);

    if (num < 1 || num > 3) {
      send_to_char("Syntax: art (1-3) (area of expertise)\n\r", ch);
      return;
    }

    if (num == 1) {
      free_string(ch->pcdata->art1);
      ch->pcdata->art1 = str_dup(argument);
      printf_to_char(ch, "Your character's first artistic talent is now; %s\n\r", argument);
      sprintf(buf, "$n's first artistic talent is now; %s", argument);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    if (num == 2) {
      free_string(ch->pcdata->art2);
      ch->pcdata->art2 = str_dup(argument);
      printf_to_char(ch, "Your character's second artistic talent is now; %s\n\r", argument);
      sprintf(buf, "$n's second artistic talent is now; %s", argument);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    if (num == 3) {
      free_string(ch->pcdata->art3);
      ch->pcdata->art3 = str_dup(argument);
      printf_to_char(ch, "Your character's third artistic talent is now; %s\n\r", argument);
      sprintf(buf, "$n's third artistic talent is now; %s", argument);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    act("$n changes their artistic focus.", ch, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_join) {
    char buf[MSL];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    CHAR_DATA *to;

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (higher_power(ch) && power_bound(ch)) {
      send_to_char("You cannot leave the binding circle.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "")) {
      send_to_char("You leave your place.\n\r", ch);
      if (is_dreaming(ch) && ch->pcdata->dream_room != 0) {
        sprintf(buf, "%s leaves their place", dream_name(ch));
        if (!goddreamer(ch))
        dreamscape_message(ch, ch->pcdata->dream_room, buf);
        free_string(ch->pcdata->dreamplace);
        ch->pcdata->dreamplace = str_dup("");
        return;
      }
      if (room_visible(ch)) {
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end(); ++it) {
          to = *it;

          if (to == NULL)
          continue;

          if (to->in_room == NULL)
          continue;

          if (ch == to)
          continue;
          if (IS_NPC(to))
          continue;

          if(!str_cmp(to->pcdata->place, ch->pcdata->place))
          printf_to_char(to, "%s leaves %s place(`DLeft your location`x).\n\r", PERS(ch, to), (ch->sex == SEX_MALE) ? "his" : "her");
          else
          printf_to_char(to, "%s leaves %s place.\n\r", PERS(ch, to), (ch->sex == SEX_MALE) ? "his" : "her");


        }
      }

      free_string(ch->pcdata->place);
      ch->pcdata->place = str_dup("");
      // DISABLED - Check to see if character is leaving a computer - Discordance
      /*
      if (access_internet(ch) == FALSE)
      {
      if (ch->pcdata->chatroom > 0)
      do_function(ch, &do_chatroom, "logoff");
      }
      */
      return;
    }
    if (is_dreaming(ch) && ch->pcdata->dream_room != 0) {
      ROOM_INDEX_DATA *dreamroom = get_room_index(ch->pcdata->dream_room);
      if (dreamroom == NULL)
      return;

      EXTRA_DESCR_DATA *ed;
      for (ed = dreamroom->places; ed; ed = ed->next) {
        if (!IS_NPC(ch) && is_name(argument, ed->keyword)) {
          free_string(ch->pcdata->dreamplace);
          ch->pcdata->dreamplace = str_dup(ed->keyword);
          printf_to_char(ch, "You join %s.\n\r", ed->keyword);
          sprintf(buf, "%s joins %s.\n\r", dream_name(ch), ed->keyword);
          if (!goddreamer(ch))
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
          send_to_char(get_extra_descr(ed->keyword, dreamroom->places), ch);
          return;
        }
      }
      free_string(ch->pcdata->dreamplace);
      ch->pcdata->dreamplace = str_dup(argument);
      printf_to_char(ch, "You move to %s.\n\r", ch->pcdata->dreamplace);
      sprintf(buf, "%s moves to %s.\n\r", dream_name(ch), ch->pcdata->dreamplace);
      if (!goddreamer(ch))
      dreamscape_message(ch, ch->pcdata->dream_room, buf);
      return;
    }
    if (!str_cmp(argument, "bar")) {
      if (ch->in_room->x >= 0 && ch->in_room->x <= 71 && ch->in_room->y >= 0 && ch->in_room->y <= 71) {
        if ((get_age(ch) < 21 && get_true_age(ch) < 21) || (get_age(ch) < 21 && get_true_age(ch) > 40)) {
          send_to_char("You're not allowed at the bar.\n\r", ch);
          return;
        }
      }
    }
    EXTRA_DESCR_DATA *ed;
    for (ed = ch->in_room->places; ed; ed = ed->next) {
      if (!IS_NPC(ch) && is_name(argument, ed->keyword)) {
        free_string(ch->pcdata->place);
        ch->pcdata->place = str_dup(ed->keyword);
        printf_to_char(ch, "You join %s.\n\r", ed->keyword);
        sprintf(buf, "$n joins %s.\n\r", ed->keyword);
        if (is_name("shower", ed->keyword)) {
          if (!IS_SET(ch->in_room->room_flags, ROOM_BATHROOM) && (prop_from_room(ch->in_room) == NULL)) {
            send_to_char("The water won't turn on.\n\r`x", ch);
          }
        }
        if (room_visible(ch)) {
          for (CharList::iterator it = ch->in_room->people->begin();
          it != ch->in_room->people->end(); ++it) {
            to = *it;

            if (to == NULL)
            continue;

            if (to->in_room == NULL)
            continue;

            if (ch == to)
            continue;
            if (IS_NPC(to))
            continue;

            if(!str_cmp(to->pcdata->place, ch->pcdata->place))
            printf_to_char(to, "%s joins %s.(`WJoined your location`x).\n\r", PERS(ch, to), ch->pcdata->place);
            else
            printf_to_char(to, "%s joins %s.\n\r", PERS(ch, to), ch->pcdata->place);
          }
        }

        send_to_char(get_extra_descr(ed->keyword, ch->in_room->places), ch);
        // DISABLED - Check to see if character is leaving a computer -
        // Discordance
        /*
        if (access_internet(ch) == FALSE)
        {
        if (ch->pcdata->chatroom > 0)
        do_function(ch, &do_chatroom, "logoff");
        }
        */

        return;
      }
    }
    if (gravesite(ch->in_room)) {
      if (gravename(ch, argument)) {
        free_string(ch->pcdata->place);
        ch->pcdata->place = str_dup(argument);
        printf_to_char(ch, "You move to stand before the grave of %s.\n\r", ch->pcdata->place);
        sprintf(buf, "$n moves to the grave of %s.\n\r", ch->pcdata->place);
        act(buf, ch, NULL, NULL, TO_ROOM);
        gravevisit(ch, argument);
        return;
      }
    }

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (ch == victim)
      continue;
      if (IS_NPC(victim))
      continue;

      if (!str_cmp(victim->pcdata->place, argument)) {
        free_string(ch->pcdata->place);
        ch->pcdata->place = str_dup(victim->pcdata->place);
        printf_to_char(ch, "You join %s.\n\r", victim->pcdata->place);
        sprintf(buf, "$n joins %s.\n\r", victim->pcdata->place);
        act(buf, ch, NULL, NULL, TO_ROOM);
        return;
      }
    }
    if (!crowded_room(ch->in_room) && (locked_room(ch->in_room, ch))) { 
      // this keeps dynamic places consistent with the rooms
      // they're in, except in crowds which need places and could
      // occur in rooms without descriptions or a reasonable amount
      // of places in the description
      char no_color[MSL];
      remove_color(no_color, ch->in_room->description);
      if (strcasestr(no_color, argument) == NULL && !is_name(argument, no_color)) {
        send_to_char("You fail to summon that from thin air.\n\r", ch);
        return;
      }
    }

    if (is_name("umbrella", argument)) {
      bool hasUmbrella = FALSE;
      obj = get_eq_char(ch, WEAR_HOLD);

      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF)) {
        hasUmbrella = TRUE;
      }

      obj = get_eq_char(ch, WEAR_HOLD_2);

      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF)) {
        hasUmbrella = TRUE;
      }

      if (has_place(ch->in_room, "umbrella")) {
        hasUmbrella = TRUE;
      }

      if (hasUmbrella == FALSE) {
        send_to_char("Your umbrella must be held and open.\n\r", ch);
        return;
      }
    }
    free_string(ch->pcdata->place);
    ch->pcdata->place = str_dup(argument);
    printf_to_char(ch, "You move to %s.\n\r", ch->pcdata->place);
    if (room_visible(ch)) {
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (ch == to)
        continue;
        if (IS_NPC(to))
        continue;

        if(!str_cmp(to->pcdata->place, ch->pcdata->place))
        printf_to_char(to, "%s moves to %s.(`WJoined your location`x).\n\r", PERS(ch, to), ch->pcdata->place);
        else
        printf_to_char(to, "%s moves to %s.\n\r", PERS(ch, to), ch->pcdata->place);
      }
    }

    return;
  }

  char *exit_prefix(CHAR_DATA *ch, ROOM_INDEX_DATA *room, EXIT_DATA *exit) {
    if (room == NULL)
    return "`w";
    if (is_dark(room) && !can_see_dark(ch)) {
      if (is_water(room))
      return "`b";
      else
      return "`D";
    }
    if (is_water(room)) {
      if (is_dark_outside())
      return "`b";
      else
      return "`B";
    }
    if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY || room->sector_type == SECT_PARKING) {
      if (!is_dark_outside() && !is_outside(ch))
      return "`Y";
      else
      return "`W";
    }
    if (room->sector_type == SECT_AIR || room->sector_type == SECT_ATMOSPHERE) {
      if (is_dark_outside())
      return "`D";
      else
      return "`C";
    }
    if (room->sector_type == SECT_TUNNELS)
    return "`D";
    if (room->sector_type == SECT_FOREST || room->sector_type == SECT_PARK || room->sector_type == SECT_CEMETARY) {
      if (is_dark_outside())
      return "`g";
      else
      return "`G";
    }
    if (room->sector_type == SECT_TUNNELS)
    return "`d";
    if (room->sector_type == SECT_BEACH || room->sector_type == SECT_DIRT) {
      if (is_dark_outside())
      return "`y";
      else
      return "`Y";
    }
    if (room->sector_type == SECT_SWAMP)
    return "`y";
    if (room->sector_type == SECT_CAVE)
    return "`D";

    if (IS_SET(room->room_flags, ROOM_INDOORS)) {
      if (is_dark_outside() && is_outside(ch))
      return "`Y";
      else
      return "`w";
    }

    return "`w";
  }

  int exit_order(CHAR_DATA *ch, int i) {
    if (!IS_FLAG(ch->comm, COMM_CARDINAL)) {
      if (i == 0)
      return DIR_NORTHWEST;
      if (i == 1)
      return DIR_NORTH;
      if (i == 2)
      return DIR_NORTHEAST;
      if (i == 3)
      return DIR_UP;
      if (i == 4)
      return DIR_WEST;
      if (i == 5)
      return DIR_SOUTHWEST;
      if (i == 6)
      return DIR_SOUTH;
      if (i == 7)
      return DIR_SOUTHEAST;
      if (i == 8)
      return DIR_EAST;
      if (i == 9)
      return DIR_DOWN;
    }
    else {
      if (i == 0)
      return get_absoldirection(DIR_NORTHWEST, ch->facing);
      if (i == 1)
      return get_absoldirection(DIR_NORTH, ch->facing);
      if (i == 2)
      return get_absoldirection(DIR_NORTHEAST, ch->facing);
      if (i == 3)
      return get_absoldirection(DIR_UP, ch->facing);
      if (i == 4)
      return get_absoldirection(DIR_WEST, ch->facing);
      if (i == 5)
      return get_absoldirection(DIR_SOUTHWEST, ch->facing);
      if (i == 6)
      return get_absoldirection(DIR_SOUTH, ch->facing);
      if (i == 7)
      return get_absoldirection(DIR_SOUTHEAST, ch->facing);
      if (i == 8)
      return get_absoldirection(DIR_EAST, ch->facing);
      if (i == 9)
      return get_absoldirection(DIR_DOWN, ch->facing);
    }

    return i;
  }

  char *empty_exit(CHAR_DATA *ch, int door, int i) {
    if (!IS_FLAG(ch->comm, COMM_CARDINAL)) {
      if (door == DIR_NORTH)
      return "        ";
      if (door == DIR_NORTHEAST)
      return "            ";
      if (door == DIR_EAST)
      return "       ";
      if (door == DIR_SOUTHEAST)
      return "            ";
      if (door == DIR_SOUTH)
      return "        ";
      if (door == DIR_SOUTHWEST)
      return "            ";
      if (door == DIR_WEST)
      return "       ";
      if (door == DIR_NORTHWEST)
      return "            ";
      if (door == DIR_UP)
      return "     ";
      if (door == DIR_DOWN)
      return "       ";
    }
    else {
      if (i == 0)
      return "              ";
      if (i == 1)
      return "          ";
      if (i == 2)
      return "               ";
      if (i == 3)
      return "     ";
      if (i == 4)
      return "       ";
      if (i == 5)
      return "               ";
      if (i == 6)
      return "           ";
      if (i == 7)
      return "                ";
      if (i == 8)
      return "        ";
      if (i == 9)
      return "       ";
    }
    return " ";
  }

  char *get_dirname(CHAR_DATA *ch, int door) {
    if (!IS_FLAG(ch->comm, COMM_CARDINAL))
    return str_dup(dir_name[door][0]);
    else
    return str_dup(reldir_name[get_reldirection(door, ch->facing)][0]);
  }

  _DOFUN(do_exits) {
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *current_room;
    char color[MSL];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door, doortmp = 0;

    if (is_dreaming(ch)) {
      current_room = get_room_index(ch->pcdata->dream_room);
    }
    else {
      current_room = ch->in_room;
    }

    fAuto = !str_cmp(argument, "auto");

    if (!fAuto && !IS_NPC(ch) && ch->pcdata->ci_editing == 2) {
      ch->pcdata->ci_absorb = 1;
      if (ch->pcdata->ci_status == 0 || ch->pcdata->ci_status == ROOM_PUBLIC) {
        send_to_char("This command is for setting private room exits, for public rooms use the quadrant command.\n\r", ch);
        return;
      }
      for (int i = 0; i < MAX_DIR; i++) {
        if (!str_cmp(argument, dir_name[i][0]) || !str_cmp(argument, dir_name[i][1])) {
          ch->pcdata->ci_x = i;
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("`cPossible exits are`g:`c North`g,`c northeast`g,`c east`g,`c southeast`g,`c south`g,`c southwest`g,`c west`g,`c northwest`g,`c up`g,`c down`g.`x\n\r", ch);
      return;
    }

    if (is_dark(current_room) && !can_see_dark(ch))
    return;

    if (!check_blind(ch))
    return;

    if (current_room->vnum == 19000) {
      if (!is_dark_outside() && !is_outside(ch)) {
        strcpy(color, "`Y");
      }
      else {
        strcpy(color, "`W");
      }
      if (trolly_moving == 0)
      printf_to_char(ch, "`g[ %sDisembark`g ]`x\n\r", color);
      else
      send_to_char("`g[`x None `g]`x\n\r", ch);
      return;
    }

    if (fAuto)
    sprintf(buf, "`g[ `x");
    else if (IS_IMMORTAL(ch))
    sprintf(buf, "`cObvious exits from room %d`g:`x\n\r", current_room->vnum);
    else
    sprintf(buf, "`cObvious exits`g:`x\n\r");

    found = FALSE;
    for (int i = 0; i <= 9; i++) {
      door = exit_order(ch, i);
      if (fAuto && i == 4)
      strcat(buf, "`g  ]\n[ `x");
      if (fAuto && i == 0 && !IS_FLAG(ch->comm, COMM_CARDINAL))
      strcat(buf, "       ");
      if (fAuto && i == 3 && !IS_FLAG(ch->comm, COMM_CARDINAL))
      strcat(buf, "        ");
      if (fAuto && i == 0 && IS_FLAG(ch->comm, COMM_CARDINAL))
      strcat(buf, "        ");
      if (fAuto && i == 3 && IS_FLAG(ch->comm, COMM_CARDINAL))
      strcat(buf, "          ");
      if (fAuto && i == 2 && IS_FLAG(ch->comm, COMM_CARDINAL))
      strcat(buf, " ");

      if ((pexit = current_room->exit[door]) != NULL && pexit->u1.to_room != NULL && ((can_see_room(ch, pexit->u1.to_room) && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && ((!IS_SET(pexit->exit_info, EX_HIDDEN) && !IS_AFFECTED(pexit, AFF_XHIDE)) || IS_ADMIN(ch))) || (ch->pcdata->spectre > 0 && ch->pcdata->dream_door == door))) {
        ROOM_INDEX_DATA *displayroom = pexit->u1.to_room;
        if (current_room->area->world != displayroom->area->world && cardinal_room(current_room, door) != NULL)
        displayroom = cardinal_room(current_room, door);
        if (ch->pcdata->spectre > 0 && ch->pcdata->dream_door == door && lobby_number(ch->pcdata->dream_exit) != NULL && get_room_index(lobby_number(ch->pcdata->dream_exit)->entrance) !=
            NULL)
        displayroom =
        get_room_index(lobby_number(ch->pcdata->dream_exit)->entrance);
        found = TRUE;
        strcpy(color, exit_prefix(ch, displayroom, pexit));
        if (fAuto) {
          doortmp = door;

          if (IS_SET(pexit->exit_info, EX_LOCKED) && (IS_SET(pexit->exit_info, EX_CLOSED))) {
            strcat(buf, "`g [`x");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "`g]`x");
          }
          else if (IS_SET(pexit->exit_info, EX_HIDDEN)) {
            strcat(buf, "`g {`x");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "`g}`x");
          }
          else if (IS_SET(pexit->exit_info, EX_CLOSED)) {
            strcat(buf, "`g (`x");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "`g)`x");
          }
          else if (pexit->jump != 0 && can_jump(ch, pexit->jump) && pexit->wall == WALL_NONE) {
            strcat(buf, "`g ^`x");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "`g^`x");
          }
          else if (pexit->climb != 0 && pexit->wall == WALL_NONE) {
            strcat(buf, "  ");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, " ");
          }
          else if (pexit->fall != 0 && pexit->wall == WALL_NONE) {
            strcat(buf, "`g !`x");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "!`x");
          }
          else if (pexit->jump != 0 && pexit->wall == WALL_NONE) {
            strcat(buf, "`r ^`x");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "`r^`x");
          }
          else if (pexit->wallcondition == WALLCOND_HOLE) {
            strcat(buf, "`y {`D");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, "`y}`x");
          }
          else if (pexit->wall == WALL_GLASS) {
            if (IS_SET(pexit->exit_info, EX_CURTAINS)) {
              strcat(buf, "`B {`x");
              strcat(buf, color);
              // strcat (buf, MXP_TAG ("Ex"));
              strcat(buf, get_dirname(ch, doortmp));
              // strcat (buf, MXP_TAG ("/Ex"));
              strcat(buf, "`B}`x");
            }
            else {
              strcat(buf, "`B [");
              strcat(buf, color);
              // strcat (buf, MXP_TAG ("Ex"));
              strcat(buf, get_dirname(ch, doortmp));
              // strcat (buf, MXP_TAG ("/Ex"));
              strcat(buf, "`B]`x");
            }

          }
          else {
            strcat(buf, "  ");
            strcat(buf, color);
            // strcat (buf, MXP_TAG ("Ex"));
            strcat(buf, get_dirname(ch, doortmp));
            // strcat (buf, MXP_TAG ("/Ex"));
            strcat(buf, " ");
          }
        }
        else {
          if (IS_SET(pexit->exit_info, EX_CLOSED)) {
            sprintf(
            buf + safe_strlen(buf), "%-5s - %s", capitalize(get_dirname(ch, door)), room_is_dark(displayroom) ? "Too dark to tell" : "A Closed Door");
          }
          else if (IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
            sprintf(buf + safe_strlen(buf), "%-5s - %s", capitalize(get_dirname(ch, door)), "An infinite expanse of darkness");
          }
          else {
            sprintf(buf + safe_strlen(buf), "%-5s - %s", capitalize(get_dirname(ch, door)), room_is_dark(displayroom) ? "Too dark to tell" : displayroom->name);
          }

          if (IS_IMMORTAL(ch))
          sprintf(buf + safe_strlen(buf), " (room %d)\n\r", pexit->u1.to_room->vnum);
          else
          sprintf(buf + safe_strlen(buf), "\n\r");
        }
      }
      else if (fAuto)
      strcat(buf, empty_exit(ch, door, i));
    }

    if (fAuto)
    strcat(buf, " `g]`x\n\r");

    found = FALSE;
    if (current_room->vnum == trolly_stops[trolly_at] && trolly_moving == 0) {
      if (is_dark_outside() && is_outside(ch)) {
        strcpy(color, "`Y");
      }
      else {
        strcpy(color, "`W");
      }
      if (found == FALSE) {
        strcat(buf, "`g[ `x");
        strcat(buf, color);
        strcat(buf, " Board`x");
        found = TRUE;
      }
      else {
        strcat(buf, color);
        strcat(buf, " Board`x");
      }
    }
    if (current_room->vnum == 405471482) {
      if (found == FALSE) {
        strcat(buf, "`g[ `x Second B1 B2`x");
        found = TRUE;
      }
      else
      strcat(buf, " Second B1 B2`x");
    }

    if (fAuto && found == TRUE)
    strcat(buf, " `g]`x\n\r");

    // CLEAR! - Disco 12/8/20 warding off MXP bleed
    /*
    if(ch->desc != NULL && ch->desc->mxp == TRUE) {
      strcat( buf, "`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q`q");
    }
    */

    send_to_char(buf, ch);
    return;
  }

  bool valid_room_distance(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (ch->in_room == NULL || desti == NULL)
    return FALSE;
    int zdiff = ch->in_room->z - desti->z;
    int ydiff = ch->in_room->y - desti->y;
    int xdiff = ch->in_room->x - desti->x;

    if (zdiff > 200 || ydiff > 200 || xdiff > 200 || zdiff < -200 || ydiff < -200 || xdiff < -200)
    return FALSE;
    return TRUE;
  }

  char *room_distance(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (ch->in_room == NULL || desti == NULL)
    return "";
    int zdiff = ch->in_room->z - desti->z;
    int ydiff = ch->in_room->y - desti->y;
    int xdiff = ch->in_room->x - desti->x;

    if(abs(zdiff) + abs(ydiff) + abs(xdiff) > 10)
    {
      if (ydiff > 0 && xdiff < 0)
      return "`159Southeast`x";
      else if (ydiff > 0 && xdiff > 0)
      return "`159Southwest`x";
      else if (ydiff > 0)
      return "`159South`x";
      else if (ydiff < 0 && xdiff < 0)
      return "`159Northeast`x";
      else if (ydiff < 0 && xdiff > 0)
      return "`159Northwest`x";
      else if (ydiff < 0)
      return "`159North`x";
      else if (xdiff < 0)
      return "`159East`x";
      else if (xdiff > 0)
      return "`159West`x";
      else if (zdiff > 0)
      return "`159Up`x";
      else if (zdiff < 0)
      return "`159Down`x";
      else
      return "`159Somewhere`x";
    }

    static char buf[MAX_STRING_LENGTH];
    char tmp[MSL];
    buf[0] = '\0';
    bool started = FALSE;
    if (zdiff > 0) {
      sprintf(tmp, "`W%dD`x", zdiff);
      strcat(buf, tmp);
      started = TRUE;
    }
    else if (zdiff < 0) {
      sprintf(tmp, "`W%dU`x", zdiff * -1);
      strcat(buf, tmp);
      started = TRUE;
    }
    if (!IS_FLAG(ch->comm, COMM_CARDINAL)) {

      if (ydiff > 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%dS`x", ydiff);
        strcat(buf, tmp);
        started = TRUE;
      }
      else if (ydiff < 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%dN`x", ydiff * -1);
        strcat(buf, tmp);
        started = TRUE;
      }
      if (xdiff > 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%dW`x", xdiff);
        strcat(buf, tmp);
        started = TRUE;
      }
      else if (xdiff < 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%dE`x", xdiff * -1);
        strcat(buf, tmp);
        started = TRUE;
      }

    }
    else {

      if (ydiff > 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%d%s`x", ydiff, reldir_name[get_reldirection(DIR_SOUTH, ch->facing)][1]);
        strcat(buf, tmp);
        started = TRUE;
      }
      else if (ydiff < 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%d%s`x", ydiff * -1, reldir_name[get_reldirection(DIR_NORTH, ch->facing)][1]);
        strcat(buf, tmp);
        started = TRUE;
      }
      if (xdiff > 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%d%s`x", xdiff, reldir_name[get_reldirection(DIR_WEST, ch->facing)][1]);
        strcat(buf, tmp);
        started = TRUE;
      }
      else if (xdiff < 0) {
        if (started == TRUE)
        strcat(buf, "`G-`x");
        sprintf(tmp, "`W%d%s`x", xdiff * -1, reldir_name[get_reldirection(DIR_EAST, ch->facing)][1]);
        strcat(buf, tmp);
        started = TRUE;
      }
    }
    return str_dup(buf);
  }

  void alt_remote_view(CHAR_DATA *ch) {
    CHAR_DATA *wch;
    bool inhaven = FALSE;
    if (in_haven(ch->in_room))
    inhaven = TRUE;
    int homeworld = get_world(ch);

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      wch = *it;

      if (wch->in_room == NULL)
      continue;

      if (wch->in_room == ch->in_room)
      continue;

      if (inhaven && !in_haven(wch->in_room))
      continue;

      if (homeworld != get_world(wch))
      continue;

      if (is_cloaked(wch) && public_room(wch->in_room))
      continue;

      int foundrange = charlineofsight_character(ch, wch);
      if (foundrange != -1) {
        if (can_see(ch, wch))
        show_char_to_char_0(wch, ch, foundrange);
      }
    }
  }

  void remote_view(CHAR_DATA *ch) {
    alt_remote_view(ch);
    return;
  }

  CHAR_DATA *get_char_distance(CHAR_DATA *ch, char *argument, int dist) {
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *orig_room;
    ROOM_INDEX_DATA *in_room;
    EXIT_DATA *pexit;
    bool valid = TRUE;
    int i;

    orig_room = ch->in_room;
    in_room = ch->in_room;
    if (ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && in_haven(get_room_index(ch->pcdata->travel_to)) && in_haven(get_room_index(ch->pcdata->travel_from)) && in_haven(ch->your_car->in_room) && ch->in_room->vnum != ch->your_car->in_room->vnum) {
      in_room = ch->your_car->in_room;
    }

    for (i = 0; i < dist && valid; i++) {
      valid = FALSE;
      if ((pexit = in_room->exit[ch->facing]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (in_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
        valid = TRUE;
        char_from_room(ch);
        char_to_room(ch, in_room);
        if ((victim = get_char_room(ch, NULL, argument)) != NULL) {
          char_from_room(ch);
          char_to_room(ch, orig_room);
          return victim;
        }
      }
    }
    char_from_room(ch);
    char_to_room(ch, orig_room);
    return NULL;
  }

  int daysidle(char *name) {
    if (name[0] == '\0') {
      return 0;
    }
    time_t com_time;
    time_t east_time;

    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if(!str_cmp(name, (*it)->charname))
      {
        if ((*it)->logon <= 0) {
          com_time = current_time;
        }
        else
        com_time = (*it)->logon;

        east_time = current_time;

        int days = (east_time - com_time) / 3600 / 24;
        return days;
      }
    }

    CHAR_DATA *victim;
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return 200;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      return 200;
    }
    if (online == TRUE)
    return 0;

    if (IS_FLAG(victim->act, PLR_DEAD))
    return 50;
    if (victim->activeat <= 0) {
      com_time = sb.st_mtime;
    }
    else
    com_time = victim->activeat;

    east_time = current_time;

    int days = (east_time - com_time) / 3600 / 24;

    if (!online)
    free_char(victim);
    return days;
  }

  int offline_hours(char *name) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    if (name[0] == '\0') {
      return 0;
    }

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return 100;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      return 10;
    }
    if (online == TRUE)
    return 200;

    if (IS_FLAG(victim->act, PLR_DEAD))
    return 10;

    int val = victim->played / 3600;

    if (!online)
    free_char(victim);

    return val;
  }

  void show_location(CHAR_DATA *ch, CHAR_DATA *victim, int location) {
    int i;
    int len;
    char string[MSL];
    char buf[MSL];
    string[0] = '\0';
    char *pdesc;
    int iWear;
    OBJ_DATA *obj;

    if (IS_NPC(victim))
    return;
    if (victim->shape != SHAPE_HUMAN && victim->shape != SHAPE_MERMAID)
    return;

    for (iWear = MAX_WEAR - 1; iWear >= 0; iWear--) {
      if (iWear == WEAR_HOLD)
      continue;
      if (iWear == WEAR_HOLD_2)
      continue;

      if ((obj = get_eq_char(victim, iWear)) != NULL && does_undercover(obj, cover_table[location]) && can_see_obj(ch, obj) && (can_see_wear(victim, iWear) || (has_xray(ch) && victim != ch && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
        if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0')
        strcat(string, obj->wear_temp);

        strcat(string, format_obj_to_char(obj, ch, FALSE));

        strcat(string, "\n\r");

        pdesc = get_extra_descr_obj("all", obj->extra_descr, obj);
        if (pdesc != NULL) {
          strcat(string, pdesc);
        }
        else {
          pdesc = get_extra_descr_obj("all", obj->pIndexData->extra_descr, obj);
          if (pdesc != NULL) {
            strcat(string, pdesc);
          }
        }
      }
    }

    strcat(string, "\n\r");

    for (i = 0; i < MAX_COVERS + 1; i++) {
      if (i != location)
      continue;
      if ((i == COVERS_ARSE || i == COVERS_GROIN || i == COVERS_THIGHS || i == COVERS_LOWER_LEGS || i == COVERS_FEET) && victim->shape == SHAPE_MERMAID)
      continue;

      if (i == MAX_COVERS && (safe_strlen(victim->pcdata->focused_descs[i]) > 2 && ch->in_room == victim->in_room && (get_skill(ch, SKILL_ACUTESMELL) > 0 || ch == victim))) {
        sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
      }
      else if (i < MAX_COVERS && (!is_covered(victim, cover_table[i]) || has_xray(ch))) {
        sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
      }
      else
      continue;

      len = safe_strlen(buf);
      if (len >= 2) {
        if (buf[len - 2] == '\n')
        buf[len - 2] = 0;
        strcat(string, buf);
      }
      for (int j = 0; j < 300; j++) {
        if (victim->pcdata->stat_log_method[j] == TRAINED_TATTOO + location && victim->pcdata->stat_log_to[j] > 0 && (!is_covered(victim, cover_table[location]) || has_xray(ch))) {
          sprintf(buf, "%s has a tattoo of %s.\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->stat_log_string[j]);
          strcat(string, buf);
          j = 300;
        }
      }

      if (victim->pcdata->branddate > 0 && victim->pcdata->brandlocation == location && (!is_covered(victim, cover_table[location]) || has_xray(ch))) {
        sprintf(buf, "%s has a symbol of %s on %s %s. ", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->brandstring, (victim->sex == SEX_MALE) ? "his" : "her", name_by_location(location));
        strcat(string, buf);
      }
      if (safe_strlen(victim->pcdata->scars[location]) > 3 && (!is_covered(victim, cover_table[location]) || has_xray(ch))) {
        sprintf(buf, "%s ", victim->pcdata->scars[location]);
        strcat(string, buf);
      }

      strcat(string, "\n\r");

      /*
      if(str_cmp(victim->pcdata->focused_descs[i], "") && safe_strlen(victim->pcdata->focused_descs[i]) > 6
      &&(!is_covered(victim, cover_table[i]) || has_xray(ch)))
      {
      strcat(string, buf);
      }
      */
    }

    if (safe_strlen(victim->pcdata->detail_over[location]) > 3) {
      sprintf(buf, " %s", victim->pcdata->detail_over[location]);
      strcat(string, buf);
    }
    if (safe_strlen(victim->pcdata->detail_under[location]) > 3 && (!is_covered(victim, cover_table[location]) || has_xray(ch))) {
      sprintf(buf, " %s", victim->pcdata->detail_under[location]);
      strcat(string, buf);
    }
    strcat(string, "\n\r");

    send_to_char(wrap_string(string, get_wordwrap(ch)), ch);

    //    send_to_char(string, ch);

    /*
    char * result = str_dup(string);
    return result;
    */
  }

  bool show_char_location_to_char(CHAR_DATA *ch, CHAR_DATA *victim, char arg1[MSL]) {

    if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;
    if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;

    if (!str_cmp(arg1, "hands")) {
      show_location(ch, victim, 0);
      return TRUE;
    }
    else if (!str_cmp(arg1, "lowerarms") || !str_cmp(arg1, "forearms")) {
      show_location(ch, victim, 1);
      return TRUE;
    }
    else if (!str_cmp(arg1, "upperarms")) {
      show_location(ch, victim, 2);
      return TRUE;
    }
    else if (!str_cmp(arg1, "feet")) {
      show_location(ch, victim, 3);
      return TRUE;
    }
    else if (!str_cmp(arg1, "lowerlegs") || !str_cmp(arg1, "calves")) {
      show_location(ch, victim, 4);
      return TRUE;
    }
    else if (!str_cmp(arg1, "forehead")) {
      show_location(ch, victim, 5);
      return TRUE;
    }
    else if (!str_cmp(arg1, "thighs")) {
      show_location(ch, victim, 6);
      return TRUE;
    }
    else if (!str_cmp(arg1, "groin") || !str_cmp(arg1, "genitals") || !str_cmp(arg1, "crotch") || !str_cmp(arg1, "penis") || !str_cmp(arg1, "vulva") || !str_cmp(arg1, "cunt") || !str_cmp(arg1, "dick") || !str_cmp(arg1, "cock") || !str_cmp(arg1, "pussy")) {
      show_location(ch, victim, 7);
      return TRUE;
    }
    else if (!str_cmp(arg1, "arse") || !str_cmp(arg1, "buttocks") || !str_cmp(arg1, "butt") || !str_cmp(arg1, "ass") || !str_cmp(arg1, "rear")) {
      show_location(ch, victim, 8);
      return TRUE;
    }
    else if (!str_cmp(arg1, "lowerback")) {
      show_location(ch, victim, 9);
      return TRUE;
    }
    else if (!str_cmp(arg1, "upperback")) {
      show_location(ch, victim, 10);
      return TRUE;
    }
    else if (!str_cmp(arg1, "lowerchest")) {
      show_location(ch, victim, 11);
      return TRUE;
    }
    else if (!str_cmp(arg1, "breasts") || !str_cmp(arg1, "tits") || !str_cmp(arg1, "boobs")) {
      show_location(ch, victim, 12);
      return TRUE;
    }
    else if (!str_cmp(arg1, "upperchest")) {
      show_location(ch, victim, 13);
      return TRUE;
    }
    else if (!str_cmp(arg1, "neck")) {
      show_location(ch, victim, 14);
      return TRUE;
    }
    else if (!str_cmp(arg1, "lowerface") || !str_cmp(arg1, "lips") || !str_cmp(arg1, "mouth")) {
      show_location(ch, victim, 15);
      return TRUE;
    }
    else if (!str_cmp(arg1, "hair")) {
      show_location(ch, victim, 16);
      return TRUE;
    }
    else if (!str_cmp(arg1, "eyes")) {
      show_location(ch, victim, 17);
      return TRUE;
    }
    else if (!str_cmp(arg1, "arms")) {
      show_multilocation(ch, victim, 0);
      //	show_location(ch, victim, 0);
      //	show_location(ch, victim, 1);
      //	show_location(ch, victim, 2);
      return TRUE;
    }
    else if (!str_cmp(arg1, "legs")) {
      show_multilocation(ch, victim, 1);
      //	show_location(ch, victim, 3);
      //	show_location(ch, victim, 4);
      //	show_location(ch, victim, 6);
      return TRUE;
    }
    else if (!str_cmp(arg1, "torso") || !str_cmp(arg1, "body") || !str_cmp(arg1, "chest")) {
      show_multilocation(ch, victim, 3);
      //	show_location(ch, victim, 7);
      //	show_location(ch, victim, 8);
      //	show_location(ch, victim, 9);
      //	show_location(ch, victim, 10);
      //	show_location(ch, victim, 11);
      //	show_location(ch, victim, 12);
      //	show_location(ch, victim, 13);

      return TRUE;
    }
    else if (!str_cmp(arg1, "head") || !str_cmp(arg1, "face")) {
      show_multilocation(ch, victim, 2);
      //	show_location(ch, victim, 5);
      //	show_location(ch, victim, 14);
      //	show_location(ch, victim, 15);
      //	show_location(ch, victim, 16);
      //	show_location(ch, victim, 17);

      return TRUE;
    }
    else if (!str_cmp(arg1, "back")) {
      show_multilocation(ch, victim, 5);
    }
    else if (!str_cmp(arg1, "everything") || !str_cmp(arg1, "all")) {
      show_multilocation(ch, victim, 4);

      //	for(int i=0;i<18;i++)
      //	    show_location(ch, victim, i);

      return TRUE;
    }

    return FALSE;
  }

  bool does_multicover(OBJ_DATA *obj, int loc) {
    if (loc == 0) // arms
    {
      if (does_undercover(obj, cover_table[0]))
      return TRUE;
      if (does_undercover(obj, cover_table[1]))
      return TRUE;
      if (does_undercover(obj, cover_table[2]))
      return TRUE;
    }
    if (loc == 1) // legs
    {
      if (does_undercover(obj, cover_table[3]))
      return TRUE;
      if (does_undercover(obj, cover_table[4]))
      return TRUE;
      if (does_undercover(obj, cover_table[6]))
      return TRUE;
    }
    if (loc == 2) // head
    {
      if (does_undercover(obj, cover_table[5]))
      return TRUE;
      if (does_undercover(obj, cover_table[14]))
      return TRUE;
      if (does_undercover(obj, cover_table[15]))
      return TRUE;
      if (does_undercover(obj, cover_table[16]))
      return TRUE;
      if (does_undercover(obj, cover_table[17]))
      return TRUE;
    }
    if (loc == 3) // torso
    {
      if (does_undercover(obj, cover_table[7]))
      return TRUE;
      if (does_undercover(obj, cover_table[8]))
      return TRUE;
      if (does_undercover(obj, cover_table[9]))
      return TRUE;
      if (does_undercover(obj, cover_table[10]))
      return TRUE;
      if (does_undercover(obj, cover_table[11]))
      return TRUE;
      if (does_undercover(obj, cover_table[12]))
      return TRUE;
      if (does_undercover(obj, cover_table[13]))
      return TRUE;
    }
    if (loc == 4) // everything
    {
      return TRUE;
    }

    return FALSE;
  }

  bool valid_multiloc(int i, int location) {

    if (location == 0) // arms
    {
      if (i == 0)
      return TRUE;
      if (i == 1)
      return TRUE;
      if (i == 2)
      return TRUE;
    }
    if (location == 1) // legs
    {
      if (i == 3)
      return TRUE;
      if (i == 4)
      return TRUE;
      if (i == 6)
      return TRUE;
    }
    if (location == 2) // head
    {
      if (i == 5)
      return TRUE;
      if (i == 14)
      return TRUE;
      if (i == 15)
      return TRUE;
      if (i == 16)
      return TRUE;
      if (i == 17)
      return TRUE;
    }
    if (location == 3) // torso
    {
      if (i == 7)
      return TRUE;
      if (i == 8)
      return TRUE;
      if (i == 9)
      return TRUE;
      if (i == 10)
      return TRUE;
      if (i == 11)
      return TRUE;
      if (i == 12)
      return TRUE;
      if (i == 13)
      return TRUE;
    }
    if (location == 5) // Back
    {
      if (i == 8)
      return TRUE;
      if (i == 9)
      return TRUE;
      if (i == 10)
      return TRUE;
    }
    if (location == 4) // everything
    {
      return TRUE;
    }

    return FALSE;
  }

  void show_multilocation(CHAR_DATA *ch, CHAR_DATA *victim, int location) {
    int i;
    int len;
    char string[MSL];
    char buf[MSL];
    string[0] = '\0';
    char *pdesc;
    int iWear;
    OBJ_DATA *obj;

    for (iWear = MAX_WEAR - 1; iWear >= 0; iWear--) {
      if (iWear == WEAR_HOLD)
      continue;
      if (iWear == WEAR_HOLD_2)
      continue;

      if ((obj = get_eq_char(victim, iWear)) != NULL && does_multicover(obj, location) && can_see_obj(ch, obj) && (can_see_wear(victim, iWear) || (has_xray(ch) && victim != ch && (!IS_IMMORTAL(ch) || !is_spyshield(victim))))) {
        if (obj->wear_temp != NULL && obj->wear_temp[0] != '\0')
        strcat(string, obj->wear_temp);

        strcat(string, format_obj_to_char(obj, ch, FALSE));

        strcat(string, "\n\r");

        pdesc = get_extra_descr_obj("all", obj->extra_descr, obj);
        if (pdesc != NULL) {
          strcat(string, pdesc);
        }
        else {
          pdesc = get_extra_descr_obj("all", obj->pIndexData->extra_descr, obj);
          if (pdesc != NULL) {
            strcat(string, pdesc);
          }
        }
      }
    }
    strcat(string, "\n\r");

    for (i = 0; i < MAX_COVERS + 1; i++) {
      if (!valid_multiloc(i, location))
      continue;
      if ((i == COVERS_ARSE || i == COVERS_GROIN || i == COVERS_THIGHS || i == COVERS_LOWER_LEGS || i == COVERS_FEET) && victim->shape == SHAPE_MERMAID)
      continue;

      if (i == MAX_COVERS && (safe_strlen(victim->pcdata->focused_descs[i]) > 2 && ch->in_room == victim->in_room && (get_skill(ch, SKILL_ACUTESMELL) > 0 || ch == victim))) {
        sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
      }
      else if (i < MAX_COVERS && safe_strlen(victim->pcdata->focused_descs[i]) > 2) {
        sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
      }
      else
      continue;

      len = safe_strlen(buf);
      if (len >= 2) {
        if (buf[len - 2] == '\n')
        buf[len - 2] = 0;
      }

      if (str_cmp(victim->pcdata->focused_descs[i], "") && safe_strlen(victim->pcdata->focused_descs[i]) > 6 && (!is_covered(victim, cover_table[i]) || has_xray(ch))) {
        strcat(string, buf);
      }
      if (safe_strlen(victim->pcdata->detail_over[i]) > 3) {
        sprintf(buf, " %s", victim->pcdata->detail_over[i]);
        strcat(string, buf);
      }
      if (safe_strlen(victim->pcdata->detail_under[i]) > 3 && (!is_covered(victim, cover_table[i]) || has_xray(ch))) {
        sprintf(buf, " %s", victim->pcdata->detail_under[i]);
        strcat(string, buf);
      }
      if (victim->pcdata->branddate > 0 && victim->pcdata->brandlocation == i) {
        sprintf(buf, "%s has a symbol of %s on %s %s. ", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->brandstring, (victim->sex == SEX_MALE) ? "his" : "her", name_by_location(i));
        strcat(string, buf);
      }
      if (safe_strlen(victim->pcdata->scars[i]) > 3 && (!is_covered(victim, cover_table[i]) || has_xray(ch))) {
        sprintf(buf, "%s ", victim->pcdata->scars[i]);
        strcat(string, buf);
      }

      strcat(string, "\n\r");
    }

    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);

    //    send_to_char(string, ch);

    /*
    char * result = str_dup(string);
    return result;
    */
  }

  int offline_age(char *name) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    if (name[0] == '\0') {
      return 0;
    }

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return 0;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      return 0;
    }

    if (IS_FLAG(victim->act, PLR_DEAD))
    return 0;

    int age = get_true_age(victim);
    if (!online)
    free_char(victim);

    return age;
  }

  void transfer_stuff(CHAR_DATA *ch, char *name) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    if (name[0] == '\0') {
      return;
    }

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (!str_cmp((*it)->owner, ch->name)) {
        free_string((*it)->owner);
        (*it)->owner = str_dup(name);
      }
    }

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      return;
    }

    if (IS_FLAG(victim->act, PLR_DEAD))
    return;

    OBJ_DATA *container;
    OBJ_DATA *obj_next;
    OBJ_DATA *obj;
    container = create_object(get_obj_index(35), 0);

    sprintf(buf, "A pile of %s's belongings", ch->pcdata->intro_desc);
    free_string(container->short_descr);
    container->short_descr = str_dup(buf);

    sprintf(buf, "A pile of %s's belongings", ch->pcdata->intro_desc);
    free_string(container->description);
    container->description = str_dup(buf);

    sprintf(buf, "pile belongings %s %s", ch->pcdata->intro_desc, ch->name);
    free_string(container->name);
    container->name = str_dup(buf);

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        obj_from_char_silent(obj);
        obj_to_obj(obj, container);
      }
    }
    obj_to_char(container, victim);
    if (ch->pcdata->total_money > 0)
    victim->pcdata->total_money += ch->pcdata->total_money;
    ch->pcdata->total_money = 0;
    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  void inherit(CHAR_DATA *ch) {

    int i;
    int sibage = 0;
    int kidage = 17;
    int pointer = -1;
    for (i = 0; i < 10; i++) {
      if (ch->pcdata->relationship_type[i] == REL_SIBLING && daysidle(ch->pcdata->relationship[i]) < 30 && offline_age(ch->pcdata->relationship[i]) > sibage) {
        pointer = i;
        kidage = offline_age(ch->pcdata->relationship[i]);
      }
    }
    for (i = 0; i < 10; i++) {
      if (ch->pcdata->relationship_type[i] == REL_PARENT && daysidle(ch->pcdata->relationship[i]) < 30 && offline_age(ch->pcdata->relationship[i]) > kidage) {
        pointer = i;
        kidage = offline_age(ch->pcdata->relationship[i]);
      }
    }
    for (i = 0; i < 10; i++) {
      if (ch->pcdata->relationship_type[i] == REL_SPOUSE && daysidle(ch->pcdata->relationship[i]) < 30) {
        pointer = i;
      }
    }

    if (pointer > -1)
    transfer_stuff(ch, ch->pcdata->relationship[pointer]);
  }

  // temporary hotspots for the where command - Discordance
  _DOFUN(do_hotspot) {
    // Exceptions and errors
    if (is_helpless(ch)) {
      send_to_char("`cYou're helpless to decide whether this is a hot spot or not`g.`x\n\r", ch);
    }
    else if (is_fighting(ch)) {
      send_to_char("`cYou're more concerned with fighting than deciding whether this is a hot spot`g.`x\n\r", ch);
    }
    else if (is_pinned(ch)) {
      send_to_char("`cYou're in a tight spot, not a hot spot`g.`x\n\r", ch);
    }
    else if (room_hostile(ch->in_room)) {
      send_to_char("`cThis spot may be a little too hot`g.`x\n\r", ch);
    }
    else if (silenced(ch)) {
      send_to_char("`cYou seem alone in your belief this could be a hot spot`g.`x\n\r", ch);
    }
    // flag toggling
    else {
      if (IS_FLAG(ch->comm, COMM_HOTSPOT)) {
        REMOVE_FLAG(ch->comm, COMM_HOTSPOT);
        send_to_char("`cYou no longer consider this a hot spot`g.`x\n\r", ch);
      }
      else {
        SET_FLAG(ch->comm, COMM_HOTSPOT);
        send_to_char("`cYou consider this a hot spot`g.`x\n\r", ch);
      }
    }
    return;
  }

  //a ban list to keep characters and accounts from seeing you on where - Disco
  void where_ban(CHAR_DATA *ch, char *argument) {
    char arg2[MSL], arg3[MSL], buf[MSL];
    int i;
    bool stored = FALSE, errored = FALSE, online = FALSE, account = FALSE, nochar = FALSE;
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;
    struct stat sb;

    argument = one_argument_nouncap(argument, arg2);

    for (i = 0; i < 50; i++) {
      if (ch->pcdata->nowhere_characters[i] == NULL) {
        ch->pcdata->nowhere_characters[i] = str_dup("");
      }
    }

    for (i = 0; i < 50; i++) {
      if (ch->pcdata->nowhere_accounts[i] == NULL) {
        ch->pcdata->nowhere_accounts[i] = str_dup("");
      }
    }

    if (!str_cmp(arg2, "list")) {
      send_to_char("`cWhere Banned List`x:\n\r", ch);
      send_to_char("`g--------------------------------------------------------------------------------`x\n\r", ch);
      send_to_char("`cCharacters`g:`x\n\r", ch);
      for (i = 0; i < 50; i++) {
        if (str_cmp("", ch->pcdata->nowhere_characters[i])) {
          printf_to_char(ch, "`g[`W%d`g]`x %s\n\r", i + 1, ch->pcdata->nowhere_characters[i]);
        }
      }
      send_to_char("\n\r", ch);
      send_to_char("`cAccounts`g:`x\n\r", ch);
      for (i = 0; i < 50; i++) {
        if (str_cmp("", ch->pcdata->nowhere_accounts[i])) {
          printf_to_char(ch, "`g[`W%d`g]`x %s\n\r", i + 1, ch->pcdata->nowhere_accounts[i]);
        }
      }
      send_to_char("\n\r", ch);
      send_to_char("`cOther`g:`x\n\r", ch);
      if (ch->pcdata->nowhere_male == 1) {
        send_to_char("All males banned.\n\r", ch);
      }
      if (ch->pcdata->nowhere_female == 1) {
        send_to_char("All females banned.\n\r", ch);
      }
      return;
    }
    d.original = NULL;
    if (!str_cmp(arg2, "account")) {
      argument = one_argument_nouncap(argument, arg3);
      account = load_account_obj(&d, arg3);
      if (account == TRUE) {
        // checking where ban account list
        for (i = 0; i < 50; i++) {
          if (!str_cmp(arg3, ch->pcdata->nowhere_accounts[i])) {
            printf_to_char(ch, "%s will be able to see you on where.\n\r", capitalize(arg3));
            free_string(ch->pcdata->nowhere_accounts[i]);
            ch->pcdata->nowhere_accounts[i] = str_dup("");
            stored = TRUE;
            break;
          }
        }

        if (stored == FALSE) {
          // Looking for blank spot for new account
          for (i = 0; i < 50; i++) {
            if (!str_cmp("", ch->pcdata->nowhere_accounts[i])) {
              ch->pcdata->nowhere_accounts[i] = str_dup(capitalize(arg3));
              printf_to_char(ch, "%s will be unable to see you on where.\n\r", capitalize(arg3));
              stored = TRUE;
              break;
            }
          }
        }
      }
      else {
        send_to_char("No account found.\n\r", ch);
        errored = TRUE;
      }
      return;
    }

    // ban based on character sex
    if (!str_cmp(arg2, "female")) {
      if (ch->pcdata->nowhere_female == 0) {
        ch->pcdata->nowhere_female = 1;
        send_to_char("All females banned.\n\r", ch);
      }
      else {
        ch->pcdata->nowhere_female = 0;
        send_to_char("All females unbanned.\n\r", ch);
      }
      return;
    }
    else if (!str_cmp(arg2, "male")) {
      if (ch->pcdata->nowhere_male == 0) {
        ch->pcdata->nowhere_male = 1;
        send_to_char("All males banned.\n\r", ch);
      }
      else {
        ch->pcdata->nowhere_male = 0;
        send_to_char("All males unbanned.\n\r", ch);
      }
      return;
    }

    // characters ban
    if ((victim = get_victim_world(ch, arg3)) != NULL) {
      online = TRUE;
    }
    else if (!load_char_obj(&d, arg2)) {
      nochar = TRUE;
    }
    else {
      log_string("DESCRIPTOR: Whereban");
      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg2));
      stat(buf, &sb);
      victim = d.character;
    }

    if (nochar == TRUE) { // victim does not exist, checking whereban list to remove
      for (i = 0; i < 50; i++) {
        if (!str_cmp(arg2, ch->pcdata->nowhere_characters[i])) {
          printf_to_char(ch, "%s will be able to see you on where.\n\r", capitalize(arg2));
          free_string(ch->pcdata->nowhere_characters[i]);
          ch->pcdata->nowhere_characters[i] = str_dup("");
          stored = TRUE;
          break;
        }
      }
      if (stored == FALSE) {
        send_to_char("No character found.\n\r", ch);
      }

      return;
    }
    else { // checking where ban character list when victim exists
      for (i = 0; i < 50; i++) {
        if (!str_cmp(victim->name, ch->pcdata->nowhere_characters[i])) {
          printf_to_char(ch, "%s will be able to see you on where.\n\r", capitalize(arg2));
          free_string(ch->pcdata->nowhere_characters[i]);
          ch->pcdata->nowhere_characters[i] = str_dup("");
          stored = TRUE;
          break;
        }
      }
    }

    if (stored == FALSE) { // adding to character list
      if (nochar == TRUE) {
        send_to_char("No character found.\n\r", ch);
        return;
      }

      if (IS_NPC(victim)) {
        send_to_char("NPCs can't see where.\n\r", ch);
        errored = TRUE;
      }
      else if (victim == ch) {
        send_to_char("You can't ban yourself.\n\r", ch);
        errored = TRUE;
      }
      else if (IS_IMMORTAL(victim)) {
        send_to_char("You can't ban immortals.\n\r", ch);
        errored = TRUE;
      }
      else {
        // Looking for blank spot for new character
        for (i = 0; i < 50; i++) {
          if (!str_cmp("", ch->pcdata->nowhere_characters[i])) {
            ch->pcdata->nowhere_characters[i] = str_dup(victim->name);
            printf_to_char(ch, "%s will be unable to see you on where.\n\r", capitalize(arg2));
            stored = TRUE;
            break;
          }
        }
      }
    }

    if (!online) {
      free_char(victim);
    }

    if (stored == FALSE && errored == FALSE) {
      send_to_char("Your where ban list is already full.  Please remove someone from it first.\n\r", ch);
    }
    return;
  }

  // reasons to obscure where output
  bool can_see_where(CHAR_DATA *ch, CHAR_DATA *victim) {
    int i, n;

    // bad stuff
    if (IS_NPC(victim)) {
      return FALSE;
    }
    if (IS_NPC(ch)) {
      return FALSE;
    }
    if (victim->in_room == NULL) {
      return FALSE;
    }
    if (ch->in_room == NULL) {
      return FALSE;
    }
    if (ch == victim) {
      return TRUE;
    }

    if (is_dreaming(victim) && goddreamer(victim))
    return FALSE;

    // checking where ban list
    for (i = 0; i < 50; i++) {
      for (n = 0; n < 25; n++) {
        if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
          if (!str_cmp(ch->pcdata->account->characters[n], victim->pcdata->nowhere_characters[i])) {
            return FALSE;
          }
        }
      }
    }

    if (victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SPAMMER) && !same_player(ch, victim))
    return FALSE;

    // checking where ban list
    for (i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->nowhere_accounts[i])) {
        return FALSE;
      }
    }

    if (ch->sex == SEX_FEMALE && victim->pcdata->nowhere_female == 1) {
      return FALSE;
    }
    if (ch->sex == SEX_MALE && victim->pcdata->nowhere_male == 1) {
      return FALSE;
    }

    // dream stuff
    if (is_dreaming(victim) && !IS_FLAG(victim->comm, COMM_PRIVATE)) {
      return TRUE;
    }
    if (is_dreaming(victim) && is_helpless(victim)) {
      return TRUE;
    }

    // shroud stuff
    if (IS_FLAG(victim->act, PLR_SHROUD) && !can_shroud(ch) && !IS_FLAG(ch->act, PLR_SHROUD)) {
      return FALSE;
    }
    if (IS_FLAG(victim->act, PLR_DEEPSHROUD) && !IS_IMMORTAL(ch) && !IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
      return FALSE;
    }
    if (!IS_FLAG(ch->act, PLR_SHROUD) && is_sprite(victim)) {
      return FALSE;
    }
    if (IS_FLAG(victim->act, PLR_SHROUD) && !can_shroud(ch) && !IS_FLAG(ch->act, PLR_SHROUD)) {
      return FALSE;
    }
    if (IS_FLAG(victim->act, PLR_DEEPSHROUD) && !IS_IMMORTAL(ch) && !IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
      return FALSE;
    }

    if (!IS_NPC(ch) && is_cloaked(victim) && ch->pcdata->paranoid >= 1000 && public_room(ch->in_room) && !IS_IMMORTAL(ch)) {
      return FALSE;
    }

    if (is_gm(victim) || IS_IMMORTAL(victim)) {
      return FALSE;
    }
    if (IS_FLAG(victim->act, PLR_DEAD) || is_ghost(victim)) {
      return FALSE;
    }
    if (IS_FLAG(victim->comm, COMM_CLOAKED)) {
      return FALSE;
    }
    if (is_forcibly_helpless(victim) && institute_room(victim->in_room) && (clinic_staff(ch, FALSE) || college_staff(ch, FALSE))) {
      return TRUE;
    }
    if (is_forcibly_helpless(victim) && (victim->in_room->area->vnum != DIST_MISTS)) {
      return FALSE;
    }
    if (in_fight(victim)) {
      return FALSE;
    }
    if (is_pinned(victim)) {
      return FALSE;
    }
    if (room_hostile(victim->in_room)) {
      return FALSE;
    }

    if (silenced(victim) || silenced(ch)) {
      return FALSE;
    }

    return TRUE;
  }

  // outputs wheretitles
  void display_where_char(CHAR_DATA *ch) {
    Buffer outbuf2;
    Buffer outbuf3;
    char preposition[2], buf[MSL], intro[MSL], header[MSL];
    char room_name[MSL], vnum_string[MSL], playertitle[MSL], no_color[MSL];
    ROOM_INDEX_DATA *room;
    bool FirstWilds = FALSE, FirstLoner = TRUE, FirstCouple = TRUE;
    int population = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;
      if ((d->connected != CON_PLAYING))
      continue;
      victim = d->character;

      // Exceptions
      if (!can_see_where(ch, victim)) {
        continue;
      }

      if (victim->in_room->vnum < 300)
      continue;

      // Determining room population
      population = 0;
      for (CharList::iterator it = victim->in_room->people->begin();
      it != victim->in_room->people->end(); ++it) {

        if (is_ghost(*it))
        continue;
        if (is_gm(*it))
        continue;
        if (IS_NPC(*it))
        continue;

        population++;
      }

      // determines which roomtitle and room to use
      if (is_dreaming(victim)) {
        if (safe_strlen(victim->pcdata->dreamtitle) <= 2) {
          strcpy(playertitle, " is");
        }
        else {
          strcpy(playertitle, victim->pcdata->dreamtitle);
        }
        room = get_room_index(victim->pcdata->dream_room);
        population = 0;
        for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
          CHAR_DATA *vch;
          DESCRIPTOR_DATA *d = *it;
          vch = CH(d);
          if (vch == NULL)
          continue;
          if (IS_NPC(vch) || !is_dreaming(vch))
          continue;
          if (vch->pcdata->dream_room == victim->pcdata->dream_room)
          population++;
        }
      }
      else {
        if (safe_strlen(victim->pcdata->title) <= 2) {
          strcpy(playertitle, " is");
        }
        else {
          strcpy(playertitle, victim->pcdata->title);
        }
        room = victim->in_room;
      }

      if (room == NULL) {
        return;
      }
      if (victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHH) && !same_player(ch, victim)) {
        sprintf(playertitle, " ");
      }
      if (victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SPAMMER) && !same_player(ch, victim)) {
        sprintf(playertitle, " ");
      }

      // This section sets up prepositions based on the room's sector type
      if (victim->pcdata->travel_type == TRAVEL_HORSE || victim->pcdata->travel_type == TRAVEL_HPASSENGER || victim->pcdata->travel_type == TRAVEL_BIKE || victim->pcdata->travel_type == TRAVEL_BPASSENGER) {
        strcpy(preposition, "on");
      }
      else if (room->sector_type == SECT_STREET) {
        remove_color(no_color, roomtitle(room, FALSE));
        if (strcasestr(no_color, " and ")) {
          strcpy(preposition, "at");
        }
        else {
          strcpy(preposition, "on");
        }
      }
      else if (room->sector_type == SECT_STREET) {
        strcpy(preposition, "in");
      }
      else if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        strcpy(preposition, "at");
      }
      else {
        strcpy(preposition, "in");
      }

      strcpy(buf, "");

      // Vnums for Imms
      if (IS_IMMORTAL(ch)) {
        sprintf(vnum_string, "`g [`W%d`g]`x", room->vnum);
      }
      else {
        strcpy(vnum_string, "");
      }

      // to prevent alteration of in_room->name by lowercase_clause and get rid of
      // color
      //    if(!is_dreaming(ch))
      //        remove_color(room_name, roomtitle(room, FALSE));
      //    else
      //	sprintf(room_name, "%s", roomtitle(room, FALSE));

      if (in_fantasy(victim) != NULL) {
        sprintf(room_name, " %s(`c%s`x)", roomtitle(room, FALSE), in_fantasy(victim)->name);
      }
      else
      remove_color(room_name, roomtitle(room, FALSE));

      if (!can_see_where(ch, victim)) {
        continue;
      }
      if (!IS_FLAG(victim->comm, COMM_WHEREVIS)) {
        continue;
      }
      /*
      if(safe_strlen(playertitle) <=2) {
      continue;
      }
      */
      /*
      if (population >= 3) {
      continue;
      }
      */
      if (!is_dreaming(victim))
      strcpy(header, distance_header(ch, victim, 1));
      else
      strcpy(header, "");

      // Section headers as necessary and based on population
      if (in_world(victim) != WORLD_EARTH && in_world(victim) != WORLD_ELSEWHERE && !is_dreaming(victim)) {
        // Displays only if valid seeker
        // if(in_world(ch) == in_world(victim) || (is_gm(ch) && // is_wildsapproved(ch)) || IS_IMMORTAL(ch)) { Header displays only once
        if (FirstWilds == TRUE) {
          outbuf2.strcat("\n\r    `cOffworld`g:`x\n\r");
          FirstWilds = FALSE;
        }
        // Anonymous strings
        if (IS_FLAG(victim->comm, COMM_WHEREANON)) {
          if (victim->shape == SHAPE_HUMAN || victim->shape == SHAPE_MERMAID) {
            sprintf(buf, "%s%s`c%s `x%s%s.\n\r", header, (victim->sex == SEX_MALE) ? "A man" : "A woman", playertitle, lowercase_clause(room_name), vnum_string);
          }
          else {
            sprintf(buf, "%sAn animal`c%s `x%s%s.\n\r", header, playertitle, lowercase_clause(room_name), vnum_string);
          }
        }
        else { // Non-anonymous strings
          if (is_dreaming(victim)) {
            sprintf(intro, "%s%s", header, dream_introduction(victim));
          }
          else if (is_masked(victim) && victim->shape == SHAPE_HUMAN)
          sprintf(intro, "%s%s", header, capitalize(mask_intro(victim, victim->pcdata->maskednumber)));
          else if (IS_FLAG(victim->act, PLR_DEAD))
          sprintf(intro, "%sThe ghost of %s", header, capitalize(get_intro(victim)));
          else
          sprintf(intro, "%s%s", header, capitalize(PERS(victim, ch)));

          sprintf(buf, "%s`c%s `x%s%s.\n\r", intro, playertitle, lowercase_clause(room_name), vnum_string);
        }
        outbuf2.strcat(buf);
        //}
      }
      else {
        if (population == 1) {
          if (FirstLoner == TRUE) {
            outbuf2.strcat("\n\r   `cLoners`g:`x\n\r");
            FirstLoner = FALSE;
          }
          if (IS_FLAG(victim->comm, COMM_WHEREANON)) {
            if (victim->shape == SHAPE_HUMAN || victim->shape == SHAPE_MERMAID) {
              sprintf(buf, "%s%s`c%s `x%s%s.\n\r", header, (victim->sex == SEX_MALE) ? "A man" : "A woman", playertitle, lowercase_clause(room_name), vnum_string);
            }
            else {
              sprintf(buf, "%sAn animal`c%s `x%s%s.\n\r", header, playertitle, lowercase_clause(room_name), vnum_string);
            }
          }
          else {
            if (is_dreaming(victim)) {
              sprintf(intro, "%s%s", header, dream_introduction(victim));
            }
            else if (is_masked(victim) && victim->shape == SHAPE_HUMAN)
            sprintf(
            intro, "%s%s", header, capitalize(mask_intro(victim, victim->pcdata->maskednumber)));
            else if (IS_FLAG(victim->act, PLR_DEAD))
            sprintf(intro, "%sThe ghost of %s", header, capitalize(get_intro(victim)));
            else
            sprintf(intro, "%s%s", header, capitalize(PERS(victim, ch)));

            sprintf(buf, "%s`c%s `x%s%s.\n\r", intro, playertitle, lowercase_clause(room_name), vnum_string);
          }
          outbuf2.strcat(buf);
        }
        else if (population > 1) {
          if (FirstCouple == TRUE) {
            outbuf3.strcat("\n\r    `cGroups`g:`x\n\r");
            FirstCouple = FALSE;
          }
          if (IS_FLAG(victim->comm, COMM_WHEREANON)) {
            if (victim->shape == SHAPE_HUMAN || victim->shape == SHAPE_MERMAID) {
              sprintf(buf, "%s%s`c%s `x%s%s.\n\r", header, (victim->sex == SEX_MALE) ? "A man" : "A woman", playertitle, lowercase_clause(room_name), vnum_string);
            }
            else {
              sprintf(buf, "%sAn animal`c%s `x%s%s.\n\r", header, playertitle, lowercase_clause(room_name), vnum_string);
            }
          }
          else {
            if (is_dreaming(victim)) {
              sprintf(intro, "%s%s", header, dream_introduction(victim));
            }
            else if (is_masked(victim) && victim->shape == SHAPE_HUMAN)
            sprintf(
            intro, "%s%s", header, capitalize(mask_intro(victim, victim->pcdata->maskednumber)));
            else if (IS_FLAG(victim->act, PLR_DEAD))
            sprintf(intro, "%sThe ghost of %s", header, capitalize(get_intro(victim)));
            else
            sprintf(intro, "%s%s", header, capitalize(PERS(victim, ch)));

            sprintf(buf, "%s`c%s `x%s%s(`039%d`x).\n\r", intro, playertitle, lowercase_clause(room_name), vnum_string, pc_pop(victim->in_room));
          }
          outbuf3.strcat(buf);
        }
      }

    }
    if (FirstLoner == FALSE || FirstWilds == FALSE) {
      page_to_char(outbuf2, ch);
    }
    if (FirstCouple == FALSE) {
      page_to_char(outbuf3, ch);
    }
    return;
  }

  //  hotspots for where - Disco
  bool display_hotspots(CHAR_DATA *ch, int rooms[100]) {
    int i = 0, population = 0;
    ROOM_INDEX_DATA *room;

    CHAR_DATA *victim;
    bool TempHotSpot = FALSE, contested = FALSE, FirstHotSpot = TRUE;
    char buf[MSL], no_color[MSL], preposition[MSL], noun[MSL], population_prefix[2];
    Buffer outbuf;

    for (i = 0; i < 100; i++) {
      if (rooms[i] == 0) {
        continue;
      }
      room = get_room_index(rooms[i]);
      if (room == NULL) {
        continue;
      }

      population = 0;
      TempHotSpot = FALSE;
      contested = FALSE;

      for (CharList::iterator it = room->people->begin();
      it != room->people->end(); ++it) {
        victim = *it;

        if (IS_NPC(victim)) {
          continue;
        }

        // Exceptions
        if (!can_see_where(ch, victim)) {
          continue;
        }
        if (in_fight(victim) || is_pinned(victim) || room_hostile(victim->in_room) || silenced(victim) || silenced(ch)) {
          contested = TRUE;
        }
        if (IS_FLAG(victim->comm, COMM_HOTSPOT)) {
          TempHotSpot = TRUE;
        }

        population++;
      }

      // check for flagged rooms unless population is 8 or higher
      if ((!IS_SET(room->room_flags, ROOM_WHEREVIS) && TempHotSpot == FALSE) || contested == TRUE) {
        if (population <= 6) {
          continue;
        }
      }

      // This section sets up prepositions based on the room's sector type
      // This looks like it's unneeded due to bypass by roomtitle(); preposition
      // not used? - Discordance
      if (room->sector_type == SECT_STREET) {
        remove_color(no_color, roomtitle(room, FALSE));
        if (strcasestr(no_color, " and ")) {
          strcpy(preposition, "at");
        }
        else {
          strcpy(preposition, "on");
        }
      }
      else if (room->sector_type == SECT_STREET) {
        strcpy(preposition, "in");
      }
      else if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        strcpy(preposition, "at");
      }
      else {
        strcpy(preposition, "in");
      }

      if (population > 0) {
        // This section sets up nouns
        if (population > 1) {
          strcpy(noun, "people");
        }
        else if (population == 1) {
          strcpy(noun, "person");
        }

        if (population > 9) {
          strcpy(population_prefix, " ");
        }
        else {
          strcpy(population_prefix, "  ");
        }

        if (FirstHotSpot == TRUE) {
          outbuf.strcat("\n\r`cHot Spots`g:`x\n\r");
          FirstHotSpot = FALSE;
        }
        // Final output without color to avoid craziness
        remove_color(no_color, roomtitle(room, FALSE));
        sprintf(buf, "%s%d `c%s `x%s\n\r", population_prefix, population, noun, no_color);
        outbuf.strcat(buf);
      }
    }

    page_to_char(outbuf, ch);
    return FirstHotSpot;
  }

  void display_dreamers(CHAR_DATA *ch, int dreamers, bool FirstHotSpot, int worlds[20], int worlds_where[20]) {
    char noun[MSL], buf[MSL];
    char population_prefix[2];
    Buffer outbuf;
    if (dreamers < 3)
    return;
    // Dreamworld population output
    if (dreamers > 0) {
      if (FirstHotSpot == TRUE) {
        outbuf.strcat("\n\r`cHot Spots`g:`x\n\r");
        FirstHotSpot = FALSE;
      }
      if (dreamers > 1) {
        strcpy(noun, "people are");
      }
      else {
        strcpy(noun, "person is");
      }

      if (dreamers > 9) {
        strcpy(population_prefix, " ");
      }
      else {
        strcpy(population_prefix, "  ");
      }
      sprintf(buf, "%s%d `c%s dreaming`x\n\r", population_prefix, dreamers, noun);
      outbuf.strcat(buf);
      for (int i = 0; i < 20; i++) {
        bool first = TRUE;
        if (worlds[i] > 0 && worlds_where[i] > 0) {
          FANTASY_TYPE *fant = fetch_fantasy(NULL, worlds[i]);
          if (fant != NULL) {
            if (first) {
              sprintf(buf, "`c      There is activity in`c:`x %s", fant->name);
              first = FALSE;
            }
            else
            sprintf(buf, "`c, %s", fant->name);
            outbuf.strcat(buf);
          }
        }
        if (first == FALSE) {
          sprintf(buf, ".`x\n\r");
          outbuf.strcat(buf);
        }
      }
      page_to_char(outbuf, ch);
    }

    return;
  }

  int dreamroom_pop(int vnum) {
    int pop = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL)
      continue;
      if (IS_NPC(vch) || !is_dreaming(vch))
      continue;
      if (IS_FLAG(vch->comm, COMM_PRIVATE))
      continue;
      if (vch->pcdata->dream_room == vnum)
      pop++;
    }
    return pop;
  }

  bool can_super_lure(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    if(ch->pcdata->tcooldown > 0)
    return FALSE;
    if (ch->faction == 0 && get_skill(ch, SKILL_HYPNOTISM) < 1 && arcane_focus(ch) < 1) {
      return FALSE;
    }
    if(is_helpless(ch))
    return FALSE;

    if(victim == NULL)
    return TRUE;

    if(victim->pcdata->availability != AVAIL_HIGH)
    return FALSE;

    if(IS_FLAG(victim->comm, COMM_AFK))
    return FALSE;

    return TRUE;
  }

  bool power_possess(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    bool canpossess = FALSE;
    if(victim->pcdata->in_domain > 0 && is_helpless(victim))
    {
      DOMAIN_TYPE *vdomain = vnum_domain(victim->pcdata->in_domain);
      DOMAIN_TYPE *cdomain = my_domain(ch);
      if (vdomain != NULL && cdomain != NULL && cdomain->vnum == vdomain->vnum)
      canpossess = TRUE;
    }
    if(victim->fcult != 0)
    {
      FACTION_TYPE *fcult = clan_lookup(victim->fcult);
      if(!str_cmp(fcult->eidilon, ch->name))
      canpossess = TRUE;
    }
    if(victim->fsect != 0)
    {
      FACTION_TYPE *fsect = clan_lookup(victim->fsect);
      if(!str_cmp(fsect->eidilon, ch->name))
      canpossess = TRUE;
    }
    return canpossess;
  }

  // This is the player where command.  It lists players based on room population, room flag, or playerflag. - Discordance
  _DOFUN(do_where) {
    char buf[MAX_STRING_LENGTH], arg1[MSL];
    char room_name[MSL];
    Buffer outbuf;
    Buffer outbuf2;
    Buffer outbuf3;
    CHAR_DATA *victim;
    int count = 0, i = 0;
    int rooms[100];
    int worlds[20];
    int worlds_where[20];
    int dreamers = 0;
    bool roomfound, valueset, FirstHotSpot = TRUE;

    argument = one_argument_nouncap(argument, arg1);

    for (i = 0; i < 100; i++) {
      rooms[i] = 0;
    }
    for (i = 0; i < 20; i++) {
      worlds[i] = 0;
    }
    for (i = 0; i < 20; i++) {
      worlds_where[i] = 0;
    }

    if (!strcmp(arg1, "ban")) {
      where_ban(ch, argument);
      return;
    }

    // this fills the room array; should have array sized based on amount of
    // players online instead of 100 someday
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;
      if ((d->connected != CON_PLAYING))
      continue;

      victim = d->character;
      count++;
      roomfound = FALSE;

      if (victim->in_room->vnum < 300)
      continue;

      if (is_dreaming(victim) && !IS_FLAG(victim->comm, COMM_PRIVATE)) {
        FANTASY_TYPE *fant = in_fantasy(victim);
        if (fant != NULL && dreamroom_pop(victim->pcdata->dream_room) > 2) {
          dreamers++;
          int num = fantasy_number(fant);
          bool fantfound = FALSE;
          for (i = 0; i < 20; i++) {
            if (num == worlds[i]) {
              fantfound = TRUE;
              if (IS_FLAG(victim->comm, COMM_WHEREVIS) || IS_FLAG(victim->comm, COMM_WHEREANON))
              worlds_where[i] = 1;
            }
          }
          if (fantfound == FALSE) {
            for (i = 0; i < 20; i++) {
              if (worlds[i] == 0) {
                worlds[i] = num;
                if (IS_FLAG(victim->comm, COMM_WHEREVIS) || IS_FLAG(victim->comm, COMM_WHEREANON))
                worlds_where[i] = 1;
                i = 20;
              }
            }
          }
        }
      }

      // Exceptions
      if (!can_see_where(ch, victim)) {
        continue;
      }

      for (i = 0; i < 100; i++) {
        if (victim->in_room->vnum == rooms[i]) {
          roomfound = TRUE;
        }
      }
      if (roomfound == FALSE) {
        valueset = FALSE;
        for (i = 0; i < 100; i++) {
          if (rooms[i] == 0 && valueset == FALSE) {
            rooms[i] = victim->in_room->vnum;
            valueset = TRUE;
          }
        }
      }
    }

    // hotspots
    if (display_hotspots(ch, rooms) == FALSE) {
      FirstHotSpot = FALSE;
    }

    // dreamworld population
    display_dreamers(ch, dreamers, FirstHotSpot, worlds, worlds_where);

    // where title output
    display_where_char(ch);

    bool isencounter = FALSE;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        victim = d->character;
        if (!is_gm(victim) || encounter_victim(victim) == NULL || safe_strlen(victim->pcdata->encounter_bringin) < 2 || pc_pop(victim->in_room) >= 5) {
          continue;
        }

        remove_color(room_name, roomtitle(victim->in_room, FALSE));
        if (isencounter == FALSE) {
          outbuf2.strcat("\n\r   `cEncounters`g:`x\n\r");
        }

        isencounter = TRUE;
        sprintf(buf, "%s is happening %s.\n%s with %s and %d others.\n\r", victim->pcdata->encounter_bringin, room_name, victim->name, PERS(encounter_victim(victim), ch), pc_pop(victim->in_room) - 1);
        outbuf2.strcat(buf);
      }
    }
    bool donefirst = FALSE;
    if(can_super_lure(ch, NULL))
    {

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (!d->character)
        continue;
        if ((d->connected != CON_PLAYING))
        continue;

        victim = d->character;
        if(norp_match(ch, victim))
        continue;

        if(IS_IMMORTAL(victim))
        continue;

        if(victim->pcdata->availability != AVAIL_HIGH || IS_FLAG(victim->comm, COMM_AFK))
        continue;

        if(!can_super_lure(ch, victim))
        continue;

        if(donefirst == FALSE)
        {
          sprintf(buf, "`074Supernaturally lurable characters:`x ");
          outbuf2.strcat(buf);
          donefirst = TRUE;
          sprintf(buf, "%s", victim->name);
        }
        else
        sprintf(buf, ", %s", victim->name);

        outbuf2.strcat(buf);
      }
    }
    outbuf2.strcat(".\n\r");
    if(higher_power(ch))
    {
      donefirst = FALSE;
      sprintf(buf, "`DWhisper:`x ");
      outbuf2.strcat(buf);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (!d->character)
        continue;
        if ((d->connected != CON_PLAYING))
        continue;

        victim = d->character;
        if(IS_IMMORTAL(victim))
        continue;

        if(ch->in_room != victim->in_room && power_whisper(victim, ch) && !higher_power(victim))
        {
          if(donefirst == FALSE)
          {
            sprintf(buf, "%s", victim->name);
            outbuf2.strcat(buf);
            donefirst = TRUE;
          }
          else
          {
            sprintf(buf, ", %s", victim->name);
            outbuf2.strcat(buf);
          }
        }
      }
      outbuf2.strcat(".\n\r");
      donefirst = FALSE;
      sprintf(buf, "`088Possess:`x ");
      outbuf2.strcat(buf);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (!d->character)
        continue;
        if ((d->connected != CON_PLAYING))
        continue;

        victim = d->character;
        if(IS_IMMORTAL(victim))
        continue;

        if(power_possess(ch, victim) && !higher_power(victim))
        {
          if(donefirst == FALSE)
          {
            sprintf(buf, "%s", victim->name);
            outbuf2.strcat(buf);
            donefirst = TRUE;
          }
          else
          {
            sprintf(buf, ", %s", victim->name);
            outbuf2.strcat(buf);
          }
        }
      }
      outbuf2.strcat(".\n\r");
    }

    sprintf(buf, "Looking to chat with %d people, to meet with %d people.\n\r", chat_with_count(ch), rp_with_count(ch));
    outbuf2.strcat(buf);
    page_to_char(outbuf2, ch);

    if (ch->pcdata->availability == AVAIL_NORMAL)
    printf_to_char(ch, "You have normal availability.\n\r");
    else if (ch->pcdata->availability == AVAIL_HIGH)
    printf_to_char(ch, "You have high availability.\n\r");
    else if (ch->pcdata->availability == AVAIL_LOW)
    printf_to_char(ch, "You have low availability.\n\r");

    return;
  }

  bool is_whoinvis(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_BOUNDFEET))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_WHOINVIS))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_CLOAKED))
    return TRUE;

    if (is_masked(ch))
    return TRUE;

    return FALSE;
  }

  char *mask_intro(CHAR_DATA *ch, int num) {
    if (IS_NPC(ch))
    return "A masked figure";
    if (num == 1 && safe_strlen(ch->pcdata->mask_intro_one) > 4)
    return ch->pcdata->mask_intro_one;
    if (num == 2 && safe_strlen(ch->pcdata->mask_intro_two) > 4)
    return ch->pcdata->mask_intro_two;

    return "A masked figure";
  }

  _DOFUN(do_mask) {
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (!str_cmp(arg1, "remove")) {
      if (ch->pcdata->maskednumber == 0) {
        send_to_char("You're not wearing a mask.\n\r", ch);
        return;
      }
      if (ch->pcdata->maskednumber == 1)
      ch->id -= SHAPE_MASKONE;
      if (ch->pcdata->maskednumber == 2)
      ch->id -= SHAPE_MASKTWO;
      ch->pcdata->maskednumber = 0;
      ch->pcdata->last_public_room = 0;
      act("$n takes off $s mask.", ch, NULL, NULL, TO_ROOM);
      dact("$n takes off $s mask.", ch, NULL, NULL, DISTANCE_MEDIUM);
      send_to_char("You take off your mask.\n\r", ch);
    }
    else if (!str_cmp(arg1, "intro") || !str_cmp(arg1, "introduction")) {
      if (!str_cmp(arg2, "one")) {
        free_string(ch->pcdata->mask_intro_one);
        ch->pcdata->mask_intro_one = str_dup(argument);
        printf_to_char(ch, "Your new masked intro is %s.\n\r", mask_intro(ch, 1));
        return;
      }
      else if (!str_cmp(arg2, "two")) {
        free_string(ch->pcdata->mask_intro_two);
        ch->pcdata->mask_intro_two = str_dup(argument);
        printf_to_char(ch, "Your new masked intro is %s.\n\r", mask_intro(ch, 2));
        return;
      }
      else
      send_to_char("Syntax: Mask intro One/Two (introduction)\n\r", ch);
    }
    else if (!str_cmp(arg1, "wear")) {
      if (!str_cmp(arg2, "one")) {
        if (ch->pcdata->maskednumber == 1) {
          send_to_char("You're already wearing that mask\n\r", ch);
          return;
        }
        sprintf(buf, "$n puts on a mask to become %s.", mask_intro(ch, 1));
        act(buf, ch, NULL, NULL, TO_ROOM);
        dact(buf, ch, NULL, NULL, DISTANCE_MEDIUM);
        printf_to_char(ch, "You put on a mask to become %s.", mask_intro(ch, 1));
        ch->pcdata->last_public_room = 0;
        if (ch->pcdata->maskednumber == 2)
        ch->id -= SHAPE_MASKTWO;
        ch->id += SHAPE_MASKONE;
        ch->pcdata->maskednumber = 1;
        return;
      }
      else if (!str_cmp(arg2, "two")) {
        if (ch->pcdata->maskednumber == 2) {
          send_to_char("You're already wearing that mask\n\r", ch);
          return;
        }
        sprintf(buf, "$n puts on a mask to become %s.", mask_intro(ch, 2));
        act(buf, ch, NULL, NULL, TO_ROOM);
        dact(buf, ch, NULL, NULL, DISTANCE_MEDIUM);
        printf_to_char(ch, "You put on a mask to become %s.", mask_intro(ch, 2));
        if (ch->pcdata->maskednumber == 1)
        ch->id -= SHAPE_MASKONE;
        ch->id += SHAPE_MASKTWO;
        ch->pcdata->maskednumber = 2;
        return;
      }
      else
      send_to_char("Mask wear One/Two.\n\r", ch);
    }
    else
    send_to_char("Mask Wear/Remove/Intro\n\r", ch);
  }

#define EYE_TYPES 11

  char *const genetic_eye_colors[] = {"unset",  "amber",  "blue",    "brown", "gray",   "green",  "hazel",   "red", "golden", "silver", "cerulean"};

#define HAIR_TYPES 13

  char *const genetic_hair_colors[] = {
    "unset", "black", "brown", "blond", "auburn", "chestnut", "red", "gray",  "white", "blue",  "pink",  "purple", "green"};

  void set_natural_eyes(CHAR_DATA *ch, char *argument) {
    if (!str_cmp(argument, "red")) {
      if (ch->race != RACE_OLDDEMONBORN && ch->race != RACE_DEMIDEMON && ch->race != RACE_SPIRIT_DEMON) {
        send_to_char("That eyecolor is unnatural for you.\n\r", ch);
        return;
      }
    }
    else if (ch->race == RACE_OLDDEMONBORN || ch->race == RACE_DEMIDEMON) {
      send_to_char("You can only have red eyes..\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "silver")) {
      if (ch->race != RACE_OLDDEMIGOD && ch->race != RACE_SPIRIT_DIVINE) {
        send_to_char("That eyecolor is unnatural for you.\n\r", ch);
        return;
      }
    }
    else if (ch->race == RACE_OLDDEMIGOD) {
      send_to_char("You can only have silver eyes.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "golden")) {
      if (ch->race != RACE_GODCHILD) {
        send_to_char("That eyecolor is unnatural for you.\n\r", ch);
        return;
      }
    }
    else if (ch->race == RACE_GODCHILD) {
      send_to_char("You can only have golden eyes.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "cerulean")) {
      if (ch->race != RACE_FACULTY) {
        send_to_char("That eyecolor is unnatural for you.\n\r", ch);
        return;
      }
    }

    for (int i = 1; i < EYE_TYPES; i++) {
      if (!str_cmp(argument, genetic_eye_colors[i])) {
        ch->pcdata->eyes_genetic = i;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    send_to_char("Valid colours are amber, blue, brown, gray, green, hazel.\n\r", ch);
  }
  void set_natural_hair(CHAR_DATA *ch, char *argument) {
    if (ch->race == RACE_ANCIENTWEREWOLF && str_cmp(argument, "white")) {
      send_to_char("That hair color is unnatural for you.\n\r", ch);
      return;
    }
    if (ch->race != RACE_FAECHILD && ch->race != RACE_OLDFAEBORN && ch->race != RACE_SPIRIT_FAE && (!str_cmp(argument, "green") || !str_cmp(argument, "blue") || !str_cmp(argument, "pink") || !str_cmp(argument, "purple"))) {
      send_to_char("That hair color is unnatural for you.\n\r", ch);
      return;
    }
    if (ch->race != RACE_FAECHILD && ch->race != RACE_SPIRIT_FAE && (!str_cmp(argument, "pink") || !str_cmp(argument, "purple"))) {
      send_to_char("That hair color is unnatural for you.\n\r", ch);
      return;
    }

    if (ch->race == RACE_FAECHILD && str_cmp(argument, "green") && str_cmp(argument, "blue") && str_cmp(argument, "pink") && str_cmp(argument, "purple")) {
      send_to_char("You can only have green, blue, pink or purple hair.\n\r", ch);
      return;
    }

    for (int i = 1; i < HAIR_TYPES; i++) {
      if (!str_cmp(argument, genetic_hair_colors[i])) {
        ch->pcdata->hair_genetic = i;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    send_to_char("Valid colours are black, brown, blond, auburn, chestnut, red, gray, white.\n\r", ch);
  }

  bool can_hair(CHAR_DATA *ch) {
    if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "blue") && ch->race != RACE_OLDFAEBORN && ch->race != RACE_FAECHILD && ch->race != RACE_SPIRIT_FAE)
    return FALSE;
    if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "green") && ch->race != RACE_OLDFAEBORN && ch->race != RACE_FAECHILD && ch->race != RACE_SPIRIT_FAE)
    return FALSE;
    if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "pink") && ch->race != RACE_FAECHILD && ch->race != RACE_SPIRIT_FAE)
    return FALSE;
    if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "purple") && ch->race != RACE_FAECHILD && ch->race != RACE_SPIRIT_FAE)
    return FALSE;
    if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "white") && ch->race != RACE_ANCIENTWEREWOLF)
    return FALSE;
    return TRUE;
  }
  bool can_eyes(CHAR_DATA *ch) {
    if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "red") && ch->race != RACE_OLDDEMONBORN && ch->race != RACE_DEMIDEMON && ch->race != RACE_SPIRIT_DEMON)
    return FALSE;
    if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "silver") && ch->race != RACE_OLDDEMIGOD)
    return FALSE;
    if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "golden") && ch->race != RACE_GODCHILD)
    return FALSE;
    return TRUE;
  }

  char *get_natural_eyes(CHAR_DATA *ch) {
    char buf[MSL];
    if (IS_NPC(ch))
    return "";

    if (ch->pcdata->eyes_genetic > 0 && ch->pcdata->eyes_genetic < EYE_TYPES) {
      if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "golden")) {
        sprintf(buf, "%s eyes", "`Yg`yo`Yld`ye`Yn`x");
      }
      else if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "silver")) {
        sprintf(buf, "%s eyes", "`Ws`wi`Wlv`we`Wr`x");
      }
      else if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "red")) {
        sprintf(buf, "%s eyes", "`rr`Re`rd`x");
      }
      else if (!str_cmp(genetic_eye_colors[ch->pcdata->eyes_genetic], "cerulean")) {
        sprintf(buf, "%s eyes", "`cc`Cer`cul`Cea`cn`x");
      }
      else {
        sprintf(buf, "%s eyes", genetic_eye_colors[ch->pcdata->eyes_genetic]);
      }
      return str_dup(buf);
    }

    return ch->pcdata->eyes;
  }
  char *get_natural_hair(CHAR_DATA *ch) {
    char buf[MSL];
    if (IS_NPC(ch))
    return "";

    if (ch->pcdata->hair_genetic > 0 && ch->pcdata->hair_genetic < HAIR_TYPES) {
      if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "blue")) {
        sprintf(buf, "%s hair", "`cb`Clu`ce`x");
      }
      else if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "green")) {
        sprintf(buf, "%s hair", "`gg`Gree`gn`x");
      }
      else if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "pink")) {
        sprintf(buf, "%s hair", "`Mpink`x");
      }
      else if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "purple")) {
        sprintf(buf, "%s hair", "`mpurple`x");
      }
      else if (!str_cmp(genetic_hair_colors[ch->pcdata->hair_genetic], "white")) {
        sprintf(buf, "%s hair", "`Wwhite`x");
      }
      else {
        sprintf(buf, "%s hair", genetic_hair_colors[ch->pcdata->hair_genetic]);
      }
      return str_dup(buf);
    }

    return ch->pcdata->hair;
  }

  char *get_basic_hair(CHAR_DATA *ch) {
    char buf[MSL];
    if (IS_NPC(ch))
    return "";

    if (safe_strlen(ch->pcdata->hair_dyed) > 3) {
      sprintf(buf, "%s", ch->pcdata->hair_dyed);
      return str_dup(buf);
    }
    return get_natural_hair(ch);
  }

  char *get_forensic_hair(CHAR_DATA *ch) {
    char buf[MSL];

    if (safe_strlen(ch->pcdata->hair_dyed) > 3 && ch->shape == SHAPE_HUMAN) {
      if (ch->pcdata->hair_genetic > 0 && ch->pcdata->hair_genetic < HAIR_TYPES) {
        sprintf(buf, "%s natural hair dyed %s", genetic_hair_colors[ch->pcdata->hair_genetic], ch->pcdata->hair_dyed);
        return str_dup(buf);
      }
    }
    if (ch->pcdata->hair_genetic > 0 && ch->pcdata->hair_genetic < HAIR_TYPES) {
      sprintf(buf, "%s hair", genetic_hair_colors[ch->pcdata->hair_genetic]);
      return str_dup(buf);
    }
    return ch->pcdata->hair;
  }

  char *animal_skin(CHAR_DATA *ch) { return "animal hide"; }

  bool torso_exposed(CHAR_DATA *ch) {

    if (!is_covered(ch, COVERS_UPPER_CHEST))
    return TRUE;

    if (!is_covered(ch, COVERS_BREASTS))
    return TRUE;

    if (!is_covered(ch, COVERS_LOWER_CHEST))
    return TRUE;

    if (!is_covered(ch, COVERS_UPPER_BACK))
    return TRUE;

    if (!is_covered(ch, COVERS_LOWER_BACK))
    return TRUE;

    if (!is_covered(ch, COVERS_UPPER_ARMS))
    return TRUE;

    if (!is_covered(ch, COVERS_LOWER_ARMS))
    return TRUE;

    if (!is_covered(ch, COVERS_THIGHS))
    return TRUE;

    return FALSE;
  }

  void replace_focused(CHAR_DATA *ch, int number, char *replacement) {
    int endpointer = 10000;
    int startpointer;

    char buf[MSL];
    char initstring[MSL];
    char endstring[MSL];
    char temp[MSL];
    char newstring[MSL];

    strcpy(initstring, "");
    strcpy(endstring, "");

    int i;

    if (number == -1)
    sprintf(buf, "%s", ch->description);
    else
    sprintf(buf, "%s", ch->pcdata->focused_descs[number]);
    for (i = 0; buf[i] != '\0'; i++) {
      if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\r' && buf[i] != '\0')
      endpointer = i + 1;
    }
    for (i = 0; buf[i] != '\0' && i < 1000; i++) {
      if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\r' && buf[i] != '\0' && !isdigit(buf[i])) {
        startpointer = i;
        break;
      }
    }

    for (i = 0; buf[i] != '\0'; i++) {
      //	printf_to_char(ch, "%d(%c)", i, buf[i]);
      if (i < startpointer) {
        sprintf(temp, "%c", buf[i]);
        strcat(initstring, temp);
      }
      if (i >= endpointer) {
        sprintf(temp, "%c", buf[i]);
        strcat(endstring, temp);
      }
    }
    //    printf_to_char(ch, "Rep: %d i, %d start, %d end.", i, startpointer, //    endpointer);

    sprintf(newstring, "%s%s%s", initstring, replacement, endstring);

    if (number == -1) {
      free_string(ch->description);
      ch->description = str_dup(newstring);
    }
    else {
      free_string(ch->pcdata->focused_descs[number]);
      ch->pcdata->focused_descs[number] = str_dup(newstring);
    }
  }

  void append_focused(CHAR_DATA *ch, int number, char *replacement) {
    int endpointer = 10000;
    int startpointer;

    char buf[MSL];
    char initstring[MSL];
    char endstring[MSL];
    char temp[MSL];
    char newstring[MSL];
    char midstring[MSL];

    strcpy(initstring, "");
    strcpy(endstring, "");
    strcpy(midstring, "");

    int i;

    if (number == -1)
    sprintf(buf, "%s", ch->description);
    else
    sprintf(buf, "%s", ch->pcdata->focused_descs[number]);
    for (i = 0; buf[i] != '\0'; i++) {
      if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\r' && buf[i] != '\0')
      endpointer = i + 1;
    }
    for (i = 0; buf[i] != '\0' && i < 1000; i++) {
      if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\r' && buf[i] != '\0' && !isdigit(buf[i])) {
        startpointer = i;
        break;
      }
    }

    for (i = 0; buf[i] != '\0'; i++) {
      //      printf_to_char(ch, "%d(%c)", i, buf[i]);
      if (i < startpointer) {
        sprintf(temp, "%c", buf[i]);
        strcat(initstring, temp);
      }
      if (i >= endpointer) {
        sprintf(temp, "%c", buf[i]);
        strcat(endstring, temp);
      }
      if (i >= startpointer && i < endpointer) {
        sprintf(temp, "%c", buf[i]);
        strcat(midstring, temp);
      }
    }
    //    printf_to_char(ch, "Rep: %d i, %d start, %d end.", i, startpointer, //    endpointer);

    sprintf(newstring, "%s%s %s%s", initstring, midstring, replacement, endstring);

    if (number == -1) {
      free_string(ch->description);
      ch->description = str_dup(newstring);
    }
    else {
      free_string(ch->pcdata->focused_descs[number]);
      ch->pcdata->focused_descs[number] = str_dup(newstring);
    }
  }

  int get_redesclocation(char *arg1) {
    int location = -10;
    if (!str_cmp(arg1, "me") || !str_cmp(arg1, "self"))
    location = -1;
    else if (!str_cmp(arg1, "hands"))
    location = 0;
    else if (!str_cmp(arg1, "lowerarms"))
    location = 1;
    else if (!str_cmp(arg1, "upperarms"))
    location = 2;
    else if (!str_cmp(arg1, "feet"))
    location = 3;
    else if (!str_cmp(arg1, "lowerlegs"))
    location = 4;
    else if (!str_cmp(arg1, "forehead"))
    location = 5;
    else if (!str_cmp(arg1, "thighs"))
    location = 6;
    else if (!str_cmp(arg1, "groin"))
    location = 7;
    else if (!str_cmp(arg1, "arse") || !str_cmp(arg1, "buttocks"))
    location = 8;
    else if (!str_cmp(arg1, "lowerback"))
    location = 9;
    else if (!str_cmp(arg1, "upperback"))
    location = 10;
    else if (!str_cmp(arg1, "lowerchest"))
    location = 11;
    else if (!str_cmp(arg1, "breasts"))
    location = 12;
    else if (!str_cmp(arg1, "upperchest"))
    location = 13;
    else if (!str_cmp(arg1, "neck"))
    location = 14;
    else if (!str_cmp(arg1, "lowerface"))
    location = 15;
    else if (!str_cmp(arg1, "hair"))
    location = 16;
    else if (!str_cmp(arg1, "eyes"))
    location = 17;

    return location;
  }

  _DOFUN(do_focusreplace) {

    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    replace_focused(ch, atoi(arg1), argument);
  }

  char *get_intro(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return str_dup("Someone");

    if (is_cloaked(ch))
    return str_dup("A blurry shape");

    if (is_masked(ch))
    return str_dup(mask_intro(ch, ch->pcdata->maskednumber));

    if (ch->shape == SHAPE_MERMAID && safe_strlen(ch->pcdata->mermaidintro) > 3)
    return str_dup(ch->pcdata->mermaidintro);
    ;

    if (ch->shape == SHAPE_WOLF && safe_strlen(ch->pcdata->wolfintro) > 3)
    return str_dup(ch->pcdata->wolfintro);
    else if (ch->shape == SHAPE_WOLF)
    return str_dup("An unnaturally large wolf");

    if (ch->shape >= SHAPE_ANIMALONE && ch->shape <= SHAPE_ANIMALSIX) {
      if (safe_strlen(ch->pcdata->animal_intros[ch->shape - SHAPE_ANIMALONE]) > 3)
      return str_dup(ch->pcdata->animal_intros[ch->shape - SHAPE_ANIMALONE]);
      return str_dup("An animal");
    }
    if (ch->pcdata->intro_desc == NULL || safe_strlen(ch->pcdata->intro_desc) < 2) {
      if (ch->sex == SEX_MALE) {
        free_string(ch->pcdata->intro_desc);
        ch->pcdata->intro_desc = str_dup("A man");
      }
      else {
        free_string(ch->pcdata->intro_desc);
        ch->pcdata->intro_desc = str_dup("A woman");
      }
      return str_dup("Someone");
    }

    return str_dup(ch->pcdata->intro_desc);
  }

  int vision_range_character(CHAR_DATA *ch, CHAR_DATA *victim) {
    int range = 30;
    ROOM_INDEX_DATA *finish = victim->in_room;
    if (IS_NPC(ch))
    return range;
    if (is_dark(finish) && !can_see_dark(ch))
    return 0;
    int sun = sunphase(finish);
    if (light_level(ch->in_room) +
        skillpoint(get_skill(ch, SKILL_NIGHTVISION)) * 5 <
        50)
    range = range * 2 / 3;
    if (sun == 4) // Noon
    {
      int shade = shade_level(finish) + shade_level(ch->in_room);
      if (is_covered(ch, COVERS_EYES))
      shade++;
      if (is_vampire(ch)) {
        range = range * UMIN(shade, 6) / 6;
      }
      else if (get_skill(ch, SKILL_NIGHTVISION) > 0) {
        shade += 2;
        range = range * UMIN(shade, 6) / 6;
        ;
      }
      else if (!IS_SET(finish->room_flags, ROOM_INDOORS))
      range = range * 12 / 10;
    }
    if (sun == 3 || sun == 5) // Afternoon.
    {
      int shade = shade_level(finish) + shade_level(ch->in_room);
      if (is_covered(ch, COVERS_EYES))
      shade++;
      if (get_skill(ch, SKILL_NIGHTVISION) > 0) {
        shade += 3;
        range = range * UMIN(shade, 6) / 6;
        ;
      }
    }
    if (get_skill(ch, SKILL_ACUTESIGHT) > 0) {
      if (range > 20)
      range = range * 12 / 10;
    }
    range += get_skill(ch, SKILL_PERCEPTION);
    if (is_animal(victim)) {
      if (animal_size(get_animal_weight(victim, ANIMAL_ACTIVE)) <= ANIMAL_TINY)
      range /= 10;
      else if (animal_size(get_animal_weight(victim, ANIMAL_ACTIVE)) <=
          ANIMAL_SMALL)
      range /= 3;
      else if (animal_size(get_animal_weight(victim, ANIMAL_ACTIVE)) >=
          ANIMAL_VLARGE)
      range = range * 3 / 2;
      else if (animal_size(get_animal_weight(victim, ANIMAL_ACTIVE)) >=
          ANIMAL_LARGE)
      range = range * 12 / 10;
      else if (animal_size(get_animal_weight(victim, ANIMAL_ACTIVE)) >=
          ANIMAL_LARGE)
      range = range * 4;
    }
    if (in_fight(ch)) {
      range = UMAX(range, 8);
      range = UMIN(range, 20);
    }
    if (IS_NPC(victim) && IS_FLAG(victim->act, ACT_SENTINEL))
    range /= 4;

    return range;
  }

  int vision_range_room(CHAR_DATA *ch, ROOM_INDEX_DATA *finish) {
    int range = 30;
    if (IS_NPC(ch))
    return range;

    if (is_dark(finish) && !can_see_dark(ch))
    return 0;
    int sun = sunphase(finish);
    if (light_level(ch->in_room) +
        skillpoint(get_skill(ch, SKILL_NIGHTVISION)) * 5 <
        50)
    if (sun == 4) // Noon
    {
      int shade = shade_level(finish) + shade_level(ch->in_room);
      if (is_covered(ch, COVERS_EYES))
      shade++;
      if (is_vampire(ch)) {
        range = range * UMIN(shade, 6) / 6;
      }
      else if (get_skill(ch, SKILL_NIGHTVISION) > 0) {
        shade += 2;
        range = range * UMIN(shade, 6) / 6;
        ;
      }
      else if (!IS_SET(finish->room_flags, ROOM_INDOORS))
      range = range * 12 / 10;
    }
    if (sun == 3 || sun == 5) // Afternoon.
    {
      int shade = shade_level(finish) + shade_level(ch->in_room);
      if (is_covered(ch, COVERS_EYES))
      shade++;
      if (get_skill(ch, SKILL_NIGHTVISION) > 0) {
        shade += 3;
        range = range * UMIN(shade, 6) / 6;
        ;
      }
    }
    if (get_skill(ch, SKILL_ACUTESIGHT) > 0) {
      if (range > 20)
      range = range * 12 / 10;
    }
    range += get_skill(ch, SKILL_PERCEPTION);
    return range;
  }

  int room_level(ROOM_INDEX_DATA *room) {

    if (room->sector_type != SECT_FOREST)
    return 0;
    if (prop_from_room(room) != NULL)
    return 0;

    if (room->area->vnum == OUTER_NORTH_FOREST || room->area->vnum == OUTER_WEST_FOREST || room->area->vnum == OUTER_SOUTH_FOREST) {
      int dist = distance_from_town(room);
      dist -= 29;
      if (dist <= 0)
      return 0;
      dist = UMIN(dist, 45);
      dist *= 15;
      dist = UMAX(20, dist - 25);
      return dist;
    }
    if (room->area->vnum == OTHER_FOREST_VNUM || room->area->vnum == GODREALM_FOREST_VNUM) {
      int dist = distance_from_town(room);
      if (dist <= 0)
      return 0;
      dist = UMIN(dist, 45);
      dist *= 15;
      dist = UMAX(20, dist - 25);
      return dist;
    }
    if (room->area->vnum == WILDS_FOREST_VNUM) {
      int dist = distance_from_town(room);
      if (dist <= 0)
      return 0;
      dist = dist / 3;
      dist = UMIN(dist, 45);
      dist *= 15;
      dist = UMAX(20, dist - 25);
      return dist;
    }
    if (room->area->vnum == HELL_FOREST_VNUM) {
      int dist = distance_from_town(room);
      if (dist <= 0)
      return 0;
      dist *= 4;
      dist = UMIN(dist, 45);
      dist *= 15;
      dist = UMAX(20, dist - 25);
      return dist;
    }
    if (mist_level(room) >= 3) {
      return 60;
    }
    return 0;
  }

  _DOFUN(do_friend) {
    char arg[MSL];
    argument = one_argument(argument, arg);

    if (!str_cmp(arg, "list")) {
      for (int i = 0; i < 25; i++) {
        if (ch->pcdata->account->friend_type[i] == 1 || ch->pcdata->account->friend_type[i] == 3)
        printf_to_char(ch, "Account: %s\n\r", ch->pcdata->account->friends[i]);
        if (ch->pcdata->account->friend_type[i] == 2 || ch->pcdata->account->friend_type[i] == 4)
        printf_to_char(ch, "Character: %s\n\r", ch->pcdata->account->friends[i]);
      }
      return;
    }
    else if (!str_cmp(arg, "add")) {
      char arg2[MSL];
      argument = one_argument(argument, arg2);
      if (!str_cmp(arg2, "character")) {
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: friend add character (name)\n\r", ch);
          return;
        }
        for (int i = 0; i < 25; i++) {
          if (ch->pcdata->account->friend_type[i] == 0) {
            ch->pcdata->account->friend_type[i] = 4;
            free_string(ch->pcdata->account->friends[i]);
            ch->pcdata->account->friends[i] = str_dup(argument);
            send_to_char("Friend added.\n\r", ch);
            return;
          }
        }
        send_to_char("You already have too many friends.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "account")) {
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: friend add account (name)\n\r", ch);
          return;
        }
        for (int i = 0; i < 25; i++) {
          if (ch->pcdata->account->friend_type[i] == 0) {
            ch->pcdata->account->friend_type[i] = 3;
            free_string(ch->pcdata->account->friends[i]);
            ch->pcdata->account->friends[i] = str_dup(argument);
            send_to_char("Friend added.\n\r", ch);
            return;
          }
        }
        send_to_char("You already have too many friends.\n\r", ch);
        return;
      }
      send_to_char("Syntax: friend add account/character (name)\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "remove")) {
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: Friend remove (name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 25; i++) {
        if (!str_cmp(argument, ch->pcdata->account->friends[i])) {
          free_string(ch->pcdata->account->friends[i]);
          ch->pcdata->account->friends[i] = str_dup("");
          ch->pcdata->account->friend_type[i] = 0;
          send_to_char("Friend removed :(\n\r", ch);
          return;
        }
      }
      return;
    }
    if (!str_cmp(arg, "notify")) {
      if (!IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTIFY)) {
        SET_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTIFY);
        send_to_char("Your friends will now be notified when you log on.\n\r", ch);
        return;
      }
      else {
        REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTIFY);
        send_to_char("Your friends will no longer be notified when you log on.\n\r", ch);
        return;
      }
      return;
    }
    send_to_char("Syntax: Friend add/remove/notify/list\n\r", ch);
  }

  bool one_way_friend(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || ch->pcdata->account == NULL)
    return FALSE;
    if (IS_NPC(victim) || victim->pcdata->account == NULL)
    return FALSE;

    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->pcdata->account->friends[i], victim->name) && ch->pcdata->account->friend_type[i] == 4) {
        for (int x = 0; x < 25; x++) {
          if (!str_cmp(victim->pcdata->account->friends[x], ch->name) && (victim->pcdata->account->friend_type[x] == 4 || victim->pcdata->account->friend_type[x] == 2)) {
            ch->pcdata->account->friend_type[i] = 2;
            victim->pcdata->account->friend_type[x] = 2;
          }
          if (!str_cmp(victim->pcdata->account->friends[x], ch->pcdata->account->name) && (victim->pcdata->account->friend_type[x] == 1 || victim->pcdata->account->friend_type[x] == 3)) {
            ch->pcdata->account->friend_type[i] = 2;
            victim->pcdata->account->friend_type[x] = 1;
          }
        }
      }
      if (!str_cmp(ch->pcdata->account->friends[i], victim->pcdata->account->name) && ch->pcdata->account->friend_type[i] == 3) {
        for (int x = 0; x < 25; x++) {
          if (!str_cmp(victim->pcdata->account->friends[x], ch->name) && (victim->pcdata->account->friend_type[x] == 4 || victim->pcdata->account->friend_type[x] == 2)) {
            ch->pcdata->account->friend_type[i] = 1;
            victim->pcdata->account->friend_type[x] = 2;
          }
          if (!str_cmp(victim->pcdata->account->friends[x], ch->pcdata->account->name) && (victim->pcdata->account->friend_type[x] == 1 || victim->pcdata->account->friend_type[x] == 3)) {
            ch->pcdata->account->friend_type[i] = 1;
            victim->pcdata->account->friend_type[x] = 1;
          }
        }
      }
      if (!str_cmp(ch->pcdata->account->friends[i], victim->name) && ch->pcdata->account->friend_type[i] == 2)
      return TRUE;
      if (!str_cmp(ch->pcdata->account->friends[i], victim->pcdata->account->name) && ch->pcdata->account->friend_type[i] == 1)
      return TRUE;
    }
    return FALSE;
  }

  bool are_friends(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (one_way_friend(ch, victim) && one_way_friend(victim, ch))
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_setroomtitle) {
    EXTRA_DESCR_DATA *ed;
    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!roomtitle", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("!roomtitle");
      ed->next = ch->in_room->extra_descr;
      ch->in_room->extra_descr = ed;
      free_string(ch->in_room->extra_descr->description);
      ch->in_room->extra_descr->description = str_dup(argument);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(argument);
    }
    if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  }

  char *first_color_code(char *txt) {
    char buf[MSL];
    char intxt[MSL];
    int txtlen, inp;
    if (txt) {
      strcpy(intxt, txt);
      txtlen = safe_strlen(intxt);
      for (inp = 0; inp < txtlen; inp++) {
        if (intxt[inp] == '`') {
          if (isdigit(intxt[inp + 1]) && isdigit(intxt[inp + 2]) && isdigit(intxt[inp + 3])) {
            sprintf(buf, "`%c%c%c", intxt[inp + 1], intxt[inp + 2], intxt[inp + 3]);
            return str_dup(buf);
          }
          if (intxt[inp + 1] == '#') {
            if (!intxt[inp + 2] || intxt[inp + 2] == ' ' || intxt[inp + 3] == '\0')
            return "";
            if (!intxt[inp + 3] || intxt[inp + 3] == ' ' || intxt[inp + 3] == '\0')
            return "";
            if (!intxt[inp + 4] || intxt[inp + 4] == ' ' || intxt[inp + 4] == '\0')
            return "";
            if (!intxt[inp + 5] || intxt[inp + 5] == ' ' || intxt[inp + 5] == '\0')
            return "";
            if (!intxt[inp + 6] || intxt[inp + 6] == ' ' || intxt[inp + 6] == '\0')
            return "";
            if (!intxt[inp + 7] || intxt[inp + 7] == ' ' || intxt[inp + 7] == '\0')
            return "";
            sprintf(buf, "`#%c%c%c%c%c%c", intxt[inp + 2], intxt[inp + 3], intxt[inp + 4], intxt[inp + 5], intxt[inp + 6], intxt[inp + 7]);
            return str_dup(buf);
          }
          sprintf(buf, "`%c", intxt[inp + 1]);
          return str_dup(buf);
        }
      }
    }
    return "";
  }

#define PREP_NO -1
#define PREP_AT 0
#define PREP_IN 1
#define PREP_ON 2

  char *roomtitle(ROOM_INDEX_DATA *room, bool capital) {
    EXTRA_DESCR_DATA *ed;
    char buf[MSL];
    int firstletter = -1;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("!roomtitle", ed->keyword))
      break;
    }
    if (ed) {
      sprintf(buf, "%s", ed->description);
      for (int i = 0; buf[i] != '\0' && firstletter == -1; i++) {
        if (buf[i] != '`' && buf[i] != ' ' && (i == 0 || buf[i - 1] != '`')) {
          firstletter = i;
          break;
        }
      }
      if (capital == TRUE)
      buf[firstletter] = UPPER(buf[firstletter]);
      else
      buf[firstletter] = LOWER(buf[firstletter]);
      return str_dup(buf);
    }
    int word = PREP_AT;
    char no_color[MSL];
    char preposition[MSL];
    char firstword[MSL];
    remove_color(no_color, room->name);
    one_argument_nouncap(no_color, firstword);
    if (!str_cmp(firstword, "Before") || !str_cmp(firstword, "Amidst")) {
      word = PREP_NO;
    }
    else if (room->area->vnum == 30)
    word = PREP_AT;
    else if (room->sector_type == SECT_STREET) {
      if (strcasestr(no_color, " and ")) {
        word = PREP_AT;
      }
      else {
        word = PREP_ON;
      }
    }
    else if (room->sector_type == SECT_WATER || room->sector_type == SECT_UNDERWATER || room->sector_type == SECT_FOREST)
    word = PREP_IN;
    else if (IS_SET(room->room_flags, ROOM_INDOORS)) {
      if (strcasestr(no_color, "bar"))
      word = PREP_AT;
      else if (strcasestr(no_color, "pool table"))
      word = PREP_AT;
      else
      word = PREP_IN;
    }

    if (word == PREP_IN)
    strcpy(preposition, "In");
    else if (word == PREP_ON)
    strcpy(preposition, "On");
    else if (word == PREP_AT)
    strcpy(preposition, "At");
    else if (word != PREP_NO)
    strcpy(preposition, "At");

    if (capital == FALSE)
    preposition[0] = LOWER(preposition[0]);

    char name[MSL];
    sprintf(name, "%s", room->name);

    if (word == PREP_NO)
    sprintf(buf, "`x%s", name);
    else if (name[0] == '`')
    sprintf(buf, "%s%s %s", first_color_code(name), preposition, name);
    else
    sprintf(buf, "`x%s %s", preposition, name);

    return str_dup(buf);
  }

  void habit_prompt(CHAR_DATA *ch) {

    int habit = number_range(HABIT_FEEDING, HABIT_SELFESTEEM);
    int level = ch->pcdata->habit[habit];
    if (habit == HABIT_EATING) {
      if (level == 1) {
        send_to_char("RP Prompt: When was the last time your character tried cooking for themselves, and what happened?\n\r", ch);
        return;
      }
      if (level == 2) {
        send_to_char("RP Prompt: What was the last unhealthy thing your character wanted to eat, did they?\n\r", ch);
        return;
      }
      if (level == 3) {
        if (number_percent() % 2 == 0)
        send_to_char("RP Prompt: What was the last unhealthy thing your character wanted to eat, did they?\n\r", ch);
        else
        send_to_char("RP Prompt: What is your character's goal weight?\n\r", ch);
      }
    }
    if (habit == HABIT_FEEDING && is_vampire(ch)) {
      if (level == 1) {
        send_to_char("RP Prompt: When was the last time your character had human blood, what happened?\n\r", ch);
        return;
      }
      send_to_char("RP Prompt: Who was the last person your character killed while feeding? What happened.\n\r", ch);
      return;
    }
    if (habit == HABIT_LUNACY && is_werewolf(ch)) {
      send_to_char("RP Prompt: What happened the last time your character broke free on a full moon?\n\r", ch);
      return;
    }
    if (habit == HABIT_SMOKING && level > 0) {
      if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What is your character's prefered brand of cigarettes?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: When was the first time your character smoked?\n\r", ch);
      else
      send_to_char("RP Prompt: Has your character ever tried to give up cigarettes? What happened?\n\r", ch);
      return;
    }
    if (habit == HABIT_DRINKING && level > 0) {
      if (number_percent() % 4 == 0) {
        send_to_char("RP Prompt: What is your character's prefered type of alcohol?\n\r", ch);
      }
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What is the most foolish thing your character has done while drunk?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: When was the last time your character got so drunk they can't properly remember the night?\n\r", ch);
      else
      send_to_char("RP Prompt: When was the first time your character got drunk?\n\r", ch);
      return;
    }
    if (habit == HABIT_DRUGS && level > 0) {
      if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: What is your characters favorite type of high?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: Who is your character's dealer? What are they like?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What was your character's best drug experience?\n\r", ch);
      else
      send_to_char("RP prompt: What was your character's worst drug experience?\n\r", ch);
      return;
    }
    if (habit == HABIT_SADISM && is_demonborn(ch)) {
      if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What was the last cruel thing your character did to someone else?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What was the cruelest thing your character has done to someone?\n\r", ch);
      else
      send_to_char("RP Prompt: What was the hardest sadistic urge to repress your character hasn't indulged?\n\r", ch);
      return;
    }
    if (habit == HABIT_SEX && level > 0) {
      if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: How did your character lose their virginity?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What was your characters worst sexual experience?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What was your character's best sexual experience?\n\r", ch);
      else
      send_to_char("RP Prompt: What was your character's strangest sexual experience?\n\r", ch);
      return;
    }
    if (habit == HABIT_GYM && level > 0) {
      if (number_percent() % 5 == 0)
      send_to_char("RP Prompt: When does your character normally attend the gym, and for how long?\n\r", ch);
      else if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: Does your character normally do resistance training, cardiovascular training or both at the gym?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What is your character's favorite exercise to do at the gym? Why do they like it?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What is your character's least favorite exercise to do at the gym? Why?\n\r", ch);
      else
      send_to_char("RP Prompt: What is the strangest thing your character has seen at the gym?\n\r", ch);
      return;
    }
    if (habit == HABIT_GAMES && level > 0) {
      if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: What is your character's favorite game from their childhood?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What is your character's prefered genre of game?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What is the game your character is absolutely the best at?\n\r", ch);
      else
      send_to_char("What is the worst your character has ever been beaten in a game?\n\r", ch);
      return;
    }
    if (habit == HABIT_COMICS && level > 0) {
      if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: What was your character's favorite comic when they were a child?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What is your character's favorite comic book hero?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What is your character's favorite comic book villain?\n\r", ch);
      else
      send_to_char("RP Prompt: What comic book character did your character have a crush on?\n\r", ch);
      return;
    }
    if (habit == HABIT_TV && level > 0) {
      if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What was your character's favorite tv show when they were a child?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What genre of TV show does your character most enjoy?\n\r", ch);
      else
      send_to_char("RP Prompt: Which TV character did your PC have their biggest crush on?\n\r", ch);
      return;
    }
    if (habit == HABIT_NOVELS && level > 0) {
      if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: What was your character's favorite novel when they were a child?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What genre of novels does your character most enjoy?\n\r", ch);
      else
      send_to_char("RP Prompt: What character from a novel did your character most want to be?\n\r", ch);
      return;
    }
    if (habit == HABIT_CLUBBING && level > 0) {
      if (number_percent() % 5 == 0)
      send_to_char("RP Prompt: What kind of music does your character prefer at clubs?\n\r", ch);
      else if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: How much does your character like to dance at clubs?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: Has your character ever hooked up with someone at a club? What happened?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: When was the last time your character went to a club? How late out did they stay and did it cause problems the next morning?\n\r", ch);
      else
      send_to_char("RP Prompt: What is the strangest thing your character has seen in a club.\n\r", ch);
      return;
    }
    if (habit == HABIT_SPORTS && level > 0) {
      if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What kind of sports does your character enjoy playing?\n\r", ch);
      else
      send_to_char("RP Prompt: Has your character ever been injured playing sports? What happened?\n\r", ch);
    }
    if (habit == HABIT_ORIENTATION) {
      if ((ch->sex == SEX_MALE && (level == 0 || level == 2)) || (ch->sex == SEX_FEMALE && (level == 1 || level == 2))) {
        if (number_percent() % 7 == 0)
        send_to_char("RP Prompt: Who is your character's celebrity crush?\n\r", ch);
        else if (number_percent() % 6 == 0)
        send_to_char("RP Prompt: Does your character prefer blondes, brunettes or redheads?\n\r", ch);
        else if (number_percent() % 5 == 0)
        send_to_char("RP Prompt: Who is the ex your character is the most hung up on?\n\r", ch);
        else if (number_percent() % 4 == 0)
        send_to_char("RP Prompt: What was your character's hardest break up?\n\r", ch);
        else if (number_percent() % 3 == 0)
        send_to_char("RP Prompt: When was the last time your character had their heart broken?\n\r", ch);
        else if (number_percent() % 2 == 0)
        send_to_char("RP Prompt: Tits, legs or ass?\n\r", ch);
      }
      if ((ch->sex == SEX_FEMALE && (level == 0 || level == 2)) || (ch->sex == SEX_MALE && (level == 1 || level == 2))) {
        if (number_percent() % 7 == 0)
        send_to_char("RP Prompt: Who is your character's celebrity crush?\n\r", ch);
        else if (number_percent() % 6 == 0)
        send_to_char("RP Prompt: What is the most attractive way for a man to dress?\n\r", ch);
        else if (number_percent() % 5 == 0)
        send_to_char("RP Prompt: How short is too short for your character?\n\r", ch);
        else if (number_percent() % 4 == 0)
        send_to_char("RP Prompt: Who is the ex your character is the most hung up on?\n\r", ch);
        else if (number_percent() % 3 == 0)
        send_to_char("RP Prompt: What was your character's hardest break up?\n\r", ch);
        else if (number_percent() % 2 == 0)
        send_to_char("RP Prompt: When was the last time your character had their heart broken?\n\r", ch);
        else
        send_to_char("RP Prompt: Does your character prefer outdoorsy types, or city types?\n\r", ch);
      }
    }
  }

  void learned_prompt(CHAR_DATA *ch) {
    int value = number_range(0, DIS_USED);
    if (ch->disciplines[value] > 0) {
      for (int i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == value) {

          if (number_percent() % 3 == 0)
          printf_to_char(
          ch, "RP Prompt: How did your character come to learn %s\n\r?", discipline_table[i].name);
          else if (number_percent() % 2 == 0)
          printf_to_char(ch, "RP Prompt: What does your character do ICly to keep proficient at %s?\n\r", discipline_table[i].name);
          else
          printf_to_char(ch, "RP Prompt: When was the last time your character practiced %s?\n\r", discipline_table[i].name);
          return;
        }
      }
    }
    value = number_range(0, SKILL_USED);
    if (ch->skills[value] > 0) {
      for (int i = 0; i < SKILL_USED; i++) {
        if (skill_table[i].vnum == value) {

          if (number_percent() % 3 == 0)
          printf_to_char(
          ch, "RP Prompt: How did your character come to learn %s\n\r?", skill_table[i].name);
          else if (number_percent() % 2 == 0)
          printf_to_char(ch, "RP Prompt: What does your character do ICly to keep proficient at %s?\n\r", skill_table[i].name);
          else
          printf_to_char(ch, "RP Prompt: When was the last time your character practiced %s?\n\r", skill_table[i].name);
          return;
        }
      }
    }
  }

  void learning_prompt(CHAR_DATA *ch) {
    if (number_percent() % 2 == 0 && ch->pcdata->training_disc != 0) {
      for (int i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == ch->pcdata->training_disc) {
          if (number_percent() % 7 == 0)
          printf_to_char(
          ch, "RP Prompt: What motivates your character to improve at %s?\n\r", discipline_table[i].name);
          else if (number_percent() % 6 == 0)
          printf_to_char(ch, "RP Prompt: What was the last thing your character did to improve at %s?\n\r", discipline_table[i].name);
          else if (number_percent() % 5 == 0)
          printf_to_char(ch, "RP Prompt: When was the last time your character noticed their improvement at %s?\n\r", discipline_table[i].name);
          else if (number_percent() % 4 == 0)
          printf_to_char(ch, "RP Prompt: What was your character's last frustration while trying to improve %s?\n\r", discipline_table[i].name);
          else
          printf_to_char(ch, "RP Prompt: What is your character doing ICly to improve at %s?\n\r", discipline_table[i].name);
          return;
        }
      }
    }
    /*
    if(ch->pcdata->training_stat != 0)
    {
    for(int i=0;i<SKILL_USED;i++)
    {
    if(skill_table[i].vnum == ch->pcdata->training_stat)
    {
    if(number_percent() % 7 == 0)
    printf_to_char(ch, "RP Prompt: What motivates your character to improve at
    %s?\n\r", skill_table[i].name); else if(number_percent() % 6 == 0)
    printf_to_char(ch, "RP Prompt: What was the last thing your character did to
    improve at %s?\n\r", skill_table[i].name); else if(number_percent() % 5 ==
    0) printf_to_char(ch, "RP Prompt: When was the last time your character
    noticed their improvement at %s?\n\r", skill_table[i].name); else
    if(number_percent() % 4 == 0) printf_to_char(ch, "RP Prompt: What was your
    character's last frustration while trying to improve %s?\n\r", skill_table[i].name);
    else
    printf_to_char(ch, "RP Prompt: What is your character doing ICly to improve
    at %s?\n\r", skill_table[i].name); return;

    }
    }
    }
    */
  }

  void tier_prompt(CHAR_DATA *ch) {
    if (get_tier(ch) >= 3) {
      if (number_percent() % 6 == 0)
      send_to_char("RP Prompt: Who has your character stepped on to get to where they are now?\n\r", ch);
      else if (number_percent() % 5 == 0)
      send_to_char("RP Prompt: What is the most pronounced way your character is no longer like normal humans?\n\r", ch);
      else if (number_percent() % 4 == 0)
      send_to_char("RP Prompt: What immoral deals has your character made to get to where they are now?\n\r", ch);
      else if (number_percent() % 3 == 0)
      send_to_char("RP Prompt: Who has your character sacrificed to get to where they are now?\n\r", ch);
      else if (number_percent() % 2 == 0)
      send_to_char("RP Prompt: What is the most despicable act your character has done to get to where they are now?\n\r", ch);
      else
      send_to_char("RP Prompt: What is the most human part of your character still remaining?\n\r", ch);
    }
  }

  void rp_prompt(CHAR_DATA *ch) {
    switch (number_percent() % 11) {
    case 0:
    case 10:
      tier_prompt(ch);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
      learning_prompt(ch);
      break;
    case 5:
    case 6:
    case 7:
      learned_prompt(ch);
      break;
    case 8:
    case 9:
      habit_prompt(ch);
      break;
    }
  }

  void social_behave_mod(CHAR_DATA *ch, int amount, char *message) {
    ch->pcdata->social_behave += amount;
    char buf[MSL];
    sprintf(buf, "INFLUENCE BEHAVE: %s gets given %d for %s.", ch->name, amount, message);
    log_string(buf);
  }

  char *newtexttime() {
    time_t east_time;
    east_time = current_time;
    char tmp[MSL];
    char datestr[MSL];
    sprintf(tmp, "%s", (char *)ctime(&east_time));

    sprintf(datestr, "%c%c%c %c%c%c%c%c%c%c%c", tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[10], tmp[11], tmp[12], tmp[13], tmp[14], tmp[15]);
    return str_dup(datestr);
  }

  char *get_default_dreamdesc(CHAR_DATA *victim) {
    int i, j, len;
    int min = 2000;
    int lastmin = -2000;
    char string[MSL];
    string[0] = '\0';
    char buf[MSL];

    if (IS_NPC(victim)) {
      return victim->description;
    }

    for (j = 0; j < 20; j++) {
      if (victim->pcdata->focused_order[COVERS_ALL] < min && victim->pcdata->focused_order[COVERS_ALL] > lastmin)
      min = victim->pcdata->focused_order[COVERS_ALL];
      for (i = 0; i < MAX_COVERS + 1; i++) {
        if (cover_table[i] != COVERS_EYES && cover_table[i] != COVERS_HAIR && cover_table[i] != COVERS_LOWER_FACE && cover_table[i] != COVERS_NECK && cover_table[i] != COVERS_FOREHEAD && cover_table[i] != COVERS_HANDS)
        continue;
        if (victim->pcdata->focused_order[i] < min && victim->pcdata->focused_order[i] > lastmin)
        min = victim->pcdata->focused_order[i];
      }

      if (victim->pcdata->focused_order[COVERS_ALL] == min && str_cmp(victim->description, "") && safe_strlen(victim->description) > 0) {
        sprintf(buf, "%s", victim->description);
        len = safe_strlen(buf);
        if (buf[len - 2] == '\n')
        buf[len - 2] = 0;
        strcat(string, buf);
      }

      for (i = 0; i < MAX_COVERS + 1; i++) {
        if (cover_table[i] != COVERS_EYES && cover_table[i] != COVERS_HAIR && cover_table[i] != COVERS_LOWER_FACE && cover_table[i] != COVERS_NECK && cover_table[i] != COVERS_FOREHEAD && cover_table[i] != COVERS_HANDS)
        continue;

        if (i != COVERS_SMELL) {
          if (victim->pcdata->focused_order[i] == min && str_cmp(victim->pcdata->focused_descs[i], "") && (!is_covered(victim, cover_table[i]))) {
            sprintf(buf, "%s", victim->pcdata->focused_descs[i]);
            len = safe_strlen(buf);
            if (buf[len - 2] == '\n')
            buf[len - 2] = 0;
            strcat(string, buf);
          }
        }
      }
      lastmin = min;
      min = 2000;
    }
    char *result = str_dup(string);
    return result;
  }

  _DOFUN(do_moontest) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];

    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    int point = moon_pointer(atoi(arg1), atoi(arg2), atoi(arg3), NULL);
    printf_to_char(ch, "%d: %s", point, moon_real[point]);
    printf_to_char(ch, "%d: %d :%d ::: %d: %d: %d\n\r", atoi(arg1), atoi(arg2), atoi(arg3), ptm->tm_mday, ptm->tm_mon, ptm->tm_year);
  }

  char *strip_newlines(char *text) {
    char fintext[MSL];
    char tmp[MSL];
    strcpy(fintext, "");
    int inp;
    int txtlen = safe_strlen(text);
    for (inp = 0; inp < txtlen; inp++) {
      if (text[inp] != '\n' && text[inp] != '\r') {
        sprintf(tmp, "%c", text[inp]);
        strcat(fintext, tmp);
      }
    }
    return str_dup(fintext);
  }

  char *line_to_box(char *line, char *delim, int width) {
    char buf[MSL];
    //    sprintf(lbuf, "LOGSTRING:%s:%d:%d:%d", line, safe_strlen_color(line), //    safe_strlen_color(delim)*2, width); log_string(lbuf);
    if (safe_strlen_color(line) + safe_strlen_color(delim) * 2 <= width) {

      int space = width - safe_strlen_color(delim) * 2 - safe_strlen_color(line);
      sprintf(buf, "%s%s%*s%s", delim, line, space, " ", delim);

    }
    else
    sprintf(buf, "%s%s", delim, line);
    return str_dup(buf);
  }
  char *text_block_to_box(char *text, char *delim, int width) {
    char tmp[MSL];
    char fintext[MSL];
    char otext[MSL];
    strcpy(otext, "");
    strcpy(fintext, "");
    int inp;
    int txtlen = safe_strlen(text);
    for (inp = 0; inp < txtlen; inp++) {
      if (text[inp] == '\n') {
        strcat(fintext, line_to_box(strip_newlines(otext), delim, width));
        strcat(fintext, "\n");
        strcpy(otext, "");
      }
      else {
        sprintf(tmp, "%c", text[inp]);
        strcat(otext, tmp);
      }
    }
    if (safe_strlen(otext) > 2) {
      strcat(fintext, line_to_box(otext, delim, width));
    }
    return str_dup(fintext);
  }
  char *text_complete_box(char *text, char *delim, int width, bool cuttop) {
    char otext[MSL];
    strcpy(otext, "");
    if (cuttop == TRUE) {
      strcat(otext, " ");
      for (int i = 0; i < width - 2; i++)
      strcat(otext, delim);
      strcat(otext, " ");
    }
    else {
      for (int i = 0; i < width; i++)
      strcat(otext, delim);
    }
    char fintext[MSL];
    sprintf(fintext, "%s\n%s\n%s\n\r", otext, text_block_to_box(text, delim, width), otext);
    return str_dup(fintext);
  }

  int moonlight_level(ROOM_INDEX_DATA *room) {

    if (supernatural_darkness == 1 && in_haven(room))
    return 0;

    int val = 0;
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    int mpoint = moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL);
    if (mpoint == 4)
    val = 100;
    else if (mpoint == 3 || mpoint == 5)
    val = 75;
    else if (mpoint == 2 || mpoint == 6)
    val = 50;
    else if (mpoint == 1 || mpoint == 7)
    val = 20;
    else
    val = 0;

    int ccover = cloud_cover(room);
    double percentage = ccover / 150.0;
    double reduction = val * percentage;
    int result = val - reduction;
    return result;
  }

  int daylight_level(ROOM_INDEX_DATA *room) {
    int val = 0;
    int dpoint = sunphase(room);
    if (dpoint == 3 || dpoint == 4)
    val = 100;
    else if (dpoint == 2 || dpoint == 5)
    val = 75;
    else if (dpoint == 2 || dpoint == 6)
    val = 50;
    else if (dpoint == 1)
    val = 20;
    else
    val = 0;

    int ccover = cloud_cover(room);
    double percentage = ccover / 200.0;
    double reduction = val * percentage;
    int result = val - reduction;
    return result;
  }

  int torch_count(ROOM_INDEX_DATA *room)
  {

    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int count = 0;

    for (CharList::iterator it = room->people->begin();
    it != room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != room)
      continue;

      if (!IS_NPC(victim) && event_cleanse == 0 && victim->pcdata->litup == 1)
      count += 3;

      obj = get_held(victim, ITEM_FLASHLIGHT);

      if (obj != NULL) {
        if (!IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 1 && !shadowcloaked(room))
        count++;
      }

      obj = get_worn(victim, ITEM_FLASHLIGHT);

      if (obj != NULL) {
        if (!IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 1 && !shadowcloaked(room))
        count++;
      }

      if (room->contents != NULL && room->contents->next_content == room->contents) {
        if (room->contents->item_type == ITEM_FLASHLIGHT && !IS_SET(room->contents->extra_flags, ITEM_OFF) && room->contents->value[0] > 1 && !shadowcloaked(room))
        count++;
      }
      else {
        for (obj = room->contents; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;

          if (obj->item_type == ITEM_FLASHLIGHT && !IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 1 && !shadowcloaked(room))
          count++;
        }
      }
    }

    return count;
  }

  int light_level(ROOM_INDEX_DATA *room) {
    if(room == NULL)
    return 50;
    int amb_light = UMAX(daylight_level(room), moonlight_level(room) / 4);
    if (crisis_light == 1)
    amb_light = 100;

    if (!IS_SET(room->room_flags, ROOM_UNLIT) &&  crisis_blackout == 0)
    amb_light = amb_light + 75;

    double percentage = mist_level(room) / 6.0;
    double reduction = amb_light * percentage;
    int islight = amb_light - reduction;

    if (room->z < 0 && room->z > -10) {
      islight = islight - (room->z * -50);
      islight -= 50;
    }

    islight += torch_count(room)*10;

    if (IS_SET(room->room_flags, ROOM_LIGHTON) && (crisis_blackout == 0 || !in_haven(room)))
    islight = UMAX(islight * 2, 50);

    return islight;
  }

  _DOFUN(do_photoattract)
  {
    float val = atof(argument);
    if(val < 1.0 || val > 10.0)
    {
      send_to_char("Syntax: photoattract 1.0-10.0", ch);
      return;
    }
    ch->pcdata->photo_attract = (int)(val*10);
    send_to_char("Set.\n\r", ch);

  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

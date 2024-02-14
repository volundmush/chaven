// Spooky Stuff - Discordance

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
// No code here
#include <map>
#include "merc.h"
#include "olc.h"
#include "gsn.h"
#include "recycle.h"
#include "lookup.h"
#include "global.h"


#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif

  bool is_dead(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      return TRUE;
    }
    return FALSE;
  }

  // ghost visibility - Discordance
  bool is_visible(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_VISIBLE)) {
      return TRUE;
    }

    return FALSE;
  }

  // ghost qualifiers - Discordance
  bool is_ghost(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GHOSTWALKING)) {
      return TRUE;
    }
    if (IS_FLAG(ch->act, PLR_GHOST)) {
      return TRUE;
    }
    if(higher_power(ch))
    return TRUE;

    return FALSE;
  }

  bool can_hear_ghost(CHAR_DATA *speaker, CHAR_DATA *listener) {
    if (IS_NPC(listener))
    return FALSE;
    if(is_gm(speaker) || higher_power(speaker))
    return TRUE;

    if (is_gm(listener))
    return TRUE;
    else if (is_deaf(listener))
    return FALSE;
    else if (!is_ghost(listener) && is_ghost(speaker) && get_skill(listener, SKILL_CLAIRAUDIENCE) < 2 && !is_manifesting(speaker))
    return FALSE;

    return TRUE;
  }

  // ghost manifestation toggle - Discordance
  bool is_manifesting(CHAR_DATA *ch) {
    if(higher_power(ch))
    return TRUE;

    if (IS_FLAG(ch->act, PLR_MANIFESTING)) {
      return TRUE;
    }
    else if (IS_FLAG(ch->act, PLR_VISIBLE)) {
      return TRUE;
    }
    return FALSE;
  }

  // Refills ghost pool
  void refresh_ghost_abilities(CHAR_DATA *ch) {
    if (is_ghost(ch) && is_dead(ch)) {
      ch->pcdata->ghost_pool = GHOST_SPIRITMAX;
    }
    else {
      ch->pcdata->ghost_pool = GHOST_WALKERMAX;
    }
    return;
  }

  // called when ghosts drop everything they're carrying periodically - Discordance
  void ghost_drop(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char buf[MSL];

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (can_see_obj(ch, obj) && (obj->wear_loc == WEAR_NONE || obj->wear_loc == WEAR_HOLD || obj->wear_loc == WEAR_HOLD_2) && can_drop_obj(ch, obj)) {
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);

        sprintf(buf, "%s %s slips from $n's grasp.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_ROOM);
        sprintf(buf, "%s %s slips from your grasp.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_CHAR);
      }
    }
  }

  bool is_possessing(CHAR_DATA *ch) {
    if (!is_ghost(ch))
    return FALSE;

    if (!ch->possessing || ch->possessing == NULL || !ch->possessing->in_room || ch->possessing->in_room == NULL) {
      ch->possessing = NULL;
      return FALSE;
    }
    return TRUE;
  }
  bool is_possessed(CHAR_DATA *ch) {
    CHAR_DATA *to;
    if (ch->in_room == NULL)
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin(); it != ch->in_room->people->end(); ++it) {
      to = *it;

      if (to == NULL)
      continue;

      if (to->in_room == NULL)
      continue;

      if (is_ghost(to) && to->possessing == ch)
      return TRUE;
    }
    return FALSE;
  }
  CHAR_DATA *get_possesser(CHAR_DATA *ch) {
    CHAR_DATA *to;

    for (CharList::iterator it = ch->in_room->people->begin(); it != ch->in_room->people->end(); ++it) {
      to = *it;

      if (to == NULL)
      continue;

      if (to->in_room == NULL)
      continue;

      if (is_ghost(to) && to->possessing == ch)
      return to;
    }
    return NULL;
  }

  bool deplete_ghostpool(CHAR_DATA *ch, int value) {
    if(higher_power(ch))
    return TRUE;

    if (ch->pcdata->ghost_pool >= value) {
      ch->pcdata->ghost_pool = ch->pcdata->ghost_pool - value;
      return TRUE;
    }

    return FALSE;
  }

  void refill_ghostpool(CHAR_DATA *ch, int value) {

    ch->pcdata->ghost_pool = ch->pcdata->ghost_pool + value;

    return;
  }

  // Ghost ability to appear X times each day - Discordance
  _DOFUN(do_manifest) {
    if (is_ghost(ch)) {
      if (is_manifesting(ch)) {
        REMOVE_FLAG(ch->act, PLR_MANIFESTING);
        send_to_char("You stop manifesting your actions in the world of the living.\n\r", ch);
      }
      else {
        SET_FLAG(ch->act, PLR_MANIFESTING);
        send_to_char("You begin to manifest your actions in the world of the living.\n\r", ch);
      }
      if (is_visible(ch)) {
        REMOVE_FLAG(ch->act, PLR_VISIBLE);
        send_to_char("You stop attempting to show yourself to the living.\n\r", ch);
      }
    }
    else {
      send_to_char("You are not bodiless.\n\r", ch);
    }
  }

  _DOFUN(do_possess) {
    if (!is_ghost(ch)) {
      send_to_char("You should probably be incorporeal first.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_GHOSTBOUND)) {
      send_to_char("You can't do that while bound.\n\r", ch);
      return;
    }
    if (is_possessing(ch)) {
      CHAR_DATA *to;
      if (ch->in_room == NULL)
      return;
      for (CharList::iterator it = ch->in_room->people->begin(); it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (to == ch->possessing)
        continue;

        if (get_skill(to, SKILL_CLAIRVOYANCE) > 1) {
          printf_to_char(to, "%s slides out of %s.\n\r", PERS(ch, to), PERS(ch->possessing, to));
        }
      }

      printf_to_char(ch, "You slide out of %s.\n\r", PERS(ch->possessing, ch));
      ch->possessing = NULL;
      return;
    }
    else {

      CHAR_DATA *victim;
      char arg1[MSL];

      argument = one_argument_nouncap(argument, arg1);

      if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
        send_to_char("You don't have the energy to possess another person today.\n\r", ch);
        return;
      }

      if(higher_power(ch))
      {
        victim = get_char_world_pc(arg1);
        if (victim == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
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
        if(!str_cmp(ch->pcdata->fixation_name, victim->name) && ch->pcdata->fixation_timeout > 0)
        canpossess = TRUE;

        if(canpossess == FALSE)
        {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
      }
      if (!higher_power(ch) && ((victim = get_char_room(ch, NULL, arg1)) == NULL || IS_NPC(victim))) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (is_ghost(victim)) {
        send_to_char("Well that would just be confusing.\n\r", ch);
        return;
      }

      char_from_room(ch);
      char_to_room(ch, victim->in_room);
      ch->possessing = victim;

      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (to == ch->possessing)
        continue;

        if (get_skill(to, SKILL_CLAIRVOYANCE) > 1) {
          printf_to_char(to, "%s slides into %s.\n\r", PERS(ch, to), PERS_2(victim, to));
        }
      }

      printf_to_char(ch, "You slide into %s.\n\r", PERS(victim, ch));
    }
  }

  _DOFUN(do_astralproject) {
    CHAR_DATA *to;

    if(ch->pcdata->boon != BOON_ASTRAL || ch->pcdata->boon_timeout < current_time) {
      send_to_char("You don't have that ability.\n\r", ch);
      return;
    }

    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return;

    if (guestmonster(ch))
    return;

    if (in_fight(ch))
    return;

    if (ch->shape != SHAPE_HUMAN) {
      send_to_char("You can only do this in your natural shape.\n\r", ch);
      return;
    }

    ch->possessing = NULL;
    if (IS_FLAG(ch->act, PLR_GHOSTWALKING)) {
      if (IS_FLAG(ch->act, PLR_GHOSTBOUND)) {
        send_to_char("You're stuck.\n\r", ch);
        return;
      }

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (to == ch->possessing)
        continue;

        if (get_skill(to, SKILL_CLAIRVOYANCE) > 1) {
          printf_to_char(to, "%s evaporates.\n\r", PERS(ch, to));
        }
      }

      ch->pcdata->travel_time = -1;
      ch->pcdata->travel_to = -1;
      ch->pcdata->travel_type = -1;
      REMOVE_FLAG(ch->act, PLR_GHOSTWALKING);
      send_to_char("You recorporialize.\n\r", ch);
      act("$n suddenly appears.", ch, NULL, NULL, TO_ROOM);
    }
    else {

      if (ch->pcdata->ghost_wound > 0) {
        send_to_char("You can't manage that right now.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->act, PLR_BOUND))
      REMOVE_FLAG(ch->act, PLR_BOUND);
      if (IS_FLAG(ch->act, PLR_BOUNDFEET))
      REMOVE_FLAG(ch->act, PLR_BOUNDFEET);


      if (IS_FLAG(ch->act, PLR_GHOSTBOUND)) {
        REMOVE_FLAG(ch->act, PLR_GHOSTBOUND);
      }

      act("$n fades into nothingness.", ch, NULL, NULL, TO_ROOM);
      SET_FLAG(ch->act, PLR_GHOSTWALKING);
      use_lifeforce(ch, 100, "Ghostwalk.");
      refresh_ghost_abilities(ch); // Hopefully temporary.
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (to == ch->possessing)
        continue;

        if (get_skill(to, SKILL_CLAIRVOYANCE) > 1) {
          printf_to_char(to, "%s steps out of their body.\n\r", PERS(ch, to));
        }
      }
      send_to_char("You step out of your body.\n\r", ch);
      ch->pcdata->ghost_room = 0;
      ch->pcdata->travel_time = -1;
      ch->pcdata->travel_to = -1;
      ch->pcdata->travel_type = -1;

      OBJ_DATA *obj;
      OBJ_DATA *container;
      OBJ_DATA *obj_next;
      container = create_object(get_obj_index(35), 0);
      char buf[MSL];

      sprintf(buf, "pile of %s's belongings", ch->pcdata->intro_desc);
      free_string(container->short_descr);
      container->short_descr = str_dup(buf);

      sprintf(buf, "A pile of %s's belongings", ch->pcdata->intro_desc);
      free_string(container->description);
      container->description = str_dup(buf);

      sprintf(buf, "pile belongings %s %s", ch->pcdata->intro_desc, ch->name);
      free_string(container->name);
      container->name = str_dup(buf);

      obj_to_room(container, ch->in_room);

      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (!IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->wear_loc == WEAR_NONE) {
          obj_from_char_silent(obj);
          obj_to_obj(obj, container);
        }
      }
    }
    save_ground_objects();
  }

  _DOFUN(do_ghostwalk) {
    CHAR_DATA *to;
    if (get_skill(ch, SKILL_GHOSTWALKING) < 3 && !IS_IMMORTAL(ch)) {
      send_to_char("You don't have that ability.\n\r", ch);
      return;
    }

    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return;

    if (guestmonster(ch))
    return;

    if (in_fight(ch))
    return;

    if (ch->shape != SHAPE_HUMAN) {
      send_to_char("You can only do this in your natural shape.\n\r", ch);
      return;
    }

    ch->possessing = NULL;
    if (IS_FLAG(ch->act, PLR_GHOSTWALKING)) {
      if (IS_FLAG(ch->act, PLR_GHOSTBOUND)) {
        send_to_char("You're stuck.\n\r", ch);
        return;
      }

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (to == ch->possessing)
        continue;

        if (get_skill(to, SKILL_CLAIRVOYANCE) > 1) {
          printf_to_char(to, "%s evaporates.\n\r", PERS(ch, to));
        }
      }
      if(ch->pcdata->ghost_room > 0 && get_room_index(ch->pcdata->ghost_room) != NULL)
      {
        char_from_room(ch);
        char_to_room(ch, get_room_index(ch->pcdata->ghost_room));
      }

      ch->pcdata->travel_time = -1;
      ch->pcdata->travel_to = -1;
      ch->pcdata->travel_type = -1;
      REMOVE_FLAG(ch->act, PLR_GHOSTWALKING);
      send_to_char("You return to your body.\n\r", ch);
      act("$n wakes up with a sharp intake of breath.", ch, NULL, NULL, TO_ROOM);
    }
    else {
      if (is_blind(ch)) {
        send_to_char("The eyes are the windows to the soul, and yours can't see.\n\r", ch);
        return;
      }
      if (ch->pcdata->ghost_wound > 0) {
        send_to_char("You can't manage that right now.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->act, PLR_GHOSTBOUND)) {
        REMOVE_FLAG(ch->act, PLR_GHOSTBOUND);
      }

      act("$n falls into a deep sleep.", ch, NULL, NULL, TO_ROOM);
      SET_FLAG(ch->act, PLR_GHOSTWALKING);
      use_lifeforce(ch, 250, "Ghostwalk.");
      refresh_ghost_abilities(ch); // Hopefully temporary.
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;

        if (to == NULL)
        continue;

        if (to->in_room == NULL)
        continue;

        if (to == ch->possessing)
        continue;

        if (get_skill(to, SKILL_CLAIRVOYANCE) > 1) {
          printf_to_char(to, "%s steps out of their body.\n\r", PERS(ch, to));
        }
      }
      send_to_char("You step out of your body.\n\r", ch);
      ch->pcdata->ghost_room = ch->in_room->vnum;
      ch->pcdata->travel_time = -1;
      ch->pcdata->travel_to = -1;
      ch->pcdata->travel_type = -1;

      OBJ_DATA *obj;
      OBJ_DATA *container;
      OBJ_DATA *obj_next;
      container = create_object(get_obj_index(35), 0);
      char buf[MSL];

      sprintf(buf, "pile of %s's belongings", ch->pcdata->intro_desc);
      free_string(container->short_descr);
      container->short_descr = str_dup(buf);

      sprintf(buf, "A pile of %s's belongings", ch->pcdata->intro_desc);
      free_string(container->description);
      container->description = str_dup(buf);

      sprintf(buf, "pile belongings %s %s", ch->pcdata->intro_desc, ch->name);
      free_string(container->name);
      container->name = str_dup(buf);

      obj_to_room(container, ch->in_room);

      if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL) {
        obj_from_char_silent(obj);
        obj_to_obj(obj, container);
      }
      
      if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL) {
        obj_from_char_silent(obj);
        obj_to_obj(obj, container);
      }

      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (!IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->wear_loc == WEAR_NONE) {
          obj_from_char_silent(obj);
          obj_to_obj(obj, container);
        }
      }
    }
    save_ground_objects();
  }

  _DOFUN(do_ghosttouch) {
    CHAR_DATA *victim;
    char buf[MSL];
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_world_pc(arg1)) != NULL && IS_FLAG(victim->act, PLR_GHOSTWALKING) && victim->pcdata->ghost_room == ch->in_room->vnum) {
      if (is_helpless(ch)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      sprintf(buf, "You make %s feel %s", NAME(victim), argument);
      act(buf, ch, NULL, victim, TO_CHAR);
      sprintf(buf, "$n makes %s feel %s", NAME(victim), argument);
      act(buf, ch, NULL, victim, TO_ROOM);
      printf_to_char(victim, "You feel %s\n\r", argument);
      return;
    }
    send_to_char("They're not here.\n\r", ch);
  }

  vector<GRAVE_TYPE *> GraveVect;
  GRAVE_TYPE *nullgrave;

  void fread_grave(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    GRAVE_TYPE *grave;

    grave = new_grave();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'D':
        KEY("Description", grave->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          GraveVect.push_back(grave);
          return;
        }
        break;
      case 'N':
        KEY("Name", grave->name, fread_string(fp));
        break;
      case 'R':
        KEY("Room", grave->room, fread_number(fp));
        break;
      case 'T':
        KEY("Type", grave->type, fread_number(fp));
        break;
      case 'V':
        KEY("Visit", grave->lastvisit, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_grave: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_graves() {
    nullgrave = new_grave();
    FILE *fp;

    if ((fp = fopen(GRAVE_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_gravess: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "GRAVE")) {
          fread_grave(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_graves: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open graves.txt", 0);
      exit(0);
    }
  }

  void save_graves(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/graves.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/graves.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/graves.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/graves.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/graves.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/graves.txt");
      else
      sprintf(buf, "../data/back7/graves.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open graves.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(GRAVE_FILE, "w")) == NULL) {
        bug("Cannot open graves.txt for writing", 0);
        return;
      }
    }

    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        bug("Save_clans: Blank grave in vector", i);
        continue;
      }

      fprintf(fpout, "#Grave\n");
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Visit %d\n", (*it)->lastvisit);
      fprintf(fpout, "Room %d\n", (*it)->room);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Description %s~\n", (*it)->description);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  _DOFUN(do_firstname) {
    if (ch->pcdata->ci_editing == 10) {
      free_string(ch->pcdata->ci_name);
      ch->pcdata->ci_name = str_dup(argument);
      send_to_char("Done.\n\r", ch);
    }
  }
  _DOFUN(do_surname) {
    if (ch->pcdata->ci_editing == 10) {
      free_string(ch->pcdata->ci_taste);
      ch->pcdata->ci_taste = str_dup(argument);
      send_to_char("Done.\n\r", ch);
    }
  }

  bool corpsecarrier(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {

      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (obj->item_type == ITEM_CORPSE_NPC && obj->timer <= 0)
        return TRUE;
      }
    }
    return FALSE;
  }
  void trashcorpse(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {

      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (obj->item_type == ITEM_CORPSE_NPC && obj->timer <= 0) {
          extract_obj(obj);
          return;
        }
      }
    }
  }

  bool plaqueroom(ROOM_INDEX_DATA *room) {
    if (!gravesite(room))
    return FALSE;

    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type == 0 && (*it)->room == room->vnum)
      return TRUE;
    }
    return FALSE;
  }
  bool graveroom(ROOM_INDEX_DATA *room) {
    if (!gravesite(room))
    return FALSE;

    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type == 1 && (*it)->room == room->vnum)
      return TRUE;
      if ((*it)->type == 2 && (*it)->room == room->vnum)
      return TRUE;
    }
    return FALSE;
  }

  void show_plaques(CHAR_DATA *ch) {
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != 0)
      continue;

      if ((*it)->room != ch->in_room->vnum)
      continue;

      sprintf(buf, "`y%s`x\n\r", (*it)->name);
      strcat(string, buf);
    }
    sprintf(buf, "\n`cYou can Look (plaquename) or Join (plaquename)`x\n\r");
    strcat(string, buf);

    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
  }
  void show_graves(CHAR_DATA *ch) {
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != 1 && (*it)->type != 2)
      continue;

      if ((*it)->room != ch->in_room->vnum)
      continue;

      sprintf(buf, "`D%s`x\n\r", (*it)->name);
      strcat(string, buf);
    }
    sprintf(buf, "\n`cYou can Look (gravename) or Join (gravename)`x\n\r");
    strcat(string, buf);
    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
  }

  bool gravename(CHAR_DATA *ch, char *argument) {
    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->room != ch->in_room->vnum)
      continue;

      if (!str_cmp(argument, (*it)->name))
      return TRUE;
    }
    return FALSE;
  }

  void show_grave(CHAR_DATA *ch, char *argument) {
    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->room != ch->in_room->vnum)
      continue;

      if (!str_cmp(argument, (*it)->name)) {
        page_to_char((*it)->description, ch);
        return;
      }
    }
  }

  void gravevisit(CHAR_DATA *ch, char *argument) {
    for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
    it != GraveVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->room != ch->in_room->vnum)
      continue;

      if (!str_cmp(argument, (*it)->name))
      (*it)->lastvisit = current_time;
    }
  }

#if defined(__cplusplus)
}
#endif

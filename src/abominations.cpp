/**************************************************************************
* Written By !  *
***************************************************************************/

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

#if defined(__cplusplus)
extern "C" {
#endif


  int manipulated_mist = 0;
  int town_blackout = 0;

  bool mist_room(ROOM_INDEX_DATA *room) {
    if (!IS_SET(room->room_flags, ROOM_INDOORS) && mist_level(room) >= 3) {
      if (sunphase(room) != 4)
      return TRUE;
    }
    return FALSE;
  }

  bool mist_manipulated_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (manipulated_mist == 0)
    return FALSE;

    if (!mist_room(room))
    return FALSE;

    return TRUE;
  }

  bool is_town_blackout(void) {
    if (town_blackout == 0)
    return FALSE;

    return TRUE;
  }

  void abomination_global_update(void) {
    if (town_blackout > 0)
    town_blackout--;

    if (manipulated_mist > 0)
    manipulated_mist--;
  }

  bool is_abom(CHAR_DATA *ch) {

    if (event_cleanse == 1)
    return FALSE;

    if (ch->abomination >= 1 && ch->abomination <= 3)
    return TRUE;

    return FALSE;
  }
  void apply_blackeyes(CHAR_DATA *ch) {
    return;
    if (!is_abom(ch))
    return;

    if (is_helpless(ch) || in_fight(ch) || room_hostile(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->pcdata->blackeyes_power >= 100)
    ch->pcdata->blackeyes = -1;
    else if (ch->pcdata->blackeyes == 0) {
      ch->pcdata->blackeyes_power++;
      ch->pcdata->blackeyes = ch->pcdata->blackeyes_power *
      (int)(sqrt(ch->pcdata->blackeyes_power)) / 2;
    }
  }
  void apply_blackeyes_logon(CHAR_DATA *ch) {
    return;
    if (!is_abom(ch))
    return;
    if (is_gm(ch))
    return;
    if (IS_FLAG(ch->act, PLR_DEAD))
    return;

    if (IS_AFFECTED(ch, AFF_ABDUCTED))
    return;

    if (ch->pcdata->blackeyes_power >= 100)
    ch->pcdata->blackeyes = -1;
    else if (ch->pcdata->blackeyes == 0) {
      if (ch->pcdata->blackeyes_power < 30)
      ch->pcdata->blackeyes_power++;
      ch->pcdata->blackeyes = ch->pcdata->blackeyes_power *
      (int)(sqrt(ch->pcdata->blackeyes_power)) / 2;
    }
  }

  bool has_blackeyes(CHAR_DATA *ch) {
    if (IS_NPC(ch)) // Addressing possible crash - Discordance
    return FALSE;

    if (IS_FLAG(ch->act, PLR_SINSPIRIT))
    return FALSE;

    if (!is_abom(ch))
    return FALSE;

    if (ch->pcdata->blackeyes != 0)
    return TRUE;

    return FALSE;
  }

  char *random_nightmare() {
    switch (number_percent() % 3) {
    case 0:
      return "You are trapped in quicksand, people walk by without seeing you as you slowly sink and call for help, they ignore you as the quicksand fills your mouth and then lungs.";
      break;
    case 1:
      return "You are running through a dark maze away from something horrible and nameless, you round a corner and run straight into a solid wall, leaving your trapped as the monster comes closer.";
      break;
    default:
      return "You are falling through the sky, flailingly helplessly against the wind as the city below grows closer and closer so rapidly until you tense up for the impact.";
      break;
    }
  }

  void abom_logon(CHAR_DATA *ch) {
    return;
    if (is_gm(ch))
    return;
    CHAR_DATA *to;
    bool found = FALSE;

    if (number_percent() % 12 == 0) {

      int type = number_percent() % 8;
      switch (type) {
      case 0:
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;

          if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;
            if (to == ch)
            continue;
            if (to->in_room == NULL || ch->in_room == NULL)
            continue;

            if (is_abom(to))
            continue;

            if (number_percent() % 5 == 0) {
              free_string(to->pcdata->nightmare);
              to->pcdata->nightmare = str_dup(random_nightmare());
            }
          }
        }
        break;
      case 1:
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;

          if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;
            if (to == ch)
            continue;
            if (to->in_room == NULL || ch->in_room == NULL)
            continue;

            if (is_abom(to) || is_undead(to))
            continue;

            if (number_percent() % 3 == 0 && to->pcdata->impregnated != 0 && !found) {
              miscarriage(to, FALSE);
              to->pcdata->impregnated = 0;
              found = TRUE;
            }
          }
        }
        break;
      case 2:
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;

          if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;
            if (to == ch)
            continue;
            if (to->in_room == NULL || ch->in_room == NULL)
            continue;

            if (is_abom(to) || is_undead(to))
            continue;

            if (number_percent() % 3 == 0) {
              if (number_percent() % 2 == 0 || !is_super(ch))
              wound_char_absolute(to, 1);
              send_to_char("You fall ill.\n\r", to);
            }
          }
        }
        break;
      case 3:
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;

          if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;
            if (to == ch)
            continue;
            if (to->in_room == NULL || ch->in_room == NULL)
            continue;

            send_to_char("The earth tremors beneath your feet.\n\r", to);
          }
        }
        break;
      case 4:
        NEWS_TYPE *news;

        news = new_news();
        news->timer = 1500;
        free_string(news->message);
        news->message = str_dup("There are reports of a bizzarely deformed cattle birth on a farm near Haven");
        free_string(news->author);
        news->author = str_dup("Town Events");
        NewsVect.push_back(news);
        break;
      case 5:
      case 6:
      case 7:
        break;
      }
    }
  }

  _DOFUN(do_warp) {
    if (!str_cmp(argument, "Mist")) {
      if (!mist_room(ch->in_room)) {
        send_to_char("You need to be inside the mist to do that.\n\r", ch);
        return;
      }
      manipulated_mist = number_range(20, 50);
      send_to_char("You start to warp the mists to your will.\n\r", ch);
      return;
    }
    send_to_char("Syntax: Warp mist\n\r", ch);
  }

  _DOFUN(do_blackout) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "town")) {
      if (ch->pcdata->abomblackcool > 0) {
        send_to_char("You've done that too recently.\n\r", ch);
        return;
      }
      town_blackout = number_range(30, 90);
      ch->pcdata->abomblackcool = (60 * 8);
      send_to_char("You start to interfere with the town's communication.\n\r", ch);
      return;
    }
    if (ch->pcdata->blackout_cooldown > 0) {
      send_to_char("You've done that too recently.\n\r", ch);
      return;
    }

    ch->pcdata->blackout_cooldown = 60;

    CHAR_DATA *victim;

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("The ability fails.\n\r", ch);
      return;
    }
    if (in_house(victim) != NULL && in_house(victim)->blackout == 0) {
      in_house(victim)->blackout = 18;
      send_to_char("You silence their residence.\n\r", ch);

      CHAR_DATA *to;

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (to == NULL)
          continue;
          if (IS_NPC(to))
          continue;
          if (in_house(to) == in_house(victim))
          send_to_char("`DThe power goes out.`x\n\r", to);
        }
      }
    }
    if (in_prop(victim) != NULL && (!is_base(in_prop(victim)) || get_roomz(victim->in_room) == 0) && in_prop(victim)->blackout == 0) {
      in_prop(victim)->blackout = 18;
      send_to_char("You silence their residence.\n\r", ch);

      CHAR_DATA *to;

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (to == NULL)
          continue;
          if (IS_NPC(to))
          continue;
          if (in_prop(to) == in_prop(victim))
          send_to_char("`DThe power goes out.`x\n\r", to);
        }
      }
    }
    else {
      send_to_char("The ability fails.\n\r", ch);
    }

    WAIT_STATE(ch, PULSE_PER_SECOND * 5);
  }

  void nounderglow(CHAR_DATA *ch) {
    if (ch->pcdata->prison_mult > 0) {
      ch->pcdata->prison_care = UMAX(ch->pcdata->prison_care, current_time);
      ch->pcdata->prison_care =
      UMIN(ch->pcdata->prison_care + (3600 * 24), current_time + (3600 * 40));
      ch->pcdata->prison_mult = 1;
    }

    CHAR_DATA *victim;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      if (can_shroud(victim))
      act("$n starts to emit a faint red aura.", ch, NULL, victim, TO_VICT);
    }
  }

  void limunderglow(CHAR_DATA *ch) {
    CHAR_DATA *victim;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      if (can_shroud(victim))
      act("$n starts to emit a faint orange aura.", ch, NULL, victim, TO_VICT);
    }
  }

  bool has_praestes(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!has_weakness(ch, victim))
    return FALSE;
    for (int i = 0; i < 10; i++) {
      if (victim->pcdata->relationship_type[i] == REL_PRAESTES)
      return TRUE;
    }
    return FALSE;
  }

  bool seems_under_limited(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;


    if(!str_cmp(ch->pcdata->fixation_name, ch->name) && ch->pcdata->fixation_level >= 7)
    return FALSE;

    if (seems_under_understanding(ch, victim))
    return FALSE;
    if (ch->pcdata->destiny_feature == DEST_FEAT_LIMITED)
    return TRUE;

    if (!str_cmp(ch->pcdata->understanding, "None"))
    return FALSE;



    if (!in_haven(ch->in_room) && ch->in_room != NULL && ch->in_room->vnum != 18999 && ch->in_room->area->vnum != 31) {
      if (in_world(ch) != WORLD_EARTH) {
        if (IS_NPC(ch) || IS_NPC(victim))
        return FALSE;
        if (ch->pcdata->resident != 0 && ch->pcdata->resident == in_world(ch))
        return FALSE;
        if (victim->pcdata->resident != 0 && victim->pcdata->resident == in_world(victim))
        return FALSE;
      }
      else
      return FALSE;
    }

    if (in_haven(victim->in_room) && has_praestes(victim, ch))
    return TRUE;

    if (in_haven(victim->in_room) && victim->vassal != 0)
    return TRUE;

    if (!str_cmp(ch->pcdata->understanding, "Limited")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("");
      bool under = seems_under_understanding(ch, victim);
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("Limited");
      return under;
    }
    return FALSE;
  }
  bool under_limited(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;

    if(!str_cmp(ch->pcdata->fixation_name, ch->name) && ch->pcdata->fixation_level >= 7)
    return FALSE;


    if (under_understanding(ch, victim))
    return FALSE;
    if (ch->pcdata->destiny_feature == DEST_FEAT_LIMITED)
    return TRUE;
    if (!str_cmp(ch->pcdata->understanding, "None"))
    return FALSE;


    if (!in_haven(ch->in_room) && ch->in_room != NULL && ch->in_room->vnum != 18999 && ch->in_room->area->vnum != 31) {
      if (in_world(ch) != WORLD_EARTH) {
        if (IS_NPC(ch) || IS_NPC(victim))
        return FALSE;
        if (ch->pcdata->resident != 0 && ch->pcdata->resident == in_world(ch))
        return FALSE;
        if (victim->pcdata->resident != 0 && victim->pcdata->resident == in_world(victim))
        return FALSE;
      }
      else
      return FALSE;
    }

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (current_time < (*it)->active_time || current_time > (*it)->deactive_time)
      continue;

      if ((*it)->type == EVENT_UNDERSTANDINGMINUS || (*it)->typetwo == EVENT_UNDERSTANDINGMINUS) {
        return FALSE;
      }
      if ((!in_haven(ch->in_room) || deepforest(ch->in_room)) && ch->in_room->area->vnum != 12) {
        if ((*it)->type == EVENT_CLEANSE || (*it)->typetwo == EVENT_CLEANSE) {
          return FALSE;
        }
      }
    }

    if (IS_AFFECTED(ch, AFF_NOUNDERSTANDING))
    return FALSE;

    if (guestmonster(ch) || guestmonster(victim))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_NIGHTMARE)
    return FALSE;

    if (in_haven(victim->in_room) && has_praestes(victim, ch))
    return TRUE;

    if (in_haven(victim->in_room) && victim->vassal != 0)
    return TRUE;

    if (!str_cmp(ch->pcdata->understanding, "Limited")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("");
      bool under = under_understanding(ch, victim);
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("Limited");
      return under;
    }
    return FALSE;
  }

  _DOFUN(do_shadowcloak) {
    if (IS_FLAG(ch->comm, COMM_SHADOWCLOAK)) {
      REMOVE_FLAG(ch->comm, COMM_SHADOWCLOAK);
      send_to_char("You let your shadowcloak fade.\n\r", ch);
      return;
    }
    if (get_skill(ch, SKILL_SHADOWCLOAK) > 0 && !is_helpless(ch)) {
      SET_FLAG(ch->comm, COMM_SHADOWCLOAK);
      send_to_char("You sumon a cloak of shadows.\n\r", ch);
      act("Nearby lights start to flicker and die.", ch, NULL, NULL, TO_ROOM);
    }
  }

  _DOFUN(do_mistpull) {
    CHAR_DATA *victim;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    return;

    if ((victim = get_char_distance(ch, arg1, 2)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (!mist_room(ch->in_room) || !mist_room(victim->in_room)) {
      send_to_char("You need more mist to work with.\n\r", ch);
      return;
    }
    if (pc_pop(victim->in_room) > 3) {
      send_to_char("There's too many people there to target them.\n\r", ch);
      return;
    }
    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (is_safe(ch, victim)) {
      send_to_char("For some reason you can't bring yourself to do that.\n\r", ch);
      return;
    }
    if (in_public(ch, victim)) {
      send_to_char("You'd certainly get in trouble with the police if you did that.\n\r", ch);
      return;
    }
    if (in_fight(victim)) {
      send_to_char("That area is a bit crowded.\n\r", ch);
      return;
    }

    if (is_ghost(victim)) {
      send_to_char("They don't seem corporeal enough.\n\r", ch);
      return;
    }

    if (is_ghost(ch)) {
      send_to_char("Such dynamism is no longer necessary./n/r", ch);
      return;
    }

    ROOM_INDEX_DATA *from_room = victim->in_room;
    CHAR_DATA *saviour;
    CHAR_DATA *truesavior = NULL;

    for (CharList::iterator it = from_room->people->begin();
    it != from_room->people->end(); ++it) {
      saviour = *it;

      if (saviour == NULL)
      continue;

      if (is_abom(saviour) || is_gm(saviour))
      continue;

      if (number_percent() % 5 == 0 || (get_skill(saviour, SKILL_FASTREFLEXES) * 2 +
            get_skill(saviour, SKILL_DEXTERITY)) > number_range(3, 10))
      truesavior = saviour;
    }

    if (truesavior != NULL) {
      act("$n gets suddenly sucked through the mist, but $N is able to grab $s hand just in time and be pulled along with $m.", victim, NULL, truesavior, TO_NOTVICT);
      act("You get suddenly sucked through the mist, but $N is able to grab your hand just in time and be pulled along with you.", victim, NULL, truesavior, TO_CHAR);
      act("$n gets suddenly sucked through the mist, but you are able to grab $s hand just in time and be pulled along with $m.", victim, NULL, truesavior, TO_VICT);
      char_from_room(truesavior);
      char_to_room(truesavior, ch->in_room);
    }
    else {
      act("$n gets suddenly sucked through the mist.", victim, NULL, NULL, TO_ROOM);
      act("You get suddenly sucked through the mist.", victim, NULL, NULL, TO_CHAR);
    }
    char_from_room(victim);
    char_to_room(victim, ch->in_room);
    act("The mists suck $N through them and deliver $M to you.", ch, NULL, victim, TO_CHAR);
    start_fight(ch, victim);
  }

  CHAR_DATA *random_person() {
    CHAR_DATA *to;
    CHAR_DATA *target = NULL;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (to->in_room == NULL)
        continue;

        if (is_gm(to) || !in_haven(to->in_room))
        continue;

        if (target == NULL || number_percent() % 4 == 0)
        target = to;
      }
    }
    return target;
  }

  void remember_detail(CHAR_DATA *ch, CHAR_DATA *target) {
    char buf[MSL];
    int iWear;
    OBJ_DATA *obj;
    if (is_abom(target) && number_percent() % 3 != 0)
    target = random_person();

    if (number_percent() % 3 == 0) {
      if (!is_covered(target, COVERS_EYES) && number_percent() % 2 == 0) {
        free_string(ch->pcdata->remember_detail);
        sprintf(buf, "You remember that someone had %s.", get_natural_eyes(target));
        ch->pcdata->remember_detail = str_dup(buf);
      }
      if (!is_covered(target, COVERS_HAIR) && number_percent() % 2 == 0) {
        free_string(ch->pcdata->remember_detail);
        sprintf(buf, "You remember that someone had %s.", get_basic_hair(target));
        ch->pcdata->remember_detail = str_dup(buf);
      }
      if (get_bust(target) > -1 && number_percent() % 2 == 0) {
        free_string(ch->pcdata->remember_detail);
        sprintf(buf, "You remember that someone had %s.", cup_size[get_bust(target)]);
        ch->pcdata->remember_detail = str_dup(buf);
      }
      if (number_percent() % 2 == 0) {
        free_string(ch->pcdata->remember_detail);
        sprintf(buf, "You remember that someone was about %d feet tall.", get_height_feet(target));
        ch->pcdata->remember_detail = str_dup(buf);
      }
    }
    else {
      for (iWear = MAX_WEAR - 1; iWear >= 0; iWear--) {

        if ((obj = get_eq_char(target, iWear)) != NULL && can_see_wear(target, iWear)) {

          if (number_percent() % 4 == 0) {
            sprintf(buf, "You remember someone with %s.", format_obj_to_char(obj, ch, FALSE));
            free_string(ch->pcdata->remember_detail);
            ch->pcdata->remember_detail = str_dup(buf);
          }
        }
      }
    }
  }

  _DOFUN(do_memorycloak) {
    if (get_skill(ch, SKILL_MEMORYCLOAK) < 1) {
      send_to_char("You don't have that ability.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_CLOAKED)) {
      send_to_char("You let your memory cloak drop.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_CLOAKED);
    }
    else {
      send_to_char("You activate your memory cloak.\n\r", ch);
      apply_blackeyes(ch);
      SET_FLAG(ch->comm, COMM_CLOAKED);
      ch->pcdata->last_public_room = 0;
    }
  }

  bool isalive(char *name) {
    char buf[MSL];
    CHAR_DATA *victim;

    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    d.original = NULL;

    if ((victim = get_char_world_pc(name)) == NULL) {
      log_string("DESCRIPTOR: isalive");

      if (!load_char_obj(&d, name)) {
        return FALSE;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    else
    online = TRUE;

    if (IS_FLAG(victim->act, PLR_DEAD)) {
      if (!online)
      free_char(victim);
      return FALSE;
    }
    if (!online)
    free_char(victim);

    return TRUE;
  }

  void breakcontrol(char *name, int type) {
    char buf[MSL];
    CHAR_DATA *victim;

    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    d.original = NULL;

    if ((victim = get_char_world_pc(name)) == NULL) {
      log_string("DESCRIPTOR: breakcontrol");

      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    else
    online = TRUE;

    if (type == 1) {
      free_string(victim->pcdata->enthralling);
      victim->pcdata->enthralling = str_dup("");
    }
    if (type == 2) {
      free_string(victim->pcdata->enrapturing);
      victim->pcdata->enrapturing = str_dup("");
    }
    if (type == 3) {
      if (IS_AFFECTED(victim, AFF_MARKED) || IS_AFFECTED(victim, AFF_MARKED2) || victim->wounds >= 3 || victim->pcdata->bloodaura > 0) {
        free_string(victim->pcdata->enlinked);
        victim->pcdata->enlinked = str_dup(victim->pcdata->enthralled);
        victim->pcdata->enlinktime = 60 * 24 * 2;
      }
      else {
        free_string(victim->pcdata->enlinked);
        victim->pcdata->enlinked = str_dup(victim->pcdata->enthralled);
        victim->pcdata->enlinktime = 60 * 5;
      }
      free_string(victim->pcdata->enthralled);
      victim->pcdata->enthralled = str_dup("");
    }
    if (type == 4) {
      if (IS_AFFECTED(victim, AFF_MARKED) || IS_AFFECTED(victim, AFF_MARKED2) || victim->wounds >= 3 || victim->pcdata->bloodaura > 0) {
        free_string(victim->pcdata->enlinked);
        victim->pcdata->enlinked = str_dup(victim->pcdata->enraptured);
        victim->pcdata->enlinktime = 60 * 24 * 2;
      }
      else {
        free_string(victim->pcdata->enlinked);
        victim->pcdata->enlinked = str_dup(victim->pcdata->enthralled);
        victim->pcdata->enlinktime = 60 * 5;
      }
      free_string(victim->pcdata->enraptured);
      victim->pcdata->enraptured = str_dup("");
    }
    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  _DOFUN(do_enthrall) {

    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "Break")) {
      if (safe_strlen(ch->pcdata->enthralling) < 2) {
        send_to_char("You're not enthralling anyone.\n\r", ch);
        return;
      }
      message_to_char(ch->pcdata->enthralling, "You are no longer enthralled.");
      breakcontrol(ch->pcdata->enthralling, 3);
      free_string(ch->pcdata->enthralling);
      ch->pcdata->enthralling = str_dup("");
      send_to_char("Done.", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (!is_helpless(victim)) {
      send_to_char("They need to be helpless first.\n\r", ch);
      return;
    }
    if (safe_strlen(ch->pcdata->enthralling) > 2) {
      send_to_char("You're already holding someone else in thrall.\n\r", ch);
      return;
    }

    if(!str_cmp(ch->pcdata->fixation_name, victim->name) && ch->pcdata->fixation_level >= 2 && ch->pcdata->fixation_charges > 0) {
      ch->pcdata->fixation_charges--;
      act("$n places you into thrall, forcing you to obey their commands.", ch, NULL, victim, TO_VICT);
      free_string(ch->pcdata->enthralling);
      ch->pcdata->enthralling = str_dup(victim->name);
      free_string(victim->pcdata->enthralled);
      victim->pcdata->enthralled = str_dup(ch->name);
      victim->pcdata->enthrall_timeout = UMAX(current_time+(3600*24*7), ch->pcdata->fixation_timeout);

      send_to_char("You place them in your thrall.\n\r", ch);
      act("$n stares into $N's eyes, $s own pulsing with black energy.", ch, NULL, victim, TO_NOTVICT);
    }

    if (get_skill(ch, SKILL_ENTHRALL) < 1) {
      send_to_char("You don't have that ability.\n\r", ch);
      return;
    }

    act("$n places you into thrall, forcing you to obey their commands.", ch, NULL, victim, TO_VICT);
    free_string(ch->pcdata->enthralling);
    ch->pcdata->enthralling = str_dup(victim->name);
    free_string(victim->pcdata->enthralled);
    victim->pcdata->enthralled = str_dup(ch->name);
    send_to_char("You place them in your thrall.\n\r", ch);
    act("$n stares into $N's eyes, $s own pulsing with black energy.", ch, NULL, victim, TO_NOTVICT);
    apply_blackeyes(ch);
  }

  _DOFUN(do_enrapture) {

    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "Break")) {

      if (safe_strlen(ch->pcdata->enrapturing) < 2) {
        send_to_char("You're not enrapturing anyone.\n\r", ch);
        return;
      }
      message_to_char(ch->pcdata->enrapturing, "You are no longer enraptured.");
      breakcontrol(ch->pcdata->enrapturing, 4);
      free_string(ch->pcdata->enrapturing);
      ch->pcdata->enrapturing = str_dup("");
      send_to_char("Done.", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (!IS_NPC(victim) && victim->pcdata->trance > 0)
    victim->pcdata->trance = -20;

    if (!is_helpless(victim)) {
      send_to_char("They need to be helpless first.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD)) {
      send_to_char("Leave the nightmare first.\n\r", ch);
      return;
    }

    if (safe_strlen(ch->pcdata->enrapturing) > 2) {
      send_to_char("You're already holding someone else in rapture.\n\r", ch);
      return;
    }



    if(!str_cmp(ch->pcdata->fixation_name, victim->name) && ch->pcdata->fixation_level >= 2 && ch->pcdata->fixation_charges > 0) {
      ch->pcdata->fixation_charges--;
      act("You realise you love $n, and you'd do almost anything to make $m happy.", ch, NULL, victim, TO_VICT);
      free_string(ch->pcdata->enrapturing);
      ch->pcdata->enrapturing = str_dup(victim->name);
      free_string(victim->pcdata->enraptured);
      victim->pcdata->enraptured = str_dup(ch->name);
      send_to_char("You place them in your rapture.\n\r", ch);
      act("$n stares into $N's eyes, $s own pulsing with black energy.", ch, NULL, victim, TO_NOTVICT);
      victim->pcdata->enrapture_timeout = UMAX(current_time+(3600*24*7), ch->pcdata->fixation_timeout);

      send_to_char("You place them in your thrall.\n\r", ch);
      act("$n stares into $N's eyes, $s own pulsing with black energy.", ch, NULL, victim, TO_NOTVICT);
    }
    if (get_skill(ch, SKILL_ENRAPTURE) < 1) {
      send_to_char("You don't have that ability.\n\r", ch);
      return;
    }
    act("You realise you love $n, and you'd do almost anything to make $m happy.", ch, NULL, victim, TO_VICT);
    free_string(ch->pcdata->enrapturing);
    ch->pcdata->enrapturing = str_dup(victim->name);
    free_string(victim->pcdata->enraptured);
    victim->pcdata->enraptured = str_dup(ch->name);
    send_to_char("You place them in your rapture.\n\r", ch);
    act("$n stares into $N's eyes, $s own pulsing with black energy.", ch, NULL, victim, TO_NOTVICT);
  }

  int badpreyscore(CHAR_DATA *ch) {
    int score = 0;
    if (guestmonster(ch))
    score += 100;
    if (higher_power(ch))
    score += 100;

    if (ch->played / 3600 < 20)
    score += 25;

    score -= 5 * (get_tier(ch) * get_tier(ch));

    if (IS_FLAG(ch->act, PLR_SHROUD))
    score /= 2;

    return score;
  }

  char *const age_names[] = {
    "late teens", "early twenties", "mid-twenties", "late twenties", "early thirties", "mid-thirties", "late thirties", "early forties", "mid-forties", "late forties", "early fifties", "mid-fifties", "late fifties", "early sixties", "mid-sixties", "late sixties", "early seventies", "mid-seventies", "late seventies", "early eighties", "mid-eighties", "late eighties", "early nineties", "mid-nineties", "late nineties", "hundreds"};

  _DOFUN(do_seek) {
    if (!guestmonster(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    int min = 10000;
    ROOM_INDEX_DATA *sought;
    CHAR_DATA *to;
    int pointer;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        if (to == ch)
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;

        if (is_gm(to) || guestmonster(to))
        continue;
        if (pc_pop(to->in_room) > 2)
        continue;

        if (IS_FLAG(to->comm, COMM_AFK))
        continue;

        if (in_world(to) != in_world(ch))
        continue;

        int dist = get_dist(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(to->in_room), get_roomy(to->in_room));
        dist += badpreyscore(to);
        if (dist < min) {
          min = get_dist(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(to->in_room), get_roomy(to->in_room));
          min += badpreyscore(to);
          sought = to->in_room;
          victim = to;
        }
      }
    }

    if (sought == NULL) {
      send_to_char("No prey found.\n\r", ch);
      return;
    }
    char buf[MSL];
    remove_color(buf, sought->name);

    if (pc_pop(sought) == 2) {
      if (room_prop(sought) != NULL) {
        printf_to_char(
        ch, "`rYou sense a pair of potential prey, in %s, at %s, %s.`x\n\r", buf, room_prop(sought)->address, relspacial[get_reldirection(
        roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(sought), get_roomy(sought)), ch->facing)]);
      }
      else {
        printf_to_char(
        ch, "`rYou sense a pair of potential prey, in %s, %s.`x\n\r", buf, relspacial[get_reldirection(
        roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(sought), get_roomy(sought)), ch->facing)]);
      }
    }
    else {
      if (room_prop(sought) != NULL) {
        pointer = get_age_descriptor(victim);
        if (victim->sex == SEX_MALE)
        printf_to_char(
        ch, "`rYou sense a male in his %s, in %s, at %s, %s.`x\n\r", age_names[pointer], buf, room_prop(sought)->address, relspacial[get_reldirection(
        roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(sought), get_roomy(sought)), ch->facing)]);
        else
        printf_to_char(
        ch, "`rYou sense a female in her %s, in %s, at %s, %s.`x\n\r", age_names[pointer], buf, room_prop(sought)->address, relspacial[get_reldirection(
        roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(sought), get_roomy(sought)), ch->facing)]);
      }
      else {
        printf_to_char(
        ch, "`rYou sense potential prey, in %s, %s.`x\n\r", buf, relspacial[get_reldirection(
        roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(sought), get_roomy(sought)), ch->facing)]);
      }
    }
    apply_blackeyes(ch);
  }

  _DOFUN(do_fleshform) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];

    if (!IS_AFFECTED(ch, AFF_FLESHFORMING)) {
      if (get_skill(ch, SKILL_FLESHFORMING) < 1) {
        if (ch->modifier != MODIFIER_FLESHFORMED) {
          send_to_char("You don't have that ability.\n\r", ch);
          return;
        }
        else if (ch->pcdata->fleshformed + (3600 * 24 * 3) < current_time) {
          send_to_char("You have already been fleshformed.\n\r", ch);
          return;
        }
      }
    }

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "self")) {
    }
    else {
      if (get_skill(ch, SKILL_FLESHFORMING) < 1) {
        send_to_char("You don't have the ability to fleshform others. Use fleshform self to fleshform yourself.\n\r", ch);
        return;
      }
      if (!is_helpless(victim) && ch != victim) {
        send_to_char("They need to be helpless first.\n\r", ch);
        return;
      }
    }

    if(under_understanding(victim, ch))
    {
      send_to_char("Your body won't do that.\n\r", ch);
      return;
    }

    if (ch != victim)
    use_lifeforce(ch, 100, "Fleshforming.");
    if (!str_cmp(arg2, "hair")) {
      if (victim->modifier == MODIFIER_FLESHFORMED) {
        free_string(victim->pcdata->hair);
        victim->pcdata->hair = str_dup(argument);
        victim->pcdata->hair_genetic = 0;
      }
      else {
        set_natural_hair(victim, argument);
      }
    }
    else if (!str_cmp(arg2, "eyes")) {
      if (victim->modifier == MODIFIER_FLESHFORMED) {
        free_string(victim->pcdata->eyes);
        victim->pcdata->eyes = str_dup(argument);
        victim->pcdata->eyes_genetic = 0;
      }
      else {
        set_natural_eyes(victim, argument);
      }
    }
    else if (!str_cmp(arg2, "skin")) {
      free_string(victim->pcdata->skin);
      victim->pcdata->skin = str_dup(argument);

    }
    else if (!str_cmp(arg2, "breasts")) {
      if (!str_cmp(argument, "Larger")) {
        if (victim->pcdata->bust >= 15 && victim->modifier != MODIFIER_FLESHFORMED) {
          send_to_char("You can't increase that any further.\n\r", ch);
          return;
        }
        victim->pcdata->bust++;
      }
      else if (!str_cmp(argument, "Smaller")) {
        if (victim->pcdata->bust < 3 && victim->modifier != MODIFIER_FLESHFORMED) {
          send_to_char("You can't decrease that any further.\n\r", ch);
          return;
        }
        victim->pcdata->bust--;
      }
      else {
        send_to_char("fleshform (person) breasts (larger/smaller)\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg2, "gender")) {
      if (!str_cmp(argument, "male")) {
        victim->pcdata->penis = 10;
        victim->pcdata->bust = 0;
        victim->sex = SEX_MALE;
      }
      else if (!str_cmp(argument, "female")) {
        victim->pcdata->bust += 2;
        victim->pcdata->penis = 0;
        victim->sex = SEX_FEMALE;
      }
      else {
        send_to_char("fleshform (person) gender (male/female)\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg2, "height")) {
      if (!str_cmp(argument, "taller")) {
        if (victim->pcdata->height_feet > 7) {
          send_to_char("Their body couldn't sustain that.\n\r", ch);
          return;
        }
        if (victim->modifier != MODIFIER_FLESHFORMED && victim->pcdata->height_inches >= 11) {
          char buf[MSL];
          sprintf(buf, "%d", victim->pcdata->height_feet + 1);
          if (!change_feet(victim, buf)) {
            send_to_char("Their body couldn't sustain that.\n\r", ch);
            return;
          }
        }
        victim->pcdata->height_inches++;
      }
      else if (!str_cmp(argument, "shorter")) {
        if (victim->pcdata->height_feet < 4) {
          send_to_char("Their body couldn't sustain that.\n\r", ch);
          return;
        }
        if (victim->modifier != MODIFIER_FLESHFORMED && victim->pcdata->height_inches <= 0) {
          char buf[MSL];
          sprintf(buf, "%d", victim->pcdata->height_feet - 1);
          if (!change_feet(victim, buf)) {
            send_to_char("Their body couldn't sustain that.\n\r", ch);
            return;
          }
        }
        victim->pcdata->height_inches--;
      }
      else {
        send_to_char("fleshform (person) height (taller/shorter)\n\r", ch);
        return;
      }

      if (victim->pcdata->height_inches >= 12) {
        victim->pcdata->height_feet++;
        victim->pcdata->height_inches -= 12;
      }
      if (victim->pcdata->height_inches < 0) {
        victim->pcdata->height_feet--;
        victim->pcdata->height_inches += 12;
      }
    }
    else if (!str_cmp(arg2, "younger")) {

      if (get_age(victim) <= 18) {
        send_to_char("Their body couldn't sustain that.\n\r", ch);
        return;
      }

      if (get_lifeforce(ch, TRUE, NULL) <= 85) {
        send_to_char("You do not have enough lifeforce to power that.\n\r", ch);
        return;
      }

      victim->pcdata->deaged += 365;
      take_lifeforce(ch, 1500, "fleshforming younger");
      take_lifeforce(victim, 1500, "fleshforming younger");

    }
    else if (!str_cmp(arg2, "older")) {

      victim->pcdata->deaged -= 365;
    }
    else if (!str_cmp(arg2, "appearance")) {
      victim->id = get_pc_id();
    }
    else {
      send_to_char("fleshform (person) (type)\n\r", ch);
      return;
    }

    victim->pcdata->fleshformed = current_time;
    send_to_char("Done.\n\r", ch);
    send_to_char("You feel a fire burn through your body.\n\r", victim);
    victim->pcdata->desclock = 0;

    return;
  }

  _DOFUN(do_blackeyes) {
    if (!is_abom(ch)) {
      send_to_char("This is an abomination command.\n\r", ch);
      return;
    }

    if (ch->pcdata->blackeyes == 0) {
      if (ch->pcdata->blackeyes_power > 0) {
        send_to_char("You can't do that yet.\n\r", ch);
        return;
      }
      if (is_neutralized(ch)) {
        send_to_char("You can't do that right now.\n\r", ch);
        return;
      }

      ch->pcdata->blackeyes = 1;
      ch->pcdata->blackeyes_power = 60;
      send_to_char("You summon your power.\n\r", ch);
      if (!is_covered(ch, COVERS_EYES))
      act("`D$n's eyes shift to an unnatrual, reflective black.`x", ch, NULL, NULL, TO_ROOM);
    }
    else {
      if (is_helpless(ch) || in_fight(ch) || room_hostile(ch->in_room) || is_cloaked(ch)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }

      if (ch->pcdata->blackeyes_power > 0) {
        printf_to_char(ch, "You'd have to wait another %d minutes first.\n\r", ch->pcdata->blackeyes_power);
        return;
      }
      ch->pcdata->blackeyes = 0;
      ch->pcdata->blackeyes_power = 12;
      send_to_char("You let your power receed.\n\r", ch);
      if (!is_covered(ch, COVERS_EYES))
      act("The darkness receeds from $n's eyes.", ch, NULL, NULL, TO_ROOM);
    }
  }

  _DOFUN(do_abomcure) {
    char arg1[MSL];
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    CHAR_DATA *victim;

    if (!is_abom(ch) || ch->pcdata->abomcuring > 0 || !has_blackeyes(ch)) {
      send_to_char("This is an abomination power.\n\r", ch);
      return;
    }
    if (in_fight(ch) || room_hostile(ch->in_room) || is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (ch->pcdata->abomtime > current_time) {
      send_to_char("You haven't yet gathered enough fear to be powerful enough to do that.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "sacrifice")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!is_helpless(victim) || IS_NPC(victim)) {
        send_to_char("They need to be helpless first.\n\r", ch);
        return;
      }
      habit_corpse(ch, "electrocution");
      act("You lay a person on top of $N and then press your hands into their chest, black power coarses down your form and through their bodies.", ch, NULL, victim, TO_CHAR);
      act("$n lays a person on top of you and then presses $S hands into their chest, black power coarses down their form and through the person above you before driving into you, making your muscles sieze up in pain before everything fades to darkness.", ch, NULL, victim, TO_VICT);
      act("$n lays a person on top of $N and then presses $s hands into their chest, black power coarses down their form and through the bodies.", ch, NULL, victim, TO_NOTVICT);
      victim->pcdata->sleeping = 50;
      victim->pcdata->coma = current_time + (3600 * 24 * 3);
      ch->pcdata->abomcuring = current_time + (3600 * 8);
      ch->pcdata->abominfecting = 0;
      send_to_char("You feel the pain and thirst for fear starting to slacken.\n\r", ch);
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * 60 * 8;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MARKED;
      affect_to_char(ch, &af);

      return;
    }
    else if (!str_cmp(arg1, "infection")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!is_helpless(victim) || IS_NPC(victim)) {
        send_to_char("They need to be helpless first.\n\r", ch);
        return;
      }
      act("With a concentrated look at $N you start the process of power transfer.", ch, NULL, victim, TO_CHAR);
      ch->pcdata->abomcuring = current_time + (3600 * 24);
      victim->pcdata->abominfecting =
      current_time + (3600 * 24 * number_range(4, 8));
      ch->pcdata->abomtime = 0;
      ch->pcdata->abominfecting = 0;

      return;
    }
    send_to_char("Syntax: Abomcure sacrifice (person)/Abomcure infection (person)\n\r", ch);
  }

  vector<LAIR_TYPE *> LairVect;
  LAIR_TYPE *nulllair;

  void fread_lair(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    LAIR_TYPE *lair;

    lair = new_lair();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Abduct", lair->abduct_date, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          LairVect.push_back(lair);
          return;
        }
        KEY("Eat", lair->eat_date, fread_number(fp));
        break;
      case 'K':
        KEY("Killed", lair->killed, fread_number(fp));
        break;
      case 'M':
        KEY("Mob", lair->mob, fread_number(fp));
        break;
      case 'P':
        KEY("Population", lair->population, fread_number(fp));
        break;
      case 'R':
        KEY("Room", lair->room, fread_number(fp));
        break;
      case 'V':
        KEY("Victim", lair->victim, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_lair: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_lairs() {
    nulllair = new_lair();
    FILE *fp;

    if ((fp = fopen(LAIR_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_lairs: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "LAIR")) {
          fread_lair(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_lairs: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open lairs.txt", 0);
      exit(0);
    }
  }
  void save_lairs(bool backup) {
    FILE *fpout;

    if ((fpout = fopen(LAIR_FILE, "w")) == NULL) {
      bug("Cannot open lair.txt for writing", 0);
      return;
    }

    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }

      fprintf(fpout, "#Lair\n");
      fprintf(fpout, "Victim %s~\n", (*it)->victim);
      fprintf(fpout, "Mob %d\n", (*it)->mob);
      fprintf(fpout, "Room %d\n", (*it)->room);
      fprintf(fpout, "Population %d\n", (*it)->population);
      fprintf(fpout, "Killed %d\n", (*it)->killed);
      fprintf(fpout, "Abduct %d\n", (*it)->abduct_date);
      fprintf(fpout, "Eat %d\n", (*it)->eat_date);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  bool in_lair(CHAR_DATA *mob) {
    if (!IS_NPC(mob) || mob->in_room == NULL)
    return FALSE;
    if (!forest_monster(mob))
    return FALSE;
    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->mob == mob->pIndexData->vnum && (*it)->room == mob->in_room->vnum)
      return TRUE;
    }
    return FALSE;
  }

  bool pc_in_lair(CHAR_DATA *mob) {
    if (IS_NPC(mob) || mob->in_room == NULL)
    return FALSE;
    if (!IS_FLAG(mob->act, PLR_BOUND))
    return FALSE;
    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->room == mob->in_room->vnum)
      return TRUE;
    }
    return FALSE;
  }

  bool lair_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->room == room->vnum)
      return TRUE;
    }
    return FALSE;
  }

  bool valid_lair_room(ROOM_INDEX_DATA *room) {
    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->room == room->vnum)
      return FALSE;

      ROOM_INDEX_DATA *lroom = get_room_index((*it)->room);
      if (lroom != NULL && get_dist(lroom->x, lroom->y, room->x, room->y <= 5))
      return FALSE;
    }
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->havenroom == room->vnum)
      return FALSE;
      if ((*it)->worldroom == room->vnum)
      return FALSE;
    }

    if (room->sector_type != SECT_FOREST)
    return FALSE;

    if (room->x >= 1000024 && room->x <= 1000026 && room->y >= 1000024 && room->y <= 1000026)
    return FALSE;
    if (room->x >= 2000024 && room->x <= 2000026 && room->y >= 2000024 && room->y <= 2000026)
    return FALSE;
    if (room->x >= 3000024 && room->x <= 3000026 && room->y >= 3000024 && room->y <= 3000026)
    return FALSE;
    if (room->x >= 4000024 && room->x <= 4000026 && room->y >= 4000024 && room->y <= 4000026)
    return FALSE;

    if (room->area->world != WORLD_EARTH) {
      if (room->x < room->area->minx + 10)
      return FALSE;
      if (room->x > room->area->maxx - 10)
      return FALSE;
      if (room->y < room->area->miny + 10)
      return FALSE;
      if (room->y > room->area->maxy - 10)
      return FALSE;
    }
    else {
      if (room->x < room->area->minx + 3)
      return FALSE;
      if (room->x > room->area->maxx - 3)
      return FALSE;
      if (room->y < room->area->miny + 3)
      return FALSE;
      if (room->y > room->area->maxy - 3)
      return FALSE;
    }

    for (int i = 0; i < 10; i++) {
      if (i == DIR_UP || i == DIR_DOWN)
      continue;

      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL) {
        if (room->exit[i]->u1.to_room->sector_type != SECT_FOREST)
        return FALSE;
        for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
        it != LairVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;

          if ((*it)->room == room->exit[i]->u1.to_room->vnum)
          return FALSE;
        }
        for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
        it != gatewayVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;

          if ((*it)->havenroom == room->exit[i]->u1.to_room->vnum)
          return FALSE;
          if ((*it)->worldroom == room->exit[i]->u1.to_room->vnum)
          return FALSE;
        }
      }
    }
    return TRUE;
  }

  ROOM_INDEX_DATA *get_lair_room(int world) {
    int avnum = -1;
    if (world == WORLD_OTHER)
    avnum = OTHER_FOREST_VNUM;
    if (world == WORLD_WILDS)
    avnum = WILDS_FOREST_VNUM;
    if (world == WORLD_GODREALM)
    avnum = GODREALM_FOREST_VNUM;
    if (world == WORLD_HELL)
    avnum = HELL_FOREST_VNUM;

    if (avnum == -1)
    return NULL;

    AREA_DATA *pArea = get_area_data(avnum);

    for (int x = 0; x < 20; x++) {
      ROOM_INDEX_DATA *room = room_by_coordinates(
      number_range(pArea->minx + 10, pArea->maxx - 10), number_range(pArea->minx + 10, pArea->maxx - 10), 0);
      if (room != NULL && room->sector_type == SECT_FOREST) {
        if (valid_lair_room(room) && number_percent() % 2 == 0)
        return room;
      }
    }
    for (int i = number_range(pArea->min_vnum, pArea->max_vnum - 1);
    i < pArea->max_vnum; i++) {
      ROOM_INDEX_DATA *room = get_room_index(i);
      if (room != NULL && room->sector_type == SECT_FOREST) {
        if (valid_lair_room(room) && number_percent() % 3 == 0)
        return room;
      }
    }
    for (int i = pArea->min_vnum; i < pArea->max_vnum; i++) {
      ROOM_INDEX_DATA *room = get_room_index(i);
      if (room != NULL && room->sector_type == SECT_FOREST) {
        if (valid_lair_room(room))
        return room;
      }
    }

    return NULL;
  }

  ROOM_INDEX_DATA *get_random_outerforest(void) {
    int avnum = number_range(19, 21);
    AREA_DATA *pArea = get_area_data(avnum);

    for (int x = 0; x < 20; x++) {
      ROOM_INDEX_DATA *room =
      room_by_coordinates(number_range(pArea->minx + 3, pArea->maxx - 3), number_range(pArea->miny + 3, pArea->maxy - 3), 0);
      if (room != NULL && room->sector_type == SECT_FOREST) {
        if (valid_lair_room(room) && number_percent() % 2 == 0)
        return room;
      }
    }
    //    for(int i=number_range(pArea->min_vnum, //    pArea->max_vnum-1);i<pArea->max_vnum;i++)
    //    {
    //        ROOM_INDEX_DATA *room = get_room_index(i);
    //        if(room != NULL && room->sector_type == SECT_FOREST)
    //        {
    //            if(valid_lair_room(room) && number_percent() % 3 == 0)
    //                return room;
    //        }
    //    }
    for (int i = pArea->min_vnum; i < pArea->max_vnum; i++) {
      ROOM_INDEX_DATA *room = get_room_index(i);
      if (room != NULL && room->sector_type == SECT_FOREST) {
        if (valid_lair_room(room))
        return room;
      }
    }

    return NULL;
  }

  _DOFUN(do_laircheck) {
    ROOM_INDEX_DATA *room = get_random_outerforest();
    if (room != NULL)
    printf_to_char(ch, "Room: %d. X:%d, Y:%d\n\r", room->vnum, room->x, room->y);
  }

  void append_messages(CHAR_DATA *ch, char *message) {
    char buf[MSL];
    sprintf(buf, "%s\n%s", ch->pcdata->messages, message);
    free_string(ch->pcdata->messages);
    ch->pcdata->messages = str_dup(buf);
  }

  ROOM_INDEX_DATA *random_inner_forest(void) {
    ROOM_INDEX_DATA *room;
    if (number_percent() % 3 == 0) {
      for (int i = 0; i < 10; i++) {
        room = room_by_coordinates(number_range(1, 70), number_range(73, 99), 0);
        if (room->sector_type == SECT_FOREST)
        return room;
      }
    }
    if (number_percent() % 2 == 0) {
      for (int i = 0; i < 10; i++) {
        room = room_by_coordinates(number_range(1, 70), number_range(-28, -2), 0);
        if (room->sector_type == SECT_FOREST)
        return room;
      }
    }
    for (int i = 0; i < 10; i++) {
      room = room_by_coordinates(number_range(-27, -2), number_range(-28, 99), 0);
      if (room->sector_type == SECT_FOREST)
      return room;
    }
    return NULL;
  }

  void monster_abduction(CHAR_DATA *mob, CHAR_DATA *victim) {
    if (is_invader(mob)) {
      if (number_percent() % 2 != 0)
      return;
      if (number_percent() % 3 == 0 && victim->wounds < 3 && victim->wounds > 0) {
        wound_char_absolute(victim, 3);
        printf_to_char(victim, "%s delivers a disabling blow to you.\n\r", PERS(mob, victim));
        act("$n delivers a disabling blow to $N.", mob, NULL, victim, TO_NOTVICT);
        dact("$n delivers a disabling blow to $N.", mob, NULL, victim, DISTANCE_MEDIUM);
      }
      else {
        if (!IS_FLAG(victim->act, PLR_BOUND) && number_percent() % 2 == 0) {
          SET_FLAG(victim->act, PLR_BOUND);
          if (number_percent() % 2 != 0)
          SET_FLAG(victim->act, PLR_BOUNDFEET);
        }
        ROOM_INDEX_DATA *toroom = random_inner_forest();
        if (toroom != NULL) {
          printf_to_char(victim, "%s drags you off into the forest for later.\n\r", PERS(mob, victim));
          act("$n drags $N off.", mob, NULL, victim, TO_NOTVICT);
          dact("$n drags $N off.", mob, NULL, victim, DISTANCE_MEDIUM);
          mob->ttl = 1;
          char_from_room(victim);
          char_to_room(victim, toroom);
        }
      }
      return;
    }

    if (!IS_FLAG(victim->act, PLR_BOUND))
    SET_FLAG(victim->act, PLR_BOUND);
    if (!IS_FLAG(victim->act, PLR_BOUNDFEET))
    SET_FLAG(victim->act, PLR_BOUNDFEET);

    int mobpoint = -1;
    for (int i = 0; i < 100 && mobpoint == -1; i++) {
      if (monster_table[i].vnum == mob->pIndexData->vnum)
      mobpoint = i;
    }
    int population = room_level(mob->in_room) / get_demon_lvl(mobpoint);
    ROOM_INDEX_DATA *desti = get_lair_room(monster_table[mobpoint].world);
    if (desti == NULL)
    return;
    bool fromhaven = FALSE;
    if (in_haven(mob->in_room))
    fromhaven = TRUE;
    act("$n drags $N off deeper into the forest.", mob, NULL, victim, TO_NOTVICT);
    dact("$n drags $N off deeper into the forest.", mob, NULL, victim, DISTANCE_MEDIUM);
    act("$n drags you off into the forest.", mob, NULL, victim, TO_VICT);

    victim->pcdata->offworld_protection = current_time + (3600 * 24 * 10);
    char_from_room(mob);
    char_to_room(mob, desti);
    char_from_room(victim);
    char_to_room(victim, desti);

    if (higher_power(victim)) {
      wound_char(victim, 3);
      WAIT_STATE(victim, PULSE_PER_SECOND * 20);
      return;
    }

    LAIR_TYPE *lair = new_lair();
    free_string(lair->victim);
    lair->victim = str_dup(victim->name);
    lair->room = desti->vnum;
    lair->population = UMAX(1, population);
    lair->population = UMIN(lair->population, 5);
    lair->abduct_date = current_time;
    lair->mob = mob->pIndexData->vnum;
    time_info.mutilate_hours--;
    int hours = UMAX(
    1, number_range(time_info.mutilate_hours, time_info.mutilate_hours * 2));
    lair->eat_date = current_time + (hours * 3600);
    lair->valid = TRUE;
    LairVect.push_back(lair);
    char buf[MSL];
    if (monster_table[mobpoint].world == WORLD_OTHER)
    sprintf(
    buf, "The scouts report seeing %s being abducted and taken into the other.", get_intro(victim));
    else if (monster_table[mobpoint].world == WORLD_WILDS)
    sprintf(
    buf, "The scouts report seeing %s being abducted and taken into the wilds.", get_intro(victim));
    else if (monster_table[mobpoint].world == WORLD_GODREALM)
    sprintf(buf, "The scouts report seeing %s being abducted and taken into the godrealm.", get_intro(victim));
    else if (monster_table[mobpoint].world == WORLD_HELL)
    sprintf(buf, "The scouts report seeing %s being abducted and taken into hell.", get_intro(victim));

    if (fromhaven == TRUE)
    scout_report(buf);
    else {
      if (monster_table[mobpoint].world == WORLD_OTHER)
      sprintf(buf, "The scouts report seeing %s being abducted in the other.", get_intro(victim));
      else if (monster_table[mobpoint].world == WORLD_WILDS)
      sprintf(buf, "The scouts report seeing %s being abducted in the wilds.", get_intro(victim));
      else if (monster_table[mobpoint].world == WORLD_GODREALM)
      sprintf(buf, "The scouts report seeing %s being abducted in the godrealm.", get_intro(victim));
      else if (monster_table[mobpoint].world == WORLD_HELL)
      sprintf(buf, "The scouts report seeing %s being abducted in hell.", get_intro(victim));
      scout_report(buf);
    }
  }

  int checkroom(char *chname) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(chname)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: checkroom");

      if (!load_char_obj(&d, chname))
      return -1;

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(chname));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim))
    return -1;

    if (victim->in_room == NULL)
    return -1;
    int room = victim->in_room->vnum;
    if (!online)
    free_char(victim);
    return room;
  }
  void corpseless_kill(CHAR_DATA *victim) {
    if (IS_NPC(victim))
    return;

    if (IS_FLAG(victim->act, PLR_DEAD) && victim->in_room->vnum == 98)
    return;

    if (is_gm(victim))
    return;

    inherit(victim);

    SET_FLAG(victim->act, PLR_DEAD);
    victim->pcdata->final_death_date = current_time;
    char_from_room(victim);
    char_to_room(victim, get_room_index(98));

    if (victim->wounds < 4)
    sympathy_pain(victim->pcdata->pledge, 4);

    send_to_char("So you died, that sucks huh? Anyway feel free to chill out here until you work out what you wanna reroll as.\n\r", victim);

    victim->wounds = 0;
    if (safe_strlen(victim->pcdata->enthralling) > 2)
    breakcontrol(victim->pcdata->enthralling, 3);
    if (safe_strlen(victim->pcdata->enrapturing) > 2)
    breakcontrol(victim->pcdata->enrapturing, 4);
    free_string(victim->pcdata->enrapturing);
    victim->pcdata->enrapturing = str_dup("");
    free_string(victim->pcdata->enthralling);
    victim->pcdata->enthralling = str_dup("");
  }

  void lair_kill(char *chname, int mobvnum) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(chname)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: lair kill");

      if (!load_char_obj(&d, chname))
      return;

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(chname));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim))
    return;

    if (in_haven(victim->in_room) || victim->in_room->area->world == WORLD_EARTH)
    return;

    int mobpoint = -1;
    for (int i = 0; i < 100 && mobpoint == -1; i++) {
      if (monster_table[i].vnum == mobvnum)
      mobpoint = i;
    }
    free_string(victim->pcdata->deathcause);

    if (monster_table[mobpoint].capture == CAPTURE_EAT) {
      victim->pcdata->deathcause = str_dup("being eaten alive.");
      append_messages(victim, "You finally succumb to your injuries and die as your captors continue to devour your body.");
    }
    if (monster_table[mobpoint].capture == CAPTURE_TORMENT) {
      victim->pcdata->deathcause = str_dup("being tortured to death.");
      append_messages(victim, "You finally succumb to your injuries and die as your captors continue to torture you.");
    }
    if (monster_table[mobpoint].capture == CAPTURE_KIDNAP) {
      victim->pcdata->deathcause = str_dup("privation");
      append_messages(victim, "You are finally sold onto your new master to live out the rest of your life in slavery.");
    }
    if (monster_table[mobpoint].capture == CAPTURE_EGGS) {
      victim->pcdata->deathcause = str_dup("massive internal haemorrhaging.");
      append_messages(victim, "You die as your chest bursts open from the inside in a spray of blood.");
    }
    if (monster_table[mobpoint].capture == CAPTURE_TURN) {
      victim->pcdata->deathcause = str_dup("being transformed.");
      append_messages(victim, "Your last memories of your human life slip away as you fully transform into something new.");
    }

    if (monster_table[mobpoint].capture == CAPTURE_TURN || monster_table[mobpoint].capture == CAPTURE_KIDNAP)
    corpseless_kill(victim);
    else
    real_kill(victim, victim);
    if (IS_FLAG(victim->act, PLR_BOUND))
    REMOVE_FLAG(victim->act, PLR_BOUND);
    if (IS_FLAG(victim->act, PLR_BOUNDFEET))
    REMOVE_FLAG(victim->act, PLR_BOUNDFEET);

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  char *lair_maim(CHAR_DATA *ch) {
    int random = number_percent() % 12;

    switch (random) {
    case 0:
      return "right arm below the elbow";
      break;
    case 1:
      return "right arm below the shoulder";
      break;
    case 2:
      return "left arm below the elbow";
      break;
    case 3:
      return "left arm below the shoulder";
      break;
    case 4:
      return "right leg below the knee";
      break;
    case 5:
      return "right leg below the thigh";
      break;
    case 6:
      return "left leg below the knee";
      break;
    case 7:
    case 8:
      return "left eye";
    case 9:
    case 10:
      return "right eye";
    default:
      return "left leg below the thigh";
      break;
    }
  }

  void lair_mean(char *chname, int mobvnum) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(chname)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: lair mean");

      if (!load_char_obj(&d, chname))
      return;

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(chname));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim))
    return;
    if (in_haven(victim->in_room) || victim->in_room->area->world == WORLD_EARTH)
    return;
    int mobpoint = -1;
    for (int i = 0; i < 100 && mobpoint == -1; i++) {
      if (monster_table[i].vnum == mobvnum)
      mobpoint = i;
    }
    char maim[MSL];
    char longmaim[MSL];

    if (monster_table[mobpoint].capture == CAPTURE_EAT) {
      sprintf(maim, "%s", lair_maim(victim));
      if (safe_strlen(victim->pcdata->maim) > 3)
      sprintf(longmaim, "%s is missing %s %s", (victim->sex == SEX_MALE) ? "he" : "he", (victim->sex == SEX_MALE) ? "his" : "her", maim);
      else
      sprintf(longmaim, "is missing %s %s", (victim->sex == SEX_MALE) ? "his" : "her", maim);
      maim_char(victim, longmaim);
      sprintf(buf, "Your captors devour %s.", maim);
      append_messages(victim, buf);
      wound_char_absolute(victim, 2);
    }
    if (monster_table[mobpoint].capture == CAPTURE_TORMENT) {
      sprintf(maim, "%s", lair_maim(victim));
      if (safe_strlen(victim->pcdata->maim) > 3)
      sprintf(longmaim, "%s is missing %s %s", (victim->sex == SEX_MALE) ? "he" : "he", (victim->sex == SEX_MALE) ? "his" : "her", maim);
      else
      sprintf(longmaim, "is missing %s %s", (victim->sex == SEX_MALE) ? "his" : "her", maim);
      maim_char(victim, longmaim);
      sprintf(buf, "Your captors cut off %s to make you scream.", maim);
      append_messages(victim, buf);
      wound_char_absolute(victim, 2);
    }
    if (monster_table[mobpoint].capture == CAPTURE_EGGS) {
      victim->pcdata->egg_date = current_time;
      victim->pcdata->egg_daddy = mobvnum;
      if (mobvnum == 14)
      append_messages(victim, "The creatures force a series of large seeds down your throat and make you swallow them.");
      else
      append_messages(
      victim, "The creatures implant their eggs into your lower torso.");
      wound_char_absolute(victim, 1);
    }
    if (monster_table[mobpoint].capture == CAPTURE_TURN) {
      if (mobvnum == 17 || mobvnum == 42) {
        sprintf(maim, "%s", lair_maim(victim));
        if (safe_strlen(victim->pcdata->maim) > 3)
        sprintf(longmaim, "%s %s is necrotic and unresponsive", (victim->sex == SEX_MALE) ? "his" : "her", maim);
        else
        sprintf(longmaim, "has a necrotic and unresponsive %s", maim);
        maim_char(victim, longmaim);
        sprintf(buf, "You twist in pain as the flesh of your %s dies, transforming into something necrotic and unresponsive.", maim);
        append_messages(victim, buf);
      }
      if (mobvnum == 29) {
        sprintf(maim, "%s", lair_maim(victim));
        if (safe_strlen(victim->pcdata->maim) > 3)
        sprintf(longmaim, "%s %s is made of sandstone", (victim->sex == SEX_MALE) ? "his" : "her", maim);
        else
        sprintf(longmaim, "has a sandstone %s", maim);
        maim_char(victim, longmaim);
        sprintf(buf, "You twist in pain as the flesh on %s falls away before your eyes, leaving a sandstone limb beneath that refuses to obey your instructions.", maim);
        append_messages(victim, buf);
      }
    }

    if (number_percent() % 3 != 0) {
      if (in_world(victim) == WORLD_HELL) {
        char_from_room(victim);
        char_to_room(victim, get_room_index(362600));
        if (IS_FLAG(victim->act, PLR_BOUND))
        REMOVE_FLAG(victim->act, PLR_BOUND);
        if (IS_FLAG(victim->act, PLR_BOUNDFEET))
        REMOVE_FLAG(victim->act, PLR_BOUNDFEET);
        append_messages(
        victim, "You manage to slip away while your captors are distracted.\n\r");
      }
      else if (in_world(victim) == WORLD_GODREALM) {
        char_from_room(victim);
        char_to_room(victim, get_room_index(342600));
        if (IS_FLAG(victim->act, PLR_BOUND))
        REMOVE_FLAG(victim->act, PLR_BOUND);
        if (IS_FLAG(victim->act, PLR_BOUNDFEET))
        REMOVE_FLAG(victim->act, PLR_BOUNDFEET);
        append_messages(
        victim, "You manage to slip away while your captors are distracted.\n\r");
      }
      else if (in_world(victim) == WORLD_WILDS) {
        char_from_room(victim);
        char_to_room(victim, get_room_index(322600));
        if (IS_FLAG(victim->act, PLR_BOUND))
        REMOVE_FLAG(victim->act, PLR_BOUND);
        if (IS_FLAG(victim->act, PLR_BOUNDFEET))
        REMOVE_FLAG(victim->act, PLR_BOUNDFEET);
        append_messages(
        victim, "You manage to slip away while your captors are distracted.\n\r");
      }
      else if (in_world(victim) == WORLD_OTHER) {
        char_from_room(victim);
        char_to_room(victim, get_room_index(302600));
        if (IS_FLAG(victim->act, PLR_BOUND))
        REMOVE_FLAG(victim->act, PLR_BOUND);
        if (IS_FLAG(victim->act, PLR_BOUNDFEET))
        REMOVE_FLAG(victim->act, PLR_BOUNDFEET);
        append_messages(
        victim, "You manage to slip away while your captors are distracted.\n\r");
      }
    }

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  int vnum_pop(ROOM_INDEX_DATA *room, int vnum) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (!IS_NPC(victim))
      continue;
      if (victim->pIndexData->vnum == vnum)
      count++;
    }
    return count;
  }

  void lair_update() {
    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->killed > 0)
      (*it)->killed--;
      int room = checkroom((*it)->victim);
      if (room != (*it)->room) {
        (*it)->valid = FALSE;
        continue;
      }
      if (current_time > (*it)->abduct_date + (3600 * 24 * 8)) {
        lair_kill((*it)->victim, (*it)->mob);
        (*it)->valid = FALSE;
        continue;
      }
      if ((*it)->eat_date != 0 && current_time > (*it)->eat_date) {
        lair_mean((*it)->victim, (*it)->mob);
        (*it)->eat_date = 0;
        time_info.mutilate_hours = 12;
      }
    }
  }

  void lair_mobkill(ROOM_INDEX_DATA *room, int vnum) {
    if (room == NULL)
    return;
    if (room->area->vnum < 22 || room->area->vnum > 25)
    return;
    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->mob != vnum)
      continue;
      ROOM_INDEX_DATA *in_room = get_room_index((*it)->room);

      if (get_dist(room->x, room->y, in_room->x, in_room->y) < 4)
      (*it)->killed++;
    }
  }

  bool proximate_fight(ROOM_INDEX_DATA *room) {
    CHAR_DATA *to;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (to == NULL)
        continue;
        if (IS_NPC(to))
        continue;
        if (to->in_room->area->vnum != room->area->vnum)
        continue;
        if (get_dist(to->in_room->x, to->in_room->y, room->x, room->y) <= 4) {
          if (in_fight(to))
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  void lair_populate() {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    for (vector<LAIR_TYPE *>::iterator it = LairVect.begin();
    it != LairVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      int pop = vnum_pop(get_room_index((*it)->room), (*it)->mob);
      if (pop < (*it)->population - (*it)->killed && !proximate_fight(get_room_index((*it)->room))) {
        pMobIndex = get_mob_index((*it)->mob);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, get_room_index((*it)->room));
        mob->hit = max_hp(mob);
        free_string(mob->aggression);
        mob->aggression = str_dup("all");
        mob->x = 25;
        mob->y = 25;
      }
      else if (pop > (*it)->population - (*it)->killed) {
        CHAR_DATA *victim;
        bool found = FALSE;
        for (CharList::iterator ij = get_room_index((*it)->room)->people->begin();
        ij != get_room_index((*it)->room)->people->end() && found == FALSE;
        ++ij) {
          victim = *ij;

          if (!IS_NPC(victim))
          continue;
          if (victim->pIndexData->vnum == (*it)->mob) {
            char_from_room(victim);
            char_to_room(victim, get_room_index(1));
            victim->ttl = 1;
            found = TRUE;
          }
        }
      }
    }
  }

  void psychic_feast(CHAR_DATA *ch, int type, int mod) {
    CHAR_DATA *victim;
    char buf[MSL];
    int amount = 0;
    if (ch->in_room == NULL)
    return;
    if (IS_NPC(ch))
    return;
    int cap;
    if (get_tier(ch) == 1)
    cap = 10000;
    else if (get_tier(ch) == 2)
    cap = 12000;
    else
    cap = 20000;

    if (IS_FLAG(ch->comm, COMM_AFK))
    return;
    if (is_gm(ch))
    return;
    if (is_ghost(ch))
    return;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;
      if (IS_NPC(victim))
      continue;
      if (is_gm(victim) || is_ghost(victim))
      continue;
      if (IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->guest_type != GUEST_NIGHTMARE)
      continue;
      if (higher_power(victim))
      continue;

      if (type == -10 && ch != victim && !is_helpless(victim)) {
        take_lifeforce(victim, 5, "");
        continue;
      }

      if (!is_super(victim))
      continue;
      if (get_tier(victim) <= get_tier(ch) && (get_tier(victim) < get_tier(ch) || !IS_FLAG(victim->comm, COMM_FEEDING) || IS_FLAG(ch->comm, COMM_FEEDING)))
      continue;
      if (ch->in_room != victim->in_room)
      continue;
      if (victim->pcdata->sleeping > 0)
      continue;
      if (institute_room(victim->in_room) && (college_student(victim, FALSE) || clinic_patient(victim)))
      continue;

      if (type == PSYCHIC_FEAR && !is_helpless(victim)) // Every minute.
      {
        if (get_tier(victim) >= 3 || (get_tier(victim) == 2 && IS_FLAG(victim->comm, COMM_FEEDING))) {
          amount = get_tier(victim) - get_tier(ch);
          if (IS_FLAG(victim->comm, COMM_FEEDING))
          amount++;
          amount *= 20;
          amount /= (pc_pop(ch->in_room) * pc_pop(ch->in_room));
          amount *= mod;
          int mult = 0;
          if (base_lifeforce(ch) > 8500) {
            mult = base_lifeforce(ch) - 8500;
            mult /= 100;
            amount *= mult;
            amount /= 10;
          }
          amount = UMAX(amount, 1);
          amount = UMIN(amount, base_lifeforce(ch) - 8500);
          sprintf(buf, "FEEDING FEAR: %s(%d) feeds %s(%d): %d taken, %d given, %d cap, %d base, %d pop, %d mult", ch->name, get_tier(ch), victim->name, get_tier(victim), amount, amount / 2, UMIN(cap - base_lifeforce(victim), amount / 2), base_lifeforce(ch), pc_pop(ch->in_room), mult);
          log_string(buf);
          if (amount <= 0)
          return;

          give_lifeforce(victim, UMAX(0, UMIN(cap - base_lifeforce(victim), amount / 2)), "Fear Feeding");
          if (IS_FLAG(victim->comm, COMM_FEEDING)) {
            if (same_faction(ch, victim))
            victim->pcdata->monster_fed += amount * 2;
            else
            victim->pcdata->monster_fed += amount / 10;
          }
          take_lifeforce(ch, amount, "Fear feeding");
          ch->pcdata->ill_count += amount / 3;
        }
      }
      if (type == PSYCHIC_ANGER && (!is_sparring(victim) || !IS_FLAG(victim->comm, COMM_FEEDING)) && (victim->fight_fast == FALSE || victim->in_room->area->vnum < 9)) // Every Round.
      {
        if (get_tier(victim) >= 4 || (get_tier(victim) == 3 && IS_FLAG(victim->comm, COMM_FEEDING))) {
          amount = get_tier(victim) - get_tier(ch);
          if (IS_FLAG(victim->comm, COMM_FEEDING))
          amount++;

          amount *= 12;
          amount /= (pc_pop(ch->in_room) * pc_pop(ch->in_room));
          amount *= mod;
          int mult = 0;
          if (base_lifeforce(ch) > 8500) {
            mult = base_lifeforce(ch) - 8500;
            mult /= 100;
            amount *= mult;
            amount /= 15;
          }

          amount = UMAX(amount, 1);
          amount = UMIN(amount, base_lifeforce(ch) - 8500);

          sprintf(buf, "FEEDING ANGER: %s(%d) feeds %s(%d): %d taken, %d given, %d cap, %d mult, %d pop, %d base", ch->name, get_tier(ch), victim->name, get_tier(victim), amount, amount, UMIN(cap - base_lifeforce(victim), amount), mult, pc_pop(ch->in_room), base_lifeforce(ch));
          log_string(buf);

          if (amount > 0) {
            give_lifeforce(victim, UMAX(0, UMIN(cap - base_lifeforce(victim), amount)), "Anger Feeding");
            take_lifeforce(ch, amount, "Anger Feeding");

            if (IS_FLAG(victim->comm, COMM_FEEDING)) {
              if (same_faction(ch, victim))
              victim->pcdata->monster_fed += amount * 2;
              else
              victim->pcdata->monster_fed += amount / 10;
            }
          }
          if (base_lifeforce(ch) < 8000 && number_percent() % 11 == 0 && ch->pcdata->sleeping <= 0) {
            act("$n slumps sideways to the ground.", ch, NULL, NULL, TO_ROOM);
            act("You pass out.", ch, NULL, NULL, TO_CHAR);
            ch->pcdata->coma = current_time + (3600 * number_range(1, 5));
            ch->pcdata->sleeping = 10;
          }
        }
      }
      if (type == PSYCHIC_LUST) // Every minute.
      {
        if (higher_power(ch) || get_tier(victim) >= 4 || (get_tier(victim) == 3 && IS_FLAG(victim->comm, COMM_FEEDING))) {
          amount = get_tier(victim) - get_tier(ch);
          if (IS_FLAG(victim->comm, COMM_FEEDING))
          amount++;

          amount *= 18;
          amount /= pc_pop(ch->in_room);
          amount *= mod;
          amount = UMAX(amount, 1);
          amount = UMIN(amount, base_lifeforce(ch) - 5000);
          if (is_helpless(victim) || is_pinned(victim)) {
            if (IS_FLAG(victim->act, PLR_BOUND) && victim->pcdata->selfbondage != 1) {
              victim->pcdata->monster_fed -= amount * 5;
              amount = amount / 10;
            }
            else if (is_pinned(victim)) {
              victim->pcdata->monster_fed -= amount * 15;
              amount = amount / 6;
            }
            else {
              victim->pcdata->monster_fed -= amount * 25;
              amount = amount / 2;
            }
          }

          sprintf(buf, "FEEDING LUST: %s(%d) feeds %s(%d): %d taken, %d given, %d cap, %d pop, %d base", ch->name, get_tier(ch), victim->name, get_tier(victim), amount, amount / 3, UMIN(cap - base_lifeforce(victim), amount / 3), pc_pop(ch->in_room), base_lifeforce(ch));
          log_string(buf);
          if (amount <= 0)
          return;

          give_lifeforce(victim, UMAX(0, UMIN(cap - base_lifeforce(victim), amount / 4)), "Lust feeding");
          if (str_cmp(ch->pcdata->last_sexed[0], victim->name) && str_cmp(ch->pcdata->last_sexed[1], victim->name) && str_cmp(ch->pcdata->last_sexed[2], victim->name))
          amount = amount * 3 / 2;
          amount = UMIN(amount, 3800);
          if (get_tier(ch) > 1)
          amount = UMIN(amount, 3000);

          take_lifeforce(ch, amount, "Lust feeding");
          if (IS_FLAG(victim->comm, COMM_FEEDING)) {
            if (same_faction(ch, victim))
            victim->pcdata->monster_fed += amount * 2;
            else
            victim->pcdata->monster_fed += amount / 3;
          }

          ch->pcdata->ill_count += amount;
          if (base_lifeforce(ch) < 6000 && number_percent() % 3 == 0 && ch->pcdata->sleeping <= 0) {
            free_string(ch->pcdata->deathcause);
            ch->pcdata->deathcause = str_dup("a stroke.");
            act("$n slumps sideways to the ground.", ch, NULL, NULL, TO_ROOM);
            real_kill(ch, ch);
            return;
          }
          else if (base_lifeforce(ch) < 7500 && ch->pcdata->sleeping <= 0) {
            act("$n slumps sideways to the ground.", ch, NULL, NULL, TO_ROOM);
            act("You pass out.", ch, NULL, NULL, TO_CHAR);
            ch->pcdata->coma = current_time + (3600 * number_range(1, 5));
            ch->pcdata->sleeping = 10;
          }
        }
      }
      if (type == PSYCHIC_AMBIANT && !is_pinned(victim) && !is_prisoner(victim) && !in_fight(ch) && !clinic_patient(victim)) // Every minute.
      {
        if (get_tier(victim) == 5 || (get_tier(victim) == 4 && IS_FLAG(victim->comm, COMM_FEEDING))) {
          amount = get_tier(victim) - get_tier(ch);
          if (IS_FLAG(victim->comm, COMM_FEEDING))
          amount++;
          sprintf(buf, "FEEDINGAMB1: %d", amount);
          log_string(buf);
          amount *= mod;
          amount = amount * 10;
          sprintf(buf, "FEEDINGAMB2: %d", amount);
          log_string(buf);
          amount /= (pc_pop(ch->in_room) * pc_pop(ch->in_room));

          if (in_lodge(ch->in_room))
          amount /= 2;
          sprintf(buf, "FEEDINGAMB3: %d", amount);
          log_string(buf);
          if (is_helpless(victim))
          amount /= 4;

          if (public_room(ch->in_room))
          amount *= 2;
          sprintf(buf, "FEEDINGAMB4: %d", amount);
          log_string(buf);
          int mult = 0;
          if (base_lifeforce(ch) > 9000) {
            mult = base_lifeforce(ch) - 9000;
            mult /= 100;
            amount *= mult;
            amount /= 10;
          }
          sprintf(buf, "FEEDINGAMB5: %d", amount);
          log_string(buf);

          amount = UMAX(amount, 1);
          sprintf(buf, "FEEDINGAMB6: %d", amount);
          log_string(buf);

          amount = UMIN(amount, base_lifeforce(ch) - 9000);

          sprintf(buf, "FEEDING AMBIANT: %s(%d) feeds %s(%d): %d taken, %d given, %d cap, %d mult, %d pop, %d base", ch->name, get_tier(ch), victim->name, get_tier(victim), amount, amount / 2, UMIN(cap - base_lifeforce(victim), amount / 2), mult, pc_pop(ch->in_room), base_lifeforce(ch));
          log_string(buf);

          if (amount > 0) {
            give_lifeforce(
            victim, UMAX(0, UMIN(cap - base_lifeforce(victim), amount / 2)), "Ambiant feeding");
            take_lifeforce(ch, amount, "Ambiant feeding");
            ch->pcdata->ill_count += amount / 2;
            if (IS_FLAG(victim->comm, COMM_FEEDING)) {
              if (same_faction(ch, victim))
              victim->pcdata->monster_fed += amount * 2;
              else
              victim->pcdata->monster_fed += amount / 10;
            }
          }
          if (base_lifeforce(ch) < 7000 && number_percent() % 17 == 0 && ch->pcdata->sleeping <= 0) {
            act("$n slumps sideways to the ground.", ch, NULL, NULL, TO_ROOM);
            act("You pass out.", ch, NULL, NULL, TO_CHAR);
            ch->pcdata->coma = current_time + (3600 * number_range(1, 5));
            ch->pcdata->sleeping = 10;
          }
        }
      }
    }
  }

  _DOFUN(do_feed) {
    if (!is_super(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_FEEDING)) {
      send_to_char("You stop focusing on feeding from ambient psychic energies.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_FEEDING);
    }
    else {
      send_to_char("You focus, sending out your senses to feed off the ambient psychic energy in the room.\n\r", ch);
      SET_FLAG(ch->comm, COMM_FEEDING);
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);
    }
  }

  _DOFUN(do_murder) {
    CHAR_DATA *victim;
    char arg1[MSL];

    if (is_animal(ch))
    return;

    if (!IS_FLAG(ch->act, PLR_GUEST) || ch->pcdata->guest_type != GUEST_NIGHTMARE) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (ch->pcdata->murder_cooldown > 0) {
      printf_to_char(ch, "You have to earn %d more rpxp on this character before you can use that.\n\r", ch->pcdata->murder_cooldown);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      send_to_char("Slaying the living is no longer so simple./n/r", ch);
      return;
    }

    if (is_ghost(victim)) {
      send_to_char("Slaying a ghost is not so simple./n/r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (!IS_NPC(victim) && victim->pcdata->trance > 0)
    victim->pcdata->trance = -20;

    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!is_helpless(victim)) {
      send_to_char("You'd have to subdue them first.\n\r", ch);
      return;
    }
    if (is_dreaming(victim)) {
      send_to_char("Their spirit is not present.\n\r", ch);
      return;
    }
    if (!is_covered(ch, COVERS_EYES))
    act("`D$n's eyes shift to an unnatrual, reflective black.`x", ch, NULL, NULL, TO_ROOM);
    act("Your eyes turn black as your summon your power to murder $N", ch, NULL, victim, TO_CHAR);
    act("You feel a sharp pain run through your insides as your organs start to fail.", ch, NULL, NULL, TO_VICT);
    free_string(victim->pcdata->murder_name);
    victim->pcdata->murder_name = str_dup(ch->name);
    victim->pcdata->murder_timer = 60;
    ch->pcdata->murder_cooldown = 25000;
  }

  DOMAIN_TYPE *domain_by_name(char *dname) {
    for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
    it != DomainVect.end(); ++it) {
      if (!str_cmp(dname, (*it)->domain_of))
      return (*it);
    }
    return NULL;
  }

  DOMAIN_TYPE *my_domain(CHAR_DATA *ch) {
    for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
    it != DomainVect.end(); ++it) {
      if (!str_cmp(ch->name, (*it)->domain_of))
      return (*it);
    }
    return NULL;
  }
  DOMAIN_TYPE *vnum_domain(int vnum) {
    for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
    it != DomainVect.end(); ++it) {
      if (vnum == (*it)->vnum)
      return (*it);
    }
    return NULL;
  }

  DOMAIN_TYPE *domain_from_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    int i;
    for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
    it != DomainVect.end(); ++it) {
      for (i = 0; i < 250; i++) {
        if (room->vnum == (*it)->roomlist[i])
        return (*it);
        if (room->vnum == (*it)->smallshrines[i])
        return (*it);
        if (room->vnum == (*it)->medshrines[i])
        return (*it);
        if (room->vnum == (*it)->bigshrines[i])
        return (*it);
        if ((*it)->medshrines[i] > 0) {
          ROOM_INDEX_DATA *toroom = get_room_index((*it)->medshrines[i]);
          if (toroom != NULL) {
            if (toroom->area->vnum == room->area->vnum && get_dist3d(room->x, room->y, room->z, toroom->x, toroom->y, toroom->z) <= 1)
            return (*it);
          }
          else
          (*it)->medshrines[i] = 0;
        }
        if ((*it)->bigshrines[i] > 0) {
          ROOM_INDEX_DATA *toroom = get_room_index((*it)->bigshrines[i]);
          if (toroom != NULL) {
            if (toroom->area->vnum == room->area->vnum && get_dist3d(room->x, room->y, room->z, toroom->x, toroom->y, toroom->z) <= 3)
            return (*it);
          }
          else
          (*it)->bigshrines[i] = 0;
        }
      }
    }
    for (vector<STORY_TYPE *>::iterator ij = StoryVect.begin();
    ij != StoryVect.end(); ++ij) {
      if ((*ij)->valid == FALSE)
      continue;
      if ((*ij)->dedication > 0) {
        CHAR_DATA *author = get_char_world_pc((*ij)->author);
        if (author != NULL && (*ij)->time <= current_time && (*ij)->time > current_time - (3600 * 6) && author->in_room->vnum == room->vnum && !higher_power(author)) {
          return vnum_domain((*ij)->dedication);
        }
      }
    }

    return NULL;
  }

  void send_domain_home(CHAR_DATA *ch) {
    char_from_room(ch);
    char_to_room(ch, get_room_index(GMHOME));
    if (IS_FLAG(ch->comm, COMM_RUNNING))
    REMOVE_FLAG(ch->comm, COMM_RUNNING);
  }

  bool is_in_domain(CHAR_DATA *ch) {
    DOMAIN_TYPE *dom = domain_from_room(ch->in_room);
    if (dom == FALSE)
    return FALSE;
    if (!str_cmp(dom->domain_of, ch->name))
    return TRUE;
    return FALSE;
  }

  bool has_binding_circle(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    for (ed = room->places; ed; ed = ed->next) {
      if (!str_cmp(ed->keyword, "binding circle"))
      return TRUE;
    }
    return FALSE;
  }

  bool power_bound(CHAR_DATA *ch) {
    if (IS_NPC(ch) || !higher_power(ch))
    return FALSE;
    // log_string("Bind1");
    if (!has_binding_circle(ch->in_room))
    return FALSE;
    // log_string("Bind2");
    if (str_cmp(ch->pcdata->place, "binding circle"))
    return FALSE;
    // log_string(ch->pcdata->place);
    if (ch->pcdata->summon_bound <= 0)
    return FALSE;
    // log_string("Bind3");
    return TRUE;
  }

  _DOFUN(do_domain) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if(higher_power(ch)) {
      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL) {
        DOMAIN_TYPE *domain;
        domain = new_domain();
        free_string(domain->domain_of);
        domain->domain_of = str_dup(ch->name);
        domain->archetype = ch->race;
        domain->power = 5;
        int mvnum = 0;
        for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
        it != DomainVect.end(); ++it) {
          if ((*it)->vnum > mvnum)
          mvnum = (*it)->vnum;
        }
        domain->vnum = mvnum + 1;
        DomainVect.push_back(domain);
      }
    }
    if (!str_cmp(arg1, "list")) {
      for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
      it != DomainVect.end(); ++it) {
        if ((*it)->archetype == RACE_SPIRIT_FAE)
        printf_to_char(ch, "`g%s`x\n\r", (*it)->domain_of);
        else if ((*it)->archetype == RACE_SPIRIT_DEMON)
        printf_to_char(ch, "`r%s`x\n\r", (*it)->domain_of);
        else if ((*it)->archetype == RACE_SPIRIT_DIVINE)
        printf_to_char(ch, "`c%s`x\n\r", (*it)->domain_of);
        else if ((*it)->archetype == RACE_SPIRIT_GHOST)
        printf_to_char(ch, "`D%s`x\n\r", (*it)->domain_of);
        else if ((*it)->archetype == RACE_SPIRIT_CTHULIAN)
        printf_to_char(ch, "`g%s`x\n\r", (*it)->domain_of);
        else if ((*it)->archetype == RACE_SPIRIT_PRIMAL)
        printf_to_char(ch, "`y%s`x\n\r", (*it)->domain_of);
      }
      return;
    }
    if (!str_cmp(arg1, "ambient")) {
      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL) {
        send_to_char("You have to establish a domain first.\n\r", ch);
        return;
      }
      string_append(ch, &dom->ambients);
      return;
    }

    if (!str_cmp(arg1, "personalambient")) {
      if(!higher_power(ch))
      {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if(strlen(argument) < 2)
      {
        send_to_char("Syntax: domain personalambient (text).\n\r", ch);
        return;
      }
      free_string(ch->pcdata->eidilon_ambient);
      ch->pcdata->eidilon_ambient = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "describe")) {
      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL) {
        send_to_char("You have to establish a domain first.\n\r", ch);
        return;
      }
      string_append(ch, &dom->desc);
    }
    if (!str_cmp(arg1, "delete") && IS_IMMORTAL(ch)) {
      DOMAIN_TYPE *dom = vnum_domain(atoi(argument));
      if (dom == NULL) {
        send_to_char("You have to establish a domain first.\n\r", ch);
        return;
      }
      dom->valid = FALSE;
    }

    if (!str_cmp(arg1, "info")) {
      for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
      it != DomainVect.end(); ++it) {
        if (!str_cmp(argument, (*it)->domain_of) && (*it)->desc != NULL && safe_strlen((*it)->desc) > 2) {
          if ((*it)->archetype == RACE_SPIRIT_FAE)
          printf_to_char(
          ch, "`WDomain of %s`x\n\n%s\n\r", (*it)->domain_of, text_complete_box((*it)->desc, "`g*`x", get_wordwrap(ch), FALSE));
          else if ((*it)->archetype == RACE_SPIRIT_DEMON)
          printf_to_char(
          ch, "`WDomain of %s`x\n\n%s\n\r", (*it)->domain_of, text_complete_box((*it)->desc, "`r*`x", get_wordwrap(ch), FALSE));
          else if ((*it)->archetype == RACE_SPIRIT_DIVINE)
          printf_to_char(
          ch, "`WDomain of %s`x\n\n%s\n\r", (*it)->domain_of, text_complete_box((*it)->desc, "`c*`x", get_wordwrap(ch), FALSE));
          else if ((*it)->archetype == RACE_SPIRIT_GHOST)
          printf_to_char(
          ch, "`WDomain of %s`x\n\n%s\n\r", (*it)->domain_of, text_complete_box((*it)->desc, "`D*`x", get_wordwrap(ch), FALSE));
          else if ((*it)->archetype == RACE_SPIRIT_CTHULIAN)
          printf_to_char(
          ch, "`WDomain of %s`x\n\n%s\n\r", (*it)->domain_of, text_complete_box((*it)->desc, "`g*`x", get_wordwrap(ch), FALSE));
          else if ((*it)->archetype == RACE_SPIRIT_PRIMAL)
          printf_to_char(
          ch, "`WDomain of %s`x\n\n%s\n\r", (*it)->domain_of, text_complete_box((*it)->desc, "`y*`x", get_wordwrap(ch), FALSE));
        }



      }
      return;
    }
    if (!str_cmp(arg1, "who")) {
      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL)
      return;

      CHAR_DATA *victim;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL) {
          victim = d->character;
          if (IS_NPC(victim))
          continue;
          if (is_gm(victim))
          continue;
          if (victim->pcdata->in_domain != dom->vnum || victim->pcdata->domain_timer < 5)
          continue;
          if (is_dreaming(victim))
          printf_to_char(
          ch, "[`c%d`x] %s, %s\n\r", get_gmtrust(ch, victim), PERS(victim, ch), roomtitle(get_room_index(victim->pcdata->dream_room), FALSE));
          else
          printf_to_char(ch, "[`c%d`x] %s, %s\n\r", get_gmtrust(ch, victim), PERS(victim, ch), roomtitle(victim->in_room, FALSE));
        }
      }
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL) {
          victim = d->character;
          if (IS_NPC(victim))
          continue;
          if (is_gm(victim))
          continue;
          if (!is_dreaming(victim))
          continue;
          FANTASY_TYPE *fant;
          if ((fant = in_fantasy(victim)) == NULL)
          continue;
          if (str_cmp(ch->name, fant->author) || IS_FLAG(victim->comm, COMM_PRIVATE))
          continue;

          printf_to_char(ch, "[`c%d`x] %s, %s\n\r", get_gmtrust(ch, victim), dream_name(victim), roomtitle(victim->in_room, FALSE));
        }
      }

      return;
    }
    if (!str_cmp(arg1, "santified")) {
      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL)
      return;

      send_to_char("`WSanctified Rooms`x.\n\r", ch);
      for (int i = 0; i < 250; i++) {
        if (dom->smallshrines[i] != 0) {
          ROOM_INDEX_DATA *sroom = get_room_index(dom->smallshrines[i]);
          if (sroom == NULL)
          dom->smallshrines[i] = 0;
          else
          printf_to_char(ch, "%s\n\r", roomtitle(sroom, TRUE));
        }
      }
      return;
    }
    if (!str_cmp(arg1, "reset")) {
      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL)
      return;
      for (int i = 0; i < 50; i++) {
        dom->roomlist[i] = 0;
      }

      send_to_char("Done.\n\r", ch);
      return;
    }

    send_to_char("Syntax: Domain list/info/who/sanctified/ambient/personalambient/describe\n\r", ch);
  }

  void domain_xp(CHAR_DATA *ch, int amount) {
    if (IS_FLAG(ch->comm, COMM_PRIVATE))
    return;

    DOMAIN_TYPE *dom = vnum_domain(ch->pcdata->in_domain);
    if (dom == NULL)
    return;
    dom->exp += amount;
    CHAR_DATA *power = get_char_world_pc(dom->domain_of);

    if (power != NULL) {
      power->pcdata->scheme_influence += amount * 2;
    }
    if (dom->exp >= 10000 && power != NULL) {
      dom->power += 1;
      dom->exp = 0;
      message_to_char(dom->domain_of, "`cYour domain grows.`x ");
      power->newexp += 50000;
      power->newrpexp += 50000;
    }
  }

#if defined(__cplusplus)
}
#endif

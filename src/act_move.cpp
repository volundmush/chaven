#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
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
#include "tables.h"

#include <math.h>


#if defined(__cplusplus)
extern "C" {
#endif


  const char *dir_name[][2] = {
    {"north", "N"}, {"east", "E"}, {"south", "S"},
    {"west", "W"}, {"up", "U"},   {"down", "D"},
    {"northeast", "NE"}, {"northwest", "NW"},
    {"southeast", "SE"}, {"southwest", "SW"}
  };

  const char *dir_door_name[][2] = {
    {"northern", "N"}, {"eastern", "E"}, {"southern", "S"},
    {"western", "W"}, {"upward", "U"}, {"downward", "D"},
    {"northeastern", "NE"}, {"northwestern", "NW"},
    {"southeastern", "SE"}, {"southwestern", "SW"}
  };

  const char *reldir_name[][2] = {
    {"forward", "F"}, {"right", "R"}, {"backward", "B"},
    {"left", "L"}, {"up", "U"}, {"down", "D"},
    {"forwardright", "FR"}, {"forwardleft", "FL"},
    {"backwardright", "BR"}, {"backwardleft", "BL"}
  };

  // You see the tower in front of you, behind you and to the left to the right of you
  const char *relspacial[] = {
    "in front of you", "to the right of you", "behind you",
    "to the left of you", "above you", "below you",
    "in front and to the right of you", "in front and to the left of you",
    "behind and to the right of you", "behind and to the left of you"
  };
  // walks in from your right, walks out to the right of you
  const char *relincoming[] = {
    "in front of you", "from your right", "behind you",
    "from your left", "above you", "below you",
    "in front and to your right", "in front and to your left",
    "behind and to your right", "behind and to your left"
  };

  const char *relfacing[] = {
    "facing away from you", "facing right", "facing you",
    "facing left", "looking up", "looking down",
    "facing away and to the right", "facing away and to the left",
    "facing you and slightly right", "facing you and slightly left"
  };

  const char *relwalking[] = {
    "away from you", "to the right", "towards you", "to the left",
    "up", "down", "away and to the right", "away and to the left",
    "towards you and to the right", "towards you and to the left"
  };

  const sh_int turn_dir[] = {6, 8, 9, 7, 4, 5, 1, 0, 2, 3};

  const sh_int aturn_dir[] = {7, 6, 8, 9, 4, 5, 0, 3, 1, 2};

  const sh_int rev_dir[] = {2, 3, 0, 1, 5, 4, 9, 8, 7, 6};

  /*
  * Local functions.
  */
  bool will_flee args((CHAR_DATA * ch, CHAR_DATA *victim));
  bool can_see_move args((CHAR_DATA * mover, CHAR_DATA *looker));
  char *move_type args((CHAR_DATA * ch));
  int mist_prey_score args((CHAR_DATA * ch));
  int mist_lost_score args((CHAR_DATA * ch));
  int mist_score_group args((CHAR_DATA * ch));
  void split_group args((CHAR_DATA * ch, ROOM_INDEX_DATA *newroom));
  ROOM_INDEX_DATA *get_newlostroom args((ROOM_INDEX_DATA * startroom));
  CHAR_DATA *find_mist_predator args((void));
  ROOM_INDEX_DATA *get_pred_room args((CHAR_DATA * ch));
  void append_smell args((CHAR_DATA * ch, int dir, ROOM_INDEX_DATA *room));
  bool security_room_blocked args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );

  int find_door(CHAR_DATA *ch, char *arg) {
    EXIT_DATA *pexit;
    int door = -1;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL)) {
      if (!str_cmp(arg, "n") || !str_cmp(arg, "north")) {
        arg = "north";
        door = 0;
      }
      else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))
      door = 1;
      else if (!str_cmp(arg, "s") || !str_cmp(arg, "south"))
      door = 2;
      else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))
      door = 3;
      else if (!str_cmp(arg, "u") || !str_cmp(arg, "up"))
      door = 4;
      else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))
      door = 5;
      else if (!str_cmp(arg, "ne") || !str_cmp(arg, "northeast"))
      door = 6;
      else if (!str_cmp(arg, "nw") || !str_cmp(arg, "northwest"))
      door = 7;
      else if (!str_cmp(arg, "se") || !str_cmp(arg, "southeast"))
      door = 8;
      else if (!str_cmp(arg, "sw") || !str_cmp(arg, "southwest"))
      door = 9;
      else {
        for (door = 0; door <= 9; door++) {
          if ((pexit = ch->in_room->exit[door]) != NULL && IS_SET(pexit->exit_info, EX_ISDOOR) && pexit->keyword != NULL && is_name(arg, pexit->keyword))
          return door;
        }
        //        act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
        return -1;
      }
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL)) {
      for (int j = 0; j < 10; j++) {
        if (!str_cmp(arg, reldir_name[j][0]) || !str_cmp(arg, reldir_name[j][1])) {
          door = get_absoldirection(j, ch->facing);
        }
      }
    }
    if (door == -1) {
      for (door = 0; door <= 9; door++) {
        if ((pexit = ch->in_room->exit[door]) != NULL && IS_SET(pexit->exit_info, EX_ISDOOR) && pexit->keyword != NULL && is_name(arg, pexit->keyword))
        return door;
      }
      return -1;
    }

    if ((pexit = ch->in_room->exit[door]) == NULL) {
      act("I see no door $T here.", ch, NULL, arg, TO_CHAR);
      return -1;
    }

    if (!IS_SET(pexit->exit_info, EX_ISDOOR)) {
      send_to_char("You can't do that.\n\r", ch);
      return -1;
    }

    return door;
  }

  int find_exit(CHAR_DATA *ch, char *arg) {
    EXIT_DATA *pexit;
    int door = -1;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL)) {
      if (!str_cmp(arg, "n") || !str_cmp(arg, "north")) {
        arg = "north";
        door = 0;
      }
      else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))
      door = 1;
      else if (!str_cmp(arg, "s") || !str_cmp(arg, "south"))
      door = 2;
      else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))
      door = 3;
      else if (!str_cmp(arg, "u") || !str_cmp(arg, "up"))
      door = 4;
      else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))
      door = 5;
      else if (!str_cmp(arg, "ne") || !str_cmp(arg, "northeast"))
      door = 6;
      else if (!str_cmp(arg, "nw") || !str_cmp(arg, "northwest"))
      door = 7;
      else if (!str_cmp(arg, "se") || !str_cmp(arg, "southeast"))
      door = 8;
      else if (!str_cmp(arg, "sw") || !str_cmp(arg, "southwest"))
      door = 9;
      else {
        for (door = 0; door <= 9; door++) {
          if ((pexit = ch->in_room->exit[door]) != NULL)
          return door;
        }
        return -1;
      }
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL)) {
      for (int j = 0; j < 10; j++) {
        if (!str_cmp(arg, reldir_name[j][0]) || !str_cmp(arg, reldir_name[j][1])) {
          door = get_absoldirection(j, ch->facing);
        }
      }
    }
    if (door == -1) {
      for (door = 0; door <= 9; door++) {
        if ((pexit = ch->in_room->exit[door]) != NULL)
        return door;
      }
      return -1;
    }

    if ((pexit = ch->in_room->exit[door]) == NULL) {
      act("I see no exit $T here.", ch, NULL, arg, TO_CHAR);
      return -1;
    }

    return door;
  }

  int room_pop_mortals(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING) {
        continue;
      }

      victim = CH(d);

      if (victim == NULL) {
        continue;
      }

      if (IS_NPC(victim)) {
        continue;
      }

      if (is_gm(victim)) {
        continue;
      }

      if (victim->in_room == NULL) {
        continue;
      }

      if (victim->in_room == room) {
        pop++;
      }
    }
    return pop;
  }

  bool door_locked(EXIT_DATA *pexit, ROOM_INDEX_DATA *to_room, CHAR_DATA *ch) {
    if (pexit == NULL || to_room == NULL || ch == NULL || ch->in_room == NULL) {
      return FALSE;
    }

    if (IS_SET(pexit->exit_info, EX_LOCKED)) {
      if(to_room->vnum == ROOM_INS_CELL_ONE || to_room->vnum == ROOM_INS_CELL_TWO || to_room->vnum == ROOM_INS_CELL_THREE || to_room->vnum == ROOM_INS_CELL_FOUR || to_room->vnum == ROOM_INS_CELL_FIVE || to_room->vnum == ROOM_INS_CELL_SIX || to_room->vnum == ROOM_INS_CELL_SEVEN || to_room->vnum == ROOM_INS_CELL_EIGHT)
      {
        if (nonpatient_pop(to_room) > 1) {
          return TRUE;
        }
        else {
          return FALSE;
        }
      }

      if (in_sheriff(to_room) && ch->race != RACE_DEPUTY && !IS_IMMORTAL(ch)) {
        return TRUE;
      }
    }

    // institute locks
    if (institute_room(to_room) && college_house_room(to_room) == 0) {
      if (institute_staff(ch) && !institute_suspension(ch)) {
        return FALSE;
      }

      return TRUE;
    }

    if (bblocked(to_room, ch)) {
      return TRUE;
    }

    if (to_room->vnum == ROOM_MEETING_EAST && (ch->pcdata->patrol_room != to_room || ch->pcdata->patrol_timer <= 0))
    return TRUE;
    if (to_room->vnum == ROOM_MEETING_WEST && (ch->pcdata->patrol_room != to_room || ch->pcdata->patrol_timer <= 0))
    return TRUE;

    if (is_privatep(to_room) && !has_access(ch, to_room)) {
      return TRUE;
    }
    return FALSE;
  }

  void patient_move(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc != WEAR_NONE || IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;
      if (obj->size <= 2)
      continue;
      printf_to_char(
      ch, "You have no where to hide %s %s, so you leave it behind.\n\r", dropprefix(obj), obj->short_descr);
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
      return;
    }
  }

  _DOFUN(do_open) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    OBJ_DATA *obj;
    int door, value;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Open what?\n\r", ch);
      return;
    }

    if ((door = find_door(ch, arg)) >= 0) {
      /* 'open door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit = ch->in_room->exit[door];
      if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
        send_to_char("It's already open.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch)) {
        send_to_char("You cannot do that from within the nightmare.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->act, PLR_BOUNDFEET)) {
        send_to_char("You are restrained.\n\r", ch);
        return;
      }
      if (IS_SET(pexit->exit_info, EX_LOCKED)) {
        if (IS_FLAG(ch->act, PLR_BOUND)) {
          send_to_char("You are restrained.\n\r", ch);
          return;
        }

        // unlocking door
        if (is_lockable(ch->in_room->vnum)) {
          value = flag_value(exit_flags, "locked");
          if (IS_SET(ch->in_room->exit[door]->exit_info, value)) {
            ch->in_room->exit[door]->exit_info =
            ch->in_room->exit[door]->rs_flags;

            to_room = pexit->u1.to_room;
            pexit_rev = to_room->exit[rev_dir[door]];

            // reverse side
            if (pexit_rev != NULL) {
              TOGGLE_BIT(pexit_rev->rs_flags, value);
              pexit_rev->rs_flags = ch->in_room->exit[door]->rs_flags;
              pexit_rev->exit_info = ch->in_room->exit[door]->exit_info;
            }
          }
        }
        else if (door_locked(pexit, pexit->u1.to_room, ch)) {
          send_to_char("It's locked.\n\r", ch);
          return;
        }
      }

      if (IS_SET(pexit->exit_info, EX_LOCKED))
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);

      if (IS_SET(pexit->exit_info, EX_LOCKED) && is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID) {
        send_to_char("You lack the opposable thumbs to attempt that.\n\r", ch);
        return;
      }
      if (is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <= ANIMAL_SMALL) {
        send_to_char("You're too small.\n\r", ch);
        return;
      }
      if (move_helpless(ch) || is_pinned(ch)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (room_hostile(ch->in_room)) {
        act("$n moves for the door.", ch, NULL, NULL, TO_ROOM);
        start_hostilefight(ch);
        return;
      }

      REMOVE_BIT(pexit->exit_info, EX_CLOSED);

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if(to->in_room == NULL)
        to->in_room = ch->in_room;

        if (ch != to) {
          printf_to_char(to, "%s opens the %s %s.\n\r", PERS(ch, to), dir_door_name[door][0], (pexit->keyword[0] == '\0') ? "door" : pexit->keyword);
        }
        else {
          printf_to_char(to, "You open the %s %s.\n\r", dir_door_name[door][0], (pexit->keyword[0] == '\0') ? "door" : pexit->keyword);
        }
      }

      /* open the other side */
      if ((to_room = pexit->u1.to_room) != NULL) {
        for (int dir = 0; dir < 10; dir++) {
          if ((pexit_rev = to_room->exit[dir]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("The $T door opens.", *it, NULL, dir_door_name[rev_dir[door]][0], TO_CHAR);
          }
        }
      }
      return;
    }

    if ((obj = get_obj_here(ch, NULL, arg)) != NULL) {
      /* open portal */
      if (obj->item_type == ITEM_PORTAL) {
        if (!IS_SET(obj->value[1], EX_ISDOOR)) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }

        if (!IS_SET(obj->value[1], EX_CLOSED)) {
          send_to_char("It's already open.\n\r", ch);
          return;
        }

        if (IS_SET(obj->value[1], EX_LOCKED)) {
          send_to_char("It's locked.\n\r", ch);
          return;
        }
        if (obj->pIndexData->vnum == 42000 && get_skill(ch, SKILL_LARCENY) < 2) {
          send_to_char("It's locked, you might need more larceny skill to open it.\n\r", ch);
          return;
        }
        REMOVE_BIT(obj->value[1], EX_CLOSED);
        act("You open $p.", ch, obj, NULL, TO_CHAR);
        act("$n opens $p.", ch, obj, NULL, TO_ROOM);
        if (obj->pIndexData->vnum == 42000) {
          act("A wash of ethereal black and white shapes stream from the box and disappear in all directions.", ch, NULL, NULL, TO_CHAR);
          act("A wash of ethereal black and white shapes stream from the box and disappear in all directions.", ch, NULL, NULL, TO_ROOM);

          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;
            if (d->connected == CON_PLAYING) {
              send_to_char("`DAll the lights flicker, going out for a second.`x\n\r", d->character);
            }
          }
        }
        return;
      }

      /* 'open object' */
      if (obj->item_type != ITEM_CONTAINER) {
        send_to_char("That's not a container.\n\r", ch);
        return;
      }
      if (!IS_SET(obj->value[1], CONT_CLOSED)) {
        send_to_char("It's already open.\n\r", ch);
        return;
      }
      if (!IS_SET(obj->value[1], CONT_CLOSEABLE)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (IS_SET(obj->value[1], CONT_LOCKED)) {
        send_to_char("It's locked.\n\r", ch);
        return;
      }

      if (obj->pIndexData->vnum == 42000 && get_skill(ch, SKILL_LARCENY) < 2) {
        send_to_char("It's locked, you might need more larceny skill to open it.\n\r", ch);
        return;
      }

      if (obj->pIndexData->vnum == 42000) {
        act("A wash of ethereal black and white shapes stream from the box and disappear in all directions.", ch, NULL, NULL, TO_CHAR);
        act("A wash of ethereal black and white shapes stream from the box and disappear in all directions.", ch, NULL, NULL, TO_ROOM);

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          if (d->connected == CON_PLAYING) {
            send_to_char("`DAll the lights flicker, going out for a second.`x\n\r", d->character);
          }
        }
      }

      // check for limit of ghost object interactions per day - Discordance
      if (is_ghost(ch)) {
        if (is_manifesting(ch)) {
          if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
            send_to_char("You're unable to muster the effort to affect that object.\n\r", ch);
            return;
          }
        }
        else {
          send_to_char("You must be prepared to manifest this power.\n\r", ch);
          return;
        }
      }

      REMOVE_BIT(obj->value[1], CONT_CLOSED);
      act("You open $p.", ch, obj, NULL, TO_CHAR);
      act("$n opens $p.", ch, obj, NULL, TO_ROOM);
      return;
    }
    act("I see no $T here.", ch, NULL, arg, TO_CHAR);

    return;
  }

  _DOFUN(do_close) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *to;
    int door;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Close what?\n\r", ch);
      return;
    }

    if ((door = find_door(ch, arg)) >= 0) {
      /* 'close door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit = ch->in_room->exit[door];
      if (IS_SET(pexit->exit_info, EX_CLOSED)) {
        send_to_char("It's already closed.\n\r", ch);
        return;
      }

      if (pexit->wallcondition > WALLCOND_NORMAL || pexit->doorbroken > 0) {
        send_to_char("That door is too damaged.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch)) {
        send_to_char("You cannot do that from within the nightmare.\n\r", ch);
        return;
      }
      if (is_helpless(ch)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      if (in_fight(ch)) {
        if (has_moved(ch)) {
          send_to_char("You would have to consume your movement for a round to do this.\n\r", ch);
          return;
        }
        usemove(ch);
      }
      SET_BIT(pexit->exit_info, EX_CLOSED);

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if (ch != to) {
          printf_to_char(to, "%s closes the %s %s.\n\r", PERS(ch, to), dir_door_name[door][0], (pexit->keyword[0] == '\0') ? "door" : pexit->keyword);
        }
        else {
          printf_to_char(to, "You close the %s %s.\n\r", dir_door_name[door][0], (pexit->keyword[0] == '\0') ? "door" : pexit->keyword);
        }
      }

      /* close the other side */
      if ((to_room = pexit->u1.to_room) != NULL) {
        for (int dir = 0; dir < 10; dir++) {
          if ((pexit_rev = to_room->exit[dir]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            SET_BIT(pexit_rev->exit_info, EX_CLOSED);
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("The $T door closes.", *it, NULL, dir_door_name[rev_dir[door]][0], TO_CHAR);
          }
        }
      }
      return;
    }
    if ((obj = get_obj_here(ch, NULL, arg)) != NULL) {
      /* portal stuff */
      if (obj->item_type == ITEM_PORTAL) {

        if (!IS_SET(obj->value[1], EX_ISDOOR) || IS_SET(obj->value[1], EX_NOCLOSE)) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }

        if (IS_SET(obj->value[1], EX_CLOSED)) {
          send_to_char("It's already closed.\n\r", ch);
          return;
        }
        if (obj->pIndexData->vnum == 42000) {
          send_to_char("The lock won't reclose.\n\r", ch);
          return;
        }

        // check for limit of ghost object interactions per day - Discordance
        if (is_ghost(ch)) {
          if (is_manifesting(ch)) {
            if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
              send_to_char("You're unable to muster the effort to affect that object.\n\r", ch);
              return;
            }
          }
          else {
            send_to_char("You must be prepared to manifest this power.\n\r", ch);
            return;
          }
        }

        SET_BIT(obj->value[1], EX_CLOSED);
        act("You close $p.", ch, obj, NULL, TO_CHAR);
        act("$n closes $p.", ch, obj, NULL, TO_ROOM);
        return;
      }

      /* 'close object' */
      if (obj->item_type != ITEM_CONTAINER) {
        send_to_char("That's not a container.\n\r", ch);
        return;
      }
      if (IS_SET(obj->value[1], CONT_CLOSED)) {
        send_to_char("It's already closed.\n\r", ch);
        return;
      }
      if (!IS_SET(obj->value[1], CONT_CLOSEABLE)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      if (obj->pIndexData->vnum == 42000) {
        send_to_char("The lock won't reclose.\n\r", ch);
        return;
      }

      SET_BIT(obj->value[1], CONT_CLOSED);
      act("You close $p.", ch, obj, NULL, TO_CHAR);
      act("$n closes $p.", ch, obj, NULL, TO_ROOM);
      return;
    }

    act("I see no $T here.", ch, NULL, arg, TO_CHAR);

    return;
  }

  _DOFUN(do_stand) {
    OBJ_DATA *obj = NULL;

    if (ch->hit < 0) {
      send_to_char("You're hurt too badly for that.\n\r", ch);
      return;
    }
    if (in_fight(ch) || is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (argument[0] != '\0') {
      if (ch->position == POS_FIGHTING) {
        send_to_char("Maybe you should finish fighting first?\n\r", ch);
        return;
      }
      obj = get_obj_list(ch, argument, ch->in_room->contents);
      if (obj == NULL) {
        send_to_char("You don't see that here.\n\r", ch);
        return;
      }
      if (obj->item_type != ITEM_FURNITURE || (!IS_SET(obj->value[2], STAND_AT) && !IS_SET(obj->value[2], STAND_ON) && !IS_SET(obj->value[2], STAND_IN))) {
        send_to_char("You can't seem to find a place to stand.\n\r", ch);
        return;
      }
      if (ch->on != obj && count_users(obj) >= obj->value[0]) {
        act_new("There's no room to stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        return;
      }
      ch->on = obj;
      // DISABLED - Check to see if character is leaving a computer - Discordance
      /*
      if(access_internet(ch) == FALSE){
        if(ch->pcdata->chatroom > 0){
          do_function(ch, &do_chatroom, "logoff");
        }
      }
      */
    }

    switch (ch->position) {
    case POS_SLEEPING:
      //	if ( IS_AFFECTED(ch, AFF_SLEEP) || IS_AFFECTED(ch, AFF_SUBDUE))
      //	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

      if (obj == NULL) {
        send_to_char("You wake and stand up.\n\r", ch);
        act("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM);
        ch->on = NULL;
      }
      else if (IS_SET(obj->value[2], STAND_AT)) {
        act_new("You wake and stand at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], STAND_ON)) {
        act_new("You wake and stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act_new("You wake and stand in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_STANDING;
      do_function(ch, &do_look, "auto");
      break;

    case POS_RESTING:
    case POS_SITTING:

      if (obj == NULL) {
        send_to_char("You stand up.\n\r", ch);
        act("$n stands up.", ch, NULL, NULL, TO_ROOM);
        ch->on = NULL;
        // DISABLED - Check to see if character is leaving a computer -
        // Discordance
        /*
        if(access_internet(ch) == FALSE){
          if(ch->pcdata->chatroom > 0){
            do_function(ch, &do_chatroom, "logoff");
          }
        }
        */
      }
      else if (IS_SET(obj->value[2], STAND_AT)) {
        act("You stand at $p.", ch, obj, NULL, TO_CHAR);
        act("$n stands at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], STAND_ON)) {
        act("You stand on $p.", ch, obj, NULL, TO_CHAR);
        act("$n stands on $p.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act("You stand in $p.", ch, obj, NULL, TO_CHAR);
        act("$n stands on $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_STANDING;
      break;

    case POS_STANDING:
      send_to_char("You are already standing.\n\r", ch);
      break;

    case POS_FIGHTING:
      send_to_char("You are already fighting!\n\r", ch);
      break;
    }

    return;
  }

  _DOFUN(do_rest) {
    OBJ_DATA *obj = NULL;
    if (ch->hit < 0) {
      send_to_char("You're hurt too badly for that.\n\r", ch);
      return;
    }
    if (in_fight(ch) || is_helpless(ch) || is_air(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->position == POS_FIGHTING) {
      send_to_char("You are already fighting!\n\r", ch);
      return;
    }

    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0') {
      obj = get_obj_list(ch, argument, ch->in_room->contents);
      if (obj == NULL) {
        send_to_char("You don't see that here.\n\r", ch);
        return;
      }
    }
    else
    obj = ch->on;

    if (obj != NULL) {
      if (obj->item_type != ITEM_FURNITURE || (!IS_SET(obj->value[2], REST_ON) && !IS_SET(obj->value[2], REST_IN) && !IS_SET(obj->value[2], REST_AT))) {
        send_to_char("You can't rest on that.\n\r", ch);
        return;
      }

      if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0]) {
        act_new("There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        return;
      }

      ch->on = obj;

      // DISABLED - Check to see if character is leaving a computer - Discordance
      /*
      if(access_internet(ch) == FALSE){
        if(ch->pcdata->chatroom > 0){
          do_function(ch, &do_chatroom, "logoff");
        }
      }
      */
    }

    switch (ch->position) {
    case POS_SLEEPING:

      if (obj == NULL) {
        send_to_char("You wake up and start resting.\n\r", ch);
        act("$n wakes up and starts resting.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_AT)) {
        act_new("You wake up and rest at $p.", ch, obj, NULL, TO_CHAR, POS_SLEEPING);
        act("$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_ON)) {
        act_new("You wake up and rest on $p.", ch, obj, NULL, TO_CHAR, POS_SLEEPING);
        act("$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act_new("You wake up and rest in $p.", ch, obj, NULL, TO_CHAR, POS_SLEEPING);
        act("$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;

    case POS_RESTING:
      send_to_char("You are already resting.\n\r", ch);
      break;

    case POS_STANDING:
      if (obj == NULL) {
        send_to_char("You rest.\n\r", ch);
        act("$n sits down and rests.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_AT)) {
        act("You sit down at $p and rest.", ch, obj, NULL, TO_CHAR);
        act("$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_ON)) {
        act("You sit on $p and rest.", ch, obj, NULL, TO_CHAR);
        act("$n sits on $p and rests.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act("You rest in $p.", ch, obj, NULL, TO_CHAR);
        act("$n rests in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;

    case POS_SITTING:
      if (obj == NULL) {
        send_to_char("You rest.\n\r", ch);
        act("$n rests.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_AT)) {
        act("You rest at $p.", ch, obj, NULL, TO_CHAR);
        act("$n rests at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_ON)) {
        act("You rest on $p.", ch, obj, NULL, TO_CHAR);
        act("$n rests on $p.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act("You rest in $p.", ch, obj, NULL, TO_CHAR);
        act("$n rests in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;
    }

    return;
  }

  _DOFUN(do_sit) {
    OBJ_DATA *obj = NULL;
    if (ch->hit < 0) {
      send_to_char("You're hurt too badly for that.\n\r", ch);
      return;
    }

    if (in_fight(ch) || is_helpless(ch) || is_air(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0') {
      obj = get_obj_list(ch, argument, ch->in_room->contents);
      if (obj == NULL) {
        send_to_char("You don't see that here.\n\r", ch);
        return;
      }
    }
    else
    obj = ch->on;

    if (obj != NULL) {
      if (obj->item_type != ITEM_FURNITURE || (!IS_SET(obj->value[2], SIT_ON) && !IS_SET(obj->value[2], SIT_IN) && !IS_SET(obj->value[2], SIT_AT))) {
        send_to_char("You can't sit on that.\n\r", ch);
        return;
      }

      if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0]) {
        act_new("There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        return;
      }

      ch->on = obj;
      // DISABLED - Check to see if character is leaving a computer - Discordance
      /*
      if(access_internet(ch) == FALSE){
        if(ch->pcdata->chatroom > 0){
          do_function(ch, &do_chatroom, "logoff");
        }
      }
      */
    }

    switch (ch->position) {
    case POS_SLEEPING:

      if (obj == NULL) {
        send_to_char("You wake and sit up.\n\r", ch);
        act("$n wakes and sits up.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_AT)) {
        act_new("You wake and sit at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_ON)) {
        act_new("You wake and sit on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act_new("You wake and sit in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM);
      }

      ch->position = POS_SITTING;
      break;
    case POS_RESTING:
      if (obj == NULL)
      send_to_char("You stop resting.\n\r", ch);
      else if (IS_SET(obj->value[2], SIT_AT)) {
        act("You sit at $p.", ch, obj, NULL, TO_CHAR);
        act("$n sits at $p.", ch, obj, NULL, TO_ROOM);
      }

      else if (IS_SET(obj->value[2], SIT_ON)) {
        act("You sit on $p.", ch, obj, NULL, TO_CHAR);
        act("$n sits on $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
    case POS_SITTING:
      send_to_char("You are already sitting down.\n\r", ch);
      break;
    case POS_STANDING:
      if (obj == NULL) {
        send_to_char("You sit down.\n\r", ch);
        act("$n sits down on the ground.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_AT)) {
        act("You sit down at $p.", ch, obj, NULL, TO_CHAR);
        act("$n sits down at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_ON)) {
        act("You sit on $p.", ch, obj, NULL, TO_CHAR);
        act("$n sits on $p.", ch, obj, NULL, TO_ROOM);
      }
      else {
        act("You sit down in $p.", ch, obj, NULL, TO_CHAR);
        act("$n sits down in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
    }
    return;
  }

  _DOFUN(do_sleep) {
    OBJ_DATA *obj = NULL;
    if (ch->hit < 0) {
      send_to_char("You're hurt too badly for that.\n\r", ch);
      return;
    }
    if (in_fight(ch) || is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    switch (ch->position) {
    case POS_SLEEPING:
      send_to_char("You are already sleeping.\n\r", ch);
      break;

    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING:
      if (argument[0] == '\0' && ch->on == NULL) {
        send_to_char("You go to sleep.\n\r", ch);
        act("$n goes to sleep.", ch, NULL, NULL, TO_ROOM);
        ch->position = POS_SLEEPING;
      }
      else /* find an object and sleep on it */
      {
        if (argument[0] == '\0')
        obj = ch->on;
        else
        obj = get_obj_list(ch, argument, ch->in_room->contents);

        if (obj == NULL) {
          send_to_char("You don't see that here.\n\r", ch);
          return;
        }
        if (obj->item_type != ITEM_FURNITURE || (!IS_SET(obj->value[2], SLEEP_ON) && !IS_SET(obj->value[2], SLEEP_IN) && !IS_SET(obj->value[2], SLEEP_AT))) {
          send_to_char("You can't sleep on that!\n\r", ch);
          return;
        }

        if (ch->on != obj && count_users(obj) >= obj->value[0]) {
          act_new("There is no room on $p for you.", ch, obj, NULL, TO_CHAR, POS_DEAD);
          return;
        }

        ch->on = obj;

        // DISABLED - Check to see if character is leaving a computer -
        // Discordance
        /*
          if(access_internet(ch) == FALSE){
            if(ch->pcdata->chatroom > 0){
              do_function(ch, &do_chatroom, "logoff");
            }
          }
        */

        if (IS_SET(obj->value[2], SLEEP_AT)) {
          act("You go to sleep at $p.", ch, obj, NULL, TO_CHAR);
          act("$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM);
        }
        else if (IS_SET(obj->value[2], SLEEP_ON)) {
          act("You go to sleep on $p.", ch, obj, NULL, TO_CHAR);
          act("$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM);
        }
        else {
          act("You go to sleep in $p.", ch, obj, NULL, TO_CHAR);
          act("$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM);
        }
        ch->position = POS_SLEEPING;
      }
      break;

    case POS_FIGHTING:
      send_to_char("You are already fighting!\n\r", ch);
      break;
    }

    return;
  }

  _DOFUN(do_wake) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (dream_slave(ch)) {
      send_to_char("You can't seem to awaken.\n\r", ch);
      return;
    }
    if (is_dreaming(ch) && ch->pcdata->dream_timer < 5) {
      printf_to_char(
      ch, "You need to wait another %d minutes before you can do that.\n\r", 5 - ch->pcdata->dream_timer);
      return;
    }
    if (is_dreaming(ch) && physical_dreamer(ch)) {
      send_to_char("This world is your reality.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->comm, COMM_DREAMSNARED)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (ch->pcdata->patrol_status >= PATROL_WARMOVINGATTACK && ch->pcdata->patrol_status <= PATROL_WAGINGWAR) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (ch->pcdata->fall_timer > 0) {
      send_to_char("You feel like you're falling.\n\r", ch);
      return;
    }

    if (ch->race == RACE_FANTASY) {
      char buf[MSL];
      sprintf(buf, "%s fades from view.\n\r", dream_name(ch));
      dreamscape_message(ch, ch->pcdata->dream_room, buf);
      ch->pcdata->dream_room = 0;
      ch->pcdata->sleeping = 0;
      wake_char(ch);
      to_spectre(ch, FALSE);
      return;
    }

    if (is_dreaming(ch) && ch->pcdata->dream_room > 0) {
      char buf[MSL];
      sprintf(buf, "%s fades from view.\n\r", dream_name(ch));
      dreamscape_message(ch, ch->pcdata->dream_room, buf);
      ch->pcdata->sleeping = UMAX(5, ch->pcdata->sleeping);
      ch->pcdata->dream_room = 0;
      send_to_char("You start to wake up.\n\r", ch);
      return;
    }

    if (ch->pcdata->dream_controller == 1) {
      if (is_dreaming(ch)) {
        printf_to_char(ch->pcdata->dream_link, "%s disappears.\n\r", PERS(ch, ch->pcdata->dream_link));
        ch->pcdata->dream_link->pcdata->sleeping =
        UMIN(ch->pcdata->dream_link->pcdata->sleeping, 10);
        ch->pcdata->dream_link->pcdata->dream_link = NULL;
        ch->pcdata->dream_link = NULL;
      }
      ch->pcdata->sleeping = UMIN(ch->pcdata->sleeping, 2);
      send_to_char("You start to wake up.\n\r", ch);
      return;
    }

    if (ch->pcdata->coma > current_time) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      if (ch->pcdata->spectre == 2) {
        wake_char(ch);
        send_to_char("You wake up.\n\r", ch);
        act("$n stirs and awakens.", ch, NULL, NULL, TO_ROOM);
        return;
      }
      else if (ch->pcdata->spectre == 1) {
        if (!in_fight(ch)) {
          wake_char(ch);
          send_to_char("You wake up.\n\r", ch);
          act("$n stirs and awakens.", ch, NULL, NULL, TO_ROOM);
          return;
        }
        else
        send_to_char("You can't do that right now.\n\r", ch);
      }
      else
      do_function(ch, &do_stand, "");
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim))
    return;
    if (victim->race == RACE_FANTASY)
    return;

    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (in_fight(ch)) {
      send_to_char("You're in the middle of a fight!\n\r", ch);
      return;
    }

    // Not being able to wake people up is annoying - Discordance
    if (IS_IMMORTAL(ch) && victim->pcdata->sleeping > 0) {
      victim->pcdata->sleeping = 0;
      if (victim->pcdata->coma > 0) {
        victim->pcdata->coma = 0;
      }
    }

    // SRs waking people up is lame - Discordance
    if (is_gm(ch) && !IS_IMMORTAL(ch)) {
      if (is_dreaming(victim)) {
        send_to_char("You can't interfere.\n\r", ch);
        return;
      }
    }

    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    if (ch->pcdata->patrol_status >= PATROL_WARMOVINGATTACK && ch->pcdata->patrol_status <= PATROL_WAGINGWAR) {
      act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (is_dreaming(victim) && (physical_dreamer(victim) || dream_slave(victim))) {
      act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (victim->pcdata->sleeping <= 0 && victim->pcdata->spectre == 0) {
      act("$N is already awake.", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (victim->desc == NULL) {
      act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (victim->pcdata->coma > current_time) {
      act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (victim->pcdata->spectre == 1 && in_fight(victim)) {
      act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (IS_FLAG(victim->comm, COMM_DREAMSNARED)) {
      act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (is_dreaming(victim) && victim->pcdata->dream_link != NULL) {
      printf_to_char(victim->pcdata->dream_link, "%s disappears.\n\r", PERS(victim, victim->pcdata->dream_link));
      victim->pcdata->dream_link->pcdata->sleeping = 2;
    }

    if (victim->pcdata->spectre > 0) {
      wake_char(victim);
      send_to_char("You wake up.\n\r", victim);
      act("$n stirs and awakens.", victim, NULL, NULL, TO_ROOM);
      return;
    }
    if (is_dreaming(victim)) {
      char buf[MSL];
      sprintf(buf, "%s fades from view.\n\r", dream_name(victim));
      dreamscape_message(victim, victim->pcdata->dream_room, buf);
    }
    victim->pcdata->sleeping = 0;
    act("You wake $N.", ch, NULL, victim, TO_CHAR);
    act("$n wakes $N.", ch, NULL, victim, TO_NOTVICT);
    act("Someone wakes you.", ch, NULL, victim, TO_VICT);
    return;
  }

  /*
  * Contributed by Alander.
  */
  _DOFUN(do_visible) {
    if (is_ghost(ch)) {
      if (is_visible(ch)) {
        REMOVE_FLAG(ch->act, PLR_VISIBLE);
        send_to_char("You stop attempting to show yourself to the living.\n\r", ch);
      }
      else {
        SET_FLAG(ch->act, PLR_VISIBLE);
        send_to_char("You prepare to show yourself to the living.\n\r", ch);
      }
      return;
    }

    if (IS_FLAG(ch->act, PLR_HIDE)) {
      REMOVE_FLAG(ch->act, PLR_HIDE);
      act("$n comes out of hiding.", ch, NULL, NULL, TO_ROOM);
    }
    if (IS_FLAG(ch->act, PLR_INVIS))
    REMOVE_FLAG(ch->act, PLR_INVIS);
    send_to_char("Ok.\n\r", ch);
    return;
  }

  bool will_agg(CHAR_DATA *ch, CHAR_DATA *victim) {

    if (ch == NULL || ch->in_room == NULL || victim == NULL || victim->in_room == NULL)
    return FALSE;

    if (IS_IMMORTAL(victim))
    return FALSE;

    if (ch->hit < 0)
    return FALSE;

    //    if(IS_NPC(victim))
    //	return FALSE;

    if (IS_NPC(ch) && !str_cmp(ch->protecting, victim->name))
    return FALSE;

    if (IS_NPC(victim) && !str_cmp(victim->protecting, ch->name))
    return FALSE;

    return TRUE;
  }

  bool will_flee(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!IS_NPC(ch))
    return FALSE;

    return FALSE;
  }

  bool can_jump(CHAR_DATA *ch, int difficulty) {

    if (difficulty < 1)
    return TRUE;

    if (is_gm(ch))
    return TRUE;

    if (get_skill(ch, SKILL_SUPERJUMP) >= 1)
    return TRUE;

    if (is_flying(ch))
    return TRUE;

    if (ch->master != NULL && ch->master->lifting != NULL && ch->master->lifting == ch && can_jump(ch->master, difficulty))
    return TRUE;

    return FALSE;
  }
  bool can_see_move(CHAR_DATA *mover, CHAR_DATA *looker) {
    if (mover == NULL || looker == NULL || mover->in_room == NULL || looker->in_room == NULL)
    return FALSE;

    if (!can_see(looker, mover) && !is_ghost(mover))
    return FALSE;

    if (IS_FLAG(mover->act, PLR_SHROUD) && !can_shroud(looker))
    return FALSE;

    if (is_gm(mover) && !is_gm(looker))
    return FALSE;

    if (public_room(mover->in_room) && is_cloaked(mover))
    return FALSE;

    if (is_ghost(mover) && (get_skill(looker, SKILL_CLAIRVOYANCE) < 1 || is_possessing(mover)))
    return FALSE;

    if (IS_FLAG(mover->act, PLR_SHADOW) && get_skill(looker, SKILL_PERCEPTION) <= get_skill(mover, SKILL_STEALTH))
    return FALSE;

    if (is_animal(mover)) {
      if (is_natural(mover->in_room) && get_skill(looker, SKILL_PERCEPTION) <= get_skill(mover, SKILL_STEALTH))
      return FALSE;
      if (animal_size(get_animal_weight(mover, ANIMAL_ACTIVE)) <= ANIMAL_SMALL && get_skill(looker, SKILL_PERCEPTION) <= get_skill(mover, SKILL_STEALTH))
      return FALSE;
    }

    return TRUE;
  }
  char *movement_message(CHAR_DATA *ch, bool pushing, bool springing, int door) {
    ROOM_INDEX_DATA *room = ch->in_room;

    if (room->exit[door] == NULL || room->exit[door]->u1.to_room == NULL)
    return "walks";

    ROOM_INDEX_DATA *to_room = room->exit[door]->u1.to_room;

    if (IS_NPC(ch) && ch->pIndexData->vnum == 205000001)
    return "scurries";
    if (IS_NPC(ch) && ch->pIndexData->vnum == 205000003)
    return "floats";

    if (deep_water(ch) || is_underwater(room))
    return "swims";
    else if (is_water(room))
    return "wades";

    if (is_air(room) || is_air(to_room))
    return "flies";

    if (!IS_NPC(ch) && available_donated(ch) >= 1250 && safe_strlen(ch->pcdata->cwalk) > 1 && ch->shape == SHAPE_HUMAN)
    return ch->pcdata->cwalk;

    return "walks";
  }

  void move_into(CHAR_DATA *ch, int tox, int toy) {
    int fromx = ch->x;
    int fromy = ch->y;
    int proportion;
    int xmove, ymove;
    int dist = number_range(5, 15);

    int totaldist = (tox - fromx) * (tox - fromx) + (toy - fromy) * (toy - fromy);

    totaldist = (int)(sqrt((double)totaldist));

    if (dist >= totaldist) {
      ch->x = tox;
      ch->y = toy;
    }
    else {
      proportion = 100 * dist / totaldist;
      ymove = (int)((toy - fromy) * proportion / 100);
      xmove = (int)((tox - fromx) * proportion / 100);
      ch->x += xmove;
      ch->y += ymove;
    }
    if (ch->x < 0)
    ch->x = 0;
    if (ch->y < 0)
    ch->y = 0;
    if (ch->x > ch->in_room->size)
    ch->x = ch->in_room->size;
    if (ch->y > ch->in_room->size)
    ch->y = ch->in_room->size;
  }

  bool wall_walker(CHAR_DATA *ch) {
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (is_ghost(ch) && !carry_objects(ch))
    return TRUE;

    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      return TRUE;
    }
    return FALSE;
  }

  // Intentional wall climbing - Discordance
  _DOFUN(do_climb) 
  {
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int door;
    char buf[MSL], arg1[MSL];

    if (!ch || ch == NULL) {
      return;
    }
    
    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }
    
    if (is_prisoner(ch)) {
      send_to_char("The guards would certainly stop you.\n\r", ch);
      return;
    }
    
    if (!IS_NPC(ch) && ch->pcdata->patrol_timer > 0) {
      if (ch->pcdata->patrol_status >= PATROL_PREYING && ch->pcdata->patrol_status <= PATROL_GRABBED) {
        send_to_char("You can't yet find your way clear.\n\r", ch);
        return;
      }
    }
    
    if (!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_APPROACHINGHUNT && ch->pcdata->patrol_timer > 0) {
      send_to_char("It isn't yet safe to proceed.\n\r", ch);
      return;
    }
    
    if (!IS_NPC(ch) && ch->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && ch->pcdata->patrol_status <= PATROL_DEFENDHIDING) {
      send_to_char("You can't find your way through the chaotic nightmare.\n\r", ch);
      return;
    }
    
    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }
    
    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }
    
    if (in_fistfight(ch)) {
      send_to_char("Use retreat to pull out of a fist fight.\n\r", ch);
      return;
    }
    
    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if (!strcmp(arg1, "north") || !strcmp(arg1, "n")) {
      door = DIR_NORTH;
    }
    else if (!strcmp(arg1, "northeast") || !strcmp(arg1, "ne")) {
      door = DIR_NORTHEAST;
    }
    else if (!strcmp(arg1, "east") || !strcmp(arg1, "e")) {
      door = DIR_EAST;
    }
    else if (!strcmp(arg1, "southeast") || !strcmp(arg1, "se")) {
      door = DIR_SOUTHEAST;
    }
    else if (!strcmp(arg1, "south") || !strcmp(arg1, "s")) {
      door = DIR_SOUTH;
    }
    else if (!strcmp(arg1, "southwest") || !strcmp(arg1, "sw")) {
      door = DIR_SOUTHWEST;
    }
    else if (!strcmp(arg1, "west") || !strcmp(arg1, "w")) {
      door = DIR_WEST;
    }
    else if (!strcmp(arg1, "northwest") || !strcmp(arg1, "nw")) {
      door = DIR_NORTHWEST;
    }
    else {
      send_to_char("You pick a direction to climb in.\n\r", ch);
      return;
    }

    in_room = ch->in_room;

    if ((pexit = in_room->exit[door]) == NULL || (pexit->wall == WALL_NONE || pexit->wallcondition == WALLCOND_HOLE) || (to_room = pexit->u1.to_room) == NULL) {
      send_to_char("There's nothing to climb.\n\r", ch);
      return;
    }

    if (pexit->climb == 0) {
      send_to_char("You can't find your footing.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_SHADOW))
    REMOVE_FLAG(ch->act, PLR_SHADOW);
    if (IS_FLAG(ch->comm, COMM_BOARDING))
    REMOVE_FLAG(ch->comm, COMM_BOARDING);
    if (IS_FLAG(ch->comm, COMM_HOSTILE))
    REMOVE_FLAG(ch->comm, COMM_HOSTILE);
    if (IS_FLAG(ch->comm, COMM_WHEREVIS))
    REMOVE_FLAG(ch->comm, COMM_WHEREVIS);
    if (IS_FLAG(ch->comm, COMM_HOTSPOT))
    REMOVE_FLAG(ch->comm, COMM_HOTSPOT);
    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->comm, PLR_HIDE);

    ch->pcdata->travel_to = to_room->vnum;
    ch->pcdata->travel_type = TRAVEL_CLIMB;
    ch->pcdata->travel_time = pexit->climb;
    sprintf(buf, "You start to climb %s.\n\r", reldir_name[get_reldirection(door, ch->facing)][0]);
    send_to_char(buf, ch);
  }

  void move_char(CHAR_DATA *ch, int door, bool follow, bool isFlee) {
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    char buf[MAX_STRING_LENGTH];
    bool pushing = FALSE;
    bool springing = FALSE;
    int chance, threshold;

    if (door < 0 || door > 9) {
      bug("Do_move: bad door %d.", door);
      return;
    }

    if (!ch || ch == NULL)
    return;

    if (!IS_NPC(ch))
    ch->pcdata->time_since_action = 0;

    if (ch->position < 7 && !follow) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    if (higher_power(ch) && power_bound(ch)) {
      send_to_char("You cannot leave the binding circle.\n\r", ch);
      return;
    }
    if (!IS_NPC(ch)) {
      if (ch->pcdata->destiny_feature == DEST_FEAT_STUCK && pc_pop(ch->in_room) > 1 && follow == FALSE)
      return;

      if (IS_FLAG(ch->act, PLR_SHROUD) && door == ch->pcdata->dream_door && !follow && !in_fight(ch)) {
        if (ch->pcdata->spectre == 0 && in_world(ch) != WORLD_EARTH && in_world(ch) != WORLD_ELSEWHERE) {
          send_to_char("You cannot go that way.\n\r", ch);
          return;
        }
        if (crisis_notravel == 1 && ch->pcdata->spectre == 0) {
          send_to_char("You cannot go that way.\n\r", ch);
          return;
        }
        if (is_helpless(ch) && ch->pcdata->spectre == 0) {
          return;
        }
        if (ch->pcdata->spectre == 0 && clinic_patient(ch)) {
          send_to_char("You cannot go that way.\n\r", ch);
          return;
        }
        ROOM_INDEX_DATA *orig = ch->in_room;
        enter_dreamworld(ch, lobby_number(ch->pcdata->dream_exit));
        if (goddreamer(ch)) {
          sprintf(buf, "%s steps through a previously unnoticed door.\n\r", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
        }
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;

          if (d->character != NULL && d->connected == CON_PLAYING) {
            CHAR_DATA *fch = d->character;
            if (IS_NPC(fch))
            continue;

            if (fch == NULL)
            break;

            if (IS_NPC(fch) || !IS_FLAG(fch->act, PLR_SHROUD))
            continue;

            if (fch->in_room != orig)
            continue;
            if (fch->pcdata->spectre == 0 && clinic_patient(fch))
            continue;

            if (fch->master == ch) {
              act("You follow $N.", fch, NULL, ch, TO_CHAR);
              enter_dreamworld(fch, lobby_number(ch->pcdata->dream_exit));
              fch->pcdata->dream_room = ch->pcdata->dream_room;
            }
            do_function(fch, &do_look, "");
          }
        }
        do_function(ch, &do_look, "");
        return;
      }
    }
    if (move_helpless(ch) && !follow) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }
    if (is_prisoner(ch) && !follow) {
      send_to_char("The guards would certainly stop you.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->patrol_timer > 0) {
      if (ch->pcdata->patrol_status >= PATROL_PREYING && ch->pcdata->patrol_status <= PATROL_GRABBED) {
        send_to_char("You can't yet find your way clear.\n\r", ch);
        return;
      }
    }

    if (!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_APPROACHINGHUNT && ch->pcdata->patrol_timer > 0) {
      send_to_char("It isn't yet safe to proceed.\n\r", ch);
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && ch->pcdata->patrol_status <= PATROL_DEFENDHIDING) {
      send_to_char("You can't find your way through the chaotic nightmare.\n\r", ch);
      return;
    }

    if (IS_AFFECTED(ch, AFF_DISORIENTATION) && !follow) {
      door = rev_dir[door];
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (in_fistfight(ch)) {
      send_to_char("Use retreat to pull out of a fist fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }

    if (follow == FALSE && IS_FLAG(ch->act, PLR_SHADOW))
    REMOVE_FLAG(ch->act, PLR_SHADOW);
    if (IS_FLAG(ch->comm, COMM_BOARDING))
    REMOVE_FLAG(ch->comm, COMM_BOARDING);
    if (IS_FLAG(ch->comm, COMM_HOSTILE))
    REMOVE_FLAG(ch->comm, COMM_HOSTILE);
    if (IS_FLAG(ch->comm, COMM_WHEREVIS))
    REMOVE_FLAG(ch->comm, COMM_WHEREVIS);
    if (IS_FLAG(ch->comm, COMM_HOTSPOT))
    REMOVE_FLAG(ch->comm, COMM_HOTSPOT);

    in_room = ch->in_room;

    if ((pexit = in_room->exit[door]) == NULL || (to_room = pexit->u1.to_room) == NULL) {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }

    if ((pexit = in_room->exit[door]) == NULL || (pexit->wall != WALL_NONE && pexit->wallcondition != WALLCOND_HOLE) || (to_room = pexit->u1.to_room) == NULL || (!follow && !can_see_room(ch, pexit->u1.to_room))) {
      if (!IS_IMMORTAL(ch) && (!is_gm(ch) || prop_from_room(pexit->u1.to_room) != NULL) && !wall_walker(ch)) {
        send_to_char("You can't go that way.\n\r", ch);
        return;
      }
    }

    if (in_room->exit[door] != NULL && in_room->exit[door]->u1.to_room != NULL)
    prepass_gate(ch, in_room, in_room->exit[door]->u1.to_room, door);

    if ((pexit = in_room->exit[door]) == NULL || (pexit->wall != WALL_NONE && pexit->wallcondition != WALLCOND_HOLE) || (to_room = pexit->u1.to_room) == NULL || (!follow && !can_see_room(ch, pexit->u1.to_room))) {
      if (!IS_IMMORTAL(ch) && (!is_gm(ch) || prop_from_room(pexit->u1.to_room) != NULL) && !wall_walker(ch)) {
        send_to_char("You can't go that way.\n\r", ch);
        return;
      }
    }

    if (crisis_prologue == 1 && to_room != NULL && !IS_IMMORTAL(ch)) {
      if (!IS_NPC(ch) && in_lodge(ch->in_room) && !in_lodge(to_room)) {
        send_to_char("You are not sure how you know, but you are absolutely certain that if you go through that door you will die\n\r", ch);
        return;
      }
    }

    if (to_room != NULL && !IS_IMMORTAL(ch)) {
      if (!IS_NPC(ch) && ch->pcdata->spectre > 0 && IS_FLAG(ch->act, PLR_SHROUD)) {
        if (institute_room(ch->in_room) && !institute_room(to_room)) {
          send_to_char("You can't pass that way.\n\r", ch);
          return;
        }
      }

      if(to_room != NULL && !IS_IMMORTAL(ch) && security_room_blocked(ch, to_room)) {

        send_to_char("You are blocked by private security.\n\r", ch);
        return;
      }

      if (!IS_NPC(ch) && ch->in_room != NULL && to_room != NULL && ch->in_room->area->vnum != 12 && to_room->area->vnum != 12) {

        if (institute_room(to_room)) {
          if (guestmonster(ch)) {
            send_to_char("You can't go that way.`x\n\r", ch);
            return;
          }

          // no practice weapons indoors
          if (has_practice_arms(ch)) {
            send_to_char("The Institute does not permit practice weapons indoors.\n\r", ch);
            return;
          }

          if (to_room->vnum == ROOM_INDEX_SHERIFFCAGE && (ch->played < 100 || ch->race != RACE_DEPUTY)) {
            send_to_char("Only deputies with 100 hours of on the job experience are allowed access to the cage.\n\r", ch);
            return;
          }

          if (private_college_room(to_room) && !private_college_room(ch->in_room) && !college_student(ch, FALSE) && !college_staff(ch, FALSE) && !follow) {
            send_to_char("`RYou think better of intruding.`x\n\r", ch);
            return;
          }

          if (private_clinic_room(to_room) && !private_clinic_room(ch->in_room) && !clinic_patient(ch) && !clinic_staff(ch, FALSE) && !follow) {
            send_to_char("`RYou think better of intruding.`x\n\r", ch);
            return;
          }

          if (!institute_room(ch->in_room)) { // entering the institute
            send_to_char("`DYou enter the Institute.`x\n\r", ch);
          }
        }

        if (clinic_patient(ch) && !institute_room(ch->in_room) && !encounter_room(ch->in_room)) {
          act("$n is seized by a sheriff's deputy.", ch, NULL, NULL, TO_ROOM);
          char_from_room(ch);
          char_to_room(ch, get_room_index(ROOM_INDEX_CLINICCOMMIT));
          act("$n is dropped off by a sheriff's deputy.", ch, NULL, NULL, TO_ROOM);
          send_to_char("You are manhandled and roughed up by a sheriff's deputy on your way back to the institute.\n\r", ch);
        }

        // leaving the institute
        if (!institute_room(to_room) && institute_room(ch->in_room)) {
          if (clinic_patient(ch)) {
            send_to_char("You're not permitted to leave.\n\r", ch);
            return;
          }
        }
      }
    }

    if (is_gm(ch) && pc_pop(ch->in_room) < 1) {
      ch->pcdata->sr_nomove = 0;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED) && !is_gm(ch) && !wall_walker(ch) && (!is_animal(ch) || is_base(prop_from_room(pexit->u1.to_room)) || animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) != ANIMAL_TINY) && (!IS_FLAG(ch->act, PLR_SHROUD) || in_fight(ch))) {
      act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
      return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED) && !is_gm(ch) && !wall_walker(ch) && is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) == ANIMAL_TINY) {
      if (number_percent() % 3 == 0 || (prop_from_room(pexit->u1.to_room) != NULL && prop_from_room(pexit->u1.to_room)->warded >= 75)) {
        act("You fail to find a way in.", ch, NULL, pexit->keyword, TO_CHAR);
        WAIT_STATE(ch, PULSE_PER_SECOND * 12);
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = (12 * 60);
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_DOORSTUCK;
        affect_to_char(ch, &af);

        return;
      }
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED) && !is_gm(ch) && !wall_walker(ch) && is_animal(ch) && IS_AFFECTED(ch, AFF_DOORSTUCK) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) == ANIMAL_TINY) {
      act("There is no way in.", ch, NULL, pexit->keyword, TO_CHAR);
      WAIT_STATE(ch, PULSE_PER_SECOND * 12);
      return;
    }

    if (!IS_IMMORTAL(ch) && door == DIR_UP) {

      if (to_room->sector_type == SECT_AIR && get_skill(ch, SKILL_SUPERJUMP) < 2 && !is_flying(ch) && (!follow || ch->master == NULL || ch->master->lifting == NULL || ch->master->lifting != ch)) {
        send_to_char("You can't jump that high.\n\r", ch);
        return;
      }

      if (to_room->sector_type == SECT_ATMOSPHERE && !is_flying(ch) && (!follow || ch->master == NULL || ch->master->lifting == NULL || ch->master->lifting != ch)) {
        send_to_char("You can't jump that high.\n\r", ch);
        return;
      }
    }

    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_LOCALMOVE) && !IS_NPC(ch)) {
      if (ch->in_room->area->vnum != to_room->area->vnum) {
        send_to_char("You can't go there.\n\r", ch);
        return;
      }
    }
    if (is_gm(ch) && !IS_IMMORTAL(ch) && in_lodge(to_room)) {
      send_to_char("You can't go there.\n\r", ch);
      return;
    }
    if (is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) == ANIMAL_MONSTEROUS && IS_SET(to_room->room_flags, ROOM_INDOORS)) {
      send_to_char("You wouldn't fit.\n\r", ch);
      return;
    }
    if (guestmonster(ch) && ch->pcdata->height_feet >= 10 && IS_SET(to_room->room_flags, ROOM_INDOORS) && to_room->vnum > 300) {
      send_to_char("You wouldn't fit.\n\r", ch);
      return;
    }

    if (guestmonster(ch) && institute_room(to_room)) {
      send_to_char("Wards around the institute prevent you from approaching.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_SHROUD) && in_lodge(to_room)) {
      send_to_char("The nightmare is too dense to pass through in that direction.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD) && prop_from_room(to_room) != NULL && prop_from_room(to_room)->shroudshield >= 50 && IS_SET(to_room->room_flags, ROOM_INDOORS)) {
      send_to_char("The nightmare is too dense to pass through in that direction.\n\r", ch);
      return;
    }
    if(IS_FLAG(ch->act, PLR_SHROUD))
    {
      if (to_room->vnum == 3417 || to_room->vnum == 5102
          || to_room->vnum == 2007 || to_room->vnum == 5115
          || to_room->vnum == 5947 || to_room->vnum == 8735
          || to_room->vnum == 8999 || to_room->vnum == 6526
          || to_room->vnum == 9360 || to_room->vnum == 9367
          || to_room->vnum == 8776 || to_room->vnum == 8696
          || to_room->vnum == 9371 || to_room->vnum == 9380) {
        send_to_char("The nightmare is too dense to pass through in that direction.\n\r", ch);
        return;
      }
    }
    if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && no_deep_access(ch->in_room, to_room)) {
      send_to_char("The nightmare is too dense to pass through in that direction.\n\r", ch);
      return;
    }

    if (in_lodge(to_room) && largeweapons(ch)) {
      send_to_char("Your weapons tingle and you find yourself unable to enter.\n\r", ch);
      return;
    }

    // Don't want the ghosties wandering all over the place - Discordance
    if (is_ghost(ch)) {
      /*if(to_room->area->vnum != 13 && to_room->area->vnum != 14) {
        send_to_char( "You seem helplessly fixated on Haven and find yourself unable to leave.\n\r", ch );
        return;
      }*/
      if (IS_FLAG(ch->act, PLR_GHOSTBOUND) && to_room->sector_type != SECT_CEMETARY) {
        send_to_char("You can't move.\n\r", ch);
        return;
      }

      if (is_ghost(ch) && current_time < ch->pcdata->timebanished && to_room->sector_type != SECT_CEMETARY) {
        send_to_char("You can't move.\n\r", ch);
        return;
      }

      if (in_lodge(to_room)) {
        send_to_char("Some sort of mystical barrier prevents you from stepping foot inside.\n\r", ch);
        return;
      }
    }

    if (!is_gm(ch) && room_fight(to_room, TRUE, FALSE, FALSE) && IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("The nightmare is too chaotic right now for you go that way.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_SHROUD) && IS_SET(to_room->room_flags, ROOM_NOSHROUD)) {
      send_to_char("The nightmare is impassable in that direction.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->act, PLR_HIDE);

    if (prop_from_room(to_room) == NULL && !IS_NPC(ch) && ch->pcdata->hangouttemp != 0) {
      ch->pcdata->hangouttwo = ch->pcdata->hangoutone;
      ch->pcdata->hangoutone = ch->pcdata->hangouttemp;
      ch->pcdata->hangouttemp = 0;
    }
    DOMAIN_TYPE *fdomain;
    DOMAIN_TYPE *tdomain;
    if (!IS_NPC(ch)) {
      ch->pcdata->cansee = NULL;
      ch->pcdata->cansee2 = NULL;
      ch->pcdata->cansee3 = NULL;
      fdomain = domain_from_room(ch->in_room);
      tdomain = domain_from_room(to_room);
      if (fdomain != NULL && tdomain == NULL) {
        ch->pcdata->in_domain = 0;
        ch->pcdata->domain_timer = 0;
        printf_to_char(ch, "You leave the domain of %s\n\r", fdomain->domain_of);
      }
      else if (fdomain != tdomain && tdomain != NULL) {
        if (higher_power(ch)) {
          if (str_cmp(tdomain->domain_of, ch->name)) {
            printf_to_char(ch, "You cannot enter the domain of %s\n\r", strip_newlines(tdomain->domain_of));
            return;
          }
        }
        ch->pcdata->in_domain = tdomain->vnum;
        ch->pcdata->domain_timer = 0;
        printf_to_char(ch, " `CYou enter the domain of %s`x \n\r", strip_newlines(tdomain->domain_of));
      }

      if (higher_power(ch) && ch->pcdata->in_domain == 0)
      ch->pcdata->summon_bound = 0;
    }

    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    ch->facing = door;

    if (!IS_NPC(ch)) {
      if (IS_FLAG(ch->comm, COMM_FEEDING))
      REMOVE_FLAG(ch->comm, COMM_FEEDING);
      set_title(ch, "");

      free_string(ch->pcdata->place);
      ch->pcdata->place = str_dup("");
      ch->on = NULL;
      ch->pcdata->shroud_dragging = 0;
      ch->pcdata->shroud_drag_depth = 0;
      ch->pcdata->travel_prepped = FALSE;
      // DISABLED - Check to see if character is leaving a computer - Discordance
      /*
      if(access_internet(ch) == FALSE){
        if(ch->pcdata->chatroom > 0){
          do_function(ch, &do_chatroom, "logoff");
        }
      }
      */
    }

    if (!IS_NPC(ch) && !is_ghost(ch) && !IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_INVIS) && !IS_IMMORTAL(ch) && !is_gm(ch) && (!is_animal(ch) || animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) > ANIMAL_TINY)) {
      char cam[MSL];
      sprintf(cam, "%s leaves.", get_intro(ch));
      spymessage(ch->in_room, cam);
      sprintf(cam, "%s enters.", get_intro(ch));
      spymessage(to_room, cam);
    }
    // mist stuff here
    /*
    if(mist_manipulated_room(ch->in_room) && mist_manipulated_room(to_room)) {
      if((mist_score_group(ch) > 20 || number_percent() % 46 == 0) && mist_score_group(ch) - 15 > number_percent()) {
        ROOM_INDEX_DATA *lostroom;
        lostroom = get_newlostroom(ch->in_room);
        if(lostroom != NULL && lostroom->vnum > 100)
        split_group(ch, lostroom);
      }
      else if(pc_pop(ch->in_room) == 1) {
        CHAR_DATA *predator = find_mist_predator();
        if(predator != NULL) {
          ROOM_INDEX_DATA * predroom = get_pred_room(predator);
          if(predroom != NULL && predroom->vnum > 100) {
            if(pc_pop(predroom) == 0 && number_percent() < mist_prey_score(ch) && number_percent() % 3 == 0) {
              char_from_room(ch);
              char_to_room(ch, predroom);
              send_to_char("You get turned around in the mist.\n\r", ch);
              dact("$n stumbles into the area, straining to see through the
              mist.", ch, NULL, NULL, DISTANCE_NEAR); WAIT_STATE(ch, PULSE_PER_SECOND*12); return;
            }
          }     
        }

        ch->recent_moved = 10;

        if(!IS_NPC(ch)) {
          if(room_level(to_room) <= 0)
          ch->pcdata->institute_action = 0;
          if(!can_jump(ch, pexit->jump) && pexit->climb == 0 && pexit->fall == 0 && !is_flying(ch)) {
            ROOM_INDEX_DATA * predroom = get_pred_room(predator);
            if(predroom != NULL && predroom->vnum > 100) {
              if(pc_pop(predroom) == 0 && number_percent() < mist_prey_score(ch) && number_percent() % 3 == 0) {
                char_from_room(ch);
                char_to_room(ch, predroom);
                send_to_char("You get turned around in the mist.\n\r", ch);
                dact("$n stumbles into the area, straining to see through the
                mist.", ch, NULL, NULL, DISTANCE_NEAR); WAIT_STATE(ch, PULSE_PER_SECOND*12); return;
              }
            }
          }
        }
      }
    }
*/
    ch->recent_moved = 10;

    if (!IS_NPC(ch)) {
      if (room_level(to_room) <= 0)
      ch->pcdata->institute_action = 0;
      if (!can_jump(ch, pexit->jump) && pexit->climb == 0 && pexit->fall == 0 && !is_flying(ch)) {
        send_to_char("That's too far for you to jump.\n\r", ch);
        return;
      }

      if (!IS_IMMORTAL(ch)) {
        bool tmove = FALSE;
        if (pexit->climb > 0 && (ch->hit >= 0 || door == DIR_DOWN) && (pexit->jump == 0
        || !can_jump(ch, pexit->jump)) && !is_flying(ch)) {
          if (ch->hit < 0 && door == DIR_DOWN) {
            ch->pcdata->travel_to = to_room->vnum;
            ch->pcdata->travel_type = TRAVEL_FALL;
            ch->pcdata->travel_time = 1;
            ch->pcdata->move_damage = 2;
            sprintf(buf, "You fall %s.\n\r", reldir_name[get_reldirection(door, ch->facing)][0]);
            send_to_char(buf, ch);
            tmove = TRUE;
          }
          else {
            ch->pcdata->travel_to = to_room->vnum;
            ch->pcdata->travel_type = TRAVEL_CLIMB;
            ch->pcdata->travel_time = pexit->climb;
            sprintf(buf, "You start to climb %s.\n\r", reldir_name[get_reldirection(door, ch->facing)][0]);
            send_to_char(buf, ch);
            tmove = TRUE;
          }
        }
        if (pexit->fall > 0 && (pexit->jump == 0 || !can_jump(ch, pexit->jump)) && !is_flying(ch)) {
          ch->pcdata->travel_to = to_room->vnum;
          ch->pcdata->travel_type = TRAVEL_FALL;
          ch->pcdata->travel_time = pexit->fall;
          ch->pcdata->move_damage = (pexit->fall * 10) * (pexit->fall * 10);
          sprintf(buf, "You fall %s.\n\r", reldir_name[get_reldirection(door, ch->facing)][0]);
          send_to_char(buf, ch);
          tmove = TRUE;
        }
        if (can_jump(ch, pexit->jump) && pexit->jump != 0) {
          ch->pcdata->travel_to = to_room->vnum;
          ch->pcdata->travel_type = TRAVEL_JUMP;
          ch->pcdata->travel_time = 2;
          sprintf(buf, "$n jumps %s.\n\r", reldir_name[get_reldirection(door, ch->facing)][0]);
          act(buf, ch, NULL, ch, TO_NOTVICT);
          tmove = TRUE;
        }

        if (tmove == TRUE) {
          if (((pexit->climb > 0 && (ch->hit >= 0 || door == DIR_DOWN) && (pexit->jump == 0
          || !can_jump(ch, pexit->jump))) || (pexit->fall > 0 && (pexit->jump == 0 || !can_jump(ch, pexit->jump)))
          || (can_jump(ch, pexit->jump) && pexit->jump != 0)) && !follow) {
            for (CharList::iterator it = in_room->people->begin();
            it != in_room->people->end();) {
              CHAR_DATA *fch = *it;
              ++it;

              if (fch == NULL)
              break;

              if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
                act("You follow $N.", fch, NULL, ch, TO_CHAR);
                move_char(fch, door, TRUE, FALSE);
              }
              else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
                act("You follow $N.", fch, NULL, ch, TO_CHAR);
                move_char(fch, door, TRUE, FALSE);
              }
              else if (ch->dragging == fch && move_helpless(fch) && !in_fight(fch)
              && (!is_animal(ch) || animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >= ANIMAL_MANSIZED)
              && ((in_haven(to_room) || room_in_school(in_room->vnum)) || !in_haven(in_room) || !under_understanding(fch, ch))) {
                if ((ch->race != RACE_FACULTY) || (!room_in_school(in_room->vnum) || room_in_school(to_room->vnum))) {
                  act("$N drags you with $M", fch, NULL, ch, TO_CHAR);
                  move_char(fch, door, TRUE, FALSE);
                }
              }
              else if (ch->lifting == fch && move_helpless(fch) && !in_fight(fch)
              && (!is_animal(ch) || animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >= ANIMAL_MANSIZED)
              && ((in_haven(to_room) || room_in_school(in_room->vnum)) || !in_haven(in_room) || !under_understanding(fch, ch))) {
                if ((ch->race != RACE_FACULTY) || (!room_in_school(in_room->vnum) || room_in_school(to_room->vnum))) {
                  act("$N carries you with $M", fch, NULL, ch, TO_CHAR);
                  move_char(fch, door, TRUE, FALSE);
                }
              }
              else if (fch->possessing == ch) {
                act("You move with $N.", fch, NULL, ch, TO_CHAR);
                move_char(fch, door, TRUE, FALSE);
              }
            }
            return;
          }
        }
      }
    }

    for (CharList::iterator it = ch->in_room->people->begin(); it != ch->in_room->people->end(); ++it) {
      CHAR_DATA *rch = *it;
      if (rch == ch)
      continue;

      if (can_see_move(ch, rch)) {
        int jk = get_reldirection(door, rch->facing);
        if (jk == 2) {
          sprintf(buf, "%s %s past you.", PERS(ch, rch), movement_message(ch, pushing, springing, door));
          act(buf, rch, NULL, NULL, TO_CHAR);
        }
        else if (jk == 8) {
          sprintf(buf, "%s %s past you on your right.", PERS(ch, rch), movement_message(ch, pushing, springing, door));
          act(buf, rch, NULL, NULL, TO_CHAR);
        }
        else if (jk == 9) {
          sprintf(buf, "%s %s past you on your left.", PERS(ch, rch), movement_message(ch, pushing, springing, door));
          act(buf, rch, NULL, NULL, TO_CHAR);
        }
        else {
          sprintf(buf, "%s %s $T.", PERS(ch, rch), movement_message(ch, pushing, springing, door));
          act(buf, rch, NULL, relwalking[get_reldirection(door, rch->facing)], TO_CHAR);
        }
      }
      // perception checks for detecting ghosts leaving room without clairvoyance
      // - Discordance
      else if (get_skill(rch, SKILL_PERCEPTION) > 0 && is_ghost(ch) && !is_possessing(ch)) {
        chance = number_range(1, 100);
        // Setting up thresholds
        if (get_skill(rch, SKILL_CLAIRVOYANCE) > 0) {
          threshold = 0;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 1) {
          threshold = 80;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 2) {
          threshold = 60;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 3) {
          threshold = 40;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 4) {
          threshold = 20;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 5) {
          threshold = 10;
        }

        // Processing chance according to thresholds
        if (chance > threshold) {
          if (room_pop(ch->in_room) > 0) {
            send_to_char("You feel as if someone has departed.\n\r", rch);
          }
          else {
            send_to_char("You feel alone./n/r", rch);
          }
        }
      }
    }

    for (CharList::iterator it = to_room->people->begin(); it != to_room->people->end(); ++it) {
      CHAR_DATA *rch = *it;
      if (rch == ch)
      continue;

      if (can_see_move(ch, rch)) {
        sprintf(buf, "%s %s in from $T.", PERS(ch, rch), movement_message(ch, pushing, springing, door));
        act(buf, rch, NULL, relincoming[get_reldirection(rev_dir[door], rch->facing)], TO_CHAR);
      }
      // perception checks for detecting ghosts leaving room without clairvoyance
      // - Discordance
      else if (get_skill(rch, SKILL_PERCEPTION) > 0 && is_ghost(ch) && !is_possessing(ch)) {
        chance = number_range(1, 100);
        // Setting up thresholds
        if (get_skill(rch, SKILL_CLAIRVOYANCE) > 0) {
          threshold = 0;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 1) {
          threshold = 80;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 2) {
          threshold = 60;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 3) {
          threshold = 40;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 4) {
          threshold = 20;
        }
        else if (get_skill(rch, SKILL_PERCEPTION) == 5) {
          threshold = 10;
        }

        // Processing chance according to thresholds
        if (chance > threshold) {
          if (room_pop(ch->in_room) > 0) {
            send_to_char("You feel a faint chill.\n\r", rch);
          }
          else {
            send_to_char("You feel watched./n/r", rch);
          }
        }
      }
    }

    for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        CHAR_DATA *to = d->character;
        if (IS_NPC(to))
        continue;
        if (to == ch)
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;
        if (to->in_room == ch->in_room)
        continue;
        if (to->in_room == to_room)
        continue;
        if (can_see_char_distance(to, ch, DISTANCE_MEDIUM) && can_see_move(ch, to)) {
          if (can_see_room_distance(to, to_room, DISTANCE_MEDIUM)) {
            sprintf(buf, "[%s]%s %s $T.", room_distance(to, ch->in_room), PERS(ch, to), movement_message(ch, pushing, springing, door));
            act(buf, to, NULL, relwalking[get_reldirection(door, to->facing)], TO_CHAR);
          }
          else {
            sprintf(buf, "[%s]%s %s $T out of view.", room_distance(to, ch->in_room), PERS(ch, to), movement_message(ch, pushing, springing, door));
            act(buf, to, NULL, relwalking[get_reldirection(door, to->facing)], TO_CHAR);
          }
        }
        else if (can_see_room_distance(to, to_room, DISTANCE_MEDIUM) && can_see_move(ch, to)) {
          sprintf(buf, "%s %s $T into view.", PERS(ch, to), movement_message(ch, pushing, springing, door));
          act(buf, to, NULL, relwalking[get_reldirection(door, to->facing)], TO_CHAR);
        }
      }
    }

    if (!IS_NPC(ch) && !IS_IMMORTAL(ch)) {
      if ((to_room->sector_type == SECT_AIR || to_room->sector_type == SECT_ATMOSPHERE) && !is_flying(ch) && ch->pcdata->fall_timer == 0) {
        if (!follow || ch->master == NULL || ch->master->lifting == NULL || ch->master->lifting != ch || !is_flying(ch->master))
        ch->pcdata->fall_timer = 6;
      }
      else if ((is_underwater(to_room)) && !water_breathe(ch) && !is_underwater(ch->in_room) && ch->pcdata->fall_timer == 0) {
        ch->pcdata->fall_timer = 20;
      }
      else if (!is_underwater(to_room) && to_room->sector_type != SECT_AIR && to_room->sector_type != SECT_ATMOSPHERE)
      ch->pcdata->fall_timer = 0;
    }

    bool isdark = TRUE;
    if (!is_dark(to_room))
    isdark = FALSE;

    ROOM_INDEX_DATA *from_room = ch->in_room;

    if (!is_gm(ch) && !is_ghost(ch) && (!is_vampire(ch) || safe_strlen(ch->pcdata->scent) > 1))
    append_smell(ch, door, from_room);

    if (from_room->area->world != to_room->area->world && to_room->area->world == WORLD_EARTH && crisis_notravel == 1)
    to_room = from_room;
    if (from_room->area->world != to_room->area->world && from_room->area->world != WORLD_ELSEWHERE && to_room->area->world != WORLD_ELSEWHERE && (is_gm(ch) || is_ghost(ch) || event_cleanse == 1))
    to_room = from_room;
    if (from_room->area->world != to_room->area->world && world_blocked(ch, to_room->area->world))
    to_room = from_room;

    if (from_room->area->world != to_room->area->world && from_room->area->world != WORLD_EARTH && !follow && from_room->area->world != WORLD_ELSEWHERE && (!has_nonconsume(ch, ITEM_COMPASS) || IS_FLAG(ch->act, PLR_STASIS) || (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID))) {
      to_room = from_room;
    }
    if (guestmonster(ch) && from_room->vnum == 56) {
      if (!IS_FLAG(ch->act, PLR_SHROUD)) {
        SET_FLAG(ch->act, PLR_SHROUD);
      }
      if (ch->race == RACE_LANDMONSTER) {
        if (number_percent() % 3 == 0)
        to_room =
        room_by_coordinates(number_range(4, 68), number_range(80, 98), 0);
        else if (number_percent() % 2 == 0)
        to_room =
        room_by_coordinates(number_range(5, 65), number_range(-27, -5), 0);
        else
        to_room =
        room_by_coordinates(number_range(-25, -5), number_range(4, 95), 0);
        scout_report("Your scouts report a monster being spotted out in the forest.");
      }
      else {
        to_room =
        room_by_coordinates(number_range(75, 90), number_range(-50, 120), 0);
        scout_report("Your scouts report a sea monster being spotted off the coast of Haven.");
      }
    }
    if (!follow && asylum_room(ch->in_room) && clinic_patient(ch))
    patient_move(ch);

    bool crowded = crowded_room(ch->in_room);

    if (!IS_NPC(ch) && (goblin_market(ch->in_room) || goblin_market(to_room)))
    ch->pcdata->market_visit = current_time;

    char_from_room(ch);
    char_to_room(ch, to_room);

    if (is_dark(to_room) && isdark == FALSE)
    act("`DThe lights flicker and die.`x", ch, NULL, NULL, TO_ROOM);

    ch->x = get_exit_x(rev_dir[door], to_room);
    ch->y = get_exit_y(rev_dir[door], to_room);

    move_into(ch, get_exit_x(door, to_room), get_exit_y(door, to_room));

    /*
    if(to_room->area->world != WORLD_EARTH && to_room->area->world != WORLD_ELSEWHERE && !guestmonster(ch))
    {
      if(IS_FLAG(ch->act, PLR_SHROUD))
      REMOVE_FLAG(ch->act, PLR_SHROUD);
      if(IS_FLAG(ch->act, PLR_DEEPSHROUD))
      REMOVE_FLAG(ch->act, PLR_DEEPSHROUD);
    }
    */
    if (is_sparring_room(to_room)) {
      ch->x = number_range(1, to_room->size);
      ch->y = number_range(1, to_room->size);
    }

    if (public_room(to_room) && !IS_NPC(ch) && !is_ghost(ch) && in_haven(to_room))
    last_public(ch, to_room->vnum);
    else if (public_room(ch->in_room) && !IS_NPC(ch) && !is_ghost(ch) && in_haven(ch->in_room))
    last_public(ch, ch->in_room->vnum);

    if (!follow)
    patrol_check(ch, ch->in_room);

    if (!IS_NPC(ch)) {
      if (to_room->vnum == ch->pcdata->job_room_one)
      ch->pcdata->job_delay_one = current_time;
      if (to_room->vnum == ch->pcdata->job_room_two)
      ch->pcdata->job_delay_two = current_time;

      ch->pcdata->police_catch_timer = 0;
      free_string(ch->pcdata->pinned_by);
      ch->pcdata->pinned_by = str_dup("");

      if (get_gm(ch->in_room, FALSE) != NULL && !battleground(ch->in_room)) {
        if (!IS_FLAG(get_gm(ch->in_room, FALSE)->comm, COMM_RUNNING) && get_gm(ch->in_room, FALSE)->pcdata->vote_timer >= 0)
        get_gm(ch->in_room, FALSE)->pcdata->vote_timer = -10;
      }

      if (IS_AFFECTED(ch, AFF_MAD) && number_percent() % 3 == 0) {
        bool followed = FALSE;
        for (CharList::iterator it = from_room->people->begin();
        it != from_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;

          if (fch == NULL)
          break;

          if (!IS_NPC(fch) && !is_gm(fch) && number_percent() % 3 == 0 && fch->master != ch) {
            printf_to_char(ch, "%s walks in from behind you.\n\r", PERS(fch, ch));
            followed = TRUE;
          }
        }
        if (followed == FALSE && number_percent() % 37 == 0)
        printf_to_char(ch, "Something unseen walks in from behind you.\n\r");
      }
    }

    if (is_gm(ch) && !IS_NPC(ch) && !battleground(ch->in_room) && !IS_FLAG(ch->comm, COMM_RUNNING) && room_pop_mortals(to_room) > 0) {
      ch->pcdata->vote_timer = -30;
    }
    else if (!IS_NPC(ch))
    ch->pcdata->vote_timer = 0;

    if (in_room == to_room) /* no circular follows */
    return;

    int pop = 0;
    for (CharList::iterator it = in_room->people->begin();
    it != in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      continue;
      pop++;
    }

    for (int i = 0; i <= pop; i++) {
      for (CharList::iterator it = in_room->people->begin();
      it != in_room->people->end();) {
        CHAR_DATA *fch = *it;
        ++it;

        if (fch == NULL)
        continue;

        if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {

          act("You follow $N.", fch, NULL, ch, TO_CHAR);
          move_char(fch, door, TRUE, FALSE);
          break;
        }

        else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
          act("You follow $N.", fch, NULL, ch, TO_CHAR);
          move_char(fch, door, TRUE, FALSE);
          break;
        }
        else if (ch->dragging == fch && move_helpless(fch) && !in_fight(fch) && ((in_haven(to_room)
                || !in_haven(from_room) || !under_understanding(fch, ch)))) {
          if ((ch->race != RACE_FACULTY) || (!room_in_school(in_room->vnum) || room_in_school(to_room->vnum))) {
            act("$N drags you with $M", fch, NULL, ch, TO_CHAR);
            move_char(fch, door, TRUE, FALSE);
          }
          break;
        }
        else if (ch->lifting == fch && move_helpless(fch) && !in_fight(fch) && ((in_haven(to_room)
                || !in_haven(from_room) || !under_understanding(fch, ch)))) {
          if ((ch->race != RACE_FACULTY) || (!room_in_school(in_room->vnum) || room_in_school(to_room->vnum))) {
            act("$N carries you with $M", fch, NULL, ch, TO_CHAR);
            move_char(fch, door, TRUE, FALSE);
          }
          break;
        }
        else if (fch->possessing == ch) {
          act("You move with $N.", fch, NULL, ch, TO_CHAR);
          move_char(fch, door, TRUE, FALSE);
          break;
        }
      }
    }
    if (ch->dragging != NULL && !IS_NPC(ch->dragging) && is_ghost(ch->dragging) && ch->dragging->pcdata->ghost_room == in_room->vnum && (in_haven(to_room) || !in_haven(from_room) || !under_understanding(ch->dragging, ch))) {
      ch->dragging->pcdata->ghost_room = to_room->vnum;
      printf_to_char(ch, "You drag the comatose form of %s with you.\n\r", ch->dragging->name);
    }
    else if (ch->dragging != NULL && !IS_NPC(ch->dragging) && ch->dragging->pcdata->spectre > 0 && IS_FLAG(ch->dragging->act, PLR_SHROUD) && ch->dragging->pcdata->ghost_room == in_room->vnum && (in_haven(to_room) || !in_haven(from_room) || !under_understanding(ch->dragging, ch))) {
      ch->dragging->pcdata->ghost_room = to_room->vnum;
      printf_to_char(ch, "You drag the comatose form of %s with you.\n\r", ch->dragging->name);
    }

    if (IS_FLAG(ch->comm, COMM_PRIVATE)) {
      REMOVE_FLAG(ch->comm, COMM_PRIVATE);
    }

    if (IS_FLAG(ch->act, PLR_AFTERGLOW)) {
      REMOVE_FLAG(ch->act, PLR_AFTERGLOW);
    }

    if (!IS_NPC(ch))
    ch->pcdata->ambush = 0;

    if (!IS_NPC(ch)) {
      ch->pcdata->dream_door = -1;
      ch->pcdata->dream_exit = -1;
    }

    if (room_level(to_room) > 0 && number_percent() % 5 == 0 && !IS_NPC(ch) && !follow && !higher_power(ch) && !in_fight(ch) && ch->pcdata->spawned_monsters <= 0 && !IS_FLAG(ch->act, PLR_DEEPSHROUD) && ch->pcdata->patrol_status != PATROL_HUNTING) {
      populate_warren(ch);
    }
    ch->endurance = 0;
    ch->fistfighting = NULL;
    ch->fistattack = 0;
    ch->fisttimer = 0;
    do_function(ch, &do_look, "auto");
    if (!IS_NPC(ch) && in_room->area->world != to_room->area->world && in_room->area->world != WORLD_ELSEWHERE && to_room->area->world != WORLD_ELSEWHERE) {
      send_to_char("Your surroundings seem to dramatically change as you walk forward.\n\r", ch);
      ch->pcdata->lingering_sanc = 240;
    }

    if (!IS_NPC(ch) && ch->pcdata->process_timer > 0) {
      ch->pcdata->process_timer = 0;
      send_to_char("You stop what you were doing.\n\r", ch);
    }

    if (crowded == FALSE && crowded_room(ch->in_room))
    act("The area starts to seem crowded.", ch, NULL, NULL, TO_ROOM);

    if (ch->dragging != NULL && ch->dragging->in_room != ch->in_room && !is_ghost(ch->dragging))
    ch->dragging = NULL;
    if (ch->lifting != NULL && ch->lifting->in_room != ch->in_room)
    ch->lifting = NULL;

    if (!IS_NPC(ch) && ch->pcdata->dirt[0] < 1000 && (ch->in_room->sector_type == SECT_PARK || ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_TUNNELS || ch->in_room->sector_type == SECT_CAVE || ch->in_room->sector_type == SECT_BASEMENT || ch->in_room->sector_type == SECT_ROCKY)) {
      ch->pcdata->dirt[0]++;
    }
    if (!follow)
    postpass_gate(ch, from_room, to_room, door);

    if (!follow && !IS_NPC(ch) && IS_FLAG(ch->act, PLR_SHROUD)) {
      if (ch->in_room != ch->pcdata->last_patrol && number_percent() % 3 == 0) {
        if ((ch->pcdata->spectre != 0 && get_room_index(ch->pcdata->ghost_room) != NULL && get_dist(ch->in_room->x, ch->in_room->y, get_room_index(ch->pcdata->ghost_room)->x, get_room_index(ch->pcdata->ghost_room)->y) >= 5))
        dream_lobby_move(ch);
        else if (ch->pcdata->spectre == 0 && number_percent() % 3 == 0)
        dream_lobby_move(ch);
        else if (ch->pcdata->spectre != 0 && number_percent() % 5 == 0)
        dream_lobby_move(ch);
      }
      ch->pcdata->last_patrol = ch->in_room;
    }
    return;
  }

  int mist_prey_score(CHAR_DATA *ch) {
    if (is_abom(ch))
    return 0;

    if (get_skill(ch, SKILL_MISTVISION) > 0)
    return 0;

    if (ch->played / 3600 < 10 || solidity(ch) < 50)
    return 0;

    if (ch->pcdata->class_type > 2)
    return 10;

    int val = mist_lost_score(ch) * 5;

    if (!is_super(ch))
    val += 15;

    if (ch->pcdata->class_type <= 1)
    val += 25;

    val = val * solidity(ch) / 100;

    return val;
  }

  int mist_lost_score(CHAR_DATA *ch) {
    int val = 10;

    if (get_skill(ch, SKILL_MISTVISION) > 0)
    return 0;

    val -= get_skill(ch, SKILL_PERCEPTION);
    val -= get_skill(ch, SKILL_ACUTESIGHT) * 2;
    val -= get_skill(ch, SKILL_ACUTESMELL) * 2;
    val -= get_skill(ch, SKILL_ACUTEHEAR) * 2;

    val = UMAX(3, val);

    return val;
  }

  int mist_score_group(CHAR_DATA *ch) {
    int score = 0;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL)
      break;
      if (IS_NPC(fch))
      break;
      if (is_ghost(fch))
      break;

      if (fch->master == ch || ch->master == fch || fch->master == ch->master || ch == fch) {
        score += mist_lost_score(ch);
      }
    }
    return score;
  }

  void split_group(CHAR_DATA *ch, ROOM_INDEX_DATA *newroom) {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      CHAR_DATA *fch = CH(d);

      if (fch == NULL)
      continue;

      if (IS_NPC(fch))
      continue;

      if (is_gm(fch))
      continue;

      if (fch->in_room == NULL)
      continue;

      if (fch->in_room != ch->in_room)
      continue;

      if (is_ghost(fch))
      break;
      if (ch == fch)
      break;

      if (fch->master == ch || ch->master == fch || fch->master == ch->master || ch == fch) {
        if (number_percent() % 2 == 0) {
          char_from_room(fch);
          char_to_room(fch, newroom);
          send_to_char("You get turned around in the mist.\n\r", fch);
          dact("$n walks into the area, straining to see through the mist.", fch, NULL, NULL, DISTANCE_NEAR);
          act("$n walks into the area, straining to see through the mist.", fch, NULL, NULL, TO_ROOM);
          WAIT_STATE(fch, PULSE_PER_SECOND * 8);
        }
      }
    }
  }

  CHAR_DATA *find_mist_predator(void) {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      CHAR_DATA *fch = CH(d);

      if (fch == NULL)
      continue;

      if (IS_NPC(fch))
      continue;

      if (is_gm(fch))
      continue;

      if (fch->in_room == NULL)
      continue;

      if (!mist_manipulated_room(fch->in_room))
      continue;

      if (number_percent() % 3 == 0)
      continue;

      if (fch->in_room->vnum >= INIT_FIGHTROOM && fch->in_room->vnum <= END_FIGHTROOM)
      continue;

      return fch;
    }
    return NULL;
  }

  ROOM_INDEX_DATA *get_pred_room(CHAR_DATA *ch) {
    int direction = get_absoldirection(DIR_NORTH, ch->facing);
    ROOM_INDEX_DATA *first_room;
    ROOM_INDEX_DATA *second_room;
    EXIT_DATA *pexit;

    pexit = ch->in_room->exit[direction];
    if (pexit != NULL && (pexit->wall == WALL_NONE || pexit->wallcondition == WALLCOND_HOLE) && (first_room = pexit->u1.to_room) != NULL && !IS_SET(pexit->exit_info, EX_CLOSED)) {
      pexit = first_room->exit[direction];
      if (pexit != NULL && (pexit->wall == WALL_NONE || pexit->wallcondition == WALLCOND_HOLE) && (second_room = pexit->u1.to_room) != NULL && !IS_SET(pexit->exit_info, EX_CLOSED)) {
        if (mist_manipulated_room(second_room))
        return second_room;
      }
      if (mist_manipulated_room(first_room))
      return first_room;
    }
    return NULL;
  }

  ROOM_INDEX_DATA *get_newlostroom(ROOM_INDEX_DATA *startroom) {
    ROOM_INDEX_DATA *newroom;
    int vnum = startroom->vnum;

    int xmod, ymod;

    for (int i = 0; i < 50; i++) {
      vnum = startroom->vnum;
      xmod = number_range(-25, 25);
      ymod = number_range(-22, 22);

      vnum = vnum + xmod + ymod * 1000;

      newroom = get_room_index(vnum);

      if (newroom != NULL && !is_privatep(newroom) && !IS_SET(newroom->room_flags, ROOM_INDOORS))
      return newroom;
    }
    return NULL;
  }

  bool is_fast(CHAR_DATA *ch) {

    if (is_gm(ch))
    return TRUE;

    if (IS_NPC(ch))
    return TRUE;

    if (is_animal(ch) && animal_stat(ch, ANIMAL_ACTIVE, ANIMAL_SPEED) >= 2)
    return TRUE;

    if (ch->shape == SHAPE_WOLF)
    return TRUE;

    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_FLEETFOOT) > 0)
    return TRUE;

    if (ch->in_room != NULL && in_water(ch)) {
      if (is_swimmer(ch))
      return TRUE;
    }
    if (ch->in_room != NULL && (ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_ATMOSPHERE)) {
      if (is_animal(ch) && animal_stat(ch, ANIMAL_ACTIVE, ANIMAL_FLIGHT) > 0)
      return TRUE;
    }
    return FALSE;
  }

  int move_speed_person(CHAR_DATA *ch) {

    if (ch->in_room->sector_type == SECT_AIR)
    return 10;

    int count = large_weapon_count(ch);
    if (get_skill(ch, SKILL_FLEETFOOT) > 0)
    count--;

    int bonus = get_speed(ch);
    bonus = bonus * (10 - count) / 10;
    if (!wearing_armor(ch) && !has_shield(ch))
    bonus = bonus * 12 / 10;

    int speed = 30 - bonus;
    speed = speed * 100 / get_lifeforce(ch, TRUE, NULL);

    if (IS_AFFECTED(ch, AFF_ARTHRITIS))
    speed *= 4;
    if (IS_FLAG(ch->act, PLR_BOUND))
    speed *= 2;

    speed = UMAX(speed, 10);

    if (shipment_carrier(ch))
    speed *= 2;

    if (!in_fight(ch)) {
      if (in_world(ch) == WORLD_WILDS)
      speed *= 2;
      else if (in_world(ch) != WORLD_EARTH)
      speed = speed * 3 / 2;
    }

    if (guestmonster(ch)) {
      if (ch->pcdata->height_feet >= 10)
      return 100;
      else
      return 20;
    }

    return speed;
  }

  int move_speed(CHAR_DATA *ch) {
    int count = 1;
    int max_speed = move_speed_person(ch);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (IS_NPC(fch))
      break;
    
      if (fch == ch)
      break;

      if (is_ghost(fch))
      break;

      if (fch->master == ch || ch->master == fch || fch->master == ch->master) {
        if (move_speed_person(fch) > max_speed)
        max_speed = move_speed_person(fch);
        count++;
      }
    }

    if (count > 1)
    max_speed = max_speed * (9 + count) / 10;

    int time = max_speed * PULSE_SECOND / 10;
    return time;
  }

  _DOFUN(do_north) {

    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "north");
      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_NORTH;
      ch->facing = DIR_NORTH;
      return;
    }
    */
    move_char(ch, DIR_NORTH, FALSE, FALSE);

    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_northeast) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "northeast");
      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_NORTHEAST;
      ch->facing = DIR_NORTHEAST;
      return;
    }
    */
    move_char(ch, DIR_NORTHEAST, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_northwest) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "north");
      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_NORTHWEST;
      ch->facing = DIR_NORTHWEST;
      return;
    }
    */
    move_char(ch, DIR_NORTHWEST, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_east) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "east");

      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_EAST;
      ch->facing = DIR_EAST;
      return;
    }
    */
    move_char(ch, DIR_EAST, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_south) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "south");
      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_SOUTH;
      ch->facing = DIR_SOUTH;
      return;
    }
    */
    move_char(ch, DIR_SOUTH, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_southeast) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "southeast");
      return;
    }

    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_SOUTHEAST;
      ch->facing = DIR_SOUTHEAST;
      return;
    }
    */
    move_char(ch, DIR_SOUTHEAST, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_southwest) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "southwest");
      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_SOUTHWEST;
      ch->facing = DIR_SOUTHWEST;
      return;
    }
    */
    move_char(ch, DIR_SOUTHWEST, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_west) {
    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "west");
      return;
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions on first.\n\r", ch);
      return;
    }

    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_WEST;
      ch->facing = DIR_WEST;
      return;
    }
    */
    move_char(ch, DIR_WEST, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_up) {

    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "up");
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_UP;
      ch->facing = DIR_UP;
      return;
    }
    */
    move_char(ch, DIR_UP, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_down) {

    if (!ch || ch == NULL)
    return;

    if (in_fight(ch)) {
      do_function(ch, &do_move, "down");
      return;
    }
    /*
    if(!IS_NPC(ch) && ch->pcdata->walking > -1){
      ch->pcdata->walking = DIR_DOWN;
      ch->facing = DIR_DOWN;
      return;
    }
    */
    move_char(ch, DIR_DOWN, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  _DOFUN(do_right) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_EAST, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_forward) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_NORTH, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));
    return;
  }

  _DOFUN(do_left) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_WEST, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_backward) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_SOUTH, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))

    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_forwardright) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_NORTHEAST, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_backwardright) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_SOUTHEAST, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_forwardleft) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_NORTHWEST, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    if (!is_gm(ch))
    WAIT_STATE(ch, move_speed(ch));

    return;
  }
  _DOFUN(do_backwardleft) {
    if (!ch || ch == NULL)
    return;

    if (!IS_FLAG(ch->comm, COMM_CARDINAL) && ch->played / 3600 > 20) {
      send_to_char("Turn cardinal directions off first.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      return;
    }
    int direction = get_absoldirection(DIR_SOUTHWEST, ch->facing);
    move_char(ch, direction, FALSE, FALSE);
    WAIT_STATE(ch, move_speed(ch));

    return;
  }

  int room_visibility_range(CHAR_DATA *ch, ROOM_INDEX_DATA *start, ROOM_INDEX_DATA *finish) {
    if (is_dark(finish) && !can_see_dark(ch))
    return 0;

    int range = 6;

    if (!battleground(ch->in_room)) {
      if (sunphase(start) == 0) {
        if (get_skill(ch, SKILL_NIGHTVISION) <= 0 && !IS_SET(finish->room_flags, ROOM_INDOORS))
        range = UMIN(2, range / 2);
      }
      if (sunphase(start) == 1 || sunphase(start) == 7) {
        if (get_skill(ch, SKILL_NIGHTVISION) <= 0 && !IS_SET(finish->room_flags, ROOM_INDOORS))
        range = UMIN(3, range / 2);
      }
      if (sunphase(start) == 4) {
        if (is_vampire(ch) && !IS_SET(finish->room_flags, ROOM_INDOORS))
        range = 0;
        else if (get_skill(ch, SKILL_NIGHTVISION) > 0 && !IS_SET(finish->room_flags, ROOM_INDOORS) && !is_covered(ch, COVERS_EYES))
        range = UMIN(1, range / 2);

        if (is_covered(ch, COVERS_EYES))
        range = UMIN(range + 1, 6);
        if (mist_level(ch->in_room) >= 3)
        range = UMIN(range + 1, 6);
      }
      if (sunphase(start) == 3 || sunphase(start) == 5) {
        if (is_vampire(ch) && !IS_SET(finish->room_flags, ROOM_INDOORS))
        range = UMIN(2, range / 2);
        else if (get_skill(ch, SKILL_NIGHTVISION) > 0 && !IS_SET(finish->room_flags, ROOM_INDOORS) && !is_covered(ch, COVERS_EYES))
        range = UMIN(3, range / 2);

        if (is_covered(ch, COVERS_EYES))
        range = UMIN(range + 1, 6);
        if (mist_level(ch->in_room) >= 3)
        range = UMIN(range + 1, 6);
      }
    }
    if (mist_level(finish) >= 3)
    range = UMIN(2, range / 2);

    return range;
  }

  bool can_see_char_distance(CHAR_DATA *ch, CHAR_DATA *victim, int range) {
    if (ch->in_room == NULL || victim->in_room == NULL)
    return FALSE;

    if(!IS_NPC(victim) && victim->pcdata->cam_spy_char == ch)
    return TRUE;

    if (in_world(ch) != in_world(victim))
    return FALSE;
    if (battleground(ch->in_room) && !battleground(victim->in_room))
    return FALSE;

    if (!IS_NPC(ch) && !IS_NPC(victim) && !same_player(ch, victim) && victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return FALSE;

    if (!IS_NPC(ch) && !IS_NPC(victim) && !same_player(ch, victim) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return FALSE;

    if (is_gm(ch) && battleground(ch->in_room) && battleground(victim->in_room))
    return TRUE;

    if(!in_fight(ch) && !in_fight(victim) && !battleground(ch->in_room) && !forest_monster(victim) && ch->in_room != victim->in_room && light_level(victim->in_room) + skillpoint(get_skill(ch, SKILL_NIGHTVISION)) * 5 <
        50)
    return FALSE;

    if (!IS_NPC(ch) && !IS_NPC(victim)) {
      if (ch->pcdata->patrol_timer > 0 && (ch->pcdata->patrol_status == PATROL_LEADING_ASSAULT || ch->pcdata->patrol_status == PATROL_UNDER_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_UNDER_ASSAULT)) {
        if (ch->pcdata->patrol_target != NULL && ch->pcdata->patrol_target->in_room != NULL && ch->pcdata->patrol_target->in_room == victim->in_room)
        return TRUE;
      }
      if (!str_cmp(ch->name, victim->pcdata->haunter) && victim->pcdata->haunt_timer > 0 && ch->pcdata->watching > 0)
      return TRUE;
    }

    if (!IS_NPC(ch) && ch->pcdata->brandstatus >= 2) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL)
          continue;
          if (str_cmp(to->pcdata->brander, ch->name))
          continue;
          if (!str_cmp(ch->pcdata->brander, to->name))
          continue;

          if (can_see_char_distance(to, victim, range))
          return TRUE;
        }
      }
    }

    int maxrooms = 45;
    if (range == DISTANCE_MEDIUM)
    maxrooms = 30;
    if (range == DISTANCE_NEAR)
    maxrooms = 15;

    if (is_animal(victim) && animal_size(get_animal_weight(
            victim, ANIMAL_ACTIVE)) == ANIMAL_MONSTEROUS)
    maxrooms *= 2;

    if (guestmonster(victim) && victim->pcdata->height_feet >= 10)
    maxrooms *= 4;

    if (ch->in_room->x - victim->in_room->x > maxrooms)
    return FALSE;
    if (ch->in_room->y - victim->in_room->y > maxrooms)
    return FALSE;
    if (ch->in_room->x - victim->in_room->x < -1 * maxrooms)
    return FALSE;
    if (ch->in_room->y - victim->in_room->y < -1 * maxrooms)
    return FALSE;

    if (in_fight(ch) && combat_distance(ch, victim, FALSE) <=
        get_skill(ch, SKILL_PERCEPTION) * 2 +
        get_skill(ch, SKILL_ACUTEHEAR) * 10 +
        (30 - get_skill(victim, SKILL_STEALTH) * 3)) {
      if (ch->in_room == victim->in_room || vision_exists(ch->in_room, roomdirection_3d(ch->in_room->x, ch->in_room->y, ch->in_room->z, victim->in_room->x, victim->in_room->y, victim->in_room->z)))
      return TRUE;
    }

    //    if(get_dist(ch->in_room->x, ch->in_room->y, victim->in_room->x, //    victim->in_room->y) > 100)
    //	return FALSE;

    if (IS_NPC(ch))
    return TRUE;
    if (ch->in_room == victim->in_room) {
      if (IS_NPC(ch))
      return TRUE;

      if (can_see(ch, victim)) {
        if (!in_fight(ch))
        return TRUE;
        else if (invisioncone_character(ch, victim))
        return TRUE;
      }
    }

    int foundrange = charlineofsight_character(ch, victim);
    if (ch->in_room != victim->in_room && foundrange <= range && foundrange != -1) {
      if (can_see(ch, victim))
      return TRUE;
    }
    return FALSE;
  }

  bool can_see_room_distance(CHAR_DATA *ch, ROOM_INDEX_DATA *finish_room, int range) {
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *temp_room;
    int door, door_two;
    EXIT_DATA *pexit;

    if (ch->in_room == NULL || finish_room == NULL)
    return FALSE;

    if (battleground(ch->in_room) && !battleground(finish_room))
    return FALSE;
    if (ch->in_room->x - finish_room->x > 100)
    return FALSE;
    if (ch->in_room->y - finish_room->y > 100)
    return FALSE;
    if (ch->in_room->x - finish_room->x < -100)
    return FALSE;
    if (ch->in_room->y - finish_room->y < -100)
    return FALSE;

    in_room = ch->in_room;
    door = ch->facing;

    if (ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && in_haven(get_room_index(ch->pcdata->travel_to)) && in_haven(get_room_index(ch->pcdata->travel_from)) && in_haven(ch->your_car->in_room) && ch->in_room->vnum != ch->your_car->in_room->vnum && finish_room != ch->your_car->in_room) {
      bool can_see;
      ROOM_INDEX_DATA *orig;
      orig = ch->in_room;
      char_from_room(ch);
      char_to_room(ch, ch->your_car->in_room);
      can_see = can_see_room_distance(ch, finish_room, range);
      char_from_room(ch);
      char_to_room(ch, orig);
      return can_see;
    }
    if (in_room == finish_room)
    return TRUE;

    if (in_room == NULL || finish_room == NULL)
    return FALSE;

    int foundrange = charlineofsight_room(ch, finish_room);
    if (foundrange == -1)
    return FALSE;
    if (foundrange <= range)
    return TRUE;

    return FALSE;

    if (get_dist3d(ch->in_room->x, ch->in_room->y, ch->in_room->z, finish_room->x, finish_room->y, finish_room->z) >
        room_visibility_range(ch, ch->in_room, finish_room))
    return FALSE;

    // 1 north
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      if (temp_room == finish_room)
      return TRUE;
      in_room = temp_room;
      // 2 north
      if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
        if (temp_room == finish_room)
        return TRUE;
        in_room = temp_room;
        // 3 north
        if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
          if (temp_room == finish_room)
          return TRUE;
        }
      }
    }
    if (door == DIR_UP || door == DIR_DOWN)
    return FALSE;

    door = ch->facing;
    in_room = ch->in_room;
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      in_room = temp_room;
      // n-u
      if ((pexit = in_room->exit[DIR_UP]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
        if (temp_room == finish_room)
        return TRUE;
        in_room = temp_room;
        // 3 north
        if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
          if (temp_room == finish_room)
          return TRUE;
          in_room = temp_room;
          // 3 north
          if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
            if (temp_room == finish_room)
            return TRUE;
          }
        }
      }
    }
    door = ch->facing;
    in_room = ch->in_room;
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      in_room = temp_room;
      // n-u
      if ((pexit = in_room->exit[DIR_DOWN]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
        if (temp_room == finish_room)
        return TRUE;
        in_room = temp_room;
        // 3 north
        if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
          if (temp_room == finish_room)
          return TRUE;
          in_room = temp_room;
          // 3 north
          if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
            if (temp_room == finish_room)
            return TRUE;
          }
        }
      }
    }

    door = turn_dir[ch->facing];
    in_room = ch->in_room;
    // 1 northeast
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      if (temp_room == finish_room)
      return TRUE;
    }
    door = turn_dir[door]; // e
    door = turn_dir[door]; // se
    door = turn_dir[door]; // s
    door = turn_dir[door]; // sw
    door = turn_dir[door]; // w
    door = turn_dir[door]; // nw
    in_room = ch->in_room;

    // 1 northwest
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      if (temp_room == finish_room)
      return TRUE;
    }

    door = ch->facing;
    door_two = turn_dir[ch->facing];

    door_two = turn_dir[door_two]; // e
    door_two = turn_dir[door_two]; // se
    door_two = turn_dir[door_two]; // s
    door_two = turn_dir[door_two]; // sw
    door_two = turn_dir[door_two]; // w
    door_two = turn_dir[door_two]; // nw
    in_room = ch->in_room;
    // north

    door = turn_dir[ch->facing];
    door = turn_dir[door]; // e
    in_room = ch->in_room;
    // east
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      if (temp_room == finish_room)
      return TRUE;
    }

    door = turn_dir[door]; // se
    door = turn_dir[door]; // s
    door = turn_dir[door]; // sw
    door = turn_dir[door]; // w
    in_room = ch->in_room;
    // west
    if ((pexit = in_room->exit[door]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && can_see_room(ch, pexit->u1.to_room)) {
      if (temp_room == finish_room)
      return TRUE;
    }
    return FALSE;
  }

  bool is_carried(CHAR_DATA *ch) {
    CHAR_DATA *victim;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (ch == victim)
      continue;
      if (victim->lifting == ch)
      return TRUE;
      if (victim->dragging == ch)
      return TRUE;
    }
    return FALSE;
  }

  _DOFUN(do_drag) {
    char arg[MSL];
    one_argument(argument, arg);
    CHAR_DATA *victim;

    if (is_gm(ch) && lair_room(ch->in_room)) {
      send_to_char("Does that seem even remotely like something you should be doing?\n\r", ch);
      return;
    }

    if (arg[0] == '\0') {
      send_to_char("Drag whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world_pc(argument)) != NULL && victim->pcdata->ghost_room == ch->in_room->vnum && (IS_FLAG(victim->act, PLR_GHOSTWALKING) || victim->pcdata->spectre > 0)) {
      if (is_helpless(ch)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      if (is_ghost(ch)) {
        send_to_char("You can't get a grip on them.\n\r", ch);
        return;
      }

      if (ch->dragging == victim) {
        printf_to_char(ch, "You let go of %s.\n\r", victim->name);
        act("You feel something happen to your body.", ch, NULL, victim, TO_VICT);
        act("$n lets go of $N.", ch, NULL, victim, TO_NOTVICT);
        ch->dragging = NULL;
        return;
      }

      if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD)) {
        send_to_char("You can't get a grip on them.\n\r", ch);
        return;
      }

      printf_to_char(ch, "You grab ahold of %s.\n\r", victim->name);
      act("You feel something happen to your body.", ch, NULL, victim, TO_VICT);
      act("$n grabs ahold of $N.", ch, NULL, victim, TO_NOTVICT);
      ch->dragging = victim;
      ch->lifting = NULL;
      WAIT_STATE(ch, PULSE_VIOLENCE);

      return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (ch->dragging == victim) {
      act("You let go of $N.", ch, NULL, victim, TO_CHAR);
      act("$n lets go of you.", ch, NULL, victim, TO_VICT);
      act("$n lets go of $N.", ch, NULL, victim, TO_NOTVICT);
      ch->dragging = NULL;
      return;
    }
    if (!IS_NPC(victim) && victim->pcdata->trance > 0)
    victim->pcdata->trance = -20;

    if (!is_helpless(victim)) {
      send_to_char("You can't get a grip on them.\n\r", ch);
      return;
    }

    if (is_carried(victim)) {
      send_to_char("Someone else is already carrying them.\n\r", ch);
      return;
    }

    act("You grab ahold of $N.", ch, NULL, victim, TO_CHAR);
    act("$n grabs ahold of you.", ch, NULL, victim, TO_VICT);
    act("$n grabs ahold of $N.", ch, NULL, victim, TO_NOTVICT);
    ch->dragging = victim;
    ch->lifting = NULL;
    WAIT_STATE(ch, PULSE_VIOLENCE);
  }

  _DOFUN(do_carry) {
    char arg[MSL];
    one_argument(argument, arg);
    CHAR_DATA *victim;

    if (is_gm(ch) && lair_room(ch->in_room)) {
      send_to_char("Does that seem even remotely like something you should be doing?\n\r", ch);
      return;
    }

    if (arg[0] == '\0') {
      send_to_char("Carry whom?\n\r", ch);
      return;
    }

    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    if (!IS_NPC(victim) && victim->pcdata->trance > 0)
    victim->pcdata->trance = -20;

    if (ch->lifting == victim) {
      act("You let go of $N.", ch, NULL, victim, TO_CHAR);
      act("$n lets go of you.", ch, NULL, victim, TO_VICT);
      act("$n lets go of $N.", ch, NULL, victim, TO_NOTVICT);
      ch->lifting = NULL;
      return;
    }

    if (!is_helpless(victim)) {
      act("You move to carry $N.", ch, NULL, victim, TO_CHAR);
      act("$n moves to carry you.", ch, NULL, victim, TO_VICT);
      ch->lifting = victim;
      ch->dragging = NULL;
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }

    if (is_carried(victim)) {
      send_to_char("Someone else is already carrying them.\n\r", ch);
      return;
    }

    act("You pick up $N.", ch, NULL, victim, TO_CHAR);
    act("$n picks you up.", ch, NULL, victim, TO_VICT);
    act("$n picks up $N.", ch, NULL, victim, TO_NOTVICT);
    ch->lifting = victim;
    ch->dragging = NULL;
    WAIT_STATE(ch, PULSE_VIOLENCE);
  }

  _DOFUN(do_second) {
    ROOM_INDEX_DATA *newroom;

    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum == 405471482 || ch->in_room->vnum == 404471482 || ch->in_room->vnum == 403471482) {
      newroom = get_room_index(406471482);
      ch->facing = DIR_NORTH;
    }
    else if (ch->in_room->vnum == 404511462 || ch->in_room->vnum == 405511462 || ch->in_room->vnum == 407511462) {
      newroom = get_room_index(406511462);
      ch->facing = DIR_WEST;
    }
    else if (ch->in_room->vnum == 28606 || ch->in_room->vnum == 28605 || ch->in_room->vnum == 28633)
    newroom = get_room_index(28621);
    else {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }

    if (newroom == NULL)
    return;

    act("$n gets on the elevator and hits the '2' button.", ch, NULL, NULL, TO_ROOM);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, newroom)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->dragging == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->lifting == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
    }
    char_from_room(ch);
    char_to_room(ch, newroom);

    act("$n steps off the elevator.", ch, NULL, NULL, TO_ROOM);

    do_function(ch, &do_look, "auto");
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }
  _DOFUN(do_fourth) {

    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    
    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || (ch->in_room->vnum != 28940 && ch->in_room->vnum != 29000 && ch->in_room->vnum != 28910)) {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }

    ROOM_INDEX_DATA *newroom = get_room_index(29000);
    if (newroom == NULL)
    return;

    act("$n gets on the elevator and hits the '4' button.", ch, NULL, NULL, TO_ROOM);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, newroom)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
      else if (ch->dragging == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
      else if (ch->lifting == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
    }
    char_from_room(ch);
    char_to_room(ch, newroom);

    act("$n steps off the elevator.", ch, NULL, NULL, TO_ROOM);

    do_function(ch, &do_look, "auto");
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }

  _DOFUN(do_third) {
    ROOM_INDEX_DATA *newroom;

    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    
    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum == 28606 || ch->in_room->vnum == 28605 || ch->in_room->vnum == 28621)
    newroom = get_room_index(28633);
    else if (ch->in_room->vnum == 404511462 || ch->in_room->vnum == 406511462 || ch->in_room->vnum == 405511462) {
      newroom = get_room_index(407511462);
      ch->facing = DIR_WEST;
    }
    else {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }

    if (newroom == NULL)
    return;

    act("$n gets on the elevator and hits the '3' button.", ch, NULL, NULL, TO_ROOM);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, newroom)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->dragging == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->lifting == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
    }
    char_from_room(ch);
    char_to_room(ch, newroom);

    act("$n steps off the elevator.", ch, NULL, NULL, TO_ROOM);

    do_function(ch, &do_look, "auto");
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }

  _DOFUN(do_basement) {
    ROOM_INDEX_DATA *newroom;

    if (ch->pcdata->ci_editing == 18) {
      ch->pcdata->ci_absorb = 1;
      ch->pcdata->ci_discipline = number_range(ENCOUNTER_ONE, ENCOUNTER_MAX);
      printf_to_char(ch, "New Base encounter: %s\n\r", encounter_prompt(ch, ch->pcdata->ci_discipline));
      return;
    }


    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    
    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum == 403471482 || ch->in_room->vnum == 405471482 || ch->in_room->vnum == 406471482) {
      newroom = get_room_index(404471482);
      ch->facing = DIR_NORTH;
    }
    else if (ch->in_room->vnum == 405511462 || ch->in_room->vnum == 406511462 || ch->in_room->vnum == 407511462) {
      newroom = get_room_index(404511462);
      ch->facing = DIR_WEST;
    }
    else if (ch->in_room->vnum == 28605 || ch->in_room->vnum == 28621 || ch->in_room->vnum == 28633)
    newroom = get_room_index(28606);
    else {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }

    if (newroom == NULL)
    return;

    act("$n gets on the elevator and hits the 'B1' button.", ch, NULL, NULL, TO_ROOM);

    for (CharList::iterator it = ch->in_room->people->begin(); it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, newroom)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->dragging == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->lifting == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
    }
    char_from_room(ch);
    char_to_room(ch, newroom);

    act("$n steps off the elevator.", ch, NULL, NULL, TO_ROOM);

    do_function(ch, &do_look, "auto");
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }

  _DOFUN(do_basementtwo) {
    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || (ch->in_room->vnum != 404471482 && ch->in_room->vnum != 405471482 && ch->in_room->vnum != 406471482)) {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }

    ROOM_INDEX_DATA *newroom = get_room_index(403471482);
    ch->facing = DIR_NORTH;
    if (newroom == NULL)
    return;

    act("$n gets on the elevator and hits the 'B2' button.", ch, NULL, NULL, TO_ROOM);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, newroom)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->dragging == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;

      }
      else if (ch->lifting == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
        fch->facing = ch->facing;
      }
    }
    char_from_room(ch);
    char_to_room(ch, newroom);

    act("$n steps off the elevator.", ch, NULL, NULL, TO_ROOM);

    do_function(ch, &do_look, "auto");
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }

  _DOFUN(do_escalator) {
    if (ch->position < 7) {
      send_to_char("Stand up first.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't manage to do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
      send_to_char("You're already moving.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not while you're in a fight.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You're being held down right now.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || (ch->in_room->vnum != 24040 && ch->in_room->vnum != 24002)) {
      send_to_char("You can't go that way.\n\r", ch);
      return;
    }
    ROOM_INDEX_DATA *newroom;
    if (ch->in_room->vnum == 24040)
    newroom = get_room_index(24006);
    else
    newroom = get_room_index(24034);
    if (newroom == NULL)
    return;

    act("$n steps on the escalator.", ch, NULL, NULL, TO_CHAR);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, newroom)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
      else if (ch->dragging == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
      else if (ch->lifting == fch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, newroom);
      }
    }
    char_from_room(ch);
    char_to_room(ch, newroom);

    act("$n steps off the ecalator.", ch, NULL, NULL, TO_ROOM);

    do_function(ch, &do_look, "auto");
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }
  
  // toggles superjumping preference - Discordance
  _DOFUN(do_superjump) {
    if (is_superjumping(ch)) {
      SET_FLAG(ch->act, PLR_SUPERJUMPING);
      send_to_char("You won't super jump.", ch);
    }
    else {
      REMOVE_FLAG(ch->act, PLR_SUPERJUMPING);
      send_to_char("You'll super jump without thinking twice.", ch);
    }
  }
  
  // moved from skills and added toggle with no argument - Discordance
  _DOFUN(do_fly) {
    if (!in_fight(ch)) {
      if (argument[0] == '\0') {
        if (is_flying(ch)) {
          SET_FLAG(ch->act, PLR_FLYING);
          send_to_char("You won't fly.\n\r", ch);
          return;
        }
        else {
          if (!can_fly(ch)) {
            send_to_char("You can't fly.\n\r", ch);
            return;
          }
          REMOVE_FLAG(ch->act, PLR_FLYING);
          send_to_char("You'll fly without thinking twice.\n\r", ch);
          return;
        }
      }
      char buf[MSL];
      sprintf(buf, "fly %s", argument);
      do_function(ch, &do_travel, buf);
      return;
    }
    do_function(ch, &do_move, "up");
  }

  bool has_exits(ROOM_INDEX_DATA *room) {
    int door;
    EXIT_DATA *pexit;

    for (door = 0; door <= 9; door++) {
      if ((pexit = room->exit[door]) != NULL && pexit->u1.to_room != NULL)
      return TRUE;
    }

    return FALSE;
  }

  _DOFUN(do_walk) {
    char arg1[MSL];
    int desti;
    ROOM_INDEX_DATA *room;
    int i;

    one_argument_nouncap(argument, arg1);

    if (!str_cmp(argument, "stop") || !str_cmp(argument, "")) {
      ch->destination = NULL;
      ch->walking = 0;
      send_to_char("You stop walking.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "remember")) {
      argument = one_argument_nouncap(argument, arg1);
      remember_location(ch, argument);
      return;
    }
    if (!str_cmp(arg1, "around")) {
      if (ch->pcdata->driving_around == TRUE) {
        ch->pcdata->driving_around = FALSE;
        send_to_char("You stop walking around.\n\r", ch);
        return;
      }
      else {
        ch->pcdata->driving_around = TRUE;
        send_to_char("You start walking around.\n\r", ch);
        return;
      }
    }
    if (!str_cmp(arg1, "list")) {
      send_to_char("You can walk to the following locations. See (`chelp landmarks`x) for more.\n\r", ch);

      for (i = 0; i < MAX_TAXIS; i++) {
        if ((room = get_room_index(taxi_table[i].vnum)) != NULL)
        printf_to_char(ch, "`W[`c%4d`W]`x %s\n\r", i + 1, room->name);
      }
      send_to_char("\n", ch);
      for (i = 0; i < 10; i++) {
        if (ch->pcdata->driveloc[i] > 0 && (room = get_room_index(ch->pcdata->driveloc[i])) != NULL) {
          printf_to_char(ch, "`W[`c%s`W]`x %s\n\r", ch->pcdata->drivenames[i], room->name);
        }
      }
      return;
    }
    ch->pcdata->driving_around = FALSE;
    int dnumber = landmark_vnum(argument, ch);
    if (dnumber > 0) {
      room = get_room_index(dnumber);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[0]) && ch->pcdata->driveloc[0] > 0 && get_room_index(ch->pcdata->driveloc[0]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[0]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[1]) && ch->pcdata->driveloc[1] > 0 && get_room_index(ch->pcdata->driveloc[1]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[1]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[2]) && ch->pcdata->driveloc[2] > 0 && get_room_index(ch->pcdata->driveloc[2]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[2]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[3]) && ch->pcdata->driveloc[3] > 0 && get_room_index(ch->pcdata->driveloc[3]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[3]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[4]) && ch->pcdata->driveloc[4] > 0 && get_room_index(ch->pcdata->driveloc[4]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[4]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[5]) && ch->pcdata->driveloc[5] > 0 && get_room_index(ch->pcdata->driveloc[5]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[5]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[6]) && ch->pcdata->driveloc[6] > 0 && get_room_index(ch->pcdata->driveloc[6]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[6]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[7]) && ch->pcdata->driveloc[7] > 0 && get_room_index(ch->pcdata->driveloc[7]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[7]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[8]) && ch->pcdata->driveloc[8] > 0 && get_room_index(ch->pcdata->driveloc[8]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[8]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }
    if (!str_cmp(argument, ch->pcdata->drivenames[9]) && ch->pcdata->driveloc[9] > 0 && get_room_index(ch->pcdata->driveloc[9]) != NULL) {
      room = get_room_index(ch->pcdata->driveloc[9]);
      ch->destination = room;
      ch->walking = 1;
      send_to_char("You start walking.\n\r", ch);
      if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
      ch->destination = get_room_index(ch->pcdata->lured_room);
      return;
    }

    desti = atoi(argument);

    if (desti < 0 || desti > 100000) {
      send_to_char("walk (number)\n\r", ch);
      return;
    }

    if (desti <= MAX_TAXIS) {
      room = get_room_index(taxi_table[desti - 1].vnum);
    }
    else {
      return;
    }
    ch->destination = room;
    ch->walking = 1;
    send_to_char("You start walking.\n\r", ch);
    if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL)
    ch->destination = get_room_index(ch->pcdata->lured_room);
  }

  _DOFUN(do_pass) {
    if (in_fight(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (!IS_FLAG(ch->comm, COMM_CARDINAL) || ch->played / 3600 < 20) {
      for (int i = 0; i < 10; i++) {
        if (!str_cmp(dir_name[i][0], argument) || !str_cmp(dir_name[i][1], argument)) {
          do_function(ch, &do_open, argument);
          move_char(ch, i, FALSE, FALSE);
          WAIT_STATE(ch, move_speed(ch));
          return;
        }
      }
    }
    if (IS_FLAG(ch->comm, COMM_CARDINAL) || ch->played / 3600 < 20) {
      for (int i = 0; i < 10; i++) {
        if (!str_cmp(reldir_name[i][0], argument) || !str_cmp(reldir_name[i][1], argument)) {
          do_function(ch, &do_open, argument);
          move_char(ch, get_absoldirection(i, ch->facing), FALSE, FALSE);
          WAIT_STATE(ch, move_speed(ch));
          return;
        }
      }
    }

    send_to_char("No such direction.\n\r", ch);
  }

  int turns_to_get(int facing, int truedir) {
    if (facing == truedir)
    return 0;
    int count = 0;

    for (int i = 0; i < 10; i++) {
      facing = turn_dir[facing];
      count++;
      if (facing == truedir)
      return count;
    }
    return 0;
  }

  int get_reldirection(int truedir, int facing) {

    if (truedir == DIR_DOWN || truedir == DIR_UP)
    return truedir;

    if (facing == DIR_NORTH || facing == DIR_UP || facing == DIR_DOWN)
    return truedir;

    int val = turns_to_get(facing, truedir);
    int dir = 0;

    for (int i = 0; i < val; i++)
    dir = turn_dir[dir];

    return dir;
  }
  int get_absoldirection(int truedir, int facing) {
    if (truedir == DIR_DOWN || truedir == DIR_UP)
    return truedir;

    if (facing == DIR_NORTH || facing == DIR_UP || facing == DIR_DOWN)
    return truedir;

    int val = turns_to_get(0, truedir);
    int dir = facing;

    for (int i = 0; i < val; i++)
    dir = turn_dir[dir];

    return dir;
  }

  _DOFUN(do_cardinal) {
    if (IS_FLAG(ch->comm, COMM_CARDINAL)) {
      send_to_char("You'll now use cardinal directions.\n\r", ch);
      social_behave_mod(ch, -10, "flipping between cardinal and not.");
      REMOVE_FLAG(ch->comm, COMM_CARDINAL);
    }
    else {
      send_to_char("You'll now use relational directions.\n\r", ch);
      SET_FLAG(ch->comm, COMM_CARDINAL);
    }
  }

  _DOFUN(do_wanderhome) {

    if (ch->in_room != NULL && ch->in_room->sector_type != SECT_FOREST && ch->in_room->vnum > 100) {
      send_to_char("You can't do that from here.\n\r", ch);
      return;
    }

    if (room_hostile(ch->in_room) || IS_FLAG(ch->act, PLR_BOUND) || in_fight(ch) || move_helpless(ch) || IS_FLAG(ch->act, PLR_DEAD))
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("Leave the shroud first.\n\r", ch);
      return;
    }

    act("$n wanders back to town.", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(105550492));
    act("$n wanders in from the forests.", ch, NULL, NULL, TO_ROOM);
    WAIT_STATE(ch, PULSE_PER_SECOND * 5);
    send_to_char("You wander back to town.", ch);
    ch->facing = DIR_NORTH;
  }

  int landmark_vnum(char *landmark, CHAR_DATA *ch) {
    char arg[MSL];
    one_argument(landmark, arg);

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); it++) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (!super_aware(ch) && (*it)->type_special == PROPERTY_OUTERFOREST)
      continue;

      if (!str_cmp(trimwhitespace((*it)->address), landmark)) {
        if ((*it)->type_special == PROPERTY_SMALLFLAT || (*it)->type_special == PROPERTY_MEDFLAT || (*it)->type_special == PROPERTY_BIGFLAT)
        return 15240;
        return (*it)->roadroom;
      }
      if ((*it)->type == PROP_SHOP) {
        if (!str_cmp(trimwhitespace(from_color((*it)->propname)), landmark))
        return (*it)->roadroom;
      }
    }

    if (strcasestr(arg, "Lighthouse") != NULL || strcasestr(arg, "Beacon") != NULL || strcasestr(arg, "Willow") != NULL || strcasestr(arg, "Cove") != NULL)
    return 6621;
    if (strcasestr(arg, "Lookout") != NULL || strcasestr(arg, "Point") != NULL)
    return 17575;
    if (strcasestr(arg, "Drive-in") != NULL)
    return 9636;
    if (strcasestr(arg, "Institute") != NULL || strcasestr(arg, "Institution") != NULL || strcasestr(arg, "Asylum") != NULL || strcasestr(arg, "School") != NULL || strcasestr(arg, "College") != NULL || strcasestr(arg, "Medical") != NULL || strcasestr(arg, "Clinic") != NULL || strcasestr(arg, "Hospital") != NULL)
    return 16026;
    if (strcasestr(arg, "Shipping") != NULL || strcasestr(arg, "Container") != NULL || strcasestr(arg, "Storage") != NULL)
    return 15273;
    if (strcasestr(arg, "Bridge") != NULL)
    return 5866;
    if (strcasestr(arg, "Overlook") != NULL || strcasestr(arg, "Underlook") != NULL)
    return 3449;
    if (strcasestr(arg, "Baseball") != NULL)
    return 5432;
    if (strcasestr(arg, "Longhouse") != NULL)
    return 14258;
    if (strcasestr(arg, "Clay") != NULL || strcasestr(arg, "Clay's") != NULL || strcasestr(arg, "Gas") != NULL)
    return 2163;
    if (strcasestr(arg, "Lodge") != NULL)
    return 2633;
    if (strcasestr(arg, "Antlers") != NULL || strcasestr(arg, "Hotel") != NULL)
    return 2623;
    if (strcasestr(arg, "Apartments") != NULL)
    return 15240;
    if (strcasestr(arg, "Courier") != NULL || strcasestr(arg, "Newspaper") != NULL || strcasestr(arg, "Paper") != NULL)
    return 2625;
    if (strcasestr(arg, "Post") != NULL || strcasestr(arg, "Mail") != NULL)
    return 2623;
    if (strcasestr(arg, "Town") != NULL)
    return 2948;
    if (strcasestr(arg, "Rooms") != NULL || strcasestr(arg, "Furniture") != NULL)
    return 14851;
    if (strcasestr(arg, "Hunt") != NULL || strcasestr(arg, "Hook") != NULL || strcasestr(arg, "Hunting") != NULL)
    return 14848;
    if (strcasestr(arg, "Narnia") != NULL || strcasestr(arg, "Thrift") != NULL)
    return 2946;
    if (strcasestr(arg, "Bank") != NULL || strcasestr(arg, "National") != NULL)
    return 14844;
    if (strcasestr(arg, "Library") != NULL || strcasestr(arg, "Planetarium") != NULL)
    return 14838;
    if (strcasestr(arg, "Court") != NULL || strcasestr(arg, "Judge") != NULL)
    return 14829;
    if (strcasestr(arg, "Sheriff") != NULL || strcasestr(arg, "HSD") != NULL)
    return 14824;
    if (strcasestr(arg, "Gallows") != NULL)
    return 1153;
    if (strcasestr(arg, "Arkwright") != NULL || strcasestr(arg, "Cemetery") != NULL)
    return 3184;
    if (strcasestr(arg, "Steel") != NULL || strcasestr(arg, "Mill") != NULL)
    return 15180;
    if (strcasestr(arg, "Badwater") != NULL)
    return 15263;
    if (strcasestr(arg, "Dive") != NULL || strcasestr(arg, "Bar") != NULL || strcasestr(arg, "Strip") != NULL)
    return 1689;
    if (strcasestr(arg, "Apartment") != NULL)
    return 15247;
    if (strcasestr(arg, "Sidney Beach") != NULL)
    return 1176;
    if (strcasestr(arg, "Westhaven") != NULL || strcasestr(arg, "Trailer") != NULL)
    return 14273;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); it++) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (!super_aware(ch) && (*it)->type_special == PROPERTY_OUTERFOREST)
      continue;

      if (!str_cmp((*it)->address, landmark)) {
        if ((*it)->type_special == PROPERTY_SMALLFLAT || (*it)->type_special == PROPERTY_MEDFLAT || (*it)->type_special == PROPERTY_BIGFLAT)
        return 15240;
        return (*it)->roadroom;
      }
      if (strcasestr(landmark, (*it)->address) != NULL) {
        if ((*it)->type_special == PROPERTY_SMALLFLAT || (*it)->type_special == PROPERTY_MEDFLAT || (*it)->type_special == PROPERTY_BIGFLAT)
        return 15240;
        return (*it)->roadroom;
      }
      if ((*it)->type == PROP_SHOP) {
        if (!str_cmp(from_color((*it)->propname), landmark))
        return (*it)->roadroom;
        if (strcasestr(landmark, from_color((*it)->propname)) != NULL)
        return (*it)->roadroom;
      }
    }

    return 0;
  }

  bool valid_scent_word(char *word) {
    if (!str_cmp(word, "A"))
    return FALSE;
    if (!str_cmp(word, "of"))
    return FALSE;
    if (!str_cmp(word, "the"))
    return FALSE;
    if (!str_cmp(word, "scent"))
    return FALSE;
    if (!str_cmp(word, "smell"))
    return FALSE;
    if (!str_cmp(word, "natural"))
    return FALSE;
    if (!str_cmp(word, "mingled"))
    return FALSE;
    if (!str_cmp(word, "mixed"))
    return FALSE;
    if (!str_cmp(word, "mixes"))
    return FALSE;
    if (!str_cmp(word, "mingles"))
    return FALSE;
    if (!str_cmp(word, "subtle"))
    return FALSE;
    if (!str_cmp(word, "light"))
    return FALSE;
    if (!str_cmp(word, "musk"))
    return FALSE;
    if (!str_cmp(word, "overpowering"))
    return FALSE;
    if (!str_cmp(word, "strong"))
    return FALSE;
    if (!str_cmp(word, "fragrance"))
    return FALSE;
    if (!str_cmp(word, "and"))
    return FALSE;
    if (!str_cmp(word, "masculine"))
    return FALSE;
    if (!str_cmp(word, "feminine"))
    return FALSE;
    if (!str_cmp(word, "air"))
    return FALSE;
    if (!str_cmp(word, "clings"))
    return FALSE;
    if (!str_cmp(word, "surrounds"))
    return FALSE;
    if (!str_cmp(word, "waft"))
    return FALSE;
    if (!str_cmp(word, "subtle"))
    return FALSE;
    if (!str_cmp(word, "vaguely"))
    return FALSE;
    if (!str_cmp(word, "young"))
    return FALSE;
    if (!str_cmp(word, "old"))
    return FALSE;
    if (!str_cmp(word, "woman"))
    return FALSE;
    if (!str_cmp(word, "man"))
    return FALSE;
    if (!str_cmp(word, "she"))
    return FALSE;
    if (!str_cmp(word, "he"))
    return FALSE;
    if (!str_cmp(word, "if"))
    return FALSE;
    if (!str_cmp(word, "as"))
    return FALSE;
    if (!str_cmp(word, "with"))
    return FALSE;
    if (!str_cmp(word, "smells"))
    return FALSE;
    if (!str_cmp(word, "hint"))
    return FALSE;
    if (!str_cmp(word, "trace"))
    return FALSE;
    if (!str_cmp(word, "skin"))
    return FALSE;
    if (!str_cmp(word, "clean"))
    return FALSE;
    if (!str_cmp(word, "bears"))
    return FALSE;
    if (!str_cmp(word, "overlay"))
    return FALSE;
    if (!str_cmp(word, "typical"))
    return FALSE;
    if (!str_cmp(word, "faint"))
    return FALSE;
    if (!str_cmp(word, "somewhere"))
    return FALSE;
    if (!str_cmp(word, "somewhat"))
    return FALSE;
    if (!str_cmp(word, "around"))
    return FALSE;
    if (!str_cmp(word, "this"))
    return FALSE;
    if (!str_cmp(word, "clinging"))
    return FALSE;
    if (!str_cmp(word, "faint"))
    return FALSE;
    if (!str_cmp(word, "clothes"))
    return FALSE;
    if (!str_cmp(word, "skin"))
    return FALSE;
    if (!str_cmp(word, "soap"))
    return FALSE;
    if (!str_cmp(word, "perfume"))
    return FALSE;
    if (!str_cmp(word, "his"))
    return FALSE;
    if (!str_cmp(word, "her"))
    return FALSE;
    if (!str_cmp(word, "captivating"))
    return FALSE;
    if (!str_cmp(word, "to"))
    return FALSE;
    if (!str_cmp(word, "faintly"))
    return FALSE;
    if (!str_cmp(word, "plain"))
    return FALSE;
    if (!str_cmp(word, "on"))
    return FALSE;
    if (!str_cmp(word, "cloys"))
    return FALSE;
    if (!str_cmp(word, "gently"))
    return FALSE;
    if (!str_cmp(word, "drifting"))
    return FALSE;
    if (!str_cmp(word, "there's"))
    return FALSE;
    if (!str_cmp(word, "enigmatic"))
    return FALSE;
    if (!str_cmp(word, "working"))
    return FALSE;
    if (!str_cmp(word, "he's"))
    return FALSE;
    if (!str_cmp(word, "she's"))
    return FALSE;
    if (!str_cmp(word, "fine"))
    return FALSE;
    if (!str_cmp(word, "suffuses"))
    return FALSE;
    if (!str_cmp(word, "blend"))
    return FALSE;
    if (!str_cmp(word, "pure"))
    return FALSE;
    if (!str_cmp(word, "lightly"))
    return FALSE;
    if (!str_cmp(word, "that"))
    return FALSE;
    if (!str_cmp(word, "dark"))
    return FALSE;
    if (!str_cmp(word, "scents"))
    return FALSE;
    if (!str_cmp(word, "them"))
    return FALSE;
    if (!str_cmp(word, "blend"))
    return FALSE;
    if (!str_cmp(word, "creamy"))
    return FALSE;
    if (!str_cmp(word, "been"))
    return FALSE;
    if (!str_cmp(word, "cool"))
    return FALSE;
    if (!str_cmp(word, "tones"))
    return FALSE;
    if (!str_cmp(word, "like"))
    return FALSE;
    if (!str_cmp(word, "lingering"))
    return FALSE;
    if (!str_cmp(word, "almost"))
    return FALSE;
    if (!str_cmp(word, "baked"))
    return FALSE;
    if (!str_cmp(word, "subtly"))
    return FALSE;
    if (!str_cmp(word, "into"))
    return FALSE;
    if (!str_cmp(word, "is"))
    return FALSE;
    if (!str_cmp(word, "it"))
    return FALSE;
    if (!str_cmp(word, "ivory"))
    return FALSE;
    if (!str_cmp(word, "plain"))
    return FALSE;
    if (!str_cmp(word, "very"))
    return FALSE;
    if (!str_cmp(word, "surrounding"))
    return FALSE;
    if (!str_cmp(word, "unique"))
    return FALSE;
    if (!str_cmp(word, "blending"))
    return FALSE;
    if (!str_cmp(word, "rustic"))
    return FALSE;
    if (!str_cmp(word, "lingers"))
    return FALSE;
    if (!str_cmp(word, "white"))
    return FALSE;
    if (!str_cmp(word, "her"))
    return FALSE;
    if (!str_cmp(word, "boasts"))
    return FALSE;
    if (!str_cmp(word, "smell"))
    return FALSE;
    if (!str_cmp(word, "has"))
    return FALSE;
    if (!str_cmp(word, "color"))
    return FALSE;
    if (!str_cmp(word, "toward"))
    return FALSE;
    if (!str_cmp(word, "veering"))
    return FALSE;
    if (!str_cmp(word, "about"))
    return FALSE;
    if (!str_cmp(word, "into"))
    return FALSE;
    if (!str_cmp(word, "slightly"))
    return FALSE;
    if (!str_cmp(word, "from"))
    return FALSE;
    if (!str_cmp(word, "skin"))
    return FALSE;
    if (!str_cmp(word, "delivered"))
    return FALSE;
    if (!str_cmp(word, "under"))
    return FALSE;
    if (!str_cmp(word, "hidden"))
    return FALSE;
    if (!str_cmp(word, "it;"))
    return FALSE;
    if (!str_cmp(word, "from"))
    return FALSE;
    if (!str_cmp(word, "more"))
    return FALSE;
    if (!str_cmp(word, "quickest"))
    return FALSE;
    if (!str_cmp(word, "surest"))
    return FALSE;
    if (!str_cmp(word, "slightly"))
    return FALSE;
    if (!str_cmp(word, "him,"))
    return FALSE;
    if (!str_cmp(word, "wiff"))
    return FALSE;
    if (!str_cmp(word, "intermingled"))
    return FALSE;
    if (!str_cmp(word, "carries"))
    return FALSE;
    if (!str_cmp(word, "truly"))
    return FALSE;
    if (!str_cmp(word, "notes"))
    return FALSE;
    if (!str_cmp(word, "haunting"))
    return FALSE;

    return TRUE;
  }

  char *find_smell_word(char *argument) {
    char arg[MSL];
    while (argument[0] != '\0') {
      arg[0] = '\0';
      argument = one_argument_true(argument, arg);

      if (valid_scent_word(arg) && number_percent() % 2 == 0)
      return str_dup(arg);
    }
    return "";
  }

  char *smell_line(CHAR_DATA *ch) {
    char buf[MSL];
    char mess[MSL];
    if (is_animal(ch)) {
      if (ch->sex == SEX_MALE) {
        sprintf(mess, "A male %s", genus_name(get_animal_genus(ch, ANIMAL_ACTIVE)));
      }
      else {
        sprintf(mess, "A female %s", genus_name(get_animal_genus(ch, ANIMAL_ACTIVE)));
      }
      return str_dup(mess);
    }
    else {
      char arg[MSL];
      remove_color(arg, ch->pcdata->scent);
      sprintf(buf, "%s", find_smell_word(arg));
      if (safe_strlen(buf) > 1) {
        sprintf(mess, "A human scent mixed with %s", buf);
        return str_dup(mess);
      }
      remove_color(arg, ch->pcdata->focused_descs[COVERS_SMELL]);
      sprintf(buf, "%s", find_smell_word(arg));
      if (safe_strlen(buf) > 1) {
        sprintf(mess, "A human scent mixed with %s", buf);
        return str_dup(mess);
      }
      if (ch->sex == SEX_MALE)
      return "A male human scent";
      else
      return "A female human scent";
    }
  }

  void append_smell(CHAR_DATA *ch, int dir, ROOM_INDEX_DATA *room) {
    if (IS_NPC(ch))
    return;
    if (IS_FLAG(ch->act, PLR_SHROUD))
    return;
    if (in_water(ch))
    return;
    for (int i = 0; i < 2; i++) {
      free_string(room->smell_desc[i + 1]);
      room->smell_desc[i + 1] = str_dup(room->smell_desc[i]);
      room->smell_dir[i + 1] = room->smell_dir[i];
    }
    free_string(room->smell_desc[0]);
    room->smell_desc[0] = str_dup(smell_line(ch));
    room->smell_dir[0] = dir;
  }

  _DOFUN(do_privatesecurity)
  {
    free_string(ch->pcdata->private_security);
    ch->pcdata->private_security = str_dup(argument);
    printf_to_char(ch, "Your private security will now watch out for: %s\n\r", argument);
    return;
  }

  bool security_room_blocked(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
  {
    char logs[MSL];
    CHAR_DATA *to;
    for (CharList::iterator it = room->people->begin();
    it != room->people->end(); ++it) {
      to = *it;

      if (to == NULL)
      continue;

      if(in_fight(to) || is_helpless(to))
      continue;

      if(!IS_NPC(to) && to->pcdata->guard_number > 0 && to->pcdata->guard_expiration > current_time)
      {
        if(strcasestr(to->pcdata->private_security, ch->name) != NULL)
        {
          if(!public_room(room) || get_tier(ch) < 2)
          {
            sprintf(logs, "ROOMBLOCK: %s blocked %s from room %d", to->name, ch->name, room->vnum);
            log_string(logs);
            return TRUE;
          }
        }
      }
    }
    to = NULL;
    for (vector<STORY_TYPE *>::iterator sit = StoryVect.begin();
    sit != StoryVect.end(); ++sit) {
      if ((*sit)->valid == FALSE)
      continue;

      if(!can_enter_event(ch, (*sit)))
      {
        to = get_char_world_pc((*sit)->author);
        if(to != NULL && to->in_room == room)
        {
          sprintf(logs, "ROOMBLOCK Calendar Room: %s blocked %s from room %d", to->name, ch->name, room->vnum);
          log_string(logs);

          return TRUE;
        }
        if(to != NULL)
        {
          PROP_TYPE *pone = prop_from_room(room);
          PROP_TYPE *ptwo = prop_from_room(to->in_room);
          if(pone == ptwo && pone != NULL)
          {
            sprintf(logs, "ROOMBLOCK Calendar Property: %s blocked %s from room %d", to->name, ch->name, room->vnum);
            log_string(logs);

            return TRUE;
          }
        }

      }
    }
    return FALSE;
  }
  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

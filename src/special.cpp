#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif
#if defined(macintosh)
#include <time.h>
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

#include "gsn.h"
#include "merc.h"

#if defined(__cplusplus)
extern "C" {
#endif

  /*
* The following special functions are available for mobiles.
*/
  DECLARE_SPEC_FUN(spec_cast_adept);
  DECLARE_SPEC_FUN(spec_executioner);
  DECLARE_SPEC_FUN(spec_guard);
  DECLARE_SPEC_FUN(spec_janitor);
  DECLARE_SPEC_FUN(spec_mayor);
  DECLARE_SPEC_FUN(spec_poison);
  DECLARE_SPEC_FUN(spec_thief);
  DECLARE_SPEC_FUN(spec_questmaster);
  DECLARE_SPEC_FUN(spec_guard_race);
  DECLARE_SPEC_FUN(spec_guard_guild);
  DECLARE_SPEC_FUN(spec_object_test);
  DECLARE_SPEC_FUN(spec_archer);
  DECLARE_SPEC_FUN(spec_cast_attack_weave);

  /* the function table */
  const struct spec_type spec_table[] = {
    {"spec_cast_adept", spec_cast_adept, SPEC_MOB}, {"spec_guard", spec_guard, SPEC_MOB}, {"spec_janitor", spec_janitor, SPEC_MOB}, {"spec_mayor", spec_mayor, SPEC_MOB}, {"spec_poison", spec_poison, SPEC_MOB}, {"spec_thief", spec_thief, SPEC_MOB}, {"spec_questmaster", spec_questmaster, SPEC_MOB}, {"spec_guard_race", spec_guard_race, SPEC_MOB}, {"spec_guard_guild", spec_guard_guild, SPEC_MOB}, {"spec_object_test", spec_object_test, SPEC_OBJ}, {"spec_archer", spec_archer, SPEC_MOB}, {"spec_cast_attack_weave", spec_cast_attack_weave, SPEC_MOB}, {NULL, NULL, 0}};

  /*
* Given a name, return the appropriate spec fun.
*/
  SPEC_FUN *spec_lookup(const char *name, long type) {
    int i;

    for (i = 0; spec_table[i].name != NULL; i++) {
      if (LOWER(name[0]) == LOWER(spec_table[i].name[0]) && !str_prefix(name, spec_table[i].name) && (IS_SET(spec_table[i].type, type) || type == 0))
      return spec_table[i].function;
    }

    return 0;
  }

  char *spec_name(SPEC_FUN *function) {
    int i;

    for (i = 0; spec_table[i].function != NULL; i++) {
      if (function == spec_table[i].function)
      return spec_table[i].name;
    }

    return NULL;
  }

  bool spec_questmaster(CHAR_DATA *ch, OBJ_DATA *obj) { return TRUE; }

  /****
* Casts some beneficial spells on the targets.
*/
  bool spec_cast_adept(CHAR_DATA *ch, OBJ_DATA *obj) {
    CHAR_DATA *victim = NULL;

    if (!IS_AWAKE(ch))
    return FALSE;

    CharList::iterator it = ch->in_room->people->begin();
    while (it != ch->in_room->people->end()) {
      victim = *it;
      ++it;

      if (victim != ch && can_see(ch, victim) && number_bits(1) == 0 && !IS_NPC(victim) && victim->level < 11)
      break;
    }

    if (victim == NULL)
    return FALSE;

    return FALSE;
  }

  bool spec_guard(CHAR_DATA *ch, OBJ_DATA *obj) { return FALSE; }

  bool spec_janitor(CHAR_DATA *ch, OBJ_DATA *obj) {
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if (!IS_AWAKE(ch))
    return FALSE;

    for (trash = ch->in_room->contents; trash != NULL; trash = trash_next) {
      trash_next = trash->next_content;

      if (!IS_SET(trash->wear_flags, ITEM_TAKE) || !can_loot(ch, trash))
      continue;

      if (trash->item_type == ITEM_DRINK_CON || trash->item_type == ITEM_TRASH || trash->cost < 10) {
        act("$n picks up some trash.", ch, NULL, NULL, TO_ROOM);
        obj_from_room(trash);
        obj_to_char(trash, ch);
        return TRUE;
      }
    }

    return FALSE;
  }

  bool spec_mayor(CHAR_DATA *ch, OBJ_DATA *obj) {
    static const char open_path[] = "22";

    static const char *path;
    static int pos;
    static bool move;

    if (!move) {
      path = open_path;
      move = TRUE;
      pos = 0;
    }

    switch (path[pos]) {
    case '0':
    case '1':
    case '2':
    case '3':
      move_char(ch, path[pos] - '0', FALSE, FALSE);
      break;
    case 's':
      move_char(ch, DIR_SOUTHWEST, FALSE, FALSE);
      break;
    case 'G':
      act("$n looks around the room.", ch, NULL, NULL, TO_ROOM);
      act("$n says 'The Patterns Tour is about to begin. If you wish to come, just follow me.'", ch, NULL, NULL, TO_ROOM);
      break;

    case 'S':
      act("$n says 'Julie is one of my friends on the mud. She is very smart, and is able to teach and train people in almost anything.'.", ch, NULL, NULL, TO_ROOM);
      break;

    case 'a':
      act("$n says 'My friend the ogier. He keeps the arena safe from bigger players'", ch, NULL, NULL, TO_ROOM);
      break;

    case 'b':
      act("$n says 'While we like rp here, most people want a few levels. If you are one of them you can try fighting the man here. Simply type Kill guard'", ch, NULL, NULL, TO_ROOM);
      act("$n says 'Normaly you would con them first, but you can trust me'/n/r $n grins.", ch, NULL, NULL, TO_ROOM);
      break;

    case 'c':
      act("$n says 'The mobs in the northern part of the mudschool are level 4 and 5 so you may want to kill them.'", ch, NULL, NULL, TO_ROOM);
      act("$n says 'We won't go there today.", ch, NULL, NULL, TO_ROOM);
      break;

    case 'd':
      act("$n says 'Back here again. If anyone wants to come, I will be touring four kings. Just follow me.'", ch, NULL, NULL, TO_ROOM);
      break;

    case 'e':
      act("$n says 'Artur Hawkwing. Protector of the innocent, and great hero. Now his statue stands over Four Kings, protecting against thieves, killers, and, naturally darkfrinds.'", ch, NULL, NULL, TO_ROOM);
      break;

    case 'E':
      act("$n says 'There are many stores in four kings. Herb shops, pet shops, armor shops, and a bank. This is the bank.'", ch, NULL, NULL, TO_ROOM);
      break;

    case 'O':
      break;

    case 'C':
      break;

    case '.':
      move = FALSE;
      break;
    }

    pos++;
    return FALSE;
  }

  bool spec_poison(CHAR_DATA *ch, OBJ_DATA *obj) {
    CHAR_DATA *victim;

    if (ch->position != POS_FIGHTING || (victim = ch->fighting) == NULL || number_percent() > 2 * ch->level)
    return FALSE;

    act("You bite $N!", ch, NULL, victim, TO_CHAR);
    act("$n bites $N!", ch, NULL, victim, TO_NOTVICT);
    act("$n bites you!", ch, NULL, victim, TO_VICT);
    return TRUE;
  }

  bool spec_thief(CHAR_DATA *ch, OBJ_DATA *obj) {
    long copper;

    if (ch->position != POS_STANDING)
    return FALSE;

    CharList::iterator it = ch->in_room->people->begin();
    while (it != ch->in_room->people->end()) {
      CHAR_DATA *victim = *it;
      ++it;

      if (IS_NPC(victim) || victim->level >= LEVEL_IMMORTAL || number_bits(5) != 0 || !can_see(ch, victim))
      continue;

      if (IS_AWAKE(victim) && number_range(0, ch->level) == 0) {
        act("You discover $n's hands in your wallet!", ch, NULL, victim, TO_VICT);
        act("$N discovers $n's hands in $S wallet!", ch, NULL, victim, TO_NOTVICT);
        return TRUE;
      }
      else {

        copper = victim->money * UMIN(number_range(1, 20), ch->level / 2) / 100;
        copper = UMIN(copper, ch->level * ch->level * 10);
        ch->money += copper;
        victim->money -= copper;
        return TRUE;
      }
    }

    return FALSE;
  }

  bool spec_guard_race(CHAR_DATA *ch, OBJ_DATA *obj) {
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim = NULL;
    char *crime;

    if (!IS_AWAKE(ch) || ch->fighting != NULL)
    return FALSE;

    crime = "";

    CharList::iterator it = ch->in_room->people->begin();
    while (it != ch->in_room->people->end()) {
      victim = *it;
      ++it;

      if (IS_NPC(victim))
      continue;

      /*	else if(!str_cmp("aiel", subrace_table[victim->sub_race].name)
&& (ch->race == RACE_HUMAN && str_cmp("aiel", subrace_table[ch->sub_race].name)))
{
crime = capitalize(subrace_table[victim->sub_race].name);
break;
}
*/
    }

    if (it != ch->in_room->people->end()) {
      sprintf(buf, "%s is a %s!  `8PROTECT THE INNOCENT!!`x ", victim->name, crime);
      REMOVE_FLAG(ch->comm, COMM_NOYELL);
      do_function(ch, &do_yell, buf);
      return TRUE;
    }

    return FALSE;
  }

  bool spec_guard_guild(CHAR_DATA *ch, OBJ_DATA *obj) { return FALSE; }

  bool spec_object_test(CHAR_DATA *ch, OBJ_DATA *obj) {
    send_to_char("This is a test!\n\r", obj->carried_by);
    return TRUE;
  }

  /***
* This has no specific code right now but I may give it some
* emotes and stuff later on but for now it is just here to
* tell the new fight code when someone is an archer.
*/
  bool spec_archer(CHAR_DATA *ch, OBJ_DATA *obj) { return TRUE; }

  /***
* The following items control the way mobiles use weaves on players.
* They of course intercorolate with the range to the target.
*/
  bool spec_cast_attack_weave(CHAR_DATA *ch, OBJ_DATA *obj) {
    const char *spell;

    if (ch->fighting == NULL)
    return FALSE;

    /**
* Now we'll scan through two different lists of weaves, mobs
* are limited to weaves at or below their levels and they can
* only hit specific ranges just like anyone else.
*/
    spell = "fireblast";
    if (ch->level >= 10)
    spell = "flamestrike";
    if (ch->level >= 30)
    spell = "spark shower";
    if (ch->level >= 55)
    spell = "fireball";
    if (ch->level >= 80 && number_percent() < 60)
    spell = "crush";

    return TRUE;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

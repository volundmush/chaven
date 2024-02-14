#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif

#if !defined(WIN32)
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dlfcn.h>
#else
#include <winsock.h>
#include <process.h>
#include <io.h>
#endif

#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "gsn.h"
#include "olc.h" // Discordance

#include <math.h>


#if defined(__cplusplus)
extern "C" {
#endif

  /*
  * Local functions.
  */
  bool remove_obj args((CHAR_DATA * ch, int iWear, bool fReplace));
  void exec_copyover args((CHAR_DATA * ch));

  /*
  * More countdown items
  */
  bool isreboot = TRUE;
  int pulse_muddeath = -1;
  int pulse_copyover = -1;
  CHAR_DATA *mudkiller = NULL;

  void check_muddeath args((void));
  void check_copyover args((void));
  char *fix_string args((const char *str));
  bool is_name_builder args((char *str, char *namelist));

  /*******************************************************************
  * Command      - Wiznet                                            *
  * Code         - do_wiznet                                         *
  * Description  - Command that shows info to immortals not normally *
  *                seen. (example: logins, sites, deaths, levels)    *
  *                                                                  *
  * Idea         - Stock ROM idea.                                   *
  * Author       - Re-written and Re-done by Michael MacLeod (Kuval) *
  *******************************************************************/
  _DOFUN(do_wiznet) {
    int flag;
    int col;
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0') /* Lists all wiznet options. */
    {
      buf[0] = '\0';

      col = 0;

      send_to_char("Wiznet options available to you are:\n\n\r", ch);

      for (flag = 0; wiznet_table[flag].name != NULL; flag++) {
        if (wiznet_table[flag].level <= get_trust(ch)) {
          sprintf(buf, "[%-9s]", capitalize(wiznet_table[flag].name));
          send_to_char(buf, ch);

          if (IS_SET(ch->wiznet, wiznet_table[flag].flag))
          send_to_char(" - (`#On`x )   ", ch);
          else
          send_to_char(" - (`BOff`x)   ", ch);

          if (++col % 3 == 0)
          send_to_char("\n\r", ch);
        }
      }

      if (col % 3 != 0)
      send_to_char("\n\r", ch);

      return;
    }

    if (!str_prefix(argument, "on")) {
      send_to_char("Welcome to Wiznet!\n\r", ch);
      SET_BIT(ch->wiznet, WIZ_ON);
      return;
    }

    if (!str_prefix(argument, "off")) {
      send_to_char("Signing off of Wiznet.\n\r", ch);
      REMOVE_BIT(ch->wiznet, WIZ_ON);
      return;
    }

    flag = wiznet_lookup(argument);

    if (flag == -1 || get_trust(ch) < wiznet_table[flag].level) {
      send_to_char("No such option.\n\r", ch);
      return;
    }

    if (IS_SET(ch->wiznet, wiznet_table[flag].flag)) {
      sprintf(buf, "You will no longer see %s on wiznet.\n\r", wiznet_table[flag].name);
      send_to_char(buf, ch);
      REMOVE_BIT(ch->wiznet, wiznet_table[flag].flag);
      return;
    }
    else {
      sprintf(buf, "You will now see %s on wiznet.\n\r", wiznet_table[flag].name);
      send_to_char(buf, ch);
      SET_BIT(ch->wiznet, wiznet_table[flag].flag);
      return;
    }
  }

  void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj, long flag, long flag_skip, int min_level) {
    DESCRIPTOR_DATA *d;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      d = *it;
      if (d->connected == CON_PLAYING && IS_IMMORTAL(d->character) && IS_SET(d->character->wiznet, WIZ_ON) && (!flag || IS_SET(d->character->wiznet, flag)) && (!flag_skip || !IS_SET(d->character->wiznet, flag_skip)) && get_trust(d->character) >= min_level && d->character != ch) {
        if (IS_SET(d->character->wiznet, WIZ_PREFIX))
        send_to_char("`Z-->`x ", d->character);
        act_new(string, d->character, obj, ch, TO_CHAR, POS_DEAD);
        send_to_char("`x", d->character);
      }
    }

    return;
  }

  void wiznet_command(char *string, CHAR_DATA *ch, OBJ_DATA *obj, long flag, long flag_skip, int min_level) {
    DESCRIPTOR_DATA *d;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      d = *it;
      if (d->connected == CON_PLAYING && IS_IMMORTAL(d->character) && IS_SET(d->character->wiznet, WIZ_ON) && (!flag || IS_SET(d->character->wiznet, flag)) && (!flag_skip || !IS_SET(d->character->wiznet, flag_skip)) && get_trust(d->character) >= min_level && d->character != ch) {
        if (IS_SET(d->character->wiznet, WIZ_PREFIX))
        send_to_char("`Z-->`x ", d->character);
        printf_to_char(d->character, "%s\n\r", string);
        send_to_char("`x", d->character);
      }
    }

    return;
  }

  _DOFUN(do_talent) {
    CHAR_DATA *victim;

    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
      send_to_char("Do what to who?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    return;
  }

  _DOFUN(do_unroster) {
    remove_from_roster(argument);
    send_to_char("Done.\n\r", ch);
    return;
  }

  _DOFUN(do_makeprivate) {
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Do what to who?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    
    if (!IS_FLAG(victim->comm, COMM_PRIVATE))
    SET_FLAG(victim->comm, COMM_PRIVATE);
    send_to_char("Done.\n\r", ch);

    return;
  }

  /* equips a character */
  _DOFUN(do_newbieoutfit) {

    OBJ_DATA *obj;

    if ((obj = get_eq_char(ch, WEAR_BODY_1)) == NULL) {
      obj = create_object(get_obj_index(50), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char_silent(ch, obj, WEAR_BODY_1);
    }

    if (ch->disciplines[DIS_PISTOLS] > 0) {
      obj = create_object(get_obj_index(17), 0);
      obj->cost = 0;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("worn black automatic handgun");
      free_string(obj->description);
      obj->description = str_dup("A worn black automatic handgun");
      free_string(obj->name);
      obj->name = str_dup("gun handgun black worn automatic");
      obj_to_char(obj, ch);
    }

    if (ch->disciplines[DIS_BOWS] > 0) {
      obj = create_object(get_obj_index(17), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("simple longbow");
      free_string(obj->description);
      obj->description = str_dup("A simple longbow");
      free_string(obj->name);
      obj->name = str_dup("simple longbow bow");
      obj_to_char(obj, ch);
    }
    if (ch->disciplines[DIS_SHOTGUNS] > 0) {
      obj = create_object(get_obj_index(17), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("worn black pump-action shotgun");
      free_string(obj->description);
      obj->description = str_dup("A worn black pump-action shotgun");
      free_string(obj->name);
      obj->name = str_dup("shotgun black worm pump");
      obj_to_char(obj, ch);
    }
    if (ch->disciplines[DIS_RIFLES] > 0) {
      obj = create_object(get_obj_index(17), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("worn brown bolt-action rifle");
      free_string(obj->description);
      obj->description = str_dup("A worn brown bolt-action rifle");
      free_string(obj->name);
      obj->name = str_dup("worn brown bolt action rifle gun");
      obj_to_char(obj, ch);
    }
    if (ch->disciplines[DIS_CARBINES] > 0) {
      obj = create_object(get_obj_index(17), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("AK-47 carbine");
      free_string(obj->description);
      obj->description = str_dup("An AK-47 carbine");
      free_string(obj->name);
      obj->name = str_dup("ak-47 carbine rifle gun");
      obj_to_char(obj, ch);
    }

    if (ch->disciplines[DIS_LONGBLADE] > 0) {
      obj = create_object(get_obj_index(16), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("rusty steel shortsword");
      free_string(obj->description);
      obj->description = str_dup("A rusty steel shortsword");
      free_string(obj->name);
      obj->name = str_dup("rusty steel shortsword sword");
      obj_to_char(obj, ch);
    }
    if (ch->disciplines[DIS_BLUNT] > 0) {
      obj = create_object(get_obj_index(16), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("rusy steel warhammer");
      free_string(obj->description);
      obj->description = str_dup("A rusty steel warhammer");
      free_string(obj->name);
      obj->name = str_dup("rusty steel warhammer hammer");
      obj_to_char(obj, ch);
    }
    if (ch->disciplines[DIS_KNIFE] > 0) {
      obj = create_object(get_obj_index(16), 0);
      obj->cost = 0;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("rusty steel knife");
      free_string(obj->description);
      obj->description = str_dup("A rusty steel knife");
      free_string(obj->name);
      obj->name = str_dup("rusty steel knife");
      obj_to_char(obj, ch);
    }
    if (ch->disciplines[DIS_BARMOR] > 0) {
      obj = create_object(get_obj_index(19), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("worn kevlar vest");
      free_string(obj->description);
      obj->description = str_dup("A worn kevlar vest");
      free_string(obj->name);
      obj->name = str_dup("worn kevlar vest");
      obj_to_char(obj, ch);
      equip_char_silent(ch, obj, WEAR_BODY_6);
    }
    else if (ch->disciplines[DIS_MARMOR] > 0) {
      obj = create_object(get_obj_index(19), 0);
      obj->cost = 0;
      obj->size = 30;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("battered steel breastplate");
      free_string(obj->description);
      obj->description = str_dup("A battered steel breastplate");
      free_string(obj->name);
      obj->name = str_dup("battered steel breastplate");
      obj_to_char(obj, ch);
      equip_char_silent(ch, obj, WEAR_BODY_6);
    }

    obj = create_object(get_obj_index(55), 0);
    obj->cost = 0;
    if (time_info.phone < 7000000)
    time_info.phone = 7000000;
    time_info.phone += number_range(1, 19);
    obj->value[0] = time_info.phone;
    obj_to_char(obj, ch);
  }

  void nochan_person(CHAR_DATA *ch, CHAR_DATA *victim, int penalty) {
    if (IS_FLAG(victim->comm, COMM_NOCHANNELS)) {
      REMOVE_FLAG(victim->comm, COMM_NOCHANNELS);
      send_to_char("The gods have restored your channel privileges.\n\r", victim);
      victim->pcdata->penalty = 0;
      send_to_char("You restored their channel privileges.\n\r", ch);
    }
    else {
      SET_FLAG(victim->comm, COMM_NOCHANNELS);
      if (penalty > 0) {
        printf_to_char(
        victim, "Your channels have been revoked for the next %d minutes.\n\r", penalty);
        victim->pcdata->penalty = penalty * 12;
        printf_to_char(ch, "You nochanneled %s for %d minutes.\n\r", victim->name, penalty);
      }
      else {
        send_to_char("The gods have revoked your channel privileges.\n\r", victim);
        victim->pcdata->penalty = -1;
        printf_to_char(ch, "NOCHANNELS set.\n\r", penalty);
      }
    }
  }

  void nochanloop(CHAR_DATA *ch, char *name, int penalty) {
    CHAR_DATA *target;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;

      if ((d->connected != CON_PLAYING && d->connected != CON_CREATION))
      continue;

      target = CH(d);

      if (target->pcdata->account != NULL && str_cmp(name, target->pcdata->account->name))
      continue;

      if (safe_strlen(target->pcdata->account_name) > 2 && str_cmp(name, target->pcdata->account_name))
      continue;

      nochan_person(ch, target, penalty);
    }
  }

  _DOFUN(do_freepower) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nochannel whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't nochannel mobs, silly!\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_FREEPOWER))
    REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_FREEPOWER);
    else
    SET_FLAG(victim->pcdata->account->flags, ACCOUNT_FREEPOWER);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_removenightmare) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nochannel whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't nochannel mobs, silly!\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NIGHTMARE))
    REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NIGHTMARE);
    else
    SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NIGHTMARE);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_nokill) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nochannel whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't nochannel mobs, silly!\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOKILL))
    REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOKILL);
    else
    SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOKILL);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_shh) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nochannel whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't nochannel mobs, silly!\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHH))
    REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_SHH);
    else
    SET_FLAG(victim->pcdata->account->flags, ACCOUNT_SHH);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_shadowban) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nochannel whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't nochannel mobs, silly!\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN);
    else
    SET_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_nochannels) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MSL];

    int penalty;

    argument = one_argument(argument, arg);
    penalty = atoi(argument);

    if (penalty <= 0)
    penalty = -1;

    if (arg[0] == '\0') {
      send_to_char("Nochannel whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      nochanloop(ch, arg, penalty);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't nochannel mobs, silly!\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOCHANNELS)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOCHANNELS);
      send_to_char("Your channels have been restored.\n\r", victim);
      send_to_char("You restored their channel privileges.\n\r", ch);
      sprintf(buf, "$N restores channels to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
      victim->pcdata->penalty = 0;
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOCHANNELS);
      if (penalty > 0) {
        printf_to_char(
        victim, "Your channels have been revoked for the next %d minutes.\n\r", penalty);
        printf_to_char(ch, "You nochanneled %s for %d minutes.\n\r", victim->name, penalty);
        sprintf(buf, "$N revokes %s's channels for %d minutes.", victim->name, penalty);
        wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
        victim->pcdata->penalty = penalty * 12;
      }
      else {
        send_to_char("Your channels have been revoked indefinately.\n\r", victim);
        printf_to_char(ch, "NOCHANNELS set.\n\r", penalty);
        sprintf(buf, "$N revokes %s's channels indefinately.", victim->name);
        wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
        victim->pcdata->penalty = -1;
      }
    }

    return;
  }

  _DOFUN(do_notitle) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MSL];
    bool online = FALSE;
    DESCRIPTOR_DATA d;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Notitle whom?\n\r", ch);
      return;
    }

    d.original = NULL;
    if ((victim = get_char_world(ch, arg)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if ((victim = get_char_world_pc(arg)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: expel");

        if (!load_char_obj(&d, arg)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg));
          return;
        }
        victim = d.character;
      }
    }

    if (get_trust(victim) >= get_trust(ch)) {
      if (!online)
      free_char(victim);

      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      send_to_char("You can't notitle mobs.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTITLE)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTITLE);

      if (!online)
      free_char(victim);
      else
      send_to_char("Your ability to set a title has been restored.\n\r", victim);
      send_to_char("You restored their title privileges.\n\r", ch);
      sprintf(buf, "$N restores %s's titles.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTITLE);

      if (!online)
      free_char(victim);
      else
      send_to_char("Your ability to set a title has been revoked.\n\r", victim);
      send_to_char("NOTITLE set.\n\r", ch);
      sprintf(buf, "$N revokes %s's titles indefinately.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_nointro) {
    char arg[MAX_INPUT_LENGTH];
    char buf[MSL];
    CHAR_DATA *victim;
    bool online = FALSE;
    DESCRIPTOR_DATA d;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nointro whom?\n\r", ch);
      return;
    }

    d.original = NULL;
    if ((victim = get_char_world(ch, arg)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if ((victim = get_char_world_pc(arg)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: expel");

        if (!load_char_obj(&d, arg)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg));
          return;
        }
        victim = d.character;
      }
    }

    if (get_trust(victim) >= get_trust(ch)) {
      if (!online)
      free_char(victim);

      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      send_to_char("You can't nointro mobs.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOINTRO)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOINTRO);

      if (!online)
      free_char(victim);
      else
      send_to_char("Your ability to set an intro has been restored.\n\r", victim);
      send_to_char("You restored their intro privileges.\n\r", ch);
      sprintf(buf, "$N restores %s's intros.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOINTRO);

      if (!online)
      free_char(victim);
      else
      send_to_char("Your ability to set an intro has been revoked.\n\r", victim);
      send_to_char("NOINTRO set.\n\r", ch);
      sprintf(buf, "$N revokes %s's intros indefinately.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  /**************************************************************
  * Name: do_nonote
  * Parameters:
  *	CHAR_DATA *ch - character executing command
  *	char *argument - name of char to nonote
  * Returns:
  *	void
  *
  * Purpose: This prevents a person from writing any notes.
  * Author: Edward Laurin (Palin)
  **************************************************************/
  // I spruced this up with offline targetting and stuff - Discordance
  _DOFUN(do_nonote) {
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    char arg1[MSL];
    char buf[MSL];

    if (argument[0] == '\0') {
      send_to_char("Nonote whom?\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    d.original = NULL;
    if ((victim = get_char_world(ch, arg1)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if ((victim = get_char_world_pc(arg1)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: nonote");

        if (!load_char_obj(&d, arg1)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
          return;
        }
        victim = d.character;
      }
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      send_to_char("\n\rYou can't target mobiles!\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      if (!online)
      free_char(victim);

      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NONOTE)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NONOTE);
      if (online)
      send_to_char("Staff has restored your note writing privileges.\n\r", victim);
      if (!online)
      free_char(victim);

      send_to_char("You restore their note writing privileges.\n\r", ch);
      sprintf(buf, "$N restores %s's notes.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NONOTE);
      if (online)
      send_to_char("Staff has revoked your note writing privileges.\n\r", victim);
      if (!online)
      free_char(victim);

      send_to_char("NONOTE Set.\n\r", ch);
      sprintf(buf, "$N revokes %s's notes indefinately.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_bamfin) {
    char buf[MAX_STRING_LENGTH];

    if (!IS_NPC(ch)) {
      smash_tilde(argument);

      if (argument[0] == '\0') {
        sprintf(buf, "Your poofin is %s\n\r", ch->pcdata->bamfin);
        send_to_char(buf, ch);
        return;
      }

      /*	if ( strstr(argument,ch->name) == NULL)
      {
        send_to_char("You must include your name.\n\r",ch);
        return;
      }
      */
      free_string(ch->pcdata->bamfin);
      ch->pcdata->bamfin = str_dup(argument);

      sprintf(buf, "Your poofin is now %s\n\r", ch->pcdata->bamfin);
      send_to_char(buf, ch);
    }
    return;
  }

  _DOFUN(do_bamfout) {
    char buf[MAX_STRING_LENGTH];

    if (!IS_NPC(ch)) {
      smash_tilde(argument);

      if (argument[0] == '\0') {
        sprintf(buf, "Your poofout is %s\n\r", ch->pcdata->bamfout);
        send_to_char(buf, ch);
        return;
      }
      /*
      if ( strstr(argument,ch->name) == NULL)
      {
        send_to_char("You must include your name.\n\r",ch);
        return;
      }
      */
      free_string(ch->pcdata->bamfout);
      ch->pcdata->bamfout = str_dup(argument);

      sprintf(buf, "Your poofout is now %s\n\r", ch->pcdata->bamfout);
      send_to_char(buf, ch);
    }
    return;
  }

  _DOFUN(do_deny) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Deny whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (!IS_FLAG(victim->act, PLR_DENY))
    SET_FLAG(victim->act, PLR_DENY);
    else
    REMOVE_FLAG(victim->act, PLR_DENY);

    send_to_char("You are denied access!\n\r", victim);
    sprintf(buf, "$N denies access to %s", victim->name);
    wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    send_to_char("OK.\n\r", ch);
    save_char_obj(victim, TRUE, FALSE);
    real_quit(victim);

    return;
  }

  _DOFUN(do_disconnect) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Disconnect whom?\n\r", ch);
      return;
    }

    if (is_number(arg)) {
      int desc;

      desc = atoi(arg);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        if ((*it)->descriptor == desc) {
          (*it)->connected = CON_QUITTING;
          // close_desc( *it );
          send_to_char("Ok.\n\r", ch);
          return;
        }
      }
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->desc == NULL) {
      act("$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR);
      return;
    }

    if (victim->level > ch->level) {
      send_to_char("Please don't do that\n\r", ch);
      return;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      if (*it == victim->desc) {
        (*it)->connected = CON_QUITTING;
        //   close_desc( *it );
        send_to_char("Ok.\n\r", ch);
        return;
      }
    }

    bug("Do_disconnect: desc not found.", 0);
    send_to_char("Descriptor not found!\n\r", ch);
    return;
  }

  _DOFUN(do_echo) {
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
      send_to_char("Global echo what?\n\r", ch);
      return;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      d = *it;
      if (d->connected == CON_PLAYING) {
        if (get_trust(d->character) >= get_trust(ch))
        send_to_char("global> ", d->character);
        send_to_char(argument, d->character);
        send_to_char("\n\r", d->character);
      }
    }

    return;
  }

  _DOFUN(do_recho) {
    if (argument[0] == '\0') {
      send_to_char("Local echo what?\n\r", ch);
      return;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected == CON_PLAYING && d->character->in_room == ch->in_room) {
        if (get_trust(d->character) >= get_trust(ch))
        send_to_char("local> ", d->character);
        send_to_char(argument, d->character);
        send_to_char("\n\r", d->character);
      }
    }

    return;
  }

  _DOFUN(do_zecho) {
    if (argument[0] == '\0') {
      send_to_char("Zone echo what?\n\r", ch);
      return;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected == CON_PLAYING && d->character->in_room != NULL && ch->in_room != NULL && d->character->in_room->area == ch->in_room->area) {
        if (get_trust(d->character) >= get_trust(ch))
        send_to_char("zone> ", d->character);
        send_to_char(argument, d->character);
        send_to_char("\n\r", d->character);
      }
    }
  }

  _DOFUN(do_pecho) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if (argument[0] == '\0' || arg[0] == '\0') {
      send_to_char("Personal echo what?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("Target not found.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL)
    send_to_char("personal> ", victim);

    send_to_char(argument, victim);
    send_to_char("\n\r", victim);
    send_to_char("personal> ", ch);
    send_to_char(argument, ch);
    send_to_char("\n\r", ch);
  }

  ROOM_INDEX_DATA *find_location(CHAR_DATA *ch, char *arg, int *mappos, int *x, int *y) {
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg1[MIL], arg2[MIL], arg3[MIL];

    arg = one_argument(arg, arg1);
    arg = one_argument(arg, arg2);
    one_argument(arg, arg3);

    if (is_number(arg1) && arg2[0] == '\0')
    return get_room_index(atoi(arg1));

    if ((victim = get_char_world(ch, arg1)) != NULL) {
      if (story_on(victim) || IS_NPC(victim))
      return victim->in_room;
    }

    if ((obj = get_obj_world(ch, arg1)) != NULL) {

      return obj->in_room;
    }

    return NULL;
  }

  _DOFUN(do_at) {
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    int map = -1, x = -1, y = -1;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("At where what?\n\r", ch);
      return;
    }

    if ((location = find_location(ch, arg, &map, &x, &y)) == NULL) {
      send_to_char("No such location.\n\r", ch);
      return;
    }

    original = ch->in_room;

    on = ch->on;
    char_from_room(ch);
    char_to_room(ch, location);
    interpret(ch, argument);

    /*
    * See if 'ch' still exists before continuing!
    * Handles 'at XXXX quit' case.
    */
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      if (*it == ch) {
        char_from_room(ch);
        char_to_room(ch, original);
        ch->on = on;
        break;
      }
    }

    return;
  }

  _DOFUN(do_goto) {
    ROOM_INDEX_DATA *location = NULL;
    int count = 0;
    int map = -1, x = -1, y = -1;

    if (argument[0] == '\0') {
      send_to_char("Goto where?\n\r", ch);
      return;
    }

    if ((location = find_location(ch, argument, &map, &x, &y)) == NULL) {
      send_to_char("No such location.\n\r", ch);
      return;
    }

    if (location == NULL) {
      send_to_char("No such location.\n\r", ch);
      return;
    }

    count = 0;
    for (CharList::iterator it = location->people->begin();
    it != location->people->end(); ++it)
    count++;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      if (get_trust(*it) >= ch->invis_level) {
        if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
        act("$t", ch, ch->pcdata->bamfout, *it, TO_VICT);
        else
        act("$n leaves in a swirling mist.", ch, NULL, *it, TO_VICT);
      }
    }

    char_from_room(ch);
    char_to_room(ch, location);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      if (get_trust(*it) >= ch->invis_level) {
        if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
        act("$t", ch, ch->pcdata->bamfin, *it, TO_VICT);
        else
        act("$n appears in a swirling mist.", ch, NULL, *it, TO_VICT);
      }
    }

    do_function(ch, &do_look, "auto");
    return;
  }

  _DOFUN(do_violate) {
    ROOM_INDEX_DATA *location;
    int map = -1, x = -1, y = -1;

    if (argument[0] == '\0') {
      send_to_char("Goto where?\n\r", ch);
      return;
    }

    if ((location = find_location(ch, argument, &map, &x, &y)) == NULL) {
      send_to_char("No such location.\n\r", ch);
      return;
    }

    if (!room_is_private(location)) {
      send_to_char("That room isn't private, use goto.\n\r", ch);
      return;
    }

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      if (get_trust(*it) >= ch->invis_level) {
        if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
        act("$t", ch, ch->pcdata->bamfout, *it, TO_VICT);
        else
        act("$n leaves in a swirling mist.", ch, NULL, *it, TO_VICT);
      }
    }

    char_from_room(ch);
    char_to_room(ch, location);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      if (get_trust(*it) >= ch->invis_level) {
        if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
        act("$t", ch, ch->pcdata->bamfin, *it, TO_VICT);
        else
        act("$n appears in a swirling mist.", ch, NULL, *it, TO_VICT);
      }
    }

    do_function(ch, &do_look, "auto");
    return;
  }

  _DOFUN(do_stat) {
    char arg[MAX_INPUT_LENGTH];
    char *string;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;
    int map = -1, x = -1, y = -1;

    string = one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  stat char <name>\n\r", ch);
      send_to_char("  stat obj  <name>\n\r", ch);
      send_to_char("  stat mob  <name>\n\r", ch);
      send_to_char("  stat room <number>\n\r", ch);
      send_to_char("  stat stash <name>\n\r", ch);
      send_to_char("  stat stats <name>\n\r", ch);
      send_to_char("  stat disciplines <name>\n\r", ch);
      send_to_char("  stat attacks <name>\n\r", ch);
      send_to_char("  stat garage <name>\n\r", ch);

      return;
    }

    if (!str_cmp(arg, "room")) {
      do_function(ch, &do_rstat, string);
      return;
    }

    if (!str_cmp(arg, "obj")) {
      do_function(ch, &do_ostat, string);
      return;
    }

    /* Added in order to seperate mstat and pstat (Char Stat) - Kuval */
    if (!str_cmp(arg, "char")) {
      do_function(ch, &do_score, string);
      return;
    }

    if (!str_cmp(arg, "secrecy")) {
      printf_to_char(ch, "Secrecy: %d\n\r", char_secrecy(get_char_world_pc(string), ch));
      return;
    }
    if (!str_cmp(arg, "stash")) {
      do_function(ch, &do_stashstat, string);
      return;
    }
    if (!str_cmp(arg, "lifeforce")) {
      lfstat(get_char_world_pc(string), ch);
      return;
    }
    if (!str_cmp(arg, "disciplines")) {
      do_function(ch, &do_discstat, string);
      return;
    }
    if (!str_cmp(arg, "garage")) {
      do_function(ch, &do_garagestat, string);
      return;
    }
    if (!str_cmp(arg, "emotes")) {
      do_function(ch, &do_emotestat, string);
      return;
    }
    if (!str_cmp(arg, "forms")) {
      do_function(ch, &do_formstat, string);
      return;
    }
    if (!str_cmp(arg, "stats")) {
      do_function(ch, &do_statstat, string);
      return;
    }
    if (!str_cmp(arg, "attacks")) {
      do_function(ch, &do_attackstat, string);
      return;
    }

    if (!str_cmp(arg, "mob")) {
      do_function(ch, &do_mstat, string);
      return;
    }
    if (!str_cmp(arg, "patrols")) {
      int patrolling = 0;
      int hunting = 0;
      int warfare = 0;
      int arcane = 0;
      int predator = 0;
      int reckless = 0;
      int diplomatic = 0;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL && !is_gm(d->character)) {
          victim = d->character;

          if (victim->pcdata->patrol_status == PATROL_PATROL)
          patrolling++;
          if (victim->pcdata->patrol_habits[PATROL_HUNTHABIT] > 0)
          hunting++;
          if (victim->pcdata->patrol_habits[PATROL_WARHABIT] > 0)
          warfare++;
          if (victim->pcdata->patrol_habits[PATROL_ARCANEHABIT] > 0)
          arcane++;
          if (victim->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] > 0)
          diplomatic++;
          if (victim->pcdata->patrol_habits[PATROL_RECKLESSHABIT] > 0)
          reckless++;
          if (victim->pcdata->patrol_habits[PATROL_PREDATORHABIT] > 0)
          predator++;
        }
      }

      printf_to_char(ch, "Patrolling: %d, Hunters: %d, Warriors: %d, Arcanists: %d, Diplomats: %d, Predators: %d, Reckless: %d\n\r", patrolling, hunting, warfare, arcane, diplomatic, predator, reckless);
      return;
    }
    /* do it the old way */
    // switched order of ostat and mstat.  Items named after characters are annoying - Discordance

    victim = get_char_anywhere(ch, arg);
    if (victim != NULL) {
      do_function(ch, &do_mstat, argument);
      return;
    }

    obj = get_obj_world(ch, argument);
    if (obj != NULL) {
      do_function(ch, &do_ostat, argument);
      return;
    }

    location = find_location(ch, argument, &map, &x, &y);
    if (location != NULL) {
      do_function(ch, &do_rstat, argument);
      return;
    }

    send_to_char("Nothing by that name found anywhere.\n\r", ch);
  }

  void skillloop_immortal(CHAR_DATA *ch, int type, CHAR_DATA *victim) {
    int i;
    int wrap = 1;

    for (i = 0; i < SKILL_USED; i++) {
      if (skilltype(skill_table[i].vnum) != type)
      continue;
      if (victim->skills[skill_table[i].vnum] == 0)
      continue;

      if (wrap < 3) {
        wrap++;
        printf_to_char(ch, "%24.24s  (%d) \t", skill_table[i].name, victim->skills[skill_table[i].vnum]);
      }
      else {
        wrap = 1;
        printf_to_char(ch, "%24.24s   (%d)\n\r", skill_table[i].name, victim->skills[skill_table[i].vnum]);
      }
    }
    if (wrap == 1)
    send_to_char("\n\r", ch);
    else
    send_to_char("\n\n\r", ch);
  }

  _DOFUN(do_attackstat) {
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    for (int i = 0; i < 25; i++) {
      if (safe_strlen(victim->pcdata->customstrings[i][0]) > 1) {
        printf_to_char(ch, "[%s]\t\t %s \t (%s)\n\r", victim->pcdata->customstrings[i][0], get_disc_string(victim->pcdata->customstats[i][1]), get_special_string(victim->pcdata->customstats[i][0]));
      }
    }
  }

  _DOFUN(do_statstat) {
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (skillcount(victim, STYPE_ORIGIN) > 0) {
      send_to_char("`W                                                    Origin`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_ORIGIN, victim);
    }
    if (skillcount(victim, STYPE_COMBATFOCUS) > 0) {
      send_to_char("`W                                              Supernatural`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_COMBATFOCUS, victim);
    }
    if (skillcount(victim, STYPE_ARCANEFOCUS) > 0) {
      send_to_char("`W                                              Supernatural`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_ARCANEFOCUS, victim);
    }
    if (skillcount(victim, STYPE_PROFFOCUS) > 0) {
      send_to_char("`W                                              Supernatural`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_PROFFOCUS, victim);
    }
    if (skillcount(victim, STYPE_SUPERNATURAL) > 0) {
      send_to_char("`W                                              Supernatural`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_SUPERNATURAL, victim);
    }
    if (skillcount(victim, STYPE_SOCIAL) > 0) {
      send_to_char("`W                                                 Social`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_SOCIAL, victim);
    }
    if (skillcount(victim, STYPE_PHYSICAL) > 0) {
      send_to_char("`W                                                Physical`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_PHYSICAL, victim);
    }
    if (skillcount(victim, STYPE_INTELLECTUAL) > 0) {
      send_to_char("`W                                               Intellectual`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_INTELLECTUAL, victim);
    }
    if (skillcount(victim, STYPE_CONTACTS) > 0) {
      send_to_char("`W                                                 Contacts`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_CONTACTS, victim);
    }
    if (skillcount(victim, STYPE_SKILLS) > 0) {
      send_to_char("`W                                                 Skills`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_SKILLS, victim);
    }
    if (skillcount(victim, STYPE_ABILITIES) > 0) {
      send_to_char("`W                                                 Abilities`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_ABILITIES, victim);
    }
    if (skillcount(victim, STYPE_SABILITIES) > 0) {
      send_to_char("`W                                       Supernatural Abilities`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_SABILITIES, victim);
    }
    if (skillcount(victim, STYPE_ABOMINATION) > 0 && ch->level > 109) {
      send_to_char("`W                                                 Abomination`x\n\n\r", ch);
      skillloop_immortal(ch, STYPE_ABOMINATION, victim);
    }
  }

  _DOFUN(do_discstat) {
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);
    int i;
    int wrap = 1;
    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    send_to_char("`RD`ri`Rs`rc`Ri`rp`Rl`ri`Rn`re`Rs`x\n\r", ch);

    send_to_char("\n  `RRanged`x\n\r", ch);

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range < 1)
      continue;
      if (victim->disciplines[discipline_table[i].vnum] <= 0)
      continue;
      if (wrap < 3) {
        wrap++;
        printf_to_char(ch, "%-18.18s   (%3d)\t\t", discipline_table[i].name, victim->disciplines[discipline_table[i].vnum]);
      }
      else {
        wrap = 1;
        printf_to_char(ch, "%-18.18s   (%3d)\n\r", discipline_table[i].name, victim->disciplines[discipline_table[i].vnum]);
      }
    }
    send_to_char("\n  `RClose Combat`x\n\r", ch);

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != 0)
      continue;
      if (victim->disciplines[discipline_table[i].vnum] <= 0)
      continue;

      if (wrap < 3) {
        wrap++;
        printf_to_char(ch, "%-18.18s   (%3d)\t\t", discipline_table[i].name, victim->disciplines[discipline_table[i].vnum]);
      }
      else {
        wrap = 1;
        printf_to_char(ch, "%-18.18s   (%3d)\n\r", discipline_table[i].name, victim->disciplines[discipline_table[i].vnum]);
      }
    }

    send_to_char("\n  `RDefensive`x\n\r", ch);

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      if (victim->disciplines[discipline_table[i].vnum] <= 0)
      continue;

      if (wrap < 3) {
        wrap++;
        printf_to_char(ch, "%-18.18s   (%3d)\t\t", discipline_table[i].name, victim->disciplines[discipline_table[i].vnum]);
      }
      else {
        wrap = 1;
        printf_to_char(ch, "%-18.18s   (%3d)\n\r", discipline_table[i].name, victim->disciplines[discipline_table[i].vnum]);
      }
    }
  }

  _DOFUN(do_garagestat) {
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    for (int i = 0; i < 10; i++) {
      if (victim->pcdata->garage_cost[i] > 0) {
        printf_to_char(
        ch, "`c%d`W)`x %s\n`cCost:`x `g$%d`x \t`cType:`x %s %s \t`cStatus:`x %s\n%s\n`W|`R%s`W|`x\n\n\r", i + 1, victim->pcdata->garage_name[i], victim->pcdata->garage_cost[i], carqualityname(victim->pcdata->garage_cost[i], victim->pcdata->garage_typeone[i], victim->pcdata->garage_typetwo[i]), cartypename(victim->pcdata->garage_typeone[i], victim->pcdata->garage_typetwo[i]), carstatusname(victim->pcdata->garage_status[i]), victim->pcdata->garage_desc[i], victim->pcdata->garage_lplate[i]);
      }
    }
  }

  _DOFUN(do_emotestat) {
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    printf_to_char(
    ch, "Emotes Total %d\nCombat %d, Victim %d\nPublic %d, Sex %d\nSocial %d, Antag %d\nPact Antag %d, Dreaming %d\n\r", victim->pcdata->emotes[EMOTE_TOTAL], victim->pcdata->emotes[EMOTE_COMBAT], victim->pcdata->emotes[EMOTE_VICTIM], victim->pcdata->emotes[EMOTE_PUBLIC], victim->pcdata->emotes[EMOTE_SEX], victim->pcdata->emotes[EMOTE_SOCIAL], victim->pcdata->emotes[EMOTE_ANTAG], victim->pcdata->emotes[EMOTE_PACTANTAG], victim->pcdata->emotes[EMOTE_DREAMING]);
  }

  _DOFUN(do_formstat) {
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (is_werewolf(victim))
    view_animal_to(victim, ANIMAL_WOLF, ch);
    for (int i = 0; i < 6; i++) {
      if (safe_strlen(victim->pcdata->animal_names[i]) > 2)
      view_animal_to(victim, i, ch);
    }
  }

  _DOFUN(do_stashstat) {
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *room;
    char buf[MSL];
    CHAR_DATA *victim;
    char arg[MSL];
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    send_to_char("\n`cStash Contents:`x\n\n\r", ch);

    for (obj = victim->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (obj->stash_room == 0)
        sprintf(buf, "%s `W(`xhome`W)`x", obj->short_descr);
        else if ((room = get_room_index(obj->stash_room)) != NULL)
        sprintf(buf, "%s `W(`x%s`W)`x", obj->short_descr, room->name);
        else
        sprintf(buf, "%s `W(`xSomewhere`W)`x", obj->short_descr);
        act(buf, ch, NULL, NULL, TO_CHAR);
      }
    }
  }
  _DOFUN(do_pstat) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];

    char tempbuf[MSL];

    char *const month_names[] = {"January",   "Febuary", "March",    "April", "May",       "June",    "July",     "August", "September", "October", "November", "December"};

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("They aren't a PC.\n\r", ch);
      return;
    }
    if (!story_on(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    /* The upper line of the score sheet. */
    outbuf.strcat("`B####################################################################`x\n\r");

    sprintf(tempbuf, " `WName :`x %10s %-12s  `WAge:`x%3d      `WGender:`x %-6s", victim->name, victim->pcdata->last_name, get_true_age(victim), (victim->sex == SEX_MALE) ? "Male" : "Female");

    sprintf(buf, "`B#`x %-77.77s`B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, " `WArchtype :`x %s`x              `WFaction:`x %-10s", race_table[victim->race].who_name, get_fac(victim));

    sprintf(buf, "`B#`x %-79.79s`B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "           You were born on the %d of %s, %d.", victim->pcdata->birth_day, month_names[victim->pcdata->birth_month - 1], victim->pcdata->birth_year);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    if (is_vampire(victim)) {
      sprintf(tempbuf, "             And died on the %d of %s, %d.", victim->pcdata->sire_day, month_names[victim->pcdata->sire_month - 1], victim->pcdata->sire_year);

      sprintf(buf, "`B#`x %-64s `B#`x\n\r", tempbuf);
      outbuf.strcat(buf);
    }
    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "`W   Wounds :`x %-27.27s`x `WFatigue   : `x%-3d", get_inj(victim), get_energy(victim));

    sprintf(buf, "`B#`x %-70.70s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "                                        `WInfluence : `x%-4d", victim->pcdata->influence);

    sprintf(buf, "`B#`x %-64.64s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(
    tempbuf, "`W   Experience Cap :`x %7d`x             `WKarma     : `x%-5d`x       ", victim->pcdata->exp_cap, victim->karma);
    //    sprintf( buf, "`B#`x %-64.64s `B#`x\n\r", " " );
    //    outbuf.strcat( buf );

    sprintf(buf, "`B#`x %-68.68s         `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "`W                           `WCommunity Credit     : `x%-5d`x   ", victim->donated);

    sprintf(buf, "`B#`x %-68.68s         `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WDisciplines`W:`x%3d                      `WStats:`x%3d", total_disc(victim), total_skills(victim));

    sprintf(buf, "`B#`x %-64s           `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    double value = victim->money;
    value /= 100;
    double amount = victim->pcdata->total_money;
    amount /= 100;

    sprintf(tempbuf, "   `WCash`W: `x$%-8.2f                      `WBanked: `x$%-9.2f", value, amount);

    sprintf(buf, "`B#`x %-64s       `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WExperience`W:`x       %7d            `WRP XP`W:`x       %7d", victim->exp, victim->rpexp);

    sprintf(buf, "`B#`x %-64s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WSpent Experience`W:`x %7d            `WSpent RP XP`W:`x %7d", victim->spentexp, victim->spentrpexp);

    sprintf(buf, "`B#`x %-64s     `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WDefense`W:`x %4d/%-4d                   `WHours`W:`x %d", victim->hit, max_hp(victim), (victim->played + (int)(current_time - victim->logon)) / 3600);

    sprintf(buf, "`B#`x %-68.68s         `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(tempbuf, "`W                                        Whoinvis :`x %s`x ", (IS_FLAG(victim->comm, COMM_WHOINVIS)) ? "On" : "Off");

    sprintf(buf, "`B#`x %-58.58s           `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    sprintf(buf, "`B#`x %-64s `B#`x\n\r", " ");
    outbuf.strcat(buf);

    sprintf(tempbuf, "   `WLocation`W: `x%s", victim->in_room->area->name);
    sprintf(buf, "`B#`x %-64s       `B#`x\n\r", tempbuf);
    outbuf.strcat(buf);

    outbuf.strcat("`B####################################################################`x\n\r");

    send_to_char(outbuf, ch);
    return;
  }

  _DOFUN(do_rstat) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    int door;
    int map = -1, x = -1, y = -1;

    one_argument(argument, arg);
    location =
    (arg[0] == '\0') ? ch->in_room : find_location(ch, arg, &map, &x, &y);
    if (location == NULL) {
      send_to_char("No such location.\n\r", ch);
      return;
    }

    sprintf(buf, "Name: '%s'\n\rArea: '%s'\n\r", location->name, location->area->name);
    send_to_char(buf, ch);

    sprintf(buf, "Vnum: %d  Sector: %d  Light: %d  Level %d\n\r", location->vnum, location->sector_type, location->light, location->level);
    send_to_char(buf, ch);

    sprintf(buf, "Room flags: %d.\n\rDescription:\n\r%s", location->room_flags, location->description);
    send_to_char(buf, ch);

    if (location->extra_descr != NULL) {
      EXTRA_DESCR_DATA *ed;

      send_to_char("Extra description keywords: '", ch);
      for (ed = location->extra_descr; ed; ed = ed->next) {
        send_to_char(ed->keyword, ch);
        if (ed->next != NULL)
        send_to_char(" ", ch);
      }
      send_to_char("'.\n\r", ch);
    }

    send_to_char("Characters:", ch);
    for (CharList::iterator it = location->people->begin();
    it != location->people->end(); ++it) {
      if (can_see(ch, *it)) {
        send_to_char(" ", ch);
        one_argument((*it)->name, buf);
        send_to_char(buf, ch);
      }
    }

    send_to_char(".\n\rObjects:   ", ch);
    for (obj = location->contents; obj; obj = obj->next_content) {
      send_to_char(" ", ch);
      one_argument(obj->name, buf);
      send_to_char(buf, ch);
    }
    send_to_char(".\n\r", ch);

    for (door = 0; door < MAX_DIR; door++) {
      EXIT_DATA *pexit;

      if ((pexit = location->exit[door]) != NULL) {
        sprintf(buf, "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s", 
        door, (pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum), pexit->key, pexit->exit_info, pexit->keyword, pexit->description[0] != '\0' ? pexit->description
        : "(none).\n\r");
        send_to_char(buf, ch);
      }
    }

    return;
  }

  _DOFUN(do_ostat) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Stat what?\n\r", ch);
      return;
    }

    if ((obj = get_obj_world(ch, argument)) == NULL) {
      send_to_char("Nothing like that in hell, earth, or heaven.\n\r", ch);
      return;
    }

    sprintf(buf, "Name(s): %s\n\r", obj->name);
    send_to_char(buf, ch);

    sprintf(buf, "Vnum: %d  Type: %s  Resets: %d Area: %s\n\r", obj->pIndexData->vnum, item_name(obj->item_type), obj->pIndexData->reset_num, obj->pIndexData->area->name);
    send_to_char(buf, ch);

    sprintf(buf, "Short description: %s\n\rLong description: %s\n\r", obj->short_descr, obj->description);
    send_to_char(buf, ch);

    sprintf(buf, "Adjust String: %s\n\r", obj->adjust_string);
    send_to_char(buf, ch);

    sprintf(buf, "Wear bits: %s\n\rExtra bits: %s\n\r", wear_bit_name(obj->wear_flags), extra_bit_name(obj->extra_flags));
    send_to_char(buf, ch);

    sprintf(buf, "Number: %d/%d  Size: %d\n\r", 1, get_obj_number(obj), obj->size);
    send_to_char(buf, ch);

    sprintf(buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d  Rot: %d\n\r", obj->level, obj->cost, obj->condition, obj->timer, obj->rot_timer);
    send_to_char(buf, ch);

    sprintf(buf, "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r", obj->in_room == NULL ? 0 : obj->in_room->vnum, obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr, obj->carried_by == NULL
    ? "(none)" : can_see(ch, obj->carried_by) ? obj->carried_by->name
    : "someone", obj->wear_loc);
    send_to_char(buf, ch);

    sprintf(buf, "Values: %d %d %d %d %d %d\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4], obj->value[5]);
    send_to_char(buf, ch);

    /* now give out vital statistics as per identify */

    switch (obj->item_type) {
    case ITEM_DRINK_CON:
      sprintf(buf, "It holds %s-colored %s.\n\r", liq_table[obj->value[2]].liq_color, liq_table[obj->value[2]].liq_name);
      send_to_char(buf, ch);
      break;

    case ITEM_WEAPON:
      send_to_char("Weapon type is ", ch);
      switch (obj->value[0]) {
      case (WEAPON_EXOTIC):
        send_to_char("exotic\n\r", ch);
        break;
      case (WEAPON_SWORD):
        send_to_char("sword\n\r", ch);
        break;
      case (WEAPON_DAGGER):
        send_to_char("dagger\n\r", ch);
        break;
      case (WEAPON_SPEAR):
        send_to_char("spear/staff\n\r", ch);
        break;
      case (WEAPON_MACE):
        send_to_char("mace/club\n\r", ch);
        break;
      case (WEAPON_AXE):
        send_to_char("axe\n\r", ch);
        break;
      case (WEAPON_FLAIL):
        send_to_char("flail\n\r", ch);
        break;
      case (WEAPON_FIST):
        send_to_char("fist\n\r", ch);
        break;
      default:
        send_to_char("unknown\n\r", ch);
        break;
      }
      sprintf(buf, "Damage is %dd%d (average %d)\n\r", obj->value[1], obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
      send_to_char(buf, ch);

      if (obj->value[4]) /* weapon flags */
      {
        sprintf(buf, "Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
        send_to_char(buf, ch);
      }
      break;

    case ITEM_ARMOR:
      sprintf(buf, "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
      send_to_char(buf, ch);
      break;

    case ITEM_CONTAINER:
      sprintf(buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r", obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
      send_to_char(buf, ch);
      if (obj->value[4] != 100) {
        sprintf(buf, "Weight multiplier: %d%%\n\r", obj->value[4]);
        send_to_char(buf, ch);
      }
      break;
    }

    if (obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL) {
      EXTRA_DESCR_DATA *ed;

      send_to_char("Extra description keywords: '", ch);

      for (ed = obj->extra_descr; ed != NULL; ed = ed->next) {
        send_to_char("1)", ch);
        send_to_char(ed->keyword, ch);
        if (ed->next != NULL)
        send_to_char(" ", ch);
      }

      for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next) {
        send_to_char("2)", ch);
        send_to_char(ed->keyword, ch);
        if (ed->next != NULL)
        send_to_char(" ", ch);
      }

      send_to_char("'\n\r", ch);
    }

    for (paf = obj->affected; paf != NULL; paf = paf->next) {
      sprintf(buf, "Affects %s by %d, level %d", affect_loc_name(paf->location), paf->modifier, paf->level);
      send_to_char(buf, ch);
      if (paf->duration > -1)
      sprintf(buf, ", %d hours.\n\r", paf->duration);
      else
      sprintf(buf, ".\n\r");
      send_to_char(buf, ch);
      if (paf->bitvector) {
        switch (paf->where) {
        case TO_AFFECTS:
          sprintf(buf, "Adds %s affect.\n", affect_bit_name(paf->bitvector));
          break;
        default:
          sprintf(buf, "Unknown bit %d: %d\n\r", paf->where, paf->bitvector);
          break;
        }
        send_to_char(buf, ch);
      }
    }

    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
      sprintf(buf, "Affects %s by %d, level %d.\n\r", affect_loc_name(paf->location), paf->modifier, paf->level);
      send_to_char(buf, ch);
      if (paf->bitvector) {
        switch (paf->where) {
        case TO_AFFECTS:
          sprintf(buf, "Adds %s affect.\n", affect_bit_name(paf->bitvector));
          break;
        default:
          sprintf(buf, "Unknown bit %d: %d\n\r", paf->where, paf->bitvector);
          break;
        }
        send_to_char(buf, ch);
      }
    }

    return;
  }

  _DOFUN(do_mstat) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    SET temp_set;
    time_t sexstat;
    //    tm          * ptm;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Stat whom?\n\r", ch);
      return;
    }
    if ((victim = get_char_anywhere(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (!IS_NPC(victim) && ch->level < 106) {
      send_to_char("Not on mortals.\n\r", ch);
      return;
    }
    if (!story_on(victim) && !IS_NPC(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    sprintf(buf, "Name: %s\n\r", victim->name);
    send_to_char(buf, ch);

    if (!IS_NPC(victim)) {
      sprintf(buf, "Intro: %s\n\r", victim->pcdata->intro_desc);
      send_to_char(buf, ch);
    }
    if (!IS_NPC(victim)) {
      sprintf(buf, "Guest: %d\n\r", victim->pcdata->guest_type);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Vnum: %d  Format: %s  Race: %s\n\rSex: %s  Room: %d\n\r", IS_NPC(victim) ? victim->pIndexData->vnum : 0, IS_NPC(victim) ? "npc" : "pc", race_table[victim->race].name, sex_table[victim->sex].name, victim->in_room == NULL ? 0 : victim->in_room->vnum);
    send_to_char(buf, ch);

    printf_to_char(ch, "TTL: %d\n\r", victim->ttl);

    sprintf(buf, "Hp: %d/%d \n\r", victim->hit, max_hp(victim));
    send_to_char(buf, ch);

    if (!IS_NPC(victim)) {
      sprintf(buf, "Drunk: %d\n\r", victim->pcdata->conditions[COND_DRUNK]);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Carry number: %d\n\r", victim->carry_number);
    send_to_char(buf, ch);

    if (!IS_NPC(victim)) {
      sprintf(buf, "Age: %d  Played: %d  Timer: %d  Home: %d\n\r", get_age(victim), (int)(victim->played + current_time - victim->logon) / 3600, victim->timer, victim->pcdata->home);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Act: %s\n\r", act_bit_name_array(victim->act));
    send_to_char(buf, ch);

    if (victim->comm) {
      sprintf(buf, "Comm: %s\n\r", comm_bit_name_array(victim->comm));
      send_to_char(buf, ch);
    }

    if (victim->affected_by) {
      sprintf(buf, "Affected by %s\n\r", affect_bit_name_array(victim->affected_by));
      send_to_char(buf, ch);
    }

    // Wanted to see sex stats for testing without checking pfiles - Discordance
    if (!IS_NPC(victim)) {
      sprintf(buf, "Testosterone: %d\n\r", victim->pcdata->testosterone);
      send_to_char(buf, ch);
      sprintf(buf, "Estrogen: %d\n\r", victim->pcdata->estrogen);
      send_to_char(buf, ch);

      if (victim->pcdata->menstruation > 0) {
        sexstat = victim->pcdata->menstruation;
        sprintf(buf, "Menstruation: %s", (char *)ctime(&sexstat));
        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "Menstruation: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }

      if (victim->pcdata->ovulation > 0) {
        sexstat = victim->pcdata->ovulation;
        sprintf(buf, "Ovulation: %s", (char *)ctime(&sexstat));
        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "Ovulation: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }

      if (victim->pcdata->inseminated > 0) {
        sexstat = victim->pcdata->inseminated;
        sprintf(buf, "Inseminated: %s", (char *)ctime(&sexstat));
        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "Date Inseminated: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }


      if (victim->pcdata->last_sex > 0) {
        sexstat = victim->pcdata->last_sex;
        sprintf(buf, "LastSex: %s", (char *)ctime(&sexstat));
        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "LastSex: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }

      if (victim->pcdata->last_sextype > 0) {
        char sextype[MSL];
        if (victim->pcdata->last_sextype == SEXTYPE_INTERCOURSE) {
          strcpy(sextype, "Intercourse");
        }
        if (victim->pcdata->last_sextype == SEXTYPE_OUTERCOURSE) {
          strcpy(sextype, "Outercourse");
        }

        sprintf(buf, "LastSexType: %s\n\r", sextype);
        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "LastSexType: %s", "NA\n\r");
        send_to_char(buf, ch);
      }


      if (victim->pcdata->last_shower > 0) {
        sexstat = victim->pcdata->last_shower;
        sprintf(buf, "LastShower: %s", (char *)ctime(&sexstat));

        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "LastShower: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }

      sprintf(buf, "SexDirty: %s\n\r", (victim->pcdata->sex_dirty == TRUE) ? "True" : "False");
      send_to_char(buf, ch);

      if (victim->pcdata->due_date > 0) {
        sexstat = victim->pcdata->due_date;
        sprintf(buf, "Due date: %s", (char *)ctime(&sexstat));

        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "Due date: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }

      if (victim->pcdata->daily_upkeep > 0) {
        sexstat = victim->pcdata->daily_upkeep;
        sprintf(buf, "Daily Upkeep: %s", (char *)ctime(&sexstat));

        send_to_char(buf, ch);
      }
      else {
        sprintf(buf, "Sex Upkeep: %d/%d\n\r", 0, 0);
        send_to_char(buf, ch);
      }

      sprintf(buf, "Fertility: %d\n\r", victim->pcdata->natural_fertility);
      send_to_char(buf, ch);

      sprintf(buf, "Potency: %d\n\r", victim->pcdata->sex_potency);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Master: %s\n\r", victim->master ? victim->master->name : "(none)");
    send_to_char(buf, ch);

    // OLC
    if (!IS_NPC(victim)) {
      sprintf(buf, "Security: %d.\n\r", victim->pcdata->security);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Short description: %s\n\rLong  description: %s\n\r", victim->short_descr, victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r");
    send_to_char(buf, ch);

    for (paf = victim->affected; paf != NULL; paf = paf->next) {
      SET_INIT(temp_set);
      bitvector_to_array(temp_set, paf->bitvector);

      sprintf(buf, "Weave: '' modifies %s by %d for %d hours with bits %d %s, level %d.\n\r", affect_loc_name(paf->location), paf->modifier, paf->duration, paf->bitvector, affect_bit_name_array(temp_set), paf->level);
      send_to_char(buf, ch);
    }

    if (is_gm(victim) && get_karma_plot(victim) != NULL) {
      printf_to_char(ch, "Active plot: %s\n\r", get_karma_plot(victim)->title);
    }

    return;
  }

  /* ofind and mfind replaced with vnum, vnum skill also added */
  _DOFUN(do_vnum) {
    char arg[MAX_INPUT_LENGTH];
    char *string;

    string = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  vnum obj <name>\n\r", ch);
      send_to_char("  vnum mob <name>\n\r", ch);
      send_to_char("  vnum skill <skill or spell>\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "obj")) {
      do_function(ch, &do_ofind, string);
      return;
    }

    if (!str_cmp(arg, "mob") || !str_cmp(arg, "char")) {
      do_function(ch, &do_mfind, string);
      return;
    }

    /* do both */
    do_function(ch, &do_mfind, argument);
    do_function(ch, &do_ofind, argument);
  }

  _DOFUN(do_mfind) {
    extern int top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Find whom?\n\r", ch);
      return;
    }

    fAll = FALSE; /* !str_cmp( arg, "all" ); */
    found = FALSE;
    nMatch = 0;

    /*
    * Yeah, so iterating over all vnum's takes 10,000 loops.
    * Get_mob_index is fast, and I don't feel like threading another link.
    * Do you?
    * -- Furey
    */
    for (vnum = 0; nMatch < top_mob_index; vnum++) {
      if ((pMobIndex = get_mob_index(vnum)) != NULL) {
        nMatch++;
        if (fAll || is_name(argument, pMobIndex->player_name)) {
          found = TRUE;
          sprintf(buf, "[%5d] {%3d} %s\n\r", pMobIndex->vnum, pMobIndex->level, pMobIndex->short_descr);
          send_to_char(buf, ch);
        }
      }
    }

    if (!found)
    send_to_char("No mobiles by that name.\n\r", ch);

    return;
  }

  _DOFUN(do_ofind) {
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Find what?\n\r", ch);
      return;
    }

    fAll = FALSE; /* !str_cmp( arg, "all" ); */
    found = FALSE;
    nMatch = 0;

    /*
    * Yeah, so iterating over all vnum's takes 10,000 loops.
    * Get_obj_index is fast, and I don't feel like threading another link.
    * Do you?
    * -- Furey
    */
    for (vnum = 0; nMatch < top_obj_index; vnum++) {
      if ((pObjIndex = get_obj_index(vnum)) != NULL) {
        nMatch++;
        if (fAll || is_name(argument, pObjIndex->name)) {
          found = TRUE;
          sprintf(buf, "[%5d] {%3d}  %s\n\r", pObjIndex->vnum, pObjIndex->level, pObjIndex->short_descr);
          send_to_char(buf, ch);
        }
      }
    }

    if (!found)
    send_to_char("No objects by that name.\n\r", ch);

    return;
  }

  _DOFUN(do_owhere) {
    char buf[MAX_INPUT_LENGTH * 4];
    Buffer outbuf;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    if (argument[0] == '\0') {
      send_to_char("Find what?\n\r", ch);
      return;
    }

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      OBJ_DATA *obj = *it;
      if (!can_see_obj(ch, obj) || !is_name(argument, obj->name) || ch->level < obj->level)
      continue;

      if (obj->pIndexData->vnum == 3403 && !IS_IMP(ch)) {
        logfi("%s attempted to locate Palin's Pocket PPPocket", ch->name);
        continue;
      }

      found = TRUE;
      number++;

      for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj)
      ;

      if (in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by) && in_obj->carried_by->in_room != NULL)
      sprintf(buf, "%3d) %s[Room %d]\n\r", number, obj->short_descr, in_obj->carried_by->in_room->vnum);
      else if (in_obj->in_room != NULL && can_see_room(ch, in_obj->in_room))
      sprintf(buf, "%3d) %s is in %s [Room %d]\n\r", number, obj->short_descr, in_obj->in_room->name, in_obj->in_room->vnum);
      else
      sprintf(buf, "%3d) %s is somewhere\n\r", number, obj->short_descr);

      buf[0] = UPPER(buf[0]);
      outbuf.strcat(buf);

      if (number >= max_found)
      break;
    }
    if (!found)
    send_to_char("Nothing like that in heaven or earth.\n\r", ch);
    else
    page_to_char(outbuf, ch);
  }

  _DOFUN(do_mwhere) {
    char buf[MAX_STRING_LENGTH], buf1[MIL], colorless_room[MSL];
    Buffer outbuf;
    CHAR_DATA *victim;
    bool found;
    int count = 0;

    if (argument[0] == '\0') {
      /* show characters logged */
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL && can_see(ch, d->character) && can_see_room(ch, d->character->in_room) && can_see(ch, d->character)) {
          victim = d->character;

          if (IS_FLAG(victim->act, PLR_SPYSHIELD))
          continue;

          count++;
          remove_color(colorless_room, victim->in_room->name);

          if (d->original != NULL) {
            sprintf(buf, "`c%3d`g) `x%s `g(`cin the body of %s`g) `cis in `x%s `g[`W%d`g]`x", count, d->original->name, victim->short_descr, colorless_room, victim->in_room->vnum);
          }
          else {
            sprintf(buf, "`c%3d`g) `x%s `cis in `x%s ", count, victim->name, colorless_room);
            sprintf(buf1, "`g[`W%d`g]`x", victim->in_room->vnum);
            strcat(buf, buf1);
          }

          if (!strcmp(ch->name, "Discordance")) {
            if (victim->pcdata->penis == 0) {
              int eggspan = 0;
              if (IS_AFFECTED(victim, AFF_FERTILITY)) {
                eggspan = 3600 * 24 * 6;
              }
              else {
                eggspan = 3600 * 24 * 3;
              }
              if (current_time <= victim->pcdata->ovulation - eggspan) {
                strcat(buf, "`g[`MFertile`g]`x");
              }
            }
          }

          strcat(buf, "\n\r");

          outbuf.strcat(buf);
        }
      }

      page_to_char(outbuf, ch);
      return;
    }

    found = FALSE;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (victim->in_room != NULL && is_name(argument, victim->name)) {
        found = TRUE;
        count++;
        sprintf(
        buf, "%3d) [%5d] %*.*s [%5d] %s\n\r", count, IS_NPC(victim) ? victim->pIndexData->vnum : 0, calc_cfield(28, IS_NPC(victim) ? victim->short_descr : victim->name) *
        -1, calc_cfield(28, IS_NPC(victim) ? victim->short_descr : victim->name) -
        3, IS_NPC(victim) ? victim->short_descr : victim->name, victim->in_room->vnum, victim->in_room->name);
        if (!outbuf.strcat(buf))
        break;
      }
    }

    if (!found)
    act("You didn't find any $T.", ch, NULL, argument, TO_CHAR);
    else
    page_to_char(outbuf, ch);

    return;
  }

  _DOFUN(do_reboot) {
    char arg1[MAX_INPUT_LENGTH];

    one_argument(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Syntax:  Reboot Now\n\r         Reboot Stop\n\r         Reboot #\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "now")) {
      reboot_shutdown(ch, TRUE);
      return;
    }

    if (!str_cmp(arg1, "stop")) {
      if (pulse_muddeath >= 0 && isreboot) {
        pulse_muddeath = -1;
        isreboot = TRUE;
        mudkiller = NULL;
        do_echo(ch, "Reboot has been stopped, resume normal activities.\n\r");
        return;
      }
      else {
        send_to_char("There is no reboot countdown in progress.\n\r", ch);
        return;
      }
    }
    if (is_number(arg1)) {
      if (atoi(arg1) < 1 || atoi(arg1) > 10) {
        send_to_char("Time range is between 1 and 10 minutes.\n\r", ch);
        return;
      }
      else {
        pulse_muddeath = atoi(arg1);
        isreboot = TRUE;
        mudkiller = ch;
        check_muddeath();
        return;
      }
    }
    else {
      send_to_char("Syntax:  Reboot Now\n\r         Reboot Stop\n\r         Reboot #\n\r", ch);
      return;
    }

    return;
  }

  _DOFUN(do_shutdown) {
    char arg1[MAX_INPUT_LENGTH];

    one_argument(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Syntax:  Shutdown Now\n\r         Shutdown Stop\n\r         Shutdown #\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "now")) {
      reboot_shutdown(ch, FALSE);
      return;
    }

    if (!str_cmp(arg1, "stop")) {
      if (pulse_muddeath >= 0 && !isreboot) {
        pulse_muddeath = -1;
        isreboot = TRUE;
        mudkiller = NULL;
        do_echo(ch, "Shutdown has been stopped, resume normal activities.\n\r");
        return;
      }
      else {
        send_to_char("There is no shutdown countdown in progress.\n\r", ch);
        return;
      }
    }

    if (is_number(arg1)) {
      if (atoi(arg1) < 1 || atoi(arg1) > 10) {
        send_to_char("Time range is between 1 and 10 minutes.\n\r", ch);
        return;
      }
      else {
        pulse_muddeath = atoi(arg1);
        isreboot = FALSE;
        mudkiller = ch;
        check_muddeath();
        return;
      }
    }
    else {
      send_to_char("Syntax:  Shutdown Now\n\r         Shutdown Stop\n\r         Shutdown #\n\r", ch);
      return;
    }

    return;
  }

  void reboot_shutdown(CHAR_DATA *ch, bool reboot) {
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    int temp = 0;

    sprintf(buf, "%s by %s.", reboot ? "Reboot" : "Shutdown", ch->name);
    log_string(buf);
    if (!reboot) {
      append_file(ch, SHUTDOWN_FILE, buf);
      strcat(buf, "\n\r");
    }

    do_echo(ch, buf);

    merc_down = TRUE;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end();) {
      DESCRIPTOR_DATA *d = *it;
      ++it;

      if (d->connected == CON_PLAYING)
      save_char_obj(d->original ? d->original : d->character, TRUE, FALSE);

      if (d->ipid > 0) /* Kill any old resolve */
      {
#if defined(WIN32)
        _cwait(&temp, d->ipid, _WAIT_CHILD);
#else
        kill(d->ipid, SIGKILL);
        waitpid(d->ipid, &temp, 0);
#endif
        close_socket(d->ifd);
      }
      close_desc(d);
    }

    return;
  }

  /*
  * Allows for auto-shutdown by the signal handling system
  */
  void do_auto_shutdown() {
    FILE *fp, *fpCopy;
    char buf[MAX_STRING_LENGTH], buf2[100];
    extern int port, control; // comm.c /
    int temp = 0;

    if ((fp = fopen(LAST_COMMAND, "a")) == NULL)
    bug("Error in do_auto_shutdown opening lastcomm.txt", 0);

    if ((fpCopy = fopen(COPYOVER_FILE, "w")) == NULL) {
      logfi("Could not write to copyover file: %s", COPYOVER_FILE);
      perror("do_copyover:fopen");
      return;
    }

    fprintf(fp, "***********************************\n");
    fprintf(fp, "Mud Crashed: %s", (char *)ctime(&current_time));
    fprintf(fp, "Last Command: %s\n", last_command);
    fprintf(fp, "***********************************\n");
    fprintf(fp, "Online Players:\n");

    save_ground_objects();

    /* For each playing descriptor, save its state */
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end();) {
      DESCRIPTOR_DATA *d = *it;
      ++it; // We delete from the list , so need to save this
      CHAR_DATA *och = CH(d);

      if (d->ipid > 0) /* Kill any old resolve */
      {
#if defined(WIN32)
        _cwait(&temp, d->ipid, _WAIT_CHILD);
#else
        kill(d->ipid, SIGKILL);
        waitpid(d->ipid, &temp, 0);
#endif
        close_socket(d->ifd);
      }

      // Drop people just logging on
      if (!d->character || (d->connected > CON_PLAYING && d->connected != CON_CREATION)) {
        write_to_descriptor(
        d->descriptor, "\n\rSorry, we are rebooting. Come back in a few seconds.\n\r", 0);
        close_desc(d);
      }
      else {
        fprintf(fpCopy, "%d %s %s %s\n", d->descriptor, och->name, d->host, d->hostip);
        och = CH(d);
        if (och != NULL) {
          fprintf(fp, "%s\n", och->name);
          save_char_obj(och, TRUE, FALSE);
        }

        write_to_descriptor(
        d->descriptor, "\n\rHaven has crashed.\n\rIf you can identify the bug, please note Immortal.\n\rPlease wait while we reload your character.\n\r", 0);
      }
    }

    fprintf(fpCopy, "-1\n");
    fclose(fpCopy);
    fclose(fp);

    /* exec - descriptors are inherited */
    sprintf(buf, "%d", port);
    sprintf(buf2, "%d", control);

#if defined(WIN32)
    _execl(EXE_FILE, "rom", buf, "copyover", buf2, (char *)NULL);
#else
    execl(EXE_FILE, "haven", buf, "copyover", buf2, (char *)NULL);
#endif

    /* Failed - sucessful exec will not return */
    perror("do_copyover: execl");
  }

  _DOFUN(do_snoop) {
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Snoop whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->desc == NULL) {
      send_to_char("No descriptor to snoop.\n\r", ch);
      return;
    }
    if (!story_on(victim) && ch->level < 106) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim == ch) {
      send_to_char("Cancelling all snoops.\n\r", ch);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        if ((*it)->snoop_by == ch->desc)
        (*it)->snoop_by = NULL;
      }
      return;
    }

    if (victim->desc->snoop_by != NULL) {
      send_to_char("Busy already.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (ch->desc != NULL) {
      for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by) {
        if (d->character == victim || d->original == victim) {
          send_to_char("No snoop loops.\n\r", ch);
          return;
        }
      }
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char("Ok.\n\r", ch);
    return;
  }

  _DOFUN(do_switch) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Switch into whom?\n\r", ch);
      return;
    }

    if (ch->desc == NULL)
    return;

    if (ch->desc->original != NULL) {
      send_to_char("You are already switched.\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim == ch) {
      send_to_char("Ok.\n\r", ch);
      return;
    }

    if (!IS_NPC(victim)) {
      send_to_char("You can only switch into mobiles.\n\r", ch);
      return;
    }

    if (victim->desc != NULL) {
      send_to_char("Character in use.\n\r", ch);
      return;
    }

    ch->desc->character = victim;
    ch->desc->original = ch;
    victim->desc = ch->desc;
    ch->desc = NULL;
    /* change communications to match */
    SET_COPY(victim->comm, ch->comm);

    victim->lines = ch->lines;

    /* Change pcdata!!!! */
    victim->pcdata = ch->pcdata;
    send_to_char("Ok.\n\r", victim);
    return;
  }

  _DOFUN(do_return) {
    char buf[MAX_STRING_LENGTH];

    if (ch->desc == NULL)
    return;

    if (ch->desc->original == NULL) {
      send_to_char("You aren't switched.\n\r", ch);
      return;
    }

    send_to_char("You return to your original body. Type replay to see any missed tells.\n\r", ch);

    if (ch->prompt != NULL) {
      free_string(ch->prompt);
      ch->prompt = NULL;
    }

    sprintf(buf, "$N returns from %s.", ch->short_descr);
    wiznet(buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE, get_trust(ch));
    SET_FLAG(ch->desc->character->comm, COMM_NOCHANNELS);
    SET_FLAG(ch->desc->character->comm, COMM_NOTELL);

    ch->desc->character->pcdata = NULL;
    ch->desc->character = ch->desc->original;
    ch->desc->original = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc = NULL;
    return;
  }

  /* trust levels for load and clone */
  bool obj_check(CHAR_DATA *ch, OBJ_DATA *obj) {
    if (IS_TRUSTED(ch, LEVEL_IMMORTAL))
    return TRUE;
    else
    return FALSE;
  }

  /* for clone, to insure that cloning goes many levels deep */
  void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone) {
    OBJ_DATA *c_obj, *t_obj;

    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content) {
      if (obj_check(ch, c_obj)) {
        t_obj = create_object(c_obj->pIndexData, 0);
        clone_object(c_obj, t_obj);
        obj_to_obj(t_obj, clone);
        recursive_clone(ch, c_obj, t_obj);
      }
    }
  }

  /* command that is similar to load
  _DOFUN(do_clone)
  {
  char arg[MAX_INPUT_LENGTH];
  char *rest;
  CHAR_DATA *mob;
  OBJ_DATA  *obj;

  rest = one_argument(argument,arg);

  if (arg[0] == '\0')
  {
  send_to_char("Clone what?\n\r",ch);
  return;
  }

  if (!str_prefix(arg,"object"))
  {
  mob = NULL;
  obj = get_obj_here(ch, NULL, rest);
  if (obj == NULL)
  {
  send_to_char("You don't see that here.\n\r",ch);
  return;
  }
  }
  else if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
  {
  obj = NULL;
  mob = get_char_room(ch, NULL, rest);
  if (mob == NULL)
  {
  send_to_char("You don't see that here.\n\r",ch);
  return;
  }
  }
  else  find both
  {
  mob = get_char_room(ch, NULL, argument);
  obj = get_obj_here(ch, NULL, argument);
  if (mob == NULL && obj == NULL)
  {
  send_to_char("You don't see that here.\n\r",ch);
  return;
  }
  }

  clone an object
  if (obj != NULL)
  {
  OBJ_DATA *clone;

  if (!obj_check(ch,obj))
  {
  send_to_char("Your powers are not great enough for such a
  task.\n\r",ch); return;
  }

  clone = create_object(obj->pIndexData,0);
  clone_object(obj,clone);
  if (obj->carried_by != NULL)
  obj_to_char(clone,ch);
  else
  obj_to_room(clone, ch->in_room);
  recursive_clone(ch,obj,clone);

  act("$n has created $p.",ch,clone,NULL,TO_ROOM);
  act("You clone $p.",ch,clone,NULL,TO_CHAR);
  wiznet("$N clones
  $p.",ch,clone,WIZ_LOAD,WIZ_SECURE,get_trust(ch)); return;
  }
  else if (mob != NULL)
  {
  CHAR_DATA *clone;
  OBJ_DATA *new_obj;
  char buf[MAX_STRING_LENGTH];

  if (!IS_NPC(mob))
  {
  send_to_char("You can only clone mobiles.\n\r",ch);
  return;
  }

  clone = create_mobile(mob->pIndexData);
  clone_mobile(mob,clone);

  for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
  {
  if (obj_check(ch,obj))
  {
  new_obj = create_object(obj->pIndexData,0);
  clone_object(obj,new_obj);
  recursive_clone(ch,obj,new_obj);
  obj_to_char(new_obj,clone);
  new_obj->wear_loc = obj->wear_loc;
  }
  }
  char_to_room(clone,ch->in_room);
  act("$n has created $N.",ch,NULL,clone,TO_ROOM);
  act("You clone $N.",ch,NULL,clone,TO_CHAR);
  sprintf(buf,"$N clones %s.",clone->short_descr);
  wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
  return;
  }
  }
  */
  
  /* RT to replace the two load commands */
  _DOFUN(do_load) {
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  load mob <vnum>\n\r", ch);
      send_to_char("  load obj <vnum> <level>\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "mob") || !str_cmp(arg, "char")) {
      do_function(ch, &do_mload, argument);
      return;
    }

    if (!str_cmp(arg, "obj")) {
      do_function(ch, &do_oload, argument);
      return;
    }
    /* echo syntax */
    do_function(ch, &do_load, "");
  }

  _DOFUN(do_mload) {
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument(argument, arg);

    if (arg[0] == '\0' || !is_number(arg)) {
      send_to_char("Syntax: load mob <vnum>.\n\r", ch);
      return;
    }

    if ((pMobIndex = get_mob_index(atoi(arg))) == NULL) {
      send_to_char("No mob has that vnum.\n\r", ch);
      return;
    }

    victim = create_mobile(pMobIndex);
    char_to_room(victim, ch->in_room);
    act("$n has created $N!", ch, NULL, victim, TO_ROOM);
    sprintf(buf, "$N loads %s.", victim->short_descr);
    //  wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    send_to_char("Ok.\n\r", ch);
    return;
  }

  _DOFUN(do_oload) {
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj = NULL;
    int level = -1;
    char arg3[MAX_INPUT_LENGTH];
    int number;
    int total = 1;
    int vnum = 0;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg1[0] == '\0' || !is_number(arg1) || (arg2[0] != '\0' && !is_number(arg2)) || (arg3[0] != '\0' && !is_number(arg3))) {

      send_to_char("Syntax: load obj <vnum>\n\r        load obj <#> <vnum>\n\r        load obj <#> <vnum> <level>\n\r", ch);

      return;
    }

    if (arg2[0] != '\0') {
      total = atoi(arg1);
      vnum = atoi(arg2);

      if (arg3[0] != '\0')
      level = atoi(arg3);
    }
    else {
      total = 1;
      vnum = atoi(arg1);
      level = ch->level;
    }

    if (level > get_trust(ch))
    level = get_trust(ch);

    if ((pObjIndex = get_obj_index(vnum)) == NULL) {
      send_to_char("No object has that vnum.\n\r", ch);
      return;
    }

    if (total > 200) {
      send_to_char("You don't quite need that many objects... the correct syntax for this command is:\n\r", ch);
      send_to_char("load object <amount> <vnum>\n\r", ch);
      return;
    }

    for (number = 1; number <= total; number++) {
      obj = create_object(pObjIndex, level);
      setdescs(obj);
      if (!IS_SET(obj->extra_flags, ITEM_NORESALE))
      SET_BIT(obj->extra_flags, ITEM_NORESALE);
      if (CAN_WEAR(obj, ITEM_TAKE))
      obj_to_char(obj, ch);
      else
      obj_to_room(obj, ch->in_room);
    }

    if (total == 1) {
      act("$n has created $p!", ch, obj, NULL, TO_ROOM);
      act("You have created $p!", ch, obj, NULL, TO_CHAR);
    }
    else {
      char buf[MAX_INPUT_LENGTH];

      sprintf(buf, "$n has created %d $p(s)!", total);
      act(buf, ch, obj, NULL, TO_ROOM);
      sprintf(buf, "You have created %d $p(s)!", total);
      act(buf, ch, obj, NULL, TO_CHAR);
    }

    send_to_char("Ok.\n\r", ch);
    return;
  }

  _DOFUN(do_purge) {
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      /* 'purge' */
      OBJ_DATA *obj_next;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end();) {
        CHAR_DATA *victim = *it;
        ++it;

        if (IS_NPC(victim) && !IS_FLAG(victim->act, ACT_NOPURGE) && victim != ch /* safety precaution */) {
          save_char_obj(victim, TRUE, FALSE);
          extract_char(victim, TRUE);
        }
      }

      for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        extract_obj(obj);
      }

      act("$n purges the room!", ch, NULL, NULL, TO_ROOM);
      send_to_char("Ok.\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (!IS_NPC(victim)) {
      if (ch == victim) {
        send_to_char("Ho ho ho.\n\r", ch);
        return;
      }

      if (get_trust(ch) <= get_trust(victim) || (get_trust(ch) < 106)) {
        send_to_char("Maybe that wasn't a good idea...\n\r", ch);
        sprintf(buf, "%s tried to purge you!\n\r", ch->name);
        send_to_char(buf, victim);
      }

      act("$n disintegrates $N.", ch, 0, victim, TO_NOTVICT);

      save_char_obj(victim, TRUE, FALSE);
      d = victim->desc;
      extract_char(victim, TRUE);
      if (d != NULL)
      d->connected = CON_QUITTING;

      //			close_desc( d );

      return;
    }

    save_char_obj(victim, TRUE, FALSE);
    extract_char(victim, TRUE);
    return;
  }

  _DOFUN(do_advance) {
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MIL];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) || arg3[0] == '\0') {
      send_to_char("Syntax: advance <char> <level> <your name>.\n\r", ch);
      return;
    }

    if (!strcmp(ch->name, arg3)) {
      send_to_char("You must confirm the advance by typing your name at the end of the command.\n\r", ch);
      send_to_char("Syntax: advance <char> <level> <your name>.\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("That player is not here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    if (victim->level >= ch->level) {
      send_to_char("Uh.. me no think so.", ch);
      return;
    }

    if ((level = atoi(arg2)) < 1 || level > MAX_LEVEL) {
      sprintf(buf, "Level must be 1 to %d.\n\r", MAX_LEVEL);
      send_to_char(buf, ch);
      return;
    }

    if (level > get_trust(ch)) {
      send_to_char("Limited to your trust level.\n\r", ch);
      return;
    }

    /*
    * Lower level:
    *   Reset to level 1.
    *   Then raise again.
    *   Currently, an imp can lower another imp.
    *   -- Swiftest
    */
    if (level <= victim->level) {

      send_to_char("Lowering a player's level!\n\r", ch);
      send_to_char("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim);
      victim->level = 1;
      victim->trust = 1;
      victim->hit = max_hp(victim);
      advance_level(victim, TRUE);
    }
    else {
      send_to_char("Raising a player's level!\n\r", ch);
      send_to_char("**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim);
    }

    for (iLevel = victim->level; iLevel < level; iLevel++) {
      victim->level += 1;
      victim->trust += 1;
      advance_level(victim, TRUE);
    }

    sprintf(buf, "You are now level %d.\n\r", victim->level);
    send_to_char(buf, victim);

    victim->level--;
    victim->level++;

    victim->trust = 0;

    save_char_obj(victim, FALSE, FALSE);
    return;
  }

  _DOFUN(do_trust) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int level;
    int i;
    if (IS_IMMORTAL(ch)) {
      argument = one_argument(argument, arg1);
      argument = one_argument(argument, arg2);

      if (arg1[0] == '\0' || arg2[0] == '\0' || (!is_number(arg2) && strcmp(arg2, "spectrust"))) {
        send_to_char("Syntax: trust <char> <level>.\n\r", ch);
        return;
      }

      if ((victim = get_char_world(ch, arg1)) == NULL) {
        send_to_char("That player is not here.\n\r", ch);
        return;
      }

      if (!strcmp(arg2, "spectrust")) {
        if (victim->pcdata->spec_trust) {
          send_to_char("You have removed their special trust.\n\r", ch);
          victim->pcdata->spec_trust = FALSE;
        }
        else {
          victim->pcdata->spec_trust = TRUE;
          send_to_char("You have given them special trust.\n\r", ch);
        }
        return;
      }

      if ((level = atoi(arg2)) < 0 || level > MAX_LEVEL) {
        sprintf(buf, "Level must be 0 (reset) or 1 to %d.\n\r", MAX_LEVEL);
        send_to_char(buf, ch);
        return;
      }

      if (level > ch->level) {
        send_to_char("Limited to your level.\n\r", ch);
        return;
      }

      victim->trust = level;
      return;
    }
    else {
      if (safe_strlen(argument) < 3) {
        send_to_char("Syntax: trust (person) negative/none/basic/moderate/advanced\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg1);
      CHAR_DATA *victim = get_char_world_pc(arg1);

      if (victim == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!IS_FLAG(victim->act, PLR_GM) && !higher_power(victim)) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      for (i = 0; i < MAX_TRUSTS; i++) {
        if (!str_cmp(victim->name, ch->pcdata->trust_names[i])) {
          if (!str_cmp(argument, "none"))
          ch->pcdata->trust_levels[i] = 0;
          else if (!str_cmp(argument, "basic"))
          ch->pcdata->trust_levels[i] = 1;
          else if (!str_cmp(argument, "moderate"))
          ch->pcdata->trust_levels[i] = 2;
          else if (!str_cmp(argument, "advanced"))
          ch->pcdata->trust_levels[i] = 3;
          else {
            send_to_char("Syntax: trust (person) none/basic/moderate/advanced\n\r", ch);
            return;
          }
          send_to_char("Trust set.\n\r", ch);
          printf_to_char(victim, "%s trusts you to %s level.\n\r", ch->name, argument);
          return;
        }
      }
      for (i = 0; i < MAX_TRUSTS; i++) {
        if (ch->pcdata->trust_levels[i] == 0) {
          if (!str_cmp(argument, "none"))
          ch->pcdata->trust_levels[i] = 0;
          else if (!str_cmp(argument, "basic"))
          ch->pcdata->trust_levels[i] = 1;
          else if (!str_cmp(argument, "moderate"))
          ch->pcdata->trust_levels[i] = 2;
          else if (!str_cmp(argument, "advanced"))
          ch->pcdata->trust_levels[i] = 3;
          else {
            send_to_char("Syntax: trust (person) none/basic/moderate/advanced\n\r", ch);
            return;
          }
          free_string(ch->pcdata->trust_names[i]);
          ch->pcdata->trust_names[i] = str_dup(victim->name);

          send_to_char("Trust set.\n\r", ch);
          printf_to_char(victim, "%s trusts you to %s level.\n\r", ch->name, argument);
          return;
        }
      }
    }
  }

  _DOFUN(do_restore) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);
    if (arg[0] == '\0' || !str_cmp(arg, "room")) {
      /* cure room */
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {

        CHAR_DATA *vch = *it;
        if (in_fight(vch))
        continue;

        vch->hit = max_hp(vch);
        vch->wounds = 0;
        update_pos(vch);
        act("$n has restored you.", ch, NULL, vch, TO_VICT);
      }

      sprintf(buf, "$N restored room %d.", ch->in_room->vnum);
      wiznet(buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust(ch));

      send_to_char("Room restored.\n\r", ch);
      return;
    }

    if (get_trust(ch) >= MAX_LEVEL - 8 && !str_cmp(arg, "all")) {
      /* cure all */
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        victim = (*it)->character;

        if (victim == NULL || IS_NPC(victim))
        continue;

        if (in_fight(victim))
        continue;

        victim->hit = max_hp(victim);
        update_pos(victim);
        if (victim->in_room != NULL)
        act("$n has restored you.", ch, NULL, victim, TO_VICT);
      }

      send_to_char("All active players restored.\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (in_fight(victim)) {
      send_to_char("They're in combat.\n\r", ch);
      return;
    }

    victim->hit = max_hp(victim);
    victim->wounds = 0;
    if (IS_FLAG(victim->act, PLR_DEAD)) {
      REMOVE_FLAG(victim->act, PLR_DEAD);
      victim->pcdata->final_death_date = 0;
    }
    update_pos(victim);
    act_new("$n has restored you.", ch, NULL, victim, TO_VICT, POS_DEAD);
    sprintf(buf, "$N restored %s", IS_NPC(victim) ? victim->short_descr : victim->name);
    wiznet(buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust(ch));
    send_to_char("Ok.\n\r", ch);
    return;
  }

  /*********************************************************
  * Name:        do_freevnum
  * Purpose:     List the freevnums in a specified range
  * Author:      Cameron Matthews-Dickson (Scaelorn)
  *********************************************************/
  _DOFUN(do_freevnum) {
    AREA_DATA *pArea;
    char arg1[MAX_STRING_LENGTH];
    int low, high, x, cnt, y;
    int arr[150][2]; /* Up to 150 areas */
    bool insert = FALSE;

    cnt = 0;
    argument = one_argument(argument, arg1);

    /* Make sure syntax is correct */
    if (arg1[0] == '\0' || argument[0] == '\0') {
      send_to_char("Syntax: freevnum [lowvnum] [high vnum]\n\r", ch);
      return;
    }

    /* Get the upper and lower vnum boundaries */
    low = atoi(arg1);
    high = atoi(argument);

    /* Make sure range is not invalid */
    if (low < 1 || low > 32767 || high < 1 || high > 32767) {
      send_to_char("Range values must be between 1-32767.\n\r", ch);
      return;
    }

    /* Lower limit can't be higher than the upper limit */
    if (high < low) {
      send_to_char("Lower limit must be less than upper limit.\n\r", ch);
      return;
    }

    /* Forces it to check in multiples of 10 */
    low = low / 10;
    low = low * 10;

    AreaList::iterator it;
    for (it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;

      /* Check to see if the area vnums are within the bounds */
      if ((pArea->min_vnum >= low && pArea->min_vnum <= high) || (pArea->max_vnum >= low && pArea->max_vnum <= high)) {
        /*
        * This section enters the vnums into the 2-d array in
        * ascending order.  This makes the output of free
        * ranges easier and cleaner.
        * arr[x][0] is always the min_vnum for the area and
        * arr[x][1] is always the max_vnum for the area
        */
        for (x = 0; x < cnt; x++) {
          if (arr[x][0] > pArea->min_vnum) {
            /* "Bump" everthing over in the array */
            for (y = cnt - 1; y >= x; y--) {
              arr[y + 1][0] = arr[y][0];
              arr[y + 1][1] = arr[y][1];
            }
            arr[x][0] = pArea->min_vnum;
            arr[x][1] = pArea->max_vnum;
            insert = TRUE;
            break;
          }
        }
        /*
        * If a swap was not needed then just add to end of the
        * array
        */
        if (!insert) {
          arr[cnt][0] = pArea->min_vnum;
          arr[cnt][1] = pArea->max_vnum;
        }
        insert = FALSE;
        cnt++;
      }
    }

    printf_to_char(ch, "Range: %d-%d\n\r", low, high);
    for (x = 0; x < cnt; x++) {
      if (arr[x][1] + 1 <= high && (arr[x + 1][0] - arr[x][1]) > 2) {
        printf_to_char(ch, "`BFree`x: %6d - %d\n\r", arr[x][1] + 1, (x == cnt - 1) ? (high - 1) : (arr[x + 1][0] - 1));
      }
    }

    return;
  }

  _DOFUN(do_immfreeze) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Freeze whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->act, PLR_FREEZE)) {
      REMOVE_FLAG(victim->act, PLR_FREEZE);
      send_to_char("You can play again.\n\r", victim);
      send_to_char("FREEZE removed.\n\r", ch);
      sprintf(buf, "$N thaws %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->act, PLR_FREEZE);
      send_to_char("You can't do ANYthing!\n\r", victim);
      send_to_char("FREEZE set.\n\r", ch);
      sprintf(buf, "$N puts %s in the deep freeze.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    save_char_obj(victim, FALSE, FALSE);

    return;
  }

  _DOFUN(do_log) {
    if (IS_IMMORTAL(ch)) {
      char arg[MAX_INPUT_LENGTH];
      CHAR_DATA *victim;

      one_argument(argument, arg);

      if (arg[0] == '\0') {
        send_to_char("Log whom?\n\r", ch);
        return;
      }

      if (!str_cmp(arg, "all")) {
        if (fLogAll) {
          fLogAll = FALSE;
          send_to_char("Log ALL off.\n\r", ch);
        }
        else {
          fLogAll = TRUE;
          send_to_char("Log ALL on.\n\r", ch);
        }
        return;
      }

      if ((victim = get_char_world(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }

      if (IS_NPC(victim)) {
        send_to_char("Not on NPC's.\n\r", ch);
        return;
      }

      if (IS_FLAG(victim->act, PLR_LOG)) {
        REMOVE_FLAG(victim->act, PLR_LOG);
        send_to_char("LOG removed.\n\r", ch);
      }
      else {
        SET_FLAG(victim->act, PLR_LOG);
        send_to_char("LOG set.\n\r", ch);
      }

      return;
    }
    else {
      char arg1[MSL];
      argument = one_argument_nouncap(argument, arg1);
      if (!str_cmp(arg1, "clear")) {
        for (int i = 0; i < 20; i++) {
          free_string(ch->pcdata->rp_log[i]);
          ch->pcdata->rp_log[i] = str_dup("");
        }
        send_to_char("Log cleared.\n\r", ch);
        return;
      }
      else if (!str_cmp(arg1, "start")) {
        for (int i = 0; i < 20; i++) {
          free_string(ch->pcdata->rp_log[i]);
          ch->pcdata->rp_log[i] = str_dup("");
        }
        char logs[MSL];

        time_t east_time = current_time;
        char tmp[MSL];
        char datestr[MSL];
        sprintf(tmp, "%s", (char *)ctime(&east_time));
        sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);

        ROOM_INDEX_DATA *room = ch->in_room;
        if (is_dreaming(ch))
        room = get_room_index(ch->pcdata->dream_room);
        sprintf(logs, "`c        [`x%s`c]`x\n\n%s\n%s\n%s\n\n", datestr, roomtitle(room, TRUE), room->description, displaysun_room(room));
        free_string(ch->pcdata->rp_log[0]);
        ch->pcdata->rp_log[0] = str_dup(logs);
        send_to_char("Logging started.\n\r", ch);
        ch->pcdata->rp_logging = 1;
        return;
      }
      else if (!str_cmp(arg1, "report")) {
        if (safe_strlen(ch->pcdata->rp_log[0]) < 100) {
          send_to_char("You don't have an RP log to report.\n\r", ch);
          return;
        }
        if (safe_strlen(argument) < 10) {
          send_to_char("Syntax: log report (Scout explanation)\n\r", ch);
          return;
        }
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->antagonist == 1 || (*it)->outcast == 1 || (*it)->attributes[FACTION_SCOUTS] == 0)
          continue;
          FACTION_TYPE *fac = *it;
          int oldpoint = -1;
          int oldtime = current_time;
          for (int i = 0; i < 10; i++) {
            if (fac->event_time[i] < oldtime) {
              oldtime = fac->event_time[i];
              oldpoint = i;
            }
          }
          if (oldpoint == -1)
          return;

          for (int i = 0; i < 20; i++) {
            free_string(fac->event_text[oldpoint][i]);
            fac->event_text[oldpoint][i] = str_dup("");
          }
          fac->event_time[oldpoint] = current_time;
          fac->event_type[oldpoint] = LOGEVENT_OTHER;
          fac->event_subtype[oldpoint] = 0;
          char logs[MSL];
          sprintf(logs, "%s's confidential report", ch->name);
          free_string(fac->event_title[oldpoint]);
          fac->event_title[oldpoint] = str_dup(logs);

          sprintf(logs, "`g                (`x%s`g)\n\n%s", argument, ch->pcdata->rp_log[0]);
          free_string(fac->event_text[oldpoint][0]);
          fac->event_text[oldpoint][0] = str_dup(logs);
          int gain = 0;
          for (int i = 1; i < 20; i++) {
            free_string(fac->event_text[oldpoint][i]);
            fac->event_text[oldpoint][i] = str_dup(ch->pcdata->rp_log[i]);
            gain += safe_strlen(ch->pcdata->rp_log[i]);
          }
          gain /= 10;
          gain_rpexp(ch, gain);
        }
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(arg1, "pause") || !str_cmp(arg1, "unpause")) {
        if (ch->pcdata->rp_logging == 1) {
          ch->pcdata->rp_logging = 0;
          send_to_char("Logging Paused\n\r", ch);
          return;
        }
        else {
          ch->pcdata->rp_logging = 1;
          send_to_char("Logging Unpaused\n\r", ch);
          return;
        }
      }
      else if (!str_cmp(arg1, "stop")) {
        ch->pcdata->rp_logging = 0;
        send_to_char("Logging stopped\n\r", ch);
        return;
      }
      else if (!str_cmp(arg1, "view")) {
        char arg2[MSL];
        argument = one_argument_nouncap(argument, arg2);
        int val = atoi(arg2);
        if (val <= 1 || val > 19)
        page_to_char(ch->pcdata->rp_log[0], ch);
        else
        page_to_char(ch->pcdata->rp_log[val - 1], ch);
      }
      else if (!str_cmp(arg1, "download")) {
        if(higher_power(ch))
        return;

        int val = number_range(2321, 5473392);
        char buf[MSL];
        sprintf(buf, "cp haven/prp/%s.txt /var/www/html/rplogs/%d.txt", ch->name, val);
        system(buf);
        printf_to_char(ch, "http:/`x/havenrpg.net/rplogs/%d.txt\n\r", val);
      }
      else
      send_to_char("Syntax: Log start/stop/pause/report/view\n\r", ch);
    }
  }

  _DOFUN(do_noemote) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Noemote whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->comm, COMM_NOEMOTE)) {
      REMOVE_FLAG(victim->comm, COMM_NOEMOTE);
      send_to_char("You can emote again.\n\r", victim);
      send_to_char("NOEMOTE removed.\n\r", ch);
      sprintf(buf, "$N restores emotes to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->comm, COMM_NOEMOTE);
      send_to_char("You can't emote!\n\r", victim);
      send_to_char("NOEMOTE set.\n\r", ch);
      sprintf(buf, "$N revokes %s's emotes.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_noyell) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Not allow who you yell?\n\r", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    return;
  }

  _DOFUN(do_notell) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Notell whom?", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTELL)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTELL);
      send_to_char("You can tell again.\n\r", victim);
      send_to_char("NOTELL removed.\n\r", ch);
      sprintf(buf, "$N restores tells to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTELL);
      send_to_char("You can't tell!\n\r", victim);
      send_to_char("NOTELL set.\n\r", ch);
      sprintf(buf, "$N revokes %s's tells.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_nopray) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nopray whom?", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOPRAY)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOPRAY);
      send_to_char("Your ability to pray has been reinstated.\n\r", victim);
      send_to_char("NOPRAY removed.\n\r", ch);
      sprintf(buf, "$N restores prays to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOPRAY);
      send_to_char("Your ability to pray has been removed.\n\r", victim);
      send_to_char("NOPRAY set.\n\r", ch);
      sprintf(buf, "$N revokes %s's prays.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_nofeedback) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Notell whom?", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOFEEDBACK)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOFEEDBACK);
      send_to_char("NOFEEDBACK removed.\n\r", ch);
      sprintf(buf, "$N restores feedback to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOFEEDBACK);
      send_to_char("NOFEEDBACK set.\n\r", ch);
      sprintf(buf, "$N revokes %s's feedback.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_nofeel) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Nofeel whom?", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOFEEL)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOFEEL);
      send_to_char("NOFEEL removed.\n\r", ch);
      sprintf(buf, "$N restores feel to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOFEEL);
      send_to_char("NOFEEL set.\n\r", ch);
      sprintf(buf, "$N revokes %s's feel.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_nosr) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Notell whom?", ch);
      return;
    }

    if ((victim = get_char_world_pc(arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
    }
    if (victim->pcdata->account == NULL) {
      send_to_char("No account found.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOSR)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOSR);
      send_to_char("You can SR again.\n\r", victim);
      send_to_char("NOSR removed.\n\r", ch);
      sprintf(buf, "$N restores SRs to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOSR);
      send_to_char("Your SRing priviledges are revoked!\n\r", victim);
      send_to_char("NOSR set.\n\r", ch);
      sprintf(buf, "$N revokes %s's SRing.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
  }

  _DOFUN(do_peace) {
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      if (IS_NPC(*it) && IS_FLAG((*it)->act, ACT_AGGRESSIVE))
      REMOVE_FLAG((*it)->act, ACT_AGGRESSIVE);
    }
    send_to_char("Ok.\n\r", ch);
    return;
  }

  _DOFUN(do_wizlock) {
    extern bool wizlock;
    wizlock = !wizlock;

    if (wizlock) {
      wiznet("$N has wizlocked the game.", ch, NULL, 0, 0, 0);
      send_to_char("Game wizlocked.\n\r", ch);
    }
    else {
      wiznet("$N removes wizlock.", ch, NULL, 0, 0, 0);
      send_to_char("Game un-wizlocked.\n\r", ch);
    }

    return;
  }

  _DOFUN(do_implock) {
    extern bool implock;
    implock = !implock;

    if (implock) {
      wiznet("$N has implocked the game.", ch, NULL, 0, 0, 0);
      send_to_char("Game implocked.\n\r", ch);
    }
    else {
      wiznet("$N removes implock.", ch, NULL, 0, 0, 0);
      send_to_char("Game un-implocked.\n\r", ch);
    }

    return;
  }

  _DOFUN(do_newlock) {
    extern bool newlock;
    newlock = !newlock;

    if (newlock) {
      wiznet("$N locks out new characters.", ch, NULL, 0, 0, 0);
      send_to_char("New characters have been locked out.\n\r", ch);
    }
    else {
      wiznet("$N allows new characters back in.", ch, NULL, 0, 0, 0);
      send_to_char("Newlock removed.\n\r", ch);
    }

    return;
  }

  /* RT set replaces sset, mset, oset, and rset */
  _DOFUN(do_set) {
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  set mob   <name> <field> <value>\n\r", ch);
      send_to_char("  set obj   <name> <field> <value>\n\r", ch);
      send_to_char("  set room  <room> <field> <value>\n\r", ch);
      send_to_char("  set skill <name> <spell or skill> <value>\n\r", ch);
      send_to_char("  set money <name> <type> <value>\n\r", ch);
      send_to_char("  set prop <type> <number> <value>\n\r", ch);
      return;
    }

    if (!str_prefix(arg, "money")) {
      do_function(ch, &do_moneyset, argument);
      return;
    }

    if (!str_prefix(arg, "prop")) {
      do_function(ch, &do_propset, argument);
      return;
    }

    if (!str_prefix(arg, "mobile") || !str_prefix(arg, "character")) {
      do_function(ch, &do_mset, argument);
      return;
    }

    if (!str_prefix(arg, "skill")) {
      do_function(ch, &do_sset, argument);
      return;
    }

    if (!str_prefix(arg, "object")) {
      do_function(ch, &do_oset, argument);
      return;
    }

    if (!str_prefix(arg, "room")) {
      do_function(ch, &do_rset, argument);
      return;
    }
    /* echo syntax */
    do_function(ch, &do_set, "");
  }

  _DOFUN(do_moneyset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    long value = 0;

    argument = one_argument(argument, arg1); // Name
    argument = one_argument(argument, arg2); // Type
    argument = one_argument(argument, arg3); // Amount

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  set money <name> <value>\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    /*
    * Snarf the value.
    */
    if (!is_number(arg2)) {
      send_to_char("Value must be numeric.\n\r", ch);
      return;
    }

    value = atoi(arg2);
    if (value < 0 || value > 10000) {
      send_to_char("Value range is 0 to 10000.\n\r", ch);
      return;
    }

    victim->money = value;

    return;
  }

  _DOFUN(do_sset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    bool fAll;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  set skill <name> <spell or skill> <value>\n\r", ch);
      send_to_char("  set skill <name> all <value>\n\r", ch);
      send_to_char("   (use the name of the skill, not the number)\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    fAll = !str_cmp(arg2, "all");

    /*
    * Snarf the value.
    */
    if (!is_number(arg3)) {
      send_to_char("Value must be numeric.\n\r", ch);
      return;
    }

    value = atoi(arg3);
    if (value < 0 || value > 100) {
      send_to_char("Value range is 0 to 100.\n\r", ch);
      return;
    }

    if (fAll) {
    }
    else {
    }

    return;
  }

  // function for returning character whether offline or not for incorporation into other functions - Discordance
  CHAR_DATA *get_char_anywhere(CHAR_DATA *ch, char *arg) {
    DESCRIPTOR_DATA d;
    CHAR_DATA *victim;

    d.original = NULL;

    if ((victim = get_char_world(ch, arg)) == NULL) {
      if ((victim = get_char_world_pc(arg)) == NULL) {
        log_string("DESCRIPTOR: get_char_anywhere");

        if (!load_char_obj(&d, arg)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg));
          return victim;
        }
        victim = d.character;
      }
    }

    return victim;
  }

  _DOFUN(do_propset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    PROP_TYPE *prop;

    smash_tilde(argument);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("set prop <type> <number> <field> <value>\n\r", ch);
      return;
    }

    if (!strcmp(arg1, "house")) {
      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
    }
    else if (!strcmp(arg1, "shop")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
    }

    if (!strcmp(arg3, "minx")) {
      if (!is_number(argument)) {
        send_to_char("MinX must be a number.\n\r", ch);
        return;
      }
      prop->minx = atoi(argument);
      send_to_char("MinX set.\n\r", ch);
    }
    else if (!strcmp(arg3, "maxx")) {
      if (!is_number(argument)) {
        send_to_char("MaxX must be a number.\n\r", ch);
        return;
      }
      prop->maxx = atoi(argument);
      send_to_char("MaxX set.\n\r", ch);
    }
    else if (!strcmp(arg3, "miny")) {
      if (!is_number(argument)) {
        send_to_char("MinY must be a number.\n\r", ch);
        return;
      }
      prop->miny = atoi(argument);
      send_to_char("MinY set.\n\r", ch);
    }
    else if (!strcmp(arg3, "maxy")) {
      if (!is_number(argument)) {
        send_to_char("MaxY must be a number.\n\r", ch);
        return;
      }
      prop->maxy = atoi(argument);
      send_to_char("MaxY set.\n\r", ch);
    }
    else if (!strcmp(arg3, "maxz")) {
      if (!is_number(argument)) {
        send_to_char("MaxZ must be a number.\n\r", ch);
        return;
      }
      prop->maxz = atoi(argument);
      send_to_char("MaxZ set.\n\r", ch);
    }
    else if (!strcmp(arg3, "minz")) {
      if (!is_number(argument)) {
        send_to_char("MinZ must be a number.\n\r", ch);
        return;
      }
      prop->minz = atoi(argument);
      send_to_char("MinZ set.\n\r", ch);
    }
    return;
  }

  _DOFUN(do_mset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MSL];
    CHAR_DATA *victim;
    int value;

    smash_tilde(argument);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    strcpy(arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("	 set char <name> <field> <value>\n\r", ch);
      send_to_char("  Field being one of:\n\r", ch);
      send_to_char("    sex race faction thirst hunger drunk cash bank\n\r", ch);
      send_to_char("    sireday siremonth sireyear heightfeet heightinches\n\r", ch);
      send_to_char("    birthday birthmonth birthyear apparentage bust\n\r", ch);
      send_to_char("    impregnated inseminated menstruation babydaddy fertility potency penis\n\r", ch);
      send_to_char("    influence finaldeath refreshghostpowers attractboost\n\r", ch);
      send_to_char("    ghost xp rpxp prom hours esteem deage abomination fleshformed\n\r", ch);
      send_to_char("    traveltime travelto storyidea_cooldown account rerolled spammer\n\r", ch);
      send_to_char("    collegegpa clean password unstasis\n\r", ch);
      send_to_char("    schemeinfluence newbiecredit nomessage impregnatedtype\n\r", ch);
      return;
    }

    if ((victim = get_char_anywhere(ch, arg1)) == NULL) {
      return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;
    /*
    * Snarf the value (which need not be numeric).
    */
    value = is_number(arg3) ? atoi(arg3) : -1;

    /*
    * Set something.
    */
    // addition of flow sets Palin

    // end additional sets

    /* Stat Setting Section */
    if (!str_cmp(arg2, "password")) {
      if (!IS_IMMORTAL(victim)) {
        char *pwdnew;
        char *p;
        pwdnew = crypt(arg3, victim->pcdata->account->name);
        for (p = pwdnew; *p != '\0'; p++) {
          if (*p == '~') {
            send_to_char("New password not acceptable, try again.\n\r", ch);
            return;
          }
        }

        free_string(victim->pcdata->account->pwd);
        victim->pcdata->account->pwd = str_dup(pwdnew);
        save_char_obj(victim, FALSE, FALSE);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Immortal accounts can't have their passwords set in this way.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg2, "lifeforce")) {
      victim->lifeforce = value;
      victim->lf_used = 0;
      victim->lf_taken = 0;
      return;
    }
    // money on hand
    if (!str_cmp(arg2, "relic")) {
      if (IS_FLAG(victim->act, PLR_MADERELIC))
      REMOVE_FLAG(victim->act, PLR_MADERELIC);
      else
      SET_FLAG(victim->act, PLR_MADERELIC);

      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "newbiecredit")) {
      victim->pcdata->total_credit = value;
      return;
    }
    if (!str_cmp(arg2, "deaged")) {
      victim->pcdata->deaged = value;
      return;
    }
    if (!str_cmp(arg2, "villain")) {
      if(IS_FLAG(victim->act, PLR_VILLAIN))
      REMOVE_FLAG(victim->act, PLR_VILLAIN);
      else
      SET_FLAG(victim->act, PLR_VILLAIN);
      return;
    }
    if (!str_cmp(arg2, "capbonus")) {
      victim->pcdata->exp_cap += value;
      return;
    }

    if (!str_cmp(arg2, "clean")) {
      victim->pcdata->blood[0] = 0;
      victim->pcdata->dirt[0] = 0;
      send_to_char("Character cleaned.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "punishbrutality")) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 60 * 40);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_SUPERTAINT;
      affect_to_char(victim, &af);

      coverup_cost(victim, 3);

      if (victim->faction != 0) {
        sprintf(buf, "Headquarters is unhappy at the level of brutality shown by %s.", victim->name);
        send_message(victim->faction, buf);
      }
      send_to_char("Punishment deployed.  This is irreversible.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "unstasis")) {
      if (IS_FLAG(victim->act, PLR_STASIS)) {
        REMOVE_FLAG(victim->act, PLR_STASIS);
        send_to_char("Stasis flag removed.\n\r", ch);
      }
      else {
        send_to_char("Char not flagged for Stasis.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg2, "nomessage")) {
      if (IS_FLAG(victim->comm, COMM_NOMESSAGE)) {
        REMOVE_FLAG(victim->comm, COMM_NOMESSAGE);
        send_to_char("NoMessage unset.\n\r", ch);
      }
      else {
        SET_FLAG(victim->comm, COMM_NOMESSAGE);
        send_to_char("NoMessage set.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg2, "rerolled")) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * 70;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_REROLLED;
      affect_to_char(victim, &af);
      send_to_char("Set.\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "unbrainwashed")) {
      if (IS_FLAG(victim->comm, COMM_CLEANSED))
      REMOVE_FLAG(victim->comm, COMM_CLEANSED);
      if (IS_FLAG(victim->comm, COMM_FORCEDPACIFIST))
      REMOVE_FLAG(victim->comm, COMM_FORCEDPACIFIST);
      free_string(victim->pcdata->brainwash_reidentity);
      victim->pcdata->brainwash_reidentity = str_dup("");
      free_string(victim->pcdata->brainwash_loyalty);
      victim->pcdata->brainwash_loyalty = str_dup("");
      for (int i = 0; i < 25; i++) {
        if (safe_strlen(victim->pcdata->memories[i]) > 2) {
          free_string(victim->pcdata->memories[i]);
          victim->pcdata->memories[i] = str_dup("");
        }
        if (safe_strlen(victim->pcdata->repressions[i]) > 2) {
          free_string(victim->pcdata->repressions[i]);
          victim->pcdata->repressions[i] = str_dup("");
        }
      }
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "storyrunner")) {
      if (IS_FLAG(victim->act, PLR_GM))
      REMOVE_FLAG(victim->act, PLR_GM);
      else
      SET_FLAG(victim->act, PLR_GM);
      send_to_char("Set.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "spammer")) {
      if (victim->pcdata->account != NULL) {
        if (!IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SPAMMER)) {
          SET_FLAG(victim->pcdata->account->flags, ACCOUNT_SPAMMER);
          send_to_char("Set as spammer.\n\r", ch);
        }
        else {
          REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_SPAMMER);
          send_to_char("SPammer flag removed.\n\r", ch);
        }
      }
      else if (victim->desc != NULL && victim->desc->account != NULL) {
        if (!IS_FLAG(victim->desc->account->flags, ACCOUNT_SPAMMER)) {
          SET_FLAG(victim->desc->account->flags, ACCOUNT_SPAMMER);
          send_to_char("Set as spammer.\n\r", ch);
        }
        else {
          REMOVE_FLAG(victim->desc->account->flags, ACCOUNT_SPAMMER);
          send_to_char("SPammer flag removed.\n\r", ch);
        }
      }
      else {
        send_to_char("Account not found.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg2, "griefer")) {
      if (victim->pcdata->account != NULL) {
        if (!IS_FLAG(victim->pcdata->account->flags, ACCOUNT_GRIEFER)) {
          SET_FLAG(victim->pcdata->account->flags, ACCOUNT_GRIEFER);
          send_to_char("Set as griefer.\n\r", ch);
        }
        else {
          REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_GRIEFER);
          send_to_char("Griefer flag removed.\n\r", ch);
        }
      }
      else if (victim->desc != NULL && victim->desc->account != NULL) {
        if (!IS_FLAG(victim->desc->account->flags, ACCOUNT_GRIEFER)) {
          SET_FLAG(victim->desc->account->flags, ACCOUNT_GRIEFER);
          send_to_char("Set as griefer.\n\r", ch);
        }
        else {
          REMOVE_FLAG(victim->desc->account->flags, ACCOUNT_GRIEFER);
          send_to_char("Griefer flag removed.\n\r", ch);
        }
      }
      else {
        send_to_char("Account not found.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg2, "due_date")) {
      if (value >= 0) {
        value = value * 3600 * 24; // days
        victim->pcdata->due_date = (current_time + (value));
      }
      else {
        victim->pcdata->due_date = 0;
      }
      return;
    }

    if (!str_cmp(arg2, "storyidea_cooldown")) {
      if (value >= 0) {
        value = value * 3600;
        victim->pcdata->account->storyidea_cooldown = (current_time + (value));
      }
      return;
    }

    if (!str_cmp(arg2, "badabomination")) {
      victim->pcdata->bad_abomination = value;
      return;
    }

    if (!str_cmp(arg2, "villainy")) {
      victim->pcdata->life_tracker[TRACK_VILLAIN] = value;
      victim->pcdata->week_tracker[TRACK_VILLAIN] = value / 2;
      return;
    }
    // setting hours on characters
    if (!str_cmp(arg2, "hours")) {
      if (value > 0) {
        value = value * 3600;
        victim->playedcopfree = victim->playedcopfree - victim->played + value;
        victim->played = value;
      }
      return;
    }

    if (!str_cmp(arg2, "traveltime")) {
      if (value >= 0) {
        victim->pcdata->travel_time = value;
      }
      return;
    }

    if (!str_cmp(arg2, "ordertimer")) {
      if (value >= 0) {
        victim->pcdata->order_timer = value;
      }
      return;
    }

    if (!str_cmp(arg2, "account")) {
      rename_account(victim, arg3);
      // victim->pcdata->account->name = str_dup(arg3);
      // victim->pcdata->account_name = str_dup(arg3);
      return;
    }

    if (!str_cmp(arg2, "travelto")) {
      if (value >= 0) {
        victim->pcdata->travel_to = value;
      }
      return;
    }

    if (!str_cmp(arg2, "fleshformed")) {
      if (value == 0) {
        victim->pcdata->fleshformed = 0;
      }
      else {
        victim->pcdata->fleshformed = current_time * UMAX(value, 1);
      }

      return;
    }
    
    if (!str_cmp(arg2, "esteem")) {
      victim->esteem_faction = value;
      return;
    }
    
    if (!str_cmp(arg2, "class")) {
      victim->pcdata->class_type = value;
      victim->pcdata->class_faction = victim->faction;
      // Command wasn't allowing me to set neutral classes backward - Discordance
      victim->pcdata->class_neutral = value;
      
      return;
    }
    
    if (!str_cmp(arg2, "deage")) {
      victim->pcdata->deaged = value;
      return;
    }
    
    if (!str_cmp(arg2, "abomination")) {
      victim->abomination = value;
      return;
    }

    if (!str_cmp(arg2, "schemeinfluence")) {
      victim->pcdata->scheme_influence = value;
      return;
    }

    if (!str_cmp(arg2, "xp")) {
      victim->exp = value;
      return;
    }

    if (!str_cmp(arg2, "influence")) {

      victim->pcdata->influence = value;
      return;
    }
    if (!str_cmp(arg2, "bust")) {
      victim->pcdata->bust = value;
      return;
    }
    if (!str_cmp(arg2, "penis")) {
      victim->pcdata->penis = value;
      return;
    }
    if (!str_prefix(arg2, "sex")) {
      if (value < 0 || value > 2) {
        send_to_char("Sex range is 0 to 2.\n\r", ch);
        return;
      }
      victim->sex = value;
      return;
    }

    // sets finaldeath date for ghosts
    if (!str_cmp(arg2, "finaldeath")) {
      if (!str_cmp(arg3, "clear"))
      victim->pcdata->final_death_date = 0;
      else if (current_time + (value * 3600) > 0)
      victim->pcdata->final_death_date = current_time + (value * 3600 * 24);
      else
      victim->pcdata->final_death_date = 0;

      return;
    }

    // restores daily ghost powers
    if (!str_cmp(arg2, "refreshghostpowers")) {
      if (!str_cmp(arg3, "all"))
      refresh_ghost_abilities(victim);

      return;
    }

    // toggles if character is ghost
    if (!str_cmp(arg2, "ghost")) {
      if (arg3[0] == '\0') {
        if (is_ghost(victim)) {
          REMOVE_FLAG(victim->act, PLR_GHOST);
        }
        else {
          SET_FLAG(victim->act, PLR_GHOST);
        }
      }
      else if (atoi(arg3) == 1) {
        SET_FLAG(victim->act, PLR_GHOST);
      }
      else if (atoi(arg3) == 0) {
        REMOVE_FLAG(victim->act, PLR_GHOST);
      }
      else {
        send_to_char("Ghost may be set to 0(false) or 1(true).\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg2, "collegeGPA")) {
      if (!is_number(arg3)) {
        send_to_char("Numbers only.", ch);
        return;
      }

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(arg1, ins->name)) {
          ins->college_prestige = atoi(arg3);
        }
      }

      return;
    }

    // sets impregnated date
    if (!str_cmp(arg2, "impregnated")) {
      if (!str_cmp(arg3, "clear"))
      victim->pcdata->impregnated = 0;
      else if (current_time + (value * 3600) > 0)
      victim->pcdata->impregnated = current_time + (value * 3600 * 24);
      else
      victim->pcdata->impregnated = 0;

      return;
    }

    if (!str_cmp(arg2, "impregnatedtype")) {
      if (!str_cmp(arg3, "parasite")) {
        victim->pcdata->impregnated_type = SEXTYPE_PARASITE;
      }
      else if (!str_cmp(arg3, "accelerated")) {
        victim->pcdata->impregnated_type = SEXTYPE_ACCELERATED;
      }
      else if (!str_cmp(arg3, "short")) {
        victim->pcdata->impregnated_type = SEXTYPE_SHORT;
      }
      else {
        victim->pcdata->impregnated_type = SEXTYPE_NORMAL;
      }

      return;
    }

    if (!str_cmp(arg2, "inseminated")) {
      if (!str_cmp(arg3, "clear"))
      victim->pcdata->inseminated = 0;
      else if (current_time + (value * 3600) > 0)
      victim->pcdata->inseminated = current_time + (value * 3600 * 24);
      else
      victim->pcdata->inseminated = 0;

      return;
    }

    if (!str_cmp(arg2, "menstruation")) {
      if (!str_cmp(arg3, "clear"))
      victim->pcdata->menstruation = 0;
      else if (current_time + (value * 3600) > 0)
      victim->pcdata->menstruation = current_time + (value * 3600 * 24);
      else
      victim->pcdata->menstruation = 0;

      return;
    }

    if (!str_cmp(arg2, "ovulation")) {
      if (!str_cmp(arg3, "clear"))
      victim->pcdata->ovulation = 0;
      else if (current_time + (value * 3600) > 0)
      victim->pcdata->ovulation = current_time + (value * 3600 * 24);
      else
      victim->pcdata->ovulation = 0;

      return;
    }

    if (!str_cmp(arg2, "potency")) {
      if (value >= 0 && value <= 100)
      victim->pcdata->sex_potency = value;
      else
      send_to_char("Potency must be from 0 to 100.", ch);

      return;
    }

    if (!str_cmp(arg2, "fertility")) {
      if (victim->sex == SEX_FEMALE) {
        if (value >= 0 && value <= 100)
        victim->pcdata->natural_fertility = value;
        else
        send_to_char("Fertility must be from 0 to 100.", ch);
      }
      else
      send_to_char("Fertility can only be set for females.", ch);

      return;
    }

    if (!str_prefix(arg2, "race")) {
      int race;
      race = race_lookup(arg3);

      if (race == -1) {
        send_to_char("That is not a valid race.\n\r", ch);
        return;
      }

      if (!IS_NPC(victim) && !race_table[race].pc_race) {
        send_to_char("That is not a valid player race.\n\r", ch);
        return;
      }

      victim->race = race;
      return;
    }

    if (!str_prefix(arg2, "modifier")) {

      if (!str_cmp(argument, "none")) {
        if (victim->modifier == MODIFIER_FLESHFORMED)
        victim->pcdata->fleshformed = 0;
        victim->modifier = 0;
        printf_to_char(ch, "%s's modifier is removed.\n\r", victim->name);
      }
      for (int i = 0; i < MAX_MODIFIER; i++) {
        if (!str_cmp(argument, modifier_table[i].name)) {
          if (elligible_modifier(victim, i)) {
            victim->modifier = i;
            printf_to_char(ch, "%s is now %s.\n\r", victim->name, modifier_table[i].name);
            if (victim->modifier == MODIFIER_FLESHFORMED)
            victim->pcdata->fleshformed = current_time;
          }
          else {
            send_to_char("That archetype can't take that modifier.\n\r", ch);
          }
          return;
        }
      }
      send_to_char("No such modifier.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Only on PCs.\n\r", ch);
      return;
    }

    if (!str_prefix(arg2, "drunk")) {
      if (value < -1 || value > 100) {
        send_to_char("Drunk range is -1 to 100.\n\r", ch);
        return;
      }
      victim->pcdata->conditions[COND_DRUNK] = value;
      return;
    }

    if (!str_prefix(arg2, "heightfeet")) {
      victim->pcdata->height_feet = atoi(arg3);
      return;
    }
    if (!str_prefix(arg2, "heightinches")) {
      victim->pcdata->height_inches = atoi(arg3);
      return;
    }
    if (!str_prefix(arg2, "birthday")) {
      if (atoi(arg3) < 1 || atoi(arg3) > 28) {
        send_to_char("Date must be between 1 and 28\n\r", ch);
        return;
      }
      victim->pcdata->birth_day = atoi(arg3);
      return;
    }

    if (!str_prefix(arg2, "birthmonth")) {
      if (atoi(arg3) < 1 || atoi(arg3) > 13) {
        send_to_char("Month must be between 1 and 13\n\r", ch);
        return;
      }
      victim->pcdata->birth_month = atoi(arg3);
      return;
    }

    if (!str_prefix(arg2, "birthyear")) {
      if (atoi(arg3) < -1000 || atoi(arg3) > 2020)
      // if(atoi(arg3) < -1000 || atoi(arg3) >time_info.year)
      {
        send_to_char("Year must be between -1000 and the present year\n\r", ch);
        return;
      }
      victim->pcdata->birth_year = atoi(arg3);
      return;
    }

    if (!str_prefix(arg2, "sireday")) {
      if (atoi(arg3) < 1 || atoi(arg3) > 28) {
        send_to_char("Date must be between 1 and 28\n\r", ch);
        return;
      }
      victim->pcdata->sire_day = atoi(arg3);
      return;
    }

    if (!str_prefix(arg2, "siremonth")) {
      if (atoi(arg3) < 1 || atoi(arg3) > 13) {
        send_to_char("Month must be between 1 and 13\n\r", ch);
        return;
      }
      victim->pcdata->sire_month = atoi(arg3);
      return;
    }

    if (!str_prefix(arg2, "sireyear")) {
      if (atoi(arg3) < -1000 || atoi(arg3) > 2020) {
        send_to_char("Year must be between -1000 and the present year\n\r", ch);
        return;
      }
      victim->pcdata->sire_year = atoi(arg3);
      return;
    }

    // Added apparent age for PCs - Discordance
    if (!str_prefix(arg2, "apparentage")) {
      if (atoi(arg3) != 0 && atoi(arg3) < 18) {
        send_to_char("Apparent age must be greater than 18.\n\r", ch);
        return;
      }
      victim->pcdata->apparant_age = atoi(argument);
      if (atoi(arg3) == 0)
      printf_to_char(ch, "%s will now appear their regular age.\n\r", victim->name);
      else
      printf_to_char(ch, "%s will now appear to be %d years old.\n\r", victim->name, victim->pcdata->apparant_age = atoi(argument));
      return;
    }

    /*
    * Generate usage message.
    */
    do_function(ch, &do_mset, "");
    return;
  }

  _DOFUN(do_string) {
    char type[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    smash_tilde(argument);
    argument = one_argument(argument, type);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    strcpy(arg3, argument);

    if (type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  string char <name> <field> <string>\n\r", ch);
      send_to_char("    fields: name short long desc roomtitle whotitle rank spec lastname hair eyes skin maim\n\r", ch);
      send_to_char("  string obj  <name> <field> <string>\n\r", ch);
      send_to_char("    fields: name short long extended name superfame\n\r", ch);
      return;
    }
    if (!str_prefix(type, "character") || !str_prefix(type, "mobile")) {
      if ((victim = get_char_world(ch, arg1)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }

      if (ch->level < victim->level) {
        send_to_char("You cannot do that.\n\r", ch);
        return;
      }

      /* clear zone for mobs */
      victim->zone = NULL;

      /* string something */
      // Adding the ability to string ranks - Discordance
      if (!str_prefix(arg2, "rank")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        if (!str_prefix(arg3, "clear")) {
          free_string(victim->pcdata->crank);
          victim->pcdata->crank = "";
          return;
        }
        // This if addresses [*****] BUG: Attempt to recycle invalid memory of
        // size 1.
        if (safe_strlen(ch->pcdata->crank) >= 1) {
          free_string(victim->pcdata->crank);
          victim->pcdata->crank = str_dup(arg3);
        }
        else {
          victim->pcdata->crank = str_dup(arg3);
        }
        return;
      }

      if (!str_prefix(arg2, "name")) {
        /*	    if ( !IS_NPC(victim) )
        {
        send_to_char( "Not on PC's.\n\r", ch );
        return;
        }*/

        char strsave[MSL];
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(victim->name));
        free_string(victim->name);
        victim->name = str_dup(arg3);
        unlink(strsave);
        return;
      }

      if (!str_prefix(arg2, "lastname")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        free_string(victim->pcdata->last_name);
        victim->pcdata->last_name = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "maim")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }

        free_string(victim->pcdata->maim);
        if (!strcmp(arg3, "clear")) {
          victim->pcdata->maim = str_dup("");
        }
        else {
          victim->pcdata->maim = str_dup(arg3);
        }
        return;
      }

      if (!str_prefix(arg2, "eyes")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        free_string(victim->pcdata->eyes);
        victim->pcdata->eyes = str_dup(arg3);
        return;
      }
      if (!str_prefix(arg2, "superfame")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        free_string(victim->pcdata->class_fame);
        victim->pcdata->class_fame = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "hair")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        free_string(victim->pcdata->hair);
        victim->pcdata->hair = str_dup(arg3);
        return;
      }
      if (!str_prefix(arg2, "skin")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        free_string(victim->pcdata->skin);
        victim->pcdata->skin = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "walk")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }
        return;
      }

      if (!str_prefix(arg2, "description")) {
        free_string(victim->description);
        victim->description = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "short")) {
        free_string(victim->short_descr);
        victim->short_descr = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "long")) {
        free_string(victim->long_descr);
        strcat(arg3, "\n\r");
        victim->long_descr = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "whorank")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }

        return;
      }

      if (!str_prefix(arg2, "roomtitle")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }

        set_title(victim, arg3);
        return;
      }

      if (!str_prefix(arg2, "whotitle")) {
        if (IS_NPC(victim)) {
          send_to_char("Not on NPC's.\n\r", ch);
          return;
        }

        set_whotitle(victim, argument);
        return;
      }

      if (!str_prefix(arg2, "spec")) {
        if (!IS_NPC(victim)) {
          send_to_char("Not on PC's.\n\r", ch);
          return;
        }

        return;
      }
    }

    if (!str_prefix(type, "object")) {
      /* string an obj */

      if ((obj = get_obj_world(ch, arg1)) == NULL) {
        send_to_char("Nothing like that in heaven or earth.\n\r", ch);
        return;
      }

      if (!str_prefix(arg2, "name")) {
        free_string(obj->name);
        obj->name = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "short")) {
        free_string(obj->short_descr);
        obj->short_descr = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "long")) {
        free_string(obj->description);
        obj->description = str_dup(arg3);
        return;
      }
      if (!str_prefix(arg2, "material")) {
        free_string(obj->material);
        obj->material = str_dup(arg3);
        return;
      }

      if (!str_prefix(arg2, "ed") || !str_prefix(arg2, "extended")) {
        EXTRA_DESCR_DATA *ed;

        argument = one_argument(argument, arg3);
        if (argument == NULL) {
          send_to_char("Syntax: oset <object> ed <keyword> <string>\n\r", ch);
          return;
        }

        strcat(argument, "\n\r");

        ed = new_extra_descr();

        ed->keyword = str_dup(arg3);
        ed->description = str_dup(argument);
        ed->next = obj->extra_descr;
        obj->extra_descr = ed;
        return;
      }
    }

    /* echo bad use message */
    do_function(ch, &do_string, "");
  }

  _DOFUN(do_oset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value;

    smash_tilde(argument);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    strcpy(arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  set obj <object> <field> <value>\n\r", ch);
      send_to_char("  Field being one of:\n\r", ch);
      send_to_char("    value0 value1 value2 value3 value4 (v1-v4)\n\r", ch);
      send_to_char("    extra wear level size cost timer type\n\r", ch);
      return;
    }

    if ((obj = get_obj_world(ch, arg1)) == NULL) {
      send_to_char("Nothing like that in heaven or earth.\n\r", ch);
      return;
    }

    /*
    * Snarf the value (which need not be numeric).
    */
    value = atoi(arg3);

    /*
    * Set something.
    */
    if (!str_cmp(arg2, "value0") || !str_cmp(arg2, "v0")) {
      obj->value[0] = UMIN(50, value);
      return;
    }

    if (!str_cmp(arg2, "value1") || !str_cmp(arg2, "v1")) {
      obj->value[1] = value;
      return;
    }

    if (!str_cmp(arg2, "value2") || !str_cmp(arg2, "v2")) {
      obj->value[2] = value;
      return;
    }

    if (!str_cmp(arg2, "value3") || !str_cmp(arg2, "v3")) {
      obj->value[3] = value;
      return;
    }

    if (!str_cmp(arg2, "value4") || !str_cmp(arg2, "v4")) {
      obj->value[4] = value;
      return;
    }

    if (!str_prefix(arg2, "extra")) {
      obj->extra_flags = value;
      return;
    }

    // better wear - Discordance
    if (!str_prefix(arg2, "wear")) {
      int value;

      if (argument[0] != '\0') {
        if ((value = flag_value(wear_flags, arg3)) != NO_FLAG) {
          TOGGLE_BIT(obj->wear_flags, value);

          send_to_char("Wear flag toggled.\n\r", ch);
          return;
        }
      }
    }

    if (!str_prefix(arg2, "level")) {
      obj->level = value;
      return;
    }

    if (!str_prefix(arg2, "size")) {
      obj->size = value;
      return;
    }

    if (!str_prefix(arg2, "cost")) {
      if (!IS_SET(obj->extra_flags, ITEM_NORESALE))
      SET_BIT(obj->extra_flags, ITEM_NORESALE);
      obj->cost = value;
      return;
    }

    if (!str_prefix(arg2, "timer")) {
      obj->timer = value;
      return;
    }

    if (!str_prefix(arg2, "rot")) {
      obj->rot_timer = value;
      return;
    }

    // Item type - Discordance
    if (!str_prefix(arg2, "type")) {
      int value;
      if (arg3[0] != '\0') {
        if ((value = flag_value(type_flags, arg3)) != NO_FLAG) {
          obj->item_type = value;

          send_to_char("Type set.\n\r", ch);
          /*
          pObj->value[0] = 0;
          pObj->value[1] = 0;
          pObj->value[2] = 0;
          pObj->value[3] = 0;
          pObj->value[4] = 0;
          */
          return;
        }
      }
    }

    /*
    * Generate usage message.
    */
    do_function(ch, &do_oset, "");
    return;
  }

  _DOFUN(do_rset) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;
    int map = -1, x = -1, y = -1;

    smash_tilde(argument);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    strcpy(arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  set room <location> <field> <value>\n\r", ch);
      send_to_char("  Field being one of:\n\r", ch);
      send_to_char("    flags sector\n\r", ch);
      return;
    }

    if ((location = find_location(ch, arg1, &map, &x, &y)) == NULL) {
      send_to_char("No such location.\n\r", ch);
      return;
    }

    /*
    * Snarf the value.
    */
    if (!is_number(arg3)) {
      send_to_char("Value must be numeric.\n\r", ch);
      return;
    }
    value = atoi(arg3);

    /*
    * Set something.
    */
    if (!str_prefix(arg2, "flags")) {
      location->room_flags = value;
      return;
    }

    if (!str_prefix(arg2, "sector")) {
      location->sector_type = value;
      return;
    }

    /*
    * Generate usage message.
    */
    do_function(ch, &do_rset, "");
    return;
  }

  // Added by Michael MacLeod (Kuval), January 15th.
  _DOFUN(do_sockets) {
    CHAR_DATA *vch;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int count;
    char *st;
    char s[100];
    char idle[10];
    bool spy = FALSE;

    count = 0;
    buf[0] = '\0';
    buf2[0] = '\0';

    strcat(buf2, "\n\r`C:`B===========================================================================`C:`x\n\r");
    strcat(buf2, "`B|<><><><><><><><><><><><><><>     `Y Sockets `B     <><><><><><><><><><><><><><>|`x\n\r");
    strcat(buf2, "`C:`B===========================================================================`C:`x\n\r");
    strcat(buf2, "`B|  [`CNum State    Login`B@`C  Idl`B] [ `CPlayer Name`B ] [  `CHosts`B  ]                   |`x\n\r");
    strcat(buf2, "`C:`B===========================================================================`C:`x\n\r");

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      spy = FALSE;
      DESCRIPTOR_DATA *d = *it;
      if (d->character)
      {
        switch (d->connected) {
        case CON_PLAYING:
          st = "Playing ";
          break;
        case CON_GET_NAME:
          st = "GetName ";
          break;
        case CON_GET_OLD_PASSWORD:
          st = "Passwd  ";
          break;
        case CON_CONFIRM_NEW_NAME:
          st = "NewName ";
          break;
        case CON_GET_NEW_PASSWORD:
          st = "NewPwd  ";
          break;
        case CON_CONFIRM_NEW_PASSWORD:
          st = "NewPwd  ";
          break;
        case CON_READ_IMOTD:
          st = "  MOTD  ";
          break;
        case CON_READ_MOTD:
          st = "  MOTD  ";
          break;
        case CON_BREAK_CONNECT:
          st = " !Link  ";
          break;
        case CON_COPYOVER_RECOVER:
          st = "CopyOvr ";
          break;
        case CON_ANSI_COLOR:
          st = "  ANSI  ";
          break;
        case CON_DONE:
          st = "  Done  ";
          break;
        case CON_REMORT_CLASS:
          st = " Remort ";
          break;
        case CON_READ_STORYLINE:
          st = " StoryL ";
          break;
        case CON_IDENT_WAIT:
          st = " IdentW ";
          break;
        case CON_QUITTING:
          st = " Quittg ";
          break;
        case CON_CREATION:
          st = "Creation";
          break;
        case CON_SELECT_NEW_SUBRACE:
          st = "NSubrace";
          break;
        case CON_SELECT_NEW_LNAME:
          st = "NewLName";
          break;
        case CON_GET_ACCOUNT:
          st = " Account";
          break;
        case CON_CHOOSE_CHARACTER:
          st = "ChooseCH";
          break;
        case CON_CONFIRM_NEW_ACCOUNT_NAME:
          st = "NewAccnt";
          break;
        case CON_GET_ACCOUNT_NAME:
          st = "AccountN";
          break;
        case CON_GET_ACCOUNT_PASSWORD:
          st = "AccntPwd";
          break;
        case CON_CONFIRM_NEW_ACCOUNT_PASSWORD:
          st = "AccntPwd";
          break;
        case CON_GET_NEW_NAME:
          st = "NewName ";
          break;
        case CON_GET_NEW_ACCOUNT_PASSWORD:
          st = "AccntPwd";
          break;
        case CON_CONFIRM_ACCOUNT_NAME:
          st = "AccntNme";
          break;
        }
        count++;

        vch = CH(d);
        strftime(s, 100, "%I:%M%p", localtime(&vch->logon));

        if (vch->timer > 0)
        sprintf(idle, "%-2d", vch->timer);
        else
        sprintf(idle, "  ");

        if (d->character != NULL) {
          if (IS_FLAG(d->character->act, PLR_SPYSHIELD)) {
            spy = TRUE;
          }
        }

        sprintf(buf, "`B|%s[`C%-3d %-8s %7s  %2s`B]  `C%-12s  %-30s %s`B|`x\n\r", (spy == TRUE) ? "`RX`B" : " ", d->descriptor, st, s, idle, (d->original) ? d->original->name
        : (d->character) ? d->character->name : "(None!)", d->host, (d->valid == TRUE) ? "Valid" : "Invalid");

        strcat(buf2, buf);
      }
      else {
        sprintf(buf, "`B|  [`C%d %d`B]  `CNone  %-30s `B|`x\n\r", d->descriptor, d->connected, d->host);
        strcat(buf2, buf);
      }
    }

    strcat(buf2, "`B||`x");
    sprintf(buf, "\n\r`B|  Users:`C%-2d`B|`x\n\r", count);
    strcat(buf2, buf);
    strcat(buf2, "`C:`B===========================================================================`C:`x\n\r");
    send_to_char(buf2, ch);
    return;
  }

  bool helpless_command(char *command) {
    if (!str_cmp(command, "wear"))
    return TRUE;

    if(!str_cmp(command, "imbue"))
    return TRUE;
    if (!str_cmp(command, "zip"))
    return TRUE;
    if (!str_cmp(command, "open"))
    return TRUE;
    //    if(!str_cmp(command, "roomtitle"))
    //	return TRUE;
    if (!str_cmp(command, "take"))
    return TRUE;
    if (!str_cmp(command, "describe"))
    return TRUE;
    //    if(!str_cmp(command, "join"))
    //	return TRUE;
    if (!str_cmp(command, "use"))
    return TRUE;
    if (!str_cmp(command, "unexpose"))
    return TRUE;
    if (!str_cmp(command, "fsay"))
    return TRUE;
    if (!str_cmp(command, "open"))
    return TRUE;

    if (!str_cmp(command, "procedure"))
    return TRUE;
    if (!str_cmp(command, "mask"))
    return TRUE;

    return FALSE;
  }

  bool ghost_command_makedo(char *command) {
    if (!str_cmp(command, "wear"))
    return TRUE;
    if (!str_cmp(command, "admit"))
    return TRUE;
    if (!str_cmp(command, "zip"))
    return TRUE;
    if (!str_cmp(command, "show"))
    return TRUE;
    if (!str_cmp(command, "use"))
    return TRUE;
    if (!str_cmp(command, "unexpose"))
    return TRUE;
    if (!str_cmp(command, "expose"))
    return TRUE;
    if (!str_cmp(command, "emote"))
    return TRUE;
    if (!str_cmp(command, "open"))
    return TRUE;
    if (!str_cmp(command, "close"))
    return TRUE;
    if (!str_cmp(command, "north"))
    return TRUE;
    if (!str_cmp(command, "south"))
    return TRUE;
    if (!str_cmp(command, "east"))
    return TRUE;
    if (!str_cmp(command, "west"))
    return TRUE;
    if (!str_cmp(command, "up"))
    return TRUE;
    if (!str_cmp(command, "down"))
    return TRUE;
    if (!str_cmp(command, "northeast"))
    return TRUE;
    if (!str_cmp(command, "southeast"))
    return TRUE;
    if (!str_cmp(command, "southwest"))
    return TRUE;
    if (!str_cmp(command, "northwest"))
    return TRUE;
    if (!str_cmp(command, "open"))
    return TRUE;
    if (!str_cmp(command, "makeup"))
    return TRUE;
    if (!str_cmp(command, "intro"))
    return TRUE;

    return FALSE;
  }

  bool helpless_command_makedo(char *command) {
    if (!str_cmp(command, "wear"))
    return TRUE;
    if (!str_cmp(command, "admit"))
    return TRUE;
    if(!str_cmp(command, "imbue"))
    return TRUE;
    if (!str_cmp(command, "remove"))
    return TRUE;
    if (!str_cmp(command, "zip"))
    return TRUE;
    if (!str_cmp(command, "roomtitle"))
    return TRUE;
    if (!str_cmp(command, "show"))
    return TRUE;
    if (!str_cmp(command, "drop"))
    return TRUE;
    if (!str_cmp(command, "give"))
    return TRUE;
    if (!str_cmp(command, "take"))
    return TRUE;
    if (!str_cmp(command, "join"))
    return TRUE;
    if (!str_cmp(command, "eat"))
    return TRUE;
    if (!str_cmp(command, "drink"))
    return TRUE;
    if (!str_cmp(command, "use"))
    return TRUE;
    if (!str_cmp(command, "unexpose"))
    return TRUE;
    if (!str_cmp(command, "expose"))
    return TRUE;
    if (!str_cmp(command, "giveblood"))
    return TRUE;
    if (!str_cmp(command, "stand"))
    return TRUE;
    if (!str_cmp(command, "mask"))
    return TRUE;
    if (!str_cmp(command, "procedure"))
    return TRUE;
    if (!str_cmp(command, "makeup"))
    return TRUE;
    if (!str_cmp(command, "intro"))
    return TRUE;

    return FALSE;
  }

  /*
  * Thanks to Grodyn for pointing out bugs in this function.
  */
  _DOFUN(do_force) {
    if (IS_IMMORTAL(ch)) {
      char buf[MAX_STRING_LENGTH];
      char arg[MAX_INPUT_LENGTH];
      char arg2[MAX_INPUT_LENGTH];

      argument = one_argument(argument, arg);

      if (arg[0] == '\0' || argument[0] == '\0') {
        send_to_char("Force whom to do what?\n\r", ch);
        return;
      }

      one_argument(argument, arg2);

      if (!str_cmp(arg2, "delete") || !str_prefix(arg2, "mob")) {
        send_to_char("That will NOT be done.\n\r", ch);
        return;
      }

      sprintf(buf, "$n forces you to '%s'.", argument);

      if (!str_cmp(arg, "all")) {
        if (get_trust(ch) < MAX_LEVEL - 3) {
          send_to_char("Not at your level!\n\r", ch);
          return;
        }

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end();) {
          DESCRIPTOR_DATA *d = *it;
          ++it;

          if (d->connected == CON_PLAYING && get_trust(d->character) < get_trust(ch) + 2) {
            act(buf, ch, NULL, d->character, TO_VICT);
            interpret(d->character, argument);
          }
        }
      }
      else if (!str_cmp(arg, "players")) {
        CHAR_DATA *vch;

        if (get_trust(ch) < MAX_LEVEL - 2) {
          send_to_char("Not at your level!\n\r", ch);
          return;
        }

        for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
          vch = *it;
          ++it;

          if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch) && vch->level <= LEVEL_HERO) {
            act(buf, ch, NULL, vch, TO_VICT);
            interpret(vch, argument);
          }
        }
      }
      else if (!str_cmp(arg, "gods")) {
        if (get_trust(ch) < MAX_LEVEL - 2) {
          send_to_char("Not at your level!\n\r", ch);
          return;
        }

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          CHAR_DATA *vch;
          DESCRIPTOR_DATA *d = *it;
          ++it;

          vch = CH(d);
          if (vch == NULL)
          continue;

          if (IS_IMMORTAL(vch) && get_trust(vch) < get_trust(ch)) {
            act(buf, ch, NULL, vch, TO_VICT);
            interpret(vch, argument);
          }
        }
      }
      else {
        CHAR_DATA *victim;

        if ((victim = get_char_world(ch, arg)) == NULL) {
          send_to_char("They aren't here.\n\r", ch);
          return;
        }

        if (victim == ch) {
          send_to_char("Aye aye, right away!\n\r", ch);
          return;
        }

        if (get_trust(victim) >= get_trust(ch) + 2) {
          send_to_char("Do it yourself!\n\r", ch);
          return;
        }

        act(buf, ch, NULL, victim, TO_VICT);
        interpret(victim, argument);
      }

      send_to_char("Ok.\n\r", ch);
      return;
    }
    else {
      char arg1[MSL];
      char arg2[MSL];
      CHAR_DATA *victim;

      argument = one_argument_nouncap(argument, arg1);
      one_argument_nouncap(argument, arg2);

      if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }

      if (in_fight(victim) || room_hostile(victim->in_room)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      if (strcasestr(argument, "Nullius Minuat sanctuarium")) {
        send_to_char("You can't force them to say those words.\n\r", ch);
        return;
      }
      if (strcasestr(argument, "Revocacionem protectione sanctuarium")) {
        send_to_char("You can't force them to say those words.\n\r", ch);
        return;
      }
      else if (strcasestr(argument, "Repeto praesidio sanctuarium")) {
        send_to_char("You can't force them to say those words.\n\r", ch);
        return;
      }

      if (strcasestr(argument, "sleeping") || strcasestr(argument, "sleeps") || strcasestr(argument, "asleep") || strcasestr(argument, "unconcious") || strcasestr(argument, "unconscious")) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      if (is_ghost(ch) && ch->possessing == victim) {
        if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
          send_to_char("You can not muster the will to force your vessel to act.\n\r", ch);
          return;
        }
        else {
          if (ghost_command_makedo(arg2)) {
            interpret(victim, argument);
          }
          else {
            send_to_char("Syntax: Force (person) wear/zip/show/take/eat/drink/use/expose/unexpose/emote/open/close/north/south/east/west/up/down/northeast/northwest/southeast/southwest\n\r", ch);
            refill_ghostpool(ch, GHOST_MANIFESTATION);
            return;
          }
        }
      }

      if (ch != victim) {
        if (in_fight(ch)) {
          send_to_char("You're a bit busy.\n\r", ch);
          return;
        }
        else if (is_helpless(ch)) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }
        else if (!is_helpless(victim)) {
          send_to_char("You'd have to subdue them first.\n\r", ch);
          return;
        }

        if (!IS_NPC(victim) && victim->pcdata->trance > 0 && str_cmp(arg2, "zip")) {
          victim->pcdata->trance = -20;
        }
      }

      if (!str_cmp(arg2, "abort") || !str_cmp(arg2, "abortion")) {
        int wounds = 0, chance = 0;
        if (under_understanding(victim, ch)) {
          send_to_char("Sanctuary protects them.", ch);
          return;
        }
        chance = number_range(1, 100);
        if (victim->pcdata->impregnated > 0) {
          if (ch == victim && chance > 50) {
            wounds = wounds + 1;
          }
          if (chance > 90) {
            wounds = wounds + 1;
          }
          if (victim->wounds >= 1) {
            wounds = wounds + 1;
          }
        }

        if (wounds == 3) {
          real_kill(victim, ch);
          free_string(victim->pcdata->deathcause);
          victim->pcdata->deathcause = str_dup("massive internal hemorrhaging");
        }
        else if (wounds == 2) {
          critplayer(ch, victim);
          wound_char_absolute(victim, 3);
        }
        else if (wounds == 1) {
          wound_char_absolute(victim, 2);
        }
        else {
          wound_char_absolute(victim, 1);
        }

        act("You try to force an abortion on $N.", ch, NULL, victim, TO_CHAR);
        act("$n tries to force an abortion on $N.", ch, NULL, victim, TO_NOTVICT);
        act("$n tries to force an abortion on you.", ch, NULL, victim, TO_VICT);
        if (chance > 10) {
          miscarriage(victim, TRUE);
        }

        return;
      }

      if (!str_cmp(arg2, "checkvirginity")) {
        if (victim->pcdata->penis == 0) {
          if (is_covered(victim, COVERS_GROIN)) {
            send_to_char("You don't have access.  Remove some clothing first.\n\r", ch);
            return;
          }
          act("You check to see if $N's hymen is intact.", ch, NULL, victim, TO_CHAR);
          act("$n probes between $N's legs.", ch, NULL, victim, TO_NOTVICT);
          act("$n probes between your legs.", ch, NULL, victim, TO_VICT);
          if (get_skill(victim, SKILL_VIRGIN) >= 1 && victim->pcdata->hymen_lost == 0) {
            send_to_char("It is.\n\r", ch);
            return;
          }
          else {
            send_to_char("It isn't.\n\r", ch);
            return;
          }
        }
        else {
          send_to_char("How do you plan to do that?\n\r", ch);
          return;
        }
      }

      if (ch == victim) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      captive_command(ch, victim);

      if (!str_cmp(arg2, "givekeys")) {
        forcecarkeys(victim, ch);
        return;
      }

      if (helpless_command_makedo(arg2))
      interpret(victim, argument);
      else {
        send_to_char("Syntax: Force (person) wear/remove/zip/roomtitle/show/give/drop/take/describe/emote/join/eat/drink/use/imbue/expose/unexpose/givekeys\n\r", ch);
        return;
      }

      send_to_char("Done.", ch);
    }
  }

  /**
  * Allows an immortal to go invisible to those
  * above a certain level.  They can go invis to
  * those in the same room, to the wholist, or to
  * both.  If 'room' or 'wholist' are not specified
  * the default is invis to both.  If no level is
  * specified the default is the level of the
  * character.
  *
  * @param ch - the character going wizinvis
  * @param argument - the type and level of invis
  * @author Cameron Matthews-Dickson (Scaelorn)
  */
  
  _DOFUN(do_invis) {
    if (!IS_IMMORTAL(ch)) {
      if (IS_FLAG(ch->act, PLR_INVIS)) {
        REMOVE_FLAG(ch->act, PLR_INVIS);
        send_to_char("You become visible once more.\n\r", ch);
        act("$n suddenly appears as if from thin air.", ch, NULL, NULL, TO_ROOM);
        return;
      }
      if (ch->pcdata->other_power < current_time || ch->pcdata->other_power > current_time + (3600 * 24 * 60)) {
        send_to_char("You do not have the ability to do that.\n\r", ch);
        return;
      }
      act("$n disappears from view.", ch, NULL, NULL, TO_ROOM);
      SET_FLAG(ch->act, PLR_INVIS);
      send_to_char("You disappear from view.\n\r", ch);
      return;
    }

    int level = -1;
    char arg[MSL], arg2[MSL];
    char room[MSL], cha[MSL];
    //    SET bit;
    bool lFound = TRUE;

    /* RT code for taking a level argument */
    argument = one_argument(argument, arg);  // Type or level
    argument = one_argument(argument, arg2); // Level

    if (arg2[0] != '\0')
    level = atoi(arg2);
    else if (arg[0] != '\0' && is_number(arg))
    level = atoi(arg);
    else {
      lFound = FALSE;
      level = ch->level;
    }

    //    SET_INIT(bit);
    cha[0] = '\0';
    sprintf(room, "$n slowly fades into existance.");

    /* Handles 'wizi' and 'wizi <level>' */
    if (arg[0] == '\0' || is_number(arg)) {
      if (!ch->invis_level) {
        sprintf(room, "$n slowly fades into thin air.");
        sprintf(cha, "You slowly vanish into thin air.\n\r");
        SET_FLAG(ch->act, PLR_ROOMINVIS);
        SET_FLAG(ch->act, PLR_WHOINVIS);
      }
      else {
        /*
        * Check to see if the person is just wizinvis in the room, wizinvis on
        * the wholist, or both.  Display the correct messages
        */
        if (IS_FLAG(ch->act, PLR_ROOMINVIS) && !IS_FLAG(ch->act, PLR_WHOINVIS))
        sprintf(cha, "You reveal yourself to those in the room.\n\r");

        /* No message is sent to the room for this action, as the person was
        * visible to those characters.
        */
        if (!IS_FLAG(ch->act, PLR_ROOMINVIS) && IS_FLAG(ch->act, PLR_WHOINVIS)) {
          room[0] = '\0';
          sprintf(cha, "You are now visible to those outside of the room.\n\r");
        }

        if (IS_FLAG(ch->act, PLR_ROOMINVIS) && IS_FLAG(ch->act, PLR_WHOINVIS) && !lFound)
        sprintf(cha, "You fade back into existance.\n\r");

        REMOVE_FLAG(ch->act, PLR_ROOMINVIS);
        REMOVE_FLAG(ch->act, PLR_WHOINVIS);
      }

    }
    else if (!strcmp(arg, "room")) {
      /*
      * Do the code for going wizinvis in a room.  If the PLR_ROOMINVIS flag
      * is already toggled then go vis again.
      */
      if (IS_FLAG(ch->act, PLR_ROOMINVIS)) {
        sprintf(cha, "You reveal yourself to those in the room.\n\r");
        REMOVE_FLAG(ch->act, PLR_ROOMINVIS);
      }
      else {
        sprintf(cha, "You mask yourself from all those in the room.\n\r");
        sprintf(room, "$n slowly fades into thin air.");
        SET_FLAG(ch->act, PLR_ROOMINVIS);
      }

    }
    else if (!strcmp(arg, "wholist")) {
      /*
      * Does the same as above, except it toggles the person invis to those not
      * in the room. ie:  This makes the person invis on the wholist (old
      * incognito).
      */
      if (IS_FLAG(ch->act, PLR_WHOINVIS)) {
        sprintf(cha, "You are now visible to those outside of the room.\n\r");
        REMOVE_FLAG(ch->act, PLR_WHOINVIS);
      }
      else {
        sprintf(cha, "You have masked yourself from those outside of the room.\n\r");
        SET_FLAG(ch->act, PLR_WHOINVIS);
      }

      room[0] = '\0';
    }
    else {
      send_to_char("What type of wizinvis did you want again?\n\rSyntax: wizinvis <type> <level>\n\rTypes: room, wholist\n\r", ch);
    }
    /*    if( bit == 0 )
    {
    send_to_char("What type of wizinvis did you want again?\n\rSyntax: wizinv <type> <level>\n\rTypes: room, wholist\n\r", ch);
    return;
    }
    */
        /*
    if( SET_ISFLAG_SET(ch->act, bit) && !lFound )
    SET_REM_SET(ch->act, bit);
    else
    SET_OR_SET(ch->act, bit);
    */

    ch->invis_level = level;
    /* If no more invis set then set level to 0, else to level found */
    if (!IS_FLAG(ch->act, PLR_WHOINVIS) && !IS_FLAG(ch->act, PLR_ROOMINVIS))
    ch->invis_level = 0;

    /* Display messages to room/character */
    if (cha[0] != '\0')
    send_to_char(cha, ch);
    if (room[0] != '\0')
    act(room, ch, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_holylight) {
    if (IS_NPC(ch))
    return;

    if (IS_FLAG(ch->act, PLR_HOLYLIGHT)) {
      REMOVE_FLAG(ch->act, PLR_HOLYLIGHT);
      send_to_char("Holy light mode off.\n\r", ch);
    }
    else {
      SET_FLAG(ch->act, PLR_HOLYLIGHT);
      send_to_char("Holy light mode on.\n\r", ch);
    }

    return;
  }

  /* prefix command: it will put the string typed on each line typed */

  _DOFUN(do_prefi) {
    send_to_char("You cannot abbreviate the prefix command.\r\n", ch);
    return;
  }

  _DOFUN(do_prefix) {
    char buf[MAX_INPUT_LENGTH];

    if (argument[0] == '\0') {
      if (ch->prefix[0] == '\0') {
        send_to_char("You have no prefix to clear.\r\n", ch);
        return;
      }

      send_to_char("Prefix removed.\r\n", ch);
      free_string(ch->prefix);
      ch->prefix = str_dup("");
      return;
    }

    if (ch->prefix[0] != '\0') {
      sprintf(buf, "Prefix changed to %s.\r\n", argument);
      free_string(ch->prefix);
    }
    else {
      sprintf(buf, "Prefix set to %s.\r\n", argument);
    }

    ch->prefix = str_dup(argument);
  }

  _DOFUN(do_copyover) {
    char arg1[MAX_INPUT_LENGTH];

    one_argument(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Syntax:  Copyover Now\n\r         Copyover Stop\n\r         Copyover #\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "now")) {
      exec_copyover(ch);
      return;
    }

    if (!str_cmp(arg1, "stop")) {
      if (pulse_copyover >= 0) {
        pulse_copyover = -1;
        mudkiller = NULL;
        do_echo(ch, "Copyover has been stopped, resume normal activities.\n\r");
        return;
      }
      else {
        send_to_char("There is no copyover countdown in progress.\n\r", ch);
        return;
      }
    }
    if (is_number(arg1)) {
      if (atoi(arg1) < 1 || atoi(arg1) > 10) {
        send_to_char("Time range is between 1 and 10 minutes.\n\r", ch);
        return;
      }
      else {
        pulse_copyover = atoi(arg1);
        mudkiller = ch;
        check_copyover();
        return;
      }
    }
    else {
      send_to_char("Syntax:  Copyover Now\n\r         Copyover Stop\n\r         Copyover #\n\r", ch);
      return;
    }

    return;
  }

  void exec_copyover(CHAR_DATA *ch) {
    FILE *fp;
    char buf[100], buf2[100];
    extern int port, control; /* db.c */
    int temp = 0;

    fp = fopen(COPYOVER_FILE, "w");

    if (!fp) {
      send_to_char("Copyover file not writeable, aborted.\n\r", ch);
      logfi("Could not write to copyover file: %s", COPYOVER_FILE);
      perror("do_copyover:fopen");
      return;
    }

    save_ground_objects();

    /* Consider changing all saved areas here, if you use OLC */
    do_function(ch, &do_asave, "auto");
    do_function(ch, &do_force, "all save");
    do_function(ch, &do_savehelps, "");

    /* For each playing descriptor, save its state */
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end();) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *och = CH(d);
      ++it; /* We delete from the list , so need to save this */

      if (d->ipid > 0) /* Kill any old resolve */
      {
#if defined(WIN32)
        _cwait(&temp, d->ipid, _WAIT_CHILD);
#else
        kill(d->ipid, SIGKILL);
        waitpid(d->ipid, &temp, 0);
#endif
        close_socket(d->ifd);
      }

      if (!d->character || (d->connected > CON_PLAYING && d->connected != CON_CREATION)) /* drop those logging on */
      {
        write_to_descriptor(d->descriptor, buf, 0);
        write_to_descriptor(
        d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
        close_desc(d); /* throw'em out */
      }
      else {
        fprintf(fp, "%d %s %s %s\n", d->descriptor, och->name, d->host, d->hostip);
        och = CH(d);
        if (och != NULL)
        save_char_obj(och, TRUE, FALSE);

        sprintf(buf, "\n\rHaven is rebooting.\n\r");

        write_to_descriptor(d->descriptor, buf, 0);
      }
    }
    fprintf(fp, "-1\n");
    fclose(fp);

    /* exec - descriptors are inherited */
    sprintf(buf, "%d", port);
    sprintf(buf2, "%d", control);

#if defined(WIN32)
    _execl(EXE_FILE, "rom", buf, "copyover", buf2, (char *)NULL);
#else
    execl(EXE_FILE, "haven", buf, "copyover", buf2, (char *)NULL);
#endif

    /* Failed - sucessful exec will not return */
    perror("do_copyover: execl");
    send_to_char("Copyover FAILED!\n\r", ch);
  }

  void auto_copyover() {
    FILE *fp;
    char buf[100], buf2[100];
    extern int port, control; /* db.c */
    int temp = 0;

    fp = fopen(COPYOVER_FILE, "w");

    if (!fp) {
      logfi("Could not write to copyover file: %s", COPYOVER_FILE);
      perror("do_copyover:fopen");
      return;
    }

    save_ground_objects();

    CHAR_DATA *victim;
    /* Consider changing all saved areas here, if you use OLC */
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      if ((*it)->connected == CON_PLAYING && (victim = (*it)->character) != NULL && !IS_NPC(victim) && victim->in_room != NULL) {

        do_function(victim, &do_save, "");
      }
    }

    /* For each playing descriptor, save its state */
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end();) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *och = CH(d);
      ++it; /* We delete from the list , so need to save this */

      if (d->ipid > 0) /* Kill any old resolve */
      {
#if defined(WIN32)
        _cwait(&temp, d->ipid, _WAIT_CHILD);
#else
        kill(d->ipid, SIGKILL);
        waitpid(d->ipid, &temp, 0);
#endif
        close_socket(d->ifd);
      }

      if (!d->character || (d->connected > CON_PLAYING && d->connected != CON_CREATION)) /* drop those logging on */
      {
        write_to_descriptor(d->descriptor, buf, 0);
        write_to_descriptor(
        d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
        close_desc(d); /* throw'em out */
      }
      else {
        fprintf(fp, "%d %s %s %s\n", d->descriptor, och->name, d->host, d->hostip);
        och = CH(d);
        if (och != NULL)
        save_char_obj(och, TRUE, FALSE);

        sprintf(buf, "\n\rHaven is rebooting automatically.\n\r");

        write_to_descriptor(d->descriptor, buf, 0);
      }
    }
    fprintf(fp, "-1\n");
    fclose(fp);

    /* exec - descriptors are inherited */
    sprintf(buf, "%d", port);
    sprintf(buf2, "%d", control);

#if defined(WIN32)
    _execl(EXE_FILE, "rom", buf, "copyover", buf2, (char *)NULL);
#else
    execl(EXE_FILE, "haven", buf, "copyover", buf2, (char *)NULL);
#endif

    /* Failed - sucessful exec will not return */
    perror("do_copyover: execl");
  }

  _DOFUN(do_immshow) {
    char arg[MIL], buf[MSL];
    struct show_struct {
      const char *name;
      DO_FUN *func;
    };

    const struct show_struct show_table[] = {
      {"sysdata", &do_showsysdata}, {NULL, NULL}, };

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Syntax: show <type> <argument>\n\r", ch);
      sprintf(buf, "Types:");
      for (int cmd = 0; show_table[cmd].name != NULL; cmd++) {
        sprintf(arg, " %s", show_table[cmd].name);
        strcat(buf, arg);
      }
      send_to_char(buf, ch);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (int cmd = 0; show_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(arg, show_table[cmd].name))
      (*show_table[cmd].func)(ch, argument);
    }

    return;
  }

  _DOFUN(do_home) {

    if(ch->pcdata->summary_type == SUMMARY_VICTIMIZER)
    {
      CHAR_DATA *target = ch->pcdata->summary_target;
      if(target == NULL)
      return;
      if(!str_cmp(argument, "yes"))
      {
        ch->pcdata->summary_home = 1;
        if(safe_strlen(ch->pcdata->summary_conclusion) < 2)
        {
          send_to_char("You will send them home, use conclusion (emote text) to write the conclusion of your summary.\n\r", ch);
          return;
        }
        process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_conclusion);
        ch->pcdata->summary_stage = 0;
        target->pcdata->summary_stage = 0;
        ch->pcdata->summary_type = 0;
        target->pcdata->summary_type = 0;
        char_from_room(target);
        char_to_room(target, ch->in_room);
        if(ch->pcdata->summary_blood == 1)
        {
          bloodbag_transfer(target, ch);
        }
        if(target->pcdata->summary_intel == 1)
        {
          trans_intel(target, ch);
        }
        handout_lifeforce_summary(target, target->pcdata->summary_lifeforce);
        if(ch->pcdata->summary_home == 1)
        {
          ROOM_INDEX_DATA *fleeroom = get_fleeroom(target, NULL);
          char_from_room(target);
          char_to_room(target, fleeroom);
          if (IS_FLAG(ch->act, PLR_BOUND))
          REMOVE_FLAG(ch->act, PLR_BOUND);
          if (IS_FLAG(ch->act, PLR_BOUNDFEET))
          REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
        }
        return;

      }
      else if(!str_cmp(argument, "no"))
      {
        ch->pcdata->summary_home = 2;
        if(safe_strlen(ch->pcdata->summary_conclusion) < 2)
        {
          send_to_char("You will not send them home, use conclusion (emote text) to write the conclusion of your summary.\n\r", ch);
          return;
        }

        process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_conclusion);
        ch->pcdata->summary_stage = 0;
        target->pcdata->summary_stage = 0;
        ch->pcdata->summary_type = 0;
        target->pcdata->summary_type = 0;
        char_from_room(target);
        char_to_room(target, ch->in_room);
        if(ch->pcdata->summary_blood == 1)
        {
          bloodbag_transfer(target, ch);
        }
        if(target->pcdata->summary_intel == 1)
        {
          trans_intel(target, ch);
        }
        handout_lifeforce_summary(target, target->pcdata->summary_lifeforce);
        if(ch->pcdata->summary_home == 1)
        {
          ROOM_INDEX_DATA *fleeroom = get_fleeroom(target, NULL);
          char_from_room(target);
          char_to_room(target, fleeroom);
          if (IS_FLAG(ch->act, PLR_BOUND))
          REMOVE_FLAG(ch->act, PLR_BOUND);
          if (IS_FLAG(ch->act, PLR_BOUNDFEET))
          REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
        }
        return;
      }
    }


    char buf[10];
    sprintf(buf, "%d", ch->pcdata->home);

    if (IS_IMMORTAL(ch)) {
      do_goto(ch, buf);
      return;
    }
    else if (IS_FLAG(ch->act, PLR_GM)) {
      act("$n heads back to the home of the SRs.", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
      char_to_room(ch, get_room_index(GMHOME));
      if (IS_FLAG(ch->comm, COMM_RUNNING))
      REMOVE_FLAG(ch->comm, COMM_RUNNING);

      send_to_char("You return home.\n\r", ch);
      ch->pcdata->vote_timer = 0;
    }
    else if (higher_power(ch)) {
      if (higher_power(ch) && power_bound(ch)) {

        send_to_char("You cannot leave the binding circle.\n\r", ch);
        return;
      }

      act("$n returns to their domain.", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
      char_to_room(ch, get_room_index(GMHOME));
      if (IS_FLAG(ch->comm, COMM_RUNNING))
      REMOVE_FLAG(ch->comm, COMM_RUNNING);
    }
  }

  _DOFUN(do_homeset) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int homeset;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Syntax: Homeset <vnum>\n\r", ch);
      return;
    }

    homeset = atoi(arg);
    ch->pcdata->home = homeset;
    sprintf(buf, "Your home is now at %d.\n\r", homeset);
    send_to_char(buf, ch);
    return;
  }

  // Written by takeda (takeda@mathlab.sunysb.edu)
  _DOFUN(do_avator) {
    char buf[MSL];
    char arg1[MIL];
    OBJ_DATA *obj_next;
    OBJ_DATA *obj;
    int level;
    int iLevel;

    argument = one_argument(argument, arg1);

    if (ch->pcdata->true_level == 0) {
      ch->pcdata->true_level = ch->level;
    }

    if (!str_cmp(arg1, "return")) {
      send_to_char("Returning to your normal level.\n\r", ch);
      for (iLevel = ch->level; iLevel <= ch->pcdata->true_level; iLevel++) {
        advance_level(ch, TRUE);
      }
      ch->level = ch->pcdata->true_level;
      return;
    }

    if (arg1[0] == '\0' || !is_number(arg1)) {
      send_to_char("Syntax: avator <level>.\n\r", ch);
      send_to_char("Syntax: avator return.\n\r", ch);
      return;
    }

    if (IS_NPC(ch)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    if ((level = atoi(arg1)) < 1 || level > ch->pcdata->true_level) {
      sprintf(buf, "Level must be 1 to %d.\n\r", ch->pcdata->true_level);
      send_to_char(buf, ch);
      return;
    }

    if (level > ch->trust) {
      send_to_char("Limited to your normal level.\n\r", ch);
      sprintf(buf, "Your trust is %d.\n\r", ch->level);
      send_to_char(buf, ch);
      return;
    }

    // Your trust stays so you will have all imm commands
    if (ch->trust == 0)
    ch->trust = ch->level;

    if (level <= ch->level) {

      send_to_char("Lowering a player's level!\n\r", ch);
      send_to_char("**** OOOHHHHHHH NNNNOOO ****\n\r", ch);
      ch->level = 1;
      ch->hit = max_hp(ch);
      advance_level(ch, TRUE);
    }
    else {
      send_to_char("Raising a player's level!\n\r", ch);
      send_to_char("**** OOOOHHHHH YYYEEESSS ****\n\r", ch);
    }

    for (iLevel = ch->level; iLevel < level; iLevel++) {
      ch->level += 1;
      advance_level(ch, TRUE);
    }

    sprintf(buf, "You are now level %d.\n\r", ch->level);
    send_to_char(buf, ch);
    ch->level--;
    ch->level++;

    if (ch->level < 103) {
      for (obj = ch->carrying; obj; obj = obj_next) {
        obj_next = obj->next_content;
        if (obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj))
        remove_obj(ch, obj->wear_loc, TRUE);
      }
    }
    save_char_obj(ch, FALSE, FALSE);
    return;
  }

  /* -----------------------------------------------------------------------
*/
  _DOFUN(do_pload) {
    DESCRIPTOR_DATA *d;
    bool isChar = FALSE;
    char name[MAX_INPUT_LENGTH];

    if (argument[0] == '\0') {
      send_to_char("Load who?\n\r", ch);
      return;
    }

    argument[0] = UPPER(argument[0]);
    one_argument(argument, name);

    /* Dont want to load a second copy of a player who's allready online! */
    if (get_char_world(ch, name) != NULL) {
      send_to_char("That person is allready connected!\n\r", ch);
      return;
    }

    log_string("DESCRIPTOR: pload");

    d = new_descriptor();
    isChar = load_char_obj(d, name); /* char pfile exists? */

    if (!isChar) {
      send_to_char("Load Who? Are you sure? I cant seem to find them.\n\r", ch);
      return;
    }

    d->character->desc = NULL;
    char_list.push_front(d->character);
    d->connected = CON_PLAYING;

    /* bring player to imm */
    if (ch->in_room != NULL)
    char_to_room(d->character, ch->in_room); /* put in room imm is in */

    act("$n has pulled $N from the ether!", ch, NULL, d->character, TO_ROOM);
  }

  /**
  * Function: do_punload
  * Descr   : Returns a player, previously 'ploaded' back to the void from
  *           whence they came.  This does not work if the player is actually
  *           connected.
  * Syntax  : punload (who)
  * @version v1.0 12/97
  * @author Gary McNickle <gary@dharvest.com>
  */
  _DOFUN(do_punload) {
    CHAR_DATA *victim;
    char who[MAX_INPUT_LENGTH];

    argument = one_argument(argument, who);

    if ((victim = get_char_world(ch, who)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    /** Person is legitametly logged on... was not ploaded.*/
    if (victim->desc != NULL) {
      send_to_char("I dont think that would be a good idea...\n\r", ch);
      return;
    }

    save_char_obj(victim, FALSE, FALSE);
    real_quit(victim);

    act("$n has released $N back to the ether.", ch, NULL, victim, TO_ROOM);
  }

  _DOFUN(do_awaken) {
    Buffer outbuf;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    bool isChar = FALSE;
    char name[MAX_INPUT_LENGTH];

    if (argument[0] == '\0') {
      send_to_char("Load who?\n\r", ch);
      return;
    }

    argument[0] = UPPER(argument[0]);
    one_argument(argument, name);

    /* Dont want to load a second copy of a player who's allready online! */
    if (get_char_world(ch, name) != NULL) {
      send_to_char("That person is allready connected!\n\r", ch);
      return;
    }
    log_string("DESCRIPTOR: pload 2");

    d = new_descriptor();
    isChar = load_char_obj(d, name); /* char pfile exists? */

    if (!isChar) {
      send_to_char("Load Who? Are you sure? I cant seem to find them.\n\r", ch);
      return;
    }
    d->character->desc = NULL;
    char_list.push_front(d->character);
    d->connected = CON_PLAYING;
    victim = d->character;
    if (IS_NPC(victim)) {
      send_to_char("\n\rYou can't target mobiles!\n\r", ch);
      return;
    }
    if (current_time - victim->lastlogoff > 75) {
      send_to_char("You can't awaken them.\n\r", ch);
      extract_char(victim, TRUE);
      return;
    }
    if ((ch->in_room->vnum < 21600 || ch->in_room->vnum > 21803) && (victim->in_room->vnum >= 21600 && victim->in_room->vnum <= 21803)) {
      send_to_char("They aren't here.\n\r", ch);
      extract_char(victim, TRUE);
      WAIT_STATE(ch, 10 * PULSE_PER_SECOND);
      return;
    }
    if (ch->in_room->vnum != victim->in_room->vnum && (victim->in_room->vnum < 21600 || victim->in_room->vnum > 21803)) {
      send_to_char("They aren't here.\n\r", ch);
      extract_char(victim, TRUE);
      return;
    }
    if (ch->in_room != NULL)
    char_to_room(victim, ch->in_room); /* put in room imm is in */
    act("$n awakens.", victim, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_squish) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    long squish_time;
    struct tm *l_time;
    FILE *warning_file;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if ((arg1[0] == '\0') || (arg2[0] == '\0') || !is_number(arg2)) {
      send_to_char("Syntax: squish <character> <# of minutes>\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("That person is not here.\n\r", ch);
      return;
    }

    if (victim == ch) {
      send_to_char("You really want to squish yourself?!?\n\r", ch);
      return;
    }

    if (!IS_IMP(ch) && IS_IMMORTAL(victim)) {
      send_to_char("Only the IMPs can squish other immortals.\n\r", ch);
      return;
    }

    if (ch->level == victim->level) {
      send_to_char("You can't squish them.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("NPCs can't be squished.\n\r", ch);
      return;
    }

    if (atoi(arg2) > 60) {
      send_to_char("Squishes can't be longer than 60 minutes.\n\r", ch);
      return;
    }

    if ((warning_file = fopen("warnings", "a")) == NULL) {
      log_string("Problem opening warning file");
      return;
    }

    l_time = localtime(&current_time);

    squish_time = l_time->tm_yday * (24 * 60) + l_time->tm_hour * 60 +
    l_time->tm_min + atoi(arg2);

    sprintf(buf, "%s has been squished for the next %d minutes.\n\r", victim->name, atoi(arg2));
    send_to_char(buf, ch);

    if (!can_see(victim, ch)) {
      sprintf(buf, "You have been squished for the next %d minutes.\n\r", atoi(arg2));
    }
    else {
      sprintf(buf, "%s has squished you for the next %d minutes.\n\r", ch->name, atoi(arg2));
    }
    send_to_char(buf, victim);

    sprintf(buf, "%s squished %s for %d minutes.\n\r\n\r", ch->name, victim->name, atoi(arg2));
    fclose(warning_file);

    victim->pcdata->squish = squish_time;
    SET_FLAG(victim->act, PLR_DENY);
    save_char_obj(victim, TRUE, FALSE);
    real_quit(victim);

    return;
  }

  _DOFUN(do_rpclear) {
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Syntax: rpclear [character]\n\r", ch);
      return;
    }
    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("That person is not here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPCs.", ch);
      return;
    }

    send_to_char("Done.\n\r", ch);
  }

  /* Useful Command. Not the snippet version.   Author: Michael MacLeod (Kuval) */
  _DOFUN(do_secset) {
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
      send_to_char("Syntax: secset [character] [security].\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("That person is not here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("You can't raise their security.", ch);
      return;
    }

    value = atoi(arg2);

    if ((value <= 0) || (value > 9)) {
      send_to_char("Security range is from 1 to 9.\n\r", ch);
      return;
    }

    if (ch == victim) {
      printf_to_char(ch, "% your security.\n\r", victim->pcdata->security > value ? "Lowering" : "Raising");
      victim->pcdata->security = value;
      return;
    }

    if (((victim->pcdata->security >= ch->pcdata->security) || (victim->level >= ch->level)) && !IS_IMP(ch)) {
      send_to_char("You cannot set that on that person!\n\r", ch);
      return;
    }

    victim->pcdata->security = value;

    sprintf(buf, "You set %s's security to %d.\n\r", victim->short_descr, value);
    send_to_char(buf, ch);

    sprintf(buf, "%s set your security to %d.\n\r", ch->short_descr, value);
    send_to_char(buf, victim);

    return;
  }

  /* Added by Kuval 2/00 */
  _DOFUN(do_email) {
    CHAR_DATA *victim;
    char arg1[MAX_STRING_LENGTH];

    if (argument[0] == '\0') {
      printf_to_char(
      ch, "Syntax: email <name> - to see a character's e-mail.\n\rSyntax: email <name> <string> - to change their e-mail.\n\r");
      return;
    }

    argument = one_argument(argument, arg1);
    victim = get_char_world(ch, arg1);
    if (victim == NULL) {
      printf_to_char(ch, "They're not here.\n\r");
      return;
    }

    if (IS_NPC(victim)) {
      printf_to_char(ch, "Not on NPC's.\n\r");
      return;
    }

    if (argument[0] != '\0') {
      free_string(victim->pcdata->email);
      victim->pcdata->email = str_dup(argument);
      printf_to_char(ch, "%s's email is now %s.\n\r", victim->name, victim->pcdata->email);
    }
    else {
      printf_to_char(ch, "%s's email is %s.\n\r", victim->name, victim->pcdata->email);
    }

    return;
  }

  bool is_name_builder(char *str, char *namelist) {
    char list[MAX_INPUT_LENGTH];
    char *part;

    if (safe_strlen(namelist) > MAX_INPUT_LENGTH) {
      bugf("Error: name list is longer than MAX_INPUT_LENGTH");
      return FALSE;
    }

    strcpy(list, namelist);
    part = strtok(list, ", ");
    while (part) {
      if (!str_cmp(str, part))
      return TRUE;
      part = strtok(NULL, ", ");
    }

    return FALSE;
  }

  /* Expand the name of a character into a string that identifies THAT
  character within a room. E.g. the second 'guard' -> 2. guard
  */
  const char *name_expand(CHAR_DATA *ch) {
    int count = 1;
    char name[MAX_INPUT_LENGTH]; /*  HOPEFULLY no mob has a name longer than THAT */

    static char outbuf[MAX_INPUT_LENGTH];

    if (!IS_NPC(ch))
    return ch->name;

    one_argument(ch->name, name); /* copy the first word into name */

    if (!name[0]) /* weird mob .. no keywords */
    {
      strcpy(outbuf, ""); /* Do not return NULL, just an empty buffer */
      return outbuf;
    }

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end() && *it != ch; ++it) {
      if (is_name(name, (*it)->name))
      count++;
    }

    sprintf(outbuf, "%d.%s", count, name);
    return outbuf;
  }

  _DOFUN(do_for) {
    char range[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    bool fGods = FALSE, fMortals = FALSE, fMobs = FALSE, fEverywhere = FALSE, found;
    ROOM_INDEX_DATA *room, *old_room;
    CHAR_DATA *p;
    int i;

    argument = one_argument(argument, range);

    if (!range[0] || !argument[0]) /* invalid usage? */
    {
      do_function(ch, &do_help, "for");
      return;
    }

    if (!str_prefix("quit", argument)) {
      send_to_char("Are you trying to crash the MUD or something?\n\r", ch);
      return;
    }

    if (!str_cmp(range, "all")) {
      fMortals = TRUE;
      fGods = TRUE;
    }
    else if (!str_cmp(range, "gods"))
    fGods = TRUE;
    else if (!str_cmp(range, "mortals"))
    fMortals = TRUE;
    else
    do_help(ch, "for"); /* show syntax */

    /* do not allow # to make it easier */
    if (strchr(argument, '#')) /* replace # ? */
    {
      for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
        p = *it;
        ++it;
        found = FALSE;

        if (IS_NPC(p) && fMobs)
        found = TRUE;
        else if (!IS_NPC(p) && p->level >= LEVEL_IMMORTAL && fGods)
        found = TRUE;
        else if (!IS_NPC(p) && p->level < LEVEL_IMMORTAL && fMortals)
        found = TRUE;

        /* It looks ugly to me.. but it works :) */
        if (found) /* p is 'appropriate' */
        {
          char *pSource = argument; /* head of buffer to be parsed */
          char *pDest = buf;        /* parse into this */

          while (*pSource) {
            if (*pSource == '#') /* Replace # with name of target */
            {
              const char *namebuf = name_expand(p);

              if (namebuf)       /* in case there is no mob name ?? */
              while (*namebuf) /* copy name over */
              *(pDest++) = *(namebuf++);

              pSource++;
            }
            else
            *(pDest++) = *(pSource++);
          }              /* while */
          *pDest = '\0'; /* Terminate */

          /* Execute */
          old_room = ch->in_room;
          char_from_room(ch);
          char_to_room(ch, p->in_room);
          interpret(ch, buf);
          char_from_room(ch);
          char_to_room(ch, old_room);
        }  /* if found */
      }    /* for every char */
    }
    else /* just for every room with the appropriate people in it */
    {
      for (i = 0; i < MAX_KEY_HASH; i++) /* run through all the buckets */
      for (room = room_index_hash[i]; room; room = room->next) {
        found = FALSE;

        /* Anyone in here at all? */
        if (fEverywhere) /* Everywhere executes always */
        found = TRUE;
        else if (room->people->empty()) /* Skip it if room is empty */
        continue;

        /* Check if there is anyone here of the requried type */
        /* Stop as soon as a match is found or there are no more ppl in room */
        for (CharList::iterator it = room->people->begin();
        it != room->people->end(); ++it) {
          CHAR_DATA *p = *it;
          if (p == ch) /* do not execute on oneself */
          continue;

          if (IS_NPC(p) && fMobs)
          found = TRUE;
          else if (!IS_NPC(p) && (p->level >= LEVEL_IMMORTAL) && fGods)
          found = TRUE;
          else if (!IS_NPC(p) && (p->level <= LEVEL_IMMORTAL) && fMortals)
          found = TRUE;
        } /* for everyone inside the room */

        if (found && !room_is_private(
              room)) /* Any of the required type here ANDroom not private? */
        {
          /* This may be ineffective. Consider moving character out ofold_room
          once at beginning of command then moving back at the end.
          This however, is more safe?
          */
          old_room = ch->in_room;
          char_from_room(ch);
          char_to_room(ch, room);
          interpret(ch, argument);
          char_from_room(ch);
          char_to_room(ch, old_room);
        } /* if found */
      }   /* for every room in a bucket */
    }       /* if strchr */
  } /* do_for */

  _DOFUN(do_settime) {
    int hour;

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax: settime x\n\r        (where x = hour)\n\r", ch);
      return;
    }

    hour = atoi(argument);

    if (hour == -1) {
      time_info.day--;
      return;
    }

    time_info.hour = hour;
  }

  _DOFUN(do_cedit) {
    CMD_TYPE *command;
    char arg1[MIL];
    char arg2[MIL];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0') {
      send_to_char("Syntax: cedit save cmdtable\n\r", ch);
      send_to_char("Syntax: cedit <command> create [code]\n\r", ch);
      send_to_char("Syntax: cedit <command> delete\n\r", ch);
      send_to_char("Syntax: cedit <command> show\n\r", ch);
      send_to_char("Syntax: cedit <command> raise\n\r", ch);
      send_to_char("Syntax: cedit <command> lower\n\r", ch);
      send_to_char("Syntax: cedit <command> list\n\r", ch);
      send_to_char("Syntax: cedit <command> [field]\n\r", ch);
      send_to_char("\n\rField being one of:\n\r", ch);
      send_to_char("  level position log code flags department\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "save") && !str_cmp(arg2, "cmdtable")) {
      save_commands();
      send_to_char("Saved.\n\r", ch);
      return;
    }

    command = find_command(arg1);

    if (!str_cmp(arg2, "create")) {
      if (command) {
        send_to_char("That command already exists!\n\r", ch);
        return;
      }

      command = new_command();
      command->name = str_dup(arg1);
      command->level = get_trust(ch);
      command->department = DEP_PLAYER;
#if !defined(_WIN32)
      void *handle;
      handle = dlopen(NULL, RTLD_LAZY);
#else
      HMODULE handle;
      handle = LoadLibrary(APPNAME);
#endif
      if (!handle) {
        send_to_char("Error loading command.\n\r", ch);
      }

      if (*argument)
      one_argument(argument, arg2);
      else
      sprintf(arg2, "do_%s", arg1);

      command->lookup_name = str_dup(arg2);
      add_command(command, handle);
      send_to_char("Command added.\n\r", ch);

      if (command->do_fun == NULL)
      printf_to_char(ch, "Code %s not found.  Set to no code.\n\r", arg2);
      return;
    }

    if (!command) {
      send_to_char("Command not found.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "show")) {
      command->show = atoi(argument);
      return;
    }
    if (arg2[0] == '\0') {
      printf_to_char(ch, "Command:  %s\n\rLevel:    %d\n\rPosition: %d\n\rLog:      %d\n\rCode:     %s\n\rShow:	   %d\n\rFlags:      \n\rDep:      %d\n\r", command->name, command->level, command->position, command->log, command->lookup_name, command->show, command->department);
      if (command->userec.num_uses)
      send_timer(&command->userec, ch);
      return;
    }

    if (!str_cmp(arg2, "raise")) {
      CMD_TYPE *tmp, *tmp_next;
      int hash = command->name[0] % MAX_COMMAND_HASH;

      if ((tmp = command_hash[hash]) == command) {
        send_to_char("That command is already at the top.\n\r", ch);
        return;
      }

      if (tmp->next == command) {
        command_hash[hash] = command;
        tmp_next = tmp->next;
        tmp->next = command->next;
        command->next = tmp;
        printf_to_char(ch, "Moved %s above %s.\n\r", command->name, command->next->name);
        return;
      }

      for (; tmp; tmp = tmp->next) {
        tmp_next = tmp->next;
        if (tmp_next->next == command) {
          tmp->next = command;
          tmp_next->next = command->next;
          command->next = tmp_next;
          printf_to_char(ch, "Moved %s above %s.\n\r", command->name, command->next->name);
          return;
        }
      }
      send_to_char("ERROR -- Not Found!\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "lower")) {
      CMD_TYPE *tmp, *tmp_next;
      int hash = command->name[0] % MAX_COMMAND_HASH;

      if (command->next == NULL) {
        send_to_char("That command is already at the bottom.\n\r", ch);
        return;
      }
      tmp = command_hash[hash];
      if (tmp == command) {
        tmp_next = tmp->next;
        command_hash[hash] = command->next;
        command->next = tmp_next->next;
        tmp_next->next = command;

        printf_to_char(ch, "Moved %s below %s.\n\r", command->name, tmp_next->name);
        return;
      }
      for (; tmp; tmp = tmp->next) {
        if (tmp->next == command) {
          tmp_next = command->next;
          tmp->next = tmp_next;
          command->next = tmp_next->next;

          tmp_next->next = command;

          printf_to_char(ch, "Moved %s below %s.\n\r", command->name, tmp_next->name);
          return;
        }
      }
      send_to_char("ERROR -- Not Found!\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "list")) {
      CMD_TYPE *tmp;
      int hash = command->name[0] % MAX_COMMAND_HASH;

      printf_to_char(ch, "Priority placement for [%s]:\n\r", command->name);
      for (tmp = command_hash[hash]; tmp; tmp = tmp->next) {
        if (tmp == command)
        printf_to_char(ch, "`G  %s`x\n\r", tmp->name);
        else
        printf_to_char(ch, "  %s\n\r", tmp->name);
      }
      return;
    }
    /*
    if ( !str_cmp( arg2, "delete" ) )
    {
    unlink_command( command );
    free_command( command );
    send_to_char( "Deleted.\n\r", ch );
    return;
    }
    */

    /*  The functions for this are missing - Discordance
    if ( !str_cmp( arg2, "code" ) )
    {
    DO_FUN *fun = skill_function( argument );

    if ( fun == skill_notfound )
    {
    send_to_char( "Code not found.\n\r", ch );
    return;
    }
    command->do_fun = fun;
    send_to_char( "Done.\n\r", ch );
    return;
    }
    */

    if (!str_cmp(arg2, "level")) {
      int level = atoi(argument);
      if ((level < 0 || level > get_trust(ch))) {
        send_to_char("Level out of range.\n\r", ch);
        return;
      }
      command->level = level;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "log")) {
      int log = atoi(argument);

      if (log < 0 || log > LOG_NEVER) {
        send_to_char("Log out of range.\n\r", ch);
        return;
      }
      command->log = log;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "position")) {
      int position = atoi(argument);

      if (position < 0 || position > POS_STANDING) {
        send_to_char("Position out of range.\n\r", ch);
        return;
      }
      command->position = position;
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "department")) {
      int department = atoi(argument);

      if (department < 0 || department >= DEP_MAX) {
        send_to_char("Department out of range.\n\r0 = Player\n\r1 = Builder\n\r2 = Admin.\n\r", ch);
        return;
      }
      command->department = department;
      send_to_char("Done.\n\r", ch);
      return;
    }
    /*
    if ( !str_cmp( arg2, "flags" ) )
    {
    int flag;
    if ( is_number(argument))
    flag = atoi ( argument );
    else
    flag = get_cmdflag ( argument );
    if ( flag < 0 || flag >= 32 )
    {
    if ( is_number( argument ) )
    ch_printf (ch, "Invalid flag: range is from 0 to 31.\n");
    else
    ch_printf (ch, "Unknown flag %s.\n", argument );
    return;
    }

    TOGGLE_BIT( command->flags, 1 << flag );
    send_to_char( "Done.\n\r", ch );
    return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
    bool relocate;

    one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
    send_to_char( "Cannot clear name field!\n\r", ch );
    return;
    }
    if ( arg1[0] != command->name[0] )
    {
    unlink_command( command );
    relocate = TRUE;
    }
    else
    relocate = FALSE;
    if ( command->name )
    {
    free( command->name );
    command->name = NULL;
    //DISPOSE( command->name );
    }
    command->name = str_dup( arg1 );
    if ( relocate )
    add_command( command );
    send_to_char( "Done.\n\r", ch );
    return;
    }
    */

    /* display usage message */
    do_cedit(ch, "");
  }

  _DOFUN(do_mudstat) {
    extern bool newlock;
    extern bool wizlock;

    printf_to_char(ch, "Current MUD Status\n\r------------------\n\rNewlock: %s\n\rWizlock: %s\n\r", newlock ? "`GON`x" : "`ROFF`x", wizlock ? "`GON`x" : "`ROFF`x");
  }

  _DOFUN(do_savedata) {
    char arg[MIL], buf[MSL];
    struct save_struct {
      const char *name;
      void (*func)();
    };

    const struct save_struct save_table[] = {{NULL, NULL}};

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Syntax: savedata <type> <argument>\n\r", ch);
      sprintf(buf, "Types:");
      for (int cmd = 0; save_table[cmd].name != NULL; cmd++) {
        sprintf(arg, " %s", save_table[cmd].name);
        strcat(buf, arg);
      }
      send_to_char(buf, ch);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (int cmd = 0; save_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(arg, save_table[cmd].name))
      (*save_table[cmd].func)();
    }

    return;
  }

  ROOM_INDEX_DATA *random_location(CHAR_DATA *ch, char *loc, int *mappos, int *x, int *y, bool *vmap, bool *pass) {
    ROOM_INDEX_DATA *room = NULL;

    *vmap = FALSE;

    if (!str_cmp(loc, "area")) {
      room = get_room_index(
      number_range(ch->in_room->area->min_vnum, ch->in_room->area->max_vnum));
      *mappos = -1;
      *x = -1;
      *y = -1;
      *pass = TRUE;
    }
    else if (!str_cmp(loc, "world")) {
      room = get_room_index(number_range(0, 32000));
      *mappos = -1;
      *x = -1;
      *y = -1;
      *pass = TRUE;
      *vmap = FALSE;
    }

    return room;
  }

  /*
  * Allows an imm to scatter morts, mobs or objs
  * throughout the world, vmap or the current area.
  * For mob and objects it will scatter <num> instances
  * of the specified vnum.  If the vnum argument is 'mort'
  * then it will scatter the morts in the current room.
  *
  * Syntax: scatter <num> <type> <vnum | mort> <location>
  * 	Num: 1-50
  *	Types: Object, Mobile
  *	Vnum: <vnum> or 'mort'
  *	Locations: Area, Vmap, World
  *
  * @param ch character doing the scattering
  * @param argument all the arguments needed/
  * @author Cameron Matthews-Dickson(Scaelorn)
  */
  _DOFUN(do_scatter) {
    char vnumc[MIL], type[MIL], loc[MIL], numc[MIL];
    int vnum, num, map, x, y, z = 0, cnt = 0;
    ROOM_INDEX_DATA *room = NULL;
    MOB_INDEX_DATA *pMobIndex = NULL;
    OBJ_INDEX_DATA *pObjIndex = NULL;
    CHAR_DATA *mob = NULL;
    OBJ_DATA *obj = NULL;
    bool pass, vmap;

    argument = one_argument(argument, numc);
    argument = one_argument(argument, type);
    argument = one_argument(argument, vnumc);
    argument = one_argument(argument, loc);

    if (type[0] == '\0' || vnumc[0] == '\0' || loc[0] == '\0' || numc[0] == '\0' || (!is_number(vnumc) && str_cmp(vnumc, "mort")) || !is_number(numc)) {
      send_to_char("Syntax: scatter <number> <type> <vnum> <location>\n\rNumber: 1-50\n\rTypes: Object, Mobile\n\rVnum: <vnum> or 'mort'\n\rLocations: Area, Vmap, World\n\r", ch);
      return;
    }

    if (str_cmp(loc, "area") && str_cmp(loc, "world") && str_cmp(loc, "vmap")) {
      send_to_char("Invalid location!  Valid locations are:\n\rArea, Vmap, World\n\r", ch);
      return;
    }

    /* Scatter mortals!  Woot! */
    if (!is_number(vnumc)) {
      int cnt;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end();) {
        CHAR_DATA *mort = *it;
        ++it;

        cnt = 0;
        room = NULL;

        if (IS_IMMORTAL(mort))
        continue;

        while ((room == NULL || !pass) && cnt < 1000) {
          room = random_location(ch, loc, &map, &x, &y, &vmap, &pass);
          cnt++;
        }

        if (room != NULL && pass) {
          char_from_room(mort);
          char_to_room(mort, room);
        }
      }
    }

    vnum = atoi(vnumc);
    num = atoi(numc);

    if (num < 1 || num > 50) {
      send_to_char("Number range is 1-50.\n\r", ch);
      return;
    }

    if (!str_prefix(type, "object")) {
      if ((pObjIndex = get_obj_index(vnum)) == NULL) {
        send_to_char("No object has that vnum.\n\r", ch);
        return;
      }
    }
    else if (!str_prefix(type, "mobile")) {
      if ((pMobIndex = get_mob_index(vnum)) == NULL) {
        send_to_char("No mobile has that vnum.\n\r", ch);
        return;
      }
    }

    while (z < num && cnt++ < 10000) {
      room = random_location(ch, loc, &map, &x, &y, &vmap, &pass);

      if (room != NULL && pass) {
        if (pObjIndex != NULL) {
          obj = create_object(pObjIndex, ch->level);
          obj_to_room(obj, room);
        }
        else if (pMobIndex != NULL) {
          mob = create_mobile(pMobIndex);
          char_to_room(mob, room);
        }
        z++;
      }
    }
  }
  _DOFUN(do_clear) {
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("clear affects on whom?\n\r", ch);
      return;
    }
    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim)) {
      send_to_char("Not on NPC's.\n\r", ch);
      return;
    }

    while (victim->affected)
    affect_remove(victim, victim->affected);
    // victim->affected_by = race_table[victim->race].aff;
    return;
  }

  _DOFUN(do_idlecounters) {
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];

    if ((victim = get_char_world(ch, argument)) == NULL) {
      send_to_char("That person isn't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim))
    return;

    sprintf(buf, "Ordinary  Timer: %d\n\r", victim->timer);
    outbuf.strcat(buf);
    sprintf(buf, "Seconadry Timer: %d\n\r", victim->pcdata->secondary_timer);
    outbuf.strcat(buf);
    sprintf(buf, "Tertiary  Timer: %d\n\r", victim->pcdata->tertiary_timer);
    outbuf.strcat(buf);
    sprintf(buf, "Idling    Timer: %d\n\r", victim->pcdata->idling);
    outbuf.strcat(buf);
    outbuf.strcat(buf);
    sprintf(buf, "Idle Answer 1  : %d\n\r", victim->pcdata->idling_values[0]);
    outbuf.strcat(buf);
    sprintf(buf, "Idle Answer 2  : %d\n\r", victim->pcdata->idling_values[1]);
    outbuf.strcat(buf);

    send_to_char(outbuf, ch);

    return;
  }

  bool is_spyshield(CHAR_DATA *ch) {
    CHAR_DATA *to;
    int people = 0;
    bool found = FALSE;

    if (ch == NULL)
    return FALSE;

    if (IS_NPC(ch))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_SPYSHIELD))
    return TRUE;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      to = *it;

      if (IS_NPC(to) || to->desc == NULL || to == ch)
      continue;

      people++;

      if (IS_FLAG(to->act, PLR_SPYSHIELD))
      found = TRUE;
    }

    if (people > 1)
    return FALSE;

    if (found)
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_spyshield) {
    char arg[MSL];
    CHAR_DATA *victim;

    argument = one_argument_nouncap(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Do what to who?\n\r", ch);
      return;
    }

    if ((victim = get_char_world_true(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->act, PLR_SPYSHIELD)) {
      REMOVE_FLAG(victim->act, PLR_SPYSHIELD);
      send_to_char("You remove their spyshield.\n\r", ch);
      wiznet("$N has entered the game.", victim, NULL, WIZ_LOGINS, WIZ_SITES, get_trust(victim));

      return;
    }
    else {
      SET_FLAG(victim->act, PLR_SPYSHIELD);
      send_to_char("You give them a spyshield.\n\r", ch);
      wiznet("$N has left the game.", victim, NULL, WIZ_LOGINS, 0, get_trust(victim));
      return;
    }
  }

  _DOFUN(do_deadify) {
    char arg[MSL];
    CHAR_DATA *victim;

    argument = one_argument_nouncap(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Do what to who?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->act, PLR_DEAD)) {
      REMOVE_FLAG(victim->act, PLR_DEAD);
      if (is_ghost(victim)) {
        REMOVE_FLAG(victim->act, PLR_GHOST);
      }
      send_to_char("You bring them to life! MWAHAHAHAHAHAHA.\n\r", ch);
      victim->pcdata->final_death_date = 0;
      return;
    }
    else {
      SET_FLAG(victim->act, PLR_DEAD);
      send_to_char("You make them dead.\n\r", ch);
      victim->pcdata->final_death_date = current_time;
      return;
    }
  }
  void etag(CHAR_DATA *ch) {
    CHAR_DATA *to;
    int people = 0;

    if (ch == NULL)
    return;

    if (IS_NPC(ch))
    return;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      to = *it;

      if (IS_NPC(to) || to->desc == NULL || to == ch)
      continue;

      people++;
    }
  }

  _DOFUN(do_weatherset) {

    if (!str_cmp(argument, "snowing")) {
      snowing = 1;
      raining = 0;
      hailing = 0;
      time_info.local_cover_total = 100;
      time_info.local_density_total = 100;
    }
    else if (!str_cmp(argument, "hailing")) {
      snowing = 0;
      raining = 0;
      hailing = 1;
      time_info.local_cover_total = 100;
      time_info.local_density_total = 100;
    }
    else if (!str_cmp(argument, "raining")) {
      snowing = 0;
      raining = 1;
      hailing = 0;
      time_info.local_cover_total = 100;
      time_info.local_density_total = 100;
    }
    else if (!str_cmp(argument, "Clear")) {
      snowing = 0;
      raining = 0;
      hailing = 0;
      time_info.local_cover_total = 0;
      time_info.local_density_total = 0;
    }
    else if (!str_cmp(argument, "partlycloudy")) {
      snowing = 0;
      raining = 0;
      hailing = 0;
      time_info.local_cover_total = 50;
      time_info.local_density_total = 50;
    }
    else if (is_number(argument)) {
      time_info.des_temp = atoi(argument);
      time_info.local_temp = (time_info.local_temp + time_info.des_temp) / 2;
    }
    else
    send_to_char("Weatherset snowing/hailing/raining/clear/partlycloudy/(temp)\n\r", ch);
  }

  int text_expand(int number) {
    long start = (long)(number);
    start *= 2;
    start = start * start * start;
    int val = (int)(sqrt(start));
    return val;
  }
  int text_contract(int number) {
    long start = (long)(number);
    start = start * start;
    int val = (int)(cbrt(start));
    val /= 2;
    return val;
  }

  _DOFUN(do_worth) {
    return;

    ROOM_INDEX_DATA *dest = get_room_index(atoi(argument));
    int dir = coord_path(ch->in_room, dest->x, dest->y, dest->z, FALSE);
    printf_to_char(ch, "Direction: %d, %s\n\r", dir, dir_name[dir][0]);

    CHAR_DATA *victim;

    return;

    if (IS_IMMORTAL(ch)) {
      for (vector<GRAVE_TYPE *>::iterator it = GraveVect.begin();
      it != GraveVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->type == 0 && (*it)->room == ch->in_room->vnum) {
          (*it)->type = 2;
        }
      }
    }

    victim = get_char_world(ch, argument);
    if (victim != NULL)
    start_survey(ch, victim);
    return;
    char *thing;
    char *thing2;
    char *thing3;
    char *buf;

    thing = str_dup("Nme\n\r");
    thing2 =
    str_dup("This is a lot of text just an awful lot of pointless text to go here to see how this whole thing is gonna work and stuff.\n\r");
    thing3 = str_dup("Signing off!\n\r");
    buf = str_dup("");
    strcat(buf, thing);
    strcat(buf, thing2);
    strcat(buf, thing3);

    page_to_char(wrap_string(buf, 80), ch);
  }

  _DOFUN(do_charlist) {
    CHAR_DATA *victim;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || !victim || !victim->name)
      continue;

      printf_to_char(ch, "%s\n\r", victim->name);
    }
  }

  _DOFUN(do_desclist) {
    int k = 0;
    DescList::iterator it;

    for (it = descriptor_list.begin(); it != descriptor_list.end() && k < 200;) {
      DESCRIPTOR_DATA *d = *it;
      k++;
      ++it;

      if (d != NULL) {
        if (d->character != NULL) {
          printf_to_char(ch, "%d:%d   %s\n\r", k, d->connected, d->character->name);
        }
        else {
          printf_to_char(ch, "%d:%d\n\r", k, d->connected);
        }
      }
    }
  }

  _DOFUN(do_validchar) {
    char arg1[MSL];
    CHAR_DATA *victim;
    if (!IS_IMMORTAL(ch))
    return;

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (ch->valid == TRUE)
    send_to_char("True\n\r", ch);
    else
    send_to_char("False\n\r", ch);
  }

  _DOFUN(do_giverespect) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if (argument[0] == '\0' || arg[0] == '\0') {
      send_to_char("Personal echo what?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("Target not found.\n\r", ch);
      return;
    }

    give_respect(victim, atoi(argument), "Staff command.", ch->faction);
  }

  _DOFUN(do_fbanish) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if (argument[0] == '\0' || arg[0] == '\0') {
      send_to_char("Personal echo what?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("Target not found.\n\r", ch);
      return;
    }
    remove_from_clanroster(victim->name, victim->faction);
    victim->faction = 0;
    victim->esteem_faction = 0;
  }

  _DOFUN(do_faction_where) {
    char buf[MAX_STRING_LENGTH];
    Buffer outbuf;
    CHAR_DATA *victim;
    int count = 0, i = 0;
    ROOM_INDEX_DATA *room;
    int rooms[100];
    bool roomfound;
    bool valueset;

    for (i = 0; i < 100; i++)
    rooms[i] = 0;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL) {
        victim = d->character;
        count++;
        roomfound = FALSE;

        if (IS_FLAG(victim->act, PLR_SPYSHIELD))
        continue;

        for (i = 0; i < 100; i++) {
          if (victim->in_room->vnum == rooms[i])
          roomfound = TRUE;
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
    }

    for (i = 0; i < 100; i++) {
      if (rooms[i] != 0) {

        room = get_room_index(rooms[i]);

        if (room != NULL) {
          for (CharList::iterator it = room->people->begin();
          it != room->people->end(); ++it) {
            victim = *it;

            if (IS_FLAG(victim->act, PLR_SPYSHIELD))
            continue;

            if (victim != NULL && !IS_NPC(victim) && victim->in_room != NULL && victim->in_room == room) {
              if (is_gm(victim))
              sprintf(buf, "`R%s`x, ", victim->name);
              else if (victim->faction != 0
                  && clan_lookup(victim->faction) != NULL) {
                if (clan_lookup(victim->faction)->alliance == ALLIANCE_SIDELEFT)
                sprintf(buf, "`r%s`x, ", victim->name);
                else if (clan_lookup(victim->faction)->alliance ==
                    ALLIANCE_SIDERIGHT)
                sprintf(buf, "`g%s`x, ", victim->name);
                else if (clan_lookup(victim->faction)->alliance ==
                    ALLIANCE_SIDEMID)
                sprintf(buf, "`m%s`x, ", victim->name);
                else if (victim->faction == FACTION_HAND)
                sprintf(buf, "`D%s`x, ", victim->name);
                else if (victim->faction == FACTION_TEMPLE)
                sprintf(buf, "`G%s`x, ", victim->name);
                else if (victim->faction == FACTION_ORDER)
                sprintf(buf, "`W%s`x, ", victim->name);
              }
              else if (clinic_staff(victim, FALSE) || college_staff(victim, FALSE))
              sprintf(buf, "`063%s`x, ", victim->name);
              else if (college_student(victim, FALSE))
              sprintf(buf, "`y%s`x, ", victim->name);
              else if (clinic_patient(victim))
              sprintf(buf, "`Y%s`x, ", victim->name);
              else if (is_super(victim))
              sprintf(buf, "`c%s`x, ", victim->name);
              else
              sprintf(buf, "`x%s`x, ", victim->name);

              outbuf.strcat(buf);
            }
          }
          sprintf(buf, "%s [%d]", room->name, room->vnum);
          outbuf.strcat(buf);

          sprintf(buf, "\n\r");
          outbuf.strcat(buf);
        }
      }
    }
    page_to_char(outbuf, ch);
    return;
  }

  _DOFUN(do_coordvnum) {
    int vnum;
    int rem;
    if (!str_cmp(argument, "here")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;
      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "down")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomz(ch->in_room) - 1);
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "up")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomz(ch->in_room) + 1);
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "north")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room), get_roomy(ch->in_room) + 1, get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "south")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room), get_roomy(ch->in_room) - 1, get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "east")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room) - 1, get_roomy(ch->in_room), get_roomz(ch->in_room));
      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "west")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room) + 1, get_roomy(ch->in_room), get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "northwest")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room) + 1, get_roomy(ch->in_room) + 1, get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "southwest")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room) + 1, get_roomy(ch->in_room) - 1, get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "northeast")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room) - 1, get_roomy(ch->in_room) + 1, get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
    else if (!str_cmp(argument, "northeast")) {
      vnum = get_coord_vnum(get_roomx(ch->in_room) + 1, get_roomy(ch->in_room) + 1, get_roomz(ch->in_room));
      rem = ch->in_room->vnum % 10000000;
      vnum += ch->in_room->vnum - rem;

      printf_to_char(ch, "%d\n\r", vnum);
    }
  }

  _DOFUN(do_idig) {
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(dir_name[i][0], argument)) {
        smartdig(ch->in_room, i, ch->in_room->area->vnum);
        send_to_char("You dig.\n\r", ch);
        return;
      }
    }
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(dir_name[i][1], argument)) {
        smartdig(ch->in_room, i, ch->in_room->area->vnum);
        send_to_char("You dig.\n\r", ch);
        return;
      }
    }
  }

  _DOFUN(do_idemolish) {
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(dir_name[i][0], argument)) {
        smartdemolish(ch->in_room, i, 2);
        send_to_char("You demolish.\n\r", ch);
        return;
      }
    }
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(dir_name[i][1], argument)) {
        smartdemolish(ch->in_room, i, 2);
        send_to_char("You demolish.\n\r", ch);
        return;
      }
    }
  }

  void rename_account_pc(char *name, char *newname) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: rename account pc");

      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    free_string(victim->pcdata->account_name);
    victim->pcdata->account_name = str_dup(newname);

    // Discordance below
    //        free_string(victim->pcdata->account->name);
    //	victim->pcdata->account->name = str_dup(newname);

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  bool account_exists(char *name) {
    if (!str_cmp(name, ""))
    return FALSE;

    FILE *fp;
    char strsave[MAX_INPUT_LENGTH];

    sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(name));
    if ((fp = fopen(strsave, "r")) != NULL) {
      fclose(fp);
      return TRUE;
    }

    return FALSE;
  }

  bool rename_account(CHAR_DATA *ch, char *newname) {
    char strsave[MSL];
    char oldaccount[MSL];

    if (ch->pcdata->account == NULL && ch->desc->account != NULL)
    ch->pcdata->account = ch->desc->account;

    if (account_exists(newname)) {
      send_to_char("Account already exists.\n\r", ch);
      return FALSE;
    }
    if (safe_strlen(newname) < 3) {
      send_to_char("You need a longer account name than that.\n\r", ch);
      return FALSE;
    }

    if (ch->pcdata->account != NULL) {
      strcpy(oldaccount, ch->pcdata->account->name);

      sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(ch->pcdata->account->name));
      free_string(ch->pcdata->account->name);
      ch->pcdata->account->name = str_dup(newname);

      free_string(ch->desc->account->name);
      ch->desc->account->name = str_dup(newname);

      for (int i = 0; i < 25; i++) {
        rename_account_pc(ch->pcdata->account->characters[i], newname);
      }
      /* Custom fit forum stuff commented out for general release - Disco
      // Writing usrrmtmp.txt for forum account deletion
      FILE *fp;

      if ((fp = fopen("/home/haven/player/usrrmtmp.txt", "a")) == NULL) {
        perror("/home/haven/player/usrrmtmp.txt");
        send_to_char("Could not open the file!\n\r", ch);
      }
      else {
        fprintf(fp, "%s\n", oldaccount);
        fclose(fp);
      }

      // Running PHP script to remove user from forum based on usrrmtmp.txt info -
      // Discordance
      system("php /var/www/html/forum/usrrm.php");
      system("rm -f /home/haven/player/usrrmtmp.txt");
      */
      unlink(strsave);
      save_account(ch->pcdata->account, FALSE);
    }
    else {
      send_to_char("Error changing account name.\n\r", ch);
      return FALSE;
    }
    return TRUE;
  }

  vector<PETITION_TYPE *> PetitionVect;

  PETITION_TYPE *nullpetition;

#define PETITION_PENDING 0
#define PETITION_SOLVENOTEST 1
#define PETITION_SOLVETEST 2
#define PETITION_RESOLVED 3
#define PETITION_DISMISSUNCLEAR 4
#define PETITION_DISMISSINVALID 5
#define PETITION_URGENT 6
#define PETITION_TYR 7
#define PETITION_DISCORDANCE 8
#define PETITION_DAED 9

#define PETITION_BUG 0
#define PETITION_SUGGESTION 1
#define PETITION_REQUEST 2
#define PETITION_RENOVATE 3

  char *const pet_type_strings[] = {"bug", "suggestion", "request", "renovate"};

  char *const pet_status_strings[] = {
    "[   `rPending`x    ]", "[`gSolveNotTested`x]", "[ `gSolveTested`x  ]", "[   `gResolved`x   ]", "[`DDismissUnclear`x]", "[`DDismissInvalid`x]", "[    `RUrgent`x    ]", "[     `rTyr`x      ]", "[ `rDiscordance`x  ]", "[     `rDaed`x     ]"};


  char *const player_pet_status_strings[] = {
    "[   `rPending`x    ]", "[`gFix Deployed but not Tested`x]", "[ `gFix Deployed and Tested`x  ]", "[   `gResolved`x   ]", "[`DUnresolved: Not enough information`x]", "[`cInvestigated`x]", "[    `RUrgent`x    ]", "[     `rTyr`x      ]", "[ `rDiscordance`x  ]", "[     `rDaed`x     ]"};

  _DOFUN(do_petition) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "list")) {
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;
        if(!IS_IMMORTAL(ch) && str_cmp((*it)->author, ch->name) && str_cmp((*it)->account, ch->pcdata->account->name))
        continue;

        if(IS_IMMORTAL(ch))
        {
          if (safe_strlen((*it)->sentto) > 2) {
            printf_to_char(ch, "%02d) %s %s's %s for %s.\n\r", i, pet_status_strings[(*it)->status], (*it)->author, pet_type_strings[(*it)->type], (*it)->sentto);
          }
          else {
            printf_to_char(ch, "%02d) %s %s's %s.\n\r", i, pet_status_strings[(*it)->status], (*it)->author, pet_type_strings[(*it)->type]);
          }
        }
        else
        {
          if (safe_strlen((*it)->sentto) > 2) {
            printf_to_char(ch, "%02d) %s %s's %s for %s.\n\r", i, player_pet_status_strings[(*it)->status], (*it)->author, pet_type_strings[(*it)->type], (*it)->sentto);
          }
          else {
            printf_to_char(ch, "%02d) %s %s's %s.\n\r", i, player_pet_status_strings[(*it)->status], (*it)->author, pet_type_strings[(*it)->type]);
          }

        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "info")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if(!IS_IMMORTAL(ch) && str_cmp((*it)->author, ch->name) && str_cmp((*it)->account, ch->pcdata->account->name))
        continue;

        if (i == val) {
          time_t east_time = (*it)->create_time;
          char tmp[MSL];
          sprintf(tmp, "%s", (char *)ctime(&east_time));
          if (!IS_IMMORTAL(ch))
          printf_to_char(
          ch, "%02d) %s %s's %s.\nDate:%sRoom: %d\nAccount: %s\n%s\n\r", i, player_pet_status_strings[(*it)->status], (*it)->author, pet_type_strings[(*it)->type], tmp, (*it)->room, (*it)->account, (*it)->description);
          else
          printf_to_char(
          ch, "%02d) %s %s's %s.\nDate:%sRoom: %d\nAccount: %s\n%s\n\r", i, pet_status_strings[(*it)->status], (*it)->author, pet_type_strings[(*it)->type], tmp, (*it)->room, (*it)->account, (*it)->description);

          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Cancel")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if(!IS_IMMORTAL(ch) && str_cmp((*it)->author, ch->name) && str_cmp((*it)->account, ch->pcdata->account->name))
        continue;


        if (i == val) {
          (*it)->status = PETITION_DISMISSINVALID;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Solvenottested")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_SOLVENOTEST;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Solvetested")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_SOLVETEST;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "resolved")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_RESOLVED;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Dismissunclear")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_DISMISSUNCLEAR;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Dismissinvalid")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_DISMISSINVALID;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Urgent")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_URGENT;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Pending")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_PENDING;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Tyr")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_TYR;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Discordance")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_DISCORDANCE;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "Daed")) {
      int val = atoi(argument);
      if (val < 0 || val > 10000)
      return;
      if(!IS_IMMORTAL(ch))
      return;
      int i = 1;
      for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
      it != PetitionVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          bug("Save_petition: Blank petition in vector", i);
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if (i == val) {
          (*it)->status = PETITION_DAED;
          (*it)->timestamp = current_time;
          return;
        }
        i++;
      }
    }
    else
    send_to_char("Syntax: Petition list, petition info (number), petition cancel (number)\n\r", ch);
  }

  void fread_petition(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    PETITION_TYPE *pet;

    pet = new_petition();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", pet->author, fread_string(fp));
        KEY("Account", pet->account, fread_string(fp));
        break;
      case 'C':
        KEY("Create", pet->create_time, fread_number(fp));
        break;
      case 'D':
        KEY("Description", pet->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!pet->author) {
            bug("Fread_Petition: Name not found.", 0);
            free_petition(pet);
            return;
          }
          PetitionVect.push_back(pet);
          return;
        }
        break;
      case 'R':
        KEY("Room", pet->room, fread_number(fp));
      case 'S':
        KEY("Sentto", pet->sentto, fread_string(fp));
        KEY("Status", pet->status, fread_number(fp));
        break;
      case 'T':
        KEY("Timestamp", pet->timestamp, fread_number(fp));
        KEY("Type", pet->type, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_petition: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_petitions() {
    nullpetition = new_petition();
    FILE *fp;

    if ((fp = fopen(PETITION_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_petitions: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PETITION")) {
          fread_petition(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_petitions: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open petitions.txt", 0);
      exit(0);
    }
  }

  void save_petitions_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/petitions.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/petitions.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/petitions.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/petitions.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/petitions.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/petitions.txt");
    else
    sprintf(buf, "../data/back7/petitions.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open petitions.txt for writing", 0);
      return;
    }

    for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
    it != PetitionVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_petition: Blank petition in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (((*it)->status != PETITION_PENDING && (*it)->status != PETITION_TYR && (*it)->status != PETITION_DISCORDANCE && (*it)->status != PETITION_DAED) && (*it)->timestamp < (current_time - (3600 * 24 * 5)))
      continue;

      if ((*it)->status != PETITION_PENDING)
      continue;

      fprintf(fpout, "#PETITION\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Account %s~\n", (*it)->account);
      fprintf(fpout, "Sentto %s~\n", (*it)->sentto);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Timestamp %d\n", (*it)->timestamp);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Create %d\n", (*it)->create_time);
      fprintf(fpout, "Room %d\n", (*it)->room);
      fprintf(fpout, "End\n\n");
    }
    for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
    it != PetitionVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_petition: Blank petition in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (((*it)->status != PETITION_PENDING && (*it)->status != PETITION_TYR && (*it)->status != PETITION_DISCORDANCE && (*it)->status != PETITION_DAED) && (*it)->timestamp < (current_time - (3600 * 24 * 5)))
      continue;

      if ((*it)->status == PETITION_PENDING)
      continue;

      fprintf(fpout, "#PETITION\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Account %s~\n", (*it)->account);
      fprintf(fpout, "Sentto %s~\n", (*it)->sentto);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Timestamp %d\n", (*it)->timestamp);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Create %d\n", (*it)->create_time);
      fprintf(fpout, "Room %d\n", (*it)->room);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_petitions() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(PETITION_FILE, "w")) == NULL) {
      bug("Cannot open petitions.txt for writing", 0);
      return;
    }

    for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
    it != PetitionVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_petition: Blank petition in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (((*it)->status != PETITION_PENDING && (*it)->status != PETITION_TYR && (*it)->status != PETITION_DISCORDANCE && (*it)->status != PETITION_DAED) && (*it)->timestamp < (current_time - (3600 * 24 * 5)))
      continue;

      if ((*it)->status == PETITION_PENDING)
      continue;

      fprintf(fpout, "#PETITION\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Account %s~\n", (*it)->account);
      fprintf(fpout, "Sentto %s~\n", (*it)->sentto);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Timestamp %d\n", (*it)->timestamp);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Create %d\n", (*it)->create_time);
      fprintf(fpout, "Room %d\n", (*it)->room);

      fprintf(fpout, "End\n\n");
    }
    for (vector<PETITION_TYPE *>::iterator it = PetitionVect.begin();
    it != PetitionVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_petition: Blank petition in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (((*it)->status != PETITION_PENDING && (*it)->status != PETITION_TYR && (*it)->status != PETITION_DISCORDANCE && (*it)->status != PETITION_DAED) && (*it)->timestamp < (current_time - (3600 * 24 * 5)))
      continue;

      if ((*it)->status != PETITION_PENDING)
      continue;

      fprintf(fpout, "#PETITION\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Account %s~\n", (*it)->account);
      fprintf(fpout, "Sentto %s~\n", (*it)->sentto);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Timestamp %d\n", (*it)->timestamp);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Create %d\n", (*it)->create_time);
      fprintf(fpout, "Room %d\n", (*it)->room);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_petitions_backup();
  }


  _DOFUN(do_trollreport) {
    CHAR_DATA *victim;
    char arg[MSL];
    argument = one_argument(argument, arg);
    bool online = FALSE;

    if (ch->pcdata->account->donated < 1000) {
      send_to_char("You require 1000 community credit to use that command.\n\r", ch);
      return;
    }
    if (ch->played / 3600 < 150 && ch->pcdata->account->maxhours < 200) {
      send_to_char("You have not played for long enough to do that.\n\r", ch);
      return;
    }

    if (!str_cmp(ch->pcdata->account->name, "Thrace")) {
      send_to_char("You cannot do that.\n\r", ch);
      return;
    }

    if ((victim = get_victim_world(ch, arg)) != NULL) // Victim is online.
    online = TRUE;
    else if ((victim = get_char_world_account(arg)) != NULL)
    online = TRUE;

    if (online == FALSE) {
      send_to_char("Nobody like that is online.\n\r", ch);
      return;
    }
    ACCOUNT_TYPE *account;
    if (victim->desc != NULL && victim->desc->account != NULL) {
      account = victim->desc->account;
      free_string(troll_ip);
      troll_ip = str_dup(victim->desc->host);
    }
    else if (!IS_NPC(victim) && victim->pcdata->account != NULL)
    account = victim->pcdata->account;
    else {
      send_to_char("Nobody like that is online.\n\r", ch);
      return;
    }
    if (!IS_FLAG(account->flags, ACCOUNT_SPAMMER))
    SET_FLAG(account->flags, ACCOUNT_SPAMMER);

    send_to_char("Done.\n\r", ch);
    char buf[MSL];
    sprintf(buf, "%s trollreports %s(%s)", ch->name, victim->name, account->name);
    log_string(buf);
    wiznet(buf, ch, NULL, 0, 0, 0);
  }

  _DOFUN(do_marketupdate) { market_update(); }

  _DOFUN(do_wipeterritories) {
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      for (int i = 0; i < 20; i++)
      (*it)->phil_amount[i] = 0;
      free_string((*it)->notes);
      (*it)->notes = str_dup("");
      free_string((*it)->government);
      (*it)->government = str_dup("");
      free_string((*it)->people);
      (*it)->people = str_dup("");
      free_string((*it)->place);
      (*it)->place = str_dup("");
      free_string((*it)->timeline);
      (*it)->timeline = str_dup("");
      free_string((*it)->base_desc_core);
      (*it)->base_desc_core = str_dup("");
      (*it)->base_faction_core = 0;
      for (int i = 0; i < 5; i++) {
        free_string((*it)->other_name[i]);
        (*it)->other_name[i] = str_dup("");
        (*it)->other_amount[i] = 0;
      }
      for (int i = 0; i < 20; i++) {
        free_string((*it)->plant_desc[i]);
        (*it)->plant_desc[i] = str_dup("");
        free_string((*it)->place_desc[i]);
        (*it)->place_desc[i] = str_dup("");
      }
    }
  }

  void rp_log(char *text) {
    return;
    FILE *fpout;
    char buf[MSL];

    if ((fpout = fopen(buf, "a")) == NULL) {
      perror(buf);
      return;
    }
    fprintf(fpout, "RP:%s\n\r", from_color(text));
    fclose(fpout);
  }

  void rp_test(char *text) {
    FILE *fpout;
    char buf[MSL];
    sprintf(buf, "../log/rptest.txt");

    if ((fpout = fopen(buf, "a")) == NULL) {
      perror(buf);
      return;
    }
    fprintf(fpout, "RP:%s\n\r", from_color(text));
    fclose(fpout);
  }

  /* Forum stuff disabled for general release
  void forum_account(char *name) {

    char strsave[MAX_INPUT_LENGTH];
    ACCOUNT_TYPE *account;
    FILE *fp;
    account = new_account();
    free_string(account->name);
    account->name = str_dup(name);
    char buf[MSL];

    sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(name));
    if ((fp = fopen(strsave, "r")) != NULL) {
      fread_account(account, fp);
      fclose(fp);
    }
    else {
      return;
    }
    //       if(fork() == 0) {
  }

  void forum_update(void) {
    return;
    system("rm /home/haven/accounts/acct.tmp");
    system("rm /home/haven/accounts/acct.lst");
    system("ls /home/haven/accounts > /home/haven/accounts/acct.tmp");
    system("cat /home/haven/accounts/acct.tmp /home/haven/data/acct.end > /home/haven/accounts/acct.lst");
    FILE *fp;

    if ((fp = fopen("/home/haven/accounts/acct.lst", "r")) != NULL) {
      for (;;) {
        char *word = fread_word(fp);
        if (word != NULL && safe_strlen(word) > 0) {
          if (!str_cmp(word, "#END")) {
            fclose(fp);
            return;
          }
          forum_account(word);
        }
      }
      fclose(fp);
    }
  }

  _DOFUN(do_forumtest) { forum_update(); }
  */

  _DOFUN(do_bloodstorm) {
    if (time_info.bloodstorm == 0)
    time_info.bloodstorm = 1;
    else
    time_info.bloodstorm = 0;
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_noguest) {
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    char arg1[MSL];
    char buf[MSL];

    if (argument[0] == '\0') {
      send_to_char("Nonote whom?\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    d.original = NULL;
    if ((victim = get_char_world(ch, arg1)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if ((victim = get_char_world_pc(arg1)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: nonote");

        if (!load_char_obj(&d, arg1)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
          return;
        }
        victim = d.character;
      }
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      send_to_char("\n\rYou can't target mobiles!\n\r", ch);
      return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
      if (!online)
      free_char(victim);

      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->act, PLR_GUEST)) {
      REMOVE_FLAG(victim->act, PLR_GUEST);
      send_to_char("Flag cleared.\n\r", ch);
    }
    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);

    return;
  }

  _DOFUN(do_setdoom) {
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    char arg1[MSL];
    char buf[MSL];

    if (argument[0] == '\0') {
      send_to_char("Nonote whom?\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    d.original = NULL;
    if ((victim = get_char_world(ch, arg1)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if ((victim = get_char_world_pc(arg1)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: nonote");

        if (!load_char_obj(&d, arg1)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
          return;
        }
        victim = d.character;
      }
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      send_to_char("\n\rYou can't target mobiles!\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      send_to_char("\n\rYou can't target mobiles!\n\r", ch);
      return;
    }
    free_string(victim->pcdata->doom_custom);
    victim->pcdata->doom_custom = str_dup(argument);
    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);

    send_to_char("Done.\n\r", ch);
    return;
  }


  _DOFUN(do_tempcommand)
  {
    time_info.sect_alliance_issue = time_info.cult_alliance_issue;
    time_info.sect_alliance_type = time_info.cult_alliance_type;

  }


  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

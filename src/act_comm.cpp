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
#include <iostream>
#include <string>
#include <stdexcept>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "merc.h"
#include "olc.h"
#include "gsn.h"
#include "recycle.h"
#include "lookup.h"
#include "global.h"
#include "math.h"

#include "telnet.h"

#if defined(__cplusplus)
extern "C" {
#endif

  extern  bool is_same_link args( ( CHAR_DATA *ach, CHAR_DATA *bch) );
  char    *mangle_words   args( (CHAR_DATA *ch, char *txt, int percent) );
  void  lower_fatigue   args( (CHAR_DATA *ch, int length) );
  bool  antagonist        args( (CHAR_DATA *ch) );
  void  hack_text       args( (int from, char * argument, int tonumber, int destination) );
  OBJ_DATA* find_phone    args( (CHAR_DATA *ch, int number) );
  char  gagletter       args( ( void ) );
  void  public_attention  args( (CHAR_DATA *ch, CHAR_DATA *victim) );

  //MXP - Discordance
#define  TELOPT_MXP        '\x5B'
  const unsigned char will_mxp_str  [] = { IAC, WILL, TELOPT_MXP, '\0' };
  const unsigned char start_mxp_str [] = { IAC, SB, TELOPT_MXP, IAC, SE, '\0' };
  const unsigned char do_mxp_str    [] = { IAC, DO, TELOPT_MXP, '\0' };
  const unsigned char dont_mxp_str  [] = { IAC, DONT, TELOPT_MXP, '\0' };


  /* RT code to delete yourself */
  _DOFUN(do_delet) {
    send_to_char("You must type the full command to delete yourself.\n\r", ch);
  }

  _DOFUN(do_delete) {
    char strsave[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
    return;

    // This seems like the easiest way to keep people from deleting Storyrunners -
    // Discordance
    if (is_gm(ch) && !IS_IMMORTAL(ch)) {
      send_to_char("You're a storyrunner.\n\r", ch);
      return;
    }
    if(higher_power(ch))
    return;

    if (total_account_count(ch->pcdata->account) <= 1) {
      // Prevents last character on an account from being deleted - Discordance
      // This keeps people from using the excuse that they couldn't play because
      // of the new character restrictions.

      send_to_char("You can't delete the last character on your account.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_NOSAVE))
    return;

    if (is_roster_char(ch)) {
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!str_cmp(ch->name, (*it)->name)) {
          if (str_cmp((*it)->owner, ch->pcdata->account->name)) {
            send_to_char("You can't delete a character fresh off the roster.\n\r", ch);
            return;
          }
        }
      }
    }
    if (in_fight(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (ch->pcdata->confirm_delete) {
      if (argument[0] != '\0') {
        send_to_char("Delete status removed.\n\r", ch);
        ch->pcdata->confirm_delete = FALSE;
        return;
      }
      else {
        if (ch->pcdata->account == NULL)
        ch->pcdata->account = ch->desc->account;

        if (ch->pcdata->account == NULL) {
          send_to_char("Unexpected problem, please contact a staff member.\n\r", ch);
          return;
        }
        if (IS_FLAG(ch->act, PLR_GUEST)) {
          if (offline_flag(ch->pcdata->guest_of, PLR_FREEZE))
          offline_setflag(ch->pcdata->guest_of, PLR_FREEZE);
          if (guestmonster(ch)) {
            if (ch->played / 3600 > time_info.monster_hours && !IS_FLAG(ch->act, PLR_DEAD)) {
              char buf[MSL];
              sprintf(buf, "NIGHTMARE MADE %s:%s, %d:%d", ch->name, ch->pcdata->account->name, ch->played / 3600, time_info.monster_hours);
              log_string(buf);
              time_info.monster_hours = 30;
              if (!IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NIGHTMARE))
              SET_FLAG(ch->pcdata->account->flags, ACCOUNT_NIGHTMARE);
            }
          }
        }
        else if (!IS_FLAG(ch->act, PLR_STASIS)) {
          ch->pcdata->account->pkarmaspent -= ch->spentpkarma;
          ch->pcdata->account->pkarmaspent =
          UMAX(ch->pcdata->account->pkarmaspent, 0);

          if (ch->spentexp > 350000) {
            ch->spentexp -= 350000;
            ch->pcdata->account->xp += 350000;
            ch->pcdata->account->xp += ch->spentexp * 8 / 10;
          }
          else
          ch->pcdata->account->xp += ch->spentexp;
          if (ch->spentrpexp > 350000) {
            ch->spentrpexp -= 350000;
            ch->pcdata->account->rpxp += 350000;
            ch->pcdata->account->rpxp += ch->spentrpexp * 8 / 10;
          }
          else
          ch->pcdata->account->rpxp += ch->spentrpexp;
          ch->pcdata->account->karmabank += ch->spentkarma;
          ch->pcdata->account->rpxp += ch->pcdata->dexp / 5;
        }

        if (ch->played / 3600 > 100)
        retire_character(ch);
        remove_from_clanroster(ch->name, ch->faction);

        for (int i = 0; i < 25; i++) {
          if (!str_cmp(ch->pcdata->account->characters[i], ch->name)) {
            free_string(ch->pcdata->account->characters[i]);
            ch->pcdata->account->characters[i] = str_dup("");
            save_account(ch->pcdata->account, FALSE);
          }
        }
        /* Disabling forum stuff for general release
        if (total_account_count(ch->pcdata->account) <= 0) {
          // Writing usrrmtmp.txt for forum account deletion - Discordance
          FILE *fp;

          if ((fp = fopen("/home/haven/player/usrrmtmp.txt", "a")) == NULL) {
            perror("/home/haven/player/usrrmtmp.txt");
            send_to_char("Could not open the file!\n\r", ch);
          }
          else {
            fprintf(fp, "%s\n", ch->pcdata->account->name);
            fclose(fp);
          }

          // Running PHP script to remove user from forum based on usrrmtmp.txt
          // info - Discordance
          system("php /var/www/html/forum/usrrm.php");
          system("rm -f /home/haven/player/usrrmtmp.txt");

          sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(ch->pcdata->account->name));
          unlink(strsave);
        }
        */
        char buf[MSL];
        sprintf(buf, "rm /home/haven/prp/%s.txt", ch->name);
        system(buf);

        sr_delete(ch);
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
        wiznet("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
        remove_from_clanroster(ch->name, ch->faction);
        room_purge(ch);
        save_char_obj(ch, FALSE, FALSE);
        real_quit(ch);
        unlink(strsave);

        return;
      }
    }

    if (argument[0] != '\0') {
      send_to_char("Just type delete. No argument.\n\r", ch);
      return;
    }

    send_to_char("Type delete again to confirm this command.\n\r", ch);
    send_to_char("WARNING: this command is irreversible.\n\r", ch);
    send_to_char("Typing delete with an argument will undo delete status.\n\r", ch);
    ch->pcdata->confirm_delete = TRUE;
    wiznet("$N is contemplating deletion.", ch, NULL, 0, 0, get_trust(ch));
  }

  _DOFUN(do_channels) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (!str_cmp(arg1, "frequency")) {
      int i = atoi(arg2) - 1;
      if (i < 0 || i > 9) {
        send_to_char("Please enter a channel 1-10\n\r", ch);
        return;
      }
      int freq = atoi(argument);
      if (freq <= 0) {
        send_to_char("Frequncies should be positive numbers.\n\r", ch);
        return;
      }
      ch->pcdata->chan_numbers[i] = freq;
      send_to_char("Frequency set.\n\r", ch);
    }
    else if (!str_cmp(arg1, "name")) {
      int i = atoi(arg2) - 1;
      if (i < 0 || i > 9) {
        send_to_char("Please enter a channel 1-10\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      free_string(ch->pcdata->chan_names[i]);
      ch->pcdata->chan_names[i] = str_dup(arg3);
      send_to_char("Name set.\n\r", ch);
    }
    else if (!str_cmp(arg1, "off")) {
      int i = 0;
      for (i = 0; i < 10; i++) {
        if (!str_cmp(arg2, ch->pcdata->chan_names[i])) {
          ch->pcdata->chan_status[i] = 0;
          send_to_char("Comms channel switched off.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "on")) {
      int i = 0;
      for (i = 0; i < 10; i++) {
        if (!str_cmp(arg2, ch->pcdata->chan_names[i])) {
          ch->pcdata->chan_status[i] = 1;
          send_to_char("Comms channel switched on.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "broadcast")) {
      int i = 0;
      for (i = 0; i < 10; i++) {
        if (!str_cmp(arg2, ch->pcdata->chan_names[i])) {
          ch->pcdata->chan_status[i] = 2;
          send_to_char("Comms channel switched to broadcast.\n\r", ch);
        }
      }
    }
    else {
      int i;
      for (i = 0; i < 10; i++) {
        if (ch->pcdata->chan_status[i] == 0)
        printf_to_char(ch, "(%2.2d) [%12.12s] (%8d) [`rOff`x]\n\r", i + 1, ch->pcdata->chan_names[i], ch->pcdata->chan_numbers[i]);
        if (ch->pcdata->chan_status[i] == 1)
        printf_to_char(ch, "(%2.2d) [%12.12s] (%8d) [`gOn`x]\n\r", i + 1, ch->pcdata->chan_names[i], ch->pcdata->chan_numbers[i]);
        if (ch->pcdata->chan_status[i] == 2)
        printf_to_char(ch, "(%2.2d) [%12.12s] (%8d) [`WBroadcast`x]\n\r", i + 1, ch->pcdata->chan_names[i], ch->pcdata->chan_numbers[i]);
      }
    }
  }

  /* RT deaf blocks out all shouts */
  _DOFUN(do_deaf) {
    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You can now hear tells again.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_DEAF);
    }
    else {
      send_to_char("From now on, you won't hear tells.\n\r", ch);
      SET_FLAG(ch->comm, COMM_DEAF);
    }
  }

  /* RT quiet blocks out all communication */
  _DOFUN(do_quiet) {
    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("Quiet mode removed.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_QUIET);
    }
    else {
      send_to_char("From now on, you will only hear says and emotes.\n\r", ch);
      SET_FLAG(ch->comm, COMM_QUIET);
    }
  }

  _DOFUN(do_story) {
    if (IS_FLAG(ch->comm, COMM_STORY)) {
      if (ch->modifier == MODIFIER_PACT) {
        send_to_char("You can't turn story off when you have a demonic pact.\n\r", ch);
        return;
      }
      send_to_char("You are no longer in story mode.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_STORY);
    }
    else {
      send_to_char("You are now in story mode.\n\r", ch);
      SET_FLAG(ch->comm, COMM_STORY);
    }
  }

  /* afk command */
  _DOFUN(do_afk) {
    char arg1[MSL];
    char arg2[MSL];

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    char AFKmessage[MSL];

    if (IS_FLAG(ch->comm, COMM_AFK)) {
      if (ch->pcdata->buffer->dataLength() <= 0) {
        send_to_char("AFK mode removed. No tells to replay.\n\r", ch);
      }
      else {
        send_to_char("AFK mode removed. Type 'replay' to see tells.\n\r", ch);
      }
      act("$n is no longer AFK.", ch, NULL, NULL, TO_ROOM);
      REMOVE_FLAG(ch->comm, COMM_AFK);
    }
    else {
      SET_FLAG(ch->comm, COMM_AFK);
      if (safe_strlen(arg1) > 0) {
        if (is_number(arg1)) {
          if (safe_strlen(arg2) > 0 && is_number(arg2))
          sprintf(AFKmessage, "$n goes AFK. `g(`xThey expect to return between %d and %d minutes from now`g)`x", atoi(arg1), atoi(arg2));
          else
          sprintf(AFKmessage, "$n goes AFK. `g(`xThey expect to return in %d minutes.`g)`x", atoi(arg1));
          act(AFKmessage, ch, NULL, NULL, TO_CHAR);
          act(AFKmessage, ch, NULL, NULL, TO_ROOM);
          if (is_dreaming(ch)) {
            if (safe_strlen(arg2) > 0 && is_number(arg2))
            sprintf(AFKmessage, "%s goes AFK. `g(`xThey expect to return between %d and %d minutes from now`g)`x", dream_name(ch), atoi(arg1), atoi(arg2));
            else
            sprintf(
            AFKmessage, "%s goes AFK. `g(`xThey expect to return in %d minutes.`g)`x", dream_name(ch), atoi(arg1));
            dreamscape_message(ch, ch->pcdata->dream_room, AFKmessage);
          }
        }
        else {
          send_to_char("Syntax: AFK (number of minutes you expect to be gone) Or AFK (min number of minutes) (max number of minutes)\n\r", ch);
          act("$n goes AFK.", ch, NULL, NULL, TO_CHAR);
          act("$n goes AFK.", ch, NULL, NULL, TO_ROOM);
          if (is_dreaming(ch)) {
            sprintf(AFKmessage, "%s goes AFK.", dream_name(ch));
            dreamscape_message(ch, ch->pcdata->dream_room, AFKmessage);
          }
        }
      }
      else {
        act("$n goes AFK.", ch, NULL, NULL, TO_CHAR);
        act("$n goes AFK.", ch, NULL, NULL, TO_ROOM);
        if (is_dreaming(ch)) {
          sprintf(AFKmessage, "%s goes AFK.", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, AFKmessage);
        }
      }
    }
  }

  _DOFUN(do_subdue) {
    if (ch->pcdata->patrol_status == PATROL_GRABBING && ch->pcdata->patrol_target != NULL) {
      act("You manhandle $N and quickly truss $M up.", ch, NULL, ch->pcdata->patrol_target, TO_CHAR);
      act("$n manhandles you and quickly trusses you up.", ch, NULL, ch->pcdata->patrol_target, TO_VICT);
      logevent_check(ch, logact("$n manhandles $N and quickly trusses $M up.", ch, ch->pcdata->patrol_target));

      SET_FLAG(ch->pcdata->patrol_target->act, PLR_BOUND);
      ch->pcdata->patrol_target->pcdata->patrol_status = 0;
      ch->pcdata->patrol_target->pcdata->patrol_timer = 0;
      ch->pcdata->patrol_status = 0;
      ch->pcdata->patrol_timer = 0;
      return;
    }

    if (IS_FLAG(ch->comm, COMM_SUBDUE)) {
      send_to_char("You will no longer automatically attempt to subdue opponents.\n\r", ch);
      REMOVE_FLAG(ch->comm, COMM_SUBDUE);
    }
    else {
      send_to_char("You will now automatically attempt to subdue opponents.\n\r", ch);
      SET_FLAG(ch->comm, COMM_SUBDUE);
    }
  }

  _DOFUN(do_gtg) {
    char arg1[MSL];
    char arg2[MSL];

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    char AFKmessage[MSL];
    if (safe_strlen(arg1) > 0) {
      if (is_number(arg1)) {
        if (safe_strlen(arg2) > 0 && is_number(arg2))
        sprintf(AFKmessage, "`gGTG:`x $n's player has to log off in %d to %d minutes.`x", atoi(arg1), atoi(arg2));
        else
        sprintf(AFKmessage, "`gGTG:`x $n's player has to log off in %d minutes.`x", atoi(arg1));
        act(AFKmessage, ch, NULL, NULL, TO_CHAR);
        act(AFKmessage, ch, NULL, NULL, TO_ROOM);
        if (is_dreaming(ch)) {
          if (safe_strlen(arg2) > 0 && is_number(arg2))
          sprintf(AFKmessage, "`gGTG:`x %s's player has to log off in %d to %d minutes.`x", dream_name(ch), atoi(arg1), atoi(arg2));
          else
          sprintf(AFKmessage, "`gGTG:`x %s's player has to log off in %d minutes.`x", dream_name(ch), atoi(arg1));
          dreamscape_message(ch, ch->pcdata->dream_room, AFKmessage);
        }
      }
      else {
        send_to_char("Syntax: gtg (number of minutes until you have to leave) Or gtg (min number of minutes) (max number of minutes)\n\r", ch);
      }
    }
    else
    send_to_char("Syntax: gtg (number of minutes until you have to leave) Or gtg (min number of minutes) (max number of minutes)\n\r", ch);
  }

  _DOFUN(do_stall) {
    if (ch->pcdata->travel_time > 0 && (ch->pcdata->travel_type == TRAVEL_BPASSENGER || ch->pcdata->travel_type == TRAVEL_CPASSENGER))
    return;

    if (IS_FLAG(ch->comm, COMM_STALL)) {
      REMOVE_FLAG(ch->comm, COMM_STALL);
      act("$n resumes traveling.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You resume traveling.\n\r", ch);

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;
        vch = CH(d);
        if (vch == NULL)
        continue;
        if (vch->in_room != ch->in_room) {
          continue;
        }
        if (ch == vch)
        continue;
        if (!IS_FLAG(vch->comm, COMM_STALL))
        continue;
        REMOVE_FLAG(vch->comm, COMM_STALL);
      }
    }
    else {
      act("$n pauses traveling.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You pause traveling.\n\r", ch);
      SET_FLAG(ch->comm, COMM_STALL);

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;

        vch = CH(d);
        if (vch == NULL)
        continue;
        if (vch->in_room != ch->in_room)
        continue;
        if (ch == vch)
        continue;
        if (IS_FLAG(vch->comm, COMM_STALL))
        continue;
        SET_FLAG(vch->comm, COMM_STALL);
      }
    }
  }

  _DOFUN(do_replay) {
    if (IS_NPC(ch)) {
      send_to_char("You can't replay.\n\r", ch);
      return;
    }

    if (ch->pcdata->buffer->getBufferData()[0] == '\0') {
      send_to_char("You have no tells to replay.\n\r", ch);
      return;
    }

    page_to_char(*(ch->pcdata->buffer), ch);
    ch->pcdata->buffer->clear();
  }
  /*
  * Struct to hold info about global channels
  */
  struct gchan_struct {
    /* Name of the channel */
    char *name;

    /* Bit for this channel */
    int bit;

    /* Indicates if an IC channel or not */
    bool ic;

    /*
    * The string that is ouput before the name of the
    * talker
    */
    char *pre;

    /* The color of the channel */
    char say;
  };

  bool check_chansocial(CHAR_DATA *ch, char *command, char *argument, const struct gchan_struct chan) {
    CHAR_DATA *victim = NULL;
    char arg[MIL], start[MIL];
    int type = -1, cmd;
    bool found = FALSE;

    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++) {
      if (command[0] == social_table[cmd].name[0] && !str_prefix(command, social_table[cmd].name)) {
        found = TRUE;
        break;
      }
    }

    if (!found)
    return FALSE;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      type = 0;
    }
    else if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return TRUE;
    }
    else if (IS_FLAG(victim->comm, chan.bit)) {
      send_to_char("They are not listening to this channel.\n\r", ch);
    }
    else if (victim == ch) {
      type = 1;
    }
    else {
      type = 2;
    }

    if (victim != NULL && IS_NPC(victim)) {
      send_to_char("NPCs are not listening to this channel!\n\r", ch);
      return TRUE;
    }

    sprintf(start, "[%s] `%c", chan.pre, chan.say);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      bool test;

      vch = CH(d);
      if (vch == NULL)
      continue;

      if (IS_NPC(vch))
      continue;

      if ((chan.bit == COMM_NOWIZ && !IS_IMMORTAL(vch)) || (chan.bit == COMM_PRAY && vch->level < 107))
      continue;

      if (chan.bit == COMM_NOSTALK && !IS_IMMORTAL(vch) && !IS_FLAG(vch->act, PLR_GM))
      continue;

      test = FALSE;
      if ((d->connected == CON_PLAYING || d->connected == CON_CREATION) && ((chan.bit < COMM_NONE && !IS_FLAG(vch->comm, chan.bit)) || test) && !IS_FLAG(vch->comm, COMM_QUIET)) {
        if (vch == victim && ch != vch) {
          if (type == 1 || type == 2) {
            sprintf(arg, "%s%s`x", start, social_table[cmd].vict_found);
            act_new(arg, ch, NULL, victim, TO_VICT, POS_DEAD);
          }
        }
        else if (vch == ch) {
          if (type == 0) {
            sprintf(arg, "%s%s`x", start, social_table[cmd].char_no_arg);
            act_new(arg, ch, NULL, victim, TO_CHAR, POS_DEAD);
          }
          else if (type == 1) {
            sprintf(arg, "%s%s`x", start, social_table[cmd].char_auto);
            act_new(arg, ch, NULL, victim, TO_CHAR, POS_DEAD);
          }
          else {
            sprintf(arg, "%s%s`x", start, social_table[cmd].char_found);
            act_new(arg, ch, NULL, victim, TO_CHAR, POS_DEAD);
          }
        }
        else {
          if (type == 0) {
            sprintf(arg, "%s%s`x", start, social_table[cmd].others_no_arg);
            act_new(arg, ch, NULL, vch, TO_VICT, POS_DEAD);
          }
          else if (type == 1) {
            sprintf(arg, "%s%s`x", start, social_table[cmd].others_auto);
            act_new(arg, ch, NULL, vch, TO_VICT, POS_DEAD);
          }
          else {
            char *point, *point2, rep[MIL];

            sprintf(arg, "%s%s`x", start, social_table[cmd].others_found);
            rep[0] = '\0';
            point2 = rep;
            for (point = arg; *point; point++) {
              if (*point == '$' && *(point + 1) == 'N') {
                strcat(point2, victim->short_descr);
                point2 += safe_strlen(victim->short_descr);
                ++point;
                continue;
              }
              *point2 = *point;
              *++point2 = '\0';
            }
            act_new(rep, ch, NULL, vch, TO_VICT, POS_DEAD);
          }
        }
      }
    }

    return found;
  }

  char *chan_name(CHAR_DATA *ch, int chan, bool account) {
    char buf[MSL], c;

    if (account == FALSE)
    return NAME(ch);

    if (IS_IMMORTAL(ch))
    return NAME(ch);

    if (ch->pcdata->account == NULL && ch->desc->account != NULL) {
      ch->pcdata->account = ch->desc->account;
    }

    if (ch->pcdata->account != NULL) {
      strcpy(buf, ch->pcdata->account->name);
    }
    else if (safe_strlen(ch->pcdata->account_name) > 3) {
      strcpy(buf, ch->pcdata->account_name);
    }
    else {
      strcpy(buf, NAME(ch));
    }

    c = buf[1];
    putchar(toupper(c));

    return str_dup(buf);
  }

  void global_channel(CHAR_DATA *ch, char *argument, int bit) {
    char buf[MAX_STRING_LENGTH];
    int chan = -1;

    if (!IS_IMMORTAL(ch)) {
      smash_vector(argument);
    }

    /*
    * List of the global channels handled by this function
    * If a new global channel is added this table needs to be
    * updated.
    */
    const struct gchan_struct gchan_table[] = {
      {"OOC", COMM_NOOOC, TRUE, "`yOOC`x", 'c'},
      {"Immortal", COMM_NOWIZ, FALSE, "`WImmortal`x", 'Y'},
      {"Guide", COMM_NOGUIDE, FALSE, "`YGuide`x", 'e'},
      {"Newbie", COMM_NONEWBIE, FALSE, "`YNewbie`x", 'e'},
      {"Pray", COMM_PRAY, FALSE, "`GPray`x", 'E'},
      {"Stalk", COMM_NOSTALK, FALSE, "`GSTalk`x", 'g'},
      {"Legends", COMM_NOLEGEND, TRUE, "`WLegends`x", 'c'},
      {NULL, 0, FALSE, NULL, ' '}
    };

    if (!IS_NPC(ch)) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }

    for (int x = 0; gchan_table[x].name; x++)
    if (gchan_table[x].bit == bit)
    chan = x;

    if (chan == -1) {
      bugf("[%s:%s] Invalid comm bit : %d", __FILE__, __FUNCTION__, bit);
      return;
    }

    if ((bit == COMM_NOWIZ && !IS_IMMORTAL(ch))) {
      send_to_char("You cannot use that channel.\n\r", ch);
      return;
    }
    // Added some checks so that immortals don't need donation credit to see
    // Legends and OOC - Discordance
    if ((bit == COMM_NOOOC && available_donated(ch) < 1000 && (!IS_IMMORTAL(ch)))) {
      send_to_char("You cannot use that channel.\n\r", ch);
      return;
    }
    if ((bit == COMM_NOLEGEND && available_donated(ch) < 3000 && (!IS_IMMORTAL(ch)))) {
      send_to_char("You cannot use that channel.\n\r", ch);
      return;
    }

    if (IS_AFFECTED(ch, AFF_GAG) && gchan_table[chan].ic) {
      send_to_char("You mutter through your gag.", ch);
      return;
    }

    if (argument[0] == '\0' && bit < COMM_NONE) {
      if (IS_FLAG(ch->comm, bit)) {
        printf_to_char(ch, "`C%s channel is now ON.`x\n\r", gchan_table[chan].name);
        REMOVE_FLAG(ch->comm, bit);
        if (bit == COMM_NOGUIDE && IS_FLAG(ch->comm, COMM_NONEWBIE))
        REMOVE_FLAG(ch->comm, COMM_NONEWBIE);
        if (bit == COMM_NONEWBIE && IS_FLAG(ch->comm, COMM_NOGUIDE))
        REMOVE_FLAG(ch->comm, COMM_NOGUIDE);
      }
      else {
        printf_to_char(ch, "`C%s channel is now OFF.`x\n\r", gchan_table[chan].name);
        SET_FLAG(ch->comm, bit);
        if (bit == COMM_NOGUIDE && !IS_FLAG(ch->comm, COMM_NONEWBIE))
        SET_FLAG(ch->comm, COMM_NONEWBIE);
        if (bit == COMM_NONEWBIE && !IS_FLAG(ch->comm, COMM_NOGUIDE))
        SET_FLAG(ch->comm, COMM_NOGUIDE);
      }
    }
    else /* gossip message sent, turn gossip on if it isn't already */
    {
      char arg[MIL], arg2[MIL], coltemp[MIL], talk[MIL], emote[MIL];

      if (IS_FLAG(ch->comm, COMM_QUIET)) {
        send_to_char("You must turn off quiet mode first.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOCHANNELS)) {
        send_to_char("Your channels have been revoked.\n\r", ch);
        return;
      }

      if (is_name("NewCharacter", ch->name)) {
        send_to_char("Unnamed characters can't use channels.\n\r", ch);
        return;
      }
      if (is_griefer(ch) && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_GRIEFER)) {
        SET_FLAG(ch->desc->account->flags, ACCOUNT_GRIEFER);
      }

      if (bit < COMM_NONE)
      REMOVE_FLAG(ch->comm, bit);

      bool spammemote = FALSE;
      if (!str_cmp(ch->pcdata->last_newbie, argument))
      spammemote = TRUE;

      free_string(ch->pcdata->last_newbie);
      ch->pcdata->last_newbie = str_dup(argument);

      bool mod_flagged = FALSE;
      mod_flagged = isTextFlagged(argument);
      arg[0] = '\0';
      arg2[0] = '\0';
      talk[0] = '\0';
      emote[0] = '\0';
      coltemp[0] = '\0';
      strcat(coltemp, argument);
      one_argument(argument, arg);
      argument = one_argument(argument, arg);
      strcat(emote, argument);
      argument = one_argument(argument, arg2);
      remove_color(talk, coltemp);

      /* Make the words drunk if needed */

      if (!str_cmp(arg, "emote")) {
        remove_color(talk, emote);
        printf_to_char(ch, "[%s] `%c%s %s`x\n\r", gchan_table[chan].pre, gchan_table[chan].say, chan_name(ch, chan, gchan_table[chan].ic), talk);
        sprintf(buf, "[%s] `%c%s %s`x\n\r", gchan_table[chan].pre, gchan_table[chan].say, chan_name(ch, chan, gchan_table[chan].ic), talk);
        remove_color(talk, emote);
        rp_log(buf);
      }
      else {
        printf_to_char(ch, "[%s] %s: '`%c%s`x'\n\r", gchan_table[chan].pre, chan_name(ch, chan, gchan_table[chan].ic), gchan_table[chan].say, talk);

        sprintf(buf, "[%s] %s: '`%c%s`x'\n\r", gchan_table[chan].pre, chan_name(ch, chan, gchan_table[chan].ic), gchan_table[chan].say, talk);
        rp_log(buf);
      }

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *victim;
        bool test = FALSE;

        victim = CH(d);

        if (spammemote == TRUE && !same_player(ch, victim))
        continue;

        if (spammer(ch) && !same_player(ch, victim) && !IS_IMMORTAL(victim))
        continue;

        if (victim != NULL && !same_player(ch, victim) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHH) && victim != NULL && !IS_IMMORTAL(victim))
        continue;

        if (victim != NULL && !same_player(ch, victim) && victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHH))
        continue;

        if (victim != NULL && !same_player(ch, victim) && mod_flagged == TRUE)
        continue;

        if (victim == NULL || IS_NPC(victim))
        continue;

        test = (bit == COMM_NOGUIDE || bit == COMM_NONEWBIE) && (victim->played / 3600 < 50 || bit >= COMM_NONE);

        test = FALSE;

        if ((d->connected == CON_PLAYING) && d->character != ch && !IS_FLAG(victim->comm, COMM_QUIET) && (test || (bit < COMM_NONE && !IS_FLAG(victim->comm, bit)))) {
          if ((bit == COMM_NOWIZ && !IS_IMMORTAL(victim)) || (bit == COMM_PRAY && !IS_ADMIN(victim)))
          continue;

          if (bit == COMM_NOSTALK && !IS_IMMORTAL(victim) && !IS_FLAG(victim->act, PLR_GM))
          continue;

          if (bit == COMM_NOOOC && available_donated(victim) < 1000)
          continue;

          if (bit == COMM_NOLEGEND && available_donated(victim) < 3000)
          continue;

          send_to_char(buf, victim);
        }
      }
    }
  }

  _DOFUN(do_gossip) {}
  _DOFUN(do_immtalk) {global_channel(ch, argument, COMM_NOWIZ);}
  _DOFUN(do_stalk)   {global_channel(ch, argument, COMM_NOSTALK);}
  _DOFUN(do_ooc)     {global_channel(ch, argument, COMM_NOOOC);}
  _DOFUN(do_legends) {global_channel(ch, argument, COMM_NOLEGEND);}

  // added a catch for no argument to prevent people accidentally praying -
  // Discordance
  _DOFUN(do_pray) {
    abuse_check(ch, argument);
    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOPRAY)) {
      send_to_char("No one can hear your prayers.  See: help petition", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("What would you like to ask the staff?", ch);
      return;
    }
    global_channel(ch, argument, COMM_PRAY);
  }

  _DOFUN(do_newbie) {
    abuse_check(ch, argument);
    if (ch->played / 3600 < 50)
    global_channel(ch, argument, COMM_NONEWBIE);
    else
    global_channel(ch, argument, COMM_NOGUIDE);
  }

  _DOFUN(do_prayto) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Pray to whom what?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim))) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
      act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s prays to you '`i%s'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      return;
    }

    act("You pray to $N '`i$t`x'", ch, argument, victim, TO_CHAR);
    act_new("$n prays to you '`i$t`x'", ch, argument, victim, TO_VICT, POS_DEAD);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *victim;

      victim = (*it)->original ? (*it)->original : (*it)->character;

      if ((*it)->connected != CON_PLAYING)
      continue;

      if (victim != ch && !IS_FLAG(victim->comm, COMM_QUIET)) {
        if (victim->level >= 107) {
          act_new("$n prays, '`E$t`x", ch, argument, victim, TO_VICT, POS_SLEEPING);
        }
      }
    }
    return;
  }

  bool same_place(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;

    if (ch->in_room == NULL || victim->in_room == NULL || ch->in_room != victim->in_room)
    return FALSE;

    if (is_ghost(ch) || is_ghost(victim))
    return TRUE;

    if (!str_cmp(ch->pcdata->place, victim->pcdata->place) && safe_strlen(ch->pcdata->place) > 1)
    return TRUE;

    if (str_cmp(ch->pcdata->place, victim->pcdata->place) && safe_strlen(ch->pcdata->place) > 1)
    return FALSE;

    if (!str_cmp(ch->pcdata->place, victim->pcdata->place) && ch->on == victim->on)
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_call) {
    int numberdialed = 0;
    CHAR_DATA *victim;
    OBJ_DATA *obj = NULL;
    char arg[MSL];

    argument = one_argument_nouncap(argument, arg);

    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (ch->pcdata->connected_to != NULL) {
      send_to_char("You have to hangup first.\n\r", ch);
      return;
    }
    // Added a check to let players call with worn phones - Discordance
    if (!holding_phone(ch) && !wearing_phone(ch) && get_skill(ch, SKILL_ELECTROPATHIC) < 1 && !IS_SET(ch->in_room->room_flags, ROOM_LANDLINE)) {
      send_to_char("You must be holding a phone first.\n\r", ch);
      return;
    }

    int dialingnumber;

    if (IS_SET(ch->in_room->room_flags, ROOM_LANDLINE)) {
      dialingnumber = 0;
    }
    else if (get_phone(ch) != NULL) {
      dialingnumber = get_phone(ch)->value[0];
    }
    else {
      dialingnumber = 0;
    }

    if (dialingnumber == 0 && !IS_SET(ch->in_room->room_flags, ROOM_LANDLINE)) {
      send_to_char("You must be holding a phone first.\n\r", ch);
      return;
    }
    if (in_fight(ch) || ch->hit <= 0) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && get_skill(ch, SKILL_ELECTROPATHIC) < 1) {
      send_to_char("Damn your lack of opposable thumbs!\n\r", ch);
      return;
    }
    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    if (!str_cmp(arg, "911")) {
      bool found = FALSE;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *tempdude;

        tempdude = CH(d);

        if (tempdude == NULL || IS_NPC(tempdude)) {
          continue;
        }
        if (d->connected != CON_PLAYING || tempdude == ch) {
          continue;
        }
        if (tempdude->race != RACE_DEPUTY) {
          continue;
        }
        if (get_phone(tempdude) == NULL && get_skill(tempdude, SKILL_ELECTROPATHIC) < 1) {
          continue;
        }
        if (is_helpless(tempdude) || silenced(tempdude) || !cell_signal(tempdude) || is_ghost(tempdude) || IS_FLAG(tempdude->act, PLR_DEAD) || is_mute(tempdude) || IS_FLAG(tempdude->act, PLR_SHROUD)) {
          continue;
        }
        if (tempdude->pcdata->connected_to != NULL) {
          continue;
        }

        found = TRUE;
        victim = tempdude;
        obj = get_phone(victim);
        send_to_char("Dispatch connects you to an emergency call.\n\r", victim);
      }
      if (found == FALSE) {
        send_to_char("All lines seem busy.\n\r", ch);
        return;
      }
    }
    else {

      for (int i = 0; i < 50; i++) {
        if (!str_cmp(ch->pcdata->speed_names[i], arg)) {
          numberdialed = ch->pcdata->speed_numbers[i];
        }
      }
      if (numberdialed == 0) {
        numberdialed = atoi(arg);
      }

      for (ObjList::iterator it = object_list.begin(); it != object_list.end();
      ++it) {
        obj = *it;

        if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
          continue;
        }
        if (IS_SET(obj->extra_flags, ITEM_OFF)) {
          continue;
        }
        else {
          break;
        }
      }

      if (obj == NULL) {
        return;
      }
      if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed || numberdialed <= 0 || IS_SET(obj->extra_flags, ITEM_OFF)) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      if (obj->carried_by != NULL && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        victim = obj->carried_by;
      }
      else {
        if (obj->in_obj != NULL && obj->in_obj->carried_by != NULL && !IS_SET(obj->in_obj->extra_flags, ITEM_WARDROBE)) {
          victim = obj->in_obj->carried_by;
        }
        else {
          send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
          return;
        }
      }
    }

    if (IS_NPC(victim)) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }
    if (!cell_signal(victim) || IS_FLAG(victim->act, PLR_SHROUD)) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }
    if (victim->pcdata->connected_to != NULL) {
      send_to_char("That number is busy.\n\r", ch);
      return;
    }
    if (obj == NULL) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_LANDLINE)) {
      if (!cell_signal(ch)) {
        send_to_char("The cell signal is too weak.\n\r", ch);
        return;
      }
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_LANDLINE) && strcasestr(ch->pcdata->place, "payphone") != NULL && has_place(ch->in_room, "payphone")) {
      if (ch->money < 100) {
        send_to_char("This payphone costs $1 which you do not have.\n\r", ch);
        return;
      }
      else {
        ch->money -= 100;
      }
    }

    ch->pcdata->connected_to = victim;
    victim->pcdata->connected_to = ch;
    ch->pcdata->connection_stage = CONNECT_DIALING;
    victim->pcdata->connection_stage = CONNECT_RINGING;

    if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
      act("Your $p starts to vibrate.\n\r", victim, obj, victim, TO_CHAR);
    }
    else {
      act("$n's $p starts to ring.\n\r", victim, obj, victim, TO_ROOM);
      act("Your $p starts to ring.\n\r", victim, obj, victim, TO_CHAR);
    }
    act("The phone starts ringing.\n\r", ch, NULL, ch, TO_CHAR);

    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    int minutes = ptm->tm_min;
    int hours = ptm->tm_hour;
    hours = hours + 1; // 12 hour clock adjustment
    hours = hours + 5; // timezone adjustment

    hours += 12;

    if (hours > 23)
    hours -= 24;

    int ref = -1;
    for (int i = 0; i < 50; i++) {
      if (dialingnumber == victim->pcdata->speed_numbers[i])
      ref = i;
    }
    char buf[MSL];
    if (ref == -1)
    sprintf(buf, "%s\nCall From %d at %d:%d", obj->material, dialingnumber, hours, minutes);
    else
    sprintf(buf, "%s\nCall from %s at %d:%d", obj->material, victim->pcdata->speed_names[ref], hours, minutes);

    free_string(obj->material);
    obj->material = str_dup(buf);
  }

  _DOFUN(do_pickup) {
    if (IS_FLAG(ch->act, PLR_DEAD) && ch->in_room->area->vnum != 103) {
      send_to_char("Hah! Nice try.\n\r", ch);
      return;
    }

    if (ch->pcdata->connected_to == NULL || ch->pcdata->connection_stage != CONNECT_RINGING) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    if (in_fight(ch) || is_pinned(ch) || is_helpless(ch)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }

    // Added a check to make sure phones can't be answered without holding or
    // wearing a phone - Discordance
    if (!holding_phone(ch) && !wearing_phone(ch)) {
      send_to_char("You must be holding a phone first.\n\r", ch);
      return;
    }

    act("$n answers $s phone.\n\r", ch, NULL, ch, TO_ROOM);
    act("You answer the phone.\n\r", ch, NULL, ch, TO_CHAR);
    act("Someone answers the phone.\n\r", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);

    ch->pcdata->connection_stage = CONNECT_TALKING;
    ch->pcdata->connected_to->pcdata->connection_stage = CONNECT_TALKING;
  }

  _DOFUN(do_hangup) {
    if (ch->pcdata->connected_to == NULL || ch->pcdata->connection_stage == CONNECT_NONE) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }

    act("You hangup.\n\r", ch, NULL, ch, TO_CHAR);
    act("The person on the other end hangs up.\n\r", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);

    ch->pcdata->connected_to->pcdata->connection_stage = CONNECT_NONE;
    ch->pcdata->connected_to->pcdata->connected_to = NULL;
    ch->pcdata->connection_stage = CONNECT_NONE;
    ch->pcdata->connected_to = NULL;
  }

  char *distort_text(char *argument) {

    if (!is_town_blackout())
    return argument;

    char buf[MSL];
    char outbuf[MSL];
    strcpy(buf, argument);
    int cnt = 0;
    char arg[MSL];

    int chance = 90;

    outbuf[0] = '\0';
    while (argument[0] != '\0') {
      argument = one_argument(argument, arg);

      if (chance < number_percent()) {
        sprintf(outbuf, "%s%s%s", outbuf, (cnt == 0) ? "" : " ", arg);
      }
      else {
        sprintf(outbuf, "%s%s", outbuf, (cnt == 0) ? "" : " ^#(*$^#@");
      }
      cnt++;
    }
    return str_dup(outbuf);
  }

  void done_text(CHAR_DATA *ch, CHAR_DATA *victim, int numberdialed, bool online, char buf[MSL]) {
    free_string(find_phone(victim, numberdialed)->material);
    find_phone(victim, numberdialed)->material = str_dup(buf);

    send_to_char("Sent.\n\r", ch);

    if (!is_gm(ch)) {
      act("$n takes a picture with their phone.", ch, NULL, NULL, TO_ROOM);
    }
    if (online) {
      if (!is_gm(victim)) {
        OBJ_DATA *obj = find_phone(victim, numberdialed);
        if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
          act("Your $p vibrates.", victim, obj, NULL, TO_CHAR);
        }
        else {
          act("Your $p beeps.", victim, obj, NULL, TO_CHAR);
          act("$n's $p beeps.", victim, obj, NULL, TO_ROOM);
        }
      }
    }
  }

  void to_photo_message(CHAR_DATA *ch, char *text) {
    char buf[MSL];
    sprintf(buf, "`c _____________________________________________________________________________ \n\r`x");
    if (safe_strlen(ch->pcdata->photo_pose) > 2) {
      char pbuf[MSL];
      strcpy(pbuf, "");
      strcat(pbuf, wrap_string(ch->pcdata->photo_pose, 80));
      strcat(pbuf, "\n\n");
      strcat(buf, text_block_to_box(pbuf, "`c| `x", 80));
      free_string(ch->pcdata->photo_pose);
      ch->pcdata->photo_pose = str_dup("");
    }
    strcat(buf, text_block_to_box(text, "`c| `x", 80));
    strcat(buf, "`c|_____________________________________________________________________________|\n\r`x");
    char nbuf[MSL];
    if (safe_strlen(ch->pcdata->photo_msgs) < 20000)
    sprintf(nbuf, "%s\n%s", ch->pcdata->photo_msgs, buf);
    else
    sprintf(nbuf, "%s", buf);
    free_string(ch->pcdata->photo_msgs);
    ch->pcdata->photo_msgs = str_dup(nbuf);
    save_char_obj(ch, FALSE, FALSE);
  }

  _DOFUN(do_photo) {
    smash_tilde(argument);
    smash_vector(argument);
    int numberdialed = 0;
    CHAR_DATA *victim = NULL;
    OBJ_DATA *obj = NULL;
    int i, from, ref = -1;
    char buf[MSL];
    char *pdesc;
    char arg1[MSL];
    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    if (!str_cmp(argument, "clear")) {
      free_string(ch->pcdata->photo_msgs);
      ch->pcdata->photo_msgs = str_dup("");
      send_to_char("Photo Messages cleared.\n\r", ch);
      return;
    }
    if(ch->pcdata->ci_editing == 22)
    {
      ch->pcdata->ci_absorb = 1;
      if(higher_power(ch))
      {
        PROFILE_TYPE *profile = profile_lookup(ch->name);
        string_append(ch, &profile->photo);
        return;
      }
      if(safe_strlen(argument) < 2)
      {
        send_to_char("Syntax: photo (pose text).\n\r", ch);
        return;
      }
      char * tmpphoto = str_dup(ch->pcdata->photo_msgs);
      free_string(ch->pcdata->photo_msgs);
      ch->pcdata->photo_msgs = str_dup("");
      free_string(ch->pcdata->photo_pose);
      ch->pcdata->photo_pose = str_dup(argument);
      show_char_to_char_1(ch, ch, LOOK_LOOK, TRUE);
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      free_string(profile->photo);
      profile->photo = str_dup(ch->pcdata->photo_msgs);
      free_string(ch->pcdata->photo_msgs);
      ch->pcdata->photo_msgs = str_dup(tmpphoto);
      printf_to_char(ch, "Photo taken:\n\r%s\n\r", profile->photo);
      if(has_symbol_core(ch))
      profile->core_symbol = ch->fcore;
      else
      profile->core_symbol = 0;
      if(has_symbol_cult(ch))
      profile->cult_symbol = ch->fcult;
      else
      profile->cult_symbol = 0;
      if(has_symbol_sect(ch))
      profile->sect_symbol = ch->fsect;
      else
      profile->sect_symbol = 0;

      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (ch->wounds >= 3 && !is_undead(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!holding_phone(ch)) {
      send_to_char("You must be holding a phone first.\n\r", ch);
      return;
    }
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID) {
      send_to_char("Damn your lack of opposable thumbs!\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD) || is_ghost(ch)) {
      send_to_char("You can't get a connection.\n\r", ch);
      return;
    }
    if (!cell_signal(ch)) {
      send_to_char("You can't get a connection.\n\r", ch);
      return;
    }
    if (in_fight(ch) || ch->hit <= 0) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (IS_SET(get_eq_char(ch, WEAR_HOLD)->extra_flags, ITEM_OFF)) {
      send_to_char("Your phone is off.\n\r", ch);
      return;
    }

    if (get_phone(ch) != NULL && get_phone(ch)->item_type == ITEM_PHONE) {
      if (get_phone(ch)->material == NULL) {
        get_phone(ch)->material = str_dup("");
      }
      if (!is_name("phone", get_phone(ch)->name)) {
        send_to_char("Phones must include the word 'phone' in their name to function.\n\r", ch);
        return;
      }
    }

    if (safe_strlen(arg1) < 2 || safe_strlen(argument) < 2) {
      send_to_char("Send photo to who?\n\r", ch);
      return;
    }

    if (get_phone(ch) == NULL) {
      send_to_char("You do not seem to have a phone.\n\r", ch);
      return;
    }


    if(arg1[0] == '@')
    {
      PROFILE_TYPE *tprofile = profile_handle_lookup(arg1);
      if(tprofile == NULL)
      {
        send_to_char("No such profile.\n\r", ch);
        return;
      }
      MATCH_TYPE * match = match_find_by_name(ch->name, tprofile->name);
      if(match == NULL || match->status_one != 1 || match->status_two != 1)
      {
        send_to_char("You have not matched with them.\n\r", ch);
        return;
      }
      bool sent = dm_to_person(ch, tprofile->name, argument, TRUE);
      return;
    }

    from = get_phone(ch)->value[0];
    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }
    for (i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->speed_names[i], arg1)) {
        numberdialed = ch->pcdata->speed_numbers[i];
      }
    }
    if (numberdialed == 0) {
      numberdialed = atoi(arg1);
    }

    bool phonefound = FALSE;
    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
        continue;
      }
      else if (IS_SET(obj->extra_flags, ITEM_OFF)) {
        continue;
      }
      else {
        phonefound = TRUE;
        break;
      }
    }

    if (phonefound == FALSE) {
      char phonename[MSL];
      bool bookfound = FALSE;
      for (vector<PHONEBOOK_TYPE *>::iterator it = PhoneVect.begin();
      it != PhoneVect.end(); ++it) {
        if ((*it)->number == numberdialed) {
          sprintf(phonename, "%s", (*it)->owner);
          bookfound = TRUE;
        }
      }
      if (bookfound == FALSE) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }
      d.original = NULL;
      if ((victim = get_char_world_pc_noname(ch, phonename)) != NULL) {
        online = TRUE;
      }
      else {
        if ((victim = get_char_world_pc(phonename)) != NULL) {
          online = TRUE;
        }
        else {
          if (!load_char_obj(&d, phonename)) {
            printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(phonename));
          stat(buf, &sb);
          victim = d.character;
        }
      }

      if (IS_NPC(victim)) {
        if (!online) {
          free_char(victim);
        }
        return;
      }
      if (online) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

    }
    else {
      online = TRUE;
      if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      bool noowner = FALSE;
      if (obj->carried_by != NULL && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        victim = obj->carried_by;
      }
      else {
        if (obj->in_obj != NULL && obj->in_obj->carried_by != NULL && !IS_SET(obj->in_obj->extra_flags, ITEM_WARDROBE)) {
          victim = obj->in_obj->carried_by;
        }
        else {
          noowner = TRUE;
        }
      }

      if (noowner == TRUE || victim == NULL || IS_NPC(victim)) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }
    }

    for (i = 0; i < 50; i++) {
      if (from == victim->pcdata->speed_numbers[i]) {
        ref = i;
      }
    }

    WAIT_STATE(ch, PULSE_PER_SECOND * 5);

    if (find_phone(victim, numberdialed) == NULL) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      if (!online) {
        free_char(victim);
      }
      return;
    }
    if (IS_SET(find_phone(victim, numberdialed)->extra_flags, ITEM_OFF)) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      if (!online) {
        free_char(victim);
      }
      return;
    }

    for (i = 0; i < 50; i++) {
      if (!str_cmp(victim->name, ch->pcdata->speed_names[i])) {
        if (ch->pcdata->speed_numbers[i] !=
            find_phone(victim, numberdialed)->value[0]) {
          send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
          if (!online) {
            free_char(victim);
          }
          return;
        }
      }
    }

    ch->pcdata->time_since_emote = 0;
    if (ref == -1) {
      sprintf(buf, "%s\n[%s]%d: A photo message(look photos to view)", find_phone(victim, numberdialed)->material, newtexttime(), from);
    }
    else {
      sprintf(buf, "%s\n[%s]%s: A photo message(look photos to view)", find_phone(victim, numberdialed)->material, newtexttime(), victim->pcdata->speed_names[ref]);
    }
    argument = one_argument(argument, arg1);
    CHAR_DATA *lookvic;
    if ((lookvic = get_char_room(ch, NULL, arg1)) != NULL && !IS_FLAG(lookvic->act, PLR_SHROUD)) {
      char pbuf[MSL];
      strcpy(pbuf, "");
      strcat(pbuf, PERS(lookvic, victim));
      if (safe_strlen(lookvic->pcdata->title) > 2)
      strcat(pbuf, lookvic->pcdata->title);
      else if (safe_strlen(lookvic->pcdata->place) > 2) {
        strcat(pbuf, " is at ");
        strcat(pbuf, lookvic->pcdata->place);
      }
      if (safe_strlen(argument) > 2) {
        strcat(pbuf, "\n\r");
        strcat(pbuf, argument);
      }
      free_string(victim->pcdata->photo_pose);
      victim->pcdata->photo_pose = str_dup(pbuf);
      show_char_to_char_1(lookvic, victim, LOOK_LOOK, TRUE);
      done_text(ch, victim, numberdialed, online, buf);
      return;
    }
    char arg2[MSL];
    char arg3[MSL];

    argument = one_argument(argument, arg2);
    free_string(victim->pcdata->photo_pose);
    victim->pcdata->photo_pose = str_dup(argument);

    int number = number_argument(arg1, arg3);
    int count = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj) && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) { /* player can see object */
        if (is_name(arg3, obj->name) && count + 1 == number) {
          show_obj_to_char(obj, victim, TRUE, FALSE);
          done_text(ch, victim, numberdialed, online, buf);
          if (!online) {
            free_char(victim);
          }
          return;
        }
        else {
          pdesc = get_extra_descr_obj(arg3, obj->extra_descr, obj);
          if (pdesc != NULL) {
            if (++count == number) {
              to_photo_message(victim, pdesc);
              done_text(ch, victim, numberdialed, online, buf);
              if (!online) {
                free_char(victim);
              }
              return;
            }
            else
            continue;
          }
          pdesc = get_extra_descr_obj(arg3, obj->pIndexData->extra_descr, obj);
          if (pdesc != NULL) {
            if (++count == number)
            to_photo_message(victim, pdesc);
            done_text(ch, victim, numberdialed, online, buf);
            if (!online) {
              free_char(victim);
            }
            return;
          }
          else
          continue;
        }
      }
    }

    for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj)) {
        pdesc = get_extra_descr(arg3, obj->extra_descr);
        if (pdesc != NULL)
        if (++count == number) {
          to_photo_message(victim, pdesc);
          done_text(ch, victim, numberdialed, online, buf);
          if (!online) {
            free_char(victim);
          }
          return;
        }

        pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
        if (pdesc != NULL)
        if (++count == number) {
          to_photo_message(victim, pdesc);
          done_text(ch, victim, numberdialed, online, buf);
          if (!online) {
            free_char(victim);
          }
          return;
        }

        if (is_name(arg3, obj->name))
        if (++count == number) {
          to_photo_message(victim, obj->description);
          done_text(ch, victim, numberdialed, online, buf);
          if (!online) {
            free_char(victim);
          }
          return;
        }
      }
    }

    pdesc = get_extra_descr(arg3, ch->in_room->extra_descr);
    if (pdesc != NULL) {
      if (++count == number) {
        to_photo_message(victim, pdesc);
        done_text(ch, victim, numberdialed, online, buf);

        send_to_char(pdesc, victim);
        if (!online) {
          free_char(victim);
        }
        return;
      }
    }

    if (count > 0 && count != number) {
      if (count == 1)
      sprintf(buf, "You only see one %s here.\n\r", arg3);
      else
      sprintf(buf, "You only see %d of those here.\n\r", count);

      send_to_char(buf, ch);
      if (!online) {
        free_char(victim);
      }
      return;
    }
  }


  void photo_process(CHAR_DATA *ch, CHAR_DATA *victim, char * argument)
  {
    char arg1[MSL];
    argument = one_argument(argument, arg1);
    OBJ_DATA *obj;
    char buf[MSL];
    char *pdesc;
    CHAR_DATA *lookvic;
    if ((lookvic = get_char_room(ch, NULL, arg1)) != NULL && !IS_FLAG(lookvic->act, PLR_SHROUD)) {
      char pbuf[MSL];
      strcpy(pbuf, "");
      strcat(pbuf, PERS(lookvic, victim));
      if (safe_strlen(lookvic->pcdata->title) > 2)
      strcat(pbuf, lookvic->pcdata->title);
      else if (safe_strlen(lookvic->pcdata->place) > 2) {
        strcat(pbuf, " is at ");
        strcat(pbuf, lookvic->pcdata->place);
      }
      if (safe_strlen(argument) > 2) {
        strcat(pbuf, "\n\r");
        strcat(pbuf, argument);
      }
      free_string(victim->pcdata->photo_pose);
      victim->pcdata->photo_pose = str_dup(pbuf);
      show_char_to_char_1(lookvic, victim, LOOK_LOOK, TRUE);
      return;
    }

    char arg2[MSL];
    char arg3[MSL];

    argument = one_argument(argument, arg2);
    free_string(victim->pcdata->photo_pose);
    victim->pcdata->photo_pose = str_dup(argument);

    int number = number_argument(arg1, arg3);
    int count = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj) && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) { /* player can see object */
        if (is_name(arg3, obj->name) && count + 1 == number) {
          show_obj_to_char(obj, victim, TRUE, FALSE);

          return;
        }
        else {
          pdesc = get_extra_descr_obj(arg3, obj->extra_descr, obj);
          if (pdesc != NULL) {
            if (++count == number) {
              to_photo_message(victim, pdesc);
              return;
            }
            else
            continue;
          }
          pdesc = get_extra_descr_obj(arg3, obj->pIndexData->extra_descr, obj);
          if (pdesc != NULL) {
            if (++count == number)
            to_photo_message(victim, pdesc);
            return;
          }
          else
          continue;
        }
      }
    }

    for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj)) {
        pdesc = get_extra_descr(arg3, obj->extra_descr);
        if (pdesc != NULL)
        if (++count == number) {
          to_photo_message(victim, pdesc);
          return;
        }

        pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
        if (pdesc != NULL)
        if (++count == number) {
          to_photo_message(victim, pdesc);
          return;
        }

        if (is_name(arg3, obj->name))
        if (++count == number) {
          to_photo_message(victim, obj->description);
          return;
        }
      }
    }

    pdesc = get_extra_descr(arg3, ch->in_room->extra_descr);
    if (pdesc != NULL) {
      if (++count == number) {
        to_photo_message(victim, pdesc);
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

  }

  bool electropathic(CHAR_DATA *ch)
  {
    if(get_skill(ch, SKILL_ELECTROPATHIC) > 0)
    return TRUE;
    if(higher_power(ch))
    return TRUE;
    return FALSE;
  }


  bool dm_to_person(CHAR_DATA *ch, char *tname, char * msg, bool picture)
  {
    CHAR_DATA *victim = NULL;
    OBJ_DATA *obj = NULL;
    bool online = FALSE;
    char buf3[MSL];
    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    PROFILE_TYPE *victim_profile = profile_lookup(tname);
    if(victim_profile == NULL)
    victim_profile = profile_handle_lookup(tname);
    if(char_profile == NULL || victim_profile == NULL)
    return FALSE;

    MATCH_TYPE * match = match_find_by_name(ch->name, victim_profile->name);

    DESCRIPTOR_DATA d;

    d.original = NULL;
    if ((victim = get_char_world_pc_noname(NULL, victim_profile->name)) != NULL) {
      online = TRUE;
    }
    else {
      if ((victim = get_char_world_pc(victim_profile->name)) != NULL) {
        online = TRUE;
      }
      else {
        if (!load_char_obj(&d, victim_profile->name)) {
          return FALSE;
        }
        victim = d.character;
      }
    }
    if (IS_NPC(victim)) {
      if (!online) {
        free_char(victim);
      }
      return FALSE;
    }

    if(victim == NULL)
    {
      log_string("Null dm target");
      return FALSE;
    }
    obj = get_phone(victim);
    if(obj == NULL && !electropathic(victim))
    {
      if(victim != NULL && !online)
      free_char(victim);
      return FALSE;
    }
    if(obj == NULL && online == FALSE)
    {
      send_to_char("You can't get a signal.\n\r", ch);
      return FALSE;
    }
    if(obj != NULL)
    {
      if (IS_SET(obj->extra_flags, ITEM_OFF)) {
        if(victim != NULL && !online)
        free_char(victim);
        return FALSE;
      }

      if(safe_strlen(obj->material) > 16000)
      {
        if(victim != NULL && !online)
        free_char(victim);

        return FALSE;
      }
    }
    if(!str_cmp(match->nameone, ch->name))
    {
      match->last_msg_one_three = match->last_msg_one_two;
      match->last_msg_one_two = match->last_msg_one_one;
      match->last_msg_one_one = current_time;
    }
    else
    {
      match->last_msg_two_three = match->last_msg_two_two;
      match->last_msg_two_two = match->last_msg_two_one;
      match->last_msg_two_one = current_time;
    }
    char sname[MSL];
    sprintf(sname, "%s", char_profile->handle);

    char outbuf[MSL];
    char tbuf[MSL];
    if(picture == true)
    {
      photo_process(ch, victim, msg);
      if(obj != NULL)
      {
        sprintf(outbuf, "%s\n[%s]%s: A photo message(look photos to view)", obj->material, newtexttime(), str_dup(sname));
        sprintf(tbuf, "[%s]%s: A photo message(look photos to view)", newtexttime(), str_dup(sname));
        free_string(obj->material);
        obj->material = str_dup(outbuf);
      }
      else
      printf_to_char(victim, "%s sends you a photo message.\n\r", sname);
      send_to_char("Sent.\n\r", ch);
      if (!is_gm(ch)) {
        act("$n takes a picture with their phone.", ch, NULL, NULL, TO_ROOM);
      }
      if (online) {
        if (!is_gm(victim)) {
          OBJ_DATA *obj = get_phone(victim);
          if(obj != NULL)
          {
            if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
              act("Your $p vibrates.", victim, obj, NULL, TO_CHAR);
            }
            else {
              act("Your $p beeps.", victim, obj, NULL, TO_CHAR);
              act("$n's $p beeps.", victim, obj, NULL, TO_ROOM);
            }
          }
        }
      }
      if(!online)
      free_char(victim);

      return TRUE;
    }
    else
    {
      char rplog[MSL];
      sprintf(rplog, "You text %s %s", victim_profile->handle, msg);
      prp_rplog(ch, rplog);

      if(obj != NULL)
      sprintf(outbuf, "%s\n[%s]%s: %s", obj->material, newtexttime(), str_dup(sname), from_color(distort_text(msg)));
      else
      sprintf(outbuf, "[%s]%s: %s", newtexttime(), str_dup(sname), from_color(distort_text(msg)));
      sprintf(tbuf, "[%s]%s: %s", newtexttime(), str_dup(char_profile->display_handle), from_color(distort_text(msg)));
      sprintf(buf3, "[%s]%s: %s", newtexttime(), str_dup(sname), from_color(distort_text(msg)));
    }
    char buf2[MSL];
    if(ch != NULL)
    {
      if (ch->in_room == victim->in_room && ch != victim)
      social_behave_mod(ch, -3, "texting person in same room");

      give_attention(ch, victim);

      for (DescList::iterator dit = descriptor_list.begin();
      dit != descriptor_list.end(); ++dit) {
        DESCRIPTOR_DATA *dtt = *dit;
        CHAR_DATA *to;

        if (dtt->character != NULL && dtt->connected == CON_PLAYING) {
          to = dtt->character;
          if (IS_NPC(to)) {
            continue;
          }
          if (to->in_room == NULL || ch->in_room == NULL) {
            continue;
          }

          if (is_ghost(to) && (to->possessing == ch || to->possessing == victim)) {
            if (to->possessing == ch) {
              printf_to_char(to, "%s sends a text: %s\n\r", PERS(to->possessing, to), buf3);
            }
            else {
              printf_to_char(to, "%s receives a text: %s\n\r", PERS(to->possessing, to), buf3);
            }
          }

          if ((!IS_FLAG(to->act, PLR_SPYING) || IS_FLAG(victim->act, PLR_SPYSHIELD) || !can_spy(to, ch))) {
            continue;
          }
          if ((!IS_FLAG(to->act, PLR_SPYING) || IS_FLAG(ch->act, PLR_SPYSHIELD) || !can_spy(to, victim))) {
            continue;
          }

          printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(msg));
        }
      }

      char rplog[MSL];
      sprintf(rplog, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(msg));
      rp_log(rplog);
      send_to_char("Sent.\n\r", ch);
      sprintf(rplog, "You get a text: %s", buf3);
      prp_rplog(victim, rplog);

      if (number_percent() % 2 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5) {
        printf_to_char(get_tracer(ch), "You intercept a text: %s\n\r", msg);
      }

      if (IS_NPC(ch) || ch->pcdata->institute_action == 0) {
        rpreward(ch, msg, TRUE, 1);
      }

    }
    
    if(obj != NULL)
    {
      free_string(obj->material);
      obj->material = str_dup(outbuf);
    }

    if(ch != NULL && victim != NULL)
    {
      char hbuf[MSL];
      sprintf(hbuf, "[%s]%s: %s", newtexttime(), ch->name, msg);
      bool hexist = FALSE;
      for (vector<TEXTHISTORY_TYPE *>::iterator hit = HTextVect.begin();
      hit != HTextVect.end(); ++hit) {
        if ((*hit)->valid == FALSE)
        continue;
        if(!str_cmp(ch->name, (*hit)->name_one) && !str_cmp(victim->name, (*hit)->name_two))
        {
          hexist = TRUE;
          char * tmpl = appendLine((*hit)->history, str_dup(hbuf));
          free_string((*hit)->history);
          (*hit)->history = str_dup(tmpl);
          (*hit)->last_msg = current_time;
        }
        else if(!str_cmp(ch->name, (*hit)->name_two) && !str_cmp(victim->name, (*hit)->name_one))
        {
          hexist = TRUE;
          char * tmpl = appendLine((*hit)->history, str_dup(hbuf));
          free_string((*hit)->history);
          (*hit)->history = str_dup(tmpl);
          (*hit)->last_msg = current_time;

        }

      }

      if(hexist == FALSE)
      {
        TEXTHISTORY_TYPE *ht = new TEXTHISTORY_TYPE;
        ht->valid = TRUE;
        ht->name_one = str_dup(ch->name);
        ht->name_two = str_dup(victim->name);
        ht->history = str_dup(hbuf);
        ht->last_msg = current_time;
        HTextVect.push_back(ht);
      }

    }


    if (online) {
      if (!is_gm(victim)) {
        if(obj != NULL)
        {
          if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
            act("Your $p vibrates.", victim, obj, NULL, TO_CHAR);
          }
          else {
            act("Your $p beeps.", victim, obj, NULL, TO_CHAR);
            act("$n's $p beeps.", victim, obj, NULL, TO_ROOM);
          }
        }
        if(obj == NULL || holding_phone(victim) || higher_power(victim))
        printf_to_char(victim, "%s\n\r", tbuf);
      }
    }

    save_char_obj(victim, FALSE, FALSE);
    if (!online) {
      free_char(victim);
    }
    return TRUE;

  }

  bool text_to_person(CHAR_DATA *ch, int numberdialed, int sourcenumber, char * msg, bool picture)
  {
    if(numberdialed == 0)
    return FALSE;
    CHAR_DATA *victim = NULL;
    OBJ_DATA *obj = NULL;
    bool online = FALSE;
    int from = sourcenumber;
    bool phonefound = FALSE;
    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
        continue;
      }
      else if (IS_SET(obj->extra_flags, ITEM_OFF)) {
        continue;
      }
      else {
        phonefound = TRUE;
        victim = obj->carried_by;
        online = true;
        break;
      }
    }

    if (phonefound == FALSE ) {
      char phonename[MSL];
      bool bookfound = FALSE;
      for (vector<PHONEBOOK_TYPE *>::iterator pit = PhoneVect.begin();
      pit != PhoneVect.end(); ++pit) {
        if ((*pit)->number == numberdialed) {
          sprintf(phonename, "%s", (*pit)->owner);
          bookfound = TRUE;
        }
      }
      if (bookfound == FALSE) {
        return FALSE;
      }
      DESCRIPTOR_DATA d;

      d.original = NULL;
      if ((victim = get_char_world_pc_noname(NULL, phonename)) != NULL) {
        online = TRUE;
      }
      else {
        if ((victim = get_char_world_pc(phonename)) != NULL) {
          online = TRUE;
        }
        else {
          if (!load_char_obj(&d, phonename)) {
            return FALSE;
          }
          victim = d.character;
        }
      }
      if (IS_NPC(victim)) {
        if (!online) {
          free_char(victim);
        }
        return FALSE;
      }
      obj = find_phone(victim, numberdialed);
    }
    if(victim == NULL)
    {
      log_string("Null group text target");
      return FALSE;
    }

    if(obj == NULL)
    {
      if(victim != NULL && !online)
      free_char(victim);
      return FALSE;
    }
    if (IS_SET(obj->extra_flags, ITEM_OFF)) {
      if(victim != NULL && !online)
      free_char(victim);
      return FALSE;
    }

    if(safe_strlen(obj->material) > 16000)
    {
      if(victim != NULL && !online)
      free_char(victim);

      return FALSE;
    }

    int vref = -1;
    for (int i = 0; i < 50; i++) {
      if(sourcenumber == victim->pcdata->speed_numbers[i]) {
        vref = i;
      }
    }
    int cref = -1;
    for (int i = 0; i < 50; i++) {
      if(numberdialed == ch->pcdata->speed_numbers[i]) {
        cref = i;
      }
    }
    char sname[MSL];
    char buf3[MSL];
    if(vref >= 0)
    sprintf(sname, "%s", victim->pcdata->speed_names[vref]);
    else
    sprintf(sname, "%d", sourcenumber);

    char outbuf[MSL];
    char tbuf[MSL];
    if(picture == true)
    {
      to_photo_message(victim, msg);
      sprintf(outbuf, "%s\n[%s]%s: A photo message(look photos to view)", obj->material, newtexttime(), str_dup(sname));
      sprintf(tbuf, "[%s]%s: A photo message(look photos to view)", newtexttime(), str_dup(sname));
    }
    else
    {
      sprintf(outbuf, "%s\n[%s]%s: %s", obj->material, newtexttime(), str_dup(sname), from_color(distort_text(msg)));
      sprintf(tbuf, "[%s]%s: %s", newtexttime(), str_dup(sname), from_color(distort_text(msg)));
      sprintf(buf3, "[%s]%s: %s", newtexttime(), str_dup(sname), from_color(distort_text(msg)));
    }
    char buf2[MSL];
    if(ch != NULL)
    {
      if (obj->value[4] > 0) {
        hack_text(from, msg, obj->value[4], obj->value[0]);
      }
      if (get_phone(ch) != NULL && get_phone(ch)->value[4] > 0) {
        hack_text(from, msg, get_phone(ch)->value[4], obj->value[0]);
      }

      for (DescList::iterator dit = descriptor_list.begin();
      dit != descriptor_list.end(); ++dit) {
        DESCRIPTOR_DATA *dtt = *dit;
        CHAR_DATA *to;

        if (dtt->character != NULL && dtt->connected == CON_PLAYING) {
          to = dtt->character;
          if (IS_NPC(to)) {
            continue;
          }
          if (to->in_room == NULL || ch->in_room == NULL) {
            continue;
          }

          if (is_ghost(to) && (to->possessing == ch || to->possessing == victim)) {
            if (to->possessing == ch) {
              printf_to_char(to, "%s sends a text: %s\n\r", PERS(to->possessing, to), buf3);
            }
            else {
              printf_to_char(to, "%s receives a text: %s\n\r", PERS(to->possessing, to), buf3);
            }
          }

          if ((!IS_FLAG(to->act, PLR_SPYING) || IS_FLAG(victim->act, PLR_SPYSHIELD) || !can_spy(to, ch))) {
            continue;
          }
          if ((!IS_FLAG(to->act, PLR_SPYING) || IS_FLAG(ch->act, PLR_SPYSHIELD) || !can_spy(to, victim))) {
            continue;
          }

          printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(msg));
        }
      }
      char rplog[MSL];
      sprintf(rplog, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(msg));
      rp_log(rplog);
      sprintf(rplog, "You get a text: %s", buf3);
      prp_rplog(victim, rplog);

      if (number_percent() % 2 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5) {
        printf_to_char(get_tracer(ch), "You intercept a text: %s\n\r", msg);
      }

      if (IS_NPC(ch) || ch->pcdata->institute_action == 0) {
        rpreward(ch, msg, TRUE, 1);
      }

    }

    free_string(obj->material);
    obj->material = str_dup(outbuf);

    if (online) {
      if (!is_gm(victim)) {
        if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
          act("Your $p vibrates.", victim, obj, NULL, TO_CHAR);
        }
        else {
          act("Your $p beeps.", victim, obj, NULL, TO_CHAR);
          act("$n's $p beeps.", victim, obj, NULL, TO_ROOM);
        }
        if(holding_phone(victim))
        printf_to_char(victim, "%s\n\r", tbuf);
      }
    }

    save_char_obj(victim, FALSE, FALSE);
    if (!online) {
      free_char(victim);
    }
    return TRUE;
  }

  _DOFUN(do_text) {
    smash_tilde(argument);
    smash_vector(argument);
    int numberdialed = 0;
    CHAR_DATA *victim = NULL;
    OBJ_DATA *obj = NULL;
    int i, from, ref = -1;
    char buf[MSL];
    char tbuf[MSL];
    char buf2[MSL];
    char buf3[MSL];
    char arg1[MSL];
    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    int twcount = UMAX(1, safe_strlen(argument) / 8);
    char tout[MSL];

    if (ch->pcdata->ci_editing == 8) {
      string_append(ch, &ch->pcdata->ci_desc);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (ch->wounds >= 3 && !is_undead(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!holding_phone(ch) && !electropathic(ch)) {
      send_to_char("You must be holding a phone first.\n\r", ch);
      return;
    }
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID) {
      send_to_char("Damn your lack of opposable thumbs!\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD) || (is_ghost(ch) && !electropathic(ch))) {
      send_to_char("You can't get a connection.\n\r", ch);
      return;
    }

    if (in_fight(ch) || ch->hit <= 0) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }

    if(electropathic(ch) && arg1[0] == '@')
    {
      PROFILE_TYPE *tprofile = profile_handle_lookup(arg1);
      if(tprofile == NULL)
      {
        send_to_char("No such profile.\n\r", ch);
        return;
      }
      MATCH_TYPE * match = match_find_by_name(ch->name, tprofile->name);
      if(match == NULL || match->status_one != 1 || match->status_two != 1)
      {
        send_to_char("You have not matched with them.\n\r", ch);
        return;
      }
      bool sent = dm_to_person(ch, tprofile->name, argument, FALSE);
      return;
    }


    if(get_phone(ch) != NULL && get_eq_char(ch, WEAR_HOLD) != NULL)
    {
      if (IS_SET(get_eq_char(ch, WEAR_HOLD)->extra_flags, ITEM_OFF)) {
        send_to_char("Your phone is off.\n\r", ch);
        return;
      }
    }

    if (get_phone(ch) != NULL && get_phone(ch)->item_type == ITEM_PHONE) {
      if (get_phone(ch)->material == NULL) {
        get_phone(ch)->material = str_dup("");
      }
      if (safe_strlen(get_phone(ch)->material) > 15000 && str_cmp(arg1, "clear")) {
        send_to_char("Your phone gives a text memory full error, please delete previous text messages(text clear)\n\r", ch);
        return;
      }
      if (!is_name("phone", get_phone(ch)->name)) {
        send_to_char("Phones must include the word 'phone' in their name to function.\n\r", ch);
        return;
      }
    }

    if(!str_cmp(arg1, "history"))
    {
      char obuf[MSL];
      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
      it != GTextVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if(!str_cmp((*it)->tname, argument))
        {
          sprintf(obuf, "%s History:\n%s\n\r", (*it)->tname, (*it)->history);
          page_to_char(obuf, ch);
          return;
        }
      }
      PROFILE_TYPE * tprof = profile_handle_lookup(argument);
      for (vector<TEXTHISTORY_TYPE *>::iterator hit = HTextVect.begin();
      hit != HTextVect.end(); ++hit) {
        if ((*hit)->valid == FALSE)
        continue;
        if(!str_cmp((*hit)->name_one, ch->name) && !str_cmp((*hit)->name_two, argument))
        {
          sprintf(obuf, "%s History:\n%s\n\r", (*hit)->name_two, (*hit)->history);
          page_to_char(obuf, ch);
          return;
        }
        if(!str_cmp((*hit)->name_two, ch->name) && !str_cmp((*hit)->name_one, argument))
        {
          sprintf(obuf, "%s History:\n%s\n\r", (*hit)->name_one, (*hit)->history);
          page_to_char(obuf, ch);
          return;
        }
        if(tprof != NULL && !str_cmp((*hit)->name_one, ch->name) && !str_cmp((*hit)->name_two, tprof->name))
        {
          sprintf(obuf, "%s History:\n%s\n\r", (*hit)->name_two, (*hit)->history);
          page_to_char(obuf, ch);
          return;
        }
        if(tprof != NULL && !str_cmp((*hit)->name_two, ch->name) && !str_cmp((*hit)->name_one, tprof->name))
        {
          sprintf(obuf, "%s History:\n%s\n\r", (*hit)->name_one, (*hit)->history);
          page_to_char(obuf, ch);
          return;
        }

      }
      send_to_char("No history found.\n\r", ch);
      return;
    }

    if (get_phone(ch) == NULL) {
      send_to_char("You do not seem to have a phone.\n\r", ch);
      return;
    }

    from = get_phone(ch)->value[0];

    if (!str_cmp(arg1, "clear")) {
      free_string(get_phone(ch)->material);
      get_phone(ch)->material = str_dup("");
      send_to_char("Messages cleared.\n\r", ch);
      return;
    }

    if (!cell_signal(ch)) {
      send_to_char("You can't get a connection.\n\r", ch);
      return;
    }

    if(arg1[0] == '@') {
      PROFILE_TYPE *tprofile = profile_handle_lookup(arg1);
      if(tprofile == NULL)
      {
        send_to_char("No such profile.\n\r", ch);
        return;
      }
      MATCH_TYPE * match = match_find_by_name(ch->name, tprofile->name);
      if(match == NULL || match->status_one != 1 || match->status_two != 1)
      {
        send_to_char("You have not matched with them.\n\r", ch);
        return;
      }
      bool sent = dm_to_person(ch, tprofile->name, argument, FALSE);
      return;
    }

    bool mod_flagged = FALSE;
    if (ch->played / 3600 < 50)
    mod_flagged = isTextFlagged(argument);


    if (room_hostile(ch->in_room))
    {
      start_hostilefight(ch);
      return;
    }

    for (i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->speed_names[i], arg1)) {
        numberdialed = ch->pcdata->speed_numbers[i];
      }
    }

    if(numberdialed == 0) {
      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();it != GTextVect.end(); ++it)
      {
        if ((*it)->valid == FALSE)
        continue;
        if(!str_cmp((*it)->tname, arg1))
        {
          bool inchain = FALSE;
          for(int i=0;i<10;i++)
          {
            if((*it)->pnumber[i] == from)
            inchain = TRUE;
          }
          if(inchain == TRUE)
          {
            if(mod_flagged == TRUE)
            {
              send_to_char("Group Text Sent.\n\r", ch);
              return;
            }
            (*it)->last_msg = current_time;
            char garg[MSL];
            sprintf(garg, "(%s)%s", (*it)->tname, argument);
            char rplog[MSL];
            sprintf(rplog, "You text %s %s", (*it)->tname, argument);
            prp_rplog(ch, rplog);
            for(int i=0;i<10;i++)
            {
              if((*it)->pnumber[i] > 0 && (*it)->pnumber[i] != from)
              {
                text_to_person(ch, (*it)->pnumber[i], from, str_dup(garg), FALSE);
              }
            }
            send_to_char("Group Text Sent.\n\r", ch);
            char hbuf[MSL];
            sprintf(hbuf, "[%s]%s: %s", newtexttime(), ch->name, argument);
            char * tmpl = appendLine((*it)->history, str_dup(hbuf));
            free_string((*it)->history);
            (*it)->history = str_dup(tmpl);

            sprintf(tout, "$n types a %d word message on their phone.", twcount);

            if (!is_gm(ch)) {
              act(tout, ch, NULL, NULL, TO_ROOM);
            }
            return;

          }
          break;
        }
      }
    }

    if (numberdialed == 0) {
      numberdialed = atoi(arg1);
    }

    bool phonefound = FALSE;

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
        continue;
      }
      else if (IS_SET(obj->extra_flags, ITEM_OFF)) {
        continue;
      }
      else {
        phonefound = TRUE;
        break;
      }
    }

    if (phonefound == FALSE) {
      char phonename[MSL];
      bool bookfound = FALSE;
      for (vector<PHONEBOOK_TYPE *>::iterator it = PhoneVect.begin();
      it != PhoneVect.end(); ++it) {
        if ((*it)->number == numberdialed) {
          sprintf(phonename, "%s", (*it)->owner);
          bookfound = TRUE;
        }
      }
      if (bookfound == FALSE) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      d.original = NULL;
      if ((victim = get_char_world_pc_noname(ch, phonename)) != NULL) {
        online = TRUE;
      }
      else {
        if ((victim = get_char_world_pc(phonename)) != NULL) {
          online = TRUE;
        }
        else {
          if (!load_char_obj(&d, phonename)) {
            printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(phonename));
          stat(buf, &sb);
          victim = d.character;
        }
      }

      if (IS_NPC(victim)) {
        if (!online) {
          free_char(victim);
        }
        return;
      }
      if (online) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      for (i = 0; i < 50; i++) {
        if (from == victim->pcdata->speed_numbers[i]) {
          ref = i;
        }
      }

      WAIT_STATE(ch, PULSE_PER_SECOND * 5);

      if (find_phone(victim, numberdialed) == NULL) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        if (!online) {
          free_char(victim);
        }
        return;
      }
      if (IS_SET(find_phone(victim, numberdialed)->extra_flags, ITEM_OFF)) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        if (!online) {
          free_char(victim);
        }
        return;
      }

      for (i = 0; i < 50; i++) {
        if (!str_cmp(victim->name, ch->pcdata->speed_names[i])) {
          if (ch->pcdata->speed_numbers[i] !=
              find_phone(victim, numberdialed)->value[0]) {
            send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
            if (!online) {
              free_char(victim);
            }
            return;
          }
        }
      }

      ch->pcdata->time_since_emote = 0;

      if (ref == -1) {
        sprintf(buf, "%s\n[%s]%d: %s", find_phone(victim, numberdialed)->material, newtexttime(), from, from_color(distort_text(argument)));
        sprintf(tbuf, "[%s]%d: %s", newtexttime(), from, from_color(distort_text(argument)));
        sprintf(buf3, "[%s]%d: %s", newtexttime(), from, from_color(distort_text(argument)));
      }
      else {
        sprintf(buf, "%s\n[%s]%s: %s", find_phone(victim, numberdialed)->material, newtexttime(), victim->pcdata->speed_names[ref], from_color(distort_text(argument)));
        sprintf(tbuf, "[%s]%s: %s", newtexttime(), victim->pcdata->speed_names[ref], from_color(distort_text(argument)));
        sprintf(buf3, "[%s]%s: %s", newtexttime(), victim->pcdata->speed_names[ref], from_color(distort_text(argument)));
      }

      if (get_phone(victim)->value[4] > 0) {
        hack_text(from, argument, find_phone(victim, numberdialed)->value[4], find_phone(victim, numberdialed)->value[0]);
      }
      
      if (get_phone(ch) != NULL && get_phone(ch)->value[4] > 0) {
        hack_text(from, argument, get_phone(ch)->value[4], get_phone(victim)->value[0]);
      }

      if (ch->in_room == victim->in_room && ch != victim)
      social_behave_mod(ch, -3, "texting person in same room");

      give_attention(ch, victim);
      for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it)
      {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to)) {
            continue;
          }
          if (to->in_room == NULL || ch->in_room == NULL) {
            continue;
          }

          if (is_ghost(to) && (to->possessing == ch || to->possessing == victim)) {
            if (to->possessing == ch) {
              printf_to_char(to, "%s sends a text: %s\n\r", PERS(to->possessing, to), buf3);
            }
            else {
              printf_to_char(to, "%s receives a text: %s\n\r", PERS(to->possessing, to), buf3);
            }
          }

          if ((!IS_FLAG(to->act, PLR_SPYING) || IS_FLAG(victim->act, PLR_SPYSHIELD) || !can_spy(to, ch))) {
            continue;
          }
          if ((!IS_FLAG(to->act, PLR_SPYING) || IS_FLAG(ch->act, PLR_SPYSHIELD) || !can_spy(to, victim))) {
            continue;
          }

          printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
        }
      }
      char rplog[MSL];
      sprintf(rplog, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
      rp_log(rplog);
      sprintf(rplog, "You text %s %s", arg1, argument);
      prp_rplog(ch, rplog);
      sprintf(rplog, "You get a text: %s", buf3);
      prp_rplog(victim, rplog);

      if (number_percent() % 2 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5) {
        printf_to_char(get_tracer(ch), "You intercept a text: %s\n\r", argument);
      }

      if (IS_NPC(ch) || ch->pcdata->institute_action == 0) {
        rpreward(ch, argument, TRUE, 1);
      }

      if (mod_flagged == FALSE) {
        free_string(find_phone(victim, numberdialed)->material);
        find_phone(victim, numberdialed)->material = str_dup(buf);
      }
      send_to_char("Sent.\n\r", ch);

      sprintf(tout, "$n types a %d word message on their phone.", twcount);
      if(ch != NULL && victim != NULL)
      {
        MATCH_TYPE * match = match_find_by_name(ch->name, victim->name);
        if(match != NULL)
        {
          if(!str_cmp(match->nameone, ch->name))
          {
            match->last_msg_one_three = match->last_msg_one_two;
            match->last_msg_one_two = match->last_msg_one_one;
            match->last_msg_one_one = current_time;
          }
          else
          {
            match->last_msg_two_three = match->last_msg_two_two;
            match->last_msg_two_two = match->last_msg_two_one;
            match->last_msg_two_one = current_time;
          }
        }

        char hbuf[MSL];
        sprintf(hbuf, "[%s]%s: %s", newtexttime(), ch->name, argument);
        bool hexist = FALSE;
        for (vector<TEXTHISTORY_TYPE *>::iterator hit = HTextVect.begin();
        hit != HTextVect.end(); ++hit) {
          if ((*hit)->valid == FALSE)
          continue;
          if(!str_cmp(ch->name, (*hit)->name_one) && !str_cmp(victim->name, (*hit)->name_two))
          {
            hexist = TRUE;
            char * tmpl = appendLine((*hit)->history, str_dup(hbuf));
            free_string((*hit)->history);
            (*hit)->history = str_dup(tmpl);
            (*hit)->last_msg = current_time;
          }
          else if(!str_cmp(ch->name, (*hit)->name_two) && !str_cmp(victim->name, (*hit)->name_one))
          {
            hexist = TRUE;
            char * tmpl = appendLine((*hit)->history, str_dup(hbuf));
            free_string((*hit)->history);
            (*hit)->history = str_dup(tmpl);
            (*hit)->last_msg = current_time;
          }
        }

        if(hexist == FALSE)
        {
          TEXTHISTORY_TYPE *ht = new TEXTHISTORY_TYPE;
          ht->valid = TRUE;
          ht->name_one = str_dup(ch->name);
          ht->name_two = str_dup(victim->name);
          ht->history = str_dup(hbuf);
          ht->last_msg = current_time;
          HTextVect.push_back(ht);
        }

      }
      if (!is_gm(ch)) {
        act(tout, ch, NULL, NULL, TO_ROOM);
      }

      save_char_obj(victim, FALSE, FALSE);
      if (!online) {
        free_char(victim);
      }

      return;
    }

    if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }

    bool noowner = FALSE;
    if (obj->carried_by != NULL && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
      victim = obj->carried_by;
    }
    else {
      if (obj->in_obj != NULL && obj->in_obj->carried_by != NULL && !IS_SET(obj->in_obj->extra_flags, ITEM_WARDROBE)) {
        victim = obj->in_obj->carried_by;
      }
      else {
        noowner = TRUE;
      }
    }

    if (noowner == FALSE) {
      if (IS_NPC(victim)) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      if (IS_FLAG(victim->act, PLR_DEAD)) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      for (i = 0; i < 50; i++) {
        if (from == victim->pcdata->speed_numbers[i]) {
          ref = i;
        }
      }
    }

    WAIT_STATE(ch, PULSE_PER_SECOND * 5);

    if (obj->value[4] > 0) {
      hack_text(from, argument, obj->value[4], obj->value[0]);
    }
    if (get_phone(ch) != NULL && get_phone(ch)->value[4] > 0) {
      hack_text(from, argument, get_phone(ch)->value[4], obj->value[0]);
    }

    if (ref == -1 || noowner == TRUE) {
      sprintf(buf, "%s\n[%s]%d: %s", obj->material, newtexttime(), from, from_color(distort_text(argument)));
      sprintf(tbuf, "[%s]%d: %s", newtexttime(), from, from_color(distort_text(argument)));
      sprintf(buf3, "[%s]%d: %s", newtexttime(), from, from_color(distort_text(argument)));
    }
    else {
      sprintf(buf, "%s\n[%s]%s: %s", obj->material, newtexttime(), victim->pcdata->speed_names[ref], from_color(distort_text(argument)));
      sprintf(tbuf, "[%s]%s: %s", newtexttime(), victim->pcdata->speed_names[ref], from_color(distort_text(argument)));
      sprintf(buf3, "[%s]%s: %s", newtexttime(), victim->pcdata->speed_names[ref], from_color(distort_text(argument)));
    }

    if (noowner == FALSE) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (to == NULL || victim == NULL || ch == NULL) {
            continue;
          }
          if (IS_NPC(to)) {
            continue;
          }
          if (to->in_room == NULL || ch->in_room == NULL) {
            continue;
          }

          if (is_ghost(to) && (to->possessing == ch || to->possessing == victim))
          printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, argument);

          if ((!IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, ch))) {
            continue;
          }
          if ((!IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, victim))) {
            continue;
          }

          printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
        }
      }
      if (victim != NULL) {
        char rplog[MSL];
        sprintf(rplog, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
        rp_log(rplog);
      }
    }

    if (number_percent() % 3 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5)
    printf_to_char(get_tracer(ch), "You intercept a text: %s\n\r", argument);

    if (IS_NPC(ch) || ch->pcdata->institute_action == 0)
    rpreward(ch, argument, TRUE, 1);

    if (noowner == FALSE) {
      if (ch->in_room == victim->in_room && ch != victim)
      social_behave_mod(ch, -3, "texting person in same room");

      give_attention(ch, victim);
    }
    sprintf(tout, "$n types a %d word message on their phone.", twcount);

    if(ch != NULL && victim != NULL)
    {
      MATCH_TYPE * match = match_find_by_name(ch->name, victim->name);
      if(match != NULL)
      {
        if(!str_cmp(match->nameone, ch->name))
        {
          match->last_msg_one_three = match->last_msg_one_two;
          match->last_msg_one_two = match->last_msg_one_one;
          match->last_msg_one_one = current_time;
        }
        else
        {
          match->last_msg_two_three = match->last_msg_two_two;
          match->last_msg_two_two = match->last_msg_two_one;
          match->last_msg_two_one = current_time;
        }
      }

      char hbuf[MSL];
      sprintf(hbuf, "[%s]%s: %s", newtexttime(), ch->name, argument);
      bool hexist = FALSE;
      for (vector<TEXTHISTORY_TYPE *>::iterator hit = HTextVect.begin();
      hit != HTextVect.end(); ++hit) {
        if ((*hit)->valid == FALSE)
        continue;
        if(!str_cmp(ch->name, (*hit)->name_one) && !str_cmp(victim->name, (*hit)->name_two))
        {
          hexist = TRUE;
          char * tmpl = appendLine((*hit)->history, str_dup(hbuf));
          free_string((*hit)->history);
          (*hit)->history = str_dup(tmpl);
          (*hit)->last_msg = current_time;
        }
        else if(!str_cmp(ch->name, (*hit)->name_two) && !str_cmp(victim->name, (*hit)->name_one))
        {
          hexist = TRUE;
          char * tmpl = appendLine((*hit)->history, str_dup(hbuf));
          free_string((*hit)->history);
          (*hit)->history = str_dup(tmpl);
          (*hit)->last_msg = current_time;
        }
      }

      if(hexist == FALSE)
      {
        TEXTHISTORY_TYPE *ht = new TEXTHISTORY_TYPE;
        ht->valid = TRUE;
        ht->name_one = str_dup(ch->name);
        ht->name_two = str_dup(victim->name);
        ht->history = str_dup(hbuf);
        ht->last_msg = current_time;
        HTextVect.push_back(ht);
      }
    }

    if (!is_gm(ch))
    act(tout, ch, NULL, NULL, TO_ROOM);
    send_to_char("Sent.\n\r", ch);

    if (mod_flagged == FALSE) {
      free_string(obj->material);
      obj->material = str_dup(buf);

      char rplog[MSL];
      sprintf(rplog, "You text %s %s", arg1, argument);
      prp_rplog(ch, rplog);
      sprintf(rplog, "You get a text: %s", buf3);
      prp_rplog(victim, rplog);

      if (noowner == FALSE) {
        if (!is_gm(victim)) {
          if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
            act("Your $p vibrates.", victim, obj, NULL, TO_CHAR);
          }
          else {
            act("Your $p beeps.", victim, obj, NULL, TO_CHAR);
            act("$n's $p beeps.", victim, obj, NULL, TO_ROOM);
          }
          if(holding_phone(victim))
          printf_to_char(victim, "%s\n\r", tbuf);
        }
      }
    }
  }

  _DOFUN(do_teletext) {
    int numberdialed = 0;
    CHAR_DATA *victim;
    OBJ_DATA *obj = NULL;
    int i, ref = -1;
    char buf[MSL];
    char buf2[MSL];
    char arg1[MSL];

    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    argument = one_argument_nouncap(argument, arg1);

    if (get_skill(ch, SKILL_ELECTROPATHIC) < 1)
    return;

    if (is_blind(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You can't get a connection.\n\r", ch);
      return;
    }
    if (!cell_signal(ch)) {
      send_to_char("You can't get a connection.\n\r", ch);
      return;
    }

    if (in_fight(ch) || ch->hit <= 0) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }

    for (i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->speed_names[i], arg1))
      numberdialed = ch->pcdata->speed_numbers[i];
    }
    if (numberdialed == 0)
    numberdialed = atoi(arg1);

    bool phonefound = FALSE;
    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed)
      continue;
      else if (IS_SET(obj->extra_flags, ITEM_OFF))
      continue;
      else {
        phonefound = TRUE;
        break;
      }
    }

    if (phonefound == FALSE) {
      d.original = NULL;
      if ((victim = get_char_world_pc_noname(ch, arg1)) !=
          NULL) // Victim is online.
      {
        online = TRUE;
      }
      else {
        if ((victim = get_char_world_pc(arg1)) != NULL) // Victim is online.
        {
          online = TRUE;
        }
        else {
          log_string("DESCRIPTOR: Offline Text");

          if (!load_char_obj(&d, arg1)) {
            printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg1));
          stat(buf, &sb);
          victim = d.character;
        }
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      if (online) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }

      WAIT_STATE(ch, PULSE_PER_SECOND * 5);

      if (get_phone(victim) == NULL) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }
      if (IS_SET(get_phone(victim)->extra_flags, ITEM_OFF)) {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }
      for (i = 0; i < 50; i++) {
        if (!str_cmp(victim->name, ch->pcdata->speed_names[i])) {
          if (ch->pcdata->speed_numbers[i] != get_phone(victim)->value[0]) {
            send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
            if (!online)
            free_char(victim);
            return;
          }
        }
      }

      ch->pcdata->time_since_emote = 0;

      if (get_phone(victim)->value[4] > 0)
      hack_text(0, argument, get_phone(victim)->value[4], get_phone(victim)->value[0]);

      if (ref == -1)
      sprintf(buf, "%s\n[%s]Unknown: %s", get_phone(victim)->material, newtexttime(), from_color(argument));
      else
      sprintf(buf, "%s\n[%s]Unknown: %s", get_phone(victim)->material, newtexttime(), from_color(argument));

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;
          if (to->in_room == NULL || ch->in_room == NULL)
          continue;
          if ((!IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, ch) || IS_FLAG(victim->act, PLR_SPYSHIELD)))
          continue;
          if ((!IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, victim) || IS_FLAG(ch->act, PLR_SPYSHIELD)))
          continue;

          printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
        }
      }
      char rplog[MSL];
      sprintf(rplog, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
      rp_log(rplog);

      if (number_percent() % 3 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5)
      printf_to_char(get_tracer(ch), "You intercept a text: %s\n\r", from_color(argument));

      if (IS_NPC(ch) || ch->pcdata->institute_action == 0)
      rpreward(ch, argument, TRUE, 1);

      free_string(get_phone(victim)->material);
      get_phone(victim)->material = str_dup(buf);

      send_to_char("Sent.\n\r", ch);

      save_char_obj(victim, FALSE, FALSE);
      if (!online)
      free_char(victim);

      return;
    }

    if (obj->item_type != ITEM_PHONE || obj->value[0] != numberdialed) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }

    if (obj->carried_by != NULL && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
      victim = obj->carried_by;
    }
    else {
      if (obj->in_obj != NULL && obj->in_obj->carried_by != NULL && !IS_SET(obj->in_obj->extra_flags, ITEM_WARDROBE))
      victim = obj->in_obj->carried_by;
      else {
        send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
        return;
      }
    }

    if (IS_NPC(victim)) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->act, PLR_DEAD)) {
      send_to_char("I'm sorry, the number you have dialed is not available.\n\r", ch);
      return;
    }

    if (obj->value[4] > 0)
    hack_text(0, argument, obj->value[4], obj->value[0]);

    WAIT_STATE(ch, PULSE_PER_SECOND * 5);

    if (ref == -1)
    sprintf(buf, "%s\n[%s]Unknown: %s", obj->material, newtexttime(), from_color(argument));
    else
    sprintf(buf, "%s\n[%s]Unknown: %s", obj->material, newtexttime(), from_color(argument));

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (to == NULL || victim == NULL || ch == NULL)
        continue;
        if (IS_NPC(to))
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;
        if ((!IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, ch)))
        continue;
        if ((!IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, victim)))
        continue;

        printf_to_char(to, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
      }
    }
    send_to_char("Sent.\n\r", ch);

    char rplog[MSL];
    sprintf(rplog, "%s texts %s: %s\n\r", ch->name, victim->name, from_color(argument));
    rp_log(rplog);

    free_string(obj->material);
    obj->material = str_dup(buf);

    if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
      act("Your $p vibrates.", victim, obj, NULL, TO_CHAR);
    }
    else {
      act("Your $p beeps.", victim, obj, NULL, TO_CHAR);
      act("$n's $p beeps.", victim, obj, NULL, TO_ROOM);
    }
  }

  void charge_influence(CHAR_DATA *ch, int type, int amount) {
    if (type == INFLUENCE_SCHEME) {
      if (amount < ch->pcdata->scheme_influence) {
        ch->pcdata->scheme_influence -= amount;
        return;
      }
      amount -= ch->pcdata->scheme_influence;
      ch->pcdata->scheme_influence = 0;
      if (amount < ch->pcdata->influence) {
        ch->pcdata->influence -= amount;
        return;
      }
      amount -= ch->pcdata->influence;
      ch->pcdata->influence = 0;
      ch->pcdata->super_influence -= amount;
      return;
    }
    if (type == INFLUENCE_FACTION) {
      if (amount < ch->pcdata->faction_influence) {
        ch->pcdata->faction_influence -= amount;
        return;
      }
      amount -= ch->pcdata->faction_influence;
      ch->pcdata->faction_influence = 0;
      if (amount < ch->pcdata->influence) {
        ch->pcdata->influence -= amount;
        return;
      }
      amount -= ch->pcdata->influence;
      ch->pcdata->influence = 0;
      ch->pcdata->super_influence -= amount;
      return;
    }
    if (type == INFLUENCE_SUPER) {
      if (amount < ch->pcdata->super_influence) {
        ch->pcdata->super_influence -= amount;
        return;
      }
      amount -= ch->pcdata->super_influence;
      ch->pcdata->super_influence = 0;
      if (amount < ch->pcdata->influence) {
        ch->pcdata->influence -= amount;
        return;
      }
      amount -= ch->pcdata->influence;
      ch->pcdata->influence = 0;
      ch->pcdata->super_influence -= amount;
      return;
    }

    ch->pcdata->influence -= amount;
  }

  bool has_goblinkey(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->faction != 0)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool has_symbol_core(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->faction == ch->fcore && clan_lookup(ch->fcore) != NULL && safe_strlen(clan_lookup(ch->fcore)->symbol) > 3)
        return TRUE;
      }
    }
    return FALSE;
  }


  bool has_symbol_cult(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->faction == ch->fcult && clan_lookup(ch->fcult) != NULL && safe_strlen(clan_lookup(ch->fcult)->symbol) > 3)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool has_symbol_sect(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->faction == ch->fsect && clan_lookup(ch->fsect) != NULL && safe_strlen(clan_lookup(ch->fsect)->symbol) > 3)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool has_other_symbol(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->faction != 0 && obj->faction != ch->fcore && obj->faction != ch->fsect && obj->faction != ch->fcult && clan_lookup(obj->faction) != NULL && safe_strlen(clan_lookup(obj->faction)->symbol) > 3)
        return TRUE;
      }
    }
    return FALSE;
  }

  void hack_speech(CHAR_DATA *ch, CHAR_DATA *victim, char *rectalk, char *argument, int tonumber) {
    bool phonefound = FALSE;
    OBJ_DATA *obj;

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE || obj->value[0] != tonumber)
      continue;
      else if (IS_SET(obj->extra_flags, ITEM_OFF))
      continue;
      else {
        phonefound = TRUE;
        break;
      }
    }
    if (!phonefound)
    return;
    if (obj->carried_by == NULL)
    return;
    CHAR_DATA *to = obj->carried_by;
    if (get_phone(to) != obj)
    return;

    char buf[MSL];

    if (is_deaf(to))
    sprintf(buf, "Your phone says something'\n\r");
    else if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
    sprintf(buf, "Your phone says, %s, '`o%s`x'\n\r", rectalk, static_text(ch, to, mangle_text(ch, to, ch->pcdata->speaking, argument)));
    else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
    sprintf(buf, "Your phone says, %s, '`o%s`x'\n\r", ch->pcdata->talk, static_text(ch, to, mangle_text(ch, to, ch->pcdata->speaking, argument)));
    else
    sprintf(buf, "Your phone says '`o%s`x'\n\r", static_text(ch, to, mangle_text(ch, to, ch->pcdata->speaking, argument)));

    page_to_char(wrap_string(buf, get_wordwrap(to)), to);
  }

  char *speaker(CHAR_DATA *ch, CHAR_DATA *to) {
    if (!can_see(to, ch))
    return "Someone";
    if (ch->in_room == to->in_room)
    return PERS(ch, to);
    if (can_see_char_distance(to, ch, DISTANCE_MEDIUM))
    return PERS(ch, to);
    return "Someone";
  }
  
  char *speaker_2(CHAR_DATA *ch, CHAR_DATA *to) {
    if (!can_see(to, ch))
    return "Someone";
    if (ch->in_room == to->in_room)
    return PERS_2(ch, to);
    if (can_see_char_distance(to, ch, DISTANCE_MEDIUM))
    return PERS_2(ch, to);
    return "Someone";
  }

  void dream_spy(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *to;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (!is_gm(to) && !higher_power(to))
        continue;

        if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
          if (ch->pcdata->dream_room > 0)
          printf_to_char(to, "Dream: %s: %s\n\r", get_room_index(ch->pcdata->dream_room)->name, argument);
          else
          printf_to_char(to, "Dreamworld: %s\n\r", argument);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "Dreamworld: %s\n\r", argument);
    rp_log(rplog);
  }

  char *optional_distance(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf[50];
    if (ch->in_room == victim->in_room)
    return "";
    else {
      sprintf(buf, "[%s]", room_distance(ch, victim->in_room));
      return str_dup(buf);
    }
  }

  _DOFUN(do_say) {
    CHAR_DATA *to;
    char buf[MSL];
    char arg1[MSL];
    char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
    if (!IS_NPC(ch))
    ch->pcdata->time_since_emote = 0;
    int volume = VOLUME_NORMAL;

    smash_percent(argument);
    smash_tilde(argument);
    if (!IS_IMMORTAL(ch)) {
      smash_vector(argument);
    }

    if (ch->hit < -7) {
      send_to_char("You're hurt too bad to talk.\n\r", ch);
      return;
    }
    if (is_mute(ch) && !is_dreaming(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->comm, COMM_GAG) && !is_dreaming(ch)) {
      sprintf(buf, "tries to say, \"%s\"", argument);
      do_function(ch, &do_emote, buf);
      return;
    }

    if (ch->in_room->vnum == 60)
    return;

    if (crowded_room(ch->in_room) && safe_strlen(ch->pcdata->place) < 2) {
      send_to_char("Find a place first.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0 && !is_dreaming(ch)) {
      send_to_char("You're unconcious.\n\r", ch);
      return;
    }
    if (in_fight(ch) && ch->fight_fast == 0 && ch->fight_current != NULL && ch->fight_current != ch) {
      send_to_char("It isn't your turn yet.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch)) {
      char nocol[MSL];
      remove_color(nocol, ch->pcdata->talk);
      free_string(ch->pcdata->talk);
      ch->pcdata->talk = str_dup(nocol);
      remove_color(nocol, argument);
      free_string(argument);
      argument = str_dup(nocol);
    }

    if (argument[0] == '\0') {
      send_to_char("Say what?\n\r", ch);
      return;
    }
    FANTASY_TYPE *fant;
    if (is_dreaming(ch)) {
      if ((fant = in_fantasy(ch)) != NULL) {
        if (part_of_fantasy(ch, fant)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->name, fant->participants[i]) && fant->participant_noaction[i] == TRUE) {
              send_to_char("You can't act in this dreamworld at the moment.\n\r", ch);
              return;
            }
          }
        }
      }
    }

    etag(ch);

    if (IS_FLAG(ch->act, PLR_HIDE))
    do_function(ch, &do_unhide, "");

    triggercheck(ch, argument);

    if (strcasestr(argument, "Revocacionem protectione sanctuarium") != NULL && str_cmp(ch->pcdata->understanding, "None")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("None");
      nounderglow(ch);
    }
    else if (strcasestr(argument, "Repeto praesidio sanctuarium") != NULL && str_cmp(ch->pcdata->understanding, "All")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("All");
    }
    else if (strcasestr(argument, "Nullius Minuat sanctuarium") != NULL && str_cmp(ch->pcdata->understanding, "Limited")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("Limited");
      limunderglow(ch);
    }

    if (argument[0] != '\0') {
      // say redirect for ghosts - Discordance
      if (is_ghost(ch)) {
        do_function(ch, &do_whisper, argument);
        return;
      }
      argument = one_argument_nouncap(argument, arg1);
      if (!strcmp(arg1, "to") && !is_dreaming(ch)) {
        do_function(ch, &do_direct, argument);
        return;
      }
      else {
        strcat(arg1, " ");
        strcat(arg1, argument);
        argument = str_dup(arg1);
      }
    }

    int seeing = 0;
    if (in_fight(ch) && !is_undead(ch) && safe_strlen(argument) >= 10)
    ch->debuff += safe_strlen(argument) / 10;

    bool eataletter = FALSE;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '(') {
        eataletter = TRUE;
        rectotalk = TRUE;
      }
      else if (tmp[i] == ')') {
        rectotalk = FALSE;
      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else {
          if (eataletter == TRUE && tmp[i] == ' ') {
            eataletter = FALSE;
          }
          else {
            sprintf(blah, "%c", tmp[i]);
            strcat(talktalk, blah);
          }
        }
      }
    }
    argument = str_dup(talktalk);

    if (!IS_NPC(ch) && safe_strlen(rectalk) > 2) {
      sprintf(buf, "You say, %s, \"%s\"\n\r", rectalk, argument);
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
      if (is_name("loud", rectalk) || is_name("loudly", rectalk))
      volume = VOLUME_LOUD;
      if (is_name("yelling", rectalk) || is_name("yells", rectalk) || is_name("yell", rectalk))
      volume = VOLUME_YELLING;
      if (is_name("screaming", rectalk) || is_name("screams", rectalk) || is_name("scream", rectalk))
      volume = VOLUME_SCREAMING;
      if (is_name("ooc", rectalk) || is_name("oocly", rectalk))
      ch->rpexp -= 100;
    }
    else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2) {
      sprintf(buf, "You say, %s, \"%s\"\n\r", ch->pcdata->talk, argument);
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
      if (is_name("loud", ch->pcdata->talk) || is_name("loudly", ch->pcdata->talk))
      volume = VOLUME_LOUD;
    }
    else {
      sprintf(buf, "You say \"%s\"\n\r", argument);
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
    }
    /* discard null rooms and chars */
    if (!ch || ch->in_room == NULL)
    return;

    /* Make the words drunk if needed */
    if (!IS_NPC(ch) && ch->pcdata->conditions[COND_DRUNK] > 50)
    argument = makedrunk(argument, ch);

    if (is_dreaming(ch)) {
      if (ch->pcdata->dream_room > 0) {
        sprintf(buf, "%s says \"`o%s`x\"\n\r", dream_name(ch), argument);
        dreamscape_message(ch, ch->pcdata->dream_room, buf);
        dream_spy(ch, buf);
        if (IS_NPC(ch) || ch->pcdata->institute_action == 0)
        rpreward(ch, argument, FALSE, dream_pop(ch));
        CHAR_DATA *log;
        char rplog[MSL];
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          if (d->character != NULL && d->connected == CON_PLAYING) {
            log = d->character;
            if (can_rp_log(log, ch)) {
              sprintf(rplog, "%s says \"`o%s`x\"", dream_name(ch), argument);
              char_rplog(log, rplog);
            }
          }
        }
        return;
      }
      else if (ch->pcdata->dream_link != NULL) {
        sprintf(buf, "%s says \"`o%s`x\"\n\r", PERS(ch, ch->pcdata->dream_link), argument);
        page_to_char(wrap_string(buf, get_wordwrap(ch->pcdata->dream_link)), ch->pcdata->dream_link);
        dream_spy(ch, buf);
        if (IS_NPC(ch) || ch->pcdata->institute_action == 0)
        rpreward(ch, argument, FALSE, dream_pop(ch));
        return;
      }
    }
    if (is_abom(ch) && !is_masked(ch) && !is_cloaked(ch) && !in_fight(ch) && public_room(ch->in_room) && !room_hostile(ch->in_room) && abom_pop(ch->in_room) < room_pop(ch->in_room) && number_percent() % 4 == 0)
    ch->pcdata->lastnormal = current_time;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;
        if (is_dreaming(ch) && ch->pcdata->dream_link != to && ch != to && !IS_IMMORTAL(to))
        continue;
        if (is_deaf(to) && is_blind(to))
        continue;
        if (to->in_room != ch->in_room && !can_hear(to, ch, volume) && !can_see_char_distance(to, ch, DISTANCE_MEDIUM) && (!battleground(ch->in_room) || ch->faction != to->faction))
        continue;
        if (to->in_room != ch->in_room && is_gm(to))
        continue;
        if (is_griefer(ch) && !is_griefer(to))
        continue;
        if (!is_griefer(ch) && is_griefer(to))
        continue;
        if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
        continue;
        if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(to->act, PLR_DEEPSHROUD))
        continue;

        if (!same_player(ch, to) && to->pcdata->account != NULL && IS_FLAG(to->pcdata->account->flags, ACCOUNT_SHADOWBAN))
        continue;

        if (!same_player(ch, to) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
        continue;

        if (crowded_room(ch->in_room) && !same_place(ch, to))
        continue;

        if (ch->in_room != NULL && battleground(ch->in_room) && ch->faction != to->faction && combat_distance(ch, to, FALSE) > 50)
        continue;

        if ((to == ch))
        continue;

        if (!can_hear(to, ch, volume) && (!battleground(ch->in_room) || ch->faction != to->faction)) {
          if (ch->in_room != to->in_room) {
            printf_to_char(to, "[%s]%s says something.\n\r", room_distance(to, ch->in_room), PERS(ch, to));
            continue;
          }
          else {
            printf_to_char(to, "%s says something.\n\r", PERS(ch, to));
            continue;
          }
        }
        if (ch->in_room != NULL && battleground(ch->in_room) && combat_distance(ch, to, FALSE) > 50) {
          if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
          sprintf(buf, "Your comms say, %s, \"`o%s`x\"\n\r", rectalk, mangle_text(ch, to, ch->pcdata->speaking, argument));
          else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
          sprintf(buf, "Your comms say, %s, \"`o%s`x\"\n\r", ch->pcdata->talk, mangle_text(ch, to, ch->pcdata->speaking, argument));
          else
          sprintf(buf, "Your comms say \"`o%s`x\"\n\r", argument);
        }
        else if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
        sprintf(buf, "%s%s says, %s, \"`o%s`x\"\n\r", optional_distance(to, ch), speaker(ch, to), rectalk, mangle_text(ch, to, ch->pcdata->speaking, argument));
        else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
        sprintf(buf, "%s%s says, %s, \"`o%s`x\"\n\r", optional_distance(to, ch), speaker(ch, to), ch->pcdata->talk, mangle_text(ch, to, ch->pcdata->speaking, argument));
        else
        sprintf(buf, "%s%s says \"`o%s`x\"\n\r", optional_distance(to, ch), speaker(ch, to), mangle_text(ch, to, ch->pcdata->speaking, argument));
        page_to_char(wrap_string(buf, get_wordwrap(to)), to);
        triggercheck(to, mangle_text(ch, to, ch->pcdata->speaking, argument));
        persuade_check(ch, to, mangle_text(ch, to, ch->pcdata->speaking, argument));
        if (!is_gm(to) && ch != to)
        seeing++;
      }
    }

    if (!IS_NPC(ch) && ch->pcdata->connected_to != NULL && ch->pcdata->connection_stage == CONNECT_TALKING && !is_dreaming(ch)) {
      CHAR_DATA *victim = ch->pcdata->connected_to;
      if (is_deaf(victim))
      sprintf(buf, "Your phone says something.\n\r");
      else if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      sprintf(
      buf, "Your phone says, %s, \"`o%s`x\"\n\r", rectalk, static_text(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
      else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
      sprintf(
      buf, "Your phone says, %s, \"`o%s`x\"\n\r", ch->pcdata->talk, static_text(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
      else
      sprintf(
      buf, "Your phone says \"`o%s`x\"\n\r", static_text(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));

      char_rplog(victim, buf);
      page_to_char(wrap_string(buf, get_wordwrap(victim)), victim);
      give_attention(ch, victim);
      triggercheck(victim, mangle_text(ch, victim, ch->pcdata->speaking, argument));
      if (number_percent() % 4 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5)
      printf_to_char(
      get_tracer(ch), "You intercept \"%s\"\n\r", mangle_text(ch, get_tracer(ch), ch->pcdata->speaking, argument));

      if (get_phone(victim) != NULL && get_phone(victim)->value[4] > 0)
      hack_speech(ch, victim, rectalk, argument, get_phone(victim)->value[4]);
      if (get_phone(ch) != NULL && get_phone(ch)->value[4] > 0)
      hack_speech(ch, victim, rectalk, argument, get_phone(ch)->value[4]);
      seeing++;
    }

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
        if (to->in_room == ch->in_room)
        continue;

        if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
          if (is_dreaming(ch)) {
            if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
            sprintf(buf, "Dreaming:`x %s says, %s, \"`o%s`x\"", PERS(ch, to), rectalk, argument);
            else
            sprintf(buf, "Dreaming:`x %s says \"`o%s`x\"", PERS(ch, to), argument);
            page_to_char(wrap_string(buf, get_wordwrap(to)), to);
          }
          else {
            if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
            sprintf(buf, "%s:`x %s says, %s, \"`o%s`x\"", ch->in_room->name, PERS(ch, to), rectalk, argument);
            else
            sprintf(buf, "%s:`x %s says \"`o%s`x\"", ch->in_room->name, PERS(ch, to), argument);
            page_to_char(wrap_string(buf, get_wordwrap(to)), to);
          }
          continue;
        }
      }
    }
    char rplog[MSL];
    if (is_dreaming(ch)) {
      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      sprintf(rplog, "Dreaming:`x %s says, %s, \"`o%s`x\"", NAME(ch), rectalk, argument);
      else
      sprintf(rplog, "Dreaming:`x %s says \"`o%s`x\"", NAME(ch), argument);
    }
    else {
      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      sprintf(rplog, "%s:`x %s says, %s, \"`o%s`x\"", ch->in_room->name, NAME(ch), rectalk, argument);
      else
      sprintf(rplog, "%s:`x %s says \"`o%s`x\"", ch->in_room->name, NAME(ch), argument);
    }
    rp_log(rplog);

    if (is_dreaming(ch)) {
      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      sprintf(rplog, "%s says, %s, \"`o%s`x\"", emote_name(ch), rectalk, argument);
      else
      sprintf(rplog, "%s says \"`o%s`x\"", emote_name(ch), argument);
    }
    else {
      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      sprintf(rplog, "%s says, %s, \"`o%s`x\"", emote_name(ch), rectalk, argument);
      else
      sprintf(rplog, "%s says \"`o%s`x\"", emote_name(ch), argument);
    }
    if (battleground(ch->in_room)) {
      op_report(rplog, ch);
    }
    logevent_check(ch, rplog);
    CHAR_DATA *log;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        log = d->character;
        if (can_rp_log(log, ch)) {
          if (is_dreaming(ch)) {
            if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
            sprintf(rplog, "%s says, %s, \"`o%s`x\"", PERS(ch, log), rectalk, argument);
            else
            sprintf(rplog, "%s says \"`o%s`x\"", PERS(ch, log), argument);
          }
          else {
            if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
            sprintf(rplog, "%s says, %s, \"`o%s`x\"", PERS(ch, log), rectalk, argument);
            else
            sprintf(rplog, "%s says \"`o%s`x\"", PERS(ch, log), argument);
          }
          char_rplog(log, rplog);
        }
      }
    }

    if (seeing > 0)
    rpreward(ch, buf, FALSE, seeing + 1);
    if (ch->wounds >= 3 && !is_undead(ch)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 15);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MUTE;
      affect_to_char(ch, &af);
    }

    if (!is_ghost(ch) && !IS_FLAG(ch->act, PLR_SHROUD)) {
      char cam[MSL];
      sprintf(cam, "%s says %s in %s.", get_intro(ch), argument, ch->pcdata->speaking);
      spymessage(ch->in_room, cam);
    }
    return;
  }

  _DOFUN(do_think) {
    CHAR_DATA *to;
    char buf[MSL];
    char nbuf[MSL];

    if (safe_strlen(argument) < 2) {
      send_to_char("You think zen thoughts.\n\r", ch);
      return;
    }
    remove_color(buf, argument);
    if(ch->pcdata->curse == CURSE_REVELATION && ch->pcdata->curse_timeout > current_time)
    {
      sprintf(nbuf, "$n says, \"%s\"", buf);
      act(nbuf, ch, NULL, NULL, TO_ROOM);
    }
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

        if (ch->in_room == to->in_room && !is_gm(to) && !IS_FLAG(to->act, PLR_GUEST) && get_skill(to, SKILL_MINDREADING) >= 2 && !mindwarded(ch) && get_skill(ch, SKILL_CRYPTOMIND) < 1)
        printf_to_char(to, "%s thinks; \"%s\"\n\r", PERS(ch, to), buf);
        else if (ch->in_room == to->in_room && !is_gm(to) && !IS_FLAG(to->act, PLR_GUEST) && get_skill(to, SKILL_MINDREADING) >= 2 && !mindwarded(ch) && get_skill(ch, SKILL_CRYPTOMIND) > 0)
        printf_to_char(to, "%s thinks: \"%s\"\n\r", PERS(ch, to), to_gibberish("crypto", buf));
        else if (ch->in_room == to->in_room && !is_gm(to) && cardinal(to) && sinmatch(ch, to) && get_skill(ch, SKILL_CRYPTOMIND) < 1)
        printf_to_char(to, "%s thinks; \"%s\"\n\r", PERS(ch, to), buf);
        else if (ch->in_room == to->in_room && !is_gm(to) && cardinal(to) && sinmatch(ch, to) && get_skill(ch, SKILL_CRYPTOMIND) > 0)
        printf_to_char(to, "%s thinks: \"%s\"\n\r", PERS(ch, to), to_gibberish("crypto", buf));

        if (!is_gm(to) && !higher_power(to))
        continue;

        if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
          if (get_skill(ch, SKILL_CRYPTOMIND) > 0)
          printf_to_char(to, "%s thinks cryptographically; \"%s\"\n\r", PERS(ch, to), buf);
          else
          printf_to_char(to, "%s thinks; \"%s\"\n\r", PERS(ch, to), buf);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && get_gmtrust(to, ch) >= 3 && get_snooptrust(to, ch) > 0) {
          if (get_skill(ch, SKILL_CRYPTOMIND) > 0)
          printf_to_char(to, "%s thinks cryptographically; \"%s\"\n\r", PERS(ch, to), buf);
          else
          printf_to_char(to, "%s thinks; \"%s\"\n\r", PERS(ch, to), buf);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch) && get_snooptrust(to, ch) > 0) {
          if (get_skill(ch, SKILL_CRYPTOMIND) > 0)
          printf_to_char(to, "%s thinks cryptographically; \"%s\"\n\r", PERS(ch, to), buf);
          else
          printf_to_char(to, "%s thinks; \"%s\"\n\r", PERS(ch, to), buf);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "%s thinks: %s\n\r", NAME(ch), argument);
    rp_log(rplog);

    printf_to_char(ch, "You think; \"%s\"\n\r", argument);
    sprintf(buf, "You think; \"%s\"", argument);
    prp_rplog(ch, buf);
    if (mindbroken(ch)) {
      if (get_skill(ch, SKILL_CRYPTOMIND) > 0)
      sprintf(buf, "A line of text appears on the display, reading: \"&£*^&* *^&(£(^ ^&(((&Y* &**&\"");
      else
      sprintf(buf, "A line of text appears on the display, reading: \"%s\"", argument);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
  }

  _DOFUN(do_recall) {
    CHAR_DATA *to;
    char buf[MSL];
    remove_color(buf, argument);

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

        if (!is_gm(to) && !higher_power(to))
        continue;

        if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
          printf_to_char(to, "%s recalls %s\n\r", PERS(ch, to), buf);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && get_gmtrust(to, ch) >= 3 && get_snooptrust(to, ch) > 0) {
          printf_to_char(to, "%s: %s recalls %s\n\r", ch->in_room->name, PERS(ch, to), buf);

        }
        else if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch) && get_snooptrust(to, ch) > 0) {
          printf_to_char(to, "%s: %s recalls %s\n\r", ch->in_room->name, PERS(ch, to), buf);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "%s recalls %s\n\r", NAME(ch), argument);
    rp_log(rplog);

    printf_to_char(ch, "You recall %s\n\r", argument);
    sprintf(buf, "You recall %s", argument);
    prp_rplog(ch, buf);
    if (mindbroken(ch)) {
      sprintf(buf, "A short film plays on the display: %s", argument);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
  }

  bool can_send_feels(CHAR_DATA *ch) {
    if (is_helpless(ch))
    return FALSE;
    if (is_ghost(ch))
    return FALSE;
    if (silenced(ch))
    return FALSE;
    if (in_fight(ch))
    return FALSE;
    if (room_hostile(ch->in_room))
    return FALSE;

    return TRUE;
  }

  _DOFUN(do_feel) {
    char buf[MSL];
    remove_color(buf, argument);

    if (strcasestr(argument, "-")) {
      send_to_char("Feel is for raw experiences only.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOFEEL)) {
      send_to_char("Your ability to use feels has been suspended.\n\r", ch);
      return;
    }

    CHAR_DATA *to;
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
        if (silenced(to))
        continue;

        if (is_neutralized(ch) || is_neutralized(to))
        continue;

        if (same_pack(ch, to) && (can_send_feels(ch) || ch->in_room == to->in_room) && safe_strlen(buf) < 20 && get_world(ch) == get_world(to))
        printf_to_char(to, "%s feels %s\n\r", PERS(ch, to), buf);

        if (cardinal(to) && sinmatch(to, ch))
        printf_to_char(to, "%s feels %s\n\r", PERS(ch, to), buf);

        if (!is_gm(to) && !higher_power(to))
        continue;

        if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
          printf_to_char(to, "%s feels %s\n\r", PERS(ch, to), buf);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && get_gmtrust(to, ch) >= 3 && get_snooptrust(to, ch) > 0) {
          printf_to_char(to, "%s: %s feels %s\n\r", ch->in_room->name, PERS(ch, to), buf);
        }

        else if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch) && get_snooptrust(to, ch) > 0) {
          printf_to_char(to, "%s: %s feels %s\n\r", ch->in_room->name, PERS(ch, to), buf);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "%s feels %s\n\r", NAME(ch), argument);
    rp_log(rplog);

    printf_to_char(ch, "You feel %s\n\r", argument);
    sprintf(buf, "You feel %s", argument);
    prp_rplog(ch, buf);

    if (mindbroken(ch)) {
      sprintf(buf, "A short bolded phrase appears on the display '%s'", capitalize(argument));
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    if (is_name("afraid", argument) || is_name("fear", argument) || is_name("scared", argument))
    psychic_feast(ch, PSYCHIC_FEAR, 5);
    else if (is_name("anger", argument) || is_name("rage", argument) || is_name("angry", argument))
    psychic_feast(ch, PSYCHIC_ANGER, 10);
    else if (is_name("lust", argument) || is_name("desire", argument) || is_name("horny", argument) || is_name("aroused", argument))
    psychic_feast(ch, PSYCHIC_LUST, 5);
    else
    psychic_feast(ch, PSYCHIC_AMBIANT, 4);
  }

  _DOFUN(do_direct) {
    CHAR_DATA *victim;
    char arg1[MIL];
    CHAR_DATA *to;
    char buf[MSL];
    bool hasphone = FALSE;
    smash_percent(argument);
    argument = one_argument(argument, arg1);
    int volume = VOLUME_NORMAL;
    char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
    int seeing = 0;
    if (ch->in_room != NULL && battleground(ch->in_room))
    return;

    if (crowded_room(ch->in_room) && safe_strlen(ch->pcdata->place) < 2) {
      send_to_char("Find a place first.\n\r", ch);
      return;
    }

    if (arg1[0] == '\0') {
      send_to_char("Tell who what?\n\r", ch);
      return;
    }
    victim = get_char_room(ch, NULL, arg1);

    if (!str_cmp(arg1, "phone"))
    hasphone = TRUE;
    else if (victim == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim != NULL && !same_place(ch, victim) && crowded_room(ch->in_room)) {
      send_to_char("You're too far away.\n\r", ch);
      return;
    }
    
    if (in_fight(ch) && !is_undead(ch) && safe_strlen(argument) >= 10)
    ch->debuff += safe_strlen(argument) / 10;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    bool eataletter = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '(') {
        rectotalk = TRUE;
      }
      else if (tmp[i] == ')') {
        rectotalk = FALSE;
        eataletter = TRUE;
      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else if (eataletter == TRUE)
        eataletter = FALSE;
        else {
          sprintf(blah, "%c", tmp[i]);
          strcat(talktalk, blah);
        }
      }
    }
    argument = str_dup(talktalk);

    if (!hasphone) {
      move_closer(ch, victim, 2);
      give_attention(ch, victim);
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        if (to == ch)
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;
        if (to->in_room == ch->in_room)
        continue;

        if (!same_player(ch, to) && to->pcdata->account != NULL && IS_FLAG(to->pcdata->account->flags, ACCOUNT_SHADOWBAN))
        continue;

        if (!same_player(ch, to) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
        continue;

        if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
          if (hasphone || victim == NULL)
          sprintf(buf, "%s: %s says to phone %s", ch->in_room->name, PERS(ch, to), argument);
          else if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
          sprintf(buf, "%s: %s says to %s, %s, %s", ch->in_room->name, PERS(ch, to), PERS_2(victim, to), rectalk, argument);
          else
          sprintf(buf, "%s: %s says to %s %s", ch->in_room->name, PERS(ch, to), PERS_2(victim, to), argument);
          if (can_rp_log(to, ch))
          char_rplog(to, buf);
          page_to_char(wrap_string(buf, get_wordwrap(to)), to);

          continue;
        }
      }
    }
    char rplog[MSL];
    if (hasphone || victim == NULL)
    sprintf(rplog, "%s: %s says to phone %s", ch->in_room->name, ch->name, argument);
    else if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
    sprintf(rplog, "%s: %s says to %s, %s, %s", ch->in_room->name, ch->name, victim->name, rectalk, argument);
    else
    sprintf(rplog, "%s: %s says to %s %s", ch->in_room->name, ch->name, victim->name, argument);
    rp_log(rplog);

    if (!is_ghost(ch) && !IS_FLAG(ch->act, PLR_SHROUD)) {
      char cam[MSL];
      sprintf(cam, "%s says %s.", get_intro(ch), argument);
      spymessage(ch->in_room, cam);
    }

    if (safe_strlen(rectalk) > 2) {
      if (is_name("loud", rectalk) || is_name("loudly", rectalk))
      volume = VOLUME_LOUD;
      if (is_name("yelling", rectalk) || is_name("yells", rectalk) || is_name("yell", rectalk))
      volume = VOLUME_YELLING;
      if (is_name("screaming", rectalk) || is_name("screams", rectalk) || is_name("scream", rectalk))
      volume = VOLUME_SCREAMING;
      if (is_name("ooc", rectalk) || is_name("oocly", rectalk))
      ch->rpexp -= 100;

    }
    else {
      if (is_name("loud", ch->pcdata->talk) || is_name("loudly", ch->pcdata->talk))
      volume = VOLUME_LOUD;
    }

    /* Make the words drunk if needed */
    if (!IS_NPC(ch) && ch->pcdata->conditions[COND_DRUNK] > 50)
    argument = makedrunk(argument, ch);

    if (hasphone) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;
          if (to->in_room == NULL || ch->in_room == NULL)
          continue;

          if (is_dreaming(ch) && ch->pcdata->dream_link != to && ch != to && !IS_IMMORTAL(to))
          continue;

          if (to->in_room != ch->in_room && !can_hear(to, ch, volume) && !can_see_char_distance(to, ch, DISTANCE_MEDIUM))
          continue;

          if ((to == ch))
          continue;

          if (is_deaf(to) && is_blind(to))
          continue;

          if (ch->in_room != NULL && battleground(ch->in_room) && ch->faction != to->faction)
          continue;

          if (crowded_room(ch->in_room) && !same_place(ch, to))
          continue;

          if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
          continue;

          if (is_gm(to) && to->in_room != ch->in_room)
          continue;

          if (!can_hear(to, ch, volume)) {
            if (ch->in_room != to->in_room) {
              printf_to_char(to, "[%s]%s says something.\n\r", room_distance(to, ch->in_room), PERS(ch, to));
              continue;
            }
            else {
              printf_to_char(to, "%s says something.\n\r", PERS(ch, to));
              continue;
            }
          }

          if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
          sprintf(buf, "%s says, %s, (`ophone`x) '`o%s`x'\n\r", speaker(ch, to), rectalk, mangle_text(ch, to, ch->pcdata->speaking, argument));
          else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
          sprintf(buf, "%s says, %s, (`ophone`x) '`o%s`x'\n\r", speaker(ch, to), ch->pcdata->talk, mangle_text(ch, to, ch->pcdata->speaking, argument));
          else
          sprintf(buf, "%s says (`ophone`x) '`o%s`x'\n\r", speaker(ch, to), mangle_text(ch, to, ch->pcdata->speaking, argument));
          triggercheck(to, buf);
          if (can_rp_log(to, ch))
          char_rplog(to, buf);

          if (!is_gm(to) && to != ch)
          seeing++;

          page_to_char(wrap_string(buf, get_wordwrap(to)), to);
        }
      }
      if (!IS_NPC(ch) && ch->pcdata->connected_to != NULL && ch->pcdata->connection_stage == CONNECT_TALKING) {
        CHAR_DATA *victim = ch->pcdata->connected_to;
        if (is_deaf(victim))
        sprintf(buf, "Your phone says something'\n\r");
        else if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
        sprintf(buf, "Your phone says, %s, '`o%s`x'\n\r", rectalk, mangle_text(ch, victim, ch->pcdata->speaking, argument));
        else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
        sprintf(buf, "Your phone says, %s, '`o%s`x'\n\r", ch->pcdata->talk, mangle_text(ch, victim, ch->pcdata->speaking, argument));
        else
        sprintf(buf, "Your phone says '`o%s`x'\n\r", mangle_text(ch, victim, ch->pcdata->speaking, argument));

        char_rplog(victim, buf);
        page_to_char(wrap_string(buf, get_wordwrap(victim)), victim);

        triggercheck(victim, mangle_text(ch, victim, ch->pcdata->speaking, argument));

        if (number_percent() % 4 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5)
        printf_to_char(
        get_tracer(ch), "You intercept '%s'\n\r", mangle_text(ch, get_tracer(ch), ch->pcdata->speaking, argument));

        if (get_phone(victim) != NULL && get_phone(victim)->value[4] > 0)
        hack_speech(ch, victim, rectalk, argument, get_phone(victim)->value[4]);
        if (get_phone(ch) != NULL && get_phone(ch)->value[4] > 0)
        hack_speech(ch, victim, rectalk, argument, get_phone(ch)->value[4]);

        seeing++;
      }
      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2) {
        sprintf(buf, "You say, %s, (`ophone`x) '%s'\n\r", rectalk, argument);
        char_rplog(ch, buf);
        page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

      }
      else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2) {
        sprintf(buf, "You say, %s, (`ophone`x) '%s'\n\r", ch->pcdata->talk, argument);
        char_rplog(ch, buf);
        page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

      }
      else {
        sprintf(buf, "You say (`ophone`x)'%s'\n\r", argument);
        char_rplog(ch, buf);
        page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
      }

      dact("$n says something into $s phone.", ch, NULL, NULL, DISTANCE_NEAR);
    }
    else {
      for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;
        
          if (to->in_room == NULL || ch->in_room == NULL)
          continue;

          if (is_dreaming(ch) && ch->pcdata->dream_link != to && ch != to && !IS_IMMORTAL(to))
          continue;

          if (to->in_room != ch->in_room && !can_hear(to, ch, volume) && !can_see_char_distance(to, ch, DISTANCE_MEDIUM))
          continue;

          if ((to == ch))
          continue;
        
          if ((to == victim))
          continue;

          if (is_deaf(to) && is_blind(to))
          continue;

          if (ch->in_room != NULL && battleground(ch->in_room) && ch->faction != to->faction)
          continue;

          if (crowded_room(ch->in_room) && !same_place(ch, to))
          continue;
        
          if (to->in_room != ch->in_room && is_gm(to))
          continue;

          if (!can_hear(to, ch, volume)) {
            if (ch->in_room != to->in_room) {
              printf_to_char(to, "[%s]%s says something.\n\r", room_distance(to, ch->in_room), PERS(ch, to));
              continue;
            }
            else {
              printf_to_char(to, "%s says something.\n\r", PERS(ch, to));
              continue;
            }
          }

          if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
          continue;

          if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
          sprintf(buf, "%s%s says (`oto %s`x), %s, '`o%s`x'\n\r", optional_distance(to, ch), speaker(ch, to), speaker_2(victim, to), rectalk, mangle_text(ch, to, ch->pcdata->speaking, argument));
          else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
          sprintf(buf, "%s%s says (`oto %s`x), %s, '`o%s`x'\n\r", optional_distance(to, ch), speaker(ch, to), speaker_2(victim, to), ch->pcdata->talk, mangle_text(ch, to, ch->pcdata->speaking, argument));
          else
          sprintf(buf, "%s%s says (`oto %s`x) '`o%s`x'\n\r", optional_distance(to, ch), speaker(ch, to), speaker_2(victim, to), mangle_text(ch, to, ch->pcdata->speaking, argument));
          triggercheck(to, argument);
          if (can_rp_log(to, ch))
          char_rplog(to, buf);
          page_to_char(wrap_string(buf, get_wordwrap(to)), to);

          if (!is_gm(to) && to != ch)
          seeing++;
        }
      }
      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2) {
        sprintf(buf, "You say (`oto %s`x), %s, '`o%s`x'\n\r", PERS(victim, ch), rectalk, argument);
        char_rplog(ch, buf);
        page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

        if (!IS_NPC(victim)) {
          if ((!can_shroud(ch) && IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD)) || is_deaf(victim))
          sprintf(buf, "%s says something to you.", PERS(ch, victim));
          else
          sprintf(buf, "%s says (`oto you`x), %s, '`o%s`x'\n\r", PERS(ch, victim), rectalk, mangle_text(ch, victim, ch->pcdata->speaking, argument));
          char_rplog(victim, buf);
          page_to_char(wrap_string(buf, get_wordwrap(victim)), victim);
        }

      }
      else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2) {
        sprintf(buf, "You say (`oto %s`x), %s, '`o%s`x'\n\r", PERS(victim, ch), ch->pcdata->talk, argument);
        char_rplog(ch, buf);
        page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
        if (!IS_NPC(victim)) {
          if ((!can_shroud(ch) && IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD)) || is_deaf(victim))
          sprintf(buf, "%s says something to you.", PERS(ch, victim));
          else
          sprintf(buf, "%s says (`oto you`x), %s, '`o%s`x'\n\r", PERS(ch, victim), ch->pcdata->talk, mangle_text(ch, victim, ch->pcdata->speaking, argument));
          char_rplog(victim, buf);
          page_to_char(wrap_string(buf, get_wordwrap(victim)), victim);
        }
      }
      else {
        sprintf(buf, "You say (`oto %s`x) '`o%s`x'\n\r", PERS(victim, ch), argument);
        char_rplog(ch, buf);
        page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

        if (!IS_NPC(victim)) {
          if ((!can_shroud(ch) && IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD)) || is_deaf(victim))
          sprintf(buf, "%s says something to you.", PERS(ch, victim));
          else
          sprintf(buf, "%s says (`oto you`x) '`o%s`x'\n\r", PERS(ch, victim), mangle_text(ch, victim, ch->pcdata->speaking, argument));
          char_rplog(victim, buf);
          page_to_char(wrap_string(buf, get_wordwrap(victim)), victim);
        }
      }
      triggercheck(victim, mangle_text(ch, victim, ch->pcdata->speaking, argument));
      persuade_check(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument));
      seeing++;
    }

    if (seeing > 0)
    rpreward(ch, argument, FALSE, seeing);

    if (ch->wounds >= 3 && !is_undead(ch)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 15);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MUTE;
      affect_to_char(ch, &af);
    }

    return;
  }

  bool power_whisper(CHAR_DATA *ch, CHAR_DATA *spir)
  {
    if(ch == NULL || spir == NULL || IS_NPC(ch) || IS_NPC(spir))
    return FALSE;

    if(higher_power(ch) && !higher_power(spir))
    return power_whisper(spir, ch);

    if(IS_IMMORTAL(ch))
    return FALSE;

    if(higher_power(ch) && higher_power(spir))
    return FALSE;

    if(!higher_power(spir))
    return FALSE;

    if(spir->possessing  == ch)
    return TRUE;

    if(get_snooptrust(spir, ch) < 0)
    return FALSE;

    if(get_snooptrust(spir, ch) > 0)
    {
      if(light_level(ch->in_room) < 75)
      return TRUE;
    }
    if(light_level(ch->in_room) < 0)
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_whisper) {
    CHAR_DATA *victim;
    CHAR_DATA *to;
    char arg[MIL], outbuf[MSL], buf[MIL];
    int cnt = 0;
    bool directed;
    char arg1[MSL];
    smash_percent(argument);
    
    if (!IS_NPC(ch))
    ch->pcdata->time_since_emote = 0;

    char *message;
    message = str_dup(argument);
    if (is_mute(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_GAG)) {
      send_to_char("You're gagged.\n\r", ch);
      return;
    }

    if (ch->hit < -7) {
      send_to_char("You're hurt too bad to talk.\n\r", ch);
      return;
    }

    if (crowded_room(ch->in_room) && safe_strlen(ch->pcdata->place) < 2) {
      send_to_char("Find a place first.\n\r", ch);
      return;
    }

    if (argument[0] != '\0') {
      argument = one_argument_nouncap(argument, arg1);
      if (!strcmp(arg1, "to")) {
        directed = TRUE;
      }
      else {
        directed = FALSE;
        strcat(arg1, " ");
        strcat(arg1, argument);
        argument = str_dup(arg1);
      }
    }
    else {
      send_to_char("Syntax: whisper (message)/whisper to (person) (message)\n\r", ch);
      return;
    }
    
    if (in_fight(ch) && !is_undead(ch) && safe_strlen(argument) >= 10)
    ch->debuff += safe_strlen(argument) / 10;

    // ghost checks for whisper - Discordance
    if (is_ghost(ch)) {
      if (is_manifesting(ch)) {
        if (deplete_ghostpool(ch, GHOST_SPEECH) == FALSE) {
          send_to_char("You can't muster the will to manifest your voice any more today.\n\r", ch);
          return;
        }
      }
    }

    if (strcasestr(argument, "Revocacionem protectione sanctuarium") != NULL && str_cmp(ch->pcdata->understanding, "None")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("None");
      nounderglow(ch);
    }
    else if (strcasestr(argument, "Repeto praesidio sanctuarium") != NULL && str_cmp(ch->pcdata->understanding, "All")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("All");
    }
    else if (strcasestr(argument, "Nullius Minuat sanctuarium") != NULL && str_cmp(ch->pcdata->understanding, "Limited")) {
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("Limited");
      limunderglow(ch);
    }

    int seeingpop = 0;

    for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
      
        if (to == ch)
        continue;
      
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;

        if (is_griefer(ch) && !is_griefer(to) && !IS_IMMORTAL(to))
        continue;
      
        if (is_griefer(to) && !is_griefer(ch))
        continue;

        if (is_ghost(ch) && !is_manifesting(ch) && !can_hear_ghost(ch, to))
        continue;

        if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch) && ch->in_room != to->in_room) {
          sprintf(buf, "%s:%s whispers %s", ch->in_room->name, PERS(ch, to), argument);
          page_to_char(wrap_string(buf, get_wordwrap(to)), to);
          continue;
        }
        else if (is_gm(to) && ch->in_room == to->in_room && (!same_place(ch, to) || directed == TRUE)) {
          sprintf(buf, "%s whispers %s", PERS(ch, to), argument);
          page_to_char(wrap_string(buf, get_wordwrap(to)), to);
          continue;
        }
      }
    }
    
    char rplog[MSL];
    sprintf(rplog, "%s whispers %s\n\r", NAME(ch), argument);
    rp_log(rplog);

    if (argument[0] == '\0') {
      send_to_char("Whisper what to whom?\n\r", ch);
      return;
    }

    if (directed == FALSE) {
      if (IS_FLAG(ch->act, PLR_HIDE))
      do_function(ch, &do_unhide, "");

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          
          if (IS_NPC(to))
          continue;
        
          if (to == ch)
          continue;
        
          if (to->in_room == NULL || ch->in_room == NULL)
          continue;

          if(to->in_room != ch->in_room && power_whisper(ch, to) && !higher_power(ch))
          {
            printf_to_char(to, "%s %s says in a low voice, '`o%s`x'", roomtitle(ch->in_room, FALSE), PERS(ch, to), mangle_text(ch, to, ch->pcdata->speaking, argument));
            continue;
          }

          if (to->in_room != ch->in_room)
          continue;

          if (is_ghost(ch) && !is_manifesting(ch) && !can_hear_ghost(ch, to))
          continue;

          if (is_griefer(ch) && !is_griefer(to) && !IS_IMMORTAL(to))
          continue;
        
          if (is_griefer(to) && !is_griefer(ch))
          continue;

          if (is_deaf(ch) && is_blind(ch))
          continue;

          if (!same_place(ch, to))
          continue;

          if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
          continue;

          if ((!can_shroud(ch) && IS_FLAG(to->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD)) || is_deaf(to))
          act("$n says something in a low voice", ch, argument, to, TO_VICT);
          else
          act("$n says in a low voice, '`o$t`x'", ch, mangle_text(ch, to, ch->pcdata->speaking, argument), to, TO_VICT);
          sprintf(rplog, "%s says in a low voice, '`o%s`x'", PERS(ch, to), mangle_text(ch, to, ch->pcdata->speaking, argument));
          char_rplog(to, rplog);

          triggercheck(to, mangle_text(ch, to, ch->pcdata->speaking, message));
          persuade_check(ch, to, mangle_text(ch, to, ch->pcdata->speaking, message));
          if (!is_gm(to) || to->in_room == ch->in_room)
          seeingpop++;
        }
      }

      if (seeingpop > 1)
      rpreward(ch, argument, FALSE, seeingpop);

      act("You say in a low voice, '`o$t`x'", ch, argument, ch, TO_CHAR);
      sprintf(rplog, "You say in a low voice, '`o%s`x'", argument);
      char_rplog(to, rplog);

      return;
    }

    argument = one_argument_nouncap(argument, arg);
    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      if(higher_power(ch))
      {
        victim = get_char_world_pc(arg);
        if(victim == NULL)
        {

          send_to_char("Syntax: whisper (message)/whisper to (person) (message)\n\r", ch);
          return;
        }
        if(!power_whisper(victim, ch))
        {

          send_to_char("Syntax: whisper (message)/whisper to (person) (message)\n\r", ch);
          return;
        }
        act("$n whispers to you, '`o$t`x'", ch, mangle_text(ch, victim, ch->pcdata->speaking, argument), victim, TO_VICT);
        sprintf(rplog, "%s whispers to you, '`o%s`x'", PERS(ch, victim), mangle_text(ch, victim, ch->pcdata->speaking, argument));
        char_rplog(victim, rplog);
        act("You whisper to $N, '`o$t`x'", ch, argument, victim, TO_CHAR);
        return;
      }

      send_to_char("Syntax: whisper (message)/whisper to (person) (message)\n\r", ch);
      return;
    }

    if (victim != NULL && !same_place(ch, victim) && !is_ghost(ch)) {
      send_to_char("You're too far away.\n\r", ch);
      return;
    }
    if(victim != NULL && !same_place(ch, victim) && higher_power(ch) && get_snooptrust(ch, victim) < 3) {
      send_to_char("You're too far away.\n\r", ch);
      return;
    }

    etag(ch);
    sprintf(buf, "%s whispers '%s'", NAME(ch), argument);
    /* Make the words drunk if needed */
    if (!IS_NPC(ch) && ch->pcdata->conditions[COND_DRUNK] > 50)
    argument = makedrunk(argument, ch);

    strcpy(buf, argument);

    outbuf[0] = '\0';
    while (argument[0] != '\0') {
      argument = one_argument(argument, arg);
      if (number_percent() < 20 && argument[0] != '\0') {
        argument = one_argument(argument, arg);
      }

      if (number_percent() > 85)
      sprintf(outbuf, "%s%s%s", outbuf, (cnt == 0) ? "" : " ", arg);
      else
      sprintf(outbuf, "%s%s", outbuf, (cnt == 0) ? "" : " ....");

      cnt++;
    }
    move_closer(ch, victim, 0);
    give_attention(ch, victim);
    if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD)) {
    }
    else if ((is_griefer(ch) && !is_griefer(to)) || (!is_griefer(ch) && is_griefer(to))) {
    }
    else if (is_ghost(ch) && !can_hear_ghost(ch, to)) {
    }
    else if ((!can_shroud(ch) && IS_FLAG(to->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD)) || is_deaf(to)) {
      act("$n whispers something to $N.", ch, outbuf, victim, TO_NOTVICT);
      act("$n whispers something to you", ch, buf, victim, TO_VICT);
    }
    else {
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        if (!IS_NPC((*it)) && !is_deaf(*it) && (*it) != ch && (*it) != victim) {
          printf_to_char((*it), "%s whispers to %s, '`o%s`x'", PERS(ch, (*it)), PERS_2(victim, (*it)), mangle_text(ch, (*it), ch->pcdata->speaking, outbuf));
        }
      }
      act("$n whispers to you, '`o$t`x'", ch, mangle_text(ch, victim, ch->pcdata->speaking, buf), victim, TO_VICT);
    }
    act("You whisper to $N, '`o$t`x'", ch, buf, victim, TO_CHAR);

    triggercheck(victim, mangle_text(ch, victim, ch->pcdata->speaking, message));
    persuade_check(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, message));
    if (IS_NPC(ch) || ch->pcdata->institute_action == 0)
    rpreward(ch, buf, TRUE, 1);
    if (ch->wounds >= 3 && !is_undead(ch)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 10);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MUTE;
      affect_to_char(ch, &af);
    }

    return;
  }

  _DOFUN(do_consent) {
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);

    if (IS_FLAG(ch->comm, COMM_CONSENT)) {
      REMOVE_FLAG(ch->comm, COMM_CONSENT);
      send_to_char("You retract your OOC rules consent.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "ooc") && !str_cmp(argument, "rules")) {
      SET_FLAG(ch->comm, COMM_CONSENT);
      send_to_char("You consent to the OOC rules.\n\r", ch);
      return;
    }
  }

  void dream_tell(CHAR_DATA *ch, char *argument, char arg[MAX_INPUT_LENGTH])
  {
    int i, n;
    bool ignored = FALSE;
    CHAR_DATA *victim;

    if (is_gm(ch)) {
      send_to_char("This is a player command.\n\r", ch);
      return;
    }
    
    if (!IS_FLAG(ch->comm, COMM_CONSENT)) {
      send_to_char("See Help OOC Rules\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL) || IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }
    
    if (in_fight(ch)) {
      send_to_char("Not right now.\n\r", ch);
      return;
    }
    
    if (is_name("NewCharacter", ch->name)) {
      send_to_char("Unnamed characters can't use channels.\n\r", ch);
      return;
    }

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }
    
    int type = REPLY_DREAM;
    victim = NULL;
    for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it) {
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

      if (!str_cmp(dream_name(vch), arg))
      victim = vch;
    }
    if (victim == NULL) {
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

        if (is_name(arg, dream_name(vch)))
        victim = vch;
      }
    }

    if (victim == NULL) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }
    
    if (IS_IMMORTAL(victim) && victim->invis_level > ch->level) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }
    
    if (!IS_FLAG(victim->comm, COMM_CONSENT) && victim->played / 3600 > 50) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    char targetname[MSL];
    char sendername[MSL];
    sprintf(targetname, "%s", dream_name(victim));
    sprintf(sendername, "%s", dream_name(ch));

    if (victim->desc == NULL && !IS_NPC(victim)) {
      printf_to_char(ch, "%s seems to have misplaced their link... Try again later.", targetname);
      return;
    }

    // checking ignored list
    for (i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->ignored_accounts[i])) {
        ignored = TRUE;
        i = 50;
      }
      if (ignored == FALSE) {
        for (n = 0; n < 25; n++) {
          if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(ch->pcdata->account->characters[n], victim->pcdata->ignored_characters[i])) {
              ignored = TRUE;
              i = 50;
              n = 25;
            }
          }
        }
      }
    }
    
    if (ignored == TRUE) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTELL)) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);

    printf_to_char(ch, "You tell %s OOCly '`i%s`x'\n\r", targetname, argument);
    if (same_player(ch, victim) || IS_IMMORTAL(victim) || (!spammer(ch) && mod_flagged == FALSE)) {
      printf_to_char(victim, "%s tells you OOCly '`i%s`x'\n\r", sendername, argument);
      victim->reply = ch;
      victim->reply_type = type;
    }
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }
    return;
  }

  _DOFUN(do_ptell) {
    int i, n;
    bool ignored = FALSE;
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char arg2[MSL];

    if (is_gm(ch)) {
      send_to_char("This is a player command.\n\r", ch);
      return;
    }
    
    if (!IS_FLAG(ch->comm, COMM_CONSENT)) {
      send_to_char("See Help OOC Rules\n\r", ch);
      return;
    }
    
    if ((ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL)) || IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }
    
    if (in_fight(ch)) {
      send_to_char("Not right now.\n\r", ch);
      return;
    }
    
    if (is_name("NewCharacter", ch->name)) {
      send_to_char("Unnamed characters can't use channels.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }
    int type = 0;

    if (!str_cmp(arg, "Account")) {
      type = REPLY_ACCOUNT;
      argument = one_argument(argument, arg2);
      if ((victim = get_char_world_account(arg2)) == NULL) {
        send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg, "player")) {
      type = REPLY_PLAYER;
      argument = one_argument(argument, arg2);
      if ((victim = get_char_world_pc(arg2)) == NULL) {
        send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg, "character")) {
      type = REPLY_CHARACTER;
      argument = one_argument(argument, arg2);
      if ((victim = get_char_world(ch, arg2)) == NULL) {
        send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
        return;
      }
    }
    else {
      if (is_dreaming(ch)) {
        smash_vector(argument);
        dream_tell(ch, argument, arg);
        return;
      }

      victim = get_char_world_pc(arg);
      if (victim != NULL)
      type = REPLY_PLAYER;
    
      if (victim == NULL) {
        victim = get_char_world_account(arg);
        if (victim != NULL)
        type = REPLY_ACCOUNT;
      }
      
      if (victim == NULL) {
        victim = get_char_world(ch, arg);
        if (victim != NULL)
        type = REPLY_CHARACTER;
      }

      if (victim == NULL) {
        send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
        return;
      }
    }
    if (IS_IMMORTAL(victim) && victim->invis_level > ch->level) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    // added a check for NPCs because it didn't seem necessary and was suspicious
    // - Discordance
    if (IS_NPC(victim)) {
      send_to_char("NPCs can't hear you.\n\r", ch);
      return;
    }

    if (!IS_FLAG(victim->comm, COMM_CONSENT) && victim->played / 3600 > 50) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }
    if (ch->in_room != NULL && victim->in_room != NULL) {
      if (victim->in_room == ch->in_room || (in_world(ch) == in_world(victim) && get_dist(victim->in_room->x, victim->in_room->y, ch->in_room->x, ch->in_room->y) < 5)) {
        if (!are_friends(ch, victim) && type != REPLY_ACCOUNT) {
          //                 send_to_char("You can't communicate OOCly with people
          //                 you're currently RPing with.\n\r", ch); return;
        }
      }
    }

    char targetname[MSL];
    char sendername[MSL];
    if (type == REPLY_ACCOUNT) {
      sprintf(targetname, "%s", victim->pcdata->account->name);
      sprintf(sendername, "%s", ch->pcdata->account->name);
    }
    else if (type == REPLY_PLAYER) {
      sprintf(targetname, "%s", victim->name);
      sprintf(sendername, "%s", ch->name);
    }
    else if (type == REPLY_CHARACTER) {
      sprintf(targetname, "%s", PERS(victim, ch));
      sprintf(sendername, "%s", PERS(ch, victim));
    }
    else {
      sprintf(targetname, "%s", "They");
      sprintf(sendername, "%s", "Someone");
    }
    if (victim->desc == NULL && !IS_NPC(victim)) {
      printf_to_char(ch, "%s seems to have misplaced their link... Try again later.", targetname);
      return;
    }

    // checking ignored list
    for (i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->ignored_accounts[i])) {
        ignored = TRUE;
        i = 50;
      }
      if (ignored == FALSE) {
        for (n = 0; n < 25; n++) {
          if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(ch->pcdata->account->characters[n], victim->pcdata->ignored_characters[i])) {
              ignored = TRUE;
              i = 50;
              n = 25;
            }
          }
        }
      }
    }
    if (ignored == TRUE) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTELL)) {
      send_to_char("They aren't here or haven't consented to the OOC rules.\n\r", ch);
      return;
    }

    // act_new( "You tell $N '`i$t`x'", ch, argument, victim, TO_CHAR, POS_DEAD );
    // // Disabled to maintain IC/OOC boundary and be consistent - Discordance
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);
    printf_to_char(ch, "You tell %s OOCly '`i%s`x'\n\r", targetname, argument);
    if (same_player(ch, victim) || IS_IMMORTAL(victim) || (!spammer(ch) && mod_flagged == FALSE)) {
      printf_to_char(victim, "%s tells you OOCly '`i%s`x'\n\r", sendername, argument);
      victim->reply = ch;
      victim->reply_type = type;
    }
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }

    return;
  }

  // This sets up a selective filter for tells - Discordance
  _DOFUN(do_ignore) {
    struct stat sb;
    char arg[MSL], buf[MSL];
    int counter, i;
    // int i;
    bool online = FALSE, stored = FALSE, errored = FALSE, account = FALSE;
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;

    argument = one_argument(argument, arg);

    for (i = 0; i < 50; i++) {
      if (ch->pcdata->ignored_characters[i] == NULL) {
        ch->pcdata->ignored_characters[i] = str_dup("");
      }
    }

    for (i = 0; i < 50; i++) {
      if (ch->pcdata->ignored_accounts[i] == NULL) {
        ch->pcdata->ignored_accounts[i] = str_dup("");
      }
    }

    if (!str_cmp(arg, "list")) {
      counter = 1;
      send_to_char("`cIgnored List`x\n\r", ch);
      send_to_char("`g--------------------------------------------------------------------------------`x\n\r", ch);
      send_to_char("`cCharacters`g:`x\n\r", ch);
      for (i = 0; i < 50; i++) {
        if (str_cmp("", ch->pcdata->ignored_characters[i])) {
          printf_to_char(ch, "`g[`W%d`g]`x %s\n\r", counter, ch->pcdata->ignored_characters[i]);
          counter++;
        }
      }
      send_to_char("\n\r", ch);
      send_to_char("`cAccounts`g:`x\n\r", ch);
      for (i = 0; i < 50; i++) {
        if (str_cmp("", ch->pcdata->ignored_accounts[i])) {
          printf_to_char(ch, "`g[`W%d`g]`x %s\n\r", counter, ch->pcdata->ignored_accounts[i]);
          counter++;
        }
      }
      return;
    }

    d.original = NULL;
    if ((victim = get_char_world_account(arg)) != NULL) {
      online = TRUE;
      account = TRUE;
    }
    else if ((victim = get_victim_world(ch, arg)) != NULL) {
      online = TRUE;
    }
    else {
      log_string("DESCRIPTOR: Ignore");
      if (!load_char_obj(&d, arg)) {
        printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg));
        return;
      }
      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg));
      stat(buf, &sb);
      victim = d.character;
    }

    if (account == TRUE) {
      // checking ignored account list
      for (i = 0; i < 50; i++) {
        if (!str_cmp(victim->pcdata->account->name, ch->pcdata->ignored_accounts[i]) && stored == FALSE) {
          printf_to_char(ch, "%s will no longer be ignored.\n\r", capitalize(arg));
          free_string(ch->pcdata->ignored_accounts[i]);
          ch->pcdata->ignored_accounts[i] = str_dup("");
          stored = TRUE;
          i = 50;
        }
      }
    }
    else {
      // checking ignored character list
      for (i = 0; i < 50; i++) {
        if (!str_cmp(victim->name, ch->pcdata->ignored_characters[i]) && stored == FALSE) {
          printf_to_char(ch, "%s will no longer be ignored.\n\r", capitalize(arg));
          free_string(ch->pcdata->ignored_characters[i]);
          ch->pcdata->ignored_characters[i] = str_dup("");
          stored = TRUE;
          i = 50;
        }
      }
    }

    if (IS_NPC(victim)) {
      send_to_char("NPCs don't send tells.\n\r", ch);
      errored = TRUE;
    }
    else if (victim == ch) {
      send_to_char("You can't ignore yourself.\n\r", ch);
      errored = TRUE;
    }
    else if (IS_IMMORTAL(victim)) {
      send_to_char("You can't ignore immortals.\n\r", ch);
      errored = TRUE;
    }
    else {
      if (account == TRUE) {
        // Looking for blank spot for new account
        for (i = 0; i < 50; i++) {
          if (!str_cmp("", ch->pcdata->ignored_accounts[i]) && stored == FALSE) {
            ch->pcdata->ignored_accounts[i] =
            str_dup(victim->pcdata->account->name);
            printf_to_char(ch, "%s will be ignored.\n\r", capitalize(arg));
            stored = TRUE;
            i = 50;
          }
        }
      }
      else {
        // Looking for blank spot for new character
        for (i = 0; i < 50; i++) {
          if (!str_cmp("", ch->pcdata->ignored_characters[i]) && stored == FALSE) {
            ch->pcdata->ignored_characters[i] = str_dup(victim->name);
            printf_to_char(ch, "%s will be ignored.\n\r", capitalize(arg));
            stored = TRUE;
            i = 50;
          }
        }
      }
    }

    if (!online) {
      free_char(victim);
    }

    if (stored == FALSE && errored == FALSE) {
      send_to_char("Your ignore list is already full.  Please remove someone from it first.\n\r", ch);
    }
    return;
  }

  _DOFUN(do_tell) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (!is_gm(ch)) {
      smash_vector(argument);
      do_function(ch, &do_ptell, argument);
      return;
    }
    if (!IS_IMMORTAL(ch)) {
      smash_vector(argument);
      do_function(ch, &do_stell, argument);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->account != NULL) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL) || IS_FLAG(ch->comm, COMM_DEAF)) {
        send_to_char("Your message didn't get through.\n\r", ch);
        return;
      }
    }
    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }

    /*
    * Can tell to PC's anywhere, but NPC's only in same room.
    * -- Furey
    */
    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim) && victim->in_room != ch->in_room)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_IMMORTAL(victim) && victim->invis_level > ch->level) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
      act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s tells you '`i%s'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
      return;
    }
    // added a check for NPCs because it didn't seem necessary and was suspicious
    // - Discordance
    if (IS_NPC(victim)) {
      send_to_char("NPCs can't hear you.\n\r", ch);
      return;
    }

    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }

    if (IS_FLAG(victim->comm, COMM_AFK)) {
      if (IS_NPC(victim)) {
        act("$E is AFK, and not receiving tells.", ch, NULL, victim, TO_CHAR);
        return;
      }

      act("$E is AFK, but your tell will go through when $E returns.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s tells you '`i%s`x'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
    }

    if (!IS_IMMORTAL(ch)) {
      if (silenced(ch) && ch->in_room != victim->in_room) {
        return;
      }
      if (is_helpless(ch) && ch->in_room != victim->in_room) {
        return;
      }
    }
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);
    printf_to_char(ch, "You tell %s '`i%s`x'", NAME(victim), argument);
    if (same_player(ch, victim) || IS_IMMORTAL(victim) || (!spammer(ch) && mod_flagged == FALSE)) {
      printf_to_char(victim, "%s tells you '`i%s`x'", PERS(ch, victim), argument);
      victim->reply = ch;
      victim->reply_type = REPLY_PLAYER;
    }
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }

    return;
  }

  _DOFUN(do_stell) {
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (!is_gm(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL) || IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }

    /*
    * Can tell to PC's anywhere, but NPC's only in same room.
    * -- Furey
    */
    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim) && victim->in_room != ch->in_room)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    if (IS_NPC(victim) || victim->pcdata == NULL || victim->pcdata->account == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOTELL)) {
      act("$E has had $S ability to use tells revoked.", ch, 0, victim, TO_CHAR);
      return;
    }
    if (IS_IMMORTAL(victim) && victim->invis_level > ch->level) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
      act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s tells you '`i%s'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
      return;
    }

    // added a check for NPCs because it didn't seem necessary and was suspicious
    // - Discordance
    if (IS_NPC(victim)) {
      send_to_char("NPCs can't hear you.\n\r", ch);
      return;
    }

    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }

    if (IS_FLAG(victim->comm, COMM_AFK)) {
      if (IS_NPC(victim)) {
        act("$E is AFK, and not receiving tells.", ch, NULL, victim, TO_CHAR);
        return;
      }

      act("$E is AFK, but your tell will go through when $E returns.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s tells you '`i%s`x'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
    }

    printf_to_char(ch, "You tell %s '`i%s`x'", NAME(victim), argument);
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);

    if (same_player(ch, victim) || IS_IMMORTAL(victim) || (!spammer(ch) && mod_flagged == FALSE)) {
      printf_to_char(victim, "Story runner %s tells you '`i%s`x'", ch->name, argument);
      victim->reply = ch;
      victim->reply_type = REPLY_PLAYER;
    }
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }
    return;
  }

  _DOFUN(do_otell) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    if (available_donated(ch) < 100) {
      send_to_char("You haven't donated enough for that.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL) || IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);
    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }

    /*
    * Can tell to PC's anywhere, but NPC's only in same room.
    * -- Furey
    */
    
    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim) && victim->in_room != ch->in_room)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }

    printf_to_char(ch, "You tell %s '`i$t`x'", NAME(victim), argument);
    printf_to_char(victim, "%s tells you '`i$t`x'", NAME(ch), argument);
    return;
  }
  
  _DOFUN(do_aid) {
    abuse_check(ch, argument);
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char *strtime;
    bool ignored = FALSE;

    smash_vector(argument);

    strtime = ctime(&current_time);
    strtime[safe_strlen(strtime) - 1] = '\0';

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL) || IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOCHANNELS)) {
      send_to_char("Your channels have been revoked.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }
    
    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim) && victim->in_room != ch->in_room) || IS_IMMORTAL(victim)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    
    if (IS_NPC(victim)) {
      send_to_char("You can't do that to NPCS.\n\r", ch);
      return;
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
      act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s tells you '`i%s'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
      return;
    }

    // checking ignored list
    for (int i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->ignored_accounts[i])) {
        ignored = TRUE;
        i = 50;
      }
      if (ignored == FALSE) {
        for (int n = 0; n < 25; n++) {
          if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(ch->pcdata->account->characters[n], victim->pcdata->ignored_characters[i])) {
              ignored = TRUE;
              i = 50;
              n = 25;
            }
          }
        }
      }
    }
    
    if (ignored == TRUE) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }
    
    if (victim->pcdata->account == NULL) {
      return;
    }

    if (ch->played / 3600 > 10 && victim->played / 3600 > 10 && ch->pcdata->account->maxhours > 200 && victim->pcdata->account->maxhours > 200) {
      send_to_char("This command is to help newbies.\n\r", ch);
      return;
    }
    
    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }
    
    act("You aid $N by telling $M OOCly '`i$t`x'", ch, argument, victim, TO_CHAR);
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);
    
    if (same_player(ch, victim) || IS_IMMORTAL(victim) || (!spammer(ch) && mod_flagged == FALSE)) {
      act("$n aids you by telling you OOCly '`i$t`x'", ch, argument, victim, TO_VICT);
      if (victim->pcdata->account->maxhours < 50)
      act("Use aid $n (message) to reply", ch, NULL, victim, TO_VICT);
    }
    
    sprintf(buf, "Aid: %s %s\n\r", strtime, argument);
    append_file(ch, "../log/aids.log", buf);

    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }
    return;
  }

  _DOFUN(do_question) {
    abuse_check(ch, argument);
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char *strtime;

    strtime = ctime(&current_time);
    strtime[safe_strlen(strtime) - 1] = '\0';

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL) || IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOCHANNELS)) {
      send_to_char("Your channels have been revoked.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_QUIET)) {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_DEAF)) {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Tell whom what?\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim) && victim->in_room != ch->in_room)) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
      act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s tells you '`i%s'\n\r", PERS(ch, victim), argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
      return;
    }
    
    if (ch->played / 3600 > 10 && victim->played / 3600 > 10 && ch->pcdata->account->maxhours > 200 && victim->pcdata->account->maxhours > 200) {
      send_to_char("This command is to help newbies.\n\r", ch);
      return;
    }
    
    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }
    
    act("You question $N '`i$t`x'", ch, argument, victim, TO_CHAR);
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);
    if (same_player(ch, victim) || IS_IMMORTAL(victim) || (!spammer(ch) && mod_flagged == FALSE))
    act("$n questions you '`i$t`x'", ch, argument, victim, TO_VICT);

    sprintf(buf, "Question: %s %s\n\r", strtime, argument);
    append_file(ch, "../log/aids.log", buf);

    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }

    return;
  }

  _DOFUN(do_reply) {
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    bool ignored = FALSE;

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL)) {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
    }

    if ((victim = ch->reply) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    // checking ignored list
    for (int i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->ignored_accounts[i])) {
        ignored = TRUE;
        i = 50;
      }
      if (ignored == FALSE) {
        for (int n = 0; n < 25; n++) {
          if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(ch->pcdata->account->characters[n], victim->pcdata->ignored_characters[i])) {
              ignored = TRUE;
              i = 50;
              n = 25;
            }
          }
        }
      }
    }
    
    if (ignored == TRUE) {
      act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }
    char targetname[MSL];
    char sendername[MSL];

    if (IS_IMMORTAL(victim)) {
      sprintf(targetname, "%s", PERS(victim, ch));
      sprintf(sendername, "%s", ch->name);
    }
    else if (IS_IMMORTAL(ch)) {
      sprintf(targetname, "%s", victim->name);
      sprintf(sendername, "%s", PERS(ch, victim));
    }
    else if (ch->reply_type == REPLY_ACCOUNT) {
      sprintf(targetname, "%s", victim->pcdata->account->name);
      sprintf(sendername, "%s", ch->pcdata->account->name);
    }
    else if (ch->reply_type == REPLY_PLAYER || is_gm(victim)) {
      sprintf(targetname, "%s", victim->name);
      sprintf(sendername, "%s", ch->name);
    }
    else if (ch->reply_type == REPLY_CHARACTER) {
      sprintf(targetname, "%s", PERS(victim, ch));
      sprintf(sendername, "%s", PERS(ch, victim));
    }
    else if (ch->reply_type == REPLY_DREAM) {
      sprintf(targetname, "%s", dream_name(victim));
      sprintf(sendername, "%s", dream_name(ch));
    }
    else {
      sprintf(targetname, "%s", "They");
      sprintf(sendername, "%s", "Someone");
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
      printf_to_char(ch, "%s seems to have misplaces their link... try again later.", targetname);
      sprintf(buf, "%s tells you '`i%s`x'\n\r", sendername, argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
      return;
    }

    if ((IS_FLAG(victim->comm, COMM_QUIET) || IS_FLAG(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim)) {
      act_new("They are not receiving tells.", ch, 0, victim, TO_CHAR, POS_DEAD);
      return;
    }

    bool mod_flagged = FALSE;
    if (ch->played / 3600 < 50)
    mod_flagged = isTextFlagged(argument);

    if (IS_FLAG(victim->comm, COMM_AFK)) {
      if (IS_NPC(victim)) {
        act_new("$E is AFK, and not receiving tells.", ch, NULL, victim, TO_CHAR, POS_DEAD);
        return;
      }

      if (ch->reply_type != REPLY_ACCOUNT)
      act_new("They are AFK, but your tell will go through when they return.", ch, NULL, victim, TO_CHAR, POS_DEAD);
      sprintf(buf, "%s tells you '`i%s`x'\n\r", sendername, argument);
      buf[0] = UPPER(buf[0]);
      victim->pcdata->buffer->strcat(buf);
      // add_buf(victim->pcdata->buffer,buf);
    }

    if (!IS_NPC(ch) && !IS_NPC(victim) && ch != victim) {
      ch->pcdata->secondary_timer = 0;
      ch->pcdata->tertiary_timer = 0;
    }

    if (!is_gm(ch) && !is_gm(victim)) {
      printf_to_char(ch, "You tell %s OOCly '`i%s`x'", targetname, argument);
      if (mod_flagged == FALSE)
      printf_to_char(victim, "%s tells you OOCly '`i%s`x'", sendername, argument);
    }
    else {
      printf_to_char(ch, "You tell %s '`i%s`x'", targetname, argument);
      if (mod_flagged == FALSE)
      printf_to_char(victim, "%s tells you '`i%s`x'", sendername, argument);
    }
    victim->reply = ch;
    victim->reply_type = ch->reply_type;
    return;
  }

  char *spyware_message(CHAR_DATA *ch, CHAR_DATA *victim, int type, char *argument, CHAR_DATA *to, bool possessive, bool selfnamed) {
    int i;

    char buf[MSL], word[MSL], newstring[MSL], blah[MSL];
    char tmp[MAX_STRING_LENGTH];
    char *argy;
    argy = str_dup("");
    free_string(argy);

    argy = str_dup(argument);
    newstring[0] = 0;
    while (*argy && *argy != '\0' && safe_strlen(argy) > 0) {
      memset(buf, 0, MSL);
      argy = one_argument_true(argy, buf);
      memset(word, 0, MSL);
      word[0] = 0;
      if (buf[0] == '@') {
        for (i = 1; isalpha(buf[i]) && buf[i] != ' ' && buf[i] != '\0'; i++) {
          sprintf(blah, "%c", buf[i]);
          strcat(word, blah);
        }
        if (!strcmp(word, "me") || !strcmp(word, "self")) {
          selfnamed = TRUE;
          strcat(newstring, get_intro(ch));
          if (tmp[0] != '.')
          //              if(isalpha(tmp[0]))
          //              {
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
          //              }
        }
        else if (!strcmp(word, "my") || !strcmp(word, "mine")) {
          selfnamed = TRUE;
          strcat(newstring, get_intro(ch));
          strcat(newstring, "'s");
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");

        }
        else if (!str_cmp(word, "line") || !str_cmp(word, "newline")) {
          strcat(newstring, " \n\r");
        }
        else if ((victim = get_char_vision(ch, NULL, word)) != NULL) {

          if (ispunct(buf[i]) && buf[i + 1] == 's') {
            strcat(newstring, get_intro(victim));
            strcat(newstring, "'s");
            if (buf[i + 2] == '.')
            strcat(newstring, ".");
            if (buf[i + 2] == ',')
            strcat(newstring, ",");
            if (buf[i + 2] == '\'')
            strcat(newstring, "'");
            if (buf[i + 2] == '"')
            strcat(newstring, "\"");

            strcat(newstring, " ");
          }
          else {
            strcat(newstring, get_intro(victim));
            if (buf[i] == '.')
            strcat(newstring, ".");
            if (buf[i] == ',')
            strcat(newstring, ",");
            if (buf[i] == '\'')
            strcat(newstring, "'");
            if (buf[i] == '"')
            strcat(newstring, "\"");

            strcat(newstring, " ");
          }
        }
        else {
          strcat(newstring, "someone");

          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
        }
      }
      else {
        strcat(newstring, buf);
        strcat(newstring, " ");
      }
    }
    if (selfnamed == TRUE)
    sprintf(tmp, "%s", newstring);
    else if (possessive == TRUE)
    sprintf(tmp, "%s%s", get_intro(ch), newstring);
    else
    sprintf(tmp, "%s %s", get_intro(ch), newstring);
    return str_dup(tmp);
  }

  void emotespy(CHAR_DATA *ch, CHAR_DATA *victim, int type, char *argument, bool possessive, bool selfnamed) {
    CHAR_DATA *to;

    ROOM_INDEX_DATA *room = ch->in_room;
    if (room == NULL)
    return;

    if(type == EMOTE_ATTEMPT)
    return;

    if (public_room(room))
    return;

    if (room->area->vnum == 1 || room->area->vnum == 12 || !in_haven(room)) {
      return;
    }

    if (mist_room(room))
    return;

    if (is_ghost(ch))
    return;

    if (is_dark(room))
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD))
    return;

    if (room_in_school(room->vnum))
    return;

    EXTRA_DESCR_DATA *ed;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("!bugs", ed->keyword))
      break;
    }
    
    if (!ed) {
      return;
    }
    char buf[MSL];
    char mess[MSL];
    char newmat[MSL];
    OBJ_DATA *obj;

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE)
      continue;
      else if (IS_SET(obj->extra_flags, ITEM_OFF))
      continue;

      if (obj->value[0] < 100)
      continue;

      to = NULL;
      if (obj->carried_by != NULL && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        to = obj->carried_by;
      }
      else {
        if (obj->in_obj != NULL && obj->in_obj->carried_by != NULL && !IS_SET(obj->in_obj->extra_flags, ITEM_WARDROBE))
        to = obj->in_obj->carried_by;
        else
        to = NULL;
      }
      if (to == NULL)
      continue;

      if (!cell_signal(to))
      continue;

      if (obj->material == NULL)
      obj->material = str_dup("");

      if (obj->material == NULL || safe_strlen(obj->material) > 20000)
      continue;

      sprintf(buf, "%d", obj->value[0]);
      if (is_name(buf, ed->description)) {
        sprintf(mess, "From Camera at %s: %s\n\r", ch->in_room->name, spyware_message(ch, victim, type, argument, to, possessive, selfnamed));

        sprintf(newmat, "%s\n%s", obj->material, mess);
        free_string(obj->material);
        obj->material = str_dup(newmat);

        if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
          act("Your $p vibrates.", to, obj, NULL, TO_CHAR);
        }
        else {
          act("Your $p beeps.", to, obj, NULL, TO_CHAR);
          act("$n's $p beeps.", to, obj, NULL, TO_ROOM);
        }
      }
    }
  }

  char *emote_name(CHAR_DATA *ch) {
    char buf[MSL];
    if (IS_NPC(ch))
    return ch->short_descr;
    if (is_dreaming(ch)) {
      return dream_name(ch);
    }
    else if (is_animal(ch)) {
      sprintf(buf, "%s(%s)", ch->name, get_animal_species(ch, ANIMAL_ACTIVE));
      return str_dup(buf);
    }
    return ch->name;
  }

  void log_personal(CHAR_DATA *ch, CHAR_DATA *to, int type, char *argument, bool possessive, bool selfnamed) {
    int i;
    char buf[MSL], word[MSL], newstring[MSL], blah[MSL];
    char tmp[MAX_STRING_LENGTH];
    char *argy;
    CHAR_DATA *victim;
    //    argy = str_dup("");
    //    free_string(argy);
    argy = str_dup(argument);
    //            newstring[0] = '\0';
    newstring[0] = 0;
    bool hearing = FALSE;
    if (is_dreaming(ch))
    hearing = TRUE;
    else if (type == EMOTE_SUBTLE)
    hearing = can_hear(to, ch, VOLUME_QUIET);
    else
    hearing = can_hear(to, ch, VOLUME_NORMAL);
    bool verbal = FALSE;
    char gagword[MSL];
    while (*argy && *argy != '\0' && safe_strlen(argy) > 0) {
      memset(buf, 0, MSL);
      argy = one_argument_true(argy, buf);
      memset(word, 0, MSL);
      word[0] = 0;
      if (buf[0] == '"' || verbal == TRUE) {
        verbal = TRUE;
        for (i = 0; (buf[i] != '"' || i == 0) && buf[i] != '\0'; i++) {
          if (buf[i] == '"' && i > 0)
          verbal = FALSE;
          sprintf(blah, "%c", buf[i]);
          strcat(word, blah);

          if ((isalpha(buf[i]) || buf[i] == '.' || buf[i] == '!' || buf[i] == '?') && (i == 0 || buf[i - 1] != '`')) {
            if (buf[i] == 'o' || buf[i] == 'O' || buf[i] == '.' || buf[i] == '!' || buf[i] == '?' || buf[i] == 'e' || buf[i] == 'E')
            sprintf(blah, "%c", buf[i]);
            else if (isupper(buf[i]))
            sprintf(blah, "%c", toupper(gagletter()));
            else
            sprintf(blah, "%c", gagletter());
            strcat(gagword, blah);
          }
        }
        if (buf[i] == '"' && i > 0)
        verbal = FALSE;
        if (hearing && type != EMOTE_PRIVATE && type != EMOTE_ILLUSION) {
          strcat(newstring, mangle_word(ch, to, ch->pcdata->speaking, word));
        }
        else if (buf[0] == '"') {
          strcat(newstring, "\"");
          if (IS_FLAG(ch->comm, COMM_GAG))
          strcat(newstring, gagword);
          else
          strcat(newstring, "...");
        }
        else {
          if (IS_FLAG(ch->comm, COMM_GAG))
          strcat(newstring, gagword);
          else
          strcat(newstring, "...");
        }
        if (verbal == FALSE)
        strcat(newstring, "`x\"");
        strcat(newstring, " ");
      }
      else if (buf[0] == '@') {
        for (i = 1; isalpha(buf[i]) && buf[i] != ' ' && buf[i] != '\0'; i++) {
          sprintf(blah, "%c", buf[i]);
          strcat(word, blah);
        }
        if (!str_cmp(word, "me") || !str_cmp(word, "self")) {
          selfnamed = TRUE;
          strcat(newstring, "`W");
          strcat(newstring, PERS(ch, to));
          strcat(newstring, "`x");
          if (tmp[0] != '.')
          //              if(isalpha(tmp[0]))
          //              {
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
          //              }
        }
        else if (!str_cmp(word, "my") || !str_cmp(word, "mine")) {
          selfnamed = TRUE;
          strcat(newstring, "`W");
          strcat(newstring, PERS(ch, to));
          strcat(newstring, "'s");
          strcat(newstring, "`x");
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");

        }
        else if (!str_cmp(word, "line") || !str_cmp(word, "newline")) {
          strcat(newstring, " \n\r");
        }
        else if ((victim = get_char_vision(ch, NULL, word)) != NULL) {
          if (ispunct(buf[i]) && buf[i + 1] == 's') {
            if (to != victim) {
              strcat(newstring, PERS_3(victim, to));
              strcat(newstring, "'s");
              if (buf[i + 2] == '.')
              strcat(newstring, ".");
              if (buf[i + 2] == ',')
              strcat(newstring, ",");
              if (buf[i + 2] == '\'')
              strcat(newstring, "'");
              if (buf[i + 2] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
            else {
              strcat(newstring, "`W");
              strcat(newstring, emote_name(victim));
              strcat(newstring, "'s");
              strcat(newstring, "`x");
              if (buf[i + 2] == '.')
              strcat(newstring, ".");
              if (buf[i + 2] == ',')
              strcat(newstring, ",");
              if (buf[i + 2] == '\'')
              strcat(newstring, "'");
              if (buf[i + 2] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");

              move_closer(ch, victim, 1);
            }
          }
          else {
            if (to != victim) {
              strcat(newstring, PERS_3(victim, to));
              if (buf[i] == '.')
              strcat(newstring, ".");
              if (buf[i] == ',')
              strcat(newstring, ",");
              if (buf[i] == '\'')
              strcat(newstring, "'");
              if (buf[i] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
            else {
              strcat(newstring, "`W");
              strcat(newstring, emote_name(victim));
              strcat(newstring, "`x");
              if (buf[i] == '.')
              strcat(newstring, ".");
              if (buf[i] == ',')
              strcat(newstring, ",");
              if (buf[i] == '\'')
              strcat(newstring, "'");
              if (buf[i] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
          }
        }
        else {
          strcat(newstring, "someone");

          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
        }
      }
      else {
        strcat(newstring, buf);
        strcat(newstring, " ");
      }
    }

    if (selfnamed == TRUE || is_gm(ch) || type == EMOTE_ILLUSION || type == EMOTE_EMIT)
    sprintf(tmp, "%s", newstring);
    else if (goddreamer(ch))
    sprintf(tmp, "%s", newstring);
    else if (possessive == TRUE)
    sprintf(tmp, "%s%s", PERS(ch, to), newstring);
    else
    sprintf(tmp, "%s %s", PERS(ch, to), newstring);
    if (type != EMOTE_INTERNAL)
    char_rplog(to, str_dup(tmp));
    else
    prp_rplog(to, str_dup(tmp));
  }

  void log_message(CHAR_DATA *ch, int type, char *argument, bool possessive, bool selfnamed, CHAR_DATA *target) {
    int i;
    CHAR_DATA *victim;
    char buf[MSL], word[MSL], newstring[MSL], blah[MSL];
    char tmp[MAX_STRING_LENGTH];
    char *argy;

    if(type == EMOTE_ATTEMPT)
    return;

    CHAR_DATA *log;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        log = d->character;

        if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(log->comm, COMM_MANDRAKE) && !IS_FLAG(log->act, PLR_SHROUD) && !guestmonster(ch))
        continue;

        if (!IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(log->comm, COMM_MANDRAKE) && IS_FLAG(log->act, PLR_SHROUD) && !guestmonster(ch))
        continue;

        if (IS_FLAG(ch->act, PLR_DEEPSHROUD) != IS_FLAG(log->act, PLR_DEEPSHROUD))
        continue;

        if (battleground(ch->in_room) && combat_distance(ch, log, FALSE) > 100)
        continue;

        if (type == EMOTE_EMOTE && crowded_room(ch->in_room) && (ch->in_room != log->in_room || !same_place(ch, log)))
        continue;

        if (type == EMOTE_EMOTE && crowded_room(log->in_room) && (ch->in_room != log->in_room || !same_place(ch, log)))
        continue;

        if (type == EMOTE_SUBTLE && (ch->in_room != log->in_room || !same_place(ch, log)))
        continue;

        if (type == EMOTE_INTERNAL && log != ch)
        continue;
        if (type == EMOTE_PRIVATE && log != ch && log != target)
        continue;
        if (type == EMOTE_ILLUSION && log != ch && log != target)
        continue;

        if (can_rp_log(log, ch)) {
          log_personal(ch, log, type, argument, possessive, selfnamed);
        }
      }
    }

    if (type == EMOTE_INTERNAL || type == EMOTE_ILLUSION || type == EMOTE_PRIVATE || type == EMOTE_SUBTLE)
    return;

    //    argy = str_dup("");
    //    free_string(argy);
    argy = str_dup(argument);
    //            newstring[0] = '\0';
    newstring[0] = 0;
    while (*argy && *argy != '\0' && safe_strlen(argy) > 0) {
      memset(buf, 0, MSL);
      argy = one_argument_true(argy, buf);
      memset(word, 0, MSL);
      word[0] = 0;
      if (buf[0] == '@') {
        for (i = 1; isalpha(buf[i]) && buf[i] != ' ' && buf[i] != '\0'; i++) {
          sprintf(blah, "%c", buf[i]);
          strcat(word, blah);
        }

        if (!str_cmp(word, "me") || !str_cmp(word, "self")) {
          selfnamed = TRUE;
          strcat(newstring, "`W");
          strcat(newstring, emote_name(ch));
          strcat(newstring, "`x");
          if (tmp[0] != '.')
          //              if(isalpha(tmp[0]))
          //              {
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
          //              }
        }
        else if (!str_cmp(word, "my") || !str_cmp(word, "mine")) {
          selfnamed = TRUE;
          strcat(newstring, "`W");
          strcat(newstring, emote_name(ch));
          strcat(newstring, "'s");
          strcat(newstring, "`x");
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");

          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");

        }
        else if (!str_cmp(word, "line") || !str_cmp(word, "newline")) {
          strcat(newstring, " \n\r");
        }
        else if ((victim = get_char_vision(ch, NULL, word)) != NULL) {

          if (ispunct(buf[i]) && buf[i + 1] == 's') {

            strcat(newstring, emote_name(victim));
            strcat(newstring, "'s");
            if (buf[i + 2] == '.')
            strcat(newstring, ".");
            if (buf[i + 2] == ',')
            strcat(newstring, ",");
            if (buf[i + 2] == '\'')
            strcat(newstring, "'");
            if (buf[i + 2] == '"')
            strcat(newstring, "\"");

            strcat(newstring, " ");
          }
          else {

            strcat(newstring, emote_name(victim));
            if (buf[i] == '.')
            strcat(newstring, ".");
            if (buf[i] == ',')
            strcat(newstring, ",");
            if (buf[i] == '\'')
            strcat(newstring, "'");
            if (buf[i] == '"')
            strcat(newstring, "\"");

            strcat(newstring, " ");
          }
        }
        else {
          strcat(newstring, "someone");
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
        }
      }
      else {
        strcat(newstring, buf);
        strcat(newstring, " ");
      }
    }
    if (selfnamed == TRUE || is_gm(ch) || type == EMOTE_ILLUSION || type == EMOTE_EMIT)
    sprintf(tmp, "%s\n", newstring);
    else if (is_dreaming(ch) && goddreamer(ch))
    sprintf(tmp, "%s\n", newstring);
    else if (possessive == TRUE)
    sprintf(tmp, "%s%s\n", emote_name(ch), newstring);
    else
    sprintf(tmp, "%s %s\n", emote_name(ch), newstring);
    if (battleground(ch->in_room))
    op_report(str_dup(tmp), ch);
    rp_log(str_dup(tmp));
    logevent_check(ch, str_dup(tmp));
  }

  char *remnant_dream_message(CHAR_DATA *ch, CHAR_DATA *victim, int type, char *argument, CHAR_DATA *to, bool possessive, bool selfnamed) {
    int i;

    char lstr[MSL];
    sprintf(lstr, "NIGHTMARE: %s %s.", to->name, argument);
    log_string(lstr);
    char buf[MSL], word[MSL], newstring[MSL], blah[MSL];
    char tmp[MAX_STRING_LENGTH];
    char *argy;
    //    argy = str_dup("");
    //    free_string(argy);
    argy = str_dup(argument);
    //            newstring[0] = '\0';
    newstring[0] = 0;
    while (*argy && *argy != '\0' && safe_strlen(argy) > 0) {
      memset(buf, 0, MSL);
      argy = one_argument_true(argy, buf);
      memset(word, 0, MSL);
      word[0] = 0;
      if (buf[0] == '@') {
        for (i = 1; isalpha(buf[i]) && buf[i] != ' ' && buf[i] != '\0'; i++) {
          sprintf(blah, "%c", buf[i]);
          strcat(word, blah);
        }

        if (!str_cmp(word, "me") || !str_cmp(word, "self")) {
          selfnamed = TRUE;
          strcat(newstring, "`W");
          strcat(newstring, to->name);
          strcat(newstring, "`x");
          if (tmp[0] != '.')
          //              if(isalpha(tmp[0]))
          //              {
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
          //              }
        }
        else if (!str_cmp(word, "my") || !str_cmp(word, "mine")) {
          selfnamed = TRUE;
          strcat(newstring, "`W");
          strcat(newstring, to->name);
          strcat(newstring, "'s");
          strcat(newstring, "`x");
          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");

        }
        else if (!str_cmp(word, "line") || !str_cmp(word, "newline")) {
          strcat(newstring, " \n\r");
        }
        else if ((victim = get_char_vision(ch, NULL, word)) != NULL) {

          if (ispunct(buf[i]) && buf[i + 1] == 's') {
            if (to != victim) {
              strcat(newstring, PERS_3(victim, to));
              strcat(newstring, "'s");
              if (buf[i + 2] == '.')
              strcat(newstring, ".");
              if (buf[i + 2] == ',')
              strcat(newstring, ",");
              if (buf[i + 2] == '\'')
              strcat(newstring, "'");
              if (buf[i + 2] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
            else {
              strcat(newstring, "`W");
              strcat(newstring, victim->name);
              strcat(newstring, "'s");
              strcat(newstring, "`x");
              if (buf[i + 2] == '.')
              strcat(newstring, ".");
              if (buf[i + 2] == ',')
              strcat(newstring, ",");
              if (buf[i + 2] == '\'')
              strcat(newstring, "'");
              if (buf[i + 2] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
          }
          else {
            if (to != victim) {
              strcat(newstring, PERS_3(victim, to));
              if (buf[i] == '.')
              strcat(newstring, ".");
              if (buf[i] == ',')
              strcat(newstring, ",");
              if (buf[i] == '\'')
              strcat(newstring, "'");
              if (buf[i] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
            else {
              strcat(newstring, "`W");
              strcat(newstring, victim->name);
              strcat(newstring, "`x");
              if (buf[i] == '.')
              strcat(newstring, ".");
              if (buf[i] == ',')
              strcat(newstring, ",");
              if (buf[i] == '\'')
              strcat(newstring, "'");
              if (buf[i] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
          }
        }
        else {
          strcat(newstring, "someone");

          if (buf[i] == '.')
          strcat(newstring, ".");
          if (buf[i] == ',')
          strcat(newstring, ",");
          if (buf[i] == '\'')
          strcat(newstring, "'");
          if (buf[i] == '"')
          strcat(newstring, "\"");

          strcat(newstring, " ");
        }
      }
      else {
        strcat(newstring, buf);
        strcat(newstring, " ");
      }
    }

    if (selfnamed == TRUE)
    sprintf(tmp, "%s", newstring);
    else if (possessive == TRUE)
    sprintf(tmp, "%s%s", PERS(to, to), newstring);
    else
    sprintf(tmp, "%s %s", PERS(to, to), newstring);
    return str_dup(tmp);
  }

  void remnant_dream(CHAR_DATA *ch, CHAR_DATA *victim, int type, char *argument, bool possessive, bool selfnamed) {
    CHAR_DATA *to;
    char buf[MSL];

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        
        if (IS_NPC(to))
        continue;
      
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;

        if (is_super(to) || get_skill(to, SKILL_TOUCHED) < 1)
        continue;

        if (number_percent() % 3 != 0)
        continue;

        if (safe_strlen(to->pcdata->nightmare) < 4) {
          if (number_percent() % 4 == 0) {
            sprintf(buf, "%s\n\n%s\n\r", ch->in_room->description, remnant_dream_message(ch, victim, type, argument, to, possessive, selfnamed));
          }
          else if (number_percent() % 3 == 0) {
            sprintf(buf, "%s\n\n%s\n\r", ch->in_room->name, remnant_dream_message(ch, victim, type, argument, to, possessive, selfnamed));
          }
          else {
            sprintf(buf, "Somewhere\n\n%s\n\r", remnant_dream_message(ch, victim, type, argument, to, possessive, selfnamed));
          }
          free_string(to->pcdata->nightmare);
          to->pcdata->nightmare = str_dup(buf);
        }
        else if (number_percent() % 4 == 0) {
          sprintf(buf, "%s\n\n%s\n\r", to->pcdata->nightmare, remnant_dream_message(ch, victim, type, argument, to, possessive, selfnamed));
          free_string(to->pcdata->nightmare);
          to->pcdata->nightmare = str_dup(buf);
        }
      }
    }
  }

  void emote_speech(CHAR_DATA *ch, CHAR_DATA *to, char *argument) {
    char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL], buf[MSL];
    bool eataletter = FALSE;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    bool ended = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '"') {
        if (rectotalk == FALSE) {
          eataletter = TRUE;
          rectotalk = TRUE;
          if (ended == TRUE) {
            strcat(rectalk, " ");
            ended = FALSE;
          }
        }
        else {
          rectotalk = FALSE;
          ended = TRUE;
        }
      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else {
          if (eataletter == TRUE && tmp[i] == ' ') {
            eataletter = FALSE;
          }
          else {
            sprintf(blah, "%c", tmp[i]);
            strcat(talktalk, blah);
          }
        }
      }
    }
    if (safe_strlen(rectalk) < 2)
    return;

    sprintf(buf, "Someone says, %s, '`o%s`x'\n\r", ch->pcdata->talk, mangle_text(ch, to, ch->pcdata->speaking, rectalk));
    page_to_char(wrap_string(buf, get_wordwrap(to)), to);
    triggercheck(to, mangle_text(ch, to, ch->pcdata->speaking, argument));
    persuade_check(ch, to, mangle_text(ch, to, ch->pcdata->speaking, argument));
  }

  void fight_talk(CHAR_DATA *ch, char *argument) {
    char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
    bool eataletter = FALSE;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    bool ended = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '"') {
        if (rectotalk == FALSE) {
          eataletter = TRUE;
          rectotalk = TRUE;
          if (ended == TRUE) {
            strcat(rectalk, " ");
            ended = FALSE;
          }
        }
        else {
          rectotalk = FALSE;
          ended = TRUE;
        }

      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else {
          if (eataletter == TRUE && tmp[i] == ' ') {
            eataletter = FALSE;
          }
          else {
            sprintf(blah, "%c", tmp[i]);
            strcat(talktalk, blah);
          }
        }
      }
    }

    if (safe_strlen(rectalk) >= 10)
    ch->debuff += safe_strlen(rectalk) / 10;
  }

  char gagletter(void) {
    if (number_percent() % 3 == 0)
    return 'm';
    else if (number_percent() % 2 == 0)
    return 'f';
    else
    return 'h';
  }

  bool can_see_emote(CHAR_DATA *ch, CHAR_DATA *to, CHAR_DATA *target, int type) {
    if (ch == to)
    return TRUE;
  
    if (to == target)
    return TRUE;

    if (ch->in_room == NULL || to->in_room == NULL)
    return FALSE;

    if(type == EMOTE_ATTEMPT)
    return FALSE;
  
    bool value = TRUE;
    FANTASY_TYPE *fant;

    if (!str_cmp(to->name, "Discordance") && to->level >= 106 && can_spy(to, ch) && IS_FLAG(to->act, PLR_SPYING)) {
      return TRUE;
    }

    if(!IS_IMMORTAL(to) && !IS_IMMORTAL(ch) && ch->in_room != NULL && ch != to && newbie_school(ch->in_room))
    return FALSE;

    if(!IS_NPC(ch) && ch->pcdata->cam_spy_char == to)
    return TRUE;

    if ((is_gm(ch) || is_gm(to)) && type == EMOTE_EMOTE && battleground(ch->in_room) && battleground(to->in_room))
    return TRUE;

    if (!IS_IMMORTAL(to) && IS_FLAG(ch->comm, COMM_PRIVATE) != IS_FLAG(to->comm, COMM_PRIVATE))
    return FALSE;

    if (is_dreaming(ch) && !IS_IMMORTAL(to)) {
      if (ch->pcdata->dream_room > 0 && to->pcdata->dream_room != ch->pcdata->dream_room)
      return FALSE;
    
      if (ch->pcdata->dream_room == 0 && ch->pcdata->dream_link != to && ch != to)
      return FALSE;

      if (ch->pcdata->dream_room > 0 && type == EMOTE_SUBTLE && str_cmp(ch->pcdata->dreamplace, to->pcdata->dreamplace) && safe_strlen(ch->pcdata->dreamplace) > 2 && !goddreamer(to))
      return FALSE;

      if (ch->pcdata->dream_room > 0 && type == EMOTE_PRIVATE && to != target && to != ch)
      return FALSE;

      bool fanblind = FALSE;
      if ((fant = in_fantasy(to)) != NULL) {
        if (part_of_fantasy(to, fant)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(to->name, fant->participants[i]) && fant->participant_blind[i] == TRUE)
            fanblind = TRUE;
          }
        }
      }
      if (fanblind == TRUE && type != EMOTE_PRIVATE)
      return FALSE;
      return TRUE;
    }
    if (is_gm(to) && ch->in_room != to->in_room && (!can_spy(to, ch) || !IS_FLAG(to->act, PLR_SPYING)))
    return FALSE;

    if ((is_gm(to) || higher_power(to)) && can_spy(to, ch) && IS_FLAG(to->act, PLR_SPYING))
    return TRUE;

    if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(to->act, PLR_SHROUD) && !IS_FLAG(ch->comm, COMM_MANDRAKE))
    value = FALSE;

    if (ch->in_room != to->in_room && type == EMOTE_SUBTLE && !is_gm(to) && !higher_power(to))
    value = FALSE;

    if (!is_gm(to) && !is_dreaming(ch)) {
      if (type == EMOTE_SUBTLE && (ch->in_room != to->in_room || !same_place(ch, to)))
      value = FALSE;
    
      if (type == EMOTE_PRIVATE && to != target && to != ch)
      value = FALSE;
    
      if (type == EMOTE_ILLUSION && to != target)
      value = FALSE;
    
      if (battleground(ch->in_room) && combat_distance(ch, to, FALSE) > 100)
      value = FALSE;
    
      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD) && !IS_FLAG(to->comm, COMM_MANDRAKE) && !guestmonster(ch))
      value = FALSE;
    }
    if (value == TRUE)
    return TRUE;

    if (to->pcdata->brandstatus >= 1) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *victim;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          victim = d->character;
          
          if (IS_NPC(victim) || victim->in_room == NULL)
          continue;
        
          if (str_cmp(victim->pcdata->brander, ch->name))
          continue;
        
          if (!str_cmp(ch->pcdata->brander, victim->name))
          continue;
        
          if (in_world(ch) != in_world(victim))
          continue;
        
          if (is_helpless(victim))
          continue;
        
          if (is_prisoner(victim))
          continue;
        
          if (prop_from_room(victim->in_room) != NULL && prop_from_room(victim->in_room)->warded > 50)
          continue;
        
          if (in_fight(victim))
          continue;
        
          if (victim == ch)
          return TRUE;
        
          if (can_see_emote(victim, to, target, type))
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  void process_emote(CHAR_DATA *ch, CHAR_DATA *target, int type, char *argument) {
    CHAR_DATA *to;
    CHAR_DATA *victim = NULL;
    FANTASY_TYPE *fant;
    bool first = TRUE;
    smash_percent(argument);
    smash_tilde(argument);
    if (!IS_IMMORTAL(ch)) {
      smash_vector(argument);
    }
    int i;
    int attencount = 0;
    char buf[MSL], word[MSL], newstring[MSL], blah[MSL], gagword[MSL];
    char *srcptr, *dstptr, tmp[MAX_STRING_LENGTH], tmp2[MAX_STRING_LENGTH];
    char *argy;
    char inter[MSL];
    argy = str_dup("");
    bool highlight_on = FALSE;

    bool selfnamed = FALSE;
    bool possessive = FALSE;

    if (!IS_NPC(ch) && IS_FLAG(ch->comm, COMM_NOEMOTE)) {
      send_to_char("You can't show your emotions.\n\r", ch);
      return;
    }
    
    if (IS_FLAG(ch->comm, COMM_GAG) && (strstr(argument, "\"") || strstr(argument, " '"))) {
      send_to_char("You're gagged.  Use say.\n\r", ch);
      return;
    }
    
    if (in_fight(ch) && ch->fight_fast == 0 && ch->fight_current != NULL && ch->fight_current != ch) {
      send_to_char("It isn't your turn yet.\n\r", ch);
      return;
    }

    if (crowded_room(ch->in_room) && safe_strlen(ch->pcdata->place) < 2 && !is_gm(ch) && !higher_power(ch)) {
      send_to_char("Find a place first.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum == 60)
    return;

    if (!IS_NPC(ch)) {
      if (battleground(ch->in_room) && safe_strlen(argument) >= 80)
      ch->pcdata->op_emotes++;

      ch->pcdata->time_since_emote = 0;
      if (safe_strlen(argument) >= 80)
      ch->pcdata->combat_emoted = TRUE;
      if (ch->pcdata->patrol_status > 1 && !IS_FLAG(ch->comm, COMM_PRIVATE) && type != EMOTE_ILLUSION && type != EMOTE_INTERNAL && type != EMOTE_PRIVATE && type != EMOTE_SUBTLE && type != EMOTE_ATTEMPT) {
        ch->pcdata->patrol_rp += safe_strlen(argument);
        char logstring[MSL];
        sprintf(logstring, "PATROLRP: %d:, (%s)%s", ch->pcdata->patrol_status, ch->name, argument);
        log_string(logstring);
        if (ch->pcdata->patrol_status == PATROL_PREY || ch->pcdata->patrol_status == PATROL_FLEEING || ch->pcdata->patrol_status == PATROL_HIDING || ch->pcdata->patrol_status == PATROL_GRABBED) {
          CHAR_DATA *pred;
          for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;
            
            if (d->connected != CON_PLAYING)
            continue;
          
            pred = CH(d);
            
            if (pred == NULL)
            continue;
          
            if (IS_NPC(pred))
            continue;
          
            if (pred->in_room == NULL)
            continue;
          
            if (is_gm(pred))
            continue;
          
            if (ch == pred)
            continue;
          
            if (pred->in_room != ch->in_room && !is_dreaming(ch))
            continue;

            villain_mod(pred, safe_strlen(argument) / 50, "Predatory Patrol");
          }
        }
      }
    }
    
    char *message;
    message = str_dup(argument);

    sprintf(inter, "%s", argument);

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0 && !is_dreaming(ch)) {
      send_to_char("You're asleep.\n\r", ch);
      return;
    }
    
    if (argument[0] == '\0') {
      send_to_char("Emote what?\n\r", ch);
      return;
    }
    
    if (!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_BARGAINING) {
      ch->pcdata->patrol_pledged += ch->pcdata->patrol_amount;
      ch->pcdata->patrol_amount = 0;
    }

    if (is_dreaming(ch)) {
      if ((fant = in_fantasy(ch)) != NULL) {
        if (part_of_fantasy(ch, fant)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->name, fant->participants[i]) && fant->participant_noaction[i] == TRUE) {
              send_to_char("You can't act in this dreamworld at the moment.\n\r", ch);
              return;
            }
          }
        }
      }
    }

    // emote limit check for ghosts - Discordance
    if (is_ghost(ch)) {
      if (is_manifesting(ch)) {
        if (type != EMOTE_PRIVATE && type != EMOTE_ATTEMPT) {
          if (deplete_ghostpool(ch, GHOST_EMOTE) == FALSE) {
            send_to_char("You don't have the strength to manifest your will any more today.\n\r", ch);
            return;
          }
        }
        else if (type == EMOTE_PRIVATE) {
          if (deplete_ghostpool(ch, GHOST_PRIVATE) == FALSE) {
            send_to_char("You can't muster the strength to manifest your will any more today.\n\r", ch);
            return;
          }
        }
      }
    }
    
    int seeingpop = 0;
    if (type != EMOTE_PRIVATE && type != EMOTE_ILLUSION && type != EMOTE_ATTEMPT && !is_dreaming(ch) && type != EMOTE_INTERNAL) {
      if (IS_FLAG(ch->act, PLR_HIDE))
      do_function(ch, &do_unhide, "");
    }
    
    bool hastalk = FALSE;
    srcptr = argument;
    dstptr = tmp;
    if (!isalpha(argument[0])) {
      possessive = TRUE;
    }
    
    if (strcasestr(argument, "@me ") != NULL || strcasestr(argument, "@self ") != NULL || strcasestr(argument, "@my ") != NULL || strcasestr(argument, "@mine ") != NULL || strcasestr(argument, "@me.") != NULL || strcasestr(argument, "@self.") != NULL || strcasestr(argument, "@my.") != NULL || strcasestr(argument, "@mine.") != NULL || strcasestr(argument, "@me,") != NULL || strcasestr(argument, "@self,") != NULL || strcasestr(argument, "@my,") != NULL || strcasestr(argument, "@mine,") != NULL || strcasestr(argument, "@me!") != NULL || strcasestr(argument, "@self!") != NULL || strcasestr(argument, "@my!") != NULL || strcasestr(argument, "@mine!") != NULL)
    selfnamed = TRUE;

    one_argument(argument, buf);
    
    if (!str_cmp(buf, "@me ") || !str_cmp(buf, "@self ") || !str_cmp(buf, "@my ") || !str_cmp(buf, "@mine ") || !str_cmp(buf, "@me.") || !str_cmp(buf, "@self.") || !str_cmp(buf, "@my.") || !str_cmp(buf, "@mine.") || !str_cmp(buf, "@me,") || !str_cmp(buf, "@self,") || !str_cmp(buf, "@my,") || !str_cmp(buf, "@mine,") || !str_cmp(buf, "@me!") || !str_cmp(buf, "@self!") || !str_cmp(buf, "@my!") || !str_cmp(buf, "@mine!"))
    selfnamed = TRUE;
    etag(ch);

    while (*srcptr) {
      if (*srcptr == '"') {
        hastalk = TRUE;
        highlight_on = !highlight_on;
        
        if (highlight_on)
        *dstptr++ = '"';

        *dstptr++ = '`';
        *dstptr++ = (highlight_on) ? 'o' : 'x';
        if (!highlight_on)
        *dstptr++ = 34;
      }
      else
      *dstptr++ = *srcptr;
      srcptr++;
    }

    /* just in case some dummy didn't end his quote */
    if (highlight_on) {
      *dstptr++ = '`';
      *dstptr++ = 'x';
      *dstptr++ = '"'; /* add an end quote for the dummy */
    }
    bool needsname = FALSE;
    if ((argument[0] == '"' || isupper(argument[0]))) {
      needsname = TRUE;
    }
    if (is_ghost(ch) && type == EMOTE_PRIVATE)
    needsname = FALSE;
    if (is_gm(ch) || goddreamer(ch))
    needsname = FALSE;
    if (type == EMOTE_ILLUSION)
    needsname = FALSE;
    if (higher_power(ch) && type == EMOTE_PRIVATE)
    needsname = FALSE;
    if (higher_power(ch) && type == EMOTE_EMIT)
    needsname = FALSE;

    *dstptr = 0;
    //    free_string(argument);
    argument = str_dup(tmp);
    newstring[0] = '\0';

    if (is_gm(ch) || goddreamer(ch) || (is_ghost(ch) && type == EMOTE_PRIVATE && ch->possessing == to) || (is_dreaming(ch) && ch->pcdata->dream_controller == 1)) {
    }
    else {
      if (selfnamed == FALSE && needsname == TRUE && type != EMOTE_ILLUSION && !goddreamer(ch) && type != EMOTE_EMIT) {
        send_to_char("Did you mean to include @me in that emote?\n\r", ch);
        return;
      }
    }

    if (in_fight(ch) && !is_undead(ch))
    fight_talk(ch, argument);

    if (type != EMOTE_ILLUSION && !is_dreaming(ch) && type != EMOTE_INTERNAL && type != EMOTE_PRIVATE && type != EMOTE_ATTEMPT) {
      if (number_percent() % 8 == 0 && ch->pcdata->victimize_char_select > 0 && !IS_FLAG(ch->comm, COMM_PRIVATE) && ch->pcdata->victimize_char_point != NULL
      && (ch->pcdata->victimize_char_point->in_room == ch->in_room || ch->pcdata->victimize_char_point->pcdata->dream_room ==ch->pcdata->dream_room)) {
        remnant_dream(ch, victim, type, argument, possessive, selfnamed);
      }

      emotespy(ch, victim, type, argument, possessive, selfnamed);
    }
    log_message(ch, type, argument, possessive, selfnamed, target);

    for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      bool mentioned = FALSE;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
      
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;

        if (!same_player(ch, to) && to->pcdata->account != NULL && IS_FLAG(to->pcdata->account->flags, ACCOUNT_SHADOWBAN))
        continue;

        if (!same_player(ch, to) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
        continue;

        if (((!is_gm(to) && !higher_power(to)) || !IS_FLAG(to->act, PLR_SPYING) || !can_spy(to, ch)) && ch->in_room != to->in_room && !is_dreaming(ch) && type != EMOTE_PRIVATE && type != EMOTE_ATTEMPT) {
          if (!in_fight(ch)) {
            if (!can_see_char_distance(to, ch, DISTANCE_NEAR) && (!is_gm(ch) || !battleground(ch->in_room) || !battleground(to->in_room))) {
              if (can_hear(to, ch, VOLUME_NORMAL) && type != EMOTE_ATTEMPT && type != EMOTE_PRIVATE && type != EMOTE_INTERNAL && type != EMOTE_SUBTLE && type != EMOTE_ILLUSION && !IS_FLAG(ch->comm, COMM_GAG) && !is_gm(to)) {
                emote_speech(ch, to, argument);
                if (!is_gm(to) || to->in_room == ch->in_room)
                seeingpop++;
              }
              else if (can_hear(to, ch, VOLUME_QUIET) && type != EMOTE_INTERNAL && type != EMOTE_ILLUSION && !IS_FLAG(ch->comm, COMM_GAG) && !is_gm(to)) {
                emote_speech(ch, to, argument);
                if (!is_gm(to) || to->in_room == ch->in_room)
                seeingpop++;
              }
              continue;
            }
          }
          else {
            if (!can_see_char_distance(to, ch, DISTANCE_MEDIUM) && ch->chattacking != to && (!is_gm(ch) || !battleground(ch->in_room) || !battleground(to->in_room))) {
              if (can_hear(to, ch, VOLUME_NORMAL) && type != EMOTE_ATTEMPT && type != EMOTE_PRIVATE && type != EMOTE_SUBTLE && type != EMOTE_INTERNAL && type != EMOTE_ILLUSION && !IS_FLAG(ch->comm, COMM_GAG) && !is_gm(to)) {
                emote_speech(ch, to, argument);
                if (!is_gm(to) || to->in_room == ch->in_room)
                seeingpop++;
              }
              else if (can_hear(to, ch, VOLUME_QUIET) && type != EMOTE_INTERNAL && type != EMOTE_ILLUSION && !IS_FLAG(ch->comm, COMM_GAG) && !is_gm(to)) {
                emote_speech(ch, to, argument);
                if (!is_gm(to) || to->in_room == ch->in_room)
                seeingpop++;
              }

              continue;
            }
          }

          if (is_dreaming(ch) && !is_dreaming(to))
          continue;

          if (!IS_IMMORTAL(ch) && !can_see(ch, to) && type != EMOTE_PRIVATE)
          continue;
        }
        if (!can_see_emote(ch, to, target, type))
        continue;

        if ((is_gm(to) || higher_power(to)) && type != EMOTE_INTERNAL && ch->in_room != to->in_room && !is_dreaming(ch) && !battleground(ch->in_room) && (!can_spy(to, ch) || !IS_FLAG(to->act, PLR_SPYING)))
        continue;
      
        if ((is_gm(to) || higher_power(to)) && type == EMOTE_INTERNAL && ch->in_room != to->in_room && !is_dreaming(ch) && (get_gmtrust(to, ch) < 3 || !IS_FLAG(to->act, PLR_SPYING)))
        continue;

        if (!is_gm(to) && !higher_power(to) && !is_dreaming(ch)) {
          if (type == EMOTE_SUBTLE && (ch->in_room == to->in_room && !same_place(ch, to)))
          continue;
        
          if (type == EMOTE_SUBTLE && !can_see_char_distance(to, ch, DISTANCE_NEAR))
          continue;

          if (type == EMOTE_PRIVATE && to != target && to != ch)
          continue;
        
          if (type == EMOTE_ILLUSION && to != target && to != ch)
          continue;
        
          if (type == EMOTE_INTERNAL && to != ch)
          continue;

          if (battleground(ch->in_room) && combat_distance(ch, to, FALSE) > 100 && !is_gm(ch) && !is_gm(to))
          continue;

          if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->comm, COMM_MANDRAKE) && !IS_FLAG(to->act, PLR_SHROUD) && !guestmonster(ch))
          continue;
        }

        bool verbal = FALSE;
        if (first == TRUE) {
          free_string(argy);
          first = FALSE;
        }
        argy = str_dup(argument);
        newstring[0] = 0;
        bool hearing = FALSE;
        if (is_dreaming(ch))
        hearing = TRUE;
        else if (type == EMOTE_SUBTLE)
        hearing = can_hear(to, ch, VOLUME_QUIET);
        else
        hearing = can_hear(to, ch, VOLUME_NORMAL);
        while (*argy && *argy != '\0' && safe_strlen(argy) > 0) {
          memset(buf, 0, MSL);
          argy = one_argument_true(argy, buf);
          memset(word, 0, MSL);
          memset(gagword, 0, MSL);
          if (buf[0] == '"' || verbal == TRUE) {

            verbal = TRUE;
            for (i = 0; (buf[i] != '"' || i == 0) && buf[i] != '\0'; i++) {
              if (buf[i] == '"' && i > 0) {
                verbal = FALSE;
              }
              sprintf(blah, "%c", buf[i]);
              strcat(word, blah);

              if ((isalpha(buf[i]) || buf[i] == '.' || buf[i] == '!' || buf[i] == '?') && (i == 0 || buf[i - 1] != '`')) {
                if (buf[i] == 'o' || buf[i] == 'O' || buf[i] == '.' || buf[i] == '!' || buf[i] == '?' || buf[i] == 'e' || buf[i] == 'E')
                sprintf(blah, "%c", buf[i]);
                else if (isupper(buf[i]))
                sprintf(blah, "%c", toupper(gagletter()));
                else
                sprintf(blah, "%c", gagletter());
                strcat(gagword, blah);
              }
              if (verbal == FALSE)
              strcat(gagword, "`x");
            }
            if (buf[i] == '"' && i > 0)
            verbal = FALSE;

            if (hearing && type != EMOTE_ILLUSION) {
              strcat(newstring, mangle_word(ch, to, ch->pcdata->speaking, word));
            }
            else if (buf[0] == '"') {
              strcat(newstring, "\"");
              if (IS_FLAG(ch->comm, COMM_GAG))
              strcat(newstring, gagword);
              else
              strcat(newstring, "...");
            }
            else {
              if (IS_FLAG(ch->comm, COMM_GAG))
              strcat(newstring, gagword);
              else
              strcat(newstring, "...");
            }
            if (verbal == FALSE)
            strcat(newstring, "`x\"");
            strcat(newstring, " ");
          }
          else if (buf[0] == '@') {
            for (i = 1; isalpha(buf[i]) && buf[i] != ' ' && buf[i] != '\0'; i++) {
              sprintf(blah, "%c", buf[i]);
              strcat(word, blah);
            }
            if (!str_cmp(word, "me") || !str_cmp(word, "self")) {
              selfnamed = TRUE;
              if (ch != to) {
                if (argument[0] == '@' && argument[1] == 'm' && argument[2] == 'e') {
                  strcat(newstring, PERS_2(ch, to));
                }
                else {
                  strcat(newstring, PERS_3(ch, to));
                }
                if (buf[i] == '.')
                strcat(newstring, ".");
                if (buf[i] == ',')
                strcat(newstring, ",");
                if (buf[i] == '\'')
                strcat(newstring, "'");
                if (buf[i] == '"')
                strcat(newstring, "\"");
                strcat(newstring, " ");
              }
              else {
                strcat(newstring, "`W");
                strcat(newstring, ch->name);
                strcat(newstring, "`x");
                if (tmp[0] != '.')
                //              if(isalpha(tmp[0]))
                //              {
                if (buf[i] == '.')
                strcat(newstring, ".");
                if (buf[i] == ',')
                strcat(newstring, ",");
                if (buf[i] == '\'')
                strcat(newstring, "'");
                if (buf[i] == '"')
                strcat(newstring, "\"");

                strcat(newstring, " ");
                //              }
              }
            }
            else if (!str_cmp(word, "my") || !str_cmp(word, "mine")) {
              selfnamed = TRUE;
              if (ch != to) {
                if (argument[0] == '@' && argument[1] == 'm' && argument[2] == 'y') {
                  strcat(newstring, PERS_2(ch, to));
                }
                else {
                  strcat(newstring, PERS_3(ch, to));
                }
                strcat(newstring, "'s");
                if (buf[i] == '.')
                strcat(newstring, ".");
                if (buf[i] == ',')
                strcat(newstring, ",");
                if (buf[i] == '\'')
                strcat(newstring, "'");
                if (buf[i] == '"')
                strcat(newstring, "\"");

                strcat(newstring, " ");
              }
              else {
                strcat(newstring, "`W");
                strcat(newstring, ch->name);
                strcat(newstring, "'s");
                strcat(newstring, "`x");
                if (buf[i] == '.')
                strcat(newstring, ".");
                if (buf[i] == ',')
                strcat(newstring, ",");
                if (buf[i] == '\'')
                strcat(newstring, "'");
                if (buf[i] == '"')
                strcat(newstring, "\"");

                strcat(newstring, " ");
              }
            }
            else if (!str_cmp(word, "line") || !str_cmp(word, "newline")) {
              strcat(newstring, " \n\r");
            }
            else if ((victim = get_char_vision(ch, NULL, word)) != NULL) {
              if (type != EMOTE_PRIVATE && type != EMOTE_ILLUSION && !is_dreaming(ch) && to == victim && type != EMOTE_INTERNAL && type != EMOTE_ATTEMPT) {
                public_attention(ch, victim);
                give_attention(ch, victim);
                attencount++;
              }
              if(victim != NULL && victim == to)
              mentioned = TRUE;
              if (ispunct(buf[i]) && buf[i + 1] == 's') {
                if (to != victim) {
                  strcat(newstring, PERS_3(victim, to));
                  strcat(newstring, "'s");
                  if (buf[i + 2] == '.')
                  strcat(newstring, ".");
                  if (buf[i + 2] == ',')
                  strcat(newstring, ",");
                  if (buf[i + 2] == '\'')
                  strcat(newstring, "'");
                  if (buf[i + 2] == '"')
                  strcat(newstring, "\"");

                  strcat(newstring, " ");
                }
                else {
                  mentioned = TRUE;
                  strcat(newstring, "`W");
                  strcat(newstring, victim->name);
                  strcat(newstring, "'s");
                  strcat(newstring, "`x");
                  if (buf[i + 2] == '.')
                  strcat(newstring, ".");
                  if (buf[i + 2] == ',')
                  strcat(newstring, ",");
                  if (buf[i + 2] == '\'')
                  strcat(newstring, "'");
                  if (buf[i + 2] == '"')
                  strcat(newstring, "\"");

                  strcat(newstring, " ");

                  move_closer(ch, victim, 1);
                }
              }
              else {
                if (to != victim) {
                  strcat(newstring, PERS_3(victim, to));
                  if (buf[i] == '.')
                  strcat(newstring, ".");
                  if (buf[i] == ',')
                  strcat(newstring, ",");
                  if (buf[i] == '\'')
                  strcat(newstring, "'");
                  if (buf[i] == '"')
                  strcat(newstring, "\"");
                  strcat(newstring, " ");
                }
                else {
                  mentioned = TRUE;
                  strcat(newstring, "`W");
                  strcat(newstring, victim->name);
                  strcat(newstring, "`x");
                  if (buf[i] == '.')
                  strcat(newstring, ".");
                  if (buf[i] == ',')
                  strcat(newstring, ",");
                  if (buf[i] == '\'')
                  strcat(newstring, "'");
                  if (buf[i] == '"')
                  strcat(newstring, "\"");
                  strcat(newstring, " ");
                  move_closer(ch, victim, 1);
                }
              }
            }
            else {
              printf_to_char(ch, "%s doesn't seem to be here.\n\r", word);
              return;

              strcat(newstring, "someone");
              if (buf[i] == '.')
              strcat(newstring, ".");
              if (buf[i] == ',')
              strcat(newstring, ",");
              if (buf[i] == '\'')
              strcat(newstring, "'");
              if (buf[i] == '"')
              strcat(newstring, "\"");

              strcat(newstring, " ");
            }
          }
          else {
            strcat(newstring, buf);
            strcat(newstring, " ");
          }
        }

        if (!is_possessing(ch) || ch->possessing != to) {
          // check to see if emote contains speech - Discordance
          // This needs a single quote check that doesn't interfere with
          // possessives and contractions if (is_ghost(ch) && (strstr(newstring, // "'") || strstr(newstring, "\""))) {
          if (is_ghost(ch) && type != EMOTE_INTERNAL && strstr(newstring, "\"")) {
            if (is_manifesting(ch)) {
              if (deplete_ghostpool(ch, GHOST_SPEECH) == FALSE) {
                send_to_char("You don't have the will to manifest your voice any more today.\n\r", ch);
                if (type == EMOTE_PRIVATE)
                refill_ghostpool(ch, GHOST_PRIVATE);
                else
                refill_ghostpool(ch, GHOST_EMOTE);
                return;
              }
            }
          }
        }

        if(mentioned == FALSE)
        {
          if (type == EMOTE_EMOTE && !higher_power(ch) && !is_gm(ch) && crowded_room(ch->in_room) && (ch->in_room != to->in_room || !same_place(ch, to)))
          continue;

          if (type == EMOTE_EMOTE && !higher_power(ch) && !is_gm(ch) && crowded_room(to->in_room) && (ch->in_room != to->in_room || !same_place(ch, to)))
          continue;
        }

        char prefixstring[MSL];
        if (is_gm(ch) || goddreamer(ch) || (is_ghost(ch) && type == EMOTE_PRIVATE && ch->possessing == to) || (is_dreaming(ch) && ch->pcdata->dream_controller == 1)) {
          sprintf(prefixstring, "%s", newstring);
        }
        else {
          if (selfnamed == FALSE && needsname == TRUE && type != EMOTE_ILLUSION && !goddreamer(ch) && type != EMOTE_EMIT) {
            send_to_char("Did you mean to include @me in that emote?\n\r", ch);
            return;
          }

          if (type != EMOTE_PRIVATE && type != EMOTE_ATTEMPT && type != EMOTE_ILLUSION && type != EMOTE_INTERNAL && !IS_FLAG(ch->comm, COMM_GAG) && !is_mute(ch) && ch->pcdata->sleeping == 0) {
            if (strcasestr(argument, "Revocacionem protectione sanctuarium") !=
                NULL && str_cmp(ch->pcdata->understanding, "None")) {
              free_string(ch->pcdata->understanding);
              ch->pcdata->understanding = str_dup("None");
              nounderglow(ch);
            }
            else if (strcasestr(argument, "Repeto praesidio sanctuarium") !=
                NULL && str_cmp(ch->pcdata->understanding, "All")) {
              free_string(ch->pcdata->understanding);
              ch->pcdata->understanding = str_dup("All");
            }
            else if (strcasestr(argument, "Nullius Minuat sanctuarium") !=
                NULL && str_cmp(ch->pcdata->understanding, "Limited")) {
              free_string(ch->pcdata->understanding);
              ch->pcdata->understanding = str_dup("Limited");
              limunderglow(ch);
            }
          }

          if (type == EMOTE_PRIVATE)
          sprintf(prefixstring, "(Privately) %s", newstring);
          else if (type == EMOTE_SUBTLE)
          sprintf(prefixstring, "(Subtly) %s", newstring);
          else if (type == EMOTE_ANNOUNCE)
          sprintf(prefixstring, "(Announcing) %s", newstring);
          else if (type == EMOTE_INTERNAL)
          sprintf(prefixstring, "(Internally) %s", newstring);
          else
          sprintf(prefixstring, "%s", newstring);

          if(ch->in_room == to->in_room && !same_place(ch, to) && strlen(ch->pcdata->place) > 2)
          {
            char ptmp[MSL];
            sprintf(ptmp, "[%s] %s", ch->pcdata->place, prefixstring);
            sprintf(prefixstring, "%s", ptmp);
          }

        }
        if (is_gm(ch) || (type == EMOTE_PRIVATE && ch->possessing == to))
        sprintf(tmp, "%s", prefixstring);
        else if (is_ghost(ch) && !is_manifesting(ch) && ch->possessing != to && get_skill(to, SKILL_CLAIRVOYANCE) < 2 && !is_gm(to) && !is_ghost(to))
        continue;
        else if (is_griefer(ch) && !is_griefer(to) && !IS_IMMORTAL(to))
        continue;
        else if (is_griefer(to) && !is_griefer(ch))
        continue;
        else if (is_dreaming(ch) && ch->pcdata->dream_controller == 1)
        sprintf(tmp, "%s", prefixstring);
        else if (goddreamer(ch))
        sprintf(tmp, "%s", prefixstring);
        else if (type == EMOTE_PRIVATE && higher_power(ch))
        sprintf(tmp, "%s", prefixstring);
        else if (type == EMOTE_EMIT)
        sprintf(tmp, "%s", prefixstring);
        else if (selfnamed == TRUE)
        sprintf(tmp, "%s", prefixstring);
        else if (type == EMOTE_ILLUSION)
        sprintf(tmp, "%s", prefixstring);
        else if (!can_shroud(ch) && IS_FLAG(to->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD))
        sprintf(tmp, "%s does something.", PERS(ch, to));
        else if (possessive == TRUE)
        sprintf(tmp, "%s%s", PERS(ch, to), prefixstring);
        else
        sprintf(tmp, "%s %s", PERS(ch, to), prefixstring);

        // CLEAR!
        if (ch->desc != NULL && ch->desc->mxp == TRUE) {
          strcat(tmp, "`q");
        }

        if(ch->pcdata->is_attempt_emote == TRUE && target != NULL && type == EMOTE_EMOTE && pc_pop(ch->in_room) > 2)
        {
          sprintf(tmp2, "%s (Checked with %s)", tmp, PERS(target, to));
          sprintf(tmp, "%s", tmp2);
        }
        if (hastalk == TRUE && !IS_NPC(ch) && !IS_NPC(to) && str_cmp(ch->pcdata->speaking, to->pcdata->speaking) && (lang_level(to, ch->pcdata->speaking) > 0 || is_gm(to))) {
          sprintf(tmp2, "%s (%s)", tmp, ch->pcdata->speaking);
          sprintf(tmp, "%s", tmp2);
        }
        if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
          if (is_dreaming(ch) && ch->pcdata->dream_room > 0)
          sprintf(buf, "Dream: %s`x: %s", get_room_index(ch->pcdata->dream_room)->name, tmp);
          else if (is_dreaming(ch))
          sprintf(buf, "Dreamworld`x: %s", tmp);
          else if (ch->in_room == to->in_room)
          sprintf(buf, "%s", tmp);
          else
          sprintf(buf, "%s`x: %s", ch->in_room->name, tmp);
          page_to_char(wrap_string(buf, get_wordwrap(to)), to);
          continue;
        }
        else if (to->in_room == ch->in_room || is_dreaming(ch) || type == EMOTE_PRIVATE || type == EMOTE_INTERNAL)
        page_to_char(wrap_string(tmp, get_wordwrap(to)), to);
        else {
          sprintf(tmp2, "[%s]%s", room_distance(to, ch->in_room), tmp);
          page_to_char(wrap_string(tmp2, get_wordwrap(to)), to);
        }

        if(type != EMOTE_ATTEMPT)
        {
          triggercheck(to, message);
          persuade_check(ch, to, message);
          if (!is_gm(to) || to->in_room == ch->in_room)
          seeingpop++;
        }
      }
    }
    if (is_abom(ch) && !is_masked(ch) && !is_cloaked(ch) && !in_fight(ch) && public_room(ch->in_room) && !room_hostile(ch->in_room) && abom_pop(ch->in_room) < room_pop(ch->in_room) && number_percent() % 4 == 0)
    ch->pcdata->lastnormal = current_time;

    if (is_dreaming(ch) && ch->pcdata->dream_room > 0)
    sprintf(buf, "Dream[%s](%s): %s", get_room_index(ch->pcdata->dream_room)->name, dream_name(ch), inter);
    else
    sprintf(buf, "[%s](%s): %s", from_color(ch->in_room->name), NAME(ch), inter);

    if (type == EMOTE_EMOTE)
    process_emoteattack(ch);

    if (seeingpop > 0 && type != EMOTE_INTERNAL && type != EMOTE_ATTEMPT) {
      if (IS_NPC(ch) || ch->pcdata->institute_action == 0 || seeingpop > 2 || type == EMOTE_EMOTE) {
        if (type == EMOTE_PRIVATE)
        rpreward(ch, argument, TRUE, seeingpop);
        else
        rpreward(ch, argument, FALSE, seeingpop);
        if (attencount == 0)
        give_room_attention(ch);
      }
    }

    if (hastalk == TRUE && ch->wounds >= 3 && !is_undead(ch) && type != EMOTE_INTERNAL) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 15);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MUTE;
      affect_to_char(ch, &af);
    }
    if (type == EMOTE_EMOTE || type == EMOTE_SUBTLE)
    victimize_emote_process(ch);

    if(seeingpop == 0 && type != EMOTE_INTERNAL && type != EMOTE_ATTEMPT)
    send_to_char("Seems like nobody saw that emote.\n\r", ch);

  }

  _DOFUN(do_announce) {
    ch->pcdata->is_attempt_emote = FALSE;
    process_emote(ch, NULL, EMOTE_ANNOUNCE, argument);
    return;
  }

  _DOFUN(do_emote) {
    ch->pcdata->is_attempt_emote = FALSE;
    process_emote(ch, NULL, EMOTE_EMOTE, argument);
    return;
  }

  _DOFUN(do_emit) {
    ch->pcdata->is_attempt_emote = FALSE;
    if (is_gm(ch) || (higher_power(ch))) {
      process_emote(ch, NULL, EMOTE_EMIT, argument);
      return;
    }
    FANTASY_TYPE *fant = in_fantasy(ch);
    if (fant == NULL || fant->emit == 0) {
      send_to_char("You can only emit from inside an emit enabled dream world.\n\r", ch);
      return;
    }
    process_emote(ch, NULL, EMOTE_EMIT, argument);
    return;
  }

  _DOFUN(do_internal) {
    ch->pcdata->is_attempt_emote = FALSE;
    process_emote(ch, NULL, EMOTE_INTERNAL, argument);
    return;
  }

  _DOFUN(do_bug) {
    append_file(ch, BUG_FILE, argument);
    send_to_char("Bug logged.\n\r", ch);
    return;
  }

  _DOFUN(do_typo) {
    append_file(ch, TYPO_FILE, argument);
    send_to_char("Typo logged.\n\r", ch);
    return;
  }

  bool can_logoff(CHAR_DATA *ch) {
    if (ch->in_room == NULL) {
      return TRUE;
    }
    else if (is_gm(ch)) {
      return TRUE;
    }
    else if (IS_FLAG(ch->act, PLR_SINSPIRIT)) {
      return TRUE;
    }
    else if (ch->race == RACE_FANTASY) {
      return TRUE;
    }
    else if (ch->played < 10 * 3600) {
      return TRUE;
    }
    else if (pc_in_lair(ch)) {
      return TRUE;
    }
    else if (in_hospital(ch) && ch->wounds > 1) {
      return TRUE;
    }
    else if (higher_power(ch)) {
      return TRUE;
    }
    else if (time_info.bloodstorm == 1) {
      return TRUE;
    }
    else if (guestmonster(ch)) {
      return TRUE;
    }

    if(IS_SET(ch->in_room->room_flags, ROOM_BEDROOM))
    return TRUE;


    if (ch->in_room->sector_type == SECT_HOUSE) {
      return TRUE;
    }
    else if (ch->in_room->sector_type == SECT_BASEMENT) {
      return TRUE;
    }
    else if (is_water(ch->in_room) && !water_breathe(ch)) {
      return FALSE;
    }
    else if (ch->in_room->sector_type == SECT_PARK || ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_BEACH) {
      if (!is_animal(ch) && IS_SET(ch->in_room->room_flags, ROOM_CAMPSITE)) {
        return TRUE;
      }
      else if (is_animal(ch) && IS_SET(ch->in_room->room_flags, ROOM_ANIMALHOME)) {
        return TRUE;
      }
    }
    else if (ch->in_room->vnum == 98) {
      return TRUE;
    }
    else if (in_prop(ch) && prop_from_room(ch->in_room)->type == PROP_HOUSE) {
      return TRUE;
    }

    return FALSE;
  }

  _DOFUN(do_qui) {
    send_to_char("If you want to QUIT, you have to spell it out.\n\r", ch);
    return;
  }

  _DOFUN(do_quit) {
    if (IS_FLAG(ch->act, PLR_GM) && !IS_IMMORTAL(ch)) {
      do_function(ch, &do_storyrunner, "");
      return;
    }

    if (ch->desc && ch->desc->connected == CON_CREATION) {
      char strsave[MAX_INPUT_LENGTH];
      DESCRIPTOR_DATA *d;

      sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
      save_char_obj(ch, FALSE, FALSE);
      d = ch->desc;
      extract_char(ch, TRUE);
      if (d != NULL)
      d->connected = CON_QUITTING;
      unlink(strsave);
      return;
    }
    
    if (in_fight(ch)) {
      send_to_char("No way! You are fighting.\n\r", ch);
      return;
    }
    
    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0) {
      send_to_char("You're in no condition to do that.\n\r", ch);
      return;
    }

    if (!can_logoff(ch)) {
      send_to_char("You can't sleep here, you should try getting a house, renting a room at the Antlers or sleeping at the Longhouse.\n\r", ch);
      return;
    }

    real_quit(ch);
  }

  void deduct_karma(CHAR_DATA *ch, int amount) {
    if (ch->pcdata->account != NULL)
    ch->pcdata->account->karma -= amount;
    else
    ch->karma -= amount;
  }
  void deduct_rpexp(CHAR_DATA *ch, int amount) {
    if (ch->pcdata->account != NULL)
    ch->pcdata->account->rpxp -= amount;
    else
    ch->rpexp -= amount;
  }

  void spirit_logoff(char *spiritname) {
    CHAR_DATA *original;
    for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;
      original = CH(d);
      if (original == NULL || IS_NPC(original))
      continue;

      if (!str_cmp(original->name, spiritname)) {
        save_account(original->pcdata->account, FALSE);
        DESCRIPTOR_DATA *dill = original->desc;
        if (dill->account == NULL && original->pcdata->account != NULL)
        dill->account = original->pcdata->account;
        free_char(original);
        load_char_obj(dill, spiritname);
        dill->character->desc = dill;
        if (dill->account != NULL) {
          free_string(dill->character->pcdata->account_name);
          dill->character->pcdata->account_name = str_dup(dill->account->name);
        }
        send_to_char("With a strange rushing sensation you return to your body.\n\r", dill->character);
        if (IS_FLAG(dill->character->act, PLR_SINSPIRIT))
        REMOVE_FLAG(dill->character->act, PLR_SINSPIRIT);
        if (IS_FLAG(dill->character->act, PLR_NOSAVE))
        REMOVE_FLAG(dill->character->act, PLR_NOSAVE);
        return;
      }
    }
  }

  void quit_process(CHAR_DATA *ch) {
    int penalty = 0;
    house_security_update(ch);

    if (!guestmonster(ch) && !is_gm(ch) && !is_ghost(ch)) {
      EXTRA_DESCR_DATA *ed;
      char buf[MSL];
      sprintf(buf, "%s is sleeping here.\n\r", ch->name);
      for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
        if (is_name("!sleepers", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("!sleepers");
        ed->next = ch->in_room->extra_descr;
        ch->in_room->extra_descr = ed;
        free_string(ch->in_room->extra_descr->description);
        ch->in_room->extra_descr->description = str_dup(buf);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(buf);
      }

      if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
      SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
    }

    if (ch->pcdata->patrol_status == PATROL_UNDER_ASSAULT && ch->pcdata->patrol_target != NULL)
    finish_arcane_assault(ch->pcdata->patrol_target, ch);

    if (ch->in_room != NULL && (ch->in_room->vnum == ROOM_PRISON_EAST || ch->in_room->vnum == ROOM_PRISON_WEST)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 6 * 8);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_BADKIDNAPPEE;
      affect_to_char(ch, &af);
    }

    if (ch->in_room != NULL && (ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_ATMOSPHERE)) {
      EXIT_DATA *pexit;
      ROOM_INDEX_DATA *to_room;
      pexit = ch->in_room->exit[DIR_DOWN];
      if (pexit != NULL) {
        to_room = pexit->u1.to_room;
        if (to_room != NULL) {
          act("$n falls out of the room.", ch, NULL, ch, TO_NOTVICT);
          char_from_room(ch);
          char_to_room(ch, to_room);
          do_function(ch, &do_look, "auto");
          if (ch->in_room->sector_type == SECT_WATER) {
            act("$n bellyflops into the water.", ch, NULL, ch, TO_NOTVICT);
            if (ch->desc != NULL)
            send_to_char("You fall into the water.\n\r", ch);
          }
          else {
            act("$n falls into the room.", ch, NULL, ch, TO_NOTVICT);
            if (ch->desc != NULL)
            send_to_char("You fall into the room!\n\r", ch);
          }
        }
      }
    }

    if (IS_FLAG(ch->comm, COMM_AFK))
    ch->logon = current_time;

    if (IS_FLAG(ch->comm, COMM_RUNNING))
    REMOVE_FLAG(ch->comm, COMM_RUNNING);

    if (is_possessed(ch)) {
      CHAR_DATA *temp = get_possesser(ch);
      temp->possessing = NULL;
    }

    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->act, PLR_HIDE);

    if (ch->pcdata->poisontimer > 0)
    poison_char(ch);

    if (ch->pcdata->escape_timer > 120)
    ch->pcdata->escape_timer -= 120;

    if (is_animal(ch) && number_percent() % 2 == 0 && !IS_AFFECTED(ch, AFF_ANIMALMINDED)) {

      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 40);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_ANIMALMINDED;
      affect_to_char(ch, &af);
    }
    free_string(ch->pcdata->chat_history);
    ch->pcdata->chat_history = str_dup("");

    // Checking rpexp gained while in private mode - Discordance
    if (ch->privaterpexp > 0) {
      penalty = ch->privaterpexp * 11 / 10;
      sprintf(log_buf, "%s logged off with %d RPXP gained while in private mode and was fined %d\n\r", ch->name, ch->privaterpexp, penalty);
      wiznet(log_buf, NULL, NULL, WIZ_LOGINS, 0, 0);
      log_string(log_buf);
      deduct_rpexp(ch, (int)penalty);
      if (ch->privaterpexp > 50)
      ch->pcdata->attract[ATTRACT_PROM] += UMIN(150, ch->privaterpexp / 3);

      if (ch->privaterpexp > 30 && safe_strlen(ch->pcdata->privatepartner) > 2 && str_cmp(ch->pcdata->privatepartner, ch->pcdata->last_sexed[0])) {
        ch->pcdata->week_tracker[TRACK_SEX]++;
        ch->pcdata->life_tracker[TRACK_SEX]++;

        free_string(ch->pcdata->last_sexed[2]);
        ch->pcdata->last_sexed[2] = str_dup(ch->pcdata->last_sexed[1]);
        free_string(ch->pcdata->last_sexed[1]);
        ch->pcdata->last_sexed[1] = str_dup(ch->pcdata->last_sexed[0]);
        free_string(ch->pcdata->last_sexed[0]);
        ch->pcdata->last_sexed[0] = str_dup(ch->pcdata->privatepartner);
      }

      ch->privaterpexp = 0;
      if (IS_FLAG(ch->act, PLR_AFTERGLOW)) {
        REMOVE_FLAG(ch->act, PLR_AFTERGLOW);
      }
    }

    // this is for karma
    if (ch->privatekarma > 0) {
      penalty = ch->privatekarma * 11 / 10;
      sprintf(log_buf, "%s logged off with %d karma gained while in private mode and was fined %d\n\r", ch->name, ch->privatekarma, penalty);
      wiznet(log_buf, NULL, NULL, WIZ_LOGINS, 0, 0);
      log_string(log_buf);
      deduct_karma(ch, (int)penalty);
      ch->privatekarma = 0;
      if (IS_FLAG(ch->act, PLR_AFTERGLOW)) {
        REMOVE_FLAG(ch->act, PLR_AFTERGLOW);
      }
    }
    if (in_prop(ch) != NULL) {
      in_prop(ch)->logoffs++;
      to_sleepers(ch, in_prop(ch));
    }
    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->act, PLR_HIDE);

    ch->privaterpexp = 0;
    ch->privatekarma = 0;

  }

  void real_quit(CHAR_DATA *ch) {
    DESCRIPTOR_DATA *d;
    char name[MSL];
    char buf[MSL];
    if (IS_NPC(ch))
    return;

    quit_process(ch);

    bool spirit = FALSE;
    char *spiritname;

    if (IS_FLAG(ch->act, PLR_SINSPIRIT)) {
      spirit = TRUE;
      spiritname = str_dup(ch->name);
      ch->pcdata->spirit_type = 0;
      REMOVE_FLAG(ch->act, PLR_SINSPIRIT);
    }

    if (IS_FLAG(ch->comm, COMM_AFK)) {
      REMOVE_FLAG(ch->comm, COMM_AFK);
    }


    if (ch->desc != NULL)
    do_quotes(ch);

    if (institute_room(ch->in_room) && !in_hospital(ch)) {
      if (!college_student(ch, FALSE) && !clinic_patient(ch) && (ch->race != RACE_FACULTY
            || (!clinic_staff(ch, FALSE) && !college_staff(ch, FALSE)))) {
        if (!IS_IMMORTAL(ch)) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(11597));
        }
      }
    }

    if (physical_dreamer(ch)) {
      wake_char(ch);
      char_from_room(ch);
      char_to_room(ch, get_room_index(11597));
    }

    if (!guestmonster(ch)) {
      if (IS_FLAG(ch->act, PLR_SHROUD)) {
        wake_char(ch);
      }
    }

    // This keeps characters from seeing people go to sleep that they normally
    // wouldn't see - Discordance
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *dill = *it;

      if (dill->character != NULL && dill->connected == CON_PLAYING) {
        CHAR_DATA *to = dill->character;

        if (IS_NPC(to))
        continue;
        if (to == ch)
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;

        if (to->pcdata->tracing == ch)
        to->pcdata->tracing = NULL;

        if (can_see_char_distance(to, ch, DISTANCE_NEAR)) {
          if (can_see(to, ch)) {
            sprintf(buf, "%s goes to sleep.", PERS(ch, to));
            act(buf, to, NULL, NULL, TO_CHAR);
          }
        }
      }
    }
    ch->pcdata->quit_room = ch->in_room->vnum;

    sprintf(log_buf, "%s has quit.", ch->name);
    log_string(log_buf);
    if (!IS_FLAG(ch->act, PLR_SPYSHIELD))
    wiznet("$N has left the game.", ch, NULL, WIZ_LOGINS, 0, get_trust(ch));

    if (spirit == FALSE)
    save_char_obj(ch, FALSE, FALSE);

    char strsave[MSL];
    bool gmchar = FALSE;
    if (IS_FLAG(ch->act, PLR_GM)) {
      gmchar = TRUE;
      sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
    }

    sprintf(name, "%s", ch->name);
    d = ch->desc;
    if (ch->pcdata->rohyptimer > 0) {
      ch->pcdata->sleeping = 200;
    }
    else {
      extract_char(ch, TRUE);
    }
    if (d != NULL)
    close_desc(d);

    if (gmchar == TRUE)
    unlink(strsave);

    if (spirit == FALSE) {
      /* toast evil cheating bastards */
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end();) {
        CHAR_DATA *tch;
        DESCRIPTOR_DATA *f = *it;
        ++it;

        tch = CH(f);
        if (tch && !str_cmp(tch->name, name) && !gmchar && !is_gm(tch) && !IS_FLAG(tch->act, PLR_SINSPIRIT)) {
          log_string("Closing duplicate descriptor.");
          if (tch->pcdata->rohyptimer == 0)
          extract_char(tch, TRUE);
          close_desc(f);
        }
      }
    }

    if (spirit == TRUE) {
      spirit_logoff(spiritname);
    }

    return;
  }

  _DOFUN(do_save) {
    if (IS_NPC(ch))
    return;

    ch->pcdata->confirm_delete = FALSE;
    save_char_obj(ch, FALSE, FALSE);

    send_to_char("Saving...\n\r", ch);
    return;
  }

  _DOFUN(do_autosave) {

    if (ch != NULL && !IS_NPC(ch))
    save_char_obj(ch, FALSE, FALSE);
    return;
  }

  _DOFUN(do_shadow) {
    /* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Follow whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    if (in_fight(ch) || is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (victim == ch) {
      if (ch->master == NULL) {
        send_to_char("You already follow yourself.\n\r", ch);
        return;
      }
      stop_follower(ch);
      return;
    }

    REMOVE_FLAG(ch->act, PLR_NOFOLLOW);

    if (ch->master != NULL)
    stop_follower(ch);

    if (!IS_FLAG(ch->act, PLR_SHADOW))
    SET_FLAG(ch->act, PLR_SHADOW);

    add_follower(ch, victim);
    return;
  }

  _DOFUN(do_follow) {
    /* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (move_helpless(ch))
    return;

    if (arg[0] == '\0') {
      send_to_char("Follow whom?\n\r", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim == ch) {
      if (ch->master == NULL) {
        send_to_char("You already follow yourself.\n\r", ch);
        return;
      }
      stop_follower(ch);
      return;
    }

    REMOVE_FLAG(ch->act, PLR_NOFOLLOW);

    if (ch->master != NULL)
    stop_follower(ch);

    add_follower(ch, victim);
    return;
  }

  void add_follower(CHAR_DATA *ch, CHAR_DATA *master) {
    if (ch->master != NULL) {
      bug("Add_follower: non-null master.", 0);
      return;
    }

    ch->master = master;

    if (!IS_FLAG(ch->act, PLR_SHADOW))
    act("$n now follows you.", ch, NULL, master, TO_VICT);
    else if (get_skill(ch, SKILL_STEALTH) < get_skill(master, SKILL_PERCEPTION))
    act("$n now follows you steathily", ch, NULL, master, TO_VICT);

    act("You now follow $N.", ch, NULL, master, TO_CHAR);

    return;
  }

  void stop_follower(CHAR_DATA *ch) {
    if (ch->master == NULL) {
      bug("Stop_follower: null master.", 0);
      return;
    }

    printf_to_char(ch, "You stop following %s.\n\r", PERS(ch->master, ch));
    if (!IS_FLAG(ch->act, PLR_SHADOW))
    printf_to_char(ch->master, "%s stops following you.\n\r", PERS(ch, ch->master));
    else if (get_skill(ch, SKILL_STEALTH) <
        get_skill(ch->master, SKILL_PERCEPTION))
    printf_to_char(ch->master, "%s stops stealthily following you.\n\r", PERS(ch, ch->master));

    if (IS_FLAG(ch->act, PLR_SHADOW))
    REMOVE_FLAG(ch->act, PLR_SHADOW);

    ch->master = NULL;
    return;
  }

  void die_follower(CHAR_DATA *ch) {
    CHAR_DATA *fch;

    if (ch->master != NULL) {
      stop_follower(ch);
    }

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      fch = *it;
      if (fch->master == ch)
      stop_follower(fch);
    }

    return;
  }

  /*
* It is very important that this be an equivalence relation:
* (1) A ~ A
* (2) if A ~ B then B ~ A
* (3) if A ~ B  and B ~ C, then A ~ C
*/
  bool is_same_group(CHAR_DATA *ach, CHAR_DATA *bch) {
    if (ach == NULL || bch == NULL)
    return FALSE;

    if (ach->master != NULL)
    ach = ach->master;
    if (bch->master != NULL)
    bch = bch->master;
    return ach == bch;
  }

  /*
* Color setting and unsetting, way cool, Lope Oct '94
*/
  _DOFUN(do_color) {
    char arg[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
      send_to_char("Color is not on, your not a character!\n\r", ch);
      return;
    }

    if (ch->pcdata->ci_editing == 1 && ch->pcdata->account->colours > 0 && !is_gm(ch) && !higher_power(ch)) {
      custom_to_obj(ch, ch->pcdata->customizing, TRUE);
      return;
    }

    argument = one_argument(argument, arg);
    if (!*arg) {
      if (!IS_FLAG(ch->act, PLR_COLOR)) {
        if (ch->desc != NULL) {
          ch->desc->ansi = TRUE;
        }
        SET_FLAG(ch->act, PLR_COLOR);
        write_to_buffer(ch->desc, (const char *)will_mxp_str, 0); // casting is naughty - Discordance
        send_to_char("Color is now enabled!\n\rPlease refer to help color for further details.\n\r", ch);

      }
      else {
        send_to_char("Color is now disabled.\n\r", ch);
        REMOVE_FLAG(ch->act, PLR_COLOR);
        if (ch->desc != NULL) {
          ch->desc->ansi = FALSE;
          ch->desc->mxp = FALSE;
        }
      }
      return;
    }

    if (!str_cmp(arg, "default")) {
      default_color(ch);
      send_to_char("Color setting set to default values.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "all")) {
      all_color(ch, argument);
      return;
    }

    /*
* Yes, I know this is ugly and unnessessary repetition, but its old
* and I can't justify the time to make it pretty. -Lope
*/
    if (!str_cmp(arg, "text")) {
      ALTER_COLOR(text)
    }
    else if (!str_cmp(arg, "ooc"))
    ALTER_COLOR(ooc)
    else if (!str_cmp(arg, "hint"))
    ALTER_COLOR(hint)
    else if (!str_cmp(arg, "minion"))
    ALTER_COLOR(minioncolor)
    else if (!str_cmp(arg, "pray"))
    ALTER_COLOR(pray)

    else if (!str_cmp(arg, "hero"))
    ALTER_COLOR(hero)

    else if (!str_cmp(arg, "osay"))
    ALTER_COLOR(osay)
    else if (!str_cmp(arg, "immortal"))
    ALTER_COLOR(immortal)
    else if (!str_cmp(arg, "auction")) {
      ALTER_COLOR(auction)
    }
    else if (!str_cmp(arg, "ic")) {
      ALTER_COLOR(gossip)
    }
    else if (!str_cmp(arg, "say")) {
      ALTER_COLOR(say);
    }
    else if (!str_cmp(arg, "tells")) {
      ALTER_COLOR(tells)
    }
    else if (!str_cmp(arg, "implementor")) {
      ALTER_COLOR(implementor)
    }
    else if (!str_cmp(arg, "reply")) {
      ALTER_COLOR(reply)
    }
    else if (!str_cmp(arg, "gtell"))
    ALTER_COLOR(gtell)

    else if (!str_cmp(arg, "wiznet")) {
      ALTER_COLOR(wiznet)
    }
    else if (!str_cmp(arg, "newbie"))
    ALTER_COLOR(newbie)
    else if (!str_cmp(arg, "yells"))
    ALTER_COLOR(yells)
    else if (!str_cmp(arg, "room_exits")) {
      ALTER_COLOR(room_exits)
    }
    else if (!str_cmp(arg, "room_things")) {
      ALTER_COLOR(room_things)
    }
    else if (!str_cmp(arg, "prompt")) {
      ALTER_COLOR(prompt)
    }
    else if (!str_cmp(arg, "fight_death")) {
      ALTER_COLOR(fight_death)
    }
    else if (!str_cmp(arg, "fight_yhit")) {
      ALTER_COLOR(fight_yhit)
    }
    else if (!str_cmp(arg, "fight_ohit")) {
      ALTER_COLOR(fight_ohit)
    }
    else if (!str_cmp(arg, "fight_thit")) {
      ALTER_COLOR(fight_thit)
    }
    else if (!str_cmp(arg, "fight_skill")) {
      ALTER_COLOR(fight_skill)
    }
    else {
      send_to_char("Unrecognized Color Parameter Not Set.\n\r", ch);
      return;
    }

    send_to_char("New Color Parameter Set.\n\r", ch);
    return;
  }

  char *makedrunk(char *string, CHAR_DATA *ch) {
    return string;
  }

  _DOFUN(do_spy) {
    if (is_gm(ch) || higher_power(ch)) {
      if (IS_FLAG(ch->act, PLR_SPYING)) {
        REMOVE_FLAG(ch->act, PLR_SPYING);
        send_to_char("You stop being a peeping Tom.\n\r", ch);
      }
      else {
        SET_FLAG(ch->act, PLR_SPYING);
        send_to_char("You become a peeping Tom.`x\n\r", ch);
      }
    }
  }

  _DOFUN(do_mentor) {
    CHAR_DATA *victim;
    CHAR_DATA *to;
    AFFECT_DATA af;

    if (!str_cmp(argument, "off")) {
      if (!IS_FLAG(ch->act, PLR_NOMENTOR))
      SET_FLAG(ch->act, PLR_NOMENTOR);
      send_to_char("You turn mentoring off.\n\r", ch);
      return;
    }
    if (!str_cmp(argument, "on")) {
      if (IS_FLAG(ch->act, PLR_NOMENTOR))
      REMOVE_FLAG(ch->act, PLR_NOMENTOR);
      send_to_char("You turn mentoring on.\n\r", ch);
      return;
    }
    if (!str_cmp(argument, "reject")) {
      send_to_char("You reject the offer of mentorship.\n\r", ch);
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6000;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_NOMENTOR;
      affect_to_char(ch, &af);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;

          if (to == NULL)
          continue;
          if (IS_NPC(to))
          continue;
          if (to == ch)
          continue;
          if (!IS_FLAG(to->comm, COMM_MENTOR))
          continue;

          if (to->pcdata->mentor_of == ch->id) {
            to->pcdata->mentor_timer = 0;
          }
        }
      }
      return;
    }
    if (!str_cmp(argument, "accept")) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (d->connected != CON_PLAYING)
          continue;
          if (to == NULL)
          continue;

          if (IS_NPC(to))
          continue;
          if (to == ch)
          continue;

          if (to->pcdata->mentor_of == ch->id) {
            to->pcdata->mentor_timer = 0;
            free_string(to->pcdata->mentor);
            if (mentoring_eligible_ooc(to)) {
              to->pcdata->mentor = str_dup(ch->name);
              REMOVE_FLAG(ch->comm, COMM_MENTOR);
              printf_to_char(to, "%s has been appointed as your `YOOC`x mentor, use `gmtalk <message>`x to talk oocly with them and read `ghelp mentor`x for more info.\n\r", ch->name);
              printf_to_char(ch, "You become %s's `YOOC`x mentor, this is an important position, ensure you have read `ghelp mentor`x before proceeding.\n\r", to->name);
            }
            else if (mentoring_eligible_ic(to)) {
              to->pcdata->mentor = str_dup(ch->name);
              REMOVE_FLAG(ch->comm, COMM_MENTOR);
            }
            else {
              to->pcdata->mentor = str_dup("");
              continue;
            }

            if (ch->faction != 0 && to->faction != 0) {
              if (get_phone(ch) != NULL) {
                char buf[MSL];
                if (get_phone(to) != NULL) {
                  sprintf(buf, "%s\nBlocked Number: A new recruit has arrived in town, we want you to mentor them. Their name is %s %s, phone number %d.", get_phone(ch)->material, to->name, to->pcdata->last_name, get_phone(to)->value[0]);
                  free_string(get_phone(ch)->material);
                  get_phone(ch)->material = str_dup(buf);
                  sprintf(buf, "%s\nBlocked Number: Your contact in town to help you get set up will be %s %s, you can reach them at %d.", get_phone(to)->material, ch->name, ch->pcdata->last_name, get_phone(ch)->value[0]);
                  free_string(get_phone(to)->material);
                  get_phone(to)->material = str_dup(buf);

                  send_to_char("Your phone beeps.\n\r", ch);
                  send_to_char("Your phone beeps.\n\r", to);
                }
                else {
                  sprintf(
                  buf, "%s\nBlocked Number: A new recruit has arrived in town, we want you to mentor them. Their name is %s %s.", get_phone(ch)->material, to->name, to->pcdata->last_name);
                  free_string(get_phone(ch)->material);
                  get_phone(ch)->material = str_dup(buf);
                  send_to_char("Your phone beeps.\n\r", ch);
                }
              }
              else {
                printf_to_char(to, "%s has been appointed as your `RIC`x mentor, read `ghelp mentor`x for more info.\n\r", ch->name);
                printf_to_char(ch, "You become %s's `RIC`x mentor, this is an important position, ensure you have read `ghelp mentor`x before proceeding.\n\r", to->name);
              }

              give_respect(ch, 2000, "Mentor accept.", to->faction);
            }
          }
        }
      }
      return;
    }
    if (!str_cmp(argument, "list")) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->connected != CON_PLAYING)
        continue;

        victim = CH(d);

        if (victim == NULL)
        continue;

        if (IS_NPC(victim))
        continue;

        if (ch == victim)
        continue;

        if (!str_cmp(victim->name, ch->pcdata->mentor)) {
          printf_to_char(ch, "(Mentor) %s\n\r", victim->name);
        }
        else if (!str_cmp(ch->name, victim->pcdata->mentor)) {
          printf_to_char(ch, "(Student) %s\n\r", victim->name);
        }
        else if (!str_cmp(ch->pcdata->mentor, victim->pcdata->mentor) && safe_strlen(ch->pcdata->mentor) > 1) {
          printf_to_char(ch, "(Student) %s\n\r", victim->name);
        }
      }
    }
    if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (ch->played / 3600 > 50) {
      if (victim->played / 3600 > 50) {
        send_to_char("They already have too many hours to become a mentoree.\n\r", ch);
        return;
      }
      send_to_char("You make your offer.\n\r", ch);
      act("$n would like to become your mentor, a position responsible for helping you OOCly AND ICly with the game, if you'd like them to be your mentor, just type 'mentor $n' you can change who your mentor is at any time.", ch, NULL, victim, TO_VICT);
    }
    else {
      free_string(ch->pcdata->mentor);
      ch->pcdata->mentor = str_dup(victim->name);
      act("You make $N your mentor.", ch, NULL, victim, TO_CHAR);
      act("$n has adopted you as their mentor, if you feel someone else would be better suited to this task, you should let them know and help them find a better mentor via the mtalk command. Remeber mentor is an OOC AND IC position.", ch, NULL, victim, TO_VICT);
    }
  }

  _DOFUN(do_mtalk) {
    CHAR_DATA *victim;

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOCHANNELS)) {
      send_to_char("Your channels have been revoked.\n\r", ch);
      return;
    }
    bool mod_flagged = FALSE;
    mod_flagged = isTextFlagged(argument);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (ch == victim)
      continue;
      if (spammer(ch) && !same_player(ch, victim))
      continue;
      if (victim != NULL && !same_player(ch, victim) && mod_flagged == TRUE)
      continue;

      if (!str_cmp(victim->name, ch->pcdata->mentor)) {
        printf_to_char(victim, "`x[`cMentor`x] %s: `x'`G%s`x'\n\r", ch->name, argument);
      }
      else if (!str_cmp(ch->name, victim->pcdata->mentor)) {
        printf_to_char(victim, "`x[`cMentor`x] %s: `x'`G%s`x'\n\r", ch->name, argument);
      }
      else if (!str_cmp(ch->pcdata->mentor, victim->pcdata->mentor) && safe_strlen(ch->pcdata->mentor) > 1) {
        printf_to_char(victim, "`x[`cMentor`x] %s: `x'`G%s`x'\n\r", ch->name, argument);
      }
    }
    printf_to_char(ch, "`x[`cMentor`x] You: `x'`G%s`x'\n\r", argument);
  }

  _DOFUN(do_subtle) {
    if (in_fight(ch)) {
      send_to_char("Now is not the time for subtlety.\n\r", ch);
      return;
    }
    ch->pcdata->is_attempt_emote = FALSE;

    process_emote(ch, NULL, EMOTE_SUBTLE, argument);
    return;
  }

  _DOFUN(do_private) {
    if (in_fight(ch)) {
      send_to_char("Now is not the time for subtlety.\n\r", ch);
      return;
    }
    ch->pcdata->is_attempt_emote = FALSE;

    CHAR_DATA *target;
    char arg1[MSL];
    char inter[MSL];

    sprintf(inter, "%s", argument);
    argument = one_argument_nouncap(argument, arg1);

    if (argument[0] == '\0') {
      if (IS_FLAG(ch->comm, COMM_PRIVATE)) {
        send_to_char("You are no longer in privacy mode.\n\r", ch);
        REMOVE_FLAG(ch->comm, COMM_PRIVATE);
        if (is_dreaming(ch)) {
          char buf[MSL];
          sprintf(buf, "%s leaves private mode.", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
        }
        else
        act("$n leaves private mode.", ch, NULL, NULL, TO_ROOM);
      }
      else {
        if(ch->pcdata->fixation_timeout > current_time && fixation_happy_room(ch) == FALSE)
        {
          printf_to_char(ch, "You are too distracted with thoughts of %s for any of that.\n\r", ch->pcdata->fixation_name);
          return;
        }
        send_to_char("You are now in privacy mode.\n\r", ch);
        SET_FLAG(ch->comm, COMM_PRIVATE);
        if (is_dreaming(ch)) {
          char buf[MSL];
          sprintf(buf, "%s enters private mode.", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
        }
        else
        act("$n enters private mode.", ch, NULL, NULL, TO_ROOM);
        if(institute_room(ch->in_room))
        {
          send_to_char("The closer you get to a sex act, alone or with someone else the sleepier you feel.\n\r", ch);

        }
      }
      return;
    }

    if (!IS_NPC(ch) && IS_FLAG(ch->comm, COMM_NOEMOTE)) {
      send_to_char("You can't show your emotions.\n\r", ch);
      return;
    }

    if (ch->wounds >= 3 && !is_undead(ch)) {
      send_to_char("You can't move.\n\r", ch);
      return;
    }


    if (is_dreaming(ch) && ch->pcdata->dream_room > 0) {
      if ((target = get_char_dream(ch, arg1)) != NULL && target->pcdata->dream_room == ch->pcdata->dream_room) {
        process_emote(ch, target, EMOTE_PRIVATE, argument);
        return;
      }
    }
    if ((target = get_char_room(ch, NULL, arg1)) == NULL) {
      if ((target = get_char_world(ch, arg1)) == NULL) {
        send_to_char("Private whom?\n\r", ch);
        return;
        strcat(arg1, " ");
        strcat(arg1, argument);
        argument = str_dup(arg1);
        process_emote(ch, NULL, EMOTE_SUBTLE, argument);
        return;
      }
      
      if ((!is_gm(ch) && !higher_power(ch)) || get_gmtrust(ch, target) < 1) {
        send_to_char("Private whom?\n\r", ch);
        return;
      }
    }
    
    if ((strstr(argument, "\"") || strstr(argument, " '")) && !is_ghost(ch) && !is_gm(ch) && !goddreamer(ch) && !higher_power(ch) && !is_gm(target) && !higher_power(target)) {
      send_to_char("Use the whisper command.\n\r", ch);
      return;
    }

    process_emote(ch, target, EMOTE_PRIVATE, argument);
    return;
  }

  void lower_fatigue(CHAR_DATA *ch, int length) {
    if (!other_players(ch))
    return;
    CHAR_DATA *to;
    if (length < number_percent())
    return;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;
        if (ch == to)
        continue;
        if (ch->in_room != to->in_room)
        continue;
        to->pcdata->fatigue -= 3;
      }
    }
  }

  bool holding_phone(CHAR_DATA *ch) {
    OBJ_DATA *phone;
    if ((phone = get_held(ch, ITEM_PHONE)) != NULL && phone->item_type == ITEM_PHONE)
    return TRUE;
    return FALSE;
  }

  // Boolean modeled on holding_phone - Discordance
  bool wearing_phone(CHAR_DATA *ch) {
    OBJ_DATA *phone;
    if ((phone = get_worn(ch, ITEM_PHONE)) != NULL && phone->item_type == ITEM_PHONE)
    return TRUE;
    return FALSE;
  }

  OBJ_DATA *get_phone(CHAR_DATA *ch) {
    OBJ_DATA *phone;

    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1))
    return NULL;

    if ((phone = get_held(ch, ITEM_PHONE)) != NULL && phone->item_type == ITEM_PHONE && !IS_SET(phone->extra_flags, ITEM_OFF)) {
      if (is_name("phone", phone->name))
      return phone;
    }
    for (phone = ch->carrying; phone != NULL; phone = phone->next_content) {
      if (phone->wear_loc != WEAR_NONE || IS_SET(phone->extra_flags, ITEM_WARDROBE))
      continue;
      if (phone->item_type != ITEM_PHONE)
      continue;
      if (IS_SET(phone->extra_flags, ITEM_OFF))
      continue;
      if (!is_name("phone", phone->name))
      continue;
      return phone;
    }
    return NULL;
  }

  OBJ_DATA *find_phone(CHAR_DATA *ch, int number) {
    OBJ_DATA *phone;
    if(ch == NULL || number == NULL)
    return NULL;

    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1))
    return NULL;

    if ((phone = get_held(ch, ITEM_PHONE)) != NULL && phone->item_type == ITEM_PHONE && !IS_SET(phone->extra_flags, ITEM_OFF)) {
      if (is_name("phone", phone->name))
      return phone;
    }
    for (phone = ch->carrying; phone != NULL; phone = phone->next_content) {
      if (phone->wear_loc != WEAR_NONE || IS_SET(phone->extra_flags, ITEM_WARDROBE))
      continue;

      if (phone->item_type != ITEM_PHONE)
      continue;
      if (IS_SET(phone->extra_flags, ITEM_OFF))
      continue;

      if (!is_name("phone", phone->name))
      continue;

      if (phone->value[0] != number)
      continue;

      return phone;
    }
    return NULL;
  }

  _DOFUN(do_speeddial) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char buf[MSL];
    int i, number;

    if (argument[0] == '\0') {
      for (i = 0; i < 50; i++) {
        if (ch->pcdata->speed_numbers[i] == 0)
        sprintf(buf, "\t`W%d`x)`x None.", i + 1);
        else {
          if (visibly_online(ch->pcdata->speed_names[i]))
          sprintf(buf, "\t`W%d`172) %s`x, %d\n\r", i + 1, ch->pcdata->speed_names[i], ch->pcdata->speed_numbers[i]);
          else
          sprintf(buf, "\t`W%d`x) %s, %d\n\r", i + 1, ch->pcdata->speed_names[i], ch->pcdata->speed_numbers[i]);
        }
        send_to_char(buf, ch);
      }
      return;
    }

    // Added some checks to make sure the character is holding or wearing a phone
    // - Discordance I moved these because it's super annoying having to hold your
    // phone just to try and remember if you have someone's number or not.
    if (!holding_phone(ch) && !wearing_phone(ch)) {
      send_to_char("You must be holding a phone first.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);

    i = atoi(arg1);

    if (i < 1 || i > 50) {
      send_to_char("Selection must be between 1 and 50.\n\r", ch);
      return;
    }
    i--;
    number = atoi(arg3);
    if (number < 0 || number > 10000000) {
      send_to_char("Third argument should be a number.\n\r", ch);
      return;
    }

    free_string(ch->pcdata->speed_names[i]);
    ch->pcdata->speed_names[i] = str_dup(arg2);
    ch->pcdata->speed_numbers[i] = number;

    printf_to_char(ch, "Selection %d set to call %s on %d.\n\r", i + 1, ch->pcdata->speed_names[i], ch->pcdata->speed_numbers[i]);
  }

  _DOFUN(do_talk) {
    free_string(ch->pcdata->talk);
    ch->pcdata->talk = str_dup(argument);
    printf_to_char(ch, "New talk string: %s\n\r", ch->pcdata->talk);
  }

  CHAR_DATA *get_gm(ROOM_INDEX_DATA *room, bool permission) {
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != room)
      continue;

      if (!is_gm(victim))
      continue;

      if (permission == TRUE && !IS_FLAG(victim->comm, COMM_RUNNING))
      continue;

      return victim;
    }
    return NULL;
  }

  _DOFUN(do_vote) {
    char buf[MSL];
    if (IS_IMMORTAL(ch)) {
      sprintf(buf, "Story Runner %s is now running the room, those who do not wish to be involved in this RP should leave now.", ch->name);
      act(buf, ch, NULL, NULL, TO_ROOM);
      act("You are now running the room.", ch, NULL, NULL, TO_CHAR);
      if (!IS_FLAG(ch->comm, COMM_RUNNING))
      SET_FLAG(ch->comm, COMM_RUNNING);
      return;
    }

    if (is_gm(ch)) {
      ch->pcdata->vote_timer = -1;
      return;
    }

    if (get_gm(ch->in_room, FALSE) == NULL) {
      send_to_char("There doesn't seem to be an SR here.\n\r", ch);
      return;
    }

    int pop = room_pop(ch->in_room) - 1;

    if (!str_cmp(argument, "yes")) {
      if (get_gm(ch->in_room, FALSE) != NULL) {
        get_gm(ch->in_room, FALSE)->pcdata->yesvotes++;
        send_to_char("You vote yes.\n\r", ch);
        if (get_gm(ch->in_room, FALSE)->pcdata->yesvotes * 2 >= pop)
        get_gm(ch->in_room, FALSE)->pcdata->vote_timer = 1;
      }
    }
    else if (!str_cmp(argument, "no")) {
      if (get_gm(ch->in_room, FALSE) != NULL) {
        get_gm(ch->in_room, FALSE)->pcdata->novotes++;
        send_to_char("You vote no.\n\r", ch);

        if (get_gm(ch->in_room, FALSE)->pcdata->novotes * 2 > pop)
        get_gm(ch->in_room, FALSE)->pcdata->vote_timer = 1;
      }
    }
  }
  int get_gmtrust(CHAR_DATA *ch, CHAR_DATA *victim) {
    int i;
    int trust = 0;

    if (ch == NULL || victim == NULL || IS_NPC(ch) || IS_NPC(victim))
    return 0;

    if (same_player(ch, victim) && !IS_IMMORTAL(ch)) {
      char buf[MSL];
      sprintf(buf, "%s tried to do something with their own char %s", ch->name, victim->name);
      wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);
    }

    if (story_on(victim) && IS_IMMORTAL(ch))
    return 3;

    if (story_on(victim) && IS_FLAG(ch->comm, COMM_DEPUTY))
    return 3;

    if (higher_power(ch) && ch->pcdata->hp_sex > current_time - (3600 * 24 * 7))
    return 0;

    if (event_cleanse == 1 && higher_power(ch) && in_world(victim) == WORLD_EARTH)
    return 0;

    for (i = 0; i < MAX_TRUSTS; i++) {
      if (!str_cmp(ch->name, victim->pcdata->trust_names[i]))
      trust = victim->pcdata->trust_levels[i];
    }
    if (trust < 1 && ch->in_room == victim->in_room && IS_FLAG(ch->comm, COMM_RUNNING))
    trust = 1;
    if (trust < 2 && ch->in_room == victim->in_room && encounter_room(ch->in_room) && ch->in_room->area->vnum == 12 && IS_FLAG(ch->comm, COMM_RUNNING))
    trust = 2;
    else if (trust < 3 && ch->in_room == victim->in_room && !encounter_room(ch->in_room) && ch->in_room->area->vnum == 12 && IS_FLAG(ch->comm, COMM_RUNNING))
    trust = 3;

    if (trust < 3 && higher_power(ch) && victim->pcdata->in_domain > 0) {
      DOMAIN_TYPE *vdomain = vnum_domain(victim->pcdata->in_domain);
      DOMAIN_TYPE *cdomain = my_domain(ch);
      if (vdomain != NULL && cdomain != NULL && cdomain->vnum == vdomain->vnum) {
        if (victim->pcdata->domain_timer >= 90)
        return UMAX(trust, 3);
        if (victim->pcdata->domain_timer >= 45)
        return UMAX(trust, 2);
        if (victim->pcdata->domain_timer >= 5)
        return UMAX(trust, 1);
      }
    }

    if(trust == 0 && higher_power(ch))
    {
      if(victim->fcult != 0)
      {
        FACTION_TYPE *fcult = clan_lookup(victim->fcult);
        if(fcult != NULL && !str_cmp(fcult->eidilon, ch->name))
        trust = 1;
      }
      if(victim->fsect != 0)
      {
        FACTION_TYPE *fcult = clan_lookup(victim->fsect);
        if(fcult != NULL && !str_cmp(fcult->eidilon, ch->name))
        trust = 1;
      }
    }

    if (trust < 3 && ch->in_room == victim->in_room && in_world(ch) != WORLD_EARTH && IS_FLAG(ch->comm, COMM_RUNNING) && is_wildsapproved(ch)) {
      if (ch->pcdata->sr_nomove > 90)
      trust = UMAX(trust, 3);
      if (ch->pcdata->sr_nomove > 45)
      trust = UMAX(trust, 2);
    }
    if (trust < 1 && story_on(victim) && crisis_runner(ch))
    return 1;

    return trust;
  }

  int get_snooptrust(CHAR_DATA *ch, CHAR_DATA *victim) {
    int i;
    int trust = 0;

    if (ch == NULL || victim == NULL || IS_NPC(ch) || IS_NPC(victim))
    return 0;

    if (story_on(victim) && IS_IMMORTAL(ch))
    return 3;

    if (story_on(victim) && IS_FLAG(ch->comm, COMM_DEPUTY))
    return 3;

    if (!IS_IMMORTAL(ch) && (is_helpless(victim) || clinic_patient(victim)) && !is_dreaming(victim))
    return 0;

    if (event_cleanse == 1 && higher_power(ch) && in_world(victim) == WORLD_EARTH)
    return 0;

    for (i = 0; i < MAX_TRUSTS; i++) {
      if (!str_cmp(ch->name, victim->pcdata->trust_names[i]))
      trust = victim->pcdata->trust_levels[i];
    }

    if (is_dreaming(victim) && higher_power(ch)) {
      FANTASY_TYPE *fant;
      if ((fant = in_fantasy(victim)) != NULL) {
        if (!str_cmp(ch->name, fant->author) && !IS_FLAG(victim->comm, COMM_PRIVATE))
        return 3;
      }
    }
    if(trust == 0 && higher_power(ch))
    {
      if(victim->fcult != 0)
      {
        FACTION_TYPE *fcult = clan_lookup(victim->fcult);
        if(fcult != NULL && !str_cmp(fcult->eidilon, ch->name))
        trust = 1;
      }
      if(victim->fsect != 0)
      {
        FACTION_TYPE *fcult = clan_lookup(victim->fsect);
        if(fcult != NULL && !str_cmp(fcult->eidilon, ch->name))
        trust = 1;
      }
    }

    if (trust < 3 && higher_power(ch) && victim->pcdata->in_domain > 0 && victim->pcdata->domain_timer >= 5) {
      DOMAIN_TYPE *vdomain = vnum_domain(victim->pcdata->in_domain);
      DOMAIN_TYPE *cdomain = my_domain(ch);
      if (vdomain != NULL && cdomain != NULL && cdomain->vnum == vdomain->vnum) {
        return 3;
      }
    }
    if (trust < 1 && ch->in_room == victim->in_room && IS_FLAG(ch->comm, COMM_RUNNING))
    trust = 1;

    if(trust >= 0 && higher_power(ch))
    {
      if(light_level(victim->in_room) < 75)
      trust++;
      if(light_level(victim->in_room) < 0)
      trust++;
    }

    return trust;
  }

  int get_probetrust(CHAR_DATA *ch, CHAR_DATA *victim) {
    int i;
    int trust = 0;

    if (ch == NULL || victim == NULL || IS_NPC(ch) || IS_NPC(victim))
    return 0;

    if (story_on(victim) && IS_IMMORTAL(ch))
    return 3;

    for (i = 0; i < MAX_TRUSTS; i++) {
      if (!str_cmp(ch->name, victim->pcdata->trust_names[i]))
      trust = victim->pcdata->trust_levels[i];
    }

    return trust;
  }

  int get_guesttrust(CHAR_DATA *ch, CHAR_DATA *guest) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    if (ch == guest)
    return 3;

    d.original = NULL;

    if ((victim = get_char_world_pc(guest->pcdata->guest_of)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Guest trust");

      if (!load_char_obj(&d, guest->pcdata->guest_of)) {
        return 0;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(guest->pcdata->guest_of));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return 0;
    }
    int trust = get_gmtrust(victim, ch);

    if (IS_IMMORTAL(victim))
    trust = 3;
    if (!online)
    free_char(victim);

    return trust;
  }

  bool guestplot(CHAR_DATA *guest) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    d.original = NULL;

    if ((victim = get_char_world_pc(guest->pcdata->guest_of)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Guest plot");

      if (!load_char_obj(&d, guest->pcdata->guest_of)) {
        return FALSE;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(guest->pcdata->guest_of));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return 0;
    }
    int val = 0;

    if (has_plot(victim))
    val = 1;
    else
    val = 0;

    if (!online)
    free_char(victim);

    if (val == 0)
    return FALSE;
    else
    return TRUE;
  }

  _DOFUN(do_probe) {
    char arg1[MSL];
    int i;
    argument = one_argument_nouncap(argument, arg1);

    if (is_dreaming(ch)) {
      if (goddreamer(ch)) {
        CHAR_DATA *victim = get_char_dream(ch, arg1);
        if (victim == NULL || in_fantasy(victim) != in_fantasy(ch)) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        FANTASY_TYPE *fant = in_fantasy(ch);
        for (int x = 0; x < 25; x++) {
          if (!str_cmp(fant->participants[x], victim->name)) {
            for (int y = 0; y < 20; y++) {
              if (!str_cmp(argument, fant->stats[y])) {
                printf_to_char(ch, "They have the %s stat at %d.\n\r", fant->stats[y], fant->participant_stats[x][y]);
                return;
              }
            }
            send_to_char("No such stat.\n\r", ch);
            return;
          }
        }
      }
      return;
    }

    if (is_gm(ch) || higher_power(ch)) {
      CHAR_DATA *victim = get_char_world(ch, arg1);
      if (victim == NULL) {
        send_to_char("They aren't here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      if (get_gmtrust(ch, victim) < 1) {
        send_to_char("They aren't here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "experience")) {
        printf_to_char(ch, "Worlds Exp: %d.\n\r", worlds_experience(victim, WORLD_EARTH));
        return;
      }
      for (i = 0; i < DIS_USED; i++) {
        if (!str_cmp(argument, discipline_table[i].name)) {
          printf_to_char(ch, "Their discipline is: %d\n\r", get_disc(victim, discipline_table[i].vnum, FALSE));
          if (get_probetrust(ch, victim) < 2)
          printf_to_char(victim, "%s probes your %s discipline.\n\r", ch->name, argument);
          return;
        }
      }
      for (i = 0; i < SKILL_USED; i++) {
        if (!str_cmp(argument, skill_table[i].name)) {
          if (get_probetrust(ch, victim) < 3 && skilltype(skill_table[i].vnum) == STYPE_ABOMINATION)
          printf_to_char(ch, "Their skill is: 0\n\r");
          else
          printf_to_char(ch, "Their skill is: %d\n\r", get_skill(victim, skill_table[i].vnum));
          if (get_probetrust(ch, victim) < 2)
          printf_to_char(victim, "%s probes your %s skill.\n\r", ch->name, argument);
          stat_explain(ch, victim, skill_table[i].vnum);
          return;
        }
      }
    }
    else {
      CHAR_DATA *victim = get_char_room(ch, NULL, arg1);
      if (victim == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }

      for (i = 0; i < DIS_USED; i++) {
        if (!str_cmp(argument, discipline_table[i].name)) {
          send_to_char("Done.\n\r", ch);
          printf_to_char(victim, "Someone has requested you reveal the level of your %s discipline.\n\r", argument);
          return;
        }
      }
      for (i = 0; i < SKILL_USED; i++) {
        if (!str_cmp(argument, skill_table[i].name)) {
          send_to_char("Done.\n\r", ch);
          printf_to_char(
          victim, "Someone has requested you reveal the level of your %s skill.\n\r", argument);
          return;
        }
      }
    }
  }

  void recruit_encounter_ally(CHAR_DATA *ch) {
    if (ch->pcdata->encounter_sr == NULL)
    return;

    act("$n leaves.", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, ch->pcdata->encounter_sr->in_room);
    send_to_char("You accept the offer.\n\r", ch);
    act("$n arrives.", ch, NULL, NULL, TO_ROOM);
    printf_to_char(ch->pcdata->encounter_sr, "%s joins the encounter.\n\r", ch->name);
    ch->pcdata->encounter_status = ENCOUNTER_ONGOING;
    ch->pcdata->encounter_countdown = 0;
  }

  _DOFUN(do_no) {
    if(ch->pcdata->summary_stage == SUM_STAGE_PENDING_YOU)
    {
      if(ch->pcdata->summary_type == SUMMARY_CONFLICT_DEF)
      {
        CHAR_DATA *target = ch->pcdata->summary_target;
        if(target != NULL)
        {
          ch->pcdata->summary_type = 0;
          target->pcdata->summary_type = 0;
          send_to_char("The summary doesn't happen.\n\r", ch);
          send_to_char("The summary doesn't happen.\n\r", target);
          return;
        }
      }
    }
    CHAR_DATA *to;
    if(ch->pcdata->narrative_query_char != NULL)
    {
      to = ch->pcdata->narrative_query_char;
      send_to_char("Narrative query declined at this time.\n\r", to);
      send_to_char("You decline the narrative query.\n\r", ch);
      ch->pcdata->narrative_query_char = NULL;
      return;
    }

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      to = *it;
      if (to == NULL || to->in_room == NULL || to->in_room != ch->in_room || IS_NPC(to))
      continue;
      if(to->pcdata->attempt_character == ch)
      {
        printf_to_char(to, "(Potential emote declined by %s.)\n\r", PERS(ch, to));
        printf_to_char(ch, "You decline the emote, please remember that your character has no IC knowledge of the potential emote and %s is free to do any other RP instead.\n\r", PERS(to, ch));
        to->pcdata->attempt_character = NULL;
        return;
      }
    }
    send_to_char("Nothing to decline.\n\r", ch);
  }

  // For answering yes to questions
  _DOFUN(do_yes) {
    bool sameplace = FALSE;

    CHAR_DATA *to;

    if(ch->pcdata->narrative_query_char != NULL)
    {
      to = ch->pcdata->narrative_query_char;
      printf_to_char(ch, "%s", narrative_pair_rewards(ch, to));
      printf_to_char(to, "%s", narrative_pair_rewards(ch, to));
      ch->pcdata->narrative_query_char = NULL;
      return;

    }

    for (CharList::iterator it = ch->in_room->people->begin();it != ch->in_room->people->end(); ++it) {
      to = *it;
      if (to == NULL || to->in_room == NULL || to->in_room != ch->in_room || IS_NPC(to))
      continue;
      if(to->pcdata->attempt_character == ch) {
        to->pcdata->is_attempt_emote = TRUE;
        process_emote(to, ch, EMOTE_EMOTE, to->pcdata->attempt_emote);
        to->pcdata->attempt_character = NULL;
        return;
      }
    }

    if(ch->pcdata->summary_stage == SUM_STAGE_PENDING_YOU)
    {
      if(ch->pcdata->summary_type == SUMMARY_CONFLICT_DEF || ch->pcdata->summary_type == SUMMARY_DEFEATED_DEF || ch->pcdata->summary_type == SUMMARY_DEFEATING_DEF)
      {
        CHAR_DATA *target = ch->pcdata->summary_target;
        if(target != NULL)
        {
          target->pcdata->summary_stage = SUM_STAGE_INTRO;
          ch->pcdata->summary_stage = SUM_STAGE_WAITING;
          printf_to_char(ch, "You accept %s's offer to summarize your conflict.\n\r", PERS(target, ch));
          printf_to_char(target, "%s accepts your offer to summarize their conflict. Use intro (emote text) to write how the assault starts.\n\r", PERS(ch, target));
          return;
        }
      }
      if(ch->pcdata->summary_type == SUMMARY_DEFEATED_INFIGHT)
      {
        CHAR_DATA *target = ch->pcdata->summary_target;
        if(target != NULL)
        {
          target->pcdata->summary_stage = SUM_STAGE_CONTENT;
          ch->pcdata->summary_stage = SUM_STAGE_CONTENT;
          printf_to_char(ch, "You accept %s's offer to summarize your conflict. Use content (emote text) to write what you do in the conflict.\n\r", PERS(target, ch));
          printf_to_char(target, "%s accepts your offer to summarize their conflict. Use content (emote text) to write what you do in the conflict.\n\r", PERS(ch, target));
          return;
        }
      }
      if(ch->pcdata->summary_type == SUMMARY_OTHER)
      {
        CHAR_DATA *target = ch->pcdata->summary_target;
        if(target != NULL)
        {
          target->pcdata->summary_stage = SUM_STAGE_INTRO;
          ch->pcdata->summary_stage = SUM_STAGE_INTRO;
          printf_to_char(ch, "You accept %s's offer to do a summary scene. Use intro (emote text) to write your introduction.\n\r", PERS(target, ch));
          printf_to_char(target, "%s accepts your offer to do a summary scene. Use intro (emote text) to write your introduction.\n\r", PERS(ch, target));
          return;
        }
      }
      if(ch->pcdata->summary_type == SUMMARY_WRAPUP)
      {
        CHAR_DATA *target = ch->pcdata->summary_target;
        if(target != NULL)
        {
          target->pcdata->summary_stage = SUM_STAGE_CONTENT;
          ch->pcdata->summary_stage = SUM_STAGE_CONTENT;
          printf_to_char(ch, "You accept %s's offer to do a summary wrapup. Use content (emote text) to write your summary.\n\r", PERS(target, ch));
          printf_to_char(target, "%s accepts your offer to do a summary wrapup. Use content (emote text) to write your summary.\n\r", PERS(ch, target));
          return;
        }
      }
    }

    if (ch->pcdata->encounter_countdown > 0 && ch->pcdata->encounter_sr != NULL && ch->pcdata->encounter_status == ENCOUNTER_PENDING && !is_gm(ch)) {
      have_encounter(ch->pcdata->encounter_sr, ch, ch->pcdata->encounter_number);
      return;
    }
    if (ch->pcdata->encounter_countdown > 0 && ch->pcdata->encounter_sr != NULL && ch->pcdata->encounter_status == ENCOUNTER_PENDINGALLY && !is_gm(ch)) {
      recruit_encounter_ally(ch);
      return;
    }

    if (ch->pcdata->sexing != NULL) {
      if ((ch->in_room->vnum != ch->pcdata->sexing->in_room->vnum)) {
        if (is_dreaming(ch) && is_dreaming(ch->pcdata->sexing)) {
          if (ch->pcdata->sexing->pcdata->dream_room != 0 && ch->pcdata->dream_room != ch->pcdata->sexing->pcdata->dream_room) {
            send_to_char("They're not here.\n\r", ch);
          }
          else {sameplace = TRUE;}
        }
        else {send_to_char("They're not here.\n\r", ch);}
      }
      else {sameplace = TRUE;}

      if (sameplace == TRUE) {
        if (IS_AFFECTED(ch, AFF_SEEKINGSEX) == TRUE) {
          if (ch->pcdata->sexing != NULL) {
            have_sex(ch->pcdata->sexing, ch, ch->pcdata->sex_risk, ch->pcdata->sex_type, ch);
            sex_cleanup(ch);
            return;
          }
        }
      }
      else {
        sex_cleanup(ch);
        return;
      }
    }
    return;
  }

  _DOFUN(do_attempt) {
    static char attempt[MAX_STRING_LENGTH];
    attempt[0] = '\0';
    static char stat1[MAX_STRING_LENGTH];
    stat1[0] = '\0';
    static char stat2[MAX_STRING_LENGTH];
    stat2[0] = '\0';
    static char stat3[MAX_STRING_LENGTH];
    stat3[0] = '\0';

    char arg1[MSL];
    int input = 0;
    int level1 = 0, level2 = 0, level3 = 0;
    int i = 0;
    argument = one_argument_nouncap(argument, arg1);

    if (get_gm(ch->in_room, FALSE) == NULL || strcasestr(argument, "with") == NULL) {
      CHAR_DATA *target;
      if(strlen(argument) < 2)
      {
        send_to_char("There is no SR here, if you are trying to do an attempt emote you need to type attempt (character name) (emote text)\n\r", ch);
        return;
      }
      if ((target = get_char_room(ch, NULL, arg1)) == NULL)
      {
        send_to_char("There is no SR here, if you are trying to do an attempt emote you need to type attempt (character name) (emote text)\n\r", ch);
        return;
      }
      free_string(ch->pcdata->attempt_emote);
      ch->pcdata->attempt_emote = str_dup(argument);
      ch->pcdata->attempt_character = target;
      printf_to_char(target, "[%s is potentially going to do the following emote, type 'yes' if you would like it to go through, 'no' if not.]\n\r", PERS(ch, target));
      printf_to_char(ch, "[Potential emote sent to %s.]\n\r", PERS(target, ch));
      process_emote(ch, target, EMOTE_ATTEMPT, argument);
      return;

    }

    for (; safe_strlen(arg1) > 0 && arg1 != '\0';) {
      if (input == 0) {
        if (str_cmp(arg1, "with")) {
          strcat(attempt, arg1);
          strcat(attempt, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 1) {
        if (str_cmp(arg1, "and")) {
          strcat(stat1, arg1);
          strcat(stat1, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 2) {
        if (str_cmp(arg1, "and")) {
          strcat(stat2, arg1);
          strcat(stat2, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 3) {
        if (str_cmp(arg1, "and")) {
          strcat(stat3, arg1);
          strcat(stat3, " ");
        }
        else {
          input++;
        }
      }
      argument = one_argument_nouncap(argument, arg1);
    }

    i = safe_strlen(stat1);
    stat1[i - 1] = '\0';
    i = safe_strlen(stat2);
    stat2[i - 1] = '\0';
    i = safe_strlen(stat3);
    stat3[i - 1] = '\0';

    for (i = 0; i < DIS_USED; i++) {
      if (!str_cmp(stat1, discipline_table[i].name)) {
        level1 = ch->disciplines[discipline_table[i].vnum];
      }
      if (!str_cmp(stat2, discipline_table[i].name)) {
        level2 = ch->disciplines[discipline_table[i].vnum];
      }
      if (!str_cmp(stat3, discipline_table[i].name)) {
        level3 = ch->disciplines[discipline_table[i].vnum];
      }
    }
    for (i = 0; i < SKILL_USED; i++) {
      if (!str_cmp(stat1, skill_table[i].name)) {
        level1 = ch->skills[skill_table[i].vnum];
      }
      if (!str_cmp(stat2, skill_table[i].name)) {
        level2 = ch->skills[skill_table[i].vnum];
      }
      if (!str_cmp(stat3, skill_table[i].name)) {
        level3 = ch->skills[skill_table[i].vnum];
      }
    }

    if (input == 1) {
      printf_to_char(ch, "You attempt %swith %s(%d)\n\r", attempt, stat1, level1);
      if (get_gm(ch->in_room, FALSE) != NULL) {
        printf_to_char(get_gm(ch->in_room, FALSE), "%s attempts %swith %s(%d)\n\r", ch->name, attempt, stat1, level1);
      }
    }
    else if (input == 2) {
      printf_to_char(ch, "You attempt %swith %s(%d) and %s(%d)\n\r", attempt, stat1, level1, stat2, level2);
      if (get_gm(ch->in_room, FALSE) != NULL) {
        printf_to_char(get_gm(ch->in_room, FALSE), "%s attempts %swith %s(%d) and %s(%d)\n\r", ch->name, attempt, stat1, level1, stat2, level2);
      }
    }
    else if (input == 3) {
      printf_to_char(ch, "You attempt %swith %s(%d), %s(%d) and %s(%d)\n\r", attempt, stat1, level1, stat2, level2, stat3, level3);
      if (get_gm(ch->in_room, FALSE) != NULL) {
        printf_to_char(get_gm(ch->in_room, FALSE), "%s attempts %swith %s(%d), %s(%d) and %s(%d)\n\r", ch->name, attempt, stat1, level1, stat2, level2, stat3, level3);
      }
    }
  }

  int get_monsterlevel(CHAR_DATA *ch) {
    int pop = BASE_MONSTER;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_IMMORTAL(victim))
      continue;

      if (IS_FLAG(victim->act, PLR_GM))
      continue;

      if (ch == victim)
      continue;
    
      if (victim->in_room == ch->in_room) {
        pop += BASE_MONSTER * get_gmtrust(ch, victim);
      }
    }

    if (senior_gm(ch))
    pop *= 2;
  
    if (IS_IMMORTAL(ch))
    return 100000;
  
    return pop;
  }

  int get_summon_x(ROOM_INDEX_DATA *room, char *argument) {
    if (!str_cmp(argument, "north")) {
      return get_mob_x(DIR_NORTH, room);
    }
    else if (!str_cmp(argument, "south")) {
      return get_mob_x(DIR_SOUTH, room);
    }
    else if (!str_cmp(argument, "east")) {
      return get_mob_x(DIR_EAST, room);
    }
    else if (!str_cmp(argument, "west")) {
      return get_mob_x(DIR_WEST, room);
    }
    else if (!str_cmp(argument, "southeast")) {
      return get_mob_x(DIR_SOUTHEAST, room);
    }
    else if (!str_cmp(argument, "southwest")) {
      return get_mob_x(DIR_SOUTHWEST, room);
    }
    else if (!str_cmp(argument, "northeast")) {
      return get_mob_x(DIR_NORTHEAST, room);
    }
    else if (!str_cmp(argument, "northwest")) {
      return get_mob_x(DIR_NORTHWEST, room);
    }
    else {
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      if (is_number(arg)) {
        return atoi(arg);
      }
      return 0;
    }
  }
  
  int get_summon_y(ROOM_INDEX_DATA *room, char *argument) {
    if (!str_cmp(argument, "north")) {
      return get_mob_y(DIR_NORTH, room);
    }
    else if (!str_cmp(argument, "south")) {
      return get_mob_y(DIR_SOUTH, room);
    }
    else if (!str_cmp(argument, "east")) {
      return get_mob_y(DIR_EAST, room);
    }
    else if (!str_cmp(argument, "west")) {
      return get_mob_y(DIR_WEST, room);
    }
    else if (!str_cmp(argument, "southeast")) {
      return get_mob_y(DIR_SOUTHEAST, room);
    }
    else if (!str_cmp(argument, "southwest")) {
      return get_mob_y(DIR_SOUTHWEST, room);
    }
    else if (!str_cmp(argument, "northeast")) {
      return get_mob_y(DIR_NORTHEAST, room);
    }
    else if (!str_cmp(argument, "northwest")) {
      return get_mob_y(DIR_NORTHWEST, room);
    }
    else {
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      if (is_number(argument)) {
        return atoi(argument);
      }
      return 0;
    }
  }

  _DOFUN(do_monster) {
    int i;
    char arg1[MSL];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    argument = one_argument_nouncap(argument, arg1);

    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    
    if (battleground(ch->in_room))
    return;

    if (!str_cmp(arg1, "list")) {
      for (i = 0; i < 5; i++) {
        printf_to_char(ch, "[%d] %s\n\r", i, ch->pcdata->monster_names[0][i]);
      }
    }
    else if (!str_cmp(arg1, "info")) {
      i = atoi(argument);
      if (i < 0 || i > 4) {
        send_to_char("Number must be between 0 and 4.\n\r", ch);
        return;
      }
      printf_to_char(
      ch, "[%d] %s\nShort: %s\nDesc: %s\n%s: %d\n%s: %d\n%s: %d\n\r", i, ch->pcdata->monster_names[0][i], ch->pcdata->monster_names[1][i], ch->pcdata->monster_names[2][i], get_disc_string(ch->pcdata->monster_discs[0][i]), ch->pcdata->monster_discs[1][i], get_disc_string(ch->pcdata->monster_discs[2][i]), ch->pcdata->monster_discs[3][i], get_disc_string(ch->pcdata->monster_discs[4][i]), ch->pcdata->monster_discs[5][i]);
    }
    else if (!str_cmp(arg1, "delete")) {
      i = atoi(argument);
      if (i < 0 || i > 4) {
        send_to_char("Number must be between 0 and 4.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->monster_names[0][i]);
      ch->pcdata->monster_names[0][i] = str_dup("");
      free_string(ch->pcdata->monster_names[1][i]);
      ch->pcdata->monster_names[1][i] = str_dup("");
      free_string(ch->pcdata->monster_names[2][i]);
      ch->pcdata->monster_names[2][i] = str_dup("");
      send_to_char("Monster deleted.\n\r", ch);
    }
    else if (!str_cmp(arg1, "unsummon")) {
      CHAR_DATA *victim;
      if (!IS_FLAG(ch->comm, COMM_RUNNING) && !IS_IMMORTAL(ch)) {
        send_to_char("You're not running the room.\n\r", ch);
        return;
      }
      victim = get_char_room(ch, NULL, argument);

      if (victim == NULL || !IS_NPC(victim) || victim->controled_by != ch) {
        send_to_char("That's not your monster.\n\r", ch);
        return;
      }
      if (victim->fighting == TRUE) {
        ROOM_INDEX_DATA *room = victim->in_room;
        extract_char(victim, TRUE);
        if (fight_over(room))
        end_fight(room);
      }
    }
    else if (!str_cmp(arg1, "summon")) {
      if (!IS_FLAG(ch->comm, COMM_RUNNING) && !IS_IMMORTAL(ch)) {
        send_to_char("You're not running the room.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);

      i = atoi(arg2);
      if (i < 0 || i > 4) {
        send_to_char("Number must be between 0 and 4.\n\r", ch);
        return;
      }

      int val = get_monsterlevel(ch);
      if (ch->pcdata->monster_discs[1][i] + ch->pcdata->monster_discs[3][i] +
          ch->pcdata->monster_discs[5][i] + ch->pcdata->currentmonster >
          val) {
        send_to_char("You already have used too many powerful monsters to summon that.\n\r", ch);
        return;
      }
      
      // pMobIndex = get_mob_index(MONSTER_TEMPLATE);
      pMobIndex = get_mob_index(11);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 10;
      mob->controled_by = ch;
      free_string(mob->short_descr);
      mob->short_descr = str_dup(ch->pcdata->monster_names[1][i]);
      free_string(mob->name);
      mob->name = str_dup(ch->pcdata->monster_names[0][i]);
      free_string(mob->long_descr);
      mob->long_descr = str_dup(ch->pcdata->monster_names[1][i]);
      free_string(mob->description);
      mob->description = str_dup(ch->pcdata->monster_names[2][i]);

      for (int i = 0; i < MAX_DIS; i++)
      mob->disciplines[i] = 0;

      mob->disciplines[ch->pcdata->monster_discs[0][i]] = ch->pcdata->monster_discs[1][i];
      mob->disciplines[ch->pcdata->monster_discs[2][i]] = ch->pcdata->monster_discs[3][i];
      mob->disciplines[ch->pcdata->monster_discs[4][i]] = ch->pcdata->monster_discs[5][i];

      ch->pcdata->currentmonster += ch->pcdata->monster_discs[1][i];
      ch->pcdata->currentmonster += ch->pcdata->monster_discs[3][i];
      ch->pcdata->currentmonster += ch->pcdata->monster_discs[5][i];

      mob->hit = max_hp(mob);

      if (IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);

      if (ch->fighting == TRUE) {
        mob->fighting = TRUE;
        mob->attacking = 1;
        mob->attack_timer = FIGHT_WAIT;
        mob->move_timer = FIGHT_WAIT;
      }
      mob->x = get_summon_x(ch->in_room, argument);
      mob->y = get_summon_y(ch->in_room, argument);
      if (mob->x == 0 && mob->y == 0) {
        mob->x = ch->x;
        mob->y = ch->y;
      }

      send_to_char("Monster summoned.\n\r", ch);
    }
    else if (!str_cmp(arg1, "attack")) {
      if (!IS_FLAG(ch->comm, COMM_RUNNING) && !IS_IMMORTAL(ch)) {
        send_to_char("You're not running the room.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);

      i = atoi(arg2);
      if (i < 0 || i > 4) {
        send_to_char("Number must be between 0 and 4.\n\r", ch);
        return;
      }

      int val = get_monsterlevel(ch);
      if (ch->pcdata->monster_discs[1][i] + ch->pcdata->monster_discs[3][i]
      +   ch->pcdata->monster_discs[5][i] + ch->pcdata->currentmonster > val)
      {
        send_to_char("You already have used too many powerful monsters to summon that.\n\r", ch);
        return;
      }
      
      pMobIndex = get_mob_index(MONSTER_TEMPLATE);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 50;
      mob->controled_by = ch;
      free_string(mob->short_descr);
      mob->short_descr = str_dup(ch->pcdata->monster_names[1][i]);
      free_string(mob->name);
      mob->name = str_dup(ch->pcdata->monster_names[0][i]);
      free_string(mob->long_descr);
      mob->long_descr = str_dup(ch->pcdata->monster_names[1][i]);
      free_string(mob->description);
      mob->description = str_dup(ch->pcdata->monster_names[2][i]);

      mob->disciplines[ch->pcdata->monster_discs[0][i]] =
      ch->pcdata->monster_discs[1][i];
      mob->disciplines[ch->pcdata->monster_discs[2][i]] =
      ch->pcdata->monster_discs[3][i];
      mob->disciplines[ch->pcdata->monster_discs[4][i]] =
      ch->pcdata->monster_discs[5][i];

      ch->pcdata->currentmonster += ch->pcdata->monster_discs[1][i];
      ch->pcdata->currentmonster += ch->pcdata->monster_discs[3][i];
      ch->pcdata->currentmonster += ch->pcdata->monster_discs[5][i];

      mob->hit = max_hp(mob);

      if (IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);

      if (ch->fighting == FALSE) {
        mob->fighting = TRUE;
        mob->attacking = 1;
        mob->attack_timer = FIGHT_WAIT;
        mob->move_timer = FIGHT_WAIT;
        ch->fight_fast = FALSE;
        next_attacker(ch, TRUE);
      }
      else {
        mob->fighting = TRUE;
        mob->attacking = 1;
        mob->attack_timer = FIGHT_WAIT;
        mob->move_timer = FIGHT_WAIT;
      }

      mob->x = get_summon_x(ch->in_room, argument);
      mob->y = get_summon_y(ch->in_room, argument);
      if (mob->x == 0 && mob->y == 0) {
        mob->x = ch->x;
        mob->y = ch->y;
      }

      send_to_char("Monster summoned.\n\r", ch);
    }
    else
    send_to_char("Syntax: Monster list, monster info (number), monster delete (number), monster summon (number), monster unsummon (name)\n\r", ch);
  }

  _DOFUN(do_stopfight) {
    if (battleground(ch->in_room))
    return;
  
    if (!is_gm(ch) || (!IS_FLAG(ch->comm, COMM_RUNNING) && !IS_IMMORTAL(ch))) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }

    if (!room_fight(ch->in_room, FALSE, FALSE, TRUE)) {
      send_to_char("There's no fight here.\n\r", ch);
      return;
    }
    end_fight(ch->in_room);
    act("$n stops the fight.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Done.\n\r", ch);
  }

  // level 4 is self lookup, level 10 is clan lookup, 5 is know secrets.
  void lookup_char(CHAR_DATA *ch, CHAR_DATA *victim, int level) {
    if (level < 1)
    return;
    if (ch == NULL || victim == NULL || IS_NPC(ch) || IS_NPC(victim))
    return;

    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    if (level <= 5 && level >= 2) {
      sprintf(buf, "`WArchtype:`x %s`x", race_table[victim->race].who_name);
      strcat(string, buf);
    }
    
    if (level >= 3 && level <= 5) {
      sprintf(buf, "\t\t`WModifier:`x %s`x\n\r", modifier_table[victim->modifier].name);
      strcat(string, buf);
    }
    else {
      sprintf(buf, "\n\r");
      strcat(string, buf);
    }
    
    if(victim->fcore != 0 && clan_lookup(victim->fcore) != NULL)
    {
      sprintf(buf, " `WFaction:`x %s", clan_lookup(victim->fcore)->name);
      strcat(string, buf);
    }
    strcat(string, "\n\r");

    if (victim->pcdata->job_type_one == JOB_MIGRANT) {
      sprintf(buf, "`cJob`x: New Arrival.\n\r");
      strcat(string, buf);
    }
    else if (victim->race == RACE_DEPUTY) {
      sprintf(buf, "`cJob`x: Deputy.\n\r");
      strcat(string, buf);
    }
    else if (victim->pcdata->job_type_one == JOB_UNEMPLOYED && (victim->pcdata->job_type_two == JOB_UNEMPLOYED || victim->pcdata->job_type_two == JOB_MIGRANT)) {
      sprintf(buf, "`cJob`x: Unemployed.\n\r");
      strcat(string, buf);
    }
    else {
      sprintf(buf, "`cJob`x: %s\n\r", job_name(victim, 1));
      strcat(string, buf);
      if (victim->pcdata->job_type_two != JOB_UNEMPLOYED && victim->pcdata->job_type_two != JOB_MIGRANT) {
        sprintf(buf, "`cJob`x: %s\n\r", job_name(victim, 2));
        strcat(string, buf);
      }
    }
    
    if (get_cabal(victim) != NULL && safe_strlen(get_cabal(victim)->name) > 2) {
      sprintf(buf, "`WGroup:`x %s\n\r", get_cabal(victim)->name);
      strcat(string, buf);
    }
    
    if (victim->pcdata->association != 0) {
      sprintf(buf, "`WAssociation:`x %s\n\r", get_subfac_name(victim->pcdata->association));
      strcat(string, buf);
    }
    
    if (safe_strlen(victim->pcdata->home_territory) > 3) {
      sprintf(buf, "`WAssociated Territory:`x %s\n\r", victim->pcdata->home_territory);
      strcat(string, buf);
    }

    if (IS_FLAG(victim->act, PLR_GUEST)) {
      if (victim->pcdata->guest_type == GUEST_HENCHMAN) {
        sprintf(buf, "They are known to have a connection with %s.\n\r", victim->pcdata->guest_of);
        strcat(string, buf);
      }
      if (victim->pcdata->guest_type == GUEST_ENEMY) {
        sprintf(buf, "They are known to be enemies with %s.\n\r", victim->pcdata->guest_of);
        strcat(string, buf);
      }
    }
    if (victim->modifier == MODIFIER_ILLNESS) {
      sprintf(buf, "They are suffering from some sort of serious illness.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_ADDICTION) {
      sprintf(buf, "They are an addict.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_INSANE) {
      sprintf(buf, "They are suffering from some sort of mental illness.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_CURSED) {
      sprintf(buf, "They are rumored to be cursed.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_CHEMICAL) {
      sprintf(buf, "They are rumored to abuse performance enhancing substances.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_TEMPLE) {
      sprintf(buf, "They are rumored to have undergone augmentation.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_CATATONIC) {
      sprintf(buf, "Their history has large holes in it.\n\r");
      strcat(string, buf);
    }
    if (victim->modifier == MODIFIER_UNLIVING) {
      sprintf(buf, "They are rumored to never activate their body.\n\r");
      strcat(string, buf);
    }
    if (pact_holder(victim) && level >= 3 && level < 10) {
      sprintf(buf, "They have a pact with a demon.\n\n%s\n\r", victim->pcdata->demon_pact);
      strcat(string, buf);
    }
    if (get_skill(victim, SKILL_SILVERVULN) > 0) {
      sprintf(buf, "They are rumored to be vulnerable to silver.\n\r");
      strcat(string, buf);
    }
    if (get_skill(victim, SKILL_GOLDVULN) > 0) {
      sprintf(buf, "They are rumored to be vulnerable to gold.\n\r");
      strcat(string, buf);
    }
    if (get_skill(victim, SKILL_BONEVULN) > 0) {
      sprintf(buf, "They are rumored to be vulnerable to bone.\n\r");
      strcat(string, buf);
    }
    if (get_skill(victim, SKILL_WOODVULN) > 0) {
      sprintf(buf, "They are rumored to be vulnerable to wood.\n\r");
      strcat(string, buf);
    }

    strcat(string, " `WContacts:`x\n\n");

    for (int i = 0; i < MAX_CONTACTS; i++) {
      if (get_skill(victim, contacts_table[i]) > 0) {
        sprintf(buf, "%s\n%s\n\n\r", victim->pcdata->contact_names[i], victim->pcdata->contact_descs[i]);
        strcat(string, buf);
      }
    }

    if (level >= 2 && (is_gm(ch) || higher_power(ch))) {
      sprintf(buf, "\nNarrative Rewards:\n%s\n\r", narrative_show(victim));
      strcat(string, buf);
    }

    sprintf(buf, "\n\n\nCharacter History:\n%s\n\r", victim->pcdata->history);
    strcat(string, buf);

    if(strlen(victim->pcdata->doom_desc) > 3)
    {
      sprintf(buf, "\n\n\nProphecy:\n%s\n\r", victim->pcdata->doom_desc);
      strcat(string, buf);
    }

    for (int i = 0; i < 10; i++) {
      switch (victim->pcdata->relationship_type[i]) {
      case REL_CHILD:
        sprintf(buf, "They are the child of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_PARENT:
        sprintf(buf, "They are the parent of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_WARD:
        sprintf(buf, "They are the ward of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_WEAKNESS:
        sprintf(buf, "%s is their weakness.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_WEAKNESS_OF:
        sprintf(buf, "They are the weakness of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_PRAESTES:
        sprintf(buf, "%s is their praestes.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_PRAESTES_OF:
        sprintf(buf, "They are the praestes of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_GUARDIAN:
        sprintf(buf, "They are the guardian of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_PACTEE:
        sprintf(buf, "They have a pact with %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_PACTER:
        sprintf(buf, "They have a pact with %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_SPOUSE:
        sprintf(buf, "They are the spouse of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_SIBLING:
        sprintf(buf, "They are the sibling of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_PACKMATE:
        sprintf(buf, "They are in a pack with %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_SIRE:
        sprintf(buf, "They are the sire of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_SIRELING:
        sprintf(buf, "%s is their sire.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_TEACHER:
        sprintf(buf, "They are the teacher of %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_STUDENT:
        sprintf(buf, "%s is their teacher.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      case REL_DATING:
        sprintf(buf, "They are dating %s.\n\r", victim->pcdata->relationship[i]);
        strcat(string, buf);
        break;
      }
    }
    sprintf(buf, "\nCharacter Habits:\n\n\r");
    strcat(string, buf);
    
    if (is_vampire(victim) && level >= 2 && level < 10) {
      sprintf(buf, "Feeding: %s\n\r", habit_level(HABIT_FEEDING, victim->pcdata->habit[HABIT_FEEDING]));
      strcat(string, buf);
    }
    
    if (is_werewolf(victim) && level >= 2 && level < 10) {
      sprintf(buf, "Lunacy: %s\n\r", habit_level(HABIT_LUNACY, victim->pcdata->habit[HABIT_LUNACY]));
      strcat(string, buf);
    }
    
    if (is_demonborn(victim) && level >= 2 && level < 10) {
      sprintf(buf, "Sadism: %s\n\r", habit_level(HABIT_SADISM, victim->pcdata->habit[HABIT_SADISM]));
      strcat(string, buf);
    }

    sprintf(buf, "Eating: %s\n\r", habit_level(HABIT_EATING, victim->pcdata->habit[HABIT_EATING]));
    strcat(string, buf);
    sprintf(buf, "Sex: %s\n\r", habit_level(HABIT_SEX, victim->pcdata->habit[HABIT_SEX]));
    strcat(string, buf);
    sprintf(buf, "Protection: %s\n\r", habit_level(HABIT_PROTECTION, victim->pcdata->habit[HABIT_PROTECTION]));
    strcat(string, buf);
    sprintf(buf, "Hormones: %s\n\r", habit_level(HABIT_HORMONES, victim->pcdata->habit[HABIT_HORMONES]));
    strcat(string, buf);
    sprintf(buf, "Smoking: %s\n\r", habit_level(HABIT_SMOKING, victim->pcdata->habit[HABIT_SMOKING]));
    strcat(string, buf);
    sprintf(buf, "Drinking: %s\n\r", habit_level(HABIT_DRINKING, victim->pcdata->habit[HABIT_DRINKING]));
    strcat(string, buf);
    sprintf(buf, "Drug: %s\n\r", habit_level(HABIT_DRUGS, victim->pcdata->habit[HABIT_DRUGS]));
    strcat(string, buf);
    sprintf(buf, "Gym: %s\n\r", habit_level(HABIT_GYM, victim->pcdata->habit[HABIT_GYM]));
    strcat(string, buf);
    sprintf(buf, "Gaming: %s\n\r", habit_level(HABIT_GAMES, victim->pcdata->habit[HABIT_GAMES]));
    strcat(string, buf);
    sprintf(buf, "Comics: %s\n\r", habit_level(HABIT_COMICS, victim->pcdata->habit[HABIT_COMICS]));
    strcat(string, buf);
    sprintf(buf, "Tv: %s\n\r", habit_level(HABIT_TV, victim->pcdata->habit[HABIT_TV]));
    strcat(string, buf);
    sprintf(buf, "Novels: %s\n\r", habit_level(HABIT_NOVELS, victim->pcdata->habit[HABIT_NOVELS]));
    strcat(string, buf);
    sprintf(buf, "Clubbing: %s\n\r", habit_level(HABIT_CLUBBING, victim->pcdata->habit[HABIT_CLUBBING]));
    strcat(string, buf);
    sprintf(buf, "Sports: %s\n\r", habit_level(HABIT_SPORTS, victim->pcdata->habit[HABIT_SPORTS]));
    strcat(string, buf);
    sprintf(buf, "School: %s\n\r", habit_level(HABIT_SCHOOL, victim->pcdata->habit[HABIT_SCHOOL]));
    strcat(string, buf);
    sprintf(buf, "Orientation: %s\n\r", habit_level(HABIT_ORIENTATION, victim->pcdata->habit[HABIT_ORIENTATION]));
    strcat(string, buf);

    if (level >= 2 && level < 10) {
      sprintf(buf, "Self Esteem: %s\n\r", habit_level(HABIT_SELFESTEEM, victim->pcdata->habit[HABIT_SELFESTEEM]));
      strcat(string, buf);
    }

    sprintf(buf, "Concern: %s\n\r", habit_level(HABIT_CONCERN, victim->pcdata->habit[HABIT_CONCERN]));
    strcat(string, buf);
    sprintf(buf, "Democracy: %s\n\r", habit_level(HABIT_DEMOCRACY, victim->pcdata->habit[HABIT_DEMOCRACY]));
    strcat(string, buf);
    sprintf(buf, "Equality: %s\n\r", habit_level(HABIT_EQUALITY, victim->pcdata->habit[HABIT_EQUALITY]));
    strcat(string, buf);
    sprintf(buf, "Cruelty: %s\n\r", habit_level(HABIT_CRUELTY, victim->pcdata->habit[HABIT_CRUELTY]));
    strcat(string, buf);
    sprintf(buf, "Intelligence: %s\n\r", habit_level(HABIT_INTEL, victim->pcdata->habit[HABIT_INTEL]));
    strcat(string, buf);

    if (level >= 2) {
      sprintf(buf, "Predator: %s\n\r", (victim->pcdata->patrol_habits[PATROL_PREDATORHABIT] == 0) ? "No" : "Yes");
      strcat(string, buf);
    }
    sprintf(buf, "Reckless: %s\n\r", (victim->pcdata->patrol_habits[PATROL_RECKLESSHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);
    sprintf(buf, "Hunting: %s\n\r", (victim->pcdata->patrol_habits[PATROL_HUNTHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);
    sprintf(buf, "Warfare: %s\n\r", (victim->pcdata->patrol_habits[PATROL_WARHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);
    sprintf(buf, "Arcane: %s\n\r", (victim->pcdata->patrol_habits[PATROL_ARCANEHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);
    sprintf(buf, "ArcaneWarfare: %s\n\r", (victim->pcdata->patrol_habits[PATROL_ARCANEWARHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);
    sprintf(buf, "Diplomatic: %s\n\r", (victim->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);
    sprintf(buf, "Spirit: %s\n\r", (victim->pcdata->patrol_habits[PATROL_SPIRITHABIT] == 0) ? "No" : "Yes");
    strcat(string, buf);

    if (level >= 1 && level < 10) {
      sprintf(buf, "Sins: %s\n\r", sinlist(victim));
      strcat(string, buf);
    }
    
    if (level >= 2 && level < 10) {
      sprintf(buf, "Character Goals:\n%s\n\r", victim->pcdata->char_goals);
      strcat(string, buf);
    }
    
    if (level >= 1 && level < 10) {
      sprintf(buf, "Fears:\n%s\n\r", victim->pcdata->char_fears);
      strcat(string, buf);
    }
    
    if (level == 4) {
      sprintf(buf, "Secrets:\n%s\n\r", victim->pcdata->char_secrets);
      strcat(string, buf);
    }
    
    if (level >= 2 && level < 10) {
      sprintf(buf, "Timeline:\n%s\n\r", victim->pcdata->char_timeline);
      strcat(string, buf);
    }
    
    if (level == 5) {
      sprintf(buf, "They've recently spoken poorly of %s, %s, and %s.\n\r", victim->pcdata->last_dissed[0], victim->pcdata->last_dissed[1], victim->pcdata->last_dissed[2]);
      strcat(string, buf);
      sprintf(buf, "They've recently spoken well of %s, %s, and %s.\n\r", victim->pcdata->last_praised[0], victim->pcdata->last_praised[1], victim->pcdata->last_praised[2]);
      strcat(string, buf);
      
      if (victim->pcdata->last_sexed[0] != NULL && safe_strlen(victim->pcdata->last_sexed[0]) > 1) {
        sprintf(buf, "They've slept with %s.\n\r", victim->pcdata->last_sexed[0]);
        strcat(string, buf);
      }
      
      if (victim->pcdata->last_sexed[1] != NULL && safe_strlen(victim->pcdata->last_sexed[1]) > 1) {
        sprintf(buf, "They've slept with %s.\n\r", victim->pcdata->last_sexed[1]);
        strcat(string, buf);
      }
      
      if (victim->pcdata->last_sexed[2] != NULL && safe_strlen(victim->pcdata->last_sexed[2]) > 1) {
        sprintf(buf, "They've slept with %s.\n\r", victim->pcdata->last_sexed[2]);
        strcat(string, buf);
      }
    }
    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
  }

  _DOFUN(do_lookup) {
    CHAR_DATA *victim;
    char arg1[MSL];
    int trust;

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "stats")) {
      printf_to_char(ch, "Total Emotes: %d\n\r", ch->pcdata->emotes[EMOTE_TOTAL]);
      printf_to_char(ch, "Combat Emotes: %d\n\r", ch->pcdata->emotes[EMOTE_COMBAT]);
      printf_to_char(ch, "Public Emotes: %d\n\r", ch->pcdata->emotes[EMOTE_PUBLIC]);
      printf_to_char(ch, "Sex Emotes: %d\n\r", ch->pcdata->emotes[EMOTE_SEX]);
      printf_to_char(ch, "Antagonistic Emotes: %d\n\r", ch->pcdata->emotes[EMOTE_ANTAG]);
      printf_to_char(ch, "Victim Emotes: %d\n\n\r", ch->pcdata->emotes[EMOTE_VICTIM]);
      printf_to_char(ch, "Influence spent on loyalty praises: %d\n\r", ch->pcdata->life_tracker[TRACK_LOYALTY_PRAISE_AMOUNT]);
      printf_to_char(ch, "Influence spent on loyalty disses: %d\n\r", ch->pcdata->life_tracker[TRACK_LOYALTY_DISS_AMOUNT]);
      printf_to_char(ch, "Influence spent on character praises: %d\n\r", ch->pcdata->life_tracker[TRACK_CHARACTER_PRAISE_AMOUNT]);
      printf_to_char(ch, "Influence spent on character disses: %d\n\r", ch->pcdata->life_tracker[TRACK_CHARACTER_DISS_AMOUNT]);
      printf_to_char(ch, "Influence spent on appearance praises: %d\n\r", ch->pcdata->life_tracker[TRACK_APPEARANCE_PRAISE_AMOUNT]);
      printf_to_char(ch, "Influence spent on appearance disses: %d\n\r", ch->pcdata->life_tracker[TRACK_APPEARANCE_DISS_AMOUNT]);
      printf_to_char(ch, "Rumors started: %d\n\r", ch->pcdata->life_tracker[TRACK_RUMORS]);
      printf_to_char(ch, "Big game hunts undertaken: %d\n\r", ch->pcdata->life_tracker[TRACK_PATROL_HUNTING]);
      printf_to_char(ch, "Patrol conflicts joined: %d\n\r", ch->pcdata->life_tracker[TRACK_PATROL_WARFARE]);
      printf_to_char(ch, "Patrol arcane tasks undertaken: %d\n\r", ch->pcdata->life_tracker[TRACK_PATROL_ARCANE]);
      printf_to_char(ch, "Patrol diplomatic tasks undertaken: %d\n\r", ch->pcdata->life_tracker[TRACK_PATROL_DIPLOMATIC]);
      printf_to_char(ch, "Victims stalked: %d\n\r", ch->pcdata->life_tracker[TRACK_PATROL_PREDATORY]);
      printf_to_char(ch, "Number of times stalked: %d\n\r", ch->pcdata->life_tracker[TRACK_PATROL_PREY]);
      printf_to_char(ch, "Total resources contributed: $%d\n\r", ch->pcdata->life_tracker[TRACK_CONTRIBUTED]);
      printf_to_char(ch, "Alchemical resources contributed: $%d\n\r", ch->pcdata->life_tracker[TRACK_ALCH_CONTRIBUTED]);
      printf_to_char(ch, "Operations created: %d\n\r", ch->pcdata->life_tracker[TRACK_OPERATIONS_CREATED]);
      printf_to_char(ch, "Operations attended: %d\n\r", ch->pcdata->life_tracker[TRACK_OPERATIONS_ATTENDED]);
      printf_to_char(ch, "Roads built: %d\n\r", ch->pcdata->life_tracker[TRACK_ROADS_BUILT]);
      printf_to_char(ch, "Forest areas cleared: %d\n\r", ch->pcdata->life_tracker[TRACK_TREES_CHOPPED]);
      printf_to_char(ch, "Schemes launched: %d\n\r", ch->pcdata->life_tracker[TRACK_SCHEMES_LAUNCHED]);
      printf_to_char(ch, "Schemes thwarted: %d\n\r", ch->pcdata->life_tracker[TRACK_SCHEMES_THWARTED]);
      printf_to_char(ch, "Social events held: %d\n\r", ch->pcdata->life_tracker[TRACK_EVENTS_HELD]);
      printf_to_char(ch, "Population at most popular event: %d\n\r", ch->pcdata->life_tracker[TRACK_EVENT_MAXPOP]);
      printf_to_char(ch, "Social events attended: %d\n\r", ch->pcdata->life_tracker[TRACK_EVENTS_ATTENDED]);
      printf_to_char(ch, "Adventures attended: %d\n\r", ch->pcdata->life_tracker[TRACK_ADVENTURES]);
      printf_to_char(ch, "Times having sex: %d\n\r", ch->pcdata->life_tracker[TRACK_SEX]);
      printf_to_char(ch, "Otherworldly monsters slain: %d\n\r", ch->pcdata->life_tracker[TRACK_BIGMONSTER]);
      return;
    }

    victim = get_char_world_pc(arg1);

    if (!str_cmp(arg1, "me") || !str_cmp(arg1, "self"))
    victim = ch;

    if (!is_gm(ch) && ch != victim && !higher_power(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }

    if (!IS_IMMORTAL(ch)) {
      if (victim == NULL || IS_NPC(victim) || (get_probetrust(ch, victim) < 1 && ch != victim)) {
        send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      if (ch == victim)
      trust = 4;
      else
      trust = get_probetrust(ch, victim);
    }
    else {
      trust = 4;
    }

    lookup_char(ch, victim, trust);
  }

  _DOFUN(do_share) {
    CHAR_DATA *victim;
    char arg1[MSL];

    int diff = current_time - ch->pcdata->create_date;
    int diff3 = current_time - ch->pcdata->last_share;

    argument = one_argument_nouncap(argument, arg1);

    diff = diff / 3600 / 24;
    diff3 = diff3 / 3600 / 24;

    if (diff < 30) {
      send_to_char("You can't share OOC contact details until you've been playing for at least a month.\n\r", ch);
      return;
    }
    if (diff3 < 7) {
      send_to_char("Wait at least a week until doing that again.\n\r", ch);
      return;
    }

    if (safe_strlen(argument) < 2) {
      send_to_char("Syntax:share (person) (Your details)\n\r", ch);
      return;
    }

    victim = get_char_world(ch, arg1);

    if (victim == NULL) {
      send_to_char("Can't find them.\n\r", ch);
      return;
    }

    int diff2 = current_time - victim->pcdata->create_date;

    diff2 = diff2 / 3600 / 24;

    if (diff2 < 30) {
      send_to_char("You can't share OOC contact details until they've been playing for at least a month.\n\r", ch);
      return;
    }

    printf_to_char(victim, "%s would like to share their OOC contact details with you, their details are: %s", PERS(victim, ch), argument);
    send_to_char("Details sent.\n\r", ch);

    ch->pcdata->last_share = current_time;
  }

  char *chatroom_name(int chan) {
    if (chan == 1)
    return "General";
    if (chan == 2)
    return "Games";
    if (chan == 3)
    return "Singles";
    if (chan == 4)
    return "PrivateOne";
    if (chan == 5)
    return "PrivateTwo";
    if (chan == 6)
    return "PrivateThree";
    if (chan == 7)
    return "Conspiracy";
    if (chan == 8)
    return "666";

    return "General";
  }

  char *chat_handle(CHAR_DATA *ch) {

    if (ch == NULL || IS_NPC(ch))
    return "Guest";

    if (safe_strlen(ch->pcdata->chat_handle) < 2)
    return "Guest";

    return ch->pcdata->chat_handle;
  }

  void chat_message(char message[MSL], int chan, CHAR_DATA *ch) {
    char buf[MSL];
    char *numbered_history;
    char *buf2;
    int lines = 1;

    bool mod_flagged = FALSE;
    if (ch->played / 3600 < 25)
    mod_flagged = isTextFlagged(message);

    if (chan == 0 || chan > 20 || chan < 0)
    return;

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
      if (victim->in_room == NULL) {
        continue;
      }
      if (IS_NPC(victim)) {
        continue;
      }
      if (spammer(ch) && !same_player(ch, victim)) {
        continue;
      }
      if (victim->pcdata->chatroom == 0) {
        continue;
      }
      if (!cell_signal(victim)) {
        continue;
      }
      if (is_town_blackout()) {
        continue;
      }
      if (victim != NULL && !same_player(ch, victim) && mod_flagged == TRUE)
      continue;

      // Added a check to make sure the person can access the internet before
      // hearing chat messages - Discordance
      if (victim->pcdata->chatroom == chan && access_internet(victim) == true) {
        send_to_char(message, victim);

        if (victim->recent_moved <= -300 && has_computer(victim) && get_skill(victim, SKILL_HACKING) >= 4 && number_percent() % 4 == 0 && get_phone(ch) != NULL)
        printf_to_char(victim, "(%d)\n\r", get_phone(ch)->value[0]);
      }

      if (victim->pcdata->chatroom == chan) {
        // this counts lines and limits the length of chat history - Discordance
        numbered_history = numlineas(victim->pcdata->chat_history);
        for (buf2 = numbered_history; *buf2; buf2++) {
          if (*buf2 == '\r') {
            lines++;
          }
        }

        strcpy(buf, "");

        if (lines < 80) {
          sprintf(buf, "%s\n%s", victim->pcdata->chat_history, message);
        }
        else {
          victim->pcdata->chat_history =
          string_deleteline(victim->pcdata->chat_history, atoi("1"));
          sprintf(buf, "%s\n%s", victim->pcdata->chat_history, message);
        }
        free_string(victim->pcdata->chat_history);
        victim->pcdata->chat_history = str_dup(buf);
      }
    }
  }

  // access_internet is a new function for determining internet access -
  // Discordance
  bool access_internet(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = ch->on;
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (is_town_blackout())
    return FALSE;
    if (get_phone(ch) == NULL || IS_SET(get_phone(ch)->extra_flags, ITEM_OFF))
    return FALSE;

    if (is_blind(ch))
    return FALSE;

    if (strcasestr(ch->pcdata->place, "computer") != NULL)
    return TRUE;
    if (strcasestr(ch->pcdata->place, "laptop") != NULL)
    return TRUE;

    if (obj != NULL) {
      if (strcasestr(obj->name, "computer") != NULL)
      return TRUE;
    }

    if (get_held(ch, ITEM_CONTAINER) != NULL) {
      if (strcasestr(get_held(ch, ITEM_CONTAINER)->name, "computer"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_CONTAINER)->name, "laptop"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_CONTAINER)->name, "tablet"))
      return TRUE;
    }

    if (get_held(ch, ITEM_TRASH) != NULL) {
      if (strcasestr(get_held(ch, ITEM_TRASH)->name, "computer"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_TRASH)->name, "laptop"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_TRASH)->name, "tablet"))
      return TRUE;
    }

    if (get_held(ch, ITEM_PHONE) != NULL) {
      if (get_held(ch, ITEM_PHONE)->cost >= 20000 && !IS_SET(get_held(ch, ITEM_PHONE)->extra_flags, ITEM_OFF))
      return TRUE;
    }

    if (get_worn(ch, ITEM_CONTAINER) != NULL) {
      if (strcasestr(get_worn(ch, ITEM_CONTAINER)->name, "computer"))
      return TRUE;
      if (strcasestr(get_worn(ch, ITEM_CONTAINER)->name, "laptop"))
      return TRUE;
      if (strcasestr(get_worn(ch, ITEM_CONTAINER)->name, "tablet"))
      return TRUE;
    }

    return FALSE;
  }

  bool has_computer(CHAR_DATA *ch) {

    OBJ_DATA *obj;

    obj = ch->on;
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (is_town_blackout())
    return FALSE;

    if (is_blind(ch))
    return FALSE;

    if (strcasestr(ch->pcdata->place, "computer") != NULL)
    return TRUE;

    if (obj != NULL) {
      if (strcasestr(obj->name, "computer") != NULL && obj->cost >= 50000)
      return TRUE;
    }
    if (get_held(ch, ITEM_CONTAINER) != NULL) {
      obj = get_held(ch, ITEM_CONTAINER);
      if (strcasestr(obj->name, "computer") && obj->cost >= 50000)
      return TRUE;
      if (strcasestr(obj->name, "laptop") && obj->cost >= 50000)
      return TRUE;
      if (strcasestr(obj->name, "tablet") && obj->cost >= 50000)
      return TRUE;
    }
    if (get_held(ch, ITEM_TRASH) != NULL) {
      obj = get_held(ch, ITEM_TRASH);
      if (strcasestr(obj->name, "computer") && obj->cost >= 50000)
      return TRUE;
      if (strcasestr(obj->name, "laptop") && obj->cost >= 50000)
      return TRUE;
      if (strcasestr(obj->name, "tablet") && obj->cost >= 50000)
      return TRUE;
    }
    if (get_held(ch, ITEM_PHONE) != NULL) {
      obj = get_held(ch, ITEM_PHONE);
      if (strcasestr(obj->name, "computer") && obj->cost >= 50000)
      return TRUE;
      if (strcasestr(obj->name, "laptop") && obj->cost >= 50000)
      return TRUE;
      if (strcasestr(obj->name, "tablet") && obj->cost >= 50000)
      return TRUE;
    }

    return FALSE;
  }

  _DOFUN(do_chatroomold) {
    char arg1[MSL];
    char buf[MSL] = "";

    argument = one_argument_nouncap(argument, arg1);

    if (IS_FLAG(ch->act, PLR_DEAD) || (ch->in_room != NULL && ch->in_room->vnum == 98)) {
      return;
    }
    if (is_helpless(ch)) {
      return;
    }
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID) {
      send_to_char("Damn your lack of opposable thumbs!\n\r", ch);
      return;
    }
    if (in_fight(ch) || ch->hit <= 0) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_SHROUD))
    return;

    if (ch->spentrpexp < 10000 && ch->spentexp < 10000 && ch->played / 3600 < 20) {
      send_to_char("You should play the game for a bit longer first.\n\r", ch);
      return;
    }

    // Added some checks to make sure people aren't connecting when already
    // connected and such - Discordance
    if (!str_cmp(arg1, "logoff")) {
      if (ch->pcdata->chatroom > 0) {
        sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
        chat_message(buf, ch->pcdata->chatroom, ch);
        ch->pcdata->chatroom = 0;

        free_string(ch->pcdata->chat_history);
        ch->pcdata->chat_history = str_dup("");

        send_to_char("You log off the chat.\n\r", ch);
      }
      else {
        send_to_char("You're already logged off.\n\r", ch);
      }
      return;
    }

    // Added a check for internet capable device for the chatroom commands
    // excluding logoff - Discordance
    if (access_internet(ch) == FALSE) {
      send_to_char("You don't have an internet capable device on hand.\n\r", ch);
      return;
    }
    if (no_tech(ch)) {
      send_to_char("Your device doesn't seem to be responding.\n\r", ch);
      return;
    } // tech hex
    if (!cell_signal(ch)) {
      return;
    }
    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    if (!str_cmp(arg1, "logon")) {
      if (ch->pcdata->chatroom > 0) {
        send_to_char("You're already logged on.\n\r", ch);
      }
      else {
        ch->pcdata->chatroom = 1;
        send_to_char("You log onto the chat.\n\r", ch);
        sprintf(buf, "`c[`x%s`c]`x%s has joined the room.\n\r", chatroom_name(1), chat_handle(ch));
        chat_message(buf, 1, ch);
      }
      return;
    }
    else if (!str_cmp(arg1, "join")) {
      if (!str_cmp(argument, "general")) {
        if (ch->pcdata->chatroom == 1) {
          send_to_char("You're already in General.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 1;
          send_to_char("You join the general room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(1), chat_handle(ch));
          chat_message(buf, 1, ch);
        }
      }
      else if (!str_cmp(argument, "games")) {
        if (ch->pcdata->chatroom == 2) {
          send_to_char("You're already in Games.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 2;
          send_to_char("You join the games room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(2), chat_handle(ch));
          chat_message(buf, 2, ch);
        }
      }
      else if (!str_cmp(argument, "singles")) {
        if (ch->pcdata->chatroom == 3) {
          send_to_char("You're already in Singles.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 3;
          send_to_char("You join the singles room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(3), chat_handle(ch));
          chat_message(buf, 3, ch);
        }
      }
      else if (!str_cmp(argument, "conspiracy")) {
        if (ch->pcdata->chatroom == 7) {
          send_to_char("You're already in Conspiracy.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 7;
          send_to_char("You join the conspiracy room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(7), chat_handle(ch));
          chat_message(buf, 3, ch);
        }
      }
      else if (!str_cmp(argument, "privateone")) {
        if (ch->pcdata->chatroom == 4) {
          send_to_char("You're already in Private One.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 4;
          send_to_char("You join the privateone room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(4), chat_handle(ch));
          chat_message(buf, 4, ch);
        }
      }
      else if (!str_cmp(argument, "privatetwo")) {
        if (ch->pcdata->chatroom == 5) {
          send_to_char("You're already in Private Two.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 5;
          send_to_char("You join the privatetwo room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(5), chat_handle(ch));
          chat_message(buf, 5, ch);
        }
      }
      else if (!str_cmp(argument, "privatethree")) {
        if (ch->pcdata->chatroom == 6) {
          send_to_char("You're already in Private Three.\n\r", ch);
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 6;
          send_to_char("You join the privatethree room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(6), chat_handle(ch));
          chat_message(buf, 6, ch);
        }
      }
      else if (!str_cmp(argument, "666")) {
        if (ch->pcdata->chatroom == 8) {
          send_to_char("You're already in 666.\n\r", ch);
        }
        if (get_skill(ch, SKILL_HACKING) < 2 || get_skill(ch, SKILL_DEMONOLOGY) < 1) {
          send_to_char("That chatroom is a myth.\n\r", ch);
          return;
        }
        else {
          sprintf(buf, "`c[`x%s`c]`x%s`x has left the room.\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch));
          chat_message(buf, ch->pcdata->chatroom, ch);
          ch->pcdata->chatroom = 8;
          send_to_char("You join the 666 room.\n\r", ch);
          sprintf(buf, "`c[`x%s`c]`x%s`x has joined the room.\n\r", chatroom_name(8), chat_handle(ch));
          chat_message(buf, 8, ch);
        }
      }
      else {
        send_to_char("Rooms are general, games, singles, conspiracy, privateone, privatetwo, privatethree, 666.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "look")) {
      CHAR_DATA *victim;

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->connected != CON_PLAYING)
        continue;

        victim = CH(d);

        if (victim == NULL)
        continue;

        if (IS_NPC(victim))
        continue;

        if (victim->pcdata->chatroom == 0)
        continue;

        if (!cell_signal(victim))
        continue;

        if (victim->pcdata->chatroom == ch->pcdata->chatroom)
        printf_to_char(ch, "%s`x\n\r", chat_handle(victim));
      }
    }
    else if (!str_cmp(arg1, "handle")) {
      free_string(ch->pcdata->chat_handle);
      ch->pcdata->chat_handle = str_dup(argument);
      printf_to_char(ch, "New handle set to %s`x.\n\r", ch->pcdata->chat_handle);
    }
    // Added history and clear to display and clear new chat log at
    // ch->pcdata->chat_history - Discordance
    else if (!str_cmp(arg1, "history")) {
      strcpy(buf, ch->pcdata->chat_history);
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg1, "clear")) {
      free_string(ch->pcdata->chat_history);
      ch->pcdata->chat_history = str_dup("");
      send_to_char("Chatroom history cleared.\n\r", ch);
    }
    else if (safe_strlen(arg1) > 0) {
      if (is_griefer(ch) || spammer(ch)) {
        sprintf(buf, "`c[`x%s`c]`x%s`x: %s %s\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch), arg1, argument);
        printf_to_char(ch, "%s", buf);
        return;
      }

      if (number_percent() % 8 == 0 && !is_gm(ch))
      act("$n types a message out on their phone.", ch, NULL, NULL, TO_ROOM);

      ch->pcdata->time_since_emote = 0;
      sprintf(buf, "`c[`x%s`c]`x%s`x: %s %s\n\r", chatroom_name(ch->pcdata->chatroom), chat_handle(ch), arg1, argument);
      chat_message(buf, ch->pcdata->chatroom, ch);

      if (IS_NPC(ch) || ch->pcdata->institute_action == 0)
      rpreward(ch, argument, TRUE, 1);

    }
    else
    send_to_char("Syntax: Chatroom logon/logoff/look/join (general/games/conspiracy/singles/privateone/privatetwo/privatethree)/666 chatroom (message)\n\r", ch);
  }

  _DOFUN(do_showto) {
    static char attempt[MAX_STRING_LENGTH];
    attempt[0] = '\0';
    static char stat1[MAX_STRING_LENGTH];
    stat1[0] = '\0';
    static char stat2[MAX_STRING_LENGTH];
    stat2[0] = '\0';
    static char stat3[MAX_STRING_LENGTH];
    stat3[0] = '\0';

    char arg1[MSL];
    int input = 0;
    int i = 0;
    argument = one_argument_nouncap(argument, arg1);

    if (!IS_FLAG(ch->comm, COMM_RUNNING)) {
      send_to_char("You have to be running the room first.\n\r", ch);
      return;
    }

    for (; safe_strlen(arg1) > 0 && arg1 != '\0';) {
      if (input == 0) {
        if (str_cmp(arg1, "with")) {
          strcat(attempt, arg1);
          strcat(attempt, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 1) {
        if (str_cmp(arg1, "and")) {
          strcat(stat1, arg1);
          strcat(stat1, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 2) {
        if (str_cmp(arg1, "and")) {
          strcat(stat2, arg1);
          strcat(stat2, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 3) {
        if (str_cmp(arg1, "and")) {
          strcat(stat3, arg1);
          strcat(stat3, " ");
        }
        else {
          input++;
        }
      }
      argument = one_argument_nouncap(argument, arg1);
    }

    i = safe_strlen(stat1);
    stat1[i - 1] = '\0';
    i = safe_strlen(stat2);
    stat2[i - 1] = '\0';
    i = safe_strlen(stat3);
    stat3[i - 1] = '\0';

    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room) {
        continue;
      }

      if (victim == ch)
      continue;

      if (input == 0) {
        printf_to_char(victim, "%s\n\r", attempt);
        continue;
      }
      for (i = 0; i < DIS_USED; i++) {
        if (!str_cmp(stat1, discipline_table[i].name)) {
          if (victim->disciplines[discipline_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
          printf_to_char(victim, "%s\n\r", attempt);
        }
        if (!str_cmp(stat2, discipline_table[i].name)) {
          if (victim->disciplines[discipline_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
          printf_to_char(victim, "%s\n\r", attempt);
        }
        if (!str_cmp(stat3, discipline_table[i].name)) {
          if (victim->disciplines[discipline_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
          printf_to_char(victim, "%s\n\r", attempt);
        }
      }
      for (i = 0; i < SKILL_USED; i++) {
        if (!str_cmp(stat1, skill_table[i].name)) {
          if (victim->skills[skill_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
          printf_to_char(victim, "%s\n\r", attempt);
        }
        if (!str_cmp(stat2, skill_table[i].name)) {
          if (victim->skills[skill_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
          printf_to_char(victim, "%s\n\r", attempt);
        }
        if (!str_cmp(stat3, skill_table[i].name)) {
          if (victim->skills[skill_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
          printf_to_char(victim, "%s\n\r", attempt);
        }
      }
    }

    if (input == 0) {
      printf_to_char(ch, "You show %sto the room.\n\r", attempt, stat1);
    }
    else if (input == 1) {
      printf_to_char(ch, "You show %sto those with %s\n\r", attempt, stat1);
    }
    else if (input == 2) {
      printf_to_char(ch, "You show %sto those with %s and %s\n\r", attempt, stat1, stat2);

    }
    else if (input == 3) {
      printf_to_char(ch, "You show %sto those with %s, %s and %s\n\r", attempt, stat1, stat2, stat3);
    }
  }

  void winner(CHAR_DATA *ch, CHAR_DATA *victim, int input, char stat1[MAX_STRING_LENGTH], char stat2[MAX_STRING_LENGTH], char stat3[MAX_STRING_LENGTH]) {
    char buf[MSL];
    if (input == 1) {
      sprintf(buf, "You beat $N at a contest of %s", stat1);
      act(buf, ch, NULL, victim, TO_CHAR);
      sprintf(buf, "$n beats you at a contest of %s", stat1);
      act(buf, ch, NULL, victim, TO_VICT);
      sprintf(buf, "$n beats $N at a contest of %s", stat1);
      act(buf, ch, NULL, victim, TO_NOTVICT);
    }
    else if (input == 2) {
      sprintf(buf, "You beat $N at a contest of %s and %s", stat1, stat2);
      act(buf, ch, NULL, victim, TO_CHAR);
      sprintf(buf, "$n beats you at a contest of %s and %s", stat1, stat2);
      act(buf, ch, NULL, victim, TO_VICT);
      sprintf(buf, "$n beats $N at a contest of %s and %s", stat1, stat2);
      act(buf, ch, NULL, victim, TO_NOTVICT);
    }
    else if (input == 3) {
      sprintf(buf, "You beat $N at a contest of %s, %s and %s", stat1, stat2, stat3);
      act(buf, ch, NULL, victim, TO_CHAR);
      sprintf(buf, "$n beats you at a contest of %s, %s and %s", stat1, stat2, stat3);
      act(buf, ch, NULL, victim, TO_VICT);
      sprintf(buf, "$n beats $N at a contest of %s, %s and %s", stat1, stat2, stat3);
      act(buf, ch, NULL, victim, TO_NOTVICT);
    }
  }

  _DOFUN(do_compete) {
    if (ch->spentrpexp < 10000 && ch->spentexp < 10000 && ch->played / 3600 < 40) {
      send_to_char("You need to get to 50 hours before using the compete command.\n\r", ch);
      return;
    }

    static char attempt[MAX_STRING_LENGTH];
    attempt[0] = '\0';
    static char stat1[MAX_STRING_LENGTH];
    stat1[0] = '\0';
    static char stat2[MAX_STRING_LENGTH];
    stat2[0] = '\0';
    static char stat3[MAX_STRING_LENGTH];
    stat3[0] = '\0';
    CHAR_DATA *victim;
    char arg1[MSL];
    int input = 0;
    int level1 = 0, level2 = 0, level3 = 0;
    int i = 0;
    int vlevel1 = 0, vlevel2 = 0, vlevel3 = 0;
    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "Against"))
    argument = one_argument_nouncap(argument, arg1);

    for (; safe_strlen(arg1) > 0 && arg1 != '\0';) {
      if (input == 0) {
        if (str_cmp(arg1, "with")) {
          strcat(attempt, arg1);
          strcat(attempt, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 1) {
        if (str_cmp(arg1, "and")) {
          strcat(stat1, arg1);
          strcat(stat1, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 2) {
        if (str_cmp(arg1, "and")) {
          strcat(stat2, arg1);
          strcat(stat2, " ");
        }
        else {
          input++;
        }
      }
      else if (input == 3) {
        if (str_cmp(arg1, "and")) {
          strcat(stat3, arg1);
          strcat(stat3, " ");
        }
        else {
          input++;
        }
      }
      argument = one_argument_nouncap(argument, arg1);
    }

    i = safe_strlen(stat1);
    stat1[i - 1] = '\0';
    i = safe_strlen(stat2);
    stat2[i - 1] = '\0';
    i = safe_strlen(stat3);
    stat3[i - 1] = '\0';

    for (i = 0; i < SKILL_USED; i++) {
      if (!str_cmp(stat1, skill_table[i].name)) {
        level1 = get_skill(ch, skill_table[i].vnum);
      }
      if (!str_cmp(stat2, skill_table[i].name)) {
        level2 = get_skill(ch, skill_table[i].vnum);
      }
      if (!str_cmp(stat3, skill_table[i].name)) {
        level3 = get_skill(ch, skill_table[i].vnum);
      }
    }
    for (i = 0; i < DIS_USED; i++) {
      if (!str_cmp(stat1, discipline_table[i].name) && level1 == 0) {
        level1 = get_disc(ch, discipline_table[i].vnum, FALSE);
      }
      if (!str_cmp(stat2, discipline_table[i].name) && level2 == 0) {
        level2 = get_disc(ch, discipline_table[i].vnum, FALSE);
      }
      if (!str_cmp(stat3, discipline_table[i].name) && level3 == 0) {
        level3 = get_disc(ch, discipline_table[i].vnum, FALSE);
      }
    }

    victim = get_char_room(ch, NULL, attempt);
    if (victim == NULL || IS_NPC(victim)) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    for (i = 0; i < SKILL_USED; i++) {
      if (!str_cmp(stat1, skill_table[i].name)) {
        vlevel1 = get_skill(victim, skill_table[i].vnum);
      }
      if (!str_cmp(stat2, skill_table[i].name)) {
        vlevel2 = get_skill(victim, skill_table[i].vnum);
      }
      if (!str_cmp(stat3, skill_table[i].name)) {
        vlevel3 = get_skill(victim, skill_table[i].vnum);
      }
    }

    for (i = 0; i < DIS_USED; i++) {
      if (!str_cmp(stat1, discipline_table[i].name) && vlevel1 == 0) {
        vlevel1 = get_disc(victim, discipline_table[i].vnum, FALSE);
      }
      if (!str_cmp(stat2, discipline_table[i].name) && vlevel2 == 0) {
        vlevel2 = get_disc(victim, discipline_table[i].vnum, FALSE);
      }
      if (!str_cmp(stat3, discipline_table[i].name) && vlevel3 == 0) {
        vlevel3 = get_disc(victim, discipline_table[i].vnum, FALSE);
      }
    }

    int chlevel = level1 + level2 + level3;
    int viclevel = vlevel1 + vlevel2 + vlevel3;

    if (str_cmp(stat1, "Games") && str_cmp(stat2, "Games") && str_cmp(stat3, "Games")) {
      if (number_percent() % 6 == 0) {
        if (IS_AFFECTED(ch, AFF_UNLUCKY))
        chlevel--;
        if (IS_AFFECTED(victim, AFF_UNLUCKY))
        viclevel--;
        if (IS_AFFECTED(ch, AFF_LUCKY))
        chlevel++;
        if (IS_AFFECTED(victim, AFF_LUCKY))
        viclevel++;
      }
    }

    if (!str_cmp(stat1, "Games") || !str_cmp(stat2, "Games") || !str_cmp(stat3, "Games")) {
      if (IS_AFFECTED(ch, AFF_UNLUCKY))
      chlevel--;
      if (IS_AFFECTED(victim, AFF_UNLUCKY))
      viclevel--;
      if (IS_AFFECTED(ch, AFF_LUCKY))
      chlevel++;
      if (IS_AFFECTED(victim, AFF_LUCKY))
      viclevel++;

      if (chlevel == viclevel) {
        if (number_percent() % 2 == 0) {
          winner(ch, victim, input, stat1, stat2, stat3);
        }
        else {
          winner(victim, ch, input, stat1, stat2, stat3);
        }
      }
      else if (chlevel > viclevel) {
        int val = chlevel - viclevel;
        int chance = 50;
        for (i = 0; i < val; i++)
        chance = chance * 4 / 5;

        if (number_percent() > chance)
        winner(ch, victim, input, stat1, stat2, stat3);
        else
        winner(victim, ch, input, stat1, stat2, stat3);
      }
      else {
        int val = viclevel - chlevel;
        int chance = 50;
        for (i = 0; i < val; i++)
        chance = chance * 4 / 5;

        if (number_percent() > chance)
        winner(victim, ch, input, stat1, stat2, stat3);
        else
        winner(ch, victim, input, stat1, stat2, stat3);
      }

    }
    else if (chlevel == viclevel) {
      if (IS_AFFECTED(ch, AFF_UNLUCKY))
      winner(victim, ch, input, stat1, stat2, stat3);
      else if (IS_AFFECTED(victim, AFF_UNLUCKY))
      winner(ch, victim, input, stat1, stat2, stat3);
      else if (IS_AFFECTED(ch, AFF_LUCKY))
      winner(ch, victim, input, stat1, stat2, stat3);
      else if (IS_AFFECTED(victim, AFF_LUCKY))
      winner(victim, ch, input, stat1, stat2, stat3);
      else if (number_percent() % 2 == 0) {
        winner(ch, victim, input, stat1, stat2, stat3);
      }
      else {
        winner(victim, ch, input, stat1, stat2, stat3);
      }
    }
    else if (chlevel > viclevel)
    winner(ch, victim, input, stat1, stat2, stat3);
    else
    winner(victim, ch, input, stat1, stat2, stat3);
  }

  void offline_message(char *arg1, char *message) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    d.original = NULL;

    if ((victim = get_char_world_pc(arg1)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Offline message");

      if (!load_char_obj(&d, arg1)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg1));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }
    sprintf(buf, "%s\n%s", victim->pcdata->messages, message);
    free_string(victim->pcdata->messages);
    victim->pcdata->messages = str_dup(buf);
    save_char_obj(victim, FALSE, FALSE);

    if (online)
    printf_to_char(victim, "%s\n\r", message);

    if (!online)
    free_char(victim);
  }

  _DOFUN(do_broadcast) {
    static char attempt[MAX_STRING_LENGTH];
    attempt[0] = '\0';
    static char stat1[MAX_STRING_LENGTH];
    stat1[0] = '\0';
    static char stat2[MAX_STRING_LENGTH];
    stat2[0] = '\0';
    static char stat3[MAX_STRING_LENGTH];
    stat3[0] = '\0';

    char arg1[MSL];
    int input = 0;
    int i = 0;

    if (ch->race == RACE_FACULTY) {
      if (ch->in_room->vnum != 16128) {
        send_to_char("You need to visit the main office to make an announcement.\n\r", ch);
        return;
      }
    }
    else {
      if (!is_gm(ch)) {
        send_to_char("That is a Story Runner command.\n\r", ch);
        return;
      }
      if (!has_crisis(ch) && !crisis_runner(ch)) {
        send_to_char("You aren't running a crisis enabled plot.\n\r", ch);
        return;
      }
    }

    if (is_gm(ch)) {
      argument = one_argument_nouncap(argument, arg1);

      for (; safe_strlen(arg1) > 0 && arg1 != '\0';) {
        if (input == 0) {
          if (str_cmp(arg1, "with")) {
            strcat(attempt, arg1);
            strcat(attempt, " ");
          }
          else {
            input++;
          }
        }
        else if (input == 1) {
          if (str_cmp(arg1, "and")) {
            strcat(stat1, arg1);
            strcat(stat1, " ");
          }
          else {
            input++;
          }
        }
        else if (input == 2) {
          if (str_cmp(arg1, "and")) {
            strcat(stat2, arg1);
            strcat(stat2, " ");
          }
          else {
            input++;
          }
        }
        else if (input == 3) {
          if (str_cmp(arg1, "and")) {
            strcat(stat3, arg1);
            strcat(stat3, " ");
          }
          else {
            input++;
          }
        }
        argument = one_argument_nouncap(argument, arg1);
      }

      i = safe_strlen(stat1);
      stat1[i - 1] = '\0';
      i = safe_strlen(stat2);
      stat2[i - 1] = '\0';
      i = safe_strlen(stat3);
      stat3[i - 1] = '\0';
    }

    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim == ch)
      continue;

      if (IS_IMMORTAL(victim) && ch->race == RACE_FACULTY) {
        printf_to_char(
        victim, "%s broadcasts: A hidden intercom crackles to life. It speaks, %s, '%s' The announcement is heard throughout the Institute.\n\r", ch->name, ch->pcdata->talk, argument);
        continue;
      }
      else if (IS_IMMORTAL(victim)) {
        printf_to_char(victim, "%s broadcasts: %s\n\r", ch->name, attempt);
        continue;
      }

      if (!IS_IMMORTAL(ch) && !is_gm(ch)) {
        if (room_in_school(victim->in_room->vnum)) {
          printf_to_char(
          victim, "A hidden intercom crackles to life. It speaks, %s, '%s' The announcement is heard throughout the Institute.\n\r", ch->pcdata->talk, argument);
          continue;
        }
      }
      else {
        if (input == 0) {
          printf_to_char(victim, "%s\n\r", attempt);
          continue;
        }
        for (i = 0; i < DIS_USED; i++) {
          if (!str_cmp(stat1, discipline_table[i].name)) {
            if (victim->disciplines[discipline_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
            printf_to_char(victim, "%s\n\r", attempt);
          }
          if (!str_cmp(stat2, discipline_table[i].name)) {
            if (victim->disciplines[discipline_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
            printf_to_char(victim, "%s\n\r", attempt);
          }
          if (!str_cmp(stat3, discipline_table[i].name)) {
            if (victim->disciplines[discipline_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
            printf_to_char(victim, "%s\n\r", attempt);
          }
        }
        for (i = 0; i < SKILL_USED; i++) {
          if (!str_cmp(stat1, skill_table[i].name)) {
            if (victim->skills[skill_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
            printf_to_char(victim, "%s\n\r", attempt);
          }
          if (!str_cmp(stat2, skill_table[i].name)) {
            if (victim->skills[skill_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
            printf_to_char(victim, "%s\n\r", attempt);
          }
          if (!str_cmp(stat3, skill_table[i].name)) {
            if (victim->skills[skill_table[i].vnum] > 0 || IS_FLAG(victim->act, PLR_GM) || IS_IMMORTAL(victim))
            printf_to_char(victim, "%s\n\r", attempt);
          }
        }
      }
    }

    if (ch->race == RACE_FACULTY) {
      printf_to_char(
      ch, "A hidden intercom crackles to life. You speak, %s, '%s' The announcement is heard throughout the Institute.\n\r", ch->pcdata->talk, argument);
    }
    else {
      if (input == 0) {
        printf_to_char(ch, "You show %sto the world.\n\r", attempt, stat1);
      }
      else if (input == 1) {
        printf_to_char(ch, "You show %sto those with %s\n\r", attempt, stat1);
      }
      else if (input == 2) {
        printf_to_char(ch, "You show %sto those with %s and %s\n\r", attempt, stat1, stat2);

      }
      else if (input == 3) {
        printf_to_char(ch, "You show %sto those with %s, %s and %s\n\r", attempt, stat1, stat2, stat3);
      }
      if (is_gm(ch)) {
        int type = KARMA_OTHER;
        int karmagain = safe_strlen(attempt) * 5 / 130;

        if (get_karma_plot(ch) != NULL && ch->pcdata->encounter_status != ENCOUNTER_ONGOING) {
          if (get_karma_plot(ch)->karmacap == 0 || get_karma_plot(ch)->earnedkarma < get_karma_plot(ch)->karmacap) {
            if (has_mystery(ch))
            type = KARMA_MYSTERY;
            else
            type = KARMA_ADVENTURE;
            karmagain *= 25;
          }
        }
        else if (ch->pcdata->karma_battery > 0) {
          type = KARMA_ENCOUNTER;
          karmagain *= 25;
        }
        else if (ch->pcdata->scheme_running != NULL) {
          type = KARMA_SCHEME;
          karmagain *= 25;
        }
        else if (battleground(ch->in_room)) {
          karmagain *= 8;
          type = KARMA_AMBIANT;
        }
        else if (in_world(ch) != WORLD_EARTH || battleground(ch->in_room)) {
          karmagain *= 4;
          type = KARMA_AMBIANT;
        }
        else if (crisis_runner(ch)) {
          karmagain *= 4;
          type = KARMA_AMBIANT;
        }
        else
        {
          type = KARMA_AMBIANT;
          karmagain /= 4;
        }

        karmagain = karmagain * UMIN(150, average_solidity(ch->in_room)) / 100;
        give_karma(ch, karmagain, type);
      }
    }
  }

  _DOFUN(do_deputize) {

    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_world_pc(arg1);

    if (!IS_IMMORTAL(ch) && ch != victim) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }

    if (victim == NULL || IS_NPC(victim) || !is_gm(victim)) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }

    if (IS_FLAG(victim->comm, COMM_DEPUTY)) {
      send_to_char("You undeputize them.\n\r", ch);
      REMOVE_FLAG(victim->comm, COMM_DEPUTY);
    }
    else {
      send_to_char("You deputize them\n\r", ch);
      SET_FLAG(victim->comm, COMM_DEPUTY);
    }
  }

  bool can_hear_freq(CHAR_DATA *ch, int number) {
    if (IS_NPC(ch))
    return FALSE;

    if (number <= 0)
    return FALSE;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->chan_numbers[i] == number && ch->pcdata->chan_status[i] > 0)
      return TRUE;
    }

    return FALSE;
  }

  char *mad_name(CHAR_DATA *ch, CHAR_DATA *victim) {
    NameMap::const_iterator nit;
    if (victim->sex == SEX_FEMALE) {
      if ((nit = ch->pcdata->female_names->find(victim->id)) !=
          ch->pcdata->female_names->end()) {
        return nit->second;
      }
    }
    if (victim->sex == SEX_MALE) {
      if ((nit = ch->pcdata->male_names->find(victim->id)) !=
          ch->pcdata->male_names->end()) {
        return nit->second;
      }
    }
    return NAME(victim);
  }

  char *comm_insult(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf[MSL];

    switch (number_percent() % 7) {
    case 0:
      sprintf(buf, "You should just kill yourself %s.", mad_name(ch, victim));
      return str_dup(buf);
      break;
    case 1:
      sprintf(buf, "Nobody cares %s.", mad_name(ch, victim));
      return str_dup(buf);
      break;
    case 2:
      sprintf(buf, "Hey %s, is it true you're a coward?", mad_name(ch, victim));
      return str_dup(buf);
      break;
    case 3:
      sprintf(buf, "You're pathetic %s.", mad_name(ch, victim));
      return str_dup(buf);
      break;
    case 4:
      sprintf(buf, "I never really liked you %s.", mad_name(ch, victim));
      return str_dup(buf);
      break;
    case 5:
      if (victim->sex == SEX_FEMALE)
      sprintf(buf, "%s you're a stupid whore.", mad_name(ch, victim));
      else
      sprintf(buf, "%s you're a pathetic coward.", mad_name(ch, victim));
      return str_dup(buf);
      break;
    default:
      sprintf(buf, "Just die already %s and do us all a favor.", mad_name(ch, victim));
      return str_dup(buf);
      break;
    }
  }

  void mad_comms(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
    printf_to_char(victim, "Your comms say, %s, '%s'\n\r", ch->pcdata->talk, comm_insult(ch, victim));
    else
    printf_to_char(victim, "Your comms say '%s'\n\r", comm_insult(ch, victim));
  }

  _DOFUN(do_psay) {
    CHAR_DATA *victim;
    int freq = 0;
    if (ch->hit < -7) {
      send_to_char("You're hurt too bad to talk.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }

    if (in_fight(ch) || ch->hit <= 0) {
      if (ch->fight_fast == FALSE) {
        send_to_char("You're a bit busy.\n\r", ch);
        return;
      }
    }

    if (get_phone(ch) == NULL) {
      send_to_char("You need a phone to do that.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Say what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You moan in your grave.\n\r", ch);
      return;
    }

    if (!cell_signal(ch))
    return;

    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    send_to_char("You transmit.\n\r", ch);

    if (IS_FLAG(ch->act, PLR_HIDE))
    do_function(ch, &do_unhide, "");

    do_function(ch, &do_say, argument);

    char rectalk[MSL], talktalk[MSL], tmp[MSL], blah[MSL];

    bool eataletter = FALSE;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '(') {
        eataletter = TRUE;
        rectotalk = TRUE;
      }
      else if (tmp[i] == ')') {
        rectotalk = FALSE;
      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else {
          if (eataletter == TRUE && tmp[i] == ' ') {
            eataletter = FALSE;
          }
          else {
            sprintf(blah, "%c", tmp[i]);
            strcat(talktalk, blah);
          }
        }
      }
    }
    argument = str_dup(talktalk);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);
      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (ch == victim)
      continue;

      bool can_hear = FALSE;
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->chan_status[i] == 2 && can_hear_freq(victim, ch->pcdata->chan_numbers[i])) {
          can_hear = TRUE;
          freq = ch->pcdata->chan_numbers[i];
        }
      }

      if (is_griefer(ch) && !is_griefer(victim))
      continue;

      if (is_griefer(victim) && !is_griefer(ch))
      continue;

      if (can_hear == FALSE)
      continue;

      if (in_haven(ch->in_room) != in_haven(victim->in_room))
      continue;

      if (is_deaf(victim))
      continue;

      if (victim->shape != SHAPE_HUMAN)
      continue;

      if (!cell_signal(victim))
      continue;

      if (room_in_school(ch->in_room->vnum) && IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      continue;

      if (is_helpless(victim) && get_phone(victim) == NULL)
      continue;

      if (IS_AFFECTED(victim, AFF_MAD) && number_percent() % 2 == 0) {
        mad_comms(ch, victim);
      }
      else {
        char buf[MSL];
        if (!IS_NPC(ch) && safe_strlen(rectalk) > 2) {
          printf_to_char(victim, "Your comms say, %s, '%s'\n\r", rectalk, static_text(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          sprintf(buf, "Your comms say, %s, '%s'\n\r", rectalk, static_text(
          ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          prp_rplog(victim, buf);
        }
        else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2) {
          printf_to_char(victim, "Your comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          sprintf(buf, "Your comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
          ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          prp_rplog(victim, buf);
        }
        else {
          printf_to_char(victim, "Your comms say '%s'\n\r", static_text(ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          sprintf(buf, "Your comms say '%s'\n\r", static_text(
          ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          prp_rplog(victim, buf);
        }
      }

      if (victim->recent_moved <= -300 && has_computer(victim) && get_skill(victim, SKILL_HACKING) >= 4 && number_percent() % 4 == 0 && get_phone(ch) != NULL)
      printf_to_char(victim, "(%d)\n\r", get_phone(ch)->value[0]);
    }

    if (number_percent() % 4 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5)
    printf_to_char(
    get_tracer(ch), "You intercept '%s' on frequency %d\n\r", mangle_text(ch, get_tracer(ch), ch->pcdata->speaking, argument), freq);

    if (pc_pop(ch->in_room) < 2)
    rpreward(ch, argument, TRUE, 1);
  }
  _DOFUN(do_donatechar) {
    char arg1[MSL];
    char buf[MSL];
    CHAR_DATA *victim;
    if (!IS_IMMORTAL(ch))
    return;

    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    argument = one_argument_nouncap(argument, arg1);
    d.original = NULL;

    if ((victim = get_char_world_pc(arg1)) == NULL) {
      log_string("DESCRIPTOR: Donatechar");

      if (!load_char_obj(&d, arg1)) {
        printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg1));
      stat(buf, &sb);
      victim = d.character;
    }
    else
    online = TRUE;

    if (IS_NPC(victim)) {
      send_to_char("Not on NPCS.\n\r", ch);
      return;
    }
    if (victim->pcdata->account == NULL) {
      send_to_char("Can't access account.\n\r", ch);
      return;
    }
    int oldamount = available_donated(victim);
    int amount = atoi(argument);
    if (amount < -10000 || amount > 100000)
    return;

    victim->pcdata->account->donated += amount;

    int xpbonus = UMIN(1000, amount);
    xpbonus = UMIN(xpbonus, 1000 - victim->pcdata->account->unrewarded_donation);

    victim->pcdata->account->rpxp += xpbonus * 100;
    victim->pcdata->account->xp += xpbonus * 100;
    victim->pcdata->account->unrewarded_donation += xpbonus;

    int temp = oldamount % 250;
    int oldtemp = (oldamount - temp);
    oldtemp /= 250;
    temp = available_donated(victim) % 250;
    int newtemp = (available_donated(victim) - temp);
    newtemp /= 250;
    temp = newtemp - oldtemp;
    victim->pcdata->colours += temp;

    temp = oldamount % 750;
    oldtemp = (oldamount - temp) / 750;
    temp = available_donated(victim) % 750;
    newtemp = (available_donated(victim) - temp) / 750;
    temp = newtemp - oldtemp;
    victim->pcdata->bandaids += temp;

    temp = oldamount % 1000;
    oldtemp = (oldamount - temp) / 1000;
    temp = available_donated(victim) % 1000;
    newtemp = (available_donated(victim) - temp) / 1000;
    temp = newtemp - oldtemp;
    victim->pcdata->renames += temp;

    victim->money += amount * 200;

    printf_to_char(ch, "You give %s %d credit.\n\r", victim->name, amount);

    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  _DOFUN(do_promify) {
    char arg1[MSL];
    char buf[MSL];
    CHAR_DATA *victim;
    if (!IS_IMMORTAL(ch))
    return;

    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    argument = one_argument_nouncap(argument, arg1);
    d.original = NULL;

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      log_string("DESCRIPTOR: Promify");

      if (!load_char_obj(&d, arg1)) {
        printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg1));
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg1));
      stat(buf, &sb);
      victim = d.character;
    }
    else
    online = TRUE;

    victim->pcdata->attract[ATTRACT_PROM] += 50;

    printf_to_char(ch, "You promty %s up.\n\r", victim->name);

    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  _DOFUN(do_telesay) {
    char buf[MSL];
    if (get_skill(ch, SKILL_ELECTROPATHIC) < 1)
    return;

    if (is_blind(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->connected_to != NULL && ch->pcdata->connection_stage == CONNECT_TALKING) {
      CHAR_DATA *victim = ch->pcdata->connected_to;
      if (is_deaf(victim))
      sprintf(buf, "Your phone says something'\n\r");
      else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
      sprintf(buf, "Your phone says, %s, '`o%s`x'\n\r", ch->pcdata->talk, static_text(ch, victim, argument));
      else
      sprintf(buf, "Your phone says '`o%s`x'\n\r", static_text(ch, victim, argument));

      page_to_char(wrap_string(buf, get_wordwrap(victim)), victim);

      CHAR_DATA *to;
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

          if (!is_gm(to))
          continue;

          if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
            printf_to_char(to, "%s telesays; '%s'\n\r", PERS(ch, to), argument);
          }
          else if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
            printf_to_char(to, "%s: %s telesays; '%s'\n\r", ch->in_room->name, PERS(ch, to), argument);
          }
        }
      }
      char rplog[MSL];
      sprintf(rplog, "%s: %s telesays; '%s'\n\r", ch->in_room->name, ch->name, argument);
      rp_log(rplog);
    }
  }

  _DOFUN(do_telefsay) {
    CHAR_DATA *victim;
    if (get_skill(ch, SKILL_ELECTROPATHIC) < 1)
    return;
    if (is_blind(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (!has_trust(ch, TRUST_COMMS, ch->faction)) {
      send_to_char("You don't have access to your faction's comms.\n\r", ch);
      return;
    }
    if (ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->attributes[FACTION_COMMS] == 0) {
      send_to_char("You don't have access to comms.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Say what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You moan in your grave.\n\r", ch);
      return;
    }
    if (!radio_signal(ch))
    return;

    if (!in_haven(ch->in_room))
    return;

    send_to_char("You transmit.\n\r", ch);

    char rectalk[MSL], talktalk[MSL], tmp[MSL], blah[MSL];

    bool eataletter = FALSE;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '(') {
        eataletter = TRUE;
        rectotalk = TRUE;
      }
      else if (tmp[i] == ')') {
        rectotalk = FALSE;
      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else {
          if (eataletter == TRUE && tmp[i] == ' ') {
            eataletter = FALSE;
          }
          else {
            sprintf(blah, "%c", tmp[i]);
            strcat(talktalk, blah);
          }
        }
      }
    }
    argument = str_dup(talktalk);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);
      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (IS_FLAG(victim->comm, COMM_NOCOMMS))
      continue;
      if (!has_trust(victim, TRUST_COMMS, ch->faction))
      continue;
      if (clan_lookup(ch->faction) == NULL)
      continue;
      if (clan_lookup(ch->faction)->alliance == 0 && !generic_faction_vnum(ch->faction))
      continue;
      if ((clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->alliance !=
            clan_lookup(ch->faction)->alliance) && (clan_lookup(victim->factiontwo) == NULL || clan_lookup(victim->factiontwo)->alliance !=
            clan_lookup(ch->faction)->alliance))
      continue;

      if (generic_faction_vnum(ch->faction) && ch->faction != victim->faction && ch->faction != victim->factiontwo) {
        continue;
      }

      if (is_deaf(victim))
      continue;

      if (victim->shape != SHAPE_HUMAN)
      continue;

      if (!radio_signal(victim))
      continue;

      if (get_world(ch) != get_world(victim))
      continue;

      if (is_helpless(victim) && get_phone(victim) == NULL)
      continue;

      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      printf_to_char(victim, "Your comms say, %s, '%s'\n\r", rectalk, static_text(ch, victim, argument));
      else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
      printf_to_char(victim, "Your comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(ch, victim, argument));
      else
      printf_to_char(victim, "Your comms say '%s'\n\r", static_text(ch, victim, argument));
    }

    CHAR_DATA *to;
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

        if (!is_gm(to))
        continue;

        if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
          printf_to_char(to, "%s telesays; '%s'\n\r", PERS(ch, to), argument);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
          printf_to_char(to, "%s: %s telesays; '%s'\n\r", ch->in_room->name, PERS(ch, to), argument);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "%s: %s telesays; '%s'\n\r", ch->in_room->name, ch->name, argument);
    rp_log(rplog);
    rpreward(ch, argument, TRUE, 1);
  }

  _DOFUN(do_telecsay) {
    CHAR_DATA *victim;
    if (get_skill(ch, SKILL_ELECTROPATHIC) < 1)
    return;
    if (is_blind(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Say what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You moan in your grave.\n\r", ch);
      return;
    }
    if (!cell_signal(ch))
    return;

    send_to_char("You transmit.\n\r", ch);

    char rectalk[MSL], talktalk[MSL], tmp[MSL], blah[MSL];

    bool eataletter = FALSE;

    sprintf(tmp, "%s", argument);
    rectalk[0] = 0;
    talktalk[0] = 0;
    bool rectotalk = FALSE;
    for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      if (tmp[i] == '(') {
        eataletter = TRUE;
        rectotalk = TRUE;
      }
      else if (tmp[i] == ')') {
        rectotalk = FALSE;
      }
      else {
        if (rectotalk == TRUE) {
          sprintf(blah, "%c", tmp[i]);
          strcat(rectalk, blah);
        }
        else {
          if (eataletter == TRUE && tmp[i] == ' ') {
            eataletter = FALSE;
          }
          else {
            sprintf(blah, "%c", tmp[i]);
            strcat(talktalk, blah);
          }
        }
      }
    }
    argument = str_dup(talktalk);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);
      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      bool can_hear = FALSE;
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->chan_status[i] == 2 && can_hear_freq(victim, ch->pcdata->chan_numbers[i]))
        can_hear = TRUE;
      }

      if (can_hear == FALSE)
      continue;

      if (is_deaf(victim))
      continue;

      if (victim->shape != SHAPE_HUMAN)
      continue;

      if (!in_haven(victim->in_room))
      continue;

      if (!cell_signal(victim))
      continue;

      if (is_helpless(victim) && get_phone(victim) == NULL)
      continue;

      if (!IS_NPC(ch) && safe_strlen(rectalk) > 2)
      printf_to_char(victim, "Your comms say, %s, '%s'\n\r", rectalk, static_text(ch, victim, argument));
      else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2)
      printf_to_char(victim, "Your comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(ch, victim, argument));
      else
      printf_to_char(victim, "Your comms say '%s'\n\r", argument);
    }

    CHAR_DATA *to;
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

        if (!is_gm(to))
        continue;

        if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
          printf_to_char(to, "%s telesays; '%s'\n\r", PERS(ch, to), argument);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && can_spy(to, ch)) {
          printf_to_char(to, "%s: %s telesays; '%s'\n\r", ch->in_room->name, PERS(ch, to), argument);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "%s: %s telesays; '%s'\n\r", ch->in_room->name, ch->name, argument);
    rp_log(rplog);
  }

  _DOFUN(do_911) {

    if (is_helpless(ch)) {
      send_to_char("You can't do that right now.\n\r", ch);
      return;
    }
    if (is_mute(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->comm, COMM_GAG)) {
      send_to_char("You're gagged.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Say what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You moan in your grave.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!cell_signal(ch))
    {
      send_to_char("You don't have a signal on your cell phone.\n\r", ch);
      return;
    }

    if (is_town_blackout())
    {
      send_to_char("That isn't working right now\n\r", ch);
      return;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *victim;

      victim = CH(d);

      if (victim == NULL || IS_NPC(victim))
      continue;

      if (d->connected != CON_PLAYING || victim == ch)
      continue;

      if (!IS_IMMORTAL(victim) && !cell_signal(victim))
      continue;

      if (!IS_IMMORTAL(victim) && victim->race != RACE_DEPUTY) {
        if (has_nonconsume(victim, 45024) && get_skill(victim, SKILL_HACKING) > 0)
        printf_to_char(victim, "[`B9`R1`B1`x] %s: '%s'\n\r", NAME(ch), argument);
        else {
          if (!IS_FLAG(victim->comm, COMM_NOCOMMS) && !is_deaf(victim) && cell_signal(victim)) {
            if (has_trust(victim, TRUST_COMMS, victim->faction) && clan_lookup(victim->faction) != NULL && clan_lookup(victim->faction)->attributes[FACTION_911] > 0 && clan_lookup(victim->faction)->attributes[FACTION_COMMS] > 0)
            printf_to_char(victim, "Your comms say after a double beep '%s'\n\r", argument);
            else if (has_trust(victim, TRUST_COMMS, victim->factiontwo) && clan_lookup(victim->factiontwo) != NULL && clan_lookup(victim->factiontwo)->attributes[FACTION_911] >
                0 && clan_lookup(victim->factiontwo)->attributes[FACTION_COMMS] >
                0)
            printf_to_char(victim, "Your comms say after a double beep '%s'\n\r", argument);
          }
        }
      }
      else
      printf_to_char(victim, "[`B9`R1`B1`x] %s: '%s'\n\r", NAME(ch), argument);
    }
    printf_to_char(ch, "[`B9`R1`B1`x] %s: '%s'\n\r", NAME(ch), argument);
  }

  _DOFUN(do_atemote) {
    CHAR_DATA *target;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    ROOM_INDEX_DATA *orig;
    if ((target = get_char_world(ch, arg1)) == NULL) {
      if ((target = get_char_world_pc(arg1)) == NULL) {
        send_to_char("You can't locate them..\n\r", ch);
        return;
      }
    }
    if ((!is_gm(ch) && !higher_power(ch)) || get_gmtrust(ch, target) < 1) {
      send_to_char("You can't locate them...\n\r", ch);
      return;
    }
    if (get_snooptrust(ch, target) < 2 || !can_spy(ch, target)) {
      send_to_char("You can't locate them...\n\r", ch);
      return;
    }

    orig = ch->in_room;

    if (safe_strlen(argument) < 3 || target->in_room == NULL || in_lodge(target->in_room) || base_room(target->in_room)) {
      send_to_char("You can't locate them.\n\r", ch);
      return;
    }

    char_from_room(ch);
    char_to_room(ch, target->in_room);
    do_function(ch, &do_emit, argument);
    char_from_room(ch);
    char_to_room(ch, orig);
  }

  void spyshow(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *to;
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

        if (!is_gm(to) && !higher_power(to))
        continue;

        if (to->in_room == ch->in_room && get_gmtrust(to, ch) > 0) {
          printf_to_char(to, "%s %s\n\r", PERS(ch, to), argument);
        }
        else if (IS_FLAG(to->act, PLR_SPYING) && get_snooptrust(to, ch) >= 3 && can_spy(to, ch)) {
          printf_to_char(to, "%s: %s %s\n\r", ch->in_room->name, PERS(ch, to), argument);
        }
      }
    }
    char rplog[MSL];
    sprintf(rplog, "%s: %s %s\n\r", ch->in_room->name, ch->name, argument);
    rp_log(rplog);
  }

  bool antagonist(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;
      if (victim == ch)
      continue;

      if (!str_cmp(ch->pcdata->last_sexed[0], victim->name))
      continue;
      if (!str_cmp(ch->pcdata->last_sexed[1], victim->name))
      continue;
      if (!str_cmp(ch->pcdata->last_sexed[2], victim->name))
      continue;

      if (victim->pcdata->captive_timer > 30000)
      continue;

      if (IS_FLAG(ch->comm, COMM_HOSTILE) && ((ch->spentexp > victim->spentexp + 250000)))
      return TRUE;

      if (is_pinned(victim) && !str_cmp(ch->name, victim->pcdata->pinned_by) && ((ch->spentexp > victim->spentexp + 250000)))
      return TRUE;

      if (IS_FLAG(victim->act, PLR_BOUND) && victim->desc != NULL && victim->pcdata->selfbondage == 0)
      return TRUE;
    }
    return FALSE;
  }

  bool active_prisoner(CHAR_DATA *ch) {
    if (!is_prisoner(ch))
    return FALSE;
    if (IS_AFFECTED(ch, AFF_DRAINED))
    return FALSE;

    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;
      if (victim == ch)
      continue;
      if (!is_prisoner(victim))
      return FALSE;

      if (is_prisoner(victim))
      return TRUE;
    }
    return FALSE;
  }

  int badass_value(CHAR_DATA *ch) {

    CHAR_DATA *victim;
    int value = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;
      if (victim == ch)
      continue;

      if (!str_cmp(ch->pcdata->last_sexed[0], victim->name))
      continue;
      if (!str_cmp(ch->pcdata->last_sexed[1], victim->name))
      continue;
      if (!str_cmp(ch->pcdata->last_sexed[2], victim->name))
      continue;

      if (victim->pcdata->captive_timer > 30000)
      continue;

      if (IS_FLAG(ch->comm, COMM_HOSTILE) && (ch->spentexp > victim->spentexp + 250000)) {
        if (victim->faction != 0 && clan_lookup(victim->faction) != NULL)
        value = UMAX(10, value);
        if (victim->factiontwo != 0 && clan_lookup(victim->factiontwo) != NULL)
        value = UMAX(10, value);
        if (victim->faction == 0 && is_super(victim))
        value = UMAX(3, value);
      }
      else if (is_pinned(victim) && !str_cmp(ch->name, victim->pcdata->pinned_by) && (ch->spentexp > victim->spentexp + 250000)) {
        if (victim->faction != 0 && clan_lookup(victim->faction) != NULL)
        value = UMAX(10, value);
        if (victim->factiontwo != 0 && clan_lookup(victim->factiontwo) != NULL)
        value = UMAX(10, value);
        if (victim->faction == 0 && is_super(victim))
        value = UMAX(3, value);
      }
      else if (is_helpless(victim) && victim->desc != NULL && victim->pcdata->selfbondage == 0 && !is_dreaming(victim)) {
        if (victim->faction != 0 && clan_lookup(victim->faction) != NULL)
        value = UMAX(10, value);
        if (victim->factiontwo != 0 && clan_lookup(victim->factiontwo) != NULL)
        value = UMAX(10, value);
        if (victim->faction == 0 && is_super(victim))
        value = UMAX(3, value);
      }
    }
    if ((ch->faction != 0 && clan_lookup(ch->faction) != NULL) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL))
    value = value * 2;

    return value;
  }

  void categorize_emote(CHAR_DATA *ch) {
    ch->pcdata->emotes[EMOTE_TOTAL]++;
    ch->pcdata->emotes[EMOTE_PAY]++;

    if (in_fight(ch))
    ch->pcdata->emotes[EMOTE_COMBAT]++;
    else if (is_dreaming(ch))
    ch->pcdata->emotes[EMOTE_DREAMING]++;
    else if (is_helpless(ch) || is_pinned(ch))
    ch->pcdata->emotes[EMOTE_VICTIM]++;
    else if (!is_masked(ch) && !IS_FLAG(ch->comm, COMM_CLOAKED) && !in_fight(ch) && public_room(ch->in_room) && !room_hostile(ch->in_room) && pc_pop(ch->in_room) > 3 && !IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(ch->comm, COMM_PRIVATE) && ch->shape == SHAPE_HUMAN) {
      ch->pcdata->emotes[EMOTE_PUBLIC]++;
    }
    else if (IS_FLAG(ch->comm, COMM_PRIVATE))
    ch->pcdata->emotes[EMOTE_SEX]++;
    else if (antagonist(ch)) {
      ch->pcdata->emotes[EMOTE_ANTAG]++;
      ch->pcdata->emotes[EMOTE_PACTANTAG]++;
    }
    else {
      ch->pcdata->emotes[EMOTE_SOCIAL]++;
    }
  }

  int naturalmod(CHAR_DATA *ch) {
    int amount = 40;

    if (ch->pcdata->account != NULL) {
      if (ch->played / 3600 < 100 && ch->pcdata->account->maxhours > ch->played / 3600 + 10)
      amount /= 2;
    }
    if (get_age(ch) < 19 && get_true_age(ch) < 19)
    amount /= 2;

    if (ch->pcdata->attract[ATTRACT_PROM] > 100)
    amount /= 2;

    if (ch->pcdata->bust > 8)
    amount /= 2;

    if (clothes_count(ch) < 40 && ch->played / 3600 > 50)
    amount /= 2;

    if (!housesleeper(ch))
    amount /= 2;

    if (ch->faction != 0)
    amount *= 3 / 2;

    if (get_skill(ch, SKILL_VIRGIN) > 0 && ch->pcdata->virginity_lost == 0)
    amount -= 5;

    amount = UMAX(amount, 5);

    return amount;
  }

  bool duplicate_emote(CHAR_DATA *ch, char *argument) {
    char arg1[MSL];
    char arg2[MSL];
    one_argument_nouncap(argument, arg1);
    one_argument_nouncap(ch->pcdata->last_emote, arg2);

    char comp1[100];
    char comp2[100];

    strncpy(comp1, arg1, 100);
    comp1[10] = '\0';
    strncpy(comp2, arg2, 100);
    comp2[10] = '\0';

    if (levenshtein_distance(comp1, comp2) < 3 && safe_strlen(ch->pcdata->last_emote) > 3)
    return TRUE;

    return FALSE;
  }

  bool running_calendar(CHAR_DATA *ch) {
    for (vector<STORY_TYPE *>::iterator ij = StoryVect.begin();
    ij != StoryVect.end(); ++ij) {
      if ((*ij)->valid == FALSE)
      continue;
      if (str_cmp(ch->name, (*ij)->author))
      continue;
      if ((*ij)->time <= current_time && (*ij)->time > current_time - (3600 * 6)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  void rpreward(CHAR_DATA *ch, char *argument, bool remote, int pop) {
    char log_buf[MSL];

    if (!IS_NPC(ch))
    ch->pcdata->time_since_action = 0;

    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return;

    if (!str_cmp(argument, ch->pcdata->last_emote) && safe_strlen(ch->pcdata->last_emote) > 2) {
      sprintf(log_buf, "Duplicate Emote 1: %s: %s:%s", ch->name, argument, ch->pcdata->last_emote);
      log_string(log_buf);

      free_string(ch->pcdata->last_emote);
      ch->pcdata->last_emote = str_dup(argument);

      return;
    }
    if (duplicate_emote(ch, argument)) {
      free_string(ch->pcdata->last_emote);
      ch->pcdata->last_emote = str_dup(argument);

      sprintf(log_buf, "Duplicate Emote 2: %s: %s", ch->name, argument);
      log_string(log_buf);

      return;
    }

    free_string(ch->pcdata->last_emote);
    ch->pcdata->last_emote = str_dup(argument);

    if (!is_gm(ch) && (strstr(argument, "OOC") != NULL || strstr(argument, " oocs ") != NULL || strstr(argument, " oocly ") != NULL))
    ch->rpexp -= 50;

    bool ownbase = FALSE;

    int length = safe_strlen(from_color(argument));

    int rpgain = 0;
    int karmagain = 0;

    if (ch->played / 3600 > 15 && safe_strlen(ch->pcdata->history) < 250 && !higher_power(ch) && !is_gm(ch) && ch->race != RACE_FANTASY) {
      if ((number_percent() % 7 == 0 && ch->played / 3600 < 50) || number_percent() % 43 == 0)
      send_to_char("You can't currently gain RPxp because you still don't have a history, type history to enter one.\n\r", ch);
      return;
    }

    if (!remote) {
      if (room_pop(ch->in_room) >= 2 && other_players(ch) == FALSE) {
        char buf[MSL];
        sprintf(buf, "%s is playing with themselves", ch->name);
        wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);
        return;
      }

      int invnum = 0;
      if (prop_from_room(ch->in_room) != NULL && prop_from_room(ch->in_room)->type == PROP_SHOP) {
        ch->pcdata->hangouttemp = prop_from_room(ch->in_room)->vnum;
        invnum = prop_from_room(ch->in_room)->vnum;
      }
      if (length > number_percent())
      categorize_emote(ch);

      if (ch->pcdata->process_timer > 3 && length > number_percent() && ch->pcdata->process_target == NULL)
      ch->pcdata->process_timer -= 2;

      if (invnum != 0 && invnum == ch->pcdata->hangoutone)
      length = length * 3 / 4;
      else if (invnum != 0 && invnum == ch->pcdata->hangouttwo)
      length = length * 4 / 5;

      if (ownbase == FALSE)
      give_influence(ch, UMAX(1, length / 50));

      int badass = badass_value(ch);
      if (badass > 0) {
        ch->pcdata->super_influence += badass * length / 150;
      }
      else if (badass < 0)
      ch->pcdata->super_influence += badass * length / 50;

      lower_fatigue(ch, length);
      if (safe_strlen(from_color(argument)) >= 100)
      rpgain = (int)(2 + (safe_strlen(from_color(argument)) / 100));
      else
      rpgain = (int)(1 + (safe_strlen(from_color(argument)) / 100));

      rpgain *= 10;

      rpgain = rpgain * (10 + UMIN(3, pop)) / 10;

      if ((IS_FLAG(ch->comm, COMM_PRIVATE) || !story_on(ch)))
      rpgain /= 2;

      if (ch->pcdata->availability == AVAIL_LOW)
      rpgain /= 2;

      if (!IS_FLAG(ch->comm, COMM_WHOINVIS))
      rpgain = rpgain * 6 / 5;

      if (invnum != 0 && invnum == ch->pcdata->hangoutone && invnum == ch->pcdata->hangouttwo && !ownbase)
      rpgain = rpgain * 3 / 4;

      gain_rpexp(ch, rpgain);

      if (ch->fcore != 0) {
        if (has_other_symbol(ch)) {
          give_respect(ch, (safe_strlen(from_color(argument)) / 100) * 5 * pop * -1, "RPing with opposite symbol.", ch->fcore);
        }
        else if (has_symbol_core(ch) && get_rank(ch, ch->fcore) < 4)
        give_respect(ch, (safe_strlen(from_color(argument)) / 100) * 5 * pop, "RPing", ch->fcore);
      }

      if (ch->fcult != 0) {
        if (has_other_symbol(ch)) {
          give_respect(ch, (safe_strlen(from_color(argument)) / 100) * 5 * pop * -1, "RPing with opposite symbol.", ch->fcult);
        }
        else if (has_symbol_cult(ch) && get_rank(ch, ch->fcult) < 4)
        give_respect(ch, (safe_strlen(from_color(argument)) / 100) * 5 * pop, "RPing", ch->fcult);
      }

      if (ch->fsect != 0) {
        if (has_other_symbol(ch)) {
          give_respect(ch, (safe_strlen(from_color(argument)) / 100) * 5 * pop * -1, "RPing with opposite symbol.", ch->fsect);
        }
        else if (has_symbol_sect(ch) && get_rank(ch, ch->fsect) < 4)
        give_respect(ch, (safe_strlen(from_color(argument)) / 100) * 5 * pop, "RPing", ch->fsect);
      }


      karmagain = length * 5 / 130;

      if (is_gm(ch))
      karmagain = karmagain * (10 + UMIN(5, pop)) / 10;
      else
      karmagain = karmagain * (10 + UMIN(3, pop)) / 10;

      if ((IS_FLAG(ch->comm, COMM_PRIVATE) || !story_on(ch)) && pop <= 2)
      karmagain /= 2;

      if (ownbase == TRUE)
      karmagain /= 2;

      if (!is_gm(ch) && !is_super(ch))
      karmagain = karmagain * 3 / 2;

      if (invnum != 0 && invnum == ch->pcdata->hangoutone && invnum == ch->pcdata->hangouttwo && !ownbase)
      karmagain = karmagain * 2 / 3;

      if (ch->pcdata->account != NULL && !is_gm(ch)) {
        if (alt_count(ch) == 1)
        ch->pcdata->account->pkarma =
        UMIN(45000 - ch->spentpkarma, ch->pcdata->account->pkarma + karmagain * 3);
        else
        ch->pcdata->account->pkarma =
        UMIN(45000 - ch->spentpkarma, ch->pcdata->account->pkarma + karmagain * 2);

        if (ch->pcdata->account->karmabank > 0) {
          karmagain = karmagain * 2 / 3;
          if (!is_super(ch))
          karmagain *= 2;
          ch->pcdata->account->karma +=
          UMIN(karmagain, ch->pcdata->account->karmabank);
          ch->pcdata->account->karmabank -=
          UMIN(karmagain, ch->pcdata->account->karmabank);
        }
      }
      int type = KARMA_OTHER;

      if (is_gm(ch)) {
        if (get_karma_plot(ch) != NULL && ch->pcdata->encounter_status != ENCOUNTER_ONGOING) {
          if (get_karma_plot(ch)->karmacap == 0 || get_karma_plot(ch)->earnedkarma < get_karma_plot(ch)->karmacap) {
            if (has_mystery(ch))
            type = KARMA_MYSTERY;
            else
            type = KARMA_ADVENTURE;
            karmagain *= 25;
          }
        }
        else if (ch->pcdata->karma_battery > 0) {
          type = KARMA_ENCOUNTER;
          karmagain *= 25;
        }
        else if (ch->pcdata->scheme_running != NULL) {
          karmagain *= 25;
          type = KARMA_SCHEME;
        }
        else if (in_world(ch) != WORLD_EARTH) {
          type = KARMA_AMBIANT;
          karmagain *= 5;
        }
        else if (crisis_runner(ch)) {
          type = KARMA_AMBIANT;
          karmagain *= 8;
        }

        karmagain = karmagain * average_solidity(ch->in_room) / 100;
      }
      else {
        if (safe_strlen(ch->pcdata->murder_name) > 1 && ch->pcdata->murder_timer > 1) {
          if (get_char_world_pc(ch->pcdata->murder_name) != NULL)
          give_karma(get_char_world_pc(ch->pcdata->murder_name), karmagain * 10, type);
          else
          offline_karma(ch->pcdata->murder_name, karmagain * 10);
        }
        else if (is_dreaming(ch) && running_calendar(ch)) {
          type = KARMA_DREAM;
          karmagain *= 2;
          if (goddreamer(ch))
          karmagain *= 3;
          give_karma(ch, karmagain, type);
        }
        karmagain = 0;
      }

      give_karma(ch, karmagain, type);

      return;
    }

    if (safe_strlen(argument) >= 100)
    rpgain = (int)(2 + (safe_strlen(from_color(argument)) / 100));
    else
    rpgain = (int)(1 + (safe_strlen(from_color(argument)) / 100));

    rpgain *= 5;

    gain_rpexp(ch, rpgain);
  }

  void hack_text(int from, char *argument, int tonumber, int destination) {
    bool phonefound = FALSE;
    OBJ_DATA *obj;

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE || obj->value[0] != tonumber)
      continue;
      else if (IS_SET(obj->extra_flags, ITEM_OFF))
      continue;
      else if (obj->carried_by != NULL && cell_signal(obj->carried_by) == FALSE)
      continue;
      else {
        phonefound = TRUE;
        break;
      }
    }
    if (!phonefound)
    return;

    if (safe_strlen(obj->material) > 18000)
    return;

    char buf[MSL];

    sprintf(buf, "%s\nText from %d to %d: %s\n", obj->material, from, destination, argument);
    free_string(obj->material);
    obj->material = str_dup(buf);
    if (obj->carried_by != NULL) {
      if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
        act("Your $p vibrates.", obj->carried_by, obj, NULL, TO_CHAR);
      }
      else {
        act("Your $p beeps.", obj->carried_by, obj, NULL, TO_CHAR);
        act("$n's $p beeps.", obj->carried_by, obj, NULL, TO_ROOM);
      }
    }
  }

  void spymessage(ROOM_INDEX_DATA *room, char *message) {
    CHAR_DATA *to;

    if (room == NULL)
    return;

    if (public_room(room))
    return;

    if (mist_room(room))
    return;

    if (is_dark(room))
    return;

    if (room_in_school(room->vnum))
    return;

    EXTRA_DESCR_DATA *ed;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("!bugs", ed->keyword))
      break;
    }
    if (!ed) {
      return;
    }
    char buf[MSL];
    char mess[MSL];
    char newmat[MSL];
    OBJ_DATA *obj;

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      obj = *it;

      if (obj->item_type != ITEM_PHONE)
      continue;
      else if (IS_SET(obj->extra_flags, ITEM_OFF))
      continue;

      if (obj->value[0] < 100)
      continue;

      to = NULL;
      if (obj->carried_by != NULL && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        to = obj->carried_by;
      }
      else {
        if (obj->in_obj != NULL && obj->in_obj->carried_by != NULL && !IS_SET(obj->in_obj->extra_flags, ITEM_WARDROBE))
        to = obj->in_obj->carried_by;
        else
        to = NULL;
      }
      if (to == NULL)
      continue;

      if (!cell_signal(to))
      continue;
      if (obj == NULL)
      continue;
      if (obj->material == NULL)
      continue;
      if (safe_strlen(obj->material) > 20000)
      continue;

      sprintf(buf, "%d", obj->value[0]);
      if (is_name(buf, ed->description)) {
        sprintf(mess, "From Camera at %s: %s\n\r", room->name, message);

        sprintf(newmat, "%s\n%s", obj->material, mess);
        free_string(obj->material);
        obj->material = str_dup(newmat);

        if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
          act("Your $p vibrates.", to, obj, NULL, TO_CHAR);
        }
        else {
          act("Your $p beeps.", to, obj, NULL, TO_CHAR);
          act("$n's $p beeps.", to, obj, NULL, TO_ROOM);
        }
        if (number_range(1, 5422) % 50 == 0) {
          free_string(ed->description);
          ed->description = str_dup("");
          send_to_char("The camera shorts out.\n\r", to);
          return;
        }
      }
    }
  }

  bool eligible_malady(CHAR_DATA *ch, EVENT_TYPE *event) {
    char targ1[MSL];
    one_argument_nouncap(event->target, targ1);
    if ((!str_cmp(targ1, "supernaturals") && is_super(ch)) || (!str_cmp(targ1, "naturals") && !is_super(ch)) || (!str_cmp(targ1, "vampires") && is_vampire(ch)) || (!str_cmp(targ1, "werewolves") && is_werewolf(ch)) || (!str_cmp(targ1, "faeborn") && is_faeborn(ch)) || (!str_cmp(targ1, "angelborn") && is_angelborn(ch)) || (!str_cmp(targ1, "demonborn") && is_demonborn(ch)) || (!str_cmp(targ1, "demigods") && is_demigod(ch)) || !str_cmp(targ1, "everyone"))
    return TRUE;
    return FALSE;
  }

  void disease_check(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_AFFECTED(victim, AFF_MALADY))
    return;
    if (is_undead(victim))
    return;
    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->type != EVENT_MALADY && (*it)->typetwo != EVENT_MALADY)
      continue;
      if (current_time > (*it)->active_time && current_time < (*it)->deactive_time) {
        char buf[MSL];
        sprintf(buf, "DISEASE CHECK: %s, %s", ch->name, victim->name);
        log_string(buf);

        if ((!str_cmp((*it)->author, ch->name)) || (IS_AFFECTED(ch, AFF_MALADY) && eligible_malady(victim, (*it)))) {
          AFFECT_DATA af;
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = 12 * 60 * 8;
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_MALADY;
          affect_to_char(victim, &af);
        }
      }
    }
  }

  bool in_relationship(CHAR_DATA *ch, CHAR_DATA *victim) {
    for (int x = 0; x < 10; x++) {
      if ((ch->pcdata->relationship_type[x] == REL_DATING || ch->pcdata->relationship_type[x] == REL_SPOUSE) && !str_cmp(ch->pcdata->relationship[x], victim->name))
      return TRUE;
      if ((victim->pcdata->relationship_type[x] == REL_DATING || victim->pcdata->relationship_type[x] == REL_SPOUSE) && !str_cmp(victim->pcdata->relationship[x], ch->name))
      return TRUE;
    }
    return FALSE;
  }
  void public_attention(CHAR_DATA *ch, CHAR_DATA *victim) {
    return;
    if (ch->pcdata->secret_recover <= 0)
    return;
    if (IS_NPC(ch) || IS_NPC(victim))
    return;
    if (IS_FLAG(ch->act, PLR_DEAD) || IS_FLAG(victim->act, PLR_DEAD))
    return;
    if (is_gm(ch) || is_ghost(ch) || is_animal(ch) || is_animal(victim) || is_gm(victim) || is_ghost(victim))
    return;
    if (ch == victim)
    return;
    if (!IS_SET(ch->in_room->room_flags, ROOM_WHEREVIS))
    return;
    if (!public_room(ch->in_room))
    return;
    if (!in_haven(ch->in_room))
    return;
    int val;
    if (victim->pcdata->spent_resources <= 0)
    val = ch->pcdata->secret_recover / 60;
    else {
      val = ch->pcdata->secret_recover / 20;
      if (val >= 15)
      send_to_char("(Your secrecy improves.)\n\r", victim);
    }
    val = UMAX(val, 1);
    ch->pcdata->secret_recover -= val;
    victim->pcdata->spent_resources -= val;
  }

  void give_attention(CHAR_DATA *ch, CHAR_DATA *victim) {
    int pop = 0;
    int occurs = 0;
    if (IS_NPC(ch) || IS_NPC(victim))
    return;
    if (IS_FLAG(ch->act, PLR_DEAD) || IS_FLAG(victim->act, PLR_DEAD))
    return;

    MATCH_TYPE *match = match_find(ch, victim);
    if(match != NULL)
    {
      if(ch->in_room == victim->in_room)
      match->rp_count++;
      else
      match->chat_count++;
    }

    if(IS_FLAG(ch->act, PLR_INFLUENCER) && ch->pcdata->influencer_bank > 0)
    {
      if(ch->in_room == victim->in_room)
      {
        give_intel(ch, 25);
        give_intel(victim, 5);
        ch->pcdata->influencer_bank -= 25;
      }
      else
      {
        give_intel(ch, 10);
        give_intel(victim, 2);
        ch->pcdata->influencer_bank -= 25;
      }
    }

    if (is_gm(ch) || is_ghost(ch) || is_animal(ch) || is_animal(victim) || is_gm(victim) || is_ghost(victim))
    return;
    if (ch == victim)
    return;

    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return;
    if (victim->in_room == NULL || victim->in_room->vnum < 300)
    return;

    if (social_standing(ch) < 75 && ch->played / 3600 > 50)
    return;

    if (ch->in_room != NULL && victim->in_room != NULL && ch->in_room == victim->in_room) {
      pop = pc_pop(ch->in_room);
    }
    else
    pop = 1;

    int tpop = pop;
    if (public_room(victim->in_room))
    pop += 3;

    int amount = ch->pcdata->influence_bank / 20;

    for (int i = 0; i < pop; i++)
    amount = amount * 11 / 10;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(victim->name, ch->pcdata->attention_history[i]))
      occurs++;
    }

    if (number_percent() % 7 == 0 && same_place(ch, victim)) {
      disease_check(ch, victim);
      disease_check(victim, ch);
    }
    if (in_relationship(ch, victim)) {
      occurs = UMIN(occurs, 1);
      amount /= 2;
    }
    if (tpop > 3) {
      if (occurs < 1 && number_percent() % 15 == 0)
      social_behave_mod(ch, 2, "giving attention to new person");
      else if (occurs > 4 && number_percent() % 15 == 0)
      social_behave_mod(ch, -1 * UMIN(occurs, tpop), "giving attention to only one person");
    }

    for (int i = 0; i < occurs; i++)
    amount = amount * 9 / 10;

    amount = UMAX(1, amount);
    give_influence(victim, amount);
    ch->pcdata->influence_bank -= amount * (10 + occurs) / 10;
    if (amount > 10)
    ch->pcdata->influence_bank += amount / 20;

    for (int i = 9; i > 0; i--) {
      free_string(ch->pcdata->attention_history[i]);
      ch->pcdata->attention_history[i] =
      str_dup(ch->pcdata->attention_history[i - 1]);
    }
    free_string(ch->pcdata->attention_history[0]);
    ch->pcdata->attention_history[0] = str_dup(victim->name);

    if (IS_FLAG(ch->comm, COMM_PRIVATE)) {
      free_string(ch->pcdata->privatepartner);
      ch->pcdata->privatepartner = str_dup(victim->name);
    }
  }

  void give_room_attention(CHAR_DATA *ch) {
    if (is_gm(ch) || is_ghost(ch) || is_animal(ch))
    return;

    if (IS_FLAG(ch->act, PLR_DEAD))
    return;

    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return;

    if (social_standing(ch) < 75 && ch->played / 3600 > 50)
    return;

    int amount;
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
      if (is_animal(victim) || is_gm(victim) || is_ghost(victim) || IS_FLAG(victim->comm, COMM_AFK))
      continue;
      if (ch == victim)
      continue;

      amount = ch->pcdata->influence_bank / 40;
      amount = UMAX(amount, 1);
      give_influence(victim, amount);
      ch->pcdata->influence_bank -= amount;
      ch->pcdata->influence_bank += amount / 20;
      if (IS_FLAG(ch->comm, COMM_PRIVATE) && IS_FLAG(victim->comm, COMM_PRIVATE)) {
        free_string(ch->pcdata->privatepartner);
        ch->pcdata->privatepartner = str_dup(victim->name);
      }
    }
  }

  _DOFUN(do_scream) {
    CHAR_DATA *to;
    if (ch->hit < -7) {
      send_to_char("You're hurt too bad to talk.\n\r", ch);
      return;
    }
    if (is_mute(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->comm, COMM_GAG)) {
      send_to_char("You're gagged.\n\r", ch);
      return;
    }

    if (ch->wounds >= 3 && !is_undead(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum == 60)
    return;

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0 && !is_dreaming(ch)) {
      send_to_char("You're unconcious.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_HIDE))
    do_function(ch, &do_unhide, "");

    send_to_char("You scream!\n\r", ch);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (to->in_room == NULL || ch->in_room == NULL)
        continue;
        if (is_dreaming(ch) && ch->pcdata->dream_link != to && ch != to && !IS_IMMORTAL(to))
        continue;
        if (is_deaf(to) && is_blind(to))
        continue;
        if (to->in_room != ch->in_room && !can_hear(to, ch, VOLUME_HISCREAMING) && !can_see_char_distance(to, ch, DISTANCE_MEDIUM))
        continue;

        if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
        continue;

        if ((to == ch))
        continue;

        if (!can_hear(to, ch, VOLUME_HISCREAMING)) {
          if (ch->in_room != to->in_room) {
            printf_to_char(to, "[%s]%s screams silently.\n\r", room_distance(to, ch->in_room), PERS(ch, to));
            continue;
          }
          else {
            printf_to_char(to, "%s screams silently.\n\r", PERS(ch, to));
            continue;
          }
        }
        if (can_see_char_distance(to, ch, DISTANCE_MEDIUM))
        printf_to_char(to, "%s screams!\n\r", speaker(ch, to));
        else {
          if (ch->sex == SEX_MALE) {
            send_to_char("You hear masculine screaming somewhere in the distance!\n\r", to);
          }
          else
          send_to_char("You hear feminine screaming somewhere in the distance!\n\r", to);
        }

        WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      }
    }
  }

  void note_reward(CHAR_DATA *ch, char *tostring, char *text) {
    if (is_name("all", tostring))
    return;

    char log_buf[MSL];
    int gain = safe_strlen(text) / 100;
    if (is_gm(ch)) {

      if (has_mystery(ch)) {
        int karmagain = gain * 5;
        if (get_karma_plot(ch) != NULL) {
          if (get_karma_plot(ch)->karmacap == 0 || get_karma_plot(ch)->earnedkarma < get_karma_plot(ch)->karmacap)
          karmagain *= 10;
        }
        give_karma(ch, karmagain, KARMA_MYSTERY);
        sprintf(log_buf, "%s gains %d karma from writing a note to: %s.\n\r", ch->name, karmagain, tostring);
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
      }

    }
    else {
      if (ch->pcdata->note_gain <= 0) {
        gain *= 5;
        gain_rpexp(ch, gain);
        sprintf(log_buf, "%s gains %d RPXP from a note to: %s.\n\r", ch->name, gain, tostring);
        ch->pcdata->note_gain = 10;
      }
      else
      sprintf(log_buf, "%s would have gained %d RPXP from a note to: %s.\n\r", ch->name, gain, tostring);

      wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
      log_string(log_buf);
    }
  }

  void research_reward(CHAR_DATA *ch, char *title, char *text) {
    char log_buf[MSL];
    int gain = safe_strlen(text) / 100;
    if (is_gm(ch)) {
      if (has_mystery(ch)) {
        int karmagain = gain * 5;
        if (get_karma_plot(ch) != NULL) {
          if (get_karma_plot(ch)->karmacap == 0 || get_karma_plot(ch)->earnedkarma < get_karma_plot(ch)->karmacap)
          karmagain *= 10;
        }
        give_karma(ch, karmagain, KARMA_MYSTERY);
        sprintf(log_buf, "%s gains %d karma from writing research titled: %s.\n\r", ch->name, karmagain, title);
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
      }
    }
    else {
      if (ch->pcdata->note_gain <= 0) {
        gain *= 5;
        gain_rpexp(ch, gain);
        sprintf(log_buf, "%s gains %d RPXP from research titled: %s.\n\r", ch->name, gain, title);
        ch->pcdata->note_gain = 10;
      }
      else
      sprintf(log_buf, "%s would have gained %d RPXP from research titled: %s.\n\r", ch->name, gain, title);

      wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
      log_string(log_buf);
    }
  }

  void retire_character(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_DEAD))
    return;
    if (ch->in_room == NULL || ch->in_room->vnum == 98)
    return;
    char buf[MSL];
    sprintf(buf, "%s %s %s", ch->pcdata->account->pastcharacters, ch->name, ch->pcdata->last_name);
    free_string(ch->pcdata->account->pastcharacters);
    ch->pcdata->account->pastcharacters = str_dup(buf);
  }

  _DOFUN(do_goals) {
    if (ch->pcdata->ci_editing == 12) {
      do_function(ch, &do_goal, argument);
      return;
    }
    char buf[MSL];
    if (safe_strlen(argument) < 2) {
      sprintf(buf, "`WGoals`x\n%s\n\r", ch->pcdata->char_goals);
      page_to_char(buf, ch);
      return;
    }
    if (!str_cmp(argument, "edit")) {
      string_append(ch, &ch->pcdata->char_goals);
      return;
    }
  }
  _DOFUN(do_secrets) {
    char buf[MSL];
    if (ch->pcdata->ci_editing == 23) {
      string_append(ch, &ch->pcdata->ci_long);
      return;
    }
    if (safe_strlen(argument) < 2) {
      sprintf(buf, "`WSecrets`x\n%s\n\r", ch->pcdata->char_secrets);
      page_to_char(buf, ch);
      return;
    }
    if (!str_cmp(argument, "edit")) {
      string_append(ch, &ch->pcdata->char_secrets);
      return;
    }
  }
  _DOFUN(do_fears) {
    char buf[MSL];
    if (in_fight(ch)) {
      sprintf(buf, "fear %s", argument);
      do_function(ch, &do_ability, buf);
      return;
    }
    if (safe_strlen(argument) < 2) {
      sprintf(buf, "`WFears`x\n%s\n\r", ch->pcdata->char_fears);
      page_to_char(buf, ch);
      return;
    }
    if (!str_cmp(argument, "edit")) {
      string_append(ch, &ch->pcdata->char_fears);
      return;
    }
  }

  bool is_abuse(char *string) {
    if (strcasestr(string, " cunt"))
    return TRUE;
    if (strcasestr(string, " CUNT"))
    return TRUE;
    if (strcasestr(string, " faggot"))
    return TRUE;
    if (strcasestr(string, " FAGGOT"))
    return TRUE;
    if (strcasestr(string, " nigger"))
    return TRUE;
    if (strcasestr(string, " NIGGER"))
    return TRUE;
    if (strcasestr(string, " ASSHOLE"))
    return TRUE;

    return FALSE;
  }

  void abuse_check(CHAR_DATA *ch, char *string) {
    if (is_abuse(string)) {
      if (ch->desc->account != NULL) {
        if (!IS_FLAG(ch->desc->account->flags, ACCOUNT_SPAMMER))
        SET_FLAG(ch->desc->account->flags, ACCOUNT_SPAMMER);
        free_string(troll_ip);
        troll_ip = str_dup(ch->desc->host);
      }
    }
  }

  bool can_rp_log(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch == NULL || victim == NULL)
    return FALSE;
    if (ch->in_room == NULL || victim->in_room == NULL)
    return FALSE;
    if (IS_NPC(ch) || victim->in_room == NULL)
    return FALSE;
    if (ch != victim && !is_dreaming(ch) && (!can_see_char_distance(ch, victim, DISTANCE_NEAR) || !can_hear(ch, victim, VOLUME_NORMAL)))
    return FALSE;
    if (is_dreaming(ch) && ch->pcdata->dream_room != victim->pcdata->dream_room)
    return FALSE;

    return TRUE;
  }

  void char_rplog(CHAR_DATA *ch, char *argument) {
    if (ch == NULL)
    return;
    if (!argument || safe_strlen(argument) < 3)
    return;
    if (ch->pcdata->rp_logging == 1) {
      int point = -1;
      for (int i = 0; i < 20; i++) {
        if (point == -1 && safe_strlen(ch->pcdata->rp_log[i]) < 25000)
        point = i;
      }
      if (point == -1) {
        send_to_char("Log full.\n\r", ch);
        return;
      }
      char logs[MSL];
      sprintf(logs, "%s\n\n%s", ch->pcdata->rp_log[point], argument);
      free_string(ch->pcdata->rp_log[point]);
      ch->pcdata->rp_log[point] = str_dup(logs);
      if (safe_strlen(ch->pcdata->rp_log[point]) >= 25000) {
        sprintf(logs, "%s\n\n`WSee log view %d to continue.`x\n\r", ch->pcdata->rp_log[point], point + 2);
        free_string(ch->pcdata->rp_log[point]);
        ch->pcdata->rp_log[point] = str_dup(logs);
      }
    }
    if (!is_gm(ch) && !IS_FLAG(ch->act, PLR_GUEST) && ch->in_room != NULL && ch->in_room->vnum > 200) {
      int newpoint;
      if (is_dreaming(ch))
      newpoint = ch->pcdata->dream_room;
      else
      newpoint = ch->in_room->vnum;

      FILE *fpout;
      char buf[MSL];
      sprintf(buf, "../prp/%s.txt", ch->name);
      if ((fpout = fopen(buf, "a")) == NULL) {
        perror(buf);
        return;
      }
      ROOM_INDEX_DATA *inroom = get_room_index(newpoint);
      ROOM_INDEX_DATA *oldroom = get_room_index(ch->pcdata->last_rp_room);
      if (inroom != NULL && (oldroom == NULL || (ch->pcdata->last_rp_room != newpoint && (!battleground(inroom) || !battleground(oldroom)) && (inroom->sector_type != SECT_FOREST || inroom->area->vnum != oldroom->area->vnum)))) {
        char tmp[MSL];
        char datestr[MSL];
        time_t east_time = current_time;

        if (oldroom != NULL && !is_dreaming(ch) && pc_pop(inroom) < 2) {
          fprintf(fpout, "%s moves to %s.\n\n", emote_name(ch), from_color(inroom->name));
        }
        else if (is_dreaming(ch) && oldroom != NULL && room_fantasy(inroom) == room_fantasy(oldroom))
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else if (!is_dreaming(ch) && oldroom != NULL && prop_from_room(inroom) == prop_from_room(oldroom))
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else if (!is_dreaming(ch) && oldroom != NULL && ch->pcdata->time_since_emote < 120 && get_dist(inroom->x, inroom->y, oldroom->x, oldroom->y) <= 5)
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else if (!is_dreaming(ch) && oldroom != NULL && inroom->area->vnum == 1)
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else {
          sprintf(tmp, "%s", (char *)ctime(&east_time));
          sprintf(datestr, "%c%c%c, %c%c%c %c%c, %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);
          fprintf(fpout, "\n\n\n\n\nIt is %s and %s is %s.\n%s\n%s\n\n", datestr, emote_name(ch), from_color(roomtitle(inroom, TRUE)), from_details(inroom->description), from_color(displaysun_room(inroom)));
        }
      }
      ch->pcdata->last_rp_room = newpoint;
      fprintf(fpout, "%s\n\n", from_color(argument));
      fclose(fpout);
    }
  }

  void prp_rplog(CHAR_DATA *ch, char *argument) {
    if (ch == NULL)
    return;
    if (!is_gm(ch) && !IS_FLAG(ch->act, PLR_GUEST) && ch->in_room != NULL && ch->in_room->vnum > 200) {
      int newpoint;
      if (is_dreaming(ch))
      newpoint = ch->pcdata->dream_room;
      else
      newpoint = ch->in_room->vnum;

      FILE *fpout;
      char buf[MSL];
      sprintf(buf, "../prp/%s.txt", ch->name);
      if ((fpout = fopen(buf, "a")) == NULL) {
        perror(buf);
        return;
      }
      ROOM_INDEX_DATA *inroom = get_room_index(newpoint);
      ROOM_INDEX_DATA *oldroom = get_room_index(ch->pcdata->last_rp_room);
      if (inroom != NULL && (oldroom == NULL || (ch->pcdata->last_rp_room != newpoint && (!battleground(inroom) || !battleground(oldroom)) && (inroom->sector_type != SECT_FOREST || inroom->area->vnum != oldroom->area->vnum)))) {
        char tmp[MSL];
        char datestr[MSL];
        time_t east_time = current_time;

        if (oldroom != NULL && !is_dreaming(ch) && pc_pop(inroom) < 2) {
          fprintf(fpout, "%s moves to %s.\n\n", emote_name(ch), from_color(inroom->name));
        }
        else if (is_dreaming(ch) && oldroom != NULL && room_fantasy(inroom) == room_fantasy(oldroom))
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else if (!is_dreaming(ch) && oldroom != NULL && prop_from_room(inroom) == prop_from_room(oldroom))
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else if (!is_dreaming(ch) && oldroom != NULL && ch->pcdata->time_since_emote < 120 && get_dist(inroom->x, inroom->y, oldroom->x, oldroom->y) <= 5)
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else if (!is_dreaming(ch) && oldroom != NULL && inroom->area->vnum == 1)
        fprintf(fpout, "%s moves to %s.\n%s\n\n", emote_name(ch), from_color(inroom->name), from_details(inroom->description));
        else {
          sprintf(tmp, "%s", (char *)ctime(&east_time));
          sprintf(datestr, "%c%c%c, %c%c%c %c%c, %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);
          fprintf(fpout, "\n\n\n\n\nIt is %s and %s is %s.\n%s\n%s\n\n", datestr, emote_name(ch), from_color(roomtitle(inroom, TRUE)), from_details(inroom->description), from_color(displaysun_room(inroom)));
        }
      }
      ch->pcdata->last_rp_room = newpoint;
      fprintf(fpout, "%s\n\n", from_color(argument));
      fclose(fpout);
    }
  }

  void pc_rp_log(CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument) {
    if (ch != NULL)
    logevent_check(ch, argument);
    CHAR_DATA *to;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        if (to->in_room == NULL)
        continue;

        if (ch != NULL && ch != to && !is_dreaming(ch) && (!can_see_char_distance(to, ch, DISTANCE_NEAR) || !can_hear(to, ch, VOLUME_NORMAL)))
        continue;
        if (ch != NULL && ch != to && is_dreaming(ch) && ch->pcdata->dream_room != to->pcdata->dream_room)
        continue;

        if (ch == NULL && to->in_room != room)
        continue;
        char_rplog(to, argument);
      }
    }
  }

  _DOFUN(do_availability) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if (!str_cmp(arg1, "high")) {
      ch->pcdata->availability = AVAIL_HIGH;
      send_to_char("You are now highly available.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "low")) {
      ch->pcdata->availability = AVAIL_LOW;
      send_to_char("You now have low availablity.\n\r", ch);
      return;
    }
    else {
      ch->pcdata->availability = AVAIL_NORMAL;
      send_to_char("You now have normal availablity.\n\r", ch);
      return;
    }
  }

  _DOFUN(do_locate) {
    do_function(ch, &do_where, "");
    return;

    send_to_char("`cLocating Characters:`x\n\r", ch);
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL || IS_NPC(vch) || vch == ch)
      continue;
      if (IS_FLAG(vch->comm, COMM_AFK))
      continue;
      if (vch->pcdata->availability == AVAIL_LOW)
      continue;
      if (vch->pcdata->availability == AVAIL_NORMAL && !destiny_linked(ch, vch))
      continue;
      if (is_helpless(vch))
      continue;
      if (room_hostile(vch->in_room))
      continue;
      if (in_fight(vch))
      continue;
      if ((clinic_patient(vch)) && !college_staff(ch, FALSE) && !clinic_staff(ch, FALSE)
          && !clinic_patient(ch) && !college_student(ch, FALSE))
      continue;
      bool blocked = FALSE;
      for (int i = 0; i < 50; i++) {
        for (int n = 0; n < 25; n++) {
          if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(ch->pcdata->account->characters[n], vch->pcdata->nowhere_characters[i])) {
              blocked = TRUE;
            }
          }
        }
      }
      if (blocked == TRUE)
      continue;
      show_char_to_char_0(vch, ch, 1);
      if (pc_pop(vch->in_room) > 1)
      printf_to_char(ch, "    +%d others.\n\r", pc_pop(vch->in_room) - 1);
    }
  }

  _DOFUN(do_lure) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg2);
    int type = 0;
    if (!str_cmp(arg2, "oocly"))
    type = 1;
    else if (!str_cmp(arg2, "supernaturally"))
    type = 2;
    else if (!str_cmp(arg2, "destiny"))
    type = 3;
    else {
      send_to_char("Syntax: Lure (character) oocly/supernaturally/destiny (reason)\n\r", ch);
      return;
    }
    if (safe_strlen(argument) < 3) {
      send_to_char("Syntax: Lure (character) oocly/supernaturally/destiny (reason)\n\r", ch);
      return;
    }
    if (type == 2 && ch->pcdata->tcooldown > 0) {
      send_to_char("You did that too recently.\n\r", ch);
      return;
    }
    if (type == 2 && ch->faction == 0 && get_skill(ch, SKILL_HYPNOTISM) < 1 && arcane_focus(ch) < 1) {
      send_to_char("You lack the ability to do that.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You lack the ability to do that.\n\r", ch);
      return;
    }

    CHAR_DATA *victim;
    if ((victim = get_char_world_pc_noname(ch, arg1)) == NULL) {
      if ((victim = get_char_world_pc(arg1)) == NULL) {

        if (type == 2) {
          send_to_char("Lure failed.\n\r", ch);
          return;
        }
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    if (IS_NPC(victim) || IS_FLAG(victim->comm, COMM_AFK) || (victim->pcdata->availability != AVAIL_HIGH && !IS_FLAG(victim->comm, COMM_WHEREVIS))) {
      if (type == 2) {
        send_to_char("Lure failed.\n\r", ch);
        return;
      }
    }

    send_to_char("Done.\n\r", ch);
    if (is_dreaming(ch) && in_fantasy(ch) != NULL) {
      if (type == 1)
      printf_to_char(
      victim, "You might want to head to %s(%s), possibly because %s.\n\r", roomtitle(get_room_index(ch->pcdata->dream_room), TRUE), in_fantasy(ch)->name, argument);
      if (type == 3)
      printf_to_char(victim, "Fate conspires to make you want to head to %s(%s), possibly because %s.\n\r", roomtitle(get_room_index(ch->pcdata->dream_room), TRUE), in_fantasy(ch)->name, argument);
      if (type == 2) {
        use_lifeforce(ch, 100, "Lure");
        if (get_tier(ch) > get_tier(victim) && get_skill(ch, SKILL_HYPNOTISM) + get_skill(ch, SKILL_PERSUADE) + arcane_focus(ch) > 3)
        printf_to_char(
        victim, "You really want to head to %s(%s), possibly because %s.\n\r", roomtitle(get_room_index(ch->pcdata->dream_room), TRUE), in_fantasy(ch)->name, argument);
        else
        printf_to_char(victim, "You want to head to %s(%s), possibly because %s.\n\r", roomtitle(get_room_index(ch->pcdata->dream_room), TRUE), in_fantasy(ch)->name, argument);
      }
    }
    else {
      if (type == 1)
      printf_to_char(
      victim, "You might want to head to [%s]%s, possibly because %s.\n\r", room_distance(victim, ch->in_room), roomtitle(ch->in_room, TRUE), argument);
      if (type == 3)
      printf_to_char(victim, "Fate conspires to make you want to head to [%s]%s, possibly because %s.\n\r", room_distance(victim, ch->in_room), roomtitle(ch->in_room, TRUE), argument);
      if (type == 2) {
        use_lifeforce(ch, 10, "Lure");
        if (get_tier(ch) > get_tier(victim) && get_skill(ch, SKILL_HYPNOTISM) + arcane_focus(ch) > 3)
        printf_to_char(
        victim, "You really want to head to [%s]%s, possibly because %s.\n\r", room_distance(victim, ch->in_room), roomtitle(ch->in_room, TRUE), argument);
        else
        printf_to_char(victim, "You want to head to [%s]%s, possibly because %s.\n\r", room_distance(victim, ch->in_room), roomtitle(ch->in_room, TRUE), argument);
      }
    }
    if (type == 2)
    ch->pcdata->tcooldown = 45;
  }

  void dream_message(CHAR_DATA *ch, char *message) {
    DESCRIPTOR_DATA d;
    CHAR_DATA *victim = NULL;
    char buf[MSL];
    struct stat sb;

    bool online = FALSE;
    for (vector<PHONEBOOK_TYPE *>::iterator it = PhoneVect.begin();
    it != PhoneVect.end(); ++it) {
      online = FALSE;
      d.original = NULL;
      if ((victim = get_char_world_pc((*it)->owner)) != NULL) {
        online = TRUE;
      }
      else {
        if (!load_char_obj(&d, (*it)->owner)) {
          continue;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize((*it)->owner));
        stat(buf, &sb);
        victim = d.character;
      }
      if (victim == NULL || IS_NPC(victim))
      continue;

      if (get_skill(victim, SKILL_TOUCHED) > 0 && get_gmtrust(ch, victim) > 0) {
        if (safe_strlen(victim->pcdata->nightmare) < 4) {
          sprintf(buf, "%s\n\r", message);
        }
        else
        sprintf(buf, "%s\n\n%s\n\r", victim->pcdata->nightmare, message);

        free_string(victim->pcdata->nightmare);
        victim->pcdata->nightmare = str_dup(buf);
      }
      else if (number_percent() % 5 == 0 && (get_skill(victim, SKILL_TOUCHED) > 0 || get_gmtrust(ch, victim) > 0)) {
        if (safe_strlen(victim->pcdata->nightmare) < 4) {
          sprintf(buf, "%s\n\r", message);
        }
        else
        sprintf(buf, "%s\n\n%s\n\r", victim->pcdata->nightmare, message);

        free_string(victim->pcdata->nightmare);
        victim->pcdata->nightmare = str_dup(buf);
      }
      save_char_obj(victim, FALSE, FALSE);
      if (!online)
      free_char(victim);
    }
  }

  _DOFUN(do_saycolor) {
    if (!is_number(argument) || atoi(argument) < 0 || atoi(argument) > 255) {
      send_to_char("Syntax: saycolor (000-255)\n\r", ch);
      return;
    }
    free_string(ch->pcdata->verbal_color);
    ch->pcdata->verbal_color = str_dup(argument);
    printf_to_char(ch, "`%sVerbal color set.`x\n\r", ch->pcdata->verbal_color);
    return;
  }

  _DOFUN(do_senddream) {
    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("Only SRs and Higher Powers can do that.\n\r", ch);
      return;
    }
    if (safe_strlen(argument) < 4) {
      send_to_char("Syntax: senddream (dream message)\n\r", ch);
      return;
    }
    dream_message(ch, argument);
    send_to_char("Dream sent\n\r", ch);
    return;
  }
  size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *response) {
    size_t totalSize = size * nmemb;
    response->append(static_cast<char *>(contents), totalSize);
    return totalSize;
  }

  bool isTextFlagged(const std::string &text) {
    return FALSE;
    try {
      std::string apiKey = "";

      std::string url = "https://api.openai.com/v1/moderations";
      std::string authorizationHeader = "Authorization: Bearer " + apiKey;

      // Build the JSON request payload
      std::string requestData = R"({"input": ")" + text +R"("})";
      std::string response;
      CURL *curl = curl_easy_init();
      if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authorizationHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
          long httpCode = 0;
          curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

          if (httpCode == 200) {
            rapidjson::Document document;
            document.Parse(response.c_str());

            // Check if the parsing was successful
            if (!document.HasParseError() && document.IsObject()) {
              // Access the "results" array
              const rapidjson::Value &results = document["results"];

              // Assuming the "results" array contains only one element
              if (results.IsArray() && results.Size() == 1) {
                // Access the first element of the "results" array
                const rapidjson::Value &result = results[0];
                // Check if the "flagged" value exists and is of type bool
                if (result.HasMember("flagged") && result["flagged"].IsBool()) {
                  // Get the boolean value of "flagged"
                  bool flagged = result["flagged"].GetBool();
                  return flagged;
                }
                else {
                  std::cout
                  << "Invalid JSON format: Unable to extract flagged value."
                  << std::endl;
                }
              }
              else {
                std::cout << "Invalid JSON format: Unexpected number of results."
                << std::endl;
              }
            }
            else {
              std::cout << "Invalid JSON format: Parsing error." << std::endl;
            }
          }
          else {
            std::cerr << "HTTP request failed with status code: " << httpCode
            << std::endl;
          }
        }
        else {
          std::cerr << "cURL request failed: " << curl_easy_strerror(res)
          << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
      }
      else {
        std::cerr << "Failed to initialize cURL" << std::endl;
      }

    } catch (...) {
      std::cerr << "An unknown error occurred" << std::endl;
    }

    return false;
  }

  _DOFUN(do_testflagging) {
    bool flag = isTextFlagged(argument);
    if (flag == false) {
      send_to_char("Not Flagged\n\r", ch);
    }
    else {
      send_to_char("Flagged\n\r", ch);
    }
  }

  void recollect_char(CHAR_DATA *ch, char * tname, char * message)
  {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(tname)) !=
        NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, tname)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(tname));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    sprintf(buf, "You recollect something from your past with %s, %s.\n\r", PERS(ch, victim), message);
    char_rplog(victim, buf);

    if(online)
    send_to_char(buf, victim);
    else
    {
      sprintf(buf, "%s\nYou recollect something from your past with %s, %s.\n\r", victim->pcdata->messages, PERS(ch, victim), message);
      free_string(victim->pcdata->messages);
      victim->pcdata->messages = str_dup(buf);
    }
    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  _DOFUN(do_recollect) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char buf[MSL];
    char combi[MSL];

    if(argument == NULL || strlen(argument) < 2)
    {
      send_to_char("Syntax: Recollect (person) (memory) or Recollect (memory)\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      sprintf(combi, "%s %s", arg1, argument);

      for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
      it != WeeklyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!str_cmp((*it)->surname, ch->pcdata->last_name))
        {
          recollect_char(ch, (*it)->charname, str_dup(combi));
          continue;
        }
        bool relationship = FALSE;
        for (int i = 0; i < 10; i++) {
          if(!str_cmp(ch->pcdata->relationship[i], (*it)->charname))
          relationship = TRUE;
        }
        if(relationship == TRUE)
        {
          recollect_char(ch, (*it)->charname, str_dup(combi));
          continue;
        }
        if(ff_surname(ch->pcdata->last_name) && ff_surname((*it)->surname))
        {
          recollect_char(ch, (*it)->charname, str_dup(combi));
          continue;
        }

      }
      sprintf(buf, "You recollect %s %s.\n\r", arg1, argument);
      send_to_char(buf, ch);
      char_rplog(ch, buf);
      return;
    }
    else
    {
      sprintf(buf, "You recollect %s.\n\r", argument);
      send_to_char(buf, ch);
      char_rplog(ch, buf);
      sprintf(buf, "You recollect something from your past with %s, %s.\n\r", PERS(ch, victim), argument);
      send_to_char(buf, victim);
      char_rplog(victim, buf);
    }
  }

  char * name_from_number(CHAR_DATA *ch, int number)
  {
    for (int i = 0; i < 50; i++) {
      if(number == ch->pcdata->speed_numbers[i])
      return str_dup(ch->pcdata->speed_names[i]);
    }
    char rback[20];
    sprintf(rback, "%d", number);
    return str_dup(rback);
  }

  int number_from_name(CHAR_DATA *ch, char * argument)
  {
    for (int i = 0; i < 50; i++) {
      if(!str_cmp(argument, ch->pcdata->speed_names[i]))
      return ch->pcdata->speed_numbers[i];
    }
    return atoi(argument);
  }

  _DOFUN(do_grouptext)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if (get_phone(ch) == NULL) {
      send_to_char("You do not seem to have a phone.\n\r", ch);
      return;
    }
    int from;
    from = get_phone(ch)->value[0];
    if(from <= 0)
    {
      send_to_char("You do not seem to have a phone.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "create"))
    {
      if(strlen(argument) < 2)
      {
        send_to_char("Syntax: grouptext create (name)\n\r", ch);
        return;
      }

      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
      it != GTextVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if(!str_cmp((*it)->tname, argument))
        {
          bool inchain = FALSE;
          for(int i=0;i<10;i++)
          {
            if((*it)->pnumber[i] == from)
            inchain = TRUE;
          }
          if(inchain == TRUE)
          {
            send_to_char("You are already in a group with that name.\n\r", ch);
            return;
          }
        }
      }
      GROUPTEXT_TYPE *gtext;
      gtext = new_grouptext();
      free_string(gtext->tname);
      gtext->tname = str_dup(argument);
      gtext->last_msg = current_time;
      gtext->pnumber[0] = from;
      GTextVect.push_back(gtext);
      send_to_char("Group created.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "list"))
    {
      send_to_char("Group Texts:\n\r", ch);
      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
      it != GTextVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        bool inchain = FALSE;
        for(int i=0;i<10;i++)
        {
          if((*it)->pnumber[i] == from)
          inchain = TRUE;
        }
        if(inchain == TRUE)
        {
          printf_to_char(ch, "%s\n\r", (*it)->tname);
        }
      }
      return;
    }
    if(!str_cmp(arg1, "info"))
    {
      if(strlen(argument) < 2)
      {
        send_to_char("Syntax: grouptext info (name)\n\r", ch);
        return;
      }
      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
      it != GTextVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if(!str_cmp((*it)->tname, argument))
        {
          bool inchain = FALSE;
          for(int i=0;i<10;i++)
          {
            if((*it)->pnumber[i] == from)
            inchain = TRUE;
          }
          if(inchain == TRUE)
          {
            printf_to_char(ch, "Group Text: %s\n\r", (*it)->tname);
            printf_to_char(ch, "Members:\n\r");
            for(int i=0;i<10;i++)
            {
              if((*it)->pnumber[i] > 0)
              {
                printf_to_char(ch, "%s\n\r", name_from_number(ch, (*it)->pnumber[i]));
              }
            }
            return;
          }
        }
      }
      send_to_char("You are not in a group with that name.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "addnumber"))
    {
      if(strlen(argument) < 2)
      {
        send_to_char("Syntax: grouptext addnumber (name) (number)\n\r", ch);
        return;
      }

      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int addnum = number_from_name(ch, argument);
      if(addnum < 1 || addnum > 9107171)
      {
        send_to_char("Not a valid number to add.\n\r", ch);
        return;
      }
      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
      it != GTextVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if(!str_cmp((*it)->tname, arg2))
        {
          bool inchain = FALSE;
          for(int i=0;i<10;i++)
          {
            if((*it)->pnumber[i] == from)
            inchain = TRUE;
          }
          bool already = FALSE;
          for(int i=0;i<10;i++)
          {
            if((*it)->pnumber[i] == addnum)
            already = TRUE;
          }
          if(already == TRUE)
          {
            send_to_char("That number is already in the group.\n\r", ch);
            return;
          }
          if(inchain == TRUE)
          {
            for(int i=0;i<10;i++)
            {
              if((*it)->pnumber[i] == 0)
              {
                (*it)->pnumber[i] = addnum;
                send_to_char("Number added.\n\r", ch);
                return;
              }
            }
            send_to_char("Group is full.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("You are not in a group with that name.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "leave"))
    {
      if(strlen(argument) < 2)
      {
        send_to_char("Syntax: grouptext leave (name)\n\r", ch);
        return;
      }
      for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
      it != GTextVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if(!str_cmp((*it)->tname, argument))
        {
          bool inchain = FALSE;
          for(int i=0;i<10;i++)
          {
            if((*it)->pnumber[i] == from)
            inchain = TRUE;
          }
          if(inchain == TRUE)
          {
            for(int i=0;i<10;i++)
            {
              if((*it)->pnumber[i] == from)
              {
                (*it)->pnumber[i] = 0;
                send_to_char("You have left the group.\n\r", ch);
                return;
              }
            }
          }
        }
      }
      send_to_char("You are not in a group with that name.\n\r", ch);
      return;


    }
    send_to_char("Syntax: grouptext create (name)/list/info/addnumber\n\r", ch);
    return;
  }


  _DOFUN(do_coinflip)
  {
    static char stat1[MAX_STRING_LENGTH];
    static char stat2[MAX_STRING_LENGTH];
    char *separator = strstr(argument, " or ");
    if (separator != NULL) {
      // Calculate the length of the text before " or "
      size_t beforeLength = separator - argument;

      // Copy the text before " or "
      strncpy(stat1, argument, beforeLength);
      stat1[beforeLength] = '\0';  // Null-terminate the string

      // Copy the text after " or "
      strcpy(stat2, separator + 4);
      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if (to == NULL || to->in_room == NULL || to->in_room != ch->in_room || IS_NPC(to))
        continue;

        if (!is_fatesensitive(to))
        continue;

        if (to == ch)
        continue;

        free_string(to->pcdata->luck_string_one);
        to->pcdata->luck_string_one = str_dup(stat1);
        free_string(to->pcdata->luck_string_two);
        to->pcdata->luck_string_two = str_dup(stat2);

        to->pcdata->luck_type = 1;
        to->pcdata->luck_character = ch;
        printf_to_char(to, "It is a coin flip whether %s does one of two things, your fae blood can influence how that coin lands, use 1 or 2 to decide.\n1) %s\n2) %s\n\r", PERS(ch, to), stat1, stat2);
      }
      send_to_char("Done.\n\r", ch);
      // Print or use the two separate strings as needed
    }
    else {
      send_to_char("Syntax: coinflip (text) or (text)\n\r", ch);
      // " or " not found in the argument
      return;
    }

  }

  _DOFUN(do_luck)
  {
    static char stat1[MAX_STRING_LENGTH];
    static char stat2[MAX_STRING_LENGTH];
    char *separator = strstr(argument, " or ");
    if (separator != NULL) {
      // Calculate the length of the text before " or "
      size_t beforeLength = separator - argument;

      // Copy the text before " or "
      strncpy(stat1, argument, beforeLength);
      stat1[beforeLength] = '\0';  // Null-terminate the string

      // Copy the text after " or "
      strcpy(stat2, separator + 4);
      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if (to == NULL || to->in_room == NULL || to->in_room != ch->in_room || IS_NPC(to))
        continue;

        if (!is_fatesensitive(to))
        continue;

        if (to == ch)
        continue;

        free_string(to->pcdata->luck_string_one);
        to->pcdata->luck_string_one = str_dup(stat1);
        free_string(to->pcdata->luck_string_two);
        to->pcdata->luck_string_two = str_dup(stat2);

        to->pcdata->luck_type = 2;
        to->pcdata->luck_character = ch;
        printf_to_char(to, "%s's luck will determine if one of two things happens, your fae blood can influence which it is, use 1 or 2 to decide.\n1) %s\n2) %s\n\r", PERS(ch, to), stat1, stat2);
      }
      send_to_char("Done.\n\r", ch);
    }
    else {
      send_to_char("Syntax: luck (text) or (text)\n\r", ch);
      // " or " not found in the argument
      return;
    }

  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

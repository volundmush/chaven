#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "lookup.h"
#include "recycle.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define HEDIT(fun) bool fun(CHAR_DATA *ch, char *argument)

#define MIN_HEDIT_SECURITY 5

  extern HELP_AREA *had_list;

  const struct olc_cmd_type hedit_table[] = {
    /*	{	command		function	}, */

    {"commands", show_commands}, {"create", hedit_new}, {"delete", hedit_delete}, {"keyword", hedit_keyword}, {"level", hedit_level}, {"list", hedit_list}, {"online", hedit_online}, {"seealso", hedit_see_also}, {"show", hedit_show}, {"text", hedit_text}, {"type", hedit_type}, {"?", show_help}, 
    {NULL, 0}};

  /*
* Help Editor - kermit 1/98
* Modified by Scaelorn 2000-12-11
*/
  HEDIT(hedit_show) {
    HELP_DATA *help;
    char buf[MSL];

    EDIT_HELP(ch, help);

    sprintf(buf, "Keyword : [%s]\n\rLevel   : [%d]\n\rOnline  : [%s]\n\rDelete  : [%s]\n\rType    : [%s]\n\rSee Also: [%s]\n\rText    :\n\r\n\r%s\n\r", help->keyword, help->level, help->online ? "Yes" : "No", help->del ? "Yes" : "No", hfile_type_table[help->type], help->see_also, help->text);
    page_to_char(buf, ch);

    return FALSE;
  }

  HEDIT(hedit_type) {
    HELP_DATA *help;
    int type = 0;

    EDIT_HELP(ch, help);

    if ((type = hfile_type_lookup(argument)) == -1) {
      send_to_char("Syntax: type [other/immortal/skill/command/class/race/form/weave]\n\r", ch);
      return FALSE;
    }

    help->type = type;
    printf_to_char(ch, "Helpfile type has been set to %s.\n\r", hfile_type_table[type]);
    return TRUE;
  }

  HEDIT(hedit_level) {
    HELP_DATA *help;
    int lev;

    EDIT_HELP(ch, help);

    if (IS_NULLSTR(argument) || !is_number(argument)) {
      send_to_char("Syntax :  level [-1..MAX_LEVEL]\n\r", ch);
      return FALSE;
    }

    lev = atoi(argument);

    if (lev < -1 || lev > MAX_LEVEL) {
      printf_to_char(ch, "HEdit : Level must be between -1 and %d.\n\r", MAX_LEVEL);
      return FALSE;
    }

    help->level = lev;
    send_to_char("Level set.\n\r", ch);
    return TRUE;
  }

  HEDIT(hedit_see_also) {
    HELP_DATA *help;

    EDIT_HELP(ch, help);

    if (IS_NULLSTR(argument)) {
      send_to_char("Syntax: seealso [helpfile1, helpfile2, helpfile3] DON'T FORGET THE COMMAS!\n\r", ch);
      return FALSE;
    }

    free_string(help->see_also);
    help->see_also = str_dup(argument);
    send_to_char("The Helpfile Refrences have been Set\n\r", ch);

    return TRUE;
  }

  HEDIT(hedit_keyword) {
    HELP_DATA *help;

    EDIT_HELP(ch, help);

    if (IS_NULLSTR(argument)) {
      send_to_char("Syntax : keyword [keyword(s)]\n\r", ch);
      return FALSE;
    }

    free_string(help->keyword);
    help->keyword = str_dup(argument);

    send_to_char("Keyword(s) set.\n\r", ch);
    return TRUE;
  }

  HEDIT(hedit_new) {
    HELP_DATA *pHelp;

    if (argument[0] == '\0') {
      send_to_char("Syntax: hedit create [keyword(s)]\n\r", ch);
      return FALSE;
    }

    pHelp = new_help();
    pHelp->keyword = str_dup(argument);
    pHelp->next = help_first;
    help_first = pHelp;
    ch->desc->pEdit = (void *)pHelp;

    send_to_char("New Help Entry Created.\n\r", ch);
    return TRUE;
  }

  HEDIT(hedit_text) {
    HELP_DATA *help;

    EDIT_HELP(ch, help);

    if (!IS_NULLSTR(argument)) {
      send_to_char("Syntax : text\n\r", ch);
      return FALSE;
    }
    // send_to_char(help->text, ch);
    string_append(ch, &help->text);

    return TRUE;
  }

  HEDIT(hedit_delete) {
    HELP_DATA *pHelp;
    char buf[MSL];

    EDIT_HELP(ch, pHelp);

    if (!IS_IMMORTAL(ch)) {
      send_to_char("You must contact palin to delete this file.\n\r", ch);
      return FALSE;
    }

    pHelp->del = pHelp->del ? FALSE : TRUE;
    sprintf(buf, "You have %s this help for deletion!\n\r", pHelp->del ? "MARKED" : "UNMARKED");
    send_to_char(buf, ch);
    return TRUE;
  }

  HEDIT(hedit_online) {
    HELP_DATA *pHelp;
    char buf[MSL];

    EDIT_HELP(ch, pHelp);

    pHelp->online = pHelp->online ? FALSE : TRUE;
    sprintf(buf, "You have %s this help for deletion!\n\r", pHelp->online ? "MARKED" : "UNMARKED");
    send_to_char(buf, ch);
    return TRUE;
  }

  void hedit(CHAR_DATA *ch, char *argument) {
    char command[MIL];
    char arg[MIL];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (ch->pcdata->security < 5) {
      send_to_char("HEdit: Insufficient security to modify code\n\r", ch);
      edit_done(ch);
    }

    if (!str_cmp(command, "done")) {
      edit_done(ch);
      return;
    }

    if (command[0] == '\0') {
      hedit_show(ch, argument);
      return;
    }

    for (cmd = 0; hedit_table[cmd].name != NULL; cmd++) {
      if (!strcmp(command, hedit_table[cmd].name)) {
        (*hedit_table[cmd].olc_fun)(ch, argument);
        return;
      }
    }

    interpret(ch, arg);
    return;
  }

  _DOFUN(do_hedit) {
    HELP_DATA *pHelp;
    char arg1[MIL];
    bool found = FALSE;

    if (!IS_IMMORTAL(ch)) {
      if (ch->pcdata->account == NULL || (str_cmp(ch->pcdata->account->name, "Person")))
      return;
    }
    if (argument[0] == '\0') {
      send_to_char("Syntax: hedit create/delete/keyword/text/seealso (file)/hedit save\n\r", ch);
      return;
    }
    argument = one_argument(argument, arg1);

    if (!str_cmp(arg1, "create"))
    do_function(ch, &do_helpnew, argument);
    else if (!str_cmp(arg1, "delete"))
    do_function(ch, &do_helpdelete, argument);
    else if (!str_cmp(arg1, "keyword"))
    do_function(ch, &do_helpkeyword, argument);
    else if (!str_cmp(arg1, "text"))
    do_function(ch, &do_helptext, argument);
    else if (!str_cmp(arg1, "seealso"))
    do_function(ch, &do_helpseealso, argument);
    else if (!str_cmp(arg1, "save")) {
      save_helps();
      send_to_char("Helps saved.\n\r", ch);
    }
    else
    send_to_char("Syntax: hedit create/delete/keyword/text/seealso (file)/save\n\r", ch);

    return;

    strcpy(arg1, argument);

    if (argument[0] != '\0') {
      /* Taken from do_help */

      /*
while (argument[0] != '\0' )
{
argument = one_argument(argument,argone);
if (argall[0] != '\0')
strcat(argall," ");
strcat(argall,argone);
}
*/
      //	printf_to_char( ch, "%s - argall %s argone %s argument\n\r", argall, //argone, argument );
      for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {

        /** Used to be argall not argument **/
        if (!strcmp(argument, pHelp->keyword)) {
          ch->desc->pEdit = (void *)pHelp;
          ch->desc->editor = ED_HELP;
          found = TRUE;
          return;
        }
      }
    }

    if (!found) {
      argument = one_argument(arg1, arg1);

      if (!str_cmp(arg1, "create") && IS_IMMORTAL(ch)) {
        if (argument[0] == '\0') {
          send_to_char("Syntax: edit help create [keyword(s)]\n\r", ch);
          return;
        }
        if (hedit_new(ch, argument))
        ch->desc->editor = ED_HELP;
        return;
      }
    }

    send_to_char("HEdit:  There is no default help to edit.\n\r", ch);
    return;
  }

  HEDIT(hedit_list) {
    char buf[MIL];
    int cnt = 0;
    HELP_DATA *pHelp;
    Buffer outbuf;

    EDIT_HELP(ch, pHelp);

    for (pHelp = help_first; pHelp; pHelp = pHelp->next) {
      sprintf(buf, "%3d. %-14.14s%s", cnt, pHelp->keyword, cnt % 4 == 3 ? "\n\r" : " ");
      outbuf.strcat(buf);
      cnt++;
    }

    if (cnt % 4)
    outbuf.strcat("\n\r");

    page_to_char(outbuf, ch);
    return FALSE;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include "merc.h"
#include "olc.h"
#include "tables.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>

#if defined(__cplusplus)
extern "C" {
#endif

  /*
* Called from act_comm.c for when someone is adjusting the body of
* a note that has already been posted.
*/

  void write_all_notes(void);
  void spell_check(CHAR_DATA *ch, char *string);
  char *numlineas(char *string);

  /*****************************************************************************
Name:		string_edit
Purpose:	Clears string and puts player into editing mode.
Called by:	none
****************************************************************************/
  void string_edit(CHAR_DATA *ch, char **pString) {
    send_to_char("-========- Entering EDIT Mode -=========-\n\r", ch);
    send_to_char("    Type .h on a new line for help\n\r", ch);
    send_to_char(" Terminate with a ~ or @ on a blank line.\n\r", ch);
    send_to_char("-=======================================-\n\r", ch);

    if (*pString == NULL) {
      *pString = str_dup("");
    }
    else {
      **pString = '\0';
    }

    ch->desc->pString = pString;

    return;
  }

  /*****************************************************************************
Name:		string_append
Purpose:	Puts player into append mode for given string.
Called by:	(many)olc_act.c
****************************************************************************/
  void string_append(CHAR_DATA *ch, char **pString) {
    if (ch == NULL || ch->desc == NULL)
    return;

    send_to_char("-=======- Entering APPEND Mode -========-\n\r", ch);
    send_to_char("    Type .h on a new line for help\n\r", ch);
    send_to_char(" Terminate with a ~ or @ on a blank line.\n\r", ch);
    send_to_char("-=======================================-\n\r", ch);

    if (*pString == NULL) {
      *pString = str_dup("");
    }
    page_to_char(numlineas(*pString), ch);
    //    send_to_char (*pString, ch);

    /* Could need to comment this out */
    if (*(*pString + safe_strlen(*pString) - 1) != '\r')
    send_to_char("\n\r", ch);

    if (ch == NULL || ch->desc == NULL)
    return;

    ch->desc->pString = pString;

    return;
  }

  /****************************************************************************
Name:		string_replace
Purpose:	Substitutes one string for another.
Called by:	string_add(string.c) (aedit_builder)olc_act.c.
****************************************************************************/
  char *string_replace(char *orig, char *old, char *newstr) {
    char xbuf[MAX_STRING_LENGTH];
    int i;

    xbuf[0] = '\0';
    strcpy(xbuf, orig);
    if (strstr(orig, old) != NULL) {
      i = safe_strlen(orig) - safe_strlen(strstr(orig, old));
      xbuf[i] = '\0';
      strcat(xbuf, newstr);
      strcat(xbuf, &orig[i + safe_strlen(old)]);
      free_string(orig);
    }

    return str_dup(xbuf);
  }

  /*****************************************************************************
Name:		string_replace2
Purpose:	Replaces a line of text.
Called by:	string_add(string.c) (aedit_builder)olc_act.c.
****************************************************************************/
  char *string_replace2(CHAR_DATA *ch, char *orig, int line, char *newstr) {
    char *rdesc;
    char xbuf[MAX_STRING_LENGTH];
    int current_line = 1;
    int i;
    bool fReplaced = FALSE;

    xbuf[0] = '\0';
    strcpy(xbuf, orig);

    i = 0;

    for (rdesc = orig; *rdesc; rdesc++) {
      if (current_line == line && !fReplaced) {
        xbuf[i] = '\0';

        if (*newstr)
        strcat(xbuf, newstr);
        strcat(xbuf, "\n\r");
        fReplaced = TRUE;
      }

      if (current_line == line + 1) {
        strcat(xbuf, &orig[i]);
        free_string(orig);

        send_to_char("Line replaced.\n\r", ch);

        return str_dup(xbuf);
      }

      i++;

      if (*rdesc == '\r')
      current_line++;
    }

    if (current_line - 1 != line) {
      send_to_char("That line does not exist.\n\r", ch);
      return str_dup(xbuf);
    }

    free_string(orig);
    send_to_char("Line replaced.\n\r", ch);

    return str_dup(xbuf);
  }

  /*****************************************************************************
Name:		string_insertline
Purpose:	Inserts a line, blank or containing text.
Called by:	string_add(string.c) (aedit_builder)olc_act.c.
****************************************************************************/
  char *string_insertline(CHAR_DATA *ch, char *orig, int line, char *addstring) {
    char *rdesc;
    char xbuf[MAX_STRING_LENGTH];
    int current_line = 1;
    int i;

    xbuf[0] = '\0';
    strcpy(xbuf, orig);

    i = 0;

    for (rdesc = orig; *rdesc; rdesc++) {
      if (current_line == line)
      break;

      i++;

      if (*rdesc == '\r')
      current_line++;
    }

    if (!*rdesc) {
      send_to_char("That line does not exist.\n\r", ch);
      return str_dup(xbuf);
    }

    xbuf[i] = '\0';

    if (*addstring)
    strcat(xbuf, addstring);
    strcat(xbuf, "\n\r");

    strcat(xbuf, &orig[i]);
    free_string(orig);

    send_to_char("Line inserted.\n\r", ch);

    return str_dup(xbuf);
  }

  /*****************************************************************************
Name:		string_deleteline
Purpose:	Deletes a specified line of the string.
Called by:	string_add(string.c) (aedit_builder)olc_act.c.
****************************************************************************/
  char *string_deleteline(char *orig, int line) {
    char *rdesc;
    char xbuf[MAX_STRING_LENGTH];
    int current_line = 1;
    int i = 0;

    xbuf[0] = '\0';

    for (rdesc = orig; *rdesc; rdesc++) {
      if (current_line != line) {
        xbuf[i] = *rdesc;
        i++;
      }

      if (*rdesc == '\r')
      current_line++;
    }

    free_string(orig);
    xbuf[i] = 0;

    return str_dup(xbuf);
  }

  /*****************************************************************************
Name:		string_add
Purpose:	Interpreter for string editing.
Called by:	game_loop_xxxx(comm.c).
****************************************************************************/
  void string_add(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];

    /*
* Thanks to James Seng
*/
    smash_tilde(argument);

    if (*argument == '.') {
      char arg1[MAX_INPUT_LENGTH];
      char arg2[MAX_INPUT_LENGTH];
      char arg3[MAX_INPUT_LENGTH];

      argument = one_argument(argument, arg1);

      if (!str_cmp(arg1, "./")) {
        interpret(ch, argument);
        send_to_char("Command performed.\n\r", ch);
        return;
      }

      argument = first_arg(argument, arg2, FALSE);
      argument = first_arg(argument, arg3, FALSE);

      if (!str_cmp(arg1, ".c")) {
        send_to_char("String cleared.\n\r", ch);
        free_string(*ch->desc->pString);
        *ch->desc->pString = str_dup("");
        return;
      }

      if (!str_cmp(arg1, ".s")) {
        /*
char   *rdesc;
int     i = 1;

printf_to_char (ch, "`5%2d`` ", i);

for (rdesc = *ch->desc->pString; *rdesc; rdesc++)
{
if (*rdesc != '`')
printf_to_char (ch, "%c", rdesc[0]);
else
{
if (rdesc[1] == 'Z')
send_to_char ("{Z}", ch);
else
printf_to_char (ch, "%c%c", rdesc[0], rdesc[1]);
rdesc++;
}

if (*rdesc == '\r' && *(rdesc + 1))
{
i++;
printf_to_char (ch, "`5%2d`` ", i);
}
}
*/
        /* OLD STRING.C */
        send_to_char("String so far:\n\r", ch);
        page_to_char(numlineas(*ch->desc->pString), ch);

        return;
      }
      /*
if (!str_cmp (arg1, ".sp"))
{
spell_check (ch, *ch->desc->pString);
return;
}
*/
      if (!str_cmp(arg1, ".r")) {
        if (arg2[0] == '\0') {
          send_to_char("Syntax:  .r \"(old string)\" \"(new string)\"\n\r", ch);
          return;
        }
        smash_tilde(arg3); /* Just to be sure -- Hugin */
        *ch->desc->pString = string_replace(*ch->desc->pString, arg2, arg3);
        sprintf(buf, "'%s' replaced with '%s'.\n\r", arg2, arg3);
        send_to_char(buf, ch);
        return;
      }

      if (!str_cmp(arg1, ".lr")) {
        if (arg2[0] == '\0' || !is_number(arg2)) {
          send_to_char("Syntax:  .lr (line number) \"(new string)\"\n\r", ch);
          return;
        }
        smash_tilde(arg3); /* Just to be sure -- Hugin */
        *ch->desc->pString =
        string_replace2(ch, *ch->desc->pString, atoi(arg2), arg3);
        return;
      }

      if (!str_cmp(arg1, ".li")) {
        if (arg2[0] == '\0' || !is_number(arg2)) {
          send_to_char("Syntax:  .li (line number) \"(new string)\"\n\r", ch);
          return;
        }
        smash_tilde(arg3); /* Just to be sure -- Hugin */
        *ch->desc->pString =
        string_insertline(ch, *ch->desc->pString, atoi(arg2), arg3);
        return;
      }

      if (!str_cmp(arg1, ".ld")) {
        if (arg2[0] == '\0' || !is_number(arg2)) {
          send_to_char("Syntax:  .d (line number)\n\r", ch);
          return;
        }
        *ch->desc->pString = string_deleteline(*ch->desc->pString, atoi(arg2));
        sprintf(buf, "Line %d deleted.\n\r", atoi(arg2));
        send_to_char(buf, ch);
        return;
      }
      /*
if (!str_cmp (arg1, ".f"))
{
*ch->desc->pString = format_string (*ch->desc->pString);
send_to_char ("String formatted.\n\r", ch);
return;
}
*/
      if (!str_cmp(arg1, ".h")) {
        send_to_char("Sedit help (commands on blank line):    \n\r", ch);
        send_to_char(".r 'old' 'new'    - replace a substring \n\r", ch);
        send_to_char("                   (requires '', \"\")  \n\r", ch);
        send_to_char(".lr <line> 'text' - replaces a line     \n\r", ch);
        send_to_char("                   (requires '')  	   \n\r", ch);
        send_to_char(".h                - get help (this info)\n\r", ch);
        send_to_char(".s                - show string so far  \n\r", ch);
        // send_to_char (".sp               - spell check string  \n\r", ch);
        send_to_char(".f                - (word wrap) string  \n\r", ch);
        send_to_char(".c                - clear string so far \n\r", ch);
        send_to_char(".ld <line>        - deletes a line      \n\r", ch);
        send_to_char(".li <line> {text} - inserts a line	   \n\r", ch);
        send_to_char("./ <command>      - do a regular command\n\r", ch);
        send_to_char("@                 - end string          \n\r", ch);
        return;
      }

      send_to_char("SEdit:  Invalid dot command.\n\r", ch);
      return;
    }

    if (*argument == '~' || *argument == '@') {
      if (ch->desc->editor == ED_MPCODE) /* for mobprogs */
      {
        MOB_INDEX_DATA *mob;
        int hash;
        PROG_LIST *mpl;
        PROG_CODE *mpc;

        EDIT_MPCODE(ch, mpc);

        if (mpc != NULL)
        for (hash = 0; hash < MAX_KEY_HASH; hash++)
        for (mob = mob_index_hash[hash]; mob; mob = mob->next)
        for (mpl = mob->mprogs; mpl; mpl = mpl->next)
        if (mpl->vnum == mpc->vnum) {
          sprintf(buf, "Arreglando mob %d.\n\r", mob->vnum);
          send_to_char(buf, ch);
          mpl->code = mpc->code;
        }
      }

      if (ch->desc->editor == ED_OPCODE) /* for the objprogs */
      {
        OBJ_INDEX_DATA *obj;
        int hash;
        PROG_LIST *opl;
        PROG_CODE *opc;

        EDIT_OPCODE(ch, opc);

        if (opc != NULL)
        for (hash = 0; hash < MAX_KEY_HASH; hash++)
        for (obj = obj_index_hash[hash]; obj; obj = obj->next)
        for (opl = obj->oprogs; opl; opl = opl->next)
        if (opl->vnum == opc->vnum) {
          sprintf(buf, "Fixing object %d.\n\r", obj->vnum);
          send_to_char(buf, ch);
          opl->code = opc->code;
        }
      }

      if (ch->desc->editor == ED_RPCODE) /* for the roomprogs */
      {
        ROOM_INDEX_DATA *room;
        int hash;
        PROG_LIST *rpl;
        PROG_CODE *rpc;

        EDIT_RPCODE(ch, rpc);

        if (rpc != NULL)
        for (hash = 0; hash < MAX_KEY_HASH; hash++)
        for (room = room_index_hash[hash]; room; room = room->next)
        for (rpl = room->rprogs; rpl; rpl = rpl->next)
        if (rpl->vnum == rpc->vnum) {
          sprintf(buf, "Fixing room %d.\n\r", room->vnum);
          send_to_char(buf, ch);
          rpl->code = rpc->code;
        }
      }

      ch->desc->pString = NULL;
      // ch->desc->string_editor = 0;
      return;
    }

    strcpy(buf, *ch->desc->pString);

    /*
* Truncate strings to MAX_STRING_LENGTH.
* --------------------------------------
*/
    if (safe_strlen(*ch->desc->pString) + safe_strlen(argument) >=
        (MAX_STRING_LENGTH - 4)) {
      send_to_char("String too long, last line skipped.\n\r", ch);

      /* Force character out of editing mode. */
      ch->desc->pString = NULL;
      return;
    }

    /*
* Ensure no tilde's inside string.
* --------------------------------
*/
    smash_tilde(argument);

    strcat(buf, argument);
    strcat(buf, "\n\r");
    free_string(*ch->desc->pString);
    *ch->desc->pString = str_dup(buf);
    return;
  }

  /*
* Thanks to Kalgen for the new procedure (no more bug!)
* Original wordwrap() written by Surreality.
* Modifications for color codes and blank lines by Geoff.
*/
  /*****************************************************************************
Name:		format_string
Purpose:	Special string formating and word-wrapping.
Called by:	string_add(string.c) (many)olc_act.c
****************************************************************************/
  char *format_string(char *oldstring /*, bool fSpace */) {
    char xbuf[MAX_STRING_LENGTH];
    char xbuf2[MAX_STRING_LENGTH];
    char *rdesc;
    int i = 0;
    int end_of_line;
    bool cap = TRUE;
    bool bFormat = TRUE;

    xbuf[0] = xbuf2[0] = 0;

    i = 0;

    for (rdesc = oldstring; *rdesc; rdesc++) {
      if (*rdesc != '`') {
        if (bFormat) {
          if (*rdesc == '\n') {
            if (*(rdesc + 1) == '\r' && *(rdesc + 2) == ' ' && *(rdesc + 3) == '\n' && xbuf[i - 1] != '\r') {
              xbuf[i] = '\n';
              xbuf[i + 1] = '\r';
              xbuf[i + 2] = '\n';
              xbuf[i + 3] = '\r';
              i += 4;
              rdesc += 2;
            }
            else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == ' ' && *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r') {
              xbuf[i] = '\n';
              xbuf[i + 1] = '\r';
              i += 2;
            }
            else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == '\n' && xbuf[i - 1] != '\r') {
              xbuf[i] = '\n';
              xbuf[i + 1] = '\r';
              xbuf[i + 2] = '\n';
              xbuf[i + 3] = '\r';
              i += 4;
              rdesc += 1;
            }
            else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r') {
              xbuf[i] = '\n';
              xbuf[i + 1] = '\r';
              i += 2;
            }
            else if (xbuf[i - 1] != ' ' && xbuf[i - 1] != '\r') {
              xbuf[i] = ' ';
              i++;
            }
          }
          else if (*rdesc == '\r')
          ;
          else if (*rdesc == 'i' && *(rdesc + 1) == '.' && *(rdesc + 2) == 'e' && *(rdesc + 3) == '.') {
            xbuf[i] = 'i';
            xbuf[i + 1] = '.';
            xbuf[i + 2] = 'e';
            xbuf[i + 3] = '.';
            i += 4;
            rdesc += 3;
          }
          else if (*rdesc == ' ') {
            if (xbuf[i - 1] != ' ') {
              xbuf[i] = ' ';
              i++;
            }
          }
          else if (*rdesc == ')') {
            if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' && (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!')) {
              xbuf[i - 2] = *rdesc;
              xbuf[i - 1] = ' ';
              xbuf[i] = ' ';
              i++;
            }
            else if (xbuf[i - 1] == ' ' && (xbuf[i - 2] == ',' || xbuf[i - 2] == ';')) {
              xbuf[i - 1] = *rdesc;
              xbuf[i] = ' ';
              i++;
            }
            else {
              xbuf[i] = *rdesc;
              i++;
            }
          }
          else if (*rdesc == ',' || *rdesc == ';') {
            if (xbuf[i - 1] == ' ') {
              xbuf[i - 1] = *rdesc;
              xbuf[i] = ' ';
              i++;
            }
            else {
              xbuf[i] = *rdesc;
              if (*(rdesc + 1) != '\"') {
                xbuf[i + 1] = ' ';
                i += 2;
              }
              else {
                xbuf[i + 1] = '\"';
                xbuf[i + 2] = ' ';
                i += 3;
                rdesc++;
              }
            }
          }
          else if (*rdesc == '.' || *rdesc == '?' || *rdesc == '!') {
            if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' && (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!')) {
              xbuf[i - 2] = *rdesc;
              if (*(rdesc + 1) != '\"') {
                xbuf[i - 1] = ' ';
                xbuf[i] = ' ';
                i++;
              }
              else {
                xbuf[i - 1] = '\"';
                xbuf[i] = ' ';
                xbuf[i + 1] = ' ';
                i += 2;
                rdesc++;
              }
            }
            else {
              xbuf[i] = *rdesc;
              if (*(rdesc + 1) != '\"') {
                xbuf[i + 1] = ' ';
                xbuf[i + 2] = ' ';
                i += 3;
              }
              else {
                xbuf[i + 1] = '\"';
                xbuf[i + 2] = ' ';
                xbuf[i + 3] = ' ';
                i += 4;
                rdesc++;
              }
            }
            cap = TRUE;
          }
          else {
            xbuf[i] = *rdesc;
            if (cap) {
              cap = FALSE;
              xbuf[i] = UPPER(xbuf[i]);
            }
            i++;
          }
        }
        else {
          xbuf[i] = *rdesc;
          i++;
        }
      }
      else {
        if (*(rdesc + 1) == 'Z')
        bFormat = !bFormat;
        xbuf[i] = *rdesc;
        i++;
        rdesc++;
        xbuf[i] = *rdesc;
        i++;
      }
    }
    xbuf[i] = 0;
    strcpy(xbuf2, xbuf);

    rdesc = xbuf2;

    xbuf[0] = 0;

    for (;;) {
      end_of_line = 77;
      for (i = 0; i < end_of_line; i++) {
        if (*(rdesc + i + 1) && isdigit(*(rdesc + i + 1)) && *(rdesc + i + 2) && isdigit(*(rdesc + i + 2)) && *(rdesc + i + 3) && isdigit(*(rdesc + i + 3))) {
          end_of_line += 4;
          i += 3;
        }
        else if (*(rdesc + i + 1) && *(rdesc + i + 1) == '#') {
          end_of_line += 8;
          i += 7;
        }
        else {
          end_of_line += 2;
          i++;
        }

        //	    if (*(rdesc + i) == '`')
        //	    {
        //		end_of_line += 2;
        //		i++;
        //	    }

        if (!*(rdesc + i))
        break;

        if (*(rdesc + i) == '\r')
        end_of_line = i;
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
          strcat(xbuf, "\n\r");
          rdesc += i + 1;
          while (*rdesc == ' ')
          rdesc++;
        }
        else {
          bug("`5Wrap_string: `@No spaces``", 0);
          *(rdesc + (end_of_line - 2)) = 0;
          strcat(xbuf, rdesc);
          strcat(xbuf, "-\n\r");
          rdesc += end_of_line - 1;
        }
      }
      else {
        *(rdesc + i - 1) = 0;
        strcat(xbuf, rdesc);
        strcat(xbuf, "\r");
        rdesc += i;
        while (*rdesc == ' ')
        rdesc++;
      }
    }
    while (*(rdesc + i) && (*(rdesc + i) == ' ' || *(rdesc + i) == '\n' || *(rdesc + i) == '\r'))
    i--;
    *(rdesc + i + 1) = 0;
    strcat(xbuf, rdesc);
    if (xbuf[safe_strlen(xbuf) - 2] != '\n')
    strcat(xbuf, "\n\r");

    free_string(oldstring);
    return (str_dup(xbuf));
  }

  /*****************************************************************************
Name:		wrap_string
Purpose:	String word-wrapping for those whose terms don't have it.
Called by:	(many)act_comm.c (act_new)comm.c
****************************************************************************/
  char *wrap_string(char *oldstring, int length) {
    char xbuf[MAX_STRING_LENGTH];
    static char xbuf2[MAX_STRING_LENGTH];
    char *rdesc;
    int i = 0;
    int end_of_line;
    if (!length || length < 10 || length > 10000) {
      strcpy(xbuf2, oldstring);
      return xbuf2;
    }

    xbuf[0] = xbuf2[0] = '\0';

    i = 0;

    rdesc = oldstring;

    for (;;) {
      end_of_line = length; // 80
      for (i = 0; i < end_of_line; i++) {
        if (*(rdesc + i) == '`') {
          if (*(rdesc + i + 1) && isdigit(*(rdesc + i + 1)) && *(rdesc + i + 2) && isdigit(*(rdesc + i + 2)) && *(rdesc + i + 3) && isdigit(*(rdesc + i + 3))) {
            end_of_line += 4;
            i += 3;
          }
          else if (*(rdesc + i + 1) && *(rdesc + i + 1) == '#') {
            end_of_line += 8;
            i += 7;
          }
          else {
            end_of_line += 2;
            i++;
          }
        }

        if (!*(rdesc + i))
        break;

        if (*(rdesc + i) == '\r')
        end_of_line = i;
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
          strcat(xbuf, "\n\r");
          rdesc += i + 1;
          while (*rdesc == ' ')
          rdesc++;
        }
        else {
          bug("Wrap String: No spaces", 0);
          *(rdesc + (end_of_line - 2)) = 0;
          strcat(xbuf, rdesc);
          strcat(xbuf, "-\n\r");
          rdesc += end_of_line - 1;
        }
      }
      else {
        *(rdesc + i - 1) = 0;
        strcat(xbuf, rdesc);
        strcat(xbuf, "\r");
        rdesc += i;
        while (*rdesc == ' ')
        rdesc++;
      }
    }
    /*
if(safe_strlen(oldstring) < safe_strlen(xbuf)*2)
{
strcpy (xbuf2, oldstring);
return xbuf2;
}

if(safe_strlen(xbuf) > MAX_STRING_LENGTH - 500)
{
strcpy (xbuf2, oldstring);
return xbuf2;
}
*/

    //    char log_buf[MSL];
    //    sprintf( log_buf, "Rdesc len: %ld, xbuf len: %ld, xbuf2 len: %ld.
    //    %ld",safe_strlen(oldstring), safe_strlen(xbuf), safe_strlen(xbuf2), MAX_STRING_LENGTH);
    //    log_string( log_buf );
    //    log_string(oldstring);

    while (*(rdesc + i) && (*(rdesc + i) == ' ' || *(rdesc + i) == '\n' || *(rdesc + i) == '\r'))
    i--;
    *(rdesc + i + 1) = 0;
    strcat(xbuf, rdesc);

    if (safe_strlen(xbuf) < (MAX_STRING_LENGTH - 10) && xbuf[safe_strlen(xbuf) - 2] != '\n')
    strcat(xbuf, "\n\r");
    strcpy(xbuf2, xbuf);
    return (xbuf2);
  }

  /*****************************************************************************
Name:		spell_check
Purpose:	Spell-check strings with ispell (*nix only)
Called by:	(string_add)string.c
****************************************************************************/
  void spell_check(CHAR_DATA *ch, char *string) {
    char buf[MAX_STRING_LENGTH];
    char newstr[MAX_STRING_LENGTH];
    char line[MAX_STRING_LENGTH];
    char chr;
    FILE *fp;
    char *rstr;
    int i = 0;

    newstr[0] = 0;
    line[0] = 0;

    if (string == NULL)
    return;

    for (rstr = string; *rstr; rstr++) {
      if (*rstr != '\r' && *rstr != '~') {
        if (*rstr == '`') {
          rstr++;
        }
        else if (*rstr == '\n') {
          newstr[i] = ' ';
          i++;
        }
        else {
          newstr[i] = *rstr;
          i++;
        }
      }
    }
    newstr[i] = 0;

    fp = fopen("spell_check.txt", "w");
    fprintf(fp, "!\n\r%s\n\r", newstr);
    fclose(fp);
    sprintf(buf, "cat spell_check.txt | ispell -a -f spell_check.txt");
    system(buf);
    if ((fp = fopen("spell_check.txt", "r")) != NULL) {
      i = 0;
      while (!feof(fp)) {
        chr = getc(fp);
        line[i] = chr;
        i++;
      }
      line[i] = 0;
      fclose(fp);
      system("rm spell_check.txt");

      newstr[0] = 0;
      i = 0;
      for (rstr = str_dup(line); *rstr; rstr++) {
        if (*rstr == '\n' && *(rstr + 1) == '\n') {
          rstr++;
        }
        else {
          if (*rstr == '\r')
          send_to_char("`5*``", ch);
          newstr[i] = *rstr;
          i++;
        }
      }
      free_string(rstr);
      newstr[i] = 0;
      strcat(newstr, "\n\r\n\r");
      send_to_char(newstr, ch);
    }
    return;
  }

  /*
* Used above in string_add.  Because this function does not
* modify case if fCase is FALSE and because it understands
* parenthesis, it would probably make a nice replacement
* for one_argument.
*/
  /*****************************************************************************
Name:		first_arg
Purpose:	Pick off one argument from a string and return the rest.
Understands quates, parenthesis (barring ) ('s) and
percentages.
Called by:	string_add(string.c)
****************************************************************************/
  char *first_arg(char *argument, char *arg_first, bool fCase) {
    char cEnd;

    while (*argument == ' ')
    argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"' || *argument == '%' || *argument == '(') {
      if (*argument == '(') {
        cEnd = ')';
        argument++;
      }
      else
      cEnd = *argument++;
    }

    while (*argument != '\0') {
      if (*argument == cEnd) {
        argument++;
        break;
      }
      if (fCase)
      *arg_first = LOWER(*argument);
      else
      *arg_first = *argument;
      arg_first++;
      argument++;
    }
    *arg_first = '\0';

    while (*argument == ' ')
    argument++;

    return argument;
  }

  /*
* Used in olc_act.c for aedit_builders.
*/
  char *string_unpad(char *argument) {
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while (*s == ' ')
    s++;

    strcpy(buf, s);
    s = buf;

    if (*s != '\0') {
      while (*s != '\0')
      s++;
      s--;

      while (*s == ' ')
      s--;
      s++;
      *s = '\0';
    }

    free_string(argument);
    return str_dup(buf);
  }

  /*
* Same as capitalize but changes the pointer's data.
* Used in olc_act.c in aedit_builder.
*/
  char *string_proper(char *argument) {
    char *s;

    s = argument;

    while (*s != '\0') {
      if (*s != ' ') {
        *s = UPPER(*s);
        while (*s != ' ' && *s != '\0')
        s++;
      }
      else {
        s++;
      }
    }

    return argument;
  }

  char *getline2(char *str, char *buf) {
    int tmp = 0;
    bool found = FALSE;

    while (*str) {
      if (*str == '\n') {
        found = TRUE;
        break;
      }

      buf[tmp++] = *(str++);
    }

    if (found) {
      if (*(str + 1) == '\r')
      str += 2;
      else
      str += 1;
    } /* para que quedemos en el inicio de la prox linea */

    buf[tmp] = '\0';

    return str;
  }

  char *numlineas(char *string) {
    int cnt = 1;
    static char buf[MAX_STRING_LENGTH * 4];
    char buf2[MAX_STRING_LENGTH], tmpb[MAX_STRING_LENGTH];

    buf[0] = '\0';
    while (*string) {
      string = getline2(string, tmpb);
      sprintf(buf2, "%2d. %s\n\r", cnt++, tmpb);
      strcat(buf, buf2);
    }
    return buf;
  }

  int linecount(char *orig) {
    char *rdesc;
    char xbuf[MAX_STRING_LENGTH];
    int current_line = 0;
    int i;

    xbuf[0] = '\0';
    strcpy(xbuf, orig);

    i = 0;

    for (rdesc = orig; *rdesc; rdesc++) {
      i++;

      if (*rdesc == '\r')
      current_line++;
    }
    return current_line;
  }

  char *fetch_line(char *orig, int line) {
    char *rdesc;
    char returnbuf[MSL];
    int current_line = 1;
    int i = 0;

    returnbuf[0] = '\0';

    for (rdesc = orig; *rdesc; rdesc++) {
      if (current_line != line) {
      }
      else {
        returnbuf[i] = *rdesc;
        i++;
      }

      if (*rdesc == '\r')
      current_line++;
    }
    returnbuf[i] = 0;

    return str_dup(returnbuf);
  }

  char * appendLine(char * orig, char *newline) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';
    int num_lines = linecount(orig);
    //   char nbuf[MSL];
    //  sprintf(nbuf, "Append count :%d, olen: %zu, nlen: %zu, nl: %s", num_lines, safe_strlen(orig), safe_strlen(newline), newline);
    //  log_string(nbuf);
    if(safe_strlen(orig) + safe_strlen(newline) > (MSL - 1000))
    {
      do {
        static char buf2[MSL];
        buf2[0] = '\0';
        int tlen = linecount(orig);
        for(int i = 2; i <= (tlen+1); i++)
        {
          strcat(buf2, fetch_line(orig, i));
        }
        free_string(orig);
        orig = str_dup(buf2);
      } while (safe_strlen(orig) + safe_strlen(newline) > (MSL - 1000));

    }
    int max = 100;
    if(num_lines < 100)
    {
      strcat(buf, orig);
      strcat(buf, "\n\r");
      strcat(buf, newline);
      return str_dup(buf);
    }
    else
    {
      /*
log_string(fetch_line(orig, 0));
log_string(fetch_line(orig, 1));
log_string(fetch_line(orig, 2));

log_string(fetch_line(orig, num_lines-2));
log_string(fetch_line(orig, num_lines-1));
log_string(fetch_line(orig, num_lines));
log_string(fetch_line(orig, num_lines+1));
*/
      for(int i = num_lines - max + 2; i <= (num_lines+1); i++)
      {
        strcat(buf, fetch_line(orig, i));
      }
      strcat(buf, "\n\r");
      strcat(buf, newline);
      return str_dup(buf);
    }
  }


  void writeLineToFile(const std::string& fileName, const std::string& text) {
    std::ofstream outputFile(fileName, std::ios::app);  // Open file in append mode

    if (outputFile.is_open()) {
      outputFile << text << '\n';  // Write the text to the file
      outputFile.close();          // Close the file
    }
    else {

      bug("Unable to open the file.", 0);
    }
  }
  void writeTextToFile(const std::string& fileName, const std::string& text) {
    std::ofstream outputFile(fileName, std::ios::app);  // Open file in append mode

    if (outputFile.is_open()) {
      outputFile << text;  // Write the text to the file
      outputFile.close();          // Close the file
    }
    else {

      bug("Unable to open the file.", 0);
    }
  }



  std::string readAndDeleteLineFromFile(const std::string& fileName) {
    std::ifstream inputFile(fileName);
    std::string line;

    if (inputFile.is_open()) {
      if (std::getline(inputFile, line)) {
        inputFile.close();
        std::ofstream tempFile("temp.txt");
        std::string tempLine;

        while (std::getline(inputFile, tempLine)) {
          tempFile << tempLine << '\n';
        }

        tempFile.close();
        inputFile.close();

        if (remove(fileName.c_str()) != 0) {
          bug("Error deleting the line from the file.", 0);
          return "";
        }

        if (rename("temp.txt", fileName.c_str()) != 0) {
          bug("Error renaming the temporary file.", 0);
          return "";
        }

        return line;
      }
      else {
        inputFile.close();
        return "";
      }
    }
    else {
      bug("Unable to open the file.", 0);
      return "";
    }
  }



  _DOFUN(do_testaiwrite)
  {
    writeLineToFile(AI_IN_FILE, "1,19,,,,,");
    send_to_char("Line written to the file successfully.\n\r", ch);

  }

  _DOFUN(do_testairead)
  {
    std::string line = readAndDeleteLineFromFile(AI_OUT_FILE);
    send_to_char(line.c_str(), ch);
    send_to_char("\n\r", ch);
  }

  std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t delimiterLength = delimiter.length();

    while (pos != std::string::npos) {
      size_t found = input.find(delimiter, pos);
      if (found == std::string::npos) {
        tokens.push_back(input.substr(pos));
        break;
      }
      tokens.push_back(input.substr(pos, found - pos));
      pos = found + delimiterLength;
    }

    return tokens;
  }

  /*
int main() {
std::string inputString = "Hello|||World|||OpenAI|||Chatbot";
std::string delimiter = "|||";

std::vector<std::string> array = splitString(inputString, delimiter);

// Print the second value (index 1)
if (array.size() >= 2) {
std::cout << "Second value: " << array[1] << std::endl;
}
else {
std::cout << "Insufficient elements in the array." << std::endl;
}

return 0;
}
*/


  size_t safe_strlen(const char* str) {
    if (str == NULL) {
      str = str_dup("");
      // Handle the case when str is a null pointer
      return 0;  // Or any other appropriate behavior for your program
    }
    else {
      return strlen(str);
    }
  }

  char* replaceSubstring(char* original, const char* substring, const char* replacement) {
    // Convert the C-style strings to C++ strings for easier manipulation
    std::string originalString(original);
    std::string substringString(substring);
    std::string replacementString(replacement);

    size_t pos = 0;

    while ((pos = originalString.find(substringString, pos)) != std::string::npos) {
      originalString.replace(pos, substringString.length(), replacementString);
      pos += replacementString.length();
    }

    // Allocate memory for the result C-style string
    char* result = new char[originalString.length() + 1];
    strcpy(result, originalString.c_str());

    return result;
  }



  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif




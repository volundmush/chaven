
#include "merc.h"
#include "tables.h"

#if defined(__cplusplus)
extern "C" {
#endif

  int flag_lookup args((const char *name, const struct flag_type *flag_table));

  void do_flag(CHAR_DATA *ch, char *argument) {
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
    char word[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    long *flag, old = 0, tnew = 0, marked = 0, pos;
    char type;
    const struct flag_type *flag_table;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    type = argument[0];

    if (type == '=' || type == '-' || type == '+')
    argument = one_argument(argument, word);

    if (arg1[0] == '\0') {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  flag mob  <name> <field> <flags>\n\r", ch);
      send_to_char("  flag char <name> <field> <flags>\n\r", ch);
      send_to_char("  mob  flags: act,aff,off,imm,res,vuln,form,part\n\r", ch);
      send_to_char("  char flags: plr,comm,aff,imm,res,vuln,\n\r", ch);
      send_to_char("  +: add flag, -: remove flag, = set equal to\n\r", ch);
      send_to_char("  otherwise flag toggles the flags listed.\n\r", ch);
      return;
    }

    if (arg2[0] == '\0') {
      send_to_char("What do you wish to set flags on?\n\r", ch);
      return;
    }

    if (arg3[0] == '\0') {
      send_to_char("You need to specify a flag to set.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Which flags do you wish to change?\n\r", ch);
      return;
    }

    if (!str_prefix(arg1, "mob") || !str_prefix(arg1, "char")) {
      victim = get_char_world(ch, arg2);
      if (victim == NULL) {
        send_to_char("You can't find them.\n\r", ch);
        return;
      }

      /* select a flag to set */
      /*
if (!str_prefix(arg3,"act"))
{
if (!IS_NPC(victim))
{
send_to_char("Use plr for PCs.\n\r",ch);
return;
}

flag = &victim->act;
flag_table = act_flags;
}

else if (!str_prefix(arg3,"plr"))
{
if (IS_NPC(victim))
{
send_to_char("Use act for NPCs.\n\r",ch);
return;
}

flag = &victim->act;
flag_table = plr_flags;
}

else if (!str_prefix(arg3,"aff"))
{
flag = &victim->affected_by;
flag_table = affect_flags_bit;
}
*/
      else if (!str_prefix(arg3, "immunity")) {
        flag = &victim->imm_flags;
        flag_table = imm_flags;
      }

      else if (!str_prefix(arg3, "form")) {
        if (!IS_NPC(victim)) {
          send_to_char("Form can't be set on PCs.\n\r", ch);
          return;
        }

        flag = &victim->form;
        flag_table = form_flags;
      }

      /*
else if (!str_prefix(arg3,"comm"))
{
if (IS_NPC(victim))
{
send_to_char("Comm can't be set on NPCs.\n\r",ch);
return;
}

flag = victim->comm;
flag_table = comm_flags;
}
*/

      else {
        send_to_char("That's not an acceptable flag.\n\r", ch);
        return;
      }

      old = *flag;
      victim->zone = NULL;

      if (type != '=')
      tnew = old;

      /* mark the words */
      for (;;) {
        argument = one_argument(argument, word);

        if (word[0] == '\0')
        break;

        pos = flag_lookup(word, flag_table);

        if (pos == NO_FLAG) {
          send_to_char("That flag doesn't exist!\n\r", ch);
          return;
        }
        else
        SET_BIT(marked, pos);
      }

      for (pos = 0; flag_table[pos].name != NULL; pos++) {
        if (!flag_table[pos].settable && IS_SET(old, flag_table[pos].bit)) {
          SET_BIT(tnew, flag_table[pos].bit);
          continue;
        }

        if (IS_SET(marked, flag_table[pos].bit)) {
          switch (type) {
          case '=':
          case '+':
            SET_BIT(tnew, flag_table[pos].bit);
            break;
          case '-':
            REMOVE_BIT(tnew, flag_table[pos].bit);
            break;
          default:
            if (IS_SET(tnew, flag_table[pos].bit))
            REMOVE_BIT(tnew, flag_table[pos].bit);
            else
            SET_BIT(tnew, flag_table[pos].bit);
          }
        }
      }
      *flag = tnew;
      return;
    }
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

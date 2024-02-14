#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif

#include "merc.h"
#include "tables.h"
#include "lookup.h"

#if defined(__cplusplus)
extern "C" {
#endif



  struct flag_stat_type {
    const struct flag_type *structure;
    bool stat;
  };

  /*****************************************************************************
Name:		flag_stat_table
Purpose:	This table catagorizes the tables following the lookup
functions below into stats and flags.  Flags can be toggled
but stats can only be assigned.  Update this table when a
new set of flags is installed.
****************************************************************************/
  const struct flag_stat_type flag_stat_table[] = {
    /*  {	structure		stat	}, */
    {area_flags, FALSE}, {sex_flags, TRUE}, {exit_flags, FALSE}, {door_resets, TRUE}, {room_flags, FALSE}, {sector_flags, TRUE}, {type_flags, TRUE}, {extra_flags, FALSE}, {wear_flags, FALSE}, {act_flags, FALSE}, {affect_flags, FALSE}, {apply_flags, TRUE}, {wear_loc_flags, TRUE}, {wear_loc_strings, TRUE}, {container_flags, FALSE}, 
    /* ROM specific flags: */

    {form_flags, FALSE}, {part_flags, FALSE}, {ac_type, TRUE}, {size_flags, TRUE}, {position_flags, TRUE}, {off_flags, FALSE}, {imm_flags, FALSE}, {res_flags, FALSE}, {vuln_flags, FALSE}, {weapon_class, TRUE}, {weapon_type2, FALSE}, {apply_types, TRUE}, {0, 0}};

  /*****************************************************************************
Name:		is_stat( table )
Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
Called by:	flag_value and flag_string.
Note:		This function is local and used only in bit.c.
****************************************************************************/
  bool is_stat(const struct flag_type *flag_table) {
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++) {
      if (flag_stat_table[flag].structure == flag_table && flag_stat_table[flag].stat)
      return TRUE;
    }
    return FALSE;
  }

  /*****************************************************************************
Name:		flag_value( table, flag )
Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
Called by:	olc.c and olc_act.c.
****************************************************************************/
  int flag_value(const struct flag_type *flag_table, char *argument) {
    char word[MAX_INPUT_LENGTH];
    int bit;
    int marked = 0;
    bool found = FALSE;

    if (is_stat(flag_table))
    return flag_lookup(argument, flag_table);

    /*
* Accept multiple flags.
*/
    for (;;) {
      argument = one_argument(argument, word);

      if (word[0] == '\0')
      break;

      if ((bit = flag_lookup(word, flag_table)) != NO_FLAG) {
        SET_BIT(marked, bit);
        found = TRUE;
      }
    }

    if (found)
    return marked;
    else
    return NO_FLAG;
  }

  /*****************************************************************************
Name:		flag_string( table, flags/stat )
Purpose:	Returns string with name(s) of the flags or stat entered.
Called by:	act_olc.c, olc.c, and olc_save.c.
****************************************************************************/
  const char *flag_string(const struct flag_type *flag_table, int bits) {
    static char buf[2][512];
    static int cnt = 0;
    int flag;

    if (++cnt > 1)
    cnt = 0;

    buf[cnt][0] = '\0';

    for (flag = 0; flag_table[flag].name != NULL; flag++) {
      if (!is_stat(flag_table) && IS_SET(bits, flag_table[flag].bit)) {
        strcat(buf[cnt], " ");
        strcat(buf[cnt], flag_table[flag].name);
      }
      else if (flag_table[flag].bit == bits) {
        strcat(buf[cnt], " ");
        strcat(buf[cnt], flag_table[flag].name);
        break;
      }
    }
    return (buf[cnt][0] != '\0') ? buf[cnt] + 1 : "none";
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

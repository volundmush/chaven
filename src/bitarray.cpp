#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#include "merc.h"

#if defined(__cplusplus)
extern "C" {
#endif

  bool is_stat args((const struct flag_type *flag_table));
  int flag_lookup args((const char *name, const struct flag_type *flag_table));

  void SET_INIT(SET s) /* initialize set to empty */
  {
    for (int x = 0; x < SETSIZE; s[x++] = 0)
    ;
  }

  void SET_COPY(SET to, const SET from) {
    for (int x = 0; x < SETSIZE; x++)
    to[x] = from[x];
  }

  void SET_AND_SET(SET s1, const SET s2) {
    for (int x = 0; x < SETSIZE; x++)
    s1[x] &= s2[x];
  }

  void SET_OR_SET(SET s1, const SET s2) {
    for (int x = 0; x < SETSIZE; x++)
    s1[x] |= s2[x];
  }

  void SET_XOR_SET(SET s1, const SET s2) {
    for (int x = 0; x < SETSIZE; x++)
    s1[x] ^= s2[x];
  }

  void SET_REM_SET(SET s1, const SET s2) {
    for (int x = 0; x < SETSIZE; x++)
    s1[x] &= ~s2[x];
  }

  void SET_DIF(const SET s1, const SET s2, SET diff) {
    SET_INIT(diff);
    for (int x = 0; x < SETSIZE; x++)
    diff[x] = (~((~s1[x] | s2[x])));
  }

  bool SET_ISFLAG_SET(const SET s1, const SET s2) {
    for (int x = 0; x < SETSIZE; x++)
    for (int y = 0; y < 32; y++)
    if (IS_FLAG(s2, (x * 32) + y) && !IS_FLAG(s1, (x * 32) + y))
    return FALSE;
    return TRUE;
  }

  bool SET_IS_ZERO(const SET s1) {
    for (int x = 0; x < SETSIZE; x++)
    if (s1[x] != 0)
    return FALSE;
    return TRUE;
  }

  bool set_flag_value(const struct flag_type *flag_table, char *argument, SET flag) {
    char word[MAX_INPUT_LENGTH];
    int bit;
    bool found = FALSE;

    SET_INIT(flag);

    if (is_stat(flag_table)) {
      if ((bit = flag_lookup(argument, flag_table)) != NO_FLAG) {
        SET_FLAG(flag, bit);
        return TRUE;
      }
      return FALSE;
    }

    /* Accept multiple flags.     */
    for (;;) {
      argument = one_argument(argument, word);
      if (word[0] == '\0')
      break;

      if ((bit = flag_lookup(word, flag_table)) != NO_FLAG) {
        SET_FLAG(flag, bit);
        found = TRUE;
      }
    }

    if (found)
    return TRUE;
    else
    return FALSE;
  }

  /********************************************************************
* Name:        bit_array_convert
* Purpose:     Converts a char to it's integer interpretation
*		(according to ASCII)
********************************************************************/
  int bit_array_convert(char letter) {
    int bitpos = 0;

    if ('A' <= letter && letter <= 'Z')
    bitpos = letter - 65;
    else if ('a' <= letter && letter <= 'f')
    bitpos = letter - 71;

    return bitpos;
  }

  /********************************************************************
* Name:        set_print_flag
* Purpose:     Returns the string value of the bit array specified
*		by the only input parameter.  In the string
* 		representation, each unsigned long of the SET is
*		seperated by the character 'x'.
*              eg. 1024x3006472x1462728
********************************************************************/
  char *set_print_flag(const SET bit_array) {
    static char buf[MSL];
    char buf2[MIL];
    int x;

    buf[0] = '\0';
    for (x = 0; x < SETSIZE; x++) {
      sprintf(buf2, "%lux", bit_array[x]);
      strcat(buf, buf2);
    }

    return buf;
  }

  /********************************************************************
* Name:        set_fread_flag
* Purpose:     Reads strings of flags in from a file and sets the
*              appropriate bits in the bit array.  Each unsigned
*              long is seperated by the character 'x'
*              eg. ABCDxHGIFxKOPSD
********************************************************************/
  void set_fread_flag(FILE *fp, SET bit_array) {
    int in_byte = 0;
    char c;
    SET_INIT(bit_array);

    do {
      c = getc(fp);
    } while (isspace(c));

    if (!isdigit(c)) {
      bugf("[%s:%s] Bad format.", __FILE__, __FUNCTION__);
      exit(1);
    }

    while ((isdigit(c) || c == 'x') && in_byte < SETSIZE) {
      if (c == 'x')
      in_byte++;
      else
      bit_array[in_byte] = bit_array[in_byte] * 10 + c - '0';
      c = getc(fp);
    }

    if (c != ' ')
    ungetc(c, fp);
  }

  /*
* Converts a bitvector into a bitarray SET
*/
  void bitvector_to_array(SET bit_array, long bitvector) {
    int x;
    for (x = 0; x < 32; x++) {
      if (IS_SET(bitvector, ((long)1 << x)))
      SET_FLAG(bit_array, x);
    }
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#include "merc.h"
#include "recycle.h"

#if defined(__cplusplus)
extern "C" {
#endif

  BanList ban_list;

  void save_bans(void) {
    FILE *fp;
    bool found = FALSE;

    if ((fp = fopen(BAN_FILE, "w")) == NULL) {
      perror(BAN_FILE);
    }

    for (BanList::iterator it = ban_list.begin(); it != ban_list.end(); ++it) {
      BAN_DATA *pban = *it;
      if (IS_SET(pban->ban_flags, BAN_PERMANENT)) {
        found = TRUE;
        fprintf(fp, "%-20s %-2d %s\n", pban->name, pban->level, print_flags(pban->ban_flags));
      }
    }

    fprintf(fp, "$\n");
    fclose(fp);
    if (!found)
    unlink(BAN_FILE);
  }

  void load_bans(void) {
    FILE *fp;

    if ((fp = fopen(BAN_FILE, "r")) == NULL)
    return;

    for (;;) {
      BAN_DATA *pban;
      char *word;

      word = fread_word(fp);
      if (!str_cmp(word, "$"))
      break;

      pban = new_ban();

      pban->name = str_dup(word);
      pban->level = fread_number(fp);
      pban->ban_flags = fread_flag(fp);
      fread_to_eol(fp);

      ban_list.push_front(pban);
    }

    fclose(fp);
  }

  bool check_ban(char *site, int type) {
    char host[MAX_STRING_LENGTH];

    strcpy(host, capitalize(site));
    host[0] = LOWER(host[0]);

    for (BanList::iterator it = ban_list.begin(); it != ban_list.end(); ++it) {
      BAN_DATA *pban = *it;

      if (!IS_SET(pban->ban_flags, type))
      continue;

      if (IS_SET(pban->ban_flags, BAN_PREFIX) && IS_SET(pban->ban_flags, BAN_SUFFIX) && strstr(pban->name, host) != NULL)
      return TRUE;

      if (IS_SET(pban->ban_flags, BAN_PREFIX) && !str_suffix(pban->name, host))
      return TRUE;

      if (IS_SET(pban->ban_flags, BAN_SUFFIX) && !str_prefix(pban->name, host))
      return TRUE;
    }

    return FALSE;
  }

  void ban_site(CHAR_DATA *ch, char *argument, bool fPerm) {
    char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char *name;
    Buffer outbuf;
    BAN_DATA *pban;
    bool prefix = FALSE, suffix = FALSE;
    int type;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0') {
      if (ban_list.empty()) {
        send_to_char("No sites banned at this time.\n\r", ch);
        return;
      }

      outbuf.printf("Banned sites  level  type     status\n\r");
      for (BanList::iterator it = ban_list.begin(); it != ban_list.end(); ++it) {
        pban = *it;

        sprintf(buf2, "%s%s%s", IS_SET(pban->ban_flags, BAN_PREFIX) ? "*" : "", pban->name, IS_SET(pban->ban_flags, BAN_SUFFIX) ? "*" : "");
        sprintf(buf, "%-12s    %-3d  %-7s  %s\n\r", buf2, pban->level, IS_SET(pban->ban_flags, BAN_NEWBIES)
        ? "newbies" : IS_SET(pban->ban_flags, BAN_PERMIT)
        ? "permit" : IS_SET(pban->ban_flags, BAN_ALL) ? "all" : "", IS_SET(pban->ban_flags, BAN_PERMANENT) ? "perm" : "temp");
        outbuf.strcat(buf);
      }

      page_to_char(outbuf, ch);
      return;
    }

    /* find out what type of ban */
    if (arg2[0] == '\0' || !str_prefix(arg2, "all"))
    type = BAN_ALL;
    else if (!str_prefix(arg2, "newbies"))
    type = BAN_NEWBIES;
    else if (!str_prefix(arg2, "permit"))
    type = BAN_PERMIT;
    else {
      send_to_char("Acceptable ban types are all, newbies, and permit.\n\r", ch);
      return;
    }

    name = str_dup(arg1);

    if (name[0] == '*') {
      prefix = TRUE;
      name++;
    }

    if (name[safe_strlen(name) - 1] == '*') {
      suffix = TRUE;
      name[safe_strlen(name) - 1] = '\0';
    }

    if (safe_strlen(name) == 0) {
      send_to_char("You have to ban SOMETHING.\n\r", ch);
      return;
    }

    for (BanList::iterator it = ban_list.begin(); it != ban_list.end(); ++it) {
      pban = (*it);
      if (!str_cmp(name, pban->name)) {
        if (pban->level > get_trust(ch)) {
          send_to_char("That ban was set by a higher power.\n\r", ch);
          return;
        }
        else {
          ban_list.remove(pban);
          free_ban(pban);
          break;
        }
      }
    }

    pban = new_ban();
    pban->name = str_dup(name);
    pban->level = get_trust(ch);

    /* set ban type */
    pban->ban_flags = type;

    if (prefix)
    SET_BIT(pban->ban_flags, BAN_PREFIX);
    if (suffix)
    SET_BIT(pban->ban_flags, BAN_SUFFIX);
    if (fPerm)
    SET_BIT(pban->ban_flags, BAN_PERMANENT);

    ban_list.push_back(pban);
    save_bans();
    sprintf(buf, "%s has been banned.\n\r", pban->name);
    send_to_char(buf, ch);
    return;
  }

  _DOFUN(do_ban) { ban_site(ch, argument, FALSE); }

  _DOFUN(do_permban) { ban_site(ch, argument, TRUE); }

  _DOFUN(do_allow) {
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Remove which site from the ban list?\n\r", ch);
      return;
    }

    for (BanList::iterator it = ban_list.begin(); it != ban_list.end(); ++it) {
      BAN_DATA *pban = *it;

      if (!str_cmp(arg, pban->name)) {
        if (pban->level > get_trust(ch)) {
          send_to_char("You are not powerful enough to lift that ban.\n\r", ch);
          return;
        }

        ban_list.remove(pban);
        free_ban(pban);

        sprintf(buf, "Ban on %s lifted.\n\r", arg);
        send_to_char(buf, ch);
        save_bans();
        return;
      }
    }

    send_to_char("Site is not banned.\n\r", ch);
    return;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif
#include "merc.h"
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif

  // Globals
  SYSTEM_DATA *sysdata;

  void new_system_data() {
    sysdata = (SYSTEM_DATA *)alloc_perm(sizeof(*sysdata));

    sysdata->mud_name = str_dup("Inferno");
    sysdata->maxplayers = 0;
    sysdata->alltimemax = 0;
    sysdata->time_of_max = str_dup("");
  }

  bool load_system_config() {
    char filename[MAX_INPUT_LENGTH];
    FILE *fp;
    bool fMatch;
    const char *word;

    sprintf(filename, "%s%s", SYSTEM_DIR, SYSTEM_DAT_FILE);

    // Setup struct with default values.
    new_system_data();

    if ((fp = fopen(filename, "r")) != NULL) {
      for (;;) {
        word = feof(fp) ? "End" : fread_word(fp);

        switch (UPPER(word[0])) {
        case '*':
          fread_to_eol(fp);
          break;

        case 'E':
          if (!str_cmp(word, "End")) {
            fclose(fp);
            return TRUE;
          }
          break;

        case 'H':
          KEY("Highplayers", sysdata->alltimemax, fread_number(fp));
          KEY("Highplayerstime", sysdata->time_of_max, fread_string(fp));

          break;

        case 'M':
          KEY("MudName", sysdata->mud_name, fread_string(fp));
          break;

        default:
          bug("load_system_config: default read case.", 0);
          break;
        }
        if (fMatch == FALSE)
        log_string("Load_system_config: No Fmatch.");
      }

      bug("load_system_config: no 'End' section.", 0);
      return FALSE;
    }
    else {
      log_string("load_system_config:  No file, using defaults.");
      return TRUE;
    }
  }

  void save_sysdata() {
    FILE *fp;
    char filename[MAX_INPUT_LENGTH];

    sprintf(filename, "%s%s", SYSTEM_DIR, SYSTEM_DAT_FILE);

    if ((fp = fopen(filename, "w")) == NULL) {
      bug("save_sysdata: fopen", 0);
      perror(filename);
    }
    else {
      fprintf(fp, "MudName         %s~\n", sysdata->mud_name);
      fprintf(fp, "Highplayers     %d\n", sysdata->alltimemax);
      fprintf(fp, "Highplayerstime %s~\n", sysdata->time_of_max);
      fprintf(fp, "End\n\n");
    }
    fclose(fp);
    return;
  }

  _DOFUN(do_showsysdata) {
    char buf[MAX_STRING_LENGTH];

    send_to_char("\n\rThe Following Options are Set:\n\r", ch);
    send_to_char("-----------------------------\n\r\n\r", ch);

    sprintf(buf, "Mud Name: %s\n\r", sysdata->mud_name);
    send_to_char(buf, ch);
    sprintf(buf, "Max Players this boot: %d\n\r", sysdata->maxplayers);
    send_to_char(buf, ch);
    sprintf(buf, "All Time Max Players: %d\n\r", sysdata->alltimemax);
    send_to_char(buf, ch);
    sprintf(buf, "Time of Max Players: %s\n\r", sysdata->time_of_max);
    send_to_char(buf, ch);
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

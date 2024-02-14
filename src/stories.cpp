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
#include <math.h>
#include <vector>
#include <map>
#include "merc.h"
#include "olc.h"
#include "gsn.h"
#include "recycle.h"
#include "lookup.h"
#include "global.h"

#if defined(__cplusplus)
extern "C" {
#endif



  char *reward_string args((int val));
  int get_subfac args((char *word));
  bool valid_subfac args((char *word));
  void print_all_subfacs args((CHAR_DATA * ch));
  int subfac_score args((CHAR_DATA * ch, int subfac));
  int aobjval args((OBJ_DATA * obj));
  std::string encounter_prompt args((CHAR_DATA * ch, int type));
  void offer_ally args((CHAR_DATA * storyrunner, CHAR_DATA *target, CHAR_DATA *victim));
  bool cross_sr_compare args((char *nameone, char *nametwo));
  GALLERY_TYPE *gallery_lookup args((char *name));
  bool can_destiny_one args((CHAR_DATA * ch, NEWDESTINY_TYPE *dest));
  bool can_destiny_two args((CHAR_DATA * ch, NEWDESTINY_TYPE *dest));
  bool narrative_pair args((CHAR_DATA * ch, CHAR_DATA *victim));
  int narrative_score args((CHAR_DATA * ch, CHAR_DATA *victim));
  char *narrative_pair_rewards args((CHAR_DATA * ch, CHAR_DATA *victim));

  int part_count args((FANTASY_TYPE * fant));

  int fantasy_bonus = 0;
  int invasion_one = 0;
  int invasion_two = 0;
  int invasion_three = 0;

  int invade_intelligent_one = 0;
  int invade_intelligent_two = 0;
  int invade_intelligent_three = 0;

  int invade_aerial_one = 0;
  int invade_aerial_two = 0;
  int invade_aerial_three = 0;

  int invade_shroud_one = 0;
  int invade_shroud_two = 0;
  int invade_shroud_three = 0;

  int invade_spawn_one = 0;
  int invade_spawn_two = 0;
  int invade_spawn_three = 0;

  int global_damage_constant = 0;
  int global_damage_erratic = 0;

  int last_dreamworld = 0;

  char *global_constant_message = str_dup("");
  char *global_erratic_message = str_dup("");

  char *crisis_atmosphere = str_dup("");

  int crisis_brainwash = 0;

  char *crisis_brainwash_message = str_dup("");

  int crisis_darkness = 0;
  int supernatural_darkness = 0;
  int crisis_blackout = 0;

  int crisis_light = 0;

  int crisis_hurricane = 0;

  int crisis_storm = 0;

  int crisis_flood = 0;
  int crisis_noshroud = 0;

  int crisis_nodrive = 0;
  int crisis_mist = 0;

  int crisis_uninvited = 0;

  int crisis_notravel = 0;

  int crisis_prologue = 0;

  vector<STORY_TYPE *> StoryVect;

  STORY_TYPE *nullstory;

  void fread_story(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    STORY_TYPE *story;

    story = new_story();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", story->author, fread_string(fp));
        KEY("Advertised", story->advertised, fread_number(fp));
        break;
      case 'C':
        KEY("Crisis", story->crisis, fread_number(fp));
        KEY("CreatedAt", story->created_at, fread_number(fp));
        KEY("CType", story->ctype, fread_number(fp));
        if (!str_cmp(word, "Comment")) {
          int i;
          for (i = 0; i < 50 && str_cmp(story->comments[i], ""); i++) {
          }
          story->comments[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'D':
        KEY("Description", story->description, fread_string(fp));
        KEY("Dedication", story->dedication, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!story->author) {
            bug("Fread_Story: Name not found.", 0);
            free_story(story);
            return;
          }
          StoryVect.push_back(story);
          return;
        }
        break;
      case 'H':
        KEY("HideList", story->hide_list, fread_string(fp));
        break;
      case 'I':
        KEY("InviteList", story->invite_list, fread_string(fp));
        break;
      case 'L':
        KEY("Location", story->location, fread_string(fp));
        break;
      case 'P':
        KEY("Protected", story->isprotected, fread_number(fp));
        KEY("Private", story->isprivate, fread_number(fp));
      case 'S':
        KEY("Stats", story->stats, fread_string(fp));
        KEY("SecurityBanList", story->security_ban_list, fread_string(fp));
        KEY("SecurityLetinList", story->security_letin_list, fread_string(fp));
        break;
      case 'T':
        KEY("Time", story->time, fread_number(fp));
        KEY("Type", story->type, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_story: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  struct cal_greater {
    inline bool operator()(const STORY_TYPE *fant1, const STORY_TYPE *fant2) {
      return (fant1->time < fant2->time);
    }
  };

  void load_stories() {
    nullstory = new_story();
    FILE *fp;

    if ((fp = fopen(STORY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Stories: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "STORY")) {
          fread_story(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Stories: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open stories.txt", 0);
      exit(0);
    }
  }

  void save_stories_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/stories.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/stories.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/stories.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/stories.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/stories.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/stories.txt");
    else
    sprintf(buf, "../data/back7/stories.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open stories.txt for writing", 0);
      return;
    }

    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_stories: Blank story in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      fprintf(fpout, "#STORY\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Dedication %d\n", (*it)->dedication);
      fprintf(fpout, "Type %s~\n", (*it)->type);
      fprintf(fpout, "Location %s~\n", (*it)->location);
      fprintf(fpout, "Stats %s~\n", (*it)->stats);
      fprintf(fpout, "Time %d\n", (*it)->time);
      fprintf(fpout, "Protected %d\n", (*it)->isprotected);
      fprintf(fpout, "Crisis %d\n", (*it)->crisis);
      fprintf(fpout, "Advertised %d\n", (*it)->advertised);
      fprintf(fpout, "CreatedAt %d\n", (*it)->created_at);
      fprintf(fpout, "Private %d\n", (*it)->isprivate);
      fprintf(fpout, "CType %d\n", (*it)->ctype);
      fprintf(fpout, "InviteList %s~\n", (*it)->invite_list);
      fprintf(fpout, "HideList %s~\n", (*it)->hide_list);
      fprintf(fpout, "SecurityBanList %s~\n", (*it)->security_ban_list);
      fprintf(fpout, "SecurityLetinList %s~\n", (*it)->security_letin_list);
      for (i = 0; i < 50; i++) {
        if (str_cmp((*it)->comments[i], ""))
        fprintf(fpout, "Comment %s~\n", (*it)->comments[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_stories() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(STORY_FILE, "w")) == NULL) {
      bug("Cannot open stories.txt for writing", 0);
      return;
    }

    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_stories: Blank story in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      fprintf(fpout, "#STORY\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Type %s~\n", (*it)->type);
      fprintf(fpout, "Location %s~\n", (*it)->location);
      fprintf(fpout, "Dedication %d\n", (*it)->dedication);
      fprintf(fpout, "Stats %s~\n", (*it)->stats);
      fprintf(fpout, "Time %d\n", (*it)->time);
      fprintf(fpout, "Protected %d\n", (*it)->isprotected);
      fprintf(fpout, "Crisis %d\n", (*it)->crisis);
      fprintf(fpout, "Advertised %d\n", (*it)->advertised);
      fprintf(fpout, "CreatedAt %d\n", (*it)->created_at);
      fprintf(fpout, "Private %d\n", (*it)->isprivate);
      fprintf(fpout, "CType %d\n", (*it)->ctype);
      fprintf(fpout, "InviteList %s~\n", (*it)->invite_list);
      fprintf(fpout, "HideList %s~\n", (*it)->hide_list);
      fprintf(fpout, "SecurityBanList %s~\n", (*it)->security_ban_list);
      fprintf(fpout, "SecurityLetinList %s~\n", (*it)->security_letin_list);
      for (i = 0; i < 50; i++) {
        if (str_cmp((*it)->comments[i], ""))
        fprintf(fpout, "Comment %s~\n", (*it)->comments[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_stories_backup();
    std::sort(StoryVect.begin(), StoryVect.end(), cal_greater());
  }

  vector<STORYIDEA_TYPE *> StoryIdeaVect;

  STORYIDEA_TYPE *nullstoryidea;

  void fread_storyidea(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    STORYIDEA_TYPE *story;

    story = new_storyidea();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", story->author, fread_string(fp));
        break;
      case 'C':
        KEY("Claimant", story->claimant, fread_string(fp));
        KEY("Crisis", story->crisis, fread_number(fp));
        break;
      case 'D':
        KEY("Description", story->description, fread_string(fp));
        break;
      case 'E':
        KEY("Eidolon", story->eidolon, fread_string(fp));
        if (!str_cmp(word, "End")) {
          if (!story->author) {
            bug("Fread_Story: Name not found.", 0);
            free_storyidea(story);
            return;
          }
          StoryIdeaVect.push_back(story);
          return;
        }
        break;
      case 'F':
        KEY("Finale", story->finale, fread_number(fp));
        break;
      case 'G':
        if (!str_cmp(word, "Galleries")) {
          int i;
          i = fread_number(fp);
          story->galleries[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'M':
        KEY("Members", story->members, fread_string(fp));
        KEY("Mastermind", story->mastermind, fread_string(fp));
        break;
      case 'N':
        KEY("Npcs", story->npcs, fread_string(fp));
        break;
      case 'L':
        KEY("Lore", story->lore, fread_string(fp));
        break;
      case 'S':
        KEY("Secrets", story->secrets, fread_string(fp));
        KEY("Seeking", story->seeking_type, fread_number(fp));
        KEY("Status", story->status, fread_number(fp));
        KEY("SubFacOne", story->subfactionone, fread_number(fp));
        KEY("SubFacTwo", story->subfactiontwo, fread_number(fp));
        KEY("Storyline", story->storyline, fread_string(fp));
        break;
      case 'T':
        KEY("Tlocked", story->time_locked, fread_number(fp));
        KEY("Towned", story->time_owned, fread_number(fp));
        KEY("TMade", story->time_made, fread_number(fp));
        KEY("Type", story->type, fread_number(fp));
        KEY("Territory", story->territory, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_storyidea: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_storyideas() {
    nullstoryidea = new_storyidea();
    FILE *fp;

    if ((fp = fopen(STORYIDEA_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Storyideas: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "STORY")) {
          fread_storyidea(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Stoyideas: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open storyidea.txt", 0);
      exit(0);
    }
  }

  void save_storyideas_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/storyideas.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/storyideas.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/storyideas.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/storyideas.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/storyideas.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/storyideas.txt");
    else
    sprintf(buf, "../data/back7/storyideas.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open storyideas.txt for writing", 0);
      return;
    }

    for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
    it != StoryIdeaVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_storyidea: Blank story in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->status == STATUS_DELETE)
      continue;
      if ((*it)->status == STATUS_INCOMPLETE)
      continue;

      if ((*it)->status == STATUS_PENDING && current_time > (*it)->time_locked)
      (*it)->status = STATUS_TAPPROVED;
      if ((*it)->status == STATUS_LOCKED && current_time > (*it)->time_locked)
      (*it)->status = STATUS_APPROVED;

      fprintf(fpout, "#STORY\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Npcs %s~\n", (*it)->npcs);
      fprintf(fpout, "Lore %s~\n", (*it)->lore);
      fprintf(fpout, "Secrets %s~\n", (*it)->secrets);
      fprintf(fpout, "Eidolon %s~\n", (*it)->eidolon);
      fprintf(fpout, "Claimant %s~\n", (*it)->claimant);
      fprintf(fpout, "Crisis %d\n", (*it)->crisis);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "SubFacOne %d\n", (*it)->subfactionone);
      fprintf(fpout, "SubFacTwo %d\n", (*it)->subfactiontwo);
      fprintf(fpout, "Seeking %d\n", (*it)->seeking_type);
      fprintf(fpout, "Finale %d\n", (*it)->finale);
      fprintf(fpout, "Territory %s~\n", (*it)->territory);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "Tlocked %d\n", (*it)->time_locked);
      fprintf(fpout, "Towned %d\n", (*it)->time_owned);
      fprintf(fpout, "Mastermind %s\n~", (*it)->mastermind);
      fprintf(fpout, "Members %s~\n", (*it)->members);

      for (int i = 0; i < 10; i++) {
        if (safe_strlen((*it)->galleries[i]) > 1) {
          fprintf(fpout, "Galleries %d %s~\n", i, (*it)->galleries[i]);
        }
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_storyideas() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(STORYIDEA_FILE, "w")) == NULL) {
      bug("Cannot open storyideas.txt for writing", 0);
      return;
    }

    for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
    it != StoryIdeaVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_storyidea: Blank story in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->status == STATUS_DELETE)
      continue;
      if ((*it)->status == STATUS_INCOMPLETE)
      continue;

      if ((*it)->time_made == 0)
      (*it)->time_made = current_time;

      if ((*it)->time_made < (current_time - (3600 * 24 * 40)))
      continue;

      if ((*it)->status == STATUS_PENDING && current_time > (*it)->time_locked)
      (*it)->status = STATUS_TAPPROVED;
      if ((*it)->status == STATUS_LOCKED && current_time > (*it)->time_locked)
      (*it)->status = STATUS_APPROVED;

      fprintf(fpout, "#STORY\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Npcs %s~\n", (*it)->npcs);
      fprintf(fpout, "Lore %s~\n", (*it)->lore);
      fprintf(fpout, "Secrets %s~\n", (*it)->secrets);
      fprintf(fpout, "Claimant %s~\n", (*it)->claimant);
      fprintf(fpout, "Eidolon %s~\n", (*it)->eidolon);
      fprintf(fpout, "Crisis %d\n", (*it)->crisis);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "SubFacOne %d\n", (*it)->subfactionone);
      fprintf(fpout, "SubFacTwo %d\n", (*it)->subfactiontwo);
      fprintf(fpout, "Seeking %d\n", (*it)->seeking_type);
      fprintf(fpout, "Territory %s~\n", (*it)->territory);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Finale %d\n", (*it)->finale);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "Tlocked %d\n", (*it)->time_locked);
      fprintf(fpout, "Towned %d\n", (*it)->time_owned);
      fprintf(fpout, "TMade %d\n", (*it)->time_made);
      fprintf(fpout, "Mastermind %s~\n", (*it)->mastermind);
      fprintf(fpout, "Members %s~\n", (*it)->members);
      for (int i = 0; i < 10; i++) {
        if (safe_strlen((*it)->galleries[i]) > 1) {
          fprintf(fpout, "Galleries %d %s~\n", i, (*it)->galleries[i]);
        }
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_storyideas_backup();
  }

  vector<PLOT_TYPE *> PlotVect;

  PLOT_TYPE *nullplot;

  void fread_plot(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    PLOT_TYPE *plot;

    plot = new_plot();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", plot->author, fread_string(fp));
        break;

      case 'C':
        KEY("Calendar", plot->calendar, fread_number(fp));
        KEY("Challenge", plot->challenge, fread_string(fp));
        KEY("Character", plot->character, fread_string(fp));
        KEY("Crisis", plot->crisis, fread_number(fp));
        if (!str_cmp(word, "Comment")) {
          int i;
          i = fread_number(fp);
          plot->comments[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'E':
        KEY("Events", plot->events, fread_string(fp));
        KEY("Eidolon", plot->eidolon, fread_string(fp));
        if (!str_cmp(word, "End")) {
          if (!plot->author) {
            bug("Fread_plot: Name not found.", 0);
            free_plot(plot);
            return;
          }
          PlotVect.push_back(plot);
          return;
        }
        break;
      case 'F':
        KEY("Finale", plot->finale, fread_number(fp));
        break;
      case 'G':
        if (!str_cmp(word, "Galleries")) {
          int i;
          i = fread_number(fp);
          plot->galleries[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'I':
        KEY("Immortals", plot->immortals, fread_string(fp));
        break;
      case 'K':
        KEY("KarmaCap", plot->karmacap, fread_number(fp));
        KEY("KarmaEarned", plot->earnedkarma, fread_number(fp));
        break;
      case 'M':
        KEY("Members", plot->members, fread_string(fp));
        KEY("Mastermind", plot->mastermind, fread_string(fp));
        break;
      case 'P':
        if (!str_cmp(word, "Participant")) {
          int i;
          i = fread_number(fp);
          plot->participants[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'R':
        if (!str_cmp(word, "Rating")) {
          int i;
          i = fread_number(fp);
          plot->rating[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'S':
        KEY("Storyrunners", plot->storyrunners, fread_string(fp));
        KEY("Size", plot->size, fread_number(fp));
        KEY("Sponsored", plot->sponsored, fread_number(fp));
        KEY("Seeking", plot->seeking_type, fread_number(fp));
        KEY("SubFacOne", plot->subfactionone, fread_number(fp));
        KEY("SubFacTwo", plot->subfactiontwo, fread_number(fp));
        KEY("Storyline", plot->storyline, fread_string(fp));
        break;
      case 'T':
        KEY("Title", plot->title, fread_string(fp));
        KEY("Timezone", plot->timezone, fread_string(fp));
        KEY("Type", plot->type, fread_number(fp));
        KEY("Territory", plot->territory, fread_string(fp));
        KEY("TMade", plot->time_made, fread_number(fp));
        break;
      case 'V':
        KEY("Visibility", plot->visibility, fread_number(fp));
      }

      if (!fMatch) {
        sprintf(buf, "Fread_story: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_plots() {
    nullplot = new_plot();
    FILE *fp;

    if ((fp = fopen(PLOT_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_plots: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PLOT")) {
          fread_plot(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Stories: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open stories.txt", 0);
      exit(0);
    }
  }

  void save_plots_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/plots.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/plots.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/plots.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/plots.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/plots.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/plots.txt");
    else
    sprintf(buf, "../data/back7/plots.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open plots.txt for writing", 0);
      return;
    }

    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_stories: Blank plot in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      fprintf(fpout, "#PLOT\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Title %s~\n", (*it)->title);
      fprintf(fpout, "Challenge %s~\n", (*it)->challenge);
      fprintf(fpout, "Events %s~\n", (*it)->events);
      fprintf(fpout, "Character %s~\n", (*it)->character);
      fprintf(fpout, "Calendar %d\n", (*it)->calendar);
      fprintf(fpout, "Immortals %s~\n", (*it)->immortals);
      fprintf(fpout, "Storyrunners %s~\n", (*it)->storyrunners);
      fprintf(fpout, "Eidolon %s~\n", (*it)->eidolon);
      fprintf(fpout, "Size %d\n", (*it)->size);
      fprintf(fpout, "Territory %s~\n", (*it)->territory);
      fprintf(fpout, "Crisis %d\n", (*it)->crisis);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "SubFacOne %d\n", (*it)->subfactionone);
      fprintf(fpout, "SubFacTwo %d\n", (*it)->subfactiontwo);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "Seeking %d\n", (*it)->seeking_type);
      fprintf(fpout, "Members %s~\n", (*it)->members);
      fprintf(fpout, "Finale %d\n", (*it)->finale);
      fprintf(fpout, "Timezone %s~\n", (*it)->timezone);
      fprintf(fpout, "Visibility %d\n", (*it)->visibility);
      fprintf(fpout, "Sponsored %d\n", (*it)->sponsored);
      fprintf(fpout, "KarmaCap %d\n", (*it)->karmacap);
      fprintf(fpout, "KarmaEarned %d\n", (*it)->earnedkarma);
      fprintf(fpout, "TMade %d\n", (*it)->time_made);
      fprintf(fpout, "Mastermind %s~\n", (*it)->mastermind);

      for (int i = 0; i < 10; i++) {
        if (safe_strlen((*it)->galleries[i]) > 1) {
          fprintf(fpout, "Galleries %d %s~\n", i, (*it)->galleries[i]);
        }
      }
      for (int i = 0; i < 30; i++) {
        if ((*it)->rating[i] > 0) {
          fprintf(fpout, "Rating %d %d\n", i, (*it)->rating[i]);
          fprintf(fpout, "Participant %d %s~\n", i, (*it)->participants[i]);
          fprintf(fpout, "Comment %d %s~\n", i, (*it)->comments[i]);
        }
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_plots() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(PLOT_FILE, "w")) == NULL) {
      bug("Cannot open plots.txt for writing", 0);
      return;
    }

    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_stories: Blank plot in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->time_made == 0)
      (*it)->time_made = current_time;

      if (((*it)->type == PLOT_ADVENTURE || (*it)->type == PLOT_QUEST) && (*it)->time_made < (current_time - (3600 * 24 * 30)))
      continue;
      if ((*it)->time_made < (current_time - (3600 * 24 * 90)))
      continue;

      fprintf(fpout, "#PLOT\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Title %s~\n", (*it)->title);
      fprintf(fpout, "Challenge %s~\n", (*it)->challenge);
      fprintf(fpout, "Events %s~\n", (*it)->events);
      fprintf(fpout, "Character %s~\n", (*it)->character);
      fprintf(fpout, "Calendar %d\n", (*it)->calendar);
      fprintf(fpout, "Immortals %s~\n", (*it)->immortals);
      fprintf(fpout, "Eidolon %s~\n", (*it)->eidolon);
      fprintf(fpout, "Storyrunners %s~\n", (*it)->storyrunners);
      fprintf(fpout, "Size %d\n", (*it)->size);
      fprintf(fpout, "Territory %s~\n", (*it)->territory);
      fprintf(fpout, "Crisis %d\n", (*it)->crisis);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "SubFacOne %d\n", (*it)->subfactionone);
      fprintf(fpout, "SubFacTwo %d\n", (*it)->subfactiontwo);
      fprintf(fpout, "Seeking %d\n", (*it)->seeking_type);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "Members %s~\n", (*it)->members);
      fprintf(fpout, "Finale %d\n", (*it)->finale);
      fprintf(fpout, "Timezone %s~\n", (*it)->timezone);
      fprintf(fpout, "Visibility %d\n", (*it)->visibility);
      fprintf(fpout, "Sponsored %d\n", (*it)->sponsored);
      fprintf(fpout, "KarmaCap %d\n", (*it)->karmacap);
      fprintf(fpout, "KarmaEarned %d\n", (*it)->earnedkarma);
      fprintf(fpout, "TMade %d\n", (*it)->time_made);
      fprintf(fpout, "Mastermind %s~\n", (*it)->mastermind);
      for (int i = 0; i < 10; i++) {
        if (safe_strlen((*it)->galleries[i]) > 1) {
          fprintf(fpout, "Galleries %d %s~\n", i, (*it)->galleries[i]);
        }
      }

      for (int i = 0; i < 30; i++) {
        if ((*it)->rating[i] > 0) {
          fprintf(fpout, "Rating %d %d\n", i, (*it)->rating[i]);
          fprintf(fpout, "Participant %d %s~\n", i, (*it)->participants[i]);
          fprintf(fpout, "Comment %d %s~\n", i, (*it)->comments[i]);
        }
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_plots_backup();
  }

  char * ctype_name(int type)
  {
    if(type == 0)
    return "Other";
    if(type == CALENDAR_PARTY)
    return "Party";
    if(type == CALENDAR_SOCIAL)
    return "Social event";
    if(type == CALENDAR_PERFORMANCE)
    return "Performance";
    if(type == CALENDAR_SOCIETY)
    return "Society event";
    if(type == CALENDAR_PLOT)
    return "Plot";
    if(type == CALENDAR_EDUCATIONAL)
    return "Educational";

    return "";
  }


  bool can_see_calendar(CHAR_DATA *ch, STORY_TYPE *cal)
  {
    if(!str_cmp(ch->name, cal->author))
    return TRUE;
    if(IS_IMMORTAL(ch))
    return TRUE;
    if(cal->isprivate == 1)
    {
      if(strcasestr(cal->invite_list, ch->name) != NULL)
      return TRUE;
      else
      return FALSE;
    }


    for (int n = 0; n < 25; n++) {
      if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
        if (strcasestr(cal->hide_list, ch->pcdata->account->characters[n]) !=
            NULL)
        return FALSE;
      }
    }
    return TRUE;
  }

  bool can_enter_event(CHAR_DATA *ch, STORY_TYPE *cal)
  {
    if(!str_cmp(ch->name, cal->author))
    return TRUE;
    if(IS_IMMORTAL(ch))
    return TRUE;

    if(cal->created_at > current_time - (3600*6))
    return TRUE;
    if(cal->time > current_time)
    return TRUE;
    if(cal->time < current_time - (3600*6))
    return TRUE;
    if(safe_strlen(cal->security_letin_list) > 2)
    {
      if(strcasestr(cal->security_letin_list, ch->name) != NULL)
      return TRUE;
      else
      return FALSE;
    }
    if(safe_strlen(cal->security_ban_list) > 2)
    {
      if(strcasestr(cal->security_ban_list, ch->name) != NULL)
      return FALSE;
    }
    return TRUE;
  }
  _DOFUN(do_calendar) {

    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    STORY_TYPE *story;
    time_t storytime;
    time_t localtime;
    time_t optime;
    int i = 0;

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (!str_cmp(arg1, "list")) {
      int i = 1;
      for (vector<OPERATION_TYPE *>::iterator ot = OpVect.begin();
      ot != OpVect.end(); ++ot)
      (*ot)->calseen = FALSE;

      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;

        storytime = (*it)->time + (ch->pcdata->jetlag * 3600);

        for (vector<OPERATION_TYPE *>::iterator ot = OpVect.begin();
        ot != OpVect.end(); ++ot) {
          if ((*ot)->hour == 0)
          continue;
          if ((*ot)->faction != ch->faction && (*ot)->faction != ch->factiontwo && (*ot)->competition == COMPETE_CLOSED)
          continue;
          if ((*ot)->calseen == TRUE)
          continue;
          if ((*ot)->goal == GOAL_PSYCHIC)
          continue;
          optime = current_time - get_minute() * 60 - get_hour(NULL) * 3600 +
          (*ot)->hour * 3600 + (*ot)->day * 3600 * 24 +
          ch->pcdata->jetlag * 3600;
          if (get_hour(NULL) >= (*ot)->hour)
          optime = optime + 24 * 3600;
          if (optime < storytime) {
            (*ot)->calseen = TRUE;
            sprintf(buf, "%s's %s Operation", clan_lookup((*ot)->faction)->name, visible_goal((*ot)->goal));
            printf_to_char(ch, "`116|`W--`116|`x %s `116| `x%s", line_to_box(buf, "", 55), (char *)ctime(&optime));
          }
        }

        if (strcasestr((*it)->location, "dreamworld"))
        sprintf(buf, "Dreamworld's %s", (*it)->type);
        else
        sprintf(buf, "%s's %s", (*it)->author, (*it)->type);

        printf_to_char(ch, "`116|`W%02d`116|`x %s `116| `x%s", i, line_to_box(buf, "", 55), (char *)ctime(&storytime));

        //        if(strcasestr((*it)->location, "dreamworld"))
        //	    printf_to_char(ch, "`116|`W%02d`116|`x Dreamworld's %s
        //`116|`x%s`116|`x\n\r", i, line_to_box((*it)->type, "", 45), (char *)
        //ctime( &storytime ));
        //        else
        //            printf_to_char(ch, "`116|`W%02d`116|`x %s's %s
        //            `116|`x%s`116|`x\n\r", i, (*it)->author, //            line_to_box((*it)->type, "", 45), (char *) ctime( &storytime
        //            ));
        //        printf_to_char(ch, "`W%d`c)`x %s's %s\n\r", i, (*it)->author, //        (*it)->type);
        i++;
      }
      for (vector<OPERATION_TYPE *>::iterator ot = OpVect.begin();
      ot != OpVect.end(); ++ot) {
        if ((*ot)->hour == 0)
        continue;
        if ((*ot)->faction != ch->faction && (*ot)->faction != ch->factiontwo && (*ot)->competition == COMPETE_CLOSED)
        continue;
        if ((*ot)->calseen == TRUE)
        continue;
        if ((*ot)->goal == GOAL_PSYCHIC)
        continue;
        optime = current_time - get_minute() * 60 - get_hour(NULL) * 3600 +
        (*ot)->hour * 3600 + (*ot)->day * 3600 * 24 +
        ch->pcdata->jetlag * 3600;
        if (get_hour(NULL) >= (*ot)->hour)
        optime = optime + 24 * 3600;
        (*ot)->calseen = TRUE;
        sprintf(buf, "%s's %s Operation", clan_lookup((*ot)->faction)->name, visible_goal((*ot)->goal));
        printf_to_char(ch, "`116|`W--`116|`x %s `116| `x%s", line_to_box(buf, "", 55), (char *)ctime(&optime));
      }

    }
    else if (!str_cmp(arg1, "info")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          storytime = (*it)->time;
          localtime = (*it)->time + (ch->pcdata->jetlag * 3600);
          sprintf(buf, "%s", (char *)ctime(&localtime));
          if (strcasestr((*it)->location, "dreamworld"))
          printf_to_char(ch, "`WAuthor:`x Dreamworld\n`WTitle:`x %s\n`WIC Time  :`x %s\n`WYour Time:`x %s\n`WLocation:`x %s\n\r", (*it)->type, (char *)ctime(&storytime), buf, (*it)->location);
          else
          printf_to_char(ch, "`WAuthor:`x %s\n`WTitle:`x %s\n`WIC Time  :`x %s\n`WYour Time:`x %s\n`WLocation:`x %s\n\r", (*it)->author, (*it)->type, (char *)ctime(&storytime), buf, (*it)->location);
          printf_to_char(ch, "`WType:`x %s\n\r", ctype_name((*it)->ctype));
          if((*it)->isprivate == 1)
          send_to_char("`cPrivate Event`x\n\r", ch);

          if ((*it)->dedication != 0) {
            DOMAIN_TYPE *dom = vnum_domain((*it)->dedication);
            printf_to_char(ch, "`WDedicated to:`x %s\n\r", dom->domain_of);
          }
          if((*it)->isprotected == 1)
          send_to_char("`cSupernaturally Protected`x\n\r", ch);
          if (safe_strlen((*it)->stats) > 2)
          printf_to_char(ch, "`WStats relevent to getting involved:`x %s\n\r", (*it)->stats);
          if ((*it)->crisis == 1)
          send_to_char("`rCrisis`x\n\r", ch);
          printf_to_char(ch, "`WDescription:`x %s\n\r", (*it)->description);

          if (!str_cmp(ch->name, (*it)->author)) {
            for (i = 0; i < 50; i++) {
              if (str_cmp((*it)->comments[i], ""))
              printf_to_char(ch, "`WComment:`x %s\n\r", (*it)->comments[i]);
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "protect")) {
      if(!higher_power(ch))
      {
        send_to_char("Only Eidolons can extend their protection over an event.\n\r", ch);
        return;
      }
      int faccost = 100;
      FACTION_TYPE *cult = clan_lookup(ch->pcdata->eidilon_of);
      if(cult->resource < 8000 + faccost)
      {
        send_to_char("Your cult doesn't have enough resources to do that.\n\r", ch);
        return;
      }
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if((*it)->isprotected == 1)
          {
            send_to_char("That event is already protected.\n\r", ch);
            return;
          }
          (*it)->isprotected = 1;
          use_resources(faccost, cult->vnum, ch, "protecting an event.");
          send_to_char("Done.\n\r", ch);
          return;
        }

      }
    }
    else if (!str_cmp(arg1, "comment")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (safe_strlen(argument) < 2) {
          send_to_char("You have to leave more of a comment than that.\n\r", ch);
          return;
        }

        if (i == atoi(arg2)) {
          for (i = 0; i < 50; i++) {
            if (!str_cmp((*it)->comments[i], "")) {
              sprintf(buf, "%s: %s", ch->name, argument);
              free_string((*it)->comments[i]);
              (*it)->comments[i] = str_dup(buf);
              send_to_char("Done.\n\r", ch);
              return;
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "delete")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Calendar event deleted.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "securitypermitlist")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your event to change.\n\r", ch);
            return;
          }
          bool hassecurity = FALSE;
          if(safe_strlen((*it)->security_ban_list) > 2 || safe_strlen((*it)->security_letin_list) > 2)
          {
            hassecurity = TRUE;
          }
          free_string((*it)->security_ban_list);
          (*it)->security_ban_list = str_dup("");
          if(hassecurity == FALSE)
          ch->pcdata->total_money -= 10000;
          send_to_char("Enter a list of all the character names you want to be let in by security with spaces between them\n\r", ch);
          string_append(ch, &(*it)->security_letin_list);
          return;

        }
      }
    }
    else if (!str_cmp(arg1, "securitybanlist")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your event to change.\n\r", ch);
            return;
          }
          bool hassecurity = FALSE;
          if(safe_strlen((*it)->security_ban_list) > 2 || safe_strlen((*it)->security_letin_list) > 2)
          {
            hassecurity = TRUE;
          }
          free_string((*it)->security_letin_list);
          (*it)->security_letin_list = str_dup("");
          if(hassecurity == FALSE)
          ch->pcdata->total_money -= 10000;
          send_to_char("Enter a list of all the character names you want to be barred by security with spaces between them\n\r", ch);
          string_append(ch, &(*it)->security_ban_list);
          return;

        }
      }
    }
    else if (!str_cmp(arg1, "hidelist")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your event to change.\n\r", ch);
            return;
          }
          send_to_char("Enter a list of all the character names you want to not be able to see this event with spaces between them.\n\r", ch);
          string_append(ch, &(*it)->hide_list);
          return;

        }
      }
    }
    else if (!str_cmp(arg1, "invitelist")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your event to change.\n\r", ch);
            return;
          }
          send_to_char("Enter a list of all the character names you want to be invited with spaces between them.\n\r", ch);
          string_append(ch, &(*it)->invite_list);
          return;

        }
      }
    }
    else if (!str_cmp(arg1, "private")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your event to change.\n\r", ch);
            return;
          }
          if((*it)->isprivate == 1)
          {
            send_to_char("You make your event public again.\n\r", ch);
            (*it)->isprivate = 0;
            return;
          }
          else
          {
            send_to_char("You make your event private.\n\r", ch);
            (*it)->isprivate = 1;
            return;
          }
        }
      }
    }
    else if (!str_cmp(arg1, "type")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your event to change.\n\r", ch);
            return;
          }
          if(!str_cmp(argument, "Party"))
          (*it)->ctype = CALENDAR_PARTY;
          else if(!str_cmp(argument, "Social event"))
          (*it)->ctype = CALENDAR_SOCIAL;
          else if(!str_cmp(argument, "Performance"))
          (*it)->ctype = CALENDAR_PERFORMANCE;
          else if(!str_cmp(argument, "Society event"))
          (*it)->ctype = CALENDAR_SOCIETY;
          else if(!str_cmp(argument, "Plot"))
          (*it)->ctype = CALENDAR_PLOT;
          else if(!str_cmp(argument, "Educational"))
          (*it)->ctype = CALENDAR_EDUCATIONAL;
          else
          {
            send_to_char("Valid types are: Party, Social event, Performance, Society event, Plot, Educational\n\r", ch);
            return;
          }
          send_to_char("Type set.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "create")) {

      story = new_story();
      free_string(story->author);
      story->author = str_dup(ch->name);
      story->time = current_time + (3600 * 24);
      story->created_at = current_time;
      StoryVect.push_back(story);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "dedicate")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          DOMAIN_TYPE *dom = domain_by_name(argument);
          if (dom == NULL) {
            send_to_char("No such domain.\n\r", ch);
            return;
          }
          (*it)->dedication = dom->vnum;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "title")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          free_string((*it)->type);
          (*it)->type = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "location")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          free_string((*it)->location);
          (*it)->location = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "description")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->description);
        }
      }
    }
    else if (!str_cmp(arg1, "stats")) {
      if (!is_gm(ch)) {
        send_to_char("This is for storyrunners only.\n\r", ch);
        return;
      }

      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          free_string((*it)->stats);
          (*it)->stats = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "timeminutes")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (!is_number(argument)) {
          send_to_char("Syntax: Calendar timeminutes (number) (offset from current time in minutes)\n\r", ch);
          return;
        }

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          (*it)->time = (*it)->time + atoi(argument) * 60;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "timehours")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (!is_number(argument)) {
          send_to_char("Syntax: Calendar timehours (number) (offset from current time in hours)\n\r", ch);
          return;
        }

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          (*it)->time = (*it)->time + atoi(argument) * 3600;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "timedays")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;

        i++;

        if (!is_number(argument)) {
          send_to_char("Syntax: Calendar timedays (number) (offset from current time in days)\n\r", ch);
          return;
        }

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          (*it)->time = (*it)->time + atoi(argument) * 86400;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "timereset")) {
      for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
      it != StoryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!can_see_calendar(ch, (*it)))
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your calendar event to change.\n\r", ch);
            return;
          }
          (*it)->time = current_time;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else
    send_to_char("Syntax: calendar list, calendar info (number), calendar comment (number) (Comment), calendar delete (number), calendar create, calendar title (number) (type), calendar location (number) (location), calendar stats (number) (stats), calendar description (number), calendar timereset (number), calendar timeminutes (number) (minutes), calendar timehours (number) (hours), calendar timedays (number) (days)\n\r", ch);
  }

  int storycount(void) {
    int i = 0;
    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      i++;
    }
    return i;
  }

  bool gm_active(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (safe_strlen(ch->pcdata->storyrunner) < 2)
    return FALSE;
    if (ch->pcdata->stories_run > 0 && ch->pcdata->rooms_run > 1)
    return TRUE;

    return FALSE;
  }

  vector<RESEARCH_TYPE *> ResearchVect;

  RESEARCH_TYPE *nullresearch;

  void fread_research(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    RESEARCH_TYPE *research;

    research = new_research();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", research->author, fread_string(fp));
        break;

      case 'D':
        KEY("Difficulty", research->difficulty, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!research->author) {
            bug("Fread_researc: Name not found.", 0);
            free_research(research);
            return;
          }
          ResearchVect.push_back(research);
          return;
        }
        break;
      case 'M':
        KEY("Message", research->message, fread_string(fp));
        break;
      case 'P':
        KEY("Participants", research->participants, fread_string(fp));
        break;
      case 'S':
        if (!str_cmp(word, "Stats")) {
          for (int i = 0; i < 10; i++)
          research->stats[i] = fread_number(fp);
        }
        fMatch = TRUE;
        break;
      case 'T':
        KEY("Timer", research->timer, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_research: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_research() {
    nullresearch = new_research();
    FILE *fp;

    if ((fp = fopen(RESEARCH_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Research: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "RESEARCH")) {
          fread_research(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Research: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open research.txt", 0);
      exit(0);
    }
  }

  void save_research() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(RESEARCH_FILE, "w")) == NULL) {
      bug("Cannot open research.txt for writing", 0);
      return;
    }

    for (vector<RESEARCH_TYPE *>::iterator it = ResearchVect.begin();
    it != ResearchVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_research: Blank research in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->timer <= 0)
      continue;

      fprintf(fpout, "#RESEARCH\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Message %s~\n", (*it)->message);
      fprintf(fpout, "Participants %s~\n", (*it)->participants);
      fprintf(fpout, "Difficulty %d\n", (*it)->difficulty);
      fprintf(fpout, "Stats %d %d %d %d %d %d %d %d %d %d\n", (*it)->stats[0], (*it)->stats[1], (*it)->stats[2], (*it)->stats[3], (*it)->stats[4], (*it)->stats[5], (*it)->stats[6], (*it)->stats[7], (*it)->stats[8], (*it)->stats[9]);
      fprintf(fpout, "Timer %d\n", (*it)->timer);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }



  vector<EXTRA_ENCOUNTER_TYPE *> EEncounterVect;

  EXTRA_ENCOUNTER_TYPE *nullenc;

  void fread_eencounter(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    EXTRA_ENCOUNTER_TYPE *enc;

    enc = new_eencounter();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'B':
        KEY("Btype", enc->btype, fread_number(fp));
        break;
      case 'C':
        KEY("ClanID", enc->clan_id, fread_number(fp));
        break;
      case 'E':
        KEY("EncounterID", enc->encounter_id, fread_number(fp));
        if (!str_cmp(word, "End")) {
          if (!enc->text) {
            bug("Fread_encounter: Text not found.", 0);
            free_eencounter(enc);
            return;
          }
          EEncounterVect.push_back(enc);
          return;
        }
        break;
      case 'I':
        KEY("ID", enc->id, fread_number(fp));
        break;
      case 'T':
        KEY("Text", enc->text, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_encounter: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_eencounter() {
    nullenc = new_eencounter();
    FILE *fp;

    if ((fp = fopen(ENCOUNTER_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Encounters: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "ENCOUNTER")) {
          fread_eencounter(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Encounter: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open research.txt", 0);
      exit(0);
    }
  }

  void save_eencounter() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(ENCOUNTER_FILE, "w")) == NULL) {
      bug("Cannot open encounters.txt for writing", 0);
      return;
    }

    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if (!(*it)->text || (*it)->text[0] == '\0') {
        bug("Save_encounters: Blank encounter in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if((*it)->clan_id > 0)
      {
        FACTION_TYPE *fac = clan_lookup((*it)->clan_id);
        if(fac == NULL)
        continue;
        if(strlen(fac->eidilon) < 2)
        continue;
        if(!character_exists(fac->eidilon))
        continue;

      }

      fprintf(fpout, "#ENCOUNTER\n");
      fprintf(fpout, "ID %d\n", (*it)->id);
      fprintf(fpout, "Btype %d\n", (*it)->btype);
      fprintf(fpout, "ClanID %d\n", (*it)->clan_id);
      fprintf(fpout, "EncounterID %d\n", (*it)->encounter_id);
      fprintf(fpout, "Text %s~\n", (*it)->text);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }


  void add_encounter(int etype, int ebase, char * text, int clan_id)
  {
    int initid = 0;
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if ((*it)->id > initid)
      initid = (*it)->id;
    }
    EXTRA_ENCOUNTER_TYPE *enc = new_eencounter();
    enc->id = initid + 1;
    enc->btype = etype;
    enc->clan_id = clan_id;
    enc->encounter_id = ebase;
    free_string(enc->text);
    enc->text = str_dup(text);
    enc->valid = TRUE;
    EEncounterVect.push_back(enc);
    save_eencounter();
  }






  vector<EXTRA_PATROL_TYPE *> EPatrolVect;

  EXTRA_PATROL_TYPE *nullpat;

  void fread_epatrol(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    EXTRA_PATROL_TYPE *pat;

    pat = new_epatrol();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'C':
        KEY("ClanID", pat->clan_id, fread_number(fp));
        KEY("Conclusion", pat->conclusion, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!pat->intro) {
            bug("Fread_patrol: Text not found.", 0);
            free_epatrol(pat);
            return;
          }
          EPatrolVect.push_back(pat);
          return;
        }
        break;
      case 'I':
        KEY("ID", pat->id, fread_number(fp));
        KEY("Intro", pat->intro, fread_string(fp));
        break;
      case 'M':
        KEY("Messages", pat->messages, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_patrol: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_epatrol() {
    nullpat = new_epatrol();
    FILE *fp;

    if ((fp = fopen(PATROL_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Patrols: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PATROL")) {
          fread_epatrol(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Patrol: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open patrols.txt", 0);
      exit(0);
    }
  }

  void save_epatrol() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(PATROL_FILE, "w")) == NULL) {
      bug("Cannot open patrols.txt for writing", 0);
      return;
    }

    for (vector<EXTRA_PATROL_TYPE *>::iterator it = EPatrolVect.begin();
    it != EPatrolVect.end(); ++it) {
      if (!(*it)->intro || (*it)->intro[0] == '\0') {
        bug("Save_patrols: Blank patrol in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if((*it)->clan_id > 0)
      {
        FACTION_TYPE *fac = clan_lookup((*it)->clan_id);
        if(fac == NULL)
        continue;
        if(strlen(fac->eidilon) < 2)
        continue;
        if(!character_exists(fac->eidilon))
        continue;

      }

      fprintf(fpout, "#PATROL\n");
      fprintf(fpout, "ID %d\n", (*it)->id);
      fprintf(fpout, "ClanID %d\n", (*it)->clan_id);
      fprintf(fpout, "Intro %s~\n", (*it)->intro);
      fprintf(fpout, "Messages %s~\n", (*it)->messages);
      fprintf(fpout, "Conclusion %s~\n", (*it)->conclusion);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }



  vector<ANNIVERSARY_TYPE *> AnniversaryVect;

  ANNIVERSARY_TYPE *nullann;

  void fread_anniversary(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    ANNIVERSARY_TYPE *ann;

    ann = new_anniversary();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'B':
        KEY("Btype", ann->btype, fread_number(fp));
        break;
      case 'C':
        KEY("CultName", ann->cult_name, fread_string(fp));
        break;
      case 'E':
        KEY("EidilonName", ann->eidilon_name, fread_string(fp));
        if (!str_cmp(word, "End")) {
          if (!ann->eidilon_name) {
            bug("Fread_anniverary: Text not found.", 0);
            free_anniversary(ann);
            return;
          }
          AnniversaryVect.push_back(ann);
          return;
        }
        break;
      case 'I':
        KEY("ID", ann->id, fread_number(fp));
        break;
      case 'H':
        KEY("HappenAt", ann->happen_at, fread_number(fp));
        break;


      case 'M':
        KEY("Messages", ann->messages, fread_string(fp));
        break;

      case 'N':
        KEY("News", ann->news, fread_string(fp));
        break;
      case 'S':
        KEY("Summary", ann->summary, fread_string(fp));
        KEY("SectName", ann->sect_name, fread_string(fp));
        break;

      }

      if (!fMatch) {
        sprintf(buf, "Fread_anniverasry: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_anniversary() {
    nullann = new_anniversary();
    FILE *fp;

    if ((fp = fopen(ANNIVERSARY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Anniversaries: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "ANNIVERSARY")) {
          fread_anniversary(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Anniversary: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open anniversaries.txt", 0);
      exit(0);
    }
  }

  void save_anniversaries() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(ANNIVERSARY_FILE, "w")) == NULL) {
      bug("Cannot open anniversaries.txt for writing", 0);
      return;
    }

    for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
    it != AnniversaryVect.end(); ++it) {
      if (!(*it)->eidilon_name || (*it)->eidilon_name[0] == '\0') {
        bug("Save_anniversaries: Blank anniversary in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      fprintf(fpout, "#ANNIVERSARY\n");
      fprintf(fpout, "ID %d\n", (*it)->id);
      fprintf(fpout, "Btype %d\n", (*it)->btype);
      fprintf(fpout, "CultName %s~\n", (*it)->cult_name);
      fprintf(fpout, "EidilonName %s~\n", (*it)->eidilon_name);
      fprintf(fpout, "HappenAt %d\n", (*it)->happen_at);
      fprintf(fpout, "Messages %s~\n", (*it)->messages);
      fprintf(fpout, "News %s~\n", (*it)->news);
      fprintf(fpout, "Summary %s~\n", (*it)->summary);
      fprintf(fpout, "SectName %s~\n", (*it)->sect_name);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }



  vector<DECREE_TYPE *> DecreeVect;

  DECREE_TYPE *nulldec;

  void fread_decree(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    DECREE_TYPE *dec;

    dec = new_decree();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'B':
        KEY("Btype", dec->btype, fread_number(fp));
        break;
      case 'C':
        KEY("CreatedAt", dec->created_at, fread_number(fp));
        break;
      case 'D':
        KEY("Desc", dec->desc, fread_string(fp));
      case 'E':
        KEY("EndTime", dec->end_time, fread_number(fp));
        if (!str_cmp(word, "End")) {
          if (!dec->btype) {
            bug("Fread_decree: Text not found.", 0);
            free_decree(dec);
            return;
          }
          DecreeVect.push_back(dec);
          return;
        }
        break;
      case 'S':
        KEY("StartTime", dec->start_time, fread_number(fp));
        break;
      case 'T':
        KEY("Target", dec->target, fread_string(fp));
        KEY("TargetNumber", dec->target_number, fread_number(fp));
        KEY("TerritoryVnum", dec->territory_vnum, fread_number(fp));
        break;
      case 'V':
        KEY("Vote1", dec->vote_1, fread_string(fp));
        KEY("Vote2", dec->vote_2, fread_string(fp));
        KEY("Vote3", dec->vote_3, fread_string(fp));


        break;


      }

      if (!fMatch) {
        sprintf(buf, "Fread_decree: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_decrees() {
    nulldec = new_decree();
    FILE *fp;

    if ((fp = fopen(DECREE_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Decrees: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "DECREE")) {
          fread_decree(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Decree: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open decrees.txt", 0);
      exit(0);
    }
  }

  struct recent_decree {
    inline bool operator()(const DECREE_TYPE *struct1, const DECREE_TYPE *struct2) {
      return (struct1->created_at > struct2->created_at);
    }
  };


  void save_decrees() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(DECREE_FILE, "w")) == NULL) {
      bug("Cannot open decrees.txt for writing", 0);
      return;
    }

    std::sort(DecreeVect.begin(), DecreeVect.end(), recent_decree());

    for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
    it != DecreeVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if((*it)->created_at < current_time - (3600*24*90))
      continue;

      fprintf(fpout, "#DECREE\n");
      fprintf(fpout, "Btype %d\n", (*it)->btype);
      fprintf(fpout, "CreatedAt %d\n", (*it)->created_at);
      fprintf(fpout, "Desc %s~\n", (*it)->desc);
      fprintf(fpout, "EndTime %d\n", (*it)->end_time);
      fprintf(fpout, "StartTime %d\n", (*it)->start_time);
      fprintf(fpout, "Target %s~\n", (*it)->target);
      fprintf(fpout, "TargetNumber %d\n", (*it)->target_number);
      fprintf(fpout, "TerritoryVnum %d\n", (*it)->territory_vnum);
      fprintf(fpout, "Vote1 %s~\n", (*it)->vote_1);
      fprintf(fpout, "Vote2 %s~\n", (*it)->vote_2);
      fprintf(fpout, "Vote3 %s~\n", (*it)->vote_3);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }






  vector<NEWS_TYPE *> NewsVect;

  NEWS_TYPE *nullnews;

  void fread_news(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    NEWS_TYPE *news;

    news = new_news();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", news->author, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!news->author) {
            bug("Fread_news: Name not found.", 0);
            free_news(news);
            return;
          }
          NewsVect.push_back(news);
          return;
        }
        break;
      case 'M':
        KEY("Message", news->message, fread_string(fp));
        break;
      case 'S':
        if (!str_cmp(word, "Stats")) {
          for (int i = 0; i < 10; i++)
          news->stats[i] = fread_number(fp);
        }
        fMatch = TRUE;
        break;
      case 'T':
        KEY("Timer", news->timer, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_news: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_news() {
    nullnews = new_news();
    FILE *fp;

    if ((fp = fopen(NEWS_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_News: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "NEWS")) {
          fread_news(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_NEWS: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open news.txt", 0);
      exit(0);
    }
  }

  void save_news_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/news.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/news.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/news.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/news.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/news.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/news.txt");
    else
    sprintf(buf, "../data/back7/news.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open news.txt for writing", 0);
      return;
    }

    for (vector<NEWS_TYPE *>::iterator it = NewsVect.begin();
    it != NewsVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_research: Blank news in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      fprintf(fpout, "#NEWS\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Message %s~\n", (*it)->message);
      fprintf(fpout, "Stats %d %d %d %d %d %d %d %d %d %d\n", (*it)->stats[0], (*it)->stats[1], (*it)->stats[2], (*it)->stats[3], (*it)->stats[4], (*it)->stats[5], (*it)->stats[6], (*it)->stats[7], (*it)->stats[8], (*it)->stats[9]);
      fprintf(fpout, "Timer %d\n", (*it)->timer);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_news() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(NEWS_FILE, "w")) == NULL) {
      bug("Cannot open news.txt for writing", 0);
      return;
    }

    for (vector<NEWS_TYPE *>::iterator it = NewsVect.begin();
    it != NewsVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_research: Blank news in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      fprintf(fpout, "#NEWS\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Message %s~\n", (*it)->message);
      fprintf(fpout, "Stats %d %d %d %d %d %d %d %d %d %d\n", (*it)->stats[0], (*it)->stats[1], (*it)->stats[2], (*it)->stats[3], (*it)->stats[4], (*it)->stats[5], (*it)->stats[6], (*it)->stats[7], (*it)->stats[8], (*it)->stats[9]);
      fprintf(fpout, "Timer %d\n", (*it)->timer);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_news_backup();
  }

  bool can_see_news(CHAR_DATA *ch, NEWS_TYPE *news) {
    if (news->stats[0] == 0)
    return TRUE;

    if (news->stats[0] == -1 && is_gm(ch))
    return TRUE;

    if (news->stats[0] == -2 && (is_super(ch) || ch->faction != 0))
    return TRUE;

    for (int i = 0; i < 10; i++) {
      if (news->stats[i] > 0 && news->stats[i] < 99 && get_skill(ch, news->stats[i]) > 0)
      return TRUE;
    }
    for (int i = 0; i < 10; i++) {
      if (news->stats[i] > 0 && ch->faction == news->stats[i] - 100)
      return TRUE;
    }

    return FALSE;
  }

  void show_news(CHAR_DATA *ch) {
    int count = 0;
    for (vector<NEWS_TYPE *>::iterator it = NewsVect.begin();
    it != NewsVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      if (can_see_news(ch, (*it))) {
        if (count == 0)
        send_to_char("`W  In news;`x\n\r", ch);

        printf_to_char(ch, "\t%s - (%s)\n\r", (*it)->message, (*it)->author);
        count++;
      }
    }
  }

  void news_update(void) {
    for (vector<NEWS_TYPE *>::iterator it = NewsVect.begin();
    it != NewsVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      (*it)->timer--;

      if ((*it)->timer == 0) {
        (*it)->valid = FALSE;
      }
    }
  }

  _DOFUN(do_news) {

    if (ch->pcdata->ci_editing == 20) {
      ANNIVERSARY_TYPE *ann;
      for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
      it != AnniversaryVect.end(); ++it) {
        if ((*it)->id == ch->pcdata->ci_discipline) {
          ann = *it;
          break;
        }
      }
      if(ann != NULL)
      {
        ch->pcdata->ci_absorb = 1;
        string_append(ch, &ann->news);
        send_to_char("Write the news for this event.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(argument, "delete")) {
      for (vector<NEWS_TYPE *>::iterator it = NewsVect.begin();
      it != NewsVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->timer <= 0)
        continue;

        if (!str_cmp(ch->name, (*it)->author)) {
          send_to_char("News deleted.\n\r", ch);
          (*it)->timer = 0;
          (*it)->valid = FALSE;
          return;
        }
      }
      return;
    }
    news_feed(ch, 1);
    //    show_news(ch);
  }

  void research_to_char(RESEARCH_TYPE *research, char *name) {
    char buf[MSL];
    sprintf(buf, "Your research is successful, you learn: %s\n\r", research->message);
    message_to_char(name, buf);
  }

  void finish_research(RESEARCH_TYPE *research) {
    char arg[MSL];
    for (int i = 0; i < 10; i++) {
      research->participants = one_argument_nouncap(research->participants, arg);
      if (safe_strlen(arg) > 1)
      research_to_char(research, arg);
      else
      break;
    }
    message_to_char(research->author, "Players finish your research.");
    research->valid = FALSE;
  }

  void research_update(void) {

    for (vector<RESEARCH_TYPE *>::iterator it = ResearchVect.begin();
    it != ResearchVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      (*it)->timer--;

      if ((*it)->timer == 0) {
        finish_research((*it));
        break;
      }
    }
  }

  void char_to_research(char *name, RESEARCH_TYPE *research) {
    int val;
    for (int i = 0; i < 10; i++) {
      val = offline_skill(name, research->stats[i]);
      if (val > 0) {
        for (int j = 0; j < val; j++) {
          research->timer = research->timer * 8 / 10;
        }
      }
    }
  }

  bool doing_research(CHAR_DATA *ch) {

    for (vector<RESEARCH_TYPE *>::iterator it = ResearchVect.begin();
    it != ResearchVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      if (is_name(ch->name, (*it)->participants))
      return TRUE;
    }
    return FALSE;
  }
  RESEARCH_TYPE *get_research(CHAR_DATA *ch) {

    for (vector<RESEARCH_TYPE *>::iterator it = ResearchVect.begin();
    it != ResearchVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->timer <= 0)
      continue;

      if (is_name(ch->name, (*it)->participants))
      return (*it);
    }
    return NULL;
  }

  _DOFUN(do_participant) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 8) {
      ch->pcdata->ci_absorb = 1;
      free_string(ch->pcdata->ci_name);
      ch->pcdata->ci_name = str_dup(argument);
      printf_to_char(ch, "Participant set to: %s\n\r", ch->pcdata->ci_name);
    }
  }
  _DOFUN(do_difficulty) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 8) {
      ch->pcdata->ci_absorb = 1;
      if (atoi(argument) < 1 || atoi(argument) > 10) {
        send_to_char("Syntax: Difficulty 1-10\n\r", ch);
        return;
      }
      ch->pcdata->ci_discipline = atoi(argument);
      send_to_char("Done.\n\r", ch);
    }
  }
  void story_update(void) {
    int i = 1;

    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->time + (3600 * 12) < current_time) {
        (*it)->valid = FALSE;
      }
      i++;
    }
  }

  void scon_update(CHAR_DATA *ch) {
    for (int i = 0; i < 20; i++) {
      if (ch->pcdata->storycon[i][1] == 1) {
        if (get_gm(ch->in_room, TRUE) == NULL) {
          ch->pcdata->storycon[i][0] = 0;
          ch->pcdata->storycon[i][1] = 0;
          ch->pcdata->storycon[i][2] = 0;
          continue;
        }
      }
      if (ch->pcdata->storycon[i][2] > 0) {
        ch->pcdata->storycon[i][2]--;
        if (ch->pcdata->storycon[i][2] <= 0) {
          ch->pcdata->storycon[i][0] = 0;
          ch->pcdata->storycon[i][1] = 0;
          ch->pcdata->storycon[i][2] = 0;
          continue;
        }
      }
    }
  }

  void sralert(char *message) {
    CHAR_DATA *target;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;

      target = CH(d);

      if (IS_NPC(target) || !is_gm(target))
      continue;

      printf_to_char(target, "[`GSAlert`x] %s\n\r", message);
    }
  }

  void srnews(char *message) {
    NEWS_TYPE *news;

    news = new_news();
    news->timer = 1200;
    news->stats[0] = -1;
    free_string(news->message);
    news->message = str_dup(message);
    NewsVect.push_back(news);
  }

  void scon_to_char(CHAR_DATA *ch, int condition, int trust) {
    bool found = FALSE;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->storycon[i][0] == condition) {
        if (condition == SCON_POLICE || condition == SCON_HUNTERS) {
          ch->pcdata->storycon[i][1]++;
          ch->pcdata->storycon[i][2] =
          UMAX((ch->pcdata->storycon[i][2] + 4300) * 2, ch->pcdata->storycon[i][2] + 8600);
        }
        else if (trust >= ch->pcdata->storycon[i][1]) {
          ch->pcdata->storycon[i][1] = trust;
          ch->pcdata->storycon[i][2] += trust * trust * 4300;
        }
        found = TRUE;
      }
    }
    if (found == FALSE) {
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->storycon[i][0] == 0) {
          ch->pcdata->storycon[i][0] = condition;
          if (condition == SCON_POLICE || condition == SCON_HUNTERS) {
            ch->pcdata->storycon[i][1] = 1;
            ch->pcdata->storycon[i][2] = 8600;
          }
          else {
            ch->pcdata->storycon[i][1] = trust;
            ch->pcdata->storycon[i][2] = trust * trust * 4300;
          }
          i = 10;
          break;
        }
      }
    }
  }

  void scon_from_char(CHAR_DATA *ch, int condition, int trust) {
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->storycon[i][0] == condition) {
        if (condition == SCON_POLICE || condition == SCON_HUNTERS) {
          ch->pcdata->storycon[i][1]--;
          ch->pcdata->storycon[i][2] /= 2;
          if (ch->pcdata->storycon[i][1] <= 0 || ch->pcdata->storycon[i][2] <= 0) {
            ch->pcdata->storycon[i][0] = 0;
            ch->pcdata->storycon[i][1] = 0;
            ch->pcdata->storycon[i][2] = 0;
          }
        }
        else if (trust >= ch->pcdata->storycon[i][1]) {
          ch->pcdata->storycon[i][0] = 0;
          ch->pcdata->storycon[i][1] = 0;
          ch->pcdata->storycon[i][2] = 0;
        }
      }
    }
  }

  _DOFUN(do_inflict) {

    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }
    if (battleground(ch->in_room))
    return;

    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_world(ch, arg1);

    if (victim == NULL || IS_NPC(victim)) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg2);

    if (get_gmtrust(ch, victim) < 1 && str_cmp(arg2, "hunters") && str_cmp(arg2, "police")) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }
    if (higher_power(ch) && power_bound(ch) && !IS_NPC(victim) && str_cmp(ch->pcdata->place, victim->pcdata->place)) {
      send_to_char("Your power is bound.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "weaken")) {
      scon_to_char(victim, SCON_WEAKEN, get_gmtrust(ch, victim));
      send_to_char("You weaken them.\n\r", ch);
      send_to_char("You feel weaker.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "nomove")) {
      scon_to_char(victim, SCON_NOMOVE, get_gmtrust(ch, victim));
      send_to_char("You stop them moving around.\n\r", ch);
      send_to_char("You feel unable to move around.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "localmove")) {
      scon_to_char(victim, SCON_LOCALMOVE, get_gmtrust(ch, victim));
      send_to_char("You stop them moving around beyond the local area.\n\r", ch);
      send_to_char("You feel unable to move around beyond the local area.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "noheal")) {
      scon_to_char(victim, SCON_NOHEAL, get_gmtrust(ch, victim));
      send_to_char("You stop them healing.\n\r", ch);
      send_to_char("You feel like you're recovering less quickly.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "police")) {
      if (!senior_gm(ch))
      return;
      if (safe_strlen(argument) < 10) {
        send_to_char("You need to add a reason to the end of that.\n\r", ch);
        return;
      }
      /*
if(ch->pcdata->cop_cooldown > 0)
{
send_to_char("You did that too recently\n\r", ch);
return;
}
*/
      scon_to_char(victim, SCON_POLICE, 1);
      send_to_char("You send the police after them.\n\r", ch);
      sprintf(buf, "%s sends the police after %s for %s", NAME(ch), NAME(victim), argument);
      sralert(buf);
      srnews(buf);
      ch->pcdata->cop_cooldown = 10000;
    }
    else if (!str_cmp(arg2, "hunters")) {
      if (!senior_gm(ch))
      return;

      if (safe_strlen(argument) < 10) {
        send_to_char("You need to add a reason to the end of that.\n\r", ch);
        return;
      }
      /*
if(ch->pcdata->cop_cooldown > 0)
{
send_to_char("You did that too recently\n\r", ch);
return;
}
*/
      scon_to_char(victim, SCON_HUNTERS, 1);
      send_to_char("You send hunters after them.\n\r", ch);
      sprintf(buf, "%s sends hunters after %s for %s", NAME(ch), NAME(victim), argument);
      sralert(buf);
      srnews(buf);
      ch->pcdata->cop_cooldown = 10000;
    }
    else
    send_to_char("Available conditions are: weaken, noheal.\n\r", ch);
  }

  _DOFUN(do_swalk) {
    char arg1[MSL];
    int i;
    ROOM_INDEX_DATA *room = NULL;

    argument = one_argument_nouncap(argument, arg1);

    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("This is a StoryRunner command.\n\r", ch);
      return;
    }
    if (higher_power(ch) && power_bound(ch)) {
      send_to_char("You cannot leave the binding circle.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum <= 300 && ch->in_room->vnum != 60)
    return;

    if (!str_cmp(arg1, "operation") && !higher_power(ch)) {
      operation_swalk(ch);
      return;
    }
    if (!str_cmp(arg1, "list")) {
      send_to_char("You can mist walk to the following locations.\n\r", ch);

      for (i = 0; i < MAX_TAXIS; i++) {
        if ((room = get_room_index(taxi_table[i].vnum)) != NULL)
        printf_to_char(ch, "`g[`W%4d`g]`x %s\n\r", i + 1, room->name);
      }

      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;

        if (room->size <= 200 && str_cmp((*it)->author, ch->name))
        continue;

        if (room->area->vnum != 41)
        continue;

        printf_to_char(ch, "`g[`W%4d`g]`x %s\n\r", get_po(room), room->name);
      }
      send_to_char("\n", ch);
      for (i = 0; i < 10; i++) {
        if (ch->pcdata->driveloc[i] > 0 && (room = get_room_index(ch->pcdata->driveloc[i])) != NULL) {
          printf_to_char(ch, "`W[`c%s`W]`x %s\n\r", ch->pcdata->drivenames[i], room->name);
        }
      }
      return;
    }
    CHAR_DATA *victim;
    victim = get_char_world_pc(arg1);

    if (victim != NULL && !is_number(arg1)) {
      if (get_gmtrust(ch, victim) >= 3 || (get_gmtrust(ch, victim) >= 1 && higher_power(ch)) || (is_wildsapproved(ch) && IS_FLAG(victim->comm, COMM_WHEREVIS) && in_world(victim) != WORLD_EARTH)) {
        room = victim->in_room;

        if (in_lodge(room) || base_room(room) || room->vnum < 300) {
          send_to_char("You can't go there.\n\r", ch);
          return;
        }
      }
      else {
        send_to_char("You cannot go there.\n\r", ch);
        return;
      }
    }
    else {
      if (!is_number(arg1)) {
        send_to_char("`WSwalk `g(`Wnumber`g) (`Wpassengers`g)`x\n\r", ch);
        return;
      }
      int desti = landmark_vnum(arg1, ch);
      if (desti == 0) {
        if (!str_cmp(arg1, ch->pcdata->drivenames[0]) && ch->pcdata->driveloc[0] > 0 && get_room_index(ch->pcdata->driveloc[0]) != NULL) {
          desti = ch->pcdata->driveloc[0];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[1]) && ch->pcdata->driveloc[1] > 0 && get_room_index(ch->pcdata->driveloc[1]) != NULL) {
          desti = ch->pcdata->driveloc[1];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[2]) && ch->pcdata->driveloc[2] > 0 && get_room_index(ch->pcdata->driveloc[2]) != NULL) {
          desti = ch->pcdata->driveloc[2];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[3]) && ch->pcdata->driveloc[3] > 0 && get_room_index(ch->pcdata->driveloc[3]) != NULL) {
          desti = ch->pcdata->driveloc[3];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[4]) && ch->pcdata->driveloc[4] > 0 && get_room_index(ch->pcdata->driveloc[4]) != NULL) {
          desti = ch->pcdata->driveloc[4];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[5]) && ch->pcdata->driveloc[5] > 0 && get_room_index(ch->pcdata->driveloc[5]) != NULL) {
          desti = ch->pcdata->driveloc[5];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[6]) && ch->pcdata->driveloc[6] > 0 && get_room_index(ch->pcdata->driveloc[6]) != NULL) {
          desti = ch->pcdata->driveloc[6];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[7]) && ch->pcdata->driveloc[7] > 0 && get_room_index(ch->pcdata->driveloc[7]) != NULL) {
          desti = ch->pcdata->driveloc[7];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[8]) && ch->pcdata->driveloc[8] > 0 && get_room_index(ch->pcdata->driveloc[8]) != NULL) {
          desti = ch->pcdata->driveloc[8];
        }
        if (!str_cmp(arg1, ch->pcdata->drivenames[9]) && ch->pcdata->driveloc[9] > 0 && get_room_index(ch->pcdata->driveloc[9]) != NULL) {
          desti = ch->pcdata->driveloc[9];
        }
      }
      if (desti == 0)
      desti = atoi(arg1);

      if (desti < 0 || desti > 100000) {
        send_to_char("`WSwalk `g(`Wnumber`g) (`Wpassengers`g)`x\n\r", ch);
        return;
      }

      if (desti <= MAX_TAXIS) {
        room = get_room_index(taxi_table[desti - 1].vnum);
      }
      else {
        room = get_poroom(desti);
      }
    }
    if (room == NULL) {
      send_to_char("`WSwalk `g(`Wnumber`g) (`Wpassengers`g)`x\n\r", ch);
      return;
    }
    ch->pcdata->sr_nomove = 0;
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

      if (!is_name(PERS(vch, ch), argument))
      continue;

      if (!IS_FLAG(ch->comm, COMM_RUNNING))
      continue;

      char_from_room(vch);
      char_to_room(vch, room);
    }
    char_from_room(ch);
    char_to_room(ch, room);
    printf_to_char(ch, "`cYou travel to %s`g.`x\n\r", room->name);
    if(!higher_power(ch) || strlen(ch->pcdata->eidilon_ambient) < 2)
    act("$n has arrived.", ch, NULL, NULL, TO_ROOM);
    else
    act(ch->pcdata->eidilon_ambient, ch, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_cure) {

    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_world(ch, arg1);

    if (victim == NULL || IS_NPC(victim)) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg2);

    if (get_gmtrust(ch, victim) < 1 && str_cmp(arg2, "hunters") && str_cmp(arg2, "police")) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "weaken")) {
      scon_from_char(victim, SCON_WEAKEN, get_gmtrust(ch, victim));
      send_to_char("You remove their weakness.\n\r", ch);
      send_to_char("You feel strong again.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "noheal")) {
      scon_from_char(victim, SCON_NOHEAL, get_gmtrust(ch, victim));
      send_to_char("You restore their recovery.\n\r", ch);
      send_to_char("You feel like you're recovering faster again.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "nomove")) {
      scon_from_char(victim, SCON_NOMOVE, get_gmtrust(ch, victim));
      send_to_char("You restore their ability to move around.\n\r", ch);
      send_to_char("You feel like you can move around again.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "localmove")) {
      scon_from_char(victim, SCON_LOCALMOVE, get_gmtrust(ch, victim));
      send_to_char("You restore their ability to move around.\n\r", ch);
      send_to_char("You feel like you can move around again.\n\r", victim);
      return;
    }
    else if (!str_cmp(arg2, "police")) {
      if (!senior_gm(ch))
      return;

      if (safe_strlen(argument) < 10) {
        send_to_char("You need to add a reason to the end of that.\n\r", ch);
        return;
      }
      /*
if(ch->pcdata->cop_cooldown > 0)
{
send_to_char("You did that too recently\n\r", ch);
return;
}
*/
      scon_from_char(victim, SCON_POLICE, 1);
      send_to_char("You recall the police after them.\n\r", ch);
      sprintf(buf, "%s recalls the police after %s for %s", NAME(ch), NAME(victim), argument);
      sralert(buf);
      srnews(buf);
      if (!IS_IMMORTAL(ch))
      ch->pcdata->cop_cooldown = 10000;
    }
    else if (!str_cmp(arg2, "hunters")) {
      if (!senior_gm(ch))
      return;

      if (safe_strlen(argument) < 10) {
        send_to_char("You need to add a reason to the end of that.\n\r", ch);
        return;
      }
      /*
if(ch->pcdata->cop_cooldown > 0)
{
send_to_char("You did that too recently\n\r", ch);
return;
}
*/
      scon_from_char(victim, SCON_HUNTERS, 1);
      send_to_char("You recall the hunters after them.\n\r", ch);
      sprintf(buf, "%s recalls the hunters after %s for %s", NAME(ch), NAME(victim), argument);
      sralert(buf);
      srnews(buf);
      if (!IS_IMMORTAL(ch))
      ch->pcdata->cop_cooldown = 10000;
    }
    else
    send_to_char("Available conditions are: weaken, noheal.\n\r", ch);
  }

  bool has_con(CHAR_DATA *ch, int condition) {
    if (IS_NPC(ch))
    return FALSE;
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->storycon[i][0] == condition && ch->pcdata->storycon[i][2] > 0)
      return TRUE;
    }
    return FALSE;
  }

  _DOFUN(do_award) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char buf[MSL];
    if (!is_gm(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }
    if (ch->pcdata->account->awards < 1 && !IS_IMMORTAL(ch)) {
      send_to_char("You don't have any awards to give out.\n\r", ch);
      return;
    }

    if (safe_strlen(argument) < 10) {
      send_to_char("Syntax: award (person) (reason)\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_world(ch, arg1);

    if (victim == NULL || IS_NPC(victim) || get_gmtrust(ch, victim) < 0) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }

    if (victim->pcdata->account == NULL) {
      send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
      return;
    }
    if (same_player(ch, victim)) {
      send_to_char("You can't award yourself.\n\r", ch);
      return;
    }
    if (!str_cmp(ch->desc->host, victim->desc->host)) {
      send_to_char("You can't award someone with the same IP as you.\n\r", ch);
      return;
    }

    if (account_allies(ch, victim))
    victim->pcdata->account->award_karma += 200;
    else
    victim->pcdata->account->award_karma += 2000;

    act("You give $N an award for their RP.", ch, NULL, victim, TO_CHAR);
    printf_to_char(victim, "You are given an award for %s\n\r", argument);
    if (!IS_IMMORTAL(ch)) {
      ch->pcdata->account->awards--;

      sprintf(buf, "%s awards %s for %s.", NAME(ch), NAME(victim), argument);
      sralert(buf);
    }
  }

  _DOFUN(do_break) {
    ROOM_INDEX_DATA *in_room;
    int door;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit_rev;

    if (!is_gm(ch)) {
      send_to_char("This is a StoryRunner command.\n\r", ch);
      return;
    }

    if (!str_prefix(argument, "north")) {
      door = DIR_NORTH;
    }
    else if (!str_prefix(argument, "south"))
    door = DIR_SOUTH;
    else if (!str_prefix(argument, "east"))
    door = DIR_EAST;
    else if (!str_prefix(argument, "west"))
    door = DIR_WEST;
    else if (!str_prefix(argument, "up"))
    door = DIR_UP;
    else if (!str_prefix(argument, "down"))
    door = DIR_DOWN;
    else if (!str_prefix(argument, "northwest") || !str_cmp(argument, "nw"))
    door = DIR_NORTHWEST;
    else if (!str_prefix(argument, "northeast") || !str_cmp(argument, "ne"))
    door = DIR_NORTHEAST;
    else if (!str_prefix(argument, "southwest") || !str_cmp(argument, "sw"))
    door = DIR_SOUTHWEST;
    else if (!str_prefix(argument, "southeast") || !str_cmp(argument, "se"))
    door = DIR_SOUTHEAST;
    else
    door = -1;
    in_room = ch->in_room;

    if (door == -1 || (pexit = in_room->exit[door]) == NULL || pexit->wall == WALL_NONE) {
      send_to_char("You can't break that way\n\r", ch);
      return;
    }
    pexit->wallcondition = WALLCOND_HOLE;

    if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
      pexit_rev->wallcondition = WALLCOND_HOLE;
    }

    send_to_char("Done.\n\r", ch);
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  }
  _DOFUN(do_repair) {
    ch->pcdata->process_target = NULL;
    ROOM_INDEX_DATA *in_room;
    int door;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit_rev;

    if (!is_gm(ch)) {
      /*
if(!in_prop(ch))
{
send_to_char("You have to be in a property first.\n\r", ch);
return;
}
*/
      ch->pcdata->process = PROCESS_REPAIR;
      ch->pcdata->process_timer = 30 - (labor_points(ch) * 5);
      ch->pcdata->process_timer = UMAX(5, ch->pcdata->process_timer);
      if (ch->in_room->sector_type == SECT_STREET)
      ch->pcdata->process_timer = UMIN(ch->pcdata->process_timer, 15);
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You start to repair the structure.\n\r", ch, NULL, NULL, TO_CHAR);
      act("$n starts to repair the structure.\n\r", ch, NULL, NULL, TO_ROOM);
      return;
    }

    if (!str_prefix(argument, "north")) {
      door = DIR_NORTH;
    }
    else if (!str_prefix(argument, "south"))
    door = DIR_SOUTH;
    else if (!str_prefix(argument, "east"))
    door = DIR_EAST;
    else if (!str_prefix(argument, "west"))
    door = DIR_WEST;
    else if (!str_prefix(argument, "up"))
    door = DIR_UP;
    else if (!str_prefix(argument, "down"))
    door = DIR_DOWN;
    else if (!str_prefix(argument, "northwest") || !str_cmp(argument, "nw"))
    door = DIR_NORTHWEST;
    else if (!str_prefix(argument, "northeast") || !str_cmp(argument, "ne"))
    door = DIR_NORTHEAST;
    else if (!str_prefix(argument, "southwest") || !str_cmp(argument, "sw"))
    door = DIR_SOUTHWEST;
    else if (!str_prefix(argument, "southeast") || !str_cmp(argument, "se"))
    door = DIR_SOUTHEAST;
    else
    door = -1;
    in_room = ch->in_room;

    if (door == -1 || (((pexit = in_room->exit[door]) == NULL) || (pexit->wall == WALL_NONE))) {
      send_to_char("You can't break that way\n\r", ch);
      return;
    }
    pexit->wallcondition = WALLCOND_NORMAL;
    if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
      pexit_rev->wallcondition = WALLCOND_NORMAL;
    }

    send_to_char("Done.\n\r", ch);
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  }

  _DOFUN(do_crisis) {
    int value, i;
    char arg1[MSL];
    char arg2[MSL];
    MOB_INDEX_DATA *pMobIndex;

    argument = one_argument_nouncap(argument, arg1);

    if (!is_gm(ch)) {
      send_to_char("This is a story runner command.\n\r", ch);
      return;
    }
    if (!has_crisis(ch) && !crisis_runner(ch)) {
      send_to_char("You aren't running a crisis enabled plot.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "brainwash")) {
      argument = one_argument_nouncap(argument, arg2);

      if (!str_cmp(arg2, "on")) {
        crisis_brainwash = 1;
        free_string(crisis_brainwash_message);
        crisis_brainwash_message = str_dup(argument);
        send_to_char("You brainwash the town.\n\r", ch);
      }
      else if (!str_cmp(arg2, "off")) {
        crisis_brainwash = 0;
        send_to_char("You end the brainwashing.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "supernaturaldarkness")) {
      if (!str_cmp(argument, "on")) {
        supernatural_darkness = 1;
        send_to_char("You create supernatural darkness.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        supernatural_darkness = 0;
        send_to_char("You end the supernatural darkness.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "nonightmare")) {
      if (!str_cmp(argument, "on")) {
        crisis_noshroud = 1;
        send_to_char("You stop people entering the nightmare.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_noshroud = 0;
        send_to_char("You let people enter the nightmare again.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "flood")) {
      if (!str_cmp(argument, "on")) {
        crisis_flood = 1;
        send_to_char("You flood the town.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_flood = 0;
        send_to_char("You end the flood.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "nodrive")) {
      if (!str_cmp(argument, "on")) {
        crisis_nodrive = 1;
        send_to_char("You stop vehicles from working.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_nodrive = 0;
        send_to_char("You let vehicles work again.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "notravel")) {
      if (!str_cmp(argument, "on")) {
        crisis_notravel = 1;
        send_to_char("You stop people being able to leave town.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_notravel = 0;
        send_to_char("People can leave town again.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "uninvite")) {
      if (!str_cmp(argument, "on")) {
        crisis_uninvited = 1;
        send_to_char("You stop people being able to use their houses.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_uninvited = 0;
        send_to_char("You let people use their houses again.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg1, "mist")) {
      if (!str_cmp(argument, "on")) {
        crisis_mist = 1;
        send_to_char("You summon a mist.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_mist = 0;
        send_to_char("You return the mist to normal levels.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg1, "darkness")) {
      if (!str_cmp(argument, "on")) {
        crisis_darkness = 1;
        send_to_char("You block out natural light.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_darkness = 0;
        send_to_char("You end blocking of natural light.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "blackout")) {
      if (!str_cmp(argument, "on")) {
        crisis_blackout = 1;
        send_to_char("You create a blackout.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_blackout = 0;
        send_to_char("You end the blackout.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "light")) {
      if (!str_cmp(argument, "on")) {
        crisis_light = 1;
        send_to_char("You bring unnatural light.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_light = 0;
        send_to_char("You end the unnatural light.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "hurricane")) {
      if (!str_cmp(argument, "on")) {
        crisis_hurricane = 1;
        send_to_char("You create a hurricane.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_hurricane = 0;
        send_to_char("You end the hurricane.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "storm")) {
      if (!str_cmp(argument, "on")) {
        crisis_storm = 1;
        send_to_char("You create a storm.\n\r", ch);
      }
      else if (!str_cmp(argument, "off")) {
        crisis_storm = 0;
        send_to_char("You end the storm.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "heatup")) {
      time_info.local_temp += atoi(argument);
      send_to_char("You heat up the world.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "cooldown")) {
      time_info.local_temp -= atoi(argument);
      send_to_char("You cool down the world.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "atmosphere")) {
      string_append(ch, &crisis_atmosphere);
      return;
    }
    if (!str_cmp(arg1, "constantdamage")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "on")) {
        free_string(global_constant_message);
        global_constant_message = str_dup(argument);
        global_damage_constant = 1;

        send_to_char("You start damaging the world constantly.\n\r", ch);
      }
      else if (!str_cmp(arg2, "off")) {
        global_damage_constant = 0;
        send_to_char("You cease raining down destruction on the world.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "erraticdamage")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "on")) {
        free_string(global_erratic_message);
        global_erratic_message = str_dup(argument);
        global_damage_erratic = 1;

        send_to_char("You start damaging the world erratically.\n\r", ch);
      }
      else if (!str_cmp(arg2, "off")) {
        global_damage_erratic = 0;
        send_to_char("You cease raining down destruction on the world.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "invade")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "intelligent")) {
        value = atoi(argument);
        if (value == 1)
        invade_intelligent_one = 1;
        if (value == 2)
        invade_intelligent_two = 1;
        if (value == 3)
        invade_intelligent_three = 1;

        send_to_char("You make your invaders intelligent.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "unintelligent")) {
        value = atoi(argument);
        if (value == 1)
        invade_intelligent_one = 0;
        if (value == 2)
        invade_intelligent_two = 0;
        if (value == 3)
        invade_intelligent_three = 0;

        send_to_char("You make your invaders unintelligent.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "nightmare")) {
        value = atoi(argument);
        if (value == 1)
        invade_shroud_one = 1;
        if (value == 2)
        invade_shroud_two = 1;
        if (value == 3)
        invade_shroud_three = 1;

        send_to_char("You make your invaders part of the nightmare.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "unnightmare")) {
        value = atoi(argument);
        if (value == 1)
        invade_shroud_one = 0;
        if (value == 2)
        invade_shroud_two = 0;
        if (value == 3)
        invade_shroud_three = 0;

        send_to_char("You make your invaders not part of the nightmare.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "flying")) {
        value = atoi(argument);
        if (value == 1)
        invade_aerial_one = 1;
        if (value == 2)
        invade_aerial_two = 1;
        if (value == 3)
        invade_aerial_three = 1;

        send_to_char("You make your invaders fly.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "walking")) {
        value = atoi(argument);
        if (value == 1)
        invade_aerial_one = 0;
        if (value == 2)
        invade_aerial_two = 0;
        if (value == 3)
        invade_aerial_three = 0;

        send_to_char("You make your invaders walk.\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "over")) {
        value = atoi(argument);
        if (value == 1)
        invasion_one = 0;
        if (value == 2)
        invasion_two = 0;
        if (value == 3)
        invasion_three = 0;

        send_to_char("You end the invasion.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || !is_number(argument))
      return;

      value = atoi(arg2);
      i = atoi(argument);

      if (value > 3)
      return;
      if (i > 5)
      return;
      pMobIndex = get_mob_index(150 + value);
      free_string(pMobIndex->short_descr);
      pMobIndex->short_descr = str_dup(ch->pcdata->monster_names[1][i]);
      free_string(pMobIndex->player_name);
      pMobIndex->player_name = str_dup(ch->pcdata->monster_names[0][i]);
      free_string(pMobIndex->long_descr);
      pMobIndex->long_descr = str_dup(ch->pcdata->monster_names[1][i]);
      free_string(pMobIndex->description);
      pMobIndex->description = str_dup(ch->pcdata->monster_names[2][i]);

      for (int j = 0; j < MAX_DIS; j++)
      pMobIndex->disciplines[j] = 0;

      pMobIndex->disciplines[ch->pcdata->monster_discs[0][i]] =
      ch->pcdata->monster_discs[1][i];
      pMobIndex->disciplines[ch->pcdata->monster_discs[2][i]] =
      ch->pcdata->monster_discs[3][i];
      pMobIndex->disciplines[ch->pcdata->monster_discs[4][i]] =
      ch->pcdata->monster_discs[5][i];

      if (ch->in_room->vnum >= 1000) {
        if (value == 1)
        invade_spawn_one = ch->in_room->vnum;
        else if (value == 2)
        invade_spawn_two = ch->in_room->vnum;
        else if (value == 3)
        invade_spawn_three = ch->in_room->vnum;
      }
      else {
        if (value == 1)
        invade_spawn_one = 0;
        else if (value == 2)
        invade_spawn_two = 0;
        else if (value == 3)
        invade_spawn_three = 0;
      }
      if (value == 1)
      invasion_one = 1;
      else if (value == 2)
      invasion_two = 1;
      else if (value == 3)
      invasion_three = 1;

      send_to_char("You start an invasion.\n\r", ch);

      return;
    }
  }

  const int spawn_points[] = {15218, 15249, 15282, 18505, 1061,  14819, 8166, 7572,  7420,  6278,  1362,  10527, 4890,  4914};

  const int spawn_directions[] = {DIR_EAST,  DIR_EAST,      DIR_EAST,  DIR_EAST, DIR_SOUTH, DIR_SOUTH,     DIR_SOUTH, DIR_WEST, DIR_WEST,  DIR_NORTHWEST, DIR_WEST,  DIR_WEST, DIR_WEST,  DIR_WEST};

  void spawn_invader(int num) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int random = 0;
    int vnum = 150 + num;
    ROOM_INDEX_DATA *room;

    if (num == 1) {
      if (get_mob_index(vnum) == NULL)
      return;

      pMobIndex = get_mob_index(vnum);
      mob = create_mobile(pMobIndex);

      if (invade_spawn_one == 0) {
        random = number_range(0, 5);
        room = get_room_index(spawn_points[random]);
      }
      else
      room = get_room_index(invade_spawn_one);

      if (room == NULL)
      return;

      char_to_room(mob, room);
      mob->hit = max_hp(mob);
      mob->x = number_range(0, room->size);
      mob->y = number_range(0, room->size);

      if (invade_shroud_one == 1)
      SET_FLAG(mob->act, PLR_SHROUD);
      if (invade_spawn_one == 0)
      mob->facing = spawn_directions[random];
      else
      mob->facing = number_range(0, 9);

    }
    else if (num == 2) {
      if (get_mob_index(vnum) == NULL)
      return;

      pMobIndex = get_mob_index(vnum);
      mob = create_mobile(pMobIndex);

      if (invade_spawn_two == 0) {
        random = number_range(0, 5);
        room = get_room_index(spawn_points[random]);
      }
      else
      room = get_room_index(invade_spawn_two);

      if (room == NULL)
      return;

      char_to_room(mob, room);
      mob->hit = max_hp(mob);
      mob->x = number_range(0, room->size);
      mob->y = number_range(0, room->size);

      if (invade_shroud_two == 1)
      SET_FLAG(mob->act, PLR_SHROUD);
      if (invade_spawn_two == 0)
      mob->facing = spawn_directions[random];
      else
      mob->facing = number_range(0, 9);

    }
    else if (num == 3) {
      if (get_mob_index(vnum) == NULL)
      return;

      pMobIndex = get_mob_index(vnum);
      mob = create_mobile(pMobIndex);

      if (invade_spawn_three == 0) {
        random = number_range(0, 5);
        room = get_room_index(spawn_points[random]);
      }
      else
      room = get_room_index(invade_spawn_three);

      if (room == NULL)
      return;

      char_to_room(mob, room);
      mob->hit = max_hp(mob);
      mob->x = number_range(0, room->size);
      mob->y = number_range(0, room->size);

      if (invade_shroud_three == 1)
      SET_FLAG(mob->act, PLR_SHROUD);
      if (invade_spawn_three == 0)
      mob->facing = spawn_directions[random];
      else
      mob->facing = number_range(0, 9);
    }
  }

  void invade_check(void) {
    if (invasion_one == 1) {
      spawn_invader(1);
      spawn_invader(1);
      spawn_invader(1);
    }
    if (invasion_two == 1) {
      spawn_invader(2);
      spawn_invader(2);
      spawn_invader(2);
    }
    if (invasion_three == 1) {
      spawn_invader(3);
      spawn_invader(3);
      spawn_invader(3);
    }
  }

  bool will_knock(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum == 151 && invade_intelligent_one == 1)
    return TRUE;
    if (ch->pIndexData->vnum == 152 && invade_intelligent_two == 1)
    return TRUE;
    if (ch->pIndexData->vnum == 153 && invade_intelligent_three == 1)
    return TRUE;
    return FALSE;
  }

  void knock_check(CHAR_DATA *ch, int door) {
    if (!will_knock(ch))
    return;

    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;

    pexit = ch->in_room->exit[door];
    if (pexit == NULL)
    return;

    if ((to_room = pexit->u1.to_room) == NULL) {
      return;
    }

    for (CharList::iterator it = to_room->people->begin();
    it != to_room->people->end(); ++it)
    printf_to_char(*it, "You hear a knocking coming from the door to the %s\n\r", dir_name[rev_dir[door]][0]);
  }

  _DOFUN(do_fight) {
    CHAR_DATA *victim;
    char arg1[MSL], arg2[MSL], arg3[MSL], arg4[MSL];

    if (!is_gm(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "fast")) {
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

        if (is_gm(victim))
        continue;

        if (victim->in_room == ch->in_room)
        victim->fight_fast = TRUE;
      }

      send_to_char("Fight made fast.\n\r", ch);
      return;
    }
    if (!str_cmp(argument, "slow")) {
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

        if (is_gm(victim))
        continue;

        if (victim->in_room == ch->in_room)
        victim->fight_fast = FALSE;
      }
      send_to_char("Fight made slow.\n\r", ch);
      return;
    }
    if (!str_cmp(argument, "power")) {
      int offense = 0;
      int defense = 0;
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        victim = *it;

        if (victim == NULL)
        continue;

        if (IS_NPC(victim))
        continue;

        if (is_gm(victim))
        continue;

        if (victim->race == RACE_CIVILIAN)
        continue;

        if (victim->in_room == NULL)
        continue;

        if (victim->in_room != ch->in_room)
        continue;

        offense += highestoffense(victim);
        defense += shield_total(victim);
      }
      printf_to_char(ch, "Total Offense: %d, Total Defense: %d\n\r", offense, defense);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "rename")) {
      argument = one_argument_nouncap(argument, arg2);

      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!IS_NPC(victim))
      return;

      free_string(victim->short_descr);
      victim->short_descr = str_dup(argument);
      free_string(victim->name);
      victim->name = str_dup(argument);
      free_string(victim->long_descr);
      victim->long_descr = str_dup(argument);
      free_string(victim->description);

      send_to_char("Done.\n\r", ch);
      return;
    }
    /*
if(!str_cmp(arg1, "speed"))
{
ch->in_room->fight_speed = atoi(argument);
send_to_char("Done.\n\r", ch);
return;
}
*/
    if (!str_cmp(arg1, "tree")) {
      MOB_INDEX_DATA *pMobIndex;
      CHAR_DATA *mob;

      pMobIndex = get_mob_index(110);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 50;
      mob->controled_by = ch;

      mob->hit = max_hp(mob);

      if (IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);

      mob->x = get_summon_x(ch->in_room, argument);
      mob->y = get_summon_y(ch->in_room, argument);
      if (mob->x == 0 && mob->y == 0) {
        mob->x = ch->x;
        mob->y = ch->y;
      }
      send_to_char("Cover summoned.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "rock")) {
      MOB_INDEX_DATA *pMobIndex;
      CHAR_DATA *mob;

      pMobIndex = get_mob_index(111);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 50;
      mob->controled_by = ch;

      mob->hit = max_hp(mob);

      if (IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);

      mob->x = get_summon_x(ch->in_room, argument);
      mob->y = get_summon_y(ch->in_room, argument);
      if (mob->x == 0 && mob->y == 0) {
        mob->x = ch->x;
        mob->y = ch->y;
      }
      send_to_char("Cover summoned.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "brush")) {
      MOB_INDEX_DATA *pMobIndex;
      CHAR_DATA *mob;

      pMobIndex = get_mob_index(112);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 50;
      mob->controled_by = ch;

      mob->hit = max_hp(mob);

      if (IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);

      mob->x = get_summon_x(ch->in_room, argument);
      mob->y = get_summon_y(ch->in_room, argument);
      if (mob->x == 0 && mob->y == 0) {
        mob->x = ch->x;
        mob->y = ch->y;
      }
      send_to_char("Cover summoned.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);
    argument = one_argument_nouncap(argument, arg4);

    if (!str_cmp(arg1, "aggro")) {
      CHAR_DATA *mob;
      if ((mob = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if ((victim = get_char_room(ch, NULL, arg3)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!is_number(arg4)) {
        send_to_char("Fight aggro (mob) (person) amount.\n\r", ch);
        return;
      }
      int amount = atoi(arg4);
      if (victim->target == mob) {
        victim->target_dam += amount;
      }
      else if (victim->target_2 == mob) {
        victim->target_dam_2 += amount;
      }
      else if (victim->target_3 == mob) {
        victim->target_dam_3 += amount;
      }
      else {
        victim->target_3 = victim->target_2;
        victim->target_dam_3 = victim->target_dam_2;
        victim->target_2 = victim->target;
        victim->target_dam_2 = victim->target_dam;
        victim->target = mob;
        victim->target_dam = amount;
      }
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "move")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      victim->x = atoi(arg3);
      victim->y = atoi(arg4);

      if (victim->x > ch->in_room->size)
      victim->x = ch->in_room->size;
      if (victim->x < 0)
      victim->x = 0;

      if (victim->y > ch->in_room->size)
      victim->y = ch->in_room->size;
      if (victim->y < 0)
      victim->y = 0;

      send_to_char("Moved.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "stun")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      victim->attack_timer += FIGHT_WAIT * fight_speed(victim);
      victim->move_timer += FIGHT_WAIT * fight_speed(victim);

      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "root")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      victim->move_timer += FIGHT_WAIT * fight_speed(victim);

      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "weaken")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!IS_FLAG(victim->fightflag, FIGHT_OVERPOWER))
      SET_FLAG(ch->fightflag, FIGHT_OVERPOWER);

      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "strengthen")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (!IS_FLAG(victim->fightflag, FIGHT_UNDERPOWER))
      SET_FLAG(ch->fightflag, FIGHT_UNDERPOWER);

      send_to_char("Done.\n\r", ch);
      return;
    }
  }

  char *get_size_string(int num) {
    if (num == 2)
    return "large";
    if (num == 1)
    return "medium";

    return "small";
  }

  void plot_reward(int type, char *argument, bool pvp) {
    CHAR_DATA *ch;

    int count = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;

      if ((d->connected != CON_PLAYING && d->connected != CON_CREATION))
      continue;

      ch = CH(d);

      if (is_name(ch->name, argument))
      count++;
    }
    int basevalue = 10;
    if (type == REWARD_MONEY) {
      basevalue = 80000;
    }
    if (type == REWARD_INFLUENCE)
    basevalue = 2000;
    if (type == REWARD_RESPECT)
    basevalue = 2000;
    if (type == REWARD_MAGICAL)
    basevalue = 12 * 60 * 12;

    if (pvp == TRUE)
    basevalue *= 3;

    for (int i = 1; i < count; i++) {
      basevalue = basevalue * 9 / 10;
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;

      if ((d->connected != CON_PLAYING && d->connected != CON_CREATION))
      continue;

      ch = CH(d);

      if (is_name(ch->name, argument)) {
        ch->pcdata->adventure_fatigue += 200;
        if (type == REWARD_MONEY) {
          ch->money += basevalue;
        }
        if (type == REWARD_INFLUENCE) {
          ch->pcdata->influence += basevalue;
        }
        if (type == REWARD_RESPECT) {
          give_respect(ch, basevalue, "Adventure.", ch->faction);
          give_respect_noecho(ch, basevalue, ch->factiontwo);
        }
        if (type == REWARD_MAGICAL) {
          AFFECT_DATA af;
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = basevalue;
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_BONUS;
          affect_to_char(ch, &af);
        }
      }
    }
  }

  bool visible_plot(CHAR_DATA *ch, PLOT_TYPE *plot) {

    if (!str_cmp(plot->author, ch->name))
    return TRUE;

    if (plot->visibility == VISIBILITY_ALL) {
      if (plot->type == PLOT_QUEST || plot->type == PLOT_PERSONAL) {
        if (is_name(ch->name, plot->members) || safe_strlen(plot->members) < 2 || is_name(ch->name, plot->character) || IS_IMMORTAL(ch))
        return TRUE;
      }
      else
      return TRUE;
    }

    if (plot->visibility == VISIBILITY_SRS && is_gm(ch))
    return TRUE;

    if (IS_IMMORTAL(ch))
    return TRUE;

    return FALSE;
  }

  char *plot_type(int type) {
    if (type == PLOT_ADVENTURE)
    return "Adventure";
    if (type == PLOT_JOINT)
    return "Joint";
    if (type == PLOT_PVP)
    return "Competetive";
    if (type == PLOT_CHARACTER)
    return "Character";
    if (type == PLOT_QUEST)
    return "Quest";
    if (type == PLOT_PERSONAL)
    return "Personal";
    if (type == PLOT_MYSTERY)
    return "Mystery";
    if (type == PLOT_CRISIS)
    return "Crisis";

    return "Other";
  }

  char *plot_visibility(int vis) {
    if (vis == VISIBILITY_ALL)
    return "All";
    if (vis == VISIBILITY_SRS)
    return "StoryRunners";

    return "None";
  }

  _DOFUN(do_plot) {

    char arg1[MSL];
    char arg2[MSL];
    PLOT_TYPE *plot;
    int i = 0;

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (ch->pcdata->aexp > 1000000 || ch->pcdata->aexp < 0)
    ch->pcdata->aexp = 0;

    if (!str_cmp(arg1, "list")) {
      i = 1;
      send_to_char("`WAdventures:`x\n\r", ch);
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        printf_to_char(ch, "`W%d`c)`x %s\n\r", i, (*it)->title);
        i++;
      }
      send_to_char("`WPersonal:`x\n\r", ch);
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        printf_to_char(ch, "`W%d`c)`x %s\n\r", i, (*it)->title);
        i++;
      }
      send_to_char("`WMysteries:`x\n\r", ch);
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        printf_to_char(ch, "`W%d`c)`x %s\n\r", i, (*it)->title);
        i++;
      }
      send_to_char("\n`WOther:`x\n\r", ch);
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        printf_to_char(ch, "`W%d`c)`x %s\n\r", i, (*it)->title);
        i++;
      }

    }
    else if (!str_cmp(arg1, "info")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          printf_to_char(
          ch, "`WTitle:`x %s\n`WStoryRunner:`x %s\n`WType:`x %s\n`WVisibility:`x %s\n`WTimes:`x %s\n`WCentral Character`x: %s\n`WEvents to Date:`x%s\n`WCurrent Challenge:`x %s\n\r", (*it)->title, (*it)->author, plot_type((*it)->type), plot_visibility((*it)->visibility), (*it)->timezone, (*it)->character, (*it)->events, (*it)->challenge);
          printf_to_char(ch, "`WTerritory:`x %s\n\r", (*it)->territory);
          printf_to_char(ch, "`WMastermind:`x %s\n\r", (*it)->mastermind);
          for (int x = 0; x < 10; x++) {
            if (safe_strlen((*it)->galleries[x]) > 1)
            printf_to_char(ch, "`WGallery NPC:`x %s\n\r", (*it)->galleries[x]);
          }

          if ((*it)->finale == 1)
          printf_to_char(ch, "`WThis plot is the finale of the storyline %s.\n\r", (*it)->storyline);
          if (is_gm(ch))
          printf_to_char(ch, "`WStoryRunner Info:`x %s\n\r", (*it)->storyrunners);
          if (IS_IMMORTAL(ch))
          printf_to_char(ch, "`WImmortal Info:`x %s\n\r", (*it)->immortals);

          if (!str_cmp(ch->name, (*it)->author) && (*it)->type != PLOT_OTHER) {
            send_to_char("`WApplicants:`x\n\r", ch);
            for (int j = 0; j < 30; j++) {
              if ((*it)->rating[j] != 0)
              printf_to_char(ch, "%s [%d] : %s\n\r", (*it)->participants[j], (*it)->rating[j], (*it)->comments[j]);
            }
          }
          else if ((*it)->type != PLOT_OTHER) {
            for (int j = 0; j < 30; j++) {
              if ((*it)->rating[j] != 0 && !str_cmp(ch->name, (*it)->participants[j]))
              printf_to_char(ch, "`WApplied`x : %s\n\r", (*it)->comments[j]);
            }
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          printf_to_char(
          ch, "`WTitle:`x %s\n`WStoryRunner:`x %s\n`WType:`x %s\n`WVisibility:`x %s\n`WTimes:`x %s\n`WCentral Character`x: %s\n`WEvents to Date:`x%s\n`WCurrentChallenge:`x %s\n\r", (*it)->title, (*it)->author, plot_type((*it)->type), plot_visibility((*it)->visibility), (*it)->timezone, (*it)->character, (*it)->events, (*it)->challenge);
          printf_to_char(ch, "`WTerritory:`x %s\n\r", (*it)->territory);
          printf_to_char(ch, "`WMastermind:`x %s\n\r", (*it)->mastermind);
          for (int x = 0; x < 10; x++) {
            if (safe_strlen((*it)->galleries[x]) > 1)
            printf_to_char(ch, "`WGallery NPC:`x %s\n\r", (*it)->galleries[x]);
          }

          if (is_gm(ch))
          printf_to_char(ch, "`WStoryRunner Info:`x %s\n\r", (*it)->storyrunners);
          if (IS_IMMORTAL(ch))
          printf_to_char(ch, "`WImmortal Info:`x %s\n\r", (*it)->immortals);

          if (!str_cmp(ch->name, (*it)->author) && (*it)->type != PLOT_OTHER) {
            send_to_char("`WApplicants:`x\n\r", ch);
            for (int j = 0; j < 30; j++) {
              if ((*it)->rating[j] != 0)
              printf_to_char(ch, "%s [%d] : %s\n\r", (*it)->participants[j], (*it)->rating[j], (*it)->comments[j]);
            }
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          printf_to_char(
          ch, "`WTitle:`x %s\n`WStoryRunner:`x %s\n`WType:`x %s\n`WVisibility:`x %s\n`WTimes:`x %s\n`WCentral Character`x: %s\n`WEvents to Date:`x%s\n`WCurrent Challenge:`x %s\n\r", (*it)->title, (*it)->author, plot_type((*it)->type), plot_visibility((*it)->visibility), (*it)->timezone, (*it)->character, (*it)->events, (*it)->challenge);
          printf_to_char(ch, "`WTerritory:`x %s\n\r", (*it)->territory);
          printf_to_char(ch, "`WMastermind:`x %s\n\r", (*it)->mastermind);
          for (int x = 0; x < 10; x++) {
            if (safe_strlen((*it)->galleries[x]) > 1)
            printf_to_char(ch, "`WGallery NPC:`x %s\n\r", (*it)->galleries[x]);
          }

          if (is_gm(ch))
          printf_to_char(ch, "`WStoryRunner Info:`x %s\n\r", (*it)->storyrunners);
          if (IS_IMMORTAL(ch))
          printf_to_char(ch, "`WImmortal Info:`x %s\n\r", (*it)->immortals);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          printf_to_char(
          ch, "`WTitle:`x %s\n`WStoryRunner:`x %s\n`WType:`x %s\n`WVisibility:`x %s\n`WTimes:`x %s\n`WCentral Character`x: %s\n`WEvents to Date:`x%s\n`WCurrent Challenge:`x %s\n\r", (*it)->title, (*it)->author, plot_type((*it)->type), plot_visibility((*it)->visibility), (*it)->timezone, (*it)->character, (*it)->events, (*it)->challenge);
          printf_to_char(ch, "`WTerritory:`x %s\n\r", (*it)->territory);
          printf_to_char(ch, "`WMastermind:`x %s\n\r", (*it)->mastermind);
          for (int x = 0; x < 10; x++) {
            if (safe_strlen((*it)->galleries[x]) > 1)
            printf_to_char(ch, "`WGallery NPC:`x %s\n\r", (*it)->galleries[x]);
          }

          if (is_gm(ch))
          printf_to_char(ch, "`WStoryRunner Info:`x %s\n\r", (*it)->storyrunners);
          if (IS_IMMORTAL(ch))
          printf_to_char(ch, "`WImmortal Info:`x %s\n\r", (*it)->immortals);

          if (!str_cmp(ch->name, (*it)->author) && (*it)->type != PLOT_OTHER) {
            send_to_char("`WApplicants:`x\n\r", ch);
            for (int j = 0; j < 30; j++) {
              if ((*it)->rating[j] != 0)
              printf_to_char(ch, "%s [%d] : %s\n\r", (*it)->participants[j], (*it)->rating[j], (*it)->comments[j]);
            }
          }
          else if ((*it)->type != PLOT_OTHER) {
            for (int j = 0; j < 30; j++) {
              if ((*it)->rating[j] != 0 && !str_cmp(ch->name, (*it)->participants[j]))
              printf_to_char(ch, "`WApplied`x : %s\n\r", (*it)->comments[j]);
            }
          }
        }
      }

    }
    else if (!str_cmp(arg1, "delete")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Calendar event deleted.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Calendar event deleted.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Calendar event deleted.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Calendar event deleted.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "crisis")) {
      if (!IS_IMMORTAL(ch))
      return;
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          if ((*it)->crisis == 0) {
            (*it)->crisis = 1;
            send_to_char("Crisis approved.\n\r", ch);
          }
          else {
            (*it)->crisis = 0;
            send_to_char("Crisis unapproved.\n\r", ch);
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          if ((*it)->crisis == 0) {
            (*it)->crisis = 1;
            send_to_char("Crisis approved.\n\r", ch);
          }
          else {
            (*it)->crisis = 0;
            send_to_char("Crisis unapproved.\n\r", ch);
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          if ((*it)->crisis == 0) {
            (*it)->crisis = 1;
            send_to_char("Crisis approved.\n\r", ch);
          }
          else {
            (*it)->crisis = 0;
            send_to_char("Crisis unapproved.\n\r", ch);
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to delete.\n\r", ch);
            return;
          }
          if ((*it)->crisis == 0) {
            (*it)->crisis = 1;
            send_to_char("Crisis approved.\n\r", ch);
          }
          else {
            (*it)->crisis = 0;
            send_to_char("Crisis unapproved.\n\r", ch);
          }
        }
      }

    }
    else if (!str_cmp(arg1, "pass")) {
      CHAR_DATA *victim;
      victim = get_char_world(ch, arg2);
      if (ch->pcdata->astatus <= 0) {
        send_to_char("You have to finish an adventure plot first.\n\r", ch);
        return;
      }
      if (victim == NULL || IS_NPC(victim)) {
        send_to_char("Target not found.\n\r", ch);
        return;
      }
      if (victim->pcdata->astatus != 0) {
        send_to_char("They've finished an adventure too recently for that.\n\r", ch);
        return;
      }
      victim->pcdata->week_tracker[TRACK_ADVENTURES]++;
      victim->pcdata->life_tracker[TRACK_ADVENTURES]++;

      victim->pcdata->awins++;
      victim->pcdata->astatus = 500;
      victim->pcdata->aexp += 2000;
      send_to_char("You succeed in your adventure.\n\r", victim);
      send_to_char("Done.\n\r", ch);
      int karma = 300 * UMIN(100, solidity(victim)) / 100;
      give_karma(ch, karma, KARMA_ADVENTURE);
      victim->pcdata->adventure_fatigue += 300;
      int mreward = 250;
      char buf[MSL];
      sprintf(buf, "%s completing an operation.", ch->name);
      gain_resources(mreward, ch->faction, ch, buf);
      ch->pcdata->week_tracker[TRACK_CONTRIBUTED] += mreward * 10;
      ch->pcdata->life_tracker[TRACK_CONTRIBUTED] += mreward * 10;

      return;
    }
    else if (!str_cmp(arg1, "fail")) {
      CHAR_DATA *victim;
      victim = get_char_world(ch, arg2);
      if (ch->pcdata->astatus <= 0) {
        send_to_char("You have to finish an adventure plot first.\n\r", ch);
        return;
      }
      if (victim == NULL || IS_NPC(victim)) {
        send_to_char("Target not found.\n\r", ch);
        return;
      }
      if (victim->pcdata->astatus != 0) {
        send_to_char("They've finished an adventure too recently for that.\n\r", ch);
        return;
      }
      victim->pcdata->week_tracker[TRACK_ADVENTURES]++;
      victim->pcdata->life_tracker[TRACK_ADVENTURES]++;
      victim->pcdata->alosses++;
      victim->pcdata->astatus = -500;
      victim->pcdata->aexp += 1000;
      send_to_char("You do not succeed in your adventure.\n\r", victim);
      int karma = 600 * UMIN(100, solidity(victim)) / 100;
      give_karma(ch, karma, KARMA_ADVENTURE);
      victim->pcdata->adventure_fatigue += 200;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "award")) {
      CHAR_DATA *victim;
      victim = get_char_world(ch, arg2);
      if (ch->pcdata->astatus <= 0 && !IS_IMMORTAL(ch)) {
        send_to_char("You have to finish an adventure plot first.\n\r", ch);
        return;
      }
      if (victim == NULL || IS_NPC(victim)) {
        send_to_char("Target not found.\n\r", ch);
        return;
      }
      char arg10[MSL];
      argument = one_argument_nouncap(argument, arg10);

      if (same_player(ch, victim)) {
        send_to_char("You can't award your own characters.\n\r", ch);
        return;
      }
      if (victim->pcdata->astatus <= 0 && !IS_IMMORTAL(ch)) {
        send_to_char("They have to pass an adventure first.\n\r", ch);
        return;
      }
      if (!str_cmp(arg10, "money")) {
        int amount = atoi(argument);
        if (amount <= 0 || amount > UMIN(ch->pcdata->account->sr_aexp / 10, victim->pcdata->aexp / 10)) {
          printf_to_char(
          ch, "Syntax: Plot award (person) money (1-%d)\n\r", UMIN(ch->pcdata->account->sr_aexp / 10, victim->pcdata->aexp / 10));
          return;
        }
        ch->pcdata->account->sr_aexp -= amount * 10;
        victim->pcdata->aexp -= amount * 10;
        victim->money += amount * 100;
        send_to_char("Done.\n\r", ch);
        printf_to_char(victim, "You are awarded $%d for your role in the adventure.\n\r", amount);
      }
      else if (!str_cmp(arg10, "influence")) {
        int amount = atoi(argument);
        if (amount <= 0 || amount > UMIN(ch->pcdata->account->sr_aexp, worlds_experience(victim, WORLD_EARTH))) {
          printf_to_char(
          ch, "Syntax: Plot award (person) influence (1-%d)\n\r", UMIN(ch->pcdata->account->sr_aexp, victim->pcdata->aexp));
          return;
        }
        victim->pcdata->influence += amount * 5;
        ch->pcdata->account->sr_aexp -= amount;
        charge_worlds_exp(victim, WORLD_EARTH, amount);
        send_to_char("Done.\n\r", ch);
        printf_to_char(
        victim, "You are awarded %d influence for your role in the adventure.\n\r", amount * 5);
      }
      else if (!str_cmp(arg10, "territory")) {
        char arg3[MSL];
        argument = one_argument_nouncap(argument, arg3);
        int amount = atoi(argument) * 1000;
        if (amount <= 0 || amount > UMIN(ch->pcdata->account->sr_aexp, victim->pcdata->aexp * 4)) {
          printf_to_char(
          ch, "Syntax: Plot award (person) territory (territory) (1-%d)\n\r", UMIN(ch->pcdata->account->sr_aexp, victim->pcdata->aexp * 4) /
          1000);
          return;
        }
        if (victim->faction == 0) {
          send_to_char("They're not in a faction.\n\r", ch);
          return;
        }
        LOCATION_TYPE *loc;
        loc = get_loc(arg3);
        if (loc == NULL) {
          send_to_char("No such territory\n\r", ch);
          return;
        }
        if (clan_lookup(victim->faction) != NULL && clan_lookup(victim->faction)->alliance != 0) {
          boost_territory(loc, clan_lookup(victim->faction)->alliance, amount / 1000, TRUE);
        }
        else {
          send_to_char("They're not in an ellible faction.\n\r", ch);
          return;
        }
        ch->pcdata->account->sr_aexp -= amount;
        victim->pcdata->aexp -= amount / 4;
        send_to_char("Done.\n\r", ch);
        printf_to_char(victim, "Your faction gains %d percent increased control over %s for your role in the adventure.\n\r", amount / 1000, loc->name);
      }
      else {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        for (obj = ch->carrying; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;

          if (is_name(arg10, obj->name) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE && can_drop_obj(ch, obj)) {
            int amount = aobjval(obj);
            if ((amount <= 0 || amount > UMIN(ch->pcdata->account->sr_aexp, worlds_experience(victim, WORLD_EARTH))) && !IS_IMMORTAL(ch)) {
              printf_to_char(
              ch, "That object would cost %d adventure exp to transfer.\n\r", aobjval(obj));
              return;
            }
            if (obj->item_type == ITEM_CONTAINER) {
              send_to_char("You cannot award a container.\n\r", ch);
              return;
            }
            if (!IS_SET(obj->extra_flags, ITEM_NORESALE))
            SET_BIT(obj->extra_flags, ITEM_NORESALE);

            obj_from_char(obj);
            obj_to_char(obj, victim);
            ch->pcdata->account->sr_aexp -= amount;
            charge_worlds_exp(victim, WORLD_EARTH, amount);
            send_to_char("Done.\n\r", ch);
            printf_to_char(
            victim, "You receive %s for your participation in the adventure.\n\r", obj->description);
            return;
          }
        }
        send_to_char("Syntax: Plot award (person) money/influence/territory/object.\n\r", ch);
      }
      return;
    }
    else if (!str_cmp(arg1, "experience")) {
      printf_to_char(ch, "`WAdventure Experience:`x %d\n\r", ch->pcdata->aexp);
      if (is_gm(ch))
      printf_to_char(ch, "`WSR Adventure Experience`x: %d\n\r", ch->pcdata->account->sr_aexp);
      printf_to_char(ch, "`WWin Rate:`x `g%d%%`x\n\r", 100 * ch->pcdata->awins /
      UMAX(1, ch->pcdata->awins + ch->pcdata->alosses));
      printf_to_char(ch, "`WWilds Experience:`x %d\n\r", ch->pcdata->wexp);

      return;
    }
    else if (!str_cmp(arg1, "learn")) {
      int amount = atoi(arg2);
      if (amount <= 1 || amount > ch->pcdata->aexp) {
        printf_to_char(ch, "Syntax: Plot learn (2-%d)\n\r", ch->pcdata->aexp);
        return;
      }
      ch->pcdata->aexp -= amount;
      ch->pcdata->account->xp += amount * 10;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "finish")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to finish.\n\r", ch);
            return;
          }
          if ((*it)->type == PLOT_OTHER) {
            send_to_char("You should delete that instead.\n\r", ch);
            return;
          }
          offline_reward((*it)->author, TYPE_KARMA, 2000, NULL);
          ch->pcdata->aexp += 1000;
          ch->pcdata->astatus = 300;
          if (safe_strlen((*it)->storyline) > 2 && get_storyline(NULL, (*it)->storyline) != NULL) {
            get_storyline(NULL, (*it)->storyline)->power += 5;
            get_storyline(NULL, (*it)->storyline)->lastmodified = current_time;
            add_mastermind(get_storyline(NULL, (*it)->storyline), (*it)->mastermind, 5);
          }

          int gallery_count = 0;
          bool useother = FALSE;
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              gal->lastmodified = current_time;
              gallery_count++;
              if (str_cmp(ch->pcdata->account->name, gal->account_owner))
              useother = TRUE;
            }
          }
          if (useother == TRUE)
          offline_reward((*it)->author, TYPE_KARMA, 500, NULL);
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              int prev = gal->xp;
              gal->xp += 2 + (8 / UMAX(gallery_count, 1));
              if ((prev - (prev % 50)) / 50 < (gal->xp - (gal->xp % 50)) / 50)
              bookadd(ch, WORLD_EARTH);
            }
          }

          if ((*it)->finale == 1) {
            free_string(ch->pcdata->finale_location);
            ch->pcdata->finale_location = str_dup((*it)->territory);
            ch->pcdata->finale_timer = 300;
            printf_to_char(
            ch, "You may now freely edit the territory %s, or use territory refer (person) to hand this power off to someone else.\n\r", ch);
          }
          (*it)->valid = FALSE;
          send_to_char("Plot Completed.\n\r", ch);
          ch->pcdata->tboosts += 2;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to finish.\n\r", ch);
            return;
          }
          if ((*it)->type != PLOT_QUEST) {
            send_to_char("You should delete that instead.\n\r", ch);
            return;
          }
          ch->pcdata->aexp += 500;
          ch->pcdata->astatus = 300;
          if (safe_strlen((*it)->storyline) > 2 && get_storyline(NULL, (*it)->storyline) != NULL) {
            get_storyline(NULL, (*it)->storyline)->power += 4;
            get_storyline(NULL, (*it)->storyline)->lastmodified = current_time;
            add_mastermind(get_storyline(NULL, (*it)->storyline), (*it)->mastermind, 4);
          }
          int gallery_count = 0;
          bool useother = FALSE;
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              gal->lastmodified = current_time;
              gallery_count++;
              if (str_cmp(ch->pcdata->account->name, gal->account_owner))
              useother = TRUE;
            }
          }
          if (useother == TRUE)
          offline_reward((*it)->author, TYPE_KARMA, 500, NULL);
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              int prev = gal->xp;
              gal->xp += 1 + (8 / UMAX(gallery_count, 1));
              if ((prev - (prev % 50)) / 50 < (gal->xp - (gal->xp % 50)) / 50)
              bookadd(ch, WORLD_EARTH);
            }
          }

          (*it)->valid = FALSE;
          send_to_char("Plot Completed.\n\r", ch);
          ch->pcdata->tboosts += 2;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to finish.\n\r", ch);
            return;
          }
          ch->pcdata->aexp += 500;
          ch->pcdata->astatus = 300;
          if (safe_strlen((*it)->storyline) > 2 && get_storyline(NULL, (*it)->storyline) != NULL) {
            get_storyline(NULL, (*it)->storyline)->power += 4;
            get_storyline(NULL, (*it)->storyline)->lastmodified = current_time;
            add_mastermind(get_storyline(NULL, (*it)->storyline), (*it)->mastermind, 4);
          }
          int gallery_count = 0;
          bool useother = FALSE;
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              gal->lastmodified = current_time;
              gallery_count++;
              if (str_cmp(ch->pcdata->account->name, gal->account_owner))
              useother = TRUE;
            }
          }
          if (useother == TRUE)
          offline_reward((*it)->author, TYPE_KARMA, 500, NULL);
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              int prev = gal->xp;
              gal->xp += 2 + (8 / UMAX(gallery_count, 1));
              if ((prev - (prev % 50)) / 50 < (gal->xp - (gal->xp % 50)) / 50)
              bookadd(ch, WORLD_EARTH);
            }
          }
          (*it)->valid = FALSE;
          send_to_char("Plot Completed.\n\r", ch);
          ch->pcdata->tboosts += 2;
        }
      }
    }
    else if (!str_cmp(arg1, "advertise")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to advertise.\n\r", ch);
            return;
          }

          CHAR_DATA *victim;
          for (DescList::iterator itt = descriptor_list.begin();
          itt != descriptor_list.end(); ++itt) {
            DESCRIPTOR_DATA *d = *itt;

            if (d->connected != CON_PLAYING)
            continue;
            victim = CH(d);

            if (victim == NULL)
            continue;

            if (IS_NPC(victim))
            continue;

            if (victim->in_room == NULL)
            continue;

            printf_to_char(victim, "Advertisement for %s:%s\n\r", (*it)->title, argument);
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to finish.\n\r", ch);
            return;
          }
          CHAR_DATA *victim;
          for (DescList::iterator itt = descriptor_list.begin();
          itt != descriptor_list.end(); ++itt) {
            DESCRIPTOR_DATA *d = *itt;

            if (d->connected != CON_PLAYING)
            continue;
            victim = CH(d);

            if (victim == NULL)
            continue;

            if (IS_NPC(victim))
            continue;

            if (victim->in_room == NULL)
            continue;

            printf_to_char(victim, "Advertisement for %s:%s\n\r", (*it)->title, argument);
          }
        }
      }
    }
    else if (!str_cmp(arg1, "transfer")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to transfer.\n\r", ch);
            return;
          }
          free_string((*it)->author);
          (*it)->author = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to transfer.\n\r", ch);
            return;
          }
          free_string((*it)->author);
          (*it)->author = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to transfer.\n\r", ch);
            return;
          }
          free_string((*it)->author);
          (*it)->author = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to transfer.\n\r", ch);
            return;
          }
          free_string((*it)->author);
          (*it)->author = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "storyline") && IS_IMMORTAL(ch)) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to advertise.\n\r", ch);
            return;
          }

          free_string((*it)->storyline);
          (*it)->storyline = str_dup(argument);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          free_string((*it)->storyline);
          (*it)->storyline = str_dup(argument);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          free_string((*it)->storyline);
          (*it)->storyline = str_dup(argument);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;

        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your story to finish.\n\r", ch);
            return;
          }
          free_string((*it)->storyline);
          (*it)->storyline = str_dup(argument);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "create")) {
      if (!is_gm(ch)) {
        send_to_char("This command is for storyrunners.\n\r", ch);
        return;
      }
      if (!IS_IMMORTAL(ch)) {
        send_to_char("Submit a storyidea first\n\r", ch);
        return;
      }

      plot = new_plot();
      free_string(plot->author);
      plot->author = str_dup(ch->name);
      PlotVect.push_back(plot);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "apply")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->type == PLOT_OTHER)
          return;
          int rating = 10;

          if ((*it)->type == PLOT_ADVENTURE) {
            rating = subfac_score(ch, (*it)->subfactionone);
          }
          else if ((*it)->type == PLOT_JOINT) {
            rating = (subfac_score(ch, (*it)->subfactionone) +
            subfac_score(ch, (*it)->subfactiontwo)) /
            2;
          }
          else if ((*it)->type == PLOT_PVP) {
            rating = UMAX(subfac_score(ch, (*it)->subfactionone), subfac_score(ch, (*it)->subfactiontwo));
          }

          if (ch->pcdata->association != 0 && ch->pcdata->association == (*it)->subfactionone)
          rating += 100;
          if (ch->pcdata->association != 0 && ch->pcdata->association == (*it)->subfactiontwo)
          rating += 100;

          if (!str_cmp(ch->pcdata->home_territory, (*it)->territory) && safe_strlen((*it)->territory) > 3)
          rating += 100;

          rating = rating * solidity(ch) / 100;

          for (int j = 0; j < 30; j++) {
            if ((*it)->rating[j] == 0) {
              free_string((*it)->participants[j]);
              (*it)->participants[j] = str_dup(ch->name);
              (*it)->rating[j] = rating;
              free_string((*it)->comments[j]);
              if (safe_strlen(argument) < 3)
              (*it)->comments[j] = str_dup("None");
              else
              (*it)->comments[j] = str_dup(argument);

              send_to_char("You apply.\n\r", ch);
              return;
            }
          }
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->type == PLOT_OTHER)
          return;
          int rating = 10;

          if ((*it)->type == PLOT_ADVENTURE) {
            rating = subfac_score(ch, (*it)->subfactionone);
          }
          else if ((*it)->type == PLOT_JOINT) {
            rating = (subfac_score(ch, (*it)->subfactionone) +
            subfac_score(ch, (*it)->subfactionone)) /
            2;
          }
          else if ((*it)->type == PLOT_PVP) {
            rating = UMAX(subfac_score(ch, (*it)->subfactionone), subfac_score(ch, (*it)->subfactionone));
          }

          if (!str_cmp(ch->pcdata->home_territory, (*it)->territory) && safe_strlen((*it)->territory) > 3)
          rating += 100;

          for (int j = 0; j < 30; j++) {
            if ((*it)->rating[j] == 0) {
              free_string((*it)->participants[j]);
              (*it)->participants[j] = str_dup(ch->name);
              (*it)->rating[j] = rating;
              free_string((*it)->comments[j]);
              if (safe_strlen(argument) < 3)
              (*it)->comments[j] = str_dup("None");
              else
              (*it)->comments[j] = str_dup(argument);

              send_to_char("You apply.\n\r", ch);
              return;
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "times")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->timezone);
          (*it)->timezone = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->timezone);
          (*it)->timezone = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->timezone);
          (*it)->timezone = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->timezone);
          (*it)->timezone = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "visibility")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "all"))
          (*it)->visibility = VISIBILITY_ALL;
          else if (!str_cmp(argument, "Storyrunners"))
          (*it)->visibility = VISIBILITY_SRS;
          else if (!str_cmp(argument, "None"))
          (*it)->visibility = VISIBILITY_IMMS;
          else {
            send_to_char("Options are, All, None, Storyrunners.\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
        }
      }

      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "all"))
          (*it)->visibility = VISIBILITY_ALL;
          else if (!str_cmp(argument, "Storyrunners"))
          (*it)->visibility = VISIBILITY_SRS;
          else if (!str_cmp(argument, "None"))
          (*it)->visibility = VISIBILITY_IMMS;
          else {
            send_to_char("Options are, All, None, Storyrunners.\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "all"))
          (*it)->visibility = VISIBILITY_ALL;
          else if (!str_cmp(argument, "Storyrunners"))
          (*it)->visibility = VISIBILITY_SRS;
          else if (!str_cmp(argument, "None"))
          (*it)->visibility = VISIBILITY_IMMS;
          else {
            send_to_char("Options are, All, None, Storyrunners.\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
        }
      }

      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "all"))
          (*it)->visibility = VISIBILITY_ALL;
          else if (!str_cmp(argument, "Storyrunners"))
          (*it)->visibility = VISIBILITY_SRS;
          else if (!str_cmp(argument, "None"))
          (*it)->visibility = VISIBILITY_IMMS;
          else {
            send_to_char("Options are, All, None, Storyrunners.\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "title")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->title);
          (*it)->title = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->title);
          (*it)->title = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->title);
          (*it)->title = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->title);
          (*it)->title = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "character")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->character);
          (*it)->character = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->character);
          (*it)->character = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->character);
          (*it)->character = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          free_string((*it)->character);
          (*it)->character = str_dup(argument);
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "territory") && IS_IMMORTAL(ch)) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          LOCATION_TYPE *loc;
          loc = get_loc(argument);
          if (loc == NULL) {
            send_to_char("No such territory.\n\r", ch);
            return;
          }
          free_string((*it)->territory);
          (*it)->territory = str_dup(loc->name);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          LOCATION_TYPE *loc;
          loc = get_loc(argument);
          if (loc == NULL) {
            send_to_char("No such territory.\n\r", ch);
            return;
          }
          free_string((*it)->territory);
          (*it)->territory = str_dup(loc->name);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          LOCATION_TYPE *loc;
          loc = get_loc(argument);
          if (loc == NULL) {
            send_to_char("No such territory.\n\r", ch);
            return;
          }
          free_string((*it)->territory);
          (*it)->territory = str_dup(loc->name);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          LOCATION_TYPE *loc;
          loc = get_loc(argument);
          if (loc == NULL) {
            send_to_char("No such territory.\n\r", ch);
            return;
          }
          free_string((*it)->territory);
          (*it)->territory = str_dup(loc->name);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }

    }
    else if (!str_cmp(arg1, "calendar")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->calendar = atoi(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->calendar = atoi(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->calendar = atoi(argument);
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->calendar = atoi(argument);
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "size")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "small")) {
            (*it)->size = 0;
          }
          else if (!str_cmp(argument, "medium")) {
            (*it)->size = 1;
          }
          else if (!str_cmp(argument, "large")) {
            (*it)->size = 2;
          }
          else {
            send_to_char("Valid sizes are small/medium/large\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "small")) {
            (*it)->size = 0;
          }
          else if (!str_cmp(argument, "medium")) {
            (*it)->size = 1;
          }
          else if (!str_cmp(argument, "large")) {
            (*it)->size = 2;
          }
          else {
            send_to_char("Valid sizes are small/medium/large\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else if (!str_cmp(arg1, "challenge")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->challenge);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->challenge);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->challenge);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->challenge);
        }
      }

    }
    else if (!str_cmp(arg1, "events")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->events);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->events);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->events);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->events);
        }
      }

    }
    else if (!str_cmp(arg1, "storyrunners")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->storyrunners);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->storyrunners);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->storyrunners);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->storyrunners);
        }
      }

    }
    else if (!str_cmp(arg1, "staff")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->immortals);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->immortals);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->immortals);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->immortals);
        }
      }

    }
    else if (!str_cmp(arg1, "members")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          send_to_char("Only quests and personal plots have participants.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->members);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          send_to_char("Only quests and personal plots have participants.\n\r", ch);
          return;
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          send_to_char("Only quests and personal plots have participants.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "delete")) {
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_MYSTERY)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Done.\n\r", ch);
        }
      }
      for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
      it != PlotVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!visible_plot(ch, (*it)))
        continue;
        if ((*it)->type != PLOT_OTHER)
        continue;
        i++;

        if (i == atoi(arg2)) {
          if (str_cmp(ch->name, (*it)->author) && !IS_IMMORTAL(ch)) {
            send_to_char("That's not your plot to change.\n\r", ch);
            return;
          }
          (*it)->valid = FALSE;
          send_to_char("Done.\n\r", ch);
        }
      }

    }
    else
    send_to_char("Syntax: plot list/plot info (number)/plot create/plot title (string)/plot character (string)/plot calendar (number)/plot events/plot challenge/plot storyrunners/plot staff/plot members/plot delete.\n\r", ch);
  }

  char *const story_status[] = {
    "`yIncomplete`x", "`gPending`x", "`GApproved`x", "`GTime Approved`x", "`YSponsored`x",  "`DLocked`x",  "`WClaimed`x",  ""};

  int storyideacount(CHAR_DATA *ch) {
    int val = 0;
    for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
    it != StoryIdeaVect.end(); ++it) {

      if (!str_cmp((*it)->claimant, ch->name) && (*it)->status == STATUS_CLAIMED)
      val++;
    }
    return val;
  }

  _DOFUN(do_storyidea) {

    char arg1[MSL];
    char arg2[MSL];
    STORYIDEA_TYPE *story;
    int i = 0;

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (!str_cmp(arg1, "list")) {
      int i = 1;
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {

        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch)) {
          continue;
        }
        if (!cross_sr_compare((*it)->author, ch->name) && !IS_IMMORTAL(ch)) {
          continue;
        }

        if ((*it)->status == STATUS_DELETE)
        continue;

        if (!cross_sr_compare((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time) {
          continue;
        }
        /*
if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
{
continue;
}
*/
        printf_to_char(ch, "`W%d`c)`x [%s]\n\r", i, story_status[(*it)->status]);
        i++;
      }
    }
    else if (!str_cmp(arg1, "count")) {
      int i = 0;
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        i++;
      }
      printf_to_char(ch, "%d\n\r", i);
    }
    else if (!str_cmp(arg1, "info")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if (!cross_sr_compare((*it)->author, ch->name) && !IS_IMMORTAL(ch))
        continue;

        if (!cross_sr_compare((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (!cross_sr_compare((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          printf_to_char(ch, "`W%d`c)`x [%s]\n\r", i, story_status[(*it)->status]);
          if (IS_IMMORTAL(ch))
          printf_to_char(ch, "`WAuthor:`x %s\n\r`WClaimant:`x %s`x\n\r", (*it)->author, (*it)->claimant);
          printf_to_char(ch, "`WType:`x %s\n\r", plot_type((*it)->type));
          if ((*it)->type != PLOT_OTHER) {
            printf_to_char(ch, "`WSubfaction: %s\n\r", get_subfac_name((*it)->subfactionone));
            if ((*it)->type != PLOT_ADVENTURE)
            printf_to_char(ch, "`WSubfactionTwo: %s\n\r", get_subfac_name((*it)->subfactiontwo));
          }
          printf_to_char(ch, "`WStoryline:`x %s\n\r", (*it)->storyline);
          if ((*it)->finale == 1)
          send_to_char("`WThis is a storyline finale.\n\r", ch);
          printf_to_char(ch, "`WDescription:`x %s\n\r", (*it)->description);
          printf_to_char(ch, "`WNPCs:`x %s\n\r", (*it)->npcs);
          printf_to_char(ch, "`WLore:`x %s\n\r", (*it)->lore);
          printf_to_char(ch, "`WTerritory:`x %s\n\r", (*it)->territory);
          printf_to_char(ch, "`WMastermind:`x %s\n\r", (*it)->mastermind);
          for (int x = 0; x < 10; x++) {
            if (safe_strlen((*it)->galleries[x]) > 1)
            printf_to_char(ch, "`WGallery NPC:`x %s\n\r", (*it)->galleries[x]);
          }
          if(IS_IMMORTAL(ch))
          printf_to_char(ch, "`WSecrets:`x %s\n\r", (*it)->secrets);

          return;
        }
      }
    }
    else if (!str_cmp(arg1, "approve")) {
      if (!IS_IMMORTAL(ch))
      return;

      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {

          (*it)->status = STATUS_APPROVED;
          send_to_char("Done.\n\r", ch);
          (*it)->time_owned = current_time + (24 * 60 * 60);

          if ((*it)->type == PLOT_CHARACTER) {
            (*it)->valid = FALSE;
            (*it)->status = STATUS_DELETE;

            message_to_char((*it)->author, "Your character concept was approved. ");
            message_to_char((*it)->author, argument);
          }
          else if (safe_strlen(argument) > 5) {
            message_to_char(
            (*it)->author, "Your story idea was approved with the following comments;");
            message_to_char((*it)->author, argument);
          }
        }
      }
    }
    else if (!str_cmp(arg1, "crisisapprove")) {
      if (!IS_IMMORTAL(ch))
      return;

      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //              continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->crisis = 1;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "sponsor")) {
      if (!IS_IMMORTAL(ch))
      return;

      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->status = STATUS_SPONSORED;
          send_to_char("Done.\n\r", ch);
          (*it)->time_owned = current_time + (24 * 60 * 60);
        }
      }
    }
    else if (!str_cmp(arg1, "lock")) {
      if (!IS_IMMORTAL(ch))
      return;

      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->status = STATUS_LOCKED;
          send_to_char("Done.\n\r", ch);
          (*it)->time_owned = current_time + (24 * 60 * 60);
          (*it)->time_locked = current_time + (atoi(argument) * 24 * 60 * 60);
        }
      }
    }
    else if (!str_cmp(arg1, "Deny")) {
      if (!IS_IMMORTAL(ch))
      return;

      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->valid = FALSE;
          (*it)->status = STATUS_DELETE;
          send_to_char("Done.\n\r", ch);

          if ((*it)->type == PLOT_CHARACTER) {
            message_to_char((*it)->author, "Your character concept was not approved because;");
            message_to_char((*it)->author, argument);
          }
          else if (safe_strlen(argument) > 5) {
            message_to_char((*it)->author, "Your story idea was turned down because;");
            message_to_char((*it)->author, argument);
          }
        }
      }
    }
    else if (!str_cmp(arg1, "submit")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if (str_cmp((*it)->author, ch->name) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          if (safe_strlen((*it)->description) < 2) {
            send_to_char("Your story needs a description, type storyidea description (number) to edit it\n\r", ch);
            return;
          }
          if (safe_strlen((*it)->npcs) < 2 && (*it)->type == PLOT_OTHER) {
            send_to_char("Your story needs it's npcs detailed, type storyidea npcs (number) to edit it\n\r", ch);
            return;
          }
          if (safe_strlen((*it)->lore) < 2 && (*it)->type == PLOT_OTHER) {
            send_to_char("Your story needs it's lore detailed, type storyidea lore (number) to edit it\n\r", ch);
            return;
          }
          if (!is_gm(ch) && ((*it)->type != PLOT_QUEST && (*it)->type != PLOT_PERSONAL && (*it)->type != PLOT_CHARACTER)) {
            send_to_char("You aren't a SR.\n\r", ch);
            return;
          }
          (*it)->status = STATUS_PENDING;
          (*it)->time_locked = current_time + (5 * 24 * 60 * 60);
          ch->pcdata->account->storyidea_cooldown =
          (current_time + (3600 * 24 * 21));

          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "create")) {
      if (ch->pcdata->account->storyidea_cooldown != 0 && ch->pcdata->account->storyidea_cooldown > current_time) {
        send_to_char("You can't make a new storyidea yet.\n\r", ch);
        return;
      }
      if(!is_gm(ch))
      {
        send_to_char("You have to do that from your SR.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 23;
      send_to_char("Done.\n\r", ch);
      return;

      story = new_storyidea();
      free_string(story->author);
      story->author = str_dup(ch->name);
      story->status = STATUS_INCOMPLETE;
      StoryIdeaVect.push_back(story);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "unclaim")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {

        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status != STATUS_CLAIMED) {
            send_to_char("That hasn't been claimed.\n\r", ch);
            return;
          }

          send_to_char("`WStory Unclaimed.`x\n\r", ch);
          (*it)->status = STATUS_APPROVED;
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "makecalendar")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;
        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !IS_IMMORTAL(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;
        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;
        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;
        i++;
        if (i == atoi(arg2)) {
          if ((*it)->status != STATUS_CLAIMED) {
            send_to_char("That hasn't been claimed.\n\r", ch);
            return;
          }
          if ((*it)->type != PLOT_CRISIS)
          return;
          STORY_TYPE *story = new_story();
          free_string(story->author);
          story->author = str_dup(ch->name);
          story->time = current_time + (3600 * 24);
          story->crisis = 1;
          if (safe_strlen((*it)->storyline) > 2 && get_storyline(NULL, (*it)->storyline) != NULL) {
            get_storyline(NULL, (*it)->storyline)->power += 3;
            get_storyline(NULL, (*it)->storyline)->lastmodified = current_time;
            add_mastermind(get_storyline(NULL, (*it)->storyline), (*it)->mastermind, 5);
          }
          int gallery_count = 0;
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              gal->lastmodified = current_time;
              gallery_count++;
            }
          }
          for (int i = 0; i < 10; i++) {
            GALLERY_TYPE *gal = NULL;
            if ((gal = gallery_lookup((*it)->galleries[i])) != NULL) {
              int prev = gal->xp;
              gal->xp += 2 + (8 / UMAX(gallery_count, 1));
              if ((prev - (prev % 50)) / 50 < (gal->xp - (gal->xp % 50)) / 50)
              bookadd(ch, WORLD_EARTH);
            }
          }
          send_to_char("Done.\n\r", ch);
          StoryVect.push_back(story);
          (*it)->status = STATUS_DELETE;
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "makeplot")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {

        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant) && safe_strlen((*it)->claimant) > 2) && !IS_IMMORTAL(ch))
        continue;

        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {

          if ((*it)->status != STATUS_CLAIMED) {
            send_to_char("That hasn't been claimed.\n\r", ch);
            return;
          }
          if ((*it)->type == PLOT_CRISIS)
          return;
          PLOT_TYPE *plot;

          plot = new_plot();
          free_string(plot->author);
          plot->author = str_dup(ch->name);
          plot->type = (*it)->type;
          plot->subfactionone = (*it)->subfactionone;
          plot->subfactiontwo = (*it)->subfactiontwo;
          plot->seeking_type = (*it)->seeking_type;
          free_string(plot->title);
          plot->title = str_dup(argument);
          char title[MSL];
          sprintf(title, "%s\tPlot made: %s", ch->pcdata->account->sr_history, plot->title);
          free_string(ch->pcdata->account->sr_history);
          ch->pcdata->account->sr_history = str_dup(title);
          free_string(plot->immortals);
          plot->immortals = str_dup((*it)->secrets);
          free_string(plot->territory);
          plot->territory = str_dup((*it)->territory);
          free_string(plot->eidolon);
          plot->eidolon = str_dup((*it)->eidolon);

          free_string(plot->storyline);
          plot->storyline = str_dup((*it)->storyline);
          free_string(plot->mastermind);
          plot->mastermind = str_dup((*it)->mastermind);
          plot->crisis = (*it)->crisis;
          for (int i = 0; i < 10; i++) {
            free_string(plot->galleries[i]);
            plot->galleries[i] = str_dup((*it)->galleries[i]);
          }
          if ((*it)->finale == 1) {
            if (plot->type == PLOT_OTHER)
            plot->crisis = 1;
            plot->finale = 1;
            get_storyline(NULL, (*it)->storyline)->power -= 15;
          }
          if ((*it)->status == STATUS_SPONSORED)
          plot->sponsored = 1;
          PlotVect.push_back(plot);

          send_to_char("`WPlot created`x\n\r", ch);

          /*
if(plot->type == PLOT_ADVENTURE || plot->type == PLOT_PVP || plot->type == PLOT_JOINT)
{
offline_reward((*it)->author, TYPE_KARMA, 2500);
}
if((*it)->status == STATUS_SPONSORED)
offline_reward((*it)->author, TYPE_KARMA, 2500);
*/
          (*it)->status = STATUS_DELETE;
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "finished") || !str_cmp(arg1, "finish")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {

        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !IS_IMMORTAL(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status != STATUS_CLAIMED) {
            send_to_char("That hasn't been claimed.\n\r", ch);
            return;
          }

          send_to_char("`WStory Finished.`x\n\r", ch);
          (*it)->status = STATUS_DELETE;
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "claim")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if (str_cmp((*it)->author, ch->name) && !IS_IMMORTAL(ch))
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status < STATUS_APPROVED || (*it)->status > STATUS_SPONSORED) {
            send_to_char("That hasn't been approved yet.\n\r", ch);
            return;
          }
          if ((*it)->type == PLOT_QUEST || (*it)->type == PLOT_PERSONAL) {
            if (!is_name(ch->name, (*it)->members) && safe_strlen((*it)->members) > 2) {
              send_to_char("You don't have permission to claim that.\n\r", ch);
              return;
            }
          }
          if ((*it)->status == STATUS_LOCKED) {
            send_to_char("That's locked.\n\r", ch);
            return;
          }
          if ((*it)->type == PLOT_ADVENTURE || (*it)->type == PLOT_PVP || (*it)->type == PLOT_JOINT) {
            if (str_cmp(ch->name, (*it)->author)) {
              send_to_char("Only the author of an adventure can claim it.\n\r", ch);
              return;
            }
          }
          if (storyideacount(ch) >= 3) {
            send_to_char("You already have 3 ideas claimed, finish them first.\n\r", ch);
            return;
          }

          send_to_char("`WStory Claimed.`x\n\r", ch);
          printf_to_char(ch, "`WDescription:`x %s\n\r", (*it)->description);
          printf_to_char(ch, "`WNPCs:`x %s\n\r", (*it)->npcs);
          printf_to_char(ch, "`WLore:`x %s\n\r", (*it)->lore);
          printf_to_char(ch, "`WSecrets:`x %s\n\r", (*it)->secrets);
          free_string((*it)->claimant);
          (*it)->claimant = str_dup(ch->name);
          (*it)->status = STATUS_CLAIMED;
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "permissions")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if (str_cmp((*it)->author, ch->name) && !IS_IMMORTAL(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          send_to_char("`WList all the SRs you'd like to be able to take your idea here separated by a space. Leave blank to allow any to claim it.`x\n\r", ch);
          string_append(ch, &(*it)->members);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "description")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          send_to_char("`WWrite the description for your story idea here.`x\n\r", ch);
          string_append(ch, &(*it)->description);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "galleryadd")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !IS_IMMORTAL(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;
        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {

          if (gallery_lookup(argument) != NULL) {
            for (int j = 0; j < 10; j++) {
              if (safe_strlen((*it)->galleries[j]) < 2) {
                free_string((*it)->galleries[j]);
                (*it)->galleries[j] = str_dup(argument);
                send_to_char("NPC added.\n\r", ch);
                return;
              }
            }
          }
          else
          send_to_char("No such Gallery NPC.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "gallerydelete")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;
        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {

          for (int j = 0; j < 10; j++) {
            if (!str_cmp(argument, (*it)->galleries[j])) {
              free_string((*it)->galleries[j]);
              (*it)->galleries[j] = str_dup("");
              send_to_char("NPC removed.\n\r", ch);
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "mastermind")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          free_string((*it)->mastermind);
          (*it)->mastermind = str_dup(argument);
          printf_to_char(ch, "%s is now the mastermind of this story.\n\r", argument);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "subfaction")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          if (!valid_subfac(argument)) {
            send_to_char("Valid Subfactions are: ", ch);
            print_all_subfacs(ch);
            return;
          }
          (*it)->subfactionone = get_subfac(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "subfactiontwo")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          if (!valid_subfac(argument)) {
            send_to_char("Valid Subfactions are: ", ch);
            print_all_subfacs(ch);
            return;
          }
          (*it)->subfactiontwo = get_subfac(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "territory")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          LOCATION_TYPE *loc;
          loc = get_loc(argument);
          if (loc == NULL) {
            send_to_char("No such territory.\n\r", ch);
            return;
          }
          free_string((*it)->territory);
          (*it)->territory = str_dup(loc->name);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "storyline")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if (safe_strlen(argument) > 2 && get_storyline(ch, argument) != NULL) {
            (*it)->finale = 0;
            free_string((*it)->storyline);
            (*it)->storyline = str_dup(get_storyline(ch, argument)->name);
            send_to_char("Done.\n\r", ch);
            if (get_storyline(ch, argument)->power >= 15) {
              send_to_char("You can make this plot a storyline finale, use storyidea finale (number) to do so.\n\r", ch);
            }
            return;
          }
          send_to_char("No such storyline.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "finale")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !IS_IMMORTAL(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if (get_storyline(NULL, (*it)->storyline) != NULL) {
            if (get_storyline(NULL, (*it)->storyline)->power < 15) {
              send_to_char("That storyline must be progressed further first.\n\r", ch);
              return;
            }
            (*it)->finale = 1;
            send_to_char("Done.\n\r", ch);
            return;
          }
          send_to_char("No such storyline.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "reward")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "type")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //            if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //                continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "Adventure")) {
            if (!is_gm(ch)) {
              send_to_char("Only Story runners can make those.\n\r", ch);
              return;
            }
            (*it)->type = PLOT_ADVENTURE;
          }
          else if (!str_cmp(argument, "Joint")) {
            if (!is_gm(ch)) {
              send_to_char("Only Story runners can make those.\n\r", ch);
              return;
            }

            (*it)->type = PLOT_JOINT;
          }
          else if (!str_cmp(argument, "Crisis")) {
            if (!is_gm(ch) || ch->pcdata->account->karmaearned < 25000) {
              send_to_char("Only Story runners who've earned at least 25k karma can make those.\n\r", ch);
              return;
            }
            (*it)->type = PLOT_CRISIS;
          }
          else if (!str_cmp(argument, "Mystery")) {
            if (!is_gm(ch)) {
              send_to_char("Only Story runners can make those.\n\r", ch);
              return;
            }
            (*it)->type = PLOT_MYSTERY;
          }
          else if (!str_cmp(argument, "Competitive")) {
            if (!is_gm(ch)) {
              send_to_char("Only Story runners can make those.\n\r", ch);
              return;
            }

            (*it)->type = PLOT_PVP;
          }
          else if (!str_cmp(argument, "Other")) {
            (*it)->type = PLOT_OTHER;
          }
          else if (!str_cmp(argument, "Character"))
          (*it)->type = PLOT_CHARACTER;
          else if (!str_cmp(argument, "Quest"))
          (*it)->type = PLOT_QUEST;
          else if (!str_cmp(argument, "Personal"))
          (*it)->type = PLOT_PERSONAL;
          else {
            send_to_char("Valid types are Adventure/Joint/Competitive/Other/Character/Quest/Personal/Crisis.\n\r", ch);
            return;
          }
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "npcs")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          send_to_char("`WDetail any NPCs involved in your story here, if there are none, just type none.`x\n\r", ch);
          string_append(ch, &(*it)->npcs);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "lore")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          send_to_char("`WDetail any lore involved in your story, such as magic, technology, history, creatures etc, if there are none, just type none.`x\n\r", ch);
          string_append(ch, &(*it)->lore);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "secrets")) {
      for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
      it != StoryIdeaVect.end(); ++it) {
        if ((*it)->status == STATUS_CLAIMED && (str_cmp(ch->name, (*it)->claimant)) && !IS_IMMORTAL(ch))
        continue;

        if ((str_cmp((*it)->author, ch->name) || (*it)->status != STATUS_INCOMPLETE) && !is_gm(ch))
        continue;
        if ((*it)->status == STATUS_DELETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status == STATUS_INCOMPLETE)
        continue;

        if (str_cmp((*it)->author, ch->name) && (*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && (*it)->time_owned > current_time)
        continue;

        //      if(!IS_IMMORTAL(ch) && (*it)->status == STATUS_PENDING)
        //    continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->status >= STATUS_APPROVED && (*it)->status <= STATUS_SPONSORED && !IS_IMMORTAL(ch)) {
            send_to_char("That story has already been approved as is.\n\r", ch);
            return;
          }
          send_to_char("`WDetail any secrets in your plot, these will only be seen by the SR who takes on the story.`x\n\r", ch);
          string_append(ch, &(*it)->secrets);
          return;
        }
      }
    }
    else
    send_to_char("Possible commands are list/info/create/description/npcs/lore/secrets/submit/claim/approve/lock/sponsor/deny/permissions/claim/unclaim/finish/makeplot\n\r", ch);
  }

#define SF_NONE 0
#define SF_BLACKCIRCLE 1
#define SF_ORDERSHIELD 2
#define SF_ORDERSWORD 3
#define SF_ORDERLIB 4
#define SF_TEMPLESTRIKE 5
#define SF_TEMPLEINT 6
#define SF_TEMPLEDEMO 7
#define SF_HANDPEACE 8
#define SF_HANDSHADOW 9
#define SF_HANDWHISPER 10
#define SF_DYNASTY 11
#define SF_REDCIRCLE 12
#define SF_AQUARIAN 13
#define SF_SHERIFF 14
#define SF_FBI 15
#define SF_INTERPOL 16
#define SF_BOUNTY 17
#define SF_MOB 18
#define SF_RUSSIANMAFIA 19
#define SF_DESMOND 20
#define SF_CIA 21
#define SF_MI6 22
#define SF_CHINESEINT 23
#define SF_RUSHELL 24
#define SF_TYRELL 25
#define SF_HANSHIN 26
#define SF_NEWWORLD 27
#define SF_CHOSEN 28
#define SF_FORSAKEN 29
#define SF_PROMETHEANS 30
#define SF_ODIN 31
#define SF_RA 32
#define SF_ZEUS 33
#define SF_JADE 34
#define SF_VISHNU 35
#define SF_JUPITER 36
#define SF_UNDERGODS 37
#define SF_CIRCUS 38
#define SF_GAMEMASTER 39
#define SF_BEASTMASTER 40
#define SF_BANISHED 41
#define SF_SYNDICATE 42
#define SF_WATCHERS 43
#define SF_SENTINELS 44
#define SF_LOST 45
#define SF_FAELINGS 46
#define SF_COURT 47
#define SF_PACK 48
#define SF_FREE 49
#define SF_ILLARIN 50
#define SF_PEOPLE 51
#define SF_NAR 52
#define SF_COVEN 53
#define SF_WITCHHUNT 54
#define SF_HUNTERS 55
#define SF_RUSSIANINT 56
#define SF_WHITECIRCLE 57
#define SF_VAMPIREEMP 58

  char *const subfac_names[] = {"None", "The Black Circle", "Order ShieldBearers", "Order SwordBearers", "Order Librarians", "Temple Strike Force", "Temple Intelligence", "Temple Demolishers", "The Peacekeeping Hand", "The Shadow Hand", "The Whispering Hand", "The Dynasty", "The Red Circle", "Aquarian Guard", "Sheriff", "FBI", "Interpol", "RockField Bounties", "The Mob", "Russian Mafia", "Desmond King", "CIA", "MI6", "Chinese Intelligence", "Rushell Industries", "Tyrell Corp", "The Hanshin Group", "New World Order", "The Chosen", "The Forsaken", "The Prometheans", "The House of Odin", "The House of Ra", "The House of Zeus", "The House of Jade", "The House of Vishnu", "The House of Jupiter", "The Undergods", "The Circus", "The Gamemasters", "The Beastmasters", "The Banished", "The Syndicate", "The Watchers", "The Sentinels", "The Lost", "The Faelings", "The Vampire Court of New York", "The Redwood Pack", "The Free", "The Illarin Empire", "The People", "The Kingdom of Nar", "The Coven", "The Witch Hunters", "The Hunters", "Russian Intelligence Service", "The White Circle", "The Vampire Empire"};

  char *get_subfac_name(int val) { return subfac_names[val]; }
  char *reward_string(int val) {
    if (val == REWARD_MONEY)
    return "Money";
    if (val == REWARD_INFLUENCE)
    return "Influence";
    if (val == REWARD_RESPECT)
    return "Respect";
    if (val == REWARD_MAGICAL)
    return "Magical";

    return "None";
  }

  int get_subfac(char *word) {
    for (int i = 0; i < SF_MAX; i++) {
      if (!str_cmp(word, subfac_names[i]))
      return i;
    }
    return 0;
  }
  bool valid_subfac(char *word) {
    for (int i = 0; i < SF_MAX; i++) {
      if (!str_cmp(word, subfac_names[i]))
      return TRUE;
    }
    return FALSE;
  }
  void print_all_subfacs(CHAR_DATA *ch) {
    for (int i = 1; i < SF_MAX; i++) {
      send_to_char(subfac_names[i], ch);
      send_to_char(", ", ch);
    }
    send_to_char("\n\r", ch);
  }

  int subfac_score(CHAR_DATA *ch, int subfac) {
    int val = 100;

    if (subfac == SF_ORDERSHIELD || subfac == SF_ORDERSWORD || subfac == SF_ORDERLIB) {
    }
    if (subfac == SF_TEMPLESTRIKE || subfac == SF_TEMPLEINT || subfac == SF_TEMPLEDEMO) {
    }
    if (subfac == SF_HANDPEACE || subfac == SF_HANDSHADOW || subfac == SF_HANDWHISPER) {
      if (is_super(ch))
      val += 50;
    }
    if (subfac == SF_AQUARIAN) {
      if (water_breathe(ch))
      val += 100;
    }
    if (subfac == SF_SHERIFF) {
      val += get_skill(ch, SKILL_CPOLICE) * 25;
    }
    if (subfac == SF_FBI) {
      val += get_skill(ch, SKILL_CPOLICE) * 25;
    }
    if (subfac == SF_INTERPOL) {
      val += get_skill(ch, SKILL_CPOLICE) * 25;
    }
    if (subfac == SF_BOUNTY) {
      val += get_skill(ch, SKILL_CPOLICE) * 25;
    }
    if (subfac == SF_MOB) {
      val += get_skill(ch, SKILL_CCRIMINAL) * 25;
    }
    if (subfac == SF_RUSSIANMAFIA) {
      val += get_skill(ch, SKILL_CCRIMINAL) * 25;
    }
    if (subfac == SF_DESMOND) {
      val += get_skill(ch, SKILL_CCRIMINAL) * 25;
    }
    if (subfac == SF_CIA) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMILITARY) * 25;
      val += get_skill(ch, SKILL_STEALTH) * 25;
    }
    if (subfac == SF_MI6) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMILITARY) * 25;
      val += get_skill(ch, SKILL_STEALTH) * 25;
    }
    if (subfac == SF_CHINESEINT) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMILITARY) * 25;
      val += get_skill(ch, SKILL_STEALTH) * 25;
    }
    if (subfac == SF_RUSSIANINT) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMILITARY) * 25;
      val += get_skill(ch, SKILL_STEALTH) * 25;
    }

    if (subfac == SF_RUSHELL) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMEDIA) * 25;
    }
    if (subfac == SF_TYRELL) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMEDIA) * 25;
    }
    if (subfac == SF_HANSHIN) {
      val += get_skill(ch, SKILL_CGOVERNMENT) * 25;
      val += get_skill(ch, SKILL_CMEDIA) * 25;
    }
    if (subfac == SF_NEWWORLD) {
      val += get_skill(ch, SKILL_CHOMELESS) * 25;
      val += get_skill(ch, SKILL_CMEDIA) * 25;
      val += get_skill(ch, SKILL_ANIMALS) * 25;
    }
    if (subfac == SF_CHOSEN) {
      if (is_demonborn(ch))
      val += 100;
    }
    if (subfac == SF_FORSAKEN) {
      if (is_demonborn(ch))
      val += 100;
    }
    if (subfac == SF_PROMETHEANS) {
      if (!is_super(ch))
      val += 100;
    }
    if (subfac == SF_ODIN) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_RA) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_ZEUS) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_JADE) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_VISHNU) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_JUPITER) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_UNDERGODS) {
      if (is_demigod(ch))
      val += 100;
      if (is_angelborn(ch))
      val += 50;
    }
    if (subfac == SF_CIRCUS) {
      if (is_faeborn(ch))
      val += 100;
    }
    if (subfac == SF_GAMEMASTER) {
      if (is_faeborn(ch))
      val += 100;
    }
    if (subfac == SF_BEASTMASTER) {
      if (is_super(ch))
      val += 100;
      val += get_skill(ch, SKILL_CHOMELESS) * 25;
    }
    if (subfac == SF_BANISHED) {
      if (is_faeborn(ch))
      val += 100;
      if (is_demonborn(ch))
      val += 100;
      val += get_skill(ch, SKILL_CCRIMINAL) * 25;
    }
    if (subfac == SF_SYNDICATE) {
      if (is_demonborn(ch))
      val += 100;
      val += get_skill(ch, SKILL_CCRIMINAL) * 25;
    }
    if (subfac == SF_WATCHERS) {
      val += get_skill(ch, SKILL_DEMONOLOGY) * 25;
    }
    if (subfac == SF_SENTINELS) {
      val += get_skill(ch, SKILL_CLAIRVOYANCE) * 25;
      val += get_skill(ch, SKILL_CLAIRAUDIENCE) * 25;
    }
    if (subfac == SF_LOST) {
      if (!is_super(ch))
      val += 100;
    }
    if (subfac == SF_FAELINGS) {
      if (is_faeborn(ch))
      val += 100;
    }
    if (subfac == SF_COURT) {
      if (is_vampire(ch))
      val += 100;
    }
    if (subfac == SF_PACK) {
      if (is_werewolf(ch))
      val += 100;
    }
    if (subfac == SF_FREE) {
      if (is_angelborn(ch))
      val += 100;
    }
    if (subfac == SF_ILLARIN) {
      if (is_wildling(ch))
      val += 100;
    }
    if (subfac == SF_PEOPLE) {
      if (is_wildling(ch))
      val += 100;
    }
    if (subfac == SF_NAR) {
      if (is_wildling(ch))
      val += 100;
    }
    if (subfac == SF_COVEN) {
      if (is_mage(ch))
      val += 100;
      if (ch->sex == SEX_MALE)
      val -= 150;
    }
    if (subfac == SF_BLACKCIRCLE || subfac == SF_REDCIRCLE || subfac == SF_WHITECIRCLE) {
      val += arcane_focus(ch) * 30;
    }
    if (subfac == SF_HUNTERS || subfac == SF_WITCHHUNT) {
      if (!is_super(ch))
      val += 100;
    }
    if (subfac == SF_VAMPIREEMP) {
      if (is_vampire(ch))
      val += 100;
    }
    if (ch->pcdata->association == subfac)
    val += 100;

    return val;
  }

  _DOFUN(do_associate) {
    LOCATION_TYPE *loc;
    if (!valid_subfac(argument)) {
      if ((loc = get_loc(argument)) == NULL || safe_strlen(argument) < 3 || loc->continent == 0) {
        send_to_char("Valid Subfactions are: ", ch);
        print_all_subfacs(ch);
        send_to_char("\nValid territories can be listed with the territory command.\n\r", ch);
        return;
      }

      if (safe_strlen(ch->pcdata->home_territory) > 3) {
        if (ch->pcdata->influence < 3000) {
          send_to_char("You don't have enough influence.\n\r", ch);
          return;
        }
        else
        ch->pcdata->influence -= 3000;
      }
      free_string(ch->pcdata->home_territory);
      ch->pcdata->home_territory = str_dup(loc->name);
      printf_to_char(ch, "Your home territory is now %s\n\r", loc->name);
      show_territory_to_char(ch, loc);
      return;
    }
    if (ch->pcdata->association != 0 && ch->pcdata->influence < 5000) {
      send_to_char("You don't have enough influence.\n\r", ch);
      return;
    }

    if (ch->pcdata->association != 0)
    ch->pcdata->influence -= 5000;
    ch->pcdata->association = get_subfac(argument);

    send_to_char("Association set.\n\r", ch);
  }

  bool has_crisis(CHAR_DATA *ch) {

    if (IS_IMMORTAL(ch))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_DEPUTY))
    return TRUE;

    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (str_cmp((*it)->author, ch->name) && str_cmp((*it)->author, nosr_name(ch->name)))
      continue;

      if ((*it)->crisis == 1)
      return TRUE;
    }

    return FALSE;
  }

  bool has_plot(CHAR_DATA *ch) {
    if (IS_IMMORTAL(ch))
    return TRUE;

    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (!visible_plot(ch, (*it)))
      continue;

      if (!str_cmp(ch->name, (*it)->author))
      return TRUE;
    }
    return FALSE;
  }

  bool has_mystery(CHAR_DATA *ch) {

    if (IS_IMMORTAL(ch))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_DEPUTY))
    return TRUE;

    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (str_cmp((*it)->author, ch->name))
      continue;

      if ((*it)->type == PLOT_MYSTERY)
      return TRUE;
    }

    return FALSE;
  }

  int aobjval(OBJ_DATA *obj) {
    int val = 0;
    val += obj->cost / 20;

    char arg1[MSL];
    char arg2[MSL];

    EXTRA_DESCR_DATA *ed;
    char *buf = str_dup("");

    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+augmentone", ed->keyword)) {
        free_string(buf);
        buf = str_dup(ed->description);
        buf = one_argument_nouncap(buf, arg1);
        buf = one_argument_nouncap(buf, arg2);
        int level = atoi(arg2);
        if (level == 1)
        val += 20000;
        else if (level == 2)
        val += 50000;
        if (level > 2)
        return -1000;
      }
    }
    val = UMIN(val, 50000);
    return val;
  }


  int giftobjval(OBJ_DATA *obj) {
    int val = 0;
    val += obj->cost / 100;

    char arg1[MSL];
    char arg2[MSL];

    EXTRA_DESCR_DATA *ed;
    char *buf = str_dup("");

    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+augmentone", ed->keyword)) {
        free_string(buf);
        buf = str_dup(ed->description);
        buf = one_argument_nouncap(buf, arg1);
        buf = one_argument_nouncap(buf, arg2);
        int level = atoi(arg2);
        if (level == 1 || level == 3)
        val += 8000;
        else if (level == 2 || level == 4)
        val += 75000;
      }
    }
    return val;
  }

  bool crazy_town() {
    if (invasion_one == 1)
    return TRUE;

    if (invasion_two == 1)
    return TRUE;

    if (invasion_three == 1)
    return TRUE;

    if (global_damage_constant == 1)
    return TRUE;

    if (global_damage_erratic == 1)
    return TRUE;

    if (supernatural_darkness == 1)
    return TRUE;

    if (crisis_hurricane == 1)
    return TRUE;

    if (crisis_flood == 1)
    return TRUE;

    return FALSE;
  }

  const char *sin_names[] = {"Pride", "Lust",  "Gluttony", "Greed", "Sloth", "Wrath", "Envy"};

  char *sinlist(CHAR_DATA *ch) {
    char buf[MSL];
    char page[MSL];
    // Setting variable defaults
    strcpy(buf, "");
    strcpy(page, "");

    bool found = FALSE;
    for (int i = 0; i < 7; i++) {
      if (ch->pcdata->sins[i] > 0) {
        if (found) {
          sprintf(buf, ", %s", sin_names[i]);
        }
        else {
          sprintf(buf, "%s", sin_names[i]);
          found = TRUE;
        }
        strcat(page, buf);
      }
    }
    return str_dup(page);
  }

  bool justi_vuln(CHAR_DATA *ch) {
    if (!story_on(ch))
    return FALSE;

    for (int i = 0; i < 7; i++) {
      if (ch->pcdata->sins[i] > 0)
      return TRUE;
    }
    return FALSE;
  }

  bool mali_vuln(CHAR_DATA *ch) {
    if (!story_on(ch))
    return FALSE;

    for (int i = 0; i < 7; i++) {
      if (ch->pcdata->sins[i] > 0)
      return TRUE;
    }
    return FALSE;
  }

  void emplace_spirit(CHAR_DATA *ch, CHAR_DATA *victim, int type) {
    log_string("DESCRIPTOR: Emplacing Spirit");
    save_char_obj(victim, FALSE, FALSE);
    save_account(ch->pcdata->account, FALSE);
    DESCRIPTOR_DATA *dill = ch->desc;
    ACCOUNT_TYPE *acc = ch->pcdata->account;
    extract_char(ch, TRUE);
    dill->character = NULL;
    load_char_obj(dill, victim->name);
    //    dill->character = victim;
    send_to_char("You take possession of the body.\n\r", dill->character);
    dill->character->pcdata->account = acc;
    free_string(dill->character->pcdata->account_name);
    dill->character->pcdata->account_name = str_dup(acc->name);

    if (!IS_FLAG(victim->act, PLR_NOSAVE))
    SET_FLAG(victim->act, PLR_NOSAVE);
    if (!IS_FLAG(victim->act, PLR_GHOST))
    SET_FLAG(victim->act, PLR_GHOST);
    if (IS_FLAG(victim->act, PLR_SINSPIRIT))
    REMOVE_FLAG(victim->act, PLR_SINSPIRIT);
    send_to_char("You feel suddenly displaced from your body.\n\r", victim);
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    for (obj = victim->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (obj->wear_loc == WEAR_NONE)
      extract_obj(obj);
    }
  }

  void remove_spirit(CHAR_DATA *ch, CHAR_DATA *victim, int type) {
    log_string("DESCRIPTOR: Removing Spirit");

    victim->pcdata->spirit_type = 0;

    CHAR_DATA *original;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;
      original = CH(d);
      if (original == victim)
      continue;
      if (original == NULL || IS_NPC(original))
      continue;

      if (!str_cmp(original->name, victim->name)) {
        save_char_obj(victim, FALSE, FALSE);
        save_account(original->pcdata->account, FALSE);
        DESCRIPTOR_DATA *dill = original->desc;
        extract_char(original, TRUE);
        dill->character = NULL;
        char buf[MSL];
        sprintf(buf, "%s", victim->name);
        real_quit(victim);
        load_char_obj(dill, buf);
        free_string(dill->character->pcdata->account_name);
        dill->character->pcdata->account_name = str_dup(dill->account->name);
        send_to_char("With a strange rushing sensation you return to your body.\n\r", dill->character);
        return;
      }
    }
  }

  _DOFUN(do_sin) {
    char arg1[MSL];
    char arg2[MSL];

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (!str_cmp(arg1, "set")) {
      if (!IS_FLAG(ch->comm, COMM_STORY)) {
        send_to_char("You need to have story on to use this feature.\n\r", ch);
        return;
      }

      for (int i = 0; i < 7; i++) {
        if (!str_cmp(arg2, sin_names[i])) {
          if (ch->pcdata->sins[i] == 0) {
            if (sincount(ch) >= 3 && cardinal(ch)) {
              send_to_char("You have too many sins already.\n\r", ch);
              return;
            }
            ch->pcdata->sins[i] = 1;
            printf_to_char(ch, "You become vulnerable to %s.\n\r", sin_names[i]);
            return;
          }
          else {
            ch->pcdata->sins[i] = 0;
            printf_to_char(ch, "You are no longer vulnerable to %s.\n\r", sin_names[i]);
            return;
          }
        }
      }
      send_to_char("No such sin.\n\r", ch);
    }
    else if (!str_cmp(arg1, "Unpossess") && IS_IMMORTAL(ch)) {
      CHAR_DATA *victim = get_char_world_pc(arg2);
      if (victim == NULL || is_gm(victim) || IS_NPC(victim))
      return;
      if (IS_FLAG(victim->act, PLR_SINSPIRIT))
      REMOVE_FLAG(victim->act, PLR_SINSPIRIT);
    }
    else if (!str_cmp(arg1, "maleficarum") && IS_IMMORTAL(ch)) {
      CHAR_DATA *victim = get_char_world_pc(arg2);
      if (victim == NULL || is_gm(victim) || IS_NPC(victim))
      return;
      if (!mali_vuln(victim)) {
        send_to_char("You need a target who's vulnerable to the sins of the justicarum.\n\r", ch);
        return;
      }
      if (IS_FLAG(victim->act, PLR_SINSPIRIT)) {
        act("A stream of black smoke flows out of $n's mouth and dissipates.", victim, NULL, NULL, TO_ROOM);
        act("A stream of black smoke flows out of your mouth and you're yourself again.", victim, NULL, NULL, TO_CHAR);
        if (!IS_FLAG(victim->act, PLR_GUEST))
        real_quit(victim);
        else
        REMOVE_FLAG(victim->act, PLR_SINSPIRIT);
      }
      else {
        act("A stream of black smoke swirls up and flows into $n's mouth.", victim, NULL, NULL, TO_ROOM);
        act("A stream of black smoke swirls up and flows into your mouth.", victim, NULL, NULL, TO_CHAR);
        SET_FLAG(victim->act, PLR_SINSPIRIT);
        victim->pcdata->spirit_type = SPIRIT_MALEFICARUM;
        if (!IS_FLAG(victim->act, PLR_GUEST))
        emplace_spirit(ch, victim, SPIRIT_MALEFICARUM);
      }
      return;
    }
    else if (!str_cmp(arg1, "justicarum") && IS_IMMORTAL(ch)) {
      CHAR_DATA *victim = get_char_world_pc(arg2);
      if (victim == NULL || is_gm(victim) || IS_NPC(victim))
      return;
      if (!justi_vuln(victim)) {
        send_to_char("You need a target who's vulnerable to the sins of the justicarum.\n\r", ch);
        return;
      }
      if (IS_FLAG(victim->act, PLR_SINSPIRIT)) {
        act("A stream of white smoke flows out of $n's mouth and dissipates.", victim, NULL, NULL, TO_ROOM);
        act("A stream of white smoke flows out of your mouth and you're yourself again.", victim, NULL, NULL, TO_CHAR);
        if (!IS_FLAG(victim->act, PLR_GUEST))
        real_quit(victim);
        else
        REMOVE_FLAG(victim->act, PLR_SINSPIRIT);
      }
      else {
        act("A stream of white smoke swirls up and flows into $n's mouth.", victim, NULL, NULL, TO_ROOM);
        act("A stream of white smoke swirls up and flows into your mouth.", victim, NULL, NULL, TO_CHAR);
        SET_FLAG(victim->act, PLR_SINSPIRIT);
        victim->pcdata->spirit_type = SPIRIT_JUSTICARUM;
        if (!IS_FLAG(victim->act, PLR_GUEST))
        emplace_spirit(ch, victim, SPIRIT_JUSTICARUM);
      }
      return;
    }
    else if (!str_cmp(arg1, "attack") && IS_FLAG(ch->act, PLR_SINSPIRIT)) {
      CHAR_DATA *victim;
      victim = get_char_world(ch, arg2);
      if (victim == NULL || IS_NPC(victim) || !story_on(victim)) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }
      if (ch->pcdata->spirit_type == SPIRIT_JUSTICARUM && !justi_vuln(victim)) {
        send_to_char("They aren't vulnerable to that sin.\n\r", ch);
        return;
      }
      if (ch->pcdata->spirit_type == SPIRIT_MALEFICARUM && !justi_vuln(victim)) {
        send_to_char("They aren't vulnerable to that sin.\n\r", ch);
        return;
      }
      if (!IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(ch->act, PLR_SHROUD);
      act("The world suddenly twists violently around you.", victim, NULL, NULL, TO_ROOM);
      act("The world suddenly twists violently around you.", victim, NULL, NULL, TO_CHAR);
      char_from_room(ch);
      char_to_room(ch, victim->in_room);
      start_fight(ch, victim);
      return;
    }
    else if (!str_cmp(arg1, "roll") && IS_IMMORTAL(ch)) {
      if (ch->pcdata->sincool > 0 && str_cmp(ch->name, "Tyr")) {
        send_to_char("You did that too recently.\n\r", ch);
        return;
      }
      int sin = -1;
      for (int i = 0; i < 7; i++) {
        if (!str_cmp(arg2, sin_names[i]))
        sin = i;
      }
      if (sin < 0) {
        send_to_char("No such sin.\n\r", ch);
        return;
      }

      CHAR_DATA *sinners[3] = {NULL};
      for (int i = 0; i < 3; i++)
      sinners[i] = NULL;

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

        if (IS_NPC(victim)) {
          continue;
        }

        if (is_gm(victim)) {
          continue;
        }

        if (victim->in_room == NULL) {
          continue;
        }

        if (!story_on(victim)) {
          continue;
        }

        if (victim->pcdata->sins[sin] > 0) {
          if (sinners[0] == NULL) {
            sinners[0] = victim;
          }
          else if (sinners[1] == NULL) {
            sinners[1] = victim;
          }
          else if (sinners[2] == NULL) {
            sinners[2] = victim;
          }
          else if (number_percent() % 2 == 0) {
            int rand = number_percent() % 3;
            sinners[rand] = victim;
          }
        }
      }
      if (sinners[0] != NULL) {
        printf_to_char(ch, "Sinner %s: %s.\n\r", sinners[0]->name, sinlist(sinners[0]));
      }
      if (sinners[1] != NULL) {
        printf_to_char(ch, "Sinner %s: %s.\n\r", sinners[1]->name, sinlist(sinners[1]));
      }
      if (sinners[2] != NULL) {
        printf_to_char(ch, "Sinner %s: %s.\n\r", sinners[2]->name, sinlist(sinners[2]));
      }
      if (sinners[1] != NULL) {
        ch->pcdata->sincool = 60;
      }
      return;
    }
    else {
      printf_to_char(ch, "Your sins: %s.\n\rSyntax: Sin set (sin)\n\r", sinlist(ch));
      return;
    }
  }

  bool members_in_room(ROOM_INDEX_DATA *room, char *members, char *character, CHAR_DATA *ch) {
    CHAR_DATA *victim;
    printf_to_char(ch, "r: %s, M: %s", room->name, members);

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      printf_to_char(ch, "N: %s", victim->name);

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != room)
      continue;

      if (IS_NPC(victim))
      continue;

      if (is_gm(victim) || IS_FLAG(victim->act, PLR_GUEST))
      continue;
      printf_to_char(ch, "N: %s", victim->name);

      if (safe_strlen(members) > 3 && is_name(victim->name, members))
      return TRUE;
      if (safe_strlen(character) > 3 && is_name(victim->name, character))
      return TRUE;
      if (safe_strlen(members) > 3 && is_name(members, victim->name))
      return TRUE;
      if (safe_strlen(character) > 3 && is_name(character, victim->name))
      return TRUE;
    }

    return FALSE;
  }

  PLOT_TYPE *get_karma_plot(CHAR_DATA *ch) {
    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!visible_plot(ch, (*it)))
      continue;

      if ((*it)->type != PLOT_ADVENTURE && (*it)->type != PLOT_PVP && (*it)->type != PLOT_JOINT && (*it)->type != PLOT_QUEST && (*it)->type != PLOT_MYSTERY && (*it)->finale == 0)
      continue;

      if (str_cmp(ch->name, (*it)->author))
      continue;

      if ((*it)->type != PLOT_OTHER && (*it)->type != PLOT_MYSTERY) {
        if (ch->in_room->area->vnum != 12)
        continue;
      }

      /*
if((*it)->type == PLOT_QUEST)
{
if(!members_in_room(ch->in_room, (*it)->members, (*it)->character, ch));
continue;
}
*/
      if ((*it)->karmacap > 0 && (*it)->earnedkarma >= (*it)->karmacap)
      continue;

      return (*it);
    }
    return NULL;
  }

  void start_survey(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch->pcdata->survey_stage > 0)
    return;
    printf_to_char(
    ch, "You've been offered the chance to complete a small feedback form on the RP of %s, for which you'll receive some small rewards, if you'd like to complete this feedback, please type 'begin'", PERS(victim, ch));
    ch->pcdata->survey_stage = 1;
    free_string(ch->pcdata->surveying);
    ch->pcdata->surveying = str_dup(victim->name);
    ch->pcdata->survey_delay = current_time + (3600 * 24 * number_range(6, 10));
  }

  _DOFUN(do_begin) {
    if (ch->pcdata->survey_stage != 1)
    return;
    ch->pcdata->survey_stage = 2;
    send_to_char("Thank you for deciding to take part. Firstly please enter a number between -10 and 10 that rates how you believe the character you're reviewing contributes to the enjoyment of the game on average. With -10 being they are detrimental to enjoyment to the maximum extent possible, 10 that they contribute to enjoyment to the maximum extent possible, and 0 that they neither contribute to the enjoyment of most scenes, nor are detrimental to them.\n\r", ch);
  }

  void process_survey_number(CHAR_DATA *ch, char *argument) {
    int val = atoi(argument);
    if (ch->pcdata->survey_stage == 2) {
      if (val > 10 || val < -10) {
        send_to_char("Please enter a number between -10 and 10.\n\r", ch);
        return;
      }
      ch->pcdata->survey_ratings[SURVEY_FUN] = val;
      ch->pcdata->survey_stage = 3;
      send_to_char("Thank you. Now please enter a number between -10 and 10 that rates how you believe the character you're reviewing contributes to the immersion of the game world on average. The ability for people to believe that it could be a real place populated by real people and to feel ICly immersed in it. With -10 being that they are detrimental to immersion to the maximum extent possible, 10 that they contribute to immersion to the maximum extent possible, and 0 that they neither contribute to, nor are detrimental to the immersion of most scenes.\n\r", ch);
    }
    else if (ch->pcdata->survey_stage == 3) {
      if (val > 10 || val < -10) {
        send_to_char("Please enter a number between -10 and 10.\n\r", ch);
        return;
      }
      ch->pcdata->survey_ratings[SURVEY_REAL] = val;
      ch->pcdata->survey_stage = 4;
      send_to_char("Thank you. Now please enter a number between -10 and 10 to rate how well you believe the person writes on average. How well they handle clothing, descriptions, emotes, how well they are at pacing and speed of their RP and everything else related. With -10 meaning that you find their writing poor to the maximum extent possible, where it detriments the RP as much as possible through being slow or unclear. 10 being that their writing is excellent to the maximum extent possible, well paced and written as to add to interest and enjoyment, and 0 being that their writing neither adds to or is detrimental to most scenes.\n\r", ch);
    }
    else if (ch->pcdata->survey_stage == 4) {
      if (val > 10 || val < -10) {
        send_to_char("Please enter a number between -10 and 10.\n\r", ch);
        return;
      }
      ch->pcdata->survey_ratings[SURVEY_WRITE] = val;
      ch->pcdata->survey_stage = 5;
      send_to_char("Thank you. Now please enter a number or several numbers corresponding to the roles in RP you believe someone takes on with the character and does to a good standard. When you are done with this section, please type 'done'\n\r", ch);
      send_to_char("[1] Roleplay Creator\n\r", ch);
      send_to_char("(A character who creates RP, such as through schemes, calendar events, quests, carrying conversations or coming up with/leading other IC occurrences.)\n\r", ch);
      send_to_char("[2] Antagonist\n\r", ch);
      send_to_char("(A character who sets themselves up in active opposition to other characters, but does so with regard to the enjoyment of those they oppose.)\n\r", ch);
      send_to_char("[3] Sidekick/Support.\n\r", ch);
      send_to_char("(A character who doesn't take up much of the spotlight, but aids the story from the side, filling some specific role in a scene besides that of being the big hero, villain or center of the story.)\n\r", ch);
      send_to_char("[4] Background/Environmental character.\n\r", ch);
      send_to_char("(A character which fills in underplayed roles which should be common, helping to add to the feeling of being surrounded by an environment of real, more normal characters.\n\r", ch);
      send_to_char("[5] Comedic relief.\n\r", ch);
      send_to_char("(A character who is able to entertain others with humor or other light roleplay, yet without breaking the fourth wall, being fearless or otherwise inappropriate.)\n\r", ch);
      send_to_char("[6] Graceful victim.\n\r", ch);
      send_to_char("(A character who is available to be victimized, yet handles it with grace. RPing well, staying OOCly unagitated, behaving realistically, and making things enjoyable RP wise for their persecutor.)\n\r", ch);
    }
    else if (ch->pcdata->survey_stage == 5) {
      if (val > 6 || val < 1) {
        send_to_char("Please enter a number between 1 and 6.\n\r", ch);
        return;
      }
      if (val == 1) {
        if (ch->pcdata->survey_ratings[SURVEY_CREATE] == 0) {
          ch->pcdata->survey_ratings[SURVEY_CREATE] = 1;
          send_to_char("You designate them as a Roleplay creator, enter more numbers for other roles, or type 'done' to move on.\n\r", ch);
        }
        else {
          ch->pcdata->survey_ratings[SURVEY_CREATE] = 0;
          send_to_char("You remove the designation as a roleplay creator.\n\r", ch);
        }
      }
      if (val == 2) {
        if (ch->pcdata->survey_ratings[SURVEY_ANTAG] == 0) {
          ch->pcdata->survey_ratings[SURVEY_ANTAG] = 1;
          send_to_char("You designate them as an Antagonist, enter more numbers for other roles, or type 'done' to move on.\n\r", ch);
        }
        else {
          ch->pcdata->survey_ratings[SURVEY_ANTAG] = 0;
          send_to_char("You remove the designation as an antagonist.\n\r", ch);
        }
      }
      if (val == 3) {
        if (ch->pcdata->survey_ratings[SURVEY_SIDE] == 0) {
          ch->pcdata->survey_ratings[SURVEY_SIDE] = 1;
          send_to_char("You designate them as a Sidekick/Support, enter more numbers for other roles, or type 'done' to move on.\n\r", ch);
        }
        else {
          ch->pcdata->survey_ratings[SURVEY_SIDE] = 0;
          send_to_char("You remove the designation as a Sidekick/Support.\n\r", ch);
        }
      }
      if (val == 4) {
        if (ch->pcdata->survey_ratings[SURVEY_BACK] == 0) {
          ch->pcdata->survey_ratings[SURVEY_BACK] = 1;
          send_to_char("You designate them as a Background/Environmental character, enter more numbers for other roles, or type 'done' to move on.\n\r", ch);
        }
        else {
          ch->pcdata->survey_ratings[SURVEY_BACK] = 0;
          send_to_char("You remove the designation as a Background/Environmental.\n\r", ch);
        }
      }
      if (val == 5) {
        if (ch->pcdata->survey_ratings[SURVEY_COMEDY] == 0) {
          ch->pcdata->survey_ratings[SURVEY_COMEDY] = 1;
          send_to_char("You designate them as comedic relief, enter more numbers for other roles, or type 'done' to move on.\n\r", ch);
        }
        else {
          ch->pcdata->survey_ratings[SURVEY_COMEDY] = 0;
          send_to_char("You remove the designation as comedic relief.\n\r", ch);
        }
      }
      if (val == 6) {
        if (ch->pcdata->survey_ratings[SURVEY_VICTIM] == 0) {
          ch->pcdata->survey_ratings[SURVEY_VICTIM] = 1;
          send_to_char("You designate them as a Graceful Victim, enter more numbers for other roles, or type 'done' to move on.\n\r", ch);
        }
        else {
          ch->pcdata->survey_ratings[SURVEY_VICTIM] = 0;
          send_to_char("You remove the designation as a graceful victim.\n\r", ch);
        }
      }
    }
  }

  _DOFUN(do_comment) {
    if (ch->pcdata->survey_stage == 7) {
      string_append(ch, &ch->pcdata->survey_comment);
    }
  }

  _DOFUN(do_recommend) {
    if (ch->pcdata->survey_stage == 6) {
      string_append(ch, &ch->pcdata->survey_improve);
    }
  }
  bool critical_survey(CHAR_DATA *ch) {
    if (ch->pcdata->survey_ratings[SURVEY_REAL] <= 0 || ch->pcdata->survey_ratings[SURVEY_WRITE] <= 0 || ch->pcdata->survey_ratings[SURVEY_FUN] <= 0) {
      if (safe_strlen(ch->pcdata->survey_comment) > 5 || safe_strlen(ch->pcdata->survey_improve) > 5)
      return TRUE;
    }
    return FALSE;
  }

  bool silly_survey(CHAR_DATA *ch) {
    if (ch->pcdata->survey_ratings[SURVEY_REAL] +
        ch->pcdata->survey_ratings[SURVEY_FUN] +
        ch->pcdata->survey_ratings[SURVEY_WRITE] >
        6) {
      if (ch->pcdata->survey_ratings[SURVEY_CREATE] < 1 && ch->pcdata->survey_ratings[SURVEY_ANTAG] < 1 && ch->pcdata->survey_ratings[SURVEY_SIDE] < 1 && ch->pcdata->survey_ratings[SURVEY_BACK] < 1 && ch->pcdata->survey_ratings[SURVEY_COMEDY] < 1 && ch->pcdata->survey_ratings[SURVEY_VICTIM] < 1) {
        if (safe_strlen(ch->pcdata->survey_comment) < 10 && safe_strlen(ch->pcdata->survey_improve) < 10)
        return TRUE;
      }
    }
    if (ch->pcdata->survey_ratings[SURVEY_REAL] >= 8 && ch->pcdata->survey_ratings[SURVEY_FUN] >= 8 && ch->pcdata->survey_ratings[SURVEY_WRITE] >= 8)
    return TRUE;

    if (ch->pcdata->survey_ratings[SURVEY_REAL] == 10 && ch->pcdata->survey_ratings[SURVEY_FUN] == 10 && ch->pcdata->survey_ratings[SURVEY_WRITE] == 10)
    return TRUE;
    if (ch->pcdata->survey_ratings[SURVEY_REAL] == -10 && ch->pcdata->survey_ratings[SURVEY_FUN] == -10 && ch->pcdata->survey_ratings[SURVEY_WRITE] == -10)
    return TRUE;
    if (ch->pcdata->survey_ratings[SURVEY_REAL] == 0 && ch->pcdata->survey_ratings[SURVEY_FUN] == 0 && ch->pcdata->survey_ratings[SURVEY_WRITE] == 0)
    return TRUE;
    if (ch->pcdata->survey_ratings[SURVEY_REAL] ==
        ch->pcdata->survey_ratings[SURVEY_FUN] && ch->pcdata->survey_ratings[SURVEY_REAL] ==
        ch->pcdata->survey_ratings[SURVEY_WRITE])
    return TRUE;

    return FALSE;
  }

  void send_survey(CHAR_DATA *ch, char *name) {
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';
    sprintf(buf, "\nSomeone has completed an anonymous roleplay feedback form for your character.\n\rBetween -10 and 10 you have been rated %d on how much you contribute to the enjoyment of RP on average, %d on how much you contribute to the immersion of RP on average, and %d on how well your writing skills contribute to RP.\n", ch->pcdata->survey_ratings[SURVEY_FUN], ch->pcdata->survey_ratings[SURVEY_REAL], ch->pcdata->survey_ratings[SURVEY_WRITE]);
    strcat(string, buf);

    if (ch->pcdata->survey_ratings[SURVEY_CREATE] > 0) {
      sprintf(buf, "They think you do a good job as a roleplay creator.\n\r");
      strcat(string, buf);
    }
    if (ch->pcdata->survey_ratings[SURVEY_ANTAG] > 0) {
      sprintf(buf, "They think you do a good job as an antagonist\n\r");
      strcat(string, buf);
    }
    if (ch->pcdata->survey_ratings[SURVEY_SIDE] > 0) {
      sprintf(
      buf, "They think you do a good job as a Sidekick/Support character.\n\r");
      strcat(string, buf);
    }
    if (ch->pcdata->survey_ratings[SURVEY_BACK] > 0) {
      sprintf(buf, "They think you do a good job as a background/environmental character.\n\r");
      strcat(string, buf);
    }
    if (ch->pcdata->survey_ratings[SURVEY_COMEDY] > 0) {
      sprintf(buf, "They think you do a good job as a comedic relief character.\n\r");
      strcat(string, buf);
    }
    if (ch->pcdata->survey_ratings[SURVEY_VICTIM] > 0) {
      sprintf(buf, "They think you do a good job as a graceful victim.\n\r");
      strcat(string, buf);
    }
    if (safe_strlen(ch->pcdata->survey_improve) > 3) {
      sprintf(buf, "They had this recommendation to make on how you could improve: %s", ch->pcdata->survey_improve);
      strcat(string, buf);
    }
    if (safe_strlen(ch->pcdata->survey_comment) > 3) {
      sprintf(buf, "They also had this to say: %s", ch->pcdata->survey_comment);
      strcat(string, buf);
    }
    if (get_char_world_pc(ch->pcdata->surveying) != NULL) {
      page_to_char(string, get_char_world_pc(ch->pcdata->surveying));
    }
    else
    message_to_char(ch->pcdata->surveying, string);

    sprintf(buf, "FEEDBACK: %s reviews %s: %s", ch->name, ch->pcdata->surveying, string);
    log_string(buf);
    wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);
  }

  void survey_update(CHAR_DATA *ch) {

    NameMap::const_iterator nit;

    if (ch->pcdata->survey_delay > current_time)
    return;
    if (ch->pcdata->survey_stage > 0)
    return;

    if (IS_FLAG(ch->act, PLR_DEAD))
    return;

    if (ch->played / 3600 < 50)
    return;

    if (is_gm(ch))
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD))
    return;

    if (number_percent() % 8 != 0) {
      ch->pcdata->survey_delay = current_time + (3600 * (number_range(1, 4)));
      return;
    }
    if(ch->pcdata->account == NULL)
    return;
    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOFEEDBACK))
    return;

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

        if (in_fight(to) || is_masked(to) || is_cloaked(to))
        continue;

        if (is_gm(to))
        continue;

        if (!can_see(ch, to))
        continue;

        if (IS_FLAG(to->act, PLR_SHROUD))
        continue;

        if ((nit = ch->pcdata->female_names->find(to->id)) !=
            ch->pcdata->female_names->end() && number_percent() % 13 == 0) {
          start_survey(ch, to);
          return;
        }
        if ((nit = ch->pcdata->male_names->find(to->id)) !=
            ch->pcdata->male_names->end() && number_percent() % 13 == 0) {
          start_survey(ch, to);
          return;
        }
      }
    }
  }

  bool can_encounter(CHAR_DATA *ch, int number) {
    if (IS_NPC(ch) || is_gm(ch) || is_helpless(ch) || IS_FLAG(ch->comm, COMM_AFK) || in_fight(ch) || IS_FLAG(ch->act, PLR_DEAD) || ch->in_room == NULL || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room) || ch->pcdata->availability == AVAIL_LOW || IS_FLAG(ch->act, PLR_SHROUD)) {
      return FALSE;
    }
    if (higher_power(ch))
    return FALSE;

    switch (number) {
    case ENCOUNTER_ONE:
    case ENCOUNTER_TWO:
    case ENCOUNTER_EIGHT:
      if (ch->faction == 0 && pc_pop(ch->in_room) < 5)
      return FALSE;
      return TRUE;
      break;
    case ENCOUNTER_THREE:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 2 && is_dark_outside())
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FOUR:
    case ENCOUNTER_FIVE:
      if (!public_room(ch->in_room) && pc_pop(ch->in_room) < 5)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_SIX:
    case ENCOUNTER_SEVEN:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_NINE:
    case ENCOUNTER_TEN:
    case ENCOUNTER_ELEVEN:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 4)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWELVE:
      if (ch->in_room->sector_type == SECT_STREET)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTEEN:
      if (ch->in_room->sector_type == SECT_STREET || ch->in_room->sector_type == SECT_PARK || ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_ROCKY) {
        if (pc_pop(ch->in_room) < 3 && !IS_FLAG(ch->act, PLR_SHROUD))
        return TRUE;
      }
      return FALSE;
      break;
    case ENCOUNTER_FOURTEEN:
      if (ch->faction != 0 && pc_pop(ch->in_room) < 4)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FIFTEEN:
    case ENCOUNTER_SIXTEEN:
      if (ch->faction != 0 && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && pc_pop(ch->in_room) < 3)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_SEVENTEEN:
      if (get_attract(ch, NULL) >= 80)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_EIGHTEEN:
      if (!seems_super(ch) && public_room(ch->in_room) && is_dark_outside())
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_NINETEEN:
      if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && full_moon() == TRUE && pc_pop(ch->in_room) < 3)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTY:
      if (public_room(ch->in_room) && full_moon() == TRUE && pc_pop(ch->in_room) < 5)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYONE:
      if (public_room(ch->in_room) && moon_phase() == 5 && pc_pop(ch->in_room) < 3)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYTWO:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 4 && is_dark_outside())
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYTHREE:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 3)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYFOUR:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYFIVE:
      if (can_shroud(ch) && (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS) || public_room(ch->in_room)))
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYSIX:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYSEVEN:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_TWENTYEIGHT:
    case ENCOUNTER_TWENTYNINE:
    case ENCOUNTER_THIRTY:
      if (seems_super(ch) && pc_pop(ch->in_room) < 2 && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTYONE:
      if (get_attract(ch, NULL) >= 80 && pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTYTWO:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 2 && ch->faction != 0)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTYTHREE:
      if (pc_pop(ch->in_room) < 2 && get_skill(ch, SKILL_STRENGTH) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTYFOUR:
      if (ch->faction != 0 && pc_pop(ch->in_room) < 5)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTYFIVE:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_THIRTYSIX:
    case ENCOUNTER_THIRTYSEVEN:
    case ENCOUNTER_THIRTYEIGHT:
    case ENCOUNTER_THIRTYNINE:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 3 && ch->faction != 0)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTY:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYONE:
      if (public_room(ch->in_room) && ch->faction != 0 && pc_pop(ch->in_room) < 5)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYTWO:
      if (nearby_water(ch) && pc_pop(ch->in_room) < 4)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYTHREE:
      if (pc_pop(ch->in_room) < 2)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYFOUR:
      if (nearby_water(ch) && pc_pop(ch->in_room) < 4 && get_attract(ch, NULL) >= 80)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYFIVE:
      if (pc_pop(ch->in_room) < 2 && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYSIX:
      if (pc_pop(ch->in_room) < 2 && ch->faction != 0)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYSEVEN:
    case ENCOUNTER_FORTYEIGHT:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 4)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FORTYNINE:
    case ENCOUNTER_FIFTY:
      if (public_room(ch->in_room) && pc_pop(ch->in_room) < 3 && ch->faction != 0)
      return TRUE;
      return FALSE;
      break;
    case ENCOUNTER_FIFTYONE:
    case ENCOUNTER_FIFTYTWO:
      if (pc_pop(ch->in_room) < 2 && ch->faction != 0)
      return TRUE;
      return FALSE;
      break;
    }
    return FALSE;
  }

  int encounter_probability(int number) {
    switch (number) {
    case ENCOUNTER_ONE:
      return 60;
      break;
    case ENCOUNTER_TWO:
      return 30;
      break;
    case ENCOUNTER_THREE:
      return 50;
      break;
    case ENCOUNTER_FOUR:
      return 40;
      break;
    case ENCOUNTER_FIVE:
      return 40;
      break;
    case ENCOUNTER_SIX:
      return 10;
      break;
    case ENCOUNTER_SEVEN:
      return 10;
      break;
    case ENCOUNTER_EIGHT:
      return 40;
      break;
    case ENCOUNTER_NINE:
    case ENCOUNTER_TEN:
      return 25;
      break;
    case ENCOUNTER_ELEVEN:
      return 20;
      break;
    case ENCOUNTER_TWELVE:
      return 80;
      break;
    case ENCOUNTER_THIRTEEN:
      return 75;
      break;
    case ENCOUNTER_FOURTEEN:
      return 60;
      break;
    case ENCOUNTER_FIFTEEN:
      return 40;
      break;
    case ENCOUNTER_SIXTEEN:
      return 65;
      break;
    case ENCOUNTER_SEVENTEEN:
      return 20;
      break;
    case ENCOUNTER_EIGHTEEN:
      return 90;
      break;
    case ENCOUNTER_NINETEEN:
      return 100;
      break;
    case ENCOUNTER_TWENTY:
      return 80;
      break;
    case ENCOUNTER_TWENTYONE:
      return 50;
      break;
    case ENCOUNTER_TWENTYTWO:
      return 20;
      break;
    case ENCOUNTER_TWENTYTHREE:
      return 10;
      break;
    case ENCOUNTER_TWENTYFOUR:
      return 20;
      break;
    case ENCOUNTER_TWENTYFIVE:
      return 40;
      break;
    case ENCOUNTER_TWENTYSIX:
      return 40;
      break;
    case ENCOUNTER_TWENTYSEVEN:
      return 60;
      break;
    case ENCOUNTER_TWENTYEIGHT:
      return 50;
      break;
    case ENCOUNTER_TWENTYNINE:
      return 30;
      break;
    case ENCOUNTER_THIRTY:
      return 50;
      break;
    case ENCOUNTER_THIRTYONE:
      return 10;
      break;
    case ENCOUNTER_THIRTYTWO:
      return 20;
      break;
    case ENCOUNTER_THIRTYTHREE:
      return 60;
      break;
    case ENCOUNTER_THIRTYFOUR:
      return 20;
      break;
    case ENCOUNTER_THIRTYFIVE:
      return 20;
      break;
    case ENCOUNTER_THIRTYSIX:
      return 20;
      break;
    case ENCOUNTER_THIRTYSEVEN:
    case ENCOUNTER_THIRTYEIGHT:
    case ENCOUNTER_THIRTYNINE:
      return 60;
      break;
    case ENCOUNTER_FORTY:
      return 30;
      break;
    case ENCOUNTER_FORTYONE:
      return 20;
      break;
    case ENCOUNTER_FORTYTWO:
      return 70;
      break;
    case ENCOUNTER_FORTYTHREE:
      return 15;
      break;
    case ENCOUNTER_FORTYFOUR:
      return 50;
      break;
    case ENCOUNTER_FORTYFIVE:
      return 30;
      break;
    case ENCOUNTER_FORTYSIX:
      return 20;
      break;
    case ENCOUNTER_FORTYSEVEN:
      return 10;
      break;
    case ENCOUNTER_FORTYEIGHT:
      return 10;
      break;
    case ENCOUNTER_FORTYNINE:
      return 20;
      break;
    case ENCOUNTER_FIFTY:
      return 30;
      break;
    case ENCOUNTER_FIFTYONE:
    case ENCOUNTER_FIFTYTWO:
      return 20;
      break;
    }
    return 30;
  }
  int autoallyscore(CHAR_DATA *ally, CHAR_DATA *target, bool super, bool faction, bool proximity) {
    int score;
    int prox;
    ROOM_INDEX_DATA *orig_room =
    get_room_index(target->pcdata->encounter_orig_room);
    if (orig_room != NULL) {
      prox = get_dist3d(get_roomx(ally->in_room), get_roomy(ally->in_room), get_roomz(ally->in_room), get_roomx(orig_room), get_roomy(orig_room), get_roomz(orig_room));
    }
    else
    prox = 25;

    if (proximity == TRUE && orig_room != ally->in_room)
    return -1200;
    score = 0;
    if (faction == TRUE) {
      if(ally->fcore == target->fcore && ally->fcore != 0)
      score = score + 1000;
      else if(ally->fcore != target->fcore)
      score = score - 600;
      if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance == clan_lookup(target->fcult)->alliance)
      score = score + 500;
      else if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance != clan_lookup(target->fcult)->alliance)
      score = score - 250;
      if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance == clan_lookup(target->fsect)->alliance)
      score = score + 500;
      else if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance != clan_lookup(target->fsect)->alliance)
      score = score - 250;

      if (proximity == TRUE) {
        score -= prox * 15;
      }
      else
      score -= prox * 3;

      return score;
    }
    if (super == TRUE) {
      if (!seems_super(ally) && ally->faction == 0) {
        if (safe_strlen(ally->pcdata->pledge) < 2)
        return -1000;
        score = 300;
      }
      else
      score = 800;

      if (proximity == TRUE) {
        score -= prox * 20;
      }
      else
      score -= prox * 3;

      return score;
    }
    if (proximity == TRUE) {
      score = 1000 - prox * 20;

      return score;
    }

    return 0;
  }
  int allyscore(CHAR_DATA *ally, CHAR_DATA *target, bool proximate, bool super, bool samefac, bool concern) {
    int score;
    int prox;
    ROOM_INDEX_DATA *orig_room =
    get_room_index(target->pcdata->encounter_orig_room);
    if (orig_room != NULL) {
      prox = get_dist3d(get_roomx(ally->in_room), get_roomy(ally->in_room), get_roomz(ally->in_room), get_roomx(orig_room), get_roomy(orig_room), get_roomz(orig_room));
    }
    else
    prox = 25;
    score = 0;
    if (samefac == TRUE) {
      if(ally->fcore == target->fcore && ally->fcore != 0)
      score = score + 1000;
      else if(ally->fcore != target->fcore)
      score = score - 600;
      if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance == clan_lookup(target->fcult)->alliance)
      score = score + 500;
      else if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance != clan_lookup(target->fcult)->alliance)
      score = score - 250;
      if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance == clan_lookup(target->fsect)->alliance)
      score = score + 500;
      else if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance != clan_lookup(target->fsect)->alliance)
      score = score - 250;

      if(score < 0)
      return -1000;
      if (proximate == TRUE) {
        score -= prox * 10;
      }
      else
      score -= prox * 3;

      if (concern == TRUE) {
        if (!str_cmp(target->pcdata->pledge, ally->name))
        score += 500;

        for (int i = 0; i < 10; i++) {
          if (!strcmp(ally->pcdata->relationship[i], target->name))
          score += 50;
        }
      }

      return score;
    }
    if (super == TRUE) {
      if (!seems_super(ally) && ally->faction == 0) {
        if (safe_strlen(ally->pcdata->pledge) < 2)
        return -1000;
        score = 300;
      }
      else
      score = 800;

      if (proximate == TRUE) {
        score -= prox * 10;
      }
      else
      score -= prox * 3;

      if (concern == TRUE) {
        if (!str_cmp(target->pcdata->pledge, ally->name))
        score += 500;

        for (int i = 0; i < 10; i++) {
          if (!strcmp(ally->pcdata->relationship[i], target->name))
          score += 100;
        }

        if(ally->fcore == target->fcore && ally->fcore != 0)
        score = score + 500;
        else if(ally->fcore != target->fcore)
        score = score - 300;
        if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance == clan_lookup(target->fcult)->alliance)
        score = score + 250;
        else if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance != clan_lookup(target->fcult)->alliance)
        score = score - 100;
        if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance == clan_lookup(target->fsect)->alliance)
        score = score + 250;
        else if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance != clan_lookup(target->fsect)->alliance)
        score = score - 100;
      }

      return score;
    }

    if (proximate == TRUE) {
      score = 1000 - prox * 20;

      if (concern == TRUE) {
        if (!str_cmp(target->pcdata->pledge, ally->name))
        score += 500;

        for (int i = 0; i < 10; i++) {
          if (!strcmp(ally->pcdata->relationship[i], target->name))
          score += 50;
        }
        if(ally->fcore == target->fcore && ally->fcore != 0)
        score = score + 200;
        else if(ally->fcore != target->fcore)
        score = score - 100;
        if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance == clan_lookup(target->fcult)->alliance)
        score = score + 100;
        else if(clan_lookup(ally->fcult) != NULL && clan_lookup(target->fcult) != NULL && clan_lookup(ally->fcult)->alliance != clan_lookup(target->fcult)->alliance)
        score = score - 50;
        if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance == clan_lookup(target->fsect)->alliance)
        score = score + 100;
        else if(clan_lookup(ally->fsect) != NULL && clan_lookup(target->fsect) != NULL && clan_lookup(ally->fsect)->alliance != clan_lookup(target->fsect)->alliance)
        score = score - 50;

        return score;
      }
    }
    return 0;
  }

  void pullin_autoallies(CHAR_DATA *storyrunner, CHAR_DATA *target, bool super, bool faction, bool proximity, int limit) {
    int pop;
    if (target != NULL)
    pop = pc_pop(target->in_room);
    else if (storyrunner != NULL)
    pop = pc_pop(storyrunner->in_room);
    else
    return;
    int max;
    CHAR_DATA *ally = NULL;
    for (int attempts = 0; pop < limit && attempts < 20; attempts++) {
      max = -1000;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *victim;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          victim = d->character;
          if (IS_NPC(victim))
          continue;
          if (victim->in_room == NULL)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_REJECTED)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_ONGOING)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_PENDING)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_PENDINGALLY)
          continue;
          if (victim->in_room == target->in_room)
          continue;
          if (is_gm(victim) || IS_FLAG(victim->act, PLR_GUEST))
          continue;

          if (same_player(storyrunner, victim) || same_player(target, victim))
          continue;

          if (get_gmtrust(storyrunner, victim) < 0)
          continue;

          if (IS_NPC(victim) || is_gm(victim) || is_helpless(victim) || IS_FLAG(victim->comm, COMM_AFK) || in_fight(victim) || IS_FLAG(victim->act, PLR_DEAD) || victim->in_room == NULL || locked_room(victim->in_room, victim) || !in_haven(victim->in_room) || room_hostile(victim->in_room) || IS_FLAG(victim->act, PLR_SHROUD))
          continue;

          if (autoallyscore(victim, target, super, faction, proximity) > max) {
            ally = victim;
            max = autoallyscore(victim, target, super, faction, proximity);
          }
        }
      }
      if (ally != NULL) {
        pop++;
        offer_ally(storyrunner, target, ally);
        ally = NULL;
      }
    }
  }
  // either going to be people in the room, or faction buddies.
  void recruit_autoallies(CHAR_DATA *storyrunner, CHAR_DATA *target, int number) {
    switch (number) {
    case ENCOUNTER_ONE:
      pullin_autoallies(storyrunner, target, TRUE, TRUE, FALSE, 5);
      break;
    case ENCOUNTER_TWO:
      pullin_autoallies(storyrunner, target, TRUE, TRUE, FALSE, 5);
      break;
    case ENCOUNTER_THREE:
      break;
    case ENCOUNTER_FOUR:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_FIVE:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_SIX:
      break;
    case ENCOUNTER_SEVEN:
      break;
    case ENCOUNTER_EIGHT:
      pullin_autoallies(storyrunner, target, TRUE, TRUE, FALSE, 5);
      break;
    case ENCOUNTER_NINE:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_TEN:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_ELEVEN:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_TWELVE:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_THIRTEEN:
      break;
    case ENCOUNTER_FOURTEEN:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_FIFTEEN:
      break;
    case ENCOUNTER_SIXTEEN:
      break;
    case ENCOUNTER_SEVENTEEN:
      break;
    case ENCOUNTER_EIGHTEEN:
      break;
    case ENCOUNTER_NINETEEN:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_TWENTY:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_TWENTYONE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_TWENTYTWO:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_TWENTYTHREE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_TWENTYFOUR:
      break;
    case ENCOUNTER_TWENTYFIVE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_TWENTYSIX:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_TWENTYSEVEN:
      break;
    case ENCOUNTER_TWENTYEIGHT:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_TWENTYNINE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_THIRTY:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_THIRTYONE:
      break;
    case ENCOUNTER_THIRTYTWO:
      pullin_autoallies(storyrunner, target, FALSE, TRUE, TRUE, 5);
      break;
    case ENCOUNTER_THIRTYTHREE:
      break;
    case ENCOUNTER_THIRTYFOUR:
      pullin_autoallies(storyrunner, target, FALSE, TRUE, FALSE, 5);
      break;
    case ENCOUNTER_THIRTYFIVE:
      break;
    case ENCOUNTER_THIRTYSIX:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_THIRTYSEVEN:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_THIRTYEIGHT:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_THIRTYNINE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_FORTY:
      break;
    case ENCOUNTER_FORTYONE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_FORTYTWO:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_FORTYTHREE:
      break;
    case ENCOUNTER_FORTYFOUR:
      break;
    case ENCOUNTER_FORTYFIVE:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 2);
      break;
    case ENCOUNTER_FORTYSIX:
      break;
    case ENCOUNTER_FORTYSEVEN:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_FORTYEIGHT:
      pullin_autoallies(storyrunner, target, FALSE, FALSE, TRUE, 3);
      break;
    case ENCOUNTER_FORTYNINE:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_FIFTY:
      pullin_autoallies(storyrunner, target, TRUE, FALSE, TRUE, 5);
      break;
    case ENCOUNTER_FIFTYONE:
      pullin_autoallies(storyrunner, target, TRUE, TRUE, FALSE, 5);
      break;
    case ENCOUNTER_FIFTYTWO:
      pullin_autoallies(storyrunner, target, TRUE, TRUE, FALSE, 5);
      break;
    }
  }

  void pullin_allies(CHAR_DATA *storyrunner, CHAR_DATA *target, bool proximate, bool super, bool samefac, bool concern) {
    int pop;
    if (target != NULL)
    pop = pc_pop(target->in_room);
    else if (storyrunner != NULL)
    pop = pc_pop(storyrunner->in_room);
    else
    return;

    int max;
    CHAR_DATA *ally = NULL;
    for (int attempts = 0; pop < 7 && attempts < 40; attempts++) {
      max = -1000;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *victim;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          victim = d->character;
          if (IS_NPC(victim))
          continue;
          if (victim->in_room == NULL)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_ONGOING)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_PENDING)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_PENDINGALLY)
          continue;
          if (is_gm(victim) || IS_FLAG(victim->act, PLR_GUEST))
          continue;
          if (victim->in_room == target->in_room)
          continue;
          if (same_player(storyrunner, victim) || same_player(target, victim))
          continue;
          if (get_gmtrust(storyrunner, victim) < 0)
          continue;

          if (IS_NPC(victim) || is_gm(victim) || is_helpless(victim) || IS_FLAG(victim->comm, COMM_AFK) || in_fight(victim) || IS_FLAG(victim->act, PLR_DEAD) || victim->in_room == NULL || locked_room(victim->in_room, victim) || !in_haven(victim->in_room) || room_hostile(victim->in_room) || IS_FLAG(victim->act, PLR_SHROUD))
          continue;

          if (allyscore(victim, target, proximate, super, samefac, concern) >
              max) {
            ally = victim;
            max = allyscore(victim, target, proximate, super, samefac, concern);
          }
        }
      }
      if (ally != NULL) {
        pop++;
        offer_ally(storyrunner, target, ally);
        ally = NULL;
      }
    }
  }
  void recruitallies(CHAR_DATA *storyrunner, CHAR_DATA *target, int number) {
    switch (number) {
    case ENCOUNTER_ONE:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, FALSE);
      break;
    case ENCOUNTER_TWO:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, FALSE);
      break;
    case ENCOUNTER_THREE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FOUR:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FIVE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_SIX:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_SEVEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_EIGHT:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, FALSE);
      break;
    case ENCOUNTER_NINE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_TEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_ELEVEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_TWELVE:
      pullin_allies(storyrunner, target, TRUE, FALSE, FALSE, FALSE);
      break;
    case ENCOUNTER_THIRTEEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FOURTEEN:
      pullin_allies(storyrunner, target, TRUE, FALSE, FALSE, FALSE);
      break;
    case ENCOUNTER_FIFTEEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, TRUE);
      break;
    case ENCOUNTER_SIXTEEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, TRUE);
      break;
    case ENCOUNTER_SEVENTEEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_EIGHTEEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_NINETEEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_TWENTY:
      pullin_allies(storyrunner, target, TRUE, FALSE, FALSE, TRUE);
      break;
    case ENCOUNTER_TWENTYONE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_TWENTYTWO:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_TWENTYTHREE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_TWENTYFOUR:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_TWENTYFIVE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_TWENTYSIX:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_TWENTYSEVEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, TRUE);
      break;
    case ENCOUNTER_TWENTYEIGHT:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_TWENTYNINE:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, TRUE);
      break;
    case ENCOUNTER_THIRTY:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_THIRTYONE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_THIRTYTWO:
      pullin_allies(storyrunner, target, TRUE, TRUE, TRUE, FALSE);
      break;
    case ENCOUNTER_THIRTYTHREE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_THIRTYFOUR:
      pullin_allies(storyrunner, target, FALSE, TRUE, TRUE, FALSE);
      break;
    case ENCOUNTER_THIRTYFIVE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_THIRTYSIX:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_THIRTYSEVEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_THIRTYEIGHT:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_THIRTYNINE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_FORTY:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYONE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_FORTYTWO:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYTHREE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYFOUR:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYFIVE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYSIX:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYSEVEN:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYEIGHT:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, TRUE);
      break;
    case ENCOUNTER_FORTYNINE:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_FIFTY:
      pullin_allies(storyrunner, target, TRUE, TRUE, FALSE, FALSE);
      break;
    case ENCOUNTER_FIFTYONE:
      pullin_allies(storyrunner, target, FALSE, TRUE, TRUE, FALSE);
      break;
    case ENCOUNTER_FIFTYTWO:
      pullin_allies(storyrunner, target, FALSE, TRUE, TRUE, FALSE);
      break;
    }
  }

  CHAR_DATA *encounter_victim(CHAR_DATA *ch) {
    if (ch->pcdata->encounter_sr != NULL && ch->pcdata->encounter_sr->in_room == ch->in_room)
    return ch->pcdata->encounter_sr;

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

      if (is_gm(victim))
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      return victim;
    }

    return NULL;
  }

  void offer_ally(CHAR_DATA *storyrunner, CHAR_DATA *target, CHAR_DATA *victim) {
    printf_to_char(victim, "%s is running an encounter with %s in which %s. You can join this scene with encounter join (%s).\n\r", storyrunner->name, PERS(target, victim), storyrunner->pcdata->encounter_bringin, storyrunner->name);
  }

  void offer_encounter(CHAR_DATA *storyrunner, CHAR_DATA *victim, int type) {
    victim->pcdata->encounter_sr = storyrunner;
    victim->pcdata->encounter_countdown = 120;
    victim->pcdata->encounter_status = ENCOUNTER_PENDING;
    victim->pcdata->encounter_number = type;
    storyrunner->pcdata->encounter_status = ENCOUNTER_PENDING;
    storyrunner->pcdata->encounter_sr = victim;
    storyrunner->pcdata->encounter_countdown = 120;
    storyrunner->pcdata->encounter_number = type;
    printf_to_char(
    victim, "You have been offered a role in an encounter run by %s. If you do not know this SR you can finger them to read logs of RP they've run before and use that to determine if you'd like to paricipate in their scenes in the future. If you'd like to accept this offer please type yes, you have two minutes to respond.\n\r", storyrunner->name);
  }

  bool encounter_overload_level(CHAR_DATA *ch)
  {
    int enc_karma = ch->pcdata->account->encounter_karma;
    if(enc_karma < 5000)
    return 1;

    int t_karma = enc_karma + ch->pcdata->account->adventure_karma + ch->pcdata->account->mystery_karma + ch->pcdata->account->monster_karma + ch->pcdata->account->other_karma + ch->pcdata->account->misc_karma;

    if(enc_karma > t_karma/2)
    {
      if(enc_karma >= 25000)
      return 3;
      else
      return 2;
    }
    return 1;
  }

  _DOFUN(do_1)
  {
    if(!is_gm(ch) && ch->pcdata->luck_type == 1)
    {
      if(ch->pcdata->luck_character == NULL || ch->pcdata->luck_character->in_room != ch->in_room)
      {
        ch->pcdata->luck_type = 0;
        send_to_char("They are no longer here.\n\r", ch);
        return;
      }
      printf_to_char(ch, "(`142Coinflip`x) %s will %s.\n\r", PERS(ch->pcdata->luck_character, ch), ch->pcdata->luck_string_one);
      printf_to_char(ch->pcdata->luck_character, "(`142Coinflip`x) You will %s.\n\r", ch->pcdata->luck_string_one);
      ch->pcdata->luck_type = 0;
      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if(to == NULL || to->in_room == NULL || IS_NPC(to))
        continue;

        if(to->pcdata->luck_character == ch->pcdata->luck_character)
        to->pcdata->luck_type = 0;
      }
      return;
    }
    if(!is_gm(ch) && ch->pcdata->luck_type == 2)
    {
      if(ch->pcdata->luck_character == NULL || ch->pcdata->luck_character->in_room != ch->in_room)
      {
        ch->pcdata->luck_type = 0;
        send_to_char("They are no longer here.\n\r", ch);
        return;
      }
      char msg[MSL];
      sprintf(msg, "(`038$n's Luck`x) %s.", ch->pcdata->luck_string_one);
      act(msg, ch->pcdata->luck_character, NULL, NULL, TO_CHAR);
      act(msg, ch->pcdata->luck_character, NULL, NULL, TO_ROOM);
      char_rplog(ch->pcdata->luck_character, msg);
      char_rplog(ch, msg);
      ch->pcdata->luck_type = 0;
      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if(to == NULL || to->in_room == NULL || IS_NPC(to))
        continue;
        if(to->pcdata->luck_character == ch->pcdata->luck_character)
        to->pcdata->luck_type = 0;
      }
      return;
    }

    if(!is_gm(ch) && is_fatesensitive(ch))
    {
      send_to_char("No luck available for you to manipulate.\n\r", ch);
      return;
    }
    char buf[MSL];
    if(!is_gm(ch) || ch->pcdata->encounter_status != ENCOUNTER_SELECTING)
    {
      send_to_char("You are not running an encounter.\n\r", ch);
      return;
    }
    if(ch->pcdata->enc_prompt_one == NULL || strlen(ch->pcdata->enc_prompt_one) < 5)
    {
      send_to_char("Not a viable option.\n\r", ch);
      return;
    }
    if(ch->pcdata->is_target_encounter == TRUE)
    ch->pcdata->account->target_encounter_cooldown = current_time + (3600 * 24 * 45);
    ch->pcdata->encounter_status = ENCOUNTER_ONGOING;
    sprintf(buf, "(%s)", ch->pcdata->enc_prompt_one);
    printf_to_char(ch, "Prompt chosen: %s\n\r", ch->pcdata->enc_prompt_one);
    free_string(ch->pcdata->encounter_bringin);
    ch->pcdata->encounter_bringin = str_dup(ch->pcdata->enc_prompt_one);
    logevent(ch->in_room, LOGEVENT_ENCOUNTER, ch->in_room->vnum, str_dup(buf), NULL);
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if((*it)->encounter_id == ch->pcdata->encounter_pnumber && number_percent() % 11 == 0)
      (*it)->valid = FALSE;
    }
  }

  _DOFUN(do_2)
  {
    if(!is_gm(ch) && ch->pcdata->luck_type == 1)
    {
      if(ch->pcdata->luck_character == NULL || ch->pcdata->luck_character->in_room != ch->in_room)
      {
        ch->pcdata->luck_type = 0;
        send_to_char("They are no longer here.\n\r", ch);
        return;
      }
      printf_to_char(ch, "(`142Coinflip`x) %s will %s.\n\r", PERS(ch->pcdata->luck_character, ch), ch->pcdata->luck_string_two);
      printf_to_char(ch->pcdata->luck_character, "(`142Coinflip`x) You will %s.\n\r", ch->pcdata->luck_string_two);
      ch->pcdata->luck_type = 0;
      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if(to == NULL || to->in_room == NULL || IS_NPC(to))
        continue;
        if(to->pcdata->luck_character == ch->pcdata->luck_character)
        to->pcdata->luck_type = 0;
      }
      return;
    }
    if(!is_gm(ch) && ch->pcdata->luck_type == 2)
    {
      if(ch->pcdata->luck_character == NULL || ch->pcdata->luck_character->in_room != ch->in_room)
      {
        ch->pcdata->luck_type = 0;
        send_to_char("They are no longer here.\n\r", ch);
        return;
      }
      char msg[MSL];
      sprintf(msg, "(`038$n's Luck`x) %s.", ch->pcdata->luck_string_two);
      act(msg, ch->pcdata->luck_character, NULL, NULL, TO_CHAR);
      act(msg, ch->pcdata->luck_character, NULL, NULL, TO_ROOM);
      char_rplog(ch->pcdata->luck_character, msg);
      char_rplog(ch, msg);
      ch->pcdata->luck_type = 0;
      CHAR_DATA *to;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        to = *it;
        if(to == NULL || to->in_room == NULL || IS_NPC(to))
        continue;
        if(to->pcdata->luck_character == ch->pcdata->luck_character)
        to->pcdata->luck_type = 0;
      }
      return;
    }

    if(!is_gm(ch) && is_fatesensitive(ch))
    {
      send_to_char("No luck available for you to manipulate.\n\r", ch);
      return;
    }


    char buf[MSL];
    if(!is_gm(ch) || ch->pcdata->encounter_status != ENCOUNTER_SELECTING)
    {
      send_to_char("You are not running an encounter.\n\r", ch);
      return;
    }
    if(ch->pcdata->enc_prompt_two == NULL || strlen(ch->pcdata->enc_prompt_two) < 5)
    {
      send_to_char("Not a viable option.\n\r", ch);
      return;
    }
    if(ch->pcdata->is_target_encounter == TRUE)
    ch->pcdata->account->target_encounter_cooldown = current_time + (3600 * 24 * 45);
    ch->pcdata->encounter_status = ENCOUNTER_ONGOING;
    sprintf(buf, "(%s)", ch->pcdata->enc_prompt_two);
    printf_to_char(ch, "Prompt chosen: %s\n\r", ch->pcdata->enc_prompt_two);
    free_string(ch->pcdata->encounter_bringin);
    ch->pcdata->encounter_bringin = str_dup(ch->pcdata->enc_prompt_two);
    logevent(ch->in_room, LOGEVENT_ENCOUNTER, ch->in_room->vnum, str_dup(buf), NULL);
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if((*it)->btype == 1 && (*it)->encounter_id == ch->pcdata->encounter_pnumber)
      (*it)->valid = FALSE;
    }
  }

  _DOFUN(do_3)
  {
    char buf[MSL];
    if(!is_gm(ch) || ch->pcdata->encounter_status != ENCOUNTER_SELECTING)
    {
      send_to_char("You are not running an encounter.\n\r", ch);
      return;
    }
    if(ch->pcdata->enc_prompt_three == NULL || strlen(ch->pcdata->enc_prompt_three) < 5)
    {
      send_to_char("Not a viable option.\n\r", ch);
      return;
    }
    if(ch->pcdata->is_target_encounter == TRUE)
    ch->pcdata->account->target_encounter_cooldown = current_time + (3600 * 24 * 45);
    ch->pcdata->encounter_status = ENCOUNTER_ONGOING;
    sprintf(buf, "(%s)", ch->pcdata->enc_prompt_three);
    printf_to_char(ch, "Prompt chosen: %s\n\r", ch->pcdata->enc_prompt_three);
    free_string(ch->pcdata->encounter_bringin);
    ch->pcdata->encounter_bringin = str_dup(ch->pcdata->enc_prompt_three);
    logevent(ch->in_room, LOGEVENT_ENCOUNTER, ch->in_room->vnum, str_dup(buf), NULL);
    int counter = 0;
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if((*it)->btype == 2)
      counter++;
    }
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if(counter > 5 && (*it)->btype == 2 && (*it)->encounter_id == ch->pcdata->encounter_pnumber && number_percent() % 4 == 0)
      (*it)->valid = FALSE;
    }
  }

  void have_encounter(CHAR_DATA *storyrunner, CHAR_DATA *target, int type) {
    ROOM_INDEX_DATA *to_room = mist_duplicate_room(target->in_room);
    ROOM_INDEX_DATA *orig = target->in_room;
    if (to_room == NULL) {
      send_to_char("No space.\n\r", storyrunner);
      return;
    }
    char title[MSL];
    sprintf(title, "%s\tEncounter launched: %s", storyrunner->pcdata->account->sr_history, target->name);
    free_string(storyrunner->pcdata->account->sr_history);
    storyrunner->pcdata->account->sr_history = str_dup(title);

    storyrunner->pcdata->account->encounter_cooldown =
    current_time + (3600 * 24 * 5);
    storyrunner->pcdata->encounter_orig_room = target->in_room->vnum;
    target->pcdata->encounter_orig_room = target->in_room->vnum;
    storyrunner->pcdata->karma_battery = UMAX(2000, storyrunner->pcdata->karma_battery+2000);
    storyrunner->pcdata->karma_battery /= encounter_overload_level(storyrunner);

    if (public_room(orig) || institute_room(orig))
    make_exit(to_room->vnum, orig->vnum, DIR_DOWN, CONNECT_ONEWAY);
    else
    make_exit(to_room->vnum, taxi_table[number_range(0, MAX_TAXIS - 1)].vnum, DIR_DOWN, CONNECT_ONEWAY);
    act("$n leaves.", storyrunner, NULL, NULL, TO_ROOM);
    char_from_room(storyrunner);
    char_to_room(storyrunner, to_room);
    act("$n leaves.", target, NULL, NULL, TO_ROOM);
    char_from_room(target);
    char_to_room(target, to_room);
    if (!IS_FLAG(storyrunner->comm, COMM_RUNNING))
    SET_FLAG(storyrunner->comm, COMM_RUNNING);
    target->pcdata->encounter_status = ENCOUNTER_ONGOING;
    storyrunner->pcdata->encounter_status = ENCOUNTER_SELECTING;

    target->pcdata->encounter_countdown = 0;
    storyrunner->pcdata->encounter_countdown = 0;

    send_to_char("Your encounter will begin shortly.\n\r", target);
    printf_to_char(storyrunner, "%s has accepted the encounter.\n\r", target->name);
    printf_to_char(storyrunner, "Choose a prompt:\n1) %s\n\r", encounter_prompt(storyrunner, type).c_str());
    free_string(storyrunner->pcdata->enc_prompt_one);
    storyrunner->pcdata->enc_prompt_one = str_dup(encounter_prompt(storyrunner, type).c_str());
    bool twofound = FALSE;
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if((*it)->btype == 1 && (*it)->encounter_id == type && twofound == FALSE)
      {
        printf_to_char(storyrunner, "2) %s\n\r", (*it)->text);
        free_string(storyrunner->pcdata->enc_prompt_two);
        storyrunner->pcdata->enc_prompt_two = str_dup((*it)->text);
        twofound = TRUE;
      }
    }
    bool threefound = FALSE;
    for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
    it != EEncounterVect.end(); ++it) {
      if((*it)->btype == 2 && threefound == FALSE && (*it)->encounter_id == type && (*it)->clan_id != target->fcult && (number_percent() % 3 == 0 || (*it)->clan_id == target->fsect))
      {
        printf_to_char(storyrunner, "3) %s\n\r", (*it)->text);
        free_string(storyrunner->pcdata->enc_prompt_three);
        storyrunner->pcdata->enc_prompt_two = str_dup((*it)->text);
        threefound = TRUE;
      }
    }
    send_to_char("Please choose a prompt by typing 1, 2 or 3.\n\r", storyrunner);
    storyrunner->pcdata->encounter_pnumber = type;


    //  recruit_autoallies(storyrunner, target, type);
    free_string(storyrunner->pcdata->encounter_storyline);
    storyrunner->pcdata->encounter_storyline = str_dup("");
    char buf[MSL];
    sprintf(buf, "Encounter %d launched. %s SR, %s Target.\n\r", storyrunner->pcdata->encounter_number, storyrunner->name, target->name);
    log_string(buf);
    wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);
  }

  std::string encounter_prompt(CHAR_DATA *ch, int type) {
    if (type == 1)
    return
    "Someone in Haven has found out about the supernatural and is freaking out about it. They're at risk of exposing the secret, hurting themselves, or hurting others. Your target and their allies are tasked with containing the situation.\n\r";
    if (type == 2)
    return
    "An amateur paranormal investigator has stumbled onto the truth of the supernatural world and has evidence. Your target and their allies are tasked with containing the situation.\n\r";
    if (type == 3)
    return
    "Your target is propositioned by a sex worker of some description who uses supernatural abilities to snare them, charming them, hypnotising them or manipulating their emotions. However their plan is in fact just to lure the target into a vulnerable location before robbing them of money, or valuables, or blood or something else of value.\n\r";
    if (type == 4)
    return "A ghost with only fragments of memory that have driven them near insane is attacking your target. They must either defeat it or find a way to calm it down.\n\r";
    if (type == 5)
    return
    "Your target is possessed by an angry spirit that is forcing them to act out and putting themselves and/or others at risk. They must either defeat it or find a way to calm it down.\n\r";
    if (type == 6)
    return "Your target has been abducted and is being held hostage by a supernatural criminal out to trade them for something or just use them as a shield against the factions. Your target must attempt to find a way to escape, or simply survive until they can be rescued by their allies.\n\r";
    if (type == 7)
    return "A demon from hell has become interested in your target, they decide to see if they can tempt them into becoming one of their instruments on earth.\n\r";
    if (type == 8)
    return "Your target and their allies are charged with tracking down a supernatural criminal on the run from the factions, what they do with them then is up to the players to decide.\n\r";
    if (type == 9)
    return
    "Your target has been cursed with madness in a public local, it is up to them to manage the affliction as best they can before their allies can arrive to handle the situation, subduing or calming them back down or working some sort of counter magic and at least removing them from the public eye.\n\r";
    if (type == 10)
    return
    "Your target has been cursed with persecution, it is up to them to survive a world suddenly turned hostile until their allies can come and help get them to safety or deal with the curse.\n\r";
    if (type == 11)
    return
    "Your target has been mind controlled by another into acting as their agent in a crime, compelled to perform a robbery or assault for this other agent. It is up to their allies to arrive and stop the crime and try to uncover the criminal.\n\r";
    if (type == 12)
    return "Your target has been flagged down by someone who wants their help getting their cat down from a tree.\n\r";
    if (type == 13)
    return
    "Your target is attacked by an animal or small group of animals driven mad with magic, it is up to them to escape or fight them off for long enough for their allies to arrive and help deal with the threat.\n\r";
    if (type == 14)
    return "Your target's been contacted to help find a civilian who's become lost in the woods.\n\r";
    if (type == 15)
    return "Your target has been picked up by members of an opposing faction or subfaction keen to get some intelligence from them. While the villains must abide by the understanding they will work their hardest to get the information, your target must resist for long enough or throw them off the trail for their allies to arrive and help get them out.\n\r";
    if (type == 16)
    return
    "Members of an opposing faction or subfaction are after your target. They must escape them or fight them off for long enough for their allies to arrive and help keep them protected.\n\r";
    if (type == 17)
    return "Your target has been abducted by the syndicate for potential sale offworld, they must escape or stall their abductors long enough for their allies to be able to come rescue them before the transaction can take place.\n\r";
    if (type == 18)
    return
    "Your target has been selected as the next meal for a vampire.\n\r";
    if (type == 19)
    return "A shifted werewolf attacks your target during the full moon. They need to escape or fight them off for long enough for their allies to come and help save them.\n\r";
    if (type == 20)
    return "Your target is attacked by a non shifted werewolf on the full moon, barely in control of their lunacy who's snapped. They need to escape or fight them off long enough for their allies to come and help save them.\n\r";
    if (type == 21)
    return
    "Your target encounters a newly made werewolf who doesn't know what they are or what they've done shortly after the full moon.\n\r";
    if (type == 22)
    return
    "Your target and their allies encounter a newly made vampire who hasn't been taught by their maker and doesn't know what they are.\n\r";
    if (type == 23)
    return
    "Your target and their allies encounter a newly activated supernatural who doesn't understand what's happened to them or what's next.\n\r";
    if (type == 24)
    return
    "Your target has been singled out by a dream stalker who's invading their dreams. They cannot be woken, but their allies may be able to go into their dreams after them to help them fight off the invader and survive the nightmare.\n\r";
    if (type == 25)
    return "Your target encounters a human who's become stuck in the nightmare, lost and frightened.\n\r";
    if (type == 26)
    return
    "Your target encounters a ghost who's fixated on some past tragedy from their life, they need to either give the spirit some sense of closure, or send it on it's way through more violent means.\n\r";
    if (type == 27)
    return
    "Someone has sent the state police after your target. Perhaps they're a real criminal or perhaps they've been framed, in either case it's up to them to get their arrest warrant handled and removed.\n\r";
    if (type == 28)
    return "A group of supernatural hunters is out to get your target. Maybe for sport, maybe from ideology, in either case they need to survive for long enough that their allies can come and help them deal with the threat.\n\r";
    if (type == 29)
    return "Your target is attacked by a lone vigilante, maybe someone they've wronged in the past or someone just out to get all supernaturals. They need to defeat their enemy or survive for long enough that their allies can come help them.\n\r";
    if (type == 30)
    return "Your target comes upon an NPC being targeted by a group of supernatural hunters or a lone vigilante. They need to try to keep them safe for long enough for help to arrive.\n\r";
    if (type == 31)
    return "Your target is swept into a pocket dream world by a true Fae interested in a romantic liaison\n\r";
    if (type == 32)
    return
    "Your target and their allies have been tasked with convincing a retired and burnt out faction member to come back to the fight.\n\r";
    if (type == 33)
    return "Your target is abducted in their sleep, waking up alone in a locked room. They need to either escape or draw attention to them so their allies can come and provide assistance.\n\r";
    if (type == 34)
    return
    "Your target and their allies have been tasked with extracting a piece of information from a member of an opposing faction or subfaction but they cannot break the understanding to do so.\n\r";
    if (type == 35)
    return
    "Your target has been hexed and transformed into an animal against their will. Unable to turn back they need to try to find allies who can understand their problem and find a way to undo the curse.\n\r";
    if (type == 36)
    return "Your target and their allies encounter the vassal of a demon or god who wants to quit working for their master and go back to a normal life. They need to either find a way to help them do that, or betray them to their master.\n\r";
    if (type == 37)
    return
    "Your target and their allies encounter the former thrall of a vampire who has run away from their previous master. Probably at least slightly mind controlled they're likely confused and struggling with their decision. The characters need to either help them found a new life, or send them back to their owner.\n\r";
    if (type == 38)
    return
    "Your target and their allies encounter the former thrall of a vampire who has been discarded by their previous owner, likely mind controlled into complete devotion the thrall wants nothing more than to return. It is up to the characters to either help them return, or stop them from doing so.\n\r";
    if (type == 39)
    return
    "Your target and their allies encounter the former thrall of a vampire who's become accidentally separated from their master. Likely mind controlled into complete devotion the thrall wants nothing more than to return. It is up to the characters to either help them do so, or prevent them from doing so.\n\r";
    if (type == 40)
    return
    "Your target is attacked by a dream stalker who subjects them to their greatest fantasies in the dream world in order to keep their body passive while it's energies are fed upon. They need to, possibly with the help of allies entering their dreams, resist the temptation long enough for other allies to find them or for them to wake up. \n\r";
    if (type == 41)
    return "Your target and their allies encounter someone who's been supernaturally influenced into committing a crime for a supernatural. It is up to them to stop the crime and then maybe also try to find the true perpetrator, maybe freeing the thrall from their control in the process.\n\r";
    if (type == 42)
    return "Your target is abducted by a sea creature that's somehow crossed over into our world, it is up to them to survive for long enough that their allies can come help.\n\r";
    if (type == 43)
    return
    "Your target is singled out by some sort of spirit that can only attack them through mirrors, it is up to them to survive long enough by avoiding mirrors/their reflection until their allies can help them find a way to defeat the monster.\n\r";
    if (type == 44)
    return "Your target is abuducted through the water to an undersea cave where an oceanic Fae is wanting a romantic or social encounter with them.\n\r";
    if (type == 45)
    return
    "Your target is attacked by monsters from the nightmare who've found a way through into our world. They need to survive for long enough for their allies to come help them.\n\r";
    if (type == 46)
    return "Your target is abducted by humans who believe that the target can help make them supernatural. They need to either get out of the situation themselves, or stall for long enough for their allies to come save them.\n\r";
    if (type == 47)
    return "Your target has been afflicted with a curse that mentally regresses them back to the mental state of an irresponsible teenager. It is up to their allies to figure out what's happening and fix it before they do anything too bad.\n\r";
    if (type == 48)
    return "Your target has been afflicted by a curse which takes away control of one of their limbs, which now has a new and likely destructive purpose. It is up to them to control the cursed limb for long enough for they and their allies to figure out how to contain or solve the problem.\n\r";
    if (type == 49)
    return
    "Your target and their allies encounter a demonborn who in the midst of sadistic lust has gone too far and is now filled with regret and self loathing at what they've done.\n\r";
    if (type == 50)
    return
    "Your target and their allies encounter a vampire who has killed their first feeding victim, likely less than six months a vampire they are horrified at what they've done and wracked with guilt. \n\r";
    if (type == 51)
    return
    "Your target and their allies have been tasked with helping to cure someone's insanity by delving into their mind with dream invading to solve the issues keeping them from sanity.\n\r";
    if (type == 52)
    return
    "Your target and their allies have been tasked with stealing a vital piece of intelligence from a subject by delving into their mind with dream invading to try to tease out the secret.\n\r";


    return "";
  }

  _DOFUN(do_encounter) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    CHAR_DATA *victim;
    int number;
    if (!str_cmp(arg1, "storyline")) {
      if (safe_strlen(argument) > 2 && get_storyline(ch, argument) != NULL) {
        if (ch->pcdata->karma_battery <= 0 || ch->pcdata->encounter_storyline_added == TRUE) {
          send_to_char("You can't do that yet, wait until after the encounter has been launched.\n\r", ch);
          return;
        }
        STORYLINE_TYPE *story = get_storyline(ch, argument);
        story->power += 2;
        story->lastmodified = current_time;
        ch->pcdata->encounter_storyline_added = TRUE;
        for (CharList::iterator ij = ch->in_room->people->begin();
        ij != ch->in_room->people->end(); ++ij) {
          CHAR_DATA *victim = *ij;
          if (victim == NULL)
          continue;
          if (victim->in_room == NULL)
          continue;
          if (IS_NPC(victim))
          continue;
          join_to_storyline(victim, story);
        }
        printf_to_char(ch, "You add the encounter to the %s storyline.\n\r", story->name);
        return;
      }
      send_to_char("No such storyline.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "mastermind")) {
      if (safe_strlen(argument) > 2) {
        if (ch->pcdata->karma_battery <= 0 || ch->pcdata->encounter_storyline_added == TRUE) {
          send_to_char("You can't do that yet, wait until after the encounter has been launched and you've assigned a storyline.\n\r", ch);
          return;
        }
        if (ch->pcdata->encounter_mastermind_added == TRUE) {
          send_to_char("You've already done that.\n\r", ch);
          return;
        }
        STORYLINE_TYPE *story = get_storyline(ch, argument);
        add_mastermind(story, argument, 2);
        story->lastmodified = current_time;
        ch->pcdata->encounter_mastermind_added = TRUE;
        for (CharList::iterator ij = ch->in_room->people->begin();
        ij != ch->in_room->people->end(); ++ij) {
          CHAR_DATA *victim = *ij;
          if (victim == NULL)
          continue;
          if (victim->in_room == NULL)
          continue;
          if (IS_NPC(victim))
          continue;
          send_to_char("`c(%s is the mastermind behind this encounter.)`x\n\r", victim);
        }
        printf_to_char(ch, "You make %s the mastermind of the encounter.\n\r", argument);
        return;
      }
      send_to_char("Encounter mastermind (name).\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "launch")) {
      if (!is_gm(ch)) {
        send_to_char("This is a storyrunner command.\n\r", ch);
        return;
      }

      if (ch->pcdata->account->encounter_cooldown > current_time) {
        send_to_char("That's still on cooldown.\n\r", ch);
        return;
      }
      if (ch->pcdata->encounter_countdown > 0) {
        send_to_char("You're already doing that.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) > 1 && ch->pcdata->account->target_encounter_cooldown > current_time) {
        send_to_char("You've done a targeted encounter too recently.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) > 1 && get_char_world_pc(argument) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }
      bool numfound;
      if (safe_strlen(argument) > 1) {
        victim = get_char_world_pc(argument);
        if (!free_to_act(victim)) {
          send_to_char("They likely would not be able to accept right now.\n\r", ch);
          return;
        }
        if(safe_strlen(argument) < 2)
        ch->pcdata->is_target_encounter = FALSE;

        free_string(ch->pcdata->encounter_bringin);
        ch->pcdata->encounter_bringin = str_dup("");
        for (int i = 0; i < 3; i++) {
          number = number_range(1, 957) % ENCOUNTER_MAX;
          number++;
        }
        numfound = FALSE;
        int c;
        for (c = 0; c < 1000 && numfound == FALSE; c++) {
          if (encounter_probability(number) > number_percent() && can_encounter(victim, number))
          numfound = TRUE;
          else {
            number = number_range(1, 957) % ENCOUNTER_MAX;
            number++;
          }
        }
        if (c > 990) {
          send_to_char("Attempt failed.\n\r", ch);
          return;
        }
        if (!spammer(ch) && !spammer(victim))
        {
          offer_encounter(ch, victim, number);
          ch->pcdata->is_target_encounter = TRUE;
        }

        send_to_char("Encounter offered.\n\r", ch);

        return;
      }

      free_string(ch->pcdata->encounter_bringin);
      ch->pcdata->encounter_bringin = str_dup("");
      for (int i = 0; i < 30; i++) {
        number = number_range(1, 957) % ENCOUNTER_MAX;
        number++;
      }
      numfound = FALSE;

      for (; numfound == FALSE;) {
        if (encounter_probability(number) > number_percent())
        numfound = TRUE;
        else {
          number = number_range(1, 957) % ENCOUNTER_MAX;
          number++;
        }
      }

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          victim = d->character;
          if (IS_NPC(victim))
          continue;
          if (victim->in_room == NULL)
          continue;
          if (victim->pcdata->encounter_status == ENCOUNTER_REJECTED)
          continue;
          if (victim->in_room->area->vnum == 12)
          continue;
          if (same_player(ch, victim))
          continue;
          if (is_gm(victim) || IS_FLAG(victim->act, PLR_GUEST))
          continue;
          if (get_gmtrust(ch, victim) < 0)
          continue;
          if (solidity(victim) < number_percent())
          continue;
          if (norp_match(ch, victim))
          continue;

          if (victim->pcdata->availability != AVAIL_HIGH && number_percent() % 4 != 0)
          continue;

          if (can_encounter(victim, number)) {
            if (!spammer(ch) && !spammer(victim))
            offer_encounter(ch, victim, number);
            send_to_char("Encounter offered.\n\r", ch);
            return;
          }
        }
      }

      send_to_char("We couldn't find anyone suitable, wait a few minutes and then try again.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "bringin")) {
      if (!is_gm(ch)) {
        send_to_char("This is a storyrunner command.\n\r", ch);
        return;
      }
      victim = encounter_victim(ch);
      if (victim == NULL) {
        send_to_char("You aren't running an encounter.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: Encounter bringin (short description of events.)\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "stop")) {
        free_string(ch->pcdata->encounter_bringin);
        ch->pcdata->encounter_bringin = str_dup("");
        send_to_char("Done.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->encounter_bringin);
      ch->pcdata->encounter_bringin = str_dup(argument);
      recruitallies(ch, victim, ch->pcdata->encounter_number);
      send_to_char("You bring in more characters.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "join")) {
      if (is_gm(ch) || is_helpless(ch) || in_fight(ch) || IS_FLAG(ch->act, PLR_DEAD) || ch->in_room == NULL || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room) || is_pinned(ch)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      victim = get_char_world_pc(argument);
      if (victim == NULL || !is_gm(victim)) {
        send_to_char("Join whose encounter?\n\r", ch);
        return;
      }
      if (encounter_victim(victim) == NULL) {
        send_to_char("They're not running an encounter.\n\r", ch);
        return;
      }
      if (pc_pop(victim->in_room) >= 5) {
        send_to_char("There's already too many people there.\n\r", ch);
        return;
      }
      if (safe_strlen(victim->pcdata->encounter_bringin) < 2) {
        send_to_char("You can't yet join that encounter.\n\r", ch);
        return;
      }
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
      char_to_room(ch, victim->in_room);
      send_to_char("You join the encounter.\n\r", ch);
      act("$n arrives.", ch, NULL, NULL, TO_ROOM);
      ch->pcdata->encounter_status = ENCOUNTER_ONGOING;
      ch->pcdata->encounter_countdown = 0;
    }
    else
    send_to_char("Syntax: Encounter launch/bringin/join\n\r", ch);
  }

  bool encounter_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (room->area->vnum != DIST_MISTS)
    return FALSE;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if ((*it)->vnum == room->vnum)
      return FALSE;
    }
    return TRUE;
  }

  vector<ROSTERCHAR_TYPE *> RosterCharVect;
  ROSTERCHAR_TYPE *nullrosterchar;

  void fread_rosterchar(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    ROSTERCHAR_TYPE *roster;

    roster = new_rosterchar();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'C':
        KEY("Claimed", roster->claimed, fread_number(fp));
        break;
      case 'D':
        KEY("Description", roster->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          RosterCharVect.push_back(roster);
          return;
        }
        break;
      case 'L':
        KEY("LastName", roster->lastname, fread_string(fp));
        KEY("LastActive", roster->lastactive, fread_number(fp));
        break;
      case 'N':
        KEY("Name", roster->name, fread_string(fp));
        break;
      case 'O':
        KEY("Owner", roster->owner, fread_string(fp));
        break;
      case 'S':
        KEY("ShortDesc", roster->shortdesc, fread_string(fp));
        KEY("Sponsored", roster->sponsored, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_rosterchar: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_rosterchars() {
    nullrosterchar = new_rosterchar();
    FILE *fp;

    if ((fp = fopen(ROSTER_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_rosterchars: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "ROSTERCHAR")) {
          fread_rosterchar(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_rosterchar: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open roster.txt", 0);
      exit(0);
    }
  }
  void save_rosterchars(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/roster.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/roster.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/roster.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/roster.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/roster.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/roster.txt");
      else
      sprintf(buf, "../data/back7/roster.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open roster.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(ROSTER_FILE, "w")) == NULL) {
        bug("Cannot open roster.txt for writing", 0);
        return;
      }
    }

    for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
    it != RosterCharVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }

      fprintf(fpout, "#RosterChar\n");
      fprintf(fpout, "Owner %s~\n", (*it)->owner);
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "LastName %s~\n", (*it)->lastname);
      fprintf(fpout, "ShortDesc %s~\n", (*it)->shortdesc);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Sponsored %d\n", (*it)->sponsored);
      fprintf(fpout, "LastActive %d\n", (*it)->lastactive);
      fprintf(fpout, "Claimed %d\n", (*it)->claimed);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_rosterchars(TRUE);
    }
  }

  _DOFUN(do_roster) {
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);
    if (!str_cmp(arg1, "list")) {
      send_to_char("`WCharacter Roster`x\n\r", ch);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;
        if ((*it)->sponsored == 1)
        sprintf(buf, "`W%2d`c[`WA`c]`x\t%s %s, %s\r", i, (*it)->name, (*it)->lastname, (*it)->shortdesc);
        else
        sprintf(buf, "`W%2d`c[ ]`x\t%s %s, %s\r", i, (*it)->name, (*it)->lastname, (*it)->shortdesc);
        strcat(string, buf);
        i++;
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      return;
    }

    if (is_griefer(ch)) {
      return;
    }

    if (!str_cmp(arg1, "description")) {
      send_to_char("This description will appear when anyone looks at the detailed information on your rostered character.\n\r", ch);
      string_append(ch, &ch->pcdata->roster_description);
      return;
    }
    if (!str_cmp(arg1, "introduction")) {
      send_to_char("This is a few words that will appear after the character's name on the roster list.\n\r", ch);
      string_append(ch, &ch->pcdata->roster_shortdesc);
      return;
    }
    if (!str_cmp(arg1, "reclaim")) {
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 0)
        continue;
        if (!str_cmp((*it)->owner, ch->pcdata->account->name) && !str_cmp(argument, (*it)->name)) {
          (*it)->claimed = 0;
          printf_to_char(ch, "You return %s to the roster.\n\r", (*it)->name);
          if (get_char_world_pc((*it)->name) != NULL) {
            send_to_char("This character has been reclaimed by the original author.\n\r", get_char_world_pc((*it)->name));
            real_quit(get_char_world_pc((*it)->name));
          }
          return;
        }
      }
      send_to_char("You don't have any such character recently taken from the roster.\n\r", ch);
    }
    if (!str_cmp(arg1, "info")) {
      int val = atoi(argument);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;

        if (i == val) {
          if ((*it)->sponsored == 1)
          sprintf(buf, "`W%d`c)`x\t%s %s, %s\n`gThis character has been checked and approved by Staff`x\n\n%s\n\r", i, (*it)->name, (*it)->lastname, (*it)->shortdesc, (*it)->description);
          else
          sprintf(buf, "`W%d`c)`x\t%s %s, %s\n\n%s\n\r", i, (*it)->name, (*it)->lastname, (*it)->shortdesc, (*it)->description);
          strcat(string, buf);
          page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
          return;
        }
        i++;
      }
      send_to_char("No such entry.\n\r", ch);
    }
    if (!str_cmp(arg1, "setowner") && IS_IMMORTAL(ch)) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;

        if (i == val) {
          free_string((*it)->owner);
          (*it)->owner = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
        i++;
      }
      send_to_char("No such entry.\n\r", ch);
    }
    if (!str_cmp(arg1, "delete") && IS_IMMORTAL(ch)) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;

        if (i == val) {
          (*it)->valid = FALSE;
          send_to_char("Done.\n\r", ch);
          return;
        }
        i++;
      }
      send_to_char("No such entry.\n\r", ch);
    }
    if (!str_cmp(arg1, "setlastname") && IS_IMMORTAL(ch)) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;

        if (i == val) {
          free_string((*it)->lastname);
          (*it)->lastname = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
        i++;
      }
      send_to_char("No such entry.\n\r", ch);
    }
    if (!str_cmp(arg1, "sponsor") && IS_IMMORTAL(ch)) {
      int val = atoi(argument);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;

        if (i == val) {
          if ((*it)->sponsored == 0) {
            (*it)->sponsored = 1;
            printf_to_char(ch, "You set %s to be sponsored.\n\r", ch);
            return;
          }
          else {
            (*it)->sponsored = 0;
            printf_to_char(ch, "You set %s to be unsponsored.\n\r", ch);
            return;
          }
        }
        i++;
      }
      send_to_char("No such entry.\n\r", ch);
    }
    if (!str_cmp(arg1, "claim")) {
      if (ch->in_room == NULL || ch->in_room->vnum != ROOM_INDEX_GENESIS) {
        send_to_char("You need to be in character generation to do this.\n\r", ch);
        return;
      }

      if (ch->pcdata->account != NULL && ch->pcdata->account->roster_cool > current_time) {
        send_to_char("You've claimed a character from the roster too recently.\n\r", ch);
        return;
      }
      if (ch->pcdata->account->newcharcount <= 0) {
        send_to_char("You can't make a new character again yet.\n\r", ch);
        return;
      }

      int val = atoi(argument);
      int i = 1;
      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->claimed == 1)
        continue;

        if (i == val) {
          log_string("DESCRIPTOR: Taking Roster Char");
          int timeval = (*it)->lastactive;
          (*it)->claimed = 1;
          save_char_obj(ch, FALSE, FALSE);
          ACCOUNT_TYPE *account = ch->pcdata->account;
          char name[MSL];
          sprintf(name, "%s", (*it)->name);
          DESCRIPTOR_DATA *d = ch->desc;
          free_char(ch);
          load_char_obj(d, name);
          ch->pcdata->account = account;
          ch->desc->account = account;
          free_string(ch->pcdata->account_name);
          ch->pcdata->account_name = str_dup(account->name);
          add_char_to_account(ch, ch->pcdata->account);
          char_from_room(ch);
          char_to_room(ch, get_room_index(50));
          ch->played = 0;
          ch->pcdata->training_disc = 0;
          ch->pcdata->training_stat = 0;
          if (str_cmp((*it)->owner, ch->pcdata->account->name)) {
            ch->spentnewrpexp += ch->spentrpexp;
            ch->spentrpexp = 0;
            ch->spentnewexp += ch->spentexp;
            ch->spentexp = 0;
          }
          printf_to_char(ch, "You become %s %s!\nYou can send this character back to the roster with roster deliver, but keep in mind if you alter anything you will be unable to claim another rostered character for a month.\n\r", ch->name, ch->pcdata->last_name);
          ch->pcdata->account->pkarmaspent += ch->spentpkarma;
          if (current_time - timeval > (3600 * 24 * 10)) {
            int years = get_real_age(ch) - 18;
            years = UMIN(years, 22);
            years = UMAX(1, years);
            ch->money = UMAX(0, ch->money);
            ch->pcdata->total_money = UMAX(0, ch->pcdata->total_money);
            ch->pcdata->total_credit =
            UMAX(ch->pcdata->total_credit, (100000 + years * 50000));
            ch->pcdata->attract[ATTRACT_PROM] /= 5;
          }
          return;
        }
        i++;
      }
      send_to_char("No such entry.\n\r", ch);
    }
    if (!str_cmp(arg1, "deliver")) {
      if (in_fight(ch) || is_helpless(ch) || room_hostile(ch->in_room) || is_pinned(ch) || is_gm(ch) || IS_FLAG(ch->act, PLR_GUEST) || IS_FLAG(ch->act, PLR_NOSAVE)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (ch->in_room == NULL || ch->in_room->vnum == ROOM_INDEX_GENESIS) {
        send_to_char("You have to finish that character first.\n\r", ch);
        return;
      }

      if (get_tier(ch) > 2) {
        send_to_char("That character is too high tier to be sent to the roster.\n\r", ch);
        return;
      }
      if (ch->skills[SKILL_MENTALDISCIPLINE] > 0 && get_tier(ch) == 2) {
        send_to_char("That character is too high tier to be sent to the roster.\n\r", ch);
        return;
      }

      for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
      it != RosterCharVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp((*it)->name, ch->name)) {
          if (safe_strlen(ch->pcdata->roster_shortdesc) > 2) {
            free_string((*it)->shortdesc);
            (*it)->shortdesc = str_dup(ch->pcdata->roster_shortdesc);
          }
          if (safe_strlen(ch->pcdata->roster_description) > 2) {
            free_string((*it)->description);
            (*it)->description = str_dup(ch->pcdata->roster_description);
          }

          ch->pcdata->account->pkarmaspent -= ch->spentpkarma;
          ch->pcdata->account->pkarmaspent =
          UMAX(ch->pcdata->account->pkarmaspent, 0);
          ch->spentkarma = 0;
          if (!str_cmp(ch->pcdata->account->lastfacchar, ch->name)) {
            free_string(ch->pcdata->account->lastfacchar);
            ch->pcdata->account->lastfacchar = str_dup("");
          }
          free_string(ch->pcdata->account_name);
          ch->pcdata->account_name = str_dup("");
          char_from_room(ch);
          char_to_room(ch, get_room_index(50));

          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->account->characters[i], ch->name)) {
              free_string(ch->pcdata->account->characters[i]);
              ch->pcdata->account->characters[i] = str_dup("");
            }
          }
          save_account(ch->pcdata->account, FALSE);
          (*it)->claimed = 0;
          ch->pcdata->account = NULL;
          save_char_obj(ch, FALSE, FALSE);
          printf_to_char(ch, "You return %s to the roster.\n\r", ch->name);
          real_quit(ch);
          return;
        }
      }
      if (safe_strlen(ch->pcdata->roster_description) < 2) {
        send_to_char("You have to write a roster description for that character first, type roster description.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->roster_shortdesc) < 2) {
        send_to_char("You have to write a short roster introduction for that character first, type roster introduction.\n\r", ch);
        return;
      }

      ROSTERCHAR_TYPE *rosterchar = new_rosterchar();
      free_string(rosterchar->name);
      rosterchar->name = str_dup(ch->name);
      free_string(rosterchar->lastname);
      rosterchar->lastname = str_dup(ch->pcdata->last_name);
      free_string(rosterchar->shortdesc);
      rosterchar->shortdesc = str_dup(ch->pcdata->roster_shortdesc);
      free_string(rosterchar->description);
      rosterchar->description = str_dup(ch->pcdata->roster_description);
      free_string(rosterchar->owner);
      rosterchar->owner = str_dup(ch->pcdata->account_name);
      rosterchar->lastactive = current_time;
      rosterchar->valid = TRUE;
      RosterCharVect.push_back(rosterchar);
      free_string(ch->pcdata->account_name);
      ch->pcdata->account_name = str_dup("");
      char_from_room(ch);
      ch->spentkarma = 0;
      char_to_room(ch, get_room_index(50));
      save_char_obj(ch, FALSE, FALSE);
      for (int i = 0; i < 25; i++) {
        if (!str_cmp(ch->pcdata->account->characters[i], ch->name)) {
          free_string(ch->pcdata->account->characters[i]);
          ch->pcdata->account->characters[i] = str_dup("");
        }
      }
      save_account(ch->pcdata->account, FALSE);
      printf_to_char(ch, "You deliver %s to the roster.\n\r", ch->name);
      real_quit(ch);
      return;
    }
  }

  ROSTERCHAR_TYPE *get_rosterchar(char *name) {
    for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
    it != RosterCharVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!str_cmp(name, (*it)->name))
      return *it;
    }
    return NULL;
  }

  bool is_roster_char(CHAR_DATA *ch) {
    for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
    it != RosterCharVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!str_cmp(ch->name, (*it)->name) && safe_strlen(ch->name) > 1)
      return TRUE;
    }
    return FALSE;
  }

  void remove_from_roster(char *name) {
    for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
    it != RosterCharVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!str_cmp(name, (*it)->name)) {
        (*it)->valid = FALSE;
        return;
      }
    }
  }

  void alter_character(CHAR_DATA *ch) {
    if (ch->played / 3600 < 25) {
      if (is_roster_char(ch)) {
        get_rosterchar(ch->name)->lastactive = current_time;
        if (ch->pcdata->account != NULL && ch->pcdata->account->roster_cool < current_time && str_cmp(get_rosterchar(ch->name)->owner, ch->pcdata->account->name)) {
          ch->pcdata->account->roster_cool = current_time + (3600 * 24 * 30);
          ch->pcdata->account->newcharcount--;
        }
      }
    }
  }

  void offline_karma(char *argument, int amount) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if (safe_strlen(argument) < 2)
    return;

    if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: offline karma");

      if (!load_char_obj(&d, argument)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online) {
        free_char(victim);
        return;
      }
    }
    give_karma(victim, amount, KARMA_OTHER);

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  void roster_update(void) {
    //    char strsave[MAX_INPUT_LENGTH];

    for (vector<ROSTERCHAR_TYPE *>::iterator it = RosterCharVect.begin();
    it != RosterCharVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (get_char_world_pc((*it)->name) != NULL)
      (*it)->lastactive = current_time;
      /*
if((*it)->lastactive + (3600*24*120) < current_time)
{
sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( (*it)->name ) );
unlink(strsave);
(*it)->valid = FALSE;
}
*/
    }
  }

  _DOFUN(do_search) {
    ch->pcdata->process = PROCESS_SEARCH;
    ch->pcdata->process_timer = 30;
    ch->pcdata->process_subtype = ch->in_room->vnum;
    act("You start to search the area.\n\r", ch, NULL, NULL, TO_CHAR);
    act("$n starts to search the area.\n\r", ch, NULL, NULL, TO_ROOM);
    return;
  }

  vector<GALLERY_TYPE *> GalleryVect;
  GALLERY_TYPE *nullgallery;

  void fread_gallery(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    GALLERY_TYPE *gallery;

    gallery = new_gallery();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("AccountOwner", gallery->account_owner, fread_string(fp));
        break;
      case 'C':
        KEY("CoAuthors", gallery->coauthors, fread_string(fp));
        break;
      case 'D':
        KEY("Description", gallery->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          GalleryVect.push_back(gallery);
          return;
        }
        KEY("Editable", gallery->editable, fread_number(fp));
        KEY("Exp", gallery->xp, fread_number(fp));
        break;
      case 'L':
        KEY("LastModified", gallery->lastmodified, fread_number(fp));
        break;
      case 'N':
        KEY("Name", gallery->name, fread_string(fp));
        break;
      case 'O':
        KEY("Owner", gallery->owner, fread_string(fp));
        break;
      case 'S':
        KEY("SubFaction", gallery->subfaction, fread_number(fp));
        break;
      case 'T':
        KEY("Territory", gallery->territory, fread_string(fp));
        KEY("Timeline", gallery->timeline, fread_string(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_gallery: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_galleries() {
    nullgallery = new_gallery();
    FILE *fp;

    if ((fp = fopen(GALLERY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_galleries: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "GALLERY")) {
          fread_gallery(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_gallery: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open gallery.txt", 0);
      exit(0);
    }
  }
  void save_galleries(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/gallery.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/gallery.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/gallery.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/gallery.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/gallery.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/gallery.txt");
      else
      sprintf(buf, "../data/back7/gallery.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open gallery.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(GALLERY_FILE, "w")) == NULL) {
        bug("Cannot open gallery.txt for writing", 0);
        return;
      }
    }

    for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
    it != GalleryVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it)->owner == NULL || safe_strlen((*it)->owner) < 2)
      continue;

      if ((*it)->lastmodified == 0)
      (*it)->lastmodified = current_time;

      if ((*it)->lastmodified + (3600 * 24 * 90) < current_time && str_cmp((*it)->owner, "Tyr"))
      continue;

      fprintf(fpout, "#GALLERY\n");
      fprintf(fpout, "Owner %s~\n", (*it)->owner);
      fprintf(fpout, "Exp %d\n", (*it)->xp);
      fprintf(fpout, "AccountOwner %s~\n", (*it)->account_owner);
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Territory %s~\n", (*it)->territory);
      fprintf(fpout, "SubFaction %d\n", (*it)->subfaction);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Timeline %s~\n", (*it)->timeline);
      fprintf(fpout, "CoAuthors %s~\n", (*it)->coauthors);
      fprintf(fpout, "LastModified %d\n", (*it)->lastmodified);
      fprintf(fpout, "Editable %d\n", (*it)->editable);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_galleries(TRUE);
    }
  }

  vector<PAGE_TYPE *> PageVect;
  PAGE_TYPE *nullpage;

  void fread_page(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    PAGE_TYPE *page;

    page = new_page();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", page->author, fread_string(fp));
        break;
      case 'C':
        KEY("CreateDate", page->createdate, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          PageVect.push_back(page);
          return;
        }
        break;
      case 'N':
        KEY("Number", page->number, fread_number(fp));
        break;
      case 'T':
        KEY("Text", page->text, fread_string(fp));
        break;
      case 'W':
        KEY("World", page->world, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_world: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_pages() {
    nullpage = new_page();
    FILE *fp;

    if ((fp = fopen("../data/pages.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_pages: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PAGE")) {
          fread_page(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_page: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open pages.txt", 0);
      exit(0);
    }
  }

  void save_pages(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/pages.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/pages.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/pages.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/pages.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/pages.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/pages.txt");
      else
      sprintf(buf, "../data/back7/pages.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open pages.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/pages.txt", "w")) == NULL) {
        bug("Cannot open pages.txt for writing", 0);
        return;
      }
    }

    for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
    it != PageVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }

      fprintf(fpout, "#PAGE\n");
      fprintf(fpout, "Number %d\n", (*it)->number);
      fprintf(fpout, "World %d\n", (*it)->world);
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Text %s~\n", (*it)->text);
      fprintf(fpout, "CreateDate %d\n", (*it)->createdate);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_pages(TRUE);
    }
  }

  vector<STORYLINE_TYPE *> StorylineVect;
  STORYLINE_TYPE *nullstoryline;

  void fread_storyline(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    STORYLINE_TYPE *storyline;

    storyline = new_storyline();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'C':
        KEY("CoAuthors", storyline->coauthors, fread_string(fp));
        break;
      case 'D':
        KEY("Description", storyline->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          StorylineVect.push_back(storyline);
          return;
        }
        break;
      case 'L':
        KEY("LastModified", storyline->lastmodified, fread_number(fp));
        break;
      case 'M':
        if (!str_cmp(word, "Masterminds")) {
          int point = -1;
          for (int i = 0; i < 10 && point == -1; i++) {
            if (safe_strlen(storyline->masterminds[i]) < 2)
            point = i;
          }
          free_string(storyline->masterminds[point]);
          storyline->masterminds[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        if (!str_cmp(word, "MastermindPower")) {
          int point = -1;
          for (int i = 0; i < 10 && point == -1; i++) {
            if (storyline->mastermind_power[i] < 1)
            point = i;
          }
          storyline->mastermind_power[point] = fread_number(fp);
          fMatch = TRUE;
        }

        break;
      case 'N':
        KEY("Name", storyline->name, fread_string(fp));
        break;
      case 'O':
        KEY("Owner", storyline->owner, fread_string(fp));
        break;
      case 'P':
        KEY("Power", storyline->power, fread_number(fp));
        if (!str_cmp(word, "Participant")) {
          int point = -1;
          for (int i = 0; i < 100 && point == -1; i++) {
            if (safe_strlen(storyline->participants[i]) < 2)
            point = i;
          }
          free_string(storyline->participants[point]);
          storyline->participants[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        break;
      case 'T':
        KEY("Timeline", storyline->timeline, fread_string(fp));
        KEY("TotalPower", storyline->total_power, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_gallery: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_storylines() {
    nullstoryline = new_storyline();
    FILE *fp;

    if ((fp = fopen(STORYLINE_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_storylines: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "STORYLINE")) {
          fread_storyline(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_storylines: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open storylines.txt", 0);
      exit(0);
    }
  }
  void save_storylines(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/storylines.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/storylines.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/storylines.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/storylines.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/storylines.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/storylines.txt");
      else
      sprintf(buf, "../data/back7/storylines.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open storylines.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(STORYLINE_FILE, "w")) == NULL) {
        bug("Cannot open storylines.txt for writing", 0);
        return;
      }
    }

    for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
    it != StorylineVect.end(); ++it) {
      if ((*it)->valid == FALSE || safe_strlen((*it)->owner) < 2) {
        continue;
      }
      if ((*it)->lastmodified == 0)
      (*it)->lastmodified = current_time;

      if ((*it)->lastmodified + (3600 * 24 * 180) < current_time)
      continue;

      if((*it)->total_power < (*it)->power)
      (*it)->total_power = (*it)->power;

      fprintf(fpout, "#STORYLINE\n");
      fprintf(fpout, "Owner %s~\n", (*it)->owner);
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "LastModified %d\n", (*it)->lastmodified);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Timeline %s~\n", (*it)->timeline);
      fprintf(fpout, "CoAuthors %s~\n", (*it)->coauthors);
      fprintf(fpout, "Power %d\n", (*it)->power);
      fprintf(fpout, "TotalPower %d\n", (*it)->total_power);

      for (int i = 0; i < 100; i++) {
        if (safe_strlen((*it)->participants[i]) > 2)
        fprintf(fpout, "Participant %s~\n", (*it)->participants[i]);
      }
      for (int i = 0; i < 10; i++) {
        if (safe_strlen((*it)->masterminds[i]) > 1 && (*it)->mastermind_power[i] > 0) {
          fprintf(fpout, "Masterminds %s~\n", (*it)->masterminds[i]);
          fprintf(fpout, "MastermindPower %d\n", (*it)->mastermind_power[i]);
        }
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_storylines(TRUE);
    }
  }

  char *const edit_states[] = {"None", "Timeline", "Details", "Life and death"};

  char *newtimeline(char *oldtimeline, char *argument) {
    char buf[MSL];
    time_t east_time;
    east_time = current_time;
    char tmp[MSL];
    char datestr[MSL];
    sprintf(tmp, "%s", (char *)ctime(&east_time));
    sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);

    sprintf(buf, "%s\n\r%s: %s", oldtimeline, datestr, argument);
    return str_dup(buf);
  }

  GALLERY_TYPE *gallery_lookup(char *name) {

    for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
    it != GalleryVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (!str_cmp((*it)->name, name))
      return (*it);
    }
    return NULL;
  }

  _DOFUN(do_gallery) {
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    char arg1[MSL];
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "add") || !str_cmp(arg1, "create")) {
      if (!is_gm(ch)) {
        send_to_char("This is a story runner command.\n\r", ch);
        return;
      }
      if (ch->pcdata->account == NULL || ch->pcdata->account->karmaearned < 1000) {
        send_to_char("You need to have earned at least 1k karma first.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: Gallery add (full name of NPC)\n\r", ch);
        return;
      }
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!str_cmp((*it)->name, argument)) {
          send_to_char("An NPC with that name already exists.\n\r", ch);
          return;
        }
      }
      GALLERY_TYPE *mob = new_gallery();
      free_string(mob->name);
      mob->name = str_dup(argument);
      mob->valid = TRUE;
      free_string(mob->owner);
      mob->owner = str_dup(ch->name);
      free_string(mob->account_owner);
      mob->account_owner = str_dup(ch->pcdata->account->name);
      mob->xp = 0;
      GalleryVect.push_back(mob);
      printf_to_char(ch, "%s has been added to the NPC gallery.", mob->name);
      return;
    }
    else if (!str_cmp(arg1, "list")) {
      int i = 1;
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        sprintf(buf, "`W%2d`c)`x %s \t\t`c[`x%s`c]`x\n\r", i, (*it)->name, (*it)->owner);
        strcat(string, buf);
        i++;
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      return;
    }
    else if (!str_cmp(arg1, "info")) {
      int i = 1;
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          sprintf(buf, "`W%2d`c)`x %s \t\t`c[`x%s`c]`x\n", i, (*it)->name, (*it)->owner);
          strcat(string, buf);
          if (safe_strlen((*it)->territory) > 2) {
            sprintf(buf, "Home Territory: %s\n\r", (*it)->territory);
            strcat(string, buf);
          }
          if ((*it)->subfaction > 0) {
            sprintf(buf, "Association: %s\n\r", get_subfac_name((*it)->subfaction));
            strcat(string, buf);
          }
          if (safe_strlen((*it)->coauthors) > 2 && (IS_IMMORTAL(ch) || is_name(ch->name, (*it)->coauthors) || !str_cmp((*it)->owner, ch->name))) {
            sprintf(buf, "Co-Authors: %s\n\r", (*it)->coauthors);
            strcat(string, buf);
          }
          if (is_gm(ch)) {
            sprintf(buf, "Share State: %s\n\r", edit_states[(*it)->editable]);
            strcat(string, buf);
          }
          sprintf(buf, "Description:\n%s\n\r", (*it)->description);
          strcat(string, buf);
          sprintf(buf, "Timeline:\n%s\n\r", (*it)->timeline);
          strcat(string, buf);
          page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
          return;
        }
        i++;
      }
      send_to_char("No such NPC in the gallery\n\r", ch);
    }
    else if (!str_cmp(arg1, "timeline")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: gallery timeline (number) (message)\n\r", ch);
        return;
      }
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 1) {
            (*it)->lastmodified = current_time;
            if (!str_cmp(argument, "edit")) {
              if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 2) {
                string_append(ch, &(*it)->timeline);
                return;
              }
              return;
            }
            sprintf(buf, "%s", newtimeline((*it)->timeline, argument));
            free_string((*it)->timeline);
            (*it)->timeline = str_dup(buf);
            send_to_char("Timeline appended.\n\r", ch);
            return;
          }
          send_to_char("You don't have the authority to append that timeline.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "share")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      int point = -1;
      if (!str_cmp(argument, edit_states[0]))
      point = 0;
      if (!str_cmp(argument, edit_states[1]))
      point = 1;
      if (!str_cmp(argument, edit_states[2]))
      point = 2;
      if (!str_cmp(argument, edit_states[3]))
      point = 3;
      if (point == -1) {
        send_to_char("Syntax: Gallery share (number) (None, Timeline, Details, Life and death)\n\r", ch);
        return;
      }
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 2) {
            (*it)->editable = point;
            send_to_char("Sharing changed.\n\r", ch);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "kill")) {
      int i = 1;
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 3) {
            (*it)->valid = FALSE;
            printf_to_char(ch, "You kill off %s.\n\r", (*it)->name);
            return;
          }
          send_to_char("You don't have the authority to do that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "territory")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2 || get_loc(argument) == NULL) {
        send_to_char("Syntax: gallery territory (number) (territory)\n\r", ch);
        return;
      }
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 2) {
            free_string((*it)->territory);
            (*it)->territory = str_dup(get_loc(argument)->name);
            printf_to_char(ch, "%s is now associated with %s.\n\r", (*it)->name, (*it)->territory);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "association")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2 || get_subfac(argument) == 0) {
        send_to_char("Syntax: gallery association (number) (sub faction)\n\r", ch);
        return;
      }
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 2) {
            (*it)->subfaction = get_subfac(argument);
            printf_to_char(ch, "%s is now associated with %s.\n\r", (*it)->name, get_subfac_name((*it)->subfaction));
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "name")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: gallery name (number) (new name)\n\r", ch);
        return;
      }
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 2) {
            free_string((*it)->name);
            (*it)->name = str_dup(argument);
            printf_to_char(ch, "You change their name to %s.\n\r", (*it)->name);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "description")) {
      int i = 1;
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 2) {
            string_append(ch, &(*it)->description);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else if (!str_cmp(arg1, "coauthors")) {
      int i = 1;
      for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
      it != GalleryVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors) || (*it)->editable >= 3) {
            string_append(ch, &(*it)->coauthors);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such NPC.\n\r", ch);
    }
    else
    send_to_char("Syntax: Gallery List/Info/Add/Timeline/Territory/Association/Name/Coauthors/Share/Description\n\r", ch);
  }

  void new_storyline_event(CHAR_DATA *sender, STORYLINE_TYPE *story, char *message) {
    CHAR_DATA *ch;
    char newmess[MSL];
    sprintf(newmess, "New Developments in %s: %s", story->name, message);
    story->lastmodified = current_time;
    if ((ch = get_char_world_pc(story->owner)) != NULL && ch != sender) {
      append_messages(ch, newmess);
      printf_to_char(ch, "%s\n\r", newmess);
    }
    else
    message_to_char(story->owner, newmess);

    char *temp = str_dup(story->coauthors);
    char arg1[MSL];
    temp = one_argument_nouncap(temp, arg1);
    if (safe_strlen(arg1) > 2) {
      for (int i = 0; safe_strlen(arg1) > 2 && i < 10; i++) {
        if ((ch = get_char_world_pc(arg1)) != NULL && ch != sender) {
          append_messages(ch, newmess);
          printf_to_char(ch, "%s\n\r", newmess);
        }
        else
        message_to_char(arg1, newmess);

        temp = one_argument_nouncap(temp, arg1);
      }
    }
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(story->participants[i]) > 1) {
        if ((ch = get_char_world_pc(story->participants[i])) != NULL && ch != sender) {
          append_messages(ch, newmess);
          printf_to_char(ch, "%s\n\r", newmess);
        }
        else
        message_to_char(story->participants[i], newmess);
      }
    }
  }
  void join_to_storyline(CHAR_DATA *ch, STORYLINE_TYPE *story) {
    story->lastmodified = current_time;

    if (!str_cmp(ch->name, story->owner))
    return;
    if (is_name(ch->name, story->coauthors))
    return;
    for (int i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, story->participants[i]))
      return;
    }
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(story->participants[i]) < 2) {
        free_string(story->participants[i]);
        story->participants[i] = str_dup(ch->name);
        printf_to_char(
        ch, "You have been invited into the storyline %s, if you don't want to be a part of this you can use storyline dropout to leave it.\n\r", story->name);
        return;
      }
    }
  }

  bool can_see_storyline(CHAR_DATA *ch, STORYLINE_TYPE *story) {

    if (story->power >= 12)
    return TRUE;
    if (!str_cmp(ch->name, story->owner))
    return TRUE;
    if (is_srname(ch->name) && !str_cmp(story->owner, nosr_name(ch->name)))
    return TRUE;
    if (!str_cmp(sr_name(ch->name), story->owner))
    return TRUE;
    if (is_name(ch->name, story->coauthors))
    return TRUE;
    if (is_srname(ch->name) && is_name(nosr_name(ch->name), story->coauthors))
    return TRUE;
    if (is_name(sr_name(ch->name), story->coauthors))

    return TRUE;
    for (int i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, story->participants[i]))
      return TRUE;
    }
    return FALSE;
  }
  _DOFUN(do_npcs)
  {
    if (ch->pcdata->ci_editing == 23) {
      string_append(ch, &ch->pcdata->ci_message);
      return;
    }
  }
  _DOFUN(do_lore)
  {
    if (ch->pcdata->ci_editing == 23) {
      string_append(ch, &ch->pcdata->ci_bystanders);
      return;
    }
  }
  _DOFUN(do_mastermind)
  {
    send_to_char("You cannot set the mastermind directly, that is based on the eidolon chosen.\n\r", ch);
    return;

    if (ch->pcdata->ci_editing == 23) {
      if(!character_exists(argument))
      {

        send_to_char("No such character.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->ci_taste);
      ch->pcdata->ci_taste = str_dup(argument);
      return;
    }
  }
  _DOFUN(do_subfactionone)
  {
    if (!valid_subfac(argument)) {
      send_to_char("Valid Subfactions are: ", ch);
      print_all_subfacs(ch);
      return;
    }
    free_string(ch->pcdata->ci_myself);
    ch->pcdata->ci_myself = str_dup(argument);
    send_to_char("Done.\n\r", ch);
    return;
  }
  _DOFUN(do_subfactiontwo)
  {
    if (!valid_subfac(argument)) {
      send_to_char("Valid Subfactions are: ", ch);
      print_all_subfacs(ch);
      return;
    }
    free_string(ch->pcdata->ci_target);
    ch->pcdata->ci_target = str_dup(argument);
    send_to_char("Done.\n\r", ch);
    return;
  }

  _DOFUN(do_storyline) {

    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2 && get_storyline(ch, argument) != NULL) {
        get_storyline(ch, argument)->lastmodified = current_time;
        free_string(ch->pcdata->ci_myselfdelayed);
        ch->pcdata->ci_myselfdelayed = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("No such storyline.\n\r", ch);
      return;
    }
    if (ch->pcdata->ci_editing == 23) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2 && get_storyline(ch, argument) != NULL) {
        get_storyline(ch, argument)->lastmodified = current_time;
        free_string(ch->pcdata->ci_short);
        ch->pcdata->ci_short = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("No such storyline.\n\r", ch);
      return;
    }

    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "add") || !str_cmp(arg1, "create")) {
      if (ch->played / 3600 < 100 && !is_gm(ch)) {
        send_to_char("You need at least 100 hours before you can make a storyline.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: Storyline add (title of storyline)\n\r", ch);
        return;
      }
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!str_cmp((*it)->name, argument)) {
          send_to_char("A Storyline with that title already exists.\n\r", ch);
          return;
        }
      }
      STORYLINE_TYPE *story = new_storyline();
      free_string(story->name);
      free_string(story->owner);
      story->owner = str_dup(ch->name);
      story->name = str_dup(capitalize(argument));
      story->valid = TRUE;
      StorylineVect.push_back(story);
      printf_to_char(ch, "You create a new storyline titled: %s.", story->name);
      return;
    }
    else if (!str_cmp(arg1, "list")) {
      int i = 1;
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        sprintf(buf, "`W%2d`c)`x %s \t\t`c[`x%s`c]`x\n\r", i, (*it)->name, (*it)->owner);
        strcat(string, buf);
        i++;
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      return;
    }
    else if (!str_cmp(arg1, "info")) {
      int i = 1;
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          sprintf(buf, "`W%2d`c)`x %s \t\t`c[`x%s`c]`x\n", i, (*it)->name, (*it)->owner);
          strcat(string, buf);
          if (IS_IMMORTAL(ch) || (safe_strlen((*it)->coauthors) > 2 && is_name(ch->name, (*it)->coauthors)) || !str_cmp((*it)->owner, ch->name)) {
            sprintf(buf, "Power: %d%%\n\r", (*it)->power * 100 / 15);
            strcat(string, buf);
            sprintf(buf, "Co-Authors: %s\n\r", (*it)->coauthors);
            strcat(string, buf);
          }
          sprintf(buf, "Description: %s\n\r", (*it)->description);
          strcat(string, buf);
          for (int j = 0; j < 10; j++) {
            if ((*it)->mastermind_power[j] >= 10) {
              sprintf(buf, "Mastermind: %s\n\r", (*it)->masterminds[j]);
              strcat(string, buf);
            }
          }
          sprintf(buf, "Timeline:\n%s\n\r", (*it)->timeline);
          strcat(string, buf);
          if (IS_IMMORTAL(ch) || (safe_strlen((*it)->coauthors) > 2 && is_name(ch->name, (*it)->coauthors)) || !str_cmp((*it)->owner, ch->name)) {
            sprintf(buf, "Participants:\n\r");
            strcat(string, buf);
            for (int i = 0; i < 100; i++) {
              if (safe_strlen((*it)->participants[i]) > 2) {
                sprintf(buf, "%s\n\r", (*it)->participants[i]);
                strcat(string, buf);
              }
            }
          }
          page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
          return;
        }
        i++;
      }
      send_to_char("No such storyline\n\r", ch);
    }
    else if (!str_cmp(arg1, "timeline")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: storyline timeline (number) (message)\n\r", ch);
        return;
      }
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            if (!str_cmp(argument, "edit")) {
              if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
                string_append(ch, &(*it)->timeline);
                return;
              }
              return;
            }
            sprintf(buf, "%s", newtimeline((*it)->timeline, argument));
            free_string((*it)->timeline);
            (*it)->timeline = str_dup(buf);
            send_to_char("Timeline appended.\n\r", ch);
            new_storyline_event(ch, (*it), argument);
            return;
          }
          send_to_char("You don't have the authority to append that timeline.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "finish")) {
      int i = 1;
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            (*it)->valid = FALSE;
            printf_to_char(ch, "You finish %s.\n\r", (*it)->name);
            return;
          }
          send_to_char("You don't have the authority to do that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "title")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: storyline title (number) (new name)\n\r", ch);
        return;
      }
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            free_string((*it)->name);
            (*it)->name = str_dup(capitalize(argument));
            printf_to_char(ch, "You change the title to %s.\n\r", (*it)->name);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "ascend")) {
      argument = one_argument_nouncap(argument, arg2);
      if(!str_cmp(arg2, "eidilon") || !str_cmp(arg2, "eidolon"))
      {
        argument = one_argument_nouncap(argument, arg3);
        int i = 1;
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: storyline ascend eidolon (storyline number) (Eidolon Name)\n\r", ch);
          return;
        }
        FACTION_TYPE *cult = clan_lookup(ch->fcult);
        if(cult == NULL || !is_leader(ch, ch->fcult))
        {
          send_to_char("You must be a leader of your cult to do that.\n\r", ch);
          return;
        }
        if(str_cmp(cult->eidilon, argument))
        {
          send_to_char("That is not your cult's eidolon.\n\r", ch);
          return;
        }
        int bases = 0;
        for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
        it != locationVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if ((*it)->base_faction_core == cult->vnum || (*it)->base_faction_cult == cult->vnum || (*it)->base_faction_sect == cult->vnum) {
            bases += 1;
          }
        }
        if(bases < 3)
        {
          send_to_char("Your cult must control at least three territories first.\n\r", ch);
          return;
        }

        for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
        it != StorylineVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if (!can_see_storyline(ch, (*it)))
          continue;

          if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
            if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {

              if ((*it)->total_power < 20) {
                printf_to_char(ch, "That storyline is only %d%% as powerful as it needs to be.\n\r", (*it)->total_power * 100 / 20);
                return;
              }
              bool mfound = FALSE;
              for (int i = 0; i < 10; i++) {
                if (!str_cmp(ch->name, (*it)->masterminds[i]) && (*it)->mastermind_power[i] >= 15)
                mfound = TRUE;
              }
              if (mfound == FALSE) {
                for (int i = 0; i < 10; i++) {
                  if (!str_cmp(ch->name, (*it)->masterminds[i])) {
                    printf_to_char(ch, "That mastermind is only %d%% as powerful as they need to be.\n\r", (*it)->mastermind_power[i] * 100 / 5);
                    return;
                  }
                }
                send_to_char("That mastermind isn't powerful enough yet.\n\r", ch);
                return;
              }
              CHAR_DATA *victim = get_char_world_pc(cult->eidilon);
              if(victim != NULL)
              {
                room_purge(victim);
                real_quit(victim);
              }
              char strsave[MAX_INPUT_LENGTH];
              sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(cult->eidilon));
              unlink(strsave);
              cult->resource = 0;

              for (int i = 0; i < 99 && safe_strlen(cult->member_names[i]) > 1; i++) {
                CHAR_DATA *member = get_char_world_pc(cult->member_names[i]);
                if(member != NULL)
                {
                  remove_from_clanroster(member->name, cult->vnum);
                }
                free_string(cult->member_names[i]);
                cult->member_names[i] = str_dup("");
              }


              for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
              it != locationVect.end(); ++it) {
                if (!(*it)->name || (*it)->name[0] == '\0') {
                  continue;
                }
                if ((*it)->valid == FALSE)
                continue;

                if((*it)->base_faction_cult == cult->vnum)
                {
                  char nout[MSL];
                  sprintf(nout, "%s\n\n%s", (*it)->fixtures, (*it)->base_desc_cult);
                  free_string((*it)->fixtures);
                  (*it)->fixtures = str_dup(nout);
                  free_string((*it)->base_desc_cult);
                  (*it)->base_desc_cult = str_dup("");
                  (*it)->base_faction_cult = 0;
                }
              }
              FACTION_TYPE *sect = NULL;
              for (vector<FACTION_TYPE *>::iterator ij = FacVect.begin();
              ij != FacVect.end(); ++ij) {
                if((*ij)->type == FACTION_SECT && !str_cmp((*ij)->eidilon, cult->eidilon))
                {
                  sect = *ij;
                  break;
                }
              }

              if(sect != NULL)
              {
                sect->resource = 0;

                for (int i = 0; i < 99 && safe_strlen(sect->member_names[i]) > 1; i++) {
                  CHAR_DATA *member = get_char_world_pc(sect->member_names[i]);
                  if(member != NULL)
                  {
                    remove_from_clanroster(member->name, sect->vnum);
                  }
                  free_string(sect->member_names[i]);
                  sect->member_names[i] = str_dup("");
                }


                for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
                it != locationVect.end(); ++it) {
                  if (!(*it)->name || (*it)->name[0] == '\0') {
                    continue;
                  }
                  if ((*it)->valid == FALSE)
                  continue;

                  if((*it)->base_faction_sect == sect->vnum)
                  {
                    free_string((*it)->base_desc_sect);
                    (*it)->base_desc_sect = str_dup("");
                    (*it)->base_faction_sect = 0;
                  }
                }

              }


              for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
              it != DomainVect.end(); ++it) {
                if (!(*it)->domain_of || (*it)->domain_of[0] == '\0') {
                  bug("Save_houses: Blank domain in vector", i);
                  continue;
                }
                if(!str_cmp((*it)->domain_of, cult->eidilon))
                {
                  (*it)->valid = FALSE;
                }
              }


              int initid = 0;
              for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
              it != AnniversaryVect.end(); ++it) {
                if ((*it)->id > initid)
                initid = (*it)->id;
              }
              ANNIVERSARY_TYPE *ann = new_anniversary();
              ann->id = initid + 1;
              ann->btype = 1;
              ann->happen_at = current_time;
              free_string(ann->eidilon_name);
              ann->eidilon_name = str_dup(cult->eidilon);
              free_string(ann->cult_name);
              ann->cult_name = str_dup(cult->name);
              ann->valid = TRUE;
              AnniversaryVect.push_back(ann);
              save_anniversaries();
              save_clans(FALSE);
              ch->pcdata->ci_editing = 20;
              ch->pcdata->ci_absorb = 1;
              ch->pcdata->ci_discipline = initid+1;
              send_to_char("Your cult ascends.\n\r", ch);
              return;

            }
            send_to_char("You don't have the authority to do that.\n\r", ch);
          }
          i++;
        }


      }
      else
      {
        int i = 1;
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: storyline ascend (number) (character)\n\r", ch);
          return;
        }
        for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
        it != StorylineVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if (!can_see_storyline(ch, (*it)))
          continue;

          if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
            if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
              CHAR_DATA *victim = get_char_world_pc(argument);
              if ((*it)->power < 30) {
                printf_to_char(ch, "That storyline is only %d%% as powerful as it needs to be.\n\r", (*it)->power * 100 / 30);
                return;
              }
              if (victim == NULL) {
                send_to_char("No one like that around to ascend.\n\r", ch);
                return;
              }
              bool mfound = FALSE;
              for (int i = 0; i < 10; i++) {
                if (!str_cmp(victim->name, (*it)->masterminds[i]) && (*it)->mastermind_power[i] >= 15)
                mfound = TRUE;
              }
              if (mfound == FALSE) {
                for (int i = 0; i < 10; i++) {
                  if (!str_cmp(victim->name, (*it)->masterminds[i])) {
                    printf_to_char(ch, "That mastermind is only %d%% as powerful as they need to be.\n\r", (*it)->mastermind_power[i] * 100 / 15);
                    return;
                  }
                }
                send_to_char("That mastermind isn't powerful enough yet.\n\r", ch);
                return;
              }
              if (IS_FLAG(victim->act, PLR_ASCENDING)) {
                send_to_char("They are already ascending.\n\r", ch);
                return;
              }
              for (int i = 0; i < 10; i++) {
                if (!str_cmp(victim->name, (*it)->masterminds[i]))
                (*it)->mastermind_power[i] -= 15;
              }

              SET_FLAG(victim->act, PLR_ASCENDING);
              (*it)->power -= 30;
              act("You grant $N the power to ascend.", ch, NULL, victim, TO_CHAR);
              send_to_char("You are given the power to ascend.\n\r", victim);
              return;
            }
            send_to_char("You don't have the authority to do that.\n\r", ch);
          }
          i++;
        }
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "defeat")) {
      argument = one_argument_nouncap(argument, arg2);
      if(!str_cmp(arg2, "eidilon") || !str_cmp(arg2, "eidolon"))
      {
        argument = one_argument_nouncap(argument, arg3);
        int i = 1;
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: storyline defeat eidolon (storyline number) (Eidolon Name)\n\r", ch);
          return;
        }
        FACTION_TYPE *sect = clan_lookup(ch->fsect);
        if(sect == NULL || !is_leader(ch, ch->fsect))
        {
          send_to_char("You must be a leader of your sect to do that.\n\r", ch);
          return;
        }
        if(str_cmp(sect->eidilon, argument))
        {
          send_to_char("That is not your sect's eidolon.\n\r", ch);
          return;
        }
        int bases = 0;
        for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
        it != locationVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if ((*it)->base_faction_core == sect->vnum || (*it)->base_faction_cult == sect->vnum || (*it)->base_faction_sect == sect->vnum) {
            bases += 1;
          }
        }
        if(bases < 3)
        {
          send_to_char("Your sect must control at least three territories first.\n\r", ch);
          return;
        }

        for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
        it != StorylineVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if (!can_see_storyline(ch, (*it)))
          continue;

          if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
            if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {

              if ((*it)->total_power < 20) {
                printf_to_char(ch, "That storyline is only %d%% as powerful as it needs to be.\n\r", (*it)->total_power * 100 / 20);
                return;
              }

              bool mfound = FALSE;
              for (int i = 0; i < 10; i++) {
                if (!str_cmp(ch->name, (*it)->masterminds[i]) && (*it)->mastermind_power[i] >= 15)
                mfound = TRUE;
              }
              if (mfound == FALSE) {
                for (int i = 0; i < 10; i++) {
                  if (!str_cmp(ch->name, (*it)->masterminds[i])) {
                    printf_to_char(ch, "That mastermind is only %d%% as powerful as they need to be.\n\r", (*it)->mastermind_power[i] * 100 / 5);
                    return;
                  }
                }
                send_to_char("That mastermind isn't powerful enough yet.\n\r", ch);
                return;
              }
              CHAR_DATA *victim = get_char_world_pc(sect->eidilon);
              if(victim != NULL)
              {
                room_purge(victim);
                real_quit(victim);
              }
              char strsave[MAX_INPUT_LENGTH];
              sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(sect->eidilon));
              unlink(strsave);
              sect->resource = 0;

              for (int i = 0; i < 99 && safe_strlen(sect->member_names[i]) > 1; i++) {
                CHAR_DATA *member = get_char_world_pc(sect->member_names[i]);
                if(member != NULL)
                {
                  remove_from_clanroster(member->name, sect->vnum);
                }
                free_string(sect->member_names[i]);
                sect->member_names[i] = str_dup("");
              }


              for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
              it != locationVect.end(); ++it) {
                if (!(*it)->name || (*it)->name[0] == '\0') {
                  continue;
                }
                if ((*it)->valid == FALSE)
                continue;

                if((*it)->base_faction_sect == sect->vnum)
                {
                  char nout[MSL];
                  sprintf(nout, "%s\n\n%s", (*it)->fixtures, (*it)->base_desc_sect);
                  free_string((*it)->fixtures);
                  (*it)->fixtures = str_dup(nout);
                  free_string((*it)->base_desc_sect);
                  (*it)->base_desc_sect = str_dup("");
                  (*it)->base_faction_sect = 0;
                }
              }
              FACTION_TYPE *cult = NULL;
              for (vector<FACTION_TYPE *>::iterator ij = FacVect.begin();
              ij != FacVect.end(); ++ij) {
                if((*ij)->type == FACTION_CULT && !str_cmp((*ij)->eidilon, sect->eidilon))
                {
                  cult = *ij;
                  break;
                }
              }

              if(cult != NULL)
              {
                cult->resource = 0;

                for (int i = 0; i < 99 && safe_strlen(cult->member_names[i]) > 1; i++) {
                  CHAR_DATA *member = get_char_world_pc(cult->member_names[i]);
                  if(member != NULL)
                  {
                    remove_from_clanroster(member->name, cult->vnum);
                  }
                  free_string(cult->member_names[i]);
                  cult->member_names[i] = str_dup("");
                }


                for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
                it != locationVect.end(); ++it) {
                  if (!(*it)->name || (*it)->name[0] == '\0') {
                    continue;
                  }
                  if ((*it)->valid == FALSE)
                  continue;

                  if((*it)->base_faction_cult == cult->vnum)
                  {
                    free_string((*it)->base_desc_cult);
                    (*it)->base_desc_cult = str_dup("");
                    (*it)->base_faction_cult = 0;
                  }
                }

              }


              for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
              it != DomainVect.end(); ++it) {
                if (!(*it)->domain_of || (*it)->domain_of[0] == '\0') {
                  bug("Save_houses: Blank domain in vector", i);
                  continue;
                }
                if(!str_cmp((*it)->domain_of, cult->eidilon))
                {
                  (*it)->valid = FALSE;
                }
              }

              int initid = 0;
              for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
              it != AnniversaryVect.end(); ++it) {
                if ((*it)->id > initid)
                initid = (*it)->id;
              }
              ANNIVERSARY_TYPE *ann = new_anniversary();
              ann->id = initid + 1;
              ann->btype = 2;
              ann->happen_at = current_time;
              free_string(ann->eidilon_name);
              ann->eidilon_name = str_dup(sect->eidilon);
              free_string(ann->sect_name);
              ann->sect_name = str_dup(sect->name);
              ann->valid = TRUE;
              AnniversaryVect.push_back(ann);
              save_anniversaries();
              save_clans(FALSE);
              ch->pcdata->ci_editing = 20;
              ch->pcdata->ci_absorb = 1;
              ch->pcdata->ci_discipline = initid+1;
              send_to_char("Your sect defeats their enemy.\n\r", ch);
              return;

            }
            send_to_char("You don't have the authority to do that.\n\r", ch);
          }
          i++;
        }


      }
    }
    else if (!str_cmp(arg1, "tyrant")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: storyline tyrant (number) (character)\n\r", ch);
        return;
      }
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            CHAR_DATA *victim = get_char_world_pc(argument);
            if ((*it)->power < 50) {
              printf_to_char(ch, "That storyline is only %d%% as powerful as it needs to be.\n\r", (*it)->power * 100 / 50);
              return;
            }
            if (victim == NULL) {
              send_to_char("No one like that around.\n\r", ch);
              return;
            }

            bool mfound = FALSE;
            for (int i = 0; i < 10; i++) {
              if (!str_cmp(victim->name, (*it)->masterminds[i]) && (*it)->mastermind_power[i] >= 15)
              mfound = TRUE;
            }
            if (mfound == FALSE) {
              for (int i = 0; i < 10; i++) {
                if (!str_cmp(victim->name, (*it)->masterminds[i])) {
                  printf_to_char(ch, "That mastermind is only %d%% as powerful as they need to be.\n\r", (*it)->mastermind_power[i] * 100 / 15);
                  return;
                }
              }
              send_to_char("That mastermind isn't powerful enough yet.\n\r", ch);
              return;
            }
            if (IS_FLAG(victim->act, PLR_TYRANT)) {
              send_to_char("They are already a tyrant.\n\r", ch);
              return;
            }
            for (int i = 0; i < 10; i++) {
              if (!str_cmp(victim->name, (*it)->masterminds[i]))
              (*it)->mastermind_power[i] -= 15;
            }
            SET_FLAG(victim->act, PLR_TYRANT);
            (*it)->power -= 30;
            act("You grant $N the power of a tyrant.", ch, NULL, victim, TO_CHAR);
            send_to_char("You are given the power of a tyrant.\n\r", victim);
            NEWS_TYPE *news;
            news = new_news();
            free_string(news->author);
            news->author = str_dup("Supernatural News");
            news->timer = 2000;
            free_string(news->message);
            sprintf(buf, "Panicked rumors in the supernatural underworld are that %s has obtained Tyranical power over the local goverment and supernatural forces.", victim->name);
            news->message = str_dup(buf);
            news->stats[0] = -2;
            NewsVect.push_back(news);
            return;
          }
          send_to_char("You don't have the authority to do that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "description")) {
      int i = 1;
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            string_append(ch, &(*it)->description);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "coauthors")) {
      int i = 1;
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            string_append(ch, &(*it)->coauthors);
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "dropout")) {
      int i = 1;
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(argument, (*it)->name) || atoi(argument) == i) {
          for (int i = 0; i < 100; i++) {
            if (!str_cmp(ch->name, (*it)->participants[i])) {
              free_string((*it)->participants[i]);
              (*it)->participants[i] = str_dup("");
              printf_to_char(ch, "You drop out of %s.\n\r", (*it)->name);
              return;
            }
          }
          send_to_char("You aren't in that storyline.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "invite")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: storyline invite (number) room/(name)\n\r", ch);
        return;
      }
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            if (!str_cmp(argument, "room")) {
              CHAR_DATA *victim;
              for (CharList::iterator ij = ch->in_room->people->begin();
              ij != ch->in_room->people->end(); ++ij) {
                victim = *ij;
                if (victim == NULL)
                continue;
                if (victim->in_room == NULL)
                continue;
                if (IS_NPC(victim))
                continue;
                join_to_storyline(victim, (*it));
              }
              send_to_char("You invite the room into your storyline.\n\r", ch);
              return;
            }
            else {
              CHAR_DATA *victim;
              if ((victim = get_char_world(ch, argument)) != NULL) {
                if (get_gmtrust(ch, victim) >= 0) {
                  join_to_storyline(victim, (*it));
                  printf_to_char(ch, "You invite %s into your storyline.\n\r", PERS(victim, ch));
                  return;
                }
              }
              send_to_char("No such player around who can be invited.\n\r", ch);
            }
            return;
          }
          send_to_char("You don't have the authority to change that.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else if (!str_cmp(arg1, "message")) {
      argument = one_argument_nouncap(argument, arg2);
      int i = 1;
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: storyline message (number) (new message to send the participants)\n\r", ch);
        return;
      }
      for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
      it != StorylineVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!can_see_storyline(ch, (*it)))
        continue;

        if (!str_cmp(arg2, (*it)->name) || atoi(arg2) == i) {
          if (IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->owner) || is_name(ch->name, (*it)->coauthors)) {
            new_storyline_event(ch, (*it), argument);
            printf_to_char(
            ch, "You send those involved in this storyline the message: %s.\n\r", argument);
            return;
          }
          send_to_char("You don't have authority to send messages for this storyline.\n\r", ch);
        }
        i++;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    else
    send_to_char("Syntax: Storyline List/Info/Add/Timeline/Title/Coauthors/Description/Message/Dropout\n\r", ch);
  }

  STORYLINE_TYPE *get_storyline(CHAR_DATA *ch, char *name) {
    if (safe_strlen(name) < 2)
    return NULL;
    for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
    it != StorylineVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (ch != NULL) {
        if (!can_see_storyline(ch, (*it)))
        continue;
      }
      if (!str_cmp(name, (*it)->name))
      return (*it);
    }
    for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
    it != StorylineVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (ch != NULL) {
        if (!can_see_storyline(ch, (*it)))
        continue;
      }
      if (strcasestr(name, (*it)->name) != NULL)
      return (*it);
      if (strcasestr((*it)->name, name) != NULL)
      return (*it);
    }
    return NULL;
  }

  vector<ARCHIVE_INDEX *> ArchiveVect;
  ARCHIVE_INDEX *nullarchive;

  void fread_archive_index(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    ARCHIVE_INDEX *arch;

    arch = new_archive_index();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'E':
        if (!str_cmp(word, "End")) {
          ArchiveVect.push_back(arch);
          return;
        }
        break;
      case 'K':
        KEY("Keywords", arch->keywords, fread_string(fp));
        break;
      case 'N':
        KEY("Number", arch->number, fread_number(fp));
        break;
      case 'T':
        KEY("Title", arch->title, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_archive_index: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_archive_indexes() {
    nullarchive = new_archive_index();
    FILE *fp;

    if ((fp = fopen("../data/archives/archive.index", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_archive_indexes: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "INDEX")) {
          fread_archive_index(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_archive_index: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open archive.index", 0);
      exit(0);
    }
  }
  void save_archive_indexes(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/archives/back1/archive.index");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/archives/back2/archive.index");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/archives/back3/archive.index");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/archives/back4/archive.index");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/archives/back5/archive.index");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/archives/back6/archive.index");
      else
      sprintf(buf, "../data/archives/back7/archive.index");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open backup archive.index for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/archives/archive.index", "w")) == NULL) {
        bug("Cannot open archive.index for writing", 0);
        return;
      }
    }

    for (vector<ARCHIVE_INDEX *>::iterator it = ArchiveVect.begin();
    it != ArchiveVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      fprintf(fpout, "#INDEX\n");
      fprintf(fpout, "Title %s~\n", (*it)->title);
      fprintf(fpout, "Keywords %s~\n", (*it)->keywords);
      fprintf(fpout, "Number %ld\n", (*it)->number);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_archive_indexes(TRUE);
    }
  }

  ARCHIVE_ENTRY *load_archive_entry(long number) {
    FILE *fp;
    char file[MSL];
    sprintf(file, "../data/archives/%ld", number);
    if ((fp = fopen(file, "r")) != NULL) {
      char buf[MSL];
      const char *word;
      bool fMatch;
      ARCHIVE_ENTRY *arch;

      arch = new_archive_entry();

      for (;;) {
        word = feof(fp) ? "End" : fread_word(fp);
        fMatch = FALSE;

        switch (UPPER(word[0])) {
        case '*':
          fMatch = TRUE;
          fread_to_eol(fp);
          break;

        case 'E':
          if (!str_cmp(word, "End")) {
            fclose(fp);
            return arch;
          }
          break;
        case 'N':
          KEY("Number", arch->number, fread_number(fp));
          break;
        case 'O':
          KEY("Owner", arch->owner, fread_string(fp));
          break;
        case 'T':
          KEY("Title", arch->title, fread_string(fp));
          KEY("Text", arch->text, fread_string(fp));

          break;
        }

        if (!fMatch) {
          sprintf(buf, "Load archive entry: no match: %s", word);
          bug(buf, 0);
        }
      }
      fclose(fp);
    }
    return NULL;
  }

  void save_archive_entry(ARCHIVE_ENTRY *arch) {
    char file[MSL];
    FILE *fpout;
    sprintf(file, "../data/archives/%ld", arch->number);
    if ((fpout = fopen(file, "w")) == NULL) {
      bug("Cannot open archive entry for writing", 0);
      return;
    }

    fprintf(fpout, "Title %s~\n", arch->title);
    fprintf(fpout, "Owner %s~\n", arch->owner);
    fprintf(fpout, "Number %ld\n", arch->number);
    fprintf(fpout, "Text %s~\n", arch->text);
    fprintf(fpout, "End\n\n");

    fclose(fpout);
  }

  _DOFUN(do_keywords) {
    if (ch->pcdata->ci_editing == 8) {
      ch->pcdata->ci_absorb = 1;
      free_string(ch->pcdata->ci_taste);
      ch->pcdata->ci_taste = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      return;
    }
  }

  void lookup_research(CHAR_DATA *ch, char *argument) {
    long value = atol(argument);

    bool found = FALSE;
    for (vector<ARCHIVE_INDEX *>::iterator it = ArchiveVect.begin();
    it != ArchiveVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }

      if (value == (*it)->number) {
        found = TRUE;
        ARCHIVE_ENTRY *archive_entry = load_archive_entry(value);
        if (archive_entry != NULL) {
          printf_to_char(ch, "Archive Index: %ld\nTitle: %s\nText:\n%s\n\r", value, archive_entry->title, archive_entry->text);
        }
      }
    }
    if (found == TRUE)
    return;

    for (vector<ARCHIVE_INDEX *>::iterator it = ArchiveVect.begin();
    it != ArchiveVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }

      if (is_name(argument, (*it)->keywords)) {
        found = TRUE;
        printf_to_char(ch, "Archive Index: %ld\nTitle: %s\n\r", (*it)->number, (*it)->title);
      }
    }
    if (found == FALSE)
    send_to_char("No entry found.\n\r", ch);
  }

  void delete_research(CHAR_DATA *ch, char *argument) {
    long value = atol(argument);
    for (vector<ARCHIVE_INDEX *>::iterator it = ArchiveVect.begin();
    it != ArchiveVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }

      if (value == (*it)->number) {
        ARCHIVE_ENTRY *archive_entry = load_archive_entry(value);
        if (archive_entry != NULL && (!str_cmp(ch->name, archive_entry->owner) || IS_IMMORTAL(ch))) {
          char buf[MSL];
          sprintf(buf, "../data/archives/%ld", value);
          unlink(buf);
          (*it)->valid = FALSE;
          send_to_char("Entry deleted.\n\r", ch);
          save_archive_indexes(FALSE);
          return;
        }
      }
    }
    send_to_char("No such entry found.\n\r", ch);
  }

  _DOFUN(do_research) {
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);

    if (!str_cmp(arg, "delete")) {
      delete_research(ch, argument);
      return;
    }
    if (!str_cmp(arg, "make")) {
      do_function(ch, &do_makeroom, "research");
      return;
    }
    if (!library_room(ch->in_room)) {
      send_to_char("You need to be in a library to do that.\n\r", ch);
      return;
    }

    ch->pcdata->process = PROCESS_RESEARCH;
    ch->pcdata->process_target = NULL;

    free_string(ch->pcdata->process_argumentone);
    ch->pcdata->process_argumentone = str_dup(arg);

    if (!IS_IMMORTAL(ch)) {
      if (is_number(arg))
      ch->pcdata->process_timer = 5;
      else
      ch->pcdata->process_timer = 45 - get_skill(ch, SKILL_EDUCATION) * 5;
    }
    else {
      conclude_process(ch, PROCESS_RESEARCH);
      return;
    }

    if (get_skill(ch, SKILL_OCCULTFOCUS) > 0)
    ch->pcdata->process_timer -= 10 * get_skill(ch, SKILL_OCCULTFOCUS);
    send_to_char("You start to research.\n\r", ch);
    act("$n starts researching.", ch, NULL, NULL, TO_ROOM);
    return;
  }

  char *sr_name(char *oldname) {
    char buf[MSL];
    sprintf(buf, "SR%s", oldname);
    return str_dup(buf);
  }
  char *nosr_name(char *oldname) {
    char tmp[MSL];
    char buf[MSL];
    char blah[MSL];
    buf[0] = 0;
    sprintf(tmp, "%s", oldname);
    for (int i = 2; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
      sprintf(blah, "%c", tmp[i]);
      strcat(buf, blah);
    }
    return str_dup(buf);
  }

  void init_char(CHAR_DATA *ch) {
    ch->race = RACE_NEWFAEBORN;
    ch->version = CURR_VERSION;
    SET_FLAG(ch->comm, COMM_NOGUIDE);
    ch->pcdata->version_player = 2;
    do_function(ch, &do_prompt, "starter");
    if (IS_FLAG(ch->comm, COMM_NONEWBIE))
    REMOVE_FLAG(ch->comm, COMM_NONEWBIE);
    if (IS_FLAG(ch->comm, COMM_NOGUIDE))
    REMOVE_FLAG(ch->comm, COMM_NOGUIDE);
    ch->pcdata->height_inches = 7;
    ch->pcdata->height_feet = 5;
    ch->pcdata->birth_year = 1980;
    ch->pcdata->birth_month = 1;
    ch->pcdata->birth_day = 1;
    ch->pcdata->lfcount = 1;
    ch->pcdata->lftotal = 100;
    if (!IS_FLAG(ch->comm, COMM_PROMPT))
    SET_FLAG(ch->comm, COMM_PROMPT);

    if (!IS_FLAG(ch->comm, COMM_STORY))
    SET_FLAG(ch->comm, COMM_STORY);

    if (IS_FLAG(ch->comm, COMM_CONSENT))
    REMOVE_FLAG(ch->comm, COMM_CONSENT);

    if (!IS_FLAG(ch->comm, COMM_SUBDUE))
    SET_FLAG(ch->comm, COMM_SUBDUE);
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    ch->pcdata->create_date = current_time;
    ch->pcdata->last_paid = current_time;

    ch->level = 1;
    char_list.push_front(ch);
    ch->desc->connected = CON_PLAYING;
    if (number_percent() % 2 == 0)
    ch->sex = SEX_MALE;
    else
    ch->sex = SEX_FEMALE;
    free_string(ch->pcdata->intro_desc);
    ch->pcdata->intro_desc = str_dup("A storyrunner");

    ch->pcdata->true_level = ch->level;
  }

  void convert_to_sr(CHAR_DATA *ch) {
    log_string("DESCRIPTOR: Convert to SR");

    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->act, PLR_HIDE);
    bool oconsent = IS_FLAG(ch->comm, COMM_CONSENT);

    bool color = IS_FLAG(ch->act, PLR_COLOR);
    int linewidth = ch->linewidth;
    int lines = ch->lines;
    account_sync(ch);
    ROOM_INDEX_DATA *oldroom = ch->in_room;
    save_char_obj(ch, FALSE, FALSE);
    char name[MSL];
    sprintf(name, "%s", sr_name(ch->name));

    DESCRIPTOR_DATA *d = ch->desc;
    quit_process(ch);
    extract_char(ch, TRUE);
    load_char_obj(d, name);
    CHAR_DATA *nch;
    nch = d->character;
    d->connected = CON_PLAYING;
    nch->desc->pEdit = (void *)nch;
    nch->pcdata->account = d->account;
    char_to_room(nch, oldroom);
    init_char(nch);
    if(oconsent == TRUE)
    SET_FLAG(ch->comm, COMM_CONSENT);

    /*
free_string(ch->name);
ch->name = str_dup(name);
ch->faction = 0;
ch->factiontwo = 0;
free_string(ch->short_descr);
ch->short_descr = str_dup(name);
free_string(ch->long_descr);
ch->long_descr = str_dup(name);
*/
    if (color == TRUE && !IS_FLAG(nch->act, PLR_COLOR))
    SET_FLAG(nch->act, PLR_COLOR);
    if (color == FALSE && IS_FLAG(nch->act, PLR_COLOR))
    REMOVE_FLAG(nch->act, PLR_COLOR);
    nch->linewidth = linewidth;
    nch->lines = lines;
    nch->played = 75 * 3600;
    SET_FLAG(nch->act, PLR_GM);
    if (IS_FLAG(nch->act, PLR_BOUND))
    REMOVE_FLAG(nch->act, PLR_BOUND);
    if (IS_FLAG(nch->act, PLR_BOUNDFEET))
    REMOVE_FLAG(nch->act, PLR_BOUNDFEET);

    if (IS_FLAG(nch->act, PLR_GHOST))
    REMOVE_FLAG(nch->act, PLR_GHOST);
    if (IS_FLAG(nch->act, PLR_GHOSTWALKING))
    REMOVE_FLAG(nch->act, PLR_GHOSTWALKING);
    /*
OBJ_DATA *obj;
OBJ_DATA *obj_next;
for ( obj = ch->carrying; obj != NULL; obj = obj_next )
{
obj_next = obj->next_content;
if(obj->wear_loc == WEAR_NONE)
extract_obj(obj);
}
*/
    ch->pcdata->total_credit = 10000000;

    send_to_char("You become an SR.\n\r", ch);
  }

  void revert_from_sr(CHAR_DATA *ch) {
    account_sync(ch);
    char name[MSL];
    sprintf(name, "%s", nosr_name(ch->name));
    save_account(ch->pcdata->account, FALSE);
    if (ch->pcdata->encounter_status > 0 && ch->pcdata->encounter_sr != NULL) {
      ch->pcdata->encounter_sr->pcdata->encounter_status = 0;
      ch->pcdata->encounter_sr->pcdata->encounter_sr = NULL;
      ch->pcdata->encounter_status = 0;
      ch->pcdata->encounter_sr = NULL;
    }
    char strsave[MSL];
    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
    //    if(get_char_world_pc(name) != NULL)
    //    {
    send_to_char("You log off.\n\r", ch);
    real_quit(ch);
    unlink(strsave);
    return;
    //    }
    DESCRIPTOR_DATA *d = ch->desc;
    free_char(ch);
    load_char_obj(d, name);
    if (d->character->in_room != NULL) {
      ROOM_INDEX_DATA *room = d->character->in_room;
      char_from_room(d->character);
      char_to_room(d->character, room);
    }
    send_to_char("You return to your regular character.\n\r", d->character);
    unlink(strsave);
  }

  bool is_srname(char *name) {
    char buf[MSL];
    sprintf(buf, "%s", name);
    if (buf[0] == 'S' && buf[1] == 'R')
    return TRUE;
    return FALSE;
  }

  bool cross_sr_compare(char *nameone, char *nametwo) {
    if (!str_cmp(nameone, nametwo))
    return TRUE;

    if (!str_cmp(sr_name(nameone), nametwo))
    return TRUE;

    if (!str_cmp(sr_name(nametwo), nameone))
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_timeline) {
    char buf[MSL];
    if (safe_strlen(argument) < 2) {
      sprintf(buf, "`WTimeline`x\n%s\n\r", ch->pcdata->char_timeline);
      page_to_char(buf, ch);
      return;
    }
    if (!str_cmp(argument, "edit")) {
      string_append(ch, &ch->pcdata->char_timeline);
      return;
    }
    sprintf(buf, "%s", newtimeline(ch->pcdata->char_timeline, argument));
    free_string(ch->pcdata->char_timeline);
    ch->pcdata->char_timeline = str_dup(buf);
    printf_to_char(ch, "%s added to your timeline.\n\r", argument);
  }

  _DOFUN(do_wildsapprove) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Wildsapprove whom?", ch);
      return;
    }

    if (!IS_IMMORTAL(ch)) {
      send_to_char("You haven't been approved for that yourself.\n\r", ch);
      return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->pcdata->account == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    if (IS_FLAG(victim->pcdata->account->flags, ACCOUNT_NOOFFWORLD)) {
      REMOVE_FLAG(victim->pcdata->account->flags, ACCOUNT_NOOFFWORLD);
      send_to_char("Offworld SRing restored.\n\r", ch);
      return;
    }
    else {
      SET_FLAG(victim->pcdata->account->flags, ACCOUNT_NOOFFWORLD);
      send_to_char("Offworld SRing suspended.\n\r", ch);
      return;
    }
  }

  vector<FANTASY_TYPE *> FantasyVect;
  FANTASY_TYPE *nullfantasy;

  void fread_fantasy(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    FANTASY_TYPE *fantasy;
    fantasy = new_fantasy();
    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'A':
        KEY("Author", fantasy->author, fread_string(fp));
        if (!str_cmp(word, "Active")) {
          int num = fread_number(fp);
          if (num == 1)
          fantasy->active = TRUE;
          else
          fantasy->active = FALSE;
          fMatch = TRUE;
        }
        break;
      case 'B':
        if (!str_cmp(word, "BanList")) {
          int point = fread_number(fp);
          free_string(fantasy->ban_list[point]);
          fantasy->ban_list[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        break;
      case 'D':
        KEY("Description", fantasy->description, fread_string(fp));
        KEY("Domain", fantasy->domain, fread_string(fp));

        break;
      case 'E':
        KEY("Entrance", fantasy->entrance, fread_number(fp));
        KEY("Elevator", fantasy->elevator, fread_string(fp));
        KEY("Emits", fantasy->emit, fread_number(fp));
        KEY("ExpDebuff", fantasy->expdebuff, fread_number(fp));
        if (!str_cmp(word, "Exit")) {
          int point = fread_number(fp);
          free_string(fantasy->exit_name[point]);
          fantasy->exit_name[point] = str_dup(fread_string(fp));
          free_string(fantasy->exit_alias[point]);
          fantasy->exit_alias[point] = str_dup(fread_string(fp));
          fantasy->entrances[point] = fread_number(fp);
          fantasy->exits[point] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "End")) {
          FantasyVect.push_back(fantasy);
          return;
        }
        break;
      case 'G':
        if (!str_cmp(word, "Godmode")) {
          int num = fread_number(fp);
          if (num == 1)
          fantasy->godmode = TRUE;
          else
          fantasy->godmode = FALSE;
          fMatch = TRUE;
        }
        break;
      case 'H':
        KEY("HighlightSince", fantasy->since_highlight, fread_number(fp));
        KEY("HighlightTime", fantasy->highlight_time, fread_number(fp));
        break;
      case 'L':
        KEY("LastUsed", fantasy->lastused, fread_number(fp));
        KEY("Locked", fantasy->locked, fread_number(fp));
        break;
      case 'N':
        KEY("Name", fantasy->name, fread_string(fp));
        if (!str_cmp(word, "NewParticipant")) {
          int point = fread_number(fp);
          free_string(fantasy->participant_names[point]);
          fantasy->participant_names[point] = str_dup(fread_string(fp));
          free_string(fantasy->participant_fames[point]);
          fantasy->participant_fames[point] = str_dup(fread_string(fp));
          free_string(fantasy->participant_eq[point]);
          fantasy->participant_eq[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        break;
      case 'P':
        KEY("Petitions", fantasy->petitions, fread_string(fp));
        KEY("Porn", fantasy->porn, fread_number(fp));
        if (!str_cmp(word, "Participant")) {
          int point = fread_number(fp);
          free_string(fantasy->participants[point]);
          fantasy->participants[point] = str_dup(fread_string(fp));
          free_string(fantasy->participant_shorts[point]);
          fantasy->participant_shorts[point] = str_dup(fread_string(fp));
          free_string(fantasy->participant_descs[point]);
          fantasy->participant_descs[point] = str_dup(fread_string(fp));
          fantasy->participant_exp[point] = fread_number(fp);
          fantasy->participant_role[point] = fread_number(fp);
          fantasy->participant_inroom[point] = fread_number(fp);
          int pnoaction = fread_number(fp);
          int pblind = fread_number(fp);
          int pnomove = fread_number(fp);
          if (pnoaction == 1)
          fantasy->participant_noaction[point] = TRUE;
          else
          fantasy->participant_noaction[point] = FALSE;
          if (pblind == 1)
          fantasy->participant_blind[point] = TRUE;
          else
          fantasy->participant_blind[point] = FALSE;
          if (pnomove == 1)
          fantasy->participant_nomove[point] = TRUE;
          else
          fantasy->participant_nomove[point] = FALSE;

          for (int i = 0; i < 30; i++)
          fantasy->participant_stats[point][i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'R':
        if (!str_cmp(word, "Rooms")) {
          for (int i = 0; i < 100; i++)
          fantasy->rooms[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "RoomSafe")) {
          for (int i = 0; i < 100; i++)
          fantasy->safe_room[i] = fread_number(fp);
          fMatch = TRUE;
        }

        if (!str_cmp(word, "Roles")) {
          int point = fread_number(fp);
          free_string(fantasy->roles[point]);
          fantasy->roles[point] = str_dup(fread_string(fp));
          fantasy->role_desc[point] = str_dup(fread_string(fp));
          fantasy->role_cost[point] = fread_number(fp);
          for (int i = 0; i < 30; i++)
          fantasy->role_stats[point][i] = fread_number(fp);
          fMatch = TRUE;
        }

        break;
      case 'S':
        KEY("Stupid", fantasy->stupid, fread_number(fp));
        KEY("Statcost", fantasy->statcost, fread_number(fp));
        KEY("StartingExp", fantasy->startingexp, fread_number(fp));
        if (!str_cmp(word, "Stat")) {
          int point = fread_number(fp);
          free_string(fantasy->stats[point]);
          fantasy->stats[point] = str_dup(fread_string(fp));
          free_string(fantasy->stat_desc[point]);
          fantasy->stat_desc[point] = str_dup(fread_string(fp));
          fantasy->stat_offense_power[point] = fread_number(fp);
          fantasy->stat_defense_power[point] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'T':
        if (!str_cmp(word, "Trusts")) {
          for (int i = 0; i < 200; i++) {
            int val = fread_number(fp);
            if (val == 1)
            fantasy->participant_trusted[i] = TRUE;
            else
            fantasy->participant_trusted[i] = FALSE;
          }
          fMatch = TRUE;
        }
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_fantasy: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  int fantasy_strength(const FANTASY_TYPE *fant) {
    int strength = 0;
    if (fant->active == FALSE)
    return 0;
    for (int i = 0; i < 30; i++) {
      if (safe_strlen(fant->exit_name[i]) > 1)
      strength += 200;
    }
    for (int i = 0; i < 200; i++) {
      if (str_cmp(fant->author, fant->participants[i]))
      strength += fant->participant_exp[i];
    }
    if (fant->lastused < (current_time - (3600 * 24 * 21)))
    strength /= 5;
    int count = 0;
    for (int i = 0; i < 200; i++) {
      if (safe_strlen(fant->participant_shorts[i]) > 1)
      count++;
    }

    if (count < 10)
    strength /= 3;
    if (fant->highlight_time == 0)
    strength /= 50;
    if (fant->porn == 1)
    strength /= 3;
    if (fant->stupid == 1)
    strength /= 2;

    return strength;
  }

  struct fantasy_greater {
    inline bool operator()(const FANTASY_TYPE *fant1, const FANTASY_TYPE *fant2) {
      return (fantasy_strength(fant1) > fantasy_strength(fant2));
    }
  };

#define FANTASY_FILE "../data/fantasies.txt"
  void load_fantasies() {
    nullfantasy = new_fantasy();
    FILE *fp;

    if ((fp = fopen(FANTASY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_fantasies: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "FANTASY")) {
          fread_fantasy(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_fantasies: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
      std::sort(FantasyVect.begin(), FantasyVect.end(), fantasy_greater());
      fantasy_bonus = 0;
    }
    else {
      bug("Cannot open fantasies.txt", 0);
      exit(0);
    }
  }

  void save_fantasies(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/fantasies.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/fantasies.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/fantasies.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/fantasies.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/fantasies.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/fantasies.txt");
      else
      sprintf(buf, "../data/back7/fantasies.txt");
      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open fantasies.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(FANTASY_FILE, "w")) == NULL) {
        bug("Cannot open storylines.txt for writing", 0);
        return;
      }
    }
    int hcount = 0;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it)->highlight_time == 0 && (*it)->since_highlight == 0)
      (*it)->since_highlight = current_time;
      if ((*it)->active == FALSE || safe_strlen((*it)->description) < 10) {
        (*it)->highlight_time = 0;
        (*it)->since_highlight = current_time;
      }
      else if ((*it)->highlight_time != 0 && (*it)->highlight_time <
          current_time - (3600 * 24 * 4) -
          (3600 * 24 * UMIN(30, part_count((*it))) / 3)) {
        (*it)->highlight_time = 0;
        (*it)->since_highlight = current_time;
      }
      if ((*it)->highlight_time > 0)
      hcount++;
    }
    int mintime = current_time;
    char fname[MSL];
    if (hcount < 4) {
      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->highlight_time > 0)
        continue;
        if ((*it)->lastused > (current_time - (3600 * 24 * 45)) && safe_strlen((*it)->description) > 5 && (*it)->active == TRUE) {
          if ((*it)->since_highlight < mintime) {
            mintime = (*it)->since_highlight;
            sprintf(fname, "%s", (*it)->name);
          }
        }
      }
      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if (safe_strlen(fname) > 1 && !str_cmp(fname, (*it)->name)) {
          (*it)->highlight_time = current_time;
        }
      }
    }

    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it)->lastused == 0)
      (*it)->lastused = current_time;
      if ((*it)->lastused < (current_time - (3600 * 24 * 90)))
      continue;

      fprintf(fpout, "#FANTASY\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Name %s~\n", from_color((*it)->name));
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Domain %s~\n", (*it)->domain);
      fprintf(fpout, "Statcost %d\n", (*it)->statcost);
      fprintf(fpout, "StartingExp %d\n", (*it)->startingexp);
      fprintf(fpout, "Petitions %s~\n", (*it)->petitions);
      fprintf(fpout, "Entrance %d\n", (*it)->entrance);
      fprintf(fpout, "ExpDebuff %d\n", (*it)->expdebuff);
      fprintf(fpout, "Emits %d\n", (*it)->emit);
      fprintf(fpout, "LastUsed %d\n", (*it)->lastused);
      fprintf(fpout, "Porn %d\n", (*it)->porn);
      fprintf(fpout, "Stupid %d\n", (*it)->stupid);
      fprintf(fpout, "Locked %d\n", (*it)->locked);
      fprintf(fpout, "Elevator %s~\n", (*it)->elevator);
      fprintf(fpout, "HighlightSince %d\n", (*it)->since_highlight);
      fprintf(fpout, "HighlightTime %d\n", (*it)->highlight_time);
      for (int i = 0; i < 30; i++) {
        if (safe_strlen((*it)->stats[i]) > 1)
        fprintf(fpout, "Stat %d %s~\n%s~\n%d %d\n", i, (*it)->stats[i], (*it)->stat_desc[i], (*it)->stat_offense_power[i], (*it)->stat_defense_power[i]);
      }
      for (int i = 0; i < 20; i++) {
        if (safe_strlen((*it)->roles[i]) > 1) {
          fprintf(fpout, "Roles %d %s~\n%s~\n%d", i, (*it)->roles[i], (*it)->role_desc[i], (*it)->role_cost[i]);
          for (int j = 0; j < 30; j++)
          fprintf(fpout, " %d", (*it)->role_stats[i][j]);
          fprintf(fpout, "\n");
        }
      }
      for (int i = 0; i < 200; i++) {
        if (safe_strlen((*it)->exit_name[i]) > 1) {
          fprintf(fpout, "Exit %d %s~\n%s~\n%d %d\n", i, (*it)->exit_name[i], (*it)->exit_alias[i], (*it)->entrances[i], (*it)->exits[i]);
        }
      }
      for (int i = 0; i < 100; i++) {
        if (safe_strlen((*it)->ban_list[i]) > 1)
        fprintf(fpout, "BanList %d %s~\n", i, (*it)->ban_list[i]);
      }
      fprintf(fpout, "Rooms");
      for (int i = 0; i < 100; i++) {
        fprintf(fpout, " %d", (*it)->rooms[i]);
      }
      fprintf(fpout, "\n");

      fprintf(fpout, "RoomSafe");
      for (int i = 0; i < 100; i++) {
        fprintf(fpout, " %d", (*it)->safe_room[i]);
      }
      fprintf(fpout, "\n");

      fprintf(fpout, "Trusts");
      for (int i = 0; i < 200; i++) {
        if ((*it)->participant_trusted[i] == FALSE)
        fprintf(fpout, " %d", 0);
        else
        fprintf(fpout, " %d", 1);
      }
      fprintf(fpout, "\n");

      if ((*it)->active == TRUE)
      fprintf(fpout, "Active 1\n");
      else
      fprintf(fpout, "Active 0\n");

      if ((*it)->godmode == TRUE)
      fprintf(fpout, "Godmode 1\n");
      else
      fprintf(fpout, "Godmode 0\n");

      for (int i = 0; i < 200; i++) {
        if (safe_strlen((*it)->participants[i]) > 1) {
          bool stats = FALSE;
          for (int j = 0; j < 30; j++) {
            if ((*it)->participant_stats[i][j] > 0)
            stats = TRUE;
          }
          if (stats == TRUE || (*it)->participant_exp[i] > 0) {
            fprintf(fpout, "Participant %d %s~\n%s~\n%s~\n%d %d %d ", i, (*it)->participants[i], (*it)->participant_shorts[i], (*it)->participant_descs[i], (*it)->participant_exp[i], (*it)->participant_role[i], (*it)->participant_inroom[i]);
            if ((*it)->participant_noaction[i] == TRUE)
            fprintf(fpout, "1 ");
            else
            fprintf(fpout, "0 ");
            if ((*it)->participant_blind[i] == TRUE)
            fprintf(fpout, "1 ");
            else
            fprintf(fpout, "0 ");
            if ((*it)->participant_nomove[i] == TRUE)
            fprintf(fpout, "1");
            else
            fprintf(fpout, "0");

            for (int j = 0; j < 30; j++)
            fprintf(fpout, " %d", (*it)->participant_stats[i][j]);
            fprintf(fpout, "\n");
            fprintf(fpout, "NewParticipant %d %s~\n%s~\n%s~\n", i, (*it)->participant_names[i], (*it)->participant_fames[i], (*it)->participant_eq[i]);
          }
        }
      }

      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_fantasies(TRUE);
    }
  }

  FANTASY_TYPE *fetch_fantasy(CHAR_DATA *ch, int number) {
    int i = 1;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (i == number) {
        if (ch == NULL)
        return (*it);
        if ((*it)->active == TRUE || !str_cmp((*it)->author, ch->name) || IS_IMMORTAL(ch))
        return (*it);
      }
      i++;
    }
    return NULL;
  }

  int dreamworld_count(CHAR_DATA *ch) {
    int count = 0;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (!str_cmp((*it)->author, ch->name))
      count++;
    }
    return count;
  }

  int fantasy_number(FANTASY_TYPE *fant) {
    int i = 1;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it) == fant)
      return i;
      i++;
    }
    return -1;
  }

  bool trusted_dreamer(CHAR_DATA *ch, FANTASY_TYPE *fant, int point) {
    if (!str_cmp(fant->author, ch->name))
    return TRUE;
    for (int i = 0; i < 200; i++) {
      if (!str_cmp(fant->participants[i], ch->name) && fant->participant_trusted[i] == TRUE)
      return TRUE;
    }
    if (point == ch->pcdata->tempdreamgodchar && fantasy_number(fant) == ch->pcdata->tempdreamgodworld)
    return TRUE;

    return FALSE;
  }

  bool power_fantasy(FANTASY_TYPE *fant) {
    int roomcount = 0;
    for (int i = 0; i < 100; i++) {
      if (fant->rooms[i] != 0)
      roomcount++;
    }
    if (roomcount < 5)
    return FALSE;
    if (fant->lastused < (current_time - (3600 * 24 * 21)))
    return FALSE;

    for (int i = 0; i < 200; i++) {
      if (fant->participant_exp[i] > 0)
      return TRUE;
    }
    return FALSE;
  }

  int stat_result(CHAR_DATA *ch, FANTASY_TYPE *fant, int stat, int type) {
    int count = 0;
    int tovalue = 0;
    int chvalue = 0;
    int chnoncom = 0;
    int tononcom = 0;
    int chcount = 0;
    int chhigher = 0;
    int chlower = 0;
    char buf[MSL];
    if (type == 3) {
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fant->participants[i], fant->author) && fant->godmode == TRUE)
        continue;
        if (fant->participant_godmode[i] == TRUE)
        continue;
        if (!str_cmp(fant->participants[i], ch->name)) {
          chvalue += fant->participant_stats[i][stat];
          for (int y = 0; y < 30; y++) {
            chvalue += fant->participant_stats[i][y];

            if (safe_strlen(fant->stats[y]) > 2 && fant->role_stats[fant->participant_role[i]][y] > 0) {
              chcount++;
              if (fant->participant_stats[i][y] >
                  fant->participant_stats[i][stat] && y != stat)
              chhigher++;
              if (fant->participant_stats[i][y] <
                  fant->participant_stats[i][stat] && y != stat)
              chlower++;
            }
          }
          sprintf(buf, "DREAMROLL: %s(%s) Rolls %s, %d base, %d total, %d exp", fant->participant_names[i], ch->name, fant->stats[stat], fant->participant_stats[i][stat], chvalue, fant->role_cost[fant->participant_role[i]] / fant->statcost);
          chvalue += fant->role_cost[fant->participant_role[i]] / fant->statcost;
          log_string(buf);
        }
        if (safe_strlen(fant->participants[i]) > 1) {
          CHAR_DATA *to = get_char_world_pc(fant->participants[i]);
          if (to != NULL && is_dreaming(to) && ch->pcdata->dream_room == to->pcdata->dream_room) {
            count++;
            tovalue += fant->participant_stats[i][stat];
            for (int y = 0; y < 30; y++) {
              //                tovalue += fant->participant_stats[i][y];
              //		count++;
            }
            sprintf(buf, "DREAMROLL: %s Background %s, %d base, %d total, %d exp", fant->participant_names[i], fant->stats[stat], fant->participant_stats[i][stat], tovalue, fant->role_cost[fant->participant_role[i]] / fant->statcost);
            tovalue +=
            fant->role_cost[fant->participant_role[i]] / fant->statcost;
            log_string(buf);
          }
        }
      }
      tovalue /= UMAX(1, count);
      tovalue = UMIN(tovalue, chvalue * 2);
      tovalue = UMAX(tovalue, chvalue / 2);
      int roll = number_range(1, chvalue);
      sprintf(buf, "DREAMROLL: chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, roll);
      log_string(buf);
      for (int i = 0; i < chhigher; i++)
      roll = roll * (100 + (number_range(0, 20))) / 100;
      for (int i = 0; i < chlower; i++)
      roll = roll * (100 - (number_range(0, 20))) / 100;
      if (chlower == 0)
      roll /= 2;
      sprintf(buf, "DREAMROLL2: chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, roll);
      log_string(buf);
      if (roll >= (tovalue * 75 / 100) && number_range(1, 54231) % 3 != 0)
      return 3;
      if (roll <= (tovalue * 25 / 100) && number_range(1, 53432) % 2 == 0)
      return 1;
      return 2;
    }
    else if (type == 1) {
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fant->participants[i], fant->author) && fant->godmode == TRUE)
        continue;
        if (fant->participant_godmode[i] == TRUE)
        continue;

        if (!str_cmp(fant->participants[i], ch->name)) {
          chvalue +=
          fant->participant_stats[i][stat] * fant->stat_offense_power[stat];
          for (int y = 0; y < 30; y++) {
            chvalue +=
            fant->participant_stats[i][y] * fant->stat_offense_power[y];
            if (fant->stat_offense_power[y] == 0 && fant->stat_defense_power[y] == 0)
            chnoncom += fant->participant_stats[i][y];
            if (safe_strlen(fant->stats[y]) > 2 && fant->role_stats[fant->participant_role[i]][y] > 0) {
              chcount++;
              if (fant->participant_stats[i][y] >
                  fant->participant_stats[i][stat] && y != stat)
              chhigher++;
              if (fant->participant_stats[i][y] <
                  fant->participant_stats[i][stat] && y != stat)
              chlower++;
            }
          }
          sprintf(buf, "DREAMROLL Offense: %s(%s) Rolls %s, %d base, %d total, %d exp, %d noncom", fant->participant_names[i], ch->name, fant->stats[stat], fant->participant_stats[i][stat] *
          fant->stat_offense_power[stat], chvalue, fant->role_cost[fant->participant_role[i]] / fant->statcost, chnoncom);
          chnoncom += fant->role_cost[fant->participant_role[i]] / fant->statcost;
          log_string(buf);
        }
        if (safe_strlen(fant->participants[i]) > 1) {
          CHAR_DATA *to = get_char_world_pc(fant->participants[i]);
          if (to != NULL && is_dreaming(to) && ch->pcdata->dream_room == to->pcdata->dream_room) {
            count++;
            tovalue +=
            fant->participant_stats[i][stat] * fant->stat_offense_power[stat];
            for (int y = 0; y < 30; y++) {
              if (fant->participant_stats[i][y] > 0) {
                tovalue +=
                fant->participant_stats[i][y] * fant->stat_offense_power[y];
                if (fant->stat_offense_power[y] == 0 && fant->stat_defense_power[y] == 0)
                tononcom += fant->participant_stats[i][y];
                count++;
              }
            }
            sprintf(buf, "DREAMROLL Offense: %s Background %s, %d base, %d total, %d exp, %d noncom", fant->participant_names[i], fant->stats[stat], fant->participant_stats[i][stat] *
            fant->stat_offense_power[stat], tovalue, fant->role_cost[fant->participant_role[i]] / fant->statcost, tononcom);
            tononcom +=
            fant->role_cost[fant->participant_role[i]] / fant->statcost;
            log_string(buf);
          }
        }
      }
      sprintf(buf, "DREAMROLL Offense1: chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, 0);
      log_string(buf);
      chvalue = chvalue * (50 + chnoncom) / 50;
      tovalue /= UMAX(1, count);
      tononcom /= UMAX(1, count);
      tovalue = tovalue * (50 + tononcom) / 50;
      tovalue = UMIN(tovalue, chvalue * 2);
      tovalue = UMAX(tovalue, chvalue / 2);
      int roll = number_range(1, chvalue);
      sprintf(buf, "DREAMROLL: Offense2 chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, roll);
      log_string(buf);
      for (int i = 0; i < chhigher; i++)
      roll = roll * (100 + (number_range(0, 10))) / 100;
      for (int i = 0; i < chlower; i++)
      roll = roll * (100 - (number_range(0, 10))) / 100;
      if (chlower == 0)
      roll /= 2;
      sprintf(buf, "DREAMROLL: Offense3 chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, roll);
      log_string(buf);
      if (roll >= (tovalue * 75 / 100) && number_range(1, 52334) % 3 != 0)
      return 3;
      if (roll <= (tovalue * 25 / 100) && number_range(1, 23254) % 2 == 0)
      return 1;
      return 2;
    }
    else {
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fant->participants[i], fant->author) && fant->godmode == TRUE)
        continue;
        if (fant->participant_godmode[i] == TRUE)
        continue;

        if (!str_cmp(fant->participants[i], ch->name)) {
          chvalue +=
          fant->participant_stats[i][stat] * fant->stat_defense_power[stat];
          for (int y = 0; y < 30; y++) {
            chvalue +=
            fant->participant_stats[i][y] * fant->stat_defense_power[y];
            if (fant->stat_offense_power[y] == 0 && fant->stat_defense_power[y] == 0)
            chnoncom += fant->participant_stats[i][y];
            if (safe_strlen(fant->stats[y]) > 2 && fant->role_stats[fant->participant_role[i]][y] > 0) {
              chcount++;
              if (fant->participant_stats[i][y] >
                  fant->participant_stats[i][stat] && y != stat)
              chhigher++;
              if (fant->participant_stats[i][y] <
                  fant->participant_stats[i][stat] && y != stat)
              chlower++;
            }
          }
          sprintf(buf, "DREAMROLL Defense: %s(%s) Rolls %s, %d base, %d total, %d exp, %d noncom", fant->participant_names[i], ch->name, fant->stats[stat], fant->participant_stats[i][stat] *
          fant->stat_defense_power[stat], chvalue, fant->role_cost[fant->participant_role[i]] / fant->statcost, chnoncom);
          chnoncom += fant->role_cost[fant->participant_role[i]] / fant->statcost;
          log_string(buf);
        }
        if (safe_strlen(fant->participants[i]) > 1) {
          CHAR_DATA *to = get_char_world_pc(fant->participants[i]);
          if (to != NULL && is_dreaming(to) && ch->pcdata->dream_room == to->pcdata->dream_room) {
            count++;
            tovalue +=
            fant->participant_stats[i][stat] * fant->stat_defense_power[stat];
            for (int y = 0; y < 30; y++) {
              if (fant->participant_stats[i][y] > 0) {
                tovalue +=
                fant->participant_stats[i][y] * fant->stat_defense_power[y];
                if (fant->stat_offense_power[y] == 0 && fant->stat_defense_power[y] == 0)
                tononcom += fant->participant_stats[i][y];
                count++;
              }
            }
            sprintf(buf, "DREAMROLL Offense: %s Background %s, %d base, %d total, %d exp, %d noncom", fant->participant_names[i], fant->stats[stat], fant->participant_stats[i][stat] *
            fant->stat_offense_power[stat], tovalue, fant->role_cost[fant->participant_role[i]] / fant->statcost, tononcom);
            tononcom +=
            fant->role_cost[fant->participant_role[i]] / fant->statcost;
            log_string(buf);
          }
        }
      }
      sprintf(buf, "DREAMROLL Defense1: chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, 0);
      log_string(buf);
      chvalue = chvalue * (50 + chnoncom) / 50;
      tovalue /= UMAX(1, count);
      tononcom /= UMAX(1, count);
      tovalue = tovalue * (50 + tononcom) / 50;
      tovalue = UMIN(tovalue, chvalue * 2);
      tovalue = UMAX(tovalue, chvalue / 2);
      int roll = number_range(1, chvalue);
      sprintf(buf, "DREAMROLL Defense2: chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, roll);
      log_string(buf);
      for (int i = 0; i < chhigher; i++)
      roll = roll * (100 + (number_range(0, 10))) / 100;
      for (int i = 0; i < chlower; i++)
      roll = roll * (100 - (number_range(0, 10))) / 100;
      if (chlower == 0)
      roll /= 2;
      sprintf(buf, "DREAMROLL: Defense3 chvalue: %d, tovalue: %d, roll: %d", chvalue, tovalue, roll);
      log_string(buf);
      if (roll >= (tovalue * 75 / 100) && number_range(1, 54523) % 3 != 0)
      return 3;
      if (roll <= (tovalue * 25 / 100) && number_range(1, 42343) % 2 == 0)
      return 1;
      return 2;
    }
  }
  _DOFUN(do_dreamworld) {
    FANTASY_TYPE *fantasy;
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    DESCRIPTOR_DATA *d;

    if (fantasy_bonus == 0)
    fantasy_bonus = (number_range(547, (int)(ch->id / 100)) % 4) + 1;

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "add") || !str_cmp(arg1, "create")) {
      if ((dreamworld_count(ch) + 1) * 10000 > ch->pcdata->dexp) {
        send_to_char("You need at least 10, 000 dreamscape experience for each dream world you create.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: Dreamworld create (name of dreamworld)\n\r", ch);
        return;
      }
      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (!str_cmp((*it)->name, argument)) {
          send_to_char("A dreamworld with that name already exists.\n\r", ch);
          return;
        }
      }
      FANTASY_TYPE *fantasy = new_fantasy();
      free_string(fantasy->name);
      fantasy->name = str_dup(argument);
      free_string(fantasy->author);
      fantasy->author = str_dup(ch->name);
      fantasy->valid = TRUE;
      if (higher_power(ch)) {
        free_string(fantasy->domain);
        fantasy->domain = str_dup(ch->name);
      }
      FantasyVect.push_back(fantasy);
      printf_to_char(ch, "You create a new dreamworld named: %s.", fantasy->name);
      return;
    }
    else if (!str_cmp(arg1, "rebonus") && IS_IMMORTAL(ch)) {
      fantasy_bonus = (number_range(5474, 5836385) % 4) + 1;
    }
    else if (!str_cmp(arg1, "list")) {
      int i = 1;
      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->highlight_time == 0)
        continue;

        if ((*it)->active != FALSE || !str_cmp(ch->name, (*it)->author) || IS_IMMORTAL(ch)) {
          if (i == fantasy_bonus)
          sprintf(buf, "`W%2d`c)`x %s`x (Bonus XP)\n\r", fantasy_number((*it)), (*it)->name);
          else
          sprintf(buf, "`W%2d`c)`x %s`x\n\r", fantasy_number((*it)), (*it)->name);
          strcat(string, buf);
        }
        i++;
      }
      strcat(string, "\n");
      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->highlight_time != 0)
        continue;

        if ((*it)->active != FALSE || !str_cmp(ch->name, (*it)->author) || IS_IMMORTAL(ch)) {
          sprintf(buf, "`W%2d`c)`x %s`x\n\r", fantasy_number((*it)), (*it)->name);
          strcat(string, buf);
        }
        i++;
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      return;
    }
    else if (!str_cmp(arg1, "porn")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (fantasy->porn == 0) {
        fantasy->porn = 1;
        send_to_char("Fantasy marked as porn.\n\r", ch);
        return;
      }
      fantasy->porn = 0;
      send_to_char("Porn tag removed.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "stupid")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (fantasy->stupid == 0) {
        fantasy->stupid = 1;
        send_to_char("Fantasy marked as stupid.\n\r", ch);
        return;
      }
      fantasy->stupid = 0;
      send_to_char("Stupid tag removed.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "age")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      fantasy->highlight_time -= (3600 * 24);
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "summary")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      sprintf(buf, "`cName`x: %s\n`cDescription`x:\n%s\n\r", fantasy->name, fantasy->description);
      strcat(string, buf);
      if (safe_strlen(fantasy->domain) > 2) {
        sprintf(buf, "`cDomain of`x: %s\n\r", fantasy->domain);
        strcat(string, buf);
      }
      if (IS_IMMORTAL(ch)) {
        sprintf(buf, "Days since RP: %d\n\r", ((int)(current_time)-fantasy->lastused) / (3600 * 24));
        strcat(string, buf);
        if (fantasy->highlight_time != 0) {
          sprintf(buf, "Days highlighted: %d\n\r", ((int)(current_time)-fantasy->highlight_time) / (3600 * 24));
          strcat(string, buf);
        }
        else {
          sprintf(buf, "Days since highlight: %d\n\r", ((int)(current_time)-fantasy->since_highlight) / (3600 * 24));
          strcat(string, buf);
        }
      }

      if (fantasy->emit == 1) {
        sprintf(buf, "`cEmits:`x `gEnabled`x\n\r");
        strcat(string, buf);
      }
      sprintf(buf, "\n`cStarting Experience`x: %d\n`cExp Debuff`x: %d\n`cStat Cost`x: %d\n`cStats`x: ", fantasy->startingexp, UMAX(1, fantasy->expdebuff), fantasy->statcost);
      strcat(string, buf);
      for (int i = 0; i < 30; i++) {
        if (safe_strlen(fantasy->stats[i]) > 1) {
          sprintf(buf, "%s, ", fantasy->stats[i]);
          strcat(string, buf);
        }
      }
      sprintf(buf, "\n\n`cRoles`x: ");
      strcat(string, buf);
      for (int i = 0; i < 20; i++) {
        if (safe_strlen(fantasy->roles[i]) > 1) {
          sprintf(buf, "`W%s`x(%dxp), ", fantasy->roles[i], fantasy->role_cost[i]);
          strcat(string, buf);
        }
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      return;
    }
    else if (!str_cmp(arg1, "fullinfo")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      sprintf(buf, "`cName`x: %s\n`cDescription`x:\n%s\n\r", fantasy->name, fantasy->description);
      strcat(string, buf);
      if (fantasy->emit == 1) {
        sprintf(buf, "`cEmits:`x `gEnabled`x\n\r");
        strcat(string, buf);
      }
      if (safe_strlen(fantasy->domain) > 2) {
        sprintf(buf, "`cDomain of`x: %s\n\r", fantasy->domain);
        strcat(string, buf);
      }

      sprintf(buf, "\n`cStarting Experience`x: %d\n`cStat Cost`x: %d\n`cStats`x: ", fantasy->startingexp, fantasy->statcost);
      strcat(string, buf);
      for (int i = 0; i < 30; i++) {
        if (safe_strlen(string) < 25000) {
          if (safe_strlen(fantasy->stats[i]) > 1) {
            sprintf(buf, "`W%s`x:\n%s\n\n ", fantasy->stats[i], fantasy->stat_desc[i]);
            strcat(string, buf);
          }
        }
      }
      sprintf(buf, "\n\n`cRoles`x: ");
      strcat(string, buf);
      for (int i = 0; i < 20; i++) {
        if (safe_strlen(string) < 25000) {
          if (safe_strlen(fantasy->roles[i]) > 1) {
            sprintf(buf, "`W%s`x: %d\n%s\n\n", fantasy->roles[i], fantasy->role_cost[i], fantasy->role_desc[i]);
            strcat(string, buf);
          }
        }
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
      return;
    }
    else if (!str_cmp(arg1, "associate")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->dream_origin) > 1) {
        send_to_char("You're already associated with a dreamworld.\n\r", ch);
        return;
      }
      if (guestmonster(ch)) {
        if (!power_fantasy(fantasy)) {
          send_to_char("That world is not powerful enough to give birth to a monster.\n\r", ch);
          return;
        }
        if (fantasy->highlight_time == 0) {
          send_to_char("That world is not close enough right now.\n\r", ch);
          return;
        }
      }
      free_string(ch->pcdata->dream_origin);
      ch->pcdata->dream_origin = str_dup(fantasy->name);
      if (ch->race == RACE_DREAMCHILD || ch->race == RACE_FANTASY) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, fantasy->participants[i]))
          fantasy->participant_exp[i] += 25000;
        }
      }
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "roll")) {
      if (is_dreaming(ch)) {
        FANTASY_TYPE *fant;
        argument = one_argument_nouncap(argument, arg2);
        if ((fant = in_fantasy(ch)) != NULL) {
          for (int i = 0; i < 100; i++) {
            if (!str_cmp(ch->name, fant->participants[i])) {
              for (int x = 0; x < 30; x++) {
                if (!str_cmp(argument, fant->stats[x])) {
                  int total = 0;
                  for (int d = 0; d < fant->participant_stats[i][x]; d++) {
                    if (number_percent() % 2 == 0)
                    total++;
                  }
                  int type;
                  if (!str_cmp(arg2, "attack") || !str_cmp(arg2, "offense") || !str_cmp(arg2, "offensive") || !str_cmp(arg2, "offensively"))
                  type = 1;
                  else if (!str_cmp(arg2, "defense") || !str_cmp(arg2, "defend") || !str_cmp(arg2, "defensive") || !str_cmp(arg2, "defensively"))
                  type = 2;
                  else if (!str_cmp(arg2, "general") || !str_cmp(arg2, "other") || !str_cmp(arg2, "generally"))
                  type = 3;
                  else if (!str_cmp(arg2, "failattack"))
                  type = 4;
                  else if (!str_cmp(arg2, "faildefend"))
                  type = 5;
                  else if (!str_cmp(arg2, "failgeneral"))
                  type = 6;
                  else {
                    send_to_char("Roll Attack/Defend/General/FailAttack/FailDefend/FailGeneral (stat)\n\r", ch);
                    return;
                  }
                  if (type == 4) {
                    sprintf(
                    buf, "\t\t`W%s`x rolls their %s offensively and fails.\n\r", dream_name(ch), fant->stats[x]);
                    dreamscape_message(ch, ch->pcdata->dream_room, buf);
                    printf_to_char(ch, "You roll your %s offensively and fail.\n\r", fant->stats[x]);
                    return;
                  }
                  if (type == 5) {
                    sprintf(
                    buf, "\t\t`W%s`x rolls their %s defensively and fails.\n\r", dream_name(ch), fant->stats[x]);
                    dreamscape_message(ch, ch->pcdata->dream_room, buf);
                    printf_to_char(ch, "You roll your %s defensively and fail.\n\r", fant->stats[x]);
                    return;
                  }
                  if (type == 6) {
                    sprintf(buf, "\t\t`W%s`x rolls their %s and fails.\n\r", dream_name(ch), fant->stats[x]);
                    dreamscape_message(ch, ch->pcdata->dream_room, buf);
                    printf_to_char(ch, "You roll your %s and fail.\n\r", fant->stats[x]);
                    return;
                  }
                  int result = stat_result(ch, fant, x, type);
                  if (result == 3) {
                    if (type == 1)
                    sprintf(buf, "\t\t`W%s`x rolls their %s offensively and attains great success.\n\r", dream_name(ch), fant->stats[x]);
                    else if (type == 2)
                    sprintf(buf, "\t\t`W%s`x rolls their %s defensively and attains great success.\n\r", dream_name(ch), fant->stats[x]);
                    else
                    sprintf(buf, "\t\t`W%s`x rolls their %s and attains great success.\n\r", dream_name(ch), fant->stats[x]);
                    dreamscape_message(ch, ch->pcdata->dream_room, buf);
                    if (type == 1)
                    printf_to_char(ch, "You roll your %s offensively and attain great success.\n\r", fant->stats[x]);
                    else if (type == 2)
                    printf_to_char(ch, "You roll your %s defensively and attain great success.\n\r", fant->stats[x]);
                    else
                    printf_to_char(
                    ch, "You roll your %s and attain great success.\n\r", fant->stats[x]);
                    return;
                  }
                  else if (result == 1) {
                    if (type == 1)
                    sprintf(
                    buf, "\t\t`W%s`x rolls their %s offensively and fails.\n\r", dream_name(ch), fant->stats[x]);
                    else if (type == 2)
                    sprintf(
                    buf, "\t\t`W%s`x rolls their %s defensively and fails.\n\r", dream_name(ch), fant->stats[x]);
                    else
                    sprintf(buf, "\t\t`W%s`x rolls their %s and fails.\n\r", dream_name(ch), fant->stats[x]);
                    dreamscape_message(ch, ch->pcdata->dream_room, buf);
                    if (type == 1)
                    printf_to_char(ch, "You roll your %s offensively and fail.\n\r", fant->stats[x]);
                    else if (type == 2)
                    printf_to_char(ch, "You roll your %s defensively and fail.\n\r", fant->stats[x]);
                    else
                    printf_to_char(ch, "You roll your %s and fail.\n\r", fant->stats[x]);
                    return;
                  }
                  else {
                    if (type == 1)
                    sprintf(buf, "\t\t`W%s`x rolls their %s offensively and attains average success.\n\r", dream_name(ch), fant->stats[x]);
                    else if (type == 2)
                    sprintf(buf, "\t\t`W%s`x rolls their %s defensively and attains average success.\n\r", dream_name(ch), fant->stats[x]);
                    else
                    sprintf(buf, "\t\t`W%s`x rolls their %s and attains average success.\n\r", dream_name(ch), fant->stats[x]);
                    dreamscape_message(ch, ch->pcdata->dream_room, buf);
                    if (type == 1)
                    printf_to_char(ch, "You roll your %s offensively and attain average success.\n\r", fant->stats[x]);
                    else if (type == 2)
                    printf_to_char(ch, "You roll your %s defensively and attain average success.\n\r", fant->stats[x]);
                    else
                    printf_to_char(
                    ch, "You roll your %s and attain average success.\n\r", fant->stats[x]);
                    return;
                  }
                }
              }
              if (is_number(arg2) && is_number(argument)) {
                int total = 0;
                int dicenumber = atoi(arg2);
                int dicefaces = atoi(argument);
                if (dicenumber < 1 || dicenumber > 1000 || dicefaces < 2 || dicefaces > 100) {
                  send_to_char("You have to roll between 1 and 1000 dice with between 2 and 100 faces.\n\r", ch);
                  return;
                }
                for (int d = 0; d < dicenumber; d++) {
                  total += ((number_range(1, 534284) % dicefaces) + 1);
                }
                sprintf(buf, "\t\r`W%s`x rolls %d dice with %d faces for a total of %d.\n\r", dream_name(ch), dicenumber, dicefaces, total);
                dreamscape_message(ch, ch->pcdata->dream_room, buf);
                printf_to_char(
                ch, "You roll %d dice with %d faces for a total of %d.\n\r", dicenumber, dicefaces, total);
                return;
              }
              else {
                send_to_char("No such stat in this dream world.\n\r", ch);
                return;
              }
            }
          }
        }
      }
      send_to_char("You're not in a dream world.\n\r", ch);
    }
    else if (!str_cmp(arg1, "rollnumber")) {
      if (is_dreaming(ch)) {
        FANTASY_TYPE *fant;
        if ((fant = in_fantasy(ch)) != NULL) {
          for (int i = 0; i < 100; i++) {
            if (!str_cmp(ch->name, fant->participants[i])) {
              for (int x = 0; x < 30; x++) {
                if (!str_cmp(argument, fant->stats[x])) {
                  int total = 0;
                  for (int d = 0; d < fant->participant_stats[i][x]; d++) {
                    total += number_range(0, 2);
                    //                    if(number_percent() % 2 == 0)
                    //                    total++;
                  }
                  sprintf(buf, "\t\t`W%s`x rolls their %s and gets %d.\n\r", dream_name(ch), fant->stats[x], total);
                  dreamscape_message(ch, ch->pcdata->dream_room, buf);
                  printf_to_char(ch, "You roll your %s and get %d.\n\r", fant->stats[x], total);
                  return;
                }
              }
              argument = one_argument_nouncap(argument, arg2);
              if (is_number(arg2) && is_number(argument)) {
                int total = 0;
                int dicenumber = atoi(arg2);
                int dicefaces = atoi(argument);
                if (dicenumber < 1 || dicenumber > 1000 || dicefaces < 2 || dicefaces > 100) {
                  send_to_char("You have to roll between 1 and 1000 dice with between 2 and 100 faces.\n\r", ch);
                  return;
                }
                for (int d = 0; d < dicenumber; d++) {
                  total += ((number_range(1, 534284) % dicefaces) + 1);
                }
                sprintf(buf, "\t\r`W%s`x rolls %d dice with %d faces for a total of %d.\n\r", dream_name(ch), dicenumber, dicefaces, total);
                dreamscape_message(ch, ch->pcdata->dream_room, buf);
                printf_to_char(
                ch, "You roll %d dice with %d faces for a total of %d.\n\r", dicenumber, dicefaces, total);
                return;
              }
              else {
                send_to_char("No such stat in this dream world.\n\r", ch);
                return;
              }
            }
          }
        }
      }
      send_to_char("You're not in a dream world.\n\r", ch);
    }
    else if (!str_cmp(arg1, "rollfail")) {
      if (is_dreaming(ch)) {
        FANTASY_TYPE *fant;
        if ((fant = in_fantasy(ch)) != NULL) {
          for (int i = 0; i < 100; i++) {
            if (!str_cmp(ch->name, fant->participants[i])) {
              for (int x = 0; x < 30; x++) {
                if (!str_cmp(argument, fant->stats[x])) {
                  int total = 0;
                  total = number_range(0, 1);
                  //		  if(total == 1 && number_percent() % 2 == 0)
                  //			total = 0;
                  sprintf(buf, "\t\t`W%s`x rolls their %s and gets %d.\n\r", dream_name(ch), fant->stats[x], total);
                  dreamscape_message(ch, ch->pcdata->dream_room, buf);
                  printf_to_char(ch, "You roll your %s and get %d.\n\r", fant->stats[x], total);
                  return;
                }
              }
              argument = one_argument_nouncap(argument, arg2);
              if (is_number(arg2) && is_number(argument)) {
                int total = 0;
                int dicenumber = atoi(arg2);
                int dicefaces = atoi(argument);
                if (dicenumber < 1 || dicenumber > 1000 || dicefaces < 2 || dicefaces > 100) {
                  send_to_char("You have to roll between 1 and 1000 dice with between 2 and 100 faces.\n\r", ch);
                  return;
                }
                for (int d = 0; d < dicenumber; d++) {
                  total += number_range(1, 2);
                }
                sprintf(buf, "\t\r`W%s`x rolls %d dice with %d faces for a total of %d.\n\r", dream_name(ch), dicenumber, dicefaces, total);
                dreamscape_message(ch, ch->pcdata->dream_room, buf);
                printf_to_char(
                ch, "You roll %d dice with %d faces for a total of %d.\n\r", dicenumber, dicefaces, total);
                return;
              }
              else {
                send_to_char("No such stat in this dream world.\n\r", ch);
                return;
              }
            }
          }
        }
      }
      send_to_char("You're not in a dream world.\n\r", ch);
    }
    else if (!str_cmp(arg1, "makedoor")) {
      if (is_dreaming(ch)) {
        FANTASY_TYPE *fant;
        if ((fant = in_fantasy(ch)) != NULL) {
          if (dream_slave(ch) || IS_FLAG(ch->comm, COMM_DREAMSNARED)) {
            send_to_char("You cannot do that right now.\n\r", ch);
            return;
          }
          argument = one_argument_nouncap(argument, arg2);
          argument = one_argument_nouncap(argument, arg3);
          if (!str_cmp(arg2, "world")) {
            FANTASY_TYPE *destination = fetch_fantasy(ch, atoi(arg3));
            if (destination == NULL) {
              send_to_char("No such dreamworld.\n\r", ch);
              return;
            }
            /*
if(ch->pcdata->dream_timer < 2)
{
printf_to_char(ch, "You cannot do that for another %d
minutes.\n\r", 2 - ch->pcdata->dream_timer); return;
}
*/
            sprintf(buf, "%s steps through a previously unnoticed door through which you catch a glimpse of %s\n\r", dream_name(ch), destination->elevator);
            dreamscape_message(ch, ch->pcdata->dream_room, buf);
            int orig_room = ch->pcdata->dream_room;
            enter_dreamworld(ch, destination);
            do_function(ch, &do_look, "");
            int desti = ch->pcdata->dream_room;
            for (int i = 0; i < 200; i++) {
              if (is_name(fant->participant_names[i], argument)) {
                if (get_char_world_pc(fant->participants[i]) != NULL) {
                  CHAR_DATA *vic = get_char_world_pc(fant->participants[i]);
                  if (is_dreaming(vic) && vic->pcdata->dream_room == orig_room) {
                    if (ch->dream_dragging == vic || dream_slave(vic)) {
                      sprintf(buf, "%s follows.\n\r", dream_name(vic));
                      dreamscape_message(vic, vic->pcdata->dream_room, buf);
                      enter_dreamworld(vic, destination);
                      vic->pcdata->dream_room = desti;
                      do_function(ch, &do_look, "");
                    }
                  }
                }
              }
            }
            return;
          }
          if (!str_cmp(arg2, "room")) {
            if (crisis_notravel == 1) {
              send_to_char("It doesn't work\n\r", ch);
              return;
            }
            if (ch->pcdata->dream_timer < 30) {
              printf_to_char(ch, "You cannot do that for another %d minutes.\n\r", 30 - ch->pcdata->dream_timer);
              return;
            }
            int orig_room = ch->pcdata->dream_room;
            ROOM_INDEX_DATA *room = get_poroom(atoi(arg3));
            if (room == NULL) {
              send_to_char("You cannot go there.\n\r", ch);
              return;
            }
            sprintf(buf, "%s steps through a previously unnoticed door to %s\n\r", dream_name(ch), room->name);
            dreamscape_message(ch, ch->pcdata->dream_room, buf);
            if (!IS_FLAG(ch->act, PLR_SHROUD))
            SET_FLAG(ch->act, PLR_SHROUD);
            if (physical_dreamer(ch)) {
              ch->pcdata->spectre = 0;
            }
            ch->pcdata->sleeping = 0;
            char_from_room(ch);
            char_to_room(ch, room);
            do_function(ch, &do_look, "");
            for (int i = 0; i < 200; i++) {
              if (is_name(fant->participant_names[i], argument)) {
                if (get_char_world_pc(fant->participants[i]) != NULL) {
                  CHAR_DATA *vic = get_char_world_pc(fant->participants[i]);
                  if (is_dreaming(vic) && vic->pcdata->dream_room == orig_room && !under_understanding(vic, ch)) {
                    if (ch->dream_dragging == vic || dream_slave(vic)) {
                      sprintf(buf, "%s follows.\n\r", dream_name(vic));
                      dreamscape_message(vic, vic->pcdata->dream_room, buf);
                      if (!IS_FLAG(vic->act, PLR_SHROUD))
                      SET_FLAG(vic->act, PLR_SHROUD);

                      if (physical_dreamer(vic)) {
                        vic->pcdata->spectre = 0;
                      }
                      vic->pcdata->sleeping = 0;
                      char_from_room(vic);
                      char_to_room(vic, room);
                      do_function(vic, &do_look, "");
                    }
                  }
                }
              }
            }
          }
          if (!str_cmp(arg2, "haven")) {
            if (clinic_patient(ch) && institute_room(ch->in_room)) {
              send_to_char("You can't leave the institute.\n\r", ch);
              return;
            }
            if (ch->pcdata->dream_timer < 15) {
              printf_to_char(ch, "You cannot do that for another %d minutes.\n\r", 15 - ch->pcdata->dream_timer);
              return;
            }
            if (atoi(arg3) <= MAX_TAXIS) {
              int orig_room = ch->pcdata->dream_room;
              ROOM_INDEX_DATA *room =
              get_room_index(taxi_table[atoi(arg3) - 1].vnum);
              if (room == NULL) {
                send_to_char("You cannot go there.\n\r", ch);
                return;
              }
              sprintf(buf, "%s steps through a previously unnoticed door to %s\n\r", dream_name(ch), room->name);
              dreamscape_message(ch, ch->pcdata->dream_room, buf);
              if (!IS_FLAG(ch->act, PLR_SHROUD))
              SET_FLAG(ch->act, PLR_SHROUD);
              if (physical_dreamer(ch)) {
                ch->pcdata->spectre = 0;
              }
              ch->pcdata->sleeping = 0;
              char_from_room(ch);
              char_to_room(ch, room);
              do_function(ch, &do_look, "");

              for (int i = 0; i < 200; i++) {
                if (is_name(fant->participant_names[i], argument)) {
                  if (get_char_world_pc(fant->participants[i]) != NULL) {
                    CHAR_DATA *vic = get_char_world_pc(fant->participants[i]);
                    if (is_dreaming(vic) && vic->pcdata->dream_room == orig_room) {
                      if (ch->dream_dragging == vic || dream_slave(vic)) {
                        sprintf(buf, "%s follows.\n\r", dream_name(vic));
                        dreamscape_message(vic, vic->pcdata->dream_room, buf);
                        if (!IS_FLAG(vic->act, PLR_SHROUD))
                        SET_FLAG(vic->act, PLR_SHROUD);
                        if (physical_dreamer(vic)) {
                          vic->pcdata->spectre = 0;
                        }
                        vic->pcdata->sleeping = 0;
                        char_from_room(vic);
                        char_to_room(vic, room);
                        do_function(vic, &do_look, "");
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "attack")) {
      CHAR_DATA *victim;
      fantasy = in_fantasy(ch);
      if (fantasy == NULL) {
        send_to_char("You're not in a dream world.\n\r", ch);
        return;
      }
      if (dream_slave(ch)) {
        send_to_char("You can't do that at the moment.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      if ((victim = get_char_dream(ch, arg2)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }
      int chpoint = 0;
      int vicpoint = 0;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fantasy->participants[i], ch->name)) {
          chpoint = i;
        }
        if (!str_cmp(fantasy->participants[i], victim->name)) {
          vicpoint = i;
        }
      }
      if (fantasy->participant_noaction[chpoint] == TRUE) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (ch->pcdata->dream_attack_cool > 0) {
        send_to_char("That's still on cooldown.\n\r", ch);
        return;
      }
      for (int i = 0; i < 100; i++) {
        if (fantasy->rooms[i] == ch->pcdata->dream_room && fantasy->safe_room[i] == 1) {
          send_to_char("You can't do that here.\n\r", ch);
          return;
        }
      }
      int statpoint = -1;
      int max = 0;
      int chtotal = 0;
      int victotal = 0;
      for (int i = 0; i < 30; i++) {
        chtotal += fantasy->participant_stats[chpoint][i] *
        fantasy->stat_offense_power[i];
        victotal += fantasy->participant_stats[vicpoint][i] *
        fantasy->stat_defense_power[i];
      }
      if (!str_cmp(argument, "")) {
        for (int i = 0; i < 30; i++) {
          if (fantasy->participant_stats[chpoint][i] *
              fantasy->stat_offense_power[i] >
              max) {
            max = fantasy->participant_stats[chpoint][i] *
            fantasy->stat_offense_power[i] >
            max;
            statpoint = i;
          }
        }
      }
      else {
        for (int i = 0; i < 20; i++) {
          if (!str_cmp(fantasy->stats[i], argument))
          statpoint = i;
        }
      }
      if (statpoint == -1) {
        send_to_char("You have no offensive stats.\n\r", ch);
        return;
      }
      int maxoff = fantasy->participant_stats[chpoint][statpoint] *
      fantasy->stat_offense_power[statpoint];
      int defpoint = -1;
      max = 0;
      for (int i = 0; i < 30; i++) {
        if (fantasy->participant_stats[vicpoint][i] *
            fantasy->stat_defense_power[i] >
            max) {
          max = fantasy->participant_stats[vicpoint][i] *
          fantasy->stat_defense_power[i];
          defpoint = i;
        }
      }
      int maxdef = max;
      ch->pcdata->dream_attack_cool = 10;
      if (number_range(0, maxoff + chtotal) >
          number_range(0, maxdef + victotal) || defpoint == -1) {
        ch->pcdata->tempdreamgodworld = fantasy_number(fantasy);
        ch->pcdata->tempdreamgodchar = vicpoint;
        if (defpoint > -1) {
          printf_to_char(ch, "You defeat %s with %s, despite their attempt to defend with %s. You now have dreamgod powers over them until you next log off.\n\r", dream_name(victim), fantasy->stats[statpoint], fantasy->stats[defpoint]);
          printf_to_char(victim, "%s defeats you with %s, despite your attempt to defend yourself with %s.\n\r", dream_name(ch), fantasy->stats[statpoint], fantasy->stats[defpoint]);
        }
        else {
          printf_to_char(ch, "You defeat %s with %s. You now have dreamgod powers over them until you next log off.\n\r", dream_name(victim), fantasy->stats[statpoint]);
          printf_to_char(victim, "%s defeats you with %s.\n\r", dream_name(ch), fantasy->stats[statpoint]);
        }
      }
      else {
        printf_to_char(ch, "You attack %s with %s, but they successfully defend themselves using %s.\n\r", dream_name(victim), fantasy->stats[statpoint], fantasy->stats[defpoint]);
        printf_to_char(victim, "%s attacks you with %s, but you successfully defend yourself with %s.\n\r", dream_name(ch), fantasy->stats[statpoint], fantasy->stats[defpoint]);
      }
    }
    else if (!str_cmp(arg1, "roster")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      sprintf(buf, "Roster for %s\n\r", fantasy->name);
      strcat(string, buf);
      for (int i = 0; i < 200; i++) {
        if (safe_strlen(fantasy->participant_shorts[i]) > 1) {
          if (safe_strlen(fantasy->participant_fames[i]) > 1)
          sprintf(buf, "`W%02d`c)`x %s, %s[%s]\n   Known for %s.\n\r", i + 1, fantasy->participant_names[i], fantasy->participant_shorts[i], fantasy->roles[fantasy->participant_role[i]], fantasy->participant_fames[i]);
          else
          sprintf(buf, "`W%02d`c)`x %s, %s[%s]\n\r", i + 1, fantasy->participant_names[i], fantasy->participant_shorts[i], fantasy->roles[fantasy->participant_role[i]]);
          strcat(string, buf);
        }
      }
      page_to_char(string, ch);
      return;
    }
    else if (!str_cmp(arg1, "scoresheet") || !str_cmp(arg1, "score")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fantasy->participants[i], ch->name)) {
          sprintf(buf, "`CScoresheet for %s`x\n\r", fantasy->name);
          strcat(string, buf);
          sprintf(buf, "`cName:`x %s\n\n\r", fantasy->participant_names[i]);
          strcat(string, buf);
          sprintf(buf, "`cIntro:`x %s\n\n\r", fantasy->participant_shorts[i]);
          strcat(string, buf);
          sprintf(buf, "`cFame:`x They are known for %s.\n\n\r", fantasy->participant_fames[i]);
          strcat(string, buf);
          sprintf(buf, "`cRole:`x %s\n%s\n\n\r", fantasy->roles[fantasy->participant_role[i]], fantasy->role_desc[fantasy->participant_role[i]]);
          strcat(string, buf);
          sprintf(buf, "`cDescription:`x\n%s\n\r", fantasy->participant_descs[i]);
          strcat(string, buf);
          sprintf(buf, "`cEquipment:`x\n%s\n\r", fantasy->participant_eq[i]);
          strcat(string, buf);

          if (fantasy->participant_blind[i] == TRUE) {
            sprintf(buf, "You can't see in this dreamworld.\n\r");
            strcat(string, buf);
          }
          if (fantasy->participant_noaction[i] == TRUE) {
            sprintf(buf, "You can't act in this dreamworld.\n\r");
            strcat(string, buf);
          }
          if (fantasy->participant_nomove[i] == TRUE) {
            sprintf(buf, "You can't move in this dreamworld.\n\r");
            strcat(string, buf);
          }
          int available = ch->pcdata->dexp / UMAX(1, fantasy->expdebuff) +
          fantasy->startingexp + fantasy->participant_exp[i];
          available -= fantasy->role_cost[fantasy->participant_role[i]];
          for (int x = 0; x < 30; x++) {
            if (safe_strlen(fantasy->stats[x]) > 1)
            available -= fantasy->participant_stats[i][x] * fantasy->statcost;
          }
          sprintf(buf, "`cAvailable Experience:`x %d\n\n`cStats:`x\n\r", available);
          strcat(string, buf);
          for (int j = 0; j < 30; j++) {
            if (safe_strlen(fantasy->stats[j]) > 1) {
              if (fantasy->role_stats[fantasy->participant_role[i]][j] != 0) {
                sprintf(buf, "%s: %d\n\r", fantasy->stats[j], fantasy->participant_stats[i][j]);
                strcat(string, buf);
              }
            }
          }
          page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
          return;
        }
      }
      send_to_char("You aren't part of that dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "emit")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (fantasy->emit == 0) {
        fantasy->emit = 1;
        send_to_char("You enable emits in your world.\n\r", ch);
        return;
      }
      else {
        fantasy->emit = 0;
        send_to_char("You disable emits in your world.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "notify")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Notify the dreamworld of what?\n\r", ch);
        return;
      }
      sprintf(buf, "New notification for the dreamworld %s: %s", fantasy->name, argument);
      for (int i = 0; i < 200; i++) {
        if (safe_strlen(fantasy->participants[i]) > 1) {
          CHAR_DATA *vic;
          if ((vic = get_char_world_pc(fantasy->participants[i])) != NULL) {
            append_messages(vic, buf);
            printf_to_char(vic, "%s\n\r", buf);
          }
          else
          message_to_char(fantasy->participants[i], buf);
        }
      }
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "rename")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 3) {
        send_to_char("Syntax: dreamworld rename (number) (new name)\n\r", ch);
        return;
      }
      free_string(fantasy->name);
      fantasy->name = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "newrole")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], argument)) {
          send_to_char("There's already a role with that name.\n\r", ch);
          return;
        }
        if (!str_cmp(fantasy->roles[x], "")) {
          free_string(fantasy->roles[x]);
          fantasy->roles[x] = str_dup(argument);
          printf_to_char(ch, "You create the role %s, use roledescribe, rolecost, roleaddstat to configure it.\n\r", argument);
          return;
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "deleterole")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], argument)) {
          free_string(fantasy->roles[x]);
          fantasy->roles[x] = str_dup("");
          free_string(fantasy->role_desc[x]);
          fantasy->role_desc[x] = str_dup("");
          for (int y = 0; y < 20; y++)
          fantasy->role_stats[x][y] = 0;
          fantasy->role_cost[x] = 0;
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "saferoom")) {
      fantasy = in_fantasy(ch);
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int x = 0; x < 100; x++) {
        if (fantasy->rooms[x] == ch->pcdata->dream_room) {
          if (fantasy->safe_room[x] == 0) {
            fantasy->safe_room[x] = 1;
            send_to_char("You make this room safe from combat.\n\r", ch);
            return;
          }
          else {
            fantasy->safe_room[x] = 0;
            send_to_char("You make this room unsafe.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("Room not found.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "roledescribe")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], argument)) {
          string_append(ch, &fantasy->role_desc[x]);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "renamerole")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], arg3)) {
          free_string(fantasy->roles[x]);
          fantasy->roles[x] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "renamestat")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      for (int x = 0; x < 30; x++) {
        if (!str_cmp(fantasy->stats[x], arg3)) {
          free_string(fantasy->stats[x]);
          fantasy->stats[x] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "rolecost")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      bool zerocost = FALSE;
      for (int x = 0; x < 20; x++) {
        if (str_cmp(fantasy->roles[x], argument) && fantasy->role_cost[x] == 0 && str_cmp(fantasy->roles[x], ""))
        zerocost = TRUE;
      }
      int cost = atoi(arg3);
      if (cost > 0 && zerocost == FALSE) {
        send_to_char("At least one role must have 0 cost.\n\r", ch);
        return;
      }
      if (cost < 0) {
        send_to_char("Cost cannot be less than 0.\n\r", ch);
        return;
      }
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], argument)) {
          fantasy->role_cost[x] = cost;
          printf_to_char(ch, "Cost for %s is now %d dream exp.\n\r", argument, cost);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "addexit")) {
      fantasy = in_fantasy(ch);
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      ROOM_INDEX_DATA *room = get_poroom(atoi(arg2));
      argument = one_argument_nouncap(argument, arg3);
      bool found = FALSE;
      if (room == NULL) {
        send_to_char("No such room.\n\r", ch);
        return;
      }
      for (int x = 0; x < 100; x++) {
        if (fantasy->rooms[x] == room->vnum)
        found = TRUE;
      }
      if (found == FALSE) {
        send_to_char("That room isn't in your fantasy.\n\r", ch);
        return;
      }
      for (int x = 0; x < 100; x++) {
        if (fantasy->rooms[x] == ch->pcdata->dream_room) {
          for (int y = 0; y < 200; y++) {
            if (fantasy->exits[y] == 0) {
              fantasy->exits[y] = ch->pcdata->dream_room;
              fantasy->entrances[y] = room->vnum;
              free_string(fantasy->exit_name[y]);
              fantasy->exit_name[y] = str_dup(argument);
              free_string(fantasy->exit_alias[y]);
              fantasy->exit_alias[y] = str_dup(arg3);
              send_to_char("Exit made.\n\r", ch);
              return;
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "deleteexit")) {
      fantasy = in_fantasy(ch);
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (fantasy->exits[i] == ch->pcdata->dream_room && (!str_cmp(argument, fantasy->exit_name[i]) || !str_cmp(argument, fantasy->exit_alias[i]))) {
          fantasy->exits[i] = 0;
          fantasy->entrances[i] = 0;
          free_string(fantasy->exit_name[i]);
          fantasy->exit_name[i] = str_dup("");
          free_string(fantasy->exit_alias[i]);
          fantasy->exit_alias[i] = str_dup("");
          send_to_char("Exit removed.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "roleaddstat")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], arg3)) {
          for (int y = 0; y < 30; y++) {
            if (!str_cmp(argument, fantasy->stats[y])) {
              fantasy->role_stats[x][y] = 1;
              printf_to_char(ch, "Stat %s added to role %s.\n\r", fantasy->stats[y], fantasy->roles[x]);
              return;
            }
          }
          send_to_char("No such stat.\n\r", ch);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "roleremovestat")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(fantasy->roles[x], arg3)) {
          for (int y = 0; y < 30; y++) {
            if (!str_cmp(argument, fantasy->stats[y])) {
              fantasy->role_stats[x][y] = 0;
              printf_to_char(ch, "Stat %s removed from role %s.\n\r", fantasy->stats[y], fantasy->roles[x]);
              return;
            }
          }
          send_to_char("No such stat.\n\r", ch);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "transfer")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name) && !IS_IMMORTAL(ch)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld transfer (number) (new charcter name)\n\r", ch);
        return;
      }
      CHAR_DATA *newvict = get_char_world_pc(argument);
      if (newvict == NULL) {
        send_to_char("Taret new author is not online.\n\r", ch);
        return;
      }
      free_string(fantasy->author);
      fantasy->author = str_dup(argument);
      if (higher_power(newvict)) {
        free_string(fantasy->domain);
        fantasy->domain = str_dup(newvict->name);
      }
      else {
        free_string(fantasy->domain);
        fantasy->domain = str_dup("");
      }
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        bool indream = FALSE;
        for (int i = 0; i < 100; i++) {
          if (fantasy->rooms[i] == (*it)->vnum)
          indream = TRUE;
        }
        if (indream == TRUE) {
          free_string((*it)->author);
          (*it)->author = str_dup(argument);
        }
      }

      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "description")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit Try dreamworld (number) mydescription.\n\r", ch);
        return;
      }
      string_append(ch, &fantasy->description);
      return;
    }
    else if (!str_cmp(arg1, "doors")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit Try dreamworld (number) mydescription.\n\r", ch);
        return;
      }
      send_to_char("You can now edit the description of your dreamworld as seen through the nightmare doors, it will appear as 'A door open, giving a glimps of (text)'\n\r", ch);
      string_append(ch, &fantasy->elevator);
      return;
    }
    else if (!str_cmp(arg1, "petition")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (!trusted_dreamer(ch, fantasy, -1)) {
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: petition dreamworld (number) (message)\n\r", ch);
          return;
        }
        sprintf(buf, "%s\n[%s]%s", fantasy->petitions, dream_name(ch), argument);
        free_string(fantasy->petitions);
        fantasy->petitions = str_dup(buf);
        send_to_char("Petition sent.\n\r", ch);
        if (get_char_world_pc(fantasy->author) != NULL)
        send_to_char("New dreamworld petition.\n\r", get_char_world_pc(fantasy->author));
        for (int y = 0; y < 200; y++) {
          if (safe_strlen(fantasy->participants[y]) > 1 && fantasy->participant_trusted[y] == TRUE && get_char_world_pc(fantasy->participants[y]) != NULL)
          send_to_char("New dreamworld petition.\n\r", get_char_world_pc(fantasy->participants[y]));
        }
        return;
      }
      else
      string_append(ch, &fantasy->petitions);
      return;
    }
    else if (!str_cmp(arg1, "startingexp")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      int val = atoi(argument);
      if (val < -1000000 || val > 1000000) {
        send_to_char("Syntax: dreamworld startingexp (number) (exp amount)\n\r", ch);
        return;
      }
      fantasy->startingexp = val;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "expdebuff")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      int val = atoi(argument);
      if (val < 1 || val > 10000) {
        send_to_char("Syntax: dreamworld expdebuff (number) (debuff amount)\n\r", ch);
        return;
      }
      fantasy->expdebuff = val;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "delete")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name) && !IS_IMMORTAL(ch)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        d = *it;
        if (d->connected == CON_PLAYING) {
          if (is_dreaming(d->character) && d->character->pcdata->dream_room != 0) {
            for (int x = 0; x < 100; x++) {
              if (fantasy->rooms[x] == d->character->pcdata->dream_room)
              d->character->pcdata->dream_room = 0;
            }
          }
        }
      }

      fantasy->valid = FALSE;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "lock") && IS_IMMORTAL(ch)) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (fantasy->locked == 0) {
        fantasy->locked = 1;
        fantasy->active = FALSE;
        send_to_char("Fantasy locked.\n\r", ch);
        return;
      }
      else {
        fantasy->locked = 0;
        fantasy->active = TRUE;
        send_to_char("Fantasy unlocked.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "deactivate")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name) && !IS_IMMORTAL(ch)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (fantasy->active == FALSE) {
        send_to_char("It's already not active.\n\r", ch);
        return;
      }
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        d = *it;
        if (d->connected == CON_PLAYING) {
          if (is_dreaming(d->character) && d->character->pcdata->dream_room != 0) {
            for (int x = 0; x < 100; x++) {
              if (fantasy->rooms[x] == d->character->pcdata->dream_room)
              d->character->pcdata->dream_room = 0;
            }
          }
        }
      }

      fantasy->active = FALSE;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "activate")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (fantasy->active == TRUE) {
        send_to_char("It's already active.\n\r", ch);
        return;
      }
      if (fantasy->locked == 1) {
        send_to_char("This dreamworld is locked, review help dreamworld requirements and petition staff after making the required alterations.\n\r", ch);
        return;
      }

      fantasy->active = TRUE;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "statcost")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (fantasy->statcost > 0) {
        send_to_char("That's already been set.\n\r", ch);
        return;
      }
      int val = atoi(argument);
      if (val < 1 || val > 1000000) {
        send_to_char("Syntax: dreamworld statcost (number) (exp amount)\n\r", ch);
        return;
      }
      fantasy->statcost = val;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "statadd")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (fantasy->statcost == 0) {
        send_to_char("You need to set your statcost first.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld statadd (stat name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 30; i++) {
        if (!str_cmp(fantasy->stats[i], argument)) {
          send_to_char("That stat already exists.\n\r", ch);
          return;
        }
      }
      for (int i = 0; i < 30; i++) {
        if (safe_strlen(fantasy->stats[i]) < 2) {
          free_string(fantasy->stats[i]);
          fantasy->stats[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("You already have 20 stats in your dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "statremove")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (safe_strlen(fantasy->participants[0]) > 2) {
        send_to_char("People are already using this dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld statremove (stat name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 30; i++) {
        if (!str_cmp(fantasy->stats[i], argument)) {
          free_string(fantasy->stats[i]);
          fantasy->stats[i] = str_dup("");
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("No such stat.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "statdescribe")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld statdescribe (stat name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 30; i++) {
        if (!str_cmp(fantasy->stats[i], argument)) {
          string_append(ch, &fantasy->stat_desc[i]);
          return;
        }
      }
      send_to_char("No such stat.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "statinfo")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }

      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld statinfo (stat name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 30; i++) {
        if (!str_cmp(fantasy->stats[i], argument)) {
          if (trusted_dreamer(ch, fantasy, -1)) {
            printf_to_char(ch, "Offense: %d\nDefense: %d\nDesc:%s\n\r", fantasy->stat_offense_power[i], fantasy->stat_defense_power[i], fantasy->stat_desc[i]);
          }
          else
          send_to_char(fantasy->stat_desc[i], ch);
          return;
        }
      }
      send_to_char("No such stat.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "roleinfo")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }

      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld roleinfo (role name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 20; i++) {
        if (!str_cmp(fantasy->roles[i], argument) && fantasy->role_desc[i] != NULL) {
          printf_to_char(ch, "`W%s`x: %d\n%s\n\n", fantasy->roles[i], fantasy->role_cost[i], fantasy->role_desc[i]);
          return;
        }
      }
      send_to_char("No such role.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "reroll")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (!trusted_dreamer(ch, fantasy, -1)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          for (int j = 0; j < 30; j++)
          fantasy->participant_stats[i][j] = 0;
        }
      }
    }
    else if (!str_cmp(arg1, "ban")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (!trusted_dreamer(ch, fantasy, -1)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x] && vic->pcdata->dream_room != 0) {
                  vic->pcdata->dream_room = 0;
                  send_to_char("You are pushed out of the dreamworld.\n\r", vic);
                }
              }
            }
          }
          for (int x = 0; x < 100; x++) {
            if (safe_strlen(fantasy->ban_list[x]) < 1) {
              free_string(fantasy->ban_list[x]);
              fantasy->ban_list[x] = str_dup(fantasy->participants[i]);
              x = 101;
            }
          }
          for (int x = 0; x < 30; x++)
          fantasy->participant_stats[i][x] = 0;
          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup("");
          free_string(fantasy->participant_descs[i]);
          fantasy->participant_descs[i] = str_dup("");
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup("");
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup("");
          free_string(fantasy->participant_eq[i]);
          fantasy->participant_eq[i] = str_dup("");
          fantasy->participant_exp[i] = 0;
          fantasy->participant_role[i] = 0;
          fantasy->participant_blind[i] = FALSE;
          fantasy->participant_noaction[i] = FALSE;
          fantasy->participant_trusted[i] = FALSE;
          fantasy->participant_nomove[i] = FALSE;
          free_string(fantasy->participants[i]);
          fantasy->participants[i] = str_dup("");
          send_to_char("You ban them from your dreamworld.\n\r", ch);

          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "remove")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (!trusted_dreamer(ch, fantasy, -1)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x] && vic->pcdata->dream_room != 0) {
                  vic->pcdata->dream_room = 0;
                  send_to_char("You are pushed out of the dreamworld.\n\r", vic);
                }
              }
            }
          }
          for (int x = 0; x < 30; x++)
          fantasy->participant_stats[i][x] = 0;
          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup("");
          free_string(fantasy->participant_descs[i]);
          fantasy->participant_descs[i] = str_dup("");
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup("");
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup("");
          free_string(fantasy->participant_eq[i]);
          fantasy->participant_eq[i] = str_dup("");
          fantasy->participant_exp[i] = 0;
          fantasy->participant_role[i] = 0;
          fantasy->participant_blind[i] = FALSE;
          fantasy->participant_noaction[i] = FALSE;
          fantasy->participant_trusted[i] = FALSE;
          fantasy->participant_nomove[i] = FALSE;
          free_string(fantasy->participants[i]);
          fantasy->participants[i] = str_dup("");
          send_to_char("You kick them from your dreamworld.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "reactivate")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      sprintf(buf, "suspended%s", ch->name);
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fantasy->participants[i], buf)) {
          for (int j = 0; j < 200; j++) {
            if (!str_cmp(fantasy->participants[j], ch->name)) {
              for (int x = 0; x < 30; x++)
              fantasy->participant_stats[j][x] = 0;
              free_string(fantasy->participant_shorts[j]);
              fantasy->participant_shorts[j] = str_dup("");
              free_string(fantasy->participant_descs[j]);
              fantasy->participant_descs[j] = str_dup("");
              free_string(fantasy->participant_names[j]);
              fantasy->participant_names[j] = str_dup("");
              free_string(fantasy->participant_fames[j]);
              fantasy->participant_fames[j] = str_dup("");
              free_string(fantasy->participant_eq[j]);
              fantasy->participant_eq[j] = str_dup("");

              fantasy->participant_exp[j] = 0;
              fantasy->participant_role[j] = 0;
              fantasy->participant_blind[j] = FALSE;
              fantasy->participant_noaction[j] = FALSE;
              fantasy->participant_trusted[j] = FALSE;
              fantasy->participant_nomove[j] = FALSE;
              free_string(fantasy->participants[j]);
              fantasy->participants[j] = str_dup("");

              for (CharList::iterator it = char_list.begin();
              it != char_list.end(); ++it) {
                CHAR_DATA *wch = *it;
                if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
                continue;
                if (IS_NPC(wch))
                continue;
                if (wch->pcdata->tempdreamgodworld == fantasy_number(fantasy) && wch->pcdata->tempdreamgodchar == j)
                wch->pcdata->tempdreamgodchar = i;
              }
            }
          }
          free_string(fantasy->participants[i]);
          fantasy->participants[i] = str_dup(ch->name);
          printf_to_char(ch, "You become %s once more.\n\r", dream_name(ch));
          return;
        }
      }
      send_to_char("You don't have a suspended identity in that dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "blind")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  if (fantasy->participant_blind[i] == FALSE)
                  send_to_char("You are blinded!.\n\r", vic);
                  else
                  send_to_char("Your blindness is lifted.\n\r", vic);
                }
              }
            }
          }
          if (fantasy->participant_blind[i] == TRUE) {
            fantasy->participant_blind[i] = FALSE;
            send_to_char("You lift their blindness.\n\r", ch);
            return;
          }
          fantasy->participant_blind[i] = TRUE;
          send_to_char("You make them blind.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "noaction")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  if (fantasy->participant_noaction[i] == FALSE)
                  send_to_char("You are rendered unable to act!\n\r", vic);
                  else
                  send_to_char("You are able to act once more.\n\r", vic);
                }
              }
            }
          }
          if (fantasy->participant_noaction[i] == TRUE) {
            fantasy->participant_noaction[i] = FALSE;
            send_to_char("You let them act again.\n\r", ch);
            return;
          }
          fantasy->participant_noaction[i] = TRUE;
          send_to_char("You make them unable to act.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "drag")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  if (ch->dream_dragging == vic) {
                    printf_to_char(vic, "%s stops dragging you.\n\r", dream_name(ch));
                    printf_to_char(ch, "You stop dragging %s.\n\r", dream_name(vic));
                    ch->dream_dragging = NULL;
                  }
                  else {
                    if (fantasy->participant_noaction[i] == FALSE && fantasy->participant_nomove[i] == FALSE) {
                      send_to_char("They have to be unable to act or move first.\n\r", ch);
                      return;
                    }
                    ch->dream_dragging = vic;
                    printf_to_char(vic, "%s starts to drag you.\n\r", dream_name(ch));
                    printf_to_char(ch, "You start to drag %s.\n\r", dream_name(vic));
                  }
                }
              }
            }
          }
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "nomove")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  if (fantasy->participant_nomove[i] == FALSE)
                  send_to_char("You are rendered unable to move!\n\r", vic);
                  else
                  send_to_char("You are able to move once more.\n\r", vic);
                }
              }
            }
          }
          if (fantasy->participant_nomove[i] == TRUE) {
            fantasy->participant_nomove[i] = FALSE;
            send_to_char("You let them move again.\n\r", ch);
            return;
          }
          fantasy->participant_nomove[i] = TRUE;
          send_to_char("You make them unable to move.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "remove")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (!trusted_dreamer(ch, fantasy, -1)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x] && vic->pcdata->dream_room != 0) {
                  vic->pcdata->dream_room = 0;
                  send_to_char("You are pushed out of the dreamworld.\n\r", vic);
                }
              }
            }
          }
          for (int x = 0; x < 30; x++)
          fantasy->participant_stats[i][x] = 0;
          fantasy->participant_exp[i] = 0;
          fantasy->participant_role[i] = 0;
          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup("");
          free_string(fantasy->participant_descs[i]);
          fantasy->participant_descs[i] = str_dup("");
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup("");
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup("");
          free_string(fantasy->participant_eq[i]);
          fantasy->participant_eq[i] = str_dup("");
          fantasy->participant_blind[i] = FALSE;
          fantasy->participant_noaction[i] = FALSE;
          fantasy->participant_trusted[i] = FALSE;
          free_string(fantasy->participants[i]);
          fantasy->participants[i] = str_dup("");
          send_to_char("You kick them from your dreamworld.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "newintro")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      argument = one_argument_nouncap(argument, arg3);

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: Dreamworld newintro (person) (new intro)\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(arg3, fantasy->participant_names[i]) || atoi(arg3) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  send_to_char("Your intro within this dream world is changed.\n\r", vic);
                }
              }
            }
          }
          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "newdescription") || !str_cmp(arg1, "newdesc")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  send_to_char("Your desciption within this dream world is being changed.\n\r", vic);
                }
              }
            }
          }
          string_append(ch, &fantasy->participant_descs[i]);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "trust")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(ch->name, fantasy->author)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (fantasy->participant_trusted[i] == TRUE) {
            fantasy->participant_trusted[i] = FALSE;
            send_to_char("You remove their trusted access.\n\r", ch);
            if (get_char_world_pc(fantasy->participants[i]) != NULL)
            send_to_char("You lose your trust in the dream world.\n\r", get_char_world_pc(fantasy->participants[i]));
            return;
          }
          else {
            fantasy->participant_trusted[i] = TRUE;
            send_to_char("You give them trusted access.\n\r", ch);
            if (get_char_world_pc(fantasy->participants[i]) != NULL)
            send_to_char("You gain trusted access in the dream world.\n\r", get_char_world_pc(fantasy->participants[i]));
            return;
          }
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "join")) {
      fantasy = fetch_fantasy(ch, atoi(argument));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          send_to_char("You're already a part of that dreamworld.\n\r", ch);
          return;
        }
      }
      for (int i = 0; i < 100; i++) {
        if (!str_cmp(ch->name, fantasy->ban_list[i])) {
          send_to_char("You cannot join that dream world.\n\r", ch);
          return;
        }
      }
      for (int i = 0; i < 200; i++) {
        if (safe_strlen(fantasy->participants[i]) < 1) {
          free_string(fantasy->participants[i]);
          fantasy->participants[i] = str_dup(ch->name);
          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup("");
          free_string(fantasy->participant_descs[i]);
          fantasy->participant_descs[i] = str_dup("");
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup("");
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup("");
          free_string(fantasy->participant_eq[i]);
          fantasy->participant_eq[i] = str_dup("");
          fantasy->participant_role[i] = 0;
          fantasy->participant_exp[i] = 0;
          for (int x = 0; x < 30; x++)
          fantasy->participant_stats[i][x] = 0;
          send_to_char("You join the dreamworld! You'll have to set your intro and desc before you can enter it however.\n\r", ch);
          return;
        }
      }
      send_to_char("That dreamworld is too full\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "myintro") || !str_cmp(arg1, "myintroduction")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld myintro (new intro)\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {

          if (safe_strlen(fantasy->participant_shorts[i]) > 2) {
            send_to_char("You already have an intro in this dreamworld.\n\r", ch);
            return;
          }

          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("You are not part of that dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "myname")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld myname (new name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          if (safe_strlen(fantasy->participant_names[i]) > 2) {
            send_to_char("You already have a name in this dreamworld.\n\r", ch);
            return;
          }
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("You are not part of that dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "newname")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(arg3, fantasy->participant_names[i]) || atoi(arg3) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(arg3, fantasy->participant_names[i]) || atoi(arg3) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  send_to_char("Your name within this dream world is being changed.\n\r", vic);
                }
              }
            }
          }
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "myfame")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: dreamworld myfame (new fame)\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("You are not part of that dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "newfame")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(arg3, fantasy->participant_names[i]) || atoi(arg3) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(arg3, fantasy->participant_names[i]) || atoi(arg3) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  send_to_char("Your fame within this dream world is being changed.\n\r", vic);
                }
              }
            }
          }
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);

          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "takerole")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Dreamworld takerole (number) (role)\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          int available = ch->pcdata->dexp / UMAX(1, fantasy->expdebuff) +
          fantasy->startingexp + fantasy->participant_exp[i];
          if (fantasy->role_cost[fantasy->participant_role[i]] > 10) {
            send_to_char("You already have a role.\n\r", ch);
            return;
          }
          for (int x = 0; x < 30; x++) {
            if (safe_strlen(fantasy->stats[x]) > 1)
            available -= fantasy->participant_stats[i][x] * fantasy->statcost;
          }
          for (int j = 0; j < 20; j++) {
            if (!str_cmp(fantasy->roles[j], argument)) {
              if (available < fantasy->role_cost[j]) {
                printf_to_char(
                ch, "You'd need %d more dream experience to become that.\n\r", fantasy->role_cost[j] - available);
                return;
              }
              fantasy->participant_role[i] = j;
              printf_to_char(ch, "You become %s.\n\r", fantasy->roles[j]);
              for (int x = 0; x < 30; x++) {
                fantasy->participant_stats[i][x] = 0;
              }
              return;
            }
          }
          send_to_char("No such role.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "buystat")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Dreamworld buystat (number) (statname)\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          int available = ch->pcdata->dexp / UMAX(1, fantasy->expdebuff) +
          fantasy->startingexp + fantasy->participant_exp[i];
          available -= fantasy->role_cost[fantasy->participant_role[i]];
          for (int x = 0; x < 30; x++) {
            if (safe_strlen(fantasy->stats[x]) > 1)
            available -= fantasy->participant_stats[i][x] * fantasy->statcost;
          }
          if (available < fantasy->statcost) {
            send_to_char("You don't have enough dreamscape experience to raise that.\n\r", ch);
            return;
          }
          for (int x = 0; x < 30; x++) {
            if (!str_cmp(argument, fantasy->stats[x])) {
              if (fantasy->role_stats[fantasy->participant_role[i]][x] == 0) {
                send_to_char("Your role cannot take that stat.\n\r", ch);
                return;
              }
              fantasy->participant_stats[i][x]++;
              send_to_char("You raise your stat.\n\r", ch);
              return;
            }
          }
          send_to_char("No such stat in that dreamworld.\n\r", ch);
          return;
        }
      }
      send_to_char("You're not part of that dream world.\n\r", ch);
    }
    else if (!str_cmp(arg1, "mydesc") || !str_cmp(arg1, "mydescription")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {

          if (safe_strlen(fantasy->participant_descs[i]) > 2) {
            send_to_char("You already have a desc in this dreamworld.\n\r", ch);
            return;
          }

          string_append(ch, &fantasy->participant_descs[i]);
          return;
        }
      }
      send_to_char("You are not part of that dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "myequipment") || !str_cmp(arg1, "myeq")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          string_append(ch, &fantasy->participant_eq[i]);

          return;
        }
      }
      send_to_char("You are not part of that dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "newequipment")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          point = i;
        }
      }
      if (!trusted_dreamer(ch, fantasy, point)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(argument, fantasy->participant_names[i]) || atoi(argument) == i + 1) {
          if (get_char_world_pc(fantasy->participants[i]) != NULL) {
            CHAR_DATA *vic = get_char_world_pc(fantasy->participants[i]);
            if (is_dreaming(vic) && vic->pcdata->dream_room != 0) {
              for (int x = 0; x < 100; x++) {
                if (vic->pcdata->dream_room == fantasy->rooms[x]) {
                  send_to_char("Your equipment within this dream world is being changed.\n\r", vic);
                }
              }
            }
          }
          string_append(ch, &fantasy->participant_eq[i]);

          return;
        }
      }
      send_to_char("They're not in your dreamworld.\n\r", ch);
    }
    else if (!str_cmp(arg1, "retire")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fantasy->participants[i])) {
          if (is_dreaming(ch) && ch->pcdata->dream_room > 0) {
            do_function(ch, &do_wake, "");
            for (int x = 0; x < 100; x++) {
              if (ch->pcdata->dream_room == fantasy->rooms[x])
              ch->pcdata->dream_room = 0;
            }
          }
          free_string(fantasy->participant_descs[i]);
          fantasy->participant_descs[i] = str_dup("");
          free_string(fantasy->participant_shorts[i]);
          fantasy->participant_shorts[i] = str_dup("");
          free_string(fantasy->participant_names[i]);
          fantasy->participant_names[i] = str_dup("");
          free_string(fantasy->participant_fames[i]);
          fantasy->participant_fames[i] = str_dup("");
          free_string(fantasy->participant_eq[i]);
          fantasy->participant_eq[i] = str_dup("");
          fantasy->participant_blind[i] = FALSE;
          fantasy->participant_noaction[i] = FALSE;
          fantasy->participant_trusted[i] = FALSE;
          free_string(fantasy->participants[i]);
          fantasy->participants[i] = str_dup("");
          send_to_char("You retire from the dreamworld.\n\r", ch);
          if (!str_cmp(fantasy->name, ch->pcdata->dream_origin)) {
            free_string(ch->pcdata->dream_origin);
            ch->pcdata->dream_origin = str_dup("");
          }
          return;
        }
      }
      send_to_char("You are not part of that dreamworld.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "rescue")) {
      FANTASY_TYPE *fant = in_fantasy(ch);
      if (fant == NULL) {
        send_to_char("You're not in a dream world.\n\r", ch);
        return;
      }
      if (ch->race == RACE_FANTASY) {
        send_to_char("You have no interest in the problems of the waking world.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->comm, COMM_DREAMSNARED)) {
        send_to_char("You are also stranded.\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fant->participants[i], ch->name)) {
          if (fant->participant_exp[i] < 100) {
            printf_to_char(ch, "You'd need to earn at least %d more dream experience in this world first.\n\r", 100 - fant->participant_exp[i]);
            return;
          }
          CHAR_DATA *victim = get_char_dream(ch, argument);
          if (victim == NULL || IS_NPC(victim) || victim->pcdata->dream_room != ch->pcdata->dream_room || ch == victim) {
            send_to_char("You don't see anyone like that here.\n\r", ch);
            return;
          }
          fant->participant_exp[i] -= 100;
          if (IS_FLAG(victim->comm, COMM_DREAMSNARED)) {
            printf_to_char(ch, "You expend psychic energy and liberate %s.\n\r", dream_name(victim));
            printf_to_char(victim, "You feel an oppressive weight lift from you.\n\r");
            REMOVE_FLAG(victim->comm, COMM_DREAMSNARED);
            for (int x = 0; x < 200; x++) {
              if (!str_cmp(victim->name, fant->participants[x])) {
                fant->participant_noaction[x] = FALSE;
                fant->participant_blind[x] = FALSE;
                fant->participant_nomove[x] = FALSE;
              }
            }
            return;
          }
          else {
            printf_to_char(ch, "You expend psychic energy but nothing happens.\n\r");
            return;
          }
        }
      }
    }
    else if (!str_cmp(arg1, "statoffense")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      int power = atoi(arg3);
      if (power < 0 || power > 10000) {
        send_to_char("Choose a value between 0 and 10000.\n\r", ch);
        return;
      }
      for (int x = 0; x < 30; x++) {
        if (!str_cmp(fantasy->stats[x], argument)) {
          fantasy->stat_offense_power[x] = power;
          printf_to_char(ch, "Offensive power of %s is now %d.\n\r", argument, power);
          return;
        }
      }
      send_to_char("No such stat.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "statdefense")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));
      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      int power = atoi(arg3);
      if (power < 0 || power > 10000) {
        send_to_char("Choose a value between 0 and 10000.\n\r", ch);
        return;
      }
      for (int x = 0; x < 30; x++) {
        if (!str_cmp(fantasy->stats[x], argument)) {
          fantasy->stat_defense_power[x] = power;
          printf_to_char(ch, "Defensive power of %s is now %d.\n\r", argument, power);
          return;
        }
      }
      send_to_char("No such stat.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "addroom")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      ROOM_INDEX_DATA *addroom = get_room_index(ch->pcdata->dream_room);
      if (addroom == NULL || addroom->area->vnum != DIST_DREAM) {
        send_to_char("You have to be in the dream room you want to add.\n\r", ch);
        return;
      }
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        if ((*it)->vnum == ch->pcdata->dream_room) {
          if (str_cmp(ch->name, (*it)->author)) {
            send_to_char("That isn't your room.\n\r", ch);
            return;
          }

          for (int i = 0; i < 100; i++) {
            if (fantasy->rooms[i] == addroom->vnum) {
              send_to_char("That room is already part of that dreamworld.\n\r", ch);
              return;
            }
          }
          for (int i = 0; i < 100; i++) {
            if (fantasy->rooms[i] == 0) {
              fantasy->rooms[i] = addroom->vnum;
              send_to_char("Room added.\n\r", ch);
              return;
            }
          }
          send_to_char("This dreamworld already uses up 10 rooms.\n\r", ch);
          return;
        }
      }
      send_to_char("No such room.\n\r", ch);
    }
    else if (!str_cmp(arg1, "entrance")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (str_cmp(fantasy->author, ch->name)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      ROOM_INDEX_DATA *addroom = get_room_index(ch->pcdata->dream_room);
      if (addroom == NULL || addroom->area->vnum != DIST_DREAM) {
        send_to_char("You have to be in the dream room you want to add.\n\r", ch);
        return;
      }
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        if ((*it)->vnum == ch->pcdata->dream_room) {
          if (str_cmp(ch->name, (*it)->author)) {
            send_to_char("That isn't your room.\n\r", ch);
            return;
          }
          fantasy->entrance = addroom->vnum;
          send_to_char("Entrance set.\n\r", ch);
          return;
        }
      }
      send_to_char("No such room.\n\r", ch);
    }
    else if (!str_cmp(arg1, "godmode")) {
      argument = one_argument_nouncap(argument, arg2);
      fantasy = fetch_fantasy(ch, atoi(arg2));

      if (fantasy == NULL) {
        send_to_char("No such dreamworld.\n\r", ch);
        return;
      }
      if (!trusted_dreamer(ch, fantasy, -1)) {
        send_to_char("That isn't your dreamworld to edit.\n\r", ch);
        return;
      }
      if (!str_cmp(ch->name, fantasy->author)) {
        if (fantasy->godmode == TRUE) {
          send_to_char("You become a simple mortal dreamer.\n\r", ch);
          fantasy->godmode = FALSE;
          return;
        }
        else {
          send_to_char("You become the god of the dream world.\n\r", ch);
          fantasy->godmode = TRUE;
          return;
        }
      }
      else {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(fantasy->participants[i], ch->name)) {
            if (fantasy->participant_godmode[i] == TRUE) {
              fantasy->participant_godmode[i] = FALSE;
              send_to_char("You become a simple mortal dreamer.\n\r", ch);
              return;
            }
            else {
              fantasy->participant_godmode[i] = TRUE;
              send_to_char("You become a god of this world.\n\r", ch);
              return;
            }
          }
        }
      }
    }
    else
    send_to_char("Syntax: Dreamworld create/list/summary/fullinfo/roster/delete/newintro/description/startingexp/statcost/statadd/statremove/remove/ban/blind/noaction/join/myintro/mydesc/myname/myfame/myequipmentbuystat/score/newname/newdesc/activate/deactive/addroom/retire/godmode/petition/roll/trust/makedoor\n\r", ch);
  }

  FANTASY_TYPE *room_fantasy(ROOM_INDEX_DATA *room) {

    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->active == FALSE)
      continue;

      for (int i = 0; i < 100; i++) {
        if (room->vnum == (*it)->rooms[i])
        return (*it);
      }
    }
    return NULL;
  }

  FANTASY_TYPE *in_fantasy(CHAR_DATA *ch) {
    if (!is_dreaming(ch))
    return NULL;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->active == FALSE)
      continue;
      for (int i = 0; i < 100; i++) {
        if (ch->pcdata->dream_room == (*it)->rooms[i]) {
          return (*it);
        }
      }
    }
    return NULL;
  }

  bool part_of_fantasy(CHAR_DATA *ch, FANTASY_TYPE *fantasy) {
    for (int i = 0; i < 200; i++) {
      if (!str_cmp(ch->name, fantasy->participants[i])) {
        if (safe_strlen(fantasy->participant_shorts[i]) > 2 && safe_strlen(fantasy->participant_names[i]) > 1)
        return TRUE;
      }
    }
    return FALSE;
  }
  bool goddreamer(CHAR_DATA *ch) {
    if (!is_dreaming(ch))
    return FALSE;

    FANTASY_TYPE *fant = in_fantasy(ch);
    if (fant == NULL)
    return FALSE;

    if (fant->godmode == TRUE && !str_cmp(fant->author, ch->name))
    return TRUE;

    for (int i = 0; i < 200; i++) {
      if (!str_cmp(ch->name, fant->participants[i]) && fant->participant_trusted[i] == TRUE && fant->participant_godmode[i] == TRUE)
      return TRUE;
    }

    return FALSE;
  }

  bool valid_wilds_legendary(int stat) {

    if (skilltype(stat) != STYPE_ABOMINATION)
    return FALSE;
    if (stat == SKILL_FLIGHT)
    return TRUE;
    if (stat == SKILL_MINDREADING)
    return TRUE;
    if (stat == SKILL_HYPERSHIFTING)
    return TRUE;
    if (stat == SKILL_MEMORYCLOAK)
    return TRUE;
    if (stat == SKILL_DARKSORC)
    return TRUE;
    if (stat == SKILL_FIRESORC)
    return TRUE;
    if (stat == SKILL_ICESORC)
    return TRUE;
    if (stat == SKILL_LIGHTNINGSORC)
    return TRUE;
    if (stat == SKILL_PRISSORC)
    return TRUE;
    if (stat == SKILL_MONSTERSHIFT)
    return TRUE;
    if (stat == SKILL_HYBRIDSHIFTING)
    return TRUE;
    if (stat == SKILL_SWARMSHIFTING)
    return TRUE;

    return FALSE;
  }
  int wobjval(OBJ_DATA *obj) {
    int val = 0;
    val += obj->cost / 20;

    char arg1[MSL];
    char arg2[MSL];

    EXTRA_DESCR_DATA *ed;
    char *buf = str_dup("");
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+augmentone", ed->keyword)) {
        free_string(buf);
        buf = str_dup(ed->description);
        buf = one_argument_nouncap(buf, arg1);
        buf = one_argument_nouncap(buf, arg2);
        int level = atoi(arg2);
        if (level == 3)
        val += 20000;
        else if (level == 4)
        val += 50000;
        if (level < 3)
        return -1000;
      }
    }
    val = UMAX(val, 5000);
    return val;
  }

  _DOFUN(do_gift)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    CHAR_DATA *victim = get_char_world_pc(argument);
    if(victim == NULL || (victim->fcult != ch->pcdata->eidilon_of && victim->fsect != ch->pcdata->eidilon_of))
    {
      send_to_char("Nobody like that to gift something to.\n\r", ch);
      return;
    }
    FACTION_TYPE *cult = clan_lookup(ch->pcdata->eidilon_of);

    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (is_name(arg1, obj->name) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE && can_drop_obj(ch, obj)) {
        int amount = giftobjval(obj);
        int faccost = amount / 10;
        if(cult->type == FACTION_CULT && cult->axes[AXES_CORRUPT] >= AXES_NEUTRAL)
        faccost = faccost * 3/2;
        if(cult->type == FACTION_SECT)
        faccost = faccost * 6/5;

        if(cult->resource < 8000 + faccost)
        {
          send_to_char("Your cult doesn't have enough resources to gift that.\n\r", ch);
          return;
        }
        if (!IS_SET(obj->extra_flags, ITEM_CURSED))
        {
          send_to_char("You can only gift cursed items.\n\r", ch);
          return;
        }
        STORYLINE_TYPE *sline = get_sect_storyline(cult);
        if(cult->type == FACTION_SECT && (sline == NULL || sline->power < 5))
        {
          send_to_char("You need a storyline for your sect with more power than that first.\n\r", ch);
          return;
        }
        printf_to_char(ch, "For $%d society resources you gift an artifact\n\r", faccost * 10);
        use_resources(faccost, cult->vnum, ch, "gifting an artifact");
        if(cult->type == FACTION_SECT)
        {
          sline->power -= 5;
          sline->total_power -= 5;
        }
        if (!IS_SET(obj->extra_flags, ITEM_NORESALE))
        SET_BIT(obj->extra_flags, ITEM_NORESALE);
        obj_from_char(obj);
        obj_to_char(obj, victim);
        printf_to_char(victim, "You receive a gift.\n\r");
        return;

      }

    }
  }

  _DOFUN(do_wilds) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg0[MSL];

    argument = one_argument_nouncap(argument, arg0);
    argument = one_argument_nouncap(argument, arg1);

    int world = -1;
    if (!str_cmp(arg0, "wilds"))
    world = WORLD_WILDS;
    if (!str_cmp(arg0, "other"))
    world = WORLD_OTHER;
    if (!str_cmp(arg0, "godrealm"))
    world = WORLD_GODREALM;
    if (!str_cmp(arg0, "hell"))
    world = WORLD_HELL;

    if (!str_cmp(arg1, "experience")) {
      printf_to_char(ch, "`WWorlds Experience`x: %d\n\r", worlds_experience(ch, world));
      return;
    }

    if (!str_cmp(arg1, "fame")) {
      if (!str_cmp(arg0, "wilds")) {
        free_string(ch->pcdata->wilds_fame);
        ch->pcdata->wilds_fame = str_dup(argument);
        printf_to_char(ch, "You are now famous in the wilds for %s.\n\r", argument);
      }
      if (!str_cmp(arg0, "other")) {
        free_string(ch->pcdata->other_fame);
        ch->pcdata->other_fame = str_dup(argument);
        printf_to_char(ch, "You are now famous in the other for %s.\n\r", argument);
      }
      if (!str_cmp(arg0, "godrealm")) {
        free_string(ch->pcdata->godrealm_fame);
        ch->pcdata->godrealm_fame = str_dup(argument);
        printf_to_char(ch, "You are now famous in the godrealm for %s.\n\r", argument);
      }
      if (!str_cmp(arg0, "hell")) {
        free_string(ch->pcdata->hell_fame);
        ch->pcdata->hell_fame = str_dup(argument);
        printf_to_char(ch, "You are now famous in hell for %s.\n\r", argument);
      }
      return;
    }
    if (!str_cmp(arg1, "award")) {
      if (!is_gm(ch))
      return;

      argument = one_argument_nouncap(argument, arg2);
      CHAR_DATA *victim;
      victim = get_char_room(ch, NULL, arg2);
      if (victim == NULL || IS_NPC(victim)) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (ch == victim) {
        send_to_char("You cannot award yourself.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      if (!str_cmp(arg3, "basicfame")) {
        int award = worlds_experience(victim, world);
        if (award < 5000) {
          send_to_char("Either you or they don't have enough worlds experience.\n\r", ch);
          return;
        }
        if (world == WORLD_WILDS) {
          if (victim->pcdata->wilds_fame_level > 0) {
            send_to_char("They already have that level of fame in the wilds.\n\r", ch);
            return;
          }
          victim->pcdata->wilds_fame_level = 1;
          charge_worlds_exp(victim, world, 5000);
          send_to_char("You grant them basic fame in the wilds.\n\r", ch);
          send_to_char("You gain basic fame in the wilds, use worlds wilds fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_OTHER) {
          if (victim->pcdata->other_fame_level > 0) {
            send_to_char("They already have that level of fame in the other.\n\r", ch);
            return;
          }
          victim->pcdata->other_fame_level = 1;
          charge_worlds_exp(victim, world, 5000);
          send_to_char("You grant them basic fame in the other.\n\r", ch);
          send_to_char("You gain basic fame in the other, use worlds other fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_GODREALM) {
          if (victim->pcdata->godrealm_fame_level > 0) {
            send_to_char("They already have that level of fame in the godrealm.\n\r", ch);
            return;
          }
          victim->pcdata->godrealm_fame_level = 1;
          charge_worlds_exp(victim, world, 5000);
          send_to_char("You grant them basic fame in the godrealm.\n\r", ch);
          send_to_char("You gain basic fame in the other, use worlds godealm fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_HELL) {
          if (victim->pcdata->hell_fame_level > 0) {
            send_to_char("They already have that level of fame in hell.\n\r", ch);
            return;
          }
          victim->pcdata->hell_fame_level = 1;
          charge_worlds_exp(victim, world, 5000);
          send_to_char("You grant them basic fame in hell.\n\r", ch);
          send_to_char("You gain basic fame in the other, use worlds hell fame (string) to set it.\n\r", victim);
          return;
        }
        return;
      }
      if (!str_cmp(arg3, "moderatefame")) {
        int award = worlds_experience(victim, world);
        if (award < 20000) {
          send_to_char("Either you or they don't have enough worlds experience.\n\r", ch);
          return;
        }
        if (world == WORLD_WILDS) {
          if (victim->pcdata->wilds_fame_level > 1) {
            send_to_char("They already have that level of fame in the wilds.\n\r", ch);
            return;
          }
          if (victim->pcdata->wilds_fame_level < 0) {
            send_to_char("They have to gain basic fame first.\n\r", ch);
            return;
          }
          victim->pcdata->wilds_fame_level = 2;
          charge_worlds_exp(victim, world, 20000);
          send_to_char("You grant them moderate fame in the wilds.\n\r", ch);
          send_to_char("You gain moderate fame in the wilds, use worlds wilds fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_OTHER) {
          if (victim->pcdata->other_fame_level > 1) {
            send_to_char("They already have that level of fame in the other.\n\r", ch);
            return;
          }
          if (victim->pcdata->other_fame_level < 0) {
            send_to_char("They have to gain basic fame first.\n\r", ch);
            return;
          }
          victim->pcdata->other_fame_level = 2;
          charge_worlds_exp(victim, world, 20000);
          send_to_char("You grant them moderate fame in the other.\n\r", ch);
          send_to_char("You gain moderate fame in the other, use worlds other fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_GODREALM) {
          if (victim->pcdata->godrealm_fame_level > 1) {
            send_to_char("They already have that level of fame in the godrealm.\n\r", ch);
            return;
          }
          if (victim->pcdata->godrealm_fame_level < 0) {
            send_to_char("They have to gain basic fame first.\n\r", ch);
            return;
          }
          victim->pcdata->godrealm_fame_level = 2;
          charge_worlds_exp(victim, world, 20000);
          send_to_char("You grant them moderate fame in the godrealm.\n\r", ch);
          send_to_char("You gain moderate fame in the godrealm, use worlds godrealm fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_HELL) {
          if (victim->pcdata->hell_fame_level > 1) {
            send_to_char("They already have that level of fame in hell.\n\r", ch);
            return;
          }
          if (victim->pcdata->hell_fame_level < 0) {
            send_to_char("They have to gain basic fame first.\n\r", ch);
            return;
          }
          victim->pcdata->hell_fame_level = 2;
          charge_worlds_exp(victim, world, 20000);
          send_to_char("You grant them moderate fame in hell.\n\r", ch);
          send_to_char("You gain moderate fame in hell, use worlds hell fame (string) to set it.\n\r", victim);
          return;
        }
        return;
      }
      if (!str_cmp(arg3, "legendaryfame")) {
        int award = worlds_experience(victim, world);
        if (award < 50000) {
          send_to_char("Either you or they don't have enough worlds experience.\n\r", ch);
          return;
        }
        if (world == WORLD_WILDS) {
          if (victim->pcdata->wilds_fame_level > 2) {
            send_to_char("They already have that level of fame in the wilds.\n\r", ch);
            return;
          }
          if (victim->pcdata->wilds_fame_level < 1) {
            send_to_char("They have to gain moderate fame first.\n\r", ch);
            return;
          }
          victim->pcdata->wilds_fame_level = 3;
          charge_worlds_exp(victim, world, 50000);
          send_to_char("You grant them legendary fame in the wilds.\n\r", ch);
          send_to_char("You gain legendary fame in the wilds, use world wilds fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_OTHER) {
          if (victim->pcdata->other_fame_level > 2) {
            send_to_char("They already have that level of fame in the other.\n\r", ch);
            return;
          }
          if (victim->pcdata->other_fame_level < 1) {
            send_to_char("They have to gain moderate fame first.\n\r", ch);
            return;
          }
          victim->pcdata->other_fame_level = 3;
          charge_worlds_exp(victim, world, 50000);
          send_to_char("You grant them legendary fame in the other.\n\r", ch);
          send_to_char("You gain legendary fame in the other, use worlds other fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_GODREALM) {
          if (victim->pcdata->godrealm_fame_level > 2) {
            send_to_char("They already have that level of fame in the godrealm.\n\r", ch);
            return;
          }
          if (victim->pcdata->godrealm_fame_level < 1) {
            send_to_char("They have to gain moderate fame first.\n\r", ch);
            return;
          }
          victim->pcdata->godrealm_fame_level = 3;
          charge_worlds_exp(victim, world, 50000);
          send_to_char("You grant them legendary fame in the godrealm.\n\r", ch);
          send_to_char("You gain legendary fame in the godrealm, use world godrealm fame (string) to set it.\n\r", victim);
          return;
        }
        if (world == WORLD_HELL) {
          if (victim->pcdata->hell_fame_level > 2) {
            send_to_char("They already have that level of fame in hell.\n\r", ch);
            return;
          }
          if (victim->pcdata->hell_fame_level < 1) {
            send_to_char("They have to gain moderate fame first.\n\r", ch);
            return;
          }
          victim->pcdata->hell_fame_level = 3;
          charge_worlds_exp(victim, world, 50000);
          send_to_char("You grant them legendary fame in hell.\n\r", ch);
          send_to_char("You gain legendary fame in hell, use world hell fame (string) to set it.\n\r", victim);
          return;
        }
        return;
      }
      if (!str_cmp(arg3, "legendarypower")) {
        int award = worlds_experience(victim, world);
        if (award < 100000) {
          send_to_char("Either you or they don't have enough worlds experience.\n\r", ch);
          return;
        }
        int point = -1;
        for (int i = 0; i < SKILL_USED; i++) {
          if (!str_cmp(skill_table[i].name, argument)) {
            if (!valid_wilds_legendary(skill_table[i].vnum)) {
              send_to_char("That isn't a legendary power that can be mastered in another world.\n\r", ch);
              return;
            }
            point = skill_table[i].vnum;
          }
        }
        if (point == -1) {
          send_to_char("No such stat.\n\r", ch);
          return;
        }
        if (world == WORLD_WILDS) {
          if (victim->pcdata->wilds_legendary > 0) {
            send_to_char("They already have a legendary wilds power.\n\r", ch);
            return;
          }
          victim->pcdata->wilds_legendary = point;
          charge_worlds_exp(victim, world, 100000);
          send_to_char("You grant them a legendary wilds power.\n\r", ch);
          printf_to_char(victim, "You gain the legendary power %s in the wilds.\n\r", argument);
          return;
        }
        if (world == WORLD_GODREALM) {
          if (victim->pcdata->godrealm_legendary > 0) {
            send_to_char("They already have a legendary godrealms power.\n\r", ch);
            return;
          }
          victim->pcdata->godrealm_legendary = point;
          charge_worlds_exp(victim, world, 100000);
          send_to_char("You grant them a legendary godrealm power.\n\r", ch);
          printf_to_char(victim, "You gain the legendary power %s in the godrealm.\n\r", argument);
          return;
        }
        if (world == WORLD_OTHER) {
          if (victim->pcdata->other_legendary > 0) {
            send_to_char("They already have a legendary other power.\n\r", ch);
            return;
          }
          victim->pcdata->other_legendary = point;
          charge_worlds_exp(victim, world, 100000);
          send_to_char("You grant them a legendary other power.\n\r", ch);
          printf_to_char(victim, "You gain the legendary power %s in the other.\n\r", argument);
          return;
        }
        if (world == WORLD_HELL) {
          if (victim->pcdata->hell_legendary > 0) {
            send_to_char("They already have a legendary hell power.\n\r", ch);
            return;
          }
          victim->pcdata->hell_legendary = point;
          charge_worlds_exp(victim, world, 100000);
          send_to_char("You grant them a legendary hell power.\n\r", ch);
          printf_to_char(victim, "You gain the legendary power %s in hell.\n\r", argument);
          return;
        }
        return;
      }
      /*
if (!str_cmp(arg3, "item")) {
OBJ_DATA *obj;
OBJ_DATA *obj_next;
for (obj = ch->carrying; obj != NULL; obj = obj_next) {
obj_next = obj->next_content;

if (is_name(argument, obj->name) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE && can_drop_obj(ch, obj)) {
int amount = wobjval(obj);
if ((amount <= 0 || amount > worlds_experience(victim, world)) && !IS_IMMORTAL(ch)) {
printf_to_char(
ch, "That object would cost %d worlds exp to transfer.\n\r", wobjval(obj));
return;
}
if (obj->item_type == ITEM_CONTAINER) {
send_to_char("You cannot award a container.\n\r", ch);
return;
}
if (trade_good(obj)) {
send_to_char("You cannot award currency.\n\r", ch);
return;
}

obj_from_char(obj);
obj_to_char(obj, victim);
charge_worlds_exp(victim, world, amount);
send_to_char("Done.\n\r", ch);
printf_to_char(victim, "You receive %s!\n\r", obj->description);
return;
}
}
send_to_char("You don't have anything like that.\n\r", ch);
return;
}
*/
    }

    send_to_char("Syntax: World Wilds/Godrealm/Other/Hell) (award/experience/fame) <target> <basicfame/moderatefame/legendaryfame/legendarypower/item> <stat/item keyword>\n\r", ch);
  }

  bool is_invader(CHAR_DATA *ch) {
    if (ch == NULL)
    return FALSE;
    if (!IS_NPC(ch))
    return FALSE;
    if (ch->pIndexData->vnum == 151 && invasion_one == 1)
    return TRUE;
    if (ch->pIndexData->vnum == 152 && invasion_two == 1)
    return TRUE;
    if (ch->pIndexData->vnum == 153 && invasion_three == 1)
    return TRUE;
    return FALSE;
  }

  int find_dream_door(ROOM_INDEX_DATA *room) {
    for (int i = 0; i < 50; i++) {
      int door = number_range(54, 2946329) % 10;
      if (door != DIR_UP && door != DIR_DOWN) {
        if (room->exit[door] == NULL || room->exit[door]->u1.to_room == NULL)
        return door;
        if (room->exit[door] != NULL && room->exit[door]->u1.to_room != NULL && (room->exit[door]->wall > WALL_GLASS && room->exit[door]->wallcondition != WALLCOND_HOLE))
        return door;
        if (room->exit[door] != NULL && room->exit[door]->u1.to_room != NULL && room->exit[door]->u1.to_room->sector_type == SECT_FOREST)
        return door;
      }
    }
    return -1;
  }

  int part_count(FANTASY_TYPE *fant) {
    int count = 0;
    for (int i = 0; i < 200; i++) {
      if (safe_strlen(fant->participant_shorts[i]) > 1)
      count++;
    }
    if (fant->porn == 1)
    count /= 5;
    else if (fant->stupid == 1)
    count /= 2;

    return count;
  }

  int elevator_count(void) {
    int count = 0;

    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->active == FALSE)
      continue;
      if ((*it)->highlight_time == 0)
      continue;

      if (safe_strlen((*it)->elevator) < 3)
      continue;

      count++;
    }
    return count - 1;
  }

  void dream_lobby_update(void) {
    int count = 0;
    char buf[MSL];
    sprintf(buf, "ELEVATOR: %d out of %d", last_dreamworld, elevator_count());
    //   log_string(buf);

    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->active == FALSE)
      continue;
      if ((*it)->highlight_time == 0)
      continue;
      if (safe_strlen((*it)->elevator) < 3)
      continue;

      if (count == last_dreamworld) {
        sprintf(buf, "ELEVATOR: Found %d,  %s", count, (*it)->name);
        //        log_string(buf);
        last_dreamworld++;
        for (DescList::iterator ij = descriptor_list.begin();
        ij != descriptor_list.end(); ++ij) {
          DESCRIPTOR_DATA *d = *ij;
          if (d->connected == CON_PLAYING) {
            CHAR_DATA *ch = d->character;
            if (ch->pcdata->spectre == 0)
            continue;
            if (!IS_FLAG(ch->act, PLR_BOUND))
            continue;
            sprintf(buf, "ELEVATOR: Spectre %s", ch->name);
            //          log_string(buf);
            if (ch->pcdata->dream_exit == -1) {
              //        log_string("ELEVATOR: No Exit");
              int door = find_dream_door(ch->in_room);
              if (door != -1) {
                //             log_string("ELEVATOR: Found Door");
                ch->pcdata->dream_door = door;
                ch->pcdata->dream_exit = count;
                printf_to_char(ch, "A set of brass elevator doors open to the %s, giving a glimpse of %s\n\r", dir_name[door][0], (*it)->elevator);
                for (CharList::iterator ik = ch->in_room->people->begin();
                ik != ch->in_room->people->end(); ++ik) {
                  CHAR_DATA *rch = *ik;
                  if (IS_NPC(rch) || rch->pcdata->spectre == 0)
                  continue;
                  if (ch == rch)
                  continue;
                  rch->pcdata->dream_door = door;
                  rch->pcdata->dream_exit = count;
                  printf_to_char(rch, "A set of brass elevator doors open to the %s, giving a glimpse of %s\n\r", dir_name[door][0], (*it)->elevator);
                }
              }
            }
          }
        }
        return;
      }
      else
      count++;
    }
    if (last_dreamworld >= elevator_count())
    last_dreamworld = 0;
  }

  FANTASY_TYPE *lobby_number(int number) {
    int count = 0;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->active == FALSE)
      continue;
      if ((*it)->highlight_time == 0)
      continue;

      if (count == number) {
        return (*it);
      }
      else
      count++;
    }
    return NULL;
  }

  void dream_lobby_move(CHAR_DATA *move) {
    int count = 0;
    char buf[MSL];
    sprintf(buf, "ELEVATOR: %d out of %d", last_dreamworld, elevator_count());
    //   log_string(buf);

    for (DescList::iterator ij = descriptor_list.begin();
    ij != descriptor_list.end(); ++ij) {
      DESCRIPTOR_DATA *d = *ij;
      if (d->connected == CON_PLAYING) {
        CHAR_DATA *ch = d->character;
        if (!IS_FLAG(ch->act, PLR_SHROUD))
        continue;
        if (ch == move)
        continue;
        if (ch->in_room != move->in_room)
        continue;
        if (ch->pcdata->dream_exit != -1) {
          move->pcdata->dream_exit = ch->pcdata->dream_exit;
          move->pcdata->dream_door = ch->pcdata->dream_door;
          printf_to_char(move, "You see a set of open brass elevators doors to the %s, giving a glimpse of %s\n\r", dir_name[ch->pcdata->dream_door][0], lobby_number(ch->pcdata->dream_exit)->elevator);
          return;
        }
      }
    }

    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->active == FALSE)
      continue;
      if ((*it)->highlight_time == 0)
      continue;

      if (safe_strlen((*it)->elevator) < 3)
      continue;

      if (count == move->pcdata->last_dreamworld) {
        sprintf(buf, "ELEVATOR: Found %d,  %s", count, (*it)->name);
        //                  log_string(buf);
        move->pcdata->last_dreamworld++;
        for (DescList::iterator ij = descriptor_list.begin();
        ij != descriptor_list.end(); ++ij) {
          DESCRIPTOR_DATA *d = *ij;
          if (d->connected == CON_PLAYING) {
            CHAR_DATA *ch = d->character;
            if (!IS_FLAG(ch->act, PLR_SHROUD))
            continue;
            sprintf(buf, "ELEVATOR: Spectre %s", ch->name);
            //                          log_string(buf);
            if (ch->in_room == move->in_room) {
              //                              log_string("ELEVATOR: No Exit");
              int door = find_dream_door(ch->in_room);
              if (door != -1) {
                //                                   log_string("ELEVATOR: Found
                //                                   Door");
                ch->pcdata->dream_door = door;
                ch->pcdata->dream_exit = count;
                printf_to_char(ch, "A set of brass elevator doors open to the %s, giving a glimpse of %s\n\r", dir_name[door][0], (*it)->elevator);
                for (CharList::iterator ik = ch->in_room->people->begin();
                ik != ch->in_room->people->end(); ++ik) {
                  CHAR_DATA *rch = *ik;
                  if (IS_NPC(rch) || !IS_FLAG(rch->act, PLR_SHROUD))
                  continue;
                  if (ch == rch)
                  continue;
                  rch->pcdata->dream_door = door;
                  rch->pcdata->dream_exit = count;
                  printf_to_char(rch, "A set of brass elevator doors open to the %s, giving a glimpse of %s\n\r", dir_name[door][0], (*it)->elevator);
                }
              }
            }
          }
        }
        return;
      }
      else
      count++;
    }
    if (move->pcdata->last_dreamworld >= elevator_count())
    move->pcdata->last_dreamworld = 0;
  }

  bool good_person(CHAR_DATA *ch) {
    int points = 0;
    if (ch->pcdata->habit[HABIT_CONCERN] >= 9)
    points++;
    if (ch->pcdata->habit[HABIT_DEMOCRACY] <= 1)
    points++;
    if (ch->pcdata->habit[HABIT_EQUALITY] <= 2)
    points++;
    if (ch->pcdata->habit[HABIT_CRUELTY] >= 5)
    points++;
    if (points >= 2)
    return TRUE;
    return FALSE;
  }


  void villain_mod(CHAR_DATA *ch, int amount, char *reason) {
    if (amount <= 0)
    return;

    if(IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_OPERATIVE)
    {
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      CHAR_DATA *victim;
      Buffer outbuf;
      char buf[MSL];
      struct stat sb;

      if (safe_strlen(ch->pcdata->guest_of) < 2)
      return;

      d.original = NULL;
      if ((victim = get_char_world_pc(ch->pcdata->guest_of)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: offline flag");

        if (!load_char_obj(&d, ch->pcdata->guest_of)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->guest_of));
        stat(buf, &sb);
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      villain_mod(victim, amount, reason);

      save_char_obj(victim, FALSE, FALSE);

      if (!online)
      free_char(victim);
      return;


    }

    if (!str_cmp(reason, ch->pcdata->last_villain_mod)) {
      if (!str_cmp(reason, "Ritual"))
      amount = amount * -1;
      else
      return;
    }
    free_string(ch->pcdata->last_villain_mod);
    ch->pcdata->last_villain_mod = str_dup(reason);

    give_clan_power(ch, amount);
    ch->pcdata->week_tracker[TRACK_VILLAIN] += amount;
    ch->pcdata->life_tracker[TRACK_VILLAIN] += amount;
    char buf[MSL];
    give_intel(ch, amount * 30);
    if (get_tier(ch) >= 3) {
      ch->pcdata->monster_fed += amount / 2;
      sprintf(buf, "VILLAIN: %s gains %d for %s, to %d(%d)", ch->name, amount, reason, ch->pcdata->week_tracker[TRACK_VILLAIN], ch->pcdata->life_tracker[TRACK_VILLAIN]);
      log_string(buf);
    }
    else {
      if (clan_lookup(ch->fsect) != NULL && (good_person(ch) && clan_lookup(ch->fsect)->axes[AXES_CORRUPT] >= AXES_NEARRIGHT) && (number_percent() % 2 == 0 || ch->skills[SKILL_MENTALDISCIPLINE] == 0))
      ch->pcdata->heroic += amount*3/2;
      else if (clan_lookup(ch->fcore) != NULL && (good_person(ch) && clan_lookup(ch->fcore)->axes[AXES_CORRUPT] >= AXES_NEARRIGHT) && (number_percent() % 2 == 0 || ch->skills[SKILL_MENTALDISCIPLINE] == 0))
      ch->pcdata->heroic += amount;
      else if (clan_lookup(ch->fcult) != NULL && (good_person(ch) && clan_lookup(ch->fcult)->axes[AXES_CORRUPT] >= AXES_NEARRIGHT) && (number_percent() % 2 == 0 || ch->skills[SKILL_MENTALDISCIPLINE] == 0))
      ch->pcdata->heroic += amount/2;

    }

    if (clan_lookup(ch->fcult) != NULL) {
      if (str_cmp(ch->name, clan_lookup(ch->fcult)->leader)) {
        if (get_char_world_pc(clan_lookup(ch->fcult)->leader) != NULL)
        villain_mod(get_char_world_pc(clan_lookup(ch->fcult)->leader), amount / 4, "Leadership");
      }
    }


  }

  void transfer_sr(char *oldname, char *newname) {
    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if (!str_cmp((*it)->author, oldname)) {
        free_string((*it)->author);
        (*it)->author = str_dup(newname);
      }
    }
    for (vector<STORYIDEA_TYPE *>::iterator it = StoryIdeaVect.begin();
    it != StoryIdeaVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if (!str_cmp((*it)->author, oldname)) {
        free_string((*it)->author);
        (*it)->author = str_dup(newname);
      }
    }
    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (!str_cmp((*it)->author, oldname)) {
        free_string((*it)->author);
        (*it)->author = str_dup(newname);
      }
    }
    for (vector<GALLERY_TYPE *>::iterator it = GalleryVect.begin();
    it != GalleryVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it)->owner == NULL || safe_strlen((*it)->owner) < 2)
      continue;

      if (!str_cmp((*it)->owner, oldname)) {
        free_string((*it)->owner);
        (*it)->owner = str_dup(newname);
      }
    }
    for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
    it != StorylineVect.end(); ++it) {
      if ((*it)->valid == FALSE || safe_strlen((*it)->owner) < 2) {
        continue;
      }
      if (!str_cmp((*it)->owner, oldname)) {
        free_string((*it)->owner);
        (*it)->owner = str_dup(newname);
      }
    }
  }

  void transfer_dreams(char *oldname, char *newname) {
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if (daysidle((*it)->author) > 45)
      continue;
      if (!str_cmp((*it)->author, oldname)) {
        free_string((*it)->author);
        (*it)->author = str_dup(newname);
      }
    }
  }

  void sr_reroll(CHAR_DATA *ch, char *newname) {
    if (!str_prefix(ch->name, "SR")) {
      if (str_prefix(newname, "SR")) {
        transfer_sr(ch->name, sr_name(newname));
        transfer_dreams(nosr_name(ch->name), newname);
      }
      else {
        transfer_sr(ch->name, newname);
        transfer_dreams(nosr_name(ch->name), nosr_name(newname));
      }
    }
    else {
      if (str_prefix(newname, "SR")) {
        transfer_sr(sr_name(ch->name), sr_name(newname));
        transfer_dreams(ch->name, newname);
      }
      else {
        transfer_sr(sr_name(ch->name), newname);
        transfer_dreams(ch->name, nosr_name(newname));
      }
    }
  }

  void sr_delete(CHAR_DATA *ch) {
    if (ch->pcdata->account == NULL && ch->desc->account != NULL)
    ch->pcdata->account = ch->desc->account;

    if (ch->pcdata->account == NULL)
    return;

    for (int i = 0; i < 25; i++) {
      if (safe_strlen(ch->pcdata->account->characters[i]) > 2 && str_cmp(ch->name, ch->pcdata->account->characters[i])) {
        sr_reroll(ch, ch->pcdata->account->characters[i]);
        return;
      }
    }
  }

  int worlds_experience(CHAR_DATA *ch, int type) {
    int amount = 0;
    amount = ch->pcdata->aexp + ch->pcdata->wexp + ch->pcdata->hexp +
    ch->pcdata->oexp + ch->pcdata->gexp;
    if (type == WORLD_EARTH)
    amount += ch->pcdata->aexp;
    if (type == WORLD_OTHER)
    amount += ch->pcdata->oexp;
    if (type == WORLD_GODREALM)
    amount += ch->pcdata->gexp;
    if (type == WORLD_HELL)
    amount += ch->pcdata->hexp;
    if (type == WORLD_WILDS)
    amount += ch->pcdata->wexp;

    return amount;
  }
  void charge_worlds_exp(CHAR_DATA *ch, int type, int amount) {
    int charge;
    if (type == WORLD_EARTH) {
      charge = UMIN(amount, ch->pcdata->aexp * 2);
      amount -= charge;
      ch->pcdata->aexp -= charge / 2;
    }
    if (type == WORLD_OTHER) {
      charge = UMIN(amount, ch->pcdata->oexp * 2);
      amount -= charge;
      ch->pcdata->oexp -= charge / 2;
      ch->pcdata->oexp /= 2;
    }
    if (type == WORLD_WILDS) {
      charge = UMIN(amount, ch->pcdata->wexp * 2);
      amount -= charge;
      ch->pcdata->wexp -= charge / 2;
    }
    if (type == WORLD_GODREALM) {
      charge = UMIN(amount, ch->pcdata->gexp * 2);
      amount -= charge;
      ch->pcdata->gexp -= charge / 2;
    }
    if (type == WORLD_HELL) {
      charge = UMIN(amount, ch->pcdata->hexp * 2);
      amount -= charge;
      ch->pcdata->hexp -= charge / 2;
      ch->pcdata->hexp /= 2;
    }
    if (type == WORLD_DREAM) {
      charge = UMIN(amount, ch->pcdata->dexp * 2);
      amount -= charge;
      ch->pcdata->dexp -= charge / 2;
    }
    ch->pcdata->aexp -= amount;
    if (ch->pcdata->aexp < 0) {
      ch->pcdata->oexp += ch->pcdata->aexp;
      ch->pcdata->aexp = 0;
    }
    if (ch->pcdata->oexp < 0) {
      ch->pcdata->wexp += ch->pcdata->oexp;
      ch->pcdata->oexp = 0;
    }
    if (ch->pcdata->wexp < 0) {
      ch->pcdata->hexp += ch->pcdata->wexp;
      ch->pcdata->wexp = 0;
    }
    if (ch->pcdata->hexp < 0) {
      ch->pcdata->gexp += ch->pcdata->hexp;
      ch->pcdata->hexp = 0;
    }
    ch->pcdata->gexp = UMAX(ch->pcdata->gexp, 0);
  }

  void add_mastermind(STORYLINE_TYPE *story, char *name, int amount) {
    if (story == NULL)
    return;
    if (amount <= 0)
    return;

    if (safe_strlen(name) < 1)
    return;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(story->masterminds[i], name)) {
        story->mastermind_power[i] += amount;
        return;
      }
    }
    if (amount > 1) {
      for (int i = 0; i < 10; i++) {
        if (story->mastermind_power[i] == 0) {
          free_string(story->masterminds[i]);
          story->masterminds[i] = str_dup(name);
          story->mastermind_power[i] = amount;
          return;
        }
      }
    }
  }

  void bookadd(CHAR_DATA *ch, int world) {
    if (world == -1) {
      ch->pcdata->account->haven_books++;
      send_to_char("You can now create a new entry in the Haven book, set in Haven Township, somewhere else in earth, or somewhere offworld.\n\r", ch);
    }
    else if (world == WORLD_EARTH) {
      ch->pcdata->account->earth_books++;
      send_to_char("You can now create a new entry in the Haven book, set somewhere else in earth, or somewhere offworld.\n\r", ch);
    }
    else if (world == WORLD_OTHER) {
      ch->pcdata->account->world_books++;
      send_to_char("You can now create a new entry in the Haven book, set somewhere offworld.\n\r", ch);
    }
  }

  _DOFUN(do_book) {
    int page;
    int entry;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg2);
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg3);

    if (safe_strlen(arg1) < 1 || !str_cmp(arg1, "index")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WIndex:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "index") && !str_cmp(arg2, "other")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WOther Index:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_OTHER)
        continue;
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "index") && !str_cmp(arg2, "haven")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WHaven Index:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != -1)
        continue;
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "index") && !str_cmp(arg2, "earth")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WEarth Index:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_EARTH)
        continue;
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "index") && !str_cmp(arg2, "godrealm")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WGodrealm Index:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_GODREALM)
        continue;
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "index") && !str_cmp(arg2, "wilds")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WWilds Index:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_WILDS)
        continue;
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "index") && !str_cmp(arg2, "hell")) {
      page = 1;
      entry = 0;
      printf_to_char(ch, "`WHell Index:\n\r");
      bool unread = FALSE;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_HELL)
        continue;
        entry++;
        if ((*it)->number > ch->pcdata->last_page)
        unread = TRUE;
        if (entry == 3) {
          if (unread == TRUE)
          printf_to_char(ch, "`W%d)\n\r", page);
          else
          printf_to_char(ch, "`c%d)\n\r", page);
          page++;
          entry = 0;
          unread = FALSE;
        }
      }
      if (entry > 0) {
        if (unread == TRUE)
        printf_to_char(ch, "`W%d)\n\r", page);
        else
        printf_to_char(ch, "`c%d)\n\r", page);
      }
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
      }
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && is_number(arg2)) {
      int point = atoi(arg2);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if (page == point) {
          if (IS_IMMORTAL(ch))
          printf_to_char(ch, "(%d)\t%s\n\n\r", (*it)->number, (*it)->text);
          else
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "other") && !str_cmp(arg3, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_OTHER)
        continue;
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_OTHER)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "haven") && !str_cmp(arg3, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != -1)
        continue;
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != -1)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "earth") && !str_cmp(arg3, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_EARTH)
        continue;
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_EARTH)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "wilds") && !str_cmp(arg3, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_WILDS)
        continue;
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_WILDS)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "godrealm") && !str_cmp(arg3, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_GODREALM)
        continue;
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_GODREALM)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "hell") && !str_cmp(arg3, "")) {
      int point = 0;
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_HELL)
        continue;
        if ((*it)->number > ch->pcdata->last_page)
        point = page;
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_HELL)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "other") && is_number(arg3)) {
      int point = atoi(arg3);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_OTHER)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "haven") && is_number(arg3)) {
      int point = atoi(arg3);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != -1)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "earth") && is_number(arg3)) {
      int point = atoi(arg3);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_EARTH)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "godrealm") && is_number(arg3)) {
      int point = atoi(arg3);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_GODREALM)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "wilds") && is_number(arg3)) {
      int point = atoi(arg3);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_WILDS)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "read") && !str_cmp(arg2, "hell") && is_number(arg3)) {
      int point = atoi(arg3);
      page = 1;
      entry = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE) {
          continue;
        }
        if ((*it)->world != WORLD_HELL)
        continue;
        if (page == point) {
          printf_to_char(ch, "\t%s\n\n\r", (*it)->text);
          ch->pcdata->last_page = UMAX(ch->pcdata->last_page, (*it)->number);
        }
        entry++;
        if (entry == 3) {
          page++;
          entry = 0;
        }
      }
      printf_to_char(ch, "                                                                         `wPage %2d\n\r", point);
      return;
    }
    if (!str_cmp(arg1, "add")) {
      PAGE_TYPE *page;
      int maxpage = 0;
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        maxpage = UMAX(maxpage, (*it)->number);
      }
      if (!str_cmp(arg2, "haven")) {
        if (ch->pcdata->account->haven_books < 1 && str_cmp(ch->name, "Tyr")) {
          send_to_char("You don't have any saved entries.\n\r", ch);
          return;
        }

        page = new_page();
        page->world = -1;
        free_string(page->author);
        page->author = str_dup(ch->pcdata->account->name);
        page->createdate = current_time;
        page->number = maxpage + 1;
        PageVect.push_back(page);
        string_append(ch, &page->text);
        ch->pcdata->account->haven_books--;
        return;
      }
      if (!str_cmp(arg2, "earth")) {
        if (ch->pcdata->account->haven_books < 1 && str_cmp(ch->name, "Tyr") && ch->pcdata->account->earth_books < 1) {
          send_to_char("You don't have any saved entries.\n\r", ch);
          return;
        }

        page = new_page();
        page->world = WORLD_EARTH;
        free_string(page->author);
        page->author = str_dup(ch->pcdata->account->name);
        page->createdate = current_time;
        page->number = maxpage + 1;
        PageVect.push_back(page);
        string_append(ch, &page->text);
        if (ch->pcdata->account->earth_books > 0)
        ch->pcdata->account->earth_books--;
        else
        ch->pcdata->account->haven_books--;
        return;
      }
      if (!str_cmp(arg2, "other")) {
        if (ch->pcdata->account->haven_books < 1 && str_cmp(ch->name, "Tyr") && ch->pcdata->account->earth_books < 1 && ch->pcdata->account->world_books < 1) {
          send_to_char("You don't have any saved entries.\n\r", ch);
          return;
        }

        page = new_page();
        page->world = WORLD_OTHER;
        free_string(page->author);
        page->author = str_dup(ch->pcdata->account->name);
        page->createdate = current_time;
        page->number = maxpage + 1;
        PageVect.push_back(page);
        string_append(ch, &page->text);
        if (ch->pcdata->account->world_books > 0)
        ch->pcdata->account->world_books--;
        else if (ch->pcdata->account->earth_books > 0)
        ch->pcdata->account->earth_books--;
        else
        ch->pcdata->account->haven_books--;
        return;
      }
      if (!str_cmp(arg2, "godrealm")) {
        if (ch->pcdata->account->haven_books < 1 && str_cmp(ch->name, "Tyr") && ch->pcdata->account->earth_books < 1 && ch->pcdata->account->world_books < 1) {
          send_to_char("You don't have any saved entries.\n\r", ch);
          return;
        }

        page = new_page();
        page->world = WORLD_GODREALM;
        free_string(page->author);
        page->author = str_dup(ch->pcdata->account->name);
        page->createdate = current_time;
        page->number = maxpage + 1;
        PageVect.push_back(page);
        string_append(ch, &page->text);
        if (ch->pcdata->account->world_books > 0)
        ch->pcdata->account->world_books--;
        else if (ch->pcdata->account->earth_books > 0)
        ch->pcdata->account->earth_books--;
        else
        ch->pcdata->account->haven_books--;
        return;
      }
      if (!str_cmp(arg2, "wilds")) {
        if (ch->pcdata->account->haven_books < 1 && str_cmp(ch->name, "Tyr") && ch->pcdata->account->earth_books < 1 && ch->pcdata->account->world_books < 1) {
          send_to_char("You don't have any saved entries.\n\r", ch);
          return;
        }

        page = new_page();
        page->world = WORLD_WILDS;
        free_string(page->author);
        page->author = str_dup(ch->pcdata->account->name);
        page->createdate = current_time;
        page->number = maxpage + 1;
        PageVect.push_back(page);
        string_append(ch, &page->text);
        if (ch->pcdata->account->world_books > 0)
        ch->pcdata->account->world_books--;
        else if (ch->pcdata->account->earth_books > 0)
        ch->pcdata->account->earth_books--;
        else
        ch->pcdata->account->haven_books--;
        return;
      }
      if (!str_cmp(arg2, "hell")) {
        if (ch->pcdata->account->haven_books < 1 && str_cmp(ch->name, "Tyr") && ch->pcdata->account->earth_books < 1 && ch->pcdata->account->world_books < 1) {
          send_to_char("You don't have any saved entries.\n\r", ch);
          return;
        }

        page = new_page();
        page->world = WORLD_HELL;
        free_string(page->author);
        page->author = str_dup(ch->pcdata->account->name);
        page->createdate = current_time;
        page->number = maxpage + 1;
        PageVect.push_back(page);
        string_append(ch, &page->text);
        if (ch->pcdata->account->world_books > 0)
        ch->pcdata->account->world_books--;
        else if (ch->pcdata->account->earth_books > 0)
        ch->pcdata->account->earth_books--;
        else
        ch->pcdata->account->haven_books--;
        return;
      }
    }
    if (!str_cmp(arg1, "modify") && IS_IMMORTAL(ch)) {
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (atoi(arg2) == (*it)->number)
        string_append(ch, &(*it)->text);
      }
    }
    if (!str_cmp(arg1, "delete") && IS_IMMORTAL(ch)) {
      for (vector<PAGE_TYPE *>::iterator it = PageVect.begin();
      it != PageVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (atoi(arg2) == (*it)->number)
        (*it)->valid = FALSE;
      }
    }
  }

  void dream_move(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    char buf[MSL];
    if (room == NULL)
    return;

    free_string(ch->pcdata->dreamplace);
    ch->pcdata->dreamplace = str_dup("");
    free_string(ch->pcdata->dreamtitle);
    ch->pcdata->dreamtitle = str_dup("");

    CHAR_DATA *dragging = NULL;

    FANTASY_TYPE *fant;
    if ((fant = room_fantasy(room)) != NULL) {
      if (!part_of_fantasy(ch, fant)) {
        send_to_char("You're not a part of that dreamworld.\n\r", ch);
        return;
      }
      if (fant != room_fantasy(get_room_index(ch->pcdata->dream_room)) && get_char_world_pc(fant->author) != NULL)
      printf_to_char(get_char_world_pc(fant->author), "Someone has entered %s.\n\r", fant->name);

      if (ch->dream_dragging != NULL && part_of_fantasy(ch->dream_dragging, fant) && ch->dream_dragging->pcdata->dream_room == ch->pcdata->dream_room) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->dream_dragging->name, fant->participants[i])) {
            if (fant->participant_nomove[i] == TRUE || fant->participant_noaction[i] == TRUE) {
              dragging = ch->dream_dragging;
              fant->participant_inroom[i] = room->vnum;
            }
          }
        }
      }

      for (int i = 0; i < 200; i++) {
        if (!str_cmp(ch->name, fant->participants[i]))
        fant->participant_inroom[i] = room->vnum;
      }
      sprintf(buf, "%s leaves for %s.", dream_name(ch), room->name);
      for (int i = 0; i < 200; i++) {
        if (ch->pcdata->dream_room == fant->exits[i] && fant->entrances[i] == room->vnum)
        sprintf(buf, "%s leaves for %s.", dream_name(ch), fant->exit_name[i]);
      }
    }
    if (!goddreamer(ch))
    dreamscape_message(ch, ch->pcdata->dream_room, buf);
    int oldroom = ch->pcdata->dream_room;
    ch->pcdata->dream_room = room->vnum;
    if (ch->pcdata->dream_room == ch->pcdata->dream_invite)
    ch->pcdata->dream_invite = 0;
    send_to_char("You move through the dream world.\n\r", ch);
    sprintf(buf, "%s arrives from %s.", dream_introduction(ch), get_room_index(oldroom)->name);
    if (!goddreamer(ch))
    dreamscape_message(ch, ch->pcdata->dream_room, buf);
    do_function(ch, &do_look, "");
    if (dragging != NULL) {
      sprintf(buf, "%s is dragged along.", dream_name(dragging));
      dreamscape_message(dragging, dragging->pcdata->dream_room, buf);
      send_to_char("You are dragged through the dream world.\n\r", dragging);
      sprintf(buf, "%s is dragged into the area.", dream_introduction(dragging));
      dragging->pcdata->dream_room = room->vnum;
      dreamscape_message(dragging, dragging->pcdata->dream_room, buf);
      do_function(dragging, &do_look, "");
    }
    return;
  }

  void wake_char(CHAR_DATA *ch) {
    if (ch == NULL || IS_NPC(ch))
    return;
    char_from_room(ch);
    if (ch->pcdata->ghost_room < 200 && ch->pcdata->deploy_from > 200)
    char_to_room(ch, get_room_index(ch->pcdata->deploy_from));
    else
    char_to_room(ch, get_room_index(ch->pcdata->ghost_room));
    if (IS_FLAG(ch->act, PLR_SHROUD))
    REMOVE_FLAG(ch->act, PLR_SHROUD);
    if (IS_FLAG(ch->act, PLR_DEEPSHROUD))
    REMOVE_FLAG(ch->act, PLR_DEEPSHROUD);
    bool weaps = FALSE;
    if (in_world(ch) != WORLD_EARTH || ch->in_room->area->vnum <= 12 || ch->in_room->area->vnum >= 19)
    weaps = TRUE;

    if (ch->pcdata->spectre == 1) {
      if (ch->pcdata->dreamfromoutfit > 0)
      autooutfit(ch, ch->pcdata->dreamfromoutfit);
      ch->pcdata->dreamfromoutfit = 0;
      OBJ_DATA *obj;
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (obj == NULL)
        continue;
        if (weaps == TRUE)
        continue;

        if (obj->item_type == ITEM_WEAPON || IS_SET(obj->extra_flags, ITEM_ARMORED) || obj->item_type == ITEM_RANGED) {
          unequip_char_silent(ch, obj);
          if (obj->size >= 25)
          SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        }
      }
    }
    ch->pcdata->spectre = 0;
  }

  void to_spectre(CHAR_DATA *ch, bool combat) {
    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return;
    if (!IS_FLAG(ch->act, PLR_SHROUD))
    SET_FLAG(ch->act, PLR_SHROUD);
    if (ch->pcdata->spectre == 0)
    ch->pcdata->ghost_room = ch->in_room->vnum;
    if (combat == FALSE) {
      ch->pcdata->spectre = 2;
    }
    else {
      autogear(ch, TRUE);
      ch->pcdata->spectre = 1;
    }
  }

  void enter_dreamworld(CHAR_DATA *ch, FANTASY_TYPE *fant) {
    if (ch == NULL || fant == NULL || IS_NPC(ch) || is_gm(ch))
    return;
    ch->pcdata->dream_door = -1;
    ch->pcdata->dream_exit = -1;
    if (guestmonster(ch))
    return;
    if (in_fantasy(ch) == NULL) {
      if (ch->pcdata->spectre == 0) {
        if (!is_helpless(ch)) {
          to_spectre(ch, FALSE);
          ch->pcdata->ghost_room = fant->entrance;
        }
        else
        to_spectre(ch, FALSE);
        char_from_room(ch);
        char_to_room(ch, get_room_index(fant->entrance));
      }
      else {
        char_from_room(ch);
        char_to_room(ch, get_room_index(ch->pcdata->ghost_room));
      }
      ch->pcdata->sleeping = 10;
      ch->pcdata->dream_timer = 0;
      if (IS_FLAG(ch->act, PLR_SHROUD))
      REMOVE_FLAG(ch->act, PLR_SHROUD);
    }
    for (int i = 0; i < 200; i++) {
      if (!str_cmp(fant->participants[i], ch->name)) {
        if (fant->participant_inroom[i] != 0) {
          ROOM_INDEX_DATA *room = get_room_index(fant->participant_inroom[i]);
          if (room != NULL && room_fantasy(room) == fant) {
            ch->pcdata->dream_room = room->vnum;
          }
          else
          ch->pcdata->dream_room = fant->entrance;
        }
        else
        ch->pcdata->dream_room = fant->entrance;
        return;
      }
    }
    for (int i = 0; i < 200; i++) {
      if (safe_strlen(fant->participants[i]) < 1) {
        free_string(fant->participants[i]);
        fant->participants[i] = str_dup(ch->name);
        free_string(fant->participant_names[i]);
        fant->participant_names[i] = str_dup(ch->name);
        free_string(fant->participant_shorts[i]);
        fant->participant_shorts[i] = str_dup(ch->pcdata->intro_desc);
        fant->participant_inroom[i] = fant->entrance;
        free_string(fant->participant_descs[i]);
        fant->participant_descs[i] = str_dup(get_default_dreamdesc(ch));
        for (int x = 0; x < 20; x++) {
          if (safe_strlen(fant->roles[x]) > 1 && fant->role_cost[x] == 0)
          fant->participant_role[i] = x;
        }
        ch->pcdata->dream_room = fant->entrance;
        return;
      }
    }
  }

  bool dream_slave(CHAR_DATA *ch) {
    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) == NULL)
    return FALSE;
    CHAR_DATA *wch;
    for (int i = 0; i < 200; i++) {
      if (!str_cmp(fant->participants[i], ch->name)) {
        int x = 0;
        for (CharList::iterator it = char_list.begin();
        it != char_list.end() && x < 200; ++it) {
          wch = *it;
          x++;
          if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
          continue;

          if (IS_NPC(wch))
          continue;
          if (!is_dreaming(wch))
          continue;
          if (wch->pcdata->tempdreamgodworld == fantasy_number(fant) && wch->pcdata->tempdreamgodchar == i)
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool fantasy_interp(CHAR_DATA *ch, char *command, char *argument, FANTASY_TYPE *fant) {
    char buf[MSL];
    int num = fantasy_number(fant);
    if (!str_cmp(command, "Rescue")) {
      sprintf(buf, "Rescue %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "Makedoor")) {
      sprintf(buf, "makedoor %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "Attack")) {
      sprintf(buf, "Attack %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "Roll")) {
      sprintf(buf, "Roll %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "Rollnumber")) {
      sprintf(buf, "Rollnumber %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "Rollfail")) {
      sprintf(buf, "Rollfail %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "statinfo")) {
      sprintf(buf, "statinfo %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "petition")) {
      sprintf(buf, "petition %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "scoresheet")) {
      sprintf(buf, "scoresheet %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "buystat")) {
      sprintf(buf, "buystat %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "newdesc")) {
      sprintf(buf, "newdesc %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "Nomove")) {
      sprintf(buf, "nomove %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "noaction")) {
      sprintf(buf, "noaction %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "blind")) {
      sprintf(buf, "blind %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }
    if (!str_cmp(command, "godmode")) {
      sprintf(buf, "godmode %d %s", num, argument);
      do_function(ch, &do_dreamworld, buf);
      return TRUE;
    }

    return FALSE;
  }

  _DOFUN(do_dreamreport) {
    if (dream_slave(ch))
    send_to_char("Dream Slave.\n\r", ch);

    printf_to_char(ch, "GW: %d, DC: %d\n\r", ch->pcdata->tempdreamgodworld, ch->pcdata->tempdreamgodchar);

    if (IS_FLAG(ch->comm, COMM_DREAMSNARED))
    send_to_char("Dream snared.\n\r", ch);

    if (physical_dreamer(ch))
    send_to_char("Physical dreamer\n\r", ch);

    printf_to_char(ch, "Ghost Room: %d\n\r", ch->pcdata->ghost_room);
  }

  bool has_adventure(CHAR_DATA *ch) {
    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!visible_plot(ch, (*it)))
      continue;

      if ((*it)->type != PLOT_ADVENTURE && (*it)->finale == 0)
      continue;

      if (str_cmp(ch->name, (*it)->author))
      continue;

      if (ch->in_room->area->vnum == 12)
      return TRUE;
    }
    return FALSE;
  }

  vector<NEWDESTINY_TYPE *> DestinyVect;
  NEWDESTINY_TYPE *nulldestiny;

  void fread_destiny(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    NEWDESTINY_TYPE *destiny;
    destiny = new_destiny();
    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'A':
        KEY("Author", destiny->author, fread_string(fp));
        KEY("ArchOne", destiny->arch_one, fread_number(fp));
        KEY("ArchTwo", destiny->arch_two, fread_number(fp));
        break;
      case 'C':
        KEY("CreatedAt", destiny->created_at, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          DestinyVect.push_back(destiny);
          return;
        }
        break;
      case 'L':
        KEY("LastUsed", destiny->lastused, fread_number(fp));
      case 'P':
        KEY("Premise", destiny->premise, fread_string(fp));
        KEY("Private", destiny->isprivate, fread_number(fp));

        break;
      case 'R':
        KEY("RoleOneName", destiny->role_one_name, fread_string(fp));
        KEY("RoleTwoName", destiny->role_two_name, fread_string(fp));
        KEY("RestrictOne", destiny->restrict_one, fread_number(fp));
        KEY("RestrictTwo", destiny->restrict_two, fread_number(fp));

        break;
      case 'S':
        if (!str_cmp(word, "Scene")) {
          int point = fread_number(fp);
          destiny->scene_location[point] = fread_number(fp);
          destiny->scene_special_one[point] = fread_number(fp);
          destiny->scene_special_two[point] = fread_number(fp);
          destiny->scene_conclusion[point] = fread_number(fp);
          free_string(destiny->scene_descs[point]);
          destiny->scene_descs[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        break;
      case 'T':
        KEY("Type", destiny->conclude_type, fread_number(fp));
        break;
      case 'V':
        KEY("Vnum", destiny->vnum, fread_number(fp));
        break;
      case 'W':
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_destiny: no match: %s", word);
        bug(buf, 0);
      }
    }
  }
#define DESTINY_FILE "../data/destiny.txt"
  void load_destinies() {
    nulldestiny = new_destiny();
    FILE *fp;

    if ((fp = fopen(DESTINY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_destinies: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "DESTINY")) {
          fread_destiny(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_destinies: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open destiny.txt", 0);
      exit(0);
    }
    for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
    it != DestinyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      bool free = TRUE;
      for (vector<NEWDESTINY_TYPE *>::iterator ik = DestinyVect.begin();
      ik != DestinyVect.end(); ++ik) {
        if ((*ik)->valid == FALSE) {
          continue;
        }
        if ((*ik)->vnum == (*it)->vnum - 1)
        free = FALSE;
      }
      if (free == TRUE && (*it)->vnum > 1)
      (*it)->vnum--;
    }
  }
  void save_destinies(bool backup) {
    FILE *fpout;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/destiny.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/destiny.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/destiny.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/destiny.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/destiny.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/destiny.txt");
      else
      sprintf(buf, "../data/back7/destiny.txt");
      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open destinies.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(DESTINY_FILE, "w")) == NULL) {
        bug("Cannot open destiny.txt for writing", 0);
        return;
      }
    }
    for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
    it != DestinyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it)->lastused > 0 && (*it)->lastused + (3600 * 24 * 180) < current_time)
      continue;

      fprintf(fpout, "#DESTINY\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Vnum %d\n", (*it)->vnum);
      fprintf(fpout, "RoleOneName %s~\n", (*it)->role_one_name);
      fprintf(fpout, "RoleTwoName %s~\n", (*it)->role_two_name);
      fprintf(fpout, "Premise %s~\n", (*it)->premise);
      fprintf(fpout, "Type %d\n", (*it)->conclude_type);
      fprintf(fpout, "LastUsed %d\n", (*it)->lastused);
      fprintf(fpout, "CreatedAt %d\n", (*it)->created_at);
      fprintf(fpout, "Private %d\n", (*it)->isprivate);
      fprintf(fpout, "ArchOne %d\n", (*it)->arch_one);
      fprintf(fpout, "ArchTwo %d\n", (*it)->arch_two);
      fprintf(fpout, "RestrictOne %d\n", (*it)->restrict_one);
      fprintf(fpout, "RestrictTwo %d\n", (*it)->restrict_two);

      for (int i = 0; i < 20; i++) {
        if (safe_strlen((*it)->scene_descs[i]) > 2)
        fprintf(fpout, "Scene %d %d %d %d %d %s~\n", i, (*it)->scene_location[i], (*it)->scene_special_one[i], (*it)->scene_special_two[i], (*it)->scene_conclusion[i], (*it)->scene_descs[i]);
      }
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
    if (backup == FALSE) {
      save_destinies(TRUE);
    }
  }

  _DOFUN(do_teaser) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      string_append(ch, &ch->pcdata->ci_short);

      //      free_string(ch->pcdata->ci_short);
      //      ch->pcdata->ci_short = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_premise) {
    if (ch->pcdata->ci_editing == 16) {
      string_append(ch, &ch->pcdata->ci_short);
      return;
      ch->pcdata->ci_editing = 0;
      free_string(ch->pcdata->ci_short);
      ch->pcdata->ci_short = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_roleonename) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      free_string(ch->pcdata->ci_message);
      ch->pcdata->ci_message = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_roletwoname) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      free_string(ch->pcdata->ci_bystanders);
      ch->pcdata->ci_bystanders = str_dup(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_conclusion) {
    if (ch->pcdata->ci_editing == 19) {
      ch->pcdata->ci_absorb = 1;
      string_append(ch, &ch->pcdata->ci_bystanders);
      send_to_char("Write the message that will display at the successful conclusion of the patrol event.\n\r", ch);
      return;
    }


    if(ch->pcdata->summary_stage == SUM_STAGE_CONCLUSION)
    {
      CHAR_DATA *target = ch->pcdata->summary_target;
      if(target == NULL)
      {
        return;
      }
      if(safe_strlen(argument) < 5 || safe_strlen(argument) > 200)
      {
        send_to_char("Your conclusion must be between 5 and 200 characters.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->summary_conclusion);
      ch->pcdata->summary_conclusion = str_dup(argument);
      if(ch->pcdata->summary_type == SUMMARY_CONFLICT_ATT || ch->pcdata->summary_type == SUMMARY_CONFLICT_DEF || ch->pcdata->summary_type == SUMMARY_DEFEATING_INFIGHT || ch->pcdata->summary_type == SUMMARY_DEFEATING_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATING_DEF)
      {
        process_emote(ch, NULL, EMOTE_EMOTE, argument);
        ch->pcdata->summary_stage = 0;
        target->pcdata->summary_stage = 0;
        ch->pcdata->summary_type = 0;
        target->pcdata->summary_type = 0;
        char_from_room(target);
        char_to_room(target, ch->in_room);
        target->pcdata->sleeping = 200;

        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_OTHER || ch->pcdata->summary_type == SUMMARY_WRAPUP)
      {
        if(safe_strlen(target->pcdata->summary_conclusion) > 3)
        {
          process_emote(target, NULL, EMOTE_EMOTE, target->pcdata->summary_conclusion);
          process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_conclusion);
          ch->pcdata->summary_stage = 0;
          target->pcdata->summary_stage = 0;
          ch->pcdata->summary_type = 0;
          target->pcdata->summary_type = 0;
        }
        else
        send_to_char("Done, waiting on the other person.\n\r", ch);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIMIZER)
      {
        if(ch->pcdata->summary_home <= 0)
        {
          send_to_char("Conclusion set, use home (yes/no) to decide if you want to send the captive home afterwards.\n\r", ch);
          return;
        }
        process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_conclusion);
        ch->pcdata->summary_stage = 0;
        target->pcdata->summary_stage = 0;
        ch->pcdata->summary_type = 0;
        target->pcdata->summary_type = 0;
        char_from_room(target);
        char_to_room(target, ch->in_room);
        if(ch->pcdata->summary_blood == 1)
        {
          bloodbag_transfer(target, ch);
        }
        if(target->pcdata->summary_intel == 1)
        {
          trans_intel(target, ch);
        }
        handout_lifeforce_summary(target, target->pcdata->summary_lifeforce);
        if(ch->pcdata->summary_home == 1)
        {
          ROOM_INDEX_DATA *fleeroom = get_fleeroom(target, NULL);
          char_from_room(target);
          char_to_room(target, fleeroom);
          if (IS_FLAG(ch->act, PLR_BOUND))
          REMOVE_FLAG(ch->act, PLR_BOUND);
          if (IS_FLAG(ch->act, PLR_BOUNDFEET))
          REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
        }
      }

    }

    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "both"))
      ch->pcdata->ci_disclevel = 0;
      else if (!str_cmp(argument, "roleone") || !str_cmp(argument, "role one"))
      ch->pcdata->ci_disclevel = 1;
      else if (!str_cmp(argument, "roletwo") || !str_cmp(argument, "role two"))
      ch->pcdata->ci_disclevel = 2;
      else {
        send_to_char("Syntax: Conclusion both/roleone/roletwo\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  void trans_intel(CHAR_DATA *ch, CHAR_DATA *pers)
  {
    if (pers->faction != 0 && ch->pcdata->intel >= 1000) {
      int iamount = UMIN(ch->pcdata->intel, 10000);
      ch->pcdata->intel -= iamount;
      if (are_allies(ch, pers))
      iamount /= 20;
      FACTION_TYPE *tfac = clan_lookup(pers->faction);

      bool highval = FALSE;
      if (ch->faction != 0 && !are_allies(ch, pers) && iamount >= 1000 && (position_difference(clan_lookup(ch->faction), tfac) >= 25)) {
        highval = TRUE;
        tfac->last_intel = current_time;
        tfac->secret_days -= iamount / 100;
        ch->pcdata->last_intel = current_time;
        ch->pcdata->heroic += iamount / 100;
      }
      if (iamount >= 9000)
      ch->pcdata->pending_resources += 4000;
      else if (iamount >= 3000)
      ch->pcdata->pending_resources += 1000;


      pers->pcdata->secret_days -= iamount / 100;
      char lbuf[MSL];
      if (highval == TRUE)
      sprintf(lbuf, "%s gathering high value intelligence from %s", pers->name, ch->name);
      else
      sprintf(lbuf, "%s gathering intelligence from %s", pers->name, ch->name);
      gain_resources(iamount / 10, tfac->vnum, pers, lbuf);
      give_intel(pers, 500);
      sprintf(lbuf, "INTEL: %s, %s, %d", ch->name, pers->name, iamount);
      log_string(lbuf);
      wiznet(lbuf, NULL, NULL, WIZ_DEATHS, 0, 0);
      guest_match(ch);
      guest_match(pers);

    }

  }
  _DOFUN(do_roleonearchetype) {
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "all"))
      ch->pcdata->ci_zips = 0;
      else if (!str_cmp(argument, "vampires") || !str_cmp(argument, "vampire"))
      ch->pcdata->ci_zips = 1;
      else if (!str_cmp(argument, "werewolf") || !str_cmp(argument, "werewolves"))
      ch->pcdata->ci_zips = 2;
      else if (!str_cmp(argument, "demigod") || !str_cmp(argument, "demigods"))
      ch->pcdata->ci_zips = 3;
      else if (!str_cmp(argument, "demonborn") || !str_cmp(argument, "demonborns"))
      ch->pcdata->ci_zips = 4;
      else if (!str_cmp(argument, "angelborn") || !str_cmp(argument, "angelborns"))
      ch->pcdata->ci_zips = 5;
      else if (!str_cmp(argument, "faeborn") || !str_cmp(argument, "faeborns"))
      ch->pcdata->ci_zips = 6;
      else if (!str_cmp(argument, "human") || !str_cmp(argument, "humans"))
      ch->pcdata->ci_zips = 7;
      else if (!str_cmp(argument, "staff") || !str_cmp(argument, "institute staff"))
      ch->pcdata->ci_zips = 8;
      else if (!str_cmp(argument, "college") || !str_cmp(argument, "college student"))
      ch->pcdata->ci_zips = 10;
      else if (!str_cmp(argument, "patient") || !str_cmp(argument, "institute patient"))
      ch->pcdata->ci_zips = 11;
      else {
        send_to_char("Syntax: Roleonearchetype vampire/werewolf/demigod/demonborn/angelborn/faeborn/human/staff/college/patient\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_roletwoarchetype) {
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "all"))
      ch->pcdata->ci_alcohol = 0;
      else if (!str_cmp(argument, "vampires") || !str_cmp(argument, "vampire"))
      ch->pcdata->ci_alcohol = 1;
      else if (!str_cmp(argument, "werewolf") || !str_cmp(argument, "werewolves"))
      ch->pcdata->ci_alcohol = 2;
      else if (!str_cmp(argument, "demigod") || !str_cmp(argument, "demigods"))
      ch->pcdata->ci_alcohol = 3;
      else if (!str_cmp(argument, "demonborn") || !str_cmp(argument, "demonborns"))
      ch->pcdata->ci_alcohol = 4;
      else if (!str_cmp(argument, "angelborn") || !str_cmp(argument, "angelborns"))
      ch->pcdata->ci_alcohol = 5;
      else if (!str_cmp(argument, "faeborn") || !str_cmp(argument, "faeborns"))
      ch->pcdata->ci_alcohol = 6;
      else if (!str_cmp(argument, "human") || !str_cmp(argument, "humans"))
      ch->pcdata->ci_alcohol = 7;
      else if (!str_cmp(argument, "staff") || !str_cmp(argument, "institute staff"))
      ch->pcdata->ci_alcohol = 8;
      else if (!str_cmp(argument, "college") || !str_cmp(argument, "college student"))
      ch->pcdata->ci_alcohol = 10;
      else if (!str_cmp(argument, "patient") || !str_cmp(argument, "institute patient"))
      ch->pcdata->ci_alcohol = 11;
      else {
        send_to_char("Syntax: Roletwoarchetype vampire/werewolf/demigod/demonborn/angelborn/faeborn/human/staff/college/patient\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_roleonerestrictions) {
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "none"))
      ch->pcdata->ci_discipline = 0;
      else if (!str_cmp(argument, "under T2"))
      ch->pcdata->ci_discipline = DEST_REST_UNDERT2;
      else if (!str_cmp(argument, "under T3"))
      ch->pcdata->ci_discipline = DEST_REST_UNDERT3;
      else if (!str_cmp(argument, "under T4"))
      ch->pcdata->ci_discipline = DEST_REST_UNDERT4;
      else if (!str_cmp(argument, "under T5"))
      ch->pcdata->ci_discipline = DEST_REST_UNDERT5;
      else if (!str_cmp(argument, "above T1"))
      ch->pcdata->ci_discipline = DEST_REST_ABOVET1;
      else if (!str_cmp(argument, "above T2"))
      ch->pcdata->ci_discipline = DEST_REST_ABOVET2;
      else if (!str_cmp(argument, "above T3"))
      ch->pcdata->ci_discipline = DEST_REST_ABOVET3;
      else if (!str_cmp(argument, "above T4"))
      ch->pcdata->ci_discipline = DEST_REST_ABOVET4;
      else if (!str_cmp(argument, "not allies"))
      ch->pcdata->ci_discipline = DEST_REST_NOTALLIES;
      else if (!str_cmp(argument, "allies"))
      ch->pcdata->ci_discipline = DEST_REST_ALLIES;
      else if (!str_cmp(argument, "professional"))
      ch->pcdata->ci_discipline = DEST_REST_PROF;
      else if (!str_cmp(argument, "sorcerer"))
      ch->pcdata->ci_discipline = DEST_REST_SORC;
      else if (!str_cmp(argument, "fighter"))
      ch->pcdata->ci_discipline = DEST_REST_FIGHT;
      else if (!str_cmp(argument, "under 30"))
      ch->pcdata->ci_discipline = DEST_REST_UNDER30;
      else if (!str_cmp(argument, "over 30"))
      ch->pcdata->ci_discipline = DEST_REST_OVER30;
      else {
        send_to_char("Options: None/under T2/Under T3/Under T4/Under T5/Above T1/Above T2/Above T3/Above T4/Not Allies/Allies/Professional/Sorcerer/Fighter/Under 30/Over 30\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_roletworestrictions) {
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "none"))
      ch->pcdata->ci_vnum = 0;
      else if (!str_cmp(argument, "under T2"))
      ch->pcdata->ci_vnum = DEST_REST_UNDERT2;
      else if (!str_cmp(argument, "under T3"))
      ch->pcdata->ci_vnum = DEST_REST_UNDERT3;
      else if (!str_cmp(argument, "under T4"))
      ch->pcdata->ci_vnum = DEST_REST_UNDERT4;
      else if (!str_cmp(argument, "under T5"))
      ch->pcdata->ci_vnum = DEST_REST_UNDERT5;
      else if (!str_cmp(argument, "above T1"))
      ch->pcdata->ci_vnum = DEST_REST_ABOVET1;
      else if (!str_cmp(argument, "above T2"))
      ch->pcdata->ci_vnum = DEST_REST_ABOVET2;
      else if (!str_cmp(argument, "above T3"))
      ch->pcdata->ci_vnum = DEST_REST_ABOVET3;
      else if (!str_cmp(argument, "above T4"))
      ch->pcdata->ci_vnum = DEST_REST_ABOVET4;
      else if (!str_cmp(argument, "not allies"))
      ch->pcdata->ci_vnum = DEST_REST_NOTALLIES;
      else if (!str_cmp(argument, "allies"))
      ch->pcdata->ci_vnum = DEST_REST_ALLIES;
      else if (!str_cmp(argument, "professional"))
      ch->pcdata->ci_vnum = DEST_REST_PROF;
      else if (!str_cmp(argument, "sorcerer"))
      ch->pcdata->ci_vnum = DEST_REST_SORC;
      else if (!str_cmp(argument, "fighter"))
      ch->pcdata->ci_vnum = DEST_REST_FIGHT;
      else if (!str_cmp(argument, "under 30"))
      ch->pcdata->ci_vnum = DEST_REST_UNDER30;
      else if (!str_cmp(argument, "over 30"))
      ch->pcdata->ci_vnum = DEST_REST_OVER30;
      else {
        send_to_char("Options: None/under T2/Under T3/Under T4/Under T5/Above T1/Above T2/Above T3/Above T4/Not Allies/Allies/Professional/Sorcerer/Fighter/Under 30/Over 30\n\r", ch);
        return;
      }

      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene01desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[0]);
      ch->pcdata->ci_includes[0] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene01location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Syntax: scene01location Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[0]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[0]);
      ch->pcdata->ci_excludes[0] = str_dup(outbuf);
      log_string(ch->pcdata->ci_excludes[0]);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene01specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;

      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable/Limited Sanctuary.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[0]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[0]);
      ch->pcdata->ci_excludes[0] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene01specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;

      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable/Limited Sanctuary.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[0]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[0]);
      ch->pcdata->ci_excludes[0] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene01conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene01conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[0]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[0]);
      ch->pcdata->ci_excludes[0] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene02desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[1]);
      ch->pcdata->ci_includes[1] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene02location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[1]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[1]);
      ch->pcdata->ci_excludes[1] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene02specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable/Limited Sanctuary.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[1]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[1]);
      ch->pcdata->ci_excludes[1] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene02specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable/Limited Sanctuary.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[1]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[1]);
      ch->pcdata->ci_excludes[1] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene02conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene01conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[1]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[1]);
      ch->pcdata->ci_excludes[1] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene03desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[2]);
      ch->pcdata->ci_includes[2] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene03location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[2]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[2]);
      ch->pcdata->ci_excludes[2] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene03specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable/Limited Sanctuary.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[2]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[2]);
      ch->pcdata->ci_excludes[2] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene03specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[2]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[2]);
      ch->pcdata->ci_excludes[2] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene03conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[2]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[2]);
      ch->pcdata->ci_excludes[2] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene04desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[3]);
      ch->pcdata->ci_includes[3] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene04location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[3]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[3]);
      ch->pcdata->ci_excludes[3] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene04specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[3]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[3]);
      ch->pcdata->ci_excludes[3] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene04specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[3]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[3]);
      ch->pcdata->ci_excludes[3] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene04conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[3]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[3]);
      ch->pcdata->ci_excludes[3] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene05desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[4]);
      ch->pcdata->ci_includes[4] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene05location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[4]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[4]);
      ch->pcdata->ci_excludes[4] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene05specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[4]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[4]);
      ch->pcdata->ci_excludes[4] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene05specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[4]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[4]);
      ch->pcdata->ci_excludes[4] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene05conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[4]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[4]);
      ch->pcdata->ci_excludes[4] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene06desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[5]);
      ch->pcdata->ci_includes[5] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene06location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[5]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[5]);
      ch->pcdata->ci_excludes[5] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene06specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[5]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[5]);
      ch->pcdata->ci_excludes[5] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene06specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[5]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[5]);
      ch->pcdata->ci_excludes[5] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene06conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[5]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[5]);
      ch->pcdata->ci_excludes[5] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene07desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[6]);
      ch->pcdata->ci_includes[6] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene07location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[6]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[6]);
      ch->pcdata->ci_excludes[6] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene07specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[6]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[6]);
      ch->pcdata->ci_excludes[6] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene07specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[6]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[6]);
      ch->pcdata->ci_excludes[6] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene07conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[6]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[6]);
      ch->pcdata->ci_excludes[6] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene08desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[7]);
      ch->pcdata->ci_includes[7] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene08location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[7]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[7]);
      ch->pcdata->ci_excludes[7] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene08specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[7]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[7]);
      ch->pcdata->ci_excludes[7] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene08specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[7]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[7]);
      ch->pcdata->ci_excludes[7] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene08conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[7]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[7]);
      ch->pcdata->ci_excludes[7] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene09desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[8]);
      ch->pcdata->ci_includes[8] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene09location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[8]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[8]);
      ch->pcdata->ci_excludes[8] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene09specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[8]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[8]);
      ch->pcdata->ci_excludes[8] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene09specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[8]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[8]);
      ch->pcdata->ci_excludes[8] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene09conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[8]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[8]);
      ch->pcdata->ci_excludes[8] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene10desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[9]);
      ch->pcdata->ci_includes[9] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene10location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[9]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[9]);
      ch->pcdata->ci_excludes[9] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene10specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[9]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[9]);
      ch->pcdata->ci_excludes[9] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene10specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[9]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[9]);
      ch->pcdata->ci_excludes[9] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene10conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[9]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[9]);
      ch->pcdata->ci_excludes[9] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene11desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[10]);
      ch->pcdata->ci_includes[10] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene11location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[10]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[10]);
      ch->pcdata->ci_excludes[10] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene11specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[10]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[10]);
      ch->pcdata->ci_excludes[10] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene11specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[10]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[10]);
      ch->pcdata->ci_excludes[10] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene11conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[10]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[10]);
      ch->pcdata->ci_excludes[10] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene12desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[11]);
      ch->pcdata->ci_includes[11] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene12location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[11]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[11]);
      ch->pcdata->ci_excludes[11] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene12specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[11]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[11]);
      ch->pcdata->ci_excludes[11] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene12specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[11]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[11]);
      ch->pcdata->ci_excludes[11] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene12conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[11]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[11]);
      ch->pcdata->ci_excludes[11] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene13desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[12]);
      ch->pcdata->ci_includes[12] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene13location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[12]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[12]);
      ch->pcdata->ci_excludes[12] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene13specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[12]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[12]);
      ch->pcdata->ci_excludes[12] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene13specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[12]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[12]);
      ch->pcdata->ci_excludes[12] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene13conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[12]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[12]);
      ch->pcdata->ci_excludes[12] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene14desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[13]);
      ch->pcdata->ci_includes[13] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene14location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[13]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[13]);
      ch->pcdata->ci_excludes[13] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene14specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[13]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[13]);
      ch->pcdata->ci_excludes[13] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene14specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[13]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[13]);
      ch->pcdata->ci_excludes[13] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene14conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[13]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[13]);
      ch->pcdata->ci_excludes[13] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene15desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[14]);
      ch->pcdata->ci_includes[14] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene15location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[14]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[14]);
      ch->pcdata->ci_excludes[14] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene15specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[14]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[14]);
      ch->pcdata->ci_excludes[14] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene15specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[14]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[14]);
      ch->pcdata->ci_excludes[14] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene15conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[14]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[14]);
      ch->pcdata->ci_excludes[14] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene16desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[15]);
      ch->pcdata->ci_includes[15] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene16location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[15]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[15]);
      ch->pcdata->ci_excludes[15] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene16specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[15]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[15]);
      ch->pcdata->ci_excludes[15] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene16specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[15]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[15]);
      ch->pcdata->ci_excludes[15] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene16conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[15]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[15]);
      ch->pcdata->ci_excludes[15] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene17desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[16]);
      ch->pcdata->ci_includes[16] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene17location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[16]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[16]);
      ch->pcdata->ci_excludes[16] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene17specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[16]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[16]);
      ch->pcdata->ci_excludes[16] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene17specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[16]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[16]);
      ch->pcdata->ci_excludes[16] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene17conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[16]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[16]);
      ch->pcdata->ci_excludes[16] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene18desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[17]);
      ch->pcdata->ci_includes[17] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene18location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[17]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[17]);
      ch->pcdata->ci_excludes[17] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene18specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[17]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[17]);
      ch->pcdata->ci_excludes[17] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene18specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[17]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[17]);
      ch->pcdata->ci_excludes[17] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene18conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[17]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[17]);
      ch->pcdata->ci_excludes[17] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene19desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[18]);
      ch->pcdata->ci_includes[18] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene19location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[18]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[18]);
      ch->pcdata->ci_excludes[18] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene19specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[18]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[18]);
      ch->pcdata->ci_excludes[18] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene19specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[18]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[18]);
      ch->pcdata->ci_excludes[18] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene19conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[18]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[18]);
      ch->pcdata->ci_excludes[18] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene20desc) {
    if (ch->pcdata->ci_editing == 16) {
      free_string(ch->pcdata->ci_includes[19]);
      ch->pcdata->ci_includes[19] = str_dup(argument);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene20location) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "Role One Location"))
      point = -1;
      else if (!str_cmp(argument, "Role Two Location"))
      point = -2;
      else if (!str_cmp(argument, "Role One Home"))
      point = -3;
      else if (!str_cmp(argument, "Role Two Home"))
      point = -4;
      else if (!str_cmp(argument, "Patrol Room"))
      point = -5;
      else if (!str_cmp(argument, "Here"))
      point = ch->in_room->vnum;
      else {
        send_to_char("Options: Role One Location/Role Two Location/Role One Home/Role Two Home/Patrol Room/Here\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[19]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%d %s %s %s", point, argtwo, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[19]);
      ch->pcdata->ci_excludes[19] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_scene20specialroleone) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[19]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %d %s %s", arg, point, argthree, argfour);
      free_string(ch->pcdata->ci_excludes[19]);
      ch->pcdata->ci_excludes[19] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene20specialroletwo) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "None")) {
        point = 0;
      }
      else if (!str_cmp(argument, "Branding"))
      point = DEST_FEAT_BRAND;
      else if (!str_cmp(argument, "Helpless"))
      point = DEST_FEAT_HELPLESS;
      else if (!str_cmp(argument, "Turning"))
      point = DEST_FEAT_TURN;
      else if (!str_cmp(argument, "NoWin"))
      point = DEST_FEAT_NOWIN;
      else if (!str_cmp(argument, "No Sanctuary"))
      point = DEST_FEAT_NOSANC;
      else if (!str_cmp(argument, "Stuck"))
      point = DEST_FEAT_STUCK;
      else if (!str_cmp(argument, "Prey"))
      point = DEST_FEAT_PREY;
      else if (!str_cmp(argument, "Armed"))
      point = DEST_FEAT_ARMED;
      else if (!str_cmp(argument, "Imprintable"))
      point = DEST_FEAT_IMPRINTABLE;
      else if (!str_cmp(argument, "Limited Sanctuary"))
      point = DEST_FEAT_LIMITED;
      else {
        send_to_char("Options: None/Branding/Helpless/Turning/NoWin/No Sanctuary/Stuck/Prey/Armed/Imprintable.\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[19]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %d %s", arg, argtwo, point, argfour);
      free_string(ch->pcdata->ci_excludes[19]);
      ch->pcdata->ci_excludes[19] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_scene20conclusion) {
    int point;
    if (ch->pcdata->ci_editing == 16) {
      if (!str_cmp(argument, "yes")) {
        point = 1;
      }
      else if (!str_cmp(argument, "no"))
      point = 0;
      else {
        send_to_char("Syntax: scene03conclusion yes/no\n\r", ch);
        return;
      }
      char *argument = str_dup(ch->pcdata->ci_excludes[19]);
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      char argtwo[MSL];
      argument = one_argument_nouncap(argument, argtwo);
      char argthree[MSL];
      argument = one_argument_nouncap(argument, argthree);
      char argfour[MSL];
      argument = one_argument_nouncap(argument, argfour);
      char outbuf[MSL];
      sprintf(outbuf, "%s %s %s %d", arg, argtwo, argthree, point);
      free_string(ch->pcdata->ci_excludes[19]);
      ch->pcdata->ci_excludes[19] = str_dup(outbuf);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_knowledge) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      if (ch->pcdata->ci_zips != 0) {
        ch->pcdata->ci_zips = 0;
      }
      else
      ch->pcdata->ci_zips = 1;
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_sexual) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Maybe")) {
        ch->pcdata->ci_discipline3 = DESTINY_MAYBE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Yes")) {
        ch->pcdata->ci_discipline3 = DESTINY_YES;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "No")) {
        ch->pcdata->ci_discipline3 = DESTINY_NO;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Syntax: Sexual Yes/No/Maybe.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }
  _DOFUN(do_disturbing) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Maybe")) {
        ch->pcdata->ci_disclevel = DESTINY_MAYBE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Yes")) {
        ch->pcdata->ci_disclevel = DESTINY_YES;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "No")) {
        ch->pcdata->ci_disclevel = DESTINY_NO;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Syntax: Disturbing Yes/No/Maybe.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_violent) {
    if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Maybe")) {
        ch->pcdata->ci_discipline2 = DESTINY_MAYBE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Yes")) {
        ch->pcdata->ci_discipline2 = DESTINY_YES;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "No")) {
        ch->pcdata->ci_discipline2 = DESTINY_NO;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Syntax: Violent Yes/No/Maybe.\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
  }

  _DOFUN(do_include) {
    if (ch->pcdata->ci_editing == 16) {
      char arg1[MSL];
      argument = one_argument_nouncap(argument, arg1);
      if (!str_cmp(arg1, "Character")) {
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_CHAR;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "Gender")) {
        if (str_cmp(argument, "male") && str_cmp(argument, "female")) {
          send_to_char("Options are: Male, Female.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_GENDER;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "Society")) {
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_SOCIETY;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "morality")) {
        if (str_cmp(argument, "corrupt") && str_cmp(argument, "virtuous")) {
          send_to_char("Options are: corrupt, virtuous.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_MORALITY;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "archetype")) {
        if (str_cmp(argument, "faeborn") && str_cmp(argument, "demigod") && str_cmp(argument, "demonborn") && str_cmp(argument, "angelborn") && str_cmp(argument, "werewolves") && str_cmp(argument, "vampires") && str_cmp(argument, "supernaturals")) {
          send_to_char("Options are: faeborn, demigod, demonborn, angelborn, werewolves, vampires, supernaturals.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_ARCHETYPE;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "institute")) {
        if(str_cmp(argument, "college students") && str_cmp(argument, "college staff") && str_cmp(argument, "clinic patients") && str_cmp(argument, "clinic staff")) {
          send_to_char("Options are: college students, college staff, clinic patients, clinic staff.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_INSTITUTE;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }

      if (!str_cmp(arg1, "focus")) {
        if (str_cmp(argument, "arcanists") && str_cmp(argument, "combatants") && str_cmp(argument, "professionals")) {
          send_to_char("Options are: arcanists, combatants, professionals.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_include_type[i] == 0) {
            ch->pcdata->ci_include_type[i] = CRITERIA_FOCUS;
            free_string(ch->pcdata->ci_includes[i]);
            ch->pcdata->ci_includes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
    }
  }

  char *destiny_features(NEWDESTINY_TYPE *destiny) {
    char onerbuf[MSL];
    char onecbuf[MSL];
    onerbuf[0] = '\0';
    onecbuf[0] = '\0';

    char tworbuf[MSL];
    char twocbuf[MSL];
    tworbuf[0] = '\0';
    twocbuf[0] = '\0';

    for (int i = 0; i < 20; i++) {
      if (destiny->scene_special_one[i] == DEST_FEAT_BRAND) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Branding ");
        else
        strcat(onerbuf, "Branding ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_BRAND) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Branding ");
        else
        strcat(tworbuf, "Branding ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_HELPLESS) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Helpless ");
        else
        strcat(onerbuf, "Helpless ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_HELPLESS) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Helpless ");
        else
        strcat(tworbuf, "Helpless ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_TURN) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Turning ");
        else
        strcat(onerbuf, "Turning ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_TURN) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Turning ");
        else
        strcat(tworbuf, "Turning ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_NOWIN) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Nowin ");
        else
        strcat(onerbuf, "Nowin ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_NOWIN) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Nowin ");
        else
        strcat(tworbuf, "Nowin ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_NOSANC) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "No Sanctuary ");
        else
        strcat(onerbuf, "No Sanctuary ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_NOSANC) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "No Sanctuary ");
        else
        strcat(tworbuf, "No Sanctuary ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_STUCK) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Stuck ");
        else
        strcat(onerbuf, "Stuck ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_STUCK) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Stuck ");
        else
        strcat(tworbuf, "Stuck ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_PREY) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Prey ");
        else
        strcat(onerbuf, "Prey ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_PREY) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Prey ");
        else
        strcat(tworbuf, "Prey ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_ARMED) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Armed ");
        else
        strcat(onerbuf, "Armed ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_ARMED) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Armed ");
        else
        strcat(tworbuf, "Armed ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_IMPRINTABLE) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Imprintable ");
        else
        strcat(onerbuf, "Imprintable ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_IMPRINTABLE) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Imprintable ");
        else
        strcat(tworbuf, "Imprintable ");
      }
      if (destiny->scene_special_one[i] == DEST_FEAT_LIMITED) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(onecbuf, "Limited Sanctuary ");
        else
        strcat(onerbuf, "Limited Sanctuary ");
      }
      if (destiny->scene_special_two[i] == DEST_FEAT_LIMITED) {
        if (destiny->scene_conclusion[i] == 1)
        strcat(twocbuf, "Limited Sanctuary ");
        else
        strcat(tworbuf, "Limited Sanctuary ");
      }
    }
    char buf[MSL];
    buf[0] = '\0';
    if (safe_strlen(onecbuf) > 2 || safe_strlen(onerbuf) > 2) {
      strcat(buf, destiny->role_one_name);
      strcat(buf, ": ");
      strcat(buf, onerbuf);
      strcat(buf, " In conclusions, ");
      strcat(buf, onecbuf);
      strcat(buf, "\n\r");
    }
    if (safe_strlen(twocbuf) > 2 || safe_strlen(tworbuf) > 2) {
      strcat(buf, destiny->role_two_name);
      strcat(buf, ": ");
      strcat(buf, tworbuf);
      strcat(buf, " In conclusions, ");
      strcat(buf, twocbuf);
    }
    if (safe_strlen(buf) > 2)
    return str_dup(buf);
    return "None";
  }

  char *destiny_info(CHAR_DATA *ch, NEWDESTINY_TYPE *destiny) {
    int ccount = 0;
    int scount = 0;
    for (int i = 0; i < 20; i++) {
      if (destiny->scene_conclusion[i] == 1)
      ccount++;
      if (safe_strlen(destiny->scene_descs[i]) > 2)
      scount++;
    }
    char colorcode = 'c';
    if (ch != NULL) {
      for (int i = 0; i < 50; i++) {
        if (ch->pcdata->destiny_in_one[i] == destiny->vnum) {
          colorcode = 'G';
          i = 50;
        }
        else if (ch->pcdata->destiny_in_two[i] == destiny->vnum) {
          colorcode = 'G';
          i = 50;
        }
        else if (ch->pcdata->destiny_interest_one[i] == destiny->vnum) {
          colorcode = 'W';
          i = 50;
        }
        else if (ch->pcdata->destiny_interest_two[i] == destiny->vnum) {
          colorcode = 'W';
          i = 50;
        }
      }
    }
    else
    colorcode = 'c';

    char buf[MSL];
    if (destiny->conclude_type == 0)
    sprintf(buf, "`%c[`W%03d`%c]`x %s and %s(%d scenes).\nPremise: %s\n%d Conclusions: Both decide\n:Features:\n%s\n\r", colorcode, destiny->vnum, colorcode, destiny->role_one_name, destiny->role_two_name, scount, destiny->premise, ccount, destiny_features(destiny));
    else if (destiny->conclude_type == 1)
    sprintf(buf, "`%c[`W%03d`%c]`x %s and %s(%d scenes).\nPremise: %s\n%d Conclusions: %s decides\n:Features:\n%s\n\r", colorcode, destiny->vnum, colorcode, destiny->role_one_name, destiny->role_two_name, scount, destiny->premise, ccount, destiny->role_one_name, destiny_features(destiny));
    else
    sprintf(buf, "`%c[`W%03d`%c]`x %s and %s(%d scenes).\nPremise:%s\n%d Conclusions: %s decides\n:Features:\n%s\n\r", colorcode, destiny->vnum, colorcode, destiny->role_one_name, destiny->role_two_name, scount, destiny->premise, ccount, destiny->role_two_name, destiny_features(destiny));
    return str_dup(buf);
  }

  char *destiny_title(CHAR_DATA *ch, NEWDESTINY_TYPE *destiny) {
    int scount = 0;
    for (int i = 0; i < 20; i++) {
      if (safe_strlen(destiny->scene_descs[i]) > 2)
      scount++;
    }

    char colorcode = 'c';
    if (ch != NULL) {
      for (int i = 0; i < 50; i++) {
        if (ch->pcdata->destiny_in_one[i] == destiny->vnum) {
          colorcode = 'G';
          i = 50;
        }
        else if (ch->pcdata->destiny_in_two[i] == destiny->vnum) {
          colorcode = 'G';
          i = 50;
        }
        else if (ch->pcdata->destiny_interest_one[i] == destiny->vnum) {
          colorcode = 'W';
          i = 50;
        }
        else if (ch->pcdata->destiny_interest_two[i] == destiny->vnum) {
          colorcode = 'W';
          i = 50;
        }
      }
    }
    else
    colorcode = 'c';

    char buf[MSL];
    sprintf(buf, "`%c[`W%03d`%c]`x %s and %s(%d scenes).\n\r", colorcode, destiny->vnum, colorcode, destiny->role_one_name, destiny->role_two_name, scount);
    return str_dup(buf);
  }

  bool can_see_destiny(CHAR_DATA *ch, NEWDESTINY_TYPE *destiny) {
    if (can_destiny_one(ch, destiny) || can_destiny_two(ch, destiny))
    return TRUE;
    return FALSE;
  }

  bool are_allies(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch->fcore > 0) {
      if (ch->fcore == victim->fcore)
      return TRUE;
    }
    if (ch->fcult > 0) {
      if (ch->fcult == victim->fcult)
      return TRUE;
      FACTION_TYPE *cclan = clan_lookup(ch->fcult);
      FACTION_TYPE *vclan = clan_lookup(victim->fcult);
      if (cclan != NULL && vclan != NULL && cclan->alliance == vclan->alliance)
      return TRUE;
    }
    if (ch->fsect > 0) {
      if (ch->fsect == victim->fsect)
      return TRUE;
      FACTION_TYPE *cclan = clan_lookup(ch->fsect);
      FACTION_TYPE *vclan = clan_lookup(victim->fsect);
      if (cclan != NULL && vclan != NULL && cclan->alliance == vclan->alliance)
      return TRUE;
    }

    return FALSE;
  }


  bool are_noncult_allies(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch->fcore > 0) {
      if (ch->fcore == victim->fcore)
      return TRUE;
    }
    if (ch->fsect > 0) {
      if (ch->fsect == victim->fsect)
      return TRUE;
      FACTION_TYPE *cclan = clan_lookup(ch->fsect);
      FACTION_TYPE *vclan = clan_lookup(victim->fsect);
      if (cclan != NULL && vclan != NULL && cclan->alliance == vclan->alliance)
      return TRUE;
    }

    return FALSE;
  }


  bool know_eachother(CHAR_DATA *ch, CHAR_DATA *victim) {
    bool charknow = FALSE;
    bool vicknow = FALSE;
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;

    for (NameMap::iterator it = ch->pcdata->male_names->begin();
    it != ch->pcdata->male_names->end(); ++it) {
      if (it->first == victim->id) {
        charknow = TRUE;
      }
    }
    for (NameMap::iterator it = ch->pcdata->female_names->begin();
    it != ch->pcdata->female_names->end(); ++it) {
      if (it->first == victim->id) {
        charknow = TRUE;
      }
    }

    for (NameMap::iterator it = victim->pcdata->male_names->begin();
    it != victim->pcdata->male_names->end(); ++it) {
      if (it->first == ch->id) {
        vicknow = TRUE;
      }
    }
    for (NameMap::iterator it = victim->pcdata->female_names->begin();
    it != victim->pcdata->female_names->end(); ++it) {
      if (it->first == ch->id) {
        vicknow = TRUE;
      }
    }

    if (charknow == TRUE && vicknow == TRUE)
    return TRUE;
    return FALSE;
  }

  int destiny_match_score(CHAR_DATA *ch, CHAR_DATA *victim, NEWDESTINY_TYPE *dest) {
    if (!can_destiny_one(ch, dest))
    return -1;
    if (!can_destiny_two(victim, dest))
    return -1;
    if (same_player(ch, victim))
    return -1;
    if (know_eachother(ch, victim) == FALSE && (get_tier(ch) < 2 || get_tier(victim)))
    return -1;
    bool onespace = FALSE, twospace = FALSE;
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->destiny_in_one[i] == 0)
      onespace = TRUE;
      if (victim->pcdata->destiny_in_two[i] == 0)
      twospace = TRUE;
    }
    if (onespace == FALSE || twospace == FALSE)
    return -1;

    if (dest->restrict_one == DEST_REST_NOTALLIES || dest->restrict_two == DEST_REST_NOTALLIES) {
      if (are_allies(ch, victim) == FALSE)
      return -1;
      if (ch->faction == 0 || victim->faction == 0)
      return -1;
    }
    if (dest->restrict_one == DEST_REST_ALLIES || dest->restrict_two == DEST_REST_ALLIES) {
      if (are_allies(ch, victim) == TRUE)
      return -1;
      if (ch->faction == 0 || victim->faction == 0)
      return -1;
    }
    int points = 30;
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->destiny_interest_one[i] == dest->vnum)
      points += 50;
      if (victim->pcdata->destiny_interest_two[i] == dest->vnum)
      points += 50;
    }
    if (!str_cmp(ch->pcdata->last_praised[0], victim->name))
    points += 25;
    if (!str_cmp(ch->pcdata->last_praised[1], victim->name))
    points += 25;
    if (!str_cmp(victim->pcdata->last_praised[0], ch->name))
    points += 25;
    if (!str_cmp(victim->pcdata->last_praised[1], ch->name))
    points += 25;

    if (!know_eachother(ch, victim))
    points -= 20;

    if (ch->pcdata->habit[HABIT_ORIENTATION] == 0 && ch->sex == victim->sex)
    points -= 15;
    if (ch->pcdata->habit[HABIT_ORIENTATION] == 1 && ch->sex != victim->sex)
    points -= 15;

    if (narrative_pair(ch, victim)) {
      points += narrative_score(ch, victim) * 25;
    }
    return points;
  }

  void offer_destiny(CHAR_DATA *ch, CHAR_DATA *victim, NEWDESTINY_TYPE *dest) {
    ch->pcdata->destiny_offer = dest->vnum;
    ch->pcdata->destiny_offer_role = 1;
    ch->pcdata->destiny_offer_char = victim;
    victim->pcdata->destiny_offer = dest->vnum;
    victim->pcdata->destiny_offer_role = 2;
    victim->pcdata->destiny_offer_char = ch;

    printf_to_char(
    ch, "Fate offers you a destiny as <b>%s</b> with <b>%s</b>.\n%s\n(Use destiny accept/reject)`x`x`x`x`x`x`x`x`x      \n\r", dest->role_one_name, victim->name, destiny_info(ch, dest));
    printf_to_char(
    victim, "Fate offers you a destiny as <b>%s</b> with <b>%s</b>.\n%s\n(Use destiny accept/reject)`x`x`x`x`x`x`x`x`x      \n\r", dest->role_two_name, ch->name, destiny_info(victim, dest));
  }

  bool can_see_destiny_char(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (victim->in_room->vnum < 300)
    return FALSE;
    if (IS_FLAG(victim->comm, COMM_AFK))
    return FALSE;
    if (victim->pcdata->availability == AVAIL_LOW)
    return FALSE;
    if (can_see_char_distance(ch, victim, DISTANCE_MEDIUM))
    return TRUE;
    if (ch->in_room == victim->in_room)
    return TRUE;
    if (!IS_FLAG(victim->comm, COMM_WHOINVIS))
    return TRUE;
    if (victim->pcdata->availability == AVAIL_HIGH)
    return TRUE;
    return FALSE;
  }

  bool can_destiny_scene(CHAR_DATA *ch, ROOM_INDEX_DATA *troom) {
    if (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || is_ghost(ch) || locked_room(ch->in_room, ch) || room_hostile(ch->in_room) || ch->in_room == NULL || ch->in_room->vnum < 300)
    return FALSE;
    if (troom != NULL) {
      if (clinic_patient(ch) && !institute_room(troom))
      return FALSE;
    }
    if (is_prisoner(ch))
    return FALSE;

    return TRUE;
  }
  NEWDESTINY_TYPE *get_destiny(int ivnum) {
    for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
    it != DestinyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->vnum == ivnum)
      return (*it);
    }
    return NULL;
  }

  char *indest_scene_location(NEWDESTINY_TYPE *dest, int number) {
    char buf[MSL];

    if (dest->scene_location[number] == -1) {
      sprintf(buf, "%s's current location.\n\r", dest->role_one_name);
      return str_dup(buf);
    }
    if (dest->scene_location[number] == -2) {
      sprintf(buf, "%s's current location.\n\r", dest->role_two_name);
      return str_dup(buf);
    }
    if (dest->scene_location[number] == -3) {
      sprintf(buf, "%s's home.\n\r", dest->role_one_name);
      return str_dup(buf);
    }
    if (dest->scene_location[number] == -4) {
      sprintf(buf, "%s's home.\n\r", dest->role_two_name);
      return str_dup(buf);
    }
    if (dest->scene_location[number] == -5) {
      return "A patrol room.";
    }
    ROOM_INDEX_DATA *room = get_room_index(dest->scene_location[number]);
    if (room == NULL)
    return "";
    return room->name;
  }

  char *destiny_scene_desc(NEWDESTINY_TYPE *dest, int number) {
    char buf[MSL];
    if (dest->scene_conclusion[number] == TRUE) {
      int point = 1;
      char tmp[MSL];
      strcpy(buf, "`WConclusion:`x\n");
      for (int i = 0; i < 20; i++) {
        if (dest->scene_conclusion[i] == TRUE) {
          sprintf(tmp, "%d) %s\nAt: %s\n\r", point, dest->scene_descs[i], indest_scene_location(dest, i));
          strcat(buf, tmp);
          point++;
        }
      }
      return str_dup(buf);
    }
    else {
      sprintf(buf, "%s\nAt: %s\n\r", dest->scene_descs[number], indest_scene_location(dest, number));
      return str_dup(buf);
    }
  }

  bool norp_match(CHAR_DATA *ch, CHAR_DATA *victim) {
    for (int i = 0; i < 50; i++) {
      if (!str_cmp(ch->pcdata->ignored_characters[i], victim->name))
      return TRUE;
      if (!str_cmp(victim->pcdata->ignored_characters[i], ch->name))
      return TRUE;
      if (!str_cmp(ch->pcdata->ignored_accounts[i], victim->pcdata->account->name))
      return TRUE;
      if (!str_cmp(victim->pcdata->ignored_accounts[i], ch->pcdata->account->name))
      return TRUE;
      if (!str_cmp(ch->pcdata->nowhere_characters[i], victim->name))
      return TRUE;
      if (!str_cmp(victim->pcdata->nowhere_characters[i], ch->name))
      return TRUE;
      if (victim->pcdata->account != NULL && !str_cmp(ch->pcdata->nowhere_accounts[i], victim->pcdata->account->name))
      return TRUE;
      if (ch->pcdata->account != NULL && !str_cmp(victim->pcdata->nowhere_accounts[i], ch->pcdata->account->name))
      return TRUE;
    }
    return FALSE;
  }

  char *scene_features(NEWDESTINY_TYPE *dest, int inum) {
    char buf[MSL];
    char tmp[MSL];
    strcpy(buf, "");
    if (dest->scene_special_one[inum] == DEST_FEAT_BRAND) {
      sprintf(tmp, "%s can be branded. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_HELPLESS) {
      sprintf(tmp, "%s is helpless. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_TURN) {
      sprintf(tmp, "%s can be turned. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_NOWIN) {
      sprintf(tmp, "%s is destined to lose any fight. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_NOSANC) {
      sprintf(tmp, "%s is without sanctuary. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_STUCK) {
      sprintf(tmp, "%s is unable to leave. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_PREY) {
      sprintf(tmp, "%s is the target of a predatory patrol. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_ARMED) {
      sprintf(tmp, "%s is armed. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_IMPRINTABLE) {
      sprintf(tmp, "%s is mindhacked. ", dest->role_one_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_one[inum] == DEST_FEAT_LIMITED) {
      sprintf(tmp, "%s has only limited sanctuary. ", dest->role_one_name);
      strcat(buf, tmp);
    }

    if (dest->scene_special_two[inum] == DEST_FEAT_BRAND) {
      sprintf(tmp, "%s can be branded. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_HELPLESS) {
      sprintf(tmp, "%s is helpless. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_TURN) {
      sprintf(tmp, "%s can be turned. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_NOWIN) {
      sprintf(tmp, "%s is destined to lose any fight. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_NOSANC) {
      sprintf(tmp, "%s is without sanctuary. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_STUCK) {
      sprintf(tmp, "%s is unable to leave. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_PREY) {
      sprintf(tmp, "%s is the target of a predatory patrol. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_ARMED) {
      sprintf(tmp, "%s is armed. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_IMPRINTABLE) {
      sprintf(tmp, "%s is mindhacked. ", dest->role_two_name);
      strcat(buf, tmp);
    }
    else if (dest->scene_special_two[inum] == DEST_FEAT_LIMITED) {
      sprintf(tmp, "%s has only limited sanctuary. ", dest->role_two_name);
      strcat(buf, tmp);
    }

    return str_dup(buf);
  }

  void launch_scene(CHAR_DATA *ch, CHAR_DATA *victim, NEWDESTINY_TYPE *dest, int inum) {
    ROOM_INDEX_DATA *troom;
    if (dest->scene_location[inum] == -1)
    troom = ch->in_room;
    if (dest->scene_location[inum] == -2)
    troom = victim->in_room;
    if (dest->scene_location[inum] == -3)
    troom = get_fleeroom(ch, NULL);
    if (dest->scene_location[inum] == -4)
    troom = get_fleeroom(victim, NULL);
    if (dest->scene_location[inum] == -5)
    troom = make_victim_room(victim);
    if (dest->scene_location[inum] > 0)
    troom = get_room_index(dest->scene_location[inum]);

    if (!can_destiny_scene(ch, troom) || !can_destiny_scene(victim, troom)) {
      send_to_char("One of both characters cannot currently enter a destiny scene.\n\r", ch);
      send_to_char("One of both characters cannot currently enter a destiny scene.\n\r", victim);
      return;
    }
    dest->lastused = current_time;
    if (str_cmp(ch->name, dest->author) && str_cmp(victim->name, dest->author))
    offline_karma(dest->author, 100);

    bool conclude = FALSE;
    if (dest->scene_conclusion[inum] == TRUE) {
      conclude = TRUE;
      int cpoint = ch->pcdata->scene_vote - 1;
      int vpoint = victim->pcdata->scene_vote - 1;
      if (cpoint == vpoint)
      inum = inum + cpoint;
      else {
        if (dest->conclude_type == 0) {
          if (number_percent() % 2 == 0)
          inum = inum + cpoint;
          else
          inum = inum + vpoint;

        }
        else if (dest->conclude_type == 1)
        inum = inum + cpoint;
        else if (dest->conclude_type == 2)
        inum = inum + vpoint;
      }
      if (inum > 19 || inum < 0 || !dest->scene_conclusion[inum] || dest->scene_conclusion[inum] == FALSE)
      return;
    }

    if (dest->scene_location[inum] == -1) {
      act("$n leaves.", victim, NULL, NULL, TO_ROOM);
      char_from_room(victim);
      char_to_room(victim, ch->in_room);
    }
    else if (dest->scene_location[inum] == -2) {
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
      char_to_room(ch, victim->in_room);
    }
    else if (dest->scene_location[inum] == -3) {
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      act("$n leaves.", victim, NULL, NULL, TO_ROOM);
      ROOM_INDEX_DATA *fleeroom = get_fleeroom(ch, NULL);
      char_from_room(ch);
      char_to_room(ch, fleeroom);
      char_from_room(victim);
      char_to_room(victim, fleeroom);
    }
    else if (dest->scene_location[inum] == -4) {
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      act("$n leaves.", victim, NULL, NULL, TO_ROOM);
      ROOM_INDEX_DATA *fleeroom = get_fleeroom(victim, NULL);
      char_from_room(ch);
      char_to_room(ch, fleeroom);
      char_from_room(victim);
      char_to_room(victim, fleeroom);
    }
    else if (dest->scene_location[inum] == -5) {
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      act("$n leaves.", victim, NULL, NULL, TO_ROOM);
      ROOM_INDEX_DATA *fleeroom = make_victim_room(victim);
      char_from_room(ch);
      char_to_room(ch, fleeroom);
      char_from_room(victim);
      char_to_room(victim, fleeroom);
    }
    else {
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      act("$n leaves.", victim, NULL, NULL, TO_ROOM);
      ROOM_INDEX_DATA *fleeroom = get_room_index(dest->scene_location[inum]);
      char_from_room(ch);
      char_to_room(ch, fleeroom);
      char_from_room(victim);
      char_to_room(victim, fleeroom);
    }
    if (IS_FLAG(ch->act, PLR_SHROUD))
    REMOVE_FLAG(ch->act, PLR_SHROUD);
    if (IS_FLAG(victim->act, PLR_SHROUD))
    REMOVE_FLAG(victim->act, PLR_SHROUD);

    printf_to_char(ch, "%s(%s) with %s(%s)`x\n%s\nFeatures:%s\n\r", ch->name, dest->role_one_name, victim->name, dest->role_two_name, dest->scene_descs[inum], scene_features(dest, inum));
    printf_to_char(victim, "%s(%s) with %s(%s)`x\n%s\nFeatures:%s\n\r", victim->name, dest->role_two_name, ch->name, dest->role_one_name, dest->scene_descs[inum], scene_features(dest, inum));
    ch->pcdata->destiny_feature = dest->scene_special_one[inum];
    ch->pcdata->destiny_feature_room = ch->in_room->vnum;
    victim->pcdata->destiny_feature = dest->scene_special_two[inum];
    victim->pcdata->destiny_feature_room = victim->in_room->vnum;
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->destiny_in_one[i] == dest->vnum)
      ch->pcdata->destiny_stage_one[i]++;
      if (victim->pcdata->destiny_in_two[i] == dest->vnum)
      victim->pcdata->destiny_stage_one[i]++;
    }
    if (ch->pcdata->destiny_feature == DEST_FEAT_ARMED)
    autogear(ch, TRUE);
    if (victim->pcdata->destiny_feature == DEST_FEAT_ARMED)
    autogear(victim, TRUE);
    if (victim->pcdata->destiny_feature == DEST_FEAT_PREY) {
      ch->pcdata->patrol_status = PATROL_PREYING;
      ch->pcdata->patrol_timer = 10;
      victim->pcdata->patrol_status = PATROL_PREY;
      victim->pcdata->patrol_timer = 10;
      victim->pcdata->patrol_target = ch;
      ch->pcdata->patrol_target = victim;
      ch->pcdata->villain_option = VILLAIN_OPTION_WAITING;
      victim->pcdata->prey_option = PREY_OPTION_CHOOSING;
      victim->pcdata->prey_emote_cooldown = 15;
      victim->pcdata->prey_option_cooldown = 5;
      free_string(ch->pcdata->building_fixation_name);
      ch->pcdata->building_fixation_name = str_dup(victim->name);
      ch->pcdata->building_fixation_level = 0;
      ch->pcdata->patrol_target = victim;
      bool move = FALSE;
      if (ch->in_room->vnum == 16490 || ch->in_room->vnum == 90052 || ch->in_room->vnum == 3438)
      move = TRUE;
      act("You move in on $N when nobody else is around, they have 10 minutes to choose if they want to run, hide or attack.", ch, NULL, victim, TO_CHAR);
      if (move == TRUE)
      act("You step out for a brief moment, to take out the trash, stretch your legs, because you heard a noise or for some other reason. Before you can return you spot $n moving towards you at a rapid walk, there's nobody else around to help you. (Type flee to try to run, hide to try to hide, attack $n to start a fight.)", ch, NULL, victim, TO_VICT);
      else
      act("You spot $n moving towards you at a rapid walk, there's nobody else around to help you. (Type flee to try to run, hide to try to hide, attack $n to start a fight.)", ch, NULL, victim, TO_VICT);
    }
    else if (ch->pcdata->destiny_feature == DEST_FEAT_PREY) {

      victim->pcdata->patrol_status = PATROL_PREYING;
      victim->pcdata->patrol_timer = 10;
      ch->pcdata->patrol_status = PATROL_PREY;
      ch->pcdata->patrol_timer = 10;
      ch->pcdata->patrol_target = victim;
      victim->pcdata->patrol_target = ch;
      victim->pcdata->villain_option = VILLAIN_OPTION_WAITING;
      ch->pcdata->prey_option = PREY_OPTION_CHOOSING;
      ch->pcdata->prey_emote_cooldown = 15;
      ch->pcdata->prey_option_cooldown = 5;
      free_string(ch->pcdata->building_fixation_name);
      victim->pcdata->building_fixation_name = str_dup(ch->name);
      victim->pcdata->building_fixation_level = 0;

      victim->pcdata->patrol_target = ch;
      bool move = FALSE;
      if (ch->in_room->vnum == 16490 || ch->in_room->vnum == 90052 || ch->in_room->vnum == 3438)
      move = TRUE;
      act("You move in on $N when nobody else is around, they have 10 minutes to choose if they want to run, hide or attack.", victim, NULL, ch, TO_CHAR);
      if (move == TRUE)
      act("You step out for a brief moment, to take out the trash, stretch your legs, because you heard a noise or for some other reason. Before you can return you spot $n moving towards you at a rapid walk, there's nobody else around to help you. (Type flee to try to run, hide to try to hide, attack $n to start a fight.)", victim, NULL, ch, TO_VICT);
      else
      act("You spot $n moving towards you at a rapid walk, there's nobody else around to help you. (Type flee to try to run, hide to try to hide, attack $n to start a fight.)", victim, NULL, ch, TO_VICT);
    }
    ch->pcdata->scene_offer = 0;
    ch->pcdata->scene_offer_role = 0;
    victim->pcdata->scene_offer = 0;
    victim->pcdata->scene_offer_role = 0;
    ch->pcdata->scene_vote = 0;
    victim->pcdata->scene_vote = 0;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->destiny_in_one[i] == dest->vnum && !str_cmp(ch->pcdata->destiny_in_with_one[i], victim->name)) {
        ch->pcdata->destiny_stage_one[i]++;
        if (conclude == TRUE) {
          ch->pcdata->destiny_in_one[i] = 0;
          ch->pcdata->destiny_stage_one[i] = 0;
          free_string(ch->pcdata->destiny_in_with_one[i]);
          ch->pcdata->destiny_in_with_one[i] = str_dup("");
        }
      }
      if (victim->pcdata->destiny_in_two[i] == dest->vnum && !str_cmp(victim->pcdata->destiny_in_with_two[i], ch->name)) {
        victim->pcdata->destiny_stage_two[i]++;
        if (conclude == TRUE) {
          victim->pcdata->destiny_in_two[i] = 0;
          victim->pcdata->destiny_stage_two[i] = 0;
          free_string(victim->pcdata->destiny_in_with_two[i]);
          victim->pcdata->destiny_in_with_two[i] = str_dup("");
        }
      }
    }
    if (narrative_pair(ch, victim)) {
      printf_to_char(ch, "%s", narrative_pair_rewards(ch, victim));
      printf_to_char(victim, "%s", narrative_pair_rewards(ch, victim));
    }
  }

  char *scene_location_tag(CHAR_DATA *ch, CHAR_DATA *victim, NEWDESTINY_TYPE *dest, int inum) {
    ROOM_INDEX_DATA *troom;
    if (dest->scene_location[inum] == 0)
    return "";
    if (dest->scene_location[inum] == -1)
    troom = ch->in_room;
    if (dest->scene_location[inum] == -2)
    troom = victim->in_room;
    if (dest->scene_location[inum] == -3)
    troom = get_fleeroom(ch, NULL);
    if (dest->scene_location[inum] == -4)
    troom = get_fleeroom(victim, NULL);
    if (dest->scene_location[inum] == -5)
    troom = make_victim_room(victim);
    if (dest->scene_location[inum] > 0)
    troom = get_room_index(dest->scene_location[inum]);
    if(troom == NULL)
    troom = make_victim_room(victim);
    char buf[MSL];
    strcpy(buf, "");
    if (is_underwater(troom))
    strcat(buf, "(Underwater)");
    if (troom->sector_type == SECT_AIR)
    strcat(buf, "(Airborne)");
    if (!in_haven(troom))
    strcat(buf, "(Not Haven)");
    return str_dup(buf);
  }

  void destiny_char_update(CHAR_DATA *ch) {
    if (ch->pcdata->destiny_offer != 0)
    return;
    if (ch->pcdata->scene_offer_role != 0)
    return;
    if (ch->pcdata->availability != AVAIL_HIGH && !IS_FLAG(ch->comm, COMM_WHEREVIS))
    return;
    if (ch->pcdata->destiny_feature != 0)
    return;
    if (IS_FLAG(ch->comm, COMM_AFK))
    return;

    CHAR_DATA *victim;
    NEWDESTINY_TYPE *dest;
    int score = -1;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;
      if ((d->connected != CON_PLAYING))
      continue;
      victim = d->character;
      if (IS_FLAG(victim->comm, COMM_AFK))
      continue;
      if (ch == victim)
      continue;
      if (victim->pcdata->availability == AVAIL_LOW)
      continue;
      if (norp_match(ch, victim))
      continue;
      if (victim->pcdata->destiny_feature != 0)
      continue;
      if (is_gm(victim))
      continue;
      if (IS_FLAG(victim->act, PLR_DEAD))
      continue;
      if (number_percent() % 2 == 0)
      continue;

      if (pc_pop(victim->in_room) > 1)
      continue;

      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->destiny_in_one[i] > 0 && !str_cmp(victim->name, ch->pcdata->destiny_in_with_one[i])) {
          NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_one[i]);
          if (tdest == NULL || tdest->scene_location[ch->pcdata->destiny_stage_one[i]] == 0) {
            ch->pcdata->destiny_in_one[i] = 0;
            return;
          }
          ch->pcdata->scene_offer = i;
          ch->pcdata->scene_offer_role = 1;
          for (int j = 0; j < 10; j++) {
            if (victim->pcdata->destiny_in_two[j] == tdest->vnum && !str_cmp(victim->pcdata->destiny_in_with_two[j], ch->name))
            victim->pcdata->scene_offer = j;
          }
          victim->pcdata->scene_offer_role = 2;
          printf_to_char(
          victim, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", ch->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_one[i]), scene_location_tag(ch, victim, tdest, ch->pcdata->destiny_stage_one[i]));
          printf_to_char(
          ch, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", victim->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_one[i]), scene_location_tag(ch, victim, tdest, ch->pcdata->destiny_stage_one[i]));
          return;
        }
        if (ch->pcdata->destiny_in_two[i] > 0 && !str_cmp(victim->name, ch->pcdata->destiny_in_with_two[i])) {
          NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_two[i]);
          if (tdest == NULL || tdest->scene_location[ch->pcdata->destiny_stage_two[i]] == 0) {
            ch->pcdata->destiny_in_two[i] = 0;
            return;
          }

          ch->pcdata->scene_offer = i;
          ch->pcdata->scene_offer_role = 2;
          for (int j = 0; j < 10; j++) {
            if (victim->pcdata->destiny_in_one[j] == tdest->vnum && !str_cmp(victim->pcdata->destiny_in_with_one[j], ch->name))
            victim->pcdata->scene_offer = j;
          }
          victim->pcdata->scene_offer_role = 1;
          printf_to_char(
          victim, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", ch->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_two[i]), scene_location_tag(victim, ch, tdest, ch->pcdata->destiny_stage_two[i]));
          printf_to_char(
          ch, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", victim->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_two[i]), scene_location_tag(victim, ch, tdest, ch->pcdata->destiny_stage_two[i]));
          return;
        }
      }

      for (vector<NEWDESTINY_TYPE *>::iterator ij = DestinyVect.begin();
      ij != DestinyVect.end(); ++ij) {
        if ((*ij)->valid == FALSE)
        continue;
        dest = (*ij);
        if (number_percent() % 2 == 1)
        continue;
        score = destiny_match_score(ch, victim, dest);
        if (score < 0)
        continue;
        if (score >= 150) {
          offer_destiny(ch, victim, dest);
          return;
        }
        else if (score >= 100) {
          if (number_percent() % 2 == 0) {
            offer_destiny(ch, victim, dest);
            return;
          }
        }
        else if (score >= 50) {
          if (number_percent() % 5 == 0) {
            offer_destiny(ch, victim, dest);
            return;
          }
        }
        else {
          if (number_percent() % 19 == 0) {
            offer_destiny(ch, victim, dest);
            return;
          }
        }

        score = destiny_match_score(victim, ch, dest);
        if (score < 0)
        continue;
        if (score >= 150) {
          offer_destiny(victim, ch, dest);
          return;
        }
        else if (score >= 100) {
          if (number_percent() % 2 == 0) {
            offer_destiny(victim, ch, dest);
            return;
          }
        }
        else if (score >= 50) {
          if (number_percent() % 5 == 0) {
            offer_destiny(victim, ch, dest);
            return;
          }
        }
        else {
          if (number_percent() % 19 == 0) {
            offer_destiny(victim, ch, dest);
            return;
          }
        }
      }
    }
  }

  _DOFUN(do_destiny) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if (!str_cmp(arg1, "list")) {
      static char string[MSL];
      char buf[MSL];
      string[0] = '\0';
      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (can_see_destiny(ch, (*it)) || IS_IMMORTAL(ch)) {
          sprintf(buf, "%s", destiny_title(ch, (*it)));
          strcat(string, buf);
        }
      }
      page_to_char(string, ch);
      return;
    }
    else if (!str_cmp(arg1, "info")) {
      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->vnum == atoi(argument))
        printf_to_char(ch, "%s\n\r", destiny_info(ch, (*it)));
      }
      return;
    }
    else if (!str_cmp(arg1, "sceneaccept")) {
      if (ch->pcdata->scene_offer_role == 1) {
        NEWDESTINY_TYPE *tdest =
        get_destiny(ch->pcdata->destiny_in_one[ch->pcdata->scene_offer]);
        CHAR_DATA *victim = get_char_world_pc(
        ch->pcdata->destiny_in_with_one[ch->pcdata->scene_offer]);
        if (tdest == NULL || victim == NULL) {
          send_to_char("Scene not found.\n\r", ch);
          return;
        }
        if (tdest->scene_conclusion
            [ch->pcdata->destiny_stage_one[ch->pcdata->scene_offer]] ==
            TRUE) {
          int val = atoi(argument);
          if (val < 1 || val > 20) {
            printf_to_char(
            ch, "Destiny sceneaccept (number)\n%s\n\r", destiny_scene_desc(
            tdest, ch->pcdata->destiny_stage_one[ch->pcdata->scene_offer]));
            return;
          }
          ch->pcdata->scene_vote = val;
        }
        ch->pcdata->scene_offer_role = -1;
        if (victim != NULL && can_destiny_scene(ch, NULL) && can_destiny_scene(victim, NULL)) {
          if (victim->pcdata->scene_offer_role < 0)
          launch_scene(ch, victim, tdest, ch->pcdata->destiny_stage_one[ch->pcdata->scene_offer]);
          else
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      else if (ch->pcdata->scene_offer_role == 2) {
        NEWDESTINY_TYPE *tdest =
        get_destiny(ch->pcdata->destiny_in_two[ch->pcdata->scene_offer]);
        CHAR_DATA *victim = get_char_world_pc(
        ch->pcdata->destiny_in_with_two[ch->pcdata->scene_offer]);
        if (tdest == NULL || victim == NULL) {
          send_to_char("Scene not found.\n\r", ch);
          return;
        }
        if (tdest->scene_conclusion
            [ch->pcdata->destiny_stage_two[ch->pcdata->scene_offer]] ==
            TRUE) {
          int val = atoi(argument);
          if (val < 1 || val > 20) {
            printf_to_char(
            ch, "Destiny sceneaccept (number)\n%s\n\r", destiny_scene_desc(
            tdest, ch->pcdata->destiny_stage_two[ch->pcdata->scene_offer]));
            return;
          }
          ch->pcdata->scene_vote = val;
        }
        ch->pcdata->scene_offer_role = -2;
        if (victim != NULL && can_destiny_scene(ch, NULL) && can_destiny_scene(victim, NULL)) {
          if (victim->pcdata->scene_offer_role < 0)
          launch_scene(victim, ch, tdest, ch->pcdata->destiny_stage_two[ch->pcdata->scene_offer]);
          else
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "drop")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int point = atoi(arg2);
      int ipoint = 1;
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->destiny_in_one[i] > 0) {
          if (ipoint == point) {
            NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_one[i]);
            CHAR_DATA *victim =
            get_char_world_pc(ch->pcdata->destiny_in_with_one[i]);
            if (tdest == NULL || victim == NULL) {
              send_to_char("Destiny not found.\n\r", ch);
              return;
            }
            if (victim == NULL) {
              send_to_char("You can only drop a destiny while the person you're matched with is online.\n\r", ch);
              return;
            }
            for (int j = 0; j < 10; j++) {
              if (victim->pcdata->destiny_in_two[j] == tdest->vnum && !str_cmp(victim->pcdata->destiny_in_with_two[j], ch->name)) {
                victim->pcdata->destiny_in_two[j] = 0;
              }
            }
            ch->pcdata->destiny_in_one[i] = 0;
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
        if (ch->pcdata->destiny_in_two[i] > 0) {
          if (ipoint == point) {
            NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_two[i]);
            CHAR_DATA *victim =
            get_char_world_pc(ch->pcdata->destiny_in_with_two[i]);
            if (tdest == NULL || victim == NULL) {
              send_to_char("Destiny not found.\n\r", ch);
              return;
            }
            if (victim == NULL) {
              send_to_char("You can only drop a destiny while the person you're matched with is online.\n\r", ch);
              return;
            }
            for (int j = 0; j < 10; j++) {
              if (victim->pcdata->destiny_in_one[j] == tdest->vnum && !str_cmp(victim->pcdata->destiny_in_with_one[j], ch->name)) {
                victim->pcdata->destiny_in_one[j] = 0;
              }
            }
            ch->pcdata->destiny_in_two[i] = 0;
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
    }
    else if (!str_cmp(arg1, "offerscene")) {
      if (!can_destiny_scene(ch, NULL)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int point = atoi(arg2);
      int ipoint = 1;
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->destiny_in_one[i] > 0) {
          if (ipoint == point) {
            NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_one[i]);
            CHAR_DATA *victim =
            get_char_world_pc(ch->pcdata->destiny_in_with_one[i]);
            if (tdest == NULL) {
              send_to_char("Destiny not found.\n\r", ch);
              return;
            }
            if (victim != NULL && can_see_destiny_char(ch, victim) && can_destiny_scene(ch, NULL) && can_destiny_scene(victim, NULL)) {
              ch->pcdata->scene_offer = i;
              if (tdest->scene_conclusion[ch->pcdata->destiny_stage_one[i]] ==
                  TRUE)
              ch->pcdata->scene_offer_role = 1;
              else
              ch->pcdata->scene_offer_role = -1;
              victim->pcdata->scene_offer_role = 2;
              for (int j = 0; j < 10; j++) {
                if (victim->pcdata->destiny_in_two[j] == tdest->vnum && !str_cmp(victim->pcdata->destiny_in_with_two[j], ch->name))
                victim->pcdata->scene_offer = j;
              }
              printf_to_char(
              victim, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", ch->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_one[i]), scene_location_tag(ch, victim, tdest, ch->pcdata->destiny_stage_one[i]));
              if (tdest->scene_conclusion[ch->pcdata->destiny_stage_one[i]] ==
                  TRUE)
              printf_to_char(
              ch, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", victim->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_one[i]), scene_location_tag(ch, victim, tdest, ch->pcdata->destiny_stage_one[i]));
              else
              send_to_char("Done.\n\r", ch);
              return;
            }
            else {
              send_to_char("They aren't available or one of you isn't able to do a scene at the moment.\n\r", ch);
              return;
            }
          }
          ipoint++;
        }
        if (ch->pcdata->destiny_in_two[i] > 0) {
          if (ipoint == point) {
            NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_two[i]);
            CHAR_DATA *victim =
            get_char_world_pc(ch->pcdata->destiny_in_with_two[i]);
            if (tdest == NULL) {
              send_to_char("Destiny not found.\n\r", ch);
              return;
            }
            if (victim != NULL && can_see_destiny_char(ch, victim) && can_destiny_scene(ch, NULL) && can_destiny_scene(victim, NULL)) {
              ch->pcdata->scene_offer = i;
              if (tdest->scene_conclusion[ch->pcdata->destiny_stage_two[i]] ==
                  TRUE)
              ch->pcdata->scene_offer_role = 2;
              else
              ch->pcdata->scene_offer_role = -2;
              victim->pcdata->scene_offer_role = 1;
              for (int j = 0; j < 10; j++) {
                if (victim->pcdata->destiny_in_one[j] == tdest->vnum && !str_cmp(victim->pcdata->destiny_in_with_one[j], ch->name))
                victim->pcdata->scene_offer = j;
              }
              printf_to_char(
              victim, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", ch->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_two[i]), scene_location_tag(victim, ch, tdest, ch->pcdata->destiny_stage_two[i]));
              if (tdest->scene_conclusion[ch->pcdata->destiny_stage_two[i]] ==
                  TRUE)
              printf_to_char(
              ch, "Would you like to do the next scene in your character's destiny with %s?(Destiny sceneaccept to agree.)\n:%s%s\n\r", victim->name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_two[i]), scene_location_tag(victim, ch, tdest, ch->pcdata->destiny_stage_two[i]));
              else
              send_to_char("Done.\n\r", ch);
              return;
            }
            else {
              send_to_char("They aren't available or one of you isn't able to do a scene at the moment.\n\r", ch);
              return;
            }
          }
          ipoint++;
        }
      }
    }
    else if (!str_cmp(arg1, "scenes")) {
      char buf[MSL];
      strcpy(buf, "");
      char tmp[MSL];
      int point = 1;
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->destiny_in_one[i] > 0) {
          NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_one[i]);
          CHAR_DATA *victim =
          get_char_world_pc(ch->pcdata->destiny_in_with_one[i]);
          if (tdest == NULL) {
            send_to_char("Destiny not found.\n\r", ch);
            return;
          }
          if (victim != NULL && can_see_destiny_char(ch, victim)) {
            sprintf(tmp, "`g%d: %s(%s) with %s(%s)`x\n%s\n\n\r", point, ch->name, tdest->role_one_name, victim->name, tdest->role_two_name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_one[i]));
            strcat(buf, tmp);
          }
          else {
            sprintf(tmp, "`r%d: %s(%s) with %s(%s)`x\n%s\n\n\r", point, ch->name, tdest->role_one_name, ch->pcdata->destiny_in_with_one[i], tdest->role_two_name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_one[i]));
            strcat(buf, tmp);
          }
          point++;
        }
        if (ch->pcdata->destiny_in_two[i] > 0) {
          NEWDESTINY_TYPE *tdest = get_destiny(ch->pcdata->destiny_in_two[i]);
          CHAR_DATA *victim =
          get_char_world_pc(ch->pcdata->destiny_in_with_two[i]);
          if (tdest == NULL) {
            send_to_char("Destiny not found.\n\r", ch);
            return;
          }
          if (victim != NULL && can_see_destiny_char(ch, victim)) {
            sprintf(tmp, "`g%d: %s(%s) with %s(%s)`x\n%s\n\n\r", point, ch->name, tdest->role_two_name, victim->name, tdest->role_one_name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_two[i]));
            strcat(buf, tmp);
          }
          else {
            sprintf(tmp, "`r%d: %s(%s) with %s(%s)`x\n%s\n\n\r", point, ch->name, tdest->role_two_name, ch->pcdata->destiny_in_with_two[i], tdest->role_one_name, destiny_scene_desc(tdest, ch->pcdata->destiny_stage_two[i]));
            strcat(buf, tmp);
          }
          point++;
        }
      }
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
      return;

    }
    else if (!str_cmp(arg1, "accept")) {
      if (ch->pcdata->destiny_offer == 0 || ch->pcdata->destiny_offer_char == NULL) {
        send_to_char("You have no destiny to accept.\n\r", ch);
        return;
      }
      if (ch->pcdata->destiny_offer_char->pcdata->destiny_offer_role < 0) {
        CHAR_DATA *victim = ch->pcdata->destiny_offer_char;
        if (ch->pcdata->destiny_offer_role == 1) {
          int cpoint = 0, vpoint = 0;
          for (int i = 0; i < 10; i++) {
            if (ch->pcdata->destiny_in_one[i] == 0 && cpoint == 0)
            cpoint = i;
            if (victim->pcdata->destiny_in_two[i] == 0 && vpoint == 0)
            vpoint = i;
          }
          ch->pcdata->destiny_in_one[cpoint] = ch->pcdata->destiny_offer;
          victim->pcdata->destiny_in_two[vpoint] = ch->pcdata->destiny_offer;
          free_string(ch->pcdata->destiny_in_with_one[cpoint]);
          ch->pcdata->destiny_in_with_one[cpoint] = str_dup(victim->name);
          free_string(victim->pcdata->destiny_in_with_two[vpoint]);
          victim->pcdata->destiny_in_with_two[vpoint] = str_dup(ch->name);
          ch->pcdata->destiny_stage_one[cpoint] = 0;
          victim->pcdata->destiny_stage_two[vpoint] = 0;
          send_to_char("Destiny begun! (Use destiny scenes to list possible scenes.)\n\r", ch);
          send_to_char("Destiny begun! (Use destiny scenes to list possible scenes.)\n\r", victim);
          ch->pcdata->destiny_offer = 0;
          victim->pcdata->destiny_offer = 0;
          return;
        }
        else {
          int cpoint = 0, vpoint = 0;
          for (int i = 0; i < 10; i++) {
            if (ch->pcdata->destiny_in_two[i] == 0 && cpoint == 0)
            cpoint = i;
            if (victim->pcdata->destiny_in_one[i] == 0 && vpoint == 0)
            vpoint = i;
          }
          ch->pcdata->destiny_in_two[cpoint] = ch->pcdata->destiny_offer;
          victim->pcdata->destiny_in_one[vpoint] = ch->pcdata->destiny_offer;
          free_string(ch->pcdata->destiny_in_with_two[cpoint]);
          ch->pcdata->destiny_in_with_two[cpoint] = str_dup(victim->name);
          free_string(victim->pcdata->destiny_in_with_one[vpoint]);
          victim->pcdata->destiny_in_with_one[vpoint] = str_dup(ch->name);
          ch->pcdata->destiny_stage_two[cpoint] = 0;
          victim->pcdata->destiny_stage_one[vpoint] = 0;
          send_to_char("Destiny begun! (Use destiny scenes to list possible scenes.)\n\r", ch);
          send_to_char("Destiny begun! (Use destiny scenes to list possible scenes.)\n\r", victim);
          ch->pcdata->destiny_offer = 0;
          victim->pcdata->destiny_offer = 0;
          return;
        }
      }
      else if (ch->pcdata->destiny_offer_char->pcdata->destiny_offer == 0) {
        ch->pcdata->destiny_offer = 0;
        send_to_char("Destiny accepted. You won't see anything further unless the other participant also accepts in the next few minutes.\n\r", ch);
      }
      else {
        ch->pcdata->destiny_offer_role *= -1;
        send_to_char("Destiny accepted. You won't see anything further unless the other participant also accepts in the next few minutes.\n\r", ch);
      }
    }
    else if (!str_cmp(arg1, "reject")) {
      if (ch->pcdata->destiny_offer == 0 || ch->pcdata->destiny_offer_char == NULL) {
        send_to_char("You have no destiny to reject.\n\r", ch);
        return;
      }
      ch->pcdata->destiny_reject = ch->pcdata->destiny_offer;
      ch->pcdata->destiny_reject_cooldown = 60;
      ch->pcdata->destiny_offer = 0;
      send_to_char("Destiny rejected.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "delete")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int point = atoi(arg2);

      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (point == (*it)->vnum) {
          if (!IS_IMMORTAL(ch)) {
            if (str_cmp((*it)->author, ch->name)) {
              send_to_char("You can only delete destinies you've created.\n\r", ch);
              return;
            }
            if ((*it)->created_at + (3600 * 2) > current_time) {
              send_to_char("You can only delete destinties that have been made in the last two hours.\n\r", ch);
              return;
            }
          }
          (*it)->valid = FALSE;
          send_to_char("Destiny deleted.\n\r", ch);
          return;
        }
      }

    }
    else if (!str_cmp(arg1, "request")) {
      if (get_tier(ch) < 2) {
        send_to_char("Fate has not yet deemed you worthy of her time.\n\r", ch);
        return;
      }
      CHAR_DATA *victim;
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if (is_number(arg2)) {
        for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
        it != DestinyVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if (atoi(arg2) == (*it)->vnum) {
            if (!str_cmp(argument, "roleone") || !str_cmp(argument, "role one") || !str_cmp(argument, "one") || !str_cmp(argument, (*it)->role_one_name)) {
              int score = -1;
              for (DescList::iterator ij = descriptor_list.begin();
              ij != descriptor_list.end(); ++ij) {
                DESCRIPTOR_DATA *d = *ij;

                if (!d->character)
                continue;
                if ((d->connected != CON_PLAYING))
                continue;
                victim = d->character;
                if (IS_FLAG(victim->comm, COMM_AFK))
                continue;
                if (ch == victim)
                continue;
                if (victim->pcdata->availability == AVAIL_LOW)
                continue;
                if (norp_match(ch, victim))
                continue;
                if (victim->pcdata->destiny_feature != 0)
                continue;
                if (is_gm(victim))
                continue;
                if (IS_FLAG(victim->act, PLR_DEAD))
                continue;
                if (number_percent() % 2 == 0)
                continue;
                score = destiny_match_score(ch, victim, (*it));
                if (score < 0)
                continue;
                if (score >= 150) {
                  offer_destiny(ch, victim, (*it));
                  return;
                }
                else if (score >= 100) {
                  if (number_percent() % 2 == 0) {
                    offer_destiny(ch, victim, (*it));
                    return;
                  }
                }
                else if (score >= 50) {
                  if (number_percent() % 5 == 0) {
                    offer_destiny(ch, victim, (*it));
                    return;
                  }
                }
                else {
                  if (number_percent() % 19 == 0) {
                    offer_destiny(ch, victim, (*it));
                    return;
                  }
                }
              }
            }
            else if (!str_cmp(argument, "roletwo") || !str_cmp(argument, "role two") || !str_cmp(argument, "two") || !str_cmp(argument, (*it)->role_two_name)) {
              int score = -1;
              for (DescList::iterator ij = descriptor_list.begin();
              ij != descriptor_list.end(); ++ij) {
                DESCRIPTOR_DATA *d = *ij;

                if (!d->character)
                continue;
                if ((d->connected != CON_PLAYING))
                continue;
                victim = d->character;
                if (IS_FLAG(victim->comm, COMM_AFK))
                continue;
                if (ch == victim)
                continue;
                if (victim->pcdata->availability == AVAIL_LOW)
                continue;
                if (norp_match(ch, victim))
                continue;
                if (victim->pcdata->destiny_feature != 0)
                continue;
                if (is_gm(victim))
                continue;
                if (IS_FLAG(victim->act, PLR_DEAD))
                continue;
                if (number_percent() % 2 == 0)
                continue;
                score = destiny_match_score(victim, ch, (*it));
                if (score < 0)
                continue;
                if (score >= 150) {
                  offer_destiny(victim, ch, (*it));
                  return;
                }
                else if (score >= 100) {
                  if (number_percent() % 2 == 0) {
                    offer_destiny(victim, ch, (*it));
                    return;
                  }
                }
                else if (score >= 50) {
                  if (number_percent() % 5 == 0) {
                    offer_destiny(victim, ch, (*it));
                    return;
                  }
                }
                else {
                  if (number_percent() % 19 == 0) {
                    offer_destiny(victim, ch, (*it));
                    return;
                  }
                }
              }
            }
          }
        }
        send_to_char("No destiny match found.\n\r", ch);
        return;
      }
      else {
        CHAR_DATA *victim = get_char_world_pc(argument);
        NEWDESTINY_TYPE *dest;
        if (victim == NULL) {
          send_to_char("They are not online, or are whoinvis, or do not have high availability..\n\r", ch);
          return;
        }
        if (victim->pcdata->availability == AVAIL_HIGH || !IS_FLAG(victim->comm, COMM_WHOINVIS)) {
          for (vector<NEWDESTINY_TYPE *>::iterator ij = DestinyVect.begin();
          ij != DestinyVect.end(); ++ij) {
            if ((*ij)->valid == FALSE)
            continue;
            dest = (*ij);
            if (number_percent() % 2 == 1)
            continue;
            int score = destiny_match_score(ch, victim, dest);
            if (score < 0)
            continue;
            if (score >= 150) {
              offer_destiny(ch, victim, dest);
              return;
            }
            else if (score >= 100) {
              if (number_percent() % 2 == 0) {
                offer_destiny(ch, victim, dest);
                return;
              }
            }
            else if (score >= 50) {
              if (number_percent() % 5 == 0) {
                offer_destiny(ch, victim, dest);
                return;
              }
            }
            else {
              if (number_percent() % 19 == 0) {
                offer_destiny(ch, victim, dest);
                return;
              }
            }

            score = destiny_match_score(victim, ch, dest);
            if (score < 0)
            continue;
            if (score >= 150) {
              offer_destiny(victim, ch, dest);
              return;
            }
            else if (score >= 100) {
              if (number_percent() % 2 == 0) {
                offer_destiny(victim, ch, dest);
                return;
              }
            }
            else if (score >= 50) {
              if (number_percent() % 5 == 0) {
                offer_destiny(victim, ch, dest);
                return;
              }
            }
            else {
              if (number_percent() % 19 == 0) {
                offer_destiny(victim, ch, dest);
                return;
              }
            }
          }
        }
        else {
          send_to_char("They are not online, or are whoinvis, or do not have high availability.\n\r", ch);
          return;
        }
      }
      send_to_char("No destiny found.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "offer")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg3);

      int point = atoi(arg2);
      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (point == (*it)->vnum) {
          /*
if (str_cmp((*it)->author, ch->name)) {

send_to_char("You can only offer destinies you've created.\n\r", ch);
return;
}
*/
          CHAR_DATA *victim = get_char_world_pc(argument);
          if (victim == NULL) {
            send_to_char("They are not online, or are whoinvis, or do not have high availability..\n\r", ch);
            return;
          }
          if (victim->pcdata->availability == AVAIL_HIGH || !IS_FLAG(victim->comm, COMM_WHOINVIS)) {
            if (!str_cmp(arg3, "roleone") || !str_cmp(arg3, "role one") || !str_cmp(arg3, "one") || !str_cmp(arg3, (*it)->role_one_name)) {
              int score = destiny_match_score(ch, victim, (*it));
              if (score == -1) {
                send_to_char("You are not compatible.\n\r", ch);
                return;
              }
              offer_destiny(ch, victim, (*it));
              return;
            }
            else if (!str_cmp(arg3, "roletwo") || !str_cmp(arg3, "role two") || !str_cmp(arg3, "two") || !str_cmp(arg3, (*it)->role_two_name)) {
              int score = destiny_match_score(victim, ch, (*it));
              if (score == -1) {
                send_to_char("You are not compatible.\n\r", ch);
                return;
              }
              offer_destiny(victim, ch, (*it));
              return;
            }
            else {
              send_to_char("Syntax: Destiny offer (number) (your role) (character)\n\r", ch);
              return;
            }
          }
          else {
            send_to_char("They are not online, or are whoinvis, or do not have high availability.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("Syntax: Destiny offer (number) (your role) (character)\n\r", ch);
      return;

    }
    else if (!str_cmp(arg1, "block")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int point = atoi(arg2);
      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (point == (*it)->vnum) {
          if (!str_cmp(argument, (*it)->role_one_name)) {
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_block_one[i] == point) {
                ch->pcdata->destiny_block_one[i] = 0;
                send_to_char("Block canceled.\n\r", ch);
                return;
              }
            }
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_block_one[i] == 0) {
                ch->pcdata->destiny_block_one[i] = point;
                send_to_char("Block set.\n\r", ch);
                return;
              }
            }
          }
          if (!str_cmp(argument, (*it)->role_two_name)) {
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_block_two[i] == point) {
                ch->pcdata->destiny_block_two[i] = 0;
                send_to_char("Block canceled.\n\r", ch);
                return;
              }
            }
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_block_two[i] == 0) {
                ch->pcdata->destiny_block_two[i] = point;
                send_to_char("Block set.\n\r", ch);
                return;
              }
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "interested") || !str_cmp(arg1, "interest")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int point = atoi(arg2);
      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (point == (*it)->vnum) {
          if (!str_cmp(argument, (*it)->role_one_name)) {
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_interest_one[i] == point) {
                ch->pcdata->destiny_interest_one[i] = 0;
                send_to_char("Interest canceled.\n\r", ch);
                return;
              }
            }
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_interest_one[i] == 0) {
                ch->pcdata->destiny_interest_one[i] = point;
                send_to_char("Interest set.\n\r", ch);
                return;
              }
            }
          }
          if (!str_cmp(argument, (*it)->role_two_name)) {
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_interest_two[i] == point) {
                ch->pcdata->destiny_interest_two[i] = 0;
                send_to_char("Interest canceled.\n\r", ch);
                return;
              }
            }
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->destiny_interest_two[i] == 0) {
                ch->pcdata->destiny_interest_two[i] = point;
                send_to_char("Interest set.\n\r", ch);
                return;
              }
            }
          }
        }
      }
    }
  }

  _DOFUN(do_exclude) {
    if (ch->pcdata->ci_editing == 16) {
      char arg1[MSL];
      argument = one_argument_nouncap(argument, arg1);
      if (!str_cmp(arg1, "Character")) {
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_CHAR;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "Gender")) {
        if (str_cmp(argument, "male") && str_cmp(argument, "female")) {
          send_to_char("Options are: Male, Female.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_GENDER;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "Age")) {
        if (str_cmp(argument, "under 21") && str_cmp(argument, "over 21") && str_cmp(argument, "over 30")) {
          send_to_char("Options are: Under 21, Over 21, Over 30.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_AGE;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "Society")) {
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_SOCIETY;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "Hours")) {
        if (str_cmp(argument, "low") && str_cmp(argument, "high")) {
          send_to_char("Options are: Low, High.(Low is < 50 hours, high is > 500 hours.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_HOURS;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "tier")) {
        if (str_cmp(argument, "1") && str_cmp(argument, "2") && str_cmp(argument, "3") && str_cmp(argument, "5") && str_cmp(argument, "4")) {
          send_to_char("Options are: 1, 2, 3, 4, 5.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_TIER;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "morality")) {
        if (str_cmp(argument, "corrupt") && str_cmp(argument, "virtuous")) {
          send_to_char("Options are: corrupt, virtuous.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_MORALITY;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "archetype")) {
        if (str_cmp(argument, "faeborn") && str_cmp(argument, "demigod") && str_cmp(argument, "demonborn") && str_cmp(argument, "angelborn") && str_cmp(argument, "werewolves") && str_cmp(argument, "vampires") && str_cmp(argument, "supernaturals")) {
          send_to_char("Options are: faeborn, demigod, demonborn, angelborn, werewolves, vampires, supernaturals.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_ARCHETYPE;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(arg1, "institute")) {
        if(str_cmp(argument, "college students") && str_cmp(argument, "college staff") && str_cmp(argument, "clinic patients") && str_cmp(argument, "clinic staff")) {
          send_to_char("Options are: college students, college staff, clinic patients, clinic staff.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_INSTITUTE;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }

      if (!str_cmp(arg1, "focus")) {
        if (str_cmp(argument, "arcanists") && str_cmp(argument, "combatants") && str_cmp(argument, "professionals")) {
          send_to_char("Options are: arcanists, combatants, professionals.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (ch->pcdata->ci_exclude_type[i] == 0) {
            ch->pcdata->ci_exclude_type[i] = CRITERIA_FOCUS;
            free_string(ch->pcdata->ci_excludes[i]);
            ch->pcdata->ci_excludes[i] = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
    }
  }

  bool can_destiny_one(CHAR_DATA *ch, NEWDESTINY_TYPE *dest) {
    if (ch->race == RACE_FANTASY)
    return FALSE;
    if (dest->vnum == ch->pcdata->destiny_reject)
    return FALSE;
    if (ch->pcdata->destiny_reject_cooldown > 0)
    return FALSE;
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->destiny_block_one[i] == dest->vnum)
      return FALSE;
    }
    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return FALSE;
    if (dest->arch_one == 1 && !is_vampire(ch))
    return FALSE;
    if (dest->arch_one == 2 && !is_werewolf(ch))
    return FALSE;
    if (dest->arch_one == 3 && !is_demigod(ch))
    return FALSE;
    if (dest->arch_one == 4 && !is_demonborn(ch))
    return FALSE;
    if (dest->arch_one == 5 && !is_angelborn(ch))
    return FALSE;
    if (dest->arch_one == 6 && !is_faeborn(ch))
    return FALSE;
    if (dest->arch_one == 7 && is_super(ch))
    return FALSE;
    if (dest->arch_one == 8 && !college_staff(ch, FALSE) && !clinic_staff(ch, FALSE))
    return FALSE;
    if (dest->arch_one == 9)
    return FALSE;
    if (dest->arch_one == 10 && !college_student(ch, TRUE))
    return FALSE;
    if (dest->arch_one == 11 && !clinic_patient(ch))
    return FALSE;

    if (dest->restrict_one == DEST_REST_UNDERT2 && get_tier(ch) < 2)
    return FALSE;
    if (dest->restrict_one == DEST_REST_UNDERT3 && get_tier(ch) < 3)
    return FALSE;
    if (dest->restrict_one == DEST_REST_UNDERT4 && get_tier(ch) < 4)
    return FALSE;
    if (dest->restrict_one == DEST_REST_UNDERT5 && get_tier(ch) < 5)
    return FALSE;
    if (dest->restrict_one == DEST_REST_ABOVET1 && get_tier(ch) > 1)
    return FALSE;
    if (dest->restrict_one == DEST_REST_ABOVET2 && get_tier(ch) > 2)
    return FALSE;
    if (dest->restrict_one == DEST_REST_ABOVET3 && get_tier(ch) > 3)
    return FALSE;
    if (dest->restrict_one == DEST_REST_ABOVET4 && get_tier(ch) > 4)
    return FALSE;
    if (dest->restrict_one == DEST_REST_PROF && prof_focus(ch) > combat_focus(ch) && prof_focus(ch) > arcane_focus(ch))
    return FALSE;
    if (dest->restrict_one == DEST_REST_SORC && arcane_focus(ch) > combat_focus(ch) && arcane_focus(ch) > prof_focus(ch))
    return FALSE;
    if (dest->restrict_one == DEST_REST_FIGHT && combat_focus(ch) > prof_focus(ch) && combat_focus(ch) > arcane_focus(ch))
    return FALSE;
    if (dest->restrict_one == DEST_REST_UNDER30 && get_age(ch) < 30)
    return FALSE;
    if (dest->restrict_one == DEST_REST_OVER30 && get_age(ch) >= 30)
    return FALSE;

    return TRUE;
  }

  bool can_destiny_two(CHAR_DATA *ch, NEWDESTINY_TYPE *dest) {
    if (ch->race == RACE_FANTASY)
    return FALSE;
    if (dest->vnum == ch->pcdata->destiny_reject)
    return FALSE;
    if (ch->pcdata->destiny_reject_cooldown > 0)
    return FALSE;
    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return FALSE;
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->destiny_block_two[i] == dest->vnum)
      return FALSE;
    }

    if (dest->arch_two == 1 && !is_vampire(ch))
    return FALSE;
    if (dest->arch_two == 2 && !is_werewolf(ch))
    return FALSE;
    if (dest->arch_two == 3 && !is_demigod(ch))
    return FALSE;
    if (dest->arch_two == 4 && !is_demonborn(ch))
    return FALSE;
    if (dest->arch_two == 5 && !is_angelborn(ch))
    return FALSE;
    if (dest->arch_two == 6 && !is_faeborn(ch))
    return FALSE;
    if (dest->arch_two == 7 && is_super(ch))
    return FALSE;
    if (dest->arch_two == 8 && !college_staff(ch, FALSE) && !clinic_staff(ch, FALSE))
    return FALSE;
    if (dest->arch_two == 9)
    return FALSE;
    if (dest->arch_two == 10 && !college_student(ch, TRUE))
    return FALSE;
    if (dest->arch_two == 11 && !clinic_patient(ch))
    return FALSE;

    if (dest->restrict_two == DEST_REST_UNDERT2 && get_tier(ch) < 2)
    return FALSE;
    if (dest->restrict_two == DEST_REST_UNDERT3 && get_tier(ch) < 3)
    return FALSE;
    if (dest->restrict_two == DEST_REST_UNDERT4 && get_tier(ch) < 4)
    return FALSE;
    if (dest->restrict_two == DEST_REST_UNDERT5 && get_tier(ch) < 5)
    return FALSE;
    if (dest->restrict_two == DEST_REST_ABOVET1 && get_tier(ch) > 1)
    return FALSE;
    if (dest->restrict_two == DEST_REST_ABOVET2 && get_tier(ch) > 2)
    return FALSE;
    if (dest->restrict_two == DEST_REST_ABOVET3 && get_tier(ch) > 3)
    return FALSE;
    if (dest->restrict_two == DEST_REST_ABOVET4 && get_tier(ch) > 4)
    return FALSE;
    if (dest->restrict_two == DEST_REST_PROF && prof_focus(ch) > combat_focus(ch) && prof_focus(ch) > arcane_focus(ch))
    return FALSE;
    if (dest->restrict_two == DEST_REST_SORC && arcane_focus(ch) > combat_focus(ch) && arcane_focus(ch) > prof_focus(ch))
    return FALSE;
    if (dest->restrict_two == DEST_REST_FIGHT && combat_focus(ch) > prof_focus(ch) && combat_focus(ch) > arcane_focus(ch))
    return FALSE;
    if (dest->restrict_two == DEST_REST_UNDER30 && get_age(ch) < 30)
    return FALSE;
    if (dest->restrict_two == DEST_REST_OVER30 && get_age(ch) >= 30)
    return FALSE;

    return TRUE;
  }

  bool crisis_runner(CHAR_DATA *ch) {
    if (ch == NULL || !is_gm(ch))
    return FALSE;

    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0')
      continue;
      if ((*it)->valid == FALSE)
      continue;
      if ((!str_cmp((*it)->author, ch->name) || !str_cmp((*it)->author, nosr_name(ch->name))) && (*it)->crisis == 1 && (*it)->time <= current_time && (*it)->time >= (current_time - (3600 * 6)))
      return TRUE;
    }
    return FALSE;
  }

  char *narrative_desc(int narr, int rtype) {
    if (narr == NARRATIVE_POWERFUL) {
      if (rtype == 1)
      return "feel/be seen as powerful";
    }
    else if (narr == NARRATIVE_MORAL) {
      if (rtype == 1)
      return "feel/be seen as morally strong";
    }
    else if (narr == NARRATIVE_HUMAN) {
      if (rtype == 1)
      return "feel/be seen as humanized/relatable/sympathetic";
    }
    else if (narr == NARRATIVE_BADASS) {
      if (rtype == 1)
      return "feel/be seen as badass";
    }
    else if (narr == NARRATIVE_BRAVE_PHYS) {
      if (rtype == 1)
      return "feel/be seen as brave(physical danger)";
    }
    else if (narr == NARRATIVE_BRAVE_HORROR) {
      if (rtype == 1)
      return "feel/be seen as brave(horror)";
    }
    else if (narr == NARRATIVE_LOYAL_CAUSE) {
      if (rtype == 1)
      return "feel/be seen as loyal(to cause)";
    }
    else if (narr == NARRATIVE_LOYAL_PERSON) {
      if (rtype == 1)
      return "feel/be seen as loyal(to person)";
    }
    else if (narr == NARRATIVE_SELFLESS) {
      if (rtype == 1)
      return "feel/be seen as selfless";
    }
    else if (narr == NARRATIVE_ROMANTIC) {
      if (rtype == 1)
      return "feel/be seen as romantically adept";
    }
    else if (narr == NARRATIVE_CLEVER) {
      if (rtype == 1)
      return "feel/be seen as clever";
    }
    else if (narr == NARRATIVE_CUNNING) {
      if (rtype == 1)
      return "feel/be seen as cunning";
    }
    else if (narr == NARRATIVE_CHARISMATIC) {
      if (rtype == 1)
      return "feel/be seen as charismatic";
    }
    else if (narr == NARRATIVE_RUTHLESS) {
      if (rtype == 1)
      return "feel/be seen as ruthless";
    }
    else if (narr == NARRATIVE_STRONGWILL) {
      if (rtype == 1)
      return "feel/be seen as strong willed/determined";
    }
    else if (narr == NARRATIVE_COVET) {
      if (rtype == 1)
      return "feel/be seen as coveted/covetable";
    }
    else if (narr == NARRATIVE_UNIQUECOVET) {
      if (rtype == 1)
      return "feel/be seen as uniquely coveted/covetable";
    }
    else if (narr == NARRATIVE_LEADER) {
      if (rtype == 1)
      return "feel/be seen as a good leader";
    }
    else if (narr == NARRATIVE_SCARY) {
      if (rtype == 1)
      return "feel/be seen as scary/dangerous";
    }
    else if (narr == NARRATIVE_INTERESTING) {
      if (rtype == 1)
      return "feel/be seen as uniquely interesting/mysterious";
    }
    else if (narr == NARRATIVE_IMPORTANT) {
      if (rtype == 1)
      return "accomplish something that feels/is seen as important";
    }
    else if (narr == NARRATIVE_DEBT) {
      if (rtype == 1)
      return "have someone be in their debt";
    }
    else if (narr == NARRATIVE_WORLD_BETTER) {
      if (rtype == 1)
      return "make the world/some aspect of the world better";
    }
    else if (narr == NARRATIVE_WORLD_INTEREST) {
      if (rtype == 1)
      return "change the world/some aspect of the world towards their interests";
    }
    else if (narr == NARRATIVE_PERSON_MORAL) {
      if (rtype == 1)
      return "make another person morally better";
    }
    else if (narr == NARRATIVE_PERSON_CORRUPT) {
      if (rtype == 1)
      return "make another person more corrupt";
    }
    else if (narr == NARRATIVE_CHANGE_PSYCH) {
      if (rtype == 1)
      return "change another person's psychology towards their interests";
    }
    else if (narr == NARRATIVE_CHANGE_CIRCUM) {
      if (rtype == 1)
      return "change another person's circumstances towards their interests";
    }

    return "";
  }

  _DOFUN(do_narrative) {
    char arg1[MSL];
    char tmp[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if(!str_cmp(arg1, "query"))
    {
      CHAR_DATA *victim;
      if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }
      if (IS_NPC(victim)) {
        send_to_char("Not on NPCs.\n\r", ch);
        return;
      }
      send_to_char("Narrative query sent.\n\r", ch);
      printf_to_char(victim, "%s wants to exchange narrative reward information, type yes or no to respond.\n\r", PERS(ch, victim));
      victim->pcdata->narrative_query_char = ch;
      return;


    }
    if (!str_cmp(arg1, "offer")) {
      int point = atoi(argument);
      if (point <= 0 || point >= 50) {
        char buf[MSL];
        strcpy(buf, "Narrative Rewards Offered\n");
        for (int i = 0; i < 50; i++) {
          if (safe_strlen(narrative_desc(i, 1)) > 2) {
            if (ch->pcdata->narrative_give[i] == 1) {
              sprintf(tmp, "`g%02d]`x %s\n", i + 1, narrative_desc(i, 1));
            }
            else {
              sprintf(tmp, "`g%02d]`x %s\n", i + 1, narrative_desc(i, 1));
            }
            strcat(buf, tmp);
          }
        }
        page_to_char(buf, ch);
        return;
      }
      point = point - 1;
      if (ch->pcdata->narrative_give[point] == 1) {
        ch->pcdata->narrative_give[point] = 0;
        printf_to_char(ch, "You stop offering %s.\n\r", narrative_desc(point, 1));
      }
      else {
        ch->pcdata->narrative_give[point] = 1;
        printf_to_char(ch, "You start offering %s.\n\r", narrative_desc(point, 1));
      }
      return;
    }
    else if (!str_cmp(arg1, "want")) {
      int point = atoi(argument);
      if (point <= 0 || point >= 50) {
        char buf[MSL];
        strcpy(buf, "Narrative Rewards Wanted\n");
        for (int i = 0; i < 50; i++) {
          if (safe_strlen(narrative_desc(i, 1)) > 2) {
            if (ch->pcdata->narrative_want[i] == 1) {
              sprintf(tmp, "`g%02d]`x %s\n", i + 1, narrative_desc(i, 1));
            }
            else {
              sprintf(tmp, "`g%02d]`x %s\n", i + 1, narrative_desc(i, 1));
            }
            strcat(buf, tmp);
          }
        }
        page_to_char(buf, ch);
        return;
      }
      point = point - 1;
      if (ch->pcdata->narrative_want[point] == 1) {
        ch->pcdata->narrative_want[point] = 0;
        printf_to_char(ch, "You stop wanting %s.\n\r", narrative_desc(point, 1));
      }
      else {
        ch->pcdata->narrative_want[point] = 1;
        printf_to_char(ch, "You start wanting %s.\n\r", narrative_desc(point, 1));
      }
      return;
    }
    else {
      char buf[MSL];
      strcpy(buf, "Narrative Rewards Offered\n");
      for (int i = 0; i < 50; i++) {
        if (safe_strlen(narrative_desc(i, 1)) > 2) {
          if (ch->pcdata->narrative_give[i] == 1) {
            sprintf(tmp, "`g%02d]`x %s\n", i + 1, narrative_desc(i, 1));
            strcat(buf, tmp);
          }
        }
      }
      strcat(buf, "\nNarrative Rewards Wanted\n");
      for (int i = 0; i < 50; i++) {
        if (safe_strlen(narrative_desc(i, 1)) > 2) {
          if (ch->pcdata->narrative_want[i] == 1) {
            sprintf(tmp, "`g%02d]`x %s\n", i + 1, narrative_desc(i, 1));
            strcat(buf, tmp);
          }
        }
      }
      page_to_char(buf, ch);
      return;
    }
  }

  bool narrative_pair(CHAR_DATA *ch, CHAR_DATA *victim) {
    bool cfound = FALSE;
    bool vfound = FALSE;
    if(norp_match(ch, victim))
    return FALSE;

    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->narrative_give[i] == 1 && victim->pcdata->narrative_want[i] == 1)
      cfound = TRUE;
      if (ch->pcdata->narrative_want[i] == 1 && victim->pcdata->narrative_give[i] == 1)
      vfound = TRUE;
    }
    if (cfound == TRUE && vfound == TRUE)
    return TRUE;
    return FALSE;
  }

  int narrative_score(CHAR_DATA *ch, CHAR_DATA *victim) {
    int cpoints = 0;
    int vpoints = 0;
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->narrative_give[i] == 1 && victim->pcdata->narrative_want[i] == 1)
      cpoints++;
      if (ch->pcdata->narrative_want[i] == 1 && victim->pcdata->narrative_give[i] == 1)
      vpoints++;
    }
    if (cpoints > vpoints)
    return (vpoints + (int)(cpoints / 2));
    else
    return (cpoints + (int)(vpoints / 2));
  }

  char *narrative_pair_rewards(CHAR_DATA *ch, CHAR_DATA *victim) {
    char cbuf[MSL];
    char vbuf[MSL];
    strcpy(cbuf, "");
    strcpy(vbuf, "");
    int ccount = 0;
    int vcount = 0;
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->narrative_give[i] == 1 && victim->pcdata->narrative_want[i] == 1 && vcount < 3) {
        strcat(vbuf, narrative_desc(i, 1));
        strcat(vbuf, "\n");
        vcount++;
      }
      if (ch->pcdata->narrative_want[i] == 1 && victim->pcdata->narrative_give[i] == 1 && ccount < 3) {
        strcat(cbuf, narrative_desc(i, 1));
        strcat(cbuf, "\n");
        ccount++;
      }
    }
    char buf[MSL];
    sprintf(buf, "\n%s Narrative Rewards:\n%s\n%s Narrative Rewards:\n%s\r", ch->name, cbuf, victim->name, vbuf);
    return str_dup(buf);
  }

  char *narrative_show(CHAR_DATA *ch) {
    char cbuf[MSL];
    strcpy(cbuf, "");
    char vbuf[MSL];
    strcpy(vbuf, "");
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->narrative_give[i] == 1) {
        strcat(cbuf, narrative_desc(i, 1));
        strcat(cbuf, "\n");
      }
      if (ch->pcdata->narrative_want[i] == 1) {
        strcat(vbuf, narrative_desc(i, 1));
        strcat(vbuf, "\n");
      }
    }
    char buf[MSL];
    sprintf(buf, "\nSought Rewards:\n%s\nOffered Rewards:\n%s\n\r", vbuf, cbuf);
    return str_dup(buf);
  }

  int remote_power_level(CHAR_DATA *ch, CHAR_DATA *victim) {
    int trust = get_gmtrust(ch, victim);
    if (trust < 0)
    return 0;

    int llevel = light_level(victim->in_room);
    if (llevel >= 50)
    return 0;
    trust = ((100 * trust) + 300) / 300;
    int darkness = 100 - llevel;
    int power = (trust * darkness / 100);
    if (llevel > 10)
    power = UMIN(power, 90);
    // 100 is complete darkness at trust 1. Whisper at > 50, brush at > 75, // hallucinate at 100, possess at 150
    return power;
  }

  // 11 for t5
  // 5 for t3
  int doom_days(CHAR_DATA *ch) {
    double tcount = (double)ch->pcdata->account->tier_count /
    UMAX(1.0, (double)ch->pcdata->account->total_count);
    double ntcount = sqrt(tcount * tcount * tcount);
    double ocount = (double)time_info.lweek_tier / UMAX(1.0, (double)time_info.lweek_total);
    double otcount = sqrt(ocount * ocount * ocount);

    int ctier = race_table[ch->race].tier;

    int sdays = 0;
    if (ctier == 5)
    sdays = 90;
    else if (ctier == 4)
    sdays = 110;
    else if (ctier == 3)
    sdays = 170;
    else if (ctier == 2)
    sdays = 350;
    else if (ctier == 1)
    sdays = 450;

    if (!is_super(ch))
    sdays = sdays * 3 / 2;

    if (ch->race == RACE_LOCAL || ch->race == RACE_IMPORTANT || ch->race == RACE_DEPUTY || ch->race == RACE_FACULTY)
    sdays = sdays * 3 / 2;

    sdays = sdays * number_range(90, 110) / 100;

    sdays = sdays + 30;

    sdays = sdays - (ntcount * 6);
    sdays = sdays - (otcount * 3);
    return sdays;
  }

  void find_doom(CHAR_DATA *ch)
  {
    char buf[MSL];
    int doomdays = doom_days(ch);
    sprintf(buf, "3,0,%s,%d,,,", ch->name, doomdays);
    log_string(buf);
    writeLineToFile(AI_IN_FILE, str_dup(buf));
  }

  void set_doom(CHAR_DATA *ch) {
    if (is_gm(ch) || higher_power(ch))
    return;

    if(strlen(ch->pcdata->doom_desc) < 2 && ch->played/3600 >= 20 && strlen(ch->pcdata->history) > 100)
    {
      if(strlen(ch->pcdata->doom_custom) > 5)
      {
        free_string(ch->pcdata->doom_desc);
        ch->pcdata->doom_desc = str_dup(ch->pcdata->doom_custom);
        free_string(ch->pcdata->doom_custom);
        ch->pcdata->doom_custom = str_dup("");
        ch->pcdata->doom_date = current_time + (3600 * 24 * (doom_days(ch)));
      }
      else
      find_doom(ch); // create the ai job
    }
    else if(ch->played/3600 >= 60)
    ch->pcdata->doom_date = current_time + (3600 * 24 * (doom_days(ch) - 10));
  }


  _DOFUN(do_outputencounters)
  {
    char buf[MSL];
    for(int i=1;i<53;i++)
    {
      sprintf(buf, "%d, \"%s\"", i, encounter_prompt(ch, i).c_str());
      writeLineToFile("../data/elist.csv", buf);
    }
  }


  bool ff_surname(char * arg)
  {
    if(arg == NULL)
    return FALSE;
    if(!str_cmp(from_color(arg), "Arkwright"))
    return TRUE;
    if(!str_cmp(from_color(arg), "Inigo"))
    return TRUE;
    if(!str_cmp(from_color(arg), "Wilson"))
    return TRUE;
    if(!str_cmp(from_color(arg), "Salte"))
    return TRUE;
    if(!str_cmp(from_color(arg), "Moore"))
    return TRUE;
    if(!str_cmp(from_color(arg), "Reeves"))
    return TRUE;
    if(!str_cmp(from_color(arg), "Swann"))
    return TRUE;
    return FALSE;


  }

  bool is_ffamily(CHAR_DATA *ch)
  {
    if(ch == NULL || IS_NPC(ch))
    return FALSE;

    if(ff_surname(ch->pcdata->last_name))
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_family) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if(!str_cmp(arg1, "knowledge"))
    {
      string_append(ch, &ch->pcdata->ff_knowledge);
      send_to_char("You can write the information on your character known to other members of the families.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "secrets") || !str_cmp(arg1, "secret"))
    {
      if(strlen(ch->pcdata->ff_secret) > 2)
      {
        send_to_char("You already have a secret.\n\r", ch);
        return;
      }
      string_append(ch, &ch->pcdata->ff_secret);
      send_to_char("Write one or more secrets your character has that one or two other family members might know\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "roster"))
    {
      send_to_char("`rFamily`x\n\r", ch);
      for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
      it != WeeklyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if(!str_cmp((*it)->surname, ch->pcdata->last_name) && str_cmp((*it)->charname, ch->name))
        printf_to_char(ch, "%s %s\n\r", (*it)->charname, (*it)->surname);
      }

      if(is_ffamily(ch))
      {
        send_to_char("\n`rRelatives`x\n\r", ch);
        for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
        it != WeeklyVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;

          if(str_cmp((*it)->surname, ch->pcdata->last_name) && ff_surname((*it)->surname))
          printf_to_char(ch, "%s %s\n\r", (*it)->charname, (*it)->surname);
        }
      }
      return;
    }
    if(!str_cmp(arg1, "lookup"))
    {
      if(!IS_IMMORTAL(ch) && !is_ffamily(ch))
      {
        send_to_char("You are not a member of the Founding Families.\n\r", ch);
        return;
      }

      char buf[MSL];
      CHAR_DATA *victim = NULL;
      bool online = FALSE;
      DESCRIPTOR_DATA d;
      struct stat sb;
      Buffer outbuf;
      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, argument)) {
          send_to_char("Nobody like that to lookup.\n\r", ch);
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        stat(buf, &sb);
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        send_to_char("Nobody like that to lookup.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }
      if(!is_ffamily(victim))
      {
        send_to_char("Nobody like that to lookup.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }
      char string[MSL];
      strcpy(string, "");
      strcpy(buf, "");
      sprintf(buf, "%s %s, known for %s\n\r", victim->name, victim->pcdata->last_name, victim->pcdata->fame);
      strcat(string, buf);
      sprintf(buf, "Family Knowledge: %s\n\r", victim->pcdata->ff_knowledge);
      strcat(string, buf);
      for (int i = 0; i < 10; i++) {
        switch (victim->pcdata->relationship_type[i]) {
        case REL_CHILD:
          sprintf(buf, "Child of %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        case REL_PARENT:
          sprintf(buf, "Parent of %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        case REL_SPOUSE:
          sprintf(buf, "Spouse of %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        case REL_GUARDIAN:
          sprintf(buf, "Guardian of %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        case REL_WARD:
          sprintf(buf, "Ward of %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        case REL_SIBLING:
          sprintf(buf, "Sibling of %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        case REL_DATING:
          sprintf(buf, "Dating %s.\n\r", victim->pcdata->relationship[i]);
          strcat(string, buf);
          break;
        }
      }
      page_to_char(string, ch);
      return;
    }
    send_to_char("Syntax: family <knowledge|secrets|roster|lookup (name)>\n\r", ch);
    return;

  }


  void end_p_char(char * cname)
  {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(cname)) !=
        NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, cname)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(cname));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }
    if(!in_lodge(victim->in_room))
    {
      if (!online)
      free_char(victim);
      return;
    }
    int rpoint = number_range(0, MAX_TAXIS);
    ROOM_INDEX_DATA *nroom = get_room_index(taxi_table[rpoint].vnum);
    if(nroom != NULL)
    {
      char_from_room(victim);
      char_to_room(victim, nroom);
    }
    if(online)
    send_to_char("You awaken suddenly to find you have sleepwalked.\n\r", victim);
    else
    {
      sprintf(buf, "%s\nYou awaken somewhere strange, having sleepwalked in your distress.", victim->pcdata->messages);
      free_string(victim->pcdata->messages);
      victim->pcdata->messages = str_dup(buf);
    }

    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  _DOFUN(do_endprologue)
  {
    crisis_prologue = 0;
    FILE *fp;
    if ((fp = fopen("../player/playerlist", "r")) != NULL) {
      for (;;) {
        char *word = fread_word(fp);
        if (word != NULL && safe_strlen(word) > 0) {
          if (!str_cmp(word, "#END")) {
            fclose(fp);
            send_to_char("Done.\n\r", ch);
            return;
          }
          end_p_char(word);
        }
      }
      fclose(fp);
    }
    send_to_char("Done.\n\r", ch);

  }

  STORYLINE_TYPE * get_sect_storyline(FACTION_TYPE *sect)
  {
    for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
    it != StorylineVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      for(int i=0;i<10;i++)
      {
        if(!str_cmp((*it)->masterminds[i], sect->leader))
        return *it;
      }

    }

    for (vector<STORYLINE_TYPE *>::iterator it = StorylineVect.begin();
    it != StorylineVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      for(int i=0;i<10;i++)
      {
        for(int j=0;j<100;j++)
        if(!str_cmp((*it)->masterminds[i], sect->member_names[j]))
        return *it;
      }

    }
    return NULL;
  }


  _DOFUN(do_summary) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];

    if (ch->pcdata->ci_editing == 20) {
      ANNIVERSARY_TYPE *ann;
      for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
      it != AnniversaryVect.end(); ++it) {
        if ((*it)->id == ch->pcdata->ci_discipline) {
          ann = *it;
          break;
        }
      }
      if(ann != NULL)
      {
        ch->pcdata->ci_absorb = 1;
        string_append(ch, &ann->summary);
        send_to_char("Write the summary for this anniversary.\n\r", ch);

      }
      return;
    }


    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if(!str_cmp(arg2, "conflict"))
    {
      if(in_fight(ch) || in_fight(victim))
      {
        send_to_char("You or your target are in a fight.\n\r", ch);
        return;
      }
      ch->pcdata->summary_type = SUMMARY_CONFLICT_ATT;
      ch->pcdata->summary_target = victim;
      free_string(ch->pcdata->summary_name);
      ch->pcdata->summary_name = str_dup(victim->name);
      ch->pcdata->summary_when = current_time;
      ch->pcdata->summary_stage = SUM_STAGE_PENDING_THEM;
      free_string(ch->pcdata->summary_intro);
      ch->pcdata->summary_intro = str_dup("");
      free_string(ch->pcdata->summary_content);
      ch->pcdata->summary_content = str_dup("");
      free_string(ch->pcdata->summary_conclusion);
      ch->pcdata->summary_conclusion = str_dup("");
      ch->pcdata->summary_blood = 0;
      ch->pcdata->summary_intel = 0;
      ch->pcdata->summary_lifeforce = 0;
      ch->pcdata->summary_helpless = 0;
      ch->pcdata->summary_home = 0;

      victim->pcdata->summary_type = SUMMARY_CONFLICT_DEF;
      victim->pcdata->summary_target = ch;
      free_string(victim->pcdata->summary_name);
      victim->pcdata->summary_name = str_dup(ch->name);
      victim->pcdata->summary_when = current_time;
      victim->pcdata->summary_stage = SUM_STAGE_PENDING_YOU;
      free_string(victim->pcdata->summary_intro);
      victim->pcdata->summary_intro = str_dup("");
      free_string(victim->pcdata->summary_content);
      victim->pcdata->summary_content = str_dup("");
      free_string(victim->pcdata->summary_conclusion);
      victim->pcdata->summary_conclusion = str_dup("");
      victim->pcdata->summary_blood = 0;
      victim->pcdata->summary_intel = 0;
      victim->pcdata->summary_lifeforce = 0;
      victim->pcdata->summary_helpless = 0;
      victim->pcdata->summary_home = 0;
      printf_to_char(ch, "Summary of a conflict scene with %s requested.(%s)\n\r", PERS(victim, ch), argument);
      printf_to_char(victim, "%s has requested a summary of a conflict scene. Type (Yes/No)\n(%s)\n\r", PERS(ch, victim), argument);
      return;
    }
    else if(!str_cmp(arg2, "winconflict"))
    {
      if(in_fight(ch) && !same_fight(ch, victim))
      {
        send_to_char("You are in the same fight.\n\r", ch);
        return;
      }
      if(in_fight(ch))
      {
        ch->pcdata->summary_type = SUMMARY_DEFEATING_INFIGHT;
        ch->pcdata->summary_target = victim;
        free_string(ch->pcdata->summary_name);
        ch->pcdata->summary_name = str_dup(victim->name);
        ch->pcdata->summary_when = current_time;
        ch->pcdata->summary_stage = SUM_STAGE_PENDING_THEM;
        free_string(ch->pcdata->summary_intro);
        ch->pcdata->summary_intro = str_dup("");
        free_string(ch->pcdata->summary_content);
        ch->pcdata->summary_content = str_dup("");
        free_string(ch->pcdata->summary_conclusion);
        ch->pcdata->summary_conclusion = str_dup("");
        ch->pcdata->summary_blood = 0;
        ch->pcdata->summary_intel = 0;
        ch->pcdata->summary_lifeforce = 0;
        ch->pcdata->summary_helpless = 0;
        ch->pcdata->summary_home = 0;

        victim->pcdata->summary_type = SUMMARY_DEFEATED_INFIGHT;
        victim->pcdata->summary_target = ch;
        free_string(victim->pcdata->summary_name);
        victim->pcdata->summary_name = str_dup(ch->name);
        victim->pcdata->summary_when = current_time;
        victim->pcdata->summary_stage = SUM_STAGE_PENDING_YOU;
        free_string(victim->pcdata->summary_intro);
        victim->pcdata->summary_intro = str_dup("");
        free_string(victim->pcdata->summary_content);
        victim->pcdata->summary_content = str_dup("");
        free_string(victim->pcdata->summary_conclusion);
        victim->pcdata->summary_conclusion = str_dup("");
        victim->pcdata->summary_blood = 0;
        victim->pcdata->summary_intel = 0;
        victim->pcdata->summary_lifeforce = 0;
        victim->pcdata->summary_helpless = 0;
        victim->pcdata->summary_home = 0;
        printf_to_char(ch, "Summary of a conflict scene in which you defeat %s requested.(%s)\n\r", PERS(victim, ch), argument);
        printf_to_char(victim, "%s has requested a summary of a conflict scene in which they defeat you. Type (Yes/No)\n(%s)\n\r", PERS(ch, victim), argument);
        return;
      }
      else
      {
        ch->pcdata->summary_type = SUMMARY_DEFEATING_ATT;
        ch->pcdata->summary_target = victim;
        free_string(ch->pcdata->summary_name);
        ch->pcdata->summary_name = str_dup(victim->name);
        ch->pcdata->summary_when = current_time;
        ch->pcdata->summary_stage = SUM_STAGE_PENDING_THEM;
        free_string(ch->pcdata->summary_intro);
        ch->pcdata->summary_intro = str_dup("");
        free_string(ch->pcdata->summary_content);
        ch->pcdata->summary_content = str_dup("");
        free_string(ch->pcdata->summary_conclusion);
        ch->pcdata->summary_conclusion = str_dup("");
        ch->pcdata->summary_blood = 0;
        ch->pcdata->summary_intel = 0;
        ch->pcdata->summary_lifeforce = 0;
        ch->pcdata->summary_helpless = 0;
        ch->pcdata->summary_home = 0;

        victim->pcdata->summary_type = SUMMARY_DEFEATED_DEF;
        victim->pcdata->summary_target = ch;
        free_string(victim->pcdata->summary_name);
        victim->pcdata->summary_name = str_dup(ch->name);
        victim->pcdata->summary_when = current_time;
        victim->pcdata->summary_stage = SUM_STAGE_PENDING_YOU;
        free_string(victim->pcdata->summary_intro);
        victim->pcdata->summary_intro = str_dup("");
        free_string(victim->pcdata->summary_content);
        victim->pcdata->summary_content = str_dup("");
        free_string(victim->pcdata->summary_conclusion);
        victim->pcdata->summary_conclusion = str_dup("");
        victim->pcdata->summary_blood = 0;
        victim->pcdata->summary_intel = 0;
        victim->pcdata->summary_lifeforce = 0;
        victim->pcdata->summary_helpless = 0;
        victim->pcdata->summary_home = 0;
        printf_to_char(ch, "Summary of a conflict scene in which you defeat %s requested.(%s)\n\r", PERS(victim, ch), argument);
        printf_to_char(victim, "%s has requested a summary of a conflict scene in which they defeat you. Type (Yes/No)\n(%s)\n\r", PERS(ch, victim), argument);
        return;
      }

    }
    else if(!str_cmp(arg2, "loseconflict"))
    {
      if(in_fight(ch) && !same_fight(ch, victim))
      {
        send_to_char("You are not in the same fight.\n\r", ch);
        return;
      }
      if(in_fight(ch))
      {
        ch->pcdata->summary_type = SUMMARY_DEFEATED_INFIGHT;
        ch->pcdata->summary_target = victim;
        free_string(ch->pcdata->summary_name);
        ch->pcdata->summary_name = str_dup(victim->name);
        ch->pcdata->summary_when = current_time;
        ch->pcdata->summary_stage = SUM_STAGE_CONTENT;
        free_string(ch->pcdata->summary_intro);
        ch->pcdata->summary_intro = str_dup("");
        free_string(ch->pcdata->summary_content);
        ch->pcdata->summary_content = str_dup("");
        free_string(ch->pcdata->summary_conclusion);
        ch->pcdata->summary_conclusion = str_dup("");
        ch->pcdata->summary_blood = 0;
        ch->pcdata->summary_intel = 0;
        ch->pcdata->summary_lifeforce = 0;
        ch->pcdata->summary_helpless = 0;
        ch->pcdata->summary_home = 0;

        victim->pcdata->summary_type = SUMMARY_DEFEATING_INFIGHT;
        victim->pcdata->summary_target = ch;
        free_string(victim->pcdata->summary_name);
        victim->pcdata->summary_name = str_dup(ch->name);
        victim->pcdata->summary_when = current_time;
        victim->pcdata->summary_stage = SUM_STAGE_CONTENT;
        free_string(victim->pcdata->summary_intro);
        victim->pcdata->summary_intro = str_dup("");
        free_string(victim->pcdata->summary_content);
        victim->pcdata->summary_content = str_dup("");
        free_string(victim->pcdata->summary_conclusion);
        victim->pcdata->summary_conclusion = str_dup("");
        victim->pcdata->summary_blood = 0;
        victim->pcdata->summary_intel = 0;
        victim->pcdata->summary_lifeforce = 0;
        victim->pcdata->summary_helpless = 0;
        victim->pcdata->summary_home = 0;
        printf_to_char(ch, "Summary of a conflict scene in which %s defeats you started, use content (emote text) to write your actions in the conflict.(%s)\n\r", PERS(victim, ch), argument);
        printf_to_char(victim, "%s has requested a summary of a conflict scene in which you defeat them. Type content (emote text) to write your actions in the conflict.\n(%s)\n\r", PERS(ch, victim), argument);
        return;
      }
      else
      {
        ch->pcdata->summary_type = SUMMARY_DEFEATED_ATT;
        ch->pcdata->summary_target = victim;
        free_string(ch->pcdata->summary_name);
        ch->pcdata->summary_name = str_dup(victim->name);
        ch->pcdata->summary_when = current_time;
        ch->pcdata->summary_stage = SUM_STAGE_PENDING_THEM;
        free_string(ch->pcdata->summary_intro);
        ch->pcdata->summary_intro = str_dup("");
        free_string(ch->pcdata->summary_content);
        ch->pcdata->summary_content = str_dup("");
        free_string(ch->pcdata->summary_conclusion);
        ch->pcdata->summary_conclusion = str_dup("");
        ch->pcdata->summary_blood = 0;
        ch->pcdata->summary_intel = 0;
        ch->pcdata->summary_lifeforce = 0;
        ch->pcdata->summary_helpless = 0;
        ch->pcdata->summary_home = 0;

        victim->pcdata->summary_type = SUMMARY_DEFEATING_DEF;
        victim->pcdata->summary_target = ch;
        free_string(victim->pcdata->summary_name);
        victim->pcdata->summary_name = str_dup(ch->name);
        victim->pcdata->summary_when = current_time;
        victim->pcdata->summary_stage = SUM_STAGE_PENDING_YOU;
        free_string(victim->pcdata->summary_intro);
        victim->pcdata->summary_intro = str_dup("");
        free_string(victim->pcdata->summary_content);
        victim->pcdata->summary_content = str_dup("");
        free_string(victim->pcdata->summary_conclusion);
        victim->pcdata->summary_conclusion = str_dup("");
        victim->pcdata->summary_blood = 0;
        victim->pcdata->summary_intel = 0;
        victim->pcdata->summary_lifeforce = 0;
        victim->pcdata->summary_helpless = 0;
        victim->pcdata->summary_home = 0;
        printf_to_char(ch, "Summary of a conflict scene in which %s defeats you requested.(%s)\n\r", PERS(victim, ch), argument);
        printf_to_char(victim, "%s has requested a summary of a conflict scene in which you defeat them. Type (Yes/No)\n(%s)\n\r", PERS(ch, victim), argument);
        return;
      }
    }
    else if(!str_cmp(arg2, "captive"))
    {
      if(is_helpless(ch) == is_helpless(victim))
      {
        send_to_char("This only works if one of you is helpless and the other isn't.\n\r", ch);
        return;
      }
      if(is_helpless(victim))
      {
        ch->pcdata->summary_type = SUMMARY_VICTIMIZER;
        ch->pcdata->summary_target = victim;
        free_string(ch->pcdata->summary_name);
        ch->pcdata->summary_name = str_dup(victim->name);
        ch->pcdata->summary_when = current_time;
        ch->pcdata->summary_stage = SUM_STAGE_INTRO;
        free_string(ch->pcdata->summary_intro);
        ch->pcdata->summary_intro = str_dup("");
        free_string(ch->pcdata->summary_content);
        ch->pcdata->summary_content = str_dup("");
        free_string(ch->pcdata->summary_conclusion);
        ch->pcdata->summary_conclusion = str_dup("");
        ch->pcdata->summary_blood = 0;
        ch->pcdata->summary_intel = 0;
        ch->pcdata->summary_lifeforce = 0;
        ch->pcdata->summary_helpless = 0;
        ch->pcdata->summary_home = 0;


        victim->pcdata->summary_type = SUMMARY_VICTIM;
        victim->pcdata->summary_target = ch;
        free_string(victim->pcdata->summary_name);
        victim->pcdata->summary_name = str_dup(ch->name);
        victim->pcdata->summary_when = current_time;
        victim->pcdata->summary_stage = SUM_STAGE_WAITING;
        free_string(victim->pcdata->summary_intro);
        victim->pcdata->summary_intro = str_dup("");
        free_string(victim->pcdata->summary_content);
        victim->pcdata->summary_content = str_dup("");
        free_string(victim->pcdata->summary_conclusion);
        victim->pcdata->summary_conclusion = str_dup("");
        victim->pcdata->summary_blood = 0;
        victim->pcdata->summary_intel = 0;
        victim->pcdata->summary_lifeforce = 0;
        victim->pcdata->summary_helpless = 0;
        victim->pcdata->summary_home = 0;
        printf_to_char(ch, "You start the summary of a scene with %s as your captive, begin by typing your introduction with intro (emote text)", PERS(victim, ch));
        printf_to_char(victim, "%s has started a summary of a scene with you as their captive.", PERS(ch, victim));
        return;
      }
      else if(is_helpless(ch))
      {
        ch->pcdata->summary_type = SUMMARY_VICTIM;
        ch->pcdata->summary_target = victim;
        free_string(ch->pcdata->summary_name);
        ch->pcdata->summary_name = str_dup(victim->name);
        ch->pcdata->summary_when = current_time;
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        free_string(ch->pcdata->summary_intro);
        ch->pcdata->summary_intro = str_dup("");
        free_string(ch->pcdata->summary_content);
        ch->pcdata->summary_content = str_dup("");
        free_string(ch->pcdata->summary_conclusion);
        ch->pcdata->summary_conclusion = str_dup("");
        ch->pcdata->summary_blood = 0;
        ch->pcdata->summary_intel = 0;
        ch->pcdata->summary_lifeforce = 0;
        ch->pcdata->summary_helpless = 0;
        ch->pcdata->summary_home = 0;


        victim->pcdata->summary_type = SUMMARY_VICTIMIZER;
        victim->pcdata->summary_target = ch;
        free_string(victim->pcdata->summary_name);
        victim->pcdata->summary_name = str_dup(ch->name);
        victim->pcdata->summary_when = current_time;
        victim->pcdata->summary_stage = SUM_STAGE_INTRO;
        free_string(victim->pcdata->summary_intro);
        victim->pcdata->summary_intro = str_dup("");
        free_string(victim->pcdata->summary_content);
        victim->pcdata->summary_content = str_dup("");
        free_string(victim->pcdata->summary_conclusion);
        victim->pcdata->summary_conclusion = str_dup("");
        victim->pcdata->summary_blood = 0;
        victim->pcdata->summary_intel = 0;
        victim->pcdata->summary_lifeforce = 0;
        victim->pcdata->summary_helpless = 0;
        victim->pcdata->summary_home = 0;
        printf_to_char(ch, "You start the summary of a scene with %s as your captor.", PERS(victim, ch));
        printf_to_char(victim, "%s has started a summary of a scene with you as their captor, begin by typing your introduction with intro (emote text).", PERS(ch, victim));
        return;
      }
    }
    else if(!str_cmp(arg2, "wrapup"))
    {
      ch->pcdata->summary_type = SUMMARY_WRAPUP;
      ch->pcdata->summary_target = victim;
      free_string(ch->pcdata->summary_name);
      ch->pcdata->summary_name = str_dup(victim->name);
      ch->pcdata->summary_when = current_time;
      ch->pcdata->summary_stage = SUM_STAGE_WAITING;
      free_string(ch->pcdata->summary_intro);
      ch->pcdata->summary_intro = str_dup("");
      free_string(ch->pcdata->summary_content);
      ch->pcdata->summary_content = str_dup("");
      free_string(ch->pcdata->summary_conclusion);
      ch->pcdata->summary_conclusion = str_dup("");
      ch->pcdata->summary_blood = 0;
      ch->pcdata->summary_intel = 0;
      ch->pcdata->summary_lifeforce = 0;
      ch->pcdata->summary_helpless = 0;
      ch->pcdata->summary_home = 0;


      victim->pcdata->summary_type = SUMMARY_WRAPUP;
      victim->pcdata->summary_target = ch;
      free_string(victim->pcdata->summary_name);
      victim->pcdata->summary_name = str_dup(ch->name);
      victim->pcdata->summary_when = current_time;
      victim->pcdata->summary_stage = SUM_STAGE_PENDING_YOU;
      free_string(victim->pcdata->summary_intro);
      victim->pcdata->summary_intro = str_dup("");
      free_string(victim->pcdata->summary_content);
      victim->pcdata->summary_content = str_dup("");
      free_string(victim->pcdata->summary_conclusion);
      victim->pcdata->summary_conclusion = str_dup("");
      victim->pcdata->summary_blood = 0;
      victim->pcdata->summary_intel = 0;
      victim->pcdata->summary_lifeforce = 0;
      victim->pcdata->summary_helpless = 0;
      victim->pcdata->summary_home = 0;
      printf_to_char(ch, "Summary wrapup with %s  requested.(%s)\n\r", PERS(victim, ch), argument);
      printf_to_char(victim, "%s has requested a summary wrap up with you. Type (Yes/No)\n(%s)\n\r", PERS(ch, victim), argument);
      return;

    }
    else if(!str_cmp(arg2, "other"))
    {
      ch->pcdata->summary_type = SUMMARY_OTHER;
      ch->pcdata->summary_target = victim;
      free_string(ch->pcdata->summary_name);
      ch->pcdata->summary_name = str_dup(victim->name);
      ch->pcdata->summary_when = current_time;
      ch->pcdata->summary_stage = SUM_STAGE_WAITING;
      free_string(ch->pcdata->summary_intro);
      ch->pcdata->summary_intro = str_dup("");
      free_string(ch->pcdata->summary_content);
      ch->pcdata->summary_content = str_dup("");
      free_string(ch->pcdata->summary_conclusion);
      ch->pcdata->summary_conclusion = str_dup("");
      ch->pcdata->summary_blood = 0;
      ch->pcdata->summary_intel = 0;
      ch->pcdata->summary_lifeforce = 0;
      ch->pcdata->summary_helpless = 0;
      ch->pcdata->summary_home = 0;


      victim->pcdata->summary_type = SUMMARY_OTHER;
      victim->pcdata->summary_target = ch;
      free_string(victim->pcdata->summary_name);
      victim->pcdata->summary_name = str_dup(ch->name);
      victim->pcdata->summary_when = current_time;
      victim->pcdata->summary_stage = SUM_STAGE_PENDING_YOU;
      free_string(victim->pcdata->summary_intro);
      victim->pcdata->summary_intro = str_dup("");
      free_string(victim->pcdata->summary_content);
      victim->pcdata->summary_content = str_dup("");
      free_string(victim->pcdata->summary_conclusion);
      victim->pcdata->summary_conclusion = str_dup("");
      victim->pcdata->summary_blood = 0;
      victim->pcdata->summary_intel = 0;
      victim->pcdata->summary_lifeforce = 0;
      victim->pcdata->summary_helpless = 0;
      victim->pcdata->summary_home = 0;
      printf_to_char(ch, "Summary of a general scene with %s  requested.(%s)\n\r", PERS(victim, ch), argument);
      printf_to_char(victim, "%s has requested a general summary scene with you. Type (Yes/No)\n(%s)\n\r", PERS(ch, victim), argument);
      return;


    }
    else
    {
      send_to_char("Syntax: summary <name> <conflict|captive|win|lose|wrapup|other> <additional message>\n\r", ch);
      return;
    }


  }

  _DOFUN(do_content)
  {
    if(safe_strlen(argument) < 5)
    {
      send_to_char("You must provide a longer description.\n\r", ch);
      return;
    }
    if(ch->pcdata->summary_stage == SUM_STAGE_CONTENT)
    {
      CHAR_DATA *target = ch->pcdata->summary_target;
      if(target == NULL)
      {
        return;
      }
      free_string(ch->pcdata->summary_content);
      ch->pcdata->summary_content = str_dup(argument);
      if(ch->pcdata->summary_type == SUMMARY_CONFLICT_ATT || ch->pcdata->summary_type == SUMMARY_CONFLICT_DEF)
      {
        if(safe_strlen(target->pcdata->summary_content) > 3)
        {
          process_emote(target, NULL, EMOTE_EMOTE, target->pcdata->summary_content);
          process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
          ch->pcdata->summary_stage = SUM_STAGE_CHANCE;
          target->pcdata->summary_stage = SUM_STAGE_CHANCE;
          send_to_char("Based on the RP enter the chance you think you would win this conflict as a percentage from 0 to 100 by typing chance (number)\n\r", ch);
          send_to_char("Based on the RP enter the chance you think you would win this conflict as a percentage from 0 to 100 by typing chance (number)\n\r", target);
          return;
        }
        else
        send_to_char("Done, waiting on the other person.\n\r", ch);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_DEFEATING_INFIGHT || ch->pcdata->summary_type == SUMMARY_DEFEATING_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATING_DEF)
      {
        if(safe_strlen(target->pcdata->summary_content) > 3)
        {
          process_emote(target, NULL, EMOTE_EMOTE, target->pcdata->summary_content);
          process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
          ch->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
          target->pcdata->summary_stage = SUM_STAGE_WAITING;
          send_to_char("You have won the conflict. Write your conclusion with conclusion (emote text)\n\r", ch);
        }
        else
        send_to_char("Done, waiting on the other person.\n\r", ch);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_OTHER || ch->pcdata->summary_type == SUMMARY_WRAPUP)
      {
        if(safe_strlen(target->pcdata->summary_content) > 3)
        {
          process_emote(target, NULL, EMOTE_EMOTE, target->pcdata->summary_content);
          process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
          ch->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
          target->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
          send_to_char("You can now write your conclusion, use conclusion (emote text) to do so.\n\r", ch);
          send_to_char("You can now write your conclusion, use conclusion (emote text) to do so.\n\r", target);
        }
        else
        send_to_char("Done, waiting on the other person.\n\r", ch);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_DEFEATED_INFIGHT || ch->pcdata->summary_type == SUMMARY_DEFEATED_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATED_DEF)
      {
        if(safe_strlen(target->pcdata->summary_content) > 3)
        {
          process_emote(target, NULL, EMOTE_EMOTE, target->pcdata->summary_content);
          process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
          target->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
          ch->pcdata->summary_stage = SUM_STAGE_WAITING;
          send_to_char("You have won the conflict. Write your conclusion with conclusion (emote text)\n\r", target);
        }
        else
        send_to_char("Done, waiting on the other person.\n\r", ch);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIMIZER)
      {
        if(ch->pcdata->summary_blood <= 0)
        {
          send_to_char("Content set, to decide if you wish to also take blood use blood (yes/no)\n\r", ch);
          return;
        }
        process_emote(ch, NULL, EMOTE_EMOTE, argument);
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        target->pcdata->summary_stage = SUM_STAGE_CONTENT;
        send_to_char("You can now write your own content pose in response, use content (emote text) to do so. You can also set how much life force you think would be taken from you if your captor is feeding from your psychic distress, use lifeforce (1-100) to set this. You can also set if you would give up intel, use intel (yes/no) to set this.\n\r", target);
        return;
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIM)
      {
        if(ch->pcdata->summary_intel <= 0)
        {
          send_to_char("Content set, to decide if your character would also give up intel use intel (yes/no)\n\r", ch);
          return;
        }
        if(ch->pcdata->summary_lifeforce <= 0)
        {
          send_to_char("Content set, to decide how much life force you think would be taken from you if your captor is feeding from your psychic distress, use lifeforce (1-100) to set this.\n\r", ch);
        }
        process_emote(ch, NULL, EMOTE_EMOTE, argument);
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        target->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
        send_to_char("You can now write the conclusion as well as set if you'd like the captive to be automatically sent home afterwards. Use conclusion (emote text) and home (yes/no) to set this..\n\r", target);
        return;
      }
    }
  }

  _DOFUN(do_intel)
  {
    if(ch->pcdata->summary_type == SUMMARY_VICTIM)
    {
      CHAR_DATA *target = ch->pcdata->summary_target;
      if(target == NULL)
      return;
      if(!str_cmp(argument, "yes"))
      {
        ch->pcdata->summary_intel = 1;
        if(ch->pcdata->summary_lifeforce <= 0)
        {
          send_to_char("You will give up intel, to decide how much life force you think would be taken from you if your captor is feeding from your psychic distress, use lifeforce (1-100) to set this.\n\r", ch);
        }
        if(safe_strlen(ch->pcdata->summary_content) < 2)
        {
          send_to_char("You will give up intel, use content (emote text) to write the content of your summary.\n\r", ch);
          return;
        }
        process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        target->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
        send_to_char("You can now write the conclusion as well as set if you'd like the captive to be automatically sent home afterwards. Use conclusion (emote text) and home (yes/no) to set this..\n\r", target);
        return;

      }
      else if(!str_cmp(argument, "no"))
      {
        ch->pcdata->summary_intel = 2;
        if(safe_strlen(ch->pcdata->summary_content) < 2)
        {
          send_to_char("You will not give up intel, use content (emote text) to write the content of your summary.\n\r", ch);
          return;
        }
        if(ch->pcdata->summary_lifeforce <= 0)
        {
          send_to_char("You will not give up intel, to decide how much life force you think would be taken from you if your captor is feeding from your psychic distress, use lifeforce (1-100) to set this.\n\r", ch);
        }
        process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
        ch->pcdata->summary_stage = SUM_STAGE_WAITING;
        target->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
        send_to_char("You can now write the conclusion as well as set if you'd like the captive to be automatically sent home afterwards. Use conclusion (emote text) and home (yes/no) to set this..\n\r", target);
        return;
      }
    }

  }



  _DOFUN(do_lifeforce)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    int number = atoi(arg1);
    if(number < 1 || number > 100)
    {
      send_to_char("You must enter a number between 1 and 100.\n\r", ch);
      return;
    }
    if(ch->pcdata->summary_type == SUMMARY_VICTIM)
    {
      CHAR_DATA *target = ch->pcdata->summary_target;
      if(target == NULL)
      return;

      ch->pcdata->summary_lifeforce = number;
      if(ch->pcdata->summary_intel <= 0)
      {
        send_to_char("Lifeforce set, to decide if your character will also give up intel use intel (yes/no).\n\r", ch);
      }
      if(safe_strlen(ch->pcdata->summary_content) < 2)
      {
        send_to_char("Lifeforce set, use content (emote text) to write the content of your summary.\n\r", ch);
        return;
      }
      process_emote(ch, NULL, EMOTE_EMOTE, ch->pcdata->summary_content);
      ch->pcdata->summary_stage = SUM_STAGE_WAITING;
      target->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
      send_to_char("You can now write the conclusion as well as set if you'd like the captive to be automatically sent home afterwards. Use conclusion (emote text) and home (yes/no) to set this..\n\r", target);
      return;
    }

  }

  _DOFUN(do_chance)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    int number = atoi(arg1);
    if(number < 0 || number > 100)
    {
      send_to_char("You must enter a number between 0 and 100.\n\r", ch);
      return;
    }
    CHAR_DATA *target = ch->pcdata->summary_target;
    if(target == NULL)
    {
      return;
    }

    if(ch->pcdata->summary_type == SUMMARY_CONFLICT_ATT || ch->pcdata->summary_type == SUMMARY_CONFLICT_DEF)
    {
      ch->pcdata->summary_helpless = number+1;
      if(target->pcdata->summary_helpless > 0)
      {
        int tnumber = target->pcdata->summary_helpless-1;
        bool cwin = FALSE;
        if((number == 100 && tnumber < 100) || (tnumber == 0 && number > 0))
        cwin = TRUE;
        int cpower = number * number / 10 * get_lvl(ch)/10;
        int tpower = tnumber * tnumber / 10 * get_lvl(target)/10;
        if(number_range(1, cpower) > number_range(1, tpower))
        cwin = TRUE;
        if((tnumber == 100 && number < 100) || (number == 0 && tnumber > 0))
        cwin = FALSE;
        if(cwin == TRUE)
        {
          ch->pcdata->summary_stage = SUM_STAGE_CONCLUSION;
          target->pcdata->summary_stage = SUM_STAGE_WAITING;
          send_to_char("You have won the conflict. Write your conclusion with conclusion (emote text)\n\r", ch);
        }
      }
    }
  }

  void summary_prompt(CHAR_DATA *ch)
  {
    if(ch->pcdata->summary_type > 0)
    {
      if(ch->pcdata->summary_when < current_time - (3600*2))
      {
        ch->pcdata->summary_type = 0;
        return;
      }
      if(ch->pcdata->summary_target == NULL)
      {
        CHAR_DATA *target = get_char_world_pc(ch->pcdata->summary_name);
        if(target == NULL)
        {
          ch->pcdata->summary_type = 0;
          return;
        }
        ch->pcdata->summary_target = target;
      }
      if(ch->pcdata->summary_type == SUMMARY_CONFLICT_ATT || ch->pcdata->summary_type == SUMMARY_CONFLICT_DEF || ch->pcdata->summary_type == SUMMARY_DEFEATING_INFIGHT || ch->pcdata->summary_type == SUMMARY_DEFEATED_INFIGHT || ch->pcdata->summary_type == SUMMARY_DEFEATING_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATING_DEF || ch->pcdata->summary_type == SUMMARY_DEFEATED_ATT || ch->pcdata->summary_type == SUMMARY_DEFEATED_DEF || ch->pcdata->summary_type == SUMMARY_OTHER || ch->pcdata->summary_type == SUMMARY_WRAPUP)
      {
        if(ch->pcdata->summary_stage == SUM_STAGE_INTRO && safe_strlen(ch->pcdata->summary_intro) < 2)
        {
          send_to_char("Intro: \n\r", ch);
        }
        else if(ch->pcdata->summary_stage == SUM_STAGE_CONTENT && safe_strlen(ch->pcdata->summary_content) < 2)
        {
          send_to_char("Content: \n\r", ch);
        }
        else if(ch->pcdata->summary_stage == SUM_STAGE_CONCLUSION && safe_strlen(ch->pcdata->summary_conclusion) < 2)
        {
          send_to_char("Conclusion: \n\r", ch);
        }
        else if(ch->pcdata->summary_stage == SUM_STAGE_CHANCE && ch->pcdata->summary_helpless == 0)
        {
          send_to_char("Chance: \n\r", ch);
        }
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIM)
      {
        if(ch->pcdata->summary_stage == SUM_STAGE_INTRO && safe_strlen(ch->pcdata->summary_intro) < 2)
        {
          send_to_char("Intro: \n\r", ch);
        }
        if(ch->pcdata->summary_stage == SUM_STAGE_CONTENT)
        {
          bool found = FALSE;
          if(safe_strlen(ch->pcdata->summary_content) < 2)
          {
            found = TRUE;
            send_to_char("Content: ", ch);
          }
          if(ch->pcdata->summary_intel == 0)
          {
            found = TRUE;
            send_to_char("Intel: ", ch);
          }
          if(ch->pcdata->summary_lifeforce == 0)
          {
            found = TRUE;
            send_to_char("Lifeforce: ", ch);
          }
          if(found == TRUE)
          send_to_char("\n\r", ch);
        }
      }
      if(ch->pcdata->summary_type == SUMMARY_VICTIMIZER)
      {
        if(ch->pcdata->summary_stage == SUM_STAGE_INTRO && safe_strlen(ch->pcdata->summary_intro) < 2)
        {
          send_to_char("Intro: \n\r", ch);
        }
        if(ch->pcdata->summary_stage == SUM_STAGE_CONTENT)
        {
          bool found = FALSE;
          if(safe_strlen(ch->pcdata->summary_content) < 2)
          {
            found = TRUE;
            send_to_char("Content: ", ch);
          }
          if(ch->pcdata->summary_blood == 0)
          {
            found = TRUE;
            send_to_char("Blood: ", ch);
          }
          if(found == TRUE)
          send_to_char("\n\r", ch);
        }
        if(ch->pcdata->summary_stage == SUM_STAGE_CONCLUSION)
        {
          bool found = FALSE;
          if(safe_strlen(ch->pcdata->summary_conclusion) < 2)
          {
            found = TRUE;
            send_to_char("Conclusion: ", ch);
          }
          if(ch->pcdata->summary_home == 0)
          {
            found = TRUE;
            send_to_char("Home: ", ch);
          }
          if(found == TRUE)
          send_to_char("\n\r", ch);
        }
      }


    }


  }


#if defined(__cplusplus)
}
#endif

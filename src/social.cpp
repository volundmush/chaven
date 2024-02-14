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
#include <random>

#if defined(__cplusplus)
extern "C" {
#endif

  /*
struct grouptext_type
{
char * tname;
int last_msg;
char *history;
int pnumber[10];
bool valid;
};
*/

  vector<GROUPTEXT_TYPE *> GTextVect;

  GROUPTEXT_TYPE *nullgtext;

  void save_grouptexts() {
    FILE *fpout;
    if ((fpout = fopen(GROUPTEXT_FILE, "w")) == NULL) {
      bug("Cannot open grouptext.txt for writing", 0);
      return;
    }
    for (vector<GROUPTEXT_TYPE *>::iterator it = GTextVect.begin();
    it != GTextVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if((*it)->last_msg > 0 && (*it)->last_msg < current_time - (3600*24*90))
      continue;

      fprintf(fpout, "#GROUPTEXT\n");
      fprintf(fpout, "TName %s~\n", (*it)->tname);
      fprintf(fpout, "LastMsg %d\n", (*it)->last_msg);
      for(int i=0;i<10;i++)
      {
        fprintf(fpout, "Number %d %d\n", i, (*it)->pnumber[i]);
      }
      fprintf(fpout, "History %s~\n", (*it)->history);
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_grouptext(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    GROUPTEXT_TYPE *gtext;

    gtext = new_grouptext();

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
          if (!gtext->tname) {
            bug("Fread_grouptext: Text not found.", 0);
            return;
          }
          GTextVect.push_back(gtext);
          return;
        }
        break;
      case 'H':
        KEY("History", gtext->history, fread_string(fp));
        break;
      case 'L':
        KEY("LastMsg", gtext->last_msg, fread_number(fp));
        break;
      case 'N':
        if(!str_cmp(word, "Number")) {
          int num = fread_number(fp);
          int val = fread_number(fp);
          gtext->pnumber[num] = val;
          fMatch = TRUE;
        }
        break;
      case 'T':
        KEY("TName", gtext->tname, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_grouptext: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_grouptexts() {
    nullgtext = new_grouptext();
    FILE *fp;

    if ((fp = fopen(GROUPTEXT_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Grouptexts: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "GROUPTEXT")) {
          fread_grouptext(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Grouptext: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open grouptext.txt", 0);
      exit(0);
    }
  }

  /*
struct texthistory_type
{
char * name_one;
char * name_two;
int last_msg;
char *history;
bool valid;
};
*/

  vector<TEXTHISTORY_TYPE *> HTextVect;

  TEXTHISTORY_TYPE *nullhtext;

  void save_texthistories() {

    FILE *fpout;
    if ((fpout = fopen(TEXTHISTORY_FILE, "w")) == NULL) {
      bug("Cannot open texthistory.txt for writing", 0);
      return;
    }
    for (vector<TEXTHISTORY_TYPE *>::iterator it = HTextVect.begin();
    it != HTextVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if((*it)->last_msg > 0 && (*it)->last_msg < current_time - (3600*24*90))
      continue;

      fprintf(fpout, "#TEXTHISTORY\n");
      fprintf(fpout, "NameOne %s~\n", (*it)->name_one);
      fprintf(fpout, "NameTwo %s~\n", (*it)->name_two);
      fprintf(fpout, "LastMsg %d\n", (*it)->last_msg);
      fprintf(fpout, "History %s~\n", (*it)->history);
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);

  }

  void fread_texthistory(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    TEXTHISTORY_TYPE *htext;

    htext = new_texthistory();

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
          if (!htext->name_one) {
            bug("Fread_texthistory: Text not found.", 0);
            return;
          }
          HTextVect.push_back(htext);
          return;
        }
        break;
      case 'H':
        KEY("History", htext->history, fread_string(fp));
        break;
      case 'L':
        KEY("LastMsg", htext->last_msg, fread_number(fp));
        break;
      case 'N':
        KEY("NameOne", htext->name_one, fread_string(fp));
        KEY("NameTwo", htext->name_two, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_texthistory: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_texthistories() {
    nullhtext = new_texthistory();
    FILE *fp;

    if ((fp = fopen(TEXTHISTORY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Texthistories: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "TEXTHISTORY")) {
          fread_texthistory(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Texthistory: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open texthistory.txt", 0);
      exit(0);
    }
  }

  /*
struct profile_type
{
char * name;
char * handle;
char * profile;
char * photo;
char * quote;
char * joke;
char * prefs;
int last_active;
int core_symbol;
int sect_symbol;
int cult_symbol;
int wealth;
int education;
int rating;
int rating_boost;
int rating_count;
int plus;
int premium;
int party_total;
int party_count;
int event_total;
int event_count;
int perf_total;
int perf_count;
int party_boost;
int influencer;
bool valid;
};
*/

  vector<PROFILE_TYPE *> ProfileVect;

  PROFILE_TYPE *nullprofile;

  void save_profiles() {
    FILE *fpout;
    if ((fpout = fopen(PROFILE_FILE, "w")) == NULL) {
      bug("Cannot open profile.txt for writing", 0);
      return;
    }
    for (vector<PROFILE_TYPE *>::iterator it = ProfileVect.begin();
    it != ProfileVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if((*it)->last_active > 0 && (*it)->last_active < current_time - (3600*24*90))
      continue;
      fprintf(fpout, "#PROFILE\n");
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Handle %s~\n", trimwhitespace((*it)->handle));
      fprintf(fpout, "DisplayHandle %s~\n", (*it)->display_handle);
      fprintf(fpout, "Profile %s~\n", (*it)->profile);
      fprintf(fpout, "Photo %s~\n", (*it)->photo);
      fprintf(fpout, "Quote %s~\n", (*it)->quote);
      fprintf(fpout, "Joke %s~\n", (*it)->joke);
      fprintf(fpout, "Prefs %s~\n", (*it)->prefs);
      fprintf(fpout, "LastActive %d\n", (*it)->last_active);
      fprintf(fpout, "LastLogon %d\n", (*it)->last_logon);
      fprintf(fpout, "CoreSymbol %d\n", (*it)->core_symbol);
      fprintf(fpout, "SectSymbol %d\n", (*it)->sect_symbol);
      fprintf(fpout, "CultSymbol %d\n", (*it)->cult_symbol);
      fprintf(fpout, "Wealth %d\n", (*it)->wealth);
      fprintf(fpout, "Education %d\n", (*it)->education);
      fprintf(fpout, "Rating %d\n", (*it)->rating);
      fprintf(fpout, "RatingBoost %d\n", (*it)->rating_boost);
      fprintf(fpout, "RatingCount %d\n", (*it)->rating_count);
      fprintf(fpout, "Plus %d\n", (*it)->plus);
      fprintf(fpout, "Premium %d\n", (*it)->premium);
      fprintf(fpout, "PartyTotal %d\n", (*it)->party_total);
      fprintf(fpout, "PartyCount %d\n", (*it)->party_count);
      fprintf(fpout, "EventTotal %d\n", (*it)->event_total);
      fprintf(fpout, "EventCount %d\n", (*it)->event_count);
      fprintf(fpout, "PerfTotal %d\n", (*it)->perf_total);
      fprintf(fpout, "PerfCount %d\n", (*it)->perf_count);
      fprintf(fpout, "PartyBoost %d\n", (*it)->party_boost);
      fprintf(fpout, "HostRating %d\n", (*it)->host_rating);
      fprintf(fpout, "Influencer %d\n", (*it)->influencer);
      fprintf(fpout, "GRatingTotal %d\n", (*it)->grating_total);
      fprintf(fpout, "GRatingCount %d\n", (*it)->grating_count);
      fprintf(fpout, "PRatingTotal %d\n", (*it)->prating_total);
      fprintf(fpout, "PRatingCount %d\n", (*it)->prating_count);
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_profile(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    PROFILE_TYPE *profile;

    profile = new_profile();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'C':
        KEY("CoreSymbol", profile->core_symbol, fread_number(fp));
        KEY("CultSymbol", profile->cult_symbol, fread_number(fp));
        break;
      case 'E':
        KEY("Education", profile->education, fread_number(fp));
        KEY("EventCount", profile->event_count, fread_number(fp));
        KEY("EventTotal", profile->event_total, fread_number(fp));
        if (!str_cmp(word, "End")) {
          if (!profile->name) {
            bug("Fread_profile: Text not found.", 0);
            return;
          }
          ProfileVect.push_back(profile);
          return;
        }

        break;
      case 'D':
        KEY("DisplayHandle", profile->display_handle, fread_string(fp));
        break;
      case 'G':
        KEY("GRatingTotal", profile->grating_total, fread_number(fp));
        KEY("GRatingCount", profile->grating_count, fread_number(fp));
        break;
      case 'H':
        KEY("Handle", profile->handle, fread_string(fp));
        KEY("HostRating", profile->host_rating, fread_number(fp));
        break;
      case 'I':
        KEY("Influencer", profile->influencer, fread_number(fp));
        break;
      case 'J':
        KEY("Joke", profile->joke, fread_string(fp));
        break;
      case 'L':
        KEY("LastActive", profile->last_active, fread_number(fp));
        KEY("LastLogon", profile->last_logon, fread_number(fp));
        break;
      case 'N':
        KEY("Name", profile->name, fread_string(fp));
        break;
      case 'P':
        KEY("Photo", profile->photo, fread_string(fp));
        KEY("Plus", profile->plus, fread_number(fp));
        KEY("Prefs", profile->prefs, fread_string(fp));
        KEY("Premium", profile->premium, fread_number(fp));
        KEY("Profile", profile->profile, fread_string(fp));
        KEY("PartyTotal", profile->party_total, fread_number(fp));
        KEY("PartyCount", profile->party_count, fread_number(fp));
        KEY("PerfTotal", profile->perf_total, fread_number(fp));
        KEY("PerfCount", profile->perf_count, fread_number(fp));
        KEY("PartyBoost", profile->party_boost, fread_number(fp));
        KEY("PRatingTotal", profile->prating_total, fread_number(fp));
        KEY("PRatingCount", profile->prating_count, fread_number(fp));
        break;
      case 'Q':
        KEY("Quote", profile->quote, fread_string(fp));
        break;
      case 'R':
        KEY("Rating", profile->rating, fread_number(fp));
        KEY("RatingBoost", profile->rating_boost, fread_number(fp));
        KEY("RatingCount", profile->rating_count, fread_number(fp));
        break;
      case 'S':
        KEY("SectSymbol", profile->sect_symbol, fread_number(fp));
        break;
      case 'W':
        KEY("Wealth", profile->wealth, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_profile: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_profiles() {
    nullprofile = new_profile();
    FILE *fp;

    if ((fp = fopen(PROFILE_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Profiles: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PROFILE")) {
          fread_profile(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Profile: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open profile.txt", 0);
      exit(0);
    }
  }

  /*
struct match_type
{
char * nameone;
char * nametwo;
int friendmatch_one;
int datematch_one;
int profmatch_one;
int friendmatch_two;
int datematch_two;
int profmatch_two;
int status_one;
int status_two;
int last_activity;
int last_prompt;
int last_msg_one_one;
int last_msg_one_two;
int last_msg_one_three;
int last_msg_two_one;
int last_msg_two_two;
int last_msg_two_three;
int premium_one;
int premium_two;
int camhack_one;
int camhack_two;
int score_one_manual_chat;
int score_two_manual_chat;
int score_one_auto_chat;
int score_two_auto_chat;
char * auto_chat_review;
int auto_score_when;
int score_one_manual_inperson;
int score_two_manual_inperson;
int chat_active_one;
int chat_active_two;
int rp_active_one;
int rp_active_two;
int chat_count;
int rp_count;
int last_chat_count;
int last_rp_count;
int last_rp_when;
int last_chat_when;
int failed_rp_one;
int failed_rp_two;
int failed_chat_one;
int failed_chat_two;
int success_rp_one;
int success_rp_two;
int success_chat_one;
int success_chat_two;
int chat_initiatior;
int rp_initiatior;
int rate_party_one;
int rate_party_two;
char * party_one_title;
char * party_two_title;
int rate_party_type_one;
int rate_party_type_two;
bool valid;
}
*/

  vector<MATCH_TYPE *> MatchVect;

  MATCH_TYPE *nullmatch;

  void save_matches() {
    FILE *fpout;
    if ((fpout = fopen(MATCH_FILE, "w")) == NULL) {
      bug("Cannot open match.txt for writing", 0);
      return;
    }
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if((*it)->last_activity > 0 && (*it)->last_activity < current_time - (3600*24*90))
      continue;

      fprintf(fpout, "#MATCH\n");
      fprintf(fpout, "NameOne %s~\n", (*it)->nameone);
      fprintf(fpout, "NameTwo %s~\n", (*it)->nametwo);
      fprintf(fpout, "FriendMatchOne %d\n", (*it)->friendmatch_one);
      fprintf(fpout, "DateMatchOne %d\n", (*it)->datematch_one);
      fprintf(fpout, "ProfMatchOne %d\n", (*it)->profmatch_one);
      fprintf(fpout, "FriendMatchTwo %d\n", (*it)->friendmatch_two);
      fprintf(fpout, "DateMatchTwo %d\n", (*it)->datematch_two);
      fprintf(fpout, "ProfMatchTwo %d\n", (*it)->profmatch_two);
      fprintf(fpout, "StatusOne %d\n", (*it)->status_one);
      fprintf(fpout, "StatusTwo %d\n", (*it)->status_two);
      fprintf(fpout, "LastActivity %d\n", (*it)->last_activity);
      fprintf(fpout, "LastPrompt %d\n", (*it)->last_prompt);
      fprintf(fpout, "LastMsgOneOne %d\n", (*it)->last_msg_one_one);
      fprintf(fpout, "LastMsgOneTwo %d\n", (*it)->last_msg_one_two);
      fprintf(fpout, "LastMsgOneThree %d\n", (*it)->last_msg_one_three);
      fprintf(fpout, "LastMsgTwoOne %d\n", (*it)->last_msg_two_one);
      fprintf(fpout, "LastMsgTwoTwo %d\n", (*it)->last_msg_two_two);
      fprintf(fpout, "LastMsgTwoThree %d\n", (*it)->last_msg_two_three);
      fprintf(fpout, "PremiumOne %d\n", (*it)->premium_one);
      fprintf(fpout, "PremiumTwo %d\n", (*it)->premium_two);
      fprintf(fpout, "CamHackOne %d\n", (*it)->camhack_one);
      fprintf(fpout, "CamHackTwo %d\n", (*it)->camhack_two);
      fprintf(fpout, "ScoreOneManualChat %d\n", (*it)->score_one_manual_chat);
      fprintf(fpout, "ScoreTwoManualChat %d\n", (*it)->score_two_manual_chat);
      fprintf(fpout, "ScoreOneAutoChat %d\n", (*it)->score_one_auto_chat);
      fprintf(fpout, "ScoreTwoAutoChat %d\n", (*it)->score_two_auto_chat);
      fprintf(fpout, "AutoChatReview %s~\n", (*it)->auto_chat_review);
      fprintf(fpout, "AutoScoreWhen %d\n", (*it)->auto_score_when);
      fprintf(fpout, "ScoreOneManualInPerson %d\n", (*it)->score_one_manual_inperson);
      fprintf(fpout, "ScoreTwoManualInPerson %d\n", (*it)->score_two_manual_inperson);
      fprintf(fpout, "ChatActiveOne %d\n", (*it)->chat_active_one);
      fprintf(fpout, "ChatActiveTwo %d\n", (*it)->chat_active_two);
      fprintf(fpout, "RPActiveOne %d\n", (*it)->rp_active_one);
      fprintf(fpout, "RPActiveTwo %d\n", (*it)->rp_active_two);
      fprintf(fpout, "ChatCount %d\n", (*it)->chat_count);
      fprintf(fpout, "RPCount %d\n", (*it)->rp_count);
      fprintf(fpout, "LastChatCount %d\n", (*it)->last_chat_count);
      fprintf(fpout, "LastRPCount %d\n", (*it)->last_rp_count);
      fprintf(fpout, "LastRPWhen %d\n", (*it)->last_rp_when);
      fprintf(fpout, "LastChatWhen %d\n", (*it)->last_chat_when);
      fprintf(fpout, "FailedRPOne %d\n", (*it)->failed_rp_one);
      fprintf(fpout, "FailedRPTwo %d\n", (*it)->failed_rp_two);
      fprintf(fpout, "FailedChatOne %d\n", (*it)->failed_chat_one);
      fprintf(fpout, "FailedChatTwo %d\n", (*it)->failed_chat_two);
      fprintf(fpout, "SuccessRPOne %d\n", (*it)->success_rp_one);
      fprintf(fpout, "SuccessRPTwo %d\n", (*it)->success_rp_two);
      fprintf(fpout, "SuccessChatOne %d\n", (*it)->success_chat_one);
      fprintf(fpout, "SuccessChatTwo %d\n", (*it)->success_chat_two);
      fprintf(fpout, "ChatInitiatior %d\n", (*it)->chat_initiatior);
      fprintf(fpout, "RPInitiatior %d\n", (*it)->rp_initiatior);
      fprintf(fpout, "RatePartyOne %d\n", (*it)->rate_party_one);
      fprintf(fpout, "RatePartyTwo %d\n", (*it)->rate_party_two);
      fprintf(fpout, "PartyOneTitle %s~\n", (*it)->party_one_title);
      fprintf(fpout, "PartyTwoTitle %s~\n", (*it)->party_two_title);
      fprintf(fpout, "RatePartyTypeOne %d\n", (*it)->rate_party_type_one);
      fprintf(fpout, "RatePartyTypeTwo %d\n", (*it)->rate_party_type_two);

      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_match(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    MATCH_TYPE *match;

    match = new_match();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'A':
        KEY("AutoChatReview", match->auto_chat_review, fread_string(fp));
        KEY("AutoScoreWhen", match->auto_score_when, fread_number(fp));
        KEY("AutoChatReview", match->auto_chat_review, fread_string(fp));
        break;
      case 'C':
        KEY("CamHackOne", match->camhack_one, fread_number(fp));
        KEY("CamHackTwo", match->camhack_two, fread_number(fp));
        KEY("ChatActiveOne", match->chat_active_one, fread_number(fp));
        KEY("ChatActiveTwo", match->chat_active_two, fread_number(fp));
        KEY("ChatCount", match->chat_count, fread_number(fp));
        KEY("ChatInitiatior", match->chat_initiatior, fread_number(fp));

        break;
      case 'D':
        KEY("DateMatchOne", match->datematch_one, fread_number(fp));
        KEY("DateMatchTwo", match->datematch_two, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!match->nameone) {
            bug("Fread_match: Text not found.", 0);
            return;
          }
          MatchVect.push_back(match);
          return;
        }
        break;
      case 'F':
        KEY("FriendMatchOne", match->friendmatch_one, fread_number(fp));
        KEY("FriendMatchTwo", match->friendmatch_two, fread_number(fp));
        KEY("FailedRPOne", match->failed_rp_one, fread_number(fp));
        KEY("FailedRPTwo", match->failed_rp_two, fread_number(fp));
        KEY("FailedChatOne", match->failed_chat_one, fread_number(fp));
        KEY("FailedChatTwo", match->failed_chat_two, fread_number(fp));
        break;
      case 'L':
        KEY("LastActivity", match->last_activity, fread_number(fp));
        KEY("LastMsgOneOne", match->last_msg_one_one, fread_number(fp));
        KEY("LastMsgOneThree", match->last_msg_one_three, fread_number(fp));
        KEY("LastMsgOneTwo", match->last_msg_one_two, fread_number(fp));
        KEY("LastMsgTwoOne", match->last_msg_two_one, fread_number(fp));
        KEY("LastMsgTwoThree", match->last_msg_two_three, fread_number(fp));
        KEY("LastMsgTwoTwo", match->last_msg_two_two, fread_number(fp));
        KEY("LastPrompt", match->last_prompt, fread_number(fp));
        KEY("LastChatCount", match->last_chat_count, fread_number(fp));
        KEY("LastRPCount", match->last_rp_count, fread_number(fp));
        KEY("LastRPWhen", match->last_rp_when, fread_number(fp));
        KEY("LastChatWhen", match->last_chat_when, fread_number(fp));

        break;
      case 'M':
        KEY("ManualChatOne", match->score_one_manual_chat, fread_number(fp));
        KEY("ManualChatTwo", match->score_two_manual_chat, fread_number(fp));
        KEY("ManualInPersonOne", match->score_one_manual_inperson, fread_number(fp));
        KEY("ManualInPersonTwo", match->score_two_manual_inperson, fread_number(fp));
        break;
      case 'N':
        KEY("NameOne", match->nameone, fread_string(fp));
        KEY("NameTwo", match->nametwo, fread_string(fp));
        break;
      case 'P':
        KEY("PremiumOne", match->premium_one, fread_number(fp));
        KEY("PremiumTwo", match->premium_two, fread_number(fp));
        KEY("ProfMatchOne", match->profmatch_one, fread_number(fp));
        KEY("ProfMatchTwo", match->profmatch_two, fread_number(fp));
        KEY("PartyOneTitle", match->party_one_title, fread_string(fp));
        KEY("PartyTwoTitle", match->party_two_title, fread_string(fp));
        break;
      case 'R':
        KEY("RPActiveOne", match->rp_active_one, fread_number(fp));
        KEY("RPActiveTwo", match->rp_active_two, fread_number(fp));
        KEY("RPCount", match->rp_count, fread_number(fp));
        KEY("RPInitiatior", match->rp_initiatior, fread_number(fp));
        KEY("RatePartyOne", match->rate_party_one, fread_number(fp));
        KEY("RatePartyTwo", match->rate_party_two, fread_number(fp));
        KEY("RatePartyTypeOne", match->rate_party_type_one, fread_number(fp));
        KEY("RatePartyTypeTwo", match->rate_party_type_two, fread_number(fp));
        break;
      case 'S':

        KEY("ScoreOneAutoChat", match->score_one_auto_chat, fread_number(fp));
        KEY("ScoreTwoAutoChat", match->score_two_auto_chat, fread_number(fp));
        KEY("ScoreOneManualChat", match->score_one_manual_chat, fread_number(fp));
        KEY("ScoreTwoManualChat", match->score_two_manual_chat, fread_number(fp));
        KEY("ScoreOneManualInPerson", match->score_one_manual_inperson, fread_number(fp));
        KEY("ScoreTwoManualInPerson", match->score_two_manual_inperson, fread_number(fp));
        KEY("StatusOne", match->status_one, fread_number(fp));
        KEY("StatusTwo", match->status_two, fread_number(fp));
        KEY("SuccessRPOne", match->success_rp_one, fread_number(fp));
        KEY("SuccessRPTwo", match->success_rp_two, fread_number(fp));
        KEY("SuccessChatOne", match->success_chat_one, fread_number(fp));
        KEY("SuccessChatTwo", match->success_chat_two, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_match: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_matches() {
    nullmatch = new_match();
    FILE *fp;

    if ((fp = fopen(MATCH_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Matches: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "MATCH")) {
          fread_match(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Match: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open match.txt", 0);
      exit(0);
    }
  }

  /*
struct chatroom_type
{
int vnum;
char * name;
char * description;
char * history;
int last_msg;
}
*/

  vector<CHATROOM_TYPE *> ChatroomVect;

  CHATROOM_TYPE *nullchatroom;

  void save_chatrooms() {
    FILE *fpout;
    if ((fpout = fopen(CHATROOM_FILE, "w")) == NULL) {
      bug("Cannot open chatroom.txt for writing", 0);
      return;
    }
    for (vector<CHATROOM_TYPE *>::iterator it = ChatroomVect.begin();
    it != ChatroomVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if((*it)->last_msg > 0 && (*it)->last_msg < current_time - (3600*24*90))
      continue;

      fprintf(fpout, "#CHATROOM\n");
      fprintf(fpout, "Vnum %d\n", (*it)->vnum);
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "History %s~\n", (*it)->history);
      fprintf(fpout, "LastMsg %d\n", (*it)->last_msg);
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_chatroom(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    CHATROOM_TYPE *chatroom;

    chatroom = new_chatroom();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'D':
        KEY("Description", chatroom->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!chatroom->name) {
            bug("Fread_chatroom: Text not found.", 0);
            return;
          }
          ChatroomVect.push_back(chatroom);
          return;
        }
        break;
      case 'H':
        KEY("History", chatroom->history, fread_string(fp));
        break;
      case 'L':
        KEY("LastMsg", chatroom->last_msg, fread_number(fp));
        break;
      case 'N':
        KEY("Name", chatroom->name, fread_string(fp));
        break;
      case 'V':
        KEY("Vnum", chatroom->vnum, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_chatroom: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_chatrooms() {
    nullchatroom = new_chatroom();
    FILE *fp;

    if ((fp = fopen(CHATROOM_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Chatrooms: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "CHATROOM")) {
          fread_chatroom(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Chatroom: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open chatroom.txt", 0);
      exit(0);
    }
  }


  PROFILE_TYPE *profile_lookup(char *name) {
    for (vector<PROFILE_TYPE *>::iterator it = ProfileVect.begin();
    it != ProfileVect.end(); ++it) {
      if (!str_cmp(name, (*it)->name))
      return (*it);
    }
    return NULL;
  }
  PROFILE_TYPE *profile_handle_lookup(char * handle) {
    for (vector<PROFILE_TYPE *>::iterator it = ProfileVect.begin();
    it != ProfileVect.end(); ++it) {
      if (!str_cmp(handle, (*it)->handle))
      return (*it);
    }
    return NULL;
  }

  MATCH_TYPE * match_find(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if (!str_cmp(ch->name, (*it)->nameone) && !str_cmp(victim->name, (*it)->nametwo))
      return (*it);
      if (!str_cmp(ch->name, (*it)->nametwo) && !str_cmp(victim->name, (*it)->nameone))
      return (*it);
    }
    return NULL;
  }
  MATCH_TYPE * match_find_by_name(char * nameone, char * nametwo)
  {
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if (!str_cmp(nameone, (*it)->nameone) && !str_cmp(nametwo, (*it)->nametwo))
      return (*it);
      if (!str_cmp(nameone, (*it)->nametwo) && !str_cmp(nametwo, (*it)->nameone))
      return (*it);
    }
    return NULL;
  }

  CHATROOM_TYPE * chatroom_lookup_name(char *name) {
    for (vector<CHATROOM_TYPE *>::iterator it = ChatroomVect.begin();
    it != ChatroomVect.end(); ++it) {
      if (!str_cmp(from_color(name), from_color((*it)->name)))
      return (*it);
    }
    return NULL;
  }

  CHATROOM_TYPE * chatroom_lookup_vnum(int vnum) {
    for (vector<CHATROOM_TYPE *>::iterator it = ChatroomVect.begin();
    it != ChatroomVect.end(); ++it) {
      if (vnum == (*it)->vnum)
      return (*it);
    }
    return NULL;
  }
  char * match_string(MATCH_TYPE *match)
  {
    char buf[MSL];
    sprintf(buf, "");
    if(match->friendmatch_one == 1 && match->friendmatch_two == 1)
    {
      strcat(buf, "friends, ");
    }
    if(match->datematch_one == 1 && match->datematch_two == 1)
    {
      strcat(buf, "dating, ");
    }
    if(match->profmatch_one == 1 && match->profmatch_two == 1)
    {
      strcat(buf, "professional, ");
    }
    return str_dup(buf);
  }

  int rp_with_count(CHAR_DATA *ch)
  {
    int count = 0;
    if(ch->pcdata->suspend_myhaven == 1)
    return 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      MATCH_TYPE *match = (*it);
      if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
      {
        if(match->rp_active_one == 1)
        count++;
      }
      if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
      {
        if(match->rp_active_two == 1)
        count++;
      }
    }
    return count;
  }
  int chat_with_count(CHAR_DATA *ch)
  {
    int count = 0;
    if(ch->pcdata->suspend_myhaven == 1)
    return 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      MATCH_TYPE *match = (*it);
      if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
      {
        if(match->chat_active_one == 1)
        count++;
      }
      if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
      {
        if(match->chat_active_two == 1)
        count++;
      }
    }
    return count;
  }
  int rp_you_count(CHAR_DATA *ch)
  {
    CHAR_DATA *victim = NULL;
    int count = 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      MATCH_TYPE *match = (*it);
      if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
      {
        victim = get_char_world_pc(match->nametwo);
        if(match->rp_active_two == 1 && victim != NULL && victim->pcdata->suspend_myhaven != 1)
        count++;
      }
      if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
      {
        victim = get_char_world_pc(match->nameone);
        if(match->rp_active_one == 1 && victim != NULL && victim->pcdata->suspend_myhaven != 1)
        count++;
      }
    }
    return count;
  }
  int chat_you_count(CHAR_DATA *ch)
  {
    CHAR_DATA *victim = NULL;
    int count = 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      MATCH_TYPE *match = (*it);
      if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
      {
        victim = get_char_world_pc(match->nametwo);
        if(match->chat_active_two == 1 && victim != NULL && victim->pcdata->suspend_myhaven != 1)
        count++;
      }
      if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
      {
        victim = get_char_world_pc(match->nameone);
        if(match->chat_active_one == 1 && victim != NULL && victim->pcdata->suspend_myhaven != 1)
        count++;
      }
    }
    return count;
  }

  bool chat_viable(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    if(IS_FLAG(victim->comm, COMM_AFK))
    {
      return FALSE;
    }
    if(IS_FLAG(ch->comm, COMM_AFK))
    {
      return FALSE;
    }
    if (ch->pcdata->availability == AVAIL_LOW)
    return FALSE;
    if (victim->pcdata->availability == AVAIL_LOW)
    return FALSE;
    if(!free_to_act(ch))
    {
      return FALSE;
    }
    if(!free_to_act(victim))
    {
      return FALSE;
    }
    if(!cell_signal(ch))
    {
      return FALSE;
    }
    if(!cell_signal(victim))
    {
      return FALSE;
    }
    MATCH_TYPE *match = match_find(ch, victim);
    if(match == NULL)
    {
      return FALSE;
    }
    if(match->status_one != 1 || match->status_two != 1)
    {
      return FALSE;
    }
    if(match->chat_active_one == 0 || match->chat_active_two == 0)
    {
      return FALSE;
    }
    if(pc_pop(ch->in_room) > 1)
    {
      return FALSE;
    }
    if(pc_pop(victim->in_room) > 1)
    {
      return FALSE;
    }


    return TRUE;
  }


  bool meet_viable(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    if(IS_FLAG(victim->comm, COMM_AFK))
    {
      return FALSE;
    }
    if(IS_FLAG(ch->comm, COMM_AFK))
    {
      return FALSE;
    }
    if (ch->pcdata->availability == AVAIL_LOW)
    return FALSE;
    if (victim->pcdata->availability == AVAIL_LOW)
    return FALSE;
    if(!free_to_act(ch))
    {
      return FALSE;
    }
    if(!free_to_act(victim))
    {
      return FALSE;
    }
    if(!cell_signal(ch))
    {
      return FALSE;
    }
    if(!cell_signal(victim))
    {
      return FALSE;
    }
    MATCH_TYPE *match = match_find(ch, victim);
    if(match == NULL)
    {
      return FALSE;
    }
    if(match->status_one != 1 || match->status_two != 1)
    {
      return FALSE;
    }
    if(match->rp_active_one == 0 || match->rp_active_two == 0)
    {
      return FALSE;
    }
    if(pc_pop(ch->in_room) > 1)
    {
      return FALSE;
    }
    if(pc_pop(victim->in_room) > 1)
    {
      return FALSE;
    }
    return TRUE;

    for (vector<TEXTHISTORY_TYPE *>::iterator hit = HTextVect.begin();
    hit != HTextVect.end(); ++hit) {
      if ((*hit)->valid == FALSE)
      continue;
      if(!str_cmp((*hit)->name_one, ch->name) && !str_cmp((*hit)->name_two, victim->name))
      {
        if(linecount((*hit)->history) > 30)
        return TRUE;
      }
      if(!str_cmp((*hit)->name_one, victim->name) && !str_cmp((*hit)->name_two, ch->name))
      {
        if(linecount((*hit)->history) > 30)
        return TRUE;
      }
    }



    return FALSE;
  }


  void social_update()
  {
    CHAR_DATA *to;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        PROFILE_TYPE *char_profile = profile_lookup(to->name);
        if(char_profile == NULL)
        continue;
        char_profile->last_logon = current_time;

        if(pc_pop(to->in_room) == 1 && !IS_FLAG(to->comm, COMM_AFK) && to->pcdata->availability != AVAIL_LOW && number_percent() % 76 == 0 && chat_with_count(to) == 0 && rp_with_count(to) == 0)
        {
          send_to_char("You get a notification on your phone from MyHaven that you aren't currently trying to chat with or meet with anyone.\n\r", to);
        }

        int rp_you = rp_you_count(to);
        int chat_you = chat_you_count(to);
        if(rp_you > to->pcdata->missed_rp_connections)
        to->pcdata->missed_rp_connections = rp_you;
        if(chat_you > to->pcdata->missed_chat_connections)
        to->pcdata->missed_chat_connections = chat_you;


        if(safe_strlen(char_profile->display_handle) < 3)
        {
          free_string(char_profile->display_handle);
          char_profile->display_handle = str_dup(char_profile->handle);
        }

        if(char_profile->last_active > 0 && char_profile->last_active > current_time - (60*30))
        continue;

        if(to->pcdata->suspend_myhaven == 1)
        continue;

        for (DescList::iterator vit = descriptor_list.begin();
        vit != descriptor_list.end(); ++vit) {
          DESCRIPTOR_DATA *vd = *vit;

          if (vd->character != NULL && vd->connected == CON_PLAYING) {
            victim = vd->character;
            if (IS_NPC(victim))
            continue;
            if(to->pcdata->account == victim->pcdata->account)
            continue;
            PROFILE_TYPE *vic_profile = profile_lookup(victim->name);
            if(vic_profile == NULL)
            continue;

            if(vic_profile->last_active > 0 && vic_profile->last_active > current_time - (60*30))
            continue;

            if(victim->pcdata->suspend_myhaven == 1)
            continue;

            if(meet_viable(to, victim) && number_percent() % 4 == 0)
            {
              char_profile->last_active = current_time;
              vic_profile->last_active = current_time;
              MATCH_TYPE *match = match_find(to, victim);
              printf_to_char(to, "[`087MyHaven`x] You have been matched to meet up with %s(%s), you should pick the place/activity.\n\r", vic_profile->handle, match_string(match));
              printf_to_char(victim, "[`087MyHaven`x] You have been matched to meet up with %s(%s), they will pick the place/activity.\n\r", char_profile->handle, match_string(match));
              match->last_rp_when = current_time;
              match->last_rp_count = match->rp_count;
              if(!str_cmp(to->name, match->nameone))
              match->rp_initiatior = 1;
              else
              match->rp_initiatior = 2;
              return;
            }
            if(!meet_viable(to, victim) && chat_viable(to, victim) && number_percent() % 3 == 0)
            {
              char_profile->last_active = current_time;
              vic_profile->last_active = current_time;
              MATCH_TYPE *match = match_find(to, victim);
              printf_to_char(to, "[`087MyHaven`x] You have been matched to chat with %s(%s), you should start the conversation. You can use text %s (message) to do so.\n\r", vic_profile->handle, match_string(match), vic_profile->handle);
              printf_to_char(victim, "[`087MyHaven`x] You have been matched to chat with %s(%s), they will start the conversation.\n\r", char_profile->handle, match_string(match));
              match->last_chat_when = current_time;
              match->last_chat_count = match->chat_count;
              if(!str_cmp(to->name, match->nameone))
              match->chat_initiatior = 1;
              else
              match->chat_initiatior = 2;
              return;
            }
          }
        }
      }
    }

    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      MATCH_TYPE *match = (*it);
      if(match->chat_initiatior == 1 && match->last_chat_when < current_time - (3600*3))
      {
        bool tried = FALSE;
        if(match->last_msg_one_one > match->last_chat_when && match->last_msg_two_one < match->last_chat_when)
        tried = TRUE;
        if(tried == FALSE)
        {
          int ccount = match->chat_count - match->last_chat_count;
          if(ccount < 2)
          match->failed_chat_one++;
          else
          match->success_chat_one++;
          match->chat_initiatior = 0;
        }
        else if(number_percent() % 3 == 0)
        {
          match->success_chat_one++;
          match->failed_chat_two++;
        }
      }
      if(match->chat_initiatior == 2 && match->last_chat_when < current_time - (3600*3))
      {
        bool tried = FALSE;
        if(match->last_msg_two_one > match->last_chat_when && match->last_msg_one_one < match->last_chat_when)
        tried = TRUE;
        if(tried == FALSE)
        {

          int ccount = match->chat_count - match->last_chat_count;
          if(ccount < 2)
          match->failed_chat_two++;
          else
          match->success_chat_two++;
          match->chat_initiatior = 0;
        }
        else if(number_percent() % 3 == 0)
        {
          match->failed_chat_one++;
          match->success_chat_two++;
        }
      }
      if(match->rp_initiatior == 1 && match->last_rp_when < current_time - (3600*6))
      {
        bool tried = FALSE;
        if(match->last_msg_one_one > match->last_rp_when && match->last_msg_two_one < match->last_rp_when)
        tried = TRUE;
        if(tried == FALSE)
        {
          int ccount = match->rp_count - match->last_rp_count;
          if(ccount < 2)
          match->failed_rp_one++;
          else
          match->success_rp_one++;
          match->rp_initiatior = 0;
        }
      }
      if(match->rp_initiatior == 2 && match->last_rp_when < current_time - (3600*6))
      {
        bool tried = FALSE;
        if(match->last_msg_two_one > match->last_rp_when && match->last_msg_one_one < match->last_rp_when)
        tried = TRUE;
        if(tried == FALSE)
        {
          int ccount = match->rp_count - match->last_rp_count;
          if(ccount < 2)
          match->failed_rp_two++;
          else
          match->success_rp_two++;
          match->rp_initiatior = 0;
        }
      }
    }
  }
  _DOFUN(do_handle)
  {
    if(ch->pcdata->ci_editing == 22)
    {
      ch->pcdata->ci_absorb = 1;
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Handle (string).\n\r", ch);
        return;
      }
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      char buf[MSL];
      sprintf(buf, "@%s", from_color(argument));
      char cbuf[MSL];
      sprintf(cbuf, "@%s", argument);
      for (vector<PROFILE_TYPE *>::iterator it = ProfileVect.begin();
      it != ProfileVect.end(); ++it) {
        if((*it)->valid == FALSE)
        continue;

        if(!str_cmp(from_color((*it)->handle), from_color(buf)))
        {
          send_to_char("That handle is already in use.\n\r", ch);
          return;
        }
      }


      free_string(profile->handle);
      profile->handle = str_dup(buf);
      free_string(profile->display_handle);
      profile->display_handle = str_dup(cbuf);
      send_to_char("Handle set.\n\r", ch);
      return;
    }
    send_to_char("You are not editing your profile.\n\r", ch);
  }

  _DOFUN(do_aboutme)
  {
    if(ch->pcdata->ci_editing == 22)
    {
      ch->pcdata->ci_absorb = 1;
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      string_append(ch, &profile->profile);
      send_to_char("The About Me section is visible to anyone who browses your profile and should provie a summary of your personality, interests and other things people may wish to know.\n\r", ch);
      return;
    }
    send_to_char("You are not editing your profile.\n\r", ch);
  }

  _DOFUN(do_quote)
  {
    if(ch->pcdata->ci_editing == 22)
    {
      ch->pcdata->ci_absorb = 1;
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Quote (string).\n\r", ch);
        return;
      }
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      free_string(profile->quote);
      profile->quote = str_dup(argument);
      send_to_char("Quote set.\n\r", ch);
      return;
    }
    send_to_char("You are not editing your profile.\n\r", ch);
  }

  _DOFUN(do_joke)
  {
    if(ch->pcdata->ci_editing == 22)
    {
      ch->pcdata->ci_absorb = 1;
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      string_append(ch, &profile->joke);
      send_to_char("The Joke section is visible to anyone who browses your profile.\n\r", ch);
      return;
    }
    send_to_char("You are not editing your profile.\n\r", ch);
  }

  _DOFUN(do_attractedto)
  {
    if(ch->pcdata->ci_editing == 22)
    {
      ch->pcdata->ci_absorb = 1;
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      string_append(ch, &profile->prefs);
      send_to_char("This section is only visible to people you've date matched with and is used to explain the kinds of people you're attracted to.\n\r", ch);
      return;
    }
    send_to_char("You are not editing your profile.\n\r", ch);
  }


  /*
PCDATA
int active_chatroom;
int in_chatroom[50];
CHAR_DATA * cam_spy_char;
*/

  //Chatroom commands: chat (message), chat create (roomname), chat listen (roomname), chat join (roomname), chat who (roomname), chat history (roomname)

  //myhaven commands: myhaven setup, myhaven browse, myhaven match (character) (friends/date/professional), myhaven viewprofile (handle), myhaven chatwith (character list), myhaven hangoutwith (character list), myhaven camhack (person), myhaven telekinesis (person) (message), myhaven hypnotise (person) (command), myhaven forcechat (person), myhaven forcehangout (person)

  bool can_social_hack(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    MATCH_TYPE *match = match_find(ch, victim);
    if(match == NULL)
    {
      return FALSE;
    }
    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    if(char_profile->premium == 0)
    return FALSE;
    if(!str_cmp(match->nameone, ch->name))
    {
      if(match->last_msg_two_one > 0 && match->last_msg_two_one > current_time - (3600*3) && match->last_msg_two_two > 0 && match->last_msg_two_two > current_time - (3600*3) && match->last_msg_two_three > 0 && match->last_msg_two_three > current_time - (3600*3))
      return TRUE;
    }
    if(!str_cmp(match->nametwo, ch->name))
    {
      if(match->last_msg_one_one > 0 && match->last_msg_one_one > current_time - (3600*3) && match->last_msg_one_two > 0 && match->last_msg_one_two > current_time - (3600*3) && match->last_msg_one_three > 0 && match->last_msg_one_three > current_time - (3600*3))
      return TRUE;
    }
    return FALSE;
  }

  int profile_rating(CHAR_DATA *ch)
  {
    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    if(char_profile == NULL)
    return 0;
    return char_profile->rating;
  }
  char * display_profile_rating(PROFILE_TYPE *profile)
  {
    float drating = (float)(profile->rating)/10.0;
    if(drating < 3.0)
    {
      float dif = 4.0 - drating;
      drating = 4.0 - (dif/2);
      if(drating > 3.8)
      drating = 3.8;
    }
    else if(drating < 4.0)
    {
      float dif = 5.0 - drating;
      drating = 5.0 - (dif/2);
      if(drating > 4.0)
      drating = 4.0;
    }
    else if(drating > 4.5)
    {
      drating -= 4.5;
      drating /= 2;
      drating += 4.5;
    }
    if(drating > 5.0)
    drating = 5.0;
    if(profile->rating_count < 2)
    {
      return str_dup("`DUnrated`w");
    }
    char buf[MSL];
    if(profile->influencer == 1)
    sprintf(buf, "%0.1f*`x[`242%d`x, `229MyHaven Influencer`x]", drating, profile->rating_count);
    else
    sprintf(buf, "%0.1f*`x[`242%d`x]", drating, profile->rating_count);
    return str_dup(buf);
  }

  TEXTHISTORY_TYPE * get_thist(char *none, char *ntwo)
  {
    for (vector<TEXTHISTORY_TYPE *>::iterator it = HTextVect.begin();
    it != HTextVect.end(); ++it) {
      if (!str_cmp(none, (*it)->name_one) && !str_cmp(ntwo, (*it)->name_two))
      return (*it);
      if (!str_cmp(none, (*it)->name_two) && !str_cmp(ntwo, (*it)->name_one))
      return (*it);
    }
    return NULL;

  }

  void ai_social_score(MATCH_TYPE *match)
  {
    TEXTHISTORY_TYPE *thist = get_thist(match->nameone, match->nametwo);
    if(thist == NULL)
    return;
    if(linecount(thist->history) < 30)
    return;

    char buf[MSL];
    sprintf(buf, "4,0,%s,%s,,,", match->nameone, match->nametwo);

    writeLineToFile(AI_IN_FILE, str_dup(buf));
  }

  int auto_score_mod(int score)
  {
    int rval = score*score / 100;
    rval = rval * 2;
    return rval;
  }



  void profile_logon(CHAR_DATA *ch)
  {
    char logs[MSL];
    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    if(char_profile == NULL)
    {
      send_to_char("You should consider setting up a MyHaven profile.\n\r", ch);
      return;
    }

    if(IS_FLAG(ch->act, PLR_INFLUENCER))
    char_profile->influencer = 1;
    else
    char_profile->influencer = 0;

    char_profile->last_active = current_time - (3600);
    char_profile->wealth = get_skill(ch, SKILL_WEALTH);
    char_profile->education = get_skill(ch, SKILL_EDUCATION);
    if(get_tier(ch) > 1 && (!str_cmp(ch->pcdata->last_name, "Inigo") || !str_cmp(ch->pcdata->last_name, "Arkwright")))
    char_profile->premium = 1;
    if (IS_FLAG(ch->act, PLR_VILLAIN))
    char_profile->premium = 1;
    if(higher_power(ch))
    char_profile->premium = 1;
    int rating_count = 0;
    int rating_score = 0;
    int rating_auto_count = 0;
    int rating_auto_score = 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if(!str_cmp(ch->name, (*it)->nameone))
      {
        if((*it)->score_one_manual_chat > 0 && (*it)->score_two_manual_chat > 0)
        {
          if((*it)->auto_score_when < current_time - (3600*24*30))
          {
            ai_social_score(*it);
          }
        }
        if(str_cmp(ch->pcdata->last_sexed[0], (*it)->nametwo) && str_cmp(ch->pcdata->last_sexed[1], (*it)->nametwo) && str_cmp(ch->pcdata->last_sexed[2], (*it)->nametwo))
        {
          if((*it)->score_one_manual_chat > 0)
          {
            rating_count++;
            rating_score += (*it)->score_one_manual_chat;
            rating_score += (*it)->success_chat_one;
            rating_score -= (*it)->failed_chat_one;
            if((*it)->score_one_auto_chat > 0)
            {
              rating_auto_count++;
              rating_auto_score += auto_score_mod((*it)->score_one_auto_chat);
              if((*it)->score_one_auto_chat > (*it)->score_two_auto_chat)
              {
                rating_score += 1;
                if((*it)->score_one_manual_chat < (*it)->score_two_manual_chat)
                rating_score += 1;
              }
              else if((*it)->score_one_auto_chat < (*it)->score_two_auto_chat)
              {
                if((*it)->score_one_manual_chat > (*it)->score_two_manual_chat)
                rating_score -= 1;
              }
            }
          }
          if((*it)->score_one_manual_inperson > 0)
          {
            rating_count++;
            rating_score += (*it)->score_one_manual_inperson;
            rating_score += (*it)->success_rp_one;
            rating_score -= (*it)->failed_rp_one;
          }
        }
        //          (*it)->chat_active_one = 0;
        //          (*it)->rp_active_one = 0;
        if((*it)->chat_count >= 50 && (*it)->score_two_manual_chat == 0)
        {
          PROFILE_TYPE *profile = profile_lookup((*it)->nametwo);
          printf_to_char(ch, "You can now anonymously rate %s on how fun they are to chat with, use myhaven rate %s (0.1 - 5.0)\n\r", profile->handle, profile->handle);
        }
        else if((*it)->rp_count >= 25 && (*it)->score_two_manual_inperson == 0 && (*it)->score_two_manual_chat != 0)
        {
          PROFILE_TYPE *profile = profile_lookup((*it)->nametwo);
          printf_to_char(ch, "You can now anonymously rate %s on how fun they are to hangout with, use myhaven rate %s (0.1 - 5.0)\n\r", profile->handle, profile->handle);
        }
        else if((*it)->rate_party_two == 1)
        {
          sprintf(logs, "PARTYRATE %s, %s", ch->name, (*it)->party_two_title);
          log_string(logs);
          printf_to_char(ch, "You can rate %s on how fun it was, use myhaven rateevent (0.1 - 5.0)\n\r", (*it)->party_two_title);
        }
      }
      if(!str_cmp(ch->name, (*it)->nametwo))
      {
        if(str_cmp(ch->pcdata->last_sexed[0], (*it)->nameone) && str_cmp(ch->pcdata->last_sexed[1], (*it)->nameone) && str_cmp(ch->pcdata->last_sexed[2], (*it)->nameone))
        {
          if((*it)->score_two_manual_chat > 0)
          {
            rating_count++;
            rating_score += (*it)->score_two_manual_chat;
            rating_score += (*it)->success_chat_two;
            rating_score -= (*it)->failed_chat_two;
          }
          if((*it)->score_two_auto_chat > 0)
          {
            rating_auto_count++;
            rating_auto_score += auto_score_mod((*it)->score_two_auto_chat);
            if((*it)->score_two_auto_chat > (*it)->score_one_auto_chat)
            {
              rating_score += 1;
              if((*it)->score_two_manual_chat < (*it)->score_one_manual_chat)
              rating_score += 1;
            }
            else if((*it)->score_two_auto_chat < (*it)->score_one_auto_chat)
            {
              if((*it)->score_two_manual_chat > (*it)->score_one_manual_chat)
              rating_score -= 1;
            }

          }
          if((*it)->score_two_manual_inperson > 0)
          {
            rating_count++;
            rating_score += (*it)->score_two_manual_inperson;
            rating_score += (*it)->success_rp_two;
            rating_score -= (*it)->failed_rp_two;
          }
        }
        //        (*it)->chat_active_two = 0;
        //        (*it)->rp_active_two = 0;
        if((*it)->chat_count >= 50 && (*it)->score_one_manual_chat == 0)
        {
          PROFILE_TYPE *profile = profile_lookup((*it)->nameone);
          printf_to_char(ch, "You can now anonymously rate %s on how fun they are to chat with, use myhaven rate %s (0.1 - 5.0)\n\r", profile->handle, profile->handle);
        }
        else if((*it)->rp_count >= 25 && (*it)->score_one_manual_inperson == 0 && (*it)->score_one_manual_chat != 0)
        {
          PROFILE_TYPE *profile = profile_lookup((*it)->nameone);
          printf_to_char(ch, "You can now anonymously rate %s on how fun they are to hangout with, use myhaven rate %s (0.1 - 5.0)\n\r", profile->handle, profile->handle);
        }
        else if((*it)->rate_party_one == 1 && (*it)->rate_party_two == 0)
        {
          sprintf(logs, "PARTYRATE %s, %s", ch->name, (*it)->party_one_title);
          log_string(logs);
          printf_to_char(ch, "You can rate %s on how fun it was, use myhaven rateevent (0.1 - 5.0)\n\r", (*it)->party_one_title);
        }
      }
    }
    int rboost = 0;
    if(rating_count > 1)
    {
      if(char_profile->rating_boost > 0)
      {
        if(rating_auto_count > 0)
        {
          rboost = ((rating_auto_score/rating_auto_count/2) + char_profile->rating_boost)/2;
        }
        else
        rboost = char_profile->rating_boost;
        int initscore = rating_score/rating_count;
        char_profile->rating = (initscore + rboost)/2;
        char_profile->rating_count = rating_count + 1;
      }
      else
      {
        if(rating_auto_count > 0)
        {
          char_profile->rating = (rating_score/rating_count + (rating_auto_score/rating_auto_count/2))/2;
        }
        else
        {
          char_profile->rating = rating_score/rating_count;
          char_profile->rating_count = rating_count;
        }

      }
    }
    float party_rating = 0.0, social_rating = 0.0, performance_rating = 0.0;
    if(char_profile->party_count > 0)
    party_rating = (float)char_profile->party_total/char_profile->party_count;
    if(char_profile->event_count > 0)
    social_rating = (float)char_profile->event_total/char_profile->event_count;
    if(char_profile->perf_count > 0)
    performance_rating = (float)char_profile->perf_total/char_profile->perf_count;

    float host_rating = 0.0;
    host_rating = UMAX(party_rating, social_rating);
    host_rating = UMAX(host_rating, performance_rating);
    if(char_profile->party_boost > 0)
    host_rating = (host_rating + char_profile->party_boost)/2;

    char_profile->host_rating = host_rating;

  }

  _DOFUN(do_profilelogon)
  {
    profile_logon(ch);
  }
  bool compareLastBrowsed(const PROFILE_TYPE* a, const PROFILE_TYPE* b) {
    return a->last_browsed < b->last_browsed;
  }

  char *const hypno_names[] = {"",          "Lust",      "Generosity",   "Logic", "Guilt",     "Anger",     "PeerPressure", "Fear", "Gratitude", "Curiosity", "Jealousy",     "Pity"};


  _DOFUN(do_myhaven)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);


    if(!str_cmp(arg1, "setup"))
    {
      if(ch->pcdata->ci_editing == 22)
      {
        send_to_char("You are already editing your profile.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 22;
      ch->pcdata->ci_absorb = 1;
      send_to_char("You are now editing your profile.\n\r", ch);
      return;
    }

    PROFILE_TYPE *my_profile = profile_lookup(ch->name);
    if(my_profile == NULL && !IS_IMMORTAL(ch))
    {
      send_to_char("Use myhaven setup first.\n\r", ch);
      return;
    }

    if(!str_cmp(arg1, "suspend"))
    {
      if(ch->pcdata->suspend_myhaven == 1)
      {
        ch->pcdata->suspend_myhaven = 0;
        send_to_char("You unsuspend searching for matches.\n\r", ch);
        return;
      }
      ch->pcdata->suspend_myhaven = 1;
      send_to_char("You suspend searching for matches.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "feedback"))
    {
      show_random_ai_feedback(ch);
      return;
    }
    if(!str_cmp(arg1, "buypremium"))
    {
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("You need to set up your profile first.\n\r", ch);
        return;
      }
      if(ch->pcdata->total_money < 500000)
      {
        send_to_char("You need $5000 to buy premium.\n\r", ch);
        return;
      }
      if(char_profile->premium == 1)
      {
        send_to_char("You already have premium.\n\r", ch);
        return;
      }
      char_profile->premium = 1;
      ch->pcdata->total_money -= 500000;
      send_to_char("You have bought premium.\n\r", ch);
      return;

    }
    if(!str_cmp(arg1, "camhack"))
    {
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("You need to set up your profile first.\n\r", ch);
        return;
      }
      PROFILE_TYPE *victim_profile = profile_handle_lookup(argument);
      if(victim_profile == NULL)
      {
        send_to_char("That person does not exist.\n\r", ch);
        return;
      }
      if(get_char_world_pc(victim_profile->name) == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_world_pc(victim_profile->name);
      if(!can_social_hack(ch, victim))
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      if(victim->pcdata->cam_spy_char != NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      victim->pcdata->cam_spy_char = ch;
      send_to_char("You have successfully hacked their phone camera.\n\r", ch);
      send_to_char("Your phone warms up a little.\n\r", victim);
      return;
    }
    if(!str_cmp(arg1, "telekinesis"))
    {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("You need to set up your profile first.\n\r", ch);
        return;
      }
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: MyHaven Telekinesis (person) (message).\n\r", ch);
        return;
      }
      PROFILE_TYPE *victim_profile = profile_handle_lookup(arg2);
      if(victim_profile == NULL)
      {
        send_to_char("That person does not exist.\n\r", ch);
        return;
      }
      if(get_char_world_pc(victim_profile->name) == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_world_pc(victim_profile->name);
      if(!can_social_hack(ch, victim))
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }

      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: MyHaven Telekinesis (person) (message).\n\r", ch);
        return;
      }
      printf_to_char(ch, "[Remotely] %s\n\r", argument);
      printf_to_char(victim, "Your phone warms up a little.\n\n%s\n\r", argument);
      rpreward(ch, argument, TRUE, 2);
      give_attention(ch, victim);
      return;
    }
    if(!str_cmp(arg1, "forcechat"))
    {
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("You need to set up your profile first.\n\r", ch);
        return;
      }
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: MyHaven forcechat (person).\n\r", ch);
        return;
      }
      PROFILE_TYPE *victim_profile = profile_handle_lookup(argument);
      if(victim_profile == NULL)
      {
        send_to_char("That person does not exist.\n\r", ch);
        return;
      }
      if(get_char_world_pc(victim_profile->name) == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_world_pc(victim_profile->name);
      if(char_profile->premium == 0)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      MATCH_TYPE *match = match_find(ch, victim);
      if(match == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      if(!chat_viable(ch, victim))
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      char_profile->last_active = current_time;
      victim_profile->last_active = current_time;
      printf_to_char(ch, "[MyHaven] You have been matched to chat with %s(%s), you should start the conversation. You can use text %s (message) to do so.\n\r", victim_profile->handle, match_string(match), victim_profile->handle);
      printf_to_char(victim, "[MyHaven] You have been matched to chat with %s(%s), they will start the conversation.\n\r", char_profile->handle, match_string(match));
      match->last_chat_when = current_time;
      match->last_chat_count = match->chat_count;
      if(!str_cmp(ch->name, match->nameone))
      match->chat_initiatior = 1;
      else
      match->chat_initiatior = 2;
      return;
    }
    if(!str_cmp(arg1, "forcemeet"))
    {
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("You need to set up your profile first.\n\r", ch);
        return;
      }
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: MyHaven forcemeet (person).\n\r", ch);
        return;
      }
      PROFILE_TYPE *victim_profile = profile_handle_lookup(argument);
      if(victim_profile == NULL)
      {
        send_to_char("That person does not exist.\n\r", ch);
        return;
      }
      if(get_char_world_pc(victim_profile->name) == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_world_pc(victim_profile->name);
      if(char_profile->premium == 0)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      MATCH_TYPE *match = match_find(ch, victim);
      if(match == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      if(!meet_viable(ch, victim))
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      char_profile->last_active = current_time;
      victim_profile->last_active = current_time;
      printf_to_char(ch, "[MyHaven] You have been matched to meet up with %s(%s), you should pick the place/activity.\n\r", victim_profile->handle, match_string(match));
      printf_to_char(victim, "[MyHaven] You have been matched to meet up with %s(%s), they will pick the place/activity.\n\r", char_profile->handle, match_string(match));
      match->last_rp_when = current_time;
      match->last_rp_count = match->rp_count;
      if(!str_cmp(ch->name, match->nameone))
      match->rp_initiatior = 1;
      else
      match->rp_initiatior = 2;
      return;

    }
    if(!str_cmp(arg1, "hypnotise") || !str_cmp(arg1, "hypnotize"))
    {
      char arg[MSL];
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg);
      char arg3[MSL];
      char arg4[MSL];
      char arg5[MSL];
      int pone = 0, ptwo = 0, pthree = 0;

      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("You need to set up your profile first.\n\r", ch);
        return;
      }
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: MyHaven Hypnotise (person) (message).\n\r", ch);
        return;
      }
      PROFILE_TYPE *victim_profile = profile_handle_lookup(arg);
      if(victim_profile == NULL)
      {
        send_to_char("That person does not exist.\n\r", ch);
        return;
      }
      if(get_char_world_pc(victim_profile->name) == NULL)
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_world_pc(victim_profile->name);
      if(!can_social_hack(ch, victim))
      {
        send_to_char("Your attempt fails.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);

      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: MyHaven hypnotize (person) (type) (pressures) (message).\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "instruction")) {
        argument = one_argument_nouncap(argument, arg3);
        argument = one_argument_nouncap(argument, arg4);
        argument = one_argument_nouncap(argument, arg5);
        for (int i = 1; i <= HYPNO_PITY; i++) {
          if (!str_cmp(arg3, hypno_names[i]))
          pone = i;
          if (!str_cmp(arg4, hypno_names[i]))
          ptwo = i;
          if (!str_cmp(arg5, hypno_names[i]))
          pthree = i;
        }
        if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
          send_to_char("Syntax: Hypnotize (person) instruction (pressure one) (pressure two) (pressure three) (instruction)\n\rPossible Pressures", ch);
          for (int i = 1; i <= HYPNO_PITY; i++)
          printf_to_char(ch, ", %s", hypno_names[i]);
          send_to_char(".\n\r", ch);
          return;
        }
        for (int i = 0; i < 25; i++) {
          if (victim->pcdata->imprint_type[i] == 0) {
            victim->pcdata->imprint_type[i] = IMPRINT_INSTRUCTION;
            free_string(victim->pcdata->imprint[i]);
            victim->pcdata->imprint[i] = str_dup(argument);
            victim->pcdata->imprint_pending[i] = -5;
            victim->pcdata->imprint_pressure_one[i] = pone;
            victim->pcdata->imprint_pressure_two[i] = ptwo;
            victim->pcdata->imprint_pressure_three[i] = pthree;
            victim->pcdata->imprinter = ch;
            if (is_helpless(victim))
            victim->pcdata->trance = -5;
            else
            victim->pcdata->trance = -60;

            printf_to_char(ch, "You attempt to instruct %s to %s with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
            ch->pcdata->total_money -= 2000;
            printf_to_char(
            victim, "Your subconcious has received an instruction to %s.\nTo accept this instruction use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(victim, ", %s", hypno_names[i]);
            send_to_char(".\n\r", victim);
            return;
          }
        }
      }
      if (!str_cmp(arg2, "compulsion")) {

        argument = one_argument_nouncap(argument, arg3);
        argument = one_argument_nouncap(argument, arg4);
        argument = one_argument_nouncap(argument, arg5);
        for (int i = 1; i <= HYPNO_PITY; i++) {
          if (!str_cmp(arg3, hypno_names[i]))
          pone = i;
          if (!str_cmp(arg4, hypno_names[i]))
          ptwo = i;
          if (!str_cmp(arg5, hypno_names[i]))
          pthree = i;
        }
        if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
          send_to_char("Syntax: Hypnotize (person) compulsion (pressure one) (pressure two) (pressure three) (instruction)\n\rPossible Pressures", ch);
          for (int i = 1; i <= HYPNO_PITY; i++)
          printf_to_char(ch, ", %s", hypno_names[i]);
          send_to_char(".\n\r", ch);
          return;
        }
        for (int i = 0; i < 25; i++) {
          if (victim->pcdata->imprint_type[i] == 0) {
            victim->pcdata->imprint_type[i] = IMPRINT_COMPULSION;
            free_string(victim->pcdata->imprint[i]);
            victim->pcdata->imprint[i] = str_dup(argument);
            victim->pcdata->imprint_pending[i] = -5;
            victim->pcdata->imprint_pressure_one[i] = pone;
            victim->pcdata->imprint_pressure_two[i] = ptwo;
            victim->pcdata->imprint_pressure_three[i] = pthree;
            victim->pcdata->imprinter = ch;
            if (is_helpless(victim))
            victim->pcdata->trance = -5;
            else
            victim->pcdata->trance = -60;

            printf_to_char(ch, "You attempt to compel %s to %s with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
            ch->pcdata->total_money -= 3000;
            printf_to_char(
            victim, "Your subconcious has received a compulsion to %s.\nTo accept this instruction  use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(victim, ", %s", hypno_names[i]);
            send_to_char(".\n\r", victim);
            return;
          }
        }
      }
      if (!str_cmp(arg2, "body")) {
        char arg6[MSL];
        argument = one_argument_nouncap(argument, arg6);
        if (!str_cmp(arg6, "instruction")) {
          argument = one_argument_nouncap(argument, arg3);
          argument = one_argument_nouncap(argument, arg4);
          argument = one_argument_nouncap(argument, arg5);
          for (int i = 1; i <= HYPNO_PITY; i++) {
            if (!str_cmp(arg3, hypno_names[i]))
            pone = i;
            if (!str_cmp(arg4, hypno_names[i]))
            ptwo = i;
            if (!str_cmp(arg5, hypno_names[i]))
            pthree = i;
          }
          if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
            send_to_char("Syntax: Hypnotize (person) body instruction (pressure one) (pressure two) (pressure three) (instruction)\n\rPossible Pressures", ch);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(ch, ", %s", hypno_names[i]);
            send_to_char(".\n\r", ch);
            return;
          }

          for (int i = 0; i < 25; i++) {
            if (victim->pcdata->imprint_type[i] == 0) {
              victim->pcdata->imprint_type[i] = IMPRINT_BODYINSTRUCTION;
              free_string(victim->pcdata->imprint[i]);
              victim->pcdata->imprint[i] = str_dup(argument);
              victim->pcdata->imprint_pending[i] = -5;
              victim->pcdata->imprint_pressure_one[i] = pone;
              victim->pcdata->imprint_pressure_two[i] = ptwo;
              victim->pcdata->imprint_pressure_three[i] = pthree;
              victim->pcdata->imprinter = ch;
              victim->pcdata->trance = -60;
              printf_to_char(
              ch, "You attempt to instruct %s's motor cortex to make it so their body %s with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
              ch->pcdata->total_money -= 3000;
              printf_to_char(
              victim, "Your motor cortext has recieved and instruction to make it so your body %s.\nTo accept this instruction  use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(victim, ", %s", hypno_names[i]);
              send_to_char(".\n\r", victim);
              return;
            }
          }
        }
        if (!str_cmp(arg6, "compulsion")) {
          argument = one_argument_nouncap(argument, arg3);
          argument = one_argument_nouncap(argument, arg4);
          argument = one_argument_nouncap(argument, arg5);
          for (int i = 1; i <= HYPNO_PITY; i++) {
            if (!str_cmp(arg3, hypno_names[i]))
            pone = i;
            if (!str_cmp(arg4, hypno_names[i]))
            ptwo = i;
            if (!str_cmp(arg5, hypno_names[i]))
            pthree = i;
          }
          if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
            send_to_char("Syntax: Hypnotize (person) body compulsion (pressure one) (pressure two) (pressure three) (instruction)\n\rPossible Pressures", ch);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(ch, ", %s", hypno_names[i]);
            send_to_char(".\n\r", ch);
            return;
          }

          for (int i = 0; i < 25; i++) {
            if (victim->pcdata->imprint_type[i] == 0) {
              victim->pcdata->imprint_type[i] = IMPRINT_BODYCOMPULSION;
              free_string(victim->pcdata->imprint[i]);
              victim->pcdata->imprint[i] = str_dup(argument);
              victim->pcdata->imprint_pending[i] = -5;
              victim->pcdata->imprint_pressure_one[i] = pone;
              victim->pcdata->imprint_pressure_two[i] = ptwo;
              victim->pcdata->imprint_pressure_three[i] = pthree;
              victim->pcdata->imprinter = ch;
              victim->pcdata->trance = -60;
              printf_to_char(
              ch, "You attempt to compel %s's motor cortex to make their body %s with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
              ch->pcdata->total_money -= 3000;
              printf_to_char(
              victim, "Your motor cortex has recieved a compulsion to make it so your body %s.\nTo accept this instruction  use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(victim, ", %s", hypno_names[i]);
              send_to_char(".\n\r", victim);
              return;
            }
          }
        }
      }
      if (!str_cmp(arg2, "triggered")) {
        char arg6[MSL];
        argument = one_argument_nouncap(argument, arg6);
        if (!str_cmp(arg6, "instruction")) {

          argument = one_argument_nouncap(argument, arg3);
          argument = one_argument_nouncap(argument, arg4);
          argument = one_argument_nouncap(argument, arg5);
          for (int i = 1; i <= HYPNO_PITY; i++) {
            if (!str_cmp(arg3, hypno_names[i]))
            pone = i;
            if (!str_cmp(arg4, hypno_names[i]))
            ptwo = i;
            if (!str_cmp(arg5, hypno_names[i]))
            pthree = i;
          }
          if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
            send_to_char("Syntax: Hypnotize [person] triggered instruction [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(ch, ", %s", hypno_names[i]);
            send_to_char(".\n\r", ch);
            return;
          }
          bool eataletter = FALSE;
          char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
          sprintf(tmp, "%s", argument);
          rectalk[0] = 0;
          talktalk[0] = 0;
          bool rectotalk = FALSE;
          for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
            if (tmp[i] == '(') {
              eataletter = TRUE;
              rectotalk = TRUE;
            }
            else if (tmp[i] == ')') {
              rectotalk = FALSE;
            }
            else {
              if (rectotalk == TRUE) {
                sprintf(blah, "%c", tmp[i]);
                strcat(rectalk, blah);
              }
              else {
                if (eataletter == TRUE && tmp[i] == ' ') {
                  eataletter = FALSE;
                }
                else {
                  sprintf(blah, "%c", tmp[i]);
                  strcat(talktalk, blah);
                }
              }
            }
          }
          argument = str_dup(talktalk);
          if (safe_strlen(argument) < 2 || safe_strlen(rectalk) < 2) {
            send_to_char("Syntax: Hypnotize [person] triggered instruction [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(ch, ", %s", hypno_names[i]);
            send_to_char(".\n\r", ch);
            return;
          }
          for (int i = 0; i < 25; i++) {
            if (victim->pcdata->imprint_type[i] == 0) {
              victim->pcdata->imprint_type[i] = IMPRINT_TRIGGEREDINSTRUCTION;
              free_string(victim->pcdata->imprint[i]);
              victim->pcdata->imprint[i] = str_dup(argument);
              free_string(victim->pcdata->imprint_trigger[i]);
              victim->pcdata->imprint_trigger[i] = str_dup(rectalk);
              victim->pcdata->imprint_pending[i] = -5;
              victim->pcdata->imprint_pressure_one[i] = pone;
              victim->pcdata->imprint_pressure_two[i] = ptwo;
              victim->pcdata->imprint_pressure_three[i] = pthree;
              victim->pcdata->imprinter = ch;
              victim->pcdata->trance = -60;
              printf_to_char(ch, "You attempt to instruct %s to %s when they see or hear '%s' with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, rectalk, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
              ch->pcdata->total_money -= 3000;
              printf_to_char(
              victim, "Your subconcious has received a compulsion to %s.\nTo accept this instruction  use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(victim, ", %s", hypno_names[i]);
              send_to_char(".\n\r", victim);
              return;
            }
          }
        }
        if (!str_cmp(arg6, "compulsion")) {
          argument = one_argument_nouncap(argument, arg3);
          argument = one_argument_nouncap(argument, arg4);
          argument = one_argument_nouncap(argument, arg5);
          for (int i = 1; i <= HYPNO_PITY; i++) {
            if (!str_cmp(arg3, hypno_names[i]))
            pone = i;
            if (!str_cmp(arg4, hypno_names[i]))
            ptwo = i;
            if (!str_cmp(arg5, hypno_names[i]))
            pthree = i;
          }
          if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
            send_to_char("Syntax: Hypnotize [person] triggered compulsion [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(ch, ", %s", hypno_names[i]);
            send_to_char(".\n\r", ch);
          }
          bool eataletter = FALSE;
          char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
          sprintf(tmp, "%s", argument);
          rectalk[0] = 0;
          talktalk[0] = 0;
          bool rectotalk = FALSE;
          for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
            if (tmp[i] == '(') {
              eataletter = TRUE;
              rectotalk = TRUE;
            }
            else if (tmp[i] == ')') {
              rectotalk = FALSE;
            }
            else {
              if (rectotalk == TRUE) {
                sprintf(blah, "%c", tmp[i]);
                strcat(rectalk, blah);
              }
              else {
                if (eataletter == TRUE && tmp[i] == ' ') {
                  eataletter = FALSE;
                }
                else {
                  sprintf(blah, "%c", tmp[i]);
                  strcat(talktalk, blah);
                }
              }
            }
          }
          argument = str_dup(talktalk);
          if (safe_strlen(argument) < 2 || safe_strlen(rectalk) < 2) {
            send_to_char("Syntax: Hypnotize [person] triggered compulsion [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
            for (int i = 1; i <= HYPNO_PITY; i++)
            printf_to_char(ch, ", %s", hypno_names[i]);
            send_to_char(".\n\r", ch);
          }


          for (int i = 0; i < 25; i++) {
            if (victim->pcdata->imprint_type[i] == 0) {
              victim->pcdata->imprint_type[i] = IMPRINT_TRIGGEREDCOMPULSION;
              free_string(victim->pcdata->imprint[i]);
              victim->pcdata->imprint[i] = str_dup(argument);
              free_string(victim->pcdata->imprint_trigger[i]);
              victim->pcdata->imprint_trigger[i] = str_dup(rectalk);
              victim->pcdata->imprint_pending[i] = -5;
              victim->pcdata->imprint_pressure_one[i] = pone;
              victim->pcdata->imprint_pressure_two[i] = ptwo;
              victim->pcdata->imprint_pressure_three[i] = pthree;
              victim->pcdata->imprinter = ch;
              victim->pcdata->trance = -60;
              printf_to_char(ch, "You attempt to instruct %s to %s when they see or hear '%s' with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, rectalk, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
              ch->pcdata->total_money -= 4000;
              printf_to_char(
              victim, "Your subconcious has received a compulsion to %s.\nTo accept this instruction use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(victim, ", %s", hypno_names[i]);
              send_to_char(".\n\r", victim);
              return;
            }
          }
        }
        if (!str_cmp(arg6, "body")) {
          char arg7[MSL];
          argument = one_argument_nouncap(argument, arg7);
          if (!str_cmp(arg7, "instruction")) {

            argument = one_argument_nouncap(argument, arg3);
            argument = one_argument_nouncap(argument, arg4);
            argument = one_argument_nouncap(argument, arg5);
            for (int i = 1; i <= HYPNO_PITY; i++) {
              if (!str_cmp(arg3, hypno_names[i]))
              pone = i;
              if (!str_cmp(arg4, hypno_names[i]))
              ptwo = i;
              if (!str_cmp(arg5, hypno_names[i]))
              pthree = i;
            }
            if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
              send_to_char("Syntax: Hypnotize [person] triggered body instruction [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(ch, ", %s", hypno_names[i]);
              send_to_char(".\n\r", ch);
              return;
            }
            bool eataletter = FALSE;
            char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
            sprintf(tmp, "%s", argument);
            rectalk[0] = 0;
            talktalk[0] = 0;
            bool rectotalk = FALSE;
            for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
              if (tmp[i] == '(') {
                eataletter = TRUE;
                rectotalk = TRUE;
              }
              else if (tmp[i] == ')') {
                rectotalk = FALSE;
              }
              else {
                if (rectotalk == TRUE) {
                  sprintf(blah, "%c", tmp[i]);
                  strcat(rectalk, blah);
                }
                else {
                  if (eataletter == TRUE && tmp[i] == ' ') {
                    eataletter = FALSE;
                  }
                  else {
                    sprintf(blah, "%c", tmp[i]);
                    strcat(talktalk, blah);
                  }
                }
              }
            }
            argument = str_dup(talktalk);
            if (safe_strlen(argument) < 2 || safe_strlen(rectalk) < 2) {
              send_to_char("Syntax: Hypnotize [person] triggered body instruction [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(ch, ", %s", hypno_names[i]);
              send_to_char(".\n\r", ch);
              return;
            }


            for (int i = 0; i < 25; i++) {
              if (victim->pcdata->imprint_type[i] == 0) {
                victim->pcdata->imprint_type[i] = IMPRINT_TRIGGEREDBODYINSTRUCTION;
                free_string(victim->pcdata->imprint[i]);
                victim->pcdata->imprint[i] = str_dup(argument);
                free_string(victim->pcdata->imprint_trigger[i]);
                victim->pcdata->imprint_trigger[i] = str_dup(rectalk);
                victim->pcdata->imprint_pending[i] = -5;
                victim->pcdata->imprint_pressure_one[i] = pone;
                victim->pcdata->imprint_pressure_two[i] = ptwo;
                victim->pcdata->imprint_pressure_three[i] = pthree;
                victim->pcdata->imprinter = ch;
                victim->pcdata->trance = -60;
                printf_to_char(ch, "You attempt to instruct %s's motor cortext so that their body %s when they see or hear '%s' with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, rectalk, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
                ch->pcdata->total_money -= 3000;
                printf_to_char(
                victim, "Your motor cortext has recieved and instruction, attempting to make it so you body %s.\nTo accept this instruction  use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
                for (int i = 1; i <= HYPNO_PITY; i++)
                printf_to_char(victim, ", %s", hypno_names[i]);
                send_to_char(".\n\r", victim);
                return;
              }
            }
          }
          if (!str_cmp(arg7, "compulsion")) {

            argument = one_argument_nouncap(argument, arg3);
            argument = one_argument_nouncap(argument, arg4);
            argument = one_argument_nouncap(argument, arg5);
            for (int i = 1; i <= HYPNO_PITY; i++) {
              if (!str_cmp(arg3, hypno_names[i]))
              pone = i;
              if (!str_cmp(arg4, hypno_names[i]))
              ptwo = i;
              if (!str_cmp(arg5, hypno_names[i]))
              pthree = i;
            }
            if (pone < 1 || ptwo < 1 || pthree < 1 || safe_strlen(argument) < 2) {
              send_to_char("Syntax: Hypnotize [person] triggered body compulsion [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(ch, ", %s", hypno_names[i]);
              send_to_char(".\n\r", ch);
              return;
            }
            bool eataletter = FALSE;
            char tmp[MSL], blah[MSL], rectalk[MSL], talktalk[MSL];
            sprintf(tmp, "%s", argument);
            rectalk[0] = 0;
            talktalk[0] = 0;
            bool rectotalk = FALSE;
            for (int i = 0; tmp[i] != '\0' && i < (int)(safe_strlen(tmp)); i++) {
              if (tmp[i] == '(') {
                eataletter = TRUE;
                rectotalk = TRUE;
              }
              else if (tmp[i] == ')') {
                rectotalk = FALSE;
              }
              else {
                if (rectotalk == TRUE) {
                  sprintf(blah, "%c", tmp[i]);
                  strcat(rectalk, blah);
                }
                else {
                  if (eataletter == TRUE && tmp[i] == ' ') {
                    eataletter = FALSE;
                  }
                  else {
                    sprintf(blah, "%c", tmp[i]);
                    strcat(talktalk, blah);
                  }
                }
              }
            }
            argument = str_dup(talktalk);
            if (safe_strlen(argument) < 2 || safe_strlen(rectalk) < 2) {
              send_to_char("Syntax: Hypnotize [person] triggered body compulsion [pressure one] [pressure two] [pressure three] ([trigger]) [instruction]\n\rPossible Pressures", ch);
              for (int i = 1; i <= HYPNO_PITY; i++)
              printf_to_char(ch, ", %s", hypno_names[i]);
              send_to_char(".\n\r", ch);
            }
            ch->pcdata->total_money -= 4000;
            for (int i = 0; i < 25; i++) {
              if (victim->pcdata->imprint_type[i] == 0) {
                victim->pcdata->imprint_type[i] = IMPRINT_TRIGGEREDBODYCOMPULSION;
                free_string(victim->pcdata->imprint[i]);
                victim->pcdata->imprint[i] = str_dup(argument);
                free_string(victim->pcdata->imprint_trigger[i]);
                victim->pcdata->imprint_trigger[i] = str_dup(rectalk);
                victim->pcdata->imprint_pending[i] = -5;
                victim->pcdata->imprint_pressure_one[i] = pone;
                victim->pcdata->imprint_pressure_two[i] = ptwo;
                victim->pcdata->imprint_pressure_three[i] = pthree;
                victim->pcdata->imprinter = ch;
                victim->pcdata->trance = -60;
                printf_to_char(ch, "You attempt to compel %s's motor cortex so that their body %s from when they see or hear '%s' with a combination of %s, %s, and %s.\n\r", PERS(victim, ch), argument, rectalk, hypno_names[pone], hypno_names[ptwo], hypno_names[pthree]);
                printf_to_char(
                victim, "Your motor cortex has recieved a compulsion, attempting to make it so your body %s.\nTo accept this instruction  use imprint accept, to try to resist use imprint resist (pressure one) (pressure two) (pressure three) where each pressure is a type of manipulation most likely to make your character succumb to the instruction and are given in order of strength. Possible pressures are", argument);
                for (int i = 1; i <= HYPNO_PITY; i++)
                printf_to_char(victim, ", %s", hypno_names[i]);
                send_to_char(".\n\r", victim);
                return;
              }
            }
          }
        }
      }

    }
    if(!str_cmp(arg1, "list"))
    {
      char outbuf[MSL];
      outbuf[0] = '\0';
      for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
      it != MatchVect.end(); ++it) {
        MATCH_TYPE *match = (*it);
        if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
        {
          PROFILE_TYPE *profile = profile_lookup(match->nametwo);
          if(profile == NULL)
          continue;
          if(profile->last_logon < (current_time - 3500*24*3))
          continue;
          char buf[MSL];
          if(match->chat_active_one == 1 && match->rp_active_one)
          {
            sprintf(buf, "%s[Chat:Meet], ", profile->display_handle);
            strcat(outbuf, buf);
          }
          else if(match->chat_active_one == 1)
          {
            sprintf(buf, "%s[Chat], ", profile->display_handle);
            strcat(outbuf, buf);
          }
          else if(match->rp_active_one == 1)
          {
            sprintf(buf, "%s[RP], ", profile->display_handle);
            strcat(outbuf, buf);
          }
          else
          {
            sprintf(buf, "%s, ", profile->display_handle);
            strcat(outbuf, buf);
          }
        }
        else if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
        {
          PROFILE_TYPE *profile = profile_lookup(match->nameone);
          if(profile == NULL)
          continue;
          if(profile->last_logon < (current_time - 3500*24*3))
          continue;
          char buf[MSL];
          if(match->chat_active_two == 1 && match->rp_active_two)
          {
            sprintf(buf, "%s[Chat:Meet], ", profile->display_handle);
            strcat(outbuf, buf);
          }
          else if(match->chat_active_two == 1)
          {
            sprintf(buf, "%s[Chat], ", profile->display_handle);
            strcat(outbuf, buf);
          }
          else if(match->rp_active_two == 1)
          {
            sprintf(buf, "%s[RP], ", profile->display_handle);
            strcat(outbuf, buf);
          }
          else
          {
            sprintf(buf, "%s, ", profile->display_handle);
            strcat(outbuf, buf);
          }
        }
      }
      send_to_char(outbuf, ch);
      return;
    }
    if(!str_cmp(arg1, "chatwith"))
    {
      for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
      it != MatchVect.end(); ++it) {
        MATCH_TYPE *match = (*it);
        if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
        {
          PROFILE_TYPE *profile = profile_lookup(match->nametwo);
          if(profile == NULL)
          continue;
          if(strcasestr(argument, profile->handle) != NULL)
          {
            if(match->chat_active_one == 1)
            {
              match->chat_active_one = 0;
            }
            else
            {
              match->chat_active_one = 1;
            }
          }
        }
        else if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
        {
          PROFILE_TYPE *profile = profile_lookup(match->nameone);
          if(profile == NULL)
          continue;
          if(strcasestr(argument, profile->handle) != NULL)
          {
            if(match->chat_active_two == 1)
            {
              match->chat_active_two = 0;
            }
            else
            {
              match->chat_active_two = 1;
            }
          }
        }
      }
      send_to_char("Chat status updated.\n\r", ch);
    }
    if(!str_cmp(arg1, "meetwith"))
    {
      for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
      it != MatchVect.end(); ++it) {
        MATCH_TYPE *match = (*it);
        if(!str_cmp(ch->name, match->nameone) && match->status_one == 1 && match->status_two == 1)
        {
          PROFILE_TYPE *profile = profile_lookup(match->nametwo);
          if(profile == NULL)
          continue;
          if(strcasestr(argument, profile->handle) != NULL)
          {
            if(match->rp_active_one == 1)
            {
              match->rp_active_one = 0;
            }
            else
            {
              match->rp_active_one = 1;
            }
          }
        }
        else if(!str_cmp(ch->name, match->nametwo) && match->status_one == 1 && match->status_two == 1)
        {
          PROFILE_TYPE *profile = profile_lookup(match->nameone);
          if(profile == NULL)
          continue;
          if(strcasestr(argument, profile->handle) != NULL)
          {
            if(match->rp_active_two == 1)
            {
              match->rp_active_two = 0;
            }
            else
            {
              match->rp_active_two = 1;
            }
          }
        }
      }
      send_to_char("Meeting status updated.\n\r", ch);
    }
    if(!str_cmp(arg1, "rateevent"))
    {
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Myhaven rateevent (0.1-5.0).\n\r", ch);
        return;
      }
      if(IS_IMMORTAL(ch))
      {
        char arg2[MSL];
        argument = one_argument_nouncap(argument, arg2);
        double rating = 0;
        rating = atof(argument);
        PROFILE_TYPE *profile = profile_lookup(arg2);
        if(profile == NULL)
        {
          send_to_char("That person does not have a profile.\n\r", ch);
          return;
        }
        profile->party_boost = (int)(rating*10);
        send_to_char("Done.\n\r", ch);
        return;
      }
      double rating = 0;
      if(safe_strlen(argument) > 0)
      {
        rating = atof(argument);
      }
      if(rating <= 0 || rating > 5.0)
      {
        send_to_char("You must rate between 0.1 and 5.0\n\r", ch);
        return;
      }
      char logs[MSL];
      PROFILE_TYPE * char_profile = profile_lookup(ch->name);
      for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
      it != MatchVect.end(); ++it) {
        if(!str_cmp(ch->name, (*it)->nameone))
        {
          CHAR_DATA *target = get_char_world_pc((*it)->nametwo);
          if((*it)->rate_party_two == 1)
          {
            PROFILE_TYPE *profile = profile_lookup((*it)->nametwo);
            if((*it)->rate_party_type_two == CALENDAR_PARTY)
            {
              profile->party_total += (int)(rating*10);
              profile->party_count++;
            }
            if((*it)->rate_party_type_two == CALENDAR_SOCIAL)
            {
              profile->event_total += (int)(rating*10);
              profile->event_count++;
            }
            if((*it)->rate_party_type_two == CALENDAR_PERFORMANCE)
            {
              profile->perf_total += (int)(rating*10);
              profile->perf_count++;
            }
            char_profile->prating_total += (int)(rating*10);
            char_profile->prating_count += 1;
            sprintf(logs, "RATING EVENT: %s rates %s's event %s: %d", ch->name, (*it)->nametwo, (*it)->party_two_title, (int)(rating*10));
            log_string(logs);
            if(target != NULL && rating >= 4.0 && char_profile->rating >= 40)
            {
              give_intel(target, 100);
            }
            (*it)->rate_party_two = 0;
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
        else if(!str_cmp(ch->name, (*it)->nametwo))
        {
          CHAR_DATA *target = get_char_world_pc((*it)->nameone);
          if((*it)->rate_party_one == 1 && (*it)->rate_party_two == 0)
          {
            PROFILE_TYPE *profile = profile_lookup((*it)->nameone);
            if((*it)->rate_party_type_one == CALENDAR_PARTY)
            {
              profile->party_total += (int)(rating*10);
              profile->party_count++;
            }
            if((*it)->rate_party_type_one == CALENDAR_SOCIAL)
            {
              profile->event_total += (int)(rating*10);
              profile->event_count++;
            }
            if((*it)->rate_party_type_one == CALENDAR_PERFORMANCE)
            {
              profile->perf_total += (int)(rating*10);
              profile->perf_count++;
            }
            char_profile->prating_total += (int)(rating*10);
            char_profile->prating_count += 1;
            sprintf(logs, "RATING EVENT: %s rates %s's event %s: %d", ch->name, (*it)->nametwo, (*it)->party_two_title, (int)(rating*10));
            log_string(logs);
            if(target != NULL && rating >= 4.0 && char_profile->rating >= 40)
            {
              give_intel(target, 100);
            }
            (*it)->rate_party_one = 0;
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("You haven't been to any events you can rate.\n\r", ch);
    }
    if(!str_cmp(arg1, "rate"))
    {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      char logs[MSL];
      if(IS_IMMORTAL(ch))
      {
        double rating = 0;
        rating = atof(argument);
        PROFILE_TYPE *profile = profile_handle_lookup(arg2);
        if(profile == NULL)
        {
          profile = profile_lookup(arg2);
          if(profile == NULL)
          {
            send_to_char("That person does not have a profile.\n\r", ch);
            return;
          }
        }
        profile->rating_boost = (int)(rating*10);
        send_to_char("Done.\n\r", ch);
        return;
      }

      if(safe_strlen(arg2) < 3)
      {
        send_to_char("Syntax: Myhaven rate (handle).\n\r", ch);
        return;
      }
      PROFILE_TYPE * char_profile = profile_lookup(ch->name);
      PROFILE_TYPE *profile = profile_handle_lookup(arg2);
      if(profile == NULL)
      {
        send_to_char("That person does not have a profile.\n\r", ch);
        return;
      }
      double rating = 0;
      if(safe_strlen(argument) > 0)
      {
        rating = atof(argument);
      }
      if(rating <= 0 || rating > 5.0)
      {
        send_to_char("You must rate between 0.1 and 5.0\n\r", ch);
        return;
      }
      if(IS_IMMORTAL(ch))
      {
        profile->rating_boost = (int)(rating*10);
        send_to_char("Done.\n\r", ch);
        return;
      }

      MATCH_TYPE *match = match_find_by_name(ch->name, profile->name);
      if(match == NULL)
      {
        send_to_char("You are not matched with that person.\n\r", ch);
        return;
      }
      if(match->status_one != 1 || match->status_two != 1)
      {
        send_to_char("You are not matched with that person.\n\r", ch);
        return;
      }
      if(!str_cmp(ch->name, match->nameone))
      {
        if(match->chat_count >= 50 && match->score_two_manual_chat == 0)
        {
          sprintf(logs, "RATING CHAT: %s rates %s: %d", ch->name, match->nametwo, (int)(rating*10));
          log_string(logs);

          char_profile->grating_total += (int)(rating*10);
          char_profile->grating_count += 1;

          match->score_two_manual_chat = (int)(rating*10);
          send_to_char("Done.\n\r", ch);
          return;
        }
        if(match->rp_count >= 25 && match->score_two_manual_inperson == 0)
        {
          sprintf(logs, "RATING RP: %s rates %s: %d", ch->name, match->nametwo, (int)(rating*10));
          log_string(logs);
          char_profile->grating_total += (int)(rating*10);
          char_profile->grating_count += 1;


          match->score_two_manual_inperson = (int)(rating*10);
          send_to_char("Done.\n\r", ch);
          return;
        }
        send_to_char("They aren't yet ready to be rated or you've already rated them.\n\r", ch);
        return;
      }
      if(!str_cmp(ch->name, match->nametwo))
      {
        if(match->chat_count >= 50 && match->score_one_manual_chat == 0)
        {
          sprintf(logs, "RATING CHAT: %s rates %s: %d", ch->name, match->nameone, (int)(rating*10));
          log_string(logs);
          char_profile->grating_total += (int)(rating*10);
          char_profile->grating_count += 1;


          match->score_one_manual_chat = (int)(rating*10);
          send_to_char("Done.\n\r", ch);
          return;
        }
        if(match->rp_count >= 25 && match->score_one_manual_inperson == 0)
        {
          sprintf(logs, "RATING RP: %s rates %s: %d", ch->name, match->nameone, (int)(rating*10));
          log_string(logs);
          char_profile->grating_total += (int)(rating*10);
          char_profile->grating_count += 1;


          match->score_one_manual_inperson = (int)(rating*10);
          send_to_char("Done.\n\r", ch);
          return;
        }
        send_to_char("They aren't yet ready to be rated or you've already rated them.\n\r", ch);
        return;
      }

    }

    if(!str_cmp(arg1, "viewprofile") || !str_cmp(arg1, "profile") || !str_cmp(arg1, "view"))
    {
      char buf[MSL];
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(safe_strlen(argument) < 3)
      {
        sprintf(buf, "%s(`228%s`x)\n%s\n\n\rAbout Me: %s\n\rQuote: %s\n\rJoke: %s\n\rAttracted To: %s\n\r", char_profile->display_handle, display_profile_rating(char_profile), char_profile->photo, char_profile->profile, char_profile->quote, char_profile->joke, char_profile->prefs);
        send_to_char(buf, ch);
        send_to_char("Syntax: Myhaven Viewprofile (handle).\n\r", ch);
        return;
      }
      PROFILE_TYPE *profile = profile_handle_lookup(argument);
      if(profile == NULL)
      {
        send_to_char("That person does not have a profile.\n\r", ch);
        return;
      }
      MATCH_TYPE *match = match_find_by_name(ch->name, profile->name);
      if(match != NULL && match->datematch_one == 1 && match->datematch_two == 1)
      sprintf(buf, "%s\n\n%s(`228%s`x)\n\rAbout Me: %s\n\rQuote: %s\n\rJoke: %s\n\rAttracted To: %s\n\r", profile->photo, profile->display_handle, display_profile_rating(profile), profile->profile, profile->quote, profile->joke, profile->prefs);
      else
      sprintf(buf, "%s\n\n%s(`228%s`x)\n\rAbout Me: %s\n\rQuote: %s\n\rJoke: %s\n\r", profile->photo, profile->display_handle, display_profile_rating(profile), profile->profile, profile->quote, profile->joke);
      send_to_char(buf, ch);
      if(char_profile->plus == 1)
      {
        printf_to_char(ch, "Wealth: %d\n\rEducation: %d.\n\r", profile->wealth, profile->education);
        if(profile->core_symbol > 0)
        {
          FACTION_TYPE *fac = clan_lookup(profile->core_symbol);
          if(fac != NULL)
          printf_to_char(ch, "Faction: %s.\n\r", fac->name);
        }
        if(profile->sect_symbol > 0)
        {
          FACTION_TYPE *fac = clan_lookup(profile->sect_symbol);
          if(fac != NULL)
          printf_to_char(ch, "Sect: %s.\n\r", fac->name);
        }
        if(profile->cult_symbol > 0)
        {
          FACTION_TYPE *fac = clan_lookup(profile->cult_symbol);
          if(fac != NULL)
          printf_to_char(ch, "Cult: %s.\n\r", fac->name);
        }
      }
      if(match != NULL && match->status_one == 1 && match->status_two == 1)
      {
        sprintf(buf, "You are matched with %s on MyHaven as %s.\n\r", profile->display_handle, match_string(match));
        send_to_char(buf, ch);
      }
      return;
    }
    if(!str_cmp(arg1, "plus"))
    {
      PROFILE_TYPE *char_profile = profile_lookup(ch->name);
      if(char_profile == NULL)
      {
        send_to_char("Use myhaven setup first.\n\r", ch);
        return;
      }
      if(char_profile->plus == 0)
      {
        char_profile->plus = 1;
        send_to_char("You are now a plus member.\n\r", ch);
        return;
      }
      else
      {
        char_profile->plus = 0;
        send_to_char("You are no longer a plus member.\n\r", ch);
        return;
      }

    }
    if(!str_cmp(arg1, "browse"))
    {
      std::random_device rd;
      // Create a random number generator
      std::mt19937 g(rd());
      // Shuffle the vector
      std::shuffle(ProfileVect.begin(), ProfileVect.end(), g);
      std::sort(ProfileVect.begin(), ProfileVect.end(), compareLastBrowsed);

      for (vector<PROFILE_TYPE *>::iterator it = ProfileVect.begin();
      it != ProfileVect.end(); ++it) {
        if((*it)->valid == FALSE)
        continue;

        if(get_char_world_pc((*it)->name) == NULL && number_percent() % 3 != 0)
        continue;


        if((*it)->last_active < current_time - (3600*24*3))
        continue;


        if((*it)->last_logon < current_time - (3600*24*3))
        continue;

        if(safe_strlen((*it)->profile) < 2)
        continue;

        if(!str_cmp(ch->name, (*it)->name))
        continue;

        MATCH_TYPE * match = match_find_by_name(ch->name, (*it)->name);

        if(match != NULL && match->status_one == 1 && match->status_two == 1)
        continue;

        if(match != NULL && !str_cmp(ch->name, match->nameone) && match->status_one == 1 && number_percent() % 2 == 0)
        continue;

        if(match != NULL && !str_cmp(ch->name, match->nametwo) && match->status_two == 1 && number_percent() % 2 == 0)
        continue;


        PROFILE_TYPE *profile = (*it);
        profile->last_browsed = current_time;
        char buf[MSL];
        sprintf(buf, "%s\n\n%s(%s)\n\rAbout Me: %s\n\rQuote: %s\n\rJoke: %s\n\r", profile->photo, profile->display_handle, display_profile_rating(profile), profile->profile, profile->quote, profile->joke);
        send_to_char(buf, ch);
        return;
      }
    }

    if(!str_cmp(arg1, "rematch"))
    {

      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      PROFILE_TYPE *profile = profile_handle_lookup(arg2);
      if(profile == NULL)
      {
        send_to_char("That person does not have a profile.\n\r", ch);
        return;
      }
      MATCH_TYPE *match = match_find_by_name(ch->name, profile->name);
      if(match == NULL)
      {
        send_to_char("You aren't currently matched.\n\r", ch);
        return;
      }
      if(!str_cmp(match->nameone, ch->name))
      {
        match->friendmatch_one = 0;
        match->datematch_one = 0;
        match->profmatch_one = 0;
        if(strcasestr(argument, "friend") != NULL)
        match->friendmatch_one = 1;
        if(strcasestr(argument, "date") != NULL || strcasestr(argument, "dating") != NULL)
        match->datematch_one = 0;
        if(strcasestr(argument, "professional") != NULL || strcasestr(argument, "networking") != NULL)
        match->profmatch_one = 0;
        send_to_char("Done.\n\r", ch);
        return;
      }
      else if(!str_cmp(match->nameone, ch->name))
      {
        match->friendmatch_two = 0;
        match->datematch_two = 0;
        match->profmatch_two = 0;
        if(strcasestr(argument, "friend") != NULL)
        match->friendmatch_two = 1;
        if(strcasestr(argument, "date") != NULL || strcasestr(argument, "dating") != NULL)
        match->datematch_two = 0;
        if(strcasestr(argument, "professional") != NULL || strcasestr(argument, "networking") != NULL)
        match->profmatch_two = 0;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    if(!str_cmp(arg1, "unmatch"))
    {

      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      PROFILE_TYPE *profile = profile_handle_lookup(arg2);
      if(profile == NULL)
      {
        send_to_char("That person does not have a profile.\n\r", ch);
        return;
      }
      MATCH_TYPE *match = match_find_by_name(ch->name, profile->name);
      if(match == NULL)
      {
        send_to_char("You aren't currently matched.\n\r", ch);
        return;
      }
      if(!str_cmp(match->nameone, ch->name))
      {
        match->status_one = 0;
        match->friendmatch_one = 0;
        match->datematch_one = 0;
        match->profmatch_one = 0;
        send_to_char("Unmatched.\n\r", ch);
        return;
      }
      else if(!str_cmp(match->nametwo, ch->name))
      {
        match->status_two = 0;
        match->friendmatch_two = 0;
        match->datematch_two = 0;
        match->profmatch_two = 0;
        send_to_char("Unmatched.\n\r", ch);
        return;
      }
    }
    if(!str_cmp(arg1, "match"))
    {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      PROFILE_TYPE *profile = profile_handle_lookup(arg2);
      if(profile == NULL)
      {
        send_to_char("That person does not have a profile.\n\r", ch);
        return;
      }
      int fmatch = 0;
      int dmatch = 0;
      int pmatch = 0;
      if(strcasestr(argument, "friend") != NULL)
      fmatch = 1;
      if(strcasestr(argument, "date") != NULL || strcasestr(argument, "dating") != NULL)
      dmatch = 1;
      if(strcasestr(argument, "professional") != NULL || strcasestr(argument, "networking") != NULL)
      pmatch = 1;
      if(fmatch == 0 && dmatch == 0 && pmatch == 0)
      {
        send_to_char("Syntax: Myhaven Match (handle) (Any combination of: friend/date/professional).\n\r", ch);
        return;
      }
      MATCH_TYPE *match = match_find_by_name(ch->name, profile->name);
      if(match == NULL)
      {
        MATCH_TYPE *newmatch = new_match();
        free_string(newmatch->nameone);
        newmatch->nameone = str_dup(ch->name);
        free_string(newmatch->nametwo);
        newmatch->nametwo = str_dup(profile->name);
        newmatch->friendmatch_one = fmatch;
        newmatch->datematch_one = dmatch;
        newmatch->profmatch_one = pmatch;
        newmatch->status_one = 1;
        MatchVect.push_back(newmatch);
        send_to_char("Match created.\n\r", ch);
        CHAR_DATA *victim = get_char_world_pc(profile->name);
        if(victim != NULL)
        {
          send_to_char("Someone matched with you on MyHaven.\n\r", victim);
        }
      }
      else
      {
        if(match->status_one == 1 && match->status_two == 1)
        {
          send_to_char("You are already matched with that person.\n\r", ch);
          return;
        }
        if(!str_cmp(match->nameone, ch->name) && match->status_one == 1)
        {
          send_to_char("You are already matched with that person.\n\r", ch);
          return;
        }
        match->friendmatch_two = fmatch;
        match->datematch_two = dmatch;
        match->profmatch_two = pmatch;
        match->status_two = 1;
        char buf[MSL];
        sprintf(buf, "");
        bool found = FALSE;
        if(match->friendmatch_one == 1 && match->friendmatch_two == 1)
        {
          strcat(buf, "friends, ");
          found = TRUE;
        }
        if(match->datematch_one == 1 && match->datematch_two == 1)
        {
          strcat(buf, "dating, ");
          found = TRUE;
        }
        if(match->profmatch_one == 1 && match->profmatch_two == 1)
        {
          strcat(buf, "professional, ");
          found = TRUE;
        }
        if(found == FALSE)
        {
          match->status_two = 0;
          return;
        }
        PROFILE_TYPE *char_profile = profile_lookup(ch->name);
        printf_to_char(ch, "You have matched with %s on MyHaven as %s. Use text %s (message) to start chatting.\n\r", profile->handle, buf, profile->handle);
        CHAR_DATA *victim = get_char_world_pc(profile->name);
        if(victim != NULL)
        {
          printf_to_char(victim, "%s has matched with you on MyHaven as %s. Use text %s (message) to start chatting.\n\r", char_profile->handle, buf, char_profile->handle);
        }
        else
        {
          char msg[MSL];
          sprintf(msg, "%s has matched with you on MyHaven as %s.\n\r", char_profile->handle, buf);
          offline_message(profile->name, msg);
        }
      }
    }
  }



  bool listening_chatroom(CHAR_DATA *ch, int vnum)
  {
    for(int i=0;i<50;i++)
    {
      if(ch->pcdata->in_chatroom[i] == vnum)
      return TRUE;
    }
    if(ch->pcdata->active_chatroom == vnum)
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_chatroom)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    PROFILE_TYPE *profile = profile_lookup(ch->name);
    if(profile == NULL)
    {
      send_to_char("You must have a profile to use chatrooms.\n\r", ch);
      return;
    }

    if(!str_cmp(arg1, "create"))
    {
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Chat Create (room name) (description).\n\r", ch);
        return;
      }
      if(ch->played/3600 < 100)
      {
        send_to_char("You must have played for at least 100 hours to create a chatroom.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      CHATROOM_TYPE * chat = chatroom_lookup_name(arg2);
      if(chat != NULL)
      {
        send_to_char("That chatroom already exists.\n\r", ch);
        return;
      }
      CHATROOM_TYPE * newchat = new_chatroom();
      int maxvnum = 0;
      for (vector<CHATROOM_TYPE *>::iterator it = ChatroomVect.begin();
      it != ChatroomVect.end(); ++it) {
        if((*it)->vnum > maxvnum)
        maxvnum = (*it)->vnum;
      }
      newchat->vnum = maxvnum + 1;
      free_string(newchat->name);
      newchat->name = str_dup(arg2);
      free_string(newchat->description);
      newchat->description = str_dup(argument);
      ChatroomVect.push_back(newchat);
      send_to_char("Chatroom created.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "listen"))
    {
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Chat Listen (room name).\n\r", ch);
        return;
      }
      CHATROOM_TYPE * chat = chatroom_lookup_name(argument);
      if(chat == NULL)
      {
        send_to_char("That chatroom does not exist.\n\r", ch);
        return;
      }
      for(int i=0;i<50;i++)
      {
        if(ch->pcdata->in_chatroom[i] == chat->vnum)
        {
          send_to_char("You are already listening to that chatroom.\n\r", ch);
          return;
        }
      }
      for(int i=0;i<50;i++)
      {
        if(ch->pcdata->in_chatroom[i] == 0)
        {
          ch->pcdata->in_chatroom[i] = chat->vnum;
          send_to_char("You are now listening to that chatroom.\n\r", ch);
          return;
        }
      }
      return;
    }
    if(!str_cmp(arg1, "tuneout"))
    {
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Chat Tuneout (room name).\n\r", ch);
        return;
      }
      CHATROOM_TYPE * chat = chatroom_lookup_name(argument);
      if(chat == NULL)
      {
        send_to_char("That chatroom does not exist.\n\r", ch);
        return;
      }
      if(ch->pcdata->active_chatroom == chat->vnum)
      {
        ch->pcdata->active_chatroom = 0;
        send_to_char("You stop sending to that chatroom.\n\r", ch);
        return;
      }
      for(int i=0;i<50;i++)
      {
        if(ch->pcdata->in_chatroom[i] == chat->vnum)
        {
          ch->pcdata->in_chatroom[i] = 0;
          send_to_char("You are no longer listening to that chatroom.\n\r", ch);
          return;
        }
      }
      send_to_char("You are not listening to that chatroom.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "list"))
    {
      for (vector<CHATROOM_TYPE *>::iterator it = ChatroomVect.begin();
      it != ChatroomVect.end(); ++it) {
        if((*it)->valid == FALSE)
        continue;
        char buf[MSL];
        if(ch->pcdata->active_chatroom == (*it)->vnum)
        sprintf(buf, "[`045Active`x]%s: %s\n\r", (*it)->name, (*it)->description);
        else if(listening_chatroom(ch, (*it)->vnum))
        sprintf(buf, "[`035Listening`x]%s: %s\n\r", (*it)->name, (*it)->description);
        else
        sprintf(buf, "%s: %s\n\r", (*it)->name, (*it)->description);
        send_to_char(buf, ch);
      }
      return;
    }
    if(!str_cmp(arg1, "join"))
    {
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Chat Join (room name).\n\r", ch);
        return;
      }
      CHATROOM_TYPE * chat = chatroom_lookup_name(argument);
      if(chat == NULL)
      {
        send_to_char("That chatroom does not exist.\n\r", ch);
        return;
      }
      if(ch->pcdata->active_chatroom == chat->vnum)
      {
        send_to_char("You are already in that chatroom.\n\r", ch);
        return;
      }
      ch->pcdata->active_chatroom = chat->vnum;
      send_to_char("You have joined the chatroom.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "history"))
    {
      if(safe_strlen(argument) < 3)
      {
        send_to_char("Syntax: Chat Tuneout (room name).\n\r", ch);
        return;
      }
      CHATROOM_TYPE * chat = chatroom_lookup_name(argument);
      if(chat == NULL)
      {
        send_to_char("That chatroom does not exist.\n\r", ch);
        return;
      }
      if(chat->history == NULL)
      {
        send_to_char("There is no history for that chatroom.\n\r", ch);
        return;
      }
      send_to_char(chat->history, ch);
      return;
    }
    char buf[MSL];
    sprintf(buf, "%s %s", arg1, argument);
    do_function(ch, &do_chat, buf);
  }

  _DOFUN(do_chat)
  {
    PROFILE_TYPE *profile = profile_lookup(ch->name);
    if(profile == NULL)
    {
      send_to_char("You must have a profile to use chatrooms.\n\r", ch);
      return;
    }
    if(!cell_signal(ch))
    {
      send_to_char("You have no cell signal.\n\r", ch);
      return;
    }
    if(ch->pcdata->active_chatroom == 0)
    {
      send_to_char("You are not in a chatroom. Use chatroom join (name) to join one\n\r", ch);
      return;
    }
    CHATROOM_TYPE * chat = chatroom_lookup_vnum(ch->pcdata->active_chatroom);
    if(chat == NULL)
    {
      send_to_char("You are not in a chatroom. Use chatroom join (name) to join one\n\r", ch);
      return;
    }
    if(safe_strlen(argument) < 3)
    {
      send_to_char("Syntax: Chat (message).\n\r", ch);
      return;
    }
    char buf[MSL];
    char hbuf[MSL];
    sprintf(buf, "[%s] %s: %s\n\r", chat->name, profile->display_handle, argument);
    sprintf(hbuf, "[%s] %s: %s", newtexttime(), profile->display_handle, argument);
    char * tmpl = appendLine(chat->history, str_dup(hbuf));
    free_string(chat->history);
    chat->history = str_dup(tmpl);
    chat->last_msg = current_time;
    rpreward(ch, argument, TRUE, 1);
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to)) {
          continue;
        }
        if(!cell_signal(to))
        {
          continue;
        }
        if (spammer(ch) && !same_player(ch, to) && !IS_IMMORTAL(to))
        continue;

        if(listening_chatroom(to, chat->vnum) && holding_phone(to))
        {

          send_to_char(buf, to);
        }

      }
    }
  }

  int influencer_score(CHAR_DATA *ch)
  {
    char buf[MSL];

    PROFILE_TYPE *profile = profile_lookup(ch->name);
    int score = profile->rating*profile->rating*profile->rating;

    float party_rating = 0.0, social_rating = 0.0, performance_rating = 0.0;
    if(profile->party_count > 0)
    party_rating = (float)profile->party_total/profile->party_count;
    if(profile->event_count > 0)
    social_rating = (float)profile->event_total/profile->event_count;
    if(profile->perf_count > 0)
    performance_rating = (float)profile->perf_total/profile->perf_count;

    int hscore = UMAX(25, (int)(party_rating)) * UMAX(25, (int)(social_rating)) * UMAX(25, (int)(performance_rating)) * UMAX(10, profile->host_rating);


    int levents = (int)sqrt(ch->pcdata->life_tracker[TRACK_EVENTS_HELD]);
    int wevents = ch->pcdata->week_tracker[TRACK_EVENTS_HELD];

    int aevents = (int)sqrt(ch->pcdata->life_tracker[TRACK_EVENTS_ATTENDED]);
    int wavents = ch->pcdata->week_tracker[TRACK_EVENTS_ATTENDED];

    int fscore = (((score/1000)*(hscore/1000))/10)*(levents+1)*(wevents+1)*(aevents+3)*(wavents+3);
    fscore /= 20;
    fscore += ch->pcdata->week_tracker[TRACK_EVENT_HOTNESS]*ch->pcdata->week_tracker[TRACK_EVENT_HOTNESS]*10;
    fscore += solidity(ch)*100;
    sprintf(buf, "INFLUENCERSCORE: %s: final score: %d, init score: %d, host_score: %d, solidity: %d, hotness: %d, events %d, %d, %d, %d", ch->name, fscore, score, hscore, solidity(ch)*100, ch->pcdata->week_tracker[TRACK_EVENT_HOTNESS]*ch->pcdata->week_tracker[TRACK_EVENT_HOTNESS]*10, levents, wevents, aevents, wavents);
    log_string(buf);
    return fscore;

  }

  void show_random_ai_feedback(CHAR_DATA *ch)
  {
    int count = 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if((!str_cmp((*it)->nameone, ch->name) || !str_cmp((*it)->nametwo, ch->name)) && (*it)->score_one_auto_chat > 0)
      count++;
    }
    if(count < 2)
    {
      send_to_char("You need to have at least two ratings first.\n\r", ch);
      return;
    }
    int point = number_range(0, count);
    int ncount = 0;
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if((!str_cmp((*it)->nameone, ch->name) || !str_cmp((*it)->nametwo, ch->name)) && (*it)->score_one_auto_chat > 0)
      {
        ncount++;
        if(ncount == point)
        {
          if(!str_cmp((*it)->nameone, ch->name))
          {
            char * feedback = replaceSubstring((*it)->auto_chat_review, (*it)->nametwo, "Name");
            printf_to_char(ch, "This feedback is written by AI, while that makes it quite impartial it also means it is somewhat limited:\n%s\n\r", feedback);
            return;
          }
          else if(!str_cmp((*it)->nametwo, ch->name))
          {
            char * feedback = replaceSubstring((*it)->auto_chat_review, (*it)->nameone, "Name");
            printf_to_char(ch, "This feedback is written by AI, while that makes it quite impartial it also means it is somewhat limited:\n%s\n\r", feedback);
            return;
          }

        }

      }
    }

  }

  _DOFUN(do_randomfeedback)
  {
    show_random_ai_feedback(ch);
  }

#if defined(__cplusplus)
}
#endif

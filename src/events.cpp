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

  /*Local Functions */
  int get_flee_direction args((CHAR_DATA * ch));
  void launch_scheme_thwart args((CHAR_DATA * storyrunner, CHAR_DATA *target, int scheme));
  void thwart_scheme args((CHAR_DATA * sr, EVENT_TYPE *event));
  void end_thwart args((CHAR_DATA * sr, EVENT_TYPE *event));

  vector<EVENT_TYPE *> EventVect;
  EVENT_TYPE *nullevent;

#define EVENT_ONE_TIME 60 * 24 * 4

#define EVENT_TWO_TIME 60 * 24 * 8

#define EVENT_THREE_TIME 60 * 24 * 12

#define SCHEME_COST_ONE 10000
#define SCHEME_COST_TWO 20000
#define SCHEME_COST_THREE 40000

  int event_recruitment = 0;
  int event_teaching = 0;
  int event_occurance = 0;
  int event_operation = 0;

  int event_dominance = 0;
  int event_aegis = 0;
  int event_cleanse = 0;
  int event_catastrophe = 0;

  char *const event_names[] = {
    "None",        "MindControl",     "Discredit",   "Steal", "Outcast",     "Brainwash",       "Heist",       "Operation", "Recruitment", "Teaching",        "Occurance",   "Condition", "Dominance",   "Aegis",           "Cleanse",     "Catastrophe", "nothing",     "RevokeSanctuary", "Rob",         "disruption", "storm",       "flood",           "blackout",    "hurricane", "malady",      "haunt",           "dreambelief", "uninvite", "mute",        "sabotage",        "ambush"};

  int scheme_cost(int type) {
    switch (type) {
    case EVENT_ROB:
    case EVENT_DISCREDIT:
    case EVENT_MINDCONTROL:
    case EVENT_OUTCAST:
    case EVENT_SUE:
    case EVENT_STORM:
      return 10000;
      break;
    case EVENT_FLOOD:
    case EVENT_BLACKOUT:
    case EVENT_HAUNT:
      return 20000;
      break;
    case EVENT_AEGIS:
    case EVENT_CATASTROPHE:
    case EVENT_CLEANSE:
    case EVENT_DOMINANCE:
    case EVENT_UNDERSTANDINGPLUS:
    case EVENT_UNDERSTANDINGMINUS:
      return 100000;
      break;
    case EVENT_BRAINWASH:
    case EVENT_DREAMBELIEF:
    case EVENT_UNINVITED:
    case EVENT_MUTE:
    case EVENT_SABOTAGE:
    case EVENT_AMBUSH:
      return 30000;
      break;
    case EVENT_CONDITION:
    case EVENT_HEIST:
    case EVENT_HURRICANE:
    case EVENT_DISRUPTION:
    case EVENT_MALADY:
      return 40000;
      break;
    case EVENT_OCCURANCE:
    case EVENT_OPERATION:
    case EVENT_RECRUITMENT:
    case EVENT_TEACHING:
      return 60000;
      break;
    }

    return 10000;
  }
  int scheme_duration(int type) {
    switch (type) {
    case EVENT_ROB:
    case EVENT_MINDCONTROL:
    case EVENT_SUE:
    case EVENT_STORM:
    case EVENT_FLOOD:
    case EVENT_BLACKOUT:
    case EVENT_HURRICANE:
    case EVENT_DISRUPTION:
    case EVENT_MALADY:
    case EVENT_DREAMBELIEF:
      return 72;
      break;
    case EVENT_BRAINWASH:
    case EVENT_HAUNT:
    case EVENT_MUTE:
    case EVENT_SABOTAGE:
    case EVENT_AMBUSH:
      return 48;
      break;
    case EVENT_AEGIS:
    case EVENT_CATASTROPHE:
    case EVENT_CLEANSE:
    case EVENT_DOMINANCE:
      return 168;
      break;
    case EVENT_UNDERSTANDINGPLUS:
    case EVENT_UNDERSTANDINGMINUS:
    case EVENT_UNINVITED:
      return 24;
      break;
    case EVENT_CONDITION:
    case EVENT_HEIST:
    case EVENT_DISCREDIT:
    case EVENT_OUTCAST:
      return 168;
      break;
    case EVENT_OCCURANCE:
    case EVENT_OPERATION:
    case EVENT_RECRUITMENT:
    case EVENT_TEACHING:
      return 120;
      break;
    }

    return 1;
  }

  void complete_event args((EVENT_TYPE * event));

  void fread_event(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    EVENT_TYPE *event;

    event = new_event();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", event->author, fread_string(fp));
        KEY("ActiveTime", event->active_time, fread_number(fp));
        KEY("Account", event->account, fread_string(fp));
        break;
      case 'B':
        KEY("BatteryAuthor", event->karma_battery_author, fread_number(fp));
        KEY("BatteryStoryRunner", event->karma_battery_storyrunner, fread_number(fp));
        break;
      case 'C':
        KEY("Coauthors", event->coauthors, fread_string(fp));
        break;
      case 'D':
        KEY("Desc", event->description, fread_string(fp));
        KEY("DescIntro", event->introduction, fread_string(fp));
        KEY("DescThwart", event->thwart_method, fread_string(fp));
        KEY("DeactiveTime", event->deactive_time, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!event->author) {
            bug("Fread_Event: Name not found.", 0);
            free_event(event);
            return;
          }
          EventVect.push_back(event);
          return;
        }
        break;
      case 'F':
        KEY("Faction", event->faction, fread_number(fp));
        break;
      case 'I':
        KEY("Imprint", event->imprint, fread_string(fp));
        break;
      case 'L':
        KEY("Limited", event->limited, fread_number(fp));
      case 'M':
        KEY("Message", event->message, fread_string(fp));
      case 'N':
        KEY("NoThwart", event->nothwart, fread_number(fp));
        break;
      case 'R':
        KEY("Researched", event->researched, fread_string(fp));
        break;
      case 'S':
        KEY("ShownNews", event->shown_news, fread_number(fp));
        KEY("Storyline", event->storyline, fread_string(fp));
      case 'T':
        KEY("Type", event->type, fread_number(fp));
        KEY("TypeTwo", event->typetwo, fread_number(fp));
        KEY("Target", event->target, fread_string(fp));
        KEY("ThwartAttempted", event->thwart_attempted, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_event: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_events() {
    nullevent = new_event();
    FILE *fp;

    if ((fp = fopen(EVENT_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_Events: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "EVENT")) {
          fread_event(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Events: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open events.txt", 0);
      exit(0);
    }
  }

  void save_events_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/events.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/events.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/events.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/events.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/events.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/events.txt");
    else
    sprintf(buf, "../data/back7/events.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open events.txt for writing", 0);
      return;
    }

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_events: Blank event in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (current_time > (*it)->deactive_time + (3600 * 36))
      continue;

      fprintf(fpout, "#EVENT\n");
      fprintf(fpout, "Author      %s~\n", (*it)->author);
      fprintf(fpout, "Desc     %s~\n", (*it)->description);
      fprintf(fpout, "DescIntro   %s~\n", (*it)->introduction);
      fprintf(fpout, "Researched %s~\n", (*it)->researched);
      fprintf(fpout, "Target  %s~\n", (*it)->target);
      fprintf(fpout, "Faction    %d\n", (*it)->faction);
      fprintf(fpout, "Imprint %s~\n", (*it)->imprint);
      fprintf(fpout, "Message %s~\n", (*it)->message);
      fprintf(fpout, "DescThwart %s~\n", (*it)->thwart_method);
      fprintf(fpout, "ActiveTime %d\n", (*it)->active_time);
      fprintf(fpout, "DeactiveTime %d\n", (*it)->deactive_time);
      fprintf(fpout, "ShownNews %d\n", (*it)->shown_news);
      fprintf(fpout, "Account %s~\n", (*it)->account);
      fprintf(fpout, "BatteryAuthor %d\n", (*it)->karma_battery_author);
      fprintf(fpout, "BatteryStoryRunner %d\n", (*it)->karma_battery_storyrunner);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "Coauthors %s~\n", (*it)->coauthors);
      fprintf(fpout, "ThwartAttempted %d\n", (*it)->thwart_attempted);
      fprintf(fpout, "NoThwart %d\n", (*it)->nothwart);
      fprintf(fpout, "Limited %d\n", (*it)->limited);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    /*
if(number_percent() % 3 == 0)
{
EventVect.clear();
load_events();
}
*/
  }

  void save_events() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(EVENT_FILE, "w")) == NULL) {
      bug("Cannot open events.txt for writing", 0);
      return;
    }

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_events: Blank event in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if (current_time > (*it)->deactive_time + (3600 * 36))
      continue;

      fprintf(fpout, "#EVENT\n");
      fprintf(fpout, "Author      %s~\n", (*it)->author);
      fprintf(fpout, "Desc     %s~\n", (*it)->description);
      fprintf(fpout, "DescIntro   %s~\n", (*it)->introduction);
      fprintf(fpout, "Researched %s~\n", (*it)->researched);
      fprintf(fpout, "Target  %s~\n", (*it)->target);
      fprintf(fpout, "Faction    %d\n", (*it)->faction);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "TypeTwo %d\n", (*it)->typetwo);

      fprintf(fpout, "Imprint %s~\n", (*it)->imprint);
      fprintf(fpout, "Message %s~\n", (*it)->message);
      fprintf(fpout, "DescThwart %s~\n", (*it)->thwart_method);
      fprintf(fpout, "ActiveTime %d\n", (*it)->active_time);
      fprintf(fpout, "DeactiveTime %d\n", (*it)->deactive_time);
      fprintf(fpout, "ShownNews %d\n", (*it)->shown_news);
      fprintf(fpout, "Account %s~\n", (*it)->account);
      fprintf(fpout, "BatteryAuthor %d\n", (*it)->karma_battery_author);
      fprintf(fpout, "BatteryStoryRunner %d\n", (*it)->karma_battery_storyrunner);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "ThwartAttempted %d\n", (*it)->thwart_attempted);
      fprintf(fpout, "Coauthors %s~\n", (*it)->coauthors);
      fprintf(fpout, "NoThwart %d\n", (*it)->nothwart);
      fprintf(fpout, "Limited %d\n", (*it)->limited);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    /*
if(number_percent() % 3 == 0)
{
EventVect.clear();
load_events();
}
*/
    save_events_backup();
  }

  void event_message(int timer, char *message) {
    NEWS_TYPE *news;

    news = new_news();
    news->timer = timer;
    free_string(news->message);
    news->message = str_dup(message);
    free_string(news->author);
    news->author = str_dup("Town Events");
    NewsVect.push_back(news);
  }

  int get_event_timer(int type) { return 60 * 24; }

  EVENT_TYPE *get_event(int number) {
    int num = 1;

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->active_time > current_time)
      continue;

      if (num == number)
      return (*it);

      num++;
    }

    return nullevent;
  }

  int get_event_number(EVENT_TYPE *event) {
    int i;
    i = 1;
    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->active_time > current_time)
      continue;

      if (!str_cmp(event->author, (*it)->author))
      return i;
      i++;
    }
    return 0;
  }

  bool has_event(CHAR_DATA *ch) {

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (!str_cmp(ch->name, (*it)->author))
      return TRUE;
    }
    return FALSE;
  }

  char *thwart_status(EVENT_TYPE *event) {
    if (current_time > event->active_time +
        (3600 * UMIN(24, scheme_duration(event->type) / 4))) {
      CHAR_DATA *sr;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;

        sr = d->character;
        if (sr == NULL)
        continue;
        if (IS_NPC(sr))
        continue;
        if (sr->pcdata->account == NULL)
        continue;
        if (sr->pcdata->account == NULL)
        continue;

        if (!str_cmp(sr->pcdata->account->name, event->account)) {
          if (!IS_FLAG(sr->comm, COMM_AFK) && sr->pcdata->availability != AVAIL_LOW)
          return "`gYes`x";
        }
        if (is_name(sr->pcdata->account->name, event->coauthors)) {
          if (!IS_FLAG(sr->comm, COMM_AFK) && sr->pcdata->availability != AVAIL_LOW)
          return "`gYes`x";
        }
      }
    }
    else
    return "`rPremature`x";

    return "`rNo`x";
  }

  int thwart_stat(EVENT_TYPE *event) {
    if (current_time > event->active_time +
        (3600 * UMIN(24, scheme_duration(event->type) / 4))) {
      CHAR_DATA *sr;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;

        sr = d->character;
        if (sr == NULL)
        continue;
        if (IS_NPC(sr))
        continue;
        if (sr->pcdata->account == NULL)
        continue;
        if (sr->pcdata->account == NULL)
        continue;

        if (!str_cmp(sr->pcdata->account->name, event->account)) {
          if (!IS_FLAG(sr->comm, COMM_AFK))
          return 1;
        }
        if (is_name(sr->pcdata->account->name, event->coauthors)) {
          if (!IS_FLAG(sr->comm, COMM_AFK))
          return 1;
        }
      }
    }
    else
    return 0;

    return 2;
  }

  _DOFUN(do_event) {
    char arg1[MSL];
    AreaList::iterator it;
    char arg2[MSL];
    char arg3[MSL];
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    CHAR_DATA *victim;
    int i, j;

    EVENT_TYPE *event;

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);

    if (!str_cmp(arg1, "list")) {
      send_to_char("`WCurrent Schemes`x\n\r", ch);
      int i = 1;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->active_time > current_time)
        continue;

        printf_to_char(ch, "`W%d`c)`x\t", i);
        if ((*it)->typetwo != 0)
        printf_to_char(ch, "`W%s's %s-%s`x\n\r", (*it)->author, event_names[(*it)->type], event_names[(*it)->typetwo]);
        else
        printf_to_char(ch, "`W%s's %s`x\n\r", (*it)->author, event_names[(*it)->type]);
        i++;
      }
      return;
    }
    if (!str_cmp(arg1, "aid")) {
      if (is_gm(ch) || IS_FLAG(ch->act, PLR_GUEST))
      return;
      int amount = atoi(arg3);
      if (amount < 0 || amount > ch->pcdata->influence) {
        send_to_char("You don't have that much to give.\n\r", ch);
        return;
      }
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL || IS_NPC(victim)) {
        if ((victim = get_char_world_pc(arg2)) == NULL)
        {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
      }
      if (ch == victim) {
        send_to_char("You cannot aid yourself.\n\r", ch);
        return;
      }
      charge_influence(ch, INFLUENCE_SCHEME, amount);
      victim->pcdata->scheme_influence += amount;
      printf_to_char(ch, "You send %d influence to %s be used in schemes.\n\r", amount, PERS(victim, ch));
      printf_to_char(victim, "%s sends you %d influence to be used in schemes.\n\r", PERS(ch, victim), amount);
    }
    if (!str_cmp(arg1, "delete")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->valid = FALSE;
        }
      }
    }
    if (!str_cmp(arg1, "retarget")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          free_string((*it)->target);
          (*it)->target = str_dup(arg3);
        }
      }
    }
    if (!str_cmp(arg1, "remessage")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          string_append(ch, &(*it)->message);
        }
      }
    }
    if (!str_cmp(arg1, "refresh")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          int duration =
          (scheme_duration((*it)->type) * 3600 * number_range(75, 130)) / 100;
          (*it)->deactive_time = (*it)->active_time + duration;
        }
      }
    }
    if (!str_cmp(arg1, "ageup")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->deactive_time -= (3600 * 12);
        }
      }
    }

    if (!str_cmp(arg1, "thwartattempted")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          (*it)->thwart_attempted = 1;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    if (!str_cmp(arg1, "reimprint")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          string_append(ch, &(*it)->imprint);
        }
      }
    }
    if (!str_cmp(arg1, "restoryrunner")) {
      if (!IS_IMMORTAL(ch))
      return;
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          string_append(ch, &(*it)->coauthors);
        }
      }
    }
    if (!str_cmp(arg1, "thwart")) {
      CHAR_DATA *author;
      int i = 0;
      if (!str_cmp(arg2, "accept")) {
        if (ch->pcdata->scheme_request == NULL) {
          send_to_char("You have no thwart request to reject.\n\r", ch);
          return;
        }
        if (is_gm(ch)) {
          launch_scheme_thwart(ch, ch->pcdata->scheme_requester, get_event_number(ch->pcdata->scheme_request));
          ch->pcdata->scheme_requester = NULL;
          ch->pcdata->scheme_request = NULL;
        }
        else {
          CHAR_DATA *request = ch->pcdata->scheme_requester;
          EVENT_TYPE *event = ch->pcdata->scheme_request;
          convert_to_sr(ch);
          launch_scheme_thwart(ch, request, get_event_number(event));
          ch->pcdata->scheme_requester = NULL;
          ch->pcdata->scheme_request = NULL;
        }
        return;
      }
      if (!str_cmp(arg2, "reject")) {
        if (ch->pcdata->scheme_request == NULL) {
          send_to_char("You have no thwart request to reject.\n\r", ch);
          return;
        }
        printf_to_char(ch->pcdata->scheme_requester, "Your thwart request has been turned down: %s %s\n\r", arg3, argument);
        send_to_char("You turn down the thwart request.\n\r", ch);
        ch->pcdata->scheme_request = NULL;
        ch->pcdata->scheme_requester = NULL;
        return;
      }
      if (!str_cmp(arg2, "refer")) {
        if (ch->pcdata->scheme_request == NULL) {
          send_to_char("You have no thwart request to transfer.\n\r", ch);
          return;
        }
        if ((author = get_char_world_pc(arg3)) == NULL) {
          if ((author = get_char_world(ch, arg3)) == NULL) {
            send_to_char("You can't find anyone around like that to refer the scheme to.\n\r", ch);
            return;
          }
        }
        printf_to_char(
        author, "You have been refered an attempt by %s to try thwarting %s's scheme. Type scheme thwart accept to take on this scene, scheme thwart refer (person) to refer it to another SR. scheme thwart reject (message) to turn down the scheme thwart attempt with the option to include a message.\n\r", PERS(ch->pcdata->scheme_requester, author), ch->pcdata->scheme_request->author);
        send_to_char("Your thwart request has been refered.\n\r", ch->pcdata->scheme_requester);
        author->pcdata->scheme_request = ch->pcdata->scheme_request;
        author->pcdata->scheme_requester = ch->pcdata->scheme_requester;
        ch->pcdata->scheme_request = NULL;
        ch->pcdata->scheme_requester = NULL;
        return;
      }
      if (!str_cmp(arg2, "finish")) {
        if (!str_cmp(arg3, "success")) {
          if (!is_gm(ch) || ch->pcdata->scheme_running == NULL) {
            send_to_char("You're not running a scheme.\n\r", ch);
            return;
          }
          if (ch->pcdata->scheme_running->karma_battery_storyrunner < 500) {
            send_to_char("You need to do more roleplay first.\n\r", ch);
            return;
          }
          EVENT_TYPE *scheme = ch->pcdata->scheme_running;
          thwart_scheme(ch, scheme);
          end_thwart(ch, scheme);
          return;
        }
        else if (!str_cmp(arg3, "failure")) {
          if (!is_gm(ch) || ch->pcdata->scheme_running == NULL) {
            send_to_char("You're not running a scheme.\n\r", ch);
            return;
          }
          act("The thwart attempt comes to an unsucessful end.", ch, NULL, NULL, TO_ROOM);
          send_to_char("You end the thwart attempt.\n\r", ch);
          EVENT_TYPE *scheme = ch->pcdata->scheme_running;
          end_thwart(ch, scheme);
          return;
        }
      }
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          if ((*it)->deactive_time < current_time) {
            send_to_char("That scheme is already over.\n\r", ch);
            return;
          }
          if ((author = get_char_world_pc((*it)->author)) == NULL) {
            if ((author = get_char_world_account((*it)->account)) == NULL) {
              CHAR_DATA *sr;
              for (DescList::iterator ik = descriptor_list.begin();
              ik != descriptor_list.end(); ++ik) {
                DESCRIPTOR_DATA *d = *ik;
                if (d->valid == FALSE)
                continue;

                if (d->connected != CON_PLAYING)
                continue;

                sr = d->character;
                if (sr == NULL)
                continue;
                if (IS_NPC(sr))
                continue;
                if (sr->pcdata->account == NULL)
                continue;

                if (is_name(sr->pcdata->account->name, (*it)->coauthors)) {
                  printf_to_char(
                  sr, "%s wants  to try thwarting %s's scheme. Type scheme thwart accept to take on this scene, scheme thwart refer (person) to refer it to another SR. scheme thwart reject (message) to turn down the scheme thwart attempt with the option to include a message.\n\r", PERS(ch, sr), (*it)->author);
                  send_to_char("Thwart request made.\n\r", ch);
                  sr->pcdata->scheme_request = (*it);
                  sr->pcdata->scheme_requester = ch;
                  if (current_time >
                      (*it)->active_time +
                      (3600 * UMIN(24, scheme_duration((*it)->type) / 4)))
                  (*it)->thwart_attempted = 1;

                  return;
                }
              }
              send_to_char("Nobody is available to handle your thwart request.\n\r", ch);
              return;
            }
            printf_to_char(
            author, "%s wants  to try thwarting %s's scheme. Type scheme thwart accept to take on this scene, scheme thwart refer (person) to refer it to another SR. scheme thwart reject (message) to turn down the scheme thwart attempt with the option to include a message.\n\r", PERS(ch, author), (*it)->author);
          }
          else
          printf_to_char(
          author, "%s wants to try thwarting your scheme. Type scheme thwart accept to take on this scene, scheme thwart refer (person) to refer it to another SR. scheme thwart reject (message) to turn down the scheme thwart attempt with the option to include a message.\n\r", PERS(ch, author));

          send_to_char("Thwart request made.\n\r", ch);
          author->pcdata->scheme_request = (*it);
          author->pcdata->scheme_requester = ch;
          if (current_time >
              (*it)->active_time +
              (3600 * UMIN(24, scheme_duration((*it)->type) / 4)))
          (*it)->thwart_attempted = 1;
          return;
        }
      }
      send_to_char("Syntax: Scheme thwart (number)/Accept/Reject/Refer/finish.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "info")) {
      int i = 0;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active_time > current_time)
        continue;

        i++;

        if (i == atoi(arg2)) {
          printf_to_char(ch, "`W%d`c)`x\n\r", i);
          printf_to_char(ch, "Author: `W%s`x\n\r", (*it)->author);
          if (current_time < (*it)->deactive_time)
          send_to_char("`gActive`x\n\r", ch);
          else
          send_to_char("`DInactive`x\n\r", ch);

          printf_to_char(ch, "Thwartable: %s\n\r", thwart_status((*it)));

          printf_to_char(ch, "Introduction: %s\n\r", (*it)->introduction);

          if (!str_cmp(ch->name, (*it)->author) || is_name(ch->name, (*it)->researched)) {
            if ((*it)->typetwo != 0)
            printf_to_char(ch, "Type: %s-%s\n\nDescription: %s\n\r", event_names[(*it)->type], event_names[(*it)->typetwo], (*it)->description);
            else
            printf_to_char(ch, "Type: %s\n\nDescription: %s\n\r", event_names[(*it)->type], (*it)->description);
            //		    printf_to_char(ch, "If this scheme succeeds: %s\n\r", //(*it)->message);
          }
          if (current_time > (*it)->deactive_time) {
            printf_to_char(ch, "Thwart method: %s\n\r", (*it)->thwart_method);
          }

          if (IS_IMMORTAL(ch)) {
            printf_to_char(ch, "NoThwart: %d, Thwartstat: %d\n\r", (*it)->nothwart, thwart_stat((*it)));
            printf_to_char(ch, "Type: %s  Target %s.\n\r", event_names[(*it)->type], (*it)->target);
            printf_to_char(ch, "Description: %s\n\n\r", (*it)->description);
            printf_to_char(ch, "Message: %s\n\r", (*it)->message);
          }
        }
      }
      return;
    }
    if (IS_AFFECTED(ch, AFF_OUTCAST)) {
      send_to_char("None of your friends seem to be talking to you at the moment.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "create")) {
      if (ch->pcdata->making_event != NULL && !str_cmp(ch->pcdata->making_event->author, ch->name)) {
        send_to_char("You're already creating a scheme\n\r", ch);
        return;
      }
      if (ch->pcdata->influence + ch->pcdata->scheme_influence < 10000 && !IS_IMMORTAL(ch)) {
        send_to_char("You need more influence.\n\r", ch);
        return;
      }
      if (has_event(ch)) {
        send_to_char("You need to wait for your current schme to finish first.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_NIGHTMARE) {
        send_to_char("That would not be very secretive.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->act, PLR_DEAD))
      return;

      if (ch->pcdata->event_cooldown > 0) {
        send_to_char("You can't make a new scheme yet.\n\r", ch);
        return;
      }
      event = new_event();
      free_string(event->author);
      event->author = str_dup(ch->name);
      if (ch->pcdata->account != NULL) {
        free_string(event->account);
        event->account = str_dup(ch->pcdata->account->name);
      }
      event->faction = ch->faction;
      ch->pcdata->making_event = event;
      send_to_char("Scheme created.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "launch")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to launch.\n\r", ch);
        return;
      }

      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to launch.\n\r", ch);
        return;
      }

      if (event->type == 0) {
        send_to_char("You have to set the scheme's type first.", ch);
        return;
      }
      if (higher_power(ch) && (event->type == EVENT_UNDERSTANDINGMINUS || event->type == EVENT_AEGIS || event->type == EVENT_CLEANSE || event->type == EVENT_DOMINANCE)) {
        send_to_char("Higher Powers cannot run these schemes.", ch);
        return;
      }
      if (higher_power(ch) && (event->typetwo == EVENT_UNDERSTANDINGMINUS || event->typetwo == EVENT_AEGIS || event->typetwo == EVENT_CLEANSE || event->typetwo == EVENT_DOMINANCE)) {
        send_to_char("Higher Powers cannot run these schemes.", ch);
        return;
      }

      if (!in_haven(ch->in_room)) {
        send_to_char("You have to get to Haven first.\n\r", ch);
        return;
      }
      if (scheme_cost(event->type) >
          ch->pcdata->influence + ch->pcdata->scheme_influence) {
        printf_to_char(ch, "You need at least %d influence to do that.\n\r", scheme_cost(event->type));
        return;
      }

      if (safe_strlen(event->description) < 100) {
        send_to_char("The scheme needs more of a description first.\n\rscheme description\n\r", ch);
        return;
      }
      if (safe_strlen(event->introduction) < 80) {
        send_to_char("The scheme needs more of an introduction first.\n\rscheme introduction\n\r", ch);
        return;
      }
      if (safe_strlen(event->thwart_method) < 80) {
        send_to_char("The scheme needs more of a thwart method first.\n\rscheme method\n\r", ch);
        return;
      }

      if ((event->type >= EVENT_ROB && event->type <= EVENT_OUTCAST) || event->type == EVENT_CONDITION || event->type == EVENT_MALADY || event->type == EVENT_HAUNT || event->type == EVENT_DREAMBELIEF || event->type == EVENT_MUTE || event->type == EVENT_SABOTAGE || event->type == EVENT_AMBUSH) {
        if (!event->target || event->target[0] == '\0' || safe_strlen(event->target) < 1) {
          send_to_char("You have to set a target first\n\rscheme target <charname>", ch);
          return;
        }
      }
      if ((event->typetwo >= EVENT_ROB && event->typetwo <= EVENT_OUTCAST) || event->typetwo == EVENT_CONDITION || event->typetwo == EVENT_MALADY || event->typetwo == EVENT_HAUNT || event->typetwo == EVENT_DREAMBELIEF || event->typetwo == EVENT_MUTE || event->typetwo == EVENT_SABOTAGE || event->typetwo == EVENT_AMBUSH) {
        if (!event->target || event->target[0] == '\0' || safe_strlen(event->target) < 1) {
          send_to_char("You have to set a target first\n\rscheme target <charname>", ch);
          return;
        }
      }

      if (event->type == EVENT_MINDCONTROL || event->type == EVENT_BRAINWASH || event->type == EVENT_CONDITION) {
        if (!event->imprint || event->imprint[0] == '\0' || safe_strlen(event->imprint) < 3) {
          send_to_char("You have to set an imprint first\n\rscheme imprint <message>", ch);
          return;
        }
      }
      if (event->typetwo == EVENT_MINDCONTROL || event->typetwo == EVENT_BRAINWASH || event->typetwo == EVENT_CONDITION) {
        if (!event->imprint || event->imprint[0] == '\0' || safe_strlen(event->imprint) < 3) {
          send_to_char("You have to set an imprint first\n\rscheme imprint <message>", ch);
          return;
        }
      }

      if (event->type == EVENT_HAUNT || event->typetwo == EVENT_HAUNT) {
        if (!event->message || event->message[0] == '\0' || safe_strlen(event->message) < 3) {
          send_to_char("You have to set a message first\n\rscheme message", ch);
          return;
        }
      }

      event->active_time = current_time + (3600 * 24);
      int duration =
      (scheme_duration(event->type) * 3600 * number_range(75, 130)) / 100;
      int durboost = 0;
      for (int i = 0; i < MAX_CONTACTS; i++) {
        if (ch->pcdata->contact_jobs[i] == CJOB_SCHEMES && safe_strlen(ch->pcdata->contact_names[i]) > 2 && safe_strlen(ch->pcdata->contact_descs[i]) > 2)
        durboost += skillpoint(get_skill(ch, contacts_table[i]));
      }
      duration = duration * (10 + durboost) / 10;
      event->deactive_time = event->active_time + duration;

      send_to_char("Scheme launched!\n\r", ch);
      if (safe_strlen(event->storyline) > 1 && get_storyline(NULL, event->storyline) != NULL)
      get_storyline(NULL, event->storyline)->power += 3;

      ch->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED]++;
      ch->pcdata->life_tracker[TRACK_SCHEMES_LAUNCHED]++;
      if (event->type == EVENT_ROB || event->type == EVENT_DISCREDIT || event->type == EVENT_OUTCAST || event->type == EVENT_SUE || event->type == EVENT_BRAINWASH || event->type == EVENT_AMBUSH || event->type == EVENT_SABOTAGE) {
        villain_mod(ch, 40, "Scheme");
      }
      if (event->type == EVENT_STORM || event->type == EVENT_BLACKOUT || event->type == EVENT_FLOOD || event->type == EVENT_CONDITION || event->type == EVENT_MALADY || event->type == EVENT_UNINVITED || event->type == EVENT_HEIST || event->type == EVENT_MUTE) {
        villain_mod(ch, 120, "Scheme");
      }
      if (event->type == EVENT_HURRICANE || event->type == EVENT_DISRUPTION || event->type == EVENT_UNDERSTANDINGMINUS) {
        villain_mod(ch, 200, "Scheme");
      }
      if (event->type == EVENT_AEGIS || event->type == EVENT_CLEANSE || event->type == EVENT_DOMINANCE || event->type == EVENT_HAUNT) {
        villain_mod(ch, 150, "Scheme");
      }

      if (event->type == EVENT_CATASTROPHE) {
        villain_mod(ch, 100, "Scheme");
      }
      if (event->typetwo == EVENT_ROB || event->typetwo == EVENT_DISCREDIT || event->typetwo == EVENT_OUTCAST || event->typetwo == EVENT_SUE || event->typetwo == EVENT_BRAINWASH || event->type == EVENT_AMBUSH || event->type == EVENT_SABOTAGE) {
        villain_mod(ch, 40, "Scheme");
      }
      if (event->typetwo == EVENT_STORM || event->typetwo == EVENT_BLACKOUT || event->typetwo == EVENT_FLOOD || event->typetwo == EVENT_CONDITION || event->typetwo == EVENT_MALADY || event->typetwo == EVENT_UNINVITED || event->typetwo == EVENT_MUTE) {
        villain_mod(ch, 120, "Scheme");
      }
      if (event->typetwo == EVENT_HEIST || event->typetwo == EVENT_HURRICANE || event->typetwo == EVENT_DISRUPTION || event->typetwo == EVENT_UNDERSTANDINGMINUS) {
        villain_mod(ch, 200, "Scheme");
      }
      if (event->typetwo == EVENT_AEGIS || event->typetwo == EVENT_CLEANSE || event->typetwo == EVENT_DOMINANCE || event->typetwo == EVENT_HAUNT) {
        villain_mod(ch, 150, "Scheme");
      }
      if (event->typetwo == EVENT_CATASTROPHE) {
        villain_mod(ch, 100, "Scheme");
      }

      if (event->type == EVENT_AEGIS)
      scout_report("Your contacts inform your society that an aegis will be occuring in one day.");
      else if (event->type == EVENT_CLEANSE)
      scout_report("Your contacts inform your society that a cleanse will occur in one day.");
      else if (event->type == EVENT_DOMINANCE)
      scout_report("Your contacts inform your society that a dominance ritual will occur in one day.");
      else if (event->type == EVENT_UNDERSTANDINGMINUS)
      scout_report("Your contacts inform your society that sanctuary will fail in one day.");
      if (event->typetwo == EVENT_AEGIS)
      scout_report("Your contacts inform your society that an aegis will be occuring in one day.");
      else if (event->typetwo == EVENT_CLEANSE)
      scout_report("Your contacts inform your society that a cleanse will occur in one day.");
      else if (event->typetwo == EVENT_DOMINANCE)
      scout_report("Your contacts inform your society that a dominance ritual will occur in one day.");
      else if (event->typetwo == EVENT_UNDERSTANDINGMINUS)
      scout_report("Your contacts inform your society that sanctuary will fail in one day.");

      ch->pcdata->event_cooldown = 100;

      charge_influence(
      ch, INFLUENCE_SCHEME, UMAX(scheme_cost(event->type), scheme_cost(event->typetwo)));

      social_behave_mod(ch, 15, "running a scheme.");

      if (IS_IMMORTAL(ch)) {
        ch->pcdata->event_cooldown = 0;
      }
      EventVect.push_back(event);

      return;
    }
    if (!str_cmp(arg1, "limited")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to launch.\n\r", ch);
        return;
      }

      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to launch.\n\r", ch);
        return;
      }

      if (event->type == 0) {
        send_to_char("You have to set the scheme's type first.", ch);
        return;
      }

      if (!in_haven(ch->in_room)) {
        send_to_char("You have to get to Haven first.\n\r", ch);
        return;
      }
      if (scheme_cost(event->type) / 2 >
          ch->pcdata->influence + ch->pcdata->scheme_influence) {
        printf_to_char(ch, "You need at least %d influence to do that.\n\r", scheme_cost(event->type) / 2);
        return;
      }

      if (safe_strlen(event->description) < 100) {
        send_to_char("The scheme needs more of a description first.\n\rscheme description\n\r", ch);
        return;
      }
      if (safe_strlen(event->introduction) < 80) {
        send_to_char("The scheme needs more of an introduction first.\n\rscheme introduction\n\r", ch);
        return;
      }
      if ((event->type >= EVENT_ROB && event->type <= EVENT_OUTCAST) || event->type == EVENT_CONDITION || event->type == EVENT_MALADY || event->type == EVENT_HAUNT || event->type == EVENT_DREAMBELIEF || event->type == EVENT_MUTE) {
        if (!event->target || event->target[0] == '\0' || safe_strlen(event->target) < 1) {
          send_to_char("You have to set a target first\n\rscheme target <charname>", ch);
          return;
        }
      }
      if (event->type == EVENT_MINDCONTROL || event->type == EVENT_BRAINWASH || event->type == EVENT_CONDITION) {
        if (!event->imprint || event->imprint[0] == '\0' || safe_strlen(event->imprint) < 3) {
          send_to_char("You have to set an imprint first\n\rscheme imprint <message>", ch);
          return;
        }
      }
      if ((event->typetwo >= EVENT_ROB && event->typetwo <= EVENT_OUTCAST) || event->typetwo == EVENT_CONDITION || event->typetwo == EVENT_MALADY || event->typetwo == EVENT_HAUNT || event->typetwo == EVENT_DREAMBELIEF || event->type == EVENT_MUTE || event->type == EVENT_AMBUSH || event->typetwo == EVENT_AMBUSH || event->type == EVENT_SABOTAGE || event->typetwo == EVENT_SABOTAGE) {
        if (!event->target || event->target[0] == '\0' || safe_strlen(event->target) < 1) {
          send_to_char("You have to set a target first\n\rscheme target <charname>", ch);
          return;
        }
      }
      if (event->typetwo == EVENT_MINDCONTROL || event->typetwo == EVENT_BRAINWASH || event->typetwo == EVENT_CONDITION) {
        if (!event->imprint || event->imprint[0] == '\0' || safe_strlen(event->imprint) < 3) {
          send_to_char("You have to set an imprint first\n\rscheme imprint <message>", ch);
          return;
        }
      }

      if (event->type == EVENT_HAUNT || event->typetwo == EVENT_HAUNT) {
        if (!event->message || event->message[0] == '\0' || safe_strlen(event->message) < 3) {
          send_to_char("You have to set a message first\n\rscheme message", ch);
          return;
        }
      }
      if (event->type == EVENT_AEGIS || event->type == EVENT_CATASTROPHE || event->type == EVENT_CLEANSE || event->type == EVENT_DOMINANCE || event->type == EVENT_UNDERSTANDINGPLUS || event->type == EVENT_UNDERSTANDINGMINUS) {
        send_to_char("You cannot do that type of scheme as a limited scheme.\n\r", ch);
        return;
      }
      if (event->typetwo == EVENT_AEGIS || event->typetwo == EVENT_CATASTROPHE || event->typetwo == EVENT_CLEANSE || event->typetwo == EVENT_DOMINANCE || event->typetwo == EVENT_UNDERSTANDINGPLUS || event->typetwo == EVENT_UNDERSTANDINGMINUS) {
        send_to_char("You cannot do that type of scheme as a limited scheme.\n\r", ch);
        return;
      }

      event->active_time = current_time + (3600 * 24);
      event->deactive_time = event->active_time + (3600 * 24);
      event->limited = 1;
      event->thwart_attempted = 1;
      send_to_char("Scheme launched!\n\r", ch);
      if (safe_strlen(event->storyline) > 1 && get_storyline(NULL, event->storyline) != NULL)
      get_storyline(NULL, event->storyline)->power += 3;

      ch->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED]++;
      ch->pcdata->life_tracker[TRACK_SCHEMES_LAUNCHED]++;
      if (event->type == EVENT_ROB || event->type == EVENT_DISCREDIT || event->type == EVENT_OUTCAST || event->type == EVENT_SUE || event->type == EVENT_BRAINWASH || event->type == EVENT_SABOTAGE || event->type == EVENT_AMBUSH) {
        villain_mod(ch, 40, "Scheme");
      }
      if (event->type == EVENT_STORM || event->type == EVENT_BLACKOUT || event->type == EVENT_FLOOD || event->type == EVENT_CONDITION || event->type == EVENT_MALADY || event->type == EVENT_UNINVITED || event->type == EVENT_MUTE) {
        villain_mod(ch, 120, "Scheme");
      }
      if (event->type == EVENT_HEIST || event->type == EVENT_HURRICANE || event->type == EVENT_DISRUPTION || event->type == EVENT_UNDERSTANDINGMINUS) {
        villain_mod(ch, 200, "Scheme");
      }
      if (event->type == EVENT_AEGIS || event->type == EVENT_CLEANSE || event->type == EVENT_DOMINANCE || event->type == EVENT_HAUNT) {
        villain_mod(ch, 100, "Scheme");
      }
      if (event->typetwo == EVENT_ROB || event->typetwo == EVENT_DISCREDIT || event->typetwo == EVENT_OUTCAST || event->typetwo == EVENT_SUE || event->typetwo == EVENT_BRAINWASH || event->typetwo == EVENT_SABOTAGE || event->typetwo == EVENT_AMBUSH) {
        villain_mod(ch, 40, "Scheme");
      }
      if (event->typetwo == EVENT_STORM || event->typetwo == EVENT_BLACKOUT || event->typetwo == EVENT_FLOOD || event->typetwo == EVENT_CONDITION || event->typetwo == EVENT_MALADY || event->typetwo == EVENT_UNINVITED || event->typetwo == EVENT_MUTE) {
        villain_mod(ch, 120, "Scheme");
      }
      if (event->typetwo == EVENT_HEIST || event->typetwo == EVENT_HURRICANE || event->typetwo == EVENT_DISRUPTION || event->typetwo == EVENT_UNDERSTANDINGMINUS) {
        villain_mod(ch, 200, "Scheme");
      }
      if (event->typetwo == EVENT_AEGIS || event->typetwo == EVENT_CLEANSE || event->typetwo == EVENT_DOMINANCE || event->typetwo == EVENT_HAUNT) {
        villain_mod(ch, 100, "Scheme");
      }

      ch->pcdata->event_cooldown = 100;

      charge_influence(ch, INFLUENCE_SCHEME, scheme_cost(event->type) / 2);

      social_behave_mod(ch, 15, "running a scheme.");
      EventVect.push_back(event);

      return;
    }
    if (!str_cmp(arg1, "storyline")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      sprintf(buf, "%s %s %s", arg2, arg3, argument);
      if (get_storyline(ch, buf) != NULL) {
        free_string(event->storyline);
        event->storyline = str_dup(get_storyline(ch, buf)->name);
        send_to_char("Done.\n\r", ch);
        return;
      }
      send_to_char("No such storyline.\n\r", ch);
    }
    if (!str_cmp(arg1, "type")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      j = -1;
      for (i = 0; i < MAX_EVENT; i++) {
        if (!str_cmp(event_names[i], arg2))
        j = i;
      }
      if (j == -1) {
        for (i = 0; i < MAX_EVENT; i++) {
          printf_to_char(ch, "%s, ", event_names[i]);
        }
      }
      else {

        for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
        it != EventVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;

          if ((*it)->type == j) {
            //		send_to_char("Someone else is already running a scheme like
            //that.\n\r", ch); 		return;
          }
        }

        event->type = j;
        send_to_char("Done.\n\r", ch);
      }
    }
    if (!str_cmp(arg1, "typetwo")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      j = -1;
      for (i = 0; i < MAX_EVENT; i++) {
        if (!str_cmp(event_names[i], arg2))
        j = i;
      }
      if (j == -1) {
        for (i = 0; i < MAX_EVENT; i++) {
          printf_to_char(ch, "%s, ", event_names[i]);
        }
      }
      else {

        for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
        it != EventVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;

          if ((*it)->type == j) {
            //              send_to_char("Someone else is already running a scheme
            //              like that.\n\r", ch); return;
          }
        }

        event->typetwo = j;
        send_to_char("Done.\n\r", ch);
      }
    }
    if (!str_cmp(arg1, "target")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("Valid group target keywords: Supernaturals, Naturals, Vampires, Vampire, Werewolves, Werewolf, Faeborn, Angelborn, Demonborn, Demigods, [Society name without spaces], (Alliance Name), (Character name), Everyone\n\r", ch);
      if (event->type == EVENT_CONDITION || event->type == EVENT_MALADY || event->type == EVENT_HAUNT || event->type == EVENT_MUTE || event->type == EVENT_SABOTAGE || event->type == EVENT_AMBUSH) {
        sprintf(buf, "%s %s %s", arg2, arg3, argument);
        free_string(event->target);
        event->target = str_dup(buf);
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (event->typetwo == EVENT_CONDITION || event->typetwo == EVENT_MALADY || event->typetwo == EVENT_HAUNT || event->typetwo == EVENT_MUTE || event->typetwo == EVENT_SABOTAGE || event->typetwo == EVENT_AMBUSH) {
        sprintf(buf, "%s %s %s", arg2, arg3, argument);
        free_string(event->target);
        event->target = str_dup(buf);
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (event->type == EVENT_DREAMBELIEF || event->typetwo == EVENT_DREAMBELIEF) {
        if (fetch_fantasy(ch, atoi(arg2)) == NULL || fetch_fantasy(ch, atoi(arg2))->active == FALSE) {
          send_to_char("No such dreamworld.\n\r", ch);
          return;
        }
        if (fetch_fantasy(ch, atoi(arg2))->highlight_time == 0) {
          send_to_char("That world isn't close enough right now.\n\r", ch);
          return;
        }

        free_string(event->target);
        event->target = str_dup(fetch_fantasy(ch, atoi(arg2))->name);
        send_to_char("Done.\n\r", ch);
        return;
      }
      bool online = FALSE;
      d.original = NULL;
      if ((victim = get_char_world_pc_noname(ch, arg2)) !=
          NULL) // Victim is online.
      {
        online = TRUE;
      }
      else {
        if (!load_char_obj(&d, arg2)) {
          printf_to_char(ch, "\n\r%s is not a character!\n\r", capitalize(arg2));
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg2));
        stat(buf, &sb);
        victim = d.character;
      }

      if (IS_NPC(victim)) {
        send_to_char("\n\rYou can't target mobiles!\n\r", ch);
        return;
      }
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (!str_cmp((*it)->target, victim->name)) {
          send_to_char("Someone else is already targeting that person.\n\r", ch);
          if (!online)
          free_char(victim);
          return;
        }
      }
      if (is_gm(victim) || higher_power(victim) || higher_power(ch)) {
        send_to_char("You cannot target them.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }

      free_string(event->target);
      event->target = str_dup(victim->name);
      if (!online)
      free_char(victim);
      send_to_char("Done.\n\r", ch);
    }
    if (!str_cmp(arg1, "description")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("`WThis description is to set the information people can obtain by doing research\n\r`x", ch);
      string_append(ch, &event->description);
    }
    if (!str_cmp(arg1, "message")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("`WThis message will set what people will see when haunted, with a different message each line.\n\r`x", ch);
      string_append(ch, &event->message);
    }
    if (!str_cmp(arg1, "storyrunners")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("`WEnter the account names of everyone you want to be able to run a thwart attempt, separated by spaces.`x\n\r", ch);

      string_append(ch, &event->coauthors);
    }
    if (!str_cmp(arg1, "introduction")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("`WThis description is to set the information people can obtain without doing any research\n\r`x", ch);
      string_append(ch, &event->introduction);
    }
    if (!str_cmp(arg1, "method")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("`WThis description is to lay out at least one way the scheme could be stopped.\n\r`x", ch);
      string_append(ch, &event->thwart_method);
    }
    if (!str_cmp(arg1, "imprint")) {
      event = ch->pcdata->making_event;

      if (event == NULL || event == nullevent) {
        send_to_char("You have no scheme to edit.\n\r", ch);
        return;
      }
      send_to_char("`WThis short string is the imprint message the target will receive.\n\r`x", ch);
      string_append(ch, &event->imprint);
    }
  }

  void event_apply(EVENT_TYPE *event) {
    CHAR_DATA *ch;
    int i;

    if (event->type == EVENT_RECRUITMENT || event->typetwo == EVENT_RECRUITMENT) {
      event_recruitment = event->faction;
    }
    if (event->type == EVENT_TEACHING || event->typetwo == EVENT_TEACHING) {
      event_teaching = event->faction;
    }
    if (event->type == EVENT_OCCURANCE || event->typetwo == EVENT_OCCURANCE) {
      event_occurance = event->faction;
    }
    if (event->type == EVENT_OPERATION || event->typetwo == EVENT_OPERATION) {
      event_operation = event->faction;
    }

    if (event->type == EVENT_DOMINANCE)
    event_dominance = 1;

    if (event->type == EVENT_AEGIS)
    event_aegis = 1;

    if (event->type == EVENT_CLEANSE)
    event_cleanse = 1;

    if (event->typetwo == EVENT_DOMINANCE)
    event_dominance = 1;

    if (event->typetwo == EVENT_AEGIS)
    event_aegis = 1;

    if (event->typetwo == EVENT_CLEANSE)
    event_cleanse = 1;

    if (event->type == EVENT_CATASTROPHE || event->typetwo == EVENT_CATASTROPHE) {
      event_catastrophe = 1;
      CHAR_DATA *victim = get_char_world_pc(event->author);
      if (victim != NULL && IS_FLAG(victim->act, PLR_ASCENDING) && get_tier(victim) < 5 && victim->pcdata->account != NULL) {
        int basekarma = charcost(victim);
        victim->pcdata->tier_raised++;
        int newkarma = charcost(victim);
        victim->pcdata->tier_raised--;
        int cost = newkarma - basekarma;
        if (cost <= available_karma(victim)) {
          victim->pcdata->account->karma -= cost;
          victim->spentkarma += cost;
          victim->pcdata->tier_raised++;
          REMOVE_FLAG(victim->act, PLR_ASCENDING);
        }
      }
    }
    if (event->type == EVENT_STORM)
    crisis_storm = 1;
    if (event->type == EVENT_FLOOD)
    crisis_flood = 1;
    if (event->type == EVENT_BLACKOUT)
    crisis_blackout = 1;
    if (event->type == EVENT_HURRICANE)
    crisis_hurricane = 1;
    if (event->type == EVENT_UNINVITED)
    crisis_uninvited = 1;
    if (event->type == EVENT_DISRUPTION)
    town_blackout = 10;
    if (event->typetwo == EVENT_STORM)
    crisis_storm = 1;
    if (event->typetwo == EVENT_FLOOD)
    crisis_flood = 1;
    if (event->typetwo == EVENT_BLACKOUT)
    crisis_blackout = 1;
    if (event->typetwo == EVENT_HURRICANE)
    crisis_hurricane = 1;
    if (event->typetwo == EVENT_UNINVITED)
    crisis_uninvited = 1;
    if (event->typetwo == EVENT_DISRUPTION)
    town_blackout = 10;

    if (event->type == EVENT_MINDCONTROL || event->typetwo == EVENT_MINDCONTROL) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (str_cmp(ch->name, event->target))
        continue;

        char buf[MSL];
        remove_newlines(buf, event->imprint);
        for (i = 0; i < 25; i++) {
          if (!str_cmp(ch->pcdata->imprint[i], buf)) {
            return;
          }
        }
        auto_imprint(ch, buf, IMPRINT_INFLUENCE);
      }
    }

    if (event->type == EVENT_DISCREDIT || event->typetwo == EVENT_DISCREDIT) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (str_cmp(ch->name, event->target))
        continue;

        if (IS_AFFECTED(ch, AFF_DISCREDIT))
        continue;

        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 240;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_DISCREDIT;
        affect_to_char(ch, &af);
      }
    }
    if (event->type == EVENT_ROB || event->typetwo == EVENT_ROB) {
      if (time_info.minute % 32 != 0)
      return;
      CHAR_DATA *author;
      if ((author = get_char_world_pc(event->author)) == NULL) {
        return;
      }
      OBJ_DATA *obj;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;
        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;
        if (str_cmp(ch->name, event->target))
        continue;

        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (!IS_SET(obj->extra_flags, ITEM_WARDROBE))
          continue;
          if (number_percent() % 77 == 0 || (obj->item_type != ITEM_CLOTHING && number_percent() % 11 == 0)) {
            obj_from_char(obj);
            obj_to_char(obj, author);
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            printf_to_char(
            author, "You get %s in loot from %s as part of your scheme.\n\r", obj->description, NAME(ch));
            return;
          }
        }
      }
    }

    if (event->type == EVENT_SUE || event->typetwo == EVENT_SUE) {
      if (time_info.minute % 32 != 0)
      return;

      CHAR_DATA *author;
      if ((author = get_char_world_pc(event->author)) == NULL) {
        return;
      }
      struct stat sb;
      DESCRIPTOR_DATA d;
      CHAR_DATA *victim;
      bool online = FALSE;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(event->target)) != NULL && !IS_NPC(victim)) // Victim is online.
      online = TRUE;
      else {
        if ((victim = get_char_world_pc(event->target)) !=
            NULL) // Victim is online.
        online = TRUE;
        else {

          if (!load_char_obj(&d, event->target)) {
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(event->target));
          stat(buf, &sb);
          victim = d.character;
        }
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);

        return;
      }
      int amount = victim->pcdata->total_money / 4 + victim->money / 8;
      amount /= 25;
      amount = UMAX(amount, 1000);
      victim->pcdata->total_money -= amount;
      author->pcdata->total_money += amount;
      printf_to_char(author, "You get $%d in loot.\n\r", amount / 100);
      save_char_obj(victim, FALSE, FALSE);
      if (!online)
      free_char(victim);

      return;
    }

    if (event->type == EVENT_OUTCAST || event->typetwo == EVENT_OUTCAST) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (str_cmp(ch->name, event->target))
        continue;

        if (IS_AFFECTED(ch, AFF_OUTCAST))
        continue;

        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 120;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_OUTCAST;
        affect_to_char(ch, &af);
      }
    }

    if (event->type == EVENT_DREAMBELIEF || event->typetwo == EVENT_DREAMBELIEF) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;

        if (ch == NULL)
        continue;
        if (IS_NPC(ch))
        continue;

        if (!str_cmp(ch->name, event->author))
        continue;

        if (ch->pcdata->dream_identity_timer < 30) {
          if (!str_cmp(ch->pcdata->identity_world, event->target))
          ch->pcdata->dream_identity_timer = 60;
          else {
            int point = -1;
            free_string(ch->pcdata->identity_world);
            ch->pcdata->identity_world = str_dup(event->target);
            free_string(ch->pcdata->dream_identity);
            ch->pcdata->dream_identity = str_dup("");
            if ((get_tier(ch) < 2 || is_weakness(NULL, ch) || (get_tier(ch) == 2 && ch->faction == 0 && number_range(1, 43432) % 135 == 0)) && safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_NAME)) <= 2) {
              for (vector<FANTASY_TYPE *>::iterator ft = FantasyVect.begin();
              ft != FantasyVect.end(); ++ft) {
                if ((*ft)->valid == FALSE)
                continue;
                if (!str_cmp((*ft)->name, event->target)) {
                  for (int i = 0; i < 200; i++) {
                    if (safe_strlen((*ft)->participants[i]) > 2 && daysidle((*ft)->participants[i]) > 10 && number_percent() % 4 == 0) {
                      free_string(ch->pcdata->dream_identity);
                      ch->pcdata->dream_identity =
                      str_dup((*ft)->participant_names[i]);
                      point = i;
                    }
                  }
                }
              }
            }
            if (point == -1)
            //                        printf_to_char(ch, "You believe you are %s
            //                        from the world of %s\n\r", //                        (dream_detail(ch, //                        ch->pcdata->identity_world, //                        DREAM_DETAIL_NAME), event->target));
            point = -1;
            else {
              for (vector<FANTASY_TYPE *>::iterator ft = FantasyVect.begin();
              ft != FantasyVect.end(); ++ft) {
                if ((*ft)->valid == FALSE)
                continue;
                if (!str_cmp((*ft)->name, event->target)) {
                  //                              printf_to_char(ch, "You believe
                  //                              you are %s, %s, from the world
                  //                              of %s. Known for %s\n\r", //                              (*ft)->participant_names[point], //                              (*ft)->participant_shorts[point], //                              (*ft)->name, //                              (*ft)->participant_fames[point]);
                }
              }
            }
          }
        }
      }
    }
    if (event->type == EVENT_BRAINWASH || event->typetwo == EVENT_BRAINWASH) {
      bool found;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;

        if (ch == NULL)
        continue;
        if (IS_NPC(ch))
        continue;

        if (!str_cmp(ch->name, event->author))
        continue;

        found = FALSE;

        for (i = 0; i < 25; i++) {
          if (!str_cmp(ch->pcdata->imprint[i], event->imprint) && found == FALSE) {
            found = TRUE;
          }
        }
        if (found == FALSE) {
          for (i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->imprint[i], "") && found == FALSE) {
              auto_imprint(ch, event->imprint, IMPRINT_INFLUENCE);
              found = TRUE;
            }
          }
        }
      }
    }
    if (event->type == EVENT_HAUNT || event->typetwo == EVENT_HAUNT) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (!str_cmp(ch->name, event->author))
        continue;

        if (number_percent() % 17 != 0)
        continue;

        char targ1[MSL];
        one_argument_nouncap(event->target, targ1);

        if ((!str_cmp(targ1, "supernaturals") && is_super(ch)) || (!str_cmp(targ1, "naturals") && !is_super(ch)) || (!str_cmp(targ1, "vampires") && is_vampire(ch)) || (!str_cmp(targ1, "werewolves") && is_werewolf(ch)) || (!str_cmp(targ1, "vampire") && is_vampire(ch)) || (!str_cmp(targ1, "werewolf") && is_werewolf(ch)) || (!str_cmp(targ1, "faeborn") && is_faeborn(ch)) || (!str_cmp(targ1, "angelborn") && is_angelborn(ch)) || (!str_cmp(targ1, "demonborn") && is_demonborn(ch)) || (!str_cmp(targ1, "demigods") && is_demigod(ch)) || (part_of_alliance(ch, event->target)) || (part_of_alliance(ch, targ1)) || (!str_cmp(targ1, ch->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(event->target, clan_lookup(ch->faction)->name)) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(event->target, clan_lookup(ch->factiontwo)->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->faction)->name))) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->factiontwo)->name))) || !str_cmp(targ1, "everyone")) {
          int value = number_range(1, linecount(event->message));
          printf_to_char(ch, "%s\n\r", fetch_line(event->message, value));
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
      }
    }

    if (event->type == EVENT_MUTE || event->typetwo == EVENT_MUTE) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (!str_cmp(ch->name, event->author))
        continue;

        char targ1[MSL];
        one_argument_nouncap(event->target, targ1);

        if (IS_AFFECTED(ch, AFF_MUTE))
        continue;


        if ((!str_cmp(targ1, "supernaturals") && is_super(ch)) || (!str_cmp(targ1, "naturals") && !is_super(ch)) || (!str_cmp(targ1, "vampires") && is_vampire(ch)) || (!str_cmp(targ1, "werewolves") && is_werewolf(ch)) || (!str_cmp(targ1, "vampire") && is_vampire(ch)) || (!str_cmp(targ1, "werewolf") && is_werewolf(ch)) || (!str_cmp(targ1, "faeborn") && is_faeborn(ch)) || (!str_cmp(targ1, "angelborn") && is_angelborn(ch)) || (!str_cmp(targ1, "demonborn") && is_demonborn(ch)) || (!str_cmp(targ1, "demigods") && is_demigod(ch)) || (!str_cmp(targ1, "everyone")) || (part_of_alliance(ch, event->target)) || (part_of_alliance(ch, targ1)) || (!str_cmp(targ1, ch->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(event->target, clan_lookup(ch->faction)->name)) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(event->target, clan_lookup(ch->factiontwo)->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->faction)->name))) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->factiontwo)->name)))) {

          AFFECT_DATA af;
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = 120;
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_MUTE;
          affect_to_char(ch, &af);
        }
      }
    }
    if (event->type == EVENT_SABOTAGE || event->typetwo == EVENT_SABOTAGE) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (ch->pcdata->travel_time <= -1 || ch->pcdata->travel_to <= -1 || ch->pcdata->travel_type <= -1)
        continue;

        if (ch->pcdata->travel_type != TRAVEL_CAR && ch->pcdata->travel_type != TRAVEL_BIKE)
        continue;

        if ((IS_FLAG(ch->comm, COMM_SLOW) || safety_bonus(ch) >= 5) && number_percent() % 3 != 0)
        continue;

        char targ1[MSL];
        one_argument_nouncap(event->target, targ1);

        if ((!str_cmp(targ1, "supernaturals") && is_super(ch)) || (!str_cmp(targ1, "naturals") && !is_super(ch)) || (!str_cmp(targ1, "vampires") && is_vampire(ch)) || (!str_cmp(targ1, "werewolves") && is_werewolf(ch)) || (!str_cmp(targ1, "vampire") && is_vampire(ch)) || (!str_cmp(targ1, "werewolf") && is_werewolf(ch)) || (!str_cmp(targ1, "faeborn") && is_faeborn(ch)) || (!str_cmp(targ1, "angelborn") && is_angelborn(ch)) || (!str_cmp(targ1, "demonborn") && is_demonborn(ch)) || (!str_cmp(targ1, "demigods") && is_demigod(ch)) || (part_of_alliance(ch, event->target)) || (part_of_alliance(ch, targ1)) || (!str_cmp(targ1, ch->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(event->target, clan_lookup(ch->faction)->name)) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(event->target, clan_lookup(ch->factiontwo)->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->faction)->name))) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->factiontwo)->name))) || !str_cmp(targ1, "everyone")) {
          act("Your vehicle is sabotaged.", ch, NULL, NULL, TO_CHAR);
          act("$n's vehicle is sabotaged.", ch, NULL, NULL, TO_ROOM);
          have_accident(ch);
        }
      }
    }

    if (event->type == EVENT_AMBUSH || event->typetwo == EVENT_AMBUSH) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (!in_haven(ch->in_room))
        continue;

        if (ch->recent_moved > -20)
        continue;

        if(ch->pcdata->spectre > 0)
        continue;

        if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
        continue;

        if (public_room(ch->in_room) && !nighttime(ch->in_room))
        continue;

        int chance = 1;
        if (public_room(ch->in_room))
        chance = 5;
        else
        chance = 2;

        if (number_percent() % chance != 0)
        continue;

        char targ1[MSL];
        one_argument_nouncap(event->target, targ1);

        if ((!str_cmp(targ1, "supernaturals") && is_super(ch)) || (!str_cmp(targ1, "naturals") && !is_super(ch)) || (!str_cmp(targ1, "vampires") && is_vampire(ch)) || (!str_cmp(targ1, "werewolves") && is_werewolf(ch)) || (!str_cmp(targ1, "vampire") && is_vampire(ch)) || (!str_cmp(targ1, "werewolf") && is_werewolf(ch)) || (!str_cmp(targ1, "faeborn") && is_faeborn(ch)) || (!str_cmp(targ1, "angelborn") && is_angelborn(ch)) || (!str_cmp(targ1, "demonborn") && is_demonborn(ch)) || (!str_cmp(targ1, "demigods") && is_demigod(ch)) || (part_of_alliance(ch, event->target)) || (part_of_alliance(ch, targ1)) || (!str_cmp(targ1, ch->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(event->target, clan_lookup(ch->faction)->name)) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(event->target, clan_lookup(ch->factiontwo)->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->faction)->name))) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->factiontwo)->name))) || !str_cmp(targ1, "everyone")) {
          act("You are attacked!", ch, NULL, NULL, TO_CHAR);
          act("$n is attacked!", ch, NULL, NULL, TO_ROOM);
          if (ch->wounds == 0)
          wound_char_absolute(ch, 1);
          else
          ch->heal_timer += 5000;
          ch->recent_moved = 10;
        }
      }
    }
    if (event->type == EVENT_CONDITION || event->typetwo == EVENT_CONDITION) {
      bool found;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (!str_cmp(ch->name, event->author))
        continue;

        char targ1[MSL];
        one_argument_nouncap(event->target, targ1);

        found = FALSE;

        if ((!str_cmp(targ1, "supernaturals") && is_super(ch)) || (!str_cmp(targ1, "naturals") && !is_super(ch)) || (!str_cmp(targ1, "vampires") && is_vampire(ch)) || (!str_cmp(targ1, "werewolves") && is_werewolf(ch)) || (!str_cmp(targ1, "vampire") && is_vampire(ch)) || (!str_cmp(targ1, "werewolf") && is_werewolf(ch)) || (!str_cmp(targ1, "faeborn") && is_faeborn(ch)) || (!str_cmp(targ1, "angelborn") && is_angelborn(ch)) || (!str_cmp(targ1, "demonborn") && is_demonborn(ch)) || (!str_cmp(targ1, "demigods") && is_demigod(ch)) || (!str_cmp(targ1, "everyone")) || (part_of_alliance(ch, event->target)) || (part_of_alliance(ch, targ1)) || (!str_cmp(targ1, ch->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(event->target, clan_lookup(ch->faction)->name)) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(event->target, clan_lookup(ch->factiontwo)->name)) || (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->faction)->name))) || (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(nospaces(event->target), nospaces(clan_lookup(ch->factiontwo)->name)))) {

          for (i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->imprint[i], event->imprint) && found == FALSE) {
              found = TRUE;
            }
          }
          if (found == FALSE) {
            for (i = 0; i < 25; i++) {
              if (!str_cmp(ch->pcdata->imprint[i], "") && found == FALSE) {
                auto_imprint(ch, event->imprint, IMPRINT_INFLUENCE);
                found = TRUE;
              }
            }
          }
        }
      }
    }
  }

  void event_end(EVENT_TYPE *event) {
    if (event->type == EVENT_RECRUITMENT) {
      event_recruitment = 0;
    }
    if (event->type == EVENT_TEACHING) {
      event_teaching = 0;
    }
    if (event->type == EVENT_OCCURANCE) {
      event_occurance = 0;
    }
    if (event->type == EVENT_DOMINANCE)
    event_dominance = 0;

    if (event->type == EVENT_AEGIS)
    event_aegis = 0;

    if (event->type == EVENT_CLEANSE)
    event_cleanse = 0;

    if (event->type == EVENT_CATASTROPHE)
    event_catastrophe = 0;
    if (event->type == EVENT_STORM)
    crisis_storm = 0;
    if (event->type == EVENT_FLOOD)
    crisis_flood = 0;
    if (event->type == EVENT_BLACKOUT)
    crisis_blackout = 0;
    if (event->type == EVENT_UNINVITED)
    crisis_uninvited = 0;
    if (event->type == EVENT_HURRICANE)
    crisis_hurricane = 0;
    if (event->type == EVENT_DISRUPTION)
    town_blackout = 0;

    if (event->typetwo == EVENT_RECRUITMENT) {
      event_recruitment = 0;
    }
    if (event->typetwo == EVENT_TEACHING) {
      event_teaching = 0;
    }
    if (event->typetwo == EVENT_OCCURANCE) {
      event_occurance = 0;
    }
    if (event->typetwo == EVENT_DOMINANCE)
    event_dominance = 0;

    if (event->typetwo == EVENT_AEGIS)
    event_aegis = 0;

    if (event->typetwo == EVENT_CLEANSE)
    event_cleanse = 0;

    if (event->typetwo == EVENT_CATASTROPHE)
    event_catastrophe = 0;
    if (event->typetwo == EVENT_STORM)
    crisis_storm = 0;
    if (event->typetwo == EVENT_FLOOD)
    crisis_flood = 0;
    if (event->typetwo == EVENT_BLACKOUT)
    crisis_blackout = 0;
    if (event->typetwo == EVENT_UNINVITED)
    crisis_uninvited = 0;
    if (event->typetwo == EVENT_HURRICANE)
    crisis_hurricane = 0;
    if (event->typetwo == EVENT_DISRUPTION)
    town_blackout = 0;
  }

  void event_update() {
    event_recruitment = 0;
    event_teaching = 0;
    event_occurance = 0;

    event_dominance = 0;
    event_aegis = 0;
    event_cleanse = 0;
    event_catastrophe = 0;

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (current_time > (*it)->deactive_time + (3600 * 24)) {
        (*it)->valid = FALSE;
        continue;
      }
      if (current_time < (*it)->deactive_time) {
        int stat = thwart_stat((*it));
        if (stat == 0)
        (*it)->nothwart = 200;
        else if (stat == 1)
        (*it)->nothwart = UMAX(0, (*it)->nothwart--);
        else if (stat == 2) {
          (*it)->nothwart++;
          if ((*it)->nothwart > 1320 && (*it)->limited == 0) {
            (*it)->thwart_attempted = 1;
            (*it)->deactive_time -= (60 * 20);
          }
        }
      }
      if ((*it)->type == EVENT_OCCURANCE || (*it)->type == EVENT_OPERATION || (*it)->type == EVENT_RECRUITMENT || (*it)->type == EVENT_TEACHING)
      (*it)->thwart_attempted = 1;
      if ((*it)->typetwo == EVENT_OCCURANCE || (*it)->typetwo == EVENT_OPERATION || (*it)->typetwo == EVENT_RECRUITMENT || (*it)->typetwo == EVENT_TEACHING)
      (*it)->thwart_attempted = 1;

      if (current_time > (*it)->active_time && current_time < (*it)->deactive_time) {
        bool found = FALSE;
        for (vector<NEWS_TYPE *>::iterator inn = NewsVect.begin();
        inn != NewsVect.end(); ++inn) {
          if (!(*inn)->author || (*inn)->author[0] == '\0') {
            continue;
          }
          if ((*inn)->valid == FALSE)
          continue;
          if ((*inn)->timer <= 0)
          continue;
          if (!str_cmp((*inn)->message, (*it)->introduction))
          found = TRUE;
        }
        if (!found)
        event_message(2 * 60, (*it)->introduction);

        if ((*it)->thwart_attempted == 0 && (*it)->limited == 0) {
          int duration = (scheme_duration((*it)->type) * 3600 * 75 / 100);
          (*it)->deactive_time =
          UMAX((*it)->deactive_time, current_time + duration);
        }

        event_apply((*it));
      }

      if (current_time > (*it)->deactive_time && current_time < (*it)->deactive_time + 3600)
      event_end((*it));
    }
    ANNIVERSARY_TYPE *ann = get_anniversary_today();
    if(ann != NULL && strlen(ann->messages) > 2) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        CHAR_DATA *ch = d->character;
        if (ch == NULL)
        continue;

        if (IS_NPC(ch))
        continue;

        if (number_percent() % 17 != 0)
        continue;

        int value = number_range(1, linecount(ann->messages));
        printf_to_char(ch, "%s\n\r", fetch_line(ann->messages, value));

      }
    }

  }

  void launch_scheme_thwart(CHAR_DATA *storyrunner, CHAR_DATA *target, int scheme) {

    storyrunner->pcdata->scheme_running = get_event(scheme);
    if (storyrunner->pcdata->scheme_running == NULL) {
      send_to_char("No such scheme.\n\r", storyrunner);
      return;
    }
    if (current_time > storyrunner->pcdata->scheme_running->deactive_time) {
      send_to_char("No such scheme.\n\r", storyrunner);
      return;
    }
    ROOM_INDEX_DATA *to_room = mist_duplicate_room(target->in_room);
    ROOM_INDEX_DATA *orig = target->in_room;
    if (to_room == NULL) {
      send_to_char("No space.\n\r", storyrunner);
      return;
    }

    storyrunner->pcdata->encounter_orig_room = target->in_room->vnum;
    target->pcdata->encounter_orig_room = target->in_room->vnum;

    if (public_room(orig))
    make_exit(to_room->vnum, orig->vnum, DIR_DOWN, CONNECT_ONEWAY);
    else
    make_exit(to_room->vnum, taxi_table[number_range(0, MAX_TAXIS - 1)].vnum, DIR_DOWN, CONNECT_ONEWAY);
    act("$n leaves.", storyrunner, NULL, NULL, TO_ROOM);
    char_from_room(storyrunner);
    char_to_room(storyrunner, to_room);

    CHAR_DATA *helper;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->valid == FALSE)
      continue;

      if (d->connected != CON_PLAYING)
      continue;
      helper = d->character;
      if (helper == NULL)
      continue;
      if (IS_NPC(helper))
      continue;
      if (helper->in_room == NULL || helper->in_room != orig)
      continue;
      act("$n leaves.", helper, NULL, NULL, TO_ROOM);
      char_from_room(helper);
      char_to_room(helper, to_room);
      send_to_char("Your scheme thwart attempt is begining.\n\r", helper);
      if (safe_strlen(storyrunner->pcdata->scheme_running->storyline) > 2 && get_storyline(NULL, storyrunner->pcdata->scheme_running->storyline) !=
          NULL)
      join_to_storyline(
      helper, get_storyline(NULL, storyrunner->pcdata->scheme_running->storyline));
    }
    if (!IS_FLAG(storyrunner->comm, COMM_RUNNING))
    SET_FLAG(storyrunner->comm, COMM_RUNNING);
    target->pcdata->encounter_status = ENCOUNTER_ONGOING;
    storyrunner->pcdata->encounter_status = ENCOUNTER_ONGOING;
    printf_to_char(storyrunner, "Scheme Thwart begining.\nIntro: %s\nDesc: %s\nPossible Thwart Method: %s\n\r", storyrunner->pcdata->scheme_running->introduction, storyrunner->pcdata->scheme_running->description, storyrunner->pcdata->scheme_running->thwart_method);

    char buf[MSL];
    sprintf(buf, "Scheme Encounter %d launched. %s SR.\n\r", get_event_number(storyrunner->pcdata->scheme_running), storyrunner->name);
    log_string(buf);
    wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);
  }

  void end_thwart(CHAR_DATA *sr, EVENT_TYPE *event) {

    ROOM_INDEX_DATA *orig = get_room_index(sr->pcdata->encounter_orig_room);

    if (orig == NULL) {
      orig = room_by_coordinates(sr->in_room->x, sr->in_room->y, sr->in_room->z);
    }
    if (orig != NULL) {
      CHAR_DATA *helper;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->valid == FALSE)
        continue;

        if (d->connected != CON_PLAYING)
        continue;
        helper = d->character;
        if (helper == NULL)
        continue;
        if (IS_NPC(helper))
        continue;
        if (helper->in_room == NULL || helper->in_room != sr->in_room)
        continue;
        if (is_gm(helper))
        continue;
        char_from_room(helper);
        char_to_room(helper, orig);
        act("$n arrives.", helper, NULL, NULL, TO_ROOM);
      }
    }
    char_from_room(sr);
    char_to_room(sr, get_room_index(GMHOME));
    send_to_char("You head back home.\n\r", sr);
    send_to_char("Type storyrunner to revert to your previous character.\n\r", sr);
    return;
  }

  void thwart_scheme(CHAR_DATA *sr, EVENT_TYPE *event) {
    char buf[MIL];
    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *author;

    online = FALSE;
    d.original = NULL;
    if ((author = get_char_world_pc(event->author)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, event->author)) {
        send_to_char("Problem retrieving this scheme's author.\n\r", sr);
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(event->author));
      stat(buf, &sb);
      author = d.character;
    }

    sr->pcdata->scheme_running = NULL;

    if (same_player(sr, author) || sr == author) {
      int amount = event->karma_battery_storyrunner + event->karma_battery_author;
      if (author->pcdata->account->last_scheme > current_time - (3600 * 24 * 20))
      amount /= 5;
      give_karma(sr, amount, KARMA_SCHEME);
    }
    else {
      int amount = event->karma_battery_storyrunner;
      if (author->pcdata->account->last_scheme > current_time - (3600 * 24 * 20))
      amount /= 2;
      give_karma(sr, amount, KARMA_SCHEME);
      amount = event->karma_battery_author;
      if (author->pcdata->account->last_scheme > current_time - (3600 * 24 * 20))
      amount /= 5;
      give_karma(author, amount, KARMA_SCHEME);
    }
    author->pcdata->account->last_scheme = current_time;
    save_char_obj(author, FALSE, FALSE);
    if (!online)
    free_char(author);

    event->deactive_time = current_time - (3600 * 16);

    for (vector<NEWS_TYPE *>::iterator inn = NewsVect.begin();
    inn != NewsVect.end(); ++inn) {
      if (!(*inn)->author || (*inn)->author[0] == '\0') {
        continue;
      }
      if ((*inn)->valid == FALSE)
      continue;
      if ((*inn)->timer <= 0)
      continue;
      if (!str_cmp((*inn)->message, event->introduction))
      (*inn)->timer = 0;
    }
    CHAR_DATA *helper;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->valid == FALSE)
      continue;

      if (d->connected != CON_PLAYING)
      continue;
      helper = d->character;
      if (helper == NULL)
      continue;
      if (IS_NPC(helper))
      continue;
      if (helper->in_room == NULL || helper->in_room != sr->in_room)
      continue;
      if (is_gm(helper))
      continue;

      helper->lifeforce += 500;
      if (helper->pcdata->account != NULL)
      helper->pcdata->account->pkarma =
      UMIN(40000 - helper->pcdata->account->pkarmaspent, helper->pcdata->account->pkarma + 1500);

      helper->pcdata->week_tracker[TRACK_SCHEMES_THWARTED]++;
      helper->pcdata->life_tracker[TRACK_SCHEMES_THWARTED]++;
    }
  }
  _DOFUN(do_swapout) {
    save_char_obj(ch, FALSE, FALSE);
    SET_FLAG(ch->act, PLR_GM);
    SET_FLAG(ch->act, PLR_NOSAVE);
    send_to_char("Done.\n\r", ch);
  }
  _DOFUN(do_swapin) {
    save_account(ch->pcdata->account, FALSE);
    char name[MSL];
    sprintf(name, "%s", ch->name);
    DESCRIPTOR_DATA *d = ch->desc;
    free_char(ch);
    load_char_obj(d, name);
    send_to_char("You return to your regular character.\n\r", d->character);
  }

#if defined(__cplusplus)
}
#endif

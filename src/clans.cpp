
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
#include <math.h>
#include <algorithm>
#include <random>

#if defined(__cplusplus)
extern "C" {
#endif


  /*Local Functions */

#define ORDER_RITUAL 1
#define ORDER_INVESTIGATION 2
#define ORDER_RESEARCH 3
#define ORDER_TAIL 4
#define ORDER_ATTACK 5
#define ORDER_BLIND 6
#define ORDER_MUTE 7
#define ORDER_DEAFEN 8
#define ORDER_WEAKEN 9
#define ORDER_HAUNT 10
#define ORDER_PROTECT 11
#define ORDER_WARD 12
#define ORDER_GUARD 13
#define ORDER_COOLDOWN 14
#define ORDER_TRACE 15
#define ORDER_PRAISEAPPEARANCE 16
#define ORDER_DISSCHARACTER 17
#define ORDER_DISSAPPEARANCE 18
#define ORDER_VALUE 19
#define ORDER_HINVESTIGATION 20
#define ORDER_HRESEARCH 21
#define ORDER_HTAIL 22
#define ORDER_FILE 23
#define ORDER_READING 24
#define ORDER_SCOUT 25
#define ORDER_MERCGUARD 26

#define BASE_DURATION 12 * 60 * 2

#define ADVERSARY_VALUE 65;
#define ADVERSARY_VNUM 115;

  int leftfaction;
  int rightfaction;
  int battle_factions[6] = {0};
  int battle_defender;
  int handpart;
  int orderpart;
  int templepart;
  int antagcore = 0;

  void cabal_update args((void));
  int max_core_power args( (OPERATION_TYPE *op, FACTION_TYPE *host) );
  int member_count args((FACTION_TYPE * fac));
  void win_battle args((int faction, int defensive));
  char *operation_location args((OPERATION_TYPE * op));
  bool badcover_point args((int bg_num, int tox, int toy));
  int first_available_battleground args((void));
  ROOM_INDEX_DATA *next_battleroom args((int number, ROOM_INDEX_DATA *from));
  ROOM_INDEX_DATA *first_battleroom args((int battlenumber));
  ROOM_INDEX_DATA *battleroom_bycoord args((int battlenumber, int x, int y));
  int bg_number args((ROOM_INDEX_DATA * room));
  int poidistance args((CHAR_DATA * ch, int poix, int poiy));
  bool join_to_operation args((int facvnum, OPERATION_TYPE *op));
  void battle_faction args((CHAR_DATA * ch, int vnum));
  void faction_daily args((FACTION_TYPE * fac));
  void lose_operation args((void));
  bool rescue_person args((CHAR_DATA * ch, char *name));
  void win_operation args((int faction, OPERATION_TYPE *op));
  bool faction_elligible args((CHAR_DATA * ch, FACTION_TYPE *fac, bool show, CHAR_DATA *display));
  bool faction_hardelligible args((CHAR_DATA * ch, FACTION_TYPE *fac, bool show, CHAR_DATA *display));
  int goal_influence args((int goal, int faction, int init));
  bool in_fac_name args((char *name, int faction));
  int faction_secrecy args((FACTION_TYPE * fac, CHAR_DATA *report));
  int get_alliance args((FACTION_TYPE * fac, int issue, int type));
  void arrange_battleground args((int battle_type, int battleground_number, char *name));
  int fac_power args((FACTION_TYPE * fac));
  int pc_op_count args((void));
  bool has_base args((FACTION_TYPE * fac, LOCATION_TYPE *loc));
  void antag_soldiers args((OPERATION_TYPE *op, FACTION_TYPE *fac));
  bool check_antag_win args((int number));
  int phil_amount	args( (FACTION_TYPE *fac, LOCATION_TYPE *loc) );

  char *const hand_divisions[] = {"None", "The Peacekeeping Hand", "The Shadow Hand", "The Whispering Hand", "None", "None"};
  char *const order_divisions[] = {"None", "Order ShieldBearers", "Order SwordBearers", "Order Librarians", "None", "None"};
  char *const temple_divisions[] = {"None", "Temple Strike Force", "Temple Intelligence", "Temple Demolishers", "None", "None"};

  const char *terrain_names[11] = {"Forest",  "Field",     "Desert",    "Town", "City",    "Mountains", "Warehouse", "Caves", "Village", "Tundra",    "Lake"};

  int blackmarket_value[10] = {0};
  char *blackmarket_name[10];

  vector<FACTION_TYPE *> FacVect;
  FACTION_TYPE *nullfac;
  vector<OPERATION_TYPE *> OpVect;
  OPERATION_TYPE *nullop;

  OPERATION_TYPE *activeoperation = NULL;
  bool isactiveoperation = FALSE;

  void fread_faction(FILE *fp) {

    char buf[MSL];
    const char *word;
    bool fMatch;
    FACTION_TYPE *fac;
    fac = new_faction();
    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        if (!str_cmp(word, "Attr")) {
          for (int i = 0; i < 30; i++)
          fac->attributes[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Axes")) {
          for (int i = 0; i < 10; i++)
          fac->axes[i] = fread_number(fp);
          fMatch = TRUE;
        }
        KEY("AllianceNumber", fac->alliance, fread_number(fp));
        KEY("AllianceName", fac->alliance_name, fread_string(fp));
        KEY("Awards", fac->awards, fread_number(fp));
        KEY("AwardProgress", fac->award_progress, fread_number(fp));
        KEY("Alchemy", fac->alchemy, fread_number(fp));
        KEY("Antagonist", fac->antagonist, fread_number(fp));
        KEY("Adversary", fac->adversary, fread_number(fp));
        KEY("AntagSoloWins", fac->antag_solo_wins, fread_number(fp));
        KEY("AntagGroupWins", fac->antag_group_wins, fread_number(fp));
        break;
      case 'B':
        KEY("BattleHour", fac->battlehour, fread_number(fp));
        KEY("BattleDay", fac->battleday, fread_number(fp));
        KEY("BattleTerritory", fac->battleterritory, fread_number(fp));
        KEY("BattleClimate", fac->battleclimate, fread_number(fp));
        KEY("BattleWins", fac->battlewins, fread_number(fp));
        KEY("BattleOps", fac->weekly_ops, fread_number(fp));
        break;
      case 'C':
        KEY("Closed", fac->closed, fread_number(fp));
        KEY("CanAlt", fac->can_alt, fread_number(fp));
        KEY("College", fac->college, fread_number(fp));
        KEY("Cardinal", fac->cardinal, fread_number(fp));

        if (!str_cmp(word, "Crystals")) {
          for (int i = 0; i < 5; i++)
          fac->crystals[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'D':
        KEY("Description", fac->description, fread_string(fp));
        KEY("DayResources", fac->dayresources, fread_number(fp));
        break;
      case 'E':
        KEY("Eidilon", fac->eidilon, fread_string(fp));
        KEY("EidilonType", fac->eidilon_type, fread_number(fp));
        KEY("EidilonPlayers", fac->eidilon_players, fread_string(fp));

        if (!str_cmp(word, "End")) {
          FacVect.push_back(fac);
          return;
        }
        if (!str_cmp(word, "ExMember")) {
          int i;
          for (i = 0; i < 100 && safe_strlen(fac->exmember_names[i]) > 1; i++) {
          }
          free_string(fac->exmember_names[i]);
          fac->exmember_names[i] = fread_string(fp);
          fac->exmember_inactivity[i] = fread_number(fp);
          fac->exmember_quit[i] = fread_number(fp);
          fac->exmember_loyalty[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Enemies")) {
          int x = fread_number(fp);
          for (int i = 0; i < 4; i++)
          fac->enemies[x][i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "EnemyDesc")) {
          int x = fread_number(fp);
          free_string(fac->enemy_descs[x]);
          fac->enemy_descs[x] = fread_string(fp);
          fMatch = TRUE;
        }

        break;
      case 'G':
        KEY("Guarding", fac->guarding, fread_number(fp));
        break;
      case 'H':
        KEY("History", fac->history, fread_string(fp));
        break;
      case 'K':
        if (!str_cmp(word, "Kidnap")) {
          int i;
          for (i = 0; i < 20 && safe_strlen(fac->kidnap_name[i]) > 1; i++) {
          }
          free_string(fac->kidnap_name[i]);
          fac->kidnap_name[i] = fread_string(fp);
          free_string(fac->kidnap_territory[i]);
          fac->kidnap_territory[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'L':
        KEY("LastOperation", fac->last_operation, fread_number(fp));
        KEY("LastIntel", fac->last_intel, fread_number(fp));
        KEY("LastHighIntel", fac->last_high_intel, fread_number(fp));
        KEY("LifeEarned", fac->lifeearned, fread_number(fp));
        KEY("LastDeploy", fac->last_deploy, fread_number(fp));
        if (!str_cmp(word, "Leader")) {
          free_string(fac->leader);
          fac->leader = fread_string(fp);
          fac->leaderesteem = fread_number(fp);
          fac->leaderinactivity = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Log")) {
          int i = fread_number(fp);
          fac->log_timer[i] = fread_number(fp);
          free_string(fac->log[i]);
          fac->log[i] = fread_string(fp);
          fMatch = TRUE;
        }
        KEY("LastIncite", fac->last_incite, fread_number(fp));
        break;
      case 'M':
        KEY("Manpower", fac->manpower, fread_number(fp));
        KEY("Missions", fac->missions, fread_string(fp));
        KEY("Manifesto", fac->manifesto, fread_string(fp));
        if (!str_cmp(word, "Member")) {
          int i;
          for (i = 0; i < 100 && safe_strlen(fac->member_names[i]) > 1; i++) {
          }
          free_string(fac->member_names[i]);
          fac->member_names[i] = fread_string(fp);
          fac->member_esteem[i] = fread_number(fp);
          fac->member_inactivity[i] = fread_number(fp);
          fac->member_rank[i] = fread_number(fp);
          fac->member_position[i] = fread_number(fp);
          fac->member_suspended[i] = fread_number(fp);
          fac->member_pay[i] = fread_number(fp);
          fac->member_tier[i] = fread_number(fp);
          fac->member_noleader[i] = fread_number(fp);
          fac->member_flags[i] = fread_number(fp);
          fac->member_power[i] = fread_number(fp);
          fac->member_highest_rank[i] = fread_number(fp);
          free_string(fac->member_rosternotes[i]);
          free_string(fac->member_filenotes[i]);
          fac->member_rosternotes[i] = fread_string(fp);
          fac->member_filenotes[i] = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Message")) {
          int i = fread_number(fp);
          fac->message_timer[i] = fread_number(fp);
          free_string(fac->messages[i]);
          fac->messages[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'N':
        KEY("Name", fac->name, fread_string(fp));
        KEY("NoPart", fac->nopart, fread_number(fp));
        break;
      case 'O':
        KEY("Outcast", fac->outcast, fread_number(fp));
        KEY("OOC", fac->ooc, fread_string(fp));
        KEY("OperationWins", fac->operation_wins, fread_number(fp));
        if (!str_cmp(word, "OpHourRan")) {
          for (int i = 0; i < 25; i++)
          fac->op_hour_ran[i] = fread_number(fp);
          fMatch = TRUE;
        }

        if (!str_cmp(word, "OpSecondPlace")) {
          for (int i = 0; i < 5; i++)
          fac->op_second_place[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'P':
        KEY("Parent", fac->parent, fread_number(fp));
        KEY("Patrolling", fac->patrolling, fread_number(fp));
        KEY("PrisonEmotes", fac->prison_emotes, fread_number(fp));
        KEY("PositionTime", fac->position_time, fread_number(fp));
        break;
      case 'R':
        KEY("Resources", fac->resource, fread_number(fp));
        KEY("Research", fac->research, fread_number(fp));
        KEY("Roles", fac->roles, fread_string(fp));
        KEY("Report", fac->report, fread_string(fp));
        KEY("RestrictTime", fac->restrict_time, fread_number(fp));
        KEY("ReportOneTitle", fac->reportone_title, fread_string(fp));
        KEY("ReportOneTime", fac->reportone_time, fread_number(fp));
        KEY("ReportOneText", fac->reportone_text, fread_string(fp));
        KEY("ReportTwoTitle", fac->reporttwo_title, fread_string(fp));
        KEY("ReportTwoTime", fac->reporttwo_time, fread_number(fp));
        KEY("ReportTwoText", fac->reporttwo_text, fread_string(fp));
        KEY("ReportThreeTitle", fac->reportthree_title, fread_string(fp));
        KEY("ReportThreeTime", fac->reportthree_time, fread_number(fp));
        KEY("ReportThreeText", fac->reportthree_text, fread_string(fp));
        KEY("ReportOneSent", fac->reportone_log_sent, fread_number(fp));
        KEY("ReportTwoSent", fac->reporttwo_log_sent, fread_number(fp));
        KEY("ReportThreeSent", fac->reportthree_log_sent, fread_number(fp));
        KEY("ReportOnePSent", fac->reportone_plog_sent, fread_number(fp));
        KEY("ReportTwoPSent", fac->reporttwo_plog_sent, fread_number(fp));
        KEY("ReportThreePSent", fac->reportthree_plog_sent, fread_number(fp));

        if(!str_cmp(word, "ReportOneParticipants")) {
          int x = fread_number(fp);
          free_string(fac->reportone_participants[x]);
          fac->reportone_participants[x] = fread_string(fp);
          fMatch = TRUE;
        }
        if(!str_cmp(word, "ReportTwoParticipants")) {
          int x = fread_number(fp);
          free_string(fac->reporttwo_participants[x]);
          fac->reporttwo_participants[x] = fread_string(fp);
          fMatch = TRUE;
        }
        if(!str_cmp(word, "ReportThreeParticipants")) {
          int x = fread_number(fp);
          free_string(fac->reportthree_participants[x]);
          fac->reportthree_participants[x] = fread_string(fp);
          fMatch = TRUE;
        }


        if (!str_cmp(word, "ReportOverflow")) {
          int x = fread_number(fp);
          int y = fread_number(fp);
          free_string(fac->report_overflow[x][y]);
          fac->report_overflow[x][y] = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "ReportEvent")) {
          int x = fread_number(fp);
          int y = fread_number(fp);
          fac->event_type[x] = fread_number(fp);
          fac->event_subtype[x] = fread_number(fp);
          fac->event_time[x] = fread_number(fp);
          free_string(fac->event_text[x][y]);
          fac->event_text[x][y] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        if (!str_cmp(word, "ReportETitle")) {
          int x = fread_number(fp);
          free_string(fac->event_title[x]);
          fac->event_title[x] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        if (!str_cmp(word, "ReportESent")) {
          int x = fread_number(fp);
          fac->event_log_sent[x] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "ReportPSent")) {
          int x = fread_number(fp);
          fac->event_plog_sent[x] = fread_number(fp);
          fMatch = TRUE;
        }
        if(!str_cmp(word, "ReportEParticipants")) {
          int x = fread_number(fp);
          int y = fread_number(fp);
          free_string(fac->event_participants[x][y]);
          fac->event_participants[x][y] = fread_string(fp);
          fMatch = TRUE;
        }

        if (!str_cmp(word, "Rank")) {
          int x = fread_number(fp);
          free_string(fac->ranks[x]);
          fac->ranks[x] = fread_string(fp);
          for (int i = 0; i < 20; i++)
          fac->rank_stats[x][i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Restrictions")) {
          for (int i = 0; i < 30; i++)
          fac->restrictions[i] = fread_number(fp);
          fMatch = TRUE;
        }

        break;
      case 'S':
        KEY("Support", fac->support, fread_number(fp));
        KEY("Subtype", fac->subtype, fread_number(fp));
        KEY("Symbol", fac->symbol, fread_string(fp));
        KEY("Stasis", fac->stasis, fread_number(fp));
        KEY("StasisTime", fac->stasis_time, fread_number(fp));
        KEY("SoftRestrict", fac->soft_restrict, fread_number(fp));
        KEY("StasisAccount", fac->stasis_account, fread_string(fp));
        KEY("SecretDays", fac->secret_days, fread_number(fp));
        KEY("SoldierDeploy", fac->soldiers_deploying, fread_number(fp));
        KEY("SoldierName", fac->soldier_name, fread_string(fp));
        KEY("SoldierDesc", fac->soldier_desc, fread_string(fp));
        KEY("Scenes", fac->scenes, fread_string(fp));
        KEY("Staff", fac->staff, fread_number(fp));
        KEY("StealMult", fac->steal_mult, fread_number(fp));
        KEY("SoldierReinforce", fac->soldier_reinforce, fread_number(fp));
        if (!str_cmp(word, "Shipment")) {
          int x = fread_number(fp);
          fac->shipment_date[x] = fread_number(fp);
          fac->shipment_destination[x] = fread_number(fp);
          fac->shipment_amount[x] = fread_number(fp);
          fMatch = TRUE;
        }

        break;
      case 'T':
        KEY("Type", fac->type, fread_number(fp));
        break;
      case 'U':
        KEY("Update", fac->update, fread_number(fp));
        break;
      case 'V':
        KEY("Vnum", fac->vnum, fread_number(fp));
        if (!str_cmp(word, "Vassal")) {
          int i;
          for (i = 0; i < 100 && safe_strlen(fac->vassal_names[i]) > 1; i++) {
          }
          free_string(fac->vassal_names[i]);
          fac->vassal_names[i] = fread_string(fp);
          fac->vassal_inactivity[i] = fread_number(fp);
          fac->vassal_tier[i] = fread_number(fp);
          int throwaway = fread_number(fp);
          throwaway = fread_number(fp);
          throwaway++;
          fMatch = TRUE;
        }

        break;
      case 'W':
        KEY("WeeklyResources", fac->weekly_resources, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_faction: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  int fac_rating(const FACTION_TYPE *fac) {
    int power = 0;
    for (int i = 0; i < 100; i++) {
      if (fac->member_tier[i] > 0)
      power += 1;
      if (fac->member_tier[i] > 1)
      power += fac->member_tier[i];
    }
    power *= fac->resource;
    return power;
  }

  struct faction_greater {
    inline bool operator()(const FACTION_TYPE *fac1, const FACTION_TYPE *fac2) {
      return (fac_rating(fac1) > fac_rating(fac2));
    }
  };

  void load_clans() {
    nullfac = new_faction();
    FILE *fp;
    if ((fp = fopen(CLAN_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Factions: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "FACTION")) {
          fread_faction(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Factions: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
      std::sort(FacVect.begin(), FacVect.end(), faction_greater());
    }
    else {
      bug("Cannot open clans.txt", 0);
      exit(0);
    }
  }

  void save_clans(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];

    save_operations(backup);

    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/clans.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/clans.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/clans.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/clans.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/clans.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/clans.txt");
      else
      sprintf(buf, "../data/back7/clans.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open clans.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(CLAN_FILE, "w")) == NULL) {
        bug("Cannot open clans.txt for writing", 0);
        return;
      }
    }

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0) {
        bug("Save_clans: Blank clan in vector", i);
        continue;
      }
      if (member_count(*it) <= 0 && (*it)->stasis == 0 && !generic_faction_vnum((*it)->vnum) && !protected_faction_vnum((*it)->vnum) && (*it)->antagonist == 0) {
        bug("Recycling faction.", 1);
        continue;
      }
      if ((*it)->resource <= 2000 && !generic_faction_vnum((*it)->vnum) && !protected_faction_vnum((*it)->vnum) && (*it)->antagonist == 0) {
        bug("Recycling faction.", 1);
        continue;
      }
      if ((*it)->lifeearned < -1000 && !generic_faction_vnum((*it)->vnum) && !protected_faction_vnum((*it)->vnum) && safe_strlen((*it)->leader) < 2 && (*it)->antagonist == 0 && (*it)->stasis == 0) {
        bug("Recycling faction.", 1);
        continue;
      }
      if(generic_faction_vnum((*it)->vnum))
      (*it)->type = FACTION_CORE;

      fprintf(fpout, "#FACTION\n");
      fprintf(fpout, "Vnum     %d\n", (*it)->vnum);
      fprintf(fpout, "Type     %d\n", (*it)->type);
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Eidilon %s~\n", (*it)->eidilon);
      fprintf(fpout, "EidilonType %d\n", (*it)->eidilon_type);
      fprintf(fpout, "EidilonPlayers %s~\n", (*it)->eidilon_players);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Outcast %d\n", (*it)->outcast);
      fprintf(fpout, "Manifesto %s~\n", (*it)->manifesto);
      fprintf(fpout, "Missions %s~\n", (*it)->missions);
      fprintf(fpout, "History %s~\n", (*it)->history);
      fprintf(fpout, "Roles %s~\n", (*it)->roles);
      fprintf(fpout, "Scenes %s~\n", (*it)->scenes);
      fprintf(fpout, "OOC %s~\n", (*it)->ooc);
      fprintf(fpout, "AllianceNumber %d\n", (*it)->alliance);
      fprintf(fpout, "AllianceName %s~\n", (*it)->alliance_name);
      fprintf(fpout, "SecretDays %d\n", (*it)->secret_days);
      fprintf(fpout, "StasisTime %d\n", (*it)->stasis_time);
      fprintf(fpout, "StasisAccount %s~\n", (*it)->stasis_account);
      fprintf(fpout, "PositionTime %d\n", (*it)->position_time);
      fprintf(fpout, "RestrictTime %d\n", (*it)->restrict_time);
      fprintf(fpout, "Cardinal %d\n", (*it)->cardinal);
      fprintf(fpout, "Staff %d\n", (*it)->staff);
      fprintf(fpout, "Subtype %d\n", (*it)->subtype);
      fprintf(fpout, "NoPart %d\n", (*it)->nopart);
      fprintf(fpout, "LastOperation %d\n", (*it)->last_operation);
      fprintf(fpout, "LastIntel %d\n", (*it)->last_intel);
      fprintf(fpout, "LastDeploy %d\n", (*it)->last_deploy);
      fprintf(fpout, "OperationWins %d\n", (*it)->operation_wins);
      fprintf(fpout, "StealMult %d\n", (*it)->steal_mult);
      fprintf(fpout, "LifeEarned %d\n", (*it)->lifeearned);
      fprintf(fpout, "WeeklyResources %d\n", (*it)->weekly_resources);
      fprintf(fpout, "LastHighIntel %d\n", (*it)->last_high_intel);
      fprintf(fpout, "Axes");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->axes[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Attr");
      for (i = 0; i < 30; i++)
      fprintf(fpout, " %d", (*it)->attributes[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Restrictions");
      for (i = 0; i < 30; i++)
      fprintf(fpout, " %d", (*it)->restrictions[i]);
      fprintf(fpout, "\n");

      for (i = 0; i < 10; i++) {
        if ((*it)->enemies[i][0] > 0) {
          fprintf(fpout, "Enemies %d", i);
          for (int j = 0; j < 4; j++)
          fprintf(fpout, " %d", (*it)->enemies[i][j]);
          fprintf(fpout, "\n");
        }
      }
      for (i = 0; i < 10; i++) {
        if (safe_strlen((*it)->enemy_descs[i]) > 2) {
          fprintf(fpout, "EnemyDesc %d %s~\n", i, (*it)->enemy_descs[i]);
        }
      }
      fprintf(fpout, "Crystals");
      for (i = 0; i < 5; i++)
      fprintf(fpout, " %d", (*it)->crystals[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "BattleHour %d\n", (*it)->battlehour);
      fprintf(fpout, "BattleDay %d\n", (*it)->battleday);
      fprintf(fpout, "BattleTerritory %d\n", (*it)->battleterritory);
      fprintf(fpout, "BattleClimate %d\n", (*it)->battleclimate);
      fprintf(fpout, "BattleWins %d\n", (*it)->battlewins);
      fprintf(fpout, "AntagSoloWins %d\n", (*it)->antag_solo_wins);
      fprintf(fpout, "AntagGroupWins %d\n", (*it)->antag_group_wins);
      fprintf(fpout, "BattleOps %d\n", (*it)->weekly_ops);
      fprintf(fpout, "CanAlt %d\n", (*it)->can_alt);
      fprintf(fpout, "PrisonEmotes %d\n", (*it)->prison_emotes);
      fprintf(fpout, "Stasis %d\n", (*it)->stasis);
      fprintf(fpout, "Closed %d\n", (*it)->closed);
      fprintf(fpout, "Alchemy %d\n", (*it)->alchemy);
      fprintf(fpout, "LastIncite %d\n", (*it)->last_incite);
      for (i = 0; i < 50; i++) {
        if (safe_strlen((*it)->log[i]) > 2)
        fprintf(fpout, "Log %d %d %s~\n", i, (*it)->log_timer[i], (*it)->log[i]);
      }
      fprintf(fpout, "DayResources %d\n", (*it)->dayresources);
      fprintf(fpout, "Parent %d\n", (*it)->parent);
      fprintf(fpout, "College %d\n", (*it)->college);
      fprintf(fpout, "Resources %d\n", (*it)->resource);
      fprintf(fpout, "Research %d\n", (*it)->research);
      fprintf(fpout, "Support %d\n", (*it)->support);
      fprintf(fpout, "SoftRestrict %d\n", (*it)->soft_restrict);
      fprintf(fpout, "Manpower %d\n", (*it)->manpower);
      fprintf(fpout, "Symbol %s~\n", (*it)->symbol);
      fprintf(fpout, "Update %d\n", (*it)->update);
      fprintf(fpout, "SoldierDeploy %d\n", (*it)->soldiers_deploying);
      fprintf(fpout, "SoldierName %s~\n", (*it)->soldier_name);
      fprintf(fpout, "SoldierDesc %s~\n", (*it)->soldier_desc);
      fprintf(fpout, "SoldierReinforce %d\n", (*it)->soldier_reinforce);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Awards %d\n", (*it)->awards);
      fprintf(fpout, "AwardProgress %d\n", (*it)->award_progress);
      fprintf(fpout, "Patrolling %d\n", (*it)->patrolling);
      fprintf(fpout, "Guarding %d\n", (*it)->guarding);
      if ((*it)->antagonist != 0) {
        fprintf(fpout, "Adversary %d\n", (*it)->adversary);
        fprintf(fpout, "Antagonist %d\n", (*it)->antagonist);
      }

      fprintf(fpout, "Report %s~\n", (*it)->report);
      fprintf(fpout, "OpHourRan");
      for (i = 0; i < 25; i++)
      fprintf(fpout, " %d", (*it)->op_hour_ran[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "OpSecondPlace");
      for (i = 0; i < 5; i++)
      fprintf(fpout, " %d", (*it)->op_second_place[i]);
      fprintf(fpout, "\n");

      for (i = 0; i < 10; i++) {
        fprintf(fpout, "Rank %d %s~", i, (*it)->ranks[i]);
        for (int y = 0; y < 20; y++)
        fprintf(fpout, " %d", (*it)->rank_stats[i][y]);
        fprintf(fpout, "\n");
      }

      for (i = 0; i < 20; i++) {
        if (safe_strlen((*it)->messages[i]) > 2)
        fprintf(fpout, "Message %d %d %s~\n", i, (*it)->message_timer[i], (*it)->messages[i]);
      }
      for (i = 0; i < 5; i++) {
        if ((*it)->shipment_date[i] > 0)
        fprintf(fpout, "Shipment %d %d %d %d\n", i, (*it)->shipment_date[i], (*it)->shipment_destination[i], (*it)->shipment_amount[i]);
      }
      if ((*it)->stasis == 0 && character_exists((*it)->leader))
      fprintf(fpout, "Leader %s~\n%d %d\n", (*it)->leader, (*it)->leaderesteem, (*it)->leaderinactivity);

      for (i = 0; i < 100; i++) {
        if (safe_strlen((*it)->member_names[i]) > 1)
        fprintf(fpout, "Member %s~\n%d %d %d %d %d %d %d %d %d %d %d %s~\n%s~\n", (*it)->member_names[i], (*it)->member_esteem[i], (*it)->member_inactivity[i], (*it)->member_rank[i], (*it)->member_position[i], (*it)->member_suspended[i], (*it)->member_pay[i], (*it)->member_tier[i], (*it)->member_noleader[i], (*it)->member_flags[i], (*it)->member_power[i], (*it)->member_highest_rank[i], (*it)->member_rosternotes[i], (*it)->member_filenotes[i]);
      }
      for (i = 0; i < 100; i++) {
        if (safe_strlen((*it)->vassal_names[i]) > 1)
        fprintf(fpout, "Vassal %s~\n%d %d %d %d\n", (*it)->vassal_names[i], (*it)->vassal_inactivity[i], (*it)->vassal_tier[i], 0, 0);
      }
      for (i = 0; i < 100; i++) {
        if (safe_strlen((*it)->exmember_names[i]) > 1)
        fprintf(fpout, "Exmember %s~\n%d %d %d\n", (*it)->exmember_names[i], (*it)->exmember_inactivity[i], (*it)->exmember_quit[i], (*it)->exmember_loyalty[i]);
      }
      for (i = 0; i < 20; i++) {
        if (safe_strlen((*it)->kidnap_name[i]) > 1)
        fprintf(fpout, "Kidnap %s~\n%s~\n", (*it)->kidnap_name[i], (*it)->kidnap_territory[i]);
      }
      if ((*it)->reportone_time > (current_time - (3600 * 24 * 14))) {
        fprintf(fpout, "ReportOneTitle %s~\n", (*it)->reportone_title);
        fprintf(fpout, "ReportOneTime %d\n", (*it)->reportone_time);
        fprintf(fpout, "ReportOneText %s~\n", (*it)->reportone_text);
      }
      fprintf(fpout, "ReportTwoTitle %s~\n", (*it)->reporttwo_title);
      fprintf(fpout, "ReportTwoTime %d\n", (*it)->reporttwo_time);
      fprintf(fpout, "ReportTwoText %s~\n", (*it)->reporttwo_text);
      fprintf(fpout, "ReportThreeTitle %s~\n", (*it)->reportthree_title);
      fprintf(fpout, "ReportThreeTime %d\n", (*it)->reportthree_time);
      fprintf(fpout, "ReportThreeText %s~\n", (*it)->reportthree_text);
      fprintf(fpout, "ReportOneSent %d\n", (*it)->reportone_log_sent);
      fprintf(fpout, "ReportTwoSent %d\n", (*it)->reporttwo_log_sent);
      fprintf(fpout, "ReportThreeSent %d\n", (*it)->reportthree_log_sent);
      fprintf(fpout, "ReportOnePSent %d\n", (*it)->reportone_plog_sent);
      fprintf(fpout, "ReportTwoPSent %d\n", (*it)->reporttwo_plog_sent);
      fprintf(fpout, "ReportThreePSent %d\n", (*it)->reportthree_plog_sent);

      for(int i=0;i<50;i++) {
        if(safe_strlen((*it)->reportone_participants[i]) > 1)
        fprintf(fpout, "ReportOneParticipants %d %s~\n", i, (*it)->reportone_participants[i]);
        if(safe_strlen((*it)->reporttwo_participants[i]) > 1)
        fprintf(fpout, "ReportTwoParticipants %d %s~\n", i, (*it)->reporttwo_participants[i]);
        if(safe_strlen((*it)->reportthree_participants[i]) > 1)
        fprintf(fpout, "ReportThreeParticipants %d %s~\n", i, (*it)->reportthree_participants[i]);
      }


      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 10; j++) {
          if (safe_strlen((*it)->report_overflow[i][j]) > 2)
          fprintf(fpout, "ReportOverflow %d %d %s~\n", i, j, (*it)->report_overflow[i][j]);
        }
      }
      for (int i = 0; i < 20; i++) {
        if ((*it)->event_time[i] > (current_time - (3600 * 24 * 14))) {
          fprintf(fpout, "ReportETitle %d %s~\n", i, (*it)->event_title[i]);
          fprintf(fpout, "ReportESent %d %d\n", i, (*it)->event_log_sent[i]);
          fprintf(fpout, "ReportPSent %d %d\n", i, (*it)->event_plog_sent[i]);
          for (int j = 0; j < 20; j++) {
            if (safe_strlen((*it)->event_text[i][j]) > 2)
            fprintf(fpout, "ReportEvent %d %d %d %d %d %s~\n", i, j, (*it)->event_type[i], (*it)->event_subtype[i], (*it)->event_time[i], (*it)->event_text[i][j]);
          }
          for(int j=0;j<50;j++) {
            if(safe_strlen((*it)->event_participants[i][j]) > 1)
            fprintf(fpout, "ReportEParticipants %d %d %s~\n", i, j, (*it)->event_participants[i][j]);
          }
        }
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_blackmarket() {
    FILE *fpout;

    if ((fpout = fopen(MARKET_FILE, "w")) == NULL) {
      bug("Cannot open market.txt for writing", 0);
      return;
    }

    fprintf(fpout, "#MARKET\n");
    for (int i = 0; i < 10; i++) {
      if (blackmarket_value[i] > 0)
      fprintf(fpout, "Human %d %d %s~\n", i, blackmarket_value[i], blackmarket_name[i]);
      else
      fprintf(fpout, "Human %d 0 ~\n", i);
    }

    fprintf(fpout, "End\n\n");

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_blackmarket(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;

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
          return;
        }
        break;
      case 'H':
        if (!str_cmp(word, "Human")) {
          int i = 0;
          i = fread_number(fp);
          blackmarket_value[i] = fread_number(fp);
          blackmarket_name[i] = fread_string(fp);
          fMatch = TRUE;
        }

        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_market: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_blackmarket() {
    FILE *fp;

    if ((fp = fopen(MARKET_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_Rent: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "MARKET")) {
          fread_blackmarket(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Rent: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open market.txt", 0);
      exit(0);
    }
  }

  void fread_operation(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    OPERATION_TYPE *op;

    op = new_operation();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", op->author, fread_string(fp));
        KEY("AdversaryType", op->adversary_type, fread_number(fp));
        KEY("AdversaryName", op->adversary_name, fread_string(fp));
        KEY("Atmosphere", op->atmosphere, fread_string(fp));
        break;
      case 'B':
        KEY("Bonus", op->bonus, fread_number(fp));
        KEY("BagName", op->bag_name, fread_string(fp));
        break;
      case 'C':
        KEY("Competition", op->competition, fread_number(fp));
        KEY("Challenge", op->challenge, fread_number(fp));
        break;
      case 'D':
        KEY("Description", op->description, fread_string(fp));
        KEY("Day", op->day, fread_number(fp));
        break;
      case 'E':
        KEY("Elitestring", op->elitestring, fread_string(fp));
        if (!str_cmp(word, "Enrolled")) {
          for (int i = 0; i < 10; i++)
          op->enrolled[i] = fread_number(fp);
          fMatch = TRUE;
        }

        if (!str_cmp(word, "End")) {
          OpVect.push_back(op);
          return;
        }
        break;
      case 'F':
        KEY("Faction", op->faction, fread_number(fp));
        if (!str_cmp(word, "FacTimers")) {
          for (int i = 0; i < 10; i++)
          op->factimers[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'G':
        KEY("Goal", op->goal, fread_number(fp));
        break;
      case 'H':
        KEY("Hour", op->hour, fread_number(fp));
        KEY("HomeSoldiers", op->home_soldiers, fread_number(fp));
        break;
        KEY("InitDays", op->initdays, fread_number(fp));
        break;
      case 'M':
        KEY("MaxPCs", op->max_pcs, fread_number(fp));
        break;
      case 'P':
        KEY("Preferred", op->preferred, fread_string(fp));

        if (!str_cmp(word, "POIX")) {
          for (int i = 0; i < 10; i++)
          op->poix[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "POIY")) {
          for (int i = 0; i < 10; i++)
          op->poiy[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "POIType")) {
          for (int i = 0; i < 10; i++)
          op->poitype[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "POIFaction")) {
          for (int i = 0; i < 10; i++)
          op->poifaction[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'R':
        KEY("RoomName", op->room_name, fread_string(fp));
        break;
      case 'S':
        KEY("Spam", op->spam, fread_number(fp));
        KEY("Speed", op->speed, fread_number(fp));
        KEY("Storyrunners", op->storyrunners, fread_string(fp));
        KEY("Size", op->size, fread_number(fp));
        KEY("Storyline", op->storyline, fread_string(fp));
        if (!str_cmp(word, "Soldiers")) {
          for (int i = 0; i < 10; i++)
          op->soldiers[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Signup")) {
          int i;
          for (i = 0; i < 100 && safe_strlen(op->sign_up[i]) > 1; i++) {
          }
          op->sign_up_type[i] = fread_number(fp);
          free_string(op->sign_up[i]);
          op->sign_up[i] = fread_string(fp);
          fMatch = TRUE;
        }

        break;
      case 'T':
        KEY("Timeshift", op->timeshifted, fread_number(fp));
        KEY("Target", op->target, fread_string(fp));
        KEY("Type", op->type, fread_number(fp));
        KEY("Terrain", op->terrain, fread_number(fp));
        KEY("Territory", op->territoryvnum, fread_number(fp));
        KEY("Timer", op->timer, fread_number(fp));
        KEY("Timeline", op->timeline, fread_string(fp));
        break;

      case 'U':
        KEY("UploadName", op->upload_name, fread_string(fp));
      }
      if (!fMatch) {
        sprintf(buf, "Fread_operation: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  int operation_order(const OPERATION_TYPE *op) {
    int strength = 0;
    strength += op->day * 24;
    strength += op->hour;
    strength -= get_hour(NULL);

    strength *= -1;
    return strength;
  }

  struct operation_greater {
    inline bool operator()(const OPERATION_TYPE *op1, const OPERATION_TYPE *op2) {
      return (operation_order(op1) > operation_order(op2));
    }
  };

  void load_operations() {
    nullop = new_operation();
    FILE *fp;
    if ((fp = fopen(OPERATION_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_Operations: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "OPERATION")) {
          fread_operation(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Operations: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open operations.txt", 0);
      exit(0);
    }
    std::sort(OpVect.begin(), OpVect.end(), operation_greater());
  }
  void save_operations(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/operations.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/operations.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/operations.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/operations.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/operations.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/operations.txt");
      else
      sprintf(buf, "../data/back7/operations.txt");
      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open operations.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(OPERATION_FILE, "w")) == NULL) {
        bug("Cannot open operations.txt for writing", 0);
        return;
      }
    }
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->hour == 0) {
        continue;
      }
      if (clan_lookup((*it)->faction) == NULL)
      continue;

      fprintf(fpout, "#OPERATION\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Territory %d\n", (*it)->territoryvnum);
      fprintf(fpout, "AdversaryType %d\n", (*it)->adversary_type);
      fprintf(fpout, "AdversaryName %s~\n", (*it)->adversary_name);
      fprintf(fpout, "Elitestring %s~\n", (*it)->elitestring);
      fprintf(fpout, "Storyrunners %s~\n", (*it)->storyrunners);
      fprintf(fpout, "Atmosphere %s~\n", (*it)->atmosphere);
      fprintf(fpout, "RoomName %s~\n", (*it)->room_name);
      fprintf(fpout, "Description %s~\n", (*it)->description);
      fprintf(fpout, "Preferred %s~\n", (*it)->preferred);
      fprintf(fpout, "BagName %s~\n", (*it)->bag_name);
      fprintf(fpout, "Timeshift %d\n", (*it)->timeshifted);
      fprintf(fpout, "UploadName %s~\n", (*it)->upload_name);
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "InitDays %d\n", (*it)->initdays);
      fprintf(fpout, "Spam %d\n", (*it)->spam);
      fprintf(fpout, "MaxPCs %d\n", (*it)->max_pcs);
      fprintf(fpout, "Speed %d\n", (*it)->speed);
      fprintf(fpout, "Terrain %d\n", (*it)->terrain);
      fprintf(fpout, "Size %d\n", (*it)->size);
      fprintf(fpout, "Hour %d\n", (*it)->hour);
      fprintf(fpout, "Day %d\n", (*it)->day);
      fprintf(fpout, "Faction %d\n", (*it)->faction);
      fprintf(fpout, "Bonus %d\n", (*it)->bonus);
      fprintf(fpout, "Storyline %s~\n", (*it)->storyline);
      fprintf(fpout, "Timer %d\n", (*it)->timer);
      fprintf(fpout, "Competition %d\n", (*it)->competition);
      fprintf(fpout, "Timeline %s~\n", (*it)->timeline);
      fprintf(fpout, "Challenge %d\n", (*it)->challenge);
      fprintf(fpout, "HomeSoldiers %d\n", (*it)->home_soldiers);
      fprintf(fpout, "Goal %d\n", (*it)->goal);
      fprintf(fpout, "Target %s~\n", (*it)->target);
      for (i = 0; i < 100; i++) {
        if (safe_strlen((*it)->sign_up[i]) > 1)
        fprintf(fpout, "Signup %d %s~\n", (*it)->sign_up_type[i], (*it)->sign_up[i]);
      }

      fprintf(fpout, "Soldiers");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->soldiers[i]);
      fprintf(fpout, "\n");

      fprintf(fpout, "Enrolled");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->enrolled[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "FacTimers");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->factimers[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "POIX");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->poix[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "POIY");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->poiy[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "POIType");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->poitype[i]);
      fprintf(fpout, "\n");
      fprintf(fpout, "POIFaction");
      for (i = 0; i < 10; i++)
      fprintf(fpout, " %d", (*it)->poifaction[i]);
      fprintf(fpout, "\n");

      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  FACTION_TYPE *clan_lookup(int vnum) {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == vnum)
      return (*it);
    }
    return NULL;
  }

  FACTION_TYPE *clan_lookup_name(char *name) {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if (!str_cmp((*it)->name, name))
      return (*it);
    }
    return NULL;
  }

  FACTION_TYPE *get_clan(CHAR_DATA *ch) { return clan_lookup(ch->faction); }

  FACTION_TYPE *get_faction(CHAR_DATA *ch) { return clan_lookup(ch->fcore); }
  FACTION_TYPE *get_cult(CHAR_DATA *ch) { return clan_lookup(ch->fcult); }
  FACTION_TYPE *get_sect(CHAR_DATA *ch) { return clan_lookup(ch->fsect); }

  int get_rank_respect(FACTION_TYPE *fac, int rank) {
    for (int i = 0; i < 100; i++) {
      if (fac->member_rank[i] == rank)
      return fac->member_esteem[i];
    }
    return 0;
  }

  const char *fac_names[4] = {
    "", "The Hand", "The Order", "The Temple", };

  int rank_requirement(int rank) {
    rank++;
    switch (rank) {
    case 1:
      return 0;
      break;
    case 2:
      return 2000;
    case 3:
      return 12500;
    case 4:
      return 25000;
    case 5:
      return 50000;
    case 6:
      return 75000;
    case 7:
      return 100000;
    case 8:
      return 150000;
    case 9:
      return 200000;
    case 10:
      return 350000;
    case 11:
      return 600000;
    }
    return 500000;
  }

  void give_clan_power(CHAR_DATA *ch, int amount) {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0) {
        continue;
      }
      FACTION_TYPE *fac = (*it);
      for (int i = 0; i < 100; i++) {
        if (!str_cmp(ch->name, fac->member_names[i])) {
          fac->member_power[i] += amount;
        }
      }
    }
  }

  void give_respect(CHAR_DATA *ch, int amount, char *reason, int faction) {
    if (faction == 0)
    return;

    amount = amount * (get_tier(ch)+3) / 4;

    amount = amount * 3 / 4;
    amount = amount * (100 - (int)sqrt(ch->pcdata->attract[ATTRACT_PROM])) / 100;

    int i;
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac == NULL || fac == nullfac)
    return;

    if (amount < 0) {
      int bel = belief_match(ch, fac);
      if (bel < 0) {
        amount = amount * (bel * -1 + 100) / 100;
      }
      else {
        amount = amount * 100 / (bel + 100);
      }
    }
    if (amount > 0) {
      if (leadotherfac(ch->name, fac->vnum))
      amount = amount /2;

      int bel = belief_match(ch, fac);
      if (bel > 0) {
        amount = amount * (bel + 100) / 100;
      }
      else {
        amount = amount * 100 / (bel * -1 + 100);
      }

      if (fac->vnum == FACTION_HAND) {
        if (prof_focus(ch) > arcane_focus(ch) && prof_focus(ch) > combat_focus(ch))
        amount = amount * 3 / 2;
        else if (combat_focus(ch) >= arcane_focus(ch) && combat_focus(ch) >= prof_focus(ch))
        amount = amount * 2 / 3;
      }
      if (fac->vnum == FACTION_ORDER) {
        if (arcane_focus(ch) > prof_focus(ch) && arcane_focus(ch) > combat_focus(ch))
        amount = amount * 3 / 2;
        else if (prof_focus(ch) >= arcane_focus(ch) && prof_focus(ch) >= arcane_focus(ch))
        amount = amount * 2 / 3;
      }
      if (fac->vnum == FACTION_TEMPLE) {
        if (combat_focus(ch) > arcane_focus(ch) && combat_focus(ch) > prof_focus(ch))
        amount = amount * 3 / 2;
        else if (arcane_focus(ch) >= combat_focus(ch) && arcane_focus(ch) >= prof_focus(ch))
        amount = amount * 2 / 3;
      }

      amount = amount * (5 + get_skill(ch, SKILL_CHARISMA));
      amount /= 5;
      if (IS_FLAG(ch->comm, COMM_NOLEADER))
      amount = amount * 2 / 3;

      if (fac->axes[AXES_SUPERNATURAL] == AXES_FARLEFT) {
        if (seems_super(ch))
        amount = amount * 3 / 2;
        else
        amount = amount / 2;
      }
      if (fac->axes[AXES_SUPERNATURAL] == AXES_MIDLEFT) {
        if (seems_super(ch))
        amount = amount * 130 / 100;
        else
        amount = amount * 70 / 100;
      }
      if (fac->axes[AXES_SUPERNATURAL] == AXES_NEARLEFT) {
        if (seems_super(ch))
        amount = amount * 110 / 100;
        else
        amount = amount * 90 / 100;
      }
      if (fac->axes[AXES_SUPERNATURAL] == AXES_FARRIGHT) {
        if (!seems_super(ch))
        amount = amount * 150 / 100;
        else
        amount = amount * 50 / 100;
      }
      if (fac->axes[AXES_SUPERNATURAL] == AXES_MIDRIGHT) {
        if (!seems_super(ch))
        amount = amount * 130 / 100;
        else
        amount = amount * 70 / 100;
      }
      if (fac->axes[AXES_SUPERNATURAL] == AXES_NEARRIGHT) {
        if (!seems_super(ch))
        amount = amount * 110 / 100;
        else
        amount = amount * 90 / 100;
      }
    }

    if (faction == ch->fcore)
    ch->esteem_faction += amount;
    if (faction == ch->fsect)
    ch->esteem_sect += amount;
    if (faction == ch->fcult)
    ch->esteem_cult += amount;

    if (!str_cmp(ch->name, fac->leader)) {
      fac->leaderesteem += amount;
    }
    if (amount > 0) {
      if (ch->pcdata->faction_influence > 10000)
      ch->pcdata->faction_influence = 10000;
    }
    if (safe_strlen(reason) > 2 && amount != 0) {
      char buf[MSL];
      sprintf(buf, "RESPECT: %s was given %d for %s.\n\r", ch->name, amount, reason);
      log_string(buf);
    }

    for (i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, fac->member_names[i])) {
        fac->member_esteem[i] += amount;
        return;
      }
    }
  }
  void give_respect_noecho(CHAR_DATA *ch, int amount, int faction) {
    if (faction == 0)
    return;

    int i;
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac == NULL || fac == nullfac)
    return;

    if (amount > 0) {
      if (IS_FLAG(ch->comm, COMM_NOLEADER))
      amount = amount * 2 / 3;
    }

    if (ch->fcore == faction)
    ch->esteem_faction += amount;
    if (ch->fcult == faction)
    ch->esteem_cult += amount;
    if (ch->fsect == faction)
    ch->esteem_sect += amount;


    if (!str_cmp(ch->name, fac->leader)) {
      fac->leaderesteem += amount;
    }

    if (amount != 0) {
      char buf[MSL];
      sprintf(buf, "RESPECT No echo: %s was given %d.\n\r", ch->name, amount);
      log_string(buf);
    }

    for (i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, fac->member_names[i])) {
        fac->member_esteem[i] += amount;
        return;
      }
    }
  }

  bool default_leader(CHAR_DATA *ch, FACTION_TYPE *fac)
  {
    int cesteem = 0;
    int maxesteem = 0;
    for (int i = 0; i < 100; i++) {
      if(str_cmp(fac->member_names[i], ch->name) && fac->member_inactivity[i] < 50 && fac->member_esteem[i] > maxesteem && fac->member_noleader[i] == 0)
      maxesteem = fac->member_esteem[i];
      if(!str_cmp(fac->member_names[i], ch->name))
      cesteem = fac->member_esteem[i];
    }
    if(cesteem > maxesteem)
    return TRUE;
    return FALSE;

  }


  bool has_trust(CHAR_DATA *ch, int trust, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    if (fac == NULL || fac == nullfac) {
      return FALSE;
    }
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (ch->in_room->vnum < 100)
    return FALSE;

    if (ch->pcdata->bloodaura > 0 && !IS_FLAG(ch->act, PLR_GUEST))
    return FALSE;

    if (trust == TRUST_COMMS && battleground(ch->in_room))
    return TRUE;

    if (!str_cmp(ch->name, fac->leader))
    return TRUE;

    if(default_leader(ch, fac))
    return TRUE;


    if (fac->axes[AXES_SUPERNATURAL] == AXES_FARLEFT && !seems_super(ch) && event_cleanse == 0 && trust >= TRUST_SUSPENSIONS)
    return FALSE;
    if (fac->axes[AXES_SUPERNATURAL] == AXES_FARRIGHT && seems_super(ch) && event_cleanse == 0 && trust >= TRUST_SUSPENSIONS)
    return FALSE;
    if (fac->axes[AXES_CORRUPT] <= AXES_FARLEFT && get_tier(ch) <= 2 && trust >= TRUST_SUSPENSIONS && fac->axes[AXES_CORRUPT] > 0)
    return FALSE;
    if (fac->axes[AXES_CORRUPT] <= AXES_NEARLEFT && get_tier(ch) <= 1 && trust >= TRUST_SUSPENSIONS && fac->axes[AXES_CORRUPT] > 0)
    return FALSE;

    if (!trust_elligible(ch, fac, FALSE, NULL) && trust >= TRUST_RESOURCES)
    return FALSE;

    if (trust >= TRUST_SUSPENSIONS && college_student(ch, TRUE) && fac->college == 0)
    return FALSE;

    if (fac->stasis == 1)
    return TRUE;

    int rank = -1;
    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], ch->name)) {
        if (fac->member_suspended[i] > 0)
        return FALSE;

        rank = fac->member_position[i];
      }
    }
    if (rank == -1)
    return FALSE;

    if (fac->rank_stats[rank][trust] > 0)
    return TRUE;

    return FALSE;
  }

  bool is_second(CHAR_DATA *ch, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac == NULL || fac == nullfac) {
      return FALSE;
    }

    if (!str_cmp(ch->name, fac->leader))
    return TRUE;

    if (!str_cmp(fac->member_names[0], ch->name))
    return TRUE;
    if (!str_cmp(fac->leader, fac->member_names[0]) && !str_cmp(fac->member_names[1], ch->name))
    return TRUE;

    return FALSE;
  }

  bool is_leader(CHAR_DATA *ch, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac == NULL || fac == nullfac) {
      return FALSE;
    }

    if (!str_cmp(ch->name, fac->leader))
    return TRUE;

    if (IS_IMMORTAL(ch))
    return TRUE;

    return FALSE;
  }

  void join_to_clan(CHAR_DATA *ch, int vnum) {
    FACTION_TYPE *fac = clan_lookup(vnum);
    if (fac->antagonist != 0)
    return;

    int i;

    if (fac == NULL || fac == nullfac) {
      send_to_char("Join attempt failed.\n\r", ch);
      return;
    }

    if (fac->type == FACTION_CORE)
    {
      if(ch->fcore == vnum)
      return;

      ch->fcore = vnum;
      ch->esteem_faction = 100;
    }
    else if (fac->type == FACTION_CULT)
    {
      if(ch->fcult == vnum)
      return;
      ch->fcult = vnum;
      ch->esteem_cult = 100;

    }
    else if (fac->type == FACTION_SECT)
    {
      if(ch->fsect == vnum)
      return;
      ch->fsect = vnum;
      ch->esteem_sect = 100;

    }
    else
    {
      send_to_char("Join attempt failed, society does not have a type.\n\r", ch);
      return;
    }
    ch->faction = vnum;

    if(ch->pcdata->guest_type == GUEST_OPERATIVE)
    return;


    for (i = 0; i < 99 && safe_strlen(fac->member_names[i]) > 1; i++) {
    }

    free_string(fac->member_names[i]);
    fac->member_names[i] = str_dup(ch->name);
    if(fac->type == FACTION_CORE)
    fac->member_esteem[i] = ch->esteem_faction;
    if(fac->type == FACTION_SECT)
    fac->member_esteem[i] = ch->esteem_sect;
    if(fac->type == FACTION_CULT)
    fac->member_esteem[i] = ch->esteem_cult;


    char buf[MSL];
    sprintf(buf, "%s joins %s.", ch->name, fac->name);
    send_log(fac->vnum, buf);
    printf_to_char(ch, "You join %s.\n\r", fac->name);
  }

  void vassal_to_clan(CHAR_DATA *ch, int vnum) {
    FACTION_TYPE *fac = clan_lookup(vnum);
    int i;

    if (fac == NULL || fac == nullfac) {
      send_to_char("Join attempt failed.\n\r", ch);
      return;
    }
    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->vassal_names[i], ch->name)) {
        return;
      }
    }
    for (i = 0; i < 99 && safe_strlen(fac->vassal_names[i]) > 1; i++) {
    }

    free_string(fac->vassal_names[i]);
    fac->vassal_names[i] = str_dup(ch->name);
    ch->vassal = vnum;
    fac->vassal_tier[i] = get_tier(ch);
    printf_to_char(ch, "You become a vassal of %s.\n\r", fac->name);
    char buf[MSL];
    sprintf(buf, "%s becomes a vassal of %s.", ch->name, fac->name);
    send_log(fac->vnum, buf);

  }

  void add_to_exroster(char *name, int vnum, bool quit) {
    FACTION_TYPE *fac = clan_lookup(vnum);
    int i;

    if (fac == NULL || fac == nullfac) {
      return;
    }
    if (generic_faction_vnum(vnum) == TRUE)
    return;
    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->exmember_names[i], name)) {
        return;
      }
    }
    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], name)) {
        if (fac->member_highest_rank[i] <= 1)
        return;
      }
    }
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(fac->exmember_names[i]) < 2) {
        free_string(fac->exmember_names[i]);
        fac->exmember_names[i] = str_dup(name);
        if (quit == TRUE)
        fac->exmember_quit[i] = 1;
        else
        fac->exmember_quit[i] = 0;
        if (quit == TRUE && fac->attributes[FACTION_LOYALTY] == 1)
        fac->exmember_loyalty[i] = 1;
        else
        fac->exmember_loyalty[i] = 0;
        return;
      }
    }
  }

  void join_to_clan_two(CHAR_DATA *ch, int vnum) {
    FACTION_TYPE *fac = clan_lookup(vnum);
    int i;

    if (fac == NULL || fac == nullfac) {
      send_to_char("Join attempt failed.\n\r", ch);
      return;
    }
    if (ch->factiontwo != 0 && ch->factiontwo != vnum)
    remove_from_clanroster(ch->name, ch->factiontwo);


    if (fac->type == FACTION_CORE)
    {
      ch->fcore = vnum;
      if(ch->esteem_faction > 100000)
      ch->esteem_faction = 100000;
    }
    else if (fac->type == FACTION_CULT)
    {
      ch->fcult = vnum;
      if(ch->esteem_cult > 100000)
      ch->esteem_cult = 100000;

    }
    else if (fac->type == FACTION_SECT)
    {
      ch->fsect = vnum;
      if(ch->esteem_sect > 100000)
      ch->esteem_sect = 100000;

    }
    else
    {
      send_to_char("Join attempt failed, society does not have a type.\n\r", ch);
      return;
    }

    if(ch->pcdata->guest_type == GUEST_OPERATIVE)
    return;

    for (i = 0; i < 99 && safe_strlen(fac->member_names[i]) > 1; i++) {
    }
    free_string(fac->member_names[i]);
    fac->member_names[i] = str_dup(ch->name);
    if(fac->type == FACTION_CORE)
    fac->member_esteem[i] = ch->esteem_faction;
    if(fac->type == FACTION_SECT)
    fac->member_esteem[i] = ch->esteem_sect;
    if(fac->type == FACTION_CULT)
    fac->member_esteem[i] = ch->esteem_cult;

    printf_to_char(ch, "You join %s.\n\r", fac->name);
  }

  void remove_from_clanroster(char *name, int vnum) {
    if (vnum == 0 || vnum > 20000)
    return;

    FACTION_TYPE *fac = clan_lookup(vnum);
    int i;
    if (fac == NULL || fac == nullfac) {
      return;
    }


    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], name)) {
        free_string(fac->member_names[i]);
        fac->member_names[i] = str_dup("");
        fac->member_esteem[i] = 0;
        fac->member_inactivity[i] = 0;
        fac->member_rank[i] = 0;
        fac->member_highest_rank[i] = 0;
        fac->member_position[i] = 0;
        fac->member_suspended[i] = 0;
        fac->member_pay[i] = 0;
        fac->member_tier[i] = 0;
        free_string(fac->member_rosternotes[i]);
        fac->member_rosternotes[i] = str_dup("");
        free_string(fac->member_filenotes[i]);
        fac->member_filenotes[i] = str_dup("");
      }
    }
    if (!str_cmp(fac->leader, name)) {
      free_string(fac->leader);
      fac->leader = str_dup("");
      fac->leaderesteem = 0;
      fac->leaderinactivity = 0;
    }


    if ((victim = get_char_world_pc(name)) !=
        NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }
    if(victim->faction == vnum)
    victim->faction = 0;
    if(victim->factiontwo == vnum)
    victim->factiontwo = 0;
    if(victim->fcore == vnum)
    victim->fcore = 0;
    if(victim->fcult == vnum)
    victim->fcult = 0;
    if(victim->fsect == vnum)
    victim->fsect = 0;
    if(victim->factiontrue == vnum)
    victim->factiontrue = 0;

    char lbuf[MSL];
    sprintf(lbuf, "%s leaves %s.", victim->name, fac->name);
    send_log(fac->vnum, lbuf);

    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);


  }

  void remove_from_vassalroster(char *name, int vnum) {
    if (vnum == 0 || vnum > 20000)
    return;

    FACTION_TYPE *fac = clan_lookup(vnum);
    int i;
    if (fac == NULL || fac == nullfac) {
      return;
    }

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->vassal_names[i], name)) {
        free_string(fac->vassal_names[i]);
        fac->vassal_names[i] = str_dup("");
        fac->vassal_inactivity[i] = 0;
        fac->vassal_tier[i] = 0;
      }
    }
  }

  bool leadotherfac(char *lname, int vnum) {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0) {
        continue;
      }
      if ((*it)->stasis == 1)
      continue;
      if ((*it)->vnum == vnum)
      continue;
      if (!str_cmp(lname, (*it)->leader))
      return TRUE;
    }
    return FALSE;
  }

  void clan_update() {
    FACTION_TYPE *fac;
    int i;

    save_clans(TRUE);
    save_cabals(TRUE);

    cabal_update();

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      fac = (*it);
      int leaderrank = 1;

      if (fac->antagonist == 1)
      fac->alliance = 9;
      if (fac->ritual_timer > 0) {
        if (fac->ritual_orderer == NULL || !can_spend_resources(fac))
        fac->ritual_timer = 0;
        else {
          fac->ritual_timer -= 15;
          if (fac->ritual_timer <= 2) {
            int amount = execute_npc_ritual(fac->ritual_string);
            if (amount <= 0 && fac->ritual_orderer != NULL) {
              printf_to_char(fac->ritual_orderer, "The ritual fails.\n\r");
              fac->ritual_timer = 0;
            }
            else {
              printf_to_char(fac->ritual_orderer, "Your ordered ritual completes.\n\r");
              use_resources(amount / 5, fac->vnum, NULL, "a ritual");
              fac->ritual_timer = 0;
            }
          }
        }
      }

      bool unset = FALSE;
      for (int i = 1; i <= AXES_MAX; i++) {
        if (fac->axes[i] == 0)
        unset = TRUE;
      }
      if (unset == FALSE && fac->alliance == 0 && fac->cardinal == 0 && fac->antagonist == 0 && fac->outcast == 0 && !generic_faction_vnum(fac->vnum)) {
        if (fac->type == FACTION_CULT) {
          fac->alliance = get_alliance(fac, time_info.cult_alliance_issue, time_info.cult_alliance_type);
          char buf[MSL];
          sprintf(buf, "You join The %s Alliance.", alliance_names(time_info.cult_alliance_issue, fac->alliance, time_info.cult_alliance_type));
          send_message(fac->vnum, buf);
        }
        else {
          fac->alliance = get_alliance(fac, time_info.sect_alliance_issue, time_info.sect_alliance_type);
          char buf[MSL];
          sprintf(buf, "You join The %s Alliance.", alliance_names(time_info.sect_alliance_issue, fac->alliance, time_info.sect_alliance_type));
          send_message(fac->vnum, buf);
        }
      }
      if (generic_faction_vnum(fac->vnum))
      fac->alliance = fac->vnum;

      for (i = 1; i < 20; i++) {
        if (fac->log_timer[i - 1] <= 0) {
          free_string(fac->log[i - 1]);
          fac->log[i - 1] = str_dup(fac->log[i]);
          fac->log_timer[i - 1] = fac->log_timer[i];
          fac->log_timer[i] = 0;
          free_string(fac->log[i]);
          fac->log[i] = str_dup("");
        }
      }

      for (i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1) {
          if (fac->member_esteem[i] < rank_requirement(fac->member_rank[i]) && fac->member_rank[i] > 0)
          fac->member_rank[i]--;
          if (fac->member_esteem[i] >
              (rank_requirement(fac->member_rank[i] + 1) + 100) && fac->member_rank[i] < 6)
          fac->member_rank[i]++;
          else if (fac->member_esteem[i] >
              (rank_requirement(fac->member_rank[i] + 1) + 100) && fac->member_rank[i] == 6) {
            if (get_rank_respect(fac, 7) < fac->member_esteem[i] - 5000) {
              for (int j = 0; j < 100; j++) {
                if (fac->member_rank[j] == 7)
                fac->member_rank[j]--;
              }
              fac->member_rank[i]++;
            }
          }
          else if (fac->member_esteem[i] >
              (rank_requirement(fac->member_rank[i] + 1) + 100) && fac->member_rank[i] == 7) {
            if (get_rank_respect(fac, 8) < fac->member_esteem[i] - 5000) {
              for (int j = 0; j < 100; j++) {
                if (fac->member_rank[j] == 8)
                fac->member_rank[j]--;
              }
              fac->member_rank[i]++;
            }
          }
          else if (fac->member_esteem[i] >
              (rank_requirement(fac->member_rank[i] + 1) + 100) && fac->member_rank[i] == 8) {
            if (get_rank_respect(fac, 9) < fac->member_esteem[i] - 5000) {
              for (int j = 0; j < 100; j++) {
                if (fac->member_rank[j] == 9)
                fac->member_rank[j]--;
              }
              fac->member_rank[i]++;
            }
          }
          if (fac->member_rank[i] > fac->member_highest_rank[i])
          fac->member_highest_rank[i] = fac->member_rank[i];

          if (get_char_world_pc(fac->member_names[i]) != NULL && !IS_FLAG(get_char_world_pc(fac->member_names[i])->act, PLR_DEAD)) {
            CHAR_DATA *member = get_char_world_pc(fac->member_names[i]);
            if (!str_cmp(fac->leader, member->name)) {
              if (cardinal(member))
              fac->cardinal = 1;
              else if (staff_char(member))
              fac->staff = 1;
              else {
                fac->cardinal = 0;
                fac->staff = 0;
              }
            }
            if (member->pcdata->job_type_one == JOB_EMPLOYEE)
            fac->member_flags[i] = 1;
            else if (is_guest(member) && member->pcdata->guest_type != GUEST_NIGHTMARE)
            fac->member_flags[i] = 2;
            else
            fac->member_flags[i] = 0;
            bool removed = FALSE;
            fac->member_tier[i] = get_tier(member);
            char buf[MSL];
            if (!seems_super(member) && (fac->axes[AXES_SUPERNATURAL] == AXES_FARLEFT) && event_cleanse == 0 && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];

              sprintf(buf, "CLAN DEMOTION HUMAN %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (fac->vnum == FACTION_HAND && combat_focus(member) > 1 && combat_focus(member) > arcane_focus(member) && combat_focus(member) > prof_focus(member) && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION COMBAT HAND %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (fac->vnum == FACTION_ORDER && prof_focus(member) > 1 && prof_focus(member) > arcane_focus(member) && prof_focus(member) > combat_focus(member) && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION PROF ORDER %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (fac->vnum == FACTION_TEMPLE && arcane_focus(member) > 1 && arcane_focus(member) > combat_focus(member) && arcane_focus(member) > prof_focus(member) && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION ARCANE TEMPLE %s:%s.", fac->name, member->name);
              log_string(buf);
            }

            if (seems_super(member) && (fac->axes[AXES_SUPERNATURAL] == AXES_FARRIGHT) && event_cleanse == 0 && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION SUPER %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (fac->axes[AXES_CORRUPT] == AXES_FARLEFT && get_tier(member) < 3 && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION LOW TIER %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (fac->axes[AXES_CORRUPT] <= AXES_NEARLEFT && get_tier(member) < 2 && fac->axes[AXES_CORRUPT] > 0 && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION VERY LOW TIER %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (college_student(member, TRUE) && fac->college == 0 && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(3));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION COLLEGE %s:%s.", fac->name, member->name);
              log_string(buf);
            }
            if (!faction_elligible(member, fac, FALSE, NULL) && fac->member_esteem[i] >
                UMIN(fac->member_esteem[i], rank_requirement(3))) {
              if (fac->member_esteem[i] >
                  1000 + UMIN(fac->member_esteem[i], rank_requirement(3))) {
                printf_to_char(member, "In %s\n\r", fac->name);
                if (!faction_elligible(member, fac, TRUE, member))
                send_to_char("\n\n\n\r", member);
              }
              fac->member_esteem[i] =
              UMIN(fac->member_esteem[i], rank_requirement(1));
              if (member->fcore == fac->vnum)
              member->esteem_faction = fac->member_esteem[i];
              else if (member->fsect == fac->vnum)
              member->esteem_sect = fac->member_esteem[i];
              else if (member->fcult == fac->vnum)
              member->esteem_cult = fac->member_esteem[i];
              sprintf(buf, "CLAN DEMOTION INELLIGIBLE %s:%s.", fac->name, member->name);
              log_string(buf);
            }

            /*
if(member->faction == fac->vnum)
{

if(leadotherfac(member->name, fac->vnum) && (generic_faction_vnum(fac->vnum) || protected_faction_vnum(fac->vnum)))
{
fac->member_esteem[i] =
UMIN(fac->member_esteem[i], get_rank_respect(fac, 1));
member->esteem = fac->member_esteem[i];
}

}
if(member->factiontwo == fac->vnum)
{
if(leadotherfac(member->name, fac->vnum) && (generic_faction_vnum(fac->vnum) || protected_faction_vnum(fac->vnum)))
{
fac->member_esteem[i] =
UMIN(fac->member_esteem[i], get_rank_respect(fac, 1));
member->esteem = fac->member_esteem[i];
}

}
*/
            if ((fac->soft_restrict == 0 && !college_student(member, FALSE) && !clinic_patient(member) && !faction_elligible(member, fac, TRUE, member)) || !faction_hardelligible(member, fac, TRUE, member)) {
              add_to_exroster(fac->member_names[i], fac->vnum, TRUE);
              remove_from_clanroster(fac->member_names[i], fac->vnum);
              removed = TRUE;
              if (member->faction == fac->vnum)
              member->faction = 0;
              if (member->factiontwo == fac->vnum)
              member->factiontwo = 0;
              if(member->fcore == fac->vnum)
              member->fcore = 0;
              if(member->fsect == fac->vnum)
              member->fsect = 0;
              if(member->fcult == fac->vnum)
              member->fcult = 0;

            }
            if (removed == FALSE) {
              fac->member_inactivity[i] = 0;
              if (!str_cmp(fac->member_names[i], fac->leader) && fac->axes[AXES_DEMOCRATIC] < AXES_NEARRIGHT && !generic_faction_vnum(fac->vnum)) {
                // get_char_world_pc(fac->member_names[i])->esteem = 1000000;
                fac->leaderesteem = 1000000;
                fac->member_esteem[i] = 1000000;
              }
              if (str_cmp(fac->member_names[i], fac->leader) && !generic_faction_vnum(fac->vnum) && !protected_faction_vnum(fac->vnum)) {
                fac->member_esteem[i] = UMIN(500000, fac->member_esteem[i]);
                fac->member_rank[i] = UMIN(8, fac->member_rank[i]);
              }
              if (fac->axes[AXES_DEMOCRATIC] >= AXES_NEARRIGHT || str_cmp(fac->member_names[i], fac->leader) || generic_faction_vnum(fac->vnum)) {
                if (fac->vnum == member->fcore) {
                  fac->member_esteem[i] = member->esteem_faction;
                  if (!str_cmp(fac->member_names[i], fac->leader))
                  fac->leaderesteem = member->esteem_faction;
                }
                if (fac->vnum == member->fcult) {
                  fac->member_esteem[i] = member->esteem_cult;
                  if (!str_cmp(fac->member_names[i], fac->leader))
                  fac->leaderesteem = member->esteem_cult;
                }
                if (fac->vnum == member->fsect) {
                  fac->member_esteem[i] = member->esteem_sect;
                  if (!str_cmp(fac->member_names[i], fac->leader))
                  fac->leaderesteem = member->esteem_sect;
                }

              }
            }

          }
          else {

            if (fac->stasis == 0) {
              fac->member_inactivity[i]++;
              if (fac->member_inactivity[i] > 1200 || (fac->member_inactivity[i] > 100 && daysidle(fac->member_names[i]) > 21))
              remove_from_clanroster(fac->member_names[i], fac->vnum);
              else if (number_percent() % 8 == 0) {
                for (int l = 0; l < 100; l++) {
                  if (!str_cmp(fac->member_names[i], fac->member_names[l]) && safe_strlen(fac->member_names[i]) > 2) {
                    if (l > i) {
                      free_string(fac->member_names[l]);
                      fac->member_names[l] = str_dup("");
                      fac->member_rank[l] = 0;
                      fac->member_tier[l] = 0;
                    }
                  }
                }
              }
            }
          }
        }
      }
      for (i = 0; i < 100; i++) {
        if (safe_strlen(fac->exmember_names[i]) > 1) {
          if (in_fac_name(fac->exmember_names[i], fac->vnum)) {
            free_string(fac->exmember_names[i]);
            fac->exmember_names[i] = str_dup("");
          }
          else if (get_char_world_pc(fac->exmember_names[i]) != NULL && !IS_FLAG(get_char_world_pc(fac->exmember_names[i])->act, PLR_DEAD))
          fac->exmember_inactivity[i] = 0;
          else if (fac->exmember_inactivity[i] < 5000)
          fac->exmember_inactivity[i]++;
        }
      }

      if (fac->axes[AXES_DEMOCRATIC] >= AXES_NEARRIGHT)
      leaderrank = 0;
      for (i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 2 && !str_cmp(fac->member_names[i], fac->leader)) {
          if (fac->axes[AXES_DEMOCRATIC] >= AXES_NEARRIGHT || generic_faction_vnum(fac->vnum))
          leaderrank = fac->member_rank[i];
          else
          leaderrank = 10;

          if (fac->member_noleader[i] == 1)
          leaderrank = -1;
        }
      }
      if (safe_strlen(fac->leader) < 3)
      leaderrank = 0;
      for (i = 0; i < 100; i++) {
        if ((fac->stasis == 1 || !can_spend_resources(fac)) && safe_strlen(fac->vassal_names[i]) > 1)
        remove_from_vassalroster(fac->vassal_names[i], fac->vnum);
        else if (get_char_world_pc(fac->vassal_names[i]) != NULL && !IS_FLAG(get_char_world_pc(fac->vassal_names[i])->act, PLR_DEAD)) {
          fac->vassal_inactivity[i] = 0;
          fac->vassal_tier[i] = get_tier(get_char_world_pc(fac->vassal_names[i]));
        }
        else {
          fac->vassal_inactivity[i]++;
          if (fac->vassal_inactivity[i] > 1500 || (fac->vassal_inactivity[i] > 100 && daysidle(fac->vassal_names[i]) > 21))
          remove_from_vassalroster(fac->vassal_names[i], fac->vnum);
          else if (number_percent() % 8 == 0) {
            for (int l = 0; l < 100; l++) {
              if (!str_cmp(fac->vassal_names[i], fac->vassal_names[l]) && safe_strlen(fac->vassal_names[i]) > 2) {
                if (l > i) {
                  free_string(fac->vassal_names[l]);
                  fac->vassal_names[l] = str_dup("");
                  fac->vassal_tier[l] = 0;
                }
              }
            }
          }
        }
      }

      for (i = 0; i < 100; i++) {
        if (fac->member_rank[i] > leaderrank && fac->member_noleader[i] == 0) {
          if (get_char_world_pc(fac->member_names[i]) != NULL && !IS_FLAG(
                get_char_world_pc(fac->member_names[i])->pcdata->account->flags, ACCOUNT_NOKILL) && faction_elligible(get_char_world_pc(fac->member_names[i]), fac, FALSE, get_char_world_pc(fac->member_names[i]))) {
            free_string(fac->leader);
            fac->leader = str_dup(fac->member_names[i]);
            leaderrank = fac->member_rank[i];
            fac->leaderesteem = fac->member_esteem[i];
          }
        }
      }

      for (i = 0; i < 99; i++) {
        if (fac->member_rank[i] < fac->member_rank[i + 1]) {

          char buf[MSL];
          sprintf(buf, "CLANSWITCH: %s, %s, %d, %d\n\r", fac->member_names[i], fac->member_names[i + 1], fac->member_esteem[i], fac->member_esteem[i + 1]);
          log_string(buf);

          char *tempname;
          tempname = str_dup(fac->member_names[i]);
          int temprank = fac->member_rank[i];
          int tempesteem = fac->member_esteem[i];
          int tempposition = fac->member_position[i];
          int tempsuspended = fac->member_suspended[i];
          int temppay = fac->member_pay[i];
          int temptier = fac->member_tier[i];
          int tempnoleader = fac->member_noleader[i];
          char *temprosternote;
          temprosternote = str_dup(fac->member_rosternotes[i]);
          char *tempfilenote;
          tempfilenote = str_dup(fac->member_filenotes[i]);
          free_string(fac->member_names[i]);
          fac->member_names[i] = str_dup(fac->member_names[i + 1]);
          free_string(fac->member_rosternotes[i]);
          fac->member_rosternotes[i] = str_dup(fac->member_rosternotes[i + 1]);
          free_string(fac->member_filenotes[i]);
          fac->member_filenotes[i] = str_dup(fac->member_filenotes[i + 1]);
          fac->member_rank[i] = fac->member_rank[i + 1];
          fac->member_esteem[i] = fac->member_esteem[i + 1];
          fac->member_position[i] = fac->member_position[i + 1];
          fac->member_tier[i] = fac->member_tier[i + 1];
          fac->member_suspended[i] = fac->member_suspended[i + 1];
          fac->member_pay[i] = fac->member_pay[i + 1];
          fac->member_noleader[i] = fac->member_noleader[i + 1];
          free_string(fac->member_names[i + 1]);
          fac->member_names[i + 1] = str_dup(tempname);
          free_string(fac->member_rosternotes[i + 1]);
          fac->member_rosternotes[i + 1] = str_dup(temprosternote);
          free_string(fac->member_filenotes[i + 1]);
          fac->member_filenotes[i + 1] = str_dup(tempfilenote);
          fac->member_rank[i + 1] = temprank;
          fac->member_esteem[i + 1] = tempesteem;
          fac->member_position[i + 1] = tempposition;
          fac->member_suspended[i + 1] = tempsuspended;
          fac->member_pay[i + 1] = temppay;
          fac->member_noleader[i + 1] = tempnoleader;
          fac->member_tier[i + 1] = temptier;
        }
      }

      for (i = 0; i < 20; i++) {
        if (safe_strlen(fac->messages[i]) > 1) {
          fac->message_timer[i]--;
          if (fac->message_timer[i] <= 0) {
            fac->message_timer[i] = 0;
            free_string(fac->messages[i]);
            fac->messages[i] = str_dup("");
          }
        }
      }
      for (i = 0; i < 50; i++) {
        if (safe_strlen(fac->log[i]) > 1) {
          fac->log_timer[i]--;
          if (fac->log_timer[i] <= 0) {
            fac->log_timer[i] = 0;
            free_string(fac->log[i]);
            fac->log[i] = str_dup("");
          }
        }
      }
    }
  }

  void broadcast(int freq, char *arg) {}

  void entering_area(CHAR_DATA *ch, ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to) {
    if (from->area->vnum == to->area->vnum)
    return;
  }
  bool in_fac_name(char *name, int faction) {
    int i;
    FACTION_TYPE *fac = clan_lookup(faction);

    if (fac == NULL)
    return FALSE;

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], name)) {
        return TRUE;
      }
    }
    if (!str_cmp(fac->leader, name))
    return TRUE;

    return FALSE;
  }

  bool in_fac(CHAR_DATA *ch, int faction) {
    int i;
    FACTION_TYPE *fac = clan_lookup(faction);

    if (fac == NULL)
    return FALSE;

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], ch->name)) {
        return TRUE;
      }
    }
    if (!str_cmp(fac->leader, ch->name))
    return TRUE;

    return FALSE;
  }

  void send_message(int faction, char *argument) {
    CHAR_DATA *to;
    char buf[MSL];
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    for (i = 0; i < 20 && safe_strlen(fac->messages[i]) > 2; i++) {
    }
    if (i == 20)
    return;
    free_string(fac->messages[i]);
    sprintf(buf, "[%s] %s", newtexttime(), argument);
    fac->messages[i] = str_dup(buf);
    fac->message_timer[i] = 100;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (to->faction == faction && has_trust(to, TRUST_COMMS, faction) && radio_signal(to) && !is_deaf(to)) {
          if (to->faction != 0 && clan_lookup(to->faction) != NULL && clan_lookup(to->faction)->attributes[FACTION_COMMS] != 0)
          printf_to_char(to, "Your earpiece announces: '%s'\n\r", argument);
        }
        if (to->factiontwo == faction && has_trust(to, TRUST_COMMS, faction) && radio_signal(to) && !is_deaf(to)) {
          if (to->factiontwo != 0 && clan_lookup(to->factiontwo) != NULL && clan_lookup(to->factiontwo)->attributes[FACTION_COMMS] != 0)
          printf_to_char(to, "Your earpiece announces: '%s'\n\r", argument);
        }
      }
    }
  }

  void send_message_temp(int faction, char *argument) {
    CHAR_DATA *to;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (to->faction == faction && has_trust(to, TRUST_COMMS, faction)) {
          if (to->faction != 0 && clan_lookup(to->faction) != NULL && clan_lookup(to->faction)->attributes[FACTION_COMMS] != 0)
          printf_to_char(to, "Your earpiece announces: '%s'\n\r", argument);
        }
        if (to->factiontwo == faction && has_trust(to, TRUST_COMMS, faction)) {
          if (to->factiontwo != 0 && clan_lookup(to->factiontwo) != NULL && clan_lookup(to->factiontwo)->attributes[FACTION_COMMS] != 0)
          printf_to_char(to, "Your earpiece announces: '%s'\n\r", argument);
        }
      }
    }
  }

  bool has_proxy(CHAR_DATA *ch) { return TRUE; }

  _DOFUN(do_factionkick) {
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    FACTION_TYPE *fac;
    fac = clan_lookup_name(argument);
    if (fac == NULL || fac == nullfac) {
      send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
      return;
    }
    CHAR_DATA *victim = get_char_world_pc(arg);
    if (victim == NULL) {
      send_to_char("No such character.\n\r", ch);
      return;
    }
    remove_from_clanroster(victim->name, fac->vnum);
    if (victim->faction == fac->vnum)
    victim->faction = 0;
    if (victim->factiontwo == fac->vnum)
    victim->factiontwo = 0;
  }

  void grief_char(char *name, int hours) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    struct stat sb;
    if (safe_strlen(name) < 2)
    return;

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = 12 * 60 * hours;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_GRIEF;
    affect_to_char(victim, &af);

    if (!online)
    free_char(victim);
  }

  bool has_authority(CHAR_DATA *ch, char *name, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i, j;
    for (i = 0; i < 100 && str_cmp(name, fac->member_names[i]); i++) {
    }
    for (j = 0; j < 100 && str_cmp(ch->name, fac->member_names[j]); j++) {
    }
    if (i == 100) {
      send_to_char("No such member.\n\r", ch);
      return FALSE;
    }

    if (!str_cmp(ch->name, fac->leader))
    return TRUE;

    return TRUE;
  }

  int clan_vnum_by_name(char *argument) {

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0) {
        continue;
      }
      if (!str_cmp((*it)->name, argument))
      return (*it)->vnum;
    }
    return 0;
  }

  char *room_function(ROOM_INDEX_DATA *room) { return "None"; }

  char *terrain_name(int terrain) {
    switch (terrain) {
    case BATTLE_CITY:
      return "in the city of";
      break;
    case BATTLE_TOWN:
      return "in a town near";
      break;
    case BATTLE_FOREST:
      return "in a forest near";
      break;
    case BATTLE_FIELD:
      return "in a field near";
      break;
    case BATTLE_DESERT:
      return "in a desert near";
      break;
    case BATTLE_MOUNTAINS:
      return "in mountains near";
      break;
    case BATTLE_WAREHOUSE:
      return "in a warehouse near";
      break;
    case BATTLE_CAVE:
      return "in caves near";
      break;
    case BATTLE_VILLAGE:
      return "in a village near";
      break;
    case BATTLE_LAKE:
      return "in a lake near";
      break;
    case BATTLE_TUNDRA:
      return "in the tundra near";
      break;
    }
    return "";
  }

  int op_count(FACTION_TYPE *fac) {
    int count = 0;
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->hour == 0)
      continue;

      if ((*it)->faction != fac->vnum && (*it)->competition == COMPETE_CLOSED)
      continue;

      count++;
    }
    return count;
  }
  int op_enrolled_count(FACTION_TYPE *fac) {
    int count = 0;
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->hour == 0)
      continue;

      if ((*it)->faction == fac->vnum)
      count++;
      else if ((*it)->competition != COMPETE_CLOSED) {
        for (int i = 0; i < 10; i++) {
          if ((*it)->enrolled[i] == fac->vnum)
          count++;
        }
      }
    }
    return count;
  }

  bool can_join_faction(CHAR_DATA *ch, int faction, bool show) {
    if (higher_power(ch))
    return FALSE;
    if (clan_lookup(faction) == NULL) {
      if (show == TRUE)
      send_to_char("No such society.\n\r", ch);
      return FALSE;
    }
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac->type == FACTION_CORE && ch->fcore != 0) {
      if (show == TRUE)
      send_to_char("Already in a core faction.\n\r", ch);
      return FALSE;
    }
    if (fac->type == FACTION_CULT && ch->fcult != 0) {
      if (show == TRUE)
      send_to_char("Already in a cult.\n\r", ch);
      return FALSE;
    }
    if (fac->type == FACTION_SECT && ch->fsect != 0) {
      if (show == TRUE)
      send_to_char("Already in a sect.\n\r", ch);
      return FALSE;
    }
    if (!faction_elligible(ch, clan_lookup(faction), TRUE, ch) && clan_lookup(faction)->soft_restrict == 0 && !clinic_patient(ch) && !college_student(ch, FALSE))
    return FALSE;
    if (!faction_hardelligible(ch, clan_lookup(faction), TRUE, ch))
    return FALSE;
    if (clan_lookup(faction)->can_alt == 1)
    return TRUE;
    if (ch->pcdata->account == NULL) {
      if (show == TRUE)
      send_to_char("No account.\n\r", ch);
      return FALSE;
    }
    if (safe_strlen(ch->pcdata->account->lastfacchar) > 1 && str_cmp(ch->name, ch->pcdata->account->lastfacchar) && ch->pcdata->account->factioncooldown > current_time) {
      if (show == TRUE)
      send_to_char("Already have, or recently have had, a character in a non alt society.\n\r", ch);
      return FALSE;
    }

    return TRUE;
  }

  bool visibly_online(char *name) {
    CHAR_DATA *ch = get_char_world_pc(name);
    if (ch == NULL)
    return FALSE;
    if (IS_FLAG(ch->comm, COMM_AFK))
    return FALSE;
    if (!IS_FLAG(ch->comm, COMM_WHOINVIS))
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_faction) {
    ch->pcdata->ftype = FACTION_CORE;
    do_function(ch, &do_clan, argument);
  }

  _DOFUN(do_sect) {
    ch->pcdata->ftype = FACTION_SECT;
    do_function(ch, &do_clan, argument);
  }

  _DOFUN(do_cult) {
    ch->pcdata->ftype = FACTION_CULT;
    do_function(ch, &do_clan, argument);
  }

  _DOFUN(do_clan) {
    char arg[MSL];
    char arg2[MSL];
    char *ctype;
    FACTION_TYPE *fac = NULL;

    if(battleground(ch->in_room))
    return;

    if (ch->pcdata->ftype == FACTION_CORE) {
      fac = clan_lookup(ch->fcore);
      ch->faction = ch->fcore;
      ctype = str_dup("Faction");
    }
    else if (ch->pcdata->ftype == FACTION_SECT) {
      fac = clan_lookup(ch->fsect);
      ch->faction = ch->fsect;
      ctype = str_dup("Sect");
    }
    else if (ch->pcdata->ftype == FACTION_CULT) {
      fac = clan_lookup(ch->fcult);
      ch->faction = ch->fcult;
      ctype = str_dup("Cult");
    }

    int i = 0, j;
    bool online = FALSE;
    DESCRIPTOR_DATA d;
    Buffer outbuf;
    char buf[MSL];
    CHAR_DATA *victim;

    argument = one_argument_nouncap(argument, arg);

    if (!str_cmp(arg, "positiondiffs") && IS_IMMORTAL(ch)) {
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        for (vector<FACTION_TYPE *>::iterator ij = FacVect.begin();
        ij != FacVect.end(); ++ij) {
          printf_to_char(ch, "%s : %s : %d\n\r", (*it)->name, (*ij)->name, position_difference((*it), (*ij)));
        }
      }

      return;
    }

    if (battleground(ch->in_room))
    return;

    if (!str_cmp(arg, "join")) {
      if (higher_power(ch))
      return;
      if (is_helpless(ch) || in_fight(ch) || is_pinned(ch) || is_ghost(ch) || room_hostile(ch->in_room)) {
        send_to_char("You're a bit busy.\n\r", ch);
        return;
      }

      // Keeping institute PCs out of factions - Discordance
      if (ch->race == RACE_FACULTY) {
        printf_to_char(ch, "White Oak Faculty cannot join %ss.\n\r", ctype);
        return;
      }

      if(clan_vnum_by_name(argument) < 1)
      {
        printf_to_char(ch, "%s join (group name).\n\r", ctype);
        return;
      }
      if(clan_lookup(clan_vnum_by_name(argument)) == NULL)
      {
        printf_to_char(ch, "%s join (group name).\n\r", ctype);
        return;
      }


      if (ch->faction == 0 && ch->pcdata->lastidentity != 0 && ch->played / 3600 < 10) {
        if (clan_vnum_by_name(argument) > 0 && clan_lookup(clan_vnum_by_name(argument))->closed == 0 && can_join_faction(ch, clan_vnum_by_name(argument), TRUE))
        join_to_clan(ch, clan_vnum_by_name(argument));
        else
        printf_to_char(ch, "%s join (group name).\n\r", ctype);
        return;
      }
      if(ch->pcdata->ftype == FACTION_CORE)
      {
        if(ch->fcore != 0)
        {
          send_to_char("You are already in a core faction.\n\r", ch);
          return;
        }
        /*
if (ch->desc->account != NULL && ch->desc->account->factiontime != 0 && ch->desc->account->factiontime > current_time) {
send_to_char("You rolled a society character too recently, you will have to join up ICly in game.\n\r", ch);
return;
}
*/
      }
      if(ch->pcdata->ftype == FACTION_SECT)
      {
        if(ch->fsect != 0)
        {
          send_to_char("You are already in a sect.\n\r", ch);
          return;
        }
        if(clan_lookup(clan_vnum_by_name(argument))->closed == 1) {
          send_to_char("That society has to be joined ICly in game.\n\r", ch);
          return;
        }
      }
      if(ch->pcdata->ftype == FACTION_CULT)
      {
        if(ch->fcult != 0)
        {
          send_to_char("You are already in a cult.\n\r", ch);
          return;
        }
        if(clan_lookup(clan_vnum_by_name(argument))->closed == 1) {
          send_to_char("That society has to be joined ICly in game.\n\r", ch);
          return;
        }
      }
      join_to_clan(ch, clan_vnum_by_name(argument));
      printf_to_char(ch, "You join %s.\n\r", clan_lookup(clan_vnum_by_name(argument))->name);


    }

    if(is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "create")) {
      if (ch->faction != 0) {
        send_to_char("You're already in a society like that.\n\r", ch);
        return;
      }
      if (higher_power(ch))
      return;

      if (safe_strlen(argument) < 3) {
        printf_to_char(ch, "Syntax: %s create (name)\n\r", ctype);
        return;
      }
      /*
if (get_day() < 20 && !IS_IMMORTAL(ch) && !IS_FLAG(ch->act, PLR_GUEST)) {
send_to_char("You can only do this after the 20th of the month.\n\r", ch);
return;
}
*/
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOKILL)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      if (ch->pcdata->influence + ch->pcdata->scheme_influence < 35000) {
        send_to_char("You need at least 35k influence to do that.\n\r", ch);
        return;
      }
      //    int creation = UMIN(60000, ch->pcdata->influence +
      //    ch->pcdata->scheme_influence);
      int creation =
      UMIN(125000, ch->pcdata->influence + ch->pcdata->scheme_influence);

      ch->pcdata->scheme_influence -= creation;
      if (ch->pcdata->scheme_influence < 0) {
        ch->pcdata->influence += ch->pcdata->scheme_influence;
        ch->pcdata->scheme_influence = 0;
      }
      if (ch->faction != 0) {
        ch->factiontwo = ch->faction;
        ch->faction = 0;
      }
      creation -= 25000;
      creation /= 2;

      FACTION_TYPE *newfac;
      newfac = new_faction();
      newfac->vnum = time_info.faction_vnum;
      time_info.faction_vnum++;
      char nfac[MSL];
      remove_color(nfac, argument);
      free_string(newfac->name);
      newfac->name = str_dup(nfac);
      newfac->can_alt = 1;
      newfac->resource = UMIN(9000, 4000 + creation / 8);
      //    newfac->resource = 2500 + creation/5;
      FacVect.push_back(newfac);
      free_string(newfac->leader);
      newfac->weekly_ops = 0;
      newfac->type = ch->pcdata->ftype;
      newfac->leader = str_dup(ch->name);
      newfac->leaderesteem = 1000000;
      newfac->member_esteem[0] = 1000000;
      newfac->last_operation = current_time;
      newfac->last_intel = current_time;
      newfac->last_deploy = current_time;
      printf_to_char(ch, "You found %s\n\r", newfac->name);
      newfac->lifeearned = newfac->resource / 20;
      join_to_clan(ch, newfac->vnum);
      if (college_student(ch, TRUE)) {
        newfac->college = 1;
      }

      return;
    }

    if (!str_cmp(arg, "forcecyle") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      faction_daily(fac);
      return;
    }
    if (!str_cmp(arg, "college") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      if(fac->college == 0)
      fac->college = 1;
      else
      fac->college = 0;
      return;
    }
    if (!str_cmp(arg, "redesc") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      string_append(ch, &fac->description);
      return;
    }


    if (!str_cmp(arg, "resetweekly") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->weekly_ops = 0;
      send_to_char("Done.\n\r", ch);
    }
    if (!str_cmp(arg, "roster") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      for (i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1) {
          printf_to_char(ch, "`W[`x%10.10s`W][`x%02d`W]`x %s %s\n\r", fac->ranks[fac->member_position[i]], fac->member_rank[i] + 1, fac->member_names[i], fac->member_rosternotes[i]);
          if (get_char_world_pc(fac->member_names[i]) != NULL) {
            if (!faction_hardelligible(get_char_world_pc(fac->member_names[i]), fac, TRUE, ch))
            send_to_char("Hard Inelligible.\n\r", ch);
            if (!faction_elligible(get_char_world_pc(fac->member_names[i]), fac, TRUE, ch))
            send_to_char("Inelligible.\n\r", ch);
          }
        }
      }
      return;
    }

    if (!str_cmp(arg, "hadop") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->last_operation = current_time;
      send_to_char("Done.\n\r", ch);
    }
    if (!str_cmp(arg, "setalt") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->can_alt = 1;
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "beenop") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->last_deploy = current_time;
      send_to_char("Done.\n\r", ch);
    }

    if (!str_cmp(arg, "setrank") && IS_IMMORTAL(ch)) {
      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg2);
      argument = one_argument_nouncap(argument, arg3);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      for (int i = 0; i < 100; i++) {
        if (!str_cmp(arg2, fac->member_names[i])) {
          fac->member_esteem[i] = rank_requirement(atoi(arg3));
          if (get_char_world_pc(fac->member_names[i]) != NULL && get_char_world_pc(fac->member_names[i])->fcore == fac->vnum)
          get_char_world_pc(fac->member_names[i])->esteem_faction =
          rank_requirement(atoi(arg3));
          if (get_char_world_pc(fac->member_names[i]) != NULL && get_char_world_pc(fac->member_names[i])->fsect == fac->vnum)
          get_char_world_pc(fac->member_names[i])->esteem_sect =
          rank_requirement(atoi(arg3));
          if (get_char_world_pc(fac->member_names[i]) != NULL && get_char_world_pc(fac->member_names[i])->fcult == fac->vnum)
          get_char_world_pc(fac->member_names[i])->esteem_cult =
          rank_requirement(atoi(arg3));

          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    if (!str_cmp(arg, "newleader") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      free_string(fac->leader);
      fac->leader = str_dup(arg2);
      if (get_char_world_pc(arg2) != NULL) {
        if (!in_fac(get_char_world_pc(arg2), fac->vnum))
        join_to_clan(get_char_world_pc(arg2), fac->vnum);
      }
      return;
    }
    if (!str_cmp(arg, "setvnum") && IS_IMMORTAL(ch)) {
      time_info.faction_vnum = atoi(argument);
      return;
    }
    if (!str_cmp(arg, "newvnum") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->vnum = atoi(arg2);
      return;
    }
    if (!str_cmp(arg, "support") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      int v = atoi(arg2);
      if (v < 0 || v > 5) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->support = v;
      printf_to_char(ch, "You set the support of %s to %d.\n\r", fac->name, fac->support);
      return;
    }
    if (!str_cmp(arg, "outcast") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction outcast (faction)\n\r", ch);
        return;
      }
      fac->outcast = 1;
      fac->alliance = 0;
      printf_to_char(ch, "You outcast %s.\n\r", fac->name);
      return;
    }
    if (!str_cmp(arg, "noalliance") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction outcast (faction)\n\r", ch);
        return;
      }
      fac->alliance = 0;
      printf_to_char(ch, "You noalliance %s.\n\r", fac->name);
      return;
    }

    if (!str_cmp(arg, "roleswap") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction roleswap (faction)\n\r", ch);
        return;
      }
      char *temp = str_dup(fac->missions);
      free_string(fac->missions);
      fac->missions = str_dup(fac->roles);
      free_string(fac->roles);
      fac->roles = str_dup(temp);
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "newleader") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction newleader (name) (faction)\n\r", ch);
        return;
      }
      free_string(fac->leader);
      fac->leader = str_dup(arg2);
      send_to_char("Done2.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "newparent") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction newparent (0-5) (faction)\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "Hand")) {
        fac->parent = FACTION_HAND;
      }
    }
    if (!str_cmp(arg, "newleader") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      free_string(fac->leader);
      fac->leader = str_dup(arg2);
      if (get_char_world_pc(arg2) != NULL) {
        if (!in_fac(get_char_world_pc(arg2), fac->vnum))
        join_to_clan(get_char_world_pc(arg2), fac->vnum);
      }
      return;
    }
    if (!str_cmp(arg, "newvnum") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->vnum = atoi(arg2);
      return;
    }
    if (!str_cmp(arg, "support") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      int v = atoi(arg2);
      if (v < 0 || v > 5) {
        send_to_char("Syntax: faction support (0-5) (faction)\n\r", ch);
        return;
      }
      fac->support = v;
      printf_to_char(ch, "You set the support of %s to %d.\n\r", fac->name, fac->support);
      return;
    }
    if (!str_cmp(arg, "outcast") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction outcast (faction)\n\r", ch);
        return;
      }
      fac->outcast = 1;
      fac->alliance = 0;
      printf_to_char(ch, "You outcast %s.\n\r", fac->name);
      return;
    }
    if (!str_cmp(arg, "roleswap") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction roleswap (faction)\n\r", ch);
        return;
      }
      char *temp = str_dup(fac->missions);
      free_string(fac->missions);
      fac->missions = str_dup(fac->roles);
      free_string(fac->roles);
      fac->roles = str_dup(temp);
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "newleader") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction newleader (name) (faction)\n\r", ch);
        return;
      }
      free_string(fac->leader);
      fac->leader = str_dup(arg2);
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "newparent") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction newparent (0-5) (faction)\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "Hand")) {
        fac->parent = FACTION_HAND;
      }
      else if (!str_cmp(arg2, "Order")) {
        fac->parent = FACTION_ORDER;
      }
      else if (!str_cmp(arg2, "Temple")) {
        fac->parent = FACTION_TEMPLE;
      }
      else if (!str_cmp(arg2, "None")) {
        fac->parent = 0;
      }
    }
    if (!str_cmp(arg, "secrecy") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction newparent (0-5) (faction)\n\r", ch);
        return;
      }
      printf_to_char(ch, "Secrecy: %d\n\r", faction_secrecy(fac, ch));
    }

    if (!str_cmp(arg, "delete") && IS_IMMORTAL(ch)) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction delete (faction)\n\r", ch);
        return;
      }
      fac->resource = 0;
      printf_to_char(ch, "You delete %s.\n\r", fac->name);
      return;
    }
    if (!str_cmp(arg, "unstasis")) {
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        printf_to_char(ch, "Syntax: %s unstasis (name)\n\r", ctype);
        return;
      }
      if (get_day() < 20) {
        send_to_char("You can only do this after the 20th of the month.\n\r", ch);
        return;
      }

      if (!in_fac(ch, fac->vnum) && str_cmp(ch->pcdata->account->name, fac->stasis_account)) {
        send_to_char("You're not a member of that society.\n\r", ch);
        return;
      }
      if (fac->stasis == 0) {
        send_to_char("That society is not in stasis\n\r", ch);
        return;
      }
      int days = (current_time - fac->stasis_time) / (3600 * 24);
      if (days >= 90) {
        fac->secret_days = 0;
        for (int j = 0; j < 100; j++) {
          free_string(fac->exmember_names[j]);
          fac->exmember_names[j] = str_dup("");
        }
        fac->lifeearned = UMAX(fac->lifeearned, fac->resource / 20);
      }
      else
      fac->secret_days = UMAX(0, fac->secret_days - days);
      fac->stasis = 0;
      fac->position_time = current_time;
      fac->restrict_time = current_time;
      if (!in_fac(ch, fac->vnum))
      join_to_clan(ch, fac->vnum);
      free_string(fac->leader);
      fac->leader = str_dup(ch->name);
      give_respect(ch, 25000, "Unstasis", fac->vnum);
      send_to_char("You bring your society back out of stasis.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "rename") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      fac = clan_lookup_name(argument);
      if (fac == NULL || fac == nullfac) {
        send_to_char("Syntax: faction rename (newname) (faction)\n\r", ch);
        return;
      }
      free_string(fac->name);
      fac->name = str_dup(arg2);
      return;
    }
    if (!str_cmp(arg, "list")) {

      send_to_char("`WThe Order\n\r`DThe Hand\n\r`gThe Temple`x\n\n\r", ch);

      send_to_char("`044  Sects`x\n\r", ch);

      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_SECT)
        continue;
        if ((*it)->alliance != ALLIANCE_SIDELEFT)
        continue;
        printf_to_char(ch, "`r%s`x\n\r", (*it)->name);
      }
      send_to_char("\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_SECT)
        continue;
        if ((*it)->alliance != ALLIANCE_SIDEMID)
        continue;
        printf_to_char(ch, "`m%s`x\n\r", (*it)->name);
      }
      send_to_char("\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_SECT)
        continue;
        if ((*it)->alliance != ALLIANCE_SIDERIGHT)
        continue;
        printf_to_char(ch, "`g%s`x\n\r", (*it)->name);
      }
      send_to_char("\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_SECT)
        continue;
        if ((*it)->alliance != 0)
        continue;
        printf_to_char(ch, "`c%s`x\n\r", (*it)->name);
      }

      send_to_char("`214  Cults`x\n\r", ch);

      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_CULT)
        continue;
        if ((*it)->alliance != ALLIANCE_SIDELEFT)
        continue;
        printf_to_char(ch, "`r%s`x\n\r", (*it)->name);
      }
      send_to_char("\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_CULT)
        continue;
        if ((*it)->alliance != ALLIANCE_SIDEMID)
        continue;
        printf_to_char(ch, "`m%s`x\n\r", (*it)->name);
      }
      send_to_char("\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_CULT)
        continue;
        if ((*it)->alliance != ALLIANCE_SIDERIGHT)
        continue;
        printf_to_char(ch, "`g%s`x\n\r", (*it)->name);
      }
      send_to_char("\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0 || (*it)->antagonist != 0 || safe_strlen((*it)->name) < 3 || (*it)->type != FACTION_CULT)
        continue;
        if ((*it)->alliance != 0)
        continue;
        printf_to_char(ch, "`c%s`x\n\r", (*it)->name);
      }


      send_to_char("\n`r  Antagonist Societies`x\n\r", ch);
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis > 0)
        continue;
        if ((*it)->antagonist != 1)
        continue;
        if (safe_strlen((*it)->name) > 3) {
          printf_to_char(ch, "`D%s, `x", (*it)->name);
        }
      }

      send_to_char("\n\r", ch);
      bool stasisfound = FALSE;
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->stasis == 0 || safe_strlen((*it)->name) < 3 || (*it)->antagonist == 1)
        continue;
        if (!in_fac(ch, (*it)->vnum) && (ch->pcdata->account == NULL || str_cmp(ch->pcdata->account_name, (*it)->stasis_account)))
        continue;
        if (stasisfound == FALSE)
        send_to_char("\n`D Stasised Societies`x\n\r", ch);
        stasisfound = TRUE;
        printf_to_char(ch, "`c%s`x\n\r", (*it)->name);
      }

      printf_to_char(ch, "Use %s info (name) for more information.\n\r", ctype);
      return;
    }

    if (!str_cmp(arg, "info")) {
      if ((fac == NULL || fac == nullfac) && safe_strlen(argument) < 2) {
        printf_to_char(ch, "You're not in a %s.\n\r", ctype);
        return;
      }
      if (safe_strlen(argument) >= 2) {
        FACTION_TYPE *disp = clan_lookup_name(argument);
        if (disp == NULL || disp == nullfac) {
          printf_to_char(ch, "No such %s, use %s list to see all options.\n\r", ctype, ctype);
          return;
        }
        if (disp->antagonist == 0) {
          if (disp->type == FACTION_CORE)
          send_to_char("`gType: `cFaction`x\n\r", ch);
          else if (disp->type == FACTION_CULT)
          printf_to_char(ch, "`gType: `DCult worshipping: %s`x\n\r", disp->eidilon);
          else if (disp->type == FACTION_SECT)
          printf_to_char(ch, "`gType: `WSect opposing: %s`x\n\r", disp->eidilon);


          printf_to_char(ch, "`gName`W:`x %s\n`gSymbol`W:`x %s\n`gLeader`W:`x %s\n\r", disp->name, disp->symbol, disp->leader);

          if (disp->college == 1)
          send_to_char("`gCollegiate`x\n\r", ch);

          if (disp->alliance != 0 && !generic_faction_vnum(disp->vnum)) {
            if (disp->type == FACTION_CULT)
            printf_to_char(ch, "Part of the %s Alliance\n\r", alliance_names(time_info.cult_alliance_issue, disp->alliance, time_info.cult_alliance_type));
            else
            printf_to_char(ch, "Part of the %s Alliance\n\r", alliance_names(time_info.sect_alliance_issue, disp->alliance, time_info.sect_alliance_type));
          }
          if (ch->faction != 0) {
            FACTION_TYPE *mfac = clan_lookup(ch->faction);
            if (position_difference(disp, mfac) >= 25)
            printf_to_char(ch, "Ideological Enemy of %s\n\r", mfac->name);
          }

          if (disp->can_alt == 1)
          printf_to_char(ch, "This %s can be joined by alts.\n\r", ctype);
          else
          printf_to_char(ch, "This %s cannot be joined by alts.\n\r", ctype);
          if (disp->closed == 0)
          printf_to_char(
          ch, "This %s is open to being joined with %s join.\n\r", ctype, ctype);
          else
          printf_to_char(
          ch, "This %s is closed, and you must be recruited to it.\n\r", ctype);
          printf_to_char(ch, "`DSecrecy`x: %d%%\t\t`gResources`x: $%d\n\r", faction_secrecy(disp, NULL) / 10, (((disp->resource + 1000) / 2500) * 25000));
        }
        else
        {
          printf_to_char(ch, "`gName`W:`x %s\n`gSymbol`W:`x %s\n\r", disp->name, disp->symbol);
        }

        /*
if(generic_faction_vnum(disp->vnum))
{
send_to_char("`cThis is a default faction, leadership can be taken
by anyone who obtains high\nenough standing, it doesn't have to pay
extra for more Tier 1 members, it\ncannot fall below 75k resources, or
exceed 125k. It cannot be disbanded.`x\n\r", ch);
}
if(protected_faction_vnum(disp->vnum))
{
send_to_char("`cThis is a protected faction, leadership can be
taken by anyone who obtains high\nenough standing, it cannot fall below
50k resources and cannot be\ndisbanded.`x\n\r", ch);
}
*/

        if (IS_IMMORTAL(ch)) {
          time_t inteltime;
          inteltime = disp->last_intel;
          printf_to_char(ch, "Last Intel: %s", (char *)ctime(&inteltime));
        }

        bool vassal = FALSE;
        for (int i = 0; i < 100; i++) {
          if (!str_cmp(ch->name, disp->vassal_names[i]))
          vassal = TRUE;
        }
        if (vassal == TRUE)
        printf_to_char(ch, "You are a vassal of this %s.\n\r", ctype);

        if (disp->antagonist == 0) {
          printf_to_char(ch, "`cPositions`x:");
          for (int i = 1; i <= AXES_MAX; i++) {
            if (disp->axes[i] == AXES_FARLEFT)
            printf_to_char(ch, " strongly %s", axes_names(i, ALLIANCE_SIDELEFT));
            if (disp->axes[i] == AXES_MIDLEFT)
            printf_to_char(ch, " moderately %s", axes_names(i, ALLIANCE_SIDELEFT));
            if (disp->axes[i] == AXES_NEARLEFT)
            printf_to_char(ch, " slightly %s", axes_names(i, ALLIANCE_SIDELEFT));
            if (disp->axes[i] == AXES_FARRIGHT)
            printf_to_char(ch, " strongly %s", axes_names(i, ALLIANCE_SIDERIGHT));
            if (disp->axes[i] == AXES_MIDRIGHT)
            printf_to_char(ch, " moderately %s", axes_names(i, ALLIANCE_SIDERIGHT));
            if (disp->axes[i] == AXES_NEARRIGHT)
            printf_to_char(ch, " slightly %s", axes_names(i, ALLIANCE_SIDERIGHT));
            if (i < AXES_MAX)
            send_to_char(",", ch);
          }
          send_to_char(".\n\r", ch);

          if(disp->vnum == FACTION_HAND)
          printf_to_char(ch, "`cRestrictions`x: No primary combatants");
          else if(disp->vnum == FACTION_ORDER)
          printf_to_char(ch, "`cRestrictions`x: No primary professionals");
          else if(disp->vnum == FACTION_TEMPLE)
          printf_to_char(ch, "`cRestrictions`x: No primary arcanists");
          else if (disp->soft_restrict == 1)
          printf_to_char(ch, "`cSoft Restrictions`x: No");
          else
          printf_to_char(ch, "`cRestrictions`x: No");

          for (int i = 0; i <= RESTRICT_MAX; i++) {
            if (disp->restrictions[i] == 1) {
              if (i == RESTRICT_MALE)
              send_to_char(" men", ch);
              else if (i == RESTRICT_FEMALE)
              send_to_char(" women", ch);
              else if (i == RESTRICT_PRIMARY_ARCANE)
              send_to_char(" primary arcanists", ch);
              else if (i == RESTRICT_ANY_ARCANE)
              send_to_char(" arcanists", ch);
              else if (i == RESTRICT_PRIMARY_COMBAT)
              send_to_char(" primary combatants", ch);
              else if (i == RESTRICT_ANY_COMBAT)
              send_to_char(" combatants", ch);
              else if (i == RESTRICT_PRIMARY_PROF)
              send_to_char(" primary professionals", ch);
              else if (i == RESTRICT_ANY_PROF)
              send_to_char(" professionals", ch);
              else if (i == RESTRICT_VAMPIRES)
              send_to_char(" vampires", ch);
              else if (i == RESTRICT_WEREWOLVES)
              send_to_char(" werewolves", ch);
              else if (i == RESTRICT_FAEBORN)
              send_to_char(" faeborn", ch);
              else if (i == RESTRICT_ANGELBORN)
              send_to_char(" angelborn", ch);
              else if (i == RESTRICT_DEMONBORN)
              send_to_char(" demonborn", ch);
              else if (i == RESTRICT_DEMIGOD)
              send_to_char(" demigods", ch);
              else if (i == RESTRICT_SUPERNATURALS)
              send_to_char(" supernaturals", ch);
              else if (i == RESTRICT_NATURALS)
              send_to_char(" naturals", ch);
              else if (i == RESTRICT_NONVIRGINS)
              send_to_char(" nonvirgins", ch);
              else if (i == RESTRICT_FRATENIZATION)
              send_to_char(" fraternization(sex with other society members)", ch);
              else if (i == RESTRICT_OUTOFWEDLOCK)
              send_to_char(" sex outside of marriage", ch);
              else if (i == RESTRICT_MURDERVAMPS)
              send_to_char(" murderers", ch);
              else if (i == RESTRICT_HOMOSEXUAL)
              send_to_char(" homosexuality", ch);
              else if (i == RESTRICT_INHIBITED)
              send_to_char(" inhibited", ch);

              if (i == RESTRICT_MAX - 1)
              send_to_char(", or", ch);
              else if (i < RESTRICT_MAX)
              send_to_char(",", ch);
            }
          }
          send_to_char(".\n\r", ch);
        }
        if (disp->antagonist == 1)
        printf_to_char(ch, "\n\n%s\n\r", disp->description);
        else {
          printf_to_char(ch, "`gThis %s has: ", ctype);
          if (safe_strlen(disp->description) > 2)
          printf_to_char(ch, " a summary");
          if (safe_strlen(disp->manifesto) > 2)
          printf_to_char(ch, ", manifesto");
          if (safe_strlen(disp->missions) > 2)
          printf_to_char(ch, ", missions");
          if (safe_strlen(disp->history) > 2)
          printf_to_char(ch, ", history");
          if (safe_strlen(disp->roles) > 2)
          printf_to_char(ch, ", roles");
          if (safe_strlen(disp->scenes) > 2)
          printf_to_char(ch, ", scenes");
          if (safe_strlen(disp->ooc) > 2)
          printf_to_char(ch, ", ooc");
          if (safe_strlen(disp->enemy_descs[0]) > 2 || disp->enemies[0][0] != 0)
          printf_to_char(ch, ", enemies");
          printf_to_char(ch, ". [%s read (section)/all (name)]`x\n\r", ctype);
        }

        if (IS_IMMORTAL(ch)) {
          printf_to_char(ch, "`gSupport`W:`x %d\n\r", disp->support);
          printf_to_char(ch, "`gResources`x: %d\n\r", disp->resource);
          printf_to_char(ch, "`gLife Earned`x: %d\n\r", disp->lifeearned);
          printf_to_char(ch, "`gPower`x: %d\n\r", fac_power(disp));
          int apower = 0;
          for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
          it != FacVect.end(); ++it) {
            if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->nopart == 1 || (*it)->antagonist == 1 || (*it)->cardinal == 1)
            continue;
            if ((*it)->alliance == disp->alliance)
            apower += fac_power((*it));
          }
          printf_to_char(ch, "Alliance Power`x: %d\n\r", apower);
        }
        return;
      }
      if (fac->resource > 50000)
      fac->resource = 10000;

      printf_to_char(ch, "`cName`W:`x %s\n\r", fac->name);
      printf_to_char(ch, "`cOperational Resources`W:`x $%d \t`cAwards`W:`x %d\n\r", fac->resource * 10, fac->awards);
      printf_to_char(
      ch, "`cManpower`W:`x %d(%d) \t`cOperation Fatigue`W:`x %d\n\r", fac->manpower, fac->manpower + fac->soldier_reinforce, fac->weekly_ops);

      printf_to_char(ch, "`cLeader`W:`x %s\n\r", fac->leader);


      bool unset = FALSE;
      for (int i = 1; i <= AXES_MAX; i++) {
        if (fac->axes[i] == 0)
        unset = TRUE;
      }
      if (unset == TRUE)
      send_to_char("`RThis society still has not set all its positions.`x\n\r", ch);

      if (!str_cmp(ch->name, fac->leader) && !generic_faction_vnum(fac->vnum) && !protected_faction_vnum(fac->vnum))
      printf_to_char(ch, "`cRank`W:`x Leader \t`cSecrecy`W:`x %d\n\r", faction_secrecy(fac, NULL) * char_secrecy(ch, NULL) /
      10000);
      else {
        for (i = 0; i < 100 && str_cmp(ch->name, fac->member_names[i]); i++) {
        }
        if(fac->type == FACTION_CORE)
        {
          if (fac->member_esteem[i] != ch->esteem_faction) {
            sprintf(buf, "FacRESPECT Mismatch: %s, %d, %d\n\r", ch->name, fac->member_esteem[i], ch->esteem_faction);
            log_string(buf);
          }
          fac->member_esteem[i] = ch->esteem_faction;
        }
        if(fac->type == FACTION_CULT)
        {
          if (fac->member_esteem[i] != ch->esteem_cult) {
            sprintf(buf, "FacRESPECT Mismatch: %s, %d, %d\n\r", ch->name, fac->member_esteem[i], ch->esteem_cult);
            log_string(buf);
          }
          fac->member_esteem[i] = ch->esteem_cult;
        }
        if(fac->type == FACTION_SECT)
        {
          if (fac->member_esteem[i] != ch->esteem_sect) {
            sprintf(buf, "FacRESPECT Mismatch: %s, %d, %d\n\r", ch->name, fac->member_esteem[i], ch->esteem_sect);
            log_string(buf);
          }
          fac->member_esteem[i] = ch->esteem_sect;
        }



        if(fac->type == FACTION_CORE)
        {
          if(ch->deploy_core == 0)
          send_to_char("You will not deploy for this group.\n\r", ch);
          else
          send_to_char("Deploying for this group.\n\r", ch);
        }
        if(fac->type == FACTION_CULT)
        {
          if(ch->deploy_cult == 0)
          send_to_char("You will not deploy for this group.\n\r", ch);
          else
          send_to_char("Deploying for this group.\n\r", ch);
        }
        if(fac->type == FACTION_SECT)
        {
          if(ch->deploy_sect == 0)
          send_to_char("You will not deploy for this group.\n\r", ch);
          else
          send_to_char("Deploying for this group.\n\r", ch);
        }

        if (fac->last_high_intel > current_time - (3600 * 24 * 2))
        send_to_char("`185Double resource benefits active.`x\n\r", ch);
        printf_to_char(ch, "`cRank`W:`x     %s \t`cSecrecy`W:`x %d\n\r", fac->ranks[fac->member_position[i]], faction_secrecy(fac, NULL) * char_secrecy(ch, NULL) /
        10000);
        printf_to_char(ch, "`cStanding`W:`x %d\n\r", fac->member_rank[i] + 1);
        printf_to_char(ch, "`cProgress`W:`x %d Percent\n\r", fac->member_esteem[i] * 100 /
        (100 + rank_requirement(fac->member_rank[i] + 1)));
      }
      printf_to_char(ch, "`c%s Influence:`x %d\n\r", ctype, ch->pcdata->faction_influence);
      int val = 0;
      int leadertier = 0;
      int membertier = 0;
      bool memberfive = FALSE;
      bool onehigh = FALSE;
      for (int i = 0; i < 100 && fac->member_tier[i] > 0; i++) {
        if (!str_cmp(fac->member_names[i], fac->leader))
        leadertier = fac->member_tier[i];
        else {
          if (fac->member_tier[i] > 2) {
            if (onehigh == FALSE && fac->member_tier[i] > 3) {
              membertier += (fac->member_tier[i] - 3);
              onehigh = TRUE;
              sprintf(buf, "FACTIERS Faction %s, membertier plus one high:%s, %d", fac->name, fac->member_names[i], fac->member_tier[i]);
              log_string(buf);
            }
            else {
              membertier += (fac->member_tier[i] - 2);
              sprintf(buf, "FACTIERS Faction %s, membertier:%s, %d", fac->name, fac->member_names[i], fac->member_tier[i]);
              log_string(buf);
            }
          }
          if (fac->member_tier[i] == 5)
          memberfive = TRUE;
        }
      }

      if (memberfive == TRUE && leadertier == 5)
      val -= 5;
      sprintf(buf, "FACTIERS Faction %s, leadertier :%d, membertier: %d, val: %d", fac->name, leadertier, membertier, val);
      log_string(buf);

      if (membertier > 3) {
        membertier -= 3;
        val -= 5 * membertier;
      }
      val *= -1;
      if (val > 0)
      printf_to_char(ch, "`cDebuff`W:`x %d percent.\n\r", val);

      printf_to_char(ch, "`cTrusts`W:`x");
      if (has_trust(ch, TRUST_BASE, fac->vnum))
      printf_to_char(ch, " Base Access");
      if (has_trust(ch, TRUST_COMMS, fac->vnum))
      printf_to_char(ch, ", Communications");
      if (has_trust(ch, TRUST_PROMOTIONS, fac->vnum))
      printf_to_char(ch, ", Promotions");
      if (has_trust(ch, TRUST_BANISHING, fac->vnum))
      printf_to_char(ch, ", Banishings");
      if (has_trust(ch, TRUST_NEWS, fac->vnum))
      printf_to_char(ch, ", News");
      if(has_trust(ch, TRUST_SPIRIT, fac->vnum))
      printf_to_char(ch, ", Eidolon");
      if (has_trust(ch, TRUST_RESEARCH, fac->vnum))
      printf_to_char(ch, ", Research");
      if (has_trust(ch, TRUST_RESOURCES, fac->vnum))
      printf_to_char(ch, ", Resources");
      if (has_trust(ch, TRUST_BRAINWASH, fac->vnum))
      printf_to_char(ch, ", Brainwashing");
      if (has_trust(ch, TRUST_SUSPENSIONS, fac->vnum))
      printf_to_char(ch, ", Suspensions");
      if (has_trust(ch, TRUST_RECRUITMENT, fac->vnum))
      printf_to_char(ch, ", Recruitment");
      if (has_trust(ch, TRUST_WAR, fac->vnum))
      printf_to_char(ch, ", Commanding");

      printf_to_char(ch, ".\n\r");
      printf_to_char(ch, "There are %d upcoming operations.\n\r", op_count(fac));
      OPERATION_TYPE *nextop = NULL;
      int minday = 100;
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->competition == COMPETE_CLOSED && (*it)->faction != fac->vnum)
        continue;

        if ((*it)->day < minday)
        minday = (*it)->day;
      }
      int minhour = 30;
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->day == minday && (*it)->hour < minhour && (minday != 0 || (*it)->hour > get_hour(NULL))) {
          if ((*it)->competition == COMPETE_CLOSED && (*it)->faction != fac->vnum)
          continue;
          minhour = (*it)->hour;
          nextop = (*it);
        }
      }
      if (nextop != NULL) {
        printf_to_char(
        ch, "`gNext operation`x: At %d hundred hours in %d days %s %s.\n\r", nextop->hour, nextop->day, terrain_names[nextop->terrain], operation_location(nextop));
      }



      printf_to_char(ch, "`cRoom Function`x: %s.\n\r", room_function(ch->in_room));
      return;
    }

    if (!str_cmp(arg, "read")) {
      FACTION_TYPE *disp = NULL;
      argument = one_argument_nouncap(argument, arg2);
      if (safe_strlen(argument) >= 2) {
        disp = clan_lookup_name(argument);
        if (disp == NULL || disp == nullfac) {
          printf_to_char(ch, "No such %s, use %s list to see all options.\n\r", ctype, ctype);
          return;
        }
      }
      else {
        if (fac == NULL || fac == nullfac) {
          printf_to_char(ch, "You're not in a %s.\n\r", ctype);
          return;
        }
        disp = fac;
      }
      char buf[MSL];
      if (!str_cmp(arg2, "all")) {
        static char string[MSL];
        string[0] = '\0';
        sprintf(buf, "`cThe basic description of %s.`x\n\n\r", disp->name);
        strcat(string, buf);
        sprintf(buf, "%s\n\r", disp->description);
        strcat(string, buf);
        if (safe_strlen(disp->manifesto) > 1) {
          sprintf(buf, "`cManifesto for %s, outlining the ways in which they wish to change the world and how territories they control are run.`x\n\n\r", disp->name);
          strcat(string, buf);
          sprintf(buf, "%s\n\r", disp->manifesto);
          strcat(string, buf);
        }
        if (safe_strlen(disp->missions) > 1) {
          sprintf(buf, "`cMissions for %s, outlining scene suggestions for members and other IC activities they wish their members to engage in outside of simple resource gathering.`x\n\n\r", disp->name);
          strcat(string, buf);
          sprintf(buf, "%s\n\r", disp->missions);
          strcat(string, buf);
        }
        if (safe_strlen(disp->roles) > 1) {
          sprintf(buf, "`cThe roles in %s.`x\n\n\r", disp->name);
          strcat(string, buf);
          sprintf(buf, "%s\n\r", disp->roles);
          strcat(string, buf);
        }
        if (safe_strlen(disp->history) > 1) {
          sprintf(buf, "`cThe history of %s.`x\n\n\r", disp->name);
          strcat(string, buf);
          sprintf(buf, "%s\n\r", disp->history);
          strcat(string, buf);
        }
        if (safe_strlen(disp->scenes) > 1) {
          sprintf(buf, "`cScene suggestions for people interacting with %s(Not typically considered ICly known information).`x\n\n\r", disp->name);
          strcat(string, buf);
          sprintf(buf, "%s\n\r", disp->scenes);
          strcat(string, buf);
        }
        if (safe_strlen(disp->ooc) > 1) {
          sprintf(
          buf, "`cAny other OOC information about %s people should know.`x\n\n\r", disp->name);
          strcat(string, buf);
          sprintf(buf, "%s\n\r", disp->ooc);
          strcat(string, buf);
        }
        if (safe_strlen(disp->enemy_descs[0]) > 2 || disp->enemies[0][0] != 0) {
          sprintf(buf, "`cEnemy %ss either current or potential.\n\r", ctype);
          strcat(string, buf);
          for (int i = 0; i < 10; i++) {
            if (safe_strlen(disp->enemy_descs[i]) > 1 || disp->enemies[0][0] != 0) {
              bool found = FALSE;
              for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
              it != FacVect.end(); ++it) {
                for (int j = 0; j < 10; j++) {
                  if ((*it)->enemies[j][0] == disp->vnum && (*it)->enemies[j][2] == i && ((*it)->alliance != disp->alliance || (*it)->type != (*it)->type)) {
                    if ((*it)->enemies[j][1] == 1 && ch->faction == disp->vnum) {
                      sprintf(buf, "[%2d] %s pending as:\n%s\n\r", i + 1, (*it)->name, disp->enemy_descs[i]);
                      strcat(string, buf);
                      found = TRUE;
                    }
                    else if ((*it)->enemies[j][1] == 2) {
                      sprintf(buf, "[%2d] %s opposing as:\n%s\n\r", i + 1, (*it)->name, disp->enemy_descs[i]);
                      strcat(string, buf);
                      found = TRUE;
                    }
                    else if ((*it)->enemies[j][1] == 2) {
                      sprintf(buf, "[%2d] %s resiting as:\n%s\n\r", i + 1, (*it)->name, disp->enemy_descs[i]);
                      strcat(string, buf);
                      found = TRUE;
                    }
                  }
                }
              }
              if (found == FALSE) {
                sprintf(buf, "[%2d] :\n%s\n\r", i + 1, disp->enemy_descs[i]);
                strcat(string, buf);
              }
            }
          }
          for (int i = 0; i < 10; i++) {
            if (disp->enemies[i][0] != 0) {
              FACTION_TYPE *opp = clan_lookup(disp->enemies[i][0]);
              if (opp != NULL && opp->alliance != disp->alliance) {
                if (disp->enemies[i][1] == 1 && ch->faction == disp->vnum) {
                  sprintf(buf, "Pending opposing %s as:\n%s\n\r", opp->name, opp->enemy_descs[disp->enemies[i][2]]);
                  strcat(string, buf);
                }
                else if (disp->enemies[i][2] == 2) {
                  sprintf(buf, "Opposing %s as:\n%s\n\r", opp->name, opp->enemy_descs[disp->enemies[i][2]]);
                  strcat(string, buf);
                }
                else if (disp->enemies[i][2] == 3) {
                  sprintf(buf, "Resisting %s as:\n%s\n\r", opp->name, opp->enemy_descs[disp->enemies[i][2]]);
                  strcat(string, buf);
                }
              }
            }
          }
        }
        page_to_char(string, ch);
        return;
      }
      else if (!str_cmp(arg2, "Enemies")) {
        static char string[MSL];
        string[0] = '\0';
        for (int i = 0; i < 10; i++) {
          if (safe_strlen(disp->enemy_descs[i]) > 1 || disp->enemies[0][0] != 0) {
            bool found = FALSE;
            for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
            it != FacVect.end(); ++it) {
              for (int j = 0; j < 10; j++) {
                if ((*it)->enemies[j][0] == disp->vnum && (*it)->enemies[j][2] == i && ((*it)->alliance != disp->alliance || (*it)->type != (*it)->type)) {
                  if ((*it)->enemies[j][1] == 1 && ch->faction == disp->vnum) {
                    sprintf(buf, "[%2d] %s pending as:\n%s\n\r", i + 1, (*it)->name, disp->enemy_descs[i]);
                    strcat(string, buf);
                    found = TRUE;
                  }
                  else if ((*it)->enemies[j][1] == 2) {
                    sprintf(buf, "[%2d] %s opposing as:\n%s\n\r", i + 1, (*it)->name, disp->enemy_descs[i]);
                    strcat(string, buf);
                    found = TRUE;
                  }
                  else if ((*it)->enemies[j][1] == 2) {
                    sprintf(buf, "[%2d] %s resisting as:\n%s\n\r", i + 1, (*it)->name, disp->enemy_descs[i]);
                    strcat(string, buf);
                    found = TRUE;
                  }
                }
              }
            }
            if (found == FALSE) {
              sprintf(buf, "[%2d] :\n%s\n\r", i + 1, disp->enemy_descs[i]);
              strcat(string, buf);
            }
          }
        }
        for (int i = 0; i < 10; i++) {
          if (disp->enemies[i][0] != 0) {
            FACTION_TYPE *opp = clan_lookup(disp->enemies[i][0]);
            if (opp != NULL && opp->alliance != disp->alliance) {
              if (disp->enemies[i][1] == 1 && ch->faction == disp->vnum) {
                sprintf(buf, "Pending opposing %s as:\n%s\n\r", opp->name, opp->enemy_descs[disp->enemies[i][2]]);
                strcat(string, buf);
              }
              else if (disp->enemies[i][2] == 2) {
                sprintf(buf, "Opposing %s as:\n%s\n\r", opp->name, opp->enemy_descs[disp->enemies[i][2]]);
                strcat(string, buf);
              }
              else if (disp->enemies[i][2] == 3) {
                sprintf(buf, "Resisting %s as:\n%s\n\r", opp->name, opp->enemy_descs[disp->enemies[i][2]]);
                strcat(string, buf);
              }
            }
          }
        }
        page_to_char(string, ch);
        return;
      }
      else if (!str_cmp(arg2, "Manifesto")) {
        sprintf(buf, "`cManifesto for %s, outlining the ways in which they wish to change the world.`x\n\n%s\n\r", disp->name, disp->manifesto);
        page_to_char(buf, ch);
        return;
      }
      else if (!str_cmp(arg2, "Missions")) {
        sprintf(buf, "`cMissions for %s, outlining scene suggestions for members and other IC activities they wish their members to engage in, outside of simple resource gathering.`x\n\n%s\n\r", disp->name, disp->missions);
        page_to_char(buf, ch);
        return;
      }
      else if (!str_cmp(arg2, "Summary")) {
        sprintf(buf, "`cThe basic description of %s.`x\n\n%s\n\r", disp->name, disp->description);
        page_to_char(buf, ch);
        return;
      }
      else if (!str_cmp(arg2, "Roles")) {
        sprintf(buf, "`cThe roles in %s.`x\n\n%s\n\r", disp->name, disp->roles);
        page_to_char(buf, ch);
        return;
      }
      else if (!str_cmp(arg2, "History")) {
        sprintf(buf, "`cThe history of %s.`x\n\n%s\r", disp->name, disp->history);
        page_to_char(buf, ch);
        return;
      }
      else if (!str_cmp(arg2, "Scenes")) {
        sprintf(buf, "`cScene suggestions for people interacting with %s(Not typically considered ICly known information).`x\n\n%s\n\r", disp->name, disp->scenes);
        page_to_char(buf, ch);
        return;
      }
      else if (!str_cmp(arg2, "ooc")) {
        sprintf(buf, "`cAny other OOC information about %s people should know.`x\n\n%s\n\r", disp->name, disp->ooc);
        page_to_char(buf, ch);
        return;
      }
      else {
        send_to_char("Society read (Summary/Manifesto/Missions/Roles/History/Scenes/OOC/All)\n\r", ch);
        return;
      }
    }
    if (fac == NULL || fac == nullfac) {
      printf_to_char(ch, "You're not in a %s.\n\r", ctype);
      return;
    }

    if(!str_cmp(arg, "deploy"))
    {
      if (fac == NULL || fac == nullfac) {
        printf_to_char(ch, "You're not in a %s.\n\r", ctype);
        return;
      }
      if(fac->type == FACTION_CORE)
      {
        if(ch->deploy_core == 1)
        {
          ch->deploy_core = 0;
          printf_to_char(ch, "You no longer deploy for your %s.\n\r", ctype);
          return;
        }
        else
        {
          ch->deploy_core = 1;
          printf_to_char(ch, "You now deploy for your %s.\n\r", ctype);
          return;
        }
      }
      if(fac->type == FACTION_CULT)
      {
        if(ch->deploy_cult == 1)
        {
          ch->deploy_cult = 0;
          printf_to_char(ch, "You no longer deploy for your %s.\n\r", ctype);
          return;
        }
        else
        {
          ch->deploy_cult = 1;
          printf_to_char(ch, "You now deploy for your %s.\n\r", ctype);
          return;
        }
      }
      if(fac->type == FACTION_SECT)
      {
        if(ch->deploy_sect == 1)
        {
          ch->deploy_sect = 0;
          printf_to_char(ch, "You no longer deploy for your %s.\n\r", ctype);
          return;
        }
        else
        {
          ch->deploy_sect = 1;
          printf_to_char(ch, "You now deploy for your %s.\n\r", ctype);
          return;
        }
      }

    }
    if (!str_cmp(arg, "uncollege")) {
      if (!is_leader(ch, fac->vnum) && !IS_IMMORTAL(ch)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }
      if(fac->college == 0)
      {
        send_to_char("That isn't a collegiate group.\n\r", ch);
        return;
      }
      if(college_student(ch, TRUE))
      {
        send_to_char("You can't do that while the leader is still a student.\n\r", ch);
        return;
      }
      fac->college = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "stasis")) {
      if (!is_leader(ch, fac->vnum) && !IS_IMMORTAL(ch)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }
      if (!IS_IMMORTAL(ch) && (generic_faction_vnum(fac->vnum) || protected_faction_vnum(fac->vnum))) {
        send_to_char("Default factions cannot be altered in this manner.\n\r", ch);
        return;
      }

      if (fac->stasis == 0) {
        fac->stasis = 1;
        fac->resource -= 100;
        printf_to_char(ch, "You put your %s into stasis.\n\r", ctype);
        fac->stasis_time = current_time;
        if (!IS_IMMORTAL(ch)) {
          free_string(fac->stasis_account);
          fac->stasis_account = str_dup(ch->pcdata->account->name);
        }
        return;
      }
    }
    if (!str_cmp(arg, "noparticipate")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }
      if (fac->nopart == 1) {
        fac->nopart = 0;
        if (fac->secret_days <= 14)
        fac->outcast = 0;
        send_to_char("You re-enter the conflict.\n\r", ch);
        return;
      }
      else {
        fac->nopart = 1;
        fac->outcast = 1;
        fac->alliance = 0;
        send_to_char("You leave the supernatural conflict.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg, "canalt")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }

      if (fac->can_alt == 0) {
        fac->can_alt = 1;
        printf_to_char(ch, "You open your %s up to alts.\n\r", ctype);
        return;
      }
      else {
        fac->can_alt = 0;
        printf_to_char(ch, "You close your %s off to alts.\n\r", ctype);
        return;
      }
    }
    if(!str_cmp(arg, "eidolonplayers")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }
      if(fac->type != FACTION_SECT && fac->type != FACTION_CULT)
      {
        send_to_char("Only sects and cults can have Eidolons.\n\r", ch);
        return;
      }
      string_append(ch, &fac->eidilon_players);
      send_to_char("Input player names separated by spaces for those you wish to be able to play your Eidolon.\n\r", ch);
      return;
    }
    if(!str_cmp(arg, "changeeidilon") || !str_cmp(arg, "changeeidolon")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }
      if(strlen(fac->eidilon) < 2) {
        send_to_char("You do not already have an eidolon.\n\r", ch);
        return;
      }
      if(fac->type != FACTION_SECT && fac->type != FACTION_CULT)
      {
        send_to_char("Only sects and cults can have Eidolons.\n\r", ch);
        return;
      }

      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg2);
      argument = one_argument_nouncap(argument, arg3);
      char ename[MSL];
      sprintf(ename, "%s", capitalize(arg3));
      int rpoint = 0;
      if(!str_cmp(arg2, "demon"))
      rpoint = RACE_SPIRIT_DEMON;
      else if(!str_cmp(arg2, "fae"))
      rpoint = RACE_SPIRIT_FAE;
      else if(!str_cmp(arg2, "divine"))
      rpoint = RACE_SPIRIT_DIVINE;
      else if(!str_cmp(arg2, "ghost"))
      rpoint = RACE_SPIRIT_GHOST;
      else if(!str_cmp(arg2, "cthulian"))
      rpoint = RACE_SPIRIT_CTHULIAN;
      else if(!str_cmp(arg2, "primal"))
      rpoint = RACE_SPIRIT_PRIMAL;
      else {
        printf_to_char(ch, "Syntax: %s makeeidolon (demon/fae/divine/ghost/cthulian/primal) (name).\n\r", ctype);
        return;
      }


      char strsave[MAX_INPUT_LENGTH];


      if(character_exists(ename))
      {
        int epoint = eidilon_clan_lookup(ename);
        if(epoint == 0)
        {
          send_to_char("That name is already taken.\n\r", ch);
          return;
        }
        FACTION_TYPE *prival = clan_lookup(epoint);
        if(prival == NULL)
        {
          send_to_char("That name is already taken.\n\r", ch);
          return;
        }
        if(fac->type == FACTION_SECT && prival->type == FACTION_CULT)
        {
          for (int i = 0; i < 10; i++) {
            if(fac->enemies[i][0] == prival->vnum && fac->enemies[i][1] > 1)
            {
              sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(fac->eidilon));
              unlink(strsave);
              use_resources(500, fac->vnum, NULL, "Eidolon change");

              free_string(fac->eidilon);
              fac->eidilon = str_dup(ename);
              fac->eidilon_type = rpoint;
              send_to_char("You set the eidolon.\n\r", ch);
              return;
            }
          }
        }
        send_to_char("That name is already taken, if you want to use the same Eidolon as another society you must be a sect, they must be a cult and you must be set as enemies.\n\r", ch);
        return;

      }
      if(strlen(ename) < 3 || strlen(ename) > 20)
      {
        send_to_char("Name must be between 3 and 20 characters.\n\r", ch);
        return;
      }
      char tempname[MSL];
      save_char_obj(ch, FALSE, FALSE);
      sprintf(tempname, "%s", ch->name);

      ch->faction = 0;
      free_string(ch->name);
      ch->name = str_dup(ename);
      char_from_room(ch);
      char_to_room(ch, get_room_index(GMHOME));
      OBJ_DATA *obj;
      int p = 0;
      for (ObjList::iterator it = object_list.begin();
      it != object_list.end() && p < 500000; p++) {
        obj = *it;
        ++it;

        if (obj == NULL)
        continue;

        if (obj->carried_by == ch) {
          obj_from_char(obj);
          extract_obj(obj);
        }
      }
      ACCOUNT_TYPE *account = ch->pcdata->account;
      if (IS_FLAG(ch->act, PLR_GM))
      REMOVE_FLAG(ch->act, PLR_GM);
      if (IS_FLAG(ch->act, PLR_NOSAVE))
      REMOVE_FLAG(ch->act, PLR_NOSAVE);
      if (IS_FLAG(ch->act, PLR_STASIS))
      REMOVE_FLAG(ch->act, PLR_STASIS);
      if (IS_FLAG(ch->act, PLR_DEAD))
      REMOVE_FLAG(ch->act, PLR_DEAD);


      do_newbieoutfit(ch, ch->name);
      ch->level = 1;
      ch->trust = 1;
      ch->wounds = 0;
      ch->faction = 0;
      ch->fcore = 0;
      ch->fsect = 0;
      ch->fcult = 0;
      ch->factiontwo = 0;
      ch->modifier = 0;
      ch->spentexp = 0;
      ch->spentrpexp = 0;
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }
      ch->spentnewexp = 0;
      ch->spentnewrpexp = 0;
      for (int x = 0; x < 10; x++)
      ch->pcdata->previous_focus_vnum[x] = 0;

      set_pcdata_default(ch->pcdata);
      VALIDATE(ch->pcdata);
      ch->id = get_pc_id();
      ch->played = 0;
      free_string(ch->short_descr);
      ch->short_descr = str_dup(ename);
      free_string(ch->long_descr);
      ch->long_descr = str_dup(ename);
      ch->pcdata->account = account;
      ch->exp = 0;
      ch->rpexp = 0;

      ch->spentexp = 0;
      ch->spentrpexp = 0;
      ch->pcdata->dtrains = 0;
      ch->pcdata->strains = 0;
      clear_flags(ch);

      ch->desc->connected = CON_PLAYING;
      do_function(ch, &do_save, "");
      ch->race = rpoint;
      ch->pcdata->eidilon_of = fac->vnum;

      sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(fac->eidilon));
      unlink(strsave);
      use_resources(500, fac->vnum, NULL, "Eidolon change");

      free_string(fac->eidilon);
      fac->eidilon = str_dup(ename);
      fac->eidilon_type = rpoint;

      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL) {
        DOMAIN_TYPE *domain;
        domain = new_domain();
        free_string(domain->domain_of);
        domain->domain_of = str_dup(ch->name);
        domain->archetype = ch->race;
        domain->power = 5;
        int mvnum = 0;
        for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
        it != DomainVect.end(); ++it) {
          if ((*it)->vnum > mvnum)
          mvnum = (*it)->vnum;
        }
        domain->vnum = mvnum + 1;
        DomainVect.push_back(domain);
      }


      send_to_char("You change the eidolon.\n\r", ch);

    }
    if(!str_cmp(arg, "makeeidilon") || !str_cmp(arg, "makeeidolon")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("Only the leader can do this.\n\r", ch);
        return;
      }
      if(strlen(fac->eidilon) > 2) {
        send_to_char("You already have an eidolon.\n\r", ch);
        return;
      }
      if(fac->type != FACTION_SECT && fac->type != FACTION_CULT)
      {
        send_to_char("Only sects and cults can have Eidolons.\n\r", ch);
        return;
      }

      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg2);
      argument = one_argument_nouncap(argument, arg3);
      char ename[MSL];
      sprintf(ename, "%s", capitalize(arg3));
      int rpoint = 0;
      if(!str_cmp(arg2, "demon"))
      rpoint = RACE_SPIRIT_DEMON;
      else if(!str_cmp(arg2, "fae"))
      rpoint = RACE_SPIRIT_FAE;
      else if(!str_cmp(arg2, "divine"))
      rpoint = RACE_SPIRIT_DIVINE;
      else if(!str_cmp(arg2, "ghost"))
      rpoint = RACE_SPIRIT_GHOST;
      else if(!str_cmp(arg2, "cthulian"))
      rpoint = RACE_SPIRIT_CTHULIAN;
      else if(!str_cmp(arg2, "primal"))
      rpoint = RACE_SPIRIT_PRIMAL;
      else {
        printf_to_char(ch, "Syntax: %s makeeidolon (demon/fae/divine/ghost/cthulian/primal) (name).\n\r", ctype);
        return;
      }
      if(character_exists(ename))
      {
        int epoint = eidilon_clan_lookup(ename);
        if(epoint == 0)
        {
          send_to_char("That name is already taken.\n\r", ch);
          return;
        }
        FACTION_TYPE *prival = clan_lookup(epoint);
        if(prival == NULL)
        {
          send_to_char("That name is already taken.\n\r", ch);
          return;
        }
        if(fac->type == FACTION_SECT && prival->type == FACTION_CULT)
        {
          for (int i = 0; i < 10; i++) {
            if(fac->enemies[i][0] == prival->vnum && fac->enemies[i][1] > 1)
            {
              free_string(fac->eidilon);
              fac->eidilon = str_dup(ename);
              fac->eidilon_type = rpoint;
              send_to_char("You set the eidolon.\n\r", ch);
              return;
            }
          }
        }
        send_to_char("That name is already taken, if you want to use the same Eidolon as another society you must be a sect, they must be a cult and you must be set as enemies.\n\r", ch);
        return;

      }
      if(strlen(ename) < 3 || strlen(ename) > 20)
      {
        send_to_char("Name must be between 3 and 20 characters.\n\r", ch);
        return;
      }
      char tempname[MSL];
      save_char_obj(ch, FALSE, FALSE);
      sprintf(tempname, "%s", ch->name);

      ch->faction = 0;
      free_string(ch->name);
      ch->name = str_dup(ename);
      char_from_room(ch);
      char_to_room(ch, get_room_index(GMHOME));
      OBJ_DATA *obj;
      int p = 0;
      for (ObjList::iterator it = object_list.begin();
      it != object_list.end() && p < 500000; p++) {
        obj = *it;
        ++it;

        if (obj == NULL)
        continue;

        if (obj->carried_by == ch) {
          obj_from_char(obj);
          extract_obj(obj);
        }
      }
      ACCOUNT_TYPE *account = ch->pcdata->account;
      if (IS_FLAG(ch->act, PLR_GM))
      REMOVE_FLAG(ch->act, PLR_GM);
      if (IS_FLAG(ch->act, PLR_NOSAVE))
      REMOVE_FLAG(ch->act, PLR_NOSAVE);
      if (IS_FLAG(ch->act, PLR_STASIS))
      REMOVE_FLAG(ch->act, PLR_STASIS);
      if (IS_FLAG(ch->act, PLR_DEAD))
      REMOVE_FLAG(ch->act, PLR_DEAD);


      do_newbieoutfit(ch, ch->name);
      ch->level = 1;
      ch->trust = 1;
      ch->wounds = 0;
      ch->faction = 0;
      ch->fcore = 0;
      ch->fsect = 0;
      ch->fcult = 0;
      ch->factiontwo = 0;
      ch->modifier = 0;
      ch->spentexp = 0;
      ch->spentrpexp = 0;
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }
      ch->spentnewexp = 0;
      ch->spentnewrpexp = 0;
      for (int x = 0; x < 10; x++)
      ch->pcdata->previous_focus_vnum[x] = 0;

      set_pcdata_default(ch->pcdata);
      VALIDATE(ch->pcdata);
      ch->id = get_pc_id();
      ch->played = 0;
      free_string(ch->short_descr);
      ch->short_descr = str_dup(ename);
      free_string(ch->long_descr);
      ch->long_descr = str_dup(ename);
      ch->pcdata->account = account;
      ch->exp = 0;
      ch->rpexp = 0;

      ch->spentexp = 0;
      ch->spentrpexp = 0;
      ch->pcdata->dtrains = 0;
      ch->pcdata->strains = 0;
      clear_flags(ch);

      ch->desc->connected = CON_PLAYING;
      do_function(ch, &do_save, "");
      ch->race = rpoint;
      ch->pcdata->eidilon_of = fac->vnum;


      free_string(fac->eidilon);
      fac->eidilon = str_dup(ename);
      fac->eidilon_type = rpoint;

      DOMAIN_TYPE *dom = my_domain(ch);
      if (dom == NULL) {
        DOMAIN_TYPE *domain;
        domain = new_domain();
        free_string(domain->domain_of);
        domain->domain_of = str_dup(ch->name);
        domain->archetype = ch->race;
        domain->power = 5;
        int mvnum = 0;
        for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
        it != DomainVect.end(); ++it) {
          if ((*it)->vnum > mvnum)
          mvnum = (*it)->vnum;
        }
        domain->vnum = mvnum + 1;
        DomainVect.push_back(domain);
      }


      send_to_char("You set the eidolon.\n\r", ch);

    }
    if (!str_cmp(arg, "antagonist")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (fac->antagonist == 0)
      fac->antagonist = 1;
      else
      fac->antagonist = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "adversary")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (!str_cmp(argument, "modern"))
      fac->adversary = ADVERSARY_MODERN;
      else if (!str_cmp(argument, "archaic"))
      fac->adversary = ADVERSARY_ARCHAIC;
      else
      send_to_char("No such adversary type.\n\r", ch);
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "becomespirit") || !str_cmp(arg, "becomeeidolon")) {
      bool playerperm = FALSE;
      if(safe_strlen(argument) > 2)
      {
        FACTION_TYPE *disp = clan_lookup_name(argument);
        if(disp != NULL && is_name(ch->name, disp->eidilon_players))
        {
          playerperm = TRUE;
          fac = disp;
        }
      }


      if (playerperm == FALSE && !has_trust(ch, TRUST_SPIRIT, ch->faction)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      if (ch->hit < 0) {
        send_to_char("You're hurt too badly for that.\n\r", ch);
        return;
      }
      if (in_fight(ch) || is_helpless(ch)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      if(get_char_world_pc(fac->eidilon) != NULL) {
        send_to_char("Your eidolon is already in the world.\n\r", ch);
        return;
      }
      act("$n passes out.", ch, NULL, NULL, TO_ROOM);
      ACCOUNT_TYPE *oaccount = ch->pcdata->account;
      save_char_obj(ch, FALSE, FALSE);
      DESCRIPTOR_DATA *d = ch->desc;
      quit_process(ch);
      extract_char(ch, TRUE);
      if(load_char_obj(d, fac->eidilon))
      {
        CHAR_DATA *nch = NULL;
        nch = d->character;
        d->connected = CON_PLAYING;
        nch->desc->pEdit = (void *)nch;
        nch->pcdata->account = oaccount;
        if(nch->pcdata->account == NULL)
        nch->pcdata->account = d->account;

        if(nch->in_room == NULL)
        char_to_room(nch, get_room_index(GMHOME));
        ch = nch;


        if (IS_FLAG(nch->act, PLR_BOUND))
        REMOVE_FLAG(nch->act, PLR_BOUND);
        if (IS_FLAG(nch->act, PLR_BOUNDFEET))
        REMOVE_FLAG(nch->act, PLR_BOUNDFEET);

        if (IS_FLAG(nch->act, PLR_GHOST))
        REMOVE_FLAG(nch->act, PLR_GHOST);
        if (IS_FLAG(nch->act, PLR_GHOSTWALKING))
        REMOVE_FLAG(nch->act, PLR_GHOSTWALKING);

        send_to_char("You pass out as the eidolon awakens.\n\r", nch);
        return;
      }
    }
    if (!str_cmp(arg, "reinforce")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      int mod = 200;
      if (fac->axes[AXES_COMBAT] == AXES_FARLEFT)
      mod = mod * 200 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_MIDLEFT)
      mod = mod * 160 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_NEARLEFT)
      mod = mod * 130 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_FARRIGHT)
      mod = mod * 50 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_MIDRIGHT)
      mod = mod * 70 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_NEARRIGHT)
      mod = mod * 90 / 100;

      int amount = atoi(argument);
      if (amount < 0 || amount > ((fac->resource - 9000) / 500)) {
        printf_to_char(ch, "Syntax: %s reinforce (number)\n\r", ctype);
        return;
      }
      if (amount + fac->soldier_reinforce > 25) {
        send_to_char("You can't have more than 25 reinforcements on standby.\n\r", ch);
        return;
      }
      if (fac->manpower + amount <= 12)
      fac->manpower += amount;
      else
      fac->soldier_reinforce += amount;
      use_resources(amount * mod, fac->vnum, NULL, "reinforcements");
      printf_to_char(ch, "You acquire an additional %d reinforcements.\n\r", amount);
      return;
    }
    if (!str_cmp(arg, "patrol")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      int amount = atoi(argument);
      if (amount < 0 || amount > (clan_lookup(ch->faction)->manpower +
            clan_lookup(ch->faction)->soldier_reinforce)) {
        printf_to_char(ch, "Syntax: %s patrol (number)\n\r", ctype);
        return;
      }
      if (amount + fac->patrolling > 10) {
        send_to_char("You can't have more than 10 soldiers assigned to patrol.\n\r", ch);
        return;
      }
      fac->soldier_reinforce -= amount;
      if (fac->soldier_reinforce < 0) {
        fac->manpower += fac->soldier_reinforce;
        fac->soldier_reinforce = 0;
      }
      fac->patrolling += amount;
      printf_to_char(ch, "You assign an additional %d soldiers to base patrols.\n\r", amount);
      return;
    }
    if (!str_cmp(arg, "guard")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      int amount = atoi(argument);
      if (amount < 0 || amount > (clan_lookup(ch->faction)->manpower +
            clan_lookup(ch->faction)->soldier_reinforce)) {

        printf_to_char(ch, "Syntax: %s guard (number)\n\r", ctype);
        return;
      }
      if (amount + fac->guarding > 20) {
        send_to_char("You can't have more than 20 soldiers assigned to guard duty.\n\r", ch);
        return;
      }
      fac->soldier_reinforce -= amount;
      if (fac->soldier_reinforce < 0) {
        fac->manpower += fac->soldier_reinforce;
        fac->soldier_reinforce = 0;
      }
      fac->guarding += amount;
      printf_to_char(
      ch, "You assign an additional %d soldiers to guard duty.\n\r", amount);
      return;
    }

    if (!str_cmp(arg, "soldiername")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      free_string(fac->soldier_name);
      fac->soldier_name = str_dup(argument);
      printf_to_char(ch, "Your %s soldiers are now named: %s\n\r", ctype, fac->soldier_name);
      return;
    }
    if (!str_cmp(arg, "soldierdescribe")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      string_append(ch, &fac->soldier_desc);
      return;
    }
    if (!str_cmp(arg, "banish")) {
      if (!has_trust(ch, TRUST_BANISHING, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        send_to_char("No such member.\n\r", ch);
        return;
      }

      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      if (!character_exists(fac->member_names[i])) {
        free_string(fac->member_names[i]);
        fac->member_names[i] = str_dup("");
        send_to_char("Empty member cleared.\n\r", ch);
        return;
      }

      d.original = NULL;
      if ((victim = get_char_world_pc(fac->member_names[i])) !=
          NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: clan banish");

        if (!load_char_obj(&d, fac->member_names[i])) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(fac->member_names[i]));
        victim = d.character;
      }
      if (victim == NULL) {
        if (!online)
        free_char(victim);
        return;
      }

      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      save_char_obj(victim, FALSE, FALSE);

      if (!online)
      free_char(victim);

      sprintf(buf, "%s has banished %s from the %s.", ch->name, fac->member_names[i], ctype);
      send_log(ch->faction, buf);
      add_to_exroster(fac->member_names[i], fac->vnum, FALSE);

      remove_from_clanroster(fac->member_names[i], fac->vnum);

      send_to_char("You banish them.\n\r", ch);
    }
    else if (!str_cmp(arg, "transfuse")) {
      if (!has_trust(ch, TRUST_RESOURCES, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (fac->resource < 8000) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      if (!can_spend_resources(fac)) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }

      int val = atoi(argument);
      if (fac->resource < 8000 + val / 10) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }

      if (!operating_room(ch->in_room) && !alchemy_room(ch->in_room)) {
        send_to_char("You need to be in a medbay, or alchemical lab to do that.\n\r", ch);
        return;
      }
      int take = val * (10 + get_skill(ch, SKILL_ALCHEMY)) / 10;
      take /= 5;
      take /= get_tier(ch);
      if (take < 10) {
        send_to_char("That wouldn't be enough for even a 1% gain.\n\r", ch);
        return;
      }
      use_resources(val / 10, fac->vnum, ch, "an empowered blood transfusion");
      give_lifeforce(ch, take, "blood transfusion");
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "symbol")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      free_string(fac->symbol);
      fac->symbol = str_dup(argument);
      printf_to_char(ch, "Your new %s symbol is: %s.\n\r", ctype, fac->symbol);
    }
    else if (!str_cmp(arg, "restrictions") || !str_cmp(arg, "restriction")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (generic_faction_vnum(fac->vnum) && !IS_IMMORTAL(ch))
      return;

      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "soft")) {
        if (fac->soft_restrict == 1) {
          fac->soft_restrict = 0;
          send_to_char("Your restrictions are now hard.\n\r", ch);
          return;
        }
        else {
          fac->soft_restrict = 1;
          send_to_char("Your restrictions are now soft.\n\r", ch);
          return;
        }
      }
      if (!str_cmp(arg2, "men")) {
        if (fac->restrictions[RESTRICT_MALE] == 1) {
          fac->restrictions[RESTRICT_MALE] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_MALE] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "women")) {
        if (fac->restrictions[RESTRICT_FEMALE] == 1) {
          fac->restrictions[RESTRICT_FEMALE] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_FEMALE] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "arcane-primary")) {
        if (fac->restrictions[RESTRICT_PRIMARY_ARCANE] == 1) {
          fac->restrictions[RESTRICT_PRIMARY_ARCANE] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_PRIMARY_ARCANE] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "arcanist")) {
        if (fac->restrictions[RESTRICT_ANY_ARCANE] == 1) {
          fac->restrictions[RESTRICT_ANY_ARCANE] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_ANY_ARCANE] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "combat-primary")) {
        if (fac->restrictions[RESTRICT_PRIMARY_COMBAT] == 1) {
          fac->restrictions[RESTRICT_PRIMARY_COMBAT] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_PRIMARY_COMBAT] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "combatant")) {
        if (fac->restrictions[RESTRICT_ANY_COMBAT] == 1) {
          fac->restrictions[RESTRICT_ANY_COMBAT] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_ANY_COMBAT] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "professional-primary")) {
        if (fac->restrictions[RESTRICT_PRIMARY_PROF] == 1) {
          fac->restrictions[RESTRICT_PRIMARY_PROF] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_PRIMARY_PROF] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "professionals")) {
        if (fac->restrictions[RESTRICT_ANY_PROF] == 1) {
          fac->restrictions[RESTRICT_ANY_PROF] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_ANY_PROF] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "vampires")) {
        if (fac->restrictions[RESTRICT_VAMPIRES] == 1) {
          fac->restrictions[RESTRICT_VAMPIRES] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_VAMPIRES] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "werewolves")) {
        if (fac->restrictions[RESTRICT_WEREWOLVES] == 1) {
          fac->restrictions[RESTRICT_WEREWOLVES] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_WEREWOLVES] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "faeborn")) {
        if (fac->restrictions[RESTRICT_FAEBORN] == 1) {
          fac->restrictions[RESTRICT_FAEBORN] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_FAEBORN] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "demonborn")) {
        if (fac->restrictions[RESTRICT_DEMONBORN] == 1) {
          fac->restrictions[RESTRICT_DEMONBORN] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_DEMONBORN] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "angelborn")) {
        if (fac->restrictions[RESTRICT_ANGELBORN] == 1) {
          fac->restrictions[RESTRICT_ANGELBORN] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_ANGELBORN] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "demigods")) {
        if (fac->restrictions[RESTRICT_DEMIGOD] == 1) {
          fac->restrictions[RESTRICT_DEMIGOD] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_DEMIGOD] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "supernaturals")) {
        if (fac->restrictions[RESTRICT_SUPERNATURALS] == 1) {
          fac->restrictions[RESTRICT_SUPERNATURALS] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_SUPERNATURALS] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "naturals")) {
        if (fac->restrictions[RESTRICT_NATURALS] == 1) {
          fac->restrictions[RESTRICT_NATURALS] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_NATURALS] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "nonvirgins")) {
        if (fac->restrictions[RESTRICT_NONVIRGINS] == 1) {
          fac->restrictions[RESTRICT_NONVIRGINS] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_NONVIRGINS] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "fraternization")) {
        if (fac->restrictions[RESTRICT_FRATENIZATION] == 1) {
          fac->restrictions[RESTRICT_FRATENIZATION] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_FRATENIZATION] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "unmarried-sex")) {
        if (fac->restrictions[RESTRICT_OUTOFWEDLOCK] == 1) {
          fac->restrictions[RESTRICT_OUTOFWEDLOCK] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_OUTOFWEDLOCK] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "murderers")) {
        if (fac->restrictions[RESTRICT_MURDERVAMPS] == 1) {
          fac->restrictions[RESTRICT_MURDERVAMPS] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_MURDERVAMPS] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "inhibited")) {
        if (fac->restrictions[RESTRICT_INHIBITED] == 1) {
          fac->restrictions[RESTRICT_INHIBITED] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_INHIBITED] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else if (!str_cmp(arg2, "homosexuality")) {
        if (fac->restrictions[RESTRICT_HOMOSEXUAL] == 1) {
          fac->restrictions[RESTRICT_HOMOSEXUAL] = 0;
          send_to_char("Restriction lifted.\n\r", ch);
        }
        else {
          fac->restrictions[RESTRICT_HOMOSEXUAL] = 1;
          send_to_char("Restriction added.\n\r", ch);
        }
      }
      else {
        printf_to_char(ch, "Syntax: %s restriction soft/men/women/arcanist/combatant/professional/arcane-primary/combat-primary/professional-primary/vampires/werewolves/faeborn/demonborn/angelborn/demigods/supernaturals/naturals/nonvirgins/fraternization/unmarried-sex/murderers/homosexuality/inhibited.\n\r", ctype);
        return;
      }
      fac->restrict_time = current_time;
    }
    else if (!str_cmp(arg, "position")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (generic_faction_vnum(fac->vnum) && !IS_IMMORTAL(ch))
      return;
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);

      int pos = 0;
      int lean = -1;
      if (!str_cmp(arg2, "Pro-supernatural")) {
        pos = AXES_SUPERNATURAL;
        lean = 1;
      }
      else if (!str_cmp(arg2, "Anti-Supernatural")) {
        pos = AXES_SUPERNATURAL;
        lean = 2;
      }
      else if (!str_cmp(arg2, "Spiritual")) {
        pos = AXES_MATERIAL;
        lean = 1;
      }
      else if (!str_cmp(arg2, "Material")) {
        pos = AXES_MATERIAL;
        lean = 2;
      }
      else if (!str_cmp(arg2, "Combative")) {
        pos = AXES_COMBAT;
        lean = 1;
      }
      else if (!str_cmp(arg2, "Manipulative")) {
        pos = AXES_COMBAT;
        lean = 2;
      }
      else if (!str_cmp(arg2, "Corrupt")) {
        pos = AXES_CORRUPT;
        lean = 1;
      }
      else if (!str_cmp(arg2, "Virtuous")) {
        pos = AXES_CORRUPT;
        lean = 2;
      }
      else if (!str_cmp(arg2, "Autocratic")) {
        pos = AXES_DEMOCRATIC;
        lean = 1;
      }
      else if (!str_cmp(arg2, "Democratic")) {
        pos = AXES_DEMOCRATIC;
        lean = 2;
      }
      else if (!str_cmp(arg2, "Anarchistic")) {
        pos = AXES_ANARCHY;
        lean = 1;
      }
      else if (!str_cmp(arg2, "Lawful")) {
        pos = AXES_ANARCHY;
        lean = 2;
      }
      else {
        send_to_char("Possible positions are: Pro-supernatural, anti-supernatural, spiritual, material, combative, manipulative, corrupt, virtuous, autocractic, democratic, anarchistic, lawful.\n\r", ch);
        return;
      }
      if (fac->axes[pos] != 0 && fac->secret_days >= 20) {
        send_to_char("Your positions are entrenched.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "strong")) {
        if (lean == 1)
        fac->axes[pos] = AXES_FARLEFT;
        else
        fac->axes[pos] = AXES_FARRIGHT;
      }
      else if (!str_cmp(argument, "moderate")) {
        if (lean == 1)
        fac->axes[pos] = AXES_MIDLEFT;
        else
        fac->axes[pos] = AXES_MIDRIGHT;
      }
      else if (!str_cmp(argument, "mild")) {
        if (lean == 1)
        fac->axes[pos] = AXES_NEARLEFT;
        else
        fac->axes[pos] = AXES_NEARRIGHT;
      }
      else if (!str_cmp(argument, "neutral")) {
        fac->axes[pos] = AXES_NEUTRAL;
      }
      else {
        printf_to_char(ch, "Syntax: %s postion (issue) (stance). Possible stances are strong, moderate, mild and neutral.\n\r", ctype);
        return;
      }
      fac->position_time = current_time;
      printf_to_char(ch, "You set your %s's position.\n\r", ctype);
      return;
    }
    else if (!str_cmp(arg, "comms")) {
      if (IS_FLAG(ch->comm, COMM_NOCOMMS)) {
        send_to_char("You turn your comms back on.\n\r", ch);
        REMOVE_FLAG(ch->comm, COMM_NOCOMMS);
        return;
      }
      else {
        send_to_char("You turn off your comms.\n\r", ch);
        SET_FLAG(ch->comm, COMM_NOCOMMS);
        return;
      }
    }
    else if (!str_cmp(arg, "makeenemy") || !str_cmp(arg, "makenemy")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      int point = atoi(arg2);
      if (point < 1 || point > 10) {
        printf_to_char(ch, "Syntax: %s makeenemy (1-10)\n\r", ctype);
        return;
      }
      string_append(ch, &fac->enemy_descs[point - 1]);
      return;
    }
    else if (!str_cmp(arg, "unoppose")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      FACTION_TYPE *opp = clan_lookup_name(argument);
      if (opp == NULL) {
        printf_to_char(ch, "Syntax: %s unoppose (society).\n\r", ctype);
        return;
      }
      for (int i = 0; i < 10; i++) {
        if (fac->enemies[i][0] == opp->vnum) {
          fac->enemies[i][0] = 0;
          fac->enemies[i][1] = 0;
          fac->enemies[i][2] = 0;
        }
      }
      for (int i = 0; i < 10; i++) {
        if (opp->enemies[i][0] == fac->vnum) {
          opp->enemies[i][0] = 0;
          opp->enemies[i][1] = 0;
          opp->enemies[i][2] = 0;
        }
      }
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg, "oppose")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      FACTION_TYPE *opp = clan_lookup_name(argument);
      if (opp == NULL) {
        printf_to_char(ch, "Syntax: %s oppose (1-10) (society).\n\r", ctype);
        return;
      }
      if (fac->axes[AXES_CORRUPT] > AXES_NEUTRAL && opp->axes[AXES_CORRUPT] > AXES_NEUTRAL) {
        send_to_char("Virtuous societies cannot oppose each other.\n\r", ch);
        return;
      }

      int point = atoi(arg2);
      if (point < 1 || point > 10) {
        printf_to_char(ch, "Syntax: %s oppose (1-10) (society).\n\r", ctype);
        return;
      }
      for (int i = 0; i < 10; i++) {
        if (fac->enemies[i][0] == 0) {
          fac->enemies[i][0] = opp->vnum;
          fac->enemies[i][1] = 1;
          fac->enemies[i][2] = point - 1;
          printf_to_char(ch, "Done, use %s describe enemy (%d) to write the description of this adversial relationship.\n\r", ctype, i+1);
          sprintf(buf, "%s is starting to become your enemy.\nUse Society approveenemy (%s) to form the relationship.\n\r", fac->name, fac->name);
          send_message(opp->vnum, buf);
          return;
        }
      }
    }
    else if (!str_cmp(arg, "approveenemy")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      FACTION_TYPE *opp = clan_lookup_name(argument);
      if (opp == NULL) {
        printf_to_char(ch, "Syntax: %s approveenemy (society).\n\r", ctype);
        return;
      }
      for (int i = 0; i < 10; i++) {
        if (opp->enemies[i][0] == fac->vnum && opp->enemies[i][1] == 1) {
          opp->enemies[i][1] = 2;
          sprintf(buf, "You become enemies with %s.\n\r", fac->name);
          send_message(opp->vnum, buf);
          sprintf(buf, "You become enemies with %s.\n\r", opp->name);
          send_message(fac->vnum, buf);
          printf_to_char(ch, "Done, use %s describe enemy (%d) to write the description of this adversial relationship.\n\r", ctype, i+1);
          return;
        }
      }
    }
    else if (!str_cmp(arg, "resist")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      FACTION_TYPE *opp = clan_lookup_name(argument);
      if (opp == NULL) {
        printf_to_char(ch, "Syntax: %s resist (1-10) (society).\n\r", ctype);
        return;
      }
      if (fac->axes[AXES_CORRUPT] <= AXES_NEUTRAL) {
        send_to_char("Only virtuous societies can be part of the resistance.\n\r", ch);
        return;
      }

      int point = atoi(arg2);
      if (point < 1 || point > 10) {
        printf_to_char(ch, "Syntax: %s resist (1-10) (society).\n\r", ctype);
        return;
      }
      for (int i = 0; i < 10; i++) {
        if (fac->enemies[i][0] == 0) {
          fac->enemies[i][0] = opp->vnum;
          fac->enemies[i][1] = 3;
          fac->enemies[i][2] = point - 1;
          sprintf(buf, "%s is resisting you in the form of %s.\n\r", fac->name, opp->enemy_descs[point - 1]);
          send_message(opp->vnum, buf);
          sprintf(buf, "You are now resisting %s in the form of %s.\n\r", opp->name, opp->enemy_descs[point - 1]);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg, "describe")) {
      printf_to_char(ch, "%s summary/manifesto/missions/roles/history/scenes/ooc.\n\r", ctype);
      return;
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      string_append(ch, &fac->description);
    }
    else if (!str_cmp(arg, "summary")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for describing the basic details of the society.`x\n\r", ch);
      string_append(ch, &fac->description);
    }
    else if (!str_cmp(arg, "enemy")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      int point = atoi(arg2)-1;
      if(point < 0 || point > 9)
      return;
      send_to_char("`WThis section is for describing an enemy of the society.`x\n\r", ch);
      string_append(ch, &fac->enemy_descs[point]);

    }
    else if (!str_cmp(arg, "manifesto")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for describing what changes your society wants to make in the world.`x\n\r", ch);
      string_append(ch, &fac->manifesto);
    }
    else if (!str_cmp(arg, "roles")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for what roles there are in your society, if any.`x\n\r", ch);
      string_append(ch, &fac->roles);
    }
    else if (!str_cmp(arg, "missions")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for describing IC actitivites you encourage or enforce members engaging in beyond simple resource gathering. It can be used by players a guide for the sort of RP in your society and a way to help them thing up society-appropriate activities and RP to do.`x\n\r", ch);
      string_append(ch, &fac->missions);
    }
    else if (!str_cmp(arg, "history")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for describing the history of the society.`x\n\r", ch);
      string_append(ch, &fac->history);
    }
    else if (!str_cmp(arg, "scenes")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for making scene suggestions for others wanting to interact with your society/society members, it is considered OOC information.`x\n\r", ch);
      string_append(ch, &fac->scenes);
    }
    else if (!str_cmp(arg, "ooc")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_to_char("`WThis section is for outlining any other information people should know about the society OOCly.`x\n\r", ch);
      string_append(ch, &fac->ooc);
    }
    else if (!str_cmp(arg, "closed")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (fac->closed == 0) {
        send_to_char("You close your society off from out of town recruitment.\n\r", ch);
        fac->closed = 1;
        return;
      }
      else {
        send_to_char("You open your society up to out of town recruitment.\n\r", ch);
        fac->closed = 0;
        fac->resource -= 100;
        return;
      }
    }
    else if (!str_cmp(arg, "ally")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "Hand")) {
        fac->parent = FACTION_HAND;
      }
      else if (!str_cmp(argument, "Order")) {
        fac->parent = FACTION_ORDER;
      }
      else if (!str_cmp(argument, "Temple")) {
        fac->parent = FACTION_TEMPLE;
      }
      else if (!str_cmp(argument, "None")) {
        fac->parent = 0;
      }
      else {
        send_to_char("Syntax: Faction ally Hand/Order/Temple/None\n\r", ch);
        return;
      }
      printf_to_char(ch, "Your group allies with the %s\n\r", argument);
    }
    else if (!str_cmp(arg, "ranktrust")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (int x = 0; x < 10; x++) {
        if (!str_cmp(arg2, fac->ranks[x])) {
          if (!str_cmp(argument, "base") && fac->rank_stats[x][TRUST_BASE] == 0) {
            printf_to_char(ch, "You add base access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_BASE] = 1;
          }
          else if (!str_cmp(argument, "base") && fac->rank_stats[x][TRUST_BASE] == 1) {
            printf_to_char(ch, "You remove base access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_BASE] = 0;
          }
          else if (!str_cmp(argument, "comms") && fac->rank_stats[x][TRUST_COMMS] == 0) {
            printf_to_char(ch, "You add comms access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_COMMS] = 1;
          }
          else if (!str_cmp(argument, "comms") && fac->rank_stats[x][TRUST_COMMS] == 1) {
            printf_to_char(ch, "You remove comms access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_COMMS] = 0;
          }
          else if (!str_cmp(argument, "news") && fac->rank_stats[x][TRUST_NEWS] == 0) {
            printf_to_char(ch, "You add news access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_NEWS] = 1;
          }
          else if (!str_cmp(argument, "news") && fac->rank_stats[x][TRUST_NEWS] == 1) {
            printf_to_char(ch, "You remove news access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_NEWS] = 0;
          }
          else if(!str_cmp(argument, "eidolon") && fac->rank_stats[x][TRUST_SPIRIT] == 0)
          {
            printf_to_char(ch, "You add eidolon calling access to %s.\n\r", fac->ranks[x]); fac->rank_stats[x][TRUST_SPIRIT] = 1;
          }
          else if(!str_cmp(argument, "eidolon") && fac->rank_stats[x][TRUST_SPIRIT] == 1)
          {
            printf_to_char(ch, "You remove eidolon calling access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_SPIRIT] = 0;
          }
          else if (!str_cmp(argument, "research") && fac->rank_stats[x][TRUST_RESEARCH] == 0) {
            printf_to_char(ch, "You add research access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_RESEARCH] = 1;
          }
          else if (!str_cmp(argument, "research") && fac->rank_stats[x][TRUST_RESEARCH] == 1) {
            printf_to_char(ch, "You remove research access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_RESEARCH] = 0;
          }
          else if (!str_cmp(argument, "resource") && fac->rank_stats[x][TRUST_RESOURCES] == 0) {
            printf_to_char(ch, "You add resource access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_RESOURCES] = 1;
          }
          else if (!str_cmp(argument, "resource") && fac->rank_stats[x][TRUST_RESOURCES] == 1) {
            printf_to_char(ch, "You remove resource access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_RESOURCES] = 0;
          }
          else if (!str_cmp(argument, "brainwashing") && fac->rank_stats[x][TRUST_BRAINWASH] == 0) {
            printf_to_char(ch, "You add brainwashing access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_BRAINWASH] = 1;
          }
          else if (!str_cmp(argument, "brainwashing") && fac->rank_stats[x][TRUST_BRAINWASH] == 1) {
            printf_to_char(ch, "You remove brainwashing access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_BRAINWASH] = 0;
          }
          else if (!str_cmp(argument, "suspensions") && fac->rank_stats[x][TRUST_SUSPENSIONS] == 0) {
            printf_to_char(ch, "You add suspensions access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_SUSPENSIONS] = 1;
          }
          else if (!str_cmp(argument, "suspensions") && fac->rank_stats[x][TRUST_SUSPENSIONS] == 1) {
            printf_to_char(ch, "You remove suspensions access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_SUSPENSIONS] = 0;
          }
          else if (!str_cmp(argument, "promotions") && fac->rank_stats[x][TRUST_PROMOTIONS] == 0) {
            printf_to_char(ch, "You add promotions access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_PROMOTIONS] = 1;
          }
          else if (!str_cmp(argument, "promotions") && fac->rank_stats[x][TRUST_PROMOTIONS] == 1) {
            printf_to_char(ch, "You remove promotions access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_PROMOTIONS] = 0;
          }
          else if (!str_cmp(argument, "recruitment") && fac->rank_stats[x][TRUST_RECRUITMENT] == 0) {
            printf_to_char(ch, "You add recruitment access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_RECRUITMENT] = 1;
          }
          else if (!str_cmp(argument, "recruitment") && fac->rank_stats[x][TRUST_RECRUITMENT] == 1) {
            printf_to_char(ch, "You remove recruitment access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_RECRUITMENT] = 0;
          }
          else if (!str_cmp(argument, "banishing") && fac->rank_stats[x][TRUST_BANISHING] == 0) {
            printf_to_char(ch, "You add banishing access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_BANISHING] = 1;
          }
          else if (!str_cmp(argument, "banishing") && fac->rank_stats[x][TRUST_BANISHING] == 1) {
            printf_to_char(ch, "You remove banishing access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_BANISHING] = 0;
          }
          else if (!str_cmp(argument, "commanding") && fac->rank_stats[x][TRUST_WAR] == 0) {
            printf_to_char(ch, "You add commanding access to %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_WAR] = 1;
          }
          else if (!str_cmp(argument, "commanding") && fac->rank_stats[x][TRUST_WAR] == 1) {
            printf_to_char(ch, "You remove commanding access from %s.\n\r", fac->ranks[x]);
            fac->rank_stats[x][TRUST_WAR] = 0;
          }
          else
          printf_to_char(ch, "Syntax: %s ranktrust (rankname) (base, comms, news, research, resource, brainwashing, suspensions, promotions, recruitment, banishing, commanding.\n\r", ctype);
          return;
        }
      }
      printf_to_char(ch, "Syntax: %sranktrust (rankname) (base, comms, news, research, resource, brainwashing, suspensions, promotions, recruitment, banishing, commanding.\n\r", ctype);
    }
    if (!str_cmp(arg, "kidnap")) {
      char karg[MSL];
      argument = one_argument_nouncap(argument, karg);

      if (!str_cmp(karg, "list") || !str_cmp(karg, "roster")) {
        send_to_char("Kidnap Roster\n\r", ch);
        for (int i = 0; i < 20; i++) {
          if (safe_strlen(fac->kidnap_name[i]) > 2) {
            printf_to_char(ch, "Someone close to %s, held in %s.\n\r", fac->kidnap_name[i], fac->kidnap_territory[i]);
          }
        }
        return;
      }
      if (!str_cmp(karg, "execute")) {
        if (!has_trust(ch, TRUST_WAR, fac->vnum)) {
          send_to_char("You aren't trusted to do that.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (!str_cmp(argument, fac->kidnap_name[i])) {
            free_string(fac->kidnap_name[i]);
            fac->kidnap_name[i] = str_dup("");
            grief_char(argument, 75);
            sprintf(buf, "Someone close to you in %s has been killed.", fac->kidnap_territory[i]);
            message_to_char(argument, buf);
            sprintf(buf, "Someone has ordered the captive close to %s, held in %s executed.", argument, fac->kidnap_territory[i]);
            send_log(ch->faction, buf);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      if (!str_cmp(karg, "release")) {
        if (!has_trust(ch, TRUST_WAR, fac->vnum)) {
          send_to_char("You aren't trusted to do that.\n\r", ch);
          return;
        }
        for (int i = 0; i < 20; i++) {
          if (!str_cmp(argument, fac->kidnap_name[i])) {
            free_string(fac->kidnap_name[i]);
            fac->kidnap_name[i] = str_dup("");
            sprintf(buf, "Someone close to you who was being held prisoner in %s has been released.", fac->kidnap_territory[i]);
            message_to_char(argument, buf);
            sprintf(buf, "Someone has ordered the captive close to %s, held in %s released.", argument, fac->kidnap_territory[i]);
            send_log(ch->faction, buf);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      else
      printf_to_char(ch, "Syntax: %s kidnap roster/execute/release\n\r", ctype);
    }
    if (!str_cmp(arg, "vassal")) {
      char varg[MSL];
      argument = one_argument_nouncap(argument, varg);

      if (!str_cmp(varg, "banish")) {
        if (!has_trust(ch, TRUST_BANISHING, fac->vnum)) {
          send_to_char("You're not trusted with that yet.\n\r", ch);
          return;
        }
        argument = one_argument_nouncap(argument, arg2);
        for (i = 0; i < 100 && str_cmp(arg2, fac->vassal_names[i]); i++) {
        }
        if (i == 100) {
          send_to_char("No such vassal.\n\r", ch);
          return;
        }
        if (!character_exists(fac->vassal_names[i])) {
          free_string(fac->vassal_names[i]);
          fac->vassal_names[i] = str_dup("");
          send_to_char("Empty vassal cleared.\n\r", ch);
          return;
        }
        d.original = NULL;
        if ((victim = get_char_world_pc(fac->vassal_names[i])) !=
            NULL) // Victim is online.
        online = TRUE;
        else {
          if (!load_char_obj(&d, fac->vassal_names[i])) {
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(fac->vassal_names[i]));
          victim = d.character;
        }
        if (victim == NULL) {
          if (!online)
          free_char(victim);
          return;
        }

        if (IS_NPC(victim)) {
          if (!online)
          free_char(victim);
          return;
        }
        if (victim->vassal == fac->vnum) {
          victim->vassal = 0;
        }
        save_char_obj(victim, FALSE, FALSE);

        if (!online)
        free_char(victim);

        sprintf(buf, "%s has removed %s from the society's vassal.", ch->name, fac->vassal_names[i]);
        send_log(ch->faction, buf);

        remove_from_vassalroster(fac->vassal_names[i], fac->vnum);

        send_to_char("You remove them.\n\r", ch);
      }
      else if (!str_cmp(varg, "roster")) {
        for (i = 0; i < 100; i++) {
          if (safe_strlen(fac->vassal_names[i]) > 1) {
            printf_to_char(ch, "%s\n\r", fac->vassal_names[i]);
          }
        }
      }
      else if (!str_cmp(varg, "recruit")) {
        if (!has_trust(ch, TRUST_RECRUITMENT, fac->vnum)) {
          send_to_char("You're not trusted with that yet.\n\r", ch);
          return;
        }
        CHAR_DATA *victim = get_char_room(ch, NULL, argument);
        if (victim == NULL || victim->vassal != 0) {
          send_to_char("They either are not here or are already a vassal.\n\r", ch);
          return;
        }
        vassal_to_clan(victim, fac->vnum);
        victim->vassal = fac->vnum;
        send_to_char("You join them to your society's vassals.\n\r", ch);
        if (get_tier(victim) == 1)
        fac->resource -= 10;
        else if (get_tier(victim) == 2)
        fac->resource -= 25;
        else
        fac->resource -= 50;
        return;
      }
    }
    else if (!str_cmp(arg, "suspend")) {
      if (!has_trust(ch, TRUST_SUSPENSIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        send_to_char("No such member.\n\r", ch);
        return;
      }

      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      if (fac->member_suspended[i] == 1) {
        send_to_char("You unsuspend them.\n\r", ch);
        fac->member_suspended[i] = 0;
      }
      else {
        send_to_char("You suspend them.\n\r", ch);
        fac->member_suspended[i] = 1;
      }
    }
    else if (!str_cmp(arg, "paybonus")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        send_to_char("No such member.\n\r", ch);
        return;
      }
      if (fac->resource < 8000) {
        send_to_char("You need at least 80k resources to do that.\n\r", ch);
        return;
      }
      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      int amount;
      if (!str_cmp(argument, ""))
      amount = 50;
      else
      amount = atoi(argument);
      if (amount < 0 || amount > 100) {
        send_to_char("Society paybonus (person) (1-100)\n\r", ch);
        return;
      }
      fac->member_pay[i] = amount;
      printf_to_char(ch, "You set them to recieve a pay bonus of %d.\n\r", amount);
    }
    else if (!str_cmp(arg, "promote")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        send_to_char("No such member.\n\r", ch);
        return;
      }
      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      for (int x = 0; x < 10; x++) {
        if (!str_cmp(argument, fac->ranks[x])) {
          fac->member_position[i] = x;
          printf_to_char(ch, "You make them %s.\n\r", fac->ranks[x]);
          return;
        }
      }
      printf_to_char(ch, "Syntax: %s promote (person) (rank)\n\r", ctype);
    }
    else if (!str_cmp(arg, "award")) {
      if (str_cmp(ch->name, fac->leader)) {
        send_to_char("Only the leader can give out awards.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        send_to_char("No such member.\n\r", ch);
        return;
      }
      if (fac->awards < 1) {
        send_to_char("You have no awards to give out.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 5) {
        send_to_char("You have to provide a reason first.\n\r", ch);
        return;
      }
      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      if (get_char_world_pc(arg2) == NULL || get_char_world_pc(arg2)->pcdata->account == NULL) {
        send_to_char("Wait until they're around first.\n\r", ch);
        return;
      }
      if (same_player(ch, get_char_world_pc(arg2))) {
        send_to_char("You can't award yourself.\n\r", ch);
        return;
      }
      if (!str_cmp(ch->desc->host, get_char_world_pc(arg2)->desc->host)) {
        send_to_char("You can't award someone with the same IP as you.\n\r", ch);
        return;
      }
      get_char_world_pc(arg2)->pcdata->account->award_karma += 2000;
      ch->pcdata->account->award_karma += 1000;
      fac->awards--;
      sprintf(buf, "%s awards %s for %s.", ch->name, arg2, argument);
      send_message(fac->vnum, buf);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "announce")) {
      if (!has_trust(ch, TRUST_NEWS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      send_message(fac->vnum, argument);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "report")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!has_trust(ch, TRUST_NEWS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (safe_strlen(arg2) < 1) {
        page_to_char(fac->report, ch);
        return;
      }
      if (!str_cmp(arg2, "battle") || !str_cmp(arg2, "operation") || !str_cmp(arg2, "battles") || !str_cmp(arg2, "Operations")) {
        if (activeoperation != NULL && isactiveoperation == TRUE) {
          send_to_char("You must wait for the current operation to conclude.\n\r", ch);
          return;
        }
        char arg3[MSL];
        argument = one_argument_nouncap(argument, arg3);
        if (!str_cmp(arg3, "1")) {
          if (safe_strlen(argument) > 0 && atoi(argument) > 0 && atoi(argument) < 12) {
            int point = atoi(argument) - 2;
            if (point >= 0 && point <= 10) {
              page_to_char(fac->report_overflow[0][point], ch);
            }
            else
            page_to_char(fac->reportone_text, ch);
          }
          else
          page_to_char(fac->reportone_text, ch);
        }
        else if (!str_cmp(arg3, "2")) {
          if (safe_strlen(argument) > 0 && atoi(argument) > 0 && atoi(argument) < 12) {
            int point = atoi(argument) - 2;
            if (point >= 0 && point <= 10) {
              page_to_char(fac->report_overflow[1][point], ch);
            }
            else
            page_to_char(fac->reporttwo_text, ch);
          }
          else
          page_to_char(fac->reporttwo_text, ch);

        }
        else if (!str_cmp(arg3, "3")) {
          if (safe_strlen(argument) > 0 && atoi(argument) > 0 && atoi(argument) < 12) {
            int point = atoi(argument) - 2;
            if (point >= 0 && point <= 10) {
              page_to_char(fac->report_overflow[2][point], ch);
            }
            else
            page_to_char(fac->reportthree_text, ch);
          }
          else
          page_to_char(fac->reportthree_text, ch);

        }
        else {
          printf_to_char(ch, "1) %s %d days ago.\n\r", fac->reportone_title, ((int)(current_time - fac->reportone_time)) /
          (3600 * 24));
          printf_to_char(ch, "2) %s %d days ago.\n\r", fac->reporttwo_title, ((int)(current_time - fac->reporttwo_time)) /
          (3600 * 24));
          printf_to_char(ch, "3) %s %d days ago.\n\r", fac->reportthree_title, ((int)(current_time - fac->reportthree_time)) /
          (3600 * 24));
        }
      }
      else if (!str_cmp(arg2, "events") || !str_cmp(arg2, "event")) {
        char arg3[MSL];
        argument = one_argument_nouncap(argument, arg3);
        int val = atoi(arg3);
        if (val >= 1 && val <= 20) {
          if (((int)(current_time - fac->event_time[val - 1])) / (3600) <
              min_hours(fac->event_type[val - 1])) {
            send_to_char("That report isn't ready yet.\n\r", ch);
            return;
          }
          int point = atoi(argument);
          if (point >= 2 && point <= 20) {
            page_to_char(fac->event_text[val - 1][point - 1], ch);
          }
          else
          page_to_char(fac->event_text[val - 1][0], ch);
        }
        else {
          for (int i = 0; i < 20; i++) {
            if (safe_strlen(fac->event_text[i][0]) > 2 && ((int)(current_time - fac->event_time[i])) / (3600) >=
                min_hours(fac->event_type[i])) {
              int hours = ((int)(current_time - fac->event_time[i])) / (3600);
              if (hours < 24)
              printf_to_char(ch, "%02d) %s %d `chours`x ago.\n\r", i + 1, fac->event_title[i], hours);
              else
              printf_to_char(ch, "%02d) %s %d `gdays`x ago.\n\r", i + 1, fac->event_title[i], hours / 24);
            }
          }
        }
      }
      else
      printf_to_char(ch, "Syntax: %s report events/battle\n\r", ch);
    }
    else if (!str_cmp(arg, "news")) {
      if (ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
        printf_to_char(ch, "`cMessages for %s`W:`x\n\n\r", clan_lookup(ch->faction)->name);
        for (i = 0; i < 20; i++) {
          if (safe_strlen(clan_lookup(ch->faction)->messages[i]) > 2 && clan_lookup(ch->faction)->message_timer[i])
          printf_to_char(ch, "%s\n\r", clan_lookup(ch->faction)->messages[i]);
        }
      }
    }
    else if (!str_cmp(arg, "ritual")) {
      if (!has_trust(ch, TRUST_RESOURCES, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (fac->resource < 8000) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      if (fac->ritual_timer > 0 && fac->ritual_orderer != NULL) {
        printf_to_char(fac->ritual_orderer, "Someone gives the ritualist a new instruction.\n\r");
      }
      fac->ritual_timer = 60;
      fac->ritual_orderer = ch;
      free_string(fac->ritual_string);
      fac->ritual_string = str_dup(argument);
      send_to_char("You send an instruction to the society's ritualist contact.\n\r", ch);
      act("$n types out a message on $s phone.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg, "export")) {
      return;
      if (!has_trust(ch, TRUST_RESOURCES, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (fac->resource < 8000) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      int val = atoi(argument);
      if (fac->resource < 8000 + val / 10) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      if (!can_spend_resources(fac)) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      fac->resource -= val / 10;
      OBJ_DATA *obj;

      obj = create_object(get_obj_index(36), 0);
      obj->level = val / 5;
      obj->level /= 2;
      obj->level = obj->level * 7 / 10;
      obj->size = 20;
      free_string(obj->name);
      obj->name = str_dup("collection sack resources valuable items");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("collection of valuable items");
      free_string(obj->description);
      obj->description = str_dup("A collection of valuable items");
      obj_to_char(obj, ch);
      send_to_char("Done.\n\r", ch);
      sprintf(buf, "Someone converts %d resources into exportable goods.", val);
      send_log(fac->vnum, buf);
      return;
    }
    else if (!str_cmp(arg, "oppress")) {
      if (!has_trust(ch, TRUST_WAR, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (event_cleanse == 1)
      return;

      if (fac->resource < 10250) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      if (!can_spend_resources(fac)) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      if (safe_strlen(argument) < 3) {
        send_to_char("Oppress who?\n\r", ch);
        return;
      }

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }
      if (newbie_level(victim) <= 3) {
        send_to_char("They are still too new to haven to be targeted by that ritual.\n\r", ch);
        return;
      }
      if (guestmonster(victim) || higher_power(victim)) {
        send_to_char("Your arcanists report this cannot be done because they are protected.\n\r", ch);
        return;
      }
      bool exmember = FALSE;
      for (int j = 0; j < 100; j++) {
        if (!str_cmp(fac->exmember_names[j], victim->name) && fac->exmember_loyalty[j] == 1)
        exmember = TRUE;
      }
      if (exmember == FALSE) {
        if (victim->faction != 0 && clan_lookup(victim->faction) != NULL && clan_lookup(victim->faction)->attributes[FACTION_UNDERSTANDING] > 0) {
          printf_to_char(ch, "Your arcanists report this cannot be done because they are protected by %s.\n\r", clan_lookup(victim->faction)->name);
          if (!online)
          free_char(victim);

          return;
        }

        if (institute_room(victim->in_room) || victim->race == RACE_FACULTY) {
          send_to_char("The institute protections prevent that from working.\n\r", ch);
          return;
        }

        if (victim->vassal != 0 && clan_lookup(victim->vassal) != NULL && clan_lookup(victim->vassal)->attributes[FACTION_UNDERSTANDING] > 0 && clan_lookup(victim->vassal)->stasis == 0) {
          printf_to_char(ch, "Your arcanists report this cannot be done because they are protected by %s.\n\r", clan_lookup(victim->vassal)->name);
          if (!online)
          free_char(victim);

          return;
        }
        if (has_weakness(ch, victim)) {
          printf_to_char(ch, "Your arcanists report this cannot be done because they are protected by a praestes relationship.\n\r");
          return;
        }
      }
      if (victim->oppress != 0 && clan_lookup(victim->oppress) != NULL) {
        send_to_char("Your arcanists report they are already oppressed.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }
      victim->oppress = fac->vnum;
      send_to_char("You feel a chill run up your spine.\n\r", victim);
      send_to_char("Your arcanists report success.\n\r", ch);
      sprintf(buf, "arranging an oppression curse against %s.", victim->name);
      use_resources(500, fac->vnum, NULL, buf);

      save_char_obj(victim, FALSE, FALSE);
      if (!online)
      free_char(victim);

    }
    else if (!str_cmp(arg, "unoppress")) {
      if (!has_trust(ch, TRUST_WAR, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (event_cleanse == 1)
      return;

      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }
      if (victim->oppress != fac->vnum) {
        send_to_char("They aren't being oppressed by your faction.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }
      victim->oppress = 0;
      send_to_char("You feel a chill run up your spine.\n\r", victim);
      send_to_char("Your arcanists report success.\n\r", ch);
      sprintf(buf, "lifting the oppression curse against %s.", victim->name);
      use_resources(500, fac->vnum, NULL, buf);
      save_char_obj(victim, FALSE, FALSE);
      if (!online)
      free_char(victim);

    }
    else if (!str_cmp(arg, "breaksanctuary")) {
      if (!has_trust(ch, TRUST_WAR, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (event_cleanse == 1)
      return;
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOKILL))
      return;

      if (fac->resource < 11000) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      if (!can_spend_resources(fac)) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }

      if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (newbie_level(victim) <= 3) {
        send_to_char("They are still too new to haven to be targetd by that ritual.\n\r", ch);
        return;
      }
      if (institute_room(victim->in_room) || victim->race == RACE_FACULTY) {
        send_to_char("The institute protections prevent that from working.\n\r", ch);
        return;
      }
      if (guestmonster(victim) || higher_power(victim)) {
        send_to_char("Your arcanists report this cannot be done because they are protected.\n\r", ch);
        return;
      }
      bool exmember = FALSE;
      for (int j = 0; j < 100; j++) {
        if (!str_cmp(fac->exmember_names[j], victim->name) && fac->exmember_loyalty[j] == 1)
        exmember = TRUE;
      }
      if (exmember == FALSE && !IS_FLAG(ch->act, PLR_TYRANT)) {
        if (victim->faction != 0 && clan_lookup(victim->faction) != NULL && clan_lookup(victim->faction)->attributes[FACTION_UNDERSTANDING] > 0) {
          printf_to_char(ch, "Your arcanists report this cannot be done because they are protected by %s.\n\r", clan_lookup(victim->faction)->name);
          return;
        }
        if (victim->vassal != 0 && clan_lookup(victim->vassal) != NULL && clan_lookup(victim->vassal)->attributes[FACTION_UNDERSTANDING] > 0 && clan_lookup(victim->vassal)->stasis == 0) {
          printf_to_char(ch, "Your arcanists report this cannot be done because they are protected by %s.\n\r", clan_lookup(victim->vassal)->name);
          return;
        }
        if (has_weakness(ch, victim)) {
          printf_to_char(ch, "Your arcanists report this cannot be done because they are protected by a praestes relationship.\n\r");
          return;
        }
      }
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * 60 * 20;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_COMMUTE;
      affect_to_char(victim, &af);
      use_resources(1000, fac->vnum, NULL, "breaking sanctuary");
      act("You feel a strange warm pain running through your body.", victim, NULL, NULL, TO_VICT);
      nounderglow(victim);
      return;
    }
    else if (!str_cmp(arg, "rescue")) {
      if (!has_trust(ch, TRUST_WAR, fac->vnum) || !has_trust(ch, TRUST_RESOURCES, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      if (!can_spend_resources(fac)) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }

      if (fac->resource < 12000) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
      if (rescue_person(ch, argument) == FALSE) {
        send_to_char("No such person to rescue.\n\r", ch);
        return;
      }
      use_resources(2000, fac->vnum, NULL, "arranging a mercenary rescue mission.\n\r");
      send_to_char("You spend $20, 000 resources on arranging a mercenary rescue mission.\n\r", ch);
    }
    else if (!str_cmp(arg, "logs") || !str_cmp(arg, "log")) {
      if (!has_trust(ch, TRUST_NEWS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      printf_to_char(ch, "`cLogs`W:`x\n\n\r");
      for (i = 0; i < 20; i++) {
        if (safe_strlen(fac->log[i]) > 2 && fac->log_timer[i])
        printf_to_char(ch, "%s\n\r", fac->log[i]);
      }
    }
    else if (!str_cmp(arg, "noleader")) {
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (fac->member_noleader[j] == 0) {
        fac->member_noleader[j] = 1;
        printf_to_char(
        ch, "You will no longer accept promotions to the leadership of %s.\n\r", fac->name);
        return;
      }
      else {
        fac->member_noleader[j] = 0;
        printf_to_char(
        ch, "You will now accept promotions to the leadership of %s.\n\r", fac->name);
        return;
      }
      return;

    }
    else if (!str_cmp(arg, "roster")) {
      for (i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1) {
          if (visibly_online(fac->member_names[i]))
          printf_to_char(ch, "`c[`x%10.10s`c][`x%02d`c]`x %s %s\n\r", fac->ranks[fac->member_position[i]], fac->member_rank[i] + 1, fac->member_names[i], fac->member_rosternotes[i]);
        }
      }
      for (i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1) {
          if (visibly_online(fac->member_names[i]) == FALSE) {
            if (fac->member_inactivity[i] >= 200 || !active_character(fac->member_names[i]))
            printf_to_char(ch, "`D[`x%10.10s`D][`x%02d`D]`x %s %s\n\r", fac->ranks[fac->member_position[i]], fac->member_rank[i] + 1, fac->member_names[i], fac->member_rosternotes[i]);
            else
            printf_to_char(ch, "`W[`x%10.10s`W][`x%02d`W]`x %s %s\n\r", fac->ranks[fac->member_position[i]], fac->member_rank[i] + 1, fac->member_names[i], fac->member_rosternotes[i]);
          }
        }
      }

    }
    else if (!str_cmp(arg, "remark")) {
      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        send_to_char("No such member.\n\r", ch);
        return;
      }
      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      free_string(fac->member_rosternotes[i]);
      fac->member_rosternotes[i] = str_dup(argument);
      send_to_char("You add a remark to their roster entry.\n\r", ch);
    }
    else if (!str_cmp(arg, "filenote")) {
      if (!has_trust(ch, TRUST_NEWS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      for (i = 0; i < 100 && str_cmp(arg2, fac->member_names[i]); i++) {
      }
      for (j = 0; j < 99 && str_cmp(ch->name, fac->member_names[j]); j++) {
      }
      if (i == 100) {
        if (get_char_world_pc(arg2) != NULL) {
          CHAR_DATA *filch = get_char_world_pc(arg2);
          if (is_whoinvis(filch) && !can_see_room(ch, filch->in_room) && ch->in_room != filch->in_room) {
            send_to_char("They are not a member of your society or they are not in sight..\n\r", ch);
            return;
          }
          for (int y = 0; y < 50; y++) {
            if (filch->pcdata->file_factions[y] == ch->faction) {
              filch->pcdata->file_factions[y] = ch->faction;
              string_append(ch, &filch->pcdata->file_message[y]);
              return;
            }
          }
          for (int y = 0; y < 50; y++) {
            if (filch->pcdata->file_factions[y] == 0) {
              filch->pcdata->file_factions[y] = ch->faction;
              string_append(ch, &filch->pcdata->file_message[y]);
              return;
            }
          }
        }

        send_to_char("They are not a member of your society or they are not in sight.\n\r", ch);
        return;
      }
      if (!has_authority(ch, arg2, fac->vnum)) {
        send_to_char("You don't have the authority to do that.\n\r", ch);
        return;
      }
      string_append(ch, &fac->member_filenotes[i]);
      return;
    }
    else if (!str_cmp(arg, "recruit")) {
      if (!has_trust(ch, TRUST_RECRUITMENT, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_room(ch, NULL, argument);

      if (victim == NULL || !can_join_faction(victim, fac->vnum, TRUE)) {
        send_to_char("There's nobody like that here who can be joined into a new society. They may be under 18, already have two societies or recently played another non-alt society character.\n\r", ch);
        return;
      }

      if (victim->faction == fac->vnum) {
        printf_to_char(ch, "They're already in this %s.\n\r", ctype);
        return;
      }

      int members = 0;
      for (int i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1)
        members++;
      }
      if (members >= 6) {
        if (get_tier(victim) == 1)
        fac->resource -= 50;
        else
        fac->resource -= 100;
      }
      victim->faction = fac->vnum;
      char lbuf[MSL];
      sprintf(lbuf, "%s recruits %s.", ch->name, victim->name);
      send_log(fac->vnum, lbuf);
      join_to_clan(victim, fac->vnum);
      printf_to_char(ch, "You join them to your %s!\n\r'", ctype);
      printf_to_char(victim, "You join %s.\n\r", fac->name);

    }
    else if (!str_cmp(arg, "leave")) {
      int number = clan_vnum_by_name(argument);
      if (number == 0 || clan_lookup(number) == NULL) {
        printf_to_char(ch, "Syntax: %s leave (faction name)\n\r", ctype);
        return;
      }
      if (ch->fcore == number) {
        sprintf(buf, "%s has left the society.", ch->name);
        send_log(ch->fcore, buf);
        add_to_exroster(ch->name, number, TRUE);
        remove_from_clanroster(ch->name, number);
        ch->faction = 0;
        send_to_char("You have left your society.\n\r", ch);
        return;
      }

      if (ch->fcult == number) {
        sprintf(buf, "%s has left the society.", ch->name);
        send_log(ch->fcult, buf);
        add_to_exroster(ch->name, number, TRUE);
        remove_from_clanroster(ch->name, number);
        ch->faction = 0;
        send_to_char("You have left your society.\n\r", ch);
        return;
      }
      if (ch->fsect == number) {
        sprintf(buf, "%s has left the society.", ch->name);
        send_log(ch->fsect, buf);
        add_to_exroster(ch->name, number, TRUE);
        remove_from_clanroster(ch->name, number);
        ch->faction = 0;
        send_to_char("You have left your society.\n\r", ch);
        return;
      }

    }
    else if (!str_cmp(arg, "rank")) {
      if (safe_strlen(argument) < 2) {
        for (int x = 0; x < 10; x++)
        printf_to_char(ch, "[%d] %s\n\r", x + 1, fac->ranks[x]);
        return;
      }

      if (!has_trust(ch, TRUST_PROMOTIONS, fac->vnum)) {
        send_to_char("You're not trusted with that yet.\n\r", ch);
        return;
      }

      argument = one_argument_nouncap(argument, arg2);

      int val = atoi(arg2);
      if (val < 1 || val > 10) {
        printf_to_char(ch, "Syntax: %s rank 1-10 (rank name)\n\r", ctype);
        return;
      }
      if (safe_strlen(argument) < 2) {
        printf_to_char(ch, "Syntax: %s rank 1-10 (rank name)\n\r", ctype);
        return;
      }
      for (i = 0; i < 100 && str_cmp(ch->name, fac->member_names[i]); i++) {
      }
      free_string(fac->ranks[val - 1]);
      fac->ranks[val - 1] = str_dup(argument);
      send_to_char("Rank renamed.\n\r", ch);
    }
    else if (!str_cmp(arg, "develop")) {
      if (!is_leader(ch, fac->vnum)) {
        send_to_char("You're not the leader of your society.\n\r", ch);
        return;
      }
      if (generic_faction_vnum(fac->vnum) && !IS_IMMORTAL(ch))
      return;
      if (!str_cmp("sanctuary", argument)) {
        if (fac->attributes[FACTION_UNDERSTANDING] == 0 && fac->resource >= 2500) {
          fac->attributes[FACTION_UNDERSTANDING] = 1;
          send_to_char("You buy sanctuary protection for your society.\n\r", ch);
          fac->resource -= 100;
          return;
        }
        else {
          fac->attributes[FACTION_UNDERSTANDING] = 0;
          send_to_char("You stop paying for sanctuary protection.\n\r", ch);
        }
      }
      if (!str_cmp("disposal", argument)) {
        if (fac->attributes[FACTION_CORPSE] == 0 && fac->resource >= 2500) {
          fac->attributes[FACTION_CORPSE] = 1;
          send_to_char("You obtain a corpse removal contract.\n\r", ch);
          fac->resource -= 100;
          return;
        }
        else {
          fac->attributes[FACTION_CORPSE] = 0;
          send_to_char("You stop paying for corpse removal.\n\r", ch);
        }
      }
      else if (!str_cmp("scouts", argument)) {
        if (fac->attributes[FACTION_SCOUTS] == 0 && fac->resource > 25) {
          fac->attributes[FACTION_SCOUTS] = 1;
          send_to_char("You buy scouts for your society.\n\r", ch);
          fac->resource -= 25;
          return;
        }
        else {
          fac->attributes[FACTION_SCOUTS] = 0;
          send_to_char("You stop paying for scouts.\n\r", ch);
        }
      }
      else if (!str_cmp("scanner", argument)) {
        if (fac->attributes[FACTION_911] == 0 && fac->resource > 25) {
          fac->attributes[FACTION_911] = 1;
          send_to_char("You pay to integrate a police scanner into your communications network.\n\r", ch);
          fac->resource -= 25;
          return;
        }
        else {
          fac->attributes[FACTION_911] = 0;
          send_to_char("You stop paying for an integrated police scanner.\n\r", ch);
        }
      }
      else if (!str_cmp("comms", argument) || !str_cmp("transmitter", argument)) {
        if (fac->attributes[FACTION_COMMS] == 0 && fac->resource > 30) {
          fac->attributes[FACTION_COMMS] = 1;
          send_to_char("You buy radio communications.\n\r", ch);
          fac->resource -= 30;
          return;
        }
        else {
          fac->attributes[FACTION_COMMS] = 0;
          send_to_char("You stop paying for radio communications.\n\r", ch);
        }
      }
    }
    else
    printf_to_char(ch, "Syntax: %s info/news/roster/join/recruit/switch/leave/banish/announce/comms/report/rank/ranktrust/promote/suspend/paybonus/log/noleader/parent/describe/symbol/closed/list/remark/filenote/participate/reinforce/soldiername/soldierdescribe/report\n\r", ctype);
  }

  _DOFUN(do_fsay) {
    if(ch->fcore == 0)
    {
      send_to_char("You aren't in a faction.\n\r", ch);
      return;
    }
    if(battleground(ch->in_room))
    return;

    ch->pcdata->ftype = FACTION_CORE;
    ch->faction = ch->fcore;
    do_function(ch, &do_clansay, argument);
  }
  _DOFUN(do_ssay) {
    if(ch->fsect == 0)
    {
      send_to_char("You aren't in a sect.\n\r", ch);
      return;
    }
    if(battleground(ch->in_room))
    return;
    ch->pcdata->ftype = FACTION_SECT;
    ch->faction = ch->fsect;
    do_function(ch, &do_clansay, argument);
  }
  _DOFUN(do_csay) {
    if(ch->fcult == 0)
    {
      send_to_char("You aren't in a cult.\n\r", ch);
      return;
    }
    if(battleground(ch->in_room))
    return;

    ch->pcdata->ftype = FACTION_CULT;
    ch->faction = ch->fcult;
    do_function(ch, &do_clansay, argument);
  }

  _DOFUN(do_clansay) {
    CHAR_DATA *victim;
    if (ch->hit < -7) {
      send_to_char("You're hurt too bad to talk.\n\r", ch);
      return;
    }

    if (is_mute(ch)) {
      send_to_char("You can't talk.\n\r", ch);
      return;
    }
    if (get_phone(ch) == NULL) {
      send_to_char("You need a phone to do that.\n\r", ch);
      return;
    }

    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Say what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You moan in your grave.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!radio_signal(ch))
    {
      send_to_char("You can't get a signal.\n\r", ch);
      return;

    }

    if (in_fight(ch) || ch->hit <= 0) {
      if (ch->fight_fast == FALSE) {
        send_to_char("You're a bit busy.\n\r", ch);
        return;
      }
    }

    if (!has_trust(ch, TRUST_COMMS, ch->faction)) {
      send_to_char("You don't have access to your society's comms.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->comm, COMM_NOCOMMS)) {
      send_to_char("Your comms are switched off.\n\r", ch);
      return;
    }
    if (ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->attributes[FACTION_COMMS] == 0) {
      send_to_char("You don't have access to comms.\n\r", ch);
      return;
    }

    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    FACTION_TYPE *sfac = clan_lookup(ch->faction);
    if (sfac->type == FACTION_CORE)
    send_to_char("You transmit on your faction channel.\n\r", ch);
    else if (sfac->type == FACTION_SECT)
    send_to_char("You transmit on your sect channel.\n\r", ch);
    else if (sfac->type == FACTION_CULT)
    send_to_char("You transmit on your cult channel.\n\r", ch);

    if (IS_FLAG(ch->act, PLR_HIDE))
    do_function(ch, &do_unhide, "");
    if(strlen(ch->pcdata->radio_action) > 2)
    {
      act(ch->pcdata->radio_action, ch, NULL, NULL, TO_ROOM);
      act(ch->pcdata->radio_action, ch, NULL, NULL, TO_CHAR);
    }

    do_function(ch, &do_say, argument);

    char rectalk[MSL], talktalk[MSL], tmp[MSL], blah[MSL];

    bool eataletter = FALSE;

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
    //      free_string(argument);
    argument = str_dup(talktalk);

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

      if (ch == victim)
      continue;

      if (!same_player(ch, victim) && victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN))
      continue;

      if (!same_player(ch, victim) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
      continue;

      if(!battleground(victim->in_room))
      {
        if(ch->pcdata->ftype == FACTION_CORE && victim->fcore != 0)
        victim->faction = victim->fcore;
        if(ch->pcdata->ftype == FACTION_SECT && victim->fsect != 0)
        victim->faction = victim->fsect;
        if(ch->pcdata->ftype == FACTION_CULT && victim->fcult != 0)
        victim->faction = victim->fcult;

      }

      if (IS_FLAG(victim->comm, COMM_NOCOMMS))
      continue;
      if (!has_trust(victim, TRUST_COMMS, victim->faction))
      continue;
      if (clan_lookup(ch->faction) == NULL)
      continue;
      if (clan_lookup(ch->faction)->alliance == 0 && !generic_faction_vnum(ch->faction))
      continue;

      if ((!generic_faction_vnum(ch->faction) && (clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->alliance !=
              clan_lookup(ch->faction)->alliance) && (clan_lookup(victim->factiontwo) == NULL || clan_lookup(victim->factiontwo)->alliance !=
              clan_lookup(ch->faction)->alliance)))
      continue;

      if(clan_lookup(ch->faction)->type != clan_lookup(victim->faction)->type)
      continue;

      if (generic_faction_vnum(ch->faction) && ch->faction != victim->faction && ch->faction != victim->factiontwo)
      continue;
      if (is_deaf(victim))
      continue;

      if (is_animal(victim) && get_animal_genus(victim, ANIMAL_ACTIVE) != GENUS_HYBRID)
      continue;

      if (!radio_signal(victim))
      continue;
      //    if(get_world(ch) != get_world(victim))
      //        continue;

      //    if(in_haven(ch->in_room) != in_haven(victim->in_room))
      //        continue;
      if (get_phone(victim) == NULL)
      continue;

      if (IS_AFFECTED(victim, AFF_MAD) && number_percent() % 2 == 0) {
        mad_comms(ch, victim);
      }
      else {
        char buf[MSL];
        if (!IS_NPC(ch) && safe_strlen(rectalk) > 2) {
          if (sfac->type == FACTION_CORE) {
            printf_to_char(
            victim, "Your faction comms say, %s, '%s'\n\r", rectalk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your faction comms say, %s, '%s'\n\r", rectalk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          else if (sfac->type == FACTION_SECT) {
            printf_to_char(
            victim, "Your sect comms say, %s, '%s'\n\r", rectalk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your sect comms say, %s, '%s'\n\r", rectalk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          else if (sfac->type == FACTION_CULT) {
            printf_to_char(
            victim, "Your cult comms say, %s, '%s'\n\r", rectalk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your cult comms say, %s, '%s'\n\r", rectalk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          prp_rplog(victim, buf);
        }
        else if (!IS_NPC(ch) && safe_strlen(ch->pcdata->talk) > 2) {
          if (sfac->type == FACTION_CORE) {
            printf_to_char(
            victim, "Your faction comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your faction comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          else if (sfac->type == FACTION_SECT) {
            printf_to_char(
            victim, "Your sect comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your sect comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          else if (sfac->type == FACTION_CULT) {
            printf_to_char(
            victim, "Your cult comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your cult comms say, %s, '%s'\n\r", ch->pcdata->talk, static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          prp_rplog(victim, buf);
        }
        else {
          if (sfac->type == FACTION_CORE) {
            printf_to_char(
            victim, "Your faction comms say '%s'\n\r", static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your faction comms say '%s'\n\r", static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          else if (sfac->type == FACTION_SECT) {
            printf_to_char(
            victim, "Your sect comms say '%s'\n\r", static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your sect comms say '%s'\n\r", static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          else if (sfac->type == FACTION_CULT) {
            printf_to_char(
            victim, "Your cult comms say '%s'\n\r", static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
            sprintf(buf, "Your cult comms say '%s'\n\r", static_text(
            ch, victim, mangle_text(ch, victim, ch->pcdata->speaking, argument)));
          }
          prp_rplog(victim, buf);
        }
      }
    }

    if (number_percent() % 6 == 0 && get_tracer(ch) != NULL && get_skill(get_tracer(ch), SKILL_HACKING) >= 5)
    printf_to_char(
    get_tracer(ch), "You intercept '%s'\n\r", mangle_text(ch, get_tracer(ch), ch->pcdata->speaking, argument));

    if (pc_pop(ch->in_room) < 2)
    rpreward(ch, argument, TRUE, 1);
  }

  int fac_lt(int vnum) {
    /*
if(vnum == FACTION_HAND)
return HAND_LT;
else if(vnum == FACTION_ORDER)
return ORDER_LT;
else if(vnum == FACTION_TEMPLE)
return TEMPLE_LT;
*/
    return 5;
  }

  int fac_soldier(int vnum) {
    /*
if(vnum == FACTION_HAND)
return HAND_SOLDIER;
else if(vnum == FACTION_ORDER)
return ORDER_SOLDIER;
else if(vnum == FACTION_TEMPLE)
return TEMPLE_SOLDIER;
*/
    return 5;
  }

  int minrank(int order) {
    if (order == ORDER_BLIND)
    return 4;
    if (order == ORDER_MUTE)
    return 4;
    if (order == ORDER_DEAFEN)
    return 4;
    if (order == ORDER_WEAKEN)
    return 4;
    if (order == ORDER_HAUNT)
    return 4;
    if (order == ORDER_PROTECT)
    return 4;
    if (order == ORDER_WARD)
    return 4;
    if (order == ORDER_INVESTIGATION)
    return 2;
    if (order == ORDER_FILE)
    return 2;
    if (order == ORDER_RESEARCH)
    return 3;
    if (order == ORDER_TAIL)
    return 4;
    if (order == ORDER_TRACE)
    return 5;
    if (order == ORDER_ATTACK)
    return 2;
    if (order == ORDER_GUARD)
    return 2;
    if (order == ORDER_READING)
    return 2;

    return 3;
  }

  _DOFUN(do_scout) {
    CHAR_DATA *victim;

    if ((victim = get_char_haven(ch, argument)) == NULL) {
      send_to_char("They cannot be located.\n\r", ch);
      return;
    }

    if (IS_NPC(victim) || str_cmp(victim->pcdata->tailing, ch->name) || victim->pcdata->tail_timer <= 0 || victim->pcdata->last_public_room == 0) {
      send_to_char("They cannot be located.\n\r", ch);
      return;
    }

    ROOM_INDEX_DATA *orig_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, get_room_index(victim->pcdata->last_public_room));
    do_function(ch, &do_look, "");
    char_from_room(ch);
    char_to_room(ch, orig_room);
  }

  char *get_origin(CHAR_DATA *ch) {
    if (get_skill(ch, SKILL_DEMIGOD) > 0)
    return "demigod";
    if (get_skill(ch, SKILL_TIMESWEPT) > 0)
    return "timeswept";
    if (get_skill(ch, SKILL_FAEBORN) > 0)
    return "faeborn";
    if (get_skill(ch, SKILL_DEMONBORN) > 0)
    return "demonborn";
    if (get_skill(ch, SKILL_ANGELBORN) > 0)
    return "angelborn";
    if (get_skill(ch, SKILL_VIRGIN) > 0)
    return "virgin";

    return "none";
  }

  bool public_room(ROOM_INDEX_DATA *room) {

    if (room == NULL)
    return FALSE;
    if (!IS_SET(room->room_flags, ROOM_INDOORS) && IS_SET(room->room_flags, ROOM_UNLIT) && (sunphase(room) <= 2 || sunphase(room) >= 6))
    return FALSE;

    if ((sunphase(room) <= 1 || sunphase(room) == 7) && shadowcloaked(room))
    return FALSE;

    if (!IS_SET(room->room_flags, ROOM_INDOORS) && mist_level(room) >= 3) {
      if (sunphase(room) != 4)
      return FALSE;
    }

    if (room->sector_type == SECT_AIR || room->sector_type == SECT_ATMOSPHERE || room->sector_type == SECT_UNDERWATER)
    return FALSE;

    if (room->sector_type == SECT_STREET && room->area->vnum == 13)
    return TRUE;

    if (is_sparring_room(room))
    return FALSE;

    if (room->sector_type == SECT_BEACH && get_roomy(room) > 490)
    return FALSE;

    if (room->sector_type == SECT_WATER)
    return FALSE;

    if (is_dark(room))
    return FALSE;

    if (IS_SET(room->room_flags, ROOM_PUBLIC))
    return TRUE;

    if (IS_SET(room->room_flags, ROOM_PRIVATE))
    return FALSE;

    return FALSE;
  }

  bool in_public(CHAR_DATA *persecutor, CHAR_DATA *ch) {
    if (!IS_NPC(ch) && ch->pcdata->bloodaura > 0)
    return FALSE;

    if (in_fight(ch))
    return FALSE;

    if (offworld(ch))
    return FALSE;

    if (silenced(ch))
    return FALSE;

    if (public_room(ch->in_room))
    return TRUE;

    if (is_sparring_room(ch->in_room))
    return FALSE;

    return FALSE;
  }

  bool in_base(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (prop_from_room(ch->in_room) == NULL)
    return FALSE;

    if (!is_base(prop_from_room(ch->in_room)))
    return FALSE;

    return TRUE;
  }

  bool with_bystanders(CHAR_DATA *ch) {
    if (ch == NULL)
    return FALSE;
    if (ch->in_room == NULL)
    return FALSE;
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != room)
      continue;

      if (IS_NPC(victim))
      continue;

      if (ch == victim)
      continue;

      if (victim->faction == 0 && !seems_super(victim))
      return TRUE;
    }
    return FALSE;
  }

  int get_rank(CHAR_DATA *ch, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    if (fac == NULL) {
      if (is_gm(ch))
      return 4;
      else
      return 0;
    }

    for (i = 0; i < 100 && str_cmp(ch->name, fac->member_names[i]); i++) {
    }

    return fac->member_rank[i] + 1;
  }

  void process_order(CHAR_DATA *ch, int type) {
    char buf[MSL];
    if (type == ORDER_COOLDOWN) {
      send_to_char("You can give orders again.\n\r", ch);
      return;
    }

    if ((type == ORDER_RESEARCH || type == ORDER_HRESEARCH) && is_number(ch->pcdata->order_target)) {
      int i = 1;
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if (current_time < (*it)->active_time)
        continue;

        if (i == atoi(ch->pcdata->order_target)) {
          sprintf(buf, "%s %s", (*it)->researched, ch->name);
          free_string((*it)->researched);
          (*it)->researched = str_dup(buf);

          send_to_char("Your orders are carried out.\n\r", ch);

          if (ch->pcdata->order_type != ORDER_HRESEARCH && ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
            ch->pcdata->order_type = ORDER_COOLDOWN;
            ch->pcdata->order_timer =
            BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
          }
          return;
        }
        i++;
      }
    }
    else if (type == ORDER_TRACE) {
      int number = atoi(ch->pcdata->order_target);
      OBJ_DATA *obj;
      ROOM_INDEX_DATA *room;

      for (ObjList::iterator it = object_list.begin(); it != object_list.end();
      ++it) {
        obj = *it;

        if (obj->item_type != ITEM_PHONE || obj->value[0] != number)
        continue;
        else if (IS_SET(obj->extra_flags, ITEM_OFF))
        continue;
        else if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (obj->in_room != NULL)
        room = obj->in_room;
        else if (obj->in_obj != NULL && obj->in_obj->in_room != NULL)
        room = obj->in_obj->in_room;
        else if (obj->carried_by != NULL && obj->carried_by->in_room != NULL)
        room = obj->carried_by->in_room;
        else {
          ch->pcdata->order_timer = 0;
          return;
        }

        send_to_char("Your orders are carried out.\n\r", ch);

        if (room == NULL)
        continue;

        if (ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
          ch->pcdata->order_type = ORDER_COOLDOWN;
          ch->pcdata->order_timer =
          BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
        }

        ROOM_INDEX_DATA *orig = ch->in_room;
        char_from_room(ch);
        char_to_room(ch, room);
        if (cell_signal(ch) && !is_town_blackout())
        maketownmap(ch);
        else
        send_to_char("No phone found.\n\r", ch);
        char_from_room(ch);
        char_to_room(ch, orig);
        return;
      }
      send_to_char("Your orders are carried out.\n\r", ch);

      if (ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
        ch->pcdata->order_type = ORDER_COOLDOWN;
        ch->pcdata->order_timer =
        BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
        ch->pcdata->order_timer /= 2;
      }
      send_to_char("No phone found.\n\r", ch);

    }
    else {
      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;

      d.original = NULL;
      if ((victim = get_char_world_pc(ch->pcdata->order_target)) !=
          NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: Order  conclude");

        if (!load_char_obj(&d, ch->pcdata->order_target)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->order_target));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      if (type == ORDER_SCOUT) {
        send_to_char("Your orders are carried out.\n\r", ch);
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
          continue;
          if (!free_to_act(to))
          continue;
          if (IS_FLAG(to->comm, COMM_AFK) && to != ch)
          continue;
          if (number_percent() % 2 == 0)
          continue;
          if (is_weakness(to, ch)) {
            if (to->in_room != NULL && to->in_room->vnum == to->pcdata->last_public_room)
            printf_to_char(to, "%s is currently %s.\n\r", PERS(to, ch), roomtitle(to->in_room, FALSE));
            else if (get_room_index(to->pcdata->last_public_room) != NULL)
            printf_to_char(
            ch, "%s was last seen %s.\n\r", PERS(to, ch), roomtitle(get_room_index(to->pcdata->last_public_room), FALSE));
          }
        }
        return;
      }
      if (type == ORDER_TAIL || type == ORDER_HTAIL) {

        free_string(victim->pcdata->tailing);
        victim->pcdata->tailing = str_dup(ch->name);

        victim->pcdata->tail_timer = 12 * 60 * 16;

        send_to_char("Your orders are carried out.\n\r", ch);

        if (is_masked(victim))
        victim->pcdata->tail_mask = 1;
        else if (victim->shape != SHAPE_HUMAN)
        victim->pcdata->tail_mask = 2;
        else
        victim->pcdata->tail_mask = 3;

        save_char_obj(victim, FALSE, FALSE);

        if (ch->pcdata->order_type != ORDER_HTAIL && ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
          ch->pcdata->order_type = ORDER_COOLDOWN;
          ch->pcdata->order_timer =
          BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
        }

        if (!online)
        free_char(victim);

        return;
      }

      if (type == ORDER_READING) {
        if (!online)
        free_char(victim);
        send_to_char("The psychics have gone inactive.\n\r", ch);
        return;

        if (is_masked(victim) || is_cloaked(victim) || victim->shape != SHAPE_HUMAN || (same_player(ch, victim) && ch != victim) || IS_FLAG(ch->act, PLR_DEAD)) {
          if (!online)
          free_char(victim);
          ch->pcdata->order_type = ORDER_COOLDOWN;
          ch->pcdata->order_timer = 2;

          return;
        }
        ACCOUNT_TYPE *vicaccount;
        if (victim->pcdata->account == NULL) {
          vicaccount = reload_account(victim->pcdata->account_name);
        }
        else
        vicaccount = victim->pcdata->account;

        if (vicaccount == NULL) {
          send_to_char("You can't get a reading.\n\r", ch);
          return;
        }
        if (!str_cmp(ch->pcdata->account->name, vicaccount->name)) {
          send_to_char("You can't do a reading on yourself.\n\r", ch);
          return;
        }
        char *associates[100];
        for (int i = 0; i < 100; i++) {
          associates[i] = str_dup("");
        }
        for (int i = 0; i < 25; i++) {
          if (safe_strlen(vicaccount->characters[i]) > 2 && str_cmp(vicaccount->characters[i], victim->name) && vicaccount->char_stasis[i] == 0 && !immaccount_name(vicaccount->name)) {
            for (int j = number_range(0, 50); j < 100; j++) {
              if (safe_strlen(associates[j]) < 2) {
                free_string(associates[j]);
                associates[j] = str_dup(vicaccount->characters[i]);
                j = 200;
              }
            }
          }
        }
        int endx;
        int count = 0;
        for (endx = number_range(5, 25);
        vicaccount->friend_type[endx] == 0 && count < 50;) {
          endx = number_range(5, 25);
          count++;
        }
        int startx = endx - 5;
        startx = UMAX(0, startx);
        for (int x = startx; x < endx; x++) {
          if (vicaccount->friend_type[x] == 1) {
            ACCOUNT_TYPE *friendaccount = reload_account(vicaccount->friends[x]);
            if (friendaccount != NULL) {
              for (int i = 0; i < 25; i++) {
                if (safe_strlen(friendaccount->characters[i]) > 2 && friendaccount->char_stasis[i] == 0) {
                  for (int j = number_range(0, 50); j < 100; j++) {
                    if (safe_strlen(associates[j]) < 2) {
                      free_string(associates[j]);
                      associates[j] = str_dup(friendaccount->characters[i]);
                      j = 200;
                    }
                  }
                }
              }
            }
          }
          if (vicaccount->friend_type[x] == 2 || vicaccount->friend_type[x] == 4) {
            for (int j = number_range(0, 50); j < 100; j++) {
              if (safe_strlen(associates[j]) < 2) {
                free_string(associates[j]);
                associates[j] = str_dup(vicaccount->friends[x]);
                j = 200;
              }
            }
          }
        }

        char tmp[MSL];
        char string[MSL];
        string[0] = 0;
        sprintf(tmp, "`cPsychic Reading`x");
        strcat(string, tmp);
        for (int i = 0; i < 100; i++) {
          if (safe_strlen(associates[i]) > 2) {
            sprintf(tmp, "%s\n", associates[i]);
            strcat(string, tmp);
          }
        }
        page_to_char(string, ch);

        if (!online)
        free_char(victim);

        return;
      }
      if (type == ORDER_INVESTIGATION || type == ORDER_HINVESTIGATION) {
        /*
if(is_masked(victim) || is_cloaked(victim) || victim->shape !=
SHAPE_HUMAN)
{
if(!online)
free_char(victim);
ch->pcdata->order_type = ORDER_COOLDOWN;
ch->pcdata->order_timer = 2;

return;
}
*/
        /*
char *  const   month_names      [] =
{
"January", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
};
*/
        printf_to_char(ch, "`WName :`x %s %s     `WAge:`x %d     `WGender:`x %s\n\r", victim->name, victim->pcdata->last_name, get_age(victim), (victim->sex == SEX_MALE) ? "Male" : "Female");

        if (victim->fcore != 0 && clan_lookup(victim->fcore) != NULL)
        printf_to_char(ch, "`WFaction:`x %s\n\r", clan_lookup(victim->fcore)->name);

        if (victim->pcdata->intel >= 1000)
        printf_to_char(ch, "`WIntel Value:`x `g$%d`x\n\r", victim->pcdata->intel);

        if (victim->pcdata->association != 0) {
          printf_to_char(ch, "`WAssociation:`x %s\n\r", get_subfac_name(victim->pcdata->association));
        }
        if (safe_strlen(victim->pcdata->home_territory) > 3) {
          printf_to_char(ch, "`WAssociated Territory:`x %s\n\r", victim->pcdata->home_territory);
        }

        if (get_cabal(victim) != NULL && safe_strlen(get_cabal(victim)->name) > 1) {
          printf_to_char(ch, "`WGroup:`x %s\n\r", get_cabal(victim)->name);
        }
        if (is_infused(victim) && type != ORDER_HINVESTIGATION) {
          printf_to_char(ch, "Their blood is infused.\n\r");
        }

        //	    printf_to_char(ch, "They were born day %d of %s, %d.\n\r", //victim->pcdata->birth_day, month_names[victim->pcdata->birth_month-1], //victim->pcdata->birth_year);

        if (get_phone(victim) != NULL && !IS_SET(get_phone(victim)->extra_flags, ITEM_OFF)) {
          printf_to_char(ch, "Phone number: %d\n\r", get_phone(victim)->value[0]);
        }
        printf_to_char(ch, "Last known address: %s.\n\r", get_house_address(victim));

        if (victim->pcdata->job_type_one == JOB_MIGRANT) {
          send_to_char("`cJob`x: New Arrival.\n\r", ch);
        }
        else if (victim->race == RACE_DEPUTY) {
          send_to_char("`cJob`x: Deputy.\n\r", ch);
        }
        else if (victim->pcdata->job_type_one == JOB_UNEMPLOYED && (victim->pcdata->job_type_two == JOB_UNEMPLOYED || victim->pcdata->job_type_two == JOB_MIGRANT)) {
          send_to_char("`cJob`x: Unemployed.\n\r", ch);
        }
        else {
          printf_to_char(ch, "`cJob`x: %s\n\r", job_name(victim, 1));
          if (victim->pcdata->job_type_two != JOB_UNEMPLOYED && victim->pcdata->job_type_two != JOB_MIGRANT)
          printf_to_char(ch, "`cJob`x: %s\n\r", job_name(victim, 2));
        }

        printf_to_char(ch, "Owned Properties:\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!str_cmp((*it)->owner, victim->name)) {
            if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) {
              continue;
            }
            if ((*it)->prop_state == 1)
            continue;
            printf_to_char(ch, "%s\n\r", (*it)->address);
          }
        }
        printf_to_char(ch, "Registered vehicles: ");
        for (int y = 0; y < 10; y++) {
          if (victim->pcdata->garage_cost[y] > 0)
          printf_to_char(ch, "%s   ", victim->pcdata->garage_lplate[y]);
        }
        send_to_char("\n\r", ch);
        if (ch->pcdata->order_type != ORDER_HINVESTIGATION && ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
          if (ch->faction != victim->faction) {
            ch->pcdata->order_type = ORDER_COOLDOWN;
            ch->pcdata->order_timer =
            BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
          }
          else {
            ch->pcdata->order_type = ORDER_COOLDOWN;
            ch->pcdata->order_timer = 5;
          }
        }

        if (!online)
        free_char(victim);

        return;
      }

      if (type == ORDER_MERCGUARD) {
        if (victim->pcdata->guard_number > ch->pcdata->order_amount && victim->pcdata->guard_expiration > current_time) {
          if (!online)
          free_char(victim);
          send_to_char("They already have more bodyguards than that.\n\r", ch);
          return;
        }

        ch->pcdata->total_money -= 1000 * 100 * ch->pcdata->order_amount;
        victim->pcdata->guard_number = ch->pcdata->order_amount;
        victim->pcdata->guard_faction = 26;
        victim->pcdata->guard_expiration = current_time + (3600 * 24 * 7);
        ch->pcdata->order_type = ORDER_COOLDOWN;
        ch->pcdata->order_timer = 0;
        ch->pcdata->order_amount = 0;

        send_to_char("Your orders are carried out.\n\r", ch);

        save_char_obj(victim, FALSE, FALSE);

        if (!online)
        free_char(victim);

        return;
      }
      if (type == ORDER_GUARD) {
        if (clan_lookup(ch->faction) == NULL || safe_strlen(clan_lookup(ch->faction)->soldier_name) < 5) {
          if (!online)
          free_char(victim);
          send_to_char("Your faction doesn't have any soldiers set up.\n\r", ch);
          return;
        }

        if (victim->pcdata->guard_number > ch->pcdata->order_amount && victim->pcdata->guard_expiration > current_time) {
          if (!online)
          free_char(victim);
          send_to_char("They already have more bodyguards than that.\n\r", ch);
          return;
        }

        clan_lookup(ch->faction)->soldier_reinforce -= ch->pcdata->order_amount;
        if (clan_lookup(ch->faction)->soldier_reinforce < 0) {
          clan_lookup(ch->faction)->manpower +=
          clan_lookup(ch->faction)->soldier_reinforce;
          clan_lookup(ch->faction)->soldier_reinforce = 0;
        }
        victim->pcdata->guard_number = ch->pcdata->order_amount;
        victim->pcdata->guard_faction = ch->faction;
        victim->pcdata->guard_expiration = current_time + (3600 * 24 * 7);
        ch->pcdata->order_type = ORDER_COOLDOWN;
        ch->pcdata->order_timer =
        BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
        ch->pcdata->order_amount = 0;

        send_to_char("Your orders are carried out.\n\r", ch);

        save_char_obj(victim, FALSE, FALSE);

        if (!online)
        free_char(victim);

        return;
      }
      if (type == ORDER_ATTACK) {

        if (!online || victim->pcdata->travel_time > 0 || victim->in_room == NULL || public_room(victim->in_room) || is_safe(victim, victim) || is_ghost(victim) || in_fight(victim) || with_bystanders(victim) || is_air(victim->in_room) || in_base(victim)) {
          ch->pcdata->order_timer++;

          if (!online)
          free_char(victim);

          return;
        }

        MOB_INDEX_DATA *pMobIndex;
        CHAR_DATA *mob = NULL;

        if (clan_lookup(ch->faction) == NULL || safe_strlen(clan_lookup(ch->faction)->soldier_name) < 5) {
          if (!online)
          free_char(victim);
          return;
        }

        clan_lookup(ch->faction)->soldier_reinforce -= ch->pcdata->order_amount;
        if (clan_lookup(ch->faction)->soldier_reinforce < 0) {
          clan_lookup(ch->faction)->manpower +=
          clan_lookup(ch->faction)->soldier_reinforce;
          clan_lookup(ch->faction)->soldier_reinforce = 0;
        }

        for (int x = 0; x < ch->pcdata->order_amount; x++) {

          pMobIndex = get_mob_index(HAND_SOLDIER);

          mob = create_mobile(pMobIndex);
          char_to_room(mob, victim->in_room);
          mob->hit = max_hp(mob);
          mob->faction = ch->faction;
          free_string(mob->short_descr);
          mob->short_descr = str_dup(clan_lookup(ch->faction)->soldier_name);
          free_string(mob->name);
          mob->name = str_dup(clan_lookup(ch->faction)->soldier_name);
          free_string(mob->long_descr);
          mob->long_descr = str_dup(clan_lookup(ch->faction)->soldier_name);
          free_string(mob->description);
          mob->description = str_dup(clan_lookup(ch->faction)->soldier_desc);

          mob->x = number_range(0, victim->in_room->size);
          mob->y = number_range(0, victim->in_room->size);
          free_string(mob->aggression);
          mob->aggression = str_dup(victim->name);
          mob->fighting = TRUE;

          mob->in_fight = TRUE;
          if (IS_FLAG(victim->act, PLR_SHROUD))
          SET_FLAG(mob->act, PLR_SHROUD);
        }
        if (mob != NULL && ch->pcdata->order_amount > 0)
        start_fight(mob, victim);

        ch->pcdata->order_type = ORDER_COOLDOWN;
        ch->pcdata->order_timer =
        BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
        ch->pcdata->order_amount = 0;

        send_to_char("Your orders are carried out.\n\r", ch);

        return;
      }
      if (type == ORDER_FILE) {
        /*
if(!same_faction(ch, victim) && (is_masked(victim) || is_cloaked(victim) || victim->shape != SHAPE_HUMAN))
{
if(!online)
free_char(victim);
ch->pcdata->order_type = ORDER_COOLDOWN;
ch->pcdata->order_timer = 2;
send_to_char("Thi
return;
}
*/

        char *const month_names[] = {
          "January", "Febuary", "March",     "April",   "May",      "June", "July",    "August",  "September", "October", "November", "December"};
        for (int x = 0; x < 50; x++) {
          if (victim->pcdata->file_factions[x] != 0) {
            if (clan_lookup(victim->pcdata->file_factions[x]) == NULL)
            victim->pcdata->file_factions[x] = 0;
            else if (victim->pcdata->file_factions[x] > 0 && (victim->pcdata->file_factions[x] == ch->faction || victim->pcdata->file_factions[x] == ch->factiontwo) && safe_strlen(victim->pcdata->file_message[x]) > 2) {
              sprintf(buf, "%s File: %s\n\r", clan_lookup(victim->pcdata->file_factions[x])->name, victim->pcdata->file_message[x]);
              page_to_char(buf, ch);
            }
          }
        }

        if (victim->pcdata->intel >= 1000)
        printf_to_char(ch, "`WIntel Value:`x `g$%d`x\n\r", victim->pcdata->intel);

        if ((ch->faction != 0 && (ch->faction == victim->faction || ch->faction == victim->factiontwo)) || (ch->factiontwo != 0 && (ch->factiontwo == victim->faction || ch->factiontwo == victim->factiontwo))) {
          printf_to_char(ch, "`WName :`x %s %s     `WGender:`x %s\n\r", victim->name, victim->pcdata->last_name, (victim->sex == SEX_MALE) ? "Male" : "Female");

          if (victim->fcore != 0 && clan_lookup(victim->fcore) != NULL)
          printf_to_char(ch, "`WFaction:`x %s\n\r", clan_lookup(victim->fcore)->name);

          printf_to_char(ch, "`WSecrecy:`x %d.\n\r", char_secrecy(victim, NULL) / 10);

          printf_to_char(ch, "`WTotal Resources Contributed: `g$%d  `WAlchemical Resources Contributed: `g$%d`x\n\r", victim->pcdata->life_tracker[TRACK_CONTRIBUTED], victim->pcdata->life_tracker[TRACK_ALCH_CONTRIBUTED]);
          printf_to_char(
          ch, "`WOperations Created:`x %d       `WOperations Attended:`x %d\n\r", victim->pcdata->life_tracker[TRACK_OPERATIONS_CREATED], victim->pcdata->life_tracker[TRACK_OPERATIONS_ATTENDED]);
          printf_to_char(ch, "`WMartial Patrols:`x %d   `WArcane Patrols:`x %d   `WDiplomatic Patrols:`x %d\n\r", victim->pcdata->life_tracker[TRACK_PATROL_WARFARE], victim->pcdata->life_tracker[TRACK_PATROL_ARCANE], victim->pcdata->life_tracker[TRACK_PATROL_DIPLOMATIC]);

          if (victim->pcdata->association != 0) {
            printf_to_char(ch, "`WAssociation:`x %s\n\r", get_subfac_name(victim->pcdata->association));
          }
          if (safe_strlen(victim->pcdata->home_territory) > 3) {
            printf_to_char(ch, "`WAssociated Territory:`x %s\n\r", victim->pcdata->home_territory);
          }

          if (get_cabal(victim) != NULL && safe_strlen(get_cabal(victim)->name) > 1) {
            printf_to_char(ch, "`WGroup:`x %s\n\r", get_cabal(victim)->name);
          }

          printf_to_char(ch, "They were born day %d of %s, %d.\n\r", victim->pcdata->birth_day, month_names[victim->pcdata->birth_month - 1], victim->pcdata->birth_year);

          if (get_phone(victim) != NULL && !IS_SET(get_phone(victim)->extra_flags, ITEM_OFF)) {
            printf_to_char(ch, "Phone number: %d\n\r", get_phone(victim)->value[0]);
          }
          printf_to_char(ch, "Last known address: %s.\n\r", get_house_address(victim));

          printf_to_char(ch, "Appearance: %s.\n\r", victim->pcdata->intro_desc);

          if (victim->pcdata->job_type_one == JOB_MIGRANT) {
            send_to_char("`cJob`x: New Arrival.\n\r", ch);
          }
          else if (victim->pcdata->job_type_one == JOB_UNEMPLOYED && (victim->pcdata->job_type_two == JOB_UNEMPLOYED || victim->pcdata->job_type_two == JOB_MIGRANT)) {
            send_to_char("`cJob`x: Unemployed.\n\r", ch);
          }
          else {
            printf_to_char(ch, "`cJob`x: %s\n\r", job_name(victim, 1));
            if (victim->pcdata->job_type_two != JOB_UNEMPLOYED && victim->pcdata->job_type_two != JOB_MIGRANT)
            printf_to_char(ch, "`cJob`x: %s\n\r", job_name(victim, 2));
          }

          printf_to_char(ch, "`W================================================================================`x\n\r");
          printf_to_char(ch, "                               `cFILE`x\n%s\n\r", victim->pcdata->file);

          if (is_leader(ch, victim->faction) || is_leader(ch, victim->factiontwo)) {
            printf_to_char(ch, "                    `RCLASSIFIED`x\n%s\n\r", victim->pcdata->classified);

            if (clan_lookup(victim->faction) != NULL && (ch->faction == victim->faction || ch->factiontwo == victim->faction)) {
              FACTION_TYPE *fac = clan_lookup(victim->faction);
              for (int i = 0; i < 100; i++) {
                if (!str_cmp(fac->member_names[i], victim->name))
                printf_to_char(ch, "                   `DNOTES`x\n%s\n\r", fac->member_filenotes[i]);
              }
            }
            if (clan_lookup(victim->factiontwo) != NULL && (ch->faction == victim->factiontwo || ch->factiontwo == victim->factiontwo)) {
              FACTION_TYPE *fac = clan_lookup(victim->factiontwo);
              for (int i = 0; i < 100; i++) {
                if (!str_cmp(fac->member_names[i], victim->name))
                printf_to_char(ch, "                   `DNOTES`x\n%s\n\r", fac->member_filenotes[i]);
              }
            }
          }
        }
        ch->pcdata->order_type = ORDER_COOLDOWN;
        ch->pcdata->order_timer = 3;

        if (!online)
        free_char(victim);

        return;
      }
      if (type == ORDER_RESEARCH || type == ORDER_HRESEARCH) {
        if (is_masked(victim) || is_cloaked(victim) || victim->shape != SHAPE_HUMAN) {
          if (!online)
          free_char(victim);
          ch->pcdata->order_type = ORDER_COOLDOWN;
          ch->pcdata->order_timer = 2;
          send_to_char("Their file can't be found right now.\n\r", ch);
          ch->pcdata->influence += 100;
          return;
        }

        send_to_char("Your orders are carried out.\n\r", ch);
        lookup_char(ch, victim, 10);

        if (!online)
        free_char(victim);

        if (ch->pcdata->order_type != ORDER_HRESEARCH && ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
          ch->pcdata->order_type = ORDER_COOLDOWN;
          ch->pcdata->order_timer =
          BASE_DURATION * minrank(type) / get_rank(ch, ch->faction);
        }
        return;
      }
    }
  }

  _DOFUN(do_hire) {
    char arg1[MSL];

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_helpless(ch) || is_animal(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return;

    argument = one_argument_nouncap(argument, arg1);

    if (is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "investigate")) {

      if (ch->money < 20000) {
        send_to_char("That'd cost $200.\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else if ((victim = get_char_world(ch, argument)) != NULL && !is_masked(victim) && !is_cloaked(victim))
      online = TRUE;
      else {
        log_string("DESCRIPTOR: Investigate");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_HINVESTIGATION;
      ch->pcdata->order_timer = 30;
      ch->money -= 20000;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
    else if (!str_cmp(arg1, "research")) {

      if (ch->money < 20000) {
        send_to_char("That'd cost $200.\n\r", ch);
        return;
      }

      if (is_number(argument)) {
        free_string(ch->pcdata->order_target);
        ch->pcdata->order_target = str_dup(argument);
        ch->pcdata->order_type = ORDER_HRESEARCH;
        ch->pcdata->order_timer = 3;
        send_to_char("Done.\n\r", ch);
        ch->money -= 20000;
      }
      else {

        CHAR_DATA *victim;
        DESCRIPTOR_DATA d;
        bool online = FALSE;
        Buffer outbuf;
        char buf[MSL];

        d.original = NULL;
        if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
        online = TRUE;
        else {
          log_string("DESCRIPTOR: Researching someone");

          if (!load_char_obj(&d, argument)) {
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
          victim = d.character;
        }
        if (IS_NPC(victim)) {
          if (!online)
          free_char(victim);
          return;
        }

        free_string(ch->pcdata->order_target);
        ch->pcdata->order_target = str_dup(victim->name);
        ch->pcdata->order_type = ORDER_HRESEARCH;
        ch->pcdata->order_timer = 30;
        ch->money -= 20000;
        send_to_char("Done.\n\r", ch);

        if (!online)
        free_char(victim);
      }
    }
    else if (!str_cmp(arg1, "tail")) {
      if (ch->money < 40000) {
        send_to_char("That'd cost $400.\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      bool online = FALSE;
      Buffer outbuf;

      if ((victim = get_char_haven(ch, argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        return;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_HTAIL;
      ch->pcdata->order_timer = 12;

      ch->money -= 40000;
      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
  }

  _DOFUN(do_order) {
    char arg1[MSL];

    if (in_fight(ch)) {
      do_function(ch, &do_minioncommand, argument);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_helpless(ch) || is_animal(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return;

    if (room_hostile(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if (ch->pcdata->order_type == ORDER_COOLDOWN && ch->pcdata->order_timer > 0) {
      send_to_char("You gave an order too recently.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "mercenaryguards")) {
      char argcount[MSL];
      argument = one_argument_nouncap(argument, argcount);
      int amount = atoi(argcount);
      if (amount < 0 || amount > 5) {
        send_to_char("Syntax, order bodyguard (1-5) (target)\n\r", ch);
        return;
      }
      if (amount * 100 * 1000 > ch->pcdata->total_money) {
        send_to_char("Each mercenary costs $1000.\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: bodyguarding someone");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }
      if (victim->pcdata->guard_number > amount && victim->pcdata->guard_expiration > current_time) {
        send_to_char("They already have more guards than that.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_MERCGUARD;
      ch->pcdata->order_amount = amount;
      ch->pcdata->order_timer = 120;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
      return;
    }
    if (ch->pcdata->influence < 500) {
      send_to_char("You need 500 influence to do that.\n\r", ch);
      return;
    }
    if (ch->faction == 0 && !is_gm(ch) && ch->race != RACE_DEPUTY && ch->race != RACE_FACULTY) {
      send_to_char("You're not in a faction.\n\r", ch);
      return;
    }
    if (!can_spend_resources(clan_lookup(ch->faction))) {
      send_to_char("Your faction cannot do that right now.\n\r", ch);
      return;
    }
    /*
if(ch->race == RACE_FACULTY || ch->race == RACE_DEPUTY) {
send_to_char("The Academy and HSD can't do this yet.\n\r", ch); return;
}
*/
    // Possible Faculty Orders - Discordance
    if (ch->race == RACE_FACULTY) {
      if (str_cmp(arg1, "file")) {
        send_to_char("Faculty can only order files on students.\n\r", ch);
        return;
      }
    }

    if (!str_cmp(arg1, "investigate")) {
      if (get_rank(ch, ch->faction) < minrank(ORDER_INVESTIGATION) && ch->race != RACE_DEPUTY) {
        send_to_char("You don't have enough standing to do that.\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else if ((victim = get_char_haven(ch, argument)) != NULL && !is_masked(victim) && !is_cloaked(victim) && victim->shape == SHAPE_HUMAN)
      online = TRUE;
      else {
        log_string("DESCRIPTOR: Investigating someone");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_INVESTIGATION;
      ch->pcdata->order_timer = 30;

      send_to_char("Done.\n\r", ch);

      if (ch->faction != victim->faction)
      ch->pcdata->influence -= 100;

      if (!online)
      free_char(victim);
    }
    else if (!str_cmp(arg1, "file")) {
      if (ch->race != RACE_DEPUTY && ch->race != RACE_FACULTY) {
        if (get_rank(ch, ch->faction) < minrank(ORDER_FILE)) {
          send_to_char("You're not a high enough rank to do that.\n\r", ch);
          return;
        }
      }
      /*
else {
if(!library_room(ch->in_room)) {
send_to_char("You need to be in a research
library.\n\r", ch); return;
}
}
*/

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else if ((victim = get_char_haven(ch, argument)) != NULL && !is_masked(victim) && !is_cloaked(victim))
      online = TRUE;
      else {
        log_string("DESCRIPTOR: Filing someone");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }
      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_FILE;
      ch->pcdata->order_timer = 5;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
    else if (!str_cmp(arg1, "research")) {

      if (is_number(argument)) {
        free_string(ch->pcdata->order_target);
        ch->pcdata->order_target = str_dup(argument);
        ch->pcdata->order_type = ORDER_RESEARCH;
        ch->pcdata->order_timer = 3;
        send_to_char("Done.\n\r", ch);
      }
      else {
        if (ch->race != RACE_DEPUTY) {
          if (get_rank(ch, ch->faction) < minrank(ORDER_RESEARCH)) {
            send_to_char("You're not a high enough rank to do that.\n\r", ch);
            return;
          }
        }
        else {
          if (!library_room(ch->in_room)) {
            send_to_char("You need to be in a research library.\n\r", ch);
            return;
          }
        }

        CHAR_DATA *victim;
        DESCRIPTOR_DATA d;
        bool online = FALSE;
        Buffer outbuf;
        char buf[MSL];

        d.original = NULL;
        if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
        online = TRUE;
        else {
          log_string("DESCRIPTOR: Researching someone 2");

          if (!load_char_obj(&d, argument)) {
            return;
          }

          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
          victim = d.character;
        }
        if (IS_NPC(victim)) {
          if (!online)
          free_char(victim);
          return;
        }

        free_string(ch->pcdata->order_target);
        ch->pcdata->order_target = str_dup(victim->name);
        ch->pcdata->order_type = ORDER_RESEARCH;
        ch->pcdata->order_timer = 30;

        send_to_char("Done.\n\r", ch);

        if (!online)
        free_char(victim);
      }
    }
    else if (!str_cmp(arg1, "trace")) {
      if (get_rank(ch, ch->faction) < minrank(ORDER_TRACE) && ch->race != RACE_DEPUTY) {
        send_to_char("You're not a high enough rank to do that.\n\r", ch);
        return;
      }

      if (is_number(argument)) {
        free_string(ch->pcdata->order_target);
        ch->pcdata->order_target = str_dup(argument);
        ch->pcdata->order_type = ORDER_TRACE;
        ch->pcdata->order_timer = 30;
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Syntax: Order trace (number)\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "reading")) {
      if (get_rank(ch, ch->faction) < minrank(ORDER_READING)) {
        send_to_char("You're not a high enough rank to do that.\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else if ((victim = get_char_haven(ch, argument)) != NULL)
      online = TRUE;
      else {
        log_string("DESCRIPTOR:Reading someone");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_READING;
      ch->pcdata->order_timer = 30;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
    else if (!str_cmp(arg1, "scout")) {
      if (get_rank(ch, ch->faction) < minrank(ORDER_TAIL)) {
        send_to_char("You're not a high enough rank to do that.\n\r", ch);
        return;
      }

      ch->pcdata->order_type = ORDER_SCOUT;
      ch->pcdata->order_timer = 12;

      send_to_char("Done.\n\r", ch);

    }
    else if (!str_cmp(arg1, "tail")) {
      if (get_rank(ch, ch->faction) < minrank(ORDER_TAIL) && ch->race != RACE_DEPUTY) {
        send_to_char("You're not a high enough rank to do that.\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }

      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        send_to_char("Your scouts can't find anyone like that.\n\r", ch);
        return;
      }
      if (is_masked(victim) || victim->shape != SHAPE_HUMAN || is_cloaked(victim)) {
        if (!online)
        free_char(victim);
        send_to_char("Your scouts can't find anyone like that.\n\r", ch);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_TAIL;
      ch->pcdata->order_timer = 12;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
    else if (!str_cmp(arg1, "assault")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You don't have the nessesary authority to do that.\n\r", ch);
        return;
      }
      char argcount[MSL];
      argument = one_argument_nouncap(argument, argcount);
      int amount = atoi(argcount);
      if (amount < 0 || amount > 5 || amount > (clan_lookup(ch->faction)->manpower +
            clan_lookup(ch->faction)->soldier_reinforce)) {
        send_to_char("Syntax, order assault (1-5) (target)\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_haven(ch, argument)) != NULL)
      online = TRUE;
      else {
        log_string("DESCRIPTOR: attacking someone order");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_ATTACK;
      ch->pcdata->order_amount = amount;
      ch->pcdata->order_timer = 120;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
    else if (!str_cmp(arg1, "bodyguard")) {
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("You don't have the nessesary authority to do that.\n\r", ch);
        return;
      }
      char argcount[MSL];
      argument = one_argument_nouncap(argument, argcount);
      int amount = atoi(argcount);
      if (amount < 0 || amount > 5 || amount > (clan_lookup(ch->faction)->manpower +
            clan_lookup(ch->faction)->soldier_reinforce)) {
        send_to_char("Syntax, order bodyguard (1-5) (target)\n\r", ch);
        return;
      }

      CHAR_DATA *victim;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      Buffer outbuf;
      char buf[MSL];

      d.original = NULL;
      if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: bodyguarding someone");

        if (!load_char_obj(&d, argument)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }
      if (victim->pcdata->guard_number > amount && victim->pcdata->guard_expiration > current_time) {
        send_to_char("They already have more guards than that.\n\r", ch);
        if (!online)
        free_char(victim);
        return;
      }

      free_string(ch->pcdata->order_target);
      ch->pcdata->order_target = str_dup(victim->name);
      ch->pcdata->order_type = ORDER_GUARD;
      ch->pcdata->order_amount = amount;
      ch->pcdata->order_timer = 120;

      send_to_char("Done.\n\r", ch);

      if (!online)
      free_char(victim);
    }
    else {
      send_to_char("Syntax: order value/file/research/investigate/tail (target)\n\r", ch);
      return;
    }

    if (str_cmp(arg1, "investigate") && str_cmp(arg1, "value") && str_cmp(arg1, "file"))
    ch->pcdata->influence -= 500;
  }

  _DOFUN(do_banish) {
    CHAR_DATA *victim;
    char arg1[MSL];

    if (ch->faction == 0 || !has_trust(ch, TRUST_WAR, ch->faction) || !has_trust(ch, TRUST_RESOURCES, ch->faction)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (clan_lookup(ch->faction)->resource < 9500) {
      send_to_char("Your faction needs more resources first.\n\r", ch);
      return;
    }
    if (!can_spend_resources(clan_lookup(ch->faction))) {
      send_to_char("You don't have enough resources for that.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      send_to_char("Not on NPCS.\n\r", ch);
      return;
    }
    if (in_fight(victim)) {
      send_to_char("They're a bit busy.\n\r", ch);
      return;
    }
    if (is_gm(victim))
    return;
    if (!in_haven(victim->in_room)) {
      send_to_char("You're not in Haven.\n\r", ch);
      return;
    }

    if (IS_AFFECTED(victim, AFF_UNBANISHED)) {
      send_to_char("Someone has protected them from banishment for the time being.\n\r", ch);
      return;
    }
    if (clinic_staff(victim, FALSE)) {
      send_to_char("Someone has protected them from banishment for the time being.\n\r", ch);
      return;
    }

    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    if (!IS_FLAG(victim->comm, COMM_BANISHED))
    SET_FLAG(victim->comm, COMM_BANISHED);

    act("At $n's order some well armed men arrive and escort $N out of town.", ch, NULL, victim, TO_NOTVICT);
    act("At your order some well armed men arrive and escort $N out of town.", ch, NULL, victim, TO_CHAR);
    act("At $n's order some well armed men arrive and escort you out of town.", ch, NULL, victim, TO_VICT);
    char_from_room(victim);
    char_to_room(victim, get_room_index(18999));
    use_resources(50, ch->faction, NULL, "a banishment");

    // lookat add resource cost
  }

  _DOFUN(do_unbanish) {
    CHAR_DATA *victim;
    char arg1[MSL];

    if (!IS_IMMORTAL(ch)) {
      if (ch->faction == 0 || !has_trust(ch, TRUST_WAR, ch->faction) || !has_trust(ch, TRUST_RESOURCES, ch->faction)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      if (clan_lookup(ch->faction)->resource < 8500) {
        send_to_char("Your faction needs more resources first.\n\r", ch);
        return;
      }
      if (!can_spend_resources(clan_lookup(ch->faction))) {
        send_to_char("You don't have enough resources for that.\n\r", ch);
        return;
      }
    }

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_world(ch, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      send_to_char("Not on NPCS.\n\r", ch);
      return;
    }
    if (in_fight(victim)) {
      send_to_char("They're a bit busy.\n\r", ch);
      return;
    }
    if (is_gm(victim))
    return;

    if (!IS_FLAG(victim->comm, COMM_BANISHED)) {
      send_to_char("They haven't been banished.\n\r", ch);
      return;
    }

    REMOVE_FLAG(victim->comm, COMM_BANISHED);

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = 12 * 60 * 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_UNBANISHED;
    affect_to_char(victim, &af);

    send_to_char("You lift their banishment.\n\r", ch);
    send_to_char("Your banishment is lifted.\n\r", ch);

    use_resources(50, ch->faction, NULL, "lifting a banishment");

    // lookat, add resource cost
  }

  int total_control(FACTION_TYPE *fac) {
    int i = 0;

    return i;
  }

  int startx(int number, int total, int size) {
    if (total == 2) {
      if (number == 1)
      return 20 * size / 500;
      else
      return 480 * size / 500;
    }
    else if (total == 3) {
      if (number == 1)
      return 20 * size / 500;
      else if (number == 2)
      return 480 * size / 500;
      else
      return 250 * size / 500;
    }
    else if (total == 4) {
      if (number == 1)
      return 250 * size / 500;
      else if (number == 2)
      return 250 * size / 500;
      else if (number == 3)
      return 20 * size / 500;
      else
      return 480 * size / 500;
    }
    else if (total == 5) {
      if (number == 1)
      return 250 * size / 500;
      else if (number == 2)
      return 20 * size / 500;
      else if (number == 3)
      return 480 * size / 500;
      else if (number == 4)
      return 100 * size / 500;
      else
      return 400 * size / 500;
    }
    else {
      if (number == 1)
      return 250 * size / 500;
      else if (number == 2)
      return 20 * size / 500;
      else if (number == 3)
      return 480 * size / 500;
      else if (number == 4)
      return 20 * size / 500;
      else if (number == 5)
      return 480 * size / 500;
      else
      return 250 * size / 500;
    }
  }

  int starty(int number, int total, int size) {
    if (total == 2) {
      if (number == 1)
      return 250 * size / 500;
      else
      return 250 * size / 500;
    }
    else if (total == 3) {
      if (number == 1)
      return 400 * size / 500;
      else if (number == 2)
      return 400 * size / 500;
      else
      return 100 * size / 500;
    }
    else if (total == 4) {
      if (number == 1)
      return 20 * size / 500;
      else if (number == 2)
      return 480 * size / 500;
      else if (number == 3)
      return 250 * size / 500;
      else
      return 250 * size / 500;
    }
    else if (total == 5) {
      if (number == 1)
      return 480 * size / 500;
      else if (number == 2)
      return 300 * size / 500;
      else if (number == 3)
      return 300 * size / 500;
      else if (number == 4)
      return 100 * size / 500;
      else
      return 100 * size / 500;
    }
    else {
      if (number == 1)
      return 480 * size / 500;
      else if (number == 2)
      return 150 * size / 500;
      else if (number == 3)
      return 150 * size / 500;
      else if (number == 4)
      return 350 * size / 500;
      else if (number == 5)
      return 350 * size / 500;
      else
      return 20 * size / 500;
    }
  }

  int startxbunker(int faction, int type) {
    FACTION_TYPE *fac;

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      fac = (*it);

      if (fac->vnum == faction) {
      }
    }

    if (faction == leftfaction)
    return 0;
    else if (faction == rightfaction)
    return 500;
    else
    return 250;
  }

  int startybunker(int faction, int type) {
    FACTION_TYPE *fac;

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      fac = (*it);

      if (fac->vnum == faction) {
      }
    }

    if (faction == leftfaction)
    return 400;
    else if (faction == rightfaction)
    return 400;
    else
    return 100;
  }

  void init_locations(int type) {
    leftfaction = 1;
    rightfaction = 2;

    if (rightfaction == leftfaction)
    rightfaction++;
  }

  int number_soldiers(FACTION_TYPE *fac, int battle) {
    int number = 0;

    return number;
  }

  int number_lie(FACTION_TYPE *fac, int battle) {
    int number = 0;

    return number;
  }

  void place_cover(int battlenumber, char *covername, int size) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    int tox = ((number_range(1, 30000) % 800) + 100) * size / 1000;
    int toy = ((number_range(1, 30000) % 785) + 100) * size / 1000;
    for (int i = 0; i < 20 && badcover_point(battlenumber, tox, toy); i++) {
      tox = ((number_range(1, 30000) % 795 - (i * 5)) + 100) * size / 1000;
      toy = ((number_range(1, 30000) % 800 - (i * 5)) + 100) * size / 1000;
    }
    ROOM_INDEX_DATA *desti = battleroom_bycoord(battlenumber, tox, toy);
    int leftx = tox % 50;
    int lefty = toy % 50;

    pMobIndex = get_mob_index(110);
    mob = create_mobile(pMobIndex);
    char_to_room(mob, desti);
    mob->hit = max_hp(mob);
    mob->x = leftx;
    mob->y = lefty;
    free_string(mob->short_descr);
    mob->short_descr = str_dup(covername);
    free_string(mob->name);
    mob->name = str_dup(covername);
    free_string(mob->long_descr);
    mob->long_descr = str_dup(covername);
    mob->in_fight = TRUE;
    if (activeoperation != NULL)
    mob->fight_speed = activeoperation->speed;
  }

  void make_cover(int type, int battlenumber) {
    int size = 250;
    switch (type) {
    case BATTLE_FOREST:
      place_cover(battlenumber, "tree", size);
      place_cover(battlenumber, "brush", size);
      place_cover(battlenumber, "depression", size);
      place_cover(battlenumber, "rock", size);
      place_cover(battlenumber, "log", size);
      place_cover(battlenumber, "sapling", size);
      break;
    case BATTLE_LAKE:
      place_cover(battlenumber, "tree", size);
      place_cover(battlenumber, "rock", size);
      place_cover(battlenumber, "log", size);
      place_cover(battlenumber, "sapling", size);
      break;
    case BATTLE_FIELD:
      place_cover(battlenumber, "log", size);
      place_cover(battlenumber, "rise", size);
      place_cover(battlenumber, "depression", size);
      break;
    case BATTLE_DESERT:
      place_cover(battlenumber, "rise", size);
      place_cover(battlenumber, "depression", size);
      break;
    case BATTLE_TUNDRA:
      place_cover(battlenumber, "rise", size);
      place_cover(battlenumber, "depression", size);
      place_cover(battlenumber, "rock-formation", size);
      break;
    case BATTLE_TOWN:
      place_cover(battlenumber, "sedan", size);
      place_cover(battlenumber, "corner", size);
      place_cover(battlenumber, "truck", size);
      place_cover(battlenumber, "trailer", size);
      break;
    case BATTLE_VILLAGE:
      place_cover(battlenumber, "hut", size);
      place_cover(battlenumber, "wagon", size);
      place_cover(battlenumber, "stall", size);
      place_cover(battlenumber, "corner", size);
      break;
    case BATTLE_CAVE:
      place_cover(battlenumber, "stalagmite", size);
      place_cover(battlenumber, "corner", size);
      place_cover(battlenumber, "depression", size);
      place_cover(battlenumber, "boulder", size);
      place_cover(battlenumber, "outcropping", size);
      break;

    case BATTLE_CITY:
      place_cover(battlenumber, "sedan", size);
      place_cover(battlenumber, "corner", size);
      place_cover(battlenumber, "alley", size);
      place_cover(battlenumber, "bus", size);
      place_cover(battlenumber, "van", size);
      place_cover(battlenumber, "taxi", size);

      break;
    case BATTLE_MOUNTAINS:
      place_cover(battlenumber, "rise", size);
      place_cover(battlenumber, "depression", size);
      place_cover(battlenumber, "rock", size);
      break;
    case BATTLE_WAREHOUSE:
      place_cover(battlenumber, "crates", size);
      place_cover(battlenumber, "container", size);
      place_cover(battlenumber, "workbench", size);
      place_cover(battlenumber, "forklift", size);
      place_cover(battlenumber, "machinery", size);
      break;
    }
  }

  void make_soldier(int faction, int total, int position, int size, int bgnum) {

    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    char buf[MSL];
    sprintf(buf, "OPERATION: Making soldier for %d", faction);
    log_string(buf);

    if (faction > 5) {
      if (clan_lookup(faction) == NULL || safe_strlen(clan_lookup(faction)->soldier_name) < 5)
      return;

      pMobIndex = get_mob_index(HAND_SOLDIER);
      mob = create_mobile(pMobIndex);

      int tox = startx(position + 1, total, size);
      int toy = starty(position + 1, total, size);
      ROOM_INDEX_DATA *desti = battleroom_bycoord(bgnum, tox, toy);

      char_to_room(mob, desti);
      mob->hit = max_hp(mob);
      free_string(mob->short_descr);
      mob->short_descr = str_dup(clan_lookup(faction)->soldier_name);
      free_string(mob->name);
      mob->name = str_dup(clan_lookup(faction)->soldier_name);
      free_string(mob->long_descr);
      mob->long_descr = str_dup(clan_lookup(faction)->soldier_name);
      free_string(mob->description);
      mob->description = str_dup(clan_lookup(faction)->soldier_desc);
      mob->faction = faction;
      free_string(mob->protecting);
      mob->protecting = str_dup(clan_lookup(faction)->name);
      mob->x = tox % 50;
      mob->y = toy % 50;
      if (!IS_FLAG(mob->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);

      mob->attacking = 1;
      mob->fighting = TRUE;
      mob->in_fight = TRUE;
      mob->attack_timer = FIGHT_WAIT * fight_speed(mob) / 2;
      mob->move_timer = FIGHT_WAIT * fight_speed(mob) / 2;

      char buf[MSL];
      sprintf(buf, "OPERATION: Soldier made, sent to room %d.", desti->vnum);
      log_string(buf);
      return;
    }

    if (faction == FACTION_HAND)
    vnum = HAND_SOLDIER;
    if (faction == FACTION_ORDER)
    vnum = ORDER_SOLDIER;
    if (faction == FACTION_TEMPLE)
    vnum = TEMPLE_SOLDIER;

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    int tox = startx(position + 1, total, size);
    int toy = starty(position + 1, total, size);
    ROOM_INDEX_DATA *desti = battleroom_bycoord(bgnum, tox, toy);

    char_to_room(mob, desti);
    mob->hit = max_hp(mob);

    mob->x = tox % 50;
    mob->y = toy % 50;

    if (faction == FACTION_HAND) {
      mob->faction = FACTION_HAND;
      free_string(mob->protecting);
      mob->protecting = str_dup("The Hand");
    }
    else if (faction == FACTION_TEMPLE) {
      mob->faction = FACTION_TEMPLE;
      free_string(mob->protecting);
      mob->protecting = str_dup("The Temple");
    }
    else if (faction == FACTION_ORDER) {
      mob->faction = FACTION_ORDER;
      free_string(mob->protecting);
      mob->protecting = str_dup("The Order");
    }
    mob->in_fight = TRUE;
    if (activeoperation != NULL)
    mob->fight_speed = activeoperation->speed;
  }

  void make_psychic(int total, int position, int size, char *master, int bgnum) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    pMobIndex = get_mob_index(HAND_SOLDIER);
    mob = create_mobile(pMobIndex);

    int tox = startx(position, total, size);
    int toy = starty(position, total, size);
    ROOM_INDEX_DATA *desti = battleroom_bycoord(bgnum, tox, toy);

    char_to_room(mob, desti);
    mob->hit = max_hp(mob);
    free_string(mob->short_descr);
    mob->short_descr = str_dup("shadowy figure");
    free_string(mob->name);
    mob->name = str_dup("shadowy figure");
    free_string(mob->long_descr);
    mob->long_descr = str_dup("`DA shadowy figure`x");

    mob->faction = 200000;
    free_string(mob->protecting);
    mob->protecting = str_dup(master);
    mob->x = tox % 50;
    mob->y = toy % 50;
    if (!IS_FLAG(mob->act, PLR_SHROUD))
    SET_FLAG(mob->act, PLR_SHROUD);

    mob->attacking = 1;
    mob->fighting = TRUE;
    mob->in_fight = TRUE;
    mob->attack_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    mob->move_timer = FIGHT_WAIT * fight_speed(mob) / 2;

    return;
  }

  void make_lie(int faction, int total, int position, int size) {
    /*
int vnum;
MOB_INDEX_DATA *pMobIndex;
CHAR_DATA *mob;

if(faction == FACTION_HAND)
vnum = HAND_LT;
if(faction == FACTION_ORDER)
vnum = ORDER_LT;
if(faction == FACTION_TEMPLE)
vnum = TEMPLE_LT;

pMobIndex = get_mob_index(vnum);
mob = create_mobile(pMobIndex);
char_to_room(mob, get_room_index(BATTLEROOM));
mob->hit = max_hp(mob);

mob->x = startx(position+1, total, size);
mob->y = starty(position+1, total, size);


if(faction == FACTION_HAND)
{
mob->faction = FACTION_HAND;
free_string(mob->protecting);
mob->protecting = str_dup("The Hand");
}
else if(faction == FACTION_TEMPLE)
{
mob->faction = FACTION_TEMPLE;
free_string(mob->protecting);
mob->protecting = str_dup("The Temple");
}
else if(faction == FACTION_ORDER)
{
mob->faction = FACTION_ORDER;
free_string(mob->protecting);
mob->protecting = str_dup("The Order");
}
*/
  }

  void setup_adversary(CHAR_DATA *mob, int type, char *name) {
    if (safe_strlen(name) > 2) {
      free_string(mob->short_descr);
      mob->short_descr = str_dup(name);
      free_string(mob->name);
      mob->name = str_dup(name);
      free_string(mob->long_descr);
      mob->long_descr = str_dup(name);
    }

    if (type == ADVERSARY_MODERN) {
      mob->disciplines[DIS_CARBINES] = 20;
      mob->disciplines[DIS_KNIFE] = 25;
      mob->disciplines[DIS_BARMOR] = 30;
      mob->disciplines[DIS_BOWS] = 0;
      mob->disciplines[DIS_MARMOR] = 0;
      mob->disciplines[DIS_LONGBLADE] = 0;
      mob->pIndexData->intelligence = 15;
    }
    else if (type == ADVERSARY_ARCHAIC) {
      mob->disciplines[DIS_CARBINES] = 0;
      mob->disciplines[DIS_BOWS] = 15;
      mob->disciplines[DIS_KNIFE] = 0;
      mob->disciplines[DIS_LONGBLADE] = 30;
      mob->disciplines[DIS_BARMOR] = 0;
      mob->disciplines[DIS_MARMOR] = 30;
      mob->pIndexData->intelligence = 15;
    }
  }

  void assign_npc_carrier(int bg_num) {
    CHAR_DATA *victim;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;
      victim->bagcarrier = 0;
    }
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;
      if (!IS_NPC(victim))
      continue;
      if (victim->pIndexData->vnum != 115)
      continue;
      victim->bagcarrier = 1;
      return;
    }
  }

  int advercount(int bg_num) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;
      if (!IS_NPC(victim))
      continue;
      if (victim->pIndexData->vnum != 115)
      continue;
      count++;
    }
    return count;
  }

  void assign_pc_carrier(int faction, int bg_num) {
    CHAR_DATA *victim;
    CHAR_DATA *carrier = NULL;
    CHAR_DATA *newcarrier = NULL;
    bool newfound = FALSE;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;
      victim->bagcarrier = 0;
    }

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;

      if (IS_NPC(victim))
      continue;
      if (victim->faction != faction)
      continue;

      carrier = victim;
      if (number_percent() % 2 == 0 && !is_animal(victim)) {
        newfound = TRUE;
        newcarrier = victim;
      }
    }
    if (newfound == TRUE && newcarrier != NULL) {
      newcarrier->bagcarrier = 1;
      send_to_char("You are the carrier.\n\r", newcarrier);
    }
    else if (carrier != NULL) {
      carrier->bagcarrier = 1;
      send_to_char("You are the carrier.\n\r", carrier);
    }
  }

  bool badpoipoint(int number, int x, int y, int bg_num) {
    for (int i = 0; i < 10; i++) {
      if (i != number && activeoperation->poitype[i] != 0 && get_dist(x, y, activeoperation->poix[i], activeoperation->poiy[i]) < 25)
      return TRUE;
    }
    ROOM_INDEX_DATA *room = battleroom_bycoord(bg_num, x, y);
    if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->wall != WALL_NONE)
    return TRUE;
    CHAR_DATA *victim;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;

      if (IS_NPC(victim) && is_cover(victim) && poidistance(victim, x, y) < 30)
      return TRUE;
      if (!IS_NPC(victim) && poidistance(victim, x, y) < 20)
      return TRUE;
    }
    return FALSE;
  }

  int min_cover_dist(int size) { return 75; }

  bool badcover_point(int bg_num, int tox, int toy) {
    CHAR_DATA *victim;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != bg_num)
      continue;
      if (IS_NPC(victim) && is_cover(victim) && poidistance(victim, tox, toy) < min_cover_dist(10))
      return TRUE;
    }
    return FALSE;
  }

  void setup_elite(CHAR_DATA *mob, int type, char *name, int power) {
    if (safe_strlen(name) > 2) {
      free_string(mob->short_descr);
      mob->short_descr = str_dup(name);
      free_string(mob->name);
      mob->name = str_dup(name);
      free_string(mob->long_descr);
      mob->long_descr = str_dup(name);
    }

    if (type == ADVERSARY_MODERN) {
      mob->disciplines[DIS_CARBINES] = 20 * power * 3 / 4;
      mob->disciplines[DIS_KNIFE] = 25 * power * 3 / 4;
      mob->disciplines[DIS_BARMOR] = 30 * power * 4 / 3;
      mob->disciplines[DIS_BOWS] = 0;
      mob->disciplines[DIS_MARMOR] = 0;
      mob->disciplines[DIS_LONGBLADE] = 0;
      mob->pIndexData->intelligence = 15;
    }
    else if (type == ADVERSARY_ARCHAIC) {
      mob->disciplines[DIS_CARBINES] = 0;
      mob->disciplines[DIS_BOWS] = 15 * power * 3 / 4;
      mob->disciplines[DIS_KNIFE] = 0;
      mob->disciplines[DIS_LONGBLADE] = 30 * power * 3 / 4;
      mob->disciplines[DIS_BARMOR] = 0;
      mob->disciplines[DIS_MARMOR] = 30 * power * 4 / 3;
      mob->pIndexData->intelligence = 15;
    }
  }

  void make_elite(int total, int position, int size, int type, char *name, int battlenumber, int count) {

    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    vnum = ADVERSARY_VNUM;

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    int bonus = UMAX(2, count * 2 / 3);

    int tox = startx(position + 1, total, size);
    int toy = starty(position + 1, total, size);
    ROOM_INDEX_DATA *desti = battleroom_bycoord(battlenumber, tox, toy);
    char_to_room(mob, desti);
    setup_elite(mob, type, name, bonus);
    mob->hit = max_hp(mob);
    mob->x = tox % 50;
    mob->y = toy % 50;
    if (!IS_FLAG(mob->act, PLR_SHROUD))
    SET_FLAG(mob->act, PLR_SHROUD);

    mob->attacking = 1;
    mob->fighting = TRUE;
    mob->in_fight = TRUE;
    mob->attack_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    mob->move_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    if (activeoperation != NULL)
    mob->fight_speed = activeoperation->speed;

    return;
  }

  void make_adversary(int total, int position, int size, int type, char *name, int battlenumber) {

    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    vnum = ADVERSARY_VNUM;

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);

    int tox = startx(position + 1, total, size);
    int toy = starty(position + 1, total, size);
    ROOM_INDEX_DATA *desti = battleroom_bycoord(battlenumber, tox, toy);
    char_to_room(mob, desti);
    setup_adversary(mob, type, name);
    mob->hit = max_hp(mob);
    mob->x = tox % 50;
    mob->y = toy % 50;
    if (!IS_FLAG(mob->act, PLR_SHROUD))
    SET_FLAG(mob->act, PLR_SHROUD);

    mob->attacking = 1;
    mob->fighting = TRUE;
    mob->in_fight = TRUE;
    mob->attack_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    mob->move_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    if (activeoperation != NULL)
    mob->fight_speed = activeoperation->speed;

    return;
  }

  void make_leader(int faction, int bgnum, OPERATION_TYPE *op) {
    FACTION_TYPE *fac = clan_lookup(faction);

    if (fac == NULL)
    return;

    int max_rank = -1;
    CHAR_DATA *leader = NULL;

    CHAR_DATA *victim;
    for (DescList::iterator ig = descriptor_list.begin();
    ig != descriptor_list.end(); ++ig) {
      DESCRIPTOR_DATA *d = *ig;

      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_helpless(victim))
      continue;

      if (!battleground(victim->in_room))
      continue;

      if (bg_number(victim->in_room) != bgnum)
      continue;

      if (victim->faction != faction)
      continue;

      if (op != NULL && !is_name(victim->name, op->preferred) && safe_strlen(op->preferred) > 2)
      continue;

      if (get_rank(victim, victim->faction) > max_rank) {
        leader = victim;
        max_rank = get_rank(victim, victim->faction);
      }
    }
    if (leader != NULL) {
      free_string(fac->battle_leader);
      fac->battle_leader = str_dup(leader->name);
      send_to_char("`CYou are appointed as the leader of this combat mission.`x\n\r", leader);
      act("`C$n is appointed as the leader of this combat mission.`x\n\r", leader, NULL, leader, TO_ROOM);
    }
    if (leader == NULL) {
      max_rank = -1;

      for (DescList::iterator ig = descriptor_list.begin();
      ig != descriptor_list.end(); ++ig) {
        DESCRIPTOR_DATA *d = *ig;

        if (d->connected != CON_PLAYING)
        continue;
        victim = CH(d);

        if (victim == NULL)
        continue;

        if (IS_NPC(victim))
        continue;

        if (victim->in_room == NULL)
        continue;

        if (is_helpless(victim))
        continue;

        if (!battleground(victim->in_room))
        continue;

        if (bg_number(victim->in_room) != bgnum)
        continue;

        if (victim->faction != faction)
        continue;
        if (get_rank(victim, victim->faction) > max_rank) {
          leader = victim;
          max_rank = get_rank(victim, victim->faction);
        }
      }
      if (leader != NULL) {
        free_string(fac->battle_leader);
        fac->battle_leader = str_dup(leader->name);
        send_to_char("`CYou are appointed as the leader of this combat mission.`x\n\r", leader);
        act("`C$n is appointed as the leader of this combat mission.`x\n\r", leader, NULL, leader, TO_ROOM);
      }
    }
  }

  int team_power(int faction, int bg_num) {
    FACTION_TYPE *fac = clan_lookup(faction);

    if (fac == NULL)
    return 0;

    int power = 0;
    CHAR_DATA *victim;
    for (DescList::iterator ig = descriptor_list.begin();
    ig != descriptor_list.end(); ++ig) {
      DESCRIPTOR_DATA *d = *ig;

      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);

      if (victim == NULL)
      continue;
      if (IS_NPC(victim))
      continue;
      if (victim->in_room == NULL)
      continue;
      if (is_helpless(victim))
      continue;
      if (!battleground(victim->in_room))
      continue;
      if (bg_number(victim->in_room) != bg_num)
      continue;

      if (victim->faction != faction)
      continue;

      power += (sqrt(get_lvl(victim))) * 10;
    }
    return power;
  }

  int deployable_pop(ROOM_INDEX_DATA *room) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;
      if (victim->in_room != room)
      continue;
      if (IS_NPC(victim))
      continue;
      if (!can_shroud(victim))
      continue;
      if (victim->wounds > 1)
      continue;
      if (is_helpless(victim))
      continue;
      if (victim->faction == 0)
      continue;

      victim->bagcarrier = 0;
      count++;
    }
    return count;
  }

  bool signed_up(CHAR_DATA *ch, OPERATION_TYPE *op, int antagcore, int type) {
    for (int i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, op->sign_up[i]))
      {
        if(antagcore == 0 && type == 0)
        return TRUE;
        if(((antagcore != 0 && antagcore == FACTION_CORE) || (type != 0 && type == FACTION_CORE)) && ch->deploy_core > 0)
        return TRUE;
        if(((antagcore != 0 && antagcore == FACTION_SECT) || (type != 0 && type == FACTION_SECT)) && ch->deploy_sect > 0)
        return TRUE;
        if(((antagcore != 0 && antagcore == FACTION_CULT) || (type != 0 && type == FACTION_CULT)) && ch->deploy_cult > 0)
        return TRUE;



        return FALSE;
      }
    }
    return FALSE;
  }

  bool fac_signed_up(int fvnum, OPERATION_TYPE *op) {
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(op->sign_up[i]) > 2 && get_char_world_pc(op->sign_up[i]) != NULL) {
        CHAR_DATA *victim = get_char_world_pc(op->sign_up[i]);
        if (victim->fcore == fvnum && victim->deploy_core > 0)
        return TRUE;
        if (victim->fsect == fvnum && victim->deploy_sect > 0)
        return TRUE;
        if (victim->fcult == fvnum && victim->deploy_cult > 0)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool defender(CHAR_DATA *ch, OPERATION_TYPE *op) {
    for (int i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, op->sign_up[i]) && op->sign_up_type[i] == 1)
      return TRUE;
    }
    return FALSE;
  }

  bool nomembers(int vnum, OPERATION_TYPE *op) {
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(op->sign_up[i]) > 2) {
        CHAR_DATA *ch = get_char_world_pc(op->sign_up[i]);
        if (ch != NULL && ((ch->fcore == vnum && ch->deploy_core == 1)
              || (ch->fsect == vnum && ch->deploy_sect == 1)
              || (ch->fcult == vnum && ch->deploy_cult == 1)))
        return FALSE;
      }
    }
    return TRUE;
  }

  void operation_assign_one(FACTION_TYPE *fac, OPERATION_TYPE *op, int antagcore) {
    char lbuf[MSL];
    sprintf(lbuf, "Op assign one: %s", fac->name);
    log_string(lbuf);
    fac->deployed_pcs = 0;
    fac->defeated_pcs = 0;
    fac->deployed_power = 0;
    fac->deployed_super = 0;
    fac->deployed_nosuper = 0;

    if (fac->outcast == 1)
    return;
    if (fac->antagonist == 1)
    return;

    if (fac->stasis == 1)
    return;

    if (nomembers(fac->vnum, op))
    return;
    log_string("Has members");
    free_string(fac->battle_leader);
    fac->battle_leader = str_dup("");

    FACTION_TYPE *opfac = clan_lookup(op->faction);
    if (op->goal != GOAL_PSYCHIC) {

      if (clan_lookup(op->faction)->antagonist == 0) {
        if (fac->type != opfac->type)
        return;
      }
      else {
        if (antagcore != fac->type)
        return;
      }
    }
    if (!join_to_operation(fac->vnum, op))
    return;
  }

  void operation_assign_two(FACTION_TYPE *fac, OPERATION_TYPE *op, int antagcore) {
    char lbuf[MSL];
    sprintf(lbuf, "Op assign two: %s", fac->name);
    log_string(lbuf);

    if (fac->outcast == 1)
    return;
    if (fac->antagonist == 1)
    return;
    if (fac->stasis == 1)
    return;
    free_string(fac->battle_leader);
    fac->battle_leader = str_dup("");

    if (op->goal != GOAL_PSYCHIC) {
      FACTION_TYPE *opfac = clan_lookup(op->faction);
      if (clan_lookup(op->faction)->antagonist == 0) {
        if (fac->type != opfac->type)
        return;
      }
      else {
        if (antagcore != fac->type)
        return;
      }
    }

    if (op->faction != fac->vnum) {
      bool found = FALSE;
      for (int i = 0; i < 10; i++) {
        if (fac->vnum == op->enrolled[i])
        found = TRUE;
      }
      if (territory_by_number(op->territoryvnum) != NULL) {
        int base = territory_by_number(op->territoryvnum)->base_faction_core;
        if (base == fac->vnum && fac->type == FACTION_CORE)
        found = TRUE;
        base = territory_by_number(op->territoryvnum)->base_faction_cult;
        if (base == fac->vnum && fac->type == FACTION_CULT)
        found = TRUE;
        base = territory_by_number(op->territoryvnum)->base_faction_sect;
        if (base == fac->vnum && fac->type == FACTION_SECT)
        found = TRUE;
      }

      if (found == FALSE)
      return;
    }
    log_string("Has base");
    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] == 0) {
        battle_factions[i] = fac->vnum;
        return;
      }
    }
  }

  int position_difference(FACTION_TYPE *facone, FACTION_TYPE *factwo) {
    int total = 0, val;
    for (int i = 1; i <= AXES_MAX; i++) {
      val = facone->axes[i] - factwo->axes[i];
      if (i == AXES_CORRUPT)
      val *= 4;
      if (i == AXES_SUPERNATURAL)
      val *= 3;
      if (i == AXES_DEMOCRATIC)
      val *= 2;

      if (val < 0)
      val *= -1;
      total += val;
    }
    total += 2;
    // char buf[MSL];
    // sprintf(buf, "Position Difference: %s and %s, %d", facone->name, // factwo->name, total); log_string(buf);
    return total;
  }

  void operation_assign_three(FACTION_TYPE *fac, OPERATION_TYPE *op, int antagcore) {
    char lbuf[MSL];
    sprintf(lbuf, "Op assign one: %s", fac->name);
    log_string(lbuf);

    if (fac->outcast == 1)
    return;
    if (fac->antagonist == 1)
    return;
    if (fac->stasis == 1)
    return;

    free_string(fac->battle_leader);
    fac->battle_leader = str_dup("");

    bool found = FALSE;
    bool base = FALSE;
    if (op->faction != fac->vnum) {
      for (int i = 0; i < 10; i++) {
        if (fac->vnum == op->enrolled[i])
        found = TRUE;
      }
      if (territory_by_number(op->territoryvnum) != NULL) {
        if (has_base(fac, territory_by_number(op->territoryvnum))) {
          found = TRUE;
          base = TRUE;
        }
      }
    }
    if (base == FALSE && battle_factions[2] != 0)
    return;
    log_string("Empty slot");

    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] != 0) {
        if (position_difference(fac, clan_lookup(battle_factions[i])) < 25 && base == FALSE)
        return;
      }
    }
    if (found == FALSE) {
      if (!join_to_operation(fac->vnum, op))
      return;
    }

    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] == 0) {
        battle_factions[i] = fac->vnum;
        return;
      }
    }
  }

  int border_count(FACTION_TYPE *fac) {
    int count = 0;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (!border_territory((*it)))
      continue;
      if (has_base(fac, (*it))) {
        count++;
      }
    }
    return count;
  }

  bool unreachable_room(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return TRUE;
    if (ch->in_room->area == NULL)
    return TRUE;
    if (ch->in_room->area->vnum == 12 || ch->in_room->area->vnum == 26 || ch->in_room->area->vnum == 27 || ch->in_room->area->vnum == 28 || ch->in_room->area->vnum == 30)
    return TRUE;
    return FALSE;
  }

  bool can_deploy(CHAR_DATA *victim, OPERATION_TYPE *op, int antagcore)
  {
    if (victim == NULL)
    return FALSE;
    if (IS_NPC(victim))
    return FALSE;
    if (victim->in_room == NULL)
    return FALSE;
    if (IS_FLAG(victim->act, PLR_BOUND))
    return FALSE;
    if (unreachable_room(victim))
    return FALSE;;
    if (battleground(victim->in_room))
    return FALSE;
    if (is_ghost(victim))
    return FALSE;
    if (clan_lookup(victim->faction) == NULL)
    return FALSE;
    if (victim->wounds > 1)
    return FALSE;
    if (!signed_up(victim, op, 0, 0))
    return FALSE;

    return TRUE;

  }

  bool try_deploy(CHAR_DATA *victim, OPERATION_TYPE *op, int vnum, STORYLINE_TYPE *storyline, int dtype, int homepower, int battle_total, int battlesize, int battleground_number, int mcorepower)
  {
    char lbuf[MSL];
    sprintf(lbuf, "Trying to deploy %s for faction %d, dtype %d, homepower %d, battle_total %d, battlesize %d", victim->name, vnum, dtype, homepower, battle_total, battlesize);
    log_string(lbuf);
    int pop = 0;
    int maxhum = 0;
    if (op->max_pcs >= 6)
    maxhum = 2;
    else if (op->max_pcs >= 3)
    maxhum = 1;
    char lstr[MSL];

    FACTION_TYPE *infac = clan_lookup(vnum);
    FACTION_TYPE *depo = NULL;
    for (int i = 0; i < 6; i++) {
      if (clan_lookup(battle_factions[i]) != NULL && clan_lookup(battle_factions[i])->alliance ==
          infac->alliance && clan_lookup(battle_factions[i])->type == infac->type)
      depo = clan_lookup(battle_factions[i]);
    }
    if (depo == NULL)
    {
      log_string("Faction not in battle factions");
      return FALSE;
    }

    if(depo->type == FACTION_CORE && victim->deploy_core < 1)
    return FALSE;
    if(depo->type == FACTION_SECT && victim->deploy_sect < 1)
    return FALSE;
    if(depo->type == FACTION_CULT && victim->deploy_cult < 1)
    return FALSE;

    log_string("Deployment setting cleared");

    if(op->competition != COMPETE_CLOSED)
    infac->last_deploy = current_time;


    for (int i = 0; i < 6; i++) {
      if (depo->vnum == battle_factions[i]) {
        sprintf(lbuf, "Battle match: %d, competition %d", i, op->competition);
        if(op->competition == COMPETE_RESTRICT && dtype > 2 && homepower > 0)
        {
          bool pass = FALSE;
          if(depo->deployed_power + sqrt(get_lvl(victim)) * 10 <= homepower)
          pass = TRUE;
          if(pass == TRUE)
          log_string("Passed Restrictions");
          else
          {
            log_string("Failed restrictions");
            return FALSE;
          }
        }
        if(mcorepower > 0)
        {
          if(depo->deployed_power + sqrt(get_lvl(victim)) * 10 > mcorepower)
          {
            log_string("Failed mcore restrictions");
            return FALSE;
          }
        }
        if ((!is_super(victim) && get_tier(victim) <= 2 && depo->deployed_nosuper < maxhum) || depo->deployed_super < op->max_pcs) {
          sprintf(lstr, "%s deploying-%d for %s, deployed %d, max %d.", victim->name, dtype, depo->name, depo->deployed_pcs, op->max_pcs);
          log_string(lstr);
          to_spectre(victim, TRUE);
          if (victim->faction != depo->vnum)
          battle_faction(victim, depo->vnum);

          if (IS_FLAG(victim->act, PLR_HIDE))
          do_function(victim, &do_unhide, "");
          victim->pcdata->deploy_from = victim->in_room->vnum;
          char_from_room(victim);
          int tox =
          startx(i + 1, battle_total, battlesize) + number_range(-8, 8);
          int toy =
          starty(i + 1, battle_total, battlesize) + number_range(-8, 8);
          ROOM_INDEX_DATA *desti =
          battleroom_bycoord(battleground_number, tox, toy);
          char_to_room(victim, desti);
          pop++;
          victim->x = tox % 50;
          victim->y = toy % 50;
          victim->facing =
          roomdirection(victim->in_room->x, victim->in_room->y, first_battleroom(battleground_number)->x + 2, first_battleroom(battleground_number)->y + 2);
          send_to_char("Reality twists around you.\n\r", victim);
          free_string(victim->pcdata->title);
          victim->pcdata->title = str_dup("");
          free_string(victim->pcdata->place);
          victim->pcdata->place = str_dup("");
          if (op->competition != COMPETE_CLOSED) {
            victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED]++;
            victim->pcdata->life_tracker[TRACK_OPERATIONS_ATTENDED]++;
          }
          if (!is_super(victim) && get_tier(victim) <= 2 && depo->deployed_nosuper < maxhum)
          depo->deployed_nosuper++;
          else
          depo->deployed_super++;

          if (storyline != NULL)
          join_to_storyline(victim, storyline);
          return TRUE;
        }
      }
    }
    return FALSE;
  }



  void launch_operation(OPERATION_TYPE *op) {
    char lbuf[MSL];
    log_string("Launching Operation.");

    std::random_device rd;
    // Create a random number generator
    std::mt19937 g(rd());

    // Shuffle the vector
    std::shuffle(FacVect.begin(), FacVect.end(), g);


    if (isactiveoperation == TRUE) {
      if (activeoperation->initdays > op->initdays) {


        clan_lookup(op->faction)->manpower += op->home_soldiers;
        for (int i = 0; i < 10; i++) {
          if (clan_lookup(op->enrolled[i]) != NULL)
          clan_lookup(op->enrolled[i])->manpower += op->soldiers[i];
        }
        op->hour = 0;
        op->valid = FALSE;
        return;
      }
      else {
        lose_operation();
        end_battle();
      }
      isactiveoperation = FALSE;
    }
    if (event_cleanse == 1) {
      op->valid = FALSE;
      return;
    }
    //    LOCATION_TYPE *oploc;
    //    oploc = territory_by_number(op->territoryvnum);

    if (op->goal == GOAL_INCITE)
    clan_lookup(op->faction)->last_incite = current_time;

    for (int i = 0; i < 6; i++) {
      battle_factions[i] = 0;
    }
    battle_defender = 0;
    clan_lookup(op->faction)->last_operation = current_time;
    bool soloop = FALSE;
    CHAR_DATA *launcher = get_char_world_pc(op->author);
    if (launcher != NULL) {
      if (op->speed > 3 && str_cmp(clan_lookup(op->faction)->leader, launcher->name))
      give_intel(launcher, 1000);
      else
      give_intel(launcher, 200);
    }
    antagcore = -1;
    if (number_percent() % 3 == 0)
    antagcore = FACTION_CORE;
    else if (number_percent() % 2 == 0)
    antagcore = FACTION_CULT;
    else
    antagcore = FACTION_SECT;

    FACTION_TYPE *host = clan_lookup(op->faction);
    if (clan_lookup(op->faction)->antagonist == 1) {
      log_string("Antag op");
      int corecount = 0;
      int cultcount = 0;
      int sectcount = 0;
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if (nomembers((*it)->vnum, op) == FALSE) {
          if ((*it)->type == FACTION_CORE)
          corecount++;
          else if ((*it)->type == FACTION_SECT)
          sectcount++;
          else if ((*it)->type == FACTION_CULT)
          cultcount++;
        }
      }
      if (antagcore == FACTION_CORE && corecount < 1) {
        if (sectcount > cultcount)
        antagcore = FACTION_SECT;
        else
        antagcore = FACTION_CULT;
      }
      if (antagcore == FACTION_CULT && cultcount < 1) {
        if (sectcount > corecount)
        antagcore = FACTION_SECT;
        else
        antagcore = FACTION_CORE;
      }
      if (antagcore == FACTION_SECT && sectcount < 1) {
        if (cultcount > corecount)
        antagcore = FACTION_CULT;
        else
        antagcore = FACTION_CORE;
      }
      sprintf(lbuf, "Counts: cult: %d, sect %d, core: %d, antagcore: %d", cultcount, sectcount, corecount, antagcore);
      log_string(lbuf);
    }
    else
    {
      antagcore = host->type;
    }
    int mcorepower = -1;

    if(antagcore == FACTION_CORE)
    mcorepower = max_core_power(op, host);


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
      if (battleground(victim->in_room))
      continue;
      if (is_helpless(victim))
      continue;
      if (is_ghost(victim))
      continue;

      if (signed_up(victim, op, antagcore, host->type)) {
        if (host->antagonist == 1) {
          if (antagcore == FACTION_CORE && victim->fcore != 0)
          victim->faction = victim->fcore;
          else if (antagcore == FACTION_CULT && victim->fcult != 0)
          victim->faction = victim->fcult;
          else if (antagcore == FACTION_SECT && victim->fsect != 0)
          victim->faction = victim->fsect;
        }
        else {
          if (host->type == FACTION_CORE && victim->fcore != 0)
          victim->faction = victim->fcore;
          else if (host->type == FACTION_CULT && victim->fcult != 0)
          victim->faction = victim->fcult;
          else if (host->type == FACTION_SECT && victim->fsect != 0)
          victim->faction = victim->fsect;
        }
      }
    }

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if (fac_signed_up((*it)->vnum, op))
      operation_assign_one((*it), op, antagcore);
    }
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if (fac_signed_up((*it)->vnum, op))
      operation_assign_two((*it), op, antagcore);
    }
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if (fac_signed_up((*it)->vnum, op))
      operation_assign_three((*it), op, antagcore);
    }
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 6; j++) {
        if (battle_factions[j] == battle_factions[i] && j > i)
        battle_factions[j] = 0;
      }
    }

    for (int i = 0; i < 10; i++) {
      if (clan_lookup(op->enrolled[i]) != NULL && op->enrolled[i] != battle_factions[0] && op->enrolled[i] != battle_factions[1] && op->enrolled[i] != battle_factions[2] && op->enrolled[i] != battle_factions[3] && op->enrolled[i] != battle_factions[4] && op->enrolled[i] != battle_factions[5]) {
        clan_lookup((op)->enrolled[i])->manpower += (op)->soldiers[i];
      }
    }
    if (battle_factions[0] == 0) {
      if (clan_lookup(op->faction) != NULL && clan_lookup(op->faction)->antagonist != 0) {
        activeoperation = op;
        win_operation(op->faction, NULL);
      }
      op->hour = 0;
      return;
    }
    if(battle_factions[3] == 0)
    {
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if((*it)->antagonist == 1 && (*it)->vnum != battle_factions[0])
        {
          if(number_percent() % 9 == 0)
          {
            if(battle_factions[3] == 0)
            battle_factions[3] = (*it)->vnum;
            else
            battle_factions[4] = (*it)->vnum;
          }
        }
      }
    }

    if (battle_factions[1] == 0)
    soloop = TRUE;
    char lstring[MSL];
    sprintf(lstring, "OPERATION:%d %d %d %d %d %d", battle_factions[0], battle_factions[1], battle_factions[2], battle_factions[3], battle_factions[4], battle_factions[5]);
    log_string(lstring);
    sprintf(lstring, "OPERATION:%d %d %d %d %d %d", op->enrolled[0], op->enrolled[1], op->enrolled[2], op->enrolled[3], op->enrolled[4], op->enrolled[5]);
    log_string(lstring);


    if (clan_lookup(op->faction)->antagonist == 1)
    {
      if(soloop == TRUE)
      op->challenge = clan_lookup(op->faction)->antag_solo_wins;
      else
      op->challenge = clan_lookup(op->faction)->antag_group_wins;
    }


    int battleground_number = first_available_battleground();
    op->battleground_number = battleground_number;
    int battle_type;
    int battle_territory;
    battle_type = op->terrain;
    battle_territory = op->territoryvnum;
    battle_factions[5] = 0;

    if (safe_strlen(op->storyline) > 2 && get_storyline(NULL, op->storyline) != NULL && op->competition != COMPETE_CLOSED) {
      if (op->speed > 3 && safe_strlen(op->storyrunners) > 1)
      get_storyline(NULL, op->storyline)->power += 2;
      add_mastermind(get_storyline(NULL, op->storyline), op->author, 1);
    }
    STORYLINE_TYPE *storyline = get_storyline(NULL, op->storyline);

    for (int x = 0; x < 20; x++) {
      int swapone = number_range(0, 5);
      int swaptwo = number_range(0, 5);
      if (battle_factions[swapone] != 0 && battle_factions[swaptwo] != 0) {
        int temp = battle_factions[swapone];
        battle_factions[swapone] = battle_factions[swaptwo];
        battle_factions[swaptwo] = temp;
      }
    }

    int pop = 0;
    int battle_total = 0;
    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] != 0)
      battle_total++;
    }
    battle_total++;
    activeoperation = op;
    make_cover(battle_type, battleground_number);
    bool offworld = FALSE;
    int battlesize = 250;
    op->size = battlesize;
    char lstr[MSL];
    LOCATION_TYPE *loc;
    int maxhum = 0;
    if (op->max_pcs >= 6)
    maxhum = 2;
    else if (op->max_pcs >= 3)
    maxhum = 1;

    if (battle_territory >= 0) {
      loc = territory_by_number(battle_territory);
      if (loc != NULL) {
        for (ROOM_INDEX_DATA *broom = first_battleroom(battleground_number);
        broom != NULL; broom = next_battleroom(battleground_number, broom)) {
          assign_weather(broom, loc, TRUE);
        }
      }
      if (loc->continent == CONTINENT_OTHER || loc->continent == CONTINENT_GODREALM || loc->continent == CONTINENT_WILDS || loc->continent == CONTINENT_HELL)
      offworld = TRUE;
    }
    arrange_battleground(battle_type, battleground_number, op->room_name);

    battle_countdown = 0;


    if(op->goal != GOAL_PSYCHIC) {

      for (int i = 0; i < 6; i++) {
        FACTION_TYPE *aonefac = clan_lookup(battle_factions[i]);
        if(aonefac  != NULL && aonefac ->antagonist == 1)
        {
          aonefac ->battle_x = startx(i + 1, battle_total, battlesize);
          aonefac ->battle_y = starty(i + 1, battle_total, battlesize);
        }

      }

    }

    if (op->goal == GOAL_PSYCHIC) {
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
        if (battleground(victim->in_room))
        continue;
        if (unreachable_room(victim))
        continue;
        if (IS_FLAG(victim->act, PLR_BOUND))
        continue;
        if (is_ghost(victim))
        continue;
        if (!is_name(victim->name, op->preferred) && safe_strlen(op->preferred) > 2)
        continue;
        if (victim->wounds > 1)
        continue;
        if (!signed_up(victim, op, 0, 0))
        continue;
        if (!IS_FLAG(victim->comm, COMM_STORY))
        continue;

        if (clan_lookup(victim->faction) != NULL && op->competition != COMPETE_CLOSED)
        clan_lookup(victim->faction)->last_deploy = current_time;
        to_spectre(victim, TRUE);
        int tox, toy;
        if (defender(victim, op)) {
          victim->factiontrue = victim->faction;
          victim->faction = 200000;
          tox = startx(1, 2, battlesize) + number_range(-8, 8);
          toy = starty(1, 2, battlesize) + number_range(-8, 8);
        }
        else {
          victim->factiontrue = victim->faction;
          victim->faction = 300000;
          tox = startx(2, 2, battlesize) + number_range(-8, 8);
          toy = starty(2, 2, battlesize) + number_range(-8, 8);
        }

        if (IS_FLAG(victim->act, PLR_HIDE))
        do_function(victim, &do_unhide, "");
        victim->pcdata->deploy_from = victim->in_room->vnum;
        char_from_room(victim);
        ROOM_INDEX_DATA *desti =
        battleroom_bycoord(battleground_number, tox, toy);
        char_to_room(victim, desti);
        pop++;
        victim->x = tox % 50;
        victim->y = toy % 50;
        victim->facing =
        roomdirection(victim->in_room->x, victim->in_room->y, first_battleroom(battleground_number)->x + 2, first_battleroom(battleground_number)->y + 2);

        if (offworld == TRUE)
        send_to_char("Reality twists around you.\n\r", victim);
        else
        send_to_char("Reality twists around you.\n\r", victim);
        free_string(victim->pcdata->title);
        victim->pcdata->title = str_dup("");
        free_string(victim->pcdata->place);
        victim->pcdata->place = str_dup("");
        if (op->competition != COMPETE_CLOSED) {
          victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED]++;
          victim->pcdata->life_tracker[TRACK_OPERATIONS_ATTENDED]++;
        }
      }
    }


    //Deploying host and allies who are preferred.
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);
      if(!can_deploy(victim, op, antagcore))
      continue;
      if (!is_name(victim->name, op->preferred) && safe_strlen(op->preferred) > 2)
      continue;
      bool has_deployed = FALSE;
      if(antagcore == FACTION_CORE && victim->fcore > 0 && clan_lookup(victim->fcore) != NULL && clan_lookup(victim->fcore)->alliance == host->alliance)
      {
        victim->faction = victim->fcore;
        has_deployed = try_deploy(victim, op, victim->fcore, storyline, 1, 0, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && antagcore == FACTION_CULT && victim->fcult > 0 && clan_lookup(victim->fcult) != NULL && clan_lookup(victim->fcult)->alliance == host->alliance)
      {
        victim->faction = victim->fcult;
        has_deployed = try_deploy(victim, op, victim->fcult, storyline, 1, 0, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && antagcore == FACTION_SECT && victim->fsect > 0 && clan_lookup(victim->fsect) != NULL && clan_lookup(victim->fsect)->alliance == host->alliance)
      {
        victim->faction = victim->fsect;
        has_deployed = try_deploy(victim, op, victim->fsect, storyline, 1, 0, battle_total, battlesize, battleground_number, mcorepower);
      }
    }

    //Deploying host and allies, non-preferred
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);
      if(!can_deploy(victim, op, antagcore))
      continue;

      bool has_deployed = FALSE;
      if(antagcore == FACTION_CORE && victim->fcore > 0 && clan_lookup(victim->fcore) != NULL && clan_lookup(victim->fcore)->alliance == host->alliance)
      {
        victim->faction = victim->fcore;
        has_deployed = try_deploy(victim, op, victim->fcore, storyline, 2, 0, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && antagcore == FACTION_CULT && victim->fcult > 0 && clan_lookup(victim->fcult) != NULL && clan_lookup(victim->fcult)->alliance == host->alliance)
      {
        victim->faction = victim->fcult;
        has_deployed = try_deploy(victim, op, victim->fcult, storyline, 2, 0, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && antagcore == FACTION_SECT && victim->fsect > 0 && clan_lookup(victim->fsect) != NULL && clan_lookup(victim->fsect)->alliance == host->alliance)
      {
        victim->faction = victim->fsect;
        has_deployed = try_deploy(victim, op, victim->fsect, storyline, 2, 0, battle_total, battlesize, battleground_number, mcorepower);
      }
    }


    int homepower = team_power(op->faction, battleground_number);
    sprintf(lstr, "deployed homepower %d", homepower);
    log_string(lstr);

    //Others
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);
      if(!can_deploy(victim, op, antagcore))
      continue;

      bool has_deployed = FALSE;
      if(antagcore == FACTION_CORE && victim->fcore > 0 && clan_lookup(victim->fcore) != NULL)
      {
        victim->faction = victim->fcore;
        has_deployed = try_deploy(victim, op, victim->fcore, storyline, 3, homepower, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && antagcore == FACTION_CULT && victim->fcult > 0 && clan_lookup(victim->fcult) != NULL)
      {
        victim->faction = victim->fcult;
        has_deployed = try_deploy(victim, op, victim->fcult, storyline, 3 ,homepower, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && antagcore == FACTION_SECT && victim->fsect > 0 && clan_lookup(victim->fsect) != NULL)
      {
        victim->faction = victim->fsect;
        has_deployed = try_deploy(victim, op, victim->fsect, storyline, 3, homepower, battle_total, battlesize, battleground_number, mcorepower);
      }


      if(has_deployed == FALSE && victim->fcore > 0 && clan_lookup(victim->fcore) != NULL)
      {
        victim->faction = victim->fcore;
        has_deployed = try_deploy(victim, op, victim->fcore, storyline, 3, homepower, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && victim->fcult > 0 && clan_lookup(victim->fcult) != NULL)
      {
        victim->faction = victim->fcult;
        has_deployed = try_deploy(victim, op, victim->fcult, storyline, 3,homepower, battle_total, battlesize, battleground_number, mcorepower);
      }
      if(has_deployed == FALSE && victim->fsect > 0 && clan_lookup(victim->fsect) != NULL)
      {
        victim->faction = victim->fsect;
        has_deployed = try_deploy(victim, op, victim->fsect, storyline, 3, homepower, battle_total, battlesize, battleground_number, mcorepower);
      }
    }



    int fac_power = 0;
    int max_power = 0;

    if (battle_total >= 3 && safe_strlen(op->description) >= 150 && op->speed >= 4) {
      if (get_char_world_pc(op->author) != NULL) {
        give_karma(get_char_world_pc(op->author), 400, KARMA_OTHER);
        if (!battleground(get_char_world_pc(op->author)->in_room))
        give_karma(get_char_world_pc(op->author), 400, KARMA_OTHER);
      }
    }

    for (int i = 0; i < 100; i++) {
      if (safe_strlen(op->sign_up[i]) > 1) {
        CHAR_DATA *point = get_char_world_pc(op->sign_up[i]);
        if (point == NULL || !battleground(point->in_room)) {
          free_string(op->sign_up[i]);
          op->sign_up[i] = str_dup("");
        }
      }
    }

    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] != 0 && battle_factions[i] > 5) {
        make_leader(battle_factions[i], battleground_number, op);
        fac_power = team_power(battle_factions[i], battleground_number);
        if (fac_power > max_power)
        max_power = fac_power;
      }
    }
    max_power = max_power * 25 / 100;

    if (battle_total > 2) {
      for (int i = 1; i < battle_total; i++)
      max_power = max_power * 9 / 10;
    }
    op->initial_power = max_power;

    if (op->challenge > 0) {
      for (int i = 0; i < op->challenge; i++)
      max_power = max_power * 12 / 10;
    }
    if (op->home_soldiers > 0) {
      max_power = max_power * (100 + (op->home_soldiers * 5)) / 100;
      //    for(int i=0;i<op->home_soldiers;i++)
      //        max_power = max_power + 10;
    }

    if (soloop == TRUE && op->competition != COMPETE_CLOSED)
    max_power = max_power * 3 /2;

    op->power = max_power;
    if (op->goal != GOAL_PSYCHIC) {
      for (int i = 0; i < 6; i++) {
        if (clan_lookup(battle_factions[i]) != NULL) {
          int bonusold = 0;
          CHAR_DATA *supp = NULL;
          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;

            if (d->connected != CON_PLAYING)
            continue;
            supp = CH(d);
            if (supp == NULL)
            continue;
            if (IS_NPC(supp))
            continue;
            if (supp->faction != battle_factions[i])
            continue;
            if (battleground(supp->in_room) && in_world(supp) == WORLD_OTHER)
            bonusold += supp->pcdata->other_fame_level;
            if (battleground(supp->in_room) && in_world(supp) == WORLD_WILDS)
            bonusold += supp->pcdata->wilds_fame_level;
            if (battleground(supp->in_room) && in_world(supp) == WORLD_GODREALM)
            bonusold += supp->pcdata->godrealm_fame_level;
            if (battleground(supp->in_room) && in_world(supp) == WORLD_HELL)
            bonusold += supp->pcdata->hell_fame_level;
          }
          char buf[MSL];
          sprintf(buf, "OPERATION BONUS SOLDIERS: %d for %d", bonusold, i);
          log_string(buf);
          bonusold = UMIN(bonusold, 8);
          if (bonusold > 0) {
            for (int x = 0; x < bonusold; x++)
            make_soldier(battle_factions[i], battle_total, i, battlesize, battleground_number);
          }

          if (loc != NULL && has_base(clan_lookup(battle_factions[i]), loc) && (clan_lookup(battle_factions[i])->type == clan_lookup(op->faction)->type || clan_lookup(battle_factions[i])->type == antagcore)) {
            int troops = UMIN(4, op->max_pcs);

            if (border_territory(loc))
            troops =
            troops / UMAX(1, border_count(clan_lookup(battle_factions[i])));

            for (int x = 0; x < troops; x++)
            make_soldier(battle_factions[i], battle_total, i, battlesize, battleground_number);
          }
          if (battle_factions[i] == op->faction && op->home_soldiers > 0) {
            for (int x = 0; x < op->home_soldiers; x++) {
              make_soldier(battle_factions[i], battle_total, i, battlesize, battleground_number);
            }
          }
          for (int f = 0; f < 10; f++) {
            if (op->enrolled[f] == battle_factions[i] && op->enrolled[f] > 0 && op->soldiers[f] > 0) {
              for (int x = 0; x < op->soldiers[f]; x++) {
                make_soldier(battle_factions[i], battle_total, i, battlesize, battleground_number);
              }
            }
          }
        }
      }
    }

    if (op->goal == GOAL_PSYCHIC) {
      for (int i = 0; i < psychic_soldiers(op->target); i++)
      make_psychic(2, 1, battlesize, op->target, battleground_number);
    }

    bool admade = FALSE;
    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] == 0 && admade == FALSE && op->goal != GOAL_PSYCHIC) {
        admade = TRUE;

        log_string("Making adversaries.");

        int avdvalue = ADVERSARY_VALUE;
        make_adversary(battle_total, i, battlesize, op->adversary_type, op->adversary_name, battleground_number);
        max_power -= avdvalue;
        for (int i = 0; max_power > avdvalue && i < 5; i++) {
          make_adversary(battle_total, i, battlesize, op->adversary_type, op->adversary_name, battleground_number);
          max_power -= avdvalue;
        }
      }
      FACTION_TYPE *atwofac = clan_lookup(battle_factions[i]);
      if(atwofac != NULL && atwofac->antagonist == 1)
      {
        antag_soldiers(op, atwofac);
      }
    }

    CHAR_DATA *newvict;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;
      if (newvict->in_room == NULL || !battleground(newvict->in_room)) // || bg_number(newvict->in_room) !=
      // battleground_number)
      continue;

      newvict->in_fight = TRUE;
      newvict->fight_fast = TRUE;
      newvict->attacking = 1;
      newvict->fight_speed = op->speed;
      if (!IS_FLAG(newvict->act, PLR_SHROUD))
      SET_FLAG(newvict->act, PLR_SHROUD);
      if (IS_FLAG(newvict->act, PLR_DEEPSHROUD))
      REMOVE_FLAG(newvict->act, PLR_DEEPSHROUD);
    }
    isactiveoperation = TRUE;
    activeoperation = op;
    for (int i = 0; i < 10; i++) {
      op->poix[i] = 0;
      op->poiy[i] = 0;
      op->poitype[i] = 0;
      op->poifaction[i] = 0;
      op->poibg[i] = 0;
    }
    if (op->goal == GOAL_PSYCHIC) {
      op->timer = 30;
      if (op->speed == 2) {
        op->timer = 60;
      }
      if (op->speed == 3) {
        op->timer = 90;
      }
      if (op->speed >= 4) {
        op->timer = 120;
      }
    }
    if (op->type == OPERATION_INTERCEPT) {
      for (int i = 0; i < 6; i++) {
        if (battle_factions[i] != 0) {
          bool found = FALSE;
          for (int j = 0; j < 10; j++) {
            if (op->poitype[j] == 0 && found == FALSE) {
              op->poix[j] = startx(i + 1, battle_total, battlesize);
              op->poiy[j] = starty(i + 1, battle_total, battlesize);
              op->poitype[j] = POI_EXTRACT;
              op->poifaction[j] = battle_factions[i];
              op->poibg[j] = battleground_number;
              found = TRUE;
            }
          }
        }
      }
      assign_npc_carrier(battleground_number);
      op->timer = 12;
      op->waves = 3;
      if (op->speed == 2) {
        op->timer = 28;
        op->waves = 7;
      }
      if (op->speed == 3) {
        op->timer = 40;
        op->waves = 10;
      }
      if (op->speed >= 4) {
        op->timer = 48;
        op->waves = 12;
      }
    }
    if (op->type == OPERATION_EXTRACT) {
      op->poitype[0] = POI_EXTRACT;
      op->poifaction[0] = 0;
      op->poix[0] = ((number_range(1, 25000) % 800) + 100) * battlesize / 1000;
      op->poiy[0] = ((number_range(1, 25000) % 800) + 100) * battlesize / 1000;
      for (; badpoipoint(0, op->poix[0], op->poiy[0], battleground_number) ==
      TRUE;) {
        op->poix[0] = ((number_range(1, 27000) % 800) + 100) * battlesize / 1000;
        op->poiy[0] = ((number_range(1, 27000) % 800) + 100) * battlesize / 1000;
      }
      op->poibg[0] = battleground_number;
      assign_pc_carrier(op->faction, battleground_number);
      op->timer = 20;
      op->waves = 5;
      if (op->speed == 2) {
        op->timer = 32;
        op->waves = 8;
      }
      if (op->speed == 3) {
        op->timer = 40;
        op->waves = 10;
      }
      if (op->speed >= 4) {
        op->timer = 48;
        op->waves = 12;
      }
    }
    if (op->type == OPERATION_CAPTURE) {
      op->poitype[0] = POI_CAPTURE;
      op->poifaction[0] = 0;
      op->poix[0] = ((number_range(1, 25000) % 800) + 100) * battlesize / 1000;
      op->poiy[0] = ((number_range(1, 25000) % 800) + 100) * battlesize / 1000;
      for (; badpoipoint(0, op->poix[0], op->poiy[0], battleground_number) ==
      TRUE;) {
        op->poix[0] = ((number_range(1, 27000) % 800) + 100) * battlesize / 1000;
        op->poiy[0] = ((number_range(1, 27000) % 800) + 100) * battlesize / 1000;
      }
      op->poibg[0] = battleground_number;

      op->timer = 140;
      op->upload = 20;
      op->upload -= op->speed;
      op->waves = 4;
      if (op->speed == 2) {
        op->waves = 7;
      }
      if (op->speed == 3) {
        op->waves = 12;
      }
      if (op->speed == 4) {
        op->waves = 15;
      }
      if (op->speed == 5) {
        op->waves = 18;
      }
      if (op->speed >= 6) {
        op->waves = 20;
      }
    }
    if (op->type == OPERATION_MULTIPLE) {
      for (int x = 0; x < 3; x++) {
        op->poitype[x] = POI_CAPTURE;
        op->poifaction[x] = 0;
        op->poix[x] = ((number_range(1, 25000) % 800) + 100) * battlesize / 1000;
        op->poiy[x] = ((number_range(1, 25000) % 800) + 100) * battlesize / 1000;
        for (; badpoipoint(x, op->poix[x], op->poiy[x], battleground_number) ==
        TRUE;) {
          op->poix[x] =
          ((number_range(1, 27000) % 800) + 100) * battlesize / 1000;
          op->poiy[x] =
          ((number_range(1, 27000) % 800) + 100) * battlesize / 1000;
        }
        op->poibg[x] = battleground_number;
      }
      op->timer = 140;
      op->upload = 25;
      op->upload -= op->speed * 3 / 2;
      op->waves = 4;
      if (op->speed == 2) {
        op->waves = 7;
      }
      if (op->speed == 3) {
        op->waves = 12;
      }
      if (op->speed == 4)
      op->waves = 15;
      if (op->speed == 5)
      op->waves = 18;
      if (op->speed >= 6) {
        op->waves = 20;
      }
    }
  }

  void defend_battle() {
    win_battle(battle_defender, 1);
    end_battle();
  }

  void end_battle() {

    CHAR_DATA *ch;
    int vnum;
    ROOM_INDEX_DATA *to_room;

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {

      if ((*it)->race <= 0 || (*it)->race > 200 || (*it)->sex < 0 || (*it)->sex > 10) {
        ++it;
        continue;
      }

      ch = *it;
      ++it;

      if (ch == NULL || !ch)
      continue;

      if (ch->race < 0 || ch->race > 200 || ch->sex < 0 || ch->sex > 10)
      continue;

      if (ch->in_room == NULL)
      continue;

      if (!battleground(ch->in_room))
      continue;

      if (!IS_NPC(ch)) {
        reclaim_items(ch);
      }

      if (IS_NPC(ch)) {
        char_from_room(ch);
        char_to_room(ch, get_room_index(2));
        ch->wounds = 4;
        ch->ttl = 1;
      }
      else {
        char_from_room(ch);

        vnum = ch->pcdata->deploy_from;
        if (vnum == 0) {
          vnum = 1;
        }
        to_room = get_room_index(vnum);
        char_to_room(ch, to_room);
        if (ch->factiontrue > -1) {
          ch->faction = ch->factiontrue;
          ch->factiontrue = -1;
        }
        wake_char(ch);
      }
    }
  }

  int battle_count(int faction) {
    CHAR_DATA *ch;
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      ch = *it;
      ++it;

      if (ch == NULL || !ch)
      continue;

      if (ch->race < 0 || ch->race > 200 || ch->sex < 0 || ch->sex > 10)
      continue;

      if (ch->in_room == NULL)
      continue;

      if (!battleground(ch->in_room))
      continue;

      if (IS_FLAG(ch->act, ACT_COVER))
      continue;

      if (ch->faction == faction)
      count++;
    }
    return count;
  }

  void win_battle(int faction, int defensive) {
    FACTION_TYPE *fac;

    fac = clan_lookup(faction);

    int count = 0;
    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] != 0 && battle_factions[i] != faction && battle_factions[i] > 3)
      count++;
    }
    int reward = 600 + (400 * count);
    if (defensive)
    reward -= 600;

    int deduction = UMAX(1, 10 - fac->deployed_pcs);

    reward = reward * deduction / 10;

    reward += 50 * fac->defeated_pcs / UMAX(fac->deployed_pcs, 1);

    if (reward < 100)
    reward = 100;
    gain_resources(reward, fac->vnum, NULL, "a combat mission");
    fac->dayresources += reward;
    char buf[MSL];
    if (fac->battleterritory != 0) {

      LOCATION_TYPE *loc = territory_by_number(fac->battleterritory);
      if (loc != NULL) {
        sprintf(buf, "%s has successfully conducted a combat mission in %s.", fac->name, loc->name);
        super_news(buf);

        if (loc->status == STATUS_WAR || loc->status == STATUS_CONTEST) {

          if (fac->alliance != 0) {
            if (fac->type == FACTION_SECT)
            territory_plus(loc, fac->alliance + 10);
            else
            territory_plus(loc, fac->alliance);
          }
          else {
            if (number_percent() % 2 == 0)
            territory_minus(loc, ALLIANCE_SIDELEFT);
            if (number_percent() % 2 == 0)
            territory_minus(loc, ALLIANCE_SIDERIGHT);
            if (number_percent() % 2 == 0)
            territory_minus(loc, ALLIANCE_SIDEMID);

            if (number_percent() % 2 == 0)
            territory_minus(loc, ALLIANCE_SIDELEFT + 10);
            if (number_percent() % 2 == 0)
            territory_minus(loc, ALLIANCE_SIDERIGHT + 10);
            if (number_percent() % 2 == 0)
            territory_minus(loc, ALLIANCE_SIDEMID + 10);
          }
        }
      }
    }

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

      if (IS_FLAG(victim->comm, COMM_NOCOMMS))
      continue;
      if (!has_trust(victim, TRUST_COMMS, victim->faction))
      continue;

      if (clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->attributes[FACTION_COMMS] == 0)
      continue;

      if (victim->faction == 0)
      continue;

      if (is_deaf(victim))
      continue;

      if (victim->faction == faction)
      printf_to_char(
      victim, "Your comms announce that you have suceeded in your mission.\n\r");
      else
      printf_to_char(victim, "Your comms announce that %s has carried the day.\n\r", fac->name);
    }

    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] > 5 && clan_lookup(battle_factions[i]) != NULL && battle_factions[i] != faction) {
        FACTION_TYPE *lfac = clan_lookup(battle_factions[i]);
        reward = 0;
        reward += 50 * lfac->defeated_pcs / UMAX(lfac->deployed_pcs, 1);

        if (lfac->resource > 8000) {
          //            lfac->manpower += 5;
        }
      }
    }
  }

  CHAR_DATA *winning_news(int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac == NULL)
    return NULL;

    CHAR_DATA *leader = NULL;
    CHAR_DATA *commleader = NULL;

    int max_leader = -1;
    int max_comm = -1;

    CHAR_DATA *victim;
    for (DescList::iterator ig = descriptor_list.begin();
    ig != descriptor_list.end(); ++ig) {
      DESCRIPTOR_DATA *d = *ig;

      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->faction != faction)
      continue;

      if (battleground(victim->in_room)) {
        if (get_rank(victim, faction) > max_comm) {
          commleader = victim;
          max_comm = get_rank(victim, faction);
        }
      }
      if (get_rank(victim, faction) > max_leader) {
        leader = victim;
        max_leader = get_rank(victim, faction);
      }
    }

    if (commleader != NULL) {
      return commleader;
    }
    if (leader != NULL) {
      return leader;
    }
    return NULL;
  }

  void comafy(char *name) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (cardinal(victim)) {
      victim->pcdata->coma = current_time + (3600 * 24 * 7 * 6);
      victim->pcdata->psychic_attempts = 0;
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->vnum == 0)
        continue;
        if (!str_cmp((*it)->leader, victim->name)) {
          (*it)->resource = 0;
          (*it)->valid = FALSE;
        }
      }
    }
    if (IS_FLAG(victim->act, PLR_TYRANT)) {
      victim->pcdata->coma = current_time + (3600 * 24 * 7);
      victim->pcdata->psychic_attempts = 0;
      REMOVE_FLAG(victim->act, PLR_TYRANT);
    }
    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  void comaproof(char *name) {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    victim->pcdata->psychic_proof = current_time + (3600 * 24 * 10);
    victim->pcdata->psychic_attempts++;
    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  int second_place(OPERATION_TYPE *op, int winner) {
    int second = 0;

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      CHAR_DATA *ch;
      if ((*it)->race <= 0 || (*it)->race > 200 || (*it)->sex < 0 || (*it)->sex > 10) {
        ++it;
        continue;
      }

      ch = *it;
      ++it;

      if (ch == NULL || !ch)
      continue;

      if (IS_NPC(ch))
      continue;

      if (ch->race < 0 || ch->race > 200 || ch->sex < 0 || ch->sex > 10)
      continue;

      if (ch->in_room == NULL)
      continue;

      if (!battleground(ch->in_room))
      continue;

      if (ch->faction == winner)
      continue;

      second = ch->faction;
    }
    return second;
  }

  int op_inf_cost(int goal) {
    switch (goal) {
    case GOAL_INCITE:
      return 0;
      break;
    case GOAL_CALM:
    case GOAL_PROVOKE:
      return 30;
      break;
    case GOAL_PLANT:
    case GOAL_SETUP:
      return 50;
      break;
    case GOAL_FOOTHOLD:
      return 80;
      break;
    case GOAL_UPROOT:
    case GOAL_SUPPRESS:
      return 50;
      break;
    case GOAL_LOOT:
      return 80;
      break;
    case GOAL_KIDNAP:
      return 75;
      break;
    case GOAL_ASSASSINATE:
    case GOAL_RAZE:
      return 70;
      break;
    case GOAL_RESCUE:
      return 35;
      break;
    case GOAL_SABOUTAGE:
      return 25;
      break;
    }
    return 0;
  }

  void win_operation(int faction, OPERATION_TYPE *op) {
    FACTION_TYPE *fac;
    fac = clan_lookup(faction);

    if (fac != NULL)
    fac->operation_wins++;
    else
    log_string("NULL Faction Win");

    bool active = FALSE;
    if (op == NULL) {
      active = TRUE;
      op = activeoperation;
    }

    if (op->goal == GOAL_PSYCHIC) {
      if (faction != 300000) {
        end_battle();
        return;
      }
      comafy(op->target);
      end_battle();
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
        if (IS_FLAG(victim->comm, COMM_NOCOMMS))
        continue;
        if (!has_trust(victim, TRUST_COMMS, victim->faction))
        continue;
        if (clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->attributes[FACTION_COMMS] == 0)
        continue;
        if (victim->faction == 0)
        continue;
        if (is_deaf(victim))
        continue;
        printf_to_char(victim, "Your comms report %s was defeated in psychic battle.\n\r", op->target);
      }

      return;
    }
    if(fac->antagonist == 1)
    {
      fac->antag_solo_wins -= 1;
      fac->antag_group_wins -= 2;
    }
    else
    {
      FACTION_TYPE *host = clan_lookup(op->faction);
      if(host->antagonist == 1)
      {
        host->antag_solo_wins += 1;
        host->antag_group_wins += 1;
      }
    }
    char buf[MSL];
    if (fac->alliance != 0 && fac->alliance < 4) {
      if (fac->type == FACTION_CULT)
      sprintf(buf, "The %s Alliance has proved victorious.", alliance_names(time_info.cult_alliance_issue, fac->alliance, time_info.cult_alliance_type));
      else
      sprintf(buf, "The %s Alliance has proved victorious.", alliance_names(time_info.sect_alliance_issue, fac->alliance, time_info.sect_alliance_type));
      op_report(buf, NULL);
    }
    else {
      sprintf(buf, "%s has proved victorious.", fac->name);
      op_report(buf, NULL);
    }

    if (active == TRUE)
    clan_lookup(op->faction)->op_hour_ran[get_hour(NULL)]++;

    if (op->competition != COMPETE_CLOSED) {
      //	if(op->speed < 4 || safe_strlen(op->storyrunners) < 2 || number_percent() % 3
      //== 0)
      clan_lookup(op->faction)->weekly_ops++;
    }
    sprintf(buf, "OPERATION Victory %s, Goal:%d, Comp:%d", fac->name, op->goal, op->competition);
    log_string(buf);

    int count = 0;
    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] != 0 && battle_factions[i] != faction && battle_factions[i] > 3)
      count++;
    }
    int reward = 300 + (300 * count);
    reward = reward * (10 + UMIN(5, op->max_pcs)) / 15;

    reward = reward * 3 / 2;

    if (fac->vnum == op->faction)
    reward = reward * 3 / 2;

    if (op->speed == 1)
    reward = reward / 2;

    if (clan_lookup(op->faction)->vnum == fac->vnum) {
      if (safe_strlen(op->storyrunners) > 2)
      reward = reward * 3 / 2;
      sprintf(buf, "OPERATION Base Reward: %d", reward);
      log_string(buf);
      if (fac->vnum == op->faction && op->bonus > 0) {
        reward = reward * (100 + op->bonus) / 100;
      }
      reward = reward * (6 + UMIN(6, op->speed)) / 9;
    }
    else {
      sprintf(buf, "OPERATION Base Reward: %d", reward);
      log_string(buf);
      reward = reward * (12 + UMIN(6, op->speed)) / 18;
    }
    sprintf(buf, "OPERATION reward bonuses: %d", reward);
    log_string(buf);

    int second = second_place(op, faction);
    if (second == 0)
    second = fac->last_defeated;
    if (second != 0 && clan_lookup(second) != NULL) {
      sprintf(buf, "OPERATION WINNER: %s, Second Place: %s", fac->name, clan_lookup(second)->name);
      log_string(buf);

      for (int x = 0; x < 4; x++)
      fac->op_second_place[x + 1] = fac->op_second_place[x];
      fac->op_second_place[0] = second;
    }
    if (fac->antagonist == 0) {
      int breward = reward;
      int total = 0;
      for (int i = 0; i < 25; i++)
      total += clan_lookup(op->faction)->op_hour_ran[i];
      reward *= UMAX(1, total);
      reward /= UMAX(1, UMIN(10, total / 2));
      reward /= UMAX(1, clan_lookup(op->faction)->op_hour_ran[get_hour(NULL)]);
      reward = UMAX(reward, breward / 2);
      reward = UMIN(reward, breward * 5 / 4);

      sprintf(buf, "OPERATION Reward hours: %d", reward);
      log_string(buf);
    }
    else
    reward = reward / 2;
    int weekly = 0;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0)
      continue;
      if ((*it)->alliance != fac->alliance)
      continue;
      if ((*it)->stasis == 1)
      continue;
      weekly += (*it)->weekly_ops;
    }
    weekly += fac->weekly_ops;
    weekly /= 2;
    if (weekly > 0 && fac->antagonist == 0) {
      if (clan_lookup(op->faction)->vnum == fac->vnum) {
        for (int i = 0; i < weekly; i++)
        reward = reward * 3 / 4;
      }
      else {
        for (int i = 0; i < weekly; i++)
        reward = reward * 9 / 10;
      }
    }

    if (op->challenge > 0) {
      for (int i = 0; i < op->challenge; i++)
      reward = reward * 11 / 10;
    }
    sprintf(buf, "OPERATION Reward challenge: %d", reward);
    log_string(buf);

    if (op->faction != fac->vnum && clan_lookup(op->faction)->antagonist == 1)
    reward = reward * 2 / 3;

    if (op->goal == GOAL_LOOT && fac->alliance == clan_lookup(op->faction)->alliance)
    reward *= 2;

    if (op->spam == 1)
    reward = reward * 2 / 3;
    if (op->initdays > 0)
    reward = reward * (70 + op->initdays * 15) / 100;

    bool goalthwarted = FALSE;

    if (reward < 50)
    reward = 50;
    if (op->competition == COMPETE_RESTRICT)
    reward = reward * 3 / 4;
    else if (op->competition == COMPETE_CLOSED)
    reward = 0;
    if (active == FALSE)
    reward = 0;
    int initreward = reward;
    if (active == TRUE) {
      int membercount = 0, totalcount = 0;
      for (int i = 0; i < 100; i++) {
        if (safe_strlen(op->sign_up[i]) > 1 && get_char_world_pc(op->sign_up[i]) != NULL) {
          if (clan_lookup(get_char_world_pc(op->sign_up[i])->faction)->vnum ==
              fac->vnum)
          membercount++;
          else if (clan_lookup(get_char_world_pc(op->sign_up[i])->faction)
              ->alliance == fac->alliance)
          totalcount++;
        }
      }
      totalcount += membercount;
      totalcount = UMAX(totalcount, 1);
      int homemember = membercount;
      int subreward = reward / totalcount;
      totalcount = UMAX(totalcount, 1);
      sprintf(buf, "OPERATION SubReward: %d from %d", subreward, totalcount);
      log_string(buf);

      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->vnum == 0)
        continue;
        if ((*it)->alliance != fac->alliance)
        continue;
        if ((*it)->stasis == 1)
        continue;
        membercount = 0;
        int freward = UMIN(subreward, 800);
        if ((*it)->battlewins > 0) {
          for (int i = 0; i < (*it)->battlewins; i++)
          freward = freward * 3 / 4;
        }
        for (int i = 0; i < 100; i++) {
          if (safe_strlen(op->sign_up[i]) > 1 && get_char_world_pc(op->sign_up[i]) != NULL) {
            CHAR_DATA *member = get_char_world_pc(op->sign_up[i]);
            if (member->factiontrue > -1) {
              member->faction = member->factiontrue;
              member->factiontrue = -1;
            }
            if (clan_lookup(member->faction)->vnum == (*it)->vnum) {
              int mreward = freward;
              if (member->pcdata->op_emotes < 1)
              mreward /= 2;
              else if (member->pcdata->op_emotes < 5)
              mreward = mreward * 2 / 3;
              else
              mreward = mreward * (50 + member->pcdata->op_emotes) / 50;
              mreward /= 2;
              sprintf(buf, "%s winning an operation.", member->name);
              gain_resources(mreward, (*it)->vnum, member, buf);
              member->pcdata->week_tracker[TRACK_CONTRIBUTED] += mreward * 10;
              member->pcdata->life_tracker[TRACK_CONTRIBUTED] += mreward * 10;
            }
          }
        }
        if (freward > 10)
        (*it)->battlewins++;
      }
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->vnum == 0)
        continue;
        if ((*it)->stasis == 1)
        continue;
        for (int i = 0; i < 100; i++) {
          if (safe_strlen(op->sign_up[i]) > 1 && get_char_world_pc(op->sign_up[i]) != NULL) {
            CHAR_DATA *member = get_char_world_pc(op->sign_up[i]);
            if (member->factiontrue > -1) {
              member->faction = member->factiontrue;
              member->factiontrue = -1;
            }
            if (clan_lookup(member->faction)->vnum == (*it)->vnum) {
              if (count > 0) {
                give_clan_power(member, 50);
              }
              else
              give_clan_power(member, 10);
            }
          }
        }
      }
      reward = reward * homemember / totalcount;
    }
    bool controlgoal = FALSE;
    if (generic_faction_vnum(fac->vnum)) {
      if (op->goal != GOAL_CONTROL && fac->alliance != clan_lookup(op->faction)->alliance && fac->antagonist == 0)
      goalthwarted = TRUE;
    }
    else {
      if (op->goal != GOAL_CONTROL && fac->vnum != clan_lookup(op->faction)->vnum && fac->antagonist == 0)
      goalthwarted = TRUE;
    }
    if (fac->vnum != op->faction) {
      if (op->goal == GOAL_UPROOT || op->goal == GOAL_ASSASSINATE || op->goal == GOAL_RAZE || op->goal == GOAL_RESCUE || op->goal == GOAL_INCITE || op->goal == GOAL_CALM || op->goal == GOAL_PROVOKE || op->goal == GOAL_SABOUTAGE)
      goalthwarted = TRUE;
    }
    else {
      if (op->goal == GOAL_PLANT || op->goal == GOAL_SETUP || op->goal == GOAL_ASSASSINATE || op->goal == GOAL_RAZE || op->goal == GOAL_INCITE || op->goal == GOAL_PROVOKE)
      controlgoal = TRUE;
    }

    LOCATION_TYPE *loc;
    int locnum = 0;
    if (op->territoryvnum > 0 && op->competition != COMPETE_CLOSED) {
      log_string("OPERATION: Not Closed");
      loc = territory_by_number(op->territoryvnum);
      if (loc != NULL) {
        if (goal_influence(op->goal, fac->vnum, op_inf_cost(op->goal)) > phil_amount(fac, loc))
        op->goal = GOAL_CONTROL;
        log_string("OPERATION: Territory exists");
        locnum = number_from_territory(loc);
        if (op->goal == GOAL_CONTROL || goalthwarted == TRUE || controlgoal == TRUE) {
          log_string("OPERATION: Control Goal");
          int base = 8;
          if (loc->status == STATUS_WAR)
          base = 17;
          else if (loc->status == STATUS_CONTEST)
          base = 12;
          if (goalthwarted == TRUE || controlgoal == TRUE) {
            if (loc->status == STATUS_WAR)
            base = 12;
            else
            base = 6;
          }
          if (fac->antagonist != 0)
          base *= 2;
          base = base * 3 / 2;
          int smod = faction_secrecy(fac, NULL);
          smod = UMAX(smod, 60);
          smod = UMIN(smod, 130);
          base = base * smod / 100;
          base = base * 2;

          if (fac->vnum == op->faction && fac->weekly_ops > 0)
          base = base * (100 - (fac->weekly_ops * 25)) / 100;

          if (op->spam == 1)
          base = base / 3;
          if (op->initdays > 0)
          base = base * (60 + op->initdays * 20) / 100;

          sprintf(buf, "OPERATION base control: %d", base);
          log_string(buf);
          base = base * UMAX(initreward, 400) / 800;
          sprintf(buf, "OPERATION post control: %d", base);
          log_string(buf);
          base = UMAX(base, 1);
          base = UMIN(base, 60);
          if (fac->last_high_intel > current_time - (3600 * 24 * 2))
          base = base * 2;
          if (base > 0) {
            for (int i = 0; i < base; i++) {
              if (!generic_faction_vnum(fac->vnum)) {
                if (fac->type == FACTION_NPC) {
                  antagonist_plus(loc, fac);
                  if (number_percent() % 10 == 0)
                  territory_minus(loc, ALLIANCE_SIDELEFT);
                  if (number_percent() % 10 == 0)
                  territory_minus(loc, ALLIANCE_SIDERIGHT);
                  if (number_percent() % 10 == 0)
                  territory_minus(loc, ALLIANCE_SIDEMID);
                  if (number_percent() % 10 == 0)
                  territory_minus(loc, ALLIANCE_SIDELEFT + 10);
                  if (number_percent() % 10 == 0)
                  territory_minus(loc, ALLIANCE_SIDERIGHT + 10);
                  if (number_percent() % 10 == 0)
                  territory_minus(loc, ALLIANCE_SIDEMID + 10);

                }
                else if (fac->type == FACTION_CULT) {
                  territory_plus(loc, fac->alliance);
                  antagonist_minus(loc);
                  if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDELEFT)
                  territory_minus(loc, ALLIANCE_SIDELEFT);
                  if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDERIGHT)
                  territory_minus(loc, ALLIANCE_SIDERIGHT);
                  if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDEMID)
                  territory_minus(loc, ALLIANCE_SIDEMID);
                }
                else {
                  territory_plus(loc, fac->alliance + 10);
                  antagonist_minus(loc);
                  if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDELEFT)
                  territory_minus(loc, ALLIANCE_SIDELEFT + 10);
                  if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDERIGHT)
                  territory_minus(loc, ALLIANCE_SIDERIGHT + 10);
                  if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDEMID)
                  territory_minus(loc, ALLIANCE_SIDEMID + 10);
                }

              }
              else {
                territory_plus(loc, fac->vnum);
                if (number_percent() % 10 && fac->vnum != FACTION_HAND)
                territory_minus(loc, FACTION_HAND);
                if (number_percent() % 10 && fac->vnum != FACTION_ORDER)
                territory_minus(loc, FACTION_ORDER);
                if (number_percent() % 10 && fac->vnum != FACTION_TEMPLE)
                territory_minus(loc, FACTION_TEMPLE);
              }
            }
          }
        }
        else {
          if (fac->antagonist == 0) {
            if (fac->type == FACTION_CORE)
            loc->phil_amount[fac->vnum] -=
            goal_influence(op->goal, fac->vnum, op_inf_cost(op->goal));
            else if (fac->type == FACTION_CULT)
            loc->phil_amount[fac->alliance] -=
            goal_influence(op->goal, fac->vnum, op_inf_cost(op->goal));
            else
            loc->phil_amount[fac->alliance + 10] -=
            goal_influence(op->goal, fac->vnum, op_inf_cost(op->goal));
          }
          else {
            for (int i = 0; i < 5; i++) {
              if (!str_cmp(loc->other_name[i], fac->name)) {
                loc->other_amount[i] -=
                goal_influence(op->goal, fac->vnum, op_inf_cost(op->goal));
              }
            }
          }
        }
        if (goalthwarted != TRUE) {
          if (op->goal == GOAL_INCITE) {
            loc->status = STATUS_CONTEST;
            loc->status_timer = -15;
          }
          if (op->goal == GOAL_CALM) {
            loc->status = STATUS_GOVERN;
            loc->status_timer = -15;
          }
          if (op->goal == GOAL_PROVOKE) {
            loc->status = STATUS_WAR;
            loc->status_timer = -15;
          }
          if (op->goal == GOAL_UPROOT) {
            if (fac->type == FACTION_CORE) {
              loc->base_faction_core = 0;
              free_string(loc->base_desc_core);
              loc->base_desc_core = str_dup("");
            }
            if (fac->type == FACTION_SECT) {
              loc->base_faction_sect = 0;
              free_string(loc->base_desc_sect);
              loc->base_desc_sect = str_dup("");
            }
            if (fac->type == FACTION_CULT) {
              loc->base_faction_cult = 0;
              free_string(loc->base_desc_cult);
              loc->base_desc_cult = str_dup("");
            }
          }
          if (op->goal == GOAL_SUPPRESS) {
            for (int i = 0; i < 20; i++) {
              if ((i < 4 && i != fac->alliance) || fac->antagonist != 0)
              loc->phil_amount[i] /= 3;
              else if (i > 4 && i != fac->vnum)
              loc->phil_amount[i] /= 3;
            }
            for (int i = 0; i < 5; i++)
            loc->other_amount[i] /= 4;
          }
          if (op->goal == GOAL_SABOUTAGE) {
            for (int i = 0; i < 20; i++) {
              loc->phil_amount[i] /= 2;
            }
            for (int i = 0; i < 5; i++)
            loc->other_amount[i] /= 3;
          }
          if (op->goal == GOAL_ASSASSINATE) {
            for (int i = 0; i < 20; i++) {
              if (safe_strlen(loc->plant_desc[i]) > 2 && strcasestr(loc->plant_desc[i], op->target) != NULL) {
                free_string(loc->plant_desc[i]);
                loc->plant_desc[i] = str_dup("");
              }
            }
          }
          if (op->goal == GOAL_RAZE) {
            for (int i = 0; i < 20; i++) {
              if (safe_strlen(loc->place_desc[i]) > 2 && strcasestr(loc->place_desc[i], op->target) != NULL) {
                free_string(loc->place_desc[i]);
                loc->place_desc[i] = str_dup("");
              }
            }
          }
          if (op->goal == GOAL_KIDNAP) {
            for (int i = 0; i < 20; i++) {
              if (safe_strlen(fac->kidnap_name[i]) < 2) {
                free_string(fac->kidnap_name[i]);
                fac->kidnap_name[i] = str_dup(op->target);
                free_string(fac->kidnap_territory[i]);
                fac->kidnap_territory[i] = str_dup(loc->name);
                char mbuf[MSL];
                sprintf(mbuf, "Someone close to you from %s has been kidnapped by %s.", loc->name, fac->name);
                message_to_char(op->target, mbuf);
                i = 100;
              }
            }
          }
          if (op->goal == GOAL_RESCUE) {
            for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
            it != FacVect.end(); ++it) {
              if ((*it)->vnum == 0) {
                continue;
              }
              for (int i = 0; i < 20; i++) {
                if (!str_cmp((*it)->kidnap_name[i], op->target) && !str_cmp(loc->name, (*it)->kidnap_territory[i])) {
                  free_string((*it)->kidnap_name[i]);
                  (*it)->kidnap_name[i] = str_dup("");
                }
              }
            }
          }
          if (op->goal == GOAL_FOOTHOLD) {
            if (fac->type == FACTION_CORE)
            loc->base_faction_core = fac->vnum;
            if (fac->type == FACTION_CULT)
            loc->base_faction_cult = fac->vnum;
            if (fac->type == FACTION_SECT)
            loc->base_faction_sect = fac->vnum;
          }
        }
      }
    }

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
      if (IS_FLAG(victim->comm, COMM_NOCOMMS))
      continue;
      if (!has_trust(victim, TRUST_COMMS, victim->faction))
      continue;
      if (clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->attributes[FACTION_COMMS] == 0)
      continue;
      if (victim->faction == 0)
      continue;
      if (is_deaf(victim))
      continue;

      if (op->competition == COMPETE_CLOSED)
      continue;

      if (clan_lookup(victim->faction)->alliance ==
          clan_lookup(faction)->alliance) {
        printf_to_char(
        victim, "Your comms announce that your operation was successful.\n\r");
        if (controlgoal == TRUE) {
          if (!generic_faction_vnum(victim->faction)) {
            if (loc->phil_amount[fac->alliance] >= 10)
            printf_to_char(victim, "You can now perform plant, setup");
            if (loc->phil_amount[fac->alliance] >= 25)
            printf_to_char(victim, ", assassinate, raze, saboutage");
            if (loc->phil_amount[fac->alliance] >= 30)
            printf_to_char(victim, ", calm, provoke");
            if (loc->phil_amount[fac->alliance] >= 35)
            printf_to_char(victim, ", rescue");
            if (loc->phil_amount[fac->alliance] >= 50)
            printf_to_char(victim, ", uproot, suppress");
            if (loc->phil_amount[fac->alliance] >= 75)
            printf_to_char(victim, ", kidnap");
            if (loc->phil_amount[fac->alliance] >= 80)
            printf_to_char(victim, ", loot, foothold");
            send_to_char(" operations in this territory.\n\r", victim);
          }
          else {
            if (loc->phil_amount[fac->vnum] >= 10)
            printf_to_char(victim, "You can now perform plant, setup");
            if (loc->phil_amount[fac->vnum] >= 25)
            printf_to_char(victim, ", assassinate, raze, saboutage");
            if (loc->phil_amount[fac->vnum] >= 30)
            printf_to_char(victim, ", calm, provoke");
            if (loc->phil_amount[fac->vnum] >= 35)
            printf_to_char(victim, ", rescue");
            if (loc->phil_amount[fac->vnum] >= 50)
            printf_to_char(victim, ", uproot, suppress");
            if (loc->phil_amount[fac->vnum] >= 75)
            printf_to_char(victim, ", kidnap");
            if (loc->phil_amount[fac->vnum] >= 80)
            printf_to_char(victim, ", loot, foothold");
            send_to_char(" operations in this territory.\n\r", victim);
          }
        }
      }
      else
      printf_to_char(
      victim, "Your comms announce that %s was victorious in the operation.\n\r", fac->name);
    }
    log_string("OPERATION: Here");
    if (op->competition != COMPETE_CLOSED)
    log_string("OPERATION: Not Closed");
    if (active == TRUE)
    log_string("OPERATION:Active");

    if (op->competition != COMPETE_CLOSED && active == TRUE) {
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->vnum == 0)
        continue;
        if ((*it)->vnum == faction || (*it)->alliance == clan_lookup(faction)->alliance)
        continue;
        if (generic_faction_vnum(fac->vnum) != generic_faction_vnum((*it)->vnum))
        continue;
        reward = 0;
        reward += 25 * (*it)->defeated_pcs;
        bool found = FALSE;
        for (int i = 0; i < 100; i++) {
          for (int j = 0; j < 100; j++) {
            if (safe_strlen(op->sign_up[i]) > 1 && safe_strlen((*it)->member_names[j]) > 1 && !str_cmp(op->sign_up[i], (*it)->member_names[j]) && get_char_world_pc(op->sign_up[i]) != NULL) {
              sprintf(buf, "OPERATION SIGNUP %s: %d", op->sign_up[i], (*it)->vnum);
              log_string(buf);
              found = TRUE;
            }
          }
        }
        if (found == TRUE) {
          if (op->faction == (*it)->vnum && (*it)->college == 0 && op->speed >= 4) {
            (*it)->manpower = UMIN(15, (*it)->manpower + 5);
          }
          else if((*it)->college == 0)
          (*it)->manpower = UMIN(15, (*it)->manpower + 2);
          gain_resources(reward/2, (*it)->vnum, NULL, "an unsuccessful combat mission.");
        }
      }
    }

    if (op->competition != COMPETE_CLOSED && op->goal != GOAL_CONTROL && !goalthwarted && loc != NULL && locnum != 0 && fac->antagonist == 0) {
      CHAR_DATA *ch = winning_news(faction);
      if (ch != NULL) {
        ch->pcdata->factionwin = fac->vnum;
        printf_to_char(
        ch, "You have been given the power to make alterations to the territory %s, to transfer this power to someone else use territory transfer (name) You have up to 2 hours to make these changes.\n\r", loc->name);
        if ((op->goal != GOAL_INCITE || loc->status == STATUS_CONTEST) && (op->goal != GOAL_PROVOKE || loc->status == STATUS_WAR) && (op->goal != GOAL_CALM || loc->status == STATUS_GOVERN)) {
          send_to_char("Use territory timeline (message) to add something to the territory timeline.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_TIMELINE] = 1;
        }
        send_to_char("Use territory news to create a supernatural news article.\n\r", ch);
        ch->pcdata->territory_editing[TEDIT_NUMBER] = locnum;
        ch->pcdata->territory_editing[TEDIT_TIMER] = 120;
        ch->pcdata->territory_editing[TEDIT_NEWS] = 1;

        if (op->goal == GOAL_SETUP) {
          send_to_char("Use territory setup to enter an editor to describe the new location in the territory.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_SETUP] = 1;
        }
        if (op->goal == GOAL_PLANT) {
          send_to_char("Use territory plant to enter an editor to describe the new important NPC in the territory.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_PLANT] = 1;
        }
        if (op->goal == GOAL_FOOTHOLD) {
          send_to_char("Use territory establish to enter an editor to describe your faction's new foothold base in the territory.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_ESTABLISH] = 1;
        }
      }
    }
    if (activeoperation != NULL)
    activeoperation->hour = 0;
    isactiveoperation = FALSE;
  }

  void lose_operation() {
    CHAR_DATA *victim;

    if (activeoperation->goal == GOAL_PSYCHIC) {
      OPERATION_TYPE *op = activeoperation;
      comaproof(activeoperation->target);
      end_battle();
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
        if (IS_FLAG(victim->comm, COMM_NOCOMMS))
        continue;
        if (!has_trust(victim, TRUST_COMMS, victim->faction))
        continue;
        if (clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->attributes[FACTION_COMMS] == 0)
        continue;
        if (victim->faction == 0)
        continue;
        if (is_deaf(victim))
        continue;
        printf_to_char(
        victim, "Your comms report the psychic assault against %s failed.\n\r", op->target);
      }
      activeoperation->hour = 0;
      isactiveoperation = FALSE;
      return;
    }

    if (clan_lookup(activeoperation->faction) != NULL && clan_lookup(activeoperation->faction)->antagonist != 0) {
      win_operation(activeoperation->faction, NULL);
      return;
    }
    if (activeoperation->competition != COMPETE_CLOSED)
    clan_lookup(activeoperation->faction)->weekly_ops++;

    if (activeoperation->competition != COMPETE_CLOSED) {
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

        if (IS_FLAG(victim->comm, COMM_NOCOMMS))
        continue;
        if (!has_trust(victim, TRUST_COMMS, victim->faction))
        continue;

        if (clan_lookup(victim->faction) == NULL || clan_lookup(victim->faction)->attributes[FACTION_COMMS] == 0)
        continue;

        if (victim->faction == 0)
        continue;

        if (is_deaf(victim))
        continue;

        printf_to_char(victim, "Your comms announce that no society claimed victory in the operation.\n\r");
      }
    }
    /*
OPERATION_TYPE *op = activeoperation;


int reward = 0;
for(int i=0;i<6;i++)
{
if(op->competition == COMPETE_CLOSED)
continue;
if(battle_factions[i] > 5 && clan_lookup(battle_factions[i]) != NULL)
{
FACTION_TYPE *lfac = clan_lookup(battle_factions[i]);
reward = 0;
reward += 5*lfac->defeated_pcs;

char buf[MSL];
sprintf(buf, "You gain $%d operational resources from an
unsuccessful combat mission.", reward*10); send_log(lfac->vnum, buf);
give_resources(lfac, reward);

}
}
*/
    activeoperation->hour = 0;
    isactiveoperation = FALSE;
  }

  void winner_check() {

    for (int i = 0; i < 6; i++) {
      if (battle_factions[i] > 0) {
        if (battle_count(battle_factions[i]) > 0) {
          bool winning = TRUE;
          for (int x = 0; x < 6; x++) {
            if (i != x && battle_count(battle_factions[x]) > 0)
            winning = FALSE;
          }
          if (winning == TRUE) {
            win_battle(battle_factions[i], 0);
            end_battle();
          }
        }
      }
    }
  }
  int hour_battles(int hour, int day) {
    int count = 0;

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->battlehour == hour && (*it)->battleday == day)
      count++;
    }
    return count;
  }

  _DOFUN(do_battle) {

    if (in_fight(ch)) {
      do_function(ch, &do_minioncommand, argument);
      return;
    }

    CHAR_DATA *victim;

    if (IS_IMMORTAL(ch)) {
      if (!str_cmp(argument, "stop"))
      end_battle();
      else if (!str_cmp(argument, "launch")) {
        for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
        it != OpVect.end(); ++it) {

          launch_operation(*it);
          return;
        }
        return;
      }
      else if (!str_cmp(argument, "moveup")) {
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->battlehour > 23)
          (*it)->battlehour -= 23;

          if ((*it)->battleday > 0) {
            (*it)->battleday--;
            if ((*it)->battleday == 0) {
              seed_battle(*it);
            }
          }
        }
      }
    }
    char arg[MSL];
    char arg2[MSL];

    argument = one_argument_nouncap(argument, arg);
    argument = one_argument_nouncap(argument, arg2);

    FACTION_TYPE *fac = clan_lookup(ch->faction);

    if (fac == NULL)
    return;

    if (!str_cmp(arg, "attack")) {
      if (str_cmp(ch->name, fac->battle_leader)) {
        send_to_char("You're not the operational leader.\n\r", ch);
        return;
      }
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      free_string(fac->battle_target);
      fac->battle_target = str_dup(victim->name);
      fac->battle_order = BORDER_ATTACK;
      send_to_char("Order given.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "follow")) {
      if (str_cmp(ch->name, fac->battle_leader)) {
        send_to_char("You're not the operational leader.\n\r", ch);
        return;
      }
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      free_string(fac->battle_target);
      fac->battle_target = str_dup(victim->name);
      fac->battle_order = BORDER_FOLLOW;
      send_to_char("Order given.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "atwill")) {
      if (str_cmp(ch->name, fac->battle_leader)) {
        send_to_char("You're not the operational leader.\n\r", ch);
        return;
      }
      free_string(fac->battle_target);
      fac->battle_target = str_dup("");
      fac->battle_order = BORDER_NONE;
      send_to_char("Order given.\n\r", ch);
      return;
    }
  }

  char *battle_owner(int battle) { return "None"; }

  int fac_count_online(int faction) {
    CHAR_DATA *to;
    int count = 0;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (to->faction == faction)
        count++;
      }
    }
    return count;
  }

  int fac_count_total(int faction) {
    FACTION_TYPE *fac;
    int count = 0;

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      fac = (*it);
      if (fac->vnum != faction)
      continue;

      for (int i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1) {
          count++;
        }
      }
    }

    return count;
  }

  vector<CABAL_TYPE *> CabalVect;
  CABAL_TYPE *nullcabal;

  void fread_cabal(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    CABAL_TYPE *cabal;

    cabal = new_cabal();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'D':
        KEY("Description", cabal->description, fread_string(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          CabalVect.push_back(cabal);
          return;
        }
        break;
      case 'L':
        if (!str_cmp(word, "Leader")) {
          cabal->leader = fread_string(fp);
          cabal->leaderinactivity = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'M':
        if (!str_cmp(word, "Member")) {
          int i;
          for (i = 0; i < 100 && safe_strlen(cabal->member_names[i]) > 1; i++) {
          }
          cabal->member_names[i] = fread_string(fp);
          cabal->member_inactivity[i] = fread_number(fp);
          cabal->member_votes[i] = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Message")) {
          int i = fread_number(fp);
          cabal->message_timer[i] = fread_number(fp);
          cabal->messages[i] = fread_string(fp);
          fMatch = TRUE;
        }

      case 'N':
        KEY("Name", cabal->name, fread_string(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_cabal: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_cabals() {
    nullcabal = new_cabal();
    FILE *fp;

    if ((fp = fopen(CABAL_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_Cabals: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "CABAL")) {
          fread_cabal(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Cabals: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open cabals.txt", 0);
      exit(0);
    }
  }

  bool invalid_cabal(CABAL_TYPE *cabal) {
    if (daysidle(cabal->leader) > 29)
    return TRUE;

    for (int i = 0; i < 100; i++) {
      if (safe_strlen(cabal->member_names[i]) > 1)
      return FALSE;
    }
    return TRUE;
  }

  void save_cabals(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/cabals.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/cabals.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/cabals.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/cabals.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/cabals.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/cabals.txt");
      else
      sprintf(buf, "../data/back7/cabals.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open cabals.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen(CABAL_FILE, "w")) == NULL) {
        bug("Cannot open cabal.txt for writing", 0);
        return;
      }
    }

    for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
    it != CabalVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        bug("Save_clans: Blank cabal in vector", i);
        continue;
      }

      if (invalid_cabal((*it)))
      continue;

      fprintf(fpout, "#CABAL\n");
      fprintf(fpout, "Name %s~\n", (*it)->name);

      fprintf(fpout, "Description %s~\n", (*it)->description);

      for (i = 0; i < 20; i++) {
        fprintf(fpout, "Message %d %d %s~\n", i, (*it)->message_timer[i], (*it)->messages[i]);
      }

      fprintf(fpout, "Leader %s~\n%d\n", (*it)->leader, (*it)->leaderinactivity);

      for (i = 0; i < 100; i++) {
        if (safe_strlen((*it)->member_names[i]) > 1)
        fprintf(fpout, "Member %s~\n%d %s~\n", (*it)->member_names[i], (*it)->member_inactivity[i], (*it)->member_votes[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  CABAL_TYPE *get_cabal(CHAR_DATA *ch) {
    int i;
    for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
    it != CabalVect.end(); ++it) {
      for (i = 0; i < 100; i++) {
        if (!str_cmp((*it)->member_names[i], ch->name))
        return (*it);
      }
    }
    return nullcabal;
  }

  _DOFUN(do_group) {
    char arg[MSL];
    char arg2[MSL];
    int i;
    argument = one_argument_nouncap(argument, arg);

    if (!str_cmp(arg, "list")) {
      i = 1;
      for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
      it != CabalVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        printf_to_char(ch, "[%d] %s\n\r", i, (*it)->name);
        i++;
      }
    }
    else if (!str_cmp(arg, "info")) {
      i = 1;
      for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
      it != CabalVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (i == atoi(argument)) {
          printf_to_char(ch, "[%d] %s\nLeader: %s\nDescription: %s\n\r", i, (*it)->name, (*it)->leader, (*it)->description);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg, "delete") && IS_IMMORTAL(ch)) {
      i = 1;
      for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
      it != CabalVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (i == atoi(argument)) {
          (*it)->valid = FALSE;
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg, "rename")) {
      i = 1;
      argument = one_argument_nouncap(argument, arg2);

      for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
      it != CabalVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (i == atoi(arg2)) {
          if (str_cmp((*it)->leader, ch->name)) {
            send_to_char("Only the leader can do that.\n\r", ch);
            return;
          }
          free_string((*it)->name);
          (*it)->name = str_dup(argument);
          send_to_char("Name set.\n\r", ch);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg, "describe")) {
      i = 1;
      for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
      it != CabalVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (i == atoi(argument)) {
          if (str_cmp((*it)->leader, ch->name)) {
            send_to_char("Only the leader can do that.\n\r", ch);
            return;
          }
          string_append(ch, &(*it)->description);

          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg, "create")) {
      if (get_cabal(ch) != NULL && get_cabal(ch) != nullcabal) {
        send_to_char("You're already in a group.\n\r", ch);
        return;
      }
      if (ch->played / 3600 < 10) {
        send_to_char("Play a little longer first.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = new_cabal();
      free_string(cabal->leader);
      cabal->leader = str_dup(ch->name);
      free_string(cabal->member_names[0]);
      cabal->member_names[0] = str_dup(ch->name);
      free_string(cabal->name);
      cabal->name = str_dup(argument);
      CabalVect.push_back(cabal);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "join")) {
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);
      if (str_cmp(cabal->leader, ch->name)) {
        send_to_char("Only the leader can do that.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_haven(ch, argument);
      if (victim == NULL)
      victim = get_char_world_pc(argument);
      if (victim == NULL || IS_NPC(victim) || (get_cabal(victim) != NULL && get_cabal(victim) != nullcabal)) {
        send_to_char("There's nobody like that around who can be joined into a new faction.\n\r", ch);
        return;
      }
      for (i = 0; i < 100 && safe_strlen(cabal->member_names[i]) > 1; i++) {
      }

      free_string(cabal->member_names[i]);
      cabal->member_names[i] = str_dup(victim->name);
      send_to_char("Done.\n\r", ch);
      printf_to_char(victim, "You are joined to %s group.\n\r", cabal->name);
    }
    else if (!str_cmp(arg, "banish")) {
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);
      if (str_cmp(cabal->leader, ch->name)) {
        send_to_char("Only the leader can do that.\n\r", ch);
        return;
      }
      for (i = 0; i < 100; i++) {
        if (!str_cmp(cabal->member_names[i], argument)) {
          free_string(cabal->member_names[i]);
          cabal->member_names[i] = str_dup("");
          free_string(cabal->member_votes[i]);
          cabal->member_votes[i] = str_dup("");
          cabal->member_inactivity[i] = 0;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg, "leave")) {
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);
      for (i = 0; i < 100; i++) {
        if (!str_cmp(cabal->member_names[i], ch->name)) {
          free_string(cabal->member_names[i]);
          cabal->member_names[i] = str_dup("");
          free_string(cabal->member_votes[i]);
          cabal->member_votes[i] = str_dup("");
          cabal->member_inactivity[i] = 0;
          send_to_char("Done.\n\r", ch);
        }
        if (!str_cmp(ch->name, cabal->leader)) {
          free_string(cabal->leader);
          cabal->leader = str_dup("");
        }
      }
    }
    else if (!str_cmp(arg, "vote")) {
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);
      for (i = 0; i < 100; i++) {
        if (!str_cmp(cabal->member_names[i], ch->name)) {
          free_string(cabal->member_votes[i]);
          cabal->member_votes[i] = str_dup(argument);
          send_to_char("Your vote is set, be sure to set your vote to the same name as appears in the roster.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg, "Roster")) {
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);
      printf_to_char(ch, "Leader: %s\n\r", cabal->leader);
      for (i = 0; i < 100; i++) {
        if (safe_strlen(cabal->member_names[i]) > 1) {
          printf_to_char(ch, "%s, supporting %s.\n\r", cabal->member_names[i], cabal->member_votes[i]);
        }
      }
    }
    else if (!str_cmp(arg, "news")) {
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);

      printf_to_char(ch, "`cMessages`W:`x\n\n\r");
      for (i = 0; i < 20; i++) {
        if (safe_strlen(cabal->messages[i]) > 1 && cabal->message_timer[i] > 0)
        printf_to_char(ch, "%s\n\r", cabal->messages[i]);
      }
    }
    else if (!str_cmp(arg, "announce")) {
      CHAR_DATA *to;
      if (get_cabal(ch) == NULL || get_cabal(ch) == nullcabal) {
        send_to_char("You're not in a group.\n\r", ch);
        return;
      }
      CABAL_TYPE *cabal = get_cabal(ch);

      for (i = 0; i < 20 && safe_strlen(cabal->messages[i]) > 1; i++) {
      }
      free_string(cabal->messages[i]);
      cabal->messages[i] = str_dup(argument);
      cabal->message_timer[i] = 50;

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;

          if (get_cabal(to) == cabal) {
            printf_to_char(to, "Your earpiece announces: '%s says %s'\n\r", ch->name, argument);
          }
        }
      }
    }
  }

  void cabal_update() {
    int i, leadervotes = 0;
    int j, maxref, maxvotes, votes;
    CABAL_TYPE *cabal;
    for (vector<CABAL_TYPE *>::iterator it = CabalVect.begin();
    it != CabalVect.end(); ++it) {
      cabal = (*it);

      for (i = 0; i < 20; i++) {
        if (cabal->message_timer[i] > 0) {
          if (number_percent() % 3 == 0)
          cabal->message_timer[i]--;
          if (cabal->message_timer[i] <= 0) {
            free_string(cabal->messages[i]);
            cabal->messages[i] = str_dup("");
          }
        }
      }

      for (i = 0; i < 100; i++) {

        if (get_char_world_pc(cabal->member_names[i]) != NULL) {
          cabal->member_inactivity[i] = 0;
        }
        else {
          if (number_percent() % 5 == 0)
          cabal->member_inactivity[i]++;
          if (cabal->member_inactivity[i] > 500) {
            free_string(cabal->member_names[i]);
            cabal->member_names[i] = str_dup("");
            free_string(cabal->member_votes[i]);
            cabal->member_votes[i] = str_dup("");
            cabal->member_inactivity[i] = 0;
          }
        }

        if (!str_cmp(cabal->member_votes[i], cabal->leader) && safe_strlen(cabal->leader) > 2)
        leadervotes++;
      }
      maxvotes = 0;
      maxref = 0;
      bool leaderranked = FALSE;
      for (i = 0; i < 100; i++) {
        if (safe_strlen(cabal->member_names[i]) > 1 && !str_cmp(cabal->member_names[i], cabal->leader)) {
          leaderranked = TRUE;
        }
      }
      if (!leaderranked) {
        leadervotes = -2;
        free_string(cabal->leader);
        cabal->leader = str_dup("");
      }

      for (i = 0; i < 100; i++) {
        if (safe_strlen(cabal->member_names[i]) > 1) {
          votes = 0;
          for (j = 0; j < 100; j++) {
            if (!str_cmp(cabal->member_votes[j], cabal->member_names[i])) {
              votes++;
              if (votes > maxvotes) {
                maxvotes = votes;
                maxref = i;
              }
            }
          }
        }
      }
      if (maxvotes > leadervotes) {

        free_string(cabal->leader);
        cabal->leader = str_dup(cabal->member_names[maxref]);
      }
    }
  }

  void super_news(char *message) {
    NEWS_TYPE *news;

    news = new_news();
    news->timer = 1500;
    news->stats[0] = -2;
    free_string(news->message);
    news->message = str_dup(message);
    free_string(news->author);
    news->author = str_dup("Supernatural News");
    NewsVect.push_back(news);
  }

  void last_public(CHAR_DATA *ch, int vnum) {
    if (is_cloaked(ch))
    return;

    if (is_masked(ch) && ch->pcdata->tail_mask != 1)
    return;

    if (ch->shape != SHAPE_HUMAN && ch->pcdata->tail_mask != 2)
    return;

    if (!is_masked(ch) && ch->shape == SHAPE_HUMAN && ch->pcdata->tail_mask != 3 && ch->pcdata->tail_mask != 0)
    return;

    ch->pcdata->last_public_room = vnum;
  }

  void send_log(int faction, char *argument) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    char buf[MSL];
    for (i = 1; i < 20; i++) {
      if (fac->log_timer[i - 1] <= 0) {
        free_string(fac->log[i - 1]);
        fac->log[i - 1] = str_dup(fac->log[i]);
        fac->log_timer[i - 1] = fac->log_timer[i];
        fac->log_timer[i] = 0;
        free_string(fac->log[i]);
        fac->log[i] = str_dup("");
      }
    }

    for (i = 0; i < 20 && safe_strlen(fac->log[i]) > 2; i++) {
    }
    free_string(fac->log[i]);
    sprintf(buf, "[%s] %s", newtexttime(), argument);
    fac->log[i] = str_dup(buf);
    fac->log_timer[i] = 200;

    sprintf(buf, "CONTRIBUTE: %s: %s", fac->name, argument);
    log_string(buf);
  }

  void send_brief_log(int faction, char *argument) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    char buf[MSL];
    for (i = 1; i < 20; i++) {
      if (fac->log_timer[i - 1] <= 0) {
        free_string(fac->log[i - 1]);
        fac->log[i - 1] = str_dup(fac->log[i]);
        fac->log_timer[i - 1] = fac->log_timer[i];
        fac->log_timer[i] = 0;
        free_string(fac->log[i]);
        fac->log[i] = str_dup("");
      }
    }

    for (i = 0; i < 20 && safe_strlen(fac->log[i]) > 2; i++) {
    }
    free_string(fac->log[i]);
    sprintf(buf, "[%s] %s", newtexttime(), argument);
    fac->log[i] = str_dup(buf);
    fac->log_timer[i] = 30;
  }

  void faction_daily(FACTION_TYPE *fac) {
    if (fac->weekly_ops > 10)
    fac->weekly_ops = 8;
    char buf[MSL];

    if (fac->last_intel == 0)
    fac->last_intel = current_time;

    if (fac->awards > 2)
    fac->awards = 2;
    if (generic_faction_vnum(fac->vnum))
    fac->secret_days = 0;
    if (fac->stasis == 1)
    fac->secret_days = UMAX(0, fac->secret_days - 1);
    else {
      for (int i = 0; i < 100; i++) {
        if (fac->member_inactivity[i] >= 100)
        inactive_disrepect(fac->member_names[i]);
      }

      int days = 1;
      if (fac->axes[AXES_CORRUPT] != AXES_FARRIGHT && fac->axes[AXES_CORRUPT] != AXES_FARLEFT) {
        days += 1;
        sprintf(buf, "SECRETDAYS %s, +1 non extremist", fac->name);
        log_string(buf);
      }
      if (safe_strlen(fac->missions) < 2) {
        days += 2;
        sprintf(buf, "SECRETDAYS %s, +2 no missions", fac->name);
        log_string(buf);
      }
      if (safe_strlen(fac->manifesto) < 2) {
        days += 2;
        sprintf(buf, "SECRETDAYS %s, +2 no manifesto", fac->name);
        log_string(buf);
      }
      if (safe_strlen(fac->description) < 2 && safe_strlen(fac->manifesto) < 2 && fac->secret_days >= 5) {
        days += 35;
        sprintf(buf, "SECRETDAYS %s, +35 no description", fac->name);
        log_string(buf);
        if (fac->secret_days >= 200)
        days += 50;
      }
      if (fac->outcast == 1) {
        days += 3;
        sprintf(buf, "SECRETDAYS %s, +3 outcast", fac->name);
        log_string(buf);
      }
      if (fac->resource <= 5000) {
        days += 5;
        sprintf(buf, "SECRETDAYS %s, +5 under 50k res", fac->name);
        log_string(buf);
      }
      if (fac->resource >= 20000) {
        days += 1;
        sprintf(buf, "SECRETDAYS %s, +1 over 200k res", fac->name);
        log_string(buf);
      }
      if (fac->axes[AXES_CORRUPT] == AXES_NEUTRAL) {
        days += 4;
        sprintf(buf, "SECRETDAYS %s, +4 neutral", fac->name);
        log_string(buf);
      }
      else if (fac->axes[AXES_CORRUPT] == AXES_NEARLEFT) {
        days += 2;
        sprintf(buf, "SECRETDAYS %s, +2 mild corrupt", fac->name);
        log_string(buf);
      }
      else if (fac->axes[AXES_CORRUPT] == AXES_NEARRIGHT) {
        days += 2;
        sprintf(buf, "SECRETDAYS %s, +2 mild virtue", fac->name);
        log_string(buf);
      }

      if (fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT && fac->restrictions[RESTRICT_SUPERNATURALS] == 0 && fac->restrictions[RESTRICT_MURDERVAMPS] == 0 && (fac->restrictions[RESTRICT_VAMPIRES] == 0 || fac->restrictions[RESTRICT_WEREWOLVES] == 0)) {
        days += 3;
        sprintf(buf, "SECRETDAYS %s, +3 virtuous murderers", fac->name);
        log_string(buf);
      }

      if (fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT && fac->axes[AXES_SUPERNATURAL] <= AXES_MIDLEFT) {
        days += 1;
        sprintf(buf, "SECRETDAYS %s, +1 virtuous pro supernaturals", fac->name);
        log_string(buf);
      }
      if (fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT && fac->axes[AXES_DEMOCRATIC] <= AXES_MIDLEFT) {
        days += 1;
        sprintf(buf, "SECRETDAYS %s, +1 virtuous autocrats", fac->name);
        log_string(buf);
      }

      if (fac->axes[AXES_COMBAT] == AXES_FARLEFT || fac->axes[AXES_COMBAT] == AXES_MIDLEFT || fac->axes[AXES_COMBAT] == AXES_NEARLEFT) {
        if (fac->nopart == 0) {
          if (fac->last_operation < current_time - (3600 * 24 * 30)) {
            days += 1;
            sprintf(buf, "SECRETDAYS %s, +1 combative, non op", fac->name);
            log_string(buf);
            if (fac->last_deploy < current_time - (3600 * 24 * 30)) {
              days += 5;
              sprintf(buf, "SECRETDAYS %s, +5 combative, no deploy", fac->name);
              log_string(buf);
            }
          }
        }
      }
      fac->secret_days += days;
      sprintf(buf, "SECRETDAYS %s, %d", fac->name, days);
      log_string(buf);
    }
    /*
}*/
    if (fac->update > 0) {
      fac->update--;
      return;
    }
    sprintf(buf, "Updating %s, starting resources: %d", fac->name, fac->resource);
    log_string(buf);
    if (fac->stasis == 1) {
      fac->resource -= 10;
      fac->update = 7;
      return;
    }
    if (fac->lifeearned > 2000)
    fac->lifeearned = 2000;
    if (fac->lifeearned < -2000)
    fac->lifeearned = -2000;

    if (generic_faction_vnum(fac->vnum) || protected_faction_vnum(fac->vnum))
    fac->closed = 0;

    if (fac->patrolling > 0)
    fac->patrolling = UMIN(fac->patrolling - 1, fac->patrolling * 3 / 4);
    if (fac->guarding > 0)
    fac->guarding = UMIN(fac->guarding - 1, fac->guarding * 3 / 4);

    for (int i = 0; i < 100; i++)
    fac->member_power[i] = fac->member_power[i] * 6 / 7;

    fac->battlewins = 0;
    fac->update = 7;
    int cost = 0;
    int bonus = 0;
    fac->attributes[FACTION_LOYALTY] = 0;

    if (fac->attributes[FACTION_UNDERSTANDING] > 0 && fac->resource >= 2250 && fac->college == 0)
    cost += 200;
    else if (!generic_faction_vnum(fac->vnum) && fac->college == 0)
    fac->attributes[FACTION_UNDERSTANDING] = 0;

    if (fac->attributes[FACTION_SCOUTS] > 0 && fac->resource >= 50)
    cost += 10;
    else if (!generic_faction_vnum(fac->vnum))
    fac->attributes[FACTION_SCOUTS] = 0;

    if (fac->attributes[FACTION_911] > 0 && fac->resource >= 25)
    cost += 20;
    else if (!generic_faction_vnum(fac->vnum))
    fac->attributes[FACTION_911] = 0;

    if (fac->attributes[FACTION_COMMS] > 0 && fac->resource >= 25)
    cost += 10;
    else if (!generic_faction_vnum(fac->vnum))
    fac->attributes[FACTION_COMMS] = 0;

    if (fac->attributes[FACTION_CORPSE] > 0 && fac->resource >= 100)
    cost += 100;
    else if (!generic_faction_vnum(fac->vnum))
    fac->attributes[FACTION_CORPSE] = 0;

    for (int i = 0; i < 5; i++) {
      if (fac->crystals[i] > 0)
      fac->crystals[i]--;
    }
    sprintf(buf, "Updating %s, starting resources: %d, Cost1: %d", fac->name, fac->resource, cost);
    log_string(buf);

    if (fac->support == 1 || fac->support == 2) {
      bonus += 1000;
    }
    if (fac->support == 3 && fac->resource < 18000) {
      bonus += 2000;
    }
    if (fac->support == 4 && fac->resource < 20000) {
      bonus += 3000;
    }
    if (fac->support == 5 && fac->resource < 20000) {
      bonus += 5000;
    }

    if (fac->dayresources < 50 && !generic_faction_vnum(fac->vnum) && !protected_faction_vnum(fac->vnum))
    bonus = 0;

    if (fac->alliance == 0) {
      bonus = 0;
    }
    if (fac->support < 2)
    bonus = UMIN(bonus, cost);

    if (generic_faction_vnum(fac->vnum)) {
      int acount = 0;
      for (int i = 0; i < 100; i++) {
        if (safe_strlen(fac->member_names[i]) > 1 && fac->member_inactivity[i] < 100 && active_character(fac->member_names[i]))
        acount++;
      }
      for (int i = 0; i < acount; i++)
      cost = cost * 105 / 100;
    }

    fac->dayresources = 0;
    sprintf(buf, "Updating %s, starting resources: %d, Cost3:%d", fac->name, fac->resource, cost);
    log_string(buf);
    use_resources(cost, fac->vnum, NULL, "ongoing costs");
    int resbonus = 0;
    int opbonus = 0;
    for (int i = 0; i < 10; i++) {
      if (fac->enemies[i][0] != 0 && fac->enemies[i][1] == 3 && clan_lookup(fac->enemies[i][0]) != NULL) {
        int newb = 50;
        int amult = fac->axes[AXES_CORRUPT] -
        clan_lookup(fac->enemies[i][0])->axes[AXES_CORRUPT];
        if (amult < 0)
        amult *= -1;
        newb = newb * amult;
        newb = newb * fac_power(clan_lookup(fac->enemies[i][0])) / fac_power(fac);
        resbonus = UMAX(resbonus, newb);
      }
      else if (fac->enemies[i][0] != 0 && fac->enemies[i][1] == 3 && clan_lookup(fac->enemies[i][0]) != NULL) {
        int newb = 50;
        newb = newb * fac_power(clan_lookup(fac->enemies[i][0])) / fac_power(fac);
        opbonus = UMAX(opbonus, newb);
      }
    }
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      for (int j = 0; j < 10; j++) {
        if ((*it)->enemies[j][0] == fac->vnum && (*it)->enemies[j][1] >= 2) {
          int newb = 50;
          newb = newb * fac_power((*it)) / fac_power(fac);
          opbonus = UMAX(opbonus, newb);
        }
      }
    }

    if (resbonus > 0 && resbonus > opbonus)
    gain_resources(resbonus, fac->vnum, NULL, "resistance sympathizers");
    else if (opbonus > 0)
    gain_resources(opbonus, fac->vnum, NULL, "political support");

    if (fac->alchemy > 0) {
      int alval = 0;
      if (fac->alchemy <= 100) {
        alval = fac->alchemy;
      }
      else {
        alval = 100;
        int mult = UMAX(0, fac->alchemy - 100);
        mult = mult * mult;
        mult = (int)cbrt(mult);
        mult = UMAX(0, mult);
        alval += mult;
      }
      gain_resources(alval, fac->vnum, NULL, "alchemical contributions");
      fac->alchemy = 0;
    }
    int embmax = 25;
    if (fac->resource > 10000) {
      embmax += (fac->resource - 10000) / 150;
    }
    if (generic_faction_vnum(fac->vnum))
    fac->steal_mult = 0;
    int emb = number_range(0, embmax);
    embmax = UMIN(embmax, 1000);
    if (emb > 9) {
      if (fac->lifeearned < 0) {
        emb += 150;
        for (int i = 0; i < fac->steal_mult; i++)
        emb = emb * 12 / 10;
        fac->steal_mult++;
      }
      fac->resource -= emb;
      sprintf(buf, "$%d resources have been stolen.", emb * 10);
      send_log(fac->vnum, buf);
    }
    else if (fac->lifeearned < 0) {
      int steal = 150;
      for (int i = 0; i < fac->steal_mult; i++)
      steal = steal * 12 / 10;
      fac->resource -= steal;
      sprintf(buf, "$%d resources have been stolen.", steal * 10);
      send_log(fac->vnum, buf);
      fac->steal_mult++;
    }
    if (fac->lifeearned > 1000 && fac->steal_mult > 0)
    fac->steal_mult--;

    if (fac->lifeearned < -1200)
    fac->steal_mult++;
    fac->lifeearned -= 400;
    fac->weekly_resources = 0;
    if (generic_faction_vnum(fac->vnum)) {
      fac->resource = UMAX(fac->resource, 6000);
      if (fac->resource > 12000) {
        int reduc = sqrt(fac->resource - 12000);
        fac->resource = fac->resource - reduc;
      }
      fac->resource = UMIN(fac->resource, 15000);
    }
    if (protected_faction_vnum(fac->vnum))
    fac->resource = UMAX(fac->resource, 5000);

    if (fac->antagonist != 0)
    fac->resource = 10000;

    if (fac->resource >= 10000) {
      //        fac->manpower += 1;
    }
    fac->weekly_ops = UMAX(0, fac->weekly_ops - 4);
    fac->manpower = UMIN(fac->manpower, 15);
    sprintf(buf, "Updating %s, starting resources: %d, CostFin:%d", fac->name, fac->resource, cost);
    log_string(buf);
    if (get_loc("Navorost")->base_faction_core == fac->vnum)
    gain_resources(500, fac->vnum, NULL, "treasure from the Wilds");
    if (get_loc("Navorost")->base_faction_cult == fac->vnum)
    gain_resources(500, fac->vnum, NULL, "treasure from the Wilds");
    if (get_loc("Navorost")->base_faction_sect == fac->vnum)
    gain_resources(500, fac->vnum, NULL, "treasure from the Wilds");
  }

  bool is_operation(int hour, int days) {
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->hour == 0)
      continue;

      if ((*it)->hour == hour && (*it)->day == days)
      return TRUE;
    }
    return FALSE;
  }

  void make_antag_op(FACTION_TYPE *fac) {
    int hour = number_range(1, 23);
    int days = number_range(3, 7);
    for (; is_operation(hour, days) || hour == 7 || hour == 17;) {
      hour = number_range(1, 23);
      days = number_range(2, 4);
    }

    LOCATION_TYPE *floc = antag_target(fac->vnum);
    if (floc == NULL)
    return;
    OPERATION_TYPE *op = new_operation();
    free_string(op->author);
    op->author = str_dup("Tyr");
    op->territoryvnum = number_from_territory(floc);
    if(floc->continent >= 7 && floc->continent <= 10)
    op->adversary_type = ADVERSARY_ARCHAIC;
    else
    op->adversary_type = ADVERSARY_MODERN;
    free_string(op->adversary_name);
    op->adversary_name = str_dup(fac->soldier_name);
    free_string(op->room_name);
    op->room_name = str_dup("A battlefield");
    free_string(op->upload_name);
    op->upload_name = str_dup("securing the area");
    if (number_percent() % 2 == 0)
    op->type = OPERATION_MULTIPLE;
    else
    op->type = OPERATION_CAPTURE;
    op->goal = GOAL_CONTROL;
    int control = 0;
    for (int i = 0; i < 5; i++) {
      if (!str_cmp(floc->other_name[i], fac->name))
      control = floc->other_amount[i];
    }
    if (control >= 50 && floc->base_faction_core != 0 && floc->base_faction_core != fac->vnum)
    op->goal = GOAL_UPROOT;
    else if (control >= 80 && floc->base_faction_core == 0)
    op->goal = GOAL_FOOTHOLD;
    else if (control >= 75 && floc->base_faction_core == fac->vnum) {
      CHAR_DATA *to;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;
          if (to->faction == 0)
          continue;
          if (!str_cmp(get_loc(to->pcdata->home_territory)->name, floc->name)) {
            op->goal = GOAL_KIDNAP;
            free_string(op->target);
            op->target = str_dup(to->name);
          }
        }
      }
    }
    if (op->goal == GOAL_CONTROL) {
      if (control >= 50 && floc->base_faction_core == fac->vnum) {
        if (number_percent() % 3 == 0)
        op->goal = GOAL_SUPPRESS;
        else if (number_percent() % 2 == 0)
        op->goal = GOAL_CALM;
      }
    }
    op->max_pcs = number_range(2, 8);
    op->speed = number_range(2, 4);
    op->hour = hour;
    op->day = days;
    op->initdays = days;
    op->size = 1000;
    op->terrain = number_range(BATTLE_FOREST, BATTLE_LAKE);
    if ((op->terrain == BATTLE_TOWN || op->terrain == BATTLE_CITY) && floc->continent >= CONTINENT_WILDS)
    op->terrain = BATTLE_VILLAGE;

    char bufname[MSL];
    sprintf(bufname, "%s %s", terrain_name(op->terrain), floc->name);
    free_string(op->room_name);
    op->room_name = str_dup(bufname);

    op->faction = fac->vnum;
    op->challenge = 2;
    if (floc->base_faction_core == fac->vnum)
    op->challenge = 5;
    op->competition = COMPETE_OPEN;
    OpVect.push_back(op);
  }

  bool has_op(int vnum) {
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->faction == vnum)
      return TRUE;
    }
    return FALSE;
  }

  int antag_op_count(void) {
    int count = 0;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->antagonist == 0)
      continue;

      if (has_op((*it)->vnum))
      count++;
    }
    return count;
  }
  int global_op_count(void) {
    int count = 0;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {

      if (has_op((*it)->vnum))
      count++;
    }
    return count;
  }

  _DOFUN(do_antagoperations) { faction_antagonist_update(); }

  void faction_antagonist_update(void) {
    if (global_op_count() > 3 && antag_op_count() > 0)
    return;

    if(antag_op_count() > 1 && number_percent() % 3 != 0)
    return;
    if(antag_op_count() > 2 && number_percent() % 3 != 0)
    return;


    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->antagonist == 0)
      continue;

      if (has_op((*it)->vnum))
      continue;

      if (antag_op_count() < 1 && number_percent() % 17 == 0)
      make_antag_op((*it));
    }

    int op_count = static_cast<int>(OpVect.size());
    if(op_count < 3)
    {
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->antagonist == 0)
        continue;

        if (has_op((*it)->vnum))
        continue;

        if (number_percent() % 17 == 0)
        make_antag_op((*it));
      }
    }
  }

  void faction_daily_update() {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      faction_daily(*it);
    }
  }

  void scout_report(char *message) {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->attributes[FACTION_SCOUTS] > 0)
      send_message((*it)->vnum, message);
    }
  }
  void scout_report_temp(char *message) {
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->attributes[FACTION_SCOUTS] > 0)
      send_message_temp((*it)->vnum, message);
    }
  }

  ROOM_INDEX_DATA *deployment_room(CHAR_DATA *ch) {
    int vnum = 0;
    ROOM_INDEX_DATA *to_room;
    vnum = ch->pcdata->deploy_from;
    if (vnum == 0) {
      vnum = 205509554;
    }
    to_room = get_room_index(vnum);
    return to_room;
  }

  int faction_pay(CHAR_DATA *ch, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    if (fac == NULL || fac == nullfac) {
      return 0;
    }

    if (ch->pcdata->bloodaura > 0 && !IS_FLAG(ch->act, PLR_GUEST))
    return 0;

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], ch->name)) {
        if (fac->member_suspended[i] > 0)
        return 0;

        return (fac->member_pay[i]);
      }
    }
    return 0;
  }

  void reset_faction_pay(CHAR_DATA *ch, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    if (fac == NULL || fac == nullfac) {
      return;
    }

    if (ch->pcdata->bloodaura > 0 && !IS_FLAG(ch->act, PLR_GUEST))
    return;

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], ch->name)) {
        use_resources(fac->member_pay[i] / 10, fac->vnum, NULL, "");
        fac->member_pay[i] = 0;
      }
    }
  }

  bool altfaction(CHAR_DATA *ch, FACTION_TYPE *fac) {
    if (ch->pcdata->account != NULL) {
      for (int i = 0; i < 25; i++) {
        if (safe_strlen(ch->pcdata->account->characters[i]) > 1) {
          if (str_cmp(ch->name, ch->pcdata->account->characters[i])) {
            for (int x = 0; x < 100; x++) {
              if (safe_strlen(fac->member_names[x]) > 1) {
                if (!str_cmp(fac->member_names[x], ch->pcdata->account->characters[i])) {
                  return TRUE;
                }
              }
            }
          }
        }
      }
    }
    else if (ch->desc->account != NULL) {
      for (int i = 0; i < 25; i++) {
        if (safe_strlen(ch->desc->account->characters[i]) > 1) {
          if (str_cmp(ch->name, ch->desc->account->characters[i])) {
            for (int x = 0; x < 100; x++) {
              if (safe_strlen(fac->member_names[x]) > 1) {
                if (!str_cmp(fac->member_names[x], ch->desc->account->characters[i])) {
                  return TRUE;
                }
              }
            }
          }
        }
      }
    }
    return FALSE;
  }

  _DOFUN(do_contribute) {
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    argument = one_argument_nouncap(argument, arg1);

    if (is_gm(ch) || higher_power(ch) || (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type != GUEST_NIGHTMARE && ch->pcdata->guest_type != GUEST_OPERATIVE))
    return;

    if (ch->pcdata->overworked > 0) {
      send_to_char("You are too tired.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "influence")) {
      argument = one_argument_nouncap(argument, arg2);
      int amount = atoi(arg2);

      if (amount > ch->pcdata->super_influence || amount < 250) {
        send_to_char("Syntax: contribute influence (amount) (faction)\n\r", ch);
        return;
      }
      FACTION_TYPE *fac = clan_lookup_name(argument);
      if (fac == NULL) {
        send_to_char("No such faction.\n\r", ch);
        return;
      }
      ch->pcdata->super_influence -= amount;

      amount = amount * 25 / 10;

      amount /= 100;

      amount = amount * (5 + get_skill(ch, SKILL_SOCIALFOCUS)) / 5;

      //    if(altfaction(ch, fac))
      //        amount = 0;

      sprintf(buf, "%s contributing influence.", ch->name);
      ch->pcdata->week_tracker[TRACK_CONTRIBUTED] += amount * 10;
      ch->pcdata->life_tracker[TRACK_CONTRIBUTED] += amount * 10;

      gain_resources(amount, fac->vnum, ch, buf);
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "blood")) {
      FACTION_TYPE *fac = clan_lookup_name(argument);
      if (fac == NULL) {
        send_to_char("No such faction.\n\r", ch);
        return;
      }

      if (!operating_room(ch->in_room)) {
        send_to_char("You have to be in a medical room first.\n\r", ch);
        return;
      }
      if (base_lifeforce(ch) <= 10000) {
        send_to_char("You can only do that if you have more than 100% life force.\n\r", ch);
        return;
      }
      int amount = base_lifeforce(ch) - 10000;
      use_lifeforce(ch, amount, "blood donation");
      amount *= get_tier(ch);
      amount /= 10;
      amount = UMIN(amount, 200);
      if (is_vampire(ch))
      amount = amount * 2 / 3;
      sprintf(buf, "%s contributing blood.", ch->name);
      ch->pcdata->week_tracker[TRACK_CONTRIBUTED] += amount * 10;
      ch->pcdata->life_tracker[TRACK_CONTRIBUTED] += amount * 10;
      gain_resources(amount, fac->vnum, ch, buf);
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "money")) {
      argument = one_argument_nouncap(argument, arg2);
      int amount = atoi(arg2);

      if (amount > ch->pcdata->total_money / 100 || amount < 5) {
        send_to_char("Syntax: contribute money (amount) (faction)\n\r", ch);
        send_to_char("You must have the available funds in your bank account.\n\r", ch);
        return;
      }
      FACTION_TYPE *fac = clan_lookup_name(argument);
      if (fac == NULL) {
        send_to_char("No such faction.\n\r", ch);
        return;
      }
      ch->pcdata->total_money -= amount * 100;

      //        if(altfaction(ch, fac))
      //            amount = 0;

      amount = amount * (5 + get_skill(ch, SKILL_BUSINESSFOCUS)) / 5;

      sprintf(buf, "%s contributing money.", ch->name);
      ch->pcdata->week_tracker[TRACK_CONTRIBUTED] += amount;
      ch->pcdata->life_tracker[TRACK_CONTRIBUTED] += amount;

      gain_resources(amount / 10, fac->vnum, ch, buf);
      send_to_char("Done.\n\r", ch);
      return;
    }
    else {
      FACTION_TYPE *fac = clan_lookup_name(argument);
      if (fac == NULL) {
        send_to_char("No such faction.\n\r", ch);
        return;
      }
      if (!goblin_market(ch->in_room)) {
        send_to_char("You need to be in the gobin market to do that.\n\r", ch);
        return;
      }
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;
        if (obj->pIndexData->vnum != 38 && obj->pIndexData->vnum != 36)
        continue;
        //        if(altfaction(ch, fac))
        //            continue;

        if (obj->level <= 0) {
          send_to_char("That isn't worth anything.\n\r", ch);
          return;
        }

        if (is_name(arg1, obj->name)) {
          if (obj->pIndexData->vnum == 36) {
            int val = obj->level;
            // 1000 = 5% = 500
            val /= 2;

            if (obj->value[0] == fac->vnum)
            val /= 5;
            if (obj->pIndexData->vnum == 36 && obj->size == 50)
            val = 0;

            sprintf(buf, "%s contributing supernatural material.", ch->name);
            if (val < 500) {
              ch->pcdata->week_tracker[TRACK_CONTRIBUTED] += val * 10;
              ch->pcdata->life_tracker[TRACK_CONTRIBUTED] += val * 10;
            }
            gain_resources(val, fac->vnum, ch, buf);
            send_to_char("Done.\n\r", ch);
            extract_obj(obj);
            return;
          }
          if (obj->pIndexData->vnum == 38) {
            int val = obj->level;
            // 1000 = 5% = 500
            //                val /= 2;

            give_intel(ch, val * 5);
            sprintf(buf, "%s contributes alchemical material worth $%d.", ch->name, val * 10);
            ch->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED] += val * 10;
            ch->pcdata->life_tracker[TRACK_ALCH_CONTRIBUTED] += val * 10;

            send_brief_log(fac->vnum, buf);
            fac->alchemy += val * char_secrecy(ch, NULL) / 1000;
            send_to_char("Done.\n\r", ch);
            extract_obj(obj);
            return;
          }
        }
      }
    }
    send_to_char("Syntax: contribute (influence/money/(object)) (faction)\n\r", ch);
  }

  void coverup_cost(CHAR_DATA *ch, int level) {
    int cost = 0;
    char buf[MSL];
    if (ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
      if (level == 1)
      cost = 100;
      if (level == 2)
      cost = 200;
      if (level == 3)
      cost = 1000;

      if (clan_lookup(ch->faction)->attributes[FACTION_CORPSE] > 0)
      cost /= 4;

      cost *= 2;

      use_resources(cost, ch->faction, NULL, "");
    }
    else {
      if (level == 1)
      cost = 5000;
      if (level == 2)
      cost = 10000;
      if (level == 3)
      cost = 100000;

      if (ch->pcdata->total_money + ch->money > cost) {
        ch->money -= cost;
        printf_to_char(ch, "You spend $%d to clean up the evidence.\n\r", cost / 100);
      }
      else {
        if (number_percent() % 2 == 0) {
          sprintf(buf, "%s was seen with a body.", ch->name);
        }
        else
        sprintf(buf, "A body was spotted near %s.", ch->in_room->name);

        gossip(buf);
      }
    }
  }

  int clan_position(CHAR_DATA *ch, int faction) {
    FACTION_TYPE *fac = clan_lookup(faction);
    int i;
    if (fac == NULL || fac == nullfac) {
      return 0;
    }

    for (i = 0; i < 100; i++) {
      if (!str_cmp(fac->member_names[i], ch->name)) {
        return fac->member_position[i] + 1;
      }
    }
    return 0;
  }

  int total_covers(FACTION_TYPE *fac) {
    int i;
    if (fac == NULL || fac == nullfac) {
      return 0;
    }
    int count = 0;

    for (i = 0; i < 100; i++) {
      if (safe_strlen(fac->member_names[i]) > 2 && fac->member_pay[i] > 0)
      count++;
    }
    return count;
  }
  int max_covers(FACTION_TYPE *fac) {
    PROP_TYPE *prop = get_base(fac->vnum);
    if (prop == NULL)
    return 0;

    if (prop->type == PROP_SHOP)
    return industry_table[prop->industry].covers;
    else
    return industry_table[INDUSTRY_RESIDENCE].covers;
  }

  int member_count(FACTION_TYPE *fac) {
    int count = 0;
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(fac->member_names[i]) > 1)
      count++;
    }
    return count;
  }

  bool generic_faction_vnum(int number) {
    if (number == FACTION_HAND)
    return TRUE;
    if (number == FACTION_ORDER)
    return TRUE;
    if (number == FACTION_TEMPLE)
    return TRUE;
    //    if(number == 11)
    //         return TRUE;
    //    if(number == 12)
    //    return TRUE;
    //    if(number == 13)
    //    return TRUE;

    return FALSE;
  }
  bool protected_faction_vnum(int number) {
    if (number == 43)
    return TRUE;

    return FALSE;
  }

  void switch_active_faction(CHAR_DATA *ch) {
    if (ch->factiontwo == 0)
    return;

    if (clan_lookup(ch->factiontwo) == NULL)
    return;

    if (ch->factiontrue > -1)
    return;

    if (ch->pcdata->order_type == ORDER_ATTACK && ch->pcdata->order_timer > 0)
    return;
    if (ch->pcdata->order_type == ORDER_GUARD && ch->pcdata->order_timer > 0)
    return;

    if (in_fight(ch) && battleground(ch->in_room))
    return;

    int tempfac = ch->factiontwo;
    ch->factiontwo = ch->faction;
    ch->faction = tempfac;
  }

  void patrol_check(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {}
  void breach_update(CHAR_DATA *ch) {}
  _DOFUN(do_areaname) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Areaname (Name of the room in which the operation will take place)\n\r", ch);

    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }
  _DOFUN(do_adversaryname) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_long);
        ch->pcdata->ci_long = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Adversaryname (The text string the adversaries in the operation will use)\n\r", ch);

    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }
  _DOFUN(do_adversarytype) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "modern")) {
        ch->pcdata->ci_mod = 0;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "archaic")) {
        ch->pcdata->ci_mod = 1;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Possible types are Modern/Archaic.\n\r", ch);
    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }
  _DOFUN(do_maximumcharacters) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      int num = atoi(argument);
      if (num < 2 || num > 8) {
        send_to_char("Syntax: Maximumcharacters (2-8)\n\r", ch);
        return;
      }
      ch->pcdata->ci_covers = num;
      send_to_char("Done.\n\r", ch);
    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }
  _DOFUN(do_hour) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      int num = atoi(argument);
      if (num < 1 || num > 23) {
        send_to_char("Syntax: Hour (1-23)\n\r", ch);
        return;
      }
      if (num == 7) {
        send_to_char("You cannot choose 7AM.", ch);
        return;
      }

      ch->pcdata->ci_alcohol = num;
      send_to_char("Done.\n\r", ch);
    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }

  _DOFUN(do_terrain) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "forest")) {
        ch->pcdata->ci_cost = BATTLE_FOREST;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "field")) {
        ch->pcdata->ci_cost = BATTLE_FIELD;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "desert")) {
        ch->pcdata->ci_cost = BATTLE_DESERT;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "town")) {
        ch->pcdata->ci_cost = BATTLE_TOWN;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "city")) {
        ch->pcdata->ci_cost = BATTLE_CITY;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "mountains")) {
        ch->pcdata->ci_cost = BATTLE_MOUNTAINS;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "warehouse")) {
        ch->pcdata->ci_cost = BATTLE_WAREHOUSE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "caves")) {
        ch->pcdata->ci_cost = BATTLE_CAVE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "village")) {
        ch->pcdata->ci_cost = BATTLE_VILLAGE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "tundra")) {
        ch->pcdata->ci_cost = BATTLE_TUNDRA;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "lake")) {
        ch->pcdata->ci_cost = BATTLE_LAKE;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Possible types are Forest/Field/Desert/Town/City/Mountains/Warehouse/Caves/Village/Tundra/Lake.\n\r", ch);
    }
    else
    send_to_char("You'd not editing anything.\n\r", ch);
  }

  int max_operation_size(int speed) {
    if (speed == 2)
    return 800;
    else if (speed == 3)
    return 700;
    else if (speed == 4)
    return 600;
    else if (speed == 5)
    return 500;
    else if (speed == 6)
    return 400;

    return 1000;
  }

  char *op_goal(OPERATION_TYPE *op) {
    char buf[MSL];
    if (op->type == OPERATION_INTERCEPT) {
      sprintf(buf, "Intercept %s and bring it back to the extraction point.", op->bag_name);
      return str_dup(buf);
    }
    else if (op->type == OPERATION_EXTRACT) {
      sprintf(
      buf, "Extract %s from the combat area and bring it to the extract point.", op->bag_name);
      return str_dup(buf);
    }
    else if (op->type == OPERATION_CAPTURE) {
      sprintf(buf, "Get to the designated site and continue %s until the process is complete.", op->upload_name);
      return str_dup(buf);
    }
    else if (op->type == OPERATION_MULTIPLE) {
      sprintf(buf, "Get to one of the three designated sites and continue %s until the process is complete.", op->upload_name);
      return str_dup(buf);
    }
    return "Something";
  }

  const char *comp_types[3] = {"Open", "Restricted", "Closed"};

  const char *visible_goal(int goal) {
    if (goal == GOAL_RESCUE)
    return visible_goal(GOAL_CONTROL);
    return goal_names[goal];
  }

  void show_operation_to_char(CHAR_DATA *ch, OPERATION_TYPE *op) {
    time_t storytime;
    storytime = current_time - get_minute() * 60 - get_hour(NULL) * 3600 +
    op->hour * 3600 + op->day * 3600 * 24 + ch->pcdata->jetlag * 3600;
    if (get_hour(NULL) >= op->hour)
    storytime = storytime + 24 * 3600;

    bool signup = FALSE;

    for (int i = 0; i < 100; i++) {
      if (!str_cmp(ch->name, op->sign_up[i])) {
        signup = TRUE;
      }
    }

    if (signup == TRUE)
    printf_to_char(ch, "`220Signed up.`x\n\r");

    if (op->goal == GOAL_PSYCHIC) {
      printf_to_char(ch, "Faction: %s\n\r", clan_lookup(op->faction)->name);
      printf_to_char(ch, "Terrain: %s\n\r", terrain_names[op->terrain]);
      printf_to_char(ch, "Task: Psychic\n\r");
      printf_to_char(ch, "Target: %s\n\r", op->target);
      printf_to_char(ch, "Leaving at %d hundred hours in %d days.\n\r", op->hour, op->day);
      printf_to_char(ch, "Maximum Deployable Supernaturals: %d \t\t Maximum Troop Support: %d\n\r", op->max_pcs, op->max_pcs * 2);
      printf_to_char(ch, "Competition: %s \t\tChallenge: %d\n\r", comp_types[op->competition], op->challenge);
      printf_to_char(ch, "Battlefield Size: %d \t\t Battle Speed: %d\n\r", op->size, op->speed);
      if (safe_strlen(op->preferred) > 2 && is_name(ch->name, op->preferred))
      send_to_char("You will be given priority deployment.\n\r", ch);
      return;
    }

    printf_to_char(ch, "Faction: %s\n\r", clan_lookup(op->faction)->name);
    printf_to_char(ch, "Operation Description: %s\n\n\r", op->description);
    printf_to_char(ch, "Territory: %s\n\r", operation_location(op));
    printf_to_char(ch, "Terrain: %s\n\r", terrain_names[op->terrain]);
    if (is_name(ch->name, op->storyrunners) || is_name(nosr_name(ch->name), op->storyrunners))
    printf_to_char(ch, "You can storyrun this operation.\n\r");
    else if (safe_strlen(op->storyrunners) > 3)
    printf_to_char(ch, "Storyrunning: Enabled.\n\r");
    printf_to_char(ch, "Task: %s\n\r", op_goal(op));
    printf_to_char(ch, "Goal: %s\n\r", visible_goal(op->goal));
    printf_to_char(ch, "Leaving at %d hundred hours in %d days.\n\r", op->hour, op->day);
    printf_to_char(ch, "Your time: %s", ctime(&storytime));
    printf_to_char(
    ch, "Maximum Deployable Supernaturals: %d \t\t Maximum Troop Support: %d\n\r", op->max_pcs, op->max_pcs * 2);
    printf_to_char(ch, "Competition: %s \t\tChallenge: %d\n\r", comp_types[op->competition], op->challenge);
    printf_to_char(ch, "Battlefield Size: %d \t\t Battle Speed: %d\n\r", op->size, op->speed);
    if (safe_strlen(op->preferred) > 2 && is_name(ch->name, op->preferred))
    send_to_char("You will be given priority deployment.\n\r", ch);
    for (int i = 0; i < 10; i++) {
      if (op->enrolled[i] > 0 && clan_lookup(op->enrolled[i]) != NULL && (op->enrolled[i] == ch->faction || op->enrolled[i] == ch->factiontwo)) {
        printf_to_char(ch, "\n%s is set to deploy to this operation with %d soldiers in support.\n\r", clan_lookup(op->enrolled[i])->name, op->soldiers[i]);
      }
    }
    if (ch->faction == op->faction || ch->factiontwo == op->faction)
    printf_to_char(ch, "\n%d soldiers will be present in support.\n\n\r", op->home_soldiers);

    if (op->territoryvnum >= 0) {
      int displayhour =
      op->hour + territory_by_number(op->territoryvnum)->timezone;
      if (displayhour >= 24)
      displayhour -= 24;
      printf_to_char(
      ch, "\nLocal time will be %d hundred hours.\n%s\n\r", displayhour, weather_forecast(territory_by_number(op->territoryvnum), op->hour));
    }
    else {
    }
  }

  _DOFUN(do_operation) {
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    bool signup = FALSE;
    if (!str_cmp(arg, "list")) {
      send_to_char("`r  Operations`x\n\r", ch);
      int count = 1;
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->territoryvnum == 0)
        (*it)->hour = 0;

        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        signup = FALSE;

        for (int i = 0; i < 100; i++) {
          if (!str_cmp(ch->name, (*it)->sign_up[i])) {
            signup = TRUE;
          }
        }

        if (signup == TRUE)
        printf_to_char(
        ch, "[%02d] `220%s's operation in %s at %d:00 in %d days`x.\n\r", count, clan_lookup((*it)->faction)->name, operation_location(*it), (*it)->hour, (*it)->day);
        else
        printf_to_char(ch, "[%02d] %s's operation in %s at %d:00 in %d days.\n\r", count, clan_lookup((*it)->faction)->name, operation_location(*it), (*it)->hour, (*it)->day);

        count++;
      }
    }
    else if (!str_cmp(arg, "reducedays") && IS_IMMORTAL(ch)) {
      int count = 1;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val)
        (*it)->day--;

        count++;
      }
    }
    else if (!str_cmp(arg, "speedup") && IS_IMMORTAL(ch)) {
      int count = 1;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val)
        (*it)->speed--;

        count++;
      }
    }
    else if (!str_cmp(arg, "changeadversary")) {
      if (!battleground(ch->in_room) || !is_gm(ch) || activeoperation == NULL) {
        send_to_char("You aren't SRing any operation.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 3) {
        send_to_char("Syntax: Operation changeadversary (new name)\n\r", ch);
        return;
      }
      free_string(activeoperation->adversary_name);
      activeoperation->adversary_name = str_dup(argument);
      printf_to_char(ch, "Adversaries changed to: %s\n\r", argument);
      return;
    }
    else if (!str_cmp(arg, "spawnelite")) {
      if (!battleground(ch->in_room) || !is_gm(ch) || activeoperation == NULL) {
        send_to_char("You aren't SRing any operation.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 3) {
        send_to_char("Syntax: Operation spawnelite (name)\n\r", ch);
        return;
      }
      free_string(activeoperation->elitestring);
      activeoperation->elitestring = str_dup(argument);
      printf_to_char(ch, "Elite %s set to spawn.\n\r", argument);
      return;
    }
    else if (!str_cmp(arg, "atmosphere")) {
      if (!battleground(ch->in_room) || !is_gm(ch) || activeoperation == NULL) {
        send_to_char("You aren't SRing any operation.\n\r", ch);
        return;
      }
      free_string(activeoperation->atmosphere);
      activeoperation->atmosphere = str_dup(argument);
      printf_to_char(ch, "Atmosphere set to %s\n\r", argument);
      return;
    }
    else if (!str_cmp(arg, "info")) {
      int count = 1;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val)
        show_operation_to_char(ch, (*it));

        count++;
      }
    }
    else if (!str_cmp(arg, "timeshift")) {
      int count = 1;
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if (str_cmp((*it)->target, ch->name) && (str_cmp((*it)->author, ch->name) || (*it)->day < 2)) {
            send_to_char("That operation isn't targeting you.\n\r", ch);
            return;
          }
          if ((*it)->timeshifted == 1) {
            send_to_char("You've already done that.\n\r", ch);
            return;
          }
          int hours = atoi(argument);
          if (hours > 23 || hours < -23) {
            send_to_char("Syntax operation timeshift (operation) (-23 to 23).\n\r", ch);
            return;
          }
          int newdays = (*it)->day;
          int newhour = (*it)->hour;
          newhour += hours;
          if (newhour > 24) {
            newhour -= 24;
            newdays++;
          }
          if (newhour < 0) {
            newhour += 24;
            newdays--;
          }
          if (newhour < 1 || newhour > 23 || newhour == 7) {
            send_to_char("That's an illegal hour for an operation.\n\r", ch);
            return;
          }
          if (newdays < 1) {
            send_to_char("That would be leaving too soon.\n\r", ch);
            return;
          }
          for (vector<OPERATION_TYPE *>::iterator ik = OpVect.begin();
          ik != OpVect.end(); ++ik) {
            if ((*ik)->hour == 0)
            continue;

            if ((*ik)->hour == (*it)->hour && (*ik)->day == (*it)->day)
            continue;
            if ((*ik)->hour == newhour && (*ik)->day == newdays) {
              send_to_char("There's already an operation set to depart then.\n\r", ch);
              return;
            }
          }
          (*it)->hour = newhour;
          (*it)->day = newdays;
          (*it)->timeshifted = 1;
          send_to_char("Done.\n\r", ch);
          return;
        }
        count++;
      }
    }
    else if (!str_cmp(arg, "bribe")) {
      if (activeoperation != NULL && isactiveoperation == TRUE) {
        send_to_char("You must wait for the current operation to conclude.\n\r", ch);
        return;
      }
      int count = 0;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        count++;
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if ((*it)->competition == COMPETE_CLOSED) {
            send_to_char("You can't do that on a closed operation.\n\r", ch);
            return;
          }
          if (has_trust(ch, TRUST_WAR, (*it)->faction) || IS_IMMORTAL(ch)) {
            int cost = 5000;
            if (clan_lookup((*it)->faction)->axes[AXES_COMBAT] == AXES_FARLEFT)
            cost = cost * 150 / 100;
            else if (clan_lookup((*it)->faction)->axes[AXES_COMBAT] ==
                AXES_MIDLEFT)
            cost = cost * 130 / 100;
            else if (clan_lookup((*it)->faction)->axes[AXES_COMBAT] ==
                AXES_NEARLEFT)
            cost = cost * 110 / 100;
            else if (clan_lookup((*it)->faction)->axes[AXES_COMBAT] ==
                AXES_FARRIGHT)
            cost = cost * 50 / 100;
            else if (clan_lookup((*it)->faction)->axes[AXES_COMBAT] ==
                AXES_MIDRIGHT)
            cost = cost * 70 / 100;
            else if (clan_lookup((*it)->faction)->axes[AXES_COMBAT] ==
                AXES_NEARRIGHT)
            cost = cost * 90 / 100;
            if (clan_lookup((*it)->faction)->resource < 10000 - cost) {
              send_to_char("You don't have enough resources for that.\n\r", ch);
              return;
            }
            clan_lookup((*it)->faction)->manpower += (*it)->home_soldiers;
            for (int i = 0; i < 10; i++) {
              if (clan_lookup((*it)->enrolled[i]) != NULL)
              clan_lookup((*it)->enrolled[i])->manpower += (*it)->soldiers[i];
            }

            use_resources(cost, (*it)->faction, NULL, "bribing an operation win.");
            win_operation((*it)->faction, (*it));
            return;
          }
          else {
            send_to_char("You're not trusted to do that.\n\r", ch);
            return;
          }
        }
      }
    }
    else if (!str_cmp(arg, "cancel")) {
      int count = 1;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if ((*it)->goal == GOAL_FOOTHOLD || (*it)->goal == GOAL_UPROOT || (*it)->goal == GOAL_PSYCHIC) {
            send_to_char("That cannot be canceled.\n\r", ch);
            return;
          }

          if ((has_trust(ch, TRUST_WAR, (*it)->faction) && !generic_faction_vnum((*it)->faction)) || IS_IMMORTAL(ch) || !str_cmp(ch->name, (*it)->author)) {
            (*it)->hour = 0;
            send_to_char("Operation scrubbed.\n\r", ch);
            clan_lookup((*it)->faction)->manpower += (*it)->home_soldiers;
            for (int i = 0; i < 10; i++) {
              if (clan_lookup((*it)->enrolled[i]) != NULL)
              clan_lookup((*it)->enrolled[i])->manpower += (*it)->soldiers[i];
            }

            return;
          }
          else
          send_to_char("You lack the authority to do that.\n\r", ch);
        }
        count++;
      }
      send_to_char("No such operation.\n\r", ch);
    }
    else if (!str_cmp(arg, "signup")) {
      if (IS_FLAG(ch->act, PLR_STASIS))
      return;
      if (is_gm(ch) || higher_power(ch))
      return;

      int count = 1;
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {

          if (activeoperation == (*it))
          return;

          for (int i = 0; i < 100; i++) {
            if (!str_cmp(ch->name, (*it)->sign_up[i])) {
              send_to_char("Unenrolled.\n\r", ch);
              free_string((*it)->sign_up[i]);
              (*it)->sign_up[i] = str_dup("");
              return;
            }
          }
          if ((*it)->goal == GOAL_PSYCHIC) {
            for (int i = 0; i < 100; i++) {
              if (safe_strlen((*it)->sign_up[i]) < 1) {
                if (!str_cmp(argument, "defend")) {
                  free_string((*it)->sign_up[i]);
                  (*it)->sign_up[i] = str_dup(ch->name);
                  (*it)->sign_up_type[i] = 1;
                }
                else if (!str_cmp(argument, "attack")) {
                  free_string((*it)->sign_up[i]);
                  (*it)->sign_up[i] = str_dup(ch->name);
                  (*it)->sign_up_type[i] = 2;
                }
                else {
                  send_to_char("Syntax: operation signup (number) (attack/defend)\n\r", ch);
                  return;
                }
                send_to_char("You sign up for the operation.\n\r", ch);
                return;
              }
            }
            return;
          }
          if (cardinal(ch))
          return;
          if (!generic_faction_vnum(ch->faction)) {
            if (ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->alliance == 0) {
              send_to_char("Your society isn't in an alliance.\n\r", ch);
              return;
            }
          }
          if (ch->faction == 0 || clan_lookup(ch->faction) == NULL) {
            send_to_char("You're not in a society.\n\r", ch);
            return;
          }

          for (int i = 0; i < 100; i++) {
            if (!str_cmp(ch->name, (*it)->sign_up[i])) {
              send_to_char("Unenrolled.\n\r", ch);
              free_string((*it)->sign_up[i]);
              (*it)->sign_up[i] = str_dup("");
              return;
            }
          }
          for (int i = 0; i < 100; i++) {
            if (safe_strlen((*it)->sign_up[i]) < 1) {
              free_string((*it)->sign_up[i]);
              (*it)->sign_up[i] = str_dup(ch->name);
              send_to_char("You sign up for the operation.\n\r", ch);
              return;
            }
          }
        }

        count++;
      }
    }
    else if (!str_cmp(arg, "launch")) {
      int count = 1;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if (IS_IMMORTAL(ch)) {
            launch_operation((*it));
            return;
          }
          else
          send_to_char("You lack the authority to do that.\n\r", ch);
        }
        count++;
      }
      send_to_char("No such operation.\n\r", ch);
    }
    else if (!str_cmp(arg, "launchfast")) {
      int count = 1;
      int val = atoi(argument);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if (IS_IMMORTAL(ch)) {
            (*it)->speed = 1;
            launch_operation((*it));
            return;
          }
          else
          send_to_char("You lack the authority to do that.\n\r", ch);
        }
        count++;
      }
      send_to_char("No such operation.\n\r", ch);
    }
    else if (!str_cmp(arg, "withdraw")) {
      int count = 1;
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if ((*it)->goal == GOAL_PSYCHIC) {
            send_to_char("You can't reinforce that type of operation.\n\r", ch);
            return;
          }
          if (ch->faction == (*it)->faction && has_trust(ch, TRUST_WAR, (*it)->faction)) {
            int amount = atoi(argument);
            if (amount > (*it)->home_soldiers || amount <= 0)
            amount = (*it)->home_soldiers;
            (*it)->home_soldiers -= amount;
            clan_lookup(ch->faction)->manpower += amount;
            printf_to_char(ch, "You will deploy with %d soldiers in support.\n\r", (*it)->home_soldiers);
            return;
          }
          for (int i = 0; i < 10; i++) {
            if (ch->faction == (*it)->enrolled[i] && has_trust(ch, TRUST_WAR, ch->faction)) {
              int amount = atoi(argument);
              if (amount > (*it)->home_soldiers || amount <= 0)
              amount = (*it)->soldiers[i];
              (*it)->soldiers[i] -= amount;
              clan_lookup(ch->faction)->manpower += amount;
              printf_to_char(ch, "You will deploy with %d soldiers in support.\n\r", (*it)->soldiers[i]);
              return;
            }
          }
        }
        count++;
      }
    }

    else if (!str_cmp(arg, "reinforce")) {
      int count = 1;
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->faction != ch->faction && (*it)->faction != ch->factiontwo && (*it)->competition == COMPETE_CLOSED)
        continue;

        if (count == val) {
          if ((*it)->goal == GOAL_PSYCHIC) {
            send_to_char("You can't reinforce that type of operation.\n\r", ch);
            return;
          }
          if (ch->faction == (*it)->faction && has_trust(ch, TRUST_WAR, (*it)->faction)) {
            int amount = atoi(argument);
            int max = UMIN(10, (*it)->max_pcs * 2);
            max -= border_count(clan_lookup(ch->faction)) * 3;
            if (amount > clan_lookup(ch->faction)->manpower || amount < 1) {
              send_to_char("You don't have that many soldiers to send.\n\r", ch);
              return;
            }
            if (amount + (*it)->home_soldiers > max) {
              send_to_char("You can't muster that many reinforcements for that operation.\n\r", ch);
              return;
            }
            (*it)->home_soldiers += amount;
            clan_lookup(ch->faction)->manpower -= amount;
            printf_to_char(ch, "You will deploy with %d soldiers in support.\n\r", (*it)->home_soldiers);
            return;
          }
          if (!join_to_operation(ch->faction, (*it))) {
            send_to_char("You can't attend that operation.\n\r", ch);
            return;
          }
          for (int i = 0; i < 10; i++) {
            if (ch->faction == (*it)->enrolled[i] && has_trust(ch, TRUST_WAR, ch->faction)) {
              int amount = atoi(argument);
              int max = UMIN(10, (*it)->max_pcs * 2);
              max -= border_count(clan_lookup(ch->faction)) * 3;
              if (amount > clan_lookup(ch->faction)->manpower || amount < 1) {
                send_to_char("You don't have that many soldiers to send.\n\r", ch);
                return;
              }
              if (amount + (*it)->soldiers[i] > max) {
                send_to_char("You can't muster that many reinforcements for that operation.\n\r", ch);
                return;
              }
              (*it)->soldiers[i] += amount;
              clan_lookup(ch->faction)->manpower -= amount;
              printf_to_char(ch, "You will deploy with %d soldiers in support.\n\r", (*it)->soldiers[i]);
              return;
            }
          }
        }
        count++;
      }
      send_to_char("No such operation.\n\r", ch);
    }

    else
    send_to_char("Syntax: Operation list/info/cancel/signup/reinforce/withdraw (number)\n\r", ch);
  }

  void battle_message(char *message, int battleground_number) {

    CHAR_DATA *newvict;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;
      if (newvict->in_room == NULL || !battleground(newvict->in_room) || bg_number(newvict->in_room) != battleground_number)
      continue;
      if (IS_NPC(newvict))
      continue;
      send_to_char(message, newvict);
    }
  }

  CHAR_DATA *get_carrier(int battleground_number) {
    CHAR_DATA *newvict;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;
      if (newvict->in_room == NULL || !battleground(newvict->in_room) || bg_number(newvict->in_room) != battleground_number)
      continue;
      if (newvict->bagcarrier == 1)
      return newvict;
    }
    return NULL;
  }

  int battle_pc_pop(int number) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != number)
      continue;
      if (!IS_NPC(victim))
      count++;
    }
    return count;
  }

  int battle_defend_pop(int number) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != number)
      continue;
      if (victim->faction == 200000)
      count++;
    }
    return count;
  }

  int battle_attack_pop(int number) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (victim == NULL || is_gm(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != number)
      continue;
      if (victim->faction == 300000)
      count++;
    }
    return count;
  }

  int op_cover_count() {
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;
      if (IS_NPC(rch) && rch->pIndexData->vnum == 110)
      count++;
    }
    return count;
  }
  int adver_count() {
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      if (!IS_NPC((*it)))
      continue;

      if ((*it)->pIndexData->vnum == 115)
      count++;
    }
    return count;
  }

  void operations_update() {
    int maxn = 12 - adver_count();
    if (isactiveoperation == FALSE)
    return;

    OPERATION_TYPE *op = activeoperation;
    int battleground_number = op->battleground_number;
    if (battle_pc_pop(battleground_number) < 1) {
      if(check_antag_win(battleground_number) == FALSE)
      {
        lose_operation();
      }
      end_battle();
      return;
    }

    if (op->goal == GOAL_PSYCHIC) {
      if (battle_defend_pop(battleground_number) < 1) {
        win_operation(300000, NULL);
        return;
      }
      if (battle_attack_pop(battleground_number) < 1) {
        lose_operation();
        end_battle();
        return;
      }

      op->timer--;
      char buf[MSL];
      if (op->timer > 0 && op->timer % 3 == 0) {
        sprintf(buf, "`c%d minutes until the assault fails.`x\n\r", op->timer);
        battle_message(buf, op->battleground_number);
      }
      if (op->timer <= 0) {
        lose_operation();
        end_battle();
        return;
      }
    }

    if (op->type == OPERATION_INTERCEPT || op->type == OPERATION_EXTRACT) {
      op->timer--;
      if (op->timer % op->waves == 0) {
        op->power = op->power * 13 / 10;
        int max_power = op->power;
        if (op_cover_count() < 1) {
          max_power /= 3;
          maxn /= 3;
        }
        if (pc_op_count() <= 1) {
          max_power /= 3;
          maxn /= 3;
        }

        int advalue = ADVERSARY_VALUE;
        int battle_total = 0;
        int battlesize = op->size;
        max_power = UMAX(advalue + 1, max_power);
        for (int i = 0; i < 6; i++) {
          if (battle_factions[i] > 0)
          battle_total++;
        }
        battle_total++;
        if (advercount(battleground_number) < 25) {
          for (int i = 0; i < 6; i++) {
            if (battle_factions[i] == 0 && max_power > advalue) {
              if (max_power / advalue >= 4 && safe_strlen(op->elitestring) > 3) {
                make_elite(battle_total, i, battlesize, op->adversary_type, op->elitestring, battleground_number, (max_power / advalue) / 2);
                for (int j = 0; max_power > advalue && j < maxn; j++) {
                  make_adversary(battle_total, i, battlesize, op->adversary_type, op->adversary_name, battleground_number);
                  max_power -= advalue * 2;
                }
                free_string(op->elitestring);
                op->elitestring = str_dup("");
              }
              else {
                for (int j = 0; max_power > advalue && j < maxn; j++) {
                  make_adversary(battle_total, i, battlesize, op->adversary_type, op->adversary_name, battleground_number);
                  max_power -= advalue;
                }
              }
            }
          }
        }
      }
      char buf[MSL];
      if (op->timer > 0 && op->timer % 3 == 0) {
        sprintf(buf, "`c%d minutes until extraction.`x\n\r", op->timer);
        battle_message(buf, op->battleground_number);
      }
      else if (op->timer == 0) {
        battle_message("`CExtraction is now possible.`x\n\r", op->battleground_number);
      }
      else if (op->timer <= 0) {
        CHAR_DATA *carrier = get_carrier(op->battleground_number);
        if (carrier != NULL) {
          for (int i = 0; i < 10; i++) {
            if (poidistance(carrier, op->poix[i], op->poiy[i]) <= 5 && op->poifaction[i] == carrier->faction && op->poitype[i] == POI_EXTRACT) {
              win_operation(carrier->faction, NULL);
              end_battle();
            }
            if (poidistance(carrier, op->poix[i], op->poiy[i]) <= 5 && op->poifaction[i] == 0 && op->poitype[i] == POI_EXTRACT) {
              win_operation(carrier->faction, NULL);
              end_battle();
            }
          }
        }
      }
    }
    if (op->type == OPERATION_CAPTURE || op->type == OPERATION_MULTIPLE) {

      op->timer--;
      if (op->timer % op->waves == 0) {
        op->power = op->power * 14 / 10;
        int max_power = op->power;
        if (op_cover_count() < 1) {
          max_power /= 3;
          maxn /= 3;
        }
        if (pc_op_count() <= 1) {
          max_power /= 3;
          maxn /= 3;
        }
        int advalue = ADVERSARY_VALUE;
        int battle_total = 0;
        int battlesize = op->size;
        max_power = UMAX(advalue + 1, max_power);
        for (int i = 0; i < 6; i++) {
          if (battle_factions[i] > 0)
          battle_total++;
        }
        battle_total++;
        if (advercount(battleground_number) < 25) {
          for (int i = 0; i < 6; i++) {
            if (battle_factions[i] == 0 && max_power > advalue) {
              if (max_power / advalue >= 4 && safe_strlen(op->elitestring) > 3) {
                make_elite(battle_total, i, battlesize, op->adversary_type, op->elitestring, battleground_number, max_power / advalue);
                for (int i = 0; max_power > advalue && i < maxn; i++) {
                  make_adversary(battle_total, i, battlesize, op->adversary_type, op->adversary_name, battleground_number);
                  max_power -= advalue * 2;
                }
                free_string(op->elitestring);
                op->elitestring = str_dup("");
              }
              else {
                for (int i = 0; max_power > advalue && i < maxn; i++) {
                  make_adversary(battle_total, i, battlesize, op->adversary_type, op->adversary_name, battleground_number);
                  max_power -= advalue;
                }
              }
            }
          }
        }
      }
      char buf[MSL];
      if (op->timer > 0 && op->timer % 3 == 0) {
        int attempts = 0;
        for (int i = 0; i < 6; i++) {
          if (battle_factions[i] > 0) {
            for (int j = 0; j < 10; j++) {
              if (op->enrolled[j] == battle_factions[i])
              attempts = op->uploads[j];
            }
            if (op->faction == battle_factions[i])
            attempts = op->home_uploads;

            sprintf(buf, "`cYou are %d percent done %s.`x", attempts * 100 / op->upload, op->upload_name);
            send_message_temp(battle_factions[i], buf);
          }
        }
      }
    }
  }

  /*
bool poi(CHAR_DATA *ch, int x, int y)
{
if(ch->in_room == NULL || !battleground(ch->in_room))
return FALSE;

if(isactiveoperation == FALSE)
return FALSE;

OPERATION_TYPE *op = activeoperation;
for(int i=0;i<10;i++)
{
if(op->poifaction[i] == 0 || op->poifaction[i] == ch->faction)
{
int xval = 39*(op->poix[i])/ch->in_room->size;
int yval = 21*(op->poiy[i])/ch->in_room->size;
if(x == xval && y == yval)
return TRUE;
}
}
return FALSE;
}


char * poichar(CHAR_DATA *ch, int x, int y)
{
OPERATION_TYPE *op = activeoperation;
for(int i=0;i<10;i++)
{
if(op->poifaction[i] == 0 || op->poifaction[i] == ch->faction)
{
int xval = 39*(op->poix[i])/ch->in_room->size;
int yval = 21*(op->poiy[i])/ch->in_room->size;
if(x == xval && y == yval)
{
if(op->poitype[i] == POI_EXTRACT)
return "`YE`x";
else if(op->poitype[i] == POI_CAPTURE)
return "`YC`x";
}
}
}
return " ";
}
*/

  int poidistance(CHAR_DATA *ch, int poix, int poiy) {
    if (ch == NULL || ch->in_room == NULL)
    return 0;
    int x = relative_x(
    ch, battleroom_bycoord(bg_number(ch->in_room), poix, poiy), poix % battleroom_bycoord(bg_number(ch->in_room), poix, poiy)->size);
    int y = relative_y(
    ch, battleroom_bycoord(bg_number(ch->in_room), poix, poiy), poiy % battleroom_bycoord(bg_number(ch->in_room), poix, poiy)->size);
    int dist = get_dist(0, 0, x, y);
    return dist;
  }

  void displaypois(CHAR_DATA *ch) {
    if (battleground(ch->in_room) && isactiveoperation) {
      for (int i = 0; i < 10; i++) {
        if (bg_number(ch->in_room) != activeoperation->poibg[i])
        continue;

        if (activeoperation->poitype[i] > 0 && (activeoperation->poifaction[i] == 0 || activeoperation->poifaction[i] == ch->faction)) {
          if (activeoperation->poitype[i] == POI_EXTRACT)
          printf_to_char(
          ch, "(`YE`x) Extraction Point (X:%d Y:%d D:%d)\n\r", relative_x(ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poix[i] %
          battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
          ->size), relative_y(ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poiy[i] %
          battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
          ->size), poidistance(ch, activeoperation->poix[i], activeoperation->poiy[i]));
          else if (activeoperation->poitype[i] == POI_CAPTURE)
          printf_to_char(
          ch, "(`YC`x) Capture Point (X:%d Y:%d D:%d)\n\r", relative_x(ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poix[i] %
          battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
          ->size), relative_y(ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poiy[i] %
          battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
          ->size), poidistance(ch, activeoperation->poix[i], activeoperation->poiy[i]));
        }
      }
    }
  }

  bool poidisplay(CHAR_DATA *ch, int size, int mapy, int mapx) {
    if (!isactiveoperation)
    return FALSE;
    if (activeoperation == NULL)
    return FALSE;
    int newx = mapx / 2;
    int newy = mapy;
    int offset = size - 1;
    offset /= 2;
    newx -= offset;
    newy -= offset;
    int xmax = map_expand(newx + 1);
    int xmin = map_expand(newx);
    int ymax = map_expand(newy + 1);
    int ymin = map_expand(newy);

    for (int i = 0; i < 10; i++) {
      if (bg_number(ch->in_room) != activeoperation->poibg[i])
      continue;

      int relx = relative_x(
      ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poix[i] % battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
      ->size);
      int rely = relative_y(
      ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poiy[i] % battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
      ->size);

      if (relx < xmax && relx >= xmin && rely < ymax && rely >= ymin) {
        return TRUE;
      }
    }
    return FALSE;
  }

  int poitype(CHAR_DATA *ch, int size, int mapy, int mapx) {
    if (!isactiveoperation)
    return FALSE;
    if (activeoperation == NULL)
    return FALSE;
    int newx = mapx / 2;
    int newy = mapy;
    int offset = size - 1;
    offset /= 2;
    newx -= offset;
    newy -= offset;
    int xmax = map_expand(newx + 1);
    int xmin = map_expand(newx);
    int ymax = map_expand(newy + 1);
    int ymin = map_expand(newy);

    for (int i = 0; i < 10; i++) {
      if (bg_number(ch->in_room) != activeoperation->poibg[i])
      continue;

      int relx = relative_x(
      ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poix[i] % battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
      ->size);
      int rely = relative_y(
      ch, battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i]), activeoperation->poiy[i] % battleroom_bycoord(bg_number(ch->in_room), activeoperation->poix[i], activeoperation->poiy[i])
      ->size);

      if (relx < xmax && relx >= xmin && rely < ymax && rely >= ymin) {
        return activeoperation->poitype[i];
      }
    }
    return -1;
  }

  int get_poix(CHAR_DATA *ch, int type) {
    if (ch->in_room == NULL || !battleground(ch->in_room) || isactiveoperation == FALSE)
    return ch->x;

    OPERATION_TYPE *op = activeoperation;
    int mindist = 1000;
    int minx;
    for (int i = 0; i < 10; i++) {
      if (op->poifaction[i] == 0 || op->poifaction[i] == ch->faction) {
        if (op->poitype[i] == type) {
          if (poidistance(ch, op->poix[i], op->poiy[i]) < mindist) {
            mindist = poidistance(ch, op->poix[i], op->poiy[i]);
            minx = relative_x(ch, battleroom_bycoord(bg_number(ch->in_room), op->poix[i], op->poiy[i]), op->poix[i] %
            battleroom_bycoord(bg_number(ch->in_room), op->poix[i], op->poiy[i])
            ->size);
          }
        }
      }
    }
    return minx;
  }
  int get_poiy(CHAR_DATA *ch, int type) {
    if (ch->in_room == NULL || !battleground(ch->in_room) || isactiveoperation == FALSE)
    return ch->y;

    OPERATION_TYPE *op = activeoperation;
    int mindist = 1000;
    int miny;
    for (int i = 0; i < 10; i++) {
      if (op->poifaction[i] == 0 || op->poifaction[i] == ch->faction) {
        if (op->poitype[i] == type) {
          if (poidistance(ch, op->poix[i], op->poiy[i]) < mindist) {
            mindist = poidistance(ch, op->poix[i], op->poiy[i]);
            miny = relative_y(ch, battleroom_bycoord(bg_number(ch->in_room), op->poix[i], op->poiy[i]), op->poiy[i] %
            battleroom_bycoord(bg_number(ch->in_room), op->poix[i], op->poiy[i])
            ->size);
          }
        }
      }
    }
    return miny;
  }

  bool capture_attack(CHAR_DATA *ch) {
    if (ch->in_room == NULL || !battleground(ch->in_room))
    return FALSE;

    OPERATION_TYPE *op = activeoperation;
    if (op == NULL)
    return FALSE;

    if (ch->attack_timer > 0)
    return FALSE;

    if (is_animal(ch)) {
      send_to_char("You'd need opposable thumbs first.\n\r", ch);
      return FALSE;
    }

    for (int i = 0; i < 3; i++) {
      if (poidistance(ch, op->poix[i], op->poiy[i]) <= 5 && op->poitype[i] == POI_CAPTURE && ch->in_room->sector_type != SECT_AIR) {
        if (op->upload_cooldown[i] <= 0) {
          if (clan_lookup(ch->faction) != NULL && clan_lookup(op->faction) != NULL && clan_lookup(ch->faction)->alliance ==
              clan_lookup(op->faction)->alliance) {
            op->home_uploads++;
            if (op->home_uploads >= op->upload) {
              win_operation(ch->faction, NULL);
              end_battle();
              return TRUE;
            }
          }
          else {
            for (int j = 0; j < 10; j++) {
              if (clan_lookup(op->enrolled[j]) != NULL && clan_lookup(ch->faction) != NULL && clan_lookup(op->enrolled[j])->alliance ==
                  clan_lookup(ch->faction)->alliance) {
                op->uploads[j]++;
                if (op->uploads[j] >= op->upload) {

                  win_operation(ch->faction, NULL);
                  end_battle();
                  return TRUE;
                }
              }
            }
          }
          ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
          op->upload_cooldown[i] = FIGHT_WAIT * fight_speed(ch);
          char buf[MSL];
          sprintf(buf, "`c$n`c continues %s.`x", activeoperation->upload_name);
          act(buf, ch, NULL, NULL, TO_ROOM);
          op_report(logact(buf, ch, ch), ch);
          sprintf(buf, "`cYou continue %s.`x", activeoperation->upload_name);
          act(buf, ch, NULL, NULL, TO_CHAR);
          if (!IS_FLAG(ch->comm, COMM_AUTOFIRE)) {
            free_string(ch->amove);
            ch->amove = str_dup("");
          }
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  void operation_second_update() {
    if (isactiveoperation == FALSE)
    return;
    if (activeoperation == NULL)
    return;

    for (int i = 0; i < 3; i++) {
      if (activeoperation->upload_cooldown[i] > 0)
      activeoperation->upload_cooldown[i]--;
    }
  }

  _DOFUN(do_capture) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_target);
        ch->pcdata->ci_target = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Capture (The name of the capturing process)\n\r", ch);
      return;
    }
    do_function(ch, &do_attack, "capture");
  }

  _DOFUN(do_itemname) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_myself);
        ch->pcdata->ci_myself = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Itemname (The name of the item being contested)\n\r", ch);

    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }
  _DOFUN(do_priority) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_bystanders);
        ch->pcdata->ci_bystanders = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Priority (A list of players to be given priority)\n\r", ch);

    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }

  _DOFUN(do_challenge) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      int val = atoi(argument);
      if (val < 0 || val > 5) {
        send_to_char("Challenge 0-5\n\r", ch);
        return;
      }
      ch->pcdata->ci_disclevel = val;
      send_to_char("Done.\n\r", ch);
    }
    else
    send_to_char("You're not editing anything.\n\r", ch);
  }
  _DOFUN(do_competition) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "open")) {
        ch->pcdata->ci_discipline = COMPETE_OPEN;
        ch->pcdata->ci_vnum = UMAX(1, ch->pcdata->ci_vnum);
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "restricted")) {
        ch->pcdata->ci_discipline = COMPETE_RESTRICT;
        ch->pcdata->ci_vnum = UMAX(1, ch->pcdata->ci_vnum);
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "closed")) {
        ch->pcdata->ci_discipline = COMPETE_CLOSED;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Competition Open/Closed/Restricted.\n\r", ch);
    }
  }

  char *bag_name() {
    if (!isactiveoperation)
    return "something";

    return activeoperation->bag_name;
  }

  _DOFUN(do_handoff) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char buf[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (ch->attack_timer > 0) {
      send_to_char("You need to wait until you can attack to do that.\n\r", ch);
      return;
    }
    if (ch->bagcarrier == 0 || !isactiveoperation || ch->in_room == NULL || !battleground(ch->in_room)) {
      send_to_char("You have nothing to hand over.\n\r", ch);
      return;
    }
    if (combat_distance(ch, victim, TRUE) > 5) {
      send_to_char("They're too far away.\n\r", ch);
      return;
    }
    if (is_animal(victim) || victim->in_room->sector_type == SECT_AIR) {
      send_to_char("They can't carry that.\n\r", ch);
      return;
    }
    if (ch->faction != victim->faction)
    ch->move_timer += FIGHT_WAIT * 2 * fight_speed(ch);

    ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
    ch->bagcarrier = 0;
    victim->bagcarrier = 1;
    sprintf(buf, "$n hands %s off to $N", bag_name());
    act(buf, ch, NULL, victim, TO_NOTVICT);
    sprintf(buf, "You hand %s off to $N", bag_name());
    act(buf, ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n hands %s off to you.", bag_name());
    act(buf, ch, NULL, victim, TO_VICT);
  }

  char *operation_location(OPERATION_TYPE *op) {
    if (territory_by_number(op->territoryvnum) == NULL)
    return "Somewhere";
    else
    return territory_by_number(op->territoryvnum)->name;
  }

  int offworld_timezone(int number) {
    if (number < 0)
    number *= -1;

    if (number == WORLD_OTHER)
    return -2;
    else if (number == WORLD_GODREALM)
    return -7;
    else if (number == WORLD_WILDS)
    return 8;

    return 0;
  }

  _DOFUN(do_testxy) {
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    int number = atoi(arg);
    int size = atoi(argument);
    number++;

    for (int i = 0; i < number; i++)
    printf_to_char(ch, "Number %d, total %d, size %d, X: %d, Y: %d\n\r", i, number, size, startx(i + 1, number, size), starty(i + 1, number, size));
  }

  _DOFUN(do_resupply) {
    OBJ_DATA *obj;

    if (!goblin_market(ch->in_room)) {
      send_to_char("You need to be in the gobin market to do that.\n\r", ch);
      return;
    }
    if (is_helpless(ch) || in_fight(ch) || is_pinned(ch) || is_ghost(ch) || room_hostile(ch->in_room)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (!has_trust(ch, TRUST_RESOURCES, ch->faction)) {
      send_to_char("You don't have the authority to do that.\n\r", ch);
      return;
    }
    FACTION_TYPE *fac = clan_lookup(ch->faction);
    if (fac->resource < 5000) {
      send_to_char("You need to have an operating resource budget of at least $50 000 to do that.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "pepper spray")) {
      obj = create_object(get_obj_index(ITEM_PEPPER), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "bandage")) {
      obj = create_object(get_obj_index(ITEM_BANDAGE), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "compass")) {
      obj = create_object(get_obj_index(74), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "taser")) {
      obj = create_object(get_obj_index(ITEM_TASER), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "taserdart") || !str_cmp(argument, "taser dart")) {
      obj = create_object(get_obj_index(ITEM_TASERDART), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "tranquilizer gun")) {
      obj = create_object(get_obj_index(ITEM_TRANQGUN), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "tranquilizer dart")) {
      obj = create_object(get_obj_index(ITEM_TRANQDART), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "caltrops")) {
      obj = create_object(get_obj_index(ITEM_CALTROPS), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "landmine")) {
      obj = create_object(get_obj_index(ITEM_LANDMINE), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "bola")) {
      obj = create_object(get_obj_index(ITEM_BOLA), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "gasmask")) {
      obj = create_object(get_obj_index(ITEM_GASMASK), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "smoke grenade")) {
      obj = create_object(get_obj_index(ITEM_SMOKEGRENADE), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "tear gas")) {
      obj = create_object(get_obj_index(ITEM_TEARGRENADE), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "frag grenade")) {
      obj = create_object(get_obj_index(ITEM_FRAGGRENADE), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "naturalizer")) {
      obj = create_object(get_obj_index(ITEM_NATURALIZER), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "neutralizer grenade")) {
      obj = create_object(get_obj_index(ITEM_NEUTRALGRENADE), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "neutralizer collar")) {
      obj = create_object(get_obj_index(ITEM_NEUTRALCOLLAR), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "spy camera")) {
      obj = create_object(get_obj_index(46405), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "police scanner")) {
      obj = create_object(get_obj_index(45024), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");

      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(argument, "dream charm")) {
      obj = create_object(get_obj_index(69), 0);
      obj_to_char(obj, ch);
      use_resources(UMAX(1, obj->cost / 1000), fac->vnum, ch, "");
      send_to_char("Done.\n\r", ch);
    }
    else
    send_to_char("Syntax: Resupply pepper spray/bandage/taser/taserdart/transquilizer gun/tranquilizer dart/caltrops/landmine/bola/gasmask/smoke grenade/tear gas/frag grenade/naturalizer/neutralizer grenade/neutralizer collar/spy camera/police scanner/dream charm/compass.\n\r", ch);
  }

  _DOFUN(do_minioncommand) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];
    CHAR_DATA *guard;
    argument = one_argument_nouncap(argument, arg1);
    FACTION_TYPE *fac = clan_lookup(ch->faction);

    if (!str_cmp(arg1, "transfer")) {
      if (str_cmp(ch->name, fac->battle_leader)) {
        send_to_char("You do not have command.\n\r", ch);
        return;
      }
      if ((victim = get_char_fight(ch, argument)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      free_string(fac->battle_leader);
      fac->battle_leader = str_dup(victim->name);
      act("`c$n transfers battle leadership to you.`x", ch, NULL, victim, TO_VICT);
      act("`cYou transfer battle leadership to $N.`x", ch, NULL, victim, TO_CHAR);

      return;
    }

    if (!str_cmp(arg1, "faction")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "attack")) {
        if ((victim = get_char_fight(ch, argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        if (fac == NULL || (str_cmp(ch->name, fac->battle_leader) && safe_strlen(fac->battle_leader) > 2)) {
          send_to_char("You're not the operational leader.\n\r", ch);
          return;
        }
        free_string(fac->battle_target);
        fac->battle_target = str_dup(victim->name);
        fac->battle_order = BORDER_ATTACK;
        printf_to_char(ch, "You command the faction soldiers to attack %s.\n\r", PERS(victim, ch));
      }
      else if (!str_cmp(arg2, "follow")) {
        if ((victim = get_char_fight(ch, argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        if (fac == NULL || (str_cmp(ch->name, fac->battle_leader) && safe_strlen(fac->battle_leader) > 2)) {
          send_to_char("You're not the operational leader.\n\r", ch);
          return;
        }
        free_string(fac->battle_target);
        fac->battle_target = str_dup(victim->name);
        fac->battle_order = BORDER_FOLLOW;
        printf_to_char(ch, "You command the faction soldiers to follow %s.\n\r", PERS(victim, ch));
      }
      else if (!str_cmp(arg2, "atwill")) {
        if (fac == NULL || (str_cmp(ch->name, fac->battle_leader) && safe_strlen(fac->battle_leader) > 2)) {
          send_to_char("You're not the operational leader.\n\r", ch);
          return;
        }
        free_string(fac->battle_target);
        fac->battle_target = str_dup("");
        fac->battle_order = 0;
        printf_to_char(ch, "You stop commanding the faction soldiers.\n\r");

      }
      else
      send_to_char("Syntax: Command faction attack/follow/atwill (target)\n\r", ch);
    }
    else if (!str_cmp(arg1, "minions") || !str_cmp(arg1, "minion")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "attack")) {
        if ((victim = get_char_fight(ch, argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          guard = *it;
          if (!in_fight(guard))
          continue;

          if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
          continue;
          if (guard->pIndexData->vnum != MINION_TEMPLATE)
          continue;
          guard->order = BORDER_ATTACK;
          free_string(guard->ordertarget);
          guard->ordertarget = str_dup(victim->name);
        }
        printf_to_char(ch, "You command your minions to attack %s.\n\r", PERS(victim, ch));
      }
      else if (!str_cmp(arg2, "follow")) {
        if ((victim = get_char_fight(ch, argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          guard = *it;
          if (!in_fight(guard))
          continue;
          if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
          continue;
          if (guard->pIndexData->vnum != MINION_TEMPLATE)
          continue;
          guard->order = BORDER_FOLLOW;
          free_string(guard->ordertarget);
          guard->ordertarget = str_dup(victim->name);
        }
        printf_to_char(ch, "You command your minions to follow %s.\n\r", PERS(victim, ch));
      }
      else if (!str_cmp(arg2, "atwill")) {
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          guard = *it;
          if (!in_fight(guard))
          continue;
          if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
          continue;
          if (guard->pIndexData->vnum != MINION_TEMPLATE)
          continue;
          guard->order = 0;
          free_string(guard->ordertarget);
          guard->ordertarget = str_dup("");
        }
        printf_to_char(ch, "You stop commanding your minions.\n\r");

      }
      else
      send_to_char("Syntax: Command minions attack/follow/atwill (target)\n\r", ch);
    }
    else if (!str_cmp(arg1, "allies") || !str_cmp(arg1, "ally") || !str_cmp(arg1, "muscle")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "attack")) {
        if ((victim = get_char_fight(ch, argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          guard = *it;
          if (!in_fight(guard))
          continue;
          if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
          continue;
          if (guard->pIndexData->vnum != ALLY_TEMPLATE)
          continue;
          guard->order = BORDER_ATTACK;
          free_string(guard->ordertarget);
          guard->ordertarget = str_dup(victim->name);
        }
        printf_to_char(ch, "You command your allies to attack %s.\n\r", PERS(victim, ch));
      }
      else if (!str_cmp(arg2, "follow")) {
        if ((victim = get_char_fight(ch, argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          guard = *it;
          if (!in_fight(guard))
          continue;
          if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
          continue;
          if (guard->pIndexData->vnum != ALLY_TEMPLATE)
          continue;
          guard->order = BORDER_FOLLOW;
          free_string(guard->ordertarget);
          guard->ordertarget = str_dup(victim->name);
        }
        printf_to_char(ch, "You command your allies to follow %s.\n\r", PERS(victim, ch));
      }
      else if (!str_cmp(arg2, "atwill")) {
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          guard = *it;
          if (!in_fight(guard))
          continue;
          if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
          continue;
          if (guard->pIndexData->vnum != ALLY_TEMPLATE)
          continue;
          guard->order = 0;
          free_string(guard->ordertarget);
          guard->ordertarget = str_dup("");
        }
        printf_to_char(ch, "You stop commanding your allies.\n\r");

      }
      else
      send_to_char("Syntax: Command allies attack/follow/atwill (target)\n\r", ch);
    }
    else if (!str_cmp(arg1, "attack")) {
      if ((victim = get_char_fight(ch, argument)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        guard = *it;
        if (!in_fight(guard))
        continue;
        if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
        continue;
        guard->order = BORDER_ATTACK;
        free_string(guard->ordertarget);
        guard->ordertarget = str_dup(victim->name);
      }
      if (fac != NULL && !str_cmp(ch->name, fac->battle_leader)) {
        free_string(fac->battle_target);
        fac->battle_target = str_dup(victim->name);
        fac->battle_order = BORDER_ATTACK;
      }
      printf_to_char(ch, "You command your soldiers to attack %s.\n\r", PERS(victim, ch));
    }
    else if (!str_cmp(arg1, "follow")) {
      if ((victim = get_char_fight(ch, argument)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        guard = *it;
        if (!in_fight(guard))
        continue;
        if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
        continue;
        guard->order = BORDER_FOLLOW;
        free_string(guard->ordertarget);
        guard->ordertarget = str_dup(victim->name);
      }
      if (fac != NULL && !str_cmp(ch->name, fac->battle_leader)) {
        free_string(fac->battle_target);
        fac->battle_target = str_dup(victim->name);
        fac->battle_order = BORDER_FOLLOW;
      }
      printf_to_char(ch, "You command your soldiers to follow %s.\n\r", PERS(victim, ch));
    }
    else if (!str_cmp(arg1, "atwill")) {
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        guard = *it;
        if (!in_fight(guard))
        continue;
        if (guard == NULL || guard->in_room == NULL || !IS_NPC(guard) || str_cmp(guard->protecting, ch->name))
        continue;
        guard->order = 0;
        free_string(guard->ordertarget);
        guard->ordertarget = str_dup("");
      }
      if (fac != NULL && !str_cmp(ch->name, fac->battle_leader)) {
        free_string(fac->battle_target);
        fac->battle_target = str_dup("");
        fac->battle_order = 0;
      }
      printf_to_char(ch, "You stop commanding your soldiers.\n\r");

    }
    else
    send_to_char("Syntax: Command attack/follow/atwill (target)\n\r", ch);
  }

  int bg_number(ROOM_INDEX_DATA *room) { return 1; }
  ROOM_INDEX_DATA *battleroom_bycoord(int battlenumber, int x, int y) {
    ROOM_INDEX_DATA *start = first_battleroom(battlenumber);
    int xshift = 0, yshift = 0;
    int newx = x;
    int newy = y;
    for (; newx > 50;) {
      newx -= 50;
      xshift++;
    }
    for (; newy > 50;) {
      newy -= 50;
      yshift++;
    }
    return sourced_room_by_coordinates(start, start->x + xshift, start->y + yshift, start->z, FALSE);
  }
  ROOM_INDEX_DATA *first_battleroom(int battlenumber) {
    return get_room_index(380000);
  }
  ROOM_INDEX_DATA *next_battleroom(int number, ROOM_INDEX_DATA *from) {
    if (from == NULL)
    return first_battleroom(number);
    ROOM_INDEX_DATA *start = first_battleroom(number);
    int tox = from->x + 1;
    int toy = from->y;
    int toz = from->z;
    if (tox > start->x + 4) {
      tox = start->x;
      toy++;
    }
    if (toy > start->y + 4) {
      if (toz == 0) {
        toz = -1;
        tox = start->x;
        toy = start->y;
      }
      else if (toz == -1) {
        toz = 1;
        tox = start->x;
        toy = start->y;
      }
      else if (toz == 1) {
        toz = 2;
        tox = start->x;
        toy = start->y;
      }
      else
      return NULL;
    }

    return sourced_room_by_coordinates(from, tox, toy, toz, FALSE);
  }
  int first_available_battleground() { return 1; }

  int active_member_pop(FACTION_TYPE *fac) {
    int count = 0;
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(fac->member_names[i]) > 1) {
        if (fac->member_esteem[i] > 100 && fac->member_inactivity[i] < 200)
        count++;
      }
    }
    return count;
  }

  bool is_infused(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (ch->pcdata->infuse_date > 0)
    return TRUE;

    return FALSE;
  }
  bool is_ripe(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (is_gm(ch))
    return FALSE;
    if (!is_infused(ch))
    return FALSE;

    if (ch->pcdata->account == NULL)
    return FALSE;

    if (ch->pcdata->account->last_infuse_date >
        current_time - (3600 * 24 * 7 * 5))
    return FALSE;

    if (ch->pcdata->infuse_date < current_time - (3600 * 24 * 7 * 6))
    return TRUE;

    return FALSE;
  }

  bool join_to_operation(int facvnum, OPERATION_TYPE *op) {
    if (op->hour == 0)
    return FALSE;

    if (op->faction != facvnum && op->competition == COMPETE_CLOSED)
    return FALSE;

    if (facvnum == op->faction)
    return TRUE;

    for (int i = 0; i < 10; i++) {
      if (facvnum == op->enrolled[i])
      return TRUE;
    }
    if (clan_lookup(facvnum)->alliance == clan_lookup(op->faction)->alliance)
    return FALSE;

    if (territory_by_number(op->territoryvnum) != NULL) {
      int base = territory_by_number(op->territoryvnum)->base_faction_core;
      if (base > 0 && clan_lookup(base) != NULL && clan_lookup(base)->type == FACTION_CORE && clan_lookup(base)->alliance == clan_lookup(facvnum)->alliance && base != facvnum)
      return FALSE;
      base = territory_by_number(op->territoryvnum)->base_faction_cult;
      if (base > 0 && clan_lookup(base) != NULL && clan_lookup(base)->type == FACTION_CULT && clan_lookup(base)->alliance == clan_lookup(facvnum)->alliance && base != facvnum)
      return FALSE;
      base = territory_by_number(op->territoryvnum)->base_faction_sect;
      if (base > 0 && clan_lookup(base) != NULL && clan_lookup(base)->type == FACTION_SECT && clan_lookup(base)->alliance == clan_lookup(facvnum)->alliance && base != facvnum)
      return FALSE;
    }

    for (int i = 0; i < 10; i++) {
      if (clan_lookup(op->enrolled[i]) != NULL && clan_lookup(op->enrolled[i])->alliance ==
          clan_lookup(facvnum)->alliance) {
        if (op->soldiers[i] == 0) {
          op->enrolled[i] = facvnum;
          return TRUE;
        }
        else
        return FALSE;
      }
    }
    for (int i = 0; i < 10; i++) {
      if (op->enrolled[i] == 0) {
        op->enrolled[i] = facvnum;
        return TRUE;
      }
    }
    return FALSE;
  }

  void battle_faction(CHAR_DATA *ch, int vnum) {
    if (vnum < 10000)
    clan_lookup(ch->faction)->last_deploy = current_time;
    if (ch->faction == vnum)
    return;
    ch->factiontrue = ch->faction;
    ch->faction = vnum;
    return;
  }

  _DOFUN(do_mindhack) {
    CHAR_DATA *victim;
    if (is_animal(ch))
    return;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      send_to_char("Slaying the living is no longer so simple./n/r", ch);
      return;
    }

    if (is_ghost(victim)) {
      send_to_char("Slaying a ghost is not so simple./n/r", ch);
      return;
    }
    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      send_to_char("You can't mindhack NPCs.\n\r", ch);
      return;
    }

    if (!is_helpless(victim)) {
      send_to_char("You'd have to subdue them first.\n\r", ch);
      return;
    }
    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }
    if (ch == victim) {
      send_to_char("You can't mindhack yourself.\n\r", ch);
      return;
    }

    if(!str_cmp(ch->pcdata->fixation_name, victim->name) && ch->pcdata->fixation_level >= 2 && ch->pcdata->fixation_charges > 0) {
      ch->pcdata->fixation_charges--;
      victim->pcdata->mindbroken = 1;
      send_to_char("You've successfully hacked their mind.\n\r", ch);
      return;
    }

    if (!brainwash_room(ch->in_room)) {
      send_to_char("You need to be in a brainwashing facility to do that.\n\r", ch);
      return;
    }
    int faccost = 0;
    int cashcost = 0;
    if (has_trust(ch, TRUST_RESOURCES, ch->faction) && clan_lookup(ch->faction)->resource >= 8000 + faccost && can_spend_resources(clan_lookup(ch->faction)))
    faccost = 10;
    else
    cashcost = 10000;

    if (is_gm(ch)) {
      faccost = 0;
      cashcost = 0;
    }
    if (ch->money < cashcost && cashcost > 0) {
      printf_to_char(ch, "You'd need at least %d dollars to do that.\n\r", cashcost / 100);
      return;
    }
    if (faccost > 0 && clan_lookup(ch->faction)->resource < 8000 + faccost) {
      printf_to_char(ch, "That costs $%d in society resources.\n\r", faccost * 10);
      return;
    }
    if (faccost > 0 && !can_spend_resources(clan_lookup(ch->faction))) {
      send_to_char("You don't have enough resources for that.\n\r", ch);
      return;
    }

    if (victim->pcdata->mindbroken == 1) {
      send_to_char("They already seem to be attached to the system.\n\r", ch);
      return;
    }
    victim->pcdata->mindbroken = 1;
    printf_to_char(ch, "You settle the crown onto %s's head and activate the device, they tense up for a moment but then relax again as the device infiltrates their mind for $%d faction resources.\n\r", PERS(victim, ch), faccost * 10);
    act("$n settles some sort of circlet on your head, a moment later there's a flash of pain through your head before it suddenly fades away again.", ch, NULL, victim, TO_VICT);
    act("$n settles some sort of circlet on $N's head and actives a device, $N tenses up for a moment before relaxing once more.", ch, NULL, victim, TO_NOTVICT);
    use_resources(faccost, ch->faction, ch, "brainwashing");
  }

  _DOFUN(do_shipment) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!has_trust(ch, TRUST_WAR, ch->faction)) {
      send_to_char("You don't have the nessesary authority to do that.\n\r", ch);
      return;
    }
    FACTION_TYPE *fac = clan_lookup(ch->faction);
    if (!str_cmp(arg1, "view") || !str_cmp(arg1, "list")) {
      for (int i = 0; i < 5; i++) {
        if (fac->shipment_date[i] > 0) {
          ROOM_INDEX_DATA *room = get_room_index(fac->shipment_destination[i]);
          PROP_TYPE *prop = prop_from_room(room);
          if (prop == NULL || room == NULL) {
            fac->shipment_date[i] = 0;
            continue;
          }
          time_t shiptime;
          shiptime = fac->shipment_date[i];
          printf_to_char(ch, "Shipment %d, $%d in supernatural materials, due to arrive at %s at %s", i + 1, fac->shipment_amount[i], prop->propname, (char *)ctime(&shiptime));
        }
      }
    }
    if (!str_cmp(arg1, "arrange")) {
      send_to_char("Interworld trade has ceased.\n\r", ch);
      return;
      int total = 0;
      for (int i = 0; i < 5; i++) {
        if (fac->shipment_date[i] > 0)
        total += fac->shipment_amount[i];
      }
      if (total >= 10000) {
        send_to_char("You already have too many shipments in progress.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg3);
      ROOM_INDEX_DATA *desti = NULL;
      if (!str_cmp(arg2, "house")) {
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) {
            continue;
          }
          if ((*it)->type != PROP_HOUSE)
          continue;
          if ((*it)->vnum == atoi(arg3))
          desti = get_room_index((*it)->firstroom);
        }
      }
      else if (!str_cmp(arg2, "shop")) {
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) {
            continue;
          }
          if ((*it)->type != PROP_SHOP)
          continue;
          if ((*it)->vnum == atoi(arg3))
          desti = get_room_index((*it)->firstroom);
        }
      }
      else {
        send_to_char("Syntax: Shipment arrange shop/house (number) (amount)\n\r", ch);
        return;
      }
      if (desti == NULL) {
        send_to_char("No such property.\n\r", ch);
        return;
      }
      if (desti->area->vnum != OUTER_NORTH_FOREST && desti->area->vnum != OUTER_WEST_FOREST && desti->area->vnum != OUTER_SOUTH_FOREST) {
        send_to_char("Shipments can only be arranged for properties in the outer forest.\n\r", ch);
        return;
      }
      int amount = atoi(argument);
      total += amount * 2;
      if (total > 10000) {
        send_to_char("You already have too many shipments in progress.\n\r", ch);
        return;
      }
      if (total <= 0) {
        send_to_char("Syntax: Shipment arrange shop/house (number) (amount)\n\r", ch);
        return;
      }
      for (int i = 0; i < 5; i++) {
        if (fac->shipment_date[i] == 0) {
          fac->shipment_date[i] = current_time + (3600 * 24 * 14);
          fac->shipment_destination[i] = desti->vnum;
          fac->shipment_amount[i] = amount * 2;

          fac->resource -= amount / 10;
          char buf[MSL];
          sprintf(buf, "%s spends $%d resources on arranging a shipment.", ch->name, amount);
          send_log(ch->faction, buf);
          printf_to_char(ch, "You arrange a shipment.\n\r", ch);
          return;
        }
      }
    }
  }

  void shipment_check(FACTION_TYPE *fac) {
    CHAR_DATA *victim;
    for (int i = 0; i < 5; i++) {
      if (fac->shipment_date[i] != 0 && fac->shipment_date[i] < (current_time + 900) && fac->shipment_date[i] > current_time && fac->shipment_warned[i] == 0) {
        ROOM_INDEX_DATA *desti = get_room_index(fac->shipment_destination[i]);
        if (desti == NULL)
        continue;
        fac->shipment_warned[i] = 1;

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

          if (victim->in_room == desti) {
            printf_to_char(victim, "A team of horses and wagons rolls in and starts unloading a shipment of crates and sacks.\n\r");
          }
          else if (can_see_room_distance(victim, desti, DISTANCE_MEDIUM)) {
            printf_to_char(victim, "[%s]A team of horses and wagons rolls in and starts unloading a shipment of crates and sacks.\n\r", room_distance(victim, desti));
          }
        }
        char buf[MSL];
        sprintf(buf, "The scouts report a team of horses and wagons unloading a shipment of valuable supernatural material at %s.", prop_from_room(desti)->propname);
        scout_report(buf);
      }

      if (fac->shipment_date[i] != 0 && fac->shipment_date[i] < current_time) {
        fac->shipment_date[i] = 0;
        ROOM_INDEX_DATA *desti = get_room_index(fac->shipment_destination[i]);
        if (desti == NULL)
        continue;
        OBJ_DATA *obj;

        obj = create_object(get_obj_index(36), 0);
        obj->level = fac->shipment_amount[i] / 5;
        obj->size = 50;
        free_string(obj->name);
        obj->name = str_dup("collection crates sacks shipment");
        free_string(obj->short_descr);
        obj->short_descr = str_dup("collection of crates and sacks");
        free_string(obj->description);
        obj->description = str_dup("A collection of crates and sacks");
        obj_to_room(obj, desti);

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

          if (victim->in_room == desti) {
            printf_to_char(victim, "The team finishes unloading the shipment.\n\r");
          }
          else if (can_see_room_distance(victim, desti, DISTANCE_MEDIUM)) {
            printf_to_char(victim, "[%s]The team finishes unloading the shipment.\n\r", room_distance(victim, desti));
          }
        }
      }
    }
  }

  void reclaim_items(CHAR_DATA *ch) {
    if (activeoperation != NULL) {
      for (ROOM_INDEX_DATA *broom =
      first_battleroom(activeoperation->battleground_number);
      broom != NULL;
      broom = next_battleroom(activeoperation->battleground_number, broom)) {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        for (obj = broom->contents; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;

          if (is_name(ch->name, obj->name)) {
            obj_from_room(obj);
            obj_to_char(obj, ch);
          }
        }
      }
    }
  }

  bool rescue_person(CHAR_DATA *ch, char *name) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if ((victim = get_char_world(ch, name)) != NULL) // Victim is online.
    online = TRUE;
    else if ((victim = get_char_world_pc(name)) != NULL)
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return FALSE;
      }
      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return FALSE;
    }
    if (victim->in_room == NULL)
    return FALSE;
    if (victim->in_room->area->vnum < 22 || victim->in_room->area->vnum > 25)
    return FALSE;
    if (pc_pop(victim->in_room) > 1)
    return FALSE;
    char_from_room(victim);
    char_to_room(victim, ch->in_room);
    if (IS_FLAG(victim->act, PLR_BOUND))
    REMOVE_FLAG(victim->act, PLR_BOUND);
    save_char_obj(victim, FALSE, FALSE);
    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
    return TRUE;
  }

  bool is_prisoner(CHAR_DATA *ch) { return FALSE; }

  int goal_influence(int goal, int faction, int init) {
    FACTION_TYPE *fac = clan_lookup(faction);

    int cost = init;
    if (goal == GOAL_INCITE || goal == GOAL_PROVOKE || goal == GOAL_SABOUTAGE || goal == GOAL_RAZE || goal == GOAL_LOOT || goal == GOAL_RESCUE) {
      if (fac->axes[AXES_ANARCHY] == AXES_FARLEFT)
      cost = cost * 50 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_MIDLEFT)
      cost = cost * 70 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_NEARLEFT)
      cost = cost * 90 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_NEARRIGHT)
      cost = cost * 110 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_MIDRIGHT)
      cost = cost * 130 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_FARRIGHT)
      cost = cost * 150 / 100;
    }
    else if (goal == GOAL_KIDNAP || goal == GOAL_SUPPRESS || goal == GOAL_SETUP || goal == GOAL_CALM) {

      if (fac->axes[AXES_ANARCHY] == AXES_FARLEFT)
      cost = cost * 150 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_MIDLEFT)
      cost = cost * 130 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_NEARLEFT)
      cost = cost * 110 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_NEARRIGHT)
      cost = cost * 90 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_MIDRIGHT)
      cost = cost * 70 / 100;
      else if (fac->axes[AXES_ANARCHY] == AXES_FARRIGHT)
      cost = cost * 50 / 100;
    }
    cost = UMIN(cost, 95);
    return cost;
  }

  _DOFUN(do_goal) {
    if (ch->pcdata->ci_editing == 12) {
      LOCATION_TYPE *loc;
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "psychic")) {
        free_string(ch->pcdata->ci_short);
        ch->pcdata->ci_short = str_dup("Haven");
        ch->pcdata->ci_discipline2 = GOAL_PSYCHIC;
        send_to_char("Done.\n\r", ch);
        return;
      }
      loc = get_loc(ch->pcdata->ci_short);
      if (loc == NULL) {
        send_to_char("You need to select a territory first.\n\r", ch);
        return;
      }
      if (ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->alliance == 0) {
        send_to_char("You must be in a faction with an alliance.\n\r", ch);
        return;
      }
      FACTION_TYPE *fac = clan_lookup(ch->faction);
      int phil = clan_lookup(ch->faction)->alliance;
      if(fac->type == FACTION_SECT)
      phil += 10;
      if (!str_cmp(argument, "unrest") || !str_cmp(argument, "incite")) {
        if (clan_lookup(ch->faction)->last_incite >
            current_time - (3600 * 24 * 14)) {
          send_to_char("Your society can't attempt that yet.\n\r", ch);
          return;
        }
        if (loc->status == STATUS_CONTEST || loc->status == STATUS_WAR) {
          send_to_char("That territory is already in a contested state.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_INCITE;
        send_to_char("Done.\n\r", ch);
        return;
      }
      else {
        if (loc->status != STATUS_CONTEST && loc->status != STATUS_WAR) {
          send_to_char("That region is too peaceful to perform any operations other than inciting.\n\r", ch);
          return;
        }
      }
      if (!str_cmp(argument, "calm") || !str_cmp(argument, "peace")) {
        if (loc->phil_amount[phil] < goal_influence(GOAL_CALM, ch->faction, 30)) {
          send_to_char("You need at least 30% influence to do that.\n\r", ch);
          return;
        }
        if (loc->status != STATUS_CONTEST && loc->status != STATUS_WAR) {
          send_to_char("That territory is already in a peaceful state.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_CALM;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "provoke") || !str_cmp(argument, "war")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_PROVOKE, ch->faction, 30)) {
          send_to_char("You need at least 30% influence to do that.\n\r", ch);
          return;
        }
        if (loc->status != STATUS_CONTEST) {
          send_to_char("That territory isn't in a contested state.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_PROVOKE;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "plant") || !str_cmp(argument, "npc")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_PLANT, ch->faction, 10)) {
          send_to_char("You need at least 10% influence to do that.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_PLANT;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "setup") || !str_cmp(argument, "place")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_SETUP, ch->faction, 10)) {
          send_to_char("You need at least 10% influence to do that.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_SETUP;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "establish") || !str_cmp(argument, "foothold")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_FOOTHOLD, ch->faction, 80)) {
          send_to_char("You need at least 80% influence to do that.\n\r", ch);
          return;
        }
        if (loc->base_faction_core != 0 && fac->type == FACTION_CORE) {
          send_to_char("Another group already has established a foothold here.\n\r", ch);
          return;
        }
        if (loc->base_faction_cult != 0 && fac->type == FACTION_CULT) {
          send_to_char("Another group already has established a foothold here.\n\r", ch);
          return;
        }
        if (loc->base_faction_sect != 0 && fac->type == FACTION_SECT) {
          send_to_char("Another group already has established a foothold here.\n\r", ch);
          return;
        }

        ch->pcdata->ci_discipline2 = GOAL_FOOTHOLD;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "uproot")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_UPROOT, ch->faction, 50)) {
          send_to_char("You need at least 50% influence to do that.\n\r", ch);
          return;
        }
        if (loc->base_faction_core == 0 && fac->type == FACTION_CORE) {
          send_to_char("No group has a foothold here.\n\r", ch);
          return;
        }
        if (loc->base_faction_cult == 0 && fac->type == FACTION_CULT) {
          send_to_char("No group has a foothold here.\n\r", ch);
          return;
        }
        if (loc->base_faction_sect == 0 && fac->type == FACTION_SECT) {
          send_to_char("No group has a foothold here.\n\r", ch);
          return;
        }

        ch->pcdata->ci_discipline2 = GOAL_UPROOT;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "loot")) {
        if (loc->phil_amount[phil] < goal_influence(GOAL_LOOT, ch->faction, 80)) {
          send_to_char("You need at least 80% influence to do that.\n\r", ch);
          return;
        }
        if (loc->base_faction_core != ch->faction && loc->base_faction_cult != ch->faction && loc->base_faction_sect != ch->faction) {
          send_to_char("Your faction doesn't have a foothold there.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_LOOT;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "suppress")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_SUPPRESS, ch->faction, 50)) {
          send_to_char("You need at least 50% influence to do that.\n\r", ch);
          return;
        }
        if (loc->base_faction_core != ch->faction && loc->base_faction_cult != ch->faction && loc->base_faction_sect != ch->faction) {
          send_to_char("Your faction doesn't have a foothold there.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_SUPPRESS;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "kidnap")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_KIDNAP, ch->faction, 75)) {
          send_to_char("You need at least 75% influence to do that.\n\r", ch);
          return;
        }
        if (loc->base_faction_core != ch->faction && loc->base_faction_cult != ch->faction && loc->base_faction_sect != ch->faction) {
          send_to_char("Your faction doesn't have a foothold there.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_KIDNAP;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "control")) {
        if (loc->phil_amount[phil] > 50) {
          send_to_char("You can  only do that when below 50% control, try plant, setup, raze, or assassinate instead.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_CONTROL;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "assassinate")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_ASSASSINATE, ch->faction, 25)) {
          send_to_char("You need at least 25% influence to do that.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_ASSASSINATE;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "raze")) {
        if (loc->phil_amount[phil] < goal_influence(GOAL_RAZE, ch->faction, 25)) {
          send_to_char("You need at least 25% influence to do that.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_RAZE;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "saboutage")) {
        if (loc->phil_amount[phil] <
            goal_influence(GOAL_SABOUTAGE, ch->faction, 25)) {
          send_to_char("You need at least 25% influence to do that.\n\r", ch);
          return;
        }
        ch->pcdata->ci_discipline2 = GOAL_SABOUTAGE;
        send_to_char("Done.\n\r", ch);
        return;
      }
      send_to_char("Viable goals are: incite, calm, provoke, plant, setup, foothold, uproot, loot, suppress, kidnap, control, assassinate, rescue, raze, saboutage\n\r", ch);
      return;
    }
  }

  const char *goal_names[] = {"None",        "Incite",   "Calm",     "Provoke", "Plant",       "Setup",    "Foothold", "Raze", "Loot",        "Suppress", "Kidnap",   "Control", "Assassinate", "Rescue",   "Uproot",   "Saboutage"};

  int phil_amount(FACTION_TYPE *fac, LOCATION_TYPE *loc) {
    if (fac->type == FACTION_CORE)
    return loc->phil_amount[fac->vnum];
    if (fac->type == FACTION_CULT)
    return loc->phil_amount[fac->alliance];
    if (fac->type == FACTION_SECT)
    return loc->phil_amount[fac->alliance + 10];
    if (fac->type == FACTION_NPC) {
      for (int i = 0; i < 5; i++) {
        if (!str_cmp(loc->other_name[i], fac->name)) {
          return loc->other_amount[i];
        }
      }
      for (int i = 0; i < 5; i++) {
        if (loc->other_amount[i] < 1) {
          free_string(loc->other_name[i]);
          loc->other_name[i] = str_dup(fac->name);
          loc->other_amount[i] = 1;
          return 1;
        }
      }
    }
    return 0;
  }

  void battlecheck(CHAR_DATA *ch) {
    if (IS_IMMORTAL(ch))
    return;
    if (ch->in_room != NULL && ch->in_room->area->vnum == 29 && isactiveoperation == FALSE) {
      char_from_room(ch);
      int vnum = ch->pcdata->deploy_from;
      if (vnum == 0) {
        vnum = 1;
      }
      ROOM_INDEX_DATA *to_room = get_room_index(vnum);
      char_to_room(ch, to_room);
      if (ch->factiontrue > -1) {
        ch->faction = ch->factiontrue;
        ch->factiontrue = -1;
      }
    }
  }

  bool shipment_carrier(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *objcont;
    OBJ_DATA *objcont_next;

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (obj->pIndexData->vnum == 36 && obj->size == 50)
      return TRUE;

      if (obj->item_type == ITEM_CONTAINER) {

        for (objcont = obj->contains; objcont != NULL; objcont = objcont_next) {
          objcont_next = objcont->next_content;

          if (objcont->pIndexData->vnum == 36 && objcont->size == 50)
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool can_spend_resources(FACTION_TYPE *fac) {
    if (fac == NULL)
    return TRUE;
    if (fac->stasis > 0)
    return FALSE;

    return TRUE;
  }

  int get_alliance(FACTION_TYPE *fac, int issue, int type) {
    int pos = fac->axes[issue];
    char buf[MSL];
    sprintf(buf, "ALLIANCEPOS: %s: %d, %d, %d", fac->name, issue, type, pos);
    //    log_string(buf);
    if (type == ALLIANCE_NORMAL) {
      if (pos == AXES_FARLEFT || pos == AXES_MIDLEFT)
      return ALLIANCE_SIDELEFT;
      else if (pos == AXES_FARRIGHT || pos == AXES_MIDRIGHT)
      return ALLIANCE_SIDERIGHT;
      else
      return ALLIANCE_SIDEMID;
    }
    if (type == ALLIANCE_HARDMIDDLE) {
      if (pos == AXES_FARLEFT || pos == AXES_MIDLEFT || pos == AXES_NEARLEFT)
      return ALLIANCE_SIDELEFT;
      else if (pos == AXES_FARRIGHT || pos == AXES_MIDRIGHT || pos == AXES_NEARRIGHT)
      return ALLIANCE_SIDERIGHT;
      else
      return ALLIANCE_SIDEMID;
    }
    if (type == ALLIANCE_HARDLEFT) {
      if (pos == AXES_FARLEFT)
      return ALLIANCE_SIDELEFT;
      else if (pos == AXES_MIDLEFT || pos == AXES_NEARLEFT)
      return ALLIANCE_SIDEMID;
      else
      return ALLIANCE_SIDERIGHT;
    }
    if (type == ALLIANCE_HARDRIGHT) {
      if (pos == AXES_FARRIGHT)
      return ALLIANCE_SIDERIGHT;
      else if (pos == AXES_MIDRIGHT || pos == AXES_NEARRIGHT)
      return ALLIANCE_SIDEMID;
      else
      return ALLIANCE_SIDELEFT;
    }
    return 0;
  }

  int fac_power(FACTION_TYPE *fac) {
    int power = 1;
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(fac->member_names[i]) < 2 || fac->member_inactivity[i] >= 150)
      continue;
      if (fac->member_tier[i] > 0)
      power += 1 * UMIN(10, UMAX(3, fac->member_power[i] / 15));
      if (fac->member_tier[i] > 1)
      power +=
      fac->member_tier[i] * UMIN(10, UMAX(3, fac->member_power[i] / 15));
    }
    char buf[MSL];
    sprintf(buf, "ALLIANCEFACPOWER: %s, %d", fac->name, power);
    //    log_string(buf);
    if (fac->last_deploy < current_time - (3600 * 24 * 21) && fac->last_operation < current_time - (3600 * 24 * 21))
    power = power * 4 / 5;
    if (fac->lifeearned <= -800)
    power = power * 4 / 5;
    if (fac->resource < 6000)
    power = power * 4 / 5;
    power += 3 * sqrt(fac->operation_wins);
    return power;
  }

  int minalliancebalance(int ftype) {
    int prohibtype;
    int prohibissue;
    if (ftype == FACTION_CULT) {
      prohibtype = time_info.cult_alliance_type;
      prohibissue = time_info.cult_alliance_issue;
    }
    else {
      prohibtype = time_info.sect_alliance_type;
      prohibissue = time_info.sect_alliance_issue;
    }
    int mindistance = 100000;
    int leftpower, rightpower, midpower, diffval = 0;
    for (int x = 1; x <= AXES_MAX; x++) // issues
    {
      for (int y = 1; y < 5; y++) // types
      {
        if (y == prohibtype && x == prohibissue)
        continue;
        leftpower = 0;
        rightpower = 0;
        midpower = 0;
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->nopart == 1 || (*it)->antagonist == 1 || (*it)->cardinal == 1)
          continue;
          if (generic_faction_vnum((*it)->vnum))
          continue;
          if ((*it)->type != ftype)
          continue;
          if (get_alliance((*it), x, y) == ALLIANCE_SIDERIGHT)
          rightpower += fac_power((*it));
          else if (get_alliance((*it), x, y) == ALLIANCE_SIDELEFT)
          leftpower += fac_power((*it));
          else if (get_alliance((*it), x, y) == ALLIANCE_SIDEMID)
          midpower += fac_power((*it));
        }
        /*
lowval = UMIN(UMIN(rightpower, leftpower), midpower);
highval = UMAX(UMAX(rightpower, leftpower), midpower);
diffval = highval - lowval;
*/
        diffval = 0;
        int rl = UMAX((rightpower - leftpower), (rightpower - leftpower) * -1);
        int ml = UMAX((midpower - leftpower), (midpower - leftpower) * -1);
        int mr = UMAX((midpower - rightpower), (midpower - rightpower) * -1);

        diffval += sqrt(rl * rl * rl);
        diffval += sqrt(ml * ml * ml);
        diffval += sqrt(mr * mr * mr);

        char buf[MSL];
        sprintf(buf, "ALLIANCEMINCALC %s:%d, %s:%d, %s:%d Diff:%d", alliance_names(x, ALLIANCE_SIDELEFT, y), leftpower, alliance_names(x, ALLIANCE_SIDEMID, y), midpower, alliance_names(x, ALLIANCE_SIDERIGHT, y), rightpower, diffval);
        log_string(buf);

        if (diffval < mindistance) {
          mindistance = diffval;
        }
      }
    }
    return mindistance;
  }
  int currentalliancebalance(int ftype) {
    int leftpower, rightpower, midpower;
    leftpower = 0;
    rightpower = 0;
    midpower = 0;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->nopart == 1 || (*it)->antagonist == 1 || (*it)->cardinal == 1)
      continue;
      if (generic_faction_vnum((*it)->vnum))
      continue;
      if ((*it)->type != ftype)
      continue;

      if (ftype == FACTION_CULT) {
        if (get_alliance((*it), time_info.cult_alliance_issue, time_info.cult_alliance_type) == ALLIANCE_SIDERIGHT)
        rightpower += fac_power((*it));
        else if (get_alliance((*it), time_info.cult_alliance_issue, time_info.cult_alliance_type) == ALLIANCE_SIDELEFT)
        leftpower += fac_power((*it));
        else if (get_alliance((*it), time_info.cult_alliance_issue, time_info.cult_alliance_type) == ALLIANCE_SIDEMID)
        midpower += fac_power((*it));
      }
      else {
        if (get_alliance((*it), time_info.sect_alliance_issue, time_info.sect_alliance_type) == ALLIANCE_SIDERIGHT)
        rightpower += fac_power((*it));
        else if (get_alliance((*it), time_info.sect_alliance_issue, time_info.sect_alliance_type) == ALLIANCE_SIDELEFT)
        leftpower += fac_power((*it));
        else if (get_alliance((*it), time_info.sect_alliance_issue, time_info.sect_alliance_type) == ALLIANCE_SIDEMID)
        midpower += fac_power((*it));
      }
    }

    int diffval = 0;
    int rl = UMAX((rightpower - leftpower), (rightpower - leftpower) * -1);
    int ml = UMAX((midpower - leftpower), (midpower - leftpower) * -1);
    int mr = UMAX((midpower - rightpower), (midpower - rightpower) * -1);

    diffval += sqrt(rl * rl * rl);
    diffval += sqrt(ml * ml * ml);
    diffval += sqrt(mr * mr * mr);
    char buf[MSL];
    sprintf(buf, "ALLIANCEMINCALCCURRENT %s:%d, %s:%d, %s:%d Diff:%d", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), leftpower, alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), midpower, alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), rightpower, diffval);
    log_string(buf);
    return diffval;
  }

  void new_alliance(int ftype) {
    char buf[MSL];
    int prohibtype;
    int prohibissue;
    if (ftype == FACTION_CULT) {
      prohibtype = time_info.cult_alliance_type;
      prohibissue = time_info.cult_alliance_issue;
    }
    else {
      prohibtype = time_info.sect_alliance_type;
      prohibissue = time_info.sect_alliance_issue;
    }
    int mindistance = 100000;
    int minissue = -1;
    int mintype = -1;
    int leftpower, rightpower, midpower;
    for (int x = 1; x <= AXES_MAX; x++) // issues
    {
      for (int y = 1; y < 5; y++) // types
      {
        if (y == prohibtype && x == prohibissue)
        continue;
        leftpower = 0;
        rightpower = 0;
        midpower = 0;
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->nopart == 1 || (*it)->antagonist == 1 || (*it)->cardinal == 1)
          continue;
          if (generic_faction_vnum((*it)->vnum))
          continue;
          if ((*it)->type != ftype)
          continue;
          if (get_alliance((*it), x, y) == ALLIANCE_SIDERIGHT)
          rightpower += fac_power((*it));
          else if (get_alliance((*it), x, y) == ALLIANCE_SIDELEFT)
          leftpower += fac_power((*it));
          else if (get_alliance((*it), x, y) == ALLIANCE_SIDEMID)
          midpower += fac_power((*it));
        }
        int diffval = 0;
        int rl = UMAX((rightpower - leftpower), (rightpower - leftpower) * -1);
        int ml = UMAX((midpower - leftpower), (midpower - leftpower) * -1);
        int mr = UMAX((midpower - rightpower), (midpower - rightpower) * -1);

        diffval += sqrt(rl * rl * rl);
        diffval += sqrt(ml * ml * ml);
        diffval += sqrt(mr * mr * mr);

        if (x == AXES_CORRUPT)
        diffval = diffval * 3 / 5;
        if (x == AXES_MATERIAL)
        diffval = diffval * 6 / 4;
        if (x == AXES_COMBAT)
        diffval = diffval * 7 / 4;
        if (x == AXES_ANARCHY)
        diffval = diffval * 6 / 4;
        if (diffval < mindistance) {
          mindistance = diffval;
          minissue = x;
          mintype = y;
        }
        sprintf(buf, "ALLIANCECALC: %d: %d, %d: Right, %d, Left %d, Mid %d, Diff %d, mindist %d, curris %d, currtype %d", ftype, x, y, rightpower, leftpower, midpower, diffval, mindistance, time_info.cult_alliance_issue, time_info.cult_alliance_type);
        log_string(buf);
      }
    }
    if (ftype == FACTION_CULT) {
      time_info.cult_alliance_issue = minissue;
      time_info.cult_alliance_type = mintype;
    }
    else {
      time_info.sect_alliance_issue = minissue;
      time_info.sect_alliance_type = mintype;
    }
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0 || generic_faction_vnum((*it)->vnum))
      continue;
      if ((*it)->type != ftype)
      continue;
      if ((*it)->cardinal == 1 || (*it)->nopart == 1)
      (*it)->alliance = 0;
      else {
        (*it)->outcast = 0;
        (*it)->alliance = get_alliance((*it), minissue, mintype);
        if (ftype == FACTION_CULT) {
          sprintf(buf, "You join the %s Alliance.", alliance_names(time_info.cult_alliance_issue, (*it)->alliance, time_info.cult_alliance_type));
          send_message((*it)->vnum, buf);
          sprintf(buf, "ALLIANCE RESULT: %s, %d:%s", (*it)->name, (*it)->alliance, axes_names(time_info.cult_alliance_issue, (*it)->alliance));
          log_string(buf);
        }
        else {
          sprintf(buf, "You join the %s Alliance.", alliance_names(time_info.sect_alliance_issue, (*it)->alliance, time_info.sect_alliance_type));
          send_message((*it)->vnum, buf);
        }
        for (int i = 0; i < 100; i++)
        (*it)->member_power[i] /= 10;
        (*it)->lifeearned /= 3;
      }
      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        for (int i = 0; i < 10; i++) {
          (*it)->phil_amount[i] /= 2;
        }
        for (int i = 0; i < 5; i++)
        (*it)->other_amount[i] /= 3;
      }
    }
  }

  char *axes_names(int axes, int position) {
    if (axes == AXES_SUPERNATURAL) {
      if (position == ALLIANCE_SIDELEFT)
      return "Pro-Supernatural";
      if (position == ALLIANCE_SIDERIGHT)
      return "Anti-Supernatural";
      if (position == ALLIANCE_SIDEMID)
      return "Balanced";
    }
    if (axes == AXES_MATERIAL) {
      if (position == ALLIANCE_SIDELEFT)
      return "Spiritual";
      if (position == ALLIANCE_SIDERIGHT)
      return "Material";
      if (position == ALLIANCE_SIDEMID)
      return "Balanced";
    }
    if (axes == AXES_COMBAT) {
      if (position == ALLIANCE_SIDELEFT)
      return "Combative";
      if (position == ALLIANCE_SIDERIGHT)
      return "Manipulative";
      if (position == ALLIANCE_SIDEMID)
      return "Balanced";
    }
    if (axes == AXES_CORRUPT) {
      if (position == ALLIANCE_SIDELEFT)
      return "Corrupt";
      if (position == ALLIANCE_SIDERIGHT)
      return "Virtuous";
      if (position == ALLIANCE_SIDEMID)
      return "Balanced";
    }
    if (axes == AXES_DEMOCRATIC) {
      if (position == ALLIANCE_SIDELEFT)
      return "Autocratic";
      if (position == ALLIANCE_SIDERIGHT)
      return "Democratic";
      if (position == ALLIANCE_SIDEMID)
      return "Balanced";
    }
    if (axes == AXES_ANARCHY) {
      if (position == ALLIANCE_SIDELEFT)
      return "Anarchistic";
      if (position == ALLIANCE_SIDERIGHT)
      return "Lawful";
      if (position == ALLIANCE_SIDEMID)
      return "Balanced";
    }

    return "";
  }

  char *alliance_names(int axes, int position, int type) {
    if (axes == AXES_SUPERNATURAL) {
      if (type == ALLIANCE_NORMAL || type == ALLIANCE_HARDMIDDLE) {
        if (position == ALLIANCE_SIDELEFT)
        return "Pro-Supernatural";
        if (position == ALLIANCE_SIDERIGHT)
        return "Anti-Supernatural";
        if (position == ALLIANCE_SIDEMID)
        return "Balanced";
      }
      else if (type == ALLIANCE_HARDLEFT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Pro-Supernatural Extremists";
        if (position == ALLIANCE_SIDERIGHT)
        return "Balanced&Anti-Supernatural";
        if (position == ALLIANCE_SIDEMID)
        return "Pro-Supernatural Moderates";
      }
      else if (type == ALLIANCE_HARDRIGHT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Balanced&Pro-Supernatural";
        if (position == ALLIANCE_SIDERIGHT)
        return "Anti-Supernatural Extremists";
        if (position == ALLIANCE_SIDEMID)
        return "Anti-Supernatural Moderates";
      }
    }
    if (axes == AXES_MATERIAL) {
      if (type == ALLIANCE_NORMAL || type == ALLIANCE_HARDMIDDLE) {
        if (position == ALLIANCE_SIDELEFT)
        return "Spiritual";
        if (position == ALLIANCE_SIDERIGHT)
        return "Material";
        if (position == ALLIANCE_SIDEMID)
        return "Balanced";
      }
      else if (type == ALLIANCE_HARDLEFT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Spiritual Extremists";
        if (position == ALLIANCE_SIDERIGHT)
        return "Balanced&Material";
        if (position == ALLIANCE_SIDEMID)
        return "Spiritual Moderates";
      }
      else if (type == ALLIANCE_HARDRIGHT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Balanced&Spiritual";
        if (position == ALLIANCE_SIDERIGHT)
        return "Material Extremists";
        if (position == ALLIANCE_SIDEMID)
        return "Material Moderates";
      }
    }
    if (axes == AXES_COMBAT) {
      if (type == ALLIANCE_NORMAL || type == ALLIANCE_HARDMIDDLE) {
        if (position == ALLIANCE_SIDELEFT)
        return "Combative";
        if (position == ALLIANCE_SIDERIGHT)
        return "Manipulative";
        if (position == ALLIANCE_SIDEMID)
        return "Balanced";
      }
      else if (type == ALLIANCE_HARDLEFT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Combative Extremists";
        if (position == ALLIANCE_SIDERIGHT)
        return "Balanced&Manipulative";
        if (position == ALLIANCE_SIDEMID)
        return "Combative Moderates";
      }
      else if (type == ALLIANCE_HARDRIGHT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Balanced&Combative";
        if (position == ALLIANCE_SIDERIGHT)
        return "Manipulative Extremists";
        if (position == ALLIANCE_SIDEMID)
        return "Manipulative Moderates";
      }
    }
    if (axes == AXES_CORRUPT) {
      if (type == ALLIANCE_NORMAL || type == ALLIANCE_HARDMIDDLE) {
        if (position == ALLIANCE_SIDELEFT)
        return "Corrupt";
        if (position == ALLIANCE_SIDERIGHT)
        return "Virtuous";
        if (position == ALLIANCE_SIDEMID)
        return "Balanced";
      }
      else if (type == ALLIANCE_HARDLEFT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Corrupt Extremists";
        if (position == ALLIANCE_SIDERIGHT)
        return "Balanced&Virtuous";
        if (position == ALLIANCE_SIDEMID)
        return "Corrupt Moderates";
      }
      else if (type == ALLIANCE_HARDRIGHT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Balanced&Corrupt";
        if (position == ALLIANCE_SIDERIGHT)
        return "Virtuous Extremists";
        if (position == ALLIANCE_SIDEMID)
        return "Virtuous Moderates";
      }
    }
    if (axes == AXES_DEMOCRATIC) {
      if (type == ALLIANCE_NORMAL || type == ALLIANCE_HARDMIDDLE) {
        if (position == ALLIANCE_SIDELEFT)
        return "Autocratic";
        if (position == ALLIANCE_SIDERIGHT)
        return "Democratic";
        if (position == ALLIANCE_SIDEMID)
        return "Balanced";
      }
      else if (type == ALLIANCE_HARDLEFT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Autocratic Extremists";
        if (position == ALLIANCE_SIDERIGHT)
        return "Balanced&Democratic";
        if (position == ALLIANCE_SIDEMID)
        return "Autocratic Moderates";
      }
      else if (type == ALLIANCE_HARDRIGHT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Balanced&Autocratic";
        if (position == ALLIANCE_SIDERIGHT)
        return "Democratic Extremists";
        if (position == ALLIANCE_SIDEMID)
        return "Democratic Moderates";
      }
    }
    if (axes == AXES_ANARCHY) {
      if (type == ALLIANCE_NORMAL || type == ALLIANCE_HARDMIDDLE) {
        if (position == ALLIANCE_SIDELEFT)
        return "Anarchistic";
        if (position == ALLIANCE_SIDERIGHT)
        return "Lawful";
        if (position == ALLIANCE_SIDEMID)
        return "Balanced";
      }
      else if (type == ALLIANCE_HARDLEFT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Anarchistic Extremists";
        if (position == ALLIANCE_SIDERIGHT)
        return "Balanced&Lawful";
        if (position == ALLIANCE_SIDEMID)
        return "Anarchistic Moderates";
      }
      else if (type == ALLIANCE_HARDRIGHT) {
        if (position == ALLIANCE_SIDELEFT)
        return "Balanced&Anarchistic";
        if (position == ALLIANCE_SIDERIGHT)
        return "Lawful Extremists";
        if (position == ALLIANCE_SIDEMID)
        return "Lawful Moderates";
      }
    }
    return "";
  }

  bool faction_hardelligible(CHAR_DATA *ch, FACTION_TYPE *fac, bool show, CHAR_DATA *display) {
    if (in_fight(ch) || battleground(ch->in_room))
    return TRUE;

    bool unset = FALSE;
    for (int i = 1; i <= AXES_MAX; i++) {
      if (fac->axes[i] == 0)
      unset = TRUE;
    }
    if (unset == TRUE)
    return TRUE;

    if (fac->vnum == FACTION_TEMPLE && arcane_focus(ch) > 1 && arcane_focus(ch) > combat_focus(ch) && arcane_focus(ch) > prof_focus(ch))
    return FALSE;
    if (fac->vnum == FACTION_HAND && combat_focus(ch) > 1 && combat_focus(ch) > arcane_focus(ch) && combat_focus(ch) > prof_focus(ch))
    return FALSE;
    if (fac->vnum == FACTION_TEMPLE && arcane_focus(ch) > 1 && arcane_focus(ch) > combat_focus(ch) && arcane_focus(ch) > prof_focus(ch))
    return FALSE;

    if ((get_real_age(ch) < 21 && get_age(ch) < 21) && (is_super(ch) || seems_super(ch)) && fac->axes[AXES_SUPERNATURAL] < 3 && !college_student(ch, FALSE) && !clinic_patient(ch) && fac->college == 0) {
      if (show == TRUE)
      send_to_char("You are too young to join a supernatural society.\n\r", display);
      return FALSE;
    }

    if (get_tier(ch) == 5 && fac->axes[AXES_CORRUPT] >= AXES_NEARRIGHT) {
      if (show == TRUE)
      send_to_char("You are too high tier to join that society.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 4 && fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT) {
      if (show == TRUE)
      send_to_char("You are too high tier to join that society.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 3 && fac->axes[AXES_CORRUPT] >= AXES_FARRIGHT && ch->skills[SKILL_MENTALDISCIPLINE] <= 0) {
      if (show == TRUE)
      send_to_char("You are too high tier to join that society.\n\r", display);
      return FALSE;
    }

    if (ch->race == RACE_FACULTY) {
      if (show == TRUE)
      send_to_char("Institute staff cannot join societies.\n\r", display);
      return FALSE;
    }

    return TRUE;
  }

  bool trust_elligible(CHAR_DATA *ch, FACTION_TYPE *fac, bool show, CHAR_DATA *display) {
    bool unset = FALSE;
    for (int i = 1; i <= AXES_MAX; i++) {
      if (fac->axes[i] == 0)
      unset = TRUE;
    }
    if (unset == TRUE)
    return TRUE;

    if ((get_real_age(ch) < 21 && get_age(ch) < 21) && (is_super(ch) || seems_super(ch)) && fac->axes[AXES_SUPERNATURAL] < 3 && fac->college == 0 && fac->axes[AXES_SUPERNATURAL] != 0) {
      if (show == TRUE)
      send_to_char("Too young for a supernatural society.\n\r", display);
      return FALSE;
    }

    if (get_tier(ch) == 5 && fac->axes[AXES_CORRUPT] >= AXES_NEARRIGHT) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 4 && fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 4 && fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 3 && fac->axes[AXES_CORRUPT] >= AXES_FARRIGHT && ch->skills[SKILL_MENTALDISCIPLINE] <= 0) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }

    if (fac->restrictions[RESTRICT_MALE] == 1 && ch->sex == SEX_MALE && ch->pcdata->sexchange_time <= 0) {
      if (show == TRUE)
      send_to_char("That society does not allow men.\n\r", display);
      return FALSE;
    }

    if (fac->restrictions[RESTRICT_FEMALE] == 1 && ch->sex == SEX_FEMALE && ch->pcdata->sexchange_time <= 0) {
      if (show == TRUE)
      send_to_char("That society does not allow women.\n\r", display);
      return FALSE;
    }

    if (event_cleanse == 0) {
      if (fac->restrictions[RESTRICT_PRIMARY_ARCANE] == 1) {

        if (arcane_focus(ch) > combat_focus(ch) && arcane_focus(ch) > prof_focus(ch)) {
          if (show == TRUE)
          send_to_char("That society does not allow primary arcanists.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_ARCANE] == 1) {

        if (arcane_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow arcanists.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_PRIMARY_COMBAT] == 1) {

        if (combat_focus(ch) > arcane_focus(ch) && combat_focus(ch) > prof_focus(ch)) {
          if (show == TRUE)
          send_to_char("That society does not allow primary commbatants.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_COMBAT] == 1) {
        if (combat_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow combatants.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_PRIMARY_PROF] == 1) {

        if (prof_focus(ch) > combat_focus(ch) && prof_focus(ch) > arcane_focus(ch)) {
          if (show == TRUE)
          send_to_char("That society does not primary professionals.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_PROF] == 1) {
        if (prof_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow professionals.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_VAMPIRES] == 1 && is_vampire(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow vampires.\n\r", display);

        return FALSE;
      }
      if (fac->restrictions[RESTRICT_WEREWOLVES] == 1 && is_werewolf(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow werewolves.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_FAEBORN] == 1 && is_faeborn(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow faeborn.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_DEMONBORN] == 1 && is_demonborn(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow demonborn.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_ANGELBORN] == 1 && is_angelborn(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow angelborn.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_DEMIGOD] == 1 && is_demigod(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow demigods.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_SUPERNATURALS] == 1 && seems_super(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow supernaturals.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_NATURALS] == 1 && !seems_super(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow naturals.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_NONVIRGINS] == 1 && safe_strlen(ch->pcdata->last_sexed[0]) > 1 && ch->pcdata->attract[ATTRACT_PROM] > 80) {
        if (show == TRUE)
        send_to_char("That society does not allow nonvirgins.\n\r", display);

        return FALSE;
      }
      if (fac->restrictions[RESTRICT_MURDERVAMPS] == 1) {
        if (is_werewolf(ch) && ch->pcdata->lastshift > current_time - (3600 * 24 * 7 * 10)) {
          if (show == TRUE)
          send_to_char("That society does not allow murderers.\n\r", display);
          return FALSE;
        }
        if (is_vampire(ch) && !animal_feeder(ch)) {
          if (show == TRUE)
          send_to_char("That society does not allow murderers.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_FRATENIZATION] == 1) {
        for (int i = 0; i < 100; i++) {
          if (safe_strlen(fac->member_names[i]) > 1 && !str_cmp(fac->member_names[i], ch->pcdata->last_sexed[0]) && ch->pcdata->attract[ATTRACT_PROM] > 80) {
            if (show == TRUE)
            send_to_char("That society does not allow fraternization.\n\r", display);
            return FALSE;
          }
        }
      }
      if (fac->restrictions[RESTRICT_OUTOFWEDLOCK] == 1 && safe_strlen(ch->pcdata->last_sexed[0]) > 1 && ch->pcdata->attract[ATTRACT_PROM] > 80) {
        bool found = FALSE;
        for (int i = 0; i < 10; i++) {
          if (ch->pcdata->relationship_type[i] == REL_SPOUSE && !str_cmp(ch->pcdata->last_sexed[0], ch->pcdata->relationship[i]))
          found = TRUE;
        }
        if (found == FALSE) {
          if (show == TRUE)
          send_to_char("That society does not allow extramarital sex.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_HOMOSEXUAL] == 1 && safe_strlen(ch->pcdata->last_sexed[0]) > 1 && ch->pcdata->attract[ATTRACT_PROM] > 80) {
        if (get_char_world_pc(ch->pcdata->last_sexed[0]) != NULL && get_char_world_pc(ch->pcdata->last_sexed[0])->sex == ch->sex) {
          if (show == TRUE)
          send_to_char("That society does not allow homosexuality.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_INHIBITED] == 1 && (is_vampire(ch) && ch->pcdata->habit[HABIT_FEEDING] != 2)) {
        if (show == TRUE)
        send_to_char("That society does not inhibited vampires.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_INHIBITED] == 1 && (is_werewolf(ch) && ch->pcdata->habit[HABIT_LUNACY] != 3)) {
        if (show == TRUE)
        send_to_char("That society does not inhibited werewolves.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_INHIBITED] == 1 && (is_demonborn(ch) && ch->pcdata->habit[HABIT_SADISM] < 3)) {
        if (show == TRUE)
        send_to_char("That society does not inhibited demonborn.\n\r", display);
        return FALSE;
      }
    }

    return TRUE;
  }

  bool faction_elligible(CHAR_DATA *ch, FACTION_TYPE *fac, bool show, CHAR_DATA *display) {
    bool unset = FALSE;
    for (int i = 1; i <= AXES_MAX; i++) {
      if (fac->axes[i] == 0)
      unset = TRUE;
    }
    if (unset == TRUE)
    return TRUE;

    if (in_fight(ch) || battleground(ch->in_room))
    return TRUE;

    if ((get_real_age(ch) < 21 && get_age(ch) < 21) && (is_super(ch) || seems_super(ch)) && fac->axes[AXES_SUPERNATURAL] < 3 && fac->axes[AXES_SUPERNATURAL] != 0 && fac->college == 0) {
      if (show == TRUE)
      send_to_char("Too young for a supernatural society.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 4 && fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 4 && fac->axes[AXES_CORRUPT] >= AXES_MIDRIGHT) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }
    if (get_tier(ch) >= 3 && fac->axes[AXES_CORRUPT] >= AXES_FARRIGHT && ch->skills[SKILL_MENTALDISCIPLINE] <= 0) {
      if (show == TRUE)
      send_to_char("Too high tier.\n\r", display);
      return FALSE;
    }

    if (fac->restrictions[RESTRICT_MALE] == 1 && ch->sex == SEX_MALE && ch->pcdata->sexchange_time <= 0) {
      if (show == TRUE)
      send_to_char("That society does not allow men.\n\r", display);
      return FALSE;
    }

    if (fac->restrictions[RESTRICT_FEMALE] == 1 && ch->sex == SEX_FEMALE && ch->pcdata->sexchange_time <= 0) {
      if (show == TRUE)
      send_to_char("That society does not allow women.\n\r", display);
      return FALSE;
    }

    if (event_cleanse == 0) {
      if (fac->restrictions[RESTRICT_PRIMARY_ARCANE] == 1) {

        if (arcane_focus(ch) > combat_focus(ch) && arcane_focus(ch) > prof_focus(ch)) {
          if (show == TRUE)
          send_to_char("That society does not allow primary arcanists.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_ARCANE] == 1) {
        if (arcane_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow arcanists.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_PRIMARY_COMBAT] == 1) {
        if (combat_focus(ch) > arcane_focus(ch) && combat_focus(ch) > prof_focus(ch)) {
          if (show == TRUE)
          send_to_char("That society does not allow primary commbatants.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_COMBAT] == 1) {
        if (combat_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow combatants.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_COMBAT] == 1) {
        if (combat_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow combatants.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_PRIMARY_PROF] == 1) {
        if (prof_focus(ch) > combat_focus(ch) && prof_focus(ch) > arcane_focus(ch)) {
          if (show == TRUE)
          send_to_char("That society does not primary professionals.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_ANY_PROF] == 1) {
        if (prof_focus(ch) > 0) {
          if (show == TRUE)
          send_to_char("That society does not allow professionals.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_VAMPIRES] == 1 && is_vampire(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow vampires.\n\r", display);

        return FALSE;
      }
      if (fac->restrictions[RESTRICT_WEREWOLVES] == 1 && is_werewolf(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow werewolves.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_FAEBORN] == 1 && is_faeborn(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow faeborn.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_DEMONBORN] == 1 && is_demonborn(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow demonborn.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_ANGELBORN] == 1 && is_angelborn(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow angelborn.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_DEMIGOD] == 1 && is_demigod(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow demigods.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_SUPERNATURALS] == 1 && seems_super(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow supernaturals.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_NATURALS] == 1 && !seems_super(ch)) {
        if (show == TRUE)
        send_to_char("That society does not allow naturals.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_NONVIRGINS] == 1 && safe_strlen(ch->pcdata->last_sexed[0]) > 1 && ch->pcdata->attract[ATTRACT_PROM] > 80) {
        if (show == TRUE)
        send_to_char("That society does not allow nonvirgins.\n\r", display);

        return FALSE;
      }
      if (fac->restrictions[RESTRICT_MURDERVAMPS] == 1) {
        if (is_werewolf(ch) && ch->pcdata->lastshift > current_time - (3600 * 24 * 7 * 10)) {
          if (show == TRUE)
          send_to_char("That society does not allow murderers.\n\r", display);
          return FALSE;
        }
        if (is_vampire(ch) && !animal_feeder(ch)) {
          if (show == TRUE)
          send_to_char("That society does not allow murderers.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_FRATENIZATION] == 1) {
        for (int i = 0; i < 100; i++) {
          if (safe_strlen(fac->member_names[i]) > 1 && !str_cmp(fac->member_names[i], ch->pcdata->last_sexed[0]) && ch->pcdata->attract[ATTRACT_PROM] > 80) {
            if (show == TRUE)
            send_to_char("That society does not allow fraternization.\n\r", display);
            return FALSE;
          }
        }
      }
      if (fac->restrictions[RESTRICT_OUTOFWEDLOCK] == 1 && safe_strlen(ch->pcdata->last_sexed[0]) > 1 && ch->pcdata->attract[ATTRACT_PROM] > 80) {
        bool found = FALSE;
        for (int i = 0; i < 10; i++) {
          if (ch->pcdata->relationship_type[i] == REL_SPOUSE && !str_cmp(ch->pcdata->last_sexed[0], ch->pcdata->relationship[i]))
          found = TRUE;
        }
        if (found == FALSE) {
          if (show == TRUE)
          send_to_char("That society does not allow extramarital sex.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_HOMOSEXUAL] == 1 && safe_strlen(ch->pcdata->last_sexed[0]) > 1 && ch->pcdata->attract[ATTRACT_PROM] > 80) {
        if (get_char_world_pc(ch->pcdata->last_sexed[0]) != NULL && get_char_world_pc(ch->pcdata->last_sexed[0])->sex == ch->sex) {
          if (show == TRUE)
          send_to_char("That society does not allow homosexuality.\n\r", display);
          return FALSE;
        }
      }
      if (fac->restrictions[RESTRICT_INHIBITED] == 1 && (is_vampire(ch) && ch->pcdata->habit[HABIT_FEEDING] != 2)) {
        if (show == TRUE)
        send_to_char("That society does not inhibited vampires.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_INHIBITED] == 1 && (is_werewolf(ch) && ch->pcdata->habit[HABIT_LUNACY] != 3)) {
        if (show == TRUE)
        send_to_char("That society does not inhibited werewolves.\n\r", display);
        return FALSE;
      }
      if (fac->restrictions[RESTRICT_INHIBITED] == 1 && (is_demonborn(ch) && ch->pcdata->habit[HABIT_SADISM] < 3)) {
        if (show == TRUE)
        send_to_char("That society does not inhibited demonborn.\n\r", display);
        return FALSE;
      }
    }

    return TRUE;
  }

  int average_position(int position, FACTION_TYPE *fac) {
    int total = 0;
    int count = 0;

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->stasis == 1)
      continue;
      if ((*it)->antagonist == 1 || (*it)->nopart == 1)
      continue;
      if ((*it)->type != fac->type)
      continue;

      if ((*it)->axes[position] != 0) {
        total += (*it)->axes[position] * fac_power(*it);
        count += (UMAX(1, fac_power(*it)));
      }
    }
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->stasis == 1)
      continue;
      if ((*it)->antagonist == 1 || (*it)->nopart == 1)
      continue;

      if ((*it)->axes[position] != 0) {
        total += (*it)->axes[position] * fac_power(*it);
        count += (UMAX(1, fac_power(*it)));
      }
    }

    return (int)(total / UMAX(count, 1));
  }

  bool underdog(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    int leftcount = 0;
    int rightcount = 0;
    int midcount = 0;
    if (ch->faction == 0 || clan_lookup(ch->faction) == NULL)
    return FALSE;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING && d->character->in_room != NULL) {
        victim = d->character;
        int vict = get_tier(victim);
        if (vict > 1)
        vict++;
        if (victim->faction != 0 && clan_lookup(victim->faction) != NULL) {
          if (clan_lookup(victim->faction)->alliance == ALLIANCE_SIDELEFT)
          leftcount += vict;
          else if (clan_lookup(victim->faction)->alliance == ALLIANCE_SIDERIGHT)
          rightcount += vict;
          else if (clan_lookup(victim->faction)->alliance == ALLIANCE_SIDEMID)
          midcount += vict;
        }
      }
    }
    if (clan_lookup(ch->faction)->alliance == ALLIANCE_SIDELEFT && leftcount < rightcount && leftcount < midcount)
    return TRUE;
    if (clan_lookup(ch->faction)->alliance == ALLIANCE_SIDERIGHT && rightcount < leftcount && rightcount < midcount)
    return TRUE;
    if (clan_lookup(ch->faction)->alliance == ALLIANCE_SIDEMID && midcount < rightcount && midcount < leftcount)
    return TRUE;
    return FALSE;
  }

  int alliance_position(FACTION_TYPE *fac) {
    int leftpower = 0;
    int midpower = 0;
    int rightpower = 0;

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->nopart == 1 || (*it)->type != fac->type)
      continue;
      if ((*it)->alliance == ALLIANCE_SIDELEFT)
      leftpower += (*it)->lifeearned;
      if ((*it)->alliance == ALLIANCE_SIDEMID)
      midpower += (*it)->lifeearned;
      if ((*it)->alliance == ALLIANCE_SIDERIGHT)
      rightpower += (*it)->lifeearned;
    }
    if (fac->alliance == ALLIANCE_SIDELEFT) {
      if (leftpower > midpower && leftpower > rightpower)
      return 1;
      if (leftpower <= midpower && leftpower <= rightpower)
      return 3;
      return 2;
    }
    if (fac->alliance == ALLIANCE_SIDERIGHT) {
      if (rightpower > midpower && rightpower > leftpower)
      return 1;
      if (rightpower <= midpower && rightpower <= leftpower)
      return 3;
      return 2;
    }
    if (fac->alliance == ALLIANCE_SIDEMID) {
      if (midpower > leftpower && midpower > rightpower)
      return 1;
      if (midpower <= leftpower && midpower <= rightpower)
      return 3;
      return 2;
    }
    return 2;
  }

  int faction_secrecy(FACTION_TYPE *fac, CHAR_DATA *report) {
    int start = 980;
    if (fac->axes[AXES_CORRUPT] == AXES_FARRIGHT || fac->axes[AXES_CORRUPT] == AXES_FARLEFT)
    start += 40;
    if (fac->axes[AXES_CORRUPT] <= AXES_NEARLEFT)
    start += 20;

    if(fac->type == FACTION_CULT)
    {
      if(fac->axes[AXES_CORRUPT] >= AXES_NEUTRAL)
      start -= 100;
    }
    if(fac->type == FACTION_SECT)
    {
      if(fac->axes[AXES_CORRUPT] >= AXES_NEARRIGHT)
      start += 200;
    }


    if (safe_strlen(fac->missions) > 2)
    start += 10;
    if (safe_strlen(fac->manifesto) > 2)
    start += 10;

    if (!generic_faction_vnum(fac->vnum))
    start += 50;

    if (report != NULL)
    printf_to_char(report, "Begin at: %d\n\r", start);

    int pos = alliance_position(fac);
    if (report != NULL)
    printf_to_char(report, "Resource position: %d\n\r", pos);
    if (pos == 1)
    start -= 50;
    else if (pos == 2)
    start += 50;
    else if (pos == 3)
    start += 100;

    if (fac->closed == 0)
    start += 50;
    if (fac->cardinal == 1)
    start += 250;
    if (fac->outcast == 1)
    start -= 100;
    if (safe_strlen(fac->leader) < 2)
    start -= 200;
    if (report != NULL)
    printf_to_char(report, "Post standings: %d\n\r", start);

    int membercount = 0;
    int totaltier = 0;
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(fac->member_names[i]) > 1) {
        if (fac->member_flags[i] == 0) {
          membercount++;
          totaltier += fac->member_tier[i];
        }
      }
    }

    int avtier = totaltier * 100 / UMAX(1, membercount);
    int countcost = 0;
    if (membercount > 12 && !generic_faction_vnum(fac->vnum)) {
      int cost = 30;
      int extramembers = membercount - 12;
      for (; extramembers > 0; extramembers--) {
        if (fac->staff == 1) {
          start -= cost * 2;
          countcost += cost * 2;
        }
        else {
          start -= cost;
          countcost += cost;
        }

        cost *= 2;
      }
      if (report != NULL)
      printf_to_char(report, "Minus %d%% for number of members.(%d)\n\r", countcost / 10, start);
    }
    int exmember = 0;
    for (int i = 0; i < 100; i++) {
      if (safe_strlen(fac->exmember_names[i]) > 1 && fac->exmember_inactivity[i] < 1500) {
        if (fac->exmember_quit[i] == 1)
        exmember += 5;
        else
        exmember += 1;
      }
    }
    if (exmember > 0) {
      start -= exmember;
      if (report != NULL)
      printf_to_char(report, "Minus %d%% for exmembers.(%d)\n\r", exmember / 10, start);
    }
    avtier = 250 - avtier;
    avtier = UMIN(avtier, 250 / 3);
    start += avtier / 2;
    if (report != NULL)
    printf_to_char(report, "%d%% for average tier.\n\r", avtier / 20);

    int apower = 0;
    int totpower = 0;
    int sidecount = 0;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->nopart == 1 || (*it)->type != fac->type)
      continue;
      if ((*it)->alliance == fac->alliance)
      apower += fac_power((*it));
      else
      sidecount++;
      totpower += fac_power((*it));
    }

    int powmult = (100 * apower) / totpower;
    int buf = 40 - powmult;
    if(buf < 0)
    {
      buf *= -1;
      double sbuf = (double)buf;
      sbuf = cbrt(sbuf*sbuf);
      buf = (int)(sbuf*10);
      buf *= -1;
    }
    else
    {
      double sbuf = (double)buf;
      sbuf = cbrt(sbuf*sbuf);
      buf = (int)(sbuf*10);
    }
    if(buf < 0)
    buf = UMAX(buf, sidecount*-20);
    if(buf > 0)
    buf = UMIN(buf, 20);

    if(sidecount == 0)
    buf = buf + 15;

    start += buf*10;
    if (report != NULL)
    printf_to_char(report, "%d%% for alliance power.\n\r", buf);

    int posbonus = 0;
    for (int i = 1; i <= AXES_MAX; i++) {
      int average = average_position(i, fac);
      int diff = UMAX(average - fac->axes[i], fac->axes[i] - average);
      if (report != NULL && IS_IMMORTAL(report)) {
        if (i == AXES_SUPERNATURAL)
        printf_to_char(
        report, "Supernatural i:%d, Av:%d, pos: %d, diff:%d, mult:%d\n\r", i, average, fac->axes[i], diff, (diff * diff * 10));
        else if (i == AXES_MATERIAL)
        printf_to_char(report, "Material i:%d, Av:%d, pos: %d, diff:%d, mult:%d\n\r", i, average, fac->axes[i], diff, (diff * diff * 10));
        else if (i == AXES_COMBAT)
        printf_to_char(report, "Combative i:%d, Av:%d, pos: %d, diff:%d, mult:%d\n\r", i, average, fac->axes[i], diff, (diff * diff * 10));
        else if (i == AXES_DEMOCRATIC)
        printf_to_char(report, "Democratic i:%d, Av:%d, pos: %d, diff:%d, mult:%d\n\r", i, average, fac->axes[i], diff, (diff * diff * 10));
        else if (i == AXES_CORRUPT)
        printf_to_char(report, "Corrupt i:%d, Av:%d, pos: %d, diff:%d, mult:%d\n\r", i, average, fac->axes[i], diff, (diff * diff * 10));
        else if (i == AXES_ANARCHY)
        printf_to_char(report, "Anarchist i:%d, Av:%d, pos: %d, diff:%d, mult:%d\n\r", i, average, fac->axes[i], diff, (diff * diff * 10));
      }
      if (diff == 0)
      posbonus -= 20;
      else
      posbonus += diff * diff * 10;
    }
    posbonus -= 125;
    posbonus = UMIN(posbonus, 350);
    if (generic_faction_vnum(fac->vnum))
    posbonus /= 2;

    start += posbonus;
    if (report != NULL)
    printf_to_char(report, "%d%% for positions.(%d)\n\r", posbonus / 10, start);

    bool ident = FALSE;
    bool earlier = FALSE;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->stasis == 1)
      continue;
      if ((*it)->vnum == fac->vnum)
      continue;
      if((*it)->antagonist == 1)
      continue;

      ident = TRUE;
      earlier = FALSE;
      if ((*it)->position_time < fac->position_time)
      earlier = TRUE;
      for (int i = 1; i < AXES_MAX; i++) {
        if (fac->axes[i] != 0 && fac->axes[i] != (*it)->axes[i])
        ident = FALSE;
      }
      if (ident == TRUE) {
        if (earlier == TRUE && start >= 500) {
          start -= 300;
          if (report != NULL)
          send_to_char("-30% for duplicate positions.\n\r", report);
        }
        else if (start >= 500) {
          start -= 100;
          if (report != NULL)
          send_to_char("-10% for duplicate positions.\n\r", report);
        }
      }
    }
    int count = 0;
    int restrictbonus = 0;
    for (int i = 0; i <= RESTRICT_MAX; i++) {
      if (fac->restrictions[i] == 1) {
        bool found = FALSE;
        earlier = FALSE;
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->stasis == 1)
          continue;
          if ((*it)->vnum == fac->vnum)
          continue;
          if ((*it)->restrictions[i] == 1) {
            found = TRUE;
            if ((*it)->restrict_time < fac->restrict_time)
            earlier = TRUE;
          }
        }
        count++;
        if (!found)
        restrictbonus += 20;
        else if (!earlier) {
          if (count < 6) {
            if (fac->soft_restrict == 1)
            restrictbonus += 8;
            else
            restrictbonus += 10;
          }
        }
        else {
          if (count < 6) {
            if (fac->soft_restrict == 1)
            restrictbonus += 5;
            else
            restrictbonus += 8;
          }
        }

        if (i == RESTRICT_ANY_ARCANE)
        restrictbonus += 15;
        if (i == RESTRICT_ANY_PROF)
        restrictbonus += 5;
        if (i == RESTRICT_ANY_COMBAT)
        restrictbonus += 15;
        if (i == RESTRICT_SUPERNATURALS)
        restrictbonus += 15;
        if (i == RESTRICT_NATURALS)
        restrictbonus += 5;
      }
    }
    restrictbonus = UMIN(restrictbonus, 250);
    start += restrictbonus;
    if (report != NULL)
    printf_to_char(report, "%d%% from restrictions.(%d)\n\r", restrictbonus / 10, start);

    int bases = 0;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      //	if(border_territory((*it)))
      //	    continue;
      if ((*it)->base_faction_core == fac->vnum || (*it)->base_faction_cult == fac->vnum || (*it)->base_faction_sect == fac->vnum) {
        bases += 1;
        //	    start += 50;
        if (report != NULL)
        printf_to_char(report, "Plus 1%% from faction bases(%d): %s\n\r", start, (*it)->name);
      }
    }
    if (bases > 0) {
      start -= (int)(50.0 * sqrt(bases));
    }
    if(report != NULL)
    printf_to_char(report, "Precrunch: %d\n\r", start);

    int cmult = 5;
    if(fac->type == FACTION_CORE)
    cmult = 3;

    if(start > 1000)
    {
      float diff = start - 1000;
      diff = diff * diff;
      float crunch = cbrt(diff);
      int icrunch = (int)(crunch);
      start = 1000 + icrunch*cmult;
      if(report != NULL)
      printf_to_char(report, "Diff: %d, Icrunch: %d\n\r", (int)(diff), icrunch);
    }
    else if(start < 1000)
    {
      float diff = 1000 - start;
      diff = diff * diff;
      float crunch = cbrt(diff);
      int icrunch = (int)(crunch);
      start = 1000 - icrunch*cmult;
      if(report != NULL)
      printf_to_char(report, "Diff: %d, Icrunch: %d\n\r", (int)(diff), icrunch);

    }

    start /= 20;
    start *= 20;

    return UMAX(0, start);
  }

  int char_secrecy(CHAR_DATA *ch, CHAR_DATA *report) {
    int base;
    if (get_tier(ch) == 1) {
      base = 1000;
    }
    else if (get_tier(ch) == 2) {
      base = 1000;
    }
    else if (get_tier(ch) == 3) {
      base = 900;
    }
    else if (get_tier(ch) == 4)
    base = 800;
    else if (get_tier(ch) == 5)
    base = 700;

    if (report != NULL)
    printf_to_char(report, "Base: %d\n\r", base / 10);

    if (ch->pcdata->job_type_one == JOB_EMPLOYEE)
    base -= 100;
    if (!is_super(ch))
    base += 50;

    base -= ((int)(sqrt(ch->pcdata->attract[ATTRACT_PROM])) / 3);

    int lf = get_lifeforce(ch, TRUE, NULL);
    if (ch->lf_taken > 0)
    lf += ch->lf_taken / 100;
    lf = UMIN(lf, 120);
    lf = UMAX(lf, 80);
    if (report != NULL)
    printf_to_char(report, "Pre-LF %d\n\r", base / 10);

    base = base * (lf + 50) / 150;

    if (report != NULL)
    printf_to_char(report, "Pre-Reduce %d\n\r", base / 10);

    if (base > 1000) {
      base -= 1000;
      base /= 2;
      base += 1000;
    }
    else {
      base -= 1000;
      base = base * 2 / 3;
      base += 1000;
    }

    return UMAX(0, base);
  }

  void use_resources(int amount, int faction, CHAR_DATA *ch, char *message) {
    char buf[MSL];
    if(amount <= 0)
    return;
    FACTION_TYPE *fac = clan_lookup(faction);
    if (fac == NULL)
    return;
    int init = fac->resource;

    int facsecret = faction_secrecy(fac, NULL);
    int facmod = 1000 - facsecret; // 200
    int start = amount;

    if (facmod > 0)
    amount = amount + amount * facmod / 400;
    else {
      for (; facmod < -400;) {
        facmod += 400;
        amount = amount * 2 / 3;
      }
      amount = amount + amount * facmod / 800;
    }

    int prechar = amount;
    int chmod = 0;
    int chsecret = 0;
    if (ch != NULL) {
      chsecret = char_secrecy(ch, NULL);
      chmod = 1000 - chsecret; // 200
      if (chmod > 0)
      amount = amount + amount * chmod / 600;
      else {
        for (; chmod < -800;) {
          facmod += 800;
          amount = amount * 2 / 3;
        }
        amount = amount + amount * facmod / 1600;
      }
      ch->pcdata->spent_resources += start * 2;
    }
    amount = UMAX(amount, start * 2 / 3);
    if (amount == 0)
    return;
    fac->resource -= amount;
    if (amount >= 5) {
      if (ch != NULL)
      sprintf(
      buf, "Resource Use: %s for %s from %s. Start: %d, Secret: %d, Mod: %d, Multi: %d, Prechar: %d, Secret: %d, Mod %d, Multi: %d, End: %d", fac->name, message, ch->name, start, facsecret, facmod, start * facmod / 200, prechar, chsecret, chmod, prechar * chmod / 200, amount);
      else
      sprintf(buf, "Resource USE: %s for %s from nobody. Start: %d, Secret: %d, Mod: %d, Multi: %d, End: %d", fac->name, message, start, facsecret, facmod, start * facmod / 200, amount);
      log_string(buf);
    }
    if (safe_strlen(message) > 1) {
      sprintf(buf, "Someone spends $%d resources on %s.", amount * 10, message);
      if (amount <= 75)
      send_brief_log(faction, buf);
      else
      send_log(faction, buf);
    }
    if (init > 6000 && fac->resource < 4000)
    fac->resource = 4000;
  }
  void gain_resources(int amount, int faction, CHAR_DATA *ch, char *message) {
    char buf[MSL];
    FACTION_TYPE *fac;


    bool immune_dim = FALSE;
    if(!str_cmp(message, "treasure from the Wilds"))
    immune_dim = TRUE;


    if(ch != NULL && IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_OPERATIVE)
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

      gain_resources(amount, faction, victim, message);

      save_char_obj(victim, FALSE, FALSE);

      if (!online)
      free_char(victim);
      return;

    }
    //    if(faction == ch->faction && ch->factiontrue > -1 && //    clan_lookup(ch->factiontrue) != NULL)
    //        fac = clan_lookup(ch->factiontrue);
    //    else
    fac = clan_lookup(faction);

    if (fac == NULL)
    return;
    if (ch != NULL && IS_NPC(ch))
    return;



    if(fac->type == FACTION_CULT || fac->type == FACTION_SECT)
    {
      if(strlen(fac->eidilon) < 2 || !character_exists(fac->eidilon))
      return;
    }
    int vstart = amount;
    int start;
    int tamount = amount + fac->weekly_resources;
    int tadd = 0;
    float mult = 1.0;
    if(tamount >= 500)
    {
      int remamount = tamount;
      while(remamount > 0) {
        tadd += UMIN(500, remamount)*mult;
        remamount -= UMIN(500, remamount);
        mult = mult * 4/5;
      }
    }
    else
    tadd = tamount;

    if(immune_dim == FALSE)
    {
      amount = tadd - fac->weekly_resources;
      amount = UMAX(amount, vstart/10);

      start = amount;
      fac->weekly_resources += amount;
    }
    else
    {
      amount = vstart;
      start = amount;
    }
    bool highintel = FALSE;
    if (ch != NULL && ch->pcdata->intel >= 10000 && vstart >= 100 && fac->last_high_intel < (current_time - (3600 * 24 * 5))) {
      fac->last_high_intel = current_time;
      sprintf(buf, "You gain a high intel contribution from %s", ch->name);
      send_log(faction, buf);
      highintel = TRUE;
    }

    if(safe_strlen(message) > 3 && vstart >= 25 && ch != NULL)
    {
      if(ch->fcore == faction)
      {
        if(ch->fcult > 0)
        {
          FACTION_TYPE *cult = clan_lookup(ch->fcult);
          if(cult != NULL)
          {
            gain_resources(amount*4/10, cult->vnum, ch, "");
            if(highintel == TRUE && cult->last_high_intel < current_time - (3600 * 24 * 5))
            {
              cult->last_high_intel = current_time;
              sprintf(buf, "You gain a high intel contribution from %s", ch->name);
              send_log(cult->vnum, buf);

            }
          }
        }
        if(ch->fsect > 0)
        {
          FACTION_TYPE *sect = clan_lookup(ch->fsect);
          if(sect != NULL)
          {
            gain_resources(amount*4/10, sect->vnum, ch, "");
            if(highintel == TRUE  && sect->last_high_intel < current_time - (3600 * 24 * 5))
            {
              sect->last_high_intel = current_time;
              sprintf(buf, "You gain a high intel contribution from %s", ch->name);
              send_log(sect->vnum, buf);
            }
          }
        }
      }
      else if(ch->fsect == faction)
      {
        if(ch->fcult > 0)
        {
          FACTION_TYPE *cult = clan_lookup(ch->fcult);
          if(cult != NULL)
          {
            gain_resources(amount*4/10, cult->vnum, ch, "");
            if(highintel == TRUE && cult->last_high_intel < current_time - (3600 * 24 * 5))
            {
              cult->last_high_intel = current_time;
              sprintf(buf, "You gain a high intel contribution from %s", ch->name);
              send_log(cult->vnum, buf);

            }
          }
        }
        if(ch->fcore > 0)
        {
          FACTION_TYPE *sect = clan_lookup(ch->fcore);
          if(sect != NULL)
          {
            gain_resources(amount*4/10, sect->vnum, ch, "");
            if(highintel == TRUE  && sect->last_high_intel < current_time - (3600 * 24 * 5))
            {
              sect->last_high_intel = current_time;
              sprintf(buf, "You gain a high intel contribution from %s", ch->name);
              send_log(sect->vnum, buf);

            }

          }
        }
      }
      else if(ch->fcult == faction)
      {
        if(ch->fcore > 0)
        {
          FACTION_TYPE *cult = clan_lookup(ch->fcore);
          if(cult != NULL)
          {
            gain_resources(amount*4/10, cult->vnum, ch, "");
            if(highintel == TRUE && cult->last_high_intel < current_time - (3600 * 24 * 5))
            {
              cult->last_high_intel = current_time;
              sprintf(buf, "You gain a high intel contribution from %s", ch->name);
              send_log(cult->vnum, buf);

            }
          }
        }
        if(ch->fcore > 0)
        {
          FACTION_TYPE *sect = clan_lookup(ch->fcore);
          if(sect != NULL)
          {
            gain_resources(amount*4/10, sect->vnum, ch, "");
            if(highintel == TRUE && sect->last_high_intel < current_time - (3600 * 24 * 5))
            {
              sect->last_high_intel = current_time;
              sprintf(buf, "You gain a high intel contribution from %s", ch->name);
              send_log(sect->vnum, buf);
            }

          }
        }
      }
    }


    if (fac->last_high_intel > current_time - (3600 * 24 * 2))
    {
      amount *= 2;
      if (ch != NULL)
      send_to_char("Your contact mentions goods are easier to move due to a recent high intel contribution.\n\r", ch);
    }
    else if(fac->last_high_intel < current_time - (3600 * 24 * 14) && fac->last_intel < current_time - (3600 * 24 * 14))
    {
      amount = amount / 2;
      if (ch != NULL)
      send_to_char("Your contact mentions goods are harder to move due to no recent high intel contribution or intel gathered.\n\r", ch);
    }

    if (ch != NULL && ch->pcdata->pending_resources >= amount) {
      ch->pcdata->pending_resources -= amount;
      amount *= 2;
    }
    if (ch != NULL)
    give_intel(ch, UMIN(150, vstart / 4));

    //    amount *= 2;
    //    if(generic_faction_vnum(fac->vnum))
    //	amount = amount *2/3;


    if (ch != NULL) {
      if (ch->pcdata->intel >= 10000 && amount >= 100)
      {
        amount = amount * 6 / 5;
        ch->pcdata->intel -= 100;
      }
    }


    int facsecret = faction_secrecy(fac, NULL);
    int facmod = 1000 - facsecret; // 200
    int chsecret = 0;
    int chmod = 0;
    fac->award_progress += amount / 3;
    if (facsecret == 0)
    amount /= 100;
    else {
      if (facmod > 0) {
        for (; facmod > 200;) {
          amount = amount * 3 / 4;
          facmod -= 200;
        }
        amount = amount - amount * facmod / 400;
      }
      else
      amount = amount - amount * facmod / 600;
    }
    int prechar = amount;

    if (ch != NULL) {
      chsecret = char_secrecy(ch, NULL);
      if (chsecret == 0)
      amount /= 100;
      else {
        chmod = 1000 - chsecret; // 200
        if (chmod > 0) {
          for (; chmod > 200;) {
            amount = amount * 2 / 3;
            chmod -= 200;
          }
          amount = amount - amount * chmod / 300;
        }
        else
        amount = amount - amount * chmod / 300;
      }
      ch->pcdata->spent_resources += start;
    }
    amount = UMIN(amount, start * 3 / 2);
    if (amount == 0)
    return;
    if (amount >= 5) {
      if (ch != NULL)
      sprintf(
      buf, "Resource Gain: %s for %s from %s. Start: %d, Secret: %d, Mod: %d, Multi: %d, Prechar: %d, Secret: %d, Mod %d, Multi: %d, End: %d", fac->name, message, ch->name, start, facsecret, facmod, start * facmod / 200, prechar, chsecret, chmod, prechar * chmod / 200, amount);
      else
      sprintf(buf, "Resource Gain: %s for %s from nobody. Start: %d, Secret: %d, Mod: %d, Multi: %d, End: %d", fac->name, message, start, facsecret, facmod, start * facmod / 200, amount);
      log_string(buf);
    }
    fac->resource += amount;
    if (ch != NULL) {
      if (strcasestr(message, "contributing influence") != NULL)
      fac->lifeearned += amount / 2;
      else
      fac->lifeearned += amount;
    }
    if (fac->award_progress >= 2000) {
      fac->awards++;
      if (fac->awards > 2)
      fac->awards = 2;
      fac->award_progress -= 1000;
    }

    if (safe_strlen(message) > 1) {
      sprintf(buf, "You gain $%d resources from %s", amount * 10, message);
      if (amount <= 75)
      send_brief_log(faction, buf);
      else
      send_log(faction, buf);
    }
  }

  int manip_cost(FACTION_TYPE *fac, int origcost) {
    int cost = origcost;
    if (fac->axes[AXES_COMBAT] == AXES_FARLEFT)
    cost = cost * 150 / 100;
    else if (fac->axes[AXES_COMBAT] == AXES_MIDLEFT)
    cost = cost * 130 / 100;
    else if (fac->axes[AXES_COMBAT] == AXES_NEARLEFT)
    cost = cost * 110 / 100;
    else if (fac->axes[AXES_COMBAT] == AXES_FARRIGHT)
    cost = cost * 50 / 100;
    else if (fac->axes[AXES_COMBAT] == AXES_MIDRIGHT)
    cost = cost * 70 / 100;
    else if (fac->axes[AXES_COMBAT] == AXES_NEARRIGHT)
    cost = cost * 90 / 100;
    return cost;
  }

  int battle_world() {
    if (activeoperation == NULL)
    return WORLD_EARTH;
    LOCATION_TYPE *loc = territory_by_number(activeoperation->territoryvnum);
    if (loc == NULL)
    return WORLD_EARTH;
    if (loc->continent == CONTINENT_WILDS)
    return WORLD_WILDS;
    if (loc->continent == CONTINENT_OTHER)
    return WORLD_OTHER;
    if (loc->continent == CONTINENT_GODREALM)
    return WORLD_GODREALM;
    if (loc->continent == CONTINENT_HELL)
    return WORLD_HELL;
    return WORLD_EARTH;
  }

  _DOFUN(do_newalliance) {
    new_alliance(FACTION_CULT);
    new_alliance(FACTION_SECT);
  }

  void op_report(char *argument, CHAR_DATA *ch) {

    if (activeoperation == NULL)
    return;

    if (activeoperation->competition == COMPETE_CLOSED)
    return;

    char buf[MSL];
    sprintf(buf, "%s's operation in %s", clan_lookup(activeoperation->faction)->name, operation_location(activeoperation));

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->antagonist == 1 || (*it)->outcast == 1 || (*it)->attributes[FACTION_SCOUTS] == 0)
      continue;
      FACTION_TYPE *fac = *it;

      if (!str_cmp(fac->reportone_title, buf) && fac->reportone_time > (current_time - (3600 * 2))) {
        char logs[MSL];
        if(ch != NULL && !IS_NPC(ch) && !is_gm(ch))
        {
          bool found = FALSE;
          for(int i=0;i<50;i++)
          {
            if(!str_cmp(ch->name, fac->reportone_participants[i]))
            {
              found = TRUE;
              break;
            }
          }
          if(found == FALSE)
          {
            for(int i=0;i<50;i++)
            {
              if(fac->reportone_participants[i][0] == '\0')
              {
                free_string(fac->reportone_participants[i]);
                fac->reportone_participants[i] = str_dup(ch->name);
                break;
              }
            }
          }
        }
        if (safe_strlen(fac->reportone_text) > 25000) {
          int point = -1;
          for (int i = 0; i < 10; i++) {
            if (point == -1 && safe_strlen(fac->report_overflow[0][i]) < 25000)
            point = i;
          }
          if (point == -1)
          return;
          if (point == 0 && safe_strlen(fac->report_overflow[0][0]) < 5) {
            sprintf(logs, "%s\n\n`WSee society report battle 1 2 to continue.`x\n\r", fac->reportone_text);
            free_string(fac->reportone_text);
            fac->reportone_text = str_dup(logs);
          }
          sprintf(logs, "%s\n%s", fac->report_overflow[0][point], argument);
          free_string(fac->report_overflow[0][point]);
          fac->report_overflow[0][point] = str_dup(logs);
          if (safe_strlen(fac->report_overflow[0][point]) >= 25000) {
            sprintf(logs, "%s\n\n`WSee society report battle 1 %d to continue.`x\n\r", fac->report_overflow[0][point], point + 3);
            free_string(fac->report_overflow[0][point]);
            fac->report_overflow[0][point] = str_dup(logs);
          }
        }
        else {
          sprintf(logs, "%s\n%s", fac->reportone_text, argument);
          free_string(fac->reportone_text);
          fac->reportone_text = str_dup(logs);
        }
      }
      else if (!str_cmp(fac->reporttwo_title, buf) && fac->reporttwo_time > (current_time - (3600 * 2))) {
        char logs[MSL];

        if(ch != NULL && !IS_NPC(ch) && !is_gm(ch))
        {
          bool found = FALSE;
          for(int i=0;i<50;i++)
          {
            if(!str_cmp(ch->name, fac->reporttwo_participants[i]))
            {
              found = TRUE;
              break;
            }
          }
          if(found == FALSE)
          {
            for(int i=0;i<50;i++)
            {
              if(fac->reporttwo_participants[i][0] == '\0')
              {
                free_string(fac->reporttwo_participants[i]);
                fac->reporttwo_participants[i] = str_dup(ch->name);
                break;
              }
            }
          }
        }

        if (safe_strlen(fac->reporttwo_text) > 25000) {
          int point = -1;
          for (int i = 0; i < 10; i++) {
            if (point == -1 && safe_strlen(fac->report_overflow[1][i]) < 25000)
            point = i;
          }
          if (point == -1)
          return;
          if (point == 0 && safe_strlen(fac->report_overflow[1][0]) < 5) {
            sprintf(logs, "%s\n\n`WSee society report battle 2 2 to continue.`x\n\r", fac->reporttwo_text);
            free_string(fac->reporttwo_text);
            fac->reporttwo_text = str_dup(logs);
          }
          sprintf(logs, "%s\n%s", fac->report_overflow[1][point], argument);
          free_string(fac->report_overflow[1][point]);
          fac->report_overflow[1][point] = str_dup(logs);
          if (safe_strlen(fac->report_overflow[1][point]) >= 25000) {
            sprintf(logs, "%s\n\n`WSee society report battle 2 %d to continue.`x\n\r", fac->report_overflow[1][point], point + 3);
            free_string(fac->report_overflow[1][point]);
            fac->report_overflow[1][point] = str_dup(logs);
          }
        }
        else {
          sprintf(logs, "%s\n%s", fac->reporttwo_text, argument);
          free_string(fac->reporttwo_text);
          fac->reporttwo_text = str_dup(logs);
        }

      }
      else if (!str_cmp(fac->reportthree_title, buf) && fac->reportthree_time > (current_time - (3600 * 2))) {
        char logs[MSL];
        if(ch != NULL && !IS_NPC(ch) && !is_gm(ch))
        {
          bool found = FALSE;
          for(int i=0;i<50;i++)
          {
            if(!str_cmp(ch->name, fac->reportthree_participants[i]))
            {
              found = TRUE;
              break;
            }
          }
          if(found == FALSE)
          {
            for(int i=0;i<50;i++)
            {
              if(fac->reportone_participants[i][0] == '\0')
              {
                free_string(fac->reportthree_participants[i]);
                fac->reportthree_participants[i] = str_dup(ch->name);
                break;
              }
            }
          }
        }
        if (safe_strlen(fac->reportthree_text) > 25000) {
          int point = -1;
          for (int i = 0; i < 10; i++) {
            if (point == -1 && safe_strlen(fac->report_overflow[2][i]) < 25000)
            point = i;
          }
          if (point == -1)
          return;
          if (point == 0 && safe_strlen(fac->report_overflow[2][0]) < 5) {
            sprintf(logs, "%s\n\n`WSee society report battle 3 2 to continue.`x\n\r", fac->reportthree_text);
            free_string(fac->reportthree_text);
            fac->reportthree_text = str_dup(logs);
          }
          sprintf(logs, "%s\n%s", fac->report_overflow[2][point], argument);
          free_string(fac->report_overflow[2][point]);
          fac->report_overflow[2][point] = str_dup(logs);
          if (safe_strlen(fac->report_overflow[2][point]) >= 25000) {
            sprintf(logs, "%s\n\n`WSee society report battle 3 %d to continue.`x\n\r", fac->report_overflow[2][point], point + 3);
            free_string(fac->report_overflow[2][point]);
            fac->report_overflow[2][point] = str_dup(logs);
          }
        }
        else {
          sprintf(logs, "%s\n%s", fac->reportthree_text, argument);
          free_string(fac->reportthree_text);
          fac->reportthree_text = str_dup(logs);
        }
      }
      else if (activeoperation->speed >= 5 && clan_lookup(activeoperation->faction)->antagonist == 0) {
        char logs[MSL];
        time_t east_time = current_time;
        char tmp[MSL];
        char datestr[MSL];
        sprintf(tmp, "%s", (char *)ctime(&east_time));
        sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);
        sprintf(logs, "At %s, %s %s\n", datestr, terrain_name(activeoperation->terrain), territory_by_number(activeoperation->territoryvnum)->name);
        free_string(fac->reportone_text);
        fac->reportone_text = str_dup(logs);
        fac->reportone_time = current_time;
        fac->reportone_log_sent = 0;
        fac->reportone_plog_sent = 0;
        free_string(fac->reportone_title);
        fac->reportone_title = str_dup(buf);

        if (activeoperation->goal != GOAL_PSYCHIC) {
          sprintf(logs, "%s\n`cMission: %s`x\n\n%s\n\n%s\n%s\n\n\r", logs, op_goal(activeoperation), activeoperation->description, activeoperation->room_name, displaysun_room(get_room_index(380000)));
          free_string(fac->reportone_text);
          fac->reportone_text = str_dup(logs);
        }




        sprintf(logs, "%s\n%s", fac->reportone_text, argument);
        free_string(fac->reportone_text);
        fac->reportone_text = str_dup(logs);
        for (int i = 0; i < 10; i++) {
          free_string(fac->report_overflow[0][i]);
          fac->report_overflow[0][i] = str_dup("");
        }
      }
      else if (fac->reporttwo_time <= fac->reportthree_time) {
        char logs[MSL];
        time_t east_time = current_time;
        char tmp[MSL];
        char datestr[MSL];
        sprintf(tmp, "%s", (char *)ctime(&east_time));
        sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);
        sprintf(logs, "At %s, %s %s\n", datestr, terrain_name(activeoperation->terrain), territory_by_number(activeoperation->territoryvnum)->name);
        free_string(fac->reporttwo_text);
        fac->reporttwo_text = str_dup(logs);
        fac->reporttwo_time = current_time;
        fac->reporttwo_log_sent = 0;
        fac->reporttwo_plog_sent = 0;
        free_string(fac->reporttwo_title);
        fac->reporttwo_title = str_dup(buf);

        if (activeoperation->goal != GOAL_PSYCHIC) {
          sprintf(logs, "%s\n`cMission: %s`x\n\n%s\n\n%s\n%s\n\n\r", logs, op_goal(activeoperation), activeoperation->description, activeoperation->room_name, displaysun_room(get_room_index(380000)));
          free_string(fac->reporttwo_text);
          fac->reporttwo_text = str_dup(logs);
        }

        sprintf(logs, "%s\n%s", fac->reporttwo_text, argument);
        free_string(fac->reporttwo_text);
        fac->reporttwo_text = str_dup(logs);

        for (int i = 0; i < 10; i++) {
          free_string(fac->report_overflow[1][i]);
          fac->report_overflow[1][i] = str_dup("");
        }

      }
      else {
        char logs[MSL];
        time_t east_time = current_time;
        char tmp[MSL];
        char datestr[MSL];
        sprintf(tmp, "%s", (char *)ctime(&east_time));
        sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);
        sprintf(logs, "At %s, %s %s\n", datestr, terrain_name(activeoperation->terrain), territory_by_number(activeoperation->territoryvnum)->name);
        free_string(fac->reportthree_text);
        fac->reportthree_text = str_dup(logs);
        fac->reportthree_time = current_time;
        fac->reportthree_log_sent = 0;
	fac->reportthree_plog_sent = 0;
        free_string(fac->reportthree_title);
        fac->reportthree_title = str_dup(buf);

        if (activeoperation->goal != GOAL_PSYCHIC) {
          sprintf(logs, "%s\n`cMission: %s`x\n\n%s\n\n%s\n%s\n\n\r", logs, op_goal(activeoperation), activeoperation->description, activeoperation->room_name, displaysun_room(get_room_index(380000)));
          free_string(fac->reportthree_text);
          fac->reportthree_text = str_dup(logs);
        }

        sprintf(logs, "%s\n%s", fac->reportthree_text, argument);
        free_string(fac->reportthree_text);
        fac->reportthree_text = str_dup(logs);

        for (int i = 0; i < 10; i++) {
          free_string(fac->report_overflow[2][i]);
          fac->report_overflow[2][i] = str_dup("");
        }
      }
    }
  }

  void arrange_battleground(int battle_type, int battleground_number, char *name) {
    bool river = FALSE;
    if (number_percent() % 3 == 0)
    river = TRUE;
    // 10004 - 10000
    int xone = 85473 % 3;
    xone += 10001;
    int yone = 54324 % 3;
    yone += 10001;
    int xtwo = 84364 % 3;
    xtwo += 10001;
    int ytwo = 14649 % 3;
    ytwo += 10001;

    for (ROOM_INDEX_DATA *broom = first_battleroom(battleground_number);
    broom != NULL; broom = next_battleroom(battleground_number, broom)) {
      if (broom->z <= 1) {
        for (int i = 0; i < 10; i++) {
          if (broom->exit[i] != NULL && broom->exit[i]->u1.to_room != NULL) {
            broom->exit[i]->wall = WALL_NONE;
            broom->exit[i]->wallcondition = WALLCOND_NORMAL;
            broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wall = WALL_NONE;
            broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
            WALLCOND_NORMAL;
          }
        }
      }
      if (broom->z > 0)
      broom->sector_type = SECT_AIR;
      if (broom->z < 0) {
        free_string(broom->name);
        broom->name = str_dup("`BUnderwater`x");
      }
    }
    for (ROOM_INDEX_DATA *broom = first_battleroom(battleground_number);
    broom != NULL; broom = next_battleroom(battleground_number, broom)) {
      if (broom->z == 0) {
        if (battle_type == BATTLE_FOREST) {
          broom->sector_type = SECT_FOREST;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_FIELD) {
          broom->sector_type = SECT_PARK;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_DESERT) {
          broom->sector_type = SECT_BEACH;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_TOWN) {
          broom->sector_type = SECT_STREET;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_CITY) {
          broom->sector_type = SECT_STREET;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_MOUNTAINS) {
          broom->sector_type = SECT_ROCKY;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_WAREHOUSE) {
          broom->sector_type = SECT_WAREHOUSE;
          if (!IS_SET(broom->room_flags, ROOM_INDOORS))
          SET_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_CAVE) {
          broom->sector_type = SECT_CAVE;
          if (!IS_SET(broom->room_flags, ROOM_INDOORS))
          SET_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_VILLAGE) {
          broom->sector_type = SECT_STREET;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_LAKE) {
          broom->sector_type = SECT_WATER;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }
        else if (battle_type == BATTLE_TUNDRA) {
          broom->sector_type = SECT_ROCKY;
          if (IS_SET(broom->room_flags, ROOM_INDOORS))
          REMOVE_BIT(broom->room_flags, ROOM_INDOORS);
        }

        char buf[MSL];
        if (broom->exit[DIR_DOWN] != NULL && broom->exit[DIR_DOWN]->u1.to_room != NULL) {
          broom->exit[DIR_DOWN]->wall = WALL_BRICK;
          broom->exit[DIR_DOWN]->wallcondition = WALLCOND_NORMAL;
          broom->exit[DIR_DOWN]->u1.to_room->exit[DIR_UP]->wall = WALL_BRICK;
          broom->exit[DIR_DOWN]->u1.to_room->exit[DIR_UP]->wallcondition =
          WALLCOND_NORMAL;
        }
        free_string(broom->name);
        sprintf(buf, "%s`x", name);
        broom->name = str_dup(buf);
      }
      if (battle_type == BATTLE_LAKE) {
        if (broom->x <= 10000 || broom->x >= 10004 || broom->y < 10000 || broom->y >= 10004) {
          if (broom->z == 0)
          broom->sector_type = SECT_SHALLOW;
        }
        else if ((broom->x == xone || broom->x == xtwo) && (broom->y == yone || broom->y == ytwo)) {
          if (broom->z == 0)
          broom->sector_type = SECT_FOREST;
        }
        else if (broom->z < 0) {
          broom->sector_type = SECT_UNDERWATER;
          for (int i = 0; i < 10; i++) {
            if (broom->exit[i] != NULL && broom->exit[i]->u1.to_room != NULL) {
              broom->exit[i]->wall = WALL_NONE;
              broom->exit[i]->wallcondition = WALLCOND_NORMAL;
              broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wall = WALL_NONE;
              broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
              WALLCOND_NORMAL;
            }
          }
        }
      }
      if (battle_type == BATTLE_CITY || battle_type == BATTLE_MOUNTAINS) {

        if ((broom->x == xone || broom->x == xtwo) && (broom->y == yone || broom->y == ytwo)) {
          if (broom->z == 0) {
            for (int i = 0; i < 10; i++) {
              if (broom->exit[i] != NULL && broom->exit[i]->u1.to_room != NULL) {
                broom->exit[i]->wall = WALL_BRICK;
                broom->exit[i]->wallcondition = WALLCOND_NORMAL;
                broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wall = WALL_BRICK;
                broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
                WALLCOND_NORMAL;
              }
            }
          }
          if (broom->z == 1) {
            if (battle_type == BATTLE_CITY)
            broom->sector_type = SECT_STREET;
            else
            broom->sector_type = SECT_ROCKY;
          }
        }
      }
      if (battle_type == BATTLE_FOREST && river == TRUE) {

        if (broom->x == 10003 && broom->z == 0) {
          broom->sector_type = SECT_WATER;
          broom->exit[DIR_DOWN]->wall = WALL_NONE;
          broom->exit[DIR_DOWN]->u1.to_room->exit[DIR_UP]->wall = WALL_NONE;
        }
        if (broom->x == 10003 && broom->z == -1) {
          broom->sector_type = SECT_UNDERWATER;
          for (int i = 0; i < 10; i++) {
            if (i == DIR_UP || i == DIR_DOWN || i == DIR_NORTH || i == DIR_SOUTH)
            continue;
            if (broom->exit[i] != NULL && broom->exit[i]->u1.to_room != NULL) {
              broom->exit[i]->wall = WALL_BRICK;
              broom->exit[i]->wallcondition = WALLCOND_NORMAL;
              broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wall = WALL_BRICK;
              broom->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
              WALLCOND_NORMAL;
            }
          }
        }
      }
      if (battle_type == BATTLE_FIELD && river == TRUE) {
        if (broom->x == 10003 && broom->z == 0) {
          broom->sector_type = SECT_SHALLOW;
          broom->exit[DIR_DOWN]->wall = WALL_NONE;
          broom->exit[DIR_DOWN]->u1.to_room->exit[DIR_UP]->wall = WALL_NONE;
        }
      }
    }
  }

  int camouflage(CHAR_DATA *ch, int battle_type) {
    int iWear;
    OBJ_DATA *obj;
    int camo = 0;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->item_type != ITEM_CLOTHING)
        continue;
        if (!can_see_wear(ch, iWear))
        continue;
        if (battle_type == BATTLE_CAVE) {
          if (is_name("black", from_color(obj->description)) || is_name("dark", from_color(obj->description)) || is_name("grey", from_color(obj->description)) || is_name("gray", from_color(obj->description)))
          camo++;
        }
        if (battle_type == BATTLE_MOUNTAINS) {
          if (is_name("white", from_color(obj->description)) || is_name("grey", from_color(obj->description)) || is_name("gray", from_color(obj->description)))
          camo++;
        }
        if (battle_type == BATTLE_TUNDRA) {
          if (is_name("white", from_color(obj->description)))
          camo++;
        }
        if (battle_type == BATTLE_FOREST) {
          if (is_name("green", from_color(obj->description)) || is_name("camoflage", from_color(obj->description)) || is_name("camouflage", from_color(obj->description)))
          camo++;
        }
        if (battle_type == BATTLE_DESERT) {
          if (is_name("sandy", from_color(obj->description)) || is_name("yellow", from_color(obj->description)))
          camo++;
        }
      }
    }
    return camo;
  }

  bool camo_protect(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (activeoperation == NULL)
    return FALSE;
    if (!battleground(ch->in_room))
    return FALSE;
    if (camouflage(ch, activeoperation->terrain) >= 3)
    return TRUE;
    return FALSE;
  }

  void action_prep(CHAR_DATA *ch, int type, char *argument) {
    CHAR_DATA *victim = NULL;
    if ((victim = get_char_fight(ch, argument)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      send_to_char("Invalid target.\n\r", ch);
      return;
    }
    if (has_weakness(ch, victim)) {
      send_to_char("They are not psychically vulnerable.\n\r", ch);
      return;
    }
    ch->pcdata->prep_target = victim;
    ch->pcdata->prep_action = type;
    send_to_char("Done.\n\r", ch);
    return;
  }

  void prep_process(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!battleground(ch->in_room))
    return;

    if (activeoperation->goal == GOAL_PSYCHIC)
    return;

    if (IS_NPC(ch)) {
      if (ch->pIndexData->vnum == MINION_TEMPLATE || ch->pIndexData->vnum == ALLY_TEMPLATE) {
        if (get_char_world_pc(ch->protecting) != NULL)
        ch = get_char_world_pc(ch->protecting);
        else
        return;
      }
      else
      return;
    }
    if (IS_NPC(ch))
    return;
    if (!battleground(ch->in_room))
    return;

    if (ch->pcdata->prep_target != victim)
    return;
    ch->fighting = FALSE;
    ch->attacking = 0;
    act("`r$n`r fades from view.`x", ch, NULL, NULL, TO_ROOM);
    send_to_char("`rYou fade from view.`x\n\r", ch);
    op_report(logact("`r$n fades out of the nightmare.`x", ch, ch), ch);
    if (ch->bagcarrier == 1) {
      char buf[MSL];

      CHAR_DATA *close;
      close = closest_character(ch);
      if (close != NULL) {
        ch->bagcarrier = 0;
        close->bagcarrier = 1;
        sprintf(buf, "`c$n`c has %s.`x", bag_name());
        act(buf, close, NULL, NULL, TO_ROOM);
        op_report(logact(buf, close, close), ch);
        if (!IS_NPC(close)) {
          sprintf(buf, "`CYou have %s!`x", bag_name());
          act(buf, close, NULL, NULL, TO_CHAR);
        }
      }
    }

    if (!IS_NPC(ch)) {
      reclaim_items(ch);
    }
    wake_char(ch);
    act("You awaken.", ch, NULL, NULL, TO_CHAR);
    act("$n wakes up.", ch, NULL, NULL, TO_ROOM);
    if (ch->pcdata->prep_action == 1 && in_haven(ch->in_room)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 60 * 10);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_ABDUCTED;
      affect_to_char(victim, &af);
      villain_mod(ch, 40, "Shroud snipe");
      int amount = 500;
      if (IS_FLAG(victim->comm, COMM_MANDRAKE))
      amount /= 4;
      if (!can_shroud(victim))
      amount *= 2;
      if (is_weakness(ch, victim))
      amount /= 3;
      use_lifeforce(ch, amount, "shroud snipe.");
      char_from_room(victim);
      char_to_room(victim, ch->in_room);
      send_to_char("You are pulled through the nightmare.\n\r", victim);
      act("$n is pulled through the nightmare", victim, NULL, victim, TO_ROOM);
      victim->pcdata->sleeping = 100;
    }
    else if (ch->pcdata->prep_action == 2 || ch->pcdata->prep_action == 3) {
      FANTASY_TYPE *fant = NULL;
      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE || (*it)->active == FALSE) {
          continue;
        }
        if (!str_cmp((*it)->name, ch->pcdata->ritual_dreamworld)) {
          fant = (*it);
        }
      }
      if (fant == NULL)
      return;

      char buf[MSL];
      sprintf(buf, "suspended%s", victim->name);
      bool suspended = FALSE;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(buf, fant->participants[i]))
        suspended = TRUE;
      }
      if (suspended == TRUE) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(victim->name, fant->participants[i])) {
            free_string(fant->participants[i]);
            fant->participants[i] = str_dup("");
          }
        }
      }
      else {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(victim->name, fant->participants[i])) {
            free_string(fant->participants[i]);
            fant->participants[i] = str_dup(buf);
          }
        }
      }
      if (IS_FLAG(victim->act, PLR_SHROUD) && victim->pcdata->spectre == 0)
      enter_dreamworld(victim, fant);
      else if (victim->pcdata->spectre == 0) {
        to_spectre(victim, FALSE);
        enter_dreamworld(victim, fant);
      }
      else
      enter_dreamworld(victim, fant);

      enter_dreamworld(ch, fant);
      int point = -1;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(victim->name, fant->participants[i]))
        point = i;
      }
      victim->pcdata->dream_room = ch->pcdata->dream_room;
      ch->pcdata->tempdreamgodworld = fantasy_number(fant);
      ch->pcdata->tempdreamgodchar = point;
      if (ch->pcdata->prep_action == 2) {
        int cost = 300;
        if (IS_FLAG(victim->comm, COMM_MANDRAKE))
        cost /= 4;
        if (is_weakness(ch, victim))
        cost /= 4;
        use_lifeforce(ch, cost, "Dream invasion.");
        villain_mod(ch, 10, "Dream invasion");
      }
      else {
        int cost = 900;
        if (IS_FLAG(victim->comm, COMM_MANDRAKE))
        cost /= 4;
        if (is_weakness(ch, victim))
        cost /= 4;
        use_lifeforce(ch, cost, "Dream snaring.");
        villain_mod(ch, 20, "Dream snaring");
        if (!IS_FLAG(victim->comm, COMM_DREAMSNARED))
        SET_FLAG(victim->comm, COMM_DREAMSNARED);
      }
      send_to_char("You invade their dreams, use 'wake' to leave the dream early.\n\r", ch);
      send_to_char("You fall asleep.\n\r", victim);
    }
  }

  bool climate_control(ROOM_INDEX_DATA *room)
  {
    if(!IS_SET(room->room_flags, ROOM_INDOORS))
    return FALSE;
    PROP_TYPE *prop = prop_from_room(room);
    if(prop == NULL)
    return TRUE;

    if(prop->prop_state == 1)
    return FALSE;

    if(prop->type_special == PROPERTY_OUTERFOREST && prop->utilities == 0)
    return FALSE;

    return TRUE;
  }


  int weather_bonus(CHAR_DATA *ch) {
    tm *ptm;
    ptm = gmtime(&current_time);
    if (get_skill(ch, SKILL_COLDSPEC) > 0) {
      if (battleground(ch->in_room)) {
        if (activeoperation->terrain == BATTLE_DESERT)
        return -10;
        if (activeoperation->terrain == BATTLE_TUNDRA)
        return 10;
      }
      else {
        if (is_snowing(ch->in_room))
        return 10;

        if (temperature(ch->in_room) <= 50) {
          if (ptm->tm_mon == 11 || ptm->tm_mon == 0 || ptm->tm_mon == 1)
          return 1;
          else if (ptm->tm_mon == 5 || ptm->tm_mon == 6 || ptm->tm_mon == 7)
          return 10;
          else
          return 5;
        }
        else if (temperature(ch->in_room) >= 85 && !climate_control(ch->in_room))
        return -10;
      }
    }
    else if (get_skill(ch, SKILL_HOTSPEC) > 0) {
      if (battleground(ch->in_room)) {
        if (activeoperation->terrain == BATTLE_DESERT)
        return 10;
        if (activeoperation->terrain == BATTLE_TUNDRA)
        return -10;
      }
      else {
        if ((temperature(ch->in_room) <= 50 && !climate_control(ch->in_room)) || is_snowing(ch->in_room))
        return -10;
        else if (temperature(ch->in_room) >= 85) {
          if (ptm->tm_mon == 11 || ptm->tm_mon == 0 || ptm->tm_mon == 1)
          return 10;
          else if (ptm->tm_mon == 5 || ptm->tm_mon == 6 || ptm->tm_mon == 7)
          return 2;
          else
          return 5;
        }
      }
    }
    return 0;
  }

  _DOFUN(do_arrangebg) { arrange_battleground(atoi(argument), 1, "place"); }

  void operation_swalk(CHAR_DATA *ch) {
    if (isactiveoperation == FALSE || activeoperation == NULL) {
      send_to_char("There is no operation right now.\n\r", ch);
      return;
    }
    if (activeoperation->competition == COMPETE_CLOSED) {
      send_to_char("There is no operation right now.\n\r", ch);
      return;
    }
    if (is_name(ch->name, activeoperation->storyrunners) || is_name(nosr_name(ch->name), activeoperation->storyrunners)) {
      if (!is_gm(ch)) {
        send_to_char("You're not a story runner.\n\r", ch);
        return;
      }
      char_from_room(ch);
      char_to_room(ch, get_room_index(380000));
      if (!IS_FLAG(ch->comm, COMM_RUNNING))
      SET_FLAG(ch->comm, COMM_RUNNING);
      if (!IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(ch->act, PLR_SHROUD);

      send_to_char("You travel to the operation.\n\r", ch);
      return;
    }
    send_to_char("You aren't on the storyrunner list for that operation.\n\r", ch);
  }

  int intel_modifier(CHAR_DATA *ch) {
    int mod = 1000;
    if (IS_FLAG(ch->comm, COMM_PRIVATE))
    return 0;
    mod = mod * (char_secrecy(ch, NULL) / 10) * (char_secrecy(ch, NULL) / 10) /
    10000;
    mod = (mod * 2 + 1000) / 3;
    mod = mod * solidity(ch) / 100;
    mod = mod * get_attract(ch, NULL) / 100;
    mod = mod * (100 + ch->pcdata->heroic) / 100;
    if (ch->pcdata->secret_recover >= 500)
    mod = mod * 3 / 2;
    mod = mod * (ch->pcdata->habit[HABIT_CONCERN] + 20) / 20;
    mod = mod * (ch->pcdata->habit[HABIT_CRUELTY] + 10) / 20;
    if (ch->skills[SKILL_MENTALDISCIPLINE] > 0)
    mod = mod * 2 / 3;
    if (get_skill(ch, SKILL_TOUCHED) > 0)
    mod = mod * 3 / 2;

    if(is_virgin(ch))
    mod = mod * 5/4;

    int scount = 0;
    if(ch->fcore != 0)
    scount++;
    if(ch->fsect != 0)
    scount++;
    if(ch->fcult != 0)
    scount++;

    mod = mod * (10 + scount*scount)/10;

    mod = mod / 10;

    if (mod <= 40)
    mod = 40;
    if (mod >= 300)
    mod = 300;
    if (mod > 125 && ch->faction == 0)
    mod = 125;
    if (mod <= 50 && ch->faction == 0)
    mod = 5;

    return mod;
  }

  void give_intel(CHAR_DATA *ch, int amount) {
    if (IS_FLAG(ch->act, PLR_STASIS) || higher_power(ch))
    return;

    if(ch->fcore > 0 && ch->fsect > 0)
    amount *= 2;
    else if(ch->fsect > 0)
    amount = amount * 3/2;

    if(ch->fsect == 0 && ch->fcore == 0 && !has_symbol_cult(ch))
    return;

    if (ch->pcdata->intel >= 10000)
    ch->pcdata->intel += (int)sqrt((amount * intel_modifier(ch)) / 100);
    else
    ch->pcdata->intel += (amount * intel_modifier(ch)) / 100;
  }

  int pc_op_count() {
    int pop = 0;
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

      if (battleground(victim->in_room))
      pop++;
    }

    return pop;
  }

  int pc_op_alliance_count(int facvnum) {
    int pop = 0;
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

      if (battleground(victim->in_room) && victim->faction == facvnum)
      pop++;
    }

    return pop;
  }

  void defeat_op_pc(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return;

    if (activeoperation == NULL)
    return;

    if (activeoperation->goal == GOAL_PSYCHIC)
    return;

    FACTION_TYPE *fac = clan_lookup(ch->faction);

    if (fac == NULL)
    return;

    char buf[MSL];
    if (activeoperation != NULL && clan_lookup(ch->faction) != NULL && activeoperation->speed > 1 && activeoperation->competition != COMPETE_CLOSED) {
      if (pc_op_alliance_count(ch->faction) == 1) {
        sprintf(buf, "%s's participation in an unsuccessful operation.", ch->name);
        gain_resources(300, ch->faction, ch, buf);
      }
    }
    if (!str_cmp(ch->name, fac->battle_leader)) {
      free_string(fac->battle_leader);
      fac->battle_leader = str_dup("");
    }
    if (pc_op_count() > 1)
    return;

    if (activeoperation != NULL && clan_lookup(ch->faction) != NULL && activeoperation->speed > 1 && activeoperation->competition != COMPETE_CLOSED) {
      int base = activeoperation->speed * 2;
      base += clan_lookup(ch->faction)->defeated_pcs;
      if (activeoperation->faction == ch->faction && activeoperation->speed > 4)
      base *= 3 / 2;
      base = UMIN(base, 20);

      if (activeoperation->spam == 1)
      base = base / 3;
      if (activeoperation->initdays > 0)
      base = base * (60 + activeoperation->initdays * 20) / 100;

      LOCATION_TYPE *loc;
      loc = territory_by_number(activeoperation->territoryvnum);
      FACTION_TYPE *fac = clan_lookup(ch->faction);
      for (int i = 0; i < base; i++) {
        if (!generic_faction_vnum(fac->vnum)) {
          if (fac->type == FACTION_NPC) {
            antagonist_plus(loc, fac);
            if (number_percent() % 10 == 0)
            territory_minus(loc, ALLIANCE_SIDELEFT);
            if (number_percent() % 10 == 0)
            territory_minus(loc, ALLIANCE_SIDERIGHT);
            if (number_percent() % 10 == 0)
            territory_minus(loc, ALLIANCE_SIDEMID);
            if (number_percent() % 10 == 0)
            territory_minus(loc, ALLIANCE_SIDELEFT + 10);
            if (number_percent() % 10 == 0)
            territory_minus(loc, ALLIANCE_SIDERIGHT + 10);
            if (number_percent() % 10 == 0)
            territory_minus(loc, ALLIANCE_SIDEMID + 10);

          }
          else if (fac->type == FACTION_CULT) {
            territory_plus(loc, fac->alliance);
            antagonist_minus(loc);
            if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDELEFT)
            territory_minus(loc, ALLIANCE_SIDELEFT);
            if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDERIGHT)
            territory_minus(loc, ALLIANCE_SIDERIGHT);
            if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDEMID)
            territory_minus(loc, ALLIANCE_SIDEMID);
          }
          else {
            territory_plus(loc, fac->alliance + 10);
            antagonist_minus(loc);
            if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDELEFT)
            territory_minus(loc, ALLIANCE_SIDELEFT + 10);
            if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDERIGHT)
            territory_minus(loc, ALLIANCE_SIDERIGHT + 10);
            if (number_percent() % 10 == 0 && fac->alliance != ALLIANCE_SIDEMID)
            territory_minus(loc, ALLIANCE_SIDEMID + 10);
          }

        }
        else {
          territory_plus(loc, fac->vnum);
          if (number_percent() % 10 && fac->vnum != FACTION_HAND)
          territory_minus(loc, FACTION_HAND);
          if (number_percent() % 10 && fac->vnum != FACTION_ORDER)
          territory_minus(loc, FACTION_ORDER);
          if (number_percent() % 10 && fac->vnum != FACTION_TEMPLE)
          territory_minus(loc, FACTION_TEMPLE);
        }
      }
    }
  }

  bool in_clan(CHAR_DATA *ch) {
    if (ch->faction > 0 && ch->faction < 30000) {
      if (clan_lookup(ch->faction) != NULL)
      return TRUE;
    }
    return FALSE;
  }

  bool in_clan_two(CHAR_DATA *ch) {
    if (ch->factiontwo > 0 && ch->factiontwo < 30000) {
      if (clan_lookup(ch->factiontwo) != NULL)
      return TRUE;
    }
    return FALSE;
  }

  bool char_in_alliance_with(CHAR_DATA *ch, int cvnum) {
    FACTION_TYPE *fac = clan_lookup(cvnum);
    FACTION_TYPE *clan;
    clan = clan_lookup(ch->faction);
    if (clan != NULL) {
      if (clan->alliance == fac->alliance)
      return TRUE;
    }
    clan = clan_lookup(ch->factiontwo);
    if (clan != NULL) {
      if (clan->alliance == fac->alliance)
      return TRUE;
    }
    return FALSE;
  }

  bool clan_in_alliance(FACTION_TYPE *fac, char *astring) {
    if (fac->alliance != 0 && !generic_faction_vnum(fac->vnum)) {
      if (fac->type == FACTION_CULT) {
        if (!str_cmp(alliance_names(time_info.cult_alliance_issue, fac->alliance, time_info.cult_alliance_type), astring))
        return TRUE;
      }
      else {
        if (!str_cmp(alliance_names(time_info.sect_alliance_issue, fac->alliance, time_info.sect_alliance_type), astring))
        return TRUE;
      }
    }
    else if (generic_faction_vnum(fac->vnum)) {
      int axes = time_info.cult_alliance_issue;
      if (axes == AXES_SUPERNATURAL) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Pro-Supernatural"))
          return TRUE;
          if (!str_cmp(astring, "Pro-Supernatural Extremists"))
          return TRUE;
          if (!str_cmp(astring, "Pro-SupernaturalExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Pro-Supernatural"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Anti-Supernatural"))
          return TRUE;
          if (!str_cmp(astring, "Anti-Supernatural Extremists"))
          return TRUE;
          if (!str_cmp(astring, "Anti-SupernaturalExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Anti-Supernatural"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Balanced"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Pro-Supernatural"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Anti-Supernatural"))
          return TRUE;
        }
      }
      else if (axes == AXES_MATERIAL) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Material"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Material"))
          return TRUE;
          if (!str_cmp(astring, "Material Extremists"))
          return TRUE;
          if (!str_cmp(astring, "MaterialExtremists"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Material"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Material"))
          return TRUE;
          if (!str_cmp(astring, "Material Moderates"))
          return TRUE;
          if (!str_cmp(astring, "MaterialModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Spiritual"))
          return TRUE;
          if (!str_cmp(astring, "Spiritual Extremists"))
          return TRUE;
          if (!str_cmp(astring, "SpiritualExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Spiritual"))
          return TRUE;
        }
      }
      else if (axes == AXES_COMBAT) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Manipulative"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Manipulative"))
          return TRUE;
          if (!str_cmp(astring, "Manipulative Moderates"))
          return TRUE;
          if (!str_cmp(astring, "ManipulativeModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Combative"))
          return TRUE;
          if (!str_cmp(astring, "Combative Extremists"))
          return TRUE;
          if (!str_cmp(astring, "CombativeExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Combative"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Balanced"))
          return TRUE;
          if (!str_cmp(astring, "Combative Moderates"))
          return TRUE;
          if (!str_cmp(astring, "CombativeModerates"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Combative"))
          return TRUE;
        }
      }
      else if (axes == AXES_CORRUPT) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Corrupt"))
          return TRUE;
          if (!str_cmp(astring, "Corrupt Extremists"))
          return TRUE;
          if (!str_cmp(astring, "Corrupt Moderates"))
          return TRUE;
          if (!str_cmp(astring, "CorruptExtremists"))
          return TRUE;
          if (!str_cmp(astring, "CorruptModerates"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Corrupt"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Virtuous Moderates"))
          return TRUE;
          if (!str_cmp(astring, "VirtuousModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Virtuous Moderates"))
          return TRUE;
          if (!str_cmp(astring, "Virtuous Extremists"))
          return TRUE;
          if (!str_cmp(astring, "VirtuousModerates"))
          return TRUE;
          if (!str_cmp(astring, "VirtuousExtremists"))
          return TRUE;
        }
      }
      else if (axes == AXES_DEMOCRATIC) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Autocratic"))
          return TRUE;
          if (!str_cmp(astring, "Autocratic Extremists"))
          return TRUE;
          if (!str_cmp(astring, "AutocraticExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Autocratic"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Democratic Moderates"))
          return TRUE;
          if (!str_cmp(astring, "DemocraticModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Democratic Extremists"))
          return TRUE;
          if (!str_cmp(astring, "DemocraticExtremists"))
          return TRUE;
        }
      }
      else if (axes == AXES_ANARCHY) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Lawful Extremists"))
          return TRUE;
          if (!str_cmp(astring, "LawfulExtremists"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Lawful Moderates"))
          return TRUE;
          if (!str_cmp(astring, "LawfulModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Anarchistic"))
          return TRUE;
          if (!str_cmp(astring, "Balanced"))
          return TRUE;
          if (!str_cmp(astring, "Anarchistic Moderates"))
          return TRUE;
          if (!str_cmp(astring, "AnarchisticModerates"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Anarchistic"))
          return TRUE;
        }
      }
      axes = time_info.sect_alliance_issue;
      if (axes == AXES_SUPERNATURAL) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Pro-Supernatural"))
          return TRUE;
          if (!str_cmp(astring, "Pro-Supernatural Extremists"))
          return TRUE;
          if (!str_cmp(astring, "Pro-SupernaturalExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Pro-Supernatural"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Anti-Supernatural"))
          return TRUE;
          if (!str_cmp(astring, "Anti-Supernatural Extremists"))
          return TRUE;
          if (!str_cmp(astring, "Anti-SupernaturalExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Anti-Supernatural"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Balanced"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Pro-Supernatural"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Anti-Supernatural"))
          return TRUE;
        }
      }
      else if (axes == AXES_MATERIAL) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Material"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Material"))
          return TRUE;
          if (!str_cmp(astring, "Material Extremists"))
          return TRUE;
          if (!str_cmp(astring, "MaterialExtremists"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Material"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Material"))
          return TRUE;
          if (!str_cmp(astring, "Material Moderates"))
          return TRUE;
          if (!str_cmp(astring, "MaterialModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Spiritual"))
          return TRUE;
          if (!str_cmp(astring, "Spiritual Extremists"))
          return TRUE;
          if (!str_cmp(astring, "SpiritualExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Spiritual"))
          return TRUE;
        }
      }
      else if (axes == AXES_COMBAT) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Manipulative"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Manipulative"))
          return TRUE;
          if (!str_cmp(astring, "Manipulative Moderates"))
          return TRUE;
          if (!str_cmp(astring, "ManipulativeModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Combative"))
          return TRUE;
          if (!str_cmp(astring, "Combative Extremists"))
          return TRUE;
          if (!str_cmp(astring, "CombativeExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Combative"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Balanced"))
          return TRUE;
          if (!str_cmp(astring, "Combative Moderates"))
          return TRUE;
          if (!str_cmp(astring, "CombativeModerates"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Combative"))
          return TRUE;
        }
      }
      else if (axes == AXES_CORRUPT) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Corrupt"))
          return TRUE;
          if (!str_cmp(astring, "Corrupt Extremists"))
          return TRUE;
          if (!str_cmp(astring, "Corrupt Moderates"))
          return TRUE;
          if (!str_cmp(astring, "CorruptExtremists"))
          return TRUE;
          if (!str_cmp(astring, "CorruptModerates"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Corrupt"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Virtuous Moderates"))
          return TRUE;
          if (!str_cmp(astring, "VirtuousModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Virtuous"))
          return TRUE;
          if (!str_cmp(astring, "Virtuous Moderates"))
          return TRUE;
          if (!str_cmp(astring, "Virtuous Extremists"))
          return TRUE;
          if (!str_cmp(astring, "VirtuousModerates"))
          return TRUE;
          if (!str_cmp(astring, "VirtuousExtremists"))
          return TRUE;
        }
      }
      else if (axes == AXES_DEMOCRATIC) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Autocratic"))
          return TRUE;
          if (!str_cmp(astring, "Autocratic Extremists"))
          return TRUE;
          if (!str_cmp(astring, "AutocraticExtremists"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Autocratic"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Democratic Moderates"))
          return TRUE;
          if (!str_cmp(astring, "DemocraticModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Democratic"))
          return TRUE;
          if (!str_cmp(astring, "Democratic Extremists"))
          return TRUE;
          if (!str_cmp(astring, "DemocraticExtremists"))
          return TRUE;
        }
      }
      else if (axes == AXES_ANARCHY) {
        if (fac->vnum == FACTION_HAND) {
          if (!str_cmp(astring, "Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Lawful Extremists"))
          return TRUE;
          if (!str_cmp(astring, "LawfulExtremists"))
          return TRUE;
        }
        if (fac->vnum == FACTION_TEMPLE) {
          if (!str_cmp(astring, "Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Lawful"))
          return TRUE;
          if (!str_cmp(astring, "Lawful Moderates"))
          return TRUE;
          if (!str_cmp(astring, "LawfulModerates"))
          return TRUE;
        }
        if (fac->vnum == FACTION_ORDER) {
          if (!str_cmp(astring, "Anarchistic"))
          return TRUE;
          if (!str_cmp(astring, "Balanced"))
          return TRUE;
          if (!str_cmp(astring, "Anarchistic Moderates"))
          return TRUE;
          if (!str_cmp(astring, "AnarchisticModerates"))
          return TRUE;
          if (!str_cmp(astring, "Balanced&Anarchistic"))
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool part_of_alliance(CHAR_DATA *ch, char *astring) {
    FACTION_TYPE *clan;
    clan = clan_lookup(ch->faction);
    if (clan != NULL) {
      if (clan_in_alliance(clan, astring))
      return TRUE;
    }
    clan = clan_lookup(ch->factiontwo);
    if (clan != NULL) {
      if (clan_in_alliance(clan, astring))
      return TRUE;
    }
    return FALSE;
  }

  bool has_base(FACTION_TYPE *fac, LOCATION_TYPE *loc) {
    if (loc == NULL || fac == NULL)
    return FALSE;
    if (fac->type == FACTION_CORE && loc->base_faction_core == fac->vnum)
    return TRUE;
    if (fac->type == FACTION_CULT && loc->base_faction_cult == fac->vnum)
    return TRUE;
    if (fac->type == FACTION_SECT && loc->base_faction_sect == fac->vnum)
    return TRUE;
    return FALSE;
  }

  bool in_cult_domain(CHAR_DATA *ch)
  {
    if(IS_NPC(ch))
    return FALSE;

    if(ch->pcdata->in_domain == NULL || ch->pcdata->in_domain <= 0)
    return FALSE;

    if(ch->fcult <= 0)
    return FALSE;

    FACTION_TYPE *cult = clan_lookup(ch->fcult);
    DOMAIN_TYPE *vdomain = vnum_domain(ch->pcdata->in_domain);
    if(cult != NULL && vdomain != NULL && !str_cmp(cult->eidilon, vdomain->domain_of))
    return TRUE;

    return FALSE;
  }

  void make_antag_soldier(OPERATION_TYPE *op, FACTION_TYPE *fac)
  {
    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    vnum = HAND_SOLDIER;

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    mob->faction = fac->vnum;
    int tox = fac->battle_x + number_range(-8, 8);
    int toy = fac->battle_y + number_range(-8, 8);


    ROOM_INDEX_DATA *desti = battleroom_bycoord(op->battleground_number, tox, toy);
    char_to_room(mob, desti);
    mob->hit = max_hp(mob);
    mob->x = tox % 50;
    mob->y = toy % 50;
    if (!IS_FLAG(mob->act, PLR_SHROUD))
    SET_FLAG(mob->act, PLR_SHROUD);

    free_string(mob->short_descr);
    mob->short_descr = str_dup(fac->soldier_name);
    free_string(mob->name);
    mob->name = str_dup(fac->soldier_name);
    free_string(mob->long_descr);
    mob->long_descr = str_dup(fac->soldier_name);
    free_string(mob->description);
    mob->description = str_dup(fac->soldier_desc);

    mob->mob_ai = MOB_AI_ANTAG_SOLDIER;
    mob->attacking = 1;
    mob->fighting = TRUE;
    mob->in_fight = TRUE;
    mob->attack_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    mob->move_timer = FIGHT_WAIT * fight_speed(mob) / 2;
    mob->fight_speed = op->speed;

  }


  void antag_soldiers(OPERATION_TYPE *op, FACTION_TYPE *fac)
  {
    int max_power = op->initial_power*3;
    int advalue = ADVERSARY_VALUE;
    int maxn = 10;
    max_power = UMAX(advalue*2 + 1, max_power);
    for (int j = 0; max_power > advalue && j < maxn; j++) {
      make_antag_soldier(op, fac);
      max_power -= advalue;
    }
  }

  bool check_antag_win(int number)
  {
    CHAR_DATA *victim;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (victim == NULL || is_gm(victim))
      continue;
      if(!IS_NPC(victim))
      continue;
      if (victim->in_room == NULL || !battleground(victim->in_room) || bg_number(victim->in_room) != number)
      continue;

      FACTION_TYPE *afac = clan_lookup(victim->faction);
      if(afac != NULL && afac->antagonist == 1 && number_percent() % 2 == 0)
      {
        win_operation(afac->vnum, NULL);
        return TRUE;
      }
    }
    return FALSE;
  }


  _DOFUN(do_testoutput)
  {
    FACTION_TYPE *fac = clan_lookup(FACTION_HAND);
    char sout[MSL];
    sprintf(sout, "%d,%d,~%s~,~", 1, 0, fac->reportone_title);
    writeTextToFile(AI_SUM_IN_FILE, str_dup(sout));
    writeTextToFile(AI_SUM_IN_FILE, fac->reportone_text);
    for(int i=0;i<10;i++)
    {
      if(strlen(fac->report_overflow[0][i]) > 2)
      writeTextToFile(AI_SUM_IN_FILE, fac->report_overflow[0][i]);
    }
    writeLineToFile(AI_SUM_IN_FILE, "~");
  }

  int max_core_power(OPERATION_TYPE *op, FACTION_TYPE *host)
  {
    CHAR_DATA *victim;
    int templep = 0;
    int handp = 0;
    int orderp = 0;
    int antagcore = FACTION_CORE;
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
      if (battleground(victim->in_room))
      continue;
      if (is_helpless(victim))
      continue;
      if (is_ghost(victim))
      continue;

      if (signed_up(victim, op, antagcore, host->type)) {
        if(victim->fcore == FACTION_HAND)
        handp += (sqrt(get_lvl(victim))) * 10;
        if(victim->fcore == FACTION_TEMPLE)
        templep += (sqrt(get_lvl(victim))) * 10;
        if(victim->fcore == FACTION_ORDER)
        orderp += (sqrt(get_lvl(victim))) * 10;
      }
    }
    if(handp >= orderp && handp >= templep)
    {
      int rval = UMAX(orderp, templep);
      if(rval <= 0)
      return handp*5/4;
      return rval;
    }
    if(templep >= orderp && templep >= handp)
    {
      int rval = UMAX(orderp, handp);
      if(rval <= 0)
      return templep*5/4;
      return rval;
    }
    if(orderp >= templep && orderp >= handp)
    {
      int rval = UMAX(templep, handp);
      if(rval <= 0)
      return orderp*5/4;
      return rval;
    }
    return 10000;
  }



  bool can_operative(CHAR_DATA *ch)
  {
    if(IS_NPC(ch))
    return FALSE;
    if(IS_FLAG(ch->act, PLR_GUEST))
    return FALSE;
    if(is_gm(ch) || higher_power(ch))
    return FALSE;

    if(ch->spentkarma < 10000)
    return FALSE;
    if(ch->faction == 0)
    return FALSE;

    if(!free_to_act(ch))
    {
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

        if(ch == victim || same_player(ch, victim))
        continue;

        if (victim->in_room == NULL)
        continue;

        if (is_gm(victim) || higher_power(victim))
        continue;

        if (victim->in_room != ch->in_room)
        continue;

        if(is_helpless(victim))
        continue;

        if(seems_under_understanding(victim, ch) == FALSE)
        return FALSE;
      }


    }
    return TRUE;
  }

  void create_ai_operative(CHAR_DATA *ch, int type)
  {
    ch->pcdata->operative_creation_type = type;
    FACTION_TYPE *fac = NULL;
    if(type == FACTION_CORE)
    fac = clan_lookup(ch->fcore);
    if(type == FACTION_CULT)
    fac = clan_lookup(ch->fcult);
    if(type == FACTION_SECT)
    fac = clan_lookup(ch->fsect);

    if(fac == NULL)
    {
      send_to_char("Attempt failed.", ch);
      return;
    }
    char scriptbuf[MSL];
    int gcode;
    if(strcasestr(from_color(fac->description), "\"") == NULL && strcasestr(from_color(get_default_dreamdesc(ch)), "\"") == NULL)
    {
      if(ch->sex == SEX_FEMALE)
      gcode = 1;
      else
      gcode = 2;
      sprintf(scriptbuf, "6,%d,%s,%s,\"%s\",\"%s\",\"%s\"", gcode, ch->name, ch->pcdata->last_name, from_color(get_default_dreamdesc(ch)), from_color(fac->description), ch->pcdata->intro_desc);
      writeLineToFile(AI_IN_FILE, str_dup(scriptbuf));
      return;
    }
    send_to_char("Attempt failed.", ch);

  }

  void become_operative(CHAR_DATA *ch, int type)
  {
    char olook[MSL];
    sprintf(olook, "%s", get_intro(ch));
    FACTION_TYPE *fac;
    if(type == FACTION_CORE)
    fac = clan_lookup(ch->fcore);
    if(type == FACTION_CULT)
    fac = clan_lookup(ch->fcult);
    if(type == FACTION_SECT)
    fac = clan_lookup(ch->fsect);
    if(fac == NULL)
    return;
    char * oname = str_dup("");
    if(type == FACTION_CORE)
    {
      if(safe_strlen(ch->pcdata->operative_core) < 1 || !character_exists(ch->pcdata->operative_core))
      {
        create_ai_operative(ch, type);
        send_to_char("Operative requested, they may take a few minutes to arrive.", ch);
        return;
      }
      free_string(oname);
      oname = str_dup(ch->pcdata->operative_core);
    }
    if(type == FACTION_CULT)
    {
      if(safe_strlen(ch->pcdata->operative_cult) < 1 || !character_exists(ch->pcdata->operative_cult))
      {
        create_ai_operative(ch, type);
        send_to_char("Operative requested, they may take a few minutes to arrive.", ch);
        return;
      }
      free_string(oname);
      oname = str_dup(ch->pcdata->operative_cult);
    }
    if(type == FACTION_SECT)
    {
      if(safe_strlen(ch->pcdata->operative_sect) < 1 || !character_exists(ch->pcdata->operative_sect))
      {
        create_ai_operative(ch, type);
        send_to_char("Operative requested, they may take a few minutes to arrive.", ch);
        return;
      }
      free_string(oname);
      oname = str_dup(ch->pcdata->operative_sect);
    }
    CHAR_DATA *test = get_char_world_pc(oname);
    if(test != NULL)
    {
      send_to_char("Operative already active.", ch);
      return;
    }
    int ftype = type;
    int lf_taken = ch->lf_taken;
    int lf_used = ch->lf_used;
    int wounds = ch->wounds;
    int htimer = ch->heal_timer;
    int intel = ch->pcdata->intel;
    int last_intel = ch->pcdata->last_intel;
    int ill_count = ch->pcdata->ill_count;
    int heroic = ch->pcdata->heroic;

    ROOM_INDEX_DATA *inroom = ch->in_room;
    bool bhands = FALSE;
    bool bfeet = FALSE;
    if(IS_FLAG(ch->act, PLR_BOUNDFEET))
    {
      bfeet = TRUE;
      REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
    }
    if(IS_FLAG(ch->act, PLR_BOUND))
    {
      bhands = TRUE;
      REMOVE_FLAG(ch->act, PLR_BOUND);
    }
    PROP_TYPE * prop = prop_from_room(inroom);
    ROOM_INDEX_DATA *home = get_fleeroom(ch, prop);
    ACCOUNT_TYPE *oaccount = ch->pcdata->account;
    char_from_room(ch);
    char_to_room(ch, home);
    char tempname[MSL];
    save_char_obj(ch, FALSE, FALSE);
    sprintf(tempname, "%s", ch->name);
    DESCRIPTOR_DATA *d = ch->desc;
    quit_process(ch);
    extract_char(ch, TRUE);
    if(load_char_obj(d, oname))
    {
      CHAR_DATA *nch = NULL;
      nch = d->character;
      d->connected = CON_PLAYING;
      nch->desc->pEdit = (void *)nch;
      nch->pcdata->account = oaccount;
      if(nch->pcdata->account == NULL)
      nch->pcdata->account = d->account;

      if(nch->in_room != NULL)
      char_from_room(nch);
      char_to_room(nch, inroom);
      ch = nch;


      if (IS_FLAG(nch->act, PLR_BOUND))
      REMOVE_FLAG(nch->act, PLR_BOUND);
      if (IS_FLAG(nch->act, PLR_BOUNDFEET))
      REMOVE_FLAG(nch->act, PLR_BOUNDFEET);

      if(bhands)
      {
        SET_FLAG(ch->act, PLR_BOUND);
      }
      if(bfeet)
      {
        SET_FLAG(ch->act, PLR_BOUNDFEET);
      }
      ch->fcore = 0;
      ch->fsect = 0;
      ch->fcult = 0;
      ch->faction = 0;
      ch->factiontwo = 0;
      if(ftype == FACTION_CORE)
      {
        ch->fcore = fac->vnum;
        ch->faction = fac->vnum;
      }
      else if(ftype == FACTION_SECT)
      {
        ch->fsect = fac->vnum;
        ch->faction = fac->vnum;
      }
      else if(ftype == FACTION_CULT)
      {
        ch->fcult = fac->vnum;
        ch->faction = fac->vnum;
      }
      ch->lf_taken = lf_taken;
      ch->lf_used = lf_used;
      ch->wounds = wounds;
      ch->heal_timer = htimer;
      ch->pcdata->intel = intel;
      ch->pcdata->last_intel = last_intel;
      ch->pcdata->ill_count = ill_count;
      ch->pcdata->heroic = heroic;
      char buf[MSL];
      sprintf(buf, "%s is revealed as $n.", olook);
      act(buf, ch, NULL, NULL, TO_ROOM);
      act(buf, ch, NULL, NULL, TO_CHAR);
    }
    else
    {
      send_to_char("Operative failed to load.", ch);
      return;
    }
  }


  _DOFUN(do_testoperative)
  {
    FACTION_TYPE *fac = clan_lookup(ch->fcore);
    char scriptbuf[MSL];
    int gcode;
    if(strcasestr(from_color(fac->description), "\"") == NULL && strcasestr(from_color(get_default_dreamdesc(ch)), "\"") == NULL)
    {
      if(ch->sex == SEX_FEMALE)
      gcode = 1;
      else
      gcode = 2;
      sprintf(scriptbuf, "6,%d,%s,%s,\"%s\",\"%s\",\"%s\"", gcode, ch->name, ch->pcdata->last_name, from_color(get_default_dreamdesc(ch)), from_color(fac->description), ch->pcdata->intro_desc);
      writeLineToFile(AI_IN_FILE, str_dup(scriptbuf));
    }
  }


  void operative_outfit(CHAR_DATA *ch, bool sranged, bool lranged, bool smelee, bool lmelee, bool armor)
  {
    OBJ_DATA *obj;

    if(sranged == TRUE)
    {
      if (ch->disciplines[DIS_PISTOLS] > 0) {
        obj = create_object(get_obj_index(17), 0);
        obj->cost = 0;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("worn black automatic handgun");
        free_string(obj->description);
        obj->description = str_dup("A worn black automatic handgun");
        free_string(obj->name);
        obj->name = str_dup("gun handgun black worn automatic");
        obj_to_char(obj, ch);
      }
    }
    if(lranged == TRUE)
    {
      if (ch->disciplines[DIS_BOWS] > 0) {
        obj = create_object(get_obj_index(17), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("simple longbow");
        free_string(obj->description);
        obj->description = str_dup("A simple longbow");
        free_string(obj->name);
        obj->name = str_dup("simple longbow bow");
        obj_to_char(obj, ch);
      }

      if (ch->disciplines[DIS_SHOTGUNS] > 0) {
        obj = create_object(get_obj_index(17), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("worn black pump-action shotgun");
        free_string(obj->description);
        obj->description = str_dup("A worn black pump-action shotgun");
        free_string(obj->name);
        obj->name = str_dup("shotgun black worm pump");
        obj_to_char(obj, ch);
      }
      if (ch->disciplines[DIS_RIFLES] > 0) {
        obj = create_object(get_obj_index(17), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("worn brown bolt-action rifle");
        free_string(obj->description);
        obj->description = str_dup("A worn brown bolt-action rifle");
        free_string(obj->name);
        obj->name = str_dup("worn brown bolt action rifle gun");
        obj_to_char(obj, ch);
      }
      if (ch->disciplines[DIS_CARBINES] > 0) {
        obj = create_object(get_obj_index(17), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("AK-47 carbine");
        free_string(obj->description);
        obj->description = str_dup("An AK-47 carbine");
        free_string(obj->name);
        obj->name = str_dup("ak-47 carbine rifle gun");
        obj_to_char(obj, ch);
      }
    }
    if(lmelee == TRUE)
    {
      if (ch->disciplines[DIS_LONGBLADE] > 0) {
        obj = create_object(get_obj_index(16), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("rusty steel shortsword");
        free_string(obj->description);
        obj->description = str_dup("A rusty steel shortsword");
        free_string(obj->name);
        obj->name = str_dup("rusty steel shortsword sword");
        obj_to_char(obj, ch);
      }
      if (ch->disciplines[DIS_BLUNT] > 0) {
        obj = create_object(get_obj_index(16), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("rusy steel warhammer");
        free_string(obj->description);
        obj->description = str_dup("A rusty steel warhammer");
        free_string(obj->name);
        obj->name = str_dup("rusty steel warhammer hammer");
        obj_to_char(obj, ch);
      }
    }
    if(smelee == TRUE)
    {
      if (ch->disciplines[DIS_KNIFE] > 0) {
        obj = create_object(get_obj_index(16), 0);
        obj->cost = 0;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("rusty steel knife");
        free_string(obj->description);
        obj->description = str_dup("A rusty steel knife");
        free_string(obj->name);
        obj->name = str_dup("rusty steel knife");
        obj_to_char(obj, ch);
      }
    }
    if(armor == TRUE)
    {
      if (ch->disciplines[DIS_BARMOR] > 0) {
        obj = create_object(get_obj_index(19), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("worn kevlar vest");
        free_string(obj->description);
        obj->description = str_dup("A worn kevlar vest");
        free_string(obj->name);
        obj->name = str_dup("worn kevlar vest");
        obj_to_char(obj, ch);
        equip_char_silent(ch, obj, WEAR_BODY_6);
      }
      else if (ch->disciplines[DIS_MARMOR] > 0) {
        obj = create_object(get_obj_index(19), 0);
        obj->cost = 0;
        obj->size = 30;
        free_string(obj->short_descr);
        obj->short_descr = str_dup("battered steel breastplate");
        free_string(obj->description);
        obj->description = str_dup("A battered steel breastplate");
        free_string(obj->name);
        obj->name = str_dup("battered steel breastplate");
        obj_to_char(obj, ch);
        equip_char_silent(ch, obj, WEAR_BODY_6);
      }
    }

    obj = create_object(get_obj_index(55), 0);
    obj->cost = 0;
    if (time_info.phone < 7000000)
    time_info.phone = 7000000;
    time_info.phone += number_range(1, 19);
    obj->value[0] = time_info.phone;
    obj_to_char(obj, ch);


    obj = create_object(get_obj_index(50), 0); // shorts
    obj->cost = 0;
    obj_to_char(obj, ch);
    equip_char_silent(ch, obj, WEAR_BODY_3);

    obj = create_object(get_obj_index(51), 0); //jeans
    obj->cost = 0;
    obj_to_char(obj, ch);
    equip_char_silent(ch, obj, WEAR_BODY_4);

    obj = create_object(get_obj_index(52), 0); //t-shirt
    obj->cost = 0;
    obj_to_char(obj, ch);
    equip_char_silent(ch, obj, WEAR_BODY_5);

    obj = create_object(get_obj_index(53), 0); //socks
    obj->cost = 0;
    obj_to_char(obj, ch);
    equip_char_silent(ch, obj, WEAR_BODY_1);


    obj = create_object(get_obj_index(54), 0); //sneakers
    obj->cost = 0;

    obj_to_char(obj, ch);
    equip_char_silent(ch, obj, WEAR_BODY_2);



    obj = create_object(get_obj_index(59), 0); //jacket
    obj->cost = 0;
    obj_to_char(obj, ch);
    equip_char_silent(ch, obj, WEAR_BODY_10);
  }


  void guest_match(CHAR_DATA *ch)
  {
    if(IS_NPC(ch) || !IS_FLAG(ch->act, PLR_GUEST) || ch->pcdata->guest_type != GUEST_OPERATIVE)
    return;
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

    victim->lf_taken = ch->lf_taken;
    victim->lf_used = ch->lf_used;
    victim->pcdata->intel = ch->pcdata->intel;
    victim->pcdata->last_intel = ch->pcdata->last_intel;
    victim->pcdata->ill_count = ch->pcdata->ill_count;
    victim->pcdata->heroic = ch->pcdata->heroic;

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
    return;
  }


  _DOFUN(do_operative)
  {
    if(IS_FLAG(ch->act, PLR_GUEST) || is_gm(ch) || higher_power(ch) || ch->spentkarma < 10000)
    {
      send_to_char("You cannot do that.", ch);
      return;
    }
    if(!can_operative(ch))
    {
      send_to_char("You cannot do that right now.", ch);
      return;
    }
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    if(!str_cmp(arg, "cult"))
    {
      if(ch->fcult <= 0)
      {
        send_to_char("You are not in a cult.", ch);
        return;
      }
      become_operative(ch, FACTION_CULT);
    }
    else if(!str_cmp(arg, "faction"))
    {
      if(ch->fcore <= 0)
      {
        send_to_char("You are not in a faction.", ch);
        return;
      }
      become_operative(ch, FACTION_CORE);
    }
    else if(!str_cmp(arg, "sect"))
    {
      if(ch->fsect <= 0)
      {
        send_to_char("You are not in a sect.", ch);
        return;
      }
      become_operative(ch, FACTION_SECT);
    }
    else if(!str_cmp(arg, "retire"))
    {
      char strsave[MAX_INPUT_LENGTH];
      if(!str_cmp(argument, "cult"))
      {
        if(safe_strlen(ch->pcdata->operative_cult) < 2 || !character_exists(ch->pcdata->operative_cult))
        {
          send_to_char("You do not have a cult operative.", ch);
          return;
        }
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->operative_cult));
        unlink(strsave);
        send_to_char("Done.\n\r", ch);
        return;
      }
      else if(!str_cmp(argument, "faction"))
      {
        if(safe_strlen(ch->pcdata->operative_core) < 2 || !character_exists(ch->pcdata->operative_core))
        {
          send_to_char("You do not have a faction operative.", ch);
          return;
        }
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->operative_core));
        unlink(strsave);
        send_to_char("Done.\n\r", ch);
        return;
      }
      else if(!str_cmp(argument, "sect"))
      {
        if(safe_strlen(ch->pcdata->operative_sect) < 2 || !character_exists(ch->pcdata->operative_sect))
        {
          send_to_char("You do not have a sect operative.", ch);
          return;
        }
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->operative_sect));
        unlink(strsave);
        send_to_char("Done.\n\r", ch);
        return;
      }
      else
      {
        send_to_char("Syntax: operative retire <cult|faction|sect>", ch);
        return;
      }
    }
    else
    {
      send_to_char("Syntax: operative <cult|faction|sect> OR operative retire <cult|faction|sect>", ch);
      return;
    }

  }


#if defined(__cplusplus)
}
#endif

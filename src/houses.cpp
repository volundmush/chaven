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
#include "tables.h"
#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif



  int get_owner_bid args((HOUSE_TYPE * house));
  bool can_decorate args((CHAR_DATA * ch, ROOM_INDEX_DATA *room));
  bool building_limit args((CHAR_DATA * ch));
  bool nearby_structure args((ROOM_INDEX_DATA * room));
  void recycle_prop args((PROP_TYPE * prop));
  bool house_exists args((int num));
  bool shop_exists args((int num));
  int shop_count args((int num));
  int house_count args((int num));
  bool land_exists args((int num));
  char *create_address args((ROOM_INDEX_DATA * doorway, int direction));
  bool can_develop args((ROOM_INDEX_DATA * room, PROP_TYPE *prop));
  bool has_paccess args((CHAR_DATA * ch, ROOM_INDEX_DATA *room));
  void populate_vnums args((PROP_TYPE * prop));
  int max_objects args((PROP_TYPE * prop));
  void notice args((CHAR_DATA * ch, PROP_TYPE *prop));
  int property_truevalue args((PROP_TYPE * prop));
  bool plot_alignment args((ROOM_INDEX_DATA * room, int direction));
  bool plot_alignment_new args((ROOM_INDEX_DATA * room, int direction));
  void prop_type_switch args((PROP_TYPE * prop, int type));
  int lightcount args((PROP_TYPE * prop));
  int plotz args((int vnum));
  int plotx args((int vnum));
  int ploty args((int vnum));
  bool is_room_in_property args((ROOM_INDEX_DATA * room, PROP_TYPE *prop));

  ROOM_INDEX_DATA *first_room_in_property args((PROP_TYPE * prop));
  ROOM_INDEX_DATA *next_room_in_property args((PROP_TYPE * prop, ROOM_INDEX_DATA *previous));

  int room_value args((ROOM_INDEX_DATA * room));
  char *room_tags args((ROOM_INDEX_DATA * room));
  void setup_proproom args((ROOM_INDEX_DATA * room, PROP_TYPE *prop));
  bool can_develop_proproom args((ROOM_INDEX_DATA * room));
  void make_property args((ROOM_INDEX_DATA * start, int direction, int xlength, int ylength, int type, int subtype, char *street));
  int prop_location args((PROP_TYPE * prop));
  int district_room args((ROOM_INDEX_DATA * room));
  bool valid_industry args((int industry, int district));

#define TOWNLAND_PRICE 50
#define FORESTLAND_PRICE 20
#define CLEAR_COST 15

#define ROOM_PRICE 20
#define FEATURE_PRICE 80

#define DECOR_ONE 25
#define DECOR_TWO 50
#define DECOR_THREE 100

#define TOUGHNESS_PRICE 20
#define SECURITY_PRICE 10

  vector<HOUSE_TYPE *> HouseVect;
  HOUSE_TYPE *nullhouse;

  vector<PROP_TYPE *> PropVect;
  PROP_TYPE *nullprop;
  vector<DOMAIN_TYPE *> DomainVect;
  DOMAIN_TYPE *nulldomain;
  char *rent_one;
  char *rent_two;
  char *rent_three;
  char *rent_four;
  char *rent_five;
  char *rent_six;
  char *rent_seven;
  char *rent_eight;
  char *rent_nine;
  char *rent_ten;
  char *rent_eleven;
  char *rent_twelve;
  char *rent_thirteen;
  char *rent_fourteen;
  char *rent_fifteen;
  char *rent_sixteen;
  char *rent_seventeen;
  char *rent_eighteen;
  char *rent_nineteen;
  char *rent_twenty;
  char *rent_twentyone;
  char *rent_twentytwo;

  char *cont_one;
  char *cont_two;
  char *cont_three;
  char *cont_four;
  char *cont_five;
  char *cont_six;
  char *cont_seven;
  char *cont_eight;
  char *cont_nine;
  char *cont_ten;
  char *cont_eleven;
  char *cont_twelve;

  // Doubled for roommates - Disco 5/25/2018
  char *enclave_room[MAX_DORMROOMS + MAX_DORMROOMS -1];

  bool room_in_prop(PROP_TYPE *prop, ROOM_INDEX_DATA *room) {
    return is_room_in_property(room, prop);
  }

  PROP_TYPE *prop_from_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return NULL;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); it++) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (room_in_prop((*it), room))
      return (*it);
    }
    return NULL;
  }

  bool owned_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (room_in_prop((*it), room))
      return TRUE;
    }
    return FALSE;
  }

  bool earlier_prop(PROP_TYPE *prop_one, PROP_TYPE *prop_two) {
    if (prop_one->vnum < prop_two->vnum)
    return TRUE;

    return FALSE;
  }

  bool inside_prange(int num, PROP_TYPE *prop) {

    if (num >= prop->lower_vnum && num <= prop->higher_vnum)
    return TRUE;

    return FALSE;
  }

  HOUSE_TYPE *in_house(CHAR_DATA *ch) {
    int j = 0;
    if (ch == NULL || !ch || ch->in_room == NULL || ch->name == NULL)
    return NULL;
    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end() && j < 40; ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }
      j++;
    }
    return NULL;
  }
  PROP_TYPE *in_prop(CHAR_DATA *ch) {
    if (ch == NULL || !ch || ch->in_room == NULL || ch->name == NULL)
    return NULL;

    return prop_from_room(ch->in_room);
  }

  HOUSE_TYPE *room_house(ROOM_INDEX_DATA *room) {
    int j = 0;

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end() && j < 40; ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }
      j++;
    }
    return NULL;
  }

  PROP_TYPE *room_prop(ROOM_INDEX_DATA *room) { return prop_from_room(room); }

  HOUSE_TYPE *house_lookup(int vnum) {
    return NULL;
    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }
      if (vnum == (*it)->vnum)
      return (*it);
    }
    return NULL;
  }

  PROP_TYPE *prop_lookup(int vnum) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (vnum == (*it)->vnum)
      return (*it);
    }
    return NULL;
  }
  PROP_TYPE *prop_lookup_shop(int vnum) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->type != PROP_SHOP)
      continue;
      if (vnum == (*it)->vnum)
      return (*it);
    }
    return NULL;
  }
  // Altered to conform to house list - Discordance
  PROP_TYPE *prop_lookup_house(int vnum) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
        continue;
      }

      if (vnum == (*it)->vnum)
      return (*it);
    }
    return NULL;
  }

  HOUSE_TYPE *get_house(CHAR_DATA *ch) {
    int i;
    int j = 0;
    /*
if(ch == NULL || !ch || ch->in_room == NULL || ch->name == NULL)
return nullhouse;
*/
    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end() && j < 40; ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }
      j++;
      if (!str_cmp(NAME(ch), (*it)->owner))
      return (*it);

      for (i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), (*it)->tenants[i]))
        return (*it);
      }
    }
    return NULL;
  }

  void fread_house(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    HOUSE_TYPE *house;

    house = new_house();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        if (!str_cmp(word, "Adate")) {
          house->auction_day = fread_number(fp);
          house->auction_month = fread_number(fp);
          if (house->auction_month > 11)
          house->auction_month = 0;
          /*
if(house->auction_month > 12)
house->auction_month -= 12;
*/
          fMatch = TRUE;
        }
        KEY("Address", house->address, fread_string(fp));
        break;
      case 'B':
        if (!str_cmp(word, "Bid")) {
          int i = 0;
          for (i = 0; i < 30 && house->bid_amounts[i] > 0; i++) {
          }
          house->bid_names[i] = fread_string(fp);
          house->bid_amounts[i] = fread_number(fp);
          fMatch = TRUE;
        }
        KEY("Blackout", house->blackout, fread_number(fp));
        break;
      case 'C':
        KEY("CPrice", house->current_price, fread_number(fp));
        break;
      case 'D':
        KEY("Door", house->door, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!house->house_name) {
            bug("Fread_House: House not found.", 0);
            free_house(house);
            return;
          }
          HouseVect.push_back(house);
          return;
        }
        break;
      case 'F':
        KEY("Faction", house->faction, fread_number(fp));
        break;
      case 'G':
        KEY("Guards", house->guards, fread_number(fp));
        break;
      case 'I':
        if (!str_cmp(word, "Invitee")) {
          int i;
          for (i = 0; i < 50 && safe_strlen(house->tenants[i]) > 1; i++) {
          }
          house->tenants[i] = fread_string(fp);
          house->tenant_trust[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'L':
        KEY("LastOwner", house->last_owner, fread_string(fp));
        break;
      case 'N':
        KEY("Name", house->house_name, fread_string(fp));
        break;
      case 'P':
        KEY("Price", house->price, fread_number(fp));
        break;
      case 'O':
        if (!str_cmp(word, "Owner")) {
          house->owner = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'S':
        KEY("Security", house->security, fread_number(fp));
        break;
      case 'T':
        KEY("Toughness", house->toughness, fread_number(fp));
        KEY("Transfer", house->transfer_point, fread_number(fp));
        break;
      case 'V':
        KEY("Vnum", house->vnum, fread_number(fp));
        if (!str_cmp(word, "Vnums")) {
          house->lower_vnum = fread_number(fp);
          house->higher_vnum = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_house: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_houses() {
    nullhouse = new_house();
    FILE *fp;

    if ((fp = fopen("../data/houses.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_HOUSES: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "HOUSE")) {
          fread_house(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Houses: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open house.txt", 0);
      exit(0);
    }
  }

  void save_houses(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];

    if (backup) {
      sprintf(buf, "house.%d%d", time_info.day, time_info.month);
      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open house.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/houses.txt", "w")) == NULL) {
        bug("Cannot open house.txt for writing", 0);
        return;
      }
    }

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        bug("Save_houses: Blank house in vector", i);
        continue;
      }

      fprintf(fpout, "#HOUSE\n");
      fprintf(fpout, "Vnum	%d\n", (*it)->vnum);
      fprintf(fpout, "Name      %s~\n", (*it)->house_name);
      fprintf(fpout, "Vnums	%d %d\n", (*it)->lower_vnum, (*it)->higher_vnum);
      fprintf(fpout, "Transfer %d\n", (*it)->transfer_point);
      fprintf(fpout, "LastOwner %s~\n", (*it)->last_owner);
      fprintf(fpout, "Address %s~\n", (*it)->address);
      fprintf(fpout, "Toughness %d\n", (*it)->toughness);
      fprintf(fpout, "Security %d\n", (*it)->security);
      fprintf(fpout, "Guards %d\n", (*it)->guards);
      fprintf(fpout, "Door	%d\n", (*it)->door);
      fprintf(fpout, "Faction %d\n", (*it)->faction);
      fprintf(fpout, "Price	%d\n", (*it)->price);
      fprintf(fpout, "Blackout %d\n", (*it)->blackout);
      fprintf(fpout, "CPrice %d\n", (*it)->current_price);
      fprintf(fpout, "ADate %d %d\n", (*it)->auction_day, (*it)->auction_month);
      for (i = 0; i < 30; i++) {
        if ((*it)->bid_amounts[i] > 0 && (*it)->bid_amounts[i] < 1000000)
        fprintf(fpout, "Bid %s~\n%d\n", (*it)->bid_names[i], (*it)->bid_amounts[i]);
      }
      fprintf(fpout, "Owner %s~\n", (*it)->owner);
      for (i = 0; i < 50; i++) {
        if (safe_strlen((*it)->tenants[i]) > 1)
        fprintf(fpout, "Invitee %s~\n %d\n", (*it)->tenants[i], (*it)->tenant_trust[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  bool eidilon_fac_exists(char * argument)
  {

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {

      if((*it)->stasis == 1)
      continue;

      if(!str_cmp(argument, (*it)->eidilon))
      return TRUE;
    }
    return FALSE;
  }

  void save_domains(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/domains.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/domains.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/domains.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/domains.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/domains.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/domains.txt");
      else
      sprintf(buf, "../data/back7/domains.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open domains.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/domains.txt", "w")) == NULL) {
        bug("Cannot open domains.txt for writing", 0);
        return;
      }
    }

    for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
    it != DomainVect.end(); ++it) {
      if (!(*it)->domain_of || (*it)->domain_of[0] == '\0') {
        bug("Save_houses: Blank domain in vector", i);
        continue;
      }
      if (!character_exists((*it)->domain_of))
      continue;

      if ((*it)->valid == FALSE)
      continue;

      if(!eidilon_fac_exists((*it)->domain_of))
      continue;

      fprintf(fpout, "#DOMAIN\n");
      fprintf(fpout, "Vnum   %d\n", (*it)->vnum);
      fprintf(fpout, "Name      %s~\n", (*it)->domain_of);
      fprintf(fpout, "Archetype %d\n", (*it)->archetype);
      fprintf(fpout, "Ambients %s~\n", (*it)->ambients);
      fprintf(fpout, "Desc %s~\n", (*it)->desc);
      fprintf(fpout, "Power %d\n", (*it)->power);
      fprintf(fpout, "Exp %d\n", (*it)->exp);
      fprintf(fpout, "Roomlist");
      for (int x = 0; x < 50; x++)
      fprintf(fpout, " %d", (*it)->roomlist[x]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Smallshrines");
      for (int x = 0; x < 50; x++)
      fprintf(fpout, " %d", (*it)->smallshrines[x]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Medshrines");
      for (int x = 0; x < 50; x++)
      fprintf(fpout, " %d", (*it)->medshrines[x]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Bigshrines");
      for (int x = 0; x < 50; x++)
      fprintf(fpout, " %d", (*it)->bigshrines[x]);
      fprintf(fpout, "\n");

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    if (backup == FALSE)
    save_domains(TRUE);
  }
  void fread_domain(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    DOMAIN_TYPE *domain;

    domain = new_domain();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Archetype", domain->archetype, fread_number(fp));
        KEY("Ambients", domain->ambients, fread_string(fp));
        break;
      case 'B':
        if (!str_cmp(word, "Bigshrines")) {
          for (int i = 0; i < 50; i++)
          domain->bigshrines[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'D':
        KEY("Desc", domain->desc, fread_string(fp));
      case 'E':
        KEY("Exp", domain->exp, fread_number(fp));
        if (!str_cmp(word, "End")) {
          if (!domain->domain_of) {
            bug("Fread_domain: Domain not found.", 0);
            free_domain(domain);
            return;
          }
          DomainVect.push_back(domain);
          return;
        }
        break;
      case 'M':
        if (!str_cmp(word, "Medshrines")) {
          for (int i = 0; i < 50; i++)
          domain->medshrines[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'N':
        KEY("Name", domain->domain_of, fread_string(fp));
        break;
      case 'P':
        KEY("Power", domain->power, fread_number(fp));
        break;
      case 'R':
        if (!str_cmp(word, "Roomlist")) {
          for (int i = 0; i < 50; i++)
          domain->roomlist[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'S':
        if (!str_cmp(word, "Smallshrines")) {
          for (int i = 0; i < 50; i++)
          domain->smallshrines[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'V':
        KEY("Vnum", domain->vnum, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_domain: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_domains() {
    nulldomain = new_domain();
    FILE *fp;

    if ((fp = fopen("../data/domains.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_DOMAINS: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "DOMAIN")) {
          fread_domain(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_properties: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open domains.txt", 0);
      exit(0);
    }
  }

  void fread_property(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    PROP_TYPE *prop;

    prop = new_prop();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        if (!str_cmp(word, "Adate")) {
          prop->auction_day = fread_number(fp);
          prop->auction_month = fread_number(fp);
          if (prop->auction_month > 11)
          prop->auction_month = 0;
          /*
prop->auction_month -= 12;
*/
          fMatch = TRUE;
        }
        KEY("Address", prop->address, fread_string(fp));
        KEY("Advertisement", prop->advertisement, fread_string(fp));
        KEY("Autoclear", prop->autoclear, fread_number(fp));
        break;
      case 'B':
        KEY("Bankrupt", prop->bankrupt, fread_number(fp));
        KEY("Blackout", prop->blackout, fread_number(fp));
        break;
      case 'C':
        KEY("Cloaked", prop->cloaked, fread_number(fp));
        KEY("CreditProfit", prop->creditprofit, fread_number(fp));
        break;
      case 'D':
        KEY("Decay", prop->decay, fread_number(fp));
        break;
      case 'E':
        KEY("Electric", prop->electric, fread_number(fp));
        if (!str_cmp(word, "Excluded")) {
          for (int i = 0; i < 5; i++)
          prop->excluded[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "End")) {
          if (!prop->propname) {
            bug("Fread_property: Property not found.", 0);
            free_prop(prop);
            return;
          }
          PropVect.push_back(prop);
          return;
        }
        break;
      case 'F':
        KEY("Faction", prop->faction, fread_number(fp));
        KEY("FirstRoom", prop->firstroom, fread_number(fp));
        break;
      case 'G':
        break;
      case 'I':
        KEY("Industry", prop->industry, fread_number(fp));
        if (!str_cmp(word, "Included")) {
          for (int i = 0; i < 5; i++)
          prop->included[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Invitee")) {
          int i;
          for (i = 0; i < 50 && safe_strlen(prop->tenants[i]) > 1; i++) {
          }
          prop->tenants[i] = fread_string(fp);
          prop->tenant_trust[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'L':
        KEY("Logoffs", prop->logoffs, fread_number(fp));
        KEY("LastEncroach", prop->last_encroach, fread_number(fp));
        break;
      case 'M':
        KEY("MarketRoom", prop->market_room, fread_number(fp));
        KEY("MarketDir", prop->market_dir, fread_number(fp));
        break;
      case 'N':
        KEY("Name", prop->propname, fread_string(fp));
        KEY("NoChange", prop->nochange, fread_number(fp));
        break;
      case 'P':
        KEY("Price", prop->price, fread_number(fp));
        KEY("Profit", prop->profit, fread_number(fp));
        KEY("PropState", prop->prop_state, fread_number(fp));
        break;
      case 'O':
        KEY("Orientation", prop->orientation, fread_number(fp));
        if (!str_cmp(word, "Objects")) {
          for (int i = 0; i < 200; i++)
          prop->objects[i] = fread_number(fp);
          fMatch = TRUE;
        }

        if (!str_cmp(word, "Owner")) {
          prop->owner = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'R':
        KEY("Renovate", prop->renovate, fread_number(fp));
        KEY("RoadRoom", prop->roadroom, fread_number(fp));
        if (!str_cmp(word, "Range")) {
          prop->minx = fread_number(fp);
          prop->miny = fread_number(fp);
          prop->minz = fread_number(fp);
          prop->maxx = fread_number(fp);
          prop->maxy = fread_number(fp);
          prop->maxz = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Roomlist")) {
          for (int i = 0; i < 200; i++)
          prop->roomlist[i] = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'S':
        KEY("SalePrice", prop->sale_price, fread_number(fp));
        KEY("ShroudShield", prop->shroudshield, fread_number(fp));
        if (!str_cmp(word, "Sleeper")) {
          int i;
          for (i = 0; i < 50 && safe_strlen(prop->sleepers[i]) > 1; i++) {
          }
          prop->sleepers[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'T':
        KEY("Type", prop->type, fread_number(fp));
        KEY("TypeSpecial", prop->type_special, fread_number(fp));
        if (!str_cmp(word, "TimeWeather")) {
          prop->timefrozen = fread_number(fp);
          prop->timeshift = fread_number(fp);
          prop->tempfrozen = fread_number(fp);
          prop->tempshift = fread_number(fp);
          prop->weather = fread_number(fp);
          fMatch = TRUE;
        }
        break;
      case 'U':
        KEY("Utilities", prop->utilities, fread_number(fp));
        break;
      case 'V':
        KEY("Vnum", prop->vnum, fread_number(fp));
        break;
      case 'W':
        KEY("Warded", prop->warded, fread_number(fp));
        if (!str_cmp(word, "Worker")) {
          int i;
          for (i = 0; i < 20 && safe_strlen(prop->workers[i]) > 1; i++) {
          }
          prop->worker_idle[i] = fread_number(fp);
          prop->workers[i] = fread_string(fp);
          prop->worker_position[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_property: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_properties() {
    nullprop = new_prop();
    FILE *fp;

    if ((fp = fopen("../data/properties.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_PROPERTIES: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PROPERTY")) {
          fread_property(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_properties: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open properties.txt", 0);
      exit(0);
    }
  }

  bool prop_exists(PROP_TYPE *prop) {
    ROOM_INDEX_DATA *room = first_room_in_property(prop);
    if (room == NULL)
    return FALSE;
    if (room->sector_type != SECT_FOREST && room->sector_type != SECT_AIR && room->sector_type != SECT_STREET)
    return TRUE;
    for (int i = 0; i < 300; i++) {
      room = next_room_in_property(prop, room);
      if (room == NULL)
      return FALSE;
      if (room->sector_type != SECT_FOREST && room->sector_type != SECT_AIR && room->sector_type != SECT_STREET)
      return TRUE;
    }
    return FALSE;
  }

  void save_properties(bool backup) {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {

      if ((*it)->type == PROP_HOUSE && !house_exists((*it)->vnum - 1) && (*it)->vnum > 1)
      (*it)->vnum--;

      if ((*it)->type == PROP_SHOP && !shop_exists((*it)->vnum - 1) && (*it)->vnum > 1)
      (*it)->vnum--;

      if ((*it)->type == PROP_HOUSE && house_count((*it)->vnum) > 1 && (*it)->vnum > 1)
      (*it)->vnum++;
      if ((*it)->type == PROP_SHOP && shop_count((*it)->vnum) > 1 && (*it)->vnum > 1)
      (*it)->vnum++;

      (*it)->minz = town_minz(NULL, (*it));
      (*it)->maxz = town_maxz(NULL, (*it));

      if (number_percent() % 7 == 0 || (*it)->lightcount == -1 || (*it)->realhouse == -1) {
        (*it)->price = property_truevalue((*it));
        if (!str_cmp((*it)->owner, "NPC"))
        (*it)->sale_price = (*it)->price / 2;
      }

      if (number_percent() % 12 == 0) {
        for (int i = 0; i < 199; i++) {
          if ((*it)->roomlist[i] == 0 && (*it)->roomlist[i + 1] != 0) {
            (*it)->roomlist[i] = (*it)->roomlist[i + 1];
            (*it)->roomlist[i + 1] = 0;
          }
          if ((*it)->objects[i] == 0 && (*it)->objects[i + 1] != 0) {
            (*it)->objects[i] = (*it)->objects[i + 1];
            (*it)->objects[i + 1] = 0;
          }
          for (int j = i + 1; j < 199; j++) {
            if ((*it)->roomlist[i] == (*it)->roomlist[j])
            (*it)->roomlist[j] = 0;
            if ((*it)->objects[i] == (*it)->objects[j])
            (*it)->objects[j] = 0;
          }
        }
      }
    }

    std::sort(PropVect.begin(), PropVect.end(), earlier_prop);

    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/properties.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/properties.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/properties.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/properties.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/properties.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/properties.txt");
      else
      sprintf(buf, "../data/back7/properties.txt");
      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open properties.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/properties.txt", "w")) == NULL) {
        bug("Cannot open properties.txt for writing", 0);
        return;
      }
    }

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        bug("Save_houses: Blank property in vector", i);
        continue;
      }
      if (!str_cmp((*it)->propname, "delete") || (*it)->valid == FALSE)
      continue;

      if (!prop_exists((*it)))
      continue;
/*
            if((*it)->type_special == PROPERTY_INNERFOREST)
              continue;
            if((*it)->type_special == PROPERTY_OUTERFOREST)
              continue;
*/
      fprintf(fpout, "#PROPERTY\n");
      fprintf(fpout, "Vnum   %d\n", (*it)->vnum);
      fprintf(fpout, "Name      %s~\n", trimwhitespace((*it)->propname));
      fprintf(fpout, "Type %d\n", (*it)->type);
      fprintf(fpout, "Industry %d\n", (*it)->industry);
      fprintf(fpout, "Electric %d\n", (*it)->electric);
      fprintf(fpout, "Profit %d\n", (*it)->profit);
      fprintf(fpout, "CreditProfit %d\n", (*it)->creditprofit);
      fprintf(fpout, "LastEncroach %d\n", (*it)->last_encroach);
      fprintf(fpout, "Bankrupt %d\n", (*it)->bankrupt);
      fprintf(fpout, "Renovate %d\n", (*it)->renovate);
      fprintf(fpout, "Address %s~\n", (*it)->address);
      fprintf(fpout, "SalePrice %d\n", (*it)->sale_price);
      fprintf(fpout, "Blackout %d\n", (*it)->blackout);
      fprintf(fpout, "Warded %d\n", (*it)->warded);
      fprintf(fpout, "Decay %d\n", (*it)->decay);
      fprintf(fpout, "Cloaked %d\n", (*it)->cloaked);
      fprintf(fpout, "Logoffs %d\n", (*it)->logoffs);
      fprintf(fpout, "MarketRoom %d\n", (*it)->market_room);
      fprintf(fpout, "MarketDir %d\n", (*it)->market_dir);
      fprintf(fpout, "Faction %d\n", (*it)->faction);
      fprintf(fpout, "Price  %d\n", (*it)->price);
      fprintf(fpout, "ADate %d %d\n", (*it)->auction_day, (*it)->auction_month);
      fprintf(fpout, "Owner %s~\n", (*it)->owner);
      fprintf(fpout, "Advertisement %s~\n", (*it)->advertisement);
      fprintf(fpout, "PropState %d\n", (*it)->prop_state);
      fprintf(fpout, "Autoclear %d\n", (*it)->autoclear);
      fprintf(fpout, "NoChange %d\n", (*it)->nochange);
      fprintf(fpout, "Utilities %d\n", (*it)->utilities);
      fprintf(fpout, "Range %d %d %d %d %d %d\n", (*it)->minx, (*it)->miny, (*it)->minz, (*it)->maxx, (*it)->maxy, (*it)->maxz);
      fprintf(fpout, "TimeWeather %d %d %d %d %d\n", (*it)->timefrozen, (*it)->timeshift, (*it)->tempfrozen, (*it)->tempshift, (*it)->weather);
      fprintf(fpout, "ShroudShield %d\n", (*it)->shroudshield);
      fprintf(fpout, "Orientation %d\n", (*it)->orientation);
      fprintf(fpout, "TypeSpecial %d\n", (*it)->type_special);
      fprintf(fpout, "FirstRoom %d\n", (*it)->firstroom);
      fprintf(fpout, "RoadRoom %d\n", (*it)->roadroom);
      fprintf(fpout, "Included");
      for (int x = 0; x < 5; x++)
      fprintf(fpout, " %d", (*it)->included[x]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Excluded");
      for (int x = 0; x < 5; x++)
      fprintf(fpout, " %d", (*it)->excluded[x]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Objects");
      for (int x = 0; x < 200; x++)
      fprintf(fpout, " %d", (*it)->objects[x]);
      fprintf(fpout, "\n");
      fprintf(fpout, "Roomlist");
      for (int x = 0; x < 200; x++)
      fprintf(fpout, " %d", (*it)->roomlist[x]);
      fprintf(fpout, "\n");

      for (i = 0; i < 50; i++) {
        if (safe_strlen((*it)->tenants[i]) > 1)
        fprintf(fpout, "Invitee %s~\n %d\n", (*it)->tenants[i], (*it)->tenant_trust[i]);
      }
      for (i = 0; i < 50; i++) {
        if (safe_strlen((*it)->sleepers[i]) > 1)
        fprintf(fpout, "Sleeper %s~\n", (*it)->sleepers[i]);
      }
      for (i = 0; i < 20; i++) {
        if (safe_strlen((*it)->workers[i]) > 1)
        fprintf(fpout, "Worker %d %s~\n%s~\n", (*it)->worker_idle[i], (*it)->workers[i], (*it)->worker_position[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    if (backup == FALSE)
    save_properties(TRUE);
  }

  int get_transfer_point(CHAR_DATA *ch) {
    HOUSE_TYPE *house = get_house(ch);

    if (house == nullhouse || house == NULL)
    return 4207;

    return 4207;
  }

  _DOFUN(do_rproom) {
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);
    if (ch->in_room->area->vnum != 110) {
      send_to_char("You can't do that here.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "name")) {
      free_string(ch->in_room->name);
      ch->in_room->name = str_dup(argument);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "description")) {
      string_append(ch, &ch->in_room->description);
      SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
    }
  }

  // Returns month as a word.  Old solutions seem scattered and suspicious -
  // Discordance
  char *month_string(int monthint) {
    char monthstr[MSL];

    if (monthint == 0)
    strcpy(monthstr, "January");
    else if (monthint == 1)
    strcpy(monthstr, "February");
    else if (monthint == 2)
    strcpy(monthstr, "March");
    else if (monthint == 3)
    strcpy(monthstr, "April");
    else if (monthint == 4)
    strcpy(monthstr, "May");
    else if (monthint == 5)
    strcpy(monthstr, "June");
    else if (monthint == 6)
    strcpy(monthstr, "July");
    else if (monthint == 7)
    strcpy(monthstr, "August");
    else if (monthint == 8)
    strcpy(monthstr, "September");
    else if (monthint == 9)
    strcpy(monthstr, "October");
    else if (monthint == 10)
    strcpy(monthstr, "November");
    else if (monthint == 11)
    strcpy(monthstr, "December");
    else
    strcpy(monthstr, "Error");

    return str_dup(monthstr);
  }

  _DOFUN(do_holdings)
  // A command to list and manipulate only property which is owned by the player
  // regardless of type - Discordance
  {
    char arg1[MSL];
    char arg2[MSL];
    char month[MSL];
    char buf[MSL] = "";
    char page[MSL] = "";
    char spacer[] = "";
    char *frame;
    int spacing;
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    sprintf(buf, "`WHoldings`x\n\r");
    strcpy(buf, "`g_");
    frame = draw_horizontal_line(ch->linewidth - 2);
    strcat(buf, frame);
    strcat(buf, "_`x\n\r");
    strcat(page, buf);

    // Lists only shops
    if (!str_cmp(arg1, "shops") || !str_cmp(arg1, "shop")) {
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0') {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          sprintf(buf, "`g[`W%3d`g]`x %-s\n\r", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "      ");
          strcat(page, buf);
          sprintf(buf, "`gAddress`x: %-s", (*it)->address);
          strcat(page, buf);
          spacing = 48 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gProperty Type`x: Commercial`x\n\r");
          strcat(page, buf);

          sprintf(buf, "     ");
          strcat(page, buf);
          sprintf(buf, "`gDue Date`x: %s %d%s", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
          spacing = 52 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gAmount Due`x: `G$`x%-4d\n\r\n\r", (*it)->price);
          strcat(page, buf);
        }
      }
    }
    // Lists only houses
    else if (!str_cmp(arg1, "houses") || !str_cmp(arg1, "house")) {
      for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
      it != HouseVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->house_name || (*it)->house_name[0] == '\0' || (*it)->auction_day > 35 || (*it)->auction_day < 0 || (*it)->auction_month < 1 || (*it)->auction_month > 12) {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          sprintf(buf, "`g[`W%3d`g]`x %s\n\r", (*it)->vnum, (*it)->house_name);
          strcat(page, buf);

          sprintf(buf, "      ");
          strcat(page, buf);
          sprintf(buf, "`gAddress`x: %-s", (*it)->address);
          strcat(page, buf);
          spacing = 48 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gProperty Type`x: Residence\n\r");
          strcat(page, buf);

          sprintf(buf, "     ");
          strcat(page, buf);
          sprintf(buf, "`gDue Date`x: %s %d%s", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
          spacing = 52 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gAmount Due`x: `G$`x%-4d\n\r\n\r", (*it)->price);
          strcat(page, buf);
        }
      }
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0') {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          sprintf(buf, "`g[`W%3d`g]`x %s\n\r", (*it)->vnum, (*it)->propname);
          strcat(page, buf);

          sprintf(buf, "      ");
          strcat(page, buf);
          sprintf(buf, "`gAddress`x: %-s", (*it)->address);
          strcat(page, buf);
          spacing = 48 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gProperty Type`x: Residence\n\r");
          strcat(page, buf);

          sprintf(buf, "     ");
          strcat(page, buf);
          sprintf(buf, "`gDue Date`x: %s %d%s", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
          spacing = 52 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gAmount Due`x: `G$`x%-4d\n\r\n\r", (*it)->price);
          strcat(page, buf);
        }
      }
    }
    // Assumes 'list' argument in event the argument isn't understood.
    else {
      for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
      it != HouseVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->house_name || (*it)->house_name[0] == '\0' || (*it)->auction_day > 35 || (*it)->auction_day < 0 || (*it)->auction_month < 1 || (*it)->auction_month > 12) {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          sprintf(buf, "`g[`W%3d`g]`x %s\n\r", (*it)->vnum, (*it)->house_name);
          strcat(page, buf);

          sprintf(buf, "      ");
          strcat(page, buf);
          sprintf(buf, "`gAddress`x: %-s", (*it)->address);
          strcat(page, buf);
          spacing = 48 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gProperty Type`x: Residence\n\r");
          strcat(page, buf);

          sprintf(buf, "     ");
          strcat(page, buf);
          sprintf(buf, "`gDue Date`x: %s %d%s", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
          spacing = 52 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gAmount Due`x: `G$`x%-4d\n\r\n\r", (*it)->price);
          strcat(page, buf);
        }
      }
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) {
            continue;
          }
          if ((*it)->type == PROP_HOUSE) {
            sprintf(buf, "`g[`W%3d`g]`x %s\n\r", (*it)->vnum, (*it)->propname);
            strcat(page, buf);
            sprintf(buf, "      ");
            strcat(page, buf);
            sprintf(buf, "`gAddress`x: %-s", (*it)->address);
            strcat(page, buf);
            spacing = 48 - safe_strlen_color(buf);
            sprintf(buf, "%*s", spacing, spacer);
            strcat(page, buf);
            sprintf(buf, "`gProperty Type`x: Residence\n\r");
            strcat(page, buf);
          }
          else {
            sprintf(buf, "`g[`W%3d`g]`x %-s\n\r", (*it)->vnum, (*it)->propname);
            strcat(page, buf);
            sprintf(buf, "      ");
            strcat(page, buf);
            sprintf(buf, "`gAddress`x: %-s", (*it)->address);
            strcat(page, buf);
            spacing = 48 - safe_strlen_color(buf);
            sprintf(buf, "%*s", spacing, spacer);
            strcat(page, buf);
            sprintf(buf, "`gProperty Type`x: Commercial\n\r");
            strcat(page, buf);
          }

          strcpy(month, month_string((*it)->auction_month));

          sprintf(buf, "     ");
          strcat(page, buf);
          sprintf(buf, "`gDue Date`x: %s %d%s", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
          spacing = 52 - safe_strlen_color(buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gAmount Due`x: `G$`x%-4d\n\r\n\r", (*it)->price);
          strcat(page, buf);
        }
      }
    }
    page_to_char(page, ch);
    frame = str_dup("");
    free_string(frame);
    return;
  }

  void abandon_property(PROP_TYPE *prop) {
    offline_message(prop->owner, "Your property is abandoned.\n\r");

    for (int i = 0; i < 50; i++) {
      free_string(prop->tenants[i]);
      prop->tenants[i] = str_dup("");
    }
    prop->bankrupt = 0;
    free_string(prop->owner);
    prop->owner = str_dup("NPC");
    prop->sale_price = prop->price / 2;
  }

  void view_prop_roomlist(PROP_TYPE *prop, CHAR_DATA *ch) {
    ROOM_INDEX_DATA *start = first_room_in_property(prop);
    if (start == NULL)
    return;
    printf_to_char(ch, "Starting, %d:%s:%d", start->vnum, room_tags(get_room_index(start->vnum)), room_value(get_room_index(start->vnum)));
    for (int i = 0; i < 300; i++) {
      start = next_room_in_property(prop, start);
      if (start == NULL)
      return;
      printf_to_char(ch, ", %d:%s:%d", start->vnum, room_tags(get_room_index(start->vnum)), room_value(get_room_index(start->vnum)));
    }
  }

  void airbubble_property(PROP_TYPE *prop) {

    ROOM_INDEX_DATA *start = first_room_in_property(prop);
    if (start == NULL)
    return;
    if (start->z > 0)
    airbubble(start, FALSE);
    for (int i = 0; i < 300; i++) {
      start = next_room_in_property(prop, start);
      if (start == NULL)
      return;
      if (start->z > 0)
      airbubble(start, FALSE);
    }
  }

  void drop_prop_room(PROP_TYPE *prop, int vnum) {
    for (int i = 0; i < 200; i++) {
      if (prop->roomlist[i] == vnum)
      prop->roomlist[i] = prop->roomlist[i + 1];
    }
  }

  int owned_shop_count(CHAR_DATA *ch) {
    int count = 0;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
        continue;
      }
      if (!str_cmp(ch->name, (*it)->owner))
      count++;
    }
    return count;
  }
  int owned_town_shop_count(CHAR_DATA *ch) {
    int count = 0;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
        continue;
      }
      if((*it)->type_special == PROPERTY_INNERFOREST ||  (*it)->type_special == PROPERTY_OUTERFOREST)
      continue;
      if (!str_cmp(ch->name, (*it)->owner))
      count++;
    }
    return count;
  }

  int owned_house_count(CHAR_DATA *ch) {
    int count = 0;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type == PROP_SHOP || (*it)->valid == FALSE) {
        continue;
      }
      if (!str_cmp(ch->name, (*it)->owner))
      count++;
    }
    return count;
  }

  _DOFUN(do_house) {
    char arg1[MSL];
    char arg2[MSL];
    char spacer[] = "";
    char month[MSL];
    char page[MSL] = "";
    char buf[MSL] = "";
    char *frame;
    int i, spacing, month_string_length = 0, longest_month_string = 0;
    HOUSE_TYPE *house;
    PROP_TYPE *prop;
    CHAR_DATA *victim;
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    if (ch->race == RACE_FANTASY)
    return;

    if (!str_cmp(arg1, "list")) {
      // This stuff is for formatting and automatically aligning columns -
      // Discordance
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
          continue;
        }
        strcpy(month, month_string((*it)->auction_month));
        month_string_length = safe_strlen(month);
        if (month_string_length > longest_month_string) {
          longest_month_string = month_string_length;
        }
      }
      sprintf(page, "`WHouse List`x\n\r");
      strcpy(buf, "`g_");
      frame = draw_horizontal_line(ch->linewidth - 2);
      strcat(buf, frame);
      strcat(buf, "_`x\n\r");
      strcat(page, buf);
      if (!str_cmp(arg2, "township") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "town") || !str_cmp(arg2, "haven")) {
        strcat(page, "`WHaven Township`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_TOWN)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (!str_cmp(arg2, "forest") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "inner") || !str_cmp(arg2, "forests") || !str_cmp(arg2, "haven")) {
        strcat(page, "`WHaven Forest`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_INNER)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (!str_cmp(arg2, "outer") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "otherwordly") || !str_cmp(arg2, "deep"))) {
        strcat(page, "`WOtherworldly Forest`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_OUTER)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (!str_cmp(arg2, "wilds") || safe_strlen(arg2) < 2)) {
        strcat(page, "`WWilds`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_WILDS)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (safe_strlen(arg2) < 2 || !str_cmp(arg2, "fae") || !str_cmp(arg2, "other"))) {
        strcat(page, "`WFae Other`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_OTHER)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (!str_cmp(arg2, "godrealm") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "godrealms"))) {
        strcat(page, "`WGodrealm`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_GODREALM)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (!str_cmp(arg2, "hell") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "hell"))) {
        strcat(page, "`WHell`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_HELL)
          continue;
          strcpy(month, month_string((*it)->auction_month));
          if ((*it)->sale_price > 0)
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          else
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }

      page_to_char(page, ch);
      frame = str_dup("");
      free_string(frame);
      return;
    }
    // Lists houses owned by the player - Discordance
    else if (!str_cmp(arg1, "owned")) {
      // formatting columns

      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          if (month_string_length > longest_month_string) {
            longest_month_string = month_string_length;
          }
        }
      }

      sprintf(page, "`WHouse Owned`x\n\r");
      strcpy(buf, "`g_");
      frame = draw_horizontal_line(ch->linewidth - 2);
      strcat(buf, frame);
      strcat(buf, "_`x\n\r");
      strcat(page, buf);
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
            continue;
          }

          strcpy(month, month_string((*it)->auction_month));
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->address);
          strcat(page, buf);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->address);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gPayment:`x ");
          strcat(page, buf);
          month_string_length = safe_strlen(month);
          if (month_string_length < longest_month_string) {
            spacing = longest_month_string - month_string_length;
            sprintf(buf, "%*s", spacing, spacer);
            month_string_length = safe_strlen(month);
            strcat(page, buf);
          }
          sprintf(buf, "%s %2d%2s\n\r", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
        }
      }
      page_to_char(page, ch);
      frame = str_dup("");
      free_string(frame);
      return;
    }
    else if (!str_cmp(arg1, "name")) {
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("You need to own this house or have permission to do that.\n\r", ch);
        return;
      }
      if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_HOUSE) {
        send_to_char("You need to own this house or have permission to do that.\n\r", ch);
        return;
      }
      char buf[MSL];
      sprintf(buf, "%s %s", arg2, argument);
      if (in_prop(ch) != NULL) {
        PROP_TYPE *nprop = in_prop(ch);
        free_string(nprop->propname);
        nprop->propname = str_dup(buf);
        send_to_char("Done.\n\r", ch);
      }
    }
    else if (!str_cmp(arg1, "update")) {
      if (IS_IMMORTAL(ch))
      house_auction_update();
    }
    else if (!str_cmp(arg1, "addbasement")) {
      if (IS_IMMORTAL(ch)) {
        if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
          send_to_char("No such shop.\n\r", ch);
          return;
        }
        if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
          send_to_char("No such house.\n\r", ch);
          return;
        }
        else {
          if (prop->minz > -3) {
            send_to_char("No basement space.\n\r", ch);
            return;
          }
          build_basement(prop);
          send_to_char("Basement built.\n\r", ch);
          return;
        }
      }
    }

    // This just looks at the property's rent due month and sets it appropriately
    // - Discordance
    else if (!str_cmp(arg1, "fixmonth")) {
      if (IS_IMMORTAL(ch)) {
        if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
          send_to_char("No such house.\n\r", ch);
          return;
        }
        if ((house = house_lookup(atoi(arg2))) == nullhouse || house == NULL) {
          if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
            send_to_char("No such house.\n\r", ch);
            return;
          }
          else {
            if (get_day() >= prop->auction_day) {
              if (get_month() < 11) {
                prop->auction_month = get_month() + 1;
              }
              else {
                prop->auction_month = 0;
              }
            }
            else {
              prop->auction_month = get_month();
            }
            return;
          }
        }
        if (get_day() >= house->auction_day) {
          if (get_month() < 11) {
            house->auction_month = get_month() + 1;
          }
          else {
            house->auction_month = 0;
          }
        }
        else {
          house->auction_month = get_month();
        }
        return;
      }

    }
    else if (!str_cmp(arg1, "clear")) {

      if (atoi(arg2) > 33 || atoi(arg2) < 1) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      if (!IS_IMMORTAL(ch))
      return;
      if ((house = house_lookup(atoi(arg2))) == nullhouse || house == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }

      free_string(house->owner);
      house->owner = str_dup("");
      for (i = 0; i < 50; i++) {
        free_string(house->tenants[i]);
        house->tenants[i] = str_dup("");
        house->tenant_trust[i] = 0;
      }
      house->current_price = house->price;
      if (get_day() >= house->auction_day) {
        house->auction_month = get_month() + 1;
      }
      else
      house->auction_month = get_month();

      if (house->auction_month > 11)
      house->auction_month = 0;
      /*
if(house->auction_month > 12)
house->auction_month -= 12;
*/
    }
    else if (!str_cmp(arg1, "airbubble")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        airbubble_property(prop);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "burndown")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        free_string(prop->propname);
        prop->propname = str_dup("delete");
        prop->valid = FALSE;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "droproom")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        drop_prop_room(prop, atoi(argument));
      }
    }
    else if (!str_cmp(arg1, "resize")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        char arg3[MSL];
        argument = one_argument_nouncap(argument, arg3);
        char arg4[MSL];
        argument = one_argument_nouncap(argument, arg4);
        char arg5[MSL];
        argument = one_argument_nouncap(argument, arg5);
        char arg6[MSL];
        argument = one_argument_nouncap(argument, arg6);

        prop->minx = atoi(arg3);
        prop->maxx = atoi(arg4);
        prop->miny = atoi(arg5);
        prop->maxy = atoi(arg6);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "reprice")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        prop->price = atoi(argument);
        return;
      }
    }
    else if (!str_cmp(arg1, "newvnum")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        prop->vnum = atoi(argument);
        return;
      }
    }
    else if (!str_cmp(arg1, "newowner")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        free_string(prop->owner);
        prop->owner = str_dup(argument);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "newaddress")) {
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such house.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name) && !IS_IMMORTAL(ch)) {
        send_to_char("You don't own that house.\n\r", ch);
        return;
      }

      free_string(prop->address);
      prop->address = str_dup(
      create_address(get_room_index(prop->roadroom), prop->orientation));
      free_string(prop->propname);
      prop->propname = str_dup(
      create_address(get_room_index(prop->roadroom), prop->orientation));
      printf_to_char(ch, "The propery is now located at %s.\n\r", prop->address);
      return;
    }
    else if (!str_cmp(arg1, "sell")) {
      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own any houses.\n\r", ch);
        return;
      }
      else if (is_base(prop)) {
        send_to_char("You can't sell a faction base.\n\r", ch);
        return;
      }
      else {
        prop->sale_price = atoi(argument);
        printf_to_char(ch, "Property sale price set at %d.\n\r", atoi(argument));
      }
    }
    else if (!str_cmp(arg1, "automaintain")) {
      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      if (prop->type_special != PROPERTY_INNERFOREST && prop->type_special != PROPERTY_OUTERFOREST) {
        send_to_char("This property doesn't need regular maintenence.\n\r", ch);
        return;
      }
      if (prop->autoclear == 0) {
        prop->autoclear = 1;
        send_to_char("You hire someone to keep the property in good repair.\n\r", ch);
        return;
      }
      prop->autoclear = 0;
      send_to_char("You stop paying for maintenence.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "utilities")) {
      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      if (prop->type_special != PROPERTY_OUTERFOREST) {
        send_to_char("This property doesn't need special provisions for utilities.\n\r", ch);
        return;
      }
      if (prop->utilities == 0) {
        prop->utilities = 1;
        send_to_char("You set up utilities access for your property.\n\r", ch);
        ch->pcdata->total_money -= 10000;
        return;
      }
      prop->utilities = 0;
      send_to_char("You stop paying for utilities.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "workers")) {
      if (is_helpless(ch))
      return;

      prop = in_prop(ch);
      if (prop == NULL || str_cmp(ch->name, prop->owner)) {
        send_to_char("You don't own this shop.\n\r", ch);
        return;
      }
      else {
        printf_to_char(ch, "Owner: %s\n\r", prop->owner);

        for (i = 0; i < 20; i++) {
          if (safe_strlen(prop->workers[i]) > 1) {
            printf_to_char(ch, "Employee: %s working as %s\n\r", prop->workers[i], prop->worker_position[i]);
          }
        }
        return;
      }
    }
    else if (!str_cmp(arg1, "fire")) {
      if (is_helpless(ch))
      return;

      if (safe_strlen(arg2) < 2) {
        send_to_char("Fire whom?\n\r", ch);
        return;
      }
      prop = in_prop(ch);
      if (prop == NULL || str_cmp(ch->name, prop->owner)) {
        send_to_char("You don't own this shop.\n\r", ch);
        return;
      }
      else {
        for (i = 0; i < 20; i++) {
          if (!str_cmp(arg2, prop->workers[i])) {
            if (get_room_index(
                  get_char_world_pc(prop->workers[i])->pcdata->job_room_one) !=
                NULL && prop_from_room(get_room_index(
                    get_char_world_pc(prop->workers[i])->pcdata->job_room_one)) ==
                prop) {
              free_string(
              get_char_world_pc(prop->workers[i])->pcdata->job_title_one);
              get_char_world_pc(prop->workers[i])->pcdata->job_title_one =
              str_dup("");
              get_char_world_pc(prop->workers[i])->pcdata->job_type_one =
              JOB_UNEMPLOYED;
            }
            if (get_room_index(
                  get_char_world_pc(prop->workers[i])->pcdata->job_room_two) !=
                NULL && prop_from_room(get_room_index(
                    get_char_world_pc(prop->workers[i])->pcdata->job_room_two)) ==
                prop) {
              free_string(
              get_char_world_pc(prop->workers[i])->pcdata->job_title_two);
              get_char_world_pc(prop->workers[i])->pcdata->job_title_two =
              str_dup("");
              get_char_world_pc(prop->workers[i])->pcdata->job_type_two =
              JOB_UNEMPLOYED;
            }
            send_to_char("Done.\n\r", ch);

            if (!offline_flag(prop->workers[i], PLR_NOPAY))
            offline_setflag(prop->workers[i], PLR_NOPAY);
            offline_message(prop->workers[i], "You get fired.");
            free_string(prop->workers[i]);
            prop->workers[i] = str_dup("");
          }
        }
      }
    }
    else if (!str_cmp(arg1, "abandon")) {
      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own any houses.\n\r", ch);
        return;
      }
      else if (is_base(prop)) {
        send_to_char("You can't abandon a faction base.\n\r", ch);
        return;
      }
      else {
        abandon_property(prop);
        send_to_char("You abandon the property.\n\r", ch);
      }
    }
    else if (!str_cmp(arg1, "buy")) {
      if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else if ((is_gm(ch) && !IS_IMMORTAL(ch)) || is_guest(ch)) {
        send_to_char("Guests and Story runners can't buy properties.\n\r", ch);
        return;
      }
      if (!str_cmp(prop->owner, NAME(ch))) {
        send_to_char("You already own that.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
      {
        send_to_char("They don't seem willing to sell.\n\r", ch);
        return;
      }

      if (prop->sale_price < 1 && daysidle(prop->owner) < 90) {
        send_to_char("They don't seem willing to sell.\n\r", ch);
        return;
      }
      if (ch->pcdata->total_money <
          UMAX(prop->sale_price, prop->price / 10) * 100) {
        printf_to_char(ch, "You'd need %d dollars to take that over.\n\r", UMAX(prop->sale_price, prop->price / 10));
        return;
      }
      if (higher_power(ch) && owned_house_count(ch) > 0) {
        send_to_char("You can only own one house.\n\r", ch);
        return;
      }

      house_charge(prop->owner, UMAX(prop->price / 10, prop->sale_price) * -1);
      free_string(prop->owner);
      prop->owner = str_dup(ch->name);
      send_to_char("You take over the property.\n\r", ch);
      prop->bankrupt = 0;
      ch->pcdata->total_money -= UMAX(prop->price / 10, prop->sale_price) * 100;
      prop->sale_price = 0;
      return;
    }
    else if (!str_cmp(arg1, "info")) {
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      if ((house = house_lookup(atoi(arg2))) == nullhouse || house == NULL) {
        if ((prop = prop_lookup_house(atoi(arg2))) == NULL) {
          send_to_char("No such house.\n\r", ch);
          return;
        }
        else {
          strcpy(month, month_string(prop->auction_month));
          printf_to_char(ch, "Address: %s\n\r", prop->address);
          printf_to_char(ch, "Value: $%d\n\r", prop->price);
          if (prop->sale_price > 0)
          printf_to_char(ch, "For sale for: $%d\n\r", prop->sale_price);
          printf_to_char(ch, "Payments: %s %d%s\n\r", month, prop->auction_day, datesuffix(prop->auction_day));
          if (IS_IMMORTAL(ch)) {
            printf_to_char(ch, "Owner: %s\n\r", prop->owner);
            printf_to_char(ch, "Tvalue: %d\n\r", property_truevalue(prop));
            printf_to_char(ch, "Max: %d, %d, %d\n\r", prop->maxx, prop->maxy, prop->maxz);
            printf_to_char(ch, "Min: %d, %d, %d\n\r", prop->minx, prop->miny, prop->minz);
            int value = 0;
            int sizex = (prop->maxx - prop->minx) + 1;
            int sizey = (prop->maxy - prop->miny) + 1;
            if (prop->type_special == PROPERTY_TOWNHOUSE || prop->type_special == PROPERTY_TOWNSHOP || prop->type_special == PROPERTY_LONGSHOP || prop->type_special == PROPERTY_LONGHOUSE) {
              value = sizex * sizey * TOWNLAND_PRICE;
            }
            if (prop->type_special == PROPERTY_TRAILER) {
              value = sizex * sizey * TOWNLAND_PRICE * 3 / 5;
            }
            else if (prop->type_special == PROPERTY_OVERFLAT || prop->type_special == PROPERTY_UNDERSHOP) {
              value = sizex * sizey * TOWNLAND_PRICE * 3 / 5;
            }
            else if (prop->type_special == PROPERTY_SMALLFLAT || prop->type_special == PROPERTY_MEDFLAT || prop->type_special == PROPERTY_BIGFLAT) {
              value = sizex * sizey * TOWNLAND_PRICE;
            }
            else if (prop->type_special == PROPERTY_INNERFOREST)
            value = sizex * sizey * FORESTLAND_PRICE;

            printf_to_char(ch, "LandValue: %d\n\r", value);

            view_prop_roomlist(prop, ch);
          }
          return;
        }
      }
      printf_to_char(ch, "Address: %s\n\r", house->address);
      if (safe_strlen(house->owner) > 1 && str_cmp(house->owner, "npc")) {
        printf_to_char(ch, "Occupied.\n\r");
        if (IS_IMMORTAL(ch)) {
          printf_to_char(ch, "Owner: %s\n\r", house->owner);
        }
      }
      else
      printf_to_char(ch, "Vaccant.\n\r");

      printf_to_char(ch, "Reserve Price: $%d\n\r", house->price);
      strcpy(month, month_string(house->auction_month));
      printf_to_char(ch, "Auction: %s %d%s\n\r", month, house->auction_day, datesuffix(house->auction_day));
    }
    else if (!str_cmp(arg1, "roster")) {
      if (is_helpless(ch))
      return;
      house = get_house(ch);
      if (house == nullhouse || house == NULL) {
        prop = in_prop(ch);
        if (prop == NULL || str_cmp(ch->name, prop->owner)) {
          send_to_char("You don't own any houses.\n\r", ch);
          return;
        }
        else {
          printf_to_char(ch, "Owner: %s\n\r", prop->owner);

          for (i = 0; i < 50; i++) {
            if (safe_strlen(prop->tenants[i]) > 1) {
              printf_to_char(ch, "Tenant: %s\n\r", prop->tenants[i]);
            }
          }
          return;
        }
      }
      printf_to_char(ch, "Owner: %s\n\r", house->owner);

      for (i = 0; i < 50; i++) {
        if (safe_strlen(house->tenants[i]) > 1) {
          printf_to_char(ch, "Tenant: %s\n\r", house->tenants[i]);
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "trust")) {
      house = in_house(ch);
      if (house == nullhouse || house == NULL) {
        prop = in_prop(ch);
        if (prop == NULL) {
          send_to_char("You aren't in a house.\n\r", ch);
          return;
        }
        else {
          if (str_cmp(prop->owner, NAME(ch))) {
            send_to_char("You don't own any houses.\n\r", ch);
            return;
          }
          if (arg2[0] == '\0') {
            send_to_char("Syntax: house trust <person>\n\r", ch);
            return;
          }
          for (i = 0; i < 50; i++) {
            if (!str_cmp(arg2, prop->tenants[i])) {
              if (prop->tenant_trust[i] == 0) {
                prop->tenant_trust[i] = 1;
                printf_to_char(ch, "You trust %s with the household expenses.\n\r", prop->tenants[i]);
                return;
              }
              else {
                prop->tenant_trust[i] = 0;
                printf_to_char(
                ch, "You stop trusting %s with the household expenses.\n\r", prop->tenants[i]);
                return;
              }
            }
          }
          send_to_char("No such tenant to be trusted.\n\r", ch);
          return;
        }
      }
      if (str_cmp(house->owner, NAME(ch))) {
        send_to_char("You don't own any houses.\n\r", ch);
        return;
      }
      if (arg2[0] == '\0') {
        send_to_char("Syntax: house trust <person>\n\r", ch);
        return;
      }
      for (i = 0; i < 50; i++) {
        if (!str_cmp(arg2, house->tenants[i])) {
          if (house->tenant_trust[i] == 0) {
            house->tenant_trust[i] = 1;
            printf_to_char(ch, "You trust %s with the household expenses.\n\r", house->tenants[i]);
            return;
          }
          else {
            house->tenant_trust[i] = 0;
            printf_to_char(
            ch, "You stop trusting %s with the household expenses.\n\r", house->tenants[i]);
            return;
          }
        }
      }
      send_to_char("No such tenant to be trusted.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "lockchange")) {
      house = in_house(ch);
      if (house == nullhouse || house == NULL) {
        prop = in_prop(ch);
        if (prop == NULL) {
          send_to_char("You must be in a house to do that.\n\r", ch);
          return;
        }
        else {
          if (str_cmp(prop->owner, NAME(ch)) && !IS_IMMORTAL(ch)) {
            send_to_char("You don't own any houses.\n\r", ch);
            return;
          }
          if (arg2[0] == '\0') {
            send_to_char("Syntax: house lockchange <person>\n\r", ch);
            return;
          }
          for (i = 0; i < 50; i++) {
            if (!str_cmp(arg2, prop->tenants[i])) {
              free_string(prop->tenants[i]);
              prop->tenants[i] = str_dup("");
              prop->tenant_trust[i] = 0;
            }
          }
          printf_to_char(ch, "You change the locks to reject the keys of %s.\n\r", arg2);

          return;
        }
      }
      if (str_cmp(house->owner, NAME(ch)) && !IS_IMMORTAL(ch)) {
        send_to_char("You don't own any houses.\n\r", ch);
        return;
      }
      if (arg2[0] == '\0') {
        send_to_char("Syntax: house lockchange <person>\n\r", ch);
        return;
      }
      for (i = 0; i < 50; i++) {
        if (!str_cmp(arg2, house->tenants[i])) {
          free_string(house->tenants[i]);
          house->tenants[i] = str_dup("");
          house->tenant_trust[i] = 0;
        }
      }
      printf_to_char(ch, "You change the locks to reject the keys of %s.\n\r", arg2);
      return;
    }
    else if (!str_cmp(arg1, "housekey")) {
      house = in_house(ch);
      if (house == nullhouse || house == NULL) {
        prop = in_prop(ch);
        if (prop == NULL) {
          send_to_char("You must be in a house to do that.\n\r", ch);
          return;
        }
        else {
          if (!has_paccess(ch, ch->in_room)) {
            send_to_char("You don't have a key for this property.\n\r", ch);
            return;
          }
          if (arg2[0] == '\0') {
            send_to_char("Syntax: house housekey <person>\n\r", ch);
            return;
          }
          if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
            send_to_char("They're not here.\n\r", ch);
            return;
          }

          for (i = 0; i < 50 && safe_strlen(prop->tenants[i]) > 1; i++) {
          }
          free_string(prop->tenants[i]);
          prop->tenants[i] = str_dup(victim->name);
          prop->tenant_trust[i] = 0;
          printf_to_char(ch, "You give %s access to your house.\n\r", arg2);

          return;
        }
      }
      if (str_cmp(house->owner, NAME(ch))) {
        send_to_char("You don't own any houses.\n\r", ch);
        return;
      }
      if (arg2[0] == '\0') {
        send_to_char("Syntax: house housekey <person>\n\r", ch);
        return;
      }
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      for (i = 0; i < 50 && safe_strlen(house->tenants[i]) > 1; i++) {
      }
      free_string(house->tenants[i]);
      house->tenants[i] = str_dup(victim->name);
      house->tenant_trust[i] = 0;
      printf_to_char(ch, "You give %s access to your house.\n\r", arg2);
      return;
    }
  }

  int get_available_shop_object_vnum(AREA_DATA *parea) {
    int vnum = parea->min_vnum;

    for (; vnum < parea->max_vnum; vnum++) {
      if (get_obj_index(vnum) == NULL)
      return vnum;
      bool found = FALSE;
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end() && found == FALSE; ++it) {

        for (int i = 0; i < 200; i++) {
          if ((*it)->objects[i] == vnum)
          found = TRUE;
        }
      }
      if (found == FALSE)
      return vnum;
    }
    return -1;
  }

  bool linked_prop(PROP_TYPE *prop) {
    if (prop->market_room == 0)
    return FALSE;

    if (get_room_index(prop->market_room) == NULL || get_room_index(prop->market_room)->exit[prop->market_dir] == NULL || get_room_index(prop->market_room)->exit[prop->market_dir]->u1.to_room ==
        NULL || get_room_index(prop->market_room)
        ->exit[prop->market_dir]
        ->u1.to_room->area->vnum != 31)
    return FALSE;
    return TRUE;
  }

  _DOFUN(do_shop) {
    char arg1[MSL];
    char arg2[MSL];
    char month[MSL];
    char buf[MSL] = "";
    char *frame;
    char page[MSL] = "";
    char spacer[] = "";
    int i, vnum, spacing = 0, longest_month_string = 0, month_string_length = 0;
    PROP_TYPE *prop;
    CHAR_DATA *victim;
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (ch->race == RACE_FANTASY)
    return;

    RESET_DATA *pReset;
    RESET_DATA *pReseti;

    ROOM_INDEX_DATA *pRoom = ch->in_room;

    //    OBJ_DATA *obj;

    if (!str_cmp(arg1, "list")) {
      // finding the longest month string for formatting the columns nicer
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
          continue;
        }

        strcpy(month, month_string((*it)->auction_month));
        month_string_length = safe_strlen(month);
        if (month_string_length > longest_month_string) {
          longest_month_string = month_string_length;
        }
      }
      sprintf(page, "`WShop List`x\n\r");
      strcpy(buf, "`g_");
      frame = draw_horizontal_line(ch->linewidth - 2);
      strcat(buf, frame);
      strcat(buf, "_`x\n\r");
      strcat(page, buf);
      if (!str_cmp(arg2, "township") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "town") || !str_cmp(arg2, "haven")) {
        strcat(page, "`WHaven Township`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_TOWN)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (!str_cmp(arg2, "forest") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "inner") || !str_cmp(arg2, "forests") || !str_cmp(arg2, "haven")) {
        strcat(page, "`WHaven Forest`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_INNER)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (!str_cmp(arg2, "outer") || safe_strlen(arg2) < 2 || !str_cmp(arg2, "otherwordly") || !str_cmp(arg2, "deep"))) {
        strcat(page, "`WOtherworldly Forest`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_OUTER)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (!str_cmp(arg2, "wilds") || safe_strlen(arg2) < 2)) {
        strcat(page, "`WWilds`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_WILDS)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (safe_strlen(arg2) < 2 || !str_cmp(arg2, "fae") || !str_cmp(arg2, "other"))) {
        strcat(page, "`WFae Other`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_OTHER)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (safe_strlen(arg2) < 2 || !str_cmp(arg2, "godrealms") || !str_cmp(arg2, "godrealm"))) {
        strcat(page, "`WGodrealm`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_GODREALM)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }
      if (super_aware(ch) && (safe_strlen(arg2) < 2 || !str_cmp(arg2, "hell") || !str_cmp(arg2, "hell"))) {
        strcat(page, "`WHell`x\n\r");
        for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
        it != PropVect.end(); ++it) {
          if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP || (*it)->valid == FALSE) {
            continue;
          }
          if (prop_location((*it)) != PROPLOC_HELL)
          continue;

          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gStatus`x: ");
          strcat(page, buf);
          if ((*it)->sale_price > 0)
          sprintf(buf, "For Sale\n\r");
          else
          sprintf(buf, "Owned\n\r");
          strcat(page, buf);
        }
      }

      page_to_char(page, ch);
      frame = str_dup("");
      free_string(frame);
      return;
    }
    // Lists shops owned by the player - Discordance
    else if (!str_cmp(arg1, "owned")) {
      // formatting columns
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0') {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          month_string_length = safe_strlen(month);
          if (month_string_length > longest_month_string) {
            longest_month_string = month_string_length;
          }
        }
      }
      sprintf(page, "`WShop Owned`x\n\r");
      strcpy(buf, "`g_");
      frame = draw_horizontal_line(ch->linewidth - 2);
      strcat(buf, frame);
      strcat(buf, "_`x\n\r");
      strcat(page, buf);
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!str_cmp((*it)->owner, ch->name)) {
          if (!(*it)->propname || (*it)->propname[0] == '\0') {
            continue;
          }
          strcpy(month, month_string((*it)->auction_month));
          spacing = 60 - longest_month_string - safe_strlen_color((*it)->propname);
          sprintf(buf, "`g[`W%3d`g]`x %-s`x", (*it)->vnum, (*it)->propname);
          strcat(page, buf);
          sprintf(buf, "%*s", spacing, spacer);
          strcat(page, buf);
          sprintf(buf, "`gPayment`x: ");
          strcat(page, buf);
          // adjusts short months to line up properly with long months
          if (month_string_length < longest_month_string) {
            spacing = longest_month_string - month_string_length;
            sprintf(buf, "%*s", spacing, spacer);
            month_string_length = safe_strlen(month);
            strcat(page, buf);
          }
          sprintf(buf, "%s %2d%2s\n\r", month, (*it)->auction_day, datesuffix((*it)->auction_day));
          strcat(page, buf);
        }
      }
      page_to_char(page, ch);
      frame = str_dup("");
      free_string(frame);
      return;
    }
    else if (!str_cmp(arg1, "addbasement")) {
      if (IS_IMMORTAL(ch)) {
        if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
          send_to_char("No such shop.\n\r", ch);
          return;
        }
        if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
          send_to_char("No such shop.\n\r", ch);
          return;
        }
        else {
          if (prop->minz > -3) {
            send_to_char("No basement space.\n\r", ch);
            return;
          }
          build_basement(prop);
          send_to_char("Basement built.\n\r", ch);
          return;
        }
      }
    }
    // This just looks at the property's rent due month and sets it appropriately
    // - Discordance
    else if (!str_cmp(arg1, "fixmonth")) {
      if (IS_IMMORTAL(ch)) {
        if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
          send_to_char("No such shop.\n\r", ch);
          return;
        }
        if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
          send_to_char("No such shop.\n\r", ch);
          return;
        }
        else {
          if (get_day() >= prop->auction_day) {
            if (get_month() < 11) {
              prop->auction_month = get_month() + 1;
            }
            else {
              prop->auction_month = 0;
            }
          }
          else {
            prop->auction_month = get_month();
          }
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "info")) {
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else {
        strcpy(month, month_string(prop->auction_month));
        printf_to_char(ch, "%s\n\r", prop->propname);
        printf_to_char(ch, "Address: %s\n\r", prop->address);
        if (get_room_index(prop->firstroom) != NULL && district_room(get_room_index(prop->firstroom)) != DISTRICT_ANYWHERE) {
          int dist = district_room(get_room_index(prop->firstroom));
          if (dist == DISTRICT_URBAN)
          send_to_char("District: Urban\n\r", ch);
          if (dist == DISTRICT_TOURIST)
          send_to_char("District: Tourist\n\r", ch);
          if (dist == DISTRICT_REDLIGHT)
          send_to_char("District: Red Light\n\r", ch);
          if (dist == DISTRICT_HISTORIC)
          send_to_char("District: Historic\n\r", ch);
        }
        printf_to_char(ch, "Industry: %s\n\r", capitalize(industry_table[prop->industry].name));
        printf_to_char(ch, "Advertisement: %s\n\r", prop->advertisement);
        if (prop->sale_price > 0)
        printf_to_char(ch, "For sale for: $%d\n\r", prop->sale_price);
        printf_to_char(ch, "Value: $%d\n\r", prop->price);
        printf_to_char(ch, "Payments: %s %d%s\n\r", month, prop->auction_day, datesuffix(prop->auction_day));
        if ((arcane_focus(ch) > 0 || IS_IMMORTAL(ch)) && linked_prop(prop))
        send_to_char("`cYou sense a mystical link.`x\n\r", ch);
        if (IS_IMMORTAL(ch)) {
          printf_to_char(ch, "Owner: %s\n\r", prop->owner);
          printf_to_char(ch, "Tvalue: %d\n\r", property_truevalue(prop));
          printf_to_char(ch, "Max: %d, %d, %d\n\r", prop->maxx, prop->maxy, prop->maxz);
          printf_to_char(ch, "Min: %d, %d, %d\n\r", prop->minx, prop->miny, prop->minz);
          view_prop_roomlist(prop, ch);
        }

        return;
      }
    }
    else if (!str_cmp(arg1, "info")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else {
        free_string(prop->propname);
        prop->propname = str_dup("delete");
        prop->valid = FALSE;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "droproom")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else {
        drop_prop_room(prop, atoi(argument));
      }
    }
    else if (!str_cmp(arg1, "resize")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        char arg3[MSL];
        argument = one_argument_nouncap(argument, arg3);
        char arg4[MSL];
        argument = one_argument_nouncap(argument, arg4);
        char arg5[MSL];
        argument = one_argument_nouncap(argument, arg5);
        char arg6[MSL];
        argument = one_argument_nouncap(argument, arg6);

        prop->minx = atoi(arg3);
        prop->maxx = atoi(arg4);
        prop->miny = atoi(arg5);
        prop->maxy = atoi(arg6);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "burndown")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else {
        free_string(prop->propname);
        prop->propname = str_dup("delete");
        prop->valid = FALSE;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "resize")) {
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        char arg3[MSL];
        argument = one_argument_nouncap(argument, arg3);
        char arg4[MSL];
        argument = one_argument_nouncap(argument, arg4);
        char arg5[MSL];
        argument = one_argument_nouncap(argument, arg5);
        char arg6[MSL];
        argument = one_argument_nouncap(argument, arg6);

        prop->minx = atoi(arg3);
        prop->maxx = atoi(arg4);
        prop->miny = atoi(arg5);
        prop->maxy = atoi(arg6);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "airbubble")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        airbubble_property(prop);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "newaddress")) {
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name) && !IS_IMMORTAL(ch)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }

      free_string(prop->address);
      prop->address = str_dup(
      create_address(get_room_index(prop->roadroom), prop->orientation));
      free_string(prop->propname);
      prop->propname = str_dup(
      create_address(get_room_index(prop->roadroom), prop->orientation));
      printf_to_char(ch, "The	propery	is now located at %s.\n\r", prop->address);
      return;
    }
    else if (!str_cmp(arg1, "reprice")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        prop->price = atoi(argument);
        return;
      }
    }
    else if (!str_cmp(arg1, "newvnum")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else {
        prop->vnum = atoi(argument);
        return;
      }
    }
    else if (!str_cmp(arg1, "newowner")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        free_string(prop->owner);
        prop->owner = str_dup(argument);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "nochange")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        if(prop->nochange == 0)
        {
          prop->nochange = 1;
          send_to_char("Nochange set.\n\r", ch);
        }
        else
        {
          prop->nochange = 0;
          send_to_char("Nochange removed.\n\r", ch);
        }
        return;
      }

    }
    else if (!str_cmp(arg1, "renovate")) {
      if (!IS_IMMORTAL(ch))
      return;
      if (atoi(arg2) > 10000 || atoi(arg2) < 1) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }

      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else {
        prop->renovate = 1;
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "roster")) {
      if (is_helpless(ch))
      return;
      prop = in_prop(ch);
      if (prop == NULL || str_cmp(ch->name, prop->owner)) {
        send_to_char("You don't own this shop.\n\r", ch);
        return;
      }
      else {
        printf_to_char(ch, "Owner: %s\n\r", prop->owner);

        for (i = 0; i < 50; i++) {
          if (safe_strlen(prop->tenants[i]) > 1) {
            printf_to_char(ch, "Staff: %s\n\r", prop->tenants[i]);
          }
        }
        return;
      }
    }
    else if (!str_cmp(arg1, "sell")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      else if (is_base(prop)) {
        send_to_char("You can't sell a faction base.\n\r", ch);
        return;
      }
      else {
        if (ch->played / 3600 < 50) {
          send_to_char("You need to be more established before entering the real estate market.\n\r", ch);
          return;
        }
        int sprice = atoi(argument);
        if (sprice != 0)
        sprice = UMAX(sprice, prop->price / 10);

        prop->sale_price = sprice;
        printf_to_char(ch, "Property sale price set at %d.\n\r", sprice);
      }
    }
    else if (!str_cmp(arg1, "workers")) {
      if (is_helpless(ch))
      return;

      prop = in_prop(ch);
      if (prop == NULL || str_cmp(ch->name, prop->owner)) {
        send_to_char("You don't own this shop.\n\r", ch);
        return;
      }
      else {
        printf_to_char(ch, "Owner: %s\n\r", prop->owner);

        for (i = 0; i < 20; i++) {
          if (safe_strlen(prop->workers[i]) > 1) {
            printf_to_char(ch, "Employee: %s working as %s\n\r", prop->workers[i], prop->worker_position[i]);
          }
        }
        return;
      }
    }
    else if (!str_cmp(arg1, "fire")) {
      if (is_helpless(ch))
      return;

      if (safe_strlen(arg2) < 2) {
        send_to_char("Fire whom?\n\r", ch);
        return;
      }
      prop = in_prop(ch);
      if (prop == NULL || str_cmp(ch->name, prop->owner)) {
        send_to_char("You don't own this shop.\n\r", ch);
        return;
      }
      else {
        for (i = 0; i < 20; i++) {
          if (!str_cmp(arg2, prop->workers[i])) {
            if (get_char_world_pc(prop->workers[i]) != NULL) {

              if (get_room_index(get_char_world_pc(prop->workers[i])
                    ->pcdata->job_room_one) != NULL && prop_from_room(
                    get_room_index(get_char_world_pc(prop->workers[i])
                      ->pcdata->job_room_one)) == prop) {
                free_string(
                get_char_world_pc(prop->workers[i])->pcdata->job_title_one);
                get_char_world_pc(prop->workers[i])->pcdata->job_title_one =
                str_dup("");
                get_char_world_pc(prop->workers[i])->pcdata->job_type_one =
                JOB_UNEMPLOYED;
              }
              if (get_room_index(get_char_world_pc(prop->workers[i])
                    ->pcdata->job_room_two) != NULL && prop_from_room(
                    get_room_index(get_char_world_pc(prop->workers[i])
                      ->pcdata->job_room_two)) == prop) {
                free_string(
                get_char_world_pc(prop->workers[i])->pcdata->job_title_two);
                get_char_world_pc(prop->workers[i])->pcdata->job_title_two =
                str_dup("");
                get_char_world_pc(prop->workers[i])->pcdata->job_type_two =
                JOB_UNEMPLOYED;
              }
            }

            send_to_char("Done.\n\r", ch);

            if (!offline_flag(prop->workers[i], PLR_NOPAY))
            offline_setflag(prop->workers[i], PLR_NOPAY);
            offline_message(prop->workers[i], "You get fired.");
            free_string(prop->workers[i]);
            prop->workers[i] = str_dup("");
          }
        }
      }
    }
    else if (!str_cmp(arg1, "industry")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      for (int i = 0; i < MAX_INDUSTRY; i++) {
        if (!str_cmp(argument, industry_table[i].name)) {
          if (!valid_industry(industry_table[i].vnum, district_room(get_room_index(prop->firstroom)))) {
            send_to_char("That would be a violation of Haven zoning laws, see help district.\n\r", ch);
            return;
          }
          if (prop->industry != 0 && prop->renovate == 0)
          ch->pcdata->total_money -= 15000;
          prop->industry = industry_table[i].vnum;
          printf_to_char(ch, "You make your business %s.\n\r", capitalize(industry_table[i].name));
          return;
        }
      }
      send_to_char("No such industry, options are", ch);
      for (int i = 0; i < MAX_INDUSTRY; i++)
      printf_to_char(ch, ", %s", industry_table[i].name);
      send_to_char(".\n\r", ch);
    }
    else if (!str_cmp(arg1, "abandon")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      else if (is_base(prop)) {
        send_to_char("You can't abandon a faction base.\n\r", ch);
        return;
      }
      else {
        abandon_property(prop);
        send_to_char("You abandon the property.\n\r", ch);
      }
    }
    else if (!str_cmp(arg1, "automaintain")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      if (prop->type_special != PROPERTY_INNERFOREST && prop->type_special != PROPERTY_OUTERFOREST) {
        send_to_char("This property doesn't need regular maintenence.\n\r", ch);
        return;
      }
      if (prop->autoclear == 0) {
        prop->autoclear = 1;
        send_to_char("You hire someone to keep the property in good repair.\n\r", ch);
        return;
      }
      prop->autoclear = 0;
      send_to_char("You stop paying for maintenence.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "advertisement")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      string_append(ch, &prop->advertisement);
      return;
    }
    else if (!str_cmp(arg1, "utilities")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such shop.\n\r", ch);
        return;
      }
      else if (str_cmp(prop->owner, ch->name)) {
        send_to_char("You don't own that shop.\n\r", ch);
        return;
      }
      if (prop->type_special != PROPERTY_OUTERFOREST) {
        send_to_char("This property doesn't need special provisions for utilities.\n\r", ch);
        return;
      }
      if (prop->utilities == 0) {
        prop->utilities = 1;
        send_to_char("You set up utilities access for your property.\n\r", ch);
        ch->pcdata->total_money -= 10000;
        return;
      }
      prop->utilities = 0;
      send_to_char("You stop paying for utilities.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "buy")) {
      if ((prop = prop_lookup_shop(atoi(arg2))) == NULL) {
        send_to_char("No such house.\n\r", ch);
        return;
      }
      else if ((is_gm(ch) && !IS_IMMORTAL(ch)) || is_guest(ch)) {
        send_to_char("Guests and Story runners can't buy properties.\n\r", ch);
        return;
      }
      else if (!str_cmp(prop->owner, NAME(ch))) {
        send_to_char("You already own that.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
      {
        send_to_char("They don't seem willing to sell.\n\r", ch);
        return;
      }

      if (prop->sale_price < 1 && daysidle(prop->owner) < 90) {
        send_to_char("They don't seem willing to sell.\n\r", ch);
        return;
      }
      if (ch->pcdata->total_money <
          UMAX(prop->sale_price, prop->price / 10) * 100) {
        printf_to_char(ch, "You'd need %d dollars to take that over.\n\r", UMAX(prop->sale_price, prop->price / 10));
        return;
      }
      if (higher_power(ch) && owned_shop_count(ch) > 0) {
        send_to_char("You can only own one shop.\n\r", ch);
        return;
      }
      if(prop->type_special != PROPERTY_INNERFOREST && prop->type_special != PROPERTY_OUTERFOREST)
      {
        if(owned_town_shop_count(ch) > get_skill(ch, SKILL_BUSINESSFOCUS))
        {
          printf_to_char(ch, "You can only own %d commercial properties in town.\n\r", get_skill(ch, SKILL_BUSINESSFOCUS)+1);
          return;
        }
      }
      house_charge(prop->owner, UMAX(prop->price / 10, prop->sale_price) * -1);
      free_string(prop->owner);
      prop->owner = str_dup(ch->name);
      send_to_char("You take over the property.\n\r", ch);
      prop->bankrupt = 0;
      ch->pcdata->total_money -= UMAX(prop->price / 10, prop->sale_price) * 100;
      prop->sale_price = 0;
      return;
    }
    else if (!str_cmp(arg1, "trust")) {
      prop = in_prop(ch);
      if (prop == NULL) {
        send_to_char("You aren't in a shop.\n\r", ch);
        return;
      }
      else {
        if (str_cmp(prop->owner, ch->name)) {
          send_to_char("You don't own this shop.\n\r", ch);
          return;
        }
        if (arg2[0] == '\0') {
          send_to_char("Syntax: shop trust <person>\n\r", ch);
          return;
        }
        for (i = 0; i < 50; i++) {
          if (!str_cmp(arg2, prop->tenants[i])) {
            if (prop->tenant_trust[i] == 0) {
              prop->tenant_trust[i] = 1;
              printf_to_char(ch, "You trust %s.\n\r", prop->tenants[i]);
            }
            else {
              prop->tenant_trust[i] = 0;
              printf_to_char(ch, "You stop trusting %s.\n\r", prop->tenants[i]);
            }
          }
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "lockchange")) {

      prop = in_prop(ch);
      if (prop == NULL) {
        send_to_char("You must be in a house to do that.\n\r", ch);
        return;
      }
      else {
        if (str_cmp(prop->owner, NAME(ch))) {
          send_to_char("You don't own any houses.\n\r", ch);
          return;
        }
        if (arg2[0] == '\0') {
          send_to_char("Syntax: house lockchange <person>\n\r", ch);
          return;
        }
        for (i = 0; i < 50; i++) {
          if (!str_cmp(arg2, prop->tenants[i])) {
            free_string(prop->tenants[i]);
            prop->tenants[i] = str_dup("");
            prop->tenant_trust[i] = 0;
          }
        }
        printf_to_char(ch, "You change the locks to reject the keys of %s.\n\r", arg2);

        return;
      }
    }
    else if (!str_cmp(arg1, "shopkey")) {
      prop = in_prop(ch);
      if (prop == NULL) {
        send_to_char("You must be in a house to do that.\n\r", ch);
        return;
      }
      else {
        if (!has_paccess(ch, ch->in_room)) {
          send_to_char("You don't have a key for this property.\n\r", ch);
          return;
        }
        if (arg2[0] == '\0') {
          send_to_char("Syntax: house housekey <person>\n\r", ch);
          return;
        }
        if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }

        for (i = 0; i < 50 && safe_strlen(prop->tenants[i]) > 1; i++) {
        }
        free_string(prop->tenants[i]);
        prop->tenants[i] = str_dup(victim->name);
        prop->tenant_trust[i] = 0;
        printf_to_char(ch, "You give %s access to your shop.\n\r", arg2);

        return;
      }
    }
    else if (!str_cmp(arg1, "name")) {
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
        return;
      }
      if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
        send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
        return;
      }
      PROP_TYPE *iprop = prop_from_room(ch->in_room);
      if(iprop != NULL && iprop->nochange == 1)
      {
        send_to_char("This property can't be altered.\n\r", ch);
        return;
      }

      char buf[MSL];
      sprintf(buf, "%s %s", arg2, argument);
      if(strlen(from_color(buf)) > 48)
      {
        send_to_char("Shop names cannot be longer than 48 characters.\n\r", ch);
        return;
      }
      PROP_TYPE *prop = in_prop(ch);
      free_string(prop->propname);
      prop->propname = str_dup(buf);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "shopkeeper")) {
      PROP_TYPE *iprop = prop_from_room(ch->in_room);
      if(iprop != NULL && iprop->nochange == 1)
      {
        send_to_char("This property can't be altered.\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "add")) {
        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }

        if (safe_strlen(argument) < 3) {
          send_to_char("Syntax: shop shopkeeper add (name)\n\r", ch);
          return;
        }

        vnum = ch->in_room->vnum;
        if (get_mob_index(vnum) == NULL) {
          make_mob(vnum);
          MOB_INDEX_DATA *pMob;
          pMob = get_mob_index(vnum);
          pMob->race = RACE_NORMAL;
          pMob->ttl = -1;
          pMob->sex = 3;
          SET_FLAG(pMob->act, ACT_SENTINEL);
          pMob->pShop = new_shop();
          shop_list.push_back(pMob->pShop);
          pMob->pShop->keeper = pMob->vnum;
        }

        free_string(get_mob_index(vnum)->player_name);
        get_mob_index(vnum)->player_name = str_dup(argument);
        free_string(get_mob_index(vnum)->short_descr);
        get_mob_index(vnum)->short_descr = str_dup(argument);
        free_string(get_mob_index(vnum)->long_descr);
        get_mob_index(vnum)->long_descr = str_dup(argument);
        pReset = new_reset_data();
        pReset->command = 'M';
        pReset->arg1 = vnum;
        pReset->arg2 = 1; /* Max # */
        pReset->arg3 = ch->in_room->vnum;
        pReset->arg4 = 1; /* Min # */
        add_reset(ch->in_room, pReset, 1);
        SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
        send_to_char("Shopkeeper added.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "remove")) {
        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }

        bool found = TRUE;

        RESET_DATA *previ = NULL;

        for (pReseti = ch->in_room->reset_first; pReseti;
        pReseti = pReseti->next) {
          if (pReseti->command == 'G' || pReseti->command == 'E')
          previ = pReseti;
        }

        if (previ == NULL) {
          found = FALSE;
        }
        if (found == TRUE) {
          send_to_char("You need to remove all the goods first.\n\r", ch);
          return;
        }

        RESET_DATA *prev = NULL;

        for (pReset = ch->in_room->reset_first; pReset; pReset = pReset->next) {
          if (pReset->command == 'M')
          break;
          prev = pReset;
        }

        if (!pReset) {
          send_to_char("Shopkeeper not found.\n\r", ch);
          return;
        }

        if (prev)
        prev->next = prev->next->next;
        else
        ch->in_room->reset_first = pRoom->reset_first->next;

        for (pRoom->reset_last = pRoom->reset_first;
        pRoom->reset_last && pRoom->reset_last->next;
        pRoom->reset_last = pRoom->reset_last->next)
        ;

        free_reset_data(pReset);
        send_to_char("Shopkeeper removed.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "good")) {
      PROP_TYPE *iprop = prop_from_room(ch->in_room);
      if(iprop != NULL && iprop->nochange == 1)
      {
        send_to_char("This property can't be altered.\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "list")) {

        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }

        for (i = 0; i < 200; i++) {
          if (get_obj_index(in_prop(ch)->objects[i]) != NULL)
          printf_to_char(ch, "[%d] %s\n\r", i, get_obj_index(in_prop(ch)->objects[i])->description);
          else if (i == 0)
          printf_to_char(ch, "[0] Nothing\n\r");
        }
        return;
      }
      if (!str_cmp(arg2, "create")) {

        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch)->objects[60] != 0) {
          send_to_char("Your shop already has too many objects.\n\r", ch);
          return;
        }
        int vnum = get_available_shop_object_vnum(ch->in_room->area);
        if (vnum == -1) {
          send_to_char("Something went wrong, please contact the administration\n\r", ch);
          return;
        }

        send_to_char("Done.\n\r", ch);
        if (get_obj_index(vnum) == NULL) {
          char temp[MSL];
          make_object(vnum);
          OBJ_INDEX_DATA *pObj;
          pObj = get_obj_index(vnum);

          if (pObj == NULL) {
            send_to_char("Something weird happened.\n\r", ch);
            return;
          }

          sprintf(buf, "<held>");
          sprintf(temp, "%-21.21s", buf);

          free_string(pObj->wear_string);
          pObj->wear_string = str_dup(temp);
          TOGGLE_BIT(pObj->wear_flags, ITEM_TAKE);
        }
        ch->pcdata->ci_editing = 1;
        ch->pcdata->ci_absorb = 1;
        ch->pcdata->ci_vnum = vnum;
        bust_a_prompt(ch);
        for (int i = 0; i < 200; i++) {
          if (in_prop(ch)->objects[i] == 0) {
            in_prop(ch)->objects[i] = vnum;
            return;
          }
        }
        return;
      }
      if (!str_cmp(arg2, "delete")) {
        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }

        if (!is_number(argument) || atoi(argument) > 199) {
          printf_to_char(ch, "Syntax: Good delete (0-50)\n\r");
          return;
        }
        in_prop(ch)->objects[atoi(argument)] = 0;

        for (int i = 0; i < 199; i++) {
          if (in_prop(ch)->objects[i] == 0 && in_prop(ch)->objects[i + 1] != 0) {
            in_prop(ch)->objects[i] = in_prop(ch)->objects[i + 1];
            in_prop(ch)->objects[i + 1] = 0;
          }
        }

        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "add")) {
        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }

        if (!is_number(argument) || atoi(argument) > 199) {
          printf_to_char(ch, "Syntax: Good add (0-%d)\n\r", 50);
          return;
        }

        vnum = in_prop(ch)->objects[atoi(argument)];
        if (vnum == 0) {
          send_to_char("You haven't created that object yet.\n\r", ch);
          return;
        }
        pReset = new_reset_data();
        pReset->arg1 = vnum;

        if (get_obj_index(vnum) == NULL) {
          send_to_char("No such object\n\r", ch);
          return;
        }
        if (safe_strlen(get_obj_index(vnum)->short_descr) < 2 || get_obj_index(vnum)->cost < 1) {
          send_to_char("You haven't created that good yet.\n\r", ch);
          return;
        }
        pReset->arg1 = vnum;
        pReset->arg3 = WEAR_NONE;
        pReset->command = 'G';

        add_reset(ch->in_room, pReset, 2);
        SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
        send_to_char("Good added.\n\r", ch);
      }
      if (!str_cmp(arg2, "remove")) {
        if (!can_decorate(ch, ch->in_room)) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }
        if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
          send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
          return;
        }

        if (pRoom->reset_first == NULL) {
          send_to_char("No goods found.\n\r", ch);
          return;
        }
        RESET_DATA *prev = NULL;

        for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
          if (pReset->command == 'G')
          break;
          prev = pReset;
        }

        if (!pReset) {
          send_to_char("Good not found.\n\r", ch);
          return;
        }

        if (prev)
        prev->next = prev->next->next;
        else
        pRoom->reset_first = pRoom->reset_first->next;

        if (pRoom->reset_first != NULL) {
          for (pRoom->reset_last = pRoom->reset_first; pRoom->reset_last->next;
          pRoom->reset_last = pRoom->reset_last->next)
          ;
        }
        free_reset_data(pReset);
        send_to_char("Good removed.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "save")) {
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
        return;
      }
      if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
        send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
        return;
      }
      SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
      //        OBJ_DATA *obj_next;
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end();) {
        CHAR_DATA *victim = *it;
        ++it;

        if (IS_NPC(victim) && !IS_FLAG(victim->act, ACT_NOPURGE) && victim != ch /* safety precaution */)
        extract_char(victim, TRUE);
      }

      reset_room(pRoom, FALSE);
      send_to_char("Done.\n\r", ch);
      return;
    }
  }

  int get_owner_bid(HOUSE_TYPE *house) {
    int i;
    for (i = 0; i < 30; i++) {
      if (!str_cmp(house->bid_names[i], house->owner)) {
        return house->bid_amounts[i];
      }
    }
    return 0;
  }
  void default_auction(HOUSE_TYPE *house) {
    int i, init_bid, point = 0, second_bid;

    if (get_month() + 1 < 12) {
      house->auction_month = get_month() + 1;
    }
    else {
      house->auction_month = 0;
    }

    /* No reference to real month
house->auction_month++;
if(house->auction_month > 11)
house->auction_month = 0;
*/

    free_string(house->owner);
    house->owner = str_dup("");
    for (i = 0; i < 50; i++) {
      free_string(house->tenants[i]);
      house->tenants[i] = str_dup("");
      house->tenant_trust[i] = 0;
    }

    for (i = 0; i < 30; i++) {
      house->bid_amounts[i] =
      UMIN(house->bid_amounts[i], bank_check(house->bid_names[i]));
      if (house->bid_amounts[i] < house->price || !str_cmp(house->bid_names[i], house->owner)) {
        free_string(house->bid_names[i]);
        house->bid_names[i] = str_dup("");
        house->bid_amounts[i] = 0;
      }
    }

    init_bid = house->price - 1;
    for (i = 0; i < 30; i++) {
      if (house->bid_amounts[i] > init_bid && daysidle(house->bid_names[i]) < 15) {
        init_bid = house->bid_amounts[i];
        point = i;
      }
    }
    if (init_bid < house->price)
    return;

    second_bid = house->price;
    for (i = 0; i < 30; i++) {
      if (i == point)
      continue;
      if (house->bid_amounts[i] > second_bid) {
        second_bid = house->bid_amounts[i];
      }
    }
    if (str_cmp(house->owner, house->bid_names[point])) {
      free_string(house->owner);
      house->owner = str_dup(house->bid_names[point]);
      for (i = 0; i < 50; i++) {
        free_string(house->tenants[i]);
        house->tenants[i] = str_dup("");
        house->tenant_trust[i] = 0;
      }
    }
    offline_message(house->owner, "You receive a message stating that you have won your property bid and can move in immediately.");

    house_charge(house->owner, second_bid);
  }

  void real_auction(HOUSE_TYPE *house) {
    int owner_bid = 0, i, init_bid, point = 0, second_bid;

    // This should keep the date grounded - Discordance
    if (get_month() + 1 < 12) {
      house->auction_month = get_month() + 1;
    }
    else {
      house->auction_month = 0;
    }

    /* No reference to actual month
house->auction_month++;
if(house->auction_month > 11)
house->auction_month = 0;
*/
    /*
house->auction_month -= 12;
*/

    for (i = 0; i < 30; i++) {
      house->bid_amounts[i] =
      UMIN(house->bid_amounts[i], bank_check(house->bid_names[i]));
      if (house->bid_amounts[i] < house->price) {
        free_string(house->bid_names[i]);
        house->bid_names[i] = str_dup("");
        house->bid_amounts[i] = 0;
      }
      if (!str_cmp(house->bid_names[i], house->owner)) {
        owner_bid = house->bid_amounts[i];
        house->bid_amounts[i] *= 2;
      }
    }
    if (owner_bid == 0) {
      free_string(house->owner);
      house->owner = str_dup("");
      for (i = 0; i < 50; i++) {
        free_string(house->tenants[i]);
        house->tenants[i] = str_dup("");
        house->tenant_trust[i] = 0;
      }
    }
    init_bid = house->price - 1;
    for (i = 0; i < 30; i++) {
      if (house->bid_amounts[i] > init_bid) {
        init_bid = house->bid_amounts[i];
        point = i;
      }
    }
    if (init_bid < house->price) {
      return;
    }

    if (!str_cmp(house->bid_names[point], house->owner))
    house->bid_amounts[point] = owner_bid;

    second_bid = house->price;
    for (i = 0; i < 30; i++) {
      if (i == point)
      continue;
      if (house->bid_amounts[i] > second_bid) {
        second_bid = house->bid_amounts[i];
      }
    }
    if (str_cmp(house->owner, house->bid_names[point])) {
      free_string(house->last_owner);
      house->last_owner = str_dup(house->owner);

      offline_message(house->owner, "You receive a message stating that you have been evicted, you have three days to clear out your belongings.");

      free_string(house->owner);
      house->owner = str_dup(house->bid_names[point]);

      offline_message(house->owner, "You receive a messaging stating that you have won your property bid, the previous owner has three days to clear out their belongings and then you can move in.");

      for (i = 0; i < 50; i++) {
        free_string(house->tenants[i]);
        house->tenants[i] = str_dup("");
        house->tenant_trust[i] = 0;
      }
    }
    house_charge(house->owner, second_bid);
  }

  void reset_prop_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return;

    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ped = NULL;

    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      int rev = rev_dir[door];
      if (room->exit[door] == NULL)
      continue;

      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */

      if (pToRoom != NULL) {
        if (pToRoom->exit[rev]) {
          free_exit(pToRoom->exit[rev]);
          pToRoom->exit[rev] = NULL;
        }
      }

      free_exit(room->exit[door]);
      room->exit[door] = NULL;
    }

    room->sector_type = SECT_HOUSE;

    if (!IS_SET(room->room_flags, ROOM_INDOORS))
    SET_BIT(room->room_flags, ROOM_INDOORS);

    if (!IS_SET(room->room_flags, ROOM_DARK))
    SET_BIT(room->room_flags, ROOM_DARK);

    if (!IS_SET(room->room_flags, ROOM_LIGHTOFF))
    SET_BIT(room->room_flags, ROOM_LIGHTOFF);

    if (IS_SET(room->room_flags, ROOM_PUBLIC))
    REMOVE_BIT(room->room_flags, ROOM_PUBLIC);

    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for (obj = room->contents; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      extract_obj(obj);
    }

    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *victim = *it;
      ++it;

      if (IS_NPC(victim) && !IS_FLAG(victim->act, ACT_NOPURGE))
      extract_char(victim, TRUE);
    }

    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (ed->next != NULL)
      ped = ed;

      if (ed) {
        if (!ped)
        room->extra_descr = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }
    for (ed = room->places; ed; ed = ed->next) {
      if (ed->next != NULL)
      ped = ed;

      if (ed) {
        if (!ped)
        room->places = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }

    RESET_DATA *pReset;

    for (int i = 30; i > 0; i--) {
      if (!room->reset_first) {
      }
      else if (i == 1) {
        pReset = room->reset_first;
        room->reset_first = room->reset_first->next;
        if (!room->reset_first)
        room->reset_last = NULL;
      }
      else if (i > 1) {
        int iReset = 0;
        RESET_DATA *prev = NULL;

        for (pReset = room->reset_first; pReset; pReset = pReset->next) {
          if (++iReset == i)
          break;
          prev = pReset;
        }

        if (!pReset) {
        }
        else {
          if (prev)
          prev->next = prev->next->next;
          else
          room->reset_first = room->reset_first->next;

          for (room->reset_last = room->reset_first; room->reset_last->next;
          room->reset_last = room->reset_last->next)
          ;
        }
      }
    }
  }

  void recycle_prop(PROP_TYPE *prop) {
    char buf[MSL];
    sprintf(buf, "PROPERTY: %s was recycled from %d to %d..\n\r", prop->propname, prop->lower_vnum, prop->higher_vnum);
    log_string(buf);

    for (int i = prop->lower_vnum; i <= prop->higher_vnum; i++) {
      reset_prop_room(get_room_index(i));
    }

    // free_string(prop->propname); // Disabling this for a bit to find problem
    // with houses getting eaten - Discordance
    prop->propname = str_dup("");
  }

  void recycle_property(int vnum, int type) {

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (vnum != (*it)->vnum)
      continue;
      if (type != (*it)->type)
      continue;

      char buf[MSL];
      sprintf(buf, "PROPERTY: %s was recycled from %d to %d..\n\r", (*it)->propname, (*it)->lower_vnum, (*it)->higher_vnum);
      log_string(buf);

      for (int i = (*it)->lower_vnum; i <= (*it)->higher_vnum; i++) {
        reset_prop_room(get_room_index(i));
      }

      // free_string((*it)->propname); // Disabling this for a bit to find problem
      // with houses getting eaten - Discordance
      (*it)->propname = str_dup("");
    }
  }

  int total_shopcost(char *chname) {
    int count = 0;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if ((*it)->type == PROP_SHOP && !str_cmp(chname, (*it)->owner) && (*it)->renovate == 0)
      count += (*it)->price;
    }
    return count;
  }

  void prop_auction(PROP_TYPE *prop) {
    char buf[MSL];

    // This should keep auction months grounded - Discordance
    if (get_month() + 1 < 12) {
      prop->auction_month = get_month() + 1;
    }
    else {
      prop->auction_month = 0;
    }

    /* No reference to real month
prop->auction_month++;
if(prop->auction_month > 11)
prop->auction_month = 0;
*/

    prop->logoffs = 0;

    if (is_base(prop) && prop->type == PROP_SHOP)
    return;
    int charges = 0;
    int electric = 0;

    electric += prop->electric / 100;
    prop->electric = 0;
    if (prop->type_special == PROPERTY_OUTERFOREST && prop->utilities == 1)
    charges += 100;

    if (prop->renovate == 1) {
      charges = 0;
      electric = 0;
    }
    house_charge(prop->owner, charges);
    if (prop->type != PROP_SHOP)
    house_charge(prop->owner, electric);
    if (prop->type != PROP_SHOP)
    sprintf(buf, "You pay %d in charges and %d in electric costs for %s this month.", charges, electric, prop->propname);
    else
    sprintf(buf, "You pay %d in charges for %s this month.", charges, prop->propname);

    offline_message(prop->owner, buf);

    // What is this for?  Bankrupt never gets set to 10?  Looks redundant or part
    // of some legacy system that no longer exists.
    if (prop->bankrupt == 10) {
      offline_message(prop->owner, "You receive a message stating that the bank has reclaimed your property.\n\r");
      sprintf(log_buf, "%s defaulted on a property payment for %s at %s was reclaimed.  This is the weird bankruptcy check.\n\r", prop->owner, prop->propname, prop->address);
      wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
      log_string(log_buf);
      abandon_property(prop);
      return;
    }
    int price = prop->price;

    if (prop->renovate == 1) {
      price = 0;
      prop->renovate = 0;
    }
    if (prop->type == PROP_SHOP) {
      int total = total_shopcost(prop->owner);
      int skill = 0 + offline_skill(prop->owner, SKILL_BUSINESSFOCUS);
      skill *= 1000;
      total = UMAX(total, 10);
      int fraction = skill * 100 / total;
      fraction = UMIN(fraction, 100);
      price = (100 - fraction) * price / 100;
    }
    ROOM_INDEX_DATA *first = first_room_in_property(prop);
    if(decree_target(first->area->world, DECREE_SPONSOR, prop->owner))
    price = 0;
    if(decree_target(first->area->world, DECREE_OVERTAX, prop->owner))
    price *= 2;


    if (bank_check(prop->owner) < price) {
      if (prop->bankrupt == 1 || daysidle(prop->owner) > 60) {
        offline_message(prop->owner, "You receive a message stating that the bank has reclaimed your property.\n\r");
        sprintf(
        log_buf, "%s defaulted on a property payment and %s at %s was reclaimed.\n\r", prop->owner, prop->propname, prop->address);
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        prop->bankrupt = 0;
        log_string(log_buf);
        abandon_property(prop);
        return;
      }
      else {
        sprintf(buf, "You pay %d for %s this month.", price, prop->propname);
        offline_message(prop->owner, buf);

        house_charge(prop->owner, price);
        offline_message(
        prop->owner, "The bank informs you you were unable to make your mortgage payment this month, you have gone into debt if you miss your next payment the bank will reclaim your property.\n\r");
        sprintf(log_buf, "%s defaulted on a property payment for %s at %s, but gets to keep it for now.\n\r", prop->owner, prop->propname, prop->address);
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
        prop->bankrupt = 1;
        return;
      }
    }
    // This is suspicious because it fires itself.  Need to understand
    // consequences.  Month grounded so not an issue.  Going bankrupt too fast
    // maybe.
    else {
      sprintf(buf, "You pay %d for %s this month.", price, prop->propname);
      offline_message(prop->owner, buf);

      house_charge(prop->owner, price);
      sprintf(log_buf, "%s paid %d for their rent for %s at %s at %d.\n\r", prop->owner, price, prop->propname, prop->address, get_day());
      wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
      log_string(log_buf);

      if (prop->bankrupt == 1) {
        prop->bankrupt = 0;
      }
    }
  }

  int plotz(int vnum) {
    int y = vnum % 1000;
    vnum -= y;
    vnum /= 1000;
    int x = vnum % 1000;
    vnum -= x;
    vnum /= 1000;
    int z = vnum % 10;
    return z;
  }
  int plotx(int vnum) {
    int y = vnum % 1000;
    vnum -= y;
    vnum /= 1000;
    int x = vnum % 1000;
    vnum -= x;
    vnum /= 1000;
    return x;
  }
  int ploty(int vnum) {
    int y = vnum % 1000;
    vnum -= y;
    vnum /= 1000;
    int x = vnum % 1000;
    vnum -= x;
    vnum /= 1000;
    return y;
  }

  void house_auction_update(void) {
    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (get_day() > (*it)->auction_day + 3) {
        free_string((*it)->last_owner);
        (*it)->last_owner = str_dup("");
      }

      if ((*it)->auction_day == get_day()) {
        if (bank_check((*it)->owner) <= 10 || get_owner_bid(*it) <= 10 || daysidle((*it)->owner) > 30)
        default_auction(*it);
        else
        real_auction(*it);
      }
      else if (get_owner_bid(*it) <= 10 || daysidle((*it)->owner) > 30 || (daysidle((*it)->owner) > 5 && offline_hours((*it)->owner) < 100))
      default_auction(*it);
    }

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if ((*it)->warded > 0)
      (*it)->warded--;

      if ((((*it)->auction_day <= get_day()) && (*it)->auction_month == get_month()))
      prop_auction(*it);
      else if ((*it)->auction_day == get_day())
      prop_auction(*it);

      if ((*it)->auction_day > 28)
      (*it)->auction_day = 28;

      if ((*it)->type == PROP_HOUSE && !house_exists((*it)->vnum - 1) && (*it)->vnum > 1)
      (*it)->vnum--;

      if ((*it)->type == PROP_SHOP && !shop_exists((*it)->vnum - 1) && (*it)->vnum > 1)
      (*it)->vnum--;

      if ((*it)->type == PROP_HOUSE && house_count((*it)->vnum) > 1 && (*it)->vnum > 1)
      (*it)->vnum++;

      if ((*it)->type == PROP_SHOP && shop_count((*it)->vnum) > 1 && (*it)->vnum > 1)
      (*it)->vnum++;

      PROP_TYPE *prop = *it;

      if (prop->type == PROP_HOUSE && prop->type_special != PROPERTY_OUTERFOREST && prop->type_special != PROPERTY_INNERFOREST && daysidle(prop->owner) >= 15)
      prop->sale_price = prop->price / 2;

      if (safe_strlen(prop->owner) < 2)
      abandon_property(prop);

      if (prop->sale_price > 0)
      prop->sale_price = prop->sale_price * 19 / 20;
    }
    save_properties(FALSE);
  }

  void house_charge(char *argument, int price) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if (safe_strlen(argument) < 2)
    return;

    price *= 100;

    if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: House charge");

      if (!load_char_obj(&d, argument)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    if(college_group(victim, FALSE) == COLLEGE_PREP)
    {
      price = UMAX(5000, price - 50000);
    }
    if(victim->race == RACE_FACULTY)
    {
      price = UMAX(10000, price - 25000);
    }
    victim->pcdata->total_credit -= price / 2;
    if (victim->pcdata->total_credit < 0) {
      victim->pcdata->total_money += victim->pcdata->total_credit;
      victim->pcdata->total_credit = 0;
    }
    victim->pcdata->total_money -= price / 2;

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  int bank_check(char *argument) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if (safe_strlen(argument) < 2)
    return 0;
    if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Bank check");

      if (!load_char_obj(&d, argument)) {
        return 0;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return 0;
    }
    int amount = victim->pcdata->total_money / 100;
    amount += UMIN(amount, victim->pcdata->total_credit);

    if (IS_AFFECTED(victim, AFF_DISCREDIT))
    amount = 0;

    if (!online)
    free_char(victim);

    return amount;
  }

  char *offline_name(CHAR_DATA *looker, char *name) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    bool found = FALSE;
    CHAR_DATA *ch;
    if (safe_strlen(name) < 2)
    return "Someone";

    d.original = NULL;
    if ((ch = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Offline name");

      if (!load_char_obj(&d, name)) {
        return "Someone";
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      ch = d.character;
    }

    /* Buffer to contain final string */
    static char buf2[MAX_INPUT_LENGTH];
    NameMap::const_iterator nit;
    sprintf(buf2, " ");
    buf2[0] = '\0';

    if (looker == NULL) {
      sprintf(buf2, "%s", ch->name);
      return buf2;
    }

    /* NPC's just show their short descriptions */
    if (IS_NPC(ch)) {
      if (!online)
      free_char(ch);
      return ch->short_descr;
    }

    if (looker == ch) {
      if (!online)
      free_char(ch);
      return "You";
    }

    /* Eveyone knows an immortal, and immortals know everyone */
    if (IS_IMMORTAL(ch) || IS_IMMORTAL(looker) || IS_NPC(looker) || ch == looker || (ch->desc && ch->desc->connected == CON_CREATION)) {
      sprintf(buf2, "%s", NAME(ch));
      // indefinite( ch->pcdata->intro_desc ), // ch->pcdata->intro_desc);
    }

    // Presenting Female
    if (ch->sex == SEX_FEMALE) {
      if ((nit = looker->pcdata->female_names->find(ch->id)) !=
          looker->pcdata->female_names->end()) {
        sprintf(buf2, "%s", nit->second);
        found = TRUE;
      }
    }
    // Presenting Male
    else if (ch->sex == SEX_MALE) {
      if ((nit = looker->pcdata->male_names->find(ch->id)) !=
          looker->pcdata->male_names->end()) {
        sprintf(buf2, "%s", nit->second);
        found = TRUE;
      }
    }

    if (found == FALSE) {
      sprintf(buf2, "%s", capitalize(ch->pcdata->intro_desc));
    }

    if (!online)
    free_char(ch);

    return buf2;
  }

  bool is_privatepp(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    PROP_TYPE *house;

    house = room_prop(room);

    if (house == NULL)
    return FALSE;

    if (house->type == PROP_PUBLIC)
    return FALSE;

    if (house == NULL) {
      return FALSE;
    }

    return TRUE;
  }

  bool is_privatep(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (is_privatepp(room))
    return TRUE;

    HOUSE_TYPE *house;

    house = room_house(room);
    if (house == NULL || house == nullhouse) {
      return FALSE;
    }

    return TRUE;
  }

  bool has_paccess(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (ch == NULL)
    return FALSE;

    if (IS_NPC(ch))
    return FALSE;

    PROP_TYPE *house;
    house = room_prop(room);
    if (house == NULL) {
      return FALSE;
    }
    bool found = FALSE;
    if (house->type == PROP_PUBLIC)
    return TRUE;

    if (safe_strlen(house->owner) < 2 && !is_base(house))
    return TRUE;
    if (!str_cmp(house->owner, "NPC"))
    return TRUE;

    if (str_cmp(ch->name, house->owner)) {
      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), house->tenants[i]))
        return TRUE;
      }
    }

    if (str_cmp(ch->name, house->owner)) {
      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), house->tenants[i]))
        found = TRUE;
      }
      if (!found) {
        return FALSE;
      }
    }
    else
    return TRUE;

    return TRUE;
  }

  bool has_access(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (ch == NULL)
    return FALSE;

    if (IS_NPC(ch))
    return FALSE;

    EXTRA_DESCR_DATA *ed;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("!denial", ed->keyword)) {
        if (is_name(ch->name, ed->description))
        return FALSE;
      }

      if (is_name("!keys", ed->keyword)) {
        if (is_name(ch->name, ed->description))
        return TRUE;
      }
    }

    if (has_paccess(ch, room))
    return TRUE;
    else
    return FALSE;
  }

  bool can_decorate_p(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (higher_power(ch) && is_in_domain(ch))
    return TRUE;

    if(college_student(ch, FALSE) && college_house(ch) == college_house_room(ch->in_room))
    {
      if(!IS_SET(ch->in_room->room_flags, ROOM_BEDROOM))
      return TRUE;
      if(ch->in_room->vnum == room_in_school(ch->in_room->vnum))
      return TRUE;
    }

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      if (room->vnum != (*it)->vnum)
      continue;

      if (!str_cmp((*it)->author, ch->name))
      return TRUE;
    }

    if (room->area->vnum == DIST_MISTS && is_gm(ch))
    return TRUE;

    PROP_TYPE *prop;
    prop = room_prop(room);
    if (prop == NULL || prop == nullprop) {
      return FALSE;
    }

    // Shops
    bool found = FALSE;
    if (!str_cmp(ch->name, prop->owner)) {
      return TRUE;
    }
    else {
      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), prop->tenants[i]) && prop->tenant_trust[i] > 0)
        found = TRUE;
      }
      if (found) {
        return TRUE;
      }
    }

    return FALSE;
  }

  bool can_decorate(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    if (IS_IMMORTAL(ch)) {
      return TRUE;
    }

    if (in_world(ch) != WORLD_EARTH && is_gm(ch) && IS_FLAG(ch->comm, COMM_RUNNING) && is_wildsapproved(ch)) {
      return TRUE;
    }

    if(is_gm(ch) && sandbox_room(ch->in_room))
    return TRUE;
    if(get_gm(ch->in_room, FALSE) == NULL && sandbox_room(ch->in_room))
    return TRUE;

    if (can_decorate_p(ch, room)) {
      return TRUE;
    }

    if (builder_helper(ch) == TRUE) {
      return TRUE;
    }

    // Houses
    HOUSE_TYPE *house;
    house = room_house(room);
    if (house == NULL || house == nullhouse) {
      return FALSE;
    }
    if (!str_cmp(ch->name, house->last_owner))
    return FALSE;

    if (!str_cmp(ch->name, house->owner) && safe_strlen(house->last_owner) > 1) {
      return TRUE;
    }
    bool found = FALSE;
    if (!str_cmp(ch->name, house->owner)) {
      return TRUE;
    }
    else {
      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), house->tenants[i]) && house->tenant_trust[i] > 0)
        found = TRUE;
      }
      if (found) {
        return TRUE;
      }
    }

    return FALSE;
  }

  void autowall(ROOM_INDEX_DATA *newroom) {
    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      int rev = rev_dir[door];
      if (newroom->exit[door] == NULL)
      continue;

      pToRoom = newroom->exit[door]->u1.to_room; /* ROM OLC */

      if (IS_SET(newroom->exit[door]->exit_info, EX_ISDOOR))
      continue;

      if (pToRoom != NULL) {
        if (IS_SET(newroom->room_flags, ROOM_INDOORS) !=
            IS_SET(pToRoom->room_flags, ROOM_INDOORS)) {
          newroom->exit[door]->wall = WALL_BRICK;

          if (pToRoom->exit[rev]) {
            pToRoom->exit[rev]->wall = WALL_BRICK;
          }
        }

        if ((pToRoom->sector_type == SECT_AIR || pToRoom->sector_type == SECT_ATMOSPHERE) && IS_SET(newroom->room_flags, ROOM_INDOORS)) {
          newroom->exit[door]->wall = WALL_BRICK;

          if (pToRoom->exit[rev]) {
            pToRoom->exit[rev]->wall = WALL_BRICK;
          }
        }
      }
    }
  }

  int base_room_price(ROOM_INDEX_DATA *room) {
    int value = 0;

    if (IS_SET(room->room_flags, ROOM_INDOORS))
    value += ROOM_PRICE;

    if(room->z < 0)
    value = 0;

    if (IS_SET(room->room_flags, ROOM_BEDROOM))
    value += FEATURE_PRICE;
    if (IS_SET(room->room_flags, ROOM_KITCHEN))
    value += FEATURE_PRICE;
    if (IS_SET(room->room_flags, ROOM_BATHROOM))
    value += FEATURE_PRICE;

    return value;
  }

  bool renovating(CHAR_DATA *ch)
  {
    if(ch->pcdata->boon == BOON_RENOVATE && ch->pcdata->boon_timeout > current_time)
    return TRUE;
    if(prop_from_room(ch->in_room) != NULL && prop_from_room(ch->in_room)->renovate == 1)
    return TRUE;
    return FALSE;
  }

  _DOFUN(do_decorate) {
    char arg1[MSL];
    char buf[MSL];
    ROOM_INDEX_DATA *room;
    argument = one_argument_nouncap(argument, arg1);

    if (ch->in_room == NULL)
    return;

    if (is_dreaming(ch) && ch->pcdata->dream_room != 0)
    room = get_room_index(ch->pcdata->dream_room);
    else
    room = ch->in_room;

    if (!can_decorate(ch, room) && str_cmp(arg1, "lock")) {
      send_to_char("`cYou need to own this house or have permission to decorate it`g.`x\n\r", ch);
      return;
    }
    if (room == NULL)
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD) && room == ch->in_room) {
      send_to_char("You cannot affect the waking world.\n\r", ch);
      return;
    }

    if (strstr(argument, "~")) {
      send_to_char("`cIllegal character in argument`g.`x\n\r", ch);
      return;
    }

    PROP_TYPE *iprop = prop_from_room(ch->in_room);
    if(iprop != NULL && iprop->nochange == 1)
    {
      send_to_char("This property can't be altered.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "roomtitle")) {
      EXTRA_DESCR_DATA *ed;
      for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
        if (is_name("!roomtitle", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("!roomtitle");
        ed->next = ch->in_room->extra_descr;
        ch->in_room->extra_descr = ed;
        free_string(ch->in_room->extra_descr->description);
        ch->in_room->extra_descr->description = str_dup(argument);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(argument);
      }
      if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED)) {
        SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
      }
      send_to_char("`cDone`g.`x\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "description")) {
      if (can_decorate(ch, room))
      string_append(ch, &room->description);
      // else
      // string_append(ch, &room->player_description);
      SET_BIT(room->area->area_flags, AREA_CHANGED);
      return;
    }
    else if (!str_cmp(arg1, "nightmaredescription")) {
      if (can_decorate(ch, room))
      string_append(ch, &room->shroud);
      // else
      // string_append(ch, &room->player_shroud);

      SET_BIT(room->area->area_flags, AREA_CHANGED);
      return;
    }

    // Added sprintf to combat color bleed - Discordance
    if (!str_cmp(arg1, "name")) {
      /*
EXTRA_DESCR_DATA *ed;
EXTRA_DESCR_DATA *ped = NULL;
for ( ed = room->extra_descr; ed; ed = ed->next )
{
if(ed->next != NULL && is_name("!roomtitle", ed->next->keyword))
ped = ed;
if ( is_name( "!roomtitle", ed->keyword ) )
break;
}

if ( ed )
{
if ( !ped )
room->extra_descr = ed->next;
else
ped->next = ed->next;

free_extra_descr( ed );
}
*/

      SET_BIT(room->area->area_flags, AREA_CHANGED);
      sprintf(buf, "%s`x", argument);
      free_string(room->name);
      room->name = str_dup(buf);
      send_to_char("`cDone`g.`x\n\r", ch);

    }
    else if (!str_cmp(arg1, "destroywardrobe")) {
      for (int f = 0; f < 10; f++) {
        if (ch->in_room->exit[f] != NULL) {
          ROOM_INDEX_DATA *toroom = ch->in_room->exit[f]->u1.to_room;
          if (toroom != NULL) {
            fix_room_dirs(toroom);
            if (!IS_SET(toroom->area->area_flags, AREA_CHANGED))
            SET_BIT(toroom->area->area_flags, AREA_CHANGED);
          }
        }
      }
      fix_room_dirs(ch->in_room);
      if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
      SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "magicwardrobe")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate magicwardrobe (person) `g(`Wdirection`g)`x\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if (!IS_SET(ch->in_room->room_flags, ROOM_BEDROOM)) {
        send_to_char("Links can only be made between bedrooms.\n\r", ch);
        return;
      }
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }
      if (!ch->in_room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (ch->in_room->exit[door]->wall != WALL_BRICK) {
        send_to_char("`cYou need to mark a wall.`x\n\r", ch);
        return;
      }
      if (ch->money < 50000) {
        send_to_char("That would cost $500.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_world_pc(arg2);
      if (victim == NULL || !IS_SET(victim->in_room->room_flags, ROOM_BEDROOM)) {
        send_to_char("They are not around or are not in a bedroom.\n\r", ch);
        return;
      }
      if(!can_world_travel(ch, ch->in_room, victim->in_room))
      {
        send_to_char("You don't know how to get to that world.\n\r", ch);
        return;
      }

      if (victim->pcdata->wardrobe_pointer == ch) {
        ch->money -= 50000;
        victim->money -= 50000;
        ch->in_room->exit[door]->wall = WALL_NONE;
        ch->in_room->exit[door]->fall = 0;
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->wall =
        WALL_NONE;
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->fall = 0;

        ch->in_room->exit[door]->u1.to_room = victim->in_room;
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->u1.to_room =
        ch->in_room;

        int value;
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(ch->in_room->exit[door]->rs_flags, value);
        ch->in_room->exit[door]->exit_info = ch->in_room->exit[door]->rs_flags;
        TOGGLE_BIT(
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->rs_flags, value);
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->exit_info =
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->rs_flags;
        value = flag_value(exit_flags, "closed");
        TOGGLE_BIT(ch->in_room->exit[door]->rs_flags, value);
        TOGGLE_BIT(
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->rs_flags, value);
        ch->in_room->exit[door]->exit_info = ch->in_room->exit[door]->rs_flags;
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->exit_info =
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->rs_flags;

        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(ch->in_room->exit[door]->rs_flags, value);
        TOGGLE_BIT(
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->rs_flags, value);
        ch->in_room->exit[door]->exit_info = ch->in_room->exit[door]->rs_flags;
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->exit_info =
        victim->in_room->exit[victim->pcdata->wardrobe_direction]->rs_flags;

        send_to_char("Magic wardrobe connection made.\n\r", ch);
        send_to_char("Magic wardrobe connection made.\n\r", victim);
        if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
        SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
        if (!IS_SET(victim->in_room->area->area_flags, AREA_CHANGED))
        SET_BIT(victim->in_room->area->area_flags, AREA_CHANGED);

        return;

      }
      else {
        ch->pcdata->wardrobe_direction = door;
        ch->pcdata->wardrobe_pointer = victim;
        send_to_char("You start trying to build a magic connection through your wardrobe.\n\r", ch);
        printf_to_char(victim, "%s is trying to build a magic connection with you through your wardrobes, use decorate magicwardrobe (name) (direction) to build the other side.\n\r", PERS(ch, victim));
        return;
      }

    }
    else if (!str_cmp(arg1, "door")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate door `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      value = flag_value(exit_flags, "door");
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }

      room->exit[door]->wall = WALL_NONE;
      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;

      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
        pToRoom->exit[rev]->wall = WALL_NONE;
        pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
      }

      value = flag_value(exit_flags, "closed");

      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;

      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
        pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
      }

      if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "hiddendoor")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate hiddendoor `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      value = flag_value(exit_flags, "door");
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }

      if (ch->money < 20000) {
        send_to_char("That would cost $200.\n\r", ch);
        return;
      }

      room->exit[door]->wall = WALL_NONE;
      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;

      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
        pToRoom->exit[rev]->wall = WALL_NONE;
        pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
      }

      value = flag_value(exit_flags, "closed");

      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;

      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
        pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
      }

      if (!IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      ch->money -= 20000;
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "marketdoor")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate marketdoor `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }
      if (!ch->in_room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (ch->in_room->exit[door]->wall != WALL_BRICK) {
        send_to_char("`cYou need to mark a wall.`x\n\r", ch);
        return;
      }

      PROP_TYPE *prop = prop_from_room(ch->in_room);
      if (prop == NULL || prop->type != PROP_SHOP) {
        send_to_char("You can only do this in shops you own.\n\r", ch);
        return;
      }
      prop->market_room = ch->in_room->vnum;
      prop->market_dir = door;
      send_to_char("You mark the wall for the market.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "doorname")) {
      int rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      char arg2[MSL];

      argument = one_argument_nouncap(argument, arg2);

      for (int f = 0; f < 10; f++) {
        if (!str_cmp(arg2, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }
      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }
      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "")) {
        send_to_char("You must include a door name.\n\r", ch);
        return;
      }

      free_string(room->exit[door]->keyword);
      room->exit[door]->keyword = str_dup(argument);

      pToRoom = room->exit[door]->u1.to_room;
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        free_string(pToRoom->exit[rev]->keyword);
        pToRoom->exit[rev]->keyword = str_dup(argument);
      }

    }
    else if (!str_cmp(arg1, "atmosphere")) {
      EXTRA_DESCR_DATA *ed;
      for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
        if (is_name("!atmosphere", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("!atmosphere");
        ed->next = ch->in_room->extra_descr;
        ch->in_room->extra_descr = ed;
        string_append(ch, &ch->in_room->extra_descr->description);
      }
      else
      string_append(ch, &ed->description);

      if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
      SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

      return;
    }
    else if (!str_cmp(arg1, "build")) {
      PROP_TYPE *prop = prop_from_room(room);
      if (prop == NULL)
      return;
      if (prop->type_special == PROPERTY_TRAILER) {
        send_to_char("You can't expand your trailer.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "up")) {
        if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
          send_to_char("You can only build up from an indoors room.\n\r", ch);
          return;
        }
        if (room->z >= prop->maxz) {
          send_to_char("You don't have planning permission to build up that high.\n\r", ch);
          return;
        }
        /*
if(room->z >= prop->maxz-1)
{
if(district_room(room) != DISTRICT_ANYWHERE)
{
send_to_char("You don't have planning permission to build up that
high.\n\r", ch); return;
}
}
*/

        if (room->exit[DIR_UP] == NULL || room->exit[DIR_UP]->u1.to_room == NULL) {
          airbubble(room, FALSE);
        }
        if (room->exit[DIR_UP] == NULL || room->exit[DIR_UP]->u1.to_room == NULL) {
          send_to_char("Something went wrong, please contact the administration.\n\r", ch);
          return;
        }
        ROOM_INDEX_DATA *toroom = room->exit[DIR_UP]->u1.to_room;
        if (prop_from_room(toroom) != NULL) {
          if (!IS_SET(toroom->room_flags, ROOM_INDOORS))
          SET_BIT(toroom->room_flags, ROOM_INDOORS);
          if (toroom->sector_type == SECT_AIR)
          toroom->sector_type = SECT_HOUSE;
          send_to_char("That room has already been developed.\n\r", ch);
          return;
        }
        if (prop_from_room(room)->type_special == PROPERTY_OUTERFOREST) {
          int point = -1;
          for (int i = 0; i < 200 && point == -1; i++) {
            if (prop_from_room(room)->roomlist[i] == 0)
            point = i;
          }
          if (point == -1) {
            send_to_char("Your claim is already too large.\n\r", ch);
            return;
          }
          prop_from_room(room)->roomlist[point] = toroom->vnum;
        }
        if (!IS_SET(toroom->room_flags, ROOM_INDOORS))
        SET_BIT(toroom->room_flags, ROOM_INDOORS);
        autowall(toroom);
        setup_proproom(toroom, prop_from_room(room));
        airbubble(toroom, FALSE);
        if(renovating(ch))
        ch->pcdata->total_money -= ROOM_PRICE * 100;
        prop_from_room(room)->price += ROOM_PRICE;
        printf_to_char(ch, "You pay %d to build a new room.\n\r", ROOM_PRICE);
        return;
      }
      else if (!str_cmp(argument, "down")) {
        if (room->z <= prop->minz) {
          bool specialroom = FALSE;
          for (int i = 0; i < 5; i++) {
            if (room->vnum == prop->included[i]) {
              specialroom = TRUE;
            }
          }
          if (specialroom == FALSE) {
            send_to_char("You don't have planning permission to dig down that low.\n\r", ch);
            return;
          }
        }
        if (room->exit[DIR_DOWN] != NULL && room->exit[DIR_DOWN]->u1.to_room != NULL) {
          send_to_char("There is already a room below this one.\n\r", ch);
          return;
        }
        smartdig(room, DIR_DOWN, room->area->vnum);
        ROOM_INDEX_DATA *toroom = room->exit[DIR_DOWN]->u1.to_room;
        if (toroom == NULL) {
          send_to_char("Something went wrong, please contact the administration.\n\r", ch);
          return;
        }
        if (prop_from_room(room)->type_special == PROPERTY_OUTERFOREST) {
          int point = -1;
          for (int i = 0; i < 200 && point == -1; i++) {
            if (prop_from_room(room)->roomlist[i] == 0)
            point = i;
          }
          if (point == -1) {
            send_to_char("Your claim is already too large.\n\r", ch);
            return;
          }
          prop_from_room(room)->roomlist[point] = toroom->vnum;
        }
        if (!IS_SET(toroom->room_flags, ROOM_INDOORS))
        SET_BIT(toroom->room_flags, ROOM_INDOORS);
        autowall(toroom);
        setup_proproom(toroom, prop_from_room(room));
        if(!renovating(ch))
        ch->pcdata->total_money -= ROOM_PRICE * 100;
        prop_from_room(room)->price += ROOM_PRICE;
        printf_to_char(ch, "You pay %d to build a new room.\n\r", ROOM_PRICE);
        return;
      }
      else {
        send_to_char("`cSyntax`g: `WDecorate build `g(`Wup`g/`Wdown`g)`x\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "wall")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate wall `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }

        value = flag_value(exit_flags, "closed");

        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      room->exit[door]->wall = WALL_BRICK;
      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->wall = WALL_BRICK;
      }

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "lock")) {
      int value;
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WLock `g(`Wdirection`g)`x\n\r", ch);
        return;
      }
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }
      if (!IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        send_to_char("`cThere is no door there`g.`x\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room) && !can_decorate(ch, room->exit[door]->u1.to_room)) {
        send_to_char("`cYou need to own this house or have permission to decorate it`g.`x\n\r", ch);
        return;
      }

      value = flag_value(exit_flags, "locked");
      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "demolish")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate demolish `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }

      if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }

        value = flag_value(exit_flags, "closed");

        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_LOCKED)) {
        value = flag_value(exit_flags, "locked");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      room->exit[door]->wall = WALL_NONE;
      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->wall = WALL_NONE;
      }
      if (IS_SET(room->room_flags, ROOM_INDOORS) && !renovating(ch)) {
        ch->pcdata->total_money -= 2500;
        printf_to_char(ch, "You pay $25 to have that wall knocked down.\n\r");
      }
      else
      send_to_char("Done.\n\r", ch);

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "largewindow")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate largewindow `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }
      if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }

        value = flag_value(exit_flags, "closed");

        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_LOCKED)) {
        value = flag_value(exit_flags, "locked");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      room->exit[door]->wall = WALL_GLASS;
      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];
      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->wall = WALL_GLASS;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "smallwindow")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate smallwindow `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }

      if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;
        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];
        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }

        value = flag_value(exit_flags, "closed");

        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_LOCKED)) {
        value = flag_value(exit_flags, "locked");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (!IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      room->exit[door]->wall = WALL_GLASS;
      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->wall = WALL_GLASS;
      }

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "indoors")) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("`cThat`g'`cs already an indoors room`g.\n\r", ch);
        return;
      }

      if (room->z >= 0 && !can_develop_proproom(room)) {
        send_to_char("That room can't be developed.\n\r", ch);
        return;
      }

      if (get_roomz(room) >= 0) {
        if(!renovating(ch)) {
          ch->pcdata->total_money -= ROOM_PRICE * 100;
          printf_to_char(ch, "You pay %d to build a new room.\n\r", ROOM_PRICE);
        }
        prop_from_room(room)->price += ROOM_PRICE;
      }

      SET_BIT(room->room_flags, ROOM_INDOORS);
      autowall(room);
      setup_proproom(room, prop_from_room(room));
      send_to_char("`cDone`g.\n\r", ch);
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "signalproof")) {
      if (!IS_SET(room->room_flags, ROOM_SILENCED)) {
        SET_BIT(room->room_flags, ROOM_SILENCED);
        send_to_char("You make the room signalproof.\n\r", ch);
        if(!renovating(ch))
        ch->pcdata->total_money -= 50000;
        return;
      }
      else {
        REMOVE_BIT(room->room_flags, ROOM_SILENCED);
        send_to_char("You remove the room's signalproofing.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "decor")) {
      int level = atoi(argument);
      if (level < 0 || level > 3) {
        send_to_char("Decorate decor 0-3\n\r", ch);
        return;
      }
      int diff = level - room->decor;
      if (diff > 0) {
        if(!renovating(ch)) {
          ch->pcdata->total_money -=
          UMAX(ROOM_PRICE, base_room_price(room)) * DECOR_ONE * diff;
          printf_to_char(ch, "You pay $%d to improve the decor.\n\r", UMAX(ROOM_PRICE, base_room_price(room)) * DECOR_ONE *
          diff / 100);
        }
      }
      room->decor = level;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "toughness")) {
      int level = atoi(argument);
      if (level < 0 || level > 4) {
        send_to_char("Decorate toughness 0-4\n\r", ch);
        return;
      }
      int diff = level - room->toughness;
      if (diff > 0) {
        ch->pcdata->total_money -=
        UMAX(ROOM_PRICE, base_room_price(room)) * TOUGHNESS_PRICE * diff;
        printf_to_char(ch, "You pay $%d to improve the toughness.\n\r", UMAX(ROOM_PRICE, base_room_price(room)) * TOUGHNESS_PRICE *
        diff / 100);
      }
      room->toughness = level;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "security")) {
      int level = atoi(argument);
      if (level < 0 || level > 4) {
        send_to_char("Decorate security 0-4\n\r", ch);
        return;
      }
      int diff = level - room->security;
      if (diff > 0) {
        ch->pcdata->total_money -= SECURITY_PRICE * skillpoint(diff) * 100;
        printf_to_char(ch, "You pay $%d to improve the security.\n\r", SECURITY_PRICE * skillpoint(diff));
      }
      room->security = level;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "demolish")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("You can't build an outdoors kitchen.\n\r", ch);
        return;
      }
      if (prop_from_room(room) == NULL || prop_from_room(room)->type == PROP_SHOP) {
        send_to_char("You can only build that in a house.\n\r", ch);
        return;
      }
      if (IS_SET(room->room_flags, ROOM_KITCHEN)) {
        REMOVE_BIT(room->room_flags, ROOM_KITCHEN);
        send_to_char("You dismantle your kitchen.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_KITCHEN);
      ch->pcdata->total_money -= FEATURE_PRICE * 100;
      printf_to_char(ch, "You pay $%d to install a kitchen.\n\r", FEATURE_PRICE);
    }
    else if (!str_cmp(arg1, "bedroom")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("You can't build an outdoors bedroom.\n\r", ch);
        return;
      }
      if (prop_from_room(room) == NULL || prop_from_room(room)->type == PROP_SHOP) {
        send_to_char("You can only build that in a house.\n\r", ch);
        return;
      }

      if (IS_SET(room->room_flags, ROOM_BEDROOM)) {
        REMOVE_BIT(room->room_flags, ROOM_BEDROOM);
        send_to_char("You dismantle your bedroom.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_BEDROOM);
      ch->pcdata->total_money -= FEATURE_PRICE * 100;
      printf_to_char(ch, "You pay $%d to build a bedroom.\n\r", FEATURE_PRICE);
    }
    else if (!str_cmp(arg1, "bathroom")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("You can't build an outdoors bathroom.\n\r", ch);
        return;
      }
      if (prop_from_room(room) == NULL || prop_from_room(room)->type == PROP_SHOP) {
        send_to_char("You can only build that in a house.\n\r", ch);
        return;
      }

      if (IS_SET(room->room_flags, ROOM_BATHROOM)) {
        REMOVE_BIT(room->room_flags, ROOM_BATHROOM);
        send_to_char("You dismantle your bathroom.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_BATHROOM);
      printf_to_char(ch, "You pay $%d to install a bathroom.\n\r", FEATURE_PRICE);
      ch->pcdata->total_money -= FEATURE_PRICE * 100;
    }
    else if (!str_cmp(arg1, "type")) {
      if (prop_from_room(room) == NULL)
      return;
      int subtype = prop_from_room(room)->type_special;
      if (subtype == PROPERTY_TOWNHOUSE || subtype == PROPERTY_TOWNSHOP || subtype == PROPERTY_TRAILER || subtype == PROPERTY_UNDERSHOP || subtype == PROPERTY_OVERFLAT || subtype == PROPERTY_LONGHOUSE || subtype == PROPERTY_LONGSHOP || subtype == PROPERTY_SMALLFLAT || subtype == PROPERTY_MEDFLAT || subtype == PROPERTY_BIGFLAT) {
        send_to_char("You'd be violating city zoning laws.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "shop")) {
        prop_from_room(ch->in_room)->type = PROP_SHOP;
        prop_type_switch(prop_from_room(ch->in_room), PROP_SHOP);
        prop_from_room(ch->in_room)->prop_state = 0;
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 30000;
          printf_to_char(
          ch, "You pay $%d to convert this property to a shop.\n\r", 300);
        }
        prop_from_room(ch->in_room)->price =
        property_truevalue(prop_from_room(ch->in_room));

        return;
      }
      if (!str_cmp(argument, "house") && (prop_from_room(ch->in_room)->prop_state != 0 || prop_from_room(ch->in_room)->type != PROP_HOUSE)) {
        prop_from_room(ch->in_room)->type = PROP_HOUSE;
        prop_type_switch(prop_from_room(ch->in_room), PROP_HOUSE);
        prop_from_room(ch->in_room)->prop_state = 0;
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 30000;
          printf_to_char(
          ch, "You pay $%d to convert this property to a house.\n\r", 300);
        }
        prop_from_room(ch->in_room)->price =
        property_truevalue(prop_from_room(ch->in_room));

        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "anonymous") && prop_from_room(ch->in_room)->prop_state != 1) {
        prop_from_room(ch->in_room)->type = PROP_HOUSE;
        prop_type_switch(prop_from_room(ch->in_room), PROP_HOUSE);
        prop_type_switch(prop_from_room(ch->in_room), 10);
        prop_from_room(ch->in_room)->prop_state = 1;
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 20000;
          printf_to_char(
          ch, "You pay $%d to convert this property be anonymously owned.\n\r", 200);
        }
        prop_from_room(ch->in_room)->price =
        property_truevalue(prop_from_room(ch->in_room));

        send_to_char("Done.\n\r", ch);
        return;
      }

    }
    else if (!str_cmp(arg1, "largewindow")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate largewindow `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }

      if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }

        value = flag_value(exit_flags, "closed");

        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_LOCKED)) {
        value = flag_value(exit_flags, "locked");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      room->exit[door]->wall = WALL_GLASS;
      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];
      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->wall = WALL_GLASS;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "smallwindow")) {
      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `WDecorate smallwindow `g(`Wdirection`g)`x\n\r", ch);
        return;
      }

      int value, rev;
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }
      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }

      if (!room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }
      if (room->exit[door]->doorbroken > 0) {
        send_to_char("That's too badly damaged right now.\n\r", ch);
        return;
      }

      if (!can_decorate(ch, room->exit[door]->u1.to_room) && prop_from_room(room->exit[door]->u1.to_room) != NULL) {
        send_to_char("You can't access the other side.\n\r", ch);
        return;
      }

      if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
        value = flag_value(exit_flags, "door");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }

        value = flag_value(exit_flags, "closed");

        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (IS_SET(room->exit[door]->exit_info, EX_LOCKED)) {
        value = flag_value(exit_flags, "locked");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      if (!IS_SET(room->exit[door]->exit_info, EX_HIDDEN)) {
        value = flag_value(exit_flags, "hidden");
        TOGGLE_BIT(room->exit[door]->rs_flags, value);
        room->exit[door]->exit_info = room->exit[door]->rs_flags;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL) {

          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
        }
      }
      room->exit[door]->wall = WALL_GLASS;
      pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->wall = WALL_GLASS;
      }

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "indoors")) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("`cThat`g'`cs already an indoors room`g.\n\r", ch);
        return;
      }

      if (room->z >= 0 && !can_develop_proproom(room)) {
        send_to_char("That room can't be developed.\n\r", ch);
        return;
      }

      if (get_roomz(room) >= 0) {
        if(!renovating(ch)) {
          ch->pcdata->total_money -= ROOM_PRICE * 100;
          printf_to_char(ch, "You pay %d to build a new room.\n\r", ROOM_PRICE);
        }
        prop_from_room(room)->price += ROOM_PRICE;
      }

      SET_BIT(room->room_flags, ROOM_INDOORS);
      autowall(room);
      setup_proproom(room, prop_from_room(room));
      send_to_char("`cDone`g.\n\r", ch);
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "signalproof")) {
      if (!IS_SET(room->room_flags, ROOM_SILENCED)) {
        SET_BIT(room->room_flags, ROOM_SILENCED);
        send_to_char("You make the room signalproof.\n\r", ch);
        if(!renovating(ch))
        ch->pcdata->total_money -= 50000;
        return;
      }
      else {
        REMOVE_BIT(room->room_flags, ROOM_SILENCED);
        send_to_char("You remove the room's signalproofing.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "decor")) {
      int level = atoi(argument);
      if (level < 0 || level > 3) {
        send_to_char("Decorate decor 0-3\n\r", ch);
        return;
      }
      int diff = level - room->decor;
      if (diff > 0) {
        if(!renovating(ch)) {
          ch->pcdata->total_money -=
          UMAX(ROOM_PRICE, base_room_price(room)) * DECOR_ONE * diff;
          printf_to_char(ch, "You pay $%d to improve the decor.\n\r", UMAX(ROOM_PRICE, base_room_price(room)) * DECOR_ONE *
          diff / 100);
        }
      }
      room->decor = level;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "toughness")) {
      int level = atoi(argument);
      if (level < 0 || level > 4) {
        send_to_char("Decorate toughness 0-4\n\r", ch);
        return;
      }
      int diff = level - room->toughness;
      if (diff > 0) {
        ch->pcdata->total_money -=
        UMAX(ROOM_PRICE, base_room_price(room)) * TOUGHNESS_PRICE * diff;
        printf_to_char(ch, "You pay $%d to improve the toughness.\n\r", UMAX(ROOM_PRICE, base_room_price(room)) * TOUGHNESS_PRICE *
        diff / 100);
      }
      room->toughness = level;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "security")) {
      int level = atoi(argument);
      if (level < 0 || level > 4) {
        send_to_char("Decorate security 0-4\n\r", ch);
        return;
      }
      int diff = level - room->security;
      if (diff > 0) {
        ch->pcdata->total_money -= SECURITY_PRICE * skillpoint(diff) * 100;
        printf_to_char(ch, "You pay $%d to improve the security.\n\r", SECURITY_PRICE * skillpoint(diff));
      }
      room->security = level;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "kitchen")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("You can't build an outdoors kitchen.\n\r", ch);
        return;
      }
      if (prop_from_room(room) == NULL || prop_from_room(room)->type == PROP_SHOP) {
        send_to_char("You can only build that in a house.\n\r", ch);
        return;
      }
      if (IS_SET(room->room_flags, ROOM_KITCHEN)) {
        REMOVE_BIT(room->room_flags, ROOM_KITCHEN);
        send_to_char("You dismantle your kitchen.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_KITCHEN);
      ch->pcdata->total_money -= FEATURE_PRICE * 100;
      printf_to_char(ch, "You pay $%d to install a kitchen.\n\r", FEATURE_PRICE);
    }
    else if (!str_cmp(arg1, "bedroom")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("You can't build an outdoors bedroom.\n\r", ch);
        return;
      }
      if (prop_from_room(room) == NULL || prop_from_room(room)->type == PROP_SHOP) {
        send_to_char("You can only build that in a house.\n\r", ch);
        return;
      }

      if (IS_SET(room->room_flags, ROOM_BEDROOM)) {
        REMOVE_BIT(room->room_flags, ROOM_BEDROOM);
        send_to_char("You dismantle your bedroom.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_BEDROOM);
      ch->pcdata->total_money -= FEATURE_PRICE * 100;
      printf_to_char(ch, "You pay $%d to build a bedroom.\n\r", FEATURE_PRICE);
    }
    else if (!str_cmp(arg1, "bathroom")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("You can't build an outdoors bathroom.\n\r", ch);
        return;
      }
      if (prop_from_room(room) == NULL || prop_from_room(room)->type == PROP_SHOP) {
        send_to_char("You can only build that in a house.\n\r", ch);
        return;
      }

      if (IS_SET(room->room_flags, ROOM_BATHROOM)) {
        REMOVE_BIT(room->room_flags, ROOM_BATHROOM);
        send_to_char("You dismantle your bathroom.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_BATHROOM);
      printf_to_char(ch, "You pay $%d to install a bathroom.\n\r", FEATURE_PRICE);
      ch->pcdata->total_money -= FEATURE_PRICE * 100;
    }
    else if (!str_cmp(arg1, "type")) {
      if (prop_from_room(room) == NULL)
      return;
      int subtype = prop_from_room(room)->type_special;
      if (subtype == PROPERTY_TOWNHOUSE || subtype == PROPERTY_TOWNSHOP || subtype == PROPERTY_TRAILER || subtype == PROPERTY_UNDERSHOP || subtype == PROPERTY_OVERFLAT || subtype == PROPERTY_LONGHOUSE || subtype == PROPERTY_LONGSHOP || subtype == PROPERTY_SMALLFLAT || subtype == PROPERTY_MEDFLAT || subtype == PROPERTY_BIGFLAT) {
        send_to_char("You'd be violating city zoning laws.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "shop")) {
        prop_from_room(ch->in_room)->type = PROP_SHOP;
        prop_type_switch(prop_from_room(ch->in_room), PROP_SHOP);
        prop_from_room(ch->in_room)->prop_state = 0;
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 30000;
          printf_to_char(
          ch, "You pay $%d to convert this property to a shop.\n\r", 300);
        }
        prop_from_room(ch->in_room)->price =
        property_truevalue(prop_from_room(ch->in_room));

        return;
      }
      if (!str_cmp(argument, "house") && (prop_from_room(ch->in_room)->prop_state != 0 || prop_from_room(ch->in_room)->type != PROP_HOUSE)) {
        prop_from_room(ch->in_room)->type = PROP_HOUSE;
        prop_type_switch(prop_from_room(ch->in_room), PROP_HOUSE);
        prop_from_room(ch->in_room)->prop_state = 0;
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 30000;
          printf_to_char(
          ch, "You pay $%d to convert this property to a house.\n\r", 300);
        }
        prop_from_room(ch->in_room)->price =
        property_truevalue(prop_from_room(ch->in_room));

        send_to_char("Done.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "anonymous") && prop_from_room(ch->in_room)->prop_state != 1) {
        prop_from_room(ch->in_room)->type = PROP_HOUSE;
        prop_type_switch(prop_from_room(ch->in_room), PROP_HOUSE);
        prop_type_switch(prop_from_room(ch->in_room), 10);
        prop_from_room(ch->in_room)->prop_state = 1;
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 20000;
          printf_to_char(
          ch, "You pay $%d to convert this property be anonymously owned.\n\r", 200);
        }
        prop_from_room(ch->in_room)->price =
        property_truevalue(prop_from_room(ch->in_room));

        send_to_char("Done.\n\r", ch);
        return;
      }

    }
    else if (!str_cmp(arg1, "public")) {
      if (!IS_SET(room->room_flags, ROOM_PUBLIC)) {
        SET_BIT(room->room_flags, ROOM_PUBLIC);
        send_to_char("You open the area up for public access.\n\r", ch);
        if (IS_SET(room->room_flags, ROOM_PRIVATE))
        REMOVE_BIT(room->room_flags, ROOM_PRIVATE);
        return;
      }
      else {
        REMOVE_BIT(room->room_flags, ROOM_PUBLIC);
        send_to_char("You close the area off from public access.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "private")) {
      if (!IS_SET(room->room_flags, ROOM_PRIVATE)) {
        SET_BIT(room->room_flags, ROOM_PRIVATE);
        send_to_char("You close the area off from public access.\n\r", ch);
        if (IS_SET(room->room_flags, ROOM_PUBLIC))
        REMOVE_BIT(room->room_flags, ROOM_PUBLIC);
        return;
      }
      else {
        REMOVE_BIT(room->room_flags, ROOM_PRIVATE);
        send_to_char("You open the area up for private access.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "sparring")) {
      if (!IS_SET(room->room_flags, ROOM_SPARRING)) {
        SET_BIT(room->room_flags, ROOM_SPARRING);
        send_to_char("You set up a sparring area.\n\r", ch);
        if(!renovating(ch)) {
          ch->pcdata->total_money -= 50000;
          printf_to_char(ch, "You pay $%d to setup the area.\n\r", 500);
        }
        return;
      }
      else {
        REMOVE_BIT(room->room_flags, ROOM_SPARRING);
        send_to_char("You close down the sparring area.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "outdoors")) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
        send_to_char("`cThat`g'`cs already a outdoors room`g.\n\r", ch);
        return;
      }
      if (get_roomz(room) >= 0) {
        if (prop_from_room(room) != NULL)
        prop_from_room(room)->price -= ROOM_PRICE;
      }
      REMOVE_BIT(room->room_flags, ROOM_INDOORS);
      if (prop_from_room(room) != NULL)
      setup_proproom(room, prop_from_room(room));
      send_to_char("Done\n\r", ch);
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "extra")) {
      EXTRA_DESCR_DATA *ed;

      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `Wdecorate extra `g(`WExisting or new keyword`g)`x\n\r", ch);
        return;
      }

      for (ed = room->extra_descr; ed; ed = ed->next) {
        if (is_name(argument, ed->keyword))
        break;
      }
      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup(argument);
        ed->next = room->extra_descr;
        room->extra_descr = ed;
        string_append(ch, &room->extra_descr->description);
      }
      else
      string_append(ch, &ed->description);

      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "access")) {
      EXTRA_DESCR_DATA *ed;

      for (ed = room->extra_descr; ed; ed = ed->next) {
        if (is_name("!keys", ed->keyword))
        break;
      }
      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("!keys");
        ed->next = room->extra_descr;
        room->extra_descr = ed;
        string_append(ch, &room->extra_descr->description);
      }
      else
      string_append(ch, &ed->description);

      send_to_char("`cEnter the names of all the people you'd like to have access`g, `cseperated by a space`g.\n\r", ch);

      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "noaccess")) {
      EXTRA_DESCR_DATA *ed;

      for (ed = room->extra_descr; ed; ed = ed->next) {
        if (is_name("!denial", ed->keyword))
        break;
      }
      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("!denial");
        ed->next = room->extra_descr;
        room->extra_descr = ed;
        string_append(ch, &room->extra_descr->description);
      }
      else
      string_append(ch, &ed->description);

      send_to_char("`cEnter the names of all the people you'd like to not have access`g, `cseperated by a space`g.\n\r", ch);

      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "extradelete")) {
      EXTRA_DESCR_DATA *ed;
      EXTRA_DESCR_DATA *ped = NULL;

      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `Wdecorate extradelete `g(`Wkeyword`g)`x\n\r", ch);
        return;
      }

      for (ed = room->extra_descr; ed; ed = ed->next) {
        if (ed->next != NULL && is_name(argument, ed->next->keyword))
        ped = ed;
        if (is_name(argument, ed->keyword))
        break;
      }

      if (ed) {
        if (!ped)
        room->extra_descr = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "place")) {
      EXTRA_DESCR_DATA *ed;

      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `Wdecorate place `g(`WExisting or new keyword`c)`x\n\r", ch);
        return;
      }
      if (in_haven(room)) {
        if ((strcasestr(argument, "shower") || strcasestr(argument, "bath") || strcasestr(argument, "jacuzzi") || strcasestr(argument, "tub")) && !IS_SET(ch->in_room->room_flags, ROOM_BATHROOM) && !is_base(prop_from_room(ch->in_room))) {
          send_to_char("This room has to be set as a bathroom first.\n\r", ch);
          return;
        }
        if (strcasestr(argument, "pool") && !strcasestr(argument, "table")) {
          if (!IS_SET(room->room_flags, ROOM_BATHROOM)) {
            send_to_char("This room has to be set as a bathroom first.\n\r", ch);
            return;
          }
          if (get_decor(room) < 1) {
            send_to_char("The decor has to be higher to add a pool.\n\r", ch);
            return;
          }
        }
        if (strcasestr(argument, "jacuzzi") && get_decor(room) < 2) {
          send_to_char("The decor has to be higher to add a jacuzzi.\n\r", ch);
          return;
        }
      }

      for (ed = room->places; ed; ed = ed->next) {
        if (is_name(argument, ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup(argument);
        ed->next = room->places;
        room->places = ed;
        string_append(ch, &room->places->description);
      }
      else
      string_append(ch, &ed->description);

      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "placedelete")) {
      EXTRA_DESCR_DATA *ed;
      EXTRA_DESCR_DATA *ped = NULL;

      if (!str_cmp(argument, "")) {
        send_to_char("`cSyntax`g: `Wdecorate placedelete `g(`Wkeyword`g)`x\n\r", ch);
        return;
      }

      for (ed = room->places; ed; ed = ed->next) {
        if (ed->next != NULL && is_name(argument, ed->next->keyword))
        ped = ed;
        if (is_name(argument, ed->keyword))
        break;
      }

      if (ed) {
        if (!ped)
        room->places = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "whereinvis") || !str_cmp(arg1, "wherevis") || !str_cmp(arg1, "hotspot")) {
      if (IS_SET(room->room_flags, ROOM_WHEREVIS)) {
        send_to_char("`cThis room is no longer a hotspot`g.`x\n\r", ch);
      }
      else {
        send_to_char("`cThis room is now a hotspot`g.`x\n\r", ch);
      }

      TOGGLE_BIT(room->room_flags, ROOM_WHEREVIS);
    }
    else
    send_to_char("`cSyntax`g: `Wdecorate `g(`Wdescription`g/`Wname`g/`Wwall`g/`Wdoor`g/`Wlargewindow`g/`Wsmallwindow`g/`Wdemolish`g/`Wlock`g/`Wname`g/`Wextra`g/`Wextradelete`g/`Wplace`g/`Wplacedelete`g/`Waccess`g/`Windoors`g/`Woutdoors`g/`Wnightmaredescription`g/`Wpublic`g.`x\n\r", ch);
  }

  void house_security_update(CHAR_DATA *ch) { return; }

  void houseguard_check(CHAR_DATA *ch, HOUSE_TYPE *house, int type) {}

  void houseguard_pcheck(CHAR_DATA *ch, PROP_TYPE *house, int type) {}

  _DOFUN(do_picklock) {
    char arg[MAX_INPUT_LENGTH];
    int door;
    bool success = FALSE;
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Open what?\n\r", ch);
      return;
    }
    if (ch->in_room == NULL) {
      send_to_char("Open what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You can't do that from the nightmare.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      send_to_char("That door won't stop you.\n\r", ch);
      return;
    }
    if (ch->shape != SHAPE_HUMAN) {
      send_to_char("You don't have hands.  Not real hands.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You're helpless.  Completely helpless.\n\r", ch);
      return;
    }
    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }

    if ((door = find_door(ch, arg)) >= 0) {
      /* 'open door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      int skill = get_skill(ch, SKILL_LARCENY);
      int lf = get_lifeforce(ch, FALSE, NULL);
      if (lf <= 95 || lf >= 105)
      skill += (lf / 10 - 10);

      pexit = ch->in_room->exit[door];
      if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
        send_to_char("It's already open.\n\r", ch);
        return;
      }
      if (!IS_SET(pexit->exit_info, EX_LOCKED)) {
        send_to_char("It isn't locked.\n\r", ch);
        return;
      }
      if (!door_locked(pexit, pexit->u1.to_room, ch)) {
        send_to_char("You have a key...\n\r", ch);
        return;
      }

      if (in_sheriff(ch->in_room) || in_sheriff(pexit->u1.to_room)) {
        send_to_char("You fail to pick the lock.\n\r", ch);
        act("$n tries to pick the lock.", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        return;
      }

      if (room_house(pexit->u1.to_room) != NULL) {
        HOUSE_TYPE *house = room_house(pexit->u1.to_room);

        if (skill >= house->security) {
          success = TRUE;
        }
      }
      else if (room_prop(pexit->u1.to_room) != NULL) {
        PROP_TYPE *prop = room_prop(pexit->u1.to_room);
        int diff = get_security(pexit->u1.to_room);
        if (prop->warded >= 75 && event_cleanse != 1)
        diff = 20;
        if (skill > diff) {
          success = TRUE;
        }
      }
      else {
        if (skill > 1) {
          success = TRUE;
        }
      }

      if (success == TRUE) {
        send_to_char("You pick the lock.\n\r", ch);
        act("$n tries to pick the lock.", ch, NULL, NULL, TO_ROOM);
        REMOVE_BIT(pexit->exit_info, EX_CLOSED);
        act("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM);

        if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
          REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
          for (CharList::iterator it = to_room->people->begin();
          it != to_room->people->end(); ++it)
          act("The $d opens.", *it, NULL, pexit_rev->keyword, TO_CHAR);
        }
      }
      else {
        send_to_char("You fail to pick the lock.\n\r", ch);
        act("$n tries to pick the lock.", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
      }
      return;
    }
  }

  void delete_circle(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ped = NULL;

    for (ed = room->places; ed; ed = ed->next) {
      if (ed->next != NULL && is_name("binding circle", ed->next->keyword))
      ped = ed;
      if (is_name("binding circle", ed->keyword))
      break;
    }

    if (ed) {
      if (!ped)
      room->places = ed->next;
      else
      ped->next = ed->next;

      free_extra_descr(ed);
    }
    SET_BIT(room->area->area_flags, AREA_CHANGED);
  }

  _DOFUN(do_breakin) {
    char arg[MAX_INPUT_LENGTH];
    int door;
    one_argument(argument, arg);
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    ROOM_INDEX_DATA *to_room;

    if (!strcmp(arg, "free")) {
      if (ch->in_room->vnum == 96) {
        char_from_room(ch);
        char_to_room(ch, get_room_index(4804));
        do_function(ch, &do_look, "auto");
        send_to_char("\n\rRipping through the birthing sac brings the stagnant water of the spawning pool pouring in.\n\r", ch);
        send_to_char("`RYou need to get to the surface!  Swim up and into the dim light!`x`x.\n\r", ch);
        act("$n breaks $s birthing sac and swims toward the surface.\n\r", ch, NULL, NULL, TO_ROOM);
        move_char(ch, DIR_UP, FALSE, FALSE);
        send_to_char("You burst from the water, gulping down your first breaths of free air.\n\r", ch);
        return;
      }
    }

    if (!str_cmp(arg, "circle")) {
      if (higher_power(ch)) {
        send_to_char("You cannot do that.\n\r", ch);
        return;
      }
      if (!has_binding_circle(ch->in_room)) {
        send_to_char("There is no binding circle here.\n\r", ch);
        return;
      }
      delete_circle(ch->in_room);
      send_to_char("You break the circle.\n\r", ch);
      act("$n breaks the circle.\n\r", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      return;
    }

    if (is_gm(ch)) {
      do_function(ch, &do_break, argument);
      return;
    }
    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You have no need for doors.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You can't do that from the nightmare.\n\r", ch);
      return;
    }
    if (is_ghost(ch)) {
      return;
    }
    if (is_helpless(ch)) {
      return;
    }
    if (is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) < ANIMAL_MANSIZED) {
      send_to_char("You're a bit small to be doing that.\n\r", ch);
      return;
    }
    if (arg[0] == '\0') {
      send_to_char("Open what?\n\r", ch);
      return;
    }
    if (ch->in_room == NULL) {
      send_to_char("Open what?\n\r", ch);
      return;
    }

    if ((door = find_door(ch, arg)) >= 0) {
      /* 'open door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (!IS_SET(pexit->exit_info, EX_CLOSED) && pexit->wall == WALL_NONE) {
        send_to_char("It's already open.\n\r", ch);
        return;
      }
      if (ch->in_room->vnum == 28002 || ch->in_room->vnum == 28003) {
        send_to_char("That isn't a good idea.\n\r", ch);
        return;
      }
      if (!IS_SET(pexit->exit_info, EX_LOCKED) && pexit->wall == WALL_NONE) {
        send_to_char("It isn't locked.\n\r", ch);
        return;
      }
      if (is_privatep(pexit->u1.to_room) && has_access(ch, pexit->u1.to_room)) {
        send_to_char("You have a key...\n\r", ch);
        return;
      }
      if (get_skill(ch, SKILL_STRENGTH) < 0) {
        send_to_char("You fail to break down the door.\n\r", ch);
        act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        return;
      }
      if (in_sheriff(ch->in_room) || in_sheriff(pexit->u1.to_room)) {
        send_to_char("You fail to break down the door.\n\r", ch);
        act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        return;
      }
      if (institute_room(ch->in_room) || institute_room(pexit->u1.to_room)) {
        send_to_char("You fail to break down the door.\n\r", ch);
        act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        return;
      }

      ch->pcdata->spent_resources += 25;

      if (room_house(pexit->u1.to_room) != NULL) {
        HOUSE_TYPE *house = room_house(pexit->u1.to_room);
        if (get_skill(ch, SKILL_STRENGTH) < 1 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && highestoffense(ch) < 35 && house->toughness > 0) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_STRENGTH) < 3 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && highestoffense(ch) < 50 && house->toughness > 1) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 2 && get_skill(ch, SKILL_STRENGTH) < 6 && highestoffense(ch) < 100 && house->toughness > 2) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 3 && get_skill(ch, SKILL_STRENGTH) < 8 && highestoffense(ch) < 150 && house->toughness > 3) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else {
          send_to_char("You break down the door..\n\r", ch);
          act("$n breaks down the door..", ch, NULL, NULL, TO_ROOM);
          REMOVE_BIT(pexit->exit_info, EX_CLOSED);
          pexit->doorbroken = 120;
          if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            pexit_rev->doorbroken = 120;
            REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("The $d crashes open.", *it, NULL, pexit_rev->keyword, TO_CHAR);
          }
          PROP_TYPE *prop;
          prop = room_prop(pexit->u1.to_room);
          if (prop != NULL) {
            if (prop->compromised != 1) {
              prop->compromised = 1;
              ch->money -= 5000;
            }
            CHAR_DATA *to;
            for (DescList::iterator itt = descriptor_list.begin();
            itt != descriptor_list.end(); ++itt) {
              DESCRIPTOR_DATA *d = *itt;
              if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
                to = d->character;
                if (IS_NPC(to))
                continue;
                if (to->in_room == NULL)
                continue;

                if (in_prop(to) == prop)
                printf_to_char(to, "You hear a loud bang.\n\r");
              }
            }
          }
        }
        houseguard_check(ch, house, TRAVEL_FORCE);
        return;
      }
      if (room_prop(pexit->u1.to_room) != NULL) {
        PROP_TYPE *prop = room_prop(pexit->u1.to_room);
        if (prop->compromised == 0 && ch->money < 5000) {
          send_to_char("That would cost $50 in supplies.\n\r", ch);
          return;
        }

        int toughness = get_toughness(pexit->u1.to_room);

        if (get_skill(ch, SKILL_STRENGTH) < 0) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (in_sheriff(ch->in_room) || in_sheriff(pexit->u1.to_room)) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (prop->warded >= 75 && event_cleanse != 1) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (get_skill(ch, SKILL_STRENGTH) < 1 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && toughness > 0) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_STRENGTH) < 4 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && toughness > 1) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 2 && get_skill(ch, SKILL_STRENGTH) < 6 && toughness > 1) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 3 && get_skill(ch, SKILL_STRENGTH) < 8 && toughness > 3) {
          send_to_char("You fail to break down the door.\n\r", ch);
          act("$n fails to break down the door.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else {
          send_to_char("You break down the door.\n\r", ch);
          act("$n breaks down the door.", ch, NULL, NULL, TO_ROOM);
          REMOVE_BIT(pexit->exit_info, EX_CLOSED);
          pexit->doorbroken = 120;

          if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            pexit_rev->doorbroken = 120;
            REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("The $d crashes open.", *it, NULL, pexit_rev->keyword, TO_CHAR);
          }
          PROP_TYPE *prop;
          prop = room_prop(pexit->u1.to_room);
          if (prop != NULL) {
            if (prop->compromised != 1) {
              prop->compromised = 1;
              ch->money -= 5000;
            }
            CHAR_DATA *to;
            for (DescList::iterator itt = descriptor_list.begin();
            itt != descriptor_list.end(); ++itt) {
              DESCRIPTOR_DATA *d = *itt;
              if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
                to = d->character;
                if (IS_NPC(to))
                continue;
                if (to->in_room == NULL)
                return;
                if (in_prop(to) == prop)
                printf_to_char(to, "You hear a loud bang.\n\r");
              }
            }
            for (int i = 0; i < 50; i++) {
              if (safe_strlen(prop->sleepers[i]) > 1)
              autohouseflee(prop->sleepers[i], prop, get_intro(ch));
            }
          }
        }
        houseguard_pcheck(ch, prop, TRAVEL_FORCE);
        return;
      }

      if (get_skill(ch, SKILL_DEMOLITIONS) > 1 || highestoffense(ch) >= 75 || get_skill(ch, SKILL_STRENGTH) > 3) {
        send_to_char("You break down the door.\n\r", ch);
        act("$n breaks down the door.", ch, NULL, NULL, TO_ROOM);
        REMOVE_BIT(pexit->exit_info, EX_CLOSED);
        pexit->doorbroken = 120;

        if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
          pexit_rev->doorbroken = 120;
          REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
          for (CharList::iterator it = to_room->people->begin();
          it != to_room->people->end(); ++it)
          act("The $d crashes open.", *it, NULL, pexit_rev->keyword, TO_CHAR);
        }
        PROP_TYPE *prop;
        prop = room_prop(pexit->u1.to_room);
        if (prop != NULL) {
          if (prop->compromised != 1) {
            prop->compromised = 1;
            ch->money -= 5000;
          }
          CHAR_DATA *to;
          for (DescList::iterator itt = descriptor_list.begin();
          itt != descriptor_list.end(); ++itt) {
            DESCRIPTOR_DATA *d = *itt;
            if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
              to = d->character;
              if (IS_NPC(to))
              continue;
              if (to->in_room == NULL)
              return;
              if (in_prop(to) == prop)
              printf_to_char(to, "You hear a loud bang.\n\r");
            }
          }
          for (int i = 0; i < 50; i++) {
            if (safe_strlen(prop->sleepers[i]) > 1)
            autohouseflee(prop->sleepers[i], prop, get_intro(ch));
          }
        }

        return;
      }
      else {
        send_to_char("You fail to break down the door.\n\r", ch);
        act("$n fails to break down the door.", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        return;
      }
    }
    else {
      if (!str_prefix(arg, "north")) {
        door = DIR_NORTH;
      }
      else if (!str_prefix(arg, "south"))
      door = DIR_SOUTH;
      else if (!str_prefix(arg, "east"))
      door = DIR_EAST;
      else if (!str_prefix(arg, "west"))
      door = DIR_WEST;
      else if (!str_prefix(arg, "up"))
      door = DIR_UP;
      else if (!str_prefix(arg, "down"))
      door = DIR_DOWN;
      else if (!str_prefix(arg, "northwest") || !str_cmp(argument, "nw"))
      door = DIR_NORTHWEST;
      else if (!str_prefix(arg, "northeast") || !str_cmp(argument, "ne"))
      door = DIR_NORTHEAST;
      else if (!str_prefix(arg, "southwest") || !str_cmp(argument, "sw"))
      door = DIR_SOUTHWEST;
      else if (!str_prefix(arg, "southeast") || !str_cmp(argument, "se"))
      door = DIR_SOUTHEAST;
      else
      door = -1;

      ROOM_INDEX_DATA *in_room = ch->in_room;
      if (door == -1 || (pexit = in_room->exit[door]) == NULL || pexit->wall == WALL_NONE) {
        send_to_char("You can't break that way\n\r", ch);
        return;
      }

      if (IS_SET(pexit->exit_info, EX_HELLGATE)) {
        if (in_world(ch) == WORLD_HELL) {
          send_to_char("You fail to break down the barrier of branches and thorns.\n\r", ch);
          act("$n fails to break down the barrier of branches and thorns.", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (get_skill(ch, SKILL_STRENGTH) < 3 && get_skill(ch, SKILL_DEMOLITIONS) < 1) {
          send_to_char("You fail to break down the barrier of branches and thorns.\n\r", ch);
          act("$n fails to break down the barrier of branches and thorns.", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        demon_hole(ch->in_room);
        pexit->wallcondition = WALLCOND_HOLE;

        send_to_char("You break through the barrier of branches and thorns.\n\r", ch);
        act("$n breaks through the barrier of branches and thorns.", ch, NULL, NULL, TO_ROOM);
        pexit->doorbroken = 120;

        if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
          pexit_rev->doorbroken = 120;
          pexit_rev->wallcondition = WALLCOND_HOLE;
          for (CharList::iterator it = to_room->people->begin();
          it != to_room->people->end(); ++it)
          act("A hole appears in the $d barrier of branches and thorns.", *it, NULL, pexit_rev->keyword, TO_CHAR);
        }
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        return;
      }

      if (room_house(pexit->u1.to_room) != NULL) {
        HOUSE_TYPE *house = room_house(pexit->u1.to_room);

        if (get_skill(ch, SKILL_STRENGTH) < 0) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (in_sheriff(ch->in_room) || in_sheriff(pexit->u1.to_room)) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }

        if (get_skill(ch, SKILL_STRENGTH) < 1 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && house->toughness > 0) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_STRENGTH) < 3 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && house->toughness > 1) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 2 && get_skill(ch, SKILL_STRENGTH) < 6 && house->toughness > 2) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 3 && get_skill(ch, SKILL_STRENGTH) < 8 && house->toughness > 3) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the dwall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else {
          pexit->wallcondition = WALLCOND_HOLE;

          send_to_char("You break a hole in the wall.\n\r", ch);
          act("$n breaks a hole in the wall.", ch, NULL, NULL, TO_ROOM);
          pexit->doorbroken = 120;

          if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            pexit_rev->doorbroken = 120;
            pexit_rev->wallcondition = WALLCOND_HOLE;
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("A hole is blasted in the $d wall.", *it, NULL, pexit_rev->keyword, TO_CHAR);
          }
          PROP_TYPE *prop;
          prop = room_prop(pexit->u1.to_room);
          if (prop != NULL) {
            CHAR_DATA *to;
            for (DescList::iterator itt = descriptor_list.begin();
            itt != descriptor_list.end(); ++itt) {
              DESCRIPTOR_DATA *d = *itt;
              if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
                to = d->character;
                if (IS_NPC(to))
                continue;
                if (to->in_room == NULL)
                return;

                if (in_prop(to) == prop)
                printf_to_char(to, "You hear a loud bang.\n\r");
              }
            }
            for (int i = 0; i < 50; i++) {
              if (safe_strlen(prop->sleepers[i]) > 1)
              autohouseflee(prop->sleepers[i], prop, get_intro(ch));
            }
          }
        }
        houseguard_check(ch, house, TRAVEL_FORCE);
        return;
      }
      if (room_prop(pexit->u1.to_room) != NULL) {
        PROP_TYPE *house = room_prop(pexit->u1.to_room);

        int toughness = get_toughness(pexit->u1.to_room);
        if (house->compromised == 0 && ch->money < 5000) {
          send_to_char("That would cost $50 in supplies.\n\r", ch);
          return;
        }
        if (get_skill(ch, SKILL_STRENGTH) < 0) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (in_sheriff(ch->in_room) || in_sheriff(pexit->u1.to_room)) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (house->warded >= 75 && event_cleanse != 1) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
          return;
        }
        if (get_skill(ch, SKILL_STRENGTH) < 1 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && toughness > 0) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_STRENGTH) < 3 && get_skill(ch, SKILL_DEMOLITIONS) < 1 && toughness > 1) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 2 && get_skill(ch, SKILL_STRENGTH) < 6 && toughness > 2) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the wall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else if (get_skill(ch, SKILL_DEMOLITIONS) < 3 && get_skill(ch, SKILL_STRENGTH) < 8 && toughness > 3) {
          send_to_char("You fail to break down the wall.\n\r", ch);
          act("$n fails to break down the dwall.\n\r", ch, NULL, NULL, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        }
        else {
          pexit->wallcondition = WALLCOND_HOLE;
          send_to_char("You break a hole in the wall.\n\r", ch);
          act("$n breaks a hole in the wall.", ch, NULL, NULL, TO_ROOM);
          pexit->doorbroken = 120;
          if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            pexit_rev->doorbroken = 120;
            pexit_rev->wallcondition = WALLCOND_HOLE;
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("A hole is blasted in the $d wall.", *it, NULL, pexit_rev->keyword, TO_CHAR);
          }
          PROP_TYPE *prop;
          prop = room_prop(pexit->u1.to_room);
          if (prop != NULL) {
            if (prop->compromised != 1) {
              prop->compromised = 1;
              ch->money -= 5000;
            }
            CHAR_DATA *to;
            for (DescList::iterator itt = descriptor_list.begin();
            itt != descriptor_list.end(); ++itt) {
              DESCRIPTOR_DATA *d = *itt;
              if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
                to = d->character;
                if (IS_NPC(to))
                continue;
                if (to->in_room == NULL)
                return;
                if (in_prop(to) == prop)
                printf_to_char(to, "You hear a loud bang.\n\r");
              }
            }
            for (int i = 0; i < 50; i++) {
              if (safe_strlen(prop->sleepers[i]) > 1)
              autohouseflee(prop->sleepers[i], prop, get_intro(ch));
            }
          }
        }
        houseguard_pcheck(ch, house, TRAVEL_FORCE);
        return;
      }
    }
  }

  char *get_house_address(CHAR_DATA *ch) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_HOUSE || (*it)->prop_state == 1) {
        continue;
      }

      if (!str_cmp(ch->name, (*it)->owner)) {
        return (*it)->address;
      }
    }

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }

      if (!str_cmp(ch->name, (*it)->owner)) {
        return (*it)->address;
      }
    }

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }

      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), (*it)->tenants[i]))
        return (*it)->address;
      }
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_HOUSE || (*it)->prop_state == 1) {
        continue;
      }

      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), (*it)->tenants[i]))
        return (*it)->address;
      }
    }

    return "Unknown";
  }

  bool apartment_resident(CHAR_DATA *ch) {
    if (ch == NULL || !ch || ch->in_room == NULL || ch->name == NULL)
    return FALSE;

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }
      if ((*it)->vnum > 9)
      continue;

      if (has_access(ch, get_room_index((*it)->lower_vnum)))
      return TRUE;
    }
    return FALSE;
  }

  bool in_apartment(CHAR_DATA *ch) {
    if (ch == NULL || !ch || ch->in_room == NULL || ch->name == NULL)
    return FALSE;

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0') {
        continue;
      }
      if ((*it)->vnum > 9)
      continue;

      if (ch->in_room == NULL)
      return FALSE;
    }
    return FALSE;
  }

  bool homeowner(CHAR_DATA *ch) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_HOUSE) {
        continue;
      }

      if (!str_cmp(ch->name, (*it)->owner)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool shopowner(CHAR_DATA *ch) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_SHOP) {
        continue;
      }

      if (!str_cmp(ch->name, (*it)->owner)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool housesleeper(CHAR_DATA *ch) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_HOUSE) {
        continue;
      }

      if (!str_cmp(ch->name, (*it)->owner)) {
        return TRUE;
      }
    }

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->type != PROP_HOUSE) {
        continue;
      }

      for (int i = 0; i < 50; i++) {
        if (!str_cmp(ch->name, (*it)->tenants[i]))
        return TRUE;
      }
    }

    return FALSE;
  }

  _DOFUN(do_buzz) {
    ROOM_INDEX_DATA *frontroom;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    frontroom = get_room_index(1194);

    if (in_apartment(ch)) {
      if (!str_cmp(argument, "open")) {
        pexit = frontroom->exit[DIR_NORTH];
        if (IS_SET(pexit->exit_info, EX_CLOSED)) {
          REMOVE_BIT(pexit->exit_info, EX_CLOSED);
          for (CharList::iterator it = frontroom->people->begin();
          it != frontroom->people->end(); ++it)
          act("The $d opens.", *it, NULL, pexit->keyword, TO_CHAR);

          if ((to_room = pexit->u1.to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[DIR_NORTH]]) != NULL && pexit_rev->u1.to_room == ch->in_room) {
            REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
            for (CharList::iterator it = to_room->people->begin();
            it != to_room->people->end(); ++it)
            act("The $d opens.", *it, NULL, pexit_rev->keyword, TO_CHAR);
          }
        }
        send_to_char("You buzz them in.\n\r", ch);
        return;
      }
      else {
        send_to_char("You speak over the intercom.\n\r", ch);

        for (CharList::iterator it = frontroom->people->begin();
        it != frontroom->people->end(); ++it)
        printf_to_char(*it, "A voice comes over the intercom: %s\n\r", argument);
      }
    }
    else {
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      CHAR_DATA *to;

      if (ch->in_room->vnum != 1194) {
        send_to_char("There is no intercom here.\n\r", ch);
        return;
      }
      /*
if(!str_cmp(arg, "101"))
house = house_lookup(1);
else if(!str_cmp(arg, "102"))
house = house_lookup(2);
else if(!str_cmp(arg, "103"))
house = house_lookup(3);
else if(!str_cmp(arg, "104"))
house = house_lookup(4);
else if(!str_cmp(arg, "201"))
house = house_lookup(5);
else if(!str_cmp(arg, "202"))
house = house_lookup(6);
else if(!str_cmp(arg, "203"))
house = house_lookup(7);
else if(!str_cmp(arg, "204"))
house = house_lookup(8);
else if(!str_cmp(arg, "400"))
house = house_lookup(9);
else
{
send_to_char("That isn't a valid apartment, Syntax: Buzz (101-400)
(message).", ch); return;
}
*/
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;

          if (to->in_room == NULL)
          return;
        }
      }
      send_to_char("You speak over the intercom.\n\r", ch);
    }
  }

  _DOFUN(do_knock) {
    char arg[MSL];
    int door;
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Knock on which door?\n\r", ch);
      return;
    }
    if (ch->in_room == NULL)
    return;
    if (is_helpless(ch) || in_fight(ch))
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You can't interact with that from the nightmare.\n\r", ch);
      return;
    }

    if ((door = find_door(ch, arg)) >= 0) {
      /* 'open door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;

      pexit = ch->in_room->exit[door];
      if (pexit == NULL)
      return;
      if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
        send_to_char("It's already open.\n\r", ch);
        return;
      }

      if ((to_room = pexit->u1.to_room) == NULL) {
        send_to_char("You knock on the door.\n\r", ch);
        act("$n knocks on the door.\n\r", ch, NULL, NULL, TO_ROOM);
        return;
      }

      if (in_sheriff(to_room)) {
        CHAR_DATA *to;

        for (DescList::iterator itt = descriptor_list.begin();
        itt != descriptor_list.end(); ++itt) {
          DESCRIPTOR_DATA *d = *itt;

          if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;

            if (to->in_room == NULL)
            return;

            if (in_sheriff(to->in_room))
            printf_to_char(
            to, "You hear a knocking that sounds like it's coming from %s.\n\r", ch->in_room->name);
          }
        }
        send_to_char("You knock on the door.\n\r", ch);
        act("$n knocks on the door.\n\r", ch, NULL, NULL, TO_ROOM);
        return;
      }

      if (!is_privatep(to_room)) {
        send_to_char("You knock on the door.\n\r", ch);
        act("$n knocks on the door.\n\r", ch, NULL, NULL, TO_ROOM);
        for (CharList::iterator it = to_room->people->begin();
        it != to_room->people->end(); ++it)
        printf_to_char(*it, "You hear a knocking coming from the door to the %s\n\r", dir_name[rev_dir[door]][0]);
        return;
      }
      PROP_TYPE *prop;
      prop = room_prop(to_room);
      CHAR_DATA *to;
      for (DescList::iterator itt = descriptor_list.begin();
      itt != descriptor_list.end(); ++itt) {
        DESCRIPTOR_DATA *d = *itt;
        if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;

          if (to->in_room == NULL)
          continue;

          if (to == ch)
          continue;

          if (in_prop(to) == prop) {
            if (to->in_room == to_room)
            printf_to_char(to, "You hear a knocking from the %s.\n\r", dir_name[rev_dir[door]][0]);
            else
            printf_to_char(to, "You hear a knocking that sounds like it's coming from the door between %s and %s.\n\r", ch->in_room->name, to_room->name);
          }
        }
      }
      send_to_char("You knock on the door.\n\r", ch);
      act("$n knocks on the door.\n\r", ch, NULL, NULL, TO_ROOM);
      return;
    }
    send_to_char("Knock on what door?\n\r", ch);
  }

  bool can_house_flee(CHAR_DATA *ch) {
    if (institute_room(ch->in_room)) {
      if (college_student(ch, FALSE) || clinic_patient(ch)) {
        return FALSE;
      }
    }
    return TRUE;
  }

  _DOFUN(do_abduct) {
    Buffer outbuf;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    bool isChar = FALSE;
    char name[MAX_INPUT_LENGTH];

    d = new_descriptor();

    int chskill = 100;
    int vicskill = 100;

    if(crisis_prologue == 1)
    return;

    if (is_griefer(ch)) {
      send_to_char("They don't seem to be here.\n\r", ch);
      return;
    }
    if (is_helpless(ch) || in_fight(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (is_animal(ch) || is_ghost(ch)) {
      send_to_char("Hands would help.\n\r", ch);
      return;
    }
    if (guestmonster(ch) && ch->played / 3600 < 100) {
      send_to_char("Hands would help.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || ch->in_room->vnum < 300) {
      send_to_char("Not here.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Abduct who?\n\r", ch);
      return;
    }

    if (institute_room(ch->in_room) && ch->played / 3600 < 100) {
      send_to_char("The security staff won't allow you to do that.\n\r", ch);
      return;
    }

    WAIT_STATE(ch, PULSE_PER_SECOND * 10);

    argument[0] = UPPER(argument[0]);
    one_argument(argument, name);

    /* Dont want to load a second copy of a player who's allready online! */
    if (get_char_world_pc(name) != NULL) {
      send_to_char("They don't seem to be here.\n\r", ch);
      return;
    }
    log_string("DESCRIPTOR: Abduct");

    isChar = load_char_obj(d, name);
    if (!isChar) {
      send_to_char("They don't seem to be here..\n\r", ch);
      return;
    }
    d->character->desc = NULL;
    char_list.push_front(d->character);
    d->connected = CON_PLAYING;

    victim = d->character;

    if (is_gm(victim)) {
      send_to_char("Story Runners can't be abducted.\n\r", ch);
      real_quit(victim);
      return;
    }
    if (guestmonster(victim) || is_ghost(victim) || higher_power(ch) || higher_power(victim)) {
      real_quit(victim);
      return;
    }
    if (IS_FLAG(victim->act, PLR_FREEZE)) {
      send_to_char("They don't seem to be here\n\r", ch);
      real_quit(victim);
      return;
    }
    if (IS_FLAG(victim->act, PLR_STASIS)) {
      send_to_char("They don't seem to be here\n\r", ch);
      real_quit(victim);
      return;
    }

    if (ch->played / 3600 < 50) {
      send_to_char("They don't seem to be here\n\r", ch);
      real_quit(victim);
      return;
    }

    if (victim->in_room != ch->in_room) {
      send_to_char("They don't seem to be here\n\r", ch);
      real_quit(victim);
      return;
    }

    if (current_time - victim->lastlogoff > (3600 * 24 * 4) && !IS_FLAG(victim->act, PLR_BOUND) && !IS_FLAG(victim->act, PLR_BOUNDFEET)) {
      send_to_char("They don't seem to be here,\n\r", ch);
      real_quit(victim);
      return;
    }

    // Determining cost of abduction - Discordance revised 10/26/2016
    int cost = 0;
    if (is_vampire(victim) || IS_IMMORTAL(ch)) { // Vampire exclusion
      cost = 0;
    }
    else if (ch->pcdata->privatepartner !=
        NULL // private player losing link exclusion
        && !str_cmp(victim->name, ch->pcdata->privatepartner) && ch->privaterpexp > 10) {
      cost = 0;
    }
    else if (room_in_school(ch->in_room->vnum)) {
      cost = 0;
    }
    else if (IS_FLAG(victim->act, PLR_BOUND) || IS_FLAG(victim->act, PLR_BOUNDFEET)) { // Bound exclusion
      cost = 0;
    }
    else {
      cost = 5000;
    }

    if (cost > 0 && ch->money < cost) {
      send_to_char("It would cost $50 for the drugs to do that.\n\r", ch);
      real_quit(victim);
      return;
    }

    if (current_time - victim->lastlogoff < 120) {
      chskill += 1000;
    }

    if (is_ghost(ch)) {
      vicskill += 500;
    }

    if (ch->played / 3600 < 20)
    vicskill += 500;

    PROP_TYPE *prop;
    prop = room_prop(ch->in_room);
    if (prop != NULL && prop->type == PROP_HOUSE) {
      if (!has_access(ch, ch->in_room)) {
        vicskill += get_security(victim->in_room);
      }
      else {
        chskill += 150;
      }
    }

    if (prop == NULL)
    chskill += 100;

    chskill += get_lifeforce(ch, FALSE, NULL) * 2;
    vicskill += get_lifeforce(victim, FALSE, NULL) * 2;

    if (IS_AFFECTED(victim, AFF_WARD))
    vicskill += 200;

    if (victim->played / 3600 < 15)
    vicskill += 200;

    vicskill += UMIN(4, get_skill(victim, SKILL_ACUTESIGHT)) * 10;
    vicskill += UMIN(4, get_skill(victim, SKILL_ACUTEHEAR)) * 25;
    vicskill += UMIN(4, get_skill(victim, SKILL_ACUTESMELL)) * 25;
    vicskill += UMIN(4, get_skill(victim, SKILL_PERCEPTION)) * 10;
    vicskill += UMIN(4, get_skill(victim, SKILL_FASTREFLEXES)) * 5;

    if (bodyguard_abduct(ch, victim))
    vicskill += 1000;

    if (get_skill(victim, SKILL_HYPERPERCEPTION) > 0)
    vicskill = UMAX(vicskill + 50, 200);

    chskill += UMIN(5, get_skill(ch, SKILL_STEALTH)) * 40;

    if (ch->in_room->area->vnum == 12)
    vicskill += 500;

    vicskill += number_range(-50, 100);

    if (ch->pcdata->bloodaura > 0)
    vicskill += 500;

    if (newbie_level(victim) <= 3)
    vicskill += 100;

    if (!IS_NPC(victim) && victim->pcdata->godrealm_power > current_time && victim->pcdata->godrealm_power < current_time + (3600 * 24 * 60)) {
      send_to_char("Your needle fails to penetrate their skin.", ch);
      vicskill += 500;
    }

    // Vampire's during nighttime
    if (is_vampire(victim) && (sunphase(ch->in_room) == 0 || sunphase(ch->in_room) == 1 || sunphase(ch->in_room) == 7))
    vicskill += 100;

    if (IS_AFFECTED(victim, AFF_ABDUCTED))
    vicskill += 150;

    if (IS_AFFECTED(victim, AFF_DRAINED))
    vicskill += 100;

    if (victim->played / 3600 < 25)
    vicskill = 1000;

    if (is_weakness(ch, victim))
    vicskill /= 4;
    else if (has_weakness(ch, victim))
    vicskill *= 4;

    if (IS_FLAG(victim->comm, COMM_MANDRAKE))
    vicskill /= 2;

    if (IS_FLAG(victim->act, PLR_BOUND))
    vicskill = 0;
    if (IS_FLAG(victim->act, PLR_BOUNDFEET))
    vicskill = 0;

    if (victim->pcdata->sleeping > 0)
    vicskill = 0;

    if (current_time - victim->lastlogoff > (3600 * 20)) {
      vicskill = 0;
    }
    if (current_time - victim->lastlogoff < 300) {
      vicskill = 0;
    }

    if (ch->in_room->vnum == 3417 || ch->in_room->vnum == 5102
        || ch->in_room->vnum == 2007 || ch->in_room->vnum == 5115
        || ch->in_room->vnum == 5947 || ch->in_room->vnum == 8735
        || ch->in_room->vnum == 8999 || ch->in_room->vnum == 6526
        || ch->in_room->vnum == 9360 || ch->in_room->vnum == 9367
        || ch->in_room->vnum == 8776 || ch->in_room->vnum == 8696
        || ch->in_room->vnum == 9371 || ch->in_room->vnum == 9380) {
      vicskill = 0;
    }

    if (ch->pcdata->privatepartner != NULL // private player losing link exclusion
        && !str_cmp(victim->name, ch->pcdata->privatepartner) && ch->privaterpexp > 10) {
      vicskill = 0;
    }

    if (guestmonster(ch))
    vicskill = 1000;
    if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_NIGHTMARE)
    vicskill = 1000;
    if (IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->guest_type == GUEST_NIGHTMARE)
    vicskill = 0;


    // Special institute rules - Discordance revised 10/26/2016
    // revised 4/12/2019
    if (room_in_school(ch->in_room->vnum)) {
      // Abductions always successful if non-faculty sleeping outside dorms
      if (ch->in_room->sector_type != SECT_HOUSE) {
        vicskill = 0;
      }

      // Non-institute characters always abducted.
      if (!college_student(victim, FALSE) && !clinic_patient(victim)
          && victim->race != RACE_FACULTY && !college_staff(victim, FALSE)
          && !clinic_staff(victim, FALSE)) {
        vicskill = 0;
      }
    }
    // Special rules for reckless
    if (victim->pcdata->patrol_habits[PATROL_RECKLESSHABIT] > 0) {
      vicskill = 0;
    }
    if (has_clinic_power_chars(ch, victim)) {
      vicskill = 0;
    }
    else {
      if (institute_room(ch->in_room) && !college_student(ch, FALSE)
          && !clinic_patient(ch) && ch->race != RACE_FACULTY && !college_staff(ch, FALSE)
          && !clinic_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("The security staff won't allow you to do that.\n\r", ch);
        return;
      }
    }

    if (vicskill > chskill && !IS_IMMORTAL(ch) && can_house_flee(victim)) {
      act("$N wakes up as you approach and flees.", ch, NULL, victim, TO_CHAR);
      act("$n moves stealthily towards $N but then $E wakes up and flees.", ch, NULL, victim, TO_ROOM);
      char buf[MSL];
      sprintf(buf, "%s", victim->name);
      real_quit(victim);
      autohouseflee(buf, in_prop(ch), ch->name);
      return;
    }
    else {
      if (is_vampire(victim)) {
        act("You quickly and stealthily approach $N in their sleep.  They sleep on trustingly as they're moved.", ch, NULL, victim, TO_CHAR);
        act("$n moves quickly and stealthily towards $N.  They sleep on trustingly as they're moved.", ch, NULL, victim, TO_ROOM);
      }
      else {
        if (victim->pcdata->patrol_habits[PATROL_RECKLESSHABIT] == 0) {
          act("You notice $N is sleeping deeply and completely unaware of $S vulnerability.  You drug $M for good measure.", ch, NULL, victim, TO_CHAR);
          act("$n notices $N is sleeping deeply and vulnerable.  $n drugs $N for good measure.", ch, NULL, victim, TO_ROOM);
        }
        if (IS_FLAG(victim->act, PLR_BOUND) || IS_FLAG(victim->act, PLR_BOUNDFEET)) {
          act("You quickly and stealthily approach $N in their sleep.  They sleep on helplessly as they're moved.", ch, NULL, victim, TO_CHAR);
          act("$n moves quickly and stealthily towards $N.  They sleep on helplessly as they're moved.", ch, NULL, victim, TO_ROOM);
        }
        else {
          act("You quickly and stealthily approach $N in their sleep, drugging them to deepen their sleep.", ch, NULL, victim, TO_CHAR);
          act("$n moves quickly and stealthily towards $N, administering a drug to their sleeping form skillfully.", ch, NULL, victim, TO_ROOM);
        }
      }
      ch->money -= cost;
      victim->pcdata->sleeping = 30;

      if (in_prop(ch) != NULL) {
        from_sleepers(victim, in_prop(ch));
      }
      if (!IS_FLAG(victim->act, PLR_BOUND) && !IS_FLAG(victim->act, PLR_BOUNDFEET)) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = (12 * 60 * 20);
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_ABDUCTED;
        affect_to_char(victim, &af);
      }

      if (IS_FLAG(victim->act, PLR_HIDE))
      REMOVE_FLAG(victim->act, PLR_HIDE);

      if (ch->in_room != NULL)
      char_to_room(victim, ch->in_room); /* put in room imm is in */

      if (in_prop(ch) != NULL && in_prop(ch)->logoffs > 0) {
        in_prop(ch)->logoffs--;

        EXTRA_DESCR_DATA *ed;
        for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
          if (is_name("!sleepers", ed->keyword))
          break;
        }
        if (ed) {
          free_string(ed->description);
          ed->description = str_dup("");
        }
      }
      return;
    }
  }

  _DOFUN(do_tuckin) {
    CHAR_DATA *victim;
    char arg1[MSL];

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

    if (victim->pcdata->sleeping <= 0 || victim->desc != NULL) {
      send_to_char("They don't seem to want to be tucked in.\n\r", ch);
      return;
    }
    act("You tuck them in and send them back into a normal sleep.", ch, NULL, victim, TO_CHAR);
    act("$n tucks $N in and sends them back into a normal sleep.", ch, NULL, victim, TO_ROOM);
    victim->pcdata->sleeping = 0;
    save_char_obj(victim, FALSE, FALSE);
    if (in_prop(ch) != NULL) {
      in_prop(ch)->logoffs++;
      to_sleepers(victim, in_prop(ch));
    }
    real_quit(victim);
    //    char_from_room(victim);
    //    free_char(victim);
    //   extract_char(victim, TRUE);
  }

  _DOFUN(do_lookfor) {
    Buffer outbuf;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    bool isChar = FALSE;
    char name[MAX_INPUT_LENGTH];

    if (is_helpless(ch) || in_fight(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Lookfor who?\n\r", ch);
      return;
    }

    argument[0] = UPPER(argument[0]);
    one_argument(argument, name);

    /* Dont want to load a second copy of a player who's allready online! */
    if (get_char_world_pc(name) != NULL) {
      send_to_char("They don't seem to be here.\n\r", ch);
      return;
    }
    log_string("DESCRIPTOR: Abduct2");

    d = new_descriptor();
    isChar = load_char_obj(d, name); /* char pfile exists? */

    if (!isChar) {
      send_to_char("They don't seem to be here.\n\r", ch);
      return;
    }
    d->character->desc = NULL;
    char_list.push_front(d->character);
    d->connected = CON_PLAYING;
    //                  reset_char(d->character);
    victim = d->character;

    if (is_gm(victim)) {
      send_to_char("Story Runners can't be abducted.\n\r", ch);
      return;
    }

    if (victim->in_room != ch->in_room) {
      send_to_char("They don't seem to be here.\n\r", ch);
      extract_char(victim, TRUE);
      return;
    }
    if (current_time - victim->lastlogoff > 48200 && !IS_FLAG(victim->act, PLR_BOUND)) {
      send_to_char("They don't seem to be here.\n\r", ch);
      extract_char(victim, TRUE);
      return;
    }
    send_to_char("They are asleep here.\n\r", ch);
    extract_char(victim, TRUE);
  }

  int get_price(ROOM_INDEX_DATA *start, int size) {
    int baseprice;

    if (size == PROP_SMALL)
    baseprice = 500;
    if (size == PROP_MEDIUM)
    baseprice = 1000;
    if (size == PROP_LARGE)
    baseprice = 1500;
    if (size == PROP_XLARGE)
    baseprice = 2100;
    if (size == PROP_TALL)
    baseprice = 2100;

    if (start->vnum <= 1080)
    baseprice = baseprice * 3 / 2;
    else if (start->vnum <= 1190)
    baseprice = baseprice * 12 / 10;

    return baseprice;
  }

  char *create_address(ROOM_INDEX_DATA *start, int dir) {

    char buf[MSL];
    char newbuf[MSL];
    int num = 0;

    if (dir == DIR_EAST || dir == DIR_WEST || dir == DIR_NORTHEAST || dir == DIR_NORTHWEST)
    num = start->y + 2;
    else
    num = start->x + 2;

    int mod = num % 2;

    num += mod;

    if (dir == DIR_SOUTH || dir == DIR_WEST || dir == DIR_SOUTHWEST || dir == DIR_SOUTHEAST)
    num++;

    if (num == 0)
    num += 2;

    if (num < 0)
    num *= -1;

    if (num > 1000) {
      mod = num % 1000;
      num = mod;
    }

    remove_color(buf, start->name);

    sprintf(newbuf, "%d %s", num, buf);

    return str_dup(newbuf);
  }

  void make_exit(int vnum_one, int vnum_two, int dir, int type) {
    ROOM_INDEX_DATA *from;
    ROOM_INDEX_DATA *to;
    EXIT_DATA *pExit;
    bool one_way = FALSE;

    from = get_room_index(vnum_one);
    to = get_room_index(vnum_two);

    if (from == NULL || to == NULL)
    return;

    if (from->exit[dir] != NULL)
    return;

    if (to->exit[rev_dir[dir]] != NULL)
    one_way = TRUE;

    //    if(one_way == TRUE && type != CONNECT_WINDOW)
    // 	  return;

    from->exit[dir] = new_exit();
    from->exit[dir]->u1.to_room = to;
    from->exit[dir]->orig_door = dir;

    if (!one_way && type != CONNECT_ONEWAY) {
      pExit = new_exit();
      pExit->u1.to_room = from;
      pExit->orig_door = rev_dir[dir];
      to->exit[rev_dir[dir]] = pExit;
    }

    int value;

    if (type == CONNECT_FRONTDOOR) {
      value = flag_value(exit_flags, "door");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;

      value = flag_value(exit_flags, "closed");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;

      value = flag_value(exit_flags, "locked");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

    }
    else if (type == CONNECT_SHOPDOOR) {
      value = flag_value(exit_flags, "door");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;

      value = flag_value(exit_flags, "closed");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;
    }
    else if (type == CONNECT_BASEMENT) {
      value = flag_value(exit_flags, "door");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;

      value = flag_value(exit_flags, "closed");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;

      value = flag_value(exit_flags, "locked");
      TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
      to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;

    }
    else if (type == CONNECT_WINDOW) {

      value = flag_value(exit_flags, "hidden");
      TOGGLE_BIT(from->exit[dir]->rs_flags, value);
      from->exit[dir]->exit_info = from->exit[dir]->rs_flags;

      if (!one_way) {
        TOGGLE_BIT(to->exit[rev_dir[dir]]->rs_flags, value);
        to->exit[rev_dir[dir]]->exit_info = to->exit[rev_dir[dir]]->rs_flags;
      }
      from->exit[dir]->wall = WALL_GLASS;

      if (!one_way)
      to->exit[rev_dir[dir]]->wall = WALL_GLASS;
    }

    if (to != NULL && from != NULL && from->area != NULL && to->area != NULL) {
      if (from->area->vnum == to->area->vnum)
      SET_BIT(from->area->area_flags, AREA_CHANGED);
      else {
        SET_BIT(from->area->area_flags, AREA_CHANGED);
        SET_BIT(to->area->area_flags, AREA_CHANGED);
      }
    }
  }

  int first_free_property(void) {
    int temp = 45000;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      temp = (*it)->higher_vnum + 1;

      bool found = FALSE;
      for (vector<PROP_TYPE *>::iterator ij = PropVect.begin();
      ij != PropVect.end(); ++ij) {
        if (!(*ij)->propname || (*ij)->propname[0] == '\0') {
          continue;
        }
        if ((*ij)->valid == FALSE)
        continue;

        if (temp <= (*ij)->higher_vnum && temp >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 5 <= (*ij)->higher_vnum && temp + 5 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 10 <= (*ij)->higher_vnum && temp + 10 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 15 <= (*ij)->higher_vnum && temp + 15 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 20 <= (*ij)->higher_vnum && temp + 20 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 30 <= (*ij)->higher_vnum && temp + 30 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 40 <= (*ij)->higher_vnum && temp + 40 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 50 <= (*ij)->higher_vnum && temp + 50 >= (*ij)->lower_vnum)
        found = TRUE;
        if (temp + 60 <= (*ij)->higher_vnum && temp + 60 >= (*ij)->lower_vnum)
        found = TRUE;
      }

      if (found == FALSE)
      return temp;
    }

    temp++;
    return temp;
  }

  bool shop_exists(int num) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_SHOP)
      continue;

      if ((*it)->vnum == num)
      return TRUE;
    }
    return FALSE;
  }

  bool house_exists(int num) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_HOUSE)
      continue;

      if ((*it)->vnum == num)
      return TRUE;
    }
    return FALSE;
  }

  int shop_count(int num) {
    int count = 0;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_SHOP)
      continue;

      if ((*it)->vnum == num)
      count++;
    }
    return count;
  }

  int house_count(int num) {
    int count = 0;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_HOUSE)
      continue;

      if ((*it)->vnum == num)
      count++;
    }
    return count;
  }

  bool land_exists(int num) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_LAND)
      continue;

      if ((*it)->vnum == num)
      return TRUE;
    }
    return FALSE;
  }

  int next_prop_hvnum(void) {
    int vnum = 1;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_HOUSE)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 1;
      if (!house_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_HOUSE)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 5;
      if (!house_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_HOUSE)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 10;
      if (!house_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_HOUSE)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 15;
      if (!house_exists(vnum))
      return vnum;
    }
    return vnum;
  }

  int next_prop_svnum(void) {
    int vnum = 1;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_SHOP)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 1;
      if (!shop_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_SHOP)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 5;
      if (!shop_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_SHOP)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 10;
      if (!shop_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_SHOP)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 15;
      if (!shop_exists(vnum))
      return vnum;
    }
    return vnum;
  }

  int next_prop_lvnum(void) {
    int vnum = 1;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_LAND)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 1;
      if (!land_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_LAND)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 5;
      if (!land_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_LAND)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 10;
      if (!land_exists(vnum))
      return vnum;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->type != PROP_LAND)
      continue;

      if ((*it)->vnum == vnum)
      vnum = (*it)->vnum + 15;
      if (!land_exists(vnum))
      return vnum;
    }
    return vnum;
  }

  void build_property(int dir, int type, int size, ROOM_INDEX_DATA *start, char *owner, char *propname) {
    ROOM_INDEX_DATA *airmid;
    ROOM_INDEX_DATA *aireast;
    ROOM_INDEX_DATA *airwest;
    ROOM_INDEX_DATA *tunnel;
    ROOM_INDEX_DATA *room;

    int new_n = dir;
    int new_ne = turn_dir[dir];
    int new_e = turn_dir[new_ne];
    int new_se = turn_dir[new_e];
    int new_s = turn_dir[new_se];
    int new_sw = turn_dir[new_s];
    int new_w = turn_dir[new_sw];
    int new_nw = turn_dir[new_w];

    int number = first_free_property();

    int end, begin;

    if (start->exit[DIR_UP] != NULL && start->exit[DIR_UP]->u1.to_room != NULL) {
      airmid = start->exit[DIR_UP]->u1.to_room;

      if (airmid->exit[new_e] != NULL && airmid->exit[new_e]->u1.to_room != NULL) {
        aireast = airmid->exit[new_e]->u1.to_room;
      }
      if (airmid->exit[new_w] != NULL && airmid->exit[new_w]->u1.to_room != NULL) {
        airwest = airmid->exit[new_w]->u1.to_room;
      }
    }
    if (start->exit[DIR_DOWN] != NULL && start->exit[DIR_DOWN]->u1.to_room != NULL) {
      tunnel = start->exit[DIR_DOWN]->u1.to_room;
    }

    if (type == PROP_HOUSE) {
      make_exit(start->vnum, number, new_n, CONNECT_NODOOR);
      room = get_room_index(number);
      REMOVE_BIT(room->room_flags, ROOM_INDOORS);
      REMOVE_BIT(room->room_flags, ROOM_DARK);
      REMOVE_BIT(room->room_flags, ROOM_LIGHTOFF);
      room->sector_type = SECT_PARK;
      free_string(room->name);
      room->name = str_dup(create_address(start, dir));
      number++;
    }
    else {
      room = start;
    }

    if (size == PROP_SMALL) {
      if (type == PROP_HOUSE)
      make_exit(room->vnum, number + 1, new_n, CONNECT_FRONTDOOR);
      else if (type == PROP_SHOP)
      make_exit(room->vnum, number + 1, new_n, CONNECT_SHOPDOOR);

      end = number + 1;
      begin = number;
      if (type == PROP_HOUSE)
      begin--;
      else
      end += 4;
    }
    else if (size == PROP_MEDIUM) {
      if (type == PROP_HOUSE)
      make_exit(room->vnum, number + 1, new_n, CONNECT_FRONTDOOR);
      else if (type == PROP_SHOP)
      make_exit(room->vnum, number + 1, new_n, CONNECT_SHOPDOOR);

      make_exit(number, number + 1, new_e, CONNECT_NODOOR);
      make_exit(number, number + 4, new_ne, CONNECT_NODOOR);
      make_exit(number, number + 3, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 3, new_nw, CONNECT_NODOOR);
      make_exit(number + 1, number + 4, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 5, new_ne, CONNECT_NODOOR);
      make_exit(number + 1, number + 2, new_e, CONNECT_NODOOR);
      make_exit(number + 2, number + 4, new_nw, CONNECT_NODOOR);
      make_exit(number + 2, number + 5, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 6, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 7, new_ne, CONNECT_NODOOR);
      make_exit(number + 3, number + 4, new_e, CONNECT_NODOOR);
      make_exit(number + 4, number + 6, new_nw, CONNECT_NODOOR);
      make_exit(number + 4, number + 7, new_n, CONNECT_NODOOR);
      make_exit(number + 4, number + 8, new_ne, CONNECT_NODOOR);
      make_exit(number + 4, number + 5, new_e, CONNECT_NODOOR);
      make_exit(number + 5, number + 8, new_n, CONNECT_NODOOR);
      make_exit(number + 5, number + 7, new_nw, CONNECT_NODOOR);
      make_exit(number + 6, number + 7, new_e, CONNECT_NODOOR);
      make_exit(number + 7, number + 8, new_e, CONNECT_NODOOR);

      make_exit(number + 4, number + 9, DIR_DOWN, CONNECT_NODOOR);
      make_exit(number + 4, number + 10, DIR_UP, CONNECT_NODOOR);

      get_room_index(number + 9)->sector_type = SECT_BASEMENT;

      end = number + 10;
      begin = number;
      if (type == PROP_HOUSE)
      begin--;
      else
      end += 4;
    }
    else if (size == SIZE_LARGE) {
      if (type == PROP_HOUSE)
      make_exit(room->vnum, number + 1, new_n, CONNECT_FRONTDOOR);
      else if (type == PROP_SHOP)
      make_exit(room->vnum, number + 1, new_n, CONNECT_SHOPDOOR);

      make_exit(number, number + 1, new_e, CONNECT_NODOOR);
      make_exit(number, number + 4, new_ne, CONNECT_NODOOR);
      make_exit(number, number + 3, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 3, new_nw, CONNECT_NODOOR);
      make_exit(number + 1, number + 4, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 5, new_ne, CONNECT_NODOOR);
      make_exit(number + 1, number + 2, new_e, CONNECT_NODOOR);
      make_exit(number + 2, number + 4, new_nw, CONNECT_NODOOR);
      make_exit(number + 2, number + 5, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 6, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 7, new_ne, CONNECT_NODOOR);
      make_exit(number + 3, number + 4, new_e, CONNECT_NODOOR);
      make_exit(number + 4, number + 6, new_nw, CONNECT_NODOOR);
      make_exit(number + 4, number + 7, new_n, CONNECT_NODOOR);
      make_exit(number + 4, number + 8, new_ne, CONNECT_NODOOR);
      make_exit(number + 4, number + 5, new_e, CONNECT_NODOOR);
      make_exit(number + 5, number + 7, new_nw, CONNECT_NODOOR);
      make_exit(number + 5, number + 8, new_n, CONNECT_NODOOR);
      make_exit(number + 6, number + 7, new_e, CONNECT_NODOOR);
      make_exit(number + 7, number + 8, new_e, CONNECT_NODOOR);

      make_exit(number + 10, number + 11, new_e, CONNECT_NODOOR);
      make_exit(number + 10, number + 14, new_ne, CONNECT_NODOOR);
      make_exit(number + 10, number + 13, new_n, CONNECT_NODOOR);
      make_exit(number + 11, number + 13, new_nw, CONNECT_NODOOR);
      make_exit(number + 11, number + 14, new_n, CONNECT_NODOOR);
      make_exit(number + 11, number + 15, new_ne, CONNECT_NODOOR);
      make_exit(number + 11, number + 12, new_e, CONNECT_NODOOR);
      make_exit(number + 12, number + 14, new_nw, CONNECT_NODOOR);
      make_exit(number + 12, number + 15, new_n, CONNECT_NODOOR);
      make_exit(number + 13, number + 16, new_n, CONNECT_NODOOR);
      make_exit(number + 13, number + 17, new_ne, CONNECT_NODOOR);
      make_exit(number + 13, number + 14, new_e, CONNECT_NODOOR);
      make_exit(number + 14, number + 16, new_nw, CONNECT_NODOOR);
      make_exit(number + 14, number + 17, new_n, CONNECT_NODOOR);
      make_exit(number + 14, number + 18, new_ne, CONNECT_NODOOR);
      make_exit(number + 14, number + 15, new_e, CONNECT_NODOOR);
      make_exit(number + 15, number + 17, new_nw, CONNECT_NODOOR);
      make_exit(number + 15, number + 18, new_n, CONNECT_NODOOR);
      make_exit(number + 16, number + 17, new_e, CONNECT_NODOOR);
      make_exit(number + 17, number + 18, new_e, CONNECT_NODOOR);

      make_exit(number, number + 10, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 1, number + 11, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 2, number + 12, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 3, number + 13, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 4, number + 14, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 5, number + 15, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 6, number + 16, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 7, number + 17, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 8, number + 18, DIR_UP, CONNECT_NODOOR);

      make_exit(number + 4, number + 19, DIR_DOWN, CONNECT_NODOOR);
      make_exit(number + 14, number + 20, DIR_UP, CONNECT_NODOOR);

      get_room_index(number + 19)->sector_type = SECT_BASEMENT;

      if (tunnel != NULL)
      make_exit(number + 19, tunnel->vnum, new_s, CONNECT_BASEMENT);
      if (airwest != NULL)
      make_exit(number + 10, airwest->vnum, new_s, CONNECT_WINDOW);
      if (airmid != NULL)
      make_exit(number + 11, airmid->vnum, new_s, CONNECT_WINDOW);
      if (aireast != NULL)
      make_exit(number + 12, aireast->vnum, new_s, CONNECT_WINDOW);

      end = number + 20;
      begin = number;
      if (type == PROP_HOUSE)
      begin--;
      else
      end += 4;
    }
    else if (size == PROP_TALL) {
      if (type == PROP_HOUSE)
      make_exit(room->vnum, number + 1, new_n, CONNECT_FRONTDOOR);
      else if (type == PROP_SHOP)
      make_exit(room->vnum, number + 1, new_n, CONNECT_SHOPDOOR);

      make_exit(number, number + 1, new_e, CONNECT_NODOOR);
      make_exit(number, number + 4, new_ne, CONNECT_NODOOR);
      make_exit(number, number + 3, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 3, new_nw, CONNECT_NODOOR);
      make_exit(number + 1, number + 4, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 5, new_ne, CONNECT_NODOOR);
      make_exit(number + 1, number + 2, new_e, CONNECT_NODOOR);
      make_exit(number + 2, number + 4, new_nw, CONNECT_NODOOR);
      make_exit(number + 2, number + 5, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 6, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 7, new_ne, CONNECT_NODOOR);
      make_exit(number + 3, number + 4, new_e, CONNECT_NODOOR);
      make_exit(number + 4, number + 6, new_nw, CONNECT_NODOOR);
      make_exit(number + 4, number + 7, new_n, CONNECT_NODOOR);
      make_exit(number + 4, number + 8, new_ne, CONNECT_NODOOR);
      make_exit(number + 4, number + 5, new_e, CONNECT_NODOOR);
      make_exit(number + 5, number + 7, new_nw, CONNECT_NODOOR);
      make_exit(number + 5, number + 8, new_n, CONNECT_NODOOR);
      make_exit(number + 6, number + 7, new_e, CONNECT_NODOOR);
      make_exit(number + 7, number + 8, new_e, CONNECT_NODOOR);

      make_exit(number + 10, number + 11, new_e, CONNECT_NODOOR);
      make_exit(number + 10, number + 14, new_ne, CONNECT_NODOOR);
      make_exit(number + 10, number + 13, new_n, CONNECT_NODOOR);
      make_exit(number + 11, number + 13, new_nw, CONNECT_NODOOR);
      make_exit(number + 11, number + 14, new_n, CONNECT_NODOOR);
      make_exit(number + 11, number + 15, new_ne, CONNECT_NODOOR);
      make_exit(number + 11, number + 12, new_e, CONNECT_NODOOR);
      make_exit(number + 12, number + 14, new_nw, CONNECT_NODOOR);
      make_exit(number + 12, number + 15, new_n, CONNECT_NODOOR);
      make_exit(number + 13, number + 16, new_n, CONNECT_NODOOR);
      make_exit(number + 13, number + 17, new_ne, CONNECT_NODOOR);
      make_exit(number + 13, number + 14, new_e, CONNECT_NODOOR);
      make_exit(number + 14, number + 16, new_nw, CONNECT_NODOOR);
      make_exit(number + 14, number + 17, new_n, CONNECT_NODOOR);
      make_exit(number + 14, number + 18, new_ne, CONNECT_NODOOR);
      make_exit(number + 14, number + 15, new_e, CONNECT_NODOOR);
      make_exit(number + 15, number + 17, new_nw, CONNECT_NODOOR);
      make_exit(number + 15, number + 18, new_n, CONNECT_NODOOR);
      make_exit(number + 16, number + 17, new_e, CONNECT_NODOOR);
      make_exit(number + 17, number + 18, new_e, CONNECT_NODOOR);

      make_exit(number, number + 10, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 1, number + 11, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 2, number + 12, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 3, number + 13, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 4, number + 14, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 5, number + 15, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 6, number + 16, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 7, number + 17, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 8, number + 18, DIR_UP, CONNECT_NODOOR);

      make_exit(number + 20, number + 21, new_e, CONNECT_NODOOR);
      make_exit(number + 20, number + 24, new_ne, CONNECT_NODOOR);
      make_exit(number + 20, number + 23, new_n, CONNECT_NODOOR);
      make_exit(number + 21, number + 23, new_nw, CONNECT_NODOOR);
      make_exit(number + 21, number + 24, new_n, CONNECT_NODOOR);
      make_exit(number + 21, number + 25, new_ne, CONNECT_NODOOR);
      make_exit(number + 21, number + 22, new_e, CONNECT_NODOOR);
      make_exit(number + 22, number + 24, new_nw, CONNECT_NODOOR);

      make_exit(number + 22, number + 25, new_n, CONNECT_NODOOR);
      make_exit(number + 23, number + 26, new_n, CONNECT_NODOOR);
      make_exit(number + 23, number + 27, new_ne, CONNECT_NODOOR);
      make_exit(number + 23, number + 24, new_e, CONNECT_NODOOR);
      make_exit(number + 24, number + 26, new_nw, CONNECT_NODOOR);
      make_exit(number + 24, number + 27, new_n, CONNECT_NODOOR);
      make_exit(number + 24, number + 28, new_ne, CONNECT_NODOOR);
      make_exit(number + 24, number + 25, new_e, CONNECT_NODOOR);
      make_exit(number + 25, number + 27, new_nw, CONNECT_NODOOR);
      make_exit(number + 25, number + 28, new_n, CONNECT_NODOOR);
      make_exit(number + 26, number + 27, new_e, CONNECT_NODOOR);
      make_exit(number + 27, number + 28, new_e, CONNECT_NODOOR);
      make_exit(number + 10, number + 20, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 11, number + 21, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 12, number + 22, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 13, number + 23, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 14, number + 24, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 15, number + 25, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 16, number + 26, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 17, number + 27, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 18, number + 28, DIR_UP, CONNECT_NODOOR);

      make_exit(number + 30, number + 31, new_e, CONNECT_NODOOR);
      make_exit(number + 30, number + 34, new_ne, CONNECT_NODOOR);
      make_exit(number + 30, number + 33, new_n, CONNECT_NODOOR);
      make_exit(number + 31, number + 33, new_nw, CONNECT_NODOOR);
      make_exit(number + 31, number + 34, new_n, CONNECT_NODOOR);
      make_exit(number + 31, number + 35, new_ne, CONNECT_NODOOR);
      make_exit(number + 31, number + 32, new_e, CONNECT_NODOOR);
      make_exit(number + 32, number + 34, new_nw, CONNECT_NODOOR);
      make_exit(number + 32, number + 35, new_n, CONNECT_NODOOR);
      make_exit(number + 33, number + 36, new_n, CONNECT_NODOOR);
      make_exit(number + 33, number + 37, new_ne, CONNECT_NODOOR);
      make_exit(number + 33, number + 34, new_e, CONNECT_NODOOR);
      make_exit(number + 34, number + 36, new_nw, CONNECT_NODOOR);
      make_exit(number + 34, number + 37, new_n, CONNECT_NODOOR);
      make_exit(number + 34, number + 38, new_ne, CONNECT_NODOOR);
      make_exit(number + 34, number + 35, new_e, CONNECT_NODOOR);
      make_exit(number + 35, number + 37, new_nw, CONNECT_NODOOR);
      make_exit(number + 35, number + 38, new_n, CONNECT_NODOOR);
      make_exit(number + 36, number + 37, new_e, CONNECT_NODOOR);
      make_exit(number + 37, number + 38, new_e, CONNECT_NODOOR);

      make_exit(number + 20, number + 30, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 21, number + 31, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 22, number + 32, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 23, number + 33, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 24, number + 34, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 25, number + 35, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 26, number + 36, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 27, number + 37, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 28, number + 38, DIR_UP, CONNECT_NODOOR);

      make_exit(number + 4, number + 39, DIR_DOWN, CONNECT_NODOOR);

      get_room_index(number + 39)->sector_type = SECT_BASEMENT;

      if (tunnel != NULL)
      make_exit(number + 39, tunnel->vnum, new_s, CONNECT_BASEMENT);
      if (airwest != NULL)
      make_exit(number + 10, airwest->vnum, new_s, CONNECT_WINDOW);
      if (airmid != NULL)
      make_exit(number + 11, airmid->vnum, new_s, CONNECT_WINDOW);
      if (aireast != NULL)
      make_exit(number + 12, aireast->vnum, new_s, CONNECT_WINDOW);

      if (airwest != NULL)
      make_exit(number + 20, airwest->vnum, new_s, CONNECT_WINDOW);
      if (airmid != NULL)
      make_exit(number + 21, airmid->vnum, new_s, CONNECT_WINDOW);
      if (aireast != NULL)
      make_exit(number + 22, aireast->vnum, new_s, CONNECT_WINDOW);

      if (airwest != NULL)
      make_exit(number + 30, airwest->vnum, new_s, CONNECT_WINDOW);
      if (airmid != NULL)
      make_exit(number + 31, airmid->vnum, new_s, CONNECT_WINDOW);
      if (aireast != NULL)
      make_exit(number + 32, aireast->vnum, new_s, CONNECT_WINDOW);

      end = number + 40;
      begin = number;
      if (type == PROP_HOUSE)
      begin--;
      else
      end += 4;
    }
    else if (size == PROP_XLARGE) {
      if (type == PROP_HOUSE)
      make_exit(room->vnum, number + 2, new_n, CONNECT_FRONTDOOR);
      else if (type == PROP_SHOP)
      make_exit(room->vnum, number + 2, new_n, CONNECT_SHOPDOOR);

      make_exit(number, number + 5, new_n, CONNECT_NODOOR);
      make_exit(number, number + 6, new_ne, CONNECT_NODOOR);
      make_exit(number, number + 1, new_e, CONNECT_NODOOR);
      make_exit(number + 1, number + 5, new_nw, CONNECT_NODOOR);
      make_exit(number + 1, number + 6, new_n, CONNECT_NODOOR);
      make_exit(number + 1, number + 7, new_ne, CONNECT_NODOOR);
      make_exit(number + 1, number + 2, new_e, CONNECT_NODOOR);
      make_exit(number + 2, number + 6, new_nw, CONNECT_NODOOR);
      make_exit(number + 2, number + 7, new_n, CONNECT_NODOOR);
      make_exit(number + 2, number + 8, new_ne, CONNECT_NODOOR);
      make_exit(number + 2, number + 3, new_e, CONNECT_NODOOR);
      make_exit(number + 3, number + 7, new_nw, CONNECT_NODOOR);
      make_exit(number + 3, number + 8, new_n, CONNECT_NODOOR);
      make_exit(number + 3, number + 9, new_ne, CONNECT_NODOOR);
      make_exit(number + 3, number + 4, new_e, CONNECT_NODOOR);
      make_exit(number + 4, number + 8, new_nw, CONNECT_NODOOR);
      make_exit(number + 4, number + 9, new_n, CONNECT_NODOOR);

      make_exit(number + 5, number + 10, new_n, CONNECT_NODOOR);
      make_exit(number + 5, number + 11, new_ne, CONNECT_NODOOR);
      make_exit(number + 5, number + 6, new_e, CONNECT_NODOOR);
      make_exit(number + 6, number + 10, new_nw, CONNECT_NODOOR);
      make_exit(number + 6, number + 11, new_n, CONNECT_NODOOR);
      make_exit(number + 6, number + 12, new_ne, CONNECT_NODOOR);
      make_exit(number + 6, number + 7, new_e, CONNECT_NODOOR);
      make_exit(number + 7, number + 11, new_nw, CONNECT_NODOOR);
      make_exit(number + 7, number + 12, new_n, CONNECT_NODOOR);
      make_exit(number + 7, number + 13, new_ne, CONNECT_NODOOR);
      make_exit(number + 7, number + 8, new_e, CONNECT_NODOOR);
      make_exit(number + 8, number + 12, new_nw, CONNECT_NODOOR);
      make_exit(number + 8, number + 13, new_n, CONNECT_NODOOR);
      make_exit(number + 8, number + 14, new_ne, CONNECT_NODOOR);
      make_exit(number + 8, number + 9, new_e, CONNECT_NODOOR);
      make_exit(number + 9, number + 13, new_nw, CONNECT_NODOOR);
      make_exit(number + 9, number + 14, new_n, CONNECT_NODOOR);

      make_exit(number + 10, number + 15, new_n, CONNECT_NODOOR);
      make_exit(number + 10, number + 16, new_ne, CONNECT_NODOOR);
      make_exit(number + 10, number + 11, new_e, CONNECT_NODOOR);
      make_exit(number + 11, number + 15, new_nw, CONNECT_NODOOR);
      make_exit(number + 11, number + 16, new_n, CONNECT_NODOOR);
      make_exit(number + 11, number + 17, new_ne, CONNECT_NODOOR);
      make_exit(number + 11, number + 12, new_e, CONNECT_NODOOR);
      make_exit(number + 12, number + 16, new_nw, CONNECT_NODOOR);
      make_exit(number + 12, number + 17, new_n, CONNECT_NODOOR);
      make_exit(number + 12, number + 18, new_ne, CONNECT_NODOOR);
      make_exit(number + 12, number + 13, new_e, CONNECT_NODOOR);
      make_exit(number + 13, number + 17, new_nw, CONNECT_NODOOR);
      make_exit(number + 13, number + 18, new_n, CONNECT_NODOOR);
      make_exit(number + 13, number + 19, new_ne, CONNECT_NODOOR);
      make_exit(number + 13, number + 14, new_e, CONNECT_NODOOR);
      make_exit(number + 14, number + 18, new_nw, CONNECT_NODOOR);
      make_exit(number + 14, number + 19, new_n, CONNECT_NODOOR);

      make_exit(number + 15, number + 20, new_n, CONNECT_NODOOR);
      make_exit(number + 15, number + 21, new_ne, CONNECT_NODOOR);
      make_exit(number + 15, number + 16, new_e, CONNECT_NODOOR);
      make_exit(number + 16, number + 20, new_nw, CONNECT_NODOOR);
      make_exit(number + 16, number + 21, new_n, CONNECT_NODOOR);
      make_exit(number + 16, number + 22, new_ne, CONNECT_NODOOR);
      make_exit(number + 16, number + 17, new_e, CONNECT_NODOOR);
      make_exit(number + 17, number + 21, new_nw, CONNECT_NODOOR);
      make_exit(number + 17, number + 22, new_n, CONNECT_NODOOR);
      make_exit(number + 17, number + 23, new_ne, CONNECT_NODOOR);
      make_exit(number + 17, number + 18, new_e, CONNECT_NODOOR);
      make_exit(number + 18, number + 22, new_nw, CONNECT_NODOOR);
      make_exit(number + 18, number + 23, new_n, CONNECT_NODOOR);
      make_exit(number + 18, number + 24, new_ne, CONNECT_NODOOR);
      make_exit(number + 18, number + 19, new_e, CONNECT_NODOOR);
      make_exit(number + 19, number + 23, new_nw, CONNECT_NODOOR);
      make_exit(number + 19, number + 24, new_n, CONNECT_NODOOR);

      make_exit(number + 20, number + 21, new_e, CONNECT_NODOOR);
      make_exit(number + 21, number + 22, new_e, CONNECT_NODOOR);
      make_exit(number + 22, number + 23, new_e, CONNECT_NODOOR);
      make_exit(number + 23, number + 24, new_e, CONNECT_NODOOR);

      make_exit(number + 30, number + 35, new_n, CONNECT_NODOOR);
      make_exit(number + 30, number + 36, new_ne, CONNECT_NODOOR);
      make_exit(number + 30, number + 31, new_e, CONNECT_NODOOR);
      make_exit(number + 31, number + 35, new_nw, CONNECT_NODOOR);
      make_exit(number + 31, number + 36, new_n, CONNECT_NODOOR);
      make_exit(number + 31, number + 37, new_ne, CONNECT_NODOOR);
      make_exit(number + 31, number + 32, new_e, CONNECT_NODOOR);
      make_exit(number + 32, number + 36, new_nw, CONNECT_NODOOR);
      make_exit(number + 32, number + 37, new_n, CONNECT_NODOOR);
      make_exit(number + 32, number + 38, new_ne, CONNECT_NODOOR);
      make_exit(number + 32, number + 33, new_e, CONNECT_NODOOR);
      make_exit(number + 33, number + 37, new_nw, CONNECT_NODOOR);
      make_exit(number + 33, number + 38, new_n, CONNECT_NODOOR);
      make_exit(number + 33, number + 39, new_ne, CONNECT_NODOOR);
      make_exit(number + 33, number + 34, new_e, CONNECT_NODOOR);
      make_exit(number + 34, number + 38, new_nw, CONNECT_NODOOR);
      make_exit(number + 34, number + 39, new_n, CONNECT_NODOOR);

      make_exit(number + 35, number + 40, new_n, CONNECT_NODOOR);
      make_exit(number + 35, number + 41, new_ne, CONNECT_NODOOR);
      make_exit(number + 35, number + 36, new_e, CONNECT_NODOOR);
      make_exit(number + 36, number + 40, new_nw, CONNECT_NODOOR);
      make_exit(number + 36, number + 41, new_n, CONNECT_NODOOR);
      make_exit(number + 36, number + 42, new_ne, CONNECT_NODOOR);
      make_exit(number + 36, number + 37, new_e, CONNECT_NODOOR);
      make_exit(number + 37, number + 41, new_nw, CONNECT_NODOOR);
      make_exit(number + 37, number + 42, new_n, CONNECT_NODOOR);
      make_exit(number + 37, number + 43, new_ne, CONNECT_NODOOR);
      make_exit(number + 37, number + 38, new_e, CONNECT_NODOOR);
      make_exit(number + 38, number + 42, new_nw, CONNECT_NODOOR);
      make_exit(number + 38, number + 43, new_n, CONNECT_NODOOR);
      make_exit(number + 38, number + 44, new_ne, CONNECT_NODOOR);
      make_exit(number + 38, number + 39, new_e, CONNECT_NODOOR);
      make_exit(number + 39, number + 43, new_nw, CONNECT_NODOOR);
      make_exit(number + 39, number + 44, new_n, CONNECT_NODOOR);

      make_exit(number + 40, number + 45, new_n, CONNECT_NODOOR);
      make_exit(number + 40, number + 46, new_ne, CONNECT_NODOOR);
      make_exit(number + 40, number + 41, new_e, CONNECT_NODOOR);
      make_exit(number + 41, number + 45, new_nw, CONNECT_NODOOR);
      make_exit(number + 41, number + 46, new_n, CONNECT_NODOOR);
      make_exit(number + 41, number + 47, new_ne, CONNECT_NODOOR);
      make_exit(number + 41, number + 42, new_e, CONNECT_NODOOR);
      make_exit(number + 42, number + 46, new_nw, CONNECT_NODOOR);
      make_exit(number + 42, number + 47, new_n, CONNECT_NODOOR);
      make_exit(number + 42, number + 48, new_ne, CONNECT_NODOOR);
      make_exit(number + 42, number + 43, new_e, CONNECT_NODOOR);
      make_exit(number + 43, number + 47, new_nw, CONNECT_NODOOR);
      make_exit(number + 43, number + 48, new_n, CONNECT_NODOOR);
      make_exit(number + 43, number + 49, new_ne, CONNECT_NODOOR);
      make_exit(number + 43, number + 44, new_e, CONNECT_NODOOR);
      make_exit(number + 44, number + 48, new_nw, CONNECT_NODOOR);
      make_exit(number + 44, number + 49, new_n, CONNECT_NODOOR);

      make_exit(number + 45, number + 50, new_n, CONNECT_NODOOR);
      make_exit(number + 45, number + 51, new_ne, CONNECT_NODOOR);
      make_exit(number + 45, number + 46, new_e, CONNECT_NODOOR);
      make_exit(number + 46, number + 50, new_nw, CONNECT_NODOOR);
      make_exit(number + 46, number + 51, new_n, CONNECT_NODOOR);
      make_exit(number + 46, number + 52, new_ne, CONNECT_NODOOR);
      make_exit(number + 46, number + 47, new_e, CONNECT_NODOOR);
      make_exit(number + 47, number + 51, new_nw, CONNECT_NODOOR);
      make_exit(number + 47, number + 52, new_n, CONNECT_NODOOR);
      make_exit(number + 47, number + 53, new_ne, CONNECT_NODOOR);
      make_exit(number + 47, number + 48, new_e, CONNECT_NODOOR);
      make_exit(number + 48, number + 52, new_nw, CONNECT_NODOOR);
      make_exit(number + 48, number + 53, new_n, CONNECT_NODOOR);
      make_exit(number + 48, number + 54, new_ne, CONNECT_NODOOR);
      make_exit(number + 48, number + 49, new_e, CONNECT_NODOOR);
      make_exit(number + 49, number + 53, new_nw, CONNECT_NODOOR);
      make_exit(number + 49, number + 54, new_n, CONNECT_NODOOR);

      make_exit(number + 50, number + 51, new_e, CONNECT_NODOOR);
      make_exit(number + 51, number + 52, new_e, CONNECT_NODOOR);
      make_exit(number + 52, number + 53, new_e, CONNECT_NODOOR);
      make_exit(number + 53, number + 54, new_e, CONNECT_NODOOR);

      make_exit(number, number + 30, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 1, number + 31, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 2, number + 32, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 3, number + 33, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 4, number + 34, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 5, number + 35, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 6, number + 36, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 7, number + 37, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 8, number + 38, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 9, number + 39, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 10, number + 40, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 11, number + 41, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 12, number + 42, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 13, number + 43, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 14, number + 44, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 15, number + 45, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 16, number + 46, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 17, number + 47, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 18, number + 48, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 19, number + 49, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 20, number + 50, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 21, number + 51, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 22, number + 52, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 23, number + 53, DIR_UP, CONNECT_NODOOR);
      make_exit(number + 24, number + 54, DIR_UP, CONNECT_NODOOR);

      make_exit(number + 7, number + 55, DIR_DOWN, CONNECT_NODOOR);
      make_exit(number + 42, number + 56, DIR_UP, CONNECT_NODOOR);

      get_room_index(number + 55)->sector_type = SECT_BASEMENT;

      if (tunnel != NULL)
      make_exit(number + 55, tunnel->vnum, new_s, CONNECT_BASEMENT);
      if (airwest != NULL)
      make_exit(number + 31, airwest->vnum, new_s, CONNECT_WINDOW);
      if (airmid != NULL)
      make_exit(number + 32, airmid->vnum, new_s, CONNECT_WINDOW);
      if (aireast != NULL)
      make_exit(number + 33, aireast->vnum, new_s, CONNECT_WINDOW);
      if (airwest != NULL)
      make_exit(number + 30, airwest->vnum, new_s, CONNECT_WINDOW);
      if (aireast != NULL)
      make_exit(number + 34, aireast->vnum, new_s, CONNECT_WINDOW);

      end = number + 57;
      begin = number;
      if (type == PROP_HOUSE)
      begin--;
      else
      end += 4;
    }

    PROP_TYPE *prop;
    prop = new_prop();
    free_string(prop->owner);
    prop->owner = str_dup(owner);
    prop->type = type;
    if (type == PROP_HOUSE || safe_strlen(propname) < 2) {
      free_string(prop->propname);
      prop->propname = str_dup(create_address(start, dir));
    }
    else {
      free_string(prop->propname);
      prop->propname = str_dup(propname);
    }
    free_string(prop->address);
    prop->address = str_dup(create_address(start, dir));
    prop->lower_vnum = begin;
    prop->higher_vnum = end;
    prop->price = get_price(start, size);

    if (get_day() > 1)
    prop->auction_day = get_day() - 1;
    else
    prop->auction_day = 28;

    // Auction month never seems to get set - Discordance
    if (get_month() < 11) {
      prop->auction_month = get_month() + 1;
    }
    else {
      prop->auction_month = 0;
    }

    if (type == PROP_HOUSE)
    prop->vnum = next_prop_hvnum();
    else
    prop->vnum = next_prop_svnum();

    PropVect.push_back(prop);

    if (type == PROP_SHOP) {
      for (int i = begin; i <= end; i++) {
        if (get_room_index(i) != NULL) {
          get_room_index(i)->sector_type = SECT_COMMERCIAL;
          REMOVE_BIT(get_room_index(i)->room_flags, ROOM_LIGHTOFF);
          REMOVE_BIT(get_room_index(i)->room_flags, ROOM_DARK);
          SET_BIT(get_room_index(i)->room_flags, ROOM_PUBLIC);
        }
      }
    }

    save_properties(FALSE);
  }

  /* Planning section  - Discordance
* Notes: Considered alternate formats for each size, but affect on face of
* street undesirable.  If individual wants to dominant street access, can
* purchase larger lot to do so.
*
* General rules:
*      Walls can not be set on border of property
*      Rooms without four walls can not be set as indoor unless they are
* bordered in open directions by indoor rooms Rooms which are connected to
* indoor rooms must be separated by doors or walls on applicable sides to be
* set as outdoor All land comes with one level of airspace above it. Rooms must
* contain an exit with at least one wall in order to set a ceiling Rooms with
* ceilings block weather affects, but use outdoor temperature Indoor rooms use
* house temperature; outdoor rooms use outdoor temperature Weather is visible
* from rooms that are outdoor or indoor rooms that possess see-through exits
* such as a window Weather affects rooms that have up exits to air rooms
*      Buildings may occupy no more than half of the plot size in rooms
*      Building sizes are limited first by plot size and then by zone.  Thus a
* small house may be three floors, but may be limited to two in certain zones.
*      Outdoor rooms may be merged with other outdoor rooms to conserve space
* provided that they are adjacent Outdoor rooms that have ceilings must have
* their ceilings removed prior to being merged Commercial property may have
* stockrooms equal to 3/4 of the total indoor rooms, rounded down Each
* stockroom provides the basis for how many of each item the shop can sell
* before restocking. Stockrooms must be indoor Stockrooms may be merged in the
* same way as outdoor rooms. Commercial property may have floor rooms equal to
* the total number of indoor rooms Each floor room provides the basis for how
* many different items the shop can sell Floor rooms must be indoor Floor rooms
* may be merged in the same way as outdoor and stockrooms. Air rooms above
* merged rooms must also be merged based on the room below Merged rooms use the
* description and exits of occupied room.  Places/extras are imported and added
* to existing places/extras. Rooms may only be merged if they are equal to the
* standard size of a room. Rooms may only be divided if they are greater than
* the standard size of a room. Rooms which have been merged, must be divided
* prior to changing their stockroom, floor room, or outdoor flags. Stock rooms, * floor rooms, and outdoor rooms are mutually exclusive states. Commercial
* properties may purchase off-grid storage i.e. warehouses to mitigate delivery
* times
*
*  In real life, land is finite and plot sizes aren't determined on a whim. You
* can not, for example, purchase a plot on a city block or housing development
* which is significantly larger than another plot on that same block.  They
* will be more or less the same.  Typically when they do vary, they vary by
* width and not by depth.  Housing on the plot may differ somewhat, mostly in
* how many bedrooms and bathrooms a building has which increases the livability
* of the structure and not necessarily the space. Families will almost always
* choose houses that have the needed number of bedrooms regardless of house
*  size.  A three bedroom house is not necessarily bigger than a two bedroom
* house, for example.  It can be, but the three bedroom house is more likely
* adding additional floors, sacrificing space, or both.
*
*  Houses are rarely more than two floors.  The majority of residences should
* be two floors with or without a basement.  Houses that are in odd locations
* like in valleys or beside a river might employ different constructions like
* having the garage underneath the house to avoid flooding and which gives the
* house the impression of being three floors, but the house still only has two
* floors that are livable.  Likewise some houses have an entryway that is
* between two floors, one higher, and one lower, but the level of the entry way
* is not a floor in itself, but just an in between method of reaching either
* floor by a flight of stairs.  These structures usually lack a basement.
*
*  Therefore, two livable floors is ideal with the optional inclusion of a
* infrastructure level whether that be a basement, garage, or entryway.
*
* This requires a system of dividing existing areas into subareas or doing so
* within the current framework. Since redoing areas seems tedious and it also
* seems like it'd still be nice to be able to group similar areas under a
* parent area, a subarea system seems best.  Subareas will need to be defined
* by a vnum range.  They will need to keep track of a zoning variable
* determining what type of structure can be built and how much of the land can
* be dedicated to said structure.  They will also need to limit the size of
* plots.
*
* Limiting the size of plots can be done either by saying that a specific zone
* only has a certain size plot available, but I think this is somewhat
* inflexible and uninteresting. It doesn't account for many situations where a
* block is not completely made up of exactly the same type of structure.  As
* such, zones should have a finite type of each building.  This should be
* customizable by builders, but also have a default distribution so as not to
* burden the builder with unnecessary design considerations.  It also means
* they have an example to fall back on and don't inadvertently create something
* that is out of touch with the format found elsewhere on the grid.
*
* The city should be divided up into:
*
* Downtown
*      Parking is either on the street or behind buildings.  Commercial
* properties are separated by narrow alleys if at all typically.  Building
* height is determined by the size and flavor of the city.  Higher than average
*      buildings are more reasonable in a downtown area.  No one lives in the
* downtown area unless it's on the second floor and usually on a side street.
*
*      Default limits: 3 floors, two basement, small plots, residential limited
*
* Residential blocks
*      Parking is in both street and driveway though nicer or traffic congested
* locations prohibit parking in the street during peak hours or possibly over
* night.  Garages are attached in medium to high quality neighborhoods. Garages
*      are detached in rural or medium to poor quality neighborhoods (also in
* older homes, but older homes are prone to being located in neighborhoods that
* have had their quality decline).  Garages are not present in some more rural
*      areas, but also in the poorest neighborhoods.
*
*      Default limits: 2 floors, one basement, medium plots, no commercial
*
* Commercial streets
*      Are often just on the outer edges of residential blocks and are
* typically lined with businesses.  Few houses should be present on main roads
* if any.  Typically houses being present on a commercial street means that
* it's underdeveloped and gives that impression.  Living on a commercial street
* is unpleasant because of noise, traffic, and aesthetic considerations.
* Security may also be an issue.
*
*      Housing may be present on commercial streets otherwise, but will
* typically be on the second floor of businesses.  The owner of the property
* will rent out the commercial unit separately than the upstairs unit or units
* typically.  Less commonly, a house may have been converted into a store by
* its owner who still lives there, but usually at the cost of living space.
* They don't simply build a house onto top of store.  Some apartment buildings
* are set up in that manner, however.  A town of Haven's size won't have that
* issue.
*
*      Smaller plots will add a disincentive to placing housing on commercial
* streets and reflect that such housing is usually substandard.
*
*      Default limits: 2 floors, no basement, small plots
*
* Commercial blocks
*      All bets are off.  Stores do what they want with these.  Typically they
* favor large, one floor structures on huge lots with parking to spare in front
* of the businesses.  A business might own a whole block or they might share it
* and have a little strip mall or collection of buildings.  These are likely to
* be accessible from a commercial street, but will not share the block with
* residential properties.
*
*      Default limits: Complicated.  Need to work out exchange rate between
* plot sizes to determine how much room left over after one size is purchased.
* No residences.  Buildings should take up half of the plot size or less
* because of parking.
*
*
* Sprawl and rural housing
*      These houses are less standardized and their plots can vary a lot
* because the land behind them tends not to have been developed and may be
* owned by the city or a private individual or business that simply has not
* desire at the moment to develop it.  Commercial and residences often share a
* street, but are spaced so that there's a clear distinction and separation.
*
*      Default limits: No small plots, buildings should take up half of the
* plot size or less because of population density.
*
* City needs mapped out in more detail.  Additional streets and blocks should
* be created dynamically as needed based on how many houses on street.  Street
* formation should be delayed and done in phases so that the grid doesn't
* expand too rapidly or unrealistically.
*
* Blocks should have a finite amount of rooms so that homes can be linked
* together automatically.
*/

  /*
//Determines how to adjust the room number depending on the direction
int positioner (int dir, int room_number) {
if(dir = DIR_NORTH){
room_number=room_number+100;
}
else if (dir = DIR_EAST) {
room_number=room_number-1;
}
else if (dir = DIR_SOUTH) {
room_number=room_number-100;
}
else if (dir = DIR_WEST) {
room_number=room_number+1;
}
else if (dir = DIR_NORTHWEST) {
room_number=room_number+101;
}
else if (dir = DIR_NORTHEAST) {
room_number=room_number+100-1;
}
else if (dir = DIR_SOUTHWEST) {
room_number=room_number-100+1;
}
else if (dir = DIR_SOUTHEAST) {
room_number=room_number-101;
}
else if (dir = DIR_UP) {
if (room_number > 56000 && room_number < 96001) {
room_number=room_number+10000;
}
else if (room_number > 96000 && room_number < 106001) {
room_number=room_number - 40000;
}
else if (room_number >106000 && room_number < 116001) {
room_number=room_number - 10000;
}
else if (room_number > 116000 && room_number < 156001) {
room_number=room_number + 10000;
}
else if (room_number > 156000 && room_number < 157001) {
room_number=room_number - 10000;
}
}

return room_number;
}

// checks to see if rooms are already used
bool survey(int size, ROOM_INDEX_DATA *starting_room) {
if(size == PROP_SMALL) {

}
}

// build_property(direction, size, ch->in_room, ch->name);
void new_build_property(int dir, int size, ROOM_INDEX_DATA *starting_room, char * owner) { ROOM_INDEX_DATA *airmid; ROOM_INDEX_DATA *aireast;
ROOM_INDEX_DATA *airwest;
ROOM_INDEX_DATA *tunnel;
ROOM_INDEX_DATA *room;

int new_n = dir;
int new_ne = turn_dir[dir];
int new_e = turn_dir[new_ne];
int new_se = turn_dir[new_e];
int new_s = turn_dir[new_se];
int new_sw = turn_dir[new_s];
int new_w = turn_dir[new_sw];
int new_nw = turn_dir[new_w];

//There's a 40000 room difference between the new streets area and the new
private property area int plot_room_number=starting_room+40000; int
border_room_number=starting_room->vnum;

//feel around border by checking for null exits
//null exit means there is space there
//double check for rooms, if they exist, they should have no exits if they're
available
//biggest problem is determining what areas border rooms are in

// 3x3 plot of land with exits connecting to each existing and adjacent room
//3
//2
//1
//  1 2 3
if(size == PROP_SMALL) {
//Coord: 2,1
plot_room_number=positioner(new_n, plot_room_number);
make_exit(border_room_number, plot_room_number, new_n, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_n, plot_room_number), new_n, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_e, plot_room_number), new_e, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_w, plot_room_number), new_w, CONNECT_NODOOR);
make_exit(plot_room_number, positioner(new_nw, plot_room_number), new_nw, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_ne, plot_room_number), new_ne, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_w, border_room_number), new_sw, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_e, border_room_number), new_se, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 1,1
plot_room_number=positioner(new_w, plot_room_number);
border_room_number=positioner(new_w, border_room_number);
make_exit(border_room_number, plot_room_number, new_n, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_n, plot_room_number), new_n, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_ne, plot_room_number), new_ne, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_w, border_room_number), new_sw, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_e, border_room_number), new_se, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(new_nw, border_room_number), new_w, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_n, positioner(new_nw, border_room_number)), new_nw, CONNECT_NODOOR); //border
room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 1,2
plot_room_number=positioner(new_n, plot_room_number);
border_room_number=positioner(new_n, positioner(new_nw, border_room_number));
make_exit(border_room_number, plot_room_number, new_e, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_n, plot_room_number), new_n, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_e, plot_room_number), new_e, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_ne, plot_room_number), new_ne, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_s, border_room_number), new_sw, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_n, border_room_number), new_nw, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 1,3
plot_room_number=positioner(new_n, plot_room_number);
border_room_number=positioner(new_n, border_room_number));
make_exit(border_room_number, plot_room_number, new_e, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_e, plot_room_number), new_e, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_se, plot_room_number), new_se, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_ne, border_room_number), new_n, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_s, border_room_number), new_sw, CONNECT_NODOOR); //border room
make_exit(plot_room_number, positioner(new_n, border_room_number), new_nw, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_e, positioner(new_ne, border_room_number)), new_ne, CONNECT_NODOOR); //border
room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 2,3
plot_room_number=positioner(new_e, plot_room_number);
border_room_number=positioner(new_e, positioner(new_ne, border_room_number));
make_exit(border_room_number, plot_room_number, new_s, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_e, plot_room_number), new_e, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_se, plot_room_number), new_se, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_w, border_room_number), new_nw, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_e, border_room_number), new_ne, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 3,3
plot_room_number=positioner(new_e, plot_room_number);
border_room_number=positioner(new_e, border_room_number);
make_exit(border_room_number, plot_room_number, new_s, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_sw, plot_room_number), new_sw, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_s, plot_room_number), new_s, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_w, border_room_number), new_nw, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_e, border_room_number), new_ne, CONNECT_NODOOR); //border room
make_exit(plot_room_number, positioner(new_se, border_room_number), new_e, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_s, positioner(new_se, border_room_number), new_se, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 3,2
plot_room_number=positioner(new_s, plot_room_number);
border_room_number=positioner(new_s, positioner(new_se, border_room_number));
make_exit(border_room_number, plot_room_number, new_w, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_w, plot_room_number), new_w, CONNECT_NODOOR); make_exit(plot_room_number, positioner(new_s, plot_room_number), new_s, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_n, border_room_number), new_ne, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_s, border_room_number), new_se, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 3,1
plot_room_number=positioner(new_s, plot_room_number);
border_room_number=positioner(new_s, border_room_number));
make_exit(border_room_number, plot_room_number, new_w, CONNECT_NODOOR);
//border room

make_exit(plot_room_number, positioner(new_nw, plot_room_number), new_nw, CONNECT_NODOOR);

make_exit(plot_room_number, positioner(new_n, border_room_number), new_ne, CONNECT_NODOOR); //border room make_exit(plot_room_number, positioner(new_s, border_room_number), new_se, CONNECT_NODOOR); //border room

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);

//Coord: 2,2
plot_room_number=positioner(new_nw, plot_room_number);

make_exit(plot_room_number, positioner(DIR_UP, plot_room_number), DIR_UP, CONNECT_NODOOR);
}
else if(size == PROP_MEDIUM) {

}
else if(size == SIZE_LARGE) {

}

PROP_TYPE *prop;
prop = new_prop();

//Free old prop fields
if (prop->owner[0] != '\0' && safe_strlen(prop->owner)) > 2 {
free_string(prop->owner);
}
if (prop->propname[0] != '\0' && safe_strlen(prop->propname)) > 2 {
free_string(prop->propname);
}
if (prop->address[0] != '\0' && safe_strlen(prop->address)) > 2 {
free_string(prop->address);
}

//set new prop fields
prop->owner = str_dup(owner);
prop->type = PROP_HOUSE;
prop->address = str_dup(new_create_address(starting_room, dir));
prop-name = str_dup(new_create_address(starting_room, dir));
prop->lower_vnum = positioner(new_n, starting_room+40000);
prop->higher_vnum = 0; //Unnecessary because rooms aren't consecutive
prop->price = get_price(starting_room, size);

if(get_day() > 1) {
prop->auction_day = get_day()-1;
}
else {
prop->auction_day = 28;
}

//Auction month never seems to get set - Discordance
if(get_day() >= prop->auction_day) {
if (get_month() < 11){
prop->auction_month = get_month()+1;
}
else {
prop->auction_month = 0;
}
}
else {
prop->auction_month = get_month();
}

prop->vnum = next_prop_hvnum();
PropVect.push_back(prop);

save_properties(FALSE);
}

*/
  bool buyable_land(ROOM_INDEX_DATA *room) {
    if (room->sector_type == SECT_STREET)
    return FALSE;
    if (room->sector_type == SECT_ALLEY)
    return FALSE;
    if (room->sector_type == SECT_WATER)
    return FALSE;
    if (room->sector_type == SECT_UNDERWATER)
    return FALSE;
    if (room->sector_type == SECT_BEACH)
    return FALSE;
    if (room->sector_type == SECT_ROCKY)
    return FALSE;

    if (room->area->vnum == 20)
    return FALSE;

    if (owned_room(room))
    return FALSE;

    return TRUE;
  }

  bool check_buyability(ROOM_INDEX_DATA *room, int direction) {

    if (room->exit[direction] == NULL)
    return FALSE;

    if (room->exit[direction]->u1.to_room->exit[direction] == NULL)
    return FALSE;

    ROOM_INDEX_DATA *croom =
    room->exit[direction]->u1.to_room->exit[direction]->u1.to_room;

    if (!buyable_land(croom))
    return FALSE;

    int pointer = direction;
    for (int i = 0; i < 10; i++) {
      if (croom->exit[pointer] == NULL)
      return FALSE;
      if (!buyable_land(croom->exit[pointer]->u1.to_room))
      return FALSE;

      pointer = turn_dir[pointer];
    }

    return TRUE;
  }

  // makes sure there aren't gaps between properties - Discordance
  bool plot_alignment(ROOM_INDEX_DATA *room, int direction) {

    return plot_alignment_new(room, direction);

    ROOM_INDEX_DATA *destination_RID;
    int destination_Vnum;
    int coord = get_coord(room->vnum);
    int coord_modifier;
    int direction_modifier;

    if (direction == DIR_NORTH) {
      coord_modifier = 2;
    }
    else if (direction == DIR_EAST) {
      coord_modifier = -2000;
    }
    else if (direction == DIR_SOUTH) {
      coord_modifier = -2;
    }
    else if (direction == DIR_WEST) {
      coord_modifier = 2000;
    }
    else if (direction == DIR_NORTHWEST) {
      coord_modifier = 2000 + 2;
    }
    else if (direction == DIR_NORTHEAST) {
      coord_modifier = -2000 + 2;
    }
    else if (direction == DIR_SOUTHEAST) {
      coord_modifier = -2000 - 2;
    }
    else if (direction == DIR_SOUTHWEST) {
      coord_modifier = 2000 - 2;
    }

    // private prop
    destination_Vnum = coord + coord_modifier + HavenPrivateProperty;
    // government
    destination_Vnum = coord + coord_modifier +
    HavenCityBuildings; // These statements don't do anything.
    // streets
    destination_Vnum =
    coord + coord_modifier + HavenStreets - HavenStreetsModifier;

    if (direction != DIR_SOUTH) {
      // looking north
      direction_modifier = 2;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_NORTH) {
      // looking south
      direction_modifier = -2;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_WEST) {
      // looking east
      direction_modifier = -2000;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_EAST) {
      // looking west
      direction_modifier = +2000;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_SOUTH && direction != DIR_EAST) {
      // looking northwest
      direction_modifier = 2 + 2000;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_SOUTH && direction != DIR_WEST) {
      // looking northeast
      direction_modifier = 2 - 2000;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_NORTH && direction != DIR_EAST) {
      // looking southwest
      direction_modifier = -2 + 2000;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    if (direction != DIR_NORTH && direction != DIR_WEST) {
      // looking southeast
      direction_modifier = -2 - 2000;
      destination_Vnum = coord + coord_modifier + HavenStreets -
      HavenStreetsModifier + direction_modifier;
      if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
        return TRUE;
      }
      else {
        destination_Vnum =
        coord + coord_modifier + HavenCityBuildings + direction_modifier;
        if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
          return TRUE;
        }
        else {
          destination_Vnum =
          coord + coord_modifier + HavenPrivateProperty + direction_modifier;
          if ((destination_RID = get_room_index(destination_Vnum)) != NULL) {
            return TRUE;
          }
        }
      }
    }

    return FALSE;
  }

  bool valid_edge(int x, int y) {
    int vnum = get_coord_vnum(x, y, 5);

    if (get_room_index(vnum + HavenStreets - HavenStreetsModifier) != NULL)
    return TRUE;

    if (get_room_index(vnum + HavenCityBuildings) != NULL)
    return TRUE;

    if (get_room_index(vnum + HavenPrivateProperty) != NULL)
    return TRUE;

    if (get_room_index(vnum + HavenWoods - HavenWoodsModifier) != NULL) {
      if (y > 545 && y < 550 && x % 6 == 0)
      return TRUE;
      if (x > 550 && x < 555 && y % 6 == 0)
      return TRUE;
    }

    return FALSE;
  }

  bool plot_alignment_new(ROOM_INDEX_DATA *room, int direction) {
    int startx = get_roomx(room);
    int starty = get_roomy(room);
    int ymod = 0;
    int xmod = 0;

    switch (direction) {
    case DIR_NORTH:
      ymod += 2;
      break;
    case DIR_NORTHEAST:
      ymod += 2;
      xmod -= 2;
      break;
    case DIR_EAST:
      xmod -= 2;
      break;
    case DIR_SOUTHEAST:
      xmod -= 2;
      ymod -= 2;
      break;
    case DIR_SOUTH:
      ymod -= 2;
      break;
    case DIR_SOUTHWEST:
      ymod -= 2;
      xmod += 2;
      break;
    case DIR_WEST:
      xmod += 2;
      break;
    case DIR_NORTHWEST:
      xmod += 2;
      ymod += 2;
      break;
    }
    int validedges = 0;
    int centerx = startx + xmod;
    int centery = starty + ymod;

    if (valid_edge(centerx, centery + 2)) // Northern edge.
    validedges++;

    if (valid_edge(centerx, centery - 2)) // Southern edge.
    validedges++;

    if (valid_edge(centerx + 2, centery)) // Western edge.
    validedges++;

    if (valid_edge(centerx - 2, centery)) // Eastern edge.
    validedges++;

    if (validedges >= 2)
    return TRUE;

    return FALSE;
  }

  void buynewland(CHAR_DATA *ch, char *argument) {

    if (ch->in_room == NULL)
    return;

    if (ch->in_room->sector_type != SECT_STREET) {
      send_to_char("You have to be standing on a street to do that.\n\r", ch);
      return;
    }
    if (safe_strlen(ch->in_room->name) < 3) {
      send_to_char("This street doesn't seem to have a name.\n\r", ch);
      return;
    }
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg2);
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg3);
    char arg4[MSL];
    argument = one_argument_nouncap(argument, arg4);
    int dir = 0;
    int type = 0;
    int subtype = 0;
    int xlength = 0;
    int ylength = 0;
    if (!str_cmp(arg1, "north"))
    dir = DIR_NORTH;
    else if (!str_cmp(arg1, "south"))
    dir = DIR_SOUTH;
    else if (!str_cmp(arg1, "west"))
    dir = DIR_WEST;
    else if (!str_cmp(arg1, "east"))
    dir = DIR_EAST;
    else {
      send_to_char("Syntax: develop buyland (north/east/south/west) (house/shop) (east-west length) (north-south length)\n\r", ch);
      return;
    }
    if (ch->in_room->area->vnum == INNER_NORTH_FOREST || ch->in_room->area->vnum == INNER_SOUTH_FOREST || ch->in_room->area->vnum == INNER_WEST_FOREST) {
      subtype = PROPERTY_INNERFOREST;
    }
    else {
      send_to_char("You can only buy land in the inner forest.\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "shop"))
    type = PROP_SHOP;
    else if (!str_cmp(arg2, "house"))
    type = PROP_HOUSE;
    else {
      send_to_char("Syntax: develop buyland (north/east/south/west) (house/shop) (east-west length) (north-south length)\n\r", ch);
      return;
    }
    if (ch->in_room->area->vnum == INNER_NORTH_FOREST || ch->in_room->area->vnum == INNER_SOUTH_FOREST || ch->in_room->area->vnum == INNER_WEST_FOREST) {
      subtype = PROPERTY_INNERFOREST;
    }
    else {
      send_to_char("You can only buy land in the inner forest.\n\r", ch);
      return;
    }
    if (!str_cmp(arg2, "shop"))
    type = PROP_SHOP;
    else if (!str_cmp(arg2, "house"))
    type = PROP_HOUSE;
    else {
      send_to_char("Syntax: develop buyland (north/east/south/west) (house/shop) (east-west length) (north-south length)\n\r", ch);
      return;
    }
    xlength = atoi(arg3);
    ylength = atoi(arg4);

    if (xlength < 2 || xlength > 10 || ylength < 2 || ylength > 10) {
      send_to_char("Properties can't be bigger than 10 rooms across, nor smaller than 3.\n\r", ch);
      return;
    }
    if ((dir == DIR_NORTH || dir == DIR_SOUTH) && xlength % 2 == 0) {
      send_to_char("West-East length must be an odd number for this property.\n\r", ch);
      return;
    }
    if ((dir == DIR_EAST || dir == DIR_WEST) && ylength % 2 == 0) {
      send_to_char("North-South length must be an odd number for this property.\n\r", ch);
      return;
    }
    ROOM_INDEX_DATA *firstroom;
    if (ch->in_room->exit[dir] == NULL || ch->in_room->exit[dir]->u1.to_room == NULL) {
      send_to_char("There's no land in that direction.\n\r", ch);
      return;
    }
    firstroom = ch->in_room->exit[dir]->u1.to_room;
    ROOM_INDEX_DATA *testroom;
    int minx, miny, maxx, maxy;

    if (dir == DIR_NORTH) {
      minx = ch->in_room->x - (xlength - 1) / 2;
      miny = ch->in_room->y + 1;
      maxx = ch->in_room->x + (xlength - 1) / 2;
      maxy = ch->in_room->y + ylength;
    }
    if (dir == DIR_SOUTH) {
      minx = ch->in_room->x - (xlength - 1) / 2;
      miny = ch->in_room->y - ylength;
      maxx = ch->in_room->x + (xlength - 1) / 2;
      maxy = ch->in_room->y - 1;
    }
    if (dir == DIR_EAST) {
      minx = ch->in_room->x + 1;
      miny = ch->in_room->y - (ylength - 1) / 2;
      maxx = ch->in_room->x + xlength;
      maxy = ch->in_room->y + (ylength - 1) / 2;
    }
    if (dir == DIR_WEST) {
      minx = ch->in_room->x - xlength;
      miny = ch->in_room->y - (ylength - 1) / 2;
      maxx = ch->in_room->x - 1;
      maxy = ch->in_room->y + (ylength - 1) / 2;
    }

    for (int tx = minx; tx <= maxx; tx++) {
      for (int ty = miny; ty <= maxy; ty++) {
        testroom = sourced_room_by_coordinates(firstroom, tx, ty, 0, TRUE);
        if (testroom == NULL) {
          send_to_char("There's unpurchasable land in that parcel.\n\r", ch);
          return;
        }
        if (testroom->sector_type == SECT_STREET || prop_from_room(testroom) != NULL) {
          send_to_char("There's unpurchasable land in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_NORTH_FOREST && testroom->y <= ch->in_room->area->miny + 3) {
          send_to_char("There's land too close to town in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_NORTH_FOREST && testroom->y >= ch->in_room->area->maxy - 1) {
          send_to_char("There's land too close to the outer forest in that parcel.\n\r", ch);
          return;
        }
        if ((ch->in_room->area->vnum == INNER_NORTH_FOREST || ch->in_room->area->vnum == INNER_SOUTH_FOREST) && testroom->x > ch->in_room->area->maxx) {
          send_to_char("There's land too close to the ocean in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_SOUTH_FOREST && testroom->y >= ch->in_room->area->maxy - 3) {
          send_to_char("There's land too close to town in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_SOUTH_FOREST && testroom->y <= ch->in_room->area->miny + 1) {
          send_to_char("There's land too close to the outer forest in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_NORTH_FOREST && testroom->y <= ch->in_room->area->miny + 3) {
          send_to_char("There's land too close to town in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_WEST_FOREST && testroom->y >= ch->in_room->area->maxy - 1) {
          send_to_char("There's land too close to the outer forest in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_WEST_FOREST && testroom->y <= ch->in_room->area->miny + 1) {
          send_to_char("There's land too close to the outer forest in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_WEST_FOREST && testroom->x >= ch->in_room->area->maxx - 3) {
          send_to_char("There's land too close to the town in that parcel.\n\r", ch);
          return;
        }
        if (ch->in_room->area->vnum == INNER_WEST_FOREST && testroom->x <= ch->in_room->area->minx + 1) {
          send_to_char("There's land too close to the outer forest in that parcel.\n\r", ch);
          return;
        }
      }
    }
    int cost = xlength * ylength * FORESTLAND_PRICE;
    if (ch->pcdata->total_money < cost * 100) {
      printf_to_char(ch, "You'd need $%d in the bank to do that.\n\r", cost);
      return;
    }
    if (dir == DIR_NORTH || dir == DIR_SOUTH) {
      if (xlength % 2 == 0)
      return;
    }
    if (dir == DIR_EAST || dir == DIR_WEST) {
      if (ylength % 2 == 0)
      return;
    }

    make_property(ch->in_room, dir, xlength, ylength, type, subtype, "");
    PROP_TYPE *prop = prop_from_room(ch->in_room->exit[dir]->u1.to_room);
    if (prop != NULL) {
      free_string(prop->owner);
      prop->owner = str_dup(ch->name);
      prop->sale_price = 0;
      ch->pcdata->total_money -= cost * 100;
      printf_to_char(ch, "You buy a parcel of land for $%d!\n\r", cost);
    }
  }

  void expandland(CHAR_DATA *ch, char *argument) {
    int i;
    if (get_roomz(ch->in_room) != 0) {
      send_to_char("You have to be on the ground level first.\n\r", ch);
      return;
    }
    PROP_TYPE *prop;
    prop = prop_from_room(ch->in_room);
    if (prop == NULL || str_cmp(prop->owner, ch->name)) {
      send_to_char("You have to be on the edge of your existing property to do that.\n\r", ch);
      return;
    }
    ROOM_INDEX_DATA *startroom = get_room_index(7);
    for (i = 0; i < 10; i++) {
      if (i == DIR_SOUTHWEST || i == DIR_SOUTHEAST || i == DIR_NORTHWEST || i == DIR_NORTHEAST)
      continue;
      if (startroom->exit[i] != NULL && prop_from_room(startroom->exit[i]->u1.to_room) == prop) {
      }
    }

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(dir_name[i][0], argument) || !str_cmp(dir_name[i][1], argument)) {
        if (i == DIR_UP || i == DIR_DOWN || i == DIR_NORTHWEST || i == DIR_NORTHEAST || i == DIR_SOUTHEAST || i == DIR_SOUTHWEST) {
          send_to_char("Valid directions to buy land are north/south/east/west.\n\r", ch);
          return;
        }

        if (!check_buyability(ch->in_room, i)) {
          send_to_char("You can't buy that land.\n\r", ch);
          return;
        }
        smartdig(ch->in_room, i, 3);
        if (ch->in_room->exit[i] == NULL)
        return;

        ROOM_INDEX_DATA *entrance = ch->in_room->exit[i]->u1.to_room;
        entrance->sector_type = SECT_PARK;
        free_string(entrance->name);
        entrance->name = str_dup("`yA Building Site`x");
        entrance->size = 100;
        SET_BIT(entrance->room_flags, ROOM_UNLIT);

        smartdig(ch->in_room->exit[i]->u1.to_room, i, 3);
        if (ch->in_room->exit[i]->u1.to_room->exit[i] == NULL)
        return;

        ROOM_INDEX_DATA *croom =
        ch->in_room->exit[i]->u1.to_room->exit[i]->u1.to_room;

        int pointer = rev_dir[i];
        pointer = turn_dir[pointer];

        for (; pointer != rev_dir[i]; pointer = turn_dir[pointer]) {
          smartdig(croom, pointer, 3);
        }

        save_properties(FALSE);

        send_to_char("Done.\n\r", ch);
      }
    }
  }

  void abandonland(CHAR_DATA *ch, char *argument) {
    int i;
    if (get_roomz(ch->in_room) != 0) {
      send_to_char("You have to be on the ground level first.\n\r", ch);
      return;
    }
    PROP_TYPE *prop;
    prop = prop_from_room(ch->in_room);
    if (prop == NULL || str_cmp(prop->owner, ch->name)) {
      send_to_char("You have to be on the edge of your existing property to do that.\n\r", ch);
      return;
    }
    ROOM_INDEX_DATA *startroom = get_room_index(7);
    for (i = 0; i < 10; i++) {
      if (i == DIR_SOUTHWEST || i == DIR_SOUTHEAST || i == DIR_NORTHWEST || i == DIR_NORTHEAST)
      continue;
      if (startroom->exit[i] != NULL && prop_from_room(startroom->exit[i]->u1.to_room) == prop) {
      }
    }

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(dir_name[i][0], argument) || !str_cmp(dir_name[i][1], argument)) {
        if (i == DIR_UP || i == DIR_DOWN || i == DIR_NORTHWEST || i == DIR_NORTHEAST || i == DIR_SOUTHEAST || i == DIR_SOUTHWEST) {
          send_to_char("Valid directions to buy land are north/south/east/west.\n\r", ch);
          return;
        }

        if (ch->in_room->exit[i] == NULL)
        return;

        ROOM_INDEX_DATA *entrance = ch->in_room->exit[i]->u1.to_room;

        if (prop_from_room(entrance) != prop) {
          send_to_char("They don't seem to be part of the same property.\n\r", ch);
          return;
        }

        save_properties(FALSE);
      }
    }
  }

  bool viableedge(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    for (int i = 0; i < 10; i++) {
      if (i == DIR_NORTHWEST || i == DIR_SOUTHWEST || i == DIR_SOUTHEAST || i == DIR_SOUTHWEST)
      continue;

      if (room->exit[i] != NULL) {
        if (room->exit[i]->u1.to_room->sector_type == SECT_STREET)
        return FALSE;
        if (room->exit[i]->u1.to_room->sector_type == SECT_ALLEY)
        return FALSE;
      }
    }
    if (IS_SET(room->room_flags, ROOM_INDOORS))
    return FALSE;

    return TRUE;
  }

  int property_landvalue(PROP_TYPE *prop) { return 0; }

  int level_roomcount(int center) {
    int counter = 0;
    ROOM_INDEX_DATA *room;
    // Center
    if ((room = get_room_index(center)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // NW
    if ((room = get_room_index(center + 1 + 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // NE
    if ((room = get_room_index(center + 1 - 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // E
    if ((room = get_room_index(center - 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // S
    if ((room = get_room_index(center - 1)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // SW
    if ((room = get_room_index(center + 1000 - 1)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // W
    if ((room = get_room_index(center + 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    return counter;
  }

  int property_truevalue(PROP_TYPE *prop) {
    int value = 0;
    int sizex = (prop->maxx - prop->minx) + 1;
    int sizey = (prop->maxy - prop->miny) + 1;
    if (prop->type_special == PROPERTY_TOWNHOUSE || prop->type_special == PROPERTY_TOWNSHOP || prop->type_special == PROPERTY_LONGSHOP || prop->type_special == PROPERTY_LONGHOUSE || prop->type_special == PROPERTY_SMALLFLAT || prop->type_special == PROPERTY_MEDFLAT || prop->type_special == PROPERTY_BIGFLAT) {
      value = sizex * sizey * TOWNLAND_PRICE;
    }
    else if (prop->type_special == PROPERTY_OVERFLAT || prop->type_special == PROPERTY_UNDERSHOP || prop->type_special == PROPERTY_TRAILER) {
      value = sizex * sizey * TOWNLAND_PRICE * 3 / 5;
    }
    else if (prop->type_special == PROPERTY_INNERFOREST)
    value = sizex * sizey * FORESTLAND_PRICE;

    bool bedroomfound = FALSE;
    bool kitchenfound = FALSE;
    bool bathroomfound = FALSE;
    prop->realhouse = 0;
    ROOM_INDEX_DATA *room = first_room_in_property(prop);
    if (room == NULL)
    return value;

    if (get_room_index(prop->firstroom) == NULL)
    prop->firstroom = room->vnum;

    if (get_room_index(prop->roadroom) != NULL && get_room_index(prop->roadroom)->y > 0) {
      ROOM_INDEX_DATA *newroad =
      sourced_room_by_coordinates(room, get_room_index(prop->roadroom)->x, get_room_index(prop->roadroom)->y, 0, TRUE);
      if (newroad != NULL)
      prop->roadroom = newroad->vnum;
      else
      return value;
    }
    if (prop->type == PROP_SHOP)
    value += room_value(room) / 3;
    else
    value += room_value(room);

    prop->airfix = FALSE;
    setup_proproom(room, prop);
    if (IS_SET(room->room_flags, ROOM_BEDROOM))
    bedroomfound = TRUE;
    if (IS_SET(room->room_flags, ROOM_KITCHEN))
    kitchenfound = TRUE;
    if (IS_SET(room->room_flags, ROOM_BATHROOM))
    bathroomfound = TRUE;
    for (int i = 0; i < 200; i++) {
      if (bedroomfound == TRUE && bathroomfound == TRUE && kitchenfound == TRUE)
      prop->realhouse = 1;
      room = next_room_in_property(prop, room);
      if (room == NULL)
      return value;
      value += room_value(room);
      setup_proproom(room, prop);
      if (IS_SET(room->room_flags, ROOM_BEDROOM))
      bedroomfound = TRUE;
      if (IS_SET(room->room_flags, ROOM_KITCHEN))
      kitchenfound = TRUE;
      if (IS_SET(room->room_flags, ROOM_BATHROOM))
      bathroomfound = TRUE;
    }
    if (prop->type_special == PROPERTY_OUTERFOREST || prop->type_special == PROPERTY_INNERFOREST)
    value = value * 4 / 5;

    return value;
  }

  // checks that building is no greater than 2/3 of plot size - Discordance
  bool building_limit(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room;
    int center = get_plotcenter(ch);
    int counter = 0;
    int limit = 6;

    // Center
    if ((room = get_room_index(center)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // NW
    if ((room = get_room_index(center + 1 + 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // NE
    if ((room = get_room_index(center + 1 - 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // E
    if ((room = get_room_index(center - 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // S
    if ((room = get_room_index(center - 1)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // SW
    if ((room = get_room_index(center + 1000 - 1)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }
    // W
    if ((room = get_room_index(center + 1000)) != NULL) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        counter++;
      }
    }

    if (counter >= limit) {
      return TRUE;
    }

    return FALSE;
  }

  // determines if alley is mandated due to proximity to another structure
  bool nearby_structure(ROOM_INDEX_DATA *room) {
    int center = room->vnum;
    int northwest = center + 1 + 1000;
    int north = center + 1;
    int northeast = center + 1 - 1000;
    int east = center - 1000;
    int southeast = center - 1000 - 1;
    int south = center - 1;
    int southwest = center + 1000 - 1;
    int west = center + 1000;

    ROOM_INDEX_DATA *room2;

    // compare current room to northwest room
    if ((room2 = get_room_index(northwest)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to north room
    if ((room2 = get_room_index(north)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to northeast room
    if ((room2 = get_room_index(northeast)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to east room
    if ((room2 = get_room_index(east)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to southeast room
    if ((room2 = get_room_index(southeast)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to south room
    if ((room2 = get_room_index(south)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to southwest room
    if ((room2 = get_room_index(southwest)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }
    // compare current room to west room
    if ((room2 = get_room_index(west)) != NULL) {
      if (get_roomplot(room) != get_roomplot(room2)) {
        if (IS_SET(room->room_flags, ROOM_INDOORS)) {
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool can_develop(ROOM_INDEX_DATA *room, PROP_TYPE *prop) { return TRUE; }

  bool expanddown(CHAR_DATA *ch) {
    int i, center;
    PROP_TYPE *prop;
    prop = prop_from_room(ch->in_room);
    if (prop == NULL || !can_decorate(ch, ch->in_room)) {
      send_to_char("You have to be in your existing property to do that.\n\r", ch);
      return FALSE;
    }

    ROOM_INDEX_DATA *startroom = get_room_index(7);
    for (i = 0; i < 10; i++) {
      if (startroom->exit[i] != NULL && prop_from_room(startroom->exit[i]->u1.to_room) == prop) {
      }
    }

    // This should prevent players from expanding properties they don't own -
    // Discordance
    center = get_plotcenter(ch);
    if (center == 0) {
      send_to_char("You don't have permission to build here.\n\r", ch);
      return FALSE;
    }

    return FALSE;
  }

  bool expandup(CHAR_DATA *ch) {
    int i, center;
    PROP_TYPE *prop;
    prop = prop_from_room(ch->in_room);
    if (prop == NULL || !can_decorate(ch, ch->in_room)) {
      send_to_char("You have to be in your existing property to do that.\n\r", ch);
      return FALSE;
    }

    ROOM_INDEX_DATA *startroom = get_room_index(7);
    for (i = 0; i < 10; i++) {
      if (startroom->exit[i] != NULL && prop_from_room(startroom->exit[i]->u1.to_room) == prop) {
      }
    }
    // This should prevent players from expanding properties they don't own -
    // Discordance
    center = get_plotcenter(ch);
    if (center == 0) {
      send_to_char("You don't have permission to build here.\n\r", ch);
      return FALSE;
    }
    int plotcenter = center - 300000000;

    return FALSE;
  }

  bool has_hypocontrolup(CHAR_DATA *ch, PROP_TYPE *prop, ROOM_INDEX_DATA *room) {

    return FALSE;
  }

  bool has_hypocontroldown(PROP_TYPE *prop, ROOM_INDEX_DATA *room) {

    return FALSE;
  }

  bool center_room(PROP_TYPE *prop, ROOM_INDEX_DATA *room) { return FALSE; }

  void buildup(CHAR_DATA *ch, char *argument) {
    bool is_expanded = TRUE;
    PROP_TYPE *prop;
    prop = prop_from_room(ch->in_room);
    if (prop == NULL || !can_decorate(ch, ch->in_room)) {
      send_to_char("You have to be in your existing property to do that.\n\r", ch);
      return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      send_to_char("You can only build on top of indoor rooms.\n\r", ch);
      return;
    }

    if (ch->in_room->exit[DIR_UP] != NULL) {
      if (has_hypocontrolup(ch, prop, ch->in_room) && ch->in_room->exit[DIR_UP]->u1.to_room->sector_type != SECT_AIR && ch->in_room->exit[DIR_UP]->u1.to_room->sector_type != SECT_ATMOSPHERE) {
        send_to_char("You've already built up there.\n\r", ch);
        return;
      }
      if (!can_decorate(ch, ch->in_room->exit[DIR_UP]->u1.to_room) && !center_room(prop, ch->in_room)) {
        send_to_char("You have to be in the center of your property to do that.\n\r", ch);
        return;
      }
    }

    if (!has_hypocontrolup(ch, prop, ch->in_room)) {
      is_expanded = expandup(ch);
    }

    if (is_expanded == TRUE) {
      smartdig(ch->in_room, DIR_UP, 3);
    }
    if (has_hypocontrolup(ch, prop, ch->in_room) && ch->in_room->exit[DIR_UP] != NULL && ch->in_room->exit[DIR_UP]->u1.to_room != NULL) {

      ch->pcdata->total_money -= ROOM_PRICE * 100;
      prop->price += ROOM_PRICE;
      ch->in_room->exit[DIR_UP]->wall = WALL_NONE;
      ch->in_room->exit[DIR_UP]->u1.to_room->exit[DIR_DOWN]->wall = WALL_NONE;
      ROOM_INDEX_DATA *room = ch->in_room->exit[DIR_UP]->u1.to_room;

      if (prop->type == PROP_HOUSE) {
        room->sector_type = SECT_HOUSE;
      }
      else {
        room->sector_type = SECT_COMMERCIAL;
      }
      if (IS_SET(room->room_flags, ROOM_UNLIT))
      REMOVE_BIT(room->room_flags, ROOM_UNLIT);
      if (!IS_SET(room->room_flags, ROOM_DARK))
      SET_BIT(room->room_flags, ROOM_DARK);
      if (!IS_SET(room->room_flags, ROOM_LIGHTON))
      SET_BIT(room->room_flags, ROOM_LIGHTON);
      if (IS_SET(room->room_flags, ROOM_LIGHTOFF))
      REMOVE_BIT(room->room_flags, ROOM_LIGHTOFF);
      if (!IS_SET(room->room_flags, ROOM_INDOORS))
      SET_BIT(room->room_flags, ROOM_INDOORS);
      autowall(room);
      SET_BIT(room->area->area_flags, AREA_CHANGED);
      send_to_char("Upper room constructed.\n\r", ch);
    }

    return;
  }

  void builddown(CHAR_DATA *ch, char *argument) {
    bool is_expanded = TRUE;
    PROP_TYPE *prop;
    prop = prop_from_room(ch->in_room);
    if (prop == NULL || !can_decorate(ch, ch->in_room)) {
      send_to_char("You have to be in your existing property to do that.\n\r", ch);
      return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      send_to_char("You can only build under indoor rooms.\n\r", ch);
      return;
    }

    if (ch->in_room->exit[DIR_DOWN] != NULL && has_hypocontroldown(prop, ch->in_room)) {
      send_to_char("You've already built down there.\n\r", ch);
      return;
    }
    if (!has_hypocontroldown(prop, ch->in_room) && !center_room(prop, ch->in_room)) {
      send_to_char("You have to be in the center of your property to do that.\n\r", ch);
      return;
    }

    if (!has_hypocontroldown(prop, ch->in_room)) {
      is_expanded = expanddown(ch);
    }

    if (is_expanded == TRUE) {
      smartdig(ch->in_room, DIR_DOWN, 3);
      if (IS_IMMORTAL(ch))
      send_to_char("Down digging.\n\r", ch);
    }
    if (has_hypocontroldown(prop, ch->in_room) && ch->in_room->exit[DIR_DOWN] != NULL && ch->in_room->exit[DIR_DOWN]->u1.to_room != NULL) {
      ch->pcdata->total_money -= ROOM_PRICE * 100;
      prop->price += ROOM_PRICE;
      ROOM_INDEX_DATA *room = ch->in_room->exit[DIR_DOWN]->u1.to_room;

      if (prop->type == PROP_HOUSE) {
        room->sector_type = SECT_HOUSE;
      }
      else {
        room->sector_type = SECT_COMMERCIAL;
      }
      if (IS_SET(room->room_flags, ROOM_UNLIT))
      REMOVE_BIT(room->room_flags, ROOM_UNLIT);
      if (!IS_SET(room->room_flags, ROOM_DARK))
      SET_BIT(room->room_flags, ROOM_DARK);
      if (!IS_SET(room->room_flags, ROOM_LIGHTON))
      SET_BIT(room->room_flags, ROOM_LIGHTON);
      if (IS_SET(room->room_flags, ROOM_LIGHTOFF))
      REMOVE_BIT(room->room_flags, ROOM_LIGHTOFF);
      if (!IS_SET(room->room_flags, ROOM_INDOORS))
      SET_BIT(room->room_flags, ROOM_INDOORS);
      autowall(room);
      SET_BIT(room->area->area_flags, AREA_CHANGED);

      send_to_char("Lower room constructed.\n\r", ch);
    }

    return;
  }

  void claimland(CHAR_DATA *ch, char *argument) {
    bool connection = FALSE;

    if (ch->in_room == NULL)
    return;
    PROP_TYPE *connectprop = NULL;
    ROOM_INDEX_DATA *roadroom = NULL;
    int orientation = 0;
    if (in_prop(ch)) {
      send_to_char("This land is already claimed.\n\r", ch);
      return;
    }
    if (ch->in_room->area->vnum >= 22 && ch->in_room->area->vnum <= 25 && ch->in_room->x >= ch->in_room->area->minx + 24 && ch->in_room->x <= ch->in_room->area->minx + 26 && ch->in_room->y >= ch->in_room->area->miny + 24 && ch->in_room->y <= ch->in_room->area->miny + 26) {
      send_to_char("This land is already claimed.\n\r", ch);
      return;
    }
    if (str_cmp(argument, "house") && str_cmp(argument, "shop")) {
      send_to_char("Develop claimland House/shop\n\r", ch);
      return;
    }

    for (int i = 0; i < 10; i++) {
      if (ch->in_room->exit[i] != NULL && ch->in_room->exit[i]->u1.to_room != NULL) {
        if (ch->in_room->exit[i]->u1.to_room->sector_type == SECT_STREET) {
          connection = TRUE;
          roadroom = ch->in_room->exit[i]->u1.to_room;
          orientation = rev_dir[i];
        }
        if (prop_from_room(ch->in_room->exit[i]->u1.to_room) != NULL && !str_cmp(prop_from_room(ch->in_room->exit[i]->u1.to_room)->owner, ch->name)) {
          if (!str_cmp(argument, "house") && prop_from_room(ch->in_room->exit[i]->u1.to_room)->type ==
              PROP_HOUSE) {
            connection = TRUE;
            connectprop = prop_from_room(ch->in_room->exit[i]->u1.to_room);
          }
          if (!str_cmp(argument, "shop") && prop_from_room(ch->in_room->exit[i]->u1.to_room)->type ==
              PROP_SHOP) {
            connection = TRUE;
            connectprop = prop_from_room(ch->in_room->exit[i]->u1.to_room);
          }
        }
      }
    }
    if (ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_STREET) {
      send_to_char("You can only do this in cleared forests.\n\r", ch);
      return;
    }
    if (connection == FALSE) {
      send_to_char("You can only claim property adjacent to a street or to a property you already own.\n\r", ch);
      return;
    }
    if (ch->in_room->area->vnum != OUTER_NORTH_FOREST && ch->in_room->area->vnum != OUTER_SOUTH_FOREST && ch->in_room->area->vnum != OUTER_WEST_FOREST && ch->in_room->area->vnum != OTHER_FOREST_VNUM && ch->in_room->area->vnum != WILDS_FOREST_VNUM && ch->in_room->area->vnum != GODREALM_FOREST_VNUM && ch->in_room->area->vnum != HELL_FOREST_VNUM) {
      send_to_char("You'd have to buy or rent this land.\n\r", ch);
      return;
    }
    if (connectprop != NULL) {
      for (int i = 0; i < 200; i++) {
        if (connectprop->roomlist[i] == 0) {
          connectprop->roomlist[i] = ch->in_room->vnum;
          send_to_char("You add this land to your claim.\n\r", ch);
          return;
        }
      }
      send_to_char("Your claim is too large already.\n\r", ch);
      return;
    }

    if (safe_strlen(argument) < 3) {
      send_to_char("Syntax: Develop claimland (house/shop)\n\r", ch);
      return;
    }

    PROP_TYPE *prop;
    prop = new_prop();

    prop->roadroom = roadroom->vnum;
    prop->orientation = orientation;
    prop->firstroom = ch->in_room->vnum;
    prop->roomlist[0] = ch->in_room->vnum;
    prop->type_special = PROPERTY_OUTERFOREST;
    if (!str_cmp(argument, "house"))
    prop->type = PROP_HOUSE;
    else
    prop->type = PROP_SHOP;
    free_string(prop->address);
    free_string(prop->propname);
    prop->address = str_dup(create_address(roadroom, orientation));
    prop->propname = str_dup(prop->address);
    free_string(prop->owner);
    prop->owner = str_dup(ch->name);
    prop->auction_day = number_range(1, 28);
    prop->auction_month = get_month();
    prop->minz = -10;
    prop->maxz = 15;

    if (prop->type == PROP_SHOP)
    prop->vnum = next_prop_svnum();
    else
    prop->vnum = next_prop_hvnum();
    prop->price = property_truevalue(prop);
    if (get_day() <= prop->auction_day) {
      if (get_month() < 11) {
        prop->auction_month = get_month() + 1;
      }
      else {
        prop->auction_month = 0;
      }
    }
    prop->valid = TRUE;
    PropVect.push_back(prop);
    save_properties(FALSE);
    send_to_char("You stake your claim!\n\r", ch);
  }

  int shrine_type(PROP_TYPE *prop) {
    ROOM_INDEX_DATA *first = first_room_in_property(prop);
    if (first->area->world == WORLD_WILDS)
    return 2;
    if (prop->type_special == PROPERTY_OUTERFOREST)
    return 3;
    if (prop->type_special == PROPERTY_INNERFOREST)
    return 2;
    return 1;
  }
  int shrine_count(DOMAIN_TYPE *dom, int type) {
    int count = 0;
    for (int i = 0; i < 50; i++) {
      if (dom->smallshrines[i] != 0 && type == 1)
      count++;
      if (dom->medshrines[i] != 0 && type == 2)
      count++;
      if (dom->bigshrines[i] != 0 && type == 3)
      count++;
    }
    return count;
  }

  _DOFUN(do_develop) {
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);
    ch->pcdata->process_target = NULL;

    if (!str_cmp(arg1, "buyland")) {
      buynewland(ch, argument);
      return;
    }
    else if (!str_cmp(arg1, "claimland")) {
      claimland(ch, argument);
      return;
    }
    else if (!str_cmp(arg1, "abandonland")) {
      if (ch->in_room->area->vnum != OUTER_NORTH_FOREST && ch->in_room->area->vnum != OUTER_SOUTH_FOREST && ch->in_room->area->vnum != OUTER_WEST_FOREST && ch->in_room->area->vnum != OTHER_FOREST_VNUM && ch->in_room->area->vnum != WILDS_FOREST_VNUM && ch->in_room->area->vnum != GODREALM_FOREST_VNUM) {
        send_to_char("You cannot abandon this land.\n\r", ch);
        return;
      }
      PROP_TYPE *inprop = in_prop(ch);
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("`cYou need to own this property or have permission to decorate it`g.`x\n\r", ch);
        return;
      }
      for (int i = 0; i < 200; i++) {
        if (inprop->roomlist[i] == ch->in_room->vnum) {
          inprop->roomlist[i] = 0;
          ch->in_room->sector_type = SECT_PARK;
          send_to_char("Land abandoned.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "dig")) {
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("`cYou need to own this property or have permission to decorate it`g.`x\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "shallow")) {
        ch->pcdata->process = PROCESS_SHALLOWDIG;
        ch->pcdata->process_target = NULL;
        ch->pcdata->process_timer = UMAX(10, 60 - (labor_points(ch) * 5));
        if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
        ch->pcdata->process_timer = 2;
        ch->pcdata->process_subtype = ch->in_room->vnum;
        if (ch->pcdata->last_develop_type == ch->pcdata->process)
        ch->pcdata->process_timer =
        UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
        act("You start to dig a shallow pit to fill with water.", ch, NULL, NULL, TO_CHAR);
        act("$n starts to dig a pit.", ch, NULL, NULL, TO_ROOM);
        return;
      }
      if (!str_cmp(argument, "deep")) {
        ch->pcdata->process = PROCESS_DEEPDIG;
        ch->pcdata->process_target = NULL;
        ch->pcdata->process_timer = UMAX(15, 120 - (labor_points(ch) * 10));
        if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
        ch->pcdata->process_timer = 2;
        ch->pcdata->process_subtype = ch->in_room->vnum;
        if (ch->pcdata->last_develop_type == ch->pcdata->process)
        ch->pcdata->process_timer =
        UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
        act("You start to dig a deep pit to fill with water.", ch, NULL, NULL, TO_CHAR);
        act("$n starts to dig a pit.", ch, NULL, NULL, TO_ROOM);
        return;
      }
    }
    else if (!str_cmp(arg1, "sanctify")) {
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("`cYou need to own this property or have permission to decorate it`g.`x\n\r", ch);
        return;
      }
      if (higher_power(ch)) {
        send_to_char("Seems a bit egotistical.\n\r", ch);
        return;
      }
      if(!has_nonconsume(ch, ITEM_BLOOD))
      {
        send_to_char("You need blood to sanctify this area.\n\r", ch);
        return;
      }
      //	if(ch->pcdata->account->lastshrine > current_time)
      //	{
      //	   send_to_char("You built a shrine too recently.\n\r", ch);
      //	   return;
      //	}
      DOMAIN_TYPE *dom = domain_by_name(argument);
      if (dom == NULL) {
        send_to_char("No such eidilon.\n\r", ch);
        return;
      }
      PROP_TYPE *prop = prop_from_room(ch->in_room);
      if (prop == NULL) {
        send_to_char("You are not in a property.\n\r", ch);
        return;
      }
      ch->pcdata->process = PROCESS_SHRINE;
      ch->pcdata->process_target = NULL;
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(dom->domain_of);
      ch->pcdata->process_timer = UMAX(10, 60 - (labor_points(ch) * 5));
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You start to sanctify the area.", ch, NULL, NULL, TO_CHAR);
      act("$n starts to sanctify the area.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg1, "desanctify")) {
      if (in_prop(ch) != NULL && !can_decorate(ch, ch->in_room)) {
        send_to_char("`cYou need to own this property or have permission to decorate it`g.`x\n\r", ch);
        return;
      }

      if (vnum_domain(ch->pcdata->in_domain) == NULL) {
        send_to_char("You are not in a domain.\n\r", ch);
        return;
      }
      if (higher_power(ch)) {
        DOMAIN_TYPE *dom = vnum_domain(ch->pcdata->in_domain);
        if (dom == NULL) {
          send_to_char("Not in a domain.\n\r", ch);
          return;
        }

        for (int i = 0; i < 50; i++) {
          if (dom->bigshrines[i] == ch->in_room->vnum) {
            dom->bigshrines[i] = 0;
            act("You finish cleansing the area.", ch, NULL, NULL, TO_CHAR);
            act("$n finishes cleansing the area.", ch, NULL, NULL, TO_ROOM);
            return;
          }
          if (dom->medshrines[i] == ch->in_room->vnum) {
            dom->medshrines[i] = 0;
            act("You finish cleansing the area.", ch, NULL, NULL, TO_CHAR);
            act("$n finishes cleansing the area.", ch, NULL, NULL, TO_ROOM);
            return;
          }
          if (dom->smallshrines[i] == ch->in_room->vnum) {
            dom->smallshrines[i] = 0;
            act("You finish cleansing the area.", ch, NULL, NULL, TO_CHAR);
            act("$n finishes cleansing the area.", ch, NULL, NULL, TO_ROOM);
            return;
          }
        }
        act("This area isn't sanctified.", ch, NULL, NULL, TO_CHAR);
        return;
      }
      bool founds = FALSE;
      for (vector<DOMAIN_TYPE *>::iterator it = DomainVect.begin();
      it != DomainVect.end(); ++it) {
        for (int i = 0; i < 250; i++) {
          if ((*it)->bigshrines[i] == ch->in_room->vnum) {
            founds = TRUE;
          }
          if ((*it)->medshrines[i] == ch->in_room->vnum) {
            founds = TRUE;
          }
          if ((*it)->smallshrines[i] == ch->in_room->vnum) {
            founds = TRUE;
          }
        }
      }
      if (founds == FALSE) {
        act("This area isn't sanctified.", ch, NULL, NULL, TO_CHAR);
        return;
      }

      ch->pcdata->process = PROCESS_NOSHRINE;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 60 - (labor_points(ch) * 5));
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You start to cleanse the area.", ch, NULL, NULL, TO_CHAR);
      act("$n starts to cleanse the area.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    else if (!str_cmp(arg1, "fillin")) {
      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("`cYou need to own this property or have permission to decorate it`g.`x\n\r", ch);
        return;
      }
      if (ch->in_room->sector_type != SECT_WATER && ch->in_room->sector_type != SECT_SHALLOW && ch->in_room->sector_type != SECT_UNDERWATER) {
        send_to_char("There doesn't seem to be any water here.\n\r", ch);
        return;
      }
      ch->pcdata->process = PROCESS_FILLIN;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 60 - (labor_points(ch) * 5));
      if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
      ch->pcdata->process_timer = 2;
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You start to drain the water and fill in the pit.", ch, NULL, NULL, TO_CHAR);
      act("$n starts to drain the water and fill in the pit.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg1, "clear")) {
      if (ch->in_room->sector_type != SECT_FOREST || prop_from_room(ch->in_room) != NULL) {
        send_to_char("You can't clear here.\n\r", ch);
        return;
      }
      if (get_energy(ch) >= 100) {
        send_to_char("You're too tired to do that.\n\r", ch);
        return;
      }
      ch->pcdata->process = PROCESS_CLEARING;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 90 - (labor_points(ch) * 10));
      if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
      ch->pcdata->process_timer = 2;
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You begin to chop down trees in the area.", ch, NULL, NULL, TO_CHAR);
      act("$n begins to chop down trees in the area.", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(arg1, "makeroad")) {
      if (ch->in_room->sector_type != SECT_PARK || prop_from_room(ch->in_room) != NULL) {
        send_to_char("You can't make a road here, try clearing the area first.\n\r", ch);
        return;
      }
      if (ch->in_room->area->vnum == HAVEN_TOWN_VNUM) {
        send_to_char("You can't make a road here.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 10) {
        send_to_char("Syntax: develop makeroad (name of road)", ch);
        return;
      }
      if (get_energy(ch) >= 100) {
        send_to_char("You're too tired to do that.\n\r", ch);
        return;
      }
      bool connection = FALSE;
      for (int i = 0; i < 10; i++) {
        if (i != DIR_NORTH && i != DIR_SOUTH && i != DIR_EAST && i != DIR_WEST)
        continue;
        if (ch->in_room->exit[i] != NULL && ch->in_room->exit[i]->u1.to_room != NULL) {
          if (ch->in_room->exit[i]->u1.to_room->sector_type == SECT_STREET)
          connection = TRUE;
          if (ch->in_room->exit[i]->u1.to_room->sector_type == SECT_ALLEY)
          connection = TRUE;
        }
      }
      if (connection == FALSE) {
        send_to_char("You need to be adjacent to an existing road first.\n\r", ch);
        return;
      }
      if (ch->money < 1000) {
        send_to_char("You need at least $10 on hand for the required materials.\n\r", ch);
        return;
      }
      ch->pcdata->process = PROCESS_ROAD;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 45 - (labor_points(ch) * 5));
      if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
      ch->pcdata->process_timer = 2;
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(argument);
      act("You begin to create a dirt road.", ch, NULL, NULL, TO_CHAR);
      act("$n begins to create a dirt road.", ch, NULL, NULL, TO_ROOM);
      send_to_char("`WYou can now write the description for the new road.\n\r`x", ch);
      string_append(ch, &ch->pcdata->process_argumenttwo);
    }
    else if (!str_cmp(arg1, "paveroad")) {
      if (ch->in_room->sector_type != SECT_STREET || prop_from_room(ch->in_room) != NULL) {
        send_to_char("You can't clear here.\n\r", ch);
        return;
      }
      if (get_energy(ch) >= 100) {
        send_to_char("You're too tired to do that.\n\r", ch);
        return;
      }
      if (!IS_SET(ch->in_room->room_flags, ROOM_DIRTROAD)) {
        send_to_char("That's already been paved.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 10) {
        send_to_char("Syntax: develop makeroad (name of road)", ch);
        return;
      }
      if (ch->money < 10000) {
        send_to_char("You'd need $100 in materials to do that.\n\r", ch);
        return;
      }
      ch->pcdata->process = PROCESS_PAVING;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 90 - (labor_points(ch) * 10));
      if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
      ch->pcdata->process_timer = 2;
      ch->pcdata->process_subtype = ch->in_room->vnum;
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(argument);
      string_append(ch, &ch->pcdata->process_argumenttwo);
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You begin to pave the road.", ch, NULL, NULL, TO_CHAR);
      act("$n begins to pave the road.", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(arg1, "relay")) {
      if (ch->in_room->sector_type != SECT_STREET || prop_from_room(ch->in_room) != NULL) {
        send_to_char("You can't clear here.\n\r", ch);
        return;
      }
      if (get_energy(ch) >= 100) {
        send_to_char("You're too tired to do that.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 10) {
        send_to_char("Syntax: develop relay (name of road)", ch);
        return;
      }
      ch->pcdata->process = PROCESS_ROAD;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 45 - (labor_points(ch) * 5));
      if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
      ch->pcdata->process_timer = 2;
      ch->pcdata->process_subtype = ch->in_room->vnum;
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(argument);
      act("You begin to create a dirt road.", ch, NULL, NULL, TO_CHAR);
      act("$n begins to create a dirt road.", ch, NULL, NULL, TO_ROOM);
      send_to_char("`WYou can now write the description for the new road.\n\r`x", ch);
      string_append(ch, &ch->pcdata->process_argumenttwo);
    }
    else if (!str_cmp(arg1, "paveroad")) {
      if (ch->in_room->sector_type != SECT_STREET || prop_from_room(ch->in_room) != NULL) {
        send_to_char("You can't clear here.\n\r", ch);
        return;
      }
      if (get_energy(ch) >= 100) {
        send_to_char("You're too tired to do that.\n\r", ch);
        return;
      }
      if (!IS_SET(ch->in_room->room_flags, ROOM_DIRTROAD)) {
        send_to_char("That's already been paved.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 10) {
        send_to_char("Syntax: develop makeroad (name of road)", ch);
        return;
      }
      if (ch->money < 10000) {
        send_to_char("You'd need $100 in materials to do that.\n\r", ch);
        return;
      }
      ch->pcdata->process = PROCESS_PAVING;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 90 - (labor_points(ch) * 10));
      if(decree_target(ch->in_room->area->world, DECREE_BUILDER, ch->name))
      ch->pcdata->process_timer = 2;
      ch->pcdata->process_subtype = ch->in_room->vnum;
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(argument);
      string_append(ch, &ch->pcdata->process_argumenttwo);
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You begin to pave the road.", ch, NULL, NULL, TO_CHAR);
      act("$n begins to pave the road.", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(arg1, "relay")) {
      if (ch->in_room->sector_type != SECT_STREET || prop_from_room(ch->in_room) != NULL) {
        send_to_char("You can't clear here.\n\r", ch);
        return;
      }
      if (get_energy(ch) >= 100) {
        send_to_char("You're too tired to do that.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 10) {
        send_to_char("Syntax: develop relay (name of road)", ch);
        return;
      }
      ch->pcdata->process = PROCESS_RELAY;
      ch->pcdata->process_target = NULL;
      ch->pcdata->process_timer = UMAX(10, 60 - (labor_points(ch) * 10));
      ch->pcdata->process_subtype = ch->in_room->vnum;
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(argument);
      string_append(ch, &ch->pcdata->process_argumenttwo);
      if (ch->pcdata->last_develop_type == ch->pcdata->process)
      ch->pcdata->process_timer =
      UMIN(ch->pcdata->process_timer, ch->pcdata->last_develop_time);
      act("You begin to relay the road.", ch, NULL, NULL, TO_CHAR);
      act("$n begins to relay the road.", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(arg1, "zone") && IS_IMMORTAL(ch)) {
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        (*it)->minz = town_minz(NULL, (*it));
        (*it)->maxz = town_maxz(NULL, (*it));
      }
    }
    else
    send_to_char("Valid commands are buyland, dig shallow, dig deep, fillin, clear, makeroad, paveroad, relay.\n\r", ch);
  }

  /*
//eventual replacement for do_building - Discordance
//premise: buildings change dimensions, but land is finite

_DOFUN(do_buyland) {
char arg1[MSL];
char arg2[MSL];

argument = one_argument_nouncap( argument, arg1);
argument = one_argument_nouncap( argument, arg2);

//syntax: buy land size direction
int size, direction;

//arg1 processing
if(!str_cmp(arg1, "north")) {
direction = DIR_NORTH;
}
else if(!str_cmp(arg1, "east")) {
direction = DIR_EAST;
}
else if(!str_cmp(arg1, "west")) {
direction = DIR_WEST;
}
else if(!str_cmp(arg1, "south")) {
direction = DIR_SOUTH;
}
else {
send_to_char("Syntax: buyland (direction) (size)\n\r", ch);
return;
}

//arg2 processing for size
if(!str_cmp(arg2, "small")) {
//3x3
size = PROP_SMALL;
}
else if(!str_cmp(arg2, "medium")) {
//6x3
size = PROP_MEDIUM;
}
else if(!str_cmp(arg2, "large")) {
//6x6
size = PROP_LARGE;
}
else if(!str_cmp(arg2, "extralarge")) {
//6x12
size = PROP_XLARGE;
}
else {
//Need to list available plot sizes if used without size argument.
send_to_char("Valid sizes are: Small, medium, large, extralarge.\n\r", ch);
return;
}

if(ch->in_room->sector_type != SECT_STREET) {
send_to_char("You can only build from streets.\n\r", ch);
return;
}

if(ch->in_room->exit[direction] != NULL) {
send_to_char("There's already something there.\n\r", ch);
return;
}

//this needs to be adjusted for typical land prices
//figure out how this works later
int price = get_price(ch->in_room, size);
price = price * 12/10;
price *= 2;

//Need to put catch for available plot sizes in location

//printf_to_char(ch, "This location is zoned for %s buildings.`x\n\r", ????);
//need variable(s) for sizes available in zone printf_to_char(ch, "It will
cost $%d for the land alone.`x\n\r", price); printf_to_char(ch, "Would you
like to purchase a %s plot to the %s?`x\n\r", arg1, arg2);
}
*/

  _DOFUN(do_building) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);
    argument = one_argument_nouncap(argument, arg4);
    int size, type, direction;

    if (ch->played / 3600 < 20) {
      send_to_char("Get to 20 hours first.\n\r", ch);
      return;
    }

    if (!str_cmp(arg3, "north")) {
      direction = DIR_NORTH;
    }
    else if (!str_cmp(arg3, "east")) {
      direction = DIR_EAST;
    }
    else if (!str_cmp(arg3, "west")) {
      direction = DIR_WEST;
    }
    else if (!str_cmp(arg3, "south")) {
      direction = DIR_SOUTH;
    }
    else {
      send_to_char("Syntax: build (size) (house/shop/public) (direction)\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "house")) {
      type = PROP_HOUSE;
    }
    else if (!str_cmp(arg2, "shop")) {
      type = PROP_SHOP;
    }
    else {
      send_to_char("Valid types are house/shop/public.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "medium")) {
      size = PROP_MEDIUM;
    }
    else if (!str_cmp(arg1, "small")) {
      size = PROP_SMALL;
    }
    else if (!str_cmp(arg1, "large")) {
      size = PROP_LARGE;
    }
    else if (!str_cmp(arg1, "extralarge")) {
      size = PROP_XLARGE;
    }
    else if (!str_cmp(arg1, "tall")) {
      size = PROP_TALL;
    }
    else {
      send_to_char("Valid sizes are:Small, medium, large, extralarge, and tall.\n\r", ch);
      return;
    }

    if (type != PROP_HOUSE && type != PROP_SHOP) {
      send_to_char("Those types of buildings aren't finished yet.\n\r", ch);
      return;
    }

    if (type == PROP_HOUSE && size == SIZE_SMALL) {
      send_to_char("Houses can't be that small.\n\r", ch);
      return;
    }

    if (is_name("and", ch->in_room->name)) {
      send_to_char("You can only build from streets.\n\r", ch);
      return;
    }

    if (ch->in_room->sector_type != SECT_STREET) {
      send_to_char("You can only build from streets.\n\r", ch);
      return;
    }
    if (ch->in_room->vnum < 1000 || ch->in_room->vnum > 1999) {
      send_to_char("You can only build from streets.\n\r", ch);
      return;
    }
    if (ch->in_room->vnum <= 1440 && ch->in_room->vnum > 1430) {
      send_to_char("You can only build from streets.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum <= 1155 && ch->in_room->vnum > 1148) {
      send_to_char("The property in this area doesn't seem to be for sale.\n\r", ch);
      return;
    }

    int new_ne = turn_dir[direction];
    int new_e = turn_dir[new_ne];
    int new_se = turn_dir[new_e];
    int new_s = turn_dir[new_se];
    int new_sw = turn_dir[new_s];
    int new_w = turn_dir[new_sw];

    if ((ch->in_room->exit[new_e] == NULL || ch->in_room->exit[new_w] == NULL) && size != PROP_SMALL) {
      send_to_char("There isn't enough room there.  Try one over.\n\r", ch);
      return;
    }

    if (ch->in_room->exit[direction] != NULL) {
      send_to_char("There's already something there.\n\r", ch);
      return;
    }

    if (ch->in_room->vnum % 2 != 0) {
      send_to_char("There isn't enough room there.  Try one over.\n\r", ch);
      return;
    }

    int price = get_price(ch->in_room, size);
    price = price * 12 / 10;
    price *= 2;

    if (ch->pcdata->total_money < (price * 100) || !str_cmp(arg4, "appraisal") || !str_cmp(arg4, "a") || !str_cmp(arg4, "ap") || !str_cmp(arg4, "app") || !str_cmp(arg4, "appr") || !str_cmp(arg4, "appra") || !str_cmp(arg4, "apprai") || !str_cmp(arg4, "apprais") || !str_cmp(arg4, "appraisa") || !str_cmp(arg4, "appraise")) {
      printf_to_char(ch, "You'd need %d dollars to build that.\n\r", price);
      return;
    }
    ch->pcdata->total_money -= (price * 100);

    build_property(direction, type, size, ch->in_room, ch->name, argument);

    send_to_char("Built!\n\r", ch);
  }

  _DOFUN(do_recycleproperty) { recycle_property(atoi(argument), 0); }

  void shop_profits(CHAR_DATA *ch) {
    char buf[MSL];
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it)->type != PROP_SHOP || str_cmp(ch->name, (*it)->owner))
      continue;

      if ((*it)->profit <= 0 && (*it)->creditprofit <= 0)
      continue;

      if ((*it)->industry == 0) {
        (*it)->profit /= 2;
        (*it)->creditprofit /= 2;
      }

      if ((*it)->profit > 0) {
        sprintf(buf, "CASH: %s gets profit %d.\n\r", ch->name, (*it)->profit);
        log_string(buf);
      }

      sprintf(buf, "%s\n%s produces a profit of $%d(%d)", ch->pcdata->messages, (*it)->propname, ((*it)->profit + (*it)->creditprofit) / 100, (*it)->profit / 100);
      free_string(ch->pcdata->messages);
      ch->pcdata->messages = str_dup(buf);
      ch->pcdata->total_money += (*it)->profit;
      ch->pcdata->total_credit += (*it)->creditprofit;
      (*it)->profit = 0;
      (*it)->creditprofit = 0;
    }
  }

  void save_dorms() {
    int i;
    FILE *fpout;

    if ((fpout = fopen(DORMS_FILE, "w")) == NULL) {
      bug("Cannot open dorms.txt for writing", 0);
      return;
    }

    fprintf(fpout, "#DORMS\n");
    for (i = 0; i < MAX_DORMROOMS + MAX_DORMROOMS; i++) {
      fprintf(fpout, "DormRoom %d %s~\n", i, enclave_room[i]);
    }
    fprintf(fpout, "End\n\n");
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_dorms(FILE *fp) {
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
      case 'D':
        if (!str_cmp(word, "DormRoom")) {
          int i = fread_number(fp);
          free_string(enclave_room[i]);
          enclave_room[i] = str_dup(fread_string(fp));
          if (!strcmp(enclave_room[i], "(null)")) {
            free_string(enclave_room[i]);
            enclave_room[i] = str_dup("");
          }
          fMatch = TRUE;
          break;
        }
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_dorms: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_dorms() {
    FILE *fp;

    if ((fp = fopen(DORMS_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_dorms: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "DORMS")) {
          fread_dorms(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_dorms: bad section.", 0);
          continue;
        }
      }
      fclose(fp);
      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open dorms.txt", 0);
      exit(0);
    }
  }

  void dorms_update() {
    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    char buf[MSL];

    for (int i = 0; i < MAX_DORMROOMS; i++) {
      if (safe_strlen(enclave_room[i]) > 2) {
        if (!strcmp(enclave_room[i], "(null)")) {
          free_string(enclave_room[i]);
          enclave_room[i] = str_dup("");
        }
        if ((victim = get_char_world_pc(enclave_room[i])) !=
            NULL) { // Online check
          online = TRUE;
        }
        else {
          // Deleted PC check and offline load
          if (!load_char_obj(&d, enclave_room[i])) {
            free_string(enclave_room[i]);
            enclave_room[i] = str_dup("");
            continue;
          }
          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(enclave_room[i]));
          stat(buf, &sb);
          victim = d.character;
        }

        // NPC catch just in case similar names cause mismatch
        if (IS_NPC(victim)) {
          if (!online) {
            free_char(victim);
          }
          // Freeing room if NPC mismatch so NPC doesn't hold room for nonexistent
          // char
          free_string(enclave_room[i]);
          enclave_room[i] = str_dup("");
          continue;
        }

        if (daysidle(enclave_room[i]) > 7 || !college_student(victim, TRUE)) {
          free_string(enclave_room[i]);
          enclave_room[i] = str_dup("");
        }
        if (!online) {
          free_char(victim);
        }
      }
    }
  }

  void save_containers() {
    FILE *fpout;

    if ((fpout = fopen(CONTAINER_FILE, "w")) == NULL) {
      bug("Cannot open containers.txt for writing", 0);
      return;
    }

    fprintf(fpout, "#RENT\n");
    fprintf(fpout, "RentOne %s~\n", cont_one);
    fprintf(fpout, "RentTwo %s~\n", cont_two);
    fprintf(fpout, "RentThree %s~\n", cont_three);
    fprintf(fpout, "RentFour %s~\n", cont_four);
    fprintf(fpout, "RentFive %s~\n", cont_five);
    fprintf(fpout, "RentSix %s~\n", cont_six);
    fprintf(fpout, "RentSeven %s~\n", cont_seven);
    fprintf(fpout, "RentEight %s~\n", cont_eight);
    fprintf(fpout, "RentNine %s~\n", cont_nine);
    fprintf(fpout, "RentTen %s~\n", cont_ten);
    fprintf(fpout, "RentEleven %s~\n", cont_eleven);
    fprintf(fpout, "RentTwelve %s~\n", cont_twelve);

    fprintf(fpout, "End\n\n");

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_container(FILE *fp) {
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
      case 'R':
        KEY("RentOne", cont_one, fread_string(fp));
        KEY("RentTwo", cont_two, fread_string(fp));
        KEY("RentThree", cont_three, fread_string(fp));
        KEY("RentFour", cont_four, fread_string(fp));
        KEY("RentFive", cont_five, fread_string(fp));
        KEY("RentSix", cont_six, fread_string(fp));
        KEY("RentSeven", cont_seven, fread_string(fp));
        KEY("RentEight", cont_eight, fread_string(fp));
        KEY("RentNine", cont_nine, fread_string(fp));
        KEY("RentTen", cont_ten, fread_string(fp));
        KEY("RentEleven", cont_eleven, fread_string(fp));
        KEY("RentTwelve", cont_twelve, fread_string(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_container: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_container() {
    FILE *fp;

    if ((fp = fopen(CONTAINER_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_container: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "RENT")) {
          fread_container(fp);
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
      bug("Cannot open container.txt", 0);
      exit(0);
    }
  }

  void container_update() {
    if (safe_strlen(cont_one) > 2) {
      if (bank_check(cont_one) < 25 || daysidle(cont_one) > 4) {
        free_string(cont_one);
        cont_one = str_dup("");
      }
      else {
        house_charge(cont_one, 25);
      }
    }
    if (safe_strlen(cont_two) > 2) {
      if (bank_check(cont_two) < 25 || daysidle(cont_two) > 4) {
        free_string(cont_two);
        cont_two = str_dup("");
      }
      else {
        house_charge(cont_two, 25);
      }
    }
    if (safe_strlen(cont_three) > 2) {
      if (bank_check(cont_three) < 25 || daysidle(cont_three) > 4) {
        free_string(cont_three);
        cont_three = str_dup("");
      }
      else {
        house_charge(cont_three, 25);
      }
    }
    if (safe_strlen(cont_four) > 2) {
      if (bank_check(cont_four) < 25 || daysidle(cont_four) > 4) {
        free_string(cont_four);
        cont_four = str_dup("");
      }
      else {
        house_charge(cont_four, 25);
      }
    }
    if (safe_strlen(cont_five) > 2) {
      if (bank_check(cont_five) < 25 || daysidle(cont_five) > 4) {
        free_string(cont_five);
        cont_five = str_dup("");
      }
      else {
        house_charge(cont_five, 25);
      }
    }
    if (safe_strlen(cont_six) > 2) {
      if (bank_check(cont_six) < 25 || daysidle(cont_six) > 4) {
        free_string(cont_six);
        cont_six = str_dup("");
      }
      else {
        house_charge(cont_six, 25);
      }
    }
    if (safe_strlen(cont_seven) > 2) {
      if (bank_check(cont_seven) < 25 || daysidle(cont_seven) > 4) {
        free_string(cont_seven);
        cont_seven = str_dup("");
      }
      else {
        house_charge(cont_seven, 25);
      }
    }
    if (safe_strlen(cont_eight) > 2) {
      if (bank_check(cont_eight) < 25 || daysidle(cont_eight) > 4) {
        free_string(cont_eight);
        cont_eight = str_dup("");
      }
      else {
        house_charge(cont_eight, 25);
      }
    }
    if (safe_strlen(cont_nine) > 2) {
      if (bank_check(cont_nine) < 25 || daysidle(cont_nine) > 4) {
        free_string(cont_nine);
        cont_nine = str_dup("");
      }
      else {
        house_charge(cont_nine, 25);
      }
    }
    if (safe_strlen(cont_ten) > 2) {
      if (bank_check(cont_ten) < 25 || daysidle(cont_ten) > 4) {
        free_string(cont_ten);
        cont_ten = str_dup("");
      }
      else {
        house_charge(cont_ten, 25);
      }
    }
    if (safe_strlen(cont_eleven) > 2) {
      if (bank_check(cont_eleven) < 25 || daysidle(cont_eleven) > 4) {
        free_string(cont_eleven);
        cont_eleven = str_dup("");
      }
      else {
        house_charge(cont_eleven, 25);
      }
    }
    if (safe_strlen(cont_twelve) > 2) {
      if (bank_check(cont_twelve) < 25 || daysidle(cont_twelve) > 4) {
        free_string(cont_twelve);
        cont_twelve = str_dup("");
      }
      else {
        house_charge(cont_twelve, 25);
      }
    }
  }

  void save_rent() {
    FILE *fpout;

    if ((fpout = fopen(RENT_FILE, "w")) == NULL) {
      bug("Cannot open rent.txt for writing", 0);
      return;
    }

    fprintf(fpout, "#RENT\n");
    fprintf(fpout, "RentOne %s~\n", rent_one);
    fprintf(fpout, "RentTwo %s~\n", rent_two);
    fprintf(fpout, "RentThree %s~\n", rent_three);
    fprintf(fpout, "RentFour %s~\n", rent_four);
    fprintf(fpout, "RentFive %s~\n", rent_five);
    fprintf(fpout, "RentSix %s~\n", rent_six);
    fprintf(fpout, "RentSeven %s~\n", rent_seven);
    fprintf(fpout, "RentEight %s~\n", rent_eight);
    fprintf(fpout, "RentNine %s~\n", rent_nine);
    fprintf(fpout, "RentTen %s~\n", rent_ten);
    fprintf(fpout, "RentEleven %s~\n", rent_eleven);
    fprintf(fpout, "RentTwelve %s~\n", rent_twelve);
    fprintf(fpout, "RentThirteen %s~\n", rent_thirteen);
    fprintf(fpout, "RentFourteen %s~\n", rent_fourteen);
    fprintf(fpout, "RentFifteen %s~\n", rent_fifteen);
    fprintf(fpout, "RentSixteen %s~\n", rent_sixteen);
    fprintf(fpout, "RentSeventeen %s~\n", rent_seventeen);
    fprintf(fpout, "RentEighteen %s~\n", rent_eighteen);
    fprintf(fpout, "RentNineteen %s~\n", rent_nineteen);
    fprintf(fpout, "RentTwenty %s~\n", rent_twenty);
    fprintf(fpout, "RentTwentyOne %s~\n", rent_twentyone);
    fprintf(fpout, "RentTwentyTwo %s~\n", rent_twentytwo);

    fprintf(fpout, "End\n\n");

    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_containers();
    save_dorms();
  }

  void fread_rent(FILE *fp) {
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
      case 'R':
        KEY("RentOne", rent_one, fread_string(fp));
        KEY("RentTwo", rent_two, fread_string(fp));
        KEY("RentThree", rent_three, fread_string(fp));
        KEY("RentFour", rent_four, fread_string(fp));
        KEY("RentFive", rent_five, fread_string(fp));
        KEY("RentSix", rent_six, fread_string(fp));
        KEY("RentSeven", rent_seven, fread_string(fp));
        KEY("RentEight", rent_eight, fread_string(fp));
        KEY("RentNine", rent_nine, fread_string(fp));
        KEY("RentTen", rent_ten, fread_string(fp));
        KEY("RentEleven", rent_eleven, fread_string(fp));
        KEY("RentTwelve", rent_twelve, fread_string(fp));
        KEY("RentThirteen", rent_thirteen, fread_string(fp));
        KEY("RentFourteen", rent_fourteen, fread_string(fp));
        KEY("RentFifteen", rent_fifteen, fread_string(fp));
        KEY("RentSixteen", rent_sixteen, fread_string(fp));
        KEY("RentSeventeen", rent_seventeen, fread_string(fp));
        KEY("RentEighteen", rent_eighteen, fread_string(fp));
        KEY("RentNineteen", rent_nineteen, fread_string(fp));
        KEY("RentTwenty", rent_twenty, fread_string(fp));
        KEY("RentTwentyOne", rent_twentyone, fread_string(fp));
        KEY("RentTwentyTwo", rent_twentytwo, fread_string(fp));

        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_rent: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_rent() {
    FILE *fp;

    if ((fp = fopen(RENT_FILE, "r")) != NULL) {
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
        if (!str_cmp(word, "RENT")) {
          fread_rent(fp);
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
      bug("Cannot open rent.txt", 0);
      exit(0);
    }
    load_container();
    load_dorms();
  }

  void rent_update() {
    if (safe_strlen(rent_one) > 2) {
      if (bank_check(rent_one) < 25 || daysidle(rent_one) > 4) {
        free_string(rent_one);
        rent_one = str_dup("");
      }
      else {
        house_charge(rent_one, 25);
        offline_message(rent_one, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_two) > 2) {
      if (bank_check(rent_two) < 25 || daysidle(rent_two) > 4) {
        free_string(rent_two);
        rent_two = str_dup("");
      }
      else {
        house_charge(rent_two, 25);
        offline_message(rent_two, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_three) > 2) {
      if (bank_check(rent_three) < 25 || daysidle(rent_three) > 4) {
        free_string(rent_three);
        rent_three = str_dup("");
      }
      else {
        house_charge(rent_three, 25);
        offline_message(rent_three, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_four) > 2) {
      if (bank_check(rent_four) < 25 || daysidle(rent_four) > 4) {
        free_string(rent_four);
        rent_four = str_dup("");
      }
      else {
        house_charge(rent_four, 25);
        offline_message(rent_four, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_five) > 2) {
      if (bank_check(rent_five) < 25 || daysidle(rent_five) > 4) {
        free_string(rent_five);
        rent_five = str_dup("");
      }
      else {
        house_charge(rent_five, 25);
        offline_message(rent_five, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_six) > 2) {
      if (bank_check(rent_six) < 25 || daysidle(rent_six) > 4) {
        free_string(rent_six);
        rent_six = str_dup("");
      }
      else {
        house_charge(rent_six, 25);
        offline_message(rent_six, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_seven) > 2) {
      if (bank_check(rent_seven) < 25 || daysidle(rent_seven) > 4) {
        free_string(rent_seven);
        rent_seven = str_dup("");
      }
      else {
        house_charge(rent_seven, 25);
        offline_message(rent_seven, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_eight) > 2) {
      if (bank_check(rent_eight) < 25 || daysidle(rent_eight) > 4) {
        free_string(rent_eight);
        rent_eight = str_dup("");
      }
      else {
        house_charge(rent_eight, 25);
        offline_message(rent_eight, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_nine) > 2) {
      if (bank_check(rent_nine) < 25 || daysidle(rent_nine) > 4) {
        free_string(rent_nine);
        rent_nine = str_dup("");
      }
      else {
        house_charge(rent_nine, 25);
        offline_message(rent_nine, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_ten) > 2) {
      if (bank_check(rent_ten) < 25 || daysidle(rent_ten) > 4) {
        free_string(rent_ten);
        rent_ten = str_dup("");
      }
      else {
        house_charge(rent_ten, 25);
        offline_message(rent_ten, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_eleven) > 2) {
      if (bank_check(rent_eleven) < 25 || daysidle(rent_eleven) > 4) {
        free_string(rent_eleven);
        rent_eleven = str_dup("");
      }
      else {
        house_charge(rent_eleven, 25);
        offline_message(rent_eleven, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_twelve) > 2) {
      if (bank_check(rent_twelve) < 25 || daysidle(rent_twelve) > 4) {
        free_string(rent_twelve);
        rent_twelve = str_dup("");
      }
      else {
        house_charge(rent_twelve, 25);
        offline_message(rent_twelve, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_thirteen) > 2) {
      if (bank_check(rent_thirteen) < 25 || daysidle(rent_thirteen) > 4) {
        free_string(rent_thirteen);
        rent_thirteen = str_dup("");
      }
      else {
        house_charge(rent_thirteen, 25);
        offline_message(rent_thirteen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_fourteen) > 2) {
      if (bank_check(rent_fourteen) < 25 || daysidle(rent_fourteen) > 4) {
        free_string(rent_fourteen);
        rent_fourteen = str_dup("");
      }
      else {
        house_charge(rent_fourteen, 25);
        offline_message(rent_fourteen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_fifteen) > 2) {
      if (bank_check(rent_fifteen) < 25 || daysidle(rent_fifteen) > 4) {
        free_string(rent_fifteen);
        rent_fifteen = str_dup("");
      }
      else {
        house_charge(rent_fifteen, 25);
        offline_message(rent_fifteen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_sixteen) > 2) {
      if (bank_check(rent_sixteen) < 25 || daysidle(rent_sixteen) > 4) {
        free_string(rent_sixteen);
        rent_sixteen = str_dup("");
      }
      else {
        house_charge(rent_sixteen, 25);
        offline_message(rent_sixteen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_seventeen) > 2) {
      if (bank_check(rent_seventeen) < 25 || daysidle(rent_seventeen) > 4) {
        free_string(rent_seventeen);
        rent_seventeen = str_dup("");
      }
      else {
        house_charge(rent_seventeen, 25);
        offline_message(rent_seventeen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_eighteen) > 2) {
      if (bank_check(rent_eighteen) < 25 || daysidle(rent_eighteen) > 4) {
        free_string(rent_eighteen);
        rent_eighteen = str_dup("");
      }
      else {
        house_charge(rent_eighteen, 25);
        offline_message(rent_eighteen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_nineteen) > 2) {
      if (bank_check(rent_nineteen) < 25 || daysidle(rent_nineteen) > 4) {
        free_string(rent_nineteen);
        rent_nineteen = str_dup("");
      }
      else {
        house_charge(rent_nineteen, 25);
        offline_message(rent_nineteen, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_twenty) > 2) {
      if (bank_check(rent_twenty) < 25 || daysidle(rent_twenty) > 4) {
        free_string(rent_twenty);
        rent_twenty = str_dup("");
      }
      else {
        house_charge(rent_twenty, 25);
        offline_message(rent_twenty, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_twentyone) > 2) {
      if (bank_check(rent_twentyone) < 25 || daysidle(rent_twentyone) > 4) {
        free_string(rent_twentyone);
        rent_twentyone = str_dup("");
      }
      else {
        house_charge(rent_twentyone, 25);
        offline_message(rent_twentyone, "You pay $25 for your room.");
      }
    }
    if (safe_strlen(rent_twentytwo) > 2) {
      if (bank_check(rent_twentytwo) < 25 || daysidle(rent_twentytwo) > 4) {
        free_string(rent_twentytwo);
        rent_twentytwo = str_dup("");
      }
      else {
        house_charge(rent_twentytwo, 25);
        offline_message(rent_twentytwo, "You pay $25 for your room.");
      }
    }

    container_update();
    dorms_update();
  }

  // this compares a vnum to the owner name stored in the dorm variables -
  // Discordance
  bool enclave_room_occupied(int rnum) {

    // House Bishop
    if      (rnum == 11) {if (safe_strlen(enclave_room[0]) > 2) {return TRUE;}}
    else if (rnum == 12) {if (safe_strlen(enclave_room[1]) > 2) {return TRUE;}}
    else if (rnum == 13) {if (safe_strlen(enclave_room[2]) > 2) {return TRUE;}}
    else if (rnum == 14) {if (safe_strlen(enclave_room[3]) > 2) {return TRUE;}}
    else if (rnum == 15) {if (safe_strlen(enclave_room[4]) > 2) {return TRUE;}}
    // House Rook
    else if (rnum == 21) {if (safe_strlen(enclave_room[5]) > 2) {return TRUE;}}
    else if (rnum == 22) {if (safe_strlen(enclave_room[6]) > 2) {return TRUE;}}
    else if (rnum == 23) {if (safe_strlen(enclave_room[7]) > 2) {return TRUE;}}
    else if (rnum == 24) {if (safe_strlen(enclave_room[8]) > 2) {return TRUE;}}
    else if (rnum == 25) {if (safe_strlen(enclave_room[9]) > 2) {return TRUE;}}
    // House Queenson
    else if (rnum == 31) {if (safe_strlen(enclave_room[10]) > 2) {return TRUE;}}
    else if (rnum == 32) {if (safe_strlen(enclave_room[11]) > 2) {return TRUE;}}
    else if (rnum == 33) {if (safe_strlen(enclave_room[12]) > 2) {return TRUE;}}
    else if (rnum == 34) {if (safe_strlen(enclave_room[13]) > 2) {return TRUE;}}
    else if (rnum == 35) {if (safe_strlen(enclave_room[14]) > 2) {return TRUE;}}
    // House Kingson
    else if (rnum == 41) {if (safe_strlen(enclave_room[15]) > 2) {return TRUE;}}
    else if (rnum == 42) {if (safe_strlen(enclave_room[16]) > 2) {return TRUE;}}
    else if (rnum == 43) {if (safe_strlen(enclave_room[17]) > 2) {return TRUE;}}
    else if (rnum == 44) {if (safe_strlen(enclave_room[18]) > 2) {return TRUE;}}
    else if (rnum == 45) {if (safe_strlen(enclave_room[19]) > 2) {return TRUE;}}

    return FALSE;
  }

  bool enclave_roomie_occupied(int rnum) {

    // House Bishop
    if      (rnum == 11) {if (safe_strlen(enclave_room[0 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 12) {if (safe_strlen(enclave_room[1 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 13) {if (safe_strlen(enclave_room[2 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 14) {if (safe_strlen(enclave_room[3 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 15) {if (safe_strlen(enclave_room[4 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    // House Rook
    else if (rnum == 21) {if (safe_strlen(enclave_room[5 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 22) {if (safe_strlen(enclave_room[6 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 23) {if (safe_strlen(enclave_room[7 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 24) {if (safe_strlen(enclave_room[8 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 25) {if (safe_strlen(enclave_room[9 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    // House Queenson
    else if (rnum == 31) {if (safe_strlen(enclave_room[10 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 32) {if (safe_strlen(enclave_room[11 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 33) {if (safe_strlen(enclave_room[12 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 34) {if (safe_strlen(enclave_room[13 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 35) {if (safe_strlen(enclave_room[14 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    // House Kingson
    else if (rnum == 41) {if (safe_strlen(enclave_room[15 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 42) {if (safe_strlen(enclave_room[16 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 43) {if (safe_strlen(enclave_room[17 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 44) {if (safe_strlen(enclave_room[18 + MAX_DORMROOMS]) > 2) {return TRUE;}}
    else if (rnum == 45) {if (safe_strlen(enclave_room[19 + MAX_DORMROOMS]) > 2) {return TRUE;}}

    return FALSE;
  }

  _DOFUN(do_roomie) {
    int rnum = 0;
    int i = 0;
    int house_mod = 0;
    bool found = FALSE;

    if ((ch->in_room == NULL
          ||  (ch->in_room->vnum != 3881   // Bishop
            &&   ch->in_room->vnum != 8996   // Rook
            &&   ch->in_room->vnum != 3894   // Queenson
            &&   ch->in_room->vnum != 9032)) // Kingson
        && str_cmp(argument, "stop")) {
      send_to_char("There's nothing here to rent.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "stop")) {
      for (i = 0; i < MAX_DORMROOMS; i++) {
        if (!str_cmp(ch->name, enclave_room[i + MAX_DORMROOMS])) {
          free_string(enclave_room[i + MAX_DORMROOMS]);
          enclave_room[i + MAX_DORMROOMS] = str_dup("");
          found = TRUE;
        }
      }

      /*
for(i=0;i<MAX_DORMROOMS+MAX_DORMROOMS;i++) { //Diag - Disco
if (enclave_room[i] == NULL) {enclave_room[i] = str_dup("");}
}
*/

      if (found == TRUE) {send_to_char("Done.`x\n\r", ch);}
      else {send_to_char("You weren't rooming with anyone.\n\r", ch);}

      return;
    }
    else {
      //dorm checks
      if (!college_student(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("The dormitory enclave is for students only.\n\r", ch);
        return;
      }
      for (i = 0; i < MAX_DORMROOMS; i++) {
        if (!str_cmp(enclave_room[i], ch->name)) {
          send_to_char("You already have a room.\n\r", ch);
          return;
        }
      }

      for (i = 0; i < MAX_DORMROOMS; i++) {
        if (!str_cmp(enclave_room[i + MAX_DORMROOMS], ch->name)) {
          send_to_char("You are already rooming with someone.\n\r", ch);
          return;
        }
      }

      //house number modifiers
      if      (ch->in_room->vnum == 3881) {house_mod = 10;} // House Bishop
      else if (ch->in_room->vnum == 8996) {house_mod = 20;} // House Rook
      else if (ch->in_room->vnum == 3894) {house_mod = 30;} // House Queen
      else if (ch->in_room->vnum == 9032) {house_mod = 40;} // House King

      // checks if room is owned by another character
      if (is_number(argument)) {
        rnum = atoi(argument);

        if (rnum < 1 || rnum > 8) {
          send_to_char("That room doesn't exist.\n\r", ch);
          return;
        }

        rnum = rnum + house_mod;

        if (!enclave_room_occupied(rnum)) {
          send_to_char("That room is empty.  There's no one to room with.\n\r",ch);
          return;
        }
        else if (enclave_roomie_occupied(rnum)) {
          send_to_char("That room is already full.\n\r", ch);
          return;
        }
        else {
          // House Bishop
          if (rnum == 11) {
            free_string(enclave_room[0 + MAX_DORMROOMS]);
            enclave_room[0 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 12) {
            free_string(enclave_room[1 + MAX_DORMROOMS]);
            enclave_room[1 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 13) {
            free_string(enclave_room[2 + MAX_DORMROOMS]);
            enclave_room[2 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 14) {
            free_string(enclave_room[3 + MAX_DORMROOMS]);
            enclave_room[3 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 15) {
            free_string(enclave_room[4 + MAX_DORMROOMS]);
            enclave_room[4 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          // House Rook
          else if (rnum == 21) {
            free_string(enclave_room[5 + MAX_DORMROOMS]);
            enclave_room[5 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 22) {
            free_string(enclave_room[6 + MAX_DORMROOMS]);
            enclave_room[6 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 23) {
            free_string(enclave_room[7 + MAX_DORMROOMS]);
            enclave_room[7 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 24) {
            free_string(enclave_room[8 + MAX_DORMROOMS]);
            enclave_room[8 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 25) {
            free_string(enclave_room[9 + MAX_DORMROOMS]);
            enclave_room[9 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          // House Queenson
          else if (rnum == 31) {
            free_string(enclave_room[10 + MAX_DORMROOMS]);
            enclave_room[10 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 32) {
            free_string(enclave_room[11 + MAX_DORMROOMS]);
            enclave_room[11 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 33) {
            free_string(enclave_room[12 + MAX_DORMROOMS]);
            enclave_room[12 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 34) {
            free_string(enclave_room[13 + MAX_DORMROOMS]);
            enclave_room[13 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 35) {
            free_string(enclave_room[14 + MAX_DORMROOMS]);
            enclave_room[14 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          // House Kingson
          else if (rnum == 41) {
            free_string(enclave_room[15 + MAX_DORMROOMS]);
            enclave_room[15 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 42) {
            free_string(enclave_room[16 + MAX_DORMROOMS]);
            enclave_room[16 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 43) {
            free_string(enclave_room[17 + MAX_DORMROOMS]);
            enclave_room[17 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 44) {
            free_string(enclave_room[18 + MAX_DORMROOMS]);
            enclave_room[18 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else if (rnum == 45) {
            free_string(enclave_room[19 + MAX_DORMROOMS]);
            enclave_room[19 + MAX_DORMROOMS] = str_dup(ch->name);
          }
          else {
            send_to_char("`cSyntax`g: `Wroomie `g(`Wroom number `g/ `Wstop`g)`x\n\r", ch);
            printf_to_char(ch, "`cAvailable room numbers`g: `W%s%s%s%s%s%s%s%s%s`x\n\r", (enclave_room_occupied(1 + house_mod) && !enclave_roomie_occupied(1 + house_mod)) ? "1": "", (enclave_room_occupied(2 + house_mod) && !enclave_roomie_occupied(2 + house_mod)) ? ", ": "", (enclave_room_occupied(2 + house_mod) && !enclave_roomie_occupied(2 + house_mod)) ? "2": "", (enclave_room_occupied(3 + house_mod) && !enclave_roomie_occupied(3 + house_mod)) ? ", ": "", (enclave_room_occupied(3 + house_mod) && !enclave_roomie_occupied(3 + house_mod)) ? "3": "", (enclave_room_occupied(4 + house_mod) && !enclave_roomie_occupied(4 + house_mod)) ? ", ": "", (enclave_room_occupied(4 + house_mod) && !enclave_roomie_occupied(4 + house_mod)) ? "4": "", (enclave_room_occupied(5 + house_mod) && !enclave_roomie_occupied(5 + house_mod)) ? ", ": "", (enclave_room_occupied(5 + house_mod) && !enclave_roomie_occupied(5 + house_mod)) ? "5": "");
            return;
          }
          printf_to_char(ch, "You get the key for room %d.\n\r",rnum - house_mod);
          return;
        }
      }
      else {
        send_to_char("`cSyntax`g: `Wroomie `g(`Wroom number `g/ `Wstop`g)`x\n\r",ch);
        printf_to_char(ch, "`cAvailable room numbers`g: `W%s%s%s%s%s%s%s%s%s`x\n\r", (enclave_room_occupied(1 + house_mod) && !enclave_roomie_occupied(1 + house_mod)) ? "1": "", (enclave_room_occupied(2 + house_mod) && !enclave_roomie_occupied(2 + house_mod)) ? ", ": "", (enclave_room_occupied(2 + house_mod) && !enclave_roomie_occupied(2 + house_mod)) ? "2": "", (enclave_room_occupied(3 + house_mod) && !enclave_roomie_occupied(3 + house_mod)) ? ", ": "", (enclave_room_occupied(3 + house_mod) && !enclave_roomie_occupied(3 + house_mod)) ? "3": "", (enclave_room_occupied(4 + house_mod) && !enclave_roomie_occupied(4 + house_mod)) ? ", ": "", (enclave_room_occupied(4 + house_mod) && !enclave_roomie_occupied(4 + house_mod)) ? "4": "", (enclave_room_occupied(5 + house_mod) && !enclave_roomie_occupied(5 + house_mod)) ? ", ": "", (enclave_room_occupied(5 + house_mod) && !enclave_roomie_occupied(5 + house_mod)) ? "5": "");
        return;
      }
    }
  }

  _DOFUN(do_rent) {
    int rnum = 0;
    int i = 0;
    int house_mod = 0;
    bool found = FALSE;

    if ((ch->in_room == NULL
          ||  (ch->in_room->vnum != 1651    // Shipping
            &&   ch->in_room->vnum != 3881    // Bishop
            &&   ch->in_room->vnum != 8996    // Rook
            &&   ch->in_room->vnum != 3894    // Queenson
            &&   ch->in_room->vnum != 9032    // Kingson
            &&   ch->in_room->vnum != 15045)) // Hotel Antlers
        && str_cmp(argument, "stop")) {
      send_to_char("There's nothing here to rent.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "stop")) {
      // Hotel ... someday this should be migrated to an array - Discordance
      if      (!str_cmp(ch->name, rent_one))       {free_string(rent_one);rent_one             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_two))       {free_string(rent_two);rent_two             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_three))     {free_string(rent_three);rent_three         = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_four))      {free_string(rent_four);rent_four           = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_five))      {free_string(rent_five);rent_five           = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_six))       {free_string(rent_six);rent_six             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_seven))     {free_string(rent_seven);rent_seven         = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_eight))     {free_string(rent_eight);rent_eight         = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_nine))      {free_string(rent_nine);rent_nine           = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_ten))       {free_string(rent_ten);rent_ten             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_eleven))    {free_string(rent_eleven);rent_eleven       = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_twelve))    {free_string(rent_twelve);rent_twelve       = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_thirteen))  {free_string(rent_thirteen);rent_thirteen   = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_fourteen))  {free_string(rent_fourteen);rent_fourteen   = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_fifteen))   {free_string(rent_fifteen);rent_fifteen     = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_sixteen))   {free_string(rent_sixteen);rent_sixteen     = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_seventeen)) {free_string(rent_seventeen);rent_seventeen = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_eighteen))  {free_string(rent_eighteen);rent_eighteen   = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_nineteen))  {free_string(rent_nineteen);rent_nineteen   = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_twenty))    {free_string(rent_twenty);rent_twenty       = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_twentyone)) {free_string(rent_twentyone);rent_twentyone = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, rent_twentytwo)) {free_string(rent_twentytwo);rent_twentytwo = str_dup("");found = TRUE;}
      // Shipping containers
      else if (!str_cmp(ch->name, cont_one))       {free_string(cont_one);cont_one             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_two))       {free_string(cont_two);cont_two             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_three))     {free_string(cont_three);cont_three         = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_four))      {free_string(cont_four);cont_four           = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_five))      {free_string(cont_five);cont_five           = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_six))       {free_string(cont_six);cont_six             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_seven))     {free_string(cont_seven);cont_seven         = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_eight))     {free_string(cont_eight);cont_eight         = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_nine))      {free_string(cont_nine);cont_nine           = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_ten))       {free_string(cont_ten);cont_ten             = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_eleven))    {free_string(cont_eleven);cont_eleven       = str_dup("");found = TRUE;}
      else if (!str_cmp(ch->name, cont_twelve))    {free_string(cont_twelve);cont_twelve       = str_dup("");found = TRUE;}
      // Dorms
      else {
        for (i = 0; i < MAX_DORMROOMS; i++) {
          if (!str_cmp(ch->name, enclave_room[i]))    {free_string(enclave_room[i]);enclave_room[i]     = str_dup("");found = TRUE;}
        }
      }

      if (found == TRUE) {send_to_char("Done.`x\n\r", ch);}
      else {send_to_char("You weren't renting anything.\n\r", ch);}

      return;
    }

    if (ch->in_room->vnum == 15045) {
      if (ch->money < 2500 && str_cmp(argument, "stop")) {
        send_to_char("Rooms are 25 dollars a night.\n\r", ch);
        return;
      }

      if (!str_cmp(argument, "one") || !str_cmp(argument, "101")) {
        if (safe_strlen(rent_one) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_one);
        rent_one = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 101 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "two") || !str_cmp(argument, "102")) {
        if (safe_strlen(rent_two) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_two);
        rent_two = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 102 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "three") || !str_cmp(argument, "103")) {
        if (safe_strlen(rent_three) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_three);
        rent_three = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 103 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "four") || !str_cmp(argument, "104")) {
        if (safe_strlen(rent_four) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_four);
        rent_four = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 104 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "five") || !str_cmp(argument, "105")) {
        if (safe_strlen(rent_five) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_five);
        rent_five = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 105 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if(!str_cmp(argument, "six") || !str_cmp(argument, "106")) {
        if(safe_strlen(rent_six) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_six);
        rent_six = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room six and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "seven") || !str_cmp(argument, "201")) {
        if (safe_strlen(rent_seven) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_seven);
        rent_seven = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 201 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "eight") || !str_cmp(argument, "202")) {
        if (safe_strlen(rent_eight) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_eight);
        rent_eight = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 202 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "nine") || !str_cmp(argument, "203")) {
        if (safe_strlen(rent_nine) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_nine);
        rent_nine = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 203 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "ten") || !str_cmp(argument, "204")) {
        if (safe_strlen(rent_ten) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_ten);
        rent_ten = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 204 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "eleven") || !str_cmp(argument, "205")) {
        if (safe_strlen(rent_eleven) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_eleven);
        rent_eleven = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 205 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if(!str_cmp(argument, "twelve") || !str_cmp(argument, "206")) {
        if(safe_strlen(rent_twelve) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_twelve);
        rent_twelve = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room twelve and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "thirteen") || !str_cmp(argument, "301")) {
        if (safe_strlen(rent_thirteen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_thirteen);
        rent_thirteen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 301 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "fourteen") || !str_cmp(argument, "302")) {
        if (safe_strlen(rent_fourteen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_fourteen);
        rent_fourteen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 302 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "fifteen") || !str_cmp(argument, "303")) {
        if (safe_strlen(rent_fifteen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_fifteen);
        rent_fifteen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 303 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "sixteen") || !str_cmp(argument, "304")) {
        if (safe_strlen(rent_sixteen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_sixteen);
        rent_sixteen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 304 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "seventeen") || !str_cmp(argument, "305")) {
        if (safe_strlen(rent_seventeen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_seventeen);
        rent_seventeen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 305 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "eighteen") || !str_cmp(argument, "401")) {
        if (safe_strlen(rent_eighteen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_eighteen);
        rent_eighteen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 401 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "nineteen") || !str_cmp(argument, "402")) {
        if (safe_strlen(rent_nineteen) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_nineteen);
        rent_nineteen = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 402 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "twenty") || !str_cmp(argument, "403")) {
        if (safe_strlen(rent_twenty) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_twenty);
        rent_twenty = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 403 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "twentyone") || !str_cmp(argument, "404")) {
        if (safe_strlen(rent_twentyone) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_twentyone);
        rent_twentyone = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 404 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else if (!str_cmp(argument, "twentytwo") || !str_cmp(argument, "405")) {
        if (safe_strlen(rent_twentytwo) > 2) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        free_string(rent_twentytwo);
        rent_twentytwo = str_dup(ch->name);
        ch->money -= 2500;
        send_to_char("You get the key for room 405 and a reminder that check out is at 11AM.\n\r", ch);
      }
      else {
        send_to_char("Rent: 101/102/103/104/105/201/202/203/204/205/301/302/303/304/305/401/402/403/404/405\n\r", ch);
      }

    }
    else if (ch->in_room->vnum == 1651) {
      if (ch->money < 1000 && str_cmp(argument, "stop")) {
        send_to_char("Containers are 10 dollars a night.\n\r", ch);
        return;
      }

      if (!str_cmp(argument, "one") || !str_cmp(argument, "1")) {
        if (safe_strlen(cont_one) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_one);
        cont_one = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container one and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "two") || !str_cmp(argument, "2")) {
        if (safe_strlen(cont_two) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_two);
        cont_two = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container two and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "three") || !str_cmp(argument, "3")) {
        if (safe_strlen(cont_three) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_three);
        cont_three = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container three and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "four") || !str_cmp(argument, "4")) {
        if (safe_strlen(cont_four) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_four);
        cont_four = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container four and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }

      else if (!str_cmp(argument, "five") || !str_cmp(argument, "5")) {
        if (safe_strlen(cont_five) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_five);
        cont_five = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container five and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "six") || !str_cmp(argument, "6")) {
        if (safe_strlen(cont_six) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_six);
        cont_six = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container six and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "seven") || !str_cmp(argument, "7")) {
        if (safe_strlen(cont_seven) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_seven);
        cont_seven = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container seven and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "eight") || !str_cmp(argument, "8")) {
        if (safe_strlen(cont_eight) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_eight);
        cont_eight = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container eight and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "nine") || !str_cmp(argument, "9")) {
        if (safe_strlen(cont_nine) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_nine);
        cont_nine = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container nine and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "ten") || !str_cmp(argument, "10")) {
        if (safe_strlen(cont_ten) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_ten);
        cont_ten = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container ten and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "eleven") || !str_cmp(argument, "11")) {
        if (safe_strlen(cont_eleven) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_eleven);
        cont_eleven = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container eleven and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
      else if (!str_cmp(argument, "twelve") || !str_cmp(argument, "12")) {
        if (safe_strlen(cont_twelve) > 2) {
          send_to_char("That container is already taken.\n\r", ch);
          return;
        }
        free_string(cont_twelve);
        cont_twelve = str_dup(ch->name);
        ch->money -= 1000;
        send_to_char("You get the key for container twelve and a reminder that the bill is due at 11AM every day.\n\r", ch);
      }
    }

    else if (ch->in_room->vnum == 3881 || ch->in_room->vnum == 8996 || ch->in_room->vnum == 3894 || ch->in_room->vnum == 9032) {
      if (!college_student(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("The dorms are for college students.\n\r", ch);
        return;
      }

      //house number mods
      if      (ch->in_room->vnum == 3881) {house_mod = 10;} // House Bishop
      else if (ch->in_room->vnum == 8996) {house_mod = 20;} // House Rook
      else if (ch->in_room->vnum == 3894) {house_mod = 30;} // House Queen
      else if (ch->in_room->vnum == 9032) {house_mod = 40;} // House King

      // checks if room is owned by another character
      if (is_number(argument)) { //  && atoi(argument)>0 && atoi(argument)<=6){
        rnum = atoi(argument);

        if (rnum < 1 || rnum > 5) {
          send_to_char("That room doesn't exist.\n\r", ch);
          return;
        }

        rnum = rnum + house_mod;

        if (enclave_room_occupied(rnum)) {
          send_to_char("That room is already taken.\n\r", ch);
          return;
        }
        else {
          // House Bishop
          if      (rnum == 11) {free_string(enclave_room[0]);enclave_room[0]  = str_dup(ch->name);}
          else if (rnum == 12) {free_string(enclave_room[1]);enclave_room[1]  = str_dup(ch->name);}
          else if (rnum == 13) {free_string(enclave_room[2]);enclave_room[2]  = str_dup(ch->name);}
          else if (rnum == 14) {free_string(enclave_room[3]);enclave_room[3]  = str_dup(ch->name);}
          else if (rnum == 15) {free_string(enclave_room[4]);enclave_room[4]  = str_dup(ch->name);}
          // House Rook
          else if (rnum == 21) {free_string(enclave_room[5]);enclave_room[5]  = str_dup(ch->name);}
          else if (rnum == 22) {free_string(enclave_room[6]);enclave_room[6]  = str_dup(ch->name);}
          else if (rnum == 23) {free_string(enclave_room[7]);enclave_room[7]  = str_dup(ch->name);}
          else if (rnum == 24) {free_string(enclave_room[8]);enclave_room[8]  = str_dup(ch->name);}
          else if (rnum == 25) {free_string(enclave_room[9]);enclave_room[9]  = str_dup(ch->name);}
          // House Queenson
          else if (rnum == 31) {free_string(enclave_room[10]);enclave_room[10] = str_dup(ch->name);}
          else if (rnum == 32) {free_string(enclave_room[11]);enclave_room[11] = str_dup(ch->name);}
          else if (rnum == 33) {free_string(enclave_room[12]);enclave_room[12] = str_dup(ch->name);}
          else if (rnum == 34) {free_string(enclave_room[13]);enclave_room[13] = str_dup(ch->name);}
          else if (rnum == 35) {free_string(enclave_room[14]);enclave_room[14] = str_dup(ch->name);}
          // House Kingson
          else if (rnum == 41) {free_string(enclave_room[15]);enclave_room[15] = str_dup(ch->name);}
          else if (rnum == 42) {free_string(enclave_room[16]);enclave_room[16] = str_dup(ch->name);}
          else if (rnum == 43) {free_string(enclave_room[17]);enclave_room[17] = str_dup(ch->name);}
          else if (rnum == 44) {free_string(enclave_room[18]);enclave_room[18] = str_dup(ch->name);}
          else if (rnum == 45) {free_string(enclave_room[19]);enclave_room[19] = str_dup(ch->name);}
          else {
            send_to_char("`cSyntax`g: `Wrent `g(`Wroom number `g/ `Wstop`g)`x\n\r", ch);
            printf_to_char(ch, "`cAvailable room numbers`g: `W%s%s%s%s%s%s%s%s%s`x\n\r", (!enclave_room_occupied(1 + house_mod)) ? "1" : "", (!enclave_room_occupied(2 + house_mod)) ? ", " : "", (!enclave_room_occupied(2 + house_mod)) ? "2" : "", (!enclave_room_occupied(3 + house_mod)) ? ", " : "", (!enclave_room_occupied(3 + house_mod)) ? "3" : "", (!enclave_room_occupied(4 + house_mod)) ? ", " : "", (!enclave_room_occupied(4 + house_mod)) ? "4" : "", (!enclave_room_occupied(5 + house_mod)) ? ", " : "", (!enclave_room_occupied(5 + house_mod)) ? "5" : "");
            return;
          }
          printf_to_char(ch, "You get the key for room %d.\n\r", rnum - house_mod);
          return;
        }
      }
      else {
        send_to_char("`cSyntax`g: `Wrent `g(`Wroom number `g/ `Wstop`g)`x\n\r", ch);
        printf_to_char(ch, "`cAvailable room numbers`g: `W%s%s%s%s%s%s%s%s%s`x`x\n\r", (!enclave_room_occupied(1 + house_mod)) ? "1" : "", (!enclave_room_occupied(2 + house_mod)) ? ", " : "", (!enclave_room_occupied(2 + house_mod)) ? "2" : "", (!enclave_room_occupied(3 + house_mod)) ? ", " : "", (!enclave_room_occupied(3 + house_mod)) ? "3" : "", (!enclave_room_occupied(4 + house_mod)) ? ", " : "", (!enclave_room_occupied(4 + house_mod)) ? "4" : "", (!enclave_room_occupied(5 + house_mod)) ? ", " : "", (!enclave_room_occupied(5 + house_mod)) ? "5" : "");
        return;
      }
    }
    else {
      send_to_char("`cSyntax`g: `Wrent `g(`Wroom number `g/ `Wstop`g)\n\r", ch);
    }
  }

  bool bblocked(ROOM_INDEX_DATA *to_room, CHAR_DATA *ch) {


    if(to_room->vnum == room_in_school(ch->in_room->vnum) && !clinic_patient(ch)) {return FALSE;}

    if(to_room->vnum == 5102 || to_room->vnum == 9360) {
      if(is_ffamily(ch) && get_tier(ch) > 1) {return FALSE;}
      else {return TRUE;}
    }

    int chouse = college_house_room(to_room);
    if(chouse > 0) {
      if (IS_SET(to_room->room_flags, ROOM_BATHROOM) && room_pop(to_room) > 0)                  {return TRUE;}
      if (IS_SET(to_room->room_flags, ROOM_BEDROOM))                                            {return TRUE;}
      if (college_house(ch) == chouse && !clinic_patient(ch))                                   {return FALSE;}
    }

    if (to_room->vnum     == 3458  && str_cmp(rent_one,                            ch->name)) {return TRUE;}
    if (to_room->vnum     == 3439  && str_cmp(rent_two,                            ch->name)) {return TRUE;}
    if (to_room->vnum     == 3463  && str_cmp(rent_three,                          ch->name)) {return TRUE;}
    if (to_room->vnum     == 3496  && str_cmp(rent_four,                           ch->name)) {return TRUE;}
    if (to_room->vnum     == 3502  && str_cmp(rent_five,                           ch->name)) {return TRUE;}
    if (to_room->vnum     == 12064 && str_cmp(rent_six,                            ch->name)) {return TRUE;}
    if (to_room->vnum     == 3756  && str_cmp(rent_seven,                          ch->name)) {return TRUE;}
    if (to_room->vnum     == 3746  && str_cmp(rent_eight,                          ch->name)) {return TRUE;}
    if (to_room->vnum     == 3736  && str_cmp(rent_nine,                           ch->name)) {return TRUE;}
    if (to_room->vnum     == 3740  && str_cmp(rent_ten,                            ch->name)) {return TRUE;}
    if (to_room->vnum     == 3708  && str_cmp(rent_eleven,                         ch->name)) {return TRUE;}
    if (to_room->vnum     == 12102 && str_cmp(rent_twelve,                         ch->name)) {return TRUE;}
    if (to_room->vnum     == 7417  && str_cmp(rent_thirteen,                       ch->name)) {return TRUE;}
    if (to_room->vnum     == 7310  && str_cmp(rent_fourteen,                       ch->name)) {return TRUE;}
    if (to_room->vnum     == 6965  && str_cmp(rent_fifteen,                        ch->name)) {return TRUE;}
    if (to_room->vnum     == 6827  && str_cmp(rent_sixteen,                        ch->name)) {return TRUE;}
    if (to_room->vnum     == 6619  && str_cmp(rent_seventeen,                      ch->name)) {return TRUE;}
    if (to_room->vnum     == 8450  && str_cmp(rent_eighteen,                       ch->name)) {return TRUE;}
    if (to_room->vnum     == 8009  && str_cmp(rent_nineteen,                       ch->name)) {return TRUE;}
    if (to_room->vnum     == 7907  && str_cmp(rent_twenty,                         ch->name)) {return TRUE;}
    if (to_room->vnum     == 7766  && str_cmp(rent_twentyone,                      ch->name)) {return TRUE;}
    if (to_room->vnum     == 7712  && str_cmp(rent_twentytwo,                      ch->name)) {return TRUE;}
    // containers
    if ((to_room->vnum == 16351 || to_room->vnum == 1665)  && str_cmp(cont_one,    ch->name)) {return TRUE;}
    if ((to_room->vnum == 16350 || to_room->vnum == 16348) && str_cmp(cont_two,    ch->name)) {return TRUE;}
    if ((to_room->vnum == 16313 || to_room->vnum == 16347) && str_cmp(cont_three,  ch->name)) {return TRUE;}
    if ((to_room->vnum == 16314 || to_room->vnum == 16345) && str_cmp(cont_four,   ch->name)) {return TRUE;}
    if ((to_room->vnum == 16315 || to_room->vnum == 16343) && str_cmp(cont_five,   ch->name)) {return TRUE;}
    if ((to_room->vnum == 1536  || to_room->vnum == 1659)  && str_cmp(cont_six,    ch->name)) {return TRUE;}
    if ((to_room->vnum == 16326 || to_room->vnum == 1657)  && str_cmp(cont_seven,  ch->name)) {return TRUE;}
    if ((to_room->vnum == 1590  || to_room->vnum == 16340) && str_cmp(cont_eight,  ch->name)) {return TRUE;}
    if ((to_room->vnum == 16325 || to_room->vnum == 16336) && str_cmp(cont_nine,   ch->name)) {return TRUE;}
    if ((to_room->vnum == 16327 || to_room->vnum == 16337) && str_cmp(cont_ten,    ch->name)) {return TRUE;}
    if ((to_room->vnum == 16330 || to_room->vnum == 16334) && str_cmp(cont_eleven, ch->name)) {return TRUE;}
    if ((to_room->vnum == 16331 || to_room->vnum == 16332) && str_cmp(cont_twelve, ch->name)) {return TRUE;}

    if (!IS_IMMORTAL(ch)) {
      // House Bishop
      if      (to_room->vnum == 3341 && (str_cmp(enclave_room[0],  ch->name)
      && str_cmp(enclave_room[0 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 9017 && (str_cmp(enclave_room[1],  ch->name)
            && str_cmp(enclave_room[1 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 9016 && (str_cmp(enclave_room[2],  ch->name)
            && str_cmp(enclave_room[2 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 9003 && (str_cmp(enclave_room[3],  ch->name)
            && str_cmp(enclave_room[3 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 9004 && (str_cmp(enclave_room[4],  ch->name)
            && str_cmp(enclave_room[4 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      // House Rook
      else if (to_room->vnum == 8997 && (str_cmp(enclave_room[5],  ch->name)
            && str_cmp(enclave_room[5 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 8998 && (str_cmp(enclave_room[6],  ch->name)
            && str_cmp(enclave_room[6 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 8999 && (str_cmp(enclave_room[7],  ch->name)
            && str_cmp(enclave_room[7 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 9006 && (str_cmp(enclave_room[8],  ch->name)
            && str_cmp(enclave_room[8 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      else if (to_room->vnum == 9014 && (str_cmp(enclave_room[9],  ch->name)
            && str_cmp(enclave_room[9 + MAX_DORMROOMS], ch->name))) {return TRUE;}
      // House Queenson
      else if (to_room->vnum == 9049 && (str_cmp(enclave_room[10], ch->name)
            && str_cmp(enclave_room[10 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9050 && (str_cmp(enclave_room[11], ch->name)
            && str_cmp(enclave_room[11 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 5951 && (str_cmp(enclave_room[12], ch->name)
            && str_cmp(enclave_room[12 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9037 && (str_cmp(enclave_room[13], ch->name)
            && str_cmp(enclave_room[13 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9038 && (str_cmp(enclave_room[14], ch->name)
            && str_cmp(enclave_room[14 + MAX_DORMROOMS], ch->name))){return TRUE;}
      // House Kingson
      else if (to_room->vnum == 9035 && (str_cmp(enclave_room[15], ch->name)
            && str_cmp(enclave_room[15 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9034 && (str_cmp(enclave_room[16], ch->name)
            && str_cmp(enclave_room[16 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9033 && (str_cmp(enclave_room[17], ch->name)
            && str_cmp(enclave_room[17 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9036 && (str_cmp(enclave_room[18], ch->name)
            && str_cmp(enclave_room[18 + MAX_DORMROOMS], ch->name))){return TRUE;}
      else if (to_room->vnum == 9042 && (str_cmp(enclave_room[19], ch->name)
            && str_cmp(enclave_room[19 + MAX_DORMROOMS], ch->name))){return TRUE;}
    }

    return FALSE;
  }

  // Creates a cardboard box and places the item inside it - Discordance
  _DOFUN(do_package) {
    char arg1[MSL];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    OBJ_DATA *parcel;
    int level = ch->level;
    int amount = 100;

    argument = one_argument(argument, arg1);

    // checking for argument
    if (arg1[0] == '\0') {
      send_to_char("`gSyntax`x: `Wpackage `g(`Wparcel`g)`x\n\r", ch);
      return;
    }

    // checking for money
    if (ch->money < amount) {
      send_to_char("You don't have that much money.\n\r", ch);
      return;
    }

    // checking for item
    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg1, obj->name)) {
      if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg1, obj->name)) {
        if ((obj = get_obj_carry(ch, arg1, ch)) == NULL) {
          send_to_char("You do not have that item.\n\r", ch);
          return;
        }
      }
    }

    // eating money
    ch->money -= amount;

    // spawning container and setting up properties
    pObjIndex = get_obj_index(28200);
    parcel = create_object(pObjIndex, level);
    setdescs(parcel);
    // resizing box
    parcel->size = obj->size + 1;
    parcel->value[0] = obj->size; // capacity
    parcel->value[3] = obj->size; // biggest object
    obj_to_char(parcel, ch);

    // putting obj in container
    obj_from_char(obj);
    obj_to_obj(obj, parcel);

    act("$n packages up $p.", ch, obj, NULL, TO_ROOM);
    act("You package up $p.", ch, obj, NULL, TO_CHAR);
  }

  // Guided addressing of packages - Discordance
  _DOFUN(do_address) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char buf[MSL];
    OBJ_DATA *parcel;
    bool found = FALSE;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    strcpy(arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
      send_to_char("`gSyntax`x: `Waddress `g(`Wparcel`g) (`Wperson`g) (`Waddress`g)`x\n\r", ch);
      return;
    }

    if ((parcel = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg1, parcel->name)) {
      if ((parcel = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg1, parcel->name)) {
        if ((parcel = get_obj_carry(ch, arg1, ch)) == NULL) {
          send_to_char("You do not have that item.\n\r", ch);
          return;
        }
      }
    }

    if (parcel->item_type != ITEM_CONTAINER) {
      send_to_char("That needs to be packaged before addressing it.\n\r", ch);
      return;
    }

    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if (!(*it)->house_name || (*it)->house_name[0] == '\0' || (*it)->auction_day > 35 || (*it)->auction_day < 0 || (*it)->auction_month < 1 || (*it)->auction_month > 12) {
        continue;
      }
      if (!str_cmp((*it)->address, arg3)) {
        found = TRUE;
      }
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) {
        continue;
      }
      if (!str_cmp((*it)->address, arg3)) {
        found = TRUE;
      }
    }

    if (found == FALSE) {
      send_to_char("That address doesn't exist.`x\n\r", ch);
      send_to_char("`gSyntax`x: `Waddress `g(`Wparcel`g) (`Wperson`g) (`Waddress`g)`x\n\r", ch);
      return;
    }
    else {
      sprintf(buf, "It's addressed:\n%s\n%s\n", capitalize(arg2), arg3);
      free_string(parcel->description);
      parcel->description = str_dup(buf);
    }
  }

  /*
//Need to check that address is included at least.  Check who is for upon
delivery.  Assume current resident if none. _DOFUN(do_mail) { char arg1 [MSL];
OBJ_DATA  *parcel;
int postage;
bool found=FALSE;

argument = one_argument( argument, arg1 );

//checks item is on character
if ( (parcel = get_eq_char( ch, WEAR_HOLD )) == NULL || !is_name(arg1, parcel->name)) { if ( (parcel = get_eq_char( ch, WEAR_HOLD_2 )) == NULL || !is_name(arg1, parcel->name)) if ( ( parcel = get_obj_carry( ch, arg1, ch ) ) ==
NULL     ) { send_to_char( "You do not have that item.\n\r", ch ); return;
}
}

//checks item is a container; looks for address on container that matches a
real address if(parcel->item_type == ITEM_CONTAINER) {
//loops through stock houses
for(vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();it !=
HouseVect.end();++it) { if(!(*it)->house_name || (*it)->house_name[0] == '\0' || (*it)->auction_day > 35 || (*it)->auction_day < 0
||(*it)->auction_month < 1 || (*it)->auction_month > 12) {
continue;
}
if (!str_cmp((*it)->address, parcel->material)) {
found=TRUE;
}

}
//loops through player created properties
for(vector<PROP_TYPE *>::iterator it = PropVect.begin();it !=
PropVect.end();++it) { if(!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) { continue;
}
if (!str_cmp((*it)->address, parcel->material)) {
found=TRUE;
}
}
//change this to allow bad addresses
if(found==FALSE) {
send_to_char( "The address on this package is invalid.\n\r", ch );
return;
}
}
else {
send_to_char( "That needs to be packaged prior to shipping.\n\r", ch );
return;
}


if (parcel->size > 100) {
postage=ceil(parcel->size-100/5*.10+1720);
}
else if (parcel->size > 85) {
postage=1720;
}
else if (parcel->size > 80) {
postage=1711;
}
else if (parcel->size > 75) {
postage=1701;
}
else if (parcel->size > 70) {
postage=1695;
}
else if (parcel->size > 65) {
postage=1625;
}
else if (parcel->size > 60) {
postage=1535;
}
else if (parcel->size > 55) {
postage=1445;
}
else if (parcel->size > 50) {
postage=1350;
}
else if (parcel->size > 45) {
postage=1285;
}
else if (parcel->size > 40) {
postage=1185;
}
else if (parcel->size > 35) {
postage=1110;
}
else if (parcel->size > 30) {
postage=1035;
}
else if (parcel->size > 25) {
postage=975;
}
else if (parcel->size > 20) {
postage=885;
}
else if (parcel->size > 15) {
postage=745;
}
else if (parcel->size > 10) {
postage=660;
}
else if (parcel->size > 5) {
postage=580;
}
else {
postage=560;
}

if( ch->money < postage ) {
send_to_char("You don't have enough money for postage.\n\r", ch);
return;
}
else {

//        need catch for using mailroom at apartment building

if ((!has_mailbox(ch->in_room))) {
send_to_char("There's no mailbox here.\n\r", ch);
return;
}
//this needs to account for other residents of property yet
else if(in_prop(ch)!=NULL && (in_prop(ch))->owner != ch) {
send_to_char("There isn't your mailbox.\n\r", ch);
return;
}

ch->money = ch->money - postage;

if(obj->wear_loc != WEAR_NONE) {
unequip_char(ch, obj);
}
act("$n mails $a $p.",ch,obj,NULL, TO_ROOM);
act("You mail $a $p.",ch,obj,NULL, TO_CHAR);

obj->stash_room=28203; //Moving to special postoffice room "Haven Post Office
- Outgoing Mail" SET_BIT(obj->extra_flags,ITEM_WARDROBE);
}
}

//This picks packages up from a location and will be called by the courier
subroutine.  Should have courier hit each property over time throughout day for
standard delivery hours. void pickup(CHAR_DATA *ch) {
//search through properties and check rooms of each for mailbox flag.  If
found, check stash in room for each resident and mail any packages within
}


void process_adressee (OBJ_DATA *parcel) {

}

//This handles the actual delivery to an address - Discordance
void delivery(CHAR_DATA *ch, OBJ_DATA *parcel) {
bool found=FALSE;
OBJ_DATA *parcel;

argument = one_argument_nouncap( argument, name);



//    need to set address being looked for somewhere


for(vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();it !=
HouseVect.end();++it) { if(!(*it)->house_name || (*it)->house_name[0] == '\0' || (*it)->auction_day > 35 || (*it)->auction_day < 0
||(*it)->auction_month < 1 || (*it)->auction_month > 12) {
continue;
}
if (!str_cmp((*it)->address, to)) {
//if character lives at address
adressee=process_addressee(parcel);


//give obj to character and stick obj in stash at address
number = number_argument( arg1, arg );
<<<<<<< HEAD
count  = 0;
for ( parcel = ch->carrying; parcel != NULL; parcel =
parcel->next_content ) {
=======
count  = 0;
for ( parcel = ch->carrying; parcel != NULL; parcel = parcel->next_content ) {
>>>>>>> d304e7fdbc0592c1406ae0bfaca4809c9f99b4f2
if(parcel->stash_room != postoffice)
continue;

if (is_name( arg, parcel->name ) && IS_SET(parcel->extra_flags, ITEM_WARDROBE)) { if ( ++count == number ) {
REMOVE_BIT(parcel->extra_flags,ITEM_WARDROBE);
return;
}
}
<<<<<<< HEAD
}
=======
}
>>>>>>> d304e7fdbc0592c1406ae0bfaca4809c9f99b4f2


//            need to check if character online and load if not

obj_from_char(parcel);
obj_to_char(parcel, addressee);
found=TRUE;
break;
}
}
for(vector<PROP_TYPE *>::iterator it = PropVect.begin();it !=
PropVect.end();++it) { if(!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE || (*it)->type != PROP_HOUSE) { continue;
}
if (!str_cmp((*it)->address, to)) {
//if character lives at address, else owner
adressee=process_addressee(parcel);
//give obj to character and stick obj in stash at address
number = number_argument( arg1, arg );
<<<<<<< HEAD
count  = 0;
for ( parcel = ch->carrying; parcel != NULL; parcel =
parcel->next_content ) {
=======
count  = 0;
for ( parcel = ch->carrying; parcel != NULL; parcel = parcel->next_content ) {
>>>>>>> d304e7fdbc0592c1406ae0bfaca4809c9f99b4f2
if(parcel->stash_room != postoffice)
continue;

if (is_name( arg, parcel->name ) && IS_SET(parcel->extra_flags, ITEM_WARDROBE)) { if ( ++count == number ) {
REMOVE_BIT(parcel->extra_flags,ITEM_WARDROBE);
<<<<<<< HEAD
act("You get $a $p from your stash.", ch, parcel, NULL, TO_CHAR); return;
}
}
}
=======
act("You get $a $p from your stash.", ch, parcel, NULL, TO_CHAR);
return;
}
}
}
>>>>>>> d304e7fdbc0592c1406ae0bfaca4809c9f99b4f2

//           need to check if character online and load if not

obj_from_char(parcel);
obj_to_char(parcel, addressee);
found=TRUE;
break;
}
}
if(found==FALSE) {
//send package back to sender
}

return;
}
*/

  bool shop_owner(CHAR_DATA *ch) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if ((*it)->type != PROP_SHOP)
      continue;
      if (!str_cmp((*it)->owner, ch->name)) {
        if (!(*it)->propname || (*it)->propname[0] == '\0') {
          continue;
        }
        return TRUE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_housesim) {

    if (!str_cmp(argument, "vnum")) {
      int i = first_free_property();
      printf_to_char(ch, "Vnum: %d\n\r", i);
      return;
    }
    if (!str_cmp(argument, "list")) {
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!(*it)->propname || (*it)->propname[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        printf_to_char(ch, "%d-%d, ", (*it)->lower_vnum, (*it)->higher_vnum);
      }
      send_to_char("\n\r", ch);
    }
    if (!str_cmp(argument, "test")) {
      int i = first_free_property();
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!(*it)->propname || (*it)->propname[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        for (int j = i; j < i + 60; j++) {
          if (j >= (*it)->lower_vnum && j <= (*it)->higher_vnum)
          printf_to_char(ch, "Error %d\n\r", j);
        }
      }
    }
    if (!str_cmp(argument, "overlap")) {
      for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
      it != PropVect.end(); ++it) {
        if (!(*it)->propname || (*it)->propname[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        for (vector<PROP_TYPE *>::iterator ij = PropVect.begin();
        ij != PropVect.end(); ++ij) {
          if (!(*ij)->propname || (*ij)->propname[0] == '\0') {
            continue;
          }
          if ((*ij)->valid == FALSE)
          continue;
          if ((*ij)->vnum == (*it)->vnum && (*ij)->type == (*it)->type)
          continue;

          for (int i = (*it)->lower_vnum; i <= (*it)->higher_vnum; i++) {
            if (i >= (*ij)->lower_vnum && i <= (*ij)->higher_vnum)
            printf_to_char(ch, "Overlap %d\n\r", i);
          }
        }
      }
    }
  }

  _DOFUN(do_idlecheck) {
    printf_to_char(ch, "Days Idle: %d\n\r", daysidle(argument));
    int minoffline;

    if (ch->activeat > 0)
    minoffline = (current_time - ch->activeat) / 60;
    else
    minoffline = (current_time - ch->lastlogoff) / 60;

    printf_to_char(ch, "Minoffline: %d\n\r", minoffline);
  }

  void blackout_update(void) {
    for (vector<HOUSE_TYPE *>::iterator it = HouseVect.begin();
    it != HouseVect.end(); ++it) {
      if ((*it)->blackout == 1)
      (*it)->blackout = -1;
      else if ((*it)->blackout == -20)
      (*it)->blackout = 0;
      else if ((*it)->blackout != 0)
      (*it)->blackout--;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if ((*it)->blackout == 1)
      (*it)->blackout = -1;
      else if ((*it)->blackout == -20)
      (*it)->blackout = 0;
      else if ((*it)->blackout != 0)
      (*it)->blackout--;
    }
  }

  void add_pvnum(PROP_TYPE *prop, int value) {
    int i;
    for (i = 0; i < 200 && prop->objects[i] != 0; i++) {
    }
    prop->objects[i] = value;
  }
  int max_objects(PROP_TYPE *prop) {
    int i;

    for (i = 0; i < 200; i++) {
      if (prop->objects[i] == 0)
      return i - 1;
    }
    return 199;
  }

  void notice(CHAR_DATA *ch, PROP_TYPE *prop) {
    CHAR_DATA *victim;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (!IS_FLAG(victim->act, PLR_NOTICED))
      SET_FLAG(victim->act, PLR_NOTICED);
    }
    CHAR_DATA *to;
    for (DescList::iterator itt = descriptor_list.begin();
    itt != descriptor_list.end(); ++itt) {
      DESCRIPTOR_DATA *d = *itt;
      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;
        if (to->in_room == NULL)
        continue;
        if (prop == NULL || !room_in_prop(prop, to->in_room)) {
          if (get_dist(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(to->in_room), get_roomy(to->in_room)) <= 5) {
            if (!IS_FLAG(to->act, PLR_NOTICED))
            SET_FLAG(to->act, PLR_NOTICED);
          }
        }
      }
    }
  }

  _DOFUN(do_tyrissexy) {
    ROOM_INDEX_DATA *doorway = ch->in_room;
    int direction = DIR_SOUTH;
    bool riverside = FALSE;
    bool downtown = FALSE;
    bool oceanview = FALSE;

    if (get_roomy(doorway) == 492 && direction == DIR_NORTH)
    riverside = TRUE;

    if (get_roomy(doorway) == 518 && get_roomx(doorway) > 475 && direction == DIR_SOUTH)
    riverside = TRUE;

    if (get_roomy(doorway) > 494 && get_roomy(doorway) < 515)
    riverside = TRUE;

    if (get_roomx(doorway) <= 493)
    oceanview = TRUE;
    if (get_roomy(doorway) <= 449)
    oceanview = TRUE;

    if (get_roomx(doorway) <= 550 && get_roomx(doorway) >= 458 && get_roomy(doorway) <= 492 && get_roomy(doorway) > 445)
    downtown = TRUE;
    int mult = 100;

    if (downtown == TRUE)
    mult *= 2;
    if (riverside == TRUE)
    mult = mult * 2;
    if (oceanview == TRUE)
    mult = mult * 2;

    if (downtown == TRUE)
    send_to_char("Down", ch);
    if (riverside == TRUE)
    send_to_char("River", ch);
    if (oceanview == TRUE)
    send_to_char("Ocean", ch);

    printf_to_char(ch, "%d", mult);
  }

  void room_type_switch(ROOM_INDEX_DATA *pRoom, int type) {
    RESET_DATA *pReset = NULL;
    while (pRoom->reset_first) {
      pReset = pRoom->reset_first;
      pRoom->reset_first = pRoom->reset_first->next;
      if (!pRoom->reset_first)
      pRoom->reset_last = NULL;
    }
    if (pReset != NULL)
    free_reset_data(pReset);

    if (type == PROP_HOUSE) {
      if (pRoom->sector_type == SECT_COMMERCIAL)
      pRoom->sector_type = SECT_HOUSE;
    }

    if (type == PROP_SHOP) {
      if (pRoom->sector_type == SECT_HOUSE)
      pRoom->sector_type = SECT_COMMERCIAL;
    }
    if (type == 10) {
      if (IS_SET(pRoom->room_flags, ROOM_LIGHTON)) {
        REMOVE_BIT(pRoom->room_flags, ROOM_LIGHTON);
        SET_BIT(pRoom->room_flags, ROOM_LIGHTOFF);
      }
    }
  }
  void prop_type_switch(PROP_TYPE *prop, int type) {
    ROOM_INDEX_DATA *first = first_room_in_property(prop);
    for (; first != NULL; first = next_room_in_property(prop, first)) {
      room_type_switch(first, type);
    }
  }

  int lightcount(PROP_TYPE *prop) {
    int count = 0;
    ROOM_INDEX_DATA *start = first_room_in_property(prop);
    if (start == NULL)
    return 0;
    for (int i = 0; i < 300; i++) {
      if (IS_SET(start->room_flags, ROOM_LIGHTON))
      count++;
      start = next_room_in_property(prop, start);
      if (start == NULL)
      return count;
    }
    return count;
  }

  PROP_TYPE *get_base(int faction) { return NULL; }

  bool is_base(PROP_TYPE *prop) { return FALSE; }

  int worker_count(PROP_TYPE *prop) {
    int count = 0;
    for (int i = 0; i < 20; i++) {
      if (safe_strlen(prop->workers[i]) > 1)
      count++;
    }
    return count;
  }

  char *job_name(CHAR_DATA *ch, int number) {
    int type, vnum;
    char buf[MSL];
    if (number == 1) {
      type = ch->pcdata->job_type_one;
      vnum = ch->pcdata->job_room_one;
    }
    else {
      type = ch->pcdata->job_type_two;
      vnum = ch->pcdata->job_room_two;
    }
    PROP_TYPE *prop = prop_from_room(get_room_index(vnum));

    if (type == JOB_PARTCOMMUTE) {
      if (number == 1)
      sprintf(buf, "A part time commuting job as %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "A part time commuting job as %s.", ch->pcdata->job_title_two);
    }
    else if (type == JOB_FULLCOMMUTE) {
      if (number == 1)
      sprintf(buf, "A full time commuting job as %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "A full time commuting job as %s.", ch->pcdata->job_title_two);
    }
    else if (type == JOB_PARTSTUDENT) {
      if (number == 1)
      sprintf(buf, "Studying part time as %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "Studying part time as %s.", ch->pcdata->job_title_two);
    }
    else if (type == JOB_FULLSTUDENT) {
      if (number == 1)
      sprintf(buf, "Studying full time as %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "Studying full time as %s.", ch->pcdata->job_title_two);
    }
    else if (type == JOB_COVER) {
      if (prop == NULL)
      return "";
      if (number == 1)
      sprintf(buf, "Working full time as %s at %s.", ch->pcdata->job_title_one, prop->propname);
      else
      sprintf(buf, "Working full time as %s at %s.", ch->pcdata->job_title_two, prop->propname);
    }
    else if (type == JOB_PARTEMPLOY) {
      if (prop == NULL)
      return "";
      if (number == 1)
      sprintf(buf, "Working part time as %s at %s.", ch->pcdata->job_title_one, prop->propname);
      else
      sprintf(buf, "Working part time as %s at %s.", ch->pcdata->job_title_two, prop->propname);
    }
    else if (type == JOB_FULLEMPLOY) {
      if (prop == NULL)
      return "";
      if (number == 1)
      sprintf(buf, "Working full time as %s at %s.", ch->pcdata->job_title_one, prop->propname);
      else
      sprintf(buf, "Working full time as %s at %s.", ch->pcdata->job_title_two, prop->propname);
    }
    else if (type == JOB_CLINIC) {
      if (number == 1)
      sprintf(buf, "Working at the Institute Clinic as %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "Working at the Institute Clinic as %s.", ch->pcdata->job_title_two);
    }
    else if (type == JOB_COLLEGE) {
      if (number == 1)
      sprintf(buf, "Working at the Institute College as %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "Working at the Institute College as %s.", ch->pcdata->job_title_two);
    }
    else if (type == JOB_EMPLOYEE) {
      if (number == 1)
      sprintf(buf, "Working as the employee of %s.", ch->pcdata->job_title_one);
      else
      sprintf(buf, "Working as the employee of %s.", ch->pcdata->job_title_two);
    }
    else
    return "";

    return str_dup(buf);
  }
  int available_jobs(PROP_TYPE *prop) {

    if (prop->type == PROP_HOUSE)
    return 5;

    int jobs = industry_table[prop->industry].jobs;
    return jobs;
  }

  _DOFUN(do_job) {
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "take")) {
      argument = one_argument_nouncap(argument, arg2);
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: job take (parttime/fulltime/parttimestudent/fulltimestudent/parttimecommute/fulltimecommute/college/clinic/employee (job name)\n\r", ch);
        return;
      }
      if (ch->pcdata->job_type_two == JOB_UNEMPLOYED) {
        free_string(ch->pcdata->job_title_two);
        ch->pcdata->job_title_two = str_dup("");
      }
      if (ch->pcdata->job_type_one == JOB_UNEMPLOYED) {
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup("");
      }
      if (!str_cmp(arg2, "parttimecommute")) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          if (safe_strlen(ch->pcdata->job_title_two) > 1) {
            send_to_char("Quit your job first.\n\r", ch);
            return;
          }
          free_string(ch->pcdata->job_title_two);
          ch->pcdata->job_title_two = str_dup(argument);
          ch->pcdata->job_type_two = JOB_PARTCOMMUTE;
          printf_to_char(ch, "You take a part time commuting job as %s.\n\r", ch->pcdata->job_title_two);
          return;
        }
        else {
          free_string(ch->pcdata->job_title_one);
          ch->pcdata->job_title_one = str_dup(argument);
          ch->pcdata->job_type_one = JOB_PARTCOMMUTE;
          printf_to_char(ch, "You take a part time commuting job as %s.\n\r", ch->pcdata->job_title_two);
          return;
        }
      }
      else if (!str_cmp(arg2, "fulltimecommute")) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          send_to_char("Quit your job first.\n\r", ch);
          return;
        }
        if (college_student(ch, TRUE)) {
          send_to_char("You're presently studying full time.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup(argument);
        ch->pcdata->job_type_one = JOB_FULLCOMMUTE;
        printf_to_char(ch, "You take a full time commuting job as %s.\n\r", ch->pcdata->job_title_two);
        return;
      }
      else if (!str_cmp(arg2, "clinic") && (ch->race == RACE_FACULTY || (clinic_staff(ch, TRUE)))) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          send_to_char("Quit your job first.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup(argument);
        ch->pcdata->job_type_one = JOB_CLINIC;
        printf_to_char(
        ch, "You take a job working for the institute clinic as %s.\n\r", ch->pcdata->job_title_one);
        return;
      }
      else if (!str_cmp(arg2, "college") && (ch->race == RACE_FACULTY || (college_staff(ch, TRUE)))) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          send_to_char("Quit your job first.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup(argument);
        ch->pcdata->job_type_one = JOB_COLLEGE;
        printf_to_char(ch, "You take a job working for the college as %s.\n\r", ch->pcdata->job_title_one);
        return;
      }
      else if (!str_cmp(arg2, "employee")) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          send_to_char("Quit your job first.\n\r", ch);
          return;
        }
        CHAR_DATA *victim = get_char_room(ch, NULL, argument);
        if (victim == NULL || IS_NPC(victim)) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        if (ch->pcdata->account == NULL || victim->pcdata->account == NULL || (!are_friends(ch, victim) && ch->pcdata->account != victim->pcdata->account)) {
          send_to_char("You can only become an employee of your own characters or characters you are friends with.\n\r", ch);
          return;
        }
        if (get_tier(ch) >= get_tier(victim)) {
          send_to_char("They must be higher tier than you.\n\r", ch);
          return;
        }
        if (has_employees(ch->pcdata->account)) {
          send_to_char("You already have an employee PC.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup(victim->name);
        ch->pcdata->job_type_one = JOB_EMPLOYEE;
        printf_to_char(ch, "You take a job working for %s.\n\r", ch->pcdata->job_title_one);
        return;
      }
      else if (!str_cmp(arg2, "parttimestudent")) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          if (safe_strlen(ch->pcdata->job_title_two) > 1) {
            send_to_char("Quit your job first.\n\r", ch);
            return;
          }
          free_string(ch->pcdata->job_title_two);
          ch->pcdata->job_title_two = str_dup(argument);
          ch->pcdata->job_type_two = JOB_PARTSTUDENT;
          printf_to_char(ch, "You start studying part time as %s.\n\r", ch->pcdata->job_title_two);
          return;
        }
        else {
          free_string(ch->pcdata->job_title_one);
          ch->pcdata->job_title_one = str_dup(argument);
          ch->pcdata->job_type_one = JOB_PARTSTUDENT;
          printf_to_char(ch, "You start studying part time as %s.\n\r", ch->pcdata->job_title_two);
          return;
        }
      }
      else if (!str_cmp(arg2, "fulltimestudent")) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          send_to_char("Quit your job first.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup(argument);
        ch->pcdata->job_type_one = JOB_FULLSTUDENT;
        printf_to_char(ch, "You start studying fully time as %s.\n\r", ch->pcdata->job_title_two);
        return;
      }
      else if (!str_cmp(arg2, "cover")) {
        send_to_char("Cover jobs do not exist any longer.\n\r", ch);
      }
      else if (!str_cmp(arg2, "fulltime")) {
        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          send_to_char("Quit your job first.\n\r", ch);
          return;
        }
        if (college_student(ch, TRUE)) {
          send_to_char("You're presently studying full time.\n\r", ch);
          return;
        }

        PROP_TYPE *prop = prop_from_room(ch->in_room);
        if (prop == NULL) {
          send_to_char("You have to be in a shop or house first.\n\r", ch);
          return;
        }
        if (worker_count(prop) >= available_jobs(prop)) {
          send_to_char("This business isn't currently accepting applications.\n\r", ch);
          return;
        }
        if (prop->type == PROP_HOUSE && !has_paccess(ch, ch->in_room)) {
          send_to_char("You'll need to get the keys to this house first.\n\r", ch);
          return;
        }
        for (int x = 0; x < 20; x++) {
          if (safe_strlen(prop->workers[x]) < 2) {
            free_string(prop->workers[x]);
            prop->workers[x] = str_dup(ch->name);
            free_string(prop->worker_position[x]);
            prop->worker_position[x] = str_dup(argument);
            prop->worker_idle[x] = 0;
            free_string(ch->pcdata->job_title_one);
            ch->pcdata->job_title_one = str_dup(argument);
            ch->pcdata->job_type_one = JOB_FULLEMPLOY;
            ch->pcdata->job_room_one = ch->in_room->vnum;
            ch->pcdata->job_delay_one = current_time;
            printf_to_char(ch, "You take a full time job as %s.\n\r", ch->pcdata->job_title_one);
            if (str_cmp(prop->owner, ch->name)) {
              sprintf(buf, "%s takes a full time job at %s as %s.", ch->name, prop->propname, ch->pcdata->job_title_one);
              message_to_char(prop->owner, buf);
            }
            return;
          }
        }
      }
      else if (!str_cmp(arg2, "parttime")) {

        if (safe_strlen(ch->pcdata->job_title_one) > 1) {
          if (safe_strlen(ch->pcdata->job_title_two) > 1) {
            send_to_char("Quit your job first.\n\r", ch);
            return;
          }
        }
        PROP_TYPE *prop = prop_from_room(ch->in_room);
        if (prop == NULL) {
          send_to_char("You have to be in a shop or house first.\n\r", ch);
          return;
        }
        if (worker_count(prop) >= available_jobs(prop)) {
          send_to_char("This business isn't currently accepting applications.\n\r", ch);
          return;
        }
        if (prop->type == PROP_HOUSE && !has_paccess(ch, ch->in_room)) {
          send_to_char("You'll need to get the keys to this house first.\n\r", ch);
          return;
        }
        for (int x = 0; x < 20; x++) {
          if (safe_strlen(prop->workers[x]) < 2) {
            free_string(prop->workers[x]);
            prop->workers[x] = str_dup(ch->name);
            free_string(prop->worker_position[x]);
            prop->worker_position[x] = str_dup(argument);
            prop->worker_idle[x] = 0;
            if (safe_strlen(ch->pcdata->job_title_one) > 1) {
              free_string(ch->pcdata->job_title_two);
              ch->pcdata->job_title_two = str_dup(argument);
              ch->pcdata->job_type_two = JOB_PARTEMPLOY;
              ch->pcdata->job_room_two = ch->in_room->vnum;
              ch->pcdata->job_delay_two = current_time;
              printf_to_char(ch, "You take a part time job as %s.\n\r", ch->pcdata->job_title_two);
              if (str_cmp(prop->owner, ch->name)) {
                sprintf(buf, "%s takes a part time job at %s as %s.", ch->name, prop->propname, ch->pcdata->job_title_two);
                message_to_char(prop->owner, buf);
              }

            }
            else {
              free_string(ch->pcdata->job_title_one);
              ch->pcdata->job_title_one = str_dup(argument);
              ch->pcdata->job_type_one = JOB_PARTEMPLOY;
              ch->pcdata->job_room_one = ch->in_room->vnum;
              ch->pcdata->job_delay_one = current_time;
              printf_to_char(ch, "You take a part time job as %s.\n\r", ch->pcdata->job_title_one);
              if (str_cmp(prop->owner, ch->name)) {
                sprintf(buf, "%s takes a part time job at %s as %s.", ch->name, prop->propname, ch->pcdata->job_title_one);
                message_to_char(prop->owner, buf);
              }
            }
            return;
          }
        }
      }
      else
      send_to_char("Syntax: job take (parttime/fulltime/parttimestudent/fulltimestudent/parttimecommute/fulltimecommute (job name)\n\r", ch);
    }
    else if (!str_cmp(arg1, "quit")) {
      if (safe_strlen(argument) < 2) {
        send_to_char("Quit what job?\n\r", ch);
        return;
      }
      if (!str_cmp(argument, ch->pcdata->job_title_one)) {
        if (ch->pcdata->job_type_one == JOB_FULLEMPLOY || ch->pcdata->job_type_one == JOB_PARTEMPLOY) {
          if (get_room_index(ch->pcdata->job_room_one) != NULL && prop_from_room(get_room_index(ch->pcdata->job_room_one)) != NULL) {
            for (int i = 0; i < 20; i++) {
              if (!str_cmp(
                    prop_from_room(get_room_index(ch->pcdata->job_room_one))
                    ->workers[i], ch->name)) {
                free_string(
                prop_from_room(get_room_index(ch->pcdata->job_room_one))
                ->workers[i]);
                prop_from_room(get_room_index(ch->pcdata->job_room_one))
                ->workers[i] = str_dup("");
              }
            }
          }
        }
        free_string(ch->pcdata->job_title_one);
        ch->pcdata->job_title_one = str_dup("");
        ch->pcdata->job_type_one = JOB_UNEMPLOYED;
        send_to_char("You quit your job.\n\r", ch);
      }
      else if (!str_cmp(argument, ch->pcdata->job_title_two)) {
        if (ch->pcdata->job_type_two == JOB_FULLEMPLOY || ch->pcdata->job_type_two == JOB_PARTEMPLOY) {
          if (get_room_index(ch->pcdata->job_room_two) != NULL && prop_from_room(get_room_index(ch->pcdata->job_room_two)) != NULL) {
            for (int i = 0; i < 20; i++) {
              if (!str_cmp(
                    prop_from_room(get_room_index(ch->pcdata->job_room_two))
                    ->workers[i], ch->name)) {
                free_string(
                prop_from_room(get_room_index(ch->pcdata->job_room_two))
                ->workers[i]);
                prop_from_room(get_room_index(ch->pcdata->job_room_two))
                ->workers[i] = str_dup("");
              }
            }
          }
        }
        free_string(ch->pcdata->job_title_two);
        ch->pcdata->job_title_two = str_dup("");
        ch->pcdata->job_type_two = JOB_UNEMPLOYED;
        send_to_char("You quit your job.\n\r", ch);
      }
      else
      send_to_char("Syntax: job quit (job name)\n\r", ch);
    }
    else if (!str_cmp(arg1, "info")) {
      if (ch->pcdata->job_type_one == JOB_MIGRANT) {
        send_to_char("`cJob`x: New Arrival.\n\r", ch);
        return;
      }
      if (ch->race == RACE_DEPUTY) {
        send_to_char("`cJob`x: Deputy.\n\r", ch);
        return;
      }
      if (ch->pcdata->job_type_one == JOB_UNEMPLOYED && (ch->pcdata->job_type_two == JOB_UNEMPLOYED || ch->pcdata->job_type_two == JOB_MIGRANT)) {
        send_to_char("`cJob`x: Unemployed.\n\r", ch);
        return;
      }
      printf_to_char(ch, "`cJob`x: %s\n\r", job_name(ch, 1));
      if (ch->pcdata->job_type_two != JOB_UNEMPLOYED && ch->pcdata->job_type_two != JOB_MIGRANT)
      printf_to_char(ch, "`cJob`x: %s\n\r", job_name(ch, 2));
      return;
    }
    else
    send_to_char("Syntax: Job take/quit/info\n\r", ch);
  }

  void worker_update() {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }

      if ((*it)->lightcount == -1)
      (*it)->lightcount = lightcount(*it);
      (*it)->electric += (*it)->lightcount;
      for (int i = 0; i < 20; i++) {
        if (safe_strlen((*it)->workers[i]) > 1) {
          if (get_char_world_pc((*it)->workers[i]) != NULL && !IS_FLAG(get_char_world_pc((*it)->workers[i])->act, PLR_DEAD)) {
            (*it)->worker_idle[i] = 0;
          }
          else {
            (*it)->worker_idle[i]++;
            if ((*it)->worker_idle[i] > 1000 || daysidle((*it)->workers[i]) > 30) {
              free_string((*it)->workers[i]);
              (*it)->workers[i] = str_dup("");
            }
          }
        }
      }
    }
  }

  bool base_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (prop_from_room(room) == NULL)
    return FALSE;
    return is_base(prop_from_room(room));
  }

  ROOM_INDEX_DATA *propfetch(PROP_TYPE *prop, int x, int y, int z) {
    ROOM_INDEX_DATA *room = NULL;
    room = sourced_room_by_coordinates(get_room_index(prop->firstroom), x, y, z, TRUE);
    if (room == NULL)
    room = sourced_room_by_coordinates(get_room_index(prop->firstroom), x, y, z, FALSE);
    if (room != NULL) {
      bool excluded = FALSE;
      for (int i = 0; i < 5; i++) {
        if (room->vnum == prop->excluded[i])
        excluded = TRUE;
      }
      if (room->x < prop->minx || room->x > prop->maxx || room->y < prop->miny || room->y > prop->maxy || room->z < prop->minz || room->z > prop->maxz)
      excluded = TRUE;
      if (excluded == FALSE)
      return room;
    }

    if (z >= prop->maxz && y >= prop->maxy && x >= prop->maxx)
    return NULL;

    if (z < prop->minz) {
      ROOM_INDEX_DATA *fetch = propfetch(prop, prop->minx, prop->miny, z + 1);
      return fetch;
    }

    if (x >= prop->maxx && y >= prop->maxy) {
      ROOM_INDEX_DATA *fetch = propfetch(prop, prop->minx, prop->miny, z + 1);
      return fetch;
    }
    if (x < prop->maxx) {
      ROOM_INDEX_DATA *fetch = propfetch(prop, x + 1, y, z);
      return fetch;
    }
    else {
      ROOM_INDEX_DATA *fetch = propfetch(prop, prop->minx, y + 1, z);
      return fetch;
    }
    return NULL;
  }

  bool is_room_in_property(ROOM_INDEX_DATA *room, PROP_TYPE *prop) {
    if (prop->minx == prop->maxx) {
      for (int i = 0; i < 200; i++) {
        if (prop->roomlist[i] == 0) {
          return FALSE;
        }
        if (room->vnum == prop->roomlist[i])
        return TRUE;
      }
      return FALSE;
    }
    for (int i = 0; i < 5; i++) {
      if (room->vnum == prop->included[i])
      return TRUE;
    }
    for (int i = 0; i < 5; i++) {
      if (room->vnum == prop->excluded[i])
      return FALSE;
    }
    if (room->x <= prop->maxx && room->y <= prop->maxy && room->z <= prop->maxz && room->x >= prop->minx && room->y >= prop->miny && room->z >= prop->minz)
    return TRUE;

    return FALSE;
  }

  ROOM_INDEX_DATA *first_room_in_property(PROP_TYPE *prop) {
    if (prop->minx == prop->maxx) {
      for (int i = 0; i < 200; i++) {
        if (get_room_index(prop->roomlist[i]) != NULL)
        return get_room_index(prop->roomlist[i]);
      }
      return NULL;
    }
    for (int i = 0; i < 5; i++) {
      if (get_room_index(prop->included[i]) != NULL)
      return get_room_index(prop->included[i]);
    }
    return propfetch(prop, prop->minx, prop->miny, prop->minz);
  }

  ROOM_INDEX_DATA *next_room_in_property(PROP_TYPE *prop, ROOM_INDEX_DATA *previous) {
    if (prop->minx == prop->maxx) {
      for (int i = 0; i < 200; i++) {
        if (prop->roomlist[i] == previous->vnum) {
          for (int j = i + 1; j < 200; j++) {
            if (j == 0)
            return NULL;
            if (get_room_index(prop->roomlist[j]) != NULL)
            return get_room_index(prop->roomlist[j]);
          }
        }
      }
    }
    for (int i = 0; i < 5; i++) {
      if (previous->vnum == prop->included[i])
      if (i == 4)
      return propfetch(prop, prop->minx, prop->miny, prop->minz);
      for (int j = i + 1; j < 5; j++) {
        if (get_room_index(prop->included[j]) != NULL)
        return get_room_index(prop->included[i]);
      }
    }
    if (previous->x == prop->maxx && previous->y == prop->maxy && previous->z == prop->maxz)
    return NULL;
    if (previous->x == prop->maxx && previous->y == prop->maxy) {
      ROOM_INDEX_DATA *fetch =
      propfetch(prop, prop->minx, prop->miny, previous->z + 1);
      return fetch;
    }
    if (previous->x < prop->maxx) {
      ROOM_INDEX_DATA *fetch =
      propfetch(prop, previous->x + 1, previous->y, previous->z);
      return fetch;
    }
    else {
      ROOM_INDEX_DATA *fetch =
      propfetch(prop, prop->minx, previous->y + 1, previous->z);
      return fetch;
    }
    return NULL;
  }

  int get_toughness(ROOM_INDEX_DATA *room) {
    int value = room->toughness;

    PROP_TYPE *prop;
    if ((prop = prop_from_room(room)) != NULL) {
      if (is_base(prop))
      value = 2;
      if (prop->decay >= 75)
      value -= prop->decay / 75;
    }
    value = UMAX(value, 0);

    return value;
  }

  int get_security(ROOM_INDEX_DATA *room) {
    int value = room->security;

    PROP_TYPE *prop;
    if ((prop = prop_from_room(room)) != NULL) {
      if (is_base(prop))
      value = 3;
      if (prop->decay >= 75)
      value -= prop->decay / 75;
    }
    value = UMAX(value, 0);

    return value;
  }

  int get_decor(ROOM_INDEX_DATA *room) {
    int value = room->decor;

    PROP_TYPE *prop;
    if ((prop = prop_from_room(room)) != NULL) {
      if (is_base(prop))
      value = 1;
      CHAR_DATA *owner;
      if((owner = get_char_world_pc(prop->owner)) != NULL)
      {
        if(owner->pcdata->boon == BOON_LAVISH && owner->pcdata->boon_timeout > current_time)
        return 3;
      }
    }

    return value;
  }

  int property_roomcount(PROP_TYPE *prop) {
    int count = 0;
    ROOM_INDEX_DATA *start = first_room_in_property(prop);
    if (start == NULL)
    return 0;
    for (int i = 0; i < 300; i++) {
      count++;
      start = next_room_in_property(prop, start);
      if (start == NULL)
      return count;
    }
    return count;
  }

  char *redo_address(ROOM_INDEX_DATA *start, int dir, char *streetname) {
    char newbuf[MSL];
    int num = 0;

    if (dir == DIR_EAST || dir == DIR_WEST)
    num = start->y + 2;
    else
    num = start->x + 2;

    int mod = num % 2;

    num += mod;

    if (dir == DIR_SOUTH || dir == DIR_WEST)
    num++;

    if (num == 0)
    num += 2;

    sprintf(newbuf, "%d %s", num, streetname);

    return str_dup(newbuf);
  }

  void make_property(ROOM_INDEX_DATA *start, int direction, int xlength, int ylength, int type, int subtype, char *street) {
    PROP_TYPE *prop;
    prop = new_prop();

    prop->roadroom = start->vnum;
    prop->orientation = direction;

    if (direction == DIR_NORTH) {
      if (subtype == PROPERTY_SMALLFLAT) {
        prop->firstroom = room_by_coordinates(start->x, start->y + 1, 0)->vnum;
        prop->minx = start->x;
        prop->miny = start->y + 1;
        prop->maxx = start->x + 1;
        prop->maxy = start->y + 2;
      }
      else if (subtype == PROPERTY_MEDFLAT) {
        prop->firstroom = room_by_coordinates(start->x, start->y + 1, 0)->vnum;
        prop->minx = start->x;
        prop->miny = start->y + 1;
        prop->maxx = start->x + 3;
        prop->maxy = start->y + 2;
      }
      else if (subtype == PROPERTY_BIGFLAT) {
        prop->firstroom = room_by_coordinates(start->x, start->y + 1, 0)->vnum;
        prop->minx = start->x;
        prop->miny = start->y + 1;
        prop->maxx = start->x + 3;
        prop->maxy = start->y + 5;
      }
      else {
        if (xlength % 2 == 0)
        return;

        prop->firstroom = room_by_coordinates(start->x, start->y + 1, 0)->vnum;
        prop->minx = start->x - (xlength - 1) / 2;
        prop->miny = start->y + 1;
        prop->maxx = start->x + (xlength - 1) / 2;
        prop->maxy = start->y + ylength;
        if (subtype == PROPERTY_UNDERSHOP)
        prop->excluded[0] =
        room_by_coordinates(start->x + 1, start->y + 3, 0)->vnum;
        if (subtype == PROPERTY_OVERFLAT)
        prop->included[0] =
        room_by_coordinates(start->x + 1, start->y + 3, 0)->vnum;
      }
    }
    if (direction == DIR_SOUTH) {
      if (xlength % 2 == 0)
      return;

      prop->firstroom = room_by_coordinates(start->x, start->y - 1, 0)->vnum;
      prop->minx = start->x - (xlength - 1) / 2;
      prop->miny = start->y - ylength;
      prop->maxx = start->x + (xlength - 1) / 2;
      prop->maxy = start->y - 1;
      if (subtype == PROPERTY_UNDERSHOP)
      prop->excluded[0] =
      room_by_coordinates(start->x - 1, start->y - 3, 0)->vnum;
      if (subtype == PROPERTY_OVERFLAT)
      prop->included[0] =
      room_by_coordinates(start->x - 1, start->y - 3, 0)->vnum;
    }
    if (direction == DIR_EAST) {
      if (ylength % 2 == 0)
      return;

      prop->firstroom = room_by_coordinates(start->x + 1, start->y, 0)->vnum;
      prop->minx = start->x + 1;
      prop->miny = start->y - (ylength - 1) / 2;
      prop->maxx = start->x + xlength;
      prop->maxy = start->y + (ylength - 1) / 2;
      if (subtype == PROPERTY_UNDERSHOP)
      prop->excluded[0] =
      room_by_coordinates(start->x + 3, start->y - 1, 0)->vnum;
      if (subtype == PROPERTY_OVERFLAT)
      prop->included[0] =
      room_by_coordinates(start->x + 3, start->y - 1, 0)->vnum;
    }
    if (direction == DIR_WEST) {
      if (ylength % 2 == 0)
      return;
      prop->firstroom = room_by_coordinates(start->x - 1, start->y, 0)->vnum;
      prop->minx = start->x - xlength;
      prop->miny = start->y - (ylength - 1) / 2;
      prop->maxx = start->x - 1;
      prop->maxy = start->y + (ylength - 1) / 2;
      if (subtype == PROPERTY_UNDERSHOP)
      prop->excluded[0] =
      room_by_coordinates(start->x - 3, start->y + 1, 0)->vnum;
      if (subtype == PROPERTY_OVERFLAT)
      prop->included[0] =
      room_by_coordinates(start->x - 3, start->y + 1, 0)->vnum;
    }

    if (subtype == PROPERTY_OVERFLAT) {
      prop->firstroom = room_by_coordinates(get_room_index(prop->firstroom)->x, get_room_index(prop->firstroom)->y, 1)
      ->vnum;
    }

    prop->type_special = subtype;

    free_string(prop->owner);
    prop->owner = str_dup("NPC");
    prop->type = type;
    prop->minz = town_minz(first_room_in_property(prop), NULL);
    prop->maxz = town_maxz(first_room_in_property(prop), NULL);

    free_string(prop->address);
    free_string(prop->propname);
    if (subtype == PROPERTY_SMALLFLAT || subtype == PROPERTY_MEDFLAT || subtype == PROPERTY_BIGFLAT) {
      prop->address = str_dup(street);
      prop->propname = str_dup(street);
    }
    else {
      if (safe_strlen(street) > 2)
      prop->address = str_dup(redo_address(start, direction, street));
      else
      prop->address = str_dup(create_address(start, direction));
      prop->propname = str_dup(prop->address);
    }
    prop->auction_day = number_range(1, 28);
    prop->auction_month = get_month() + 1;
    if (type == PROP_SHOP)
    prop->vnum = next_prop_svnum();
    else
    prop->vnum = next_prop_hvnum();
    prop->price = property_truevalue(prop);
    if (prop->auction_month > 11)
    prop->auction_month = 0;
    /*
if(get_day() <= prop->auction_day) {
if (get_month() < 11){
prop->auction_month = get_month()+1;
}
else {
prop->auction_month = 0;
}
}
*/
    prop->valid = TRUE;
    PropVect.push_back(prop);
    save_properties(FALSE);
  }

  bool can_develop_proproom(ROOM_INDEX_DATA *room) {
    PROP_TYPE *prop = prop_from_room(room);
    if (prop == NULL)
    return FALSE;

    if (prop->type_special == PROPERTY_TOWNHOUSE) {
      ROOM_INDEX_DATA *first = get_room_index(prop->firstroom);
      if (room->x == first->x && room->y == first->y)
      return FALSE;
      return TRUE;
    }
    if (prop->type_special == PROPERTY_TOWNSHOP) {
      ROOM_INDEX_DATA *first = get_room_index(prop->firstroom);
      if ((prop->orientation == DIR_EAST || prop->orientation == DIR_WEST) && room->x == first->x)
      return FALSE;
      if ((prop->orientation == DIR_NORTH || prop->orientation == DIR_SOUTH) && room->y == first->y)
      return FALSE;
      return TRUE;
    }
    if (prop->type_special == PROPERTY_TRAILER) {
      ROOM_INDEX_DATA *first = get_room_index(prop->firstroom);
      if ((prop->orientation == DIR_EAST || prop->orientation == DIR_WEST) && room->x == first->x)
      return FALSE;
      if ((prop->orientation == DIR_NORTH || prop->orientation == DIR_SOUTH) && room->y == first->y)
      return FALSE;
      return TRUE;
    }

    if (prop->type_special == PROPERTY_OUTERFOREST) {
      ROOM_INDEX_DATA *first = get_room_index(prop->firstroom);
      if (room->x == first->x && room->y == first->y)
      return FALSE;
      return TRUE;
    }
    if (prop->type_special == PROPERTY_INNERFOREST) {
      ROOM_INDEX_DATA *first = get_room_index(prop->firstroom);
      if ((prop->orientation == DIR_EAST || prop->orientation == DIR_WEST) && room->x == first->x && room->y == first->y + 1)
      return FALSE;
      if ((prop->orientation == DIR_NORTH || prop->orientation == DIR_SOUTH) && room->y == first->y && room->x == first->x + 1)
      return FALSE;
      return TRUE;
    }
    if (prop->type_special == PROPERTY_UNDERSHOP || prop->type_special == PROPERTY_OVERFLAT) {
      ROOM_INDEX_DATA *first = get_room_index(prop->firstroom);
      if (room->x == first->x && room->y == first->y)
      return FALSE;
      return TRUE;
    }
    if (prop->type_special == PROPERTY_LONGHOUSE || prop->type_special == PROPERTY_LONGSHOP) {
      if ((prop->orientation == DIR_EAST || prop->orientation == DIR_WEST) && room->x == prop->minx)
      return FALSE;
      if ((prop->orientation == DIR_NORTH || prop->orientation == DIR_SOUTH) && room->y == prop->miny)
      return FALSE;
      if ((prop->orientation == DIR_EAST || prop->orientation == DIR_WEST) && room->x == prop->maxx)
      return FALSE;
      if ((prop->orientation == DIR_NORTH || prop->orientation == DIR_SOUTH) && room->y == prop->maxy)
      return FALSE;

      return TRUE;
    }
    return TRUE;
  }

  _DOFUN(do_makeproperty) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg2);
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg3);
    char arg4[MSL];
    argument = one_argument_nouncap(argument, arg4);
    char arg5[MSL];
    argument = one_argument_nouncap(argument, arg5);

    int dir = 0;
    int type = 0;
    int subtype = 0;
    int xlength = 0;
    int ylength = 0;
    if (!str_cmp(arg1, "north"))
    dir = DIR_NORTH;
    else if (!str_cmp(arg1, "south"))
    dir = DIR_SOUTH;
    else if (!str_cmp(arg1, "west"))
    dir = DIR_WEST;
    else if (!str_cmp(arg1, "east"))
    dir = DIR_EAST;
    else {
      send_to_char("Syntax: makeproperty (direction) (type) (subtype) (xlength) (ylength)\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "shop"))
    type = PROP_SHOP;
    else if (!str_cmp(arg2, "house"))
    type = PROP_HOUSE;
    else {
      send_to_char("Syntax: makeproperty (direction) (type) (subtype) (xlength) (ylength)\n\r", ch);
      return;
    }

    if (!str_cmp(arg3, "townhouse"))
    subtype = PROPERTY_TOWNHOUSE;
    else if (!str_cmp(arg3, "townshop"))
    subtype = PROPERTY_TOWNSHOP;
    else if (!str_cmp(arg3, "undershop"))
    subtype = PROPERTY_UNDERSHOP;
    else if (!str_cmp(arg3, "overflat"))
    subtype = PROPERTY_OVERFLAT;
    else if (!str_cmp(arg3, "longhouse"))
    subtype = PROPERTY_LONGHOUSE;
    else if (!str_cmp(arg3, "longshop"))
    subtype = PROPERTY_LONGSHOP;
    else if (!str_cmp(arg3, "trailer"))
    subtype = PROPERTY_TRAILER;
    else if (!str_cmp(arg3, "smallflat"))
    subtype = PROPERTY_SMALLFLAT;
    else if (!str_cmp(arg3, "medflat"))
    subtype = PROPERTY_MEDFLAT;
    else if (!str_cmp(arg3, "bigflat"))
    subtype = PROPERTY_BIGFLAT;

    else {
      send_to_char("Syntax: makeproperty (direction) (type) (subtype) (xlength) (ylength)\n\r", ch);
      return;
    }
    xlength = atoi(arg4);
    ylength = atoi(arg5);

    if (xlength < 1 || xlength > 10 || ylength < 1 || ylength > 10) {
      send_to_char("Syntax: makeproperty (direction) (type) (subtype) (xlength) (ylength)\n\r", ch);
      return;
    }
    make_property(ch->in_room, dir, xlength, ylength, type, subtype, argument);
    send_to_char("Done.\n\r", ch);
    if (ch->in_room->exit[dir] != NULL) {
      // printf_to_char(ch, "Address: %s, rooms %d.\n\r", // prop_from_room(ch->in_room->exit[dir]->u1.to_room)->address, // property_roomcount(prop_from_room(ch->in_room->exit[dir]->u1.to_room)));
      // printf_to_char(ch, "Coords: %d %d, %d %d\n\r", // prop_from_room(ch->in_room->exit[dir]->u1.to_room)->minx, // prop_from_room(ch->in_room->exit[dir]->u1.to_room)->miny, // prop_from_room(ch->in_room->exit[dir]->u1.to_room)->maxx, // prop_from_room(ch->in_room->exit[dir]->u1.to_room)->maxy);
    }
  }

  void delete_exit(ROOM_INDEX_DATA *room, int door) {
    if (room == NULL || room->exit[door] == NULL)
    return;

    int value;
    room->exit[door]->wall = WALL_NONE;
    if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR)) {
      value = flag_value(exit_flags, "door");
      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;
    }
    if (IS_SET(room->exit[door]->exit_info, EX_CLOSED)) {
      value = flag_value(exit_flags, "closed");
      TOGGLE_BIT(room->exit[door]->rs_flags, value);
      room->exit[door]->exit_info = room->exit[door]->rs_flags;
    }

    room->exit[door]->wall = 0;
    room->exit[door]->wallcondition = 0;
    ROOM_INDEX_DATA *pToRoom = room->exit[door]->u1.to_room;
    int rev = rev_dir[door];

    if (pToRoom == NULL || pToRoom->exit[rev] == NULL)
    return;

    if (pToRoom->exit[door] ==
        NULL) // Added to combat crash - Discordance 9-14-2015
    return;

    pToRoom->exit[rev]->wall = 0;
    pToRoom->exit[rev]->wallcondition = 0;

    if (IS_SET(pToRoom->exit[rev]->exit_info, EX_ISDOOR)) {
      value = flag_value(exit_flags, "door");
      TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
      pToRoom->exit[rev]->exit_info = pToRoom->exit[door]->rs_flags;
    }
    if (IS_SET(pToRoom->exit[rev]->exit_info, EX_CLOSED)) {
      value = flag_value(exit_flags, "closed");
      TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
      pToRoom->exit[rev]->exit_info = pToRoom->exit[rev]->rs_flags;
    }
  }

  void setup_proproom(ROOM_INDEX_DATA *room, PROP_TYPE *prop) {

    if (prop == NULL)
    return;

    if (prop->vnum == 0)
    return;

    if (room->z > 0) {
      for (int x = 0; x < 10; x++) {
        if (room->exit[x] != NULL && room->exit[x]->u1.to_room != NULL) {
          if (room->exit[x]->u1.to_room->sector_type == SECT_AIR && x != DIR_UP)
          room->exit[x]->fall = room->z * 2;
          else
          room->exit[x]->fall = 0;
        }
        if (room->sector_type != SECT_AIR && prop->airfix == FALSE) {
          prop->airfix = TRUE;
          //        char buf[MSL];
          //        sprintf(buf, "AIRFIX: %d:%d", room->vnum, prop->vnum);
          //        log_string(buf);
          if (room->exit[DIR_EAST] == NULL && room->z > 0)
          makesky(room->x + 1, room->y, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_WEST] == NULL && room->z > 0)
          makesky(room->x - 1, room->y, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_NORTHEAST] == NULL && room->z > 0)
          makesky(room->x + 1, room->y + 1, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_SOUTHEAST] == NULL && room->z > 0)
          makesky(room->x + 1, room->y - 1, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_NORTHWEST] == NULL && room->z > 0)
          makesky(room->x - 1, room->y + 1, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_SOUTHWEST] == NULL && room->z > 0)
          makesky(room->x - 1, room->y - 1, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_NORTH] == NULL && room->z > 0)
          makesky(room->x, room->y + 1, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_SOUTH] == NULL && room->z > 0)
          makesky(room->x, room->y - 1, room->z, room->area->vnum, FALSE);
          else if (room->exit[DIR_UP] == NULL)
          makesky(room->x, room->y, room->z + 1, room->area->vnum, FALSE);
        }
      }
    }
    else if (room->z < 0) {
      if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL) {
        if (room->exit[DIR_UP]->wall == WALL_NONE && (room->exit[DIR_UP]->u1.to_room->sector_type == SECT_WATER || room->exit[DIR_UP]->u1.to_room->sector_type == SECT_UNDERWATER || room->exit[DIR_UP]->u1.to_room->sector_type == SECT_SHALLOW))
        room->sector_type = SECT_UNDERWATER;
        else if (room->sector_type == SECT_UNDERWATER)
        room->sector_type = SECT_WATER;
      }
      else if (room->sector_type == SECT_UNDERWATER)
      room->sector_type = SECT_WATER;
    }

    if (prop->type == PROP_SHOP) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW && room->sector_type != SECT_UNDERWATER && !is_base(prop))
        room->sector_type = SECT_COMMERCIAL;

        if (IS_SET(room->room_flags, ROOM_UNLIT))
        REMOVE_BIT(room->room_flags, ROOM_UNLIT);
        if (!IS_SET(room->room_flags, ROOM_DARK))
        SET_BIT(room->room_flags, ROOM_DARK);
        if (!IS_SET(room->room_flags, ROOM_LIGHTON) && !IS_SET(room->room_flags, ROOM_LIGHTOFF))
        SET_BIT(room->room_flags, ROOM_LIGHTOFF);
        if (IS_SET(room->room_flags, ROOM_BEDROOM))
        REMOVE_BIT(room->room_flags, ROOM_BEDROOM);
        if (IS_SET(room->room_flags, ROOM_BATHROOM))
        REMOVE_BIT(room->room_flags, ROOM_BATHROOM);
        if (IS_SET(room->room_flags, ROOM_KITCHEN))
        REMOVE_BIT(room->room_flags, ROOM_KITCHEN);
      }
      else {
        for (int x = 0; x < 10; x++) {
          if (room->exit[x] != NULL && room->exit[x]->u1.to_room != NULL) {
            if (!IS_SET(room->exit[x]->u1.to_room->room_flags, ROOM_INDOORS))
            delete_exit(room, x);
          }
        }
        if (!IS_SET(room->room_flags, ROOM_UNLIT))
        SET_BIT(room->room_flags, ROOM_UNLIT);
        if (IS_SET(room->room_flags, ROOM_DARK))
        REMOVE_BIT(room->room_flags, ROOM_DARK);
        if (IS_SET(room->room_flags, ROOM_LIGHTON))
        REMOVE_BIT(room->room_flags, ROOM_LIGHTON);
        if (IS_SET(room->room_flags, ROOM_LIGHTOFF))
        REMOVE_BIT(room->room_flags, ROOM_LIGHTOFF);
        if (IS_SET(room->room_flags, ROOM_BEDROOM))
        REMOVE_BIT(room->room_flags, ROOM_BEDROOM);
        if (IS_SET(room->room_flags, ROOM_BATHROOM))
        REMOVE_BIT(room->room_flags, ROOM_BATHROOM);
        if (IS_SET(room->room_flags, ROOM_KITCHEN))
        REMOVE_BIT(room->room_flags, ROOM_KITCHEN);
        if (room->exit[DIR_DOWN] != NULL && room->exit[DIR_DOWN]->u1.to_room != NULL && !IS_SET(room->exit[DIR_DOWN]->u1.to_room->room_flags, ROOM_INDOORS) && prop_from_room(room->exit[DIR_DOWN]->u1.to_room) != NULL && prop_from_room(room->exit[DIR_DOWN]->u1.to_room)->vnum ==
            prop->vnum && !is_water(room->exit[DIR_DOWN]->u1.to_room) && room->z > 0) {
          room->sector_type = SECT_AIR;
          free_string(room->name);
          room->name = str_dup("`CSkies`x");

        }
        else if (room->z == 0) {
          if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW && room->sector_type != SECT_SIDEWALK && room->sector_type != SECT_PARKING && room->sector_type != SECT_BEACH)
          room->sector_type = SECT_PARK;
        }
        else if (room->z > 0 && room->z > prop->minz) {
          room->sector_type = SECT_ROOFTOP;
        }
        else if (room->z < 0) {
          if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
            if (room->exit[DIR_UP] == NULL || room->exit[DIR_UP]->u1.to_room == NULL || room->exit[DIR_UP]->wall > WALL_NONE || IS_SET(room->exit[DIR_UP]->exit_info, EX_CLOSED))
            SET_BIT(room->room_flags, ROOM_INDOORS);
          }
          if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL && (room->exit[DIR_UP]->u1.to_room->sector_type == SECT_WATER || room->exit[DIR_UP]->u1.to_room->sector_type == SECT_UNDERWATER))
          room->sector_type = SECT_UNDERWATER;
          else if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW)
          room->sector_type = SECT_CAVE;
        }
      }
    }
    else {

      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW && room->sector_type != SECT_UNDERWATER)
        room->sector_type = SECT_HOUSE;

        if (IS_SET(room->room_flags, ROOM_UNLIT))
        REMOVE_BIT(room->room_flags, ROOM_UNLIT);
        if (!IS_SET(room->room_flags, ROOM_DARK))
        SET_BIT(room->room_flags, ROOM_DARK);
        if (!IS_SET(room->room_flags, ROOM_LIGHTON) && !IS_SET(room->room_flags, ROOM_LIGHTOFF))
        SET_BIT(room->room_flags, ROOM_LIGHTOFF);
      }
      else {
        for (int x = 0; x < 10; x++) {
          if (room->exit[x] != NULL && room->exit[x]->u1.to_room != NULL) {
            if (!IS_SET(room->exit[x]->u1.to_room->room_flags, ROOM_INDOORS))
            if (x != DIR_NORTHWEST && x != DIR_NORTHEAST && x != DIR_SOUTHWEST && x != DIR_SOUTHEAST)
            delete_exit(room, x);
          }
        }
        if (!IS_SET(room->room_flags, ROOM_UNLIT))
        SET_BIT(room->room_flags, ROOM_UNLIT);
        if (IS_SET(room->room_flags, ROOM_DARK))
        REMOVE_BIT(room->room_flags, ROOM_DARK);
        if (IS_SET(room->room_flags, ROOM_LIGHTON))
        REMOVE_BIT(room->room_flags, ROOM_LIGHTON);
        if (IS_SET(room->room_flags, ROOM_LIGHTOFF))
        REMOVE_BIT(room->room_flags, ROOM_LIGHTOFF);
        if (IS_SET(room->room_flags, ROOM_BEDROOM))
        REMOVE_BIT(room->room_flags, ROOM_BEDROOM);
        if (IS_SET(room->room_flags, ROOM_BATHROOM))
        REMOVE_BIT(room->room_flags, ROOM_BATHROOM);
        if (IS_SET(room->room_flags, ROOM_KITCHEN))
        REMOVE_BIT(room->room_flags, ROOM_KITCHEN);

        if (room->exit[DIR_DOWN] != NULL && room->exit[DIR_DOWN]->u1.to_room != NULL && !IS_SET(room->exit[DIR_DOWN]->u1.to_room->room_flags, ROOM_INDOORS) && prop_from_room(room->exit[DIR_DOWN]->u1.to_room) != NULL && prop_from_room(room->exit[DIR_DOWN]->u1.to_room)->vnum ==
            prop->vnum && !is_water(room->exit[DIR_DOWN]->u1.to_room) && room->z > 0) {
          room->sector_type = SECT_AIR;
          free_string(room->name);
          room->name = str_dup("`CSkies`x");
        }
        else if (room->z == 0) {
          if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW && room->sector_type != SECT_SIDEWALK && room->sector_type != SECT_PARKING && room->sector_type != SECT_BEACH)
          room->sector_type = SECT_PARK;
        }
        else if (room->z > 0 && room->z > prop->minz) {
          room->sector_type = SECT_ROOFTOP;
        }
        else if (room->z < 0) {
          if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
            if (room->exit[DIR_UP] == NULL || room->exit[DIR_UP]->u1.to_room == NULL || room->exit[DIR_UP]->wall > WALL_NONE || IS_SET(room->exit[DIR_UP]->exit_info, EX_CLOSED))
            SET_BIT(room->room_flags, ROOM_INDOORS);
          }
          if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL && (room->exit[DIR_UP]->u1.to_room->sector_type == SECT_WATER || room->exit[DIR_UP]->u1.to_room->sector_type == SECT_UNDERWATER))
          room->sector_type = SECT_UNDERWATER;
          else if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW)
          room->sector_type = SECT_CAVE;
        }
      }
    }
  }

  int room_value(ROOM_INDEX_DATA *room) {
    int value = 0;

    if (IS_SET(room->room_flags, ROOM_INDOORS))
    value += ROOM_PRICE;

    if(room->z < 0)
    value = 0;

    if (IS_SET(room->room_flags, ROOM_BEDROOM))
    value += FEATURE_PRICE;
    if (IS_SET(room->room_flags, ROOM_KITCHEN))
    value += FEATURE_PRICE;
    if (IS_SET(room->room_flags, ROOM_BATHROOM))
    value += FEATURE_PRICE;

    int decorprice = 0;
    if (room->decor == 1)
    decorprice = UMAX(ROOM_PRICE, value) * (DECOR_ONE) / 100;
    if (room->decor == 2)
    decorprice = UMAX(ROOM_PRICE, value) * (DECOR_TWO) / 100;
    if (room->decor == 3)
    decorprice = UMAX(ROOM_PRICE, value) * (DECOR_THREE) / 100;

    int toughprice = 0;
    toughprice =
    UMAX(ROOM_PRICE, value) * (room->toughness) * TOUGHNESS_PRICE / 100;

    int secureprice = 0;
    secureprice = SECURITY_PRICE * skillpoint(room->security);

    if(room->z < 0)
    {
      secureprice /= 4;
      toughprice /= 4;
    }

    int cost = value + decorprice + toughprice + secureprice;
    return cost;
  }

  char *room_tags(ROOM_INDEX_DATA *room) {
    char buf[MSL] = "";

    if (IS_SET(room->room_flags, ROOM_INDOORS))
    strcat(buf, "i");

    if (IS_SET(room->room_flags, ROOM_BEDROOM))
    strcat(buf, "b");
    if (IS_SET(room->room_flags, ROOM_KITCHEN))
    strcat(buf, "k");
    if (IS_SET(room->room_flags, ROOM_BATHROOM))
    strcat(buf, "a");

    if (room->decor == 1)
    strcat(buf, "d-1");
    if (room->decor == 2)
    strcat(buf, "d-2");
    if (room->decor == 3)
    strcat(buf, "d-3");

    char tmp[MSL];
    sprintf(tmp, "t-%ds-%d", room->toughness, room->security);
    strcat(buf, tmp);
    return str_dup(buf);
  }

  void reclaim_room(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ped = NULL;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    for (obj = room->contents; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
        obj_from_room(obj);
        obj_to_room(obj, get_room_index(1));
      }
      else
      extract_obj(obj);
    }
    ROOM_INDEX_DATA *first = room;
    for (int i = 0; i < 10; i++) {
      if (first->exit[i] == NULL)
      continue;
      if (first->exit[i]->u1.to_room == NULL)
      continue;

      if (IS_SET(first->exit[i]->exit_info, EX_ISDOOR)) {
        int value = flag_value(exit_flags, "door");
        TOGGLE_BIT(first->exit[i]->rs_flags, value);
        first->exit[i]->exit_info = first->exit[i]->rs_flags;

        ROOM_INDEX_DATA *pToRoom = first->exit[i]->u1.to_room; /* ROM OLC */
        int rev = rev_dir[i];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          if (pToRoom->z < 0)
          pToRoom->exit[rev]->wall = WALL_BRICK;
          else
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->wallcondition = 0;
          pToRoom->exit[rev]->doorbroken = 0;
          pToRoom->exit[rev]->rs_flags = first->exit[i]->rs_flags;
          pToRoom->exit[rev]->exit_info = first->exit[i]->exit_info;
        }
      }
      if (IS_SET(first->exit[i]->exit_info, EX_CLOSED)) {
        int value = flag_value(exit_flags, "closed");
        TOGGLE_BIT(first->exit[i]->rs_flags, value);
        first->exit[i]->exit_info = first->exit[i]->rs_flags;

        ROOM_INDEX_DATA *pToRoom = first->exit[i]->u1.to_room; /* ROM OLC */
        int rev = rev_dir[i];

        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          if (pToRoom->z < 0)
          pToRoom->exit[rev]->wall = WALL_BRICK;
          else
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->wallcondition = 0;
          pToRoom->exit[rev]->doorbroken = 0;
          pToRoom->exit[rev]->rs_flags = first->exit[i]->rs_flags;
          pToRoom->exit[rev]->exit_info = first->exit[i]->exit_info;
        }
      }
      else {
        ROOM_INDEX_DATA *pToRoom = first->exit[i]->u1.to_room; /* ROM OLC */
        int rev = rev_dir[i];

        if (pToRoom->exit[rev] != NULL) {
          if (pToRoom->z < 0)
          pToRoom->exit[rev]->wall = WALL_BRICK;
          else
          pToRoom->exit[rev]->wall = WALL_NONE;
          pToRoom->exit[rev]->wallcondition = 0;
          pToRoom->exit[rev]->doorbroken = 0;
        }
      }

      if (first->z < 0)
      first->exit[i]->wall = WALL_BRICK;
      else
      first->exit[i]->wall = WALL_NONE;
      first->exit[i]->wallcondition = 0;
      first->exit[i]->doorbroken = 0;
    }
    if (IS_SET(first->room_flags, ROOM_BEDROOM))
    REMOVE_BIT(first->room_flags, ROOM_BEDROOM);
    if (IS_SET(first->room_flags, ROOM_BATHROOM))
    REMOVE_BIT(first->room_flags, ROOM_BATHROOM);
    if (IS_SET(first->room_flags, ROOM_KITCHEN))
    REMOVE_BIT(first->room_flags, ROOM_KITCHEN);
    if (IS_SET(first->room_flags, ROOM_LIGHTOFF))
    REMOVE_BIT(first->room_flags, ROOM_LIGHTOFF);
    if (IS_SET(first->room_flags, ROOM_LIGHTON))
    REMOVE_BIT(first->room_flags, ROOM_LIGHTON);
    if (IS_SET(first->room_flags, ROOM_INDOORS))
    REMOVE_BIT(first->room_flags, ROOM_INDOORS);
    if (!IS_SET(first->room_flags, ROOM_UNLIT))
    SET_BIT(first->room_flags, ROOM_UNLIT);

    if (first->z == 0) {
      first->sector_type = SECT_FOREST;
      free_string(first->name);
      first->name = str_dup("The Forest");
      free_string(first->description);
      first->description = str_dup("");
    }
    else if (first->z > 0) {
      first->sector_type = SECT_AIR;
      free_string(first->name);
      first->name = str_dup("`CThe Skies`x");
      free_string(first->description);
      first->description = str_dup("");
    }
    else if (first->z < 0) {
      free_string(first->name);
      first->name = str_dup("");
      free_string(first->description);
      first->description = str_dup("");
    }

    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (ed->next != NULL)
      ped = ed;

      if (ed) {
        if (!ped)
        room->extra_descr = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }
    for (ed = room->places; ed; ed = ed->next) {
      if (ed->next != NULL)
      ped = ed;

      if (ed) {
        if (!ped)
        room->places = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }

    RESET_DATA *pReset;

    for (int i = 30; i > 0; i--) {
      if (!room->reset_first) {
      }
      else if (i == 1) {
        pReset = room->reset_first;
        room->reset_first = room->reset_first->next;
        if (!room->reset_first)
        room->reset_last = NULL;
      }
      else if (i > 1) {
        int iReset = 0;
        RESET_DATA *prev = NULL;

        for (pReset = room->reset_first; pReset; pReset = pReset->next) {
          if (++iReset == i)
          break;
          prev = pReset;
        }
        if (!pReset) {
        }
        else {
          if (prev)
          prev->next = prev->next->next;
          else
          room->reset_first = room->reset_first->next;

          for (room->reset_last = room->reset_first; room->reset_last->next;
          room->reset_last = room->reset_last->next)
          ;
        }
      }
    }
    EXTRA_DESCR_DATA *pExtra = NULL;
    for (pExtra = room->extra_descr; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }
    pExtra = NULL;
    for (pExtra = room->places; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    if (room->reset_first == NULL && room->reset_last != NULL)
    room->reset_first = room->reset_last;
    for (; room->reset_last != NULL;) {
      room->reset_first = room->reset_first->next;
      if (!room->reset_first)
      room->reset_last = NULL;
    }
  }

  void reclaim_property(PROP_TYPE *prop) {
    //	OBJ_DATA *obj_next;
    //     	OBJ_DATA *obj;
    offline_message(prop->owner, "Your property is reclaimed by the forest.\n\r");

    ROOM_INDEX_DATA *first = first_room_in_property(prop);
    for (int x = 0; first != NULL && is_room_in_property(first, prop) && x < 150;
    first = next_room_in_property(prop, first)) {
      x++;
      reclaim_room(first);
      /*
RESET_DATA *pReset = NULL;
while(first->reset_first)
{
pReset = first->reset_first;
first->reset_first = first->reset_first->next;
if ( !first->reset_first )
first->reset_last = NULL;
}
if(pReset != NULL)
free_reset_data(pReset);
for(CharList::iterator it = first->people->begin();
it != first->people->end(); )
{
CHAR_DATA *victim = *it;
++it;

if ( IS_NPC(victim))
{
save_char_obj( victim, TRUE, FALSE );
extract_char( victim, TRUE );
}
}

for ( obj = first->contents; obj != NULL; obj = obj_next )
{
obj_next = obj->next_content;
if(IS_SET(obj->extra_flags, ITEM_RELIC))
{
obj_from_room(obj);
obj_to_room(obj, get_room_index(1));
}
else
extract_obj( obj );
}

for(int i=0;i<10;i++)
{
if(first->exit[i] == NULL)
continue;

if(IS_SET(first->exit[i]->exit_info, EX_ISDOOR))
{
int value = flag_value( exit_flags, "door" );
TOGGLE_BIT(first->exit[i]->rs_flags,  value);
first->exit[i]->exit_info = first->exit[i]->rs_flags;

ROOM_INDEX_DATA *pToRoom = first->exit[i]->u1.to_room;
int rev = rev_dir[i];

if (pToRoom->exit[rev] != NULL)
{
TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
if(pToRoom->z < 0)
pToRoom->exit[rev]->wall = WALL_BRICK;
else
pToRoom->exit[rev]->wall = WALL_NONE;
pToRoom->exit[rev]->wallcondition = 0;
pToRoom->exit[rev]->doorbroken = 0;
pToRoom->exit[rev]->rs_flags = first->exit[i]->rs_flags;
pToRoom->exit[rev]->exit_info = first->exit[i]->exit_info;
}
}
if(IS_SET(first->exit[i]->exit_info, EX_CLOSED))
{
int value = flag_value( exit_flags, "closed" );
TOGGLE_BIT(first->exit[i]->rs_flags,  value);
first->exit[i]->exit_info = first->exit[i]->rs_flags;

ROOM_INDEX_DATA *pToRoom = first->exit[i]->u1.to_room;
int rev = rev_dir[i];

if (pToRoom->exit[rev] != NULL)
{
TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
if(pToRoom->z < 0)
pToRoom->exit[rev]->wall = WALL_BRICK;
else
pToRoom->exit[rev]->wall = WALL_NONE;
pToRoom->exit[rev]->wallcondition = 0;
pToRoom->exit[rev]->doorbroken = 0;
pToRoom->exit[rev]->rs_flags = first->exit[i]->rs_flags;
pToRoom->exit[rev]->exit_info = first->exit[i]->exit_info;
}
}
else
{
ROOM_INDEX_DATA *pToRoom = first->exit[i]->u1.to_room;
int rev = rev_dir[i];

if (pToRoom->exit[rev] != NULL)
{
if(pToRoom->z < 0)
pToRoom->exit[rev]->wall = WALL_BRICK;
else
pToRoom->exit[rev]->wall = WALL_NONE;
pToRoom->exit[rev]->wallcondition = 0;
pToRoom->exit[rev]->doorbroken = 0;
}
}

if(first->z < 0)
first->exit[i]->wall = WALL_BRICK;
else
first->exit[i]->wall = WALL_NONE;
first->exit[i]->wallcondition = 0;
first->exit[i]->doorbroken = 0;
}
if(IS_SET(first->room_flags, ROOM_BEDROOM))
REMOVE_BIT(first->room_flags, ROOM_BEDROOM);
if(IS_SET(first->room_flags, ROOM_BATHROOM))
REMOVE_BIT(first->room_flags, ROOM_BATHROOM);
if(IS_SET(first->room_flags, ROOM_KITCHEN))
REMOVE_BIT(first->room_flags, ROOM_KITCHEN);
if(IS_SET(first->room_flags, ROOM_LIGHTOFF))
REMOVE_BIT(first->room_flags, ROOM_LIGHTOFF);
if(IS_SET(first->room_flags, ROOM_LIGHTON))
REMOVE_BIT(first->room_flags, ROOM_LIGHTON);
if(IS_SET(first->room_flags, ROOM_INDOORS))
REMOVE_BIT(first->room_flags, ROOM_INDOORS);
if(!IS_SET(first->room_flags, ROOM_UNLIT))
SET_BIT(first->room_flags, ROOM_UNLIT);

if(first->z == 0)
{
first->sector_type = SECT_FOREST;
free_string(first->name);
first->name = str_dup("The Forest");
free_string(first->description);
first->description = str_dup("");
}
else if(first->z > 0)
{
first->sector_type = SECT_AIR;
free_string(first->name);
first->name = str_dup("`CThe Skies`x");
free_string(first->description);
first->description = str_dup("");
}
else if(first->z < 0)
{
free_string(first->name);
first->name = str_dup("");
free_string(first->description);
first->description = str_dup("");
}
*/
    }
    prop->minx = 0;
    prop->miny = 0;
    prop->maxx = 0;
    prop->maxy = 0;
    for (int i = 0; i < 200; i++) {
      prop->roomlist[i] = 0;
      prop->objects[i] = 0;
    }
    prop->valid = FALSE;
  }

  int encroach_score(ROOM_INDEX_DATA *room) {
    if (room->sector_type == SECT_AIR)
    return 0;
    if (room->sector_type == SECT_ATMOSPHERE)
    return 0;

    if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
      if (room->z > 0)
      return 0;

      return 10;
    }

    int score = 0;
    for (int i = 0; i < 10; i++) {
      if (i == DIR_UP || i == DIR_DOWN)
      continue;

      if (room->exit[i] == NULL || room->exit[i]->u1.to_room == NULL)
      score += 2;
      else if (room->exit[i]->u1.to_room->encroachment >=
          500) // Changed from if to else if - Discordance crash 9-14-2015
      {
        if (IS_SET(room->exit[i]->u1.to_room->room_flags, ROOM_INDOORS))
        score += 10;
        else
        score += 5;
      }
      else if (room->exit[i]->u1.to_room->sector_type == SECT_FOREST)
      score += 6;
      else if (room->exit[i]->u1.to_room->sector_type == SECT_AIR)
      score += 2;
    }
    return score;
  }

  void make_hole(ROOM_INDEX_DATA *room) {
    if (!IS_SET(room->room_flags, ROOM_INDOORS))
    return;

    for (int i = number_range(0, 9); i < 10; i++) {
      if (i == DIR_UP || i == DIR_DOWN)
      continue;

      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL && room->exit[i]->u1.to_room->exit[rev_dir[i]] != NULL) {
        if (room->exit[i]->wall != WALL_NONE && room->exit[i]->wallcondition != WALLCOND_HOLE) {
          room->exit[i]->wallcondition = WALLCOND_HOLE;
          room->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
          WALLCOND_HOLE;
          return;
        }
      }
    }

    for (int i = 0; i < 10; i++) {
      if (i == DIR_UP || i == DIR_DOWN)
      continue;

      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL && room->exit[i]->u1.to_room->exit[rev_dir[i]] != NULL) {
        if (room->exit[i]->wall != WALL_NONE && room->exit[i]->wallcondition != WALLCOND_HOLE) {
          room->exit[i]->wallcondition = WALLCOND_HOLE;
          room->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
          WALLCOND_HOLE;
          return;
        }
      }
    }
    for (int i = 0; i < 10; i++) {
      if (i == DIR_UP || i == DIR_DOWN)
      continue;

      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL && room->exit[i]->u1.to_room->exit[rev_dir[i]] != NULL) {
        if (IS_SET(room->exit[i]->exit_info, EX_ISDOOR)) {
          if (IS_SET(room->exit[i]->exit_info, EX_CLOSED))
          REMOVE_BIT(room->exit[i]->exit_info, EX_CLOSED);
          room->exit[i]->doorbroken = 1;

          if (IS_SET(room->exit[i]->u1.to_room->exit[rev_dir[i]]->exit_info, EX_CLOSED))
          REMOVE_BIT(room->exit[i]->u1.to_room->exit[rev_dir[i]]->exit_info, EX_CLOSED);
          room->exit[i]->u1.to_room->exit[rev_dir[i]]->doorbroken = 1;
          return;
        }
      }
    }
    for (int i = 0; i < 10; i++) {
      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL && room->exit[i]->u1.to_room->exit[rev_dir[i]] != NULL) {
        if (room->exit[i]->wall != WALL_NONE && room->exit[i]->wallcondition != WALLCOND_HOLE) {
          room->exit[i]->wallcondition = WALLCOND_HOLE;
          room->exit[i]->u1.to_room->exit[rev_dir[i]]->wallcondition =
          WALLCOND_HOLE;
          return;
        }
      }
    }
  }
  void encroachment_cleanup() {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      PROP_TYPE *prop = (*it);

      if (prop->valid == FALSE || (prop->type_special != PROPERTY_INNERFOREST && prop->type_special != PROPERTY_OUTERFOREST))
      continue;

      if (prop->reclaim == TRUE) {
        char buf[MSL];
        sprintf(buf, "Reclaiming %s", prop->propname);
        log_string(buf);
        reclaim_property(prop);
        return;
      }
    }
  }

  void encroach_property(PROP_TYPE *prop)
  {
    int maxenc = 0;
    int count = 0;
    ROOM_INDEX_DATA *first = first_room_in_property(prop);

    for (; first != NULL && count < 200;
    first = next_room_in_property(prop, first)) {
      count++;
      if(first->encroachment > maxenc)
      maxenc = first->encroachment;
    }

    count = 0;
    first = first_room_in_property(prop);

    for (; first != NULL && count < 200;
    first = next_room_in_property(prop, first)) {
      count++;
      if(maxenc < 500)
      {
        if(number_percent() % 2 == 0)
        first->encroachment = 500;
      }
      else if(maxenc < 700)
      {
        if(number_percent() % 2 == 0)
        {
          make_hole(first);
          first->encroachment = 700;
        }
      }
      else
      {
        if(number_percent() % 2 == 0)
        {
          make_hole(first);
          first->encroachment = 900;
        }
      }
    }
    if(maxenc > 800 && (prop->type_special == PROPERTY_INNERFOREST || prop->type_special == PROPERTY_OUTERFOREST))
    {
      prop->reclaim = TRUE;
    }
    save_properties(FALSE);

  }


  void encroachment_update() {
    return;
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      PROP_TYPE *prop = (*it);

      if (prop->valid == FALSE || (prop->type_special != PROPERTY_INNERFOREST && prop->type_special != PROPERTY_OUTERFOREST))
      continue;

      if (prop->vnum % 60 != time_info.minute)
      continue;

      if (number_percent() % 9 != 0)
      continue;

      free_string(prop->address);
      prop->address = str_dup(
      create_address(get_room_index(prop->roadroom), prop->orientation));

      ROOM_INDEX_DATA *first = first_room_in_property(prop);
      int sun = sunphase(first);
      if (sun == 0 || sun != 0) {
        int count = 0;
        bool reclaim = TRUE;
        for (; first != NULL && count < 200;
        first = next_room_in_property(prop, first)) {
          count++;
          if (first->area->vnum >= 22 && first->area->vnum <= 25 && first->x >= first->area->minx + 24 && first->x <= first->area->minx + 26 && first->y >= first->area->miny + 2 && first->y <= first->area->miny + 26)
          continue;

          // Disco 1/8/2019
          if (first->sector_type == SECT_AIR) {
            if (first->encroachment > 0) {
              first->encroachment = 0;
            }
            continue;
          }

          int orig = first->encroachment;
          int score = encroach_score(first);
          score /= 2;
          if (prop->type_special == PROPERTY_OUTERFOREST)
          score = score * 2;
          if (!str_cmp(prop->owner, "NPC")) {
            score *= 10;
            prop->autoclear = 0;
          }
          first->encroachment += score;
          if (orig < 500 && first->encroachment >= 500 && IS_SET(first->room_flags, ROOM_INDOORS)) {
            if (prop->autoclear == 1) {
              char buf[MSL];
              sprintf(buf, "AUTOCLEAR: %s, %d, %d", prop->owner, first->vnum, first->encroachment);
              log_string(buf);
              house_charge(prop->owner, CLEAR_COST);
              first->encroachment = 0;
            }
          }
          if (orig < 700 && first->encroachment > 700)
          make_hole(first);
          if (orig < 800 && first->encroachment > 800) {
            make_hole(first);
            if (first->z < 0 && IS_SET(first->room_flags, ROOM_INDOORS))
            first->sector_type = SECT_SHALLOW;
          }
          if (orig < 900 && first->encroachment > 900)
          make_hole(first);
          if (orig < 1000 && first->encroachment > 1000 && IS_SET(first->room_flags, ROOM_INDOORS)) {
            first->sector_type = SECT_FOREST;
            if (IS_SET(first->room_flags, ROOM_INDOORS))
            TOGGLE_BIT(first->room_flags, ROOM_INDOORS);
            free_string(first->name);
            first->name = str_dup("`gAn overgrown area.`x");
            free_string(first->description);
            first->description = str_dup("");
          }
          if (first->encroachment < 750 && first->sector_type != SECT_FOREST)
          reclaim = FALSE;
        }
        if (reclaim == TRUE)
        prop->reclaim = TRUE;
      }
    }
  }

  void dawn_update(void) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      int loc = prop_location(*it);
      if (loc != PROPLOC_OTHER && loc != PROPLOC_WILDS && loc != PROPLOC_GODREALM && loc != PROPLOC_HELL)
      (*it)->warded = 0;
      else if ((*it)->warded > 50)
      (*it)->warded -= 2;
      if ((*it)->shroudshield > 20)
      (*it)->shroudshield -= 2;
      (*it)->decay = 0;
    }
  }

  void to_sleepers(CHAR_DATA *ch, PROP_TYPE *prop) {
    for (int i = 0; i < 50; i++) {
      if (!str_cmp(prop->sleepers[i], ch->name))
      return;
    }

    for (int i = 0; i < 50; i++) {
      if (safe_strlen(prop->sleepers[i]) < 2) {
        free_string(prop->sleepers[i]);
        prop->sleepers[i] = str_dup(ch->name);
        return;
      }
    }
  }
  void from_sleepers(CHAR_DATA *ch, PROP_TYPE *prop) {
    for (int i = 0; i < 50; i++) {
      if (!str_cmp(prop->sleepers[i], ch->name)) {
        free_string(prop->sleepers[i]);
        prop->sleepers[i] = str_dup("");
      }
    }
  }

  ROOM_INDEX_DATA *propfleeroom(PROP_TYPE *prop) {
    ROOM_INDEX_DATA *room = first_room_in_property(prop);
    if (room == NULL)
    return room;

    if (prop->type == PROP_SHOP) {
      return NULL;
    }

    if (IS_SET(room->room_flags, ROOM_INDOORS))
    return room;

    for (int i = 0; i < 200; i++) {
      room = next_room_in_property(prop, room);
      if (room == NULL)
      return room;
      if (IS_SET(room->room_flags, ROOM_INDOORS))
      return room;
    }
    return NULL;
  }

  ROOM_INDEX_DATA *get_fleeroom_precheck(CHAR_DATA *ch, PROP_TYPE *exclude) {
    ROOM_INDEX_DATA *desti;

    ROOM_INDEX_DATA * lquit = get_room_index(ch->pcdata->quit_room);
    if (lquit != NULL && IS_SET(lquit->room_flags, ROOM_BEDROOM))
    {
      if(prop_from_room(lquit) == NULL || prop_from_room(lquit) != exclude)
      return lquit;
    }
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if ((*it) == exclude)
      continue;
      if ((*it)->type == PROP_SHOP)
      continue;

      ROOM_INDEX_DATA *temp = first_room_in_property(*it);
      if (temp != NULL && temp->area->vnum > 21)
      continue;

      if (!str_cmp(NAME(ch), (*it)->owner)) {
        desti = propfleeroom((*it));
        if (desti != NULL)
        return desti;
      }
      for (int i = 0; i < 50; i++) {
        if (!str_cmp(NAME(ch), (*it)->tenants[i])) {
          desti = propfleeroom((*it));
          if (desti != NULL)
          return desti;
        }
      }
    }
    int area = number_range(16, 18);
    AREA_DATA *pArea = get_area_data(area);
    int x = number_range(pArea->minx, pArea->maxx);
    int y = number_range(pArea->miny, pArea->maxy);
    desti = room_by_coordinates(x, y, 0);
    if (desti != NULL) {
      if (!IS_SET(desti->room_flags, ROOM_CAMPSITE) && (desti->sector_type == SECT_FOREST || desti->sector_type == SECT_PARK)) {
        SET_BIT(desti->room_flags, ROOM_CAMPSITE);
        ch->money -= 2000;
      }
      return desti;
    }
    return NULL;
  }
  ROOM_INDEX_DATA *get_fleeroom(CHAR_DATA *ch, PROP_TYPE *exclude) {
    ROOM_INDEX_DATA *proom = get_fleeroom_precheck(ch, exclude);
    if(proom == NULL)
    return ch->in_room;
    if(clinic_patient(ch) && !institute_room(proom))
    return ch->in_room;
    if(ch->in_room->area->world != proom->area->world)
    return ch->in_room;

    return proom;

  }

  void autohouseflee(char *name, PROP_TYPE *prop, char *creepname) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    if (safe_strlen(name) < 2)
    return;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Auto house flee");

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
    if (online)
    return;
    /*
if(current_time - victim->lastlogoff < 300)
{
free_char(victim);
return;
}
*/
    if (IS_FLAG(victim->act, PLR_BOUND)) {
      free_char(victim);
      return;
    }
    if (is_weakness(NULL, victim) || IS_FLAG(victim->comm, COMM_MANDRAKE)) {
      if (number_percent() % 2 == 0) {
        free_char(victim);
        return;
      }
    }

    if (in_prop(victim) != NULL && prop != NULL && in_prop(victim) != prop) {
      free_char(victim);
      return;
    }
    if (victim->in_room == NULL || victim->in_room->vnum < 300) {
      free_char(victim);
      return;
    }
    if (victim->pcdata->patrol_habits[PATROL_RECKLESSHABIT] > 0) {
      free_char(victim);
      return;
    }

    ROOM_INDEX_DATA *fleeroom = get_fleeroom(victim, prop);
    if (fleeroom != NULL) {
      char_from_room(victim);
      char_to_room(victim, fleeroom);
      if (safe_strlen(creepname) < 2)
      append_messages(victim, "You flee the property in the night.");
      else {
        char buf[MSL];
        sprintf(buf, "You flee the property in the night, catching a glimpse of %s.", creepname);
        append_messages(victim, buf);
      }
      if (prop != NULL)
      from_sleepers(victim, prop);
      save_char_obj(victim, FALSE, FALSE);
      free_char(victim);
      return;
    }

    free_char(victim);
  }

  _DOFUN(do_houseeval) {
    PROP_TYPE *prop;
    if ((prop = prop_lookup_house(atoi(argument))) == NULL)
    return;

    ROOM_INDEX_DATA *first = first_room_in_property(prop);
    printf_to_char(ch, "%d, ", first->vnum);
    for (int i; i < 200; i++) {
      first = next_room_in_property(prop, first);
      if (first == NULL)
      return;
      printf_to_char(ch, "%d, ", first->vnum);
    }
  }

  _DOFUN(do_curtains) {
    char arg[MSL];
    int door;
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Close what curtains?\n\r", ch);
      return;
    }
    if (ch->in_room == NULL)
    return;
    if (is_helpless(ch) || in_fight(ch))
    return;
    if (is_ghost(ch)) {
      if (is_manifesting(ch)) {
        if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
          send_to_char("You can't muster the strength to do that.\n\r", ch);
          return;
        }
      }
      else {
        send_to_char("You must be prepared to manifest this power.\n\r", ch);
        return;
      }
    }
    if ((door = find_exit(ch, arg)) >= 0) {
      /* 'open door' */
      EXIT_DATA *pexit;

      pexit = ch->in_room->exit[door];
      if (pexit == NULL)
      return;
      if (pexit->wall != WALL_GLASS) {
        send_to_char("That isn't a window.\n\r", ch);
        return;
      }
      if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        send_to_char("I think the curtains are on the other side.\n\r", ch);
        return;
      }
      if (!IS_SET(pexit->exit_info, EX_CURTAINS)) {
        send_to_char("You close the curtains.\n\r", ch);
        act("$n closes the curtains.", ch, NULL, NULL, TO_ROOM);
        dact("$n closes the curtains.", ch, NULL, NULL, DISTANCE_MEDIUM);
        SET_BIT(pexit->exit_info, EX_CURTAINS);
        if (!IS_SET(pexit->rs_flags, EX_CURTAINS))
        SET_BIT(pexit->rs_flags, EX_CURTAINS);
        if (pexit->u1.to_room->exit[rev_dir[door]] != NULL) {
          if (!IS_SET(pexit->u1.to_room->exit[rev_dir[door]]->exit_info, EX_CURTAINS))
          SET_BIT(pexit->u1.to_room->exit[rev_dir[door]]->exit_info, EX_CURTAINS);
          if (!IS_SET(pexit->u1.to_room->exit[rev_dir[door]]->rs_flags, EX_CURTAINS))
          SET_BIT(pexit->u1.to_room->exit[rev_dir[door]]->rs_flags, EX_CURTAINS);
        }
        return;
      }
      else {
        REMOVE_BIT(pexit->exit_info, EX_CURTAINS);
        if (IS_SET(pexit->rs_flags, EX_CURTAINS))
        REMOVE_BIT(pexit->rs_flags, EX_CURTAINS);
        if (pexit->u1.to_room->exit[rev_dir[door]] != NULL) {
          if (IS_SET(pexit->u1.to_room->exit[rev_dir[door]]->exit_info, EX_CURTAINS))
          REMOVE_BIT(pexit->u1.to_room->exit[rev_dir[door]]->exit_info, EX_CURTAINS);
          if (IS_SET(pexit->u1.to_room->exit[rev_dir[door]]->rs_flags, EX_CURTAINS))
          REMOVE_BIT(pexit->u1.to_room->exit[rev_dir[door]]->rs_flags, EX_CURTAINS);
        }
        send_to_char("You open the curtains.\n\r", ch);
        act("$n opens the curtains.", ch, NULL, NULL, TO_ROOM);
        dact("$n opens the curtains.", ch, NULL, NULL, DISTANCE_MEDIUM);
        return;
      }
    }
    send_to_char("I see no curtains there.\n\r", ch);
  }

  int prop_location(PROP_TYPE *prop) {
    ROOM_INDEX_DATA *room;
    room = get_room_index(prop->roadroom);
    if (room == NULL)
    room = get_room_index(prop->firstroom);
    if (room == NULL)
    return PROPLOC_TOWN;

    if (room->area->vnum == 13 || room->area->vnum == 14)
    return PROPLOC_TOWN;
    if (room->area->vnum >= 16 && room->area->vnum <= 18)
    return PROPLOC_INNER;
    if (room->area->vnum >= 19 && room->area->vnum <= 21)
    return PROPLOC_OUTER;
    if (room->area->vnum == 22)
    return PROPLOC_OTHER;
    if (room->area->vnum == 23)
    return PROPLOC_WILDS;
    if (room->area->vnum == 24)
    return PROPLOC_GODREALM;
    if (room->area->vnum == 25)
    return PROPLOC_HELL;
    return PROPLOC_TOWN;
  }

  _DOFUN(do_lock) {
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg[MSL];
    if (ch->in_room == NULL)
    return;

    one_argument(argument, arg);

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) {

      if (in_sheriff(ch->in_room) && ch->race != RACE_DEPUTY) {
        send_to_char("`cOnly members of the sheriff`g'`cs department have keys`g.`x", ch);
        return;
      }
      if (district_room(ch->in_room) != DISTRICT_INSTITUTE) {
        send_to_char("`cThere aren`g'`ct any lockable doors`g.`x", ch);
        return;
      }
      else if (!institute_staff(ch) || institute_suspension(ch)) {
        send_to_char("`cYou don`g'`ct have the key`g.`x", ch);
        return;
      }
      int value, rev;
      value = flag_value(exit_flags, "door");
      ROOM_INDEX_DATA *pToRoom;
      int door = -1;
      for (int f = 0; f < 10; f++) {
        if (!str_cmp(argument, dir_name[f][0]))
        door = f;
      }

      if (door < 0) {
        send_to_char("`cExit doesn`g'`ct exist`g.`x\n\r", ch);
        return;
      }

      if (!ch->in_room->exit[door]) {
        send_to_char("`cExit doesn`g'`ct exist`g.\n\r", ch);
        return;
      }

      if (ch->in_room->exit[door]->doorbroken > 0) {
        send_to_char("`cThat`g'`cs too badly damaged right now`g.`x\n\r", ch);
        return;
      }

      value = flag_value(exit_flags, "locked");
      if (!IS_SET(ch->in_room->exit[door]->exit_info, value)) {
        act("`cYou lock the door`g.`x", ch, NULL, NULL, TO_CHAR);
        act("`c$n locks the door`g.`x", ch, NULL, NULL, TO_ROOM);
      }
      else {
        act("`cYou unlock the door`g.`x", ch, NULL, NULL, TO_CHAR);
        act("`c$n unlocks the door`g.`x", ch, NULL, NULL, TO_ROOM);
      }

      // locking door
      TOGGLE_BIT(ch->in_room->exit[door]->rs_flags, value);
      ch->in_room->exit[door]->exit_info = ch->in_room->exit[door]->rs_flags;

      pToRoom = ch->in_room->exit[door]->u1.to_room;
      rev = rev_dir[door];

      // reverse side
      if (pToRoom->exit[rev] != NULL) {
        TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
        pToRoom->exit[rev]->rs_flags = ch->in_room->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = ch->in_room->exit[door]->exit_info;
      }

      // closing door if not closed
      value = flag_value(exit_flags, "closed");
      if (!IS_SET(ch->in_room->exit[door]->exit_info, value)) {
        TOGGLE_BIT(ch->in_room->exit[door]->rs_flags, value);
        ch->in_room->exit[door]->exit_info = ch->in_room->exit[door]->rs_flags;

        // reverse side
        if (pToRoom->exit[rev] != NULL) {
          TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
          pToRoom->exit[rev]->rs_flags = ch->in_room->exit[door]->rs_flags;
          pToRoom->exit[rev]->exit_info = ch->in_room->exit[door]->exit_info;
        }
      }

      SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

      return;
    }

    bool objectfound = FALSE;
    for (obj = victim->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->pIndexData->vnum == 75 || obj->pIndexData->vnum == ITEM_NEUTRALCOLLAR) {
        objectfound = TRUE;
        break;
      }
    }

    if (!objectfound) {
      send_to_char("There's nothing to lock or unlock on that person.\n\r", ch);
      return;
    }

    if (!is_helpless(victim) && !IS_IMMORTAL(ch)) {
      send_to_char("They need to be restrained or helpless.\n\r", ch);
      return;
    }

    if (IS_SET(obj->extra_flags, ITEM_LOCK)) {
      if (!strcmp(arg, "self") || victim == ch) {
        send_to_char("You can't seem to unlock it yourself.\n\r", ch);
        return;
      }

      act("`cYou unlock the collar`g.`x", ch, NULL, NULL, TO_CHAR);
      act("`c$n unlocks the collar`g.`x", ch, NULL, NULL, TO_ROOM);
      REMOVE_BIT(obj->extra_flags, ITEM_LOCK);
    }
    else {
      act("`cYou lock the collar`g.`x", ch, NULL, NULL, TO_CHAR);
      act("`c$n locks the collar`g.`x", ch, NULL, NULL, TO_ROOM);
      SET_BIT(obj->extra_flags, ITEM_LOCK);
    }

    return;
  }

  void market_link(int marketvnum, int propvnum, int propdir, int marketdir) {
    int value;
    ROOM_INDEX_DATA *marketroom = get_room_index(marketvnum);
    ROOM_INDEX_DATA *proproom = get_room_index(propvnum);
    if (marketroom == NULL || proproom == NULL)
    return;
    ROOM_INDEX_DATA *orig = marketroom->exit[marketdir]->u1.to_room;
    int olddoor = -1;
    if (orig != NULL) {
      for (int odoor = 0; odoor <= 9; odoor++) {
        if (orig->exit[odoor] != NULL && orig->exit[odoor]->u1.to_room == marketroom)
        olddoor = odoor;
      }
    }
    if (olddoor != -1) {
      orig->exit[olddoor]->u1.to_room = cardinal_room(orig, olddoor);
      orig->exit[olddoor]->wall = WALL_BRICK;
      value = flag_value(exit_flags, "door");
      TOGGLE_BIT(orig->exit[olddoor]->rs_flags, value);
      value = flag_value(exit_flags, "closed");
      TOGGLE_BIT(orig->exit[olddoor]->rs_flags, value);
      value = flag_value(exit_flags, "locked");
      TOGGLE_BIT(orig->exit[olddoor]->rs_flags, value);
      orig->exit[olddoor]->exit_info = orig->exit[olddoor]->rs_flags;

      if (!IS_SET(orig->area->area_flags, AREA_CHANGED))
      SET_BIT(orig->area->area_flags, AREA_CHANGED);
    }
    marketroom->exit[marketdir]->u1.to_room = proproom;
    marketroom->exit[marketdir]->wall = WALL_NONE;
    if (!IS_SET(marketroom->area->area_flags, AREA_CHANGED))
    SET_BIT(marketroom->area->area_flags, AREA_CHANGED);
    proproom->exit[propdir]->u1.to_room = marketroom;
    proproom->exit[propdir]->wall = WALL_NONE;
    value = flag_value(exit_flags, "door");
    TOGGLE_BIT(proproom->exit[propdir]->rs_flags, value);
    value = flag_value(exit_flags, "closed");
    TOGGLE_BIT(proproom->exit[propdir]->rs_flags, value);
    value = flag_value(exit_flags, "locked");
    TOGGLE_BIT(proproom->exit[propdir]->rs_flags, value);
    proproom->exit[propdir]->exit_info = proproom->exit[propdir]->rs_flags;
    if (!IS_SET(proproom->area->area_flags, AREA_CHANGED))
    SET_BIT(proproom->area->area_flags, AREA_CHANGED);
  }

  void market_update(void) {
    int farsouth = 0;
    int south = 0;
    int haven = 0;
    int west = 0;
    int farwest = 0;
    int north = 0;
    int farnorth = 0;
    char buf[MSL];
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      PROP_TYPE *prop = (*it);
      if (prop->valid == FALSE || prop->type != PROP_SHOP)
      continue;
      if (prop->market_room == 0)
      continue;
      if (get_room_index(prop->market_room) == NULL || get_room_index(prop->market_room)->exit[prop->market_dir] == NULL || get_room_index(prop->market_room)->exit[prop->market_dir]->wall !=
          WALL_BRICK)
      continue;
      ROOM_INDEX_DATA *first = first_room_in_property(prop);
      if (first == NULL)
      continue;
      sprintf(buf, "MARKET: Considering %s:%d", from_color(prop->propname), first->area->vnum);
      log_string(buf);
      if (first->area->vnum >= 13 && first->area->vnum <= 15)
      haven++;
      if (first->area->vnum == 16)
      north++;
      if (first->area->vnum == 17)
      south++;
      if (first->area->vnum == 18)
      west++;
      if (first->area->vnum == 19)
      farnorth++;
      if (first->area->vnum == 20)
      farsouth++;
      if (first->area->vnum == 21)
      farwest++;
    }
    sprintf(buf, "MARKET: h, %d, n, %d, s, %d, w, %d, fn, %d, fs, %d, fw, %d", haven, north, south, west, farnorth, farsouth, farwest);
    log_string(buf);

    if (haven > 0)
    haven = number_range(1, haven);
    if (north > 0)
    north = number_range(1, north);
    if (south > 0)
    south = number_range(1, south);
    if (west > 0)
    west = number_range(1, west);
    if (farnorth > 0)
    farnorth = number_range(1, farnorth);
    if (farsouth > 0)
    farsouth = number_range(1, farsouth);
    if (farwest > 0)
    farwest = number_range(1, farwest);

    sprintf(buf, "MARKET: h, %d, n, %d, s, %d, w, %d, fn, %d, fs, %d, fw, %d", haven, north, south, west, farnorth, farsouth, farwest);
    log_string(buf);

    int hpoint = 1;
    int npoint = 1;
    int spoint = 1;
    int wpoint = 1;
    int fnpoint = 1;
    int fspoint = 1;
    int fwpoint = 1;

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      PROP_TYPE *prop = (*it);
      if (prop->valid == FALSE || prop->type != PROP_SHOP)
      continue;
      if (prop->market_room == 0)
      continue;
      if (get_room_index(prop->market_room) == NULL || get_room_index(prop->market_room)->exit[prop->market_dir] == NULL || get_room_index(prop->market_room)->exit[prop->market_dir]->wall !=
          WALL_BRICK)
      continue;
      ROOM_INDEX_DATA *first = first_room_in_property(prop);
      if (first == NULL)
      continue;
      if (first->area->vnum >= 13 && first->area->vnum <= 15) {
        if (haven == hpoint) {
          sprintf(buf, "MARKET 405002E: %s, H, %d HP %d", from_color(prop->propname), haven, hpoint);
          log_string(buf);
          market_link(405002, prop->market_room, prop->market_dir, DIR_EAST);
        }
        else if (haven == hpoint + 1) {
          sprintf(buf, "MARKET 405002W: %s, H, %d HP %d", from_color(prop->propname), haven, hpoint);
          log_string(buf);

          market_link(405002, prop->market_room, prop->market_dir, DIR_WEST);
        }
        hpoint++;
      }
      if (first->area->vnum == 16) {
        if (north == npoint) {
          market_link(405003, prop->market_room, prop->market_dir, DIR_EAST);
          sprintf(buf, "MARKET 405003E: %s, N, %d NP %d", prop->propname, north, npoint);
          log_string(buf);
        }
        npoint++;
      }
      if (first->area->vnum == 17) {
        if (south == spoint) {
          sprintf(buf, "MARKET 405000E: %s, S, %d SP %d", prop->propname, south, spoint);
          log_string(buf);
          market_link(405000, prop->market_room, prop->market_dir, DIR_EAST);
        }
        spoint++;
      }
      if (first->area->vnum == 18) {
        if (west == wpoint) {
          sprintf(buf, "MARKET 405001E: %s, W, %d WP %d", prop->propname, west, wpoint);
          log_string(buf);
          market_link(405001, prop->market_room, prop->market_dir, DIR_EAST);
        }
        wpoint++;
      }
      if (first->area->vnum == 19) {
        if (farnorth == fnpoint) {
          sprintf(buf, "MARKET 405003W: %s,FN, %d FNP %d", prop->propname, farnorth, fnpoint);
          log_string(buf);
          market_link(405003, prop->market_room, prop->market_dir, DIR_WEST);
        }
        fnpoint++;
      }
      if (first->area->vnum == 20) {
        if (farsouth == fspoint) {
          sprintf(buf, "MARKET 405000W: %s, FS, %d FSP %d", prop->propname, farsouth, fspoint);
          log_string(buf);
          market_link(405000, prop->market_room, prop->market_dir, DIR_WEST);
        }
        fspoint++;
      }
      if (first->area->vnum == 21) {
        if (farwest == fwpoint) {
          sprintf(buf, "MARKET 405001W: %s, FW, %d FWP %d", prop->propname, farwest, fwpoint);
          log_string(buf);
          market_link(405001, prop->market_room, prop->market_dir, DIR_WEST);
        }
        fwpoint++;
      }
    }
  }

  _DOFUN(do_propcheck) {
    ROOM_INDEX_DATA *room = ch->in_room;
    PROP_TYPE *prop;
    if ((prop = prop_from_room(room)) != NULL) {
      printf_to_char(ch, "%s, %s, %s, %d %d %d %d %d %d.\n\r", prop->propname, prop->address, prop->owner, prop->timefrozen, prop->timeshift, prop->tempfrozen, prop->tempshift, prop->weather, prop->market_room);
    }
  }

  _DOFUN(do_encroachment) {
    encroachment_update();
    encroachment_cleanup();
  }

  int district_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return DISTRICT_ANYWHERE;
    if (room->x >= 1 && room->x <= 23 && room->y >= 48 && room->y <= 71)
    return DISTRICT_INSTITUTE;
    if (room->x >= 22 && room->x <= 30 && room->y >= 1 && room->y <= 22)
    return DISTRICT_WESTHAVEN;
    if (room->y < 72 && room->x >= 39 && room->x <= 54 && room->y >= 35)
    return DISTRICT_TOURIST;
    if (room->y < 72 && room->y >= 50 && room->x < 39 && room->x >= 20)
    return DISTRICT_URBAN;
    if (room->y < 72 && room->y > 35 && room->x < 26 && room->x >= 0)
    return DISTRICT_REDLIGHT;
    if (room->y < 35 && room->y >= 0 && room->x > 0 && room->x <= 72)
    return DISTRICT_HISTORIC;

    return DISTRICT_ANYWHERE;
  }

  bool valid_industry(int industry, int district) {
    if (district == DISTRICT_URBAN) {
      if (industry == INDUSTRY_ONLINE)
      return TRUE;
      if (industry == INDUSTRY_STORE)
      return TRUE;
      if (industry == INDUSTRY_GAMES)
      return TRUE;
      if (industry == INDUSTRY_BAR)
      return TRUE;
      if (industry == INDUSTRY_FINANCIAL)
      return TRUE;
      if (industry == INDUSTRY_RESEARCH)
      return TRUE;
      if (industry == INDUSTRY_EDUCATION)
      return TRUE;
      if (industry == INDUSTRY_TAKEOUT)
      return TRUE;
      if (industry == INDUSTRY_TECHNOLOGY)
      return TRUE;
      if (industry == INDUSTRY_JOURNALISM)
      return TRUE;
      if (industry == INDUSTRY_LEGAL)
      return TRUE;
      if (industry == INDUSTRY_NONPROFIT)
      return TRUE;
      return FALSE;
    }
    if (district == DISTRICT_TOURIST) {
      if (industry == INDUSTRY_RESTAURANT)
      return TRUE;
      if (industry == INDUSTRY_CAFE)
      return TRUE;
      if (industry == INDUSTRY_FINANCIAL)
      return TRUE;
      if (industry == INDUSTRY_NONPROFIT)
      return TRUE;
      if (industry == INDUSTRY_HOSPITALITY)
      return TRUE;
      if (industry == INDUSTRY_CLOTHES)
      return TRUE;
      if (industry == INDUSTRY_ARTIST)
      return TRUE;
      return FALSE;
    }
    if (district == DISTRICT_REDLIGHT) {
      if (industry == INDUSTRY_CONSTRUCTION)
      return TRUE;
      if (industry == INDUSTRY_CLOTHES)
      return TRUE;
      if (industry == INDUSTRY_STORE)
      return TRUE;
      if (industry == INDUSTRY_BAR)
      return TRUE;
      if (industry == INDUSTRY_SEX)
      return TRUE;
      if (industry == INDUSTRY_CLUB)
      return TRUE;
      if (industry == INDUSTRY_SECURITY)
      return TRUE;
      if (industry == INDUSTRY_INVESTIGATION)
      return TRUE;
      if (industry == INDUSTRY_FACTORY)
      return TRUE;
      if (industry == INDUSTRY_MSERVICE)
      return TRUE;
      if (industry == INDUSTRY_SSERVICE)
      return TRUE;
      if (industry == INDUSTRY_TAKEOUT)
      return TRUE;
      if (industry == INDUSTRY_TRANSPORT)
      return TRUE;
      if (industry == INDUSTRY_ARTIST)
      return TRUE;
      if (industry == INDUSTRY_GAMES)
      return TRUE;
      if (industry == INDUSTRY_HEAD)
      return TRUE;
      return FALSE;
    }
    if (district == DISTRICT_HISTORIC) {
      if (industry == INDUSTRY_CLOTHES)
      return TRUE;
      if (industry == INDUSTRY_CAFE)
      return TRUE;
      if (industry == INDUSTRY_FINANCIAL)
      return TRUE;
      if (industry == INDUSTRY_TAKEOUT)
      return TRUE;
      if (industry == INDUSTRY_LEGAL)
      return TRUE;
      if (industry == INDUSTRY_EDUCATION)
      return TRUE;
      if (industry == INDUSTRY_NONPROFIT)
      return TRUE;
      return FALSE;
    }

    return TRUE;
  }

  _DOFUN(do_reclaimroom) { reclaim_room(ch->in_room); }

  bool protected_room(ROOM_INDEX_DATA *room) {
    if (room->sector_type == SECT_WATER)
    return TRUE;
    if (room->sector_type == SECT_UNDERWATER)
    return TRUE;
    if (room->sector_type == SECT_SHALLOW)
    return TRUE;
    if (room->vnum == 302496)
    return TRUE;
    if (room->vnum == 302598)
    return TRUE;
    if (room->vnum == 302700)
    return TRUE;
    if (room->vnum == 302702)
    return TRUE;
    if (room->vnum == 302704)
    return TRUE;
    if (room->vnum == 302602)
    return TRUE;
    if (room->vnum == 302500)
    return TRUE;
    if (room->vnum == 302498)
    return TRUE;
    if (room->vnum == 302600)
    return TRUE;

    if (room->vnum == 322496)
    return TRUE;
    if (room->vnum == 322598)
    return TRUE;
    if (room->vnum == 322700)
    return TRUE;
    if (room->vnum == 322702)
    return TRUE;
    if (room->vnum == 322704)
    return TRUE;
    if (room->vnum == 322602)
    return TRUE;
    if (room->vnum == 322500)
    return TRUE;
    if (room->vnum == 322498)
    return TRUE;
    if (room->vnum == 322600)
    return TRUE;

    if (room->vnum == 342496)
    return TRUE;
    if (room->vnum == 342598)
    return TRUE;
    if (room->vnum == 342700)
    return TRUE;
    if (room->vnum == 342702)
    return TRUE;
    if (room->vnum == 342704)
    return TRUE;
    if (room->vnum == 342602)
    return TRUE;
    if (room->vnum == 342500)
    return TRUE;
    if (room->vnum == 342498)
    return TRUE;
    if (room->vnum == 342600)
    return TRUE;

    if (room->vnum == 362496)
    return TRUE;
    if (room->vnum == 362598)
    return TRUE;
    if (room->vnum == 362700)
    return TRUE;
    if (room->vnum == 362702)
    return TRUE;
    if (room->vnum == 362704)
    return TRUE;
    if (room->vnum == 362602)
    return TRUE;
    if (room->vnum == 362500)
    return TRUE;
    if (room->vnum == 362498)
    return TRUE;
    if (room->vnum == 362600)
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_reclaimarea) {
    int point = atoi(argument);
    if (get_area_data(point) == NULL)
    return;
    for (int j = get_area_data(point)->min_vnum;
    j < get_area_data(point)->max_vnum; j++) {
      ROOM_INDEX_DATA *room = get_room_index(j);
      if (room != NULL && !protected_room(room))
      // room->sector_type != SECT_STREET && prop_from_room(room) == NULL)
      reclaim_room(room);
    }
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_cleanupprops) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0') {
        continue;
      }
      if (!str_cmp((*it)->propname, "delete") || (*it)->valid == FALSE)
      continue;

      if (!prop_exists((*it)))
      continue;

      for (int x = 0; x < 200; x++) {
        if ((*it)->roomlist[x] != 0) {
          ROOM_INDEX_DATA *temp2 = get_room_index((*it)->roomlist[x]);
          if (temp2 != NULL) {
            bool found = FALSE;
            for (int x = 0; x < 10; x++) {
              if (temp2->exit[x] != NULL && temp2->exit[x]->u1.to_room != NULL) {
                PROP_TYPE *ptype = prop_from_room(temp2->exit[x]->u1.to_room);
                if (isInvalid(ptype) == false && isInvalid(reinterpret_cast<const void *>(ptype->vnum)) ==
                    false && ptype->vnum == (*it)->vnum)
                found = TRUE;
              }
            }
            if (found == FALSE)
            (*it)->roomlist[x] = 0;
          }
        }
      }
    }
  }

  void security_wipeout(char *cname) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) {
        continue;
      }
      if (!str_cmp((*it)->owner, cname)) {
        ROOM_INDEX_DATA *start = first_room_in_property(*it);
        if (start == NULL)
        return;
        start->security = 0;
        start->toughness = 0;
        for (int i = 0; i < 300; i++) {
          start = next_room_in_property((*it), start);
          if (start == NULL)
          return;
          start->security = 0;
          start->toughness = 0;
        }
      }
    }
  }

  _DOFUN(do_newdesc)
  {
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup(argument);
    ROOM_INDEX_DATA *troom = ch->in_room;
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == troom->area)
        {
          if(!str_cmp(troom->name, pRoomIndex->name) && !str_cmp(roomtitle(troom, FALSE), roomtitle(pRoomIndex, FALSE)))
          {
            free_string(pRoomIndex->description);
            pRoomIndex->description = str_dup(argument);
          }

        }
      }
    }
    if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

    send_to_char("Done.\n\r", ch);
  }


  void fix_basement_room(ROOM_INDEX_DATA *room)
  {
    if(room->z >= 0)
    return;

    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    ROOM_INDEX_DATA *to_room;
    if(room->z == -1)
    {
      pexit = room->exit[DIR_UP];
      if(!IS_SET(pexit->exit_info, EX_CLOSED))
      {
        to_room = pexit->u1.to_room;
        pexit_rev = to_room->exit[DIR_DOWN];
        pexit->wall = WALL_BRICK;
        pexit_rev->wall = WALL_BRICK;

      }
    }
    if (!IS_SET(room->room_flags, ROOM_INDOORS))
    SET_BIT(room->room_flags, ROOM_INDOORS);
    room->decor = 0;
    room->toughness = 0;
    room->security = 0;

  }



  void fix_basement(PROP_TYPE *prop)
  {
    ROOM_INDEX_DATA *start = first_room_in_property(prop);
    if(start == NULL)
    return;
    fix_basement_room(start);
    for (int i = 0; i < 300; i++) {
      start = next_room_in_property(prop, start);
      if (start == NULL)
      return;
      fix_basement_room(start);
    }

  }

  _DOFUN(do_basementfix)
  {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if (!(*it)->propname || (*it)->propname[0] == '\0' || (*it)->valid == FALSE) {
        continue;
      }
      fix_basement(*it);
    }
  }

  bool sandbox_room(ROOM_INDEX_DATA *room)
  {
    if(room->area->vnum != DIST_MISTS)
    return FALSE;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if((*it)->vnum == room->vnum)
      return FALSE;
    }
    return TRUE;
  }


#if defined(__cplusplus)
}
#endif

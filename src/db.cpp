#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif

#include "Note.h"

#if defined(WIN32)
#include <process.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#include "structs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(macintosh)
  extern	int	_filbuf		args( (FILE *) );
#endif

#if defined(_WIN32)
#define random rand
#define srandom srand
#endif


  /* externals for counting purposes */
  extern ObjList obj_free;
  extern CharList char_free;
  extern DescList descriptor_free;
  extern PC_DATA *pcdata_free;
  extern AFFECT_DATA *affect_free;

  /*
* Globals.
*/

  HELP_DATA *help_first;
  HELP_DATA *help_last;

  HELP_AREA *had_list;

  ShopList shop_list;

  EXIT_DATA *exit_upd_list = NULL;
  ROOM_INDEX_DATA *room_upd_list = NULL;
  GROUP_INDEX_DATA *group_list;

  ProgList mprog_list;
  ProgList oprog_list;
  ProgList rprog_list;

  char bug_buf[2 * MAX_INPUT_LENGTH];
  CharList char_list;
  ChestList chest_list;
  char *help_greeting[3];
  char *help_inferno;
  char *help_story;
  char *help_motd;
  char *troll_ip;
  int greeting_count = 0;
  char log_buf[2 * MAX_INPUT_LENGTH];
  ObjList object_list;
  TIME_INFO_DATA time_info;

  int maxCommands;
  struct cmd_type *cmd_table;

  /*
* Locals.
*/
  MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
  GROUP_INDEX_DATA *group_index_hash[MAX_KEY_HASH];
  OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
  ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
  char *string_hash[MAX_KEY_HASH];

  AreaList area_list;
  AREA_DATA *area_last;
  AREA_DATA *current_area;

  void mob_wealth args((CHAR_DATA * mob));
  void load_quotes args((void));

  char *string_space;
  char *top_string;
  char str_empty[1];

  int top_affect;
  int top_area;
  int top_ed;
  int top_place;
  int top_exit;
  int top_help;
  int top_mob_index;
  int top_obj_index;
  int top_group_index;
  int top_reset;
  int top_room;
  int top_shop;
  int top_vnum_room;   /* OLC */
  int top_vnum_mob;    /* OLC */
  int top_vnum_obj;    /* OLC */
  int top_mprog_index; /* OLC */
  int top_oprog_index; /* OLC */
  int top_rprog_index; /* OLC */
  int mobile_count = 0;
  int newmobs = 0;
  int newobjs = 0;
  int num_quotes;

  int nAllocString;
  int sAllocString;

  /*
* These are skill_lookup return values for common skills and spells.
* THIS FILE HAS BEEN NICELY FORMATTED AND ALPHEBETIZED.  DO NOT MESS
* IT UP!!!  - Scaelorn
*/

  /**
* These are the few weaves that need gsns in the code.
*/

  /*
* Semi-locals.
*/
  bool fBootDb;
  FILE *fpArea;
  char strArea[MAX_INPUT_LENGTH];

  /*
* Local booting procedures.
*/
  void init_mm args((void));
  void load_area args((FILE * fp)); /* OLC */
  void load_helps args((FILE * fp, char *fname));
  void load_mobiles args((FILE * fp));
  void load_groups args((FILE * fp));
  void load_objects args((FILE * fp));
  void load_resets args((FILE * fp));
  void load_rooms args((FILE * fp));
  void load_shops args((FILE * fp));
  void load_specials args((FILE * fp));
  void loadNotes args((void));
  void load_bans args((void));
  void load_mobprogs args((FILE * fp));
  void load_objprogs args((FILE * fp));
  void load_roomprogs args((FILE * fp));
  void load_subraces args((void));

  void fix_exits args((void));
  void fix_mobprogs args((void));
  void fix_objprogs args((void));
  void fix_roomprogs args((void));

  void reset_area args((AREA_DATA * pArea, bool runtime));

  /*
* Big mama top level function.
*/
  void boot_db() {
    /*
* Init some data space stuff.
*/
    {
      strspace_alloc();
      top_string = string_space;
      fBootDb = TRUE;
    }

    log_string("------------------[ Boot Log ]------------------");

    log_string("Loading System Configuration...");
    if (!load_system_config()) {
      perror("Error while loading System Config.");
      exit(1);
    }

    log_string("Loading Commands...");
    load_commands();

    log_string("Loading Clans...");
    load_clans();

    log_string("Loading Operations...");
    load_operations();

    log_string("Loading Cabals...");

    load_cabals();
    initiate_demon_holes();

    log_string("Loading Graves...");

    load_graves();
    log_string("Loading Lairs...");

    load_lairs();
    log_string("Loading Rosterchars...");

    load_rosterchars();
    log_string("Loading Storylines...");

    load_storylines();
    log_string("Loading Fantasies...");

    load_fantasies();

    log_string("Loading Destinies...");

    load_destinies();

    log_string("Loading Galleries...");

    load_galleries();
    log_string("Loading Pages...");

    load_pages();
    log_string("Loading Archives...");

    load_archive_indexes();

    log_string("Loading Playerrooms...");

    load_playerrooms();

    log_string("Loading Institute...");
    load_institutes();

    log_string("Loading Phonebook...");
    load_phonebooks();

    log_string("Loading Map Distances...");

    setup_translation();

    log_string("Loading Names...");

    load_names();

    log_string("Loading Stories...");

    load_stories();

    log_string("Loading Plots...");

    load_plots();

    log_string("Loading Storyideas...");

    load_storyideas();

    log_string("Loading Petitions...");

    load_petitions();

    log_string("Loading News...");

    load_news();

    log_string("Loading Research...");

    load_research();


    log_string("Loading Encounters...");

    load_eencounter();


    log_string("Loading Patrols...");

    load_epatrol();

    log_string("Loading Anniversaries...");

    load_anniversary();

    log_string("Loading Decrees...");

    load_decrees();

    log_string("Loading Grouptexts...");

    load_grouptexts();


    log_string("Loading Test Histories...");

    load_texthistories();

    log_string("Loading Chatrooms...");

    load_chatrooms();

    log_string("Loading Profiles...");

    load_profiles();

    log_string("Loading Matches...");

    load_matches();


    log_string("Loading Locations...");

    load_locations();


    troll_ip = "";

    log_string("Loading Races...");
    load_events();
    log_string("Loading Weekly...");

    load_weekly_characters();

    log_string("Loading Areas...");

    /*
* Init random number generator.
*/
    { init_mm(); }

    /*
* Assign gsn's for blademaster forms
*/

    /*
* Read in all the area files.
*/
    {
      FILE *fpList;

      if ((fpList = fopen(AREA_LIST, "r")) == NULL) {
        perror(AREA_LIST);
        exit(1);
      }

      for (;;) {
        strcpy(strArea, fread_word(fpList));
        if (strArea[0] == '$')
        break;

        if (strArea[0] == '-') {
          fpArea = stdin;
        }
        else {
          log_string("Loading Subarea");
          log_string(strArea);

          if ((fpArea = fopen(strArea, "r")) == NULL) {
            perror(strArea);
            exit(1);
          }
        }

        current_area = NULL;
        for (;;) {
          char *word;

          if ((fread_letter(fpArea)) != '#') {
            bug("Boot_db: # not found.", 0);
            exit(1);
          }

          word = fread_word(fpArea);
          if (word[0] == '$')
          break;

          else if (!str_cmp(word, "AREADATA"))
          load_area(fpArea);
          else if (!str_cmp(word, "HELPS"))
          load_helps(fpArea, strArea);
          else if (!str_cmp(word, "MOBILES"))
          load_mobiles(fpArea);
          else if (!str_cmp(word, "GROUPS"))
          load_groups(fpArea);
          else if (!str_cmp(word, "MOBPROGS"))
          load_mobprogs(fpArea);
          else if (!str_cmp(word, "OBJPROGS"))
          load_objprogs(fpArea);
          else if (!str_cmp(word, "ROOMPROGS"))
          load_roomprogs(fpArea);
          else if (!str_cmp(word, "OBJECTS"))
          load_objects(fpArea);
          else if (!str_cmp(word, "RESETS"))
          load_resets(fpArea);
          else if (!str_cmp(word, "ROOMS"))
          load_rooms(fpArea);
          else if (!str_cmp(word, "SHOPS"))
          load_shops(fpArea);
          else if (!str_cmp(word, "SPECIALS"))
          load_specials(fpArea);
          else {
            bug("Boot_db: bad section name.", 0);
            exit(1);
          }
        }

        if (fpArea != stdin)
        fclose(fpArea);
        fpArea = NULL;
      }
      fclose(fpList);
    }

    /*
* Set time and weather.
*/
    {
      FILE *weather;

      if ((weather = fopen(WEATHER_FILE, "r")) != NULL) {
        log_string("Here");
        /* Read in the values for what day/month it is. (Year too!) */
        time_info.minute = fread_number(weather);
        time_info.hour = fread_number(weather);
        time_info.day = fread_number(weather);
        time_info.month = fread_number(weather);
        time_info.year = fread_number(weather);
        time_info.local_temp = fread_number(weather);
        time_info.des_temp = fread_number(weather);
        time_info.local_cover_total = fread_number(weather);
        time_info.cover_trend = fread_number(weather);
        time_info.density_trend = fread_number(weather);
        time_info.local_density_total = fread_number(weather);
        time_info.understanding = fread_number(weather);
        time_info.phone = fread_number(weather);
        time_info.local_mist_level = fread_number(weather);
        time_info.mist_timer = fread_number(weather);
        time_info.faction_vnum = fread_number(weather);
        time_info.monster_hours = fread_number(weather);
        time_info.mutilate_hours = fread_number(weather);
        time_info.cult_alliance_issue = fread_number(weather);
        time_info.cult_alliance_type = fread_number(weather);
        time_info.bloodstorm = fread_number(weather);
        time_info.sect_alliance_issue = fread_number(weather);
        time_info.sect_alliance_type = fread_number(weather);
        time_info.lweek_tier = fread_number(weather);
        time_info.lweek_total = fread_number(weather);
        time_info.tweek_tier = fread_number(weather);
        time_info.tweek_total = fread_number(weather);

        fclose(weather);
        log_string("Here2");
      }
      else
      bugf("[%s:%s] Could not open weather file!", __FILE__, __FUNCTION__);
    }
    log_string("Weather File loaded");


    /*
* Fix up exits.
* Declare db booting over.
* Reset all areas once.
* Load up the songs, notes and ban files.
*/
    {
      fix_exits();
      log_string("Exits Fixed");
      fix_mobprogs();
      log_string("Mobprogs Fixed");
      fix_objprogs();
      log_string("Objprogs Fixed");
      fix_roomprogs();
      log_string("Roomprogs Fixed");
      fBootDb = FALSE;

      area_update(TRUE);
      log_string("Areas updated");

      log_string("Loading Notes...");
      loadNotes();

      log_string("Loading Disabled Commands...");
      load_disabled();

      log_string("Loading Banned Sites...");
      load_bans();

      log_string("Loading Songs...");
      load_songs();

      log_string("Loading Quotes...");
      load_quotes();

      log_string("Loading Socials...");
      load_social_table();

      log_string("Building Maps...");

      buildmaps();

      log_string("Loading ground objects...");
      load_ground_objects();


      log_string("Loading Gateways...");

      load_gateways();

      log_string("Loading Houses...");
      load_houses();

      log_string("Loading Properties...");

      load_properties();

      log_string("Loading Domains...");

      load_domains();

      log_string("Loading Rent...");

      load_rent();
      log_string("Loading Blackmarket...");

      load_blackmarket();



      log_string("Setting Clock...");
      clock_hour = get_hour(NULL);
      clock_minute = get_minute();
      clock_second = (current_time % 60) * PULSE_PER_SECOND;
    }

    return;
  }

  /*
* OLC
* Use these macros to load any new area formats that you choose to
* support on your MUD.  See the load_area format below for
* a short example.
*/
  /*
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
if ( !str_cmp( word, literal ) )    \
{                                   \
field  = value;                 \
fMatch = TRUE;                  \
break;                          \
}

#define SKEY( string, field )                       \
if ( !str_cmp( word, string ) )     \
{                                   \
free_string( field );           \
field = fread_string( fp );     \
fMatch = TRUE;                  \
break;                          \
}

*/

  /* OLC
* Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
* too.
*
* #AREAFILE
* Name   { All } Locke    Newbie School~
* Repop  A teacher pops in the room and says, 'Repop coming!'~
* Recall 3001
* End
*/
  void load_area(FILE *fp) {
    AREA_DATA *pArea;
    const char *word;
    bool fMatch;

    pArea = (AREA_DATA *)alloc_perm(sizeof(*pArea));
    pArea->age = 15;
    pArea->nplayer = 0;
    pArea->file_name = str_dup(strArea);
    pArea->vnum = top_area;
    pArea->name = str_dup("New Area");
    pArea->min_vnum = 0;
    pArea->max_vnum = 0;
    pArea->area_flags = 0;
    pArea->world = 0;
    pArea->minx = 0;
    pArea->miny = 0;
    pArea->maxx = 0;
    pArea->maxy = 0;
    pArea->version = 1;

    /*  pArea->recall       = ROOM_VNUM_TEMPLE;        ROM OLC */

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case 'N':
        SKEY("Name", pArea->name);
        break;

      case 'V':
        if (!str_cmp(word, "VNUMs")) {
          pArea->min_vnum = fread_number(fp);
          pArea->max_vnum = fread_number(fp);

          break;
        }
        KEY("Version", pArea->version, fread_number(fp));
        break;

      case 'R':
        if (!str_cmp(word, "Range")) {
          pArea->minx = fread_number(fp);
          pArea->miny = fread_number(fp);
          pArea->maxx = fread_number(fp);
          pArea->maxy = fread_number(fp);
        }
        break;
      case 'L':
        break;

      case 'E':
        if (!str_cmp(word, "End")) {
          fMatch = TRUE;
          area_list.push_back(pArea);
          area_last = pArea;
          current_area = pArea;
          top_area++;
          return;
        }
        break;

      case 'W':
        KEY("World", pArea->world, fread_number(fp));
        break;
        if (fMatch == FALSE)
        log_string("Load area, no Fmatch");
      }
    }
  }

  /*
* Sets vnum range for area using OLC protection features.
*/
  void assign_area_vnum(int vnum) {
    if (area_last->min_vnum == 0 || area_last->max_vnum == 0)
    area_last->min_vnum = area_last->max_vnum = vnum;
    if (vnum != URANGE(area_last->min_vnum, vnum, area_last->max_vnum)) {
      char temp[MSL];
      sprintf(temp, "T %d %d %d\n\r", vnum, area_last->min_vnum, area_last->max_vnum);
      log_string(temp);

      if (vnum < area_last->min_vnum)
      area_last->min_vnum = vnum;
      else
      area_last->max_vnum = vnum;
    }
    return;
  }

  /*
* Snarf a help section.
*/

  void load_helps(FILE *fp, char *fname) {
    HELP_DATA *pHelp;
    HELP_AREA *had;
    const char *word;
    bool fMatch = FALSE;
    bool filedone = FALSE;

    if (!had_list) {
      had = new_had();
      had->filename = str_dup(fname);
      had->area = current_area;
      if (current_area)
      current_area->helps = had;
      had_list = had;
    }
    else if (str_cmp(fname, had_list->filename)) {
      had = new_had();
      had->filename = str_dup(fname);
      had->area = current_area;
      if (current_area)
      current_area->helps = had;
      had->next = had_list;
      had_list = had;
    }
    else
    had = had_list;

    pHelp = new_help();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);

      switch (UPPER(word[0])) {
      case '#':
        log_string(word);
        filedone = TRUE;
        break;

      case 'E':
        if (!strcmp(word, "End")) {
          if (!str_prefix("greeting", pHelp->keyword)) {
            help_greeting[greeting_count] = pHelp->text;
            greeting_count++;
          }
          if (!str_prefix("infernohelp", pHelp->keyword)) {
            help_inferno = pHelp->text;
          }
          if (!str_prefix("motd", pHelp->keyword)) {
            help_motd = pHelp->text;
          }
          if (!str_prefix("introstory", pHelp->keyword)) {
            help_story = pHelp->text;
          }

          if (help_first == NULL)
          help_first = pHelp;
          if (help_last != NULL)
          help_last->next = pHelp;

          help_last = pHelp;
          pHelp->next = NULL;

          if (!had->first)
          had->first = pHelp;
          if (!had->last)
          had->last = pHelp;

          had->last->next = pHelp;
          had->last = pHelp;
          pHelp->next = NULL;

          top_help++;

          if (!had_list) {
            had = new_had();
            had->filename = str_dup(fname);
            had->area = current_area;
            if (current_area)
            current_area->helps = had;
            had_list = had;
          }
          else if (str_cmp(fname, had_list->filename)) {
            had = new_had();
            had->filename = str_dup(fname);
            had->area = current_area;
            if (current_area)
            current_area->helps = had;
            had->next = had_list;
            had_list = had;
          }
          else
          had = had_list;
          break;
        }
        break;

      case 'G':
        break;

      case 'K':
        if (!strcmp(word, "Keyword")) {
          pHelp = new_help();
          pHelp->keyword = fread_string(fp);
          break;
        }
        break;

      case 'L':
        KEY("Level", pHelp->level, fread_number(fp));
        break;

      case 'O':
        KEY("Online", pHelp->online, fread_number(fp));
        break;

      case 'S':
        KEY("SeeAlso", pHelp->see_also, fread_string(fp));
        break;

      case 'T':
        KEY("Text", pHelp->text, fread_string(fp));
        KEY("Type", pHelp->type, fread_number(fp));
        break;
        if (fMatch == FALSE)
        log_string("Fread helps, no Fmatch");
      }
      if (filedone) {
        log_string("filedone");
        break;
      }
    }
    log_string("return");
    return;
  }

  /*
* Adds a reset to a room.  OLC
* Similar to add_reset in olc.c
*/
  void new_reset(ROOM_INDEX_DATA *pR, RESET_DATA *pReset) {
    RESET_DATA *pr;

    if (!pR)
    return;

    pr = pR->reset_last;

    if (!pr) {
      pR->reset_first = pReset;
      pR->reset_last = pReset;
    }
    else {
      pR->reset_last->next = pReset;
      pR->reset_last = pReset;
      pR->reset_last->next = NULL;
    }

    /*    top_reset++; no estamos asignando memoria!!!! */

    return;
  }

  /*
* Snarf a reset section.
*/
  void load_resets(FILE *fp) {
    RESET_DATA *pReset;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *pRoomIndex;
    int rVnum = -1;

    if (!area_last) {
      bug("Load_resets: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      char letter;

      if ((letter = fread_letter(fp)) == 'S')
      break;

      if (letter == '*') {
        fread_to_eol(fp);
        continue;
      }

      pReset = new_reset_data();
      pReset->command = letter;
      /* if_flag */ fread_number(fp);
      pReset->arg1 = fread_number(fp);
      pReset->arg2 = fread_number(fp);
      pReset->arg3 = (letter == 'G' || letter == 'R') ? 0 : fread_number(fp);
      pReset->arg4 = (letter == 'P' || letter == 'M') ? fread_number(fp) : 0;
      fread_to_eol(fp);

      switch (pReset->command) {
      case 'M':
      case 'Q':
      case 'O':
        rVnum = pReset->arg3;
        break;

      case 'P':
      case 'G':
      case 'E':
        break;

      case 'D':
        pRoomIndex = get_room_index((rVnum = pReset->arg1));
        if (pReset->arg2 < 0 || pReset->arg2 >= MAX_DIR || !pRoomIndex || !(pexit = pRoomIndex->exit[pReset->arg2])) {
          bugf("Load_resets: 'D': exit %d, room %d not door.", pReset->arg2, pReset->arg1);
          exit(1);
        }
        if (pexit->wallcondition == WALLCOND_NORMAL && IS_SET(pexit->rs_flags, EX_ISDOOR)) {
          switch (pReset->arg3) {
          default:
            bug("Load_resets: 'D': bad 'locks': %d.", pReset->arg3);
            break;
          case 0:
            break;
          case 1:
            SET_BIT(pexit->rs_flags, EX_CLOSED);
            SET_BIT(pexit->exit_info, EX_CLOSED);
            break;
          case 2:
            SET_BIT(pexit->rs_flags, EX_CLOSED | EX_LOCKED);
            SET_BIT(pexit->exit_info, EX_CLOSED | EX_LOCKED);
            break;
          }
        }
        else if (number_percent() % 6 == 2 && pexit->wallcondition > 0)
        pexit->wallcondition--;
        break;

      case 'R':
        rVnum = pReset->arg1;
        break;
      }

      if (rVnum == -1) {
        bugf("load_resets : rVnum == -1");
        exit(1);
      }

      if (pReset->command != 'D')
      new_reset(get_room_index(rVnum), pReset);
      else
      free_reset_data(pReset);
    }

    return;
  }

  /*
* Snarf a room section.
*/
  void load_rooms(FILE *fp) {
    ROOM_INDEX_DATA *pRoomIndex;

    if (area_last == NULL) {
      bug("Load_resets: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;
      int door;
      int iHash;

      letter = fread_letter(fp);
      if (letter != '#') {
        bug("Load_rooms: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      fBootDb = FALSE;
      if (get_room_index(vnum) != NULL) {
        //	    bug( "Load_rooms: vnum %d duplicated.", vnum );
        //	    exit( 1 );
      }
      fBootDb = TRUE;

      pRoomIndex = (ROOM_INDEX_DATA *)alloc_perm(sizeof(*pRoomIndex));
      pRoomIndex->visited = FALSE;
      pRoomIndex->owner = str_dup("");
      pRoomIndex->people = new CharList();
      pRoomIndex->contents = NULL;
      pRoomIndex->extra_descr = NULL;
      pRoomIndex->area = area_last;
      pRoomIndex->vnum = vnum;
      pRoomIndex->name = fread_string(fp);
      pRoomIndex->description = fread_string(fp);
      /** Blank Reads For Specific Area */
      /* Area number */
      fread_number(fp);
      pRoomIndex->room_flags = fread_flag(fp);
      pRoomIndex->sector_type = fread_number(fp);
      pRoomIndex->light = 0;
      for (door = 0; door < MAX_DIR; door++)
      pRoomIndex->exit[door] = NULL;

      /* defaults */
      pRoomIndex->level = 0;
      pRoomIndex->mana_rate = 0;

      for (int y = 0; y < 10; y++) {
        pRoomIndex->vehicle_names[y] = str_dup("");
        pRoomIndex->vehicle_lplates[y] = str_dup("");
        pRoomIndex->vehicle_descs[y] = str_dup("");
        pRoomIndex->vehicle_owners[y] = str_dup("");
        pRoomIndex->vehicle_cost[y] = 0;
        pRoomIndex->vehicle_typeone[y] = 0;
        pRoomIndex->vehicle_typetwo[y] = 0;
      }
      for (int y = 0; y < 3; y++) {
        pRoomIndex->smell_desc[y] = str_dup("");
        pRoomIndex->smell_dir[y] = 0;
      }

      for (;;) {
        letter = fread_letter(fp);

        if (letter == 'S' || letter == 'Z')
        break;

        if (letter == 'H') /* healing room */
        pRoomIndex->level = fread_number(fp);

        else if (letter == 'M') /* mana room */
        pRoomIndex->timezone = fread_number(fp);
        else if (letter == 'D') {
          EXIT_DATA *pexit;
          int locks;

          door = fread_number(fp);
          if (door < 0 || door > 9) {
            bug("Fread_rooms: vnum %d has bad door number.", vnum);
            exit(1);
          }

          pexit = (EXIT_DATA *)alloc_perm(sizeof(*pexit));
          pexit->affected = NULL;
          pexit->next_upd = NULL;
          SET_INIT(pexit->affected_by);
          pexit->description = fread_string(fp);
          pexit->keyword = fread_string(fp);
          pexit->exit_info = 0;
          pexit->rs_flags = 0; /* OLC */

          locks = fread_number(fp);
          pexit->key = fread_number(fp);
          pexit->u1.vnum = fread_number(fp);
          pexit->orig_door = door; /* OLC */
          pexit->jump = fread_number(fp);
          pexit->climb = fread_number(fp);
          pexit->fall = fread_number(fp);
          pexit->wall = fread_number(fp);
          pexit->wallcondition = fread_number(fp);
          pexit->doorbroken = fread_number(fp);
          int temp = 0;
          temp += fread_number(fp);
          temp += fread_number(fp);
          temp += fread_number(fp);

          switch (locks) {
          case 1:
            pexit->exit_info = EX_ISDOOR;
            pexit->rs_flags = EX_ISDOOR;
            break;
          case 2:
            pexit->exit_info = EX_ISDOOR | EX_CURTAINS;
            pexit->rs_flags = EX_ISDOOR | EX_CURTAINS;
            break;
          case 3:
            pexit->exit_info = EX_ISDOOR | EX_NOPASS;
            pexit->rs_flags = EX_ISDOOR | EX_NOPASS;
            break;
          case 4:
            pexit->exit_info = EX_ISDOOR | EX_NOPASS | EX_CURTAINS;
            pexit->rs_flags = EX_ISDOOR | EX_NOPASS | EX_CURTAINS;
            break;
          case 5:
            pexit->exit_info = EX_HIDDEN;
            pexit->rs_flags = EX_HIDDEN;
            break;
          case 6:
            pexit->exit_info = EX_ISDOOR | EX_HIDDEN;
            pexit->rs_flags = EX_ISDOOR | EX_HIDDEN;
            break;
          case 7:
            pexit->exit_info = EX_ISDOOR | EX_CURTAINS | EX_HIDDEN;
            pexit->rs_flags = EX_ISDOOR | EX_CURTAINS | EX_HIDDEN;
            break;
          case 8:
            pexit->exit_info = EX_ISDOOR | EX_NOPASS | EX_HIDDEN;
            pexit->rs_flags = EX_ISDOOR | EX_NOPASS | EX_HIDDEN;
            break;
          case 9:
            pexit->exit_info = EX_ISDOOR | EX_NOPASS | EX_CURTAINS | EX_HIDDEN;
            pexit->rs_flags = EX_ISDOOR | EX_NOPASS | EX_CURTAINS | EX_HIDDEN;
            break;
          case 10:
            pexit->exit_info = EX_CURTAINS;
            pexit->rs_flags = EX_CURTAINS;
            break;
          case 11:
            pexit->exit_info = EX_CURTAINS | EX_HIDDEN;
            pexit->rs_flags = EX_CURTAINS | EX_HIDDEN;
            break;
          }

          pRoomIndex->exit[door] = pexit;
          top_exit++;
        }
        else if (letter == 'E') {
          EXTRA_DESCR_DATA *ed;

          ed = (EXTRA_DESCR_DATA *)alloc_perm(sizeof(*ed));
          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = pRoomIndex->extra_descr;
          pRoomIndex->extra_descr = ed;
          top_ed++;
        }
        else if (letter == 'G') {
          pRoomIndex->locx = fread_number(fp);
          pRoomIndex->locy = fread_number(fp);
          pRoomIndex->size = fread_number(fp);
          pRoomIndex->entryx = fread_number(fp);
          pRoomIndex->entryy = fread_number(fp);
        }
        else if (letter == 'A') {
          pRoomIndex->encroachment = fread_number(fp);
          pRoomIndex->security = fread_number(fp);
          pRoomIndex->toughness = fread_number(fp);
          pRoomIndex->decor = fread_number(fp);
        }
        else if (letter == 'C') {
          pRoomIndex->x = fread_number(fp);
          pRoomIndex->y = fread_number(fp);
          pRoomIndex->z = fread_number(fp);
        }
        else if (letter == 'F') {
          FEATURE_TYPE *feature;
          feature = (FEATURE_TYPE *)alloc_perm(sizeof(*feature));
          feature->type = fread_number(fp);
          feature->position = fread_number(fp);
          feature->names = fread_string(fp);
          feature->shortdesc = fread_string(fp);
          feature->desc = fread_string(fp);

          int k;
          for (k = 0; k < 10 && pRoomIndex->features[k] != NULL && pRoomIndex->features[k]->type != 0;
          k++) {
          }
          pRoomIndex->features[k] = feature;
        }
        else if (letter == 'O') {
          if (pRoomIndex->owner[0] != '\0') {
            bug("Load_rooms: duplicate owner.", 0);
            exit(1);
          }

          pRoomIndex->owner = fread_string(fp);
        }
        else if (letter == 'P') {
          EXTRA_DESCR_DATA *ed;

          ed = (EXTRA_DESCR_DATA *)alloc_perm(sizeof(*ed));
          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = pRoomIndex->places;
          pRoomIndex->places = ed;
          top_place++;
        }
        else if (letter == 'R') {
          PROG_LIST *pRprog;
          char *word;
          int trigger = 0;

          pRprog = (PROG_LIST *)alloc_perm(sizeof(*pRprog));
          word = fread_word(fp);
          if (!(trigger = flag_lookup(word, rprog_flags))) {
            bug("ROOMprogs: invalid trigger.", 0);
            exit(1);
          }
          SET_BIT(pRoomIndex->rprog_flags, trigger);
          pRprog->trig_type = trigger;
          pRprog->vnum = fread_number(fp);
          pRprog->trig_phrase = fread_string(fp);
          pRprog->next = pRoomIndex->rprogs;
          pRoomIndex->rprogs = pRprog;
        }
        else if (letter == 'T') {
          pRoomIndex->time = fread_number(fp);
          if (pRoomIndex->time == 0)
          pRoomIndex->time = 3000;
        }
        else if (letter == 'W') {
          pRoomIndex->shroud = fread_string(fp);
        }
        else if (letter == 'X') {
          // pRoomIndex->player_description = fread_string( fp ); //Discordance
        }
        else if (letter == 'Y') {
          pRoomIndex->subarea = fread_string(fp); // Discordance
        }
        else if (letter == 'Z') {
          // pRoomIndex->player_shroud = fread_string( fp ); //Discordance
        }
        else {
          bug("Load_rooms: vnum %d has flag not 'DES'.", vnum);
          exit(1);
        }
      }

      iHash = vnum % MAX_KEY_HASH;
      pRoomIndex->next = room_index_hash[iHash];
      room_index_hash[iHash] = pRoomIndex;
      top_room++;
      top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; /* OLC */
      assign_area_vnum(vnum);                                      /* OLC */
    }

    return;
  }

  /*
* Snarf a shop section.
*/
  void load_shops(FILE *fp) {
    SHOP_DATA *pShop;

    for (;;) {
      MOB_INDEX_DATA *pMobIndex;
      int iTrade;

      pShop = (SHOP_DATA *)alloc_perm(sizeof(*pShop));
      pShop->keeper = fread_number(fp);
      if (pShop->keeper == 0)
      break;

      for (iTrade = 0; iTrade < MAX_TRADE; iTrade++)
      pShop->buy_type[iTrade] = fread_number(fp);
      pShop->profit_buy = fread_number(fp);
      pShop->profit_sell = fread_number(fp);
      pShop->open_hour = fread_number(fp);
      pShop->close_hour = fread_number(fp);
      pShop->owner = fread_number(fp);
      pShop->proceed = fread_number(fp);
      fread_to_eol(fp);
      pMobIndex = get_mob_index(pShop->keeper);
      pMobIndex->pShop = pShop;

      shop_list.push_back(pShop);

      top_shop++;
    }

    return;
  }

  /*
* Snarf spec proc declarations.
*/
  void load_specials(FILE *fp) {
    for (;;) {
      //		MOB_INDEX_DATA *pMobIndex;
      //		OBJ_INDEX_DATA *pObjIndex;
      char letter;

      switch (letter = fread_letter(fp)) {
      default:
        bug("Load_specials: letter '%c' not *MS.", letter);
        exit(1);

      case 'S':
        return;

      case '*':
        break;

      case 'M':
        //			pMobIndex		= get_mob_index (
        //fread_number ( fp ) );
        break;
      case 'O':
        //           pObjIndex           = get_obj_index ( fread_number ( fp ) );
        break;
      }

      fread_to_eol(fp);
    }
  }

  /*
* Translate all room exits from virtual to real.
* Has to be done after all rooms are read in.
* Check for bad reverse exits.
*/
  void fix_exits(void) {
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    RESET_DATA *pReset;
    ROOM_INDEX_DATA *iLastRoom, *iLastObj;
    int iHash;
    int door;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL;
      pRoomIndex = pRoomIndex->next) {
        bool fexit;

        iLastRoom = iLastObj = NULL;

        /* OLC : nuevo chequeo de resets */
        for (pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next) {
          switch (pReset->command) {
          default:
            bugf("fix_exits : cuarto %d con reset cmd %c", pRoomIndex->vnum, pReset->command);
            exit(1);
            break;

          case 'M':
            get_mob_index(pReset->arg1);
            iLastRoom = get_room_index(pReset->arg3);
            break;

          case 'Q':
            get_group_index(pReset->arg1);
            break;

          case 'O':
            get_obj_index(pReset->arg1);
            iLastObj = get_room_index(pReset->arg3);
            break;

          case 'P':
            get_obj_index(pReset->arg1);
            if (iLastObj == NULL) {
              bugf("fix_exits : reset en cuarto %d con iLastObj NULL", pRoomIndex->vnum);
              exit(1);
            }
            break;

          case 'G':
          case 'E':
            get_obj_index(pReset->arg1);
            if (iLastRoom == NULL) {
              bugf("fix_exits : reset en cuarto %d con iLastRoom NULL", pRoomIndex->vnum);
              break;
            }
            iLastObj = iLastRoom;
            break;

          case 'D':
            bugf("???");
            break;

          case 'R':
            get_room_index(pReset->arg1);
            if (pReset->arg2 < 0 || pReset->arg2 > MAX_DIR) {
              bugf("fix_exits : reset en cuarto %d con arg2 %d >= MAX_DIR", pRoomIndex->vnum, pReset->arg2);
              exit(1);
            }
            break;
          } /* switch */
        }   /* for */

        fexit = FALSE;
        for (door = 0; door <= 9; door++) {
          if ((pexit = pRoomIndex->exit[door]) != NULL) {
            if (pexit->u1.vnum <= 0 || get_room_index(pexit->u1.vnum) == NULL) {
              pexit->u1.to_room = NULL;
            }
            else {
              fexit = TRUE;
              pexit->u1.to_room = get_room_index(pexit->u1.vnum);
            }
          }
        }
        if (!fexit)
        SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
      }
    }

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL;
      pRoomIndex = pRoomIndex->next) {
        for (door = 0; door <= 9; door++) {
          if ((pexit = pRoomIndex->exit[door]) != NULL && (to_room = pexit->u1.to_room) != NULL && (pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299)) {
            /*		    char buf[MSL];
sprintf( buf, "Fix_exits: %d:%d ->%d ->.", pRoomIndex->vnum, door, to_room->vnum);
bug( buf, 0 );*/
          }
        }
      }
    }

    return;
  }

  void load_groups(FILE *fp) {
    GROUP_INDEX_DATA *pGroupIndex;

    if (area_last == NULL) {
      bug("Load_groups: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;

      letter = fread_letter(fp);

      if (letter != '#') {
        bug("Load_groups: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      fBootDb = FALSE;
      if (get_group_index(vnum) != NULL) {
        bug("Load_groups: vnum %d duplicated.", vnum);
        exit(1);
      }
      fBootDb = TRUE;

      pGroupIndex = (GROUP_INDEX_DATA *)alloc_perm(sizeof(*pGroupIndex));
      pGroupIndex->vnum = vnum;

      for (int x = 0; x < 6; x++) {
        pGroupIndex->member_vnum[x] = fread_number(fp);
        pGroupIndex->member_position[x] = fread_number(fp);
      }

      if (group_list == NULL)
      group_list = pGroupIndex;
      else {
        pGroupIndex->next = group_list;
        group_list = pGroupIndex;
      }
      top_group_index++;
    }
    return;
  }

  /*
* Load mobprogs section
*/
  void load_mobprogs(FILE *fp) {
    PROG_CODE *pMprog;

    if (area_last == NULL) {
      bug("Load_mobprogs: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;

      letter = fread_letter(fp);
      if (letter != '#') {
        bug("Load_mobprogs: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      fBootDb = FALSE;
      if (get_prog_index(vnum, PRG_MPROG) != NULL) {
        bug("Load_mobprogs: vnum %d duplicated.", vnum);
        exit(1);
      }
      fBootDb = TRUE;

      pMprog = (PROG_CODE *)alloc_perm(sizeof(*pMprog));
      pMprog->vnum = vnum;
      pMprog->code = fread_string(fp);
      mprog_list.push_front(pMprog);
      top_mprog_index++;
    }
    return;
  }

  /*
*  Translate mobprog vnums pointers to real code
*/
  void fix_mobprogs(void) {
    MOB_INDEX_DATA *pMobIndex;
    PROG_LIST *list;
    PROG_CODE *prog;
    int iHash;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL;
      pMobIndex = pMobIndex->next) {
        for (list = pMobIndex->mprogs; list != NULL; list = list->next) {
          if ((prog = get_prog_index(list->vnum, PRG_MPROG)) != NULL)
          list->code = prog->code;
          else {
            bug("Fix_mobprogs: code vnum %d not found.", list->vnum);
            exit(1);
          }
        }
      }
    }
  }

  /*
* Repopulate areas periodically.
*/
  void area_update(bool runtime) {
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];

    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;

      if (++pArea->age < 3)
      continue;

      /*
* Check age and reset.
* Note: Mud School resets every 3 minutes (not 15).
*/
      if ((!pArea->empty && (pArea->nplayer == 0 || pArea->age >= 15)) || pArea->age >= 31) {
        ROOM_INDEX_DATA *pRoomIndex;

        reset_area(pArea, runtime);
        log_string("Area reset");
        sprintf(buf, "%s has just been reset.", pArea->name);
        wiznet(buf, NULL, NULL, WIZ_RESETS, 0, 0);

        pArea->age = number_range(0, 3);
        pRoomIndex = get_room_index(ROOM_VNUM_LIMBO);
        if (pRoomIndex != NULL && pArea == pRoomIndex->area)
        pArea->age = 15 - 2;
        else if (pArea->nplayer == 0)
        pArea->empty = TRUE;
      }
    }

    return;
  }

  /* OLC
* Reset one room.  Called by reset_area and olc.
*/
  void reset_room(ROOM_INDEX_DATA *pRoom, bool runtime) {
    RESET_DATA *pReset;
    CHAR_DATA *pMob;
    OBJ_DATA *pObj;
    CHAR_DATA *LastMob = NULL;
    OBJ_DATA *LastObj = NULL;
    int iExit;
    int level = 0;
    bool last;

    if (!pRoom)
    return;

    pMob = NULL;
    last = FALSE;

    for (iExit = 0; iExit < MAX_DIR; iExit++) {
      EXIT_DATA *pExit;
      if ((pExit = pRoom->exit[iExit]) && pExit->doorbroken <= 0) {
        pExit->exit_info = pExit->rs_flags;
        if ((pExit->u1.to_room != NULL)) {
          /* nail the other side */
          pExit->exit_info = pExit->rs_flags;
        }
      }
    }

    for (pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next) {
      //	CHAR_DATA *leader = NULL;
      //	MOB_INDEX_DATA *leader_temp = NULL;
      MOB_INDEX_DATA *pMobIndex;
      GROUP_INDEX_DATA *pGroupIndex;
      OBJ_INDEX_DATA *pObjIndex;
      OBJ_INDEX_DATA *pObjToIndex;
      ROOM_INDEX_DATA *pRoomIndex;
      char buf[MAX_STRING_LENGTH];
      int count = 0, limit = 0;
      int q = 0;
      bool member_found = FALSE;

      switch (pReset->command) {
      default:
        bug("Reset_room: bad command %c.", pReset->command);
        break;

      case 'Q':
        if (!(pGroupIndex = get_group_index(pReset->arg1))) {
          bug("Reset_group: 'Q': bad vnum %d.", pReset->arg1);
          continue;
        }

        if ((pRoomIndex = get_room_index(pReset->arg3)) == NULL) {
          bug("Reset_area: 'R': bad vnum %d.", pReset->arg3);
          continue;
        }

        /* Now what we have to do is scan through the rooms character list;) */
        // leader_temp = get_mob_index(pGroupIndex->member_vnum[0]);
        // leader = get_char_room( NULL, pRoomIndex, leader_temp->player_name );

        for (CharList::iterator it = pRoomIndex->people->begin();
        it != pRoomIndex->people->end(); ++it) {
          CHAR_DATA *rch = *it;

          if (!IS_NPC(rch))
          continue;

          for (q = 0; q < 6; q++) {
            if (pGroupIndex->member_vnum[q] == rch->pIndexData->vnum) {
              member_found = TRUE;
              break;
            }
          }

          if (!member_found)
          continue;
        }
        break;

      case 'M':
        if (!(pMobIndex = get_mob_index(pReset->arg1))) {
          char buf[MSL];
          sprintf(buf, "Reset_room: 'M': Bad vnum %d in %d.", pReset->arg1, pRoom->vnum);
          bug(buf, 1);
          continue;
        }

        if ((pRoomIndex = get_room_index(pReset->arg3)) == NULL) {
          bug("Reset_area: 'R': bad vnum %d.", pReset->arg3);
          continue;
        }

        if (pMobIndex->count >= pReset->arg2) {
          last = FALSE;
          break;
        }

        /* */
        count = 0;
        for (CharList::iterator it = pRoomIndex->people->begin();
        it != pRoomIndex->people->end(); ++it) {
          if ((*it)->pIndexData == pMobIndex) {
            count++;
            if (count >= pReset->arg4) {
              last = FALSE;
              break;
            }
          }
        }

        if (count >= pReset->arg4)
        break;

        /* */
        pMob = create_mobile(pMobIndex);

        /*
* Some more hard coding.
*/

        /*
* Pet shop mobiles get ACT_PET set.
*/

        char_to_room(pMob, pRoom);
        if (IS_FLAG(pMob->act, ACT_ARCHER)) {
          for (int a = 0; a < 10; a++) {
            if (pMob->pIndexData->targeting[a] == NULL)
            continue;

            pMob->next_observing_room = pMob->pIndexData->targeting[a]->observing;
            pMob->pIndexData->targeting[a]->observing = pMob;
          }
        }

        LastMob = pMob;
        level = URANGE(0, pMob->level - 2, LEVEL_HERO - 1); /* -1 ROM */
        last = TRUE;
        break;

      case 'O':
        if (!(pObjIndex = get_obj_index(pReset->arg1))) {
          bug("Reset_room: 'O' 1 : bad vnum %d", pReset->arg1);
          sprintf(buf, "%d %d %d %d", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4);
          bug(buf, 1);
          continue;
        }

        if (!(pRoomIndex = get_room_index(pReset->arg3))) {
          bug("Reset_room: 'O' 2 : bad vnum %d.", pReset->arg3);
          sprintf(buf, "%d %d %d %d", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4);
          bug(buf, 1);
          continue;
        }

        if (count_obj_list(pObjIndex, pRoom->contents) > 0) {
          last = FALSE;
          break;
        }

        if (number_percent() < pObjIndex->load_chance) {
          pObj =
          create_object(pObjIndex, UMIN(number_fuzzy(level), LEVEL_HERO - 1));
          pObj->cost = 0;
          obj_to_room(pObj, pRoom);
        }
        last = TRUE;
        break;

      case 'P':
        if (!(pObjIndex = get_obj_index(pReset->arg1))) {
          bug("Reset_room: 'P': bad vnum %d.", pReset->arg1);
          continue;
        }

        if (!(pObjToIndex = get_obj_index(pReset->arg3))) {
          bug("Reset_room: 'P': bad vnum %d.", pReset->arg3);
          continue;
        }

        if (pReset->arg2 > 50)
        limit = 6;
        else if (pReset->arg2 == -1)
        limit = 999;
        else
        limit = pReset->arg2;

        if ((LastObj = get_obj_type(pObjToIndex)) == NULL
            //              || ( LastObj->in_room == NULL && !last)
            //      || ( pObjIndex->count >= limit )
            || ((number_percent() > pObjIndex->load_chance)) || (count = count_obj_list(pObjIndex, LastObj->contains)) >
            pReset->arg4) {
          last = FALSE;
          break;
        }

        /* lastObj->level - ROM */
        while (count < pReset->arg4) {
          pObj = create_object(pObjIndex, number_fuzzy(LastObj->level));
          obj_to_obj(pObj, LastObj);

          count++;
          if (pObjIndex->count >= limit)
          break;
        }

        /* fix object lock state! */
        LastObj->value[1] = LastObj->pIndexData->value[1];
        last = TRUE;
        break;

      case 'G':
      case 'E':
        if (!(pObjIndex = get_obj_index(pReset->arg1))) {
          bug("Reset_room: 'E' or 'G': bad vnum %d.", /*pReset->arg1,*/ pRoom->vnum);
          continue;
        }

        if (!last)
        break;

        if (!LastMob) {
          bug("Reset_room: 'E' or 'G': null mob for vnum %d.", pReset->arg1);
          last = FALSE;
          break;
        }

        if (LastMob->pIndexData->pShop) /* Shop-keeper? */
        {
          int olevel = 0, i, j;

          switch (pObjIndex->item_type) {
          default:
            olevel = 0;
            break;
          case ITEM_POTION:
            olevel = 53;
            for (i = 1; i < 5; i++) {
              if (pObjIndex->value[i] > 0) {
                for (j = 0; j < MAX_CLASS; j++) {
                  olevel = olevel;
                }
              }
            }

            olevel = UMAX(0, (olevel * 3 / 4) - 2);
            break;

          case ITEM_KEYRING:
            olevel = number_range(10, 20);
            break;
          case ITEM_ARMOR:
            olevel = number_range(5, 15);
            break;
          case ITEM_WEAPON:
            olevel = number_range(5, 15);
            break;

            break;
          }

          pObj = create_object(pObjIndex, olevel);
          SET_BIT(pObj->extra_flags, ITEM_INVENTORY); /* ROM OLC */
        }
        else                                        /* ROM OLC else version */
        {
          int limit;
          if (pReset->arg2 > 50) /* old format */
          limit = 6;
          else if (pReset->arg2 == -1 || pReset->arg2 == 0) /* no limit */
          limit = 999;
          else
          limit = pReset->arg2;

          if ((pObjIndex->count < limit || number_range(0, 4) == 0) && (pObjIndex->load_chance > number_percent())) {
            pObj = create_object(pObjIndex, UMIN(number_fuzzy(level), LEVEL_HERO - 1));
            /* error message if it is too high */
            if (pObj->level > LastMob->level + 10) {
              char buf[MSL];
              fprintf(stderr, "Err: obj %s (%d) -- %d, mob %s (%d) -- %d\n", remove_color(buf, pObj->short_descr), pObj->pIndexData->vnum, pObj->level, remove_color(buf, LastMob->short_descr), LastMob->pIndexData->vnum, LastMob->level);
            }
          }
          else
          break;
        }

        obj_to_char(pObj, LastMob);
        if (pReset->command == 'E')
        equip_char(LastMob, pObj, pReset->arg3);
        last = TRUE;
        break;

      case 'D':
        break;

      case 'R':
        if (!(pRoomIndex = get_room_index(pReset->arg1))) {
          bug("Reset_room: 'R': bad vnum %d.", pReset->arg1);
          continue;
        }

        {
          EXIT_DATA *pExit;
          int d0;
          int d1;

          for (d0 = 0; d0 < pReset->arg2 - 1; d0++) {
            d1 = number_range(d0, pReset->arg2 - 1);
            pExit = pRoomIndex->exit[d0];
            pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
            pRoomIndex->exit[d1] = pExit;
          }
        }
        break;
      }
    }

    return;
  }

  /* OLC
* Reset one area.
*/
  void reset_area(AREA_DATA *pArea, bool runtime) {
    int iHash;

    ROOM_INDEX_DATA *pRoomIndex;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == pArea)
        reset_room(pRoomIndex, runtime);
      }
    }

    /*
for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
{
if ( ( pRoom = get_room_index(vnum) ) )
reset_room(pRoom, runtime);
}
*/
    return;
  }

  /*
* Create an instance of a mobile.
*/

  char *mob_move(int vnum) {
    switch (vnum) {
    case 10:
      return "strolls";
      break;
    case 12:
    case 52:
    case 53:
      return "wanders";
      break;
    case 13:
      return "stumbles";
      break;
    case 14:
    case 15:
    case 18:
    case 56:
    case 57:
      return "shuffles";
      break;
    case 20:
    case 30:
    case 36:
    case 60:
    case 61:
    case 73:
    case 139:
    case 154:
    case 155:
    case 104:
      return "strides";
      break;
    case 23:
    case 24:
      return "jogs";
      break;
    case 25:
    case 26:
    case 31:
    case 32:
    case 100:
    case 107:
    case 137:
    case 138:
    case 102:
      return "patrols";
      break;
    case 29:
    case 64:
    case 65:
    case 74:
    case 70:
    case 105:
    case 101:
    case 108:
    case 109:
    case 110:
    case 142:
    case 143:
    case 144:
      return "marches";
      break;
    case 33:
    case 34:
      return "floats";
      break;
    case 35:
      return "creeps";
      break;
    case 50:
    case 51:
    case 72:
    case 76:
    case 141:
      return "skulks";
      break;
    case 62:
    case 63:
    case 152:
      return "stalks";
      break;
    case 67:
    case 166:
      return "lumbers";
      break;
    case 69:
      return "scurries";
      break;
    case 71:
      return "slinks";
      break;
    case 75:
    case 79:
    case 149:
    case 150:
      return "swaggers";
      break;
    case 99:
      return "swarms";
      break;
    case 103:
      return "prowls";
      break;
    case 130:
    case 140:
    case 156:
      return "sneaks";
      break;
    case 147:
    case 157:
      return "wanders";
      break;
    case 167:
      return "pads";
      break;
    }
    return "";
  }

  CHAR_DATA *create_mobile(MOB_INDEX_DATA *pMobIndex) {
    CHAR_DATA *mob;
    //    AFFECT_DATA af;

    mobile_count++;

    if (pMobIndex == NULL) {
      bug("Create_mobile: NULL pMobIndex.", 0);
      exit(1);
    }

    mob = new_char();

    mob->pIndexData = pMobIndex;

    mob->pcdata = NULL;
    mob->name = str_dup(pMobIndex->player_name);        /* OLC */
    mob->short_descr = str_dup(pMobIndex->short_descr); /* OLC */
    mob->long_descr = str_dup(pMobIndex->long_descr);   /* OLC */
    mob->description = str_dup(pMobIndex->description); /* OLC */
    mob->id = get_mob_id();
    mob->prompt = NULL;

    if (pMobIndex->wealth == 0) {
      mob->money = 0;
    }
    else
    mob_wealth(mob);

    /* read from prototype */
    SET_COPY(mob->act, pMobIndex->act);
    SET_COPY(mob->affected_by, pMobIndex->affected_by);
    //    bitvector_to_array(mob->affected_by, pMobIndex->affected_by);
    mob->level = pMobIndex->level;

    mob->ttl = pMobIndex->ttl;

    for (int b = 0; b < MAX_DIS; b++)
    mob->disciplines[b] = pMobIndex->disciplines[b];

    /*
* This will produce a random value for a mobiles level, somewhere
* between level - fuzzy and level + fuzzy.
*    Palin - October 29, 2001
*/
    mob->hit = max_hp(mob);

    mob->start_pos = pMobIndex->start_pos;
    mob->default_pos = pMobIndex->default_pos;
    mob->sex = pMobIndex->sex;

    if (mob->sex == 3) /* random sex */
    mob->sex = number_range(1, 2);

    mob->race = pMobIndex->race;
    mob->form = pMobIndex->form;

    /*

af.where 	 	= TO_AFFECTS;
af.level 	 	= mob->level;
af.duration  	= -1;
af.caster 	 	= NULL;
af.weave     	= TRUE;
*/
    /* let's get some spell action */

    mob->position = mob->start_pos;

    /* link the mob to the world list */
    char_list.push_front(mob);
    pMobIndex->count++;
    return mob;
  }

  /* duplicate a mobile exactly -- except inventory */
  void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone) {
    AFFECT_DATA *paf;

    if (parent == NULL || clone == NULL || !IS_NPC(parent))
    return;

    /* start fixing values */
    clone->name = str_dup(parent->name);
    clone->version = parent->version;
    clone->short_descr = str_dup(parent->short_descr);
    clone->long_descr = str_dup(parent->long_descr);
    clone->description = str_dup(parent->description);
    clone->sex = parent->sex;
    clone->race = parent->race;
    clone->level = parent->level;
    clone->trust = 0;
    clone->timer = parent->timer;
    clone->wait = parent->wait;
    clone->hit = parent->hit;
    clone->money = parent->money;
    SET_COPY(clone->act, parent->act);
    SET_COPY(clone->comm, parent->comm);
    clone->invis_level = parent->invis_level;
    SET_COPY(clone->affected_by, parent->affected_by);
    clone->form = parent->form;
    clone->start_pos = parent->start_pos;
    clone->default_pos = parent->default_pos;

    /* now add the affects */
    for (paf = parent->affected; paf != NULL; paf = paf->next)
    affect_to_char(clone, paf);
  }

  /*
* Create an instance of an object.
*/
  OBJ_DATA *create_object(OBJ_INDEX_DATA *pObjIndex, int level) {
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    int objlevel = 0;

    if (pObjIndex == NULL) {
      bug("Create_object: NULL pObjIndex.", 0);
      return NULL;
    }

    obj = new_obj();

    obj->pIndexData = pObjIndex;
    obj->in_room = NULL;
    obj->number_enchant = 0;
    obj->level = pObjIndex->level;

    /*
* This will produce a random level on the object as well as scatter
* the stats for the object if necessary.  Only works with Armor.
* Implemented by Palin on October 29th, 2001.
*/
    if (pObjIndex->fuzzy > 0)
    obj->level = number_range((obj->level - pObjIndex->fuzzy), (obj->level + pObjIndex->fuzzy));

    if (pObjIndex->level == 0)
    objlevel = 1;
    else
    objlevel = obj->level / pObjIndex->level;

    obj->wear_loc = -1;

    obj->owned_by = NULL;
    obj->owned_time = 0;
    obj->stash_room = 0;

    obj->name = str_dup(pObjIndex->name);               /* OLC */
    obj->short_descr = str_dup(pObjIndex->short_descr); /* OLC */
    obj->description = str_dup(pObjIndex->description); /* OLC */
    if (pObjIndex->material != NULL)
    obj->material = str_dup(pObjIndex->material);
    else
    obj->material = str_dup(" ");
    obj->wear_string = str_dup(pObjIndex->wear_string);
    obj->item_type = pObjIndex->item_type;
    obj->extra_flags = pObjIndex->extra_flags;
    obj->wear_flags = pObjIndex->wear_flags;
    obj->rot_timer = pObjIndex->rot_timer;
    obj->buff = pObjIndex->buff;
    obj->faction = pObjIndex->faction;
    obj->value[0] = pObjIndex->value[0] * objlevel;
    obj->value[1] = pObjIndex->value[1] * objlevel;
    obj->value[2] = pObjIndex->value[2] * objlevel;
    obj->value[3] = pObjIndex->value[3] * objlevel;
    obj->value[4] = pObjIndex->value[4];
    obj->value[5] = pObjIndex->value[5];

    obj->size = pObjIndex->size;
    obj->cost = pObjIndex->cost;
    obj->condition = pObjIndex->condition;

    /*
* Mess with object properties.
*/

    switch (obj->item_type) {
    default:
      bug("Read_object: vnum %d bad type.", pObjIndex->vnum);
      break;

    case ITEM_LIGHT:
      if (obj->value[2] == 999)
      obj->value[2] = -1;
      break;

    case ITEM_GAME:
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_UMBRELLA:
    case ITEM_CLOTHING:
    case ITEM_KEYRING:
    case ITEM_PERFUME:
    case ITEM_ROOM_KEY:
    case ITEM_JEWELRY:
    case ITEM_WEAPON:
    case ITEM_ARMOR:
    case ITEM_POTION:
    case ITEM_MONEY:
    case ITEM_COMPONENT:
    case ITEM_STAKE:
    case ITEM_LIGHTER:
    case ITEM_BLOODCRYSTAL:
    case ITEM_ARTIFACT:
    case ITEM_RANGED:
    case ITEM_JUKEBOX:
    case ITEM_FLASHLIGHT:
    case ITEM_PHONE:
    case ITEM_DRUGS:
    case ITEM_BABY:
      break;
    }

    for (paf = pObjIndex->affected; paf != NULL; paf = paf->next)
    if (paf->location == APPLY_SPELL_AFFECT)
    affect_to_obj(obj, paf);

    object_list.push_front(obj);
    pObjIndex->count++;

    return obj;
  }

  /* duplicate an object exactly -- except contents */
  // Updated this.  Looked old and some clothing duplicated was being removed -
  // Discordance
  void clone_object(OBJ_DATA *parent, OBJ_DATA *clone) {
    int i;
    AFFECT_DATA *paf;
    EXTRA_DESCR_DATA *ed, *ed_new;

    if (parent == NULL || clone == NULL)
    return;

    /* start fixing the object */
    clone->name = str_dup(parent->name);
    clone->short_descr = str_dup(parent->short_descr);
    clone->description = str_dup(parent->description);
    /*
clone->extra_descr->keyword         =
str_dup(parent->extra_descr->keyword); clone->extra_descr->description =
str_dup(parent->extra_descr->description);
*/
    clone->item_type = parent->item_type;
    clone->extra_flags = parent->extra_flags;
    clone->wear_flags = parent->wear_flags;
    clone->wear_string = str_dup(parent->wear_string);
    clone->layer = parent->layer;
    clone->exposed = parent->exposed;
    //    clone->adjust_string                = str_dup(parent->adjust_string);
    clone->size = parent->size;
    clone->cost = parent->cost;
    clone->level = parent->level;
    clone->condition = parent->condition;
    clone->material = str_dup(parent->material);
    clone->timer = parent->timer;
    clone->rot_timer = parent->rot_timer;
    clone->buff = parent->buff;
    clone->faction = parent->faction;

    for (i = 0; i < 5; i++)
    clone->value[i] = parent->value[i];

    /* affects */
    clone->number_enchant = parent->number_enchant;

    for (paf = parent->affected; paf != NULL; paf = paf->next)
    affect_to_obj(clone, paf);

    /* extended desc */
    for (ed = parent->extra_descr; ed != NULL; ed = ed->next) {
      ed_new = new_extra_descr();
      ed_new->keyword = str_dup(ed->keyword);
      ed_new->description = str_dup(ed->description);
      ed_new->next = clone->extra_descr;
      clone->extra_descr = ed_new;
    }
  }

  /*
* Clear a new character.
*/
  void clear_char(CHAR_DATA *ch) {
    static CHAR_DATA ch_zero;

    *ch = ch_zero;
    ch->name = &str_empty[0];
    ch->short_descr = &str_empty[0];
    ch->long_descr = &str_empty[0];
    ch->description = &str_empty[0];
    ch->prompt = &str_empty[0];
    ch->logon = current_time;
    ch->lines = PAGELEN;
    ch->position = POS_STANDING;
    ch->hit = 200;
    ch->on = NULL;
    ch->money = 5;

    ch->pcdata->creation_location = CR_ALL;

    ch->pcdata->spec_trust = FALSE;

    ch->pcdata->history = &str_empty[0];

    return;
  }

  /*
* Get an extra description from a list.
*/
  char *get_extra_descr(const char *name, EXTRA_DESCR_DATA *ed) {
    for (; ed != NULL; ed = ed->next) {
      if (is_name((char *)name, ed->keyword) || !str_cmp(ed->keyword, "all"))
      return ed->description;
    }
    return NULL;
  }

  char *get_extra_descr_obj(const char *name, EXTRA_DESCR_DATA *ed, OBJ_DATA *obj) {
    for (; ed != NULL; ed = ed->next) {
      if (is_name((char *)name, ed->keyword) || (!str_cmp(ed->keyword, "all") && is_name((char *)name, obj->name)))
      return ed->description;
    }
    return NULL;
  }

  /*
* Translates mob virtual number to its mob index struct.
* Hash table lookup.
*/
  MOB_INDEX_DATA *get_mob_index(int vnum) {
    MOB_INDEX_DATA *pMobIndex;

    if (vnum < 0) {
      bug("Get_mob_index: bad vnum %d..", vnum);
      exit(1);
    }

    for (pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex != NULL;
    pMobIndex = pMobIndex->next) {
      if (pMobIndex->vnum == vnum)
      return pMobIndex;
    }
    /*
if ( fBootDb )
{
bug( "Get_mob_index: bad vnum %d.", vnum );
exit( 1 );
}
*/
    return NULL;
  }

  GROUP_INDEX_DATA *get_group_index(int vnum) {
    GROUP_INDEX_DATA *pGroupIndex;

    for (pGroupIndex = group_list; pGroupIndex != NULL;
    pGroupIndex = pGroupIndex->next) {
      if (pGroupIndex->vnum == vnum)
      return pGroupIndex;
    }

    if (fBootDb) {
      bug("Get_group_index: bad vnum %d.", vnum);
      exit(1);
    }

    return NULL;
  }

  /*
* Translates mob virtual number to its obj index struct.
* Hash table lookup.
*/
  OBJ_INDEX_DATA *get_obj_index(int vnum) {
    OBJ_INDEX_DATA *pObjIndex;

    for (pObjIndex = obj_index_hash[vnum % MAX_KEY_HASH]; pObjIndex != NULL;
    pObjIndex = pObjIndex->next) {
      if (pObjIndex->vnum == vnum)
      return pObjIndex;
    }

    /*
if ( fBootDb )
{
bug( "Get_obj_index: bad vnum %d.", vnum );
exit( 1 );
}
*/
    return NULL;
  }

  /*
* Translates mob virtual number to its room index struct.
* Hash table lookup.
*/
  ROOM_INDEX_DATA *get_room_index(int vnum) {
    ROOM_INDEX_DATA *pRoomIndex;

    for (pRoomIndex = room_index_hash[vnum % MAX_KEY_HASH]; pRoomIndex != NULL;
    pRoomIndex = pRoomIndex->next) {
      if (pRoomIndex->vnum == vnum)
      return pRoomIndex;
    }

    if (fBootDb) {
      bug("Get_room_index: bad vnum %d...", vnum);
      //		exit( 1 );
    }

    return NULL;
  }

  PROG_CODE *get_prog_index(int vnum, int type) {
    ProgList prg;

    switch (type) {
    case PRG_MPROG:
      prg = mprog_list;
      break;
    case PRG_OPROG:
      prg = oprog_list;
      break;
    case PRG_RPROG:
      prg = rprog_list;
      break;
    default:
      return NULL;
    }

    for (ProgList::iterator it = prg.begin(); it != prg.end(); ++it) {
      if ((*it)->vnum == vnum)
      return *it;
    }
    return NULL;
  }

  /*
* Read a letter from a file.
*/
  char fread_letter(FILE *fp) {
    char c;

    do {
      c = getc(fp);
    } while (isspace(c));

    return c;
  }

  /*
* Read a number from a file.
*/
  int fread_number(FILE *fp) {
    int number;
    bool sign;
    char c;

    do {
      c = getc(fp);
    } while (isspace(c));

    number = 0;

    sign = FALSE;
    if (c == '+') {
      c = getc(fp);
    }
    else if (c == '-') {
      sign = TRUE;
      c = getc(fp);
    }

    if (!isdigit(c)) {
      char buf[MSL];
      sprintf(buf, "Fread_number: bad format: %c", c);
      bug(buf, 0);
      exit(1);
    }

    while (isdigit(c)) {
      number = number * 10 + c - '0';
      c = getc(fp);
    }

    if (sign)
    number = 0 - number;

    if (c == '|')
    number += fread_number(fp);
    else if (c != ' ')
    ungetc(c, fp);

    return number;
  }

  long fread_flag(FILE *fp) {
    int number;
    char c;
    bool negative = FALSE;

    do {
      c = getc(fp);
    } while (isspace(c));

    if (c == '-') {
      negative = TRUE;
      c = getc(fp);
    }

    number = 0;

    if (!isdigit(c)) {
      while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
        number += flag_convert(c);
        c = getc(fp);
      }
    }

    while (isdigit(c)) {
      number = number * 10 + c - '0';
      c = getc(fp);
    }

    if (c == '|')
    number += fread_flag(fp);

    else if (c != ' ')
    ungetc(c, fp);

    if (negative)
    return -1 * number;

    return number;
  }

  long flag_convert(char letter) {
    long bitsum = 0;
    char i;

    if ('A' <= letter && letter <= 'Z') {
      bitsum = 1;
      for (i = letter; i > 'A'; i--)
      bitsum *= 2;
    }
    else if ('a' <= letter && letter <= 'z') {
      bitsum = 67108864; /* 2^26 */
      for (i = letter; i > 'a'; i--)
      bitsum *= 2;
    }

    return bitsum;
  }

  /*
* Read and allocate space for a string from a file.
* These strings are read-only and shared.
* Strings are hashed:
*   each string prepended with hash pointer to prev string, *   hash code is simply the string length.
*   this function takes 40% to 50% of boot-up time.
*/
  char *fread_string(FILE *fp) {
    char *plast;
    char c;

    plast = top_string + sizeof(char *);
    if (plast > &string_space[strspace_size * 1024 - MAX_STRING_LENGTH])
    strspace_size_increment();

    /*
* Skip blanks.
* Read first char.
*/
    do {
      c = getc(fp);
    } while (isspace(c));

    if ((*plast++ = c) == '~')
    return &str_empty[0];

    for (;;) {
      /*
* Back off the char type lookup, *   it was too dirty for portability.
*   -- Furey
*/

      switch (*plast = getc(fp)) {
      default:
        plast++;
        break;

      case EOF:
        /* temp fix */
        bug("Fread_string: EOF", 0);
        return NULL;
        /* exit( 1 ); */
        break;

      case '\n':
        plast++;
        *plast++ = '\r';
        break;

      case '\r':
        break;

      case '~':
        plast++;
        {
          union {
            char *pc;
            char rgc[sizeof(char *)];
          } u1;
          unsigned int ic;
          int iHash;
          char *pHash;
          char *pHashPrev;
          char *pString;

          plast[-1] = '\0';
          iHash = UMIN(MAX_KEY_HASH - 1, plast - 1 - top_string);
          for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev) {
            for (ic = 0; ic < sizeof(char *); ic++)
            u1.rgc[ic] = pHash[ic];
            pHashPrev = u1.pc;
            pHash += sizeof(char *);

            if (top_string[sizeof(char *)] == pHash[0] && !strcmp(top_string + sizeof(char *) + 1, pHash + 1))
            return pHash;
          }

          if (fBootDb) {
            pString = top_string;
            top_string = plast;
            u1.pc = string_hash[iHash];
            for (ic = 0; ic < sizeof(char *); ic++)
            pString[ic] = u1.rgc[ic];
            string_hash[iHash] = pString;

            nAllocString += 1;
            sAllocString += top_string - pString;
            return pString + sizeof(char *);
          }
          else {
            return str_dup(top_string + sizeof(char *));
          }
        }
      }
    }
  }

  char *fread_string_eol(FILE *fp) {
    static bool char_special[256 - EOF];
    char *plast;
    char c;

    if (char_special[EOF - EOF] != TRUE) {
      char_special[EOF - EOF] = TRUE;
      char_special['\n' - EOF] = TRUE;
      char_special['\r' - EOF] = TRUE;
    }

    plast = top_string + sizeof(char *);

    if (plast > &string_space[strspace_size * 1024 - MAX_STRING_LENGTH])
    strspace_size_increment();

    /*
* Skip blanks.
* Read first char.
*/
    do {
      c = getc(fp);
    } while (isspace(c));

    if ((*plast++ = c) == '\n')
    return &str_empty[0];

    for (;;) {
      if (!char_special[(*plast++ = getc(fp)) - EOF])
      continue;

      switch (plast[-1]) {
      default:
        break;

      case EOF:
        bug("Fread_string_eol  EOF", 0);
        exit(1);
        break;

      case '\n':
      case '\r': {
          union {
            char *pc;
            char rgc[sizeof(char *)];
          } u1;
          unsigned int ic;
          int iHash;
          char *pHash;
          char *pHashPrev;
          char *pString;

          plast[-1] = '\0';
          iHash = UMIN(MAX_KEY_HASH - 1, plast - 1 - top_string);
          for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev) {
            for (ic = 0; ic < sizeof(char *); ic++)
            u1.rgc[ic] = pHash[ic];
            pHashPrev = u1.pc;
            pHash += sizeof(char *);

            if (top_string[sizeof(char *)] == pHash[0] && !strcmp(top_string + sizeof(char *) + 1, pHash + 1))
            return pHash;
          }

          if (fBootDb) {
            pString = top_string;
            top_string = plast;
            u1.pc = string_hash[iHash];
            for (ic = 0; ic < sizeof(char *); ic++)
            pString[ic] = u1.rgc[ic];
            string_hash[iHash] = pString;

            nAllocString += 1;
            sAllocString += top_string - pString;
            return pString + sizeof(char *);
          }
          else {
            return str_dup(top_string + sizeof(char *));
          }
        }
      }
    }
  }

  /*
* Read to end of line (for comments).
*/
  void fread_to_eol(FILE *fp) {
    char c;

    do {
      c = getc(fp);
    } while (c != '\n' && c != '\r');

    do {
      c = getc(fp);
    } while (c == '\n' || c == '\r');

    ungetc(c, fp);
    return;
  }

  /*
* Read one word (into static buffer).
*/
  char *fread_word(FILE *fp) {
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;
    do {
      cEnd = getc(fp);
    } while (isspace(cEnd));

    if (cEnd == '\'' || cEnd == '"') {
      pword = word;
    }
    else {
      word[0] = cEnd;
      pword = word + 1;
      cEnd = ' ';
    }

    for (; pword < word + MAX_INPUT_LENGTH; pword++) {
      *pword = getc(fp);
      if (cEnd == ' ' ? isspace(*pword) : *pword == cEnd) {
        if (cEnd == ' ')
        ungetc(*pword, fp);
        *pword = '\0';
        return word;
      }
    }
    bug("Fread_word: word too long.", 0);
    char buf[MSL];
    sprintf(buf, "Fread_word: word too long: %ld. %c%c%c%c", ftell(fp), word[0], word[1], word[2], word[3]);
    bug(buf, 0);
    //    exit(1);
    return NULL;
  }

  /*
* Duplicate a string into dynamic memory.
* Fread_strings are read-only and shared.
*/
  char *str_dup(const char *str) {
    char *str_new;

    if (str[0] == '\0')
    return &str_empty[0];

    if (str >= string_space && str < top_string)
    return (char *)str;

    str_new = (char *)alloc_mem(safe_strlen(str) + 1);
    strcpy(str_new, str);
    return str_new;
  }

  /*
* Free a string.
* Null is legal here to simplify callers.
* Read-only shared strings are not touched.
*/
  void free_string(char *pstr) {
    // char tmp[MSL];

    if (pstr == NULL || isInvalid(pstr) || pstr == &str_empty[0] || (pstr >= string_space && pstr < top_string))
    return;
    // sprintf(tmp, "MEMCHECK %d %s.", safe_strlen(pstr), pstr);
    // log_string(tmp);
    if (safe_strlen(pstr) > 0)
    free_mem(pstr, safe_strlen(pstr) + 1);
    // log_string("MEMCHECK: free string.");

    return;
  }

  _DOFUN(do_areas) {
    do_function(ch, &do_help, "arealist");
    return;
  }

  _DOFUN(do_dump) {
    int count, count2, num_pcs, aff_count;
    CHAR_DATA *fch;
    MOB_INDEX_DATA *pMobIndex;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    // EXIT_DATA *exit;
    DESCRIPTOR_DATA *d;
    AFFECT_DATA *af;
    FILE *fp;
    int vnum, nMatch = 0;

    /* open file */
    fp = fopen("mem.dmp", "w");

    /* report use of data structures */
    num_pcs = 0;
    aff_count = 0;

    /* mobile prototypes */
    fprintf(fp, "MobProt	%4d (%8lu bytes)\n", top_mob_index, top_mob_index * (sizeof(*pMobIndex)));

    /* mobs */
    count = 0;
    count2 = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      fch = *it;
      count++;
      if (fch->pcdata != NULL)
      num_pcs++;

      for (af = fch->affected; af != NULL; af = af->next)
      aff_count++;
    }

    for (CharList::iterator it = char_free.begin(); it != char_free.end(); ++it)
    count2++;

    fprintf(fp, "Mobs	%4d (%8lu bytes), %2d free (%lu bytes)\n", count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)));

    fprintf(fp, "Pcdata	%4d (%8lu bytes), %2d free (%lu bytes)\n", num_pcs, num_pcs * (sizeof(PC_DATA)), count, count * (sizeof(PC_DATA)));

    /* descriptors */
    count = 0;
    count2 = 0;
    DescList::iterator it;
    for (it = descriptor_list.begin(); it != descriptor_list.end(); ++it)
    count++;
    for (it = descriptor_free.begin(); it != descriptor_free.end(); ++it)
    count2++;

    fprintf(fp, "Descs	%4d (%8lu bytes), %2d free (%lu bytes)\n", count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));

    /* object prototypes */
    for (vnum = 0; nMatch < top_obj_index; vnum++)
    if ((pObjIndex = get_obj_index(vnum)) != NULL) {
      for (af = pObjIndex->affected; af != NULL; af = af->next)
      aff_count++;
      nMatch++;
    }

    fprintf(fp, "ObjProt	%4d (%8lu bytes)\n", top_obj_index, top_obj_index * (sizeof(*pObjIndex)));

    /* objects */
    ObjList::iterator oit;
    count = 0;
    count2 = 0;
    for (oit = object_list.begin(); oit != object_list.end(); ++oit) {
      count++;
      for (af = (*oit)->affected; af != NULL; af = af->next)
      aff_count++;
    }

    for (oit = obj_free.begin(); oit != obj_free.end(); ++oit)
    count2++;

    fprintf(fp, "Objs	%4d (%8lu bytes), %2d free (%lu bytes)\n", count, count * (sizeof(*obj)), count2, count2 * (sizeof(OBJ_DATA)));

    /* affects */
    count = 0;
    for (af = affect_free; af != NULL; af = af->next)
    count++;

    fprintf(fp, "Affects	%4d (%8lu bytes), %2d free (%lu bytes)\n", aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)));

    /* rooms */
    fprintf(fp, "Rooms	%4d (%8lu bytes)\n", top_room, top_room * (sizeof(ROOM_INDEX_DATA)));

    /* exits */
    fprintf(fp, "Exits	%4d (%8lu bytes)\n", top_exit, top_exit * (sizeof(EXIT_DATA)));

    fclose(fp);

    /* start printing out mobile data */
    fp = fopen("mob.dmp", "w");

    fprintf(fp, "\nMobile Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_mob_index; vnum++)
    if ((pMobIndex = get_mob_index(vnum)) != NULL) {
      nMatch++;
      fprintf(fp, "#%-4d %3d active killed     %s\n", pMobIndex->vnum, pMobIndex->count, pMobIndex->short_descr);
    }
    fclose(fp);

    /* start printing out object data */
    fp = fopen("obj.dmp", "w");

    fprintf(fp, "\nObject Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_obj_index; vnum++)
    if ((pObjIndex = get_obj_index(vnum)) != NULL) {
      nMatch++;
      fprintf(fp, "#%-4d %3d active %3d reset      %s\n", pObjIndex->vnum, pObjIndex->count, pObjIndex->reset_num, pObjIndex->short_descr);
    }

    /* close file */
    fclose(fp);
  }

  /*
* Stick a little fuzz on a number.
*/
  int number_fuzzy(int number) {
    switch (number_bits(2)) {
    case 0:
      number -= 1;
      break;
    case 3:
      number += 1;
      break;
    }

    return UMAX(1, number);
  }

  /*
* Generate a random number.
*/
  int number_range(int from, int to) {
    int power;
    int number;

    if (from == 0 && to == 0)
    return 0;

    if ((to = to - from + 1) <= 1)
    return from;

    for (power = 2; power < to; power <<= 1)
    ;

    while ((number = number_mm() & (power - 1)) >= to)
    ;

    return from + number;
  }

  /*
* Generate a percentile roll.
*/
  int number_percent(void) {
    int percent;

    while ((percent = number_mm() & (128 - 1)) > 99)
    ;

    return 1 + percent;
  }

  /*
* Generate a random door.
*/
  int number_door(void) {
    int door;

    while ((door = number_mm() & (16 - 1)) > 9)
    ;

    return door;
  }

  int number_bits(int width) { return number_mm() & ((1 << width) - 1); }

  /*
* I've gotten too many bad reports on OS-supplied random number generators.
* This is the Mitchell-Moore algorithm from Knuth Volume II.
* Best to leave the constants alone unless you've read Knuth.
* -- Furey
*/

  /* I noticed streaking with this random number generator, so I switched
back to the system srandom call.  If this doesn't work for you, define OLD_RAND to use the old system -- Alander */

#if defined(OLD_RAND)
  static int rgiState[2 + 55];
#endif

  void init_mm() {
#if defined(OLD_RAND)
    int *piState;
    int iState;

    piState = &rgiState[2];

    piState[-2] = 55 - 55;
    piState[-1] = 55 - 24;

    piState[0] = ((int)current_time) & ((1 << 30) - 1);
    piState[1] = 1;
    for (iState = 2; iState < 55; iState++) {
      piState[iState] =
      (piState[iState - 1] + piState[iState - 2]) & ((1 << 30) - 1);
    }
#else
    srandom(time(NULL) ^ getpid());
#endif
    return;
  }

  long number_mm(void) {
#if defined(OLD_RAND)
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState = &rgiState[2];
    iState1 = piState[-2];
    iState2 = piState[-1];
    iRand = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
    piState[iState1] = iRand;
    if (++iState1 == 55)
    iState1 = 0;
    if (++iState2 == 55)
    iState2 = 0;
    piState[-2] = iState1;
    piState[-1] = iState2;
    return iRand >> 6;
#else
    return random() >> 6;
#endif
  }

  /*
* Roll some dice.
*/
  int dice(int number, int size) {
    int idice;
    int sum;

    switch (size) {
    case 0:
      return 0;
    case 1:
      return number;
    }

    for (idice = 0, sum = 0; idice < number; idice++)
    sum += number_range(1, size);

    return sum;
  }

  /*
* Simple linear interpolation.
*/
  int interpolate(int level, int value_00, int value_32) {
    return value_00 + level * (value_32 - value_00) / 100;
  }

  /*
* Removes the tildes from a string.
* Used for player-entered strings that go into disk files.
*/
  void smash_tilde(char *str) {
    for (; *str != '\0'; str++) {
      if (*str == '~')
      *str = '-';
    }

    return;
  }

  void smash_vector(char *str) // MXP
  {
    return;
    for (; *str != '\0'; str++) {
      if (*str == '<')
      *str = '[';

      if (*str == '>')
      *str = ']';
    }

    return;
  }

  char *smash_MXP(char *str) // MXP
  {
    static char result[5][MSL * 2];
    static int i;
    ++i %= 5;
    static char *r;
    r = result[i];
    for (const char *p = str; !IS_NULLSTR(p); p++) {
      if (*p == '&') {
        *r++ = '&';
        *r++ = 'a';
        *r++ = 'm';
        *r++ = 'p';
        *r++ = ';';
      }
      else if (*p == '<') {
        *r++ = '&';
        *r++ = 'l';
        *r++ = 't';
        *r++ = ';';
      }
      else if (*p == '>') {
        *r++ = '&';
        *r++ = 'g';
        *r++ = 't';
        *r++ = ';';
      }
      else {
        *r++ = *p;
      }
    }
    *r = '\0';
    return result[i];
  }

  void smash_percent(char *str) {
    for (; *str != '\0'; str++) {
      if (*str == '%')
      *str = '/';
    }

    return;
  }

  /*
* Compare strings, case insensitive.
* Return TRUE if different
*   (compatibility with historical functions).
*/
  bool str_cmp(const char *astr, const char *bstr) {
    if (astr == NULL) {
      astr = str_dup("");
      if (bstr == NULL) {
        bstr = str_dup("");
        bug("Str_cmp: null astr.", 0);
        return TRUE;
      }
      char buf[MSL];
      sprintf(buf, "Str_cmp: null astr, bstr: %s", bstr);
      bug(buf, 0);
      return TRUE;
    }

    if (bstr == NULL) {
      bstr = str_dup("");
      char buf[MSL];
      sprintf(buf, "Str_cmp: null bstr, astr: %s", astr);
      bug(buf, 0);
      return TRUE;
    }
    if (astr[0] == '\0' && bstr[0] != '\0')
    return TRUE;
    if (astr[0] != '\0' && bstr[0] == '\0')
    return TRUE;
    for (; *astr || *bstr; astr++, bstr++) {
      if ((*astr) == '\0' && (*bstr) != '\0')
      return TRUE;
      if ((*astr) != '\0' && (*bstr) == '\0')
      return TRUE;

      if (LOWER(*astr) != LOWER(*bstr))
      return TRUE;
    }

    return FALSE;
  }

  /*
* Compare strings, case insensitive, for prefix matching.
* Return TRUE if astr not a prefix of bstr
*   (compatibility with historical functions).
*/
  bool str_prefix(const char *astr, const char *bstr) {
    if (astr == NULL) {
      bug("Strn_cmp: null astr.", 0);
      return TRUE;
    }

    if (bstr == NULL) {
      bug("Strn_cmp: null bstr.", 0);
      return TRUE;
    }

    for (; *astr; astr++, bstr++) {
      if (LOWER(*astr) != LOWER(*bstr))
      return TRUE;
    }

    return FALSE;
  }

  /*
* Compare strings, case insensitive, for match anywhere.
* Returns TRUE is astr not part of bstr.
*   (compatibility with historical functions).
*/
  bool str_infix(const char *astr, const char *bstr) {
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ((c0 = LOWER(astr[0])) == '\0')
    return FALSE;

    sstr1 = safe_strlen(astr);
    sstr2 = safe_strlen(bstr);

    for (ichar = 0; ichar <= sstr2 - sstr1; ichar++) {
      if (c0 == LOWER(bstr[ichar]) && !str_prefix(astr, bstr + ichar))
      return FALSE;
    }

    return TRUE;
  }

  /*
* Compare strings, case insensitive, for suffix matching.
* Return TRUE if astr not a suffix of bstr
*   (compatibility with historical functions).
*/
  bool str_suffix(const char *astr, const char *bstr) {
    int sstr1;
    int sstr2;

    sstr1 = safe_strlen(astr);
    sstr2 = safe_strlen(bstr);
    if (sstr1 <= sstr2 && !str_cmp(astr, bstr + sstr2 - sstr1))
    return FALSE;
    else
    return TRUE;
  }

  /*
* Returns an initial-capped string.
*/
  char *capitalize(const char *str) {
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for (i = 0; str[i] != '\0'; i++)
    strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
  }

  /*
* Append a string to a file.
*/
  void append_file(CHAR_DATA *ch, char *file, char *str) {
    FILE *fp;

    if (IS_NPC(ch) || str[0] == '\0')
    return;

    if ((fp = fopen(file, "a")) == NULL) {
      perror(file);
      send_to_char("Could not open the file!\n\r", ch);
    }
    else {
      fprintf(fp, "[%5d] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0, ch->name, str);
      fclose(fp);
    }

    return;
  }

  /*
* Reports a bug.
*/
  void bug(const char *str, int param) {
    char buf[MAX_STRING_LENGTH];

    if (fpArea != NULL) {
      int iLine;
      int iChar;

      if (fpArea == stdin) {
        iLine = 0;
      }
      else {
        iChar = ftell(fpArea);
        fseek(fpArea, 0, 0);
        for (iLine = 0; ftell(fpArea) < iChar; iLine++) {
          while (getc(fpArea) != '\n')
          ;
        }
        fseek(fpArea, iChar, 0);
      }

      sprintf(buf, "[*****] FILE: %s LINE: %d", strArea, iLine);
      log_string(buf);
      /* RT removed because we don't want bugs shutting the mud
if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
{
fprintf( fp, "[*****] %s\n", buf );
fclose( fp );
}
*/
    }

    strcpy(buf, "[*****] BUG: ");
    sprintf(buf + safe_strlen(buf), str, param);
    log_string(buf);
    /* RT removed due to bug-file spamming
if ( ( fp = fopen( BUG_FILE, "a" ) ) != NULL )
{
fprintf( fp, "%s\n", buf );
fclose( fp );
}
*/

    return;
  }

  /*
* Writes a string to the log.
*/
  void log_string(const char *str) {
    char *strtime;

    strtime = ctime(&current_time);
    strtime[safe_strlen(strtime) - 1] = '\0';
    fprintf(stderr, "%s :: %s\n", strtime, str);
    return;
  }

  void loadNotes() {
    InputFile inGroup(NOTEGROUP_FILE);

    if (inGroup) {
      char memb[1024], key[256], name[256];

      for (;;) {
        if (inGroup.isEof())
        break;

        if (str_cmp(inGroup.getWord(key), "Name"))
        break;

        inGroup.getString(name);

        if (str_cmp(inGroup.getWord(key), "Members"))
        break;

        inGroup.getString(memb);
        NoteBoard::addGroup(string(name), string(memb));
      }
    }
    else {
      bugf("[%s::%s] Unable to open file: %s", __FILE__, __FUNCTION__, NOTEGROUP_FILE);
      exit(1);
    }

    InputFile inBoard(NOTEBOARD_FILE);
    int cnt = 0;

    if (inBoard) {
      while (!inBoard.isEof()) {
        NoteBoard *nb = new NoteBoard(cnt);
        if (nb->readFrom(inBoard)) {
          NoteBoard::addBoard(nb);
          cnt++;
        }
        else
        delete nb;
      }
    }
    else {
      bugf("[%s::%s] Unable to open file: %s", __FILE__, __FUNCTION__, NOTEBOARD_FILE);
      exit(1);
    }

    InputFile inNotes(NOTES_FILE);

    if (inNotes) {
      while (!inNotes.isEof()) {
        Note *n = new Note(inNotes);
        time_t now = time(NULL);
        long interval = 60 * 60 * 24 * 60; // 60 days

        if ((now - n->getDateStamp()) > interval)
        continue;

        if (n->getNumber() > Note::counter)
        Note::counter = n->getNumber();
        if (n->getThread() > Note::threadCounter)
        Note::threadCounter = n->getThread();
        n->getBoard()->add(n);
        n->getBoard()->sort();
      }
    }
    else {
      bugf("[%s::%s] Unable to open file: %s", __FILE__, __FUNCTION__, NOTES_FILE);
      exit(1);
    }
  }

  // old safe_strlen color
  int old_safe_strlen_color(const char *argument) {
    const char *str;
    int safe_strlength = 0;

    if (argument != NULL && argument[0] != '\0') {
      str = argument;

      while (*str != '\0') {
        if (*str != '`') {
          str++;
          safe_strlength++;
          continue;
        }
        if (*(++str) == '`') {
          safe_strlength++;
        }
        str++;
      }
    }
    return safe_strlength;
  }

  // new component of safe_strlen color used with old safe_strlen color to handle html and
  // xterm colors
  int safe_strlen_disco_lor(char string[MSL]) {
    int length = 0, colorlength = 0, i = 0;
    length = old_safe_strlen_color(string);
    // length = safe_strlen(string);
    colorlength = length;
    int counter = 0, multiplier = 0;

    for (i = 0; i < length; i++) {
      if (string[i] == '`') {
        if (string[i + 1] == '#') {
          counter++;
          if (counter == 5) {
            multiplier++;
            counter = 0;
          }

          colorlength = colorlength - 6; //-(6*multiplier);
        }
        else if (isdigit(string[i + 1]) && isdigit(string[i + 2]) && isdigit(string[i + 3])) { // Xterm
          colorlength = colorlength - 2;
        }
        continue;
      }
    }

    return colorlength;
  }

  // experimental -- not used
  int safe_strlen_newcolor(const char *argument) {
    int length = 0, colorlength = 0, i = 0;
    char string[MSL];

    strcpy(string, argument);
    length = safe_strlen(string);

    for (i = 0; i < length; i++) {
      if (string[i] == '`') {
        colorlength += 1;
      }
      else if (string[i] == MXP_BEGc) {
        colorlength += 5;
      }
      else if (string[i] == MXP_ENDc) {
        colorlength += 5;
      }
      else {
        colorlength++;
      }
    }

    return colorlength;
  }

  int safe_strlen_color(const char *argument) {
    Buffer cstring;
    return atoi(tyr_lor(cstring, argument, NULL, COLOR_LENGTH));
    char string[MSL];
    int safe_strlength = 0;
    strcpy(string, argument);
    safe_strlength = safe_strlen_disco_lor(string);
    return safe_strlength;

    /*Old version
const char *str;
int safe_strlength = 0;

if (argument != NULL && argument[0] != '\0')
{
str = argument;

while (*str != '\0')
{
if(*str != '`')
{
str++;
safe_strlength++;
continue;
}
if (*(++str) == '`')      {safe_strlength++;}
str++;
}
}
return safe_strlength;
*/
  }

  void load_quotes(void) {
    int n = 0;
    FILE *fp;
    char buf[100];

    sprintf(buf, "%squote.1", QUOTE_DIR);

    while ((fp = fopen(buf, "r")) != NULL) {
      fclose(fp);
      n++;
      sprintf(buf, "%squote.%d", QUOTE_DIR, n);
    }
    n--;

    num_quotes = n;
    sprintf(buf, "%d Quote files found in %s.", num_quotes, QUOTE_DIR);
    log_string(buf);
    return;
  }

  void load_objprogs(FILE *fp) {
    PROG_CODE *pOprog;

    if (area_last == NULL) {
      bug("Load_objprogs: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;

      letter = fread_letter(fp);
      if (letter != '#') {
        bug("Load_objprogs: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      fBootDb = FALSE;
      if (get_prog_index(vnum, PRG_OPROG) != NULL) {
        bug("Load_objprogs: vnum %d duplicated.", vnum);
        exit(1);
      }
      fBootDb = TRUE;

      pOprog = (PROG_CODE *)alloc_perm(sizeof(*pOprog));
      pOprog->vnum = vnum;
      pOprog->code = fread_string(fp);
      oprog_list.push_front(pOprog);
      top_oprog_index++;
    }
    return;
  }

  void load_roomprogs(FILE *fp) {
    PROG_CODE *pRprog;

    if (area_last == NULL) {
      bug("Load_roomprogs: no #AREA seen yet.", 0);
      exit(1);
    }

    for (;;) {
      int vnum;
      char letter;

      letter = fread_letter(fp);
      if (letter != '#') {
        bug("Load_roomprogs: # not found.", 0);
        exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      break;

      fBootDb = FALSE;
      if (get_prog_index(vnum, PRG_RPROG) != NULL) {
        bug("Load_roomprogs: vnum %d duplicated.", vnum);
        exit(1);
      }
      fBootDb = TRUE;

      pRprog = (PROG_CODE *)alloc_perm(sizeof(*pRprog));
      pRprog->vnum = vnum;
      pRprog->code = fread_string(fp);
      rprog_list.push_front(pRprog);
      top_rprog_index++;
    }
    return;
  }

  void fix_objprogs(void) {
    OBJ_INDEX_DATA *pObjIndex;
    PROG_LIST *list;
    PROG_CODE *prog;
    int iHash;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pObjIndex = obj_index_hash[iHash]; pObjIndex != NULL;
      pObjIndex = pObjIndex->next) {
        for (list = pObjIndex->oprogs; list != NULL; list = list->next) {
          if ((prog = get_prog_index(list->vnum, PRG_OPROG)) != NULL)
          list->code = prog->code;
          else {
            bug("Fix_objprogs: code vnum %d not found.", list->vnum);
            exit(1);
          }
        }
      }
    }
  }

  void fix_roomprogs(void) {
    ROOM_INDEX_DATA *pRoomIndex;
    PROG_LIST *list;
    PROG_CODE *prog;
    int iHash;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL;
      pRoomIndex = pRoomIndex->next) {
        for (list = pRoomIndex->rprogs; list != NULL; list = list->next) {
          if ((prog = get_prog_index(list->vnum, PRG_RPROG)) != NULL)
          list->code = prog->code;
          else {
            bug("Fix_roomprogs: code vnum %d not found.", list->vnum);
            exit(1);
          }
        }
      }
    }
  }

  void mob_wealth(CHAR_DATA *mob) {
    int money = 0;
    int wealth;

    wealth = mob->pIndexData->wealth;

    if (wealth < 1 || wealth > 100)
    return;

    if (number_percent() % 3 != 0)
    return;

    switch (wealth) {
      /* Light Copper */
    case 1:
      money = number_range(1, 3);
      break;

      /* Heavy Copper */
    case 2:
      money = number_range(1, 5);
      break;

      /* Light Silver */
    case 3:
      money = number_range(5, 10);
      break;

      /* Heavy Silver */
    case 4:
      money = number_range(10, 20);
      break;

      /* Light Gold Only */
    case 5:
      money = number_range(20, 35);
      break;

      /* Light Gold, Light Silver, Copper */
    case 6:
      money = number_range(30, 50);
      break;

      /* Heavy Gold, Light Silver, Copper */
    case 7:
      money = number_range(50, 75);
      break;
    }

    mob->money = money;
  }

  /*
* This function is here to aid in debugging.
* If the last expression in a function is another function call, *   gcc likes to generate a JMP instead of a CALL.
* This is called "tail chaining."
* It hoses the debugger call stack for that call.
* So I make this the last call in certain critical functions, *   where I really need the call stack to be right for debugging!
*
* If you don't understand this, then LEAVE IT ALONE.
* Don't remove any calls to tail_chain anywhere.
*
* -- Furey
*/
  void tail_chain(void) { return; }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

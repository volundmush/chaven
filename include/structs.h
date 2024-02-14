#ifndef _STRUCTS_H_
  #define _STRUCTS_H_
  
  using namespace std;
  
  
  #if defined(__cplusplus)
    extern "C" {
    #endif
    
    /*
      struct lstr
      {
      bool operator()(const char* s1, const char* s2) const
      {
      return strcmp(s1, s2) < 0;
      }
      };
    */
    
    typedef unsigned long SET[SETSIZE];
    class Note;
    class NoteBoard;
    
    /*
      * Structure types.
    */
    #if defined(WIN32)
      //typedef HANDLE pid_t;
      typedef int pid_t;
    #endif
    
    typedef struct  affect_data   AFFECT_DATA;
    typedef struct  area_data   AREA_DATA;
    typedef struct  auction_data    AUCTION_DATA;
    typedef struct  ban_data    BAN_DATA;
    typedef struct  bet_data    BET_DATA;
    typedef struct  bond_data   BOND_DATA;
    typedef struct  char_data   CHAR_DATA;
    typedef struct  char_mem_data   MEM_DATA;
    typedef struct  chest_list_type   CHEST_DATA;
    typedef struct  city_type   CITY_TYPE;
    typedef struct  skill_type    SKILL_TYPE;
    typedef struct  path_type   PATH_TYPE;
    typedef struct  arch_type   ARCH_TYPE;
    typedef struct modifier_type    MODIFIER_TYPE;
    typedef struct  industry_type   INDUSTRY_TYPE;
    typedef struct  clan_type               CLAN_TYPE;
    typedef struct  faction_type    FACTION_TYPE;
    typedef struct  institute_type    INSTITUTE_TYPE;
    typedef struct  phonebook_type    PHONEBOOK_TYPE;
    typedef struct  cabal_type    CABAL_TYPE;
    typedef struct  grave_type    GRAVE_TYPE;
    typedef struct  lair_type   LAIR_TYPE;
    typedef struct  rosterchar_type   ROSTERCHAR_TYPE;
    typedef struct  gallery_type    GALLERY_TYPE;
    typedef struct  page_type   PAGE_TYPE;
    typedef struct  storyline_type    STORYLINE_TYPE;
    typedef struct  archive_entry   ARCHIVE_ENTRY;
    typedef struct  archive_index   ARCHIVE_INDEX;
    typedef struct  house_type    HOUSE_TYPE;
    typedef struct  prop_type   PROP_TYPE;
    typedef struct  domain_type		DOMAIN_TYPE;
    typedef struct  story_type            STORY_TYPE;
    typedef struct  weekly_type   WEEKLY_TYPE;
    typedef struct  location_type   LOCATION_TYPE;
    typedef struct  account_type    ACCOUNT_TYPE;
    typedef struct  storyidea_type    STORYIDEA_TYPE;
    typedef struct  petition_type   PETITION_TYPE;
    typedef struct  operation_type    OPERATION_TYPE;
    typedef struct  plot_type   PLOT_TYPE;
    typedef struct  fantasy_type    FANTASY_TYPE;
    typedef struct  destiny_type    DESTINY_TYPE;
    typedef struct  newdestiny_type NEWDESTINY_TYPE;
    typedef struct  event_type              EVENT_TYPE;
    typedef struct  research_type           RESEARCH_TYPE;
    typedef struct  extra_encounter_type    EXTRA_ENCOUNTER_TYPE;
    typedef struct  extra_patrol_type       EXTRA_PATROL_TYPE;
    typedef struct  anniversary_type       ANNIVERSARY_TYPE;
    typedef struct  decree_type             DECREE_TYPE;
    typedef struct  grouptext_type          GROUPTEXT_TYPE;
    typedef struct  texthistory_type	  TEXTHISTORY_TYPE;

    typedef struct  profile_type    PROFILE_TYPE;
    typedef struct  match_type      MATCH_TYPE;
    typedef struct  chatroom_type   CHATROOM_TYPE;
    
    
    typedef struct  news_type               NEWS_TYPE;
    typedef struct  gateway_type    GATEWAY_TYPE;
    typedef struct  playerroom_type         PLAYERROOM_TYPE;
    //typedef struct  participant_type  PARTICIPANT_TYPE;
    //typedef struct  fight_type    FIGHT_TYPE;
    typedef struct  discipline_type   DISCIPLINE_TYPE;
    typedef struct  monster_type    MONSTER_TYPE;
    typedef struct  special_type    SPECIAL_TYPE;
    typedef struct  dungeon_type    DUNGEON_TYPE;
    typedef struct  shop_type              SHOP_TYPE;
    typedef struct  job_type    JOB_TYPE;
    typedef struct  feature_type  FEATURE_TYPE;
    typedef struct  taxi_type   TAXI_TYPE;
    typedef struct  cmd_type    CMD_TYPE;
    typedef struct  color_data    COLOR_DATA;
    typedef struct  coordinate_data   COORDINATE_DATA;
    typedef struct  descriptor_data   DESCRIPTOR_DATA;
    typedef struct  disabled_data   DISABLED_DATA;
    typedef struct  entrance_data     ENTRANCE_DATA;
    typedef struct  exit_data   EXIT_DATA;
    typedef struct  extra_descr_data  EXTRA_DESCR_DATA;
    typedef struct  help_area_data    HELP_AREA;
    typedef struct  help_data   HELP_DATA;
    typedef struct  kill_data   KILL_DATA;
    typedef struct  mob_index_data    MOB_INDEX_DATA;
    typedef struct  group_index_data  GROUP_INDEX_DATA;
    typedef struct  memorize_data   MEMORIZE_DATA;
    typedef struct  obj_data    OBJ_DATA;
    typedef struct  obj_index_data    OBJ_INDEX_DATA;
    typedef struct  pc_data     PC_DATA;
    typedef struct  prog_code   PROG_CODE;
    typedef struct  prog_list   PROG_LIST;
    typedef struct  reset_data    RESET_DATA;
    typedef struct  restring_data   RESTRING_DATA;
    typedef struct  room_index_data   ROOM_INDEX_DATA;
    typedef struct  shop_data   SHOP_DATA;
    typedef struct  system_data   SYSTEM_DATA;
    typedef struct  time_info_data    TIME_INFO_DATA;
    typedef struct  thing_data    THING;
    typedef struct  weather_data    WEATHER_DATA;
    typedef list<CHAR_DATA *>   CharList;
    typedef list<OBJ_DATA *>    ObjList;
    typedef list<AREA_DATA *>   AreaList;
    typedef list<DESCRIPTOR_DATA *>   DescList;
    typedef list<CHEST_DATA *>    ChestList;
    typedef list<PROG_CODE *>   ProgList;
    typedef list<DISABLED_DATA *>   DisabledList;
    typedef list<SHOP_DATA *>   ShopList;
    typedef list<BAN_DATA *>    BanList;
    
    /*
      * Function types.
    */
    typedef void DO_FUN       args( ( CHAR_DATA *ch, char *argument ) );
    typedef bool SPEC_FUN     args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
    typedef void SPELL_FUN    args( ( int sn, int level, CHAR_DATA *ch, void *vo,int target ) );
    typedef void SPELL_HEAL_FUN   args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target, OBJ_DATA *obj ) );
    typedef void HERB_FUN     args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
    typedef void OBJ_FUN      args( ( OBJ_DATA *obj, char *argument ) );
    typedef void ROOM_FUN     args( ( ROOM_INDEX_DATA *room, char *argument ) );
    typedef void FORM_FUN     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
    
    typedef map<long, char *, less<long> > NameMap;
    
    /**
      * @struct system_data structs.h
      * System Data
      *
      * Used to keep track of system setting and statistics.
    */
    struct system_data
    {
      /** Name of the MUD */
      char *  mud_name;
      
      /** Maximum players this boot */
      int   maxplayers;
      
      /** Maximum players ever */
      int   alltimemax;
      
      /** Time of max ever */
      char *  time_of_max;
    };
    
    
    /**
      * Chest Data
      *
      * This structure contains data for the saving of guild chests or other
      * containers in the MUD with contents that need to be persistent.  It
      * is used as nodes in a linked -list that contains all the persistent
      * containers.
    */
    struct chest_list_type
    {
      /** The container that should have persistent data */
      OBJ_DATA*       chest;
      
      /** The vnum of the container specified at this node */
      int             vnum;
      
      /** Number of the guild the chest belongs to **/
      int       guild;
    };
    typedef struct  note_data               NOTE_DATA;
    
    
    /**
      * Ban Data
      *
      * Contains information about domains that are banned from
      * the MUD.
    */
    struct  ban_data
    {
      bool  valid;
      
      /**
        * Flags that determine the type of ban.
        * see const.h (BAN_*)
      */
      sh_int  ban_flags;
      
      /** Trust level of the imm that placed the ban */
      sh_int  level;
      
      /** Name of domain to ban */
      char *  name;
    };
    
    
    /**
      * Timerset Data
      *
      * Used to keep time stats for execution times of commands
      * Each time a command is run its timer data is updated.
      * The cedit command can be used to see the execution times
      * of a command
    */
    struct timerset
    {
      /** Number of times this command has executes */
      int num_uses;
      
      /** Total combined time for all executions */
      struct timeval total_time;
      
      /** Shortest execution time */
      struct timeval min_time;
      
      /** Longest execution time */
      struct timeval max_time;
    };
    
    
    struct  feature_type
    {
      char *              names;
      char *      shortdesc;
      char *      desc;
      int     type;
      int       position;
      int     hp;
      bool    valid;
    };
    
    
    
    /**
      * Structure for a command in the command lookup table.
    */
    struct  cmd_type
    {
      /** Next command in the hashed command list */
      CMD_TYPE *    next;
      
      /** Name of the command */
      char *              name;
      
      /** Function to execute when this command is called */
      DO_FUN *            do_fun;
      
      /** Minimum positon need for char to execute this command */
      sh_int              position;
      
      /** Minimum level required to execute this command */
      sh_int              level;
      
      /** Should this command be logged? */
      sh_int              log;
      
      /** */
      long                flags;
      
      /** Show this in the list of commands? */
      sh_int              show;
      
      /** Name of the function to execute for this command */
      char                *lookup_name;
      
      /** Contains the execution times for this command */
      struct timerset userec;
      
      /** Indicates whether or not this is a valid command */
      bool    valid;
      
      /** Department that the command is designed for. */
      int     department;
    };
    
    
    /**
      * Disabled Command Data
      *
      * Keeps data about commands that have been disabled.  Used as a
      * node in a linked list of all disabled commands (disable_list)
    */
    struct disabled_data
    {
      /** Pointer to next node in disabled list */
      DISABLED_DATA     *next;
      
      /** Pointer to the command that is disabled */
      struct cmd_type const   *command;
      
      /** name of the imm who disabled the command */
      char      *disabled_by;
      
      /** Trust level of the imm who disabled the command */
      sh_int      level;
      
      /** Reason the command was disabled */
      char      *reason;
    };
    
    /**
      * Time Info Data
      *
      * Contains data about the current time on the MUD
    */
    struct  time_info_data
    {
      int   minute;
      
      /** The hour */
      int   hour;
      
      /** The day */
      int   day;
      
      /** The month */
      int   month;
      
      /** The year */
      int   year;
      
      int   local_temp;
      
      int   des_temp;
      int   local_cover_total;
      int   cover_trend;
      int   density_trend;
      int   local_density_total;
      
      int   understanding;
      
      long  phone;
      
      int   local_mist_level;
      int   mist_timer;
      
      int   faction_vnum;
      
      int   lterrain;

      int   lweek_tier;
      int   lweek_total;
      int   tweek_tier;
      int   tweek_total;
      
      int   monster_hours;
      int   mutilate_hours;
      int   cult_alliance_issue;
      int   cult_alliance_type;
      int   sect_alliance_issue;
      int   sect_alliance_type;   
      int   bloodstorm;
    };
    
    
    /**
      * @struct descriptor_data structs.h
      * Descriptor (channel) structure.
      *
      * Contain information about all the current connections
      * to the MUD.  Kept in a linked list (descriptor_list)
    */
    struct descriptor_data
    {
      /**
        * Pointer to the descriptor of the imm who is
        * snooping this character
      */
      DESCRIPTOR_DATA * snoop_by;
      
      /** Pointer to character this descriptor is associated to */
      CHAR_DATA *   character;
      
      ACCOUNT_TYPE *  account;
      
      /**
        * Pointer to this descriptors original character.
        * Used when an immortal 'switches' into another char/mob
      */
      CHAR_DATA *   original;
      
      /** Indeicates if this is a currently valid descriptor */
      bool    valid;
      
      /** name of the host this descriptor is connected from */
      char *    host;
      
      /** IP of the host this descriptor is connected from */
      char *    hostip;
      
      /** Descriptor number */
      sh_int    descriptor;
      
      /** This descriptors connected state (see CON_* in const.h) */
      sh_int    connected;
      
      
      /** Indicateswhether or not a command is wiating to be executed */
      bool    fcommand;
      
      /** Input buffer for this descriptor */
      char    inbuf   [4 * MAX_INPUT_LENGTH];
      
      /** Incomming command taken from inbuf */
      char    incomm    [MAX_INPUT_LENGTH];
      
      /** Last input command received for this descriptor */
      char    inlast    [MAX_INPUT_LENGTH];
      
      /** Count of repeated command.  To catch spammers */
      int     repeat;
      /** This descriptors output buffer */
      char *    outbuf;
      /** Size of this desciptors output buffer */
      int     outsize;
      /** Where to start output of the buffer */
      int     outtop;
      /** */
      char *    showstr_head;
      /** */
      char *    showstr_point;
      /** Void pointer to whatever is being editted in OLC */
      void *              pEdit;
      
      /** String this descriptor is currently editting */
      char **             pString;
      
      /** OLC Editor this descriptor is currently using */
      int     editor;
      
      /** Indicates if this descriptor is using color */
      bool    ansi;
      
      /** */
      char *    ident;
      
      /**
        * File descriptor open to this descriptor's host
        * resolving process.  Used to pass resolve results
        * back to main process
      */
      int     ifd;    /* Ident file descriptor */
      
      /**
        * The process ID of this descriptors host
        * resolving process
      */
      //#if defined(WIN32)
      //  DWORD   ipid;
      //#else
      pid_t   ipid;   /* Ident proc id */
      //#endif
      
      /** Indicates if this descriptor's host has been resolved */
      bool    got_ident;
      
      //MXP - Discordance
      bool    mxp;   /* player using MXP flag */
    };
    
    struct race_type
    {
      /** Name of this race */
      char *      name;
      
      /** Display name of the race */
      char *      who_name;
      
      /** Indicates if this is a PC race */
      bool        pc_race;
      
      bool  creatable;
      
      int         vnum;
      int         tier;
      bool        super;
      bool  change_to;
      int         min_age;
      int         max_age;
      int         min_aage;
      int         max_aage;
    };
    
    
    /**
      * @struct str_app_type structs.h
      *
      * Strength attribute bonus structures.  Used to determine
      * what bonuses are awarded for different strenght levels.
    */
    struct  str_app_type
    {
      /** Bonus added to hit roll */
      sh_int  tohit;
      
      /** Bonus added to dam roll */
      sh_int  todam;
      
      /** Bonus added to how much a char can carry */
      sh_int  carry;
      
      /** How heavy a weapon can they wield */
      sh_int  wield;
    };
    
    /**
      * @struct help_data structs.h
      *
      * Contains information about help files.  Used as a node
      * in a linked list of all the help files.
    */
    struct  help_data
    {
      /** Pointer to the next help file in the list */
      HELP_DATA   *next;
      
      /** Leve this help file can be viewed at */
      sh_int  level;
      
      /** The keyword for this help file */
      char *  keyword;
      
      /** The actual contents of the help file */
      char *  text;
      
      /** Indicates if this help file should be put on web page */
      bool  online;
      
      /** Indicates if this help file should be saved */
      bool  del;
      
      /** Any refernce Helpfiles? **/
      char *  see_also;
      
      /** What type of helpfile?  Determines printout to mortals. **/
      sh_int  type;
      
    };
    
    struct help_area_data
    {
      HELP_AREA * next;
      HELP_DATA * first;
      HELP_DATA * last;
      AREA_DATA * area;
      char *  filename;
      bool  changed;
    };
    
    /**
      * @struct shop_data structs.h
      *
      * Contains information about a shop.
    */
    struct  shop_data
    {
      /** Vnum of the shopkeeper mob */
      int keeper;
      
      /** Items types the shop will buy */
      sh_int  buy_type [MAX_TRADE];
      
      /** Cost multiplier for buying */
      sh_int  profit_buy;
      
      /** Cost multiplier for selling */
      sh_int  profit_sell;
      
      /** Hour when the shop opens */
      sh_int  open_hour;
      
      /** Hour when the shop closes */
      sh_int  close_hour;
      
      /** The guild that owns the shop */
      sh_int  owner;      /* Guild owner of a shop  */
      
      /** How much profit is given to the owner */
      sh_int  proceed;
    };
    
    
    
    
    struct  job_type
    {
      int   vnum;
      
      char *      name;
      char *  name2;
      char *  name3;
      
      char *  client;
      
      char *  closing_msg;
      
      char *  attack1;
      char *  attack1_msg;
      char *  attack1_msg_o;
      
      char *  attack2;
      char *  attack2_msg;
      char *  attack2_msg_o;
      
      char *  attack3;
      char *  attack3_msg;
      char *  attack3_msg_o;
      
      char *  attack4;
      char *  attack4_msg;
      char *  attack4_msg_o;
      
      char *  attack5;
      char *  attack5_msg;
      char *  attack5_msg_o;
      
      char *  close1;
      char *  close1_msg;
      char *  close1_msg_o;
      
      char *  close2;
      char *  close2_msg;
      char *  close2_msg_o;
      
      char *  close3;
      char *  close3_msg;
      char *  close3_msg_o;
      
      char *  close4;
      char *  close4_msg;
      char *  close4_msg_o;
      
      char *  shield1;
      char *  shield1_msg;
      char *  shield1_msg_o;
      
      char *  shield2;
      char *  shield2_msg;
      char *  shield2_msg_o;
      
      char *  shield3;
      char *  shield3_msg;
      char *  shield3_msg_o;
      
      
      int   room_lower;
      int   room_upper;
      
      int   area;
      int   sector;
      
      int   payscale;
    };
    
    struct dungeon_type
    {
      int   type;
      int   target;
      int   align;
      int   size;
      char *  minion_1;
      char *  minion_2;
      CHAR_DATA *traps[60];
      int   trap_info[60][5];
      int   start_vnum;
      CHAR_DATA *mobs[50];
      CHAR_DATA *buttons[20];
      int   button_info[20][2];
      CHAR_DATA *boss;
      int   level;
      int   end_room;
      int   terminal_rooms[30];
      bool  valid;
    };
    
    struct district_type
    {
      int         area;
      char *      name;
      char *  nickname;
      int   x;
      int   y;
      int   wealth;
    };
    struct discipline_type
    {
      int         vnum;
      char *      name;
      int   range;
      int   pc;
    };
    
    struct monster_type
    {
      int   vnum;
      int   world;
      int   capture;
      int   humanoid;
      char *  object;
    };
    
    struct worldlink_type
    {
      int earth_vnum;
      int offworld_vnum;
    };
    
    struct taxi_type
    {
      int vnum;
      int x;
      int y;
    };
    
    struct special_type
    {
      int         vnum;
      char *      name;
      int   pc;
    };
    
    
    struct skill_type
    {
      int         vnum;
      char *      name;
      int   levels[6];
    };
    
    struct arch_type
    {
      int   vnum;
      int   tier;
      bool  super;
      bool  change_to;
      int   min_age;
      int   max_age;
      int   min_aage;
      int   max_aage;
    };
    
    struct modifier_type
    {
      int   vnum;
      char *  name;
      int   amount;
    };
    
    struct cliquerole_type
    {
      int   vnum;
      char *  name;
      int   clique;
    };
    
    struct industry_type
    {
      int   vnum;
      char *  name;
      int   jobs;
      int   covers;
      int   pay;
    };
    
    struct path_type
    {
      int         vnum;
      char *  name;
      char *  prefix;
      char *      levelone;
      char *     leveltwo;
      char *     levelthree;
      int     opage_male;
      int   opage_female;
      int     minage;
      int     pathfamily;
      int   supernatural;
      int   support;
    };
    
    struct playerroom_type
    {
      char *      author;
      
      int         cooldown;
      int   vnum;
      int   status;
      
      bool  checked;
      
      bool valid;
      
    };
    
    struct research_type
    {
      char *      author;
      
      int         timer;
      
      char *      message;
      
      int   stats[10];
      
      char *  participants;
      
      int   difficulty;
      
      bool valid;
      
    };

    struct extra_encounter_type
    {
      int btype;
      int id;
      int encounter_id;
      int clan_id;
      char * text;
      
      bool valid;
      
    };

    struct extra_patrol_type
    {
      int id;
      int clan_id;
      char * intro;
      char * conclusion;
      char * messages;
      
      bool valid;
      
    };


    struct decree_type
    {
      int territory_vnum;
      int btype;
      int created_at;
      int start_time;
      int end_time;
      char * vote_1;
      char * vote_2;
      char * vote_3;
      char * desc;      
      char * target;
      int target_number;
      bool valid;
      
    };

    struct anniversary_type
    {
      int id;
      int btype;
      int happen_at;
      char * summary;
      char * news;
      char * eidilon_name;
      char * cult_name;
      char * sect_name;
      char * messages;
      
      bool valid;
      
    };

    
    struct news_type
    {
      char *      author;
      
      int         timer;
      
      char *      message;
      
      int   stats[10];
      
      bool valid;
      
    };
    
    
    struct account_type
    {
      char *    name;
      /** Indicates if the pc-data has been validated */
      bool        valid;
      
      /** The PCs password */
      char *      pwd;
      
      /** The PCs password temporarily unecrypted for forum setup - Discordance */
      char *      upwd;
      
      char *    creation_ip; //The address the account was created from - Discordance
      
      SET         flags;
      
      int     maxhours;
      
      int     focusedchar;
      int     focusedcount;
      
      char *    characters[25];
      
      char *    pastcharacters;

     char * 	email;
      
      int     char_stasis[25];
      
      int	lastshrine;
      int     lastnotalone;
      int     last_scheme;
      int     factiontime;
      int     newcharcool;
      int     newcharcount;
      int     roster_cool;
      int     prey_cool;
      int     prey_cool_s;
      int     predator_cool;
      int     target_encounter_cooldown;

      int     villain_cooldown;
      int     villain_score;
      int     villain_mod;
    

      int      sparring_xp;
      
      int     tier_count;
      int     total_count;

      int     storyidea_cooldown;
      int     encounter_cooldown;
      
      int     last_bribe;
      int     last_bargain;
      int     last_ghost;
      int     last_decurse;
      int     last_war;
      int     last_awar;
      int     last_pred;
      int     last_hunt;
      
      int           awards;
      int           award_progress;
      
      int     unrewarded_donation;
      int     paycooldown;
      int     socialcooldown;
      int     hightiercooldown;
      int     factioncooldown;
      char *    lastfacchar;
      char *    lasttierchar;
      char *    sr_history;
      int     encounter_karma;
      int     adventure_karma;
      int     mystery_karma;
      int     ambiant_karma;
      int     monster_karma;
      int     misc_karma;
      int     other_karma;
      int     scheme_karma;
int     dream_karma;
      int     offworld_karma;
      int     world_books;
      int     earth_books;
      int     haven_books;
      int     xp;
      int     rpxp;
      int     karma;
      int     pkarma;
      int     karmabank;
      int     karmaearned;
      int     donated;
      int                 renames;
      int                 colours;
      int                 bandaids;
      
      int     pkarmaspent;
      int     daily_pvp_exp;
      int     last_infuse_date;
      int     award_karma;
      
      char *              sr_website;
      char *              sr_info;
      char *              sr_logs;
	  char *              sr_fdesc;
      int     sr_aexp;
      
      char *    friends[25];
      int     friend_type[25];
    };
    
    struct weekly_type
    {
      char *  charname;
      char *  surname;
      int     logon;
      bool valid;
    };
    
    struct story_type
    {
      char *      author;
      
      int   time;

      int   created_at;

      int   isprivate;

      char * invite_list;
      char * hide_list;
      int  ctype;
      char * security_ban_list;
      char * security_letin_list;

      int   isprotected;
      
      int   crisis;

      int   dedication;
      
      char *  type;
      
      char *  location;
      
      char *  stats;
      
      int   advertised;
      
      char *  description;
      
      char *  comments[50];
      
      bool valid;
      
    };
    
    struct location_type
    {
      char *      name;
      
      int   status;
      int   continent;
      int   hand;
      int   order;
      int   temple;
      int   timezone;
      
      int   lockout;
      int   phil_amount[20];
      
      char *  notes;
      char *  government;
      char *  people;
      char *  place;
      char *  timeline;
      char * fixtures;
      int   x;
      int   y;
      int   radius;
      int   colour;
      char *      other_name[10];
      int   other_amount[10];
      int   battleclimate;
      
      char *  plant_desc[20];
      char *  place_desc[20];
      char *  base_desc_core;
      int   base_faction_core;
      char *  base_desc_cult;
      int   base_faction_cult;
       char *  base_desc_sect;
      int   base_faction_sect;     

      int   base_type;

      
      int   status_timer;
      
      bool valid;
      
    };
    struct petition_type
    {
      char *      author;
      char *  account;
      int   room;
      int   timestamp;
      int   create_time;
      int   status;
      int   type;
      char *  sentto;
      char *  description;
      bool valid;
    };
    struct operation_type
    {
      char *      author;
      int   territoryvnum;
      int   adversary_type;
      char *  adversary_name;
      char *  room_name;
      char *  description;
      char *  bag_name;
      char *  upload_name;
      char *  preferred;
      char *  storyline;
      char *  timeline;
      char *  elitestring;
      char *  storyrunners;
      char *  atmosphere;
      int   timeshifted;
      int   goal;
      int   initdays;
      int   spam;
      bool  calseen;
      char *  target;
      char *  sign_up[100];
      int   sign_up_type[100];
      int   type;
      int   max_pcs;
      int   speed;
      int   terrain;
      int   size;
      int   hour;
      int   day;
      int   faction;
      int   bonus;
      int   enrolled[10];
      int   soldiers[10];
      int   home_soldiers;
      int   initial_power;
      int   timer;
      int   power;
      int   waves;
      int   battleground_number;
      int   factimers[10];
      int   poix[10];
      int   poiy[10];
      int   poitype[10];
      int   poifaction[10];
      int   poibg[10];
      int   upload;
      int   home_uploads;
      int   uploads[10];
      int   upload_cooldown[3];
      int   competition;
      int   challenge;
      bool valid;
    };
    struct storyidea_type
    {
      char *      author;
      
      char *  claimant;
      
      char *  mastermind;
      int   crisis;
      int   type;
      int   subfactionone;
      int   subfactiontwo;
      int   reward_type;
      int   seeking_type;
      
      int         status;
      
      char *      description;
      char *  members;
      char * eidolon;
      char *      npcs;
      char *  storyline;
      char *      lore;
      
      char *      secrets;
      
      char *  territory;
      
      char *  galleries[10];
      
      int   time_made;
      int   finale;
      
      int   time_locked;
      int   time_owned;
      
      bool valid;
      
    };
    
struct grouptext_type
{
    char * tname;
    int last_msg;
    char *history;
    int pnumber[10];
    bool valid;
};

struct texthistory_type
{
    char * name_one;
    char * name_two;
    int last_msg;
    char *history;
    bool valid;
};

struct profile_type
{
  char * name;
  char * handle;
  char * display_handle;
  char * profile;
  char * photo;
  char * quote;
  char * joke;
  char * prefs;
  int last_browsed;
  int last_active;
  int last_logon;
  int core_symbol;
  int sect_symbol;
  int cult_symbol;
  int wealth;
  int education;
  int rating;
  int rating_boost;
  int rating_count;
  int host_rating;
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
  int grating_total;
  int grating_count;
  int prating_total;
  int prating_count;
  bool valid;
};

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
};

struct chatroom_type
{
  int vnum;
  char * name;
  char * description;
  char * history;
  int last_msg;
  bool valid;
};

struct newdestiny_type
{
char * author;
int vnum;
char * role_one_name;
char * role_two_name;
int arch_one;
int arch_two;
int restrict_one;
int restrict_two;
char * premise;
int conclude_type;
bool isprivate;
char * scene_descs[20];
int scene_location[20];
int scene_special_one[20];
int scene_special_two[20];
int lastused;
int created_at;
bool scene_conclusion[20];
bool valid;
};

    struct destiny_type
    {
      char * author;
      int vnum;
      int type;
      int status;
      char * claimant;
      int sex;
      int violence;
      int disturbing;
      char * description;
      int include_types[20];
      char * includes[20];
      int exclude_types[20];
      char * excludes[20];
      char * teaser;
      int claim_type;
      int claim_timer;
      int anon;
      int inactive;
      char * ignore[50];
      char * watch[50];
      bool valid;
    };
    
    struct fantasy_type
    {
      char *  author;
      char *  name;
      char *  description;
      char * domain;
      char *  stats[30];
      char *  roles[20];
      int   locked;
      int   since_highlight;
      int   highlight_time;
      int   emit;
      int	porn;
      int	stupid;
      int   role_cost[20];
      int   role_stats[20][30];
      char *  role_desc[20];
      char *  stat_desc[30];
      int   stat_offense_power[30];
      int   stat_defense_power[30];
      int   statcost;
      int   startingexp;
      int   expdebuff;
      int   lastused;
      int   entrance;
      char *  elevator;
      char *  participants[200];
      int   participant_exp[200];
      int   participant_role[200];
      int   participant_stats[200][30];
      char *  participant_shorts[200];
      char *  participant_descs[200];
      char *  participant_names[200];
      char *  participant_fames[200];
      char *  participant_eq[200];
      bool  participant_blind[200];
      bool  participant_noaction[200];
      bool  participant_nomove[200];
      bool  participant_trusted[200];
      bool  participant_godmode[200];
      int   participant_inroom[200];
      char *  ban_list[100];
      char *  petitions;
      bool  active;
      bool  godmode;
      int   rooms[100];
      int   safe_room[100];
      int   exits[200];
      int   entrances[200];
      char *  exit_name[200];
      char *  exit_alias[200];
      bool  valid;
    };
    
    struct gateway_type
    {
      int havenroom;
      int worldroom;
      int world;
      int direction;
int lastused;
      bool valid;
    };
    
    struct plot_type
    {
      char *      author;
      
      char *  mastermind;
      int   visibility;
      int   type;
      int   crisis;
      int   subfactionone;
      int   subfactiontwo;
      int   seeking_type;
      char *  timezone;
      char *  territory;
      char *  storyline;
      char *  galleries[10];
      char *      participants[30];
      char *      comments[30];
      int      rating[30];
      
      char *  members;
      char *  eidolon;

      int         calendar;
      
      int   size;
      int   finale;
      
      int   time_made;
      int   sponsored;
      
      int   earnedkarma;
      
      int   karmacap;
      
      char *      title;
      
      char *      events;
      
      char *      challenge;
      
      char *      character;
      
      char *  storyrunners;
      
      char *  immortals;
      
      bool valid;
      
    };
    
    struct event_type
    {
      char *      author;
      
      int         faction;
      
      int         type;
      int   typetwo;
      char *  imprint;
      
      char *  coauthors;
      
      char *  researched;
      
      char *      target;
      
      int   limited;
      int   thwart_attempted;
      int   nothwart;
      int         active_time;
      int   deactive_time;
      int   shown_news;
      char *  introduction;
      char *  description;
      char *  thwart_method;
      char *  message;
      
      char *  account;
      
      char *  storyline;
      
      int   karma_battery_author;
      int   karma_battery_storyrunner;
      
      bool        valid;
    };
    
    /*
      struct participant_type
      {
      
      CHAR_DATA *   ch;
      
      int           attack_turn;
      
      int     attack_timer;
      int     move_timer;
      int     ability_timer;
      char *    qmove;
      char *    amove;
      char *    abilmove;
      
      int                 delays[10][3];
      
      CHAR_DATA *   target;
      CHAR_DATA *   target_2;
      CHAR_DATA *   target_3;
      int     target_dam;
      int     target_dam_2;
      int     target_dam_3;
      
      SET               fightflag;
      
      bool    moving;
      int     attackdam;
      int     actiontimer;
      CHAR_DATA *   chattacking;
      int     attackdisc;
      int     shadowdisc;
      int     defensedisc;
      
      
      bool                valid;
      };
      
      struct fight_type
      {
      
      vector<PARTICIPANT_TYPE *> Participants;
      
      bool    fast;
      
      CHAR_DATA *   current;
      
      ROOM_INDEX_DATA *room;
      
      int     attacks;
      
      bool                valid;
      };
    */
    
    
    
    struct house_type
    {
      int   vnum;
      char *      house_name;
      
      int   lower_vnum;
      int   higher_vnum;
      int   transfer_point;
      int   door;
      int   price;
      int   current_price;
      int   auction_day;
      int   auction_month;
      
      int   faction;
      
      char *  last_owner;
      char *  address;
      int   security;
      int   toughness;
      int   guards;
      char *  owner;
      
      char *  bid_names[30];
      int   bid_amounts[30];
      
      char *  tenants[50];
      int   tenant_trust[50];
      
      int   blackout;
      bool        valid;
    };
    
    
    struct domain_type
    {
     int vnum;
     char * domain_of;
     char * desc;
     char * ambients;
     int archetype;
     int power;
     int exp;
     int   roomlist[250];
     int smallshrines[250];
     int medshrines[250];
     int bigshrines[250];
     bool valid;
    };
    struct prop_type
    {
      int         vnum;
      char *      propname;
      int     industry;
      int   nochange;
      bool  airfix;
      int   compromised;
      int   type;
      int   profit;
      int   creditprofit;
      int   last_encroach;
      int   bankrupt;
      int   renovate;
      int   electric;
      int   market_dir;
      int   market_room;
      int   lightcount;
      int   realhouse;
      int   minx;
      int   miny;
      int   maxx;
      int   maxy;
      int   minz;
      int   maxz;
      int   roadroom;
      int   firstroom;
      int   type_special;
      int   orientation;
      int   prop_state;
      int   autoclear;
      int   utilities;
      
      int   excluded[5];
      int   included[5];
      
      int   roomlist[200];
      
      int   sale_price;
      int         lower_vnum;
      int         higher_vnum;
      int         price;
      int         auction_day;
      int         auction_month;
      
      int         faction;
      char *      address;
      char *      owner;
      int   warded;
      int   decay;
      int   shroudshield;
      int   cloaked;
      
      int   timefrozen;
      int   timeshift;
      int   tempfrozen;
      int   tempshift;
      int   weather;
      
      int     logoffs;
      
      int     blackout;
      int     objects[200];
      char *      tenants[50];
      int         tenant_trust[50];
      
      char *  sleepers[50];
      
      char *    workers[20];
      char *    worker_position[20];
      int     worker_idle[20];
      char *    advertisement;
      bool    reclaim;
      bool        valid;
    };
    
    struct shop_type
    {
      int         vnum;
      char *      shop_name;
      
      int         lower_vnum;
      int         higher_vnum;
      int         transfer_point;
      int         door;
      int         price;
      int         current_price;
      int         auction_day;
      int         auction_month;
      
      char *      owner;
      int         owner_transfer;
      
      char *      bid_names[30];
      int         bid_amounts[30];
      
      char *      tenants[50];
      int         tenant_transfer[50];
      int         tenant_trust[50];
      bool        valid;
    };
    
    struct phonebook_type
    {
      char *  owner;
      int   number;
      int   inactivity;
      bool  valid;
    };
    
    struct institute_type
    {
      char *  name;
      int   inactivity;
      int   college_prestige;
      int   college_pending_prestige;
      int   asylum_status;
      int   asylum_basecost;
      int   asylum_extracost;
      int   age;
      int   gender;
      char * surname;
      int   college_house;
      int   dorm_room;
      int   race;
      int   modifier;
      int   clique_role;
      int   school_habit;
      int   nophone;
      char *  college_committer;
      char *  asylum_diagnoses;
      char *  college_notes;
      char *  asylum_notes;
      char *  arrest_notes;
      char *  college_pending_notes;
      char *  asylum_pending_notes;
      int   commit_date;
      char *  asylum_owner;
      int   asylum_inactive;
      int   asylum_protest_days;
      int   arrest_cost;
      int   asylum_prestige;
      int   college_rank;
      int   asylum_rank;
      int   asylum_suspended;
      int   college_suspended;
      int   college_suspend_rank;
      int   asylum_suspend_rank;
      int   college_pending_suma;
      int   college_suma;
      int   college_staff_prestige;
      int   asylum_staff_prestige;
      int   college_grade;
      int   college_group;
      int   college_power;
      int   clinic_breakout;
      int   college_immune;
      bool  valid;
    };
    
    
    struct faction_type
    {
      int   vnum;
      
      char *  name;
      char *  description;
      char *  manifesto;
      char *  missions;
      char *  history;
      char *  scenes;
      char *  ooc;
      char *  roles;
      char *  eidilon;
      int     eidilon_type;
      char *  eidilon_players;
      char *  ranks[10];
      int   enemies[10][4];
      char *  enemy_descs[10];
      int   rank_stats[10][20];
      int   outcast;
      int   faction_type;
      int   last_operation;
      int   last_intel;
      int   last_deploy;
      int   restrictions[30];
      int   axes[10];
      int   staff;
      int   cardinal;
      int   nopart;
      int   subtype;
      int   lifeearned;
      int   steal_mult;
      int   last_high_intel;
      int   weekly_resources;
      char *  reportone_title;
      char *  reportone_text;
      int   reportone_time;
      char *      reporttwo_title;
      char *      reporttwo_text;
      int         reporttwo_time;
      char *      reportthree_title;
      char *      reportthree_text;
      int         reportthree_time;
      
      int	reportone_log_sent;
      int	reporttwo_log_sent;
      int	reportthree_log_sent;

      int	reportone_plog_sent;
      int	reporttwo_plog_sent;
      int	reportthree_plog_sent;

      char * reportone_participants[50];
      char * reporttwo_participants[50];
      char * reportthree_participants[50];

      char *  report_overflow[3][10];
      
      int   event_time[20];
      int   event_log_sent[20];
      int   event_plog_sent[20];
      int   event_type[20];
      int   event_subtype[20];
      char *  event_text[20][20];
      char *  event_title[20];
      char *  event_participants[20][50];
      int   alliance;
      char *  alliance_name;
      int   soft_restrict;
      int   weekly_ops;
      int   deployed_pcs;
      int   deployed_super;
      int   deployed_nosuper;
      int   deployed_power;
      int   battle_type;
      int   last_defeated;
      char *  report;
      int   total_tracked[40];
      int   population;
      int   antagonist;
      int   adversary;
      int   battle_x;
      int   battle_y;
      int   battle_order;
      char *  battle_leader;
      char *  battle_target;
      char *  leader;
      int   leaderesteem;
      int   leaderinactivity;
      int   last_incite;
      int   can_alt;
      int   op_hour_ran[25];
      int   op_second_place[5];
      int   prison_emotes;
      
      int   alchemy;
      
      int   ritual_timer;
      char *  ritual_string;
      CHAR_DATA * ritual_orderer;
      
      
      int   update;
      int   stasis;
      int   dayresources;
      int   battlewins;
      int   closed;
      int   antag_solo_wins;
      int   antag_group_wins;

      char *  messages[20];
      int   message_timer[20];
      
      int   operation_wins;
      
      int   resource;
      int   research;
      int   battlehour;
      int   battleday;
      int   battleterritory;
      int   battleclimate;
      int   support;
      int   parent;
      int   college;
      int   type;
      int   secret_days;
      int   position_time;
      int   restrict_time;
      char *  stasis_account;
      int   stasis_time;
      int   awards;
      int   award_progress;
      char *  log[50];
      int   log_timer[50];
      int   attributes[30];
      
      int   manpower;
      int   soldiers_deploying;
      char *  soldier_name;
      char *  soldier_desc;
      int   soldier_reinforce;
      
      int   patrolling;
      int   guarding;
      
      char *  symbol;
      
      char *  exmember_names[100];
      int   exmember_inactivity[100];
      int   exmember_quit[100];
      int   exmember_loyalty[100];
      char *  member_names[100];
      int   member_esteem[100];
      int   member_inactivity[100];
      int   member_rank[100];
      int   member_highest_rank[100];
      int   member_position[100];
      int   member_suspended[100];
      int   member_pay[100];
      char *  member_rosternotes[100];
      char *  member_filenotes[100];
      int   member_tier[100];
      int   member_noleader[100];
      int   member_flags[100];
      int   member_power[100];
      
      char *  vassal_names[100];
      int   vassal_inactivity[100];
      int   vassal_tier[100];
      
      char *  kidnap_name[20];
      char *  kidnap_territory[20];
      
      int   defeated_pcs;
      
      int   crystals[5];
      
      int   shipment_date[5];
      int   shipment_destination[5];
      int   shipment_amount[5];
      int   shipment_warned[5];
      
      bool        valid;
      
    };
    
    struct cabal_type
    {
      char *      name;
      char *  description;
      char *      leader;
      int         leaderinactivity;
      char *      messages[20];
      int         message_timer[20];
      
      char *      member_names[100];
      int         member_inactivity[100];
      char *      member_votes[100];
      
      bool        valid;
      
    };
    
    struct rosterchar_type
    {
      char *  owner;
      char *  name;
      char *  lastname;
      char *  shortdesc;
      char *  description;
      int   sponsored;
      int   lastactive;
      int   claimed;
      bool valid;
    };
    
    struct lair_type
    {
      char *      victim;
      int   room;
      int   mob;
      int   population;
      int   killed;
      int   abduct_date;
      int   eat_date;
      bool        valid;
      
    };
    
    struct archive_entry
    {
      long  number;
      char *  title;
      char *  owner;
      char *  text;
      bool        valid;
      
    };
    
    struct archive_index
    {
      long         number;
      char *      title;
      char *      keywords;
      bool        valid;
    };
    
    struct page_type
    {
      int   createdate;
      int   number;
      int   world;
      char *  text;
      char *  author;
      bool  valid;
    };
    struct gallery_type
    {
      char *  name;
      char *  territory;
      int   subfaction;
      char *  description;
      char *  timeline;
      char *  owner;
      char *  account_owner;
      int   xp;
      char *  coauthors;
      int   editable;
      int   lastmodified;
      bool  valid;
    };
    
    struct storyline_type
    {
      char *  name;
      char *  owner;
      char *  coauthors;
      char *  description;
      char *  timeline;
      char *  participants[100];
      int   power;
      int	total_power;
      char *  masterminds[10];
      int   mastermind_power[10];
      int   lastmodified;
      bool  valid;
    };
    
    struct grave_type
    {
      char *      name;
      char *      description;
      int     type;
      int     room;
      int       lastvisit;
      
      bool        valid;
      
    };
    
    struct clan_type
    {
      char *  clan_desc;
      
      char *  clan_name;
      
      char *  clan_hist;
      
      int   type;
      
      char *  member_names[100];
      int   member_vlast[100];
      int   member_last[100];
      int   member_this[100];
      int   member_join[100];
      char *  member_voting[100];
      
      char *  leader;
      char *  leader_voting;
      long  total;
      
      int   member_divisions[100];
      char *  division_leaders[5];
      int   oak;
      int   stone;
      int   glade;
      int   well;
      
      int   bonus;
      int   penalty;
      bool  valid;
      
    };
    
    struct level_mobs
    {
      int min_level;
      
      int chance;
      
      int vnum;
      
      int area;
    };
    
    /**
      * @struct item_type structs.h
      *
      * Contains information on the different item types
      * eg. ITEM_WEAPON, ITEM_ARMOR, ITEM_GEM, etc
    */
    struct item_type
    {
      /** Number assigned to the item type (see const.h) */
      int   type;
      
      /** name of this item type */
      char *  name;
    };
    
    /**
      * @struct trap_type structs.h
      *
    */
    struct trap_type
    {
      char *  name;
      sh_int  *trap_gsn;
      int   trap_affect;
    };
    
    /**
      * @struct wiznet_type structs.h
      *
      * Contains information on the different wiznet flag types
    */
    struct wiznet_type
    {
      /** Name of the wiznet type */
      char *  name;
      
      /** Bit flag to set for this wiznet type */
      long  flag;
      
      /** Level required to use this wiznet type */
      int   level;
    };
    
    /**
      * @struct spec_type structs.h
      *
      * Contains information on special procedures that can
      * be executed by mobs.
      * Eg. spec_guard, spec_janitor, etc
    */
    struct spec_type
    {
      /** Name of the special function */
      char *  name;
      
      /** Function to execute for this special function */
      SPEC_FUN *  function;
      
      /** The type of spec this can be used for... mob or obj or both */
      long type;
    };
    
    /**
      * @struct thing_data structs.h
      *
      *   This is a small structure to give some re-usability in code. Basically,
      *   I am emulating a C++ base class. I am not sure if this is the perfect
      *   way to do this - unions have bothered me since programming day 1. :-)
    */
    struct thing_data
    {
      /** the type of thing contained */
      int thing_type;
      
      /**
        * @union thing
        *
        * A pointer to the 'thing' contained in this instance
        * It can be one of the following:
        * - Pointer to a character (CHAR_DATA *)
        * - Pointer to an object   (OBJ_DATA *)
        * - Pointer to a room    (ROOM_INDEX_DATA *)
        * - Pointer to an area     (AREA_DATA *)
        * - Typeless       (void *)
      */
      union
      {
        CHAR_DATA *ch;
        OBJ_DATA  *obj;
        ROOM_INDEX_DATA *room;
        AREA_DATA *area;
        void    *typeless;
      } thing;
    };
    
    /**
      * @struct affect_data structs.h
      *
      * An affect.  An affect can be added to the affects
      * list of a character, room or exit.
    */
    struct affect_data
    {
      /** Pointer to next affect in the list */
      AFFECT_DATA * next;
      
      /**
        * Pointer to the affect data on the reverse
        * exit.  USed for exit affects
      */
      AFFECT_DATA * rev;
      
      /** Indicates if this affect has been validated */
      bool    valid;
      
      /** Indicates if the weave has been woven to make it stronger */
      bool    woven;
      
      /** Where to add affects.  Usually TO_AFFECTS */
      sh_int    where;
      
      /** The type of affect */
      sh_int    type;
      
      /** Level of the affect */
      sh_int    level;
      
      /** How long the affect will last */
      int   duration;
      
      /** What stat the affect changes
        * Eg, APPLY_AC, APPLY_HITROLL, etc
      */
      sh_int    location;
      
      /**
        * Amount by which it modifies the stat
        * indicated by 'location'
      */
      sh_int    modifier;
      
      /** Bitvector of the affect.  Added to affected_by */
      int     bitvector;
      
      /**
        * Indicates if the weave has been inverted to
        * make it invisible except to the caster
      */
      bool    inverted;
      
      /**
        * Indicates if the weave is being held or if
        * it has been tied off.
        * TRUE = held.  FALSE = tied
      */
      bool    held;
      
      /** Pointer to the caster, if affect is a weave */
      CHAR_DATA * caster;
      
      /** Indicatesif the affect is due to a weave or other */
      bool    weave;
    };
    
    
    /**
      * @struct group_index_data
      *
      * Prototype for a group of mobiles.
      * These include a range of 1-6 mobs.  These mobs must all be
      * located within a room for the group to work properly.
    */
    struct group_index_data
    {
      
      GROUP_INDEX_DATA *  next;
      
      /* Vnum of group. */
      int   vnum;
      
      /* Who is the selected member? - Vnum of a mobile. */
      int   member_vnum[6];
      
      /* What is their associated battle position? */
      sh_int    member_position[6];
      
      sh_int    member_bonus[6];
      
    };
    
    
    /**
      * @struct mob_index_data
      *
      * Prototype for a mob.
      * This is the in-memory version of #MOBILES.  When a new instance
      * of a mob is created, this is the prototype that it uses for all the
      * default values.
    */
    struct  mob_index_data
    {
      /** Pointer to next mob_index in list */
      MOB_INDEX_DATA *  next;
      
      /** Pointer to any special prgrams the mob has */
      SPEC_FUN *    spec_fun;
      
      /** Pointer to any shopkeeper data the mob has */
      SHOP_DATA *   pShop;
      
      /** Pointer to the list of mprogs for the mob */
      PROG_LIST *        mprogs;
      
      /** Pointer to the area to which the mob belongs. OLC */
      AREA_DATA *   area;
      
      /** Vnum of the mob */
      int   vnum;
      
      /** Group the mob belongs to, used for old stock spec progs */
      int   group;
      
      /** Count of the number of the mob loaded in the game */
      sh_int    count;
      
      /** Keywords for the mob */
      char *    player_name;
      
      /** The short description of the mob.  Displayed in combat, etc */
      char *    short_descr;
      
      /** The long description of the mob.  Displayed in rooms, etc */
      char *    long_descr;
      
      /** Description of the mob that is displayed when using 'look mob' */
      char *    description;
      
      /** The ACT bits of the mob. See const.h (ACT_*) */
      SET     act;
      
      /** Bits the mob is affected by */
      SET     affected_by;
      
      /** Default level of the mob */
      sh_int    level;
      
      /** Offensive flags of the mob.  See const.h (OFF_*) */
      long    off_flags;
      
      /** Pointer to a targeting room **/
      ROOM_INDEX_DATA * targeting[10];
      
      /** Position the mob starts in when loaded */
      sh_int    start_pos;
      
      /** The position the mob start in when loaded */
      sh_int    default_pos;
      
      /** Sex of the mob. SEX_MALE or SEX_FEMALE */
      sh_int    sex;
      
      /** Race of the mob */
      sh_int    race;
      
      /** How much silver/gold the mob has when laoded */
      long    wealth;
      
      /**
        * Bitvector describing default form of the mob
        * See const.h (FORM_*)
      */
      long    form;
      
      /**
        * Bitvector describing default form of the mob
        * See const.h (PART_*)
      */
      long    parts;
      
      /** The material the mob is made of? */
      char *    material;
      
      /** Flags the mob triggers on for mprogs */
      long    mprog_flags;
      
      int     ttl;
      
      int         disciplines[MAX_DIS];
      
      
      int     weapon_type;
      int     intelligence;
      int     maxwounds;
      
      
    };
    
    /**
      * @struct char_data structs.h
      *
      * One character (PC or NPC).
    */
    struct  char_data
    {
      /** Points to next character observing a room */
      CHAR_DATA *   next_observing_room;
      
      /** Pointer to the char's master when charmed */
      CHAR_DATA *   master;
      
      /** POinter to the char this char will reply to */
      CHAR_DATA *   reply;
      int     reply_type;
      
      CHAR_DATA *   controled_by;
      
      CHAR_DATA *   possessing;
      
      /** The default prototype for the char.  NPC only */
      MOB_INDEX_DATA *  pIndexData;
      
      /** Descriptor of this char's connection. PC only */
      DESCRIPTOR_DATA * desc;
      
      /** List of affects on the char */
      AFFECT_DATA * affected;
      
      /* Pointer to the start of the list of the char's posessions */
      OBJ_DATA *    carrying;
      
      /** Pointer to whatever the char is sitting/resting/sleeping on */
      OBJ_DATA *    on;
      
      /** Pointer to the room the char is in */
      ROOM_INDEX_DATA * in_room;
      
      ROOM_INDEX_DATA * destination;
      
      /** Pointer to the area the char a part of */
      AREA_DATA *   zone;
      
      /** Pointer to the char's PC-only information.  PC only(obviously :>). */
      PC_DATA *   pcdata;
      
      /** Indicates if the char instance  has been validated */
      bool    valid;
      
      /** The name of the char */
      char *    name;
      
      /** The unique ID of the char. PC only, I believe */
      long    id;
      
      
      /** The version of the char's data.  Used for saving proper info */
      sh_int    version;
      
      /** The short description of the char.  Displayed in combat, etc */
      char *    short_descr;
      
      /** The long description of the char.  Displayed in rooms, etc */
      char *    long_descr;
      
      /** Description of the mob that is displayed when using 'look char' */
      char *    description;
      
      /** The prompt used by this char */
      char *    prompt;
      
      char *    last_ip;
      
      /** */
      char *    prefix;
      
      /** Sex of the char. SEX_MALE or SEX_FEMALE */
      sh_int    sex;
      
      /** the race of the char */
      sh_int    race;
      
      sh_int    level;
      
      int     lifeforce;
      int     lf_used;
      int     lf_taken;
      int     lf_sused;
      
      int     modifier;
      /** The trust of the char.  This can be used to give access to
        * commands that can only be used at a level higher than that
        * of the char
      */
      sh_int    trust;
      
      bool                fight_fast;
      CHAR_DATA *         fight_current;
      CHAR_DATA *   fight_next;
      int                 fight_attacks;
      int                 fight_speed;
      bool    hadturn;
      
      /** How many hours the char has played */
      int     played;
      int     walking;
      int     car_inroom;
      
      int     playedcopfree;
      
      /** Number of lines per page.  Used by the pager */
      int     lines;
      
      int     linewidth;
      
      /** The time that the char logged on */
      time_t    logon;
      
      /** Time of last input, used to calculate idle time */
      time_t              idle;
      
      /** Count of ticks since last input */
      sh_int    timer;
      
      /** How many pulses the char must wait before new input is processed */
      sh_int    wait;
      
      /** Current hitpoints for the char */
      int     hit;
      
      /** How much of each coin someone has in their posession **/
      long    money;
      
      /** The amount of experience points the char has earned */
      
      int     recent_moved;
      
      int     exp;
      int     rpexp;
      int     spentexp;
      int     spentrpexp;
      int     privaterpexp;                   //Record of rpexp gained while in private mode - Discordance
      int     privatekarma;                   //Record of karma gained while in private mode - Discordance
      
      int     karma;
      int     spentkarma;
      int     pkarma;
      int     spentpkarma;
      
      int     newexp;
      int     spentnewexp;
      int     newrpexp;
      int     spentnewrpexp;
      
      int     positive_magic;
      int     negative_magic;
      int     command_force;
      bool	  doneabil;
      
      int     order;
      char *    ordertarget;
      
      bool    preyvictim;

      int     mapcount;
      int     abomination;
      int     esteem_faction;
      int     esteem_cult;
      int     esteem_sect;
      int     faction;
      int     factiontwo;
      int     factiontrue;
      int     mob_ai;
      int     fcore;
      int     fcult;
      int     fsect;
      int     deploy_core;
      int     deploy_cult;
      int     deploy_sect;
      int     esteemtwo;
      int     vassal;
      int     oppress;
      
      int     disx;
      int     disy;
      
      int     ttl;
      bool    killed;
      
      int     donated;
      
      
      char *    aggression;
      char *    protecting;
      
      
      int     x;
      int     y;
      
      char *              clan;
      
      int     lastlogoff;
      
      int   shape;
      int   lastshape;
      int   bagcarrier;
      
      int   skills[SKILL_MAX];
      int   wilds_skills[SKILL_MAX];
      int   other_skills[SKILL_MAX];
      int   godrealm_skills[SKILL_MAX];
      int   hell_skills[SKILL_MAX];
      
      int   disciplines[MAX_DIS];
      
      int   attacking;
      int   facing;

      int   hits_taken;
      
      bool  valuable;
      
      int   activeat;
      int     wounds;
      int     heal_timer;
      int     death_timer;
      
      int     specialpoint;
      
      int   lastdisc;
      int   diminished;
      int   diminish_offervalue;
      int   rounddiminish;
      int   diminish_lt;
      int   diminish_till;
      int   diminish_vis;
      
      CHAR_DATA *diminish_offer;
      
      CHAR_DATA *last_hit_by;
      int   last_hit_damage;
      
      CHAR_DATA *dragging;
      CHAR_DATA *cfighting;
      CHAR_DATA *lifting;
      CHAR_DATA *dream_dragging;
      CHAR_DATA *delay_target;
      CHAR_DATA *afraid_of;
      CHAR_DATA *underpowering;
      CHAR_DATA *mimic;
      CHAR_DATA *cloaked;
      CHAR_DATA *reflect;
      CHAR_DATA *commanded;
      
      CHAR_DATA *your_car;
      
      CHAR_DATA *fistfighting;
      int   endurance;
      int   fistattack;
      int   guard;
      int   fisttimer;
      
      int                 attack_turn;
      
      int                 attack_timer;
      int                 move_timer;
      int     ability_timer;
      int     ability_dead;
      int     round_timer;
      int     debuff;
      int     to_debuff;
      int     damage_absorb;
      
      char *              qmove;
      char *              amove;
      char *    abilmove;
      
      int                 delays[10][3];
      
      CHAR_DATA *         target;
      CHAR_DATA *         target_2;
      CHAR_DATA *         target_3;
      int                 target_dam;
      int                 target_dam_2;
      int                 target_dam_3;
      
      int     caff[30];
      int     caff_duration[30];
      
      SET               fightflag;
      bool                abildone;
      bool                moving;
      int                 attackdam;
      int     knockfails;
      int                 actiontimer;
      int		  tsincemove;
      CHAR_DATA *         chattacking;
      int                 attackdisc;
      int     shadowdisc;
      int                 defensedisc;
      
      bool    fighting;
      bool    in_fight;
      
      int     moved;
      int     run_dir;
      int     wound_location;
      
      /** The ACT bits set for the char. See const.h (ACT_*) */
      SET     act;
      
      SET     legacy;
      
      /**
        * Communication bits the char has set. The SET
        * is an array of longs.  This allows for unlimited bits.
        * See const.h (COMM_*)
      */
      SET     comm;
      
      /**
        * Bitvector containing the wiznet channels the char wishes
        * to monitor
      */
      long    wiznet; /* wiz stuff */
      
      /** Immunity flags of the char.  See const.h (IMM_*) */
      long    imm_flags;
      
      /** Level the char is invis to.  Used for different wizinvis levels */
      sh_int    invis_level;
      
      /**
        * Level the char is incognito.  Chars above this level and in the
        * same room will see the char
      */
      sh_int    incog_level;
      
      /**
        * Affect bits the char has set. The SET
        * is an array of longs.  This allows for unlimited bits.
        * See const.h (AFF_*)
      */
      SET     affected_by;
      
      /** Current position the char is in. Eg standing, sitting, etc */
      sh_int    position;
      
      /** Number of objects being carrie dby the char */
      sh_int    carry_number;
      
      /**
        * Bitvector describing the form of the char
        * See const.h (FORM_*)
      */
      long    form;
      
      /** Position the char starts in when loaded */
      sh_int    start_pos;
      
      /** The position the char starts in when loaded */
      sh_int    default_pos;
      
      
      /* Sector type this mob is restricted to */
      sh_int    sector;
      NameMap      *trusts;
      
      
    };
    
    /**
      * @struct char_mem_data structs.h
      *
      * Contains the id of a character that is being
      * remembered by a mob for future reference.
    */
    struct char_mem_data
    {
      /** Pointer to the next mem_data in the list */
      MEM_DATA    *next;
      
      /** When the mob last encountered the character being remembered */
      time_t    when;
      
      /** ID of the char being remembered */
      long    id;
      
      /** The action(s) the mob will take when it encounts the char being remembered */
      long    action;
    };
    
    /**
      * @struct memorize_data structs.h
      *
      * Contains the name and vnum of a room that has been
      * memorized by a character to use for Travelling and
      * Skimming weaves.
    */
    struct memorize_data
    {
      /** Pointer to the next room_mem_data in the list */
      MEMORIZE_DATA *next;
      
      /** The keyword for this memorized room */
      char    *keyword;
      
      /** The vnum of the room which has been memorized */
      int     vnum;
      
      /** Indicates if the room has been completely memorized yet */
      bool    complete;
    };
    
    #define MAX_TRUSTS 200
    
    /**
      * @struct pc_data structs.h
      *
      * Data which only PC's have.
    */
    struct  pc_data
    {
      /** Pointer to next PC in the list */
      PC_DATA *   next;
      
      ACCOUNT_TYPE *  account;
      
      /** The PC's output bufffer for replay during AFK */
      Buffer    *buffer;
      
      //COLOR_DATA  *code;
      
      /** Indicates if the pc-data has been validated */
      bool    valid;
      
      /** The PCs password */
      char *    pwd;
      
      /** The PCs password temporarily unecrypted for forum setup - Discordance */
      char *    upwd;
      
      /** String to display for poofin */
      char *    bamfin;
      
      /** String to display for poofout */
      char *    bamfout;
      
      /** IC ttle for the PC displayed to those in the room */
      char *    title;
      
      char *    account_name;
      
      /** Where the PC lives.  Used in the 'finger' command */
      char *    floc;
      
      /** Best time to reach the PC.  Used in the 'finger' command */
      char *    ftime;
      
      /** Short description to be displayed by the 'finger' command */
      char *    fdesc;
      
      /** The email address of the PC */
      char *    email;
      
      /** Title for the PC displayed in the wholist */
      char *      whotitle;
      
      /** The PC's last name */
      char *    storyrunner;
      char *    guest_of;
      char *    pledge;
      int     pledge_stealth;
      char *    last_name;
      char *    verbal_color;
      char *    remember_detail;
      char *    enthralling;
      char *    enrapturing;
      char *    enthralled;
      char *    enraptured;
      
      int     lfcount;
      int     lftotal;
      char *    privatepartner;
      
      char *    enlinked;
      int     enlinktime;
      int         height_feet;
      int         height_inches;
      
      int     paranoid;
      
      int     ftype;
      
      int	wander_time;
bool	crowded_room;
      int	last_victim_bled;
      int	last_imbue;
      char *    luck_string_one;
      char *    luck_string_two;
      int       luck_type;
      CHAR_DATA *luck_character;

      char *   attempt_emote;
      CHAR_DATA * attempt_character;
      bool    is_attempt_emote;

      int     eidilon_of;
      char *  eidilon_ambient;
      char * implant_dream;


      char *  ff_knowledge;
      char *  ff_secret;

      EVENT_TYPE *  scheme_running;
      EVENT_TYPE *  scheme_request;
      CHAR_DATA *   scheme_requester;

      CHAR_DATA *   wardrobe_pointer;
      int	wardrobe_direction;

      int     monster_fed;
      int     superlocked;
      int     translocked;
      int     medic_uses;
      int     experiment_uses;
      CHAR_DATA *   tracing;
      
      char *    brainwash_loyalty;
      char *    brainwash_reidentity;
      int     mindmessed;
      int     implant_frequency;
      int     blackeyes_power;
      int     blackeyes;
      int     apower;
      int     fleshformed;
      int     infuse_date;
      int     shroud_dragging;
      int     shroud_drag_depth;
      int     monster_beaten;
      int     egg_date;
      int     egg_daddy;
      char *    maim;
      int     process;
      int     process_timer;
      int     process_subtype;
      char *    process_argumentone;
      char *    process_argumenttwo;
      CHAR_DATA *   process_target;
      
      char *    job_title_one;
      int     job_type_one;
      int     job_delay_one;
      int     job_room_one;
      
      char *    job_title_two;
      int     job_type_two;
      int     job_delay_two;
      int     job_room_two;
      
      char *  enc_prompt_one;
      char *  enc_prompt_two;
      char *  enc_prompt_three;
      int     encounter_pnumber;
      int     vote_timer;
      int     yesvotes;
      int     novotes;
      int     tboosts;
      
      int     lastaccident;
      int     lastnormal;
      int     lastshift;
      int     lunacy_curse;
      
      int     mimic;
      int     lastidentity;
      
      int     deputy_cooldown;
      int     disperse_timer;
      int     disperse_room;
      int     movealong_timer;
      int     movealong_vnum;
      
      int     last_outside_fullmoon;
      int     last_drained_person;
      
      int     divine_focus;
      

      int     extra_focus;
      int     bonus_origin;
      int     decay_stage;
      int     decay_timer;
      int     max_decay_timer;
      
      int     blackout_cooldown;
      
      int     class_type;
      int     class_faction;
      int     class_cooldown;
      
      int     class_oldfaction;
      int     class_oldlevel;
      
      int     class_neutral;
      char *    class_fame;
      
      int     vbloodcool;
      
      int     healthtimer;
      int     hangoutone;
      int     hangouttwo;
      int     hangouttemp;
      
      char *    home_territory;
      char *    eyes;
      char *    hair;
      char *    skin;
      char *    deathcause;
      
      char *      dream_environment;
      char *      dream_intro;
      char *      dream_description;
      
      CHAR_DATA   * dream_link;
      int       dream_cooldown;
      int       dream_duration;
      int       dream_controller;
      
      int     logoff_room; //Room PC logged off in last - Discordance
      
      int     escape_timer;
      
      int     garbled;
      int     bad_abomination;
      int     lucky_abomination;
      
      char *    diss_target;
      char *    diss_message;
      int     diss_sendable;
      
      int     survey_ratings[15];
      int     survey_stage;
      int     survey_delay;
      char *    survey_comment;
      char *    survey_improve;
      char *    surveying;
      
      int     abommistcool;
      int     abomblackcool;
      
      int     breach_origin;
      int     breach_destination;
      int     breach_timer;
      
      int     maskednumber;
      
      char *    mask_intro_one;
      char *    mask_intro_two;
      
      int     eyes_genetic;
      int     hair_genetic;
      
      char *    hair_dyed;

      int     boon_history[10];
      int     curse_history[10];
      int     boon;
      int     boon_timeout;
      int     curse;
      int     curse_timeout;
      char *  curse_text;
      OBJ_DATA *boon_gift;
      
      int     garage_typeone[10];
      int     garage_typetwo[10];
      int     garage_status[10];
      int     garage_timer[10];
      int     garage_cost[10];
      int     garage_location[10];
      char *    garage_name[10];
      char *    garage_desc[10];
      char *    garage_lplate[10];
      
      int     driveloc[10];
      char *    drivenames[10];
      
      char *    contact_names[20];
      char *    contact_descs[20];
      int     contact_cooldowns[20];
      int     contact_jobs[20];
      
      int     deepshroud;
      int     classpoints;
      int     classtotal;
      
      int     aexp;
      int     wexp;
      int     oexp;
      int     gexp;
      int     hexp;
      int     dexp;
      int     awins;
      int     alosses;
      int     astatus;
      
      char *    editing_territory;
      
      int     racial_power;
      int     racial_power_two;
      
      int     payscale;
      int     bond_drop;
      int     blood_date;
      int     blood_level;
      char *    bonds[5];
      CHAR_DATA *   bond_offer;
      int     vbond;
      
      CHAR_DATA *   offering;
      int     offer_type;
      
      char *    relationship[10];
      int     relationship_type[10];
      
      int     packcooldown;
      int     teachercooldown;
      
      
      char *    understanding;
      int     understandtoggle;
      
      int     lured_room;
      
      int     blood_potency;
      int     blood_ticks;
      
      int     public_alarm;
      int     last_drove;
      int     free_ritual;
      
      int     true_id;
      

      int     rpainamount;
      int     icooldown;
      int     tcooldown;
      int     timesincedamage;
      
      int     aiding_scheme;
      char *    aiding_thwart;
      int     scheme_influence;
      int     faction_influence;
      
      int     proxies;
      int     influence_balance;
      char *    last_praised[3];
      char *    last_dissed[3];
      char *    outfit_nicknames[20];
      int     outfit_nickpoint[20];
      CLAN_TYPE *   clan_offering;
      int     clan_position;
      int     clan_bonus;
      
      int     dirt[MAX_COVERS];
      int     blood[MAX_COVERS];
      
      int     exposed[MAX_COVERS];
      
      int     last_page;
      int     resident;
      int     last_inhaven;
      
      int     sins[7];
      int     known_gateways[50];
      
      int     heldblood[6];
      
      int     association;
      int     adventure_fatigue;
      
      char *    fame;
      char *    art1;
      char *    art2;
      char *    art3;
      
      char *    mentor;
      int     atolerance;
      
      int                 event_cooldown;
      EVENT_TYPE *        making_event;
      
      int     secret_days;
      int   secret_recover;
      int	intel;
      int	last_intel;
      int     spent_resources;
      int     stasis_time;
      int	op_emotes;
      int	pending_resources;
      int	in_domain;
      int	recent_exp;
      int	destiny_reject;
      int	destiny_reject_cooldown;
      int	bloodstorm_resist;
      int	domain_timer;
      int	summon_bound;
      
      int     gen_stage;
      
      int     market_visit;
      int     time_since_emote;
      int     time_since_action;
      int     institute_action;
      int     tier_raised;
      int     detention_time;
      int     hell_power;
      int     godrealm_power;
      int     other_power;
      int     timenotalone;
      int     lastnotalone;
      int     overworked;
      int     weakness_status;
      int     weakness_timer;
      int     victimize_difficult_time;
      int     victimize_history[20];
      CHAR_DATA *   victimize_char_point;
      CHAR_DATA *   victimize_vic_point;
      CHAR_DATA *   prep_target;
      int	destiny_offer;
      int 	destiny_offer_role;
      int	scene_offer;
      int	scene_offer_role;
      int	scene_vote;

      CHAR_DATA * destiny_offer_char;
      int     narrative_give[50];
      int     narrative_want[50];
      int     prep_action;
      int     victimize_vic_timer;
      int     victimize_char_select;
      int     victimize_vic_response_to;
      int     victimize_vic_select;
      int     victimize_difficult_count;
      int     victimize_vic_pending;
      int     victimize_lf_pending_loss;
      int     victimize_pending_lf_gain;
      int     victimize_last_response;
      int     stasis_spent_pkarma;
      int     stasis_spent_exp;
      int     stasis_spent_rpexp;
      int     stasis_spent_karma;
      int     last_dreamworld;
      int     dream_timer;
      char *    ritual_dreamworld;
      char *    dream_identity;
      char *    identity_world;
      int     dream_identity_timer;
      int     patrol_rp;
      bool    combat_emoted;
      int     deluded_cost;
      int     deluded_duration;
      char *   last_villain_mod;
      char *    deluded_reason;
      char *    rp_log[20];
      int     nightmare_shifted;
      int     dreamoutfit;
      int     dreamfromoutfit;
      int     rp_logging;
      int     nightmare_dragged;
      int     last_rp_room;
      int     availability;
      int	destiny_feature;
      int	destiny_feature_room;
      int     destiny_lockout_time[10];
      char *    destiny_lockout_char[10];
      int	destiny_interest_one[50];
      int	destiny_block_one[50];
      int	destiny_interest_two[50];
      int	destiny_block_two[50];
      int	destiny_in_one[10];
      int	destiny_in_two[10];
      int       institute_contraband_violations;

      char *	destiny_in_with_one[10];
      char *	destiny_in_with_two[10];
      int	destiny_stage_one[10];
      int	destiny_stage_two[10];

      int	victim_points;
      int     watching;
      int     truepenis;
      int     heroic;
      int     truebreasts;
      char *    last_ritual;
      int     sexchange_time;
      int     maintained_ritual;
      char *    ritual_maintainer;
      char *    maintained_target;
      int     maintain_cost;
      int     penis;
      int     mentor_of;
      int     bust;
      int     solitary_time;
      
      int     psychic_attempts;
      int     psychic_proof;
      
      int     sleeping;
      int     desclock;
      
      int     ambush;
      
      int     police_catch_timer;
      int     earned_karma;
      
      int     captive_timer;
      int     poisontimer;
      int     rohyptimer;
      
      int     relapsewerewolf;
      int     relapsevampire;
      
      int     last_paid;
      int     became_tyrant;
      
      int     wetness;
      
      char *    place;
      
      char *    messages;
      
      char *    pubic_hair;
      int     travel_to;
      int     travel_type;
      int     travel_time;
      int     travel_from;
      int     travel_slow;
      
      int     boobjob;
      
      int     neutralized;
      
      int     selfbondage;
      int     deploy_from;
      int     move_damage;
      
      int     fall_timer;
      
      char *    last_emote;
      char *    last_newbie;
      
      char *    nightmare;
      
      char *    focused_descs[MAX_COVERS+10];
      
      char *      detail_over[MAX_COVERS+10];
      char *      detail_under[MAX_COVERS+10];
      
      char *    scars[MAX_COVERS+10];
      
      int     focused_order[MAX_COVERS+10];
      
      int     bloodaura;
      
      char *    pinned_by;
      
      int     extra_powers;
      int     energy_recovery;
      
      int     time_since_train;
      
      int     exp_cap;
      int     rpexp_cap;


      float     attract_count;
      float     attract_count_fashion;
      float     attract_count_phys;
      float     attract_count_skin;
            
      int     perfume_cost;
      
      char *    scent;
      char *    taste;
      
      char *    imprint[25];
      int     imprint_pressure_one[25];
      int     imprint_pressure_two[25];
      int     imprint_pressure_three[25];
      int     imprint_type[25];
      char *    imprint_trigger[25];
      int     imprint_pending[25];
      int     imprint_resist_cost;
      CHAR_DATA * imprinter;
      int     trance;
      
      int     mindbroken;
      
      CHAR_DATA * persuade_target;
      int     persuade_cooldown;
      char *    persuade_message;
      int     persuade_pone;
      int     persuade_ptwo;
      int     persuade_pthree;
      
      
      int     training_stat;
      int     training_disc;
      
      bool    patrol_did_arcane;
      bool    patrol_did_hunting;
      bool    patrol_did_diplomatic;
      bool    patrol_did_war;
      
      char *    villain_praise;
      int     villain_type;
      char *    villain_message;
      
      int     training_stage;
      int     training_type;
      int     deactivated_stats[10];
      int     stat_log_stat[300];
      int     stat_log_from[300];
      int     stat_log_to[300];
      int     stat_log_cost[300];
      int     stat_log_method[300];
      char *    stat_log_string[300];
      
      int     patrol_habits[10];
      int     patrol_status;
      int     patrol_timer;
      int     patrol_amount;
      int     patrol_pledged;
      ROOM_INDEX_DATA * patrol_room;
      CHAR_DATA *     patrol_target;
      ROOM_INDEX_DATA * last_patrol;
      int     patrol_subtype;

      int     lf_modifier;
      int     spawned_monsters;
      bool    travel_prepped;
      int     dream_sex;
      int     note_gain;
      int     dream_room;
      int     dream_invite;
      int     tempdreamgodworld;
      int     tempdreamgodchar;
      int     dream_attack_cool;
      int     last_develop_type;
      int     last_develop_time;
      int     doom_date;
      int     doom_countdown;
      char *  doom_desc;
      char *  doom_custom;
      int     previous_focus_vnum[25];
      int     previous_focus_level[25];
      int     form_change_date;
      int     lingering_sanc;
      int     prison_emotes;
      char *    roster_description;
      char *    roster_shortdesc;
      char *    brander;
      char *    brandstring;
      char *    last_bitten;
      int     ill_count;
      int     ill_time;
      int     mind_guarded;
      int     mind_guard_mult;
      int     branddate;
      int     brandlocation;
      int     brandstatus;
      int     default_speed;
      int     brandcool;
      char *    demon_pact;
      
      char *    dreamplace;
      char *    dreamtitle;
      
      bool    sniffing;
      
      int     bittenloss;
      int     monster_wounds;
      int     rerollselftime;
      int     sr_nomove;
      int     factionwin;
      int     territory_editing[15];
      int     week_tracker[40];
      int     life_tracker[40];
      char *    last_praise_message[3];
      char *    last_diss_message[3];
      char *    last_rumor;
      
      bool    driving_around;
      int     legendary_cool;
      int     wilds_fame_level;
      char *    wilds_fame;
      int     wilds_legendary;
      int         other_fame_level;
      char *      other_fame;
      int         other_legendary;
      int         godrealm_fame_level;
      char *      godrealm_fame;
      int         godrealm_legendary;
      int         hell_fame_level;
      char *      hell_fame;
      int         hell_legendary;
      
      
      
      int     offworld_protection;
      
      int     spirit_type;
      int     next_henchman;
      int     next_enemy;
      int     next_monster;
      
      int     prison_care;
      int     prison_mult;
      
      char *    finale_location;
      int     finale_timer;
      
      int     guest_type;
      int     guest_tier;
      char *    dream_origin;
      char *    speaking;
      
      char *    memories[25];
      char *    repressions[25];
      
      char *    murder_name;
      int     murder_timer;
      int     murder_cooldown;
      
      bool    encounter_storyline_added;
      bool    encounter_mastermind_added;
      char *    encounter_storyline;
      char *    encounter_bringin;
      CHAR_DATA * encounter_sr;
      int     encounter_status;
      int     encounter_countdown;
      int     encounter_orig_room;
      int     encounter_number;
      int     karma_battery;
      int     pathtotal;
      int     genesis_stage;
      
      int         augdisc_timer[10];
      int         augdisc_disc[10];
      int         augdisc_level[10];
      
      int         augskill_timer[10];
      int         augskill_skill[10];
      int         augskill_level[10];
      
      int     mark_timer[4];
      char *    mark[4];
      
      int     ability_trees[5];
      int     abilcools[20];
      int     walking;
      
      int     haunt_timer;
      char *    haunter;
      
      int     order_type;
      int     order_timer;
      char *    order_target;
      int     order_amount;
      
      int     guard_number;
      int     guard_faction;
      int     guard_expiration;
      
      int     tail_mask;
      int     tail_timer;
      char *    tailing;
      
      int     last_public_room;
      
      int     base_standing;
      int     social_praise;
      int     social_behave;
      int     influence_bank;
      int     influence;
      int     attract_base;
      int     attract_mod;
      
      int     attract[20];
      int     vamp_fullness;
      int     vomit_timer;
      
      /** The PC's history */
      
      char *    history;
      char *    char_goals;
      char *    char_fears;
      char *    char_secrets;
      char *    char_timeline;
      char *    text_msgs;
    char *              photo_msgs;
      char * 	photo_pose;

      char *    file;
      char *    classified;
      
      int     dtrains;
      int     strains;
      
      int     timeswept;
      int     deaged;
      int     agemod;
      
      int         speed_numbers[50];
      char *      speed_names[50];
      
      int     file_factions[50];
      char *    file_message[50];
      
      char *      ignored_characters[50];
      char *      ignored_accounts[50];
      char *      nowhere_characters[50];
      char *      nowhere_accounts[50];
      int         nowhere_male;
      int         nowhere_female;
      
      int         chan_numbers[10];
      int     chan_status[10];
      char *      chan_names[10];
      
      char *    attention_history[10];
      
      
      char *    feedback;
      int     feedbackcooldown;
      
      char *    feedbackinc[10];
      int     feedbacktim[10];
      int     feedbackbon[10];
      
      int     xpbonus;
      int     rpxpbonus;
      
      int     ally_limit;
      int     minion_limit;
      
      int     lifeforcecooldown;
      
      int     timebanished;
      int     create_date;
      int     beacon;
      int     last_share;
      
      int     stories_run;
      int     stories_countdown;
      int     rooms_run;
      int     rooms_countdown;
      
      int     sincool;
      
      int     youbadcounter;
      
      int     cop_cooldown;
      
      int     currentmonster;
      
      char *    monster_names[3][5];
      int     monster_discs[10][5];
      int         trust_levels[MAX_TRUSTS];
      char *      trust_names[MAX_TRUSTS];
      
      int     faction;
      
      char *    languages[5];
      int     portaled_from;
      int     customstats[25][2];
      char *    customstrings[25][7];
      int     specialcools[20][4];
      int     specials[25][2];
      
      int     delays[10][3];
      
      char *    talk;
      
      
      int     story_con[10][3];
      
      int     lastwarcheck;
      
      int     police_timer;
      int     police_intensity;
      int     police_number;
      
      int     tresspassing;
      
      int     earnt_xp;
      
      int     rank_type;
      
      int     version_player;
      
      //Community credit rewards
      int     renames;
      int     shadow_attacks[20][2];
      int     colours;
      int     bandaids;
      char *    caura;
      char *    cwalk;
      char *    crank;
      
      int     ci_editing;
      
      int     ci_vnum;
      int     ci_type;
      int     ci_cost;
      int     ci_size;
      char *    ci_name;
      char *    ci_short;
      char *    ci_long;
      char *    ci_taste;
      char *    ci_imprint;
      int     ci_covers;
      int     ci_zips;
      char *    ci_wear;
      char *    ci_desc;
      
      int     ci_mod;
      int     ci_layer;
      int     ci_alcohol;
      
      char *    ci_myself;
      char *    ci_target;
      char *    ci_bystanders;
      char *    ci_myselfdelayed;
      char *    ci_targetdelayed;
      char *    ci_bystandersdelayed;
      
      char *    ci_message;
      
      int     ci_exclude_type[20];
      char *    ci_excludes[20];
      int         ci_include_type[20];
      char *      ci_includes[20];
      
      int     ci_discipline;
      int     ci_disclevel;
      int         ci_discipline2;
      int         ci_disclevel2;
      int         ci_discipline3;
      int         ci_disclevel3;
      int         ci_discipline4;
      int         ci_disclevel4;
      int         ci_discipline5;
      int         ci_disclevel5;
      
      int     ci_special;
      int     selfesteem;
      
      //Role variables  - Discordance
      int         role;
      int     job;
      
      int     habit[30];
      int     habits_done;
      
      int     litup;
      int     autoskip;
      
      int     ci_x;
      int         ci_y;
      int         ci_area;
      int         ci_status;
      
      int     ci_absorb;
      
      int     ci_stats[10];
      
      /** The amount of a coin each person has in the bank **/
      long    total_money;
      int     total_credit;
      
      /** */
      int     penalty;
      int     difficulty;
      int     fatigue;
      
      int     fatigue_temp;
      
      int     storycon[10][3];
      
      /** Timezone offset used for the time command */
      int     jetlag;
      
      int     conditions[10];
      
      /** Keeps track of maxAbility variable sizes */
      int     iMemSize[1];
      
      /** The PC's true level.  Used for the 'avator' command */
      sh_int    true_level;
      
      /**
        * Indicates if the PC has used the delete command once already
        * Used to prompt to make sure they want to do this
      */
      bool                confirm_delete;
      
      int     mentor_timer;
      
      char *    alias[MAX_ALIAS];
      
      /** Used when doing alias command substitution */
      char *    alias_sub[MAX_ALIAS];
      
      /** Builder security of the PC to limit access to areas. OLC */
      int     security;
      
      /** Time duration the PC has been squished for */
      long    squish;
      
      /*
        * Color data stuff for config.
      */
      int     text[3];    /* {t */
      int     auction[3];   /* {n */
      int     irl[3];     /* {j */
      int     immortal[3];    /* {k */
      int     implementor[3];   /* {l */
      int     newbie[3];    /* {e */
      int     ooc[3];     /* {f */
      int     yells[3];   /* {h */
      int     osay[3];    /* {p */
      int     gossip[3];    /* {a */
      int     info[3];    /* {j */
      int     say[3];     /* {o */
      int     tells[3];   /* {i */
      int     reply[3];   /* {i */
      int     gtell[3];   /* `A */
      int     wiznet[3];    /* {Z */
      int     room_exits[3];    /* {q */
      int     room_things[3];   /* {Q */
      int     prompt[3];    /* {p */
      int     fight_death[3];   /* {V */
      int     fight_yhit[3];    /* {z */
      int     fight_ohit[3];    /* {u */
      int     fight_thit[3];    /* {U */
      int     fight_skill[3];   /* {v */
      int     hero[3];    /* {s */
      int     hint[3];    /* {t */
      int     minioncolor[3];   /* {T */
      int     pray[3];    /* {E */
      
      
      
      /** the characters birth details */
      sh_int    birth_day;
      sh_int    birth_month;
      sh_int    birth_year;
      
      
      sh_int      sire_day;
      sh_int      sire_month;
      int         sire_year;
      int     apparant_age;
      
      CHAR_DATA * protecting;
      int photo_attract;
      char * private_security;
      int active_chatroom;
      int in_chatroom[50];
      CHAR_DATA * cam_spy_char;
      CHAR_DATA * narrative_query_char;
      bool    is_target_encounter;

      int quit_room;
      int	ritual_prey_timer;
      char *    ritual_prey_target;
      int	missed_chat_connections;
      int	missed_rp_connections;
      int	influencer_bank;
      int	suspend_myhaven;
      int	page_timeout;

      CHAR_DATA * summary_target;
      char * summary_name;
      char * summary_intro;
      char * summary_content;
      char * summary_conclusion;
      int summary_when;
      int summary_type;
      int summary_stage;
      int summary_blood;
      int summary_intel;
      int summary_lifeforce;
      int summary_helpless;
      int summary_home;


      int operative_creation_type;
      char * operative_core;
      char * operative_sect;
      char * operative_cult;
      
      /* Shapeshifting Variables                          - Discordance                   */
      char *    wolfdesc;
      char *    wolfintro;
      char *    fishmanintro;
      char *    mermaidintro;
      char *    mermaiddesc;
      char *    mermaid_change_to;
      char *    mermaid_change_from;
      char *    wolf_change_to;
      char *    wolf_change_from;
      char *    radio_action;
      char *    makeup_light;
      char *    makeup_medium;
      char *    makeup_heavy;

      char *    animal_change_to[6];
      char *    animal_change_from[6];
      char *    animal_names[6];
      char *    animal_intros[6];
      char *    animal_descs[6];
      char *    animal_species[6];

      int       shadow_walk_room;
      int       shadow_walk_cooldown;

      int     brand_timeout;
      int     enthrall_timeout;
      int     enrapture_timeout;
      char *  building_fixation_name;
      int     building_fixation_level;
      char *  fixation_name;
      int     fixation_level;
      int     fixation_timeout;
      int     fixation_charges;
      int     fixation_mourning;

      int     prey_option_cooldown;
      int     prey_option;
      int     prey_emote_cooldown;
      int     villain_option_cooldown;
      int     villain_option;
      int     villain_emote_cooldown;
      bool    villain_grab;

      OBJ_DATA *  prey_object;

      int     animal_weights[6];
      int     animal_genus[6];
      int     animal_stats[6][25];
      
      int     firsts[5];
      int     emotes[10];
      int     coma;
      
      int     abomtime;
      int     abomcuring;
      int     abominfecting;
      int     pathtraining;
      int     pathlosing;
      int     pathtimer;
      
      int     partpay_timer;
      int     fulltime_timer;
      
      int clique_role; //Individual clique subcategories e.g. druggie, bully, hacker, etc
      int clique_rolechange; //last date changed
      
      /* Sex Variables                                    - Discordance                   */
      bool                auntflo_called;                 //Menses Notification check
      int                 last_sex;                       //Date of last sex
      int                 last_sextype;                   //Last sex_type -- Outercourse overwrites intercourse, shower clears
      int                 last_sexprotection;

      char *              sex_risk;                       //Risk of sex being attempted - Temp/Not saved - For consent mechanics
      char *              sex_type;                       //Type of sex being attempted - Temp/Not saved - For consent mechanics
      int                 last_shower;                    //Date of last shower
      bool                sex_dirty;                      //Is character dirty from outercourse
      int                 virginity_lost;                 //Date of mystic virginity loss
      int                 hymen_lost;                     //Date of physical virginity loss
      int                 inseminated;                    //Date inseminated
      int                 inseminated_type;               
      int                 inseminated_daddy_ID;           //Semen owner tracking
      int                 impregnated;                    //Date impregnated
      int                 impregnated_type;               //for determining length, special effects, etc
      int                 impregnated_daddy_ID;           //parentage
      int                 due_date;                       //Date impregnated
      int                 natural_fertility;              //Egg viability
      int                 estrogen;                       //hormone effects
      int                 testosterone;                   //hormone effects
      int                 maledevelopment;                //highest testosterone level reached
      int                 femaledevelopment;              //highest estrogen level reached
      int                 menstruation;                   //Date cycle started
      int                 ovulation;                      //Date of last ovulation
      CHAR_DATA *         sexing;                         //Attempting to have sex
      bool                sex_penetration;                //Records if sex_type includes penetration
      int                 sex_potency;                    //Sperm count, mobility, etc
      int                 daily_upkeep;                   //Date last updated
      int                 manual_upkeep;                  //Date of last manual upkeep
      char *              last_sexed[3];                 //last sexual partner
      int                 last_sexed_ID[3];              //last_sexed replacement IDs instead of names
      int                 last_true_sexed_ID;            //last sexual partner ID
      int                 count_dreamsex;                 //dreamsex counter

      int		  hp_sex;

      
            
      int         cdisc_range;
      char *        cdisc_name;
      
      int         super_influence;
      int         without_sleep;
      /* Ghost Variables                                  -Discordance                    */
      int                 ghost_pool;                     //action pool for ghosts
      int                 ghost_banishment;               //date ghost banished
      int                 final_death_date;               //date PC was removed from play
      int         ghost_room;
      int         ghost_wound;
      
      int         spectre;
      int         dream_door;
      int         dream_exit;
      CHAR_DATA *     connected_to;
      int         connection_stage;
      
      CHAR_DATA *     sr_connection;
      
      CHAR_DATA *     cansee;
      CHAR_DATA *         cansee2;
      CHAR_DATA *         cansee3;
      
      /* Chatroom Variables       */
      int         chatroom;
      char *        chat_handle;
      char *              chat_history;    //This is for the chatroom command - Discordance
      
      OBJ_DATA *      customizing;
      
      /** The vnum to which the 'home' command bring the PC */
      int       home;
      
      /** Number of attempts at password at login */
      sh_int        passatt;
      
      /** Map containing id to name associations */
      NameMap      *male_names;
      NameMap      *female_names;
      
      /** Description shown to others before introduction */
      char *       intro_desc;
      
      /* Used to determine if a person is permitted into an immortal area */
      bool    spec_trust;
      
      /* Location in Creation */
      int     creation_location;
      
      /** Note currently being worked on by the PC */
      Note      *pNote;
      
      /** NoteBoard currently being worked on by the PC */
      NoteBoard     *pNoteBoard;
      
      char      *noteText;
      
      /** Last read time stamps for each board */
      map<string, long>  *lastReadTimes;
      
      /** The following is an array of recently used commands by the PC. **/
      CMD_TYPE      *recent_command[5];
      
      int       secondary_timer;
      int       tertiary_timer;
      int       idling;
      
      int       idling_values[2];
    };
    
    
    /*
      * Liquids.
    */
    #define LIQ_WATER        0
    
    /**
      * @struct liq_type structs.h
      *
      * Contains information about a type of liquid
    */
    struct  liq_type
    {
      /** Name of the liquid */
      char *  liq_name;
      
      /** Color of the liquid */
      char *  liq_color;
      
      /** Affects of this liquid.  Drunk, etc */
      sh_int  liq_affect[5];
    };
    
    
    /**
      * @struct extra_descr_data structs.h
      *
      * Extra description data for a room or object.
    */
    struct  extra_descr_data
    {
      /** Pointer to next in list */
      EXTRA_DESCR_DATA *next;
      
      /** Indicates if this desc has been validated */
      bool valid;
      
      /** Keyword to use to look/examine the extra desc */
      char *keyword;
      
      /** The actual description to display */
      char *description;
    };
    
    /**
      * @struct obj_index_data
      *
      * Prototype for a mob.
      * This is the in-memory version of #OBJECTS.  When a new instance
      * of an obj is created, this is the prototype that it uses for all the
      * default values.
    */
    struct  obj_index_data
    {
      /** Pointer to next object data in the list */
      OBJ_INDEX_DATA *  next;
      
      /** Pointer to the objects extra descriptions */
      EXTRA_DESCR_DATA *  extra_descr;
      
      /** Pointer to the objects list of affects */
      AFFECT_DATA * affected;
      
      /** Pointer to the area the object is a part of. OLC */
      AREA_DATA *   area;
      
      /** Pointer to any special prgrams the object has */
      SPEC_FUN *          spec_fun;
      
      /** Name of the object */
      char *    name;
      
      /** The short description of the obj.  Displayed in inv, etc */
      char *    short_descr;
      
      /** The long description of the object.  Displayed in rooms, etc */
      char *    description;
      
      /** THe vnum of the object */
      int   vnum;
      
      /** the reset_num of the object */
      int   reset_num;
      
      /** name of the material the object is made of */
      char *    material;
      
      char *    wear_string;
      
      char *    wear_temp;
      
      char *    adjust_string;
      
      int     layer;
      int     exposed;
      
      /** The type of the object. See const.h (ITEM_*) */
      sh_int    item_type;
      
      /** Extra item flags applied to the object.
        * See const.h (ITEM_*)
      */
      int     extra_flags;
      
      /** The positions in which the object can be worn */
      int     wear_flags;
      
      /** The leve of the object */
      sh_int    level;
      
      /** The condition that the object is in.  Degrades through use */
      sh_int    condition;
      
      /** Number of the object in the game */
      sh_int    count;
      
      /** Weight of the object */
      sh_int    size;
      
      /** Cost of the object, in copper */
      int     cost;
      
      /** Values defining the object. Dependent on item_type */
      int     value[6];
      
      /** How many ticks before the object rots */
      sh_int    rot_timer;
      
      /** Pointer to the object's list of object programs */
      PROG_LIST *   oprogs;
      
      /** Bitvector of flags the object responds to for oprogs */
      long    oprog_flags;
      
      /** The "Fuzzy" level of the object */
      int     fuzzy;
      
      int     buff;
      int     faction;
      
      /** Percentage chance this object has of being loaded/reset */
      int     load_chance;
    };
    
    /**
      * @struct obj_data structs.h
      *
      * Contains information for an actual instance of an object
    */
    struct  obj_data
    {
      /** Pointer to the next object in the container */
      OBJ_DATA *    next_content;
      
      /** Pointer to the list of contents of the object */
      OBJ_DATA *    contains;
      
      /** Pointer to the object that this object is in */
      OBJ_DATA *    in_obj;
      
      /** Pointer to the object that the object is on */
      OBJ_DATA *    on;
      
      /** Pointer to the PC/NPC that carries the object */
      CHAR_DATA *   carried_by;
      
      CHAR_DATA *   owned_by;
      int     owned_time;
      int     stash_room;
      
      int     outfit[10];
      int     outfit_location[10];
      
      /** Pointer to the objects extra descriptions */
      EXTRA_DESCR_DATA *  extra_descr;
      
      /** Pointer to the objects list of affects */
      AFFECT_DATA * affected;
      
      /** Pointer to the object default prototype */
      OBJ_INDEX_DATA *  pIndexData;
      
      /** Pointer to the room the object is in */
      ROOM_INDEX_DATA * in_room;
      
      /** Indicates if the object has been validated */
      bool    valid;
      
      /** Number of enchants **/
      int     number_enchant;
      
      /** Name of the owner of the object.  Used to check looting */
      char *          owner;
      
      /** Name(keyword) of the object */
      char *    name;
      
      /** The short description of the obj.  Displayed in inv, etc */
      char *    short_descr;
      
      /** The long description of the object.  Displayed in rooms, etc */
      char *    description;
      
      char *    wear_string;
      
      char *    wear_temp;
      
      char *    adjust_string;
      
      int     layer;
      int     exposed;
      
      /** The type of the object. See const.h (ITEM_*) */
      sh_int    item_type;
      
      /**
        * Extra item flags applied to the object.
        * See const.h (ITEM_*)
      */
      int     extra_flags;
      
      /** The positions in which the object can be worn */
      int     wear_flags;
      
      /** The position the object is currently being worn in */
      sh_int    wear_loc;
      
      /** Weight of the object */
      sh_int    size;
      
      /** Cost of the object, in copper */
      int     cost;
      
      /** The leve of the object */
      sh_int    level;
      
      /** The condition that the object is in.  Degrades through use */
      sh_int    condition;
      
      /** name of the material the object is made of */
      char *    material;
      
      /** Number of ticks before item rots.  eg corpses */
      sh_int    timer;
      
      /** Values defining the object. Dependent on item_type */
      int     value [5];
      
      /** How many ticks before the object rots, worn or in inv */
      sh_int    rot_timer;
      
      int     buff;
      int     faction;
      
      /** Pointer to the target of the object program */
      CHAR_DATA *   oprog_target;
      
      /**
        * How many ticks has the oprog delay been set for
        * after the given number of ticks a new prog can be
        * triggered
      */
      sh_int    oprog_delay;
    };
    
    /**
      * @struct exit_data structs.h
      *
      * Contains information about a single exit
    */
    struct  exit_data
    {
      /**
        * @union u1
        * Union that either has the vnum to which the exit
        * goes or a pointer to the room_index_data
      */
      union
      {
        ROOM_INDEX_DATA * to_room;
        int     vnum;
      } u1;
      
      /** Info about status of door.
        * Eg. EX_LOCKED, EX_CLOSED, etc
      */
      sh_int    exit_info;
      
      /** Vnum of the key for this exit */
      sh_int    key;
      
      /** Keyword to unlock/open the exit */
      char *    keyword;
      
      /** Description of the exit */
      char *    description;
      
      /** Pointer to the next exit in the list of
        * exits that need to be updated.  This is currently
        * only used for exit affects
      */
      EXIT_DATA *   next_upd;
      
      /** Pointer to the exit in the list */
      EXIT_DATA *   next;
      
      int     rs_flags; /* OLC */
      
      /** The door dir to which this exit is associated */
      int     orig_door;
      
      int     jump;
      int     climb;
      int     fall;
      int     wall;
      
      int     wallcondition;
      
      int     doorbroken;
      
      /** Linked list of affects on the exit */
      AFFECT_DATA * affected;
      
      /** Bitvector of affects on the exit */
      SET     affected_by;
    };
    
    /*
      * Reset commands:
      *   '*': comment
      *   'M': read a mobile
      *   'O': read an object
      *   'P': put object in object
      *   'G': give object to mobile
      *   'E': equip object to mobile
      *   'D': set state of door
      *   'R': randomize room exits
      *   'S': stop (end of list)
    */
    
    /**
      * @struct reset_data structs.h
      *
      * Area-reset definition.
    */
    struct  reset_data
    {
      /** Pointer to next reset in the list */
      RESET_DATA *  next;
      
      /**
        * Indicates what type of reset
        *   '*': comment
        *   'M': read a mobile
        *   'O': read an object
        *   'P': put object in object
        *   'G': give object to mobile
        *   'E': equip object to mobile
        *   'D': set state of door
        *   'R': randomize room exits
        *   'S': stop (end of list)
      */
      char    command;
      
      /** Vnum of obj/mod to load */
      int   arg1;
      
      /** Vnum of where to put arg1 */
      int   arg2;
      
      /** Vnum dependant on command */
      int   arg3;
      
      /** Vnum dependent on command */
      int   arg4;
    };
    
    /**
      * @struct area_data structs.h
      *
      * Area definition.
    */
    struct  area_data
    {
      /** List of helps contained in this area */
      HELP_AREA *   helps;
      
      
      /** Name of the file that contains area info */
      char *    file_name;
      
      /** The name of the area */
      char *    name;
      
      /** How long since last reset */
      sh_int    age;
      
      /** Number of players in the area */
      sh_int    nplayer;
      
      /** Minimum vnum in the area */
      int     min_vnum;
      
      /** Maximum vnum in the area */
      int     max_vnum;
      
      bool    empty;
      
      /** Vnum of the area. OLC */
      int     vnum;
      
      /** Flags associated to this area. OLC */
      int     area_flags;
      
      /** Current version number of the area */
      int     version;
      
      /** Indicates if the area has been completed */
      int     area_completed;
      
      /** Climate of Area **/
      int     world;
      
      int     minx;
      int     miny;
      int     maxx;
      int     maxy;
      int     building;
    };
    
    /**
      * @struct room_index_data
      *
      * Prototype for a room.
      * This is the in-memory version of #ROOMS.  These are the prototypes
      * that it uses for all the default values.
    */
    struct  room_index_data
    {
      /** Pointer to next room in the list */
      ROOM_INDEX_DATA * next;
      
      /** Pointer to the list of people in the room */
      CharList    *people;
      
      /** This allows for archers to target other rooms */
      CHAR_DATA *   observing;
      
      /** Pointer to the list of the rooms contents */
      OBJ_DATA *    contents;
      
      /** Pointer to the room's extra descriptions */
      EXTRA_DESCR_DATA *  extra_descr;
      
      EXTRA_DESCR_DATA *  places;
      
      /** Pointer to the area the room belongs to */
      AREA_DATA *   area;
      
      //string to organize subareas by
      char *      subarea; //Discordance
      
      /** pointer to each of the exits */
      EXIT_DATA *   exit  [10];
      
      FEATURE_TYPE *  features[10];
      
      RESET_DATA *  reset_first;  /* OLC */
      RESET_DATA *  reset_last; /* OLC */
      
      /** The name of the room */
      char *    name;
      
      /** The long description of the room */
      char *    description;
      //char *    player_description; //playerbase descriptions
      //char *    player_shroud; //playersbase shroud descriptions
      
      char *    shroud;
      
      /** Name of the owner of the room */
      char *    owner;
      
      /** Vnum of the room */
      int   vnum;
      
      /** Special room flags. eg private, imp_only
        * see const.h (ROOM_*)
      */
      int     room_flags;
      sh_int    light;
      
      /**
        * What type of terrain the room consists of
        * See const.h (SECT_*)
      */
      sh_int    sector_type;
      
      int     x;
      int     y;
      int     z;
      
      sh_int    locx;
      sh_int    locy;
      sh_int    size;
      sh_int    entryx;
      sh_int    entryy;
      
      int     time;
      
      sh_int    timezone;
      int     cloud_cover;
      int     cloud_density;
      int     temperature;
      int     raining;
      int     hailing;
      int     snowing;
      int     mist_level;
      
      int     encroachment;
      int     security;
      int     toughness;
      int     decor;
      
      char *    vehicle_names[10];
      char *    vehicle_lplates[10];
      char *    vehicle_descs[10];
      char *    vehicle_owners[10];
      int     vehicle_cost[10];
      int     vehicle_typeone[10];
      int     vehicle_typetwo[10];
      
      char *    smell_desc[3];
      int     smell_dir[3];
      
      /** Healing rate for chars in the room */
      sh_int    level;
      
      /** Mana regain rate for chars in the room */
      sh_int    mana_rate;
      
      /** The clan the room belongs to */
      sh_int    clan;
      
      /** Pointer to list of room programs */
      PROG_LIST *   rprogs;
      
      /** List of targets for the rooms room progs */
      CHAR_DATA *   rprog_target;
      
      /** Bitvector of flags this room responds to for room progs */
      long    rprog_flags;
      
      /**
        * How many ticks has the oprog delay been set for
        * after the given number of ticks a new prog can be
        * triggered
      */
      sh_int    rprog_delay;
      
      /** Pointer to the next room in the list of rooms to update */
      ROOM_INDEX_DATA * next_upd;
      
      /** Pointer to the rooms list of affects */
      AFFECT_DATA *       affected;
      
      /**
        * Affect bits for the room.  Used SET (array of long)
        * to allow for unlimited bits.
        * See const.h (AFF_X*)
      */
      SET                 affected_by;
      
      /* Indicates if this room has been visited.  Used for pathfinding */
      bool    visited;
    };
    
    
    /**
      * @struct group_type structs.h
      *
      * Contains a list of skills that are grouped together as a package
    */
    struct  group_type
    {
      /** Name of the group */
      char *  name;
    };
    
    /**
      * @struct prog_list structs.h
      *
      * Contains the basic information for all type of progs.
      * Mprogs, rprogs, oprogs
    */
    struct prog_list
    {
      /** The type of trigger for the program */
      int   trig_type;
      
      /** Phrase that triggers the program(if applicable) */
      char *  trig_phrase;
      
      /** Vnum of the program */
      int vnum;
      
      /** The code associated to the program */
      char *  code;
      
      /** Pointer to next program in the list */
      PROG_LIST * next;
      
      /** Indicates if the list is validated */
      bool  valid;
    };
    
    /**
      * @struct prog_code structs.h
      *
      * Contains the basic information for all type of
      * progs.  Mprogs, rprogs, oprogs
    */
    struct prog_code
    {
      /** Vnum of the prog*/
      int   vnum;
      
      /** The code associated to the program */
      char *  code;
    };
    
    /**
      * @struct social_type structs.h
      *
      * Structure for a social in the socials table.
    */
    struct  social_type
    {
      /** Name of the social */
      char *      name;
      
      /** Message displayed TO_CHAR when no argument specified */
      char *      char_no_arg;
      
      /** Message displayed TO_ROOM when no argument specified */
      char *      others_no_arg;
      
      /** Message displayed TO_CHAR when victim is specified and found */
      char *      char_found;
      
      /** Message displayed TO_NOTVICT when victim is specified and found */
      char *      others_found;
      
      /** Message displayed TO_VICT hen victim is specified and found */
      char *      vict_found;
      
      /** Message displayed TO_CHAR when victim not found */
      char *      char_not_found;
      
      /** Message displayed TO_CHAR when target is 'self' */
      char *  char_auto;
      
      /** Message displayed TO_ROOM when target is 'self' */
      char *      others_auto;
    };
    
    
    /**
      * @struct body_info structs.h
      *
      * Structure to hold info about a body location for ACs
    */
    struct body_info
    {
      /* Wear locations associated to this body location */
      int locs[10];
      
      /* gsns of skills used to defend this body location */
      sh_int *gsns[10];
    };
    
    
    // This must be at the end of the file - Scaelorn
    #if defined(__cplusplus)
    }
  #endif
  
#endif



#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Global constants.
 */
extern	const	struct	con_app_type	con_app		[];
extern	const	struct	dex_app_type	dex_app		[];
extern	const	struct	int_app_type	int_app		[];
extern	const	struct	str_app_type	str_app		[];
extern	const	struct	wis_app_type	wis_app		[];

extern  	CLAN_TYPE		*nullclan;
extern		HOUSE_TYPE		*nullhouse;
extern	const	int			cover_table	[MAX_COVERS];
extern  const	int			contacts_table	[MAX_CONTACTS];
extern  const	struct	body_info	body_table	[MAX_BODY];
extern	const	struct	class_type	class_table	[MAX_CLASS];
extern	const   struct  move_type       punch_table     [MAX_PUNCH];
extern	const	struct	taxi_type	taxi_table	[MAX_TAXIS];
extern  const   struct  stance_type     stance_table    [MAX_STANCE];
extern  const   struct  move_type       kick_table	[MAX_KICK];
extern	const	struct 	finish_type	finish_table	[MAX_FINISH];
extern  const   struct  group_type      group_table	[MAX_GROUP];
extern  const   struct  item_type	item_table	[];
extern	const	struct	liq_type	liq_table	[];
extern  const   struct  metal_type	metal_table 	[];
extern  const	struct	spec_type	spec_table	[];
extern	const	struct	weapon_type	weapon_table	[];
extern  const   struct  department_type department_table [];
extern	const	struct	dstatus_type	dstatus_table [];
extern	const	struct	wiznet_type	wiznet_table	[];
extern	const	struct	roleplay_class_type	roleplay_class_table	[];
extern	const	char *		cup_size	[];

extern  const   struct  district_type       district_table      [];
extern  const   struct  discipline_type     discipline_table    [];
extern	const	struct	monster_type		monster_table	    [];
extern const	struct	special_type		special_table	    [];
extern  const   struct  skill_type          skill_table         [];
extern	const	struct	path_type		    path_table	        [];
extern	const	struct	arch_type		    arch_table	        [];
extern  const   struct  race_type		    race_table	        [];
extern  const   struct  cliquerole_type		cliquerole_table	[];
extern	const	struct	modifier_type		modifier_table	    [];
extern  const	struct	industry_type		industry_table	    [];
extern  const   int     room_list                               [];

extern  const   struct  worldlink_type            other_links        [];
extern  const   struct  worldlink_type            godrealm_links        [];
extern  const   struct  worldlink_type            wilds_links        [];

extern	int		blackmarket_value	[10];
extern	char *		blackmarket_name	[10];
extern  const		char *		world_names	[];
extern  const           char *          ammo_names     [];

extern  const		int		trolly_stops	[];
extern  const   	char *		comm_table      [];
extern	const		char *		dir_name        [][2];
extern  const		char *		reldir_name	[][2];
extern	const		char *		relspacial	[];
extern	const		char *		relincoming	[];
extern	const		char *		relfacing	[];
extern	const		char *		relwalking	[];
extern const		char *		tech_names	[];
extern	const		char *		sin_names	[];
extern	const		char *		goal_names	[];
extern	const		char *		hfile_type_table [MAX_HFILE_TYPE];
extern	const		char *		iname_table	[MAX_INAME];
extern  		char *		gl_table	[];
extern	const		sh_int		rev_dir         [];          /* sh_int - ROM OLC */
extern  const           sh_int          turn_dir	[];
extern  const           sh_int          aturn_dir        [];

extern 	const 		size_t      	rgSizeList      [MAX_MEM_LIST];


extern 		struct 	cmd_type 	*cmd_table;
extern          struct 	social_type 	*social_table;
extern		struct	system_data 	*sysdata;

/*
 * Global variables.
 */

extern          vector<EVENT_TYPE *>    EventVect;
extern		vector<ROSTERCHAR_TYPE *> RosterCharVect;

extern		vector<EXTRA_ENCOUNTER_TYPE *> EEncounterVect;

extern		vector<EXTRA_PATROL_TYPE *> EPatrolVect;

extern		vector<ANNIVERSARY_TYPE *> AnniversaryVect;


extern		vector<DECREE_TYPE *> DecreeVect;

extern		vector<WEEKLY_TYPE *> WeeklyVect;

extern		vector<GROUPTEXT_TYPE *> GTextVect;

extern		vector<TEXTHISTORY_TYPE *> HTextVect;


extern		vector<CHATROOM_TYPE *> ChatroomVect;

extern		vector<PROFILE_TYPE *> ProfileVect;

extern		vector<MATCH_TYPE *> MatchVect;

extern		vector<STORYIDEA_TYPE *> StoryIdeaVect;



extern		CharList	  	char_list;
extern		ChestList	  	chest_list;
extern		DescList   		descriptor_list;
extern		ShopList   		shop_list;
extern		HELP_DATA	  *	help_first;
extern		ObjList			object_list;
extern		EXIT_DATA	  *	exit_upd_list;
extern		ROOM_INDEX_DATA	  *	room_upd_list;
extern		GROUP_INDEX_DATA  *	group_list;

extern		ProgList		mprog_list;
extern		ProgList		oprog_list;
extern		ProgList		rprog_list;

extern		AreaList 	  	area_list;
extern		AREA_DATA 	  *	area_last;

extern		MOB_INDEX_DATA 	  *	mob_index_hash  [MAX_KEY_HASH];
extern		OBJ_INDEX_DATA 	  *	obj_index_hash  [MAX_KEY_HASH];
extern		ROOM_INDEX_DATA   *	room_index_hash [MAX_KEY_HASH];

extern		AUCTION_DATA	  *	auction_info;
extern 		CMD_TYPE 	  * 	command_hash[MAX_COMMAND_HASH];
extern		DisabledList		disabled_list; 

extern		KILL_DATA		kill_table	[];

extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;

extern		time_t			current_time;

extern		bool			fLogAll;
extern		bool			MOBtrigger;

extern		char			bug_buf		[];
extern      	char        		last_command	[MAX_STRING_LENGTH];
extern		char			log_buf		[];
extern		char			str_empty       [1];

extern		int			fight_problem;
extern		int			maxCommands;
extern		int     		maxRaces;
extern		int     		maxSubraces;
extern		int			num_quotes;
extern		int			top_affect;
extern		int			top_area;
extern		int			top_ed;
extern		int			top_exit;
extern		int			top_help;
extern		int			top_mob_index;
extern		int			top_group_index;
extern		int			top_obj_index;
extern		int			top_reset;
extern		int			top_room;
extern		int			top_shop;

extern			vector<PAGE_TYPE *> PageVect;
extern			vector<FANTASY_TYPE *> FantasyVect;
extern			vector<NEWDESTINY_TYPE *> DestinyVect;

extern                 vector<SKILL_TYPE *> SkillVect;

extern			vector<PLAYERROOM_TYPE *> PlayerroomVect;

extern			vector<STORY_TYPE *>	StoryVect;

extern                  vector<PLOT_TYPE *>    PlotVect;

extern			vector<LOCATION_TYPE *> locationVect;

extern			vector<GATEWAY_TYPE * > gatewayVect;

extern			vector<NEWS_TYPE *>	NewsVect;

extern			vector<ARCHIVE_INDEX *>	ArchiveVect;

extern 			vector<PROP_TYPE *> 	PropVect;

extern			vector<DOMAIN_TYPE *>	DomainVect;

extern			vector<FACTION_TYPE *>	FacVect;


extern			vector<INSTITUTE_TYPE *> InVect;

extern			vector<PHONEBOOK_TYPE *> PhoneVect;

extern			vector<PETITION_TYPE *> PetitionVect;

extern			vector<OPERATION_TYPE *> OpVect;

extern			vector<GRAVE_TYPE * > 	GraveVect;

extern			OPERATION_TYPE * activeoperation;

extern		char *			help_inferno;
extern		char *			help_motd;
extern		char *			help_story;
extern		char *			troll_ip;

extern		int			fantasy_bonus;

extern		int			battle_countdown;
extern		int			hailing;
extern		int			snowing;
extern		int			raining;

extern		int			clock_hour;
extern		int			clock_minute;
extern		int			clock_second;

extern		int			trolly_at;
extern		int			trolly_moving;
extern		int			trolly_timer;

extern		int			evil_hotspot;
extern		int			good_hotspot;

extern		int			town_blackout;
extern		int			crisis_noshroud;
extern		int			crisis_flood;
extern		int			crisis_storm;
extern		int			crisis_lockdown;
extern		int			crisis_hurricane;
extern		int			crisis_light;
extern		int			crisis_blackout;
extern		int			supernatural_darkness;
extern		int			crisis_darkness;
extern		int			crisis_brainwash;
extern		char *			crisis_brainwash_message;
extern		char *			crisis_atmosphere;
extern		char *			global_erratic_message;
extern		char *			global_constant_message;
extern		int			global_damage_constant;
extern		int			global_damage_erratic;
extern		int			crisis_nodrive;
extern		int			crisis_mist;
extern		int			crisis_uninvited;
extern		int			crisis_notravel;
extern		int			crisis_prologue;

extern		int			invade_aerial_one;
extern          int                     invade_aerial_two;
extern          int                     invade_aerial_three;

extern		int			other_cloud_cover;
extern		int			other_cloud_density;
extern		int			other_temperature;
extern		int			other_raining;
extern		int			other_hailing;
extern		int			other_snowing;

extern		int			wilds_cloud_cover;
extern		int			wilds_cloud_density;
extern		int			wilds_temperature;
extern		int			wilds_raining;
extern		int			wilds_hailing;
extern		int			wilds_snowing;

extern		int			godrealm_cloud_cover;
extern		int			godrealm_cloud_density;
extern		int			godrealm_temperature;
extern		int			godrealm_raining;
extern		int			godrealm_hailing;
extern		int			godrealm_snowing;

extern		int			hell_cloud_cover;
extern		int			hell_cloud_density;
extern		int			hell_temperature;
extern		int			hell_raining;
extern		int			hell_hailing;
extern		int			hell_snowing;

extern		char * 			last_on_names[10];
extern		char * 			last_on_times[25];

extern		char *			global_damage;

extern		char *			hero_name;
extern		char *			villain_name;

extern		int			top_vnum_mob;
extern		int			top_vnum_obj;
extern		int			top_vnum_room;

extern		int			maxClubs;
extern		int			gClubsInUse;


extern	int	event_recruitment;
extern 	int	event_teaching;
extern	int	event_occurance;
extern	int	event_operation;

extern	int	event_dominance;
extern	int	event_aegis;
extern	int	event_cleanse;
extern	int	event_catastrophe;

/*
 * Items for countdown rebot/shutdown
 */
extern 		bool 			isreboot;
extern	 	int 			pulse_muddeath;
extern 		int 			pulse_copyover;
extern 		CHAR_DATA 	  * 	mudkiller;

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif


#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */

DECLARE_DO_FUN( do_tpoint	    );


/* others */
DECLARE_DO_FUN( do_ability	);
DECLARE_DO_FUN(	do_accept	);
DECLARE_DO_FUN(	do_activate	);
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_addapply	);
DECLARE_DO_FUN( do_affects	);
DECLARE_DO_FUN( do_afk		);
DECLARE_DO_FUN( do_alia		);
DECLARE_DO_FUN( do_aikido	);
DECLARE_DO_FUN( do_alias	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN( do_ageless	);
DECLARE_DO_FUN( do_angreal	);
DECLARE_DO_FUN( do_anticlockwise);
DECLARE_DO_FUN( do_antispec	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN( do_armor	);
DECLARE_DO_FUN( do_arrow	);
DECLARE_DO_FUN( do_assassinate	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN( do_attack	);
DECLARE_DO_FUN( do_attackstat	);
DECLARE_DO_FUN( do_auction	);
DECLARE_DO_FUN( do_autoassist	);
DECLARE_DO_FUN( do_autoexit	);
DECLARE_DO_FUN( do_autogold	);
DECLARE_DO_FUN( do_autolist	);
DECLARE_DO_FUN( do_autoloot	);
DECLARE_DO_FUN( do_autosac	);
DECLARE_DO_FUN( do_autosave	);
DECLARE_DO_FUN( do_autosplit	);
DECLARE_DO_FUN( do_autostyle	);
DECLARE_DO_FUN( do_autotie	);
DECLARE_DO_FUN( do_avator	);

DECLARE_DO_FUN( do_balance	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN(	do_banish	);
DECLARE_DO_FUN( do_bite		);
DECLARE_DO_FUN( do_bond		);
DECLARE_DO_FUN( do_barge	);
DECLARE_DO_FUN( do_board	);
DECLARE_DO_FUN( do_bondtalk	);
DECLARE_DO_FUN( do_bonus	);
DECLARE_DO_FUN( do_break	);
DECLARE_DO_FUN( do_break_obj    );
DECLARE_DO_FUN( do_breakout     );
DECLARE_DO_FUN( do_brief	);
DECLARE_DO_FUN(	do_build	);
DECLARE_DO_FUN(	do_building	);
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);

DECLARE_DO_FUN( do_call		);
DECLARE_DO_FUN( do_cashremove	);   
DECLARE_DO_FUN( do_carry	);
DECLARE_DO_FUN( do_change	);
DECLARE_DO_FUN( do_changes	);
DECLARE_DO_FUN( do_channels	);
DECLARE_DO_FUN( do_chatroom	);
DECLARE_DO_FUN( do_cityedit     );
DECLARE_DO_FUN( do_clan         );
DECLARE_DO_FUN( do_clansay      );
DECLARE_DO_FUN( do_chat		);
DECLARE_DO_FUN( do_chatroom	);

DECLARE_DO_FUN( do_cleanse	);

DECLARE_DO_FUN( do_clear	);
DECLARE_DO_FUN( do_clearscr	);
DECLARE_DO_FUN( do_climb	);
DECLARE_DO_FUN( do_clockwise	);
DECLARE_DO_FUN( do_clone	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_coding       );
DECLARE_DO_FUN( do_color        );
DECLARE_DO_FUN( do_colortest	);
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN( do_combine	);
DECLARE_DO_FUN( do_compact	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN( do_convert	);
DECLARE_DO_FUN(	do_copyover	);
DECLARE_DO_FUN( do_count	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_custom	);
DECLARE_DO_FUN( do_deaf		);
DECLARE_DO_FUN( do_deathblow	);
DECLARE_DO_FUN( do_delet	);
DECLARE_DO_FUN( do_delete	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN( do_deposit	);
DECLARE_DO_FUN(	do_descedit	);
DECLARE_DO_FUN( do_direct	);
DECLARE_DO_FUN( do_disable	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN( do_disciplines	);
DECLARE_DO_FUN( do_discstat	);
DECLARE_DO_FUN( do_disperse     );
DECLARE_DO_FUN( do_display	);
DECLARE_DO_FUN( do_dlook	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN( do_draw         );
DECLARE_DO_FUN( do_dream	);
DECLARE_DO_FUN( do_dreamworld	);
DECLARE_DO_FUN( do_drag         );
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN( do_dump		);

DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN( do_email	);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN( do_emotestat	);
DECLARE_DO_FUN( do_engage	);
DECLARE_DO_FUN(	do_enroll		);
DECLARE_DO_FUN( do_entranceedit	);
DECLARE_DO_FUN( do_enter	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN( do_event	);
DECLARE_DO_FUN(	do_evolve	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN( do_expel	);

DECLARE_DO_FUN( do_face		);
DECLARE_DO_FUN( do_faction_where);
DECLARE_DO_FUN( do_famestat	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_fingershow   );
DECLARE_DO_FUN(	do_finger	);
DECLARE_DO_FUN( do_fingerset	);
DECLARE_DO_FUN(	do_fish		);
DECLARE_DO_FUN( do_flag		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN( do_flow		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN( do_for		);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN( do_formstat	);
DECLARE_DO_FUN(	do_freevnum	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN( do_fset		);
DECLARE_DO_FUN( do_fvlist	);
DECLARE_DO_FUN( do_gain		);
DECLARE_DO_FUN( do_garagestat   );
DECLARE_DO_FUN( do_gchange	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN( do_goal		);
DECLARE_DO_FUN( do_gossip	);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN( do_groups	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_guard	);
DECLARE_DO_FUN( do_guide	);
DECLARE_DO_FUN( do_grab		);
DECLARE_DO_FUN( do_villainchase);
DECLARE_DO_FUN( do_preyescape);


DECLARE_DO_FUN( do_hangup	);
DECLARE_DO_FUN( do_handtalk	);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN( do_helpseealso  );
DECLARE_DO_FUN( do_helptext     );
DECLARE_DO_FUN( do_helpkeyword  );
DECLARE_DO_FUN( do_helpdelete   );
DECLARE_DO_FUN( do_helpnew      );
DECLARE_DO_FUN( do_hint		);
DECLARE_DO_FUN( do_history	);
DECLARE_DO_FUN( do_hotspot	);
DECLARE_DO_FUN( do_hlook	);
DECLARE_DO_FUN(	do_hoe		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN( do_holdings      );
DECLARE_DO_FUN( do_home		);
DECLARE_DO_FUN( do_homeset	);
DECLARE_DO_FUN( do_hp		);

DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN( do_imprint	);
DECLARE_DO_FUN( do_idleabuse	);
DECLARE_DO_FUN( do_idlecounters );
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN( do_imotd	);
DECLARE_DO_FUN( do_implock	);
DECLARE_DO_FUN( do_imptalk	);
DECLARE_DO_FUN( do_improve_armor);
DECLARE_DO_FUN( do_incognito	);
DECLARE_DO_FUN( do_inform	);
DECLARE_DO_FUN( do_insomnia	);
DECLARE_DO_FUN( do_intro	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);

DECLARE_DO_FUN( do_jetlag	);

DECLARE_DO_FUN( do_karate	);
DECLARE_DO_FUN( do_kickboxing	);
DECLARE_DO_FUN( do_kungfu	);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN( do_knock	);
DECLARE_DO_FUN( do_knockoutpunch);

DECLARE_DO_FUN( do_landmarkedit	);
DECLARE_DO_FUN(	do_landscape	);
DECLARE_DO_FUN(	do_lay		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN( do_load		);
DECLARE_DO_FUN( do_locate_reset	);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN( do_lore         );

DECLARE_DO_FUN( do_mail		);
DECLARE_DO_FUN( do_mask		);
DECLARE_DO_FUN( do_manifest	);
DECLARE_DO_FUN( do_makeobject	);
DECLARE_DO_FUN( do_makeroom	);
DECLARE_DO_FUN( do_mbanish	);
DECLARE_DO_FUN(	do_memorize	);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN( do_minion	);
DECLARE_DO_FUN( do_minioncommand);
DECLARE_DO_FUN( do_mintalk	);
DECLARE_DO_FUN( do_mission	);
DECLARE_DO_FUN( do_missionstat	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN( do_mob		);
DECLARE_DO_FUN( do_money	);
DECLARE_DO_FUN( do_moneyset	);
DECLARE_DO_FUN( do_motd		);
DECLARE_DO_FUN( do_move	);
DECLARE_DO_FUN( do_mpaward	);
DECLARE_DO_FUN( do_mpstat	);
DECLARE_DO_FUN( do_mpdump	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN( do_myarea	);

DECLARE_DO_FUN( do_newbie	);
DECLARE_DO_FUN( do_newdrive	);
DECLARE_DO_FUN( do_newlock	);
DECLARE_DO_FUN( do_newlook	);
DECLARE_DO_FUN( do_news		);
DECLARE_DO_FUN( do_negtrain	);
DECLARE_DO_FUN( do_nochannels	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN( do_nofollow	);
DECLARE_DO_FUN( do_noloot	);
DECLARE_DO_FUN( do_nopkill	);
DECLARE_DO_FUN( do_nointro	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN( do_northeast	);
DECLARE_DO_FUN( do_northwest	);
DECLARE_DO_FUN( do_nosummon	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_noyell	);
DECLARE_DO_FUN(	do_nukez	);

DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN( do_ogier	);
DECLARE_DO_FUN(	do_oldscore	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN( do_ooc		);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_opedit	);
DECLARE_DO_FUN(	do_opdump	);
DECLARE_DO_FUN(	do_opstat	);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN( do_osay		);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_outfit	);
DECLARE_DO_FUN( do_newbieoutfit );
DECLARE_DO_FUN( do_owhere	);

DECLARE_DO_FUN(	do_package	);
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN( do_park		);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_peaceful	);
DECLARE_DO_FUN( do_pecho	);
DECLARE_DO_FUN( do_pemote	);
DECLARE_DO_FUN( do_penalty	);
DECLARE_DO_FUN( do_permban	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN( do_pickup	);
DECLARE_DO_FUN( do_pin		);
DECLARE_DO_FUN( do_play		);
DECLARE_DO_FUN( do_pload	);
DECLARE_DO_FUN( do_pmote	);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN( do_pour		);
DECLARE_DO_FUN( do_pray		);
DECLARE_DO_FUN( do_prayto       );
DECLARE_DO_FUN( do_prefi	);
DECLARE_DO_FUN( do_prefix	);
DECLARE_DO_FUN( do_procedure	);
DECLARE_DO_FUN( do_prompt	);
DECLARE_DO_FUN( do_promote	);
DECLARE_DO_FUN( do_propset	);
DECLARE_DO_FUN(	do_prospect	);
DECLARE_DO_FUN( do_protect	);
DECLARE_DO_FUN( do_pstat	);
DECLARE_DO_FUN( do_punch	);
DECLARE_DO_FUN( do_punload	);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN( do_push         );
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN( do_ptell	);

DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN( do_questchannel	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN( do_quiet	);
DECLARE_DO_FUN(	do_quit		);

DECLARE_DO_FUN( do_rcm		);
DECLARE_DO_FUN( do_read		);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_refuse	);
DECLARE_DO_FUN( do_release	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN( do_replay	);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_roomie	);
DECLARE_DO_FUN(	do_role	);
DECLARE_DO_FUN( do_roleplay	);
DECLARE_DO_FUN( do_roll	        );
DECLARE_DO_FUN( do_roomlist	);
DECLARE_DO_FUN( do_rpaward	);
DECLARE_DO_FUN( do_rpedit	);
DECLARE_DO_FUN( do_rpexpaward	);
DECLARE_DO_FUN( do_rpdump	);
DECLARE_DO_FUN( do_rpstat	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN( do_rules	);

DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN( do_savehelps	);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN( do_scroll	);
DECLARE_DO_FUN( do_search	);
DECLARE_DO_FUN( do_secset	);
DECLARE_DO_FUN( do_sedit	);
DECLARE_DO_FUN(	do_seed		);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_sense	);
DECLARE_DO_FUN( do_set		);
DECLARE_DO_FUN( do_settime	);
DECLARE_DO_FUN( do_sex   	);
DECLARE_DO_FUN( do_sexupkeep   	);
DECLARE_DO_FUN( do_sheath	);
DECLARE_DO_FUN( do_show		);
DECLARE_DO_FUN( do_showaffects	);
DECLARE_DO_FUN( do_showcity     );
DECLARE_DO_FUN( do_showlandmarks);
DECLARE_DO_FUN( do_showstats	);
DECLARE_DO_FUN( do_showraces	);
DECLARE_DO_FUN( do_showsysdata	);
DECLARE_DO_FUN( do_showsubrace	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN( do_sinspirit	);
DECLARE_DO_FUN( do_sip          );
DECLARE_DO_FUN( do_sit		);
DECLARE_DO_FUN( do_skills	);
DECLARE_DO_FUN( do_skstat	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN( do_smote	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN( do_socials	);
DECLARE_DO_FUN( do_sockets	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN(	do_southeast	);
DECLARE_DO_FUN(	do_southwest	);
DECLARE_DO_FUN( do_spar		);
DECLARE_DO_FUN( do_speak	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN( do_squish	);
DECLARE_DO_FUN( do_sredit       );
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN( do_stake	);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_stash	);
DECLARE_DO_FUN( do_stashstat	);
DECLARE_DO_FUN( do_stat		);
DECLARE_DO_FUN( do_statstat	);
DECLARE_DO_FUN( do_stell	);
DECLARE_DO_FUN( do_stopspell	);
DECLARE_DO_FUN( do_story	);
DECLARE_DO_FUN( do_storyrunner  );
DECLARE_DO_FUN( do_string	);
DECLARE_DO_FUN(	do_superjump	);
DECLARE_DO_FUN(	do_surrender	);
DECLARE_DO_FUN( do_swear	);
DECLARE_DO_FUN(	do_switch	);

DECLARE_DO_FUN( do_talent       );
DECLARE_DO_FUN( do_taint        );
DECLARE_DO_FUN( do_teach	    );
DECLARE_DO_FUN( do_techstat	    );
DECLARE_DO_FUN(	do_tell		    );
DECLARE_DO_FUN(	do_time		    );
DECLARE_DO_FUN(	do_title	    );
DECLARE_DO_FUN( do_tpaward	    );
DECLARE_DO_FUN( do_travel	    );
DECLARE_DO_FUN(	do_transfer	    );
DECLARE_DO_FUN( do_treesing     );
DECLARE_DO_FUN(	do_trim		    );
DECLARE_DO_FUN( do_trip		    );
DECLARE_DO_FUN( do_trp          );
DECLARE_DO_FUN(	do_tranquilize	);
DECLARE_DO_FUN(	do_trust	    );
DECLARE_DO_FUN(	do_typo		    );

DECLARE_DO_FUN( do_unalias	);
DECLARE_DO_FUN(	do_unenroll	);
DECLARE_DO_FUN( do_unhide	);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN( do_unread	);
DECLARE_DO_FUN( do_unroster	);
DECLARE_DO_FUN( do_unsheath	);
DECLARE_DO_FUN(	do_up		);

DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_violate	);
DECLARE_DO_FUN( do_vomit	);
DECLARE_DO_FUN( do_vnum		);

DECLARE_DO_FUN( do_walk		);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN( do_where	);
DECLARE_DO_FUN( do_whisper	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN( do_whois	);
DECLARE_DO_FUN( do_whorank	);
DECLARE_DO_FUN( do_whotitle     );
DECLARE_DO_FUN( do_width	);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN( do_withdraw	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_wizlist	);
DECLARE_DO_FUN( do_wiznet	);
DECLARE_DO_FUN( do_work		);
DECLARE_DO_FUN( do_worth	);
DECLARE_DO_FUN( do_write	);
DECLARE_DO_FUN( do_wrlist	);
DECLARE_DO_FUN(	do_wsave	);
DECLARE_DO_FUN( do_wset		);
DECLARE_DO_FUN(	do_wwhere	);

DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN( do_yes          );

DECLARE_DO_FUN( do_zecho	);

// OLC commands.
DECLARE_DO_FUN( do_asave	);
DECLARE_DO_FUN( do_aedit	);
DECLARE_DO_FUN( do_alist	);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN( do_medit	);
DECLARE_DO_FUN( do_mpedit	);
DECLARE_DO_FUN( do_oedit	);
DECLARE_DO_FUN( do_olc		);
DECLARE_DO_FUN( do_redit	);
DECLARE_DO_FUN( do_resets	);
DECLARE_DO_FUN( do_groupedit	);


DECLARE_DO_FUN( print_guess_level );
DECLARE_DO_FUN( print_level_value );
/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define AD	AFFECT_DATA
#define PC	PROG_CODE

int 	get_mlvl		args( (MOB_INDEX_DATA *ch) );

bool 	check_flee		args( ( CHAR_DATA *ch, char *argument ) );
bool	flee_vmap		args( ( CHAR_DATA *ch ) );
void    disarm          	args( ( CHAR_DATA *ch) );

/* abomination.c */
void limunderglow		args( (CHAR_DATA *ch) );
bool seems_under_limited	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool under_limited		args( (CHAR_DATA *ch, CHAR_DATA *victim) );

bool	has_binding_circle	args( (ROOM_INDEX_DATA *room) );
void	domain_xp		args( (CHAR_DATA *ch, int amount) );
DOMAIN_TYPE * my_domain		args( (CHAR_DATA *ch) );
DOMAIN_TYPE * vnum_domain	args( (int vnum) );
DOMAIN_TYPE * domain_by_name       args( (char * dname) );

bool power_bound		args( (CHAR_DATA *ch) );
DOMAIN_TYPE * domain_from_room	args( (ROOM_INDEX_DATA *room) );
bool is_in_domain	args( (CHAR_DATA *ch) );
void send_domain_home	args( (CHAR_DATA *ch));

ROOM_INDEX_DATA * get_lair_room	args( (int world) );
ROOM_INDEX_DATA * get_random_outerforest args( ( void ) );
ROOM_INDEX_DATA * random_inner_forest args( ( void) );
CHAR_DATA *	random_person	args( ( void ) );
bool	lair_room		args( (ROOM_INDEX_DATA *room) );
void	psychic_feast		args( (CHAR_DATA *ch, int type, int mod) );
void 	append_messages		args( (CHAR_DATA *ch, char * message) );
void	lair_update		args( ( void ) );
void	lair_mobkill		args( (ROOM_INDEX_DATA *room, int vnum) );
void	lair_populate		args( ( void ) );
bool	in_lair			args( (CHAR_DATA *mob) );
bool	pc_in_lair		args( (CHAR_DATA *mob) );
void    monster_abduction args( (CHAR_DATA *mob, CHAR_DATA *victim) );
void    save_lairs             args( (bool backup) );
void    load_lairs             args( ( void ) );

bool 	mist_room		args( (ROOM_INDEX_DATA *room) );
void	abomination_global_update args( (void ) );
bool	is_town_blackout		args( ( void ) );
bool	mist_manipulated_room	args( (ROOM_INDEX_DATA *room) );
void 	breakcontrol		args( (char * name, int type) );
bool 	isalive			args( (char * name) );
void 	remember_detail		args( (CHAR_DATA *ch, CHAR_DATA *target) );
void	nounderglow		args( (CHAR_DATA *ch) );
void	apply_blackeyes		args( (CHAR_DATA *ch) );
bool 	is_abom			args( (CHAR_DATA *ch) );
bool	has_blackeyes		args( (CHAR_DATA *ch) );
void	abom_logon		args( (CHAR_DATA *ch) );

/* attrib.c */
void 	change_attribute   	args( (CHAR_DATA *ch, int stat) );
void 	attributes_at_level	args( (CHAR_DATA *ch) );

/* act_comm.c */


bool 	power_whisper		args( (CHAR_DATA *ch, CHAR_DATA *spir) );
bool    dm_to_person		args( (CHAR_DATA *ch, char *tname, char * msg, bool picture) );

bool 	isTextFlagged		args( (const std::string& text) );

void 	to_photo_message	args( (CHAR_DATA *ch, char * text) );
void	prp_rplog		args( (CHAR_DATA *ch, char * argument) );
bool	can_rp_log		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	char_rplog		args( (CHAR_DATA *ch, char * argument) );
char * emote_name		args( (CHAR_DATA *ch) );
void	pc_rp_log			args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room, char * argument) );
char *	logact			args( (char * format, CHAR_DATA *ch, CHAR_DATA *vch));
bool     character_exists        args( (char * name) );
void	abuse_check		args( (CHAR_DATA *ch, char * string) );
void	disease_check		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	quit_process		args( (CHAR_DATA *ch) );
LOCATION_TYPE * offline_territory args( (char * name) );
bool         antagonist              args( (CHAR_DATA *ch) );
void	retire_character	args( (CHAR_DATA *ch) );
void	lookup_char		args( (CHAR_DATA *ch, CHAR_DATA *victim, int level) );
void	research_reward		args( (CHAR_DATA *ch, char * title, char * text) );
void	note_reward		args( (CHAR_DATA *ch, char * tostring, char * text) );
int	making_minion_cost	args( (CHAR_DATA *ch) );
int	making_minion_shield	args( (CHAR_DATA *ch) );
void	give_room_attention	args( (CHAR_DATA *ch) );
void	give_attention		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	spymessage		args( (ROOM_INDEX_DATA *ch, char * message) );
void 	process_emote		args( (CHAR_DATA *ch, CHAR_DATA *target, int type, char * argument) );
void	charge_influence	args( (CHAR_DATA *ch, int type, int amount) );
void	mad_comms		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool 	same_place		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int			teacher_bonus				args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
//MXP - Discordance
int				count_mxp_tags				args( (const int bMXP, const char *txt, int length) );
void			convert_mxp_tags			args( (const int bMXP, char * dest, const char *src, int length) );
int			naturalmod				args( (CHAR_DATA *ch) );
void			re_race					args( (CHAR_DATA *ch) );
void			rpreward				args( (CHAR_DATA *ch, char * argument, bool remote, int pop) );
void			spyshow					args( (CHAR_DATA *ch, char * argument) );
int 			get_guesttrust				args( (CHAR_DATA *ch, CHAR_DATA *guest) );
ACCOUNT_TYPE *	reload_account				args( (char * name) );
bool 			can_logoff				args( (CHAR_DATA *ch) );
bool			guestplot			args( (CHAR_DATA *guest) );
bool			offline_flag				args( (char * name, int flag) );
void			offline_setflag				args( (char * name, int flag) );
void			add_char_to_account			args( (CHAR_DATA *ch, ACCOUNT_TYPE *account) );
int				character_account_count		args( (ACCOUNT_TYPE *account) );
int			total_account_count			args( (ACCOUNT_TYPE *account) );
void			save_account				args( (ACCOUNT_TYPE *account, bool backup) );
bool			load_account_obj			args( ( DESCRIPTOR_DATA *d, char *name ) );
int 			get_summon_x				args( (ROOM_INDEX_DATA *room, char * argument) );
int 			get_summon_y				args( (ROOM_INDEX_DATA *room, char * argument) );
int 			ci_heightmod				args( (int amount) );
int				ci_bustmod					args( (int amount) );
void 			bust_a_prompt				args( ( CHAR_DATA *ch ) );
void			news_feed					args( (CHAR_DATA *ch, int type) );
void			offline_message				args( (char *arg1, char *message) );
int 			get_snooptrust				args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int 			social_count				args( (CHAR_DATA *ch) );
bool			has_goblinkey			    args( (CHAR_DATA *ch) );
bool			has_symbol_core					args( (CHAR_DATA *ch) );
bool                    has_symbol_cult                                 args( (CHAR_DATA *ch) );
bool                    has_symbol_sect                                 args( (CHAR_DATA *ch) );
bool 			has_other_symbol			args( (CHAR_DATA *ch) );
CHAR_DATA	*	get_gm						args( (ROOM_INDEX_DATA *room, bool permission) );
char		* 	skill_name					args( (int vnum) );

int 			get_gmtrust					args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int			get_probetrust				args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void			take_artifacts				args( ( CHAR_DATA *ch) );
void  			check_sex					args( ( CHAR_DATA *ch) );
void			add_follower				args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void			stop_follower				args( ( CHAR_DATA *ch ) );
void 			nuke_pets					args( ( CHAR_DATA *ch ) );
void			die_follower				args( ( CHAR_DATA *ch ) );
bool			is_same_group				args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void 			logfi 						args((char * fmt, ...));
bool			write_to_descriptor			args( ( int desc, char *txt, int length ) );
void			do_quotes					args( ( CHAR_DATA *ch ) );
//void			  do_immtalk				  args( ( CHAR_DATA *ch, char * argument));
//void			  do_imptalk				  args( ( CHAR_DATA *ch, char * argument));
void 			writef_to_buffer 			args( (DESCRIPTOR_DATA *d, char *fmt, ...) );
bool			access_internet				args( (CHAR_DATA *ch) );
bool			holding_phone				args( (CHAR_DATA *ch) );
bool			wearing_phone				args( (CHAR_DATA *ch) );
OBJ_DATA	*	get_phone					args( (CHAR_DATA *ch) );

/* act_enter.c */
RID  	*get_random_room   	args ( (CHAR_DATA *ch) );

/* attract.c */

char * 	attract_color		args( (int val) );
void 	attract_average_update	args( (CHAR_DATA *ch) );
int	attractive_rare_bonus	args( (CHAR_DATA *ch) );
int	behaviour_mod		args( (CHAR_DATA *ch) );
int 	praise_mod		args( (CHAR_DATA *ch) );
int 	prom_mod		args( (CHAR_DATA *ch) );
int 	standards_mod		args( (CHAR_DATA *ch) );
int	character_mod		args( (CHAR_DATA *ch) );
int 	outfit_bonus		args( (CHAR_DATA *ch) );
int 	base_attract_new	args( (CHAR_DATA *ch) );
int     cover_count     	args( (CHAR_DATA *ch) );
int	influence_mod		args( (CHAR_DATA *ch) );
int 	nudity_value		args( (CHAR_DATA *ch) );
int	attract_modifiers	args( (CHAR_DATA *ch) );
void	assess_appearance	args( (CHAR_DATA *ch, CHAR_DATA *victim) );

/* act_info.c */
int 	daylight_level			args( (ROOM_INDEX_DATA *room) );

int 	light_level			args( (ROOM_INDEX_DATA *room) );
int	distance_from_town		args( (ROOM_INDEX_DATA *current_room));
char *  text_complete_box		args( (char * text, char * delim, int width, bool cuttop) );
char * 	strip_newlines			args( (char * text) );
bool	can_hair			args( (CHAR_DATA *ch) );
bool	can_eyes			args( (CHAR_DATA *ch) );
 int            attract_lifeforce           args( (CHAR_DATA *ch) );
char * 	text_block_to_box		args( (char * text, char * delim, int width) );
char *  line_to_box			args( (char * line, char * delim, int width) );
void 	introduce_char_to_char		args( (CHAR_DATA *victim, CHAR_DATA *ch) );
void    show_char_to_char_0                 args( ( CHAR_DATA *victim, CHAR_DATA *ch, int range ) );
char * 	get_default_dreamdesc		args( (CHAR_DATA *ch) );
void	show_room_to_char		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
char *	class_string			args( (int vnum) );
void    show_char_to_char_1     args( ( CHAR_DATA *victim, CHAR_DATA *ch, int type, bool photo ) );
char *	newtexttime				args( (void) );
char *	get_hair				args( (CHAR_DATA *ch, CHAR_DATA *victim) );
char *	get_eyes				args( (CHAR_DATA *ch, CHAR_DATA *victim) );
char *  roomtitle               args( (ROOM_INDEX_DATA *room, bool capital) );
void	social_behave_mod		args( (CHAR_DATA *ch, int amount, char * message) );
char *	equip_string			args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	rp_prompt				args( (CHAR_DATA *ch) );
bool	are_friends				args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool 	immaccount				args( (CHAR_DATA *ch) );
bool	immaccount_name			args( (char * argument) );
int 	location_by_name		args( (char * arg1) );
char * 	name_by_location		args( (int location) );
int		room_level				args( (ROOM_INDEX_DATA *room) );
char*	room_distance			args ( ( CHAR_DATA *ch, ROOM_INDEX_DATA *finish_room) );
bool	valid_room_distance		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *finish_room) );
int		vision_range_character	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int		vision_range_room		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *finish) );
int 	social_standing			args( (CHAR_DATA *ch) );
int 	get_age_descriptor		args( (CHAR_DATA *victim) );
bool	multiplayer_three		args( (CHAR_DATA *ch) );
bool	multiplayer_super		args( (CHAR_DATA *ch) );
bool	multiplayer_abom		args( (CHAR_DATA *ch) );
bool    is_whoinvis				args( ( CHAR_DATA *ch ) );
char *	get_intro				args( (CHAR_DATA *ch) );
int		racebypowername			args( (char *argument) );
char *	get_forensic_hair		args( (CHAR_DATA *ch) );
void 	set_natural_eyes		args( (CHAR_DATA *ch, char *argument) );
void    set_natural_hair        args( (CHAR_DATA *ch, char *argument) );
char *	get_natural_eyes		args( (CHAR_DATA *ch) );
char *	get_natural_hair		args( (CHAR_DATA *ch) );
char *	get_basic_hair			args( (CHAR_DATA *ch) );
char *  mask_intro				args( (CHAR_DATA *ch, int num) );
char *	format_obj_to_char		args( ( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort ) );
int 	get_height_feet			args( (CHAR_DATA *ch) );
char *	get_class_name			args( (CHAR_DATA *ch) );
void	inherit					args( (CHAR_DATA *ch) );
bool	has_focused				args( (CHAR_DATA *ch) );
bool	has_detail				args( (CHAR_DATA *ch) );
char *	get_focused				args( (CHAR_DATA *ch, CHAR_DATA *victim, bool xray) );
int 	clothes_count			args( (CHAR_DATA *ch) );
int		get_jewelry_cost		args( (CHAR_DATA *ch) );
int		get_clothes_cost		args( (CHAR_DATA *ch) );
int		get_height_mod			args( (CHAR_DATA *ch) );
int		get_bust_mod			args( (CHAR_DATA *ch) );
int 	daysidle				args( (char *name) );
int		offline_hours			args( (char *name) );
int 	get_attract				args( (CHAR_DATA *ch, CHAR_DATA *looker) );
bool	is_asleep				args( (CHAR_DATA *ch) );
bool	is_blind				args( (CHAR_DATA *ch) );
bool	is_deaf					args( (CHAR_DATA *ch) );
char * 	datesuffix				args( (int val) );
int     get_month               args( (void ) );
int		get_minute				args( ( void ) );
int		get_hour				args( (ROOM_INDEX_DATA *room ) );
int		get_last_hour			args( ( ROOM_INDEX_DATA *room ) );
int     get_day                 args( (void ) );
int		get_weekday				args( ( void ) );
int     get_last_day            args( ( void ) );
char *  get_whorank             args( (CHAR_DATA *ch) );
void	attract_logon			args( (CHAR_DATA *ch) );
void	attractive_update		args( (CHAR_DATA *ch) );
int 	get_bust				args( (CHAR_DATA *ch) );
int		sunphase_delayed		args( ( ROOM_INDEX_DATA *room ) );
int 	total_disc				args( (CHAR_DATA *ch) );
int 	total_skills			args( (CHAR_DATA *ch) );
char * 	get_inj					args( (CHAR_DATA *ch) );
int 	get_wordwrap			args( (CHAR_DATA *ch) );
char * 	get_fac					args( (CHAR_DATA *ch) );
int 	moon_pointer			args( (int strDay, int strMonth, int strYear, CHAR_DATA *ch) );
void	show_nearbyrooms		args( ( CHAR_DATA *ch) );
char *	displaysun				args( ( CHAR_DATA *ch) );
char *	displaysun_room				args( (ROOM_INDEX_DATA *room) );
int		sunphase				args( ( ROOM_INDEX_DATA *room ) );
int		get_warmth				args( ( CHAR_DATA *ch) );
bool    is_raining              args( ( ROOM_INDEX_DATA *room ) );
bool    is_hailing              args( ( ROOM_INDEX_DATA *room ) );
bool    is_snowing              args( ( ROOM_INDEX_DATA *room ) );
int		mist_level				args( ( ROOM_INDEX_DATA *room ) );
int 	temperature				args( ( ROOM_INDEX_DATA *room ) );
int 	cloud_cover				args( ( ROOM_INDEX_DATA *room ) );
int 	cloud_density			args( ( ROOM_INDEX_DATA *room ) );
bool	is_nudity				args( ( ROOM_INDEX_DATA *room) );
bool	is_favored				args( (CHAR_DATA *ch) );
int		get_hotliness			args( (CHAR_DATA *ch) );
bool	has_xray				args( (CHAR_DATA *ch) );
bool	has_omni				args( (CHAR_DATA *ch) );
int		max_scope				args( (CHAR_DATA *ch) );
void	show_obj_to_char		args( (OBJ_DATA *obj, CHAR_DATA *ch, bool photo, bool texts) );
bool	has_pierce				args( (CHAR_DATA *ch, int loc) );
bool    is_covered              args( (CHAR_DATA *ch, int sel) );
bool 	can_see_wear			args( (CHAR_DATA *ch, int iWear) );
void	set_title				args( ( CHAR_DATA *ch, char *title ) );
void    set_whotitle            args( ( CHAR_DATA *ch, char *whotitle ) );
void	set_ftime				args( ( CHAR_DATA *ch, char *ftime ) );
void	set_floc				args( ( CHAR_DATA *ch, char *floc  ) );
void	set_fdesc				args( ( CHAR_DATA *ch, char *fdesc ) );
void	noteToChar				args( ( CHAR_DATA *ch, char *info, char *note_to ) );
//void	note_to_char			args( ( CHAR_DATA *ch, char *info, char *note_to ) );
char *  condition_message		args( ( CHAR_DATA *ch) );
bool 	is_aikido				args( (int skillref) );
bool 	is_karate				args( (int skillref) );
bool 	is_kickboxing			args( (int skillref) );
bool 	is_kungfu				args( (int skillref) );
bool 	is_kick					args( (int skillref) );
bool 	is_punch				args( (int skillref) );
void 	add_to_last_on			args( (CHAR_DATA *ch) );
void 	remove_from_last_on		args( (CHAR_DATA *ch) );
void 	add_to_on_now			args( (CHAR_DATA *ch) );
void 	remove_from_on_now		args( (CHAR_DATA *ch) );
char *  draw_horizontal_line	 args( (int span     ) );

/* act_mob.c */
MEM_DATA *get_memory		args( (CHAR_DATA *npc, CHAR_DATA *pc) );
void 	 mob_remember		args( (CHAR_DATA *npc, CHAR_DATA *pc, long action) );
void 	 mob_forget		args( (CHAR_DATA *npc, MEM_DATA *mem) );
void 	 mob_mem_fade		args( (CHAR_DATA *npc) );

/* act_move.c */
char *		movement_message        args( (CHAR_DATA *ch, bool pushing, bool springing, int door) );
int			landmark_vnum			args( (char * landmark, CHAR_DATA *ch) );
bool 		door_locked				args( (EXIT_DATA *pexit, ROOM_INDEX_DATA *to_room, CHAR_DATA *ch) );
int 		move_speed				args( (CHAR_DATA *ch) );
int			get_absoldirection		args( (int truedir, int facing) );
int 		get_reldirection		args( (int truedir, int facing) );
int 		room_pop_mortals		args( (ROOM_INDEX_DATA *room) );
void		remote_view				args( (CHAR_DATA *ch) );
bool 		has_hellgate			args( (ROOM_INDEX_DATA *room) );
int 		travel_time				args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool		has_hotel_key			args( (CHAR_DATA *ch, int vnum) );
int			get_skill_cost			args( (int ref) );
bool		is_invited_door			args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) ); 
void		move_char				args( ( CHAR_DATA *ch, int door, bool follow, bool isFlee ) );
bool		has_skill				args( (CHAR_DATA *ch, int skill_ref) );
bool		has_skill_display       args( (CHAR_DATA *ch, int skill_ref) );
bool		has_growth				args( (CHAR_DATA *ch, int ref) );
int			highest_stat			args( (int ref) );
bool		can_see_char_distance	args( (CHAR_DATA *ch, CHAR_DATA *victim, int range) );
bool		can_see_room_distance	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *finish_room, int range) );
bool 		can_gain				args( (CHAR_DATA *ch, int ref) );
bool		can_jump				args( ( CHAR_DATA *ch, int difficulty ) );
int			room_distance_away      args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
CHAR_DATA * get_char_distance		args( (CHAR_DATA *ch, char * argument, int dist) );
int 		get_train_cost			args( (CHAR_DATA *ch, int stat) );
int			get_train_cost_mod		args( (CHAR_DATA *ch) );
int 		dark_total				args( (CHAR_DATA *ch) );
int			fire_total				args( (CHAR_DATA *ch) );
int			elec_total				args( (CHAR_DATA *ch) );
int			ice_total				args( (CHAR_DATA *ch) );
int			dark_pure				args( (CHAR_DATA *ch) );
bool		will_agg				args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 		to_home					args( (CHAR_DATA *ch) );
void		to_guest_home			args( (CHAR_DATA *ch, char *owner) );
void		real_quit				args( (CHAR_DATA *ch) );

/* act_obj.c */
bool 	has_imbuement		args( (CHAR_DATA *ch, int number) );
bool 	trade_good		args( (OBJ_DATA *obj) );
void	update_phonebook	args( (int number, char * owner) );
bool	has_cash		args( (CHAR_DATA *ch, int amount) );
void	take_cash		args( (CHAR_DATA *ch, int amount) );
char *  dropprefix              args( (OBJ_DATA *obj) );
void	start_vomit		args( (CHAR_DATA *ch) );
void    custom_to_obj		args( (CHAR_DATA *ch, OBJ_DATA *poi, bool colour) );
void	setdescs		args( (OBJ_DATA *obj) );
bool 	is_transperant		args( (OBJ_DATA *obj) );
void    unequip_char_silent     args( (CHAR_DATA*, OBJ_DATA*) );
CD *    find_keeper     	args( (CHAR_DATA *ch ) );
bool 	can_loot		args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void	wear_obj		args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool silent) );
bool	is_pair			args( (OBJ_DATA *obj) );
void    get_obj         	args( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container ) );
bool  	remove_obj      	args( (CHAR_DATA *ch, int iWear, bool fReplace ) );
OBJ_DATA *    get_jacket  args( (CHAR_DATA *ch));
OBJ_DATA *    get_pants  args( (CHAR_DATA *ch));

bool is_shadowspawn          args( ( CHAR_DATA *ch ) );
bool is_fade			args( ( CHAR_DATA *ch ) );
void    save_phonebooks              args( ( bool backup ) );
void    load_phonebooks              args( ( void ) );

/* act_wiz.c */
void	forum_update		args( ( void ) );
void	rp_test			args( ( char * text) );
void 	rp_log			args( ( char * text) );
void    save_petitions         args( ( void ) );
void    load_petitions         args( ( void ) );
CHAR_DATA *get_char_anywhere     args( (CHAR_DATA *ch, char *arg) );
bool	rename_account		args( (CHAR_DATA *ch, char * newname) );
bool	helpless_command	args( (char *command) );
void 	auto_copyover		args( (void) );
void    reboot_shutdown         args( (CHAR_DATA  *mudkiller, bool isreboot) );
void	etag			args( ( CHAR_DATA *ch) );
void 	set_whorank		args( ( CHAR_DATA *ch, char *whorank ) );
void 	wiznet			args( ( char *string, CHAR_DATA *ch, OBJ_DATA *obj,
			       		long flag, long flag_skip, int min_level ) );
void 	wiznet_command		args( ( char *string, CHAR_DATA *ch, OBJ_DATA *obj,
                                        long flag, long flag_skip, int min_level ) );
void 	copyover_recover 	args( ( void ) );
ROOM_INDEX_DATA * find_location   args( ( CHAR_DATA *ch, char *arg, int *m, int *x, int *y ));
bool	is_spyshield		args( ( CHAR_DATA *ch) );
/* alias.c */
void 	substitute_alias 	args( ( DESCRIPTOR_DATA *d, char *input ) );

/* auction.c */
void	auction_update		args( ( void ) );
void	auction_channel		args( ( char *msg ) );

/* arena.c */
void	challenge_update	args( ( void ) );
void 	check_duel_end		args( ( CHAR_DATA *ch ) );
void 	duel_end		args( ( CHAR_DATA *ch, bool bWin) );
void 	arena_channel		args( ( char * msg ) );

/* ban.c */
bool	check_ban		args( ( char *site, int type ) );

/* bitarray.c */
void 	SET_INIT		args( ( SET s ) );
void 	SET_COPY		args( ( SET to, const SET from ) );
void 	SET_AND_SET		args( ( SET s1, const SET s2 ) );
void 	SET_OR_SET		args( ( SET s1, const SET s2 ) );
void 	SET_XOR_SET		args( ( SET s1, const SET s2 ) );
void 	SET_REM_SET		args( ( SET s1, const SET s2 ) );
void 	SET_DIF			args( ( const SET s1, const SET s2, SET diff ) );
bool	SET_ISFLAG_SET		args( ( const SET s1, const SET s2 ) );
bool 	SET_IS_ZERO		args( ( const SET s1 ) );
bool 	set_flag_value		args( ( const struct flag_type *flag_table, char *argument, SET flag ) );
void	fread_bit_array 	args( ( FILE *fp, SET bit_array ) );
int	fread_bit_array_bit 	args( ( FILE *fp ) );
//char	*print_array_flags	args( ( const SET bit_array ) );
char	*set_print_flag		args( ( const SET bit_array ) );
void 	set_fread_flag		args( ( FILE *fp, SET bit_array ) );
void	bitvector_to_array	args( ( SET bit_array, long bitvector ) );

FEATURE_TYPE *	new_feature	args( ( void ) );

/* clans.c */

void grief_char			args( (char * name, int hours) );

void operative_outfit		args( (CHAR_DATA *ch, bool sranged, bool lranged, bool smelee, bool lmelee, bool armor) );

void    guest_match			args( (CHAR_DATA *ch) );
bool 	in_cult_domain		args( (CHAR_DATA *ch) );

bool 	in_clan			args( (CHAR_DATA *ch) );

bool    in_clan_two                 args( (CHAR_DATA *ch) );

const char * visible_goal	args( (int goal) );

bool 	char_in_alliance_with	args( (CHAR_DATA *ch, int cvnum) );
bool 	part_of_alliance	args( (CHAR_DATA *ch, char * astring) );

int     position_difference	args( (FACTION_TYPE *facone, FACTION_TYPE *factwo) );

bool    leadotherfac		args( (char * lname, int vnum) );

void	defeat_op_pc		args( (CHAR_DATA *ch) );
bool	visibly_online		args( (char * name) );
int 	intel_modifier		args( (CHAR_DATA *ch) );

void	give_intel		args( (CHAR_DATA *ch, int amount) );
bool    trust_elligible args( (CHAR_DATA *ch, FACTION_TYPE *fac, bool show, CHAR_DATA *display) );
void 	give_clan_power		args( (CHAR_DATA *ch, int amount) );
void	operation_swalk		args( (CHAR_DATA *ch) );
int	weather_bonus		args( (CHAR_DATA *ch) );
void 	prep_process		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void 	action_prep		args( (CHAR_DATA *ch, int type, char * argument) );
bool	camo_protect		args( (CHAR_DATA *ch) );
void	op_report		args( (char * argument, CHAR_DATA *ch) );
int	minalliancebalance	args( (int ftype) );
int	currentalliancebalance	args( (int ftype) );
int	battle_world		args( (void) );
int	manip_cost		args( (FACTION_TYPE *fac, int origcost) );
int 	char_secrecy		args( (CHAR_DATA *ch, CHAR_DATA *report) );
void	new_alliance		args( (int ftype) );
void	gain_resources		args( (int amount, int faction, CHAR_DATA *ch, char * message) );
void	use_resources		args( (int amount, int faction, CHAR_DATA *ch, char * message) );
char *	axes_names		args( (int axes, int position) );
char *	alliance_names		args( (int axes, int position, int type) );
bool	can_spend_resources		args( (FACTION_TYPE *fac) );
bool	shipment_carrier		args( (CHAR_DATA *ch) );
void 	remove_from_vassalroster	args( (char *name, int vnum) );
void	faction_antagonist_update args( ( void ) );
void	battlecheck		args( (CHAR_DATA *ch) );
bool	is_prisoner		args( (CHAR_DATA *ch) );
void	reclaim_items		args( (CHAR_DATA *ch) );
void	shipment_check		args( (FACTION_TYPE *fac) );
bool	is_infused		args( (CHAR_DATA *ch) );
bool	is_ripe			args( (CHAR_DATA *ch) );
bool	poidisplay		args( (CHAR_DATA *ch, int size, int mapy, int mapx) );
int	poitype			args( (CHAR_DATA *ch, int size, int mapy, int mapx) );
void	reset_faction_pay	args( (CHAR_DATA *ch, int faction) );
int 	offworld_timezone	args( ( int number) );
void	launch_operation	args( ( OPERATION_TYPE *op) );
char *	bag_name		args( ( void ) );
bool	capture_attack		args( (CHAR_DATA *ch) );
void	operation_second_update	args( ( void ) );
int 	get_poix		args( (CHAR_DATA *ch, int type) );
int	get_poiy		args( (CHAR_DATA *ch, int type) );
bool	poi			args( (CHAR_DATA *ch, int x, int y) );
char *	poichar			args( (CHAR_DATA *ch, int x, int y) );
void	displaypois		args( (CHAR_DATA *ch) );
void	operations_update	args( ( void ) );
int	max_operation_size	args( (int speed) );
void	breach_update		args( (CHAR_DATA *ch) );
void	patrol_check		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool 	generic_faction_vnum	args( (int number) );
bool	protected_faction_vnum	args( (int number) );
void 	switch_active_faction	args( (CHAR_DATA *ch) );
bool	generic_faction_vnum	args( (int number) );
bool	is_second		args( (CHAR_DATA *ch, int faction) );
int	total_covers		args( (FACTION_TYPE *fac) );
int	max_covers		args( (FACTION_TYPE *fac) );
int	clan_position		args( (CHAR_DATA *ch, int faction) );
void	coverup_cost		args( (CHAR_DATA *ch, int level) );
int	faction_pay		args( (CHAR_DATA *ch, int faction) );
void	faction_daily_update	args( ( void ) );
int	parent_fac		args( (CHAR_DATA *ch) );
ROOM_INDEX_DATA *deployment_room args( (CHAR_DATA *ch) );
FACTION_TYPE * clan_lookup_name	args( (char *argument) );
void	scout_report		args( (char * message) );
void	scout_report_temp	args( (char * message) );
void	last_public		args( (CHAR_DATA *ch, int vnum) );
void 	super_news		args( (char * message) );
char * 	get_origin		args( (CHAR_DATA *ch) );
bool	public_room		args( ( ROOM_INDEX_DATA *room) );
char *	smallest_total_string	args( ( void ) );
CABAL_TYPE *	get_cabal	args( (CHAR_DATA *ch) );
int     get_divnum      args( (CHAR_DATA *ch) );
char *	get_division_name		args( (CHAR_DATA *ch) );
bool	has_proxy		args( (CHAR_DATA *ch) );
bool	same_division		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int	get_rank		args( (CHAR_DATA *ch, int faction) );
char *	battle_owner		args( ( int battle ) );
void	start_battle		args( ( void ) );
void	winner_check		args( ( void ) );
void	end_battle		args( ( void ) );
void	defend_battle		args( ( void ) );
bool 	in_public		args( (CHAR_DATA *persecutor, CHAR_DATA *ch) );
void	process_order	   	args( (CHAR_DATA *ch, int type) );
void	give_respect		args( (CHAR_DATA *ch, int amount, char * reason, int faction) );
void	give_respect_noecho	args( (CHAR_DATA *ch, int amount, int faction) );
FACTION_TYPE * clan_lookup	args( (int vnum) );
void 	remove_from_clanroster	args( (char *name, int vnum) );
void 	reduce_manpower		args( ( int faction, char * target ) );
bool 	has_trust		args( (CHAR_DATA *ch, int trust, int faction) );
ROOM_INDEX_DATA * get_hq	args( (int vnum) );
void	send_message		args( (int faction, char *argument) );
void	send_log		args( (int faction, char * argument) );
void    send_message_temp       args( (int faction, char *argument) );
void	do_warcheck		args( ( void ) );
bool 	in_fac			args( (CHAR_DATA *ch, int faction) );
void	vassal_to_clan		args( (CHAR_DATA *ch, int vnum) );
void 	join_to_clan		args( (CHAR_DATA *ch, int vnum) );
void 	join_to_clan_two	args( (CHAR_DATA *ch, int vnum) );
void	broadcast		args( (int freq, char *arg) );
void 	entering_area		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to) );
void	kill_defender		args( ( int vnum) );
void	kill_aggressor		args( ( int vnum) );
int 	get_rep_person		args( ( CHAR_DATA *ch, int num) );
int	get_leader_bonus	args( ( CLAN_TYPE * clan) );
int	get_member_bonus	args( ( CLAN_TYPE * clan, CHAR_DATA *ch) );
char *	get_hero		args( ( void ) );
char *	get_villian		args( ( void ) );
void	clan_update		args( ( void ) );
void	clan_mini_update	args( ( void ) );
bool	is_hero			args( ( CHAR_DATA *ch ) );
bool	is_villian		args( ( CHAR_DATA *ch) );
bool	is_sub_hero		args( ( CHAR_DATA *ch ) );
bool 	is_sub_villian		args( ( CHAR_DATA *ch ) );
bool	is_leader		args( ( CHAR_DATA *ch, int faction) );
void    save_clans              args( ( bool backup ) );
void    load_clans              args( ( void ) );
void	save_operations		args( ( bool backup ) );
void	load_operations		args( ( void ) );
void	save_cabals		args( ( bool backup ) );
void	load_cabals		args( ( void ) );
void	save_blackmarket	args( ( void ) );
void  	load_blackmarket	args( ( void ) );
FACTION_TYPE * get_clan		args( ( CHAR_DATA *ch) );
void	give_tribute		args( ( CHAR_DATA *ch, int amount, int align) );

/* events.c */

void aid_scheme			args( (CHAR_DATA *ch, int amount) );
int get_completion		args( (EVENT_TYPE *event) );
int skillpoint			args( ( int val) );
int number_events		args( ( void ) );
int number_gatherings		args( ( void ) );
void add_event_log		args( ( CHAR_DATA *ch, char * argument) );
void event_kill_check		args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void event_update		args( ( void ) );
void load_events		args( ( void ) );
void save_events		args( ( void ) );

void load_gatherings                args( ( void ) );
void save_gatherings                args( ( void ) );
void gathering_update		args( ( void ) );

/* comm.c */
bool isInvalid			args( (const void* address) );

int blessing_cost		args( (CHAR_DATA *ch, int bless, int curse) );
char * tyr_lor			args( (Buffer &buffer, const char *intxt, DESCRIPTOR_DATA *d, int ctype) );
void fread_account		args( (ACCOUNT_TYPE *account, FILE *fp) );
#if defined(_WIN32)
char *LastErrorStr args( (void) );
void gettimeofday (struct timeval *t, void *tz);
#endif
bool    check_parse_name        args( ( char *name ) );
void	show_string	args( ( struct descriptor_data *d, char *input) );
void	close_socket	args( ( int fd ) );
void	close_desc		args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt, int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void    send_to_eavesdrop args ( ( const char *txt, ROOM_INDEX_DATA *room ) );
void	page_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	act		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );
void	act_new		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type,
			    int min_pos) );
void	visualact	args( ( const char * format, CHAR_DATA *ch, const void *aarg1, const void *arg2, int type) );
void	printf_to_char	args( ( CHAR_DATA *, char *, ... ) );
int	chprintf	args( ( CHAR_DATA* ch, const char *format, ... ) );
void	bugf		args( ( char *, ... ) );
void    dact            args( ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int range) );
void	pact		args( ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int xcoord, int ycoord, int dist, int optx, int opty) );
void	send_timer	args( ( struct timerset *vtime, CHAR_DATA *ch ) );
int    	color          	args( ( char type, CHAR_DATA *ch, char *string ) );
char *	remove_color	args( ( char *buffer, const char *txt ) );
char *	remove_newlines	args( ( char * buffer, const char *txt) );
char *	from_color	args( (char * argument) );
char *	from_details	args( (char * argument) );
void   	colorconv      	args( ( Buffer &buffer, const char *txt, CHAR_DATA *ch ) );
void  	colorconv_desc 	args( ( Buffer &buffer, const char *txt, DESCRIPTOR_DATA *d ) );

int	strswitch	args( ( const char* arg, ... ) );
void	vinterpret	args( ( CHAR_DATA *ch, char *argument, ... ) );
void	sphere_choice	args((DESCRIPTOR_DATA *d, CHAR_DATA *ch, char *argument));
int     color           args((char type, CHAR_DATA *ch, char *string));


/* db.c */
// safe_strlen_color - eliminates color codes from regular str count
char *  get_cover_list          args( (int ref) );
int	    safe_strlen_color	        args( ( const char *argument ) ); // Palin 1999
int	    safe_strlen_disco_lor	    args( ( char string[MSL] ) );
void	reset_area              args( ( AREA_DATA * pArea, bool runtime ) );		/* OLC */
void	reset_room	            args( ( ROOM_INDEX_DATA *pRoom, bool runtime ) );	/* OLC */
void	save_area_list	        args( ( void ) );
char *	print_flags	            args( ( int flag ));
void	boot_db		            args( ( void ) );
void	area_update	            args( ( bool runtime ) );
CD *	create_mobile	        args( ( MOB_INDEX_DATA *pMobIndex ) );
void	clone_mobile	        args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
OD *	create_object	        args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clone_object	        args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void	clear_char	            args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	        args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
char *  get_extra_descr_obj     args( ( const char *name, EXTRA_DESCR_DATA *ed, OBJ_DATA *obj ) );
MID *	get_mob_index	        args( ( int vnum ) );
GROUP_INDEX_DATA * get_group_index	args( ( int vnum ) );
OID *	get_obj_index	        args( ( int vnum ) );
RID *	get_room_index	        args( ( int vnum ) );
PC *	get_prog_index	        args( ( int vnum, int type ) );
char	fread_letter	        args( ( FILE *fp ) );
int	    fread_number	        args( ( FILE *fp ) );
long 	fread_flag	            args( ( FILE *fp ) );
char *	fread_string	        args( ( FILE *fp ) );
char *  fread_string_eol        args(( FILE *fp ) );
void	fread_to_eol	        args( ( FILE *fp ) );
char *	fread_word	            args( ( FILE *fp ) );
long	flag_convert	        args( ( char letter) );
void *	alloc_mem	            args( ( size_t sMem ) );
void *	alloc_perm	            args( ( size_t sMem ) );
void	free_mem	            args( ( void *pMem, size_t sMem ) );
char *	str_dup		            args( ( const char *str ) );
void	free_string	            args( ( char *pstr ) );
int	    number_fuzzy	        args( ( int number ) );
int	    number_range	        args( ( int from, int to ) );
int	    number_percent	        args( ( void ) );
int	    number_door	            args( ( void ) );
int	    number_bits	            args( ( int width ) );
long 	number_mm               args( ( void ) );
int	    dice		            args( ( int number, int size ) );
int	    interpolate	            args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	            args( ( char *str ) );
void	smash_vector	        args( ( char *str ) );
char *	smash_MXP               args( ( char *str ) );
void	smash_percent	        args( ( char *str ) );
bool	str_cmp		            args( ( const char *astr, const char *bstr ) );
bool	str_prefix	            args( ( const char *astr, const char *bstr ) );
bool	str_infix	            args( ( const char *astr, const char *bstr ) );
bool	str_suffix	            args( ( const char *astr, const char *bstr ) );
char *	capitalize	            args( ( const char *str ) );
void	append_file	            args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		                args( ( const char *str, int param ) );
void	log_string	            args( ( const char *str ) );
void	tail_chain	            args( ( void ) );

/*institute.c*/
void handout_lifeforce_summary	args( (CHAR_DATA *ch, int perc)  );
void bloodbag_transfer			args( (CHAR_DATA *ch, CHAR_DATA *dest) );
int college_house_room			args( (ROOM_INDEX_DATA *room) );
void college_roster				args ((CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
int 	college_house			args( (CHAR_DATA *ch) );
int 	dorm_room			args (( CHAR_DATA *ch) );
int 	college_group			args( (CHAR_DATA *ch, bool countsuspended) );
bool	institute_staff		        args( (CHAR_DATA *ch) );
bool	institute_suspension		  args( (CHAR_DATA *ch) );
void  commit_char               args( (CHAR_DATA *ch) );
bool has_clinic_power_chars	  args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	victimize_emote_process   args( (CHAR_DATA *ch) );
void	process_victim_number     args( (CHAR_DATA *ch, int number) );
void	victim_prompt			        args( (CHAR_DATA *ch) );
void	process_victim_choice     args( (CHAR_DATA *ch, int number) );
bool	is_char_prefect			      args( (CHAR_DATA *ch) );
bool	is_char_headboy			      args( (CHAR_DATA *ch) );
bool	is_char_headgirl		      args( (CHAR_DATA *ch) );
bool	is_char_headjock		      args( (CHAR_DATA *ch) );
void	solitary_check			      args( (CHAR_DATA *ch) );
void	clinic_charge			        args( (CHAR_DATA *ch) );
void 	wanted_check			        args( (CHAR_DATA *ch) );
bool 	can_institute_teach		    args( (CHAR_DATA *ch) ) ;
bool  is_institute_taught       args( (CHAR_DATA *victim) );
void	institute_xp			        args( (CHAR_DATA *ch, int amount) );
bool 	college_student		        args( (CHAR_DATA *ch, bool countsuspended) );
bool	college_staff		          args( (CHAR_DATA *ch, bool countsuspended) );
bool  institute_contractor      args( (CHAR_DATA *ch));
bool	academy_student		        args( (CHAR_DATA *ch) );
bool 	academy_staff		          args( (CHAR_DATA *ch, bool countsuspended) );
bool	clinic_patient		        args( (CHAR_DATA *ch) );
bool	clinic_staff		          args( (CHAR_DATA *ch, bool countsuspended) );
void	institute_update	        args( (void) );
void  save_institutes           args( ( bool backup ) );
void  load_institutes           args( ( void ) );
int   college_staff_rank        args((CHAR_DATA *ch));
int   clinic_staff_rank         args((CHAR_DATA *ch));
int   academy_staff_rank        args((CHAR_DATA *ch));

/*Patrols.c*/

bool valid_pc_prey		args( (CHAR_DATA *ch, CHAR_DATA *pred, bool isauto) );
bool fixation_target		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool fixation_happy_room	args( (CHAR_DATA *ch) );
void process_prey_emote		args( (CHAR_DATA *ch));
void process_villain_emote	args( (CHAR_DATA *ch) );

void 	logevent			args( (ROOM_INDEX_DATA *room, int type, int subtype, char *argument, CHAR_DATA *ch) );
ROOM_INDEX_DATA * make_victim_room	args( (CHAR_DATA *ch) );

bool  in_detention                      args( (CHAR_DATA *ch) );
bool  room_in_detention                 args( (ROOM_INDEX_DATA *room) );
bool	institute_staff		                args( (CHAR_DATA *ch) );
bool	institute_suspension		          args( (CHAR_DATA *ch) );
bool	institute_suspension_IMMORTAL		  args( (CHAR_DATA *ch) );
void  clear_institute_rank              args( (CHAR_DATA *ch) );
void  commit_char                       args( (CHAR_DATA *ch) );
void	victimize_emote_process           args( (CHAR_DATA *ch) );
void	process_victim_number             args( (CHAR_DATA *ch, int number) );
void	victim_prompt			                args( (CHAR_DATA *ch) );
void	process_victim_choice             args( (CHAR_DATA *ch, int number) );
bool	is_char_prefect			              args( (CHAR_DATA *ch) );
bool	is_char_headboy			              args( (CHAR_DATA *ch) );
bool	is_char_headgirl		              args( (CHAR_DATA *ch) );
bool	is_char_headjock		              args( (CHAR_DATA *ch) );
void	solitary_check			              args( (CHAR_DATA *ch) );
void	clinic_charge			                args( (CHAR_DATA *ch) );
void 	wanted_check			                args( (CHAR_DATA *ch) );
bool 	can_institute_teach		            args( (CHAR_DATA *ch) ) ;
bool  is_institute_taught               args( (CHAR_DATA *victim) );
void	institute_xp			                args( (CHAR_DATA *ch, int amount) );
bool 	college_student		                args( (CHAR_DATA *ch, bool countsuspended) );
bool	college_staff		                  args( (CHAR_DATA *ch, bool countsuspended) );
bool  institute_contractor              args( (CHAR_DATA *ch));
bool	academy_student		                args( (CHAR_DATA *ch) );
bool 	academy_staff		                  args( (CHAR_DATA *ch, bool countsuspended) );
bool	clinic_patient		                args( (CHAR_DATA *ch) );
bool	clinic_staff		                  args( (CHAR_DATA *ch, bool countsuspended) );
void	institute_update	                args( (void) );
void  save_institutes                   args( ( bool backup ) );
void  load_institutes                   args( ( void ) );
int   college_staff_rank                args((CHAR_DATA *ch));
int   clinic_staff_rank                 args((CHAR_DATA *ch));
int   academy_staff_rank                args((CHAR_DATA *ch));

/*Patrols.c*/
ROOM_INDEX_DATA * graveyard_room args( (void) );
bool	only_faction		args( (CHAR_DATA *ch) );
bool	only_allies		args( (CHAR_DATA *ch) );
int 	min_hours		args( (int type) );
char *	event_title		args( (int type, int subtype, ROOM_INDEX_DATA *room) );
void    logevent_check		args( (CHAR_DATA *ch, char * argument) );
void	autooutfit		args( (CHAR_DATA *ch, int number) );
void	autogear		args( (CHAR_DATA *ch, bool dreaming) );
bool	free_to_act		args( (CHAR_DATA *ch) );
void	weekly_process		args( ( void ) );
void	add_to_weekly_characters args( (CHAR_DATA *ch) );
void	load_weekly_characters	args( ( void ) );
void	save_weekly_characters	args( ( void ) );
void	patrol_update		args( ( CHAR_DATA *ch ) );
void	patrol_launch_check	args( ( CHAR_DATA *ch ) );
ROOM_INDEX_DATA * mist_duplicate_room args( (ROOM_INDEX_DATA *orig) );
void	patrol_personal_award	args( (CHAR_DATA *ch, int type) );
void	finish_arcane_assault	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	same_faction	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );

//recycle.c
GROUPTEXT_TYPE * new_grouptext		args( ( void ) );
TEXTHISTORY_TYPE * new_texthistory		args( ( void ) );

CHATROOM_TYPE * new_chatroom		args( ( void ) );
PROFILE_TYPE * new_profile		args( ( void ) );
MATCH_TYPE * new_match		args( ( void ) );


NEWDESTINY_TYPE * new_destiny		args( ( void ) );
void	free_destiny			args( (NEWDESTINY_TYPE *destiny) );
PHONEBOOK_TYPE * new_phonebook      args( ( void ) );
void    free_phonebook            args( (PHONEBOOK_TYPE *phonebook) );
GATEWAY_TYPE * new_gateway	args( ( void ) );
void	free_gateway		args( (GATEWAY_TYPE *gateway) );
WEEKLY_TYPE * new_weekly	args( ( void ) );
void	free_weekly		args( (WEEKLY_TYPE *weekly) );
EVENT_TYPE * new_event          args( ( void ) );
void    free_event              args( (EVENT_TYPE *event) );
FACTION_TYPE * new_faction          args( ( void ) );
void    free_faction              args( (FACTION_TYPE *fac) );
INSTITUTE_TYPE * new_institute	args( ( void) );
void	free_institute		args( (INSTITUTE_TYPE *ins) );
CABAL_TYPE * new_cabal		args( ( void ) );
void	free_cabal		args( (CABAL_TYPE *cabal) );
GRAVE_TYPE * new_grave		args( ( void ) );
void	free_grave		args( (GRAVE_TYPE *grave) );
LAIR_TYPE * new_lair		args( ( void ) );
void	free_lair		args( (LAIR_TYPE *lair) );
ARCHIVE_INDEX * new_archive_index args( ( void ) );
void	free_archive_index	args( (ARCHIVE_INDEX *arch) );
ARCHIVE_ENTRY * new_archive_entry args( ( void ) );
void    free_archive_entry      args( (ARCHIVE_ENTRY *arch) );
ROSTERCHAR_TYPE *new_rosterchar	args( ( void ) );
void	free_rosterchar		args( (ROSTERCHAR_TYPE *rosterchar) );
GALLERY_TYPE *new_gallery args( ( void ) );
void    free_gallery         args( (GALLERY_TYPE *gallery) );
PAGE_TYPE *new_page args( ( void ) );
void    free_page         args( (PAGE_TYPE *page) );
STORYLINE_TYPE *new_storyline args( ( void ) );
void    free_storyline         args( (STORYLINE_TYPE *storyline) );
HOUSE_TYPE * new_house		args( ( void ) );
void	free_house		args( (HOUSE_TYPE *house) );
DOMAIN_TYPE * new_domain            args( ( void ) );
void    free_domain               args( ( DOMAIN_TYPE *domain) );

PROP_TYPE * new_prop		args( ( void ) );
void	free_prop		args( ( PROP_TYPE *prop) );
DUNGEON_TYPE * new_dungeon	args( ( void ) );
void	free_dungeon		args( (DUNGEON_TYPE *dungeon) );
SHOP_TYPE *new_pshop		args( ( void ) );
void	free_pshop		args( (SHOP_TYPE *shop) );
PLAYERROOM_TYPE * new_playerroom          args( ( void ) );
void    free_playerroom              args( (PLAYERROOM_TYPE *playerroom) );
STORY_TYPE * new_story          args( ( void ) );
void    free_story              args( (STORY_TYPE *story) );
PETITION_TYPE * new_petition          args( ( void ) );
void    free_petition              args( (PETITION_TYPE *pet) );
OPERATION_TYPE * new_operation		args( ( void) );
void	free_operation		   args( (OPERATION_TYPE *op) );
PLOT_TYPE * new_plot          args( ( void ) );
void    free_plot              args( (PLOT_TYPE *plot) );
FANTASY_TYPE * new_fantasy	args( ( void ) );
void	free_fantasy		args( (FANTASY_TYPE *story) );
STORYIDEA_TYPE * new_storyidea	args( ( void ) );
void	free_storyidea		args( (STORYIDEA_TYPE *story) );
RESEARCH_TYPE * new_research          args( ( void ) );
void    free_research              args( (RESEARCH_TYPE *research) );
EXTRA_ENCOUNTER_TYPE * new_eencounter          args( ( void ) );
void    free_eencounter              args( (EXTRA_ENCOUNTER_TYPE *enc) );

EXTRA_PATROL_TYPE * new_epatrol          args( ( void ) );
void    free_epatrol              args( (EXTRA_PATROL_TYPE *pat) );

ANNIVERSARY_TYPE * new_anniversary          args( ( void ) );
void    free_anniversary              args( (ANNIVERSARY_TYPE *ann) );


DECREE_TYPE * new_decree          args( ( void ) );
void    free_decree              args( (DECREE_TYPE *dec) );


NEWS_TYPE * new_news          args( ( void ) );
void    free_news              args( (NEWS_TYPE *news) );
LOCATION_TYPE *new_location	args( ( void ) );
void	free_location		args( (LOCATION_TYPE *location) );
ACCOUNT_TYPE *new_account	args( ( void ) );
void	free_account		args( (ACCOUNT_TYPE *account) );

void	 populate_warren	args( (CHAR_DATA *ch) );


void	load_social_table	args( ( void ) );
void	save_social_table	args( ( void ) );
void	save_area		args( ( AREA_DATA *pArea, bool backup ) );

/* dungeons.c */
void	arti_check		args( ( OBJ_DATA *obj) );
bool	room_empty		args( (ROOM_INDEX_DATA *room) );
void	transfer_artifact	args( (CHAR_DATA *victim, int vnum) );
bool	is_artifact		args( ( int vnum) );
int	number_artifacts	args( ( CHAR_DATA *ch) );
void    load_artifacts           args( (void) );
void    save_artifacts           args( (void) );
void	dungeon_clear		args( ( void) );
void	dungeon_second_update	args( ( void) );
void	dungeon_hospitalise	args( (CHAR_DATA *victim ) );
void	nemesis_update		args( ( CHAR_DATA *ch) );
void	nemesis_profile		args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	dungeon_walk_update	args( ( void ) );
void	check_dungeon_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
/* db2.c */
void 	save_races	args( ( void ) );
void 	save_subraces	args( ( void ) );
void 	save_vessels	args( ( void ) );
void	save_ports	args( ( void ) );
bool	go_sector	args( ( int Sector, int Vessel_type, bool is_pc ) );

/* effect.c */
void	acid_effect	args( ( void *vo, int level, int dam, int target ) );
void	cold_effect	args( ( void *vo, int level, int dam, int target ) );
void	fire_effect	args( ( void *vo, int level, int dam, int target ) );
void	poison_effect	args( ( void *vo, int level, int dam, int target ) );
void	shock_effect	args( ( void *vo, int level, int dam, int target ) );

/* fame.c */
char * 	named_rank	args( ( CHAR_DATA *ch) );
void	fame_update	args( ( CHAR_DATA *ch) );
void	load_fame	args( ( void ) );
void	save_fame	args( ( void ) );


/* fatality.c */
void    do_fatality               args( ( CHAR_DATA *ch, CHAR_DATA *victim) );

/* fight.c */
OBJ_DATA *get_lmelee args((CHAR_DATA * ch));
OBJ_DATA *get_smelee args((CHAR_DATA * ch));
OBJ_DATA *get_lranged args((CHAR_DATA * ch));
OBJ_DATA *get_sranged args((CHAR_DATA * ch));
bool 	overwhelm			args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool 	pvp_character      	args( (CHAR_DATA *ch) );
bool	monster_fight		args( (CHAR_DATA *ch) );
bool	bodyguard_abduct	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	combat_damage		args( (CHAR_DATA *victim, CHAR_DATA *ch, int amount, int disc) );
bool	is_sparring		args( (CHAR_DATA *ch) );
CHAR_DATA * get_cover   args( (CHAR_DATA *ch) );

ROOM_INDEX_DATA * aheadroom	args( (int direction, ROOM_INDEX_DATA *origin) );
int 	char_power_score	args( (CHAR_DATA *mob) );
int 	mob_power_score		args( (MOB_INDEX_DATA *mob) );
CHAR_DATA * next_fight_member	args( (CHAR_DATA *current));
void 	join_to_fight		args( (CHAR_DATA *rch) );
void	demon_spawn		args( (ROOM_INDEX_DATA *spawnroom) );
int 	large_weapon_count	args( (CHAR_DATA *ch) );
int 	get_demon_lvl		args( (int pointer) );
CHAR_DATA * find_abductee	args( (CHAR_DATA *mob) );
CHAR_DATA * find_prey		args( (CHAR_DATA *mob) );
bool	forest_monster		args( (CHAR_DATA *ch) );
void    start_fight     args( (CHAR_DATA *ch, CHAR_DATA *target) );
bool 	nearby_deep		args( (ROOM_INDEX_DATA *room) );
bool	no_deep_access		args( (ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to) );
int 	map_expand		args( (int number) );
void	scan_fight		args( (CHAR_DATA *ch, bool mapcount) );
bool	has_moved		args( (CHAR_DATA *ch) );
void	fall_character		args( (CHAR_DATA *ch) );
void	setup_translation	args( ( void ) );
bool	can_get_to		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *desti) );
bool	same_fight		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int	relative_x		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *desti, int x) );
int     relative_y              args( (CHAR_DATA *ch, ROOM_INDEX_DATA *desti, int y) );
int     relative_z              args( (CHAR_DATA *ch, ROOM_INDEX_DATA *desti) );
int	combat_distance		args( (CHAR_DATA *ch, CHAR_DATA *victim, bool attacking) );
void	move_relative		args( (CHAR_DATA *ch, int x, int y, int z) );
void	start_hostilefight	args( (CHAR_DATA *ch) );
int     get_speed       	args( (CHAR_DATA *ch) );
int	muscle_exp_cap		args( (CHAR_DATA *ch) );
int 	minion_exp_cap		args( (CHAR_DATA *ch) );
int	minion_shield_cap	args( (CHAR_DATA *ch) );
int	built_disc_cost		args( (int value) );
int	current_minion_cost	args( (CHAR_DATA *ch, int x) );
int	current_minion_shield	args( (CHAR_DATA *ch, int x) );
bool    battleground		args( (ROOM_INDEX_DATA *room) );
int 	default_ranged		args( (CHAR_DATA *ch) );
int default_base_ranged		args( (CHAR_DATA *ch) );
int	dam_cap		args( (int damage, int cap) );
void	round_process		args( (CHAR_DATA *ch) );
char *	battleflags		args( (CHAR_DATA *ch, CHAR_DATA *pers) );
CHAR_DATA * closest_character	args( (CHAR_DATA *ch) );
bool	is_cover		args( (CHAR_DATA *ch) );
void    move_away       args( (CHAR_DATA *ch, int tox, int toy, int dist, int z, bool voluntary) );
void	explode_grenade		args( (CHAR_DATA *ch) );
bool	can_map_see		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void    summon_cobj     	args( (ROOM_INDEX_DATA *room, int vnum, int duration, int x, int y, CHAR_DATA *summoner) );
void	nomove			args( (CHAR_DATA *ch) );
void	noattack		args( (CHAR_DATA *ch) );
void	apply_caff		args( (CHAR_DATA *ch, int aff, int rounds) );
bool	has_caff		args( (CHAR_DATA *ch, int aff) );
void	remove_caff		args( (CHAR_DATA *ch, int aff) );
int 	get_mob_x		args( (int facing, ROOM_INDEX_DATA *room) );
int     get_mob_y               args( (int facing, ROOM_INDEX_DATA *room) );
void	end_fight		args( (ROOM_INDEX_DATA *room) );
void 	create_crystal		args( (ROOM_INDEX_DATA *room, int level) );
bool 	is_in_cover		args( (CHAR_DATA *ch) );
void    critplayer      	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool 	is_sparring_room	args( (ROOM_INDEX_DATA *room) );
int	get_flee_direction	args( (CHAR_DATA *ch) );
int 	get_exit_x		args( (int dir, ROOM_INDEX_DATA *room) );
int 	get_exit_y		args( (int dir, ROOM_INDEX_DATA *room) );
CHAR_DATA *get_npc_target	args( (CHAR_DATA *ch) );
CHAR_DATA *get_npc_target_new	args( (CHAR_DATA *ch) );

bool 	fight_over		args( (ROOM_INDEX_DATA *room) );
void 	copspank		args( (CHAR_DATA *ch) );
void 	killplayer		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	kidnap			args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int     max_specials    args( (CHAR_DATA *ch, int discipline) );
void	copcheck		args( ( CHAR_DATA *ch ) );
char * 	get_special_string	  args( (int val) );
char * 	get_disc_string		args ( (int val) );
bool 	room_fight		args( (ROOM_INDEX_DATA *room, bool shroud, bool deepshroud, bool any) );
bool 	in_fight		args( (CHAR_DATA *ch) );
bool	check_fight		args( (CHAR_DATA *ch) );
void	start_fight		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	start_roomfight		args( (ROOM_INDEX_DATA *room, bool fast, int speed, bool shroud, bool deepshroud) );
void	move_message		args( (CHAR_DATA *ch, int type, int originalx, int originaly, ROOM_INDEX_DATA *origroom) );
void 	move_towards		args( (CHAR_DATA *ch, int tox, int toy, int dist, int z, bool voluntary) );
bool	fast_combat		args( ( CHAR_DATA *ch) );
void	npc_combat_move		args( ( CHAR_DATA *ch) );
void	npc_combat_attack	args( ( CHAR_DATA *ch) );
void	real_kill		args( (CHAR_DATA *victim, CHAR_DATA *killer) );
void	real_kill_headless	args( (CHAR_DATA *victim, CHAR_DATA *killer) );
void	process_emoteattack	args( ( CHAR_DATA *ch) );
void	next_attacker		args( ( CHAR_DATA *ch, bool initial) );
void	regened_message		args( ( CHAR_DATA *ch) );
int 	get_dist		args( ( int xone, int yone, int xtwo, int ytwo) );
bool 	in_combat		args( ( CHAR_DATA *ch) );
void	mobstrike		args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	mobstrikes		args( ( CHAR_DATA *ch) );
void	do_rush			args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	do_round		args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool	mortal_tick		args ( ( CHAR_DATA *victim) );
void	aggro_kill_check	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	true_death	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool	is_in_pk_hist	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	add_to_pk_hist	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void 	slay_message args ( (CHAR_DATA *ch, CHAR_DATA *victim) );
char *  aligncolor      args( (CHAR_DATA *ch) );
int     get_slay_quality args( (CHAR_DATA *ch) );
int	get_explored_rating	args( ( CHAR_DATA *ch ) );
void    do_unconcious   args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void    fight_death     args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	fire_arrow	args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool fShow, bool AutoDraw ) );
bool	draw_arrow	args( ( CHAR_DATA *ch, char *argument, bool fShow ) );
void	punch		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	not_nullified	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int	diminish	args( (int num, int slope) );
char * 	get_throw_message args( (int sector) );
bool    has_sword       args( (CHAR_DATA *ch) );
bool    has_dagger      args( (CHAR_DATA *ch) );
bool    has_axe         args( (CHAR_DATA *ch) );
bool	has_mace	args( (CHAR_DATA *ch) );
int     get_num_weapons args( (CHAR_DATA *ch) );
bool	fight_paused	args( (ROOM_INDEX_DATA *room) );
OBJ_DATA *get_weapon	args( (CHAR_DATA *ch) );
bool	has_weapon	args( (CHAR_DATA *ch) );
void    throw_char	args( (CHAR_DATA *ch, CHAR_DATA *victim, int door, int strength) );
void	make_dirty	args( (CHAR_DATA *ch ) );
void    apply_marked      args( (CHAR_DATA *ch, int length ) );

// Put this back in for the real one_hit function.
bool    is_tank         args( ( CHAR_DATA *ch ) );
bool	same_tier	args( ( CHAR_DATA *ch, CHAR_DATA *gch ) );
void    one_hit         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int WEAR_LOC, bool isSkill, float speed_mod, double mult) );
void 	weapon_speed	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int WEAR_LOC ) );
void    group_gain      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

bool    check_dodge     	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_parry     	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) ); 
bool    check_acrobatics 	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	check_block_weave 	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void	bonus_attacks	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int WEAR_LOC ) );
void    raw_kill        args( ( CHAR_DATA *victim, CHAR_DATA *killer) );
bool 	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool 	is_safe_spell	args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool area ) );
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	damage		args( (CHAR_DATA *victim, CHAR_DATA *ch, int amount) );
bool    damage_old      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt, int dam_type, bool show ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );

void	set_suspicious	args( ( CHAR_DATA *victim, CHAR_DATA *ch) );
int     bsmod			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	weapon_damage	args( ( CHAR_DATA *ch, OBJ_DATA *wield ) );

/* ghosts.c */
void	gravevisit		args( (CHAR_DATA *ch, char * argument) );
void	show_grave		args( (CHAR_DATA *ch, char * argument) );
bool	gravename		args( (CHAR_DATA *ch, char * argument) );
void	show_graves		args( (CHAR_DATA *ch) );
void	show_plaques		args( (CHAR_DATA *ch) );
bool	graveroom		args( (ROOM_INDEX_DATA *room) );
bool	plaqueroom		args( (ROOM_INDEX_DATA *room) );
bool	corpsecarrier		args( (CHAR_DATA *ch) );
void	trashcorpse		args( (CHAR_DATA *ch) );
void	save_graves		args( (bool backup) );
void	load_graves		args( ( void ) );
bool    deplete_ghostpool       args( (CHAR_DATA *ch, int value) );
void    refill_ghostpool        args( (CHAR_DATA *ch, int value) );
bool    is_visible              args( (CHAR_DATA *ch) );
bool    is_ghost                args( (CHAR_DATA *ch) );
bool    is_dead                 args( (CHAR_DATA *ch) );
bool    is_manifesting          args( (CHAR_DATA *ch) );
bool    can_hear_ghost		args( (CHAR_DATA *speaker, CHAR_DATA *listener) );
void    ghost_drop              args( (CHAR_DATA *ch) );
void    refresh_ghost_abilities args( (CHAR_DATA *ch) );
bool	is_possessing		args( (CHAR_DATA *ch) );
bool	is_possessed		args( (CHAR_DATA *ch) );
CHAR_DATA *	get_possesser	args( (CHAR_DATA *ch) );

/* houses.c */
bool    sandbox_room		args((ROOM_INDEX_DATA *room));
int 	get_security 		args((ROOM_INDEX_DATA * room));
int 	get_decor 		args((ROOM_INDEX_DATA * room));
int 	get_toughness 		args((ROOM_INDEX_DATA * room));

void 	encroach_property	args( (PROP_TYPE *prop) );
void	security_wipeout	args( (char * cname) );
bool	protected_room		args( (ROOM_INDEX_DATA *room) );
ROOM_INDEX_DATA *get_fleeroom	args( (CHAR_DATA *ch, PROP_TYPE *exclude) );

bool	homeowner		args( (CHAR_DATA *ch) );
bool	shopowner		args( (CHAR_DATA *ch) );
int     district_room           args( (ROOM_INDEX_DATA *room) );
void	market_update		    args( (void) );
ROOM_INDEX_DATA *first_room_in_property	args( (PROP_TYPE *prop) );
void	reclaim_property	    args( (PROP_TYPE *prop));
void	reclaim_room		    args( (ROOM_INDEX_DATA *room) );
PROP_TYPE * prop_lookup_shop	args( (int vnum));
PROP_TYPE * prop_lookup_house	args( (int vnum));
void	encroachment_update		args( (void) );
void	encroachment_cleanup	args( (void) );
void	autohouseflee			args( (char * name, PROP_TYPE *prop, char * creepname) );
void	to_sleepers				args( (CHAR_DATA *ch, PROP_TYPE *prop) );
void	from_sleepers			args( (CHAR_DATA *ch, PROP_TYPE *prop) );
void	dawn_update				args( (void) );
int		get_toughness			args( (ROOM_INDEX_DATA *room) );
int		get_decor				args( (ROOM_INDEX_DATA *room) );
bool	base_room				args( (ROOM_INDEX_DATA *room) );
void	worker_update			args( ( void ) );
char *	job_name				args( (CHAR_DATA *ch, int number) );
int 	property_roomcount		args( (PROP_TYPE *prop) );
bool	is_base					args( (PROP_TYPE *prop) );
PROP_TYPE *	get_base			args( (int faction) );
PROP_TYPE * prop_from_room		args( (ROOM_INDEX_DATA *room) );
bool	housesleeper			args( (CHAR_DATA *ch) );
void    make_exit       		args( (int vnum_one, int vnum_two, int dir, int type) );
void	reset_prop_room 		args( (ROOM_INDEX_DATA *room) );
void	blackout_update			args( (void) );
bool	shop_owner				args( (CHAR_DATA *ch) );
HOUSE_TYPE * room_house			args( (ROOM_INDEX_DATA *room) );
void	save_rent				args( (void) );
void	load_rent				args( (void) );
void	rent_update				args( (void) );
bool	bblocked				args( (ROOM_INDEX_DATA *to_room, CHAR_DATA *ch) );
void	shop_profits			args( (CHAR_DATA *ch) );
bool	can_decorate			args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
PROP_TYPE * in_prop				args( (CHAR_DATA *ch) );
bool	apartment_resident		args( (CHAR_DATA *ch) );
char * 	get_house_address		args( (CHAR_DATA *ch) );
PROP_TYPE * room_prop           args( (ROOM_INDEX_DATA *room) );
HOUSE_TYPE * in_house           args( (CHAR_DATA *ch) );
void	house_security_update	args( (CHAR_DATA *ch) );
bool	is_privatep             args( (ROOM_INDEX_DATA *room) );
bool	has_access              args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
int		max_healing				args( (CHAR_DATA *ch) );
void	save_houses             args( (bool backup) );
void	load_houses             args( (void) );
void	save_domains		args( (bool backup) );
void	load_domains		args( (void) );
void	save_properties         args( (bool backup) );
void	load_properties         args( (void) );
void    save_playerrooms        args( (void) );
void    load_playerrooms        args( (void) );
void	playerroom_update		args( (void) );
void	house_auction_update	args( (void) );
HOUSE_TYPE * get_house          args( (CHAR_DATA *ch) );
bool	is_invited              args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room));
int		get_transfer_point      args( (CHAR_DATA *ch) );
char *	month_string            args( (int monthint ) );

/* invasions.c */
char *	random_goodguy		args( (void) );
char *	random_badguy		args( (void) );
void	load_soldiers		args( (void) );
void	save_soldiers		args( (void) );
void	spawn_invasion	args( (int type, int align) );
void	research_legacy	args( (CHAR_DATA *ch, char * argument) );
void	legacy_feat	args( (CHAR_DATA *ch, char * message) );
void	research_all_legacies	args( (CHAR_DATA *ch) );
void	legacy_saveworld	args( (CHAR_DATA *ch) );
void	legacy_endworld		args( (CHAR_DATA *ch) );
void	legacy_avertapoc	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	legacy_rk		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	legacy_arrest		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	load_legacies		args( (void) );
void	save_legacies		args( (void) );
void	invasion_radio	args( (void) );
void	update_invasion	args( (void) );
void 	invasion_setup	args( (void) );
void	minion_kill_check	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	pvp_kill_check		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	boss_kill_check		args( (CHAR_DATA *to, CHAR_DATA *victim) );
void	legacy		args( (CHAR_DATA *ch, int amount) );

/* shop.c */

int	shop_healing	args( (CHAR_DATA *ch) );
bool    is_invited_door_shop args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );
SHOP_TYPE * in_shop	args( ( CHAR_DATA *ch) );
void	save_pshops	args( (bool backup) );
void	load_pshops	args( (void) );
void	shop_auction_update	args( (void) );

/* interp.c */
int 	levenshtein_distance	args( (char *s,char*t) );
bool    check_social    args( ( CHAR_DATA *ch, char *command, char *argument ) );
bool	check_department	args( ( CMD_TYPE *cmd, CHAR_DATA *ch ) );
CMD_TYPE * find_command		args( ( char * command ) );
void	save_commands		args( ( void ) );
void	load_commands		args( ( void ) );
void 	load_disabled		args( ( void ) );
void 	save_disabled		args( ( void ) );
void	add_command		args( ( CMD_TYPE * command, void * handle ) );
/* wrapper function for safe command execution */
void 	do_function 		args( ( CHAR_DATA *ch, DO_FUN *do_fun, char *argument ) );

/* city.c */
void	save_cities		args( ( void ) );
void	load_city		args( ( void ) );
int	city_lookup_int		args( ( char * keyword ) );

bool	at_helm			args( ( CHAR_DATA *ch ) );

/* country.c */
void	save_countries		args( ( void ) );
void 	load_country		args( ( void ) );
int	country_lookup_int	args( ( char * keyword ) );

/* critical.c */
int check_critical	args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *weapon ) );


/* handler.c */
bool		room_visible					args( (CHAR_DATA *ch) );
CHAR_DATA *	get_char_by_account				args( (char * argument) );
CHAR_DATA *	get_char_by_id					args( (int id) );
void		account_sync					args( (CHAR_DATA *ch) );
void			logon_spirit				args( (CHAR_DATA *ch) );
char *			nospaces				args( (char * begining) );
void			declass					args( (CHAR_DATA *ch) );
void			add_intro						args( ( CHAR_DATA *ch, long id, int sex, char *name) );
void			logon_char						args( ( CHAR_DATA *ch) );
void			obj_from_char_silent			args( ( OBJ_DATA *obj ) );
void 			extract_obj_silent				args( ( OBJ_DATA *obj ) );
bool			is_pcouncil						args( ( CHAR_DATA *ch ) );
bool			has_resist						args( ( CHAR_DATA *ch, int type, CHAR_DATA *hitter ) );
bool			has_vuln						args( ( CHAR_DATA *ch, int type, CHAR_DATA *hitter ) );
bool			check_security					args( ( CHAR_DATA *ch, AREA_DATA *area ) );
const char	*	affect_bit_name_array			args( ( SET s ) );
void			affect_to_exit					args( ( EXIT_DATA *pExit, AFFECT_DATA *paf, bool reverse ) );
void			affect_remove_exit				args( ( EXIT_DATA *pExit, AFFECT_DATA *paf ) );
void			affect_to_room					args( ( ROOM_INDEX_DATA *pRoom, AFFECT_DATA *paf ) );
void			affect_remove_room				args( ( ROOM_INDEX_DATA *pRoom, AFFECT_DATA *paf ) );
AFFECT_DATA *	get_affect_by_location			args( ( AFFECT_DATA *paf, sh_int location ) );
AFFECT_DATA *	get_affect_by_type				args( ( AFFECT_DATA *paf, sh_int type ) );
AFFECT_DATA *	get_affect_by_bit				args( ( AFFECT_DATA *paf, long bit ) );
MEMORIZE_DATA *	get_memorize					args( ( CHAR_DATA *ch, char *keyword, int vnum ) );

char	*NAME2			args( ( CHAR_DATA *ch) );
char	*PERS			args( ( CHAR_DATA *ch, CHAR_DATA *looker ) );
char	*PERS_2			args( ( CHAR_DATA *ch, CHAR_DATA *looker ) );
char 	*PERS_3			args( ( CHAR_DATA *ch, CHAR_DATA *looker) );
int 	calc_cfield		args( ( int size, char *str ) ); 
void	pay_specific		args( ( CHAR_DATA *ch, long amount, int type_pay, int location ) );
void	pay_change		args( ( CHAR_DATA *ch, long amount, int country, int coin1, int coin2 ) );
void	generic_coin		args( ( CHAR_DATA *ch, CHAR_DATA *give_to, char * coin, int input_type, long amount, int guild  ) );
long	total_money		args( ( CHAR_DATA *ch, int type, int nation ) );
bool	is_coin_type		args( ( char *argument ) );
int	sec_stat 		args( ( CHAR_DATA *ch, int stat ) );
CD	*get_char_id		args( ( long id ) );
CD	*get_char_range 	args( ( CHAR_DATA *ch, char *argument, int range) );
int	get_char_range_int 	args( ( CHAR_DATA *ch, char *argument, int range) );
CD	*get_char_area  	args( ( CHAR_DATA *ch, char *argument ) );
char *  lowercase_clause        args( ( char clause[MSL]) );
AD  	*affect_find 		args( ( AFFECT_DATA *paf, int sn ));
void	affect_check		args( ( CHAR_DATA *ch, int where, int vector ) );
int	count_users		args( ( OBJ_DATA *obj ) );
void 	deduct_cost		args( ( CHAR_DATA *ch, int cost, int country ) );
void	affect_enchant		args( ( OBJ_DATA *obj ) );
int 	check_immune		args( ( CHAR_DATA *ch, int dam_type ) );
int 	material_lookup 	args( ( const char *name ) );
int	weapon_lookup		args( ( const char *name ) );
int	weapon_type		args( ( CHAR_DATA *ch ) );
char 	*weapon_name		args( ( int weapon_Type ) );
char	*item_name		args( ( int item_type ) );
int	attack_lookup		args( ( const char *name ) );
long	wiznet_lookup		args( ( const char *name ) );
int	class_lookup		args( ( const char *name ) );
int	coin_lookup		args( ( const char *name ) );
int	stance_lookup		args( ( const char *name ) );
int	duel_attack_lookup	args( ( const char *name ) );

bool	is_clan			args( ( CHAR_DATA *ch ) );
bool	is_same_clan		args( ( CHAR_DATA *ch, CHAR_DATA *victim));
bool	is_old_mob		args( ( CHAR_DATA *ch ) );
int	get_weapon_sn		args( ( CHAR_DATA *ch ) );
int	get_weapon_skill 	args( ( CHAR_DATA *ch, int sn ) );
int     get_age         	args( ( CHAR_DATA *ch ) );
int	get_true_age		args( ( CHAR_DATA *ch ) );
int	get_real_age		args( ( CHAR_DATA *ch ) );
void	reset_char		args( ( CHAR_DATA *ch ) );
int	get_trust		args( ( CHAR_DATA *ch ) );
long	get_curr_stat		args( ( CHAR_DATA *ch, int stat ) );
long	get_curr_stat_true	args( ( CHAR_DATA *ch, int stat ) );
int	get_curr_flow		args( ( CHAR_DATA *ch, int stat ) );
int 	get_curr_ss		args( ( CHAR_DATA *ch, int stat ) );
int 	get_max_train		args( ( CHAR_DATA *ch, int stat ) );
int	can_carry_n		args( ( CHAR_DATA *ch ) );
int	can_carry_w		args( ( CHAR_DATA *ch ) );
bool	is_name			args( ( char *str, char *namelist ) );
bool	is_exact_name		args( ( char *str, char *namelist ) );
bool	is_name_nocol		args( ( char *str, char *namelist ) );
void	affect_to_char		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_to_obj		args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void	affect_remove		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove_obj 	args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
bool	is_obj_affected		args( ( OBJ_DATA *obj, int bitvec ) );
void	affect_strip		args( ( CHAR_DATA *ch, int sn ) );
int		get_affect_bonus	args( ( CHAR_DATA *ch, int apply_mod ) );
void	affect_strip_room	args( ( ROOM_INDEX_DATA *room, int sn ) );
bool	is_affected		args( ( CHAR_DATA *ch, int sn ) );
void	affect_join		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room		args( ( CHAR_DATA *ch ) );
void	char_to_room		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char		args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char		args( ( OBJ_DATA *obj ) );
int		apply_ac		args( ( OBJ_DATA *obj, int iWear, int type ) );
OD *	get_eq_char		args( ( CHAR_DATA *ch, int iWear ) );
OD *	get_held		args( ( CHAR_DATA *ch, int item_type) );
OD *	get_worn		args( ( CHAR_DATA *ch, int item_type ) );
OD *	get_eqr_char		args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char_silent	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	equip_char		args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int		count_obj_list		args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *l ) );
void	obj_from_room		args( ( OBJ_DATA *obj ) );
void	obj_to_room		args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex) );
void	obj_to_obj		args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj		args( ( OBJ_DATA *obj ) );
void	extract_obj		args( ( OBJ_DATA *obj ) );
void	extract_char		args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_vision		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument) );
CD *	get_char_room		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
CD *	get_char_fight		args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world_new	args( ( char *argument ) );
CD *	get_char_haven		args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world_true	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world_pc	args( ( char *argument ) );
CD *	get_char_world_account	args( ( char * argument) );
CD *	get_char_world		args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_victim_world	args( ( CHAR_DATA *ch, char *argument ) );
CD * 	get_char_world_pc_noname args( ( CHAR_DATA *ch, char * argument) );
OD *	get_obj_type		args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list		args( ( CHAR_DATA *ch, char *argument, OBJ_DATA *l ) );
OD *	get_obj_carry		args( ( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer ) );
OD *	get_obj_carryhold	args( ( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer ) );
OD *	get_obj_wear		args( ( CHAR_DATA *ch, char *argument, bool character ) );
OD *	get_obj_here		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
OD *	get_obj_world		args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money		args( ( int amount, CHAR_DATA *ch ) );
long	convert_money		args( ( long amount, int type_in, int type_out, int location ) );
int	get_obj_number		args( ( OBJ_DATA *obj ) );
int	get_obj_weight		args( ( OBJ_DATA *obj ) );
int	get_true_weight		args( ( OBJ_DATA *obj ) );
bool	is_room_owner		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool	room_is_private		args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_see_room		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool	can_drop_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
const char *	affect_loc_name		args( ( int location ) );
const char *	affect_bit_name		args( ( int vector ) );
const char *	extra_bit_name		args( ( int extra_flags ) );
const char * 	wear_bit_name		args( ( int wear_flags ) );
const char *  talent_bit_name 	args( ( int talent_flags ) );
const char *	off_bit_name		args( ( int off_flags ) );
const char *  imm_bit_name		args( ( int imm_flags ) );
const char * 	form_bit_name		args( ( int form_flags ) );
const char *	part_bit_name		args( ( int part_flags ) );
const char *	weapon_bit_name		args( ( int weapon_flags ) );
const char *  comm_bit_name_array	args( ( SET s ) );
const char *  act_bit_name_array	args( ( SET s ) );
const char *	cont_bit_name		args( ( int cont_flags) );
char *	flow_lookup		args( ( int flow ) );
int	flow_lookup_int		args( ( const char *name) );
void    default_color   	args( ( CHAR_DATA *ch ) );
void    all_color       	args( ( CHAR_DATA *ch, char *argument ) );
int	get_char_ac		args( ( CHAR_DATA *ch, int ac_type, int body_location, bool improve ) );
void	mod_eq_hp		args( ( OBJ_DATA *obj, int mod ) );
void    damage_eq_body		args( ( CHAR_DATA *victim, int body_pary, int dam, int mod ) );
int	char_response		args( ( CHAR_DATA *ch, CHAR_DATA *fch ) );

int     find_door       	args( ( CHAR_DATA *ch, char *arg ) );
int	find_exit		args( ( CHAR_DATA *ch, char *arg ) );
/* interp.c */
void	interpret		args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number		args( ( char *arg ) );
bool	is_number_float		args( ( char *arg ) );
int	number_argument		args( ( char *argument, char *arg ) );
int	mult_argument		args( ( char *argument, char *arg) );
char *	one_argument		args( ( char *argument, char *arg_first ) );
char *  one_argument_nouncap	args( ( char *argument, char *arg_first ) );
char *	one_argument_true	args( ( char *argument, char *arg_first ) );
/* specia skill.c */
int	el_skill		args( ( int skill ) );
bool	is_el_skill		args( ( int skill ) );


/* mob_prog.c */
void	program_flow		args( ( sh_int vnum, char *source, CHAR_DATA *mob,
				    OBJ_DATA *obj, ROOM_INDEX_DATA *room,
				    CHAR_DATA *ch, const void *arg1,
				    const void *arg2 ) );
void	p_act_trigger		args( ( char *argument, CHAR_DATA *mob,
				    OBJ_DATA *obj, ROOM_INDEX_DATA *room,
				    CHAR_DATA *ch, const void *arg1,
				    const void *arg2, int type ) );
bool	p_percent_trigger 	args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				    ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				    const void *arg1, const void *arg2, int type ));
void	p_bribe_trigger  	args( ( CHAR_DATA *mob, CHAR_DATA *ch, int amount ) );
bool	p_exit_trigger   	args( ( CHAR_DATA *ch, int dir, int type ) );
void	p_give_trigger   	args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				    ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				    OBJ_DATA *dropped, int type ) );
void 	p_greet_trigger  	args( ( CHAR_DATA *ch, int type ) );
void	p_hprct_trigger  	args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );

/* magic.c */
char *	magic_language		args( (CHAR_DATA *ch) );
bool	is_spell_immune		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	cast_spell		args( ( CHAR_DATA *ch, int spell) );

/* mem.c */
void free_feature               args( ( FEATURE_TYPE *feature ) );
void free_exit                  args( ( EXIT_DATA *pExit ) );
void free_reset_data            args( ( RESET_DATA *pReset ) );
void free_extra_descr	        args( ( EXTRA_DESCR_DATA *pExtra ) );

/* mob_cmds.c */
void	mob_interpret		args( ( CHAR_DATA *ch, char *argument ) );
void	obj_interpret		args( ( OBJ_DATA *obj, char *argument ) );
void	room_interpret		args( ( ROOM_INDEX_DATA *room, char *argument ) );

/* music.c */
bool	is_social_radio_space	args( ( void ) );
void	add_to_social_radio	args( ( char *arg) );
bool	is_news_radio_space	args( ( void ) );
void	add_to_news_radio	args( ( char *arg ) );
bool	is_snews_radio_space	args( ( void ) );
void	add_to_snews_radio	args( ( char *arg ) );

/*note_new.c */

char *	note_news		args( (CHAR_DATA *ch ) );

/* save.c */
void	load_ground_objects	args( ( void ) );
void 	save_ground_objects	args( ( void ) );
void	save_char_obj		args( ( CHAR_DATA *ch, bool crash, bool Forsaken ) );
bool	load_char_obj		args( ( DESCRIPTOR_DATA *d, char *name ) );
void	fread_obj		args( ( THING TH, FILE *fp ) );
void    fwrite_obj      	args( ( OBJ_DATA  *obj, FILE *fp, int iNest ) );

void	save_chests		args( ( void ) );
void	load_chests		args( ( void ) );
void	save_corpses		args( ( void ) );
void	load_corpses		args( ( void ) );

void set_ch_default   args( ( CHAR_DATA *ch ) );
void set_pcdata_default args( ( PC_DATA *pcdata ) );

/* sex.c */
bool	is_virgin			args( (CHAR_DATA *ch) );
int     get_pregnancy_minlength    args( (CHAR_DATA *ch) ); 
void	dream_sex		        args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void    heres_johnny            args( (CHAR_DATA *ch) );
void    apply_contractions      args( (CHAR_DATA *victim, int length) );
bool    is_fertile              args( (CHAR_DATA *ch) );
void    have_sex                args( (CHAR_DATA *ch, CHAR_DATA *victim, char risk[MSL], char type[MSL], CHAR_DATA *originator) );
void    sex_upkeep              args( (CHAR_DATA *ch) );
void    miscarriage             args( (CHAR_DATA *ch, bool forced) );
void    sex_cleanup             args( (CHAR_DATA *ch) );
bool    has_item_name           args( (CHAR_DATA *ch, char itemname[MSL]) );
char *  day_ordinal             args( (int day ) );

/* shifters.c */

char *  genus_name		args( (int genus) );
int	swarm_count		args( (CHAR_DATA *ch, int number) );
bool 	viable_form		args( (CHAR_DATA *ch, int number) );
void	human_transformation	args( (CHAR_DATA *ch) );
void	view_animal_to		args( (CHAR_DATA *ch, int number, CHAR_DATA *to) );
int 	animal_size		args( (int weight) );
int	get_animal_weight	args( (CHAR_DATA *ch, int number) );
void	show_animal_to_char	args( (CHAR_DATA *ch, CHAR_DATA *looker, int type, bool photo) );
int 	animal_skill	args( (CHAR_DATA *ch, int number, int skill) );
int	get_animal_claw		args( (CHAR_DATA *ch, int number) );
int	get_animal_tough	args( (CHAR_DATA *ch, int number) );
void	pants_off		args( (CHAR_DATA *ch) );
void	remove_clothing		args( (CHAR_DATA *ch) );
char *	get_animal_description	args( (CHAR_DATA *ch, int number) );
char *	get_animal_species	args( (CHAR_DATA *ch, int number) );
int	animal_stat		args( (CHAR_DATA *ch, int number, int stat) );
int	get_animal_genus	args( (CHAR_DATA *ch, int number) );

/*social.c*/
void   show_random_ai_feedback	args( (CHAR_DATA *ch) );

int influencer_score			args ((CHAR_DATA *ch) );

int profile_rating			args( (CHAR_DATA *ch) );
int rp_you_count                       args( (CHAR_DATA *ch) );
int chat_you_count                     args( (CHAR_DATA *ch) );

void profile_logon	args( (CHAR_DATA *ch) );
CHATROOM_TYPE * chatroom_lookup_vnum args( (int vnum) );
CHATROOM_TYPE * chatroom_lookup_name args( (char *name) );
MATCH_TYPE *match_find_by_name args( (char * nameone, char * nametwo) );
MATCH_TYPE * match_find args( (CHAR_DATA *ch, CHAR_DATA *victim) );
PROFILE_TYPE *profile_handle_lookup args( (char * handle) );
PROFILE_TYPE *profile_lookup args( (char *name) );
int rp_with_count			args( (CHAR_DATA *ch) );
int chat_with_count			args( (CHAR_DATA *ch) );
bool meet_viable		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool chat_viable		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void social_update		args ((void));

/* social-edit.c */
void	see_random_social	args( ( CHAR_DATA *ch ) );
void	do_random_social	args( ( CHAR_DATA *ch) );
void 	do_random_kill		args( ( CHAR_DATA *ch) );

/* skills.c */
bool    is_fatesensitive	args( (CHAR_DATA *ch) );
bool    seems_fatesensitive	args( (CHAR_DATA *ch) );
bool 	can_train_disc		args( (CHAR_DATA *ch, int disc) );
bool	seems_suffer_sensitive  args( (CHAR_DATA *ch) );
void    clear_flags		args( (CHAR_DATA *target) );
int 	eidilon_clan_lookup	args( (char *argument) );

char 	*trimwhitespace		args( (char *str) );
void 	inactive_disrepect	args( (char * cname) );

bool	active_character	args( (char * cname) );
int	belief_match		args( (CHAR_DATA *ch, FACTION_TYPE *fac) );
int 	ritual_debuff_power	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void 	steal_object		args( (OBJ_DATA *obj, CHAR_DATA *ch) );
void 	stolen_update		args( (OBJ_DATA *obj) );
void    return_object		args( (OBJ_DATA *obj) );
bool 	stolen_object		args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool	my_object		args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool	account_allies		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	has_employees		args( (ACCOUNT_TYPE *account) );
void	weakness_update		args( (CHAR_DATA *ch, char * vicname) );
void 	offline_potion		args( (char * name, int vnum) );
int	commit_cost		args( (CHAR_DATA *committer, char * vicname) );
void	stat_explain		args( (CHAR_DATA *ch, CHAR_DATA *target, int stat) );
void 	process_training_argument args( (CHAR_DATA *ch, char * argument) );
void	shroudescape		args( (CHAR_DATA *ch) );
char *	to_gibberish		args( (char * language, char * word) );
int	arcane_lifeforce	args( (CHAR_DATA *ch, bool used) );
bool	biteable		args( (CHAR_DATA *ch, CHAR_DATA *victim, bool show) );
void	statcheck		args( (CHAR_DATA *ch) );
void	disccheck		args( (CHAR_DATA *ch) );
int	execute_npc_ritual	args( (char * argument) );
int	lang_level	args( (CHAR_DATA *ch, char *argument) );
void 	contact_logon		args( (CHAR_DATA *ch) );
int	labor_points		args( (CHAR_DATA *ch) );
bool 	has_focus		args( (CHAR_DATA *ch) );
void    focus_glow              args( (CHAR_DATA *ch, char * color) );
void    equip_focus             args( (CHAR_DATA *ch) );
void	autotrain_discipline	args( (CHAR_DATA *ch, int number) );
void	autotrain_stat		args( (CHAR_DATA *ch, int number) );
char *	mangle_text		args( (CHAR_DATA *ch, CHAR_DATA *victim, char * language, char * argument) );
char *	mangle_word		args( (CHAR_DATA *ch, CHAR_DATA *victim, char * language, char * argument) );
bool 	mindbroken		args( (CHAR_DATA *ch) );
void	persuade_check		args( (CHAR_DATA *ch, CHAR_DATA *victim, char * message) );
void	triggercheck		args( (CHAR_DATA *ch, char * message) );
void	show_imprints		args( (CHAR_DATA *ch) );
void	auto_imprint		args( (CHAR_DATA *ch, char * message, int type) );
void	auto_imprint_timed	args( (CHAR_DATA *ch, char * message, int type, int time) );
void	maim_char		args( (CHAR_DATA *ch, char * message) );
void	werewolf_live_kill	args( (CHAR_DATA *ch) );
CHAR_DATA * get_tracer		args( (CHAR_DATA *ch) );
void	reset_char		args( (CHAR_DATA *ch) );
void	conclude_process	args( (CHAR_DATA *ch, int type) );
void	process_message		args( (CHAR_DATA *ch, int type) );
void	gossip			args( (char * argument) );
int	skilltype_count		args( (CHAR_DATA *ch, int type) );
void    habit_corpse    	args( (CHAR_DATA *ch, char *deathcause) );
bool	is_fulltimer		args( (CHAR_DATA *ch) );
bool	is_parttimer		args( (CHAR_DATA *ch) );
bool	is_natural		args( (ROOM_INDEX_DATA *room) );
int 	skillbase_count		args( (CHAR_DATA *ch, int type) );
char *	static_text		args( (CHAR_DATA *ch, CHAR_DATA *victim, char * argument) );
bool 	social_abom		args( (CHAR_DATA *ch) );
bool	nonevil_abom		args( (CHAR_DATA *ch) );
int 	average_solidity	args( (ROOM_INDEX_DATA *room) );
bool	seems_vampire		args( (CHAR_DATA *ch) );
bool 	seems_werewolf		args( (CHAR_DATA *ch) );
bool	seems_demonborn		args( (CHAR_DATA *ch) );
bool	seems_angelborn		args( (CHAR_DATA *ch) );
bool	seems_demigod		args( (CHAR_DATA *ch) );
bool	seems_faeborn		args( (CHAR_DATA *ch) );
bool	seems_suggestible	args( (CHAR_DATA *ch) );
void	under_taint		args( (CHAR_DATA *ch, CHAR_DATA *victim, int time) );
bool	youngvampire		args( (CHAR_DATA *ch) );
int	timesinceaccident	args( (CHAR_DATA *ch) );
void 	useattack		args( (CHAR_DATA *ch) );
void 	usemove			args( (CHAR_DATA *ch) );

int 	abilcount		args( (CHAR_DATA *ch) );
void	resetabils		args( (CHAR_DATA *ch) );
bool	has_nonconsume		args( (CHAR_DATA *ch, int item) );
bool 	has_consume		args( (CHAR_DATA *ch, int item) );
bool 	has_requirements	args( (CHAR_DATA *ch, int skill, int level, bool show) );
bool 	take_blood		args( (CHAR_DATA *ch, int type, int amount) );
void 	sympathy_pain		args( (char *name, int level) );
OBJ_DATA* get_crystal		args( (CHAR_DATA *ch) );
void	offline_reward		args( (char * argument, int type, int amount, CHAR_DATA *restrict) );
bool	in_fistfight		args( (CHAR_DATA *ch) );
void	fistfight		args( (CHAR_DATA *ch) );
void	habit_logon		args( (CHAR_DATA *ch) );
char * habit_level		args( (int habit, int level) );
void 	give_karma		args( (CHAR_DATA *ch, int amount, int type) );
void	captive_command		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	wound_char_noregen	args( (CHAR_DATA *victim, int type) );
void	wound_char		args( (CHAR_DATA *victim, int type) );
void 	wound_char_absolute	args( (CHAR_DATA *victim, int type) );
void	influence_update	args( (CHAR_DATA *ch) );
void	give_influence		args( (CHAR_DATA *ch, int amount) );
void 	message_to_char		args( (char * name, char * message) );
int 	skilltype		args( (int skill) );
int 	skillcount		args( (CHAR_DATA *ch, int type) );
int 	offline_skill		args( (char *name, int skill) );
int	wallnumber		args( ( CHAR_DATA *ch) );
char *	area_name		args( ( char * looker, CHAR_DATA *victim) );
char *	offline_name		args( ( CHAR_DATA *looker, char * name) );
ROOM_INDEX_DATA * find_path	args( (ROOM_INDEX_DATA *room, ROOM_INDEX_DATA *from, int pastvnums[], int limit) );
int	track_zone		args(( ROOM_INDEX_DATA *room, int zone) );
int	track_room		args( ( ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to) );
void	house_charge		args( (char *argument, int price) );
int 	bank_check		args( (char *argument) );
char *  get_skname		args( (int val) );

/* creation.c */
bool	newbie_school		args( (ROOM_INDEX_DATA *room) );
int	extract_feet		args( ( CHAR_DATA *ch ) );
int	extract_inches		args( ( CHAR_DATA *ch) );
bool	readychar		args( ( CHAR_DATA *ch) );
int	charcost		args( ( CHAR_DATA *ch) );
void	genesis_display		args( ( CHAR_DATA *ch) );
void	start_genesis		args( ( CHAR_DATA *ch) );
bool    change_inches      	args( ( CHAR_DATA *ch, char argument[MSL]) );
bool    change_feet      	args( ( CHAR_DATA *ch, char argument[MSL]) );

/* drunk.c */
char    *makedrunk      	args( ( char *string, CHAR_DATA *ch ) );

/* screenmanip.c */
void	cursor_up		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int rows ) );
void	cursor_down		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int rows ) );
void	cursor_forward		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int cols ) );
void	cursor_back		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int cols ) );
void	save_cursor		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d ) );
void	restore_cursor		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d ) );
void	erase_eol		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d ) );
void	erase_xyeol		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int col, int row ) );
void	beep_char		args( ( CHAR_DATA *ch, char *argument ) );
void	goto_xy			args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int col, int row ) );
void	clear_screen		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d ) );
void	send_to_char_xy		args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int col, int row, char *str ) );
void	send_to_char_center_xy	args( ( CHAR_DATA *ch, DESCRIPTOR_DATA *d, int row, char *str ) );



/* special.c */
SF *	spec_lookup		args( ( const char *name, long type ) );
char *	spec_name		args( ( SPEC_FUN *function ) );

/* stat.c */
void	showstat		args( ( CHAR_DATA *ch ) );

/* stories.c */
void trans_intel		args( (CHAR_DATA *ch, CHAR_DATA *pers) );
void summary_prompt			args( (CHAR_DATA *ch) );
STORYLINE_TYPE * get_sect_storyline args( (FACTION_TYPE *sect) );
bool can_see_calendar		args( (CHAR_DATA *ch, STORY_TYPE *cal) );
bool can_enter_event		args( (CHAR_DATA *ch, STORY_TYPE *cal) );
bool twinky_hero		args( (CHAR_DATA *ch) );
bool valid_subfac 		args((char *word));
int  get_subfac			args( (char *word) );

bool good_person		args( (CHAR_DATA *ch) );
bool ff_surname			args( (char * arg) );
bool is_ffamily			args( (CHAR_DATA *ch) );
ANNIVERSARY_TYPE * get_anniversary_today args( (void) );
std::string encounter_prompt	args( (CHAR_DATA *ch, int type) );
int giftobjval			args( (OBJ_DATA *obj) );
void init_char			args( (CHAR_DATA *ch) );
void add_encounter		args( (int etype, int ebase, char * text, int clan_id) );
void set_doom			args( ( CHAR_DATA *ch) );
int remote_power_level	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool norp_match			args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool are_allies args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool are_noncult_allies args( (CHAR_DATA *ch, CHAR_DATA *victim) );

  bool narrative_pair args( (CHAR_DATA *ch, CHAR_DATA *victim) );
  int  narrative_score args( (CHAR_DATA *ch, CHAR_DATA *victim) );
  char * narrative_pair_rewards args( (CHAR_DATA *ch, CHAR_DATA *victim) );

char * 	narrative_show		args( (CHAR_DATA *ch) );
void 	destiny_char_update	args( (CHAR_DATA *ch) );
bool	crisis_runner		args( (CHAR_DATA *ch) );
bool	can_see_destiny		args( (CHAR_DATA *ch, NEWDESTINY_TYPE *destiny) );
bool	crisis_runner		args( (CHAR_DATA *ch) );
bool	destiny_linked		args( (CHAR_DATA *ch, CHAR_DATA *victim));
bool	destiny_prey		args( (CHAR_DATA *ch, CHAR_DATA *victim));
void	destiny_update		args( ( void ) );
bool	has_adventure		args( (CHAR_DATA *ch) );
bool	power_fantasy		args( (FANTASY_TYPE *fant) );
bool	fantasy_interp		args( (CHAR_DATA *ch, char * command, char *argument, FANTASY_TYPE *fant) );
int	worlds_experience	args( (CHAR_DATA *ch, int type) );
int 	fantasy_number		args( (FANTASY_TYPE *fant) );
FANTASY_TYPE * fetch_fantasy	args( (CHAR_DATA *ch, int number) );
bool	dream_slave		args( (CHAR_DATA *ch) );
void	enter_dreamworld	args( (CHAR_DATA *ch, FANTASY_TYPE *fant) );
FANTASY_TYPE *lobby_number	args( (int number) );
void	to_spectre		args( (CHAR_DATA *ch, bool combat) );
void	wake_char		args( (CHAR_DATA *ch) );
void	dream_move		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
void	bookadd			args( (CHAR_DATA *ch, int world) );
void	add_mastermind		args( (STORYLINE_TYPE *story, char * name, int amount) );
int	worlds_experience	args( (CHAR_DATA *ch, int type) );
void	charge_worlds_exp	args( (CHAR_DATA *ch, int type, int amount) );
void	sr_reroll		args( (CHAR_DATA *ch, char * newname) );
void 	sr_delete		args( (CHAR_DATA *ch) );
void	villain_mod		args( (CHAR_DATA *ch, int amount, char * reason) );
void	dream_lobby_update	args( ( void ) );
void	dream_lobby_move	args( (CHAR_DATA *move) );
int    subfac_score	args( (CHAR_DATA *ch, int subfac) );
bool	is_invader		args( (CHAR_DATA *ch) );
CHAR_DATA * encounter_victim	args( (CHAR_DATA *ch) );
bool	goddreamer		args( (CHAR_DATA *ch) );
bool	part_of_fantasy		args( (CHAR_DATA *ch, FANTASY_TYPE *fantasy) );
FANTASY_TYPE * in_fantasy	args( (CHAR_DATA *ch) );
FANTASY_TYPE * room_fantasy	args( (ROOM_INDEX_DATA *room) );
bool 	mali_vuln		args( (CHAR_DATA *ch) );
bool 	justi_vuln		args( (CHAR_DATA *ch) );
char * 	sinlist			args( (CHAR_DATA *ch) );
bool	has_mystery		args( (CHAR_DATA *ch) );
bool	is_srname		args( (char * name) );
char *  sr_name			args( (char * oldname) );
char *	nosr_name		args( (char *oldname) );
void	convert_to_sr		args( (CHAR_DATA *ch) );
void	revert_from_sr		args( (CHAR_DATA *ch) );
void	lookup_research		args( (CHAR_DATA *ch, char * argument) );
void	load_archive_indexes	args( ( void) );
void	save_archive_indexes	args( ( bool backup) );
void	save_archive_entry	args( (ARCHIVE_ENTRY *arch) );
STORYLINE_TYPE * get_storyline	args( (CHAR_DATA *ch, char *name) );
void 	join_to_storyline	args( (CHAR_DATA *ch, STORYLINE_TYPE *story) );
void	roster_update		args( ( void ) );
void	offline_karma		args( (char * argument, int amount) );
void 	alter_character		args( (CHAR_DATA *ch) );
void 	remove_from_roster	args( (char * name) );
bool 	is_roster_char		args( (CHAR_DATA *ch) );
ROSTERCHAR_TYPE * get_rosterchar args( (char * name) );
void    save_rosterchars             args( (bool backup) );
void    load_rosterchars             args( ( void ) ); 
void    save_galleries             args( (bool backup) );
void    load_galleries             args( ( void ) );
void	save_pages		   args( (bool backup) );
void	load_pages			args( ( void ) );
void    save_storylines             args( (bool backup) );
void    load_storylines             args( ( void ) );
void	save_destinies		args( (bool backup) );
void	load_destinies		args( ( void ) );
void	save_fantasies		args( (bool backup) );
void	load_fantasies		args( ( void ) );
bool	encounter_room		args( (ROOM_INDEX_DATA *room) );
void	have_encounter		args( (CHAR_DATA *storyrunner, CHAR_DATA *target, int type) );
bool 	visible_plot		args( (CHAR_DATA *ch, PLOT_TYPE *plot) );
void	start_survey		args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void	survey_update		args( (CHAR_DATA *ch) );
bool	silly_survey		args( (CHAR_DATA *ch) );
bool	critical_survey		args( (CHAR_DATA *ch) );
void	send_survey		args( (CHAR_DATA *ch, char *name) );
void	process_survey_number	args( (CHAR_DATA *ch, char * argument) );
PLOT_TYPE *get_karma_plot	args( (CHAR_DATA *ch) );
bool	crazy_town		args( ( void ) );
bool	has_plot		args( (CHAR_DATA *ch) );
bool 	has_crisis		args( (CHAR_DATA *ch) );
char * get_subfac_name  	args( (int val) );
void	knock_check		args( ( CHAR_DATA *ch, int door ) );
void	invade_check		args( ( void ) );
bool 	can_see_news		args( (CHAR_DATA *ch, NEWS_TYPE *news) );
bool	has_con			args( ( CHAR_DATA *ch, int condition) );
void	scon_update		args( ( CHAR_DATA *ch) );
void	char_to_research	args( (char * name, RESEARCH_TYPE *research) );
void	save_stories		args( ( void ) );
void 	load_stories		args( ( void ) );
void	save_plots		args( ( void ) );
void	load_plots		args( ( void ) );
void	save_storyideas		args( ( void ) );
void	load_storyideas		args( ( void ) );
void	save_news		args( ( void ) );
void 	load_news		args( ( void ) );
void	news_update		args( ( void ) );
void	story_update		args( ( void ) );
void	save_research		args( ( void ) );
void	load_research		args( ( void ) );
void    save_eencounter           args( ( void ) );
void    load_eencounter           args( ( void ) );
void    save_epatrol            args( ( void ) );
void    load_epatrol            args( ( void ) );

void    save_anniversaries            args( ( void ) );
void    load_anniversary            args( ( void ) );

void    save_decrees            args( ( void ) );
void    load_decrees            args( ( void ) );

void    save_grouptexts            args( ( void ) );
void    load_grouptexts            args( ( void ) );

void    save_texthistories            args( ( void ) );
void    load_texthistories            args( ( void ) );

void	save_matches            args( ( void ) );
void	load_matches            args( ( void ) );

void 	save_profiles		args( ( void ) );
void	load_profiles		args( ( void ) );

void 	save_chatrooms		args( ( void ) );
void	load_chatrooms		args( ( void ) );

void	research_update		args( ( void ) );
void	show_news		args( (CHAR_DATA *ch) );
int     storycount              args( ( void ) );
bool 	gm_active		args( (CHAR_DATA *ch) );
void 	story_credit		args( (CHAR_DATA *ch) );
void 	room_credit		args( (CHAR_DATA *ch) );

/* system.c */
bool load_system_config		args( ( void ) );
//void do_showsysdata		args( ( CHAR_DATA *ch, char * argument ) );
void save_sysdata		args( ( void ) );

/* teleport.c */
RID *	room_by_name		args( ( char *target, int level, bool error) );

/* travel.c */
bool can_world_travel		args ((CHAR_DATA *ch, ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *to_room) );
void 	have_accident		args( (CHAR_DATA *ch) );
int	nonhelpless_pop		args( (ROOM_INDEX_DATA *room) );
int	nondetain_pop		args( (ROOM_INDEX_DATA *room) );
int	nonpatient_pop		args( (ROOM_INDEX_DATA *room) );
void	npc_rundown		args( (CHAR_DATA *victim) );
int	dream_pop		args( (CHAR_DATA *ch) );
void	force_park		args((CHAR_DATA *car) );
CHAR_DATA * get_char_dream	args( (CHAR_DATA *ch, char * argument) );
void	dreamscape_message	args( (CHAR_DATA *ch, int room, char * message) );
char *	dream_detail		args( (CHAR_DATA *ch, char * world, int type) );
char *	dream_introduction	args( (CHAR_DATA *ch) );
char *  dream_name      args( (CHAR_DATA *ch) );
char *  dream_fame      args( (CHAR_DATA *ch) );
char *  dream_eq      args( (CHAR_DATA *ch) );
char *  dream_description	args( (CHAR_DATA *ch) );
void 	place_car		args( (CHAR_DATA *ch, int location) );
void	remember_location	args( (CHAR_DATA *ch, char * argument) );
int     vehicle_quality 	args((CHAR_DATA *ch) );
char * 	carstatusname		args( (int status) );
void	carattack		args( (CHAR_DATA *ch, CHAR_DATA *car) );
bool	is_car			args( (CHAR_DATA *ch) );
void	forcecarkeys		args( (CHAR_DATA *ch, CHAR_DATA *forcer) );
void	accident_check		args( (CHAR_DATA *ch) );
int	garage_charge		args( (CHAR_DATA *ch) );
int	status_bonus		args( (CHAR_DATA *ch) );
int	cruiser_bonus		args( (CHAR_DATA *ch) );
int	safety_bonus		args( (CHAR_DATA *ch) );
int	toughness_bonus		args( (CHAR_DATA *ch) );
int 	stakeout_bonus		args( (CHAR_DATA *ch) );
int	vehicle_location	args( (CHAR_DATA *ch) );
bool	fast_car		args( (CHAR_DATA *ch) );
void	park_car		args( (CHAR_DATA *ch, int location) );
char *	carqualityname		args( (int cost, int typeone, int typetwo) );
char *	cartypename		args( (int typeone, int typetwo) );
int	carqualitylevel		args( (int cost, int typeone, int typetwo) );
bool	has_active_vehicle	args( (CHAR_DATA *ch) );
int	vehicle_typeone		args( (CHAR_DATA *ch) );
int	vehicle_typetwo		args( (CHAR_DATA *ch) );
int	vehicle_cost		args( (CHAR_DATA *ch) );
char *	vehicle_name		args( (CHAR_DATA *ch) );
char * 	vehicle_desc		args( (CHAR_DATA *ch) );
char *	vehicle_lplate		args( (CHAR_DATA *ch) );
void	logonkeyscheck		args( (CHAR_DATA *ch) );
void	garage_update		args( (CHAR_DATA *ch, int amount) );
bool	sin_in_room		args( (ROOM_INDEX_DATA *room) );
int	abom_pop		args( (ROOM_INDEX_DATA *room) );
int	power_pop		args( (ROOM_INDEX_DATA *room) );
bool	other_players		args( (CHAR_DATA *ch) );
bool	room_hostile		args( (ROOM_INDEX_DATA *room) );
int	pc_pop			args( (ROOM_INDEX_DATA *room) );
int	npc_pop			args( (ROOM_INDEX_DATA *room) );
bool	room_guest		args( (ROOM_INDEX_DATA *room) );
bool	room_ambush		args( (ROOM_INDEX_DATA *room) );
bool	room_ambushing		args( (ROOM_INDEX_DATA *room) );
int 	highestoffense		args( (CHAR_DATA *ch) );
int get_po			args( (ROOM_INDEX_DATA *room) );
ROOM_INDEX_DATA * get_poroom	args( (int val) );
void	trolly_message		args( (int type) );
bool 	directory_display	args( (ROOM_INDEX_DATA * room, char *argument) );
bool	room_jeopardy		args( (ROOM_INDEX_DATA *room) );
bool	has_key			args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool 	is_tresspassable	args( (ROOM_INDEX_DATA *room) );
void	room_purge		args( (CHAR_DATA *ch) );
int	player_pop		args( (ROOM_INDEX_DATA *room) );
int 	houserooms		args( (CHAR_DATA *ch) );
int 	wealth_req		args( (ROOM_INDEX_DATA *room) );
void message			args( (char * argument, char * message) );
int	room_pop		args( (ROOM_INDEX_DATA *room) );
int	fight_room_pop		args( (ROOM_INDEX_DATA *room) );
bool 	freeplayerroom		args( (int vnum) );

void	move_closer		args( (CHAR_DATA *ch, CHAR_DATA *victim, int dist) );
void	bustowalk		args( ( CHAR_DATA *ch) );
void	subwaytowalk		args( (CHAR_DATA *ch) );
void	walktosubway		args( (CHAR_DATA *ch) );
void	cartowalk		args( ( CHAR_DATA *ch) );
void	cabtowalk		args( ( CHAR_DATA *ch) );
void	flytowalk		args( (CHAR_DATA *ch) );

/* techs.c */
int	get_tech_level		args( (int selection, CHAR_DATA *ch) );
int     get_tech_blevel         args( (int selection, CHAR_DATA *ch) );

/* update.c */
bool 	is_alone			args( (CHAR_DATA *ch) );
int	soc_wealth_mod			args( (CHAR_DATA *ch, bool invert) );
bool 	deepforest			args( (ROOM_INDEX_DATA *room) );
void 	forest_update			args( (AREA_DATA *area) );
bool	is_winter			args( (void) );
void 	gain_dexp			args ( (CHAR_DATA *ch, int amount) );
void 	switch_vehicle			args( (CHAR_DATA *ch, bool tohorse) );
bool        has_place                   args( (ROOM_INDEX_DATA *room, char * argument) );
void		stat_active_update		    args( ( CHAR_DATA *ch) );
CHAR_DATA *	fetch_guestmonster	        args( (void) );
CHAR_DATA * fetch_guestmonster_exclusive	args( (CHAR_DATA *ch) );
void        academy_mood                args( ( CHAR_DATA *ch));
void        academy_imprint             args( ( CHAR_DATA *ch));
void        academy_gate    		    args( (int hour) );
void	    hear_academy_gate		    args( (CHAR_DATA *ch, int hour) );
bool 	    trapped_room			    args( (ROOM_INDEX_DATA *room, CHAR_DATA *ch) );
void	    demon_hole				    args( (ROOM_INDEX_DATA *room) );
void	    demon_hole_update		    args( ( void ) );
void	    initiate_demon_holes	    args( ( void ) );
void        send_message_to_char        args( ( CHAR_DATA *ch ) );
void 	    auto_arrest				    args( (CHAR_DATA *ch) );
bool 	    mentoring_eligible_ooc      args( (CHAR_DATA *ch) );
bool        mentoring_eligible_ic      args( (CHAR_DATA *ch) );

bool 	    locked_room				    args( (ROOM_INDEX_DATA *room, CHAR_DATA *ch) );
void 	    seed_battle				    args( (FACTION_TYPE *fac) );
int	        estimated_expenses		    args( (CHAR_DATA *ch) );
int	        estimated_pay			    args( (CHAR_DATA *ch) );
bool        has_attendant               args( (CHAR_DATA *ch) );
bool	    can_hear_ocean			    args( (CHAR_DATA *ch) );
int 	    blood_tvalue			    args( (CHAR_DATA *ch) );
bool 	    full_moon				    args( ( void ) );
bool	    new_moon				    args( ( void ) );
int 	    room_attract			    args( (ROOM_INDEX_DATA *room) );
int 	    get_drunk 				    args( (CHAR_DATA *ch) );
void	    poison_char				    args( (CHAR_DATA *ch) );
bool        in_shower                   args( (CHAR_DATA *ch) );
bool	    in_bath					    args( (CHAR_DATA *ch) );
bool	    in_stream			args( (CHAR_DATA *ch) );
bool	    is_sprite				    args( (CHAR_DATA *ch) );
bool	    is_bird					    args( (CHAR_DATA *ch) );
void 	    display_bonusrooms		    args( (CHAR_DATA *ch) );
bool   	    in_hospital     		    args( ( CHAR_DATA *ch ) );
int 	    daysInMonth				    args( (int m, int y) );
int         dayIndex        		    args( (int d, int m, int y) );
void	    hospitalise	                args( ( CHAR_DATA *victim) );
void	    advance_level			    args( ( CHAR_DATA *ch, bool hide ) );
void	    gain_exp				    args( ( CHAR_DATA *ch, int amount, int type ) );
void	    gain_rpexp				    args( ( CHAR_DATA *ch, int amount ) );
void	    gain_condition			    args( ( CHAR_DATA *ch, int iCond, int value ) );
void	    update_handler			    args( ( void ) );
void	    hint_update				    args( ( void ) );
void        check_public                args( (CHAR_DATA *ch) );
void 	    shoot					    args( (CHAR_DATA *ch) );
void	    update_clothes			    args( (CHAR_DATA *ch) );
void	    update_clothes_slow		    args( (CHAR_DATA *ch) );
/* string.c */

char * replaceSubstring 			args( (char* original, const char* substring, const char* replacement) );
char * 	appendLine				args ((char * orig, char *newline) );
size_t 	safe_strlen				args( (const char* str) );
char *  fetch_line		            args( ( char * orig, int line) );
int	    linecount		            args( ( char * orig) );
char *  wrap_string                 args( ( char *oldstring, int length ) );
char *  numlineas                   args( ( char *string ) );
void	string_edit		            args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   	        args( ( CHAR_DATA *ch, char **pString ) );
char *  string_deleteline           args( (char *orig, int line) );
char *	string_replace		        args( ( char * orig, char * old, char * new_str ) );
void    string_add      	        args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   	        args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       	        args( ( char *argument, char *arg_first, bool fCase ) );
char *	string_unpad		        args( ( char * argument ) );
char *	string_proper		        args( ( char * argument ) );
void	do_statstuff		        args( ( CHAR_DATA *ch, char *argument ) );
void 	ai_update					args( ( void ) );
void 	writeLineToFile				args( (const std::string& fileName, const std::string& text) );
void    writeTextToFile                         args( (const std::string& fileName, const std::string& text) );

std::vector<std::string> splitString args( (const std::string& input, const std::string& delimiter) );
std::string readAndDeleteLineFromFile args ( (const std::string& fileName) );

/* lookup.c */
bool    can_manual_task			args( (CHAR_DATA *ch) );
bool 	real_weapon			args( (OBJ_DATA *obj) );
bool 	real_armor			args( (OBJ_DATA *obj) );
bool 	is_metashifter			args( (CHAR_DATA *ch) );
int    	alt_count			        args( (CHAR_DATA *ch) );
bool	higher_power			    args( (CHAR_DATA *ch) );
bool 	clique_is_popular			args( (int clique_role) );
bool 	clique_is_nerd			    args( (int clique_role) );
bool 	clique_is_rebel			args( (int clique_role) );
bool	builder_helper			    args( (CHAR_DATA *ch) );
bool	has_gasmask			        args( (CHAR_DATA *ch) );
bool	physical_dreamer            args( (CHAR_DATA *ch) );
bool    is_griefer                  args( (CHAR_DATA *ch) );
bool	has_weakness			    args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	is_weakness			        args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	asylum_room			        args( (ROOM_INDEX_DATA *room) );
bool	college_dorms			    args( (ROOM_INDEX_DATA *room) );
bool	institute_room			    args( (ROOM_INDEX_DATA *room) );
bool 	visitors_area			    args( (ROOM_INDEX_DATA *room) );
bool	private_clinic_room		    args( (ROOM_INDEX_DATA *room) );
bool	private_school_room		    args( (ROOM_INDEX_DATA *room) );
bool	private_college_room		args( (ROOM_INDEX_DATA *room) );
bool	clinic_visitors_area		args( (ROOM_INDEX_DATA *room) );
bool	has_clinic_visitors_pass	args( (CHAR_DATA *ch) );
bool	arcane_circle			    args( (ROOM_INDEX_DATA *room) );
bool	valid_psychic_target		args( (char * name) );
int	    psychic_soldiers		    args( (char * name) );
bool 	staff_account			    args( (ACCOUNT_TYPE *account) );
bool 	staff_char			        args( (CHAR_DATA *ch) );
bool	cardinal			        args( (CHAR_DATA *ch) );
int	    sincount			        args( (CHAR_DATA *ch));
bool	sinmatch			        args( (CHAR_DATA *ch, CHAR_DATA *victim));
bool	goblin_market			    args( (ROOM_INDEX_DATA *room) );
bool	library_room			    args( (ROOM_INDEX_DATA *room) );
bool 	brainwash_room			    args( (ROOM_INDEX_DATA *room) );
bool	operating_room			    args( (ROOM_INDEX_DATA *room) );
bool	workshop_room			    args( (ROOM_INDEX_DATA *room) );
bool 	engineering_room		    args( (ROOM_INDEX_DATA *room) );
bool	alchemy_room			    args( (ROOM_INDEX_DATA *room) );
bool	harvesting_room			    args( (ROOM_INDEX_DATA *room) );
bool	has_computer			    args( (CHAR_DATA *ch) );
bool	mirror_room			        args( (ROOM_INDEX_DATA *room) );
bool    story_on                    args( (CHAR_DATA *ch) );
bool	move_helpless				args( (CHAR_DATA *ch) );
bool	is_lockable					args( (int vnum) );
bool	is_ill						args( (CHAR_DATA *ch) );
int		base_lifeforce				args( (CHAR_DATA *ch) );
int		unused_lifeforce			args( (CHAR_DATA *ch) );
void	give_lifeforce				args( (CHAR_DATA *ch, int amount, char * reason) );
void	give_lifeforce_nouse	    args( (CHAR_DATA *ch, int amount, char * reason) );
void	use_lifeforce				args( (CHAR_DATA *ch, int amount, char * reason) );
void	take_lifeforce				args( (CHAR_DATA *ch, int amount, char * reason) );

bool	carry_objects				args( (CHAR_DATA *ch) );
bool 	super_aware					args( (CHAR_DATA *ch) );
void	lfstat						args( (CHAR_DATA *ch, CHAR_DATA *view) );
bool	under_opression				args( (CHAR_DATA *ch) );
int		villain_rating				args( (CHAR_DATA *ch) );
bool	in_wilds					args( (int areavnum) );
bool	is_wildsapproved			args( (CHAR_DATA *ch) );
bool	pact_holder					args( (CHAR_DATA *ch) );
int		regress_mod					args( (CHAR_DATA *ch, int stat) );
bool	guestmonster				args( (CHAR_DATA *ch) );
int		custom_vnum					args( (CHAR_DATA *ch) );
int		shade_level					args( (ROOM_INDEX_DATA *room) );
bool	has_relic					args( (CHAR_DATA *ch) );
int		lifeforce_mod				args( (CHAR_DATA *ch, CHAR_DATA *viewer) );
bool	no_tech						args( (CHAR_DATA *ch) );
bool	radio_signal				args( (CHAR_DATA *ch) );
bool 	cell_signal					args( (CHAR_DATA *ch) );
bool	illegal_augmentation		args( (int skill) );
int		in_world					args( (CHAR_DATA *ch) );
int		get_world					args( (CHAR_DATA *ch) );
int		get_lifeforce				args( (CHAR_DATA *ch, bool used, CHAR_DATA *viewer) );
int		get_display_lifeforce		args( (CHAR_DATA *ch) );
int		get_display_lfmod			args( (CHAR_DATA *ch) );
bool    has_institute_contraband    args( (CHAR_DATA *ch) );
bool    institute_contraband        args( (OBJ_DATA  *obj));
void  remove_institute_contraband   args( (CHAR_DATA *ch));
bool    has_practice_arms           args( (CHAR_DATA *ch) );
bool    is_practice_arm             args( (OBJ_DATA  *obj));
bool	has_lweapon					args( (CHAR_DATA *ch) );
bool	has_sweapon					args( (CHAR_DATA *ch) );
bool	has_shield					args( (CHAR_DATA *ch) );
int		combat_focus				args( (CHAR_DATA *ch) );
int		arcane_focus				args( (CHAR_DATA *ch) );
int		prof_focus					args( (CHAR_DATA *ch) );
bool	elligible_modifier			args( (CHAR_DATA *ch, int mod) );
int		min_age						args( (CHAR_DATA *ch) );
int     max_age						args( (CHAR_DATA *ch) );
int     min_aage					args( (CHAR_DATA *ch) );
int     max_aage					args( (CHAR_DATA *ch) );
int		get_tier					args( (CHAR_DATA *ch) );
int		available_exp				args( (CHAR_DATA *ch) );
int 	available_rpexp				args( (CHAR_DATA *ch) );
int		available_karma				args( (CHAR_DATA *ch) );
int		available_pkarma			args( (CHAR_DATA *ch) );
int		available_donated			args( (CHAR_DATA *ch) );
int		available_karmabank			args( (CHAR_DATA *ch) );
int		human_cap					args( (int stat) );
int		armor_cap					args( (CHAR_DATA *ch) );
int		moon_phase					args( ( void ) );
bool	is_neutralized				args( (CHAR_DATA *ch) );
bool	can_see_outside				args( (ROOM_INDEX_DATA *room) );
bool	shadowcloaked				args( (ROOM_INDEX_DATA *room) );
bool	gravesite					args( (ROOM_INDEX_DATA *room) );
bool	crowded_room				args( (ROOM_INDEX_DATA *room) );
bool	spammer						args( (CHAR_DATA *ch) );
bool	valid_money					args( (OBJ_DATA *obj, CHAR_DATA *ch) );
bool	is_cloaked					args( (CHAR_DATA *ch) );
bool	string_alphanum				args( (char * word) );
bool	string_alpha				args( (char * word) );
bool	is_academic					args( (CHAR_DATA *ch) );
bool	is_combatant				args( (CHAR_DATA *ch) );
bool	nighttime					args( (ROOM_INDEX_DATA *room) );
bool	can_fly						args( (CHAR_DATA *ch) );
bool	has_sgun					args( (CHAR_DATA *ch) );
bool	animal_feeder				args( (CHAR_DATA *ch) );
int 	solidity					args( (CHAR_DATA *ch) );
bool	is_undead					args( (CHAR_DATA *ch) );
bool	seems_super					args( (CHAR_DATA *ch) );
bool	seems_under_understanding	args( (CHAR_DATA *ch, CHAR_DATA *pers) );
int		get_race					args( (CHAR_DATA *ch) );
int		get_app_race				args( (CHAR_DATA *ch) );
bool 	has_shroudmanip				args( (CHAR_DATA *ch) );
bool	stealthvassal				args( (char * name) );
bool	is_masked					args( (CHAR_DATA *ch) );
bool	all_covered					args( (CHAR_DATA *ch) );
int 	neweffective_trains			args( (CHAR_DATA *ch, int type, int cost) );
int 	skillpointcost				args( (CHAR_DATA *ch, int skill) );
int		get_roomplot				args( (ROOM_INDEX_DATA *room) );
bool	is_dark						args( (ROOM_INDEX_DATA *room) );
bool	room_is_dark				args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    is_dark_outside				args( (                     ) );
bool    is_outside					args( (CHAR_DATA *ch) );
bool	can_see_dark				args( (CHAR_DATA *ch) );
bool    is_identifiable				args( (CHAR_DATA *viewer, CHAR_DATA *victim) );
bool 	is_mage						args( (CHAR_DATA *ch) );
bool    is_smelly					args( (CHAR_DATA *ch) );
bool	in_sheriff					args( (ROOM_INDEX_DATA *room) );
bool    in_lodge					args( (ROOM_INDEX_DATA *room) );
bool    in_medcent					args( (ROOM_INDEX_DATA *room) );
bool    in_handhq					args( (ROOM_INDEX_DATA *room) );
bool    in_templehq					args( (ROOM_INDEX_DATA *room) );
bool    in_orderhq					args( (ROOM_INDEX_DATA *room) );
bool    can_see_faction				args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int     get_plotcenter				args( (CHAR_DATA *ch) );
int 	get_coord_vnum				args( (int x, int y, int z) );
int		get_roomx					args( (ROOM_INDEX_DATA *room) );
int     get_roomy					args( (ROOM_INDEX_DATA *room) );
int     get_roomz					args( (ROOM_INDEX_DATA *room) );
int		get_dist3d					args( (int xone, int yone, int zone, int xtwo, int ytwo, int ztwo) );
bool	in_haven					args( (ROOM_INDEX_DATA *room) );
bool	in_redlight					args( (ROOM_INDEX_DATA *room) );
bool	in_beachfront				args( (ROOM_INDEX_DATA *room) );
bool	in_waterfront				args( (ROOM_INDEX_DATA *room) );
bool	in_badlands					args( (ROOM_INDEX_DATA *room) );
bool	in_westhaven				args( (ROOM_INDEX_DATA *room) );
bool	in_downtown					args( (ROOM_INDEX_DATA *room) );
bool	prop_in_downtown			args( (PROP_TYPE *prop) );
int		town_minz					args( (ROOM_INDEX_DATA *room, PROP_TYPE *prop) );
int		town_maxz					args( (ROOM_INDEX_DATA *room, PROP_TYPE *prop) );
bool	is_clothed					args( (CHAR_DATA *ch) );
bool 	wearing_armor				args( (CHAR_DATA *ch) );
bool	wearing_collar				args( (CHAR_DATA *ch) );
bool	has_lgun					args( (CHAR_DATA *ch) );
bool	holding_lgun				args( (CHAR_DATA *ch) );
bool	holding_sweapon				args( (CHAR_DATA *ch) );
void	equip_sweapon				args( (CHAR_DATA *ch) );
bool	mindwarded					args( (CHAR_DATA *ch) );
bool	same_pack					args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int		newbie_level				args( (CHAR_DATA *ch) );
bool	can_remoteinvade			args( (CHAR_DATA *ch) );
bool	is_dreaming					args( (CHAR_DATA *ch) );
int		effective_trains			args( (CHAR_DATA *ch) );
bool	silenced					args( (CHAR_DATA *ch) );
bool	largeweapons				args( (CHAR_DATA *ch) );
bool	heavyweapons				args( (CHAR_DATA *ch) );
bool	visiblyarmed				args( (CHAR_DATA *ch) );
bool	visiblybloody				args( (CHAR_DATA *ch) );
bool 	holding_lweapon				args( (CHAR_DATA *ch) );
bool	offworld					args( (CHAR_DATA *ch) );
bool 	has_scuba					args( (CHAR_DATA *ch) );
int		fight_speed					args( (CHAR_DATA *ch) );
bool	can_blood					args( (CHAR_DATA *ch) );
bool	under_understanding			args( (CHAR_DATA *ch, CHAR_DATA *pers) );
bool	can_shroud					args( (CHAR_DATA *ch) );
bool	is_super					args( (CHAR_DATA *ch) );
int		dollars						args( (int amount) );
int		cents						args( (int amount) );
bool	is_underwater				args( (ROOM_INDEX_DATA *room) );
bool	senior_gm					args( (CHAR_DATA *ch) );
bool	is_water					args( (ROOM_INDEX_DATA *room) );
bool	is_air						args( (ROOM_INDEX_DATA *room) );
bool	in_school					args( (CHAR_DATA *ch) );
bool	room_in_school				args( (int vnum) );
bool    in_police_station			args( (CHAR_DATA *ch) );
bool    in_medical_facility			args( (CHAR_DATA *ch) );
bool    in_cosmetic					args( (CHAR_DATA *ch ));
bool	in_water					args( (CHAR_DATA *ch) );
bool	deep_water					args( (CHAR_DATA *ch) );
bool	airborne					args( (CHAR_DATA *ch) );
bool	nearby_water				args( (CHAR_DATA *ch) );
int		get_energy					args( (CHAR_DATA *ch) );
bool	can_spy						args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	water_breathe				args( (CHAR_DATA *ch) );
bool	is_vampire					args( (CHAR_DATA *ch) );
bool	is_werewolf					args( (CHAR_DATA *ch) );
bool	is_angelborn				args( (CHAR_DATA *ch) );
bool	is_demigod					args( (CHAR_DATA *ch) );
bool	is_demonborn				args( (CHAR_DATA *ch) );
bool	is_helpless					args( (CHAR_DATA *ch) );
bool	is_forcibly_helpless		args( (CHAR_DATA *ch) );
bool 	is_faeborn					args( (CHAR_DATA *ch) );
bool	is_wildling					args( (CHAR_DATA *ch) );
bool	is_swimmer					args( (CHAR_DATA *ch) );
bool    is_shifter					args( (CHAR_DATA *ch) );
bool    is_animal					args( (CHAR_DATA *ch) );
bool	is_npcanimal				args( (CHAR_DATA *ch) );
bool    is_hybrid					args( (CHAR_DATA *ch) );
bool	is_mute 					args( (CHAR_DATA *ch) );
bool    is_superjumping				args( (CHAR_DATA *ch) );
bool	is_pinned					args( (CHAR_DATA *ch) );
bool    is_flying					args( (CHAR_DATA *ch) );
bool	is_gm						args( (CHAR_DATA *ch) );
bool    is_guest					args( (CHAR_DATA *ch) );
bool	timetraveler				args( (CHAR_DATA *ch) );
bool	same_player					args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int		shield_total				args( (CHAR_DATA *ch) );
int		shield_total_creation		args( (CHAR_DATA *ch) );
bool	is_fighting					args( (CHAR_DATA *ch) );
int		weapon_number				args( (const char *name) );
int		get_skill					args( (CHAR_DATA *ch, int skill) );
int		get_disc					args( (CHAR_DATA *ch, int disc, bool usingit) );
int		get_lvl						args( (CHAR_DATA *ch) );
int 	race_stat_mod				args( (int race, int stat) );
int		level_vnum					args( ( CHAR_DATA *ch, int vnum) );
bool	has_vnum					args( (CHAR_DATA *ch, int vnum) );\
bool	has_armoredform				args( (CHAR_DATA *ch) );
bool	has_fightingform			args( (CHAR_DATA *ch) );
bool	has_flyingform				args( (CHAR_DATA *ch) );
bool	has_runningform				args( (CHAR_DATA *ch) );
bool	has_stealthform				args( (CHAR_DATA *ch) );
bool	has_swimmingform			args( (CHAR_DATA *ch) );
bool	has_wolfform				args( (CHAR_DATA *ch) );
char *	random_monster_name			args( ( void ) );
char *	random_male_name			args( ( void ) );
char *	random_female_name			args( ( void ) );
int		get_diminish				args( ( CHAR_DATA *ch) );
void	load_names					args( ( void ) );
bool	can_exp						args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool	is_same_align				args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool	is_good						args( ( CHAR_DATA *ch) );
bool	is_evil						args( ( CHAR_DATA *ch) );
bool	is_werewolf					args( ( CHAR_DATA *ch) );
int     get_dam_mod					args( ( OBJ_DATA *obj) );
int     get_acc_mod					args( ( OBJ_DATA *obj) );
int     get_scope_mod				args( ( OBJ_DATA *obj) );
int     get_clip_mod				args( ( OBJ_DATA *obj) );
int     get_range_mod				args( ( OBJ_DATA *obj) );
bool    does_cover					args( ( OBJ_DATA *obj, int selection) );
bool    does_conceal				args( ( OBJ_DATA *obj_over, OBJ_DATA *obj_under) );
bool	is_human					args( ( CHAR_DATA *ch) );
bool	is_monster					args( ( CHAR_DATA *ch) );
bool	is_prey						args( ( CHAR_DATA *ch) );
int		remort_lookup				args( ( const char *name) );
int		race_lookup					args( ( const char *name) );
int		subrace_lookup				args( ( const char *name) );
int		item_lookup					args( ( const char *name) );
int		liq_lookup					args( ( const char *name) );
int 	metal_lookup				args( ( const char *name) );
int		armor_lookup				args( ( const char *name) );
int 	max_hp						args( (CHAR_DATA *ch) );
int		get_lvl						args( (CHAR_DATA *ch) );
int		get_pslvl					args( (CHAR_DATA *ch) );
int		get_cslvl					args( (CHAR_DATA *ch) );
int     get_eca						args( (CHAR_DATA *ch) );
bool	is_pinned					args( (CHAR_DATA *ch) );
int     max_sp						args( (CHAR_DATA *ch) );
int 	metal_lookup_int			args( ( const char *name) );
OBJ_DATA * get_artifact				args( (CHAR_DATA *ch) );
OBJ_DATA * get_armor				args( (CHAR_DATA *ch) );
OBJ_DATA * get_amulet				args( (CHAR_DATA *ch) );
OBJ_DATA * get_expectancy			args( ( CHAR_DATA *ch) );
bool	has_echarm					args( (CHAR_DATA *ch) );
bool    is_big						args( (OBJ_DATA *obj) );
int     get_big_items				args( (CHAR_DATA *ch) );
int     get_small_items				args( (CHAR_DATA *ch) );
char *  smart_a_or_an				args( (char * arg) );
char *  a_or_an						args( (char *arg) );

/* map.c */
bool	world_access		args( (CHAR_DATA *ch, int world) );
bool knows_gateway		args( (CHAR_DATA *ch, int gateway) );
void	makemonstermap		args( (void ));
int	compass_direction	args( (ROOM_INDEX_DATA *room) );
int	offworld_direction	args( (ROOM_INDEX_DATA *room) );
bool 	vision_exists		args( (ROOM_INDEX_DATA *room, int direction) );
int 	roomdirection_3d	args( (int startx, int starty, int startz, int finx, int finy, int finz) );
void	buildmaps		args( (void) );
void 	buildasciimap		args( (void) );
void	build_a_map		args( (CHAR_DATA *ch) );
int 	coord_path		args( (ROOM_INDEX_DATA *in_room, int desx, int desy, int desz, bool xfirst) );
bool	invisioncone_character	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
bool	invisioncone_coordinates	args( (CHAR_DATA *ch, int x, int y) );
bool 	open_sound		args( (ROOM_INDEX_DATA *room, int direction) );
ROOM_INDEX_DATA * sourced_room_by_coordinates	args( (ROOM_INDEX_DATA *orig, int desx, int desy, int desz, bool xfirst) );
bool	can_hear		args( (CHAR_DATA *ch, CHAR_DATA *victim, int volume) );
int 	roomdirection		args( (int startx, int starty, int finx, int finy) );
bool 	charlineofsight_landmark		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *desti) );
int	charlineofsight_character	args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int	charlineofsight_room		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *desti) );
void	maketownmap		args( (CHAR_DATA *ch) );
void	makeworldmap		args( (void) );
int 	path_dir		args( (ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination, int moving, CHAR_DATA *ch) );
int	street_distance		args( (ROOM_INDEX_DATA *orig, ROOM_INDEX_DATA *desti, CHAR_DATA *ch) );
/* olc.c */
void	make_mob		args( ( int value) );
void	make_object		args( ( int value) );
bool	run_olc_editor		args( ( DESCRIPTOR_DATA *d ) );
char	*olc_ed_name		args( ( CHAR_DATA *ch ) );
char	*olc_ed_vnum		args( ( CHAR_DATA *ch ) );

void	locate_mob_reset	args( ( CHAR_DATA *ch, int vnum, bool equip_list ) );
void	locate_equip_reset	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom, MOB_INDEX_DATA *pMob, Buffer *outbuf ) );

/* quest.c */
ROOM_INDEX_DATA *get_quest_corner	args( (void) );
ROOM_INDEX_DATA *get_quest_room		args( (void) );
bool	has_exits		args( (ROOM_INDEX_DATA *room) );
void	give_auto_quest		args( ( CHAR_DATA *ch) );
void	gain_quest_exp		args( ( CHAR_DATA *ch) );
bool	is_subtype		args( ( CHAR_DATA *ch, int type) );

/* web.c */
void 	init_web		args( ( int port) );
void 	handle_web		args( ( void ) );
void 	shutdown_web		args( ( void ) );

/* world.c */
ROOM_INDEX_DATA *directional_outerforest args( (int dir) );

bool world_blocked		args( (CHAR_DATA *ch, int world) );
bool decree_target		args( (int world, int decree_type, char * name) );
bool territory_leader	args( (CHAR_DATA *ch, LOCATION_TYPE *loc) );
void build_basement		args( (PROP_TYPE *prop) );
void fix_room_dirs		args( (ROOM_INDEX_DATA *room) );

bool	border_territory	args( (LOCATION_TYPE *loc) );
void clean_room			args( (ROOM_INDEX_DATA *room) );
void prepass_gate		args( (CHAR_DATA *ch, ROOM_INDEX_DATA *fromroom, ROOM_INDEX_DATA *toroom, int dir) );
void postpass_gate               args( (CHAR_DATA *ch, ROOM_INDEX_DATA *fromroom, ROOM_INDEX_DATA *toroom, int dir) );

ROOM_INDEX_DATA * cardinal_room args( (ROOM_INDEX_DATA *first, int dir) );
void makesky			args( (int x, int y, int z, int area, bool overwrite) );
LOCATION_TYPE *antag_target	args( (int faction) );
bool	is_terr_war		args( ( void ) );
int	number_unrest		args( ( void ) );
void 	add_to_timeline		args( (LOCATION_TYPE *loc, char * argument) );
void	offworld_weather_update	args( ( void ) );
void	create_room		args( (int iVnum) );
void 	airbubble		args( (ROOM_INDEX_DATA *room, bool overwrite) );
bool 	coordinates_exist	args( (int x, int y, int z) );
ROOM_INDEX_DATA *room_by_coordinates	args( (int x, int y, int z) );
void 	show_territory_to_char	args( (CHAR_DATA *ch, LOCATION_TYPE *loc) );
char *  offworld_weather_forecast	args( (int world, int havenhour, int climate) );
void	assign_offworld_weather	args( (ROOM_INDEX_DATA *room, int world, int climate) );
int	number_from_territory	args( (LOCATION_TYPE *terr) );
char *	weather_forecast	args( (LOCATION_TYPE *territory, int havenhour) );
void	assign_weather		args( (ROOM_INDEX_DATA *room, LOCATION_TYPE *territory, bool climate) );
void	kill_room		args( (ROOM_INDEX_DATA *room) );
int 	first_terr_points	args( (LOCATION_TYPE *loc) );
int     second_terr_points      args( (LOCATION_TYPE *loc) );
char *  first_dom_terr 	      	args( (LOCATION_TYPE *loc) );
char *  second_dom_terr       	args( (LOCATION_TYPE *loc) );

void 	territory_update	args( ( void ) );
void	territory_plus		args( (LOCATION_TYPE *loc, int type) );
void	territory_minus		args( (LOCATION_TYPE *loc, int type) );
void	antagonist_plus		args( (LOCATION_TYPE *loc, FACTION_TYPE *fac) );
void	antagonist_minus	args( (LOCATION_TYPE *loc) );
LOCATION_TYPE * territory_by_number	args( (int number) );
int	territory_count		args( ( void ) );
LOCATION_TYPE *	get_loc		args( (char * argument) );
void 	boost_territory		args( (LOCATION_TYPE *loc, int type, int amount, bool loop) );
int     get_coord               args( (int room_number) );
int 	get_coordx		args( (int vnum) );
int 	get_coordy		args( (int vnum) );
void	smartdig		args( (ROOM_INDEX_DATA *orig, int direction, int area) );
void	smartdemolish		args( (ROOM_INDEX_DATA *orig, int direction, int area) );
char	*stock_description	args( (CHAR_DATA *ch, ROOM_INDEX_DATA *room) );	
void    world_sync          args( (char type[MSL]) );
void	load_locations		args( ( void ) );
void	save_locations		args( ( void ) );
void	load_gateways		args( ( void ) );
void	save_gateways		args( ( void ) );

/* Note.c */
void updateReadTime		args( (CHAR_DATA *ch, NoteBoard *pNoteBoard, long value) );

/* Update.c */
void lockdown_academy   args( (int on) );	

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef AD

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif

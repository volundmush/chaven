/***************************************************************************
 *  File: olc.h                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  -Jason Dinkel
 */

#ifndef _OLC_H_
#define _OLC_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define VERSION	"ILAB Online Creation [Beta 1.0, ROM 2.3 modified]\n\r"	\
		"     Port a ROM 2.4 v1.8\n\r"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)\n\r" 	\
                "     Modified for use with ROM 2.3\n\r"		\
                "     By Hans Birkeland (hansbi@ifi.uio.no)\n\r"	\
                "     Modificado para uso en ROM 2.4b6\n\r"		\
                "     Por Ivan Toledo (itoledo@ctcreuna.cl)\n\r"
#define DATE	"     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r"	\
		"     (Port a ROM 2.4 - Nov 2, 1996)\n\r" 		\
		"     Version actual : 1.8 - Sep 8, 1998\n\r"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"



/*
 * New typedefs.
 */
typedef	bool OLC_FUN		args( ( CHAR_DATA *ch, char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun


/* Command procedures needed ROM OLC */
DECLARE_DO_FUN(    do_help    );
DECLARE_SPELL_FUN( spell_null );


/*
 * Connected states for editor.
 */
#define ED_NONE		0
#define ED_AREA		1
#define ED_ROOM		2
#define ED_OBJECT	3
#define ED_MOBILE	4
#define ED_MPCODE	5
#define ED_OPCODE	6
#define ED_RPCODE	7
#define ED_HELP		8
#define ED_GUILD	9
#define ED_SUBRACE 	10
#define ED_CITY		11
#define ED_LANDMARK	12
#define ED_ENTRANCE	13
#define ED_CREATION	14
#define ED_CREATION_SK	15
#define ED_VESSEL	16
#define ED_PORT		17
#define ED_GROUP	18
#define ED_NEWTURN	19

/*
 * Interpreter Prototypes
 */
DECLARE_DO_FUN(aedit);
DECLARE_DO_FUN(redit);
DECLARE_DO_FUN(medit);
DECLARE_DO_FUN(oedit);
DECLARE_DO_FUN(mpedit);
DECLARE_DO_FUN(opedit);
DECLARE_DO_FUN(rpedit);
DECLARE_DO_FUN(hedit);
DECLARE_DO_FUN(cityedit);
DECLARE_DO_FUN(guildedit);
DECLARE_DO_FUN(sraceedit);
DECLARE_DO_FUN(vesseledit);
DECLARE_DO_FUN(portedit);

//void    aedit           args( ( CHAR_DATA *ch, char *argument ) );
//void    redit           args( ( CHAR_DATA *ch, char *argument ) );
//void    medit           args( ( CHAR_DATA *ch, char *argument ) );
//void    oedit           args( ( CHAR_DATA *ch, char *argument ) );
//void	mpedit		args( ( CHAR_DATA *ch, char *argument ) );
//void	opedit		args( ( CHAR_DATA *ch, char *argument ) );
//void	rpedit		args( ( CHAR_DATA *ch, char *argument ) );
//void	hedit		args( ( CHAR_DATA *, char * ) );
//void	cityedit	args( ( CHAR_DATA *ch, char *argument ) );
//void	guildedit	args( ( CHAR_DATA *ch, char *argument ) );
//void	sraceedit	args( ( CHAR_DATA *ch, char *argument ) );
//void	vesseledit	args( ( CHAR_DATA *ch, char *argument ) );
//void	portedit	args( ( CHAR_DATA *ch, char *argument ) );


/*
 * OLC Constants
 */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */

#define ALT_FLAGVALUE_SET( _blargh, _table, _arg )              \
        {                                                       \
                int blah = flag_value( _table, _arg );          \
                _blargh = (blah == NO_FLAG) ? 0 : blah;         \
        }

#define ALT_FLAGVALUE_TOGGLE( _blargh, _table, _arg )           \
        {                                                       \
                int blah = flag_value( _table, _arg );          \
                _blargh ^= (blah == NO_FLAG) ? 0 : blah;        \
        }


/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type
{
    char * 	name;
    OLC_FUN *		olc_fun;
};

/*
 * Structure for an OLC editor startup command.
 */
struct	editor_cmd_type
{
    char * 			name;
    DO_FUN *		do_fun;
};



/*
 * Utils.
 */
AREA_DATA *get_vnum_area	args ( ( int vnum ) );
AREA_DATA *get_area_data	args ( ( int vnum ) );
typedef struct  	flag_type           FLAG_TYPE;
int flag_value			args ( ( const struct flag_type *flag_table,
				         char *argument) );
char *flag_string		args ( ( const struct flag_type *flag_table,
				         int bits ) );
void add_reset			args ( ( ROOM_INDEX_DATA *room, 
				         RESET_DATA *pReset, int index ) );



/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	mpedit_table[];
extern const struct olc_cmd_type	opedit_table[];
extern const struct olc_cmd_type	rpedit_table[];
extern const struct olc_cmd_type	hedit_table[];
extern const struct olc_cmd_type	cityedit_table[];
extern const struct olc_cmd_type	gedit_table[];
extern const struct olc_cmd_type	subraceedit_table[];
extern const struct olc_cmd_type	vedit_table[];
extern const struct olc_cmd_type	pedit_table[];
extern const struct olc_cmd_type	groupedit_table[];
extern const struct editor_cmd_type editor_table[];
/*
 * Editor Commands.
 */
DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );
DECLARE_DO_FUN( do_mpedit	);
DECLARE_DO_FUN( do_opedit	);
DECLARE_DO_FUN( do_rpedit	);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN( do_cityedit	);
DECLARE_DO_FUN( do_guildedit	);
DECLARE_DO_FUN( do_subraceedit	);
DECLARE_DO_FUN( do_vesseledit	);
DECLARE_DO_FUN( do_portedit	);
DECLARE_DO_FUN( do_groupedit	);

/*
 * General Functions
 */
bool show_commands		args ( ( CHAR_DATA *ch, char *argument ) );
bool show_help			args ( ( CHAR_DATA *ch, char *argument ) );
bool edit_done			args ( ( CHAR_DATA *ch ) );
bool show_version		args ( ( CHAR_DATA *ch, char *argument ) );

/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN( aedit_show		);
DECLARE_OLC_FUN( aedit_create		);
DECLARE_OLC_FUN( aedit_delete		);
DECLARE_OLC_FUN( aedit_name		);
DECLARE_OLC_FUN( aedit_file		);
DECLARE_OLC_FUN( aedit_age		);
DECLARE_OLC_FUN( aedit_location		);
/* DECLARE_OLC_FUN( aedit_recall	);       ROM OLC */
DECLARE_OLC_FUN( aedit_reset		);
DECLARE_OLC_FUN( aedit_security		);
DECLARE_OLC_FUN( aedit_builder		);
DECLARE_OLC_FUN( aedit_vnum		);
DECLARE_OLC_FUN( aedit_lvnum		);
DECLARE_OLC_FUN( aedit_uvnum		);
DECLARE_OLC_FUN( aedit_credits		);
DECLARE_OLC_FUN( aedit_completed	);
DECLARE_OLC_FUN( aedit_areatype		);
DECLARE_OLC_FUN( aedit_world		);
DECLARE_OLC_FUN( aedit_range		);
/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN( redit_show			);
DECLARE_OLC_FUN( redit_create		);
DECLARE_OLC_FUN( redit_name			);
DECLARE_OLC_FUN( redit_subarea		);  // Discordance
DECLARE_OLC_FUN( redit_desc			);
DECLARE_OLC_FUN( redit_ed			);
DECLARE_OLC_FUN( redit_place		);
DECLARE_OLC_FUN( redit_format		);
DECLARE_OLC_FUN( redit_north		);
DECLARE_OLC_FUN( redit_northeast	);
DECLARE_OLC_FUN( redit_northwest	);
DECLARE_OLC_FUN( redit_south		);
DECLARE_OLC_FUN( redit_southeast	);
DECLARE_OLC_FUN( redit_southwest	);
DECLARE_OLC_FUN( redit_east			);
DECLARE_OLC_FUN( redit_west			);
DECLARE_OLC_FUN( redit_up			);
DECLARE_OLC_FUN( redit_down			);
DECLARE_OLC_FUN( redit_mreset		);
DECLARE_OLC_FUN( redit_oreset		);
DECLARE_OLC_FUN( redit_mlist		);
DECLARE_OLC_FUN( redit_rlist		);
DECLARE_OLC_FUN( redit_olist		);
DECLARE_OLC_FUN( redit_mshow		);
DECLARE_OLC_FUN( redit_oshow		);
DECLARE_OLC_FUN( redit_groupshow	);
DECLARE_OLC_FUN( redit_level		);
DECLARE_OLC_FUN( redit_shroud		);
DECLARE_OLC_FUN( redit_mana			);
DECLARE_OLC_FUN( redit_clan			);
DECLARE_OLC_FUN( redit_owner		);
DECLARE_OLC_FUN( redit_room			);
DECLARE_OLC_FUN( redit_sector		);
DECLARE_OLC_FUN( redit_xy           );
DECLARE_OLC_FUN( redit_coordinates	);
DECLARE_OLC_FUN( redit_size         );
DECLARE_OLC_FUN( redit_entry        );
DECLARE_OLC_FUN( redit_feature      );
DECLARE_OLC_FUN( redit_sectorset	);
DECLARE_OLC_FUN( redit_copy			);
DECLARE_OLC_FUN( redit_addrprog		);
DECLARE_OLC_FUN( redit_delrprog		);

/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN( oedit_show		);
DECLARE_OLC_FUN( oedit_spec		);
DECLARE_OLC_FUN( oedit_statset		);
DECLARE_OLC_FUN( oedit_create		);
DECLARE_OLC_FUN( oedit_delete		);
DECLARE_OLC_FUN( oedit_name		);
DECLARE_OLC_FUN( oedit_short		);
DECLARE_OLC_FUN( oedit_long		);
DECLARE_OLC_FUN( oedit_addaffect	);
DECLARE_OLC_FUN( oedit_addapply		);
DECLARE_OLC_FUN( oedit_delaffect	);
DECLARE_OLC_FUN( oedit_value0		);
DECLARE_OLC_FUN( oedit_value1		);
DECLARE_OLC_FUN( oedit_value2		);
DECLARE_OLC_FUN( oedit_value3		);
DECLARE_OLC_FUN( oedit_value4		);  /* ROM */
DECLARE_OLC_FUN( oedit_value5		);  /* Patterns */
DECLARE_OLC_FUN( oedit_size		);
DECLARE_OLC_FUN( oedit_cost		);
DECLARE_OLC_FUN( oedit_ed		);

DECLARE_OLC_FUN( oedit_extra            );  /* ROM */
DECLARE_OLC_FUN( oedit_wear             );  /* ROM */
DECLARE_OLC_FUN( oedit_rottimer		);  /* Patterns */
DECLARE_OLC_FUN( oedit_type             );  /* ROM */
DECLARE_OLC_FUN( oedit_affect           );  /* ROM */
DECLARE_OLC_FUN( oedit_material		);  /* ROM */
DECLARE_OLC_FUN( oedit_wearstring	);
DECLARE_OLC_FUN( oedit_lchance          );  /* Patterns */
DECLARE_OLC_FUN( oedit_level            );  /* ROM */
DECLARE_OLC_FUN( oedit_fuzzy		);
DECLARE_OLC_FUN( oedit_condition        );  /* ROM */
DECLARE_OLC_FUN( oedit_copy		);
DECLARE_OLC_FUN( oedit_addoprog		);
DECLARE_OLC_FUN( oedit_deloprog		);

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN( medit_target		);
DECLARE_OLC_FUN( medit_show		);
DECLARE_OLC_FUN( medit_create		);
DECLARE_OLC_FUN( medit_delete		);
DECLARE_OLC_FUN( medit_name		);
DECLARE_OLC_FUN( medit_short		);
DECLARE_OLC_FUN( medit_long		);
DECLARE_OLC_FUN( medit_shop		);
DECLARE_OLC_FUN( medit_restring		);
DECLARE_OLC_FUN( medit_desc		);
DECLARE_OLC_FUN( medit_level		);
DECLARE_OLC_FUN( medit_fuzzy		);
DECLARE_OLC_FUN( medit_align		);
DECLARE_OLC_FUN( medit_exp		);
DECLARE_OLC_FUN(medit_ttl		);
DECLARE_OLC_FUN( medit_spec		);
DECLARE_OLC_FUN( medit_statset		);
//DECLARE_OLC_FUN( medit_copy		);

DECLARE_OLC_FUN( medit_intelligence	);
DECLARE_OLC_FUN( medit_wounds		);
DECLARE_OLC_FUN( medit_weapon		);

DECLARE_OLC_FUN( medit_sex		);  /* ROM */
DECLARE_OLC_FUN( medit_speed		);
DECLARE_OLC_FUN( medit_discipline	);
DECLARE_OLC_FUN( medit_act		);  /* ROM */
DECLARE_OLC_FUN( medit_affect		);  /* ROM */
DECLARE_OLC_FUN( medit_ac		);  /* ROM */
DECLARE_OLC_FUN( medit_form		);  /* ROM */
DECLARE_OLC_FUN( medit_part		);  /* ROM */
DECLARE_OLC_FUN( medit_imm		);  /* ROM */
DECLARE_OLC_FUN( medit_res		);  /* ROM */
DECLARE_OLC_FUN( medit_vuln		);  /* ROM */
DECLARE_OLC_FUN( medit_material		);  /* ROM */
DECLARE_OLC_FUN( medit_off		);  /* ROM */
DECLARE_OLC_FUN( medit_size		);  /* ROM */
DECLARE_OLC_FUN( medit_hitdice		);  /* ROM */
DECLARE_OLC_FUN( medit_manadice		);  /* ROM */
DECLARE_OLC_FUN( medit_damdice		);  /* ROM */
DECLARE_OLC_FUN( medit_race		);  /* ROM */
DECLARE_OLC_FUN( medit_subrace		);  /* ROM */
DECLARE_OLC_FUN( medit_country		);
DECLARE_OLC_FUN( medit_position		);  /* ROM */
DECLARE_OLC_FUN( medit_gold		);  /* ROM */
DECLARE_OLC_FUN( medit_hitroll		);  /* ROM */
DECLARE_OLC_FUN( medit_damtype		);  /* ROM */
DECLARE_OLC_FUN( medit_group		);  /* ROM */
DECLARE_OLC_FUN( medit_addmprog		);  /* ROM */
DECLARE_OLC_FUN( medit_delmprog		);  /* ROM */
DECLARE_OLC_FUN( medit_agility		);  /* ROM */
DECLARE_OLC_FUN( medit_teaches		);  /* Patterns*/
DECLARE_OLC_FUN( medit_guild		);  /* Patterns*/

/* Mobprog editor */
DECLARE_OLC_FUN( mpedit_create		);
DECLARE_OLC_FUN( mpedit_code		);
DECLARE_OLC_FUN( mpedit_show		);
DECLARE_OLC_FUN( mpedit_list		);

/* Editor de helps */
DECLARE_OLC_FUN( hedit_see_also		);
DECLARE_OLC_FUN( hedit_type		);
DECLARE_OLC_FUN( hedit_guild		);
DECLARE_OLC_FUN( hedit_keyword		);
DECLARE_OLC_FUN( hedit_text		);
DECLARE_OLC_FUN( hedit_new		);
DECLARE_OLC_FUN( hedit_level		);
DECLARE_OLC_FUN( hedit_delete		);
DECLARE_OLC_FUN( hedit_online		);
DECLARE_OLC_FUN( hedit_show		);
DECLARE_OLC_FUN( hedit_list		);
DECLARE_OLC_FUN( hedit_consol		);
DECLARE_OLC_FUN( hedit_guild		);

/* Guild editor */
DECLARE_OLC_FUN( gedit_show		);
DECLARE_OLC_FUN( gedit_type		);
DECLARE_OLC_FUN( gedit_name		);
DECLARE_OLC_FUN( gedit_whoname		);
DECLARE_OLC_FUN( gedit_recall		);
DECLARE_OLC_FUN( gedit_enable		);
DECLARE_OLC_FUN( gedit_create		);
DECLARE_OLC_FUN( gedit_delete		);
DECLARE_OLC_FUN( gedit_skill		);
DECLARE_OLC_FUN( gedit_tax		);
DECLARE_OLC_FUN( gedit_week_check	);
DECLARE_OLC_FUN( gedit_members		);

/* SubRace Editor */
DECLARE_OLC_FUN( subraceedit_show	);
DECLARE_OLC_FUN( subraceedit_name	);
DECLARE_OLC_FUN( subraceedit_whoname	);
DECLARE_OLC_FUN( subraceedit_enable	);
DECLARE_OLC_FUN( subraceedit_create	);
DECLARE_OLC_FUN( subraceedit_delete	);
DECLARE_OLC_FUN( subraceedit_parentname	);
DECLARE_OLC_FUN( subraceedit_str	);
DECLARE_OLC_FUN( subraceedit_int	);
DECLARE_OLC_FUN( subraceedit_wis	);
DECLARE_OLC_FUN( subraceedit_dex	);
DECLARE_OLC_FUN( subraceedit_con	);
DECLARE_OLC_FUN( subraceedit_adr	);
DECLARE_OLC_FUN( subraceedit_agi	);
DECLARE_OLC_FUN( subraceedit_awa	);
DECLARE_OLC_FUN( subraceedit_cnc	);
DECLARE_OLC_FUN( subraceedit_ref	);
DECLARE_OLC_FUN( subraceedit_luck	);
DECLARE_OLC_FUN( subraceedit_skill	);
DECLARE_OLC_FUN( subraceedit_pc_subrace	);
DECLARE_OLC_FUN( subraceedit_temp	);
DECLARE_OLC_FUN( subraceedit_remort	);
DECLARE_OLC_FUN( subraceedit_act	);
DECLARE_OLC_FUN( subraceedit_affect	);
DECLARE_OLC_FUN( subraceedit_form	);
DECLARE_OLC_FUN( subraceedit_part	);
DECLARE_OLC_FUN( subraceedit_imm	);
DECLARE_OLC_FUN( subraceedit_res	);
DECLARE_OLC_FUN( subraceedit_vuln	);
DECLARE_OLC_FUN( subraceedit_off	);
DECLARE_OLC_FUN( subraceedit_size	);

/* City editor */
DECLARE_OLC_FUN( cityedit_show		);
DECLARE_OLC_FUN( cityedit_name		);
DECLARE_OLC_FUN( cityedit_recall	);
DECLARE_OLC_FUN( cityedit_enable	);
DECLARE_OLC_FUN( cityedit_create	);
DECLARE_OLC_FUN( cityedit_delete	);

/* Landmark Editor */
DECLARE_OLC_FUN( landmarkedit_coord             );
DECLARE_OLC_FUN( landmarkedit_create            );
DECLARE_OLC_FUN( landmarkedit_delete            );
DECLARE_OLC_FUN( landmarkedit_description       );  
DECLARE_OLC_FUN( landmarkedit_distance          );
DECLARE_OLC_FUN( landmarkedit_map               );
DECLARE_OLC_FUN( landmarkedit_name              );
DECLARE_OLC_FUN( landmarkedit_show              );
    
/* Landmark Editor */
DECLARE_OLC_FUN( entranceedit_cansee		);
DECLARE_OLC_FUN( entranceedit_create            );
DECLARE_OLC_FUN( entranceedit_delete            );
DECLARE_OLC_FUN( entranceedit_exitcoord         );
DECLARE_OLC_FUN( entranceedit_exitmap           );
DECLARE_OLC_FUN( entranceedit_forced		);
DECLARE_OLC_FUN( entranceedit_name              );
DECLARE_OLC_FUN( entranceedit_oncoord           );
DECLARE_OLC_FUN( entranceedit_onmap             );
DECLARE_OLC_FUN( entranceedit_priority		);
DECLARE_OLC_FUN( entranceedit_show              );
DECLARE_OLC_FUN( entranceedit_vnum              );

/* Vessel Editor */

DECLARE_OLC_FUN( vedit_show			);
DECLARE_OLC_FUN( vedit_book_cost		);
DECLARE_OLC_FUN( vedit_create			);
DECLARE_OLC_FUN( vedit_delete			);
DECLARE_OLC_FUN( vedit_desti			);
DECLARE_OLC_FUN( vedit_enable			);
DECLARE_OLC_FUN( vedit_hport			);
DECLARE_OLC_FUN( vedit_onmap			);
DECLARE_OLC_FUN( vedit_name			);
DECLARE_OLC_FUN( vedit_oncoord			);
DECLARE_OLC_FUN( vedit_pcvessel			);
DECLARE_OLC_FUN( vedit_sperm			);
DECLARE_OLC_FUN( vedit_vtype			);
DECLARE_OLC_FUN( vedit_wperm			);
DECLARE_OLC_FUN( vedit_vnums			);

/* Port Editor */
DECLARE_OLC_FUN( pedit_show			);
DECLARE_OLC_FUN( pedit_boats			);
DECLARE_OLC_FUN( pedit_create			);
DECLARE_OLC_FUN( pedit_delete			);
DECLARE_OLC_FUN( pedit_enable			);
DECLARE_OLC_FUN( pedit_name			);
DECLARE_OLC_FUN( pedit_location			);
DECLARE_OLC_FUN( pedit_oncoord			);
DECLARE_OLC_FUN( pedit_onmap			);

/* Group Editor */
DECLARE_OLC_FUN( groupedit_create		);
DECLARE_OLC_FUN( groupedit_show			);
DECLARE_OLC_FUN( groupedit_member		);


/*
 * Macros
 */
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)Ch->desc->pEdit )
#define EDIT_MPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_OPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_RPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_HELP(Ch, Help)     ( Help = (HELP_DATA *)Ch->desc->pEdit )
#define EDIT_GROUP( Ch, Group)  ( Group = ( GROUP_INDEX_DATA *)Ch->desc->pEdit )

/*
 * Prototypes
 */
/* mem.c - memory prototypes. */
#define ED	EXTRA_DESCR_DATA
RESET_DATA	*new_reset_data		args ( ( void ) );
void		free_reset_data		args ( ( RESET_DATA *pReset ) );
AREA_DATA	*new_area		args ( ( void ) );
void		free_area		args ( ( AREA_DATA *pArea ) );
EXIT_DATA	*new_exit		args ( ( void ) );
void		free_exit		args ( ( EXIT_DATA *pExit ) );
ED 		*new_extra_descr	args ( ( void ) );
void		free_extra_descr	args ( ( ED *pExtra ) );
ROOM_INDEX_DATA *new_room_index		args ( ( void ) );
void		free_room_index		args ( ( ROOM_INDEX_DATA *pRoom ) );
GROUP_INDEX_DATA *new_group_index	args ( ( void ) );
void		free_group_index	args ( ( GROUP_INDEX_DATA *pGroup ) );
AFFECT_DATA	*new_affect		args ( ( void ) );
void		free_affect		args ( ( AFFECT_DATA* pAf ) );
SHOP_DATA	*new_shop		args ( ( void ) );
RESTRING_DATA	*new_restring		args ( ( void ) );
void		free_shop		args ( ( SHOP_DATA *pShop ) );
OBJ_INDEX_DATA	*new_obj_index		args ( ( void ) );
void		free_obj_index		args ( ( OBJ_INDEX_DATA *pObj ) );
MOB_INDEX_DATA	*new_mob_index		args ( ( void ) );
void		free_mob_index		args ( ( MOB_INDEX_DATA *pMob ) );
#undef	ED

void		show_liqlist		args ( ( CHAR_DATA *ch ) );
void		show_coinlist		args ( ( CHAR_DATA *ch ) );
void		show_damlist		args ( ( CHAR_DATA *ch ) );

char *		prog_type_to_name	args ( ( int type ) );
PROG_LIST      *new_mprog              args ( ( void ) );
void            free_mprog              args ( ( PROG_LIST *mp ) );
PROG_CODE	*new_mpcode		args ( (void) );
void		free_mpcode		args ( ( PROG_CODE *pMcode));

PROG_LIST	*new_oprog		args ( ( void ) );
void		free_oprog		args ( ( PROG_LIST *op ) );
PROG_LIST	*new_rprog		args ( ( void ) );
void		free_rprog		args ( ( PROG_LIST *rp ) );
PROG_CODE	*new_opcode		args ( ( void ) );
void		free_opcode		args ( ( PROG_CODE *pOcode ) );
PROG_CODE	*new_rpcode		args ( ( void ) );
void		free_rpcode		args ( ( PROG_CODE *pRcode ) );


HELP_DATA 	*new_help 		args ( (void) );
void 		free_help  		args( ( HELP_DATA * pHelp));
void 		save_helps 		args( (void) );

/* Objprog editor */
DECLARE_OLC_FUN( opedit_create		);
DECLARE_OLC_FUN( opedit_code		);
DECLARE_OLC_FUN( opedit_show		);
DECLARE_OLC_FUN( opedit_list		);

/* Roomprog editor */
DECLARE_OLC_FUN( rpedit_create		);
DECLARE_OLC_FUN( rpedit_code		);
DECLARE_OLC_FUN( rpedit_show		);
DECLARE_OLC_FUN( rpedit_list		);

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif

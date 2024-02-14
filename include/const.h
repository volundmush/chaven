#ifndef _CONST_H_
#define _CONST_H_

#if defined(__cplusplus)
extern "C" {
#endif

/* ea */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH

/* helpfile appendings */
#define HELP_FILE 	"help.txt" /* For undefined helps */


/*
 * Accommodate old non-Ansi compilers.
 */
/*
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
b#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#define DECLARE_SPELL_HEAL_FUN( fun )   void fun( )
#define DECLARE_HERB_FUN( fun )		void fun( )
#define DECLARE_OBJ_FUN( fun )		void fun( )
#define DECLARE_ROOM_FUN( fun )		void fun( )
#define DECLARE_FORM_FUN( fun )		void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#define DECLARE_SPELL_HEAL_FUN( fun )	SPELL_HEAL_FUN fun
#define DECLARE_HERB_FUN( fun )		HERB_FUN fun
#define DECLARE_OBJ_FUN( fun )		OBJ_FUN fun
#define DECLARE_ROOM_FUN( fun )		ROOM_FUN fun
#define DECLARE_FORM_FUN( fun )		FORM_FUN fun
#endif
*/
#define MAX_ROW		20
#define COLUMNS		8

/* * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 2048 //This used to be 1024 - Discordance
#define MAX_STRING_LENGTH	 27648
#define MAX_INPUT_LENGTH	  6000
//#define MAX_INPUT_LENGTH          1000
#define PAGELEN			   34

/* Auction Stuff */
#define MINIMUM_BID		10
#define AUCTION_LENGTH		3

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_MEM_LIST 		   24  //This used to be 12 and is also defined in sys.c - Discordance
#define CURR_VERSION		    7
#define MAX_SOCIALS		  256
#define MAX_WEAVE		   95
#define MAX_FORM		   34
#define MAX_HERB		   20
#define MAX_GROUP		   30
#define MAX_IN_GROUP		   15
#define MAX_ALIAS		    5
#define MAX_CLASS		    10
#define MAX_CLASS_TYPE 		   4
#define MAX_PC_RACE		   41
#define MAX_CLAN		    3
#define MAX_DAMAGE_MESSAGE	   44
#define MAX_INFORM		   15
#define MAX_MEMORIZE		   2
#define MEM_MAXABIL		   0
#define MAX_PASSWORD_ATTEMPTS   3
#define MAX_HOTSPOTS		   18
#define MAX_PUNCH		12
#define MAX_KICK		15
#define MAX_FINISH		43
#define MAX_STANCE		80
#define MAX_JOBS		0


#define CONNECT_NONE		0
#define CONNECT_DIALING		1
#define CONNECT_RINGING		2
#define CONNECT_TALKING		3

#define MAX_LEVEL                 111
#define LEVEL_ADMIN               (MAX_LEVEL - 6)
#define LEVEL_ATTENDING           LEVEL_ADMIN  
#define LEVEL_CODER               LEVEL_ADMIN   
#define LEVEL_JUNIOR              (MAX_LEVEL - 9)
#define LEVEL_RESIDENT            LEVEL_JUNIOR  
#define LEVEL_IMMORTAL            (MAX_LEVEL - 9)
#define LEVEL_HERO                (MAX_LEVEL - 10)

#define ADMIN 			LEVEL_ADMIN
#define CODER			LEVEL_CODER
#define JUNIOR			LEVEL_JUNIOR
#define IMMORTAL		LEVEL_IMMORTAL
#define HERO			LEVEL_HERO
#define IMPLEMENTOR     MAX_LEVEL
#define IMP			    MAX_LEVEL


#define L1			(MAX_LEVEL - 1)
#define L2			(MAX_LEVEL - 2)
#define L3			(MAX_LEVEL - 3)
#define L4			(MAX_LEVEL - 4)
#define L5			(MAX_LEVEL - 5)
#define L6			(MAX_LEVEL - 6)
#define L7			(MAX_LEVEL - 7)
#define L8			(MAX_LEVEL - 8)
#define L9			(MAX_LEVEL - 9)
#define L10			(MAX_LEVEL - 10)
#define L11			(MAX_LEVEL - 11)
#define L12			(MAX_LEVEL - 12)
#define L13			(MAX_LEVEL - 13)
#define L14			(MAX_LEVEL - 14)

#define MAX_HFILE_TYPE		    8

#define DEP_PLAYER		        0
#define DEP_BUILDING		    1
#define DEP_ADMIN		        2
#define DEP_MAX			        3
#define MAX_DEP			        3

#define STATUS_NONE		        0
#define STATUS_INTERN		    1
#define STATUS_RESIDENT		    2
#define STATUS_ATTENDING	    3
#define STATUS_IMP		        4


#define RACE_CIVILIAN		        0
#define RACE_LOCAL			1
#define RACE_VISITOR			2
#define RACE_STUDENT			3
#define RACE_SOLDIER			4
#define RACE_TIMESWEPT			5
#define RACE_WILDLING			6
#define RACE_BROWN			7
#define RACE_IMPORTANT			8
#define RACE_DEPUTY			9
#define RACE_NEWVAMPIRE			10
#define RACE_NEWWEREWOLF		11
#define RACE_NEWFAEBORN			12
#define RACE_NEWANGELBORN		13
#define RACE_NEWDEMONBORN		14
#define RACE_NEWDEMIGOD			15
#define RACE_GIFTED			16
#define RACE_SFORCES			17
#define RACE_CELEBRITY			18
#define RACE_PILLAR			19
#define RACE_VETVAMPIRE			20
#define RACE_VETWEREWOLF		21
#define RACE_VETFAEBORN			22
#define RACE_VETANGELBORN		23
#define RACE_VETDEMONBORN		24
#define RACE_VETDEMIGOD			25
#define RACE_VETGIFTED			26
#define RACE_OLDVAMPIRE			27
#define RACE_OLDWEREWOLF		28
#define RACE_OLDFAEBORN			29
#define RACE_OLDANGELBORN		30
#define RACE_OLDDEMONBORN		31
#define RACE_OLDDEMIGOD			32
#define RACE_ANCIENTVAMPIRE		33
#define RACE_ANCIENTWEREWOLF	        34
#define RACE_DEMIDEMON			35
#define RACE_FREEANGEL			36
#define RACE_GODCHILD			37
#define RACE_FAECHILD			38

#define RACE_DEMON                      39

#define RACE_FACULTY			40

#define RACE_HUMAN  RACE_CIVILIAN

#define RACE_ANIMAL			41

#define RACE_WIGHT			42
#define RACE_DABBLER			43
#define RACE_REPTILE			44
#define RACE_INSECT			45
#define RACE_VEGETATION			46

#define RACE_LANDMONSTER		47
#define RACE_SEAMONSTER			48

#define RACE_NORMAL			49
#define RACE_DREAMCHILD			50
#define RACE_FANTASY			51
#define RACE_SPIRIT_FAE			52
#define RACE_SPIRIT_DIVINE		53
#define RACE_SPIRIT_DEMON		54
#define RACE_ELSEBORN			55
#define RACE_SPIRIT_GHOST		56
#define RACE_SPIRIT_CTHULIAN		57
#define RACE_SPIRIT_PRIMAL		58
#define MAX_RACES		        59

//Cliques
#define CLIQUE_NONE			0
#define CLIQUE_REBEL			1
#define CLIQUE_NERD			2
#define CLIQUE_POPULAR			3
#define MAX_CLIQUE			4

#define CLIQUEROLE_NONE			0
#define CLIQUEROLE_DRUGGIE  	        1
#define CLIQUEROLE_BULLY         	2
#define CLIQUEROLE_HACKER               3
#define CLIQUEROLE_OVERACHIEVER         4
#define CLIQUEROLE_ARTIST		5
#define CLIQUEROLE_GOTH			6
#define CLIQUEROLE_PREP			7
#define CLIQUEROLE_JOCK			8
#define MAX_CLIQUEROLE			9


#define TAR_EXTREMLY_LOW		50
#define TAR_VERY_LOW			75
#define TAR_LOW				90
#define TAR_SAME			100
#define TAR_SLIGHTLY_HIGHER		110
#define TAR_HIGHER			125
#define TAR_MUCH_HIGHER			150
#define TAR_EXTREMLY_HIGHER		200



#define BP_TANK			   	0
#define BP_RIGHT_FLANK			1
#define BP_LEFT_FLANK			2
#define BP_RIGHT_BOWMAN			3
#define BP_LEFT_BOWMAN			4
#define BP_REAR_GUARD			5

/* End of Battle Position Definitions */

#define PULSE_PER_SECOND	    4
#define PULSE_SECOND		  ( PULSE_PER_SECOND )

#define PULSE_WEATHER		  (   180 * PULSE_PER_SECOND)

#define PULSE_MUSIC		  (   7 * PULSE_PER_SECOND)
#define PULSE_TICK		  (  5 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE		  ( 12 * PULSE_PER_SECOND)
#define PULSE_AREA		  ( 300 * PULSE_PER_SECOND)
#define PULSE_AUCTION		  (  40 * PULSE_PER_SECOND)
#define PULSE_AUTOSAVE		  ( 600 * PULSE_PER_SECOND)
#define PULSE_HINT                (  30 * PULSE_PER_SECOND)
#define PULSE_CHEST               ( 300 * PULSE_PER_SECOND)
#define PULSE_MUD_HOUR		  ( 60 * PULSE_PER_SECOND)
#define PULSE_PER_HOUR		  ( 3600 * PULSE_PER_SECOND )
#define PULSE_FIGHT_CHECK	  ( 6*PULSE_PER_SECOND +2)
#define PULSE_OBJ		  (61*PULSE_PER_SECOND + 1);
#define COM_INGORE      1

/*
 * Color stuff by Lope
 */
#define CLS		"[2J"	        /* Clear Screen         */
#define HOME		"[1;1H"	/* Home                 */
#define CLEAR		"[40;0;37m"	/* Resets Colors        */
#define UNDERLINE   	"[4m"         /* Underline            */
#define BLINK   	"[5m"         /* Blink Colors         */
#define REVERSE  	"[7m"         /* Reverse Colors       */
#define C_BLACK		"[0;30m"	/* Normal Colors        */
#define C_RED		"[0;31m"
#define C_GREEN		"[0;32m"
#define C_YELLOW	"[0;33m"
#define C_BLUE		"[0;34m"
#define C_MAGENTA	"[0;35m"
#define C_CYAN		"[0;36m"
#define C_WHITE		"[0;37m"
#define C_D_GREY	"[1;30m"  	/* Bold Colors		*/
#define C_B_RED		"[1;31m"
#define C_B_GREEN	"[1;32m"
#define C_B_YELLOW	"[1;33m"
#define C_B_BLUE	"[1;34m"
#define C_B_MAGENTA	"[1;35m"
#define C_B_CYAN	"[1;36m"
#define C_B_WHITE	"[1;37m"
#define B_BLACK   	"[40m"      /* Background Colors    */
#define B_RED   	"[41m"
#define B_GREEN   	"[42m"
#define B_YELLOW   	"[43m"
#define B_BLUE  	"[44m"
#define B_MAGENTA   	"[45m"
#define B_CYAN   	"[46m"
#define B_WHITE   	"[47m"

#define COLOR_NONE	7

#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define WHITE		7
#define BLACK		0
#define NORMAL		0
#define BRIGHT		1

#define COLOR_COLOR	1
#define COLOR_NOCOLOR	2
#define COLOR_LENGTH	3

#define ALTER_COLOR( type )	if( !str_prefix( argument, "red" ) )		\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = RED;			\
				}						\
				else if( !str_prefix( argument, "hi-red" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = RED;			\
				}						\
				else if( !str_prefix( argument, "green" ) )	\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = GREEN;		\
				}						\
				else if( !str_prefix( argument, "hi-green" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = GREEN;		\
				}						\
				else if( !str_prefix( argument, "yellow" ) )	\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = YELLOW;		\
				}						\
				else if( !str_prefix( argument, "hi-yellow" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = YELLOW;		\
				}						\
				else if( !str_prefix( argument, "blue" ) )	\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = BLUE;		\
				}						\
				else if( !str_prefix( argument, "hi-blue" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = BLUE;		\
				}						\
				else if( !str_prefix( argument, "magenta" ) )	\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = MAGENTA;		\
				}						\
				else if( !str_prefix( argument, "hi-magenta" ) ) \
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = MAGENTA;		\
				}						\
				else if( !str_prefix( argument, "cyan" ) )	\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = CYAN;			\
				}						\
				else if( !str_prefix( argument, "hi-cyan" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = CYAN;			\
				}						\
				else if( !str_prefix( argument, "grey" ) )	\
				{						\
				    ch->pcdata->type[0] = NORMAL;		\
				    ch->pcdata->type[1] = WHITE;		\
				}						\
				else if( !str_prefix( argument, "white" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = WHITE;		\
				}						\
				else if( !str_prefix( argument, "black" ) )	\
				{						\
				    ch->pcdata->type[0] = BRIGHT;		\
				    ch->pcdata->type[1] = BLACK;		\
				}						\
				else if( !str_prefix( argument, "beep" ) )	\
				{						\
				    ch->pcdata->type[2] = 1;			\
				}						\
				else if( !str_prefix( argument, "nobeep" ) )	\
				{						\
				    ch->pcdata->type[2] = 0;			\
				}						\
				else						\
				{						\
		send_to_char( "Unrecognized color, unchanged.\n\r", ch );	\
				    return;					\
				}

#define LOAD_COLOR( field )	ch->pcdata->field[1] = fread_number( fp );	\
				if( ch->pcdata->field[1] > 100 )		\
				{						\
				    ch->pcdata->field[1] -= 100;		\
				    ch->pcdata->field[2] = 1;			\
				}						\
				else						\
				{						\
				    ch->pcdata->field[2] = 0;			\
				}						\
				if( ch->pcdata->field[1] > 10 )			\
				{						\
				    ch->pcdata->field[1] -= 10;			\
				    ch->pcdata->field[0] = 1;			\
				}						\
				else						\
				{						\
				    ch->pcdata->field[0] = 0;			\
				}

// ------------------------------------------------------------------------- //
// Stat Data                                                                 //
//                                                                           //
// - All the stat related defines should be here.                            //
//                                                                           //
// ------------------------------------------------------------------------- //


#define MAX_COMMAND_HASH 126

#define BAN_SUFFIX		A
#define BAN_PREFIX		B
#define BAN_NEWBIES		C
#define BAN_ALL			D
#define BAN_PERMIT		E
#define BAN_PERMANENT		F

#define MAX_DOOR		10
#define MAX_BIG_ITEMS	150
#define MAX_SMALL_ITEMS	100

#define TRAVEL_CLIMB 0
#define TRAVEL_FALL 1
#define TRAVEL_JUMP 2
#define TRAVEL_TAXI 3
#define TRAVEL_CAR  4
#define TRAVEL_FLY  5
#define TRAVEL_SPEED 6
#define TRAVEL_LARCENY 7
#define TRAVEL_FORCE 8
#define TRAVEL_BIKE 9
#define TRAVEL_PLANE	10
#define TRAVEL_BUS	11
#define TRAVEL_CAB	12
#define TRAVEL_CPASSENGER	13
#define TRAVEL_BPASSENGER	14
#define TRAVEL_HORSE		15
#define TRAVEL_HPASSENGER	16
#define TRAVEL_PATHING		17


// Command logging types.
#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

// Command Flags
#define CMD_FLAG_NO_SHOW	(A)
#define CMD_FLAG_NO_MOB		(B)
#define CMD_FLAG_NO_PC		(C)



#define DIST_W		0
#define DIST_E		1
#define DIST_N2NW	2
#define DIST_2NNW	3
#define DIST_NNW	4
#define DIST_N2NE	5
#define DIST_2NNE	6
#define DIST_NNE	7

#define DIST_3NW	8
#define DIST_2NW	9
#define DIST_NW		10

#define DIST_3NE	11
#define DIST_2NE	12
#define DIST_NE		13
#define DIST_4N		14
#define DIST_3N		15
#define DIST_2N		16
#define DIST_N		17

#define DIST_3NU	18
#define DIST_3ND	19
#define DIST_NU		20
#define DIST_ND		21
#define DIST_2NU	22
#define DIST_2ND	23


/*
 * Time and weather stuff.
 */
#define MOON_FULL		0
#define MOON_WANE		1
#define MOON_HALF		2
#define MOON_WAX		3
#define MOON_NEW		4

#define SUN_DARK		0
#define SUN_RISE		1
#define SUN_LIGHT		2
#define SUN_SET			3

#define SKY_CLEAR		0
#define SKY_CLOUDY		1
#define SKY_FOG			5
#define SKY_RAINING		2
#define SKY_LIGHTNING		6
#define SKY_THUNDER		7
#define SKY_HAILING		4
#define SKY_SNOWING		3

#define WALLCOND_NORMAL 0
#define WALLCOND_DAMAGED 1
#define WALLCOND_HOLE 2

#define WALL_NONE 0
#define WALL_GLASS 1
#define WALL_PLASTER 2
#define WALL_WOOD 3
#define WALL_BRICK 4
#define WALL_STEEL 5


/*
 * Connected state for a channel.
 */
#define CON_PLAYING			                0
#define CON_GET_NAME			            1
#define CON_GET_OLD_PASSWORD		        2
#define CON_CONFIRM_NEW_NAME		        3
#define CON_GET_NEW_PASSWORD		        4
#define CON_CONFIRM_NEW_PASSWORD	        5
#define CON_READ_IMOTD			            6
#define CON_READ_MOTD			            7
#define CON_BREAK_CONNECT		            8
#define CON_COPYOVER_RECOVER		        9
#define CON_ANSI_COLOR			            10
#define CON_DONE			                11
#define CON_REMORT_CLASS		            12
#define CON_READ_STORYLINE		            13
#define CON_IDENT_WAIT			            14
#define CON_QUITTING			            15
#define CON_CREATION			            16
#define CON_SELECT_NEW_SUBRACE		        17
#define CON_SELECT_NEW_LNAME		        18
#define CON_GET_ACCOUNT			            19
#define CON_CHOOSE_CHARACTER		        20
#define CON_CONFIRM_NEW_ACCOUNT_NAME	    21
#define CON_GET_ACCOUNT_NAME		        22
#define CON_GET_ACCOUNT_PASSWORD	        23
#define CON_CONFIRM_NEW_ACCOUNT_PASSWORD    24
#define CON_GET_NEW_NAME		            25
#define CON_GET_NEW_ACCOUNT_PASSWORD	    26
#define CON_CONFIRM_ACCOUNT_NAME		    27


/*
 * Creation States for Creation
 */
#define CR_ALL                          0
#define CR_EMAIL                        1
#define CR_LASTNAME                     2
#define CR_SEX                          3
#define CR_CLASS                        4
#define CR_HUMAN			5
#define CR_MONSTER			6
#define CR_DEMONBORN			7
#define CR_DEMIGOD			8
#define CR_SHIFTER			9
#define CR_FAEBORN			10
#define CR_ANGELBORN			11
#define CR_YEAR                         12
#define CR_MONTH                        13
#define CR_DAY                          14
#define CR_SIREYEAR                     15
#define CR_SIREMONTH                    16
#define CR_SIREDAY                      17
#define CR_HEIGHTFEET                   18
#define CR_HEIGHTINCH                   19
#define CR_EYES				20
#define CR_HAIR				21
#define CR_SKIN				22
#define CR_BUST                         23
#define CR_INTRO                        24
#define CR_FACTIONS			25

//Ghosts - Discordance
#define GHOST_PRIVATE                   10
#define GHOST_EMOTE                     15
#define GHOST_MANIFESTATION             20
#define GHOST_SPEECH                    25
#define GHOST_WALKERMAX                 300
#define GHOST_SPIRITMAX                 600

//Sex - Discordance
#define EFFECTIVE_ESTROGEN				196
#define MAX_ESTROGEN					812
#define NORMAL_ESTROGEN_THRESHOLD		420
#define ESTROGEN_DOSAGE					56

#define SEXTYPE_NONE                  0
#define SEXTYPE_INTERCOURSE			      1
#define SEXTYPE_OUTERCOURSE				    2

#define SEXTYPE_NORMAL                0
#define SEXTYPE_FACULTY               1
#define SEXTYPE_PARASITE              2
#define SEXTYPE_ACCELERATED           3
#define SEXTYPE_SHORT                 4

#define SEXPROTECTION_NONE            0
#define SEXPROTECTION_ACCIDENT        1
#define SEXPROTECTION_PRETEND         2
#define SEXPROTECTION_PULLOUT         3
#define SEXPROTECTION_SLIPOFF         4
#define SEXPROTECTION_CONDOM          5

#define CR_CUSTOMISE                    26
#define CR_DISCIPLINES                  27
#define CR_SKILLS                       28
#define CR_ADULT                        29
#define CR_SPECIAL			30

/*
 * TO types for act.
 */
#define TO_ROOM		    		0
#define TO_NOTVICT	    		1
#define TO_VICT		    		2
#define TO_CHAR		    		3
#define TO_ALL		    		4
#define TO_FIGHT_BRIEF	    		5
#define TO_FIGHT_BRIEF_NOTVICT 		6
#define TO_FIGHT_NONBRIEF		7
#define TO_FIGHT_NONBRIEF_NOTVICT	8
#define TO_NOTVICT_NOTIMP		9
#define TO_IMP				10

#define MAX_TRADE	 5

#define MAX_RESTRING 3	/* Max possible restringable objects per mobile.*/

#define MAX_INAME	39

#define MAX_LEVEL_ROOMS	50
#define BE_CHANCE	-2

/*
 * Values for Metals, and Armors that can be forged.
 *
 * Palin - (elaurin@umich.edu)
 */
#define MAX_METAL	8
#define MAX_ARMOR	16


//edhere
/*
 * Primary Stats
 */
#define MAX_STATS 	14
#define STAT_STR 	0
#define STAT_AGI	1
#define STAT_END	2
#define STAT_WILL	3
#define STAT_INF	4
#define STAT_FAV	5
#define STAT_PER	6
#define STAT_ACC	7
#define STAT_MISC6	8
#define STAT_MISC7	9
#define STAT_MISC8	10
#define STAT_MISC9	11
#define STAT_MISC10	12
#define STAT_MISC11	13

#define THING_TYPELESS	0
#define THING_CH	1
#define THING_OBJ	2
#define THING_ROOM	3

/* where definitions */
#define TO_AFFECTS	0
#define TO_OBJECT	1
#define TO_IMMUNE	2
#define TO_RESIST	3
#define TO_VULN		4
#define TO_WEAPON	5
#define	TO_BITS		6

/* Definitions for bit arrays
 * Two notes of interest:
 *   1.  UL_BITS MUST = Size of unsigned long on the system
 *   2.  MAXELEMS can be any multiple of UL_BITS
 *  - Scaelorn
 */
#define UL_BITS			 32
#define MAXELEMS		 128
#define SETSIZE			 MAXELEMS / UL_BITS

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/* RT ASCII conversions -- used so we can have letters in this file */

#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		    1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864 	/* doubled due to conflicts */
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee			1073741824

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		    0		/* Auto set for mobs	*/
#define ACT_SENTINEL	    1		/* Stays in one room	*/
#define ACT_SCAVENGER		2		/* Picks up objects	*/
#define ACT_VALUABLE		3
#define ACT_MOUNT		    4
#define ACT_AGGRESSIVE		5    		/* Attacks PC's		*/
#define ACT_STAY_AREA		6		/* Won't leave area	*/
#define ACT_WIMPY		    7
#define ACT_PET			    8		/* Auto set for pets	*/
#define ACT_TAILOR		    9		/* Can train PC's	*/
#define ACT_JEWELER		    10		/* Can practice PC's	*/
#define ACT_STONEWALL		11		/* Protecting Exit	*/
#define ACT_FASTMOVE		12
#define ACT_WEAPONSMITH		13
#define ACT_ARMORSMITH		14
#define ACT_DEFENSELESS		16
#define ACT_MONSTER		    17
#define ACT_COVER		    18
#define ACT_TURRET		    19
#define ACT_COMBATOBJ		20
#define ACT_NOPURGE		    21
#define ACT_OUTDOORS		22
#define ACT_INDOORS		    24
#define ACT_SOULLESS		25

#define ACT_BIGGAME		    26
#define ACT_BRIBEMOB		27
#define ACT_UPDATE_ALWAYS	28
#define ACT_IS_CHANGER		29
#define ACT_NOEXP		    30
#define ACT_TRAIN_NOVICE	31
#define ACT_TRAIN_INTERMED	32
#define ACT_TRAIN_EXPERT	33
#define ACT_TRAIN_MASTER	34
#define ACT_LIGHT_SLEEPER	35
#define ACT_TRAIN_ALL		36
#define ACT_GUARD		    37
#define ACT_ARCHER		    38
#define ACT_STAY_SUBAREA    40
#define ACT_NONE		    41


/*
 * The different bits for the different 
 * types of special procedures for mobs/obj
 */
#define SPEC_MOB        	(A)
#define SPEC_OBJ        	(B)

/*
 * Action bits for the different types
 * of action to take when a mob
 * encounters a char it "remembers" 
 */
#define MEM_ACT_WIMP		(A)
#define MEM_ACT_AGGR		(B)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT		        15
#define DAM_OTHER               16
#define DAM_HARM		        17
#define DAM_CHARM		        18
#define DAM_SOUND		        19

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH		        (O)
#define ASSIST_ALL       	    (P)
#define ASSIST_ALIGN	        (Q)
#define ASSIST_RACE    	     	(R)
#define ASSIST_PLAYERS      	(S)
#define ASSIST_GUARD        	(T)
#define ASSIST_VNUM		        (U)
#define OFF_HUNT		        (V)

/* return values for check_imm */
#define IS_NORMAL		    0
#define IS_IMMUNE		    1
#define IS_RESISTANT		2
#define IS_VULNERABLE		3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT		        (S)
#define IMM_SOUND		        (T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)

/* RES bits for mobs */
#define RES_SUMMON		        (A)
#define RES_CHARM		        (B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		        (S)
#define RES_SOUND		        (T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)

/* VULN bits for mobs */
#define VULN_SUMMON		        (A)
#define VULN_CHARM		        (B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		        (S)
#define VULN_SOUND		        (T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		        (Z)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)
#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB		        (S)
#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD		    (cc)

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE		        (K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS		        (Y)

/* These are the difficulty levels for abilities */
#define DIFF_NOVICE		        0
#define DIFF_INTERMED		    1
#define DIFF_EXPERT		        2
#define DIFF_MASTER		        3

/**
 * These are used primary in creation.  Do not toy with them.
 * Palin
 */

#define ST_WEAPON		        0
#define ST_CHANNELER		    1
#define ST_DEFENSIVE		    2
#define ST_INTELLIGENCE		    3
#define ST_OFFENSIVE		    4
#define ST_ROGUE		        5
#define ST_SURVIVAL		        6
#define ST_ADVANCED		        7
#define ST_MARTIAL		        8
#define ST_TRADES		        9
#define ST_NOTHING		        10
/* The following are spell classes.. however they just draw upon the above counting. */
#define ST_AIR			        11
#define ST_EARTH		        12
#define ST_FIRE			        13
#define ST_SPIRIT		        14
#define ST_WATER		        15
#define ST_MIX_BASIC		    16
#define ST_MIX_ADV		        17
#define ST_REMORT_SKILL         18
#define ST_EXPERT_SPEC_SKILL	19
#define ST_FORM                 20
#define ST_TRUEPOWER            21
#define ST_EXT_LEVEL            22
#define ST_DIFFICULT_WEAVE	    23
#define ST_RARE_WEAVE		    24
#define ST_GUILD_SKILL          25

/* Race dependant */
#define ST_AIEL_SKILL		    26
#define ST_FADE_SKILL		    27
#define ST_ROLEPLAY_SKILL	    28


#define ST_CHAN			ST_CHANNELER
#define ST_DEF			ST_DEFENSIVE
#define ST_INT			ST_INTELLIGENCE
#define ST_OFF			ST_OFFENSIVE
#define ST_SURV			ST_SURVIVAL
#define ST_ADV			ST_ADVANCED

/* Some basic skill stuff.. for chaining a couple skills */

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND				0
#define AFF_PROTECT				1
#define AFF_FERTILITY			2
#define AFF_DEAF                3
#define AFF_SUNBLESS            4
#define AFF_MOONBLESS           5
#define AFF_WARBLESS 	        6
#define AFF_LUCKY				7
#define AFF_UNLUCKY				8
#define AFF_SUNG				9
#define AFF_VBLOOD				10
#define AFF_BERSERK				11
#define AFF_DESCLOCK			12
#define AFF_PENALTY				13
#define AFF_BONUS				14
#define AFF_PATHED				15
#define AFF_MARKED				16
#define AFF_FLESHFORMING	    17
#define AFF_WET                 18
#define AFF_REROLLED            19
#define AFF_MARKED2             20
#define AFF_UNBANISHED			21
#define AFF_DSENSITIVE			22
#define AFF_WARD				23
#define AFF_WEAKEN				24
#define AFF_COMMUTE             25
#define AFF_DRAINED             26
#define AFF_SILENCED            27
#define AFF_GRIEF               28
#define AFF_SLOW				29
#define AFF_BITTEN				30
#define AFF_MARKED3				31
#define AFF_MINDWARD 			32
#define AFF_VITALITY			33
#define AFF_KILLABLE_2			34
#define AFF_DOORSTUCK				35
#define AFF_DISCREDIT 			36
#define AFF_OUTCAST				37	// defends a person against attack
#define AFF_PIN					38	// knocks a person down for a short period
#define AFF_UNDERSTANDING		39	// Compulsion like, freezing mind
#define AFF_NOUNDERSTANDING		40	// Shield of Air - Around Room
#define AFF_WEAKENED			41	// disguise spell - mask name
#define AFF_BADKIDNAPPEE	    42	// Indicates the person is in a link
#define AFF_GAG					43	// Char affected by "gag" spell
#define AFF_SHIELD				44	// Person affected by "shield" spell **
#define AFF_EXPERIMENTED		45
#define AFF_PRISON				46
#define AFF_WATERBLESS			47
#define AFF_PHYS_BOOST			48
#define AFF_NOTICED			    49
#define AFF_ABDUCTED			50
#define AFF_ANIMALMINDED		51
#define AFF_NOMENTOR			52     // dart skill
#define AFF_FULLSHIELD			53
#define AFF_HOSPITAL			54
#define AFF_BURSTVESSEL			55
#define AFF_MAD				    56	// charge skill
#define AFF_LURED			    57
#define AFF_PERSECUTED			58
#define AFF_NEUTRALIZED			59
#define AFF_TRIGGERED			60
#define AFF_NOTECH			    61
#define AFF_ILLNESS			    62
#define AFF_ARTHRITIS			63
#define AFF_SEENSUPER		    64
#define AFF_MALADY				65
#define AFF_LAZARUS				66
#define AFF_FROST				67
#define AFF_HELMET				68
#define AFF_STONE_GRIP			69
#define AFF_SOLID_GRIP			70
#define AFF_ENTANGLE			71
#define AFF_FLASH				72
#define AFF_PEACE				73
#define AFF_RUSH				74
#define AFF_SLUSH				75
#define AFF_BUFFER				76
#define AFF_TWISTED				77
#define AFF_MIRROR_OF_MIST		78
#define AFF_DUPLICATE			79
#define AFF_PHASE				80
#define AFF_RINGING				81
#define AFF_HALT				82
#define AFF_CONFUSION			83
#define AFF_FEATHER				84
#define AFF_FADED				85
#define AFF_BOIL				86
#define AFF_NULLIFY				87
#define AFF_GLOW				88
#define AFF_REGEN				89
#define AFF_STEADFAST			90
#define AFF_BLINK				91
#define AFF_REFRESH				92
#define AFF_STRENGTH_SAPPER		93
#define AFF_GREEK_FIRE			94
#define AFF_CRIPPLING_ROOTS		95
#define AFF_BLINDING_STARS		96
#define AFF_PARALYZED   		97
#define AFF_SLEEPER_STAB		98
#define AFF_BLACK_WIDOW			99
#define AFF_INVISIBLE		    100	
#define AFF_STAKED				101	// when vampires are staked
#define AFF_SONG_MARCH			102
#define AFF_SONG				103
#define AFF_WHISTLE_WIND		104
#define AFF_FEIGN_DEATH			105
#define AFF_GBH					106
#define AFF_REAR_GUARD_PLACEMENT 107
#define AFF_BLOCK				108
#define AFF_DEPLOY				109
#define AFF_SHIELD_ENHANCEMENT	110
#define AFF_COMPOSURE			111
#define AFF_FILLME3             112
#define AFF_HEAL                113
#define AFF_DISORIENTATION      114
#define AFF_NOHEAL              115
#define AFF_MUTE                116
#define AFF_SUFFER				117
#define AFF_SUPERTAINT			118   
#define AFF_SHOOTINGBLANKS      119     // curbs male fertility
#define AFF_SEEKINGSEX          120     // places time limit on sex transaction
#define AFF_TIMEDDROP           121     // drops all items after timer runs out
#define AFF_STUCKSHIFTER        122     // keeps shifter from shifting
#define AFF_CONTRACTIONS        123     // labor pains
    
/*
 *  THIS MUST BE LAST.  If you add an affect, increment this
 *  define accordingly.
 *	- Scaelorn
 */
#define AFF_NONE		124

/*
 * Special affects for exits for the different
 * exit-affecting weaves
 *	- Scaelorn
 */
#define AFF_XAIRWALL 		1
#define AFF_XFIRETRAP		2
#define AFF_XICETRAP		3
#define AFF_XSPIKETRAP		4
#define AFF_XSTONEWALL		5
#define AFF_XFIREWALL		6
#define AFF_XBUBBLE		7
#define AFF_XANIMAL_TRAP	8
#define AFF_XPITFALL		9
#define AFF_XLOOSE_STONE	10
#define AFF_XROPE_TRAP		11
#define AFF_XHANGMAN		12
#define AFF_XEAVESDROP		13
#define AFF_XICE_PATCH		14
#define AFF_XGRAVEL		15
#define AFF_XHIDE		16
#define AFF_XDARKEN		17
#define AFF_XSLOW		18
/*
 *  THIS MUST BE LAST.  If you add an affect, increment this
 *  define accordingly.
 *      - Scaelorn
 */
#define AFF_XNONE		19

#define EXIT_SPEC_NONE 0

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		0
#define SEX_MALE		1
#define SEX_FEMALE		2

/* AC types */
#define AC_PIERCE		0
#define AC_BASH			1
#define AC_SLASH		2
#define AC_EXOTIC		3

/* dice */
#define DICE_NUMBER		0
#define DICE_TYPE		1
#define DICE_BONUS		2

/* size */
#define SIZE_TINY		0
#define SIZE_SMALL		1
#define SIZE_MEDIUM		2
#define SIZE_LARGE		3
#define SIZE_HUGE		4
#define SIZE_GIANT		5



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
/* Here are the Coin Definitions */

#define OBJ_VNUM_MONEY		1
#define OBJ_VNUM_MONEY_S	2


#define OBJ_VNUM_STICK		20
#define OBJ_VNUM_BRANCH		21
#define OBJ_VNUM_CHAIRLEG	22
#define OBJ_VNUM_STANDLEG	23
#define OBJ_VNUM_POOLCUE	24



#define OBJ_VNUM_PHONE 55
#define OBJ_VNUM_SHOES 54
#define OBJ_VNUM_SOCKS 53
#define OBJ_VNUM_SHIRT 52
#define OBJ_VNUM_JEANS 50
#define OBJ_VNUM_SHORTS 51

#define OBJ_VNUM_CORPSE_NPC	     3
#define OBJ_VNUM_CORPSE_PC	     4
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_GUTS		     16
#define OBJ_VNUM_BRAINS		     17

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_DISC		     23
#define OBJ_VNUM_PORTAL		     25
#define OBJ_VNUM_PUDDLE		     26


#define OBJ_VNUM_PIT		      7

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT                   1
#define ITEM_WEAPON                  2
#define ITEM_PERFUME                 3
#define ITEM_ARMOR                   4
#define ITEM_POTION                  5
#define ITEM_CLOTHING                6
#define ITEM_FURNITURE               7
#define ITEM_TRASH                   8
#define ITEM_CONTAINER               9
#define ITEM_DRINK_CON              10
#define ITEM_KEY                    11
#define ITEM_FOOD                   12
#define ITEM_MONEY                  13
#define ITEM_CORPSE_NPC             14
#define ITEM_CORPSE_PC              15
#define ITEM_FOUNTAIN               16
#define ITEM_UMBRELLA               17
#define ITEM_PORTAL                 18
#define ITEM_ROOM_KEY               19 
#define ITEM_RANGED                 20 
#define ITEM_JEWELRY                21 
#define ITEM_JUKEBOX                22 
#define ITEM_FLASHLIGHT             23
#define ITEM_KEYRING                24
#define ITEM_GAME                   25
#define ITEM_COMPONENT		        26
#define ITEM_STAKE                  27
#define ITEM_LIGHTER                28
#define ITEM_BLOODCRYSTAL           29
#define ITEM_ARTIFACT               30
#define ITEM_PHONE		            31
#define ITEM_DRUGS		            32
#define ITEM_BABY		            33


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_AMULET		    (A)
#define ITEM_OFF		    (B)
#define ITEM_VBLOOD		    (C)
#define ITEM_LOCK		    (D)
#define ITEM_ARTIFACTWEAPON	(E)
#define ITEM_INVIS		    (F)
#define ITEM_CHARM		    (G)
#define ITEM_NODROP		    (H)
#define ITEM_WARDROBE		(I)
#define ITEM_SCUBA		    (J)
#define ITEM_COLLAR		    (K)
#define ITEM_FOCUS		    (L)
#define ITEM_SILENT		    (M)
#define ITEM_INVENTORY		(N)
#define ITEM_RELIC		    (O)
#define ITEM_ROHYPNOL		(P)
#define ITEM_POISON		    (Q)
#define	ITEM_NOINVENTORY	(R)  // for spell_token
#define ITEM_NOOUTFIT		(S)
#define ITEM_NODECAY		(T)
#define ITEM_NORESALE		(U)
#define ITEM_USABLE		    (V)
#define ITEM_SHROUD		    (W)
#define ITEM_ALTERED		(X)
#define ITEM_LARGE		    (Y)
#define ITEM_CURSED		    (Z)
#define	ITEM_HIDDEN		    (aa)
#define ITEM_AMMOCHANGED	(bb)
#define ITEM_ROT_TIMER		(cc)
#define ITEM_WATERPROOF		(dd)
#define ITEM_ARMORED		(ee)

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE               (A)
#define ITEM_WEAR_BODY          (B)
#define ITEM_WEAR_HEAD          (C)
#define ITEM_WEAR_LEGS          (D)
#define ITEM_WEAR_FEET          (E)
#define ITEM_WEAR_HANDS         (F)
#define ITEM_WEAR_ACCES		    (G)
#define ITEM_WEAR_NECK		    (H)
#define ITEM_WEAR_HOLD		    (I)
#define ITEM_HOLD               (J)
#define ITEM_NO_SAC             (K)
#define ITEM_WEAR_EYES          (L)
#define ITEM_WEAR_HOLE		    (M)

/* weapon class */
#define MAX_WEAPON		9
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE		4
#define WEAPON_AXE		5
#define WEAPON_FLAIL		6
#define WEAPON_FIST		7
#define WEAPON_STAFF		8
#define WEAPON_POLEARM		9

#define ATTACK_BASH		0
#define ATTACK_CUT		1
#define ATTACK_STAB		2


#define WTYPE_SLASH		0
#define WTYPE_BLUNT		1
#define WTYPE_EXOTIC		2

/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)

/* gate flags */
#define GATE_NORMAL_EXIT	(A)
#define GATE_NOCURSE		(B)
#define GATE_GOWITH		(C)
#define GATE_BUGGY		(D)
#define GATE_RANDOM		(E)

/* furniture flags */
#define STAND_AT		(A)
#define STAND_ON		(B)
#define STAND_IN		(C)
#define SIT_AT			(D)
#define SIT_ON			(E)
#define SIT_IN			(F)
#define REST_AT			(G)
#define REST_ON			(H)
#define REST_IN			(I)
#define SLEEP_AT		(J)
#define SLEEP_ON		(K)
#define SLEEP_IN		(L)
#define PUT_AT			(M)
#define PUT_ON			(N)
#define PUT_IN			(O)
#define PUT_INSIDE		(P)




/*
 * WApply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_SPE        		2
#define APPLY_MART       		3
#define APPLY_MGEN       		4
#define APPLY_MEND       		5
#define APPLY_MIND       		6
#define APPLY_DOFF      		 7
#define APPLY_DBUFF      		8
#define APPLY_DMYS       		9
#define APPLY_REA        		10
#define APPLY_MALI       		11
#define APPLY_WASS       		12
#define APPLY_WSK        		13
#define APPLY_VIT        		14
#define APPLY_SEX		      15
#define APPLY_CLASS		      16
#define APPLY_LEVEL		      17
#define APPLY_AGE		      18
#define APPLY_HEIGHT		     19
#define APPLY_WEIGHT		     20
#define APPLY_MANA		     21
#define APPLY_HIT		     22
#define APPLY_MOVE		     23
#define APPLY_GOLD		     24
#define APPLY_EXP		     25
#define APPLY_AC		     26
#define APPLY_HITROLL		     27
#define APPLY_DAMROLL		     28
#define APPLY_SAVES		     29
#define APPLY_SAVING_PARA	     29
#define APPLY_SAVING_ROD	     30
#define APPLY_SAVING_PETRI	     31
#define APPLY_SAVING_BREATH	     32
#define APPLY_SAVING_SPELL	     33
#define APPLY_SPELL_AFFECT	     34
#define APPLY_SECONDARY_CONCEALMENT  35
#define APPLY_SECONDARY_AGILITY	     36
#define APPLY_SECONDARY_AWARENESS    37
#define APPLY_SECONDARY_COMFORT      38
#define APPLY_SECONDARY_ENCUMBRANCE  39
#define APPLY_SECONDARY_BALANCE      40
#define APPLY_SECONDARY_SPEED	     41
#define APPLY_SECONDARY_ADRENALINE   42
#define APPLY_SECONDARY_FEAR         43
#define APPLY_SECONDARY_ONE_POWER    44
#define APPLY_SECONDARY_WILLPOWER    45
#define APPLY_SECONDARY_REFLEXES     46
#define APPLY_SECONDARY_INITIATIVE   47
#define APPLY_SECONDARY_HEALTH       48
#define APPLY_SECONDARY_LUCK         49
#define APPLY_TORSO_AC		     50
#define APPLY_HEAD_AC		     51
#define APPLY_POISON		     52
#define APPLY_POISON_DAMAGE	     53
/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8
#define CONT_PUT_ON		     16

/*
 * Values for Body Locations, where a person hits another in combat.
 */

#define BODY_NONE	0
#define BODY_ARMS	1
#define BODY_HANDS	2
#define BODY_HEAD	3
#define BODY_LEGS	4
#define BODY_NECK	5
#define BODY_TORSO	6
#define BODY_ALL	7
#define MAX_BODY	8

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		        1
#define ROOM_VNUM_FADE			1
#define ROOM_VNUM_CREATION		3
#define ROOM_VNUM_QUEST                 1
#define ROOM_VNUM_SCHOOL		50

#define ROOM_POSTOFFICE			1
#define ROOM_BLOODBANK			405510472

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK			(A)
#define ROOM_KITCHEN        (B)
#define ROOM_NO_MOB			(C)
#define ROOM_INDOORS		(D)
#define ROOM_PERM			(E)
#define ROOM_PUBLIC			(F)
#define ROOM_LOCKED			(G)
#define ROOM_PRIVATE		(J)
#define ROOM_SAFE			(K)
#define ROOM_SILENCED		(L)
#define ROOM_INVISIBLE		(M)
#define ROOM_BEDROOM	    (N)
#define ROOM_SPARRING		(O)
#define ROOM_CAMPSITE		(P)
#define ROOM_ANIMALHOME		(Q)
#define ROOM_NOSHROUD		(R)
#define ROOM_SHOWER			(S)
#define ROOM_LIGHTOFF		(T)
#define ROOM_BANK			(U)
#define ROOM_LIGHTON	    (V)
#define ROOM_UNLIT			(W)
#define ROOM_DIRTROAD		(X)
#define ROOM_BATHROOM		(Y)
#define ROOM_STASH			(Z)
#define ROOM_WHEREVIS		(aa)
#define ROOM_COMMERCIAL		(bb)
#define ROOM_RESIDENTIAL	(cc)
#define ROOM_LANDLINE	    (dd)
#define ROOM_SIGNALBOOST	(ee)

/*
 * Climates
 * Used in tables.
 */ 
#define CLIMATE_BLIGHT 			0 
#define CLIMATE_DESERT 			1 
#define CLIMATE_SOUTHMAIN 		2 
#define CLIMATE_SWMAIN 			3 
#define CLIMATE_BORDERLANDS		4 
#define CLIMATE_MIDMAIN 		5 
#define CLIMATE_MOUNTAINS 		6 
#define CLIMATE_OCEAN 			7 
#define CLIMATE_NONE			8
#define MAX_CLIMATE				9

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH                     0
#define DIR_EAST                      1
#define DIR_SOUTH                     2
#define DIR_WEST                      3
#define DIR_UP                        4
#define DIR_DOWN                      5
#define DIR_NORTHEAST                 6
#define DIR_NORTHWEST                 7
#define DIR_SOUTHEAST                 8
#define DIR_SOUTHWEST                 9


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR				(A)
#define EX_CLOSED				(B)
#define EX_LOCKED				(C)
#define EX_CURTAINS			    (F)
#define EX_NOPASS				(G)
#define EX_SEETHRU				(H)
#define EX_HARD					(I)
#define EX_HELLGATE			    (J)
#define EX_NOCLOSE				(K)
#define EX_NOLOCK				(L)
#define EX_HIDDEN				(M)

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_STREET						1

#define SECT_HOUSE						2
#define SECT_CLUB						3
#define SECT_RESTERAUNT					4
#define SECT_SHOP						5
#define SECT_TAVERN						6
#define SECT_ALLEY						7
#define SECT_WAREHOUSE					8
#define SECT_COMMERCIAL					9
#define SECT_PARK						10
#define SECT_TUNNELS					11
#define SECT_CAFE						12
#define SECT_ROOFTOP					13
#define SECT_BASEMENT					14
#define SECT_HOSPITAL					15
#define SECT_BANK						16
#define SECT_CAR						17
#define SECT_AIR						18
#define SECT_HELLGATE					19
#define SECT_WATER						20
#define SECT_UNDERWATER					21
#define SECT_ATMOSPHERE					22
#define SECT_FOREST						23
#define SECT_CAVE						24
#define SECT_SWAMP						25
#define SECT_BEACH						26
#define SECT_ROCKY						27
#define SECT_SHALLOW					28
#define SECT_SIDEWALK					29
#define SECT_PARKING					30
#define SECT_CEMETARY					31
#define SECT_DIRT						32
#define SECT_MAX						33


/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
typedef enum
{
        WEAR_NONE = -1,         WEAR_BODY_1 = 0,        WEAR_BODY_2,
        WEAR_BODY_3,            WEAR_BODY_4,            WEAR_BODY_5,
        WEAR_BODY_6,            WEAR_BODY_7,            WEAR_BODY_8,
        WEAR_BODY_9,            WEAR_BODY_10,           WEAR_BODY_11,
        WEAR_BODY_12,           WEAR_BODY_13,           WEAR_BODY_14,
        WEAR_BODY_15,           WEAR_BODY_16,           WEAR_BODY_17,
        WEAR_BODY_18,           WEAR_BODY_19,           WEAR_BODY_20,
        WEAR_BODY_21,           WEAR_BODY_22,           WEAR_BODY_23,
        WEAR_BODY_24,           WEAR_BODY_25,           WEAR_HOLD,
        WEAR_HOLD_2,            MAX_WEAR
} wear_locations;

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0

/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_SITTING		      6
#define POS_FIGHTING		      7
#define POS_STANDING		      8

/*
 * ACT bits for players.
 */
#define 	PLR_IS_NPC		0		/* Don't EVER set.	*/

/* special flags */
#define PLR_ANTISPEC		1		/* Shuts off special messages(fades, bonded) */

/* RT auto flags */
#define PLR_GM 			2
#define PLR_SHROUD		3
#define PLR_GHOSTWALKING	4
#define PLR_GHOSTBOUND          5
#define PLR_SHIFTSTASH          6
#define PLR_BOUND		7
#define PLR_NOPAY		8

#define PLR_ASCENDING		9
#define PLR_SHOWERING		10
#define PLR_HIDE		11
#define PLR_SHADOW		12
#define PLR_HOLYLIGHT		13
#define PLR_NOSAVE		14
#define PLR_BOUNDFEET		15
#define PLR_DEAD		16
#define PLR_NOFOLLOW		17
#define PLR_ROOMINVIS		18
#define PLR_COLOR              	19
#define PLR_PERMIT		20
#define PLR_WHOINVIS		21
#define PLR_LOG			22
#define PLR_DENY		23
#define PLR_FREEZE		24
#define PLR_REROLL		25
#define PLR_NOTICED		26
#define PLR_NONOTE      	27
#define PLR_DEEPSHROUD      	28
#define PLR_DEMONPACT		29
#define PLR_SPYSHIELD		30
#define PLR_STASIS		31
#define PLR_SINSPIRIT		32
#define PLR_VILLAIN		33
#define PLR_GUEST		34
#define PLR_NOMENTOR		35
#define PLR_INVIS		36
#define PLR_SPYING		37
#define PLR_TYRANT		38
#define PLR_MADERELIC		39
#define PLR_MONSTER		40
#define PLR_RUNNING		41
#define PLR_WOLFBIT		42
#define PLR_GHOST		43
#define PLR_MANIFESTING		44
#define PLR_SUPERJUMPING	45
#define PLR_FLYING      	46
#define PLR_VISIBLE      	47
#define PLR_AFTERGLOW      	48
#define PLR_INFLUENCER	     	49
#define PLR_NONE		50

/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET               0
#define COMM_DEAF                1
#define COMM_NOWIZ               2
#define COMM_NOOOC               3
#define COMM_SPARRING            4
#define COMM_VBLOOD              5
#define COMM_RUNNING             6
#define COMM_NONEWBIE            7
#define COMM_NOGUIDE             8
#define COMM_CONSENT             9
#define COMM_REPORTED            10

/* display flags */
#define COMM_COMPACT            11
#define COMM_BRIEF              12
#define COMM_PROMPT             13
#define COMM_COMBINE            14
#define COMM_TELNET_GA          15
#define COMM_BLINDFOLD          16

#define COMM_PACIFIST           17
#define COMM_NOLEGEND           18
#define COMM_SUBDUE             19
#define COMM_NOEMOTE            20
#define COMM_BANISHED           21
#define COMM_NOTELL             22
#define COMM_NOCHANNELS         23
#define COMM_DEPUTY             24
#define COMM_PROXIED			25
#define COMM_AFK                26
#define COMM_WHOINVIS           27
#define COMM_NOTITLE            28
#define COMM_NOLEADER           29
#define COMM_STORY              30
#define COMM_STALL              31
#define COMM_MENTOR             32
#define COMM_RACIAL				33
#define COMM_FEEDING			34
#define COMM_NOSTALK			35
#define COMM_AUTOFIRE			36
#define COMM_NOINTRO			37
#define COMM_HELPLESS			38
#define COMM_GUARDED			39
#define COMM_NOCOMMS			40
#define COMM_PRIVATE			41
#define COMM_NOFRIGID		    42
#define COMM_RESEARCHED		    43
#define COMM_BOARDING			44
#define COMM_HOSTILE			45
#define COMM_CARDINAL			46
#define COMM_CLOAKED			47
#define COMM_WHEREVIS			48
#define COMM_HOTSPOT            49
#define COMM_WHEREANON          50
#define COMM_PRAY               51
#define COMM_SLOW		        52
#define COMM_GAG		        53
#define COMM_FORCEDPACIFIST		54
#define COMM_CLEANSED		    55
#define COMM_EMPOWERED		    56
#define COMM_SHADOWCLOAK	    57
#define COMM_MANDRAKE		    58
#define COMM_DREAMSNARED	    59
#define COMM_NOMESSAGE          60


/*
 *  THIS MUST BE LAST.  If you add a channel, increment this
 *  define accordingly. Also add to comm_table in tables.c
 *      - Scaelorn
 */
#define COMM_NONE                61


/* WIZnet flags */
#define WIZ_ON			(A)
#define WIZ_TICKS		(B)
#define WIZ_LOGINS		(C)
#define WIZ_SITES		(D)
#define WIZ_LINKS		(E)
#define WIZ_DEATHS		(F)
#define WIZ_RESETS		(G)
#define WIZ_MOBDEATHS		(H)
#define WIZ_FLAGS		(I)
#define WIZ_PENALTIES		(J)
#define WIZ_SACCING		(K)
#define WIZ_LEVELS		(L)
#define WIZ_SECURE		(M)
#define WIZ_SWITCHES		(N)
#define WIZ_SNOOPS		(O)
#define WIZ_RESTORE		(P)
#define WIZ_LOAD		(Q)
#define WIZ_NEWBIE		(R)
#define WIZ_PREFIX		(S)
#define WIZ_SPAM		(T)
#define WIZ_INFO		(U)
#define WIZ_ETIMER              (V)
/*
 * MOBprog definitions
 */
#define TRIG_ACT	(A)
#define TRIG_BRIBE	(B)
#define TRIG_DEATH	(C)
#define TRIG_ENTRY	(D)
#define TRIG_FIGHT	(E)
#define TRIG_GIVE	(F)
#define TRIG_GREET	(G)
#define TRIG_GRALL	(H)
#define TRIG_KILL	(I)
#define TRIG_HPCNT	(J)
#define TRIG_RANDOM	(K)
#define TRIG_SPEECH	(L)
#define TRIG_EXIT	(M)
#define TRIG_EXALL	(N)
#define TRIG_DELAY	(O)
#define TRIG_SURR	(P)
#define TRIG_GET	(Q)
#define TRIG_DROP	(R)
#define TRIG_SIT	(S)

#define TURN_CLOCK	1
#define TURN_ANTICLOCK	2

/*
 * Prog Types
 */
#define PRG_MPROG	0
#define PRG_OPROG	1
#define PRG_RPROG	2

/*
 * Utility macros.
 */
#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define IS_NULLSTR(str)		((str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)	( ((min) < (num)) && ((num) < (max)) )
#define CHECK_POS(a, b, c)	{							\
					(a) = (b);					\
					if ( (a) < 0 )					\
					bug( "CHECK_POS : " c " == %d < 0", a );	\
				}							\

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

/*
 * Character macros.
 */

#define RNDDOWN(x, y)		( (x%y) ? ((x/y)-1) : (x/y) )
#define IS_NPC(ch)		(IS_FLAG((ch)->act, ACT_IS_NPC))
#define IS_ADMIN(ch)		(get_trust(ch) >= LEVEL_ADMIN)
#define IS_ATTENDING(ch)	(get_trust(ch) >= LEVEL_ATTENDING)
#define IS_IMP(ch)		(ch->level == MAX_LEVEL)
#define IS_NIMP(ch)		(ch->level >= MAX_LEVEL -1)
#define IS_CODER(ch)		(get_trust(ch) >= LEVEL_CODER)
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_RESIDENT(ch)		(ch->level >= LEVEL_RESIDENT && ch->level < LEVEL_ATTENDING)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))
#define IS_GLEADER(ch)		((ch->pcdata->guild_level < 4) || IS_ADMIN(ch))

/*
 * Utility macros for bitarrays - Scaelorn
 */
#define IS_FLAG( flag, bit )    ( ((unsigned) flag[ bit / UL_BITS ] >>  \
                                (bit % UL_BITS))  & 01 )
#define SET_FLAG(flag, bit)     ( flag[bit/UL_BITS] |= 1 << bit%UL_BITS )
#define REMOVE_FLAG(flag, bit)  ( flag[bit/UL_BITS] &= ~(1 << bit%UL_BITS ))
#define TOGGLE_FLAG(flag, bit)  ( flag[bit/UL_BITS] ^= (1 << bit % UL_BITS))
#define SET_CMP(a, b) 		(memcmp((a), (b), SETSIZE) == 0)


#define IS_SQUARE_AFF( fight, x, y, sn ) (IS_FLAG( (fight)->affected_by[(x)][(y)], (sn)))
#define IS_AFFECTED(ch, sn)	(IS_FLAG((ch)->affected_by, (sn)))
#define IS_GUIDE(ch)		(!IS_FLAG((ch)->comm, COMM_NOGUIDE))
#define IS_QUESTOR(ch)		(IS_FLAG((ch)->act, PLR_QUESTOR))

#define GET_AGE(ch)		((int) (17 + ((ch)->played \
				    + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch,type)		(ch->armor[type] < 0) ? (ch->armor[type] * -1) : (ch->armor[type])
#define GET_HITROLL(ch)	\
		( ( IS_NPC(ch) ) ? ( (ch)->hitroll + (ch)->level * 2 ) : \
		( (ch)->level * 2 + (ch)->hitroll + (ch)->pcdata->extlevel / 10 ) )

#define GET_DAMROLL(ch) \
		((ch)->damroll + (ch)->level + get_curr_stat( ch, STAT_STR ) / 5 )

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define get_carry_weight(ch)	((ch)->carry_weight/10)
#define COVERS_EYES 		1
#define COVERS_HAIR		2
#define COVERS_LOWER_FACE	4
#define COVERS_NECK		8
#define COVERS_UPPER_CHEST	16
#define COVERS_BREASTS		32
#define COVERS_LOWER_CHEST	64
#define COVERS_UPPER_BACK	128
#define COVERS_LOWER_BACK	256
#define COVERS_ARSE		512
#define COVERS_GROIN		1024
#define COVERS_THIGHS		2048
#define COVERS_FOREHEAD		4096
#define COVERS_LOWER_LEGS	8192
#define COVERS_FEET		16384
#define COVERS_UPPER_ARMS	32769
#define COVERS_LOWER_ARMS	65536
#define COVERS_HANDS		131072

#define MAX_COVERS		18

#define COVERS_SMELL		18
#define COVERS_ALL		19




#define MOUNTED(ch) \
		((!IS_NPC(ch) && ch->mount && ch->riding) ? ch->mount : NULL)
#define RIDDEN(ch) \
		((IS_NPC(ch) && ch->mount && ch->riding) ? ch->mount : NULL)
#define IS_DRUNK(ch)		((ch->pcdata->condition[COND_DRUNK] > 10))


#define act(format,ch,arg1,arg2,type)\
	act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING)

#define HAS_TRIGGER_MOB(ch,trig)(IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define HAS_TRIGGER_OBJ(obj,trig)(IS_SET((obj)->pIndexData->oprog_flags,(trig)))
#define HAS_TRIGGER_ROOM(room,trig)(IS_SET((room)->rprog_flags,(trig)))
#define IS_SWITCHED( ch )       ( ch->desc && ch->desc->original )
#define IS_BUILDER(ch, Area)	( strstr( ch->name, Area->builders )  || \
				( strstr( Area->builders, "All") ) )	\


/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? \
	(obj)->value[4] : 100)



/*
 * Description macros.
 */
#define NAME(ch)	(( IS_IMMORTAL(ch) && (ch)->short_descr[0] != '\0' ) ? \
			(ch)->short_descr : (ch)->name )

/*
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: NAME(ch) ) : "Someone" )
*/

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
//#define PLAYER_DIR	""			/* Player files	*/
#define TEMP_FILE	"romtmp"
#define NULL_FILE	"proto.are"		/* To reserve one stream */
#endif

#if defined(MSDOS)
//#define PLAYER_DIR	""			/* Player files */
#define TEMP_FILE	"romtmp"
#define NULL_FILE	"nul"			/* To reserve one stream */
#endif

#if defined(_WIN32)
	#define NULL_FILE			"proto.are"		/* To reserve one stream */
	#define TEMP_FILE			"../player/romtmp"
	#define EXE_FILE			"../debug/PatternsEnd.exe"

	#define WEATHER_FILE        "../data/weather.dat"
	#define CLAN_FILE			"../data/clans.txt"
	#define CABAL_FILE			"../data/cabals.txt"
	#define GRAVE_FILE			"../data/graves.txt"
	#define LAIR_FILE			"../data/lairs.txt"
	#define ROSTER_FILE			"../data/roster.txt"
	#define GALLERY_FILE		"../data/gallery.txt"
 	#define STORYLINE_FILE		"../data/storylines.txt"
	#define EVENT_FILE			"../data/events.txt"
	#define STORY_FILE			"../data/stories.txt"
	#define PETITION_FILE		"../data/petitions.txt"
        #define AI_IN_FILE          "../data/ai_in.csv"
        #define AI_OUT_FILE         "../data/ai_out.csv"
	#define POPS_FILE		"../data/pops.csv"
        #define IOPS_FILE               "../data/iops.csv"

        #define AI_SUM_IN_FILE          "../data/ai_sum_in.csv"
        #define AI_SUM_OUT_FILE         "../data/ai_sum_out.csv"



	#define LOCATION_FILE		"../data/locations.txt"
   	#define GATEWAY_FILE		"../data/gateways.txt"
	#define PLOT_FILE			"../data/plots/txt"
	#define STORYIDEA_FILE		"../data/storyideas.txt"
	#define RESEARCH_FILE		"../data/research.txt"
	#define ENCOUNTER_FILE		"../data/encounters.txt"
	#define PATROL_FILE			"../data/patrols.txt"
	#define GROUPTEXT_FILE 		"../data/grouptexts.txt"
    #define TEXTHISTORY_FILE    "../data/texthistories.txt"

    #define MATCH_FILE    "../data/matches.txt"
	#define PROFILE_FILE     "../data/profiles.txt"
	#define CHATROOM_FILE    "../data/chatrooms.txt"



	#define ANNIVERSARY_FILE 	"../data/anniversaries.txt"
	#define DECREE_FILE			"../data/decrees.txt"
	
	#define NEWS_FILE			"../data/news.txt"
	#define GATHERING_FILE		"../data/gatherings.txt"
 	#define HOUSE_FILE			"../data/houses.txt"
	#define PLAYERROOM_FILE		"../data/playerrooms.txt"
	#define RENT_FILE			"../data/rent.txt"
	#define CONTAINER_FILE		"../data/container.txt"
	#define DORMS_FILE		"../data/dorms.txt"
	#define MARKET_FILE			"../data/market.txt"
	#define OPERATION_FILE		"../data/operations.txt"
	#define SKILL_FILE			"../data/skill.txt"
	#define WEAVE_FILE			"../data/weave.txt"
	#define FORM_FILE			"../data/form.txt"
	#define XML_SKILL_FILE		"../data/skill.xml"
	#define XML_WEAVE_FILE		"../data/weave.xml"
	#define XML_FORM_FILE		"../data/form.xml"

	#define MAP_DIR 		    "../data/vmaps/"

	#define RESOLVE_FILE	    "../src/resolve"
	#define RACE_FILE		    "../data/races.txt"  
	#define SUBRACE_FILE   	    "../data/subraces.txt"  
	#define COMMAND_FILE	    "../data/commands.txt"
	#define LOCATION_FILE	    "../data/locations.txt"
    #define GATEWAY_FILE	    "../data/gateways.txt"

	#define CHEST_DIR		    "../data/chests/"
	#define GUILD_DIR		    "../data/guild/"        /* Has all the guild info.*/
	#define VESSEL_DIR		    "../data/vessel/"        /* Has all the guild info.*/
	#define PORT_DIR		    "../data/port/"        /* Has all the guild info.*/
	    
	#define CLUB_FILE		    "../data/clubs.txt"
	#define CITY_DIR		    "../city/"      	/* Has all the city info */

	#define SYSTEM_DIR	 	    "../data/"
	#define COUNTRY_DIR     	"../data/country/"      /* Directory for Countries */

	#define QUOTE_DIR		    "../data/quotes/"	/* Dir for quote files*/
	#define CORPSE_DIR		    "../data/corpses/"	/* where all the corpses save on crash */
	#define AREA_LIST       	"../area/area.lst"      /* List of areas*/

	#define REMORT_DIR          "../player/remort/"     /* backup dir for remorters */
	#define DELETE_DIR          "../player/delete/"     /* backup of deleted chars */
	#define PLAYER_DIR          "../player/"            /* Player files */
	#define GOD_DIR             "../gods/"              /* list of gods */
	#define ACCOUT_DIR	        "../accounts/"
	
	#define LAST_COMMAND	    "../log/lastcomm.txt"

	#define NOTES_FILE          "../data/notes/notes.dat"		/* For 'notes'*/
	#define NOTEBOARD_FILE      "../data/notes/noteboards.dat"	/* For 'notes'*/
	#define NOTEGROUP_FILE	    "../data/notes/groups.dat"		/* For 'notes'*/

	#define LOCATION_FILE	    "../data/locations.txt"
    #define GATEWAY_FILE		"../data/gateways.txt"
#endif

//#define DEV_PORT

#if defined(unix)
#ifndef DEV_PORT
    #define WEATHER_FILE        "../data/weather.dat"
    #define SKILL_FILE		    "../data/skill.txt"
    #define CLAN_FILE		    "../data/clans.txt"
    #define CABAL_FILE		    "../data/cabals.txt"
    #define GRAVE_FILE		    "../data/graves.txt"
    #define LAIR_FILE		    "../data/lairs.txt"
    #define ROSTER_FILE		    "../data/roster.txt"
    #define GALLERY_FILE        "../data/gallery.txt"
    #define STORYLINE_FILE      "../data/storylines.txt"
    #define EVENT_FILE		    "../data/events.txt"
    #define STORY_FILE		    "../data/stories.txt"
    #define PETITION_FILE	    "../data/petitions.txt"
    #define AI_IN_FILE          "../data/ai_in.csv"
    #define AI_OUT_FILE         "../data/ai_out.csv"
    #define AI_SUM_IN_FILE          "../data/ai_sum_in.csv"
    #define AI_SUM_OUT_FILE         "../data/ai_sum_out.csv"

    #define POPS_FILE               "../data/pops.csv"
    #define IOPS_FILE               "../data/iops.csv"


    #define LOCATION_FILE	    "../data/locations.txt"
    #define GATEWAY_FILE	    "../data/gateways.txt"
    #define PLOT_FILE		    "../data/plots.txt"
    #define STORYIDEA_FILE	    "../data/storyideas.txt"
    #define RESEARCH_FILE           "../data/research.txt"
    #define ENCOUNTER_FILE          "../data/encounters.txt"
    #define PATROL_FILE		    "../data/patrols.txt"
    #define GROUPTEXT_FILE 	    "../data/grouptexts.txt"
    #define TEXTHISTORY_FILE        "../data/texthistories.txt"
    #define MATCH_FILE              "../data/matches.txt"
    #define PROFILE_FILE            "../data/profiles.txt"
    #define CHATROOM_FILE           "../data/chatrooms.txt"

    #define ANNIVERSARY_FILE        "../data/anniversaries.txt"
    #define DECREE_FILE		    "../data/decrees.txt"

    #define NEWS_FILE               "../data/news.txt"
    #define RENT_FILE		    "../data/rent.txt"
    #define CONTAINER_FILE	    "../data/container.txt"
    #define DORMS_FILE	            "../data/dorms.txt"
    #define MAX_DORMROOMS           20
    #define MARKET_FILE		    "../data/market.txt"
    #define OPERATION_FILE	    "../data/operations.txt"

    #define GATHERING_FILE	    "../data/gatherings.txt"
    #define WEAVE_FILE		    "../data/weave.txt"
    #define FORM_FILE		    "../data/form.txt"
    #define XML_SKILL_FILE	    "../data/skill.xml"
    #define XML_WEAVE_FILE	    "../data/weave.xml"
    #define XML_FORM_FILE	    "../data/form.xml"

    #define MAP_DIR 		    "../data/vmaps/"

    #define RESOLVE_FILE	    "../src/resolve"
    #define RACE_FILE		    "../data/races.txt"  
    #define SUBRACE_FILE   	    "../data/subraces.txt"  
    #define COMMAND_FILE	    "../data/commands.txt"

    #define CHEST_DIR		    "../data/chests/"
    #define GUILD_DIR		    "../data/guild/"        /* Has all the guild info.*/
    #define VESSEL_DIR		    "../data/vessel/"        /* Has all the guild info.*/
    #define PORT_DIR		    "../data/port/"        /* Has all the guild info.*/
    
    #define CLUB_FILE		    "../data/clubs.txt"
    #define CITY_DIR		    "../city/"      	/* Has all the city info */

    #define SYSTEM_DIR	 	    "../data/"
    #define COUNTRY_DIR     	"../data/country/"      /* Directory for Countries */

    #define QUOTE_DIR		    "../data/quotes/"	/* Dir for quote files*/
    #define CORPSE_DIR		    "../data/corpses/"	/* where all the corpses save on crash */
    #define AREA_LIST       	"../area/area.lst"      /* List of areas*/

    #define REMORT_DIR          "../player/remort/"     /* backup dir for remorters */
    #define DELETE_DIR          "../player/delete/"     /* backup of deleted chars */
    #define PLAYER_DIR          "../player/"            /* Player files */
    #define BACK1_DIR		    "../player/back1/"
    #define BACK2_DIR		    "../player/back2/"
    #define BACK3_DIR		    "../player/back3/"
    #define BACK4_DIR           "../player/back4/"
    #define BACK5_DIR           "../player/back5/"
    #define BACK6_DIR           "../player/back6/"
    #define BACK7_DIR           "../player/back7/"

    #define GOD_DIR             "../gods/"              /* list of gods */
    #define ACCOUNT_DIR		    "../accounts/"
    #define EXE_FILE            "../src/haven"
    #define TEMP_FILE           "../player/romtmp"
    #define LAST_COMMAND	    "../log/lastcomm.txt"

    #define NOTES_FILE       	"../data/notes/notes.dat"	/* For 'notes'*/
    #define NOTEBOARD_FILE      "../data/notes/noteboards.dat"	/* For 'notes'*/
    #define NOTEGROUP_FILE	    "../data/notes/groups.dat"		/* For 'notes'*/

    #define LOCATION_FILE	    "../data/locations.txt"
    #define GATEWAY_FILE		"../data/gateways.txt"
#else
    #define WEATHER_FILE	    "/home/laurin/coders/data/weather.dat"

    #define RESOLVE_FILE        "/home/laurin/coders/data/resolve"
    #define RACE_FILE		    "/home/laurin/coders/data/races.txt"
    #define SUBRACE_FILE    	"/home/laurin/coders/data/subraces.txt"
    #define PORT_FILE    	    "/home/laurin/coders/data/ports.txt"
    #define VESSEL_FILE    	    "/home/laurin/coders/data/vessels.txt"    
    #define SKILL_FILE          "/home/laurin/coders/data/skill.txt" 
    #define WEAVE_FILE          "/home/laurin/coders/data/weave.txt" 
    #define FORM_FILE           "/home/laurin/coders/data/form.txt"  
    #define XML_SKILL_FILE	    "/home/laurin/coders/data/skill.xml" 
    #define XML_WEAVE_FILE 	    "/home/laurin/coders/data/weave.xml" 
    #define XML_FORM_FILE	    "/home/laurin/coders/data/form.xml"  
    
    #define MAP_DIR 		    "/home/laurin/coders/data/vmaps/"

    #define COMMAND_FILE    	"/home/laurin/coders/data/commands.txt"
                                
    #define CHEST_DIR       	"/home/laurin/coders/data/chests/"
    #define GUILD_DIR       	"/home/laurin/coders/data/guild/"        /* Has all the guild info.*/
    #define VESSEL_DIR       	"/home/laurin/coders/data/vessel/"        /* Has all the guild info.*/
    #define PORT_DIR       	    "/home/laurin/coders/data/port/"        /* Has all the guild info.*/
    
    #define CLUB_FILE       	"/home/laurin/coders/data/clubs.txt"
    #define CITY_DIR        	"/home/laurin/coders/city/"              /* Has all the city info */
 
    #define SYSTEM_DIR      	"/home/laurin/coders/data/"
    #define COUNTRY_DIR     	"/home/laurin/coders/data/country/"      /* Directory for Countries */
 
    #define QUOTE_DIR       	"/home/laurin/coders/data/quotes/"       /* Dir for quote files*/
    #define CORPSE_DIR      	"/home/laurin/coders/data/corpses/"      /* where all the corpses save on crash */
    #define AREA_LIST       	"../area/area.lst"      /* List of areas*/

    #define REMORT_DIR          "/home/laurin/coders/palin/mud/player/remort/"     /* backup dir for remorters */
    #define DELETE_DIR          "/home/laurin/coders/palin/mud/player/delete/"     /* backup of deleted chars */
//    #define PLAYER_DIR          "/home/laurin/coders/palin/mud/player/"            /* Player files */
    #define GOD_DIR             "/home/laurin/coders/palin/mud/gods/"              /* list of gods */
    #define EXE_FILE            "/home/laurin/coders/palin/mud/src/rom"
    #define TEMP_FILE           "/home/laurin/coders/palin/mud/player/romtmp"
    #define LAST_COMMAND        "/home/laurin/coders/palin/mud/log/lastcomm.txt"

    #define NOTES_FILE       	"/home/laurin/coders/data/notes/notes.dat"	/* For 'notes'*/
    #define NOTEBOARD_FILE      "/home/laurin/coders/data/notes/noteboards.dat"	/* For 'notes'*/
    #define NOTEGROUP_FILE	    "/home/laurin/coders/data/notes/groups.dat"		/* For 'notes'*/

    #define LOCATION_FILE	    "/home/laurin/coders/data/locations.txt"		/* For 'notes'*/
#endif

#define NULL_FILE           	"/dev/null"             /* To reserve one stream */
#endif

#define MAP_LIST 				"maps.lst"
#define ENTRANCE_FILE 	        "entrances.dat"
#define LANDMARK_FILE 	        "landmarks.dat"
#define BUG_FILE                "bugs.txt" 	/* For 'bug' and bug()*/
#define TYPO_FILE               "typos.txt" 	/* For 'typo'*/
#define NOTE_FILE               "notes.not"	/* For 'notes'*/
#define IDEA_FILE				"ideas.not"
#define PENALTY_FILE		    "penal.not"
#define CHANGES_FILE		    "chang.not"
#define SHUTDOWN_FILE           "shutdown.txt"	/* For 'shutdown'*/
#define BAN_FILE				"ban.txt"
#define MUSIC_FILE			    "music.txt"
#define CORPSE_LIST			    "corpses.lst"	/* where all the corpses save on crash */
#define CHEST_FILELIST          "chest.lst"
#define GUILD_LIST              "guild.lst"	/* List of guilds.		*/
#define VESSEL_LIST			    "vessel.lst"	/* List of vessels.		*/
#define PORT_LIST				"port.lst"	/* List of ports.		*/
#define CITY_LIST               "city.lst"	/* List of cities 		*/
#define SYSTEM_DAT_FILE         "sysconfig.dat"
#define COUNTRY_LIST            "country.lst"   /* List of Countries */
#define DISABLED_FILE           "disabled.txt."

/* Copyover Defines */

#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/* This file holds the copyover data */
#define COPYOVER_FILE "copyover.data"

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY	30

/*
 * Area flags.
 */
#define AREA_NONE       0
#define AREA_CHANGED    1	/* Area has been modified. */
#define AREA_ADDED      2	/* Area has been added to. */
#define AREA_LOADING    4	/* Used for counting in db.c */
#define	AREA_COMPLETED  5       /* Used for Questing       */


#define CLAN_DICTATORSHIP	0
#define CLAN_DEMOCRACY		1
#define CLAN_DOMINANCE		2
#define CLAN_COMMUNAL		3
#define CLAN_REPUTATION		4

#define PIN_PIN 1


#define JOB_DOWNPAYMENT		1
#define JOB_APPEARANCE		2
#define JOB_STR			3
#define JOB_REPUTATION		4
#define JOB_JOB			5
#define JOB_VIT			6
#define JOB_MART		7
#define JOB_WASS		8
#define JOB_WSK			9


#define FEATURE_STALECTITE	1
#define FEATURE_STALECMITE	2
#define FEATURE_BOULDER		3
#define FEATURE_LOG		4
#define FEATURE_MONOLITH	5
#define FEATURE_CART		6
#define FEATURE_MINE_CART	7
#define FEATURE_WATER_WALL	8
#define FEATURE_STORE_WALL	9
#define FEATURE_BALCONY		10
#define FEATURE_PILLAR		11
#define FEATURE_BRUSH		12
#define FEATURE_BIN		13
#define FEATURE_PIT		14
#define FEATURE_TABLE		15
#define FEATURE_WARDROBE	16
#define FEATURE_CHEST		17
#define FEATURE_CARRIAGE	18
#define FEATURE_STAND		19
#define FEATURE_TROUGH		20


#define BACKGROUND_SHOPAPP		0
#define BACKGROUND_BARB			1
#define BACKGROUND_BULLY		2
#define BACKGROUND_BOOK			3
#define BACKGROUND_CHARLATAN		4
#define BACKGROUND_WARRIORCHILD		5
#define BACKGROUND_SOLDIERCHILD		6
#define BACKGROUND_EDUCATED		7
#define BACKGROUND_FERAL_CHILD		8
#define BACKGROUND_HYDROPHOBIC		9
#define BACKGROUND_NOBLE		10
#define BACKGROUND_CIRCUS		11
#define BACKGROUND_ELVES		12
#define BACKGROUND_ORCS			13
#define BACKGROUND_DWARVES		14
#define BACKGROUND_SLAVES		15
#define BACKGROUND_GYPSY		16
#define BACKGROUND_URCHIN		17
#define MAX_BACKGROUNDS			18

#define BLOCK_LEFT 	0
#define BLOCK_RIGHT	1
#define BLOCK_CENTER	2
#define BLOCK_UP	3
#define BLOCK_DOWN	4
#define BLOCK_MIDDLE	5

#define DIST_BROWN		2
#define DIST_BOSTON             3

#define DIST_AMERICA		4
#define DIST_MEXICO		5
#define DIST_CANADA		6
#define DIST_SAMERICA		7
#define DIST_EUROPE		8
#define DIST_AFRICA		9
#define DIST_ASIA		10
#define DIST_AUSTRALIA		11
#define DIST_MISTS		12
#define DIST_OTHER		26
#define DIST_GODREALM		28
#define DIST_WILDS		27
#define DIST_DREAM		30

#define MAX_DIST                14


#define MAX_ROOMS		10


#define SKILL_REGEN                     1
#define SKILL_NIGHTVISION               2
#define SKILL_TELEPATHY                 3
#define SKILL_ATTRACTIVENESS            4
#define SKILL_HYPNOTISM                 5
#define SKILL_WEALTH                    6
#define SKILL_DARKSORC                  7
#define SKILL_FIRESORC                  8
#define SKILL_ICESORC                   9
#define SKILL_LIGHTNINGSORC             10
#define SKILL_VIRGIN                    11
#define SKILL_FLIGHT                    12
#define SKILL_SUPERJUMP                 13
#define SKILL_STRENGTH                  14
#define SKILL_DEXTERITY                 15
#define SKILL_ACROBATICS                16
#define SKILL_RUNNING                   17
#define SKILL_EDUCATION                 18
#define SKILL_CHARISMA                  19
#define SKILL_FAME                      20
#define SKILL_LANGUAGE2                 21
#define SKILL_LANGUAGE3                 22
#define SKILL_LANGUAGE4         	23
#define SKILL_LANGUAGE5                 24
#define SKILL_ART                       25
#define SKILL_ART2                      26
#define SKILL_ART3              	27
#define SKILL_CAR                       28
#define SKILL_DODGING                   29
#define SKILL_EVADING                   30
#define SKILL_CPOLICE                   31
#define SKILL_CMILITARY                 32
#define SKILL_CGOVERNMENT               33
#define SKILL_CMEDIA                    34
#define SKILL_CCRIMINAL                 35
#define SKILL_COCCULT                   36
#define SKILL_ENDOWMENT                 37
#define SKILL_DEMOLITIONS               38
#define SKILL_HACKING                   39
#define SKILL_STEALTH                   40
#define SKILL_PERCEPTION                41
#define SKILL_MEDICINE                  42
#define SKILL_DEMONOLOGY                43
#define SKILL_ENGINEERING               44
#define SKILL_HANDYMAN                  45
#define SKILL_STAMINA                   46
#define SKILL_LARCENY                   47
#define SKILL_ANIMALS                   48
#define SKILL_MARTIALART                49
#define SKILL_COMBATTRAINING            50
#define SKILL_FASTREFLEXES              51
#define SKILL_ACUTESIGHT                52
#define SKILL_ACUTESMELL                53
#define SKILL_ACUTEHEAR                 54
#define SKILL_KNOWVAMP                  55
#define SKILL_KNOWWERE                  56
#define SKILL_KNOWMAGE                  57
#define SKILL_AGELESS                   58
#define SKILL_ALLIES                    59
#define SKILL_MINIONS                   60
#define SKILL_CHOMELESS                 61
#define SKILL_BIKE                      62
#define SKILL_ARTAMULET                 63
#define SKILL_GAMES                     64
#define SKILL_DEMIGOD                   65
#define SKILL_TIMESWEPT                 66
#define SKILL_FAEBORN                   67
#define SKILL_ANGELBORN                 68
#define SKILL_DEMONBORN                 69
#define SKILL_OBFUSCATE                 70
#define SKILL_TELEMPATHY                71
#define SKILL_LIGHTUP                   72
#define SKILL_ELECTROPATHIC             73
#define SKILL_CLAIRVOYANCE              74
#define SKILL_CLAIRAUDIENCE             75
#define SKILL_SHAPESHIFTING             76
#define SKILL_HYPERSHIFTING             77
#define SKILL_AURASHIFTING              78
#define SKILL_METASHIFTING              79
#define SKILL_SENSEBLOOD                80
#define SKILL_FACESTEALING		        81
#define SKILL_SILVERVULN                82
#define SKILL_GOLDVULN                  83
#define SKILL_GHOSTWALKING		        84
#define SKILL_RITUALISM			        85
#define SKILL_ILLUSIONS			        86
#define SKILL_FORENSICS			        87
#define SKILL_DREAMING			        88
#define SKILL_PRISSORC			        89
#define SKILL_BONEVULN	  	            90
#define SKILL_WOODVULN			        91
#define SKILL_HYPERSTRENGTH		        92
#define SKILL_HYPERREFLEXES		        93
#define SKILL_HYPERSPEED		        94
#define SKILL_SWARMSHIFTING		        95
#define SKILL_HYBRIDSHIFTING		    96
#define SKILL_CUSTOM			        97
#define SKILL_WINGS			            98
#define SKILL_MONSTERSHIFT		        99
#define SKILL_BRANDING			        100
#define SKILL_GUARDEDMIND		101
#define SKILL_ARMORED			102
#define SKILL_OCCULTFOCUS		103
#define SKILL_MESMERIZING		104
#define SKILL_HYPERREGEN		105
#define SKILL_MISTVISION		106
#define SKILL_DREAMSTALK		107
#define SKILL_FLESHFORMED		108
#define SKILL_GADGETS			109
#define SKILL_MARTIAL			110
#define SKILL_SOLDIER			111
#define SKILL_COMMANDO			112
#define SKILL_RIOT			113
#define SKILL_NATURE			114
#define SKILL_PSYCHIC			115
#define SKILL_COMBAT_ILLUSIONS		116
#define SKILL_HAEMOMANCY		117
#define SKILL_TELEKINESIS		118
#define SKILL_WEATHER			119
#define SKILL_MAGNO			120
#define SKILL_BIOMANCY			121
#define SKILL_CRYO			122
#define SKILL_PYRO			123
#define SKILL_NECRO			124
#define SKILL_MINDREADING		125
#define SKILL_CRYPTOMIND		126
#define SKILL_MEMORYCLOAK		127
#define SKILL_ENTHRALL			128
#define SKILL_ENRAPTURE			129
#define SKILL_SEEKPREY			130
#define SKILL_SUNRESIST			131
#define SKILL_FIREPROOF			132
#define SKILL_ICEPROOF			133
#define SKILL_ELECPROOF			134
#define SKILL_IMMORTALITY		135
#define SKILL_FLESHFORMING		136
#define SKILL_MIMIC			137
#define SKILL_RITUALPROOF		138
#define SKILL_HYPERPERCEPTION		139
#define SKILL_RESURRECTION		140
#define SKILL_MENTALDISCIPLINE		141
#define SKILL_SHADOWCLOAK		142
#define SKILL_CLOUDSUMMON		143
#define SKILL_FAEKNIGHTFOCUS		144
#define SKILL_DEMONWARRIORFOCUS		145
#define SKILL_ANGELCHAMPIONFOCUS	146
#define SKILL_DEMIWARRIORFOCUS		147
#define SKILL_BLADEDISCFOCUS		148
#define SKILL_GUNDISCFOCUS		149
#define SKILL_PRIMALFOCUS		150
#define SKILL_WARRIORFOCUS		151
#define SKILL_SOLDIERFOCUS		152
#define SKILL_ASSASSINFOCUS		153
#define SKILL_ILLUSORYMAGICFOCUS	154
#define SKILL_NATURALMAGICFOCUS		155
#define SKILL_DARKMAGICFOCUS		156
#define SKILL_ELEMAGICFOCUS		157
#define SKILL_BUSINESSFOCUS		158
#define SKILL_GENERALFOCUS		159
#define SKILL_MOGULFOCUS		160
#define SKILL_SOCIALFOCUS		161
#define SKILL_SCIENCEFOCUS		162
#define SKILL_THIEFFOCUS		163
#define SKILL_ENGINEERFOCUS		164

#define SKILL_SEDUCE			165
#define SKILL_ENRAGE			166
#define SKILL_RAGEFUELED		167
#define SKILL_NIGHTMARE 		168
#define SKILL_WAILING			169
#define SKILL_STORMCALLING		170
#define SKILL_OCEANAFFINITY		171
#define SKILL_SOLARBLESS		172
#define SKILL_LUNARBLESS		173
#define SKILL_DEATHTOUCH		174
#define SKILL_WARBLESS			175
#define SKILL_FERTILITY			176
#define SKILL_LUCKCHANGE		177
#define SKILL_MERSHIFTER		178
#define SKILL_BLOOMING			179
#define SKILL_SIRENSONG			180
#define SKILL_SENSEWEAKNESS		181
#define SKILL_GUARDIAN			182
#define SKILL_FLEETFOOT			183
#define SKILL_SOOTHING			184
#define SKILL_TOUCHED			185
#define SKILL_CHAND			186
#define SKILL_CORDER			187
#define SKILL_CTEMPLE			188
#define SKILL_CSUBFACTION		189
#define SKILL_ALCHEMY			190
#define SKILL_CONDITIONING		191
#define SKILL_BEAUTY			192
#define SKILL_FASHION			193
#define SKILL_INCANTATION		194

#define SKILL_ANGELCOMBAT		195
#define SKILL_DAYASPECT			196
#define SKILL_NIGHTASPECT		197
#define SKILL_PRIMALCOMBAT		198

#define SKILL_GLIDE			199
#define SKILL_FLUIDSHIFT		200
#define SKILL_DUALASPECT		201
#define SKILL_DEMONMETABOL		202
#define SKILL_VICIOUSSTRIKES		203
#define SKILL_BLADEDEFENSE		204
#define SKILL_QUICKRELOAD		205
#define SKILL_ILLUSORYAFFINITY		206
#define SKILL_NATURALAFFINITY		207
#define SKILL_MAGICOVERLOAD		208
#define SKILL_DUALELEMENTS		209
#define SKILL_SNIPERTRAINING		210
#define SKILL_RIPOSTE			211
#define SKILL_PARALYTIC			212
#define SKILL_POISONTOXIN		213
#define SKILL_GIANTSLAYER		214
#define SKILL_DISPOSABLEMUSCLE		215
#define SKILL_PROTECTIONDETAIL		216
#define SKILL_MILITARYTRAINING		217
#define SKILL_PERSUADE			218
#define SKILL_PATHING			219
#define SKILL_CALLNATURE		220
#define SKILL_CALLGRAVE			221
#define SKILL_ELEMINIONS		222
#define SKILL_POWERARMOR		223
#define SKILL_FAMOUS			224
#define SKILL_FINANCIALRUIN		225
#define SKILL_ARRANGEHIT		226
#define SKILL_VIEWSECRETS		227
#define SKILL_CLINICCONTRACT		228
#define SKILL_COLLEGECONTRACT		229
#define SKILL_DREAMSIGHT		230
#define SKILL_SPLITMIND			231
#define SKILL_COLDSPEC			232
#define SKILL_HOTSPEC			233
#define SKILL_USED                      233

#define SKILL_MAX                       300

#define DIS_TELEKINESIS		1
#define DIS_PISTOLS		2
#define DIS_THROWN		3
#define DIS_BOWS		4
#define DIS_ENERGY		5
#define DIS_FIRE		6
#define DIS_ICE			7
#define DIS_LIGHTNING		8
#define DIS_DARK		9
#define DIS_SHOTGUNS		10
#define DIS_TELEPATHY		11
#define DIS_SONIC		12
#define DIS_RADIATION		13
#define DIS_STONE		14
#define DIS_LONGBLADE		15
#define DIS_STRIKING		16
#define DIS_KNIFE		17
#define DIS_CLAW		18
#define DIS_ENERGYF		19
#define DIS_FIREF		20
#define DIS_ICEF		21
#define DIS_LIGHTNINGF		22
#define DIS_DARKF		23
#define DIS_BRUTE		24
#define DIS_BARMOR		25
#define DIS_MARMOR		26
#define DIS_NARMOR		27
#define DIS_TOUGHNESS		28
#define DIS_ENERGYS		29
#define DIS_FIRES		30
#define DIS_ICES		31
#define DIS_LIGHTNINGS		32
#define DIS_STONES		33
#define DIS_MENTALS		34
#define DIS_DARKS		35

#define DIS_WEATHER		36
#define DIS_NATURE		37
#define DIS_WOODS		38
#define DIS_CUSTOM		39
#define DIS_RIFLES		40
#define DIS_CARBINES		41
#define DIS_BLUNT		42
#define DIS_HELLFIRE		43
#define DIS_SHADOWL		44
#define DIS_GRAPPLE		45
#define DIS_BONES		46
#define DIS_FORCES		47
#define DIS_UNDEAD		48
#define DIS_FATE		49
#define DIS_PUSH		50
#define DIS_BSHIELD		51
#define DIS_MSHIELD		52
#define DIS_PRISMATIC		53
#define DIS_SPEAR		54
#define DIS_SPEARGUN		55

#define DIS_USED		56

#define MAX_DIS			100

#define FACTION_HAND		5
#define FACTION_ORDER		6
#define FACTION_TEMPLE		7


#define FACTION_CORE		1
#define FACTION_NPC			2
#define FACTION_SECT		3
#define FACTION_CULT		4

#define BOON_BEAUTY		1
#define BOON_STRENGTH	2
#define BOON_LAVISH		3
#define BOON_GIFT		4
#define BOON_LOVER		5
#define BOON_SILVERTONGUE	6
#define BOON_VITALITY	7
#define BOON_ASTRAL		8
#define BOON_NOWORRIES	9
#define BOON_BLISS		10
#define BOON_RENOVATE		11


#define CURSE_GULLIBLITY	1
#define CURSE_NARCISSISM	2
#define CURSE_REVELATION		3
#define CURSE_WEAKNESS	4
#define CURSE_TIMID	5
#define CURSE_PURITANICAL 6
#define CURSE_DEPENDENCY 7
#define CURSE_PHOBIA	8
#define CURSE_IMPRINT	9



#define MOVE_MOVE		0
#define MOVE_CHARGE		1
#define MOVE_JUMP		2
#define MOVE_RETREAT		3
#define MOVE_FLY		4
#define MOVE_FLEE		5
#define MOVE_PROTECT		6
#define MOVE_THROWN		7
#define MOVE_UP			8
#define MOVE_DOWN		9
#define MOVE_PATH		10

#define SHAPE_HUMAN             0
#define SHAPE_WOLF              1
#define SHAPE_ANIMALONE         2
#define SHAPE_ANIMALTWO         3
#define SHAPE_ANIMALTHREE       4
#define SHAPE_ANIMALFOUR        5
#define SHAPE_ANIMALFIVE        6
#define SHAPE_ANIMALSIX         7
#define SHAPE_MERMAID           8

#define SHAPE_MASKONE		20
#define SHAPE_MASKTWO		21

#define FIGHT_NOATTACK	0
#define FIGHT_NOMOVE	1
#define FIGHT_OVERPOWER	5
#define FIGHT_WEAKEN	6
#define FIGHT_UNDERPOWER 7
#define FIGHT_FLEEING 8

#define FIGHT_MINIONS1	9
#define FIGHT_MINIONS2	10
#define FIGHT_ALLIES1	11
#define FIGHT_ALLIES2	12
#define FIGHT_ALLIES3	13
#define FIGHT_ALLIES4	14

#define FIGHT_KNOCKOUT	15
#define FIGHT_KNOCKFAIL	16

#define FIGHT_SPECUSED	17
#define FIGHT_DEFEND	18

#define FIGHT_AOE	19
#define FIGHT_WOUND	20
#define FIGHT_SLOW1	21
#define FIGHT_SLOW2	22
#define FIGHT_WEAKEN1	23
#define FIGHT_WEAKEN2	24
#define FIGHT_WEAKEN3	25

#define FIGHT_SUPPRESSOR 26
#define FIGHT_BIPOD	 27
#define FIGHT_DEADSWITCH	28
#define FIGHT_GASMASK	29
#define FIGHT_NOSHIELD	30
#define FIGHT_NOSPEAR	31

#define FIGHT_MAXFLAG	31

#define SPECIAL_STUN            1
#define SPECIAL_KNOCKBACK       2
#define SPECIAL_ENTRAP          3
#define SPECIAL_GRAPPLE         4
#define SPECIAL_LIFESTEAL       5
#define SPECIAL_DELAY           6
#define SPECIAL_DELAY2          7
#define SPECIAL_OVERPOWER       8
#define SPECIAL_DAMAGE          9
#define SPECIAL_WEAKEN          10
#define SPECIAL_UNDERPOWER      11
#define SPECIAL_LONGWEAKEN      12
#define SPECIAL_SLOW            13
#define SPECIAL_AOE             14
#define SPECIAL_WOUND           15

#define MAX_SPECIAL		15

#define SPECIAL_MINION		50
#define SPECIAL_ALLY		51

#define EXP_MULT		160
#define RPEXP_MULT		220

#define EXP_GAIN_MULT		200

#define FIGHT_WAIT		15

#define BASE_DIFFICULTY		75

#define WORD_WRAP		80

#define GMHOME			60

#define BASE_MONSTER		75
#define MONSTER_TEMPLATE	3
#define MINION_TEMPLATE		140
#define ALLY_TEMPLATE		141

#define HAND_LT              101
#define ORDER_LT             103
#define TEMPLE_LT                 109

#define HAND_SOLDIER         100
#define ORDER_SOLDIER        102
#define TEMPLE_SOLDIER            108

#define POLICE_OFFICER		6
#define POLICE_SWAT		7
#define NATIONAL_GUARD		8

#define HUNTER			4
#define HUNTER_CHAMP		9


#define TRUST_BASE	0
#define TRUST_COMMS	1
#define TRUST_NEWS	2
#define TRUST_SPIRIT	3
#define TRUST_RESEARCH	4
#define TRUST_RESOURCES	5
#define TRUST_BRAINWASH	6
#define TRUST_SUSPENSIONS 7
#define TRUST_PROMOTIONS 8
#define TRUST_RECRUITMENT 9
#define TRUST_BANISHING 10
#define TRUST_WAR	11

#define FACTION_UNDERSTANDING	0
#define FACTION_SCOUTS		2
#define FACTION_COMMS		3
#define FACTION_911		1
#define FACTION_CORPSE		4
#define FACTION_LOYALTY		5


#define STYPE_SUPERNATURAL	1
#define STYPE_SOCIAL		2
#define STYPE_MATERIAL		3
#define STYPE_PHYSICAL		4
#define STYPE_INTELLECTUAL	5
#define STYPE_CONTACTS		6
#define STYPE_SKILLS		7
#define STYPE_ORIGIN		8
#define STYPE_ABILITIES		9
#define STYPE_SABILITIES	10
#define STYPE_ABOMINATION	11
#define STYPE_DEFENSE           12
#define STYPE_SPECIALIZATION    13
#define STYPE_COMBATFOCUS	14
#define STYPE_ARCANEFOCUS	15
#define STYPE_PROFFOCUS		16

#define DTYPE_UNKNOWN           0
#define DTYPE_DEFENSE           1
#define DTYPE_CLOSECOMBAT       2
#define DTYPE_RANGED            3

#define SCON_NONE		0
#define SCON_WEAKEN		1
#define SCON_NOHEAL		2
#define SCON_POLICE		3
#define SCON_HUNTERS		4
#define SCON_NOMOVE		5
#define SCON_LOCALMOVE		6

#define EVENT_NONE               0
#define EVENT_MINDCONTROL       1
#define EVENT_DISCREDIT         2
#define EVENT_SUE               3
#define EVENT_OUTCAST           4

#define EVENT_BRAINWASH         5
#define EVENT_HEIST             6
#define EVENT_OPERATION         7
#define EVENT_RECRUITMENT       8
#define EVENT_TEACHING          9
#define EVENT_OCCURANCE         10
#define EVENT_CONDITION         11

#define EVENT_DOMINANCE         12
#define EVENT_AEGIS             13
#define EVENT_CLEANSE           14
#define EVENT_CATASTROPHE       15

#define EVENT_UNDERSTANDINGPLUS	16
#define EVENT_UNDERSTANDINGMINUS 17
#define EVENT_ROB               18

#define EVENT_DISRUPTION	19
#define EVENT_STORM		20
#define EVENT_FLOOD		21
#define EVENT_BLACKOUT		22
#define EVENT_HURRICANE		23

#define EVENT_MALADY		24
#define EVENT_HAUNT		25
#define EVENT_DREAMBELIEF	26

#define EVENT_UNINVITED		27
#define EVENT_MUTE		28

#define EVENT_SABOTAGE		29
#define EVENT_AMBUSH		30

#define MAX_EVENT               31

#define BLOOD_REGULAR	0
#define BLOOD_VIRGIN	1
#define BLOOD_FAEBORN	2
#define BLOOD_ANGELBORN	3
#define BLOOD_DEMONBORN	4
#define BLOOD_DEMIGOD	5

#define DEFAULT_MAXEXP		1000000
#define DEFAULT_MAXRPEXP	1000000

#define SKILL_MULTIPLIER	110

#define MAX_TAXIS 20
#define INIT_CABS 19010
#define END_CABS 19099


#define PROP_HOUSE      1
#define PROP_SHOP       2
#define PROP_PUBLIC     3
#define PROP_LAND	4

#define PROP_SMALL      1
#define PROP_MEDIUM     2
#define PROP_LARGE      3
#define PROP_XLARGE     4
#define PROP_TALL       5

#define CONNECT_NODOOR          1
#define CONNECT_FRONTDOOR       2
#define CONNECT_BASEMENT        3
#define CONNECT_WINDOW          4
#define CONNECT_SHOPDOOR	5
#define CONNECT_ONEWAY		6

#define BATTLE_OAK              0
#define BATTLE_STONE            1
#define BATTLE_WELL             2
#define BATTLE_GLADE            3

#define BORDER_NONE		0
#define BORDER_ATTACK		1
#define BORDER_FOLLOW		2

#define HABIT_FEEDING       0
#define HABIT_LUNACY        1
#define HABIT_SADISM        2
#define HABIT_EATING        3
#define HABIT_SEX           4
#define HABIT_SMOKING       5
#define HABIT_DRINKING      6
#define HABIT_GYM           7
#define HABIT_DRUGS         8
#define HABIT_GAMES         9
#define HABIT_COMICS        10
#define HABIT_TV            11
#define HABIT_NOVELS        12
#define HABIT_CLUBBING      13
#define HABIT_SPORTS        14
#define HABIT_ORIENTATION	15
#define HABIT_SELFESTEEM	16
#define HABIT_CONCERN	    17
#define HABIT_DEMOCRACY     18
#define HABIT_EQUALITY      19
#define HABIT_CRUELTY       20
#define HABIT_INTEL	        21
#define HABIT_PROTECTION    22
#define HABIT_HORMONES	    23
#define HABIT_SCHOOL	    24
#define MAX_HABITS          24
#define MAX_NEWHABITS       30

#define FIST_HIGH	1
#define FIST_LOW	2
#define FIST_KICK	3
#define FIST_RETREAT	4

#define GUARD_HIGH	1
#define GUARD_LOW	2

#define TYPE_KARMA	1
#define TYPE_DEXP	2

#define MAX_DIR	10
#define NO_FLAG -99	/* Must not be used in flags or stats. */

#define LEGACY_WEREWOLF	1

#define PLOT_OTHER      0
#define PLOT_ADVENTURE  1
#define PLOT_JOINT      2
#define PLOT_PVP        3
#define PLOT_CHARACTER	4
#define PLOT_QUEST	5
#define PLOT_PERSONAL	6
#define PLOT_MYSTERY	7
#define PLOT_CRISIS	8

#define STATUS_GOVERN   1
#define STATUS_SHARED   2
#define STATUS_CONTEST  3
#define STATUS_WAR      4
#define STATUS_WILD     5

#define CONTINENT_NA    1
#define CONTINENT_SA    2
#define CONTINENT_EU    3
#define CONTINENT_ASIA  4
#define CONTINENT_AFRICA 5
#define CONTINENT_AUS   6
#define CONTINENT_WILDS 7
#define CONTINENT_OTHER 8
#define CONTINENT_GODREALM 9
#define CONTINENT_HELL	10


#define BASE_ATTRACT    0
#define BOOST_ATTRACT   1
#define AVERAGE_OUTFIT  2
#define ATTRACT_MAKEUP  3
#define ATTRACT_PRAISE  4
#define AVERAGE_JEWELRY 5
#define FATIGUE_MAKEUP1 6
#define FATIGUE_MAKEUP2 7
#define FATIGUE_MAKEUP3 8
#define FATIGUE_HEELS1  9
#define FATIGUE_HEELS2  10
#define FATIGUE_BUST1   11
#define FATIGUE_BUST2   12
#define FATIGUE_PERFUME1        13
#define FATIGUE_PERFUME2        14
#define ATTRACT_BEHAVE	15
#define ATTRACT_PROM	16
#define ATTRACT_STANDARDS 17
#define FATIGUE_HEALTH	18

#define INIT_FIGHTROOM	105000000
#define END_FIGHTROOM	105000030

#define REL_CHILD	1
#define REL_PARENT	2
#define REL_SIBLING	3
#define REL_PACKMATE	4
#define REL_SIRE	5
#define REL_SIRELING	6
#define REL_STUDENT	7
#define REL_TEACHER	8
#define REL_EMPLOYER	9
#define REL_EMPLOYEE	10
#define REL_SPOUSE	11
#define REL_GUARDIAN	12
#define REL_WARD	13
#define REL_DATING	14
#define REL_WEAKNESS	15
#define REL_WEAKNESS_OF	16
#define REL_PRAESTES	17
#define REL_PRAESTES_OF 18
#define REL_PACTEE      19
#define REL_PACTER      20



#define CAFF_BRUTE	1
#define CAFF_BLIND	2
#define CAFF_SLOW	3
#define CAFF_WEAKEN	4
#define CAFF_DISARMED	5
#define CAFF_THROWING	6
#define CAFF_LASERED	7
#define CAFF_SUPPRESSED	8
#define CAFF_CALTROPING	9
#define CAFF_LANDMINING 10
#define CAFF_SPRINTING	11
#define CAFF_TEAR	12
#define CAFF_BEWILDER	13
#define CAFF_CONFUSE	14
#define CAFF_STASIS	15
#define CAFF_FEAR	16
#define CAFF_MIMIC	17
#define CAFF_CLOAKBLIND	18
#define CAFF_AURA	19
#define CAFF_TRICKING	20
#define CAFF_HEARTSLOW	21
#define CAFF_DECLOT	22
#define CAFF_GUST	23
#define CAFF_JAM	24
#define CAFF_ATTRACT	25
#define CAFF_REPEL	26
#define CAFF_OVERLOAD	27
#define CAFF_REFLECT	28
#define CAFF_ADRENALINE	29
#define CAFF_RESISTPAIN	30
#define CAFF_CONSTRICT	31
#define CAFF_CHILL	32
#define CAFF_SWEAT	33
#define CAFF_BURNARMOR	34
#define CAFF_ICEARMOR	35
#define CAFF_STICK	36
#define CAFF_DISCIPLINE	37
#define CAFF_BURNWEAPON	38
#define CAFF_HEATMETAL	39
#define CAFF_COMMANDED	40
#define CAFF_MELEED	41
#define CAFF_NOSHIFTING	42
#define CAFF_NEUTRALIZED 43
#define CAFF_JUMPED	44
#define CAFF_STUNNED	45
#define CAFF_ROOT	46
#define CAFF_GRAVITY	47
#define CAFF_FIREWEAPON	48
#define CAFF_GLOWING	49
#define CAFF_RUSH	50
#define CAFF_PROWL	51
#define CAFF_AMESSENGER	52
#define CAFF_ASUN	53
#define CAFF_AWARRIOR	54
#define CAFF_ASMITH	55
#define CAFF_AHEALER	56
#define CAFF_AUNDERWORLD 57
#define CAFF_AMOON	58
#define CAFF_ATHUNDER	59
#define CAFF_AWIND	60
#define CAFF_AHUNTER	61
#define CAFF_GLIDED	62
#define CAFF_GLIDING	63
#define CAFF_FLUIDSHIFT 64
#define CAFF_PARALYTICTOXIN 65
#define CAFF_POISONTOXIN 66
#define CAFF_TKJUMP	67
#define CAFF_TRIAGE	68
#define CAFF_SEMIWEAKEN 69

#define COBJ_CALTROPS	120
#define COBJ_LANDMINE	121
#define COBJ_SMOKE	122
#define COBJ_TEAR	123
#define COBJ_FRAG	124
#define COBJ_QUICKSAND	125
#define COBJ_THICKAIR	126

//Core Area vnums
//Modifiers are for offsetting coordinates  i.e. there's no reason haven streets needs
//underground vnums because that's the purpose of HavenUnderground
#define HavenSize                  300000
#define HavenUnderground        100000000
#define HavenStreets            105000000
#define HavenStreetsModifier      5000000
#define HavenSkies              106000000
#define HavenSkiesModifier        6000000
#define HavenWoods              205000000
#define HavenWoodsModifier        5000000
#define HavenPrivateProperty    300000000
#define HavenCityBuildings      400000000
#define HavenOcean              500000000
    
//Offworld area vnums
#define TheOther                25500
#define TheWilds                25600
#define TheGodrealm             25700

#define SIN_LUST		1
#define SIN_GLUTTONY		2
#define SIN_GREED		3
#define SIN_SLOTH		4
#define SIN_WRATH		5
#define SIN_ENVY		6
#define SIN_PRIDE		0


#define ANIMAL_ACTIVE		10
#define ANIMAL_PRIMARY		11
#define ANIMAL_WOLF		12

#define ANIMAL_POWER		1
#define ANIMAL_TOUGHNESS	2
#define ANIMAL_CAMOUFLAGE	3
#define ANIMAL_SPEED		4
#define ANIMAL_AGILITY		5
#define ANIMAL_SENSES		6
#define ANIMAL_FLIGHT		7
#define ANIMAL_NOCTURNAL	8
#define ANIMAL_POISONOUS	9
#define ANIMAL_AMPHIBIOUS	10
#define ANIMAL_ARMORED		11
#define ANIMAL_SMELL		12
#define ANIMAL_SIGHT		13
#define ANIMAL_HEARING		14

#define GENUS_INSECT		1
#define GENUS_RODENT		2
#define GENUS_BIRD		3
#define GENUS_AQUATIC		4
#define GENUS_REPTILE		5
#define GENUS_MAMMAL		6
#define GENUS_MYTHOLOGICAL	7
#define GENUS_HYBRID		8
#define GENUS_SWARM		9

#define ANIMAL_TINY		1
#define ANIMAL_SMALL		2
#define ANIMAL_SMALLMED		3
#define ANIMAL_MEDIUM		4
#define ANIMAL_MANSIZED		5
#define ANIMAL_LARGE		6
#define ANIMAL_VLARGE		7
#define ANIMAL_MONSTEROUS	8

#define EMOTE_TOTAL	0
#define EMOTE_PUBLIC	1
#define EMOTE_SEX	2
#define EMOTE_SOCIAL	3
#define EMOTE_VICTIM	4
#define EMOTE_ANTAG	5
#define EMOTE_PACTANTAG	6
#define EMOTE_DREAMING		7
#define EMOTE_COMBAT		8
#define EMOTE_PAY		9

#define INDUSTRY_NONE		0
#define INDUSTRY_CONSTRUCTION	1
#define INDUSTRY_ONLINE		2
#define INDUSTRY_STORE		3
#define INDUSTRY_RESTAURANT	4
#define INDUSTRY_CLOTHES	5
#define INDUSTRY_BAR		6
#define INDUSTRY_CLUB		7
#define INDUSTRY_SECURITY	8
#define INDUSTRY_INVESTIGATION	9
#define INDUSTRY_RESIDENCE	10
#define INDUSTRY_FINANCIAL	11
#define INDUSTRY_NONPROFIT	12
#define INDUSTRY_RESEARCH	13
#define INDUSTRY_PRIMARY	14
#define INDUSTRY_FACTORY	15
#define INDUSTRY_EDUCATION	16
#define INDUSTRY_HOSPITALITY	17
#define INDUSTRY_MSERVICE	18
#define INDUSTRY_SSERVICE	19
#define INDUSTRY_AGRICULTURE	20
#define INDUSTRY_CAFE		21
#define INDUSTRY_TAKEOUT	22
#define INDUSTRY_TRANSPORT	23
#define INDUSTRY_SEX		24
#define INDUSTRY_LEGAL		25
#define INDUSTRY_ARTIST		26
#define INDUSTRY_TECHNOLOGY	27
#define INDUSTRY_JOURNALISM	28
#define INDUSTRY_GAMES		29
#define INDUSTRY_HEAD		30
#define INDUSTRY_BEAUTY		31
#define MAX_INDUSTRY		32

#define JOB_MIGRANT	0
#define JOB_PARTCOMMUTE	1
#define JOB_FULLCOMMUTE	2
#define JOB_PARTEMPLOY	3
#define JOB_FULLEMPLOY	4
#define JOB_PARTSTUDENT	5
#define JOB_FULLSTUDENT	6
#define JOB_COVER	7
#define JOB_UNEMPLOYED	8
#define JOB_CLINIC	9
#define JOB_COLLEGE	10
#define JOB_EMPLOYEE	11

#define PROCESS_DRAINPC		1
#define PROCESS_DRAINABOM	2
#define PROCESS_TREATING	3
#define PROCESS_RESEARCH	4
#define PROCESS_EXPERIMENT	5
#define PROCESS_LOOKINGUP	6
#define PROCESS_RITUAL		7
#define PROCESS_REPAIR		8
#define PROCESS_DEEPDIG		9
#define PROCESS_SHALLOWDIG	10
#define PROCESS_FILLIN		11
#define PROCESS_CLEARING	12
#define PROCESS_ROAD		13
#define PROCESS_PAVING		14
#define PROCESS_DRAWBLOOD	15
#define PROCESS_EXTERMINATE	16
#define PROCESS_SEARCH		17
#define PROCESS_EXORCISE	18
#define PROCESS_TRAVEL_PREP	19
#define PROCESS_BUTCHER		20
#define PROCESS_RELAY		21
#define PROCESS_SHRINE		22
#define PROCESS_NOSHRINE	23
#define PROCESS_FISH		24


#define BATTLE_FOREST	0
#define BATTLE_FIELD	1
#define BATTLE_DESERT	2
#define BATTLE_TOWN	3
#define BATTLE_CITY	4
#define BATTLE_MOUNTAINS 5
#define BATTLE_WAREHOUSE 6
#define BATTLE_CAVE	7
#define BATTLE_VILLAGE  8
#define BATTLE_TUNDRA	9
#define BATTLE_LAKE	10

#define BATTLE_DEFENDER	1
#define BATTLE_ATTACKER	2


#define SURVEY_FUN	0
#define SURVEY_REAL 	1
#define SURVEY_WRITE	2
#define SURVEY_CREATE	3
#define SURVEY_ANTAG	4
#define SURVEY_SIDE		5
#define SURVEY_BACK		6
#define SURVEY_COMEDY	7
#define SURVEY_VICTIM	8

#define INFLUENCE_SCHEME 	1
#define INFLUENCE_FACTION	2
#define INFLUENCE_SUPER		3

#define MISSION_CONTROL		0
#define MISSION_INSTIGATE	1
#define MISSION_DIPLOMACY	2
#define MISSION_LIAISE		3

#define EMOTE_EMOTE		1
#define EMOTE_SUBTLE		2
#define EMOTE_PRIVATE		3
#define EMOTE_ILLUSION		4
#define EMOTE_ANNOUNCE		5
#define EMOTE_INTERNAL		6
#define EMOTE_EMIT		7
#define EMOTE_ATTEMPT		8

#define ITEM_NEUTRALCOLLAR  45029
#define ITEM_NEEDLEPOTION 45030
#define ITEM_PEPPER     46000  
#define ITEM_BANDAGE    46001 
#define ITEM_TASER      46002 
#define ITEM_TASERDART  46014 
#define ITEM_TRANQGUN   46015
#define ITEM_TRANQDART  46016
#define ITEM_CALTROPS   46017
#define ITEM_LANDMINE   46018
#define ITEM_BOLA       46019
#define ITEM_GASMASK    46020
#define ITEM_SMOKEGRENADE 46021
#define ITEM_TEARGRENADE 46022
#define ITEM_FRAGGRENADE 46023
#define ITEM_NATURALIZER 45027
#define ITEM_NEUTRALGRENADE 46028
#define ITEM_COMPASS	74
#define ITEM_BLOOD	91

#define ENCOUNTER_ONE	1
#define ENCOUNTER_TWO	2
#define ENCOUNTER_THREE	3
#define ENCOUNTER_FOUR	4
#define ENCOUNTER_FIVE	5
#define ENCOUNTER_SIX	6
#define ENCOUNTER_SEVEN	7
#define ENCOUNTER_EIGHT	8
#define ENCOUNTER_NINE	9
#define ENCOUNTER_TEN	10
#define ENCOUNTER_ELEVEN	11
#define ENCOUNTER_TWELVE	12
#define ENCOUNTER_THIRTEEN	13
#define ENCOUNTER_FOURTEEN	14
#define ENCOUNTER_FIFTEEN	15
#define ENCOUNTER_SIXTEEN	16
#define ENCOUNTER_SEVENTEEN	17
#define ENCOUNTER_EIGHTEEN	18
#define ENCOUNTER_NINETEEN	19
#define ENCOUNTER_TWENTY	20
#define ENCOUNTER_TWENTYONE	21
#define ENCOUNTER_TWENTYTWO	22
#define ENCOUNTER_TWENTYTHREE	23
#define ENCOUNTER_TWENTYFOUR	24
#define ENCOUNTER_TWENTYFIVE	25
#define ENCOUNTER_TWENTYSIX	26
#define ENCOUNTER_TWENTYSEVEN	27
#define ENCOUNTER_TWENTYEIGHT	28
#define ENCOUNTER_TWENTYNINE	29
#define ENCOUNTER_THIRTY	30
#define ENCOUNTER_THIRTYONE	31
#define ENCOUNTER_THIRTYTWO	32
#define ENCOUNTER_THIRTYTHREE	33
#define ENCOUNTER_THIRTYFOUR	34
#define ENCOUNTER_THIRTYFIVE		35
#define ENCOUNTER_THIRTYSIX		36
#define ENCOUNTER_THIRTYSEVEN	37
#define ENCOUNTER_THIRTYEIGHT	38
#define ENCOUNTER_THIRTYNINE	39
#define ENCOUNTER_FORTY			40
#define ENCOUNTER_FORTYONE	41
#define ENCOUNTER_FORTYTWO	42
#define ENCOUNTER_FORTYTHREE	43
#define ENCOUNTER_FORTYFOUR	44
#define ENCOUNTER_FORTYFIVE		45
#define ENCOUNTER_FORTYSIX		46
#define ENCOUNTER_FORTYSEVEN	47
#define ENCOUNTER_FORTYEIGHT	48
#define ENCOUNTER_FORTYNINE		49
#define ENCOUNTER_FIFTY		50
#define ENCOUNTER_FIFTYONE	51
#define ENCOUNTER_FIFTYTWO	52
#define ENCOUNTER_MAX		52

#define ENCOUNTER_PENDING	1
#define ENCOUNTER_REJECTED	2
#define ENCOUNTER_ONGOING	3
#define ENCOUNTER_PENDINGALLY	4
#define ENCOUNTER_SELECTING 5

#define OPERATION_INTERCEPT	0
#define OPERATION_EXTRACT	1
#define OPERATION_CAPTURE	2
#define OPERATION_MULTIPLE	3

#define ADVERSARY_MODERN	0
#define ADVERSARY_ARCHAIC	1

#define POI_EXTRACT		1
#define POI_CAPTURE		2

#define WORLD_EARTH	0
#define WORLD_OTHER	1
#define WORLD_WILDS	2
#define WORLD_GODREALM	3
#define WORLD_HELL	4
#define WORLD_ELSEWHERE	5
#define WORLD_DREAM	6

#define COMPETE_OPEN		0
#define COMPETE_RESTRICT	1
#define COMPETE_CLOSED		2

#define REPLY_PLAYER		1
#define REPLY_CHARACTER		2
#define REPLY_ACCOUNT		3
#define REPLY_DREAM		4

#define HAVEN_TOWN_VNUM		13
#define HAVEN_OCEAN_VNUM	15
#define INNER_NORTH_FOREST	16
#define INNER_SOUTH_FOREST	17
#define INNER_WEST_FOREST	18
#define OUTER_NORTH_FOREST	19
#define OUTER_SOUTH_FOREST	20
#define OUTER_WEST_FOREST	21
#define OTHER_FOREST_VNUM	22
#define WILDS_FOREST_VNUM	23
#define GODREALM_FOREST_VNUM	24
#define HELL_FOREST_VNUM	25


#define LIMBO_ROOM_VNUM         1
#define TRAVEL_ROOM_VNUM        19000
#define BROWN_ROOM_VNUM         31000
#define BOSTON_ROOM_VNUM        32000
#define AMERICA_ROOM_VNUM       33000
#define MEXICO_ROOM_VNUM        34000
#define CANADA_ROOM_VNUM        35000
#define SAMERICA_ROOM_VNUM      36000
#define EUROPE_ROOM_VNUM        37000
#define AFRICA_ROOM_VNUM        38000
#define ASIA_ROOM_VNUM          39000
#define AUSTRALIA_ROOM_VNUM     40000
#define MIST_ROOM_VNUM          41000
#define HAVEN_TOWN_ROOM_VNUM    1000
#define HAVEN_SHOP_ROOM_VNUM    45000
#define HAVEN_OCEAN_ROOM_VNUM   50000
#define HAVEN_NFOREST_ROOM_VNUM 90000
#define HAVEN_SFOREST_ROOM_VNUM 110000
#define HAVEN_WFORES_ROOM_VNUM  130000
#define HAVEN_ONFOREST_ROOM_VNUM 150000
#define HAVEN_OSFOREST_ROOM_VNUM 190000
#define HAVEN_OWFOREST_ROOM_VNUM 230000
#define OTHER_FOREST_ROOM_VNUM  300000
#define WILDS_FOREST_ROOM_VNUM  320000
#define GOD_FOREST_ROOM_VNUM    340000
#define HELL_FORES_ROOM_VNUM    360000
#define DREAM_ROOM_VNUM         390000
#define BATTLE_ROOM_VNUM        380000



#define MODIFIER_ADDICTION	1
#define MODIFIER_INJURY		2
#define MODIFIER_ILLNESS	3
#define MODIFIER_CRIPPLED	4
#define MODIFIER_DISFIGURED	5
#define MODIFIER_CURSED		6
#define MODIFIER_INSANE		7
#define MODIFIER_CATATONIC	8
#define MODIFIER_RELIC		9
#define MODIFIER_PACT		10
#define MODIFIER_CHEMICAL	11
#define MODIFIER_TEMPLE		12
#define MODIFIER_FLESHFORMED	13
#define MODIFIER_UNLIVING	14
#define MODIFIER_LUNACY		15
#define MODIFIER_DRINKING	16
#define MODIFIER_NEW		17
#define MODIFIER_STRONG		18
#define MAX_MODIFIER		19

#define MAX_PATHS               80

#define ROOM_INDEX_GENESIS	40

#define LOOK_LOOK		1
#define LOOK_GLANCE		2

#define CONTACT_POLICE		0
#define CONTACT_MILITARY	1
#define CONTACT_GOVERNMENT	2
#define CONTACT_MEDIA		3
#define CONTACT_CRIMINAL	4
#define CONTACT_OCCULT		5
#define CONTACT_HOMELESS	6
#define CONTACT_HAND		7
#define CONTACT_ORDER		8
#define CONTACT_TEMPLE		9
#define CONTACT_SUBFACTION	10
#define MAX_CONTACTS		11

#define CJOB_MUSCLE		1
#define CJOB_REPUTATION		2
#define CJOB_SCHEMES		3
#define CJOB_INFLUENCE		4
#define CJOB_CAPITAL		5
#define CJOB_POLITICS		6
#define CJOB_PLANNING		7
#define CJOB_LABOR		8


#define BASE_DISC_COST          200
#define BASE_STAT_COST          10000


#define AMMO_NONE	0
#define AMMO_AUGMENTED	1
#define AMMO_SILVER	2
#define AMMO_GOLD	3
#define AMMO_WOOD	4
#define AMMO_INCENDIARY	5
#define AMMO_IRON	6
#define AMMO_BONE	7
#define AMMO_QUARTZ	8
#define AMMO_OBSIDIAN	9
#define AMMO_RADIOACTIVE 10
#define AMMO_ARMOR	11
#define MAX_AMMO	12

#define AMMO_PARALYTIC  20
#define AMMO_POISON	21

#define RITUAL_NONE	0
#define RITUAL_IMPRINT	1
#define RITUAL_SILENCE	2
#define RITUAL_SHADOWCLOAK	3
#define RITUAL_CLEANSE	4
#define RITUAL_DREAM	5
#define RITUAL_SANCTUARY	6
#define RITUAL_SCRY	7
#define RITUAL_MUTE	8
#define RITUAL_CORRUPT	9
#define RITUAL_LUNACY	10
#define RITUAL_DEAFEN	11
#define RITUAL_LURE	12
#define RITUAL_MADNESS	13
#define RITUAL_PERSECUTION	14
#define RITUAL_ENFEEBLE	16
#define RITUAL_HAUNT	17
#define RITUAL_PROTECT	18
#define RITUAL_RAISEWARD	19
#define RITUAL_MIST	20
#define RITUAL_BANISH	21
#define RITUAL_BINDING	22
#define RITUAL_UNBINDING 23
#define RITUAL_MINDWARD 24
#define RITUAL_DECAY	25
#define RITUAL_ILLNESS	26
#define RITUAL_CLOAKING 27
#define RITUAL_SACRIFICE 28
#define RITUAL_AGETHEFT	30
#define RITUAL_LOWERWARD 31
#define RITUAL_TECHHEX	32
#define RITUAL_SLEEPWALK 33
#define RITUAL_SHROUDSHIELD 34
#define RITUAL_SHIELDBREAK 35
#define RITUAL_ARTHRITIS 36
#define RITUAL_FREEZETIME 37
#define RITUAL_TIMESHIFT 38
#define RITUAL_FREEZETEMPERATURE 39
#define RITUAL_TEMPERATURESHIFT 40
#define RITUAL_LOCALIZEDWEATHER 41
#define RITUAL_NATURALIZETIME 42
#define RITUAL_NATURALIZEWEATHER 43
#define RITUAL_PURIFY	44
#define RITUAL_DREAMBELIEF 45
#define RITUAL_DREAMIDENTITY 46
#define RITUAL_TRACE	47
#define RITUAL_SEXCHANGE 48
#define RITUAL_DREAMSNARE 49
#define RITUAL_IMPRINTBODY 50
#define RITUAL_SUMMONING 51
#define RITUAL_ENCROACHMENT 52
#define RITUAL_SHADOWALK 53
#define RITUAL_DREADWATCHER 54
#define RITUAL_PREDATE	55

#define DISTANCE_NEAR	0
#define DISTANCE_MEDIUM 1
#define DISTANCE_FAR	2

#define VOLUME_NORMAL   0
#define VOLUME_LOUD	1
#define VOLUME_YELLING	2
#define VOLUME_SCREAMING 3
#define VOLUME_HISCREAMING 4
#define VOLUME_QUIET	5

#define PROPERTY_OUTERFOREST 8

#define CAPTURE_TORMENT		1
#define CAPTURE_EAT		2
#define CAPTURE_KIDNAP		3
#define CAPTURE_EGGS		4
#define CAPTURE_TURN		5                                    

#define IMPRINT_INSTRUCTION	1
#define IMPRINT_COMPULSION	2
#define IMPRINT_BODYINSTRUCTION	3
#define IMPRINT_TRIGGEREDINSTRUCTION	4
#define IMPRINT_BODYCOMPULSION	5
#define IMPRINT_TRIGGEREDCOMPULSION 6
#define IMPRINT_LOCKEDCOMPULSION 7
#define IMPRINT_INFLUENCE	8
#define IMPRINT_CURSED		9
#define IMPRINT_DRUGS		10
#define IMPRINT_LOCKEDBODYCOMPULSION 11
#define IMPRINT_TRIGGEREDBODYINSTRUCTION 12
#define IMPRINT_TRIGGEREDBODYCOMPULSION 13
#define IMPRINT_PERSUADE		14
#define IMPRINT_BCURSED			15

#define HYPNO_LUST 1
#define HYPNO_GENEROSITY 2
#define HYPNO_LOGIC 3
#define HYPNO_GUILT 4
#define HYPNO_ANGER 5
#define HYPNO_PEERPRESSURE 6
#define HYPNO_FEAR 7
#define HYPNO_GRATITUDE 8
#define HYPNO_CURIOUSITY 9
#define HYPNO_JEALOUSY 10
#define HYPNO_PITY 11

#define MAX_NEWCHAR 4

#define CAR_HORSE       8

#define GUEST_PLOT	1
#define GUEST_STAFF	2
#define GUEST_HENCHMAN	3
#define GUEST_ENEMY	4
#define GUEST_MONSTER	5
#define GUEST_NIGHTMARE 6
#define GUEST_CARDINAL 7
#define GUEST_OPERATIVE 8

#define SPIRIT_MALEFICARUM 1
#define SPIRIT_JUSTICARUM 2

#define PSYCHIC_FEAR		1
#define PSYCHIC_ANGER		2
#define PSYCHIC_SEX		3
#define PSYCHIC_LUST		3
#define PSYCHIC_AMBIANT		4

#define ACCOUNT_SPAMMER  0
#define ACCOUNT_NOSR	1
#define ACCOUNT_NOTIFY  2
#define ACCOUNT_WILDSR  3
#define ACCOUNT_DIDDIPLO 4
#define ACCOUNT_DIDARCANE 5
#define ACCOUNT_DIDHUNT 6
#define ACCOUNT_DIDWAR 7
#define ACCOUNT_DIDARCANEWAR 8
#define ACCOUNT_NOTELL 9
#define ACCOUNT_NOFEEDBACK 10
#define ACCOUNT_NOCHANNELS 11
#define ACCOUNT_NOTITLE 12
#define ACCOUNT_NOINTRO 13
#define ACCOUNT_NONOTE 14
#define ACCOUNT_NOPRAY 15
#define ACCOUNT_NOFEEL 16
#define ACCOUNT_NIGHTMARE 17
#define ACCOUNT_GRIEFER 18
#define ACCOUNT_NOOFFWORLD 19
#define ACCOUNT_FREEPOWER 20
#define ACCOUNT_NOKILL 21
#define ACCOUNT_SHH 22
#define ACCOUNT_SHADOWBAN 23
#define ACCOUNT_DIDSPIRIT 24

#define STARTING_EXP	150000
#define STARTING_RPEXP	150000

#define OTHER_EXITS	10
#define WILDS_EXITS	14
#define GODREALM_EXITS	19


#define PATROL_HUNTHABIT 0
#define PATROL_WARHABIT 1
#define PATROL_RECKLESSHABIT 2
#define PATROL_ARCANEHABIT 3
#define PATROL_DIPLOMATICHABIT 4
#define PATROL_PREDATORHABIT 5
#define PATROL_ARCANEWARHABIT 6
#define PATROL_SPIRITHABIT 7

#define PATROL_PATROL	1
#define PATROL_HUNT	2
#define PATROL_APPROACHINGHUNT 3
#define PATROL_HUNTING	4
#define PATROL_HUNTMOVING 5
#define PATROL_WAR 6

#define PATROL_WARMOVINGATTACK 7
#define PATROL_WARMOVINGDEFEND 8
#define PATROL_ATTACKSEARCHING 9
#define PATROL_ATTACKWAITING 10
#define PATROL_ATTACKASSISTING 11
#define PATROL_DEFENDASSISTING 12
#define PATROL_DEFENDHIDING 13
#define PATROL_WAGINGWAR 14
#define PATROL_ARCANE	15
#define PATROL_ARTIFACT 16
#define PATROL_GHOST 17
#define PATROL_DECURSING 18
#define PATROL_ASSISTING_DECURSING 19
#define PATROL_SENSING_ARTIFACT 20
#define PATROL_HOLDING_ARTIFACT 21
#define PATROL_SENSING_GHOST	22
#define PATROL_GHOST_FOCUS	23
#define PATROL_BANISHING	24
#define PATROL_ASSISTING_BANISHING 25
#define PATROL_LEADING_ASSAULT 26
#define PATROL_UNDER_ASSAULT 27
#define PATROL_ASSISTING_ASSAULT 28
#define PATROL_ASSISTING_UNDER_ASSAULT 29
#define PATROL_ARCANEWAR 30 
#define PATROL_DIPLOMATIC 31
#define PATROL_KIDNAP	32
#define PATROL_KIDNAPPED 33
#define PATROL_BIDDING 34
#define PATROL_COLLECTING 35
#define PATROL_BARGAIN 36
#define PATROL_BARGAINING 37
#define PATROL_BRIBE 38
#define PATROL_BRIBING 39
#define PATROL_RECKLESS 40
#define PATROL_PREYING 41
#define PATROL_PREY 42
#define PATROL_FLEEING 43
#define PATROL_CHASING 44
#define PATROL_HIDING 45
#define PATROL_SEARCHING 46
#define PATROL_GRABBING 47
#define PATROL_GRABBED 48
#define PATROL_STALKING 49
#define PATROL_PREDATOR 50
#define PATROL_HUNTFIGHTING 51


#define VILLAIN_OPTION_WAITING 1
#define VILLAIN_OPTION_CHOOSING 2
#define VILLAIN_OPTION_EMOTING 3
#define VILLAIN_OPTION_CHASE   2
#define VILLAIN_OPTION_ABANDON 3
#define VILLAIN_OPTION_GRAB    4
#define VILLAIN_OPTION_MISS    5
#define VILLAIN_OPTION_CHOOSING_GRAB 6
#define VILLAIN_OPTION_DRAWOUT 7
#define VILLAIN_OPTION_AGGRESSIVE 8


#define PREY_OPTION_WAITING  1
#define PREY_OPTION_CHOOSING 2
#define PREY_OPTION_EMOTING  3
#define PREY_OPTION_INJURE   4
#define PREY_OPTION_DISTANCE 5
#define PREY_OPTION_TRAIL    6
#define PREY_OPTION_TUSSLE   7
#define PREY_OPTION_BLOCKADE 8
#define PREY_OPTION_PUBLIC   9
#define PREY_OPTION_CHARM    10
#define PREY_OPTION_PANDER   11
#define PREY_OPTION_GIVEUP   12
#define PREY_OPTION_DASH     13
#define PREY_OPTION_DRAWOUT  14


#define ROOM_MEETING_EAST 1372
#define ROOM_MEETING_WEST 18440
#define ROOM_PRISON_EAST 6351
#define ROOM_PRISON_WEST 6355
#define ROOM_MEDICAL_CARPARK 16026
#define ROOM_POLICE_CARPARK 3514

#define SF_MAX          59

#define MESSAGE_CHARACTER 0
#define MESSAGE_LOYALTY 1
#define MESSAGE_APPEARANCE 2
#define TRACK_LOYALTY_PRAISE_AMOUNT 0
#define TRACK_LOYALTY_DISS_AMOUNT 1
#define TRACK_CHARACTER_PRAISE_AMOUNT 2
#define TRACK_CHARACTER_DISS_AMOUNT 3
#define TRACK_APPEARANCE_PRAISE_AMOUNT 4
#define TRACK_APPEARANCE_DISS_AMOUNT 5
#define TRACK_RUMORS 6
#define TRACK_PATROL_HUNTING 7
#define TRACK_PATROL_WARFARE 8
#define TRACK_PATROL_ARCANE 9
#define TRACK_PATROL_DIPLOMATIC 10
#define TRACK_PATROL_PREDATORY 11
#define TRACK_PATROL_PREY 12
#define TRACK_CONTRIBUTED 13
#define TRACK_OPERATIONS_CREATED 14
#define TRACK_OPERATIONS_ATTENDED 15
#define TRACK_ROADS_BUILT 16
#define TRACK_SCHEMES_LAUNCHED 17
#define TRACK_SCHEMES_THWARTED 18
#define TRACK_TREES_CHOPPED 19
#define TRACK_ALCH_CONTRIBUTED 20
#define TRACK_EVENTS_HELD 21
#define TRACK_EVENT_MAXPOP 22
#define TRACK_EVENT_HOTNESS 23
#define TRACK_EVENTS_ATTENDED 24
#define TRACK_SEX 25
#define TRACK_ADVENTURES 26
#define TRACK_BIGMONSTER 27
#define TRACK_BANK 28
#define TRACK_VILLAIN 29


#define GOAL_INCITE 1
#define GOAL_CALM 2
#define GOAL_PROVOKE 3
#define GOAL_PLANT 4
#define GOAL_SETUP 5
#define GOAL_FOOTHOLD 6
#define GOAL_RAZE 7
#define GOAL_LOOT 8
#define GOAL_SUPPRESS 9
#define GOAL_KIDNAP 10
#define GOAL_CONTROL 11
#define GOAL_ASSASSINATE 12
#define GOAL_RESCUE 13
#define GOAL_UPROOT 14
#define GOAL_SABOUTAGE 15
#define GOAL_PSYCHIC 16

#define TEDIT_NUMBER 0
#define TEDIT_TIMER 1
#define TEDIT_NEWS 2
#define TEDIT_TIMELINE 3
#define TEDIT_SETUP 4
#define TEDIT_PLANT 5
#define TEDIT_ESTABLISH 6

#define PROPWEATHER_CLEAR	1
#define PROPWEATHER_CLOUDY	2
#define PROPWEATHER_RAIN	3
#define PROPWEATHER_SNOW	4
#define PROPWEATHER_HAIL	5

#define ECHO_GRAZE 1
#define ECHO_MISS 2
#define ECHO_DODGE 3
#define ECHO_ARMOR 4
#define ECHO_SHIELD 5
#define ECHO_SUPERTOUGH 6
#define ECHO_SUPERSHIELD 6
#define ECHO_ARMORBRUISE 7
#define ECHO_SHIELDBRUISE 8
#define ECHO_PARRY 9
#define ECHO_SHIELDBLOCK 10
#define ECHO_COVER 11
#define ECHO_PUNCH 12
#define ECHO_GRAPPLE 13
#define ECHO_PUNCHBLOCK 14
#define ECHO_GRAPPLEBLOCK 15
#define ECHO_COVERSTRIKE  16

#define KARMA_ENCOUNTER 1
#define KARMA_ADVENTURE 2
#define KARMA_MYSTERY 3
#define KARMA_AMBIANT 4
#define KARMA_MONSTER 5
#define KARMA_OTHER 6
#define KARMA_SCHEME 7
#define KARMA_DREAM 9

    #define PROPERTY_TOWNHOUSE 1
    #define PROPERTY_TOWNSHOP 2
    #define PROPERTY_UNDERSHOP 3
    #define PROPERTY_OVERFLAT 4
    #define PROPERTY_LONGHOUSE 5
    #define PROPERTY_LONGSHOP 6
    #define PROPERTY_INNERFOREST 7
    #define PROPERTY_OUTERFOREST 8
    #define PROPERTY_TRAILER    9
    #define PROPERTY_SMALLFLAT	10
    #define PROPERTY_MEDFLAT 11
    #define PROPERTY_BIGFLAT 12

    #define PROPLOC_TOWN 1
    #define PROPLOC_INNER 2
    #define PROPLOC_OUTER 3
    #define PROPLOC_WILDS 4
    #define PROPLOC_OTHER 5
    #define PROPLOC_GODREALM 6
    #define PROPLOC_HELL 7
#define GARAGE_GARAGED  1
#define GARAGE_ACTIVE   2
#define GARAGE_LOANED   3
#define GARAGE_REPAIR   4
#define GARAGE_RECOVER  5
#define GARAGE_RECOVERREPAIR    6
#define GARAGE_LOANEDDAMAGED    7


#define TRAINED_SELF	1
#define TRAINED_ITEM	2
#define TRAINED_TAUGHT  3
#define TRAINED_FRIEND	4
#define TRAINED_BLACKMAIL 5
#define TRAINED_MINDCONTROL 6
#define TRAINED_TOOL 7
#define TRAINED_JOBLEGAL 8
#define TRAINED_JOBBLACKMAIL 9
#define TRAINED_JOBMINDCONTROL 10
#define TRAINED_THEFT	11
#define TRAINED_INHERIT 12
#define TRAINED_DEMON	13
#define TRAINED_FAE	14
#define TRAINED_BOOK	15
#define TRAINED_EDUFRAUD 16
#define TRAINED_EDUBRIBE 17
#define TRAINED_EDULEGIT 18
#define TRAINED_NATURAL 19
#define TRAINED_IMPLANT 20
#define TRAINED_TATTOO	100


#define AXES_SUPERNATURAL 5
#define AXES_MATERIAL 1
#define AXES_COMBAT 2
#define AXES_CORRUPT 3
#define AXES_DEMOCRATIC 4
#define AXES_ANARCHY 6
#define AXES_MAX 6

#define AXES_FARLEFT 1
#define AXES_MIDLEFT 2
#define AXES_NEARLEFT 3
#define AXES_NEUTRAL 4
#define AXES_NEARRIGHT 5
#define AXES_MIDRIGHT 6
#define AXES_FARRIGHT 7

#define ALLIANCE_NORMAL 1
#define ALLIANCE_HARDMIDDLE 2
#define ALLIANCE_HARDLEFT 3
#define ALLIANCE_HARDRIGHT 4

#define ALLIANCE_SIDELEFT 1
#define ALLIANCE_SIDEMID 2
#define ALLIANCE_SIDERIGHT 3

#define RESTRICT_MALE	0
#define RESTRICT_FEMALE 1
#define RESTRICT_PRIMARY_ARCANE	2
#define RESTRICT_ANY_ARCANE	3
#define RESTRICT_PRIMARY_COMBAT 4
#define RESTRICT_ANY_COMBAT	5
#define RESTRICT_PRIMARY_PROF	6
#define RESTRICT_ANY_PROF	7
#define RESTRICT_VAMPIRES	8
#define RESTRICT_WEREWOLVES	9
#define RESTRICT_FAEBORN	10
#define RESTRICT_DEMONBORN	11
#define RESTRICT_ANGELBORN	12
#define RESTRICT_DEMIGOD	13
#define RESTRICT_SUPERNATURALS	14
#define RESTRICT_NATURALS	15
#define RESTRICT_NONVIRGINS	16
#define RESTRICT_FRATENIZATION	17
#define RESTRICT_OUTOFWEDLOCK	18
#define RESTRICT_MURDERVAMPS	19
#define RESTRICT_HOMOSEXUAL	20
#define RESTRICT_INHIBITED	21
#define RESTRICT_MAX		21

#define ASYLUM_SELFASSESS	1
#define ASYLUM_SELFCOMMIT	2
#define ASYLUM_DROPOFF		3
#define ASYLUM_DEPUTY_DROPOFF	4
#define ASYLUM_REMOTECOMMIT	5
#define ASYLUM_NEED_APPROVAL	6
#define ASYLUM_WANTED		7
#define ASYLUM_COLLEGECOMMIT	8

#define DISTRICT_ANYWHERE	0
#define DISTRICT_URBAN		1
#define DISTRICT_TOURIST	2
#define DISTRICT_REDLIGHT	3
#define DISTRICT_HISTORIC	4
#define DISTRICT_INSTITUTE	5
#define DISTRICT_WESTHAVEN	6

#define INSTITUTE_EXPERIMENT	1
#define INSTITUTE_TREAT		2
#define INSTITUTE_TRAUMA	3
#define INSTITUTE_TEACH		4


#define MOB_AI_ANTAG_SOLDIER	1

#define COLLEGE_GOTH			1
#define COLLEGE_JOCK		2
#define COLLEGE_OVERACHIEVER 3
#define COLLEGE_PREP		4
#define COLLEGE_BADKID		5
#define COLLEGE_THEATRE		6

#define DECREE_LOCKDOWN 1
#define DECREE_BORDER 2
#define DECREE_BUILDER 3
#define DECREE_SPONSOR 4
#define DECREE_OVERTAX 5


#define ROOM_INDEX_CLINICSTORE	9794
#define ROOM_INDEX_CLINICCOMMIT 9693
#define ROOM_INDEX_DETENTION 4854
#define ROOM_INDEX_SHERIFFCELL 2358
#define ROOM_INDEX_SHERIFFCAGE 9392
#define ROOM_INDEX_EMERGENCYROOM 4274
#define ROOM_INDEX_CLINICDISCHARGE 9794


#define COLLEGE_ROOM_REP_ONE 3864
#define COLLEGE_ROOM_REP_TWO 3865
#define COLLEGE_ROOM_REP_THREE 3870
#define COLLEGE_ROOM_REP_FOUR 3882
#define COLLEGE_ROOM_REP_FIVE 3892
#define COLLEGE_ROOM_REP_SIX 4310
#define COLLEGE_ROOM_PUR_ONE 3230
#define COLLEGE_ROOM_PUR_TWO 3804
#define COLLEGE_ROOM_PUR_THREE 3798
#define COLLEGE_ROOM_PUR_FOUR 3809
#define COLLEGE_ROOM_PUR_FIVE 3800
#define COLLEGE_ROOM_PUR_SIX 3818


#define COLLEGE_ROOM_FOR_ONE 4385
#define COLLEGE_ROOM_FOR_TWO 4386
#define COLLEGE_ROOM_FOR_THREE 4393
#define COLLEGE_ROOM_FOR_FOUR 4396
#define COLLEGE_ROOM_FOR_FIVE 4716
#define COLLEGE_ROOM_FOR_SIX 4719


#define COLLEGE_ROOM_CHAR_ONE 4790
#define COLLEGE_ROOM_CHAR_TWO 4795
#define COLLEGE_ROOM_CHAR_THREE 4798
#define COLLEGE_ROOM_CHAR_FOUR 4804
#define COLLEGE_ROOM_CHAR_FIVE 4823
#define COLLEGE_ROOM_CHAR_SIX 4831

#define HOUSE_CHARITY 1
#define HOUSE_FORBEAR 2
#define HOUSE_REPENT 3
#define HOUSE_PURITY 4

#define FRAT_ALPHA 2
#define FRAT_SIGMA 4
#define SORIT_DELTA 1
#define SORIT_PHI 3

#define ROOM_INS_CELL_ONE 4848
#define ROOM_INS_CELL_TWO 4858
#define ROOM_INS_CELL_THREE 4850
#define ROOM_INS_CELL_FOUR 4862
#define ROOM_INS_CELL_FIVE 4851
#define ROOM_INS_CELL_SIX 4868
#define ROOM_INS_CELL_SEVEN 4854
#define ROOM_INS_CELL_EIGHT 4876



#define WEAKNESS_IS		1
#define WEAKNESS_HAS		2
#define WEAKNESS_MILD		3
#define WEAKNESS_SEVERE		4
#define WEAKNESS_CRITICAL	5
#define WEAKNESS_PRISONER	6
#define WEAKNESS_COMMITTED	7
#define WEAKNESS_DREAMSNARED	8


#define VICTIMIZE_MILDPAIN	1
#define VICTIMIZE_SEVEREPAIN	2
#define VICTIMIZE_BANTER	3
#define VICTIMIZE_RELIEF	4
#define VICTIMIZE_COMPLIMENT	5
#define VICTIMIZE_REWARD	6
#define VICTIMIZE_QUESTION	7
#define VICTIMIZE_REQUESTNOW	8
#define VICTIMIZE_REQUESTLATER	9
#define VICTIMIZE_BLEED		10
#define VICTIMIZE_TEASE		11
#define VICTIMIZE_DEGRADE	12
#define VICTIMIZE_INSULT	13
#define VICTIMIZE_EMBARASS	14
#define VICTIMIZE_HUMILIATE	15
#define VICTIMIZE_DISTURB	16
#define VICTIMIZE_DISCOMFORT	17
#define VICTIMIZE_ASPHYXIATE	18
#define VICTIMIZE_STRAIN	19
#define VICTIMIZE_MAX		19

#define RESPONSE_STOIC		1
#define RESPONSE_SCREAMPAIN	2
#define RESPONSE_SCREAMHELP	3
#define RESPONSE_BELLOWPAIN	4
#define RESPONSE_GRUNT		5
#define RESPONSE_GRITTEETH	6
#define RESPONSE_BEGRELIEF	7
#define RESPONSE_BEGFREE	8
#define RESPONSE_BARGAINRELIEF	9
#define RESPONSE_BARGAINFREE	10
#define RESPONSE_THREATENRELIEF	11
#define RESPONSE_THREATENFREE	12
#define RESPONSE_INSULT		13
#define RESPONSE_STRUGGLE	14
#define RESPONSE_LASHOUT	15
#define RESPONSE_AGREE		16
#define RESPONSE_FAKEAGREE	17
#define RESPONSE_ANSWERTRUE	18
#define RESPONSE_ANSWERFALSE	19
#define RESPONSE_BANTER		20
#define RESPONSE_DISTRACT	21
#define RESPONSE_BLUSH		22
#define RESPONSE_SQUIRM		23
#define RESPONSE_THANK		24
#define RESPONSE_ENJOY		25
#define RESPONSE_FAILHIDDENENJOY	26
#define RESPONSE_HIDDENENJOY	27
#define RESPONSE_PASSOUT	28
#define RESPONSE_RETURNCOMPLIMENT 29
#define RESPONSE_CHARMFREE	30
#define RESPONSE_ARGUE		31
#define RESPONSE_INTEL		32
#define RESPONSE_MAX		32

#define DREAM_DETAIL_SHORT	1
#define DREAM_DETAIL_NAME	2
#define DREAM_DETAIL_FAME	3
#define DREAM_DETAIL_EQ		4
#define DREAM_DETAIL_DESC	5

#define LOGEVENT_ENCOUNTER 1
#define LOGEVENT_PATROL	   2
#define LOGEVENT_THWART	   3
#define LOGEVENT_ADVENTURE 4
#define LOGEVENT_OTHER     5
#define LOGEVENT_MONSTER   6

#define AVAIL_NORMAL	0
#define AVAIL_LOW	1
#define AVAIL_HIGH	2

#define DESTINY_PREY	1
#define DESTINY_PREDATOR 2
#define DESTINY_RIVAL 3
#define DESTINY_SOCIAL 4
#define DESTINY_ADVENTURE 5
#define DESTINY_INVESTIGATION 6
#define DESTINY_WORK 7

#define DESTINY_MAYBE 0
#define DESTINY_YES 1
#define DESTINY_NO 2

#define CLAIM_PENDING 1
#define CLAIM_REJECTING 2
#define CLAIM_ACTIVE 3

#define CRITERIA_CHAR 1
#define CRITERIA_GENDER 2
#define CRITERIA_ORIENT 3
#define CRITERIA_SOCIETY 4
#define CRITERIA_HOURS 5
#define CRITERIA_TIER 6
#define CRITERIA_MORALITY 7
#define CRITERIA_ARCHETYPE 8
#define CRITERIA_FOCUS 9
#define CRITERIA_NATURE 10
#define CRITERIA_INSTITUTE 11
#define CRITERIA_AGE 12

#define DEST_FEAT_BRAND 1
#define DEST_FEAT_HELPLESS 2
#define DEST_FEAT_TURN	3
#define DEST_FEAT_NOWIN 4
#define DEST_FEAT_NOSANC 5
#define DEST_FEAT_STUCK 6
#define DEST_FEAT_PREY 7
#define DEST_FEAT_ARMED 8
#define DEST_FEAT_IMPRINTABLE 9
#define DEST_FEAT_LIMITED 10

#define DEST_REST_UNDERT2 1
#define DEST_REST_UNDERT3 2
#define DEST_REST_UNDERT4 3
#define DEST_REST_UNDERT5 4
#define DEST_REST_ABOVET1 5
#define DEST_REST_ABOVET2 6
#define DEST_REST_ABOVET3 7
#define DEST_REST_ABOVET4 8
#define DEST_REST_ABOVET5 9
#define DEST_REST_NOTALLIES 10
#define DEST_REST_ALLIES 11
#define DEST_REST_PROF 12
#define DEST_REST_SORC 13
#define DEST_REST_FIGHT 14
#define DEST_REST_UNDER30 15
#define DEST_REST_OVER30 16

#define NARRATIVE_POWERFUL 0
#define NARRATIVE_MORAL 1
#define NARRATIVE_HUMAN 2
#define NARRATIVE_BADASS 3
#define NARRATIVE_BRAVE_PHYS 4
#define NARRATIVE_BRAVE_HORROR 5
#define NARRATIVE_LOYAL_CAUSE 6
#define NARRATIVE_LOYAL_PERSON 7
#define NARRATIVE_SELFLESS 8
#define NARRATIVE_ROMANTIC 9
#define NARRATIVE_CLEVER 10
#define NARRATIVE_CUNNING 11
#define NARRATIVE_CHARISMATIC 12
#define NARRATIVE_RUTHLESS 13
#define NARRATIVE_STRONGWILL 14
#define NARRATIVE_COVET 15
#define NARRATIVE_UNIQUECOVET 16
#define NARRATIVE_LEADER 17
#define NARRATIVE_SCARY 18
#define NARRATIVE_INTERESTING 19
#define NARRATIVE_IMPORTANT 20
#define NARRATIVE_DEBT 21
#define NARRATIVE_WORLD_BETTER 22
#define NARRATIVE_WORLD_INTEREST 23
#define NARRATIVE_PERSON_MORAL 24
#define NARRATIVE_PERSON_CORRUPT 25
#define NARRATIVE_CHANGE_PSYCH 26
#define NARRATIVE_CHANGE_CIRCUM 27


#define STATUS_INCOMPLETE 0
#define STATUS_PENDING 1
#define STATUS_APPROVED 2
#define STATUS_TAPPROVED 3
#define STATUS_SPONSORED 4
#define STATUS_LOCKED 5
#define STATUS_CLAIMED 6
#define STATUS_DELETE 7

#define VISIBILITY_ALL 0
#define VISIBILITY_SRS 1
#define VISIBILITY_IMMS 2

#define REWARD_MONEY 1
#define REWARD_INFLUENCE 2
#define REWARD_RESPECT 3
#define REWARD_MAGICAL 4



#define STATUS_INCOMPLETE 0
#define STATUS_PENDING 1
#define STATUS_APPROVED 2
#define STATUS_TAPPROVED 3
#define STATUS_SPONSORED 4
#define STATUS_LOCKED 5
#define STATUS_CLAIMED 6
#define STATUS_DELETE 7

#define VISIBILITY_ALL 0
#define VISIBILITY_SRS 1
#define VISIBILITY_IMMS 2

#define REWARD_MONEY 1
#define REWARD_INFLUENCE 2
#define REWARD_RESPECT 3
#define REWARD_MAGICAL 4



#define CALENDAR_OTHER 0
#define CALENDAR_PARTY 1
#define CALENDAR_SOCIAL 2
#define CALENDAR_PERFORMANCE 3
#define CALENDAR_SOCIETY 4
#define CALENDAR_PLOT 5
#define CALENDAR_EDUCATIONAL 6


#define FEXP_RP 1
#define FEXP_MONSTER 2
#define FEXP_ATTACK 3
#define FEXP_PATROL 4


#define SUMMARY_CONFLICT_ATT 1
#define SUMMARY_CONFLICT_DEF 2
#define SUMMARY_DEFEATING_INFIGHT 3
#define SUMMARY_DEFEATING_ATT 4
#define SUMMARY_DEFEATING_DEF 5

#define SUMMARY_DEFEATED_INFIGHT 6
#define SUMMARY_DEFEATED_ATT 7
#define SUMMARY_DEFEATED_DEF 8

#define SUMMARY_VICTIM 9
#define SUMMARY_VICTIMIZER 10
#define SUMMARY_WRAPUP 11
#define SUMMARY_OTHER 12
#define SUMMARY_PENDING 13

#define SUM_STAGE_INTRO 1
#define SUM_STAGE_CONTENT 2
#define SUM_STAGE_CONCLUSION 3
#define SUM_STAGE_WAITING 4
#define SUM_STAGE_PENDING_YOU 5
#define SUM_STAGE_PENDING_THEM 6
#define SUM_STAGE_CHANCE 7




// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif


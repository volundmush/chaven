#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "db.h"
#include "global.h"

#if defined(__cplusplus)
extern "C" {
#endif


  /*
* Local functions.
*/
  AREA_DATA *get_area_data args((int vnum));
  void sredit args((CHAR_DATA * ch, char *argument));
  void subraceedit args((CHAR_DATA * ch, char *argument));
  void creation args((CHAR_DATA * ch, char *argument));
  void newturn args((CHAR_DATA * ch, char *argument));
  /*
*/

  void skedit args((DESCRIPTOR_DATA * d, char *argument));
  void do_function args((CHAR_DATA * ch, DO_FUN *do_fun, char *argument));

  /* Executed from comm.c.  Minimizes compiling when changes are made. */
  bool run_olc_editor(DESCRIPTOR_DATA *d) {
    switch (d->editor) {
    case ED_AREA:
      aedit(d->character, d->incomm);
      break;
    case ED_ROOM:
      redit(d->character, d->incomm);
      break;
    case ED_OBJECT:
      oedit(d->character, d->incomm);
      break;
    case ED_MOBILE:
      medit(d->character, d->incomm);
      break;
    case ED_MPCODE:
      mpedit(d->character, d->incomm);
      break;
    case ED_OPCODE:
      opedit(d->character, d->incomm);
      break;
    case ED_RPCODE:
      rpedit(d->character, d->incomm);
      break;
    case ED_HELP:
      hedit(d->character, d->incomm);
      break;
    case ED_SUBRACE:
      break;
    case ED_CREATION:
      creation(d->character, d->incomm);
      break;
    default:
      return FALSE;
    }
    return TRUE;
  }

  char *olc_ed_name(CHAR_DATA *ch) {
    static char buf[10];

    buf[0] = '\0';
    switch (ch->desc->editor) {
    case ED_AREA:
      sprintf(buf, "AEdit");
      break;
    case ED_ROOM:
      sprintf(buf, "REdit");
      break;
    case ED_OBJECT:
      sprintf(buf, "OEdit");
      break;
    case ED_MOBILE:
      sprintf(buf, "MEdit");
      break;
    case ED_MPCODE:
      sprintf(buf, "MPEdit");
      break;
    case ED_OPCODE:
      sprintf(buf, "OPEdit");
      break;
    case ED_RPCODE:
      sprintf(buf, "RPEdit");
      break;
    case ED_HELP:
      sprintf(buf, "HEdit");
      break;
    case ED_GUILD:
      sprintf(buf, "GuildEdit");
      break;
    case ED_SUBRACE:
      sprintf(buf, "SubraceEdit");
      break;
    case ED_CREATION:
      sprintf(buf, "Creation");
      break;
    case ED_VESSEL:
      sprintf(buf, "Vessel");
      break;
    case ED_PORT:
      sprintf(buf, "Port");
      break;
    case ED_GROUP:
      sprintf(buf, "Group");
      break;
    default:
      sprintf(buf, " ");
      break;
    }
    return buf;
  }

  char *olc_ed_vnum(CHAR_DATA *ch) {
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMob;
    PROG_CODE *pMprog;
    PROG_CODE *pOprog;
    PROG_CODE *pRprog;
    HELP_DATA *pHelp;
    static char buf[MIL];

    buf[0] = '\0';
    switch (ch->desc->editor) {
    case ED_AREA:
      pArea = (AREA_DATA *)ch->desc->pEdit;
      sprintf(buf, "%d", pArea ? pArea->vnum : 0);
      break;
    case ED_ROOM:
      pRoom = ch->in_room;
      sprintf(buf, "%d", pRoom ? pRoom->vnum : 0);
      break;
    case ED_OBJECT:
      pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
      sprintf(buf, "%d", pObj ? pObj->vnum : 0);
      break;
    case ED_MOBILE:
      pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
      sprintf(buf, "%d", pMob ? pMob->vnum : 0);
      break;
    case ED_MPCODE:
      pMprog = (PROG_CODE *)ch->desc->pEdit;
      sprintf(buf, "%d", pMprog ? pMprog->vnum : 0);
      break;
    case ED_OPCODE:
      pOprog = (PROG_CODE *)ch->desc->pEdit;
      sprintf(buf, "%d", pOprog ? pOprog->vnum : 0);
      break;
    case ED_RPCODE:
      pRprog = (PROG_CODE *)ch->desc->pEdit;
      sprintf(buf, "%d", pRprog ? pRprog->vnum : 0);
      break;
    case ED_HELP:
      pHelp = (HELP_DATA *)ch->desc->pEdit;
      sprintf(buf, "%s", pHelp ? pHelp->keyword : "");
      break;
    default:
      sprintf(buf, " ");
      break;
    }

    return buf;
  }

  /*****************************************************************************
Name:		show_olc_cmds
Purpose:	Format up the commands from given table.
Called by:	show_commands(olc_act.c).
****************************************************************************/
  void show_olc_cmds(CHAR_DATA *ch, const struct olc_cmd_type *olc_table) {
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int cmd;
    int col;

    buf1[0] = '\0';
    col = 0;
    for (cmd = 0; olc_table[cmd].name != NULL; cmd++) {
      sprintf(buf, "%-15.15s", olc_table[cmd].name);
      strcat(buf1, buf);
      if (++col % 5 == 0)
      strcat(buf1, "\n\r");
    }

    if (col % 5 != 0)
    strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
  }

  /*****************************************************************************
Name:		show_commands
Purpose:	Display all olc commands.
Called by:	olc interpreters.
****************************************************************************/
  bool show_commands(CHAR_DATA *ch, char *argument) {
    switch (ch->desc->editor) {
    case ED_AREA:
      show_olc_cmds(ch, aedit_table);
      break;
    case ED_ROOM:
      show_olc_cmds(ch, redit_table);
      break;
    case ED_OBJECT:
      show_olc_cmds(ch, oedit_table);
      break;
    case ED_MOBILE:
      show_olc_cmds(ch, medit_table);
      break;
    case ED_MPCODE:
      show_olc_cmds(ch, mpedit_table);
      break;
    case ED_OPCODE:
      show_olc_cmds(ch, opedit_table);
      break;
    case ED_RPCODE:
      show_olc_cmds(ch, rpedit_table);
      break;
    case ED_HELP:
      show_olc_cmds(ch, hedit_table);
      break;
    }
    return FALSE;
  }

  /*****************************************************************************
*                           Interpreter Tables.                             *
*****************************************************************************/
  const struct olc_cmd_type aedit_table[] = {
    /*  {   command		function	}, */

    {"age", aedit_age}, {"world", aedit_world}, {"range", aedit_range}, {"commands", show_commands}, {"create", aedit_create}, {"delete", aedit_delete}, {"filename", aedit_file}, {"name", aedit_name}, /*  {   "recall",	aedit_recall	},   ROM OLC */
    {"reset", aedit_reset}, {"show", aedit_show}, {"vnum", aedit_vnum}, {"lvnum", aedit_lvnum}, {"uvnum", aedit_uvnum}, {"?", show_help}, {"version", show_version}, {"location", aedit_location}, 
    {
      NULL, 0, }};

  const struct olc_cmd_type groupedit_table[] = {
    /*  {  	command,	function	}, */

    {"create", groupedit_create}, {"member", groupedit_member}, {NULL, 0}};

  const struct olc_cmd_type redit_table[] = {
    /*  {   command		function	}, */

    {"commands", show_commands}, {"create", redit_create}, {"desc", redit_desc}, {"ed", redit_ed}, {"place", redit_place}, {"format", redit_format}, {"name", redit_name}, {"subarea", redit_subarea}, // Discordance
    {"show", redit_show}, {"level", redit_level}, {"shroud", redit_shroud}, {"mana", redit_mana}, {"clan", redit_clan}, {"copy", redit_copy}, 
    {"north", redit_north}, {"south", redit_south}, {"east", redit_east}, {"west", redit_west}, {"up", redit_up}, {"down", redit_down}, {"northeast", redit_northeast}, {"northwest", redit_northwest}, {"southeast", redit_southeast}, {"southwest", redit_southwest}, 
    /* New reset commands. */
    {"mreset", redit_mreset}, {"oreset", redit_oreset}, {"mlist", redit_mlist}, {"rlist", redit_rlist}, {"olist", redit_olist}, {"mshow", redit_mshow}, {"oshow", redit_oshow}, {"owner", redit_owner}, {"room", redit_room}, {"sector", redit_sector}, {"xy", redit_xy}, {"coordinates", redit_coordinates}, {"size", redit_size}, {"entry", redit_entry}, {"feature", redit_feature}, {"sectorset", redit_sectorset}, 
    {"?", show_help}, {"version", show_version}, 
    {"addrprog", redit_addrprog}, {"delrprog", redit_delrprog}, 
    {
      NULL, 0, }};

  const struct olc_cmd_type oedit_table[] = {
    /*  {   command		function	}, */

    {"addaffect", oedit_addaffect}, {"addapply", oedit_addapply}, {"commands", show_commands}, {"cost", oedit_cost}, {"create", oedit_create}, {"delete", oedit_delete}, {"delaffect", oedit_delaffect}, {"ed", oedit_ed}, {"long", oedit_long}, {"name", oedit_name}, {"short", oedit_short}, {"wearstring", oedit_wearstring}, {"show", oedit_show}, {"spec", oedit_spec}, {"v0", oedit_value0}, {"v1", oedit_value1}, {"v2", oedit_value2}, {"v3", oedit_value3}, {"v4", oedit_value4}, /* ROM */
    {"v5", oedit_value5}, /* Patterns */
    {"size", oedit_size}, {"copy", oedit_copy}, 
    {"extra", oedit_extra},         /* ROM */
    {"rottimer", oedit_rottimer},   /* Patterns */
    {"wearflag", oedit_wear},       /* ROM */
    {"type", oedit_type},           /* ROM */
    {"material", oedit_material},   /* ROM */
    {"loadchance", oedit_lchance},  /* Patterns - chance of load/reset */
    {"level", oedit_level},         /* ROM */
    {"fuzzy", oedit_fuzzy},         /* Random levels for objects*/
    {"condition", oedit_condition}, /* ROM */
    {"statset", oedit_statset}, 
    {"addoprog", oedit_addoprog}, {"deloprog", oedit_deloprog}, 
    {"?", show_help}, {"version", show_version}, 
    {
      NULL, 0, }};

  const struct olc_cmd_type medit_table[] = {
    /*  {   command		function	}, */

    {"commands", show_commands}, {"create", medit_create}, {"delete", medit_delete}, {"desc", medit_desc}, {"discipline", medit_discipline}, {"level", medit_level}, {"long", medit_long}, {"name", medit_name}, {"shop", medit_shop}, {"short", medit_short}, {"show", medit_show}, {"spec", medit_spec}, {"statset", medit_statset}, //  {	"copy",		medit_copy	}, {"intelligence", medit_intelligence}, {"wounds", medit_wounds}, {"weapon", medit_weapon}, {"ttl", medit_ttl}, {"sex", medit_sex}, /* ROM */
    {"speed", medit_speed}, {"act", medit_act},           /* ROM */
    {"affect", medit_affect},     /* ROM */
    {"form", medit_form},         /* ROM */
    {"part", medit_part},         /* ROM */
    {"material", medit_material}, /* ROM */
    {"off", medit_off},           /* ROM */
    {"race", medit_race},         /* ROM */
    {"position", medit_position}, /* ROM */
    {"wealth", medit_gold},       /* ROM */
    {"group", medit_group},       /* ROM */
    {"addmprog", medit_addmprog}, /* ROM */
    {"delmprog", medit_delmprog}, /* ROM */
    {"target", medit_target}, {"?", show_help}, {"version", show_version}, 
    {
      NULL, 0, }};

  /*****************************************************************************
*                          End Interpreter Tables.                          *
*****************************************************************************/

  /*****************************************************************************
Name:		get_area_data
Purpose:	Returns pointer to area with given vnum.
Called by:	do_aedit(olc.c).
****************************************************************************/
  AREA_DATA *get_area_data(int vnum) {
    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      if ((*it)->vnum == vnum)
      return *it;
    }

    return 0;
  }

  /*****************************************************************************
Name:		edit_done
Purpose:	Resets builder information on completion.
Called by:	aedit, redit, oedit, medit(olc.c)
****************************************************************************/
  bool edit_done(CHAR_DATA *ch) {
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    return FALSE;
  }

  /*****************************************************************************
*                              Interpreters.                                *
*****************************************************************************/

  /* Area Interpreter, called by do_aedit. */
  void aedit(CHAR_DATA *ch, char *argument) {
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int cmd;
    int value;

    EDIT_AREA(ch, pArea);
    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (!str_cmp(command, "done")) {
      edit_done(ch);
      return;
    }

    if (command[0] == '\0') {
      aedit_show(ch, argument);
      return;
    }

    if ((value = flag_value(area_flags, command)) != NO_FLAG) {
      TOGGLE_BIT(pArea->area_flags, value);

      send_to_char("Flag toggled.\n\r", ch);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (cmd = 0; aedit_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(command, aedit_table[cmd].name)) {
        if ((*aedit_table[cmd].olc_fun)(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          return;
        }
        else
        return;
      }
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
  }

  /****
* Group interperator.  Basically this sets up mobiles already toggled in a room
* into groups so that they may combat mortals with all their vigor!
*/
  void groupedit(CHAR_DATA *ch, char *argument) {
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    EDIT_ROOM(ch, pRoom);
    pArea = pRoom->area;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (!str_cmp(command, "done")) {
      edit_done(ch);
      return;
    }

    if (command[0] == '\0') {
      groupedit_show(ch, argument);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (cmd = 0; groupedit_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(command, groupedit_table[cmd].name)) {
        if ((*groupedit_table[cmd].olc_fun)(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          return;
        }
        else
        return;
      }
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
  }

  /* Room Interpreter, called by do_redit. */
  void redit(CHAR_DATA *ch, char *argument) {
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    EDIT_ROOM(ch, pRoom);
    pArea = pRoom->area;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (!str_cmp(command, "done")) {
      edit_done(ch);
      return;
    }

    if (command[0] == '\0') {
      redit_show(ch, argument);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (cmd = 0; redit_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(command, redit_table[cmd].name)) {
        if ((*redit_table[cmd].olc_fun)(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          return;
        }
        else
        return;
      }
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
  }

  /* Object Interpreter, called by do_oedit. */
  void oedit(CHAR_DATA *ch, char *argument) {
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    EDIT_OBJ(ch, pObj);
    pArea = pObj->area;

    if (!str_cmp(command, "done")) {
      edit_done(ch);
      return;
    }

    if (command[0] == '\0') {
      oedit_show(ch, argument);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (cmd = 0; oedit_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(command, oedit_table[cmd].name)) {
        if ((*oedit_table[cmd].olc_fun)(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          return;
        }
        else
        return;
      }
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
  }

  /* Mobile Interpreter, called by do_medit. */
  void medit(CHAR_DATA *ch, char *argument) {
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    EDIT_MOB(ch, pMob);
    pArea = pMob->area;

    if (!str_cmp(command, "done")) {
      edit_done(ch);
      return;
    }

    if (command[0] == '\0') {
      medit_show(ch, argument);
      return;
    }

    /* Search Table and Dispatch Command. */
    for (cmd = 0; medit_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(command, medit_table[cmd].name)) {
        if ((*medit_table[cmd].olc_fun)(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          return;
        }
        else
        return;
      }
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
  }

  const struct editor_cmd_type editor_table[] = {
    /*  {   command		function	}, */

    {"area", do_aedit}, {"room", do_redit}, {"object", do_oedit}, {"mobile", do_medit}, {"mpcode", do_mpedit}, {"rpcode", do_rpedit}, {"opcode", do_opedit}, {"hedit", do_hedit}, {"group", do_groupedit}, /*
{	"sredit",	do_sredit	}, */
    {
      NULL, 0, }};

  /* Entry point for all editors. */
  _DOFUN(do_olc) {
    char command[MAX_INPUT_LENGTH];
    int cmd;

    if (IS_NPC(ch))
    return;

    argument = one_argument(argument, command);

    if (command[0] == '\0') {
      do_function(ch, &do_help, "olc");
      return;
    }

    /* Search Table and Dispatch Command. */
    for (cmd = 0; editor_table[cmd].name != NULL; cmd++) {
      if (!str_prefix(command, editor_table[cmd].name)) {
        (*editor_table[cmd].do_fun)(ch, argument);
        return;
      }
    }

    /* Invalid command, send help. */
    do_function(ch, &do_help, "olc");
    return;
  }

  /* Entry point for editing area_data. */
  _DOFUN(do_aedit) {
    AREA_DATA *pArea;
    int value;
    char arg[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    return;

    pArea = ch->in_room->area;

    argument = one_argument_nouncap(argument, arg);

    if (!str_cmp(arg, "completed")) {
      if (pArea->area_completed != 1) {
        pArea->area_completed = 1;
        send_to_char("Area Completed", ch);
        return;
      }
      if (pArea->area_completed == 1) {
        pArea->area_completed = 0;
        send_to_char("Area InCompleted", ch);
        return;
      }
    }
    if (is_number(arg)) {
      value = atoi(arg);
      if (!(pArea = get_area_data(value))) {
        send_to_char("That area vnum does not exist.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg, "delete")) {
      aedit_delete(ch, argument);
      return;
    }

    else if (!str_cmp(arg, "create")) {

      aedit_create(ch, "");
      ch->desc->editor = ED_AREA;
      return;
    }

    ch->desc->pEdit = (void *)pArea;
    ch->desc->editor = ED_AREA;
    return;
  }

  /* Entry point for editing room_index_data. */
  _DOFUN(do_redit) {
    ROOM_INDEX_DATA *pRoom;
    char arg1[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    return;

    argument = one_argument(argument, arg1);

    pRoom = ch->in_room;

    if (!str_cmp(arg1, "reset")) /* redit reset */
    {
      reset_room(pRoom, FALSE);
      send_to_char("Room reset.\n\r", ch);

      return;
    }
    else if (!str_cmp(arg1, "create")) /* redit create <vnum> */
    {
      if (argument[0] == '\0' || atoi(argument) == 0) {
        send_to_char("Syntax:  edit room create [vnum]\n\r", ch);
        return;
      }

      if (redit_create(ch, argument)) /* pEdit == nuevo cuarto */
      {
        ch->desc->editor = ED_ROOM;
        char_from_room(ch);
        char_to_room(ch, (ROOM_INDEX_DATA *)ch->desc->pEdit);
        SET_BIT(((ROOM_INDEX_DATA *)ch->desc->pEdit)->area->area_flags, AREA_CHANGED);
      }

      return;
    }
    else if (!IS_NULLSTR(arg1)) /* redit <vnum> */
    {
      pRoom = get_room_index(atoi(arg1));

      if (!pRoom) {
        send_to_char("REdit : cuarto inexistente.\n\r", ch);
        return;
      }

      char_from_room(ch);
      char_to_room(ch, pRoom);
    }

    ch->desc->pEdit = (void *)pRoom;
    ch->desc->editor = ED_ROOM;

    return;
  }

  /* Entry point for editing obj_index_data. */
  _DOFUN(do_oedit) {
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    char arg1[MAX_STRING_LENGTH];
    int value;

    if (IS_NPC(ch))
    return;

    argument = one_argument(argument, arg1);

    if (is_number(arg1)) {
      value = atoi(arg1);
      if (!(pObj = get_obj_index(value))) {
        send_to_char("OEdit:  That vnum does not exist.\n\r", ch);
        return;
      }

      ch->desc->pEdit = (void *)pObj;
      ch->desc->editor = ED_OBJECT;
      return;
    }
    else {
      if (!str_cmp(arg1, "create")) {
        value = atoi(argument);
        if (argument[0] == '\0' || value == 0) {
          send_to_char("Syntax:  edit object create [vnum]\n\r", ch);
          return;
        }

        pArea = get_vnum_area(value);

        if (!pArea) {
          send_to_char("OEdit:  That vnum is not assigned an area.\n\r", ch);
          return;
        }

        if (oedit_create(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          ch->desc->editor = ED_OBJECT;
        }
        return;
      }
      else if (!str_cmp(arg1, "delete")) {
        oedit_delete(ch, argument);
        return;
      }
    }

    send_to_char("OEdit:  There is no default object to edit.\n\r", ch);
    return;
  }

  /* Entry point for editing group_index_data. */
  _DOFUN(do_groupedit) {
    GROUP_INDEX_DATA *pGroup;
    AREA_DATA *pArea;
    char arg1[MAX_STRING_LENGTH];
    int value;

    if (IS_NPC(ch))
    return;

    argument = one_argument(argument, arg1);

    if (is_number(arg1)) {
      value = atoi(arg1);
      if (!(pGroup = get_group_index(value))) {
        send_to_char("GROUPEdit:  That vnum does not exist.\n\r", ch);
        return;
      }

      ch->desc->pEdit = (void *)pGroup;
      ch->desc->editor = ED_GROUP;
      return;
    }
    else {
      if (!str_cmp(arg1, "create")) {
        value = atoi(argument);
        if (argument[0] == '\0' || value == 0) {
          send_to_char("Syntax:  edit group create [vnum]\n\r", ch);
          return;
        }

        pArea = get_vnum_area(value);

        if (!pArea) {
          send_to_char("GROUPEdit:  That vnum is not assigned an area.\n\r", ch);
          return;
        }

        if (groupedit_create(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          ch->desc->editor = ED_GROUP;
        }
        return;
      }
    }

    send_to_char("GROUPEdit:  There is no default group to edit.\n\r", ch);
    return;
  }

  /* Entry point for editing mob_index_data. */
  _DOFUN(do_medit) {
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int value;
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg1);

    if (IS_NPC(ch))
    return;

    if (is_number(arg1)) {
      value = atoi(arg1);
      if (!(pMob = get_mob_index(value))) {
        send_to_char("MEdit:  That vnum does not exist.\n\r", ch);
        return;
      }

      ch->desc->pEdit = (void *)pMob;
      ch->desc->editor = ED_MOBILE;
      return;
    }
    else {
      if (!str_cmp(arg1, "create")) {
        value = atoi(argument);
        if (arg1[0] == '\0' || value == 0) {
          send_to_char("Syntax:  edit mobile create [vnum]\n\r", ch);
          return;
        }

        pArea = get_vnum_area(value);

        if (!pArea) {
          send_to_char("OEdit:  That vnum is not assigned an area.\n\r", ch);
          return;
        }

        if (medit_create(ch, argument)) {
          SET_BIT(pArea->area_flags, AREA_CHANGED);
          ch->desc->editor = ED_MOBILE;
        }
        return;
      }
      else if (!str_cmp(arg1, "delete")) {
        medit_delete(ch, argument);
        return;
      }
    }

    send_to_char("MEdit:  There is no default mobile to edit.\n\r", ch);
    return;
  }

  void display_resets(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *pRoom;
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pMob = NULL;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char final[MAX_STRING_LENGTH];
    int iReset = 0;

    EDIT_ROOM(ch, pRoom);
    final[0] = '\0';

    send_to_char(" No.  Loads    Description       Location         Vnum   Mx Mn Description\n\r==== ======== ============= =================== ======== ===== ===========\n\r", ch);

    for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
      OBJ_INDEX_DATA *pObj;
      MOB_INDEX_DATA *pMobIndex;
      OBJ_INDEX_DATA *pObjIndex;
      OBJ_INDEX_DATA *pObjToIndex;
      ROOM_INDEX_DATA *pRoomIndex;
      GROUP_INDEX_DATA *pGroupIndex;

      final[0] = '\0';
      sprintf(final, "[%2d] ", ++iReset);

      switch (pReset->command) {
      default:
        sprintf(buf, "Bad reset command: %c.", pReset->command);
        strcat(final, buf);
        break;

      case 'Q':
        if (!(pGroupIndex = get_group_index(pReset->arg1))) {
          sprintf(buf, "Load Group - Bad Group %d\n\r", pReset->arg1);
          strcat(final, buf);
          continue;
        }

        if (!(pRoomIndex = get_room_index(pReset->arg3))) {
          sprintf(buf, "Load Group - Bad Room %d\n\r", pReset->arg3);
          strcat(final, buf);
          continue;
        }

        sprintf(buf, "Q[%5d] are in the room              R[%5d]\n\r", pReset->arg1, pReset->arg3);
        strcat(final, buf);
        /** EDHERE **/
        break;

      case 'M':
        if (!(pMobIndex = get_mob_index(pReset->arg1))) {
          sprintf(buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1);
          strcat(final, buf);
          continue;
        }

        if (!(pRoomIndex = get_room_index(pReset->arg3))) {
          sprintf(buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3);
          strcat(final, buf);
          continue;
        }

        pMob = pMobIndex;
        remove_color(buf2, pMob->short_descr);
        remove_color(buf3, pRoomIndex->name);
        sprintf(
        buf, "M[%5d] %13.13s in room             R[%5d] %2d-%2d %-.15s`x\n\r", pReset->arg1, buf2, pReset->arg3, pReset->arg2, pReset->arg4, buf3);
        strcat(final, buf);

        /*
* Check for pet shop.
* -------------------
*/

        break;

      case 'O':
        if (!(pObjIndex = get_obj_index(pReset->arg1))) {
          sprintf(buf, "Load Object - Bad Object %d\n\r", pReset->arg1);
          strcat(final, buf);
          continue;
        }

        pObj = pObjIndex;

        if (!(pRoomIndex = get_room_index(pReset->arg3))) {
          sprintf(buf, "Load Object - Bad Room %d\n\r", pReset->arg3);
          strcat(final, buf);
          continue;
        }

        remove_color(buf2, pObj->short_descr);
        remove_color(buf3, pRoomIndex->name);
        sprintf(buf, "O[%5d] %13.13s in room             R[%5d]       %-.15s\n\r", pReset->arg1, buf2, pReset->arg3, buf3);
        strcat(final, buf);
        break;

      case 'P':
        if (!(pObjIndex = get_obj_index(pReset->arg1))) {
          sprintf(buf, "Put Object - Bad Object %d\n\r", pReset->arg1);
          strcat(final, buf);
          continue;
        }

        pObj = pObjIndex;

        if (!(pObjToIndex = get_obj_index(pReset->arg3))) {
          sprintf(buf, "Put Object - Bad To Object %d\n\r", pReset->arg3);
          strcat(final, buf);
          continue;
        }

        remove_color(buf2, pObj->short_descr);
        remove_color(buf3, pObjToIndex->short_descr);
        sprintf(
        buf, "O[%5d] %13.13s inside              O[%5d] %2d-%2d %-.15s\n\r", pReset->arg1, buf2, pReset->arg3, pReset->arg2, pReset->arg4, buf3);
        strcat(final, buf);

        break;

      case 'G':
      case 'E':
        if (!(pObjIndex = get_obj_index(pReset->arg1))) {
          sprintf(buf, "Give/Equip Object - Bad Object %d\n\r", pReset->arg1);
          strcat(final, buf);
          continue;
        }

        pObj = pObjIndex;

        if (!pMob) {
          sprintf(buf, "Give/Equip Object - No Previous Mobile\n\r");
          strcat(final, buf);
          break;
        }

        remove_color(buf2, pObj->short_descr);
        remove_color(buf3, pMob->short_descr);
        if (pMob->pShop) {
          sprintf(buf, "O[%5d] %13.13s in the inventory of S[%5d]       %-.15s\n\r", pReset->arg1, buf2, pMob->vnum, buf3);
        }
        else
        sprintf(buf, "O[%5d] %.13s %-19.19s M[%5d]       %-.15s`x\n\r", pReset->arg1, buf2, (pReset->command == 'G')
        ? flag_string(wear_loc_strings, WEAR_NONE)
        : flag_string(wear_loc_strings, pReset->arg3), pMob->vnum, buf3);
        strcat(final, buf);

        break;

        /*
* Doors are set in rs_flags don't need to be displayed.
* If you want to display them then uncomment the new_reset
* line in the case 'D' in load_resets in db.c and here.
*/
      case 'D':
        pRoomIndex = get_room_index(pReset->arg1);
        remove_color(buf2, pRoomIndex->name);
        sprintf(buf, "R[%5d] %s door of %-19.19s reset to %s\n\r", pReset->arg1, capitalize(dir_name[pReset->arg2][0]), buf2, flag_string(door_resets, pReset->arg3));
        strcat(final, buf);

        break;
        /*
* End Doors Comment.
*/
      case 'R':
        if (!(pRoomIndex = get_room_index(pReset->arg1))) {
          sprintf(buf, "Randomize Exits - Bad Room %d\n\r", pReset->arg1);
          strcat(final, buf);
          continue;
        }

        sprintf(buf, "R[%5d] Exits are randomized in %s\n\r", pReset->arg1, pRoomIndex->name);
        strcat(final, buf);

        break;
      }
      send_to_char(final, ch);
    }

    return;
  }

  /*****************************************************************************
Name:		add_reset
Purpose:	Inserts a new reset in the given index slot.
Called by:	do_resets(olc.c).
****************************************************************************/
  void add_reset(ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index) {
    RESET_DATA *reset;
    int iReset = 0;

    if (!room->reset_first) {
      room->reset_first = pReset;
      room->reset_last = pReset;
      pReset->next = NULL;
      return;
    }

    index--;

    if (index == 0) /* First slot (1) selected. */
    {
      pReset->next = room->reset_first;
      room->reset_first = pReset;
      return;
    }

    /*
* If negative slot( <= 0 selected) then this will find the last.
*/
    for (reset = room->reset_first; reset->next; reset = reset->next) {
      if (++iReset == index)
      break;
    }

    pReset->next = reset->next;
    reset->next = pReset;
    if (!pReset->next)
    room->reset_last = pReset;
    return;
  }

  _DOFUN(do_resets) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char arg5[MAX_INPUT_LENGTH];
    char arg6[MAX_INPUT_LENGTH];
    char arg7[MAX_INPUT_LENGTH];
    RESET_DATA *pReset = NULL;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    argument = one_argument(argument, arg4);
    argument = one_argument(argument, arg5);
    argument = one_argument(argument, arg6);
    argument = one_argument(argument, arg7);

    /*
* Display resets in current room.
* -------------------------------
*/
    if (arg1[0] == '\0') {
      if (ch->in_room->reset_first) {
        send_to_char("Resets: M = mobile, R = room, O = object, P = pet, S = shopkeeper\n\r", ch);
        display_resets(ch);
      }
      else
      send_to_char("No resets in this room.\n\r", ch);
    }

    /*
* Take index number and search for commands.
* ------------------------------------------
*/
    if (is_number(arg1)) {
      ROOM_INDEX_DATA *pRoom = ch->in_room;

      /*
* Delete a reset.
* ---------------
*/
      if (!str_cmp(arg2, "delete")) {
        int insert_loc = atoi(arg1);

        if (!ch->in_room->reset_first) {
          send_to_char("No resets in this area.\n\r", ch);
          return;
        }

        if (insert_loc - 1 <= 0) {
          pReset = pRoom->reset_first;
          pRoom->reset_first = pRoom->reset_first->next;
          if (!pRoom->reset_first)
          pRoom->reset_last = NULL;
        }
        else {
          int iReset = 0;
          RESET_DATA *prev = NULL;

          for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
            if (++iReset == insert_loc)
            break;
            prev = pReset;
          }

          if (!pReset) {
            send_to_char("Reset not found.\n\r", ch);
            return;
          }

          if (prev)
          prev->next = prev->next->next;
          else
          pRoom->reset_first = pRoom->reset_first->next;

          for (pRoom->reset_last = pRoom->reset_first; pRoom->reset_last->next;
          pRoom->reset_last = pRoom->reset_last->next)
          ;
        }

        free_reset_data(pReset);
        send_to_char("Reset deleted.\n\r", ch);
      }
      else
      /*
* Add a reset.
* ------------
*/
      if ((!str_cmp(arg2, "mob") && is_number(arg3)) || (!str_cmp(arg2, "obj") && is_number(arg3)) || (!str_cmp(arg2, "group") && is_number(arg3))) {
        /*
* Check for Mobile reset.
* -----------------------
*/
        if (!str_cmp(arg2, "mob")) {
          if (get_mob_index(is_number(arg3) ? atoi(arg3) : 1) == NULL) {
            send_to_char("Mob no existe.\n\r", ch);
            return;
          }
          pReset = new_reset_data();
          pReset->command = 'M';
          pReset->arg1 = atoi(arg3);
          pReset->arg2 = is_number(arg4) ? atoi(arg4) : 1; /* Max # */
          pReset->arg3 = ch->in_room->vnum;
          pReset->arg4 = is_number(arg5) ? atoi(arg5) : 1; /* Min # */
        }
        else if (!str_cmp(arg2, "group")) {
          if (get_group_index(is_number(arg3) ? atoi(arg3) : 1) == NULL) {
            send_to_char("Group doesn't exist, fool!\n\r", ch);
            return;
          }

          pReset = new_reset_data();
          pReset->command = 'Q';
          pReset->arg1 = atoi(arg3);
          pReset->arg2 = -1;
          pReset->arg3 = ch->in_room->vnum;
          pReset->arg4 = -1;
        }
        else
        /*
* Check for Object reset.
* -----------------------
*/
        if (!str_cmp(arg2, "obj")) {
          pReset = new_reset_data();
          pReset->arg1 = atoi(arg3);
          /*
* Inside another object.
* ----------------------
*/
          if (!str_prefix(arg4, "inside")) {
            OBJ_INDEX_DATA *temp;

            temp = get_obj_index(is_number(arg5) ? atoi(arg5) : 1);
            if ((temp->item_type != ITEM_CONTAINER) && (temp->item_type != ITEM_CORPSE_NPC)) {
              send_to_char("That object is not a container or a quiver.\n\r", ch);
              return;
            }
            pReset->command = 'P';
            pReset->arg2 = is_number(arg6) ? atoi(arg6) : 1;
            pReset->arg3 = is_number(arg5) ? atoi(arg5) : 1;
            pReset->arg4 = is_number(arg7) ? atoi(arg7) : 1;
          }
          else
          /*
* Inside the room.
* ----------------
*/
          if (!str_cmp(arg4, "room")) {
            if (get_obj_index(atoi(arg3)) == NULL) {
              send_to_char("Vnum no existe.\n\r", ch);
              return;
            }
            pReset->command = 'O';
            pReset->arg2 = 0;
            pReset->arg3 = ch->in_room->vnum;
            pReset->arg4 = 0;
          }
          else
          /*
* Into a Mobile's inventory.
* --------------------------
*/
          {
            if (flag_value(wear_loc_flags, arg4) == NO_FLAG) {
              send_to_char("Resets: '? wear-loc'\n\r", ch);
              return;
            }
            if (get_obj_index(atoi(arg3)) == NULL) {
              send_to_char("Vnum no existe.\n\r", ch);
              return;
            }
            pReset->arg1 = atoi(arg3);
            pReset->arg3 = flag_value(wear_loc_flags, arg4);
            if (pReset->arg3 == WEAR_NONE)
            pReset->command = 'G';
            else
            pReset->command = 'E';
          }
        }
        add_reset(ch->in_room, pReset, atoi(arg1));
        SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
        send_to_char("Reset added.\n\r", ch);
      }
      else if (!str_cmp(arg2, "random") && is_number(arg3)) {
        if (atoi(arg3) < 1 || atoi(arg3) > 6) {
          send_to_char("Invalid argument.\n\r", ch);
          return;
        }
        pReset = new_reset_data();
        pReset->command = 'R';
        pReset->arg1 = ch->in_room->vnum;
        pReset->arg2 = atoi(arg3);
        add_reset(ch->in_room, pReset, atoi(arg1));
        SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
        send_to_char("Random exits reset added.\n\r", ch);
      }
      else {
        send_to_char("Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch);
        send_to_char("        RESET <number> OBJ <vnum> inside <vnum> [limit] [count]\n\r", ch);
        send_to_char("        RESET <number> OBJ <vnum> room\n\r", ch);
        send_to_char("        RESET <number> MOB <vnum> [max #x area] [max #x room]\n\r", ch);
        send_to_char("        RESET <number> DELETE\n\r", ch);
        send_to_char("        RESET <number> RANDOM [#x exits]\n\r", ch);
      }
    }

    return;
  }

  /*****************************************************************************
Name:		do_alist
Purpose:	Normal command to list areas and display area information.
Called by:	interpreter(interp.c)
****************************************************************************/
  _DOFUN(do_alist) {
    char buf[MAX_STRING_LENGTH];
    char result[MAX_STRING_LENGTH * 3]; /* May need tweaking. */
    char arg[MSL];
    int totalarea = 0;
    //    bool fAll = FALSE;
    AREA_DATA *pArea;

    if (IS_NPC(ch))
    return;

    sprintf(result, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %-10s\n\r", "Num", "Area Name", "lvnum", "uvnum", "Filename", "World");

    one_argument(argument, arg);
    /*
if (arg[0] == '\0')
fAll = TRUE;
else if(!str_cmp(arg, "new"))
type = 0;
else if(!str_cmp(arg, "done"))
type = 1;
else if(!str_cmp(arg, "recall"))
type = 2;
else if(!str_cmp(arg, "review"))
type = 3;
else if(!str_cmp(arg, "general"))
type = 4;
*/
    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;

      sprintf(buf, "[%3d] %-29.29s (%-5d-%5d) %-12.12s %-10.10s\n\r", pArea->vnum, pArea->name, pArea->min_vnum, pArea->max_vnum, pArea->file_name, world_names[pArea->world]);
      strcat(result, buf);
      totalarea++;
    }

    sprintf(buf, "\nTotal Areas: %d", totalarea);
    strcat(result, buf);
    page_to_char(result, ch);
    return;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

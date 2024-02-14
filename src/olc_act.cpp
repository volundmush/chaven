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
#include <algorithm>
#include <vector>
#include <map>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"

#if defined(__cplusplus)
extern "C" {
#endif


  void do_function args((CHAR_DATA * ch, DO_FUN *do_fun, char *arg));

  /* Return TRUE if area changed, FALSE if not. */
#define REDIT(fun) bool fun(CHAR_DATA *ch, char *argument)
#define OEDIT(fun) bool fun(CHAR_DATA *ch, char *argument)
#define MEDIT(fun) bool fun(CHAR_DATA *ch, char *argument)
#define AEDIT(fun) bool fun(CHAR_DATA *ch, char *argument)
#define GROUPEDIT(fun) bool fun(CHAR_DATA *ch, char *argument)

  struct olc_help_type {
    char *command;
    const void *structure;
    char *desc;
  };

  bool show_version(CHAR_DATA *ch, char *argument) {
    send_to_char(VERSION, ch);
    send_to_char("\n\r", ch);
    send_to_char(AUTHOR, ch);
    send_to_char("\n\r", ch);
    send_to_char(DATE, ch);
    send_to_char("\n\r", ch);
    send_to_char(CREDITS, ch);
    send_to_char("\n\r", ch);

    return FALSE;
  }

  /*
* This table contains help commands and a brief description of each.
* ------------------------------------------------------------------
*/
  const struct olc_help_type help_table[] = {
    {"area", area_flags, "Area attributes."}, {"room", room_flags, "Room attributes."}, {"sector", sector_flags, "Sector types, terrain."}, {"exit", exit_flags, "Exit types."}, {"type", type_flags, "Types of objects."}, {"extra", extra_flags, "Object attributes."}, {"wear", wear_flags, "Where to wear object."}, {"sex", sex_flags, "Sexes."}, {"act", act_flags, "Mobile attributes."}, {"affect", affect_flags, "Mobile affects."}, {"wear-loc", wear_loc_flags, "Where mobile wears object."}, {"container", container_flags, "Container status."}, 
    /* ROM specific bits: */

    {"armor", ac_type, "Ac for different attacks."}, {"apply", apply_flags, "Apply flags"}, {"form", form_flags, "Mobile body form."}, {"part", part_flags, "Mobile body parts."}, {"imm", imm_flags, "Mobile immunity."}, {"res", res_flags, "Mobile resistance."}, {"vuln", vuln_flags, "Mobile vulnerability."}, {"off", off_flags, "Mobile offensive behaviour."}, {"size", size_flags, "Mobile size."}, {"position", position_flags, "Mobile positions."}, {"wclass", weapon_class, "Weapon class."}, {"wtype", weapon_type2, "Special weapon type."}, {"portal", portal_flags, "Portal types."}, {"furniture", furniture_flags, "Furniture types."}, {"liquid", liq_table, "Liquid types."}, {"apptype", apply_types, "Apply types."}, {"mprog", mprog_flags, "MobProgram flags."}, {"oprog", oprog_flags, "ObjProgram flags."}, {"rprog", rprog_flags, "RoomProgram flags."}, {NULL, NULL, NULL}};

  /*****************************************************************************
Name:		show_flag_cmds
Purpose:	Displays settable flags and stats.
Called by:	show_help(olc_act.c).
****************************************************************************/
  void show_flag_cmds(CHAR_DATA *ch, const struct flag_type *flag_table) {
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int flag;
    int col;

    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++) {
      if (flag_table[flag].settable) {
        sprintf(buf, "%-19.18s", flag_table[flag].name);
        strcat(buf1, buf);
        if (++col % 4 == 0)
        strcat(buf1, "\n\r");
      }
    }

    if (col % 4 != 0)
    strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
  }

  /*****************************************************************************
Name:		show_skill_cmds
Purpose:	Displays all skill functions.
Does remove those damn immortal commands from the list.
Could be improved by:
(1) Adding a check for a particular class.
(2) Adding a check for a level range.
Called by:	show_help(olc_act.c).
****************************************************************************/
  void show_skill_cmds(CHAR_DATA *ch, int tar) {
    /*
char buf  [ MAX_STRING_LENGTH ];
char buf1 [ MAX_STRING_LENGTH*2 ];
int  col;
indirect_vector<Ability>::iterator it = ability_vect.begin();

buf1[0] = '\0';
col = 0;

for (; it != ability_vect.end(); ++it)
{
if ( ***it == NULL )
break;

if ( !str_cmp( ***it, "reserved" ) || !instanceof<Skill>(*it))
continue;

if ( tar == -1 || (*it)->getTarget() == tar )
{
sprintf( buf, "%-19.18s", ***it );
strcat( buf1, buf );
if ( ++col % 4 == 0 )
strcat( buf1, "\n\r" );
}
}

if ( col % 4 != 0 )
strcat( buf1, "\n\r" );


send_to_char( buf1, ch );
*/
    return;
  }

  /*****************************************************************************
Name:		show_help
Purpose:	Displays help for many tables used in OLC.
Called by:	olc interpreters.
****************************************************************************/
  bool show_help(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char spell[MAX_INPUT_LENGTH];
    int cnt;

    argument = one_argument(argument, arg);
    one_argument(argument, spell);

    /*
* Display syntax.
*/
    if (arg[0] == '\0') {
      send_to_char("Syntax:  ? [command]\n\r\n\r", ch);
      send_to_char("[command]  [description]\n\r", ch);
      for (cnt = 0; help_table[cnt].command != NULL; cnt++) {
        sprintf(buf, "%-10.10s -%s\n\r", capitalize(help_table[cnt].command), help_table[cnt].desc);
        send_to_char(buf, ch);
      }
      return FALSE;
    }

    /*
* Find the command, show changeable data.
* ---------------------------------------
*/
    for (cnt = 0; help_table[cnt].command != NULL; cnt++) {
      if (arg[0] == help_table[cnt].command[0] && !str_prefix(arg, help_table[cnt].command)) {
        if (help_table[cnt].structure == liq_table) {
          show_liqlist(ch);
          return FALSE;
        }
        /*
else
if ( help_table[cnt].structure == *ability_vect )
{

if ( spell[0] == '\0' )
{
send_to_char( "Syntax:  ? spells [ignore/attack/defend/self/object/all]\n\r", ch
); return FALSE;
}

if ( !str_prefix( spell, "all" ) )
show_skill_cmds( ch, -1 );
else if ( !str_prefix( spell, "ignore" ) )
show_skill_cmds( ch, TAR_IGNORE );
else if ( !str_prefix( spell, "attack" ) )
show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
else if ( !str_prefix( spell, "defend" ) )
show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
else if ( !str_prefix( spell, "self" ) )
show_skill_cmds( ch, TAR_CHAR_SELF );
else if ( !str_prefix( spell, "object" ) )
show_skill_cmds( ch, TAR_OBJ_INV );
else
send_to_char( "Syntax:  ? spell [ignore/attack/defend/self/object/all]\n\r", ch
);

return FALSE;
}
*/
        else {
          show_flag_cmds(ch, (const struct flag_type *)help_table[cnt].structure);
          return FALSE;
        }
      }
    }

    show_help(ch, "");
    return FALSE;
  }

  // edhere
  GROUPEDIT(groupedit_create) {
    AREA_DATA *pArea;
    GROUP_INDEX_DATA *pGroup;
    int value;
    //    int iHash;

    EDIT_GROUP(ch, pGroup);

    value = atoi(argument);

    if (argument[0] == '\0' || value <= 0) {
      send_to_char("Syntax:  create [vnum > 0]\n\r", ch);
      return FALSE;
    }

    pArea = get_vnum_area(value);
    if (!pArea) {
      send_to_char("GROUPEdit:  That vnum is not assigned an area.\n\r", ch);
      return FALSE;
    }

    if (get_group_index(value)) {
      send_to_char("GROUPEdit:  Group vnum already exists.\n\r", ch);
      return FALSE;
    }

    pGroup = new_group_index();
    pGroup->vnum = value;

    if (value > top_group_index)
    top_group_index = value;

    //    iHash			= value % MAX_KEY_HASH;
    pGroup->next = group_list;
    group_list = pGroup;
    ch->desc->pEdit = (void *)pGroup;

    send_to_char("Group created.\n\r", ch);
    return TRUE;
  }

  GROUPEDIT(groupedit_member) {
    MOB_INDEX_DATA *pMobIndex;
    int value = -1, value2 = -1;
    int bposition = -1;
    GROUP_INDEX_DATA *pGroup;
    int bonus = 1;

    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];

    argument = one_argument(argument, arg1); // Which member slot.
    argument = one_argument(argument, arg2); // Which mob vnum.
    argument = one_argument(argument, arg3); // Which battle position.
    one_argument(argument, arg4);            // Any bonus to exp on this mob?C

    EDIT_GROUP(ch, pGroup);

    if (!is_number(arg1) || !is_number(arg2)) {
      send_to_char("Syntax: members (0-5) (mob vnum) (battle position)\n\r", ch);
      return FALSE;
    }

    printf_to_char(ch, "%s %s %s\n\r", arg1, arg2, arg3);
    value = atoi(arg1);
    value2 = atoi(arg2);
    bonus = atoi(arg4);

    if (bonus == 0)
    bonus = 1;

    if (!(pMobIndex = get_mob_index(value2))) {
      send_to_char("Illegal Mobile Vnum\n\r", ch);
      return FALSE;
    }
    if (value < 0 || value > 5) {
      send_to_char("Value of 0-5 is only allowed.\n\r", ch);
      return FALSE;
    }
    if (bonus > 5) {
      send_to_char("That experience value is insane, please adjust it.\n\r", ch);
      return FALSE;
    }

    pGroup->member_vnum[value] = pMobIndex->vnum;
    pGroup->member_position[value] = bposition;
    pGroup->member_bonus[value] = bonus;
    send_to_char("Group Position Set.\n\r", ch);
    return TRUE;
  }

  GROUPEDIT(groupedit_show) {
    GROUP_INDEX_DATA *pGroup;
    char buf[MAX_STRING_LENGTH];
    char buf1[2 * MAX_STRING_LENGTH];

    EDIT_GROUP(ch, pGroup);

    buf1[0] = '\0';

    sprintf(buf, "%-10s    : %5d\n\r", "Vnum", pGroup->vnum);
    strcat(buf1, buf);

    for (int x = 0; x < 6; x++) {
      sprintf(buf, "%-10s %3d: [%5d] [%-15s]\n\r", "Member", x, pGroup->member_vnum[x], "");
      strcat(buf1, buf);
    }

    send_to_char(buf1, ch);
    return FALSE;
  }

  /* Now for the rooms */
  REDIT(redit_rlist) {
    ROOM_INDEX_DATA *pRoomIndex;
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];
    Buffer outbuf;
    char arg[MAX_INPUT_LENGTH];
    bool found;
    int vnum;
    int col = 0;

    one_argument(argument, arg);

    pArea = ch->in_room->area;
    found = FALSE;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
      if ((pRoomIndex = get_room_index(vnum))) {
        found = TRUE;
        //		sprintf( buf, "[%5d] %-17.16s", // Wanted to see the whole room name in list - Discordance
        sprintf(buf, "[%5d] %s", vnum, capitalize(pRoomIndex->name));
        outbuf.strcat(buf);
        if (++col % 1 == 0)
        outbuf.strcat("\n\r");
      }
    }

    if (!found) {
      send_to_char("Room(s) not found in this area.\n\r", ch);
      return FALSE;
    }

    if (col % 3 != 0)
    outbuf.strcat("\n\r");

    page_to_char(outbuf, ch);
    return FALSE;
  }

  REDIT(redit_mlist) {
    MOB_INDEX_DATA *pMobIndex;
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];
    Buffer outbuf;
    char arg[MAX_INPUT_LENGTH];
    bool fAll, found;
    int vnum;
    int col = 0;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Syntax:  mlist (all/name)\n\r", ch);
      return FALSE;
    }

    pArea = ch->in_room->area;
    fAll = !str_cmp(arg, "all");
    found = FALSE;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
      if ((pMobIndex = get_mob_index(vnum)) != NULL) {
        if (fAll || is_name(arg, pMobIndex->player_name)) {
          found = TRUE;
          sprintf(buf, "[%5d] (%3d) %s", pMobIndex->vnum, pMobIndex->level, capitalize(pMobIndex->short_descr));
          outbuf.strcat(buf);
          if (++col % 1 == 0)
          outbuf.strcat("\n\r");
        }
      }
    }

    if (!found) {
      send_to_char("Mobile(s) not found in this area.\n\r", ch);
      return FALSE;
    }

    if (col % 3 != 0)
    outbuf.strcat("\n\r");

    page_to_char(outbuf, ch);
    return FALSE;
  }

  REDIT(redit_olist) {
    OBJ_INDEX_DATA *pObjIndex;
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];
    Buffer outbuf;
    char arg[MAX_INPUT_LENGTH];
    bool fAll, found;
    int vnum;
    int col = 0;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Syntax:  olist (all/name/item_type)\n\r", ch);
      return FALSE;
    }

    pArea = ch->in_room->area;
    fAll = !str_cmp(arg, "all");
    found = FALSE;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
      if ((pObjIndex = get_obj_index(vnum))) {
        if (fAll || is_name(arg, pObjIndex->name) || flag_value(type_flags, arg) == pObjIndex->item_type) {
          found = TRUE;
          sprintf(buf, "[%5d] (%3d) %s", pObjIndex->vnum, pObjIndex->level, capitalize(pObjIndex->short_descr));
          outbuf.strcat(buf);
          if (++col % 1 == 0)
          outbuf.strcat("\n\r");
        }
      }
    }

    if (!found) {
      send_to_char("Object(s) not found in this area.\n\r", ch);
      return FALSE;
    }

    if (col % 3 != 0)
    outbuf.strcat("\n\r");

    page_to_char(outbuf, ch);
    return FALSE;
  }

  REDIT(redit_grouplist) {
    GROUP_INDEX_DATA *pGroupIndex;
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];
    Buffer outbuf;
    char arg[MAX_INPUT_LENGTH];
    bool found;
    int vnum;
    int col = 0;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Syntax:  grouplist (all)\n\r", ch);
      return FALSE;
    }

    pArea = ch->in_room->area;
    found = FALSE;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
      if ((pGroupIndex = get_group_index(vnum))) {
        found = TRUE;
        sprintf(buf, "[%5d]", pGroupIndex->vnum);
        outbuf.strcat(buf);
        if (++col % 1 == 0)
        outbuf.strcat("\n\r");
      }
    }

    if (!found) {
      send_to_char("Group(s) not found in this area.\n\r", ch);
      return FALSE;
    }

    if (col % 3 != 0)
    outbuf.strcat("\n\r");

    page_to_char(outbuf, ch);
    return FALSE;
  }

  REDIT(redit_mshow) {
    MOB_INDEX_DATA *pMob;
    int value;

    if (argument[0] == '\0') {
      send_to_char("Syntax:  mshow (vnum)\n\r", ch);
      return FALSE;
    }

    if (!is_number(argument)) {
      send_to_char("REdit: Need a number.\n\r", ch);
      return FALSE;
    }

    if (is_number(argument)) {
      value = atoi(argument);
      if (!(pMob = get_mob_index(value))) {
        send_to_char("REdit:  That mobile does not exist.\n\r", ch);
        return FALSE;
      }

      ch->desc->pEdit = (void *)pMob;
    }

    medit_show(ch, argument);
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE;
  }

  REDIT(redit_oshow) {
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] == '\0') {
      send_to_char("Syntax:  oshow (vnum)\n\r", ch);
      return FALSE;
    }

    if (!is_number(argument)) {
      send_to_char("REdit: Need a number.\n\r", ch);
      return FALSE;
    }

    if (is_number(argument)) {
      value = atoi(argument);
      if (!(pObj = get_obj_index(value))) {
        send_to_char("REdit:  That object does not exist.\n\r", ch);
        return FALSE;
      }

      ch->desc->pEdit = (void *)pObj;
    }

    oedit_show(ch, argument);
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE;
  }

  /*****************************************************************************
Name:		check_range( lower vnum, upper vnum )
Purpose:	Ensures the range spans only one area.
Called by:	aedit_vnum(olc_act.c).
****************************************************************************/
  bool check_range(int lower, int upper) {
    int cnt = 0;

    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      /*
* lower < area < upper
*/
      if ((lower <= (*it)->min_vnum && (*it)->min_vnum <= upper) || (lower <= (*it)->max_vnum && (*it)->max_vnum <= upper))
      ++cnt;

      if (cnt > 1)
      return FALSE;
    }
    return TRUE;
  }

  AREA_DATA *get_vnum_area(int vnum) {
    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      if (vnum >= (*it)->min_vnum && vnum <= (*it)->max_vnum)
      return *it;
    }

    return 0;
  }

  /*
* Area Editor Functions.
*/
  AEDIT(aedit_show) {
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    sprintf(buf, "Name:     [%5d] %s\n\r", pArea->vnum, pArea->name);
    send_to_char(buf, ch);

#if 0  /* ROM OLC */
    sprintf( buf, "Recall:   [%5d] %s\n\r", pArea->recall, get_room_index( pArea->recall )
    ? get_room_index( pArea->recall )->name : "none" );
    send_to_char( buf, ch );
#endif /* ROM */

    sprintf(buf, "File:     %s\n\r", pArea->file_name);
    send_to_char(buf, ch);

    sprintf(buf, "Vnums:    [%d-%d]\n\r", pArea->min_vnum, pArea->max_vnum);
    send_to_char(buf, ch);

    sprintf(buf, "Range:     [%d, %d to %d, %d]\n\r", pArea->minx, pArea->miny, pArea->maxx, pArea->maxy);
    send_to_char(buf, ch);

    sprintf(buf, "Age:      [%d]\n\r", pArea->age);
    send_to_char(buf, ch);

    sprintf(buf, "Players:  [%d]\n\r", pArea->nplayer);
    send_to_char(buf, ch);

    sprintf(buf, "Flags:    [%s]\n\r", flag_string(area_flags, pArea->area_flags));
    send_to_char(buf, ch);

    sprintf(buf, "World:    [%s]\n\r", world_names[pArea->world]);
    send_to_char(buf, ch);

    return FALSE;
  }

  AEDIT(aedit_world) {
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    for (int i = 0; i <= WORLD_ELSEWHERE; i++) {
      if (!str_cmp(argument, world_names[i])) {
        pArea->world = i;
        send_to_char("World type set.\n\r", ch);
        return TRUE;
      }
    }
    send_to_char("Possible worlds are: ", ch);
    for (int i = 0; i <= WORLD_ELSEWHERE; i++)
    printf_to_char(ch, "%s ", world_names[i]);
    send_to_char("\n\r", ch);
    return FALSE;
  }
  AEDIT(aedit_range) {
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);

    pArea->minx = atoi(arg1);
    pArea->miny = atoi(arg2);
    pArea->maxx = atoi(arg3);
    pArea->maxy = atoi(argument);

    send_to_char("Range set.\n\r", ch);
    return TRUE;
  }

  AEDIT(aedit_reset) {
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    reset_area(pArea, FALSE);
    send_to_char("Area reset.\n\r", ch);

    return FALSE;
  }

  AEDIT(aedit_create) {
    AREA_DATA *pArea;

    pArea = new_area();
    area_list.push_back(pArea);
    //    area_last->next     =   pArea;
    //    area_last		=   pArea;	/* Thanks, Walker. */
    ch->desc->pEdit = (void *)pArea;
    //    pArea->area_completed	=   FALSE;
    SET_BIT(pArea->area_flags, AREA_ADDED);
    send_to_char("Area Created.\n\r", ch);
    return FALSE;
  }

  // For deleteing areas - Discordance
  // This should probably eventually have old fields to defaults before it clears
  // too
  AEDIT(aedit_delete) {
    char arg[MSL];
    int door, i, value;
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom1;
    ROOM_INDEX_DATA *pToRoom;

    argument = one_argument(argument, arg);

    if (is_number(arg)) {
      value = atoi(arg);
      if (!(pArea = get_area_data(value))) {
        send_to_char("That area vnum does not exist.\n\r", ch);
        return FALSE;
      }
      else {
        for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          // Clearing all objects
          if (get_obj_index(i)) {
            oedit_delete(ch, arg);
          }
          // Clearing all mobs
          if (get_mob_index(i)) {
            medit_delete(ch, arg);
          }
          // Clearing all room exits
          if ((pRoom1 = get_room_index(i)) != NULL) {
            // room exits in room and in connecting rooms
            for (door = 0; door < 9; door++) {
              int rev = rev_dir[door];
              if (pRoom1->exit[door] == NULL) {
                continue;
              }

              pToRoom = pRoom1->exit[door]->u1.to_room; /* ROM OLC */

              if (pToRoom != NULL) {
                if (pToRoom->exit[rev]) {
                  free_exit(pToRoom->exit[rev]);
                  pToRoom->exit[rev] = NULL;
                }
              }

              free_exit(pRoom1->exit[door]);
              pRoom1->exit[door] = NULL;
            }
            /*
//room exits from other rooms into the current
room - important for one way exits
//all ares
for(AreaList::iterator it = area_list.begin(); it
!= area_list.end(); ++it) {
//each room per area
for(i=(*it)->min_vnum;i<=(*it)->max_vnum;i++)
{ if ((pRoom2 = get_room_index(i)) != NULL){
//each exit per room
for(door = 0;door<9;door++) {
pToRoom =
pRoom2->exit[door]->u1.to_room; if(pToRoom != NULL) {
if(pToRoom->vnum==pRoom1->vnum)
{ free_exit(pRoom2->exit[door] ); pRoom2->exit[door] = NULL;
}
}
}
}
}
}
*/
          }
        }

        reset_area(pArea, false);
        free_string(pArea->file_name);
        free_string(pArea->name);
        pArea->min_vnum = 0;
        pArea->max_vnum = 0;
        pArea->vnum = 0;
        pArea->world = 0;
        area_list.remove(pArea);
        save_area_list();
      }
    }
    else {
      send_to_char("Please enter a valid area vnum from `Walist`x.\n\r", ch);
      return FALSE;
    }

    return TRUE;
  }

  AEDIT(aedit_name) {
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if (argument[0] == '\0') {
      send_to_char("Syntax:   name [$name]\n\r", ch);
      return FALSE;
    }

    free_string(pArea->name);
    pArea->name = str_dup(argument);

    send_to_char("Name set.\n\r", ch);
    return TRUE;
  }

  AEDIT(aedit_file) {
    AREA_DATA *pArea;
    char file[MAX_STRING_LENGTH];
    int i, length;

    EDIT_AREA(ch, pArea);

    one_argument(argument, file); /* Forces Lowercase */

    if (argument[0] == '\0') {
      send_to_char("Syntax:  filename [$file]\n\r", ch);
      return FALSE;
    }

    /*
* Simple Syntax Check.
*/
    length = safe_strlen(argument);
    if (length > 8) {
      send_to_char("No more than eight characters allowed.\n\r", ch);
      return FALSE;
    }

    /*
* Allow only letters and numbers.
*/
    for (i = 0; i < length; i++) {
      if (!isalnum(file[i])) {
        send_to_char("Only letters and numbers are valid.\n\r", ch);
        return FALSE;
      }
    }

    free_string(pArea->file_name);
    strcat(file, ".are");
    pArea->file_name = str_dup(file);

    send_to_char("Filename set.\n\r", ch);
    return TRUE;
  }

  AEDIT(aedit_location) { return FALSE; }

  AEDIT(aedit_age) {
    AREA_DATA *pArea;
    char age[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument(argument, age);

    if (!is_number(age) || age[0] == '\0') {
      send_to_char("Syntax:  age [#xage]\n\r", ch);
      return FALSE;
    }

    pArea->age = atoi(age);

    send_to_char("Age set.\n\r", ch);
    return TRUE;
  }

#if 0  /* ROM OLC */
  AEDIT( aedit_recall )
  {
    AREA_DATA *pArea;
    char room[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
      send_to_char( "Syntax:  recall [#xrvnum]\n\r", ch );
      return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
      send_to_char( "AEdit:  Room vnum does not exist.\n\r", ch );
      return FALSE;
    }

    pArea->recall = value;

    send_to_char( "Recall set.\n\r", ch );
    return TRUE;
  }
#endif /* ROM OLC */

  AEDIT(aedit_vnum) {
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    char upper[MAX_STRING_LENGTH];
    int ilower;
    int iupper;

    EDIT_AREA(ch, pArea);

    argument = one_argument(argument, lower);
    one_argument(argument, upper);

    if (!is_number(lower) || lower[0] == '\0' || !is_number(upper) || upper[0] == '\0') {
      send_to_char("Syntax:  vnum [#xlower] [#xupper]\n\r", ch);
      return FALSE;
    }

    if ((ilower = atoi(lower)) > (iupper = atoi(upper))) {
      send_to_char("AEdit:  Upper must be larger then lower.\n\r", ch);
      return FALSE;
    }

    if (!check_range(atoi(lower), atoi(upper))) {
      send_to_char("AEdit:  Range must include only this area.\n\r", ch);
      return FALSE;
    }

    if (get_vnum_area(ilower) && get_vnum_area(ilower) != pArea) {
      send_to_char("AEdit:  Lower vnum already assigned.\n\r", ch);
      return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char("Lower vnum set.\n\r", ch);

    if (get_vnum_area(iupper) && get_vnum_area(iupper) != pArea) {
      send_to_char("AEdit:  Upper vnum already assigned.\n\r", ch);
      return TRUE; /* The lower value has been set. */
    }

    pArea->max_vnum = iupper;
    send_to_char("Upper vnum set.\n\r", ch);

    return TRUE;
  }

  AEDIT(aedit_lvnum) {
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    int ilower;
    int iupper;

    EDIT_AREA(ch, pArea);

    one_argument(argument, lower);

    if (!is_number(lower) || lower[0] == '\0') {
      send_to_char("Syntax:  min_vnum [#xlower]\n\r", ch);
      return FALSE;
    }

    if ((ilower = atoi(lower)) > (iupper = pArea->max_vnum)) {
      send_to_char("AEdit:  Value must be less than the max_vnum.\n\r", ch);
      return FALSE;
    }

    if (!check_range(ilower, iupper)) {
      send_to_char("AEdit:  Range must include only this area.\n\r", ch);
      return FALSE;
    }

    if (get_vnum_area(ilower) && get_vnum_area(ilower) != pArea) {
      send_to_char("AEdit:  Lower vnum already assigned.\n\r", ch);
      return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char("Lower vnum set.\n\r", ch);
    return TRUE;
  }

  AEDIT(aedit_uvnum) {
    AREA_DATA *pArea;
    char upper[MAX_STRING_LENGTH];
    int ilower;
    int iupper;

    EDIT_AREA(ch, pArea);

    one_argument(argument, upper);

    if (!is_number(upper) || upper[0] == '\0') {
      send_to_char("Syntax:  max_vnum [#xupper]\n\r", ch);
      return FALSE;
    }

    if ((ilower = pArea->min_vnum) > (iupper = atoi(upper))) {
      send_to_char("AEdit:  Upper must be larger then lower.\n\r", ch);
      return FALSE;
    }

    if (!check_range(ilower, iupper)) {
      send_to_char("AEdit:  Range must include only this area.\n\r", ch);
      return FALSE;
    }

    if (get_vnum_area(iupper) && get_vnum_area(iupper) != pArea) {
      send_to_char("AEdit:  Upper vnum already assigned.\n\r", ch);
      return FALSE;
    }

    pArea->max_vnum = iupper;
    send_to_char("Upper vnum set.\n\r", ch);

    return TRUE;
  }

  /*
* Room Editor Functions.
*/
  REDIT(redit_show) {
    ROOM_INDEX_DATA *pRoom;
    char buf[MAX_STRING_LENGTH];
    char buf1[2 * MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int door;
    bool fcnt;

    EDIT_ROOM(ch, pRoom);

    buf1[0] = '\0';

    sprintf(buf, "Description:\n\r%s\n\r", pRoom->description);
    strcat(buf1, buf);

    // sprintf( buf, "Player Description:\n\r%s\n\r", pRoom->player_description );
    // strcat( buf1, buf );

    sprintf(buf, "Shroud Description:\n\r%s\n\r", pRoom->shroud);
    strcat(buf1, buf);

    // sprintf( buf, "Player Shroud:\n\r%s\n\r", pRoom->player_shroud );
    // strcat( buf1, buf );

    sprintf(buf, "Name:       [%s]\n\rArea:       [%5d] %s\n\r", pRoom->name, pRoom->area->vnum, pRoom->area->name);
    strcat(buf1, buf);

    // Subareas - Discordance
    sprintf(buf, "Subarea:    [%s]\n\r", pRoom->subarea);
    strcat(buf1, buf);

    // districts - Discordance
    int district = district_room(pRoom);
    if (district == 0) {
      sprintf(buf, "District:   [%s]\n\r", "Anywhere");
    }
    else if (district == 1) {
      sprintf(buf, "District:   [%s]\n\r", "Urban");
    }
    else if (district == 2) {
      sprintf(buf, "District:   [%s]\n\r", "Tourist");
    }
    else if (district == 3) {
      sprintf(buf, "District:   [%s]\n\r", "Redlight");
    }
    else if (district == 4) {
      sprintf(buf, "District:   [%s]\n\r", "Historic");
    }
    else if (district == 5) {
      sprintf(buf, "District:   [%s]\n\r", "Institute");
    }
    else if (district == 6) {
      sprintf(buf, "District:   [%s]\n\r", "Westhaven");
    }
    else if (district == 7) {
      sprintf(buf, "District:   [%s]\n\r", "Farm");
    }
    else {
      sprintf(buf, "District:   [%s]\n\r", "UNKNOWN");
    }

    strcat(buf1, buf);

    sprintf(buf, "Vnum:       [%5d]\n\rSector:     [%s]\n\r", pRoom->vnum, flag_string(sector_flags, pRoom->sector_type));
    strcat(buf1, buf);

    sprintf(buf, "Coordinates: [%d, %d, %d]\n\r", pRoom->x, pRoom->y, pRoom->z);
    strcat(buf1, buf);

    sprintf(buf, "Room flags: [%s]\n\r", flag_string(room_flags, pRoom->room_flags));
    strcat(buf1, buf);

    sprintf(buf, "XY: [%d, %d]\n\r", pRoom->locx, pRoom->locy);
    strcat(buf1, buf);
    sprintf(buf, "Size: [%d]     Level: [%d]\n\r", pRoom->size, pRoom->level);
    strcat(buf1, buf);
    sprintf(buf, "Entry: [%d, %d]\n\r", pRoom->entryx, pRoom->entryy);
    strcat(buf1, buf);
    sprintf(buf, "Timezone: [%d]\n\r", pRoom->timezone);
    strcat(buf1, buf);

    int i;
    for (i = 0; i < 10; i++) {
      if (pRoom->features != NULL && pRoom->features[i] != NULL) {
        sprintf(buf, "Feature: [%d] Type: %d Pos:%d Names:%s\nShortdesc:%s\nDesc:%s\r", i, pRoom->features[i]->type, pRoom->features[i]->position, pRoom->features[i]->names, pRoom->features[i]->shortdesc, pRoom->features[i]->desc);
        strcat(buf1, buf);
      }
    }

    /*
if ( pRoom->clan > 0 )
{
sprintf( buf, "Clan      : [%d] %s\n\r", pRoom->clan, clan_table[pRoom->clan].name );
strcat( buf1, buf );
}
*/
    if (!IS_NULLSTR(pRoom->owner)) {
      sprintf(buf, "Owner     : [%s]\n\r", pRoom->owner);
      strcat(buf1, buf);
    }

    if (pRoom->extra_descr) {
      EXTRA_DESCR_DATA *ed;

      strcat(buf1, "Desc Kwds:  [");
      for (ed = pRoom->extra_descr; ed; ed = ed->next) {
        strcat(buf1, ed->keyword);
        if (ed->next)
        strcat(buf1, " ");
      }
      strcat(buf1, "]\n\r");
    }
    if (pRoom->places) {
      EXTRA_DESCR_DATA *ed;

      strcat(buf1, "Places:  [");
      for (ed = pRoom->places; ed; ed = ed->next) {
        strcat(buf1, ed->keyword);
        if (ed->next)
        strcat(buf1, " ");
      }
      strcat(buf1, "]\n\r");
    }

    strcat(buf1, "Characters: [");
    fcnt = FALSE;
    for (CharList::iterator it = pRoom->people->begin();
    it != pRoom->people->end(); ++it) {
      one_argument((*it)->name, buf);
      strcat(buf1, buf);
      strcat(buf1, " ");
      fcnt = TRUE;
    }

    if (fcnt) {
      int end;

      end = safe_strlen(buf1) - 1;
      buf1[end] = ']';
      strcat(buf1, "\n\r");
    }
    else
    strcat(buf1, "none]\n\r");

    strcat(buf1, "Objects:    [");
    fcnt = FALSE;
    for (obj = pRoom->contents; obj; obj = obj->next_content) {
      one_argument(obj->name, buf);
      strcat(buf1, buf);
      strcat(buf1, " ");
      fcnt = TRUE;
    }

    if (fcnt) {
      int end;

      end = safe_strlen(buf1) - 1;
      buf1[end] = ']';
      strcat(buf1, "\n\r");
    }
    else
    strcat(buf1, "none]\n\r");

    for (door = 0; door < MAX_DIR; door++) {
      EXIT_DATA *pexit;

      if ((pexit = pRoom->exit[door])) {
        char word[MAX_INPUT_LENGTH];
        char reset_state[MAX_STRING_LENGTH];
        char *state;
        int i, length;

        sprintf(buf, "-%-5s to [%5d] Key: [%5d] Jump: [%d] Climb: [%d] Fall: [%d] Wall: [%d]", capitalize(dir_name[door][0]), pexit->u1.to_room ? pexit->u1.to_room->vnum : 0, /* ROM OLC */
        pexit->key, pexit->jump, pexit->climb, pexit->fall, pexit->wall);
        strcat(buf1, buf);

        /*
* Format up the exit info.
* Capitalize all flags that are not part of the reset info.
*/
        strcpy(reset_state, flag_string(exit_flags, pexit->rs_flags));
        state = flag_string(exit_flags, pexit->exit_info);
        strcat(buf1, " Exit flags: [");
        for (;;) {
          state = one_argument(state, word);

          if (word[0] == '\0') {
            int end;

            end = safe_strlen(buf1) - 1;
            buf1[end] = ']';
            strcat(buf1, "\n\r");
            break;
          }

          if (str_infix(word, reset_state)) {
            length = safe_strlen(word);
            for (i = 0; i < length; i++)
            word[i] = UPPER(word[i]);
          }
          strcat(buf1, word);
          strcat(buf1, " ");
        }

        if (pexit->keyword && pexit->keyword[0] != '\0') {
          sprintf(buf, "Kwds: [%s]\n\r", pexit->keyword);
          strcat(buf1, buf);
        }
        if (pexit->description && pexit->description[0] != '\0') {
          sprintf(buf, "%s", pexit->description);
          strcat(buf1, buf);
        }
      }
    }

    send_to_char(buf1, ch);

    return FALSE;
  }

  // redit_copy function thanks to Zanthras of Mystical Realities MUD.
  REDIT(redit_copy) {
    ROOM_INDEX_DATA *pRoom;
    ROOM_INDEX_DATA *pRoom2;

    int vnum;
    if (argument[0] == '\0') {
      send_to_char("Syntax: copy (vnum) \n\r", ch);
      return FALSE;
    }

    if (!is_number(argument)) {
      send_to_char("REdit: You must enter a number (vnum).\n\r", ch);
      return FALSE;
    }
    else {
      vnum = atoi(argument);
      if (!(pRoom2 = get_room_index(vnum))) {
        send_to_char("REdit: That room doesn't exist.\n\r", ch);
        return FALSE;
      }
    }

    EDIT_ROOM(ch, pRoom);

    free_string(pRoom->description);
    pRoom->description = str_dup(pRoom2->description);

    free_string(pRoom->name);
    pRoom->name = str_dup(pRoom2->name);

    // subareas - Discordance
    free_string(pRoom->subarea);
    pRoom->name = str_dup(pRoom2->subarea);

    pRoom->sector_type = pRoom2->sector_type;

    pRoom->room_flags = pRoom2->room_flags;

    pRoom->level = pRoom2->level;
    pRoom->mana_rate = pRoom2->mana_rate;

    pRoom->clan = pRoom2->clan;

    free_string(pRoom->owner);
    pRoom->owner = str_dup(pRoom2->owner);

    pRoom->extra_descr = pRoom2->extra_descr;

    send_to_char("Room info copied.", ch);
    return TRUE;
  }

  /* Local function. */
  bool change_exit(CHAR_DATA *ch, char *argument, int door) {
    ROOM_INDEX_DATA *pRoom;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int value;
    int rev;

    EDIT_ROOM(ch, pRoom);

    /*
* Set the exit flags, needs full argument.
* ----------------------------------------
*/
    if ((value = flag_value(exit_flags, argument)) != NO_FLAG) {
      ROOM_INDEX_DATA *pToRoom;

      if (!pRoom->exit[door]) {
        send_to_char("Exit doesn't exist.\n\r", ch);
        return FALSE;
      }

      /*
* This room.
*/
      TOGGLE_BIT(pRoom->exit[door]->rs_flags, value);
      /* Don't toggle exit_info because it can be changed by players. */
      pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

      /*
* Connected room.
*/
      pToRoom = pRoom->exit[door]->u1.to_room; /* ROM OLC */
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL) {
        pToRoom->exit[rev]->rs_flags = pRoom->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = pRoom->exit[door]->exit_info;
      }

      send_to_char("Exit flag toggled.\n\r", ch);
      return TRUE;
    }

    /*
* Now parse the arguments.
*/
    argument = one_argument(argument, command);
    one_argument(argument, arg);

    if (command[0] == '\0' && argument[0] == '\0') /* Move command. */
    {
      move_char(ch, door, TRUE, FALSE);
      return FALSE;
    }

    if (command[0] == '?') {
      do_function(ch, &do_help, "EXIT");
      return FALSE;
    }

    if (!str_cmp(command, "delete")) {
      ROOM_INDEX_DATA *pToRoom;
      sh_int rev; /* ROM OLC */

      if (!pRoom->exit[door]) {
        send_to_char("REdit:  Cannot delete a null exit.\n\r", ch);
        return FALSE;
      }

      /*
* Remove ToRoom Exit.
*/
      rev = rev_dir[door];
      pToRoom = pRoom->exit[door]->u1.to_room; /* ROM OLC */

      if (pToRoom->exit[rev]) {
        free_exit(pToRoom->exit[rev]);
        pToRoom->exit[rev] = NULL;
      }

      /*
* Remove this exit.
*/
      free_exit(pRoom->exit[door]);
      pRoom->exit[door] = NULL;

      send_to_char("Exit unlinked.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "link")) {
      EXIT_DATA *pExit;
      ROOM_INDEX_DATA *toRoom;

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax:  [direction] link [vnum]\n\r", ch);
        return FALSE;
      }

      value = atoi(arg);

      if (!(toRoom = get_room_index(value))) {
        send_to_char("REdit:  Cannot link to non-existant room.\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door])
      pRoom->exit[door] = new_exit();

      pRoom->exit[door]->u1.to_room = toRoom;
      pRoom->exit[door]->orig_door = door;

      door = rev_dir[door];
      pExit = new_exit();
      pExit->u1.to_room = pRoom;
      pExit->orig_door = door;
      toRoom->exit[door] = pExit;

      send_to_char("Two-way link established.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "dig")) {
      char buf[MAX_STRING_LENGTH];

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax: [direction] dig (vnum)\n\r", ch);
        return FALSE;
      }

      redit_create(ch, arg);
      sprintf(buf, "link %s", arg);
      change_exit(ch, buf, door);
      return TRUE;
    }

    if (!str_cmp(command, "room")) {
      ROOM_INDEX_DATA *toRoom;

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax:  [direction] room [vnum]\n\r", ch);
        return FALSE;
      }

      value = atoi(arg);

      if (!(toRoom = get_room_index(value))) {
        send_to_char("REdit:  Cannot link to non-existant room.\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door])
      pRoom->exit[door] = new_exit();

      pRoom->exit[door]->u1.to_room = toRoom; /* ROM OLC */
      pRoom->exit[door]->orig_door = door;

      send_to_char("One-way link established.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "key")) {
      OBJ_INDEX_DATA *key;

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax:  [direction] key [vnum]\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door]) {
        send_to_char("Salida no existe.\n\r", ch);
        return FALSE;
      }

      value = atoi(arg);

      if (!(key = get_obj_index(value))) {
        send_to_char("REdit:  Key doesn't exist.\n\r", ch);
        return FALSE;
      }

      if (key->item_type != ITEM_KEY) {
        send_to_char("REdit:  Objeto no es llave.\n\r", ch);
        return FALSE;
      }

      pRoom->exit[door]->key = value;

      send_to_char("Exit key set.\n\r", ch);
      return TRUE;
    }
    if (!str_cmp(command, "jump")) {

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax:  [direction] jump [num]\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door]) {
        send_to_char("Salida no existe.\n\r", ch);
        return FALSE;
      }

      value = atoi(arg);

      pRoom->exit[door]->jump = value;

      send_to_char("Jump set.\n\r", ch);
      return TRUE;
    }
    if (!str_cmp(command, "fall")) {

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax:  [direction] fall [num]\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door]) {
        send_to_char("Salida no existe.\n\r", ch);
        return FALSE;
      }

      value = atoi(arg);

      pRoom->exit[door]->fall = value;

      send_to_char("Fall set.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "wall")) {

      if (!str_cmp(arg, "none"))
      value = WALL_NONE;
      else if (!str_cmp(arg, "glass"))
      value = WALL_GLASS;
      else if (!str_cmp(arg, "plaster"))
      value = WALL_PLASTER;
      else if (!str_cmp(arg, "wood"))
      value = WALL_WOOD;
      else if (!str_cmp(arg, "brick"))
      value = WALL_BRICK;
      else if (!str_cmp(arg, "steel"))
      value = WALL_STEEL;
      else {
        send_to_char("Syntax:  [direction] wall glass/plaster/wood/brick/steel/none\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door]) {
        send_to_char("Salida no existe.\n\r", ch);
        return FALSE;
      }

      pRoom->exit[door]->wall = value;

      send_to_char("Wall set.\n\r", ch);
      return TRUE;
    }
    if (!str_cmp(command, "climb")) {

      if (arg[0] == '\0' || !is_number(arg)) {
        send_to_char("Syntax:  [direction] climb [num]\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door]) {
        send_to_char("Salida no existe.\n\r", ch);
        return FALSE;
      }

      value = atoi(arg);

      pRoom->exit[door]->climb = value;

      send_to_char("climb set.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "name")) {
      if (arg[0] == '\0') {
        send_to_char("Syntax:  [direction] name [string]\n\r", ch);
        send_to_char("         [direction] name none\n\r", ch);
        return FALSE;
      }

      if (!pRoom->exit[door]) {
        send_to_char("Salida no existe.\n\r", ch);
        return FALSE;
      }

      free_string(pRoom->exit[door]->keyword);

      if (str_cmp(arg, "none"))
      pRoom->exit[door]->keyword = str_dup(arg);
      else
      pRoom->exit[door]->keyword = str_dup("");

      send_to_char("Exit name set.\n\r", ch);
      return TRUE;
    }

    if (!str_prefix(command, "description")) {
      if (arg[0] == '\0') {
        if (!pRoom->exit[door]) {
          send_to_char("Salida no existe.\n\r", ch);
          return FALSE;
        }

        string_append(ch, &pRoom->exit[door]->description);
        return TRUE;
      }

      send_to_char("Syntax:  [direction] desc\n\r", ch);
      return FALSE;
    }

    return FALSE;
  }

  REDIT(redit_north) {
    if (change_exit(ch, argument, DIR_NORTH))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_northeast) {
    if (change_exit(ch, argument, DIR_NORTHEAST))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_northwest) {
    if (change_exit(ch, argument, DIR_NORTHWEST))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_south) {
    if (change_exit(ch, argument, DIR_SOUTH))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_southeast) {
    if (change_exit(ch, argument, DIR_SOUTHEAST))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_southwest) {
    if (change_exit(ch, argument, DIR_SOUTHWEST))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_east) {
    if (change_exit(ch, argument, DIR_EAST))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_west) {
    if (change_exit(ch, argument, DIR_WEST))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_up) {
    if (change_exit(ch, argument, DIR_UP))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_down) {
    if (change_exit(ch, argument, DIR_DOWN))
    return TRUE;

    return FALSE;
  }

  REDIT(redit_place) {
    ROOM_INDEX_DATA *pRoom;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument(argument, command);
    // Commented out old line and added in the one below because the prior was
    // making everything lowercase - Discordance
    argument = one_argument_nouncap(argument, keyword);

    //    one_argument( argument, keyword );

    if (command[0] == '\0' || keyword[0] == '\0') {
      send_to_char("Syntax:  place add [keyword]\n\r", ch);
      send_to_char("         place edit [keyword]\n\r", ch);
      send_to_char("         place delete [keyword]\n\r", ch);
      send_to_char("         place format [keyword]\n\r", ch);
      return FALSE;
    }

    if (!str_cmp(command, "add")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  place add [keyword]\n\r", ch);
        return FALSE;
      }

      ed = new_extra_descr();
      ed->keyword = str_dup(keyword);
      ed->description = str_dup("");
      ed->next = pRoom->places;
      pRoom->places = ed;

      string_append(ch, &ed->description);

      return TRUE;
    }

    if (!str_cmp(command, "edit")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  place edit [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pRoom->places; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
      }

      if (!ed) {
        send_to_char("REdit:  place keyword not found.\n\r", ch);
        return FALSE;
      }

      string_append(ch, &ed->description);

      return TRUE;
    }

    if (!str_cmp(command, "delete")) {
      EXTRA_DESCR_DATA *ped = NULL;

      if (keyword[0] == '\0') {
        send_to_char("Syntax:  place delete [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pRoom->places; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
        ped = ed;
      }

      if (!ed) {
        send_to_char("REdit:  Place keyword not found.\n\r", ch);
        return FALSE;
      }

      if (!ped)
      pRoom->places = ed->next;
      else
      ped->next = ed->next;

      free_extra_descr(ed);

      send_to_char("Place deleted.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "format")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  place format [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pRoom->places; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
      }

      if (!ed) {
        send_to_char("REdit:  Place keyword not found.\n\r", ch);
        return FALSE;
      }

      ed->description = format_string(ed->description);

      send_to_char("Place formatted.\n\r", ch);
      return TRUE;
    }

    redit_place(ch, "");
    return FALSE;
  }

  REDIT(redit_ed) {
    ROOM_INDEX_DATA *pRoom;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument(argument, command);
    one_argument(argument, keyword);

    if (command[0] == '\0' || keyword[0] == '\0') {
      send_to_char("Syntax:  ed add [keyword]\n\r", ch);
      send_to_char("         ed edit [keyword]\n\r", ch);
      send_to_char("         ed delete [keyword]\n\r", ch);
      send_to_char("         ed format [keyword]\n\r", ch);
      return FALSE;
    }

    if (!str_cmp(command, "add")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed add [keyword]\n\r", ch);
        return FALSE;
      }

      ed = new_extra_descr();
      ed->keyword = str_dup(keyword);
      ed->description = str_dup("");
      ed->next = pRoom->extra_descr;
      pRoom->extra_descr = ed;

      string_append(ch, &ed->description);

      return TRUE;
    }

    if (!str_cmp(command, "edit")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed edit [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pRoom->extra_descr; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
      }

      if (!ed) {
        send_to_char("REdit:  Extra description keyword not found.\n\r", ch);
        return FALSE;
      }

      string_append(ch, &ed->description);

      return TRUE;
    }

    if (!str_cmp(command, "delete")) {
      EXTRA_DESCR_DATA *ped = NULL;

      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed delete [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pRoom->extra_descr; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
        ped = ed;
      }

      if (!ed) {
        send_to_char("REdit:  Extra description keyword not found.\n\r", ch);
        return FALSE;
      }

      if (!ped)
      pRoom->extra_descr = ed->next;
      else
      ped->next = ed->next;

      free_extra_descr(ed);

      send_to_char("Extra description deleted.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "format")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed format [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pRoom->extra_descr; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
      }

      if (!ed) {
        send_to_char("REdit:  Extra description keyword not found.\n\r", ch);
        return FALSE;
      }

      ed->description = format_string(ed->description);

      send_to_char("Extra description formatted.\n\r", ch);
      return TRUE;
    }

    redit_ed(ch, "");
    return FALSE;
  }

  REDIT(redit_name) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  name [name]\n\r", ch);
      return FALSE;
    }

    free_string(pRoom->name);
    pRoom->name = str_dup(argument);

    send_to_char("Name set.\n\r", ch);
    return TRUE;
  }

  // subareas - Discordance
  REDIT(redit_subarea) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (argument[0] == '\0') {
      send_to_char("`cSyntax`g: `Wsubarea `g(`Wname`g)`x\n\r", ch);
      return FALSE;
    }

    free_string(pRoom->subarea);
    pRoom->subarea = str_dup(argument);

    send_to_char("`cSubarea set`g.`x\n\r", ch);
    return TRUE;
  }

  REDIT(redit_desc) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (argument[0] == '\0') {
      string_append(ch, &pRoom->description);
      return TRUE;
    }

    send_to_char("Syntax:  desc\n\r", ch);
    return FALSE;
  }
  REDIT(redit_shroud) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (argument[0] == '\0') {
      string_append(ch, &pRoom->shroud);
      return TRUE;
    }

    send_to_char("Syntax:  shroud\n\r", ch);
    return FALSE;
  }

  REDIT(redit_level) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (is_number(argument)) {
      pRoom->level = atoi(argument);
      send_to_char("level set.\n\r", ch);
      return TRUE;
    }

    send_to_char("Syntax : level (#xnumber)\n\r", ch);
    return FALSE;
  }

  REDIT(redit_mana) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (is_number(argument)) {
      pRoom->mana_rate = atoi(argument);
      send_to_char("Mana rate set.\n\r", ch);
      return TRUE;
    }

    send_to_char("Syntax : mana (#xnumber)\n\r", ch);
    return FALSE;
  }

  REDIT(redit_clan) {
    send_to_char("Clan set.\n\r", ch);
    return TRUE;
  }

  REDIT(redit_format) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    pRoom->description = format_string(pRoom->description);

    send_to_char("String formatted.\n\r", ch);
    return TRUE;
  }

  REDIT(redit_mreset) {
    ROOM_INDEX_DATA *pRoom;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *newmob;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    RESET_DATA *pReset;
    char output[MAX_STRING_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument(argument, arg);
    argument = one_argument(argument, arg2);

    if (arg[0] == '\0' || !is_number(arg)) {
      send_to_char("Syntax:  mreset (vnum) (max #x) (mix #x)\n\r", ch);
      return FALSE;
    }

    if (!(pMobIndex = get_mob_index(atoi(arg)))) {
      send_to_char("REdit: No mobile has that vnum.\n\r", ch);
      return FALSE;
    }

    if (pMobIndex->area != pRoom->area) {
      send_to_char("REdit: No such mobile in this area.\n\r", ch);
      return FALSE;
    }

    /*
* Create the mobile reset.
*/
    pReset = new_reset_data();
    pReset->command = 'M';
    pReset->arg1 = pMobIndex->vnum;
    pReset->arg2 = is_number(arg2) ? atoi(arg2) : MAX_MOB;
    pReset->arg3 = pRoom->vnum;
    pReset->arg4 = is_number(argument) ? atoi(argument) : 1;
    add_reset(pRoom, pReset, 0 /* Last slot*/);

    /*
* Create the mobile.
*/
    newmob = create_mobile(pMobIndex);
    char_to_room(newmob, pRoom);

    sprintf(output, "%s (%d) has been loaded and added to resets.\n\rThere will be a maximum of %d loaded to this room.\n\r", capitalize(pMobIndex->short_descr), pMobIndex->vnum, pReset->arg2);
    send_to_char(output, ch);
    act("$n has created $N!", ch, NULL, newmob, TO_ROOM);
    return TRUE;
  }

  struct wear_type {
    int wear_loc;
    int wear_bit;
  };

  const struct wear_type wear_table[] = {{WEAR_NONE, ITEM_TAKE}, {WEAR_BODY_1, ITEM_WEAR_BODY}, {WEAR_BODY_2, ITEM_WEAR_BODY}, {WEAR_BODY_3, ITEM_WEAR_BODY}, {WEAR_BODY_4, ITEM_WEAR_BODY}, {WEAR_BODY_5, ITEM_WEAR_BODY}, {WEAR_BODY_6, ITEM_WEAR_BODY}, {WEAR_BODY_7, ITEM_WEAR_BODY}, {WEAR_BODY_8, ITEM_WEAR_BODY}, {WEAR_BODY_9, ITEM_WEAR_BODY}, {WEAR_BODY_10, ITEM_WEAR_BODY}, {WEAR_BODY_11, ITEM_WEAR_BODY}, {WEAR_BODY_12, ITEM_WEAR_BODY}, {WEAR_BODY_13, ITEM_WEAR_BODY}, {WEAR_BODY_14, ITEM_WEAR_BODY}, {WEAR_BODY_15, ITEM_WEAR_BODY}, {WEAR_BODY_16, ITEM_WEAR_BODY}, {WEAR_BODY_17, ITEM_WEAR_BODY}, {WEAR_BODY_18, ITEM_WEAR_BODY}, {WEAR_BODY_19, ITEM_WEAR_BODY}, {WEAR_BODY_20, ITEM_WEAR_BODY}, {WEAR_BODY_21, ITEM_WEAR_BODY}, {WEAR_BODY_22, ITEM_WEAR_BODY}, {WEAR_BODY_23, ITEM_WEAR_BODY}, {WEAR_BODY_24, ITEM_WEAR_BODY}, {WEAR_BODY_25, ITEM_WEAR_BODY}, {WEAR_HOLD, ITEM_HOLD}, {WEAR_HOLD_2, ITEM_HOLD}, {NO_FLAG, NO_FLAG}};

  /*****************************************************************************
Name:		wear_loc
Purpose:	Returns the location of the bit that matches the count.
1 = first match, 2 = second match etc.
Called by:	oedit_reset(olc_act.c).
****************************************************************************/
  int wear_loc(int bits, int count) {
    int flag;

    for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++) {
      if (IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
      return wear_table[flag].wear_loc;
    }

    return NO_FLAG;
  }

  /*****************************************************************************
Name:		wear_bit
Purpose:	Converts a wear_loc into a bit.
Called by:	redit_oreset(olc_act.c).
****************************************************************************/
  int wear_bit(int loc) {
    int flag;

    for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++) {
      if (loc == wear_table[flag].wear_loc)
      return wear_table[flag].wear_bit;
    }

    return 0;
  }

  REDIT(redit_oreset) {
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *newobj;
    OBJ_DATA *to_obj;
    CHAR_DATA *to_mob;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int olevel = 0;

    RESET_DATA *pReset;
    char output[MAX_STRING_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || !is_number(arg1)) {
      send_to_char("Syntax:  oreset <vnum> <args>\n\r", ch);
      send_to_char("        -no_args               = into room\n\r", ch);
      send_to_char("        -(obj_name)            = into obj\n\r", ch);
      send_to_char("        -(mob_name) (wear_loc) = into mob\n\r", ch);
      return FALSE;
    }

    if (!(pObjIndex = get_obj_index(atoi(arg1)))) {
      send_to_char("REdit: No object has that vnum.\n\r", ch);
      return FALSE;
    }

    if (pObjIndex->area != pRoom->area) {
      send_to_char("REdit: No such object in this area.\n\r", ch);
      return FALSE;
    }

    /*
* Load into room.
*/
    if (arg2[0] == '\0') {
      pReset = new_reset_data();
      pReset->command = 'O';
      pReset->arg1 = pObjIndex->vnum;
      pReset->arg2 = 0;
      pReset->arg3 = pRoom->vnum;
      pReset->arg4 = 0;
      add_reset(pRoom, pReset, 0 /* Last slot*/);

      newobj = create_object(pObjIndex, number_fuzzy(olevel));
      obj_to_room(newobj, pRoom);

      sprintf(output, "%s (%d) has been loaded and added to resets.\n\r", capitalize(pObjIndex->short_descr), pObjIndex->vnum);
      send_to_char(output, ch);
    }
    else
    /*
* Load into object's inventory.
*/
    if (argument[0] == '\0' && ((to_obj = get_obj_list(ch, arg2, pRoom->contents)) != NULL)) {
      pReset = new_reset_data();
      pReset->command = 'P';
      pReset->arg1 = pObjIndex->vnum;
      pReset->arg2 = 0;
      pReset->arg3 = to_obj->pIndexData->vnum;
      pReset->arg4 = 1;
      add_reset(pRoom, pReset, 0 /* Last slot*/);

      newobj = create_object(pObjIndex, number_fuzzy(olevel));
      newobj->cost = 0;
      obj_to_obj(newobj, to_obj);

      sprintf(output, "%s (%d) has been loaded into %s (%d) and added to resets.\n\r", capitalize(newobj->short_descr), newobj->pIndexData->vnum, to_obj->short_descr, to_obj->pIndexData->vnum);
      send_to_char(output, ch);
    }
    else
    /*
* Load into mobile's inventory.
*/
    if ((to_mob = get_char_room(ch, NULL, arg2)) != NULL) {
      int wear_loc;

      /*
* Make sure the location on mobile is valid.
*/
      if ((wear_loc = flag_value(wear_loc_flags, argument)) == NO_FLAG) {
        send_to_char("REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch);
        return FALSE;
      }

      /*
* Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
*/
      if (!IS_SET(pObjIndex->wear_flags, wear_bit(wear_loc))) {
        sprintf(output, "%s (%d) has wear flags: [%s]\n\r", capitalize(pObjIndex->short_descr), pObjIndex->vnum, flag_string(wear_flags, pObjIndex->wear_flags));
        send_to_char(output, ch);
        return FALSE;
      }

      /*
* Can't load into same position.
*/
      if (get_eq_char(to_mob, wear_loc)) {
        send_to_char("REdit:  Object already equipped.\n\r", ch);
        return FALSE;
      }

      pReset = new_reset_data();
      pReset->arg1 = pObjIndex->vnum;
      pReset->arg2 = wear_loc;
      if (pReset->arg2 == WEAR_NONE)
      pReset->command = 'G';
      else
      pReset->command = 'E';
      pReset->arg3 = wear_loc;

      add_reset(pRoom, pReset, 0 /* Last slot*/);

      olevel = URANGE(0, to_mob->level - 2, LEVEL_HERO);
      newobj = create_object(pObjIndex, number_fuzzy(olevel));

      if (to_mob->pIndexData->pShop) /* Shop-keeper? */
      {
        switch (pObjIndex->item_type) {
        default:
          olevel = 0;
          break;
        case ITEM_POTION:
          olevel = number_range(0, 10);
          break;
        case ITEM_ARMOR:
          olevel = number_range(5, 15);
          break;
        case ITEM_WEAPON:
          if (pReset->command == 'G')
          olevel = number_range(5, 15);
          else
          olevel = number_fuzzy(olevel);
          break;
        }

        newobj = create_object(pObjIndex, olevel);
        if (pReset->arg2 == WEAR_NONE)
        SET_BIT(newobj->extra_flags, ITEM_INVENTORY);
      }
      else
      newobj = create_object(pObjIndex, number_fuzzy(olevel));

      obj_to_char(newobj, to_mob);
      if (pReset->command == 'E')
      equip_char(to_mob, newobj, pReset->arg3);

      sprintf(output, "%s (%d) has been loaded %s of %s (%d) and added to resets.\n\r", capitalize(pObjIndex->short_descr), pObjIndex->vnum, flag_string(wear_loc_strings, pReset->arg3), to_mob->short_descr, to_mob->pIndexData->vnum);
      send_to_char(output, ch);
    }
    else /* Display Syntax */
    {
      send_to_char("REdit:  That mobile isn't here.\n\r", ch);
      return FALSE;
    }

    act("$n has created $p!", ch, newobj, NULL, TO_ROOM);
    return TRUE;
  }

  char *get_cover_list(int ref) {
    int value = ref;
    char buf[MSL];
    //    char * word = str_dup(" ");
    char word[MSL];
    bool found = FALSE;

    sprintf(word, " ");
    if (value >= COVERS_HANDS) {
      value -= COVERS_HANDS;
      sprintf(buf, ", Hands");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_LOWER_ARMS) {
      value -= COVERS_LOWER_ARMS;
      sprintf(buf, ", Lower arms");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_UPPER_ARMS) {
      value -= COVERS_UPPER_ARMS;
      sprintf(buf, ", Upper arms");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_FEET) {
      value -= COVERS_FEET;
      sprintf(buf, ", Feet");
      strcat(word, buf);
      found = TRUE;
    }

    if (value >= COVERS_LOWER_LEGS) {
      value -= COVERS_LOWER_LEGS;
      sprintf(buf, ", Lower legs");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_FOREHEAD) {
      value -= COVERS_FOREHEAD;
      sprintf(buf, ", Forehead");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_THIGHS) {
      value -= COVERS_THIGHS;
      sprintf(buf, ", Thighs");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_GROIN) {
      value -= COVERS_GROIN;
      sprintf(buf, ", Groin");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_ARSE) {
      value -= COVERS_ARSE;
      sprintf(buf, ", Buttocks");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_LOWER_BACK) {
      value -= COVERS_LOWER_BACK;
      sprintf(buf, ", Lower Back");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_UPPER_BACK) {
      value -= COVERS_UPPER_BACK;
      sprintf(buf, ", Upper back");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_LOWER_CHEST) {
      value -= COVERS_LOWER_CHEST;
      sprintf(buf, ", Lower chest");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_BREASTS) {
      value -= COVERS_BREASTS;
      sprintf(buf, ", Breasts");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_UPPER_CHEST) {
      value -= COVERS_UPPER_CHEST;
      sprintf(buf, ", Upper Chest");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_NECK) {
      value -= COVERS_NECK;
      sprintf(buf, ", Neck");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_LOWER_FACE) {
      value -= COVERS_LOWER_FACE;
      sprintf(buf, ", Lower face");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_HAIR) {
      value -= COVERS_HAIR;
      sprintf(buf, ", Hair");
      strcat(word, buf);
      found = TRUE;
    }
    if (value >= COVERS_EYES) {
      value -= COVERS_EYES;
      sprintf(buf, ", Eyes");
      strcat(word, buf);
      found = TRUE;
    }

    if (found)
    return str_dup(word);
    else
    return " none";
  }

  /*
* Object Editor Functions.
*/
  void show_obj_values(CHAR_DATA *ch, OBJ_INDEX_DATA *obj) {
    char buf[MAX_STRING_LENGTH];

    switch (obj->item_type) {
    default: /* No values. */
      sprintf(buf, "[v0] :        [%d]\n\r[v1] :     [%d]\n\r[v2] :   [%d]\n\r[v3]: [%d]\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
      send_to_char(buf, ch);

      break;

    case ITEM_LIGHT:
      if (obj->value[2] == -1 || obj->value[2] == 999) /* ROM OLC */
      sprintf(buf, "[v2] Light:  Infinite[-1]\n\r");
      else
      sprintf(buf, "[v2] Light:  [%d]\n\r", obj->value[2]);
      send_to_char(buf, ch);
      break;

    case ITEM_KEYRING:
      sprintf(buf, "[v0] Bike:	%d\n\r", obj->value[0]);
      send_to_char(buf, ch);
      sprintf(buf, "[v1] Parked:	%d\n\r", obj->value[1]);
      send_to_char(buf, ch);
      break;

    case ITEM_PORTAL:
      sprintf(buf, "[v0] Charges:        [%d]\n\r[v1] Exit Flags:     %s\n\r[v2] Portal Flags:   %s\n\r[v3] Goes to (vnum): [%d]\n\r", obj->value[0], flag_string(exit_flags, obj->value[1]), flag_string(portal_flags, obj->value[2]), obj->value[3]);
      send_to_char(buf, ch);
      break;

    case ITEM_FURNITURE:
      sprintf(buf, "[v0] Max people:      [%d]\n\r[v1] Max weight:      [%d]\n\r[v2] Furniture Flags: %s\n\r[v3] Heal bonus:      [%d]\n\r[v4] Mana bonus:      [%d]\n\r", obj->value[0], obj->value[1], flag_string(furniture_flags, obj->value[2]), obj->value[3], obj->value[4]);
      send_to_char(buf, ch);
      break;

    case ITEM_POTION:
      sprintf(buf, "[v0] Level:  [%d]\n\r[v1] Spell:  %s\n\r[v2] Spell:  %s\n\r[v3] Spell:  %s\n\r[v4] Spell:  %s\n\r", obj->value[0], "none", "none", "none", "none");
      send_to_char(buf, ch);
      break;

      /* ARMOR for ROM */
    case ITEM_PHONE:
      sprintf(buf, "[v0] Number	[%d]\n\r[v1] Level	[%d]\n\r", obj->value[0], obj->value[1]);
      send_to_char(buf, ch);
      break;

    case ITEM_CLOTHING:
      sprintf(buf, "[v0] Covers %s\n\r[v1] ZipUps %s\n\r[v2] Shape %d \n\r", get_cover_list(obj->value[0]), get_cover_list(obj->value[1]), obj->value[2]);
      send_to_char(buf, ch);
      break;
    case ITEM_JEWELRY:
      sprintf(buf, "[v0] Unused %d\n\r[v1] Unused %d\n\r[v2] Unused %d\n\r[v3] Covers %s\n\r", obj->value[0], obj->value[1], obj->value[2], get_cover_list(obj->value[3]));
      send_to_char(buf, ch);
      break;
    case ITEM_RANGED:
      sprintf(buf, "[v0] Type       [%d]\n\r[v1] flags      [%d]\n\r[v2] Damage     [%d]\n\r[v3] Accuracy	 [%d]\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
      send_to_char(buf, ch);
      break;
    case ITEM_BLOODCRYSTAL:
      sprintf(buf, "[v0] Stat       [%d]\n\r[v1] Magnitude  [%d]\n\r[v2] Life	 [%d]\n\r", obj->value[0], obj->value[1], obj->value[2]);
      send_to_char(buf, ch);
      break;

    case ITEM_ARTIFACT:
      sprintf(buf, "[v0] Stat       [%d]\n\r[v1] Magnitude  [%d]\n\r", obj->value[0], obj->value[1]);
      send_to_char(buf, ch);
      break;

    case ITEM_ARMOR:
      sprintf(buf, "[v0] Endurance       [%d]\n\r[v1] Coverage        [%d]\n\r", obj->value[0], obj->value[1]);
      send_to_char(buf, ch);
      break;

      /* WEAPON changed in ROM: */
      /* I had to split the output here, I have no idea why, but it helped -- Hugin
*/
      /* It somehow fixed a bug in showing scroll/pill/potions too ?! */
    case ITEM_WEAPON:
      sprintf(buf, "[v0] Weapon class:   %s\n\r", flag_string(weapon_class, obj->value[0]));
      send_to_char(buf, ch);
      sprintf(buf, "[v1] Number of dice: [%d]\n\r", obj->value[1]);
      send_to_char(buf, ch);
      sprintf(buf, "[v2] Type of dice:   [%d]\n\r", obj->value[2]);
      send_to_char(buf, ch);
      sprintf(buf, "[v4] Special type:   %s\n\r", flag_string(weapon_type2, obj->value[4]));
      send_to_char(buf, ch);
      break;

    case ITEM_CONTAINER:
      sprintf(buf, "[v0] Capacity:   [%d]\n\r[v1] Flags:      [%s]\n\r[v2] Key:        [%d]\n\r[v3] Biggest object [%d]\n\r", obj->value[0], flag_string(container_flags, obj->value[1]), obj->value[2], obj->value[3]);
      send_to_char(buf, ch);
      break;

    case ITEM_BABY:
      sprintf(buf, "[v0] Age:        [%d]\n\r[v1] Flags:      [%s]\n\r[v2] Eye Color:  [%d]\n\r[v3] Hair Color: [%d]\n\r[v4] Skin Color: [%d]\n\r", obj->value[0], flag_string(sex_flags, obj->value[1]), obj->value[2], obj->value[3], obj->value[4]);
      send_to_char(buf, ch);
      break;

    case ITEM_DRINK_CON:
      sprintf(buf, "[v0] Liquid Total: [%d]\n\r[v1] Liquid Left:  [%d]\n\r[v2] Liquid:       %s\n\r[v3] Poisoned:     %s\n\r", obj->value[0], obj->value[1], liq_table[obj->value[2]].liq_name, obj->value[3] != 0 ? "Yes" : "No");
      send_to_char(buf, ch);
      break;

    case ITEM_FOUNTAIN:
      sprintf(buf, "[v0] Liquid Total: [%d]\n\r[v1] Liquid Left:  [%d]\n\r[v2] Liquid:	    %s\n\r", obj->value[0], obj->value[1], liq_table[obj->value[2]].liq_name);
      send_to_char(buf, ch);
      break;

    case ITEM_FOOD:
      sprintf(buf, "[v0] Food hours: [%d]\n\r[v3] Poisoned:   %s\n\r", obj->value[0], obj->value[3] != 0 ? "Yes" : "No");
      send_to_char(buf, ch);
      break;

    case ITEM_MONEY:
      sprintf(buf, "[v0] Amount:   [%d]\n\r", obj->value[0]);
      send_to_char(buf, ch);
      break;
    }

    return;
  }

  bool set_obj_values(CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument) {
    switch (pObj->item_type) {
    default:
      pObj->value[value_num] = atoi(argument);
      send_to_char("Set.\n\r", ch);
      break;

    case ITEM_LIGHT:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_LIGHT");
        return FALSE;
      case 2:
        send_to_char("HOURS OF LIGHT SET.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      }
      break;

    case ITEM_KEYRING:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_KEYRING");
        return FALSE;
      case 0:
        send_to_char("BIKE SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("PARKED SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      }
      break;

    case ITEM_POTION:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_SCROLL_POTION_PILL");
        return FALSE;
      case 0:
        send_to_char("WEAVE LEVEL SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("WEAVE TYPE 1 SET.\n\r\n\r", ch);
        break;
      case 2:
        send_to_char("WEAVE TYPE 2 SET.\n\r\n\r", ch);
        break;
      case 3:
        send_to_char("WEAVE TYPE 3 SET.\n\r\n\r", ch);
        break;
      case 4:
        send_to_char("WEAVE TYPE 4 SET.\n\r\n\r", ch);
        break;
      }
      break;

      /* ARMOR for ROM: */
    case ITEM_PHONE:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_PHONE");
        return FALSE;
      case 0:
        send_to_char("PHONENUMBER SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("LEVEL SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      }
      break;

    case ITEM_CLOTHING:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_CLOTHING");
        return FALSE;
      case 0:
        if (!str_cmp(argument, "hands"))
        pObj->value[0] += COVERS_HANDS;
        else if (!str_cmp(argument, "lower arms"))
        pObj->value[0] += COVERS_LOWER_ARMS;
        else if (!str_cmp(argument, "upper arms"))
        pObj->value[0] += COVERS_UPPER_ARMS;
        else if (!str_cmp(argument, "feet"))
        pObj->value[0] += COVERS_FEET;
        else if (!str_cmp(argument, "lower legs"))
        pObj->value[0] += COVERS_LOWER_LEGS;
        else if (!str_cmp(argument, "forehead"))
        pObj->value[0] += COVERS_FOREHEAD;
        else if (!str_cmp(argument, "thighs"))
        pObj->value[0] += COVERS_THIGHS;
        else if (!str_cmp(argument, "groin"))
        pObj->value[0] += COVERS_GROIN;
        else if (!str_cmp(argument, "buttocks"))
        pObj->value[0] += COVERS_ARSE;
        else if (!str_cmp(argument, "lower back"))
        pObj->value[0] += COVERS_LOWER_BACK;
        else if (!str_cmp(argument, "upper back"))
        pObj->value[0] += COVERS_UPPER_BACK;
        else if (!str_cmp(argument, "lower chest"))
        pObj->value[0] += COVERS_LOWER_CHEST;
        else if (!str_cmp(argument, "breasts"))
        pObj->value[0] += COVERS_BREASTS;
        else if (!str_cmp(argument, "upper chest"))
        pObj->value[0] += COVERS_UPPER_CHEST;
        else if (!str_cmp(argument, "neck"))
        pObj->value[0] += COVERS_NECK;
        else if (!str_cmp(argument, "lower face"))
        pObj->value[0] += COVERS_LOWER_FACE;
        else if (!str_cmp(argument, "hair"))
        pObj->value[0] += COVERS_HAIR;
        else if (!str_cmp(argument, "eyes"))
        pObj->value[0] += COVERS_EYES;
        else if (!str_cmp(argument, "clear"))
        pObj->value[0] = 0;
        else {
          send_to_char("Options are: hands, lower arms, upper arms, feet, lower legs, knees, thighs, groin, buttocks, lower back, upper back, lower chest, breasts, upper chest, neck, lower face, hair, eyes and clear\n\r", ch);
          break;
        }
        send_to_char("Coverage Set.\n\r\n\r", ch);
        break;
      case 1:
        if (!str_cmp(argument, "hands"))
        pObj->value[1] += COVERS_HANDS;
        else if (!str_cmp(argument, "lower arms"))
        pObj->value[1] += COVERS_LOWER_ARMS;
        else if (!str_cmp(argument, "upper arms"))
        pObj->value[1] += COVERS_UPPER_ARMS;
        else if (!str_cmp(argument, "feet"))
        pObj->value[1] += COVERS_FEET;
        else if (!str_cmp(argument, "lower legs"))
        pObj->value[1] += COVERS_LOWER_LEGS;
        else if (!str_cmp(argument, "forehead"))
        pObj->value[1] += COVERS_FOREHEAD;
        else if (!str_cmp(argument, "thighs"))
        pObj->value[1] += COVERS_THIGHS;
        else if (!str_cmp(argument, "groin"))
        pObj->value[1] += COVERS_GROIN;
        else if (!str_cmp(argument, "buttocks"))
        pObj->value[1] += COVERS_ARSE;
        else if (!str_cmp(argument, "lower back"))
        pObj->value[1] += COVERS_LOWER_BACK;
        else if (!str_cmp(argument, "upper back"))
        pObj->value[1] += COVERS_UPPER_BACK;
        else if (!str_cmp(argument, "lower chest"))
        pObj->value[1] += COVERS_LOWER_CHEST;
        else if (!str_cmp(argument, "breasts"))
        pObj->value[1] += COVERS_BREASTS;
        else if (!str_cmp(argument, "upper chest"))
        pObj->value[1] += COVERS_UPPER_CHEST;
        else if (!str_cmp(argument, "neck"))
        pObj->value[1] += COVERS_NECK;
        else if (!str_cmp(argument, "lower face"))
        pObj->value[1] += COVERS_LOWER_FACE;
        else if (!str_cmp(argument, "hair"))
        pObj->value[1] += COVERS_HAIR;
        else if (!str_cmp(argument, "eyes"))
        pObj->value[1] += COVERS_EYES;
        else if (!str_cmp(argument, "clear"))
        pObj->value[1] = 0;
        else {
          send_to_char("Options are: hands, lower arms, upper arms, feet, lower legs, knees, thighs, groin, buttocks, lower back, upper back, lower chest, breasts, upper chest, neck, lower face, hair, eyes and clear\n\r", ch);
          break;
        }
        send_to_char("Zip up Coverage Set.\n\r\n\r", ch);
        break;
      case 2:
        send_to_char("Shape.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      }
      break;

    case ITEM_RANGED:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_RANGED");
        return FALSE;
      case 0:
        send_to_char("TYPE SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("FLAGS SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      case 2:
        send_to_char("DAMAGE SET.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      case 3:
        send_to_char("ACCURACY SET.\n\r\n\r", ch);
        pObj->value[3] = atoi(argument);
        break;
      }
      break;

    case ITEM_BLOODCRYSTAL:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_AMULET");
        return FALSE;
      case 0:
        send_to_char("STAT SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("MAGNITUDE SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      case 2:
        send_to_char("LIFE SET.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      }
      break;

    case ITEM_ARTIFACT:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_ARTIFACT");
        return FALSE;
      case 0:
        send_to_char("STAT SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("MAGNITUDE SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      }
      break;

    case ITEM_ARMOR:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_ARMOR");
        return FALSE;
      case 0:
        send_to_char("ENDURANCE SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("COVERAGE SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      }
      break;

      /* WEAPONS changed in ROM */

    case ITEM_WEAPON:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_WEAPON");
        return FALSE;
      case 0:
        send_to_char("WEAPON CLASS SET.\n\r\n\r", ch);
        ALT_FLAGVALUE_SET(pObj->value[0], weapon_class, argument);
        break;
      case 1:
        send_to_char("NUMBER OF DICE SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      case 2:
        send_to_char("TYPE OF DICE SET.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      case 3:
        send_to_char("WEAPON TYPE SET.\n\r\n\r", ch);
        pObj->value[3] = attack_lookup(argument);
        break;
      case 4:
        send_to_char("SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch);
        ALT_FLAGVALUE_TOGGLE(pObj->value[4], weapon_type2, argument);
        break;
      }
      break;

    case ITEM_PORTAL:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_PORTAL");
        return FALSE;

      case 0:
        send_to_char("CHARGES SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("EXIT FLAGS SET.\n\r\n\r", ch);
        ALT_FLAGVALUE_SET(pObj->value[1], exit_flags, argument);
        break;
      case 2:
        send_to_char("PORTAL FLAGS SET.\n\r\n\r", ch);
        ALT_FLAGVALUE_SET(pObj->value[2], portal_flags, argument);
        break;
      case 3:
        send_to_char("EXIT VNUM SET.\n\r\n\r", ch);
        pObj->value[3] = atoi(argument);
        break;
      }
      break;
    case ITEM_FURNITURE:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_FURNITURE");
        return FALSE;

      case 0:
        send_to_char("NUMBER OF PEOPLE SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("MAX WEIGHT SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      case 2:
        send_to_char("FURNITURE FLAGS TOGGLED.\n\r\n\r", ch);
        ALT_FLAGVALUE_TOGGLE(pObj->value[2], furniture_flags, argument);
        break;
      case 3:
        send_to_char("HEAL BONUS SET.\n\r\n\r", ch);
        pObj->value[3] = atoi(argument);
        break;
      case 4:
        send_to_char("MANA BONUS SET.\n\r\n\r", ch);
        pObj->value[4] = atoi(argument);
        break;
      }
      break;

    case ITEM_CONTAINER:
      switch (value_num) {
        int value;

      default:
        do_function(ch, &do_help, "ITEM_CONTAINER");
        return FALSE;
      case 0:
        send_to_char("CAPACITY SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        if ((value = flag_value(container_flags, argument)) != NO_FLAG)
        TOGGLE_BIT(pObj->value[1], value);
        else {
          do_function(ch, &do_help, "ITEM_CONTAINER");
          return FALSE;
        }
        send_to_char("CONTAINER TYPE SET.\n\r\n\r", ch);
        break;
      case 2:
        send_to_char("COMBINATION SET.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      case 3:
        send_to_char("CONTAINER MAX OBJECT SIZE SET.\n\r", ch);
        pObj->value[3] = atoi(argument);
        break;
      case 4:
        send_to_char("WEIGHT MULTIPLIER SET.\n\r\n\r", ch);
        pObj->value[4] = atoi(argument);
        break;
      }
      break;

    case ITEM_BABY:
      switch (value_num) {
        int value;

      default:
        do_function(ch, &do_help, "ITEM_BABY");
        return FALSE;
      case 0:
        send_to_char("AGE SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        if ((value = flag_value(sex_flags, argument)) != NO_FLAG) {
          TOGGLE_BIT(pObj->value[1], value);
          send_to_char("SEX SET.\n\r\n\r", ch);
        }
        else {
          do_function(ch, &do_help, "ITEM_BABY");
          return FALSE;
        }
        break;
      case 2:
        send_to_char("Eye Color SET.\n\r\n\r", ch);
        pObj->value[2] = atoi(argument);
        break;
      case 3:
        send_to_char("Hair Color SET.\n\r", ch);
        pObj->value[3] = atoi(argument);
        break;
      case 4:
        send_to_char("Skin Color SET.\n\r\n\r", ch);
        pObj->value[4] = atoi(argument);
        break;
      }
      break;

    case ITEM_DRINK_CON:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_DRINK");
        return FALSE;
      case 0:
        send_to_char("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      case 2:
        send_to_char("LIQUID TYPE SET.\n\r\n\r", ch);
        pObj->value[2] = (liq_lookup(argument) != -1 ? liq_lookup(argument) : 0);
        break;
      case 3:
        send_to_char("POISON VALUE TOGGLED.\n\r\n\r", ch);
        pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
        break;
      }
      break;

    case ITEM_FOUNTAIN:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_FOUNTAIN");
        return FALSE;
      case 0:
        send_to_char("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch);
        pObj->value[1] = atoi(argument);
        break;
      case 2:
        send_to_char("LIQUID TYPE SET.\n\r\n\r", ch);
        pObj->value[2] = (liq_lookup(argument) != -1 ? liq_lookup(argument) : 0);
        break;
      }
      break;
    case ITEM_JEWELRY:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_FOUNTAIN");
        return FALSE;
      case 3:
        if (!str_cmp(argument, "hands"))
        pObj->value[3] += COVERS_HANDS;
        else if (!str_cmp(argument, "lower arms"))
        pObj->value[3] += COVERS_LOWER_ARMS;
        else if (!str_cmp(argument, "upper arms"))
        pObj->value[3] += COVERS_UPPER_ARMS;
        else if (!str_cmp(argument, "feet"))
        pObj->value[3] += COVERS_FEET;
        else if (!str_cmp(argument, "lower legs"))
        pObj->value[3] += COVERS_LOWER_LEGS;
        else if (!str_cmp(argument, "forehead"))
        pObj->value[3] += COVERS_FOREHEAD;
        else if (!str_cmp(argument, "thighs"))
        pObj->value[3] += COVERS_THIGHS;
        else if (!str_cmp(argument, "groin"))
        pObj->value[3] += COVERS_GROIN;
        else if (!str_cmp(argument, "buttocks"))
        pObj->value[3] += COVERS_ARSE;
        else if (!str_cmp(argument, "lower back"))
        pObj->value[3] += COVERS_LOWER_BACK;
        else if (!str_cmp(argument, "upper back"))
        pObj->value[3] += COVERS_UPPER_BACK;
        else if (!str_cmp(argument, "lower chest"))
        pObj->value[3] += COVERS_LOWER_CHEST;
        else if (!str_cmp(argument, "breasts"))
        pObj->value[3] += COVERS_BREASTS;
        else if (!str_cmp(argument, "upper chest"))
        pObj->value[3] += COVERS_UPPER_CHEST;
        else if (!str_cmp(argument, "neck"))
        pObj->value[3] += COVERS_NECK;
        else if (!str_cmp(argument, "lower face"))
        pObj->value[3] += COVERS_LOWER_FACE;
        else if (!str_cmp(argument, "hair"))
        pObj->value[3] += COVERS_HAIR;
        else if (!str_cmp(argument, "eyes"))
        pObj->value[3] += COVERS_EYES;
        else if (!str_cmp(argument, "clear"))
        pObj->value[3] = 0;
        else {
          send_to_char("Options are: hands, lower arms, upper arms, feet, lower legs, knees, thighs, groin, buttocks, lower back, upper back, lower chest, breasts, upper chest, neck, lower face, hair, eyes and clear\n\r", ch);
          break;
        }
        send_to_char("Coverage Set.\n\r\n\r", ch);
        break;
      }
      break;
    case ITEM_FOOD:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_FOOD");
        return FALSE;
      case 0:
        send_to_char("HOURS OF FOOD SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
        /*
case 1:
send_to_char( "HOURS OF FULL SET.\n\r\n\r", ch );
pObj->value[1] = atoi( argument );
break;
*/
      case 3:
        send_to_char("POISON VALUE TOGGLED.\n\r\n\r", ch);
        pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
        break;
      }
      break;
    case ITEM_PERFUME:
    case ITEM_FLASHLIGHT:
      switch (value_num) {
      default:
        return FALSE;
      case 0:
        send_to_char("USAGE SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      }
      break;
    case ITEM_MONEY:
      switch (value_num) {
      default:
        do_function(ch, &do_help, "ITEM_MONEY");
        return FALSE;
      case 0:
        send_to_char("AMOUNT SET.\n\r\n\r", ch);
        pObj->value[0] = atoi(argument);
        break;
      case 1:
        send_to_char("NATION SET.\n\r\n\r", ch);
        pObj->value[1] =
        (coin_lookup(argument) != -1 ? coin_lookup(argument) : 0);
        break;
      }
      break;
    }

    show_obj_values(ch, pObj);

    return TRUE;
  }

  OEDIT(oedit_statset) {
    OBJ_INDEX_DATA *pObj;
    int cnt = 0;
    int level = 0;

    EDIT_OBJ(ch, pObj);

    level = atoi(argument);
    if ((argument[0] = '\0') || (level == 0)) {
      send_to_char("Syntax: oedit statset level\n\r", ch);
      return FALSE;
    }

    if (pObj->item_type != ITEM_ARMOR) {
      send_to_char("You cannot use this command with non-armor items.\n\r", ch);
      return FALSE;
    }

    for (cnt = 0; cnt < 3; cnt++)
    pObj->value[cnt] = level / 2;

    pObj->value[3] = level / 3;
    pObj->level = level;
    send_to_char("Value flags toggled.\n\r", ch);

    return TRUE;
  }

  OEDIT(oedit_show) {
    OBJ_INDEX_DATA *pObj;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int cnt;

    EDIT_OBJ(ch, pObj);

    sprintf(buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r", pObj->name, !pObj->area ? -1 : pObj->area->vnum, !pObj->area ? "No Area" : pObj->area->name);
    send_to_char(buf, ch);

    sprintf(buf, "Vnum:        [%5d]\n\rType:        [%s]\n\r", pObj->vnum, flag_string(type_flags, pObj->item_type));
    send_to_char(buf, ch);

    if (pObj->item_type != ITEM_ARMOR) {
      sprintf(buf, "Level:       [%5d]\n\r", pObj->level);
      send_to_char(buf, ch);
    }
    else {
      sprintf(buf, "Level:       [%5d]      Fuzzy:     [%5d]\n\r", pObj->level, pObj->fuzzy);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Wear flags:  [%s]\n\r", flag_string(wear_flags, pObj->wear_flags));
    send_to_char(buf, ch);

    sprintf(buf, "Rot Timer:   [%d]\n\r", pObj->rot_timer);
    send_to_char(buf, ch);

    sprintf(buf, "Extra flags: [%s]\n\r", flag_string(extra_flags, pObj->extra_flags));
    send_to_char(buf, ch);

    sprintf(buf, "Load Chance: [%d]\n\r", pObj->load_chance);
    send_to_char(buf, ch);

    sprintf(buf, "Wear String:    [%s]\n\r", /* ROM */
    pObj->wear_string);
    send_to_char(buf, ch);

    sprintf(buf, "Material:    [%s]\n\r", /* ROM */
    pObj->material);
    send_to_char(buf, ch);

    sprintf(buf, "Condition:   [%5d]\n\r", /* ROM */
    pObj->condition);
    send_to_char(buf, ch);

    sprintf(buf, "Size:      [%5d]\n\rCost:        [%5d]\n\r", pObj->size, pObj->cost);
    send_to_char(buf, ch);

    if (pObj->extra_descr) {
      EXTRA_DESCR_DATA *ed;

      send_to_char("Ex desc kwd: ", ch);

      for (ed = pObj->extra_descr; ed; ed = ed->next) {
        send_to_char("[", ch);
        send_to_char(ed->keyword, ch);
        send_to_char("]", ch);
      }

      send_to_char("\n\r", ch);
    }

    sprintf(buf, "Short desc:  %s\n\rLong desc:\n\r     %s\n\r", pObj->short_descr, pObj->description);
    send_to_char(buf, ch);

    for (cnt = 0, paf = pObj->affected; paf; paf = paf->next) {
      if (cnt == 0) {
        send_to_char("Number Modifier Affects        Level\n\r", ch);
        send_to_char("------ -------- -------        -----\n\r", ch);
      }
      sprintf(buf, "[%4d] %-8d %-15s %d\n\r", cnt, paf->modifier, flag_string(apply_flags, paf->location), paf->level);
      send_to_char(buf, ch);
      cnt++;
    }

    show_obj_values(ch, pObj);

    return FALSE;
  }

  OEDIT(oedit_spec) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  spec [special function]\n\r", ch);
      return FALSE;
    }

    if (!str_cmp(argument, "none")) {
      pObj->spec_fun = NULL;

      send_to_char("Spec removed.\n\r", ch);
      return TRUE;
    }

    send_to_char("MEdit: No such special function.\n\r", ch);
    return FALSE;
  }

  /*
* Need to issue warning if flag isn't valid. -- does so now -- Hugin.
*/
  OEDIT(oedit_addaffect) {
    int value;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];
    char level[MSL];

    EDIT_OBJ(ch, pObj);

    argument = one_argument(argument, loc);
    argument = one_argument(argument, mod);
    one_argument(argument, level);

    if (loc[0] == '\0' || mod[0] == '\0' || !is_number(mod) || (!is_number(level) && level[0] != '\0')) {
      send_to_char("Syntax:  addaffect [location] [#xmod] [level(optional)]\n\r", ch);
      return FALSE;
    }

    if ((value = flag_value(apply_flags, loc)) == NO_FLAG) /* Hugin */
    {
      send_to_char("Valid affects are:\n\r", ch);
      show_help(ch, "apply");
      return FALSE;
    }

    pAf = new_affect();
    pAf->location = value;
    pAf->modifier = atoi(mod);
    pAf->where = TO_OBJECT;
    pAf->type = -1;
    pAf->duration = -1;
    pAf->bitvector = 0;
    pAf->level = (level[0] != '\0') ? atoi(level) : pObj->level;
    pAf->next = pObj->affected;
    pObj->affected = pAf;

    send_to_char("Affect added.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_addapply) {
    int value, bv, typ;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];
    char type[MAX_STRING_LENGTH];
    char bvector[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument(argument, type);
    argument = one_argument(argument, loc);
    argument = one_argument(argument, mod);
    one_argument(argument, bvector);

    if (type[0] == '\0' || (typ = flag_value(apply_types, type)) == NO_FLAG) {
      send_to_char("Invalid apply type. Valid apply types are:\n\r", ch);
      show_help(ch, "apptype");
      return FALSE;
    }

    if (loc[0] == '\0' || (value = flag_value(apply_flags, loc)) == NO_FLAG) {
      send_to_char("Valid applys are:\n\r", ch);
      show_help(ch, "apply");
      return FALSE;
    }

    if (bvector[0] == '\0' || (bv = flag_value(bitvector_type[typ].table, bvector)) == NO_FLAG) {
      send_to_char("Invalid bitvector type.\n\r", ch);
      send_to_char("Valid bitvector types are:\n\r", ch);
      show_help(ch, bitvector_type[typ].help);
      return FALSE;
    }

    if (mod[0] == '\0' || !is_number(mod)) {
      send_to_char("Syntax:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch);
      return FALSE;
    }

    pAf = new_affect();
    pAf->location = value;
    pAf->modifier = atoi(mod);
    pAf->where = apply_types[typ].bit;
    pAf->type = -1;
    pAf->duration = -1;
    pAf->bitvector = bv;
    pAf->level = pObj->level;
    pAf->next = pObj->affected;
    pObj->affected = pAf;

    send_to_char("Apply added.\n\r", ch);
    return TRUE;
  }

  /*
* My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
* for really teaching me how to manipulate pointers.
*/
  OEDIT(oedit_delaffect) {
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument(argument, affect);

    if (!is_number(affect) || affect[0] == '\0') {
      send_to_char("Syntax:  delaffect [#xaffect]\n\r", ch);
      return FALSE;
    }

    value = atoi(affect);

    if (value < 0) {
      send_to_char("Only non-negative affect-numbers allowed.\n\r", ch);
      return FALSE;
    }

    if (!(pAf = pObj->affected)) {
      send_to_char("OEdit:  Non-existant affect.\n\r", ch);
      return FALSE;
    }

    if (value == 0) /* First case: Remove first affect */
    {
      pAf = pObj->affected;
      pObj->affected = pAf->next;
      free_affect(pAf);
    }
    else /* Affect to remove is not the first */
    {
      while ((pAf_next = pAf->next) && (++cnt < value))
      pAf = pAf_next;

      if (pAf_next) /* See if it's the next affect */
      {
        pAf->next = pAf_next->next;
        free_affect(pAf_next);
      }
      else /* Doesn't exist */
      {
        send_to_char("No such affect.\n\r", ch);
        return FALSE;
      }
    }

    send_to_char("Affect removed.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_name) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  name [string]\n\r", ch);
      return FALSE;
    }

    free_string(pObj->name);
    pObj->name = str_dup(argument);

    send_to_char("Name set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_wearstring) {
    OBJ_INDEX_DATA *pObj;
    char buf[MSL];
    // char temp[MSL];
    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
      strcpy(buf, "");
      // send_to_char( "Syntax:  short [string]\n\r", ch );
      // return FALSE;
    }
    else {
      sprintf(buf, "%s", argument);
      // sprintf(buf, "%-21.21s", buf);
    }

    free_string(pObj->wear_string);
    pObj->wear_string = str_dup(buf);

    send_to_char("Wear string set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_short) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  short [string]\n\r", ch);
      return FALSE;
    }

    free_string(pObj->short_descr);
    pObj->short_descr = str_dup(argument);
    //    pObj->short_descr[0] = LOWER( pObj->short_descr[0] );

    send_to_char("Short description set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_long) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  long [string]\n\r", ch);
      return FALSE;
    }

    free_string(pObj->description);
    pObj->description = str_dup(argument);
    // pObj->description[0] = UPPER( pObj->description[0] );

    send_to_char("Long description set.\n\r", ch);
    return TRUE;
  }

  bool set_value(CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value) {
    if (argument[0] == '\0') {
      set_obj_values(ch, pObj, -1, ""); /* '\0' changed to "" -- Hugin */
      return FALSE;
    }

    if (set_obj_values(ch, pObj, value, argument))
    return TRUE;

    return FALSE;
  }

  /*****************************************************************************
Name:		oedit_values
Purpose:	Finds the object and sets its value.
Called by:	The four valueX functions below. (now five -- Hugin )
****************************************************************************/
  bool oedit_values(CHAR_DATA *ch, char *argument, int value) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (set_value(ch, pObj, argument, value))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_value0) {
    if (oedit_values(ch, argument, 0))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_value1) {
    if (oedit_values(ch, argument, 1))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_value2) {
    if (oedit_values(ch, argument, 2))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_value3) {
    if (oedit_values(ch, argument, 3))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_value4) {
    if (oedit_values(ch, argument, 4))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_value5) {
    if (oedit_values(ch, argument, 5))
    return TRUE;

    return FALSE;
  }

  OEDIT(oedit_copy) {
    OBJ_INDEX_DATA *pObj;
    OBJ_INDEX_DATA *pObj2;
    int vnum, cnt;

    EDIT_OBJ(ch, pObj);

    vnum = atoi(argument);
    if ((argument[0] = '\0') || (vnum == 0)) {
      send_to_char("Syntax: oedit copy (vnum)\n\r", ch);
      return FALSE;
    }

    if (!(pObj2 = get_obj_index(vnum))) {
      send_to_char("REdit:  That object does not exist.\n\r", ch);
      return FALSE;
    }

    pObj->item_type = pObj2->item_type;
    pObj->wear_flags = pObj2->wear_flags;
    pObj->rot_timer = pObj2->rot_timer;
    pObj->extra_flags = pObj2->extra_flags;
    free_string(pObj->material);
    pObj->material = str_dup(pObj2->material);
    pObj->size = pObj2->size;

    for (cnt = 0; cnt < 6; cnt++)
    pObj->value[cnt] = pObj2->value[cnt];

    return TRUE;
  }
  OEDIT(oedit_size) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  size [number]\n\r", ch);
      return FALSE;
    }

    pObj->size = atoi(argument);

    send_to_char("Size set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_cost) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  cost [number]\n\r", ch);
      return FALSE;
    }

    pObj->cost = atoi(argument);

    send_to_char("Cost set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_create) {
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int value;
    int iHash;

    value = atoi(argument);
    if (argument[0] == '\0' || value == 0) {
      send_to_char("Syntax:  oedit create [vnum]\n\r", ch);
      return FALSE;
    }

    pArea = get_vnum_area(value);
    if (!pArea) {
      send_to_char("OEdit:  That vnum is not assigned an area.\n\r", ch);
      return FALSE;
    }

    if (get_obj_index(value)) {
      send_to_char("OEdit:  Object vnum already exists.\n\r", ch);
      return FALSE;
    }

    pObj = new_obj_index();
    pObj->vnum = value;
    pObj->area = pArea;

    if (value > top_vnum_obj)
    top_vnum_obj = value;

    iHash = value % MAX_KEY_HASH;
    pObj->next = obj_index_hash[iHash];
    obj_index_hash[iHash] = pObj;
    ch->desc->pEdit = (void *)pObj;

    send_to_char("Object Created.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_ed) {
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument(argument, command);
    one_argument(argument, keyword);

    if (command[0] == '\0') {
      send_to_char("Syntax:  ed add [keyword]\n\r", ch);
      send_to_char("         ed delete [keyword]\n\r", ch);
      send_to_char("         ed edit [keyword]\n\r", ch);
      send_to_char("         ed format [keyword]\n\r", ch);
      return FALSE;
    }

    if (!str_cmp(command, "add")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed add [keyword]\n\r", ch);
        return FALSE;
      }

      ed = new_extra_descr();
      ed->keyword = str_dup(keyword);
      ed->next = pObj->extra_descr;
      pObj->extra_descr = ed;

      string_append(ch, &ed->description);

      return TRUE;
    }

    if (!str_cmp(command, "edit")) {
      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed edit [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pObj->extra_descr; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
      }

      if (!ed) {
        send_to_char("OEdit:  Extra description keyword not found.\n\r", ch);
        return FALSE;
      }

      string_append(ch, &ed->description);

      return TRUE;
    }

    if (!str_cmp(command, "delete")) {
      EXTRA_DESCR_DATA *ped = NULL;

      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed delete [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pObj->extra_descr; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
        ped = ed;
      }

      if (!ed) {
        send_to_char("OEdit:  Extra description keyword not found.\n\r", ch);
        return FALSE;
      }

      if (!ped)
      pObj->extra_descr = ed->next;
      else
      ped->next = ed->next;

      free_extra_descr(ed);

      send_to_char("Extra description deleted.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "format")) {

      if (keyword[0] == '\0') {
        send_to_char("Syntax:  ed format [keyword]\n\r", ch);
        return FALSE;
      }

      for (ed = pObj->extra_descr; ed; ed = ed->next) {
        if (is_name(keyword, ed->keyword))
        break;
      }

      if (!ed) {
        send_to_char("OEdit:  Extra description keyword not found.\n\r", ch);
        return FALSE;
      }

      ed->description = format_string(ed->description);

      send_to_char("Extra description formatted.\n\r", ch);
      return TRUE;
    }

    oedit_ed(ch, "");
    return FALSE;
  }

  /* ROM object functions : */

  OEDIT(oedit_extra) /* Moved out of oedit() due to naming conflicts -- Hugin */
  {
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0') {
      EDIT_OBJ(ch, pObj);

      if ((value = flag_value(extra_flags, argument)) != NO_FLAG) {
        TOGGLE_BIT(pObj->extra_flags, value);

        send_to_char("Extra flag toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax:  extra [flag]\n\rType '? extra' for a list of flags.\n\r", ch);
    return FALSE;
  }

  OEDIT(oedit_wear) /* Moved out of oedit() due to naming conflicts -- Hugin */
  {
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0') {
      EDIT_OBJ(ch, pObj);

      if ((value = flag_value(wear_flags, argument)) != NO_FLAG) {
        TOGGLE_BIT(pObj->wear_flags, value);

        send_to_char("Wear flag toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax:  wear [flag]\n\rType '? wear' for a list of flags.\n\r", ch);
    return FALSE;
  }

  OEDIT(
  oedit_rottimer) /* Moved out of oedit() due to naming conflicts -- Hugin */
  {
    OBJ_INDEX_DATA *pObj;

    if (argument[0] != '\0' && is_number(argument) && atoi(argument) >= -1 && atoi(argument) <= 100) {
      EDIT_OBJ(ch, pObj);
      pObj->rot_timer = atoi(argument);
      send_to_char("Rot timer set.\n\r", ch);
      return TRUE;
    }

    send_to_char("Syntax:  rottimer [number(1-100)]\n\r", ch);
    return FALSE;
  }

  OEDIT(oedit_type) /* Moved out of oedit() due to naming conflicts -- Hugin */
  {
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0') {
      EDIT_OBJ(ch, pObj);

      if ((value = flag_value(type_flags, argument)) != NO_FLAG) {
        pObj->item_type = value;

        send_to_char("Type set.\n\r", ch);

        /*
* Clear the values.
*/
        pObj->value[0] = 0;
        pObj->value[1] = 0;
        pObj->value[2] = 0;
        pObj->value[3] = 0;
        pObj->value[4] = 0; /* ROM */

        return TRUE;
      }
    }

    send_to_char("Syntax:  type [flag]\n\rType '? type' for a list of flags.\n\r", ch);
    return FALSE;
  }

  OEDIT(oedit_material) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    /*
if ( argument[0] == '\0' )
{
send_to_char( "Syntax:  material [string]\n\r", ch );
return FALSE;
}
*/
    free_string(pObj->material);
    pObj->material = str_dup(argument);

    send_to_char("Material set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_lchance) {
    OBJ_INDEX_DATA *pObj;
    int chance;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  loadchance [number]\n\r", ch);
      return FALSE;
    }

    chance = atoi(argument);
    if (chance < 1 || chance > 100) {
      send_to_char("Load chance must be in the range 1-100.\n\r", ch);
      return FALSE;
    }

    pObj->load_chance = chance;

    send_to_char("Load chance set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_level) {
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  level [number]\n\r", ch);
      return FALSE;
    }

    pObj->level = atoi(argument);

    send_to_char("Level set.\n\r", ch);
    return TRUE;
  }

  /*
*	This function toggles the value for the fuzzy setting
*	on a object.  This will create a random level on the object
*      upon being loaded.  Also the acs will differ based on the
*      new random level according to oedit_statset.
*			- Palin October 29, 2001
*/
  OEDIT(oedit_fuzzy) {
    OBJ_INDEX_DATA *pObj;
    int flevel = 0;
    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  level [number]\n\r", ch);
      return FALSE;
    }

    if (pObj->item_type != ITEM_ARMOR) {
      send_to_char("You cannot use this command with non-armor items.\n\r", ch);
      return FALSE;
    }

    flevel = atoi(argument);

    // Doesn't make any sense to have a fuzzy less than zero.
    if (flevel < 0 || flevel > 9) {
      send_to_char("Fuzzy level must be greater than zero and less than ten.\n\r", ch);
      return FALSE;
    }

    // We don't want the object to be below 0 or above level ML + 10. */
    if (((pObj->level - flevel) < 1) || ((pObj->level + flevel) > (MAX_LEVEL + 10))) {
      send_to_char("Fuzzy level must be between 1 and Max Level.\n\r", ch);
      return FALSE;
    }

    pObj->fuzzy = atoi(argument);

    send_to_char("Level set.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_condition) {
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0' && (value = atoi(argument)) >= 0 && (value <= 100)) {
      EDIT_OBJ(ch, pObj);

      pObj->condition = value;
      send_to_char("Condition set.\n\r", ch);

      return TRUE;
    }

    send_to_char("Syntax:  condition [number]\n\rWhere number can range from 0 (ruined) to 100 (perfect).\n\r", ch);
    return FALSE;
  }
  int get_mlvl(MOB_INDEX_DATA *ch) {
    int i, max = 0, maxpoint = 0, nextmax = 0, nextmaxpoint = 0, level = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range < 0)
      continue;
      if (ch->disciplines[discipline_table[i].vnum] > max) {
        max = ch->disciplines[discipline_table[i].vnum];
        maxpoint = discipline_table[i].vnum;
      }
    }
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range < 0)
      continue;
      if (discipline_table[i].vnum == maxpoint)
      continue;
      if (ch->disciplines[discipline_table[i].vnum] > nextmax) {
        nextmax = ch->disciplines[discipline_table[i].vnum];
        nextmaxpoint = discipline_table[i].vnum;
      }
    }
    level = ch->disciplines[maxpoint] + ch->disciplines[nextmaxpoint] / 4;

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      level += ch->disciplines[discipline_table[i].vnum];
    }
    level -= 10;
    level += ch->intelligence;

    return level;
  }

  int get_demon_lvl_mob(MOB_INDEX_DATA *pMobIndex) {
    int value = 0;
    value = mob_power_score(pMobIndex);
    value /= 2000;
    int speed = pMobIndex->intelligence + 20;
    speed /= 3;
    value = value * speed / 10;
    value += 4;
    return value;
  }

  /*
* Mobile Editor Functions.
*/
  MEDIT(medit_show) {
    MOB_INDEX_DATA *pMob;
    char buf[MAX_STRING_LENGTH];

    EDIT_MOB(ch, pMob);

    sprintf(buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r", pMob->player_name, !pMob->area ? -1 : pMob->area->vnum, !pMob->area ? "No Area" : pMob->area->name);
    send_to_char(buf, ch);

    sprintf(buf, "Act:         [%s]\n\r", act_bit_name_array(pMob->act));
    send_to_char(buf, ch);

    sprintf(buf, "Vnum:        [%5d] Sex:   [%s]   Race: [%s]\n\r", pMob->vnum, pMob->sex == SEX_MALE ? "male   " : pMob->sex == SEX_FEMALE
    ? "female " : pMob->sex == 3 ? "random " : "neutral", race_table[pMob->race].name);
    send_to_char(buf, ch);

    sprintf(buf, "TTL: [%d]       Wounds: [%d]      Speed: [%d]\n\r", pMob->ttl, pMob->maxwounds, pMob->intelligence);
    send_to_char(buf, ch);

    sprintf(buf, "Level:       [%2d]\n\r", pMob->level);
    send_to_char(buf, ch);

    if (pMob->group) {
      sprintf(buf, "Group:       [%5d]\n\r", pMob->group);
      send_to_char(buf, ch);
    }

    sprintf(buf, "Affected by: [%s]\n\r", affect_bit_name_array(pMob->affected_by));
    send_to_char(buf, ch);

    /* ROM values: */

    sprintf(buf, "Start pos.   [%s]\n\r", flag_string(position_flags, pMob->start_pos));
    send_to_char(buf, ch);

    sprintf(buf, "Default pos  [%s]\n\r", flag_string(position_flags, pMob->default_pos));
    send_to_char(buf, ch);

    sprintf(buf, "Wealth:      [%5ld]\n\r", pMob->wealth);
    send_to_char(buf, ch);

    sprintf(buf, "Lvl:  [%d] DLvl: [%d]\n\r", get_mlvl(pMob), get_demon_lvl_mob(pMob));
    send_to_char(buf, ch);

    for (int b = 0; b < MAX_DIS; b++) {
      if (pMob->disciplines[b] > 0) {
        for (int i = 0; i < DIS_USED; i++) {
          if (discipline_table[i].vnum == b) {
            sprintf(buf, "%s: %d\n\r", discipline_table[i].name, pMob->disciplines[b]);
            send_to_char(buf, ch);
          }
        }
      }
    }

    /* ROM values end */

    sprintf(buf, "Short descr: %s\n\rLong descr:\n\r%s", pMob->short_descr, pMob->long_descr);
    send_to_char(buf, ch);

    sprintf(buf, "Description:\n\r%s", pMob->description);
    send_to_char(buf, ch);

    if (pMob->pShop) {
      SHOP_DATA *pShop;
      int iTrade;

      pShop = pMob->pShop;

      sprintf(buf, "Shop data for [%5d]:\n\r  Markup for purchaser: %d%%\n\r  Markdown for seller:  %d%%\n\r", pShop->keeper, pShop->profit_buy, pShop->profit_sell);
      send_to_char(buf, ch);
      sprintf(buf, "  Hours: %d to %d.\n\r", pShop->open_hour, pShop->close_hour);
      send_to_char(buf, ch);
      sprintf(buf, "  Proceeds: %d%%.\n\r", pShop->proceed);
      send_to_char(buf, ch);
      for (iTrade = 0; iTrade < MAX_TRADE; iTrade++) {
        if (pShop->buy_type[iTrade] != 0) {
          if (iTrade == 0) {
            send_to_char("  Number Trades Type\n\r", ch);
            send_to_char("  ------ -----------\n\r", ch);
          }
          sprintf(buf, "  [%4d] %s\n\r", iTrade, flag_string(type_flags, pShop->buy_type[iTrade]));
          send_to_char(buf, ch);
        }
      }
    }

    return FALSE;
  }

  MEDIT(medit_target) {
    ROOM_INDEX_DATA *pRoom = NULL;
    MOB_INDEX_DATA *pMob;

    char arg[MSL];
    int room_value = -1, slot_value = -1;
    EDIT_MOB(ch, pMob);

    argument = one_argument(argument, arg);

    if (!is_number(arg) || !is_number(argument)) {
      send_to_char("Syntax: target (which slot) (vnum)\n\r", ch);
      return FALSE;
    }

    room_value = atoi(argument);
    slot_value = atoi(arg);

    if (slot_value < 0 || slot_value > 9 || ((pRoom = get_room_index(room_value)) == NULL)) {
      send_to_char("Syntax: target (which slot) (vnum)\n\r", ch);
      return FALSE;
    }

    pMob->targeting[slot_value] = pRoom;
    send_to_char("Set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_statset) {
    int level = 0;
    char buf[MAX_STRING_LENGTH];
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    level = atoi(argument);
    if ((argument[0] = '\0') || (level == 0)) {
      send_to_char("Syntax: medit statset level\n\r", ch);
      return FALSE;
    }

    //    if(level <= 90)
    //    {
    pMob->level = level;

    sprintf(buf, "Mob stats set for a normal level %d", level);
    send_to_char(buf, ch);
    return TRUE;
    /*    }
if(level > 90)
{
pMob->level             = level;
pMob->hitroll           = level + (level /2);
pMob->hit[DICE_NUMBER]  = level + (level /2);
pMob->hit[DICE_TYPE]    = 25;
pMob->hit[DICE_BONUS]   = (level * 60);
pMob->mana[DICE_NUMBER] = level;
pMob->mana[DICE_TYPE]   = 20;
pMob->mana[DICE_BONUS]  = (level * 55);
pMob->damage[DICE_NUMBER] = (( level/8 ) + 1);
pMob->damage[DICE_TYPE] = 6;
pMob->damage[DICE_BONUS] = level;
pMob->ac[AC_PIERCE]     = ((level * 18) /10);
pMob->ac[AC_BASH]       = ((level * 18) /10);
pMob->ac[AC_SLASH]      = ((level * 18) /10 );
pMob->ac[AC_EXOTIC]     = ((level * 15) /10 );

sprintf(buf, "Mob stats set for a normal level %d \nRemember level 90+
mobs are now a bit stronger on average", level); send_to_char( buf, ch );
return TRUE;
}
*/
    return TRUE;
  }

  MEDIT(medit_create) {
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int value;
    int iHash;

    value = atoi(argument);
    if (argument[0] == '\0' || value == 0) {
      send_to_char("Syntax:  medit create [vnum]\n\r", ch);
      return FALSE;
    }

    pArea = get_vnum_area(value);

    if (!pArea) {
      send_to_char("MEdit:  That vnum is not assigned an area.\n\r", ch);
      return FALSE;
    }

    if (get_mob_index(value)) {
      send_to_char("MEdit:  Mobile vnum already exists.\n\r", ch);
      return FALSE;
    }

    pMob = new_mob_index();
    pMob->vnum = value;
    pMob->area = pArea;

    if (value > top_vnum_mob)
    top_vnum_mob = value;

    SET_FLAG(pMob->act, ACT_IS_NPC);
    iHash = value % MAX_KEY_HASH;
    pMob->next = mob_index_hash[iHash];
    mob_index_hash[iHash] = pMob;
    ch->desc->pEdit = (void *)pMob;

    send_to_char("Mobile Created.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_spec) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  spec [special function]\n\r", ch);
      return FALSE;
    }

    if (!str_cmp(argument, "none")) {
      pMob->spec_fun = NULL;

      send_to_char("Spec removed.\n\r", ch);
      return TRUE;
    }

    send_to_char("MEdit: No such special function.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_wounds) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  wounds [number]\n\r", ch);
      return FALSE;
    }

    pMob->maxwounds = atoi(argument);

    send_to_char("Wounds set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_intelligence) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  intelligence [number]\n\r", ch);
      return FALSE;
    }

    pMob->intelligence = atoi(argument);

    send_to_char("Intelligence set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_weapon) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  weapon none/sword/spear/fist/dagger/mace/flail/staff/polearm/axe\n\r", ch);
      return FALSE;
    }
    if (!str_cmp(argument, "None"))
    pMob->weapon_type = -1;
    else if (!str_cmp(argument, "Sword"))
    pMob->weapon_type = WEAPON_SWORD;
    else if (!str_cmp(argument, "spear"))
    pMob->weapon_type = WEAPON_SPEAR;
    else if (!str_cmp(argument, "Fist"))
    pMob->weapon_type = WEAPON_FIST;
    else if (!str_cmp(argument, "dagger"))
    pMob->weapon_type = WEAPON_DAGGER;
    else if (!str_cmp(argument, "Mace"))
    pMob->weapon_type = WEAPON_MACE;
    else if (!str_cmp(argument, "Flail"))
    pMob->weapon_type = WEAPON_FLAIL;
    else if (!str_cmp(argument, "Staff"))
    pMob->weapon_type = WEAPON_STAFF;
    else if (!str_cmp(argument, "Polearm"))
    pMob->weapon_type = WEAPON_POLEARM;
    else if (!str_cmp(argument, "Axe"))
    pMob->weapon_type = WEAPON_AXE;
    else {
      send_to_char("Syntax:  weapon none/sword/spear/fist/dagger/mace/flail/staff/polearm/axe\n\r", ch);
      return FALSE;
    }

    send_to_char("Weapon set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_ttl) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  ttl [number]\n\r", ch);
      return FALSE;
    }

    pMob->ttl = atoi(argument);

    send_to_char("TTL set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_level) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  level [number]\n\r", ch);
      return FALSE;
    }

    pMob->level = atoi(argument);

    send_to_char("Level set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_desc) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      string_append(ch, &pMob->description);
      return TRUE;
    }

    send_to_char("Syntax:  desc    - line edit\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_long) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  long [string]\n\r", ch);
      return FALSE;
    }

    free_string(pMob->long_descr);
    strcat(argument, "\n\r");
    pMob->long_descr = str_dup(argument);
    pMob->long_descr[0] = UPPER(pMob->long_descr[0]);

    send_to_char("Long description set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_short) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  short [string]\n\r", ch);
      return FALSE;
    }

    free_string(pMob->short_descr);
    pMob->short_descr = str_dup(argument);

    send_to_char("Short description set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_name) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  name [string]\n\r", ch);
      return FALSE;
    }

    free_string(pMob->player_name);
    pMob->player_name = str_dup(argument);

    send_to_char("Name set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_shop) {
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);
    argument = one_argument(argument, arg1);

    EDIT_MOB(ch, pMob);

    if (command[0] == '\0') {
      send_to_char("Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch);
      send_to_char("         shop profit [#xbuying%] [#xselling%]\n\r", ch);
      send_to_char("         shop type [#x0-4] [item type]\n\r", ch);
      send_to_char("         shop assign\n\r", ch);
      send_to_char("         shop remove\n\r", ch);
      if (IS_ADMIN(ch)) {
        send_to_char("		shop owner [player]", ch);
        send_to_char("		shop proceed [value]", ch);
      }
      return FALSE;
    }

    if (!str_cmp(command, "hours")) {
      if (arg1[0] == '\0' || !is_number(arg1) || argument[0] == '\0' || !is_number(argument)) {
        send_to_char("Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch);
        return FALSE;
      }

      if (!pMob->pShop) {
        send_to_char("MEdit:  Debes crear un shop primero (shop assign).\n\r", ch);
        return FALSE;
      }

      pMob->pShop->open_hour = atoi(arg1);
      pMob->pShop->close_hour = atoi(argument);

      send_to_char("Shop hours set.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "proceed") && IS_ADMIN(ch)) {
      if (arg1[0] == '\0' || !is_number(arg1)) {
        send_to_char("Syntax: shop proceed (amount)", ch);
        return FALSE;
      }

      pMob->pShop->proceed = atoi(arg1);
      send_to_char("You have toggled proceeds of this shop.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "owner") && IS_ADMIN(ch)) {
      if (arg1[0] == '\0') {
        send_to_char("Syntax: shop owner [guild]\n\r", ch);
        return FALSE;
      }

      return TRUE;
    }

    if (!str_cmp(command, "profit")) {
      if (arg1[0] == '\0' || !is_number(arg1) || argument[0] == '\0' || !is_number(argument)) {
        send_to_char("Syntax:  shop profit [#xbuying%] [#xselling%]\n\r", ch);
        return FALSE;
      }

      if (!pMob->pShop) {
        send_to_char("MEdit:  Debes crear un shop primero (shop assign).\n\r", ch);
        return FALSE;
      }

      pMob->pShop->profit_buy = atoi(arg1);
      pMob->pShop->profit_sell = atoi(argument);

      send_to_char("Shop profit set.\n\r", ch);
      return TRUE;
    }

    if (!str_cmp(command, "type")) {
      char buf[MAX_INPUT_LENGTH];
      int value;

      if (arg1[0] == '\0' || !is_number(arg1) || argument[0] == '\0') {
        send_to_char("Syntax:  shop type [#x0-4] [item type]\n\r", ch);
        return FALSE;
      }

      if (atoi(arg1) >= MAX_TRADE) {
        sprintf(buf, "MEdit:  May sell %d items max.\n\r", MAX_TRADE);
        send_to_char(buf, ch);
        return FALSE;
      }

      if (!pMob->pShop) {
        send_to_char("MEdit:  Debes crear un shop primero (shop assign).\n\r", ch);
        return FALSE;
      }

      if ((value = flag_value(type_flags, argument)) == NO_FLAG) {
        send_to_char("MEdit:  That type of item is not known.\n\r", ch);
        return FALSE;
      }

      pMob->pShop->buy_type[atoi(arg1)] = value;

      send_to_char("Shop type set.\n\r", ch);
      return TRUE;
    }

    /* shop assign && shop delete by Phoenix */

    if (!str_prefix(command, "assign")) {
      if (pMob->pShop) {
        send_to_char("Mob already has a shop assigned to it.\n\r", ch);
        return FALSE;
      }

      pMob->pShop = new_shop();
      shop_list.push_back(pMob->pShop);
      pMob->pShop->keeper = pMob->vnum;

      send_to_char("New shop assigned to mobile.\n\r", ch);
      return TRUE;
    }

    if (!str_prefix(command, "remove")) {
      SHOP_DATA *pShop;

      pShop = pMob->pShop;
      pMob->pShop = NULL;

      shop_list.remove(pShop);
      free_shop(pShop);

      send_to_char("Mobile is no longer a shopkeeper.\n\r", ch);
      return TRUE;
    }

    medit_shop(ch, "");
    return FALSE;
  }

  /* ROM medit functions: */

  MEDIT(medit_speed) /* Moved out of medit() due to naming conflicts -- Hugin */
  {
    MOB_INDEX_DATA *pMob;
    int value;

    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      value = atoi(argument);
      pMob->intelligence = value;
      send_to_char("Speed set.\n\r", ch);
      return TRUE;
    }
    send_to_char("Syntax: Speed [0-20]\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_sex) /* Moved out of medit() due to naming conflicts -- Hugin */
  {
    MOB_INDEX_DATA *pMob;
    int value;

    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      if ((value = flag_value(sex_flags, argument)) != NO_FLAG) {
        pMob->sex = value;

        send_to_char("Sex set.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax: sex [sex]\n\rType '? sex' for a list of flags.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_act) /* Moved out of medit() due to naming conflicts -- Hugin */
  {
    MOB_INDEX_DATA *pMob;
    SET value;

    SET_INIT(value);
    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      if (set_flag_value(act_flags, argument, value)) {
        SET_XOR_SET(pMob->act, value);
        SET_FLAG(pMob->act, ACT_IS_NPC);

        send_to_char("Act flag toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax: act [flag]\n\rType '? act' for a list of flags.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_affect) /* Moved out of medit() due to naming conflicts -- Hugin */
  {
    MOB_INDEX_DATA *pMob;
    SET value;

    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      if (set_flag_value(affect_flags_bit, argument, value)) {
        SET_XOR_SET(pMob->affected_by, value);

        send_to_char("Affect flag toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax: affect [flag]\n\rType '? affect' for a list of flags.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_form) {
    MOB_INDEX_DATA *pMob;
    int value;

    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      if ((value = flag_value(form_flags, argument)) != NO_FLAG) {
        if (!str_cmp(argument, "none"))
        pMob->form = 0;
        else
        pMob->form ^= value;
        send_to_char("Form toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax: form [flags]\n\rType '? form' for a list of flags.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_part) {
    MOB_INDEX_DATA *pMob;
    int value;

    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      if ((value = flag_value(part_flags, argument)) != NO_FLAG) {
        pMob->parts ^= value;
        send_to_char("Parts toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax: part [flags]\n\rType '? part' for a list of flags.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_material) {
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  material [string]\n\r", ch);
      return FALSE;
    }

    free_string(pMob->material);
    pMob->material = str_dup(argument);

    send_to_char("Material set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_off) {
    MOB_INDEX_DATA *pMob;
    int value;

    if (argument[0] != '\0') {
      EDIT_MOB(ch, pMob);

      if ((value = flag_value(off_flags, argument)) != NO_FLAG) {
        pMob->off_flags ^= value;
        send_to_char("Offensive behaviour toggled.\n\r", ch);
        return TRUE;
      }
    }

    send_to_char("Syntax: off [flags]\n\rType '? off' for a list of flags.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_race) {
    MOB_INDEX_DATA *pMob;
    int race;

    if (argument[0] != '\0' && (race = race_lookup(argument)) != -1) {
      EDIT_MOB(ch, pMob);

      pMob->race = race;

      send_to_char("Race set.\n\r", ch);
      return TRUE;
    }

    if (argument[0] == '?') {
      char buf[MAX_STRING_LENGTH];

      send_to_char("Available races are:", ch);

      for (race = 0; race < MAX_RACES; race++) {
        if ((race % 3) == 0)
        send_to_char("\n\r", ch);
        sprintf(buf, " %-15s", race_table[race].name);
        send_to_char(buf, ch);
      }

      send_to_char("\n\r", ch);
      return FALSE;
    }

    send_to_char("Syntax:  race [race]\n\rType 'race ?' for a list of races.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_position) {
    MOB_INDEX_DATA *pMob;
    char arg[MAX_INPUT_LENGTH];
    int value;

    argument = one_argument(argument, arg);

    switch (arg[0]) {
    default:
      break;

    case 'S':
    case 's':
      if (str_prefix(arg, "start"))
      break;

      if ((value = flag_value(position_flags, argument)) == NO_FLAG)
      break;

      EDIT_MOB(ch, pMob);

      pMob->start_pos = value;
      send_to_char("Start position set.\n\r", ch);
      return TRUE;

    case 'D':
    case 'd':
      if (str_prefix(arg, "default"))
      break;

      if ((value = flag_value(position_flags, argument)) == NO_FLAG)
      break;

      EDIT_MOB(ch, pMob);

      pMob->default_pos = value;
      send_to_char("Default position set.\n\r", ch);
      return TRUE;
    }

    send_to_char("Syntax:  position [start/default] [position]\n\rType '? position' for a list of positions.\n\r", ch);
    return FALSE;
  }

  MEDIT(medit_gold) {
    MOB_INDEX_DATA *pMob;
    int wealth = 0;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0' || !is_number(argument)) {
      send_to_char("Syntax:  wealth [number]\n\r", ch);
      return FALSE;
    }

    wealth = atoi(argument);

    if (wealth < 0 || wealth >= 100) {
      send_to_char("That is not an acceptable wealth setting.  Here is the correct list:\n\n\r", ch);
      do_function(ch, &do_help, "wealthtypes");
      return FALSE;
    }

    pMob->wealth = wealth;

    send_to_char("Wealth set.\n\r", ch);
    return TRUE;
  }

  void show_liqlist(CHAR_DATA *ch) {
    int liq;
    Buffer outbuf;
    char buf[MAX_STRING_LENGTH];

    for (liq = 0; liq_table[liq].liq_name != NULL; liq++) {
      if ((liq % 21) == 0)
      outbuf.strcat("Name                 Color          Proof Full Thirst Food Ssize\n\r");

      sprintf(buf, "%-20s %-14s %5d %4d %6d %4d %5d\n\r", liq_table[liq].liq_name, liq_table[liq].liq_color, liq_table[liq].liq_affect[0], liq_table[liq].liq_affect[1], liq_table[liq].liq_affect[2], liq_table[liq].liq_affect[3], liq_table[liq].liq_affect[4]);
      outbuf.strcat(buf);
    }

    page_to_char(outbuf, ch);
    return;
  }

  void show_coinlist(CHAR_DATA *ch) {}

  void show_damlist(CHAR_DATA *ch) {}

  MEDIT(medit_group) {
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *pMTemp;
    char arg[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int temp;
    Buffer outbuf;
    bool found = FALSE;

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax: group [number]\n\r", ch);
      send_to_char("        group show [number]\n\r", ch);
      return FALSE;
    }

    if (is_number(argument)) {
      pMob->group = atoi(argument);
      send_to_char("Group set.\n\r", ch);
      return TRUE;
    }

    argument = one_argument(argument, arg);

    if (!strcmp(arg, "show") && is_number(argument)) {
      if (atoi(argument) == 0) {
        send_to_char("Are you crazy?\n\r", ch);
        return FALSE;
      }

      for (temp = 0; temp < 65536; temp++) {
        pMTemp = get_mob_index(temp);
        if (pMTemp && (pMTemp->group == atoi(argument))) {
          found = TRUE;
          sprintf(buf, "[%5d] %s\n\r", pMTemp->vnum, pMTemp->player_name);
          outbuf.strcat(buf);
        }
      }

      if (found)
      page_to_char(outbuf, ch);
      else
      send_to_char("No mobs in that group.\n\r", ch);

      return FALSE;
    }

    return FALSE;
  }

  REDIT(redit_owner) {
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  owner [owner]\n\r", ch);
      send_to_char("         owner none\n\r", ch);
      return FALSE;
    }

    free_string(pRoom->owner);
    if (!str_cmp(argument, "none"))
    pRoom->owner = str_dup("");
    else
    pRoom->owner = str_dup(argument);

    send_to_char("Owner set.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_discipline) {
    MOB_INDEX_DATA *pMob;
    char arg1[MSL], arg2[MSL], arg3[MSL], buf[MSL];

    EDIT_MOB(ch, pMob);

    if (argument[0] == '\0') {
      send_to_char("Syntax:  discipline [name] [number]\n\r", ch);
      return FALSE;
    }
    argument = one_argument_nouncap(argument, arg1);
    for (int i = 0; i < DIS_USED; i++) {
      if (!str_cmp(arg1, discipline_table[i].name)) {
        pMob->disciplines[discipline_table[i].vnum] = atoi(argument);
        send_to_char("Disciplines set.\n\r", ch);
        return TRUE;
      }
    }
    argument = one_argument_nouncap(argument, arg2);
    sprintf(buf, "%s %s", arg1, arg2);
    for (int i = 0; i < DIS_USED; i++) {
      if (!str_cmp(buf, discipline_table[i].name)) {
        pMob->disciplines[discipline_table[i].vnum] = atoi(argument);
        send_to_char("Disciplines set.\n\r", ch);
        return TRUE;
      }
    }
    argument = one_argument_nouncap(argument, arg3);
    sprintf(buf, "%s %s %s", arg1, arg2, arg3);
    for (int i = 0; i < DIS_USED; i++) {
      if (!str_cmp(buf, discipline_table[i].name)) {
        pMob->disciplines[discipline_table[i].vnum] = atoi(argument);
        send_to_char("Disciplines set.\n\r", ch);
        return TRUE;
      }
    }

    return FALSE;
  }

  MEDIT(medit_addmprog) {
    int value;
    MOB_INDEX_DATA *pMob;
    PROG_LIST *list;
    PROG_CODE *code;
    char trigger[MAX_STRING_LENGTH];
    char phrase[MAX_STRING_LENGTH];
    char num[MAX_STRING_LENGTH];

    EDIT_MOB(ch, pMob);
    argument = one_argument(argument, num);
    argument = one_argument(argument, trigger);
    argument = one_argument(argument, phrase);

    if (!is_number(num) || trigger[0] == '\0' || phrase[0] == '\0') {
      send_to_char("Syntax:   addmprog [vnum] [trigger] [phrase]\n\r", ch);
      return FALSE;
    }

    if ((value = flag_value(mprog_flags, trigger)) == NO_FLAG) {
      send_to_char("Valid flags are:\n\r", ch);
      show_help(ch, "mprog");
      return FALSE;
    }

    if ((code = get_prog_index(atoi(num), PRG_MPROG)) == NULL) {
      send_to_char("No such MOBProgram.\n\r", ch);
      return FALSE;
    }

    list = new_mprog();
    list->vnum = atoi(num);
    list->trig_type = value;
    list->trig_phrase = str_dup(phrase);
    list->code = code->code;
    SET_BIT(pMob->mprog_flags, value);
    list->next = pMob->mprogs;
    pMob->mprogs = list;

    send_to_char("Mprog Added.\n\r", ch);
    return TRUE;
  }

  MEDIT(medit_delmprog) {
    MOB_INDEX_DATA *pMob;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char mprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_MOB(ch, pMob);

    one_argument(argument, mprog);
    if (!is_number(mprog) || mprog[0] == '\0') {
      send_to_char("Syntax:  delmprog [#mprog]\n\r", ch);
      return FALSE;
    }

    value = atoi(mprog);

    if (value < 0) {
      send_to_char("Only non-negative mprog-numbers allowed.\n\r", ch);
      return FALSE;
    }

    if (!(list = pMob->mprogs)) {
      send_to_char("MEdit:  Non existant mprog.\n\r", ch);
      return FALSE;
    }

    if (value == 0) {
      REMOVE_BIT(pMob->mprog_flags, pMob->mprogs->trig_type);
      list = pMob->mprogs;
      pMob->mprogs = list->next;
      free_mprog(list);
    }
    else {
      while ((list_next = list->next) && (++cnt < value))
      list = list_next;

      if (list_next) {
        REMOVE_BIT(pMob->mprog_flags, list_next->trig_type);
        list->next = list_next->next;
        free_mprog(list_next);
      }
      else {
        send_to_char("No such mprog.\n\r", ch);
        return FALSE;
      }
    }

    send_to_char("Mprog removed.\n\r", ch);
    return TRUE;
  }

  REDIT(redit_room) {
    ROOM_INDEX_DATA *room;
    int value;

    EDIT_ROOM(ch, room);

    if ((value = flag_value(room_flags, argument)) == NO_FLAG) {
      send_to_char("Sintaxis: room [flags]\n\r", ch);
      return FALSE;
    }

    TOGGLE_BIT(room->room_flags, value);
    send_to_char("Room flags toggled.\n\r", ch);
    return TRUE;
  }

  REDIT(redit_feature) {
    ROOM_INDEX_DATA *room;
    int i;
    char arg1[MSL];
    char arg2[MSL];
    EDIT_ROOM(ch, room);

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!str_cmp(arg1, "add")) {
      bool found = FALSE;
      for (i = 0; i < 10 && found == FALSE; i++) {
        if (!room->features[i]) {
          room->features[i] = new_feature();
          found = TRUE;
          send_to_char("Done.\n\r", ch);
        }
      }
    }
    else if (!str_cmp(arg1, "type")) {
      i = atoi(arg2);
      if (i < 0 || i > 9)
      return FALSE;

      room->features[i]->type = atoi(argument);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "position")) {
      i = atoi(arg2);
      if (i < 0 || i > 9)
      return FALSE;

      room->features[i]->position = atoi(argument);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "names")) {
      i = atoi(arg2);
      if (i < 0 || i > 9)
      return FALSE;

      free_string(room->features[i]->names);
      room->features[i]->names = str_dup(argument);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "shortdesc")) {
      i = atoi(arg2);
      if (i < 0 || i > 9)
      return FALSE;

      free_string(room->features[i]->shortdesc);
      room->features[i]->shortdesc = str_dup(argument);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "desc")) {
      i = atoi(arg2);
      if (i < 0 || i > 9)
      return FALSE;

      string_append(ch, &room->features[i]->desc);

      send_to_char("Done.\n\r", ch);
    }
    return TRUE;
  }
  REDIT(redit_coordinates) {
    ROOM_INDEX_DATA *room;
    char arg[MSL];
    char arg2[MSL];
    argument = one_argument(argument, arg);
    argument = one_argument(argument, arg2);

    EDIT_ROOM(ch, room);

    room->x = atoi(arg);
    room->y = atoi(arg2);
    room->z = atoi(argument);
    send_to_char("Coordinates set.\n\r", ch);

    return TRUE;
  }

  REDIT(redit_xy) {
    ROOM_INDEX_DATA *room;
    int value, valuetwo;
    char arg[MSL];
    argument = one_argument(argument, arg);

    EDIT_ROOM(ch, room);
    value = atoi(argument);
    if (!is_number(argument) || value < -1000 || value > 1000) {
      send_to_char("Enter a value between 0 and 1000.\n\r", ch);
      return FALSE;
    }
    valuetwo = atoi(arg);
    if (!is_number(arg) || valuetwo < -1000 || valuetwo > 1000) {
      send_to_char("Enter a value between 0 and 1000.\n\r", ch);
      return FALSE;
    }

    room->locx = value;
    room->locy = valuetwo;
    send_to_char("X-Y set.\n\r", ch);

    return TRUE;
  }
  REDIT(redit_entry) {
    ROOM_INDEX_DATA *room;
    int value, valuetwo;
    char arg[MSL];
    argument = one_argument(argument, arg);

    EDIT_ROOM(ch, room);
    value = atoi(argument);
    if (!is_number(argument) || value < 0 || value > 1000) {
      send_to_char("Enter a value between 0 and 1000.\n\r", ch);
      return FALSE;
    }
    valuetwo = atoi(arg);
    if (!is_number(arg) || valuetwo < 0 || valuetwo > 1000) {
      send_to_char("Enter a value between 0 and 1000.\n\r", ch);
      return FALSE;
    }

    room->entryx = value;
    room->entryy = valuetwo;
    send_to_char("Entry set.\n\r", ch);

    return TRUE;
  }
  REDIT(redit_size) {
    ROOM_INDEX_DATA *room;
    int value;

    EDIT_ROOM(ch, room);
    value = atoi(argument);
    if (!is_number(argument) || value < 0 || value > 1000) {
      send_to_char("Enter a value between 0 and 1000.\n\r", ch);
      return FALSE;
    }

    room->size = value;
    send_to_char("Size set.\n\r", ch);

    return TRUE;
  }

  REDIT(redit_sector) {
    ROOM_INDEX_DATA *room;
    int value;

    EDIT_ROOM(ch, room);

    if ((value = flag_value(sector_flags, argument)) == NO_FLAG) {
      send_to_char("Sintaxis: sector [tipo]\n\r", ch);
      return FALSE;
    }

    room->sector_type = value;
    send_to_char("Sector type set.\n\r", ch);

    return TRUE;
  }

  REDIT(redit_sectorset) {
    ROOM_INDEX_DATA *pRoomIndex;
    char arg[MAX_INPUT_LENGTH];
    char arg1[MIL];
    bool found;
    int vnum;
    int value, value1, value2;

    argument = one_argument(argument, arg);
    argument = one_argument(argument, arg1);

    found = FALSE;

    if (!is_number(arg1) || !is_number(argument)) {
      send_to_char("Syntax: sectorsec (sector) (lvnum) (uvnum)\n\r", ch);
      return FALSE;
    }

    value1 = atoi(arg1);
    value2 = atoi(argument);

    if ((value = flag_value(sector_flags, arg)) == NO_FLAG) {
      send_to_char("Syntax: sectorset (sector) (lvnum) (uvnum)\n\r", ch);
      return FALSE;
    }

    if (value1 > value2) {
      send_to_char("Your lvnum cannot exceed your uvnum.\n\r", ch);
      send_to_char("Syntax: sectorset (sector) (lvnum) (uvnum)\n\r", ch);
      return FALSE;
    }

    if (value1 < ch->in_room->area->min_vnum || value2 > ch->in_room->area->max_vnum) {

      send_to_char("You may not edit sectors outside of the area you are in.\n\r", ch);
      send_to_char("Syntax: sectorset (sector) (lvnum) (uvnum)\n\r", ch);
      return FALSE;
    }

    for (vnum = value1; vnum <= value2; vnum++) {
      if ((pRoomIndex = get_room_index(vnum))) {
        pRoomIndex->sector_type = value;
        found = TRUE;
      }
    }

    if (!found) {
      send_to_char("Room(s) not found in this area.\n\r", ch);
      return FALSE;
    }

    send_to_char("Done.\n\r", ch);
    return FALSE;
  }

  OEDIT(oedit_addoprog) {
    int value;
    OBJ_INDEX_DATA *pObj;
    PROG_LIST *list;
    PROG_CODE *code;
    char trigger[MAX_STRING_LENGTH];
    char phrase[MAX_STRING_LENGTH];
    char num[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);
    argument = one_argument(argument, num);
    argument = one_argument(argument, trigger);
    argument = one_argument(argument, phrase);

    if (!is_number(num) || trigger[0] == '\0' || phrase[0] == '\0') {
      send_to_char("Syntax:   addoprog [vnum] [trigger] [phrase]\n\r", ch);
      return FALSE;
    }

    if ((value = flag_value(oprog_flags, trigger)) == NO_FLAG) {
      send_to_char("Valid flags are:\n\r", ch);
      show_help(ch, "oprog");
      return FALSE;
    }

    if ((code = get_prog_index(atoi(num), PRG_OPROG)) == NULL) {
      send_to_char("No such OBJProgram.\n\r", ch);
      return FALSE;
    }

    list = new_oprog();
    list->vnum = atoi(num);
    list->trig_type = value;
    list->trig_phrase = str_dup(phrase);
    list->code = code->code;
    SET_BIT(pObj->oprog_flags, value);
    list->next = pObj->oprogs;
    pObj->oprogs = list;

    send_to_char("Oprog Added.\n\r", ch);
    return TRUE;
  }

  OEDIT(oedit_deloprog) {
    OBJ_INDEX_DATA *pObj;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char oprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument(argument, oprog);
    if (!is_number(oprog) || oprog[0] == '\0') {
      send_to_char("Syntax:  deloprog [#oprog]\n\r", ch);
      return FALSE;
    }

    value = atoi(oprog);

    if (value < 0) {
      send_to_char("Only non-negative oprog-numbers allowed.\n\r", ch);
      return FALSE;
    }

    if (!(list = pObj->oprogs)) {
      send_to_char("OEdit:  Non existant oprog.\n\r", ch);
      return FALSE;
    }

    if (value == 0) {
      REMOVE_BIT(pObj->oprog_flags, pObj->oprogs->trig_type);
      list = pObj->oprogs;
      pObj->oprogs = list->next;
      free_oprog(list);
    }
    else {
      while ((list_next = list->next) && (++cnt < value))
      list = list_next;

      if (list_next) {
        REMOVE_BIT(pObj->oprog_flags, list_next->trig_type);
        list->next = list_next->next;
        free_oprog(list_next);
      }
      else {
        send_to_char("No such oprog.\n\r", ch);
        return FALSE;
      }
    }

    send_to_char("Oprog removed.\n\r", ch);
    return TRUE;
  }

  REDIT(redit_addrprog) {
    int value;
    ROOM_INDEX_DATA *pRoom;
    PROG_LIST *list;
    PROG_CODE *code;
    char trigger[MAX_STRING_LENGTH];
    char phrase[MAX_STRING_LENGTH];
    char num[MAX_STRING_LENGTH];

    EDIT_ROOM(ch, pRoom);
    argument = one_argument(argument, num);
    argument = one_argument(argument, trigger);
    argument = one_argument(argument, phrase);

    if (!is_number(num) || trigger[0] == '\0' || phrase[0] == '\0') {
      send_to_char("Syntax:   addrprog [vnum] [trigger] [phrase]\n\r", ch);
      return FALSE;
    }

    if ((value = flag_value(rprog_flags, trigger)) == NO_FLAG) {
      send_to_char("Valid flags are:\n\r", ch);
      show_help(ch, "rprog");
      return FALSE;
    }

    if ((code = get_prog_index(atoi(num), PRG_RPROG)) == NULL) {
      send_to_char("No such ROOMProgram.\n\r", ch);
      return FALSE;
    }

    list = new_rprog();
    list->vnum = atoi(num);
    list->trig_type = value;
    list->trig_phrase = str_dup(phrase);
    list->code = code->code;
    SET_BIT(pRoom->rprog_flags, value);
    list->next = pRoom->rprogs;
    pRoom->rprogs = list;

    send_to_char("Rprog Added.\n\r", ch);
    return TRUE;
  }

  REDIT(redit_delrprog) {
    ROOM_INDEX_DATA *pRoom;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char rprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_ROOM(ch, pRoom);

    one_argument(argument, rprog);
    if (!is_number(rprog) || rprog[0] == '\0') {
      send_to_char("Syntax:  delrprog [#rprog]\n\r", ch);
      return FALSE;
    }

    value = atoi(rprog);

    if (value < 0) {
      send_to_char("Only non-negative rprog-numbers allowed.\n\r", ch);
      return FALSE;
    }

    if (!(list = pRoom->rprogs)) {
      send_to_char("REdit:  Non existant rprog.\n\r", ch);
      return FALSE;
    }

    if (value == 0) {
      REMOVE_BIT(pRoom->rprog_flags, pRoom->rprogs->trig_type);
      list = pRoom->rprogs;
      pRoom->rprogs = list->next;
      free_rprog(list);
    }
    else {
      while ((list_next = list->next) && (++cnt < value))
      list = list_next;

      if (list_next) {
        REMOVE_BIT(pRoom->rprog_flags, list_next->trig_type);
        list->next = list_next->next;
        free_rprog(list_next);
      }
      else {
        send_to_char("No such rprog.\n\r", ch);
        return FALSE;
      }
    }

    send_to_char("Rprog removed.\n\r", ch);
    return TRUE;
  }

  /**
* This is the hub command for locating resets.  It enables someone to search
* for a reset of a mob/what objects are reset to it, or to search for specific
* resets for an object.
*/
  _DOFUN(do_locate_reset) {
    char arg1[MSL];
    char arg2[MSL];
    int vnum = 0;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (strcmp(arg1, "mob") && strcmp(arg1, "object") && strcmp(arg1, "mobj")) {
      send_to_char("Syntax: lreset (mob/object/mobj) (vnum)\n\r", ch);
      return;
    }

    if (!is_number(arg2)) {
      send_to_char("That is not an acceptable value for a vnum.\n\rSyntax: lreset (mob/object/mobj) (vnum)\n\r", ch);
      return;
    }

    vnum = atoi(arg2);

    if (!strcmp(arg1, "mob"))
    locate_mob_reset(ch, vnum, FALSE);

    if (!strcmp(arg1, "mobj"))
    locate_mob_reset(ch, vnum, TRUE);

    return;
  }

  /**
* This code locates a specific mob.  One should notice that it also calls for
* 'equipment' if the person specified wanting to search for that in the above
* with mobj.
*/
  void locate_mob_reset(CHAR_DATA *ch, int vnum, bool equip_list) {
    Buffer outbuf;
    RESET_DATA *pReset = NULL;
    ROOM_INDEX_DATA *pRoom = NULL;
    MOB_INDEX_DATA *pMob = NULL;
    char buf[MSL];
    char buf2[MSL];
    char bonus_stuff[MSL];
    int location = 0;
    bool found_mob = FALSE;

    /* If this vnum is not assigned a mob then kick out */
    if (!(pMob = get_mob_index(vnum))) {
      send_to_char("That particular mobile doesn't appear to exist.\n\r", ch);
      return;
    }

    /* Don't want color from short descrs */
    remove_color(buf2, pMob->short_descr);

    /* Send the null printout of the mobs name/statistics that don't repeat */
    sprintf(
    buf, "You have searched for Mobile %-20s `W(`Y%5d`W) `RLevel`W: `G%3d`x\n\r", pMob->short_descr, pMob->vnum, pMob->level);
    outbuf.strcat(buf);

    sprintf(buf, "%-7s          %-11s          %-12s\n\n\r", "`W[`xRoom`W]`x", "`W(`xMax`W)-(`xMin`W)`x", "Bonuses");
    outbuf.strcat(buf);

    /* We have to look through all the vnums in the game - these are under 30000
* for now. */
    for (location = 1; location <= 30000; location++) {
      if (!(pRoom = get_room_index(location)))
      continue;

      /* now loop through all the resets inside of the current room we are in */
      for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
        MOB_INDEX_DATA *pMobIndex = NULL;
        ROOM_INDEX_DATA *pRoomIndex = NULL;

        bonus_stuff[0] = '\0';
        if (pReset->command == 'M') {
          if (!(pMobIndex = get_mob_index(pReset->arg1)))
          continue;

          if (!(pRoomIndex = get_room_index(pReset->arg3)))
          continue;

          if (pMobIndex->vnum != vnum)
          continue;

          /* The following section is for special features on a mob.  Like we want
* to know if its a healer or what not. */
          if (pMob->pShop)
          strcat(bonus_stuff, "`BS`x");

          if (IS_FLAG(pMob->act, ACT_SENTINEL))
          strcat(bonus_stuff, "`CM`x");

          /* End Special Section */

          sprintf(buf, "[`B%5d`x]          (`W%3d`x)-(`W%3d`x)         (%-12s)\n\r", pRoom->vnum, pReset->arg2, pReset->arg4, bonus_stuff);

          outbuf.strcat(buf);

          if (equip_list) {
            RESET_DATA *pReset2 = NULL;
            sprintf(buf, "`R-`x    %-7s          %-15s                %-8s\n\r", "`W[Vnum`W]", "Short", "Wear Loc");
            outbuf.strcat(buf);

            /* now loop through all the resets inside of the current room we are
* in */
            for (pReset2 = pRoom->reset_first; pReset2; pReset2 = pReset2->next) {
              MOB_INDEX_DATA *pMobIndex = NULL;
              OBJ_INDEX_DATA *pObjIndex = NULL;

              pMobIndex = get_mob_index(pReset2->arg1);
              if (!found_mob) {
                if (!pMobIndex)
                continue;

                if (pMobIndex == pMob)
                found_mob = TRUE;
                else
                found_mob = FALSE;
              }

              if (pMobIndex)
              if (pMobIndex != pMob)
              found_mob = FALSE;

              if (found_mob) {
                if (!(pObjIndex = get_obj_index(pReset2->arg1)))
                continue;

                remove_color(buf2, pObjIndex->short_descr);

                if (pReset2->command == 'E') {
                  sprintf(buf, "     [%5d]          %-15s                %-8s\n\r", pReset2->arg1, buf2, (pReset2->command == 'G')
                  ? flag_string(wear_loc_strings, WEAR_NONE)
                  : flag_string(wear_loc_strings, pReset2->arg3));

                  outbuf.strcat(buf);
                }
              }
            }
          }
        }
      }
    }
    send_to_char(outbuf, ch);
    return;
  }

  REDIT(redit_create) {
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int value;
    int iHash;

    EDIT_ROOM(ch, pRoom);

    value = atoi(argument);

    if (argument[0] == '\0' || value <= 0) {
      send_to_char("Syntax:  create [vnum > 0]\n\r", ch);
      return FALSE;
    }

    pArea = get_vnum_area(value);
    if (!pArea) {
      send_to_char("REdit:  That vnum is not assigned an area.\n\r", ch);
      return FALSE;
    }

    if (get_room_index(value)) {
      send_to_char("REdit:  Room vnum already exists.\n\r", ch);
      return FALSE;
    }

    pRoom = new_room_index();
    pRoom->area = pArea;
    pRoom->vnum = value;

    if (value > top_vnum_room)
    top_vnum_room = value;

    iHash = value % MAX_KEY_HASH;
    pRoom->next = room_index_hash[iHash];
    room_index_hash[iHash] = pRoom;
    ch->desc->pEdit = (void *)pRoom;

    send_to_char("Room created.\n\r", ch);
    return TRUE;
  }

  /*
From: Christopher Feist <Christopher.Feist.feist@nt.com>

Recently I have been working on OLC delete commands since they would
be really handy.

I have come up with working versions of oedit_delete
and medit_delete. redit_delete is still in the works and I dont
think im going to bother with aedit_delete.

The basic premise is that you remove the vnum from the appropriate
hash and then delete all resets which refer to said vnum. This is
of course much easier to say then to do. The tricky part is
getting resets for objects inside other objects. Both of the
delete functions handle these cases.

I specificaly do not free up the index data because doing that
would require me to eliminate every instance of said vnum from
the mud. (because pIndexData still points to this memory location)
This isnt too bad for mobiles but for objects it really
really bites. If someone wants to code it up ill gladly take it
however. :)

I am submitting the two delete functions so that other muds besides
Aeon can test them and maybe force out any small bugs that I havent
caught. I STRONGLY suggest you read and understand what is happening
before you put these in. You will also have to add the hooks in
to call the functions yourself. (That should prevent most copy/paste
implementors from doing dumb things :P)

If you are trying to understand what is happening here I reccomend
uncommenting the debug code in oedit delete as it spews alot less
output then the medit delete code. I admit that neither of these
routines are particularily elegant coding examples however they
do work.

Lastly on Aeon there is a small glitch in medit_delete whereby
the person deleting the mobile somehow has his mount set to
a predictable value sometime after the exit of the function.
(i.e its something in our code not the function)

Unfortunatly I havent been able to figureout how/why/where this happens.
If anyone knows how to break on the change of the value
of a dereferenced pointer in gdb an example would be REALLY REALLY
appreciated. For those of you who have muds without mount code
(or with mount code) you will have to get rid of that little
section at the bottom of medit_delete since our mount code
isnt taken from a stock snippet.

Enjoy.

Narbo

Coder @ Aeon
telnet://mud.aeon.org:4000
*/

  OEDIT(oedit_delete) {
    OBJ_INDEX_DATA *pObj;
    OBJ_INDEX_DATA *iObj;
    OBJ_INDEX_DATA *sObj;
    RESET_DATA *pReset = NULL;
    RESET_DATA *prev = NULL;
    ROOM_INDEX_DATA *pRoom = NULL;
    char arg[MIL];
    char buf[MSL];
    int index, count, iHash, i;

    if (argument[0] == '\0') {
      send_to_char("Syntax:  oedit delete [vnum]\n\r", ch);
      return FALSE;
    }

    one_argument(argument, arg);

    if (is_number(arg)) {
      index = atoi(arg);
      pObj = get_obj_index(index);
    }
    else {
      send_to_char("That is not a number.\n\r", ch);
      return FALSE;
    }

    SET_BIT(pObj->area->area_flags, AREA_CHANGED);

    /* Remove it from the object list */

    iHash = index % MAX_KEY_HASH;

    /* DEBUG CODE - uncomment this if you have doubts */
    /* printf("\nObject hash for location %d:\n", iHash);
for ( tObj = obj_index_hash[iHash]; tObj != NULL; tObj = tObj->next )
printf("name: %s vnum: %d\n", tObj->name, tObj->vnum ); */

    sObj = obj_index_hash[iHash];

    if (sObj->next == NULL) /* only entry */
    obj_index_hash[iHash] = NULL;
    else if (sObj == pObj) /* first entry */
    obj_index_hash[iHash] = pObj->next;
    else /* everything else */
    {
      for (iObj = sObj; iObj != NULL; iObj = iObj->next) {
        if (iObj == pObj) {
          sObj->next = pObj->next;
          break;
        }
        sObj = iObj;
      }
    }

    /* If you uncomment this you also need to
find every instance of the object that exists in
the mud and extract them otherwise each of thier
pIndexData will be pointing at free memory.
(Which may or may not contain the actual info)
As it is all the objects will be removed the reboot/login
automatically by fread_obj when it cant find the index */

    /* free_string( pObj->name );
free_string( pObj->short_descr );
free_string( pObj->description );

for( pAf = pObj->affected; pAf; pAf = pAf->next )
free_affect( pAf );

for( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next )
free_extra_descr( pExtra );

free( pObj ); */

    /* DEBUG CODE - uncomment this if you have doubts */
    /* printf("\nObject hash for location %d after removal:\n", iHash);
for ( tObj = obj_index_hash[iHash]; tObj != NULL; tObj = tObj->next )
printf("name: %s vnum: %d\n", tObj->name, tObj->vnum ); */

    /* DEBUG CODE */
    // printf( "\ntop_vnum_obj before: %d\n", top_vnum_obj );

    if (top_vnum_obj == index)
    for (i = 1; i < index; i++)
    if (get_obj_index(i))
    top_vnum_obj = i;

    /* DEBUG CODE */
    // printf( "top_vnum_obj after: %d\n", top_vnum_obj );

    top_obj_index--;

    /* Now crush all resets */
    count = 0;
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next) {
        prev = pRoom->reset_first;
        for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
          switch (pReset->command) {
          case 'O':
          case 'E':
          case 'P':
          case 'G':
            if ((pReset->arg1 == index) || ((pReset->command == 'P') && (pReset->arg3 == index))) {
              // printf("\nprev: %d  prev->next: %d\n",prev, prev->next );

              /* DEBUG CODE - uncomment this if you have doubts */
              /* printf("\nReset info for room %d:\n",pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d memloc:%d\n", tReset->command, tReset->arg1, tReset ); */

              if (pRoom->reset_first == pReset) {
                pRoom->reset_first = pReset->next;
                if (!pRoom->reset_first)
                pRoom->reset_last = NULL;
              }
              else if (pRoom->reset_last == pReset) {
                pRoom->reset_last = prev;
                prev->next = NULL;
              }
              else {
                prev->next = prev->next->next;
              }

              count++;
              SET_BIT(pRoom->area->area_flags, AREA_CHANGED);

              /* DEBUG CODE - uncomment this if you have doubts */
              /* printf("\nReset info for room %d after removal:\n", pRoom->vnum
); for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d memloc:%d\n", tReset->command, tReset->arg1, tReset ); */

              // printf("\nprev: %d  prev->next: %d\n",prev, prev->next );
            }
          }
          prev = pReset;
        }
      }
    }

    sprintf(buf, "Removed object vnum ^C%d^x and ^C%d^x resets.\n\r", index, count);
    send_to_char(buf, ch);
    return TRUE;
  }

  MEDIT(medit_delete) {
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *sMob;
    MOB_INDEX_DATA *iMob;
    RESET_DATA *pReset = NULL;
    RESET_DATA *prev = NULL;
    ROOM_INDEX_DATA *pRoom = NULL;
    char arg[MIL];
    char buf[MSL];
    int index, count, iHash, i;
    int dobj[100]; /* I highly doubt one mobile will have 100 unique object resets
*/
    bool foundmob = FALSE;
    bool exist = FALSE;
    bool foundobj = FALSE;

    if (argument[0] == '\0') {
      send_to_char("Syntax:  medit delete [vnum]\n\r", ch);
      return FALSE;
    }

    one_argument(argument, arg);

    if (is_number(arg)) {
      index = atoi(arg);
      pMob = get_mob_index(index);
    }
    else {
      send_to_char("That is not a number.\n\r", ch);
      return FALSE;
    }

    if (!pMob) {
      send_to_char("No such mobile.\n\r", ch);
      return FALSE;
    }

    SET_BIT(pMob->area->area_flags, AREA_CHANGED);

    /* Remove it from the object list */

    iHash = index % MAX_KEY_HASH;

    /* DEBUG CODE - uncomment this if you have doubts */
    /* printf("\nMobile hash for location %d:\n", iHash);
for ( tMob = mob_index_hash[iHash]; tMob != NULL; tMob = tMob->next )
printf("short_desc: %s  vnum: %d\n", tMob->short_descr, tMob->vnum );*/

    sMob = mob_index_hash[iHash];

    if (sMob->next == NULL) /* only entry */
    mob_index_hash[iHash] = NULL;
    else if (sMob == pMob) /* first entry */
    mob_index_hash[iHash] = pMob->next;
    else /* everything else */
    {
      for (iMob = sMob; iMob != NULL; iMob = iMob->next) {
        if (iMob == pMob) {
          sMob->next = pMob->next;
          break;
        }
      }
    }

    /* See oedit_delete for why i dont free pMob here */

    /* DEBUG CODE - uncomment this if you have doubts */
    /* printf("\nMobile hash for location %d after removal:\n", iHash);
for ( tMob = mob_index_hash[iHash]; tMob != NULL; tMob = tMob->next )
printf("short_desc: %s  vnum: %d\n", tMob->short_descr, tMob->vnum );*/

    if (top_vnum_mob == index)
    for (i = 1; i < index; i++)
    if (get_obj_index(i))
    top_vnum_obj = i;

    top_mob_index--;

    /* Now crush all resets */
    count = 0;
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next) {
        dobj[0] = -1;
        prev = pRoom->reset_first;
        for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
          switch (pReset->command) {
          case 'M':
            if (pReset->arg1 == index) {
              foundmob = TRUE;

              /* DEBUG CODE - uncomment this if you have doubts */
              /* printf("\nReset info for room %d:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1
); */

              if (pRoom->reset_first == pReset) {
                pRoom->reset_first = pReset->next;
                if (!pRoom->reset_first)
                pRoom->reset_last = NULL;
              }
              else if (pRoom->reset_last == pReset) {
                pRoom->reset_last = prev;
                prev->next = NULL;
              }
              else
              prev->next = prev->next->next;

              count++;
              SET_BIT(pRoom->area->area_flags, AREA_CHANGED);

              /* DEBUG CODE - uncomment this if you have doubts */
              /* printf("\nReset info for room %d after removal:\n", pRoom->vnum
); for( tReset = pRoom->reset_first; tReset; tReset = tReset->next )
printf("command: %c  vnum: %d\n", tReset->command, tReset->arg1
); */
            }
            else
            foundmob = FALSE;

            break;
          case 'E':
          case 'G':
            if (foundmob) {
              // printf( "Removing: command: %c  vnum: %d\n", pReset->command, // pReset->arg1 );

              /* DEBUG CODE - uncomment this if you have doubts */
              /* printf("\nReset info for room %d:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset; tReset = tReset->next )
printf("command: %c  vnum: %d\n", tReset->command, tReset->arg1
); */

              exist = FALSE;

              for (i = 0; dobj[i] != -1; i++) {
                if (dobj[i] == pReset->arg1) {
                  exist = TRUE;
                  break;
                }
              }

              if (!exist) {
                dobj[i] = pReset->arg1;
                dobj[i + 1] = -1;

                /* DEBUG CODE */
                /* for( i = 0; dobj[i] != -1; i++ )
printf( "dobj[%d] : %d\n", i,dobj[i] ); */
              }

              if (pRoom->reset_first == pReset) {
                pRoom->reset_first = pReset->next;
                if (!pRoom->reset_first)
                pRoom->reset_last = NULL;
              }
              else if (pRoom->reset_last == pReset) {
                pRoom->reset_last = prev;
                prev->next = NULL;
              }
              else
              prev->next = prev->next->next;

              count++;
              SET_BIT(pRoom->area->area_flags, AREA_CHANGED);

              /* DEBUG CODE - uncomment this if you havedoubts */
              /* printf("\nReset info for room %d afterremoval:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1
); */
            }

            break;
          case 'P':
            foundobj = FALSE;

            for (i = 0; dobj[i] != -1; i++)
            if (dobj[i] == pReset->arg3)
            foundobj = TRUE;

            if (foundobj) {
              printf("Removing: command: %c  vnum: %d\n", pReset->command, pReset->arg1);

              /* DEBUG CODE - uncomment this if you havedoubts */
              /* printf("\nReset info for room %d:\n",pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1
); */

              if (pRoom->reset_first == pReset) {
                pRoom->reset_first = pReset->next;
                if (!pRoom->reset_first)
                pRoom->reset_last = NULL;
              }
              else if (pRoom->reset_last == pReset) {
                pRoom->reset_last = prev;
                prev->next = NULL;
              }
              else
              prev->next = prev->next->next;

              count++;
              SET_BIT(pRoom->area->area_flags, AREA_CHANGED);

              /* DEBUG CODE - uncomment this if you havedoubts */
              /* printf("\nReset info for room %d afterremoval:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1
); */
            }
          }
          prev = pReset;
        }
      }
    }

    sprintf(buf, "Removed mobile vnum ^C%d^x and ^C%d^x resets.\n\r", index, count);
    send_to_char(buf, ch);
    return TRUE;
  }

  void make_mob(int value) {
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int iHash;

    if (value == 0) {
      return;
    }

    pArea = get_vnum_area(value);

    if (!pArea) {
      return;
    }

    if (get_mob_index(value)) {
      return;
    }

    pMob = new_mob_index();
    pMob->vnum = value;
    pMob->area = pArea;

    if (value > top_vnum_mob)
    top_vnum_mob = value;

    SET_FLAG(pMob->act, ACT_IS_NPC);
    iHash = value % MAX_KEY_HASH;
    pMob->next = mob_index_hash[iHash];
    mob_index_hash[iHash] = pMob;
  }

  void make_object(int value) {
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int iHash;

    if (value == 0) {
      return;
    }

    pArea = get_vnum_area(value);
    if (!pArea) {
      return;
    }

    if (get_obj_index(value)) {
      return;
    }

    pObj = new_obj_index();
    pObj->vnum = value;
    pObj->area = pArea;

    if (value > top_vnum_obj)
    top_vnum_obj = value;

    iHash = value % MAX_KEY_HASH;
    pObj->next = obj_index_hash[iHash];
    obj_index_hash[iHash] = pObj;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

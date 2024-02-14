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

#if defined(__cplusplus)
extern "C" {
#endif



  bool is_institute_experimenting args((CHAR_DATA * victim));
  bool can_institute_experiment args((CHAR_DATA * ch));
  bool is_institute_treating args((CHAR_DATA * victim));
  bool can_institute_treat args((CHAR_DATA * ch));
  bool is_institute_trauma args((CHAR_DATA * victim));
  bool can_institute_trauma args((CHAR_DATA * ch));
  bool can_institute_teach args((CHAR_DATA * ch));
  int get_gpa args((INSTITUTE_TYPE * student, int type));
  void institute_victimize args((CHAR_DATA * ch, CHAR_DATA *vic, int amount));
  int	blood_level	args((CHAR_DATA *ch, CHAR_DATA *victim));
  int find_dorm_room	args( (int house) );

  vector<INSTITUTE_TYPE *> InVect;
  INSTITUTE_TYPE *nullins;

  bool institute_ch(CHAR_DATA *ch) {
    if (clinic_staff(ch, TRUE)) {
      return TRUE;
    }
    if (clinic_patient(ch)) {
      return TRUE;
    }
    if (college_student(ch, TRUE)) {
      return TRUE;
    }
    if (college_staff(ch, TRUE)) {
      return TRUE;
    }
    if(ch->race == RACE_DEPUTY)
    return TRUE;

    return FALSE;
  }

  bool college_student(CHAR_DATA *ch, bool countsuspended) {
    if (IS_NPC(ch))
    return FALSE;

    INSTITUTE_TYPE *ins;
    if (countsuspended == FALSE && clinic_patient(ch))
    return FALSE;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ins->name, ch->name) && ins->college_prestige > 0) {
        if (countsuspended == TRUE || ins->college_suspended == 0)
        return TRUE;
      }
    }
    return FALSE;
  }


  int college_group(CHAR_DATA *ch, bool countsuspended) {
    if (IS_NPC(ch))
    return FALSE;

    INSTITUTE_TYPE *ins;
    if (countsuspended == FALSE && clinic_patient(ch))
    return FALSE;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ins->name, ch->name) && ins->college_prestige > 0) {
        if (countsuspended == TRUE || ins->college_suspended == 0)
        return ins->college_group;
      }
    }
    return 0;
  }

  bool clinic_patient(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    INSTITUTE_TYPE *ins;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ins->name, ch->name) && ins->asylum_prestige > 0 && ins->asylum_inactive == 0 && ins->clinic_breakout <= 0) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool college_staff(CHAR_DATA *ch, bool countsuspended) {
    if (IS_NPC(ch))
    return FALSE;

    if (clinic_patient(ch) && countsuspended == FALSE)
    return FALSE;

    INSTITUTE_TYPE *ins;
    if (ch->race != RACE_FACULTY && ch->skills[SKILL_COLLEGECONTRACT] < 1)
    return FALSE;
    if (is_griefer(ch)) {
      return FALSE;
    }
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ins->name, ch->name) && ins->college_rank > 0) {
        if (countsuspended == TRUE || ins->college_suspended == 0)
        return TRUE;
        else
        return FALSE;
      }
    }
    if ((ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_COLLEGE) || ch->skills[SKILL_COLLEGECONTRACT] > 0) {
      x = 0;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end() && x < 1000; ++it) {
        x++;
        ins = (*it);
        if (!str_cmp(ins->name, ch->name)) {
          ins->college_rank = 1;
          return TRUE;
        }
      }

      ins = new_institute();
      free_string(ins->name);
      ins->name = str_dup(ch->name);
      free_string(ins->surname);
      ins->surname = str_dup(ch->pcdata->last_name);
      ins->college_rank = 1;
      InVect.push_back(ins);
      return TRUE;
    }

    return FALSE;
  }

  bool room_in_detention(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return FALSE;
    }

    if (room->vnum == 4854 || room->vnum == 4858 || room->vnum == 4850 || room->vnum == 4868 || room->vnum == 4331 || room->vnum == 4876 || room->vnum == 4848 || room->vnum == 4851) {
      return TRUE;
    }

    if (!str_cmp(room->subarea, "detention")) {
      return TRUE;
    }

    return FALSE;
  }

  bool in_detention(CHAR_DATA *ch) {
    if (room_in_detention(ch->in_room)) {
      return TRUE;
    }

    return FALSE;
  }

  bool institute_contractor(CHAR_DATA *ch) {
    if (college_staff(ch, TRUE) || clinic_staff(ch, TRUE)) {
      if (ch->race != RACE_FACULTY) {
        return TRUE;
      }
    }

    if (ch->race == RACE_DEPUTY) {
      return TRUE;
    }

    return FALSE;
  }

  bool institute_staff(CHAR_DATA *ch) {
    if (ch->race == RACE_FACULTY) {
      return TRUE;
    }

    if (institute_contractor(ch)) {
      return TRUE;
    }

    return FALSE;
  }

  bool institute_suspension_IMMORTAL(CHAR_DATA *ch) { return FALSE; }

  bool institute_suspension(CHAR_DATA *ch) {
    if (!institute_staff(ch)) {
      return FALSE;
    }

    if (institute_suspension_IMMORTAL(ch)) {
      return TRUE;
    }

    INSTITUTE_TYPE *ins;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ch->name, ins->name)) {
        if (ins->college_suspended > 0 || ins->asylum_suspended > 0) {
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool clinic_staff(CHAR_DATA *ch, bool countsuspended) {
    if (IS_NPC(ch))
    return FALSE;

    INSTITUTE_TYPE *ins;
    if (ch->race != RACE_FACULTY && ch->skills[SKILL_CLINICCONTRACT] < 1)
    return FALSE;

    if (is_griefer(ch)) {
      return FALSE;
    }
    if (clinic_patient(ch) && countsuspended == FALSE)
    return FALSE;

    if (countsuspended == FALSE) {
      if (institute_suspension_IMMORTAL(ch) == TRUE) {
        return FALSE;
      }
    }

    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ins->name, ch->name) && ins->asylum_rank > 0) {
        if (ins->asylum_prestige > 0 && countsuspended == FALSE && ins->asylum_inactive < 1)
        return FALSE;
        else if (countsuspended == TRUE || ins->asylum_suspended == 0)
        return TRUE;
        else
        return FALSE;
      }
    }
    if ((ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_CLINIC) || ch->skills[SKILL_CLINICCONTRACT] > 0) {
      x = 0;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end() && x < 1000; ++it) {
        x++;
        ins = (*it);
        if (!str_cmp(ins->name, ch->name)) {
          ins->asylum_rank = 1;
          return TRUE;
        }
      }

      ins = new_institute();
      free_string(ins->name);
      ins->name = str_dup(ch->name);
      free_string(ins->surname);
      ins->surname = str_dup(ch->pcdata->last_name);
      ins->asylum_rank = 1;
      InVect.push_back(ins);
      return TRUE;
    }

    return FALSE;
  }

  int college_staff_rank(CHAR_DATA *ch) {
    if (IS_IMMORTAL(ch)) {
      return 5;
    }

    INSTITUTE_TYPE *ins;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ch->name, ins->name))
      return ins->college_rank;
    }
    return 0;
  }

  int clinic_staff_rank(CHAR_DATA *ch) {
    if (IS_IMMORTAL(ch)) {
      return 5;
    }

    INSTITUTE_TYPE *ins;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ch->name, ins->name))
      return ins->asylum_rank;
    }
    return 0;
  }

  int college_student_prestige(CHAR_DATA *ch) {
    INSTITUTE_TYPE *ins;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      x++;
      ins = (*it);
      if (!str_cmp(ch->name, ins->name))
      return ins->college_prestige;
    }
    return 0;
  }

  int clinic_patient_prestige(CHAR_DATA *ch) {
    INSTITUTE_TYPE *ins;
    int x = 0;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end() && x < 1000; ++it) {
      ins = (*it);
      x++;
      if (!str_cmp(ch->name, ins->name))
      return ins->asylum_prestige;
    }
    return 0;
  }

  void commit_char(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;
      unequip_char_silent(ch, obj);
      SET_BIT(obj->extra_flags, ITEM_WARDROBE);
      obj->stash_room = ROOM_INDEX_CLINICSTORE;
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        obj->stash_room = ROOM_INDEX_CLINICSTORE;
      }
    }
    obj = create_object(get_obj_index(81), 0);
    obj_to_char(obj, ch);
    wear_obj(ch, obj, TRUE, TRUE);
    obj = create_object(get_obj_index(82), 0);
    obj_to_char(obj, ch);
    wear_obj(ch, obj, TRUE, TRUE);
    obj = create_object(get_obj_index(83), 0);
    obj_to_char(obj, ch);
    wear_obj(ch, obj, TRUE, TRUE);
    ch->pcdata->total_money += ch->money;
    ch->money = 0;
    if (IS_FLAG(ch->act, PLR_BOUND))
    REMOVE_FLAG(ch->act, PLR_BOUND);
    if (IS_FLAG(ch->act, PLR_BOUNDFEET))
    REMOVE_FLAG(ch->act, PLR_BOUNDFEET);

    if (IS_FLAG(ch->act, PLR_SHROUD))
    REMOVE_FLAG(ch->act, PLR_SHROUD);

    act("Some orderlies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_INDEX_CLINICCOMMIT));
    act("Some orderlies bring $n in.", ch, NULL, NULL, TO_ROOM);
    act("Some orderlies come and take you away.", ch, NULL, NULL, TO_CHAR);
    auto_imprint(ch, "happy and disinhibited, as if you were drunk but without the nausea. You also feel a little lazy and sleepy", IMPRINT_DRUGS);

    CHAR_DATA *victim;
    int x = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end() && x < 1000; ++it) {
      DESCRIPTOR_DATA *d = *it;
      x++;
      if (d->connected != CON_PLAYING)
      continue;
      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim == ch)
      continue;

      if (!clinic_staff(victim, FALSE) && !clinic_patient(victim))
      continue;

      send_to_char("You hear about a new patient being committed.\n\r", victim);
    }
  }
  void institute_announce(char *argument) {
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

      if (institute_room(victim->in_room))
      printf_to_char(victim, "The Institute loudspeakers announce, '%s'\n\r", argument);
    }
  }

  void institute_staff_announce(char *argument) {
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

      if (!clinic_staff(victim, FALSE) && !college_staff(victim, FALSE))
      continue;

      if (institute_room(victim->in_room))
      printf_to_char(victim, "The Institute loudspeakers announce, '%s'\n\r",argument);
    }
  }

  void straight_jacket(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    obj = create_object(get_obj_index(45195), 0);
    obj_to_char(obj, ch);
    wear_obj(ch, obj, TRUE, TRUE);
    if (!IS_FLAG(ch->act, PLR_BOUND))
    SET_FLAG(ch->act, PLR_BOUND);
    if (IS_FLAG(ch->act, PLR_BOUNDFEET))
    REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
  }

  struct college_student_greater {
    inline bool operator()(const INSTITUTE_TYPE *struct1, const INSTITUTE_TYPE *struct2) {

      return (struct1->college_prestige * (3 + struct1->college_grade) >
      struct2->college_prestige * (3 + struct2->college_grade) );

    }
  };
  struct college_staff_greater {
    inline bool operator()(const INSTITUTE_TYPE *struct1, const INSTITUTE_TYPE *struct2) {
      return (struct1->college_staff_prestige > struct2->college_staff_prestige);
    }
  };

  struct clinic_patient_greater {
    inline bool operator()(const INSTITUTE_TYPE *struct1, const INSTITUTE_TYPE *struct2) {
      return (struct1->asylum_prestige > struct2->asylum_prestige);
    }
  };
  struct clinic_staff_greater {
    inline bool operator()(const INSTITUTE_TYPE *struct1, const INSTITUTE_TYPE *struct2) {
      return (struct1->asylum_staff_prestige > struct2->asylum_staff_prestige);
    }
  };


  bool has_clinic_power(CHAR_DATA *ch, INSTITUTE_TYPE *patient, bool release) {
    if (IS_IMMORTAL(ch))
    return TRUE;
    if (patient->asylum_inactive == 1)
    return FALSE;
    CHAR_DATA *victim;
    victim = get_char_world_pc(patient->name);
    if (victim != NULL) {
      if (ch == victim)
      return FALSE;
      if (same_player(ch, victim))
      return FALSE;
    }

    if (patient->asylum_status == ASYLUM_SELFCOMMIT || patient->asylum_status == ASYLUM_COLLEGECOMMIT) {
      if (clinic_staff(ch, FALSE) && clinic_staff_rank(ch) >= 1)
      return TRUE;
      else
      return FALSE;
    }
    if (patient->asylum_status == ASYLUM_REMOTECOMMIT) {
      if (!str_cmp(ch->name, patient->asylum_owner))
      return TRUE;
      if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && !str_cmp(clan_lookup(ch->faction)->name, patient->asylum_owner)) {
        if (release == FALSE || has_trust(ch, TRUST_RESOURCES, ch->faction))
        return TRUE;
      }
      if (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL && !str_cmp(clan_lookup(ch->factiontwo)->name, patient->asylum_owner)) {
        if (release == FALSE || has_trust(ch, TRUST_RESOURCES, ch->factiontwo))
        return TRUE;
      }
      if (release == FALSE && clinic_staff(ch, FALSE))
      return TRUE;
    }
    return FALSE;
  }

  char * college_group_name(int group)
  {
    if(group == COLLEGE_GOTH)
    return "Goth";
    if(group == COLLEGE_JOCK)
    return "Jock";
    if(group == COLLEGE_OVERACHIEVER)
    return "Nerd";
    if(group == COLLEGE_PREP)
    return "Prep";
    if(group == COLLEGE_BADKID)
    return "Delinquent";
    if(group == COLLEGE_THEATRE)
    return "Theatre";
    return "";
  }


  void college_roll(CHAR_DATA *ch) {
    char buf[MSL] = "", page[MSL] = "", entry[MSL] = "", spacer[MSL] = "";
    char tmp[MSL];
    char *frame;
    int col = 0, spacing = 0, rowwidth = ch->linewidth, colwidth = 0;
    int minwidth = 26, columns = 0;

    for (int i = 5; i >= 1; i--) { // determining columns
      if (floor(rowwidth / i) >= minwidth) {
        columns = i;
        break;
      }
    }

    colwidth = floor(rowwidth / columns);

    strcpy(page, "`CFreshmen`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");
    std::sort(InVect.begin(), InVect.end(), college_student_greater());
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->college_prestige <= 0) {
        continue;
      }

      if(ins->college_grade != 1) {
        continue;
      }

      sprintf(entry, "%15s - %.1f GPA", ins->name, (float(get_gpa(ins, 1))) / 10);
      if (ins->college_suma == 1) {
        strcat(entry, "(`cSumma Cum Laude`x)");
      }
      else if(ins->college_group > 0) {
        sprintf(tmp, "(%s)", college_group_name(ins->college_group));
        strcat(entry, tmp);
      }

      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }

    strcat(page, "\n\r`CSophmores`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");


    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->college_prestige <= 0) {
        continue;
      }

      if(ins->college_grade != 2) {
        continue;
      }

      sprintf(entry, "%15s - %.1f GPA", ins->name, (float(get_gpa(ins, 1))) / 10);
      if (ins->college_suma == 1) {
        strcat(entry, "(`cSumma Cum Laude`x)");
      }
      else if(ins->college_group > 0) {
        sprintf(tmp, "(%s)", college_group_name(ins->college_group));
        strcat(entry, tmp);
      }


      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }


    strcat(page, "\n\r`CJuniors`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");


    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->college_prestige <= 0) {
        continue;
      }

      if(ins->college_grade != 3) {
        continue;
      }

      sprintf(entry, "%15s - %.1f GPA", ins->name, (float(get_gpa(ins, 1))) / 10);
      if (ins->college_suma == 1) {
        strcat(entry, "(`cSumma Cum Laude`x)");
      }
      else if(ins->college_group > 0) {
        sprintf(tmp, "(%s)", college_group_name(ins->college_group));
        strcat(entry, tmp);
      }

      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }

    strcat(page, "\n\r`CSeniors`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");


    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->college_prestige <= 0) {
        continue;
      }

      if(ins->college_grade != 4) {
        continue;
      }

      sprintf(entry, "%15s - %.1f GPA", ins->name, (float(get_gpa(ins, 1))) / 10);
      if (ins->college_suma == 1) {
        strcat(entry, "(`cSumma Cum Laude`x)");
      }
      else if(ins->college_group > 0) {
        sprintf(tmp, "(%s)", college_group_name(ins->college_group));
        strcat(entry, tmp);
      }

      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }






    if (col > 0) {
      strcat(page, "\n\r");
    }
    strcat(page, "`x\n\r");
    page_to_char(page, ch);
    frame = str_dup("");
    free_string(frame);
    return;
  }

  void college_staff_roll(CHAR_DATA *ch) {
    char buf[MSL] = "", page[MSL] = "", entry[MSL] = "", spacer[MSL] = "";
    char *frame;
    int col = 0, spacing = 0, rowwidth = ch->linewidth, colwidth = 0;
    int minwidth = 20, columns = 0;

    for (int i = 5; i >= 1; i--) { // determining columns
      if (floor(rowwidth / i) >= minwidth) {
        columns = i;
        break;
      }
    }

    colwidth = floor(rowwidth / columns);

    strcpy(page, "`CCollege Staff`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");
    std::sort(InVect.begin(), InVect.end(), college_staff_greater());
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->college_rank <= 0) {
        continue;
      }

      if (ins->college_suspended > 0) {
        sprintf(entry, "[-] %s", ins->name);
      }
      else {
        sprintf(entry, "[%d] %s", ins->college_rank, ins->name);
      }

      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }
    if (col > 0) {
      strcat(page, "\n\r");
    }
    strcat(page, "`x\n\r");
    page_to_char(page, ch);
    frame = str_dup("");
    free_string(frame);
    return;
  }

  void clinic_roll(CHAR_DATA *ch) {
    char buf[MSL] = "", page[MSL] = "", entry[MSL] = "", spacer[MSL] = "";
    char *frame;
    int col = 0, spacing = 0, rowwidth = ch->linewidth, colwidth = 0;
    int minwidth = 70, columns = 0;

    for (int i = 5; i >= 1; i--) { // determining columns
      if (floor(rowwidth / i) >= minwidth) {
        columns = i;
        break;
      }
    }

    colwidth = floor(rowwidth / columns);

    strcpy(page, "`CPatient Roster`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");
    std::sort(InVect.begin(), InVect.end(), clinic_patient_greater());
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->asylum_prestige <= 0 || ins->asylum_inactive > 0) {
        continue;
      }

      sprintf(entry, "%15s - %s", ins->name, ins->asylum_diagnoses);

      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }
    if (col > 0) {
      strcat(page, "\n\r");
    }
    strcat(page, "`x\n\r");
    page_to_char(page, ch);
    frame = str_dup("");
    free_string(frame);
    return;
  }

  void clinic_staff_roll(CHAR_DATA *ch) {
    char buf[MSL] = "", page[MSL] = "", entry[MSL] = "", spacer[MSL] = "";
    char *frame;
    int col = 0, spacing = 0, rowwidth = ch->linewidth, colwidth = 0;
    int minwidth = 20, columns = 0;

    for (int i = 5; i >= 1; i--) { // determining columns
      if (floor(rowwidth / i) >= minwidth) {
        columns = i;
        break;
      }
    }

    colwidth = floor(rowwidth / columns);

    strcpy(page, "`CClinic Staff`x\n\r");
    strcat(page, "`g_");
    frame = draw_horizontal_line(rowwidth - 2);
    strcat(page, frame);
    strcat(page, "_`x\n\r");
    std::sort(InVect.begin(), InVect.end(), clinic_staff_greater());
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (ins->inactivity > 500 || ins->asylum_rank <= 0) {
        continue;
      }

      if (ins->asylum_suspended > 0) {
        sprintf(entry, "[-] %s", ins->name);
      }
      else {
        sprintf(entry, "[%d] %s", ins->asylum_rank, ins->name);
      }

      strcat(page, entry);
      if (columns > 1) {
        col++;
      }

      if (columns == 1) { // spacing columns
        spacing = rowwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        strcat(buf, ("\n\r"));
      }
      else {
        spacing = colwidth - safe_strlen_color(entry);
        sprintf(buf, "%*s", spacing, spacer);
        if (col == columns) {
          col = 0;
          strcat(buf, ("\n\r"));
        } // next row
      }
      strcat(page, buf);
    }
    if (col > 0) {
      strcat(page, "\n\r");
    }
    strcat(page, "`x\n\r");
    page_to_char(page, ch);
    frame = str_dup("");
    free_string(frame);
    return;
  }

  void institute_roll(CHAR_DATA *ch, char branch[MSL]) {
    bool college_rolled = FALSE, clinic_rolled = FALSE;
    if (!institute_ch(ch) && !IS_IMMORTAL(ch)) {
      send_to_char("Only other students and staff can see the roll\n\r", ch);
      return;
    }

    if (!str_cmp(branch, "college")) {
      college_roll(ch);
      college_rolled = TRUE;
    }
    else if (!str_cmp(branch, "clinic")) {
      clinic_roll(ch);
      clinic_rolled = TRUE;
    }

    if (college_rolled == FALSE) {
      college_roll(ch);
    }
    if (clinic_rolled == FALSE) {
      clinic_roll(ch);
    }

    return;
  }

  void institute_staff_roll(CHAR_DATA *ch, char branch[MSL]) {
    bool college_rolled = FALSE, clinic_rolled = FALSE;
    if (!institute_ch(ch) && !IS_IMMORTAL(ch)) {
      send_to_char("Only other students and staff can see the roll\n\r", ch);
      return;
    }

    if (!str_cmp(branch, "college")) {
      college_staff_roll(ch);
      college_rolled = TRUE;
    }
    else if (!str_cmp(branch, "clinic")) {
      clinic_staff_roll(ch);
      clinic_rolled = TRUE;
    }

    if (college_rolled == FALSE) {
      college_staff_roll(ch);
    }
    if (clinic_rolled == FALSE) {
      clinic_staff_roll(ch);
    }

    return;
  }

  _DOFUN(do_clinic) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (is_griefer(ch)) {
      return;
    }

    if (!str_cmp(arg1, "roster")) {
      institute_roll(ch, "clinic");
      return;
    }
    else if (!str_cmp(arg1, "purge") && IS_IMMORTAL(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, argument)) {
          ins->asylum_prestige = 0;
          ins->college_prestige = 0;
          ins->college_rank = 0;
          ins->asylum_rank = 0;
          ins->asylum_status = 0;
        }
      }
    }
    else if (!str_cmp(arg1, "staff")) {
      institute_staff_roll(ch, "clinic");
    }
    else if (!str_cmp(arg1, "readfile")) {
      if (!clinic_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(argument, ins->name)) {
          if (ins->asylum_prestige > 0) {
            printf_to_char(ch, "Patient Record for: %s.\n\n%s\n\r", ins->name, ins->asylum_notes);
            return;
          }
        }
      }
      send_to_char("There aren't any patients like that.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "page")) {

      if (ch->played / 3600 < 25) {
        send_to_char("You need to play more before using that.\n\r", ch);
        return;
      }

      if (!institute_room(ch->in_room)) {
        send_to_char("You can only do that from within the institute.\n\r", ch);
        return;
      }
      if(ch->pcdata->page_timeout > 0)
      {
        send_to_char("You can't do that again yet.\n\r", ch);
        return;
      }
      ch->pcdata->page_timeout = 30;
      if (!str_cmp(argument, "staff")) {
        char buf[MSL];
        sprintf(buf, "Paging clinic staff to %s, clinic staff to %s.\n\r", ch->in_room->name, ch->in_room->name);
        institute_announce(buf);
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, argument)) {
          char buf[MSL];
          sprintf(buf, "Paging %s to %s, %s to %s.\n\r", ins->name, ch->in_room->name, ins->name, ch->in_room->name);
          institute_announce(buf);
          return;
        }
      }
      send_to_char("No such staffer.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "announce")) {
      if (!clinic_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }

      institute_announce(argument);
      return;
    }
    else if (!str_cmp(arg1, "writefile")) {
      if (!clinic_staff(ch, FALSE)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(argument, ins->name)) {
          if (ins->asylum_prestige > 0) {
            string_append(ch, &ins->asylum_pending_notes);
            return;
          }
        }
      }
      send_to_char("There aren't any patients like that.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "diagnose")) {
      if (!clinic_staff(ch, FALSE)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(arg2, ins->name)) {
          if (ins->asylum_prestige > 0) {
            free_string(ins->asylum_diagnoses);
            ins->asylum_diagnoses = str_dup(argument);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("There aren't any patients like that.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "nophone")) {

      if (ch->played / 3600 < 50)
      return;

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {
          if (!IS_IMMORTAL(ch)) {
            if (!has_clinic_power(ch, ins, TRUE) || !str_cmp(ins->name, ch->name)) {
              send_to_char("You don't have the authority to do that.\n\r", ch);
              return;
            }
          }
          if (ins->nophone == 0) {
            ins->nophone = 1;
            send_to_char("You remove their ability to get a signal.\n\r", ch);
            return;
          }
          if (ins->nophone == 1) {
            ins->nophone = 0;
            send_to_char("You return their ability to get a signal.\n\r", ch);
            return;
          }
        }
      }
    }
    else if (!str_cmp(arg1, "exemption")) {
      if(college_group(ch, FALSE) == COLLEGE_JOCK)
      {
        INSTITUTE_TYPE *ins;
        for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
        it != InVect.end(); ++it) {
          ins = (*it);
          if (!str_cmp(ch->name, ins->name)) {
            if(ins->asylum_status == ASYLUM_SELFCOMMIT || ins->asylum_status == ASYLUM_COLLEGECOMMIT)
            {
              if(ins->college_power > current_time - (3600 * 24 * 10))
              {
                send_to_char("You can't do that yet.\n\r", ch);
                return;
              }
              ins->asylum_inactive = 1;
              ins->asylum_status = 0;
              ins->college_power = current_time;
              ins->college_immune = current_time;
              if (IS_FLAG(ch->act, PLR_BOUND))
              REMOVE_FLAG(ch->act, PLR_BOUND);
              if (IS_FLAG(ch->act, PLR_BOUNDFEET))
              REMOVE_FLAG(ch->act, PLR_BOUNDFEET);

              char_from_room(ch);
              char_to_room(ch, get_room_index(ROOM_INDEX_CLINICDISCHARGE));
              send_to_char("You use your athletic exemption to get out of the clinic.\n\r", ch);
              return;

            }
          }
        }
      }
      if(college_group(ch, FALSE) == COLLEGE_OVERACHIEVER)
      {
        INSTITUTE_TYPE *ins;
        for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
        it != InVect.end(); ++it) {
          ins = (*it);
          if (!str_cmp(ch->name, ins->name)) {
            if(ins->asylum_status == ASYLUM_SELFCOMMIT || ins->asylum_status == ASYLUM_COLLEGECOMMIT)
            {
              if(ins->college_power > current_time - (3600 * 24 * 10))
              {
                send_to_char("You can't do that yet.\n\r", ch);
                return;
              }
              ins->asylum_inactive = 1;
              ins->asylum_status = 0;
              ins->college_power = current_time;
              ins->college_immune = current_time;
              if (IS_FLAG(ch->act, PLR_BOUND))
              REMOVE_FLAG(ch->act, PLR_BOUND);
              if (IS_FLAG(ch->act, PLR_BOUNDFEET))
              REMOVE_FLAG(ch->act, PLR_BOUNDFEET);

              char_from_room(ch);
              char_to_room(ch, get_room_index(ROOM_INDEX_CLINICDISCHARGE));
              send_to_char("You use your academic exemption to get out of the clinic.\n\r", ch);
              return;

            }
          }
        }
      }


    }
    else if (!str_cmp(arg1, "breakout")) {

      if(college_group(ch, FALSE) != COLLEGE_BADKID)
      {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      INSTITUTE_TYPE *youins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        if(!str_cmp(ch->name, (*it)->name))
        {
          youins = (*it);
        }
      }
      if(youins->college_power > current_time - (3600 * 24 * 10))
      {
        send_to_char("You can't do that yet.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {

          youins->college_power = current_time;
          ins->clinic_breakout = 180;
          send_to_char("You break them out them.\n\r", ch);
          char buf[MSL];
          if (!IS_IMMORTAL(ch)) {
            sprintf(buf, "PRISONER: %s broken out by %s.", ins->name, ch->name);
          }
          else {
            sprintf(buf, "PRISONER: %s broken out by %s.", ins->name, "the Institute administration");
          }

          log_string(buf);

          if (get_char_world_pc(ins->name) != NULL) {
            send_to_char("You have been broken out from the clinic.\n\r", get_char_world_pc(ins->name));
            CHAR_DATA *victim = get_char_world_pc(ins->name);
            if (IS_FLAG(victim->act, PLR_BOUND))
            REMOVE_FLAG(victim->act, PLR_BOUND);
            if (IS_FLAG(victim->act, PLR_BOUNDFEET))
            REMOVE_FLAG(victim->act, PLR_BOUNDFEET);

            char_from_room(victim);
            char_to_room(victim, get_room_index(ROOM_INDEX_CLINICDISCHARGE));

          }
          else {
            message_to_char(ins->name, "You have been broken out from the clinic.\n\r");
          }
          return;
        }
      }
      send_to_char("There's nobody like that you can release.\n\r", ch);
      return;

    }
    else if (!str_cmp(arg1, "getout")) {

      if(college_group(ch, FALSE) != COLLEGE_PREP)
      {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }

      INSTITUTE_TYPE *youins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        if(!str_cmp(ch->name, (*it)->name))
        {
          youins = (*it);
        }
      }
      if(youins->college_power > current_time - (3600 * 24 * 10))
      {
        send_to_char("You can't do that yet.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {
          if(ins->asylum_status != ASYLUM_SELFCOMMIT && ins->asylum_status != ASYLUM_COLLEGECOMMIT)
          {
            send_to_char("You don't have the authority to do that.\n\r", ch);
            return;
          }
        }
        youins->college_power = current_time;
        ins->asylum_inactive = 1;
        ins->asylum_status = 0;
        ins->college_immune = current_time;
        send_to_char("You release them.\n\r", ch);
        char buf[MSL];
        if (!IS_IMMORTAL(ch)) {
          sprintf(buf, "PRISONER: %s released by %s.", ins->name, ch->name);
        }
        else {
          sprintf(buf, "PRISONER: %s released by %s.", ins->name, "the Institute administration");
        }

        log_string(buf);
        if (clan_lookup_name(ins->asylum_owner) != NULL) {
          sprintf(buf, "%s is released.", ins->name);
          send_log(clan_lookup_name(ins->asylum_owner)->vnum, buf);
        }

        if (get_char_world_pc(ins->name) != NULL) {
          send_to_char("You have been discharged from the clinic.\n\r", get_char_world_pc(ins->name));
          CHAR_DATA *victim = get_char_world_pc(ins->name);
          if (IS_FLAG(victim->act, PLR_BOUND))
          REMOVE_FLAG(victim->act, PLR_BOUND);
          if (IS_FLAG(victim->act, PLR_BOUNDFEET))
          REMOVE_FLAG(victim->act, PLR_BOUNDFEET);

          char_from_room(victim);
          char_to_room(victim, get_room_index(ROOM_INDEX_CLINICDISCHARGE));

        }
        else {
          message_to_char(ins->name, "You have been discharged from the clinic.\n\r");
        }
        return;
      }
      send_to_char("There's nobody like that you can release.\n\r", ch);
      return;

    }
    else if (!str_cmp(arg1, "release")) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {
          if (!IS_IMMORTAL(ch)) {
            if (!has_clinic_power(ch, ins, TRUE) || !str_cmp(ins->name, ch->name)) {
              send_to_char("You don't have the authority to do that.\n\r", ch);
              return;
            }
          }
          ins->asylum_inactive = 1;
          ins->asylum_status = 0;
          send_to_char("You release them.\n\r", ch);
          char buf[MSL];
          if (!IS_IMMORTAL(ch)) {
            sprintf(buf, "PRISONER: %s released by %s.", ins->name, ch->name);
          }
          else {
            sprintf(buf, "PRISONER: %s released by %s.", ins->name, "the Institute administration");
          }

          log_string(buf);
          if (clan_lookup_name(ins->asylum_owner) != NULL) {
            sprintf(buf, "%s is released.", ins->name);
            send_log(clan_lookup_name(ins->asylum_owner)->vnum, buf);
          }

          if (get_char_world_pc(ins->name) != NULL) {
            send_to_char("You have been discharged from the clinic.\n\r", get_char_world_pc(ins->name));
            CHAR_DATA *victim = get_char_world_pc(ins->name);
            if (IS_FLAG(victim->act, PLR_BOUND))
            REMOVE_FLAG(victim->act, PLR_BOUND);
            if (IS_FLAG(victim->act, PLR_BOUNDFEET))
            REMOVE_FLAG(victim->act, PLR_BOUNDFEET);

            char_from_room(victim);
            char_to_room(victim, get_room_index(ROOM_INDEX_CLINICDISCHARGE));

          }
          else {
            message_to_char(ins->name, "You have been discharged from the clinic.\n\r");
          }
          return;
        }
      }
      send_to_char("There's nobody like that you can release.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "suspend")) {
      if (!clinic_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }
      if (clinic_staff_rank(ch) < 2 && !IS_IMMORTAL(ch)) {
        send_to_char("You need to be at least rank 2 to do that.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if (safe_strlen(argument) < 2) {
        send_to_char("You need to provide a reason.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(arg2, ins->name)) {
          if (ins->asylum_rank > 0 && ins->asylum_rank < clinic_staff_rank(ch)) {
            char buf[MSL];
            sprintf(buf, "SUSPEND: Clinic: %s Suspends %s for %s", ch->name, ins->name, argument);
            log_string(buf);
            wiznet(buf, NULL, NULL, WIZ_DEATHS, 0, 0);
            sprintf(buf, "%s suspends %s for %s.", ch->name, ins->name, argument);
            institute_staff_announce(buf);
            ins->asylum_suspended = 1;
            ins->asylum_suspend_rank = clinic_staff_rank(ch);
            send_to_char("You suspend them.\n\r", ch);
            if (get_char_world_pc(ins->name) != NULL)
            send_to_char("You have been suspended from the clinic.\n\r", get_char_world_pc(ins->name));
            else
            message_to_char(ins->name, "You have been suspended from the clinic.\n\r");
            return;
          }
        }
      }
      send_to_char("There's nobody like that you can suspend.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "unsuspend")) {
      if (!clinic_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {
          if (ins->asylum_suspended > 0 && ins->asylum_suspend_rank <= clinic_staff_rank(ch)) {
            DESCRIPTOR_DATA *d;
            d = new_descriptor();
            CHAR_DATA *victim;
            char buf[MSL];
            if (get_char_world_pc(ins->name) != NULL) {
              victim = get_char_world_pc(ins->name);
            }
            else {
              bool isChar = FALSE;
              isChar = load_char_obj(d, ins->name);
              if (isChar) {
                d->character->desc = NULL;
                char_list.push_front(d->character);
                d->connected = CON_PLAYING;

                victim = d->character;
              }
            }

            if (institute_suspension_IMMORTAL(victim) == TRUE) {
              send_to_char("You don't have the authority to revoke this suspension.\n\r", ch);
              return;
            }

            if (victim != NULL) {
              real_quit(victim);
            }

            sprintf(buf, "SUSPEND: Clinic: %s Unsuspends %s", ch->name, ins->name);
            log_string(buf);
            wiznet(buf, NULL, NULL, WIZ_DEATHS, 0, 0);
            sprintf(buf, "%s unsuspends %s.", ch->name, ins->name);
            institute_staff_announce(buf);

            send_to_char("You lift the suspension.\n\r", ch);
            if (get_char_world_pc(ins->name) != NULL)
            send_to_char("Your suspension from the clinic has been lifted.\n\r", get_char_world_pc(ins->name));
            else
            message_to_char(
            ins->name, "Your suspension from the clinic has been lifted.\n\r");
            ins->asylum_suspended = 0;
            ins->asylum_suspend_rank = 0;
            return;
          }
        }
      }
      send_to_char("There isn't anyone like that who you can lift the suspension of.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "Treat")) {
      if (ch->in_room->vnum == 16269) {
        send_to_char("You can't treat patients in the recreation room.\n\r", ch);
        return;
      }
      if (ch->pcdata->institute_action == INSTITUTE_TREAT) {
        send_to_char("You stop treating.\n\r", ch);
        act("$n stops treating.", ch, NULL, NULL, TO_ROOM);
        ch->pcdata->institute_action = 0;
        return;
      }
      if (!can_institute_treat(ch)) {
        send_to_char("There's nobody here you can treat.\n\r", ch);
        return;
      }
      ch->pcdata->institute_action = INSTITUTE_TREAT;
      send_to_char("You start treating.\n\r", ch);
      act("$n starts treating.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg1, "Experiment")) {
      if (ch->in_room->vnum == 16269) {
        send_to_char("You can't treat patients in the recreation room.\n\r", ch);
        return;
      }
      if (ch->pcdata->institute_action == INSTITUTE_EXPERIMENT) {
        send_to_char("You stop experimenting.\n\r", ch);
        act("$n stops experimenting.", ch, NULL, NULL, TO_ROOM);
        ch->pcdata->institute_action = 0;
        return;
      }
      if (!can_institute_experiment(ch)) {
        send_to_char("There's nobody here you can experiment on.\n\r", ch);
        return;
      }
      ch->pcdata->institute_action = INSTITUTE_EXPERIMENT;
      send_to_char("You start experimenting.\n\r", ch);
      act("$n starts experimenting.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg1, "MakeArrest")) {
      if (ch->played / 3600 < 75 && ch->pcdata->account->maxhours < 100)
      return;

      free_string(ch->pcdata->ci_message);
      ch->pcdata->ci_message = str_dup("");
      free_string(ch->pcdata->ci_desc);
      ch->pcdata->ci_desc = str_dup("");
      ch->pcdata->ci_editing = 15;
      send_to_char("Done.", ch);
      return;
    }
    else if (!str_cmp(arg1, "ViewArrests")) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 500)
        continue;
        if (ins->asylum_status == ASYLUM_NEED_APPROVAL) {
          printf_to_char(
          ch, "Subject:%s\nProcessing Cost: $%d\n\nSupporting Statement %s\n\n\r", ins->name, ins->arrest_cost, ins->arrest_notes);
        }
        if (ins->asylum_status == ASYLUM_WANTED) {
          printf_to_char(ch, "`RWanted`x:%s\n\nSupporting Statement %s\n\n\r", ins->name, ins->arrest_notes);
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "BribeArrest")) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, argument) && ins->asylum_status == ASYLUM_NEED_APPROVAL) {
          if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && has_trust(ch, TRUST_RESOURCES, ch->faction)) {
            villain_mod(ch, 15, "Bribe arrest");
            use_resources(
            manip_cost(clan_lookup(ch->faction), ins->arrest_cost / 10), ch->faction, NULL, "bribing a judge");
            ins->asylum_status = ASYLUM_WANTED;
            send_to_char("They are now wanted.\n\r", ch);
          }
          else {
            if (ch->pcdata->total_money / 100 < ins->arrest_cost) {
              send_to_char("You don't have enough in the bank for that bribe.\n\r", ch);
              return;
            }
            ch->pcdata->total_money -= ins->arrest_cost * 100;
            ins->asylum_status = ASYLUM_WANTED;
            send_to_char("They are now wanted.\n\r", ch);
          }
          return;
        }
      }
      send_to_char("No request like that found.\n\r", ch);
    }
    else if (!str_cmp(arg1, "ApproveArrest")) {
      if (ch->race != RACE_DEPUTY && ch->race != RACE_PILLAR) {
        send_to_char("Arrests can only be approved by deputies and pillars of the community.\n\r", ch);
        return;
      }
      if (ch->pcdata->overworked > 0) {
        send_to_char("You are too tired.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, argument) && ins->asylum_status == ASYLUM_NEED_APPROVAL) {
          if (clan_lookup_name(ins->asylum_owner) != NULL) {
            use_resources(manip_cost(clan_lookup_name(ins->asylum_owner), ins->arrest_cost / 20), clan_lookup_name(ins->asylum_owner)->vnum, NULL, "bribing a judge");
          }
          else {
            house_charge(ins->asylum_owner, ins->arrest_cost / 2);
            printf_to_char(ch, "You pay %d to have the arrest approved.\n\r", ins->arrest_cost / 2);
          }
          ins->asylum_status = ASYLUM_WANTED;
          send_to_char("They are now wanted.\n\r", ch);
          return;
        }
      }
      send_to_char("No request like that found.\n\r", ch);
    }
    else if (!str_cmp(arg1, "remove")) {
      if (!institute_room(ch->in_room)) {
        send_to_char("You are not in the institute.\n\r", ch);
        return;
      }
      if (!clinic_staff(ch, FALSE) && !college_staff(ch, FALSE)) {
        send_to_char("You have to be staff to do that.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_room(ch, NULL, argument);
      if (victim == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (clinic_staff(victim, FALSE) || college_staff(victim, FALSE)
          || college_student(victim, FALSE) || clinic_patient(victim)) {
        send_to_char("You cannot have active staff, students or patients removed.\n\r", ch);
        return;
      }
      act("Some staff come and escort $n out.", victim, NULL, NULL, TO_ROOM);
      act("Some staff come and escort you out.", victim, NULL, NULL, TO_CHAR);
      char_from_room(victim);
      char_to_room(victim, get_room_index(1188));
      act("Some Institute staff escort $n into the area.", victim, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg1, "Commit")) {
      // if(!asylum_room(ch->in_room)) {send_to_char("You have to be in the clinic first.\n\r", ch); return;}
      CHAR_DATA *victim = get_char_room(ch, NULL, argument);
      if (victim == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (clinic_patient(ch) || is_helpless(ch) || room_hostile(ch->in_room) || clinic_patient(victim)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (!is_helpless(victim) && ch != victim) {
        send_to_char("They don't seem likely to let that happen.\n\r", ch);
        return;
      }
      if (victim->faction != 0 && ch->race == RACE_FACULTY) {
        send_to_char("Best to stay out of society business.\n\r", ch);
        return;
      }
      if (ch->pcdata->overworked > 0) {
        send_to_char("You are too tired.\n\r", ch);
        return;
      }
      int cost = commit_cost(ch, victim->name);
      cost /= 10;
      if (clinic_staff(ch, FALSE) || ch->race == RACE_DEPUTY) {
        cost /= 4;
      }

      if (ch != victim) {
        printf_to_char(
        ch, "The Institute is called in.  You slip an orderly $%d and they take %s off your hands. (The patient is currently sponsored by you personally, use clinic changesponsor to change ownership)\n\r", cost / 100, PERS(victim, ch));
        ch->money -= cost;
      }
      villain_mod(ch, 10, "Arrest");

      act("Some orderlies from the Institute arrive.  They sedate and haul $n away.", victim, NULL, NULL, TO_ROOM);

      char buf[MSL];
      sprintf(buf, "PRISONER: %s committed by %s for %d.", victim->name, ch->name, cost / 100);
      log_string(buf);

      INSTITUTE_TYPE *ins;
      bool found = FALSE;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, victim->name)) {
          ins->asylum_prestige = UMAX(ins->asylum_prestige, 1);
          ins->asylum_inactive = 0;
          found = TRUE;
        }
      }
      if (found == FALSE) {
        ins = new_institute();
        free_string(ins->name);
        ins->name = str_dup(victim->name);
        free_string(ins->surname);
        ins->surname = str_dup(victim->pcdata->last_name);
        ins->asylum_prestige = 1;
        InVect.push_back(ins);
      }

      commit_char(victim);
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, victim->name)) {
          if (ins->asylum_status == ASYLUM_WANTED) {
            ins->asylum_status = ASYLUM_REMOTECOMMIT;
            free_string(ins->asylum_pending_notes);
            ins->asylum_pending_notes = str_dup(ins->arrest_notes);
          }
          else {
            if (ch == victim) {
              ins->asylum_status = ASYLUM_SELFCOMMIT;
            }
            else {
              ins->asylum_status = ASYLUM_REMOTECOMMIT;
              free_string(ins->asylum_owner);
              ins->asylum_owner = str_dup(ch->name);
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "Summon")) {
      if (!institute_room(ch->in_room)) {
        send_to_char("You're not in the clinic.\n\r", ch);
        return;
      }

      if (ch->played / 3600 < 10) {
        send_to_char("You have to get to 10 hours first.\n\r", ch);
        return;
      }

      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, arg2)) {
          if (!has_clinic_power(ch, ins, FALSE)) {
            send_to_char("You do not have the authority to do that.\n\r", ch);
            return;
          }
          CHAR_DATA *victim = get_char_world_pc(ins->name);
          if (victim == NULL) {
            send_to_char("They're not available.\n\r", ch);
            return;
          }
          if (is_helpless(victim) && nonhelpless_pop(victim->in_room) > 0) {
            send_to_char("They're not available.\n\r", ch);
            return;
          }

          if (victim == NULL) {
            send_to_char("They're not available.\n\r", ch);
            return;
          }
          
          // keeps people from summoning characters off world / out of chargen - Disco 6/6/20
          if (victim->in_room->area->vnum != HAVEN_TOWN_VNUM) { 
            send_to_char("They're not available.\n\r", ch);
            return;
          }
          straight_jacket(victim);
          act("Some orderlies come and take $n away.", victim, NULL, NULL, TO_ROOM);
          send_to_char("Some orderlies come and put you in a straight jacket before taking you away.\n\r", victim);
          char_from_room(victim);
          char_to_room(victim, ch->in_room);
          act("Some orderlies bring $N in in a straight jacket.", ch, NULL, victim, TO_CHAR);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "Straightjacket")) {
      if (!institute_room(ch->in_room)) {
        send_to_char("You're not in the clinic.\n\r", ch);
        return;
      }

      if (ch->played / 3600 < 10) {
        send_to_char("You have to get to 10 hours first.\n\r", ch);
        return;
      }

      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, arg2)) {
          if (!has_clinic_power(ch, ins, FALSE)) {
            send_to_char("You do not have the authority to do that.\n\r", ch);
            return;
          }
          CHAR_DATA *victim = get_char_world_pc(ins->name);
          if (victim == NULL || victim->in_room != ch->in_room) {
            send_to_char("They're not here.\n\r", ch);
            return;
          }
          straight_jacket(victim);
          act("Some orderlies come and put $n in a straight jacket.", victim, NULL, NULL, TO_ROOM);
          send_to_char("Some orderlies come and put you in a straight jacket.\n\r", victim);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "Changesponsor")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, arg2)) {
          if (!has_clinic_power(ch, ins, TRUE) || ins->asylum_status != ASYLUM_REMOTECOMMIT) {
            send_to_char("You can't do that.\n\r", ch);
            return;
          }
          if (clan_lookup_name(argument) != NULL) {
            if (!has_trust(ch, TRUST_RESOURCES, clan_lookup_name(argument)->vnum)) {
              send_to_char("You don't have the authority to do that.\n\r", ch);
              return;
            }
            free_string(ins->asylum_owner);
            ins->asylum_owner = str_dup(argument);
            char buf[MSL];
            sprintf(buf, "%s is transfered to your care.", ins->name);
            send_message(clan_lookup_name(argument)->vnum, buf);
            send_to_char("Done.\n\r", ch);
            return;
          }
          else {
            CHAR_DATA *victim = get_char_room(ch, NULL, argument);
            if (victim == NULL || clinic_patient(victim) || IS_NPC(victim)) {
              send_to_char("They're not here.\n\r", ch);
              return;
            }
            free_string(ins->asylum_owner);
            ins->asylum_owner = str_dup(victim->name);
            printf_to_char(victim, "%s is transfered to your care.\n\r", ins->name);
            send_to_char("Done.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("You don't have sponsorship of any patients like that.\n\r", ch);
    }
    else if (!str_cmp(arg1, "Petition")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      int amount = atoi(argument);
      if (amount < 1 || amount > 10000) {
        send_to_char("Syntax: Clinic Petition (person) (amount)\n\r", ch);
        return;
      }
      if (clan_lookup(ch->faction) == NULL || !has_trust(ch, TRUST_RESOURCES, ch->faction) || clan_lookup(ch->faction)->resource < 5000 + amount / 10) {
        send_to_char("You don't have the authority of your society to do that.\n\r", ch);
        return;
      }
      if (!is_leader(ch, ch->faction) && clinic_patient(ch)) {
        send_to_char("You don't have access to your accounts from inside the clinic.\n\r", ch);
        return;
      }

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->asylum_prestige <= 0 || ins->asylum_inactive == 1)
        continue;
        amount = UMIN(amount, 2000 - ins->asylum_extracost);
        if (amount < 1 || amount > 10000) {
          send_to_char("Syntax: Clinic Petition (person) (amount)\n\r", ch);
          return;
        }

        if (!str_cmp(ins->name, arg2)) {
          ins->asylum_extracost += amount;
          use_resources(amount / 10, ch->faction, NULL, "petitioning for the release of a patient.");
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("No such patient.\n\r", ch);
    }
    else if (!str_cmp(arg1, "Report")) {
      CHAR_DATA *victim = get_char_room(ch, NULL, argument);
      if (victim == NULL || !clinic_patient(victim) || IS_NPC(victim)) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (is_helpless(ch)) {
        send_to_char("You can't do that at the moment.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, victim->name)) {
          if (has_clinic_power(ch, ins, FALSE)) {
            act("$n reports you and some orderlies come and take you away.", ch, NULL, victim, TO_VICT);
            act("You report $N and some orderlies come and take $M away.", ch, NULL, victim, TO_CHAR);
            act("$n reports $N and some orderlies come and take $M away.", ch, NULL, victim, TO_NOTVICT);
            ROOM_INDEX_DATA *room;
            room = get_room_index(ROOM_INS_CELL_ONE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_THREE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FOUR);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FIVE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_TWO);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FOUR);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_SIX);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_EIGHT);


            victim->pcdata->solitary_time = 120;
            char_from_room(victim);
            char_to_room(victim, room);
            return;
          }
          else if (clinic_patient_prestige(ch) >
              ins->asylum_prestige * 12 / 10) {
            act("$n reports you and some orderlies come and take you away.", ch, NULL, victim, TO_VICT);
            act("You report $N and some orderlies come and take $M away.", ch, NULL, victim, TO_CHAR);
            act("$n reports $N and some orderlies come and take $M away.", ch, NULL, victim, TO_NOTVICT);
            ROOM_INDEX_DATA *room;
            room = get_room_index(ROOM_INS_CELL_ONE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_THREE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FOUR);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FIVE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_TWO);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FOUR);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_SIX);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_EIGHT);
            victim->pcdata->solitary_time = 120;
            char_from_room(victim);
            char_to_room(victim, room);
            for (vector<INSTITUTE_TYPE *>::iterator ik = InVect.begin();
            ik != InVect.end(); ++ik) {
              if (!str_cmp((*ik)->name, ch->name))
              (*ik)->asylum_prestige = UMAX(1, (*ik)->asylum_prestige - 100);
            }
            return;
          }
          else if (college_student(ch, FALSE)) {
            act("$n reports you and some orderlies come and take you away.", ch, NULL, victim, TO_VICT);
            act("You report $N and some orderlies come and take $M away.", ch, NULL, victim, TO_CHAR);
            act("$n reports $N and some orderlies come and take $M away.", ch, NULL, victim, TO_NOTVICT);
            ROOM_INDEX_DATA *room;
            room = get_room_index(ROOM_INS_CELL_ONE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_THREE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FOUR);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FIVE);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_TWO);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_FOUR);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_SIX);
            if (room_pop(room) > 0)
            room = get_room_index(ROOM_INS_CELL_EIGHT);
            victim->pcdata->solitary_time = 120;
            char_from_room(victim);
            char_to_room(victim, room);
            for (vector<INSTITUTE_TYPE *>::iterator ik = InVect.begin();
            ik != InVect.end(); ++ik) {
              if (!str_cmp((*ik)->name, ch->name)) {
                if ((*ik)->college_prestige > 0)
                (*ik)->college_prestige = UMAX(1, (*ik)->college_prestige - 100);
              }
            }
            return;
          }
        }
      }
      send_to_char("You can't report them.\n\r", ch);
    }
    else
    send_to_char("Syntax: Clinic Roster/Staff/Suspend/Unsuspend/Treat/Experiment/MakeArrest/ViewArrests/BribeArrest/ApproveArrest/Commit/Release/Changesponsor/Petition/Signin/Summon/Report/Readfile/Writefile/Straightjacket/Diagnose.\n\r", ch);
  }

  _DOFUN(do_college) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if (!str_cmp(arg1, "enroll")) {
      if (college_student(ch, TRUE)) {
        send_to_char("You're already a college student.\n\r", ch);
        return;
      }
      if (college_staff(ch, TRUE)) {
        send_to_char("You're part of the faculty.\n\r", ch);
        return;
      }
      if (ch->pcdata->tier_raised > 0) {
        send_to_char("You have learned all you can.\n\r", ch);
        return;
      }
      if (get_tier(ch) >= 3) {
        send_to_char("You have learned all you can.\n\r", ch);
        return;
      }
      if (is_guest(ch) && ch->pcdata->guest_type == GUEST_NIGHTMARE) {
        send_to_char("You have learned all you can.\n\r", ch);
        return;
      }
      int ingrade = 0;
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if(!str_cmp(arg2, "freshman"))
      {
        ingrade = 1;
      }
      else if(!str_cmp(arg2, "sophomore"))
      {
        if(get_age(ch) < 19)
        {
          send_to_char("You need to be at least 19 to do that.\n\r", ch);
          return;
        }
        ingrade = 2;
      }
      else if(!str_cmp(arg2, "junior"))
      {
        if(get_age(ch) < 20)
        {
          send_to_char("You need to be at least 21 to do that.\n\r", ch);
          return;
        }
        ingrade = 3;
      }
      else if(!str_cmp(arg2, "senior"))
      {
        if(get_age(ch) < 21)
        {
          send_to_char("You need to be at least 21 to do that.\n\r", ch);
          return;
        }
        ingrade = 4;
      }
      else
      {
        send_to_char("Syntax: College Enroll (freshman/sophomore/junior/senior) (None/Goth/Jock/Nerd/Prep/Delinquent/Theater)\n\r", ch);
        return;
      }
      int ingroup = 0;
      if(!str_cmp(argument, "none"))
      {
        ingroup = 0;
      }
      else if(!str_cmp(argument, "goth"))
      {
        ingroup = COLLEGE_GOTH;
      }
      else if(!str_cmp(argument, "jock"))
      {
        ingroup = COLLEGE_JOCK;
      }
      else if(!str_cmp(argument, "nerd"))
      {
        ingroup = COLLEGE_OVERACHIEVER;
      }
      else if(!str_cmp(argument, "prep"))
      {
        ingroup = COLLEGE_PREP;
      }
      else if(!str_cmp(argument, "delinquent"))
      {
        ingroup = COLLEGE_BADKID;
      }
      else if(!str_cmp(argument, "theater"))
      {
        ingroup = COLLEGE_THEATRE;
      }
      else
      {
        send_to_char("Syntax: College Enroll (freshman/sophomore/junior/senior) (None/Goth/Jock/Nerd/Prep/Delinquent/Theater)\n\r", ch);
        return;
      }

      send_to_char("You sign up to the college.(Note: As White Oak is a school for supernatural students it is very unlikely your character could find themselves enrolled if they weren't somehow involved in or knowledgeable of the supernatural world.)\n\r", ch);
      if(ch->sex == SEX_MALE)
      {
        send_to_char("You now have the option of joining a fraternity for free on campus housing, to do so use 'college join fraternity (Alpha Gamma Omega/Sigma Lambda Pi)'\n\r", ch);
      }
      else
      {
        send_to_char("You now have the option of joining a sorority for free on campus housing, to do so use 'college join sorority (Delta Delta Delta/Phi Beta Chi)'\n\r", ch);

      }
      ch->pcdata->job_type_one = JOB_FULLSTUDENT;
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, ch->name)) {
          ins->college_prestige = 1;
          ins->college_grade = ingrade;
          ins->college_group = ingroup;
          ins->college_power = current_time;
          return;
        }
      }
      ins = new_institute();
      free_string(ins->name);
      ins->name = str_dup(ch->name);
      free_string(ins->surname);
      ins->surname = str_dup(ch->pcdata->last_name);

      ins->college_prestige = 1;
      ins->college_grade = ingrade;
      ins->college_group = ingroup;
      ins->college_power = current_time;
      InVect.push_back(ins);
      send_to_char("You sign up to the college.\n\r", ch);
      ch->pcdata->job_type_one = JOB_FULLSTUDENT;
      return;
    }
    else if (!str_cmp(arg1, "quit")) {
      if (!college_student(ch, TRUE)) {
        send_to_char("You're not a college student.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, ch->name) && ins->college_prestige > 0) {
          ins->college_prestige = 0;
          send_to_char("You quit the college.\n\r", ch);
          return;
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "join")) {
      if (!college_student(ch, TRUE)) {
        send_to_char("You're not a college student.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if(!str_cmp(arg2, "fraternity") && ch->sex == SEX_MALE)
      {
        if(college_house(ch) != 0)
        {
          send_to_char("You are already in a fraternity.\n\r", ch);
          return;
        }
        bool joined = FALSE;
        char msg[MSL];
        if(!str_cmp(argument, "alpha gamma omega"))
        {
          INSTITUTE_TYPE *ins;
          for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
          it != InVect.end(); ++it) {
            ins = (*it);
            if(!str_cmp(ins->name, ch->name))
            {
              ins->college_house = FRAT_ALPHA;
              int nroom = find_dorm_room(FRAT_ALPHA);
              if(nroom == 0)
              {
                send_to_char("There are no rooms available in that fraternity.\n\r", ch);
                return;
              }
              ROOM_INDEX_DATA *dorm = get_room_index(nroom);
              ins->dorm_room = nroom;
              printf_to_char(ch, "You join Alpha Gamma Omega and are assigned %s in Forbearance House.\n\r", dorm->name);
              joined = TRUE;
            }
          }
          if(joined == TRUE)
          {
            for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
            it != InVect.end(); ++it) {
              ins = (*it);
              if(str_cmp(ins->name, ch->name) && ins->college_house == FRAT_ALPHA)
              {
                sprintf(msg, "%s joins Alpha Gamma Omega.", ch->name);
                offline_message(ins->name, msg);
              }
            }
          }
          return;
        }
        if(!str_cmp(argument, "sigma lambda pi"))
        {
          INSTITUTE_TYPE *ins;
          for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
          it != InVect.end(); ++it) {
            ins = (*it);
            if(!str_cmp(ins->name, ch->name))
            {
              ins->college_house = FRAT_SIGMA;
              int nroom = find_dorm_room(FRAT_SIGMA);
              if(nroom == 0)
              {
                send_to_char("There are no rooms available in that fraternity.\n\r", ch);
                return;
              }
              ROOM_INDEX_DATA *dorm = get_room_index(nroom);
              ins->dorm_room = nroom;
              printf_to_char(ch, "You join Sigma Lambda Pi and are assigned %s in Purity House.\n\r", dorm->name);
              joined = TRUE;
            }
          }
          if(joined == TRUE)
          {
            for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
            it != InVect.end(); ++it) {
              ins = (*it);
              if(str_cmp(ins->name, ch->name) && ins->college_house == FRAT_ALPHA)
              {
                sprintf(msg, "%s joins Sigma Lambda Pi.", ch->name);
                offline_message(ins->name, msg);
              }
            }
          }
          return;
        }
        send_to_char("Syntax: College Join Fraternity (Alpha Gamma Omega/Sigma Lambda Pi)\n\r", ch);
        return;
      }
      if(!str_cmp(arg2, "sorority") && ch->sex == SEX_FEMALE)
      {
        if(college_house(ch) != 0)
        {
          send_to_char("You are already in a sorority.\n\r", ch);
          return;
        }
        bool joined = FALSE;
        char msg[MSL];
        if(!str_cmp(argument, "delta delta delta"))
        {
          INSTITUTE_TYPE *ins;
          for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
          it != InVect.end(); ++it) {
            ins = (*it);
            if(!str_cmp(ins->name, ch->name))
            {
              ins->college_house = SORIT_DELTA;
              int nroom = find_dorm_room(SORIT_DELTA);
              if(nroom == 0)
              {
                send_to_char("There are no rooms available in that sorority.\n\r", ch);
                return;
              }
              ROOM_INDEX_DATA *dorm = get_room_index(nroom);
              ins->dorm_room = nroom;
              printf_to_char(ch, "You join Delta Delta Delta and are assigned %s in Charity House.\n\r", dorm->name);
              joined = TRUE;
            }
          }
          if(joined == TRUE)
          {
            for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
            it != InVect.end(); ++it) {
              ins = (*it);
              if(str_cmp(ins->name, ch->name) && ins->college_house == SORIT_DELTA)
              {
                sprintf(msg, "%s joins Delta Delta Delta.", ch->name);
                offline_message(ins->name, msg);
              }
            }
          }
          return;
        }
        if(!str_cmp(argument, "phi beta chi"))
        {
          INSTITUTE_TYPE *ins;
          for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
          it != InVect.end(); ++it) {
            ins = (*it);
            if(!str_cmp(ins->name, ch->name))
            {
              ins->college_house = SORIT_PHI;
              int nroom = find_dorm_room(SORIT_PHI);
              if(nroom == 0)
              {
                send_to_char("There are no rooms available in that sorority.\n\r", ch);
                return;
              }
              ROOM_INDEX_DATA *dorm = get_room_index(nroom);
              ins->dorm_room = nroom;
              printf_to_char(ch, "You join Phi Beta Chi and are assigned %s in Repentence House.\n\r", dorm->name);
              joined = TRUE;
            }
          }
          if(joined == TRUE)
          {
            for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
            it != InVect.end(); ++it) {
              ins = (*it);
              if(str_cmp(ins->name, ch->name) && ins->college_house == SORIT_PHI)
              {
                sprintf(msg, "%s joins Phi Beta Chi.", ch->name);
                offline_message(ins->name, msg);
              }
            }
          }
          return;
        }
        send_to_char("Syntax: College Join Sorority (Delta Delta Delta/Phi Beta Chi)\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "leave")) {
      if (!college_student(ch, TRUE)) {
        send_to_char("You're not a college student.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if(!str_cmp(arg2, "fraternity"))
      {
        if(college_house(ch) == 0)
        {
          send_to_char("You aren't in a fraternity.\n\r", ch);
          return;
        }
        if(college_house(ch) == college_house_room(ch->in_room))
        {
          send_to_char("You can't leave your fraternity while in your house.\n\r", ch);
          return;
        }
        INSTITUTE_TYPE *ins;
        for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
        it != InVect.end(); ++it) {
          ins = (*it);
          if (!str_cmp(ins->name, ch->name)) {
            ins->college_house = 0;
            ins->dorm_room = 0;
            send_to_char("You leave your fraternity.\n\r", ch);
            return;
          }
        }
      }
      if(!str_cmp(arg2, "sorority"))
      {
        if(college_house(ch) == 0)
        {
          send_to_char("You aren't in a sorority.\n\r", ch);
          return;
        }
        if(college_house(ch) == college_house_room(ch->in_room))
        {
          send_to_char("You can't leave your sorority while in your house.\n\r", ch);
          return;
        }
        INSTITUTE_TYPE *ins;
        for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
        it != InVect.end(); ++it) {
          ins = (*it);
          if (!str_cmp(ins->name, ch->name)) {
            ins->college_house = 0;
            ins->dorm_room = 0;
            send_to_char("You leave your sorority.\n\r", ch);
            return;
          }
        }
      }
      send_to_char("Syntax: College Leave (Fraternity/Sorority)\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "progress")) {
      if (!college_student(ch, TRUE)) {
        send_to_char("You're not a college student.\n\r", ch);
        return;
      }
      if (get_month() + 1 != 5 && get_month() + 1 != 1 && get_month() + 1 != 8) {
        send_to_char("You can only progress in May, August or January.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, ch->name) && ins->college_prestige > 0) {
          if(ins->college_grade == 1 && get_age(ch) >= 19)
          {
            ins->college_grade++;
            send_to_char("You progress to sophomore.\n\r", ch);
            return;
          }
          if(ins->college_grade == 2 && get_age(ch) >= 20)
          {
            ins->college_grade++;
            send_to_char("You progress to junior.\n\r", ch);
            return;
          }
          if(ins->college_grade == 3 && get_age(ch) >= 21)
          {
            ins->college_grade++;
            send_to_char("You progress to senior.\n\r", ch);
            return;
          }
          send_to_char("You can't progress yet.\n\r", ch);
          return;
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "graduate")) {
      if (!college_student(ch, TRUE)) {
        send_to_char("You're not a college student.\n\r", ch);
        return;
      }
      if (get_age(ch) < 19) {
        send_to_char("You need to be at least 19 to do that.\n\r", ch);
        return;
      }
      if (get_month() + 1 != 5 && get_month() + 1 != 1 && get_month() + 1 != 8) {
        send_to_char("You can only graduate in May, August or January.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, ch->name) && ins->college_prestige > 0) {
          if (ins->college_suma == 1)
          ch->pcdata->tier_raised++;
          ch->rpexp += ins->college_prestige / 2;
          ins->college_prestige = 0;
          send_to_char("You graduate from college.\n\r", ch);
          return;
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "remove")) {
      if (!institute_room(ch->in_room)) {
        send_to_char("You are not in the institute.\n\r", ch);
        return;
      }
      if (!clinic_staff(ch, FALSE) && !college_staff(ch, FALSE)) {
        send_to_char("You have to be staff to do that.\n\r", ch);
        return;
      }
      CHAR_DATA *victim = get_char_room(ch, NULL, argument);
      if (victim == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (clinic_staff(victim, FALSE) || college_staff(victim, FALSE)
          || college_student(victim, FALSE) || clinic_patient(victim)) {
        send_to_char("You cannot have active staff, students or patients removed.\n\r", ch);
        return;
      }
      act("Some staff come and escort $n out.", victim, NULL, NULL, TO_ROOM);
      act("Some staff come and escort you out.", victim, NULL, NULL, TO_CHAR);
      char_from_room(victim);
      char_to_room(victim, get_room_index(1188));
      act("Some Institute staff escort $n into the area.", victim, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!str_cmp(arg1, "page")) {
      if (ch->played / 3600 < 25) {
        send_to_char("You need to play more before using that.\n\r", ch);
        return;
      }

      if (!institute_room(ch->in_room)) {
        send_to_char("You can only do that from within the institute.\n\r", ch);
        return;
      }
      
      if(ch->pcdata->page_timeout > 0)
      {
        send_to_char("You can't do that again yet.\n\r", ch);
        return;
      }
      ch->pcdata->page_timeout = 30;

      if (!str_cmp(argument, "staff")) {
        char buf[MSL];
        sprintf(buf, "Paging college staff to %s, college staff to %s.\n\r", ch->in_room->name, ch->in_room->name);
        institute_announce(buf);
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, argument)) {
          char buf[MSL];
          sprintf(buf, "Paging %s to %s, %s to %s.\n\r", ins->name, ch->in_room->name, ins->name, ch->in_room->name);
          institute_announce(buf);
          return;
        }
      }
      send_to_char("No such person in the college.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "announce")) {
      if (!college_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the staff.\n\r", ch);
        return;
      }
      institute_announce(argument);
      return;
    }
    else if (!str_cmp(arg1, "roll") || !str_cmp(arg1, "roster")) {
      institute_roll(ch, "college");
      return;
    }
    else if (!str_cmp(arg1, "staff")) {
      institute_staff_roll(ch, "college");
    }
    else if (!str_cmp(arg1, "readfile")) {
      if (!college_staff(ch, FALSE)) {
        send_to_char("You aren't an active member of the faculty.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(argument, ins->name)) {
          if (ins->college_prestige > 0) {
            printf_to_char(ch, "College Record for: %s.\n\n%s\n\r", ins->name, ins->college_notes);
            return;
          }
        }
      }
      send_to_char("There aren't any students like that.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "writefile")) {
      if (!college_staff(ch, FALSE)) {
        send_to_char("You aren't an active member of the faculty.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(argument, ins->name)) {
          if (ins->college_prestige > 0) {
            string_append(ch, &ins->college_pending_notes);
            return;
          }
        }
      }
      send_to_char("There aren't any students like that.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "nophone")) {
      if (!college_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the faculty.\n\r", ch);
        return;
      }

      if (ch->played / 3600 < 50) {
        send_to_char("You need to play more before using that.\n\r", ch);
        return;
      }

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {
          if (ins->nophone == 0) {
            ins->nophone = 1;
            send_to_char("You remove their ability to get a signal.\n\r", ch);
            return;
          }
          if (ins->nophone == 1) {
            ins->nophone = 0;
            send_to_char("You return their ability to get a signal.\n\r", ch);
            return;
          }
        }
      }
    }
    else if (!str_cmp(arg1, "commit")) {
      if (!college_staff(ch, FALSE)) {
        send_to_char("You aren't an active member of the faculty.\n\r", ch);
        return;
      }

      if (ch->played / 3600 < 50) {
        send_to_char("You need to play more before using that.\n\r", ch);
        return;
      }

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(argument, ins->name)) {
          if (ins->college_prestige > 0 && (ins->asylum_prestige == 0 || ins->asylum_inactive != 0)) {
            if (get_char_world_pc(ins->name) == NULL) {
              send_to_char("They have to be awake to be committed.\n\r", ch);
              return;
            }
            if (!institute_room(get_char_world_pc(ins->name)->in_room)) {
              send_to_char("They are not on institute grounds.\n\r", ch);
              return;
            }
            if(ins->college_immune > current_time - 3600*24*3)
            {
              send_to_char("They have been commited too recently.\n\r", ch);
              return;
            }
            CHAR_DATA *vic = get_char_world_pc(ins->name);
            if(get_skill(vic, SKILL_ENDOWMENT) > 0)
            {
              send_to_char("They helped arrange an endowment for the college..\n\r", ch);
              return;
            }
            ins->asylum_prestige = UMAX(1, ins->asylum_prestige);
            ins->asylum_inactive = 0;
            ins->asylum_status = ASYLUM_COLLEGECOMMIT;
            free_string(ins->college_committer);
            ins->college_committer = str_dup(ch->name);
            commit_char(vic);
            return;
          }
        }
      }
      send_to_char("There aren't any students like that.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "reset")) {
      if (!IS_IMMORTAL(ch)) {
        send_to_char("You need to be at least rank 2 to do that.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(arg2, ins->name)) {
          ins->asylum_prestige = 0;
          ins->asylum_rank = 0;
          ins->college_prestige = 0;
          ins->college_rank = 0;
          ins->college_staff_prestige = 0;
          ins->asylum_staff_prestige = 0;
        }
      }
    }
    else if (!str_cmp(arg1, "suspend")) {
      if (!college_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the faculty.\n\r", ch);
        return;
      }
      if (college_staff_rank(ch) < 2 && !IS_IMMORTAL(ch)) {
        send_to_char("You need to be at least rank 2 to do that.\n\r", ch);
        return;
      }
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if (safe_strlen(argument) < 2) {
        send_to_char("You need to provide a reason.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 1200)
        continue;
        if (!str_cmp(arg2, ins->name)) {
          if (ins->college_rank > 0 && ins->college_rank < college_staff_rank(ch)) {
            char buf[MSL];
            sprintf(buf, "SUSPEND: College: %s Suspends %s for %s", ch->name, ins->name, argument);
            log_string(buf);
            wiznet(buf, NULL, NULL, WIZ_DEATHS, 0, 0);
            sprintf(buf, "%s suspends %s for %s.", ch->name, ins->name, argument);
            institute_staff_announce(buf);
            ins->college_suspended = 1;
            ins->college_suspend_rank = college_staff_rank(ch);
            send_to_char("You suspend them.\n\r", ch);
            if (get_char_world_pc(ins->name) != NULL)
            send_to_char("You have been suspended from the college.\n\r", get_char_world_pc(ins->name));
            else
            message_to_char(ins->name, "You have been suspended from the college.\n\r");
            return;
          }
          else if (ins->college_prestige > 0) {
            ins->college_suspended = 1;
            ins->college_suspend_rank = college_staff_rank(ch);
            send_to_char("You suspend them.\n\r", ch);
            if (get_char_world_pc(ins->name) != NULL)
            send_to_char("You have been suspended from the college.\n\r", get_char_world_pc(ins->name));
            else
            message_to_char(ins->name, "You have been suspended from the college.\n\r");
            return;
          }
        }
      }
      send_to_char("There's nobody like that you can suspend.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "unsuspend")) {
      if (!college_staff(ch, FALSE) && !IS_IMMORTAL(ch)) {
        send_to_char("You aren't an active member of the faculty.\n\r", ch);
        return;
      }
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(argument, ins->name)) {
          if (ins->college_suspended > 0 && ins->college_suspend_rank <= college_staff_rank(ch)) {
            char buf[MSL];
            sprintf(buf, "SUSPEND: Clinic: %s Unsuspends %s", ch->name, ins->name);
            log_string(buf);
            wiznet(buf, NULL, NULL, WIZ_DEATHS, 0, 0);
            sprintf(buf, "%s unsuspends %s.", ch->name, ins->name);
            institute_staff_announce(buf);

            send_to_char("You lift their suspension.\n\r", ch);
            if (get_char_world_pc(ins->name) != NULL)
            send_to_char("Your suspension from the college has been lifted.\n\r", get_char_world_pc(ins->name));
            else
            message_to_char(
            ins->name, "Your suspension from the college has been lifted.\n\r");
            ins->college_suspended = 0;
            ins->college_suspend_rank = 0;
            return;
          }
        }
      }
      send_to_char("There isn't anyone like that who you can lift the suspension of.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "Teach")) {
      if (ch->pcdata->institute_action == INSTITUTE_TEACH) {
        send_to_char("You stop teaching.\n\r", ch);
        act("$n stops teaching.", ch, NULL, NULL, TO_ROOM);
        ch->pcdata->institute_action = 0;
        return;
      }
      if (!can_institute_teach(ch)) {
        send_to_char("There's nobody here you can teach.\n\r", ch);
        return;
      }
      ch->pcdata->institute_action = INSTITUTE_TEACH;
      send_to_char("You start teaching.\n\r", ch);
      act("$n starts teaching.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else
    send_to_char("Syntax: College Enroll/Quit/Graduate/Roll/Staff/Suspend/Unsuspend/Commit/Teach/Readfile/Writefile.\n\r", ch);
  }

  void institute_dream(CHAR_DATA *ch, int type) {
    if (type == 2) {
      free_string(ch->pcdata->nightmare);
      ch->pcdata->nightmare = str_dup("You have a dream in which you are on one knee, your head is bowed and you can only see the pristine white marble floors under you. You feel the flat of a blade lightly tapping your left shoulder, then your right. You look up but all you see is blazing, blinding light burning your eyes from your sockets.");
    }
    if (type == 3) {
      free_string(ch->pcdata->nightmare);
      ch->pcdata->nightmare = str_dup("You have a dream where are gazing out of a gold-rimmed window overlooking a fast expanse of clouds punctuated by the odd mountain peak. You turn around to see your family, tears well in your eyes and you say simply. 'I have to.'");
    }
  }

  int get_gpa(INSTITUTE_TYPE *student, int type) {
    INSTITUTE_TYPE *ins;
    int maxcredit = 0;
    int mincredit = 1000000;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (type == 1 && ins->college_prestige < 2)
      continue;
      if (type == 1) {
        if (ins->college_prestige + UMIN(1000, (ins->college_pending_prestige / 10)) > maxcredit) {
          maxcredit = ins->college_prestige + UMIN(1000, (ins->college_pending_prestige / 10));
        }
        else if (ins->college_prestige + UMIN(1000, (ins->college_pending_prestige / 10)) < mincredit) {
          mincredit = ins->college_prestige + UMIN(1000, (ins->college_pending_prestige / 10));
        }
      }
    }
    double point = (log2(maxcredit) - log2(mincredit)) / 20;
    if (type == 1) {
      double val = log2(student->college_prestige +
      UMIN(1000, (student->college_pending_prestige / 10)));
      val -= log2(mincredit);
      int mod = val / point;
      mod = UMIN(40, mod + 23);
      mod = UMAX(mod, 15);

      return mod;
    }

    return 20;
  }

  void clear_institute_rank(CHAR_DATA *ch) {
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (!str_cmp(ch->name, ins->name)) {
        if (ins->college_staff_prestige > 0) {
          ins->college_staff_prestige = 0;
        }
        if (ins->asylum_staff_prestige > 0) {
          ins->asylum_staff_prestige = 0;
        }

        if (ins->college_prestige > 0) {
          ins->college_prestige = 0;
        }
        if (ins->asylum_prestige > 0) {
          ins->asylum_prestige = 0;
        }
      }
    }
  }

  void institute_update() {
    INSTITUTE_TYPE *ins;
    char buf[MSL];
    int amount = 0;
    if (get_month() + 1 == 4) {
      int maxcredit = 0;
      char *maxname = "";
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        ins->college_pending_suma = 0;
        if (ins->age < 21)
        continue;
        if(ins->college_grade < 4)
        continue;
        if (ins->inactivity > 2000)
        continue;
        int pres = ins->college_prestige;
        if(ins->college_group == COLLEGE_OVERACHIEVER)
        pres = pres * 11/10;

        if (pres > maxcredit) {
          maxcredit = pres;
          free_string(maxname);
          maxname = str_dup(ins->name);
        }
      }
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, maxname) && safe_strlen(ins->name) > 1)
        ins->college_pending_suma = 1;
      }
    }
    else if (get_month() + 1 == 5) {
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->college_pending_suma == 1) {
          ins->college_suma = 1;
        }
        ins->college_pending_suma = 0;
      }
    }
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if(ins->inactivity > 500)
      ins->dorm_room = 0;
      if (get_char_world_pc(ins->name) != NULL && !IS_FLAG(get_char_world_pc(ins->name)->act, PLR_DEAD)) {
        CHAR_DATA *ch = get_char_world_pc(ins->name);

        ins->inactivity = 0;
        ins->age = get_age(ch);
        ins->gender = ch->sex;
        ins->race = ch->race;
        ins->modifier = ch->modifier;
        ins->clique_role = ch->pcdata->clique_role;
        ins->school_habit = ch->pcdata->habit[HABIT_SCHOOL];

        if (ins->college_rank > 2 && (ch->race != RACE_FACULTY || ch->pcdata->job_type_one != JOB_COLLEGE))
        ins->college_rank = 2;
        if (ins->asylum_rank > 2 && (ch->race != RACE_FACULTY || ch->pcdata->job_type_one != JOB_CLINIC))
        ins->asylum_rank = 2;

        if (ins->college_rank == 1 && ins->college_staff_prestige >= 5000) {
          ins->college_rank = 2;
          send_to_char("You are promoted in the college.\n\r", ch);
        }
        if (ins->college_rank == 2 && ins->college_staff_prestige >= 15000 && ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_COLLEGE) {
          ins->college_rank = 3;
          send_to_char("You are promoted in the college.\n\r", ch);
        }
        if (ins->college_rank == 3 && ins->college_staff_prestige >= 40000 && ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_COLLEGE) {
          institute_dream(ch, 2);
          ins->college_rank = 4;
          send_to_char("You are promoted in the college.\n\r", ch);
        }
        if (ins->college_rank == 4 && ins->college_staff_prestige >= 100000 && ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_COLLEGE) {
          institute_dream(ch, 3);
          ins->college_rank = 5;
          send_to_char("You are promoted in the college.\n\r", ch);
        }
        if (ins->asylum_rank == 1 && ins->asylum_staff_prestige >= 5000) {
          ins->asylum_rank = 2;
          send_to_char("You are promoted in the clinic.\n\r", ch);
        }
        if (ins->asylum_rank == 2 && ins->asylum_staff_prestige >= 15000 && ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_CLINIC) {
          ins->asylum_rank = 3;
          send_to_char("You are promoted in the clinic.\n\r", ch);
        }
        if (ins->asylum_rank == 3 && ins->asylum_staff_prestige >= 40000 && ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_CLINIC) {
          institute_dream(ch, 2);
          ins->asylum_rank = 4;
          send_to_char("You are promoted in the clinic.\n\r", ch);
        }
        if (ins->asylum_rank == 4 && ins->asylum_staff_prestige >= 100000 && ch->race == RACE_FACULTY && ch->pcdata->job_type_one == JOB_CLINIC) {
          institute_dream(ch, 3);
          ins->asylum_rank = 5;
          send_to_char("You are promoted in the asylum.\n\r", ch);
        }
        if (ch->race == RACE_FACULTY) {
          int maxrank = UMAX(ins->college_rank, ins->asylum_rank);
          if (maxrank >= 2 && ch->pcdata->tier_raised == 0)
          ch->pcdata->tier_raised = 1;
          if (maxrank >= 4 && ch->pcdata->tier_raised == 1)
          ch->pcdata->tier_raised = 2;
          if (maxrank == 5 && ch->pcdata->tier_raised == 2)
          ch->pcdata->tier_raised = 3;
        }

      }
      else if (ins->inactivity < 100000) {
        ins->inactivity++;
      }

      if (ins->inactivity < 1000 && ins->college_prestige > 0 && ins->college_suspended == 0) {
        if (((get_month() + 1 == 4 && get_day() > 19) || (get_month() + 1 == 12 && get_day() < 12) || (get_month() + 1 == 7 && get_day() > 24)) || ins->college_prestige < 1000) {
          if (ins->college_pending_prestige >= 100) {
            amount = 100;
            ins->college_pending_prestige -= amount;
            if (ins->clique_role == CLIQUEROLE_OVERACHIEVER) {
              amount = amount * 12 / 10;
            }
            ins->college_prestige += amount;
          }
          else if (ins->college_pending_prestige > 0) {
            ins->college_pending_prestige -= 1;
            ins->college_prestige += 1;
          }
        }
        else if ((get_month() + 1 == 2 && get_day() > 20) || (get_month() + 1 == 9 && get_day() > 22) || (get_month() + 1 == 6 && get_day() > 23)) {
          if (ins->college_pending_prestige >= 30) {
            amount = 30;
            ins->college_pending_prestige -= amount;
            if (ins->clique_role == CLIQUEROLE_OVERACHIEVER) {
              amount = amount * 12 / 10;
            }
            ins->college_prestige += amount;
          }
        }
      }

      if (safe_strlen(ins->college_pending_notes) > 2) {
        sprintf(buf, "%s\n\n%s", ins->college_notes, ins->college_pending_notes);
        free_string(ins->college_pending_notes);
        ins->college_pending_notes = str_dup("");
        free_string(ins->college_notes);
        ins->college_notes = str_dup(buf);
      }
      if (safe_strlen(ins->asylum_pending_notes) > 2) {
        sprintf(buf, "%s\n\n%s", ins->asylum_notes, ins->asylum_pending_notes);
        free_string(ins->asylum_pending_notes);
        ins->asylum_pending_notes = str_dup("");
        free_string(ins->asylum_notes);
        ins->asylum_notes = str_dup(buf);
      }
    }
  }

  void fread_institute(FILE *fp) {

    char buf[MSL];
    const char *word;
    bool fMatch;
    INSTITUTE_TYPE *ins;

    ins = new_institute();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("AsylumPrestige", ins->asylum_prestige, fread_number(fp));
        KEY("AsylumRank", ins->asylum_rank, fread_number(fp));
        KEY("AsylumStatus", ins->asylum_status, fread_number(fp));
        KEY("AsylumBaseCost", ins->asylum_basecost, fread_number(fp));
        KEY("AsylumExtraCost", ins->asylum_extracost, fread_number(fp));
        KEY("AsylumNotes", ins->asylum_notes, fread_string(fp));
        KEY("AsylumPendingNotes", ins->asylum_pending_notes, fread_string(fp));
        KEY("AsylumOwner", ins->asylum_owner, fread_string(fp));
        KEY("AsylumSuspended", ins->asylum_suspended, fread_number(fp));
        KEY("AsylumDiagnoses", ins->asylum_diagnoses, fread_string(fp));
        KEY("AsylumSuspendRank", ins->asylum_suspend_rank, fread_number(fp));
        KEY("AsylumStaffPrestige", ins->asylum_staff_prestige, fread_number(fp));
        KEY("ArrestNotes", ins->arrest_notes, fread_string(fp));
        KEY("Age", ins->age, fread_number(fp));
        KEY("ArrestCost", ins->arrest_cost, fread_number(fp));
        KEY("AsylumProtestDays", ins->asylum_protest_days, fread_number(fp));
        KEY("AsylumInactive", ins->asylum_inactive, fread_number(fp));
        break;
      case 'C':
        KEY("CollegePrestige", ins->college_prestige, fread_number(fp));
        KEY("CollegeStaffPrestige", ins->college_staff_prestige, fread_number(fp));
        KEY("CollegeRank", ins->college_rank, fread_number(fp));
        KEY("CollegePendingPrestige", ins->college_pending_prestige, fread_number(fp));
        KEY("CommitDate", ins->commit_date, fread_number(fp));
        KEY("CollegeNotes", ins->college_notes, fread_string(fp));
        KEY("CollegePendingNotes", ins->college_pending_notes, fread_string(fp));
        KEY("CollegeSuspended", ins->college_suspended, fread_number(fp));
        KEY("CollegeSuspendRank", ins->college_suspend_rank, fread_number(fp));
        KEY("CollegeGrade", ins->college_grade, fread_number(fp));
        KEY("CollegeGroup", ins->college_group, fread_number(fp));
        KEY("CollegeHouse", ins->college_house, fread_number(fp));
        KEY("CollegePower", ins->college_power, fread_number(fp));
        KEY("CollegeImmune", ins->college_immune, fread_number(fp));
        KEY("ClinicBreakout", ins->clinic_breakout, fread_number(fp));
        KEY("CollegePendingSuma", ins->college_pending_suma, fread_number(fp));
        KEY("CollegeSuma", ins->college_suma, fread_number(fp));
        KEY("CollegeCommitter", ins->college_committer, fread_string(fp));
        KEY("CliqueRole", ins->clique_role, fread_number(fp));
        break;
      case 'D':
        KEY("DormRoom", ins->dorm_room, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          InVect.push_back(ins);
          return;
        }

        break;
      case 'G':
        KEY("Gender", ins->gender, fread_number(fp));
        break;
      case 'I':
        KEY("Inactivity", ins->inactivity, fread_number(fp));
        break;
      case 'M':
        KEY("Modifier", ins->modifier, fread_number(fp));
        break;
      case 'N':
        KEY("Name", ins->name, fread_string(fp));
        KEY("NoPhone", ins->nophone, fread_number(fp));
        break;
      case 'R':
        KEY("Race", ins->race, fread_number(fp));
        break;
      case 'S':
        KEY("SchoolHabit", ins->school_habit, fread_number(fp));
        KEY("Surname", ins->surname, fread_string(fp));
      }

      if (!fMatch) {
        sprintf(buf, "Fread_institute: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_institutes() {
    nullins = new_institute();
    FILE *fp;

    if ((fp = fopen("../data/institute.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_Institutes: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "INSTITUTE")) {
          fread_institute(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Institutes: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open institute.txt", 0);
      exit(0);
    }
  }
  void save_institutes(bool backup) {
    FILE *fpout;
    char buf[MSL];

    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/institute.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/institute.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/institute.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/institute.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/institute.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/institute.txt");
      else
      sprintf(buf, "../data/back7/institute.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open institute.txt for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/institute.txt", "w")) == NULL) {
        bug("Cannot open institute.txt for writing", 0);
        return;
      }
    }

    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      if ((*it)->asylum_prestige == 0 && (*it)->college_prestige == 0
          && (*it)->college_rank == 0 && (*it)->asylum_rank == 0 && (*it)->asylum_status == 0)
      continue;

      if ((*it)->inactivity > 300 && !character_exists((*it)->name))
      continue;

      fprintf(fpout, "#INSTITUTE\n");
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Surname %s~\n", (*it)->surname);
      if ((*it)->inactivity > 0)
      fprintf(fpout, "Inactivity %d\n", (*it)->inactivity);
      if ((*it)->asylum_prestige > 0)
      fprintf(fpout, "AsylumPrestige %d\n", (*it)->asylum_prestige);
      if ((*it)->asylum_rank > 0)
      fprintf(fpout, "AsylumRank %d\n", (*it)->asylum_rank);
      if ((*it)->college_prestige > 0)
      fprintf(fpout, "CollegePrestige %d\n", (*it)->college_prestige);
      if ((*it)->college_rank > 0)
      fprintf(fpout, "CollegeRank %d\n", (*it)->college_rank);
      if ((*it)->college_pending_prestige > 0)
      fprintf(fpout, "CollegePendingPrestige %d\n", (*it)->college_pending_prestige);
      if ((*it)->arrest_cost > 0)
      fprintf(fpout, "ArrestCost %d\n", (*it)->arrest_cost);
      if ((*it)->asylum_status > 0)
      fprintf(fpout, "AsylumStatus %d\n", (*it)->asylum_status);
      if ((*it)->asylum_basecost > 0)
      fprintf(fpout, "AsylumBaseCost %d\n", (*it)->asylum_basecost);
      if ((*it)->asylum_extracost > 0)
      fprintf(fpout, "AsylumExtraCost %d\n", (*it)->asylum_extracost);
      if (safe_strlen((*it)->asylum_owner) > 1)
      fprintf(fpout, "AsylumOwner %s~\n", (*it)->asylum_owner);
      if ((*it)->nophone > 0)
      fprintf(fpout, "NoPhone %d\n", (*it)->nophone);
      if (safe_strlen((*it)->college_committer) > 1)
      fprintf(fpout, "CollegeCommitter %s~\n", (*it)->college_committer);
      if ((*it)->asylum_suspended > 0)
      fprintf(fpout, "AsylumSuspended %d\n", (*it)->asylum_suspended);
      if ((*it)->asylum_inactive > 0)
      fprintf(fpout, "AsylumInactive %d\n", (*it)->asylum_inactive);
      if ((*it)->asylum_protest_days > 0)
      fprintf(fpout, "AsylumProtestDays %d\n", (*it)->asylum_protest_days);
      if ((*it)->college_suspended > 0)
      fprintf(fpout, "CollegeSuspended %d\n", (*it)->college_suspended);
      if ((*it)->college_suspend_rank > 0)
      fprintf(fpout, "CollegeSuspendRank %d\n", (*it)->college_suspend_rank);
      if ((*it)->asylum_suspend_rank > 0)
      fprintf(fpout, "AsylumSuspendRank %d\n", (*it)->asylum_suspend_rank);
      if ((*it)->asylum_staff_prestige > 0)
      fprintf(fpout, "AsylumStaffprestige %d\n", (*it)->asylum_staff_prestige);
      if ((*it)->college_staff_prestige > 0)
      fprintf(fpout, "CollegeStaffPrestige %d\n", (*it)->college_staff_prestige);
      if ((*it)->commit_date > 0)
      fprintf(fpout, "CommitDate %d\n", (*it)->commit_date);
      if (safe_strlen((*it)->asylum_notes) > 1)
      fprintf(fpout, "AsylumNotes %s~\n", (*it)->asylum_notes);
      if (safe_strlen((*it)->asylum_pending_notes) > 1)
      fprintf(fpout, "AsylumPendingNotes %s~\n", (*it)->asylum_pending_notes);
      if (safe_strlen((*it)->asylum_diagnoses) > 1)
      fprintf(fpout, "AsylumDiagnoses %s~\n", (*it)->asylum_diagnoses);
      if (safe_strlen((*it)->college_notes) > 1)
      fprintf(fpout, "CollegeNotes %s~\n", (*it)->college_notes);
      if (safe_strlen((*it)->college_pending_notes) > 1)
      fprintf(fpout, "CollegePendingNotes %s~\n", (*it)->college_pending_notes);
      if (safe_strlen((*it)->arrest_notes) > 1)
      fprintf(fpout, "ArrestNotes %s~\n", (*it)->arrest_notes);
      if ((*it)->college_pending_suma > 0)
      fprintf(fpout, "CollegePendingSuma %d\n", (*it)->college_pending_suma);
      if ((*it)->college_grade > 0)
      fprintf(fpout, "CollegeGrade %d\n", (*it)->college_grade);
      if ((*it)->college_group > 0)
      fprintf(fpout, "CollegeGroup %d\n", (*it)->college_group);
      if((*it)->college_power > 0)
      fprintf(fpout, "CollegePower %d\n", (*it)->college_power);
      if((*it)->college_immune > 0)
      fprintf(fpout, "CollegeImmune %d\n", (*it)->college_immune);
      if((*it)->clinic_breakout > 0)
      fprintf(fpout, "ClinicBreakout %d\n", (*it)->clinic_breakout);
      if((*it)->college_house > 0)
      fprintf(fpout, "CollegeHouse %d\n", (*it)->college_house);
      if((*it)->dorm_room > 0)
      fprintf(fpout, "DormRoom %d\n", (*it)->dorm_room);
      if ((*it)->college_suma > 0)
      fprintf(fpout, "CollegeSuma %d\n", (*it)->college_suma);
      if ((*it)->age > 0)
      fprintf(fpout, "Age %d\n", (*it)->age);
      if ((*it)->gender > 0)
      fprintf(fpout, "Gender %d\n", (*it)->gender);
      if ((*it)->race > 0)
      fprintf(fpout, "Race %d\n", (*it)->race);
      if ((*it)->modifier > 0)
      fprintf(fpout, "Modifier %d\n", (*it)->modifier);
      if ((*it)->clique_role > 0)
      fprintf(fpout, "CliqueRole %d\n", (*it)->clique_role);
      if ((*it)->school_habit > 0)
      fprintf(fpout, "SchoolHabit %d\n", (*it)->school_habit);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    if (backup == FALSE)
    save_institutes(TRUE);
  }

  int active_staff(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING) {
        continue;
      }

      victim = CH(d);

      if (victim == NULL) {
        continue;
      }
      if (IS_NPC(victim)) {
        continue;
      }
      if (victim->in_room == NULL) {
        continue;
      }
      if (is_gm(victim)) {
        continue;
      }
      if (victim->in_room != room) {
        continue;
      }
      if (victim->pcdata->institute_action != 0) {
        pop++;
      }
    }
    return UMAX(1, pop);
  }

  bool can_institute_teach(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;
    if (room == NULL) {
      return FALSE;
    }

    CHAR_DATA *victim;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;
      if (IS_NPC(victim)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }

      if (college_staff(ch, FALSE)) {
        if (college_student(victim, FALSE)) {
          return TRUE;
        }
      }

      if (college_student(ch, FALSE) && college_student(victim, FALSE)) {
        if (college_student_prestige(ch) >
            college_student_prestige(victim) * 15 / 10 || college_group(ch, FALSE) == COLLEGE_OVERACHIEVER) {
          return TRUE;
        }
      }

      if (college_student(ch, FALSE)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool is_institute_taught(CHAR_DATA *victim) {
    ROOM_INDEX_DATA *room = victim->in_room;
    if (room == NULL) {
      return FALSE;
    }
    if (IS_FLAG(victim->comm, COMM_AFK)) {
      return FALSE;
    }

    CHAR_DATA *ch;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();++it) {
      ch = *it;
      if (IS_NPC(ch)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }
      if (ch->pcdata->institute_action != INSTITUTE_TEACH) {
        continue;
      }

      if (college_staff(ch, FALSE)) {
        if (college_student(victim, FALSE)) {
          return TRUE;
        }
      }

      if (college_student(ch, FALSE) && college_student(victim, FALSE)) {
        if (college_student_prestige(ch) >
            college_student_prestige(victim) * 12 / 10) {
          return TRUE;
        }
      }
      else if (college_student(ch, FALSE)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool can_institute_trauma(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;
    if (room == NULL) {
      return FALSE;
    }

    CHAR_DATA *victim;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;
      if (IS_NPC(victim)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }
      if(clinic_staff(ch, FALSE) && clinic_patient(victim))
      return TRUE;
    }
    return FALSE;
  }

  bool is_institute_trauma(CHAR_DATA *victim) {
    ROOM_INDEX_DATA *room = victim->in_room;
    if (room == NULL) {
      return FALSE;
    }
    if (IS_FLAG(victim->comm, COMM_AFK)) {
      return FALSE;
    }

    CHAR_DATA *ch;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      ch = *it;
      if (IS_NPC(ch)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }
      if (ch->pcdata->institute_action != INSTITUTE_TRAUMA) {
        continue;
      }

      if (!IS_FLAG(victim->comm, COMM_AFK)) {
        if (clinic_staff(ch, FALSE) && clinic_patient(victim)) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool can_institute_treat(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;
    if (room == NULL) {
      return FALSE;
    }

    CHAR_DATA *victim;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;
      if (IS_NPC(victim)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }

      if (clinic_staff(ch, FALSE) && (clinic_patient(victim) || (is_super(victim) && college_student(victim, FALSE)))) {
        return TRUE;
      }
      else if (college_student(ch, FALSE) && clinic_patient(victim)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool is_institute_treating(CHAR_DATA *victim) {
    ROOM_INDEX_DATA *room = victim->in_room;
    if (room == NULL) {
      return FALSE;
    }
    if (IS_FLAG(victim->comm, COMM_AFK)) {
      return FALSE;
    }

    CHAR_DATA *ch;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      ch = *it;
      if (IS_NPC(ch)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }
      if (ch->pcdata->institute_action != INSTITUTE_TREAT) {
        continue;
      }

      if (clinic_staff(ch, FALSE) && (clinic_patient(victim) || (is_super(victim) && college_student(victim, FALSE)))) {
        return TRUE;
      }
      else if (college_student(ch, FALSE) && clinic_patient(victim)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool can_institute_experiment(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;
    if (room == NULL) {
      return FALSE;
    }

    CHAR_DATA *victim;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;
      if (IS_NPC(victim)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }

      if ((clinic_staff(ch, FALSE) || college_staff(ch, FALSE)) && clinic_patient(victim)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  bool is_institute_experimenting(CHAR_DATA *victim) {
    ROOM_INDEX_DATA *room = victim->in_room;
    if (room == NULL) {
      return FALSE;
    }
    if (IS_FLAG(victim->comm, COMM_AFK)) {
      return FALSE;
    }

    CHAR_DATA *ch;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      ch = *it;
      if (IS_NPC(ch)) {
        continue;
      }
      if (ch == victim) {
        continue;
      }
      if (ch->pcdata->institute_action != INSTITUTE_EXPERIMENT) {
        continue;
      }

      if ((clinic_staff(ch, FALSE) || college_staff(ch, FALSE)) && (clinic_patient(victim))) {
        return TRUE;
      }
    }
    return FALSE;
  }

  void institute_xp(CHAR_DATA *ch, int amount) {
    char buf[MSL];
    amount = amount * 2 / 3;
    if (ch->faction != 0) {
      amount /= 2;
    }

    if (IS_FLAG(ch->comm, COMM_PRIVATE)) {
      if (institute_room(ch->in_room)) {
        INSTITUTE_TYPE *ins;
        for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
        it != InVect.end(); ++it) {
          ins = (*it);
          if (!str_cmp(ch->name, ins->name)) {
            if (clinic_staff(ch, FALSE))
            ins->asylum_staff_prestige -= amount / 2;
            if (college_staff(ch, FALSE))
            ins->college_staff_prestige -= amount / 2;
          }
        }
      }
      return;
    }
    if (is_institute_experimenting(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ch->name, ins->name)) {
          if (clinic_patient(ch)) {
            ins->asylum_prestige += amount;
            sprintf(buf, "INSTITUTEEXP Asylum, experimented on: %d", amount);
            log_string(buf);
          }
          return;
        }
      }
    }
    if (ch->pcdata->institute_action == INSTITUTE_EXPERIMENT && can_institute_experiment(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ch->name, ins->name)) {
          if (clinic_staff(ch, FALSE)) {
            ins->asylum_staff_prestige +=
            (amount * 3 / 2) / active_staff(ch->in_room);
            sprintf(buf, "INSTITUTEEXP Asylum, experimenting: %d", amount);
            log_string(buf);
          }
          else if (college_staff(ch, FALSE)) {
            ins->college_staff_prestige +=
            (amount * 3 / 2) / active_staff(ch->in_room);
            sprintf(buf, "INSTITUTEEXP College, experimenting: %d", amount);
            log_string(buf);
          }

          return;
        }
      }
    }
    if (is_institute_treating(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ch->name, ins->name)) {
          if (clinic_patient(ch)) {
            ins->asylum_prestige += amount;
            sprintf(buf, "INSTITUTEEXP Asylum, treated: %d", amount);
            log_string(buf);
          }
          else if (is_super(ch) && college_student(ch, FALSE)) {
            ins->college_prestige += amount;
            sprintf(buf, "INSTITUTEEXP College, treated: %d", amount);
            log_string(buf);
          }

          return;
        }
      }
    }
    if (ch->pcdata->institute_action == INSTITUTE_TREAT && can_institute_treat(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ch->name, ins->name)) {
          if (clinic_staff(ch, FALSE)) {
            ins->asylum_staff_prestige +=
            (amount * 2 / 3) / active_staff(ch->in_room);
            sprintf(buf, "INSTITUTEEXP Asylum, treating: %d", amount);
            log_string(buf);
          }
          else if (college_student(ch, FALSE)) {
            ins->college_prestige += amount;
            sprintf(buf, "INSTITUTEEXP College, treating: %d", amount);
            log_string(buf);
          }

          return;
        }
      }
    }

    if (is_institute_taught(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ch->name, ins->name)) {
          if (college_student(ch, FALSE)) {
            ins->college_pending_prestige += amount;
            sprintf(buf, "INSTITUTEEXP College taught: %d", amount);
            log_string(buf);
          }
          return;
        }
      }
    }
    if (ch->pcdata->institute_action == INSTITUTE_TEACH && can_institute_teach(ch)) {
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ch->name, ins->name)) {
          if (college_student(ch, FALSE)) {
            ins->college_pending_prestige += amount;
            sprintf(buf, "INSTITUTEEXP College teaching: %d", amount);
            log_string(buf);
          }
          else if (college_staff(ch, FALSE)) {
            ins->college_staff_prestige += amount / active_staff(ch->in_room);
            sprintf(buf, "INSTITUTEEXP College teaching: %d", amount);
            log_string(buf);
          }
          return;
        }
      }
    }
  }

  void wanted_check(CHAR_DATA *ch) {
    INSTITUTE_TYPE *ins;
    if (ch->in_room->z < 0)
    return;

    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if(ins->clinic_breakout > 0)
      {
        ins->clinic_breakout--;
        if(ins->clinic_breakout <= 0)
        {
          send_to_char("Some Deputies come and escort you away.\n\r", ch);
          act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
          commit_char(ch);
        }
      }
    }

    if (ch->in_room->area->vnum < 13 || ch->in_room->area->vnum > 18)
    return;
    if (prop_from_room(ch->in_room) != NULL && prop_from_room(ch->in_room)->type == PROP_HOUSE)
    return;

    if (ch->recent_moved > -300)
    return;

    if (clinic_patient(ch))
    return;

    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (!str_cmp(ins->name, ch->name) && ins->asylum_status == ASYLUM_WANTED) {
        if (ch->shape != SHAPE_HUMAN && !is_neutralized(ch))
        human_transformation(ch);

        ins->asylum_prestige = UMAX(1, ins->asylum_prestige);
        ins->asylum_inactive = 0;
        ins->asylum_status = ASYLUM_REMOTECOMMIT;
        ins->arrest_cost = 0;
        free_string(ins->asylum_notes);
        ins->asylum_notes = str_dup(ins->arrest_notes);
        send_to_char("Some Deputies come and escort you away.\n\r", ch);
        act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
        commit_char(ch);
      }
    }
  }

  void clinic_charge(CHAR_DATA *ch) {
    char buf[MSL];
    sprintf(buf, "PRISONER: %s charge.", ch->name);
    log_string(buf);

    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (!str_cmp(ins->name, ch->name)) {
        if (ins->asylum_status == ASYLUM_REMOTECOMMIT || (safe_strlen(ins->asylum_owner) > 2 && ins->asylum_status > 0)) {
          int cost = 100;
          if (ch->race == RACE_WILDLING || ch->race == RACE_TIMESWEPT || ch->race == RACE_DREAMCHILD || ch->modifier == MODIFIER_CHEMICAL)
          cost /= 10;
          if (ins->asylum_extracost > 0) {
            cost += UMIN(ins->asylum_extracost, 500) * 2;
            ins->asylum_extracost -= UMIN(500, ins->asylum_extracost);
            if (ins->asylum_protest_days > 0) {
              for (int i = 0; i < ins->asylum_protest_days; i++)
              cost = cost * 15 / 10;
            }
            ins->asylum_protest_days++;
          }
          if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_CARDINAL)
          cost *= 5;
          if (has_weakness(NULL, ch))
          cost = cost * 3 / 2;
          else if (is_weakness(NULL, ch))
          cost = cost * 2 / 3;
          FACTION_TYPE *fac = clan_lookup_name(ins->asylum_owner);
          if (fac != NULL && fac->stasis == 0 && fac->resource > (8000 + cost / 10)) {
            use_resources(manip_cost(fac, cost / 10), fac->vnum, NULL, "clinic fees");
            char buf[MSL];
            sprintf(buf, "PRISONER: %d paid for %s by %s.", cost, ch->name, ins->asylum_owner);
            log_string(buf);
          }
          else if (fac != NULL) {
            char buf[MSL];
            ins->asylum_prestige = 0;
            ins->asylum_status = 0;
            send_to_char("You're informed you're being released from the clinic.\n\r", ch);
            sprintf(buf, "PRISONER: %s owned by %s released, no faction.", ch->name, ins->asylum_owner);
            log_string(buf);
          }
          else if (fac == NULL && bank_check(ins->asylum_owner) >= cost) {
            house_charge(ins->asylum_owner, cost);
            char buf[MSL];
            sprintf(buf, "You pay %d in ongoing costs for the detainment of %s.", cost, ins->name);
            message_to_char(ins->asylum_owner, buf);
            sprintf(buf, "PRISONER: %d paid for %s by %s.", cost, ch->name, ins->asylum_owner);
            log_string(buf);
          }
          else {
            char buf[MSL];
            sprintf(buf, "You are unable to pay the %d to keep %s detained.", cost, ins->name);
            message_to_char(ins->asylum_owner, buf);
            ins->asylum_prestige = 0;
            ins->asylum_status = 0;
            send_to_char("You're informed you're being released from the clinic.\n\r", ch);
            sprintf(buf, "PRISONER: %s owned by %s released, not enough funds.", ch->name, ins->asylum_owner);
            log_string(buf);
          }
        }
        else if (ins->asylum_protest_days > 0)
        ins->asylum_protest_days--;
      }
    }
  }

  void solitary_check(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return;

    if (ch->in_room->vnum == ROOM_INS_CELL_TWO || ch->in_room->vnum == ROOM_INS_CELL_FOUR || ch->in_room->vnum == ROOM_INS_CELL_SIX || ch->in_room->vnum == ROOM_INS_CELL_EIGHT) {
      ch->pcdata->solitary_time--;
      if (ch->pcdata->solitary_time <= 0) {
        EXIT_DATA *pexit = ch->in_room->exit[DIR_NORTH];
        if (IS_SET(pexit->exit_info, EX_CLOSED)) {
          REMOVE_BIT(pexit->exit_info, EX_CLOSED);
          REMOVE_BIT(pexit->u1.to_room->exit[DIR_SOUTH]->exit_info, EX_CLOSED);
          send_to_char("An orderly comes by and opens the door to let you out.\n\r", ch);
          if (IS_FLAG(ch->act, PLR_BOUNDFEET))
          REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
        }
      }
    }
    if (ch->in_room->vnum == ROOM_INS_CELL_ONE || ch->in_room->vnum == ROOM_INS_CELL_THREE || ch->in_room->vnum == ROOM_INS_CELL_FIVE || ch->in_room->vnum == ROOM_INS_CELL_SEVEN) {
      ch->pcdata->solitary_time--;
      if (ch->pcdata->solitary_time <= 0) {
        EXIT_DATA *pexit = ch->in_room->exit[DIR_SOUTH];
        if (IS_SET(pexit->exit_info, EX_CLOSED)) {
          REMOVE_BIT(pexit->exit_info, EX_CLOSED);
          REMOVE_BIT(pexit->u1.to_room->exit[DIR_NORTH]->exit_info, EX_CLOSED);
          send_to_char("An orderly comes by and opens the door to let you out.\n\r", ch);
          if (IS_FLAG(ch->act, PLR_BOUNDFEET))
          REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
        }
      }
    }
  }

  char *const victimize_actions[] = {
    "", "Mild Pain", "Severe Pain", "Banter",
    "Relief", "Compliment", "Reward", "Question",
    "Immediate Request", "Delayed Request", "Bleed",
    "Verbally Tease", "Verbally Degrade", "Verbally Insult",
    "Physically Embarass", "Physically Humiliate",
    "Emotionally Disturb", "Physically Discomfort",
    "Asphyxiate", "Strain"
  };
  char *const victimize_responses[] = {
    "", "Be Stoically Unresponsive",
    "Scream in pain or discomfort",
    "Scream for help", "Bellow in pain or discomfort",
    "Grunt in pain or discomfort", "Grit teeth",
    "Beg for relief", "Beg for freedom",
    "Bargain for relief", "Bargain for freedom",
    "Threaten for relief", "Threaten for freedom",
    "Insult", "Struggle against bonds",
    "Attempt to lash out physically at captor",
    "Agree to request", "Pretend to agree to request",
    "Answer truthfully", "Answer falsely", "Banter back",
    "Distract", "Blush in embarassment or humiliation",
    "Squirm in embarassment or humilation",
    "Thank captor", "Visibly enjoy treatment",
    "Unsuccessfully attempt to hide visible enjoyment of treatment",
    "Successfully hide enjoyment of treatment", "Pass out",
    "Return Compliment", "Try to charm your way to freedom",
    "Argue", "Provide Intel"
  };

  char *const victimize_responses_you[] = {
    "", "Remain Stoically Unresponsive", "Scream in pain or discomfort",
    "Scream for help", "Bellow in pain or discomfort",
    "Grunt in pain or discomfort", "Grit your teeth", "Beg for relief",
    "Beg for freedom", "Bargain for relief", "Bargain for freedom",
    "Threaten for relief", "Threaten for freedom", "Insult",
    "Struggle against your bonds",
    "Attempt to lash out physically at captor",
    "Agree to their request", "Pretend to agree to their request",
    "Answer truthfully", "Answer falsely", "Banter back", "Distract",
    "Blush in embarassment or humiliation",
    "Squirm in embarassment or humilation", "Thank your captor",
    "Visibly enjoy treatment",
    "Unsuccessfully attempt to hide visible enjoyment of treatment",
    "Successfully hide your enjoyment of treatment", "Pass out",
    "Return the Compliment", "Try to charm your way to freedom",
    "Argue", "Provide Intel"
  };

  char *const victimize_responses_them[] = {
    "", "Remains Stoically Unresponsive", "Screams", "Screams for help",
    "Bellows", "Grunts", "Grits their teeth", "Begs for relief",
    "Begs for freedom", "Bargains for relief", "Bargains for freedom",
    "Threatens for relief", "Threatens for freedom", "Insults",
    "Struggles against their bonds", "Attempts to lash out physically",
    "Agrees to the request", "Agrees to the request",
    "Answers the question", "Answers the question", "Banters back",
    "Distract", "Blushes", "Squirms", "Thanks you",
    "Visibly enjoys the treatment",
    "Unsuccessfully attempts to hide their visible enjoyment of treatment",
    "Does not enjoy the treatment", "Passes out", "Returns the Compliment",
    "Tries to charm their way to freedom", "Argues", "Provides intel"
  };

  int response_value(int type, int response) {
    if (response == RESPONSE_STOIC)
    return 0;

    if (response == RESPONSE_INTEL)
    return 75;

    if (type == VICTIMIZE_MILDPAIN || type == VICTIMIZE_BLEED) {
      if (response == RESPONSE_SCREAMPAIN)
      return 75;
      else if (response == RESPONSE_SCREAMHELP)
      return 70;
      else if (response == RESPONSE_BELLOWPAIN)
      return 74;
      else if (response == RESPONSE_GRUNT)
      return 71;
      else if (response == RESPONSE_GRITTEETH)
      return 67;
      else if (response == RESPONSE_BEGRELIEF)
      return 81;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINRELIEF)
      return 79;
      else if (response == RESPONSE_BARGAINFREE)
      return 78;
      else if (response == RESPONSE_CHARMFREE)
      return 68;
      else if (response == RESPONSE_THREATENRELIEF)
      return 30;
      else if (response == RESPONSE_THREATENFREE)
      return 10;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 72;
      else if (response == RESPONSE_LASHOUT)
      return 35;
      else if (response == RESPONSE_DISTRACT)
      return 50;
      else if (response == RESPONSE_PASSOUT)
      return 46;
    }
    else if (type == VICTIMIZE_SEVEREPAIN) {
      if (response == RESPONSE_SCREAMPAIN)
      return 100;
      else if (response == RESPONSE_SCREAMHELP)
      return 82;
      else if (response == RESPONSE_BELLOWPAIN)
      return 90;
      else if (response == RESPONSE_GRUNT)
      return 76;
      else if (response == RESPONSE_GRITTEETH)
      return 68;
      else if (response == RESPONSE_BEGRELIEF)
      return 97;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINRELIEF)
      return 78;
      else if (response == RESPONSE_BARGAINFREE)
      return 72;
      else if (response == RESPONSE_THREATENRELIEF)
      return 50;
      else if (response == RESPONSE_THREATENFREE)
      return 20;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 88;
      else if (response == RESPONSE_LASHOUT)
      return 49;
      else if (response == RESPONSE_PASSOUT)
      return 67;
    }
    else if (type == VICTIMIZE_BANTER) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_THREATENFREE)
      return 20;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 50;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_BANTER)
      return 95;
    }
    else if (type == VICTIMIZE_RELIEF) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_THANK)
      return 100;
    }
    else if (type == VICTIMIZE_COMPLIMENT) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_THANK)
      return 99;
      else if (response == RESPONSE_RETURNCOMPLIMENT)
      return 99;
    }
    else if (type == VICTIMIZE_REWARD) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_THANK)
      return 99;
      else if (response == RESPONSE_ENJOY)
      return 100;
      else if (response == RESPONSE_FAILHIDDENENJOY)
      return 98;
      else if (response == RESPONSE_HIDDENENJOY)
      return 91;
    }
    else if (type == VICTIMIZE_QUESTION) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_ANSWERTRUE)
      return 100;
      else if (response == RESPONSE_ANSWERFALSE)
      return 81;
    }
    else if (type == VICTIMIZE_REQUESTNOW) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_AGREE)
      return 100;
    }
    else if (type == VICTIMIZE_REQUESTLATER) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_AGREE)
      return 100;
      else if (response == RESPONSE_FAKEAGREE)
      return 81;
    }
    else if (type == VICTIMIZE_TEASE) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 15;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_BLUSH)
      return 98;
      else if (response == RESPONSE_SQUIRM)
      return 99;
      else if (response == RESPONSE_ARGUE)
      return 50;
    }
    else if (type == VICTIMIZE_DEGRADE) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 15;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_BLUSH)
      return 98;
      else if (response == RESPONSE_SQUIRM)
      return 99;
      else if (response == RESPONSE_ARGUE)
      return 50;
    }
    else if (type == VICTIMIZE_INSULT) {
      if (response == RESPONSE_SCREAMHELP)
      return 20;
      else if (response == RESPONSE_BEGFREE)
      return 50;
      else if (response == RESPONSE_BARGAINFREE)
      return 60;
      else if (response == RESPONSE_CHARMFREE)
      return 59;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 10;
      else if (response == RESPONSE_LASHOUT)
      return 26;
      else if (response == RESPONSE_BLUSH)
      return 58;
      else if (response == RESPONSE_SQUIRM)
      return 51;
      else if (response == RESPONSE_ARGUE)
      return 20;
    }
    else if (type == VICTIMIZE_EMBARASS) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 15;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_BLUSH)
      return 98;
      else if (response == RESPONSE_SQUIRM)
      return 99;
    }
    else if (type == VICTIMIZE_HUMILIATE) {
      if (response == RESPONSE_SCREAMHELP)
      return 50;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINFREE)
      return 90;
      else if (response == RESPONSE_CHARMFREE)
      return 79;
      else if (response == RESPONSE_INSULT)
      return 15;
      else if (response == RESPONSE_STRUGGLE)
      return 30;
      else if (response == RESPONSE_LASHOUT)
      return 46;
      else if (response == RESPONSE_BLUSH)
      return 98;
      else if (response == RESPONSE_SQUIRM)
      return 99;
    }
    else if (type == VICTIMIZE_DISTURB) {
      if (response == RESPONSE_SCREAMPAIN)
      return 100;
      else if (response == RESPONSE_SCREAMHELP)
      return 82;
      else if (response == RESPONSE_BELLOWPAIN)
      return 90;
      else if (response == RESPONSE_GRUNT)
      return 76;
      else if (response == RESPONSE_GRITTEETH)
      return 68;
      else if (response == RESPONSE_BEGRELIEF)
      return 97;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINRELIEF)
      return 78;
      else if (response == RESPONSE_BARGAINFREE)
      return 72;
      else if (response == RESPONSE_THREATENRELIEF)
      return 50;
      else if (response == RESPONSE_THREATENFREE)
      return 20;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 88;
      else if (response == RESPONSE_LASHOUT)
      return 49;
      else if (response == RESPONSE_PASSOUT)
      return 67;
    }
    else if (type == VICTIMIZE_DISCOMFORT) {
      if (response == RESPONSE_SCREAMPAIN)
      return 75;
      else if (response == RESPONSE_SCREAMHELP)
      return 70;
      else if (response == RESPONSE_BELLOWPAIN)
      return 74;
      else if (response == RESPONSE_GRUNT)
      return 71;
      else if (response == RESPONSE_GRITTEETH)
      return 67;
      else if (response == RESPONSE_BEGRELIEF)
      return 81;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINRELIEF)
      return 79;
      else if (response == RESPONSE_BARGAINFREE)
      return 78;
      else if (response == RESPONSE_CHARMFREE)
      return 68;
      else if (response == RESPONSE_THREATENRELIEF)
      return 30;
      else if (response == RESPONSE_THREATENFREE)
      return 10;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 72;
      else if (response == RESPONSE_LASHOUT)
      return 35;
      else if (response == RESPONSE_DISTRACT)
      return 50;
      else if (response == RESPONSE_PASSOUT)
      return 46;
    }
    else if (type == VICTIMIZE_ASPHYXIATE) {
      if (response == RESPONSE_GRITTEETH)
      return 68;
      else if (response == RESPONSE_STRUGGLE)
      return 88;
      else if (response == RESPONSE_LASHOUT)
      return 49;
      else if (response == RESPONSE_PASSOUT)
      return 67;
    }
    else if (type == VICTIMIZE_STRAIN) {
      if (response == RESPONSE_SCREAMPAIN)
      return 75;
      else if (response == RESPONSE_SCREAMHELP)
      return 70;
      else if (response == RESPONSE_BELLOWPAIN)
      return 74;
      else if (response == RESPONSE_GRUNT)
      return 71;
      else if (response == RESPONSE_GRITTEETH)
      return 67;
      else if (response == RESPONSE_BEGRELIEF)
      return 81;
      else if (response == RESPONSE_BEGFREE)
      return 80;
      else if (response == RESPONSE_BARGAINRELIEF)
      return 79;
      else if (response == RESPONSE_BARGAINFREE)
      return 78;
      else if (response == RESPONSE_CHARMFREE)
      return 68;
      else if (response == RESPONSE_THREATENRELIEF)
      return 30;
      else if (response == RESPONSE_THREATENFREE)
      return 10;
      else if (response == RESPONSE_INSULT)
      return 5;
      else if (response == RESPONSE_STRUGGLE)
      return 72;
      else if (response == RESPONSE_LASHOUT)
      return 35;
      else if (response == RESPONSE_DISTRACT)
      return 50;
      else if (response == RESPONSE_PASSOUT)
      return 46;
    }
    return -1;
  }

  _DOFUN(do_victimtest) {
    if (safe_strlen(argument) < 2) {
      static char string[MSL];
      char buf[MSL];
      string[0] = '\0';
      sprintf(buf, "Mild Pain: e.g. Pinches, slaps, ice cubes.\n\r");
      strcat(string, buf);
      sprintf(buf, "Severe Pain: e.g. Electrocution, Cutting, Whipping.\n\r");
      strcat(string, buf);
      sprintf(buf, "Banter: Engage target in normal, casual conversation.\n\r");
      strcat(string, buf);
      sprintf(buf, "Relief: Relieve some previous pain or discomfort.\n\r");
      strcat(string, buf);
      sprintf(
      buf, "Compliment: Give the target some sort of compliment or praise.\n\r");
      strcat(string, buf);
      sprintf(buf, "Reward: Give the target some sort of nice, pleasurable or rewarding experience.\n\r");
      strcat(string, buf);
      sprintf(buf, "Question: As the target a non-rhetorical question.\n\r");
      strcat(string, buf);
      sprintf(buf, "Immediate Request: Request the target perform some action or behavioral change immediately.\n\r");
      strcat(string, buf);
      sprintf(
      buf, "Delayed Request: Request the target perform some action or behavioral change at some later time when you are not longer present.\n\r");
      strcat(string, buf);
      sprintf(buf, "Verbally Tease: Tease the target in such a way as to bring about some mild embarassment.\n\r");
      strcat(string, buf);
      sprintf(buf, "Verbally Degrade: Degrade the target in such a way as to bring about major embarassment or humiliation.\n\r");
      strcat(string, buf);
      sprintf(buf, "Verbally Insult: Insult the target to attempt to cause psychological distress.\n\r");
      strcat(string, buf);
      sprintf(buf, "Physically Embarass: Do something to the target to cause them some minor embarassment.\n\r");
      strcat(string, buf);
      sprintf(buf, "Physically Humilate: Do something to the target to cause them some major embarassment or humiliation.\n\r");
      strcat(string, buf);
      sprintf(buf, "Emotionally Disturb: Mentally disturb the target without using pain/discomfort such as by exploiting common or particular phobias. e.g. Insects, darkness, confined spaces.\n\r");
      strcat(string, buf);
      sprintf(buf, "Physically Discomfort: Cause the victim physical discomfort short of actual pain, such as by being bound in an uncomfortable or stressful position.\n\r");
      strcat(string, buf);
      sprintf(
      buf, "Asphyxiate: Limit or temporarily stop the target's breathing, such as through drowning, waterboarding, choking, or smothering.\n\r");
      strcat(string, buf);
      sprintf(buf, "Strain: Put strain on the target's body/muscles, such as by forcing them to stand on tip toes for a prolonged period.\n\r");
      strcat(string, buf);
      page_to_char(string, ch);
      return;
    }

    for (int i = 0; i <= VICTIMIZE_MAX; i++) {
      if (!str_cmp(argument, victimize_actions[i])) {
        int highest = 101;
        for (int x = 1; x < 20; x++) {
          int max = -1;
          int maxpoint = -1;
          for (int y = 0; y <= RESPONSE_MAX; y++) {
            if (response_value(i, y) >= highest)
            continue;
            if (response_value(i, y) > max) {
              max = response_value(i, y);
              maxpoint = y;
            }
          }
          highest = max;
          if (maxpoint != -1) {
            printf_to_char(ch, "[%02d: %s]  ", x, victimize_responses[maxpoint]);
          }
        }
        send_to_char("\n\r", ch);
      }
    }
  }

  bool nice_victimize(int option) {
    if (option == VICTIMIZE_BANTER)
    return TRUE;
    if (option == VICTIMIZE_RELIEF)
    return TRUE;
    if (option == VICTIMIZE_COMPLIMENT)
    return TRUE;
    if (option == VICTIMIZE_REWARD)
    return TRUE;
    return FALSE;
  }

  void become_difficult(CHAR_DATA *ch) {
    int amount;
    if (is_dreaming(ch) && !physical_dreamer(ch)) {
      amount = 700 - ch->lf_taken;
    }
    else
    amount = 1000 - ch->lf_taken;
    amount = UMAX(amount, 100);
    int count = UMAX(1, nonhelpless_pop(ch->in_room));
    take_lifeforce(ch, amount, "Difficult Prisoner");
    ch->pcdata->intel /= 3;
    ch->pcdata->heroic = -50;
    if (get_tier(ch) == 1)
    amount /= 4;
    if (get_tier(ch) > 2 || (get_tier(ch) == 2 && ch->skills[SKILL_MENTALDISCIPLINE] > 0))
    amount = amount * 13 / 10;
    amount = amount * 4 / 10;
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

      if (ch == victim)
      continue;

      if (victim->in_room != ch->in_room && !is_dreaming(ch))
      continue;

      if (victim->pcdata->dream_room != ch->pcdata->dream_room && is_dreaming(ch))
      continue;

      if (is_helpless(victim) && !is_dreaming(victim))
      continue;

      if (is_pinned(victim))
      continue;

      if (victim->faction != 0) {
        FACTION_TYPE *tfac = clan_lookup(victim->faction);
        if (ch->faction != 0 && !are_allies(ch, victim) && generic_faction_vnum(ch->faction) ==
            generic_faction_vnum(victim->faction)) {
          tfac->last_intel = tfac->last_intel + (3600 * 24 * 5);
        }
      }

      if (is_super(victim) && get_tier(victim) > get_tier(ch) && !institute_room(ch->in_room)) {
        give_lifeforce(victim, amount / count, "difficult prisoner");
      }
      else {
        give_lifeforce(victim, (amount / count), "difficult prisoner");
        if (institute_room(ch->in_room))
        institute_victimize(victim, ch, amount / count);
      }
    }
    ch->pcdata->victimize_difficult_time = current_time + (3600 * 24 * 3);
    ch->pcdata->ill_count += amount;
  }

  _DOFUN(do_become) {
    if (!str_cmp(argument, "difficult prisoner")) {
      if (!is_helpless(ch) || nonhelpless_pop(ch->in_room) < 1) {
        if (!dream_slave(ch) && !is_pinned(ch)) {
          send_to_char("You're not being held prisoner.\n\r", ch);
          return;
        }
      }
      become_difficult(ch);
      take_lifeforce(ch, 100, "");
      act("$n becomes a difficult prisoner.", ch, NULL, NULL, TO_ROOM);
      act("You become a difficult prisoner.", ch, NULL, NULL, TO_CHAR);
      return;
    }
  }

  void handout_lifeforce_summary(CHAR_DATA *ch, int perc) {

    int amount = 1000 - ch->lf_taken;
    amount = amount * perc / 100;
    amount = UMAX(amount, 100);
    int count = UMAX(1, nonhelpless_pop(ch->in_room));
    take_lifeforce(ch, amount, "Summary Victim");
    if (get_tier(ch) == 1)
    amount /= 4;
    if(ch->spentkarma > 10000)
    amount = amount * 13 / 10;
    amount = amount * 7 / 10;
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

      if (ch == victim)
      continue;

      if (victim->in_room != ch->in_room && !is_dreaming(ch))
      continue;
      if (victim->pcdata->dream_room != ch->pcdata->dream_room && is_dreaming(ch))
      continue;

      if (is_helpless(victim) && !is_dreaming(victim))
      continue;

      if (is_pinned(victim))
      continue;

      if (is_super(victim) && get_tier(victim) > get_tier(ch))
      {
        give_lifeforce(victim, amount / count, "summary victimize");
        victim->pcdata->monster_fed += amount/count;
      }
      if (institute_room(ch->in_room))
      institute_victimize(victim, ch, amount / count);
      guest_match(victim);
    }
    ch->pcdata->ill_count += amount;
    guest_match(ch);

  }

  void handout_lifeforce(CHAR_DATA *ch) {

    int amount = 1000 - ch->lf_taken;
    if (is_dreaming(ch) && !physical_dreamer(ch))
    amount = 700 - ch->lf_taken;
    amount = UMAX(amount, 100);
    int count = UMAX(1, nonhelpless_pop(ch->in_room));
    take_lifeforce(ch, amount, "Pass out");
    if (get_tier(ch) == 1)
    amount /= 4;
    if(ch->spentkarma > 10000)
    amount = amount * 13 / 10;
    amount = amount * 5 / 10;
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

      if (ch == victim)
      continue;

      if (victim->in_room != ch->in_room && !is_dreaming(ch))
      continue;
      if (victim->pcdata->dream_room != ch->pcdata->dream_room && is_dreaming(ch))
      continue;

      if (is_helpless(victim) && !is_dreaming(victim))
      continue;

      if (is_pinned(victim))
      continue;

      if (is_super(victim) && get_tier(victim) > get_tier(ch) && !institute_room(ch->in_room))
      give_lifeforce(victim, amount / count, "prisoner pass out");
      else {
        give_lifeforce(victim, (amount / count), "prisoner pass out");
        if (institute_room(ch->in_room))
        institute_victimize(victim, ch, amount / count);
      }
      guest_match(victim);
    }
    ch->pcdata->ill_count += amount * 2;
    guest_match(ch);
  }

  _DOFUN(do_victimize) {
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    CHAR_DATA *victim = get_char_room(ch, NULL, arg1);
    if (is_dreaming(ch))
    victim = get_char_dream(ch, arg1);
    if (victim == NULL || (!is_helpless(victim) && !is_pinned(victim) && !dream_slave(victim)) || safe_strlen(argument) < 2) {
      sprintf(buf, "`WSyntax`x: Victimize (target) (option)\n`WOptions`x:\n\r");
      strcat(string, buf);
      sprintf(buf, "Mild Pain: e.g. Pinches, slaps, ice cubes.\n\r");
      strcat(string, buf);
      sprintf(buf, "Severe Pain: e.g. Electrocution, Cutting, Whipping.\n\r");
      strcat(string, buf);
      sprintf(buf, "Banter: Engage target in normal, casual conversation.\n\r");
      strcat(string, buf);
      sprintf(buf, "Relief: Relieve some previous pain or discomfort.\n\r");
      strcat(string, buf);
      sprintf(buf, "Bleed: Obtain some blood to be used in sanctifying areas.\n\r");
      strcat(string, buf);

      sprintf(
      buf, "Compliment: Give the target some sort of compliment or praise.\n\r");
      strcat(string, buf);
      sprintf(buf, "Reward: Give the target some sort of nice, pleasurable or rewarding experience.\n\r");
      strcat(string, buf);
      sprintf(buf, "Question: As the target a non-rhetorical question.\n\r");
      strcat(string, buf);
      sprintf(buf, "Immediate Request: Request the target perform some action or behavioral change immediately.\n\r");
      strcat(string, buf);
      sprintf(
      buf, "Delayed Request: Request the target perform some action or behavioral change at some later time when you are not longer present.\n\r");
      strcat(string, buf);
      sprintf(buf, "Verbally Tease: Tease the target in such a way as to bring about some mild embarassment.\n\r");
      strcat(string, buf);
      sprintf(buf, "Verbally Degrade: Degrade the target in such a way as to bring about major embarassment or humiliation.\n\r");
      strcat(string, buf);
      sprintf(buf, "Verbally Insult: Insult the target to attempt to cause psychological distress.\n\r");
      strcat(string, buf);
      sprintf(buf, "Physically Embarass: Do something to the target to cause them some minor embarassment.\n\r");
      strcat(string, buf);
      sprintf(buf, "Physically Humilate: Do something to the target to cause them some major embarassment or humiliation.\n\r");
      strcat(string, buf);
      sprintf(buf, "Emotionally Disturb: Mentally disturb the target without using pain/discomfort such as by exploiting common or particular phobias. e.g. Insects, darkness, confined spaces.\n\r");
      strcat(string, buf);
      sprintf(buf, "Physically Discomfort: Cause the victim physical discomfort short of actual pain, such as by being bound in an uncomfortable or stressful position.\n\r");
      strcat(string, buf);
      sprintf(
      buf, "Asphyxiate: Limit or temporarily stop the target's breathing, such as through drowning, waterboarding, choking, or smothering.\n\r");
      strcat(string, buf);
      sprintf(buf, "Strain: Put strain on the target's body/muscles, such as by forcing them to stand on tip toes for a prolonged period.\n\r");
      strcat(string, buf);
      page_to_char(string, ch);
      return;
    }
    if(!str_cmp(argument, "bleed") && victim->pcdata->last_victim_bled > current_time - (3600*24*4))
    {
      send_to_char("They have been bled too recently.\n\r", ch);
      return;
    }
    for (int i = 0; i <= VICTIMIZE_MAX; i++) {
      if (!str_cmp(argument, victimize_actions[i])) {

        ch->pcdata->victimize_char_point = victim;
        ch->pcdata->victimize_char_select = i;
        printf_to_char(ch, "You prepare to %s %s.\n\r", victimize_actions[i], PERS(victim, ch));
        return;
      }
    }
  }

  void victimize_emote_process(CHAR_DATA *ch) {

    if(!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_PREY)
    {
      if(ch->pcdata->prey_option >= PREY_OPTION_INJURE && ch->pcdata->prey_option <= PREY_OPTION_DASH && ch->pcdata->prey_option_cooldown < 30)
      process_prey_emote(ch);
    }
    if(!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_PREYING)
    {
      if(ch->pcdata->villain_option >= VILLAIN_OPTION_CHASE && ch->pcdata->villain_option <= VILLAIN_OPTION_AGGRESSIVE && ch->pcdata->villain_option_cooldown < 30)
      process_villain_emote(ch);
    }

    int cap = 1000;
    if (is_dreaming(ch) && !physical_dreamer(ch))
    cap = 700;
    if (ch->pcdata->victimize_char_select > 0 && ch->pcdata->victimize_char_point != NULL && (ch->pcdata->victimize_char_point->in_room == ch->in_room || ch->pcdata->victimize_char_point->pcdata->dream_room ==
          ch->pcdata->dream_room)) {
      int choice = ch->pcdata->victimize_char_select;
      CHAR_DATA *victim = ch->pcdata->victimize_char_point;
      if (!nice_victimize(choice)) {
        int take = 0;
        if ((is_super(ch) && get_tier(ch) > get_tier(victim)) || institute_room(victim->in_room))
        take = UMIN(cap - victim->lf_taken, 100 + victim->pcdata->victimize_lf_pending_loss);
        else
        take = UMIN(cap - victim->lf_taken, 50 + victim->pcdata->victimize_lf_pending_loss);
        take = UMAX(take, 10);
        take_lifeforce(victim, take, "Victimize Feeding");
        victim->pcdata->victimize_lf_pending_loss = 0;
      }
      else {
        if ((is_super(ch) && get_tier(ch) > get_tier(victim)) || institute_room(victim->in_room))
        victim->pcdata->victimize_lf_pending_loss +=
        UMAX(10, UMIN(100, 1000 - victim->lf_taken));
        else
        victim->pcdata->victimize_lf_pending_loss +=
        UMAX(10, UMIN(50, 1000 - victim->lf_taken));
      }
      ch->pcdata->victimize_char_select = 0;
      ch->pcdata->victimize_char_point = NULL;
      victim->pcdata->victimize_vic_timer = 120;
      victim->pcdata->victimize_vic_response_to = choice;
      victim->pcdata->victimize_vic_point = ch;
      printf_to_char(victim, "[%s is attempting to %s you. Choose a response by number from the options below, listed in order of reward for your victimizer, you have two minutes to decide.]\n\r", PERS(ch, victim), victimize_actions[choice]);
      printf_to_char(ch, "[You attempt to %s %s.]\n\r", victimize_actions[choice], PERS(victim, ch));
      if (choice == 2 && safe_strlen(victim->pcdata->maintained_target) > 1 && number_percent() % 3 == 0) {
        free_string(victim->pcdata->maintained_target);
        victim->pcdata->maintained_target = str_dup("");
        send_to_char("You lose focus on your spell.\n\r", victim);
      }
    }
    if (ch->pcdata->victimize_vic_select > 0 && ch->pcdata->victimize_vic_timer > 0 && ch->pcdata->victimize_vic_point != NULL && (ch->pcdata->victimize_vic_point->in_room == ch->in_room || ch->pcdata->victimize_vic_point->pcdata->dream_room ==
          ch->pcdata->dream_room)) {
      printf_to_char(ch, "[You %s]\n\r", victimize_responses_you[ch->pcdata->victimize_vic_select]);
      printf_to_char(ch->pcdata->victimize_vic_point, "[%s %s]", PERS(ch, ch->pcdata->victimize_vic_point), victimize_responses_them[ch->pcdata->victimize_vic_select]);
      if(ch->pcdata->victimize_vic_response_to == VICTIMIZE_BLEED && ch->pcdata->last_victim_bled < (current_time - (3600*12)))
      {
        ch->pcdata->last_victim_bled = current_time;
        OBJ_DATA *obj;
        ch->lf_taken += 100;
        guest_match(ch);
        obj = create_object(get_obj_index(ITEM_BLOOD), 0);
        obj->level = blood_level(ch->pcdata->victimize_vic_point, ch);
        char mbuf[MSL];
        if(is_virgin(ch))
        {
          if(is_faeborn(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("virgin faeborn");
          }
          else if(is_angelborn(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("virgin angelborn");
          }
          else if(is_demonborn(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("virgin demonborn");
          }
          else if(is_demigod(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("virgin demigod");
          }
          else if(is_vampire(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("virgin vampire");
          }
          else if(is_werewolf(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("virgin werewolf");
          }
          else
          {
            free_string(obj->material);
            obj->material = str_dup("virgin");
          }
        }
        else
        {
          if(is_faeborn(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("faeborn");
          }
          else if(is_angelborn(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("angelborn");
          }
          else if(is_demonborn(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("demonborn");
          }
          else if(is_demigod(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("demigod");
          }
          else if(is_vampire(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("vampire");
          }
          else if(is_werewolf(ch))
          {
            free_string(obj->material);
            obj->material = str_dup("werewolf");
          }
          else
          {
            free_string(obj->material);
            obj->material = str_dup("mundane");
          }


        }
        obj_to_char(obj, ch->pcdata->victimize_vic_point);
      }
      if (ch->lf_taken >= 910 || (ch->lf_taken >= 610 && is_dreaming(ch) && !physical_dreamer(ch))) {
        send_to_char(" (They seem weak.)", ch->pcdata->victimize_vic_point);
        if (ch->pcdata->intel >= 3000) {
          ch->pcdata->intel /= 5;
          guest_match(ch);
          if (ch->pcdata->victimize_vic_point->faction != 0) {
            FACTION_TYPE *tfac =
            clan_lookup(ch->pcdata->victimize_vic_point->faction);
            if (ch->faction != 0 && !are_allies(ch, ch->pcdata->victimize_vic_point) && generic_faction_vnum(ch->faction) ==
                generic_faction_vnum(
                  ch->pcdata->victimize_vic_point->faction)) {
              tfac->last_intel = tfac->last_intel + (3600 * 24 * 5);
            }
          }
        }
      }
      send_to_char("\n\r", ch->pcdata->victimize_vic_point);
      ch->pcdata->victimize_vic_timer = 0;
      ch->pcdata->victimize_vic_response_to = 0;
      ch->pcdata->victimize_vic_point = NULL;
      ch->pcdata->victimize_vic_select = 0;
    }
  }

  void victim_prompt(CHAR_DATA *ch) {
    int i = ch->pcdata->victimize_vic_response_to;
    int highest = 101;
    for (int x = 1; x < 20; x++) {
      int max = -1;
      int maxpoint = -1;
      for (int y = 0; y <= RESPONSE_MAX; y++) {
        if (response_value(i, y) >= highest)
        continue;
        if (response_value(i, y) > max) {
          max = response_value(i, y);
          maxpoint = y;
        }
      }
      highest = max;
      if (maxpoint != -1 && (maxpoint != RESPONSE_INTEL || ch->pcdata->intel > 1000)) {
        printf_to_char(ch, "[%02d: %s]  ", x, victimize_responses[maxpoint]);
      }
    }
    send_to_char("\n\r", ch);
  }

  void process_victim_choice(CHAR_DATA *ch, int number) {
    int cap = 1000;
    if (is_dreaming(ch) && !physical_dreamer(ch))
    cap = 700;
    if (number == ch->pcdata->victimize_last_response) {
      if (number == RESPONSE_STOIC) {
        act("$n is stoically unresponsive.", ch, NULL, NULL, TO_ROOM);
        if (is_dreaming(ch)) {
          char buf[MSL];
          sprintf(buf, "%s is stoically unresponsive.", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
        }
        act("You are stoically unresponsive.", ch, NULL, NULL, TO_CHAR);
        become_difficult(ch);
        ch->pcdata->victimize_vic_timer = 0;
        ch->pcdata->victimize_vic_response_to = 0;
        ch->pcdata->victimize_vic_point = NULL;
        act("$n becomes a difficult prisoner.", ch, NULL, NULL, TO_ROOM);
        if (is_dreaming(ch)) {
          char buf[MSL];
          sprintf(buf, "%s becomes a difficult prisoner.", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
        }
        act("You become a difficult prisoner.", ch, NULL, NULL, TO_CHAR);
        return;
      }
      if (institute_room(ch->in_room))
      ch->pcdata->ill_count += 150;
      else
      ch->pcdata->ill_count += 250;
    }
    if (number == RESPONSE_THREATENFREE || number == RESPONSE_INSULT)
    use_lifeforce(ch, 100, "Threatening/insulting captor");
    ch->pcdata->victimize_last_response = number;
    int choice = ch->pcdata->victimize_vic_response_to;
    CHAR_DATA *pers = ch->pcdata->victimize_vic_point;
    if (choice > 0 && number > 0 && pers != NULL && (ch->in_room == pers->in_room || ch->pcdata->dream_room == pers->pcdata->dream_room)) {
      if (number == RESPONSE_STOIC) {
        if (is_dreaming(ch)) {
          char buf[MSL];
          sprintf(buf, "%s is stoically unresponsive.", dream_name(ch));
          dreamscape_message(ch, ch->pcdata->dream_room, buf);
        }
        act("$n is stoically unresponsive.", ch, NULL, NULL, TO_ROOM);
        act("You are stoically unresponsive.", ch, NULL, NULL, TO_CHAR);
        ch->pcdata->victimize_vic_timer = 0;
        ch->pcdata->victimize_vic_response_to = 0;
        ch->pcdata->victimize_vic_point = NULL;
        return;
      }
      villain_mod(pers, 25, "Victimize");

      ch->pcdata->victimize_vic_timer = 900;
      ch->pcdata->victimize_vic_select = number;
      int value = response_value(ch->pcdata->victimize_vic_response_to, number);
      if (IS_IMMORTAL(pers))
      printf_to_char(pers, "Initial: %d\n", value);
      value = UMIN(value, cap - ch->lf_taken);
      if (get_tier(ch) == 1)
      value /= 3;
      if (get_tier(ch) > 2 || (get_tier(ch) == 2 && ch->skills[SKILL_MENTALDISCIPLINE] > 0))
      value = value * 13 / 10;
      if (ch->played / 3600 < 20)
      value /= 5;
      if (IS_IMMORTAL(pers))
      printf_to_char(pers, "Tier: %d\n", value);
      if (has_weakness(pers, ch))
      value = value * 2 / 3;
      else if (is_weakness(pers, ch) && ch->faction != 0)
      value = value * 3 / 2;

      if (choice == pers->pcdata->victimize_history[0])
      value /= 2;
      int nicecount = 0;
      int meancount = 0;
      for (int i = 1; i < 20; i++) {
        if (nice_victimize(pers->pcdata->victimize_history[i]))
        nicecount++;
        else if (pers->pcdata->victimize_history[i] != 0)
        meancount++;
        if (pers->pcdata->victimize_history[i] == choice) {
          value -= ((20 - i) / 4);
        }
      }
      if (number == RESPONSE_INTEL && pers->faction != 0 && ch->pcdata->intel >= 1000) {
        int iamount = UMIN(ch->pcdata->intel, 10000);
        ch->pcdata->intel -= iamount;
        if (meancount < 2)
        iamount /= 2;
        if (ch->lf_taken < 300)
        iamount /= 2;
        if (are_allies(ch, pers))
        iamount /= 20;
        FACTION_TYPE *tfac = clan_lookup(pers->faction);

        bool highval = FALSE;
        if (ch->faction != 0 && !are_allies(ch, pers) && iamount >= 1000 && (position_difference(clan_lookup(ch->faction), tfac) >= 25)) {
          highval = TRUE;
          tfac->last_intel = current_time;
          tfac->secret_days -= iamount / 100;
          ch->pcdata->last_intel = current_time;
          ch->pcdata->heroic += iamount / 100;
        }
        if (iamount >= 9000)
        ch->pcdata->pending_resources += 8000;
        else if (iamount >= 3000)
        ch->pcdata->pending_resources += 3000;

        pers->pcdata->secret_days -= iamount / 100;
        char lbuf[MSL];
        if (highval == TRUE)
        sprintf(lbuf, "%s gathering high value intelligence from %s", pers->name, ch->name);
        else
        sprintf(lbuf, "%s gathering intelligence from %s", pers->name, ch->name);
        gain_resources(iamount / 10, tfac->vnum, pers, lbuf);
        give_intel(pers, 500);
        guest_match(pers);
        guest_match(ch);
        sprintf(lbuf, "INTEL: %s, %s, %d", ch->name, pers->name, iamount);
        log_string(lbuf);
        wiznet(lbuf, NULL, NULL, WIZ_DEATHS, 0, 0);
      }
      if (IS_IMMORTAL(pers))
      printf_to_char(pers, "History: %d\n", value);

      if (nicecount > 15)
      value /= 2;
      if (meancount >= 18)
      value = value * 2 / 3;
      if (IS_IMMORTAL(pers))
      printf_to_char(pers, "Nice/Mean: %d\n", value);

      if (IS_AFFECTED(ch, AFF_ABDUCTED))
      value /= 2;

      value = UMAX(value, 5);

      for (int i = 19; i > 0; i--) {
        pers->pcdata->victimize_history[i] =
        pers->pcdata->victimize_history[i - 1];
      }
      pers->pcdata->victimize_history[0] = choice;

      if (nice_victimize(ch->pcdata->victimize_vic_response_to)) {
        if (is_super(pers) && get_tier(pers) > get_tier(ch) && !institute_room(ch->in_room))
        pers->pcdata->victimize_pending_lf_gain += value;
        else
        pers->pcdata->victimize_pending_lf_gain += value * 4 / 5;
      }
      if (is_super(pers) && get_tier(pers) > get_tier(ch) && !institute_room(ch->in_room))
      give_lifeforce(pers, value + pers->pcdata->victimize_pending_lf_gain, "victimize");
      else
      give_lifeforce(pers, (value) + pers->pcdata->victimize_pending_lf_gain, "victimize");
      pers->pcdata->monster_fed +=
      value + pers->pcdata->victimize_pending_lf_gain * 2;
      pers->pcdata->victimize_pending_lf_gain = 0;
      if (institute_room(ch->in_room))
      institute_victimize(pers, ch, value);
      printf_to_char(ch, "You elect to %s.\n\r", victimize_responses_you[number]);
      if (number == RESPONSE_PASSOUT)
      handout_lifeforce(ch);
    }
  }

  void process_victim_number(CHAR_DATA *ch, int number) {
    int i = ch->pcdata->victimize_vic_response_to;
    int highest = 101;
    for (int x = 1; x < 20; x++) {
      int max = -1;
      int maxpoint = -1;
      for (int y = 0; y <= RESPONSE_MAX; y++) {
        if (response_value(i, y) >= highest)
        continue;
        if (response_value(i, y) > max) {
          max = response_value(i, y);
          maxpoint = y;
        }
      }
      highest = max;
      if (x == number) {
        process_victim_choice(ch, maxpoint);
        if (x == 1) {
          if (institute_room(ch->in_room))
          ch->pcdata->ill_count += 40;
          else
          ch->pcdata->ill_count += 80;
        }
      }
    }
  }

  void institute_victimize(CHAR_DATA *ch, CHAR_DATA *vic, int amount) {
    amount *= 10;
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (vic != NULL && !str_cmp(ins->name, vic->name)) {
        if (safe_strlen(ins->college_committer) > 1) {
          for (vector<INSTITUTE_TYPE *>::iterator ij = InVect.begin();
          ij != InVect.end(); ++ij) {
            if (!str_cmp((*ij)->name, ins->college_committer) && (*ij)->college_rank > 0)
            (*ij)->college_staff_prestige += amount * 2;
          }
        }
      }
      if (!str_cmp(ins->name, ch->name)) {
        if (ins->asylum_staff_prestige > 0) {
          ins->asylum_staff_prestige += amount;
        }
        if (ins->college_staff_prestige > 0) {
          amount /= 2;
          ins->college_staff_prestige += amount;
        }
        if (ins->asylum_prestige > 0) {
          amount /= 2;
          ins->asylum_prestige += amount / 2;
        }
        else if (ins->college_prestige > 0) {
          amount /= 2;
          ins->college_prestige += amount / 2;
        }
      }
    }
  }

  _DOFUN(do_instituteboost) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    CHAR_DATA *victim = get_char_world_pc(arg1);
    if (victim != NULL)
    institute_victimize(victim, NULL, atoi(argument));
  }

  _DOFUN(do_collegeboost) {
    INSTITUTE_TYPE *ins;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (!str_cmp(arg1, ins->name))
      ins->college_staff_prestige += atoi(argument);
    }
  }

  _DOFUN(do_clinicboost) {
    INSTITUTE_TYPE *ins;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (!str_cmp(arg1, ins->name))
      ins->asylum_staff_prestige += atoi(argument);
    }
  }

  bool has_clinic_power_chars(CHAR_DATA *ch, CHAR_DATA *victim) {
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if (!str_cmp(ins->name, victim->name)) {
        return has_clinic_power(ch, ins, FALSE);
      }
    }
    return FALSE;
  }

  int blood_level(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    if(ch == NULL || victim == NULL)
    return 0;

    if(IS_NPC(victim))
    return 0;

    int base = 100;
    if(victim->pcdata->intel >= 2500)
    base *= 2;

    base = base * solidity(victim)/100;

    if(get_tier(victim) > 1 && is_virgin(victim))
    base *= 2;

    if(are_allies(ch, victim) && (ch->fcult != victim->fcult || (clan_lookup(ch->fcult) != NULL && clan_lookup(ch->fcult)->college == 0)))
    base /= 10;
    else if(are_allies(ch, victim))
    base /= 3;

    if (IS_AFFECTED(victim, AFF_DRAINED))
    base /= 100;
    int level = number_range(1, base);
    return level;
  }

  void ins_reset_room(ROOM_INDEX_DATA *room)
  {
    if (IS_SET(room->room_flags, ROOM_INDOORS))
    TOGGLE_BIT(room->room_flags, ROOM_INDOORS);

    if (!IS_SET(room->room_flags, ROOM_UNLIT))
    TOGGLE_BIT(room->room_flags, ROOM_UNLIT);
    free_string(room->name);
    room->name = str_dup("a grassy field");
    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      if (room->exit[door] != NULL) {
        pToRoom = room->exit[door]->u1.to_room;
        if (pToRoom != NULL) {
          room->exit[door]->wall = 0;
          if (IS_SET(room->exit[door]->exit_info, EX_CLOSED))
          REMOVE_BIT(room->exit[door]->exit_info, EX_CLOSED);
          if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR))
          REMOVE_BIT(room->exit[door]->exit_info, EX_ISDOOR);
          if (IS_SET(room->exit[door]->exit_info, EX_LOCKED))
          REMOVE_BIT(room->exit[door]->exit_info, EX_LOCKED);
          if (IS_SET(room->exit[door]->exit_info, EX_CURTAINS))
          REMOVE_BIT(room->exit[door]->exit_info, EX_CURTAINS);
          if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN))
          REMOVE_BIT(room->exit[door]->exit_info, EX_HIDDEN);
        }
      }
    }
    free_string(room->description);
    room->description = str_dup("");
    room->sector_type = SECT_PARK;
    EXTRA_DESCR_DATA *ed;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("!roomtitle", ed->keyword))
      {
        free_string(ed->description);
        ed->description = str_dup("`cOn the `WCourtyard `gL`Ga`gw`Gn `cof the `255W`254h`230i`229t`228e `178O`229a`255k`x `WInstitute`x");
      }
      else
      {
        free_string(ed->description);
        ed->description = str_dup("");
      }
    }
  }

  _DOFUN(do_insclear)
  {
    ins_reset_room(ch->in_room);
    send_to_char("Done.\n\r", ch);
  }

  int college_house_room(ROOM_INDEX_DATA *room)
  {
    if(room == NULL)
    return 0;
    if(!institute_room(room))
    return 0;

    if(room->x >= 7 && room->x <= 10 && room->y >= 57 && room->y <= 59)
    return HOUSE_CHARITY;

    if(room->x >= 2 && room->x <= 5 && room->y >= 56 && room->y <= 58)
    return HOUSE_FORBEAR;

    if(room->x >= 7 && room->x <= 10 && room->y >= 53 && room->y <= 55)
    return HOUSE_REPENT;

    if(room->x >= 2 && room->x <= 5 && room->y >= 52 && room->y <= 54)
    return HOUSE_PURITY;

    return 0;
  }

  int college_house(CHAR_DATA *ch)
  {
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if(!str_cmp(ins->name, ch->name))
      return ins->college_house;
    }
    return 0;
  }

  int dorm_room(CHAR_DATA *ch)
  {
    INSTITUTE_TYPE *ins;
    for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
    it != InVect.end(); ++it) {
      ins = (*it);
      if(!str_cmp(ins->name, ch->name))
      return ins->dorm_room;
    }
    return 0;
  }

  int find_dorm_room(int house)
  {
    int rone_occ = 0;
    int rone_inac = 0;
    int rtwo_occ = 0;
    int rtwo_inac = 0;
    int rthree_occ = 0;
    int rthree_inac = 0;
    int rfour_occ = 0;
    int rfour_inac = 0;
    int rfive_occ = 0;
    int rfive_inac = 0;
    int rsix_occ = 0;
    int rsix_inac = 0;
    int rone_point = 0;
    int rtwo_point = 0;
    int rthree_point = 0;
    int rfour_point = 0;
    int rfive_point = 0;
    int rsix_point = 0;

    if(house == HOUSE_CHARITY)
    {
      rone_point = COLLEGE_ROOM_CHAR_ONE;
      rtwo_point = COLLEGE_ROOM_CHAR_TWO;
      rthree_point = COLLEGE_ROOM_CHAR_THREE;
      rfour_point = COLLEGE_ROOM_CHAR_FOUR;
      rfive_point = COLLEGE_ROOM_CHAR_FIVE;
      rsix_point = COLLEGE_ROOM_CHAR_SIX;
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_ONE)
        {
          rone_occ++;
          if(ins->inactivity > rone_inac)
          rone_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_TWO)
        {
          rtwo_occ++;
          if(ins->inactivity > rtwo_inac)
          rtwo_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_THREE)
        {
          rthree_occ++;
          if(ins->inactivity > rthree_inac)
          rthree_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_FOUR)
        {
          rfour_occ++;
          if(ins->inactivity > rfour_inac)
          rfour_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_FIVE)
        {
          rfive_occ++;
          if(ins->inactivity > rfive_inac)
          rfive_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_SIX)
        {
          rsix_occ++;
          if(ins->inactivity > rsix_inac)
          rsix_inac = ins->inactivity;
        }
      }

    }
    else if(house == HOUSE_FORBEAR)
    {
      rone_point = COLLEGE_ROOM_FOR_ONE;
      rtwo_point = COLLEGE_ROOM_FOR_TWO;
      rthree_point = COLLEGE_ROOM_FOR_THREE;
      rfour_point = COLLEGE_ROOM_FOR_FOUR;
      rfive_point = COLLEGE_ROOM_FOR_FIVE;
      rsix_point = COLLEGE_ROOM_FOR_SIX;

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if(ins->dorm_room == COLLEGE_ROOM_FOR_ONE)
        {
          rone_occ++;
          if(ins->inactivity > rone_inac)
          rone_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_TWO)
        {
          rtwo_occ++;
          if(ins->inactivity > rtwo_inac)
          rtwo_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_THREE)
        {
          rthree_occ++;
          if(ins->inactivity > rthree_inac)
          rthree_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_FOUR)
        {
          rfour_occ++;
          if(ins->inactivity > rfour_inac)
          rfour_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_FIVE)
        {
          rfive_occ++;
          if(ins->inactivity > rfive_inac)
          rfive_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_SIX)
        {
          rsix_occ++;
          if(ins->inactivity > rsix_inac)
          rsix_inac = ins->inactivity;
        }
      }
    }
    else if(house == HOUSE_REPENT)
    {
      rone_point = COLLEGE_ROOM_REP_ONE;
      rtwo_point = COLLEGE_ROOM_REP_TWO;
      rthree_point = COLLEGE_ROOM_REP_THREE;
      rfour_point = COLLEGE_ROOM_REP_FOUR;
      rfive_point = COLLEGE_ROOM_REP_FIVE;
      rsix_point = COLLEGE_ROOM_REP_SIX;

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if(ins->dorm_room == COLLEGE_ROOM_REP_ONE)
        {
          rone_occ++;
          if(ins->inactivity > rone_inac)
          rone_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_TWO)
        {
          rtwo_occ++;
          if(ins->inactivity > rtwo_inac)
          rtwo_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_THREE)
        {
          rthree_occ++;
          if(ins->inactivity > rthree_inac)
          rthree_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_FOUR)
        {
          rfour_occ++;
          if(ins->inactivity > rfour_inac)
          rfour_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_FIVE)
        {
          rfive_occ++;
          if(ins->inactivity > rfive_inac)
          rfive_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_SIX)
        {
          rsix_occ++;
          if(ins->inactivity > rsix_inac)
          rsix_inac = ins->inactivity;
        }
      }
    }
    else if(house == HOUSE_PURITY)
    {
      rone_point = COLLEGE_ROOM_PUR_ONE;
      rtwo_point = COLLEGE_ROOM_PUR_TWO;
      rthree_point = COLLEGE_ROOM_PUR_THREE;
      rfour_point = COLLEGE_ROOM_PUR_FOUR;
      rfive_point = COLLEGE_ROOM_PUR_FIVE;
      rsix_point = COLLEGE_ROOM_PUR_SIX;

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if(ins->dorm_room == COLLEGE_ROOM_PUR_ONE)
        {
          rone_occ++;
          if(ins->inactivity > rone_inac)
          rone_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_TWO)
        {
          rtwo_occ++;
          if(ins->inactivity > rtwo_inac)
          rtwo_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_THREE)
        {
          rthree_occ++;
          if(ins->inactivity > rthree_inac)
          rthree_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_FOUR)
        {
          rfour_occ++;
          if(ins->inactivity > rfour_inac)
          rfour_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_FIVE)
        {
          rfive_occ++;
          if(ins->inactivity > rfive_inac)
          rfive_inac = ins->inactivity;
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_SIX)
        {
          rsix_occ++;
          if(ins->inactivity > rsix_inac)
          rsix_inac = ins->inactivity;
        }
      }
    }
    if(rone_occ < 1)
    return rone_point;

    if(rtwo_occ < 1)
    return rtwo_point;

    if(rthree_occ < 1)
    return rthree_point;

    if(rfour_occ < 1)
    return rfour_point;

    if(rfive_occ < 1)
    return rfive_point;

    if(rsix_occ < 1)
    return rsix_point;

    if(rone_occ < 2 && rone_inac > rtwo_inac && rone_inac > rthree_inac && rone_inac > rfour_inac && rone_inac > rfive_inac && rone_inac > rsix_inac)
    return rone_point;

    if(rtwo_occ < 2 && rtwo_inac > rone_inac && rtwo_inac > rthree_inac && rtwo_inac > rfour_inac && rtwo_inac > rfive_inac && rtwo_inac > rsix_inac)
    return rtwo_point;

    if(rthree_occ < 2 && rthree_inac > rone_inac && rthree_inac > rtwo_inac && rthree_inac > rfour_inac && rthree_inac > rfive_inac && rthree_inac > rsix_inac)
    return rthree_point;

    if(rfour_occ < 2 && rfour_inac > rone_inac && rfour_inac > rtwo_inac && rfour_inac > rthree_inac && rfour_inac > rfive_inac && rfour_inac > rsix_inac)
    return rfour_point;

    if(rfive_occ < 2 && rfive_inac > rone_inac && rfive_inac > rtwo_inac && rfive_inac > rthree_inac && rfive_inac > rfour_inac && rfive_inac > rsix_inac)
    return rfive_point;

    if(rsix_occ < 2 && rsix_inac > rone_inac && rsix_inac > rtwo_inac && rsix_inac > rthree_inac && rsix_inac > rfour_inac && rsix_inac > rfive_inac)
    return rsix_point;


    if(rone_occ < 3 && rone_inac > rtwo_inac && rone_inac > rthree_inac && rone_inac > rfour_inac && rone_inac > rfive_inac && rone_inac > rsix_inac)
    return rone_point;

    if(rtwo_occ < 3 && rtwo_inac > rone_inac && rtwo_inac > rthree_inac && rtwo_inac > rfour_inac && rtwo_inac > rfive_inac && rtwo_inac > rsix_inac)
    return rtwo_point;

    if(rthree_occ < 3 && rthree_inac > rone_inac && rthree_inac > rtwo_inac && rthree_inac > rfour_inac && rthree_inac > rfive_inac && rthree_inac > rsix_inac)
    return rthree_point;

    if(rfour_occ < 3 && rfour_inac > rone_inac && rfour_inac > rtwo_inac && rfour_inac > rthree_inac && rfour_inac > rfive_inac && rfour_inac > rsix_inac)
    return rfour_point;

    if(rfive_occ < 3 && rfive_inac > rone_inac && rfive_inac > rtwo_inac && rfive_inac > rthree_inac && rfive_inac > rfour_inac && rfive_inac > rsix_inac)
    return rfive_point;

    if(rsix_occ < 3 && rsix_inac > rone_inac && rsix_inac > rtwo_inac && rsix_inac > rthree_inac && rsix_inac > rfour_inac && rsix_inac > rfive_inac)
    return rsix_point;


    if(rone_occ < 4 && rone_inac > rtwo_inac && rone_inac > rthree_inac && rone_inac > rfour_inac && rone_inac > rfive_inac && rone_inac > rsix_inac)
    return rone_point;

    if(rtwo_occ < 4 && rtwo_inac > rone_inac && rtwo_inac > rthree_inac && rtwo_inac > rfour_inac && rtwo_inac > rfive_inac && rtwo_inac > rsix_inac)
    return rtwo_point;

    if(rthree_occ < 4 && rthree_inac > rone_inac && rthree_inac > rtwo_inac && rthree_inac > rfour_inac && rthree_inac > rfive_inac && rthree_inac > rsix_inac)
    return rthree_point;

    if(rfour_occ < 4 && rfour_inac > rone_inac && rfour_inac > rtwo_inac && rfour_inac > rthree_inac && rfour_inac > rfive_inac && rfour_inac > rsix_inac)
    return rfour_point;

    if(rfive_occ < 4 && rfive_inac > rone_inac && rfive_inac > rtwo_inac && rfive_inac > rthree_inac && rfive_inac > rfour_inac && rfive_inac > rsix_inac)
    return rfive_point;

    if(rsix_occ < 4 && rsix_inac > rone_inac && rsix_inac > rtwo_inac && rsix_inac > rthree_inac && rsix_inac > rfour_inac && rsix_inac > rfive_inac)
    return rsix_point;

    return 0;
  }

  void college_roster(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
  {
    if(room->vnum == 9597)
    {
      send_to_char("Delta Delta Delta Roster\n\r", ch);

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 500 || ins->college_prestige <= 0) {
          continue;
        }

        if(ins->dorm_room == COLLEGE_ROOM_CHAR_ONE)
        {
          printf_to_char(ch, "Room One: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_TWO)
        {
          printf_to_char(ch, "Room Two: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_THREE)
        {
          printf_to_char(ch, "Room Three: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_FOUR)
        {
          printf_to_char(ch, "Room Four: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_FIVE)
        {
          printf_to_char(ch, "Room Five: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_CHAR_SIX)
        {
          printf_to_char(ch, "Room Six: %s %s\n\r", ins->name, ins->surname);
        }
      }
    }
    if(room->vnum == 3335)
    {
      send_to_char("Alpha Gamma Omega Roster\n\r", ch);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 500 || ins->college_prestige <= 0) {
          continue;
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_ONE)
        {
          printf_to_char(ch, "Room One: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_TWO)
        {
          printf_to_char(ch, "Room Two: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_THREE)
        {
          printf_to_char(ch, "Room Three: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_FOUR)
        {
          printf_to_char(ch, "Room Four: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_FIVE)
        {
          printf_to_char(ch, "Room Five: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_FOR_SIX)
        {
          printf_to_char(ch, "Room Six: %s %s\n\r", ins->name, ins->surname);
        }
      }

    }
    if(room->vnum == 1494)
    {
      send_to_char("Phi Beta Chi Roster\n\r", ch);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 500 || ins->college_prestige <= 0) {
          continue;
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_ONE)
        {
          printf_to_char(ch, "Room One: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_TWO)
        {
          printf_to_char(ch, "Room Two: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_THREE)
        {
          printf_to_char(ch, "Room Three: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_FOUR)
        {
          printf_to_char(ch, "Room Four: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_FIVE)
        {
          printf_to_char(ch, "Room Five: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_REP_SIX)
        {
          printf_to_char(ch, "Room Six: %s %s\n\r", ins->name, ins->surname);
        }
      }
    }
    if(room->vnum == 1356)
    {
      send_to_char("Sigma Lambda Pi Roster\n\r", ch);
      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (ins->inactivity > 500 || ins->college_prestige <= 0) {
          continue;
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_ONE)
        {
          printf_to_char(ch, "Room One: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_TWO)
        {
          printf_to_char(ch, "Room Two: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_THREE)
        {
          printf_to_char(ch, "Room Three: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_FOUR)
        {
          printf_to_char(ch, "Room Four: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_FIVE)
        {
          printf_to_char(ch, "Room Five: %s %s\n\r", ins->name, ins->surname);
        }
        if(ins->dorm_room == COLLEGE_ROOM_PUR_SIX)
        {
          printf_to_char(ch, "Room Six: %s %s\n\r", ins->name, ins->surname);
        }
      }

    }
  }

  _DOFUN(do_bloodtest) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_INDEX_DATA *pObjIndex;
    char arg1[MSL];

    if(safe_strlen(argument) < 1)
    {
      send_to_char("Syntax: bloodtest (object)\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);
    if(ch->money < 1000)
    {
      send_to_char("You need at least $10 on hand to do that.\n\r", ch);
      return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;
      if(obj->pIndexData->vnum != ITEM_BLOOD)
      continue;
      if (is_name(arg1, obj->name) && can_see_obj(ch, obj)) {
        if(strcasestr(obj->material, "virgin") != NULL)
        send_to_char("This is virgin blood.\n\r", ch);
        if(strcasestr(obj->material, "faeborn") != NULL)
        send_to_char("This is faeborn blood.\n\r", ch);
        if(strcasestr(obj->material, "angelborn") != NULL)
        send_to_char("This is angelborn blood.\n\r", ch);
        if(strcasestr(obj->material, "demonborn") != NULL)
        send_to_char("This is demonborn blood.\n\r", ch);
        if(strcasestr(obj->material, "demigod") != NULL)
        send_to_char("This is demigod blood.\n\r", ch);
        if(strcasestr(obj->material, "vampire") != NULL)
        send_to_char("This is vampire blood.\n\r", ch);
        if(strcasestr(obj->material, "werewolf") != NULL)
        send_to_char("This is werewolf blood.\n\r", ch);
        ch->money -= 1000;
        return;
      }
    }
    send_to_char("You don't have any blood like that.\n\r", ch);
  }

  void bloodbag_transfer(CHAR_DATA *ch, CHAR_DATA *dest)
  {
    ch->pcdata->last_victim_bled = current_time;
    OBJ_DATA *obj;
    ch->lf_taken += 100;
    obj = create_object(get_obj_index(ITEM_BLOOD), 0);
    obj->level = blood_level(dest, ch);
    char mbuf[MSL];
    if(is_virgin(ch))
    {
      if(is_faeborn(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("virgin faeborn");
      }
      else if(is_angelborn(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("virgin angelborn");
      }
      else if(is_demonborn(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("virgin demonborn");
      }
      else if(is_demigod(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("virgin demigod");

      }
      else if(is_vampire(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("virgin vampire");
      }
      else if(is_werewolf(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("virgin werewolf");
      }
      else
      {
        free_string(obj->material);
        obj->material = str_dup("virgin");
      }
    }
    else
    {
      if(is_faeborn(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("faeborn");
      }
      else if(is_angelborn(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("angelborn");
      }
      else if(is_demonborn(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("demonborn");
      }
      else if(is_demigod(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("demigod");
      }
      else if(is_vampire(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("vampire");
      }
      else if(is_werewolf(ch))
      {
        free_string(obj->material);
        obj->material = str_dup("werewolf");
      }
      else
      {
        free_string(obj->material);
        obj->material = str_dup("mundane");
      }


    }
    obj_to_char(obj, dest);
  }


#if defined(__cplusplus)
}
#endif

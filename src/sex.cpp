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
#include "const.h"

#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif


  // replacement for some change options to separate OOC and IC command plus
  // expand scope-- needs rewritten with procedure cost table function, // universal error handling, and explicit insted of implicit procedure choice... someday - Discordance
  _DOFUN(do_procedure) {
    int cost;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    if (!str_cmp(arg1, "eye") || !str_cmp(arg1, "eyes")) {
      if (ch->played / 3600 > 5) {
        if (!in_cosmetic(ch) || ch->money < 100000) {
          send_to_char("`gYou need $1000 and to be in a cosmetic surgery clinic for that.`x\n\r", ch);
          return;
        }
        ch->money -= 100000;
        wound_char_absolute(ch, 1);
      }
      free_string(ch->pcdata->eyes);
      ch->pcdata->eyes = str_dup(argument);
      send_to_char("`gYou have your eyes altered.`x\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "breast") || !str_cmp(arg1, "breasts")) {
      if (!in_cosmetic(ch) || ch->money < 50000) {
        send_to_char("`gYou need $500 and to be in a cosmetic surgery clinic for that.`x\n\r", ch);
        return;
      }

      if (!str_cmp(argument, "Larger")) {
        ch->pcdata->bust++;
      }
      else if (!str_cmp(argument, "Smaller")) {
        ch->pcdata->bust--;
      }
      else {
        send_to_char("`gSyntax`x: `Wchange breasts `g(larger or smaller`g)`x\n\r", ch);
        return;
      }

      ch->money -= 50000;
      wound_char_absolute(ch, 1);
      send_to_char("`gYou undergo breast augmentation.`x\n\r", ch);
      ch->pcdata->boobjob++;
      return;
    }
    else if (!str_cmp(arg1, "sex") || !str_cmp(arg1, "sexchange")) {
      if (!in_cosmetic(ch) || ch->money < 1000000) {
        send_to_char("`gYou need $10000 and to be in a cosmetic surgery clinic for that.`x\n\r", ch);
        return;
      }

      if (ch->pcdata->penis > 0) {
        ch->pcdata->penis = 0;
        ch->pcdata->bust += 2;
      }
      else {
        ch->pcdata->penis = 10;
        ch->pcdata->bust = 0;
      }
      // SRS renders characters infertile
      ch->pcdata->sex_potency = 0;
      ch->pcdata->natural_fertility = 0;
      wound_char_absolute(ch, 2);
      ch->money -= 1000000;
      send_to_char("`gYou undergo a sex change.`x\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "abortion")) {
      if (!in_medical_facility(ch)) {
        send_to_char("`gYou need to be in a medical facility to have an abortion.`x\n\r", ch);
        return;
      }

      if (ch->pcdata->natural_fertility == 0) {
        send_to_char("`gYou aren't fertile.`x\n\r", ch);
        return;
      }

      if (ch->pcdata->impregnated != 0) {
        // staging cost according to how long character has been pregnant
        if (current_time < ch->pcdata->impregnated + (3600 * 24 * 7 * 9)) {
          cost = 81000;
        }
        else if (current_time < ch->pcdata->impregnated + (3600 * 24 * 7 * 19)) {
          cost = 131000;
        }
        else if (current_time < ch->pcdata->impregnated + (3600 * 24 * 7 * 24)) {
          cost = 197500;
        }
        else {
          send_to_char("`gYour pregnancy is too far along.`x\n\r", ch);
          return;
        }

        if (ch->money < cost) {
          send_to_char("`gYou can't afford an abortion at your stage.`x\n\r", ch);
          return;
        }

        wound_char_absolute(ch, 1);
        ch->pcdata->impregnated = 0;
        ch->pcdata->inseminated = 0;
        ch->pcdata->inseminated_type = 0;
        ch->money -= cost;

        send_to_char("`gYou have an abortion.`x\n\r", ch);

        return;
      }
      else {
        cost = 10000;
        ch->money -= cost;
        send_to_char("`gYou aren't pregnant.  The exam and tests cost you $100.`x\n\r",ch);
      }
    }
    else if (!str_cmp(arg1, "fertilize") || !str_cmp(arg1, "fertilization") || !str_cmp(arg1, "insemination")) {
      if (!in_medical_facility(ch)) {
        send_to_char("`gYou need to be in a medical facility to benefit from in vitro fertilization.`x\n\r", ch);
        return;
      }

      if (ch->pcdata->penis > 0) {
        send_to_char("`gMales can't get pregnant.`x\n\r", ch);
        return;
      }

      if (ch->pcdata->natural_fertility > 0) {
        send_to_char("`gYou aren't fertile.`x\n\r", ch);
        return;
      }

      if (ch->pcdata->impregnated != 0) {
        cost = 10000;
        ch->money -= cost;
        send_to_char("`gYou're already pregnant.  The exam and tests cost you $100.`x\n\r", ch);
        return;
      }

      cost = 2172000;
      if (ch->money < cost) {
        send_to_char("`gYou can't afford in vitro fertilization.`x\n\r", ch);
        return;
      }

      ch->pcdata->impregnated = current_time;
      CHAR_DATA *victim;

      if ((victim = get_char_room(ch, NULL, argument)) != NULL && victim->pcdata->penis > 0) {
        ch->pcdata->impregnated_daddy_ID = victim->id;
      }

      ch->pcdata->inseminated = 0;
      ch->pcdata->inseminated_type = 0;
      ch->pcdata->inseminated_daddy_ID = 0;
      ch->money -= cost;
      send_to_char("`gYou have in vitro fertilization performed.`x\n\r", ch);
      return;
    }
    // sterilization - Discordance
    else if (!str_cmp(arg1, "fertility") || !str_cmp(arg1, "sterilization")) {
      if (ch->pcdata->natural_fertility > 0) {
        // cost according sex
        if (ch->pcdata->penis > 0) {
          cost = 67500;
        }
        else {
          cost = 375000;
        }

        if (!in_medical_facility(ch) || ch->money < cost || !str_cmp(argument, "prices") || !str_cmp(argument, "cost") || !str_cmp(argument, "price") || !str_cmp(argument, "costs")) {
          printf_to_char(ch,"`gYou need `G$`x%d and to be in a medical facility for that.`x\n\r", cost / 100);
          return;
        }

        if (ch->pcdata->impregnated != 0) {
          send_to_char("`gYou can't undergo fertility surgery while you're pregnant.`x\n\r", ch);
          return;
        }

        // addressing invasiveness of procedure
        if (ch->pcdata->penis > 0) {
          wound_char_absolute(ch, 1);
        }
        else {
          wound_char_absolute(ch, 2);
        }

        ch->pcdata->inseminated = 0;
        ch->pcdata->inseminated_type = 0;
        ch->pcdata->natural_fertility = 0;
        ch->money -= cost;
        send_to_char("`gYou undergo sterilization treatment.`x\n\r", ch);
        return;
      }
      else {
        // cost according sex
        if (ch->pcdata->penis > 0) {
          cost = 500000;
        }
        else {
          cost = 660000;
        }

        if (!in_medical_facility(ch) || ch->money < cost || !str_cmp(argument, "prices") || !str_cmp(argument, "cost") || !str_cmp(argument, "price") || !str_cmp(argument, "costs")) {
          printf_to_char(ch,"`gYou need `G$`x%d and to be in a medical facility for that.`x\n\r", cost / 100);
          return;
        }

        // addressing invasiveness of procedure
        if (ch->pcdata->penis > 0) {
          wound_char_absolute(ch, 1);
        }
        else {
          wound_char_absolute(ch, 2);
        }

        ch->pcdata->natural_fertility = 100;
        ch->money -= cost;
        send_to_char("`gYou undergo fertility treatment.`x\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(arg1, "appearance")) {
      if (!in_cosmetic(ch) || ch->money < 500000) {
        send_to_char("`gYou need $5000 and to be in a cosmetic surgery clinic for that.`x\n\r", ch);
        return;
      }

      ch->id = get_pc_id();
      ch->money -= 500000;
      send_to_char("`gYou have your appearance significantly altered.`x\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "height")) {
      if (!in_cosmetic(ch) || ch->money < 50000) {
        send_to_char("`gYou need $500 and to be in a cosmetic surgery clinic for that.`x\n\r", ch);
        return;
      }

      if (!str_cmp(argument, "taller")) {
        ch->pcdata->height_inches++;
      }
      else if (!str_cmp(argument, "shorter")) {
        ch->pcdata->height_inches--;
      }
      else {
        send_to_char("`gSyntax`x: `Wprocedure height `g(`Wtaller or shorter`g)`x\n\r", ch);
        return;
      }

      ch->money -= 50000;
      wound_char_absolute(ch, 2);

      if (ch->pcdata->height_inches >= 12) {
        ch->pcdata->height_feet++;
        ch->pcdata->height_inches -= 12;
      }
      else if (ch->pcdata->height_inches < 0) {
        ch->pcdata->height_feet--;
        ch->pcdata->height_inches += 12;
      }

      send_to_char("`gYou have your height altered.`x\n\r", ch);
      return;
    }
    else {
      send_to_char("`gSyntax`x: `Wprocedure `g(`Wname of procedure`g)`x\n\r", ch);
      send_to_char("`gValid procedures`x: `Wabortion`g, `Wappearance`g, `Wbreast`g, `Weye`g, `Wfertility`g, `Wfertilization`g, `Wheight`g, `Wsex`x\n\r", ch);
      return;
    }
  }

  // Used in checks for demigod sex benefits
  void apply_godlysex(CHAR_DATA *victim, int length) {
    if (length == 0) {
      length = 12 * 60 * 48;
    }

    victim->pcdata->deaged += 3;
    return;
  }

  // For time limitation on sex propositions
  void apply_seekingsex(CHAR_DATA *victim, int length) {
    if (length == 0) {
      length = 24;
    }

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = length;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_SEEKINGSEX;
    affect_to_char(victim, &af);
    return;
  }

  // Temporarily prevents males from being fertile
  void apply_shootingblanks(CHAR_DATA *victim, int length) {
    if (length == 0) {
      length = 12 * 60 * 1;
    }

    if (victim->sex == SEX_MALE) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = length;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_SHOOTINGBLANKS;
      affect_to_char(victim, &af);
    }
    return;
  }

  // Checks for the name of whatever item
  bool has_item_name(CHAR_DATA *ch, char itemname[MSL]) {
    OBJ_DATA *obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (can_see_obj(ch, obj)) {
        if (strcasestr(obj->name, itemname)) {
          return TRUE;
        }
      }
    }

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (strcasestr(obj->name, itemname)) {
          return TRUE;
        }
      }
    }

    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || strcasestr(obj->name, itemname) != NULL) {
      if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || strcasestr(obj->name, itemname) != NULL) {
        return FALSE;
      }
      else {
        return TRUE;
      }
    }
    else {
      return TRUE;
    }

    return FALSE;
  }

  // Checks for contraceptives
  bool has_contraceptive_device(CHAR_DATA *ch, char risk[MSL]) {
    if (str_cmp(risk, "condom")) {
      return FALSE;
    }
    return FALSE;
  }

  // Beginning of menstrual cycle
  void aunt_flo(CHAR_DATA *ch) {
    char buf[MSL];
    if (ch->pcdata->impregnated > 0) {
      return;
    }

    sprintf(buf, "%s\nYou got your period.`x", ch->pcdata->messages);
    free_string(ch->pcdata->messages);
    ch->pcdata->messages = str_dup(buf);
    send_to_char(buf, ch);
    ch->pcdata->auntflo_called = TRUE;
    ch->pcdata->menstruation = current_time;
    return;
  }

  // Determines fertility
  bool is_fertile(CHAR_DATA *ch) {
    if (is_undead(ch))                                   {return FALSE;}
    else if (ch->race == RACE_FANTASY)                   {return FALSE;}
    else if (IS_AFFECTED(ch, AFF_FERTILITY))             {return TRUE;}
    else if (ch->pcdata->natural_fertility == 0)         {return FALSE;}
    else if (ch->pcdata->penis == 0 && get_age(ch) > 50) {return FALSE;}
    else if (IS_AFFECTED(ch, AFF_SHOOTINGBLANKS))        {return FALSE;}

    return TRUE;
  }

  // Baking the baby batter
  void bun_in_the_oven(CHAR_DATA *ch) {
    int chance = 0, target = 0;

    if (ch->pcdata->impregnated != 0) {
      sprintf(log_buf, "%s wasn't impregnated: already pregnant\n\r", ch->name);
      wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
      log_string(log_buf);
      return;
    }

    // checking fertility and if sperm is still alive
    if (is_fertile(ch) == TRUE && ch->pcdata->inseminated != 0 && ch->pcdata->sex_potency > 0) {
      int eggspan = 0;
      if (IS_AFFECTED(ch, AFF_FERTILITY)) {
        eggspan = 3600 * 24 * 6;
      }
      else {
        eggspan = 3600 * 24 * 3;
      }

      // the ovulation variable is actually the cue for code to call aunt_flo and
      // so the window for impregnation has to be entirely before this happens
      if (ch->pcdata->ovulation - eggspan <= ch->pcdata->inseminated && ch->pcdata->ovulation >= ch->pcdata->inseminated) {
        chance = number_range(1, 10000);
        if      (IS_AFFECTED(ch, AFF_FERTILITY))         {target = 0;}
        else if (ch->pcdata->habit[HABIT_HORMONES] == 5) {target = 9800;}
        else if (ch->pcdata->habit[HABIT_HORMONES] == 4) {target = 9500;}
        else if (ch->pcdata->habit[HABIT_HORMONES] == 3) {target = 9000;}
        else if (ch->pcdata->habit[HABIT_HORMONES] == 2) {target = 8000;}
        else if (ch->pcdata->habit[HABIT_HORMONES] == 1) {target = 7500;}

        if (chance >= target || ch->shape != SHAPE_HUMAN) {
          chance = number_range(1, 100);
          // chance has to be lower than remaining sperm percentage
          if (ch->pcdata->sex_potency >= chance) {
            ch->pcdata->impregnated = current_time;
            ch->pcdata->impregnated_type = ch->pcdata->inseminated_type;
            sprintf(log_buf, "%s was impregnated.\n\r", ch->name);
            wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
            log_string(log_buf);
          }
          else {
            sprintf(log_buf, "%s wasn't impregnated: potency failure\n\r", ch->name);
            wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
            log_string(log_buf);
          }
        }
        else {
          sprintf(log_buf, "%s wasn't impregnated: hormonal prevention\n\r", ch->name);
          wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
          log_string(log_buf);
        }
      }
      if (ch->pcdata->ovulation < ch->pcdata->inseminated) {
        sprintf(log_buf, "%s wasn't impregnated: outside fertilization window - %d(window) < %d(insemination)\n\r", ch->name, ch->pcdata->ovulation, ch->pcdata->inseminated);
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
      }
    }
  }

  // unintentional abortions due to physical trauma
  void miscarriage(CHAR_DATA *ch, bool forced) {
    int chance = 0, severe_threshold = 0, critical_threshold = 0;

    // if pregnant
    if (ch->pcdata->impregnated != 0) {
      chance = number_range(1, 100);

      if (forced == FALSE) {
        // Sets up thresholds based on trimester
        if (ch->pcdata->impregnated + (3600 * 24 * 7 * 29) < current_time) {
          severe_threshold = 50;
          critical_threshold = 25;
        }
        else if (ch->pcdata->impregnated + (3600 * 24 * 7 * 12) < current_time) {
          severe_threshold = 75;
          critical_threshold = 50;
        }
        else {
          severe_threshold = 100;
          critical_threshold = 75;
        }
      }
      else {
        severe_threshold = 0;
        critical_threshold = 0;
      }

      if ((ch->wounds == 2 && chance > severe_threshold) || (ch->wounds == 3 && chance > critical_threshold) || is_undead(ch)) {
        if (is_undead(ch)) {
          act("$n's body fails to sustain her pregnancy and causes her to miscarry.\n\r", ch, NULL, NULL, TO_ROOM);
          act("Your body fails to sustain your pregnancy and causes you to miscarry.\n\r", ch, NULL, NULL, TO_CHAR);
          sprintf(log_buf, "%s undead miscarried.\n\r", ch->name);
        }
        else if (ch->pcdata->impregnated + (3600 * 24 * 7 * 4) < current_time) {
          act("The wounds $n sustains causes her to miscarry.\n\r", ch, NULL, NULL, TO_ROOM);
          act("The wounds you sustain cause you to miscarry.\n\r", ch, NULL, NULL, TO_CHAR);
          sprintf(log_buf, "%s miscarried.\n\r", ch->name);
        }
        else {
          sprintf(log_buf, "%s miscarried silently.\n\r", ch->name);
        }

        ch->pcdata->impregnated = 0;
        ch->pcdata->impregnated_type = 0;
        ch->pcdata->impregnated_daddy_ID = 0;

        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
      }
    }
    return;
  }

  void apply_contractions(CHAR_DATA *victim, int length) {
    if (length == 0) {
      length = 12 * 1;
    }

    send_to_char("`RYour abdomen is wracked with wave like contractions`x.\n\r", victim);

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = length;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_CONTRACTIONS;
    affect_to_char(victim, &af);

    return;
  }

  void oven_timer(CHAR_DATA *ch) {
    int length;
    send_to_char("Your water breaks.`x\n\r", ch);
    length = number_range(1, 60); // Minutes
    length = length * 60;
    ch->pcdata->due_date = current_time + length;

    return;
  }

  // Birth
  void heres_johnny(CHAR_DATA *ch) {
    int chance = 0, threshold = 0, count = 0, skill = 0;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj = NULL;

    if (ch->pcdata->due_date < current_time) {
      chance = number_range(1, 100);
      // Professional environment
      if (in_medical_facility(ch)) {
        threshold = 1;
      }
      else {
        // Assistance outside professional environment
        CHAR_DATA *to;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end(); ++it) {
          to = *it;

          if      (to->in_room == NULL)        {continue;}
          else if (to->in_room != ch->in_room) {continue;}
          else if (to == ch)                   {continue;}
          else if (IS_NPC(to))                 {continue;}

          count = count + 1;

          if(get_skill(to, SKILL_MEDICINE) > skill) {
            skill = get_skill(to, SKILL_MEDICINE);
          }
        }

        if (count > 0) {
          // Qualified assistance
          if      (skill == 5)        {threshold = 1;}
          else if (skill == 4)        {threshold = 7;}
          else if (skill == 3)        {threshold = 12;}
          else if (has_attendant(ch)) {threshold = 12;}
          else if (skill == 2)        {threshold = 16;}
          else if (skill == 1)        {threshold = 19;}
          // unqualified assistance
          else                        {threshold = 20;}
        }
        else /* all alone */          {threshold = 30;}
      }

      // The mother dies
      if (chance <= threshold) {
        act("$n is critically wounded during childbirth.\n\r", ch, NULL, NULL, TO_ROOM);
        act("You are critically wounded during childbirth.\n\r", ch, NULL, NULL, TO_CHAR);
        sprintf(log_buf, "%s was critically wounded during childbirth.\n\r", ch->name);
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
        critplayer(ch, ch);
        wound_char_absolute(ch, 3);
      }
      // Successful birth
      else {
        chance = number_range(1, 100);
        int baby_sex = 0;
        // spawning baby
        pObjIndex = get_obj_index(73);
        obj = create_object(pObjIndex, 0);
        setdescs(obj);

        if (chance >= 50) {
          baby_sex = SEX_FEMALE;
          obj->value[0] = current_time;
          obj->value[1] = SEX_FEMALE;
          free_string(obj->name);
          obj->name = str_dup("newborn baby girl");

          free_string(obj->short_descr);
          obj->short_descr = str_dup("newborn baby girl");

          free_string(obj->description);
          obj->description = str_dup("A newborn baby girl");
        }
        else {
          baby_sex = SEX_MALE;
          obj->value[0] = current_time;
          obj->value[1] = SEX_MALE;
          free_string(obj->name);
          obj->name = str_dup("newborn baby boy");

          free_string(obj->short_descr);
          obj->short_descr = str_dup("newborn baby boy");

          free_string(obj->description);
          obj->description = str_dup("A newborn baby boy");
        }

        if (baby_sex == SEX_MALE) {
          act("$n gives birth to a new baby boy.\n\r", ch, NULL, NULL, TO_ROOM);
          act("You give birth to a new baby boy.\n\r", ch, NULL, NULL, TO_CHAR);
        }
        else {
          act("$n gives birth to a new baby girl.\n\r", ch, NULL, NULL, TO_ROOM);
          act("You give birth to a new baby girl.\n\r", ch, NULL, NULL, TO_CHAR);
        }

        obj_to_char(obj, ch);

        sprintf(log_buf, "%s gave birth to a new baby %s.\n\r", ch->name, (baby_sex == SEX_MALE) ? "boy" : "girl");
        wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(log_buf);
      }
      ch->pcdata->due_date = 0;
      ch->pcdata->impregnated = 0;
    }
    else {
      send_to_char("`RYour abdomen is wracked with wave like contractions`x.\n\r",ch);
    }
    return;
  }

  int get_pregnancy_minlength(CHAR_DATA *ch) {
    if      (ch->pcdata->impregnated_type == SEXTYPE_NORMAL)      {return 37;}
    else if (ch->pcdata->impregnated_type == SEXTYPE_PARASITE)    {return 5;}
    else if (ch->pcdata->impregnated_type == SEXTYPE_ACCELERATED) {return 5;}
    else if (ch->pcdata->impregnated_type == SEXTYPE_SHORT)       {return 1;}

    return 37;
  }

  // Keeps track of menstruation, ovulation, and checks for pregnancy -
  // Discordance also sperm and fertility
  void sex_upkeep(CHAR_DATA *ch) {
    int chance;

    if (IS_FLAG(ch->act, PLR_DEAD)) {return;}

    if (get_skill(ch, SKILL_VIRGIN) == 2) {
      if (ch->pcdata->virginity_lost > 0) {
        if (ch->pcdata->virginity_lost + (3600 * 24 * 21) < current_time) {
          ch->pcdata->virginity_lost = 0;
          for (int i = 0; i < 3; i++) {ch->pcdata->last_sexed[i] = str_dup("");}
          for (int i = 0; i < 3; i++) {ch->pcdata->last_sexed_ID[i] = 0;}

          char buf[MSL];
          sprintf(buf, "%s\n`WYou feel pure.`x", ch->pcdata->messages);
          free_string(ch->pcdata->messages);
          ch->pcdata->messages = str_dup(buf);
          send_to_char(buf, ch);
        }
      }

      if (ch->pcdata->penis == 0) {
        if (ch->pcdata->hymen_lost > 0) {
          if (ch->pcdata->hymen_lost + (3600 * 24 * 21) < current_time) {
            ch->pcdata->hymen_lost = 0;
            char buf[MSL];
            sprintf(buf, "%s\n`WYou feel whole.`x", ch->pcdata->messages);
            free_string(ch->pcdata->messages);
            ch->pcdata->messages = str_dup(buf);
            send_to_char(buf, ch);
          }
        }
      }
    }

    // hormones
    // testosterone for women
    if (ch->pcdata->penis == 0) {
      if (ch->pcdata->testosterone > 0) { // testosterone for women
        int months;
        months = floor(ch->pcdata->testosterone / 30);

        if (ch->pcdata->natural_fertility > (100 - (months * 5))) {
          ch->pcdata->natural_fertility =
          UMAX(0, ch->pcdata->natural_fertility - (months * 5));
        }
      }
    }
    else {
      // estrogen for men
      if (ch->pcdata->estrogen > 0) {
        ch->pcdata->estrogen += ch->pcdata->habit[HABIT_HORMONES] + 1;

        int months;
        months = floor(ch->pcdata->estrogen / 150);

        if (ch->pcdata->natural_fertility > (100 - (months * 5))) {
          ch->pcdata->natural_fertility =
          UMAX(0, ch->pcdata->natural_fertility - (months * 5));
        }
      }
    }
    if (ch->pcdata->estrogen > 6000) {
      ch->pcdata->estrogen = 6000;
    }
    if (ch->pcdata->testosterone > 6000) {
      ch->pcdata->testosterone = 6000;
    }
    // storing historical high
    if (ch->pcdata->estrogen > ch->pcdata->femaledevelopment) {
      ch->pcdata->femaledevelopment = ch->pcdata->estrogen;
    }
    if (ch->pcdata->testosterone > ch->pcdata->maledevelopment) {
      ch->pcdata->maledevelopment = ch->pcdata->testosterone;
    }

    // tacking on sperm count revitalization
    if (ch->pcdata->penis > 0) {
      if (!is_fertile(ch)) {
        if (ch->pcdata->sex_potency > 0) {ch->pcdata->sex_potency -= 20;}
        else                             {ch->pcdata->sex_potency  =  0;}
      }
      else {
        if (ch->pcdata->sex_potency < 100) {
          ch->pcdata->sex_potency = UMIN(100, ch->pcdata->sex_potency + 20);
        }
        else if (ch->pcdata->sex_potency > 100) {
          ch->pcdata->sex_potency = UMAX(100, ch->pcdata->sex_potency - 10);
        }
      }
    }
    // Only for women
    else if (ch->pcdata->penis == 0) {
      // Only after puberty and before menopause.  Leaving puberty and menopause
      // static for now.
      if (is_fertile(ch) == TRUE) {
        // reset for characters who haven't logged in for over 45
        // days
        if (ch->pcdata->menstruation == 0 || (ch->pcdata->menstruation + (3600 * 24 * 45) < current_time)) {
          aunt_flo(ch);
        }

        // Resets menses notification after ovulation date in time for next
        // ovulation
        if (current_time > ch->pcdata->ovulation) {
          ch->pcdata->auntflo_called = FALSE;
        }

        // Sends menses notification if character hasn't ovulated, but has passed
        // menstruation date.  Only once per cycle.
        if (current_time >= ch->pcdata->menstruation && current_time < ch->pcdata->ovulation && ch->pcdata->auntflo_called == FALSE) {
          aunt_flo(ch);
        }

        // Only if ovulation has not occurred since the last menstrual period(LMP)
        if (ch->pcdata->ovulation < ch->pcdata->menstruation) {
          // Distribution table below
          /*
          2
            8		 2.10%	  2.4		    2.4
            9				      4.5		    6.9
            10	13.60%	  4.5		    11.4
          1
            11				     4.5		  15.9
            12	34.10%	  13.64		  29.54
            13				    13.64		  43.18
          0
            14				    13.64		  56.82
            15            13.64		  70.46
            16  34.10%	  13.64		  84.1
          1
            17				    4.5		    88.6
            18	13.60%		4.5		    93.1
            19				    4.5		    97.6
          2	
            20	 2.10%		2.4		    100
          */
          // Only if at least 8 days have passed since LMP
          if (ch->pcdata->menstruation + (3600 * 24 * 8) < current_time) {
            chance = number_range(1, 10000);

            // Percentage change for ovulation to occur.  Average ovulation is day
            // 14 of cycle.  Should fall between day 8 and 20.
            if (chance <= 240) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 8);
            }
            else if (chance <= 690) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 9);
            }
            else if (chance <= 1140) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 10);
            }
            else if (chance <= 1590) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 11);
            }
            else if (chance <= 2954) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 12);
            }
            else if (chance <= 4318) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 13);
            }
            else if (chance <= 5682) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 14);
            }
            else if (chance <= 7046) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 15);
            }
            else if (chance <= 8410) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 16);
            }
            else if (chance <= 8860) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 17);
            }
            else if (chance <= 9310) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 18);
            }
            else if (chance <= 9760) {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 19);
            }
            else {
              ch->pcdata->ovulation =
              (ch->pcdata->menstruation) + (3600 * 24 * 20);
            }
          }
        }
        // Ovulation has occurred; checks to begin next menstrual cycle
        else {
          // Distribution table below
          /*
            18				    3.4		3.4
            19				    3.4		6.8
            20	 13.60%		3.4		10.2
          1
            21				    3.4		13.6
            22   34.10%		5.1		18.7
            23				    5.1		23.8
            24				    5.1		28.9
            25				    5.1		34
            26				    5.1		39.1
            27				    5.1		44.2
            28				    5.1		49.3
          0
            29				    5.1		54.4
            30				    5.1		59.5
            31				    5.1		64.6
            32				    5.1		69.7
            33				    5.1		74.8
            34				    5.1		79.9
            35	 34.10%		5.1		85
          1
            36				    1.6		86.6
            37	 13.60%		1.6		88.2
            38				    1.6		89.8
            39				    1.6		91.4
            40				    1.6		93
            41				    1.6		94.6
            42				    1.6		96.2
            43				    1.6		97.8
          2
            44		2.10%		1.1		98.9
            45				    1.1		100
          */
          // Average cycle is 28 days.  Variation should be 18-45.
          if (ch->pcdata->menstruation + (3600 * 24 * 18) < current_time) {
            // Don't want menstruation to begin before ovulation
            while (ch->pcdata->ovulation >= ch->pcdata->menstruation) {
              chance = number_range(1, 10000);

              if (chance <= 340) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 18);
              }
              else if (chance <= 680) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 19);
              }
              else if (chance <= 1020) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 20);
              }
              else if (chance <= 1360) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 21);
              }
              else if (chance <= 1870) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 22);
              }
              else if (chance <= 2380) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 23);
              }
              else if (chance <= 2890) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 24);
              }
              else if (chance <= 3400) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 25);
              }
              else if (chance <= 3910) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 26);
              }
              else if (chance <= 4420) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 27);
              }
              else if (chance <= 4930) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 28);
              }
              else if (chance <= 5440) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 29);
              }
              else if (chance <= 5950) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 30);
              }
              else if (chance <= 6460) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 31);
              }
              else if (chance <= 6970) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 32);
              }
              else if (chance <= 7480) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 33);
              }
              else if (chance <= 7990) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 34);
              }
              else if (chance <= 8500) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 35);
              }
              else if (chance <= 8660) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 36);
              }
              else if (chance <= 8820) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 37);
              }
              else if (chance <= 8980) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 38);
              }
              else if (chance <= 9140) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 39);
              }
              else if (chance <= 9300) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 40);
              }
              else if (chance <= 9460) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 41);
              }
              else if (chance <= 9620) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 42);
              }
              else if (chance <= 9780) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 43);
              }
              else if (chance <= 9890) {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 44);
              }
              else {
                ch->pcdata->menstruation =
                (ch->pcdata->menstruation) + (3600 * 24 * 45);
              }
            }
          }
        }
      }
      // Only if inseminated. Killing off sperm. 3-5 days.
      if (ch->pcdata->inseminated != 0) {
        // Safe for at least 3 days
        if (ch->pcdata->inseminated + (3600 * 24 * 3) < current_time) {
          ch->pcdata->sex_potency -= 20;

          if (ch->pcdata->sex_potency <= 0) {
            ch->pcdata->sex_potency = 0;
            ch->pcdata->inseminated = 0;
            ch->pcdata->inseminated_type = 0;
          }
        }
        bun_in_the_oven(ch);
      }
      else {
        ch->pcdata->sex_potency = 0;
      }

      if (!IS_AFFECTED(ch, AFF_CONTRACTIONS)) {
        // Checking due date
        if (ch->pcdata->impregnated != 0) {
          if (is_undead(ch)) {miscarriage(ch, FALSE);}

          int weeks = get_pregnancy_minlength(ch);

          if (ch->pcdata->impregnated + (3600 * 24 * 7 * weeks) < current_time) {
            chance = number_range(1, 100);

            if (ch->pcdata->impregnated + (3600 * 24 * 7 * (weeks + 3)) < current_time) {
              oven_timer(ch);
            }
            else if (ch->pcdata->impregnated + (3600 * 24 * 7 * (weeks + 2)) < current_time) {
              if (chance <= 30) {oven_timer(ch);}
            }
            else if (ch->pcdata->impregnated + (3600 * 24 * 7 * (weeks + 1)) < current_time) {
              if (chance <= 20) {oven_timer(ch);}
            }
            else if (chance <= 10) {oven_timer(ch);}
          }
        }
      }
    }
  }

  _DOFUN(do_sexupkeep) {
    char target[MSL];
    argument = one_argument_nouncap(argument, target);

    CHAR_DATA *victim;

    if (target[0] == '\0') {
      return;
    }
    else if ((victim = get_char_room(ch, NULL, target)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    sex_upkeep(victim);
    return;
  }

  // Sets up method for checking viability of sperm over time (3-5 days after ejaculation)
  void baby_batter(CHAR_DATA *ch, CHAR_DATA *victim, bool impotent) {

    if (impotent == TRUE) {
      return;
    }

    // Decides who the male is in hetero pairing and compares sperm to sperm
    // already transfered
    if (ch->pcdata->penis > 0 && victim->pcdata->penis == 0) {
      if (ch->pcdata->sex_potency >= victim->pcdata->sex_potency) {
        victim->pcdata->inseminated = current_time;

        //parasite pregnancy not migrated yet
        victim->pcdata->inseminated_type = SEXTYPE_NORMAL;

        if (IS_AFFECTED(ch, AFF_FERTILITY)) {
          victim->pcdata->sex_potency = 200;
        }
        else if (victim->pcdata->sleeping > 0) {
          victim->pcdata->sex_potency = ch->pcdata->sex_potency / 2;
        }
        else {
          victim->pcdata->sex_potency = ch->pcdata->sex_potency;
        }

        victim->pcdata->inseminated_daddy_ID = ch->id;
      }
    }
    else if (ch->pcdata->penis == 0 && victim->pcdata->penis > 0) {
      if (ch->pcdata->sex_potency <= victim->pcdata->sex_potency) {
        ch->pcdata->inseminated = current_time;

        //parasite pregnancy not migrated yet
        ch->pcdata->inseminated_type = SEXTYPE_NORMAL;

        if (IS_AFFECTED(ch, AFF_FERTILITY)) {
          ch->pcdata->sex_potency = 200;
        }
        else if (ch->pcdata->sleeping > 0) {
          ch->pcdata->sex_potency = victim->pcdata->sex_potency / 2;
        }
        else {
          ch->pcdata->sex_potency = victim->pcdata->sex_potency;
        }

        ch->pcdata->inseminated_daddy_ID = victim->id;
      }
    }
  }

  // Defaults sex variables
  void sex_cleanup(CHAR_DATA *ch) {
    ch->privaterpexp = 0;
    ch->privatekarma = 0;
    ch->pcdata->sexing = NULL;
    free_string(ch->pcdata->sex_risk);
    free_string(ch->pcdata->sex_type);
    ch->pcdata->sex_risk = str_dup("");
    ch->pcdata->sex_type = str_dup("");
    if (!IS_FLAG(ch->act, PLR_AFTERGLOW)) {
      SET_FLAG(ch->act, PLR_AFTERGLOW);
      if (is_dreaming(ch)) {ch->pcdata->dream_sex++;}
      else                 {ch->pcdata->dream_sex--;}
    }

    return;
  }

  // Handles valid type arguments and interpretations
  char *process_type_arguments(CHAR_DATA *ch, CHAR_DATA *top, int bottom_penis, char type[MSL]) {
    // Universal
    if      (!str_cmp(type, "voyeur"))   {return "voyeur";}
    else if (!str_cmp(type, "watch"))    {return "voyeur";}
    else if (!str_cmp(type, "watcher"))  {return "voyeur";}
    else if (!str_cmp(type, "watching")) {return "voyeur";}
    else if (!str_cmp(type, "spy"))      {return "voyeur";}
    else if (!str_cmp(type, "spying"))   {return "voyeur";}

    // Male options
    if (top->pcdata->penis > 0) {
      // Male heterosexual sex
      if (bottom_penis == 0) {
        if      (!str_cmp(type, "anal"))           {return "noncoital";}
        else if (!str_cmp(type, "analoral"))       {return "outercourse";}
        else if (!str_cmp(type, "anal-oral"))      {return "outercourse";}
        else if (!str_cmp(type, "anilingus"))      {return "outercourse";}
        else if (!str_cmp(type, "ass"))            {return "noncoital";}
        else if (!str_cmp(type, "axillary"))       {return "outercourse";}
        else if (!str_cmp(type, "butt"))           {return "noncoital";}
        else if (!str_cmp(type, "butterfly"))      {return "coital";}
        else if (!str_cmp(type, "buttlicking"))    {return "outercourse";}
        else if (!str_cmp(type, "circlejerk"))     {return "outercourse";}
        else if (!str_cmp(type, "coital"))         {return "coital";}
        else if (!str_cmp(type, "cowgirl"))        {return "coital";}
        else if (!str_cmp(type, "cunnilingus"))    {return "False";}
        else if (!str_cmp(type, "daisychain"))     {return "noncoital";}
        else if (!str_cmp(type, "deepthroat"))     {return "noncoital";}
        else if (!str_cmp(type, "dildo"))          {return "noncoital";}
        else if (!str_cmp(type, "docking"))        {return "false";}
        else if (!str_cmp(type, "dong"))           {return "coital";}
        else if (!str_cmp(type, "bagpiping"))      {return "outercourse";}
        else if (!str_cmp(type, "bent"))           {return "coital";}
        else if (!str_cmp(type, "bentover"))       {return "coital";}
        else if (!str_cmp(type, "blow"))           {return "noncoital";}
        else if (!str_cmp(type, "blowing"))        {return "noncoital";}
        else if (!str_cmp(type, "blowjob"))        {return "noncoital";}
        else if (!str_cmp(type, "doggy"))          {return "coital";}
        else if (!str_cmp(type, "doggystyle"))     {return "coital";}
        else if (!str_cmp(type, "fellatio"))       {return "noncoital";}
        else if (!str_cmp(type, "fingering"))      {return "noncoital";}
        else if (!str_cmp(type, "footjob"))        {return "outercourse";}
        else if (!str_cmp(type, "forking"))        {return "coital";}
        else if (!str_cmp(type, "frog"))           {return "coital";}
        else if (!str_cmp(type, "frot"))           {return "False";}
        else if (!str_cmp(type, "frottage"))       {return "outercourse";}
        else if (!str_cmp(type, "handjob"))        {return "outercourse";}
        else if (!str_cmp(type, "intercrural"))    {return "outercourse";}
        else if (!str_cmp(type, "lapdance"))       {return "outercourse";}
        else if (!str_cmp(type, "mammary"))        {return "outercourse";}
        else if (!str_cmp(type, "mission"))        {return "coital";}
        else if (!str_cmp(type, "misionary"))      {return "coital";}
        // intentional misspelling
        else if (!str_cmp(type, "missonary"))      {return "coital";}
        // intentional misspelling
        else if (!str_cmp(type, "missionary"))     {return "coital";}
        else if (!str_cmp(type, "noncoital"))      {return "noncoital";}
        else if (!str_cmp(type, "nonpenetrative")) {return "outercourse";}
        else if (!str_cmp(type, "oral"))           {return "noncoital";}
        else if (!str_cmp(type, "outercourse"))    {return "outercourse";}
        else if (!str_cmp(type, "penetrative"))    {return "coital";}
        else if (!str_cmp(type, "penis"))          {return "coital";}
        else if (!str_cmp(type, "pet"))            {return "outercourse";}
        else if (!str_cmp(type, "petting"))        {return "outercourse";}
        else if (!str_cmp(type, "pitjob"))         {return "outercourse";}
        else if (!str_cmp(type, "reversecowgirl")) {return "coital";}
        else if (!str_cmp(type, "rimjob"))         {return "outercourse";}
        else if (!str_cmp(type, "rimming"))        {return "outercourse";}
        else if (!str_cmp(type, "rubbing"))        {return "outercourse";}
        else if (!str_cmp(type, "scissor"))        {return "coital";}
        else if (!str_cmp(type, "scissoring"))     {return "coital";}
        else if (!str_cmp(type, "shocker"))        {return "noncoital";}
        else if (!str_cmp(type, "69"))             {return "noncoital";}
        else if (!str_cmp(type, "sixtynine"))      {return "noncoital";}
        else if (!str_cmp(type, "sixty-nine"))     {return "noncoital";}
        else if (!str_cmp(type, "spoon"))          {return "outercourse";}
        else if (!str_cmp(type, "spoons"))         {return "outercourse";}
        else if (!str_cmp(type, "spooning"))       {return "outercourse";}
        else if (!str_cmp(type, "starfish"))       {return "noncoital";}
        else if (!str_cmp(type, "strapon"))        {return "noncoital";}
        else if (!str_cmp(type, "suck"))           {return "noncoital";}
        else if (!str_cmp(type, "sucking"))        {return "noncoital";}
        else if (!str_cmp(type, "suckoff"))        {return "noncoital";}
        else if (!str_cmp(type, "sumata"))         {return "outercourse";}
        else if (!str_cmp(type, "titjob"))         {return "outercourse";}
        else if (!str_cmp(type, "toy"))            {return "noncoital";}
        else if (!str_cmp(type, "toys"))           {return "noncoital";}
        else if (!str_cmp(type, "trib"))           {return "False";}
        else if (!str_cmp(type, "tribadism"))      {return "False";}
        else if (!str_cmp(type, "vagina"))         {return "coital";}
        else if (!str_cmp(type, "vaginal"))        {return "coital";}
        else if (!str_cmp(type, "vibrator"))       {return "outercourse";}
      }
      // Male homosexual sex
      else if (bottom_penis > 0) {
        if      (!str_cmp(type, "anal"))           {return "noncoital";}
        else if (!str_cmp(type, "analoral"))       {return "outercourse";}
        else if (!str_cmp(type, "anal-oral"))      {return "outercourse";}
        else if (!str_cmp(type, "anilingus"))      {return "outercourse";}
        else if (!str_cmp(type, "ass"))            {return "noncoital";}
        else if (!str_cmp(type, "axillary"))       {return "outercourse";}
        else if (!str_cmp(type, "butt"))           {return "noncoital";}
        else if (!str_cmp(type, "butterfly"))      {return "noncoital";}
        else if (!str_cmp(type, "buttlicking"))    {return "outercourse";}
        else if (!str_cmp(type, "circlejerk"))     {return "outercourse";}
        else if (!str_cmp(type, "coital"))         {return "noncoital";}
        else if (!str_cmp(type, "cowgirl"))        {return "noncoital";}
        else if (!str_cmp(type, "cunnilingus"))    {return "False";}
        else if (!str_cmp(type, "daisychain"))     {return "noncoital";}
        else if (!str_cmp(type, "deepthroat"))     {return "noncoital";}
        else if (!str_cmp(type, "dildo"))          {return "noncoital";}
        else if (!str_cmp(type, "dong"))           {return "noncoital";}
        else if (!str_cmp(type, "bagpiping"))      {return "outercourse";}
        else if (!str_cmp(type, "bent"))           {return "noncoital";}
        else if (!str_cmp(type, "bentover"))       {return "noncoital";}
        else if (!str_cmp(type, "blow"))           {return "noncoital";}
        else if (!str_cmp(type, "blowing"))        {return "noncoital";}
        else if (!str_cmp(type, "blowjob"))        {return "noncoital";}
        else if (!str_cmp(type, "docking"))        {return "outercourse";}
        else if (!str_cmp(type, "doggy"))          {return "noncoital";}
        else if (!str_cmp(type, "doggystyle"))     {return "noncoital";}
        else if (!str_cmp(type, "fellatio"))       {return "noncoital";}
        else if (!str_cmp(type, "fingering"))      {return "noncoital";}
        else if (!str_cmp(type, "footjob"))        {return "outercourse";}
        else if (!str_cmp(type, "forking"))        {return "noncoital";}
        else if (!str_cmp(type, "frog"))           {return "noncoital";}
        else if (!str_cmp(type, "frot"))           {return "outercourse";}
        else if (!str_cmp(type, "frottage"))       {return "outercourse";}
        else if (!str_cmp(type, "handjob"))        {return "outercourse";}
        else if (!str_cmp(type, "intercrural"))    {return "outercourse";}
        else if (!str_cmp(type, "lapdance"))       {return "outercourse";}
        else if (!str_cmp(type, "mammary"))        {return "outercourse";}
        else if (!str_cmp(type, "mission"))        {return "noncoital";}
        else if (!str_cmp(type, "misionary"))      {return "noncoital";}
        // intentional misspelling
        else if (!str_cmp(type, "missonary"))      {return "noncoital";}
        // intentional misspelling
        else if (!str_cmp(type, "missionary"))     {return "noncoital";}
        else if (!str_cmp(type, "noncoital"))      {return "noncoital";}
        else if (!str_cmp(type, "nonpenetrative")) {return "outercourse";}
        else if (!str_cmp(type, "oral"))           {return "noncoital";}
        else if (!str_cmp(type, "outercourse"))    {return "outercourse";}
        else if (!str_cmp(type, "penetrative"))    {return "noncoital";}
        else if (!str_cmp(type, "penis"))          {return "noncoital";}
        else if (!str_cmp(type, "pet"))            {return "outercourse";}
        else if (!str_cmp(type, "petting"))        {return "outercourse";}
        else if (!str_cmp(type, "pitjob"))         {return "outercourse";}
        else if (!str_cmp(type, "reversecowgirl")) {return "noncoital";}
        else if (!str_cmp(type, "rimjob"))         {return "outercourse";}
        else if (!str_cmp(type, "rimming"))        {return "outercourse";}
        else if (!str_cmp(type, "rubbing"))        {return "outercourse";}
        else if (!str_cmp(type, "scissor"))        {return "outercourse";}
        else if (!str_cmp(type, "scissoring"))     {return "outercourse";}
        else if (!str_cmp(type, "shocker"))        {return "False";}
        else if (!str_cmp(type, "69"))             {return "noncoital";}
        else if (!str_cmp(type, "sixtynine"))      {return "noncoital";}
        else if (!str_cmp(type, "sixty-nine"))     {return "noncoital";}
        else if (!str_cmp(type, "spoon"))          {return "outercourse";}
        else if (!str_cmp(type, "spoons"))         {return "outercourse";}
        else if (!str_cmp(type, "spooning"))       {return "outercourse";}
        else if (!str_cmp(type, "starfish"))       {return "noncoital";}
        else if (!str_cmp(type, "strapon"))        {return "noncoital";}
        else if (!str_cmp(type, "suck"))           {return "noncoital";}
        else if (!str_cmp(type, "sucking"))        {return "noncoital";}
        else if (!str_cmp(type, "suckoff"))        {return "noncoital";}
        else if (!str_cmp(type, "sumata"))         {return "False";}
        else if (!str_cmp(type, "titjob"))         {return "outercourse";}
        else if (!str_cmp(type, "toy"))            {return "noncoital";}
        else if (!str_cmp(type, "toys"))           {return "noncoital";}
        else if (!str_cmp(type, "trib"))           {return "False";}
        else if (!str_cmp(type, "tribadism"))      {return "False";}
        else if (!str_cmp(type, "vagina"))         {return "False";}
        else if (!str_cmp(type, "vaginal"))        {return "False";}
        else if (!str_cmp(type, "vibrator"))       {return "outercourse";}
      }
    }
    // Female options
    else if (top->pcdata->penis == 0) {
      // Female heterosexual sex
      if (bottom_penis > 0) {
        if      (!str_cmp(type, "anal"))           {return "noncoital";}
        else if (!str_cmp(type, "analoral"))       {return "outercourse";}
        else if (!str_cmp(type, "anal-oral"))      {return "outercourse";}
        else if (!str_cmp(type, "anilingus"))      {return "outercourse";}
        else if (!str_cmp(type, "ass"))            {return "noncoital";}
        else if (!str_cmp(type, "axillary"))       {return "outercourse";}
        else if (!str_cmp(type, "butt"))           {return "noncoital";}
        else if (!str_cmp(type, "butterfly"))      {return "coital";}
        else if (!str_cmp(type, "buttlicking"))    {return "outercourse";}
        else if (!str_cmp(type, "circlejerk"))     {return "outercourse";}
        else if (!str_cmp(type, "coital"))         {return "coital";}
        else if (!str_cmp(type, "cowgirl"))        {return "coital";}
        else if (!str_cmp(type, "cunnilingus"))    {return "False";}
        else if (!str_cmp(type, "daisychain"))     {return "noncoital";}
        else if (!str_cmp(type, "deepthroat"))     {return "noncoital";}
        else if (!str_cmp(type, "dildo"))          {return "noncoital";}
        else if (!str_cmp(type, "docking"))        {return "false";}
        else if (!str_cmp(type, "dong"))           {return "coital";}
        else if (!str_cmp(type, "bagpiping"))      {return "outercourse";}
        else if (!str_cmp(type, "bent"))           {return "coital";}
        else if (!str_cmp(type, "bentover"))       {return "coital";}
        else if (!str_cmp(type, "blow"))           {return "noncoital";}
        else if (!str_cmp(type, "blowing"))        {return "noncoital";}
        else if (!str_cmp(type, "blowjob"))        {return "noncoital";}
        else if (!str_cmp(type, "doggy"))          {return "coital";}
        else if (!str_cmp(type, "doggystyle"))     {return "coital";}
        else if (!str_cmp(type, "fellatio"))       {return "noncoital";}
        else if (!str_cmp(type, "fingering"))      {return "noncoital";}
        else if (!str_cmp(type, "footjob"))        {return "outercourse";}
        else if (!str_cmp(type, "forking"))        {return "coital";}
        else if (!str_cmp(type, "frog"))           {return "coital";}
        else if (!str_cmp(type, "frot"))           {return "False";}
        else if (!str_cmp(type, "frottage"))       {return "outercourse";}
        else if (!str_cmp(type, "handjob"))        {return "outercourse";}
        else if (!str_cmp(type, "intercrural"))    {return "outercourse";}
        else if (!str_cmp(type, "lapdance"))       {return "outercourse";}
        else if (!str_cmp(type, "mammary"))        {return "outercourse";}
        else if (!str_cmp(type, "mission"))        {return "coital";}
        else if (!str_cmp(type, "misionary"))      {return "coital";}
        // intentional misspelling
        else if (!str_cmp(type, "missonary"))      {return "coital";}
        // intentional misspelling
        else if (!str_cmp(type, "missionary"))     {return "coital";}
        else if (!str_cmp(type, "noncoital"))      {return "noncoital";}
        else if (!str_cmp(type, "nonpenetrative")) {return "outercourse";}
        else if (!str_cmp(type, "oral"))           {return "noncoital";}
        else if (!str_cmp(type, "outercourse"))    {return "outercourse";}
        else if (!str_cmp(type, "penetrative"))    {return "coital";}
        else if (!str_cmp(type, "penis"))          {return "coital";}
        else if (!str_cmp(type, "pet"))            {return "outercourse";}
        else if (!str_cmp(type, "petting"))        {return "outercourse";}
        else if (!str_cmp(type, "pitjob"))         {return "outercourse";}
        else if (!str_cmp(type, "reversecowgirl")) {return "coital";}
        else if (!str_cmp(type, "rimjob"))         {return "outercourse";}
        else if (!str_cmp(type, "rimming"))        {return "outercourse";}
        else if (!str_cmp(type, "rubbing"))        {return "outercourse";}
        else if (!str_cmp(type, "scissor"))        {return "coital";}
        else if (!str_cmp(type, "scissoring"))     {return "coital";}
        else if (!str_cmp(type, "shocker"))        {return "noncoital";}
        else if (!str_cmp(type, "69"))             {return "noncoital";}
        else if (!str_cmp(type, "sixtynine"))      {return "noncoital";}
        else if (!str_cmp(type, "sixty-nine"))     {return "noncoital";}
        else if (!str_cmp(type, "spoon"))          {return "outercourse";}
        else if (!str_cmp(type, "spoons"))         {return "outercourse";}
        else if (!str_cmp(type, "spooning"))       {return "outercourse";}
        else if (!str_cmp(type, "starfish"))       {return "noncoital";}
        else if (!str_cmp(type, "strapon"))        {return "noncoital";}
        else if (!str_cmp(type, "suck"))           {return "noncoital";}
        else if (!str_cmp(type, "sucking"))        {return "noncoital";}
        else if (!str_cmp(type, "suckoff"))        {return "noncoital";}
        else if (!str_cmp(type, "sumata"))         {return "outercourse";}
        else if (!str_cmp(type, "titjob"))         {return "outercourse";}
        else if (!str_cmp(type, "toy"))            {return "noncoital";}
        else if (!str_cmp(type, "toys"))           {return "noncoital";}
        else if (!str_cmp(type, "trib"))           {return "False";}
        else if (!str_cmp(type, "tribadism"))      {return "False";}
        else if (!str_cmp(type, "vagina"))         {return "coital";}
        else if (!str_cmp(type, "vaginal"))        {return "coital";}
        else if (!str_cmp(type, "vibrator"))       {return "outercourse";}
      }
      // Female homosexual sex
      else if (bottom_penis == 0) {
        if      (!str_cmp(type, "anal"))           {return "noncoital";}
        else if (!str_cmp(type, "analoral"))       {return "outercourse";}
        else if (!str_cmp(type, "anal-oral"))      {return "outercourse";}
        else if (!str_cmp(type, "anilingus"))      {return "outercourse";}
        else if (!str_cmp(type, "ass"))            {return "noncoital";}
        else if (!str_cmp(type, "axillary"))       {return "False";}
        else if (!str_cmp(type, "butt"))           {return "noncoital";}
        else if (!str_cmp(type, "butterfly"))      {return "noncoital";}
        else if (!str_cmp(type, "buttlicking"))    {return "outercourse";}
        else if (!str_cmp(type, "circlejerk"))     {return "outercourse";}
        else if (!str_cmp(type, "coital"))         {return "noncoital";}
        else if (!str_cmp(type, "cowgirl"))        {return "noncoital";}
        else if (!str_cmp(type, "cunnilingus"))    {return "outercourse";}
        else if (!str_cmp(type, "daisychain"))     {return "outercourse";}
        else if (!str_cmp(type, "deepthroat"))     {return "noncoital";}
        else if (!str_cmp(type, "dildo"))          {return "noncoital";}
        else if (!str_cmp(type, "docking"))        {return "false";}
        else if (!str_cmp(type, "dong"))           {return "noncoital";}
        else if (!str_cmp(type, "bagpiping"))      {return "False";}
        else if (!str_cmp(type, "bent"))           {return "noncoital";}
        else if (!str_cmp(type, "bentover"))       {return "noncoital";}
        else if (!str_cmp(type, "blow"))           {return "outercourse";}
        else if (!str_cmp(type, "blowing"))        {return "outercourse";}
        else if (!str_cmp(type, "blowjob"))        {return "outercourse";}
        else if (!str_cmp(type, "doggy"))          {return "noncoital";}
        else if (!str_cmp(type, "doggystyle"))     {return "noncoital";}
        else if (!str_cmp(type, "fellatio"))       {return "noncoital";}
        else if (!str_cmp(type, "fingering"))      {return "noncoital";}
        else if (!str_cmp(type, "footjob"))        {return "outercourse";}
        else if (!str_cmp(type, "forking"))        {return "noncoital";}
        else if (!str_cmp(type, "frog"))           {return "noncoital";}
        else if (!str_cmp(type, "frot"))           {return "False";}
        else if (!str_cmp(type, "frottage"))       {return "outercourse";}
        else if (!str_cmp(type, "handjob"))        {return "outercourse";}
        else if (!str_cmp(type, "intercrural"))    {return "outercourse";}
        else if (!str_cmp(type, "lapdance"))       {return "outercourse";}
        else if (!str_cmp(type, "mammary"))        {return "outercourse";}
        else if (!str_cmp(type, "mission"))        {return "noncoital";}
        else if (!str_cmp(type, "misionary"))      {return "noncoital";}
        // intentional misspelling
        else if (!str_cmp(type, "missonary"))      {return "noncoital";}
        // intentional misspelling
        else if (!str_cmp(type, "missionary"))     {return "noncoital";}
        else if (!str_cmp(type, "noncoital"))      {return "noncoital";}
        else if (!str_cmp(type, "nonpenetrative")) {return "outercourse";}
        else if (!str_cmp(type, "oral"))           {return "outercourse";}
        else if (!str_cmp(type, "outercourse"))    {return "outercourse";}
        else if (!str_cmp(type, "penetrative"))    {return "noncoital";}
        else if (!str_cmp(type, "penis"))          {return "noncoital";}
        else if (!str_cmp(type, "pet"))            {return "outercourse";}
        else if (!str_cmp(type, "petting"))        {return "outercourse";}
        else if (!str_cmp(type, "pitjob"))         {return "outercourse";}
        else if (!str_cmp(type, "reversecowgirl")) {return "noncoital";}
        else if (!str_cmp(type, "rimjob"))         {return "outercourse";}
        else if (!str_cmp(type, "rimming"))        {return "outercourse";}
        else if (!str_cmp(type, "rubbing"))        {return "outercourse";}
        else if (!str_cmp(type, "scissor"))        {return "outercourse";}
        else if (!str_cmp(type, "scissoring"))     {return "outercourse";}
        else if (!str_cmp(type, "shocker"))        {return "noncoital";}
        else if (!str_cmp(type, "69"))             {return "outercourse";}
        else if (!str_cmp(type, "sixtynine"))      {return "outercourse";}
        else if (!str_cmp(type, "sixty-nine"))     {return "outercourse";}
        else if (!str_cmp(type, "spoon"))          {return "outercourse";}
        else if (!str_cmp(type, "spoons"))         {return "outercourse";}
        else if (!str_cmp(type, "spooning"))       {return "outercourse";}
        else if (!str_cmp(type, "starfish"))       {return "noncoital";}
        else if (!str_cmp(type, "strapon"))        {return "noncoital";}
        else if (!str_cmp(type, "suck"))           {return "outercourse";}
        else if (!str_cmp(type, "sucking"))        {return "outercourse";}
        else if (!str_cmp(type, "suckoff"))        {return "outercourse";}
        else if (!str_cmp(type, "sumata"))         {return "False";}
        else if (!str_cmp(type, "titjob"))         {return "False";}
        else if (!str_cmp(type, "toy"))            {return "noncoital";}
        else if (!str_cmp(type, "toys"))           {return "noncoital";}
        else if (!str_cmp(type, "trib"))           {return "outercourse";}
        else if (!str_cmp(type, "tribadism"))      {return "outercourse";}
        else if (!str_cmp(type, "vagina"))         {return "noncoital";}
        else if (!str_cmp(type, "vaginal"))        {return "noncoital";}
        else if (!str_cmp(type, "vibrator"))       {return "outercourse";}
      }
    }
    send_to_char("`cSyntax`g: `WSex `g(`Wtarget`g) (`Wtype`g)`x\n\r", ch);
    send_to_char("`cSyntax`g: `WSex `Wcategory`g (`Wsex position or type`g) (`Worientation`g)`x\n\r",ch);
    send_to_char("`x\n\r", ch);
    send_to_char("`cValid types`g: `Wcoital`g, `Wnoncoital`g, `Woutercourse`x\n\r", ch);
    send_to_char("`x\n\r", ch);
    append_file(ch, "sextype.txt", type);
    return "False";
  }

  // Handles valid risk arguments and interpretation
  char *process_risk_arguments(CHAR_DATA *ch, CHAR_DATA *top, CHAR_DATA *bottom, char risk[MSL]) {

    if      (!str_cmp(risk, "safe"))        {return "condom";}
    else if (!str_cmp(risk, "risky"))       {return "none";}
    else if (!str_cmp(risk, "unprotect"))   {return "none";}
    else if (!str_cmp(risk, "unprotected")) {return "none";}
    else if (!str_cmp(risk, "none"))        {return "none";}
    else if (!str_cmp(risk, "pullout"))     {return "pullout";}
    else if (!str_cmp(risk, "interrupt"))   {return "pullout";}
    else if (!str_cmp(risk, "interrupted")) {return "pullout";}
    else if (!str_cmp(risk, "condom"))      {return "condom";}
    else if (!str_cmp(risk, "withdraw"))    {return "pullout";}
    else if (!str_cmp(risk, "withdrawal"))  {return "pullout";}
    else if (!str_cmp(risk, "pretend"))     {return "pretend";}
    else if (!str_cmp(risk, "accident"))    {return "accident";}
    else if (!str_cmp(risk, "accidental"))  {return "accident";}
    else if (!str_cmp(risk, "slipoff"))     {return "slipoff";}
    else if (!str_cmp(risk, "slippedoff"))  {return "slipoff";}

    send_to_char("`cSyntax`g: `WSex `g(`Wtarget`g) (`Wrisk`g) (`Wtype`g)`x\n\r", ch);
    send_to_char("`x\n\r", ch);
    send_to_char("`cValid risk arguments`x\n\r", ch);
    send_to_char("-------------------------------------------------------------------------------`x\n\r",ch);
    send_to_char("  `cValid options`g: `WAccident`g, `WCondom`g, `WPullout`g, `WPretend`g, `WSlipoff`g, `WNone`x `x\n\r", ch);
    send_to_char("`x\n\r", ch);

    return "False";
  }

  void update_standards(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch == NULL || victim == NULL)
    return;

    int diff = 0;
    diff = get_attract(victim, ch) - get_attract(ch, victim);
    diff += get_tier(victim) * 2;
    diff -= get_tier(ch) * 2;

    int flat = get_attract(victim, ch) - 60;
    flat /= 2;

    int total = diff + flat;

    if (total < -200 || total > 200)
    return;

    if (ch->pcdata->attract[ATTRACT_STANDARDS] < -2000
        || ch->pcdata->attract[ATTRACT_STANDARDS] > 2000) {
      ch->pcdata->attract[ATTRACT_STANDARDS] = 0;
    }
    if (ch->pcdata->attract[ATTRACT_STANDARDS] == 0) {
      ch->pcdata->attract[ATTRACT_STANDARDS] = total;
    }
    else {
      if (ch->pcdata->attract[ATTRACT_STANDARDS] < total) {
        int val = (total - ch->pcdata->attract[ATTRACT_STANDARDS]) / 5;
        ch->pcdata->attract[ATTRACT_STANDARDS] += val;
      }
      else if (total < ch->pcdata->attract[ATTRACT_STANDARDS]) {
        int val = (ch->pcdata->attract[ATTRACT_STANDARDS] - total) / 5;
        ch->pcdata->attract[ATTRACT_STANDARDS] -= val;
      }
    }
  }

  void sex_bloodify(CHAR_DATA *ch) {
    // OBJ_DATA *obj;
    // int iWear;
    if (IS_NPC(ch)) {
      return;
    }

    ch->pcdata->blood[0] += 31;

    /*
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
      if ( ( obj = get_eq_char( ch, iWear ) ) != NULL
      && (can_see_wear(ch, iWear)))
      {
        if(obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_WEAPON
        && obj->item_type != ITEM_RANGED && obj->item_type != ITEM_JEWELRY)
        continue;
        
        if(obj->item_type == ITEM_CLOTHING) {obj->value[3] = UMIN(obj->value[3]+31, 500);}
        if(obj->item_type == ITEM_JEWELRY)  {obj->value[4] = UMIN(obj->value[4]+31, 500);}
      }
    }
    */
  }

  // Processes sex after consent, if needed, is given
  void have_sex(CHAR_DATA *ch, CHAR_DATA *victim, char risk[MSL], char type[MSL], CHAR_DATA *originator) {
    int i;
    char remember_bottom[MSL], remember_top[MSL];
    bool checkcharacterlatent = TRUE;
    bool checkvictimlatent = TRUE;
    CHAR_DATA *top, *bottom;
    bool chdating = FALSE;
    bool vicdating = FALSE;
    bool impotent = FALSE;

    for (int x = 0; x < 10; x++) {
      if ((ch->pcdata->relationship_type[x] == REL_DATING || ch->pcdata->relationship_type[x] == REL_SPOUSE)
          && !str_cmp(ch->pcdata->relationship[x], victim->name)) {
        chdating = TRUE;
      }
      if ((victim->pcdata->relationship_type[x] == REL_DATING || victim->pcdata->relationship_type[x] == REL_SPOUSE)
          && !str_cmp(victim->pcdata->relationship[x], ch->name)) {
        vicdating = TRUE;
      }
    }
    int chprom = 0;
    int vicprom = 0;

    if (impotent == FALSE) {
      if (is_dreaming(ch)) {
        ch->pcdata->last_sextype = SEXTYPE_NONE;
        victim->pcdata->last_sextype = SEXTYPE_NONE;
        ch->pcdata->count_dreamsex++;
        victim->pcdata->count_dreamsex++;
      }
      else { // if not sleeping
        disease_check(ch, victim);
        disease_check(victim, ch);

        ch->pcdata->week_tracker[TRACK_SEX]++;
        ch->pcdata->life_tracker[TRACK_SEX]++;
        victim->pcdata->week_tracker[TRACK_SEX]++;
        victim->pcdata->life_tracker[TRACK_SEX]++;

        // Sex date tracking
        ch->pcdata->last_sex = current_time;
        victim->pcdata->last_sex = current_time;

        // Sex type tracking
        if (!str_cmp(type, "noncoital") || !str_cmp(type, "coital")) {
          ch->pcdata->last_sextype = SEXTYPE_INTERCOURSE;
          victim->pcdata->last_sextype = SEXTYPE_INTERCOURSE;
        }
        else if (!str_cmp(type, "outercourse")) {
          ch->pcdata->last_sextype = SEXTYPE_OUTERCOURSE;
          victim->pcdata->last_sextype = SEXTYPE_OUTERCOURSE;
        }

        // protection tracking
        if (!str_cmp(risk, "none")) {
          ch->pcdata->last_sexprotection = SEXPROTECTION_NONE;
          victim->pcdata->last_sexprotection = SEXPROTECTION_NONE;
        }
        else if (!str_cmp(risk, "accident")) {
          ch->pcdata->last_sexprotection = SEXPROTECTION_ACCIDENT;
          victim->pcdata->last_sexprotection = SEXPROTECTION_ACCIDENT;
        }
        else if (!str_cmp(risk, "pretend")) {
          ch->pcdata->last_sexprotection = SEXPROTECTION_PRETEND;
          victim->pcdata->last_sexprotection = SEXPROTECTION_PRETEND;
        }
        else if (!str_cmp(risk, "pullout")) {
          ch->pcdata->last_sexprotection = SEXPROTECTION_PULLOUT;
          victim->pcdata->last_sexprotection = SEXPROTECTION_PULLOUT;
        }
        else if (!str_cmp(risk, "slipoff")) {
          ch->pcdata->last_sexprotection = SEXPROTECTION_SLIPOFF;
          victim->pcdata->last_sexprotection = SEXPROTECTION_SLIPOFF;
        }
        else if (!str_cmp(risk, "condom")) {
          ch->pcdata->last_sexprotection = SEXPROTECTION_CONDOM;
          victim->pcdata->last_sexprotection = SEXPROTECTION_CONDOM;
        }

        // Sex mess tracking
        if (!str_cmp(risk, "none") && str_cmp(type, "noncoital") && str_cmp(type, "voyeur")) {
          ch->pcdata->sex_dirty = TRUE;
          victim->pcdata->sex_dirty = TRUE;
        }

        //hp tracking
        if (higher_power(ch))     {ch->pcdata->hp_sex     = current_time;}
        if (higher_power(victim)) {victim->pcdata->hp_sex = current_time;}

        if (!str_cmp(type, "voyeur")) {
          chprom = 20;
          if (!chdating) {
            social_behave_mod(ch, -10, "watching sex with a stranger.");
            chprom = chprom * 4 / 3;
          }
          ch->pcdata->attract[ATTRACT_PROM] += chprom;
        }
        else {
          // uID of last sexual partner without exception
          ch->pcdata->last_true_sexed_ID = victim->id;
          victim->pcdata->last_true_sexed_ID = ch->id;

          if (ch->privaterpexp < 100 && get_tier(ch) < get_tier(victim)) {
            psychic_feast(ch, PSYCHIC_LUST, 120);
          }

          if (victim->privaterpexp < 100 && get_tier(ch) > get_tier(victim)) {
            psychic_feast(victim, PSYCHIC_LUST, 120);
          }

          if (!str_cmp(ch->pcdata->last_sexed[0], victim->name)) {
            chprom += 2;
            chprom += victim->pcdata->attract[ATTRACT_PROM] / 100;
          }
          else if (!str_cmp(ch->pcdata->last_sexed[1], victim->name)) {
            chprom += 30;
            chprom += victim->pcdata->attract[ATTRACT_PROM] / 20;
          }
          else if (!str_cmp(ch->pcdata->last_sexed[2], victim->name)) {
            chprom += 50;
            chprom += victim->pcdata->attract[ATTRACT_PROM] / 10;
          }
          else {
            chprom += 40;
            chprom += victim->pcdata->attract[ATTRACT_PROM] / 10;
          }

          // chprom caps
          // Disco rewrite 11/7/2018 mimics prior functionality, but accounts for new arguments
          if (!str_cmp(risk, "condom") || is_undead(ch) || is_undead(victim) || str_cmp(type,"outercourse")) {
            chprom = UMIN(chprom, 40);
          }
          else if (ch->pcdata->account->maxhours < 150) {
            chprom = UMIN(chprom, 80);
          }
          else if (!str_cmp(risk, "pullout")) {
            chprom = UMIN(chprom, 120);
          }
          else if (!is_angelborn(ch)) {
            chprom *= 2;
          }

          if (!chdating) {
            social_behave_mod(ch, -20, "sex with a stranger.");
            chprom = chprom * 4 / 3;
          }

          ch->pcdata->attract[ATTRACT_PROM] += chprom;
          update_standards(ch, victim);

          if (str_cmp(ch->pcdata->last_sexed[0], victim->name) && victim->played / 3600 > 100
              && !IS_FLAG(ch->comm, COMM_NOFRIGID)) {
            SET_FLAG(ch->comm, COMM_NOFRIGID);
          }

          if (!str_cmp(victim->pcdata->last_sexed[0], ch->name)) {
            vicprom += 2;
            vicprom += ch->pcdata->attract[ATTRACT_PROM] / 100;
          }
          else if (!str_cmp(victim->pcdata->last_sexed[1], ch->name)) {
            vicprom += 30;
            vicprom += ch->pcdata->attract[ATTRACT_PROM] / 20;
          }
          else if (!str_cmp(victim->pcdata->last_sexed[2], ch->name)) {
            vicprom += ch->pcdata->attract[ATTRACT_PROM] / 10;
            vicprom += 40;
          }
          else {
            vicprom += ch->pcdata->attract[ATTRACT_PROM] / 10;
            vicprom += 50;
          }

          // vicprom caps
          if (!str_cmp(risk, "condom") || is_undead(ch) || is_undead(victim) || str_cmp(type, "outercourse")) {
            vicprom = UMIN(vicprom, 80);
          }
          else if (!str_cmp(risk, "pullout")) {
            vicprom = UMIN(vicprom, 120);
          }
          else if (victim->pcdata->account->maxhours < 150) {
            vicprom = UMIN(vicprom, 80);
          }
          else if (!is_angelborn(victim)) {
            vicprom *= 2;
          }

          if (!vicdating) {
            vicprom = vicprom * 4 / 3;
            social_behave_mod(victim, -20, "sex with a stranger");
          }

          victim->pcdata->attract[ATTRACT_PROM] += vicprom;
          update_standards(victim, ch);

          if (str_cmp(victim->pcdata->last_sexed[0], ch->name) && ch->played / 3600 > 100
              && !IS_FLAG(victim->comm, COMM_NOFRIGID)) {
            SET_FLAG(victim->comm, COMM_NOFRIGID);
          }

          if (str_cmp(ch->pcdata->last_sexed[0], victim->name)) {
            free_string(ch->pcdata->last_sexed[2]);
            ch->pcdata->last_sexed[2] = str_dup(ch->pcdata->last_sexed[1]);
            free_string(ch->pcdata->last_sexed[1]);
            ch->pcdata->last_sexed[1] = str_dup(ch->pcdata->last_sexed[0]);
            free_string(ch->pcdata->last_sexed[0]);
            ch->pcdata->last_sexed[0] = str_dup(victim->name);
          }

          if (str_cmp(victim->pcdata->last_sexed[0], ch->name)) {
            free_string(victim->pcdata->last_sexed[2]);
            victim->pcdata->last_sexed[2] = str_dup(victim->pcdata->last_sexed[1]);
            free_string(victim->pcdata->last_sexed[1]);
            victim->pcdata->last_sexed[1] = str_dup(victim->pcdata->last_sexed[0]);
            free_string(victim->pcdata->last_sexed[0]);
            victim->pcdata->last_sexed[0] = str_dup(ch->name);
          }

          // uID last sexed tracking
          if (ch->pcdata->last_sexed_ID[0] != victim->id) {
            ch->pcdata->last_sexed_ID[2] = ch->pcdata->last_sexed_ID[1];
            ch->pcdata->last_sexed_ID[1] = ch->pcdata->last_sexed_ID[0];
            ch->pcdata->last_sexed_ID[0] = victim->id;
          }

          if (victim->pcdata->last_sexed_ID[0] != ch->id) {
            victim->pcdata->last_sexed_ID[2] = victim->pcdata->last_sexed_ID[1];
            victim->pcdata->last_sexed_ID[1] = victim->pcdata->last_sexed_ID[0];
            victim->pcdata->last_sexed_ID[0] = ch->id;
          }
        }
      }
    }

    if (!str_cmp(type, "voyeur")) {
      sprintf(log_buf, "%s watched %s have sex.\n\r", ch->name, victim->name);
      sex_cleanup(ch);
      return;
    }
    else {
      sprintf(log_buf, "%s had sex with %s.\n\r", ch->name, victim->name);
    }

    wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

    // Regular sex messages
    if ((ch->pcdata->sexing == victim)) {
      top = ch;
      bottom = victim;

      if (is_dreaming(top) && is_dreaming(bottom)) {
        strcpy(remember_top, dream_name(top));
        strcpy(remember_bottom, dream_name(bottom));
      }
      else {
        strcpy(remember_top, (PERS(top, bottom)));
        strcpy(remember_bottom, (PERS(bottom, top)));
      }

      /*
      //Following hoops are for addressing capital letters that start intros
      first=remember_bottom[0];

      if (remember_bottom[0] == 'A' && remember_bottom[1] == ' ')
      putchar (tolower(first));
      else if (remember_bottom[0] == 'A' && remember_bottom[1] == 'n' && remember_bottom[2] == ' ') putchar (tolower(first));

      first=remember_top[0];

      if (remember_top[0] == 'A' && remember_top[1] == ' ')
      putchar (tolower(first));
      else if (remember_top[0] == 'A' && remember_top[1] == 'n' && remember_top[2] == ' ') putchar (tolower(first));
      */
      // sex with no penis
      if (top->pcdata->penis == 0 && bottom->pcdata->penis == 0) {
        if (!str_cmp(risk, "none")) {
          if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected noncoital intercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,    "You have unprotected outercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected outercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "pullout")) {
          if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected noncoital intercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,    "You have unprotected outercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected outercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "condom")) {
          if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have noncoital intercourse with %s and use protection.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has noncoital intercourse with you and uses protection.`x\n\r", remember_top);
            act("$n has noncoital intercourse with $N and uses protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,   "You have outercourse with %s and use protection.`x\n\r", remember_bottom);
            printf_to_char(bottom,"%s has outercourse with you and uses protection.`x\n\r", remember_top);
            act("$n has outercourse with $N and uses protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
      }
      // sex with penis
      else {
        if (!str_cmp(risk, "none")) {
          if (!str_cmp(type, "coital")) {
            if (top->pcdata->penis == 0) { // Female initiator messages
              if (get_skill(top, SKILL_VIRGIN) >= 1 && top->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s.  You give him your virginity and he finishes inside you.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you.  She gives you her virginity and you finish inside her.`x\n\r",   remember_top);
                act("$n has unprotected coital sex with $N.  She gives him her virginity and he finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s.  He finishes inside you.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you.  You finish inside her.`x\n\r",   remember_top);
                act("$n has unprotected coital sex with $N.  He finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }
            else { // Male initiator messages
              if (get_skill(bottom, SKILL_VIRGIN) >= 1 && bottom->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s.  You take her virginity and finish inside her.`x\n\r",   remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you.  He takes your virginity and finishes inside you.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N.  He takes her virginity and finishes inside $M.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s.  You finish inside her.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you.  He finishes inside you.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N.  He finishes inside $M.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }

            baby_batter(ch, victim, impotent);
          }
          else if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected noncoital intercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,    "You have unprotected outercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected outercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "pretend")) {
          if (!str_cmp(type, "coital")) {
            if (top->pcdata->penis == 0) { // Female initiator messages
              if (get_skill(top, SKILL_VIRGIN) >= 1 && top->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s and give him your virginity.  You pretend that you're going to pull off, but don't and he finishes inside you instead.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you and gives you her virginity.  She doesn't pull off in time and you finish inside her instead.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and  gives him her virginity.  She doesn't pull off before he finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s and pretend that you're going to pull off.  You don't and he finishes inside you instead.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you.  She doesn't pull off in time and you finish inside her instead.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and doesn't pull off before he finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }
            else { // Male initiator messages
              if (get_skill(bottom, SKILL_VIRGIN) >= 1 && bottom->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s and take her virginity.  You pretend that you're going to pull out, but don't and finish inside her instead.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you and takes your virginity.  He doesn't pull out in time and finishes inside you instead.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and takes her virginity.  He doesn't pull out before he finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s and pretend that you're going to pull out.  You don't and finish inside her instead.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you.  He doesn't pull out in time and finishes inside you instead.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and doesn't pull out before he finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }

            baby_batter(ch, victim, impotent);
          }
          else if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected noncoital intercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,    "You have unprotected outercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected outercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "slipoff")) {
          if (!str_cmp(type, "coital")) {
            if (top->pcdata->penis == 0) { // Female initiator messages
              if (get_skill(top, SKILL_VIRGIN) >= 1 && top->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have protected coital sex with %s and give him your virginity.  Before he's done, you slip the condom off and he finishes inside you instead.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has protected coital sex with you and gives you her virginity.  Before it's done, she slips the condom off and you finish inside her instead.`x\n\r",  remember_top);
                act("$n has protected coital sex with $N and gives him her virginity.  Before he's done, she slips the condom off and he finishes inside her instead.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have protected coital sex with %s.  Before he's done, you slip the condom off.  He finishes inside you instead.`x\n\r",  remember_bottom);
                printf_to_char(bottom, "%s has protected coital sex with you.  Before it's done, she slips the condom off and you finish inside her instead.`x\n\r", remember_top);
                act("$n has protected coital sex with $N.  Before he's done, she slips the condom off and he finishes inside her instead.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }
            else { // Male initiator messages
              if (get_skill(bottom, SKILL_VIRGIN) >= 1 && bottom->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have protected coital sex with %s and take her virginity.  Before you're done, you slip the condom off and finish inside her instead.`x\n\r",  remember_bottom);
                printf_to_char(bottom, "%s has protected coital sex with you and takes your virginity.  Before he's done, he slips the condom off and finishes inside you instead.`x\n\r", remember_top);
                act("$n has protected coital sex with $N and takes her virginity.  Before he's done, he slips the condom off and finishes inside her instead.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have protected coital sex with %s.  Before you're done, you slip the condom off and finish inside her instead.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has protected coital sex with you.  Before he's done, he slips the condom off and finishes inside you instead.`x\n\r",  remember_top);
                act("$n has protected coital sex with $N.  Before he's done, he slips the condom off and finishes inside her instead.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }

            baby_batter(ch, victim, impotent);
          }
          else if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have protected noncoital intercourse with %s, but slip the condom off before it's over.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has protected noncoital intercourse with you, but slips the condom off before it's over.`x\n\r", remember_top);
            act("$n has protected noncoital intercourse with $N, but slips the condom off before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,    "You have protected outercourse with %s, but slip the condom off before it's over.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has protected outercourse with you, but slips the condom off before it's over.`x\n\r", remember_top);
            act("$n has protected outercourse with $N, but slips the condom off before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "accident")) {
          if (!str_cmp(type, "coital")) {
            if (top->pcdata->penis == 0) { // Female initiator messages
              if (get_skill(top, SKILL_VIRGIN) >= 1 && top->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s and give him your virginity.  You don't pull off in time and he finishes inside you.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you and gives you her virginity.  She doesn't pull off in time and you finish inside her.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and gives him her virginity.  She doesn't pull off in time and he finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s, but don't pull off in time.  He finishes inside you.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you, but doesn't pull off in time.  You finish inside her.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N, but doesn't pull off in time.  He finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }
            else { // Male initiator messages
              if (get_skill(bottom, SKILL_VIRGIN) >= 1 && bottom->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s and take her virginity.  You don't pull out in time and finish inside her.`x\n\r",     remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you and takes your virginity.  He doesn't pull out in time and finishes inside you.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and takes her virginity.  He doesn't pull out in time and finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s, but don't pull out in time.  You finish inside her.`x\n\r",  remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you, but doesn't pull out in time  He finishes inside you.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N, but doesn't pull out in time.  He finishes inside her.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }

            baby_batter(ch, victim, impotent);
          }
          else if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected noncoital intercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,    "You have unprotected outercourse with %s.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected outercourse with you.`x\n\r",  remember_top);
            act("$n has unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "pullout")) {
          if (!str_cmp(type, "coital")) {
            if (top->pcdata->penis == 0) { // Female initiator messages
              if (get_skill(top, SKILL_VIRGIN) >= 1 && top->pcdata->hymen_lost == 0) {
                printf_to_char(top,    "You have unprotected coital sex with %s and give him your virginity.  You pull off before it's over.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you and gives you her virginity.  She pulls off before it's over.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and gives him her virginity.  She pulls off before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s, but pull off before it's over.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you, but pulls off before it's over.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N, but pulls off before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }
            else { // Male initiator messages
              if (get_skill(bottom, SKILL_VIRGIN) >= 1 && bottom->pcdata->hymen_lost == 0) {
                printf_to_char(top,   "You have unprotected coital sex with %s and take her virginity.  You pull out before it's over.`x\n\r",  remember_bottom);
                printf_to_char(bottom,"%s has unprotected coital sex with you and takes your virginity.  He pulls out before it's over.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N and takes her virginity.  He pulls out before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
              else {
                printf_to_char(top,    "You have unprotected coital sex with %s, but pull out before it's over.`x\n\r", remember_bottom);
                printf_to_char(bottom, "%s has unprotected coital sex with you, but pulls out before it's over.`x\n\r", remember_top);
                act("$n has unprotected coital sex with $N, but pulls out before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
              }
            }
          }
          else if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have unprotected noncoital intercourse with %s, but withdraw before it's over.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has unprotected noncoital intercourse with you, but withdraws before it's over.`x\n\r", remember_top);
            act("$n has unprotected noncoital intercourse with $N, but withdraws before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,   "You have unprotected outercourse with %s, but withdraw before it's over.`x\n\r", remember_bottom);
            printf_to_char(bottom,"%s has unprotected outercourse with you, but withdraws before it's over.`x\n\r", remember_top);
            act("$n has unprotected outercourse with $N, but withdraws before it's over.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
        else if (!str_cmp(risk, "condom")) {
          if (!str_cmp(type, "coital")) {
            if (top->pcdata->penis == 0) { // Female initiator switch up
              top = victim;
              bottom = ch;

              if (is_dreaming(top) && is_dreaming(bottom)) {
                strcpy(remember_top, dream_name(top));
                strcpy(remember_bottom, dream_name(bottom));
              }
              else {
                strcpy(remember_top, (PERS(top, bottom)));
                strcpy(remember_bottom, (PERS(bottom, top)));
              }
            }

            if (get_skill(bottom, SKILL_VIRGIN) >= 1 && bottom->pcdata->hymen_lost == 0) {
              printf_to_char(top,    "You have coital sex with %s and take her virginity.  You use protection.`x\n\r",  remember_bottom);
              printf_to_char(bottom, "%s has coital sex with you and takes your virginity.  He uses protection.`x\n\r", remember_top);
              act("$n has coital sex with $N and takes her virginity.  He uses protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top,    "You have coital sex with %s and use protection.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s has coital sex with you and uses protection.`x\n\r", remember_top);
              act("$n has coital sex with $N and uses protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(type, "noncoital")) {
            printf_to_char(top,    "You have noncoital intercourse with %s and use protection.`x\n\r", remember_bottom);
            printf_to_char(bottom, "%s has noncoital intercourse with you and uses protection.`x\n\r", remember_top);
            act("$n has noncoital intercourse with $N and uses protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
          else if (!str_cmp(type, "outercourse")) {
            printf_to_char(top,   "You have outercourse with %s and use protection.`x\n\r", remember_bottom);
            printf_to_char(bottom,"%s has outercourse with you and uses protection.`x\n\r", remember_top);
            act("$n has outercourse with $N and uses protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          }
        }
      }
    }

    // sanctuary loss for sex
    if (under_understanding(top, bottom)) {
      if ((str_cmp(top->pcdata->last_sexed[0], bottom->name)
            && (get_attract(top, bottom) < (get_attract(bottom, top) - 10) && number_percent() % 4 == 0))) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 12 * 60;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_NOUNDERSTANDING;
        affect_to_char(top, &af);
        nounderglow(top); // broadcasts aura change
      }
      else if ((str_cmp(top->pcdata->last_sexed[0], bottom->name)
            && number_percent() % 3 == 0) && (get_attract(top, bottom) < (get_attract(bottom, top) - 10))) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 12 * 60;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_NOUNDERSTANDING;
        affect_to_char(top, &af);
        nounderglow(top); // broadcasts aura change
      }

      if (under_understanding(bottom, top)) {
        if ((str_cmp(bottom->pcdata->last_sexed[0], top->name)
              && (get_attract(top, bottom) > (get_attract(bottom, top) + 10) && number_percent() % 4 == 0))) {
          AFFECT_DATA af;
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = 12 * 60;
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_NOUNDERSTANDING;
          affect_to_char(bottom, &af);
          nounderglow(bottom); // broadcasts aura change
        }
        else if ((str_cmp(bottom->pcdata->last_sexed[0], top->name)
              && (get_attract(top, bottom) > (get_attract(bottom, top) + 10)))) {
          AFFECT_DATA af;
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = 12 * 60;
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_NOUNDERSTANDING;
          affect_to_char(bottom, &af);
          nounderglow(bottom); // broadcasts aura change
        }
      }
    }

    if (!is_dreaming(ch)) {
      if (!is_ghost(ch) && !is_ghost(victim)) {
        // virginity handling
        if (!str_cmp(type, "coital") || !str_cmp(type, "noncoital")) {

          // physical virginity loss
          if (bottom->pcdata->penis == 0) {
            if (bottom->pcdata->hymen_lost == 0 && !str_cmp(type, "coital")) {
              bottom->pcdata->hymen_lost = current_time;
              sex_bloodify(bottom);
              sex_bloodify(top);
            }
          }

          if (top->pcdata->penis == 0) {
            if (top->pcdata->hymen_lost == 0 && !str_cmp(type, "coital")) {
              top->pcdata->hymen_lost = current_time;
              sex_bloodify(top);
              sex_bloodify(bottom);
            }
          }

          if (get_skill(bottom, SKILL_VIRGIN) >= 1) {
            if (bottom->pcdata->virginity_lost == 0) {
              if (get_skill(bottom, SKILL_VIRGIN) == 1) {
                do_function(bottom, &do_negtrain, "Virgin");
              }
              printf_to_char(bottom, "You lose your virginity to %s.`x\n\r", remember_top);
              printf_to_char(top,    "%s loses %s virginity to you.`x\n\r",  remember_bottom, (bottom->sex == SEX_MALE) ? "his" : "her");
              act("$n loses $s virginity.`x\n\r", bottom, NULL, top, TO_NOTVICT);
            }
            bottom->pcdata->virginity_lost = current_time;
          }

          if (get_skill(top, SKILL_VIRGIN) >= 1) {
            if (top->pcdata->virginity_lost == 0) {
              if (get_skill(top, SKILL_VIRGIN) == 1) {
                do_function(top, &do_negtrain, "Virgin");
              }
              printf_to_char(top,    "You lose your virginity to %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s loses %s virginity to you.`x\n\r",  remember_top, (top->sex == SEX_MALE) ? "his" : "her");
              act("$n loses $s virginity.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            top->pcdata->virginity_lost = current_time;
          }
        }
      }

      if (is_vampire(ch)) {
        ch->pcdata->deaged -= 2;
        if (str_cmp(risk, "condom")) {sex_bloodify(victim);}
      }
      if (is_vampire(victim)) {
        victim->pcdata->deaged -= 2;
        if (str_cmp(risk, "condom")) {sex_bloodify(ch);}
      }

      if (str_cmp(type, "outercourse")) {
        // Checking character archetype and applying effects
        if (is_demigod(ch)) {
          apply_godlysex(victim, 0);
          checkcharacterlatent = FALSE;
        }
        // Checking victim archetype and applying effects
        if (is_demigod(victim)) {
          apply_godlysex(ch, 0);
          checkvictimlatent = FALSE;
        }

        // Checking for latency
        if (checkcharacterlatent == TRUE || checkvictimlatent == TRUE) {
          for (i = 0; i < SKILL_USED; i++) {
            if (!str_cmp(skill_table[i].name, "Demigod")) {
              if (ch->skills[skill_table[i].vnum] == 1 && checkcharacterlatent == TRUE && !is_super(ch)) {
                apply_godlysex(victim, 0);
                checkcharacterlatent = FALSE;
              }
              if (victim->skills[skill_table[i].vnum] == 1 && checkvictimlatent == TRUE && !is_super(victim)) {
                apply_godlysex(ch, 0);
                checkvictimlatent = FALSE;
              }
            }
          }
        }
      }
    }

    // Applying sperm count reduction
    if (ch->pcdata->penis > 0) {
      ch->pcdata->sex_potency = UMAX(0, ch->pcdata->sex_potency - 20);
    }
    if (victim->pcdata->penis > 0) {
      victim->pcdata->sex_potency = UMAX(0, ch->pcdata->sex_potency - 20);
    }

    sex_cleanup(ch);
    sex_cleanup(victim);
    return;
  }

  char *day_ordinal(int day) {
    char suf[MSL];
    char daystr[MSL];

    if      (day > 4 && day < 20) {strcpy(suf, "th");}
    else if (day % 10 == 1)       {strcpy(suf, "st");}
    else if (day % 10 == 2)       {strcpy(suf, "nd");}
    else if (day % 10 == 3)       {strcpy(suf, "rd");}
    else                          {strcpy(suf, "th");}

    sprintf(daystr, "%d%s", day, suf);

    return str_dup(daystr);
  }

  // Processes the category argument for sex commands
  void sex_category(CHAR_DATA *ch, char type[MSL], char orientation[MSL]) {

    if (type[0] == '\0' || (str_cmp(orientation, "homosexual") && str_cmp(orientation, "heterosexual")
          && str_cmp(orientation, "hetero") && str_cmp(orientation, "homo"))) {
      send_to_char("`gSyntax`x: `WSex `Wcategory`g (`Wsex position or type`g) (`Worientation`g)`x\n\r", ch);
      send_to_char("`gValid orientations`x: `WHeterosexual`g, `WHomosexual`x\n\r", ch);
      return;
    }
    else {
      int bottom_penis;
      char process_result[MSL];

      if (!str_cmp(orientation, "heterosexual") || !str_cmp(orientation, "hetero")) {
        if (ch->pcdata->penis > 0) {bottom_penis = 0;}
        else                       {bottom_penis = 10;}
      }
      else if (!str_cmp(orientation, "homosexual") || !str_cmp(orientation, "homo")) {
        if (ch->pcdata->penis > 0) {bottom_penis = 10;}
        else                       {bottom_penis = 0;}
      }

      strcpy(process_result, process_type_arguments(ch, ch, bottom_penis, type));

      if (!str_cmp(type, "False")) {
        printf_to_char(ch, "`cThat `g(`W%s`g)`c isn't recognized.`x", type);
      }
      else {
        printf_to_char(ch, "`cThat `g(`W%s`g)`c counts as %s.`x", type, process_result);
      }

      bottom_penis = 0;
    }
  }

  /* In case I ever get conned into looking at group sex again - Disco
  Multiple sex commands are superior, don't require all this madness, and more.
  This is more flexible than a group sex command that applies a blanket sex
  category to all participants.
  */
  /*
  void orgy (CHAR_DATA *orgyorganizer, CHAR_DATA *orgymember, char arg[MSL])
  {
    int counter, i;
    bool stored=FALSE, errored=FALSE;

    for(i=0;i<50;i++) {
      if(orgyorganizer->pcdata->orgy_characters[i]==NULL){
        orgyorganizer->pcdata->orgy_characters[i]=str_dup("");
      }
    }

    if(!str_cmp(arg, "list")) {
      counter=1;
      send_to_char("`cOrgy List`x\n\r", orgyorganizer);
      send_to_char("`g--------------------------------------------------------------------------------`x\n\r", orgyorganizer);
      for(i=0;i<50;i++) {
        if(str_cmp("", orgyorganizer->pcdata->orgy_characters[i])) { 
          printf_to_char(orgyorganizer, "`g[`W%d`g]`x %s\n\r", counter, orgyorganizer->pcdata->orgy_characters[i]);
          counter++;
        }
      }
      send_to_char("\n\r", orgyorganizer);
      return;
    }

    //checking orgy character list
    for(i=0;i<50;i++) {
      if(!str_cmp(orgymember->name, orgyorganizer->pcdata->orgy_characters[i]) && stored==FALSE) {
        printf_to_char(orgyorganizer, "%s is uninvited to the orgy.\n\r", capitalize(arg));
        free_string(orgyorganizer->pcdata->orgy_characters[i]);
        orgyorganizer->pcdata->orgy_characters[i]=str_dup("");
        stored=TRUE;
        i=50;
      }
    }

    if(IS_NPC(orgymember)) {send_to_char( "The NPC ignores your advances.\n\r", orgyorganizer );errored=TRUE;}
    else if(orgymember==orgyorganizer){
      send_to_char( "Your body is ready.\n\r", orgyorganizer );errored=TRUE;}
    else {
      if(IS_IMMORTAL(orgymember)) {
        send_to_char( "The staffer ignores your advances.\n\r", orgyorganizer );errored=TRUE;}
      else {
        //Looking for blank spot for new character
        for(i=0;i<50;i++) {
          if(!str_cmp("", orgyorganizer->pcdata->orgy_characters[i]) && stored==FALSE) {
            orgyorganizer->pcdata->orgy_characters[i]=str_dup(orgymember->name);
            printf_to_char(orgyorganizer, "%s is invited to the orgy.\n\r", capitalize(arg)); stored=TRUE; i=50;
          }
        }
      }
    }

    if(stored==FALSE && errored==FALSE) {
      send_to_char( "Your dance card is already full.  Please remove someone from it first.\n\r", ch );
    }

    return;
  }
  */

  _DOFUN(do_sex) {
    // Syntax: sex (target's name) (type)
    char remember_top[MSL], remember_bottom[MSL];
    char risk[MSL];
    char type[MSL];
    char bottomname[MSL];
    char topname[MSL];

    CHAR_DATA *top;
    CHAR_DATA *bottom;

    if(ch->pcdata->curse == CURSE_PURITANICAL && ch->pcdata->curse_timeout > current_time)
    {
      send_to_char("Eww no, gross.", ch);
      return;
    }

    strcpy(topname, "");
    strcpy(bottomname, "");

    if (IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, topname); // Possible top: (top's name)
      argument = one_argument_nouncap(argument, bottomname); // Possible bottom: (bottom's name)
      argument = one_argument_nouncap(argument, risk); // Possible risk: risky, safe
      argument = one_argument_nouncap(argument, type); // Possible type: coital, noncoital, outercourse

      if (topname[0] == '\0' || bottomname[0] == '\0' || risk[0] == '\0' || type[0] == '\0') {
        send_to_char("`cSyntax`g: `WSex `g(`WTop`g) (`WBottom`g) (`Wrisk`g) (`Wtype`g)`x\n\r", ch);
        return;
      }
      else {
        if ((top = get_char_room(ch, NULL, topname)) == NULL) {
          send_to_char("`cTop not present.`x\n\r", ch);
          return;
        }
      }
    }
    else {
      top = ch;
      argument = one_argument_nouncap(argument, bottomname); // Possible bottom: (bottom's name)
      argument = one_argument_nouncap(argument, type); // Possible type: coital, noncoital, outercourse
      strcpy(risk,habit_level(HABIT_PROTECTION, ch->pcdata->habit[HABIT_PROTECTION]));
    }

    if (is_gm(top) && !IS_IMMORTAL(top)) {
      send_to_char("Story Runners can't have sex.`x\n\r", ch);
      return;
    }
    // display syntax if no arguments
    if (bottomname[0] == '\0') {
      send_to_char("`cSyntax`g: `WSex `g(`Wtarget`g) (`Wtype`g)`x\n\r", ch);
      send_to_char("`cSyntax`g: `WSex `Wcategory`g (`Wsex position or type`g) (`Worientation`g)`x\n\r", ch);
      return;
    }

    // outputs category for argument
    if (!str_cmp(bottomname, "category")) {
      argument = one_argument_nouncap(argument, risk);
      sex_category(ch, type, risk);
      return;
    }

    /* See comments on orgy function for why this is dumb - Disco
    if (!str_cmp(bottomname, "group")){ //orgy code
      char orgymember[MSL];
      argument = one_argument_nouncap(argument, orgymember);
      if ((bottom = get_char_world(top, orgymember)) == NULL) {
        send_to_char("`cThey're not here.`x\n\r", top);
        return;
      }

      if (safe_strlen(type) > 2 && str_cmp(type, "list")) {
        send_to_char("`cSyntax`g: `WSex `Wgroup`g (`Wlist`g|`Wremove`g|`Wclear`g)`x\n\r", top); return;
      }

      orgy(top, bottom, type);
      return;
    }
    */

    // Checks for bottom.  Assumes bottomname = bottom's name.
    if (is_dreaming(top)) {
      bottom = get_char_dream(top, bottomname);
      if (bottom == NULL || (top->pcdata->dream_room != 0
            && bottom->pcdata->dream_room != top->pcdata->dream_room)
          || (top->pcdata->dream_room == 00 && top->pcdata->dream_link != bottom)) {
        if (IS_IMMORTAL(ch)) {
          send_to_char("`cBottom not present.`x\n\r", ch);
          return;
        }
        else {
          send_to_char("`cThey're not here.`x\n\r", ch);
          return;
        }
      }
    }
    // catches for regular sex
    else {
      if ((bottom = get_char_world(ch, bottomname)) == NULL) {
        if (IS_IMMORTAL(ch)) {
          send_to_char("`cBottom not present.`x\n\r", ch);
          return;
        }
        else {
          send_to_char("`cThey're not here.`x\n\r", ch);
          return;
        }
      }
      else if (!can_see_char_distance(top, bottom, DISTANCE_NEAR)) {
        send_to_char("`cThey're not here.`x\n\r", ch);
        return;
      }
      else if (is_helpless(top)) {
        send_to_char("`cYou can't do that.`x\n\r", ch);
        return;
      }
      else if (top->modifier == MODIFIER_UNLIVING || bottom->modifier == MODIFIER_UNLIVING) {
        send_to_char("`cUnliving corpses can't have sex.`x\n\r", ch);
        return;
      }
    }
    // universal catches
    if (IS_NPC(bottom)) {
      send_to_char("`cThere's no point in having sex with mobiles.`x\n\r", ch);
      return;
    }
    else if (bottom == top) {
      send_to_char("`cYou masturbate (furiously).`x\n\r", ch);
      return;
    }
    else {
      top->pcdata->sexing = bottom;
      bottom->pcdata->sexing = top;

      if (IS_IMMORTAL(ch)) {
        strcpy(risk, process_risk_arguments(ch, top, bottom, risk));
        if (!str_cmp(risk, "False")) {
          return;
        }
      }

      if (!str_cmp(risk, "condom")) {
        // Check for contraceptives
        if (!IS_IMMORTAL(ch) && !is_dreaming(top)) {
          if (has_contraceptive_device(top, risk) == FALSE) {
            // Check for contraceptives on bottom
            if (has_contraceptive_device(bottom, risk) == FALSE) {
              printf_to_char(ch, "`cYou need a %s first.`x\n\r", risk);
              return;
            }
          }
        }
      }

      strcpy(type, process_type_arguments(ch, top, bottom->pcdata->penis, type));
      if (!str_cmp(type, "False")) {
        send_to_char("`cImproper type.`x\n\r", ch);
        return;
      }

      if (!IS_IMMORTAL(ch) && !is_dreaming(top)) {
        if (is_ghost(top)) {
          if (is_manifesting(top)) {
            if (deplete_ghostpool(top, GHOST_MANIFESTATION) == FALSE) {
              send_to_char("`cYou can't muster the strength to manipulate another body today.`x\n\r", ch);
              return;
            }
          }
          else {
            send_to_char("`cYou must be prepared to manifest your will.`x\n\r", ch);
            return;
          }
        }
        else if (is_dead(top)) {
          send_to_char("`cYour party has already come to an end.`x\n\r", ch);
          return;
        }
      }

      free_string(bottom->pcdata->sex_risk);
      free_string(bottom->pcdata->sex_type);
      bottom->pcdata->sex_risk = str_dup(risk);
      bottom->pcdata->sex_type = str_dup(type);

      if (IS_IMMORTAL(ch)) {
        have_sex(top, bottom, risk, type, ch);
      }
      else {
        if (str_cmp(type, "voyeur")) { // voyeurism doesn't require consent
          apply_seekingsex(bottom, 0);
        }

        // Determining appropriate message and sending
        if (is_dreaming(top)) {
          strcpy(remember_top, dream_name(top));
          strcpy(remember_bottom, dream_name(bottom));
        }
        else {
          strcpy(remember_top, (PERS(top, bottom)));
          strcpy(remember_bottom, (PERS(bottom, top)));
        }

        if (!str_cmp(type, "voyeur")) {
          printf_to_char(top,    "You hangs around to watch %s have sex.`x\n\r", remember_bottom);
          printf_to_char(bottom, "%s hangs around to watch you have sex.`x\n\r", remember_top);
          act("$n hangs around to watch $N have sex.`x\n\r", top, NULL, bottom, TO_NOTVICT);
          have_sex(top, bottom, risk, type, top);
        }
        else if (top->pcdata->penis == 0) {
          if (!str_cmp(risk, "none")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top,    "You are trying to have unprotected coital sex with %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected coital sex with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top,    "You are trying to have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top,    "You are trying to have unprotected outercourse with %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "pretend")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top,    "You are trying to have unprotected coital sex with %s, but pretend like you're going to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected coital sex with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top,    "You are trying to have unprotected noncoital intercourse with %s, but pretend like you're going to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top,    "You are trying to have unprotected outercourse with %s, but pretend like you're going to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "pullout")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top,    "You are trying to have unprotected coital sex with %s, but intend to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.", remember_top);
              act("$n is trying to have unprotected coital sex with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top,    "You are trying to have unprotected noncoital intercourse with %s, but intend to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top,    "You are trying to have unprotected outercourse with %s, but intend to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "accident")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top,    "You are trying to have unprotected coital sex with %s, but will accidentally not pull away in time.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.", remember_top);
              act("$n is trying to have unprotected coital sex with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top,    "You are trying to have unprotected noncoital intercourse with %s, but will accidentally not pull away in time.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top,    "You are trying to have unprotected outercourse with %s, but will accidentally not pull away in time.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "slipoff")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have coital sex with %s and want to use a %s.  You plan to slip it off before it's over.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have coital sex with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have coital sex with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have noncoital intercourse with %s and want to use a %s.  You plan to slip it off before it's over.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have noncoital intercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have noncoital intercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have outercourse with %s and want to use a %s.   You plan to slip it off before it's over.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have outercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have outercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "condom")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have coital sex with %s and want to use a %s.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have coital sex with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have coital sex with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have noncoital intercourse with %s and want to use a %s.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have noncoital intercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have noncoital intercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have outercourse with %s and want to use a %s.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have outercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have outercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
        }
        else {
          // Standard messages
          if (!str_cmp(risk, "none")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have unprotected coital sex with %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected coital sex with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have unprotected noncoital intercourse with %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have unprotected outercourse %s.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse you.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "pullout")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have unprotected coital sex with %s, but intend to pull out before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you, but intends to pull out before it's finished. Type '`Wyes`x' if this is okay.", remember_top);
              act("$n is trying to have unprotected coital sex with $N, but intends to pull out before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have unprotected noncoital intercourse with %s, but intend to pull out before it's finished.`x\n\r",remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you, but intends to pull out before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N, but intends to pull out before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have unprotected outercourse with %s, but intend to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "slipoff")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top,"You are trying to have coital sex with %s and want to use a %s.  You plan to slip it off before it's over.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have coital sex with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have coital sex with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have noncoital intercourse with %s and want to use a %s.  You plan to slip it off before it's over.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have noncoital intercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have noncoital intercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have outercourse with %s and want to use a %s.   You plan to slip it off before it's over.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have outercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have outercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "accident")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have unprotected coital sex with %s, but will accidentally not pull out in time.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you, but intends to pull out before it's finished. Type '`Wyes`x' if this is okay.", remember_top);
              act("$n is trying to have unprotected coital sex with $N, but intends to pull out before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have unprotected noncoital intercourse with %s, but will accidentally not pull out in time.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you, but intends to pull out before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N, but intends to pull out before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have unprotected outercourse with %s, but will accidentally not pull away in time.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "pretend")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have unprotected coital sex with %s, but pretend like you're going to pull out before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected coital sex with you, but intends to pull out before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected coital sex with $N, but intends to pull out before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have unprotected noncoital intercourse with %s, but pretend like you're going to pull out before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected noncoital intercourse with you, but intends to pull out before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected noncoital intercourse with $N, but intends to pull out before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have unprotected outercourse with %s, but pretend like you're going to pull away before it's finished.`x\n\r", remember_bottom);
              printf_to_char(bottom, "%s is trying to have unprotected outercourse with you, but intends to pull away before it's finished.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top);
              act("$n is trying to have unprotected outercourse with $N, but intends to pull away before it's finished.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
          else if (!str_cmp(risk, "condom")) {
            if (!str_cmp(type, "coital")) {
              printf_to_char(top, "You are trying to have coital sex with %s and want to use a %s.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have coital sex with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have coital sex with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else if (!str_cmp(type, "noncoital")) {
              printf_to_char(top, "You are trying to have noncoital intercourse with %s and want to use a %s.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have noncoital intercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have noncoital intercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
            else {
              printf_to_char(top, "You are trying to have outercourse with %s and want to use a %s.`x\n\r", remember_bottom, risk);
              printf_to_char(bottom, "%s is trying to have outercourse with you and wants to use a %s.  Type '`Wyes`x' if this is okay.`x\n\r", remember_top, risk);
              act("$n is trying to have outercourse with $N and wants to use protection.`x\n\r", top, NULL, bottom, TO_NOTVICT);
            }
          }
        }
      }
    }
    return;
  }

  void dream_sex(CHAR_DATA *ch, CHAR_DATA *victim) {
    sex_cleanup(ch);
    sex_cleanup(victim);
    printf_to_char(ch, "You have sex with %s in the dreamscape.\n\r", dream_name(victim));
    printf_to_char(victim, "You have sex with %s in the dreamscape.\n\r", dream_name(ch));
  }

  bool is_virgin(CHAR_DATA *ch) {
    if (get_skill(ch, SKILL_VIRGIN) > 0 && ch->pcdata->virginity_lost == 0) {
      return TRUE;
    }
    return FALSE;

  }

  // Redirect
  _DOFUN(do_ijusthadsex) {
    do_function(ch, &do_sex, "");
    return;
  }

#if defined(__cplusplus)
}
#endif

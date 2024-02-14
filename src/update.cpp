#if defined (_WIN31)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <map>
#include "merc.h"
#include "music.h"
#include "recycle.h"
#include "gsn.h"
#include "lookup.h"
#include "global.h"
#include "const.h"
#include "tables.h"

#ifndef WIN32
#include <sys/stat.h>
#endif

#include "olc.h"
#include "gsn.h"

#if defined(__cplusplus)
extern "C" {
#endif



  void wetify args((CHAR_DATA * ch));
  void snowify args((CHAR_DATA * ch));
  void dry args((CHAR_DATA * ch));

  void healup args((CHAR_DATA * ch));
  int hit_gain args((CHAR_DATA * ch));
  int move_gain args((CHAR_DATA * ch));
  void send_message_to_char args((CHAR_DATA * ch));
  void mobile_update args((CHAR_DATA * ch));
  void fight_update args((void));
  void time_update args((void));
  void clock_update args((void));
  void char_update args((int time_value, int roleplay_value));
  void pc_update args((CHAR_DATA * ch, int save_number));
  void obj_update args((void));
  void reboot_shutdown args((CHAR_DATA * mudkiller, bool isreboot));
  void exec_copyover args((CHAR_DATA * ch));
  void check_muddeath args((void));
  void check_copyover args((void));
  int track_room_fast args((ROOM_INDEX_DATA * from, ROOM_INDEX_DATA *to));
  void newbie_update args((void));
  void room_exit_update args((void));
  void explode_obj args((OBJ_DATA * obj, ROOM_INDEX_DATA *room));
  void second_update args((void));
  void legacy_update args((CHAR_DATA * ch));
  bool is_leap_year args((int y));
  void pledge_blood args((CHAR_DATA * ch));
  void ai_update  args( ( void ) );
  void ai_minute args( ( void ) );

  int save_number = 0;
  int snowing = 0;
  int hailing = 0;
  int raining = 0;
  int battle_countdown = 0;

  int copyover_coming_up = 0;

  int bonusrooms[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int demon_hole_room[50] = {0};
  int demon_hole_timer[50] = {0};

  int clock_hour;
  int clock_minute;
  int clock_second;

  void advance_level(CHAR_DATA *ch, bool hide) {
    char buf[MAX_STRING_LENGTH];
    int add_hp = 0, add_move = 0, add_prac = 0;

    if (ch->level == 101)
    add_prac /= 3;

    add_hp = add_hp * 9 / 10;
    add_move = add_move * 9 / 10;

    if (ch->level == LEVEL_HERO) {
      add_hp /= 5;
      add_move /= 5;
    }

    if (ch->level == LEVEL_HERO) {
    }

    if (!hide) {
      sprintf(
      buf, "Your hitpoints increase by %d points, and your movement by %d.\n\r", add_hp, add_move);
      send_to_char(buf, ch);
    }

    save_char_obj(ch, FALSE, FALSE);
    return;
  }

  bool is_alone(CHAR_DATA *ch) {
    CHAR_DATA *to;
    if(ch->in_room == NULL)
    return TRUE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      to = *it;

      if (IS_NPC(to) || to->desc == NULL || to == ch)
      continue;

      if (to == ch)
      continue;

      if (IS_FLAG(to->act, PLR_BOUND))
      continue;
      if (IS_FLAG(to->act, PLR_BOUNDFEET))
      continue;

      return FALSE;
    }
    return TRUE;
  }

  int exp_mod(int diff) {
    if (diff == 5)
    return 100;
    if (diff == 6)
    return 120;
    if (diff == 7)
    return 140;
    if (diff == 8)
    return 160;
    if (diff == 9)
    return 180;
    if (diff == 10)
    return 200;
    if (diff == 4)
    return 90;
    if (diff == 3)
    return 70;
    if (diff == 2)
    return 50;
    if (diff == 1)
    return 30;

    return 100;
  }

  int get_diff(CHAR_DATA *ch) {

    int wlevel = get_lvl(ch);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || is_gm(rch))
      continue;
      if (IS_NPC(rch) || ch == rch)
      continue;

      wlevel += (get_lvl(rch) / 2);
    }

    return UMAX(1, wlevel);
  }

  void gain_exp(CHAR_DATA *ch, int amount, int type) {
    int bonus = 0;
    amount = amount * 140 / 100;
    if (!IS_NPC(ch)) {
      if (ch->pcdata->xpbonus > 0) {
        bonus = UMIN(50, ch->pcdata->xpbonus);
        amount += bonus;
        ch->pcdata->xpbonus -= bonus;
      }
      if (ch->pcdata->xpbonus < 0) {
        bonus = UMAX(-25, ch->pcdata->xpbonus);
        amount += bonus;
        ch->pcdata->xpbonus -= bonus;
        amount = UMAX(1, amount);
      }
    }

    if((in_fight(ch) && ch->fight_fast == FALSE) || type == FEXP_RP)
    {
      if(amount > ch->pcdata->account->sparring_xp)
      {
        if(type == FEXP_RP)
        amount = UMAX(amount/25, ch->pcdata->account->sparring_xp);
        else if(type == FEXP_ATTACK)
        amount = UMAX(amount/10, ch->pcdata->account->sparring_xp);
      }
      ch->pcdata->account->sparring_xp -= amount;
    }

    if (event_recruitment != 0 && !char_in_alliance_with(ch, event_recruitment))
    amount = amount * 4 / 5;
    if (event_teaching != 0 && !char_in_alliance_with(ch, event_teaching))
    amount = amount * 4 / 5;

    if (event_teaching != 0 && char_in_alliance_with(ch, event_teaching))
    amount = amount * 7 / 5;

    if (event_catastrophe == 1)
    amount = amount / 2;

    if (ch->pcdata->training_disc == 0)
    amount /= 2;

    if (get_energy(ch) >= 100)
    amount = amount / 2;

    if (amount <= 0)
    return;

    if (ch->pcdata->account != NULL) {
      ch->pcdata->account->xp += amount;
      if (ch->exp > 0 && !is_roster_char(ch)) {
        ch->pcdata->account->xp += ch->exp;
        ch->exp = 0;
      }
    }
    else
    ch->exp += amount;
    ch->pcdata->earnt_xp += amount;
    if (!battleground(ch->in_room))
    {
      if(ch->fight_fast == TRUE)
      ch->pcdata->recent_exp = UMIN(20000, ch->pcdata->recent_exp + amount);
      else
      ch->pcdata->recent_exp = UMIN(20000, ch->pcdata->recent_exp + amount*10);
    }
    if (ch->pcdata->training_disc != 0)
    autotrain_discipline(ch, ch->pcdata->training_disc);
    else if ((number_percent() % 7 == 0 && ch->played / 3600 < 50) || number_percent() % 43 == 0)
    send_to_char("You're not working on developing any disciplines, you'll gain more exp if you are, use train (discipline name) to start.\n\r", ch);
  }

  void gain_aexp(CHAR_DATA *ch, int amount) {
    if (pc_pop(ch->in_room) < 1)
    return;

    amount = amount * 3;

    if (in_haven(ch->in_room))
    return;

    if (is_gm(ch)) {
      if (get_karma_plot(ch) != NULL)
      ch->pcdata->account->sr_aexp += amount * 6;
      return;
    }
    if (get_gm(ch->in_room, FALSE) == NULL || get_karma_plot(get_gm(ch->in_room, FALSE)) == NULL)
    return;

    if (ch->pcdata->alosses * 5 > ch->pcdata->awins)
    amount = amount * 4 / 3;

    ch->pcdata->aexp += amount;
  }

  void gain_wexp(CHAR_DATA *ch, int amount) {
    //	if(!other_players(ch))
    //	    return;

    if (is_dreaming(ch))
    return;
    int bonus = amount;
    amount = amount * 3;

    if (higher_power(ch))
    return;

    if (is_gm(ch)) {
      ch->pcdata->account->sr_aexp += amount * 8;
      return;
    }
    if(in_fight(ch) && ch->fight_fast == FALSE)
    amount /= 2;
    if (ch->race == RACE_WILDLING)
    amount = amount * 13 / 10;
    if (in_world(ch) == WORLD_WILDS) {
      ch->pcdata->wexp += amount;
      give_intel(ch, amount / 80);
    }
    else if (in_world(ch) == WORLD_GODREALM) {
      ch->pcdata->gexp += amount;
      give_intel(ch, amount / 80);
    }
    else if (in_world(ch) == WORLD_OTHER) {
      ch->pcdata->oexp += amount;
      give_intel(ch, amount / 80);
    }
    else if (in_world(ch) == WORLD_HELL) {
      ch->pcdata->hexp += amount;
      give_intel(ch, amount / 80);
    }

    if (ch->pcdata->resident == WORLD_WILDS)
    ch->pcdata->wexp += bonus;
    else if (ch->pcdata->resident == WORLD_GODREALM)
    ch->pcdata->gexp += bonus;
    else if (ch->pcdata->resident == WORLD_OTHER)
    ch->pcdata->oexp += bonus;
    else if (ch->pcdata->resident == WORLD_HELL)
    ch->pcdata->hexp += bonus;
  }

  void part_shuffle(FANTASY_TYPE *fant, int number) {
    if (number == 0)
    return;
    if (fant->participant_exp[number] > fant->participant_exp[number - 1]) {
      char *tempname = str_dup(fant->participants[number - 1]);
      char *tempshort = str_dup(fant->participant_shorts[number - 1]);
      char *tempdesc = str_dup(fant->participant_descs[number - 1]);
      char *tempdname = str_dup(fant->participant_names[number - 1]);
      char *tempfame = str_dup(fant->participant_fames[number - 1]);
      char *tempeq = str_dup(fant->participant_eq[number - 1]);
      int tempexp = fant->participant_exp[number - 1];
      int temprole = fant->participant_role[number - 1];
      int temproom = fant->participant_inroom[number - 1];
      bool tempnoact = fant->participant_noaction[number - 1];
      bool tempblind = fant->participant_blind[number - 1];
      bool tempnomove = fant->participant_nomove[number - 1];
      bool tempgod = fant->participant_godmode[number - 1];
      bool temptrust = fant->participant_trusted[number - 1];
      int tempstats[30];
      for (int i = 0; i < 30; i++)
      tempstats[i] = fant->participant_stats[number - 1][i];

      free_string(fant->participants[number - 1]);
      fant->participants[number - 1] = str_dup(fant->participants[number]);
      free_string(fant->participant_shorts[number - 1]);
      fant->participant_shorts[number - 1] =
      str_dup(fant->participant_shorts[number]);
      free_string(fant->participant_descs[number - 1]);
      fant->participant_descs[number - 1] =
      str_dup(fant->participant_descs[number]);
      free_string(fant->participant_names[number - 1]);
      fant->participant_names[number - 1] =
      str_dup(fant->participant_names[number]);
      free_string(fant->participant_fames[number - 1]);
      fant->participant_fames[number - 1] =
      str_dup(fant->participant_fames[number]);
      free_string(fant->participant_eq[number - 1]);
      fant->participant_eq[number - 1] = str_dup(fant->participant_eq[number]);
      fant->participant_exp[number - 1] = fant->participant_exp[number];
      fant->participant_role[number - 1] = fant->participant_role[number];
      fant->participant_inroom[number - 1] = fant->participant_inroom[number];
      fant->participant_noaction[number - 1] = fant->participant_noaction[number];
      fant->participant_blind[number - 1] = fant->participant_blind[number];
      fant->participant_nomove[number - 1] = fant->participant_nomove[number];
      fant->participant_trusted[number - 1] = fant->participant_trusted[number];
      fant->participant_godmode[number - 1] = fant->participant_godmode[number];
      for (int i = 0; i < 30; i++)

      {
        fant->participant_stats[number - 1][i] =
        fant->participant_stats[number][i];
      }
      if (daysidle(tempname) >= 150) {

        free_string(fant->participants[number]);
        fant->participants[number] = str_dup("");
        free_string(fant->participant_shorts[number]);
        fant->participant_shorts[number] = str_dup("");
        free_string(fant->participant_descs[number]);
        fant->participant_descs[number] = str_dup("");
        fant->participant_names[number] = str_dup("");
        fant->participant_fames[number] = str_dup("");
        fant->participant_eq[number] = str_dup("");
        fant->participant_exp[number] = 0;
        fant->participant_role[number] = 0;
        fant->participant_inroom[number] = 0;
        fant->participant_noaction[number] = 0;
        fant->participant_blind[number] = 0;
        fant->participant_nomove[number] = 0;
        fant->participant_trusted[number] = 0;
        fant->participant_godmode[number] = 0;
        for (int i = 0; i < 30; i++)
        fant->participant_stats[number][i] = 0;
      }
      else {
        free_string(fant->participants[number]);
        fant->participants[number] = str_dup(tempname);
        free_string(fant->participant_shorts[number]);
        fant->participant_shorts[number] = str_dup(tempshort);
        free_string(fant->participant_descs[number]);
        fant->participant_descs[number] = str_dup(tempdesc);
        free_string(fant->participant_names[number]);
        fant->participant_names[number] = str_dup(tempdname);
        free_string(fant->participant_fames[number]);
        fant->participant_fames[number] = str_dup(tempfame);
        free_string(fant->participant_eq[number]);
        fant->participant_eq[number] = str_dup(tempeq);
        fant->participant_exp[number] = tempexp;
        fant->participant_role[number] = temprole;
        fant->participant_inroom[number] = temproom;
        fant->participant_noaction[number] = tempnoact;
        fant->participant_blind[number] = tempblind;
        fant->participant_nomove[number] = tempnomove;
        fant->participant_trusted[number] = temptrust;
        fant->participant_godmode[number] = tempgod;
        for (int i = 0; i < 30; i++)
        fant->participant_stats[number][i] = tempstats[i];
      }
    }
  }

  bool calendar_fantasy(CHAR_DATA *ch) {
    if (!is_dreaming(ch))
    return FALSE;
    CHAR_DATA *vict;
    for (vector<STORY_TYPE *>::iterator ij = StoryVect.begin();
    ij != StoryVect.end(); ++ij) {
      if ((*ij)->valid == FALSE)
      continue;
      if ((*ij)->time <= current_time && (*ij)->time > current_time - (3600 * 6)) {
        vict = get_char_world_pc((*ij)->author);
        if (vict != NULL && is_dreaming(vict) && !str_cmp(in_fantasy(ch)->name, in_fantasy(vict)->name))
        return TRUE;
      }
    }
    return FALSE;
  }

  void gain_dexp(CHAR_DATA *ch, int amount) {
    if (!other_players(ch))
    return;
    amount = amount * 2;

    if (!str_cmp(ch->name, dream_name(ch)) || !str_cmp(ch->name, dream_introduction(ch)))
    amount = amount * 3 / 2;

    ch->pcdata->dream_timer += 4;
    if (IS_FLAG(ch->comm, COMM_PRIVATE))
    amount /= 2;

    if (ch->race == RACE_DREAMCHILD)
    amount = amount * 6 / 5;

    if (calendar_fantasy(ch))
    amount *= 3;

    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      int number = fantasy_number(fant);
      if (number == fantasy_bonus)
      amount = amount * 12 / 10;
      for (int i = 0; i < 200; i++) {
        if (!str_cmp(fant->participants[i], ch->name)) {
          fant->participant_exp[i] += amount;
          part_shuffle(fant, i);
          if (str_cmp(fant->participants[i], ch->name)) {
            for (CharList::iterator it = char_list.begin(); it != char_list.end();
            ++it) {
              CHAR_DATA *wch = *it;
              if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
              continue;
              if (IS_NPC(wch))
              continue;
              if (wch->pcdata->tempdreamgodworld == number && wch->pcdata->tempdreamgodchar == i)
              wch->pcdata->tempdreamgodchar = i - 1;
            }
          }
        }
      }
      fant->lastused = current_time;
      if (!goddreamer(ch)) {
        CHAR_DATA *auth;
        if (IS_FLAG(ch->comm, COMM_PRIVATE)) {
          //        if((auth = get_char_world_pc(fant->author)) != NULL)
          //	{
          //	    if(!same_player(auth, ch) && auth->pcdata->account !=
          //ch->pcdata->account)
          //              auth->pcdata->dexp -= UMAX(1, amount/10);
          //	}
          //      else if(fant->porn == 0)
          //        offline_reward(fant->author, TYPE_DEXP, UMIN(-1, amount/10), //        ch);
        }
        else {
          if ((auth = get_char_world_pc(fant->author)) != NULL) {
            if (!same_player(auth, ch) && auth->pcdata->account != ch->pcdata->account && fant->porn == 0 && fant->stupid == 0)
            auth->pcdata->dexp += UMAX(1, amount / 10);
          }
          else if (fant->porn == 0 && fant->stupid == 0)
          offline_reward(fant->author, TYPE_DEXP, UMAX(1, amount / 10), ch);
        }
      }
    }
    if (fant->porn == 1)
    amount /= 20;
    else if (fant->stupid == 1)
    amount /= 10;
    else if (!goddreamer(ch))
    give_intel(ch, amount / 80);

    ch->pcdata->dexp = UMIN(250000, ch->pcdata->dexp + amount);
  }

  bool can_rpexp(CHAR_DATA *ch) {
    if (!largeweapons(ch))
    return TRUE;
    if (antagonist(ch) || ch->in_room->area->vnum <= 12 || ch->in_room->area->vnum >= 19)
    return TRUE;
    if (ch->pcdata->patrol_status != 0)
    return TRUE;
    if (is_helpless(ch))
    return TRUE;
    if (in_fight(ch))
    return TRUE;
    if (higher_power(ch))
    return TRUE;
    if (room_level(ch->in_room) > 0)
    return TRUE;

    return FALSE;
  }

  void gain_rpexp(CHAR_DATA *ch, int amount) {
    int bonus = 0;

    if (IS_FLAG(ch->act, PLR_STASIS))
    return;


    if (in_fight(ch) && ch->debuff < 20) {
      if (!other_players(ch) && !battleground(ch->in_room))
      return;
      if (ch->fight_fast == FALSE)
      gain_exp(ch, amount * 15, FEXP_RP);
      else
      gain_exp(ch, amount * 2 * ch->fight_speed, FEXP_RP);
    }

    if (is_dreaming(ch)) {
      if (!other_players(ch))
      return;
      gain_dexp(ch, amount);
    }

    if (ch->in_room != NULL && ch->in_room->area->vnum == 12 && !encounter_room(ch->in_room)) {
      gain_aexp(ch, amount);
      give_intel(ch, amount / 3);
    }
    else
    gain_wexp(ch, amount);

    if (power_pop(ch->in_room) > 0)
    give_intel(ch, amount / 10);

    if (!is_super(ch))
    amount = amount * 12 / 10;

    if (is_gm(ch))
    return;

    if(ch->pcdata->fixation_timeout > current_time && fixation_happy_room(ch) == FALSE)
    ch->pcdata->account->rpxp -= 50;

    if (amount > number_percent())
    attract_average_update(ch);

    if (!can_rpexp(ch)) {
      if ((number_percent() % 7 == 0 && ch->played / 3600 < 50) || number_percent() % 43 == 0)
      send_to_char("You're not currently earning RPxp because you have large weaponry on you and aren't in a fight.\n\r", ch);
      return;
    }
    // This keeps track of xp gained while in private mode - Discordance
    if (IS_FLAG(ch->comm, COMM_PRIVATE) && !IS_FLAG(ch->act, PLR_AFTERGLOW)) {
      ch->privaterpexp += amount;
    }

    if (ch->pcdata->rpxpbonus > 0) {
      bonus = UMIN(1, ch->pcdata->rpxpbonus);
      amount += bonus;
      ch->pcdata->rpxpbonus -= bonus;
    }
    if (ch->pcdata->rpxpbonus < 0 && amount > 1) {
      bonus = UMAX(-1, ch->pcdata->rpxpbonus);
      amount += bonus;
      ch->pcdata->rpxpbonus -= bonus;
      amount = UMAX(1, amount);
    }

    if (ch->pcdata->account != NULL) {
      ch->pcdata->account->rpxp += amount;
      if (ch->rpexp > 0 && !is_roster_char(ch)) {
        ch->pcdata->account->rpxp += ch->rpexp;
        ch->rpexp = 0;
      }
    }
    else
    ch->rpexp += amount;

    institute_xp(ch, amount);
    if (ch->pcdata->in_domain != 0 && other_players(ch))
    domain_xp(ch, amount);

    if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->murder_cooldown > 0)
    ch->pcdata->murder_cooldown -= amount;
  }

  void gain_condition(CHAR_DATA *ch, int iCond, int value) {
    int condition;

    if (value == 0 || IS_NPC(ch) || ch->level >= LEVEL_IMMORTAL)
    return;

    condition = ch->pcdata->conditions[iCond];

    if (condition == -1)
    return;

    ch->pcdata->conditions[iCond] += value;

    if (ch->pcdata->conditions[iCond] == 0) {
      switch (iCond) {
      case COND_DRUNK:
        send_to_char("You are sober.\n\r", ch);
        break;
      }
    }

    return;
  }

  int const ground_vnum[] = {27000, 27020, 27040, 27060, 27080, 27100, 27120, 27140, 27160, 27180};

  bool is_bird(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum == 151 && invade_aerial_one == 1)
    return TRUE;
    if (ch->pIndexData->vnum == 152 && invade_aerial_two == 1)
    return TRUE;
    if (ch->pIndexData->vnum == 153 && invade_aerial_three == 1)
    return TRUE;

    if (ch->pIndexData->vnum == 205000000)
    return TRUE;

    if (ch->pIndexData->vnum == 205000002)
    return TRUE;

    if (ch->pIndexData->vnum == 205000004)
    return TRUE;

    if (ch->pIndexData->vnum == 205000010)
    return TRUE;

    return FALSE;
  }
  bool is_fish(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum == 205000005)
    return TRUE;

    return FALSE;
  }
  bool is_parkcreature(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum == 205000001)
    return TRUE;

    if (ch->pIndexData->vnum == 205000009)
    return TRUE;

    if (ch->pIndexData->vnum == 205000008)
    return TRUE;

    return FALSE;
  }
  bool is_sprite(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum == 205000004)
    return TRUE;

    if (ch->pIndexData->vnum == 205000003)
    return TRUE;

    return FALSE;
  }

  void civ_flee(CHAR_DATA *ch, CHAR_DATA *monster) {
    if (ch == NULL || monster == NULL)
    return;
    int dir = get_flee_direction(ch);

    if (dir == -1)
    dir = DIR_NORTH;

    act("As $N moves towards you you desperately run for cover.", ch, NULL, monster, TO_CHAR);
    act("$n runs quickly as $N moves towards $m.", ch, NULL, monster, TO_ROOM);

    move_char(ch, dir, FALSE, FALSE);
    WAIT_STATE(ch, PULSE_PER_SECOND * 2);
  }
  bool deepforest(ROOM_INDEX_DATA *room) {
    if (room->area->vnum >= 19 && room->area->vnum <= 25)
    return TRUE;
    return FALSE;
  }

  bool readytofight(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!can_get_to(ch, victim->in_room))
    return FALSE;

    if (forest_monster(ch) && IS_FLAG(victim->act, PLR_HIDE) && !in_lair(ch)) {
      if (get_skill(victim, SKILL_STEALTH) * 40 > number_percent() || number_percent() % 2 == 0)
      return FALSE;
    }

    if (combat_distance(ch, victim, FALSE) <= 180 && can_see_char_distance(victim, ch, DISTANCE_FAR) && !in_lair(ch))
    return TRUE;
    if (combat_distance(ch, victim, FALSE) <= 120 && can_see_char_distance(victim, ch, DISTANCE_FAR))
    return TRUE;

    if (ch->in_room == victim->in_room)
    return TRUE;
    return FALSE;
  }

  void mobile_update(CHAR_DATA *ch) {
    EXIT_DATA *pexit;
    if (ch == NULL)
    return;

    if (ch->in_room == NULL || !IS_NPC(ch))
    return;

    if (!ch || !ch->name)
    return;

    if (ch->ttl > 0 && ch->ttl < 25 && (in_fight(ch) || find_prey(ch) != NULL))
    ch->ttl++;
    else if (ch->ttl > 0) {
      if (ch->controled_by == NULL || ch->in_room == NULL || ch->controled_by->in_room == NULL || ch->in_room != ch->controled_by->in_room)
      ch->ttl--;
      if (in_lair(ch))
      ch->ttl = 15;
      if (forest_monster(ch) && mist_level(ch->in_room) == 0 && ch->ttl > 1)
      ch->ttl = 1;
      if (!in_fight(ch)) {
        ch->target = NULL;
        ch->target_dam = 0;
        ch->target_2 = NULL;
        ch->target_dam_2 = 0;
        ch->target_3 = NULL;
        ch->target_dam_3 = 0;
        ch->attacking = 0;

        ch->fight_speed = 1;
        ch->fight_fast = TRUE;
        ch->hadturn = FALSE;
        ch->fight_current = NULL;
      }
    }
    if (!in_fight(ch) && (ch->pIndexData->vnum == MINION_TEMPLATE || ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == HAND_SOLDIER)) {
      ch->ttl = 0;
    }
    if (ch->ttl == 0) {
      extract_char(ch, TRUE);
      return;
    }
    if (ch->wounds >= 4) {
      extract_char(ch, TRUE);
      return;
    }
    if (forest_monster(ch) && fight_problem == 1) {
      extract_char(ch, TRUE);
      return;
    }

    if (ch == NULL || ch->in_room == NULL || ch->position == POS_DEAD)
    return;

    /*
if (ch->wait > 0)
{
ch->wait--;
return;
}
*/
    if (in_lair(ch))
    ch->ttl = UMAX(ch->ttl, 30);

    bool crowded = crowded_room(ch->in_room);
    ch->recent_moved = UMAX(0, ch->recent_moved - 1);
    if (crowded == FALSE && crowded_room(ch->in_room)) {
      act("The area starts to seem crowded.", ch, NULL, NULL, TO_ROOM);
      act("The area starts to seem crowded.", ch, NULL, NULL, TO_CHAR);
    }
    int door = ch->facing;
    if (door > 9 || door < 0) {
      ch->facing = number_range(0, 9);
      door = ch->facing;
    }
    if (!in_fight(ch) && (forest_monster(ch) || is_invader(ch))) {
      CHAR_DATA *prey = find_prey(ch);
      if (prey != NULL) {
        if (IS_FLAG(ch->act, ACT_BIGGAME)) {
          ch->ttl = UMIN(ch->ttl + 1, 20);
        }
        if (readytofight(ch, prey))
        start_fight(ch, prey);
        else
        door = roomdirection(ch->in_room->x, ch->in_room->y, prey->in_room->x, prey->in_room->y);
      }
      else {
        prey = find_abductee(ch);
        if (prey != NULL) {
          if (prey->in_room == ch->in_room || combat_distance(ch, prey, FALSE) <= 25)
          monster_abduction(ch, prey);
          else
          door = roomdirection(ch->in_room->x, ch->in_room->y, prey->in_room->x, prey->in_room->y);
        }
        else {
          if (IS_FLAG(ch->act, ACT_BIGGAME))
          REMOVE_FLAG(ch->act, ACT_BIGGAME);
          if (ch->ttl > 2)
          ch->ttl -= 2;
          if (number_percent() % 12 == 0)
          door = number_range(0, 9);
        }
      }
    }
    else if (number_percent() % 12 == 0)
    door = number_range(0, 9);

    if (is_bird(ch) && ch->in_room->sector_type != SECT_AIR && ch->in_room->sector_type != SECT_ATMOSPHERE && ch->in_room->exit[DIR_UP] != NULL)
    door = DIR_UP;

    if (ch->in_room->exit[door] != NULL && IS_SET(ch->in_room->exit[door]->exit_info, EX_CLOSED))
    knock_check(ch, door);

    if (ch == NULL || ch->in_room == NULL)
    return;

    pexit = ch->in_room->exit[door];
    if (pexit == NULL)
    return;

    if (!IS_FLAG(ch->act, ACT_SENTINEL) && (!IS_FLAG(ch->act, ACT_BIGGAME)) && (door <= 9) && (ch->master == NULL) && (ch->position > POS_INCAP) && (ch->hit > 0) && (!in_fight(ch)) && (!in_lair(ch)) && (!is_invader(ch) || pexit->u1.to_room->sector_type != SECT_FOREST) && ((!forest_monster(ch) || mist_level(pexit->u1.to_room) >= 3 || (deepforest(pexit->u1.to_room)))) && (pexit = ch->in_room->exit[door]) != NULL && pexit->u1.to_room != NULL && (pexit->wall == WALL_NONE || pexit->wallcondition == WALLCOND_HOLE) && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) && (!IS_FLAG(ch->act, ACT_STAY_AREA) || pexit->u1.to_room->area == ch->in_room->area) && (!IS_FLAG(ch->act, ACT_STAY_SUBAREA) || pexit->u1.to_room->subarea == ch->in_room->subarea) && (!IS_FLAG(ch->act, ACT_OUTDOORS) || !IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS)) && (!is_fish(ch) || is_underwater(pexit->u1.to_room)) && (is_fish(ch) || (!is_water(pexit->u1.to_room) && !is_underwater(pexit->u1.to_room))) && (ch->pIndexData != NULL && ch->pIndexData->vnum != 110) && (!is_invader(ch) || pexit->u1.to_room->sector_type == SECT_STREET || number_percent() % 5 == 0) && (!is_parkcreature(ch) || pexit->u1.to_room->sector_type != SECT_STREET || number_percent() % 5 == 0) && (!is_bird(ch) || ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_ATMOSPHERE || number_percent() % 5 == 0) && (is_bird(ch) || (pexit->u1.to_room->sector_type != SECT_AIR)) && (!IS_FLAG(ch->act, ACT_INDOORS) || IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS))) {
      move_char(ch, door, FALSE, FALSE);
    }
    else if (!in_fight(ch))
    ch->facing = number_percent() % 10;

    /*
if(!in_fight(ch) && get_npc_target(ch) != NULL)
{
if(!room_fight(ch->in_room))
{
if(ch->pIndexData->vnum >= 150 && ch->pIndexData->vnum <= 153)
{
if(get_npc_target(ch)->race == RACE_CIVILIAN || get_npc_target(ch)->played/3600 < 10)
{
civ_flee(get_npc_target(ch), ch);
return;
}
}
start_combat(ch->in_room, ch);
ch->fight_fast = TRUE;
}
}
*/
    return;
  }

  bool is_leap_year(int y) {
    if ((y % 4) != 0)
    return false;

    if (y % 400)
    return true;

    if (y % 100)
    return false;

    return true;
  }

  int dayIndex(int d, int m, int y) {

    if (m == 1 || m == 2) {
      m += 12;
      d -= is_leap_year(y) ? 2 : 1;
    }

    int z = (1 + d + (m * 2) + (3 * (m + 1) / 5) + y + y / 4 - y / 100 + y / 400);
    return z % 7;
  }

  int daysInMonth(int m, int y) {
    if (m == 2)
    return is_leap_year(y) ? 29 : 28;

    if (m == 4 || m == 6 || m == 9 || m == 11)
    return 30;

    return 31;
  }

  int isWeekDay() {
    int dIndex = dayIndex(time_info.day, time_info.month, time_info.year);
    return (dIndex > 0) && (dIndex < 6);
  }

  bool is_holiday(void) {

    if (dayIndex(time_info.day, time_info.month, time_info.year) == 0 || dayIndex(time_info.day, time_info.month, time_info.year) == 6)
    return TRUE;

    return FALSE;
  }

  int const month_temps[] = {32, 33, 41, 52, 62, 72, 77, 74, 68, 59, 47, 35};

  void weather_update(void) {
    int temp;
    if (number_percent() % 3 == 0) {
      if (number_percent() % 2 == 0)
      time_info.cover_trend = UMAX(-10, time_info.cover_trend - 1);
      else
      time_info.cover_trend = UMIN(10, time_info.cover_trend + 1);
    }

    time_info.local_cover_total += time_info.cover_trend;

    if (time_info.local_cover_total > 100)
    time_info.local_cover_total = 100;
    if (time_info.local_cover_total < 0)
    time_info.local_cover_total = 0;
    if (number_percent() % 2 == 0) {
      if (number_percent() % 2 == 0)
      time_info.density_trend = UMAX(-8, time_info.density_trend - 1);
      else
      time_info.density_trend = UMIN(8, time_info.density_trend + 1);
    }
    time_info.local_density_total += time_info.density_trend / 2;

    temp = time_info.local_temp - 55;
    temp /= 2;
    if (temp > 3)
    temp = 3;
    if (temp < -3)
    temp = -3;
    time_info.local_density_total -= temp;

    if (time_info.local_density_total > 100)
    time_info.local_density_total = 100;
    if (time_info.local_density_total < 0)
    time_info.local_density_total = 0;
  }

  void global_message(char *string) {
    CHAR_DATA *to;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (to == NULL)
        continue;
        if (IS_NPC(to))
        continue;
        if (!in_haven(to->in_room))
        continue;

        send_to_char(string, to);
      }
    }
  }

  void temp_update(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);

    if (sunphase(NULL) == 4) {
      time_info.des_temp = month_temps[ptm->tm_mon];
      time_info.des_temp += number_percent() % 7;
      time_info.des_temp -= 3;
      time_info.local_temp++;
      time_info.des_temp += 2;
    }
    if ((snowing == 1 || raining == 1) && time_info.local_temp < 70 && number_percent() % 17 == 0)
    time_info.mist_timer += number_range(1, 5);
    if (time_info.local_temp < 65 && (sunphase(NULL) == 1 || sunphase(NULL) == 2) && number_percent() % 11 == 0)
    time_info.mist_timer += number_range(1, 5);
    if (time_info.local_temp < 65 && sunphase(NULL) == 0 && number_percent() % 13 == 0)
    time_info.mist_timer += number_range(1, 5);
    else if (time_info.local_temp < 65 && number_percent() % 15 == 0)
    time_info.mist_timer += number_range(1, 5);
    if (time_info.local_temp > 70 && number_percent() % 2 == 0)
    time_info.mist_timer = UMAX(0, time_info.mist_timer - 1);
    if (sunphase(NULL) == 4)
    time_info.mist_timer = UMAX(0, time_info.mist_timer - 1);

    if (time_info.local_cover_total < 85 || time_info.local_density_total < 85) {
      if (sunphase(NULL) == 0 || sunphase(NULL) == 1 || sunphase(NULL) == 6 || sunphase(NULL) == 7) {
        time_info.local_temp--;
        time_info.des_temp -= 2;
      }
      else {
        time_info.local_temp++;
        time_info.des_temp += 2;
      }
    }
    else {
      if (sunphase(NULL) == 0 || sunphase(NULL) == 1 || sunphase(NULL) == 6 || sunphase(NULL) == 7) {
        time_info.des_temp -= 1;
      }
      else {
        time_info.des_temp += 1;
      }
    }
    if (time_info.local_temp < time_info.des_temp)
    time_info.local_temp++;
    if (time_info.local_temp + 7 < time_info.des_temp)
    time_info.local_temp++;
    if (time_info.local_temp + 15 < time_info.des_temp)
    time_info.local_temp += 2;

    if (time_info.local_temp > time_info.des_temp)
    time_info.local_temp--;
    if (time_info.local_temp - 7 > time_info.des_temp)
    time_info.local_temp--;
    if (time_info.local_temp - 15 > time_info.des_temp)
    time_info.local_temp -= 2;

    if (hailing == 1) {
      if (time_info.local_density_total < 70 || time_info.local_cover_total < 80 || time_info.local_temp >= 50)
      hailing = 0;
      else if ((time_info.local_density_total < 90 || time_info.local_cover_total < 100 || time_info.local_temp > 35) && number_percent() % 2 == 0)
      hailing = 0;
      else if (number_percent() % 5 == 0)
      hailing = 0;
    }
    else {
      if (time_info.local_density_total > 90 && time_info.local_cover_total >= 100 && time_info.local_temp < 35 && number_percent() % 23 == 0) {
        hailing = 1;
        snowing = 0;
        raining = 0;
        global_message("It starts to `Chail`x.\n\r");
      }
    }
    if (snowing == 1) {
      if (time_info.local_density_total < 70 || time_info.local_cover_total < 60 || time_info.local_temp > 45 || time_info.local_temp < 20)
      snowing = 0;
      else if ((time_info.local_density_total < 90 || time_info.local_cover_total < 90 || time_info.local_temp > 40) && number_percent() % 6 == 0)
      snowing = 0;
      else if (number_percent() % 22 == 0)
      snowing = 0;
    }
    else {
      if (time_info.local_density_total > 90 && time_info.local_cover_total > 80 && time_info.local_temp < 40 && time_info.local_temp > 20 && number_percent() % 3 == 0) {
        snowing = 1;
        hailing = 0;
        raining = 0;
        global_message("It starts to `Wsnow`x.\n\r");
      }
    }
    if (raining == 1) {
      if (time_info.local_density_total < 70 || time_info.local_cover_total < 50 || time_info.local_temp < 40)
      raining = 0;
      else if ((time_info.local_density_total < 80 || time_info.local_cover_total < 80) && number_percent() % 7 == 0)
      raining = 0;
      else if (number_percent() % 18 == 0)
      raining = 0;
    }
    else {
      if (time_info.local_density_total > 80 && time_info.local_cover_total > 80 && time_info.local_temp > 45 && number_percent() % 18 == 0) {
        raining = 1;
        snowing = 0;
        hailing = 0;
        global_message("It starts to `Brain`x.\n\r");
      }
    }
    if (crisis_hurricane == 1 && is_winter() && hailing == 0) {
      snowing = 1;
      hailing = 0;
      raining = 0;
    }
    if (crisis_hurricane == 1 && is_winter() && hailing == 0) {
      snowing = 1;
      hailing = 0;
      raining = 0;
    }

    if (raining == 1 || snowing == 1 || hailing == 1)
    time_info.local_temp--;
  }

  bool full_moon(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    if (moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL) == 4)
    return TRUE;

    return FALSE;
  }
  bool new_moon(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    if (moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL) == 0)
    return TRUE;

    return FALSE;
  }

  int count_directory(void) {
    int count = 0;
    ROOM_INDEX_DATA *room;
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (IS_SET(room->room_flags, ROOM_INVISIBLE))
      continue;

      if (room->time != 3000)
      continue;

      if (room->exit[1] == NULL || !IS_SET(room->exit[1]->u1.to_room->room_flags, ROOM_PUBLIC) || room->exit[1]->u1.to_room->sector_type != SECT_STREET)
      continue;

      if (IS_SET(room->room_flags, ROOM_LOCKED))
      continue;

      if (room->sector_type == SECT_STREET)
      continue;

      if (directory_display(room, "all")) {
        count++;
      }
    }
    return count;
  }

  ROOM_INDEX_DATA *random_directory(void) {
    ROOM_INDEX_DATA *room;
    bool found = FALSE;
    int i = 0;
    for (found = FALSE; found == FALSE && i < 100;) {
      i++;
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (IS_SET(room->room_flags, ROOM_INVISIBLE))
        continue;

        if (room->time != 3000)
        continue;

        if (room->exit[1] == NULL || !IS_SET(room->exit[1]->u1.to_room->room_flags, ROOM_PUBLIC) || room->exit[1]->u1.to_room->sector_type != SECT_STREET)
        continue;

        if (IS_SET(room->room_flags, ROOM_LOCKED))
        continue;

        if (room->sector_type == SECT_STREET)
        continue;

        if (directory_display(room, "all")) {
          if (number_percent() % count_directory() == 0) {
            found = TRUE;
            return room;
          }
        }
      }
    }
    return NULL;
  }

  int player_count(void) {
    CHAR_DATA *victim;
    int count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (is_gm(victim))
      continue;

      if (IS_NPC(victim))
      continue;

      count++;
    }
    return count;
  }

  void make_fish(ROOM_INDEX_DATA *room) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    int vnum = 205000005;

    if (get_mob_index(vnum) == NULL) {
      char buf[MSL];
      sprintf(buf, "Couldn't load animal %d.\n\r", vnum);
      log_string(buf);
      return;
    }

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    char_to_room(mob, room);
    mob->hit = max_hp(mob);
    mob->x = number_range(0, room->size);
    mob->y = number_range(0, room->size);
  }
  void make_nocturnal(ROOM_INDEX_DATA *room) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int vnum;

    if (number_percent() % 6 == 0)
    vnum = 205000001;
    else if (number_percent() % 5 == 0)
    vnum = 205000002;
    else if (number_percent() % 4 == 0)
    vnum = 205000003;
    else if (number_percent() % 3 == 0)
    vnum = 205000004;
    else if (number_percent() % 2 == 0)
    vnum = 205000006;
    else
    vnum = 205000007;

    if (get_mob_index(vnum) == NULL) {
      char buf[MSL];
      sprintf(buf, "Couldn't load animal %d.\n\r", vnum);
      log_string(buf);
      return;
    }

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    char_to_room(mob, room);
    mob->hit = max_hp(mob);
    mob->x = number_range(0, room->size);
    mob->y = number_range(0, room->size);
  }
  void make_animal(ROOM_INDEX_DATA *room) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int vnum;

    if (number_percent() % 6 == 0)
    vnum = 205000000;
    else if (number_percent() % 2 == 0)
    vnum = 205000001;
    else if (number_percent() % 4 == 0)
    vnum = 205000004;
    else if (number_percent() % 3 == 0)
    vnum = 205000008;
    else if (number_percent() % 2 == 0)
    vnum = 205000009;
    else
    vnum = 205000010;
    if (get_mob_index(vnum) == NULL) {
      char buf[MSL];
      sprintf(buf, "Couldn't load animal %d.\n\r", vnum);
      log_string(buf);
      return;
    }

    if (room == NULL || room->size < 2)
    return;

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    char_to_room(mob, room);
    mob->hit = max_hp(mob);
    mob->x = number_range(0, room->size);
    mob->y = number_range(0, room->size);
  }

  void spawn_animal() {
    int vnum = number_range(205475475, 205550550);

    ROOM_INDEX_DATA *room;

    if (get_room_index(vnum) == NULL)
    vnum = number_range(205475475, 205550550);

    if (get_room_index(vnum) == NULL)
    return;

    room = get_room_index(vnum);

    if (is_water(room)) {
      make_fish(room);
    }
    else {
      if (sunphase(room) == 0)
      make_nocturnal(room);
      else
      make_animal(room);
    }
  }

  bool can_hear_bell(ROOM_INDEX_DATA *room) {

    if (is_underwater(room))
    return FALSE;

    if (!in_haven(room))
    return FALSE;

    if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
      if (get_dist3d(get_roomx(room), get_roomy(room), get_roomz(room), 41, 65, 2) < 75)
      return TRUE;
    }
    else {
      if (get_dist3d(get_roomx(room), get_roomy(room), get_roomz(room), 41, 65, 2) < 30)
      return TRUE;
    }

    return FALSE;
  }

  void sound_bell(CHAR_DATA *ch, int hour) {
    switch (hour) {
    case 0:
    case 12:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 1:
    case 13:
      send_to_char("The Clocktower chimes: Bong.\n\r", ch);
      break;
    case 2:
    case 14:
      send_to_char("The Clocktower chimes: Bong, Bong.\n\r", ch);
      break;
    case 3:
    case 15:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong.\n\r", ch);
      break;
    case 4:
    case 16:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 5:
    case 17:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 6:
    case 18:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 7:
    case 19:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 8:
    case 20:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 9:
    case 21:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 10:
    case 22:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    case 11:
    case 23:
      send_to_char("The Clocktower chimes: Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong, Bong.\n\r", ch);
      break;
    }
  }

  void lock_door(int south_vnum, int north_vnum, int direction_one, int direction_two) {
    ROOM_INDEX_DATA *room;
    int value;
    room = get_room_index(south_vnum);
    room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;

    value = flag_value(exit_flags, "door");
    if (!IS_SET(room->exit[direction_two]->rs_flags, value)) {
      SET_BIT(room->exit[direction_two]->rs_flags, value);
      room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;

      value = flag_value(exit_flags, "closed");
      SET_BIT(room->exit[direction_two]->rs_flags, value);
      room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;

      value = flag_value(exit_flags, "locked");
      SET_BIT(room->exit[direction_two]->rs_flags, value);
      room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;
    }
    room = get_room_index(north_vnum);
    room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;

    value = flag_value(exit_flags, "door");
    if (!IS_SET(room->exit[direction_one]->rs_flags, value)) {
      SET_BIT(room->exit[direction_one]->rs_flags, value);
      room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;

      value = flag_value(exit_flags, "closed");
      SET_BIT(room->exit[direction_one]->rs_flags, value);
      room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;

      value = flag_value(exit_flags, "locked");
      SET_BIT(room->exit[direction_one]->rs_flags, value);
      room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;
    }
  }

  void unlock_door(int south_vnum, int north_vnum, int direction_one, int direction_two) {
    ROOM_INDEX_DATA *room;
    int value;
    room = get_room_index(south_vnum);
    value = flag_value(exit_flags, "door");
    if (IS_SET(room->exit[direction_two]->rs_flags, value)) {
      REMOVE_BIT(room->exit[direction_two]->rs_flags, value);
      room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;

      value = flag_value(exit_flags, "closed");
      REMOVE_BIT(room->exit[direction_two]->rs_flags, value);
      room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;

      value = flag_value(exit_flags, "locked");
      REMOVE_BIT(room->exit[direction_two]->rs_flags, value);
      room->exit[direction_two]->exit_info = room->exit[direction_two]->rs_flags;
    }
    room = get_room_index(north_vnum);
    value = flag_value(exit_flags, "door");
    if (IS_SET(room->exit[direction_one]->rs_flags, value)) {
      REMOVE_BIT(room->exit[direction_one]->rs_flags, value);
      room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;

      value = flag_value(exit_flags, "closed");
      REMOVE_BIT(room->exit[direction_one]->rs_flags, value);
      room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;

      value = flag_value(exit_flags, "locked");
      REMOVE_BIT(room->exit[direction_one]->rs_flags, value);
      room->exit[direction_one]->exit_info = room->exit[direction_one]->rs_flags;
    }
  }

  int get_story_hour(int val) {
    int hour;

    tm *ptm;
    time_t east_time;

    east_time = val;
    ptm = gmtime(&east_time);

    hour = ptm->tm_hour;
    return hour;
  }

  int get_story_day(int val) {
    int hour;

    tm *ptm;
    time_t east_time;

    east_time = val;
    ptm = gmtime(&east_time);

    hour = ptm->tm_mday;
    return hour;
  }

  int room_attract(ROOM_INDEX_DATA *room) {
    int max = 0;
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

      if (victim->in_room == room) {
        if (get_attract(victim, NULL) > max)
        max = get_attract(victim, NULL);
      }
    }
    return max;
  }

  int blood_stats(CHAR_DATA *ch) {
    int total = total_skills(ch);
    total -= skillpointcost(ch, SKILL_WEALTH);
    total -= skillpointcost(ch, SKILL_ATTRACTIVENESS);

    total -= 18;
    /*
char buf[MSL];
sprintf(buf, "BloodStats %s, %d %d %d %d\n\r", ch->name, total, total_skills(ch), skillpointcost(ch, SKILL_WEALTH),  skillpointcost(ch, SKILL_ATTRACTIVENESS)); log_string(buf);
*/
    return total;
  }

  // 90 max for a non promty main char, 20-25 average for non promty alt, 2-5 for
  // proms. Every five minutes. 1k for a tick

  int blood_tvalue(CHAR_DATA *ch) { return 0; }

  bool can_hear_ocean(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;
    if (!in_haven(ch->in_room))
    return FALSE;
    if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    return FALSE;
    if (ch->in_room->x >= 60 && ch->in_room->x <= 200)
    return TRUE;
    if (get_dist(ch->in_room->x, ch->in_room->y, 57, 35) <= 50)
    return TRUE;
    if (get_skill(ch, SKILL_ACUTEHEAR) > 0) {
      if (ch->in_room->x >= 45 && ch->in_room->x < 200)
      return TRUE;
      if (get_dist(ch->in_room->x, ch->in_room->y, 57, 35) <= 100)
      return TRUE;
    }

    return FALSE;
  }

  bool is_downtown(CHAR_DATA *ch) {
    if (ch->in_room->y >= 44 && ch->in_room->y <= 71 && ch->in_room->x <= 65 && ch->in_room->x >= 1)
    return TRUE;

    return FALSE;
  }

  bool is_spring(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time - 14400;
    east_time -= 60;
    ptm = gmtime(&east_time);

    if (ptm->tm_mon == 2 || ptm->tm_mon == 3 || ptm->tm_mon == 4)
    return TRUE;

    return FALSE;
  }
  bool is_summer(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time - 14400;
    east_time -= 60;
    ptm = gmtime(&east_time);

    if (ptm->tm_mon == 5 || ptm->tm_mon == 6 || ptm->tm_mon == 7)
    return TRUE;

    return FALSE;
  }
  bool is_fall(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time - 14400;
    east_time -= 60;
    ptm = gmtime(&east_time);

    if (ptm->tm_mon == 8 || ptm->tm_mon == 9 || ptm->tm_mon == 10)
    return TRUE;

    return FALSE;
  }
  bool is_winter(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time - 14400;
    east_time -= 60;
    ptm = gmtime(&east_time);

    if (ptm->tm_mon == 11 || ptm->tm_mon == 0 || ptm->tm_mon == 1)
    return TRUE;

    return FALSE;
  }

  char *const ambiants[] = {
    "An owl hoots somewhere off in the distance.", "You hear a wolf howl somewhere in the distance.", "A bat flies by overhead.", "You are briefly blinded by headlights as a car passes you.", "You hear the roar of a motorcycle engine somewhere off in the distance.", "Some dry leaves blow past your feet.", "Some snow wafts past your feet.", "Lightning strikes down nearby, followed by a boom of thunder.", "Sheet lightning cracks between the clouds.", "You hear a trilling bird call.", "A shooting star briefly darts across the night sky.", "A face seems to be trying to push it's way through a nearby wall, distorting it like elastic.", "A taloned hand seems to be trying to claw through the air, twisting reality around it like rubber.", "A distortion in the air slithers past you.", "A pale will-o-the-wisp drifts past.", "A loud group of drunk college kids wander past", "A man passes you on the street.", "A woman passes you on the street.", "A family passes you on the street.", "You hear a floorboard creak somewhere nearby.", "A nearby streetlamp flickers.", "You hear a fog horn off in the distance.", "You feel a faint chill.", "You feel as if someone has departed.", "You feel alone.", "You feel eyes watching you.", "You feel like something's stalking you.", "You think you see a unicorn, but then it's gone."};


  char *const prologue_ambiants[] = {
    "`041An otherworldly growl, deep and resonant, reverberates from outside, causing the building to shake.`x", "`047A flash of sickly green light illuminates the windows, followed by an abrupt silence.`x", "`088A distant, inhuman scream pierces the quiet.`x", "`094The ground rumbles violently, knocking over glasses and plates.`x", "`045A sudden, unnatural cold sweeps through the building.`x", "`051The air outside crackles, causing the hair on everyone's arms to stand on end.`x", "`241A heavy, oppressive silence falls outside.`x", "`231There is a sudden, sharp scent of ozone.`x", "`237A low, mournful wail echoes from outside.`x", "`052The building shakes violently as a monstrous roar tears through the night.`x", "`159A blinding flash of light starkly illuminates the inside of the building.`x", "`178You hear the sound of something scrabbling against the walls.`x", "`069A strange, low hum reverberates through the building.`x", "`212The sound of distant, chaotic laughter echoes through the building.`x", "`196A sudden, intense heat wave sweeps through the building.`x", "`136You hear the faint sound of a violin playing from somewhere outside.`x", "`183A deep, resonating hum fills the air, causing the windows to rattle.`x", "`051A chilling wind howls around the building, despite there being no visible signs of a storm.`x", "`039The air outside crackles with an unseen energy, causing lights to flicker inside.`x", "`195A piercing, high-pitched screech shatters the quiet.`x", "`214An unsettling, rhythmic thumping can be heard from beneath the floor.`x", "`220The faint smell of sulfur permeates the air.`x", "`037A sudden, intense pressure change makes everyone's ears pop.`x", "`120A sickening squelching sound echoes from outside, followed by an ominous silence.`x"
  };

  bool can_ambiant(CHAR_DATA *ch, int message) {
    if (ch->in_room == NULL)
    return FALSE;

    if (in_fantasy(ch) != NULL)
    return FALSE;

    if (message == 0) {
      if (sunphase(ch->in_room) == 0)
      return TRUE;
    }
    if (message == 1) {
      if (sunphase(ch->in_room) == 0 && !is_downtown(ch))
      return TRUE;
    }
    if (message == 2) {
      if (sunphase(ch->in_room) == 0 && !is_downtown(ch) && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
    }
    if (message == 3) {
      if (ch->in_room->sector_type == SECT_STREET && is_dark_outside() && is_downtown(ch))
      return TRUE;
    }
    if (message == 4) {
      if (ch->in_room->sector_type == SECT_STREET && is_dark_outside())
      return TRUE;
    }
    if (message == 5) {
      if (is_fall() && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
    }
    if (message == 6) {
      if (is_winter() && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
    }
    if (message == 7 || message == 8) {
      if (is_raining(ch->in_room) || is_hailing(ch->in_room))
      return TRUE;
    }
    if (message == 9) {
      if (is_spring() && !is_downtown(ch))
      return TRUE;
    }
    if (message == 10) {
      if (cloud_cover(ch->in_room) < 50 && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && sunphase(ch->in_room) == 0 && number_percent() % 5 == 0)
      return TRUE;
    }
    if (message == 11 || message == 12 || message == 13) {
      if (IS_FLAG(ch->act, PLR_SHROUD))
      return TRUE;
    }
    if (message == 14) {
      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
    }
    if (message == 15) {
      if (is_downtown(ch) && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && (sunphase(ch->in_room) == 0 || sunphase(ch->in_room) > 5))
      return TRUE;
    }
    if (message == 16 || message == 17 || message == 18) {
      if (is_downtown(ch) && sunphase(ch->in_room) != 0 && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
    }
    if (message == 19) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      return TRUE;
    }
    if (message == 20) {
      if (ch->in_room->sector_type == SECT_STREET && sunphase(ch->in_room) == 0)
      return TRUE;
    }
    if (message == 21) {
      if (can_hear_ocean(ch))
      return TRUE;
    }
    if (message == 22) {
      if (temperature(ch->in_room) <= 55 && number_percent() % 2 == 0)
      return TRUE;
    }
    if (message == 23 || message == 24) {
      if (pc_pop(ch->in_room) < 2 && number_percent() % 10 == 0)
      return TRUE;
    }
    if (message == 25 || message == 26 || message == 27) { // unicorns
      if (ch->pcdata->virginity_lost == 0 && (get_skill(ch, SKILL_VIRGIN) > 0)) {
        if ((ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_PARK) && sunphase(ch->in_room) == 0)
        return TRUE;
      }
    }

    return FALSE;
  }

#define MAX_AMBIANT 27

  void focustick(CHAR_DATA *ch) {
    if (ch->pcdata->account == NULL)
    return;

    if (ch->pcdata->account->focusedcount <= 0) {
      for (int i = 0; i < 25; i++) {
        if (!str_cmp(ch->pcdata->account->characters[i], ch->name)) {
          ch->pcdata->account->focusedchar = i;
          ch->pcdata->account->focusedcount = 1;
        }
      }
    }
    else {
      int count = ch->pcdata->account->focusedcount + 1;
      if (!str_cmp(
            ch->pcdata->account->characters[ch->pcdata->account->focusedchar], ch->name))
      ch->pcdata->account->focusedcount = UMIN(120, count);
      else if (!IS_FLAG(ch->act, PLR_GUEST) && !IS_FLAG(ch->act, PLR_GM) && !is_gm(ch))
      ch->pcdata->account->focusedcount -= 18;
    }
  }

  void auto_arrest(CHAR_DATA *ch) {
    if (guestmonster(ch) || higher_power(ch))
    return;
    if (is_gm(ch))
    return;
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_INDEX_SHERIFFCELL));
    if (ch->shape != SHAPE_HUMAN && !is_neutralized(ch))
    human_transformation(ch);
  }

  bool in_cell(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (locked_room(ch->in_room, ch))
    return TRUE;

    return FALSE;
  }

  int deputy_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
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
      if (victim->in_room != room)
      continue;
      if (victim->race == RACE_DEPUTY)
      pop++;
    }
    return pop;
  }

  bool has_persecution_imprint(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf[MSL];
    sprintf(buf, "see %s hurt", PERS(victim, ch));
    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->pcdata->imprint[i], buf))
      return TRUE;
    }
    sprintf(buf, "see %s suffer", PERS(victim, ch));
    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->pcdata->imprint[i], buf))
      return TRUE;
    }
    sprintf(buf, "see %s be unhappy", PERS(victim, ch));
    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->pcdata->imprint[i], buf))
      return TRUE;
    }
    sprintf(buf, "see %s attacked", PERS(victim, ch));
    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->pcdata->imprint[i], buf))
      return TRUE;
    }
    sprintf(buf, "see %s cry", PERS(victim, ch));
    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->pcdata->imprint[i], buf))
      return TRUE;
    }
    return FALSE;
  }
  void persecute_char(CHAR_DATA *ch, CHAR_DATA *victim) {

    wiznet("$N persecuting.", ch, NULL, WIZ_LINKS, 0, 0);

    if (mindwarded(ch))
    return;

    char buf[MSL];
    switch (number_percent() % 5) {
    case 0:
      sprintf(buf, "see %s hurt", PERS(victim, ch));
      break;
    case 1:
      sprintf(buf, "see %s suffer", PERS(victim, ch));
      break;
    case 2:
      sprintf(buf, "see %s be unhappy", PERS(victim, ch));
      break;
    case 3:
      sprintf(buf, "see %s attacked", PERS(victim, ch));
      break;
    default:
      sprintf(buf, "see %s cry", PERS(victim, ch));
      break;
    }
    printf_to_char(ch, "You need to %s\n\r", buf);
    auto_imprint(ch, buf, IMPRINT_INSTRUCTION);
  }

  void seed_battle(FACTION_TYPE *fac) {
    if (fac == NULL)
    return;
    if (safe_strlen(fac->soldier_name) < 2)
    return;

    LOCATION_TYPE *terr;
    terr = territory_by_number(fac->battleterritory);
    if (terr == NULL)
    return;
    int val = 0;

    if (fac->parent == FACTION_HAND)
    val = terr->hand;
    else if (fac->parent == FACTION_ORDER)
    val = terr->order;
    else if (fac->parent == FACTION_TEMPLE)
    val = terr->temple;
    else
    val = 15 + (terr->other_amount[0] * 2);

    for (int i = 0; i < 5; i++) {
      if (val > number_percent()) {
        fac->soldiers_deploying++;
        val -= 10;
      }
    }
  }

  int unbound_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
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

      if (IS_FLAG(victim->act, PLR_BOUND))
      continue;
      if (IS_FLAG(victim->act, PLR_BOUNDFEET))
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  bool throughroad(ROOM_INDEX_DATA *room) {
    if (room->exit[DIR_NORTH] != NULL && room->exit[DIR_NORTH]->u1.to_room != NULL && room->exit[DIR_SOUTH] != NULL && room->exit[DIR_SOUTH]->u1.to_room != NULL && room->exit[DIR_NORTH]->u1.to_room->sector_type == SECT_STREET && room->exit[DIR_SOUTH]->u1.to_room->sector_type == SECT_STREET && (room->exit[DIR_WEST] == NULL || room->exit[DIR_WEST]->u1.to_room == NULL || room->exit[DIR_WEST]->u1.to_room->sector_type != SECT_STREET) && (room->exit[DIR_EAST] == NULL || room->exit[DIR_EAST]->u1.to_room == NULL || room->exit[DIR_EAST]->u1.to_room->sector_type != SECT_STREET))
    return TRUE;

    if (room->exit[DIR_EAST] != NULL && room->exit[DIR_EAST]->u1.to_room != NULL && room->exit[DIR_WEST] != NULL && room->exit[DIR_WEST]->u1.to_room != NULL && room->exit[DIR_EAST]->u1.to_room->sector_type == SECT_STREET && room->exit[DIR_EAST]->u1.to_room->sector_type == SECT_STREET && (room->exit[DIR_SOUTH] == NULL || room->exit[DIR_SOUTH]->u1.to_room == NULL || room->exit[DIR_SOUTH]->u1.to_room->sector_type != SECT_STREET) && (room->exit[DIR_NORTH] == NULL || room->exit[DIR_NORTH]->u1.to_room == NULL || room->exit[DIR_NORTH]->u1.to_room->sector_type != SECT_STREET))
    return TRUE;
    return FALSE;
  }

  void forest_update(AREA_DATA *area) {

    int sun = sunphase(get_room_index(area->min_vnum));
    if (sun == 0) {
      int countmissing = 0;
      ROOM_INDEX_DATA *room;
      for (int i = area->min_vnum; i < area->max_vnum; i++) {
        if (i % 7 != time_info.hour % 7)
        continue;

        if ((room = get_room_index(i)) != NULL) {
          if (room->sector_type == SECT_FOREST) {
            room->encroachment = 0;
            if (area->vnum >= 19 && area->vnum <= 25 && number_percent() % 11 == 0) {
              EXTRA_DESCR_DATA *ed;
              EXTRA_DESCR_DATA *ped = NULL;
              for (ed = room->extra_descr; ed; ed = ed->next) {
                if (ed->next != NULL && is_name("temporary", ed->next->keyword))
                ped = ed;
                if (is_name("temporary", ed->keyword))
                break;
              }

              if (ed) {
                if (!ped)
                room->extra_descr = ed->next;
                else
                ped->next = ed->next;

                free_extra_descr(ed);
              }
            }
          }
          else {
            int mod = 5;
            if (room->area->vnum >= 22 && room->area->vnum <= 25) {
              mod = 3;
              if (room->x >= room->area->minx + 24 && room->x <= room->area->minx + 26 && room->y >= room->area->miny + 2 && room->y <= room->area->miny + 26)
              mod = 0;
            }
            if (room->sector_type == SECT_PARK && prop_from_room(room) == NULL)
            room->encroachment += 50 * mod;
            else if (room->sector_type == SECT_STREET && number_percent() % 2 == 0 && !throughroad(room)) {
              if (IS_SET(room->room_flags, ROOM_DIRTROAD))
              room->encroachment += 3 * mod;
              else
              room->encroachment += 1 * mod;
            }
          }
          if (room->encroachment >= 1000) {
            reclaim_room(room);
            room->sector_type = SECT_FOREST;
            free_string(room->description);
            room->description = str_dup("");
            free_string(room->name);
            room->name = str_dup("The Forest");
          }
          countmissing = 0;
        }
        else
        countmissing++;

        if (countmissing >= 5)
        return;
      }
    }
  }

  int secret_recover_points(CHAR_DATA *ch) {
    if (get_tier(ch) > 2)
    return 0;
    if (ch->skills[SKILL_MENTALDISCIPLINE] > 0)
    return 0;
    if (attract_lifeforce(ch) <= 90)
    return 0;
    if (ch->pcdata->lastnotalone < current_time - (3600 * 24 * 7))
    return 0;

    int val = 100;
    val = val * solidity(ch) / 100;
    if (alt_count(ch) == 1)
    val = val * 3 / 2;
    else
    val = val / 2;

    if (!is_super(ch))
    val = val * 3 / 2;
    if (college_student(ch, TRUE) || clinic_patient(ch))
    val /= 2;
    if (ch->race == RACE_FACULTY)
    val /= 5;
    else if (college_staff(ch, TRUE) || clinic_staff(ch, TRUE))
    val /= 2;
    if (ch->race == RACE_LOCAL || ch->race == RACE_IMPORTANT || ch->race == RACE_DEPUTY || ch->race == RACE_PILLAR || ch->race == RACE_CELEBRITY)
    val = val * 3 / 2;
    if (attract_lifeforce(ch) <= 100)
    val = val / 2;

    return val;
  }

  void disc_daily_check(CHAR_DATA *ch)
  {
    char logs[MSL];
    for (int i = 0; i < DIS_USED; i++) {
      if (ch->disciplines[discipline_table[i].vnum] > 0) {
        ch->disciplines[discipline_table[i].vnum]--;
        if(can_train_disc(ch, discipline_table[i].vnum))
        ch->disciplines[discipline_table[i].vnum]++;
        else
        {
          sprintf(logs, "Illegal disciplines %s, %s", ch->name, discipline_table[i].name);
          log_string(logs);

        }
      }
    }
  }


  void once_per_day(CHAR_DATA *ch) {
    int chance;

    if (IS_NPC(ch))
    return;

    char lbuf[MSL];
    sprintf(lbuf, "ONCE_PER_DAY %s", ch->name);
    log_string(lbuf);

    if (higher_power(ch)) {
      security_wipeout(ch->name);
    }

    if(ch->wounds == 1 && ch->heal_timer > 50000)
    ch->heal_timer = 50000;

    if(ch->pcdata->influencer_bank < 2500)
    {
      ch->pcdata->influencer_bank += 500;
    }

    if (ch->pcdata->attract[ATTRACT_PROM] > 15)
    ch->pcdata->attract[ATTRACT_PROM] -= 15;

    FACTION_TYPE * ocult = clan_lookup(ch->fcult);
    if(ocult != NULL)
    {
      if(ocult->college == 1 && !str_cmp(ocult->leader, ch->name) && !college_student(ch, TRUE))
      ocult->college = 0;
    }
    FACTION_TYPE * osect = clan_lookup(ch->fsect);
    if(osect != NULL)
    {
      if(osect->college == 1 && !str_cmp(osect->leader, ch->name) && !college_student(ch, TRUE))
      osect->college = 0;
    }


    if(ch->pcdata->account->sparring_xp < 0)
    ch->pcdata->account->sparring_xp = 0;

    ch->pcdata->account->sparring_xp = UMIN(50000, ch->pcdata->account->sparring_xp+2000);

    if(ch->pcdata->brand_timeout > 0 && ch->pcdata->brand_timeout < current_time)
    {
      ch->pcdata->brand_timeout = 0;
      send_to_char("Your brand fades.\n\r", ch);
      ch->pcdata->branddate = 0;
      free_string(ch->pcdata->brander);
      ch->pcdata->brander = str_dup("");
      free_string(ch->pcdata->brandstring);
      ch->pcdata->brandstring = str_dup("");
      ch->pcdata->brandlocation = 0;
    }
    if(ch->pcdata->enthrall_timeout > 0 && ch->pcdata->enthrall_timeout < current_time)
    {
      breakcontrol(ch->pcdata->enthralled, 1);
      breakcontrol(ch->name, 3);
      ch->pcdata->enthrall_timeout = 0;
      send_to_char("Your enthrallment ends.\n\r", ch);
    }
    if(ch->pcdata->enrapture_timeout > 0 && ch->pcdata->enrapture_timeout < current_time)
    {
      breakcontrol(ch->pcdata->enraptured, 2);
      breakcontrol(ch->name, 4);
      ch->pcdata->enrapture_timeout = 0;
      send_to_char("Your enrapturement ends.\n\r", ch);
    }
    if(ch->pcdata->fixation_timeout > 0 && ch->pcdata->fixation_timeout < current_time)
    {
      free_string(ch->pcdata->fixation_name);
      ch->pcdata->fixation_name = str_dup("");
      ch->pcdata->fixation_timeout = 0;
    }
    if(event_cleanse == 0)
    disc_daily_check(ch);

    if(ch->played/3600 > 25 && is_ffamily(ch) && strlen(ch->pcdata->ff_secret) > 2)
    {
      bool sent = FALSE;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;
          if (to->in_room == NULL || ch->in_room == NULL)
          continue;
          if(to != ch && is_ffamily(to) && number_percent() % 5 == 0)
          {
            printf_to_char(to, "You recollect a secret about %s %s.\n%s\n\r", ch->name, ch->pcdata->last_name, ch->pcdata->ff_secret);
            sent = TRUE;
          }
        }
        if(sent == TRUE && number_percent() % 3 == 0)
        {
          free_string(ch->pcdata->ff_secret);
          ch->pcdata->ff_secret = str_dup("");
        }
      }


    }

    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOKILL))
    ch->pcdata->next_monster = current_time + 3600 * 24 * 7 * 4;

    if (ch->pcdata->last_intel == 0)
    ch->pcdata->last_intel = current_time;
    if (ch->pcdata->last_intel < current_time - (3600 * 24 * 45)) {
      ch->pcdata->intel = 0;
      ch->pcdata->last_intel = current_time;
      send_to_char("Your intel becomes out of date.\n\r", ch);
    }

    for (int i = 0; i < 50; i++) {
      if (safe_strlen(ch->pcdata->ignored_characters[i]) > 1 && !character_exists(ch->pcdata->ignored_characters[i])) {
        free_string(ch->pcdata->ignored_characters[i]);
        ch->pcdata->ignored_characters[i] = str_dup("");
      }
      if (safe_strlen(ch->pcdata->nowhere_characters[i]) > 1 && !character_exists(ch->pcdata->nowhere_characters[i])) {
        free_string(ch->pcdata->nowhere_characters[i]);
        ch->pcdata->nowhere_characters[i] = str_dup("");
      }
    }

    if (!is_gm(ch) && !IS_FLAG(ch->act, PLR_STASIS) && !IS_FLAG(ch->act, PLR_DEAD))
    add_to_weekly_characters(ch);

    clinic_charge(ch);

    int srp = secret_recover_points(ch);
    if (srp > 0) {
      ch->pcdata->secret_recover += srp;
      if (ch->pcdata->secret_recover > srp * 10)
      ch->pcdata->secret_recover = srp * 10;
    }
    if (ch->pcdata->event_cooldown > 0)
    ch->pcdata->event_cooldown -= 4;

    if (ch->faction != 0 && clan_lookup(ch->faction) != NULL) {
      char logs[MSL];
      sprintf(logs, "FACBELIEF: %s in %s: %d", ch->name, clan_lookup(ch->faction)->name, belief_match(ch, clan_lookup(ch->faction)));
      log_string(logs);
    }
    if (ch->factiontwo != 0 && clan_lookup(ch->factiontwo) != NULL) {
      char logs[MSL];
      sprintf(logs, "FACBELIEF: %s in %s: %d", ch->name, clan_lookup(ch->factiontwo)->name, belief_match(ch, clan_lookup(ch->factiontwo)));
      log_string(logs);
    }
    ch->pcdata->recent_exp = UMAX(0, ch->pcdata->recent_exp - 1000);

    if (is_gm(ch))
    return;
    if (ch->race != RACE_LOCAL && ch->race != RACE_IMPORTANT && ch->race != RACE_CELEBRITY && ch->race != RACE_PILLAR && ch->race != RACE_FACULTY && ch->race != RACE_DEPUTY && !college_student(ch, FALSE) && !clinic_patient(ch) && ch->pcdata->lastnotalone > current_time - (3600 * 24) && !IS_FLAG(ch->act, PLR_STASIS)) {
      ch->pcdata->secret_days++;
      FACTION_TYPE *fac = clan_lookup(ch->faction);
      if (fac != NULL) {
        if (fac->axes[AXES_CORRUPT] == AXES_NEUTRAL)
        ch->pcdata->secret_days += 3;
        if (fac->axes[AXES_CORRUPT] >= AXES_NEARRIGHT) {
          if (is_werewolf(ch) && ch->pcdata->lastshift > current_time - (3600 * 24 * 7 * 10))
          ch->pcdata->secret_days += 3;
          if (is_vampire(ch) && !animal_feeder(ch))
          ch->pcdata->secret_days += 3;
        }

      }
      else if (number_percent() % 2 == 0)
      ch->pcdata->secret_days--;

    }
    else if (ch->pcdata->secret_days > 0)
    ch->pcdata->secret_days--;

    if (ch->pcdata->spent_resources > 0) {
      ch->pcdata->spent_resources -= UMAX(10, ch->pcdata->spent_resources / 10);
      ch->pcdata->spent_resources = UMAX(0, ch->pcdata->spent_resources);
    }
    if (ch->pcdata->spent_resources < 0) {
      ch->pcdata->spent_resources += UMAX(5, ch->pcdata->spent_resources / 20);
      ch->pcdata->spent_resources = UMIN(0, ch->pcdata->spent_resources);
    }
    if (ch->pcdata->heroic > 0)
    ch->pcdata->heroic--;
    if (get_tier(ch) >= 3)
    ch->pcdata->heroic = 0;

    if (event_cleanse == 0) {
      statcheck(ch);
      disccheck(ch);
    }

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->destiny_in_one[i] > 0) {
        if (!active_character(ch->pcdata->destiny_in_with_one[i]))
        ch->pcdata->destiny_in_one[i] = 0;
        CHAR_DATA *toch = get_char_world_pc(ch->pcdata->destiny_in_with_one[i]);
        if (toch != NULL) {
          bool dfound = FALSE;
          for (int j = 0; j < 10; j++) {
            if (toch->pcdata->destiny_in_two[j] ==
                ch->pcdata->destiny_in_one[i] && !str_cmp(ch->name, toch->pcdata->destiny_in_with_two[j]))
            dfound = TRUE;
          }
          if (dfound == FALSE)
          ch->pcdata->destiny_in_one[i] = 0;
        }
      }
      if (ch->pcdata->destiny_in_two[i] > 0) {
        if (!active_character(ch->pcdata->destiny_in_with_two[i]))
        ch->pcdata->destiny_in_two[i] = 0;
        CHAR_DATA *toch = get_char_world_pc(ch->pcdata->destiny_in_with_two[i]);
        if (toch != NULL) {
          bool dfound = FALSE;
          for (int j = 0; j < 10; j++) {
            if (toch->pcdata->destiny_in_one[j] ==
                ch->pcdata->destiny_in_two[i] && !str_cmp(ch->name, toch->pcdata->destiny_in_with_one[j]))
            dfound = TRUE;
          }
          if (dfound == FALSE)
          ch->pcdata->destiny_in_two[i] = 0;
        }
      }
    }

    if (get_tier(ch) >= 3 && is_super(ch) && !higher_power(ch) && ch->pcdata->ill_time < current_time) {
      int mod = 0;
      int mult = 100;
      if (ch->pcdata->account != NULL && ch->pcdata->account->villain_mod != 0) {
        if (ch->pcdata->account->villain_mod < 0)
        mult += ch->pcdata->account->villain_mod * 5;
        else
        mult += ch->pcdata->account->villain_mod * 10;
      }
      mod = ch->pcdata->week_tracker[TRACK_VILLAIN] +
      sqrt(ch->pcdata->life_tracker[TRACK_VILLAIN]);
      mod = mod * mult / 100;
      if (mod < 60 && ch->pcdata->lastnotalone > current_time - (3600 * 24 * 2)) {
        ch->pcdata->monster_fed -= 10;
        if (ch->skills[SKILL_MENTALDISCIPLINE] > 0)
        ch->pcdata->monster_fed += 5;
      }
    }
    if (under_opression(ch) && in_haven(ch->in_room)) {
      if (ch->in_room != NULL && ch->in_room->sector_type == SECT_STREET && number_percent() % 2 == 0) {
        npc_rundown(ch);
      }
      else if (number_percent() % 2 == 0) {
        act("Some deputies come and bring you in for questioning.", ch, NULL, NULL, TO_CHAR);
        act("Some deputuies come and take $n in for questioning.", ch, NULL, NULL, TO_ROOM);
        dact("Some deputies come and take $n in for questioning.", ch, NULL, NULL, DISTANCE_MEDIUM);
        auto_arrest(ch);
        ch->pcdata->public_alarm = -60;
      }
      else {
        ch->money = 0;
        send_to_char("You suddenly realize your wallet is missing.\n\r", ch);
      }
    }

    if (ch->pcdata->prison_emotes > 0 && !is_prisoner(ch)) {
      int pem = UMAX(1, ch->pcdata->prison_emotes / 10);
      ch->pcdata->prison_emotes -= pem;
    }

    if (ch->pcdata->account != NULL) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO))
      REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO);
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANE))
      REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANE);
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDHUNT))
      REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDHUNT);
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDWAR))
      REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDWAR);
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDSPIRIT))
      REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDSPIRIT);
    }

    if (ch->fcore != 0 && ch->esteem_faction > 0) {
      int minoffline = (60 * 12);
      int pen = UMIN(20000, minoffline);
      if (get_age(ch) < 20)
      pen = pen * 2;
      if (get_age(ch) < 25)
      pen = pen * 2;
      pen /= 2;
      if (!in_haven(ch->in_room)) {
        give_respect(ch, -1 * pen / 15, "Logon Flat2", ch->fcore);
        give_respect(ch, ch->esteem_faction * -1 / 100, "Logon Percent2", ch->fcore);
      }
      else {
        give_respect(ch, -1 * pen / 25, "Logon Flat2", ch->fcore);
        give_respect(ch, ch->esteem_faction * -1 / 200, "Logon Percent2", ch->fcore);
      }
    }
    if (ch->fsect != 0 && ch->esteem_sect > 0) {
      int minoffline = (60 * 12);
      int pen = UMIN(20000, minoffline);
      if (get_age(ch) < 20)
      pen = pen * 2;
      if (get_age(ch) < 25)
      pen = pen * 2;
      pen /= 2;
      if (!in_haven(ch->in_room)) {
        give_respect(ch, -1 * pen / 15, "Logon Flat2", ch->fsect);
        give_respect(ch, ch->esteem_sect * -1 / 100, "Logon Percent2", ch->fsect);
      }
      else {
        give_respect(ch, -1 * pen / 25, "Logon Flat2", ch->fsect);
        give_respect(ch, ch->esteem_sect * -1 / 200, "Logon Percent2", ch->fsect);
      }
    }

    if (ch->fcult != 0 && ch->esteem_cult > 0) {
      int minoffline = (60 * 12);
      int pen = UMIN(20000, minoffline);
      if (get_age(ch) < 20)
      pen = pen * 2;
      if (get_age(ch) < 25)
      pen = pen * 2;
      pen /= 2;
      if (!in_haven(ch->in_room)) {
        give_respect(ch, -1 * pen / 15, "Logon Flat2", ch->fcult);
        give_respect(ch, ch->esteem_cult * -1 / 100, "Logon Percent2", ch->fcult);
      }
      else {
        give_respect(ch, -1 * pen / 25, "Logon Flat2", ch->fcult);
        give_respect(ch, ch->esteem_cult * -1 / 200, "Logon Percent2", ch->fcult);
      }
    }




    do_function(ch, &do_prompt, "starter");
    if (get_skill(ch, SKILL_HYPERREGEN) > 0 && safe_strlen(ch->pcdata->maim) > 2 && number_percent() % 13 == 0) {
      free_string(ch->pcdata->maim);
      ch->pcdata->maim = str_dup("");
      send_to_char("Your body heals.\n\r", ch);
    }
    ch->pcdata->emotes[EMOTE_PACTANTAG] -=
    UMAX(ch->pcdata->emotes[EMOTE_PACTANTAG] / 50, 1);
    ch->pcdata->emotes[EMOTE_PACTANTAG] =
    UMAX(ch->pcdata->emotes[EMOTE_PACTANTAG], 0);

    if (IS_FLAG(ch->comm, COMM_RACIAL))
    REMOVE_FLAG(ch->comm, COMM_RACIAL);
    if (IS_FLAG(ch->comm, COMM_DEPUTY))
    REMOVE_FLAG(ch->comm, COMM_DEPUTY);

    if (ch->pcdata->account->socialcooldown > current_time + (3600 * 24))
    ch->pcdata->account->socialcooldown = current_time;

    contact_logon(ch);

    if (ch->faction != 0)
    ch->pcdata->faction_influence += 100 * get_rank(ch, ch->faction);
    if (is_leader(ch, ch->faction))
    ch->pcdata->faction_influence += 200 * get_rank(ch, ch->faction);

    ch->pcdata->faction_influence = UMIN(ch->pcdata->faction_influence, 10000);
    ch->pcdata->influence = UMIN(ch->pcdata->influence, 100000);
    ch->pcdata->scheme_influence = UMIN(ch->pcdata->scheme_influence, 100000);
    ch->pcdata->super_influence = UMIN(ch->pcdata->super_influence, 50000);
    if ((!IS_FLAG(ch->act, PLR_BOUND) && !trapped_room(ch->in_room, ch) && !IS_FLAG(ch->act, PLR_BOUNDFEET)) || !in_haven(ch->in_room)) {
      if (ch->pcdata->prison_mult <= 1) {
        ch->pcdata->prison_mult = 0;
        ch->pcdata->prison_care = 0;
      }
      else
      ch->pcdata->prison_mult /= 2;
    }

    if (base_lifeforce(ch) >= 9800) // && get_tier(ch) >= 2) //Extended to all
    // tiers - Discordance 6-18-2016
    {
      for (int i = 0; i < 25; i++) {
        if (safe_strlen(ch->pcdata->repressions[i]) > 2) {
          if (get_tier(ch) >= 2)
          chance = 3;
          else
          chance = 2;

          if (number_percent() % chance == 0) {
            printf_to_char(ch, "You remember %s.\n\r", ch->pcdata->repressions[i]);
            free_string(ch->pcdata->repressions[i]);
            ch->pcdata->repressions[i] = str_dup("");
          }
        }
      }
      if (safe_strlen(ch->pcdata->brainwash_loyalty) > 2 && ch->pcdata->mindmessed < current_time) {
        if (get_tier(ch) >= 2)
        chance = 11;
        else
        chance = 6;

        if (number_percent() % chance == 0) {
          printf_to_char(ch, "You are no longer loyaly to %s.\n\r", ch->pcdata->brainwash_loyalty);
          free_string(ch->pcdata->brainwash_loyalty);
          ch->pcdata->brainwash_loyalty = str_dup("");
        }
      }
      if (safe_strlen(ch->pcdata->brainwash_reidentity) > 2 && ch->pcdata->mindmessed < current_time) {
        if (get_tier(ch) >= 2)
        chance = 29;
        else
        chance = 15;

        if (number_percent() % chance == 0) {
          printf_to_char(ch, "You no longer believe you are %s.\n\r", ch->pcdata->brainwash_reidentity);
          free_string(ch->pcdata->brainwash_reidentity);
          ch->pcdata->brainwash_reidentity = str_dup("");
        }
      }
      if (IS_FLAG(ch->comm, COMM_FORCEDPACIFIST) && ch->pcdata->mindmessed < current_time) {
        if (get_tier(ch) >= 2)
        chance = 14;
        else
        chance = 7;

        if (number_percent() % chance == 0) {
          send_to_char("You are no longer a pacifist.\n\r", ch);
          REMOVE_FLAG(ch->comm, COMM_FORCEDPACIFIST);
        }
      }
      if (IS_FLAG(ch->comm, COMM_CLEANSED) && ch->pcdata->mindmessed < current_time) {
        if (get_tier(ch) >= 2)
        chance = 18;
        else
        chance = 9;

        if (number_percent() % chance == 0) {
          send_to_char("You recall all your dark memories.\n\r", ch);
          REMOVE_FLAG(ch->comm, COMM_CLEANSED);
        }
      }
    }
  }

  void regenerate_lifeforce(CHAR_DATA *ch) {
    if (ch->in_room == NULL || ch->in_room->vnum < 100)
    return;
    if (IS_NPC(ch))
    return;
    if (is_gm(ch))
    return;

    once_per_day(ch);

    if (ch->lf_sused > 0)
    ch->lf_sused = UMAX(0, ch->lf_sused - 20);

    if (is_vampire(ch)) {
      int mod = 10000;
      //        int offmod = 5000;
      if (animal_feeder(ch)) {
        mod = 20000;
        //           offmod = 25000;
      }
      if (ch->lf_used > 0) {
        int cost = ch->lf_used;
        cost = (int)(sqrt(cost));
        cost *= 4;
        cost = UMAX(cost, 350);
        if (IS_FLAG(ch->act, PLR_VILLAIN))
        cost *= 3;
        else if (cardinal(ch))
        cost *= 3;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        if (safe_strlen(ch->pcdata->maintained_target) > 1 && str_cmp(ch->pcdata->maintained_target, "casting"))
        cost /= 2;
        cost = UMIN(cost, ch->lf_used);
        ch->lf_used -= cost;
      }
      else if (ch->lf_used < 0) {
        int cost = ch->lf_used * -1;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMAX(cost, 150);
        cost = UMIN(cost, ch->lf_used * -1);
        ch->lf_used += cost;
      }

      if (ch->lf_taken - mod > 0) {
        int cost = ch->lf_taken - mod;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMAX(cost, 250);
        if (ch->pcdata->heroic >= 50 && ch->lf_taken > 0) {
          cost *= 5;
          //        ch->pcdata->heroic -= 50;
        }
        else if (ch->pcdata->heroic >= 10 && ch->lf_taken > 0) {
          cost *= 2;
          //       ch->pcdata->heroic -= 10;
        }
        cost = UMIN(cost, ch->lf_taken - mod);
        ch->lf_taken -= cost;
      }
      else if (ch->lf_taken - mod < 0) {
        int orig = ch->lf_taken;
        int cost = ch->lf_taken * -1;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMIN(cost, ch->lf_taken * -1);
        if (ch->lf_taken >= 0)
        cost = UMAX(cost, 25);
        else
        cost = UMAX(cost, 50);
        ch->lf_taken += cost;
        char buf[MSL];
        sprintf(buf, "VAMPIRE %s, degen from %d to %d.", ch->name, orig, ch->lf_taken);
        log_string(buf);
      }
    }
    else {
      if (ch->lf_used > 0) {
        int cost = ch->lf_used;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMAX(cost, 350);
        if (IS_FLAG(ch->act, PLR_VILLAIN))
        cost *= 3;
        else if (cardinal(ch))
        cost *= 3;
        if (safe_strlen(ch->pcdata->maintained_target) > 1 && str_cmp(ch->pcdata->maintained_target, "casting"))
        cost /= 2;
        cost = UMIN(cost, ch->lf_used);
        ch->lf_used -= cost;
      }
      else if (ch->lf_used < 0) {
        int cost = ch->lf_used * -1;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMAX(cost, 150);
        cost = UMIN(cost, ch->lf_used * -1);
        ch->lf_used += cost;
      }

      if (ch->lf_taken > 0) {
        int cost = ch->lf_taken;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMAX(cost, 350);
        if (ch->pcdata->heroic >= 50 && ch->lf_taken > 0) {
          cost *= 5;
          //        ch->pcdata->heroic -= 50;
        }
        else if (ch->pcdata->heroic >= 10 && ch->lf_taken > 0) {
          cost *= 2;
          //       ch->pcdata->heroic -= 10;
        }
        cost = UMIN(cost, ch->lf_taken);
        ch->lf_taken -= cost;
      }
      else if (ch->lf_taken < 0) {
        int cost = ch->lf_taken * -1;
        cost = (int)(sqrt(cost));
        cost *= 4;
        if (alt_count(ch) == 1)
        cost = cost * 3 / 2;
        cost = UMAX(cost, 250);
        cost = UMIN(cost, ch->lf_taken * -1);
        ch->lf_taken += cost;
      }
    }

    int resting = 10000;
    if (ch->lifeforce > 10000) {
      int cost = ch->lifeforce - resting;
      cost = (int)(sqrt(cost));
      cost *= 6;
      if (alt_count(ch) == 1)
      cost = cost * 3 / 2;
      cost = UMAX(cost, 250);
      if (is_undead(ch))
      cost /= 2;
      ch->lifeforce = UMAX(resting, ch->lifeforce - cost);
    }
    else if (ch->lifeforce < resting && ch->pcdata->ill_time < current_time) {
      int val = 250;
      if (IS_FLAG(ch->act, PLR_VILLAIN))
      val *= 3;
      else if (cardinal(ch))
      val *= 3;
      if (alt_count(ch) == 1)
      val = val * 3 / 2;
      ch->lifeforce = UMIN(resting, ch->lifeforce + val);
    }
    ch->pcdata->lifeforcecooldown += (3600 * 23);
    ch->pcdata->lifeforcecooldown =
    UMAX(ch->pcdata->lifeforcecooldown, current_time);

    ch->pcdata->lftotal = UMAX(0, ch->pcdata->lftotal);
    ch->pcdata->lfcount = UMAX(0, ch->pcdata->lfcount);
    int workinglf = get_lifeforce(ch, FALSE, NULL);
    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (current_time < (*it)->active_time || current_time > (*it)->deactive_time)
      continue;

      if ((*it)->type == EVENT_HEIST || (*it)->typetwo == EVENT_HEIST) {
        if (!str_cmp(ch->name, (*it)->author))
        workinglf = workinglf * 5;
        else
        workinglf = workinglf * 4 / 5;
      }
      if ((*it)->type == EVENT_OPERATION || (*it)->typetwo == EVENT_OPERATION) {
        if (char_in_alliance_with(ch, (*it)->faction))
        workinglf *= 2;
        else
        workinglf /= 2;
      }
      if ((*it)->type == EVENT_CATASTROPHE || (*it)->typetwo == EVENT_CATASTROPHE)
      workinglf = workinglf * 3 / 5;
    }
    ch->pcdata->ill_count -= (75 + ch->pcdata->heroic * 3);

    if (ch->pcdata->ill_count >= 4000) {
      ch->pcdata->ill_time = current_time + (3600 * 24 * 45);
      ch->pcdata->ill_count -= 500;
    }
    if (get_tier(ch) >= 3 && is_super(ch) && ch->pcdata->monster_fed <= 0 && !IS_FLAG(ch->act, PLR_GUEST)) {
      ch->pcdata->monster_fed = 300;
      ch->pcdata->ill_time = current_time + (3600 * 24 * 21);
    }

    ch->pcdata->lfcount++;
    ch->pcdata->lftotal += workinglf;

  }

  bool trapped_room(ROOM_INDEX_DATA *room, CHAR_DATA *ch) {
    int door;
    EXIT_DATA *pexit;


    if(IS_NPC(ch))
    return FALSE;

    if(ch->pcdata->travel_time > 0)
    return FALSE;

    if (in_cell(ch))
    return TRUE;

    if (room->vnum == ROOM_INDEX_SHERIFFCELL)
    return TRUE;

    for (door = 0; door <= 9; door++) {
      if ((pexit = room->exit[door]) != NULL && pexit->u1.to_room != NULL) {
        if (pexit->wall == WALL_NONE || pexit->wallcondition != WALLCOND_HOLE)
        return FALSE;
        if (!IS_SET(pexit->exit_info, EX_CLOSED) || !IS_SET(pexit->exit_info, EX_LOCKED))
        return FALSE;
        if (!door_locked(pexit, pexit->u1.to_room, ch))
        return FALSE;
      }
    }
    if (!IS_NPC(ch) && ch->pcdata->travel_time >= 0)
    return FALSE;

    return TRUE;
  }

  void autorelease(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *desti;
    int destix = number_range(70, 73);
    int destiy = 59;
    desti = room_by_coordinates(destix, destiy, 0);

    if (institute_room(ch->in_room) && (clinic_patient(ch) || college_student(ch, FALSE))) {
      act("A nurse comes and takes $n to get some exercise", ch, NULL, NULL, TO_ROOM);
      act("A nurse comes and takes you to get some exercise.", ch, NULL, NULL, TO_CHAR);
      ch->pcdata->travel_time = 0;
      char_from_room(ch);
      char_to_room(ch, get_room_index(16295));
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);
      if (IS_FLAG(ch->act, PLR_BOUND))
      REMOVE_FLAG(ch->act, PLR_BOUND);
      if (IS_FLAG(ch->act, PLR_BOUNDFEET))
      REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
      if (IS_FLAG(ch->comm, COMM_BLINDFOLD))
      REMOVE_FLAG(ch->comm, COMM_BLINDFOLD);
    }
    else {
      act("The floor suddenly opens up and you fall through it.", ch, NULL, NULL, TO_ROOM);
      act("The floor suddenly opens up and you fall through it.", ch, NULL, NULL, TO_CHAR);
      dact("People suddenly fall through the floor.", ch, NULL, NULL, DISTANCE_MEDIUM);
      ch->pcdata->travel_time = 0;
      ROOM_INDEX_DATA *origin = ch->in_room;
      if (is_prisoner(ch)) {
        char_from_room(ch);
        char_to_room(ch, sourced_room_by_coordinates(
        desti, desti->x, desti->y - 10 + number_range(0, 10), 0, FALSE));
        ch->pcdata->travel_time = 0;
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        if (IS_FLAG(ch->act, PLR_BOUND))
        REMOVE_FLAG(ch->act, PLR_BOUND);
        if (IS_FLAG(ch->act, PLR_BOUNDFEET))
        REMOVE_FLAG(ch->act, PLR_BOUNDFEET);
        if (IS_FLAG(ch->comm, COMM_BLINDFOLD))
        REMOVE_FLAG(ch->comm, COMM_BLINDFOLD);

        return;
      }
      CHAR_DATA *victim;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->connected != CON_PLAYING)
        continue;

        victim = CH(d);

        if (victim->in_room != origin)
        continue;

        char_from_room(victim);
        char_to_room(victim, sourced_room_by_coordinates(
        desti, desti->x, desti->y - 10 + number_range(0, 10), 0, FALSE));
        if (IS_FLAG(victim->act, PLR_BOUND))
        REMOVE_FLAG(victim->act, PLR_BOUND);
        if (IS_FLAG(victim->act, PLR_BOUNDFEET))
        REMOVE_FLAG(victim->act, PLR_BOUNDFEET);

        if (IS_FLAG(victim->comm, COMM_BLINDFOLD))
        REMOVE_FLAG(victim->comm, COMM_BLINDFOLD);
        WAIT_STATE(victim, PULSE_PER_SECOND * 5);
        victim->pcdata->travel_time = 0;
      }
    }
  }

  CHAR_DATA *fetch_guestmonster() {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL)
        continue;
        if (!guestmonster(to))
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        if (to->in_room->vnum < 300)
        continue;

        return to;
      }
    }
    return NULL;
  }

  CHAR_DATA *fetch_guestmonster_exclusive(CHAR_DATA *ch) {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL)
        continue;
        if (to == ch)
        continue;
        if (!guestmonster(to))
        continue;
        if (d->connected != CON_PLAYING)
        continue;

        if (to->in_room->vnum < 300)
        continue;

        return to;
      }
    }
    return NULL;
  }
  bool pre_full_moon(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time + (3600 * 24);
    ptm = gmtime(&east_time);
    if (moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL) == 4)
    return TRUE;

    return FALSE;
  }

  void time_update(void) {
    FILE *weather;
    char buf[MSL];
    int i;
    CHAR_DATA *ch;

    buf[0] = '\0';

    if ((weather = fopen(WEATHER_FILE, "w")) != NULL) {
      fprintf(weather, "%d %d %d %d %d %d %d %d %d %d %d %d %ld %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", time_info.minute, time_info.hour, time_info.day, time_info.month, time_info.year, time_info.local_temp, time_info.des_temp, time_info.local_cover_total, time_info.cover_trend, time_info.density_trend, time_info.local_density_total, time_info.understanding, time_info.phone, time_info.local_mist_level, time_info.mist_timer, time_info.faction_vnum, time_info.monster_hours, time_info.mutilate_hours, time_info.cult_alliance_issue, time_info.cult_alliance_type, time_info.bloodstorm, time_info.sect_alliance_issue, time_info.sect_alliance_type, time_info.lweek_tier, time_info.lweek_total, time_info.tweek_tier, time_info.tweek_total);
      fclose(weather);
    }
    else
    bugf("[%s:%s] Could not open weather file!", __FILE__, __FUNCTION__);
    time_info.minute++;
    if (time_info.minute == 25)
    temp_update();
    if (time_info.minute % 18 == 0)
    weather_update();


    if(time_info.minute % 5 == 0)
    social_update();
    operations_update();

    if (time_info.minute % 12 == 0) {
      if (time_info.mist_timer <= 0 && time_info.local_mist_level > 0) {
        time_info.local_mist_level--;
        global_message("The mist level lowers.\n\r");
      }
      if (time_info.mist_timer > 0) {
        time_info.mist_timer--;
        if (time_info.mist_timer < time_info.local_mist_level)
        time_info.local_mist_level--;
        if (time_info.mist_timer >= time_info.local_mist_level && time_info.local_mist_level < 3) {
          time_info.local_mist_level++;
          global_message("The mist level rises.\n\r");
        }
      }
    }

    if (time_info.minute >= INNER_NORTH_FOREST && time_info.minute <= HELL_FOREST_VNUM)
    forest_update(get_area_data(time_info.minute));

    if (time_info.minute % 10 == 0) {
      blackout_update();

      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->vnum == 0 || (*it)->stasis == 1)
        continue;

        shipment_check(*it);
      }
    }
    if (time_info.minute % 26 == 0) {
      playerroom_update();
      save_playerrooms();
    }
    save_weekly_characters();
    save_stories();
    save_plots();
    save_storyideas();
    save_petitions();
    save_locations();
    save_gateways();
    save_news();
    save_research();
    save_eencounter();
    save_epatrol();
    save_anniversaries();
    save_decrees();
    save_grouptexts();
    save_texthistories();
    news_update();
    story_update();
    research_update();
    event_update();
    save_houses(FALSE);
    save_events();
    // if(time_info.minute % 8 == 0)
    save_properties(FALSE);
    save_rent();
    save_blackmarket();
    abomination_global_update();
    demon_hole_update();
    // encroachment_update();
    encroachment_cleanup();
    save_chatrooms();
    save_profiles();
    save_matches();

    if (hell_temperature == 0 && other_temperature == 0 && wilds_temperature == 0)
    offworld_weather_update();
    // else if(time_info.minute % time_info.hour == 0 && number_percent() % 43 ==
    // 0) Disco's quick fix 7/3/2015: Game was crashing because result was
    // undefined when hour = 0
    else if (time_info.hour != 0) {
      if (time_info.minute % time_info.hour == 0 && number_percent() % 43 == 0) {
        offworld_weather_update();
      }
    }

    if (crisis_storm == 1) {
      if (time_info.local_temp < 35 && time_info.local_temp > 25)
      snowing = 1;
      else if (time_info.local_temp < 35)
      hailing = 1;
      else
      raining = 1;

      time_info.local_density_total = 100;
      time_info.local_cover_total = 100;
    }

    spawn_animal();
    spawn_animal();
    spawn_animal();
    invade_check();

    dream_lobby_update();


    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      /*
//Idle cleanup here.
if ((d->connected == CON_PLAYING && d->character == NULL) || d->connected < 0 || d->connected > 50 || d->descriptor < 0) {
close_desc(d);
continue;
}
*/
      if (d->connected == CON_PLAYING && IS_AWAKE(d->character)) {
        free_string(d->character->last_ip);
        d->character->last_ip = str_dup(d->host);
      }
    }

    if (time_info.minute % 54 == 0)
    roster_update();

    if (time_info.minute % 15 == 0) {
      CHAR_DATA *monster = fetch_guestmonster();
      if (monster != NULL) {
        makemonstermap();
        char sbuf[MSL];
        sprintf(sbuf, "Your scouts report the monster's location as: %s\n\rhttp://havenrpg.net/report.bmp\n\r", monster->in_room->name);
        scout_report_temp(sbuf);
      }
    }

    /*
if(room_fight(get_room_index(BATTLEROOM)))
{
winner_check();
battle_countdown++;
if(battle_countdown >= 35)
defend_battle();
}
else
{
if(pc_pop(get_room_index(BATTLEROOM)) > 0)
end_battle();
}
*/
    if (sunphase(NULL) == 2 && sunphase_delayed(NULL) == 1)
    dawn_update();

    if (time_info.minute % 2 == 0) {
      save_clans(FALSE);
      save_cabals(FALSE);
    }
    if (time_info.minute % 3 == 0) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        if ((*it)->connected == 10) {
          close_desc((*it));
          break;
        }
      }
    }
    if (time_info.minute % 5 == 0) {
      if (time_info.minute == 45)
      lair_update();

      if (time_info.minute % 15 == 0) {
        clan_update();
        institute_update();
        worker_update();
      }
      save_graves(FALSE);
      save_graves(TRUE);
      save_lairs(FALSE);
      save_rosterchars(FALSE);
      save_galleries(FALSE);
      save_pages(FALSE);
      save_storylines(FALSE);
      save_archive_indexes(FALSE);
      save_fantasies(FALSE);
      save_institutes(FALSE);
      save_phonebooks(FALSE);
      save_destinies(FALSE);
      save_domains(FALSE);
    }

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      ch = *it;
      ++it;

      if (ch == NULL)
      continue;

      if (IS_NPC(ch))
      continue;

      if (!ch->desc || ch->desc->connected == CON_CREATION)
      continue;

      ch->pcdata->time_since_train++;
      ch->pcdata->time_since_action++;

      if (in_fight(ch)) {
        ch->pcdata->time_since_action = 0;
        if (ch->pcdata->spectre == 2) {
          wake_char(ch);
          send_to_char("You wake up.\n\r", ch);
          act("$n stirs and awakens.", ch, NULL, NULL, TO_ROOM);
          log_string("WAKEUP FIGHT ONE");
        }
      }
      else {
        if (ch->pcdata->spectre == 1 && (ch->pcdata->patrol_status < PATROL_WARMOVINGATTACK || ch->pcdata->patrol_status > PATROL_WAGINGWAR)) {
          wake_char(ch);
          send_to_char("You wake up.\n\r", ch);
          act("$n stirs and awakens.", ch, NULL, NULL, TO_ROOM);
          log_string("WAKEUP FIGHT TWO");
        }
        ch->pcdata->combat_emoted = FALSE;
      }
      if (ch->pcdata->patrol_status <= 1)
      ch->pcdata->patrol_rp = 0;
      if (ch->pcdata->overworked > 0)
      ch->pcdata->overworked--;
      if (other_players(ch)) {
        ch->pcdata->timenotalone++;
        ch->pcdata->lastnotalone = current_time;
        ch->pcdata->account->lastnotalone = current_time;
      }
      if(!is_helpless(ch))
      {
        ch->pcdata->victimize_vic_response_to = 0;
        ch->pcdata->victimize_vic_timer = 0;
        ch->pcdata->victimize_vic_select = 0;
      }

      DOMAIN_TYPE *fdomain;
      if (!IS_NPC(ch) && !is_helpless(ch)) {
        fdomain = domain_from_room(ch->in_room);
        if (fdomain == NULL) {
          ch->pcdata->in_domain = 0;
          ch->pcdata->domain_timer = 0;
        }
        else if (fdomain != NULL) {
          if (ch->pcdata->in_domain != fdomain->vnum) {
            printf_to_char(ch, " `CYou enter the domain of %s`x \n\r", strip_newlines(fdomain->domain_of));
            ch->pcdata->domain_timer = 0;
          }
          ch->pcdata->in_domain = fdomain->vnum;
        }
      }

      if (time_info.minute % 13 == 0) {
        if (ch->pcdata->job_type_one == JOB_EMPLOYEE && get_char_world_pc(ch->pcdata->job_title_one) != NULL) {
          CHAR_DATA *boss = get_char_world_pc(ch->pcdata->job_title_one);
          if (ch->lf_taken > boss->lf_taken)
          boss->lf_taken = ch->lf_taken;
          if (boss->lf_taken > ch->lf_taken)
          ch->lf_taken = boss->lf_taken;
          if (boss->pcdata->emotes[EMOTE_TOTAL] /
              UMAX(1, boss->pcdata->timenotalone) >
              10 * ch->pcdata->emotes[EMOTE_TOTAL] /
              UMAX(1, ch->pcdata->timenotalone) && ch->pcdata->overworked == 0) {
            ch->pcdata->resident = 0;
            ch->pcdata->overworked = 120;
            send_to_char("You are overworked.\n\r", ch);
          }
        }
      }
      bool weakfound = FALSE;
      for (int x = 0; x < 10; x++) {
        if (ch->pcdata->relationship_type[x] == REL_WEAKNESS || ch->pcdata->relationship_type[x] == REL_PRAESTES) {
          weakfound = TRUE;
          ch->pcdata->weakness_timer--;
          if (ch->pcdata->weakness_timer <= 0) {
            weakness_update(ch, ch->pcdata->relationship[x]);
            ch->pcdata->weakness_timer = 60;
          }
        }
      }
      if (weakfound == FALSE) {
        ch->pcdata->weakness_status = 0;
      }

      if (IS_FLAG(ch->act, PLR_INVIS) && is_helpless(ch)) {
        REMOVE_FLAG(ch->act, PLR_INVIS);
      }

      if (ch->pcdata->exp_cap < DEFAULT_MAXEXP)
      ch->pcdata->exp_cap = DEFAULT_MAXEXP;

      if (ch->pcdata->dream_timer < 1000)
      ch->pcdata->dream_timer++;

      if (is_gm(ch) && !IS_IMMORTAL(ch) && !is_wildsapproved(ch) && !battleground(ch->in_room)) {
        if (in_world(ch) == WORLD_GODREALM || in_world(ch) == WORLD_OTHER || in_world(ch) == WORLD_WILDS || in_world(ch) == WORLD_HELL) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(GMHOME));
          if (IS_FLAG(ch->comm, COMM_RUNNING))
          REMOVE_FLAG(ch->comm, COMM_RUNNING);

          send_to_char("You return home.\n\r", ch);
          ch->pcdata->vote_timer = 0;
        }
      }
      if (crisis_uninvited == 1 && number_percent() % 6 == 0 && ch->pcdata->mind_guarded <= 0 && ch->in_room != NULL && ch->in_room->vnum > 300 && !is_gm(ch) && ch->race != RACE_FANTASY) {
        if (prop_from_room(ch->in_room) != NULL && get_room_index(prop_from_room(ch->in_room)->roadroom) != NULL && ch->in_room != NULL) {
          act("$n wanders outside.", ch, NULL, NULL, TO_ROOM);
          ROOM_INDEX_DATA *desti =
          get_room_index(prop_from_room(ch->in_room)->roadroom);
          char_from_room(ch);
          char_to_room(ch, desti);
        }
        else if (in_lodge(ch->in_room) || ch->in_room->area->vnum == 31) {
          act("$n wanders outside.", ch, NULL, NULL, TO_ROOM);
          char_from_room(ch);
          char_to_room(ch, get_room_index(2633));
        }
        else if (institute_room(ch->in_room) && IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && ch->wounds < 2) {
          act("$n wanders outside.", ch, NULL, NULL, TO_ROOM);
          char_from_room(ch);
          char_to_room(ch, get_room_index(1643));
        }
      }

      if (ch->pcdata->murder_timer > 0 && ch->pcdata->murder_timer < 100) {
        if (!in_hospital(ch) || number_percent() % 2 == 0) {
          if (ch->pcdata->sleeping < 1 || number_percent() % 2 == 0) {
            ch->pcdata->murder_timer--;
            if (ch->pcdata->murder_timer <= 0) {
              act("You feel your life slip away.", ch, NULL, NULL, TO_CHAR);
              act("$n goes still.", ch, NULL, NULL, TO_ROOM);
              free_string(ch->pcdata->deathcause);
              ch->pcdata->deathcause = str_dup("Organ failure");
              real_kill(ch, ch);
            }
            else {
              if (ch->pcdata->murder_timer % 10 == 0) {
                act("You weakly cough up some blood as your organs continue to fail.", ch, NULL, NULL, TO_CHAR);
                act("$n weakly coughs up some blood.", ch, NULL, NULL, TO_ROOM);
              }
            }
          }
        }
      }
      if (IS_FLAG(ch->comm, COMM_RUNNING))
      ch->pcdata->sr_nomove++;

      if (ch->pcdata->destiny_reject_cooldown > 0)
      ch->pcdata->destiny_reject_cooldown--;

      if (ch->pcdata->in_domain > 0 && ch->pcdata->domain_timer < 300) {
        if (ch->pcdata->domain_timer == 5) {
          DOMAIN_TYPE *vdomain = vnum_domain(ch->pcdata->in_domain);
          CHAR_DATA *domhp = get_char_world_pc(vdomain->domain_of);
          if (domhp != NULL) {
            printf_to_char(domhp, "\n`c%s is in your domain.`x\n\r", PERS(ch, domhp));
            printf_to_char(
            ch, "\n`c%s now bears witness to your mortal struggles.`x\n\r", PERS(domhp, ch));
          }
        }
        ch->pcdata->domain_timer++;
      }

      if (ch->pcdata->summon_bound < 0)
      ch->pcdata->summon_bound++;
      else if (ch->pcdata->summon_bound > 0) {
        ch->pcdata->summon_bound--;
        if (ch->pcdata->summon_bound == 0)
        ch->pcdata->summon_bound = -270;
      }

      if (ch->in_room != NULL && ch->in_room->area->vnum == 30 && !is_dreaming(ch)) {
        char_from_room(ch);
        if (ch->race == RACE_FANTASY) {
          char_to_room(ch, get_room_index(405500));
        }
        else
        char_to_room(ch, get_room_index(11597));
      }

      solitary_check(ch);
      if (ch->pcdata->detention_time > 0) {
        if (nondetain_pop(ch->in_room) < 1) {
          ch->pcdata->detention_time--;
          if (ch->pcdata->detention_time <= 0 && ch->in_room->vnum == 4854) {
            send_to_char("You are released.\n\r", ch);
            char_from_room(ch);
            char_to_room(ch, get_room_index(4331));
          }
        }
      }
      if (!institute_room(ch->in_room)) {
        OBJ_DATA *obj;
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (obj->pIndexData->vnum == 45194) {
            extract_obj(obj);
            return;
          }
        }
      }

      if (!in_fight(ch) && !battleground(ch->in_room)) {
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->vnum == 0)
          continue;

          if (in_fac(ch, (*it)->vnum)) {
            if ((*it)->stasis == 1) {
              if (ch->faction == (*it)->vnum)
              ch->faction = 0;
              if (ch->factiontwo == (*it)->vnum)
              ch->factiontwo = 0;
            }
            else {
              if (ch->faction != (*it)->vnum && ch->factiontwo != (*it)->vnum) {
                if (ch->faction == 0 || !in_fac(ch, ch->faction))
                ch->faction = (*it)->vnum;
                else if (ch->factiontwo == 0 || !in_fac(ch, ch->factiontwo))
                ch->factiontwo = (*it)->vnum;
              }
            }
          }
        }
      }
      if (!IS_FLAG(ch->comm, COMM_MANDRAKE) && get_skill(ch, SKILL_DREAMSIGHT) > 0) {
        SET_FLAG(ch->comm, COMM_MANDRAKE);
      }
      if (!IS_FLAG(ch->comm, COMM_MANDRAKE) && get_skill(ch, SKILL_TOUCHED) > 0) {
        SET_FLAG(ch->comm, COMM_MANDRAKE);
      }

      if (ch->pcdata->psychic_proof == 0) {
        if (cardinal(ch) || IS_FLAG(ch->act, PLR_TYRANT))
        ch->pcdata->psychic_proof = current_time + (3600 * 24 * 7 * 2);
      }

      if (ch->pcdata->process_timer > 0 && !is_gm(ch) && !IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_HIDE)) {
        if (ch->pcdata->process_target != NULL && ch->in_room != ch->pcdata->process_target->in_room) {
          ch->pcdata->process_timer = 0;
          send_to_char("Your target isn't here any longer.\n\r", ch);
        }
        else if (ch->pcdata->process_target == NULL && ch->pcdata->process_subtype > 0 && ch->pcdata->process != PROCESS_TRAVEL_PREP && ch->pcdata->process != PROCESS_RESEARCH && ch->pcdata->process != PROCESS_RITUAL && ch->pcdata->process_subtype != ch->in_room->vnum) {
          ch->pcdata->process_timer = 0;
          send_to_char("You stop working when you move.\n\r", ch);
        }
        if (ch->pcdata->process_timer > 0 && (ch->pcdata->sleeping > 0 || IS_FLAG(ch->act, PLR_BOUND))) {
          ch->pcdata->process_timer = 0;
          send_to_char("You are interrupted.\n\r", ch);
        }
        if (IS_FLAG(ch->act, PLR_HIDE))
        REMOVE_FLAG(ch->act, PLR_HIDE);
      }

      patrol_update(ch);
      patrol_launch_check(ch);

      bool neutralfound = FALSE;
      OBJ_DATA *obj;

      for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) != NULL) {
          if (obj->pIndexData->vnum == ITEM_NEUTRALCOLLAR)
          neutralfound = TRUE;
        }
      }

      if (neutralfound == TRUE) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 18;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_NEUTRALIZED;
        affect_to_char(ch, &af);
      }

      if (ch->pcdata->dream_attack_cool > 0)
      ch->pcdata->dream_attack_cool--;

      if (!cell_signal(ch)) {
        if (ch->pcdata->connected_to != NULL && ch->pcdata->connection_stage != CONNECT_NONE) {
          act("You lose the connection.\n\r", ch, NULL, ch, TO_CHAR);
          act("You lose the connection.\n\r", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);

          ch->pcdata->connected_to->pcdata->connection_stage = CONNECT_NONE;
          ch->pcdata->connected_to->pcdata->connected_to = NULL;
          ch->pcdata->connection_stage = CONNECT_NONE;
          ch->pcdata->connected_to = NULL;
        }
      }

      if (ch->in_room != NULL && ch->in_room->vnum == GMHOME && !is_gm(ch) && !higher_power(ch)) {
        char_from_room(ch);
        char_to_room(ch, get_room_index(1468));
      }

      if (is_animal(ch) && ch->pcdata->sleeping > 0 && number_percent() % 7 == 0)
      human_transformation(ch);

      if (ch->pcdata->lunacy_curse < 0 || ch->pcdata->lunacy_curse > 2000)
      ch->pcdata->lunacy_curse = 0;

      if (ch->pcdata->lunacy_curse > 0 && ch->pcdata->maintained_ritual != RITUAL_LUNACY)
      ch->pcdata->lunacy_curse--;

      if (time_info.minute == (number_percent() % 60) && time_info.minute % 2 == 0 && !is_gm(ch))
      rp_prompt(ch);

      if (ch->pcdata->process_timer > 0 && !room_hostile(ch->in_room)) {
        if (IS_FLAG(ch->act, PLR_HIDE))
        REMOVE_FLAG(ch->act, PLR_HIDE);

        if (ch->pcdata->time_since_emote < (12 * 8) || ch->pcdata->process_target == NULL || ch->pcdata->process_target == ch) {
          if (is_pinned(ch) || in_fight(ch)) {
            ch->pcdata->process_timer = 0;
            ch->pcdata->process = 0;
            ch->pcdata->process_target = NULL;
            send_to_char("Your concentration is ruined.\n\r", ch);
          }
          else {
            ch->pcdata->process_timer--;

            if ((ch->pcdata->process >= PROCESS_REPAIR && ch->pcdata->process <= PROCESS_PAVING) || ch->pcdata->process == PROCESS_SEARCH) {
              for (CharList::iterator it = ch->in_room->people->begin();
              it != ch->in_room->people->end();) {
                CHAR_DATA *fch = *it;
                ++it;
                if (fch == NULL || IS_NPC(fch))
                continue;

                if (fch->pcdata->process == ch->pcdata->process && ch->pcdata->process_timer > 1)
                ch->pcdata->process_timer--;
              }
            }
            if (ch->pcdata->process_timer == 0)
            conclude_process(ch, ch->pcdata->process);
            else if (ch->pcdata->process_timer % 5 == 0)
            process_message(ch, ch->pcdata->process);
          }
        }
      }

      if (ch->pcdata->encounter_status == ENCOUNTER_REJECTED && time_info.minute == 35)
      ch->pcdata->encounter_status = 0;

      if (ch->pcdata->note_gain > 0)
      ch->pcdata->note_gain--;

      if (guestmonster(ch)) {
        if (ch->in_room != NULL) {
          if (ch->in_room->area->vnum < 13 || ch->in_room->area->vnum > 18)
          ch->played = 0;
          if (ch->in_room->sector_type == SECT_AIR)
          ch->played = 0;
          if (ch->in_room->sector_type == SECT_UNDERWATER)
          ch->played = 0;
          if (ch->in_room->x > 73)
          ch->played = 0;
          if (event_cleanse == 1)
          ch->played = 0;
          if (in_world(ch) != WORLD_EARTH)
          ch->played = 0;
        }
      }

      if (ch->pcdata->territory_editing[TEDIT_TIMER] > 0)
      ch->pcdata->territory_editing[TEDIT_TIMER]--;

      if (ch->in_room != NULL && time_info.minute % 54 == 0 && ch->in_room->area->vnum >= 2 && ch->in_room->area->vnum < 12) {
        bool proomfound = FALSE;
        for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
        it != PlayerroomVect.end(); ++it) {
          if (!(*it)->author || (*it)->author[0] == '\0') {
            continue;
          }
          if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
          continue;
          if ((*it)->vnum == ch->in_room->vnum)
          proomfound = TRUE;
        }
        if (proomfound == FALSE) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(1021));
        }
      }
      if (under_opression(ch) && !IS_AFFECTED(ch, AFF_DISCREDIT)) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 300;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_DISCREDIT;
        affect_to_char(ch, &af);
      }

      if (get_tier(ch) < 3 || ch->pcdata->next_henchman == 0 || ch->in_room == NULL || ch->in_room->vnum < 300)
      ch->pcdata->next_henchman =
      current_time + (3600 * 24 * number_range(20, 30) * 2);
      if (get_tier(ch) < 2 || ch->pcdata->next_enemy == 0 || ch->in_room == NULL || ch->in_room->vnum < 300)
      ch->pcdata->next_enemy =
      current_time + (3600 * 24 * number_range(20, 30) * 2);
      if (get_tier(ch) < 2 || ch->pcdata->next_monster == 0 || ch->in_room == NULL || ch->in_room->vnum < 300)
      ch->pcdata->next_monster =
      current_time + (3600 * 24 * number_range(20, 30) * 4);

      if (IS_FLAG(ch->act, PLR_TYRANT) && ch->pcdata->became_tyrant == 0)
      ch->pcdata->became_tyrant = current_time;
      if (!IS_FLAG(ch->act, PLR_TYRANT))
      ch->pcdata->became_tyrant = 0;
      if (IS_FLAG(ch->act, PLR_TYRANT) && ch->pcdata->became_tyrant < (current_time - (3600 * 24 * 75))) {
        REMOVE_FLAG(ch->act, PLR_TYRANT);
        send_to_char("Your tyranny ends.\n\r", ch);
      }

      if (ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NIGHTMARE) && ch->pcdata->account->maxhours < 200) {
        sprintf(buf, "NIGHTMARE STRIP %s", ch->name);
        log_string(buf);
        REMOVE_FLAG(ch->pcdata->account->flags, ACCOUNT_NIGHTMARE);
      }

      if (in_haven(ch->in_room))
      ch->pcdata->offworld_protection = 0;

      if (ch->pcdata->other_fame_level > 3 || ch->pcdata->other_fame_level < 0)
      ch->pcdata->other_fame_level = 0;
      if (ch->pcdata->other_legendary > SKILL_USED || ch->pcdata->other_legendary < 0)
      ch->pcdata->other_legendary = 0;
      if (ch->pcdata->wilds_fame_level > 3 || ch->pcdata->wilds_fame_level < 0)
      ch->pcdata->wilds_fame_level = 0;
      if (ch->pcdata->wilds_legendary > SKILL_USED || ch->pcdata->wilds_legendary < 0)
      ch->pcdata->wilds_legendary = 0;
      if (ch->pcdata->godrealm_fame_level > 3 || ch->pcdata->godrealm_fame_level < 0)
      ch->pcdata->godrealm_fame_level = 0;
      if (ch->pcdata->godrealm_legendary > SKILL_USED || ch->pcdata->godrealm_legendary < 0)
      ch->pcdata->godrealm_legendary = 0;
      if (ch->pcdata->hell_fame_level > 3 || ch->pcdata->hell_fame_level < 0)
      ch->pcdata->hell_fame_level = 0;
      if (ch->pcdata->hell_legendary > SKILL_USED || ch->pcdata->hell_legendary < 0)
      ch->pcdata->hell_legendary = 0;

      if (time_info.minute % 5 == 0) {
        int stand = social_standing(ch);
        stand -= 35;
        stand = UMAX(stand, 2);
        int max = stand * (int)(sqrt(stand));
        if (IS_FLAG(ch->act, PLR_GUEST))
        max /= 3;

        max = UMIN(max, 10000);
        ch->pcdata->influence_bank =
        UMIN(max, ch->pcdata->influence_bank + stand);
        for (int i = 9; i > 0; i--) {
          free_string(ch->pcdata->attention_history[i]);
          ch->pcdata->attention_history[i] =
          str_dup(ch->pcdata->attention_history[i - 1]);
        }
      }

      if (time_info.minute % 8 == 0)
      account_sync(ch);

      if (time_info.minute % 25 == 0 && ch->pcdata->mind_guard_mult > 0)
      ch->pcdata->mind_guard_mult--;

      if (ch->pcdata->disperse_room > 0) {
        if (!is_gm(ch)) {
          if (ch->in_room->vnum == ch->pcdata->disperse_room && !IS_FLAG(ch->act, PLR_SHROUD)) {
            ch->pcdata->disperse_timer += 10;
            if (ch->pcdata->disperse_timer >= 100 && !guestmonster(ch)) {
              act("Some deputies come and pick $n up.", ch, NULL, NULL, TO_ROOM);
              act("Some deputies come and pick you up.", ch, NULL, NULL, TO_CHAR);
              auto_arrest(ch);
              ch->pcdata->public_alarm = -60;
              ch->pcdata->disperse_timer = 0;
              ch->pcdata->disperse_room = 0;
            }
          }
          else if (ch->pcdata->disperse_timer > 0) {
            ch->pcdata->disperse_timer--;
          }
        }
      }

      if (is_helpless(ch) && ch->pcdata->victimize_difficult_time > current_time)
      psychic_feast(ch, -10, 1);
      else if (IS_FLAG(ch->comm, COMM_PRIVATE))
      psychic_feast(ch, PSYCHIC_LUST, 1);
      else
      psychic_feast(ch, PSYCHIC_AMBIANT, 1);

      if (ch->played / 3600 < 20 && ch->played / 3600 > 5 && is_roster_char(ch)) {
        get_rosterchar(ch->name)->lastactive = current_time;
        if (ch->pcdata->account != NULL && ch->pcdata->account->roster_cool < current_time && str_cmp(ch->pcdata->account_name, get_rosterchar(ch->name)->owner)) {
          ch->pcdata->account->roster_cool = current_time + (3600 * 24 * 30);
          ch->pcdata->account->newcharcount--;
        }
      }
      for (int x = 0; x < 10; x++) {
        if (ch->pcdata->previous_focus_vnum[x] <
            SKILL_MAX) { // Disco quick fix 10/22/2015
          if (ch->skills[ch->pcdata->previous_focus_vnum[x]] >=
              ch->pcdata->previous_focus_level[x])
          ch->pcdata->previous_focus_vnum[x] = 0;
        }
        else
        ch->pcdata->previous_focus_vnum[x] = 0;
      }

      if (ch->played / 3600 > 200 && is_roster_char(ch) && str_cmp(get_rosterchar(ch->name)->owner, "Daed")) {
        if (ch->pcdata->account->maxhours <= 300) {
          CHAR_DATA *authorchar =
          get_char_by_account(get_rosterchar(ch->name)->owner);
          if (authorchar != NULL) {
            give_karma(authorchar, 5000, KARMA_OTHER);
            remove_from_roster(ch->name);
          }
        }
        else {
          CHAR_DATA *authorchar =
          get_char_by_account(get_rosterchar(ch->name)->owner);
          if (authorchar != NULL) {
            give_karma(authorchar, 1000, KARMA_OTHER);
            remove_from_roster(ch->name);
          }
        }
      }
      else if (ch->played / 3600 > 250 && is_roster_char(ch) && str_cmp(get_rosterchar(ch->name)->owner, "Daed")) {
        remove_from_roster(ch->name);
      }

      if (in_haven(ch->in_room))
      ch->pcdata->lingering_sanc = 0;
      else if (ch->pcdata->lingering_sanc > 0 && !is_helpless(ch) && !is_prisoner(ch) && !locked_room(ch->in_room, ch) && !is_pinned(ch))
      ch->pcdata->lingering_sanc--;

      if (ch->pcdata->account != NULL) {
        if (ch->pcdata->account->newcharcount < MAX_NEWCHAR) {
          if (ch->pcdata->account->newcharcount < 0)
          ch->pcdata->account->newcharcount = 0;
          if (ch->pcdata->account->newcharcool == 0)
          ch->pcdata->account->newcharcool = current_time + (3600 * 24 * 45);
          if (ch->pcdata->account->newcharcool <
              (current_time + (3600 * 25 * 14))) {
            ch->pcdata->account->newcharcool = current_time + (3600 * 24 * 45);
            ch->pcdata->account->newcharcount++;
          }
        }
      }

      if (in_haven(ch->in_room) && under_understanding(ch, ch) && event_cleanse == 0 && (IS_FLAG(ch->act, PLR_BOUND) || IS_FLAG(ch->act, PLR_BOUNDFEET) || trapped_room(ch->in_room, ch))) {
        if (ch->pcdata->prison_mult == 0) {
          ch->pcdata->prison_mult = 1;
          ch->pcdata->prison_care = current_time + (3600 * 5);
        }
        if (ch->pcdata->prison_care < current_time)
        autorelease(ch);
        else {
          for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
          it != EventVect.end(); ++it) {
            if ((*it)->valid == FALSE)
            continue;

            if (current_time > (*it)->active_time || current_time > (*it)->deactive_time)
            continue;

            if ((*it)->type == EVENT_UNDERSTANDINGMINUS || (*it)->type == EVENT_CLEANSE)
            autorelease(ch);
            if ((*it)->typetwo == EVENT_UNDERSTANDINGMINUS || (*it)->typetwo == EVENT_CLEANSE)
            autorelease(ch);
          }
        }
      }
      else
      ch->pcdata->prison_care = current_time + (3600 * 14);

      if (in_fight(ch) && ch->fight_fast == FALSE && (ch->fight_current == NULL || !same_fight(ch, ch->fight_current))) {
        CHAR_DATA *temp = next_fight_member(ch);
        if (temp != NULL && temp->fight_current != NULL && same_fight(ch, temp->fight_current) && temp->fight_current->actiontimer > 0)
        ch->fight_current = temp->fight_current;
        else {
          if (ch->fight_current == NULL)
          sprintf(buf, "FIGHTPROB: No current fight %s", ch->name);
          else
          sprintf(buf, "FIGHTPROB: NOt in same fight. %s and %s.", ch->name, ch->fight_current->name);
          log_string(buf);
          next_attacker(ch, FALSE);
        }
      }
      if (ch->pcdata->maintained_ritual != 0) {
        if (safe_strlen(ch->pcdata->ritual_maintainer) < 1)
        ch->pcdata->maintained_ritual = 0;
        else {
          CHAR_DATA *caster;
          caster = get_char_world_pc(ch->pcdata->ritual_maintainer);
          if (caster != NULL) {
            if (str_cmp(caster->pcdata->maintained_target, ch->name)) {
              ch->pcdata->maintained_ritual = 0;
              free_string(ch->pcdata->ritual_maintainer);
              ch->pcdata->ritual_maintainer = str_dup("");
            }
          }
          else if (!character_exists(ch->pcdata->ritual_maintainer)) {
            ch->pcdata->maintained_ritual = 0;
            free_string(ch->pcdata->ritual_maintainer);
            ch->pcdata->ritual_maintainer = str_dup("");
          }
          else if (daysidle(ch->pcdata->ritual_maintainer) > 2) {
            ch->pcdata->maintained_ritual = 0;
            free_string(ch->pcdata->ritual_maintainer);
            ch->pcdata->ritual_maintainer = str_dup("");
          }
          else if (active_character(ch->pcdata->ritual_maintainer) == FALSE) {
            ch->pcdata->maintained_ritual = 0;
            free_string(ch->pcdata->ritual_maintainer);
            ch->pcdata->ritual_maintainer = str_dup("");
          }
        }
      }
      if (safe_strlen(ch->pcdata->maintained_target) > 1 && str_cmp(ch->pcdata->maintained_target, "casting")) {
        CHAR_DATA *caster;
        caster = get_char_world_pc(ch->pcdata->maintained_target);
        if (caster != NULL) {
          if (ch->pcdata->maintain_cost == 0)
          ch->pcdata->maintain_cost =
          10 * 100 / ritual_debuff_power(ch, caster);
          if (caster->pcdata->maintained_ritual == 0) {
            free_string(ch->pcdata->maintained_target);
            ch->pcdata->maintained_target = str_dup("");
          }
        }
        else if (!character_exists(ch->pcdata->maintained_target)) {
          free_string(ch->pcdata->maintained_target);
          ch->pcdata->maintained_target = str_dup("");
        }
        else if (daysidle(ch->pcdata->maintained_target) > 10) {
          free_string(ch->pcdata->maintained_target);
          ch->pcdata->maintained_target = str_dup("");
        }
      }
      else
      ch->pcdata->maintain_cost = 0;

      if (ch->pcdata->trance > 0) {
        ch->pcdata->trance--;
        if (ch->pcdata->trance == 0)
        ch->pcdata->trance = -60;
      }
      else if (ch->pcdata->trance < 0)
      ch->pcdata->trance++;

      if (ch->pcdata->persuade_cooldown > 0)
      ch->pcdata->persuade_cooldown--;

      if (ch->pcdata->dream_identity_timer > 0 && ch->pcdata->maintained_ritual != RITUAL_DREAMBELIEF && ch->pcdata->maintained_ritual != RITUAL_DREAMIDENTITY)
      ch->pcdata->dream_identity_timer--;

      if(ch->pcdata->ritual_prey_timer > 0 && ch->pcdata->maintained_ritual != RITUAL_PREDATE)
      ch->pcdata->ritual_prey_timer--;

      if (ch->pcdata->sexchange_time > 0) {
        if (ch->pcdata->maintained_ritual != RITUAL_SEXCHANGE) {
          ch->pcdata->sexchange_time--;
          if (ch->pcdata->sexchange_time <= 0) {
            ch->pcdata->penis = ch->pcdata->truepenis;
            ch->pcdata->bust = ch->pcdata->truebreasts;
            send_to_char("Your body tingles.\n\r", ch);
          }
        }
      }

      for (i = 0; i < 25; i++) {
        if (ch->pcdata->imprint_pending[i] > 10 && ch->pcdata->imprint_type[i] > 0) {
          ch->pcdata->imprint_pending[i]--;
          if (ch->pcdata->imprint_pending[i] == 10)
          ch->pcdata->imprint_type[i] = 0;
        }
        if (ch->pcdata->imprint_pending[i] < 0 && ch->pcdata->imprint_type[i] > 0) {
          ch->pcdata->imprint_pending[i]++;
          if (ch->pcdata->imprint_pending[i] == 0)
          send_to_char("The imprint takes hold.\n\r", ch);
        }
      }
      if (time_info.minute % 25 == 0)
      show_imprints(ch);

      if (time_info.minute % 23 == 0 && ch->pcdata->availability == AVAIL_LOW)
      send_to_char("You have low availability.\n\r", ch);

      if(ch->pcdata->movealong_vnum != 0 && (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || room_hostile(ch->in_room) || locked_room(ch->in_room, ch)))
      {
        ch->pcdata->movealong_timer = 0;
        ch->pcdata->movealong_vnum = 0;
      }


      if (ch->pcdata->movealong_vnum != 0) {
        if (prop_from_room(ch->in_room) ==
            prop_from_room(get_room_index(ch->pcdata->movealong_vnum))) {
          ch->pcdata->movealong_timer += 10;
        }
        else if (ch->pcdata->movealong_timer > 0)
        ch->pcdata->movealong_timer--;
        if (ch->pcdata->movealong_timer >= 100 && !guestmonster(ch)) {
          act("Some deputies come and pick $n up.", ch, NULL, NULL, TO_ROOM);
          act("Some deputies come and pick you up.", ch, NULL, NULL, TO_CHAR);

          auto_arrest(ch);
          ch->pcdata->public_alarm = -60;
          ch->pcdata->movealong_timer = 0;
        }
      }

      if (ch->pcdata->finale_timer > 0)
      ch->pcdata->finale_timer--;

      if (time_info.minute % 5 == 0 && !in_fight(ch)) {
        ch->preyvictim = FALSE;
        ch->pcdata->minion_limit = 0;
        ch->pcdata->ally_limit = 0;
        ch->positive_magic = 0;
        ch->negative_magic = 0;
        ch->command_force = 0;
      }

      if (IS_FLAG(ch->act, PLR_BOUND)) {
        if (ch->pcdata->escape_timer > (60 * 80))
        ch->pcdata->escape_timer = 0;
        if (unbound_pop(ch->in_room) == 0) {
          ch->pcdata->escape_timer++;
          if (ch->pcdata->escape_timer >= (60 * 60)) {
            char_from_room(ch);
            char_to_room(ch, get_room_index(12789));
            REMOVE_FLAG(ch->act, PLR_BOUND);
            send_to_char("You escape!\n\r", ch);
          }
        }
        else
        ch->pcdata->escape_timer -= 10;
      }
      else
      ch->pcdata->escape_timer -= 5;

      if (ch->pcdata->account != NULL) // && number_percent() % 24 == 0)
      {
        ch->pcdata->account->rpxp += ch->rpexp;
        ch->rpexp = 0;
        ch->pcdata->account->xp += ch->exp;
        ch->exp = 0;
        ch->pcdata->account->karma += ch->karma;
        ch->karma = 0;
        ch->pcdata->account->pkarma += ch->pkarma;
        ch->pkarma = 0;
        if (ch->pcdata->account->pkarma < 0)
        ch->pcdata->account->pkarma = 0;
        ch->pcdata->account->donated += ch->donated;
        ch->donated = 0;
        ch->pcdata->account->colours += ch->pcdata->colours;
        ch->pcdata->colours = 0;
        ch->pcdata->account->bandaids += ch->pcdata->bandaids;
        ch->pcdata->bandaids = 0;
        ch->pcdata->account->renames += ch->pcdata->renames;
        ch->pcdata->renames = 0;

        if (ch->in_room != NULL && ch->in_room->vnum == 98)
        ch->pcdata->influence = 0;
        if (ch->pcdata->dexp > 1000000 || ch->pcdata->dexp < 0)
        ch->pcdata->dexp = 0;
        if (ch->pcdata->wexp > 1000000 || ch->pcdata->wexp < 0)
        ch->pcdata->wexp = 0;
        if (ch->pcdata->aexp > 1000000 || ch->pcdata->aexp < 0)
        ch->pcdata->aexp = 0;
        if (ch->pcdata->influence > 1000000 || ch->pcdata->influence < -1000000)
        ch->pcdata->influence = 10000;
        if (ch->pcdata->influence_bank > 1000000 || ch->pcdata->influence_bank < 0)
        ch->pcdata->influence_bank = 0;
      }

      ch->pcdata->escape_timer = UMAX(ch->pcdata->escape_timer, 0);

      if (ch->pcdata->healthtimer > 0 && ch->pcdata->healthtimer < 50) {
        ch->pcdata->healthtimer++;
        if (ch->pcdata->healthtimer >= 46) {
          send_to_char("\n\n\n\n\n\n|||Take a break from your computer screen and stretch.|||\n\n\n\n\r", ch);
          ch->pcdata->healthtimer = 1;
        }
      }

      if (time_info.minute % 15 == 0) {
        if (multiplayer_three(ch))
        send_to_char("`rYou cannot have three characters logged in at once.`x\n\r", ch);
        if (multiplayer_super(ch))
        send_to_char("`rYou cannot have two supernatural characters logged in at once.`x\n\r", ch);
        if (multiplayer_abom(ch))
        send_to_char("`rYou cannot have two characters logged in at once when one of them is an abomination.`x\n\r", ch);
      }

      if (time_info.minute == 28) {
        if (ch->wounds == 0 && !is_ill(ch)) {
          if (ch->pcdata->lf_modifier < 0)
          ch->pcdata->lf_modifier++;
        }
        else {
          int limit = 0;
          if (ch->wounds == 3)
          limit = UMIN(limit, -20);
          if (ch->wounds == 2)
          limit = UMIN(limit, -10);
          if (ch->wounds == 1)
          limit = UMIN(limit, -5);
          if (is_ill(ch))
          limit = UMIN(limit, -10);
          if (ch->pcdata->lf_modifier > limit)
          ch->pcdata->lf_modifier--;
        }
      }

      if (time_info.minute % 23 == 0 && number_percent() % 21 == 0) {
        if (current_time > ch->pcdata->superlocked && ch->pcdata->relapsevampire > 0) {
          tm *ptm;
          time_t east_time;

          east_time = current_time;
          ptm = gmtime(&east_time);
          int year = ptm->tm_year + 1900;

          ch->race = RACE_NEWVAMPIRE;
          ch->pcdata->sire_year = year;
          ch->pcdata->sire_month = ptm->tm_mon + 1;
          ch->pcdata->sire_day = ptm->tm_mday;
          ch->pcdata->coma = current_time + (3600 * 2);
          send_to_char("You feel a sudden sharp shooting pain in your left arm and then collapse.\n\r", ch);
          act("$n collapses.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->relapsevampire = 0;
          ch->pcdata->relapsewerewolf = 0;
        }
        if (current_time > ch->pcdata->superlocked && ch->pcdata->relapsewerewolf > 0) {
          ch->race = RACE_NEWWEREWOLF;
          ch->pcdata->relapsewerewolf = 0;
          ch->pcdata->relapsevampire = 0;
        }
      }

      if (IS_AFFECTED(ch, AFF_PERSECUTED) && time_info.minute % 34 == 0) {
        CHAR_DATA *tch;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end(); ++it) {
          tch = *it;

          if (tch == NULL || IS_NPC(tch) || is_gm(tch) || mindwarded(tch) || tch == ch)
          continue;

          if (!has_persecution_imprint(tch, ch))
          persecute_char(tch, ch);
        }
      }

      if (IS_AFFECTED(ch, AFF_MAD) && number_percent() % 9 == 0) {
        bool madness = FALSE;
        CHAR_DATA *tch;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end(); ++it) {
          tch = *it;

          if (tch == NULL || IS_NPC(tch) || is_gm(tch) || tch == ch)
          continue;

          if (number_percent() % 7 == 0) {
            switch (number_percent() % 4) {
            case 0:
              printf_to_char(ch, "%s stares at you.\n\r", PERS(tch, ch));
              break;
            case 1:
              printf_to_char(ch, "%s points at you.\n\r", PERS(tch, ch));
              break;
            case 2:
              printf_to_char(ch, "%s laughs at you.\n\r", PERS(tch, ch));
            case 3:
              printf_to_char(ch, "%s flips you off.\n\r", PERS(tch, ch));

              break;
            }
            madness = TRUE;
          }
        }
        if (madness == FALSE && number_percent() % 4 == 0) {
          switch (number_percent() % 12) {
          case 0:
            act_new("A voice in your head says '$t`x'", ch, "Kill them.", ch, TO_VICT, POS_DEAD);
            break;
          case 1:
            act_new("A voice in your head says '$t`x'", ch, "They know what you really are.", ch, TO_VICT, POS_DEAD);
            break;
          case 2:
            act_new("A voice in your head says '$t`x'", ch, "It will stop if you kill them.", ch, TO_VICT, POS_DEAD);
            break;
          case 3:
            act_new("A voice in your head says '$t`x'", ch, "Make them shut up!", ch, TO_VICT, POS_DEAD);
            break;
          case 4:
            act_new("A voice in your head says '$t`x'", ch, "Wear their blood.", ch, TO_VICT, POS_DEAD);
            break;
          case 5:
            act_new("A voice in your head says '$t`x'", ch, "They're coming for you.", ch, TO_VICT, POS_DEAD);
            break;
          case 6:
            if (ch->sex == SEX_FEMALE)
            act_new("A voice in your head says '$t`x'", ch, "Slut.", ch, TO_VICT, POS_DEAD);
            else
            act_new("A voice in your head says '$t`x'", ch, "Weakling.", ch, TO_VICT, POS_DEAD);
            break;
          case 7:
            send_to_char("You feel a faint chill.\n\r", ch);
            break;
          case 8:
            send_to_char("You feel watched.\n\r", ch);
            break;
          case 9:
            act_new("A voice in your head says '$t`x'", ch, "They're laughing at you.", ch, TO_VICT, POS_DEAD);
            break;
          case 10:
            act_new("A voice in your head says '$t`x'", ch, "Nobody really likes you.", ch, TO_VICT, POS_DEAD);
            break;
          case 11:
            act_new("A voice in your head says '$t`x'", ch, "Everyone knows how pathetic you are.", ch, TO_VICT, POS_DEAD);
            break;
          }
          madness = TRUE;
        }
        if (madness == FALSE && number_percent() % 3 == 0 && get_phone(ch) != NULL) {
          if (IS_SET(get_phone(ch)->extra_flags, ITEM_SILENT)) {
            act("Your $p vibrates.", ch, get_phone(ch), NULL, TO_CHAR);
          }
          else
          act("Your $p beeps.", ch, get_phone(ch), NULL, TO_CHAR);
          madness = TRUE;
        }
      }

      if (ch->pcdata->sincool > 0)
      ch->pcdata->sincool--;

      if (ch->pcdata->astatus > 500)
      ch->pcdata->astatus = 0;
      if (ch->pcdata->astatus < -500)
      ch->pcdata->astatus = 0;
      if (ch->pcdata->astatus > 0)
      ch->pcdata->astatus--;
      else if (ch->pcdata->astatus < 0)
      ch->pcdata->astatus++;

      if (ch->pcdata->blackout_cooldown > 0)
      ch->pcdata->blackout_cooldown--;

      if (ch->pcdata->abomblackcool > 0)
      ch->pcdata->abomblackcool--;
      if (ch->pcdata->abommistcool > 0)
      ch->pcdata->abommistcool--;

      garage_update(ch, 1);

      if (time_info.minute == 23) {
        if ((is_werewolf(ch) || arcane_focus(ch) > 0) && pre_full_moon() == TRUE)
        send_to_char("You sense the rising of the full moon.\n\r", ch);

        if (guestmonster(ch) && ch->played / 3600 > time_info.monster_hours)
        send_to_char("Your monster can now delete to be reborn as a living nightmare.\n\r", ch);
      }

      if (time_info.minute == 43 && time_info.hour % 3 == 0) {
        if (get_tier(ch) >= 3 && is_super(ch) && !IS_FLAG(ch->act, PLR_GUEST)) {
          if (ch->pcdata->monster_fed <= 400)
          send_to_char("You feel slightly weak, as if you haven't consumed enough life force recently.\n\r", ch);
          else if (ch->pcdata->monster_fed <= 250)
          send_to_char("You feel weak, as if you haven't consumed enough life force recently.\n\r", ch);
          else if (ch->pcdata->monster_fed <= 100)
          send_to_char("You feel extremely weak, as if you haven't consumed enough life force recently.\n\r", ch);
        }
        if (ch->pcdata->ill_count >= 3000)
        send_to_char("You feel tired all the time.\n\r", ch);
        if (pact_holder(ch) && ch->pcdata->emotes[EMOTE_PACTANTAG] < 10 && ch->played / 3600 > 100)
        send_to_char("Your demonic masters are unsatisfied with the amount of suffering you've been causing.\n\r", ch);
        if (ch->modifier == MODIFIER_LUNACY && ch->pcdata->last_outside_fullmoon < current_time - (3600 * 24 * 60) && ch->played / 3600 > 100)
        send_to_char("You feel weak from it being too long since you last ran free under the full moon.\n\r", ch);
        if (ch->modifier == MODIFIER_DRINKING && ch->pcdata->last_drained_person + (3600 * 24 * 60) < current_time && ch->played / 3600 > 100)
        send_to_char("You feel weak from it being too long since you last drained someone entirely of their blood.\n\r", ch);
        if (ch->modifier == MODIFIER_RELIC && !has_relic(ch))
        send_to_char("You feel weak without your relic.\n\r", ch);

        if ((get_month() + 1 == 4 && get_day() > 19) || (get_month() + 1 == 12 && get_day() < 12) || (get_month() + 1 == 7 && get_day() > 24)) {
          if (college_student(ch, TRUE) || college_staff(ch, TRUE))
          send_to_char("Final exams are ongoing.\n\r", ch);
        }
        else if ((get_month() + 1 == 2 && get_day() > 20) || (get_month() + 1 == 9 && get_day() > 22) || (get_month() + 1 == 6 && get_day() > 23)) {
          if (college_student(ch, TRUE) || college_staff(ch, TRUE))
          send_to_char("Mid-term exams are ongoing.\n\r", ch);
        }

        /*
if(ch->faction != 0 && (get_month()+1 == 5 || get_month()+1 == 11 || get_month()+1 == 2 || get_month()+1 == 8) && get_day() >= 28)
{
send_to_char("The council of Venice is meeting to discuss
alliances.\n\r", ch);
}
*/
        if (ch->faction != 0 && get_day() >= 28) {
          int curbal = currentalliancebalance(FACTION_CULT);
          int minbal = minalliancebalance(FACTION_CULT);
          if (curbal > minbal * 2 && curbal > 4000)
          send_to_char("The council of Venice is meeting to discuss alliances.\n\r", ch);
        }

        for (int i = 0; i < 25; i++) {
          if (safe_strlen(ch->pcdata->memories[i]) > 2)
          printf_to_char(ch, "You have a vivid recollection of %s.\n\r", ch->pcdata->memories[i]);
          if (safe_strlen(ch->pcdata->repressions[i]) > 2)
          printf_to_char(ch, "You cannot remember %s.\n\r", ch->pcdata->repressions[i]);
        }

        if (IS_FLAG(ch->comm, COMM_CLEANSED))
        send_to_char("You've had a happy life with no memories of bad events or dark desires.\n\r", ch);
        if (IS_FLAG(ch->comm, COMM_FORCEDPACIFIST))
        send_to_char("You don't commit violence against others.\n\r", ch);
        if (safe_strlen(ch->pcdata->brainwash_loyalty) > 2)
        printf_to_char(ch, "You are completely loyal to %s.\n\r", ch->pcdata->brainwash_loyalty);
        if (safe_strlen(ch->pcdata->brainwash_reidentity) > 2)
        printf_to_char(ch, "You believe you are %s.\n\r", ch->pcdata->brainwash_reidentity);
        if (IS_AFFECTED(ch, AFF_TRIGGERED))
        printf_to_char(
        ch, "You are currently entirely without morality, remorse or compassion and are indifferent to the health or happiness of others. Feelings like love and affection are shadows of what they used to be.  The only enjoyment others can provide is as targets for cruelty or manipulation.\n\r");
        if (ch->pcdata->dream_identity_timer > 0 && safe_strlen(ch->pcdata->identity_world) > 2 && ch->pcdata->identity_world != NULL) {
          if (safe_strlen(ch->pcdata->dream_identity) > 2) {
            printf_to_char(ch, "You believe you are %s from the world of %s.\n\r", ch->pcdata->dream_identity, ch->pcdata->identity_world);
          }
          else if (safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_NAME)) > 2) {
            printf_to_char(
            ch, "You believe you are %s from the world of %s.\n\r", dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_NAME), ch->pcdata->identity_world);
          }
        }

        if (ch->pcdata->without_sleep > 5000)
        send_to_char("You are exhausted.\n\r", ch);
      }
      if (time_info.minute == 22) {
        if (ch->pcdata->dream_identity_timer > 0 && safe_strlen(ch->pcdata->identity_world) > 2 && ch->pcdata->identity_world != NULL) {
          if (safe_strlen(ch->pcdata->dream_identity) > 2) {
            printf_to_char(ch, "You believe you are %s from the world of %s.\n\r", ch->pcdata->dream_identity, ch->pcdata->identity_world);
          }
          else if (safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_NAME)) > 2) {
            printf_to_char(
            ch, "You believe you are %s from the world of %s.\n\r", dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_NAME), ch->pcdata->identity_world);
          }
        }
        if (guestmonster(ch) && ch->played / 3600 > time_info.monster_hours)
        send_to_char("Your monster can now delete to be reborn as a living nightmare.\n\r", ch);
      }

      if ((time_info.minute + safe_strlen(ch->name)) % 28 == 0 && number_percent() % UMAX(1, (int)sqrt(pc_pop(ch->in_room))) == 0) {
        bool ambidone = FALSE;

        if(crisis_prologue == 1 && in_lodge(ch->in_room)) {
          int ambi = number_range(0, 23);
          CHAR_DATA *to;
          for (CharList::iterator it = ch->in_room->people->begin();
          it != ch->in_room->people->end(); ++it) {
            to = *it;

            if (to == NULL || IS_NPC(to))
            continue;

            printf_to_char(to, "%s\n\r", prologue_ambiants[ambi]);
            ambidone = TRUE;
          }
        }
        else
        {
          if (ambidone == FALSE && ch->pcdata->in_domain != 0) {
            DOMAIN_TYPE *dom = vnum_domain(ch->pcdata->in_domain);
            if (dom != NULL && dom->ambients != NULL && safe_strlen(dom->ambients) > 3) {
              int value = number_range(1, linecount(dom->ambients));
              act(fetch_line(dom->ambients, value), ch, NULL, NULL, TO_ROOM);
              act(fetch_line(dom->ambients, value), ch, NULL, NULL, TO_CHAR);
              ambidone = TRUE;
            }
          }
          if (ambidone == FALSE) {
            EXTRA_DESCR_DATA *ed = NULL;
            bool tastefound = FALSE;

            for (ed = ch->in_room->extra_descr; ed && tastefound == FALSE;
            ed = ed->next) {
              if (is_name("!atmosphere", ed->keyword)) {
                tastefound = TRUE;
                int value = number_range(1, linecount(ed->description));
                act(fetch_line(ed->description, value), ch, NULL, NULL, TO_ROOM);
                act(fetch_line(ed->description, value), ch, NULL, NULL, TO_CHAR);
                ambidone = TRUE;
              }
            }



            int ambi = number_range(0, MAX_AMBIANT);
            if (can_ambiant(ch, ambi) && in_haven(ch->in_room)) {
              CHAR_DATA *to;
              for (CharList::iterator it = ch->in_room->people->begin();
              it != ch->in_room->people->end(); ++it) {
                to = *it;

                if (to == NULL || IS_NPC(to))
                continue;

                if (can_ambiant(to, ambi)) {
                  printf_to_char(to, "%s\n\r", ambiants[ambi]);
                  ambidone = TRUE;
                }
              }
            }
          }



        }
      }

      if (ch->pcdata->enlinktime > 0)
      ch->pcdata->enlinktime--;

      if (ch->race == RACE_WIGHT && time_info.minute % 51 == 0 && event_cleanse == 0 && ch->in_room != NULL && ch->in_room->vnum > 100) {
        if (IS_FLAG(ch->act, PLR_GHOST)) {
          ch->pcdata->decay_stage = 0;
          ch->pcdata->decay_timer = 0;
          ch->pcdata->max_decay_timer = 0;
        }
        else {
          ch->pcdata->decay_timer--;
          if (ch->pcdata->decay_timer <= 0) {
            if (ch->pcdata->decay_stage == 5) {
              send_to_char("You feel yourself fading away as your body decays around you.\n\r", ch);
              act("$n abruptly falls over, spasming a few times before falling unnaturally still.", ch, NULL, NULL, TO_ROOM);
              real_kill(ch, ch);
            }
            else {
              ch->pcdata->decay_stage++;
              ch->pcdata->decay_timer = 30;
              ch->pcdata->max_decay_timer = 30;
              send_to_char("`yYou feel your body decaying.`x\n\r", ch);
            }
          }
        }
      }

      if (time_info.minute % 20 == 0)
      focustick(ch);

      if (!seems_super(ch) && ch->pcdata->time_since_emote < 240 && !has_shroudmanip(ch)) {
        if (ch->pcdata->blood_potency < 150) {
          if (ch->pcdata->blood_potency <= 110) {
            if (time_info.minute % 10 == 0) {
              ch->pcdata->blood_ticks += UMIN(100, blood_tvalue(ch));
            }
          }
          else if (time_info.minute % 30 == 0)
          ch->pcdata->blood_ticks += UMIN(100, blood_tvalue(ch));

          if (ch->pcdata->blood_ticks >= 1000) {
            ch->pcdata->blood_potency++;
            ch->pcdata->blood_ticks -= 1000;
            if (is_super(ch) && ch->pcdata->blood_potency >= 70 && number_percent() % 4 == 0)
            ch->pcdata->blood_potency -= 10;
            else if (is_super(ch) && ch->pcdata->blood_potency >= 75)
            ch->pcdata->blood_potency -= 10;
          }
        }
      }
      else if (!seems_super(ch) && has_shroudmanip(ch)) {
        if (time_info.minute % 5 == 0)
        ch->pcdata->blood_ticks -= blood_tvalue(ch);
      }

      if (!IS_NPC(ch) && ch->in_room != NULL && IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
        if (!in_fight(ch) && !sin_in_room(ch->in_room) && !room_fight(ch->in_room, FALSE, FALSE, TRUE) && (ch->pcdata->patrol_status < PATROL_ATTACKSEARCHING || ch->pcdata->patrol_status > PATROL_WAGINGWAR)) {
          ch->pcdata->deepshroud++;
          if (ch->pcdata->deepshroud >= 10) {
            REMOVE_FLAG(ch->act, PLR_DEEPSHROUD);
            send_to_char("Reality starts to fade back into existance.\n\r", ch);
            act("$n fades into view.", ch, NULL, NULL, TO_ROOM);
            dact("$n fades into view.", ch, NULL, NULL, DISTANCE_MEDIUM);
            int x = 0;
            for (CharList::iterator it = ch->in_room->people->begin();
            it != ch->in_room->people->end() && x < 100; x++) {
              if ((*it) != NULL && (*it)->master == ch && IS_FLAG((*it)->act, PLR_DEEPSHROUD))
              REMOVE_FLAG((*it)->act, PLR_DEEPSHROUD);
            }
          }
        }
        else
        ch->pcdata->deepshroud = 0;
      }

      if (!IS_NPC(ch) && public_room(ch->in_room) && !IS_NPC(ch) && !is_ghost(ch) && in_haven(ch->in_room))
      last_public(ch, ch->in_room->vnum);

      if (!in_fight(ch) || ch->fight_fast == TRUE) {
        for (i = 0; i < 10; i++) {
          if (ch->pcdata->augdisc_timer[i] > 0)
          ch->pcdata->augdisc_timer[i]--;

          if (ch->pcdata->augskill_timer[i] > 0)
          ch->pcdata->augskill_timer[i]--;
        }
      }

      if (!IS_FLAG(ch->act, PLR_SINSPIRIT) && !IS_FLAG(ch->act, PLR_NOSAVE) && ch->desc->connected == CON_PLAYING) {
        if (ch->pcdata->account == NULL && ch->desc != NULL && ch->desc->account != NULL)
        ch->pcdata->account = ch->desc->account;

        if (ch->pcdata->account != NULL && safe_strlen(ch->pcdata->account_name) < 2 && ch->pcdata->account->name != NULL) {
          free_string(ch->pcdata->account_name);
          ch->pcdata->account_name = str_dup(ch->pcdata->account->name);
        }

        if (ch->pcdata->account == NULL && safe_strlen(ch->pcdata->account_name) > 2 && ch->desc != NULL)
        load_account_obj(ch->desc, ch->pcdata->account_name);

        if (ch->pcdata->account != NULL)
        add_char_to_account(ch, ch->pcdata->account);
      }

      if (ch->in_room != NULL && in_haven(ch->in_room) && !crazy_town() && !is_gm(ch) && !institute_room(ch->in_room)) {

        wanted_check(ch);
        if (sunphase(ch->in_room) > 2 && sunphase(ch->in_room) < 7) {
          if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && guestmonster(ch)) {
            ch->pcdata->public_alarm += 2;

            send_to_char("Your monsterous appearance seems to be causing some public alarm.\n\r", ch);
          }
          else if (institute_room(ch->in_room) && has_institute_contraband(ch) && !IS_FLAG(ch->act, PLR_BOUND)) {
            ch->pcdata->public_alarm += 2;
            send_to_char("Nearby orderlies start to look at you suspiciously, as if suspecting contraband.\n\r", ch);
            if (ch->pcdata->public_alarm >= 30) {
              remove_institute_contraband(ch);

              if (ch->pcdata->institute_contraband_violations == 3) {
                ch->pcdata->institute_contraband_violations = 0;
                char buf[MSL];
                sprintf(buf, "PRISONER: %s autocommitted for weapon possession.", ch->name);
                log_string(buf);

                INSTITUTE_TYPE *ins;
                bool found = FALSE;
                for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
                it != InVect.end(); ++it) {
                  ins = (*it);
                  if (!str_cmp(ins->name, ch->name)) {
                    ins->asylum_prestige = UMAX(ins->asylum_prestige, 1);
                    ins->asylum_inactive = 0;
                    found = TRUE;
                  }
                }
                if (found == FALSE) {
                  ins = new_institute();
                  free_string(ins->name);
                  ins->name = str_dup(ch->name);
                  ins->asylum_prestige = 1;
                  InVect.push_back(ins);
                }
                send_to_char("You're apprehended for possession of dangerous contraband on campus.\n\r", ch);
                commit_char(ch);
                for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
                it != InVect.end(); ++it) {
                  ins = (*it);
                  if (!str_cmp(ins->name, ch->name)) {
                    ins->asylum_status = ASYLUM_COLLEGECOMMIT;
                    free_string(ins->asylum_pending_notes);
                    ins->asylum_pending_notes =
                    str_dup("`RDetained on premises for possession of dangerous contraband.`x\n\n\r");
                  }
                }
              }
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && ch->pcdata->public_alarm >= 0 && !is_ghost(ch) && is_animal(ch) && (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >=
                ANIMAL_MANSIZED || IS_AFFECTED(ch, AFF_NOTICED) || get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_HYBRID)) {
            ch->pcdata->public_alarm += 2;
            send_to_char("Your presence seems to be causing some alarm.\n\r", ch);
            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Animal control comes and takes you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Animal control comes and takes $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Animal control comes and takes $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);
              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && visiblyarmed(ch) && ch->pcdata->public_alarm >= 0 && ch->race != RACE_DEPUTY && !is_ghost(ch) && !is_animal(ch)) {
            ch->pcdata->public_alarm += 2;

            send_to_char("Your weapons seem to be causing some public alarm.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);
              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && visiblybloody(ch) && ch->pcdata->public_alarm >= 0 && !in_hospital(ch) && ch->wounds < 3 && !is_ghost(ch) && !is_animal(ch)) {
            ch->pcdata->public_alarm += 2;

            send_to_char("The blood on you seems to be causing some public alarm.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);

              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && ch->race == RACE_DEMIDEMON && (!is_covered(ch, COVERS_EYES) || !is_covered(ch, COVERS_THIGHS) || !is_covered(ch, COVERS_HANDS) || !is_covered(ch, COVERS_UPPER_CHEST) || !is_covered(ch, COVERS_FEET)) && ch->pcdata->public_alarm >= 0 && !in_hospital(ch) && ch->wounds < 3 && !is_ghost(ch) && !is_animal(ch)) {
            ch->pcdata->public_alarm += 2;

            send_to_char("Your monsterous appearance seems to be causing some public alarm.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);

              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && get_skill(ch, SKILL_WINGS) > 0 && !is_covered(ch, COVERS_UPPER_BACK) && ch->pcdata->public_alarm >= 0 && !in_hospital(ch) && ch->wounds < 3 && !is_ghost(ch) && !is_animal(ch)) {
            ch->pcdata->public_alarm += 2;

            send_to_char("Your monsterous appearance seems to be causing some public alarm.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);

              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && !is_covered(ch, COVERS_GROIN) && ch->pcdata->public_alarm >= 0 && !in_hospital(ch) && ch->wounds < 3 && !is_ghost(ch) && ch->shape == SHAPE_HUMAN) {
            ch->pcdata->public_alarm += 2;

            send_to_char("Your nudity seems to be causing some public alarm.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the disturbed population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);
              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && ch->pcdata->public_alarm >= 0 && !in_hospital(ch) && ch->wounds < 3 && !is_ghost(ch) && ch->shape != SHAPE_HUMAN && !is_animal(ch)) {
            ch->pcdata->public_alarm += 20;

            send_to_char("Your inhuman shape starts causing panic.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);
              ch->pcdata->public_alarm = -60;
            }
          }
          else if (public_room(ch->in_room) && !IS_FLAG(ch->act, PLR_SHROUD) && !in_fight(ch) && ch->pcdata->public_alarm >= 0 && !in_hospital(ch) && ch->wounds < 3 && !is_ghost(ch) && is_masked(ch) && !is_animal(ch) && !nighttime(ch->in_room)) {
            ch->pcdata->public_alarm += 2;

            send_to_char("Your mask seems to be causing some public alarm.\n\r", ch);

            if (ch->pcdata->public_alarm >= 30 && !guestmonster(ch)) {
              act("Some deputies come an escort you away from the panicking population.", ch, NULL, NULL, TO_CHAR);
              act("Some deputies come and escort $n away.", ch, NULL, NULL, TO_ROOM);
              dact("Some deputies come and escort $n away.", ch, NULL, NULL, DISTANCE_MEDIUM);
              auto_arrest(ch);

              ch->pcdata->public_alarm = -60;
            }
          }
        }

        if (ch->pcdata->public_alarm < 0 && in_sheriff(ch->in_room) && !IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
          if (number_percent() % 3 == 0 || !under_opression(ch))
          ch->pcdata->public_alarm++;
          if (ch->pcdata->public_alarm >= 0) {
            act("The deputies let $n go.", ch, NULL, NULL, TO_ROOM);
            char_from_room(ch);
            char_to_room(ch, get_room_index(3518));
            if (IS_FLAG(ch->act, PLR_BOUND))
            REMOVE_FLAG(ch->act, PLR_BOUND);
            if (IS_FLAG(ch->act, PLR_BOUNDFEET))
            REMOVE_FLAG(ch->act, PLR_BOUNDFEET);

            if (under_opression(ch))
            act("The deputies let you go, explaining your arrest was a clerical error.", ch, NULL, NULL, TO_CHAR);
            else
            act("The deputies let you go.", ch, NULL, NULL, TO_CHAR);
          }
        }
        else if (ch->pcdata->public_alarm == 0 && in_sheriff(ch->in_room) && ch->race != RACE_DEPUTY && deputy_pop(ch->in_room) <= 0 && (in_cell(ch) || is_helpless(ch)) && !IS_FLAG(ch->act, PLR_DEEPSHROUD)) {
          act("The deputies let $n go.", ch, NULL, NULL, TO_ROOM);
          char_from_room(ch);
          char_to_room(ch, get_room_index(3518));
          act("The deputies let you go.", ch, NULL, NULL, TO_CHAR);
          if (IS_FLAG(ch->act, PLR_BOUND))
          REMOVE_FLAG(ch->act, PLR_BOUND);
          if (IS_FLAG(ch->act, PLR_BOUNDFEET))
          REMOVE_FLAG(ch->act, PLR_BOUNDFEET);

        }
        else if (ch->pcdata->public_alarm > 0)
        ch->pcdata->public_alarm--;
      }

      if (ch->pcdata->icooldown > 0)
      ch->pcdata->icooldown--;
      if (ch->pcdata->tcooldown > 0)
      ch->pcdata->tcooldown--;

      if (time_info.minute % 45 == 0) {
        if (safe_strlen(ch->pcdata->enthralled) > 2) {
          if (get_char_world_pc(ch->pcdata->enthralled) != NULL && get_char_world_pc(ch->pcdata->enthralled)->skills[SKILL_ENTHRALL] <
              1) {
            free_string(ch->pcdata->enthralled);
            ch->pcdata->enthralled = str_dup("");
          }
          else
          printf_to_char(ch, "You have to obey your master.\n\r");
        }
        if (safe_strlen(ch->pcdata->enraptured) > 2) {
          if (get_char_world_pc(ch->pcdata->enraptured) != NULL && get_char_world_pc(ch->pcdata->enraptured)->skills[SKILL_ENRAPTURE] <
              1) {
            free_string(ch->pcdata->enraptured);
            ch->pcdata->enraptured = str_dup("");
          }
          else
          printf_to_char(ch, "You have to make your adored happy.\n\r");
        }
      }

      if (time_info.minute % 15 == 0 && ch->pcdata->adventure_fatigue > 0) {
        if (ch->pcdata->adventure_fatigue > 300)
        ch->pcdata->adventure_fatigue--;
        else if (ch->pcdata->adventure_fatigue > 100 && number_percent() % 3 == 0)
        ch->pcdata->adventure_fatigue--;
        else if (number_percent() % 8 == 0)
        ch->pcdata->adventure_fatigue--;
      }

      if (ch->in_room != NULL) {
        if (ch->pcdata->bloodaura > 0 && ch->pcdata->time_since_emote < 30)
        ch->pcdata->bloodaura--;
      }

      /*
if(ch->pcdata->fatigue > 50)
ch->pcdata->fatigue = 50;
else if(ch->pcdata->fatigue > -1000)
ch->pcdata->fatigue--;
else if(ch->pcdata->fatigue > -2000 && number_percent() % 2 == 0)
ch->pcdata->fatigue--;
else if(ch->pcdata->fatigue > -3000 && number_percent() % 4 == 0)
ch->pcdata->fatigue--;
else if(number_percent() % 8 == 0)
ch->pcdata->fatigue--;
*/

      if (time_info.minute % 3 == 0)
      copcheck(ch);

      house_security_update(ch);

      if (time_info.minute % 5 == 0) {
        for (int x = 0; x < 10; x++) {
          if (ch->pcdata->conditions[x] > 0)
          ch->pcdata->conditions[x]--;
        }
      }

      if (is_vampire(ch) && !IS_NPC(ch) && time_info.minute % 25 == 0 && event_cleanse == 0) {
        ch->pcdata->vamp_fullness--;
        if (ch->pcdata->vamp_fullness >= 8)
        start_vomit(ch);
      }

      ch->pcdata->without_sleep++;

      ch->activeat = current_time;

      if (time_info.minute % 4 == 0) {
        if (!is_ghost(ch)) { // Ghosts shouldn't get any dirtier - Discordance
          if (!IS_NPC(ch) && ch->in_room != NULL && ch->pcdata->dirt[0] < 500 && (ch->in_room->sector_type == SECT_PARK || ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_TUNNELS || ch->in_room->sector_type == SECT_CAVE || ch->in_room->sector_type == SECT_BASEMENT || ch->in_room->sector_type == SECT_ROCKY)) {
            ch->pcdata->dirt[0]++;
          }
        }
      }

      if (sunphase(ch->in_room) == 1 && sunphase_delayed(ch->in_room) == 0)
      send_to_char("The eastern horizon starts to lighten.\n\r", ch);
      if (sunphase(ch->in_room) == 2 && sunphase_delayed(ch->in_room) == 1)
      send_to_char("The sun dawns over the eastern horizon.\n\r", ch);
      if (sunphase(ch->in_room) == 4 && sunphase_delayed(ch->in_room) == 3)
      send_to_char("The noontime sun shines down on the city.\n\r", ch);
      if (sunphase(ch->in_room) == 7 && sunphase_delayed(ch->in_room) == 6)
      send_to_char("The sun dips below the western horizon.\n\r", ch);

      if (time_info.minute == 30) {
        if (ch->pcdata->atolerance > 25000)
        ch->pcdata->atolerance = 3000;
        if (ch->pcdata->atolerance < 0)
        ch->pcdata->atolerance = 0;

        if (ch->pcdata->habit[HABIT_DRINKING] == 0 && ch->pcdata->atolerance > 0)
        ch->pcdata->atolerance--;
        if (ch->pcdata->habit[HABIT_DRINKING] == 1 && ch->pcdata->atolerance > 1000)
        ch->pcdata->atolerance--;
        if (ch->pcdata->habit[HABIT_DRINKING] == 2 && ch->pcdata->atolerance > 2500)
        ch->pcdata->atolerance--;
        if (ch->pcdata->habit[HABIT_DRINKING] == 3 && ch->pcdata->atolerance > 5000)
        ch->pcdata->atolerance--;
        if (ch->pcdata->habit[HABIT_DRINKING] == 4 && ch->pcdata->atolerance > 10000)
        ch->pcdata->atolerance--;

        if (ch->pcdata->habit[HABIT_DRINKING] == 1 && ch->pcdata->atolerance < 500)
        ch->pcdata->atolerance++;
        if (ch->pcdata->habit[HABIT_DRINKING] == 2 && ch->pcdata->atolerance < 1500)
        ch->pcdata->atolerance++;
        if (ch->pcdata->habit[HABIT_DRINKING] == 3 && ch->pcdata->atolerance < 3000)
        ch->pcdata->atolerance++;
        if (ch->pcdata->habit[HABIT_DRINKING] == 4 && ch->pcdata->atolerance < 7500)
        ch->pcdata->atolerance++;
      }

      if (time_info.minute % 15 == 0 && event_cleanse != 1) {
        if (is_vampire(ch) && sunphase(ch->in_room) > 1 && sunphase(ch->in_room) < 7 && get_skill(ch, SKILL_SUNRESIST) < 3)
        send_to_char("The daylight fatigue wears on you.\n\r", ch);
        if (!is_werewolf(ch) && full_moon() == TRUE && IS_FLAG(ch->act, PLR_WOLFBIT) && ch->wounds == 0 && (sunphase(NULL) < 2 || sunphase(NULL) == 7) && get_tier(ch) < 3 && ch->race != RACE_FACULTY) {
          if (get_tier(ch) == 1) {
            ch->race = RACE_NEWWEREWOLF;
            ch->modifier = MODIFIER_NEW;
          }
          else if (get_tier(ch) == 2 && ch->skills[SKILL_MENTALDISCIPLINE] <= 0) {
            ch->race = RACE_NEWWEREWOLF;
            ch->modifier = 0;
          }
          else if (get_tier(ch) == 3 || (get_tier(ch) == 2 && ch->skills[SKILL_MENTALDISCIPLINE] > 0)) {
            ch->race = RACE_NEWWEREWOLF;
            ch->modifier = MODIFIER_STRONG;
          }
        }
        if (is_werewolf(ch) && full_moon() == TRUE && (sunphase(NULL) < 2 || sunphase(NULL) == 7)) {
          if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && ch->shape == SHAPE_WOLF && ch->pcdata->habit[HABIT_LUNACY] >= 2)
          ch->pcdata->last_outside_fullmoon = current_time;

          if (ch->pcdata->lastshift > current_time - (3600 * 24 * 7 * 10))
          send_to_char("You wrestle with your sanity as lunacy grips you.\n\r", ch);
          else
          send_to_char("You partially resist the lunacy that grips you.\n\r", ch);

          if (ch->pcdata->habit[HABIT_LUNACY] == 1) {
            if (ch->in_room != NULL && ch->pcdata->lastaccident + (3600 * 24 * 7 * 4 * 6) <
                current_time && (ch->in_room->area->vnum >= 16 && ch->in_room->area->vnum <= 21)) {
              werewolf_live_kill(ch);
            }
          }
        }
        if (is_werewolf(ch) && ch->pcdata->lunacy_curse > 0) {
          if (ch->pcdata->lastshift > current_time - (3600 * 24 * 7 * 10))
          send_to_char("You wrestle with your sanity as lunacy grips you.\n\r", ch);
          else
          send_to_char("You partially resist the lunacy that grips you.\n\r", ch);

          if (nighttime(ch->in_room) && ch->pcdata->lastshift > current_time - (3600 * 24 * 7 * 10) && pc_pop(ch->in_room) < 2 && (ch->in_room->area->vnum >= 16 && ch->in_room->area->vnum <= 21) && ch->pcdata->lastaccident + (3600 * 24 * 7 * 4 * 6) < current_time && ch->pcdata->lunacy_curse < 90) {
            werewolf_live_kill(ch);
          }
        }
      }
    }

    if (time_info.minute >= 60) {
      time_t east_time;

      time_info.minute = 0;
      time_info.hour++;
      east_time = current_time;
      sprintf(buf, "%s", (char *)ctime(&east_time));

      log_string("Game Hour for LAG check");
    }
    if (time_info.hour >= 24) {
      time_info.hour = 0;
      time_info.day++;

      if (time_info.day % 6 == 0)
      market_update();
    }
    if (time_info.day > 30) {
      time_info.day = 1;
      time_info.month++;
    }

    if (time_info.month >= 13) {
      time_info.month = 1;
      time_info.year++;
    }

    /*
if(time_info.minute == 0)
{
for(DescList::iterator it = descriptor_list.begin();
it != descriptor_list.end(); ++it)
{
DESCRIPTOR_DATA *d = *it;

if ( d->connected == CON_PLAYING
&&   IS_AWAKE(d->character) )
{
buf[0] = '\0';

free_string(d->character->last_ip);
d->character->last_ip = str_dup(d->host);

if(buf != '\0')
send_to_char(buf, d->character);
}  // End If
} // End For
}
*/
    return;
  }

  void room_exit_update(void) {
    EXIT_DATA *pExit = NULL, *nExit = NULL;
    //    ROOM_INDEX_DATA *pRoom = NULL, *nRoom = NULL;
    //    AFFECT_DATA *paf = NULL, *paf_next = NULL;

    for (pExit = exit_upd_list; pExit; pExit = nExit) {
      nExit = pExit->next_upd;
      /*
for(paf= pExit->affected; paf; paf = paf_next)
{
paf_next    = paf->next;


if ( --paf->duration >= 0 )
{
if (number_range(0, 4) == 0 && paf->level > 0)
paf->level--;  // spell strength fades with time
}
}
*/
    }
    /*
for(pRoom = room_upd_list; pRoom; pRoom = nRoom)
{
nRoom = pRoom->next_upd;

for(paf = pRoom->affected; paf; paf = paf_next)
{
paf_next    = paf->next;

if ( paf->duration == -1 )
continue;

if ( --paf->duration >= 0 )
{
if (number_range(0, 4) == 0 && paf->level > 0)
paf->level--;  // spell strength fades with time
}
else
{
send_to_char("The affect has ended!!\n\r", paf->caster);
}
}
}
*/
    return;
  }

  const char *cover_names[MAX_COVERS] = {
    "hands",      "forearms", "upper arms", "feet",        "lower legs", "knees",      "thighs",   "groin",      "rear",        "lower back", "upper back", "stomach",  "chest",      "upper chest", "neck", "face",       "hair",     "forehead"};

  void shower_update(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int iWear, i, dirt = 0;
    AFFECT_DATA af;
    if (ch == NULL || IS_NPC(ch))
    return;

    if (is_ghost(ch))
    return;

    if (!IS_FLAG(ch->act, PLR_SHOWERING) && ch->pcdata->wetness > 0) {
      ch->pcdata->wetness--;
      if (ch->pcdata->wetness == 0)
      send_to_char("You are completely dry.\n\r", ch);
    }
    if (IS_FLAG(ch->act, PLR_SHOWERING)) {
      ch->pcdata->wetness = UMIN(30, ch->pcdata->wetness + 5);
      if (!IS_SET(ch->in_room->room_flags, ROOM_SHOWER)) {
        send_to_char("You step out of the bath.\n\r", ch);
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        if (ch->sex == SEX_MALE)
        af.duration = 30;
        else
        af.duration = 100;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_WET;
        affect_to_char(ch, &af);
        REMOVE_FLAG(ch->act, PLR_SHOWERING);
        return;
      }
      for (i = 0; i < MAX_COVERS; i++) {
        if (ch->pcdata->blood[i] > 0)
        dirt++;
        if (ch->pcdata->dirt[i] > 0)
        dirt++;
      }
      if (dirt <= 0) {
        send_to_char("You continue to soak in the warm water.\n\r", ch);
        act("$n continues to soak in the warm water.", ch, NULL, NULL, TO_ROOM);
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        if (obj->item_type != ITEM_CLOTHING)
        continue;

        /*
if(can_see_wear(ch, iWear))
{
if(obj->value[3] > 0)
obj->value[3]--;
}
if(obj->value[4] < 400)
obj->value[4] += 3;
*/
      }
    }
  }
  void update_clothes(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;

    if (IS_NPC(ch) || get_lvl(ch) < 5 || IS_IMMORTAL(ch))
    return;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj == NULL || !obj)
      continue;

      if (obj->cost > 100000 || obj->cost < 0)
      continue;

      if (obj->item_type != ITEM_CLOTHING)
      continue;

      if (obj->value[3] > 600)
      obj->value[3] = 0;

      if (obj->value[4] > 100)
      obj->value[4] = 20;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (obj->value[3] > 0)
        obj->value[3]--;
        if (obj->value[4] > 0)
        obj->value[4]--;
      }
    }
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (obj->item_type != ITEM_CLOTHING)
      continue;

      //  	if(ch->pcdata->wetness > obj->value[4])
      //	    obj->value[4] = ch->pcdata->wetness;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (obj->value[3] > 0)
        obj->value[3]--;
        if (obj->value[4] > 0)
        obj->value[4]--;

      }
      else if (!can_see_wear(ch, iWear)) {
        if (obj->value[4] > 0 && number_percent() % 13 == 0)
        obj->value[4]--;
      }
      else {
        if (obj->value[4] > 0)
        obj->value[4]--;
      }
    }
  }

  int get_drunk(CHAR_DATA *ch) {
    int drunk = ch->pcdata->conditions[COND_DRUNK];

    drunk -= (int)(sqrt((double)ch->pcdata->atolerance));

    if (ch->sex == SEX_FEMALE)
    drunk = drunk * 6 / 5;

    if (is_undead(ch))
    drunk = drunk / 2;
    return drunk;
  }

  void fight_update(void) {
    CHAR_DATA *ch;

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      ch = *it;
      ++it;

      if (ch == NULL || !ch)
      continue;

      if (ch->race < 0 || ch->race > 200 || ch->sex < 0 || ch->sex > 10)
      continue;

      if (ch->in_room == NULL || !in_fight(ch))
      continue;

      if (ch->fight_fast == TRUE) {
        for (int i = 0; i < 30; i++) {
          if (ch->caff_duration[i] > 0) {
            ch->caff_duration[i]--;
            if (ch->caff_duration[i] <= 0 && ch->caff[i] == CAFF_RESISTPAIN)
            ch->hit = UMAX(0, ch->hit - 100);
          }
        }
      }

      if (IS_NPC(ch)) {
        if (ch->killed == TRUE) {
          raw_kill(ch, ch);
          continue;
        }
        if (ch == NULL || ch->in_room == NULL)
        continue;

        if (!in_fight(ch))
        continue;

        if (ch->fight_fast == TRUE) {
          ch->move_timer--;
          if (ch->move_timer <= 0)
          npc_combat_move(ch);

          ch->attack_timer--;
          if (ch->attack_timer <= 0) {
            if (IS_FLAG(ch->act, ACT_COMBATOBJ)) {
              if (ch->pIndexData->vnum == COBJ_FRAG)
              explode_grenade(ch);

              ch->wounds = 4;
              char_from_room(ch);
              char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
              ch->ttl = 2;
            }
            else
            npc_combat_attack(ch);
          }
        }
        else {
          if (ch->fight_current == ch && ch->actiontimer > 0) {
            ch->actiontimer--;
            if (ch->actiontimer <= 0) {
              ch->attacking = 0;
              next_attacker(ch, FALSE);
            }
          }
        }
      }
      else {
        if (ch->fight_fast == TRUE) {
        }
        else {
          if (ch->fight_current == ch && ch->actiontimer > 0) {
            ch->actiontimer -= 1;
            if (ch->actiontimer <= 0) {
              ch->pcdata->autoskip = 1;
              ch->attacking = 0;
              next_attacker(ch, FALSE);
              send_to_char("You skip your turn, and will now automatically skip turns, type autoskip to toggle this off.\n\r", ch);
            }
            else if (ch->actiontimer == 120) {
              if (IS_NPC(ch) && ch->controled_by != NULL)
              send_to_char("Two minutes left to write your combat emote.\n\r", ch->controled_by);
              else
              send_to_char("Two minutes left to write your combat emote.\n\r", ch);
            }
            else if (ch->actiontimer == 60) {
              if (IS_NPC(ch) && ch->controled_by != NULL)
              send_to_char("One minute left to write your combat emote.\n\r", ch->controled_by);
              else
              send_to_char("One minute left to write your combat emote.\n\r", ch);
            }
            else if (ch->actiontimer == 30) {
              if (IS_NPC(ch) && ch->controled_by != NULL)
              send_to_char("Thirty seconds left to write your combat emote.\n\r", ch->controled_by);
              else
              send_to_char("Thirty seconds left to write your combat emote.\n\r", ch);
            }
          }
        }
      }
    }
  }

  void give_bonus(CHAR_DATA *ch, int type) {
    if (type == 0) {
      ch->pcdata->xpbonus += 500;
      ch->pcdata->rpxpbonus += 10;
    }
    else {
      ch->pcdata->xpbonus -= 500;
      ch->pcdata->rpxpbonus -= 10;
    }
  }

  void process_feedback(CHAR_DATA *ch) {
    int i, j;
    char buf[MSL];
    if (ch->pcdata->feedbackcooldown > 0)
    ch->pcdata->feedbackcooldown--;

    for (i = 0; i < 10; i++) {
      if (ch->pcdata->feedbacktim[i] > 0) {
        ch->pcdata->feedbacktim[i]--;
        if (ch->pcdata->feedbacktim[i] == 0) {
          if (ch->pcdata->feedbackbon[i] > 0) {
            for (j = 0; j < ch->pcdata->feedbackbon[i]; j++)
            give_bonus(ch, 0);
            printf_to_char(ch, "You have receive positive RP feedback: %s\n\r", ch->pcdata->feedbackinc[i]);
            sprintf(buf, "%s\nPositive: %s", ch->pcdata->feedback, ch->pcdata->feedbackinc[i]);
            free_string(ch->pcdata->feedback);
            ch->pcdata->feedback = str_dup(buf);
          }
          else if (ch->pcdata->feedbackbon[i] < 0) {
            for (j = 0; j > ch->pcdata->feedbackbon[i]; j--)
            give_bonus(ch, 1);
            printf_to_char(ch, "You have receive negative RP feedback: %s\n\r", ch->pcdata->feedbackinc[i]);
            sprintf(buf, "%s\nNegative: %s", ch->pcdata->feedback, ch->pcdata->feedbackinc[i]);
            free_string(ch->pcdata->feedback);
            ch->pcdata->feedback = str_dup(buf);
          }
          else {
            printf_to_char(ch, "You have receive neutral RP feedback: %s\n\r", ch->pcdata->feedbackinc[i]);
            sprintf(buf, "%s\nNeutral: %s", ch->pcdata->feedback, ch->pcdata->feedbackinc[i]);
            free_string(ch->pcdata->feedback);
            ch->pcdata->feedback = str_dup(buf);
          }
        }
      }
    }
  }

  int spay(CHAR_DATA *ch) {
    int val = 20;

    if ((ch->pcdata->job_type_one != JOB_COVER || safe_strlen(ch->pcdata->job_title_one) < 2) && (val > 20))
    val /= 2;
    else if (ch->pcdata->job_delay_one < current_time - (3600 * 24 * 3))
    val = val * 4 / 5;
    return val;
  }

  int clan_wealth_mod(CHAR_DATA *ch, FACTION_TYPE *fac)
  {

    if(fac == NULL)
    return 0;

    if (fac->axes[AXES_MATERIAL] == AXES_FARLEFT)
    return -5;
    else if (fac->axes[AXES_MATERIAL] == AXES_MIDLEFT)
    return -3;
    else if (fac->axes[AXES_MATERIAL] == AXES_NEARLEFT)
    return -1;
    else if (fac->axes[AXES_MATERIAL] == AXES_FARRIGHT)
    return 5;
    else if (fac->axes[AXES_MATERIAL] == AXES_MIDRIGHT)
    return 3;
    else if (fac->axes[AXES_MATERIAL] == AXES_NEARRIGHT)
    return 1;
    return 0;
  }


  int soc_wealth_mod(CHAR_DATA *ch, bool invert)
  {
    int vcult = 0;
    int vsect = 0;
    int vfac = 0;
    FACTION_TYPE *cult = clan_lookup(ch->fcult);
    FACTION_TYPE *sect = clan_lookup(ch->fsect);
    FACTION_TYPE *core = clan_lookup(ch->fcore);
    int count = 0;
    if(cult != NULL)
    {
      count++;
      vcult = clan_wealth_mod(ch, cult);
    }
    if(sect != NULL)
    {
      count++;
      vsect = clan_wealth_mod(ch, sect);
    }
    if(core != NULL)
    {
      count++;
      vfac = clan_wealth_mod(ch, core);
    }
    if(invert == TRUE)
    {
      vfac *= -1;
      vsect *= -1;
      vcult *= -1;
    }
    int vone = 0;
    int vtwo = 0;
    int vthree = 0;

    if (vcult >= vsect && vcult >= vfac) {
      vone = vcult;
      if (vsect >= vfac) {
        vtwo = vsect;
        vthree = vfac;
      }
      else {
        vtwo = vfac;
        vthree = vsect;
      }
    }
    else if (vsect >= vcult && vsect >= vfac) {
      vone = vsect;
      if (vcult >= vfac) {
        vtwo = vcult;
        vthree = vfac;
      }
      else {
        vtwo = vfac;
        vthree = vcult;
      }
    }
    else {
      vone = vfac;
      if (vcult >= vsect) {
        vtwo = vcult;
        vthree = vsect;
      }
      else {
        vtwo = vsect;
        vthree = vcult;
      }
    }

    int rval = (vone*4 + vtwo*2 + vthree)/7;
    return rval;
  }



  bool payday(CHAR_DATA *ch) {
    char buf[MSL];

    if (IS_FLAG(ch->act, PLR_GM)) {
      ch->pcdata->total_money += 20000;
      ch->pcdata->last_paid += (3600 * 24 * 2);
      return FALSE;
    }
    int amount = 0;

    if (ch->pcdata->emotes[EMOTE_PAY] < 5 && ch->pcdata->total_money + ch->money > 100000) {
      sprintf(buf, "CASH: %s no roleplay.\n\r", ch->name);
      log_string(buf);

      ch->pcdata->last_paid += (3600 * 24 * 2);
      return FALSE;
    }
    ch->pcdata->account->tier_count += get_tier(ch);
    ch->pcdata->account->total_count += 1;
    time_info.tweek_tier += get_tier(ch);
    time_info.tweek_total += 1;

    int maxpay = 1000000 * get_skill(ch, SKILL_WEALTH);

    if (ch->pcdata->total_money > 0)
    amount += UMIN(ch->pcdata->total_money, maxpay) *
    (get_skill(ch, SKILL_WEALTH)) / 10000;

    if (get_skill(ch, SKILL_WEALTH) == 0)
    amount += 200;
    else if (get_skill(ch, SKILL_WEALTH) == 1)
    amount += 400;
    else if (get_skill(ch, SKILL_WEALTH) == 2)
    amount += 700;
    else if (get_skill(ch, SKILL_WEALTH) == 3)
    amount += 950;
    else if (get_skill(ch, SKILL_WEALTH) == 4)
    amount += 1500;
    else if (get_skill(ch, SKILL_WEALTH) == 5)
    amount += 2900;

    if (ch->pcdata->job_type_one == JOB_MIGRANT) {
      ch->pcdata->job_type_one = JOB_UNEMPLOYED;
    }
    if (ch->pcdata->job_type_two == JOB_MIGRANT)
    ch->pcdata->job_type_two = JOB_UNEMPLOYED;

    if (ch->race != RACE_DEPUTY) {
      int mult = 20;
      if (ch->pcdata->job_type_one == JOB_CLINIC)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_one == JOB_COLLEGE)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_one == JOB_EMPLOYEE)
      mult = UMAX(mult, 20);
      if (ch->pcdata->job_type_one == JOB_FULLSTUDENT)
      mult = UMAX(mult, 65);
      if (ch->pcdata->job_type_two == JOB_PARTSTUDENT)
      mult = UMAX(mult, 65);
      if (ch->pcdata->job_type_one == JOB_PARTCOMMUTE)
      mult = UMAX(mult, 75);
      if (ch->pcdata->job_type_two == JOB_PARTCOMMUTE)
      mult = UMAX(mult, 75);
      if (ch->pcdata->job_type_one == JOB_FULLCOMMUTE)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_two == JOB_FULLCOMMUTE)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_one == JOB_FULLEMPLOY && ch->pcdata->job_delay_one > current_time - (3600 * 24 * 7)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_one)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_one))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 80);
        else
        mult = UMAX(mult, 100);
      }
      if (ch->pcdata->job_type_two == JOB_FULLEMPLOY && ch->pcdata->job_delay_two > current_time - (3600 * 24 * 7)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_two)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_two))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 80);
        else
        mult = UMAX(mult, 100);
      }
      if (ch->pcdata->job_type_one == JOB_PARTEMPLOY && ch->pcdata->job_delay_one > current_time - (3600 * 24 * 7)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_one)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_one))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 55);
        else
        mult = UMAX(mult, 85);
      }
      if (ch->pcdata->job_type_two == JOB_PARTEMPLOY && ch->pcdata->job_delay_two > current_time - (3600 * 24 * 7)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_two)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_two))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 55);
        else
        mult = UMAX(mult, 85);
      }
      amount = amount * mult / 100;
    }

    if (ch->pcdata->lfcount > 0) {
      int avglf = ch->pcdata->lftotal / ch->pcdata->lfcount;
      amount = amount * avglf / 100;
      ch->pcdata->lftotal = 0;
      ch->pcdata->lfcount = 0;
    }
    bool rval = TRUE;
    if (ch->pcdata->lastnotalone < current_time - (3600 * 24 * 14)) {
      amount = amount / 10;
      sprintf(buf, "CASH: %s idle.\n\r", ch->name);
      log_string(buf);
      rval = FALSE;
      sprintf(
      buf, "%s\nYou are paid less for not having RPd much in the last two weeks.", ch->pcdata->messages);
      free_string(ch->pcdata->messages);
      ch->pcdata->messages = str_dup(buf);
      send_to_char(buf, ch);
    }

    if (IS_FLAG(ch->act, PLR_NOPAY)) {
      amount = 0;
      REMOVE_FLAG(ch->act, PLR_NOPAY);
    }
    if (ch->fcore != 0) {
      if (ch->pcdata->intel >= 5000)
      amount += 50;

      amount += faction_pay(ch, ch->fcore) / 2;
    }
    if (ch->fcult != 0) {
      amount += faction_pay(ch, ch->fcult) / 2;
    }

    if (ch->fsect != 0) {
      amount += faction_pay(ch, ch->fsect) / 2;
    }

    amount = amount * (100 + soc_wealth_mod(ch, FALSE)) / 100;


    int deduct = 0;
    if (ch->pcdata->total_money < 0)
    deduct -= ch->pcdata->total_money / 2000;

    deduct += ch->pcdata->habit[HABIT_SMOKING] * 5;
    deduct += ch->pcdata->habit[HABIT_DRUGS] * 5;

    if (ch->pcdata->habit[HABIT_EATING] == 2)
    deduct += 10;
    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    if(char_profile != NULL && char_profile->plus == 1)
    deduct += 12;

    deduct += garage_charge(ch);
    ch->pcdata->total_money += (amount * 100);
    ch->pcdata->total_money -= (deduct * 100);
    sprintf(buf, "CASH: %s gets paid %d and loses %d.\n\r", ch->name, amount * 100, deduct * 100);
    log_string(buf);
    sprintf(buf, "%s\nYou receive a cheque for $%d, minus $%d in expenses.", ch->pcdata->messages, amount, deduct);
    free_string(ch->pcdata->messages);
    ch->pcdata->messages = str_dup(buf);
    send_to_char(buf, ch);

    if (ch->faction != 0)
    reset_faction_pay(ch, ch->faction);
    if (ch->factiontwo != 0)
    reset_faction_pay(ch, ch->factiontwo);

    shop_profits(ch);
    return rval;
  }

  _DOFUN(do_superpay) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);
    if ((victim = get_char_world(ch, arg)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    if (IS_NPC(victim))
    return;
    payday(victim);
    send_to_char("Done.\n\r", ch);
  }

  int estimated_expenses(CHAR_DATA *ch) {
    int deduct = 0;
    if (ch->pcdata->total_money < 0)
    deduct -= ch->pcdata->total_money / 2000;

    deduct += ch->pcdata->habit[HABIT_SMOKING] * 5;
    deduct += ch->pcdata->habit[HABIT_DRUGS] * 5;

    if (ch->pcdata->habit[HABIT_EATING] == 2)
    deduct += 10;

    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    if(char_profile != NULL && char_profile->plus == 1)
    deduct += 12;

    return deduct;
  }

  int estimated_pay(CHAR_DATA *ch) {
    int amount = 0;

    int maxpay = 1000000 * get_skill(ch, SKILL_WEALTH);

    if (ch->pcdata->total_money < 0)
    amount = 0;
    else
    amount += UMIN(ch->pcdata->total_money, maxpay) *
    (get_skill(ch, SKILL_WEALTH)) / 10000;

    if (get_skill(ch, SKILL_WEALTH) == 0)
    amount += 200;
    else if (get_skill(ch, SKILL_WEALTH) == 1)
    amount += 400;
    else if (get_skill(ch, SKILL_WEALTH) == 2)
    amount += 700;
    else if (get_skill(ch, SKILL_WEALTH) == 3)
    amount += 950;
    else if (get_skill(ch, SKILL_WEALTH) == 4)
    amount += 1500;
    else if (get_skill(ch, SKILL_WEALTH) == 5)
    amount += 2900;

    if (ch->pcdata->job_type_one == JOB_MIGRANT) {
      ch->pcdata->job_type_one = JOB_UNEMPLOYED;
    }
    if (ch->pcdata->job_type_two == JOB_MIGRANT)
    ch->pcdata->job_type_two = JOB_UNEMPLOYED;

    if (ch->race != RACE_DEPUTY) {
      int mult = 20;
      if (ch->pcdata->job_type_one == JOB_CLINIC)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_one == JOB_COLLEGE)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_one == JOB_EMPLOYEE)
      mult = UMAX(mult, 20);
      if (ch->pcdata->job_type_one == JOB_FULLSTUDENT)
      mult = UMAX(mult, 65);
      if (ch->pcdata->job_type_two == JOB_PARTSTUDENT)
      mult = UMAX(mult, 65);
      if (ch->pcdata->job_type_one == JOB_PARTCOMMUTE)
      mult = UMAX(mult, 75);
      if (ch->pcdata->job_type_two == JOB_PARTCOMMUTE)
      mult = UMAX(mult, 75);
      if (ch->pcdata->job_type_one == JOB_FULLCOMMUTE)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_two == JOB_FULLCOMMUTE)
      mult = UMAX(mult, 100);
      if (ch->pcdata->job_type_one == JOB_FULLEMPLOY && ch->pcdata->job_delay_one > current_time - (3600 * 24 * 5)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_one)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_one))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 80);
        else
        mult = UMAX(mult, 100);
      }
      if (ch->pcdata->job_type_two == JOB_FULLEMPLOY && ch->pcdata->job_delay_two > current_time - (3600 * 24 * 5)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_two)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_two))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 80);
        else
        mult = UMAX(mult, 100);
      }
      if (ch->pcdata->job_type_one == JOB_PARTEMPLOY && ch->pcdata->job_delay_one > current_time - (3600 * 24 * 5)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_one)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_one))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 55);
        else
        mult = UMAX(mult, 85);
      }
      if (ch->pcdata->job_type_two == JOB_PARTEMPLOY && ch->pcdata->job_delay_two > current_time - (3600 * 24 * 5)) {
        if (prop_from_room(get_room_index(ch->pcdata->job_room_two)) == NULL || prop_from_room(get_room_index(ch->pcdata->job_room_two))->type ==
            PROP_HOUSE)
        mult = UMAX(mult, 55);
        else
        mult = UMAX(mult, 85);
      }
      amount = amount * mult / 100;
    }
    if (IS_FLAG(ch->act, PLR_NOPAY)) {
      amount = 0;
      REMOVE_FLAG(ch->act, PLR_NOPAY);
    }

    if (ch->fcore != 0) {
      if (ch->pcdata->intel >= 5000)
      amount += 50;

      amount += faction_pay(ch, ch->fcore) / 2;
    }
    if (ch->fcult != 0) {
      amount += faction_pay(ch, ch->fcult) / 2;
    }

    if (ch->fsect != 0) {
      amount += faction_pay(ch, ch->fsect) / 2;
    }

    amount = amount * (100 + soc_wealth_mod(ch, FALSE)) / 100;

    if (ch->pcdata->lfcount > 0) {
      int avglf = ch->pcdata->lftotal / ch->pcdata->lfcount;
      amount = amount * avglf / 100;
    }

    return amount;
  }
  bool has_place(ROOM_INDEX_DATA *room, char *argument) {

    EXTRA_DESCR_DATA *ed;
    for (ed = room->places; ed; ed = ed->next) {
      if (is_name(argument, ed->keyword)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  // Changed this from str_cmp to strcasestr so the word shower or bath can be
  // anywhere in the place name - Discordance
  bool in_shower(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    PROP_TYPE *prop;
    if ((prop = prop_from_room(ch->in_room)) != NULL) {
      if (!is_base(prop) && !IS_SET(ch->in_room->room_flags, ROOM_BATHROOM))
      return FALSE;
    }
    if (strcasestr(ch->pcdata->place, "shower") != NULL && has_place(ch->in_room, "shower"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "bath") != NULL && has_place(ch->in_room, "bath"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "jacuzzi") != NULL && has_place(ch->in_room, "jacuzzi") && get_decor(ch->in_room) > 2)
    return TRUE;
    if (strcasestr(ch->pcdata->place, "tub") != NULL && has_place(ch->in_room, "tub"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "pool") != NULL && has_place(ch->in_room, "pool") && get_decor(ch->in_room) > 1) {
      if (strcasestr(ch->pcdata->place, "table"))
      return FALSE;
      else
      return TRUE;
    }

    return FALSE;
  }
  bool in_stream(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    if (strcasestr(ch->pcdata->place, "lake") != NULL && has_place(ch->in_room, "lake"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "stream") != NULL && has_place(ch->in_room, "stream"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "river") != NULL && has_place(ch->in_room, "river"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "creek") != NULL && has_place(ch->in_room, "creek"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "brook") != NULL && has_place(ch->in_room, "brook"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "spring") != NULL && has_place(ch->in_room, "spring"))
    return TRUE;

    if (strcasestr(ch->pcdata->place, "pool") != NULL && has_place(ch->in_room, "pool")) {
      if (strcasestr(ch->pcdata->place, "table"))
      return FALSE;
      else
      return TRUE;
    }

    return FALSE;
  }

  bool in_bath(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    PROP_TYPE *prop;
    if ((prop = prop_from_room(ch->in_room)) != NULL) {
      if (!is_base(prop) && !IS_SET(ch->in_room->room_flags, ROOM_BATHROOM))
      return FALSE;
    }

    if (strcasestr(ch->pcdata->place, "bath") != NULL && has_place(ch->in_room, "bath"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "jacuzzi") != NULL && has_place(ch->in_room, "jacuzzi") && get_decor(ch->in_room) > 2)
    return TRUE;
    if (strcasestr(ch->pcdata->place, "tub") != NULL && has_place(ch->in_room, "tub"))
    return TRUE;
    if (strcasestr(ch->pcdata->place, "pool") != NULL && has_place(ch->in_room, "pool") && get_decor(ch->in_room) > 1) {
      if (strcasestr(ch->pcdata->place, "table"))
      return FALSE;
      else
      return TRUE;
    }

    return FALSE;
  }

  bool at_sink(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    if (strcasestr(ch->pcdata->place, "sink") != NULL && has_place(ch->in_room, "sink") && (IS_SET(ch->in_room->room_flags, ROOM_BATHROOM) || !IS_SET(ch->in_room->room_flags, ROOM_KITCHEN)))
    return TRUE;

    if (strcasestr(ch->pcdata->place, "counter") != NULL && has_place(ch->in_room, "counter") && IS_SET(ch->in_room->room_flags, ROOM_BATHROOM))
    return TRUE;

    return FALSE;
  }

  void debloodify(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    if (IS_NPC(ch))
    return;
    if (ch->in_room == NULL)
    return;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {

      if (obj->wear_loc == WEAR_NONE || (obj->item_type == ITEM_CLOTHING && ch->pcdata->wetness > 0)) {
        if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_RANGED && obj->item_type != ITEM_JEWELRY)
        continue;
        if (obj->item_type == ITEM_CLOTHING || obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_RANGED)
        obj->value[3] = UMAX(obj->value[3] - 1, 0);
        else
        obj->value[4] = UMAX(obj->value[4] - 1, 0);
      }

      if (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_RANGED)
      obj->value[3] = UMAX(obj->value[3] - 1, 0);
      if (obj->item_type == ITEM_JEWELRY)
      obj->value[4] = UMAX(obj->value[4] - 1, 0);

      if (obj->item_type == ITEM_CLOTHING && number_percent() % 15 == 0)
      obj->value[3] = UMAX(obj->value[3] - 1, 0);
    }
    if (in_shower(ch)) {
      // Cleaning sex stuff
      if (current_time > (ch->pcdata->last_shower + 300)) {
        ch->pcdata->last_shower = current_time;
      }
      ch->pcdata->sex_dirty = FALSE;

      ch->pcdata->blood[0] -= 50;
      ch->pcdata->dirt[0] -= 50;
      if (ch->pcdata->blood[0] > 0 || ch->pcdata->dirt[0] > 0) {
        send_to_char("You wash yourself.\n\r", ch);
        act("$n washes $mself.", ch, NULL, NULL, TO_ROOM);
      }
      if (number_percent() % 2 == 0) {
        free_string(ch->pcdata->scent);
        ch->pcdata->scent = str_dup("");
        ch->pcdata->perfume_cost = 0;
      }
      if (ch->pcdata->mark_timer[0] > 0)
      ch->pcdata->mark_timer[0]--;

      if (prop_from_room(ch->in_room) != NULL && prop_from_room(ch->in_room)->type_special == PROPERTY_OUTERFOREST && prop_from_room(ch->in_room)->utilities == 0)
      send_to_char("`cThe water is cold.`x\n\r", ch);
    }
    else if (in_stream(ch)) {
      if (current_time > (ch->pcdata->last_shower + 300)) {
        ch->pcdata->last_shower = current_time;
      }
      ch->pcdata->sex_dirty = FALSE;
      ch->pcdata->blood[0] -= 25;
      ch->pcdata->dirt[0] -= 25;
      if (ch->pcdata->blood[0] > 0 || ch->pcdata->dirt[0] > 0) {
        send_to_char("You wash yourself.\n\r", ch);
        act("$n washes $mself.", ch, NULL, NULL, TO_ROOM);
      }
      if (number_percent() % 2 == 0) {
        free_string(ch->pcdata->scent);
        ch->pcdata->scent = str_dup("");
        ch->pcdata->perfume_cost = 0;
      }
      if (ch->pcdata->mark_timer[0] > 0)
      ch->pcdata->mark_timer[0]--;

      if (prop_from_room(ch->in_room) != NULL && prop_from_room(ch->in_room)->type_special == PROPERTY_OUTERFOREST && prop_from_room(ch->in_room)->utilities == 0)
      send_to_char("`cThe water is cold.`x\n\r", ch);

    }
    else if (at_sink(ch)) {
      ch->pcdata->blood[0] -= 1;
      ch->pcdata->dirt[0] -= 1;

      if (ch->pcdata->blood[0] > 0 || ch->pcdata->dirt[0] > 0) {
        send_to_char("You wash yourself.\n\r", ch);
        act("$n washes $mself.", ch, NULL, NULL, TO_ROOM);

        if (prop_from_room(ch->in_room) != NULL && prop_from_room(ch->in_room)->type_special == PROPERTY_OUTERFOREST && prop_from_room(ch->in_room)->utilities == 0)
        send_to_char("`cThe water is cold.`x\n\r", ch);
      }
      if (ch->pcdata->mark_timer[0] > 0)
      ch->pcdata->mark_timer[0]--;

    }
    else if (ch->pcdata->wetness > 0) {
      ch->pcdata->blood[0] -= 1;
      ch->pcdata->dirt[0] -= 1;

      if (number_percent() % 5 == 0 && ch->pcdata->wetness > 50) {
        free_string(ch->pcdata->scent);
        ch->pcdata->scent = str_dup("");
        ch->pcdata->perfume_cost = 0;
      }
    }
    else if (in_hospital(ch) && ch->wounds > 1) {
      ch->pcdata->blood[0] -= 1;
      ch->pcdata->dirt[0] -= 1;
    }

    ch->pcdata->blood[0] = UMAX(ch->pcdata->blood[0], 0);
    ch->pcdata->dirt[0] = UMAX(ch->pcdata->dirt[0], 0);
  }

  void sex_update(CHAR_DATA *ch) {
    // if(!is_covered(ch, COVERS_GROIN))
    //{
    // if(ch->pcdata->penis > 0)
    //     ch->sex = SEX_MALE;
    // else
    //     ch->sex = SEX_FEMALE;
    //}
    // else
    //{
    if (get_bust(ch) > -1)
    ch->sex = SEX_FEMALE;
    else
    ch->sex = SEX_MALE;
    //}
  }

  int get_obj_warmth(OBJ_DATA *obj, CHAR_DATA *ch, int iWear) {

    if (obj == NULL || obj->item_type != ITEM_CLOTHING) {
      return 0;
    }

    int points = 2;

    if (does_cover(obj, COVERS_HAIR)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_LOWER_FACE)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_NECK)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_UPPER_CHEST)) {
      points += 1;
    }
    if (does_cover(obj, COVERS_BREASTS)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_LOWER_CHEST)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_UPPER_BACK)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_LOWER_BACK)) {
      points += 2;
    }
    if (does_cover(obj, COVERS_ARSE)) {
      points += 1;
    }
    if (does_cover(obj, COVERS_GROIN)) {
      points++;
    }
    if (does_cover(obj, COVERS_THIGHS)) {
      points++;
    }
    if (does_cover(obj, COVERS_LOWER_LEGS)) {
      points++;
    }
    if (does_cover(obj, COVERS_FEET)) {
      points++;
    }
    if (does_cover(obj, COVERS_UPPER_ARMS)) {
      points++;
    }
    if (does_cover(obj, COVERS_LOWER_ARMS)) {
      points++;
    }
    if (does_cover(obj, COVERS_HANDS)) {
      points++;
    }
    if (is_name("heated", obj->short_descr)) {
      points = points * 3;
    }
    else if (is_name("fur", obj->short_descr)) {
      points = points * 2;
    }
    else if (is_name("leather", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("suede", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("wool", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("woolen", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("woollen", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("Merino", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("padded", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("quilted", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("thermal", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("insulated", obj->short_descr)) {
      points = points * 3 / 2;
    }
    else if (is_name("silk", obj->short_descr)) {
      points = points / 2;
    }
    else if (is_name("satin", obj->short_descr)) {
      points = points / 2;
    }
    else if (is_name("sheer", obj->short_descr)) {
      points = points / 2;
    }
    else if (is_name("gauzy", obj->short_descr)) {
      points = points / 2;
    }
    else if (is_name("white", obj->short_descr)) {
      points = points *4/5;
    }
    else if (is_name("vented", obj->short_descr)) {
      points = points / 2;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      if (sunphase(ch->in_room) >= 3 && sunphase(ch->in_room) <= 5) {
        if (is_name("black", obj->short_descr) && can_see_wear(ch, iWear)) {
          points = points * 3 / 2;
        }
        else if (is_name("dark", obj->short_descr) && can_see_wear(ch, iWear)) {
          points = points * 5 / 4;
        }
      }
    }

    return points;
  }

  int get_warmth(CHAR_DATA *ch) {
    int iWear;
    int warmth = 0;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        warmth += get_obj_warmth(get_eq_char(ch, iWear), ch, iWear);
      }
    }
    return warmth;
  }

  bool climatecon(ROOM_INDEX_DATA *room) {
    if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
      return FALSE;
    }
    if (room->sector_type == SECT_CLUB) {
      return TRUE;
    }
    if (room->sector_type == SECT_HOUSE) {
      return TRUE;
    }
    if (room->sector_type == SECT_RESTERAUNT) {
      return TRUE;
    }
    if (room->sector_type == SECT_SHOP) {
      return TRUE;
    }
    if (room->sector_type == SECT_TAVERN) {
      return TRUE;
    }
    if (room->sector_type == SECT_COMMERCIAL) {
      return TRUE;
    }
    if (room->sector_type == SECT_CAFE) {
      return TRUE;
    }
    if (room->sector_type == SECT_HOSPITAL) {
      return TRUE;
    }
    if (room->sector_type == SECT_BANK) {
      return TRUE;
    }
    if (room->sector_type == SECT_CAR) {
      return TRUE;
    }
    if (district_room(room) == DISTRICT_INSTITUTE) {
      return TRUE;
    }
    if (in_lodge(room) == TRUE) {
      return TRUE;
    }
    return FALSE;
  }

  bool has_attendant(CHAR_DATA *ch) {

    CHAR_DATA *victim;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      if (victim == ch)
      continue;

      if (IS_NPC(victim))
      continue;
      if (get_skill(victim, SKILL_SOOTHING) > 0)
      return TRUE;
    }
    return FALSE;
  }

  bool can_heal(CHAR_DATA *ch) { return TRUE; }

  void linkdeadtravel(CHAR_DATA *ch) {
    if (IS_FLAG(ch->comm, COMM_STALL))
    return;
    char buf[MSL];

    if (ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1 && !in_fight(ch)) {
      ch->pcdata->travel_time -= 5;

      if (ch->pcdata->travel_time <= 0) {
        if (ch->pcdata->travel_type == TRAVEL_CLIMB) {
          if (!is_ghost(ch))
          act("$n climbs out of the room.", ch, NULL, ch, TO_NOTVICT);
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
          if (!is_ghost(ch))
          act("$n climbs into the room.", ch, NULL, ch, TO_NOTVICT);
        }
        if (ch->pcdata->travel_type == TRAVEL_FALL) {
          if (!is_ghost(ch))
          act("$n falls out of the room.", ch, NULL, ch, TO_NOTVICT);
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          fall_character(ch);
          if (ch->in_room->sector_type == SECT_WATER) {
            if (!is_ghost(ch))
            act("$n bellyflops into the water.", ch, NULL, ch, TO_NOTVICT);
          }
          else {
            if (!is_ghost(ch))
            act("$n falls into the room.", ch, NULL, ch, TO_NOTVICT);
          }
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_JUMP) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          fall_character(ch);
          if (ch->in_room->sector_type == SECT_WATER) {
            if (!is_ghost(ch))
            act("$n dives into the water.", ch, NULL, ch, TO_NOTVICT);
          }
          else {
            if (!is_ghost(ch))
            act("$n jumps into the room.", ch, NULL, ch, TO_NOTVICT);
          }
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_TAXI) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch))
          act("$n steps out of a taxi.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_CPASSENGER) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch))
          act("$n steps out of a car.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_BUS) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch))
          act("$n steps off a bus.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_PATHING) {
          ROOM_INDEX_DATA *to_room = get_room_index(ch->pcdata->travel_to);
          if (in_haven(to_room))
          to_room = room_by_coordinates(number_range(-10, 80), number_range(-10, 80), 0);
          if (to_room == NULL)
          to_room = get_room_index(ch->pcdata->travel_to);
          char_from_room(ch);
          char_to_room(ch, to_room);
          if (!is_ghost(ch))
          act("$n steps out of thin air.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_PLANE) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch))
          act("$n steps out of a a taxi.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_CAR) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch)) {
            if (safe_strlen(vehicle_name(ch)) > 2) {
              sprintf(buf, "$n steps out of $s %s.", vehicle_name(ch));
              act(buf, ch, NULL, NULL, TO_ROOM);
            }
            else
            act("$n steps out of $s car.", ch, NULL, NULL, TO_ROOM);
          }
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_BPASSENGER) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch))
          act("$n gets off a motorcycle.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_BIKE) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch)) {
            if (safe_strlen(vehicle_name(ch)) > 2) {
              sprintf(buf, "$n gets off $s %s.", vehicle_name(ch));
              act(buf, ch, NULL, NULL, TO_ROOM);
            }
            else
            act("$n gets off $s motorcycle.", ch, NULL, NULL, TO_ROOM);
          }
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_HPASSENGER) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch))
          act("$n gets off a horse.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
        if (ch->pcdata->travel_type == TRAVEL_HORSE) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(ch->pcdata->travel_to));
          if (!is_ghost(ch)) {
            if (safe_strlen(vehicle_name(ch)) > 2) {
              sprintf(buf, "$n gets off $s %s.", vehicle_name(ch));
              act(buf, ch, NULL, NULL, TO_ROOM);
            }
            else
            act("$n gets off $s horse.", ch, NULL, NULL, TO_ROOM);
          }
          ch->pcdata->travel_time = -1;
          ch->pcdata->travel_to = -1;
          ch->pcdata->travel_type = -1;
          ch->pcdata->move_damage = 0;
        }
      }
    }
  }

  void fight_check_update(void) {
    CHAR_DATA *ch;
    bool wasinfight;
    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      ch = *it;
      if (it != char_list.end())
      ++it;

      if (ch == NULL || !ch)
      continue;

      if (ch->race < 0 || ch->race > 200 || ch->sex < 0 || ch->sex > 10)
      continue;

      wasinfight = ch->in_fight;
      if (ch->in_fight == FALSE) {
        ch->in_fight = check_fight(ch);
        if (ch->in_fight == TRUE) {
          if (next_fight_member(ch) != NULL) {
            ch->fight_fast = next_fight_member(ch)->fight_fast;
            ch->fight_speed = next_fight_member(ch)->fight_speed;
          }
          join_to_fight(ch);
        }
      }
      else
      ch->in_fight = check_fight(ch);

      if (!IS_NPC(ch)) {
        if (wasinfight == TRUE && ch->in_fight == FALSE)
        send_to_char("`rYou leave combat.`x\n\r", ch);
        else if (wasinfight == FALSE && ch->in_fight == TRUE)
        send_to_char("`rYou enter combat.`x\n\r", ch);
      }
      if (!in_fight(ch)) {
        if (!IS_NPC(ch)) {
          ch->pcdata->default_speed = UMAX(1, ch->pcdata->default_speed);
          ch->pcdata->default_speed = UMIN(5, ch->pcdata->default_speed);
          ch->fight_speed = ch->pcdata->default_speed;
          if (IS_FLAG(ch->comm, COMM_SPARRING))
          REMOVE_FLAG(ch->comm, COMM_SPARRING);
        }
        ch->target = NULL;
        ch->target_dam = 0;
        ch->target_2 = NULL;
        ch->target_dam_2 = 0;
        ch->target_3 = NULL;
        ch->target_dam_3 = 0;
        ch->attacking = 0;
        ch->fight_fast = TRUE;
        ch->hadturn = FALSE;
        ch->fight_current = NULL;
      }
    }
  }

  void clean_descriptors() {
    //    std::sort(descriptor_list.begin(), descriptor_list.end());
    descriptor_list.erase(unique(descriptor_list.begin(), descriptor_list.end()), descriptor_list.end());
    return;

    int i = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      i++;
      if (!d->character)
      continue;
      int j = 0;
      for (DescList::iterator ij = descriptor_list.begin();
      ij != descriptor_list.end(); ++ij) {
        DESCRIPTOR_DATA *x = *ij;
        j++;
        if (!x->character)
        continue;
        if (i != j) {
          if (!str_cmp(d->character->name, x->character->name)) {
            close_desc(x);
            return;
          }
        }
      }
    }
  }

  _DOFUN(do_cleandescs) {
    int i = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      i++;
      if (!d->character)
      continue;
      int j = 0;
      for (DescList::iterator ij = descriptor_list.begin();
      ij != descriptor_list.end(); ++ij) {
        DESCRIPTOR_DATA *x = *ij;
        j++;
        if (!x->character)
        continue;
        printf_to_char(ch, "i: %d, j: %d, dc: %s, xc: %s\n\r", i, j, d->character->name, x->character->name);
        if (x != d && i != j) {
          if (d->character == x->character)
          send_to_char("1", ch);
          if (!str_cmp(d->character->name, x->character->name))
          send_to_char("2", ch);
        }
        if (i > 5)
        return;
      }
    }
  }

  // Every 5 seconds
  void char_update(int time_value, int roleplay_value) {
    CHAR_DATA *ch;

    save_number++;

    lair_populate();

    clean_descriptors();

    if (save_number > 29)
    save_number = 0;

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {

      if ((*it) == NULL)
      continue;

      // if((*it)->race <= 0 || (*it)->race > 200 || (*it)->sex < 0 || (*it)->sex
      // > 10) // Disco crash resolution 9/24/2018  Bad news bears. continue;

      ch = *it;
      if (it != char_list.end())
      ++it;

      if (ch == NULL || !ch)
      continue;

      if (ch->race < 0 || ch->race > 200 || ch->sex < 0 || ch->sex > 10)
      continue;

      if (in_fight(ch) && ch->fight_fast == FALSE) {
        ch->tsincemove++;
        if (IS_NPC(ch) && ch->tsincemove >= 200)
        ch->in_fight = FALSE;
        if (!IS_NPC(ch) && ch->tsincemove >= 300)
        ch->in_fight = FALSE;
      }
      else
      ch->tsincemove = 0;

      if (!IS_NPC(ch)) {
        pc_update(ch, save_number);
        if (ch->desc == NULL)
        linkdeadtravel(ch);
      }
      else {
        mobile_update(ch);
      }
    }
  }

  bool mentoring_eligible_ic(CHAR_DATA *ch) {
    if (ch->played / 3600 > 50)
    return FALSE;
    if (IS_FLAG(ch->act, PLR_NOMENTOR))
    return FALSE;
    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return FALSE;

    if (is_gm(ch) || IS_FLAG(ch->act, PLR_GUEST))
    return FALSE;

    if (ch->pcdata->account != NULL && ch->spentrpexp + ch->pcdata->account->rpxp >= 50000)
    return FALSE;

    if (ch->faction == 0)
    return FALSE;

    return TRUE;
  }
  bool mentoring_eligible_ooc(CHAR_DATA *ch) {
    if (ch->played / 3600 > 50)
    return FALSE;
    if (IS_FLAG(ch->act, PLR_NOMENTOR))
    return FALSE;
    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return FALSE;

    if (is_gm(ch) || IS_FLAG(ch->act, PLR_GUEST))
    return FALSE;

    if (ch->pcdata->account != NULL) {
      if (ch->spentrpexp + ch->pcdata->account->rpxp >= 50000)
      return FALSE;

      if (str_cmp(ch->pcdata->account->characters[0], ch->name))
      return FALSE;
      if (ch->pcdata->account->maxhours > 50)
      return FALSE;
    }
    return TRUE;
  }

  void poison_char(CHAR_DATA *ch) {

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = 12 * 60 * 20;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_ILLNESS;
    affect_to_char(ch, &af);

    if (ch->wounds <= 1)
    wound_char_noregen(ch, 1);
  }

  void apply_timeddrop(CHAR_DATA *victim, int length) {
    if (length == 0)
    length = 12 * 5;

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = length;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_TIMEDDROP;
    affect_to_char(victim, &af);
    return;
  }

  void ghost_update(CHAR_DATA *ch) {

    if (is_ghost(ch)) {
      if (ch->pcdata->timebanished > current_time) {
        if (ch->in_room->sector_type != SECT_CEMETARY) {
          ROOM_INDEX_DATA *toroom = graveyard_room();
          char_from_room(ch);
          char_to_room(ch, toroom);
        }
      }
    }

    if (IS_FLAG(ch->act, PLR_DEAD)) {
      if (!IS_AFFECTED(ch, AFF_TIMEDDROP)) {
        int chance, length;
        ghost_drop(ch);
        chance = number_range(1, 5);
        length = chance * 12;
        apply_timeddrop(ch, length);
      }
    }
  }

  bool locked_room(ROOM_INDEX_DATA *room, CHAR_DATA *ch) {
    int door;
    EXIT_DATA *pexit;
    if(room == NULL)
    return FALSE;
    if (is_prisoner(ch))
    return FALSE;

    for (door = 0; door <= 9; door++) {
      if ((pexit = room->exit[door]) != NULL && pexit->u1.to_room != NULL) {
        if (pexit->wall == WALL_NONE || pexit->wallcondition != WALLCOND_HOLE)
        return FALSE;
        if (!IS_SET(pexit->exit_info, EX_CLOSED) || !IS_SET(pexit->exit_info, EX_LOCKED))
        return FALSE;
      }
    }
    return TRUE;
  }

  int mentor_weight(CHAR_DATA *ch, CHAR_DATA *to) {
    int val = to->played / 3600;
    if (ch->faction == to->faction)
    val *= 5;
    if (ch->faction == to->factiontwo)
    val *= 3;
    if (college_student(ch, TRUE) && college_student(to, TRUE))
    val *= 5;
    if (clinic_patient(ch) && clinic_patient(to))
    val *= 5;
    if (college_staff(ch, TRUE) && college_staff(to, TRUE))
    val *= 5;
    if (clinic_staff(ch, TRUE) && clinic_staff(to, TRUE))
    val *= 5;
    if (college_student(ch, TRUE) && college_staff(ch, TRUE))
    val *= 3;
    if (clinic_patient(ch) && clinic_staff(ch, TRUE))
    val *= 3;
    return val;
  }

  void curse_update(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    EXTRA_DESCR_DATA *ed;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (IS_SET(obj->extra_flags, ITEM_CURSED)) {
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+imprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description)) {
                }
              }
              /*
if(found == FALSE)
{
imprint_person_nomod(ch, ed->description, 50, 2);
return;
}
*/
            }
          }
        }
      }
    }
  }

  void pc_update(CHAR_DATA *ch, int save_number) {
    CHAR_DATA *to;
    char buf[MSL];
    char buf2[MSL];
    EXTRA_DESCR_DATA *ed;
    OBJ_DATA *obj;
    AFFECT_DATA *paf = NULL, *paf_next = NULL;

    if (ch == NULL)
    return;

    if (ch->pcdata == NULL || ch->in_room == NULL)
    return;

    if (ch->desc && ch->desc->connected == CON_QUITTING)
    return;

    if (ch->desc && ch->desc->connected == CON_CREATION)
    return;

    if (ch->desc != NULL && ch->desc->character != ch)
    ch->desc->character = ch;

    if (!ch->name || ch->sex > 5 || ch->sex < -1 || ch->race > 200 || ch->race < -1)
    return;

    if (IS_IMMORTAL(ch)) {
      ch->pcdata->total_money = UMIN(ch->pcdata->total_money, 100000);
      ch->money = UMIN(ch->money, 100000);
      if (ch->pcdata->account != NULL) {
        ch->pcdata->account->rpxp = UMIN(ch->pcdata->account->rpxp, 10000);
        ch->pcdata->account->xp = UMIN(ch->pcdata->account->xp, 10000);
        ch->pcdata->account->karma = UMIN(ch->pcdata->account->karma, 1000);
      }
    }

    if (ch->modifier == MODIFIER_LUNACY)
    ch->pcdata->habit[HABIT_LUNACY] = 3;

    ghost_update(ch);
    legacy_update(ch);
    curse_update(ch);

    if (!IS_FLAG(ch->act, PLR_GUEST) && (ch->pcdata->guest_type == NULL || ch->pcdata->guest_type <= 0)) {
      REMOVE_FLAG(ch->act, PLR_GUEST);
    }

    if (!IS_FLAG(ch->act, PLR_SHROUD))
    ch->pcdata->nightmare_dragged = 0;

    if (ch->pcdata->account != NULL) {
      ch->pcdata->account->pkarmaspent = 0;
      //	ch->pcdata->account->pkarmaspent =
      //UMAX(ch->pcdata->account->pkarmaspent, ch->spentpkarma);
    }
    /*
if(ch->pcdata->mentor_of > 0)
{
sprintf(buf, "Mentor Tick: %s:%d", ch->name, ch->pcdata->mentor_of);
log_string(buf);
}
*/
    if (ch->race == RACE_FANTASY && ch->pcdata->sleeping <= 0)
    to_spectre(ch, FALSE);

    if (ch->pcdata->victimize_vic_timer > 0) {
      if (ch->pcdata->victimize_vic_timer == 60)
      send_to_char("One minute left to make your choice.\n\r", ch);
      if (ch->pcdata->victimize_vic_timer == 0) {
        if (ch->pcdata->victimize_vic_select == 0)
        process_victim_choice(ch, RESPONSE_STOIC);
        else
        victimize_emote_process(ch);
      }
    }

    if (IS_FLAG(ch->comm, COMM_RUNNING)) {
      if (!is_gm(ch))
      REMOVE_FLAG(ch->comm, COMM_RUNNING);
    }
    else {
      if (ch->in_room != NULL && (ch->in_room->area->vnum == 12 || ch->in_room->area->vnum == 23 || ch->in_room->area->vnum == 27) && is_gm(ch) && ch->pcdata->vote_timer == 0 && pc_pop(ch->in_room) > 1)
      ch->pcdata->vote_timer = -1;
    }

    if (ch->pcdata->travel_time <= 0 && IS_FLAG(ch->comm, COMM_STALL))
    REMOVE_FLAG(ch->comm, COMM_STALL);

    if (ch->pcdata->mind_guarded > 0)
    ch->pcdata->mind_guarded -= 5;

    ch->pcdata->time_since_emote++;

    ch->lastlogoff = current_time;

    if (ch->pcdata->paranoid > 0)
    ch->pcdata->paranoid--;
    if (ch->pcdata->spawned_monsters > 0)
    ch->pcdata->spawned_monsters--;

    if (guestmonster(ch) && fetch_guestmonster_exclusive(ch) != NULL && fetch_guestmonster_exclusive(ch) != ch) {
      send_to_char("There is already another monster in Haven.\n\r", ch);
      ch->played += 3300 * 2;
      real_quit(ch);
      return;
    }

    if (ch->pcdata->watching > 0)
    ch->pcdata->watching -= 5;

    if (in_world(ch) == WORLD_EARTH && !IS_FLAG(ch->act, PLR_SHROUD)) {
      if (in_haven(ch->in_room))
      ch->pcdata->last_inhaven = current_time;
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->augskill_level[i] > 2) {
          ch->pcdata->augskill_level[i] = 0;
          ch->pcdata->augskill_timer[i] = 0;
        }
        if (ch->pcdata->augdisc_level[i] > 10) {
          ch->pcdata->augdisc_level[i] = 0;
          ch->pcdata->augdisc_timer[i] = 0;
        }
      }
    }
    else {
      if (ch->pcdata->last_inhaven < current_time - (3600 * 24 * 2))
      ch->pcdata->resident = in_world(ch);
    }

    if (ch->pcdata->garbled > 0)
    ch->pcdata->garbled--;
    if (ch->pcdata->garbled > 200)
    ch->pcdata->garbled = 200;

    stat_active_update(ch);

    if (ch->pcdata->maintained_ritual == RITUAL_MUTE && !IS_AFFECTED(ch, AFF_MUTE)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MUTE;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_ILLNESS && !IS_AFFECTED(ch, AFF_ILLNESS)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_ILLNESS;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_ARTHRITIS && !IS_AFFECTED(ch, AFF_ARTHRITIS)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_ILLNESS;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_TECHHEX && !IS_AFFECTED(ch, AFF_NOTECH)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_NOTECH;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_CORRUPT && !IS_AFFECTED(ch, AFF_TRIGGERED)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_TRIGGERED;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_DEAFEN && !IS_AFFECTED(ch, AFF_DEAF)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_DEAF;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_MADNESS && !IS_AFFECTED(ch, AFF_MAD)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_MAD;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_PERSECUTION && !IS_AFFECTED(ch, AFF_PERSECUTED)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_PERSECUTED;
      affect_to_char(ch, &af);
    }
    if (ch->pcdata->maintained_ritual == RITUAL_ENFEEBLE && !IS_AFFECTED(ch, AFF_WEAKEN)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 6;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_WEAKEN;
      affect_to_char(ch, &af);
    }

    if (ch->pcdata->travel_time > 20 && get_vnum_area(ch->in_room->vnum) == get_vnum_area(HavenStreets))
    ch->pcdata->travel_time = -1;

    if (!in_fight(ch)) {
      for (int i = 0; i < 20; i++) {
        ch->pcdata->abilcools[i] = 0;
      }
    }

    if (guestmonster(ch) && !IS_FLAG(ch->act, PLR_SHROUD) && ch->in_room != NULL && ch->in_room->vnum < 100)
    SET_FLAG(ch->act, PLR_SHROUD);

    if (!IS_FLAG(ch->act, PLR_SHROUD) && !is_dreaming(ch))
    ch->pcdata->spectre = 0;

    if (ch->race == RACE_CIVILIAN) {
      ch->pcdata->dtrains = 60;
      ch->race = RACE_NORMAL;
    }

    if (is_ghost(ch) && ch->possessing != NULL && ch->possessing->in_room != NULL && ch->in_room != NULL && ch->in_room != ch->possessing->in_room)
    ch->possessing = NULL;

    if (!IS_NPC(ch) && ch->factiontrue > -1 && !battleground(ch->in_room) && !in_fight(ch) && ch->pcdata->spectre != 1) {
      ch->faction = ch->factiontrue;
      ch->factiontrue = -1;
    }

    if (ch->walking == 1 && ch->destination != NULL && ch->destination && ch->destination->area && ch->in_room != NULL && ch->wait <= 0) {
      if (ch->destination == ch->in_room) {
        if (ch->pcdata->driving_around == TRUE) {
          ROOM_INDEX_DATA *newroom = NULL;
          for (int i = 0; i < MAX_TAXIS; i++) {
            if (number_percent() % 2 == 0 && taxi_table[i].y > 45 && (taxi_table[i].x == ch->destination->x || taxi_table[i].y == ch->destination->y))
            newroom = get_room_index(taxi_table[i].vnum);
          }
          if (newroom == NULL) {
            for (int i = 0; i < MAX_TAXIS; i++) {
              if (taxi_table[i].y > 45 && (taxi_table[i].x == ch->destination->x || taxi_table[i].y == ch->destination->y))
              newroom = get_room_index(taxi_table[i].vnum);
            }
          }
          if (newroom == NULL)
          ch->pcdata->driving_around = FALSE;
          else {
            ch->destination = newroom;
          }
        }
        else {
          ch->walking = 0;
          if (ch->in_room->vnum == 13806) {
            send_to_char("You burst from the water, gulping down your first breaths of free air.\n\r", ch);
          }
          else {
            send_to_char("You arrive.\n\r", ch);
          }
        }
      }
      else {
        if (ch->in_room->vnum == 13894) {
          WAIT_STATE(
          ch, number_range(move_speed(ch) * 8 / 10, move_speed(ch) * 8 / 10));
          printf_to_char(ch, "You swim %s.\n\r", dir_name[DIR_UP][0]);
          move_char(ch, DIR_UP, FALSE, FALSE);
        }
        else {
          int dir = path_dir(ch->in_room, ch->destination, ch->facing, ch);
          if (dir == -1) {
            ch->walking = 0;
            send_to_char("No path found.\n\r", ch);
          }
          else {
            printf_to_char(ch, "You walk %s.\n\r", dir_name[dir][0]);
            move_char(ch, dir, FALSE, FALSE);
            WAIT_STATE(ch, number_range(move_speed(ch) * 8 / 10, move_speed(ch) * 12 / 10));
          }
        }
      }
    }

    if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(ch->act, PLR_SHROUD))
    REMOVE_FLAG(ch->act, PLR_DEEPSHROUD);

    if (event_cleanse == 1) {
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD))
      REMOVE_FLAG(ch->act, PLR_DEEPSHROUD);
      if (IS_FLAG(ch->act, PLR_SHROUD))
      REMOVE_FLAG(ch->act, PLR_SHROUD);
    }

    if (ch->recent_moved > -1000)
    ch->recent_moved -= 5;

    if (ch->pcdata->shroud_dragging > 0)
    ch->pcdata->shroud_dragging = UMAX(0, ch->pcdata->shroud_dragging - 5);

    if (!is_helpless(ch) && !in_fight(ch))
    ch->pcdata->monster_beaten = 0;

    if (ch->pcdata->trance > 0 && (in_fight(ch) || room_hostile(ch->in_room) || in_fistfight(ch)))
    ch->pcdata->trance = -20;

    if (ch->pcdata->superlocked > current_time + (3600 * 24 * 45))
    ch->pcdata->superlocked = 0;
    if (ch->pcdata->translocked > current_time + (3600 * 24 * 45))
    ch->pcdata->translocked = 0;
    if (ch->pcdata->medic_uses > 10 || ch->pcdata->medic_uses < 0)
    ch->pcdata->medic_uses = 0;
    if (ch->pcdata->experiment_uses > 10 || ch->pcdata->experiment_uses < 0)
    ch->pcdata->experiment_uses = 0;
    if (ch->pcdata->apower > 10 || ch->pcdata->apower < 0)
    ch->pcdata->apower = 0;
    if (ch->pcdata->vbloodcool > current_time + (3600 * 24 * 10))
    ch->pcdata->vbloodcool = 0;

    if (event_recruitment != 0 && char_in_alliance_with(ch, event_recruitment) && (ch->pcdata->guard_number <= 0 || ch->pcdata->guard_expiration <= current_time) && !is_helpless(ch) && !in_fight(ch)) {
      ch->pcdata->guard_number = 1;
      ch->pcdata->guard_faction = event_recruitment;
      ch->pcdata->guard_expiration = current_time + (3600 * 24);
    }

    if (!in_fight(ch)) {
      if (ch->debuff > 0)
      ch->debuff = UMAX(0, ch->debuff - 10);
      if (ch->debuff < 0)
      ch->debuff = 0;
      ch->rounddiminish = 0;
      ch->damage_absorb = 0;
    }

    if (ch->diminish_offervalue > 0) {
      if (ch->diminish_offer == NULL || ch->diminish_offer->in_room == NULL)
      ch->diminish_offervalue = 0;
      else if (ch->in_room != ch->diminish_offer->in_room) {
        ch->diminish_offervalue = 0;
        ch->diminish_offer->diminish_offervalue = 0;
        ch->diminish_offer->diminish_offer = NULL;
        ch->diminish_offer = NULL;
      }
    }

    if (safe_strlen(ch->pcdata->surveying) < 2 && ch->pcdata->survey_stage > 1) {
      ch->pcdata->survey_stage = 0;
      ch->pcdata->survey_delay = 0;
    }
    survey_update(ch);

    if (ch->pcdata->encounter_countdown > 0 && ch->pcdata->encounter_sr != NULL && ch->pcdata->encounter_status == ENCOUNTER_PENDING && !is_gm(ch)) {
      ch->pcdata->encounter_countdown -= 5;
      if (ch->pcdata->encounter_countdown <= 0 && !IS_NPC(ch->pcdata->encounter_sr)) {

        ch->pcdata->encounter_sr->pcdata->encounter_countdown = 0;
        ch->pcdata->encounter_sr->pcdata->encounter_status = 0;
        ch->pcdata->encounter_status = ENCOUNTER_REJECTED;
        send_to_char("The encounter offer has timed out.\n\r", ch);
        send_to_char("The encounter timed out, please try again.\n\r", ch->pcdata->encounter_sr);
      }
    }
    else if (ch->pcdata->encounter_countdown > 0 && ch->pcdata->encounter_sr != NULL && ch->pcdata->encounter_status == ENCOUNTER_PENDINGALLY && !is_gm(ch)) {
      ch->pcdata->encounter_countdown -= 5;
      if (ch->pcdata->encounter_countdown <= 0 && !IS_NPC(ch->pcdata->encounter_sr)) {
        ch->pcdata->encounter_status = ENCOUNTER_REJECTED;
        send_to_char("The encounter offer has timed out.\n\r", ch);
      }
    }
    else if (ch->pcdata->encounter_countdown > 0) {
      ch->pcdata->encounter_countdown -= 5;
    }

    battlecheck(ch);

    if (IS_FLAG(ch->act, PLR_BOUND) && ch->in_room != NULL && ch->pcdata->captive_timer > 17200 && public_room(ch->in_room) && room_pop(ch->in_room) < 2) {
      REMOVE_FLAG(ch->act, PLR_BOUND);
      send_to_char("You work yourself free.\n\r", ch);
    }
    /*
if(is_abom(ch) && IS_AFFECTED(ch, AFF_MARKED))
ch->pcdata->bloodaura = 60*50;
*/

    if (in_fight(ch) == FALSE) {
      ch->knockfails = 0;
      for (int i = FIGHT_NOATTACK; i <= FIGHT_MAXFLAG; i++) {
        if (IS_FLAG(ch->fightflag, i))
        REMOVE_FLAG(ch->fightflag, i);
      }
      ch->attack_timer = 0;
      ch->move_timer = 0;
      ch->ability_timer = 0;
      ch->ability_dead = 0;
      ch->bagcarrier = 0;
      for (int i = 0; i < 30; i++) {
        ch->caff[i] = 0;
        ch->caff_duration[i] = 0;
      }
      for (int i = 0; i < 10; i++) {
        ch->delays[i][0] = 0;
        ch->delays[i][1] = 0;
      }
    }

    if (!IS_FLAG(ch->act, PLR_BOUND) && !IS_FLAG(ch->act, PLR_BOUNDFEET))
    ch->pcdata->selfbondage = 0;

    if (!is_dreaming(ch)) {
      ch->pcdata->dream_link = NULL;
      ch->pcdata->dream_controller = 0;
    }

    if (ch->pcdata->version_player < 2) {
      if (ch->pcdata->account == NULL && ch->desc->account != NULL)
      ch->pcdata->account = ch->desc->account;
      if (!is_gm(ch)) {
        if (!IS_FLAG(ch->act, PLR_DEAD))
        SET_FLAG(ch->act, PLR_DEAD);
        char_from_room(ch);
        char_to_room(ch, get_room_index(98));
      }
      else if (!IS_IMMORTAL(ch) && is_gm(ch)) {
        ch->pcdata->confirm_delete = TRUE;
        send_to_char("Independent SRs have been removed from the game.\n\r", ch);
        if (ch->pcdata->account != NULL) {
          ch->karma /= 3;
          ch->karma = UMIN(ch->karma, 50000);
          ch->pcdata->account->karma += ch->karma;
          ch->karma = 0;
          do_function(ch, &do_delete, "");
        }
        return;
      }
      else if (is_gm(ch)) {
        ch->exp = 0;
        ch->spentexp = 0;
        ch->rpexp = 0;
        ch->spentrpexp = 0;
      }
      if (IS_FLAG(ch->act, PLR_NOSAVE))
      REMOVE_FLAG(ch->act, PLR_NOSAVE);
      ch->money = 0;
      ch->pcdata->total_money = 0;
      ch->race = RACE_LOCAL;
      ch->pcdata->influence = 0;
      ch->exp += ch->spentexp;
      ch->spentexp = 0;
      ch->exp /= 10;
      ch->exp = UMIN(ch->exp, 100000);
      ch->rpexp += ch->spentrpexp;
      ch->spentrpexp = 0;
      ch->rpexp /= 4;
      ch->rpexp = UMIN(ch->rpexp, 100000);
      if (ch->pcdata->pathtotal > 2)
      ch->karma += (ch->pcdata->pathtotal - 2) * 5000;
      ch->karma /= 3;
      ch->karma = UMIN(ch->karma, 50000);
      ch->pcdata->version_player = 2;
      ch->faction = 0;
      ch->spentkarma = 0;
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }
      if (ch->pcdata->account != NULL) {
        ch->pcdata->account->karmabank = 0;
        ch->pcdata->account->xp += ch->exp;
        ch->exp = 0;
        ch->pcdata->account->rpxp += ch->rpexp;
        ch->rpexp = 0;
        ch->pcdata->account->karma += ch->karma;
        ch->karma = 0;
        ch->pcdata->account->pkarma += 2500 * UMIN(2, ch->pcdata->pathtotal);
        ch->pcdata->account->factiontime = 0;
        ch->pcdata->account->donated += ch->donated;
        ch->donated = 0;
      }
    }

    if (ch->pcdata->habits_done > 100 || ch->pcdata->habits_done < 0)
    ch->pcdata->habits_done = 0;
    if (ch->pcdata->habits_done > 0) {
      ch->pcdata->habits_done++;

      if (ch->pcdata->habits_done >= 3) {
        habit_logon(ch);
        profile_logon(ch);
        if (IS_AFFECTED(ch, AFF_BITTEN) && ch->wounds > 0)
        send_to_char("Your neck feels slightly bruised and tender, and you feel a little lethargic today.\n\r", ch);
        if (safe_strlen(ch->pcdata->nightmare) > 2) {
          printf_to_char(ch, "You awaken from a horrible nightmare:\n%s\n\r", ch->pcdata->nightmare);
          free_string(ch->pcdata->nightmare);
          ch->pcdata->nightmare = str_dup("");

          AFFECT_DATA af;
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = 750;
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_WEAKEN;
          affect_to_char(ch, &af);
        }
        else if(safe_strlen(ch->pcdata->implant_dream) > 2) {
          printf_to_char(ch, "You awaken after a particularly vivid dream:\n%s\n\r", ch->pcdata->implant_dream);
          free_string(ch->pcdata->implant_dream);
          ch->pcdata->implant_dream = str_dup("");
        }
        if (safe_strlen(ch->pcdata->remember_detail) > 2) {
          if (number_percent() % 2 == 0)
          printf_to_char(ch, "%s\n\r", ch->pcdata->remember_detail);
          free_string(ch->pcdata->remember_detail);
          ch->pcdata->remember_detail = str_dup("");
        }
        if (safe_strlen(ch->pcdata->diss_target) > 1) {
          printf_to_char(ch, "Yesterday you wrote the following roleplay feedback: %s\n\nWould you still like to send this? Type 'send' to send it, or 'undo' to clear it.\n\r", ch->pcdata->diss_message);
          ch->pcdata->diss_sendable = 1;
        }

        ch->pcdata->habits_done = 0;
      }
    }

    if (ch->played / 3600 < 18 && !IS_FLAG(ch->comm, COMM_RACIAL)) {
      SET_FLAG(ch->comm, COMM_RACIAL);
    }
    if (event_cleanse == 1 && !IS_FLAG(ch->comm, COMM_RACIAL)) {
      SET_FLAG(ch->comm, COMM_RACIAL);
    }

    if (event_cleanse == 1 && ch->shape != SHAPE_HUMAN) {
      ch->id = ch->id - ch->shape;
      ch->shape = SHAPE_HUMAN;
    }

    if (ch->pcdata->egg_daddy > 0 && !is_ghost(ch)) {
      if (current_time > (ch->pcdata->egg_date + (3600 * 24 * 7))) {
        free_string(ch->pcdata->deathcause);
        ch->pcdata->deathcause = str_dup("massive internal haemorrhaging.");
        act("Your chest suddenly explodes in a shower of gore and you die.", ch, NULL, NULL, TO_CHAR);
        if (ch->pcdata->egg_daddy == 14) {
          act("$n's chest suddenly explodes in a shower of gore, when it clears several small plants can be seen growing in side the ruins of $s chest cavity.", ch, NULL, NULL, TO_ROOM);
          dact("$n's chest suddenly explodes in a shower of gore, when it clears several small plants can be seen growing in side the ruins of $s chest cavity.", ch, NULL, NULL, DISTANCE_MEDIUM);
        }
        else if (ch->pcdata->egg_daddy == 27) {
          act("$n's chest suddenly explodes in a shower of gore and dozens of small black snakes wiggle free and swarm away in all directions.", ch, NULL, NULL, TO_ROOM);
          dact("$n's chest suddenly explodes in a shower of gore and dozens of small black snakes wiggle free and swarm away in all directions.", ch, NULL, NULL, DISTANCE_MEDIUM);
        }
        else if (ch->pcdata->egg_daddy == 38) {
          act("$n's chest suddenly explodes in a shower of gore and dozens of small black spiders scramble free and swarm away in all directions.", ch, NULL, NULL, TO_ROOM);
          dact("$n's chest suddenly explodes in a shower of gore and dozens of small black spiders scramble free and swarm away in all directions.", ch, NULL, NULL, DISTANCE_MEDIUM);
        }
        real_kill(ch, ch);
        ch->pcdata->egg_date = 0;
        ch->pcdata->egg_daddy = 0;
      }
      else if (current_time > (ch->pcdata->egg_date + (3600 * 24 * 7) - 120)) {
        send_to_char("You feel an agonizing, twisting pain in your stomach.\n\r", ch);
      }
    }

    ch->pcdata->timesincedamage++;

    if (ch->pcdata->ambush > 0 && ch->pcdata->ambush < 50) {
      ch->pcdata->ambush++;
      if (ch->pcdata->ambush >= 50)
      send_to_char("Your ambush is ready.\n\r", ch);
    }

    if (ch->on != NULL && (ch->in_room == NULL || ch->on->in_room == NULL || ch->in_room != ch->on->in_room))
    ch->on = NULL;

    if (ch->timer > 10000 || ch->timer < 0)
    ch->timer = 0;

    ch->timer++;
    if (ch->desc == NULL && !IS_NPC(ch)) {
      ch->timer += 10;
    }

    if ((((!IS_IMMORTAL(ch)) && !is_gm(ch)) || ch->desc == NULL) && ch->timer >= 1440) {
      if (ch->in_room != NULL && ch->in_room->area->vnum == 1) {
        char_from_room(ch);
        char_to_room(ch, get_room_index(1468));
      }
      log_string("Idle Off");
      real_quit(ch);
      return;
    }
    else if (!IS_IMMORTAL(ch) && !IS_FLAG(ch->comm, COMM_AFK) && ch->timer >= 400) {
      SET_FLAG(ch->comm, COMM_AFK);
    }

    if (ch->desc && ch->desc->connected == CON_CREATION)
    return;

    if (global_damage_constant == 1 && ch->in_room != NULL && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      printf_to_char(ch, "%s\n\r", global_constant_message);
      if (max_hp(ch) > 1)
      damage(ch, ch, 1);
    }
    if (global_damage_erratic == 1 && ch->in_room != NULL && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && number_percent() == 87) {
      printf_to_char(ch, "%s\n\r", global_erratic_message);
      wound_char_absolute(ch, 1);
      ch->hit = UMIN(ch->hit, max_hp(ch) / 2);
    }

    if (ch->in_room != NULL && in_lodge(ch->in_room) && IS_FLAG(ch->act, PLR_SHROUD))
    REMOVE_FLAG(ch->act, PLR_SHROUD);
    if (is_ghost(ch) && IS_FLAG(ch->act, PLR_SHROUD))
    REMOVE_FLAG(ch->act, PLR_SHROUD);

    if (ch->hit <= -11) {
      if (ch->hit <= -10000)
      ch->hit = max_hp(ch);
      else
      ch->hit = -10;
    }
    if (ch->hit > max_hp(ch))
    ch->hit = max_hp(ch);

    if (ch->hit < max_hp(ch)) {
      if (ch->in_room != NULL && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && (global_damage_constant == 1 || global_damage_erratic == 1)) {
      }
      else if ((!in_fight(ch) || (!IS_NPC(ch) && is_in_cover(ch) && ch->fight_fast == TRUE && ch->pcdata->timesincedamage >= (6 * fight_speed(ch)))) && !has_con(ch, SCON_NOHEAL)) {
        ch->hit += (2 + shield_total(ch) / 4);

        if (ch->hit >= max_hp(ch)) {
          regened_message(ch);
          ch->hit = max_hp(ch);
        }
      }
    }

    if (crisis_brainwash == 1) {
      bool found = FALSE;

      for (int i = 0; i < 25; i++) {
        if (!str_cmp(ch->pcdata->imprint[i], crisis_brainwash_message) && found == FALSE) {
          found = TRUE;
        }
      }
      if (found == FALSE) {
        auto_imprint(ch, crisis_brainwash_message, IMPRINT_INFLUENCE);
      }
    }

    if (crisis_hurricane == 1 && ch->in_room != NULL && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && number_percent() % 2 == 0 && in_haven(ch->in_room) && !in_fight(ch)) {
      int random = number_range(0, 9);
      if (ch->in_room->exit[random] != NULL && random != DIR_UP && random != DIR_DOWN) {
        act("The wind blows you $T.", ch, NULL, dir_name[random][0], TO_CHAR);
        act("The wind blows $n $T.", ch, NULL, dir_name[random][0], TO_ROOM);
        move_char(ch, random, FALSE, FALSE);
      }
    }

    if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number) {
      save_char_obj(ch, FALSE, FALSE);
    }

    if ((is_safe(ch, ch) && !IS_FLAG(ch->act, PLR_SHROUD)) || in_fantasy(ch) != NULL || higher_power(ch)) {
      if (ch->pcdata->fatigue > 0)
      ch->pcdata->fatigue -= 3;
      if (number_percent() % 3 == 0 && ch->pcdata->fatigue_temp > 0)
      ch->pcdata->fatigue_temp--;
    }

    /*

ch->pcdata->energy_recovery += 1;

ch->pcdata->energy_recovery = ch->pcdata->energy_recovery * 12/10;

if(in_fight(ch))
ch->pcdata->energy_recovery = 0;

if(!is_safe(ch, ch))
ch->pcdata->energy_recovery = UMIN(ch->pcdata->energy_recovery, 10);

ch->pcdata->fatigue -= ch->pcdata->energy_recovery;

ch->pcdata->fatigue = UMAX(ch->pcdata->fatigue, 0);
*/

    if (!in_fight(ch) && IS_FLAG(ch->comm, COMM_GUARDED))
    REMOVE_FLAG(ch->comm, COMM_GUARDED);
    if (!in_fight(ch) && !IS_FLAG(ch->comm, COMM_PACIFIST))
    SET_FLAG(ch->comm, COMM_PACIFIST);

    if (ch->pcdata->police_timer > 0 && ch->pcdata->police_catch_timer < 300)
    ch->pcdata->police_catch_timer++;

    if (ch->pcdata->account != NULL && get_skill(ch, SKILL_WEALTH) > -1 && ch->in_room != NULL && ch->pcdata->lastnotalone > current_time - (3600 * 24 * 21) && ch->in_room->vnum > 100 && !IS_FLAG(ch->act, PLR_DEAD) && ch->race != RACE_FANTASY && !IS_FLAG(ch->act, PLR_STASIS)) {
      if (ch->pcdata->last_paid > current_time)
      ch->pcdata->last_paid = current_time - 1000;
      if (ch->pcdata->last_paid < current_time - 604000 && ch->pcdata->account->paycooldown <
          current_time - (3600 * 24 * 4)) // 7 days
      {
        bool didpay = payday(ch);
        ch->pcdata->last_paid = current_time;
        if (!is_gm(ch) && !higher_power(ch) && didpay == TRUE) {
          ch->pcdata->account->paycooldown = current_time;
        }
      }
    }
    if (IS_FLAG(ch->comm, COMM_RUNNING) && ch->in_room != NULL && room_pop(ch->in_room) <= 1 && !battleground(ch->in_room))
    REMOVE_FLAG(ch->comm, COMM_RUNNING);

    if (ch->pcdata->account != NULL) {
      ch->pcdata->account->factioncooldown = UMIN(
      ch->pcdata->account->factioncooldown, current_time + (3600 * 24 * 3));
      ch->pcdata->account->hightiercooldown = UMIN(
      ch->pcdata->account->hightiercooldown, current_time + (3600 * 24 * 80));
    }
    if (!IS_FLAG(ch->act, PLR_GUEST) && !IS_FLAG(ch->act, PLR_GM) && !IS_IMMORTAL(ch) && ch->in_room != NULL && ch->in_room->vnum > 100) {
      if (ch->pcdata->account != NULL && ch->faction != 0 && clan_lookup(ch->faction) != NULL && clan_lookup(ch->faction)->can_alt == 0 && (ch->in_room != NULL && ch->in_room->vnum > 300)) {
        free_string(ch->pcdata->account->lastfacchar);
        ch->pcdata->account->lastfacchar = str_dup(ch->name);
        ch->pcdata->account->factioncooldown = current_time + (3600 * 24 * 7);
      }
      if (ch->pcdata->account != NULL && get_tier(ch) >= 3 && get_tier(ch) < 6 && !IS_FLAG(ch->act, PLR_GUEST) && !IS_FLAG(ch->act, PLR_STASIS) && !IS_FLAG(ch->act, PLR_SINSPIRIT) && ch->in_room != NULL && ch->in_room->vnum > 100 && ch->desc != NULL && (current_time > ch->pcdata->account->hightiercooldown || !str_cmp(ch->name, ch->pcdata->account->lasttierchar))) {
        free_string(ch->pcdata->account->lasttierchar);
        ch->pcdata->account->lasttierchar = str_dup(ch->name);
        if (get_tier(ch) == 3)
        ch->pcdata->account->hightiercooldown =
        UMAX(ch->pcdata->account->hightiercooldown, current_time + (3600 * 24 * 7));
        else if (get_tier(ch) == 4)
        ch->pcdata->account->hightiercooldown =
        UMAX(ch->pcdata->account->hightiercooldown, current_time + (3600 * 24 * 30));
        else if (get_tier(ch) == 5)
        ch->pcdata->account->hightiercooldown = current_time + (3600 * 24 * 60);
      }
    }

    if (ch->pcdata->mark_timer[0] > 0)
    ch->pcdata->mark_timer[0]--;

    if (ch->wounds < 1)
    ch->pcdata->mark_timer[1] = 0;
    if (ch->wounds < 2)
    ch->pcdata->mark_timer[2] = 0;
    if (ch->wounds < 3)
    ch->pcdata->mark_timer[3] = 0;

    if (ch->pcdata->haunt_timer > 0 && ch->pcdata->maintained_ritual != RITUAL_HAUNT)
    ch->pcdata->haunt_timer--;

    if (ch->pcdata->order_timer > 0) {
      ch->pcdata->order_timer--;
      if (ch->pcdata->order_timer == 0)
      process_order(ch, ch->pcdata->order_type);
    }

    if (ch->pcdata->tail_timer > 0)
    ch->pcdata->tail_timer--;

    if (ch->pcdata->connected_to != NULL && ch->pcdata->connection_stage == CONNECT_DIALING && get_phone(ch->pcdata->connected_to) != NULL) {
      obj = get_phone(ch->pcdata->connected_to);

      if (IS_SET(obj->extra_flags, ITEM_SILENT)) {
        act("Your phone vibrates.", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);
      }
      else {

        sprintf(buf, "$n's phone rings.");
        sprintf(buf2, "Your phone rings.");
        for (ed = obj->extra_descr; ed; ed = ed->next) {
          if (is_name("+ringtone", ed->keyword)) {
            sprintf(buf, "$n's phone plays %s.", ed->description);
            sprintf(buf2, "Your phone plays %s.", ed->description);
          }
        }
        act(buf, ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_ROOM);
        act(buf2, ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);
      }
      act("The phone keeps ringing.", ch, NULL, ch, TO_CHAR);
    }

    if (ch->in_room != NULL) {
      if (is_raining(ch->in_room) && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      wetify(ch);
      else if (is_hailing(ch->in_room) && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      snowify(ch);
      else if (is_snowing(ch->in_room) && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
      snowify(ch);
      else
      dry(ch);
    }
    sex_update(ch);

    if (ch->in_room != NULL && event_cleanse == 0) {
      if (get_skill(ch, SKILL_MERSHIFTER) > 0 && (deep_water(ch)) && ch->shape == SHAPE_HUMAN && ch->pcdata->wetness > 40) {
        char cto[MSL];
        sprintf(cto, "%s", ch->pcdata->mermaid_change_to);
        if(strlen(cto) > 2)
        {
          act(cto, ch, NULL, NULL, TO_CHAR);
          act(cto, ch, NULL, NULL, TO_ROOM);
        }
        sprintf(buf, "$n turns into %s.", ch->pcdata->mermaidintro);
        act(buf, ch, NULL, NULL, TO_ROOM);
        ch->shape = SHAPE_MERMAID;
        pants_off(ch);
        printf_to_char(ch, "You turn into %s.\n\r", ch->pcdata->mermaidintro);
      }
      else if (get_skill(ch, SKILL_MERSHIFTER) > 0 && (deep_water(ch)) && ch->shape == SHAPE_HUMAN && ch->pcdata->wetness < 30) {
        send_to_char("Your legs tingle.\n\r", ch);
      }
      else if ((!deep_water(ch) && ch->shape == SHAPE_MERMAID && ch->pcdata->wetness < 10)) {
        char cto[MSL];
        sprintf(cto, "%s", ch->pcdata->mermaid_change_from);
        if(strlen(cto) > 2)
        {
          act(cto, ch, NULL, NULL, TO_CHAR);
          act(cto, ch, NULL, NULL, TO_ROOM);
        }
        sprintf(buf, "$n turns back into a human.");
        act(buf, ch, NULL, NULL, TO_ROOM);
        ch->shape = SHAPE_HUMAN;
        printf_to_char(ch, "You turn back into a human.\n\r");
      }
    }

    if (!is_undead(ch) && !is_gm(ch) && ch->in_room != NULL && number_percent() % 12 == 0 && ch->race != RACE_FANTASY && !higher_power(ch)) {
      int temp = temperature(ch->in_room);
      if (ch->pcdata->wetness > 40)
      temp -= 12;
      else if (ch->pcdata->wetness > 0)
      temp -= 6;

      if (ch->in_room->sector_type == SECT_ATMOSPHERE)
      temp -= 8;
      else if (ch->in_room->sector_type == SECT_AIR)
      temp -= 3;

      // Labor and childbirth - Discordance
      if (ch->pcdata->due_date != 0) {
        if (!IS_AFFECTED(ch, AFF_CONTRACTIONS)) {
          int length;
          if (ch->pcdata->due_date < current_time) {
            heres_johnny(ch);
            return;
          }
          length = number_range(1, 5); // Minutes
          length = length * 12;
          apply_contractions(ch, length);
        }
      }

      if (ch->in_room->vnum > 300 && !is_dreaming(ch)) { // No shivering/sweating in Limbo - Discordance
        if (is_water(ch->in_room) && ch->pcdata->wetness < 30 && !water_breathe(ch)) {
          send_to_char("You shiver.\n\r", ch);
          return;
        }
        else if (!is_water(ch->in_room) && ch->shape == SHAPE_HUMAN && !is_ghost(ch) && !is_dreaming(ch)) {
          if (temp < 53) {
            int modtemp = get_warmth(ch);

            modtemp = modtemp * 6 / 5;

            modtemp /= 3;
            temp += modtemp;
            if (climatecon(ch->in_room)) {
              temp += 50;
              if (temp < 53)
              temp = 53;
            }
            if (is_possessed(ch))
            temp -= 20;
            if (get_skill(ch, SKILL_COLDSPEC) > 0)
            temp += 20;

            if ((temp < 53 || is_possessed(ch)) && get_skill(ch, SKILL_ICEPROOF) < 1 && number_percent() % 5 == 0) {
              send_to_char("You shiver.\n\r", ch);
              return;
            }
          }
          else if (temp > 68) {
            int modtemp = get_warmth(ch);
            modtemp /= 5;
            temp += modtemp;

            if (climatecon(ch->in_room)) {
              temp -= 25;
              if (temp > 68)
              temp = 68;
            }
            if (is_possessed(ch))
            temp -= 20;
            if (get_skill(ch, SKILL_HOTSPEC) > 0)
            temp -= 20;

            if (temp > 95 && get_skill(ch, SKILL_FIREPROOF) < 1 && number_percent() % 5 == 0) {
              send_to_char("You sweat.\n\r", ch);
              return;
            }
          }
        }
      }
    }

    if (is_gm(ch)) {
      if (ch->pcdata->cop_cooldown > 0)
      ch->pcdata->cop_cooldown--;
    }

    if (!in_fight(ch)) {
      for (int i = 0; i < 20; i++) {
        if (ch->pcdata->specialcools[i][2] > 0)
        ch->pcdata->specialcools[i][2] = 0;
        if (ch->pcdata->specialcools[i][3] > 0)
        ch->pcdata->specialcools[i][3] = 0;
      }

      if (!room_fight(ch->in_room, FALSE, FALSE, TRUE)) {
        if (!is_dreaming(ch) && ch->pcdata->sleeping > 500) {
          ch->pcdata->sleeping = 2;
          ch->pcdata->dream_cooldown = 0;
        }

        if (ch->pcdata->sleeping > 10000 || ch->pcdata->sleeping < 0)
        ch->pcdata->sleeping = 0;
        if (ch->pcdata->coma > current_time)
        ch->pcdata->sleeping = 50;
        else if (ch->pcdata->sleeping > 0 && ch->desc != NULL) {
          if (!is_dreaming(ch) || ch->pcdata->sleeping > 15)
          ch->pcdata->sleeping--;
          if (ch->pcdata->sleeping == 0)
          send_to_char("You wake up.\n\r", ch);
        }
      }
    }
    if (ch->pcdata->desclock > 0)
    ch->pcdata->desclock--;

    if (is_helpless(ch))
    ch->pcdata->captive_timer++;
    else
    ch->pcdata->captive_timer -= 50;

    if (ch->pcdata->sleeping == 0) {
      ch->pcdata->dream_room = 0;
      ch->pcdata->dream_invite = 0;
    }

    if (ch->pcdata->captive_timer < 0)
    ch->pcdata->captive_timer = 0;

    if (ch->pcdata->poisontimer > 0) {
      ch->pcdata->poisontimer--;
      if (ch->pcdata->poisontimer == 0)
      poison_char(ch);
    }
    if (ch->pcdata->rohyptimer > 0) {
      ch->pcdata->rohyptimer--;
      if (ch->pcdata->rohyptimer == 0) {
        send_to_char("You start feeling a bit sleepy.\n\r", ch);
        act("$n blinks a few times and then slumps sideways and passes out.", ch, NULL, NULL, TO_ROOM);
        ch->pcdata->sleeping = 490;
      }
    }

    process_feedback(ch);

    scon_update(ch);

    if (ch->pcdata->tresspassing > 0 && ch->desc != NULL) {
      ch->pcdata->tresspassing--;
      if (ch->pcdata->tresspassing == 16) {
        act("The police arrive and ask you to leave.", ch, NULL, NULL, TO_CHAR);
        act("The police arrive and ask $n to leave.", ch, NULL, NULL, TO_ROOM);
      }
      if (ch->pcdata->tresspassing == 0) {
        ch->pcdata->police_number = 2;
        ch->pcdata->police_intensity = 1;
        ch->pcdata->police_timer = 720;
        copspank(ch);
      }
    }

    if (ch->pcdata->earnt_xp < 0 || ch->pcdata->earnt_xp > 20000)
    ch->pcdata->earnt_xp = 0;

    if (!in_fight(ch) && ch->pcdata->earnt_xp > 0) {

      printf_to_char(ch, "You earned %d experience", ch->pcdata->earnt_xp);
      ch->pcdata->earnt_xp = 0;

      if (event_teaching != 0 && ch->faction == event_teaching)
      send_to_char(", teaching", ch);

      send_to_char(".\n\r", ch);

      /*
if(!IS_FLAG(ch->comm, COMM_BRIEF))
{
SET_FLAG(ch->comm, COMM_BRIEF);
do_function(ch, &do_look, "auto");
REMOVE_FLAG(ch->comm, COMM_BRIEF);
}
else
do_function(ch, &do_look, "auto");
*/
    }

    debloodify(ch);
    /*
if(in_combat(ch) && ch->attack_turn == 0 && ch->actiontimer > 0)
{
ch->actiontimer-= 5;
if(ch->actiontimer <= 0)
next_attacker(ch->in_room);
else if(ch->actiontimer == 120)
send_to_char("Two minutes left to write your combat emote.\n\r", ch);
else if(ch->actiontimer == 60)
send_to_char("One minute left to write your combat emote.\n\r", ch); else if(ch->actiontimer == 30) send_to_char("Thirty seconds left to
write your combat emote.\n\r", ch);
}
*/

    if (is_gm(ch) && !in_fight(ch) && ch->pcdata->currentmonster > 0)
    ch->pcdata->currentmonster = UMAX(0, ch->pcdata->currentmonster - 10);

    if (ch->pcdata->stories_countdown > 0) {
      ch->pcdata->stories_countdown--;
      if (ch->pcdata->stories_countdown == 0 && ch->pcdata->stories_run > 0) {
        ch->pcdata->stories_run--;
        ch->pcdata->stories_countdown = 450000;
      }
    }
    if (ch->pcdata->rooms_countdown > 0) {
      ch->pcdata->rooms_countdown--;
      if (ch->pcdata->rooms_countdown == 0 && ch->pcdata->rooms_run > 0) {
        ch->pcdata->rooms_run--;
        ch->pcdata->rooms_countdown = 450000;
      }
    }

    if (ch->wounds > 0 && get_skill(ch, SKILL_HYPERREGEN) > 0 && !in_fight(ch)) {
      if (ch->heal_timer > 50)
      ch->heal_timer = ch->heal_timer * 19 / 20;
    }

    // Add some checks to see jump scale healing up if the character had sex with
    // a demigod recently - Discordance
    if (ch->wounds > 0 && !in_fight(ch) && has_attendant(ch)) {

      if (ch->wounds == 1) {
        if (can_heal(ch)) {
          if (IS_FLAG(ch->comm, COMM_PRIVATE))
          ch->heal_timer += 5;
          else if (get_skill(ch, SKILL_REGEN) >= 3) {
            ch->heal_timer -= 104;
          }
          // Regen 1 and demigod sex buff
          else if (get_skill(ch, SKILL_REGEN) >= 1) {
            ch->heal_timer -= 34;
          }
          else
          ch->heal_timer -= 3;

          if (ch->heal_timer <= 0)
          ch->wounds = 0;
        }
      }
      else if (ch->wounds == 2) {
        if (in_hospital(ch) || get_skill(ch, SKILL_REGEN) >= 1) {
          if (IS_FLAG(ch->comm, COMM_PRIVATE))
          ch->heal_timer += 5;
          else if (get_skill(ch, SKILL_REGEN) >= 3)
          ch->heal_timer -= 47;
          // Regen 1 and demigod sex
          else if (get_skill(ch, SKILL_REGEN) >= 1)
          ch->heal_timer -= 12;
          else
          ch->heal_timer -= 3;
          if (ch->heal_timer <= 0) {
            ch->wounds = 1;
            ch->heal_timer = 25000;
          }
        }
      }
    }

    if (ch->wounds > 0 && !room_fight(ch->in_room, FALSE, FALSE, TRUE)) {
      if (ch->wounds == 1) {
        if (higher_power(ch))
        ch->heal_timer -= 500;
        if (can_heal(ch)) {
          if (IS_FLAG(ch->comm, COMM_PRIVATE))
          ch->heal_timer += 5;
          else if (get_skill(ch, SKILL_REGEN) >= 3) {
            ch->heal_timer -= 84;
          }
          // Regen 1 and demigod sex
          else if (get_skill(ch, SKILL_REGEN) >= 1) {
            ch->heal_timer -= 24;
          }
          else
          ch->heal_timer -= 2;
          if (ch->heal_timer <= 0)
          ch->wounds = 0;
        }
      }
      else if (ch->wounds == 2) {
        if (higher_power(ch))
        ch->heal_timer -= 500;
        if (IS_FLAG(ch->comm, COMM_PRIVATE))
        ch->heal_timer += 5;
        else if (get_skill(ch, SKILL_REGEN) >= 3)
        ch->heal_timer -= 37;
        // Regen 1 and demigod sex
        else if (get_skill(ch, SKILL_REGEN) >= 1)
        ch->heal_timer -= 7;
        else
        ch->heal_timer -= 2;
        if (ch->heal_timer <= 0) {
          ch->wounds = 1;
          ch->heal_timer = 25000;
        }
      }
      else if (ch->wounds == 3) {
        if (in_hospital(ch) || is_undead(ch) || higher_power(ch)) {
          if (higher_power(ch))
          ch->heal_timer -= 500;
          ch->heal_timer -= 2;
          if (ch->heal_timer <= 0) {
            ch->wounds = 2;
            ch->heal_timer = 125000;
          }
        }
        else {
          if (!is_undead(ch)) // && get_gm(ch->in_room, TRUE) != NULL)
          {
            if (ch->death_timer % 24 == 0) {
              send_to_char("You feel the world fading away as you bleed.\n\r", ch);
              if (get_gm(ch->in_room, FALSE) != NULL)
              printf_to_char(get_gm(ch->in_room, FALSE), "%s will bleed out in %d minutes, type heal to reset this timer.\n\r", NAME(ch), ch->death_timer / 12);
            }
            ch->death_timer--;

            if (ch->death_timer <= 0) {
              char temp[MSL];
              sprintf(temp, "Blood loss brought on by %s", ch->pcdata->deathcause);
              free_string(ch->pcdata->deathcause);
              ch->pcdata->deathcause = str_dup(temp);

              real_kill(ch, ch);
              return;
            }
          }
        }
      }
      /*
else if(ch->wounds == 4 && ch->heal_timer > 0)
{
char temp[MSL];
sprintf(temp, "Blood loss brought on by %s", ch->pcdata->deathcause); free_string(ch->pcdata->deathcause);
ch->pcdata->deathcause = str_dup(temp);

real_kill(ch, ch);
return;
}
*/
      else if (ch->wounds > 3)
      ch->wounds = 0;
    }

    if (mentoring_eligible_ic(ch) || mentoring_eligible_ooc(ch)) {
      if (!str_cmp(ch->pcdata->mentor, "") || safe_strlen(ch->pcdata->mentor) < 3) {
        if (ch->pcdata->mentor_timer <= 0 || ch->pcdata->mentor_timer > 1000) {
          int max_id = 0;
          int max_amount = 0;
          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;
            if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
              to = d->character;
              if (IS_NPC(to))
              continue;
              if (to == ch)
              continue;

              if (IS_AFFECTED(to, AFF_NOMENTOR))
              continue;

              if (IS_FLAG(to->comm, COMM_NOCHANNELS))
              continue;

              if (to->played / 3600 < 20)
              continue;

              if (safe_strlen(to->pcdata->mentor) > 2)
              continue;

              if (IS_IMMORTAL(to) || IS_FLAG(to->act, PLR_GM))
              continue;

              if (ch->faction != 0 && ch->faction != to->faction && ch->faction != to->factiontwo)
              continue;

              if (mentor_weight(ch, to) > max_amount) {
                max_amount = mentor_weight(ch, to) + number_range(0, 50);
                max_id = to->id;
              }
            }
          }
          if (max_amount > 0 && (to = get_char_by_id(max_id)) != NULL) {
            ch->pcdata->mentor_timer = 50;
            ch->pcdata->mentor_of = max_id;
            sprintf(buf, "Mentor Offer:%s:%s", ch->name, to->name);
            log_string(buf);
            if (!IS_FLAG(ch->comm, COMM_MENTOR))
            SET_FLAG(ch->comm, COMM_MENTOR);
            if (!mentoring_eligible_ooc(ch))
            printf_to_char(
            to, "You have been automatically assigned as an IC mentor to %s, you can `gmentor reject`x this offer anytime, or `gmentor accept`x it within the next half an hour. See`g help mentor`x for more information.\n\r", ch->name);
            else
            printf_to_char(
            to, "You have been automatically assigned as an IC and OOC mentor to %s, you can `gmentor reject`x this offer anytime, or `gmentor accept`x it within the next half an hour. See`g help mentor`x for more information.\n\r", ch->name);

            AFFECT_DATA af;
            af.where = TO_AFFECTS;
            af.type = 0;
            af.level = 10;
            af.duration = 180;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.caster = NULL;
            af.weave = FALSE;
            af.bitvector = AFF_NOMENTOR;
            affect_to_char(to, &af);
          }
        }
        else
        ch->pcdata->mentor_timer--;
      }
    }
    else {
    }
    if (!in_fight(ch) && number_percent() % 2 == 0) {
      ch->x += number_percent() % 3;
      ch->x--;
      ch->y += number_percent() % 3;
      ch->y--;
    }
    if (ch->in_room != NULL) {
      if (ch->x > ch->in_room->size)
      ch->x = ch->in_room->size;
      if (ch->x < 0)
      ch->x = 0;
      if (ch->y > ch->in_room->size)
      ch->y = ch->in_room->size;
      if (ch->y < 0)
      ch->y = 0;
    }

    update_clothes(ch);

    if (number_percent() % 12 == 0) {
      switch (number_percent() % 8) {
        AFFECT_DATA af;
      case 1:
        if (get_drunk(ch) >= 20 && ch->wounds < 2)
        send_to_char("You feel really happy.\n\r", ch);
        break;
      case 2:
        if (get_drunk(ch) >= 30 && ch->wounds < 2)
        send_to_char("You want to tell people how you really feel.\n\r", ch);
        break;
      case 3:
        if (get_drunk(ch) >= 60 && ch->wounds < 2)
        act("$n giggles.", ch, NULL, NULL, TO_ROOM);
        break;
      case 4:
        if (get_drunk(ch) >= 70 && !IS_AFFECTED(ch, AFF_DISORIENTATION)) {
          af.where = TO_AFFECTS;
          af.type = 0;
          af.level = 10;
          af.duration = number_range(5, 12);
          af.location = APPLY_NONE;
          af.modifier = 0;
          af.caster = NULL;
          af.weave = FALSE;
          af.bitvector = AFF_DISORIENTATION;
          affect_to_char(ch, &af);
        }
        break;
      case 5:
        if (get_drunk(ch) >= 80 && ch->position > POS_SLEEPING) {
          send_to_char("You start to feel dizzy, as the world rocks you find yourself sitting abruptly down.\n\r", ch);
          do_function(ch, &do_sit, "");
        }
      case 6:
        if (get_drunk(ch) >= 90) {
          start_vomit(ch);
          /*
send_to_char("You feel a strange sensation
in your stomach, then suddenly vomit onto the floor.\n\r", ch);
act("$n vomits onto the floor.", ch, NULL, NULL, TO_ROOM); ch->pcdata->conditions[COND_DRUNK] -= 5;
if(ch->pcdata->attract_mod > -30)
{
ch->pcdata->attract_mod -= 20;
}
*/
        }
      case 7:
        if (get_drunk(ch) >= 100) {
          send_to_char("You pass out.\n\r", ch);
          act("$n passes out.", ch, NULL, NULL, TO_ROOM);
          ch->pcdata->sleeping = 10;
        }
      }
    }

    if (ch->pcdata->penalty > 0) {
      ch->pcdata->penalty--;

      if (ch->pcdata->penalty == 0 && IS_FLAG(ch->comm, COMM_NOCHANNELS)) {
        REMOVE_FLAG(ch->comm, COMM_NOCHANNELS);
        send_to_char("Your channels have been reactivated.\n\r", ch);
      }
    }

    if (!in_fight(ch) || ch->fight_fast == TRUE) {

      for (paf = ch->affected; paf != NULL; paf = paf_next) {
        paf_next = paf->next;

        if (paf->duration > 0) {
          if (paf->bitvector != AFF_ABDUCTED || !is_helpless(ch) || in_lodge(ch->in_room))
          paf->duration--;
        }
        else {
          if (paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0) {
          }
          affect_remove(ch, paf);
        }
      }
    }

    return;
  }

  void resurrection(OBJ_DATA *corpse) {
    struct stat sb;
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    d.original = NULL;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *room;
    char buf[MSL];
    sprintf(buf, "HYPERREGEN: %s coming back\n\r", corpse->material);
    log_string(buf);

    if (event_cleanse == 1)
    return;

    if ((victim = get_char_world_pc(corpse->material)) !=
        NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Ressurection");

      if (!load_char_obj(&d, corpse->material)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(corpse->material));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }
    if (guestmonster(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    if (corpse->in_room != NULL)
    room = corpse->in_room;
    else if (corpse->carried_by != NULL && corpse->carried_by->in_room != NULL)
    room = corpse->carried_by->in_room;
    else if (corpse->in_obj != NULL && corpse->in_obj->in_room != NULL)
    room = corpse->in_obj->in_room;
    else if (corpse->in_obj != NULL && corpse->in_obj->carried_by != NULL && corpse->in_obj->carried_by->in_room != NULL)
    room = corpse->in_obj->carried_by->in_room;
    else
    room = get_room_index(1000);

    if (IS_FLAG(victim->act, PLR_DEAD)) {
      REMOVE_FLAG(victim->act, PLR_DEAD);
      if (is_ghost(victim)) {
        REMOVE_FLAG(victim->act, PLR_GHOST);
      }
      victim->pcdata->final_death_date = 0;
    }

    char_from_room(victim);
    char_to_room(victim, room);

    for (obj = corpse->contains; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_char(obj, victim);
    }
    extract_obj(corpse);
    if (online)
    act("You wake up suddenly.", victim, NULL, NULL, TO_CHAR);
    act("$n suddenly wakes up.", victim, NULL, NULL, TO_ROOM);

    save_char_obj(victim, FALSE, FALSE);
    if (!online)
    free_char(victim);
  }

  bool is_decay_room(ROOM_INDEX_DATA *room) {

    if (is_privatep(room))
    return FALSE;

    if (room->sector_type == SECT_AIR || room->sector_type == SECT_ATMOSPHERE)
    return TRUE;

    if (get_roomy(room) == 45 && number_percent() % 14 != 0)
    return FALSE;

    if (room->sector_type == SECT_PARK || room->sector_type == SECT_FOREST || room->sector_type == SECT_WATER || room->sector_type == SECT_UNDERWATER || room->sector_type == SECT_SHALLOW)
    return TRUE;

    return FALSE;
  }

  int washuproom(void) {
    int x = 205473450;
    int y = number_percent() % 8;
    y -= 4;
    x += y * 1000;

    return x;
  }

  void obj_update(void) {
    int p = 0;

    save_ground_objects();

    for (ObjList::iterator it = object_list.begin();
    it != object_list.end() && p < 500000; p++) {
      OBJ_DATA *obj = *it;
      ++it;

      if (obj == NULL)
      continue;

      if (!obj)
      continue;

      if (obj->pIndexData == NULL)
      continue;

      if (obj->item_type == ITEM_FLASHLIGHT && !IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 0)
      obj->value[0]--;

      stolen_update(obj);

      if (obj->in_room != NULL && pc_pop(obj->in_room) < 1 && number_percent() % 7 == 0 && obj->in_room->vnum >= INIT_FIGHTROOM && obj->in_room->vnum <= END_FIGHTROOM) {
        obj_from_room(obj);
        obj_to_room(obj, get_room_index(washuproom()));
      }
      /*
if(obj->owned_by == NULL && CAN_WEAR(obj, ITEM_TAKE) && obj->owned_time <=
0 && !IS_SET(obj->extra_flags, ITEM_NODECAY) && obj->in_obj == NULL && obj->item_type != ITEM_FURNITURE) obj->condition--;
*/

      if (obj->item_type == ITEM_PHONE && obj->carried_by != NULL) {
        update_phonebook(obj->value[0], obj->carried_by->name);
        if (obj->value[0] == 7124702 && obj->carried_by->in_room != NULL && obj->carried_by->in_room->vnum != 16269) {
          obj_from_char(obj);
          obj_to_room(obj, get_room_index(16269));
        }
      }

      if (obj->in_room != NULL && is_decay_room(obj->in_room) && obj->item_type != ITEM_CORPSE_NPC && room_pop(obj->in_room) < 1 && !IS_SET(obj->extra_flags, ITEM_RELIC) && number_percent() % 10 == 0)
      obj->condition--;
      else if (obj->carried_by != NULL && obj->condition < 100)
      obj->condition++;

      if (obj->in_room != NULL && obj->in_room->vnum >= 50 && obj->in_room->vnum <= 65)
      obj->condition -= UMIN(10, obj->condition);

      if (--obj->rot_timer < 0)
      obj->rot_timer = -1;

      if (--obj->timer < 0)
      obj->timer = -1;

      if (obj->timer == 0 && safe_strlen(obj->material) > 2 && obj->item_type == ITEM_CORPSE_NPC) {
        resurrection(obj);
        continue;
      }

      if (obj->rot_timer != 0 && obj->item_type == ITEM_CORPSE_NPC)
      continue;

      if (obj->timer != 0 && obj->rot_timer != 0 && obj->condition > 0)
      continue;

      if ((obj->item_type == ITEM_CORPSE_PC) && obj->contains) {
        OBJ_DATA *t_obj, *next_obj;
        p = 0;

        for (t_obj = obj->contains; t_obj != NULL && p < 1000; t_obj = next_obj) {
          p++;
          next_obj = t_obj->next_content;
          obj_from_obj(t_obj);

          if (obj->in_obj) // in another object
          obj_to_obj(t_obj, obj->in_obj);

          else if (obj->carried_by) // carried
          obj_to_char(t_obj, obj->carried_by);

          else if (obj->in_room == NULL) // destroy it
          extract_obj(t_obj);

          else // to a room
          obj_to_room(t_obj, obj->in_room);
        }
      }

      extract_obj(obj);
    }

    return;
  }

  void aggr_update(void) { return; }

  void update_handler(void) {
    static int pulse_second;
    static int pulse_fight;
    static int pulse_area;
    static int pulse_point;
    static int pulse_fight_check;
    static int pulse_mud_hour;
    static int pulse_per_hour;
    static int pulse_roleplay;
    static int pulse_obj;

    char buf[MSL];
    char buftwo[MSL];
    int start = current_time;
    if (--pulse_fight <= 0) {
      pulse_fight = PULSE_VIOLENCE;
    }

    if (--pulse_second <= 0) {
      sprintf(buf, "LAGCHECK: Second Start %d", start);
      pulse_second = PULSE_SECOND;
      fight_update();
      second_update();
      ai_update();
      int newtime = current_time;
      sprintf(buftwo, "LAGCHECK: Second End %d: %d", newtime, newtime - start);
      if (newtime > start) {
        bug(buf, 0);
        bug(buftwo, 0);
      }
    }
    if (++clock_second >= (60 * PULSE_PER_SECOND)) {
      clock_second = 0;
      clock_update();
    }

    if (--pulse_area <= 0) {
      start = current_time;
      sprintf(buf, "LAGCHECK: Area Start %d", start);
      pulse_area = PULSE_AREA;
      save_area_list();
      for (AreaList::iterator it = area_list.begin(); it != area_list.end();
      ++it) {
        if (IS_SET((*it)->area_flags, AREA_CHANGED)) {
          save_area(*it, FALSE);
          REMOVE_BIT((*it)->area_flags, AREA_CHANGED);
        }
        else if ((*it)->vnum % 22 + 1 == time_info.hour) {
          reset_area((*it), FALSE);
          save_area(*it, FALSE);
        }
      }

      int newtime = current_time;
      sprintf(buftwo, "LAGCHECK: Area End %d: %d", newtime, newtime - start);
      if (newtime > start) {
        bug(buf, 0);
        bug(buftwo, 0);
      }
    }

    if (--pulse_fight_check <= 0) {
      fight_check_update();
      pulse_fight_check = PULSE_FIGHT_CHECK;
    }

    if (--pulse_mud_hour <= 0) {

      start = current_time;
      sprintf(buf, "LAGCHECK: Time Start %d", start);
      time_update();
      // room_exit_update();
      pulse_mud_hour = PULSE_MUD_HOUR;
      int newtime = current_time;
      sprintf(buftwo, "LAGCHECK: Time End %d: %d", newtime, newtime - start);
      if (newtime > start) {
        bug(buf, 0);
        bug(buftwo, 0);
      }
    }

    if (--pulse_obj <= 0) {
      obj_update();
      pulse_obj = PULSE_OBJ;
    }

    if (--pulse_point <= 0) {

      start = current_time;
      sprintf(buf, "LAGCHECK: Tick Start %d", start);
      wiznet("TICK!", NULL, NULL, WIZ_TICKS, 0, 0);
      pulse_point = PULSE_TICK;
      if (--pulse_roleplay <= 0)
      pulse_roleplay = 7;
      check_muddeath();
      check_copyover();
      char_update(pulse_per_hour, pulse_roleplay);
      int newtime = current_time;
      sprintf(buftwo, "LAGCHECK: Tick End %d: %d", newtime, newtime - start);
      if (newtime > start) {
        bug(buf, 0);
        bug(buftwo, 0);
      }
    }

    if (--pulse_per_hour <= 0) {
      pulse_per_hour = PULSE_PER_HOUR;
    }
    tail_chain();
    return;
  }

  void check_muddeath(void) {
    char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (pulse_muddeath < 0)
    return;

    if (pulse_muddeath == 0)
    reboot_shutdown(mudkiller, isreboot);

    if (pulse_muddeath > 0) {
      sprintf(buf, "%s in %d seconds(s)\n\r", isreboot ? "Reboot" : "Shutdown", pulse_muddeath * 5);
      do_echo(mudkiller, buf);
    }

    --pulse_muddeath;

    return;
  }

  void check_copyover(void) {
    char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (pulse_copyover < 0)
    return;

    if (pulse_copyover == 0)
    exec_copyover(mudkiller);

    if (pulse_copyover > 0) {
      sprintf(buf, "Copyover in %d tick(s)\n\r", pulse_copyover);
      do_echo(mudkiller, buf);
    }

    --pulse_copyover;

    return;
  }

  void hint_update(void) { return; }

  void explode_obj(OBJ_DATA *obj, ROOM_INDEX_DATA *room) {}

  int get_mob_lvl(int i) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    int temp;
    pMobIndex = get_mob_index(i);
    victim = create_mobile(pMobIndex);
    temp = get_cslvl(victim);
    return temp;
  }

  bool is_waterproof(OBJ_DATA *obj) {

    if (IS_SET(obj->extra_flags, ITEM_WATERPROOF))
    return TRUE;

    if (is_name("leather", obj->name))
    return TRUE;

    if (is_name("suede", obj->name))
    return TRUE;

    if (is_name("nylon", obj->name))
    return TRUE;

    if (is_name("vinyl", obj->name))
    return TRUE;

    if (is_name("wool", obj->name))
    return TRUE;

    return FALSE;
  }

  bool can_get_wet(CHAR_DATA *ch, int iWear) {
    if (!in_water(ch)) {
      OBJ_DATA *obj = get_eq_char(ch, WEAR_HOLD);

      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF))
      return FALSE;

      obj = get_eq_char(ch, WEAR_HOLD_2);

      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF))
      return FALSE;

      for (int i = 0; i < MAX_WEAR; i++) {
        if ((obj = get_eq_char(ch, i)) == NULL)
        continue;

        if (!is_waterproof(obj))
        continue;

        if (does_conceal(obj, get_eq_char(ch, iWear)))
        return FALSE;
      }
    }

    if (get_eq_char(ch, iWear)->item_type != ITEM_CLOTHING)
    return FALSE;

    if (is_waterproof(get_eq_char(ch, iWear)))
    return FALSE;

    return TRUE;
  }

  void wetify(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;

    if (IS_NPC(ch)) {
      return;
    }

    if (!in_water(ch)) {
      // umbrellas
      obj = get_eq_char(ch, WEAR_HOLD);
      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF)) {
        dry(ch);
        return;
      }
      obj = get_eq_char(ch, WEAR_HOLD_2);
      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF)) {
        dry(ch);
        return;
      }

      // Umbrella places - Disco 10/28/2017
      if (strcasestr(ch->pcdata->place, "umbrella") != NULL) {
        dry(ch);
        return;
      }
      // other structures
      if (strcasestr(ch->pcdata->place, "tent") != NULL && has_place(ch->in_room, "tent")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "marquee") != NULL && has_place(ch->in_room, "marquee")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "carport") != NULL && has_place(ch->in_room, "carport")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "gazebo") != NULL && has_place(ch->in_room, "gazebo")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "overhang") != NULL && has_place(ch->in_room, "overhang")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "porch") != NULL && has_place(ch->in_room, "porch")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "pergola") != NULL && has_place(ch->in_room, "pergola")) {
        dry(ch);
        return;
      }
      if (strcasestr(ch->pcdata->place, "awning") != NULL && has_place(ch->in_room, "awning")) {
        dry(ch);
        return;
      }
    }

    if (ch->shape == SHAPE_HUMAN) {
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) != NULL && (can_get_wet(ch, iWear))) {
          obj->value[4] = UMIN(obj->value[4] + 3, 200);
        }
      }
    }

    ch->pcdata->wetness = UMIN(ch->pcdata->wetness + 3, 100);

    if (in_water(ch))
    ch->pcdata->wetness = UMIN(ch->pcdata->wetness + 7, 100);
  }

  void snowify(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;

    if (IS_NPC(ch)) {
      return;
    }

    if (!in_water(ch)) {
      // umbrellas
      obj = get_eq_char(ch, WEAR_HOLD);
      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF))
      return;
      obj = get_eq_char(ch, WEAR_HOLD_2);
      if (obj != NULL && obj->item_type == ITEM_UMBRELLA && !IS_SET(obj->extra_flags, ITEM_OFF))
      return;
      if (strcasestr(ch->pcdata->place, "umbrella") !=
          NULL) // Umbrella places - Disco 10/28/2017
      return;

      // other structures
      if (strcasestr(ch->pcdata->place, "tent") != NULL && has_place(ch->in_room, "tent"))
      return;
      if (strcasestr(ch->pcdata->place, "marquee") != NULL && has_place(ch->in_room, "marquee"))
      return;
      if (strcasestr(ch->pcdata->place, "carport") != NULL && has_place(ch->in_room, "carport"))
      return;
      if (strcasestr(ch->pcdata->place, "gazebo") != NULL && has_place(ch->in_room, "gazebo"))
      return;
      if (strcasestr(ch->pcdata->place, "overhang") != NULL && has_place(ch->in_room, "overhang"))
      return;
      if (strcasestr(ch->pcdata->place, "porch") != NULL && has_place(ch->in_room, "porch"))
      return;
      if (strcasestr(ch->pcdata->place, "pergola") != NULL && has_place(ch->in_room, "pergola"))
      return;
      if (strcasestr(ch->pcdata->place, "awning") != NULL && has_place(ch->in_room, "awning"))
      return;
    }

    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && (can_get_wet(ch, iWear))) {
        obj->value[4] = UMIN(obj->value[4] + 1, 9);
      }
    }
  }

  void dry(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && (can_get_wet(ch, iWear))) {
        obj->value[4] = UMAX(obj->value[4] - 1, -10);
      }
    }
    if (!IS_NPC(ch) && ch->pcdata->wetness > 0) {
      ch->pcdata->wetness = UMAX(ch->pcdata->wetness - 10, -10);
      if (ch->pcdata->wetness <= 0)
      send_to_char("You are completely dry.\n\r", ch);
    }
  }

  void switch_vehicle(CHAR_DATA *ch, bool tohorse) {
    int location = 0;
    OBJ_DATA *obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0) {
      location = obj->value[4];
    }
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE) {
        location = ch->pcdata->garage_location[i];
      }
    }

    if (ch->your_car != NULL && ch->your_car->in_room != NULL) {
      if (tohorse == TRUE && ch->your_car->in_room->area->world > WORLD_EARTH && ch->your_car->in_room->area->world <= WORLD_HELL)
      park_car(ch, location);
      else if (tohorse == TRUE)
      park_car(ch, ch->your_car->in_room->vnum);
      if (tohorse == FALSE && ch->your_car->in_room->area->vnum == 13)
      park_car(ch, location);
      else if (tohorse == FALSE)
      park_car(ch, ch->your_car->in_room->vnum);
    }
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE) {
        ch->pcdata->garage_status[i] = GARAGE_GARAGED;
      }
    }
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_GARAGED) {
        if (tohorse == TRUE && ch->pcdata->garage_typeone[i] == CAR_HORSE)
        ch->pcdata->garage_status[i] = GARAGE_ACTIVE;
        if (tohorse == FALSE && ch->pcdata->garage_typeone[i] != CAR_HORSE)
        ch->pcdata->garage_status[i] = GARAGE_ACTIVE;
      }
    }
    if (tohorse == TRUE) {
      printf_to_char(ch, "You switch to riding your %s.\n\r", vehicle_name(ch));
      act("$n switches to riding $s horse.", ch, NULL, NULL, TO_ROOM);
    }
    else {
      printf_to_char(ch, "You switch to driving your %s.\n\r", vehicle_name(ch));
      act("$n switches to driving $s car.", ch, NULL, NULL, TO_ROOM);
    }
  }

  void carupdate(CHAR_DATA *ch) {
    EXIT_DATA *pexit;
    CHAR_DATA *to;

    if (ch->your_car == NULL || ch->your_car->in_room == NULL)
    return;

    if (IS_FLAG(ch->comm, COMM_SLOW)) {
      ch->your_car->ttl += 4;
    }

    if (IS_FLAG(ch->comm, COMM_STALL)) {
      ch->your_car->ttl++;
      return;
    }

    ROOM_INDEX_DATA *room = get_room_index(ch->car_inroom);
    ROOM_INDEX_DATA *desti = get_room_index(ch->pcdata->travel_to);

    if (room == NULL || desti == NULL)
    return;

    if (room == desti) {
      char_from_room(ch->your_car);
      char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
      ch->your_car->ttl = 2;
      ch->your_car->wounds = 4;
      return;
    }
    /*
if(!str_cmp(ch->name, "Malcolm"))
{
printf_to_char(ch, "Room: %d, Desti: %d, Vehicle type: %d, desti
world: %d, room area: %d, desti area: %d\n\r", room->vnum, desti->vnum, vehicle_typeone(ch), desti->area->world, room->area->vnum, desti->area->vnum);
}
*/

    if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
      if (room->area->vnum >= 19 && room->area->vnum <= 21) {
        switch_vehicle(ch, TRUE);
      }
    }
    if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {

      if (room->area->vnum >= 16 && room->area->vnum <= 18) {
        switch_vehicle(ch, FALSE);
      }
    }

    int dir = path_dir(room, desti, ch->facing, ch);

    if (dir < 0) {
      if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
        switch_vehicle(ch, TRUE);
      }
      if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {
        switch_vehicle(ch, FALSE);
      }
      return;
    }
    ch->facing = dir;
    pexit = room->exit[dir];

    ROOM_INDEX_DATA *newroom = pexit->u1.to_room;
    if (newroom == NULL) {
      if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
        switch_vehicle(ch, TRUE);
      }
      if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {
        switch_vehicle(ch, FALSE);
      }
      return;
    }
    if (newroom->area->world == WORLD_OTHER || newroom->area->world == WORLD_HELL || newroom->area->world == WORLD_WILDS || newroom->area->world == WORLD_GODREALM) {
      if (vehicle_typeone(ch) != CAR_HORSE) {
        force_park(ch->your_car);
        return;
      }
    }
    if (newroom->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE && mist_level(newroom) < 3) {
      force_park(ch->your_car);
      return;
    }
    char_from_room(ch->your_car);
    char_to_room(ch->your_car, newroom);
    ch->car_inroom = newroom->vnum;
    ch->your_car->recent_moved = 10;
    char buf[MSL];
    sprintf(buf, "$n drives %s.", dir_name[dir][0]);
    dact(buf, ch->your_car, NULL, ch, DISTANCE_MEDIUM);

    sprintf(buf, "$n drives into the area.\n\r");
    act(buf, ch->your_car, NULL, ch, TO_ROOM);

    int count = 0;
    CHAR_DATA *victim;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;
      victim->facing = dir;
    }

    for (CharList::iterator it = newroom->people->begin();
    it != newroom->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;
      if (victim->in_room != newroom)
      continue;
      if (victim == ch->your_car)
      continue;

      if (!can_see(ch, victim))
      continue;

      if (!IS_NPC(victim))
      printf_to_char(
      victim, "%s drives into the area from %s and then heads further on.\n\r", PERS(ch->your_car, victim), relincoming[get_reldirection(rev_dir[dir], victim->facing)]);

      if (count == 0) {
        for (CharList::iterator ij = ch->in_room->people->begin();
        ij != ch->in_room->people->end(); ++ij) {
          to = *ij;

          if (to == NULL || to->in_room == NULL)
          continue;
          if (!can_see(ch, to))
          continue;
          if (IS_FLAG(to->act, PLR_HIDE))
          continue;

          printf_to_char(to, "You pass %s", PERS(victim, to));
        }
        count++;
      }
      else {
        for (CharList::iterator ij = ch->in_room->people->begin();
        ij != ch->in_room->people->end(); ++ij) {
          to = *ij;

          if (to == NULL || to->in_room == NULL)
          continue;

          if (!can_see(ch, to))
          continue;

          if (IS_FLAG(to->act, PLR_HIDE))
          continue;

          printf_to_char(ch, ", %s", PERS(victim, ch));
        }
      }
    }
    if (count > 0) {
      for (CharList::iterator ij = ch->in_room->people->begin();
      ij != ch->in_room->people->end(); ++ij) {
        to = *ij;

        if (to == NULL || to->in_room == NULL)
        continue;
        if (!can_see(ch, to))
        continue;

        if (IS_FLAG(to->act, PLR_HIDE))
        continue;

        printf_to_char(ch, ".\n\r");
      }
    }
  }

  void bikeupdate(CHAR_DATA *ch) {
    EXIT_DATA *pexit;
    CHAR_DATA *to;
    if (ch->your_car == NULL || ch->your_car->in_room == NULL)
    return;

    ROOM_INDEX_DATA *room = get_room_index(ch->car_inroom);
    ROOM_INDEX_DATA *desti = get_room_index(ch->pcdata->travel_to);

    if (IS_FLAG(ch->comm, COMM_SLOW)) {
      ch->your_car->ttl += 4;
    }

    if (IS_FLAG(ch->comm, COMM_STALL)) {
      ch->your_car->ttl++;
      return;
    }

    if (room == NULL || desti == NULL)
    return;

    if (room == desti) {
      char_from_room(ch->your_car);
      char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
      ch->your_car->ttl = 2;
      ch->your_car->wounds = 4;
      return;
    }
    if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
      if (room->area->vnum >= 19 && room->area->vnum <= 21) {
        switch_vehicle(ch, TRUE);
      }
    }
    if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {
      if (room->area->vnum >= 16 && room->area->vnum <= 18) {
        switch_vehicle(ch, FALSE);
      }
    }

    char buf[MSL];
    int dir = path_dir(room, desti, ch->facing, ch);

    if (dir < 0) {
      if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
        switch_vehicle(ch, TRUE);
      }
      if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {
        switch_vehicle(ch, FALSE);
      }
      return;
    }
    ch->facing = dir;
    pexit = room->exit[dir];

    ROOM_INDEX_DATA *newroom = pexit->u1.to_room;

    if (newroom == NULL) {
      if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
        switch_vehicle(ch, TRUE);
      }
      if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {
        switch_vehicle(ch, FALSE);
      }
      return;
    }

    if (newroom->area->world == WORLD_OTHER || newroom->area->world == WORLD_HELL || newroom->area->world == WORLD_WILDS || newroom->area->world == WORLD_GODREALM) {
      if (vehicle_typeone(ch) != CAR_HORSE) {
        force_park(ch->your_car);
        return;
      }
    }
    if (newroom->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE && mist_level(newroom) < 3) {
      force_park(ch->your_car);
      return;
    }
    if (ch->your_car->in_room->area->world != newroom->area->world && ch->your_car->in_room->area->world != WORLD_EARTH && ch->your_car->in_room->area->world != WORLD_ELSEWHERE && !has_nonconsume(ch, ITEM_COMPASS)) {
      force_park(ch->your_car);
      return;
    }

    char_from_room(ch->your_car);
    char_to_room(ch->your_car, newroom);
    ch->car_inroom = newroom->vnum;
    sprintf(buf, "$N rides %s on the back of $n.", dir_name[dir][0]);
    dact(buf, ch->your_car, NULL, ch, DISTANCE_MEDIUM);

    sprintf(buf, "$N rides into the area on the back of $n.\n\r");
    act(buf, ch->your_car, NULL, ch, TO_ROOM);

    int count = 0;
    CHAR_DATA *victim;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;
      victim->facing = dir;
    }

    for (CharList::iterator it = newroom->people->begin();
    it != newroom->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != newroom)
      continue;

      if (victim == ch->your_car)
      continue;

      if (!can_see(ch, victim))
      continue;

      if (count == 0) {
        for (CharList::iterator ij = ch->in_room->people->begin();
        ij != ch->in_room->people->end(); ++ij) {
          to = *ij;

          if (to == NULL || to->in_room == NULL)
          continue;
          if (!can_see(ch, to))
          continue;
          if (IS_FLAG(to->act, PLR_HIDE))
          continue;

          printf_to_char(to, "You pass %s", PERS(victim, to));
        }
        count++;
      }
      else {
        for (CharList::iterator ij = ch->in_room->people->begin();
        ij != ch->in_room->people->end(); ++ij) {
          to = *ij;

          if (to == NULL || to->in_room == NULL)
          continue;
          if (!can_see(ch, to))
          continue;
          if (IS_FLAG(to->act, PLR_HIDE))
          continue;

          printf_to_char(ch, ", %s", PERS(victim, ch));
        }
      }
    }
    if (count > 0) {
      for (CharList::iterator ij = ch->in_room->people->begin();
      ij != ch->in_room->people->end(); ++ij) {
        to = *ij;

        if (to == NULL || to->in_room == NULL)
        continue;
        if (!can_see(ch, to))
        continue;
        if (IS_FLAG(to->act, PLR_HIDE))
        continue;

        printf_to_char(ch, ".\n\r");
      }
    }
  }

  void second_update() {
    CHAR_DATA *ch;
    char buf[MSL];

    operation_second_update();

    trolly_timer++;
    if (trolly_timer >= 8 && trolly_moving == 0) {
      trolly_message(2);
      trolly_timer = 0;
      trolly_moving = 1;
    }
    else if (trolly_timer >= 15 && trolly_moving == 1) {
      trolly_at++;
      if (trolly_at > 23)
      trolly_at = 0;

      trolly_moving = 0;
      trolly_timer = 0;
      trolly_message(1);
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        ch = d->character;
        if (IS_NPC(ch))
        continue;

        if (ch->in_room != NULL) {
          if (in_water(ch))
          wetify(ch);
        }

        if (ch->fisttimer > 0) {
          if (in_fight(ch)) {
            ch->fisttimer = 0;
          }
          else {
            ch->fisttimer--;
            if (ch->fisttimer == 0) {
              fistfight(ch);
            }
          }
        }

        if (ch->pcdata->vomit_timer > 0) {
          ch->pcdata->vomit_timer--;
          if (ch->pcdata->vomit_timer == 0)
          do_function(ch, &do_vomit, "");
        }

        if (ch->pcdata->ghost_wound > 0) {
          ch->pcdata->ghost_wound--;
          if (ch->pcdata->ghost_wound % 30 == 0) {
            send_to_char("You are wracked with pain.\n\r", ch);
            act("$n convulses.", ch, NULL, NULL, TO_ROOM);
          }
          if (ch->pcdata->ghost_wound == 0) {
            wound_char_absolute(ch, 3);
            act("You vomit blood and then pass out.", ch, NULL, NULL, TO_CHAR);
            act("$n vomits blood and then passes out.", ch, NULL, NULL, TO_ROOM);
          }
        }

        if (in_fight(ch) && ch->fight_fast == TRUE) {
          if (ch->attack_timer > 0) {
            ch->attack_timer--;
            if (ch->attack_timer == 0) {
              send_to_char("You can attack again.\n\r", ch);
              if (safe_strlen(ch->amove) > 2) {
                do_function(ch, &do_attack, ch->amove);
              }
            }
          }
          if (ch->move_timer > 0) {
            ch->move_timer--;
            if (ch->move_timer == 0) {
              ch->moved = 0;
              send_to_char("You can move again.\n\r", ch);

              if (safe_strlen(ch->qmove) > 1) {
                do_function(ch, &do_move, ch->qmove);
              }
            }
          }
          if (ch->round_timer <= 0) {
            round_process(ch);
            ch->round_timer = FIGHT_WAIT * fight_speed(ch);
          }
          else
          ch->round_timer--;

          if (ch->ability_timer > 0) {
            ch->ability_timer--;
            if (ch->ability_timer == 0) {
              send_to_char("You can use an ability again.\n\r", ch);
              if (safe_strlen(ch->abilmove) > 2) {
                do_function(ch, &do_ability, ch->abilmove);
              }
            }
          }
        }
        // Added this to make immortals immune to voting - Discordance
        if (!IS_IMMORTAL(ch)) {
          if (is_gm(ch) && ch->pcdata->vote_timer < 0) {
            ch->pcdata->vote_timer++;
            if (ch->pcdata->vote_timer == 0 && room_pop_mortals(ch->in_room) > 0 && !IS_FLAG(ch->comm, COMM_RUNNING)) {
              act("You apply to run the room.", ch, NULL, NULL, TO_CHAR);
              sprintf(buf, "Story Runner %s is applying to run the room, type vote yes if you agree, vote no if you do not.", ch->name);
              act(buf, ch, NULL, NULL, TO_ROOM);
              ch->pcdata->vote_timer = 30;
              ch->pcdata->yesvotes = 0;
              ch->pcdata->novotes = 0;
            }
          }

          if (is_gm(ch) && ch->pcdata->vote_timer > 0) {
            ch->pcdata->vote_timer--;
            if (ch->pcdata->vote_timer == 0) {
              if (ch->pcdata->novotes > ch->pcdata->yesvotes) {
                sprintf(buf, "The players have decided against %s running the room.", ch->name);
                act(buf, ch, NULL, NULL, TO_ROOM);
                act("The players have decided against you running the room.", ch, NULL, NULL, TO_CHAR);
                char_from_room(ch);
                char_to_room(ch, get_room_index(GMHOME));
                if (IS_FLAG(ch->comm, COMM_RUNNING))
                REMOVE_FLAG(ch->comm, COMM_RUNNING);
              }
              else {
                sprintf(buf, "Story Runner %s is now running the room, those who do not wish to be involved in this RP should leave now.", ch->name);
                act(buf, ch, NULL, NULL, TO_ROOM);
                act("You are now running the room.", ch, NULL, NULL, TO_CHAR);
                if (!IS_FLAG(ch->comm, COMM_RUNNING))
                SET_FLAG(ch->comm, COMM_RUNNING);
              }
            }
          }
        }

        if (ch->in_room != NULL && !in_fight(ch) && ch->pcdata->fall_timer == 0 && is_air(ch->in_room) && !is_flying(ch) && !is_ghost(ch) && !is_gm(ch) && (ch->master == NULL || ch->master->lifting == NULL || ch->master->lifting != ch || !is_flying(ch->master) || ch->master->in_room->vnum != ch->in_room->vnum)) {
          ch->pcdata->fall_timer = 1;
        }
        if (ch->in_room != NULL && ch->pcdata->fall_timer > 0 && (!in_fight(ch))) {
          ROOM_INDEX_DATA *to_room;
          EXIT_DATA *pexit;
          ch->pcdata->fall_timer--;
          if (ch->pcdata->fall_timer == 0) {
            if (is_air(ch->in_room) && !is_flying(ch)) {
              pexit = ch->in_room->exit[DIR_DOWN];
              if (pexit != NULL && !IS_SET(pexit->exit_info, EX_CLOSED) && pexit->wall == WALL_NONE) {
                to_room = pexit->u1.to_room;
                if (to_room != NULL) {
                  if (!is_ghost(ch))
                  act("$n falls out of the room.", ch, NULL, ch, TO_NOTVICT);
                  char_from_room(ch);
                  char_to_room(ch, to_room);
                  do_function(ch, &do_look, "auto");
                  if (ch->in_room->sector_type == SECT_WATER) {
                    if (!is_ghost(ch))
                    act("$n bellyflops into the water.", ch, NULL, ch, TO_NOTVICT);
                    send_to_char("You fall into the water.\n\r", ch);
                  }
                  else {
                    if (!is_ghost(ch))
                    act("$n falls into the room.", ch, NULL, ch, TO_NOTVICT);
                    send_to_char("You fall into the room!\n\r", ch);
                  }
                }
              }
            }
            else if (is_underwater(ch->in_room) && !is_ghost(ch)) {
              if (power_pop(ch->in_room) < 1 && get_gm(ch->in_room, FALSE) == NULL) {
                pexit = ch->in_room->exit[DIR_UP];
                if (pexit != NULL && !IS_SET(pexit->exit_info, EX_CLOSED) && pexit->wall == WALL_NONE) {
                  to_room = pexit->u1.to_room;
                  if (to_room != NULL) {
                    if (!is_ghost(ch))
                    act("$n swims up desperately.", ch, NULL, ch, TO_NOTVICT);
                    char_from_room(ch);
                    char_to_room(ch, to_room);
                    do_function(ch, &do_look, "auto");
                    if (!is_ghost(ch))
                    act("$n swims up into the area.", ch, NULL, ch, TO_NOTVICT);
                    send_to_char("You swim up quickly!\n\r", ch);

                    if (is_underwater(to_room))
                    ch->pcdata->fall_timer = 1;
                  }
                }
                else {
                  wound_char_absolute(ch, 2);
                  send_to_char("You start panicking for breath, desperate for a way out.\n\r", ch);
                  ch->pcdata->fall_timer += 5;
                }
              }
            }
          }
        }

        if (IS_FLAG(ch->comm, COMM_SLOW)) {
          ch->pcdata->travel_slow--;
        }
        if (ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1 && !in_fight(ch) && (!IS_FLAG(ch->comm, COMM_SLOW) || ch->pcdata->travel_slow <= 0)) {
          if (ch->pcdata->travel_type == TRAVEL_CAR || ch->pcdata->travel_type == TRAVEL_BIKE)
          accident_check(ch);
        }
        if (ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1 && !in_fight(ch) && (!IS_FLAG(ch->comm, COMM_SLOW) || ch->pcdata->travel_slow <= 0)) {
          if (IS_FLAG(ch->comm, COMM_SLOW)) {
            ch->pcdata->travel_slow = 4;
          }

          if (in_haven(get_room_index(ch->pcdata->travel_to)) && in_haven(get_room_index(ch->pcdata->travel_from))) {
            if (ch->pcdata->travel_type == TRAVEL_CAR) {
              carupdate(ch);
              if (!IS_FLAG(ch->comm, COMM_STALL) && fast_car(ch) && ch->pcdata->travel_time > 3)
              carupdate(ch);
            }
            if (ch->pcdata->travel_type == TRAVEL_BIKE || ch->pcdata->travel_type == TRAVEL_HORSE) {
              bikeupdate(ch);
              if (!IS_FLAG(ch->comm, COMM_STALL) && fast_car(ch) && ch->pcdata->travel_time > 3)
              bikeupdate(ch);
            }
          }

          if (ch->pcdata->travel_time <= 0) {
            if (ch->your_car != NULL && ch->your_car->your_car != NULL && !IS_NPC(ch->your_car->your_car) && ch->your_car->your_car->pcdata->driving_around == TRUE) {
              if (ch->pcdata->travel_type == TRAVEL_HORSE || ch->pcdata->travel_type == TRAVEL_HPASSENGER || ch->pcdata->travel_type == TRAVEL_BIKE || ch->pcdata->travel_type == TRAVEL_BPASSENGER || ch->pcdata->travel_type == TRAVEL_CAR || ch->pcdata->travel_type == TRAVEL_CPASSENGER) {
                ROOM_INDEX_DATA *newroom = NULL;
                ROOM_INDEX_DATA *olddesti = get_room_index(ch->pcdata->travel_to);
                for (int i = 0; i < MAX_TAXIS; i++) {
                  if (number_percent() % 2 == 0 && taxi_table[i].y > 45 && (taxi_table[i].x == olddesti->x || taxi_table[i].y == olddesti->y) && taxi_table[i].vnum != olddesti->vnum)
                  newroom = get_room_index(taxi_table[i].vnum);
                }
                if (newroom == NULL) {
                  for (int i = 0; i < MAX_TAXIS; i++) {
                    if (taxi_table[i].y > 45 && (taxi_table[i].x == olddesti->x || taxi_table[i].y == olddesti->y) && taxi_table[i].vnum != olddesti->vnum)
                    newroom = get_room_index(taxi_table[i].vnum);
                  }
                }
                if (newroom != NULL) {
                  int dist = street_distance(olddesti, newroom, ch);
                  ch->your_car->ttl = UMAX(ch->your_car->ttl, dist + 1);
                  for (CharList::iterator it = ch->in_room->people->begin();
                  it != ch->in_room->people->end();) {
                    CHAR_DATA *fch = *it;
                    ++it;
                    if (fch == NULL || IS_NPC(fch))
                    continue;
                    fch->pcdata->travel_from = ch->your_car->in_room->vnum;
                    fch->pcdata->travel_to = newroom->vnum;
                    fch->pcdata->travel_time = dist;
                    fch->pcdata->last_drove = ch->pcdata->travel_from;
                  }
                }
              }
            }
          }

          if (ch->pcdata->travel_time <= 0) {
            if (!IS_NPC(ch) && public_room(get_room_index(ch->pcdata->travel_to)) && !IS_NPC(ch) && !is_ghost(ch) && in_haven(get_room_index(ch->pcdata->travel_to)))
            last_public(ch, ch->pcdata->travel_to);
            if (ch->pcdata->travel_type == TRAVEL_CLIMB) {
              if (!is_ghost(ch))
              act("$n climbs out of the room.", ch, NULL, ch, TO_NOTVICT);
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              if (!is_ghost(ch))
              act("$n climbs into the room.", ch, NULL, ch, TO_NOTVICT);
            }
            if (ch->pcdata->travel_type == TRAVEL_FALL) {
              if (!is_ghost(ch))
              act("$n falls out of the room.", ch, NULL, ch, TO_NOTVICT);
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              fall_character(ch);
              do_function(ch, &do_look, "auto");
              if (ch->in_room->sector_type == SECT_WATER || ch->in_room->sector_type == SECT_SHALLOW || ch->in_room->sector_type == SECT_UNDERWATER) {
                if (!is_ghost(ch))
                act("$n bellyflops into the water.", ch, NULL, ch, TO_NOTVICT);
                send_to_char("You fall into the water.\n\r", ch);
              }
              else {
                if (!is_ghost(ch))
                act("$n falls into the room.", ch, NULL, ch, TO_NOTVICT);
                send_to_char("You hit the ground!\n\r", ch);
              }
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
            }
            if (ch->pcdata->travel_type == TRAVEL_JUMP) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              fall_character(ch);
              do_function(ch, &do_look, "auto");
              if (ch->in_room->sector_type == SECT_WATER || ch->in_room->sector_type == SECT_SHALLOW || ch->in_room->sector_type == SECT_UNDERWATER) {
                if (!is_ghost(ch))
                act("$n dives into the water.", ch, NULL, ch, TO_NOTVICT);
                send_to_char("You dive into the water.\n\r", ch);
              }
              else {
                if (!is_ghost(ch))
                act("$n jumps into the room.", ch, NULL, ch, TO_NOTVICT);
                send_to_char("You land gracefully.\n\r", ch);
              }
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
            }
            if (ch->pcdata->travel_type == TRAVEL_TAXI) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n steps out of a taxi.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You step out of the taxi.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_CPASSENGER) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n steps out of a car.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You step out of the car.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_BUS) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n steps off a bus.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You get off a bus.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_PLANE) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n steps out of a a taxi.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You step out a taxi from the airport.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_PATHING) {
              ROOM_INDEX_DATA *to_room = get_room_index(ch->pcdata->travel_to);
              if (in_haven(to_room))
              to_room = room_by_coordinates(number_range(-10, 80), number_range(-10, 80), 0);
              if (to_room == NULL)
              to_room = get_room_index(ch->pcdata->travel_to);
              char_from_room(ch);
              char_to_room(ch, to_room);
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n steps out of thin air.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You step out of thin air.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_CAR) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch)) {
                if (safe_strlen(vehicle_name(ch)) > 2) {
                  sprintf(buf, "$n steps out of $s %s.", vehicle_name(ch));
                  act(buf, ch, NULL, NULL, TO_ROOM);
                }
                else
                act("$n steps out of $s car.", ch, NULL, NULL, TO_ROOM);
              }
              send_to_char("You step out of the car.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              if (ch->your_car != NULL) {
                char_from_room(ch->your_car);
                char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
                ch->your_car->ttl = 2;
                ch->your_car->wounds = 4;
              }
              if (has_active_vehicle(ch))
              park_car(ch, ch->in_room->vnum);
            }
            if (ch->pcdata->travel_type == TRAVEL_BPASSENGER) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n gets off a motorcycle.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You get off the motorcycle.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_BIKE) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch)) {
                if (safe_strlen(vehicle_name(ch)) > 2) {
                  sprintf(buf, "$n gets off $s %s.", vehicle_name(ch));
                  act(buf, ch, NULL, NULL, TO_ROOM);
                }
                else
                act("$n gets off $s motorcycle.", ch, NULL, NULL, TO_ROOM);
              }
              send_to_char("You get off your motorcycle.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              if (ch->your_car != NULL) {
                char_from_room(ch->your_car);
                char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
                ch->your_car->ttl = 2;
                ch->your_car->wounds = 4;
              }
              if (has_active_vehicle(ch))
              park_car(ch, ch->in_room->vnum);
            }
            if (ch->pcdata->travel_type == TRAVEL_HPASSENGER) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch))
              act("$n gets off a horse.", ch, NULL, NULL, TO_ROOM);
              send_to_char("You get off the horse.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              ch->facing = DIR_NORTH;
            }
            if (ch->pcdata->travel_type == TRAVEL_HORSE) {
              char_from_room(ch);
              char_to_room(ch, get_room_index(ch->pcdata->travel_to));
              do_function(ch, &do_look, "auto");
              if (!is_ghost(ch)) {
                if (safe_strlen(vehicle_name(ch)) > 2) {
                  sprintf(buf, "$n gets off $s %s.", vehicle_name(ch));
                  act(buf, ch, NULL, NULL, TO_ROOM);
                }
                else
                act("$n gets off $s horse.", ch, NULL, NULL, TO_ROOM);
              }
              send_to_char("You get off your horse.\n\r", ch);
              ch->pcdata->travel_time = -1;
              ch->pcdata->travel_to = -1;
              ch->pcdata->travel_type = -1;
              ch->pcdata->move_damage = 0;
              if (ch->your_car != NULL) {
                char_from_room(ch->your_car);
                char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
                ch->your_car->ttl = 2;
                ch->your_car->wounds = 4;
              }
              if (has_active_vehicle(ch))
              park_car(ch, ch->in_room->vnum);
            }

          }
          else {
            if (!IS_FLAG(ch->comm, COMM_STALL))
            ch->pcdata->travel_time--;
            if (!IS_FLAG(ch->comm, COMM_STALL) && fast_car(ch) && ch->pcdata->travel_time > 2)
            ch->pcdata->travel_time--;

            if((ch->pcdata->travel_time > 120 && ch->pcdata->travel_time % 55 == 0) || (ch->pcdata->travel_time > 3 && ch->pcdata->travel_time < 120 && ch->pcdata->travel_time % 10 == 0) || (ch->pcdata->travel_time < 3))
            {
              if (ch->pcdata->travel_type == TRAVEL_FALL)
              send_to_char("You fall through the air.\n\r", ch);
              else if (ch->pcdata->travel_type == TRAVEL_CLIMB && ch->pcdata->travel_time % 2 == 1)
              send_to_char("You continue to climb.\n\r", ch);
              else if (ch->pcdata->travel_type == TRAVEL_JUMP && ch->pcdata->travel_time <= 0)
              send_to_char("You fly through the air.\n\r", ch);
              else if (ch->pcdata->travel_type == TRAVEL_TAXI || ch->pcdata->travel_type == TRAVEL_CAR || ch->pcdata->travel_type == TRAVEL_CPASSENGER || ch->pcdata->travel_type == TRAVEL_CAR || ch->pcdata->travel_type == TRAVEL_CPASSENGER) {
                if (ch->pcdata->travel_time % 5 == 0)
                send_to_char("You drive along the street.\n\r", ch);
              }
              else if (ch->pcdata->travel_type == TRAVEL_BUS)
              send_to_char("The bus continues to rumble along the street.\n\r", ch);
              else if (ch->pcdata->travel_type == TRAVEL_PLANE)
              send_to_char("The plane cruises through the sky.\n\r", ch);
              else if (ch->pcdata->travel_type == TRAVEL_PATHING)
              send_to_char("You continue to walk along the path through the gloomy forest.\n\r", ch);
            }
          }
        }
      }
    }
  }

  _DOFUN(do_idleabuse) {
    char arg[MIL];
    char buf[MSL];
    int number1 = 0, number2 = 0;

    if (ch->pcdata->idling == 0)
    return;

    argument = one_argument(argument, arg);

    if (!is_number(argument) || !is_number(arg)) {
      send_to_char("That is not the correct sequence.  Please enter the above syntax.\n\r", ch);
      return;
    }

    number1 = atoi(arg);
    number2 = atoi(argument);

    if (number1 != ch->pcdata->idling_values[0] || number2 != ch->pcdata->idling_values[1]) {
      send_to_char("That is not the correct sequence of numbers.  Please make\n\rcertain that the first number you enter is the one that is marked\n\ras the `Rhigher`x numbered question.  If you feel you were correct\n\rplease note Palin and you will be reimbursed for your troubles.\n\r", ch);
      return;
    }

    send_to_char("You have successfully avoided being kicked off.  However, a record of\n\ryour near miss has been send to the Implementors.", ch);

    sprintf(buf, "%s has nearly been idle abused.\n\r", ch->name);
    log_string(buf);

    ch->pcdata->idling = 0;
    ch->pcdata->tertiary_timer = 0;
    ch->pcdata->idling_values[0] = 0;
    ch->pcdata->idling_values[1] = 0;
    return;
  }

  void check_birthday() {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *ch = CH(*it);
      if (ch == NULL)
      continue;

      if (ch->pcdata->birth_year == 1) {
        ch->pcdata->birth_day = time_info.day;
        ch->pcdata->birth_month = time_info.month;
        ch->pcdata->birth_year = (time_info.year - 23);
      }
    }
  }

  _DOFUN(do_shower) {
    AFFECT_DATA af;
    if (!IS_SET(ch->in_room->room_flags, ROOM_SHOWER)) {
      send_to_char("There is no bath here.\n\r", ch);
      return;
    }

    if (IS_FLAG(ch->act, PLR_SHOWERING)) {
      send_to_char("You step out of the bath.\n\r", ch);
      REMOVE_FLAG(ch->act, PLR_SHOWERING);
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      if (ch->sex == SEX_MALE)
      af.duration = 30;
      else
      af.duration = 100;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_WET;
      affect_to_char(ch, &af);
      return;
    }
    else {
      send_to_char("You step into the bath.\n\r", ch);
      SET_FLAG(ch->act, PLR_SHOWERING);
      return;
    }
  }

  char *fread_line(FILE *fp) {
    static char line[MSL];
    char *pline;
    char c;
    int ln;

    pline = line;
    line[0] = '\0';
    ln = 0;

    //     * Skip blanks.
    //     * Read first char.
    do {
      if (feof(fp)) {
        strcpy(line, "");
        return line;
      }
      c = getc(fp);
    } while (isspace(c));

    ungetc(c, fp);
    do {
      if (feof(fp)) {
        *pline = '\0';
        return line;
      }
      c = getc(fp);
      *pline++ = c;
      ln++;
      if (ln >= (MSL - 1)) {
        break;
      }
    } while (c != '\n' && c != '\r');

    do {
      c = getc(fp);
    } while (c == '\n' || c == '\r');

    ungetc(c, fp);
    *pline = '\0';
    return line;
  }

  bool in_hospital(CHAR_DATA *ch) {
    CHAR_DATA *to;
    if (ch->in_room == NULL)
    return FALSE;

    if (in_medical_facility(ch)) {
      return TRUE;
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (get_skill(to, SKILL_MEDICINE) >= 3)
        return TRUE;
      }
    }
    return FALSE;
  }

  void legacy_update(CHAR_DATA *ch) {

    if (is_werewolf(ch)) {
      if (!IS_FLAG(ch->legacy, LEGACY_WEREWOLF))
      SET_FLAG(ch->legacy, LEGACY_WEREWOLF);
    }
  }

  void send_message_to_char(CHAR_DATA *ch) {}

  void reseal_demon_hole(ROOM_INDEX_DATA *room) {
    EXIT_DATA *pexit;

    for (int dir = 0; dir < 10; dir++) {
      if ((pexit = room->exit[dir]) != NULL) {
        if (IS_SET(pexit->exit_info, EX_HELLGATE)) {
          pexit->wallcondition = 0;
          for (CharList::iterator it = room->people->begin();
          it != room->people->end(); ++it)
          act("The hole in the $d barrier of branches and thorns rapidly repairs itself.", *it, NULL, pexit->keyword, TO_CHAR);

          if (pexit->u1.to_room != NULL && pexit->u1.to_room->exit[rev_dir[dir]] != NULL) {
            pexit->u1.to_room->exit[rev_dir[dir]]->wallcondition = 0;
            for (CharList::iterator it = pexit->u1.to_room->people->begin();
            it != pexit->u1.to_room->people->end(); ++it)
            act("The hole in the $d barrier of branches and thorns rapidly repairs itself.", *it, NULL, pexit->u1.to_room->exit[rev_dir[dir]]->keyword, TO_CHAR);
          }
        }
      }
    }
  }

  void initiate_demon_holes(void) {
    for (int i = 0; i < 50; i++) {
      demon_hole_room[i] = 0;
      demon_hole_timer[i] = 0;
    }
  }

  void demon_hole_update(void) {
    for (int i = 0; i < 50; i++) {
      if (demon_hole_timer[i] > 0 && !room_fight(get_room_index(demon_hole_room[i]), FALSE, FALSE, TRUE)) {
        demon_hole_timer[i]--;
        if (demon_hole_timer[i] == 0) {
          reseal_demon_hole(get_room_index(demon_hole_room[i]));
          demon_hole_room[i] = 0;
        }
        else if (number_percent() % 5 == 0)
        demon_spawn(get_room_index(demon_hole_room[i]));
      }
    }
  }

  void demon_hole(ROOM_INDEX_DATA *room) {
    for (int i = 0; i < 50; i++) {
      if (demon_hole_room[i] == 0 || demon_hole_timer[i] == 0) {
        demon_hole_room[i] = room->vnum;
        demon_hole_timer[i] = 5;
        if (number_percent() % 2 == 0)
        demon_spawn(room);
        return;
      }
    }
  }

  _DOFUN(do_testercommand) {
    return;
    ch->pcdata->account->xp += 50000;
    ch->pcdata->account->rpxp += 50000;
    ch->pcdata->account->karma += 25000;
    ch->pcdata->account->pkarma += 10000;
    ch->pcdata->account->hightiercooldown = current_time;
    ch->pcdata->account->factioncooldown = current_time;
    ch->pcdata->account->newcharcount = 4;
    ch->pcdata->form_change_date = 0;
    ch->pcdata->next_henchman = current_time;
    ch->pcdata->next_enemy = current_time;
    if (ch->faction > 0)
    give_respect(ch, 5000, "Test command", ch->faction);
    ch->played += (3600 * 50);
    ch->money += 500000;
    ch->lifeforce = 10000;
    ch->pcdata->fatigue = 0;
    send_to_char("Done.\n\r", ch);
    ch->pcdata->dexp += 50000;
    ch->pcdata->aexp += 50000;
    ch->pcdata->wexp += 50000;
  }

  _DOFUN(do_lag) {
    char buf[MSL];
    sprintf(buf, "LAG - %s", ch->name);
    log_string(buf);
    send_to_char("Done.\n\r", ch);
  }

  void calendar_broadcast(STORY_TYPE *cal, char *argument) {
    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      CHAR_DATA *ch = *it;
      ++it;

      if (ch == NULL)
      continue;

      if (IS_NPC(ch))
      continue;

      if(!can_see_calendar(ch, cal))
      continue;

      if (!ch->desc || ch->desc->connected == CON_CREATION)
      continue;

      send_to_char(argument, ch);
    }
  }

  bool dreamer(CHAR_DATA *ch) {
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      for (int i = 0; i < 200; i++) {
        if (!str_cmp((*it)->participants[i], ch->name))
        return TRUE;
      }
    }
    return FALSE;
  }
  void timeonline(int hour) {
    int count = 0;
    int faccount = 0;
    int dcount = 0;
    int acount = 0;
    CHAR_DATA *to;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;

        if (is_gm(to))
        continue;
        count++;
        if (to->faction != 0)
        faccount++;
        if (dreamer(to))
        dcount++;
        if (to->pcdata->aexp > 0)
        acount++;
      }
    }
    char buf[MSL];
    sprintf(buf, "ONLINE: %d: Total: %d, Dreamer: %d, Society: %d, Adventure: %d", hour, count, dcount, faccount, acount);
    log_string(buf);
  }

  bool monster_fightable(CHAR_DATA *ch) {
    int pop = 0;
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
      if (victim->faction == 0)
      continue;
    }
    if (pop >= 5)
    return TRUE;
    return FALSE;
  }

  void hourly_update(int hour) {
    //    sprintf(buf, "Hourly update %d at %s.", hour, (char *) ctime(
    //    &current_time )); wiznet(buf,NULL,NULL,WIZ_LOGINS,0,0);

    timeonline(hour);
    faction_antagonist_update();
    if (hour == 6) {
      copyover_coming_up = 1;
      makeworldmap();
     // forum_update(); disabled for general release
    }
    if (hour == 8) {
      territory_update();
    }
    if (hour == 9) {
      faction_daily_update();
      if (get_day() == 1) {
        LOCATION_TYPE *loc;
        loc = get_loc("District 82");
        if (loc->base_faction_core != 0 && clan_lookup(loc->base_faction_core) != NULL && clan_lookup(loc->base_faction_core)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_core)->leader, 87);
        if (loc->base_faction_sect != 0 && clan_lookup(loc->base_faction_sect) != NULL && clan_lookup(loc->base_faction_sect)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_sect)->leader, 87);
        if (loc->base_faction_cult != 0 && clan_lookup(loc->base_faction_cult) != NULL && clan_lookup(loc->base_faction_cult)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_cult)->leader, 87);

        loc = get_loc("Lauriea");
        if (loc->base_faction_core != 0 && clan_lookup(loc->base_faction_core) != NULL && clan_lookup(loc->base_faction_core)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_core)->leader, 85);
        if (loc->base_faction_sect != 0 && clan_lookup(loc->base_faction_sect) != NULL && clan_lookup(loc->base_faction_sect)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_sect)->leader, 85);
        if (loc->base_faction_cult != 0 && clan_lookup(loc->base_faction_cult) != NULL && clan_lookup(loc->base_faction_cult)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_cult)->leader, 85);

        loc = get_loc("Rhagost");
        if (loc->base_faction_core != 0 && clan_lookup(loc->base_faction_core) != NULL && clan_lookup(loc->base_faction_core)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_core)->leader, 86);
        if (loc->base_faction_sect != 0 && clan_lookup(loc->base_faction_sect) != NULL && clan_lookup(loc->base_faction_sect)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_sect)->leader, 86);
        if (loc->base_faction_cult != 0 && clan_lookup(loc->base_faction_cult) != NULL && clan_lookup(loc->base_faction_cult)->stasis == 0)
        offline_potion(clan_lookup(loc->base_faction_cult)->leader, 86);
      }
    }
    if (hour == 16 && get_weekday() == 5)
    weekly_process();

    if (hour == 11) {
      rent_update();
    }
    if (hour == 9) {
      house_auction_update();
      int curbal = currentalliancebalance(FACTION_CULT);
      int minbal = minalliancebalance(FACTION_CULT);
      char buf[MSL];
      sprintf(buf, "ALLIANCEBALANCECULT %d, min %d", curbal, minbal);
      log_string(buf);
      if (get_day() == 1 && curbal > minbal * 2 && curbal > 4000)
      new_alliance(FACTION_CULT);

      curbal = currentalliancebalance(FACTION_SECT);
      minbal = minalliancebalance(FACTION_SECT);
      sprintf(buf, "ALLIANCEBALANCESECT %d, min %d", curbal, minbal);
      log_string(buf);
      if (get_day() == 1 && curbal > minbal * 2 && curbal > 4000)
      new_alliance(FACTION_SECT);
    }
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->hour > 23)
      (*it)->hour -= 23;

      if ((*it)->hour == hour && (*it)->hour > 0) {
        if ((*it)->day > 0)
        (*it)->day--;
        else
        launch_operation(*it);
      }
      else if ((*it)->hour == (hour + 1) % 24 && (*it)->day == 0 && event_cleanse == 0) {
        send_message_temp(
        (*it)->faction, "Your comms announce the team for your operation will be leaving in an hour. Use operation signup (number) to take part");
        for (vector<FACTION_TYPE *>::iterator ik = FacVect.begin();
        ik != FacVect.end(); ++ik) {
          if ((*ik)->vnum != (*it)->faction && (*it)->competition != COMPETE_CLOSED) {
            send_message_temp(
            (*ik)->vnum, "Your comms announce that an operation will be occuring in one hour. Use operation signup (number) to take part");
          }
        }
      }
    }

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      CHAR_DATA *ch = *it;
      ++it;

      if (ch == NULL)
      continue;

      if (IS_NPC(ch))
      continue;

      if (!ch->desc || ch->desc->connected == CON_CREATION)
      continue;

      if (!guestmonster(ch) || monster_fightable(ch)) {
        ch->played = ch->played + (int)(current_time - ch->logon);
      }
      ch->logon = current_time;

      if (ch->in_room != NULL && can_hear_bell(ch->in_room))
      sound_bell(ch, hour);

      if (hour == 6 || hour == 16)
      send_to_char("The game will automatically reboot in one hour.\n\r", ch);

      char lbuf[MSL];

      if (ch->pcdata->lifeforcecooldown > current_time + (3600 * 24)) {
        ch->pcdata->lifeforcecooldown = current_time;
        sprintf(lbuf, "LIFEFORCE COOLDOWN RESET: %s", ch->name);
        log_string(lbuf);
      }

      if (ch->pcdata->lifeforcecooldown < current_time) {
        sprintf(lbuf, "REGEN LIFEFORCE: %s", ch->name);
        log_string(lbuf);

        regenerate_lifeforce(ch);
        if (ch->pcdata->account != NULL)
        ch->pcdata->account->daily_pvp_exp = 0;
      }
    }

    if (hour == 7 || hour == 17) // added extra copyover 0-13-2019 Disco
    {
      auto_copyover();
    }
  }

  int immortal_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
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
      if (victim->in_room != room)
      continue;

      if (IS_IMMORTAL(victim))
      pop++;
    }
    return pop;
  }

  void bloodstorm_update(CHAR_DATA *ch) {
    if (time_info.bloodstorm == 0)
    return;

    if (ch->in_room == NULL)
    return;
    if (!in_haven(ch->in_room))
    return;
    if (ch->in_room->vnum < 300)
    ch->pcdata->bloodstorm_resist = 5;
    bool sprotected = FALSE;
    if (in_lodge(ch->in_room)) {
      ch->pcdata->bloodstorm_resist = UMIN(5, ch->pcdata->bloodstorm_resist + 1);
      sprotected = TRUE;
    }
    if (institute_room(ch->in_room) && ch->in_room->z < 0) {
      ch->pcdata->bloodstorm_resist = UMIN(5, ch->pcdata->bloodstorm_resist + 1);
      sprotected = TRUE;
    }
    int immpop = immortal_pop(ch->in_room);
    if (immpop < 1 && sprotected == FALSE) {
      ch->pcdata->bloodstorm_resist--;
      if (ch->pcdata->bloodstorm_resist <= 0) {
        if (institute_room(ch->in_room)) {
          char_from_room(ch);
          char_to_room(ch, get_room_index(4820));
          send_to_char("`RYou are driven underground by the storm!\n\r", ch);
          act("$n arrives looking battered by the storm.", ch, NULL, NULL, TO_ROOM);
          return;
        }
        else {
          char_from_room(ch);
          char_to_room(ch, get_room_index(15050));
          send_to_char("`RYou are driven into shelter by the storm!\n\r", ch);
          act("$n arrives looking battered by the storm.", ch, NULL, NULL, TO_ROOM);
          return;
        }
      }
    }
    if (sprotected == FALSE) {
      if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        send_to_char("`rYou are lashed by howling winds and blood-tinged rain.\n\r", ch);
      }
      else {
        send_to_char("`rYou are lashed by howling winds and blood-tinged rain that seem to pass straight through the walls!\n\r", ch);
      }
    }
  }

  void minute_update(int minute) {
    char buf[MSL];
    //    sprintf(buf, "Minute update %d at %s.", minute, (char *) ctime(
    //    &current_time )); wiznet(buf,NULL,NULL,WIZ_LOGINS,0,0);

    fight_problem = 0;
    for (vector<STORY_TYPE *>::iterator ij = StoryVect.begin();
    ij != StoryVect.end(); ++ij) {
      if ((*ij)->valid == FALSE)
      continue;


      if ((*ij)->time <= (current_time + (60 * 30)) && (*ij)->time > (current_time - (3600 * 2)) && (*ij)->advertised == 0 && safe_strlen((*ij)->type) > 2) {
        if (strcasestr((*ij)->location, "dreamworld") != NULL)
        sprintf(buf, "Dreamworld's %s is starting in thirty minutes.\n\r", (*ij)->type);
        else
        sprintf(buf, "%s's %s is starting in thirty minutes.\n\r", (*ij)->author, (*ij)->type);
        calendar_broadcast((*ij), buf);
        (*ij)->advertised = 1;
      }
      else if ((*ij)->time <= current_time && (*ij)->advertised == 1) {
        if (strcasestr((*ij)->location, "dreamworld") != NULL)
        sprintf(buf, "Dreamworld's %s is starting now.\n\r", (*ij)->type);
        else
        sprintf(buf, "%s's %s is starting now.\n\r", (*ij)->author, (*ij)->type);
        calendar_broadcast((*ij), buf);
        (*ij)->advertised = 2;
        if (get_char_world_pc((*ij)->author) != NULL && !is_gm(get_char_world_pc((*ij)->author)))
        get_char_world_pc((*ij)->author)->pcdata->influence += 2500;
      }
      else if ((*ij)->time <= (current_time - 3600) && (*ij)->advertised == 2) {
        (*ij)->advertised = 3;
        CHAR_DATA *author = get_char_world_pc((*ij)->author);
        if (author != NULL && !is_gm(author) && (*ij)->ctype != 0 && (*ij)->ctype != CALENDAR_PLOT && (*ij)->ctype != CALENDAR_EDUCATIONAL && strcasestr((*ij)->location, "dreamworld") == NULL) {
          give_influence(author, pc_pop(author->in_room) * 300);
          social_behave_mod(author, pc_pop(author->in_room) * 2, "hosting an event.");
          if(author->faction != 0)
          {
            give_intel(author, 100*(int)sqrt(pc_pop(author->in_room)));
          }
          author->pcdata->attract_mod++;
          author->pcdata->week_tracker[TRACK_EVENTS_HELD]++;
          author->pcdata->life_tracker[TRACK_EVENTS_HELD]++;
          if (pc_pop(author->in_room) >
              author->pcdata->week_tracker[TRACK_EVENT_MAXPOP])
          author->pcdata->week_tracker[TRACK_EVENT_MAXPOP] =
          pc_pop(author->in_room);
          if (pc_pop(author->in_room) >
              author->pcdata->life_tracker[TRACK_EVENT_MAXPOP])
          author->pcdata->life_tracker[TRACK_EVENT_MAXPOP] =
          pc_pop(author->in_room);
          for (CharList::iterator it = author->in_room->people->begin();
          it != author->in_room->people->end();) {
            CHAR_DATA *fch = *it;
            ++it;
            if (fch == NULL || IS_NPC(fch))
            continue;

            if((*ij)->ctype != CALENDAR_PLOT && (*ij)->isprivate == 0)
            {
              MATCH_TYPE *match = match_find(author, fch);
              if(match != NULL)
              {
                log_string("PARTYMATCH Setting");
                if(!str_cmp(match->nameone, author->name))
                {
                  match->rate_party_type_one = (*ij)->ctype;
                  match->rate_party_one = 1;
                  free_string(match->party_one_title);
                  match->party_one_title = str_dup(from_color((*ij)->type));
                }
                else if(!str_cmp(match->nametwo, author->name))
                {
                  match->rate_party_type_two = (*ij)->ctype;
                  match->rate_party_two = 1;
                  free_string(match->party_two_title);
                  match->party_two_title = str_dup(from_color((*ij)->type));
                }
              }
            }
            fch->pcdata->week_tracker[TRACK_EVENTS_ATTENDED]++;
            fch->pcdata->life_tracker[TRACK_EVENTS_ATTENDED]++;
            if (get_attract(fch, NULL) >
                fch->pcdata->week_tracker[TRACK_EVENT_HOTNESS] && profile_rating(fch) > 25)
            fch->pcdata->week_tracker[TRACK_EVENT_HOTNESS] =
            get_attract(fch, NULL);
            if (get_attract(fch, NULL) >
                fch->pcdata->life_tracker[TRACK_EVENT_HOTNESS] && profile_rating(fch) > 25)
            fch->pcdata->life_tracker[TRACK_EVENT_HOTNESS] =
            get_attract(fch, NULL);
            if (fch->pcdata->total_money > fch->pcdata->week_tracker[TRACK_BANK])
            fch->pcdata->week_tracker[TRACK_BANK] = fch->pcdata->total_money;
            if (fch->pcdata->total_money > fch->pcdata->life_tracker[TRACK_BANK])
            fch->pcdata->life_tracker[TRACK_BANK] = fch->pcdata->total_money;
          }
        }
      }
    }
    CHAR_DATA *to;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to))
        continue;

        if (to->pcdata->destiny_feature > 0 && to->pcdata->destiny_feature_room != to->in_room->vnum)
        to->pcdata->destiny_feature = 0;


        if(to->pcdata->page_timeout > 0)
        to->pcdata->page_timeout--;

        if (time_info.minute % 14 == 0)
        destiny_char_update(to);




        if(time_info.minute % 3 == 0 && to->pcdata->ritual_prey_timer > 0)
        {
          CHAR_DATA *pred = get_char_world_pc(to->pcdata->ritual_prey_target);
          if(pred != NULL && pred->in_room != to->in_room && valid_pc_prey(to, pred, FALSE))
          printf_to_char(pred, "You sense %s's vulnerability.\n\r", PERS(to, pred));
        }
        if(to->pcdata->crowded_room == FALSE)
        {
          if(crowded_room(to->in_room))
          {
            to->pcdata->crowded_room = TRUE;
            send_to_char("`cThe area you are in is now crowded.`x\n\r", to);
          }
        }
        else
        {
          if(crowded_room(to->in_room) == FALSE)
          {
            to->pcdata->crowded_room = FALSE;
            send_to_char("`210The area you are in is no longer crowded.`x\n\r", to);
          }
        }

        if (!IS_FLAG(to->act, PLR_DEAD)) {
          if (to->pcdata->doom_date <= 1 && time_info.minute % 13 == 0) {
            set_doom(to);
          }
          if (to->pcdata->doom_date > 1 && to->pcdata->doom_date < current_time && to->pcdata->doom_date > current_time - (3600 * 24 * 28)) {
            if(to->pcdata->doom_countdown == 0)
            to->pcdata->doom_countdown = 60;
            if(to->pcdata->doom_countdown > 0)
            {
              to->pcdata->doom_countdown--;
              if(to->pcdata->doom_countdown == 1)
              {
                send_to_char("Your doom catches up to you.\n\r", to);
                real_kill(to, to);
                if (!IS_NPC(to)) {
                  free_string(to->pcdata->deathcause);
                  to->pcdata->deathcause = str_dup("Unclear");
                }
              }
              else
              {
                send_to_char("You feel your doom approaching.\n\r(You need to log off or reroll this character immediately to prevent their death.\n\r", to);
                return;
              }
            }
          }
          else if (to->pcdata->doom_date > 1 && to->pcdata->doom_date < current_time) {
            free_string(to->pcdata->doom_desc);
            to->pcdata->doom_desc = str_dup("");
            set_doom(to);
          }
        }


        if (time_info.minute % 3 == 0) {
          if (in_fight(to) && to->fight_fast == FALSE) {
            if (to->fight_current == NULL || to->fight_current->in_room == NULL || !same_fight(to, to->fight_current))
            next_attacker(to, FALSE);
            else {
              if (to->fight_current->actiontimer <= 0)
              next_attacker(to, FALSE);
            }
          }
        }
        if (higher_power(to)) {
          if (to->in_room != NULL && to->in_room->vnum > 300 && to->in_room->vnum != GMHOME && !mist_room(to->in_room) && to->in_room->area->vnum != 12 && !is_in_domain(to) && to->pcdata->patrol_status < 1 && (to->pcdata->destiny_feature == 0 || to->pcdata->destiny_feature_room != to->in_room->vnum) && !power_bound(to) && to->pcdata->summon_bound <= 0 && to->pcdata->destiny_feature == 0
              && light_level(to->in_room) > 0
              && is_possessing(to) == FALSE) {
            to->pcdata->wander_time++;
            if(to->pcdata->wander_time >= 60)
            send_domain_home(to);
          }
          else if(to->pcdata->wander_time > 0)
          {
            to->pcdata->wander_time--;
          }

          if (number_percent() % 3 == 0 && (IS_FLAG(to->act, PLR_BOUND) || IS_FLAG(to->act, PLR_BOUNDFEET))) {
            act("You break free.\n\r", to, NULL, NULL, TO_CHAR);
            act("$n breaks free.\n\r", to, NULL, NULL, TO_ROOM);
            to->hit = max_hp(to);
            if (IS_FLAG(to->act, PLR_BOUND))
            REMOVE_FLAG(to->act, PLR_BOUND);
            if (IS_FLAG(to->act, PLR_BOUNDFEET))
            REMOVE_FLAG(to->act, PLR_BOUNDFEET);
            if (IS_FLAG(to->comm, COMM_BLINDFOLD))
            REMOVE_FLAG(to->comm, COMM_BLINDFOLD);
          }
        }

        bloodstorm_update(to);

        if (in_world(to) == WORLD_EARTH && IS_FLAG(to->act, PLR_STASIS)) {
          log_string("Stasis logoff");
          real_quit(to);
          return;
        }
        if(!IS_FLAG(to->act, PLR_GUEST) && !is_gm(to) && !higher_power(to))
        {

          CHAR_DATA *co;
          for (DescList::iterator cit = descriptor_list.begin();
          cit != descriptor_list.end(); ++cit) {
            DESCRIPTOR_DATA *d = *cit;
            if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
              co = d->character;
              if (IS_NPC(co))
              continue;

              if(IS_FLAG(co->act, PLR_GUEST) && co->pcdata->guest_type == GUEST_OPERATIVE && !str_cmp(co->pcdata->guest_of, to->name))
              {
                send_to_char("You have an active operative.\n\r", to);
                real_quit(to);
                return;
              }

            }
          }
        }
        if(multiplayer_super(to))
        {
          send_to_char("You cannot have two supernatural characters logged on at the same time.\n\r", to);
          real_quit(to);
          return;
        }
        if(multiplayer_three(to))
        {
          send_to_char("You cannot have three characters logged on at the same time.\n\r", to);
          real_quit(to);
          return;
        }



      }
    }
    ai_minute();
  }

  void clock_update(void) {
    if (clock_hour != get_hour(NULL)) {
      clock_hour++;
      if (clock_hour >= 24)
      clock_hour = 0;
      hourly_update(clock_hour);
    }
    if (clock_hour != get_hour(NULL)) {
      clock_hour++;
      if (clock_hour >= 24)
      clock_hour = 0;
      hourly_update(clock_hour);
    }
    if (clock_minute != get_minute()) {
      clock_minute++;
      if (clock_minute >= 60)
      clock_minute = 0;
      minute_update(clock_minute);
    }
    /*
if(clock_minute != get_minute())
{
clock_minute++;
if(clock_minute >= 60)
clock_minute = 0;
minute_update(clock_minute);
}
*/
    clock_second = (current_time % 60) * PULSE_PER_SECOND;
  }

  int terrain_pointer(const char * argument)
  {
    if (!str_cmp(argument, "forest")) {
      return BATTLE_FOREST;
    }
    else if (!str_cmp(argument, "field")) {
      return BATTLE_FIELD;
    }
    else if (!str_cmp(argument, "desert")) {
      return BATTLE_DESERT;
    }
    else if (!str_cmp(argument, "town")) {
      return BATTLE_TOWN;
    }
    else if (!str_cmp(argument, "city")) {
      return BATTLE_CITY;
    }
    else if (!str_cmp(argument, "mountains")) {
      return BATTLE_MOUNTAINS;
    }
    else if (!str_cmp(argument, "warehouse")) {
      return BATTLE_WAREHOUSE;
    }
    else if (!str_cmp(argument, "caves")) {
      return BATTLE_CAVE;
    }
    else if (!str_cmp(argument, "village")) {
      return BATTLE_VILLAGE;
    }
    else if (!str_cmp(argument, "tundra")) {
      return BATTLE_TUNDRA;
    }
    else if (!str_cmp(argument, "lake")) {
      return BATTLE_LAKE;
    }
    else {
      return BATTLE_FIELD;
    }

  }

  void update_ai_operation(std::vector<std::string> tarray) {
    for(vector<OPERATION_TYPE *>::iterator it = OpVect.begin();it !=
    OpVect.end();++it)
    {
      FACTION_TYPE *fac = clan_lookup((*it)->faction);
      if(!str_cmp(fac->name, tarray[1].c_str()) && strlen((*it)->description) < 10 && fac->antagonist == 1)
      {
        (*it)->terrain =  terrain_pointer(tarray[2].c_str());
        free_string((*it)->room_name);
        (*it)->room_name = str_dup(tarray[3].c_str());
        free_string((*it)->upload_name);
        (*it)->upload_name = str_dup(tarray[4].c_str());
        free_string((*it)->description);
        (*it)->description = str_dup(tarray[5].c_str());
      }
    }
  }
  void update_ai_doom(std::vector<std::string> tarray)
  {
    struct stat sb;
    char buf[MSL];
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    d.original = NULL;
    if ((victim = get_char_world_pc(const_cast<char*>(tarray[1].c_str()))) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Offline doom");

      if (!load_char_obj(&d, const_cast<char*>(tarray[1].c_str()))) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(tarray[1].c_str()));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }
    if(strlen(victim->pcdata->doom_desc) > 1)
    {
      if (!online)
      free_char(victim);
      return;
    }
    int days = std::stoi(tarray[2]);
    char logs[MSL];
    sprintf(logs, "Days: %d", days);
    log_string(logs);
    free_string(victim->pcdata->doom_desc);
    victim->pcdata->doom_desc = str_dup(tarray[3].c_str());
    victim->pcdata->doom_date = current_time + (days * 3600 * 24);
    sprintf(buf, "%s\nYou receive a prophecy: %s", victim->pcdata->messages, tarray[3].c_str());
    free_string(victim->pcdata->messages);
    victim->pcdata->messages = str_dup(buf);
    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
    else
    printf_to_char(victim, "You receive a prophecy: %s\n\r", victim->pcdata->doom_desc);
  }

  void update_ai_social(std::vector<std::string> tarray)
  {
    log_string("AI SOCIAL UPDATE");
    for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
    it != MatchVect.end(); ++it) {
      if(!str_cmp((*it)->nameone, tarray[1].c_str()) && !str_cmp((*it)->nametwo, tarray[3].c_str()))
      {
        log_string("FOUND1");
        (*it)->score_one_auto_chat = std::stoi(tarray[2]);
        (*it)->score_two_auto_chat = std::stoi(tarray[4]);
        free_string((*it)->auto_chat_review);
        (*it)->auto_chat_review = str_dup(tarray[5].c_str());
        (*it)->auto_score_when = current_time;
      }
      else if(!str_cmp((*it)->nameone, tarray[3].c_str()) && !str_cmp((*it)->nametwo, tarray[1].c_str()))
      {
        log_string("FOUND2");
        (*it)->score_one_auto_chat = std::stoi(tarray[4]);
        (*it)->score_two_auto_chat = std::stoi(tarray[2]);
        free_string((*it)->auto_chat_review);
        (*it)->auto_chat_review = str_dup(tarray[5].c_str());
        (*it)->auto_score_when = current_time;
      }
    }
  }



  void ptm_log(char * name, const char *argument) {
    FILE *fpout;
    char buf[MSL];
    sprintf(buf, "../ptm/%s.txt", name);
    if ((fpout = fopen(buf, "a+")) == NULL) {
      perror(buf);
      return;
    }

    char tmp[MSL];
    char datestr[MSL];
    time_t east_time = current_time;

    sprintf(tmp, "%s", (char *)ctime(&east_time));
    sprintf(datestr, "%c%c%c, %c%c%c %c%c, %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);

    fprintf(fpout, "[%s] %s\n\n", datestr, argument);
    fclose(fpout);
  }


  void update_ai_news(std::vector<std::string> tarray)
  {
    char logs[MSL];
    sprintf(logs, "AINEWS: %s", tarray[1].c_str());
    log_string(logs);
    NEWS_TYPE *news;
    news = new_news();
    free_string(news->author);
    news->author = str_dup("Supernatural News");
    news->timer = 1000;
    free_string(news->message);
    news->message = str_dup(tarray[1].c_str());
    news->stats[0] = -2;
    NewsVect.push_back(news);

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


      if (can_see_news(victim, news))
      printf_to_char(victim, "`157Supernatural Rumors:`x\n\r%s\n\r", news->message);
    }
    FACTION_TYPE *fac = clan_lookup(FACTION_HAND);
    if(!str_cmp(tarray[3].c_str(), fac->reportone_title) && fac->reportone_plog_sent == 0)
    {
      for(int i=0;i<50;i++)
      {
        if(safe_strlen(fac->reportone_participants[i]) > 1)
        ptm_log(fac->reportone_participants[i], tarray[1].c_str());
      }
      fac->reportone_plog_sent = 1;
    }
    if(!str_cmp(tarray[3].c_str(), fac->reporttwo_title) && fac->reporttwo_plog_sent == 0)
    {
      for(int i=0;i<50;i++)
      {
        if(safe_strlen(fac->reporttwo_participants[i]) > 1)
        ptm_log(fac->reporttwo_participants[i], tarray[1].c_str());
      }
      fac->reporttwo_plog_sent = 1;
    }
    if(!str_cmp(tarray[3].c_str(), fac->reportthree_title) && fac->reportthree_plog_sent == 0)
    {
      for(int i=0;i<50;i++)
      {
        if(safe_strlen(fac->reportthree_participants[i]) > 1)
        ptm_log(fac->reportthree_participants[i], tarray[1].c_str());
      }
      fac->reportthree_plog_sent = 1;
    }
    for(int k=0;k<20;k++)
    {
      if(!str_cmp(tarray[3].c_str(), fac->event_title[k]) && fac->event_plog_sent[k] == 0)
      {
        for(int i=0;i<50;i++)
        {
          if(safe_strlen(fac->event_participants[k][i]) > 1)
          ptm_log(fac->event_participants[k][i], tarray[1].c_str());
        }
        fac->event_plog_sent[k] = 1;
      }
    }


  }
  //    outstring = "6|||" + cname + "|||" + nname + "|||" + nsurname + "|||" + nintro + "|||" + ndesc
  void update_operative(std::vector<std::string> tarray)
  {
    try {
      CHAR_DATA *orig = get_char_world_pc(const_cast<char*>(tarray[1].c_str()));
      if(orig == NULL)
      {
        return;
      }
      if(orig->pcdata->operative_creation_type == 0)
      return;
      int ftype = orig->pcdata->operative_creation_type;
      FACTION_TYPE *fac;
      if(ftype == FACTION_CORE)
      {
        fac = clan_lookup(orig->fcore);
        free_string(orig->pcdata->operative_core);
        orig->pcdata->operative_core = str_dup(tarray[2].c_str());
      }
      else if(ftype == FACTION_SECT)
      {
        fac = clan_lookup(orig->fsect);
        free_string(orig->pcdata->operative_sect);
        orig->pcdata->operative_sect = str_dup(tarray[2].c_str());
      }
      else if(ftype == FACTION_CULT)
      {
        fac = clan_lookup(orig->fcult);
        free_string(orig->pcdata->operative_cult);
        orig->pcdata->operative_cult = str_dup(tarray[2].c_str());
      }
      else
      return;
      char olook[MSL];
      sprintf(olook, "%s", get_intro(orig));
      int lf_taken = orig->lf_taken;
      int lf_used = orig->lf_used;
      int wounds = orig->wounds;
      int htimer = orig->heal_timer;
      int intel = orig->pcdata->intel;
      bool sranged = FALSE;
      bool lranged = FALSE;
      bool smelee = FALSE;
      bool lmelee = FALSE;
      bool armor = FALSE;
      if(get_lmelee(orig) != NULL)
      lmelee = TRUE;
      if(get_smelee(orig) != NULL)
      smelee = TRUE;
      if(get_lranged(orig) != NULL)
      lranged = TRUE;
      if(get_sranged(orig) != NULL)
      sranged = TRUE;
      if(get_armor(orig) != NULL)
      armor = TRUE;

      ROOM_INDEX_DATA *inroom = orig->in_room;
      bool bhands = FALSE;
      bool bfeet = FALSE;
      if(IS_FLAG(orig->act, PLR_BOUNDFEET))
      {
        bfeet = TRUE;
        REMOVE_FLAG(orig->act, PLR_BOUNDFEET);
      }
      if(IS_FLAG(orig->act, PLR_BOUND))
      {
        bhands = TRUE;
        REMOVE_FLAG(orig->act, PLR_BOUND);
      }
      PROP_TYPE * prop = prop_from_room(inroom);
      ROOM_INDEX_DATA *home = get_fleeroom(orig, prop);
      char_from_room(orig);
      char_to_room(orig, home);
      char tempname[MSL];
      save_char_obj(orig, FALSE, FALSE);
      sprintf(tempname, "%s", orig->name);
      CHAR_DATA *ch = orig;
      ch->pcdata->operative_creation_type = 0;
      free_string(ch->name);
      ch->name = str_dup(tarray[2].c_str());
      free_string(ch->pcdata->last_name);
      ch->pcdata->last_name = str_dup(tarray[3].c_str());
      free_string(ch->pcdata->intro_desc);
      ch->pcdata->intro_desc = str_dup(tarray[4].c_str());
      char_from_room(ch);
      char_to_room(ch, inroom);
      if(bhands)
      {
        SET_FLAG(ch->act, PLR_BOUND);
      }
      if(bfeet)
      {
        SET_FLAG(ch->act, PLR_BOUNDFEET);
      }
      free_string(ch->pcdata->whotitle);
      ch->pcdata->whotitle = str_dup("");
      ch->fcore = 0;
      ch->fsect = 0;
      ch->fcult = 0;
      ch->faction = 0;
      ch->factiontwo = 0;
      free_string(ch->pcdata->maim);
      ch->pcdata->maim = str_dup("");

      if(ftype == FACTION_CORE)
      {
        ch->fcore = fac->vnum;
        ch->faction = fac->vnum;
      }
      else if(ftype == FACTION_SECT)
      {
        ch->fsect = fac->vnum;
        ch->faction = fac->vnum;
      }
      else if(ftype == FACTION_CULT)
      {
        ch->fcult = fac->vnum;
        ch->faction = fac->vnum;
      }
      ch->lf_taken = lf_taken;
      ch->lf_used = lf_used;
      ch->wounds = wounds;
      ch->heal_timer = htimer;
      ch->pcdata->intel = intel;
      free_string(ch->pcdata->guest_of);
      ch->pcdata->guest_of = str_dup(tempname);
      ch->money = 0;
      ch->pcdata->total_money = 0;
      ch->pcdata->total_credit = UMIN(ch->pcdata->total_credit, 15000);

      OBJ_DATA *obj;
      int p = 0;
      for (ObjList::iterator it = object_list.begin();
      it != object_list.end() && p < 500000; p++) {
        obj = *it;
        ++it;

        if (obj == NULL)
        continue;

        if (obj->carried_by == ch) {
          obj_from_char_silent(obj);
          extract_obj(obj);
        }
      }
      free_string(ch->pcdata->hair_dyed);
      ch->pcdata->hair_dyed = str_dup("");
      SET_FLAG(ch->act, PLR_GUEST);
      ch->pcdata->guest_type = GUEST_OPERATIVE;

      for (int i = 0; i < MAX_COVERS + 1; i++) {
        free_string(ch->pcdata->focused_descs[i]);
        ch->pcdata->focused_descs[i] = str_dup("");
      }
      free_string(ch->description);
      ch->description = str_dup(tarray[5].c_str());
      ch->id = get_pc_id();
      free_string(ch->short_descr);
      ch->short_descr = str_dup(ch->name);
      free_string(ch->long_descr);
      ch->long_descr = str_dup(ch->name);
      set_natural_hair(ch, str_dup(tarray[6].c_str()));
      set_natural_eyes(ch, str_dup(tarray[7].c_str()));
      ch->pcdata->height_feet = std::stoi(tarray[8]);
      ch->pcdata->height_inches = std::stoi(tarray[9]);
      free_string(ch->pcdata->skin);
      ch->pcdata->skin = str_dup(tarray[10].c_str());
      if(strcasestr(ch->pcdata->skin, "skin") == NULL)
      {
        char nskin[MSL];
        sprintf(nskin, "%s skin", ch->pcdata->skin);
        free_string(ch->pcdata->skin);
        ch->pcdata->skin = str_dup(nskin);
      }
      free_string(ch->pcdata->scent);
      ch->pcdata->scent = str_dup("");
      free_string(ch->pcdata->makeup_light);
      ch->pcdata->makeup_light = str_dup("");
      free_string(ch->pcdata->makeup_medium);
      ch->pcdata->makeup_medium = str_dup("");
      free_string(ch->pcdata->makeup_heavy);
      ch->pcdata->makeup_heavy = str_dup("");
      if(ch->sex == SEX_FEMALE)
      {
        ch->pcdata->bust += number_range(-2, 2);
      }
      ch->pcdata->birth_day -= (number_range(0, 5000));

      while (ch->pcdata->birth_day <= 0) {
        ch->pcdata->birth_month--;
        ch->pcdata->birth_day += 28;
      }

      while (ch->pcdata->birth_month <= 0) {
        ch->pcdata->birth_year--;
        ch->pcdata->birth_month += 11;
      }


      operative_outfit(ch, sranged, lranged, smelee, lmelee, armor);
      char buf[MSL];
      sprintf(buf, "%s is revealed as $n.", olook);
      act(buf, ch, NULL, NULL, TO_ROOM);
      act(buf, ch, NULL, NULL, TO_CHAR);
    } catch (const std::exception& e) {
      bug("Error running AI job.", 0);
    }

  }

  void run_ai_job(const std::string& inputString) {
    try {
      log_string(inputString.c_str());
      std::vector<std::string> tarray = splitString(inputString, "|||");
      int arg1 = std::stoi(tarray[0]);
      if (arg1 == 1) {
        int arg2 = std::stoi(tarray[1]);
        std::string arg3 = tarray[2];
        add_encounter(1, arg2, &arg3[0], 0);
      }
      if( arg1 == 2) {
        update_ai_operation(tarray);
      }
      if(arg1 == 3)
      {
        update_ai_doom(tarray);
      }
      if(arg1 == 4)
      {
        update_ai_social(tarray);
      }
      if(arg1 == 5)
      {
        update_ai_news(tarray);
      }
      if(arg1 == 6)
      {
        update_operative(tarray);
      }
    } catch (const std::exception& e) {
      bug("Error running AI job.", 0);
    }
  }

  void ai_update()
  {
    std::string line = readAndDeleteLineFromFile(AI_OUT_FILE);
    if (!line.empty()) {
      run_ai_job(line);
    }
    std::string sline = readAndDeleteLineFromFile(AI_SUM_OUT_FILE);
    if (!sline.empty()) {
      run_ai_job(sline);
    }

  }

  void ai_encounter_job()
  {
    char buf[MSL];
    for(int i=1;i<53;i++)
    {
      bool found = FALSE;
      for (vector<EXTRA_ENCOUNTER_TYPE *>::iterator it = EEncounterVect.begin();
      it != EEncounterVect.end(); ++it) {
        if((*it)->btype == 1 && (*it)->encounter_id == i)
        {
          found = TRUE;
          break;
        }
      }
      if(found == FALSE)
      {
        sprintf(buf, "1,%d,,,,,", i);
        writeLineToFile(AI_IN_FILE, str_dup(buf));
        return;
      }
    }
  }
  void ai_operation_job()
  {
    char buf[MSL];
    for(vector<OPERATION_TYPE *>::iterator it = OpVect.begin();it !=
    OpVect.end();++it)
    {
      FACTION_TYPE *fac = clan_lookup((*it)->faction);
      if(strlen((*it)->description) < 10 && fac->antagonist == 1)
      {
        sprintf(buf, "2,0,%s,\"%s\",,,", fac->name, territory_by_number((*it)->territoryvnum)->name);

        writeLineToFile(AI_IN_FILE, str_dup(buf));}
    }
  }

  void ai_log_job()
  {
    FACTION_TYPE *fac = clan_lookup(FACTION_HAND);
    char sout[MSL];
    if(fac->reportone_log_sent == 0 && fac->reportone_time < current_time - (3600 * 24))
    {
      sprintf(sout, "%d,%d,~%s~,~", 1, 0, fac->reportone_title);
      writeTextToFile(AI_SUM_IN_FILE, str_dup(sout));
      writeTextToFile(AI_SUM_IN_FILE, fac->reportone_text);
      for(int i=0;i<10;i++)
      {
        if(strlen(fac->report_overflow[0][i]) > 2)
        writeTextToFile(AI_SUM_IN_FILE, fac->report_overflow[0][i]);
      }
      writeLineToFile(AI_SUM_IN_FILE, "~");
      fac->reportone_log_sent = 1;
    }
    if(fac->reporttwo_log_sent == 0 && fac->reporttwo_time < current_time - (3600 * 24))
    {
      sprintf(sout, "%d,%d,~%s~,~", 1, 0, fac->reporttwo_title);
      writeTextToFile(AI_SUM_IN_FILE, str_dup(sout));
      writeTextToFile(AI_SUM_IN_FILE, fac->reporttwo_text);
      for(int i=0;i<10;i++)
      {
        if(strlen(fac->report_overflow[1][i]) > 2)
        writeTextToFile(AI_SUM_IN_FILE, fac->report_overflow[1][i]);
      }
      writeLineToFile(AI_SUM_IN_FILE, "~");
      fac->reporttwo_log_sent = 1;
    }
    if(fac->reportthree_log_sent == 0 && fac->reportthree_time < current_time - (3600 * 24))
    {
      sprintf(sout, "%d,%d,~%s~,~", 1, 0, fac->reportthree_title);
      writeTextToFile(AI_SUM_IN_FILE, str_dup(sout));
      writeTextToFile(AI_SUM_IN_FILE, fac->reportthree_text);
      for(int i=0;i<10;i++)
      {
        if(strlen(fac->report_overflow[2][i]) > 2)
        writeTextToFile(AI_SUM_IN_FILE, fac->report_overflow[2][i]);
      }
      writeLineToFile(AI_SUM_IN_FILE, "~");
      fac->reportthree_log_sent = 1;
    }
    for(int p =0;p<20;p++)
    {
      if(fac->event_log_sent[p] == 0 && fac->event_time[p] < current_time - (3600 * 24) && strlen(fac->event_title[p]) > 2)
      {
        sprintf(sout, "%d,%d,~%s~,~", 2, fac->event_type[p], from_color(fac->event_title[p]));
        writeTextToFile(AI_SUM_IN_FILE, str_dup(sout));
        for(int i=0;i<20;i++)
        {
          if(strlen(fac->event_text[p][i]) > 2)
          writeTextToFile(AI_SUM_IN_FILE, fac->event_text[p][i]);
        }
        writeLineToFile(AI_SUM_IN_FILE, "~");
        fac->event_log_sent[p] = 1;
      }
    }

  }

  void ai_minute()
  {
    ai_encounter_job();
    ai_operation_job();
    ai_log_job();
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

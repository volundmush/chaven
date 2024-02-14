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

#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif

  bool has_exit args((ROOM_INDEX_DATA * room, int dir));

  int get_flee_direction args((CHAR_DATA * ch));
  int GET_NPC_SPECIAL args((int dis));
  bool is_protected args((CHAR_DATA * ch));
  CHAR_DATA *get_protector args((CHAR_DATA * ch));
  void fleeroom args((CHAR_DATA * ch));
  char *special_name args((int disc));
  bool fight_over args((ROOM_INDEX_DATA * room));
  void summon_support args((CHAR_DATA * ch, int type));
  void killcivilian args((CHAR_DATA * ch, CHAR_DATA *victim));
  int xp_per_attack args((CHAR_DATA * ch, CHAR_DATA *victim, int disc, int damage));
  int xp_per_attack_old args((CHAR_DATA * ch, CHAR_DATA *victim, int disc, int damage));
  void delayattacks args((CHAR_DATA * ch, CHAR_DATA *victim));
  CHAR_DATA *get_npc_target args((CHAR_DATA * ch));
  void death_cause args((CHAR_DATA * ch, CHAR_DATA *victim, int disc));
  int dam_caff_mod args((CHAR_DATA * ch, CHAR_DATA *victim, int dam, int disc, bool lower));
  int move_caff_mod args((CHAR_DATA * ch, int dist));
  void dead_explode args((CHAR_DATA * ch));
  CHAR_DATA *trickcheck args((CHAR_DATA * ch));
  void lower_caff args((CHAR_DATA * ch, int aff));
  void reset_caff args((CHAR_DATA * ch, int aff));
  bool shroud_monster args((CHAR_DATA * ch));
  void bodyguard_check args((CHAR_DATA * ch, CHAR_DATA *victim));
  int damage_calculate args((CHAR_DATA * ch, CHAR_DATA *victim, int point, int shield));
  int round_cap args((CHAR_DATA * ch, CHAR_DATA *victim));
  int muscle_count args((CHAR_DATA * ch));
  int apply_stagger args((CHAR_DATA * ch, CHAR_DATA *victim, int dam, int disc));

  void add_aggro args((CHAR_DATA * victim, CHAR_DATA *ch, int amount));
  void start_roomfight args((ROOM_INDEX_DATA * room, bool fast, int speed, bool shroud, bool deepshroud));
  int combat_distance args((CHAR_DATA * ch, CHAR_DATA *victim, bool attacking));
  bool is_enemy args((CHAR_DATA * ch, CHAR_DATA *victim));
  bool has_enemy args((CHAR_DATA * ch));
  bool same_fight args((CHAR_DATA * ch, CHAR_DATA *victim));
  bool in_fight args((CHAR_DATA * ch));
  CHAR_DATA *next_fight_member args((CHAR_DATA * current));
  CHAR_DATA *next_fight_member_init args((CHAR_DATA * current));
  int fight_pop args((CHAR_DATA * ch));
  bool can_get_to args((CHAR_DATA * ch, ROOM_INDEX_DATA *desti));
  bool is_combat_jumper args((CHAR_DATA * ch));
  void fall_character args((CHAR_DATA * ch));
  void swimup_character args((CHAR_DATA * ch));
  bool is_combat_flyer args((CHAR_DATA * ch));
  bool is_attacking args((CHAR_DATA * ch));
  bool valid_minion args((CHAR_DATA * ch));
  bool valid_ally args((CHAR_DATA * ch));
  bool invisioncone_running args((CHAR_DATA * ch, CHAR_DATA *victim));
  bool behindyou_running args((CHAR_DATA * ch, CHAR_DATA *victim));
  OBJ_DATA *get_lmelee args((CHAR_DATA * ch));
  OBJ_DATA *get_smelee args((CHAR_DATA * ch));
  OBJ_DATA *get_lranged args((CHAR_DATA * ch));
  OBJ_DATA *get_sranged args((CHAR_DATA * ch));
  OBJ_DATA *get_shield_obj args((CHAR_DATA * ch));
  int damage_echo args((CHAR_DATA * ch, CHAR_DATA *victim, int realdisc, int disc, int dam, int range, bool damdone));
  int monster_round_cap args((CHAR_DATA * ch, CHAR_DATA *victim));
  int average_muscle args((CHAR_DATA * ch));
  int init_combat_distance args((CHAR_DATA * ch, CHAR_DATA *victim, bool attacking));
  bool pvp_target args((CHAR_DATA * ch, CHAR_DATA *victim));
  bool pvp_character args((CHAR_DATA * ch));

  // 6, 2
#define HIT_FATIGUE 10
#define MOVE_FATIGUE 3

  int map_translation[50] = {0};
  int butcher_count = 0;
  int fight_problem = 0;

  void multi_hit(CHAR_DATA *ch, CHAR_DATA *victim) {}

  void stop_fighting(CHAR_DATA *ch, bool both) {}

  void slay_message(CHAR_DATA *ch, CHAR_DATA *victim) {}

  void mortal_message(CHAR_DATA *ch, CHAR_DATA *victim) {}

  bool is_cover(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if ((IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET)))
    return TRUE;

    return FALSE;
  }

  bool is_antibullet(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (is_undead(ch))
    return TRUE;

    if (ch->shape != SHAPE_HUMAN)
    return FALSE;

    if (ch->disciplines[DIS_BARMOR] + ch->disciplines[DIS_BSHIELD] >
        ch->disciplines[DIS_MARMOR] + ch->disciplines[DIS_MSHIELD])
    return TRUE;

    return FALSE;
  }
  bool is_antimelee(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (is_undead(ch))
    return FALSE;

    if (ch->shape != SHAPE_HUMAN)
    return TRUE;

    if (ch->disciplines[DIS_MARMOR] + ch->disciplines[DIS_MSHIELD] >=
        ch->disciplines[DIS_BARMOR] + ch->disciplines[DIS_BSHIELD])
    return TRUE;

    return FALSE;
  }

  char *disc_name(CHAR_DATA *ch, int point) {
    if (IS_NPC(ch) || available_donated(ch) < 1750) {
      return discipline_table[point].name;
    }
    if (discipline_table[point].vnum == DIS_CUSTOM)
    return ch->pcdata->cdisc_name;

    return discipline_table[point].name;
  }

  int disc_range(CHAR_DATA *ch, int point) {
    if (point < 0)
    return 0;

    int range = discipline_table[point].range;
    if (point == DIS_RIFLES - 1 && get_skill(ch, SKILL_SNIPERTRAINING) > 0)
    range += 15;
    if (point == DIS_CARBINES - 1 && get_skill(ch, SKILL_SNIPERTRAINING) > 0)
    range += 10;

    if (point == DIS_CUSTOM - 1)
    range = discipline_table[custom_vnum(ch) - 1].range;

    return range;
  }

  bool will_fight_show(CHAR_DATA *ch, bool damage) {

    if (IS_NPC(ch)) {
      if (is_cover(ch) && damage == FALSE)
      return FALSE;
    }
    if (IS_NPC(ch)) {
      if (npc_pop(ch->in_room) < 4)
      return TRUE;
      else if (npc_pop(ch->in_room) < 4 && damage == TRUE)
      return TRUE;
    }
    if (!IS_NPC(ch)) {
      if (pc_pop(ch->in_room) < 4)
      return TRUE;
      else if (pc_pop(ch->in_room) < 6 && damage == TRUE)
      return TRUE;
    }
    return FALSE;
  }

  int damage_mod(int distance, int range) {
    int crit = (int)(range / 2);

    if (distance == crit)
    return 175;

    if (distance <= 1)
    return 0;

    int mod = (int)(range / 10);

    if (distance <= crit + 10 && distance >= crit - 10)
    return 150;

    if (distance >= range - mod)
    return 35;

    if (distance <= mod)
    return 35;

    if (distance > crit)
    return 125;

    return 100;
  }

  int get_shadow(CHAR_DATA *ch, int val) {
    for (int i = 0; i < 20; i++) {
      if (ch->pcdata->shadow_attacks[i][0] == val && ch->pcdata->shadow_attacks[i][1] != 0 && discipline_table[ch->pcdata->shadow_attacks[i][1]].pc > 0)
      return ch->pcdata->shadow_attacks[i][1];
    }
    return -1;
  }

  void end_fight(ROOM_INDEX_DATA *room) {
    CHAR_DATA *wch;
    int limit = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && limit < 1000; ++it) {
      limit++;
      wch = *it;
      if (wch == NULL || wch->act == NULL || wch->in_room == NULL)
      continue;
      if (wch->in_room->vnum != room->vnum)
      continue;

      wch->in_fight = FALSE;
    }
  }

  void check_position(CHAR_DATA *ch) {
    switch (ch->position) {
    case POS_MORTAL:
      act("$n is mortally wounded, and will die soon, if not aided.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You are mortally wounded, and will die soon, if not aided.\n\r", ch);
      break;
    case POS_INCAP:
      if (!IS_FLAG(ch->act, PLR_BOUND))
      send_to_char("You are incapacitated.\n\r", ch);
      break;
    case POS_STUNNED:
      act("$n is stunned, but will probably recover.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You are stunned, but will probably recover.\n\r", ch);
      stop_fighting(ch, TRUE);
      break;
    case POS_DEAD:
      break;
    default:
      if (ch->hit < max_hp(ch) / 4 && number_percent() % 6 == 0)
      send_to_char("You sure are Hurting!\n\r", ch);
      break;
    }
  }

  void make_corpse(CHAR_DATA *ch, CHAR_DATA *killer) {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if (!is_vampire(ch)) {
      name = ch->short_descr;
      corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
      if (IS_NPC(ch)) {
        corpse->timer = 0;
        corpse->rot_timer = number_range(30, 60);
      }
      else {
        corpse->timer = (60 * 72);
        corpse->rot_timer = 0;
        //            corpse->timer   = number_range( 10, 20 );
        //            corpse->rot_timer = number_range(10, 20);

        if (get_skill(ch, SKILL_HYPERREGEN) > 0 && ch->race != RACE_WIGHT) {
          free_string(corpse->material);
          corpse->material = str_dup(ch->name);
          corpse->timer = 20;

          char buf[MSL];
          sprintf(buf, "HYPERREGEN: %s died.\n\r", ch->name);
          log_string(buf);
        }
      }

      corpse->cost = 0;
      if (!IS_NPC(ch))
      obj_to_obj(create_money(ch->money, ch), corpse);
      corpse->carried_by = NULL;

      if (IS_NPC(ch)) {

        sprintf(buf, corpse->short_descr, name);
        free_string(corpse->short_descr);
        corpse->short_descr = str_dup(buf);

        sprintf(buf, corpse->description, name);
        free_string(corpse->description);
        corpse->description = str_dup(buf);

        for (int i = 0; i < 33; i++) {
          if (ch->pIndexData->vnum == monster_table[i].vnum) {
            if (IS_FLAG(ch->act, ACT_BIGGAME) || in_world(ch) != WORLD_EARTH) {
              if (monster_table[i].humanoid == 1 && (number_percent() % 2 == 0 || IS_FLAG(ch->act, ACT_BIGGAME) || ch->valuable == TRUE)) {
                OBJ_DATA *obj;
                obj = create_object(get_obj_index(36), 0);
                obj->level = 0;
                obj->size = 10;
                free_string(obj->name);
                obj->name = str_dup(monster_table[i].object);
                free_string(obj->short_descr);
                obj->short_descr = str_dup(monster_table[i].object);
                char tmp[MSL];
                sprintf(tmp, "%s %s", a_or_an(monster_table[i].object), monster_table[i].object);
                free_string(obj->description);
                obj->description = str_dup(tmp);
                if (number_percent() % 3 == 0 || IS_FLAG(ch->act, ACT_BIGGAME) || ch->valuable == TRUE)
                obj->level = 3 * get_demon_lvl(i) / 20;
                obj_to_obj(obj, corpse);
              }
              else if (monster_table[i].humanoid == 0) {
                corpse->value[3] = ch->pIndexData->vnum;
                corpse->value[4] = 0;
                if (ch->valuable == TRUE || IS_FLAG(ch->act, ACT_BIGGAME))
                corpse->value[4] = get_demon_lvl(i) / 10;
              }
            }
          }
        }

      }
      else {
        sprintf(buf, "The corpse of %s", ch->pcdata->intro_desc);
        free_string(corpse->short_descr);
        corpse->short_descr = str_dup(buf);

        sprintf(buf, "The corpse of %s", ch->pcdata->intro_desc);
        free_string(corpse->description);
        corpse->description = str_dup(buf);

        sprintf(buf, "corpse %s %s", ch->pcdata->intro_desc, ch->name);
        free_string(corpse->name);
        corpse->name = str_dup(buf);
        tm *ptm;
        time_t east_time;

        east_time = current_time;
        ptm = gmtime(&east_time);
        int hours = ptm->tm_hour;
        hours = hours + 1; // 12 hour clock adjustment
        hours = hours + 5; // timezone adjustment
        hours += 12;
        if (hours > 23)
        hours -= 24;
        int day = ptm->tm_mday + 1;
        int month = ptm->tm_mon + 1;
        int year = ptm->tm_year + 1900;

        if (killer != NULL && killer != ch && !IS_NPC(killer)) {
          if (killer->pcdata->penis > 0)
          sprintf(
          buf, "The cause of death seems to be %s. Time of death approximately %d on the %d of %d %d.\nYou collect trace evidence of a male with:\n%s\n%s\nAround %d feet tall and %d years old.\n\r", ch->pcdata->deathcause, hours, day, month, year, get_forensic_hair(killer), get_natural_eyes(killer), killer->pcdata->height_feet, get_age(killer) - (get_age(killer) % 5));
          else
          sprintf(
          buf, "The cause of death seems to be %s. Time of death approximately %d on the %d of %d %d.\nYou collect trace evidence of a female with:\n%s\n%s\nAround %d feet tall and %d years old.\n\r", ch->pcdata->deathcause, hours, day, month, year, get_forensic_hair(killer), get_natural_eyes(killer), killer->pcdata->height_feet, get_age(killer) - (get_age(killer) % 5));
        }
        else {
          sprintf(buf, "The cause of death seems to be %s. Time of death approximately %d on the %d of %d %d.\nThere doesn't seem to be any trace evidence.\n\r", ch->pcdata->deathcause, hours, day, month, year);
        }
        EXTRA_DESCR_DATA *ed;

        for (ed = corpse->extra_descr; ed; ed = ed->next) {
          if (is_name("+forensic", ed->keyword))
          break;
        }

        if (!ed) {
          ed = new_extra_descr();
          ed->keyword = str_dup("+forensic");
          ed->next = corpse->extra_descr;
          corpse->extra_descr = ed;
          free_string(corpse->extra_descr->description);
          corpse->extra_descr->description = str_dup(buf);
        }
        else {
          free_string(ed->description);
          ed->description = str_dup(buf);
        }
      }

      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (IS_SET(obj->extra_flags, ITEM_INVENTORY)) {
          obj_from_char_silent(obj);
          extract_obj(obj);
        }
        else {
          // Cloning objects worn so ghosts can appear the same as they were
          // before death - Discordance
          if (obj != NULL) {
            if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_PHONE && obj->item_type != ITEM_RANGED) {
              OBJ_DATA *clone;

              clone = create_object(obj->pIndexData, 0);
              clone_object(obj, clone);
              if (obj->carried_by != NULL)
              obj_to_char(clone, ch);
              wear_obj(ch, clone, FALSE, TRUE);
            }
          }
          obj_from_char_silent(obj);
          obj_to_obj(obj, corpse);
        }
      }
      if (ch->in_room != NULL)
      obj_to_room(corpse, ch->in_room);
      return;
    }
    else {
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        obj_from_char_silent(obj);
        obj_to_room(obj, ch->in_room);
      }
      act("$n disintigrates into ash.", ch, NULL, NULL, TO_ROOM);
      act("You disintigrate into ash.", ch, NULL, NULL, TO_CHAR);
    }
  }

  void make_headless(CHAR_DATA *ch, CHAR_DATA *killer) {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;
    OBJ_DATA *head;

    if (!is_vampire(ch)) {
      name = ch->short_descr;
      corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
      head = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
      if (IS_NPC(ch)) {
        corpse->timer = 0;
        corpse->rot_timer = number_range(30, 60);
      }
      else {
        corpse->timer = (60 * 24);
        corpse->rot_timer = 0;
        //            corpse->timer   = number_range( 10, 20 );
        //            corpse->rot_timer = number_range(10, 20);

        if (get_skill(ch, SKILL_HYPERREGEN) > 0) {
          free_string(corpse->material);
          corpse->material = str_dup(ch->name);
          corpse->timer = 20;
        }
        head->timer = (60 * 24);
        head->rot_timer = 0;

        if (get_skill(ch, SKILL_HYPERREGEN) > 0) {
          free_string(head->material);
          head->material = str_dup(ch->name);
          head->timer = 20;
        }
      }

      head->cost = 0;
      corpse->cost = 0;
      if (!IS_NPC(ch))
      obj_to_obj(create_money(ch->money, ch), corpse);
      corpse->carried_by = NULL;
      head->carried_by = NULL;

      if (IS_NPC(ch)) {

        sprintf(buf, corpse->short_descr, name);
        free_string(corpse->short_descr);
        corpse->short_descr = str_dup(buf);

        sprintf(buf, corpse->description, name);
        free_string(corpse->description);
        corpse->description = str_dup(buf);
      }
      else {
        sprintf(buf, "The corpse of %s", ch->pcdata->intro_desc);
        free_string(corpse->short_descr);
        corpse->short_descr = str_dup(buf);

        sprintf(buf, "The corpse of %s", ch->pcdata->intro_desc);
        free_string(corpse->description);
        corpse->description = str_dup(buf);

        sprintf(buf, "corpse %s %s", ch->pcdata->intro_desc, ch->name);
        free_string(corpse->name);
        corpse->name = str_dup(buf);

        sprintf(buf, "The head of %s", ch->pcdata->intro_desc);
        free_string(head->short_descr);
        head->short_descr = str_dup(buf);

        sprintf(buf, "The head of %s", ch->pcdata->intro_desc);
        free_string(head->description);
        head->description = str_dup(buf);

        sprintf(buf, "head %s %s", ch->pcdata->intro_desc, ch->name);
        free_string(head->name);
        head->name = str_dup(buf);

        if (killer != NULL && killer != ch && !IS_NPC(killer)) {
          if (killer->pcdata->penis > 0)
          sprintf(buf, "The cause of death seems to be %s.\nYou collect trace evidence of a male with:\n%s\n%s\nAround %d feet tall and %d years old.\n\r", ch->pcdata->deathcause, get_forensic_hair(killer), get_natural_eyes(killer), killer->pcdata->height_feet, get_age(killer) - (get_age(killer) % 5));
          else
          sprintf(buf, "The cause of death seems to be %s.\nYou collect trace evidence of a female with:\n%s\n%s\nAround %d feet tall and %d years old.\n\r", ch->pcdata->deathcause, get_forensic_hair(killer), get_natural_eyes(killer), killer->pcdata->height_feet, get_age(killer) - (get_age(killer) % 5));
        }
        else {
          sprintf(buf, "The cause of death seems to be %s.\nThere doesn't seem to be any trace evidence.\n\r", ch->pcdata->deathcause);
        }
        EXTRA_DESCR_DATA *ed;

        for (ed = corpse->extra_descr; ed; ed = ed->next) {
          if (is_name("+forensic", ed->keyword))
          break;
        }

        if (!ed) {
          ed = new_extra_descr();
          ed->keyword = str_dup("+forensic");
          ed->next = corpse->extra_descr;
          corpse->extra_descr = ed;
          free_string(corpse->extra_descr->description);
          corpse->extra_descr->description = str_dup(buf);
        }
        else {
          free_string(ed->description);
          ed->description = str_dup(buf);
        }

        EXTRA_DESCR_DATA *fed;

        for (fed = head->extra_descr; fed; fed = fed->next) {
          if (is_name("+forensic", fed->keyword))
          break;
        }

        if (!fed) {
          fed = new_extra_descr();
          fed->keyword = str_dup("+forensic");
          fed->next = head->extra_descr;
          head->extra_descr = fed;
          free_string(head->extra_descr->description);
          head->extra_descr->description = str_dup(buf);
        }
        else {
          free_string(fed->description);
          fed->description = str_dup(buf);
        }
      }

      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (IS_SET(obj->extra_flags, ITEM_INVENTORY)) {
          obj_from_char_silent(obj);
          extract_obj(obj);
        }
        else {
          // Cloning objects worn so ghosts can appear the same as they were
          // before death - Discordance
          if (obj != NULL) {
            if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_PHONE && obj->item_type != ITEM_RANGED) {
              OBJ_DATA *clone;

              clone = create_object(obj->pIndexData, 0);
              clone_object(obj, clone);
              if (obj->carried_by != NULL)
              obj_to_char(clone, ch);
              wear_obj(ch, clone, FALSE, TRUE);
            }
          }
          obj_from_char_silent(obj);
          obj_to_obj(obj, corpse);
        }
      }
      if (ch->in_room != NULL) {
        obj_to_room(corpse, ch->in_room);
        obj_to_room(head, ch->in_room);
      }
      return;
    }
    else {
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        obj_from_char_silent(obj);
        obj_to_room(obj, ch->in_room);
      }
      act("$n disintigrates into ash.", ch, NULL, NULL, TO_ROOM);
      act("You disintigrate into ash.", ch, NULL, NULL, TO_CHAR);
    }
  }

  void raw_kill(CHAR_DATA *victim, CHAR_DATA *killer) {
    victim->wounds = 4;
    if (IS_NPC(victim)) {
      if (!is_vampire(victim))
      make_corpse(victim, killer);
      victim->attacking = 0;
    }
    char_from_room(victim);
    char_to_room(victim, get_room_index(ROOM_VNUM_LIMBO));
    victim->ttl = 2;
    victim->attacking = 0;

    return;
  }

  void real_kill(CHAR_DATA *victim, CHAR_DATA *killer) {
    if (IS_NPC(victim))
    return;

    if (IS_FLAG(victim->act, PLR_DEAD) && victim->in_room->vnum == 98)
    return;

    if (is_gm(victim))
    return;

    if (get_skill(victim, SKILL_IMMORTALITY) > 0 && !shroud_monster(killer) && !guestmonster(victim) && !higher_power(victim)) {
      return;
    }
    if (higher_power(victim))
    return;

    make_corpse(victim, killer);

    inherit(victim);

    if(IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->guest_type == GUEST_OPERATIVE)
    grief_char(victim->pcdata->guest_of, 15);

    SET_FLAG(victim->act, PLR_DEAD);
    victim->pcdata->final_death_date = current_time;
    char_from_room(victim);
    char_to_room(victim, get_room_index(98));

    if (victim->wounds < 4)
    sympathy_pain(victim->pcdata->pledge, 4);

    send_to_char("So you died, that sucks huh? Anyway feel free to chill out here until you work out what you wanna reroll as.\n\r", victim);

    sr_delete(victim);
    victim->wounds = 0;
    if (safe_strlen(victim->pcdata->enthralling) > 2)
    breakcontrol(victim->pcdata->enthralling, 3);
    if (safe_strlen(victim->pcdata->enrapturing) > 2)
    breakcontrol(victim->pcdata->enrapturing, 4);
    free_string(victim->pcdata->enrapturing);
    victim->pcdata->enrapturing = str_dup("");
    free_string(victim->pcdata->enthralling);
    victim->pcdata->enthralling = str_dup("");
  }

  void real_kill_headless(CHAR_DATA *victim, CHAR_DATA *killer) {
    if (IS_NPC(victim))
    return;

    if (IS_FLAG(victim->act, PLR_DEAD) && victim->in_room->vnum == 98)
    return;

    if (is_gm(victim))
    return;

    make_headless(victim, killer);

    inherit(victim);

    SET_FLAG(victim->act, PLR_DEAD);
    victim->pcdata->final_death_date = current_time;
    char_from_room(victim);
    char_to_room(victim, get_room_index(98));

    send_to_char("So you died, that sucks huh? Anyway feel free to chill out here until you work out what you wanna reroll as.\n\r", victim);

    victim->wounds = 0;
    if (safe_strlen(victim->pcdata->enthralling) > 2)
    breakcontrol(victim->pcdata->enthralling, 3);
    if (safe_strlen(victim->pcdata->enrapturing) > 2)
    breakcontrol(victim->pcdata->enrapturing, 4);
    free_string(victim->pcdata->enrapturing);
    victim->pcdata->enrapturing = str_dup("");
    free_string(victim->pcdata->enthralling);
    victim->pcdata->enthralling = str_dup("");
  }

  void update_pos(CHAR_DATA *victim) {
    if (victim->hit > 0) {
      if (victim->position <= POS_STUNNED)
      victim->position = POS_STANDING;
    }

    if (victim->hit < 0)
    victim->position = POS_INCAP;

    if (victim->hit < -10) {
      victim->position = POS_DEAD;
    }
    check_position(victim);
    return;
  }

  int default_ranged(CHAR_DATA *ch) {
    int i, max = 0, maxpoint = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range < 1)
      continue;
      if (get_disc(ch, discipline_table[i].vnum, FALSE) > max) {
        max = get_disc(ch, discipline_table[i].vnum, FALSE);
        maxpoint = discipline_table[i].vnum;
      }
    }
    return maxpoint;
  }

  int default_base_ranged(CHAR_DATA *ch) {
    int i, max = 0, maxpoint = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range < 1)
      continue;
      if (ch->disciplines[discipline_table[i].vnum] > max) {
        max = ch->disciplines[discipline_table[i].vnum];
        maxpoint = discipline_table[i].vnum;
      }
    }
    return maxpoint;
  }

  int default_melee(CHAR_DATA *ch) {
    int i, max = 0, maxpoint = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != 0)
      continue;
      if (get_disc(ch, discipline_table[i].vnum, FALSE) > max) {
        max = get_disc(ch, discipline_table[i].vnum, FALSE);
        maxpoint = discipline_table[i].vnum;
      }
    }
    return maxpoint;
  }

  int default_ranged_value(CHAR_DATA *ch) {
    int point = 0;
    int vnum = default_ranged(ch);

    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].vnum == vnum)
      point = i;
    }

    return get_disc(ch, discipline_table[point].vnum, FALSE);
  }

  int default_melee_value(CHAR_DATA *ch) {
    int i, point = 0;
    int vnum = default_melee(ch);

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].vnum == vnum)
      point = i;
    }

    return get_disc(ch, discipline_table[point].vnum, FALSE);
  }

  int shield_type(CHAR_DATA *ch) {
    int i, max = 0, maxpoint = DIS_TOUGHNESS, total = 0;

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      if (get_disc(ch, discipline_table[i].vnum, FALSE) > max) {
        max = get_disc(ch, discipline_table[i].vnum, FALSE);
        maxpoint = discipline_table[i].vnum;
      }
      if (get_disc(ch, discipline_table[i].vnum, FALSE) > 0) {
        total += get_disc(ch, discipline_table[i].vnum, FALSE);
      }
    }
    total = UMAX(1, total);
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      if (get_disc(ch, discipline_table[i].vnum, FALSE) > 0) {
        if ((get_disc(ch, discipline_table[i].vnum, FALSE) * 100 / total) >=
            number_percent() && discipline_table[i].vnum != maxpoint) {
          return discipline_table[i].vnum;
        }
      }
    }
    return maxpoint;
  }

  char *punch_location(CHAR_DATA *ch) {
    switch (ch->wound_location % 3) {
    case 0:
      return "stomach";
      break;
    case 1:
      return "chest";
      break;
    case 2:
      return "jaw";
      break;
    }
    return "abdomen";
  }

  char *grapple_location(CHAR_DATA *ch) {
    switch (ch->wound_location % 8) {
    case 0:
      return "right elbow";
      break;
    case 1:
      return "left elbow";
      break;
    case 2:
      return "right wrist";
      break;
    case 3:
      return "left wrist";
      break;
    case 4:
      return "right shoulder";
      break;
    case 5:
      return "left shoulder";
      break;
    case 6:
      return "right knee";
      break;
    case 7:
      return "left knee";
      break;
    }
    return "abdomen";
  }

  char *wound_location(CHAR_DATA *ch) {
    switch (ch->wound_location % 6) {
    case 0:
      return "left shoulder";
      break;
    case 1:
      return "right shoulder";
      break;
    case 2:
      return "right arm";
      break;
    case 3:
      return "left arm";
      break;
    case 4:
      return "right leg";
      break;
    case 5:
      return "left leg";
      break;
    }
    return "abdomen";
  }

  char *undead_location(CHAR_DATA *ch) {
    switch (ch->wound_location % 9) {
    case 0:
      return "left shoulder";
      break;
    case 1:
      return "right shoulder";
      break;
    case 2:
      return "right arm";
      break;
    case 3:
      return "left arm";
      break;
    case 4:
      return "right leg";
      break;
    case 5:
      return "left leg";
      break;
    case 6:
      return "chest";
      break;
    case 7:
      return "stomach";
      break;
    case 8:
      return "forehead";
      break;
    }
    return "abdomen";
  }

  char *graze_location(CHAR_DATA *ch) {
    switch (ch->wound_location % 9) {
    case 0:
      return "left arm";
      break;
    case 1:
      return "right arm";
      break;
    case 2:
      return "left hand";
      break;
    case 3:
      return "right hand";
      break;
    case 4:
      return "forehead";
      break;
    case 5:
      return "cheek";
      break;
    case 6:
      return "left leg";
      break;
    case 7:
      return "right leg";
      break;
    case 8:
      return "ribs";
      break;
    }
    return "abdomen";
  }

  char *defense_message(CHAR_DATA *ch, int type, bool fast, int echo) {
    char buf[MSL];
    char woundloc[MSL];
    char grazeloc[MSL];
    char undeadloc[MSL];
    char punchloc[MSL];
    char grappleloc[MSL];
    sprintf(undeadloc, "%s", undead_location(ch));
    sprintf(punchloc, "%s", punch_location(ch));
    sprintf(grappleloc, "%s", grapple_location(ch));

    if (ch->hit <= 0 && ch->wounds > 0) {
      sprintf(woundloc, "%s", wound_location(ch));
      if (ch->wounds == 1) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "mildly wounds your %s.", woundloc);
          else
          sprintf(buf, "mildly wounds $S %s.", woundloc);
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "mildly wound your %s.", woundloc);
          else
          sprintf(buf, "mildly wound $S %s.", woundloc);
          return str_dup(buf);
        }
      }
      else if (ch->wounds == 2) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "severely wounds your %s.", woundloc);
          else
          sprintf(buf, "severely wounds $S %s.", woundloc);
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "severely wound your %s.", woundloc);
          else
          sprintf(buf, "severely wound $S %s.", woundloc);
          return str_dup(buf);
        }
      }
      else if (ch->wounds == 3) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "critically wounds you.");
          else
          sprintf(buf, "critically wounds $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "critically wound you.");
          else
          sprintf(buf, "critically wound $M.");
          return str_dup(buf);
        }
      }
      else {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "kills you.");
          else
          sprintf(buf, "kills $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "kill you.");
          else
          sprintf(buf, "kill you $M.");
          return str_dup(buf);
        }
      }
    }
    sprintf(grazeloc, "%s", graze_location(ch));

    if (echo == ECHO_GRAZE) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "grazes your %s.", grazeloc);
        else
        sprintf(buf, "grazes $S %s.", grazeloc);
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "graze your %s.", grazeloc);
        else
        sprintf(buf, "graze $S %s.", grazeloc);
        return str_dup(buf);
      }
    }
    if (echo == ECHO_MISS) {
      if (ch->wound_location % 2 != 0 || ch->moved < 5 || ch->in_room->sector_type == SECT_AIR) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "misses you.");
          else
          sprintf(buf, "misses $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "miss you.");
          else
          sprintf(buf, "miss $M.");
          return str_dup(buf);
        }
      }
      if (in_water(ch)) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "sends up a spray of water near your feet.");
          else
          sprintf(buf, "sends up a spray of water near $S feet.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "send up a spray of water near your feet.");
          else
          sprintf(buf, "send up a spray of water near $S feet.");
          return str_dup(buf);
        }
      }
      if (is_snowing(ch->in_room)) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "sends up a spray of water near your feet.");
          else
          sprintf(buf, "sends up a spray of water near $S feet.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "send up a spray of water near your feet.");
          else
          sprintf(buf, "send up a spray of water near $S feet.");
          return str_dup(buf);
        }
      }
      if (is_natural(ch->in_room)) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "sends up a spray of soil near your feet.");
          else
          sprintf(buf, "sends up a spray of soil near $S feet.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "send up a spray of soil near your feet.");
          else
          sprintf(buf, "send up a spray of soil near $S feet.");
          return str_dup(buf);
        }
      }
      else {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "sparks off the ground near your feet.");
          else
          sprintf(buf, "sparks off the ground near $S feet.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "spark off the ground near your feet.");
          else
          sprintf(buf, "spark off the ground near $S feet.");
          return str_dup(buf);
        }
      }
    }
    if (echo == ECHO_COVERSTRIKE) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "damages you.");
        else
        sprintf(buf, "damages it.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "damage you.");
        else
        sprintf(buf, "damage it.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_DODGE) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "is dodged by you.");
        else
        sprintf(buf, "is dodged by $M.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "be dodged by you.");
        else
        sprintf(buf, "be dodged by $M.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_ARMOR) {
      if (ch->disciplines[DIS_MARMOR] >= ch->disciplines[DIS_BARMOR]) {
        if (get_armor(ch) != NULL && number_percent() % 2 == 0) {
          if (fast == TRUE) {
            if (type == TO_VICT)
            sprintf(buf, "deflects off your %s.", get_armor(ch)->short_descr);
            else
            sprintf(buf, "deflects off $S %s.", get_armor(ch)->short_descr);
            return str_dup(buf);
          }
          else {
            if (type == TO_VICT)
            sprintf(buf, "deflect off your %s.", get_armor(ch)->short_descr);
            else
            sprintf(buf, "deflect off $S %s.", get_armor(ch)->short_descr);
            return str_dup(buf);
          }
        }

        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "deflects off your armor.");
          else
          sprintf(buf, "deflects off $S armor.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "deflect off your armor.");
          else
          sprintf(buf, "deflect off $S armor.");
          return str_dup(buf);
        }
      }
      else {
        if (get_armor(ch) != NULL && number_percent() % 2 == 0) {
          if (fast == TRUE) {
            if (type == TO_VICT)
            sprintf(buf, "impacts against your %s.", get_armor(ch)->short_descr);
            else
            sprintf(buf, "impacts against $S %s.", get_armor(ch)->short_descr);
            return str_dup(buf);
          }
          else {
            if (type == TO_VICT)
            sprintf(buf, "impact against your %s.", get_armor(ch)->short_descr);
            else
            sprintf(buf, "impact against $S %s.", get_armor(ch)->short_descr);
            return str_dup(buf);
          }
        }
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "impacts against your armor.");
          else
          sprintf(buf, "impacts against $S armor.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "impact against your armor.");
          else
          sprintf(buf, "impact against $S armor.");
          return str_dup(buf);
        }
      }
    }
    if (echo == ECHO_SHIELD) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "deflects off your shield.");
        else
        sprintf(buf, "deflects off $S shield.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "deflect off your shield.");
        else
        sprintf(buf, "deflect off $S shield.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_SUPERTOUGH) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "ineffectually wounds your %s.", undeadloc);
        else
        sprintf(buf, "ineffectually wounds $S %s.", undeadloc);
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "ineffectually wound your %s.", undeadloc);
        else
        sprintf(buf, "ineffectually wound $S %s.", undeadloc);
        return str_dup(buf);
      }
    }
    if (echo == ECHO_SUPERSHIELD) {
      if (get_disc(ch, DIS_FORCES, FALSE) >= get_disc(ch, DIS_FATE, FALSE) && get_disc(ch, DIS_FORCES, FALSE) >= get_disc(ch, DIS_PUSH, FALSE)) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "deflects off an almost invisible distortion in the air around you.");
          else
          sprintf(buf, "deflects off an almost invisible distortion in the air around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "deflect off an almost invisible distortion in the air around you.");
          else
          sprintf(buf, "deflect off an almost invisible distortion in the air around $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_PUSH, FALSE) >= get_disc(ch, DIS_FATE, FALSE) && get_disc(ch, DIS_PUSH, FALSE) >=
          get_disc(ch, DIS_FORCES, FALSE)) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is pushed aside at the last instant.");
          else
          sprintf(buf, "is pushed aside at the last instant.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be pushed aside at the last instant.");
          else
          sprintf(buf, "be pushed aside at the last instant.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_FATE, FALSE) >= get_disc(ch, DIS_PUSH, FALSE) && get_disc(ch, DIS_FATE, FALSE) >=
          get_disc(ch, DIS_FORCES, FALSE)) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "narrowly misses you.");
          else
          sprintf(buf, "narrowly misses $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "narrowly miss you.");
          else
          sprintf(buf, "narrowly miss $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_ENERGYS, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is absorbed by your energy shield.");
          else
          sprintf(buf, "is absorbed by $S energy shield.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be absorbed by your energy shield.");
          else
          sprintf(buf, "be absorbed by $S energy shield.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_FIRES, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(
          buf, "bounces off the firey shield that flickers to life around you.");
          else
          sprintf(
          buf, "bounces off the firey shield that flickers to life around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(
          buf, "bounce off the firey shield that flickers to life around you.");
          else
          sprintf(
          buf, "bounce off the firey shield that flickers to life around $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_ICES, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is deflected by the shield of ice that appears the instant of impact around you.");
          else
          sprintf(buf, "is deflected by the shield of ice that appears the instant of impact around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be deflected by the shield of ice that appears the instant of impact around you.");
          else
          sprintf(buf, "be deflected by the shield of ice that appears the instant of impact around $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_WOODS, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is deflected by the twisting shield of wood that wraps around you.");
          else
          sprintf(buf, "is deflected by the twisting shield of wood that wraps around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be deflected by the twisting shield of wood that wraps around you.");
          else
          sprintf(buf, "be deflected by the twisting shield of wood that wraps around $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_LIGHTNINGS, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is absorbed by the shield of lightning that flickers to life around you.");
          else
          sprintf(buf, "is absorbed by the shield of lightning that flickers to life around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be absorbed by the shield of lightning that flickers to life around you.");
          else
          sprintf(buf, "be absorbed by the shield of lightning that flickers to life around $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_STONES, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "bounces off the stone barrier that appears before you.");
          else
          sprintf(buf, "bounces off the stone barrier that appears before $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "bounce off the stone barrier that appears before you.");
          else
          sprintf(buf, "bounce off the stone barrier that appears before $M.");
          return str_dup(buf);
        }
      }
      else if (get_disc(ch, DIS_DARKS, FALSE) >= 5) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "bounces off the shield of darkness that flickers to life around you.");
          else
          sprintf(buf, "bounces off the shield of darkness that flickers to life around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "bounce off the shield of darkness that flickers to life around you.");
          else
          sprintf(buf, "bounce off the shield of darkness that flickers to life around $M.");
          return str_dup(buf);
        }
      }
      else {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "deflects off an almost invisible distortion in the air around you.");
          else
          sprintf(buf, "deflects off an almost invisible distortion in the air around $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "deflect off an almost invisible distortion in the air around you.");
          else
          sprintf(buf, "deflect off an almost invisible distortion in the air around $M.");
          return str_dup(buf);
        }
      }
    }
    if (echo == ECHO_ARMORBRUISE) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "strikes your armor with bruising force.");
        else
        sprintf(buf, "strikes $S armor with bruising force.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "strike your armor with bruising force.");
        else
        sprintf(buf, "strike $S armor with bruising force.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_SHIELDBRUISE) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "strikes your shield with bruising force.");
        else
        sprintf(buf, "strikes $S shield with bruising force.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "strike your shield with bruising force.");
        else
        sprintf(buf, "strike $S shield with bruising force.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_PARRY) {
      if (get_lmelee(ch) != NULL) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is parried by your %s.", get_lmelee(ch)->short_descr);
          else
          sprintf(buf, "is parried by $S %s.", get_lmelee(ch)->short_descr);
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be parried by your %s.", get_lmelee(ch)->short_descr);
          else
          sprintf(buf, "be parried by $S %s.", get_lmelee(ch)->short_descr);
          return str_dup(buf);
        }
      }
      else if (get_smelee(ch) != NULL) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is parried by your %s.", get_smelee(ch)->short_descr);
          else
          sprintf(buf, "is parried by $S %s.", get_smelee(ch)->short_descr);
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be parried by your %s.", get_smelee(ch)->short_descr);
          else
          sprintf(buf, "be parried by $S %s.", get_smelee(ch)->short_descr);
          return str_dup(buf);
        }
      }
      else {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is parried by you.");
          else
          sprintf(buf, "is parried by $M.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be parried by you.");
          else
          sprintf(buf, "be parried by $M.");
          return str_dup(buf);
        }
      }
    }
    if (echo == ECHO_SHIELDBLOCK) {
      if (get_shield_obj(ch) != NULL) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is blocked by your %s.", get_shield_obj(ch)->short_descr);
          else
          sprintf(buf, "is blocked by $S %s.", get_shield_obj(ch)->short_descr);
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be blocked by your %s.", get_shield_obj(ch)->short_descr);
          else
          sprintf(buf, "be blocked by $S %s.", get_shield_obj(ch)->short_descr);
          return str_dup(buf);
        }
      }

      else {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "is blocked by your shield.");
          else
          sprintf(buf, "is blocked by $S shield.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "be blocked by your shield.");
          else
          sprintf(buf, "be blocked by $S shield.");
          return str_dup(buf);
        }
      }
    }
    if (echo == ECHO_COVER) {
      if (get_cover(ch) != NULL) {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "strikes the %s you're taking cover behind.", get_cover(ch)->short_descr);
          else
          sprintf(buf, "strikes the %s $E's taking cover behind.", get_cover(ch)->short_descr);
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "strike the %s you're taking cover behind.", get_cover(ch)->short_descr);
          else
          sprintf(buf, "strike the %s $E's taking cover behind.", get_cover(ch)->short_descr);
          return str_dup(buf);
        }

      }
      else {
        if (fast == TRUE) {
          if (type == TO_VICT)
          sprintf(buf, "strikes your cover.");
          else
          sprintf(buf, "strikes $S cover.");
          return str_dup(buf);
        }
        else {
          if (type == TO_VICT)
          sprintf(buf, "strike your cover.");
          else
          sprintf(buf, "strike $S cover.");
          return str_dup(buf);
        }
      }
    }
    if (echo == ECHO_PUNCHBLOCK) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "is blocked by you.");
        else
        sprintf(buf, "is blocked by $M.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "be blocked by you.");
        else
        sprintf(buf, "be blocked by $M.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_GRAPPLEBLOCK) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "is fended off by you.");
        else
        sprintf(buf, "is fended off by $M.");
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "be fended off by you.");
        else
        sprintf(buf, "be fended off by $M.");
        return str_dup(buf);
      }
    }
    if (echo == ECHO_PUNCH) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "strikes you in the %s.", punchloc);
        else
        sprintf(buf, "strikes $M in the %s.", punchloc);
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "strike you in the %s.", punchloc);
        else
        sprintf(buf, "strike $M in the %s.", punchloc);
        return str_dup(buf);
      }
    }
    if (echo == ECHO_GRAPPLE) {
      if (fast == TRUE) {
        if (type == TO_VICT)
        sprintf(buf, "strains your %s.", grappleloc);
        else
        sprintf(buf, "strains $S %s.", grappleloc);
        return str_dup(buf);
      }
      else {
        if (type == TO_VICT)
        sprintf(buf, "strain your %s.", grappleloc);
        else
        sprintf(buf, "strain $S %s.", grappleloc);
        return str_dup(buf);
      }
    }

    if (fast == TRUE) {
      if (type == TO_VICT)
      sprintf(buf, "misses you.");
      else
      sprintf(buf, "miss $M.");
      return str_dup(buf);
    }
    else {
      if (type == TO_VICT)
      sprintf(buf, "miss you.");
      else
      sprintf(buf, "miss $M.");
      return str_dup(buf);
    }
  }

  void hit_message(CHAR_DATA *ch, CHAR_DATA *victim, int discipline, int point, int shield) {
    char tochar[MSL];
    char tovict[MSL];
    char tonovict[MSL];
    if (point == -1) {
      switch (discipline) {
      case DIS_TELEKINESIS:
        sprintf(tochar, "You pick up a nearby object with your telekineis and hurl it at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n picks up a nearby object with $s telekineis and hurls it at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n pick up a nearby object with $s telekineis and hurls it at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_PISTOLS:
      case DIS_RIFLES:
      case DIS_CARBINES:
        sprintf(tochar, "You fire off a shot at $N which %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n fires off a shot at you which %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n fires off a shot at $N which %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_THROWN:
        sprintf(tochar, "You throw a small knife end over end at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n throws a small knife end over end at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n throws a small knife end over end at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_BOWS:
        sprintf(tochar, "You draw and quickly fire an arrow at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n draws and quickly fires an arrow at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n draws and quickly fires an arrow at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_ENERGY:
        sprintf(tochar, "You hurl a blast of magical energy at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n hurls a blast of magical energy at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n hurls a blast of magical energy at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_FIRE:
        sprintf(tochar, "You hurl a small fireball at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n hurls a small fireball at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n hurls a small fireball at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_ICE:
        sprintf(tochar, "You conjure and launch a half dozen spinning jagged ice shards at $N, they %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n conjures and launches a half dozen spinning jagged ice shards at you, they %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n conjures and launches a half dozen spinning jagged ice shards at $N, they %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_WEATHER:
        sprintf(tochar, "You gesture and a lightning bolt arcs from the sky towards $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n gestures and a lightning bolt arcs from the sky towards you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n gestures and a lightning bolt arcs from the sky towards $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_NATURE:
        sprintf(tochar, "You gesture and a sharp root launches from the earth towards $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n gestures and a sharp root launches from the earth towards you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n gestures and a sharp root launches from the earth towards $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_LIGHTNING:
        sprintf(tochar, "You launch a bolt of lightning at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n launches a bolt of lightning at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n launches a bolt of lightning at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_DARK:
        sprintf(tochar, "You hurl a small ball of roiling dark energy at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n hurls a small ball of roiling dark energy at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n hurls a small ball of roiling dark energy at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_PRISMATIC:
        sprintf(
        tochar, "You hurl a small ball of brilliant prismatic energy at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(
        tovict, "$n hurls a small ball of brilliant prismatic energy at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(
        tonovict, "$n hurls a small ball of brilliant prismatic energy at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_HELLFIRE:
        sprintf(tochar, "You hurl a ball of roiling black and red flame at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n hurls a ball of roiling black and red flame at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n hurls a ball of roiling black and red flame at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_SHADOWL:
        sprintf(tochar, "You launch a bolt of black lightning at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n launches a bolt of black lightning at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n launches a bolt of black lightning at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_SHOTGUNS:
        sprintf(tochar, "You fire off a quick blast at $N which %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n fires off a quick blast at you which %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n fires off a quick blast at $N which %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_TELEPATHY:
        sprintf(tochar, "You launch a psychic attack at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n launches a pyschic assault at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n concentrates and the air between $m and $N ripples slightly before the distortion %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_SONIC:
        sprintf(tochar, "You launches an ear-shattering sonic attack at $N which %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n launches an ear-shattering sonic attack at you which %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n launches and ear-shattering sonic attack at $N which %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_RADIATION:
        sprintf(tochar, "You fire a beam of radiation at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n fires a beam of radiation at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n fires a beam of radiation at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_STONE:
        sprintf(tochar, "You conjure a jagged piece of stone and send it flying at $N and it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n conjures a jagged piece of stone and sends it flying at you and it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n conjures a jagged piece of stone and sends it flying at $N and it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_STRIKING:
        sprintf(tochar, "You throw a punch at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n throws a punch at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n throws a punch at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_GRAPPLE:
        sprintf(tochar, "You wrench $N's arm in a move which %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n wrenches your arm in a move which %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n wrenches $N's arm in a move which %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_LONGBLADE:
        sprintf(tochar, "You swing your weapon at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n swings $s weapon at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n swings $s weapon at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_BLUNT:
        sprintf(tochar, "You swing your weapon at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n swings $s weapon at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n swings $s weapon at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_KNIFE:
        sprintf(tochar, "You launch a stab at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n launches a stab at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n launches a stab at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_CLAW:
        sprintf(
        tochar, "You launch a slashing rake attack at $N with your claws which %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(
        tovict, "$n launches a slashing rake attack at you with $s claws which %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n launches a slashing rake attack at $N with $s claws which %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_ENERGYF:
        sprintf(tochar, "You charge your fist with mystical energy and throw a punch at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n charges $s fist with mystical energy and throws a punch at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n charges $s fist with mystical energy and throws a punch at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_FIREF:
        sprintf(tochar, "You ignite your fist and throw a punch at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n ignites $s fist and throws a punch at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n ignites $s fist and throws a punch at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_ICEF:
        sprintf(tochar, "You conjure ice blades from your knuckles and throw a punch at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n conjures ice blades from $s knuckles and throws a punch at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n conjures ice blades from $s knuckles and throws a punch at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_LIGHTNINGF:
        sprintf(tochar, "You electrify your fist and throw a punch at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n electrifies $s fist and throws a punch at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n electrifies $s fist and throws a punch at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_DARKF:
        sprintf(tochar, "You charge your fist with dark energy and throw a punch at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n charges $s fist with dark energy and throws a punch at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(
        tonovict, "$n charges $s fist with dark energy and throws a punch at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      case DIS_BRUTE:
        sprintf(tochar, "You raise both hands and bring them down in a smashing attack at $N, it %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n raises both hands and brings them down in a smashing attack at you, it %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n raises both hands and brings them down in a smashing attack at $N, it %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      default:
        sprintf(tochar, "You launch an attack at $N which %s", defense_message(victim, TO_CHAR, TRUE, shield));
        sprintf(tovict, "$n launches an attack at you which %s", defense_message(victim, TO_VICT, TRUE, shield));
        sprintf(tonovict, "$n launches an attack at $N which %s", defense_message(victim, TO_NOTVICT, TRUE, shield));
        break;
      }
    }

    if ((!IS_NPC(victim) || !IS_NPC(ch)) && battleground(ch->in_room)) {
      op_report(logact(tonovict, ch, victim), ch);
    }

    act(tochar, ch, NULL, victim, TO_CHAR);
    act(tovict, ch, NULL, victim, TO_VICT);
    if (will_fight_show(ch, TRUE)) {
      act(tonovict, ch, NULL, victim, TO_NOTVICT);
      dact(tonovict, ch, NULL, victim, DISTANCE_MEDIUM);
    }
    else
    pact(tonovict, ch, NULL, victim, ch->x, ch->y, 25, victim->x, victim->y);
  }

  bool is_sparring(CHAR_DATA *ch) {
    if (IS_FLAG(ch->comm, COMM_SPARRING))
    return TRUE;
    if (is_sparring_room(ch->in_room))
    return TRUE;
    return FALSE;
  }

  bool is_sparring_room(ROOM_INDEX_DATA *room) {

    if (room == NULL)
    return FALSE;
    if (institute_room(room))
    return TRUE;

    if (IS_SET(room->room_flags, ROOM_SPARRING))
    return TRUE;

    return FALSE;
  }

  bool sparring_conditions(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (guestmonster(ch) || guestmonster(victim))
    return FALSE;

    if (IS_FLAG(ch->comm, COMM_SUBDUE))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_SPARRING))
    return TRUE;

    if (IS_FLAG(ch->fightflag, FIGHT_KNOCKOUT))
    return TRUE;
    if (IS_FLAG(ch->fightflag, FIGHT_KNOCKFAIL))
    return TRUE;

    if (is_sparring_room(ch->in_room))
    return TRUE;

    if (battleground(ch->in_room))
    return TRUE;

    return FALSE;
  }

  void bloodify(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int iWear;
    if (IS_NPC(ch))
    return;

    ch->pcdata->blood[0]++;

    if (is_animal(ch))
    return;

    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && (can_see_wear(ch, iWear))) {
        if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_RANGED && obj->item_type != ITEM_JEWELRY)
        continue;
        if (obj->item_type == ITEM_CLOTHING)
        obj->value[3] = UMIN(obj->value[3] + 1, 500);
        if (obj->item_type == ITEM_JEWELRY)
        obj->value[4] = UMIN(obj->value[4] + 1, 500);
        else
        obj->value[3] = UMIN(obj->value[3] + 10, 500);
      }
    }
  }

  void vicbloodify(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int iWear;
    if (IS_NPC(ch))
    return;

    ch->pcdata->blood[0] += 100;

    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && (can_see_wear(ch, iWear))) {
        if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_RANGED && obj->item_type != ITEM_JEWELRY)
        continue;
        if (obj->item_type == ITEM_CLOTHING)
        obj->value[3] =
        UMIN(obj->value[3] + (50 * ch->wounds * ch->wounds), 500);
        if (obj->item_type == ITEM_JEWELRY)
        obj->value[4] =
        UMIN(obj->value[4] + (50 * ch->wounds * ch->wounds), 500);
      }
    }
  }

  CHAR_DATA *closest_character(CHAR_DATA *ch) {
    CHAR_DATA *closest = NULL;
    int mindistance = 2000;

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *victim = *it;

      if (victim == NULL || IS_IMMORTAL(victim))
      continue;
      if (!in_fight(victim))
      continue;
      if (!same_fight(ch, victim))
      continue;
      if (victim == ch)
      continue;
      if (victim->in_room == NULL)
      continue;

      if (!IS_NPC(victim) && (is_animal(victim)))
      continue;

      if (IS_NPC(victim)) {
        if (IS_FLAG(victim->act, ACT_COVER) || IS_FLAG(victim->act, ACT_TURRET) || IS_FLAG(victim->act, ACT_COMBATOBJ))
        continue;
      }

      if (combat_distance(victim, ch, FALSE) < mindistance) {
        closest = victim;
        mindistance = combat_distance(victim, ch, FALSE);
      }
    }
    if (mindistance == 2000) {
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        CHAR_DATA *victim = *it;

        if (victim == NULL || IS_IMMORTAL(victim))
        continue;
        if (!in_fight(victim))
        continue;
        if (!same_fight(ch, victim))
        continue;
        if (victim == ch)
        continue;
        if (victim->in_room == NULL)
        continue;

        if (combat_distance(victim, ch, FALSE) < mindistance) {
          closest = victim;
          mindistance = combat_distance(victim, ch, FALSE);
        }
      }
    }
    return closest;
  }

  void lose_muscle(char *name) {
    CHAR_DATA *ch;

    if (safe_strlen(name) < 2)
    return;

    if ((ch = get_char_world_pc(name)) == NULL)
    return;

    int min = 10;
    int minpoint = -1;
    for (int i = 0; i < MAX_CONTACTS; i++) {
      if (ch->pcdata->contact_jobs[i] == CJOB_MUSCLE && safe_strlen(ch->pcdata->contact_names[i]) > 2 && safe_strlen(ch->pcdata->contact_descs[i]) > 2) {
        if (get_skill(ch, contacts_table[i]) < min) {
          min = get_skill(ch, contacts_table[i]);
          minpoint = i;
        }
      }
    }
    if (minpoint != -1) {
      ch->pcdata->contact_jobs[minpoint] = 0;
      if (ch->pcdata->spectre != 0) {
        if (get_skill(ch, SKILL_DISPOSABLEMUSCLE) > 0)
        ch->pcdata->contact_cooldowns[minpoint] = current_time + (3600 * 12);
        else
        ch->pcdata->contact_cooldowns[minpoint] = current_time + (3600 * 24);
      }
      else {
        if (get_skill(ch, SKILL_DISPOSABLEMUSCLE) > 0)
        ch->pcdata->contact_cooldowns[minpoint] = current_time + (3600 * 24);
        else
        ch->pcdata->contact_cooldowns[minpoint] =
        current_time + (3600 * 24 * 2);
      }
    }
  }

  void bigmonster_takedown(CHAR_DATA *ch) {
    char buf[MSL];

    int pccount = 0;
    int tiercount = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || is_gm(rch))
      continue;
      if (IS_NPC(rch))
      continue;
      if (!in_fight(rch))
      continue;
      if (!same_fight(rch, ch))
      continue;
      if (guestmonster(rch))
      continue;
      if (same_player(ch, rch))
      continue;

      sprintf(buf, "MONSTER: %s participating", rch->name);
      log_string(buf);

      if (rch->pcdata->account != NULL)
      rch->pcdata->account->pkarma =
      UMIN(40000 - rch->pcdata->account->pkarmaspent, rch->pcdata->account->pkarma + 1000);
      gain_exp(rch, 25000, FEXP_MONSTER);
      rch->pcdata->week_tracker[TRACK_BIGMONSTER]++;
      rch->pcdata->life_tracker[TRACK_BIGMONSTER]++;
      pccount++;
      tiercount += get_tier(rch);
    }
    if (ch->pcdata->monster_wounds >= 1) {
      int amount;
      if (ch->spentnewexp >= 2400000)
      amount = 500000;
      else if (ch->spentnewexp >= 1900000)
      amount = 400000;
      else if (ch->spentnewexp >= 1400000)
      amount = 300000;
      else
      amount = 100000;
      amount /= UMAX(1, tiercount);
      amount *= 2;
      if (pccount <= 1)
      amount = 10000;
      amount = UMIN(amount, 200000);

      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        CHAR_DATA *rch = *it;

        if (rch == NULL || is_gm(rch))
        continue;
        if (IS_NPC(rch))
        continue;
        if (!in_fight(rch))
        continue;
        if (!same_fight(rch, ch))
        continue;
        if (guestmonster(rch))
        continue;

        sprintf(buf, "MONSTER: %s cap reward", rch->name);
        log_string(buf);

        rch->pcdata->exp_cap += amount;
      }
    }
  }

  void wound_check(CHAR_DATA *ch, CHAR_DATA *victim, int wound) {

    if (IS_NPC(victim) && wound > 2)
    lair_mobkill(victim->in_room, victim->pIndexData->vnum);

    if (!IS_FLAG(ch->act, PLR_SHROUD)) {
      if (combat_distance(ch, victim, TRUE) < 2)
      bloodify(ch);
      vicbloodify(victim);
    }
    if (guestmonster(ch) && !IS_NPC(victim) && victim->spentrpexp > 1000) {
      ch->pcdata->monster_wounds++;
      if (get_tier(victim) == 1)
      give_karma(ch, 100, KARMA_MONSTER);
      else
      give_karma(ch, 300, KARMA_MONSTER);
    }
    if (guestmonster(victim)) {
      log_string("MONSTER: Hurting");
      if (victim->wounds >= 4) {
        log_string("MONSTER: Takedown.");
        bigmonster_takedown(victim);
      }
    }
    if (wound == 4 || wound == 3) {
      if (IS_NPC(victim) && victim->pIndexData->vnum == POLICE_OFFICER && !IS_NPC(ch)) {
        ch->pcdata->police_intensity++;
        ch->pcdata->police_number++;
        ch->pcdata->police_timer += 720;
      }
      if (IS_NPC(victim) && victim->pIndexData->vnum == POLICE_SWAT && !IS_NPC(ch)) {
        ch->pcdata->police_intensity++;
        ch->pcdata->police_number++;
        ch->pcdata->police_timer += 720;
      }
      if (IS_NPC(victim) && victim->pIndexData->vnum == NATIONAL_GUARD && !IS_NPC(ch)) {
        ch->pcdata->police_intensity++;
        ch->pcdata->police_number++;
        ch->pcdata->police_timer += 720;
      }
      if (!IS_NPC(victim)) {
        if (wound == 4)
        killplayer(ch, victim);
        else
        critplayer(ch, victim);
      }

      if (wound == 3 && number_percent() % 4 == 0 && !IS_NPC(victim) && !is_undead(victim)) {
        victim->pcdata->coma = current_time + (3600 * number_range(6, 45));
      }
    }
    /*
else if(wound == 2)
{
if(!IS_NPC(victim) && IS_NPC(ch) && victim->in_room != NULL && IS_FLAG(victim->act, PLR_SHROUD) && ch->controled_by == NULL)
{
if(victim->played/3600 < 20)
{
victim->wounds = 1;
victim->heal_timer = 10000;
}
REMOVE_FLAG(victim->act, PLR_SHROUD);
}
}
*/
    if (!IS_NPC(victim) && victim->pcdata->patrol_status == PATROL_WAGINGWAR) {
      shroudescape(victim);
      victim->heal_timer /= 25;
      return;
    }
    if (is_sparring(ch)) {
      victim->fighting = FALSE;
      victim->attacking = 0;
      ch->attacking = 0;
      act("$n is defeated.", victim, NULL, NULL, TO_ROOM);
      dact("$n is defeated.", victim, NULL, NULL, DISTANCE_NEAR);

      if (fight_over(victim->in_room) == TRUE) {
        end_fight(victim->in_room);
        return;
      }
      victim->heal_timer /= 25;
    }
    if (battleground(victim->in_room)) {
      defeat_op_pc(victim);
      victim->fighting = FALSE;
      victim->attacking = 0;
      if (fight_over(victim->in_room) == TRUE) {
        end_fight(victim->in_room);
      }
      act("`r$n`r fades from view.`x", victim, NULL, NULL, TO_ROOM);
      send_to_char("`rYou fade from view.`x\n\r", victim);
      char opbuf[MSL];
      sprintf(opbuf, "\n`r%s fades out of the nightmare.`x", NAME(victim));
      op_report(opbuf, NULL);
      victim->heal_timer /= 50;
      if (!IS_NPC(victim) && victim->faction != ch->faction && ch->faction != 0 && clan_lookup(ch->faction) != NULL)
      clan_lookup(ch->faction)->last_defeated = victim->faction;

      if (victim->bagcarrier == 1) {
        char buf[MSL];
        if (combat_distance(ch, victim, TRUE) <= 5 && !is_animal(ch)) {
          victim->bagcarrier = 0;
          ch->bagcarrier = 1;
          sprintf(buf, "`c$n`c has %s.`x", bag_name());
          act(buf, ch, NULL, NULL, TO_ROOM);
          op_report(logact(buf, ch, ch), ch);
          if (!IS_NPC(ch)) {
            sprintf(buf, "`CYou have %s!`x", bag_name());
            act(buf, ch, NULL, NULL, TO_CHAR);
          }
        }
        else {
          CHAR_DATA *close;
          close = closest_character(victim);
          if (close != NULL) {
            victim->bagcarrier = 0;
            close->bagcarrier = 1;
            sprintf(buf, "`c$n`c has %s.`x", bag_name());
            act(buf, close, NULL, NULL, TO_ROOM);
            op_report(logact(buf, ch, ch), ch);
            if (!IS_NPC(close)) {
              sprintf(buf, "`CYou have %s!`x", bag_name());
              act(buf, ch, NULL, NULL, TO_CHAR);
            }
          }
        }
      }
      if (ch->factiontrue > -1) {
        if (IS_NPC(victim) && ch != NULL && ch->factiontrue != victim->faction && ch->factiontrue != 0 && clan_lookup(ch->factiontrue) != NULL && victim->pIndexData->vnum == 115 && (!IS_NPC(ch) || ch->pIndexData->vnum != victim->pIndexData->vnum))
        clan_lookup(ch->factiontrue)->defeated_pcs += 2;
        else if (!IS_NPC(victim) && ch != NULL && ch->factiontrue != victim->faction && ch->factiontrue != 0 && clan_lookup(ch->factiontrue) != NULL && !IS_NPC(ch))
        clan_lookup(ch->factiontrue)->defeated_pcs++;
      }
      else {
        if (IS_NPC(victim) && ch != NULL && ch->faction != victim->faction && ch->faction != 0 && clan_lookup(ch->faction) != NULL && victim->pIndexData->vnum == 115 && (!IS_NPC(ch) || ch->pIndexData->vnum != victim->pIndexData->vnum))
        clan_lookup(ch->faction)->defeated_pcs += 2;
        else if (!IS_NPC(victim) && ch != NULL && ch->faction != victim->faction && ch->faction != 0 && clan_lookup(ch->faction) != NULL && !IS_NPC(ch))
        clan_lookup(ch->faction)->defeated_pcs++;
      }
      if (!IS_NPC(victim)) {
        reclaim_items(victim);
      }

      if (IS_NPC(victim)) {
        char_from_room(victim);
        char_to_room(victim, get_room_index(2));
        victim->wounds = 4;
        victim->ttl = 1;
      }
      else {
        wake_char(victim);
        act("You awaken.", victim, NULL, NULL, TO_CHAR);
        act("$n wakes up.", victim, NULL, NULL, TO_ROOM);
      }
      prep_process(ch, victim);
      return;
    }

    if (IS_NPC(victim) && (victim->pIndexData->vnum == MINION_TEMPLATE)) {
      victim->fighting = FALSE;
      if (fight_over(victim->in_room) == TRUE) {
        end_fight(victim->in_room);
      }
      act("$n's enchantement breaks.", victim, NULL, NULL, TO_ROOM);
      victim->heal_timer /= 25;

      char_from_room(victim);
      if (IS_NPC(victim)) {
        char_to_room(victim, get_room_index(2));
        victim->wounds = 4;
        victim->ttl = 1;
      }
      return;
    }
    if (IS_NPC(victim) && (victim->pIndexData->vnum == ALLY_TEMPLATE)) {
      victim->fighting = FALSE;
      if (fight_over(victim->in_room) == TRUE) {
        end_fight(victim->in_room);
      }
      act("$n is forced to retreat.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are forced to retreat.\n\r", victim);

      lose_muscle(victim->protecting);

      char_from_room(victim);
      if (IS_NPC(victim)) {
        char_to_room(victim, get_room_index(2));
        victim->wounds = 4;
        victim->ttl = 1;
      }
      return;
    }
    if (IS_NPC(victim) && (victim->pIndexData->vnum == TEMPLE_SOLDIER || victim->pIndexData->vnum == HAND_SOLDIER || victim->pIndexData->vnum == ORDER_SOLDIER || victim->pIndexData->vnum == TEMPLE_LT || victim->pIndexData->vnum == HAND_LT || victim->pIndexData->vnum == ORDER_LT || victim->pIndexData->vnum == 5)) {
      victim->fighting = FALSE;
      if (fight_over(victim->in_room) == TRUE) {
        end_fight(victim->in_room);
      }
      act("$n is forced to retreat.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are forced to retreat.\n\r", victim);
      victim->heal_timer /= 25;

      char_from_room(victim);
      if (IS_NPC(victim)) {
        char_to_room(victim, get_room_index(2));
        victim->wounds = 4;
        victim->ttl = 1;
      }
      return;
    }
  }

  bool in_own_home(CHAR_DATA *ch) {
    if (in_house(ch) == NULL)
    return FALSE;

    if (has_access(ch, ch->in_room))
    return TRUE;

    return FALSE;
  }

  void guard_check(CHAR_DATA *ch, CHAR_DATA *rch) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    if (IS_FLAG(rch->comm, COMM_GUARDED))
    return;
    if (!IS_FLAG(rch->comm, COMM_PACIFIST))
    return;

    if (IS_NPC(ch))
    return;

    if (is_sparring(ch))
    return;

    if (in_own_home(rch) && in_house(rch)->guards > 2) {

      if (in_house(rch)->guards == 5) {
        pMobIndex = get_mob_index(5);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, rch->in_room);
        mob->hit = max_hp(mob);
        mob->x = number_range(0, rch->in_room->size);
        mob->y = number_range(0, rch->in_room->size);
        free_string(mob->protecting);
        mob->protecting = str_dup(rch->name);
        pMobIndex = get_mob_index(5);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, rch->in_room);
        mob->hit = max_hp(mob);
        mob->x = number_range(0, rch->in_room->size);
        mob->y = number_range(0, rch->in_room->size);
        free_string(mob->protecting);
        mob->protecting = str_dup(rch->name);
        mob->fighting = TRUE;

        if (IS_FLAG(rch->act, PLR_SHROUD))
        SET_FLAG(mob->act, PLR_SHROUD);
        if (IS_FLAG(rch->act, PLR_DEEPSHROUD))
        SET_FLAG(mob->act, PLR_DEEPSHROUD);

        if (in_fight(ch)) {
          mob->fighting = TRUE;
          mob->attacking = 1;
          mob->attack_timer = FIGHT_WAIT;
          mob->move_timer = FIGHT_WAIT;
        }

        pMobIndex = get_mob_index(5);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, rch->in_room);
        mob->hit = max_hp(mob);
        mob->x = number_range(0, rch->in_room->size);
        mob->y = number_range(0, rch->in_room->size);
        free_string(mob->protecting);
        mob->protecting = str_dup(rch->name);
        if (IS_FLAG(rch->act, PLR_SHROUD))
        SET_FLAG(mob->act, PLR_SHROUD);
        if (IS_FLAG(rch->act, PLR_DEEPSHROUD))
        SET_FLAG(mob->act, PLR_DEEPSHROUD);

        if (in_fight(ch)) {
          mob->fighting = TRUE;
          mob->attacking = 1;
          mob->attack_timer = FIGHT_WAIT;
          mob->move_timer = FIGHT_WAIT;
        }

        pMobIndex = get_mob_index(5);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, rch->in_room);
        mob->hit = max_hp(mob);
        mob->x = number_range(0, rch->in_room->size);
        mob->y = number_range(0, rch->in_room->size);
        free_string(mob->protecting);
        mob->protecting = str_dup(rch->name);
        if (IS_FLAG(rch->act, PLR_SHROUD))
        SET_FLAG(mob->act, PLR_SHROUD);
        if (IS_FLAG(rch->act, PLR_DEEPSHROUD))
        SET_FLAG(mob->act, PLR_DEEPSHROUD);

        if (in_fight(ch)) {
          mob->fighting = TRUE;
          mob->attacking = 1;
          mob->attack_timer = FIGHT_WAIT;
          mob->move_timer = FIGHT_WAIT;
        }

        pMobIndex = get_mob_index(5);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, rch->in_room);
        mob->hit = max_hp(mob);
        mob->x = number_range(0, rch->in_room->size);
        mob->y = number_range(0, rch->in_room->size);
        free_string(mob->protecting);
        mob->protecting = str_dup(rch->name);
        if (IS_FLAG(rch->act, PLR_DEEPSHROUD))
        SET_FLAG(mob->act, PLR_DEEPSHROUD);

        if (in_fight(ch)) {
          mob->fighting = TRUE;
          mob->attacking = 1;
          mob->attack_timer = FIGHT_WAIT;
          mob->move_timer = FIGHT_WAIT;
        }

      }
      else if (in_house(rch)->guards > 2) {

        pMobIndex = get_mob_index(5);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, rch->in_room);
        mob->hit = max_hp(mob);
        mob->x = number_range(0, rch->in_room->size);
        mob->y = number_range(0, rch->in_room->size);
        free_string(mob->protecting);
        mob->protecting = str_dup(rch->name);
        if (IS_FLAG(rch->act, PLR_SHROUD))
        SET_FLAG(mob->act, PLR_SHROUD);

        if (in_fight(ch)) {
          mob->fighting = TRUE;
          mob->attacking = 1;
          mob->attack_timer = FIGHT_WAIT;
          mob->move_timer = FIGHT_WAIT;
        }
      }
    }
    if (!IS_FLAG(rch->comm, COMM_GUARDED))
    SET_FLAG(rch->comm, COMM_GUARDED);
  }

  bool lunacy_wolf(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (is_werewolf(ch) && is_animal(ch) && full_moon() == TRUE && (sunphase(NULL) < 2 || sunphase(NULL) == 7))
    return TRUE;
    return FALSE;
  }

  void damage(CHAR_DATA *victim, CHAR_DATA *ch, int amount) {
    if (victim->wounds >= 4)
    return;
    if (ch->wounds >= 4)
    return;

    if (victim->wounds >= 2 && victim->in_room != NULL && battleground(victim->in_room))
    return;

    if (in_fight(ch) && ch != victim && !IS_NPC(ch) && !IS_NPC(victim) && IS_FLAG(ch->comm, COMM_PACIFIST))
    REMOVE_FLAG(ch->comm, COMM_PACIFIST);

    if (amount > 0)
    guard_check(ch, victim);

    if (!IS_NPC(victim))
    victim->pcdata->timesincedamage = 0;

    if (!IS_NPC(victim) && forest_monster(ch) && (IS_FLAG(victim->comm, COMM_AFK) || victim->desc == NULL))
    ch->fight_speed = 5;

    if (!IS_NPC(ch) && ch->pcdata->patrol_status == PATROL_WAGINGWAR) {
      ch->fight_speed = 5;
      victim->fight_speed = 5;
    }
    if (!IS_NPC(victim) && victim->pcdata->patrol_status == PATROL_WAGINGWAR) {
      ch->fight_speed = 5;
      victim->fight_speed = 5;
    }

    if (guestmonster(victim)) {
      ch->fight_fast = victim->fight_fast;
      ch->fight_speed = victim->fight_speed;
    }
    else {
      victim->fight_fast = ch->fight_fast;
      victim->fight_speed = ch->fight_speed;
    }
    if (battleground(ch->in_room) && activeoperation != NULL) {
      ch->fight_speed = activeoperation->speed;
      victim->fight_speed = activeoperation->speed;
    }
    if (in_fight(ch) && in_fight(victim) && combat_distance(ch, victim, FALSE) > 1 && ch->facing != DIR_UP && ch->facing != DIR_DOWN) {
      ch->facing = roomdirection(0, 0, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y));
    }

    if (!IS_NPC(ch) && ch->rounddiminish < 100 && ch->rounddiminish > 0) {
      amount = amount * ch->rounddiminish / 100;
      ch->rounddiminish = 0;
    }

    if (ch->fighting == TRUE)
    victim->fighting = TRUE;

    if (!IS_NPC(ch) && !IS_NPC(victim) && get_skill(victim, SKILL_ARMORED) > 0 && victim->shape == SHAPE_HUMAN)
    amount /= 2;

    if (amount > 0)
    bodyguard_check(ch, victim);

    if ((forest_monster(ch) || is_invader(ch)) && !IS_NPC(victim))
    victim->pcdata->monster_beaten = 1;

    if (in_fight(victim) && victim != ch && ch != NULL && amount > 0) {
      if (victim->last_hit_by == ch)
      victim->last_hit_damage += amount;
      else {
        victim->last_hit_by = ch;
        victim->last_hit_damage = amount;
      }
    }

    if (amount > 0 && combat_distance(ch, victim, TRUE) <= 1) {
      if (has_caff(victim, CAFF_MELEED))
      reset_caff(victim, CAFF_MELEED);
      else
      apply_caff(victim, CAFF_MELEED, 1);
    }

    if (!is_sparring(ch) && same_pack(ch, victim)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * 60 * 2;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_WEAKEN;
      affect_to_char(ch, &af);
    }

    if (amount >= victim->hit) {
      if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_NPC(victim) && guestmonster(ch) && victim->pcdata->nightmare_dragged == 1) {
        act("`r$n`r fades from view.`x", victim, NULL, NULL, TO_ROOM);
        send_to_char("`rYou fade out of the nightmare.`x\n\r", victim);
        REMOVE_FLAG(victim->act, PLR_SHROUD);
        victim->fighting = FALSE;
        victim->attacking = 0;
        if (fight_over(victim->in_room) == TRUE)
        end_fight(victim->in_room);
        return;
      }
      victim->hit = 0;
      if (IS_NPC(victim) && victim->wounds == 0) {
        victim->wounds = 2;
        victim->heal_timer = 115000;
        wound_check(ch, victim, 2);
      }
      else if (victim->wounds == 0 || (victim->wounds == 1 && !lunacy_wolf(ch) && (sparring_conditions(ch, victim) || under_understanding(victim, ch)))) {
        if (victim->wounds == 1)
        victim->debuff += 50;
        victim->wounds = 1;
        victim->heal_timer = 30000;
        wound_check(ch, victim, 1);
      }
      else if (victim->wounds >= 2 && IS_NPC(victim)) {
        victim->wounds = 4;
        wound_check(ch, victim, 4);
        raw_kill(victim, ch);
      }
      else if (victim->wounds == 1 || (victim->wounds == 2 && (sparring_conditions(ch, victim) || under_limited(victim, ch) || higher_power(ch) || under_understanding(victim, ch))) || (victim->wounds > 1 && !shroud_monster(ch) && !guestmonster(victim) && get_skill(victim, SKILL_IMMORTALITY) > 1)) {
        if (!IS_NPC(victim) && !IS_NPC(ch) && is_werewolf(ch) && is_animal(ch) && full_moon() == TRUE && (sunphase(NULL) < 2 || sunphase(NULL) == 7)) {
          if (victim->wounds < 2 && number_percent() % 6 == 0 && get_tier(victim) < 3 && !is_werewolf(victim) && !is_vampire(victim) && victim->race != RACE_FACULTY && !is_institute_taught(victim)) {
            if (!IS_FLAG(victim->act, PLR_WOLFBIT))
            SET_FLAG(victim->act, PLR_WOLFBIT);
          }
        }
        if (victim->wounds == 2)
        victim->debuff += 50;
        victim->wounds = 2;
        victim->heal_timer = 115000;
        wound_check(ch, victim, 2);
        miscarriage(victim, FALSE);
      }
      else if (victim->wounds == 2 || (victim->wounds == 3 && (sparring_conditions(ch, victim) || under_limited(victim, ch) || under_understanding(victim, ch)))) {
        victim->wounds = 3;
        victim->heal_timer = 4300;
        victim->death_timer = 720;
        wound_check(ch, victim, 3);
        miscarriage(victim, FALSE);

        if (IS_FLAG(victim->fightflag, FIGHT_DEADSWITCH))
        dead_explode(victim);

      }
      else if (victim->wounds == 3 && guestmonster(ch)) {
        victim->wounds = 3;
        victim->heal_timer = 4300;
        victim->death_timer = 720;
        wound_check(ch, victim, 3);
        miscarriage(victim, FALSE);

        if (IS_FLAG(victim->fightflag, FIGHT_DEADSWITCH))
        dead_explode(victim);
      }
      else if (victim->wounds == 3) {
        if (IS_FLAG(victim->fightflag, FIGHT_DEADSWITCH))
        dead_explode(victim);

        victim->wounds = 4;
        wound_check(ch, victim, 4);
        real_kill(victim, ch);
      }
      if (victim != NULL && victim->in_room != NULL && victim->in_room->area->vnum == 18)
      victim->heal_timer /= 2;
    }
    else {
      if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_NPC(victim) && guestmonster(ch) && victim->pcdata->nightmare_dragged == 1) {
        act("`r$n`r fades from view.`x", victim, NULL, NULL, TO_ROOM);
        send_to_char("`rYou fade out of the nightmare.`x\n\r", victim);
        REMOVE_FLAG(victim->act, PLR_SHROUD);
        victim->fighting = FALSE;
        victim->attacking = 0;
        if (fight_over(victim->in_room) == TRUE)
        end_fight(victim->in_room);
        return;
      }
      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_NPC(ch) && guestmonster(victim) && ch->pcdata->nightmare_dragged == 1) {
        act("`r$n`r fades from view.`x", ch, NULL, NULL, TO_ROOM);
        send_to_char("`rYou fade out of the nightmare.`x\n\r", ch);
        REMOVE_FLAG(ch->act, PLR_SHROUD);
        ch->fighting = FALSE;
        ch->attacking = 0;
        if (fight_over(ch->in_room) == TRUE)
        end_fight(ch->in_room);
        return;
      }

      victim->hit -= amount;
    }
    add_aggro(victim, ch, amount);
  }
  bool is_peaceful(CHAR_DATA *ch) {

    if (!IS_NPC(ch)) {
      if (is_helpless(ch))
      return TRUE;
      if (is_gm(ch))
      return TRUE;
      if (ch->pcdata->autoskip == 1)
      return TRUE;
      if (is_ghost(ch))
      return TRUE;
      if (ch->attacking == 0) {
        return TRUE;
      }
      if (ch->wounds > 3) {
        return TRUE;
      }
      if (get_npc_target(ch) == NULL && ch->fight_attacks >= 2 && ch->fight_fast == TRUE) {
        // send_to_char("4", ch);

        return TRUE;
      }
      // send_to_char("5", ch);

      return FALSE;
    }
    else {
      if (ch->attacking == 0)
      return TRUE;
      if (ch->wounds > 2)
      return TRUE;
      if (get_npc_target(ch) == NULL && ch->fight_attacks >= 2)
      return TRUE;
      if (ch->race == RACE_ANIMAL)
      return TRUE;
      if (ch->race == RACE_CIVILIAN)
      return TRUE;

      return FALSE;
    }
  }

  bool fight_over(ROOM_INDEX_DATA *room) { return FALSE; }

  void reset_turns(CHAR_DATA *ch) {
    CHAR_DATA *part = next_fight_member(ch);
    ch->hadturn = FALSE;
    int count = 0;
    if (part == NULL || part == ch)
    return;
    for (; part != NULL && part != ch && count < 2000;) {
      part->hadturn = FALSE;
      part = next_fight_member(part);
      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: reset_turns");
      fight_problem++;
    }
  }
  int fight_reflexes(CHAR_DATA *ch) {
    int val = get_skill(ch, SKILL_FASTREFLEXES);
    if (get_skill(ch, SKILL_HYPERREFLEXES) > 0)
    val *= 2;
    return val;
  }
  CHAR_DATA *fastest_fighter(CHAR_DATA *ch) {
    CHAR_DATA *fastest = NULL;
    int maxspeed = -1;
    CHAR_DATA *wch = NULL;
    int count = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {
      wch = *it;

      if (wch == ch)
      continue;
      if (ch->fight_fast == FALSE && !IS_NPC(wch) && wch->pcdata->autoskip == 1)
      continue;

      if (IS_NPC(wch) && (wch->ttl < 0 || wch->race == RACE_NORMAL))
      continue;

      if (in_fight(wch) && (same_fight(ch, wch) || wch == ch)) {
        if (wch->hadturn == FALSE && fight_reflexes(wch) > maxspeed) {
          fastest = wch;
          maxspeed = fight_reflexes(wch);
        }
      }
      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: fastest_figher");
      fight_problem++;
    }

    if (fastest != NULL)
    return fastest;

    log_string("FAST1");

    count = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {
      wch = *it;

      if (wch == ch)
      continue;

      if (IS_NPC(wch) && (wch->ttl < 0 || wch->race == RACE_NORMAL))
      continue;

      if (in_fight(wch) && (same_fight(ch, wch) || wch == ch)) {
        if (wch->hadturn == FALSE && fight_reflexes(wch) > maxspeed) {
          fastest = wch;
          maxspeed = fight_reflexes(wch);
        }
      }
      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: fastest_figher");
      fight_problem++;
    }

    if (fastest != NULL)
    return fastest;
    log_string("FAST2");

    count = 0;

    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {
      wch = *it;

      if (IS_NPC(wch) && (wch->ttl < 0 || wch->race == RACE_NORMAL))
      continue;

      if (in_fight(wch) && (same_fight(ch, wch) || wch == ch)) {
        if (wch->hadturn == FALSE && fight_reflexes(wch) > maxspeed) {
          fastest = wch;
          maxspeed = fight_reflexes(wch);
        }
      }
      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: fastest_figher");
      fight_problem++;
    }

    if (fastest != NULL)
    return fastest;
    log_string("FAST3");

    count = 0;
    maxspeed = -1;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {
      wch = *it;

      if (IS_NPC(wch) && (wch->ttl < 0 || wch->race == RACE_NORMAL))
      continue;

      if (in_fight(wch) && (same_fight(ch, wch) || wch == ch)) {
        if (wch->hadturn == FALSE && fight_reflexes(wch) > maxspeed) {
          fastest = wch;
          maxspeed = fight_reflexes(wch);
        }
      }
      count++;
    }

    if (count > 1500) {
      log_string("FIGHTPROBLEM: fastest_figher");
      fight_problem++;
    }

    return fastest;
  }
  int slow_fight_count_pcs(CHAR_DATA *ch) {
    CHAR_DATA *part = next_fight_member(ch);
    int count = 0;
    if (!IS_NPC(ch))
    count++;
    int limit = 0;
    for (; part != NULL && part != ch && limit < 2000;) {
      if (!IS_NPC(part))
      count++;
      part = next_fight_member(part);
      limit++;
    }
    if (limit > 1500) {
      log_string("FIGHTPROBLEM: slow_fight_count_pcs");
      fight_problem++;
    }

    return count;
  }
  int slow_fight_count_all(CHAR_DATA *ch) {
    CHAR_DATA *part = next_fight_member(ch);
    int count = 0;
    if (ch->attacking == 1 && (!IS_NPC(ch) || !IS_FLAG(ch->act, ACT_COMBATOBJ)))
    count++;
    int limit = 0;
    for (; part != NULL && part != ch && limit < 2000;) {
      if (part->attacking == 0)
      if (!IS_NPC(part) || !IS_FLAG(part->act, ACT_COMBATOBJ))
      count++;
      part = next_fight_member(part);
      limit++;
    }
    if (limit > 1500) {
      log_string("FIGHTPROBLEM: slow_fight_count_all");
      fight_problem++;
    }

    return count;
  }
  void convert_to_fast(CHAR_DATA *ch) {
    CHAR_DATA *part = next_fight_member(ch);
    ch->fight_fast = TRUE;
    ch->fight_speed = 3;
    if (!IS_NPC(ch))
    send_to_char("`rThe fight switches to fast combat as more participants join.\n\r", ch);
    int limit = 0;
    for (; part != NULL && part != ch && limit < 2000;) {
      part->fight_fast = TRUE;
      part->fight_speed = 3;
      if (!IS_NPC(part))
      send_to_char("`rThe fight switches to fast combat as more participants join.\n\r", part);
      part = next_fight_member(part);
      limit++;
    }
    if (limit > 1500) {
      log_string("FIGHTPROBLEM: convert-to_fast");
      fight_problem++;
    }
  }
  void update_current(CHAR_DATA *ch, CHAR_DATA *newcurrent) {
    CHAR_DATA *part = next_fight_member(ch);
    ch->fight_current = newcurrent;
    int limit = 0;
    for (; part != NULL && part != ch && limit < 2000;) {
      part->fight_current = newcurrent;
      part = next_fight_member(part);
      limit++;
    }
    if (limit > 1500) {
      log_string("FIGHTPROBLEM: Update_current");
      fight_problem++;
    }
  }

  void next_attacker(CHAR_DATA *ch, bool initial) {
    if (fight_problem > 0)
    return;
    CHAR_DATA *next = ch->fight_next;
    char pclist[MSL];
    bool ochturn = ch->hadturn;
    //    if(initial == FALSE)
    //        ch->hadturn = TRUE;
    char logstring[MSL];
    if (next != NULL)
    sprintf(logstring, "FIGHT NEXT1: %s %s", ch->name, next->name);
    else
    sprintf(logstring, "FIGHT NEXT1: %s Null", ch->name);
    log_string(logstring);
    if (next != NULL && !in_fight(next))
    next = NULL;
    else if (next != NULL && !same_fight(ch, next)) {
      if (is_helpless(next) || IS_FLAG(next->act, PLR_DEAD) || next == ch)
      return;
      next = NULL;
    }

    if (next == NULL) {
      if (initial == FALSE)
      ch->hadturn = TRUE;
      next = fastest_fighter(ch);
      if (next == NULL) {
        reset_turns(ch);
        if (initial == FALSE)
        ch->hadturn = TRUE;
        next = fastest_fighter(ch);
      }
      if (next == NULL) {
        ch->attacking = 0;
        send_to_char("`rThe fight ends.\n\r", ch);
        return;
      }
    }
    ch->hadturn = ochturn;

    if (next != NULL)
    sprintf(logstring, "FIGHT NEXT2: %s %s", ch->name, next->name);
    else
    sprintf(logstring, "FIGHT NEXT2: %s Null", ch->name);
    log_string(logstring);

    CHAR_DATA *part = ch;
    int count = 0;
    int atcount = 0;
    int pccount = 0;
    sprintf(pclist, "FIGHTPCLIST:");

    if (!IS_NPC(part) || !IS_FLAG(part->act, ACT_COMBATOBJ)) {
      if (!is_gm(part)) {
        if (!IS_NPC(part) && !is_gm(part)) {
          pccount++;
          sprintf(pclist, "%s %s", pclist, part->name);
        }
        if (part->attacking == 1 && !is_helpless(part))
        atcount++;
        count++;
      }
    }
    part = next_fight_member(ch);
    if (part == NULL) {
      if (pccount > 1 && atcount == 1)
      part = ch;
      else {
        ch->attacking = 0;
        send_to_char("`rThe fight ends..\n\r", ch);
      }
    }
    if (part != NULL)
    sprintf(logstring, "FIGHT Part: %s %s", ch->name, part->name);
    else
    sprintf(logstring, "FIGHT Part: %s Null", ch->name);
    log_string(logstring);

    int limit = 0;
    if (part != ch && part != NULL && in_fight(ch) && ch->attacking != 0) {
      for (; part != NULL && part != ch && limit < 2000;) {
        if (!IS_NPC(part) || !IS_FLAG(part->act, ACT_COMBATOBJ)) {
          if (part->attacking == 1 && !is_gm(part) && !is_helpless(part)) {
            atcount++;
            if (!IS_NPC(part)) {
              sprintf(pclist, "%s %s", pclist, part->name);
              pccount++;
              log_string(pclist);
            }
            part->fight_fast = FALSE;
          }
          count++;
        }
        limit++;
        part = next_fight_member(part);
      }
      if (limit > 1500) {
        log_string("FIGHTPROBLEM: Next_attacker");
        fight_problem++;
      }
    }
    ch->fight_fast = FALSE;
    if (pccount >= 5 && pccount < 100) {
      sprintf(pclist, "%s, %d", pclist, pccount);
      log_string(pclist);
      convert_to_fast(ch);
      return;
    }
    else if (limit > 1500) {
      sprintf(pclist, "%s, %d", pclist, pccount);
      log_string(pclist);
    }
    sprintf(logstring, "FIGHT: Count %d, limit %d, atcount %d, pccount %d\n\r", count, limit, atcount, pccount);
    log_string(logstring);

    if (in_fight(ch) && ch->attacking != 0) {
      if (count < 2 || atcount < 1 || pccount < 1) {
        ch->attacking = 0;
        send_to_char("`rThe fight ends..\n\r", ch);
        return;
      }
      next->fight_fast = ch->fight_fast;
    }
    update_current(ch, next);

    //    bool hadturn = next->hadturn;
    next->hadturn = TRUE;
    next->tsincemove = 0;
    CHAR_DATA *after;
    after = fastest_fighter(next);

    if (after != NULL)
    sprintf(logstring, "FIGHT After: %s %s", ch->name, after->name);
    else
    sprintf(logstring, "FIGHT After: %s Null", ch->name);
    log_string(logstring);

    if (after == NULL) {
      reset_turns(next);
      next->hadturn = TRUE;
      after = fastest_fighter(next);
      next->hadturn = FALSE;
    }
    else
    next->hadturn = TRUE;

    if (after != NULL)
    next->fight_next = after;

    if (after != NULL)
    sprintf(logstring, "FIGHT After2: %s %s", ch->name, after->name);
    else
    sprintf(logstring, "FIGHT After2: %s Null", ch->name);
    log_string(logstring);

    if (!IS_NPC(next) && next->desc == NULL) {
      act("$n is unable to act.", next, NULL, NULL, TO_ROOM);
      next_attacker(next, FALSE);
      return;
    }

    if (next->ability_timer > 0) {
      next->ability_timer -= FIGHT_WAIT;
      if (next->ability_timer <= 0)
      send_to_char("You can use an ability again.\n\r", ch);
    }
    if (IS_NPC(next) && IS_FLAG(next->act, ACT_COMBATOBJ)) {
      next->attack_timer -= (FIGHT_WAIT * fight_speed(next));
      CHAR_DATA *temp = next;
      next_attacker(next, FALSE);
      if (temp->attack_timer <= 0) {
        if (temp->pIndexData->vnum == COBJ_FRAG)
        explode_grenade(temp);

        temp->wounds = 4;
        char_from_room(temp);
        char_to_room(temp, get_room_index(ROOM_VNUM_LIMBO));
        temp->ttl = 2;
      }
      return;
    }

    if ((IS_FLAG(next->fightflag, FIGHT_NOMOVE) && IS_FLAG(next->fightflag, FIGHT_NOATTACK)) || has_caff(next, CAFF_STUNNED)) {
      REMOVE_FLAG(next->fightflag, FIGHT_NOMOVE);
      REMOVE_FLAG(next->fightflag, FIGHT_NOATTACK);

      for (int i = 0; i < 30; i++) {
        if (next->caff_duration[i] > 0) {
          next->caff_duration[i] -= (FIGHT_WAIT * fight_speed(next) * 3 / 2);
          if (next->caff_duration[i] <= 0 && next->caff[i] == CAFF_RESISTPAIN)
          next->hit = UMAX(0, next->hit - 100);
        }
      }
      act("$n is not able to act.", next, NULL, NULL, TO_ROOM);
      next_attacker(next, FALSE);
      return;
    }
    if (!IS_NPC(next) && is_helpless(next)) {
      next->attacking = 0;
      act("$n is unable to act.", next, NULL, NULL, TO_ROOM);
      next_attacker(next, FALSE);
      return;
    }
    if (is_gm(next)) {
      next->attacking = 0;
      next_attacker(next, FALSE);
      return;
    }

    if (IS_NPC(next) && next->controled_by != NULL) {
      next->actiontimer = 120;
      next->moving = FALSE;
      next->attackdam = 0;
      next->doneabil = FALSE;

      printf_to_char(
      next->controled_by, "`RIt is %s turn to attack, you have two minutes to input your attack and moves, then five minutes after that to write the emote.\n\r`x", next->name);
      act("It is $n's turn to attack.", next, NULL, NULL, TO_ROOM);
      return;
    }

    if (IS_NPC(next)) {
      next->actiontimer = 60;
      next->attack_timer = 0;
      next->move_timer = 0;
      npc_combat_move(next);
      npc_combat_attack(next);
      next_attacker(next, FALSE);
      return;
    }

    sprintf(logstring, "FIGHT: %s taking turn from %s.", next->name, ch->name);
    log_string(logstring);
    if (IS_FLAG(ch->fightflag, FIGHT_NOMOVE) && IS_FLAG(ch->fightflag, FIGHT_NOATTACK)) {
      next->actiontimer = 600;
      next->moving = FALSE;
      next->attackdam = 0;
      next->doneabil = FALSE;
      round_process(next);
      send_to_char("`R\nIt is your turn to attack, you have five minutes to write your emote. Or you can type attack skip to skip your turn and possibly end the fight.`x\n\r", next);
      act("It is $n's turn to attack.", next, NULL, NULL, TO_ROOM);

    }
    else {
      next->actiontimer = 120;
      next->moving = FALSE;
      next->attackdam = 0;
      next->doneabil = FALSE;
      round_process(next);
      send_to_char("`R\nIt is your turn to attack, you have two minutes to input your attack and moves, then five minutes after that to write the emote for them. Or you can type attack skip to skip your turn and possibly end the fight.`x\n\r", next);
      act("It is $n's turn to attack.", next, NULL, NULL, TO_ROOM);
    }
  }

  void process_gmemote(CHAR_DATA *ch) {
    int i, point = 0, opoint = 0;
    char buf[MSL];
    CHAR_DATA *mon;

    if (ch->in_room == NULL)
    return;

    mon = ch->fight_current;

    if (mon == NULL)
    return;

    if (mon->attackdam == 0 && mon->moving == FALSE)
    return;

    int origx = mon->x;
    int origy = mon->y;
    ROOM_INDEX_DATA *origroom = mon->in_room;

    CHAR_DATA *victim = mon->chattacking;
    if (mon->attackdam > 0) {
      combat_damage(victim, mon, mon->attackdam, mon->attackdisc);
      death_cause(ch, victim, mon->attackdisc);
      apply_stagger(ch, victim, mon->attackdam, mon->attackdisc);

      for (i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == mon->attackdisc)
        point = i;
      }
      for (i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == mon->defensedisc)
        opoint = i;
      }
      sprintf(buf, "(`W$n attacks $N's %s with %s`x)", discipline_table[opoint].name, discipline_table[point].name);
      act(buf, mon, NULL, victim, TO_NOTVICT);
      sprintf(buf, "(`W$n attacks your %s with %s for %d damage`x)\n\r", discipline_table[opoint].name, discipline_table[point].name, mon->attackdam);
      act(buf, mon, NULL, victim, TO_VICT);
      if (mon->controled_by != NULL)
      printf_to_char(mon->controled_by, "(`WFor %d damage`x)\n\r", mon->attackdam);
    }
    if (mon->moving == TRUE) {
      move_message(mon, MOVE_MOVE, origx, origy, origroom);
    }
    if (IS_FLAG(mon->fightflag, FIGHT_NOMOVE))
    REMOVE_FLAG(mon->fightflag, FIGHT_NOMOVE);
    if (IS_FLAG(mon->fightflag, FIGHT_NOATTACK))
    REMOVE_FLAG(mon->fightflag, FIGHT_NOATTACK);

    if (has_caff(mon, CAFF_STUNNED))
    lower_caff(mon, CAFF_STUNNED);
    if (has_caff(mon, CAFF_ROOT))
    lower_caff(mon, CAFF_ROOT);

    next_attacker(ch, FALSE);
  }

  char *special_message_emote(CHAR_DATA *ch, int point) {
    int special = ch->pcdata->customstats[point][0];

    if (special < 0 || special > 30)
    return "";

    if (special == SPECIAL_STUN) {
      return " (STUN)";
    }
    if (special == SPECIAL_KNOCKBACK)
    return " (Knockback)";
    if (special == SPECIAL_ENTRAP)
    return " (Entrap)";
    if (special == SPECIAL_GRAPPLE)
    return " (Grapple)";
    if (special == SPECIAL_LIFESTEAL)
    return " (Lifesteal)";
    if (special == SPECIAL_OVERPOWER)
    return " (Overpower)";
    if (special == SPECIAL_DAMAGE)
    return " (Damage)";
    if (special == SPECIAL_WEAKEN)
    return " (Weaken)";
    if (special == SPECIAL_UNDERPOWER)
    return " (Underpower)";
    if (special == SPECIAL_LONGWEAKEN)
    return " (Longweaken)";
    if (special == SPECIAL_SLOW)
    return " (SLOW)";
    if (special == SPECIAL_AOE)
    return " (AOE)";

    return "";
  }

  void process_emoteattack(CHAR_DATA *ch) {
    int i, point = 0, opoint = 0;
    char buf[MSL];

    if (!in_fight(ch))
    return;

    if (ch->in_room == NULL)
    return;

    if (ch->fight_fast == TRUE)
    return;

    if (ch->fight_current == NULL)
    return;

    ROOM_INDEX_DATA *room = ch->in_room;

    if (ch->fight_current != ch && ch->fight_current != NULL) {
      if (!is_gm(ch) || ch->fight_current->controled_by != ch)
      return;
      else {
        process_gmemote(ch);
        return;
      }
    }
    CHAR_DATA *victim = ch->chattacking;
    int throwaway = dam_caff_mod(ch, victim, ch->attackdam, ch->attackdisc, TRUE);
    throwaway += 1;

    int stag = apply_stagger(ch, victim, ch->attackdam, ch->attackdisc);
    int display;
    if (ch->shadowdisc != 0)
    display = ch->shadowdisc;
    else
    display = ch->attackdisc;
    int echo = -1;
    if (victim != NULL && (ch->attackdam > 0 || stag > 0)) {

      if (ch->attackdam > 0 || stag > 0) {
        echo =
        damage_echo(ch, victim, discipline_table[point].vnum, display, ch->attackdam, combat_distance(ch, victim, TRUE), FALSE);
        combat_damage(victim, ch, ch->attackdam, ch->attackdisc);
        death_cause(ch, victim, ch->attackdisc);

        for (i = 0; i < DIS_USED; i++) {
          if (discipline_table[i].vnum == display)
          point = i;
        }
        for (i = 0; i < DIS_USED; i++) {
          if (discipline_table[i].vnum == ch->defensedisc)
          opoint = i;
        }

        if (!IS_NPC(ch))
        gain_exp(ch, xp_per_attack(ch, victim, discipline_table[point].vnum, ch->attackdam), FEXP_ATTACK);
      }
      if (echo == -1)
      echo =
      damage_echo(ch, victim, discipline_table[point].vnum, display, ch->attackdam, combat_distance(ch, victim, TRUE), TRUE);

      if (ch->attackdam <= 0) {
        if (IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER))
        REMOVE_FLAG(ch->fightflag, FIGHT_UNDERPOWER);
      }

      if (ch->attackdam > 0 && victim->hit <= 0) {
        sprintf(buf, "[$n's best %s attack against $N %s`x]", disc_name(ch, point), defense_message(victim, TO_NOTVICT, TRUE, echo));
        act(buf, trickcheck(ch), NULL, victim, TO_NOTVICT);
        dact(buf, trickcheck(ch), NULL, victim, DISTANCE_MEDIUM);
        sprintf(buf, "[$n's best %s attack against you %s`x Reducing defenses by `r%d`x, balance by `c%d`x%s]", disc_name(ch, point), defense_message(victim, TO_VICT, TRUE, echo), ch->attackdam, stag, special_message_emote(ch, ch->specialpoint));
        act(buf, trickcheck(ch), NULL, victim, TO_VICT);
        sprintf(buf, "[Your best %s attack against $N %s`x Reducing defenses by `r%d`x, balance by `c%d`x%s]", disc_name(ch, point), defense_message(victim, TO_CHAR, TRUE, echo), ch->attackdam, stag, special_message_emote(ch, ch->specialpoint));
        act(buf, trickcheck(ch), NULL, victim, TO_CHAR);
      }
      else {
        if (!IS_FLAG(ch->fightflag, FIGHT_KNOCKOUT) && !IS_FLAG(ch->fightflag, FIGHT_KNOCKFAIL)) {
          if (!IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR) || combat_distance(ch, victim, TRUE) >= 50) {
            sprintf(buf, "[$n's best %s attack against $N %s`x]", disc_name(ch, point), defense_message(victim, TO_NOTVICT, TRUE, echo));
            act(buf, trickcheck(ch), NULL, victim, TO_NOTVICT);
            dact(buf, trickcheck(ch), NULL, victim, DISTANCE_MEDIUM);
          }
          sprintf(buf, "[Your best %s attack against $N %s`x Reducing defenses by `r%d`x, balance by `c%d`x%s.]", disc_name(ch, point), defense_message(victim, TO_CHAR, TRUE, echo), ch->attackdam, stag, special_message_emote(ch, ch->specialpoint));
          act(buf, trickcheck(ch), NULL, victim, TO_CHAR);
          if (!IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR) || combat_distance(ch, victim, TRUE) >= 50) {
            sprintf(buf, "[$n's best %s attack against you %s`x Reducing defenses by `r%d`x, balance by `c%d`x%s.]", disc_name(ch, point), defense_message(victim, TO_VICT, TRUE, echo), ch->attackdam, stag, special_message_emote(ch, ch->specialpoint));
            act(buf, trickcheck(ch), NULL, victim, TO_VICT);
          }
          else {
            sprintf(buf, "(`WYour %s is attacked silently, lowering defenses by `r%d`x, balance by `c%d`x%s`x)\n\r", discipline_table[opoint].name, ch->attackdam, stag, special_message_emote(ch, ch->specialpoint));
            act(buf, trickcheck(ch), NULL, victim, TO_VICT);
          }
          if (ch->controled_by != NULL)
          printf_to_char(ch->controled_by, "(`WLowering defenses by `r%d`x.`x)\n\r", ch->attackdam);
          if (victim->controled_by != NULL)
          printf_to_char(victim->controled_by, "(`WLowering defenses by `r%d`x`x)\n\r", ch->attackdam);
        }
      }
    }
    if (IS_FLAG(ch->fightflag, FIGHT_KNOCKOUT) && victim != NULL) {
      REMOVE_FLAG(ch->fightflag, FIGHT_KNOCKOUT);
      act("(You knock $N out.)", ch, NULL, victim, TO_CHAR);
      act("($n knocks you out.)", trickcheck(ch), NULL, victim, TO_VICT);
      act("($n knocks $N out.)", trickcheck(ch), NULL, victim, TO_NOTVICT);
      dact("($n knocks $N out.)", trickcheck(ch), NULL, victim, DISTANCE_MEDIUM);

      victim->pcdata->sleeping = 400;
      if (IS_FLAG(victim->fightflag, FIGHT_DEADSWITCH))
      dead_explode(victim);
      victim->fighting = FALSE;
      victim->attacking = 0;
      if (fight_over(room)) {
        end_fight(room);
        act("The fight ends.", ch, NULL, NULL, TO_CHAR);
        act("The fight ends.", ch, NULL, NULL, TO_ROOM);
        return;
      }

    }
    else if (IS_FLAG(ch->fightflag, FIGHT_KNOCKFAIL) && victim != NULL) {
      REMOVE_FLAG(ch->fightflag, FIGHT_KNOCKFAIL);
      act("(You attempt to knock $N out.)", ch, NULL, victim, TO_CHAR);
      act("($n attempts to knock you out.)", trickcheck(ch), NULL, victim, TO_VICT);
      act("($n attempts to knock $N out.)", trickcheck(ch), NULL, victim, TO_NOTVICT);
      dact("($n attempts to knock $N out.)", trickcheck(ch), NULL, victim, DISTANCE_MEDIUM);

    }
    else if (IS_FLAG(ch->fightflag, FIGHT_DEFEND)) {
      act("(You defend)", ch, NULL, victim, TO_CHAR);
      act("($n defends.)", trickcheck(ch), NULL, victim, TO_VICT);
      act("($n defends.)", trickcheck(ch), NULL, victim, TO_NOTVICT);
      dact("($n defends.)", trickcheck(ch), NULL, victim, DISTANCE_MEDIUM);
    }

    delayattacks(ch, ch->chattacking);

    if (!IS_NPC(ch)) {
      ch->pcdata->fatigue += HIT_FATIGUE;
    }

    if (IS_FLAG(ch->fightflag, FIGHT_NOMOVE))
    REMOVE_FLAG(ch->fightflag, FIGHT_NOMOVE);
    if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK))
    REMOVE_FLAG(ch->fightflag, FIGHT_NOATTACK);

    if (has_caff(ch, CAFF_STUNNED))
    lower_caff(ch, CAFF_STUNNED);
    if (has_caff(ch, CAFF_ROOT))
    lower_caff(ch, CAFF_ROOT);

    for (int i = 0; i < 30; i++) {
      if (ch->caff_duration[i] > 0) {
        ch->caff_duration[i] -= (FIGHT_WAIT * fight_speed(ch) * 3 / 2);
        if (ch->caff_duration[i] <= 0 && ch->caff[i] == CAFF_RESISTPAIN)
        ch->hit = UMAX(0, ch->hit - 100);
      }
    }
    next_attacker(ch, FALSE);
    ch->chattacking = NULL;
    ch->attackdam = 0;
    ch->attackdisc = 0;
    ch->shadowdisc = 0;
  }

  void move_towards(CHAR_DATA *ch, int tox, int toy, int dist, int z, bool voluntary) {
    if (IS_NPC(ch)) {
      if (IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET))
      return;
    }
    if (has_caff(ch, CAFF_FEAR) && ch->afraid_of != NULL && ch->afraid_of->in_room != NULL && ch->afraid_of->in_room == ch->in_room) {
      remove_caff(ch, CAFF_FEAR);
      act("You are gripped by a sudden fear.", ch, NULL, NULL, TO_CHAR);
      move_away(ch, relative_x(ch, ch->afraid_of->in_room, ch->afraid_of->x), relative_y(ch, ch->afraid_of->in_room, ch->afraid_of->y), get_speed(ch), 0, voluntary);
      return;
    }

    if (dist < 0) {
      move_away(ch, tox, toy, dist * -1, z, voluntary);
      return;
    }

    dist = move_caff_mod(ch, dist);
    if (dist <= 0)
    return;

    int proportion;
    int xmove, ymove;

    if (is_in_cover(ch)) {
      if (has_caff(get_cover(ch), CAFF_SUPPRESSED)) {
        act("You are shot as you leave cover.\n\r", ch, NULL, NULL, TO_CHAR);
        ch->hit -= (max_hp(ch) / 10);
      }
    }
    int totaldist = (tox) * (tox) + (toy) * (toy);
    totaldist = UMAX(totaldist, 1);
    totaldist = (int)(sqrt((double)totaldist));

    int origx = ch->x;
    int origy = ch->y;
    ch->facing = roomdirection(0, 0, tox, toy);
    ch->run_dir = ch->facing;
    ROOM_INDEX_DATA *origroom = ch->in_room;
    if (dist >= totaldist) {
      move_relative(ch, tox, toy, z);
    }
    else {
      proportion = 10000 * dist / totaldist;
      ymove = (int)((toy)*proportion / 10000);
      xmove = (int)((tox)*proportion / 10000);
      move_relative(ch, xmove, ymove, z);
    }

    ch->moved += UMIN(dist, totaldist);

    if (ch->x < 0)
    ch->x = 0;
    if (ch->y < 0)
    ch->y = 0;
    if (ch->x > ch->in_room->size)
    ch->x = ch->in_room->size;
    if (ch->y > ch->in_room->size)
    ch->y = ch->in_room->size;

    CHAR_DATA *cobj;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      cobj = *it;

      if (cobj == NULL)
      continue;

      if (cobj->in_room == NULL)
      continue;
      if (!IS_NPC(cobj))
      continue;

      if (!IS_FLAG(cobj->act, ACT_COMBATOBJ))
      continue;

      if (cobj->pIndexData->vnum == COBJ_CALTROPS && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 10) {
        cobj->attack_timer =
        UMAX(1, cobj->attack_timer - (2 * FIGHT_WAIT * fight_speed(ch)));
        apply_caff(ch, CAFF_SLOW, 2);
        act("You step on some caltrops.", ch, NULL, NULL, TO_CHAR);
      }
      if (cobj->pIndexData->vnum == COBJ_QUICKSAND && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 10) {
        apply_caff(ch, CAFF_SLOW, 1);
        act("You step in some quicksand.", ch, NULL, NULL, TO_CHAR);
      }

      if (cobj->pIndexData->vnum == COBJ_LANDMINE && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 5) {
        cobj->attack_timer = 0;
        combat_damage(ch, ch, max_hp(ch) / 2, DIS_FIRE);
        act("You step on a landmine!", ch, NULL, NULL, TO_CHAR);
        act("$n steps on a landmine!", ch, NULL, NULL, TO_ROOM);
      }
      if (cobj->pIndexData->vnum == COBJ_TEAR && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 15) {
        if (!has_gasmask(ch) && !is_undead(ch)) {
          apply_caff(ch, CAFF_TEAR, 1);
          act("You breathe in a lungful of teargas.", ch, NULL, NULL, TO_CHAR);
        }
      }
    }

    if (has_caff(ch, CAFF_CALTROPING)) {
      send_to_char("You spread the caltrops.", ch);
      remove_caff(ch, CAFF_CALTROPING);
      summon_cobj(origroom, COBJ_CALTROPS, 20, origx, origy, ch);
    }
    if (has_caff(ch, CAFF_LANDMINING) && dist > 6) {
      send_to_char("You plant the landmine.", ch);
      remove_caff(ch, CAFF_LANDMINING);
      summon_cobj(origroom, COBJ_LANDMINE, 20, origx, origy, ch);
    }

    if (voluntary == TRUE)
    ch->move_timer = FIGHT_WAIT * fight_speed(ch);
  }
  void move_away(CHAR_DATA *ch, int tox, int toy, int dist, int z, bool voluntary) {
    if (IS_NPC(ch)) {
      if (IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET))
      return;
    }

    if (has_caff(ch, CAFF_FEAR) && ch->afraid_of != NULL && ch->afraid_of->in_room != NULL && ch->afraid_of->in_room == ch->in_room) {
      remove_caff(ch, CAFF_FEAR);
      act("You are gripped by a sudden fear.", ch, NULL, NULL, TO_CHAR);
      move_away(ch, relative_x(ch, ch->afraid_of->in_room, ch->afraid_of->x), relative_y(ch, ch->afraid_of->in_room, ch->afraid_of->y), get_speed(ch), 0, voluntary);
      return;
    }

    if (dist < 0) {
      move_towards(ch, tox, toy, dist * -1, z, voluntary);
      return;
    }
    dist = move_caff_mod(ch, dist);
    if (dist <= 0)
    return;

    if (tox == 0) {
      tox++;
    }
    if (toy == 0) {
      toy++;
    }
    tox = tox * -50;
    toy = toy * -50;

    move_towards(ch, tox, toy, dist, z, voluntary);
    return;

    int fromx = ch->x;
    int fromy = ch->y;
    int proportion;
    int xmove, ymove;
    ROOM_INDEX_DATA *origroom = ch->in_room;
    if (is_in_cover(ch)) {
      if (has_caff(get_cover(ch), CAFF_SUPPRESSED)) {
        act("You are shot as you leave cover.\n\r", ch, NULL, NULL, TO_CHAR);
        ch->hit -= (max_hp(ch) / 10);
      }
    }

    int origx = ch->x;
    int origy = ch->y;

    if (tox == 0 && toy == 0)
    tox += 1;
    int totaldist = (tox) * (tox) + (toy) * (toy);
    totaldist = UMAX(totaldist, 1);
    ch->facing = roomdirection(0, 0, tox, toy);
    ch->run_dir = ch->facing;

    totaldist = (int)(sqrt((double)totaldist));
    proportion = 100 * dist / totaldist;
    ymove = (int)((toy - fromy) * proportion / 100);
    xmove = (int)((tox - fromx) * proportion / 100);
    move_relative(ch, xmove, ymove, z);

    if (ch->x < 0)
    ch->x = 0;
    if (ch->y < 0)
    ch->y = 0;
    if (ch->x > ch->in_room->size)
    ch->x = ch->in_room->size;
    if (ch->y > ch->in_room->size)
    ch->y = ch->in_room->size;

    ch->moved += UMIN(dist, totaldist);

    CHAR_DATA *cobj;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      cobj = *it;

      if (cobj == NULL)
      continue;

      if (cobj->in_room == NULL)
      continue;
      if (!IS_NPC(cobj))
      continue;

      if (!IS_FLAG(cobj->act, ACT_COMBATOBJ))
      continue;

      if (cobj->pIndexData->vnum == COBJ_CALTROPS && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 10) {
        cobj->attack_timer =
        UMAX(1, cobj->attack_timer - (2 * FIGHT_WAIT * fight_speed(ch)));
        apply_caff(ch, CAFF_SLOW, 2);
        act("You step on some caltrops.", ch, NULL, NULL, TO_CHAR);
      }
      if (cobj->pIndexData->vnum == COBJ_QUICKSAND && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 10) {
        apply_caff(ch, CAFF_SLOW, 1);
        act("You step in some quicksand.", ch, NULL, NULL, TO_CHAR);
      }
      if (cobj->pIndexData->vnum == COBJ_LANDMINE && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 5) {
        cobj->attack_timer = 0;
        combat_damage(ch, ch, max_hp(ch) / 2, DIS_FIRE);
        act("You step on a landmine!", ch, NULL, NULL, TO_CHAR);
        act("$n steps on a landmine!", ch, NULL, NULL, TO_ROOM);
      }
      if (cobj->pIndexData->vnum == COBJ_TEAR && get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 15) {
        if (!has_gasmask(ch) && !is_undead(ch)) {
          apply_caff(ch, CAFF_TEAR, 1);
          act("You breathe in a lungful of teargas.", ch, NULL, NULL, TO_CHAR);
        }
      }
    }

    if (has_caff(ch, CAFF_CALTROPING)) {
      remove_caff(ch, CAFF_CALTROPING);
      summon_cobj(origroom, COBJ_CALTROPS, 20, origx, origy, ch);
    }
    if (has_caff(ch, CAFF_LANDMINING) && dist > 6) {
      remove_caff(ch, CAFF_LANDMINING);
      summon_cobj(origroom, COBJ_LANDMINE, 20, origx, origy, ch);
    }

    if (voluntary == TRUE)
    ch->move_timer = FIGHT_WAIT * fight_speed(ch);
  }

  int get_speed(CHAR_DATA *ch) {
    int val = 0;
    if (IS_NPC(ch)) {
      if (ch->pIndexData->intelligence == 0)
      val = 10;
      else
      val = ch->pIndexData->intelligence;
    }
    else
    val = 10 + get_skill(ch, SKILL_RUNNING) * 2;

    if (!is_animal(ch) && !is_ghost(ch)) {
      if (ch->sex == SEX_FEMALE) {
        if (get_height_mod(ch) >= 2)
        val -= 5;
        else if (get_height_mod(ch) >= 1)
        val -= 2;
      }
      else {
        if (get_height_mod(ch) >= 2)
        val -= 2;
        else if (get_height_mod(ch) >= 1)
        val -= 1;
      }
    }
    if (guestmonster(ch) || higher_power(ch)) {
      int minval = 2;
      if (ch->pcdata->height_feet >= 10)
      minval = val / 2;
      else
      minval = val * 25 / 100;
      if (ch->debuff > 0)
      val = UMAX(2, val - (ch->debuff / 15));
      if (val < minval)
      val = minval;
      if (in_water(ch))
      val /= 2;
    }
    else {
      if (ch->debuff > 0)
      val = UMAX(2, val - (ch->debuff / 15));
    }

    val = val * get_lifeforce(ch, TRUE, NULL) / 100;
    if (IS_AFFECTED(ch, AFF_ARTHRITIS))
    val /= 2;

    if (ch->bagcarrier == 1)
    val = UMIN(val, 10 + get_skill(ch, SKILL_STAMINA));

    if (IS_FLAG(ch->fightflag, FIGHT_SLOW1) || IS_FLAG(ch->fightflag, FIGHT_SLOW2))
    val = UMIN(val / 2, 10);

    if (in_water(ch) && !is_swimmer(ch))
    val = UMIN(val / 2, 10);

    if (has_caff(ch, CAFF_GUST))
    val = val * 3 / 5;

    return val;
  }

  int get_dist(int xone, int yone, int xtwo, int ytwo) {
    int totalx, totaly;

    if (xone > xtwo)
    totalx = xone - xtwo;
    else
    totalx = xtwo - xone;

    if (yone > ytwo)
    totaly = yone - ytwo;
    else
    totaly = ytwo - yone;

    int total = totalx * totalx + totaly * totaly;
    if (total == 0)
    return 1;

    total = (int)sqrt((double)total);
    return UMAX(total, 1);
  }

  void move_message(CHAR_DATA *ch, int type, int originalx, int originaly, ROOM_INDEX_DATA *origroom) {
    if (ch->fight_fast == FALSE) {
      if (type == MOVE_MOVE)
      send_to_char("You will move.\n\r", ch);
      else if (type == MOVE_CHARGE)
      send_to_char("You will charge.\n\r", ch);
      else if (type == MOVE_JUMP)
      send_to_char("You will jump.\n\r", ch);
      else if (type == MOVE_RETREAT)
      send_to_char("You will retreat.\n\r", ch);
      else if (type == MOVE_FLY)
      send_to_char("You will fly.\n\r", ch);
      else if (type == MOVE_FLEE)
      send_to_char("You will sprint.\n\r", ch);
      else if (type == MOVE_PROTECT)
      send_to_char("You will move protectively.\n\r", ch);
      else if (type == MOVE_UP)
      send_to_char("You will move up.\n\r", ch);
      else if (type == MOVE_DOWN)
      send_to_char("You will move down.\n\r", ch);
      else if (type == MOVE_PATH)
      send_to_char("You will path.\n\r", ch);
    }
    else {
      if (type == MOVE_MOVE)
      send_to_char("You move.\n\r", ch);
      else if (type == MOVE_CHARGE)
      send_to_char("You charge.\n\r", ch);
      else if (type == MOVE_JUMP)
      send_to_char("You jump.\n\r", ch);
      else if (type == MOVE_RETREAT)
      send_to_char("You retreat.\n\r", ch);
      else if (type == MOVE_FLY)
      send_to_char("You fly.\n\r", ch);
      else if (type == MOVE_FLEE)
      send_to_char("You sprint.\n\r", ch);
      else if (type == MOVE_PROTECT)
      send_to_char("You move protectively.\n\r", ch);
      else if (type == MOVE_UP)
      send_to_char("You move up.\n\r", ch);
      else if (type == MOVE_DOWN)
      send_to_char("You move down.\n\r", ch);
      else if (type == MOVE_PATH)
      send_to_char("You path.\n\r", ch);
    }

    int finalx = ch->x;
    int finaly = ch->y;
    ROOM_INDEX_DATA *finalroom = ch->in_room;
    CHAR_DATA *rch;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d->connected != CON_PLAYING)
      continue;
      rch = CH(d);
      if (rch == NULL || IS_NPC(rch))
      continue;

      if (!same_fight(ch, rch))
      continue;
      if (ch == rch)
      continue;

      if (!will_fight_show(ch, FALSE) && type != MOVE_FLY) {
        if (combat_distance(ch, rch, FALSE) > 100)
        continue;
      }
      bool seestart = TRUE;
      bool seefinish = TRUE;
      char_from_room(ch);
      char_to_room(ch, origroom);
      ch->x = originalx;
      ch->y = originaly;
      if (!can_see_char_distance(rch, ch, DISTANCE_MEDIUM))
      seestart = FALSE;
      char_from_room(ch);
      char_to_room(ch, finalroom);
      ch->x = finalx;
      ch->y = finaly;
      if (!can_see_char_distance(rch, ch, DISTANCE_MEDIUM))
      seefinish = FALSE;

      if (seefinish == TRUE && ch->fight_fast == FALSE)
      continue;
      if (seestart == FALSE && seefinish == FALSE)
      continue;

      int origx = ch->x;
      int origy = ch->y;

      if (has_caff(rch, CAFF_BEWILDER)) {
        ch->x = number_range(0, 150);
        ch->y = number_range(0, 150);
      }
      if (type == MOVE_UP) {
        if (ch->in_room->sector_type == SECT_AIR) {
          if (has_caff(ch, CAFF_GLIDING) && has_caff(ch, CAFF_AWIND))
          printf_to_char(rch, "%s is carried aloft by fierce winds.\n\r", PERS(ch, rch));
          else if (has_caff(ch, CAFF_GLIDING))
          printf_to_char(rch, "%s leaps into the air.\n\r", PERS(ch, rch));
          else
          printf_to_char(rch, "%s flies upwards.\n\r", PERS(ch, rch));
        }
        else if (ch->in_room->sector_type == SECT_WATER || ch->in_room->sector_type == SECT_UNDERWATER || ch->in_room->sector_type == SECT_SHALLOW) {
          printf_to_char(rch, "%s swims up.\n\r", PERS(ch, rch));
        }
        else
        printf_to_char(rch, "%s moves up.\n\r", PERS(ch, rch));
      }
      else if (type == MOVE_DOWN) {
        if (ch->in_room->exit[DIR_UP] != NULL && ch->in_room->exit[DIR_UP]->u1.to_room->sector_type == SECT_AIR) {
          printf_to_char(rch, "%s flies down.\n\r", PERS(ch, rch));
        }
        else if (ch->in_room->sector_type == SECT_WATER || ch->in_room->sector_type == SECT_UNDERWATER || ch->in_room->sector_type == SECT_SHALLOW) {
          printf_to_char(rch, "%s swims down.\n\r", PERS(ch, rch));
        }
        else
        printf_to_char(rch, "%s moves down.\n\r", PERS(ch, rch));
      }
      else if (ch->in_room->sector_type == SECT_AIR) {
        if (has_caff(ch, CAFF_GLIDING) && has_caff(ch, CAFF_AWIND))
        printf_to_char(rch, "%s glides on fierce winds to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
        else if (has_caff(ch, CAFF_GLIDING))
        printf_to_char(rch, "%s glides to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
        else
        printf_to_char(rch, "%s flies to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      }
      else if (type == MOVE_MOVE)
      printf_to_char(rch, "%s moves to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      else if (type == MOVE_PATH)
      printf_to_char(
      rch, "%s steps into thin air and emerges at %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      else if (type == MOVE_FLY) {
        if (has_caff(ch, CAFF_GLIDING) && has_caff(ch, CAFF_AWIND))
        printf_to_char(rch, "%s glides on fierce winds to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
        else if (has_caff(ch, CAFF_GLIDING))
        printf_to_char(rch, "%s glides to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
        else
        printf_to_char(rch, "%s flies to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      }
      else if (type == MOVE_CHARGE)
      printf_to_char(rch, "%s charges to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      else if (type == MOVE_RETREAT)
      printf_to_char(rch, "%s retreats to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      else if (type == MOVE_JUMP) {
        if (has_caff(ch, CAFF_AWIND))
        printf_to_char(
        rch, "%s jumps and is carried by fierce winds to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
        else
        printf_to_char(rch, "%s jumps to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      }
      else if (type == MOVE_FLEE)
      printf_to_char(rch, "%s sprints to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      else if (type == MOVE_PROTECT)
      printf_to_char(rch, "%s moves protectively to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));
      else if (type == MOVE_THROWN)
      printf_to_char(rch, "%s is thrown to %d,%d: Distance %d\n\r", PERS(ch, rch), relative_x(rch, ch->in_room, ch->x), relative_y(rch, ch->in_room, ch->y), combat_distance(ch, rch, FALSE));

      ch->x = origx;
      ch->y = origy;
    }
  }

  void fleeroom(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;

    if (room == NULL)
    return;

    int direction = get_flee_direction(ch);

    if (direction == -1)
    return;

    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;

    if ((pexit = room->exit[direction]) == NULL || (pexit->wall != WALL_NONE && pexit->wallcondition != WALLCOND_HOLE) || (to_room = pexit->u1.to_room) == NULL) {
      return;
    }

    //	return;

    act("You flee the fight.", ch, NULL, NULL, TO_CHAR);
    act("$n flees the fight.", ch, NULL, NULL, TO_ROOM);

    if (IS_FLAG(ch->comm, COMM_REPORTED))
    REMOVE_FLAG(ch->comm, COMM_REPORTED);

    char_from_room(ch);
    char_to_room(ch, to_room);
  }

  int get_exit_x(int dir, ROOM_INDEX_DATA *room) {
    switch (dir) {
    case DIR_NORTH:
    case DIR_SOUTH:
      return room->size / 2;
      break;
    case DIR_SOUTHWEST:
    case DIR_NORTHWEST:
    case DIR_WEST:
      return 0;
      break;
    case DIR_NORTHEAST:
    case DIR_SOUTHEAST:
    case DIR_EAST:
      return room->size;
      break;
    }

    return room->size / 2;
  }

  int get_exit_y(int dir, ROOM_INDEX_DATA *room) {
    switch (dir) {
    case DIR_NORTHWEST:
    case DIR_NORTH:
    case DIR_NORTHEAST:
      return room->size;
      break;
    case DIR_EAST:
    case DIR_WEST:
      return room->size / 2;
      break;
    case DIR_SOUTHWEST:
    case DIR_SOUTH:
    case DIR_SOUTHEAST:
      return 0;
      break;
    }
    return 0;
  }

  int closest_exit_x(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    int mindist = 1000;
    int i, dist, direction = DIR_UP;
    for (i = 0; i < 9; i++) {
      if (has_exit(ch->in_room, i)) {
        dist = get_dist(ch->x, ch->y, get_exit_x(i, room), get_exit_y(i, room));
        if (dist < mindist) {
          mindist = dist;
          direction = i;
        }
      }
    }
    return get_exit_x(direction, room);
  }

  int closest_exit_y(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    int mindist = 1000;
    int i, dist, direction = DIR_UP;
    for (i = 0; i < 9; i++) {
      if (has_exit(ch->in_room, i)) {
        dist = get_dist(ch->x, ch->y, get_exit_x(i, room), get_exit_y(i, room));
        if (dist < mindist) {
          mindist = dist;
          direction = i;
        }
      }
    }
    return get_exit_y(direction, room);
  }

  bool has_exit(ROOM_INDEX_DATA *room, int dir) {
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;

    if ((pexit = room->exit[dir]) == NULL || (pexit->wall != WALL_NONE && pexit->wallcondition != WALLCOND_HOLE) || (to_room = pexit->u1.to_room) == NULL) {
      return FALSE;
    }
    return TRUE;
  }

  bool on_roomexit(CHAR_DATA *ch) {
    int i;
    for (i = 0; i < 9; i++) {
      if (ch->x == get_exit_x(i, ch->in_room) && ch->y == get_exit_y(i, ch->in_room) && has_exit(ch->in_room, i))
      return TRUE;
    }
    return FALSE;
  }

  int get_flee_direction(CHAR_DATA *ch) {
    int i;
    for (i = 0; i < 9; i++) {
      if (ch->x == get_exit_x(i, ch->in_room) && ch->y == get_exit_y(i, ch->in_room) && has_exit(ch->in_room, i))
      return i;
    }
    return -1;
  }

  _DOFUN(do_gmmove) {
    char arg1[MSL];
    char arg2[MSL];
    CHAR_DATA *victim;
    CHAR_DATA *mon;
    if (IS_FLAG(ch->act, PLR_HIDE)) {
      send_to_char("Not while you're hidden.\n\r", ch);
      return;
    }
    if (in_fight(ch) == FALSE) {
      send_to_char("You're not in a fight.\n\r", ch);
      return;
    }

    mon = ch->fight_current;

    if (mon == NULL) {
      send_to_char("You don't have anything to fight with.\n\r", ch);
      return;
    }

    if (mon->controled_by != ch) {
      send_to_char("It's not your turn yet.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (ch->fight_fast == FALSE && mon->moving == TRUE) {
      send_to_char("You've already input your move for this round, input your attack or write your attack emote.\n\r", ch);
      return;
    }
    int origx = mon->x;
    int origy = mon->y;
    ROOM_INDEX_DATA *origroom = mon->in_room;
    if (ch->fight_fast == FALSE && IS_FLAG(mon->fightflag, FIGHT_NOMOVE)) {
      send_to_char("You can't move this round.\n\r", ch);
      return;
    }

    if ((victim = get_char_fight(ch, arg1)) == NULL) {
      if (!is_number(arg1) && !is_number(arg2)) {
        send_to_char("Syntax move (person) (distance)/away/charge\n\r", ch);
        return;
      }
      else {
        move_towards(mon, atoi(arg1), atoi(arg2), get_speed(mon), 0, TRUE);
        move_message(mon, MOVE_MOVE, origx, origy, origroom);
      }
    }
    else if (is_number(arg2)) {
      move_towards(mon, relative_x(mon, victim->in_room, victim->x), relative_y(mon, victim->in_room, victim->y), UMIN(atoi(arg2), get_speed(mon)), victim->in_room->z - mon->in_room->z, TRUE);
      move_message(mon, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg2, "jump")) {
      move_towards(mon, relative_x(mon, victim->in_room, victim->x), relative_y(mon, victim->in_room, victim->y), get_speed(mon) * 3, victim->in_room->z - mon->in_room->z, TRUE);
      move_message(mon, MOVE_JUMP, origx, origy, origroom);
    }
    else if (!str_cmp(arg2, "flee")) {
      if (mon->x == mon->in_room->entryx && mon->y == mon->in_room->entryy) {
        if (IS_FLAG(mon->fightflag, FIGHT_FLEEING)) {
          REMOVE_FLAG(mon->fightflag, FIGHT_FLEEING);
          ROOM_INDEX_DATA *room = ch->in_room;
          fleeroom(mon);
          if (fight_over(room))
          end_fight(room);
          else if (ch->fight_fast == FALSE)
          next_attacker(ch, FALSE);
        }
        else {
          SET_FLAG(mon->fightflag, FIGHT_FLEEING);
          send_to_char("You prepare to flee./n/r", ch);
          if (ch->fight_fast == TRUE)
          act("$n prepares to flee.", mon, NULL, NULL, TO_ROOM);
        }
      }
      else {
        move_towards(mon, mon->in_room->entryx, mon->in_room->entryy, get_speed(mon), victim->in_room->z - mon->in_room->z, TRUE);
        move_message(mon, MOVE_FLEE, origx, origy, origroom);
      }
    }
    else if (!str_cmp(arg2, "charge")) {
      move_towards(mon, relative_x(mon, victim->in_room, victim->x), relative_y(mon, victim->in_room, victim->y), get_speed(mon) * 2, victim->in_room->z - mon->in_room->z, TRUE);
      move_message(mon, MOVE_CHARGE, origx, origy, origroom);
    }
    else if (!str_cmp(arg2, "away") || !str_cmp(arg2, "Retreat")) {
      move_away(mon, relative_x(mon, victim->in_room, victim->x), relative_y(mon, victim->in_room, victim->y), get_speed(mon), victim->in_room->z - mon->in_room->z, TRUE);
      move_message(mon, MOVE_RETREAT, origx, origy, origroom);
    }
    else {
      send_to_char("Syntax move (person) (distance)/away/charge/flee\n\r", ch);
      return;
    }

    if (ch->fight_fast == TRUE) {
      send_to_char("You move.\n\r", ch);
      do_function(ch, &do_look, "");
    }
    else {
      printf_to_char(ch, "You will move to %d %d\n\r", mon->x, mon->y);
      mon->actiontimer = UMIN(600, mon->actiontimer + 120);
      printf_to_char(ch, "You have %d seconds left to pick your attack or write your attacking emote.\n\r", ch->actiontimer);
      mon->moving = TRUE;
    }
  }

  int large_weapon_count(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *objcont;
    OBJ_DATA *objcont_next;

    int count = 0;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_WEAPON && obj->size >= 25)
    count++;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_WEAPON && obj->size >= 25)
    count++;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_RANGED && obj->size >= 25)
    count++;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_RANGED && obj->size >= 25)
    count++;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25 && obj->wear_loc == WEAR_NONE)
      count++;
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25 && obj->wear_loc == WEAR_NONE)
      count++;
      if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        for (objcont = obj->contains; objcont != NULL; objcont = objcont_next) {
          objcont_next = objcont->next_content;

          if (objcont->item_type == ITEM_WEAPON && objcont->size >= 25)
          count++;
          if (objcont->item_type == ITEM_RANGED && objcont->size >= 25)
          count++;
        }
      }
    }

    return count;
  }

  int combat_move_speed(CHAR_DATA *ch, int type) {
    int speed = get_speed(ch);
    int bonus = 0;

    if (ch->in_room->sector_type == SECT_AIR && ch->bagcarrier == 0) {
      if (is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <= ANIMAL_TINY)
      return 50;
      return 100;
    }
    if (type == MOVE_FLEE) {
      if (guestmonster(ch))
      return get_speed(ch);

      int count = large_weapon_count(ch);
      if (get_skill(ch, SKILL_COMMANDO) > 1)
      count--;

      bonus = get_speed(ch);
      bonus += UMIN(10, get_skill(ch, SKILL_STRENGTH) + get_skill(ch, SKILL_DEXTERITY) + get_skill(ch, SKILL_ACROBATICS) + get_skill(ch, SKILL_STAMINA));
      if (ch->wounds == 0)
      bonus += 16;
      bonus = bonus * (7 - count) / 7;
      if (!wearing_armor(ch) && !has_shield(ch))
      bonus = bonus * 12 / 10;
      else if (get_skill(ch, SKILL_COMMANDO) < 2)
      bonus = bonus * 8 / 10;

      speed += bonus;
      return speed;
    }
    if (type == MOVE_CHARGE) {
      int count = large_weapon_count(ch);
      if (get_skill(ch, SKILL_FLEETFOOT) > 0)
      count--;

      bonus = get_speed(ch);
      bonus = bonus * (15 - count) / 15;
      if (!wearing_armor(ch) && !has_shield(ch))
      bonus = bonus * 12 / 10;

      if (has_caff(ch, CAFF_RUSH)) {
        bonus = bonus * 3 / 2;
      }

      speed += bonus;
      return speed;
    }
    if (type == MOVE_JUMP) {
      int count = large_weapon_count(ch);
      if (get_skill(ch, SKILL_FLEETFOOT) > 0)
      count--;

      bonus = get_speed(ch) * 2;
      bonus = bonus * (5 - count) / 5;
      if (!wearing_armor(ch) && !has_shield(ch))
      bonus = bonus * 12 / 10;
      speed += bonus;

      if ((has_caff(ch, CAFF_AWIND) || has_caff(ch, CAFF_TKJUMP)) && ch->skills[SKILL_SUPERJUMP] > 0) {
        speed = speed * 3 / 2;
      }
      speed = speed * (4 + get_skill(ch, SKILL_ACROBATICS)) / 4;
      return speed;
    }
    if (type == MOVE_PROTECT) {
      bonus = get_speed(ch) * 3 / 2;
      speed += bonus;
      return speed;
    }
    return speed;
  }

  int facingx(CHAR_DATA *ch) {
    if (ch->facing == DIR_NORTHEAST || ch->facing == DIR_EAST || ch->facing == DIR_SOUTHEAST)
    return 100;
    if (ch->facing == DIR_NORTHWEST || ch->facing == DIR_WEST || ch->facing == DIR_SOUTHWEST)
    return -100;
    return 0;
  }
  int facingy(CHAR_DATA *ch) {
    if (ch->facing == DIR_NORTHWEST || ch->facing == DIR_NORTH || ch->facing == DIR_NORTHEAST)
    return 100;
    if (ch->facing == DIR_SOUTHWEST || ch->facing == DIR_SOUTH || ch->facing == DIR_SOUTHEAST)
    return -100;
    return 0;
  }
  _DOFUN(do_move) {
    char arg1[MSL];
    char arg2[MSL];
    CHAR_DATA *victim;
    char logm[MSL];
    if (is_gm(ch)) {
      do_function(ch, &do_gmmove, argument);
      return;
    }

    if (IS_FLAG(ch->act, PLR_HIDE)) {
      send_to_char("Not while you're hidden.\n\r", ch);
      return;
    }
    if (!IS_NPC(ch) && is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->fightflag, FIGHT_BIPOD)) {
      send_to_char("You can't move with a deployed bipod.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && (IS_SET(ch->in_room->room_flags, ROOM_LANDLINE))) {
      if (ch->pcdata->connected_to != NULL) {
        do_function(ch, &do_hangup, "");
      }
    }

    if (in_fight(ch) == TRUE && ch->move_timer > 0 && ch->fight_fast == TRUE) {
      free_string(ch->qmove);
      ch->qmove = str_dup(argument);
      send_to_char("Command stacked.\n\r", ch);
      return;
    }

    free_string(ch->qmove);
    ch->qmove = str_dup("");

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    int orig = ch->facing;
    int origx = ch->x;
    int origy = ch->y;
    ROOM_INDEX_DATA *origroom = ch->in_room;
    if (in_fight(ch) == FALSE) {
      if (is_number(arg1) && is_number(arg2)) {
        move_towards(ch, atoi(arg1), atoi(arg2), get_speed(ch) * 2, 0, TRUE);
        send_to_char("You reposition.\n\r", ch);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);
        ch->facing = orig;
        return;
      }
      send_to_char("You're  not in a fight, you'll have to use move (x) (y) to reposition\n\r", ch);
      return;
    }

    if (ch->fight_fast == FALSE && ch->fight_current != ch) {
      send_to_char("It's not your turn yet.\n\r", ch);
      return;
    }

    if (ch->fight_fast == FALSE && ch->moving == TRUE) {
      send_to_char("You've already input your move for this round, input your attack or write your emote.\n\r", ch);
      return;
    }

    if (ch->fight_fast == FALSE && IS_FLAG(ch->fightflag, FIGHT_NOMOVE)) {
      send_to_char("You can't move this round.\n\r", ch);
      return;
    }
    if (ch->fight_fast == FALSE && has_caff(ch, CAFF_ROOT)) {
      send_to_char("You can't move this round.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch))
    ch->pcdata->fatigue += MOVE_FATIGUE;

    if (!IS_NPC(ch))
    ch->pcdata->protecting = NULL;

    if (!str_cmp(arg1, "flee") || !str_cmp(arg1, "sprint")) {
      if (ch->facing == DIR_DOWN || ch->facing == DIR_UP) {
        send_to_char("You can't sprint in that direction.\n\r", ch);
        return;
      }
      if (is_attacking(ch)) {
        move_towards(ch, facingx(ch), facingy(ch), combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
        move_message(ch, MOVE_MOVE, origx, origy, origroom);
      }
      else {
        if (!str_cmp(arg1, "flee"))
        ch->attacking = 0;
        move_towards(ch, facingx(ch), facingy(ch), combat_move_speed(ch, MOVE_FLEE), 0, TRUE);
        move_message(ch, MOVE_FLEE, origx, origy, origroom);
        useattack(ch);
      }
    }
    else if (!str_cmp(arg1, "down")) {
      if (ch->in_room == NULL || ch->in_room->exit[DIR_DOWN] == NULL || ch->in_room->exit[DIR_DOWN]->u1.to_room == NULL || !open_sound(ch->in_room, DIR_DOWN) || !can_get_to(ch, ch->in_room->exit[DIR_DOWN]->u1.to_room)) {
        send_to_char("You can't go down any further.\n\r", ch);
        return;
      }
      ROOM_INDEX_DATA *desti = ch->in_room->exit[DIR_DOWN]->u1.to_room;
      char_from_room(ch);
      char_to_room(ch, desti);
      move_message(ch, MOVE_DOWN, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "up")) {
      if (ch->in_room == NULL || ch->in_room->exit[DIR_UP] == NULL || ch->in_room->exit[DIR_UP]->u1.to_room == NULL || !open_sound(ch->in_room, DIR_UP) || !can_get_to(ch, ch->in_room->exit[DIR_UP]->u1.to_room)) {
        send_to_char("You can't go up any further.\n\r", ch);
        return;
      }
      ROOM_INDEX_DATA *desti = ch->in_room->exit[DIR_UP]->u1.to_room;
      char_from_room(ch);
      char_to_room(ch, desti);
      move_message(ch, MOVE_UP, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "north")) {
      move_towards(ch, 0, 100, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "south")) {
      move_towards(ch, 0, -100, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "east")) {
      move_towards(ch, 100, 0, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "west")) {
      move_towards(ch, -100, 0, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "northeast")) {
      move_towards(ch, 100, 100, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "northwest")) {
      move_towards(ch, -100, 100, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "southeast")) {
      move_towards(ch, 100, -100, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "southwest")) {
      move_towards(ch, -100, -100, combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "extract") || !str_cmp(arg1, "extraction")) {
      if (battleground(ch->in_room)) {
        sprintf(logm, "%s runs towards the extraction point.", ch->name);
        op_report(str_dup(logm), ch);
      }
      move_towards(ch, get_poix(ch, POI_EXTRACT), get_poiy(ch, POI_EXTRACT), combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg1, "capture")) {
      if (battleground(ch->in_room)) {
        sprintf(logm, "%s runs towards the objective.", ch->name);
        op_report(str_dup(logm), ch);
      }
      move_towards(ch, get_poix(ch, POI_CAPTURE), get_poiy(ch, POI_CAPTURE), combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if ((victim = get_char_fight(ch, arg1)) == NULL) {
      if (!is_number(arg1) && !is_number(arg2)) {
        send_to_char("Syntax move (person) (distance)/away/charge/protect\n\r", ch);
        return;
      }
      else {
        move_towards(ch, atoi(arg1), atoi(arg2), combat_move_speed(ch, MOVE_MOVE), 0, TRUE);
        ch->facing = orig;
        move_message(ch, MOVE_MOVE, origx, origy, origroom);
      }
    }
    else if (is_number(arg2) && atoi(arg2) > 0) {
      move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->x), UMIN(atoi(arg2), combat_move_speed(ch, MOVE_MOVE)), victim->in_room->z - ch->in_room->z, TRUE);
      move_message(ch, MOVE_MOVE, origx, origy, origroom);
    }
    else if (!str_cmp(arg2, "jump")) {
      if (get_skill(ch, SKILL_SUPERJUMP) >= 1 && !deep_water(ch) && !airborne(ch) && !has_caff(ch, CAFF_JUMPED) && !has_caff(ch, CAFF_GUST)) {
        if (combat_distance(ch, victim, FALSE) <=
            combat_move_speed(ch, MOVE_JUMP) + 1 && has_caff(victim, CAFF_THROWING)) {
          remove_caff(victim, CAFF_THROWING);
          int xmod = 0, ymod = 0;
          if (relative_x(ch, victim->in_room, victim->x) < 0)
          xmod = -30;
          else if (relative_x(ch, victim->in_room, victim->x) > 0)
          xmod = 30;
          if (relative_y(ch, victim->in_room, victim->y) < 0)
          ymod = -30;
          else if (relative_y(ch, victim->in_room, victim->y) > 0)
          ymod = 30;
          move_towards(ch, relative_x(ch, victim->in_room, victim->x) + xmod, relative_y(ch, victim->in_room, victim->y) + ymod, 2 * combat_move_speed(ch, MOVE_JUMP), victim->in_room->z - ch->in_room->z, TRUE);
          move_message(ch, MOVE_THROWN, origx, origy, origroom);
          send_to_char("You are thrown.\n\r", ch);
        }
        else {
          if (battleground(ch->in_room)) {
            sprintf(logm, "%s makes a supernatural leap to %s.", ch->name, victim->name);
            op_report(str_dup(logm), ch);
          }

          move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_JUMP), victim->in_room->z - ch->in_room->z, TRUE);
          move_message(ch, MOVE_JUMP, origx, origy, origroom);
        }
        apply_caff(ch, CAFF_JUMPED, 4);
      }
      else {
        static char buf[200];
        sprintf(buf, "%s charge", arg1);
        do_function(ch, &do_move, buf);
        return;
      }
    }
    else if (!str_cmp(arg2, "path") || !str_cmp(arg2, "pathing")) {
      if (IS_FLAG(ch->act, PLR_SHROUD) || get_skill(ch, SKILL_PATHING) < 3) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (combat_distance(ch, victim, FALSE) <= 200 && same_fight(ch, victim)) {
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        ch->x = victim->x;
        ch->y = victim->y;
        move_message(ch, MOVE_PATH, origx, origy, origroom);
      }
      else {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), 100, victim->in_room->z - ch->in_room->z, TRUE);
        move_message(ch, MOVE_PATH, origx, origy, origroom);
      }
    }
    else if (!str_cmp(arg2, "charge")) {
      if (battleground(ch->in_room)) {
        sprintf(logm, "%s charges toward %s.", ch->name, victim->name);
        op_report(str_dup(logm), ch);
      }
      if (combat_distance(ch, victim, FALSE) <=
          combat_move_speed(ch, MOVE_CHARGE) + 1 && has_caff(victim, CAFF_THROWING)) {
        remove_caff(victim, CAFF_THROWING);

        int xmod = 0, ymod = 0;
        if (relative_x(ch, victim->in_room, victim->x) < 0)
        xmod = -20;
        else if (relative_x(ch, victim->in_room, victim->x) > 0)
        xmod = 20;
        if (relative_y(ch, victim->in_room, victim->y) < 0)
        ymod = -20;
        else if (relative_y(ch, victim->in_room, victim->y) > 0)
        ymod = 20;

        move_towards(ch, relative_x(ch, victim->in_room, victim->x) + xmod, relative_y(ch, victim->in_room, victim->y) + ymod, 2 * combat_move_speed(ch, MOVE_CHARGE), victim->in_room->z - ch->in_room->z, TRUE);
        move_message(ch, MOVE_THROWN, origx, origy, origroom);
        send_to_char("You are thrown.\n\r", ch);
      }
      else {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_CHARGE), victim->in_room->z - ch->in_room->z, TRUE);
        move_message(ch, MOVE_CHARGE, origx, origy, origroom);
      }
    }
    else if (!str_cmp(arg2, "protect") && ch != victim) {
      if (battleground(ch->in_room)) {
        sprintf(logm, "%s moves protectively towards %s.", ch->name, victim->name);
        op_report(str_dup(logm), ch);
      }

      move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_PROTECT), victim->in_room->z - ch->in_room->z, TRUE);
      move_message(ch, MOVE_PROTECT, origx, origy, origroom);
      if (!IS_NPC(ch))
      ch->pcdata->protecting = victim;
      ch->facing = orig;
    }
    else if (!str_cmp(arg2, "away") || !str_cmp(arg2, "Retreat")) {
      if (battleground(ch->in_room)) {
        sprintf(logm, "%s retreats from %s.", ch->name, victim->name);
        op_report(str_dup(logm), ch);
      }

      if (atoi(argument) > 0 && atoi(argument) < combat_move_speed(ch, MOVE_RETREAT) && atoi(argument) > 0)
      move_away(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), atoi(argument), 0, TRUE);
      else
      move_away(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_RETREAT), 0, TRUE);
      if (!str_cmp(arg2, "retreat"))
      ch->facing = orig;
      ch->run_dir = orig;
      move_message(ch, MOVE_RETREAT, origx, origy, origroom);
    }
    else {
      send_to_char("Syntax move (person) (distance)/away/charge/flee/protect\n\r", ch);
      return;
    }

    if (ch->fight_fast == TRUE) {
      if (will_fight_show(ch, FALSE)) {
        if (!IS_FLAG(ch->comm, COMM_BRIEF)) {
          SET_FLAG(ch->comm, COMM_BRIEF);
          do_function(ch, &do_look, "auto");
          REMOVE_FLAG(ch->comm, COMM_BRIEF);
        }
        else
        do_function(ch, &do_look, "auto");
      }
    }
    else {
      ch->actiontimer = UMIN(600, ch->actiontimer + 120);
      printf_to_char(
      ch, "You have %d seconds left to write your attacking/moving emote.\n\r", ch->actiontimer);
      ch->moving = TRUE;
    }

    if (IS_FLAG(ch->fightflag, FIGHT_SLOW1)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_SLOW1);
      SET_FLAG(ch->fightflag, FIGHT_SLOW2);
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_SLOW2)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_SLOW2);
    }
  }

  char *special_name(int disc) {

    if (disc == SPECIAL_STUN)
    return "Stun";
    if (disc == SPECIAL_KNOCKBACK)
    return "Knockback";
    if (disc == SPECIAL_ENTRAP)
    return "Entrap";
    if (disc == SPECIAL_GRAPPLE)
    return "Grapple";
    if (disc == SPECIAL_LIFESTEAL)
    return "Life Steal";
    if (disc == SPECIAL_DELAY)
    return "Delayed";
    if (disc == SPECIAL_DELAY2)
    return "Long Delayed";
    if (disc == SPECIAL_OVERPOWER)
    return "Overpower";
    if (disc == SPECIAL_DAMAGE)
    return "Damage";
    if (disc == SPECIAL_WEAKEN)
    return "WEAKEN";
    if (disc == SPECIAL_UNDERPOWER)
    return "Underpower";
    if (disc == SPECIAL_AOE)
    return "AOE";
    if (disc == SPECIAL_SLOW)
    return "Slow";
    if (disc == SPECIAL_LONGWEAKEN)
    return "Long Weaken";
    if (disc == SPECIAL_WOUND)
    return "Wounding";

    return "";
  }

  _DOFUN(do_hostile) {
    CHAR_DATA *victim;
    if (!IS_NPC(ch) && is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->comm, COMM_HOSTILE)) {
      REMOVE_FLAG(ch->comm, COMM_HOSTILE);
      act("You stop taking a hostile stance.", ch, NULL, NULL, TO_CHAR);
      act("$n relaxes from their hostile stance.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);
      return;
    }

    if ((victim = get_char_fight(ch, argument)) == NULL) {
      send_to_char("Attack who?\n\r", ch);
      return;
    }
    if (victim->wounds >= 4)
    return;
    if (in_public(ch, victim)) {
      if (!IS_FLAG(ch->act, PLR_SHROUD) || !room_ambush(ch->in_room)) {
        send_to_char("This area is too public to start a fight.\n\r", ch);
        return;
      }
    }
    if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD))
    return;

    if (is_safe(ch, victim)) {
      send_to_char("You can't bring yourself to do that.\n\r", ch);
      return;
    }
    SET_FLAG(ch->comm, COMM_HOSTILE);
    act("You take a hostile stance.", ch, NULL, NULL, TO_CHAR);
    act("$n takes a hostile stance.", ch, NULL, NULL, TO_ROOM);
  }

  bool can_use(CHAR_DATA *ch, int special, int discipline) {
    int k, l;
    int uniques[15];
    int count = 0;

    if (special == SPECIAL_MINION || special == SPECIAL_ALLY || special == 0)
    return TRUE;

    for (l = 0; l < 15; l++)
    uniques[l] = 0;

    for (k = 0; k < 25; k++) {

      if (discipline == ch->pcdata->customstats[k][1]) {
        bool found = FALSE;
        for (l = 0; l < 15; l++) {
          if (uniques[l] == ch->pcdata->customstats[k][0])
          found = TRUE;
        }
        if (found == FALSE)
        for (l = 0; l < 15; l++) {
          if (uniques[l] == 0) {
            uniques[l] = ch->pcdata->customstats[k][0];
            break;
          }
        }
      }
    }
    for (l = 0; l < 15; l++) {
      if (uniques[l] > 0 && uniques[l] != SPECIAL_MINION && uniques[l] != SPECIAL_ALLY)
      count += 1;
    }
    if (count > max_specials(ch, discipline))
    return FALSE;

    return TRUE;
  }

  int process_special(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int point) {
    if (IS_NPC(ch))
    return dam;
    int special = ch->pcdata->customstats[point][0];
    int i = 0;

    int disc = ch->pcdata->customstats[point][1];

    ROOM_INDEX_DATA *room = ch->in_room;

    if (room == NULL)
    return dam;
    int facedir = victim->facing;

    switch (special) {
    case SPECIAL_STUN:
      if (ch->fight_fast == TRUE) {
        victim->attack_timer += FIGHT_WAIT * fight_speed(victim);
        victim->move_timer += FIGHT_WAIT * fight_speed(victim);
      }
      else {
        SET_FLAG(victim->fightflag, FIGHT_NOATTACK);
        SET_FLAG(victim->fightflag, FIGHT_NOMOVE);
      }
      dam = dam / 2;
      if (IS_FLAG(victim->fightflag, FIGHT_DEFEND))
      REMOVE_FLAG(victim->fightflag, FIGHT_DEFEND);
      break;
    case SPECIAL_KNOCKBACK:
      int temporary;
      temporary = victim->move_timer;
      move_away(victim, ch->x, ch->y, 15, 0, FALSE);
      victim->facing = facedir;
      victim->move_timer = temporary;
      fall_character(victim);

      break;
    case SPECIAL_ENTRAP:
      if (ch->fight_fast == TRUE)
      victim->move_timer =
      UMIN(victim->move_timer + FIGHT_WAIT * fight_speed(victim), FIGHT_WAIT * fight_speed(victim) * 3);
      else
      SET_FLAG(victim->fightflag, FIGHT_NOMOVE);
      fall_character(victim);
      break;
    case SPECIAL_GRAPPLE:
      dam = dam * 8 / 10;
      if (ch->fight_fast == TRUE)
      victim->move_timer += FIGHT_WAIT * fight_speed(victim);
      else
      SET_FLAG(victim->fightflag, FIGHT_NOMOVE);

      fall_character(victim);

      break;
    case SPECIAL_LIFESTEAL:
      ch->hit += dam / 2;
      dam = dam * 3 / 4;
      break;
    case SPECIAL_DELAY:
      for (i = 0; i < 10 && ch->delays[i][1] > 0; i++) {
      }
      ch->delays[i][0] = point;
      if (ch->fight_fast != TRUE)
      ch->delays[i][1] = 2;
      else
      ch->delays[i][1] = 1;

      ch->delays[i][2] =
      UMIN(UMAX(dam_caff_mod(ch, victim, 10, disc, FALSE), 8), 12);
      dam = 0;
      ch->delay_target = victim;
      break;
    case SPECIAL_DELAY2:
      for (i = 0; i < 10 && ch->delays[i][1] > 0; i++) {
      }
      ch->delays[i][0] = point;
      if (ch->fight_fast != TRUE)
      ch->delays[i][1] = 3;
      else
      ch->delays[i][1] = 2;

      ch->delays[i][2] =
      UMIN(UMAX(dam_caff_mod(ch, victim, 10, disc, FALSE), 8), 12);
      dam = 0;
      ch->delay_target = victim;
      break;
    case SPECIAL_OVERPOWER:
      if(IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER))
      dam = dam * 12/10;
      else if(IS_FLAG(ch->fightflag, FIGHT_OVERPOWER))
      dam = dam * 12/10;
      else
      dam = dam * 14 / 10;
      SET_FLAG(ch->fightflag, FIGHT_OVERPOWER);
      break;
    case SPECIAL_DAMAGE:
      dam = dam * 110 / 100;
      break;
    case SPECIAL_WEAKEN:
      SET_FLAG(victim->fightflag, FIGHT_WEAKEN);
      dam = dam * 3 / 4;
      break;
    case SPECIAL_LONGWEAKEN:
      SET_FLAG(victim->fightflag, FIGHT_WEAKEN1);
      dam = dam * 3 / 4;
      break;
    case SPECIAL_SLOW:
      SET_FLAG(victim->fightflag, FIGHT_SLOW1);
      fall_character(victim);

      break;
    case SPECIAL_AOE:
      SET_FLAG(ch->fightflag, FIGHT_AOE);
      dam = dam * 2 / 5;
      break;
    case SPECIAL_WOUND:
      SET_FLAG(ch->fightflag, FIGHT_WOUND);
      dam = dam / 2;
      break;
    case SPECIAL_UNDERPOWER:
      dam = dam / 2;
      if (dam > 0)
      SET_FLAG(ch->fightflag, FIGHT_UNDERPOWER);
      ch->underpowering = victim;
      break;
    case SPECIAL_MINION:
      dam = 0;
      if (!is_neutralized(ch))
      SET_FLAG(ch->fightflag, FIGHT_MINIONS1);
      break;
    case SPECIAL_ALLY:
      dam = 0;
      SET_FLAG(ch->fightflag, FIGHT_ALLIES1);
      break;
    }

    if (!IS_NPC(victim) && max_hp(victim) >= 100) {
      dam = UMIN(
      dam, max_hp(victim) *
      (20 - get_skill(ch, SKILL_MINIONS) - average_muscle(ch)) / 50);
      if (IS_NPC(ch) && ch->pIndexData->vnum == ALLY_TEMPLATE) {
        if (safe_strlen(ch->protecting) > 1 && get_char_world_pc(ch->protecting) != NULL) {
          if (is_attacking(get_char_world_pc(ch->protecting)))
          dam = UMIN(dam, max_hp(victim) * 3 / 10);
          ;
        }
      }
      if (IS_NPC(ch) && ch->pIndexData->vnum == MINION_TEMPLATE) {
        if (safe_strlen(ch->protecting) > 1 && get_char_world_pc(ch->protecting) != NULL) {
          if (is_attacking(get_char_world_pc(ch->protecting)))
          dam = UMIN(dam, max_hp(victim) * 3 / 10);
          ;
        }
      }
    }
    else if (IS_NPC(victim) && (victim->pIndexData->vnum == MINION_TEMPLATE) && max_hp(victim) >= 30)
    dam = UMIN(dam, max_hp(victim) * 6 / 10);
    else if (IS_NPC(victim) && (victim->pIndexData->vnum == ALLY_TEMPLATE) && max_hp(victim) >= 50)
    dam = UMIN(dam, max_hp(victim) * 6 / 10);
    return dam;
  }

  void delayattacks(CHAR_DATA *ch, CHAR_DATA *victim) {

    if (ch->delay_target != NULL)
    victim = ch->delay_target;

    if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL || ch == victim)
    return;

    if (IS_NPC(ch))
    return;

    bool protecting = FALSE;

    if (is_protected(victim) && get_protector(victim) != ch && !is_in_cover(victim)) {
      CHAR_DATA *temp;
      temp = get_protector(victim);
      victim = temp;
      protecting = TRUE;
    }

    int j = 0, dam, k, point = 0;

    for (j = 0; j < 10; j++) {
      if (ch->delays[j][1] > 0) {
        ch->delays[j][1]--;
        if (ch->delays[j][1] == 0) {
          for (k = 0; k < DIS_USED; k++) {
            if (discipline_table[k].vnum ==
                ch->pcdata->customstats[ch->delays[j][0]][1])
            point = k;
          }
          int shield = shield_type(victim);
          dam = damage_calculate(ch, victim, point, shield);

          dam = ch->delays[j][2] * dam / 10;
          if (ch->pcdata->customstats[ch->delays[j][0]][0] == SPECIAL_DELAY)
          dam = dam*11/10;
          else if (ch->pcdata->customstats[ch->delays[j][0]][0] == SPECIAL_DELAY2)
          dam = dam * 12 / 10;

          if (protecting == TRUE && !IS_NPC(victim) && get_skill(victim, SKILL_GUARDIAN) > 0)
          dam = dam * 2 / 3;

          if (!IS_NPC(ch))
          gain_exp(
          ch, xp_per_attack(ch, victim, discipline_table[point].vnum, dam), FEXP_ATTACK);

          int stag = apply_stagger(ch, victim, dam, discipline_table[point].vnum);
          victim->wound_location = number_percent();
          int echo = damage_echo(ch, victim, discipline_table[point].vnum, discipline_table[point].vnum, dam, combat_distance(ch, victim, TRUE), TRUE);

          if (ch->fight_fast == TRUE) {
            char buf[MSL];
            if (dam > 0 || stag > 0) {
              sprintf(buf, "%s %s", ch->pcdata->customstrings[ch->delays[j][0]][4], defense_message(victim, TO_CHAR, TRUE, echo));
              act(buf, ch, NULL, victim, TO_CHAR);
              sprintf(buf, "%s %s", ch->pcdata->customstrings[ch->delays[j][0]][5], defense_message(victim, TO_VICT, TRUE, echo));
              act(buf, ch, NULL, victim, TO_VICT);
              sprintf(buf, "%s %s", ch->pcdata->customstrings[ch->delays[j][0]][6], defense_message(victim, TO_NOTVICT, TRUE, echo));
              act(buf, ch, NULL, victim, TO_ROOM);
            }
            else {
              act(ch->pcdata->customstrings[ch->delays[j][0]][4], ch, NULL, victim, TO_CHAR);
              act(ch->pcdata->customstrings[ch->delays[j][0]][5], ch, NULL, victim, TO_VICT);
              act(ch->pcdata->customstrings[ch->delays[j][0]][6], ch, NULL, victim, TO_NOTVICT);
            }
          }
          if (dam > 0 || stag > 0) {
            if (get_shadow(ch, ch->delays[j][0]) > -1) {
              printf_to_char(
              ch, "`x[%s reduces defenses by `r%d`x, balance by `c%d`x]\n\r", discipline_table[get_shadow(ch, ch->delays[j][0])].name, dam, stag);
              printf_to_char(
              victim, "`x[%s reduces defenses by `r%d`x, balance by `c%d`x]\n\r", discipline_table[get_shadow(ch, ch->delays[j][0])].name, dam, stag);
            }
            else {
              printf_to_char(
              ch, "`x[%s reduces defenses by `r%d`x, balance by `c%d`x]\n\r", disc_name(ch, point), dam, stag);
              printf_to_char(
              victim, "`x[%s reduces defenses by `r%d`x, balance by `c%d`x]\n\r", disc_name(ch, point), dam, stag);
            }
          }
          combat_damage(victim, ch, dam, discipline_table[point].vnum);
          death_cause(ch, victim, discipline_table[point].vnum);
        }
      }
    }
  }

  int char_power_score(CHAR_DATA *mob) {
    int shield_total = 0;
    int cost = 0;
    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range == -1)
      shield_total += mob->disciplines[discipline_table[i].vnum];
      else
      cost += built_disc_cost(mob->disciplines[discipline_table[i].vnum]);
    }
    cost += built_disc_cost(shield_total);
    return cost;
  }

  int power_score(CHAR_DATA *ch) {
    int val = char_power_score(ch);
    val /= 1000;
    val /= 10;
    val += 5;
    val = UMAX(7, val);
    val = UMIN(150, val);
    return val;
  }

  int xp_per_attack(CHAR_DATA *ch, CHAR_DATA *victim, int disc, int damage) {
    int val = 0;
    int mod;
    if (damage == 0)
    return 0;
    if (IS_NPC(victim) && victim->controled_by != NULL && same_player(victim->controled_by, ch))
    return 0;
    if (IS_NPC(victim) && victim->pIndexData->vnum < 10)
    return 0;
    //    if(disc == DIS_HELLFIRE || disc == DIS_SHADOWL)
    //        return 0;

    if (ch == victim)
    return 0;

    if (IS_NPC(victim) && (IS_FLAG(victim->act, ACT_COVER) || IS_FLAG(victim->act, ACT_TURRET)))
    return 0;

    if (higher_power(ch) || higher_power(victim))
    return 0;

    if (power_pop(ch->in_room) > 0)
    return 0;

    if (victim->hit <= 0) {
      damage = UMIN(damage, max_hp(victim) / 5);
      damage = UMAX(damage, max_hp(victim) / 10);
    }
    else
    damage = UMIN(damage, victim->hit);

    val = 250 * damage / max_hp(victim);

    val /= 28;
    val *= power_score(victim);

    if (ch->in_room != NULL && battleground(ch->in_room)) {
      if (activeoperation == NULL || activeoperation->competition == COMPETE_CLOSED)
      val /= 2;
      else
      val *= 2;
    }
    if (ch->fight_fast == TRUE && !IS_NPC(ch)) {
      for (int i = 0; i < ch->pcdata->default_speed; i++)
      val = val * 6 / 5;
    }

    if (ch->fight_fast == FALSE) {
      val *= 5;
      if (!is_sparring(ch) && !are_allies(ch, victim) && ch->faction != 0)
      give_intel(ch, 100);
    }

    if (IS_NPC(victim) && IS_FLAG(victim->act, ACT_BIGGAME))
    val *= 5;

    if (IS_NPC(victim) && forest_monster(victim) && in_world(victim) != WORLD_EARTH)
    val = val * 3 / 2;

    if (ch->fight_fast == FALSE && !IS_NPC(victim) && combat_focus(victim) > combat_focus(ch))
    val *= 4;

    if (guestmonster(victim))
    val *= 2;

    if (damage > 20) {
      mod = 200 - damage;
      mod /= 10;
      val += mod;
    }
    val = UMIN(val, 300);

    if (battleground(ch->in_room) && !IS_NPC(victim))
    val *= 2;
    else if (!IS_NPC(victim)) {
      if (ch->pcdata->account != NULL) {
        if (ch->pcdata->account->daily_pvp_exp > 15000)
        return 0;
        else
        ch->pcdata->account->daily_pvp_exp += val * EXP_GAIN_MULT / 100;
      }
    }

    val = val * EXP_GAIN_MULT / 100;

    return val;

    /*
char buf[MSL];
sprintf(buf,  "XPTRACK %s %d hits %s N: %d, O: %d\n\r", ch->name, get_lvl(ch), victim->name, val, xp_per_attack_old(ch, victim, disc, damage)); log_string(buf);

return xp_per_attack_old(ch, victim, disc, damage);
*/
  }

  int xp_per_attack_old(CHAR_DATA *ch, CHAR_DATA *victim, int disc, int damage) {
    int point = 0, val, max;
    int max_ranged = default_ranged_value(ch);
    int max_melee = default_melee_value(ch);

    if (damage == 0)
    return 0;

    if (IS_NPC(victim) && victim->controled_by != NULL && same_player(victim->controled_by, ch))
    return 0;

    if (IS_NPC(victim) && victim->pIndexData->vnum < 10)
    return 0;

    if (disc == DIS_HELLFIRE || disc == DIS_SHADOWL || disc == DIS_CUSTOM)
    return 0;

    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].vnum == disc)
      point = i;
    }

    if (max_ranged > max_melee)
    max = default_ranged(ch);
    else
    max = default_melee(ch);

    int maxval = get_disc(ch, max, FALSE);
    if (maxval == 0)
    return 0;

    val = 100 * damage / maxval;
    if (disc_range(ch, point) > 1) {
      if (max_melee > max_ranged)
      val = val * 5 / 4;
    }
    else {
      if (max_ranged > max_melee)
      val = val * 5 / 4;
    }

    char buf[MSL];
    if (total_disc(ch) < 10) {
      sprintf(buf, "EXPCHEATER %s %d", ch->name, total_disc(ch));
      log_string(buf);
    }

    val = val * EXP_GAIN_MULT / 100;
    return val;
  }

  _DOFUN(do_gmattack) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];
    char buf[MSL];
    int vnum = 0, dam, i, point, opoint;
    CHAR_DATA *victim;
    CHAR_DATA *mon;

    if (in_fight(ch) == FALSE) {
      if (fight_room_pop(ch->in_room) < 2) {
        send_to_char("There's nobody here for you to fight.\n\r", ch);
        return;
      }
      start_hostilefight(ch);
      act("\n`RA fight breaks out!`x", ch, NULL, NULL, TO_CHAR);
      act("\n`RA fight breaks out!`x", ch, NULL, NULL, TO_ROOM);
      if (in_fight(ch) != FALSE) {
        next_attacker(ch, TRUE);
      }
      return;
    }

    if (ch->fight_fast == FALSE) {
      argument = one_argument_nouncap(argument, arg1);
      argument = one_argument_nouncap(argument, arg2);

      mon = ch->fight_current;

      if (mon == NULL) {
        send_to_char("You don't have anything to fight with.\n\r", ch);
        return;
      }

      if (mon->controled_by != ch) {
        send_to_char("It's not your turn yet.\n\r", ch);
        return;
      }
      if (mon->attacking == 0)
      mon->attacking = 1;

      if (!str_cmp(arg1, "skip")) {
        send_to_char("Attack skipped.\n\r", ch);
        act("$n skips $s chance to attack.", mon, NULL, NULL, TO_ROOM);
        mon->attacking = 0;
        next_attacker(ch, FALSE);
        return;
      }

      if (IS_FLAG(mon->fightflag, FIGHT_NOATTACK)) {
        send_to_char("You can't attack this round.\n\r", ch);
        return;
      }
      if (has_caff(mon, CAFF_STUNNED)) {
        send_to_char("You can't attack this round.\n\r", ch);
        return;
      }

      else if ((victim = get_char_fight(ch, arg1)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (victim->wounds >= 4)
      return;

      if (mon->attackdam > 0 && mon->actiontimer > 0) {
        send_to_char("You've already put in an attack this round, either input your move or write your combat emote.\n\r", ch);
        return;
      }
      ch->fight_attacks++;
      point = -1;
      if (point == -1) {
        for (i = 0; i < DIS_USED; i++) {
          if (!str_cmp(arg2, disc_name(ch, i)))
          point = i;
        }
        if (point == -1) {
          argument = one_argument_nouncap(argument, arg3);
          sprintf(buf, "%s %s", arg2, arg3);
          for (i = 0; i < DIS_USED; i++) {
            if (!str_cmp(buf, disc_name(ch, i)))
            point = i;
          }
        }
        if (point == -1) {
          argument = one_argument_nouncap(argument, arg4);
          sprintf(buf, "%s %s", buf, arg4);
          for (i = 0; i < DIS_USED; i++) {
            if (!str_cmp(buf, disc_name(ch, i)))
            point = i;
          }
        }
        if (point == -1) {
          if (combat_distance(mon, victim, TRUE) <= 1)
          vnum = default_melee(mon);
          else
          vnum = default_ranged(mon);

          for (i = 0; i < DIS_USED; i++) {
            if (discipline_table[i].vnum == vnum)
            point = i;
          }
        }
      }
      if (combat_distance(mon, victim, TRUE) > 1 && combat_distance(mon, victim, TRUE) > disc_range(mon, point)) {
        send_to_char("They're out of range of that discipline.\n\r", ch);
        return;
      }
      dam = get_disc(mon, discipline_table[point].vnum, FALSE);
      if (discipline_table[point].range > 1) {
        dam -= get_skill(victim, SKILL_EVADING);
        if (combat_distance(ch, victim, TRUE) < 2)
        dam = dam *
        damage_mod(combat_distance(mon, victim, TRUE), disc_range(mon, point)) /
        100;
        else
        apply_blackeyes(ch);
      }
      else {
        dam -= get_skill(victim, SKILL_DODGING);
      }

      opoint = point;
      mon->chattacking = victim;
      mon->attackdisc = discipline_table[point].vnum;
      mon->attackdam = dam;
      mon->defensedisc = shield_type(victim);
      victim->wound_location = number_percent();
      for (i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == mon->defensedisc)
        point = i;
      }

      if (IS_FLAG(mon->fightflag, FIGHT_OVERPOWER)) {
        dam = dam * 2 / 5;
        REMOVE_FLAG(mon->fightflag, FIGHT_OVERPOWER);
      }
      if (IS_FLAG(mon->fightflag, FIGHT_WEAKEN)) {
        dam = dam / 2;
        REMOVE_FLAG(mon->fightflag, FIGHT_WEAKEN);
      }
      if (IS_FLAG(mon->fightflag, FIGHT_WEAKEN1)) {
        REMOVE_FLAG(mon->fightflag, FIGHT_WEAKEN1);
        SET_FLAG(mon->fightflag, FIGHT_WEAKEN2);
        dam = dam * 3 / 4;
      }
      else if (IS_FLAG(mon->fightflag, FIGHT_WEAKEN2)) {
        REMOVE_FLAG(mon->fightflag, FIGHT_WEAKEN2);
        SET_FLAG(mon->fightflag, FIGHT_WEAKEN3);
        dam = dam * 3 / 4;
      }
      else if (IS_FLAG(mon->fightflag, FIGHT_WEAKEN3)) {
        REMOVE_FLAG(mon->fightflag, FIGHT_WEAKEN3);
        dam = dam * 3 / 4;
      }

      if (IS_FLAG(mon->fightflag, FIGHT_UNDERPOWER) && mon->underpowering == victim) {
        dam = dam * 150 / 100;
        REMOVE_FLAG(mon->fightflag, FIGHT_UNDERPOWER);
      }

      mon->attackdam = dam;
      if (dam <= victim->hit)
      printf_to_char(ch, "Your %s hit will be absorbed by %s's %s.\n\r", disc_name(ch, opoint), PERS(victim, ch), discipline_table[point].name);
      else {
        if (IS_NPC(victim) && victim->wounds == 0)
        printf_to_char(ch, "Your %s hit will severely wound %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
        else if (victim->wounds == 0)
        printf_to_char(ch, "Your %s hit will mildly wound %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
        else if (victim->wounds == 1)
        printf_to_char(ch, "Your %s hit will severely wound %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
        else if (victim->wounds == 2 && !IS_NPC(victim))
        printf_to_char(ch, "Your %s hit will critically wound %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
        else
        printf_to_char(ch, "Your %s hit will kill %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
      }
      mon->attacking = 1;
      mon->actiontimer = 600;
      printf_to_char(
      ch, "You have %d seconds left to write your attacking emote.\n\r", ch->actiontimer);
    }
  }

  char *special_message(CHAR_DATA *ch, int point) {
    int special = ch->pcdata->customstats[point][0];
    if (special < 0 || special > 30)
    return "";

    if (special == SPECIAL_STUN) {
      return " (STUN)";
    }
    if (special == SPECIAL_KNOCKBACK)
    return " (KNOCKBACK)";
    if (special == SPECIAL_ENTRAP)
    return " (SNARE)";
    if (special == SPECIAL_GRAPPLE)
    return " (SNARE)";
    if (special == SPECIAL_LIFESTEAL)
    return " (LIFESTEAL)";
    if (special == SPECIAL_OVERPOWER)
    return " (OVERPOWER)";
    if (special == SPECIAL_DAMAGE)
    return " (DAMAGE)";
    if (special == SPECIAL_WEAKEN)
    return " (WEAKEN)";
    if (special == SPECIAL_UNDERPOWER)
    return " (UNDERPOWER)";
    if (special == SPECIAL_LONGWEAKEN)
    return " (WEAKEN)";
    if (special == SPECIAL_SLOW)
    return " (SLOW)";
    if (special == SPECIAL_AOE)
    return " (AOE)";

    return "";
  }

  CHAR_DATA *trickcheck(CHAR_DATA *ch) {
    if (!has_caff(ch, CAFF_TRICKING))
    return ch;

    CHAR_DATA *newvict;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (!in_fight(newvict))
      continue;
      if (newvict == NULL || is_gm(newvict))
      continue;
      if (!same_fight(newvict, ch))
      continue;

      if (IS_FLAG(newvict->act, ACT_COMBATOBJ) && IS_NPC(newvict))
      continue;

      if (ch == newvict)
      continue;

      if (number_percent() % 3 == 0)
      return newvict;
    }
    return ch;
  }

  _DOFUN(do_attack) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];
    char buf[MSL];
    int vnum = 0, dam, i, point, opoint, shadow = -1;
    CHAR_DATA *victim;
    ch->cfighting = NULL;
    bool protecting = FALSE;

    if (is_dreaming(ch) && in_fantasy(ch) != NULL) {
      sprintf(buf, "attack %s", argument);
      do_function(ch, &do_dreamworld, buf);
      return;
    }

    if (ch->in_room->vnum == 60)
    return;

    ROOM_INDEX_DATA *room = ch->in_room;

    if (is_gm(ch)) {
      do_function(ch, &do_gmattack, argument);
      return;
    }

    if (!IS_NPC(ch) && is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && !in_fight(ch) && (ch->pcdata->patrol_status == PATROL_PREYING || ch->pcdata->patrol_status == PATROL_CHASING || ch->pcdata->patrol_status == PATROL_SEARCHING || ch->pcdata->patrol_status == PATROL_GRABBING)) {
      send_to_char("You'd have to lay hands on them first.\n\r", ch);
      return;
    }

    if(!IS_NPC(ch) && !in_fight(ch) && (ch->pcdata->patrol_status == PATROL_PREY || ch->pcdata->patrol_status == PATROL_FLEEING || ch->pcdata->patrol_status == PATROL_HIDING || ch->pcdata->patrol_status == PATROL_GRABBED))
    {
      send_to_char("You're in no position to do that.\n\r", ch);
      ch->preyvictim = TRUE;
      return;
    }


    if (in_fight(ch) == FALSE) {
      /*
if(fight_room_pop(ch->in_room) < 2)
{
send_to_char("There's nobody here for you to fight.\n\r", ch);
return;
}
*/
      if (!is_gm(ch)) {
        if ((victim = get_char_fight(ch, argument)) == NULL || victim == ch) {
          victim = get_char_distance(ch, argument, 4);
          if (victim == NULL || !is_car(victim)) {
            send_to_char("Attack who?\n\r", ch);
            return;
          }
        }
        if (ch->pcdata->spectre == 2) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }
        if (is_griefer(ch)) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }
        if (!IS_NPC(victim) && victim->pcdata->spectre == 2) {
          take_lifeforce(victim, 1000, "Spectre attacked.");
          act("You drive $N from the nightmare.", ch, NULL, victim, TO_CHAR);
          act("$n drives you from the nightmare.", ch, NULL, victim, TO_VICT);
          act("$n drives $N from the nightmare.", ch, NULL, victim, TO_NOTVICT);
          wake_char(victim);
          send_to_char("You wake up.\n\r", ch);
          act("$n stirs and awakens.", ch, NULL, NULL, TO_ROOM);
          return;
        }
        if (victim->wounds >= 4)
        return;
        if (is_prisoner(ch)) {
          send_to_char("You can't start a fight in here.\n\r", ch);
          return;
        }

        if (is_car(victim)) {
          if (public_room(victim->in_room)) {
            send_to_char("This area is too public to start a fight.\n\r", ch);
            return;
          }
          if (IS_FLAG(ch->act, PLR_DEAD)) {
            send_to_char("Ghosts are incapable of engaging or being engaged in combat.\n\r", ch);
            return;
          }
          if (IS_FLAG(ch->act, PLR_HIDE))
          REMOVE_FLAG(ch->act, PLR_HIDE);
          carattack(ch, victim);
          return;
        }

        if (in_public(ch, victim)) {
          if (!IS_FLAG(ch->act, PLR_SHROUD)) {
            send_to_char("This area is too public to start a fight.\n\r", ch);
            return;
          }
        }
        if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD)) {
          send_to_char("They're in the nightmare.\n\r", ch);
          return;
        }
        if (!IS_FLAG(victim->act, PLR_SHROUD) && IS_FLAG(ch->act, PLR_SHROUD)) {
          send_to_char("You're in the nightmare.\n\r", ch);
          return;
        }

        if (is_safe(ch, victim)) {
          send_to_char("You can't bring yourself to do that.\n\r", ch);
          return;
        }
      }
      // prevents ghosts from fighting - Discordance
      if (IS_FLAG(ch->act, PLR_DEAD) || IS_FLAG(victim->act, PLR_DEAD)) {
        send_to_char("Ghosts are incapable of engaging or being engaged in combat.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->act, PLR_HIDE)) {
        REMOVE_FLAG(ch->act, PLR_HIDE);
      }
      act("`rYou attack $N.`x", ch, NULL, victim, TO_CHAR);
      start_fight(ch, victim);
      return;
    }

    if (IS_FLAG(ch->act, PLR_HIDE)) {
      REMOVE_FLAG(ch->act, PLR_HIDE);
    }

    if (ch->attacking == 0)
    ch->attacking = 1;

    ch->specialpoint = -1;

    if (ch->fight_fast == FALSE) {
      argument = one_argument_nouncap(argument, arg1);
      argument = one_argument_nouncap(argument, arg2);

      if (ch->fight_current != ch) {
        send_to_char("It's not your turn yet.\n\r", ch);
        return;
      }
      if (fight_over(room)) {
        end_fight(room);
        return;
      }
      if (IS_FLAG(ch->fightflag, FIGHT_DEFEND))
      REMOVE_FLAG(ch->fightflag, FIGHT_DEFEND);

      if (ch->wounds >= 2)
      ch->heal_timer += 1500;

      if (!str_cmp(arg1, "skip")) {
        if (ch->moving == TRUE) {
          ch->actiontimer = 600;
          printf_to_char(ch, "You have %d minutes to write your movement emote.\n\r", ch->actiontimer / 60);
        }
        else {
          send_to_char("Attack skipped.\n\r", ch);
          act("$n skips $s chance to attack.", ch, NULL, NULL, TO_ROOM);

          if (IS_FLAG(ch->fightflag, FIGHT_NOMOVE))
          REMOVE_FLAG(ch->fightflag, FIGHT_NOMOVE);
          if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK))
          REMOVE_FLAG(ch->fightflag, FIGHT_NOATTACK);
          if (IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER))
          REMOVE_FLAG(ch->fightflag, FIGHT_UNDERPOWER);

          if (has_caff(ch, CAFF_STUNNED))
          lower_caff(ch, CAFF_STUNNED);
          if (has_caff(ch, CAFF_ROOT))
          lower_caff(ch, CAFF_ROOT);

          for (int i = 0; i < 30; i++) {
            if (ch->caff_duration[i] > 0) {
              ch->caff_duration[i] -= (FIGHT_WAIT * fight_speed(ch) * 3 / 2);
              if (ch->caff_duration[i] <= 0 && ch->caff[i] == CAFF_RESISTPAIN)
              ch->hit = UMAX(0, ch->hit - 100);
            }
          }
          ch->attacking = 0;
          next_attacker(ch, FALSE);
          return;
        }
      }

      if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK)) {
        send_to_char("You can't attack this round.\n\r", ch);
        return;
      }
      if (has_caff(ch, CAFF_STUNNED)) {
        send_to_char("You can't attack this round.\n\r", ch);
        return;
      }

      if (!str_cmp(arg1, "defend")) {
        if (!IS_FLAG(ch->fightflag, FIGHT_DEFEND))
        SET_FLAG(ch->fightflag, FIGHT_DEFEND);

        if (ch->moving == TRUE) {
          ch->actiontimer = 600;
          printf_to_char(ch, "You have %d minutes to write your moving emote.\n\r", ch->actiontimer / 60);

        }
        else {
          send_to_char("You defend.\n\r", ch);
          act("$n defends $mself.", ch, NULL, NULL, TO_ROOM);

          if (IS_FLAG(ch->fightflag, FIGHT_NOMOVE))
          REMOVE_FLAG(ch->fightflag, FIGHT_NOMOVE);
          if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK))
          REMOVE_FLAG(ch->fightflag, FIGHT_NOATTACK);

          if (IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER))
          REMOVE_FLAG(ch->fightflag, FIGHT_UNDERPOWER);

          if (has_caff(ch, CAFF_STUNNED))
          lower_caff(ch, CAFF_STUNNED);
          if (has_caff(ch, CAFF_ROOT))
          lower_caff(ch, CAFF_ROOT);

          for (int i = 0; i < 30; i++) {
            if (ch->caff_duration[i] > 0) {
              ch->caff_duration[i] -= (FIGHT_WAIT * fight_speed(ch) * 3 / 2);
              if (ch->caff_duration[i] <= 0 && ch->caff[i] == CAFF_RESISTPAIN)
              ch->hit = UMAX(0, ch->hit - 100);
            }
          }

          next_attacker(ch, FALSE);
          return;
        }
      }
      if (IS_FLAG(ch->fightflag, FIGHT_DEFEND))
      REMOVE_FLAG(ch->fightflag, FIGHT_DEFEND);

      if ((victim = get_char_fight(ch, arg1)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      if (victim->wounds >= 4) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD)) {
        send_to_char("They're not in the nightmare.\n\r", ch);
        return;
      }
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD)) {
        send_to_char("They're in the nightmare.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK)) {
        send_to_char("You can't attack this round.\n\r", ch);
        return;
      }
      if (has_caff(ch, CAFF_STUNNED)) {
        send_to_char("You can't attack this round.\n\r", ch);
        return;
      }

      if (is_safe(ch, victim)) {
        send_to_char("You just can't bring yourself to attack them.\n\r", ch);
        return;
      }
      if (IS_NPC(victim) && IS_FLAG(victim->act, ACT_COMBATOBJ)) {
        send_to_char("You can't attack that.\n\r", ch);
        return;
      }
      if (!can_map_see(ch, victim)) {
        send_to_char("Attack who?\n\r", ch);
        return;
      }

      if (ch->attackdam > 0 && ch->actiontimer > 0) {
        send_to_char("You've already put in an attack this round, either input your move or write your combat emote.\n\r", ch);
        return;
      }

      if (!IS_NPC(victim) && !is_sparring(ch) && !battleground(ch->in_room)) {
        if (ch->faction != 0 && ch->faction != victim->faction && ch->factiontwo != victim->faction && victim->factiontwo != ch->faction && ch->factiontwo != victim->factiontwo && victim->race != RACE_CIVILIAN) {
          give_respect(ch, 200, "Doing Damage.", ch->faction);
          give_respect_noecho(ch, 200, ch->factiontwo);
        }
        if (ch->faction != 0 && ch->faction == victim->faction)
        give_respect(ch, -20, "Doing damage sam fac.", ch->faction);
      }
      if (ch != victim && victim != NULL)
      ch->cfighting = victim;
      if (has_caff(ch, CAFF_CONFUSE)) {
        CHAR_DATA *newvict;
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          newvict = *it;
          if (!in_fight(newvict))
          continue;
          if (newvict == NULL || is_gm(newvict))
          continue;
          if (!same_fight(newvict, ch))
          continue;

          if (IS_FLAG(newvict->act, ACT_COMBATOBJ) && IS_NPC(newvict))
          continue;
          if (ch == newvict)
          continue;

          if (victim == newvict)
          continue;
          if (combat_distance(newvict, ch, FALSE) >
              combat_distance(victim, ch, FALSE))
          continue;
          if (number_percent() % 2 == 0)
          victim = newvict;
        }
      }
      if (has_caff(victim, CAFF_MIMIC) && victim->mimic != NULL && victim->mimic->in_room != NULL && victim->mimic != ch) {
        victim = victim->mimic;
      }

      if (has_caff(ch, CAFF_COMMANDED) && ch->commanded != NULL && ch->commanded->in_room != NULL && ch->commanded->in_room == ch->in_room) {
        victim = ch->commanded;
        lower_caff(ch, CAFF_COMMANDED);
      }

      if (is_protected(victim) && get_protector(victim) != ch && !is_in_cover(victim)) {
        CHAR_DATA *temp;
        temp = get_protector(victim);
        victim = temp;
        protecting = TRUE;
      }

      CHAR_DATA *cobj;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        cobj = *it;

        if (cobj == NULL)
        continue;

        if (cobj->in_room == NULL)
        continue;

        if (!IS_FLAG(cobj->act, ACT_COMBATOBJ) || !IS_NPC(cobj))
        continue;

        if (cobj->pIndexData->vnum == COBJ_TEAR) {
          if (get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 15) {
            if (!has_gasmask(ch) && !is_undead(ch)) {
              apply_caff(ch, CAFF_TEAR, 1);
              act("You breathe in a lungful of tear gas.", ch, NULL, NULL, TO_CHAR);
            }
          }
        }
      }

      ch->fight_attacks++;
      point = -1;
      int cpoint = -1;
      int j = 0;
      if (!IS_NPC(ch)) {
        if ((!str_cmp(arg2, "knockout") || ((IS_FLAG(ch->comm, COMM_SUBDUE)) && !IS_NPC(victim) && !IS_NPC(ch) && victim->hit <= 0))) {
          if (guestmonster(ch)) {
            send_to_char("You know no mercy.\n\r", ch);
            return;
          }
          if (combat_distance(ch, victim, TRUE) > 1) {
            send_to_char("You're too far away.\n\r", ch);
            return;
          }
          if (victim->hit > 0) {
            send_to_char("They're too well defended for that.\n\r", ch);
            return;
          }
          if (IS_NPC(victim)) {
            send_to_char("Only PCs can be knocked out.\n\r", ch);
            return;
          }
          if (IS_FLAG(ch->fightflag, FIGHT_KNOCKFAIL) || IS_FLAG(ch->fightflag, FIGHT_KNOCKOUT)) {
            send_to_char("You're already trying to do that.\n\r", ch);
            return;
          }
          int knockresult = 0;

          if (is_abom(ch) && has_blackeyes(ch) && !is_abom(victim))
          ch->knockfails++;

          if (get_skill(victim, SKILL_IMMORTALITY) < 1) {
            if (number_percent() % 2 == 0 || ch->knockfails >= 2)
            knockresult = 1;
            else
            knockresult = 0;
          }
          else {
            if (ch->knockfails >= 3)
            knockresult = 1;
            else
            knockresult = 0;
          }

          if (knockresult == 1 && !guestmonster(victim)) {
            if (!IS_FLAG(ch->fightflag, FIGHT_KNOCKOUT))
            SET_FLAG(ch->fightflag, FIGHT_KNOCKOUT);
            if (IS_FLAG(ch->fightflag, FIGHT_KNOCKFAIL))
            REMOVE_FLAG(ch->fightflag, FIGHT_KNOCKFAIL);
            ch->attackdam = 0;
            ch->attacking = 1;
            ch->chattacking = victim;
            ch->actiontimer = 600;
            act("Your attack will knock $N out.", ch, NULL, victim, TO_CHAR);
            printf_to_char(
            ch, "You have %d minutes to write your attacking emote.\n\r", ch->actiontimer / 60);
          }
          else {
            if (!IS_FLAG(ch->fightflag, FIGHT_KNOCKFAIL))
            SET_FLAG(ch->fightflag, FIGHT_KNOCKFAIL);
            if (IS_FLAG(ch->fightflag, FIGHT_KNOCKOUT))
            REMOVE_FLAG(ch->fightflag, FIGHT_KNOCKOUT);

            ch->attackdam = 0;
            ch->attacking = 1;
            ch->knockfails++;
            ch->chattacking = victim;
            ch->actiontimer = 600;
            act("Your attack will fail to knock $N out.", ch, NULL, victim, TO_CHAR);
            printf_to_char(
            ch, "You have %d minutes to write your attacking emote.\n\r", ch->actiontimer / 60);
          }
          return;
        }
        for (i = 0; i < 25; i++) {
          if (!str_cmp(arg2, ch->pcdata->customstrings[i][0]) && safe_strlen(arg2) >
              1) // && strstr(ch->pcdata->customstrings[i][2], "$N") != NULL
          // && strstr(ch->pcdata->customstrings[i][2], "$n") != NULL)
          {
            for (j = 0; j < DIS_USED; j++) {
              if (discipline_table[j].vnum == ch->pcdata->customstats[i][1] && disc_range(ch, j) > -1) {
                point = j;
                cpoint = i;
              }
            }
          }
        }
      }
      if (cpoint != -1) {
        shadow = get_shadow(ch, cpoint);
      }
      if (point == -1) {
        for (i = 0; i < DIS_USED; i++) {
          if (!str_cmp(arg2, disc_name(ch, i)))
          point = i;
        }
        if (point == -1) {
          argument = one_argument_nouncap(argument, arg3);
          sprintf(buf, "%s %s", arg2, arg3);
          for (i = 0; i < DIS_USED; i++) {
            if (!str_cmp(buf, disc_name(ch, i)) && safe_strlen(buf) > 2)
            point = i;
          }
        }
        if (point == -1) {
          argument = one_argument_nouncap(argument, arg4);
          sprintf(buf, "%s %s", buf, arg4);
          for (i = 0; i < DIS_USED; i++) {
            if (!str_cmp(buf, disc_name(ch, i)) && safe_strlen(buf) > 2)
            point = i;
          }
        }
        if (point == -1) {
          if (combat_distance(ch, victim, TRUE) <= 1)
          vnum = default_melee(ch);
          else
          vnum = default_ranged(ch);

          for (i = 0; i < DIS_USED; i++) {
            if (discipline_table[i].vnum == vnum)
            point = i;
          }
        }
      }
      if (point < 0 || point > 10000) {
        send_to_char("No such discipline.\n\r", ch);
        return;
      }
      if (!str_cmp(ch->name, "monkey"))
      printf_to_char(ch, "Point %d, Dist: %d, Range: %d  ", point, combat_distance(ch, victim, TRUE), disc_range(ch, point));

      if (combat_distance(ch, victim, TRUE) > 1 && combat_distance(ch, victim, TRUE) > disc_range(ch, point)) {
        send_to_char("They're out of range of that discipline.\n\r", ch);
        return;
      }
      if (combat_distance(ch, victim, TRUE) <= 1 && disc_range(ch, point) > 1) {
        send_to_char("They're too close for that discipline.\n\r", ch);
        return;
      }
      int range = disc_range(ch, point);
      if (get_disc(ch, discipline_table[point].vnum, FALSE) < 1) {
        send_to_char("You can't use that discipline.\n\r", ch);
        return;
      }
      int shield = shield_type(victim);
      dam = damage_calculate(ch, victim, point, shield);
      if (protecting == TRUE && !IS_NPC(victim) && get_skill(victim, SKILL_GUARDIAN) > 0)
      dam = dam * 2 / 3;
      if (has_caff(ch, CAFF_REFLECT) && ch->reflect == victim) {
        if (discipline_table[point].vnum == DIS_SHOTGUNS || discipline_table[point].vnum == DIS_RIFLES || discipline_table[point].vnum == DIS_CARBINES || discipline_table[point].vnum == DIS_BOWS || discipline_table[point].vnum == DIS_CARBINES || discipline_table[point].vnum == DIS_THROWN || discipline_table[point].vnum == DIS_PISTOLS) {
          victim = ch;
          lower_caff(ch, CAFF_REFLECT);
        }
      }

      opoint = point;
      ch->chattacking = victim;
      ch->attackdisc = discipline_table[point].vnum;
      ch->attackdam = dam;
      ch->defensedisc = shield;
      victim->wound_location = number_percent();

      if (range < 2 && ch->in_room != victim->in_room) {
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
      }
      for (i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == ch->defensedisc)
        point = i;
      }

      if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN)) {
        dam = dam / 2;
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN);
      }
      if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN1)) {
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN1);
        SET_FLAG(ch->fightflag, FIGHT_WEAKEN2);
        dam = dam * 3 / 4;
      }
      else if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN2)) {
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN2);
        SET_FLAG(ch->fightflag, FIGHT_WEAKEN3);
        dam = dam * 3 / 4;
      }
      else if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN3)) {
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN3);
        dam = dam * 3 / 4;
      }

      if (IS_FLAG(ch->fightflag, FIGHT_AOE)) {
        dam = dam * 2 / 5;
      }
      if (IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER)) {
        if (ch->underpowering == victim)
        dam = dam * 150 / 100;
        else
        dam = dam * 120/100;

        REMOVE_FLAG(ch->fightflag, FIGHT_UNDERPOWER);
      }

      bool overpowering = FALSE;
      if (IS_FLAG(ch->fightflag, FIGHT_OVERPOWER))
      overpowering = TRUE;
      bool delaying = FALSE;
      if (cpoint != -1) {
        dam = process_special(ch, victim, dam, cpoint);
        ch->specialpoint = cpoint;
        if (ch->pcdata->customstats[cpoint][0] == SPECIAL_DELAY || ch->pcdata->customstats[cpoint][0] == SPECIAL_DELAY2)
        delaying = TRUE;

      }
      else
      ch->specialpoint = -1;

      if (IS_FLAG(ch->fightflag, FIGHT_OVERPOWER) && !delaying && overpowering) {
        dam = dam * 2 / 5;
        REMOVE_FLAG(ch->fightflag, FIGHT_OVERPOWER);
      }
      if (shadow != -1) {
        ch->shadowdisc = discipline_table[shadow].vnum;
        opoint = shadow;
      }

      int echo =
      damage_echo(ch, victim, discipline_table[opoint].vnum, ch->shadowdisc, dam, combat_distance(ch, victim, TRUE), FALSE);
      ch->attackdam = dam;
      if (dam <= victim->hit) {
        sprintf(buf, "Your best %s attack against $N will %s", disc_name(ch, opoint), defense_message(victim, TO_CHAR, FALSE, echo));
        act(buf, ch, NULL, victim, TO_CHAR);
      }
      else {
        if (IS_NPC(victim) && victim->wounds == 0)
        printf_to_char(
        ch, "Your best %s attack will severely wound %s's %s.\n\r", disc_name(ch, opoint), PERS(victim, ch), wound_location(victim));
        else if (victim->wounds == 0)
        printf_to_char(ch, "Your best %s attack will mildly wound %s's %s.\n\r", disc_name(ch, opoint), PERS(victim, ch), wound_location(victim));
        else if (victim->wounds == 1 && (sparring_conditions(ch, victim) || under_understanding(victim, ch)))
        printf_to_char(ch, "Your best %s attack will mildly wound %s's %s.\n\r", disc_name(ch, opoint), PERS(victim, ch), wound_location(victim));
        else if (victim->wounds == 1)
        printf_to_char(
        ch, "Your best %s attack will severely wound %s's %s.\n\r", disc_name(ch, opoint), PERS(victim, ch), wound_location(victim));
        else if (victim->wounds == 2 && !IS_NPC(victim))
        printf_to_char(ch, "Your best %s attack will critically wound %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
        else
        printf_to_char(ch, "Your best %s attack will kill %s.\n\r", disc_name(ch, opoint), PERS(victim, ch));
      }
      ch->attackdam = dam;
      ch->attacking = 1;
      ch->actiontimer = 600;
      printf_to_char(ch, "You have %d minutes to write your attacking emote.\n\r", ch->actiontimer / 60);
      return;
    }
    else {
      if (ch->attack_timer > 0) {
        free_string(ch->amove);
        ch->amove = str_dup(argument);
        send_to_char("Command stacked.\n\r", ch);
        return;
      }
      if (IS_FLAG(ch->comm, COMM_AUTOFIRE)) {
        free_string(ch->amove);
        ch->amove = str_dup(argument);
      }
      argument = one_argument_nouncap(argument, arg1);
      argument = one_argument_nouncap(argument, arg2);

      if (!IS_FLAG(ch->comm, COMM_AUTOFIRE)) {
        free_string(ch->amove);
        ch->amove = str_dup("");
      }

      if (IS_FLAG(ch->fightflag, FIGHT_DEFEND))
      REMOVE_FLAG(ch->fightflag, FIGHT_DEFEND);

      if (!str_cmp(arg1, "capture")) {
        if (capture_attack(ch))
        return;
      }
      if (!str_cmp(arg1, "skip")) {
        send_to_char("Attack skipped.\n\r", ch);
        act("$n skips $s chance to attack.", ch, NULL, NULL, TO_ROOM);
        ch->attacking = 0;
        free_string(ch->amove);
        ch->amove = str_dup("");

        return;
      }
      if (!str_cmp(arg1, "defend")) {
        if (!IS_FLAG(ch->fightflag, FIGHT_DEFEND))
        SET_FLAG(ch->fightflag, FIGHT_DEFEND);

        send_to_char("You defend.\n\r", ch);
        if (will_fight_show(ch, FALSE) && !IS_NPC(ch))
        act("$n defends.", ch, NULL, NULL, TO_ROOM);
        else if (!IS_NPC(ch))
        pact("$n defends.", ch, NULL, NULL, ch->x, ch->y, 25, 0, 0);

        ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
        free_string(ch->amove);
        ch->amove = str_dup("");

        return;
      }
      if ((victim = get_char_fight(ch, arg1)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        free_string(ch->amove);
        ch->amove = str_dup("");

        return;
      }

      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD)) {
        send_to_char("They're not in the nightmare.\n\r", ch);
        free_string(ch->amove);
        ch->amove = str_dup("");

        return;
      }
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD)) {
        send_to_char("They're in the nightmare.\n\r", ch);
        free_string(ch->amove);
        ch->amove = str_dup("");

        return;
      }

      if (IS_NPC(victim) && IS_FLAG(victim->act, ACT_COMBATOBJ)) {
        send_to_char("You can't attack that.\n\r", ch);
        return;
      }
      /*
if(!can_map_see(ch, victim))
{
send_to_char("Attack who?\n\r", ch);
return;
}
*/

      if (!IS_NPC(victim) && !is_sparring(ch) && !battleground(ch->in_room)) {
        if (ch->faction != 0 && ch->faction != victim->faction && victim->race != RACE_CIVILIAN)
        give_respect(ch, 100, "Doing Damage", ch->faction);
        if (ch->faction != 0 && ch->faction == victim->faction)
        give_respect(ch, -20, "Doing Damage Sam Fac.", ch->faction);
      }
      if (battleground(ch->in_room) && ch->faction == victim->faction)
      give_respect(ch, -100, "Faction battleground same fac.", ch->faction);

      if (ch != victim && victim != NULL)
      ch->cfighting = victim;
      if (has_caff(ch, CAFF_CONFUSE)) {
        CHAR_DATA *newvict;
        for (CharList::iterator it = char_list.begin(); it != char_list.end();
        ++it) {
          newvict = *it;
          if (!in_fight(newvict))
          continue;
          if (newvict == NULL || is_gm(newvict))
          continue;
          if (!same_fight(newvict, ch))
          continue;

          if (IS_FLAG(newvict->act, ACT_COMBATOBJ) && IS_NPC(newvict))
          continue;

          if (ch == newvict)
          continue;

          if (victim == newvict)
          continue;

          if (combat_distance(newvict, ch, FALSE) >
              combat_distance(victim, ch, FALSE))
          continue;
          if (number_percent() % 2 == 0)
          victim = newvict;
        }
      }
      if (has_caff(victim, CAFF_MIMIC) && victim->mimic != NULL && victim->mimic->in_room != NULL && victim->mimic != ch) {
        victim = victim->mimic;
      }
      if (has_caff(ch, CAFF_COMMANDED) && ch->commanded != NULL && ch->commanded->in_room != NULL && ch->commanded->in_room == ch->in_room) {
        victim = ch->commanded;
        lower_caff(ch, CAFF_COMMANDED);
      }

      if (is_protected(victim) && get_protector(victim) != ch && !is_in_cover(victim)) {
        CHAR_DATA *temp;
        temp = get_protector(victim);
        victim = temp;
        protecting = TRUE;
      }

      CHAR_DATA *cobj;

      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end(); ++it) {
        cobj = *it;

        if (cobj == NULL)
        continue;

        if (cobj->in_room == NULL)
        continue;

        if (!IS_FLAG(cobj->act, ACT_COMBATOBJ) || !IS_NPC(cobj))
        continue;

        if (cobj->pIndexData->vnum == COBJ_TEAR) {
          if (get_dist(ch->x, ch->y, cobj->x, cobj->y) <= 15) {
            if (!has_gasmask(ch) && !is_undead(ch)) {
              apply_caff(ch, CAFF_TEAR, 1);
              act("You breathe in a lungful of tear gas.", ch, NULL, NULL, TO_CHAR);
            }
          }
        }
      }

      delayattacks(ch, victim);

      if (victim->wounds >= 4)
      return;

      point = -1;
      int cpoint = -1;
      int j = 0;
      if (!IS_NPC(ch)) {
        if (!battleground(ch->in_room) && !guestmonster(ch) && (!str_cmp(arg2, "knockout") || (IS_FLAG(ch->comm, COMM_SUBDUE) && !IS_NPC(victim) && !is_sparring(ch) && victim->hit <= 0))) {
          if (combat_distance(ch, victim, TRUE) > 1) {
            send_to_char("You're too far away.\n\r", ch);
            free_string(ch->amove);
            ch->amove = str_dup("");

            return;
          }
          if (victim->hit > 0) {
            send_to_char("They're too well defended for that.\n\r", ch);
            free_string(ch->amove);
            ch->amove = str_dup("");

            return;
          }
          if (IS_NPC(victim)) {
            send_to_char("Only PCs can be knocked out.\n\r", ch);
            free_string(ch->amove);
            ch->amove = str_dup("");

            return;
          }
          if (number_percent() % 2 == 0 && !guestmonster(victim)) {
            act("You knock $N out.\n\r", ch, NULL, victim, TO_CHAR);
            act("$n knocks you out.\n\r", ch, NULL, victim, TO_VICT);
            act("$n knocks $N out.", ch, NULL, victim, TO_NOTVICT);
            ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
            if (!IS_NPC(ch))
            ch->pcdata->fatigue += HIT_FATIGUE;

            victim->pcdata->sleeping = 400;
            if (IS_FLAG(victim->fightflag, FIGHT_DEADSWITCH))
            dead_explode(victim);
            sprintf(buf, "%s has knocked %s out at %s.", ch->name, victim->name, ch->in_room->name);

            victim->fighting = FALSE;
            if (fight_over(room)) {
              end_fight(room);
              return;
            }
          }
          else {
            act("You attempt to knock $N out.\n\r", ch, NULL, victim, TO_CHAR);
            act("$n attempts to knock you out.\n\r", ch, NULL, victim, TO_VICT);
            act("$n attempts to knock $N out.", ch, NULL, victim, TO_NOTVICT);
            ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
            if (!IS_NPC(ch))
            ch->pcdata->fatigue += HIT_FATIGUE;
            dam = 0;
          }
        }

        if (victim->wounds >= 4)
        return;

        for (i = 0; i < 25; i++) {
          if (!str_cmp(arg2, ch->pcdata->customstrings[i][0]) && safe_strlen(arg2) >
              1) // && strstr(ch->pcdata->customstrings[i][2], "$N") != NULL
          // && strstr(ch->pcdata->customstrings[i][2], "$n") != NULL)
          {
            if ((strstr(ch->pcdata->customstrings[i][2], "\$N") == NULL && strcasestr(ch->pcdata->customstrings[i][2], "you") == NULL) || strstr(ch->pcdata->customstrings[i][2], "\$n") == NULL || strstr(ch->pcdata->customstrings[i][3], "\$N") == NULL || strstr(ch->pcdata->customstrings[i][3], "\$n") == NULL) {
              if (ch->pcdata->customstats[i][0] != SPECIAL_ALLY && ch->pcdata->customstats[i][0] != SPECIAL_MINION) {
                char buf[MSL];
                sprintf(buf, "BADCUSTOM: %s", ch->pcdata->customstrings[i][2]);
                log_string(buf);
                send_to_char("Invalid custom, they must include both $n and $N in the message.\n\r", ch);
                return;
              }
            }
            if (safe_strlen(from_color(ch->pcdata->customstrings[i][3])) > 200) {
              send_to_char("Invalid custom, message too long.\n\r", ch);
              return;
            }
            for (j = 0; j < DIS_USED; j++) {
              if (discipline_table[j].vnum == ch->pcdata->customstats[i][1] && disc_range(ch, j) > -1) {
                point = j;
                cpoint = i;
              }
            }
          }
        }
      }
      if (point == -1) {
        if(ch->played/3600 > 100 && dam > 5)
        dam = dam * 9/10;

        for (i = 0; i < DIS_USED; i++) {
          if (!str_cmp(arg2, disc_name(ch, i)) && safe_strlen(arg2) > 2)
          point = i;
        }
        if (point == -1) {
          argument = one_argument_nouncap(argument, arg3);
          sprintf(buf, "%s %s", arg2, arg3);
          for (i = 0; i < DIS_USED; i++) {
            if (!str_cmp(buf, disc_name(ch, i)) && safe_strlen(buf) > 2)
            point = i;
          }
        }
        if (point == -1) {
          argument = one_argument_nouncap(argument, arg4);
          sprintf(buf, "%s %s", buf, arg4);
          for (i = 0; i < DIS_USED; i++) {
            if (!str_cmp(buf, disc_name(ch, i)) && safe_strlen(buf) > 2)
            point = i;
          }
        }
        if (point == -1) {
          if (combat_distance(ch, victim, FALSE) <= 1)
          vnum = default_melee(ch);
          else
          vnum = default_ranged(ch);

          for (i = 0; i < DIS_USED; i++) {
            if (discipline_table[i].vnum == vnum)
            point = i;
          }
        }
      }
      if (point < 0 || point > 10000) {
        send_to_char("No such discipline.\n\r", ch);
        return;
      }

      if (combat_distance(ch, victim, TRUE) > 1 && combat_distance(ch, victim, TRUE) > disc_range(ch, point)) {
        if (!str_cmp(ch->name, "monkey"))
        printf_to_char(ch, "D:%d, R:%d, P:%d, V%d-%d\n\r", combat_distance(ch, victim, TRUE), disc_range(ch, point), point, vnum, discipline_table[point].vnum);
        send_to_char("They're out of range of that discipline.\n\r", ch);
        free_string(ch->amove);
        ch->amove = str_dup("");

        return;
      }
      if (combat_distance(ch, victim, TRUE) <= 1 && disc_range(ch, point) > 1) {
        send_to_char("They're too close for that discipline.\n\r", ch);
        free_string(ch->amove);
        ch->amove = str_dup("");
        return;
      }
      int range = disc_range(ch, point);
      int shield = shield_type(victim);
      dam = damage_calculate(ch, victim, point, shield);

      if (has_caff(ch, CAFF_REFLECT) && ch->reflect == victim) {
        if (discipline_table[point].vnum == DIS_SHOTGUNS || discipline_table[point].vnum == DIS_RIFLES || discipline_table[point].vnum == DIS_CARBINES || discipline_table[point].vnum == DIS_BOWS || discipline_table[point].vnum == DIS_THROWN || discipline_table[point].vnum == DIS_PISTOLS) {
          victim = ch;
          lower_caff(ch, CAFF_REFLECT);
        }
      }
      if (protecting == TRUE && !IS_NPC(victim) && get_skill(victim, SKILL_GUARDIAN) > 0)
      dam = dam * 2 / 3;

      if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN)) {
        dam = dam / 2;
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN);
      }

      if (IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER)) {
        if (ch->underpowering == victim)
        {
          dam = dam * 150 / 100;
        }
        else
        dam = dam * 120 / 100;

        REMOVE_FLAG(ch->fightflag, FIGHT_UNDERPOWER);
      }
      if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN1)) {
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN1);
        SET_FLAG(ch->fightflag, FIGHT_WEAKEN2);
        dam = dam * 3 / 4;
      }
      else if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN2)) {
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN2);
        SET_FLAG(ch->fightflag, FIGHT_WEAKEN3);
        dam = dam * 3 / 4;
      }
      else if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN3)) {
        REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN3);
        dam = dam * 3 / 4;
      }

      if (IS_FLAG(ch->fightflag, FIGHT_AOE)) {
        dam = dam * 2 / 5;
      }
      bool overpowering = FALSE;
      if (IS_FLAG(ch->fightflag, FIGHT_OVERPOWER))
      overpowering = TRUE;

      bool delaying = FALSE;
      char *special_mes;
      special_mes = str_dup("");
      if (cpoint != -1) {
        free_string(special_mes);
        special_mes = str_dup(special_message(ch, cpoint));
        dam = process_special(ch, victim, dam, cpoint);
        if (ch->pcdata->customstats[cpoint][0] == SPECIAL_DELAY || ch->pcdata->customstats[cpoint][0] == SPECIAL_DELAY2)
        delaying = TRUE;
      }

      if (IS_FLAG(ch->fightflag, FIGHT_OVERPOWER) && !delaying && overpowering) {
        dam = dam * 2 / 5;
        REMOVE_FLAG(ch->fightflag, FIGHT_OVERPOWER);
      }
      if (range < 2 && ch->in_room != victim->in_room) {
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
      }

      if (!IS_NPC(ch) && !IS_NPC(victim) && !battleground(ch->in_room) && !guestmonster(ch)) {
        if (((IS_FLAG(ch->comm, COMM_SUBDUE)) && !IS_NPC(victim) && !IS_NPC(ch) && victim->hit <= 0) && !is_sparring(ch)) {
          if (combat_distance(ch, victim, TRUE) > 1) {
            send_to_char("You're too far away.\n\r", ch);
            return;
          }
          if (victim->hit > 0) {
            send_to_char("They're too well defended for that.\n\r", ch);
            return;
          }
          if (IS_NPC(victim)) {
            send_to_char("Only PCs can be knocked out.\n\r", ch);
            return;
          }
          if ((number_percent() % 2 == 0 || ch->knockfails >= 2) && !guestmonster(victim)) {
            act("(You knock $N out.)", ch, NULL, victim, TO_CHAR);
            act("($n knocks you out.)", trickcheck(ch), NULL, victim, TO_VICT);
            act("($n knocks $N out.)", trickcheck(ch), NULL, victim, TO_NOTVICT);

            victim->pcdata->sleeping = 400;
            if (IS_FLAG(victim->fightflag, FIGHT_DEADSWITCH))
            dead_explode(victim);
            victim->fighting = FALSE;
            victim->attacking = 0;
            if (fight_over(ch->in_room)) {
              end_fight(ch->in_room);
              act("The fight ends.", ch, NULL, NULL, TO_CHAR);
              act("The fight ends.", ch, NULL, NULL, TO_ROOM);
              return;
            }

          }
          else {
            ch->knockfails++;
            act("(You attempt to knock $N out.)", ch, NULL, victim, TO_CHAR);
            act("($n attempts to knock you out.)", trickcheck(ch), NULL, victim, TO_VICT);
            act("($n attempts to knock $N out.)", trickcheck(ch), NULL, victim, TO_NOTVICT);
          }
          return;
        }
      }

      int disc = discipline_table[point].vnum;
      if (disc == DIS_FIRE) {
        equip_focus(ch);
        focus_glow(ch, "glow red-orange");
      }
      if (disc == DIS_ICE) {
        equip_focus(ch);
        focus_glow(ch, "glow blue-white");
      }
      if (disc == DIS_LIGHTNING) {
        equip_focus(ch);
        focus_glow(ch, "crackle with lightning");
      }
      if (disc == DIS_DARK) {
        equip_focus(ch);
        focus_glow(ch, "turn pure black and suck in ambiant light");
      }
      if (disc == DIS_PRISMATIC) {
        equip_focus(ch);
        focus_glow(ch, "shimmer with prismatic light");
      }

      if (!IS_NPC(ch))
      gain_exp(ch, xp_per_attack(ch, victim, discipline_table[point].vnum, dam), FEXP_ATTACK);
      int stag = apply_stagger(ch, victim, dam, discipline_table[point].vnum);
      int echo;
      victim->wound_location = number_percent();

      if (cpoint != -1 && get_shadow(ch, cpoint) != -1)
      echo = damage_echo(ch, victim, discipline_table[point].vnum, discipline_table[get_shadow(ch, cpoint)].vnum, dam, range, TRUE);
      else
      echo = damage_echo(ch, victim, discipline_table[point].vnum, discipline_table[point].vnum, dam, range, TRUE);

      if (IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR) && combat_distance(ch, victim, FALSE) >= 50) {
        sprintf(buf, "You are shot silently, it %s", defense_message(victim, TO_VICT, TRUE, echo));
        act(buf, ch, NULL, victim, TO_VICT);
        if (cpoint != -1) {
          if (dam > 0 || stag > 0) {
            sprintf(buf, "%s %s", ch->pcdata->customstrings[cpoint][1], defense_message(victim, TO_CHAR, TRUE, echo));
            act(buf, ch, NULL, victim, TO_CHAR);
          }
          else
          act(ch->pcdata->customstrings[cpoint][1], ch, NULL, victim, TO_CHAR);
        }
        else
        act("You shoot $N silently.", ch, NULL, victim, TO_CHAR);
      }
      else if (cpoint != -1) {
        if (dam > 0 || stag > 0) {
          sprintf(buf, "%s %s", ch->pcdata->customstrings[cpoint][1], defense_message(victim, TO_CHAR, TRUE, echo));
          act(buf, ch, NULL, victim, TO_CHAR);
          sprintf(buf, "%s %s", ch->pcdata->customstrings[cpoint][2], defense_message(victim, TO_VICT, TRUE, echo));
          act(buf, trickcheck(ch), NULL, victim, TO_VICT);
          sprintf(buf, "%s %s", ch->pcdata->customstrings[cpoint][3], defense_message(victim, TO_ROOM, TRUE, echo));
          act(buf, trickcheck(ch), NULL, victim, TO_NOTVICT);
          dact(buf, trickcheck(ch), NULL, victim, DISTANCE_NEAR);
          if (battleground(ch->in_room)) {
            op_report(logact(buf, ch, victim), ch);
          }
        }
        else {
          act(ch->pcdata->customstrings[cpoint][1], ch, NULL, victim, TO_CHAR);
          act(ch->pcdata->customstrings[cpoint][2], trickcheck(ch), NULL, victim, TO_VICT);
          act(ch->pcdata->customstrings[cpoint][3], trickcheck(ch), NULL, victim, TO_NOTVICT);
          dact(ch->pcdata->customstrings[cpoint][3], trickcheck(ch), NULL, victim, DISTANCE_NEAR);
          if (battleground(ch->in_room)) {
            op_report(logact(ch->pcdata->customstrings[cpoint][3], ch, victim), ch);
          }
        }
      }
      else
      hit_message(trickcheck(ch), victim, discipline_table[point].vnum, -1, echo);



      if (!IS_NPC(ch)) {
      }

      if (dam > 0 || stag > 0) {
        if (cpoint != -1 && get_shadow(ch, cpoint) != -1) {
          printf_to_char(
          ch, "[%s reduces defenses by `r%d`x, balance by `c%d`x%s]\n\r", discipline_table[get_shadow(ch, cpoint)].name, dam, stag, special_mes);
          printf_to_char(
          victim, "[%s reduces defenses by `r%d`x, balance by `c%d`x%s]\n\r", discipline_table[get_shadow(ch, cpoint)].name, dam, stag, special_mes);
        }
        else {
          printf_to_char(
          ch, "[%s reduces defenses by `r%d`x, balance by `c%d`x%s]\n\r", disc_name(ch, point), dam, stag, special_mes);
          printf_to_char(
          victim, "[%s reduces defenses by `r%d`x, balance by `c%d`x%s]\n\r", disc_name(ch, point), dam, stag, special_mes);
        }
      }
      combat_damage(victim, ch, dam, discipline_table[point].vnum);
      death_cause(ch, victim, discipline_table[point].vnum);
      ch->fight_attacks++;

      ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
      if (!IS_NPC(ch)) {
        ch->pcdata->fatigue += HIT_FATIGUE;
      }
      else {
        if (fight_over(room)) {
          end_fight(room);
          return;
        }
      }
    }
  }

  void regened_message(CHAR_DATA *ch) {
    int shield = shield_type(ch);

    switch (shield) {
    case DIS_BARMOR:
      act("You fix up and readjust your ballistic armor.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_MARMOR:
      act("You fix up and readjust your armor.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_NARMOR:
      act("The punctures in your natural armor heal over.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_TOUGHNESS:
      act("The pain from your injuries fades.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_ENERGYS:
      act("Your energy shield flares strongly to life before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_FIRES:
      act("Your fire shield flares strongly to life before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_ICES:
      act("Your ice shield flares strongly to life before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_LIGHTNINGS:
      act("Your lightning shield flares strongly to life before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_STONES:
      act("Your shield of stone forms strongly around you before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_WOODS:
      act("Your shield of wood forms strongly around you before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_MENTALS:
      act("Your mental shield ripples strongly to life before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_DARKS:
      act("Your shield of darkness flares strongly to life before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_BONES:
      act("The pain from your injuries fades.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_FORCES:
      act("Your shield pulses briefly with power before fading to invisibility.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_UNDEAD:
      act("The discomfort from your injuries fades.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_FATE:
      act("You start to feel lucky again.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_PUSH:
      act("You feel your mental fatigue lifting.", ch, NULL, NULL, TO_CHAR);
      break;
    case DIS_BSHIELD:
    case DIS_MSHIELD:
      act("You resettle your shield into place.", ch, NULL, NULL, TO_CHAR);
      break;
    default:
      act("You recover your defenses.", ch, NULL, NULL, TO_CHAR);
      break;
    }
  }

  int shield_total(CHAR_DATA *ch) {
    int i, val = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      val += get_disc(ch, discipline_table[i].vnum, FALSE);
    }
    return val;
  }
  int shield_total_creation(CHAR_DATA *ch) {
    int i, val = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      val += ch->disciplines[i];
    }
    return val;
  }

  bool is_ranged(CHAR_DATA *ch) {
    if (default_ranged_value(ch) >= default_melee_value(ch))
    return TRUE;

    return FALSE;
  }

  int get_agg(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch == victim)
    return 0;
    int agg = 0, dis, i, point = 0, dam;

    if (is_gm(victim) || is_ghost(victim))
    return 0;

    if (IS_NPC(ch) && !IS_NPC(victim) && ch->pIndexData->vnum == MONSTER_TEMPLATE && victim->wounds >= 2 && ch->controled_by == NULL)
    return 0;

    if (in_fight(ch)) {
      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
      return 0;
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD))
      return 0;
    }
    if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return 0;
    if (!IS_FLAG(ch->act, PLR_DEEPSHROUD) && IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return 0;

    if (battleground(ch->in_room) && ch->faction == victim->faction)
    return 0;

    if (IS_NPC(ch) && victim->in_room != NULL && ch->in_room != victim->in_room && !can_get_to(ch, victim->in_room))
    return 0;

    if (IS_NPC(ch) && !IS_NPC(victim) && ch->pIndexData->vnum >= 150 && ch->pIndexData->vnum <= 160 && victim->wounds > 2)
    return 0;

    if (IS_NPC(ch) && !IS_NPC(victim) && (ch->pIndexData->vnum == HAND_SOLDIER || ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == MINION_TEMPLATE) && is_helpless(victim))
    return 0;
    if (IS_NPC(ch) && safe_strlen(ch->protecting) > 1 && !str_cmp(ch->protecting, victim->name))
    return 0;

    if (victim->target != NULL && victim->target == ch)
    agg += victim->target_dam;
    if (victim->target_2 != NULL && victim->target_2 == ch)
    agg += victim->target_dam_2;
    if (victim->target_3 != NULL && victim->target_3 == ch)
    agg += victim->target_dam_3;

    if (IS_NPC(ch) && (ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == MINION_TEMPLATE) && ch->order == BORDER_ATTACK && safe_strlen(ch->ordertarget) > 2 && !str_cmp(ch->ordertarget, victim->name))
    return 1000;

    if (IS_NPC(ch) && (ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == MINION_TEMPLATE))
    return agg;

    if (IS_NPC(ch) && !str_cmp(ch->aggression, "all") && str_cmp(ch->protecting, victim->name) && str_cmp(ch->protecting, get_fac(victim)) && ch->race != victim->race)
    agg += 10;
    else if (IS_NPC(ch) && ch->race == RACE_DEMON && victim->race != RACE_DEMON)
    agg += 30;

    if (IS_NPC(ch)) {
      if (!str_cmp(ch->aggression, victim->name))
      agg += 50;
      if (!str_cmp(ch->aggression, get_fac(victim)))
      agg += 20;
      if (victim->factiontwo != 0 && clan_lookup(victim->factiontwo) != NULL && !str_cmp(ch->aggression, clan_lookup(victim->factiontwo)->name))
      agg += 20;
    }

    if (ch->faction != 0 && victim->faction != ch->faction && IS_NPC(ch) && battleground(ch->in_room))
    agg += 30;

    if (IS_NPC(ch) && battleground(ch->in_room) && ch->pIndexData->vnum == 115) {
      if (IS_NPC(victim) && victim->pIndexData->vnum == 115)
      return 0;
      if (!IS_NPC(victim) || victim->pIndexData->vnum != 115)
      agg += 50;
      if (victim->bagcarrier == 1)
      agg += 100;
    }

    if (IS_FLAG(victim->act, ACT_COVER) && !IS_FLAG(victim->act, ACT_TURRET) && IS_NPC(victim))
    return 0;
    if ((IS_FLAG(ch->act, ACT_COMBATOBJ) && IS_NPC(ch)) || (IS_FLAG(victim->act, ACT_COMBATOBJ) && IS_NPC(victim)))
    return 0;

    if (IS_NPC(victim) && (victim->race == RACE_HUMAN || victim->race == RACE_ANIMAL))
    return 0;

    if (IS_NPC(ch) && str_cmp(ch->protecting, "") && agg <= 12 && ch->pIndexData->vnum == MONSTER_TEMPLATE)
    return 0;

    if (IS_NPC(ch) && ch->faction > 3 && clan_lookup(ch->faction) != NULL && ch->in_room != NULL && battleground(ch->in_room) && !str_cmp(clan_lookup(ch->faction)->battle_target, victim->name) && clan_lookup(ch->faction)->battle_order == BORDER_ATTACK)
    return 1000;

    if (IS_NPC(ch) && ch->faction == victim->faction && ch->faction > 0)
    agg = UMAX(0, agg - 30);

    if (IS_NPC(ch) && !IS_NPC(victim) && ch->pIndexData->vnum == MONSTER_TEMPLATE && ch->controled_by != NULL && IS_FLAG(victim->act, PLR_GUEST) && same_player(ch->controled_by, victim))
    agg = UMAX(0, agg - 50);

    if (agg == 0)
    return 0;

    if (victim->wounds >= 3)
    return 1;

    if (IS_NPC(ch)) {
      if (is_ranged(ch)) {
        agg += 5;
        dis = default_ranged(ch);
        for (i = 0; i < DIS_USED; i++) {
          if (discipline_table[i].vnum == dis)
          point = i;
        }
        dam = 100;
        dam =
        dam *
        damage_mod(combat_distance(ch, victim, TRUE), disc_range(ch, point)) /
        100;

        dam = UMAX(dam, 10);
        agg = agg * dam / 100;

        if (IS_FLAG(victim->act, ACT_COVER) && IS_NPC(victim))
        agg /= 2;
        if (is_in_cover(victim))
        agg /= 2;
      }
      else {
        agg += 10;
        agg = agg * 30 / combat_distance(ch, victim, TRUE);
      }
    }
    if (!IS_NPC(victim) && victim->race == RACE_CIVILIAN)
    agg /= 2;
    return agg;
  }

  CHAR_DATA *get_npc_target(CHAR_DATA *ch) {
    CHAR_DATA *victim = NULL;
    int max = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || IS_IMMORTAL(rch))
      continue;

      if (in_public(ch, rch) && !IS_FLAG(rch->act, PLR_SHROUD))
      continue;

      if (forest_monster(ch) && rch->in_room != NULL && mist_level(rch->in_room) < 3 && (rch->in_room->area->vnum == INNER_NORTH_FOREST || rch->in_room->area->vnum == INNER_SOUTH_FOREST || rch->in_room->area->vnum == INNER_WEST_FOREST || rch->in_room->area->vnum == HAVEN_TOWN_VNUM))
      continue;

      if ((forest_monster(ch) || is_invader(ch)) && is_helpless(rch))
      continue;

      if (in_fight(ch) || in_fight(rch)) {
        if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(rch->act, PLR_SHROUD))
        continue;
        if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(rch->act, PLR_SHROUD))
        continue;
      }

      if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(rch->act, PLR_DEEPSHROUD))
      continue;
      if (!IS_FLAG(ch->act, PLR_DEEPSHROUD) && IS_FLAG(rch->act, PLR_DEEPSHROUD))
      continue;

      if (!will_agg(ch, rch))
      continue;
      // act("Here.", ch, NULL, NULL, TO_ROOM);

      if (get_agg(ch, rch) > max) {
        victim = rch;
        max = get_agg(ch, rch);
        // act("Here2.", ch, NULL, NULL, TO_ROOM);
      }
    }
    if (max <= 0) {
      return NULL;
    }
    if (!victim || victim->in_room == NULL || victim->wounds > 3)
    return NULL;

    if (get_cover(victim) != NULL && combat_distance(ch, victim, TRUE) > 1)
    victim = get_cover(victim);

    if (!victim || victim->in_room == NULL || victim->wounds > 3)
    return NULL;

    return victim;
  }

  int process_npc_special(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int special) {
    int facingdir = victim->facing;

    switch (special) {
    case SPECIAL_STUN:
      if (ch->fight_fast == TRUE) {
        victim->attack_timer += FIGHT_WAIT * fight_speed(victim);
        victim->move_timer += FIGHT_WAIT * fight_speed(victim);
      }
      else {
        SET_FLAG(victim->fightflag, FIGHT_NOATTACK);
        SET_FLAG(victim->fightflag, FIGHT_NOMOVE);
      }
      if (IS_FLAG(victim->fightflag, FIGHT_DEFEND))
      REMOVE_FLAG(victim->fightflag, FIGHT_DEFEND);
      dam = dam / 2;
      break;
    case SPECIAL_KNOCKBACK:
      int temporary;
      temporary = victim->move_timer;
      move_away(victim, ch->x, ch->y, 10, 0, FALSE);
      victim->facing = facingdir;
      victim->move_timer = temporary;

      break;
    case SPECIAL_ENTRAP:
      if (ch->fight_fast == TRUE)
      victim->move_timer += FIGHT_WAIT * fight_speed(victim);
      else
      SET_FLAG(victim->fightflag, FIGHT_NOMOVE);
      fall_character(victim);
      break;
    case SPECIAL_GRAPPLE:
      dam = dam * 8 / 10;
      if (ch->fight_fast == TRUE)
      victim->move_timer += FIGHT_WAIT * fight_speed(victim);
      else
      SET_FLAG(victim->fightflag, FIGHT_NOMOVE);

      fall_character(victim);
      break;
    case SPECIAL_LIFESTEAL:
      ch->hit += dam / 2;
      dam = dam * 3 / 4;
      break;
    case SPECIAL_OVERPOWER:
      dam = dam * 14 / 10;
      SET_FLAG(ch->fightflag, FIGHT_OVERPOWER);
      break;
    case SPECIAL_DAMAGE:
      dam = dam * 110 / 100;
      break;
    case SPECIAL_WEAKEN:
      SET_FLAG(victim->fightflag, FIGHT_WEAKEN);
      dam = dam * 3 / 4;
      break;
    case SPECIAL_UNDERPOWER:
      dam = dam / 2;
      if (dam > 0)
      SET_FLAG(ch->fightflag, FIGHT_UNDERPOWER);
      ch->underpowering = victim;
      break;
      break;
    }
    return dam;
  }

  char *npc_special_message(CHAR_DATA *ch, int special) {
    if (special == SPECIAL_STUN) {
      return " (STUN)";
    }
    if (special == SPECIAL_KNOCKBACK)
    return " (KNOCKBACK)";
    if (special == SPECIAL_ENTRAP)
    return " (SNARED)";
    if (special == SPECIAL_GRAPPLE)
    return " (SNARED)";
    if (special == SPECIAL_LIFESTEAL)
    return " (LIFESTEAL)";
    if (special == SPECIAL_OVERPOWER)
    return " (OVERPOWER)";
    if (special == SPECIAL_DAMAGE)
    return " (DAMAGE)";
    if (special == SPECIAL_WEAKEN)
    return " (WEAKEN)";
    if (special == SPECIAL_UNDERPOWER)
    return " (UNDERPOWER)";
    return "";
  }

  void npc_combat_attack(CHAR_DATA *ch) {
    int i, point = 0, vnum, dam, spec = 0;
    CHAR_DATA *victim = get_npc_target(ch);
    bool protecting = FALSE;
    ch->cfighting = NULL;

    ROOM_INDEX_DATA *room = ch->in_room;

    if (victim == NULL) {
      ch->attacking = 0;
      return;
    }
    if (victim == ch) {
      ch->attacking = 0;
      return;
    }

    if (ch->wounds > 2 || ch->in_room == NULL || victim->in_room == NULL)
    return;

    if (IS_FLAG(ch->act, ACT_COVER) && !IS_FLAG(ch->act, ACT_TURRET) && IS_NPC(ch))
    return;
    if (IS_FLAG(ch->act, ACT_COMBATOBJ) && IS_NPC(ch))
    return;

    CHAR_DATA *original = victim;

    if (has_caff(ch, CAFF_CONFUSE)) {
      CHAR_DATA *newvict;
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        newvict = *it;

        if (!in_fight(newvict))
        continue;
        if (newvict == NULL || is_gm(newvict))
        continue;
        if (!same_fight(newvict, ch))
        continue;

        if (IS_FLAG(newvict->act, ACT_COMBATOBJ) && IS_NPC(newvict))
        continue;

        if (ch == newvict)
        continue;

        if (victim == newvict)
        continue;

        if (combat_distance(newvict, ch, TRUE) >
            combat_distance(victim, ch, TRUE))
        continue;
        if (number_percent() % 2 == 0)
        victim = newvict;
      }
    }

    if (is_protected(victim) && get_protector(victim) != ch && !is_in_cover(victim)) {
      CHAR_DATA *temp = get_protector(victim);
      victim = temp;
      protecting = TRUE;
    }

    if (ch->attack_timer > 0)
    return;

    if (victim == NULL)
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
    return;

    if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD))
    return;
    if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return;

    if (!IS_FLAG(ch->act, PLR_DEEPSHROUD) && IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return;

    if (IS_FLAG(ch->fightflag, FIGHT_DEFEND))
    REMOVE_FLAG(ch->fightflag, FIGHT_DEFEND);

    if (combat_distance(ch, victim, TRUE) > 1)
    vnum = default_ranged(ch);
    else
    vnum = default_melee(ch);

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].vnum == vnum)
      point = i;
    }

    if (get_disc(ch, discipline_table[point].vnum, FALSE) < 1 && combat_distance(ch, victim, TRUE) > get_speed(ch) * 2) {
      if (!IS_FLAG(ch->fightflag, FIGHT_DEFEND))
      SET_FLAG(ch->fightflag, FIGHT_DEFEND);
      if (will_fight_show(ch, FALSE) && !IS_NPC(ch))
      act("$n defends.", ch, NULL, NULL, TO_ROOM);
      else if (!IS_NPC(ch))
      pact("$n defends.", ch, NULL, NULL, ch->x, ch->y, 25, 0, 0);
      ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
      return;
    }

    if (combat_distance(ch, victim, TRUE) > 1 && combat_distance(ch, victim, TRUE) > discipline_table[point].range) {
      if (combat_distance(ch, victim, TRUE) >
          discipline_table[point].range + get_speed(ch) * 2) {
        if (!IS_FLAG(ch->fightflag, FIGHT_DEFEND))
        SET_FLAG(ch->fightflag, FIGHT_DEFEND);
        if (will_fight_show(ch, FALSE) && !IS_NPC(ch))
        act("$n defends.", ch, NULL, NULL, TO_ROOM);
        else if (!IS_NPC(ch))
        pact("$n defends.", ch, NULL, NULL, ch->x, ch->y, 25, 0, 0);
        ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
      }
      return;
    }
    int shield = shield_type(victim);

    dam = damage_calculate(ch, victim, point, shield);

    if (dam <= 0) {
      if (forest_monster(ch)) {
        if (get_disc(ch, discipline_table[point].vnum, FALSE) < 1)
        return;
        else
        dam = 2;
      }
      else
      return;
    }
    ch->cfighting = original;
    if (!IS_NPC(victim)) {
      spec = GET_NPC_SPECIAL(vnum);
      if (spec != 0 && !IS_FLAG(ch->fightflag, FIGHT_SPECUSED)) {
        dam = process_npc_special(ch, victim, dam, spec);
        SET_FLAG(ch->fightflag, FIGHT_SPECUSED);
      }
      else {
        spec = 0;
        if (IS_FLAG(ch->fightflag, FIGHT_SPECUSED))
        REMOVE_FLAG(ch->fightflag, FIGHT_SPECUSED);
      }
    }

    if (protecting == TRUE && !IS_NPC(victim) && get_skill(victim, SKILL_GUARDIAN) > 0)
    dam = dam * 2 / 3;

    if (IS_FLAG(ch->fightflag, FIGHT_OVERPOWER)) {
      dam = dam * 2 / 5;
      REMOVE_FLAG(ch->fightflag, FIGHT_OVERPOWER);
    }
    if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN)) {
      dam = dam / 2;
      REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN);
    }
    if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN1)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN1);
      SET_FLAG(ch->fightflag, FIGHT_WEAKEN2);
      dam = dam * 3 / 4;
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN2)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN2);
      SET_FLAG(ch->fightflag, FIGHT_WEAKEN3);
      dam = dam * 3 / 4;
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_WEAKEN3)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_WEAKEN3);
      dam = dam * 3 / 4;
    }

    if (IS_FLAG(ch->fightflag, FIGHT_UNDERPOWER)) {
      if (ch->underpowering == victim)
      dam = dam * 140 / 100;
      REMOVE_FLAG(ch->fightflag, FIGHT_UNDERPOWER);
    }

    ch->attack_timer = FIGHT_WAIT * fight_speed(ch);
    ch->fight_attacks++;

    if (IS_NPC(ch) && !IS_NPC(victim) && (ch->pIndexData->vnum == HAND_SOLDIER || ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == MINION_TEMPLATE) && victim->hit <= 0 && !battleground(ch->in_room) && !guestmonster(victim)) {
      act("$n knocks $N out.\n\r", ch, NULL, victim, TO_NOTVICT);
      act("$n knocks you out.\n\r", ch, NULL, victim, TO_VICT);
      victim->pcdata->sleeping = 240;
      return;
    }
    if ((forest_monster(ch) || is_invader(ch)) && !IS_NPC(victim) && victim->hit <= 0 && !battleground(ch->in_room) && !guestmonster(victim)) {
      act("$n knocks $N out.\n\r", ch, NULL, victim, TO_NOTVICT);
      act("$n knocks you out.\n\r", ch, NULL, victim, TO_VICT);
      victim->pcdata->sleeping = 240;
      return;
    }

    combat_damage(victim, ch, dam, discipline_table[point].vnum);
    death_cause(ch, victim, discipline_table[point].vnum);
    int stag = apply_stagger(ch, victim, dam, discipline_table[point].vnum);
    victim->wound_location = number_percent();
    int echo = damage_echo(ch, victim, discipline_table[point].vnum, discipline_table[point].vnum, dam, combat_distance(ch, victim, TRUE), TRUE);

    hit_message(ch, victim, discipline_table[point].vnum, -1, echo);
    fall_character(victim);
    printf_to_char(
    victim, "[%s reduces your defenses by `r%d`x, balance by `c%d`x%s]\n\r", disc_name(ch, point), dam, stag, npc_special_message(victim, spec));

    if (fight_over(room)) {
      end_fight(room);
      return;
    }
  }

  CHAR_DATA *get_npc_follow(CHAR_DATA *ch) {
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;
      if (!in_fight(rch))
      continue;
      if (rch == NULL || is_gm(rch))
      continue;
      if (!same_fight(rch, ch))
      continue;

      if (ch->faction == 0)
      return NULL;

      if (!str_cmp(clan_lookup(ch->faction)->battle_target, rch->name))
      return rch;
    }
    return NULL;
  }

  CHAR_DATA *get_close_cover(CHAR_DATA *ch) {

    CHAR_DATA *victim;
    CHAR_DATA *cover = NULL;
    int mindist = 1000;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;
      if (!in_fight(victim))
      continue;
      if (victim == NULL || is_gm(victim))
      continue;
      if (!same_fight(ch, victim))
      continue;

      if (victim == ch)
      continue;
      if (victim->in_room == NULL)
      continue;
      if (!IS_NPC(victim))
      continue;

      if (!is_cover(victim))
      continue;

      if (combat_distance(victim, ch, FALSE) < mindist) {
        mindist = combat_distance(victim, ch, FALSE);
        cover = victim;
      }
    }
    return cover;
  }

  CHAR_DATA *get_close_ally(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    CHAR_DATA *closest = NULL;
    int mindistance = 1000;

    if (ch->pIndexData->vnum != 115 && ch->pIndexData->vnum != ALLY_TEMPLATE && (ch->faction == 0 || !battleground(ch->in_room)))
    return NULL;

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (!in_fight(victim))
      continue;
      if (victim == NULL || is_gm(victim))
      continue;
      if (!same_fight(ch, victim))
      continue;

      if (victim == ch)
      continue;
      if (victim->in_room == NULL)
      continue;
      if (!IS_NPC(victim))
      continue;
      if (ch->pIndexData->vnum != victim->pIndexData->vnum && ch->pIndexData->vnum == 115)
      continue;

      if (ch->pIndexData->vnum == ALLY_TEMPLATE && ((ch->faction != victim->faction) || ch->faction == 0))
      continue;

      if (ch->pIndexData->vnum != 115 && ch->faction != victim->faction)
      continue;

      if (combat_distance(victim, ch, FALSE) < mindistance && combat_distance(victim, ch, FALSE) > 0) {
        closest = victim;
        mindistance = combat_distance(victim, ch, FALSE);
      }
    }
    return closest;
  }

  CHAR_DATA *get_bag_carrier() {
    CHAR_DATA *victim;

    //    ROOM_INDEX_DATA *room = get_room_index(BATTLEROOM);

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL || is_gm(victim))
      continue;
      if (!in_fight(victim))
      continue;

      if (victim->bagcarrier == 1)
      return victim;
    }
    return NULL;
  }

  void npc_combat_move(CHAR_DATA *ch) {
    //    char buf[MSL];
    // sprintf(buf("NPC MOVE: %s", ch->name);
    // log_string(buf);
    round_process(ch);

    int dis, i, point = 0, opti, dist;
    CHAR_DATA *victim = get_npc_target(ch);

    int origx = ch->x;
    int origy = ch->y;
    ROOM_INDEX_DATA *origroom = ch->in_room;

    if (ch->wounds > 2 || ch->in_room == NULL)
    return;

    if (ch->move_timer > 0)
    return;
    ch->move_timer = FIGHT_WAIT * fight_speed(ch);

    if (IS_FLAG(ch->act, ACT_TURRET) && IS_NPC(ch))
    return;
    if (IS_FLAG(ch->act, ACT_COVER) && IS_NPC(ch))
    return;
    if (IS_FLAG(ch->act, ACT_COMBATOBJ) && IS_NPC(ch))
    return;

    if (IS_NPC(ch) && (ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == MINION_TEMPLATE) && ch->order == BORDER_FOLLOW && safe_strlen(ch->ordertarget) > 2 && get_char_fight(ch, ch->ordertarget) != NULL) {
      victim = get_char_fight(ch, ch->ordertarget);
      if (victim != NULL) {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_PROTECT), victim->in_room->z - ch->in_room->z, TRUE);
        ch->move_timer = FIGHT_WAIT * fight_speed(ch);
        return;
      }
    }

    if (IS_NPC(ch) && clan_lookup(ch->faction) != NULL && ch->faction > 3 && ch->in_room != NULL && battleground(ch->in_room) && clan_lookup(ch->faction)->battle_order == BORDER_FOLLOW) {
      victim = get_npc_follow(ch);
      if (victim != NULL) {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_PROTECT), victim->in_room->z - ch->in_room->z, TRUE);
        ch->move_timer = FIGHT_WAIT * fight_speed(ch);
        return;
      }
    }

    if (victim == NULL) {
      return;
    }
    if(IS_NPC(ch) && ch->mob_ai == MOB_AI_ANTAG_SOLDIER)
    {
      if(victim->hit >= (max_hp(victim)-5))
      {
        CHAR_DATA *cover = get_close_cover(ch);
        bool show = FALSE;
        if(cover != NULL)
        {
          if (combat_distance(ch, cover, FALSE) > 5)
          show = TRUE;
          move_towards(ch, relative_x(ch, cover->in_room, cover->x), relative_y(ch, cover->in_room, cover->y), combat_move_speed(ch, MOVE_RETREAT), cover->in_room->z - ch->in_room->z, TRUE);
          if (show == TRUE)
          move_message(ch, MOVE_RETREAT, origx, origy, origroom);
          ch->move_timer = FIGHT_WAIT * fight_speed(ch);
          return;
        }
      }
    }

    if (IS_NPC(ch) && ch->pIndexData->vnum == 115) {
      CHAR_DATA *cover = get_close_cover(ch);
      bool show = FALSE;
      if (ch->bagcarrier == 1) {
        CHAR_DATA *cover = get_close_cover(ch);
        if (cover != NULL) {
          if (combat_distance(ch, cover, FALSE) > 5)
          show = TRUE;
          move_towards(ch, relative_x(ch, cover->in_room, cover->x), relative_y(ch, cover->in_room, cover->y), combat_move_speed(ch, MOVE_RETREAT), cover->in_room->z - ch->in_room->z, TRUE);
          if (show == TRUE)
          move_message(ch, MOVE_RETREAT, origx, origy, origroom);
          ch->move_timer = FIGHT_WAIT * fight_speed(ch);
          return;
        }
      }
      else {
        CHAR_DATA *carry = get_bag_carrier();
        if (carry != NULL && IS_NPC(carry) && carry->pIndexData->vnum == 115) {
          if (combat_distance(ch, carry, FALSE) > 15)
          show = TRUE;
          move_towards(ch, relative_x(ch, carry->in_room, carry->x), relative_y(ch, carry->in_room, carry->y), combat_move_speed(ch, MOVE_MOVE), carry->in_room->z - ch->in_room->z, TRUE);
          if (show == TRUE)
          move_message(ch, MOVE_MOVE, origx, origy, origroom);
          ch->move_timer = FIGHT_WAIT * fight_speed(ch);
          return;
        }
        else if (combat_distance(ch, victim, TRUE) <= 75) {
          if (cover != NULL && combat_distance(ch, cover, TRUE) <= 50 && combat_distance(victim, cover, TRUE) <= 80 && combat_distance(victim, cover, TRUE) > 2) {
            if (combat_distance(ch, cover, FALSE) > 5)
            show = TRUE;
            move_towards(ch, relative_x(ch, cover->in_room, cover->x), relative_y(ch, cover->in_room, cover->y), combat_move_speed(ch, MOVE_RETREAT), cover->in_room->z - ch->in_room->z, TRUE);
            if (show == TRUE)
            move_message(ch, MOVE_RETREAT, origx, origy, origroom);
            ch->move_timer = FIGHT_WAIT * fight_speed(ch);
            return;
          }
        }
      }
    }

    if (IS_NPC(ch) && ch->pIndexData->vnum == ALLY_TEMPLATE) {
      dis = default_ranged(ch);
      for (i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == dis)
        point = i;
      }
      if (ch->disciplines[dis] >= 10 && combat_distance(ch, victim, TRUE) <=
          discipline_table[point].range * 3 / 4) {
        CHAR_DATA *cover = get_close_cover(ch);
        bool show = FALSE;
        if (cover != NULL && combat_distance(ch, cover, TRUE) <= 50 && combat_distance(victim, cover, TRUE) <=
            discipline_table[point].range * 3 / 4 && (combat_distance(victim, cover, TRUE) > 2 || !is_ranged(ch))) {
          if (combat_distance(ch, cover, FALSE) > 5)
          show = TRUE;
          move_towards(ch, relative_x(ch, cover->in_room, cover->x), relative_y(ch, cover->in_room, cover->y), combat_move_speed(ch, MOVE_RETREAT), cover->in_room->z - ch->in_room->z, TRUE);
          if (show == TRUE)
          move_message(ch, MOVE_RETREAT, origx, origy, origroom);
          ch->move_timer = FIGHT_WAIT * fight_speed(ch);
          return;
        }
      }
    }

    if (victim == NULL)
    return;

    if (is_ranged(ch)) {
      dis = default_ranged(ch);
      for (i = 0; i < DIS_USED; i++) {
        if (discipline_table[i].vnum == dis)
        point = i;
      }
      opti = discipline_table[point].range / 2;
      //	opti += 10;
      dist = combat_distance(ch, victim, TRUE);

      if (opti > dist + 1) {
        move_away(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), UMIN(opti - dist, combat_move_speed(ch, MOVE_RETREAT)), 0, TRUE);
        if (combat_distance(ch, victim, TRUE) > 50) {
          CHAR_DATA *ally = get_close_ally(ch);
          if (ally != NULL)
          move_towards(ch, relative_x(ch, ally->in_room, ally->x), relative_y(ch, ally->in_room, ally->y), 5, ally->in_room->z - ch->in_room->z, FALSE);
        }
        move_message(ch, MOVE_RETREAT, origx, origy, origroom);
      }
      else if (dist > discipline_table[point].range) {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_CHARGE), victim->in_room->z - ch->in_room->z, TRUE);
        if (combat_distance(ch, victim, TRUE) > 50) {
          CHAR_DATA *ally = get_close_ally(ch);
          if (ally != NULL)
          move_towards(ch, relative_x(ch, ally->in_room, ally->x), relative_y(ch, ally->in_room, ally->y), 5, ally->in_room->z - ch->in_room->z, FALSE);
        }
        move_message(ch, MOVE_CHARGE, origx, origy, origroom);
      }
      else if (dist > opti + 1) {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), UMIN(dist - opti, combat_move_speed(ch, MOVE_MOVE)), victim->in_room->z - ch->in_room->z, TRUE);
        if (combat_distance(ch, victim, TRUE) > 50) {
          CHAR_DATA *ally = get_close_ally(ch);
          if (ally != NULL)
          move_towards(ch, relative_x(ch, ally->in_room, ally->x), relative_y(ch, ally->in_room, ally->y), 5, ally->in_room->z - ch->in_room->z, FALSE);
        }
        move_message(ch, MOVE_MOVE, origx, origy, origroom);
      }
      ch->move_timer = FIGHT_WAIT * fight_speed(ch);
    }
    else {
      if (combat_distance(ch, victim, FALSE) > 1) {
        move_towards(ch, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y), combat_move_speed(ch, MOVE_CHARGE), victim->in_room->z - ch->in_room->z, TRUE);
        if (combat_distance(ch, victim, TRUE) > 50) {
          CHAR_DATA *ally = get_close_ally(ch);
          if (ally != NULL)
          move_towards(ch, relative_x(ch, ally->in_room, ally->x), relative_y(ch, ally->in_room, ally->y), 5, ally->in_room->z - ch->in_room->z, FALSE);
        }

        move_message(ch, MOVE_CHARGE, origx, origy, origroom);
      }
      ch->move_timer = FIGHT_WAIT * fight_speed(ch);
    }
  }

#define MIN_DEMON 10
#define MAX_DEMON 40

  int const demon_levels[] = {10, 12,  18,  22,  25,  25,  30,  26,  30, 37, 40, 47, 50,  55,  62,  65,  70,  80,  80,  80, 85, 90, 95, 100, 112, 115, 132, 135, 135, 145, 167};

  int mob_power_score(MOB_INDEX_DATA *mob) {
    int shield_total = 0;
    int cost = 0;
    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range == -1)
      shield_total += mob->disciplines[discipline_table[i].vnum];
      else
      cost += built_disc_cost(mob->disciplines[discipline_table[i].vnum]);
    }
    cost += built_disc_cost(shield_total);
    return cost;
  }

  int get_demon_lvl(int pointer) {
    int value = 0;
    MOB_INDEX_DATA *pMobIndex;
    pMobIndex = get_mob_index(monster_table[pointer].vnum);
    value = mob_power_score(pMobIndex);
    value /= 2000;
    int speed = pMobIndex->intelligence + 20;
    speed /= 3;
    value = value * speed / 10;
    value += 4;
    value = UMAX(value, 10);
    return value;
  }

  int max_dlvl(int level) {
    int i = MAX_DEMON - MIN_DEMON;
    for (; demon_levels[i] > level && i > 0; i--) {
    }
    return UMAX(i, 0);
  }

  int difficulty_mod(int diff) {

    if (diff == 5)
    return 100;
    if (diff == 6)
    return 120;
    if (diff == 7)
    return 144;
    if (diff == 8)
    return 173;
    if (diff == 9)
    return 207;
    if (diff == 10)
    return 249;
    if (diff == 4)
    return 80;
    if (diff == 3)
    return 64;
    if (diff == 2)
    return 51;
    if (diff == 1)
    return 41;

    return 100;
  }

  char *const crystal_levels[] = {"chipped", "flawed", "murky", "flawless", "perfect"};

  char *const crystal_types[] = {"ruby",    "onyx",        "sapphire", "emerald", "diamond", "amethyst",    "garnet",   "topaz", "opal",    "rainbow star"};

  void create_crystal(ROOM_INDEX_DATA *room, int level) {
    int type;
    OBJ_DATA *obj;
    char buf[MSL];

    if (room_guest(room))
    return;

    obj = create_object(get_obj_index(32), 0);

    obj->level = level;

    if (level == 6)
    type = 10;
    else
    type = number_range(1, 9);

    sprintf(buf, "small, glowing %s %s crystal", crystal_levels[level - 1], crystal_types[type - 1]);

    obj->value[4] = type;

    free_string(obj->short_descr);
    obj->short_descr = str_dup(buf);
    free_string(obj->name);
    obj->name = str_dup(buf);

    sprintf(buf, "A small, glowing %s %s crystal", crystal_levels[level - 1], crystal_types[type - 1]);

    free_string(obj->description);
    obj->description = str_dup(buf);

    obj_to_room(obj, room);
  }

  int get_mob_x(int facing, ROOM_INDEX_DATA *room) {
    switch (facing) {
    case DIR_NORTH:
      return number_range(0, room->size);
      break;
    case DIR_NORTHWEST:
      return number_range(0, room->size / 2);
      break;
    case DIR_NORTHEAST:
      return number_range(room->size / 2, room->size);
      break;
    case DIR_EAST:
      return number_range(room->size / 2, room->size);
      break;
    case DIR_SOUTHEAST:
      return number_range(room->size / 2, room->size);
      break;
    case DIR_SOUTH:
      return number_range(0, room->size);
      break;
    case DIR_SOUTHWEST:
      return number_range(0, room->size / 2);
      break;
    }
    return number_range(0, room->size);
  }
  int get_mob_y(int facing, ROOM_INDEX_DATA *room) {
    switch (facing) {
    case DIR_NORTH:
    case DIR_NORTHEAST:
    case DIR_NORTHWEST:
      return number_range(room->size / 2, room->size);
      break;
    case DIR_WEST:
    case DIR_EAST:
      return number_range(0, room->size);
      break;
    case DIR_SOUTH:
    case DIR_SOUTHEAST:
    case DIR_SOUTHWEST:
      return number_range(0, room->size / 2);
      break;
    }
    return number_range(0, room->size);
  }

  ROOM_INDEX_DATA *aheadroom(int direction, ROOM_INDEX_DATA *origin) {
    if (direction == DIR_NORTH)
    return sourced_room_by_coordinates(origin, origin->x, origin->y + 5, origin->z, FALSE);
    else if (direction == DIR_NORTHEAST)
    return sourced_room_by_coordinates(origin, origin->x + 4, origin->y + 4, origin->z, FALSE);
    else if (direction == DIR_EAST)
    return sourced_room_by_coordinates(origin, origin->x + 5, origin->y, origin->z, FALSE);
    else if (direction == DIR_SOUTHEAST)
    return sourced_room_by_coordinates(origin, origin->x + 4, origin->y - 4, origin->z, FALSE);
    else if (direction == DIR_SOUTH)
    return sourced_room_by_coordinates(origin, origin->x, origin->y - 5, origin->z, FALSE);
    else if (direction == DIR_SOUTHWEST)
    return sourced_room_by_coordinates(origin, origin->x - 4, origin->y - 4, origin->z, FALSE);
    else if (direction == DIR_WEST)
    return sourced_room_by_coordinates(origin, origin->x - 5, origin->y, origin->z, FALSE);
    else if (direction == DIR_NORTHWEST)
    return sourced_room_by_coordinates(origin, origin->x - 4, origin->y + 4, origin->z, FALSE);

    return NULL;
  }

  void populate_warren(CHAR_DATA *ch) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int demon, demontwo;
    int wlevel;

    if (is_gm(ch))
    return;
    if (is_ghost(ch))
    return;

    if (event_cleanse == 1)
    return;

    if (ch->in_room->area->vnum < OUTER_NORTH_FOREST && number_percent() % 11 != 0)
    return;

    if (IS_FLAG(ch->act, PLR_SHROUD))
    return;

    if (higher_power(ch))
    return;

    wlevel = room_level(ch->in_room);

    ROOM_INDEX_DATA *spawnroom = aheadroom(ch->facing, ch->in_room);
    if (spawnroom == NULL || get_dist(ch->in_room->x, ch->in_room->y, spawnroom->x, spawnroom->y) >
        7 || ch->in_room->area->world != spawnroom->area->world) {
      int turn = turn_dir[ch->facing];
      turn = turn_dir[ch->facing];
      spawnroom = aheadroom(turn, ch->in_room);
    }
    if (spawnroom == NULL || get_dist(ch->in_room->x, ch->in_room->y, spawnroom->x, spawnroom->y) >
        7 || ch->in_room->area->world != spawnroom->area->world) {
      return;
    }

    int monsterpoint = 0;
    for (int i = 0; i < 100 && monsterpoint == 0; i++) {
      if (monster_table[i].world == -1) {
        monsterpoint = i;
        break;
      }
    }
    int fromworld = -1;
    if (ch->in_room->area->vnum == OTHER_FOREST_VNUM)
    fromworld = WORLD_OTHER;
    if (ch->in_room->area->vnum == GODREALM_FOREST_VNUM)
    fromworld = WORLD_GODREALM;
    if (ch->in_room->area->vnum == WILDS_FOREST_VNUM)
    fromworld = WORLD_WILDS;
    if (ch->in_room->area->vnum == HELL_FOREST_VNUM)
    fromworld = WORLD_HELL;
    int demonvnum = 0;
    int count = 0;
    int dlvl = 0;
    for (int i = 0; i < 40 && count == 0; i++) {
      demon = number_range(1, 1000) % monsterpoint;
      demontwo = number_range(1, 777) % monsterpoint;
      if (get_demon_lvl(demon) < get_demon_lvl(demontwo) && get_demon_lvl(demontwo) < wlevel)
      demon = demontwo;
      demontwo = number_range(1, 777) % monsterpoint;
      if (get_demon_lvl(demon) < get_demon_lvl(demontwo) && get_demon_lvl(demontwo) < wlevel)
      demon = demontwo;

      demonvnum = monster_table[demon].vnum;

      if (get_demon_lvl(demon) <= wlevel && (fromworld == -1 || fromworld == monster_table[demon].world) && (fromworld == WORLD_HELL || monster_table[demon].world != WORLD_HELL) && count == 0) {
        for (int x = 0; x < 6; x++) {
          if (get_demon_lvl(demon) < wlevel) {
            dlvl = get_demon_lvl(demon);
            ch->pcdata->spawned_monsters = 12;
            pMobIndex = get_mob_index(demonvnum);
            mob = create_mobile(pMobIndex);
            char_to_room(mob, spawnroom);
            mob->hit = max_hp(mob);
            wlevel -= ((8 + count) * get_demon_lvl(demon) / 8);
            free_string(mob->aggression);
            mob->aggression = str_dup("all");
            mob->x = 25;
            mob->y = 25;
            mob->recent_moved = 10;
            mob->facing = roomdirection(spawnroom->x, spawnroom->y, ch->x, ch->y);
            fromworld = monster_table[demon].world;
            if (monster_table[demon].humanoid == 0 && in_world(ch) != WORLD_EARTH) {
              if (butcher_count > 1) {
                mob->valuable = TRUE;
                butcher_count = 0;
              }
              else {
                mob->valuable = FALSE;
                butcher_count++;
                if (butcher_count > 10)
                butcher_count = 0;
              }
            }
            count++;
          }
        }
        char buf[MSL];
        sprintf(buf, "Monster Spawn on %s, in room %d. World %d. Room Level %d. Last vnum %d, Lvl: %d. Count %d, Butcher: %d.", ch->name, spawnroom->vnum, fromworld, room_level(ch->in_room), demonvnum, dlvl, count, butcher_count);
        log_string(buf);
      }
    }
    /*
if(room_level(ch->in_room) > 800 && number_percent() % 7 == 0)
create_crystal(spawnroom, 6);
else if(room_level(ch->in_room) > 300 && number_percent() % 7 == 0)
create_crystal(spawnroom, 5);
else if(room_level(ch->in_room) > 210 && number_percent() % 7 == 0)
create_crystal(spawnroom, 4);
else if(room_level(ch->in_room) > 150 && number_percent() % 7 == 0)
create_crystal(spawnroom, 3);
else if(room_level(ch->in_room) > 100 && number_percent() % 7 == 0)
create_crystal(spawnroom, 2);
else if(room_level(ch->in_room) > 70 && number_percent() % 7 == 0)
create_crystal(spawnroom, 1);
*/
  }

  void demon_spawn(ROOM_INDEX_DATA *spawnroom) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int demon, demontwo;
    int wlevel;

    wlevel = 600;
    int monsterpoint = 0;
    for (int i = 0; i < 100 && monsterpoint == 0; i++) {
      if (monster_table[i].world == -1) {
        monsterpoint = i;
        break;
      }
    }
    int fromworld = WORLD_HELL;

    for (int i = 0; i < 60; i++) {
      demon = number_range(1, 1000) % monsterpoint;
      demontwo = number_range(1, 777) % monsterpoint;
      if (get_demon_lvl(demon) < get_demon_lvl(demontwo) && get_demon_lvl(demontwo) < wlevel)
      demon = demontwo;
      int demonvnum = monster_table[demon].vnum;
      if (get_demon_lvl(demon) <= wlevel && fromworld == monster_table[demon].world) {
        if (get_demon_lvl(demon) < wlevel) {
          pMobIndex = get_mob_index(demonvnum);
          mob = create_mobile(pMobIndex);
          char_to_room(mob, spawnroom);
          mob->hit = max_hp(mob);
          wlevel -= get_demon_lvl(demon);
          free_string(mob->aggression);
          mob->aggression = str_dup("all");
          mob->x = 25;
          mob->y = 25;
          mob->recent_moved = 10;
          mob->facing = DIR_NORTH;
          fromworld = monster_table[demon].world;
        }
      }
    }
  }

  void decorate_warren(ROOM_INDEX_DATA *room) {
    free_string(room->name);
    free_string(room->description);

    switch (number_percent() % 6) {
    case 0:
      room->name = str_dup("A desert terrain demonic warren");
      room->description = str_dup("Dunes of sand stretch as far as the eye can see, a blazing red sun scorching the\nlandscape. Tracks both fresh and old mar the stretch of sand, hinting at a heavy\npopulation. In the distance is what looks like an oasis of some sort, the\nheaviest concentration of occupants is probably found there.");
      break;
    case 1:
      room->name = str_dup("A jungle terrain demonic warren");
      room->description = str_dup("The air is hot and humid, making breathing uncomfortable. Insects unlike those\nfound on Earth buzz loudly, just as annoying as the ones from home but possibly\nmore dangerous. Dense trees, heavy vines, and patches of quick sand\nare all obstacles here.");
      break;
    case 2:
      room->name = str_dup("A mountain terrain demonic warren");
      room->description = str_dup("Whatever lies below the fog hundreds of feet below is a mystery, but above are\nonly more and more crags and outcroppings of rock, each step precarious. Caves\ndug into the mountainside likely house any number of beings, and\nthe rumbling from above hints that the mountain is likely an active volcano.");
      break;
    case 3:
      room->name = str_dup("A swamp terrain demonic warren");
      room->description = str_dup("A damp fog rolls heavily over the ground here, obscuring the thick, boggy mire\nthat makes up the landscape. Floating logs and other more dangerous obstacles\nmove through the swampy wasteland.");
      break;
    case 4:
      room->name = str_dup("A storm-wrecked island demonic warren");
      room->description = str_dup("A small island surrounded on all sides by violently choppy black waves, the\ncenter of the island has a belching volcano in the center, and dense forestry\nset back from the shore. Rain falls heavily, obscuring vision past\nseveral feet in front of you.");
      break;
    default:
      room->name = str_dup("An industrial demonic warren");
      room->description = str_dup("Square buildings of an unknown metal release thick, oily smoke into the air.\nCaravans of some sort of vehicle transporting what can be recognized as weaponry\ntraverse the crudely paved streets, and high watchtowers have mounted\nguns sweeping the landscape.");
      break;
    }
  }

  int max_specials(CHAR_DATA *ch, int discipline) {
    int total = ch->disciplines[discipline];

    if (total < 10)
    return 0;
    else if (total < 25)
    return 1;
    else if (total < 40)
    return 2;
    else if (total < 55)
    return 3;
    else if (total < 70)
    return 4;
    else if (total < 85)
    return 5;
    else if (total < 100)
    return 6;
    else if (total < 115)
    return 7;
    else if (total < 130)
    return 8;
    else if (total < 145)
    return 9;
    else
    return 10;
  }

  _DOFUN(do_special) {
    int j, k, l;
    int uniques[15];
    int count = 0;
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (ch->pcdata->ci_discipline == 0) {
        send_to_char("You have to set a discipline first.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "none")) {
        ch->pcdata->ci_special = 0;
        send_to_char("Special cleared.\n\r", ch);
        return;
      }

      if (!str_cmp(argument, "msummon")) {
        ch->pcdata->ci_special = SPECIAL_MINION;
        printf_to_char(ch, "Special set to minion summoning\n\r");
        return;
      }
      if (!str_cmp(argument, "asummon")) {
        ch->pcdata->ci_special = SPECIAL_ALLY;
        printf_to_char(ch, "Special set to ally summoning\n\r");
        return;
      }
      for (j = 0; j < MAX_SPECIAL; j++) {
        for (l = 0; l < 15; l++)
        uniques[l] = 0;

        if (!str_cmp(argument, special_table[j].name)) {
          if (special_table[j].pc < 1) {
            send_to_char("No such special.\n\r", ch);
            return;
          }
          for (k = 0; k < 25; k++) {
            for (l = 0; l < 15; l++)
            uniques[l] = 0;

            if (ch->pcdata->ci_discipline == ch->pcdata->customstats[k][1] && ch->pcdata->customstats[k][0] == special_table[j].vnum) {
              ch->pcdata->ci_special = special_table[j].vnum;
              printf_to_char(ch, "Special set to %s\n\r", special_table[j].name);
              return;
            }
            if (ch->pcdata->ci_discipline == ch->pcdata->customstats[k][1]) {
              bool found = FALSE;
              for (l = 0; l < 15; l++) {
                if (uniques[l] == ch->pcdata->customstats[k][0])
                found = TRUE;
              }
              if (found == FALSE)
              for (l = 0; l < 15; l++) {
                if (uniques[l] == 0) {
                  uniques[l] = ch->pcdata->customstats[k][0];
                  break;
                }
              }
            }
          }
          for (l = 0; l < 15; l++) {
            if (uniques[l] > 0 && uniques[l] != SPECIAL_MINION && uniques[l] != SPECIAL_ALLY)
            count += 1;
          }
          ch->pcdata->ci_special = special_table[j].vnum;
          printf_to_char(ch, "Special set to %s\n\r", special_table[j].name);
          return;
        }
      }

      send_to_char("No such special, choices are: None, delayed, longdelayed, overpower, underpower, aoe, wound\n\r", ch);
    }
  }

  _DOFUN(do_bystandersdelayed) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (safe_strlen(from_color(argument)) > 160) {
          send_to_char("Custom attack string is too long.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_bystandersdelayed);
        ch->pcdata->ci_bystandersdelayed = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Bystandersdelayed (The message others will see when this attack's delayed effect happens.)\n\r", ch);
    }
  }
  _DOFUN(do_targetdelayed) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (safe_strlen(from_color(argument)) > 160) {
          send_to_char("Custom attack string is too long.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_targetdelayed);
        ch->pcdata->ci_targetdelayed = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Targetdelayed (The message your target will see when this attack's delayed effect happens.)\n\r", ch);
    }
  }
  _DOFUN(do_myselfdelayed) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (safe_strlen(from_color(argument)) > 160) {
          send_to_char("Custom attack string is too long.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_myselfdelayed);
        ch->pcdata->ci_myselfdelayed = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Myselfdelayed (The message you will see when this attack's delayed effect happens.)\n\r", ch);
    }
  }
  _DOFUN(do_bystanders) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (safe_strlen(from_color(argument)) > 160) {
          send_to_char("Custom attack string is too long.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_bystanders);
        ch->pcdata->ci_bystanders = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Bystanders (The message others will see when using this attack.)\n\r", ch);
    }
  }

  _DOFUN(do_target) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (safe_strlen(from_color(argument)) > 160) {
          send_to_char("Custom attack string is too long.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_target);
        ch->pcdata->ci_target = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Target (The message the target of the attack will see when using this attack.)\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 21) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (daysidle(argument) > 30) {
          send_to_char("Nobody like that is in town.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_short);
        ch->pcdata->ci_short = str_dup(argument);
        send_to_char("Done.\n\r", ch);
        return;
      }
      else
      send_to_char("Target (argument)\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (ch->pcdata->ci_discipline2 == GOAL_PSYCHIC && daysidle(argument) > 30) {
          send_to_char("Nobody like that is in town.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_message);
        ch->pcdata->ci_message = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Target (argument)\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 15) {
      ch->pcdata->ci_absorb = 1;
      if (daysidle(argument) > 30) {
        CHAR_DATA *victim;
        if ((victim = get_char_world(ch, argument)) == NULL || IS_NPC(victim)) {
          send_to_char("There is nobody like that in town.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_message);
        ch->pcdata->ci_message = str_dup(victim->name);
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_message);
        ch->pcdata->ci_message = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Target (argument)\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 17) {
      ch->pcdata->ci_absorb = 1;
      if (daysidle(argument) > 30) {
        send_to_char("There is nobody like that in town.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_short);
        ch->pcdata->ci_short = str_dup(argument);
        send_to_char("Done.\n\r", ch);
        return;
      }
      else
      send_to_char("Target (argument)\n\r", ch);
    }


  }
  _DOFUN(do_myself) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        if (safe_strlen(from_color(argument)) > 160) {
          send_to_char("Custom attack string is too long.\n\r", ch);
          return;
        }
        free_string(ch->pcdata->ci_myself);
        ch->pcdata->ci_myself = str_dup(argument);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Myself (The message you will see when using this attack.)\n\r", ch);
    }
  }

  _DOFUN(do_statement) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 15) {
      ch->pcdata->ci_absorb = 1;
      string_append(ch, &ch->pcdata->ci_desc);
      send_to_char("Enter your supporting statement for the police.\n\r", ch);
    }
  }

  int find_free_shadow(CHAR_DATA *ch) {

    for (int i = 0; i < 20; i++) {
      if (ch->pcdata->shadow_attacks[i][0] == 0 && ch->pcdata->shadow_attacks[i][1] == 0 && available_donated(ch) >= (i + 1) * 500)
      return i;
    }
    return -1;
  }

  char *get_disc_string(int val) {
    for (int j = 0; j < DIS_USED; j++) {
      if (discipline_table[j].vnum == val)
      return discipline_table[j].name;
    }
    return "None";
  }

  char *get_special_string(int val) {
    if (val == SPECIAL_ALLY)
    return "Ally summoning";
    if (val == SPECIAL_MINION)
    return "Minion summoning";

    for (int j = 0; j < MAX_SPECIAL; j++) {
      if (special_table[j].vnum == val)
      return special_table[j].name;
    }
    return "None";
  }

  _DOFUN(do_custom) {
    char arg1[MSL];
    char arg2[MSL];
    int i, j, k, l, count = 0;
    int uniques[15];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (!str_cmp(arg1, "create")) {
      if (in_fight(ch)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 3;
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg1, "list")) {
      for (i = 0; i < 25; i++) {
        if (safe_strlen(ch->pcdata->customstrings[i][0]) > 1) {
          printf_to_char(ch, "[%s]\t\t %s \t (%s)\n\r", ch->pcdata->customstrings[i][0], get_disc_string(ch->pcdata->customstats[i][1]), get_special_string(ch->pcdata->customstats[i][0]));
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "info")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0]) && safe_strlen(arg1) > 1) {
          printf_to_char(ch, "Name: %s\n\r", ch->pcdata->customstrings[i][0]);
          printf_to_char(ch, "Discipline: %s\n\r", get_disc_string(ch->pcdata->customstats[i][1]));
          printf_to_char(ch, "Special: %s\n\r", get_special_string(ch->pcdata->customstats[i][0]));
          printf_to_char(ch, "My message: %s\n\r", ch->pcdata->customstrings[i][1]);
          printf_to_char(ch, "Target message: %s\n\r", ch->pcdata->customstrings[i][2]);
          printf_to_char(ch, "Bystander message: %s\n\r", ch->pcdata->customstrings[i][3]);
          if (ch->pcdata->customstats[i][0] == SPECIAL_DELAY || ch->pcdata->customstats[i][0] == SPECIAL_DELAY2) {
            printf_to_char(ch, "My delayed message: %s\n\r", ch->pcdata->customstrings[i][4]);
            printf_to_char(ch, "Target delayed message: %s\n\r", ch->pcdata->customstrings[i][5]);
            printf_to_char(ch, "Bystander delayed message: %s\n\r", ch->pcdata->customstrings[i][6]);
          }
        }
      }
      return;
    }
    else if (!str_cmp(arg1, "rename")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          char arg[MSL];
          one_argument_nouncap(argument, arg);

          free_string(ch->pcdata->customstrings[i][0]);
          ch->pcdata->customstrings[i][0] = str_dup(arg);
          printf_to_char(ch, "Custom renamed to %s.\n\r", arg);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "delete")) {
      char tmp[MSL];
      sprintf(tmp, "%s %s", arg1, arg2);
      char tmp2[MSL];
      sprintf(tmp2, "%s %s %s", arg1, arg2, argument);
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0]) || !str_cmp(tmp, ch->pcdata->customstrings[i][0]) || !str_cmp(tmp2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][0]);
          ch->pcdata->customstrings[i][0] = str_dup("");
          ch->pcdata->customstats[i][0] = 0;
          ch->pcdata->customstats[i][1] = 0;
          printf_to_char(ch, "Custom deleted.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "self") || !str_cmp(arg1, "me") || !str_cmp(arg1, "myself")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][1]);
          ch->pcdata->customstrings[i][1] = str_dup(argument);
          printf_to_char(ch, "Custom message for self set.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "victim") || !str_cmp(arg1, "target")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][2]);
          ch->pcdata->customstrings[i][2] = str_dup(argument);
          printf_to_char(ch, "Custom message for target set.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "bystanders") || !str_cmp(arg1, "others")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][3]);
          ch->pcdata->customstrings[i][3] = str_dup(argument);
          printf_to_char(ch, "Custom message for bystanders set.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "selfdelayed") || !str_cmp(arg1, "medelayed") || !str_cmp(arg1, "myselfdelayed")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][4]);
          ch->pcdata->customstrings[i][4] = str_dup(argument);
          printf_to_char(ch, "Custom message for self set.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "victimdelayed") || !str_cmp(arg1, "targetdelayed")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][5]);
          ch->pcdata->customstrings[i][5] = str_dup(argument);
          printf_to_char(ch, "Custom message for target set.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "bystandersdelayed") || !str_cmp(arg1, "othersdelayed")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          free_string(ch->pcdata->customstrings[i][6]);
          ch->pcdata->customstrings[i][6] = str_dup(argument);
          printf_to_char(ch, "Custom message for bystanders set.\n\r");
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "discipline") || !str_cmp(arg1, "disc")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          for (j = 0; j < DIS_USED; j++) {
            if (!str_cmp(argument, discipline_table[j].name)) {
              ch->pcdata->customstats[i][1] = discipline_table[j].vnum;
              printf_to_char(ch, "Discipline set to %s\n\r", argument);
              ch->pcdata->customstats[i][0] = 0;
              return;
            }
          }
          send_to_char("Discipline not found.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg1, "shadow")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          int k = find_free_shadow(ch);
          if (k == -1) {
            send_to_char("You don't have any free shadow attacks.\n\r", ch);
            return;
          }

          for (j = 0; j < DIS_USED; j++) {
            if (!str_cmp(argument, discipline_table[j].name)) {
              ch->pcdata->shadow_attacks[k][0] = i;
              ch->pcdata->shadow_attacks[k][1] = j;
              printf_to_char(ch, "Shadow Discipline set to %s\n\r", argument);
              return;
            }
          }
          send_to_char("Discipline not found.\n\r", ch);
          return;
        }
      }
      send_to_char("Custom attack not found.\n\r", ch);
    }
    else if (!str_cmp(arg1, "unshadow")) {
      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          for (j = 0; j < 20; j++) {
            if (ch->pcdata->shadow_attacks[j][0] == i) {
              ch->pcdata->shadow_attacks[j][0] = 0;
              ch->pcdata->shadow_attacks[j][1] = 0;
              send_to_char("Cleared.\n\r", ch);
            }
          }
        }
      }
    }
    else if (!str_cmp(arg1, "special")) {
      if (in_fight(ch)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }

      for (i = 0; i < 25; i++) {
        if (!str_cmp(arg2, ch->pcdata->customstrings[i][0])) {
          if (ch->pcdata->customstats[i][1] == 0) {
            send_to_char("You have to set a discipline first.\n\r", ch);
            return;
          }
          if (!str_cmp(argument, "none")) {
            ch->pcdata->customstats[i][0] = 0;
            send_to_char("Special cleared.\n\r", ch);
            return;
          }

          if (!str_cmp(argument, "msummon")) {
            ch->pcdata->customstats[i][0] = SPECIAL_MINION;
            printf_to_char(ch, "Special set to minion summoning\n\r");
            return;
          }
          if (!str_cmp(argument, "asummon")) {
            ch->pcdata->customstats[i][0] = SPECIAL_ALLY;
            printf_to_char(ch, "Special set to ally summoning\n\r");
            return;
          }
          for (j = 0; j < MAX_SPECIAL; j++) {
            if (!str_cmp(argument, special_table[j].name)) {
              if (special_table[j].pc < 1) {
                send_to_char("No such special.\n\r", ch);
                return;
              }
              for (k = 0; k < 25; k++) {
                if (ch->pcdata->customstats[i][1] ==
                    ch->pcdata->customstats[k][1] && ch->pcdata->customstats[k][0] == special_table[j].vnum) {
                  ch->pcdata->customstats[i][0] = special_table[j].vnum;
                  printf_to_char(ch, "Special set to %s\n\r", special_table[j].name);
                  return;
                }
                if (ch->pcdata->customstats[i][1] ==
                    ch->pcdata->customstats[k][1]) {
                  bool found = FALSE;
                  for (l = 0; l < 15; l++)
                  uniques[l] = 0;
                  for (l = 0; l < 15; l++) {
                    if (uniques[l] == ch->pcdata->customstats[k][0])
                    found = TRUE;
                  }
                  if (found == FALSE)
                  for (l = 0; l < 15; l++) {
                    if (uniques[l] == 0)
                    uniques[l] = ch->pcdata->customstats[k][0];
                  }
                }
              }
              for (l = 0; l < 15; l++) {
                if (uniques[l] > 0 && uniques[l] != SPECIAL_MINION && uniques[l] != SPECIAL_ALLY)
                count += 1;
              }
              ch->pcdata->customstats[i][0] = special_table[j].vnum;
              printf_to_char(ch, "Special set to %s\n\r", special_table[j].name);
              return;
            }
          }
        }
      }
      send_to_char("No such special, choices are: None, delayed, longdelayed, overpower, underpower, aoe, wound\n\r", ch);
    }
    else
    send_to_char("Syntax: myattacks list, myattacks info <name>, myattacks create, myattacks delete <name>, myattacks rename <name> <newname>, myattacks discipline <name> <discipline>, myattacks special <name> <special>, myattacks myself <name> <message>, myattacks target <name> <message>, myattacks bystanders <name> <message>, myattacks myselfdelayed <name> <message>, myattacks targetdelayed <name> <message>, myattacks bystandersdelayed <name> <message>\n\r", ch);
  }

  _DOFUN(do_hurt) {

    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];
    int max = 0, dam = 0;
    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }
    if (!battleground(ch->in_room))
    return;

    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_world(ch, arg1);

    if (victim == NULL)
    return;

    if (!IS_NPC(victim)) {
      if (victim == NULL || get_gmtrust(ch, victim) < 1) {
        send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);

      if (!is_number(argument) || atoi(argument) <= 0) {
        send_to_char("Syntax: hurt (character) flat/percent (amount)\n\r", ch);
        return;
      }
      if (higher_power(ch) && power_bound(ch) && !IS_NPC(victim) && str_cmp(ch->pcdata->place, victim->pcdata->place)) {
        send_to_char("Your power is bound.\n\r", ch);
        return;
      }

      int val = get_gmtrust(ch, victim);
      if (val == 3)
      max = max_hp(victim);
      else if (val == 2)
      max = max_hp(victim) / 2;
      else if (val == 1)
      max = max_hp(victim) / 5;
      else
      max = max_hp(victim) / 20;

    }
    else if (victim->controled_by == ch) {
      if (!is_number(argument) || atoi(argument) <= 0) {
        send_to_char("Syntax: hurt (character) flat/percent (amount)\n\r", ch);
        return;
      }
      max = max_hp(victim);
    }
    else
    return;

    if (!str_cmp(arg2, "flat")) {
      dam = atoi(argument);
    }
    else if (!str_cmp(arg2, "percent")) {
      dam = atoi(argument) * max_hp(victim) / 100;
    }
    else {
      send_to_char("Syntax: hurt (character) flat/percent (amount)\n\r", ch);
      return;
    }
    dam = UMIN(dam, max);
    damage(victim, victim, dam);
    act("You damage $N.", ch, NULL, victim, TO_CHAR);
    printf_to_char(victim, "You take %d damage.", dam);
  }

  _DOFUN(do_injure) {

    CHAR_DATA *victim;
    char arg1[MSL];
    int max = 0;
    if (!is_gm(ch) && !higher_power(ch)) {
      send_to_char("This is a storyrunner command.\n\r", ch);
      return;
    }
    if (battleground(ch->in_room))
    return;
    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_world(ch, arg1);

    if (victim == NULL)
    return;

    if (!IS_NPC(victim)) {
      if (victim == NULL || get_gmtrust(ch, victim) < 1) {
        send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      if (higher_power(ch) && power_bound(ch) && !IS_NPC(victim) && str_cmp(ch->pcdata->place, victim->pcdata->place)) {
        send_to_char("Your power is bound.\n\r", ch);
        return;
      }

      int val = get_gmtrust(ch, victim);
      max = val;
    }
    else if (victim->controled_by && victim->controled_by == ch) {
      max = 4;
    }
    else
    return;

    if (!str_cmp(argument, "mild")) {
      if (max >= 1) {
        if (victim->wounds < 1) {
          victim->wounds = 1;
          victim->heal_timer = 30000;
          send_to_char("You suffer a mild wound.\n\r", victim);
        }
      }
      else {
        send_to_char("You can't hurt them any more.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(argument, "severe")) {
      if (max >= 2) {
        if (victim->wounds < 2) {
          victim->wounds = 2;
          victim->heal_timer = 115000;
          send_to_char("You suffer a severe wound.\n\r", victim);
          miscarriage(victim, FALSE);
        }
      }
      else {
        send_to_char("You can't hurt them any more.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(argument, "knockout")) {
      if (max >= 2) {
        victim->pcdata->sleeping = 120;
        send_to_char("You pass out.\n\r", victim);
      }
      else {
        send_to_char("You can't hurt them any more.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(argument, "critical") && !higher_power(ch)) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOKILL))
      return;
      if (max >= 2) {
        if (IS_NPC(victim))
        raw_kill(victim, victim);
        else if (victim->wounds < 3) {
          victim->wounds = 3;
          victim->heal_timer = 4300;
          victim->death_timer = 720;
          send_to_char("You suffer a critical wound!\n\r", victim);
          if (!IS_NPC(victim)) {
            free_string(victim->pcdata->deathcause);
            victim->pcdata->deathcause = str_dup("Unclear");
          }

          miscarriage(victim, FALSE);
        }
      }
      else {
        send_to_char("You can't hurt them any more.\n\r", ch);
        return;
      }
    }
    else if (!str_cmp(argument, "fatal") && !higher_power(ch)) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOKILL))
      return;
      if (max >= 3) {
        // changed this from raw to real - Discorance
        real_kill(victim, victim);
        if (!IS_NPC(victim)) {
          free_string(victim->pcdata->deathcause);
          victim->pcdata->deathcause = str_dup("Unclear");
        }
      }
      else {
        send_to_char("You can't hurt them any more.\n\r", ch);
        return;
      }
    }
    else {
      send_to_char("Syntax: injure (person) mild/severe/critical/fatal/knckout\n\r", ch);
      return;
    }
    send_to_char("Done.\n\r", ch);
  }

  void killplayer(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return;
    if (is_gm(victim))
    return;

    if (ch == victim)
    return;

    if(!str_cmp(ch->pcdata->fixation_name, victim->name))
    {
      free_string(ch->pcdata->fixation_name);
      ch->pcdata->fixation_name = str_dup("");
      ch->pcdata->fixation_timeout = 0;
      ch->pcdata->fixation_mourning = current_time + (3600*24*14);
    }

    if (victim->in_room != NULL && !in_haven(victim->in_room))
    return;

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = 12 * 60 * 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_MARKED;
    affect_to_char(ch, &af);

    if (seems_super(victim)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 60 * 40);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_SUPERTAINT;
      affect_to_char(ch, &af);
    }
    if (!is_undead(victim) && !guestmonster(victim) && !IS_FLAG(victim->act, PLR_GUEST))
    coverup_cost(ch, 3);

    if (ch->faction != 0 && victim->played / 3600 > 10 && ch->played / 3600 > 5 && !is_abom(victim) && !guestmonster(victim) && !IS_FLAG(victim->act, PLR_GUEST)) {
      if (victim->desc == NULL) {
        send_message(
        ch->faction, "Headquarters are unhappy at the rising death toll in Haven.");
      }
      else {
        send_message(
        ch->faction, "Headquarters are unhappy at the rising death toll in Haven.");
      }
    }

    if (in_haven(ch->in_room)) {
      if (victim->race == RACE_CIVILIAN) {
        int val = victim->played - ch->playedcopfree / 2;
        val /= 60;
        val += 1200;

        val = UMAX(val, 1200);

        ch->pcdata->police_timer += val;
        ch->playedcopfree = 0;
        ch->pcdata->police_intensity += 1;
        ch->pcdata->police_number += 2;
      }
    }
  }

  void critplayer(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return;
    if (is_gm(victim))
    return;

    if (ch == victim)
    return;

    if (victim->in_room != NULL && !in_haven(victim->in_room))
    return;

    if (seems_super(victim)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = (12 * 60 * 40);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_SUPERTAINT;
      affect_to_char(ch, &af);
    }

    if (in_haven(ch->in_room)) {
      if (victim->race == RACE_CIVILIAN) {
        int val = victim->played - ch->playedcopfree / 2;
        val /= 60;
        val += 1200;

        val = UMAX(val, 1200);
        ch->pcdata->police_timer += val;
        ch->playedcopfree = 0;
        ch->pcdata->police_intensity += 1;
        ch->pcdata->police_number += 2;
      }
    }
  }

  void kidnap(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return;
    if (is_gm(victim))
    return;

    if (ch == victim)
    return;

    if (in_haven(ch->in_room)) {

      if (victim->race == RACE_CIVILIAN) {
        int val = victim->played - ch->playedcopfree / 2;
        val /= 60;
        val += 1200;

        val = UMAX(val, 1200);

        ch->pcdata->police_timer += val;
        ch->playedcopfree = 0;
        ch->pcdata->police_intensity += 1;
        ch->pcdata->police_number += 2;
      }
    }
  }

  void scopspank(CHAR_DATA *ch) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int number = 0;
    int j;
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->storycon[i][0] == SCON_POLICE) {
        number = ch->pcdata->storycon[i][1];
        j = i;
      }
    }
    if (number <= 0)
    return;

    for (int i = 0; i < number; i++) {
      if (number % 10 == 0)
      pMobIndex = get_mob_index(NATIONAL_GUARD);
      else if (number % 5 == 0)
      pMobIndex = get_mob_index(POLICE_SWAT);
      else
      pMobIndex = get_mob_index(POLICE_OFFICER);

      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->x = number_range(0, ch->in_room->size);
      mob->y = number_range(0, ch->in_room->size);
      free_string(mob->aggression);
      mob->aggression = str_dup(ch->name);
      mob->fighting = TRUE;
    }
    start_fight(mob, ch);
    ch->pcdata->storycon[j][2] += 720;
    ch->pcdata->storycon[j][1]++;
  }

  void shuntspank(CHAR_DATA *ch) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int number = 0;
    int j;
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->storycon[i][0] == SCON_HUNTERS) {
        number = ch->pcdata->storycon[i][1];
        j = i;
      }
    }
    if (number <= 0)
    return;

    for (int i = 0; i < number; i++) {
      if (number % 3 == 0)
      pMobIndex = get_mob_index(HUNTER_CHAMP);
      else
      pMobIndex = get_mob_index(HUNTER);

      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->x = number_range(0, ch->in_room->size);
      mob->y = number_range(0, ch->in_room->size);
      free_string(mob->aggression);
      mob->aggression = str_dup(ch->name);
      mob->fighting = TRUE;
    }
    start_fight(mob, ch);
    ch->pcdata->storycon[j][2] += 720;
    ch->pcdata->storycon[j][1]++;
  }

  void copspank(CHAR_DATA *ch) {
    send_to_char("6", ch);

    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    for (int i = 0; i < ch->pcdata->police_number; i++) {
      if (ch->pcdata->police_intensity >= 10)
      pMobIndex = get_mob_index(NATIONAL_GUARD);
      else if (ch->pcdata->police_intensity >= 2)
      pMobIndex = get_mob_index(POLICE_SWAT);
      else
      pMobIndex = get_mob_index(POLICE_OFFICER);

      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->x = number_range(0, ch->in_room->size);
      mob->y = number_range(0, ch->in_room->size);
      free_string(mob->aggression);
      mob->aggression = str_dup(ch->name);
      mob->fighting = TRUE;
    }
    start_fight(mob, ch);
    ch->pcdata->police_timer += 720;
    ch->pcdata->police_number++;
  }

  int super_pop(ROOM_INDEX_DATA *room) {
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

      if (is_vampire(victim) || is_werewolf(victim) || is_mage(victim))
      pop++;
    }
    return pop;
  }

  void shuntcheck(CHAR_DATA *ch) {

    if (ch == NULL || ch->in_room == NULL)
    return;
    if (IS_NPC(ch))
    return;

    if (!has_con(ch, SCON_HUNTERS))
    return;

    if (in_fight(ch))
    return;

    if (IS_FLAG(ch->comm, COMM_REPORTED)) {
      shuntspank(ch);
      return;
    }

    if (ch->pcdata->police_catch_timer < 60)
    return;

    if (IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      shuntspank(ch);
      return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && sunphase(ch->in_room) != 0) {
      shuntspank(ch);
      return;
    }

    if (in_house(ch) != NULL && !str_cmp(in_house(ch)->owner, ch->name)) {
      shuntspank(ch);
      return;
    }

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if (!str_cmp((*it)->author, ch->name)) {
        shuntspank(ch);
        return;
      }
    }

    if (super_pop(ch->in_room) > 2) {
      shuntspank(ch);
      return;
    }
  }

  void scopcheck(CHAR_DATA *ch) {

    if (ch == NULL || ch->in_room == NULL)
    return;
    if (IS_NPC(ch))
    return;

    if (!has_con(ch, SCON_POLICE))
    return;

    if (in_fight(ch))
    return;

    if (is_safe(ch, ch))
    return;

    if (IS_FLAG(ch->comm, COMM_REPORTED)) {
      scopspank(ch);
      return;
    }

    if (ch->pcdata->police_catch_timer < 60)
    return;

    if (IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      scopspank(ch);
      return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && sunphase(ch->in_room) != 0) {
      scopspank(ch);
      return;
    }

    if (in_house(ch) != NULL && !str_cmp(in_house(ch)->owner, ch->name)) {
      scopspank(ch);
      return;
    }
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if (!str_cmp((*it)->author, ch->name)) {
        scopspank(ch);
        return;
      }
    }
  }

  void copcheck(CHAR_DATA *ch) {

    if (ch == NULL || ch->in_room == NULL)
    return;
    if (IS_NPC(ch))
    return;

    scopcheck(ch);
    shuntcheck(ch);

    if (ch->pcdata->police_timer <= 0 || ch->pcdata->police_number <= 0 || ch->pcdata->police_intensity <= 0)
    return;

    if (in_fight(ch))
    return;

    ch->pcdata->police_timer -= 3;

    if (ch->pcdata->police_timer <= 0) {
      ch->pcdata->police_timer = 0;
      ch->pcdata->police_number = 0;
      ch->pcdata->police_intensity = 0;
    }

    if (is_safe(ch, ch))
    return;

    if (IS_FLAG(ch->comm, COMM_REPORTED)) {
      copspank(ch);
      send_to_char("1", ch);
      return;
    }

    if (ch->pcdata->police_catch_timer < 60)
    return;

    if (IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      copspank(ch);
      send_to_char("2", ch);

      return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && sunphase(ch->in_room) != 0) {
      copspank(ch);
      send_to_char("3", ch);

      return;
    }

    if (in_house(ch) != NULL && !str_cmp(in_house(ch)->owner, ch->name)) {
      copspank(ch);
      send_to_char("4", ch);

      return;
    }

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if (!str_cmp((*it)->author, ch->name)) {
        copspank(ch);
        send_to_char("5", ch);

        return;
      }
    }
  }

  _DOFUN(do_report) {
    CHAR_DATA *victim;

    if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
      if (!IS_IMMORTAL(ch)) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      else {
        if ((victim = get_char_world_pc(argument)) == NULL) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
      }
    }
    if (IS_NPC(victim)) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    send_to_char("You report them.\n\r", ch);

    if (!spammer(ch) && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTELL)) {
      send_to_char("Someone believes you have violated Haven's emote standards, please read help emote standards.\n\r", victim);
      CHAR_DATA *rch;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        if (d->connected != CON_PLAYING)
        continue;
        rch = CH(d);
        if (rch == NULL || IS_NPC(rch))
        continue;
        if (!IS_IMMORTAL(rch))
        continue;
        printf_to_char(rch, "%s reports %s.\n\r", ch->name, victim->name);
        char buf[MSL];
        sprintf(buf, "REPORT: %s reports %s.\n\r", ch->name, victim->name);
        log_string(buf);
      }
    }
  }

  void summon_support(CHAR_DATA *ch, int type) {

    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    int i;

    if (type == SPECIAL_MINION)
    i = 0;
    else if (type == SPECIAL_ALLY)
    i = 1;
    else
    return;

    if (type == SPECIAL_MINION) {
      if (current_minion_cost(ch, i) > minion_exp_cap(ch)) {
        send_to_char("You fail to summon your minion, over experience cap.\n\r", ch);
        return;
      }
      if (making_minion_shield(ch) > minion_shield_cap(ch)) {
        send_to_char("You fail to summon your minion, over shield cap.\n\r", ch);
        return;
      }
      if (!valid_minion(ch)) {
        send_to_char("You fail to summon your minion, illegal disciplines.\n\r", ch);
        return;
      }
      for (int x = 0; x < DIS_USED; x++) {
        if (discipline_table[x].range > 0) {
          for (int y = 0; y < 10; y += 2) {
            if (ch->pcdata->monster_discs[y][i] == discipline_table[x].vnum && discipline_table[x].vnum != DIS_CLAW && discipline_table[x].vnum != DIS_FIRE && discipline_table[x].vnum != DIS_ICE && discipline_table[x].vnum != DIS_LIGHTNING) {
              send_to_char("You fail to summon your minion, illegal disciplines.\n\r", ch);
              return;
            }
            if (get_skill(ch, SKILL_ELEMINIONS) < 1 && ch->pcdata->monster_discs[y][i] == discipline_table[x].vnum && discipline_table[x].vnum != DIS_CLAW) {
              send_to_char("You fail to summon your minion, illegal disciplines.\n\r", ch);
              return;
            }
          }
        }
      }

      int phase = sunphase(ch->in_room);
      if (get_skill(ch, SKILL_CALLNATURE) > 0 && is_natural(ch->in_room) && (ch->lf_used + ch->lf_taken < 1500)) {
        if (ch->pcdata->spectre != 0)
        use_lifeforce(ch, 100 + (200 * ch->pcdata->minion_limit), "Minion summoning.");
        else
        use_lifeforce(ch, 150 + (300 * ch->pcdata->minion_limit), "Minion summoning.");
        ch->pcdata->minion_limit++;
      }
      else if (get_skill(ch, SKILL_CALLGRAVE) > 0 && (phase == 0 || phase == 1 || phase == 7) && (ch->lf_used + ch->lf_taken < 1500)) {
        if (ch->pcdata->spectre != 0)
        use_lifeforce(ch, 100 * (ch->pcdata->minion_limit + 1), "Minion summoning");
        else
        use_lifeforce(ch, 150 * (ch->pcdata->minion_limit + 1), "Minion summoning");

        if (ch->pcdata->minion_limit == 0)
        ch->pcdata->minion_limit++;
        else
        ch->pcdata->minion_limit *= 2;
      }
      else {
        if (ch->pcdata->minion_limit > 0) {
          send_to_char("You fail to summon your minion, you've already done that in this fight.\n\r", ch);
          return;
        }
        ch->pcdata->minion_limit++;
        if (ch->pcdata->spectre != 0)
        use_lifeforce(ch, 100, "Minion summoning.");
        else
        use_lifeforce(ch, 150, "Minion summoning.");
      }
    }
    else if (type == SPECIAL_ALLY) {
      if (current_minion_cost(ch, i) > muscle_exp_cap(ch)) {
        send_to_char("You fail to summon your ally, over experience cap.\n\r", ch);
        return;
      }

      if (ch->pcdata->ally_limit >= muscle_count(ch)) {
        send_to_char("You fail to summon your ally, no allies left to help you.\n\r", ch);
        return;
      }
      if (!valid_ally(ch)) {
        send_to_char("You fail to summon your ally, illegal disciplines.\n\r", ch);
        return;
      }
      if (is_mute(ch)) {
        send_to_char("You fail to summon your ally, you cannot speak.\n\r", ch);
        return;
      }
      ch->pcdata->ally_limit++;
    }
    if (type == SPECIAL_MINION)
    pMobIndex = get_mob_index(MINION_TEMPLATE);
    else
    pMobIndex = get_mob_index(ALLY_TEMPLATE);

    mob = create_mobile(pMobIndex);
    char_to_room(mob, ch->in_room);
    mob->hit = max_hp(mob);
    mob->ttl = 10;
    free_string(mob->short_descr);
    mob->short_descr = str_dup(ch->pcdata->monster_names[1][i]);
    free_string(mob->name);
    mob->name = str_dup(from_color(ch->pcdata->monster_names[1][i]));
    free_string(mob->long_descr);
    mob->long_descr = str_dup(ch->pcdata->monster_names[1][i]);
    free_string(mob->description);
    mob->description = str_dup(ch->pcdata->monster_names[2][i]);

    mob->disciplines[ch->pcdata->monster_discs[0][i]] =
    ch->pcdata->monster_discs[1][i];
    mob->disciplines[ch->pcdata->monster_discs[2][i]] =
    ch->pcdata->monster_discs[3][i];
    mob->disciplines[ch->pcdata->monster_discs[4][i]] =
    ch->pcdata->monster_discs[5][i];
    mob->disciplines[ch->pcdata->monster_discs[6][i]] =
    ch->pcdata->monster_discs[7][i];
    mob->disciplines[ch->pcdata->monster_discs[8][i]] =
    ch->pcdata->monster_discs[9][i];

    mob->lifeforce = get_lifeforce(ch, FALSE, NULL) * 100;

    if (mob->disciplines[DIS_RIFLES] >= 10)
    mob->disciplines[DIS_RIFLES] -= 5;

    mob->hit = max_hp(mob);
    mob->x = ch->x;
    mob->y = ch->y;
    mob->faction = ch->faction;
    free_string(mob->protecting);
    mob->protecting = str_dup(ch->name);

    if (IS_FLAG(ch->act, PLR_SHROUD))
    SET_FLAG(mob->act, PLR_SHROUD);
    if (IS_FLAG(ch->act, PLR_DEEPSHROUD))
    SET_FLAG(mob->act, PLR_DEEPSHROUD);

    mob->in_fight = TRUE;
    if (in_fight(ch)) {
      mob->attacking = 1;
      mob->fighting = TRUE;
      mob->attack_timer = FIGHT_WAIT * fight_speed(mob) / 2;
      mob->move_timer = FIGHT_WAIT * fight_speed(mob) / 2;
      mob->fight_fast = ch->fight_fast;
      mob->fight_speed = ch->fight_speed;
      mob->hadturn = FALSE;
    }
    printf_to_char(ch, "%s shows up.\n\r", mob->long_descr);
  }
  void bodyguard_check(CHAR_DATA *ch, CHAR_DATA *victim) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    if (IS_NPC(victim))
    return;

    if (is_sparring(victim) || battleground(victim->in_room))
    return;

    if (IS_NPC(ch) && ch->pIndexData->vnum != HAND_SOLDIER)
    return;

    if (!in_fight(victim))
    return;

    if (!same_fight(ch, victim))
    return;

    if (!in_fight(victim))
    return;

    if (!same_fight(ch, victim))
    return;

    if(victim->pcdata->spectre > 0)
    return;

    if (victim->pcdata->guard_number < 1 || victim->pcdata->guard_expiration < current_time) {
      if (in_world(victim) == WORLD_OTHER && victim->pcdata->other_fame_level > 0 && victim->hit >= max_hp(victim)) {
        victim->pcdata->guard_number =
        victim->pcdata->other_fame_level * victim->pcdata->other_fame_level;
        victim->pcdata->guard_faction = victim->faction;
      }
      else if (in_world(victim) == WORLD_HELL && victim->pcdata->hell_fame_level > 0 && victim->hit >= max_hp(victim)) {
        victim->pcdata->guard_number =
        victim->pcdata->hell_fame_level * victim->pcdata->hell_fame_level;
        victim->pcdata->guard_faction = victim->faction;
      }
      else if (in_world(victim) == WORLD_GODREALM && victim->pcdata->godrealm_fame_level > 0 && victim->hit >= max_hp(victim)) {
        victim->pcdata->guard_number = victim->pcdata->godrealm_fame_level *
        victim->pcdata->godrealm_fame_level;
        victim->pcdata->guard_faction = victim->faction;
      }
      else if (in_world(victim) == WORLD_WILDS && victim->pcdata->wilds_fame_level > 0 && victim->hit >= max_hp(victim)) {
        victim->pcdata->guard_number =
        victim->pcdata->wilds_fame_level * victim->pcdata->wilds_fame_level;
        victim->pcdata->guard_faction = victim->faction;
      }
      else
      return;
    }

    if (clan_lookup(victim->pcdata->guard_faction) == NULL || safe_strlen(clan_lookup(victim->pcdata->guard_faction)->soldier_name) < 5)
    return;

    if (ch->faction == victim->pcdata->guard_faction || ch->factiontwo == victim->pcdata->guard_faction)
    return;

    for (int i = 0; i < victim->pcdata->guard_number; i++) {
      pMobIndex = get_mob_index(HAND_SOLDIER);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 10;
      free_string(mob->short_descr);
      mob->short_descr =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_name);
      free_string(mob->name);
      mob->name =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_name);
      free_string(mob->long_descr);
      mob->long_descr =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_name);
      free_string(mob->description);
      mob->description =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_desc);
      mob->faction = victim->pcdata->guard_faction;
      mob->hit = max_hp(mob);
      mob->x = victim->x;
      mob->y = victim->y;
      free_string(mob->protecting);
      mob->protecting = str_dup(victim->name);

      if (IS_FLAG(victim->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);
      if (IS_FLAG(victim->act, PLR_DEEPSHROUD))
      SET_FLAG(mob->act, PLR_DEEPSHROUD);

      if (in_fight(victim)) {
        mob->attacking = 1;
        mob->fighting = TRUE;
        mob->attack_timer = FIGHT_WAIT * fight_speed(mob);
        mob->move_timer = FIGHT_WAIT * fight_speed(mob);
        mob->fight_fast = victim->fight_fast;
        mob->fight_speed = victim->fight_speed;
        mob->hadturn = TRUE;
      }
    }
    victim->pcdata->guard_number = 0;
  }

  bool bodyguard_abduct(CHAR_DATA *ch, CHAR_DATA *victim) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    if (IS_NPC(victim))
    return FALSE;

    if (IS_NPC(ch))
    return FALSE;

    if (victim->pcdata->guard_number < 1 || victim->pcdata->guard_expiration < current_time)
    return FALSE;

    if (clan_lookup(victim->pcdata->guard_faction) == NULL || safe_strlen(clan_lookup(victim->pcdata->guard_faction)->soldier_name) < 5)
    return FALSE;

    if (ch->faction == victim->pcdata->guard_faction || ch->factiontwo == victim->pcdata->guard_faction)
    return FALSE;

    for (int i = 0; i < victim->pcdata->guard_number; i++) {
      pMobIndex = get_mob_index(HAND_SOLDIER);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, ch->in_room);
      mob->hit = max_hp(mob);
      mob->ttl = 10;
      free_string(mob->short_descr);
      mob->short_descr =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_name);
      free_string(mob->name);
      mob->name =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_name);
      free_string(mob->long_descr);
      mob->long_descr =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_name);
      free_string(mob->description);
      mob->description =
      str_dup(clan_lookup(victim->pcdata->guard_faction)->soldier_desc);
      mob->faction = victim->pcdata->guard_faction;
      mob->hit = max_hp(mob);
      mob->x = victim->x;
      mob->y = victim->y;
      free_string(mob->protecting);
      mob->protecting = str_dup(victim->name);
      free_string(mob->aggression);
      mob->aggression = str_dup(ch->name);
      if (IS_FLAG(ch->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD))
      SET_FLAG(mob->act, PLR_DEEPSHROUD);

      if (in_fight(ch)) {
        mob->attacking = 1;
        mob->fighting = TRUE;
        mob->attack_timer = FIGHT_WAIT * fight_speed(mob);
        mob->move_timer = FIGHT_WAIT * fight_speed(mob);
        mob->fight_fast = ch->fight_fast;
        mob->fight_speed = ch->fight_speed;
        mob->hadturn = TRUE;
      }
      else
      start_fight(mob, ch);
    }
    victim->pcdata->guard_number = 0;
    return TRUE;
  }

  void summon_cobj(ROOM_INDEX_DATA *room, int vnum, int duration, int x, int y, CHAR_DATA *summoner) {

    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    pMobIndex = get_mob_index(vnum);
    mob = create_mobile(pMobIndex);
    char_to_room(mob, room);
    mob->hit = max_hp(mob);
    mob->ttl = 5;
    mob->in_fight = TRUE;
    mob->x = x;
    mob->y = y;

    if (in_fight(summoner)) {
      mob->attacking = 1;
      mob->fighting = TRUE;
      mob->in_fight = TRUE;
      mob->fight_fast = summoner->fight_fast;
      mob->attack_timer = duration * FIGHT_WAIT * fight_speed(summoner);
    }
    if (IS_FLAG(summoner->act, PLR_SHROUD) && !IS_FLAG(mob->act, PLR_SHROUD))
    SET_FLAG(mob->act, PLR_SHROUD);
    if (!IS_FLAG(summoner->act, PLR_SHROUD) && IS_FLAG(mob->act, PLR_SHROUD))
    REMOVE_FLAG(mob->act, PLR_SHROUD);
    if (IS_FLAG(summoner->act, PLR_DEEPSHROUD) && !IS_FLAG(mob->act, PLR_DEEPSHROUD))
    SET_FLAG(mob->act, PLR_DEEPSHROUD);
    if (!IS_FLAG(summoner->act, PLR_DEEPSHROUD) && IS_FLAG(mob->act, PLR_DEEPSHROUD))
    REMOVE_FLAG(mob->act, PLR_DEEPSHROUD);
  }

  bool is_in_cover(CHAR_DATA *ch) {
    CHAR_DATA *victim;

    if (IS_NPC(ch) && (IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET)))
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (!IS_FLAG(victim->act, ACT_COVER) || !IS_NPC(victim))
      continue;

      if (victim->x + 1 < ch->x || victim->y + 1 < ch->y || victim->x - 1 > ch->x || victim->y - 1 > ch->y)
      continue;

      return TRUE;
    }
    return FALSE;
  }

  CHAR_DATA *get_cover(CHAR_DATA *ch) {
    CHAR_DATA *victim;

    if (IS_NPC(ch) && (IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET)))
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (!IS_FLAG(victim->act, ACT_COVER) || !IS_NPC(victim))
      continue;

      if (victim->x + 1 < ch->x || victim->y + 1 < ch->y || victim->x - 1 > ch->x || victim->y - 1 > ch->y)
      continue;

      return victim;
    }
    return NULL;
  }

  void explode_grenade(CHAR_DATA *ch) {
    act("`RA grenade explodes.`x", ch, NULL, NULL, TO_ROOM);
    CHAR_DATA *victim;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (!in_fight(victim))
      continue;
      if (victim == NULL || is_gm(victim))
      continue;
      if (!same_fight(ch, victim))
      continue;

      if (IS_FLAG(victim->act, ACT_COMBATOBJ) && IS_NPC(victim))
      continue;

      if (ch == victim)
      continue;

      if (combat_distance(ch, victim, TRUE) > 10)
      continue;

      combat_damage(victim, ch, max_hp(victim) / 5, DIS_FIRE);
      act("You are blasted by shrapnel from a nearby explosion", victim, NULL, NULL, TO_CHAR);
    }
  }

  void dead_explode(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    act("`R$n explodes.`x", ch, NULL, NULL, TO_ROOM);
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (!in_fight(victim))
      continue;
      if (victim == NULL || is_gm(victim))
      continue;
      if (!same_fight(ch, victim))
      continue;

      if (IS_FLAG(victim->act, ACT_COMBATOBJ) && IS_NPC(victim))
      continue;

      if (combat_distance(ch, victim, TRUE) > 10)
      continue;
      if (ch == victim)
      continue;

      combat_damage(victim, ch, max_hp(victim) / 5, DIS_FIRE);
      act("You are blasted by shrapnel from a nearby explosion", victim, NULL, NULL, TO_CHAR);
    }
  }

  bool is_protecter(CHAR_DATA *ch) {

    CHAR_DATA *victim;

    if (IS_NPC(ch) && (IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET)))
    return FALSE;

    if (IS_NPC(ch))
    return FALSE;

    if (ch->pcdata->protecting == NULL)
    return FALSE;

    if (ch->hit < 1)
    return FALSE;

    if (is_helpless(ch))
    return FALSE;

    if (ch->debuff >= 75)
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;
      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim) && (IS_FLAG(victim->act, ACT_COVER) || IS_FLAG(victim->act, ACT_TURRET)))
      continue;

      if (victim->x + 1 < ch->x || victim->y + 1 < ch->y || victim->x - 1 > ch->x || victim->y - 1 > ch->y)
      continue;

      if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD))
      continue;

      if (ch->pcdata->protecting != NULL && ch->pcdata->protecting == victim)
      return TRUE;
    }
    return FALSE;
  }

  bool is_protected(CHAR_DATA *ch) {
    CHAR_DATA *protector;

    CHAR_DATA *victim;

    if (IS_NPC(ch) && (IS_FLAG(ch->act, ACT_COVER) || IS_FLAG(ch->act, ACT_TURRET)))
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      protector = victim;

      if (IS_NPC(protector))
      continue;

      if (protector->hit < 1)
      continue;

      if (is_helpless(protector))
      continue;

      if (protector->debuff >= 75)
      continue;

      if (protector->x + 1 < ch->x || protector->y + 1 < ch->y || protector->x - 1 > ch->x || protector->y - 1 > ch->y)
      continue;

      if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD))
      continue;

      if (protector->pcdata->protecting != NULL && protector->pcdata->protecting == ch)
      return TRUE;
    }
    return FALSE;
  }

  CHAR_DATA *get_protector(CHAR_DATA *ch) {
    CHAR_DATA *protector;

    CHAR_DATA *victim;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      protector = victim;

      if (is_helpless(protector))
      continue;

      if (IS_NPC(protector))
      continue;

      if (protector->debuff >= 75)
      continue;

      if (protector->x + 1 < ch->x || protector->y + 1 < ch->y || protector->x - 1 > ch->x || protector->y - 1 > ch->y)
      continue;

      if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD))
      continue;

      if (protector->pcdata->protecting != NULL && protector->pcdata->protecting == ch)
      return protector;
    }
    return NULL;
  }
  /*
int get_mapx(CHAR_DATA *ch, int size)
{
int base = 19;
if(size == MAP_LARGE)
base = 7;
else if(size == MAP_MEDIUM)
base = 3;
else if(size == MAP_SMALL)
base = 1;
int dis = base*(ch->x+ch->disx)/ch->in_room->size;
return dis;
}

int get_mapy(CHAR_DATA *ch, int size)
{
int base = 11;
if(size == MAP_LARGE)
base = 5;
if(size == MAP_MEDIUM)
base = 3;
if(size == MAP_SMALL)
base = 2;
int dis = base*(ch->y+ch->disy)/ch->in_room->size;
return dis;
}

void displace(CHAR_DATA *ch, CHAR_DATA *to, int size)
{
if(get_mapy(ch, size) != get_mapy(to, size))
return;

if(get_mapx(ch, size) > get_mapx(to, size) + 1)
return;
if(get_mapx(to, size) > get_mapx(ch, size) + 1)
return;

if(get_mapx(ch, size) < get_mapx(to, size) - 1)
return;
if(get_mapx(to, size) < get_mapx(ch, size) - 1)
return;


if(ch->x+ch->disx > to->x+to->disx)
{
if(number_percent() % 2 == 0 && ch->x+ch->disx < ch->in_room->size)
ch->disx++;
else if(to->x+to->disx > 0)
to->disx--;
}
else if(ch->y+ch->disy > to->y+to->disy)
{
if(number_percent() % 2 == 0 && ch->y+ch->disy < ch->in_room->size)
ch->disy++;
else if(to->y+to->disy > 0)
to->disx--;
}
else if(ch->x+ch->disx < to->x+to->disx)
{
if(number_percent() % 2 == 0 && ch->x+ch->disx > 0)
ch->disx--;
else if(to->x+to->disx < ch->in_room->size)
to->disx++;
}
else if(ch->y+ch->disy < to->y+to->disy)
{
if(number_percent() % 2 == 0 && ch->y+ch->disy > 0)
ch->disy--;
else if(to->y+to->disy < ch->in_room->size)
to->disy++;
}
else if(ch->x+ch->disx == to->x+to->disx)
{
if(number_percent() % 4 == 0 && ch->x+ch->disx < ch->in_room->size)
ch->disx++;
else if(number_percent() % 3 == 0 && to->x+to->disx > 0)
to->disx--;
else if(number_percent() % 2 == 0 && ch->x+ch->disx > 0)
ch->disx--;
else if(to->x+to->disx < ch->in_room->size)
to->disx++;
}
else if(ch->y+ch->disy == to->y+to->disy)
{
if(number_percent() % 4 == 0 && ch->y+ch->disy < ch->in_room->size)
ch->disy++;
else if(number_percent() % 3 == 0 && to->y+to->disy > 0)
to->disy--;
else if(number_percent() % 2 == 0 && ch->y+ch->disy > 0)
ch->disy--;
else if(to->y+to->disy < ch->in_room->size)
to->disy++;
}
displace(ch, to, size);
}

*/

  int setup_expand(int number) {
    long start = (long)(number);
    start *= 3;
    start = start * start * start;
    int val = (int)(sqrt(start));
    return val;
  }

  void setup_translation() {
    map_translation[0] = 0;
    for (int i = 1; i < 50; i++) {
      map_translation[i] = setup_expand(i);
    }
  }
  int difference(int first, int second) {
    int diff = first - second;
    if (diff < 0)
    diff *= -1;
    return diff;
  }

  int map_expand(int number) {
    bool negative = FALSE;
    int val;
    if (number < 0) {
      number *= -1;
      negative = TRUE;
    }
    if (number < 50)
    val = map_translation[number];
    else
    val = 1000;

    if (negative == TRUE)
    val *= -1;
    return val;
  }
  int map_contract(int number) {
    bool negative = FALSE;
    int val = -1;
    if (number < 0) {
      number *= -1;
      negative = TRUE;
    }

    if (difference(number, map_translation[49]) <=
        difference(number, map_translation[48]))
    val = 49;
    else if (difference(number, map_translation[0]) <
        difference(number, map_translation[1]))
    val = 0;
    else {
      for (int i = 1; i < 49; i++) {
        if (difference(number, map_translation[i]) <=
            difference(number, map_translation[i - 1]) && difference(number, map_translation[i]) <
            difference(number, map_translation[i + 1]))
        val = i;
      }
    }
    if (val == -1)
    return 0;
    if (negative == TRUE)
    val *= -1;
    return val;
  }

  int default_mapsize(CHAR_DATA *looker) {
    int maxdist = 0;
    if (!in_fight(looker))
    return 11;

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;

      if (!in_fight(rch))
      continue;
      if (rch == NULL || is_gm(rch))
      continue;
      if (!same_fight(rch, looker))
      continue;

      if (combat_distance(looker, rch, FALSE) > maxdist) {
        maxdist = combat_distance(looker, rch, FALSE);
        // printf_to_char(looker, "Found: %s, %d\n\r", rch->name, // combat_distance(looker, rch, FALSE));
      }
    }
    int size = map_contract(maxdist);
    // printf_to_char(looker, "Base size: %d, dist %d\n\r", size, maxdist);
    size *= 2;
    size += 4;
    if (size % 2 == 0)
    size--;

    if (size > 31)
    size = 31;
    if (size < 11)
    size = 11;
    return size;
  }

  CHAR_DATA *get_mapch(CHAR_DATA *ch, int size, int mapy, int mapx) {
    int newx = mapx / 2;
    int newy = mapy;
    int offset = size - 1;
    offset /= 2;
    newx -= offset;
    newy -= offset;

    int xmax = map_expand(newx + 1);
    int xmin = map_expand(newx);
    int ymax = map_expand(newy + 1);
    int ymin = map_expand(newy);

    CHAR_DATA *newvict;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;

      if (!can_see_char_distance(ch, newvict, DISTANCE_MEDIUM))
      continue;

      if (!can_map_see(ch, newvict))
      continue;

      int relx = relative_x(ch, newvict->in_room, newvict->x);
      int rely = relative_y(ch, newvict->in_room, newvict->y);

      if (relx < xmax && relx >= xmin && rely < ymax && rely >= ymin) {
        //	printf_to_char(ch, "Found: Name: %s,  mapy: %d, mapx: %d, relx:
        //%d, rely: %d, newx: %d, newy: %d, xmin: %d, ymin: %d, xmax: %d, ymax:
        //%d\n\r", newvict->name, mapy, mapx, relx, rely, newx, newy,xmin, ymin, //xmax, ymax);
        return newvict;
      }
    }
    return NULL;
  }
  int mapch_count(CHAR_DATA *ch, int size, int mapy, int mapx) {
    int newx = mapx / 2;
    int newy = mapy;
    int count = 0;
    int offset = size - 1;
    offset /= 2;
    newx -= offset;
    newy -= offset;

    int xmax = map_expand(newx + 1);
    int xmin = map_expand(newx);
    int ymax = map_expand(newy + 1);
    int ymin = map_expand(newy);

    CHAR_DATA *newvict;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;

      if (!can_see_char_distance(ch, newvict, DISTANCE_MEDIUM))
      continue;

      if (!can_map_see(ch, newvict))
      continue;

      int relx = relative_x(ch, newvict->in_room, newvict->x);
      int rely = relative_y(ch, newvict->in_room, newvict->y);
      if (relx < xmax && relx >= xmin && rely < ymax && rely >= ymin) {
        count++;
      }
    }
    return count;
  }

  int name_count(CHAR_DATA *ch, CHAR_DATA *looker) {
    int count = 1;
    char first;
    char pfirst;
    char colbuf[MSL];

    remove_color(colbuf, PERS_2(ch, looker));
    if ((colbuf[0] == 'a' && colbuf[1] == ' ') || (colbuf[0] == 'A' && colbuf[1] == ' ')) {
      pfirst = colbuf[2];
    }
    else if ((colbuf[0] == 'a' && colbuf[1] == 'n' && colbuf[2] == ' ') || (colbuf[0] == 'A' && colbuf[1] == 'n' && colbuf[2] == ' ')) {
      pfirst = colbuf[3];
    }
    else {
      pfirst = colbuf[0];
    }

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || is_gm(rch))
      continue;
      if (!can_see_char_distance(looker, rch, DISTANCE_MEDIUM))
      continue;

      if (!can_map_see(looker, rch))
      continue;

      remove_color(colbuf, PERS(rch, looker));
      if ((colbuf[0] == 'a' && colbuf[1] == ' ') || (colbuf[0] == 'A' && colbuf[1] == ' ')) {
        first = colbuf[2];
      }
      else if ((colbuf[0] == 'a' && colbuf[1] == 'n' && colbuf[2] == ' ') || (colbuf[0] == 'A' && colbuf[1] == 'n' && colbuf[2] == ' ')) {
        first = colbuf[3];
      }
      else {
        first = colbuf[0];
      }

      if (rch == looker)
      continue;

      if (rch == ch)
      return count;

      if (first == pfirst)
      count++;
    }
    return count;
  }
  char *mapname(CHAR_DATA *rch, CHAR_DATA *ch) {
    char buf[MSL];
    char *temp;
    char colbuf[MSL];
    if (ch == rch)
    return "`WMe`x";
    char first;
    char second;

    remove_color(colbuf, PERS(rch, ch));

    if ((colbuf[0] == 'a' && colbuf[1] == ' ') || (colbuf[0] == 'A' && colbuf[1] == ' ')) {
      first = colbuf[2];
      second = colbuf[3];
    }
    else if ((colbuf[0] == 'a' && colbuf[1] == 'n' && colbuf[2] == ' ') || (colbuf[0] == 'A' && colbuf[1] == 'n' && colbuf[2] == ' ')) {
      first = colbuf[3];
      second = colbuf[4];
    }
    else {
      first = colbuf[0];
      second = colbuf[1];
    }

    int count = name_count(rch, ch);
    if (count == 1) {
      if (get_agg(ch, rch) > 0 || get_agg(rch, ch) > 0) {
        sprintf(buf, "`R%c%c`x", first, second);
      }
      else
      sprintf(buf, "`G%c%c`x", first, second);
    }
    else {
      if (get_agg(ch, rch) > 0 || get_agg(rch, ch) > 0) {
        sprintf(buf, "`R%c%d`x", first, count);
      }
      else
      sprintf(buf, "`G%c%d`x", first, count);
    }

    temp = str_dup(buf);
    return temp;
  }

  bool can_map_see(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(victim) && victim->pIndexData->vnum == COBJ_LANDMINE && combat_distance(ch, victim, FALSE) > 10)
    return FALSE;
    /*
if(IS_NPC(victim) && IS_FLAG(victim->act, ACT_COMBATOBJ))
return FALSE;
*/
    if (has_caff(ch, CAFF_CLOAKBLIND) && ch->cloaked != NULL && ch->cloaked == victim)
    return FALSE;

    if (!in_fight(ch) && !can_see(ch, victim))
    return FALSE;

    if (is_ghost(victim) && get_skill(ch, SKILL_CLAIRVOYANCE) < 2)
    return FALSE;

    if (is_ghost(victim) && is_possessing(victim))
    return FALSE;

    if (ch->level < victim->invis_level && IS_FLAG(victim->act, PLR_ROOMINVIS) && ch->in_room == victim->in_room)
    return FALSE;

    if (ch->level < victim->invis_level && IS_FLAG(victim->act, PLR_WHOINVIS) && ch->in_room != victim->in_room)
    return FALSE;

    if (has_caff(ch, CAFF_STASIS)) {
      if (ch->fight_fast == TRUE) {
        if (ch->attack_timer > 0 && ch->move_timer > 0)
        return FALSE;
      }
      else {
        if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK) && IS_FLAG(ch->fightflag, FIGHT_NOMOVE))
        return FALSE;
      }
    }

    CHAR_DATA *cobj;

    for (CharList::iterator it = victim->in_room->people->begin();
    it != victim->in_room->people->end(); ++it) {
      cobj = *it;

      if (cobj == NULL)
      continue;

      if (cobj->in_room == NULL)
      continue;

      if (!IS_FLAG(cobj->act, ACT_COMBATOBJ) || !IS_NPC(cobj))
      continue;

      if (combat_distance(ch, victim, FALSE) <= 10)
      continue;

      if (cobj->pIndexData->vnum == COBJ_SMOKE && get_dist(ch->x, ch->y, victim->x, victim->y) > 10) {
        if (combat_distance(ch, cobj, FALSE) <= 10)
        return FALSE;
        if (!IS_NPC(victim) || victim->pIndexData->vnum != COBJ_SMOKE) {
          if (combat_distance(victim, cobj, FALSE) <= 10)
          return FALSE;
        }
      }
    }

    return TRUE;
  }

  int maptox(int size, int mapx) {
    int newx = mapx / 2;
    int offset = size - 1;
    offset /= 2;
    newx -= offset;
    int xdiff = map_expand(newx);
    return xdiff;
  }
  int maptoy(int size, int mapy) {
    int newy = mapy;
    int offset = size - 1;
    offset /= 2;
    newy -= offset;
    int ydiff = map_expand(newy);
    return ydiff;
  }
  int maproomx(CHAR_DATA *ch, int size, int mapx) {
    int offset = maptox(size, mapx);
    offset += ch->x;
    int xmove = 0;
    if (offset > 50) {
      for (; offset > 50;) {
        offset -= 50;
        xmove++;
      }
    }
    if (offset < 0) {
      for (; offset < 0;) {
        offset += 50;
        xmove--;
      }
    }
    return ch->in_room->x + xmove;
  }
  int maproomy(CHAR_DATA *ch, int size, int mapy) {
    int offset = maptoy(size, mapy);
    offset += ch->y;
    int ymove = 0;
    if (offset > 50) {
      for (; offset > 50;) {
        offset -= 50;
        ymove++;
      }
    }
    if (offset < 0) {
      for (; offset < 0;) {
        offset += 50;
        ymove--;
      }
    }
    return ch->in_room->y + ymove;
  }
  char *mapfill(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    if (can_get_to(ch, room)) {
      if (IS_SET(room->room_flags, ROOM_INDOORS)) {
        if (is_dark(room))
        return "`D/";
        else if (public_room(room))
        return "`W/";
        else
        return "`x/";
      }
      if (room->sector_type == SECT_UNDERWATER)
      return "`D}";
      if (room->sector_type == SECT_WATER)
      return "`B}";
      if (room->sector_type == SECT_SHALLOW)
      return "`c}";
      if (is_dark(room)) {
        if (room->sector_type == SECT_PARK || room->sector_type == SECT_FOREST || room->sector_type == SECT_BEACH || room->sector_type == SECT_CEMETARY || room->sector_type == SECT_DIRT)
        return "`D+";
        if (room->sector_type == SECT_STREET || room->sector_type == SECT_PARKING || room->sector_type == SECT_ALLEY || room->sector_type == SECT_SIDEWALK)
        return "`D=";
      }
      else {
        if (room->sector_type == SECT_PARK || room->sector_type == SECT_CEMETARY)
        return "`g+";
        if (room->sector_type == SECT_FOREST)
        return "`g+";
        if (room->sector_type == SECT_BEACH || room->sector_type == SECT_DIRT)
        return "`y+";
        if (room->sector_type == SECT_STREET && IS_SET(room->room_flags, ROOM_DIRTROAD))
        return "`y=";
        else if (room->sector_type == SECT_STREET)
        return "`c=";
        else if (room->sector_type == SECT_ALLEY)
        return "`D=";
        else if (room->sector_type == SECT_PARKING)
        return "`c+";
        else if (room->sector_type == SECT_SIDEWALK)
        return "`x=";
      }
      return "`x+";
    }
    if (room->exit[DIR_DOWN] != NULL && room->exit[DIR_DOWN]->u1.to_room != NULL && can_get_to(ch, room->exit[DIR_DOWN]->u1.to_room)) {
      if (is_dark(room->exit[DIR_DOWN]->u1.to_room))
      return "`Dv";
      else if (public_room(room->exit[DIR_DOWN]->u1.to_room))
      return "`Wv";
      else
      return "`yv";
    }
    if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL) {
      if (is_dark(room->exit[DIR_UP]->u1.to_room))
      return "`D^";
      else if (can_get_to(ch, room->exit[DIR_UP]->u1.to_room))
      return "`y^";
      else
      return "`C^";
    }
    return "`D#";
  }

  void draw_map(CHAR_DATA *ch, int size) {
    static char buf[MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    buf[0] = '\0';
    int i, j;
    send_to_char("`D ", ch);

    int maxi = size - 1;
    int mini = 0;
    int maxj = size * 2;
    int minj = 0;

    if (ch->facing == DIR_NORTH || ch->facing == DIR_NORTHEAST || ch->facing == DIR_NORTHWEST) {
      mini = maxi / 4;
    }
    if (ch->facing == DIR_SOUTH || ch->facing == DIR_SOUTHEAST || ch->facing == DIR_SOUTHWEST) {
      maxi = maxi * 3 / 4;
    }
    if (ch->facing == DIR_EAST || ch->facing == DIR_NORTHEAST || ch->facing == DIR_SOUTHEAST) {
      minj = maxj / 4;
    }
    if (ch->facing == DIR_WEST || ch->facing == DIR_NORTHWEST || ch->facing == DIR_SOUTHWEST) {
      maxj = maxj * 3 / 4;
    }

    for (i = 0; i < (maxj - minj); i++)
    send_to_char("_", ch);
    send_to_char("`x\n\r", ch);
    ROOM_INDEX_DATA *mroom = ch->in_room;
    for (i = maxi; i >= mini; i--) {
      strcat(buf, "`D|");
      for (j = minj; j < maxj; j++) {
        int ppoint = poitype(ch, size, i, j);
        if (ppoint == POI_EXTRACT)
        strcat(buf, "`YE");
        else if (ppoint == POI_CAPTURE)
        strcat(buf, "`YC");
        else {
          rch = get_mapch(ch, size, i, j);
          if (rch == NULL) {
            if (i == -1)
            strcat(buf, "`D_`x");
            else {
              if (!invisioncone_coordinates(ch, maptox(size, j), maptoy(size, i))) {
                if (i == mini)
                strcat(buf, "`D_");
                else
                strcat(buf, " ");
              }
              else {
                int xdiff = maproomx(ch, size, j);
                int ydiff = maproomy(ch, size, i);
                if (mroom->x == xdiff && mroom->y == ydiff)
                strcat(buf, mapfill(ch, mroom));
                else {
                  mroom = sourced_room_by_coordinates(mroom, xdiff, ydiff, mroom->z, FALSE);
                  if (mroom == NULL) {
                    strcat(buf, "`D#");
                    mroom = ch->in_room;
                  }
                  else
                  strcat(buf, mapfill(ch, mroom));
                }
              }
            }
          }
          else {
            strcat(buf, mapname(rch, ch));
            rch->mapcount = mapch_count(ch, size, i, j);
            j++;
          }
        }
      }
      strcat(buf, "`D|`x\n");
    }
    send_to_char(buf, ch);
    send_to_char("`x\n\r", ch);
    scan_fight(ch, TRUE);
  }

  void init_map(ROOM_INDEX_DATA *room, int size) {
    return;
    /*
for(CharList::iterator it = room->people->begin();
it != room->people->end(); ++it)
{
CHAR_DATA *rch = *it;

if(rch == NULL || IS_IMMORTAL(rch) || IS_FLAG(rch->act, PLR_GM))
continue;
rch->disx = 0;
rch->disy = 0;
}
for(CharList::iterator it = room->people->begin();
it != room->people->end(); ++it)
{
CHAR_DATA *rch = *it;

if(rch == NULL || IS_IMMORTAL(rch) || IS_FLAG(rch->act, PLR_GM))
continue;

for(CharList::iterator tt = room->people->begin();
tt != room->people->end(); ++tt)
{
CHAR_DATA *to = *tt;

if(to == NULL || IS_IMMORTAL(to) || IS_FLAG(to->act, PLR_GM))
continue;

if(to == rch)
continue;

displace(rch, to, size);
}
}
for(CharList::iterator it = room->people->begin();
it != room->people->end(); ++it)
{
CHAR_DATA *rch = *it;

if(rch == NULL || IS_IMMORTAL(rch) || IS_FLAG(rch->act, PLR_GM))
continue;

for(CharList::iterator tt = room->people->begin();
tt != room->people->end(); ++tt)
{
CHAR_DATA *to = *tt;

if(to == NULL || IS_IMMORTAL(to) || IS_FLAG(to->act, PLR_GM))
continue;

if(to == rch)
continue;

displace(rch, to, size);
}
}
*/
  }

  _DOFUN(do_map) {
    if (!in_fight(ch)) {
      send_to_char("`yTown`g:`x `chttp://havenrpg.net/town.php`x\n\r", ch);
      send_to_char("`gNorth Forest`y:`x `chttp://havenrpg.net/northforest.php`x\n\r", ch);
      send_to_char("`gSouth Forest`y:`x `chttp://havenrpg.net/southforest.php`x\n\r", ch);
      send_to_char("`gWest Forest`y:`x `chttp://havenrpg.net/westforest.php`x\n\r", ch);

      return;
    }

    if (ch->in_room == NULL || ch->in_room->size < 1)
    return;

    draw_map(ch, default_mapsize(ch));
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }
  _DOFUN(do_knockout) {
    if (in_fight(ch)) {
      static char buf[200];
      sprintf(buf, "%s knockout", argument);
      do_function(ch, &do_attack, buf);
    }
    else
    do_function(ch, &do_knock, argument);
  }
  _DOFUN(do_charge) {
    static char buf[200];
    sprintf(buf, "%s charge", argument);
    do_function(ch, &do_move, buf);
  }
  _DOFUN(do_flee) {
    /*
if (ch->pcdata->patrol_status == PATROL_INIT_PREY && ch->pcdata->patrol_target != NULL) {
act("You run away from $n. You can use hide to try to find a hiding spot at any time.", ch->pcdata->patrol_target, NULL, ch, TO_VICT);
act("$N runs away from you.", ch->pcdata->patrol_target, NULL, ch, TO_CHAR);
ch->pcdata->patrol_status = PATROL_FLEEING;
ch->pcdata->patrol_timer = 60;
ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_CHASING;
ch->pcdata->patrol_target->pcdata->patrol_timer =
15 + 2 * (get_speed(ch) - get_speed(ch->pcdata->patrol_target));
ch->pcdata->patrol_target->pcdata->patrol_timer =
UMAX(ch->pcdata->patrol_target->pcdata->patrol_timer, 5);
}
else if (ch->pcdata->patrol_status == PATROL_HIDING && ch->pcdata->patrol_target != NULL) {
act("You run away from $n. You can use hide to try to find a hiding spot at any time.", ch->pcdata->patrol_target, NULL, ch, TO_VICT);
act("$N bursts out of $S hiding spot and runs away from you.", ch->pcdata->patrol_target, NULL, ch, TO_CHAR);
ch->pcdata->patrol_status = PATROL_FLEEING;
ch->pcdata->patrol_timer = 60;
ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_CHASING;
ch->pcdata->patrol_target->pcdata->patrol_timer +=
1 + 2 * (get_speed(ch) - get_speed(ch->pcdata->patrol_target));
ch->pcdata->patrol_target->pcdata->patrol_timer =
UMAX(ch->pcdata->patrol_target->pcdata->patrol_timer, 1);
}
else if (ch->pcdata->patrol_status != 0) {
send_to_char("You're a bit busy.\n\r", ch);
return;
}
*/
    do_function(ch, &do_move, "flee");
  }
  _DOFUN(do_sprint) { do_function(ch, &do_move, "sprint"); }

  _DOFUN(do_retreat) {

    if (in_fistfight(ch) && !in_fight(ch)) {
      ch->fistattack = FIST_RETREAT;
      ch->fisttimer += 15;
      send_to_char("You'll back out of the fight.\n\r", ch);
      act("$n starts backing away.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    static char buf[200];
    sprintf(buf, "%s retreat", argument);
    do_function(ch, &do_move, buf);
  }
  _DOFUN(do_protect) {
    static char buf[200];
    sprintf(buf, "%s protect", argument);
    do_function(ch, &do_move, buf);
  }
  _DOFUN(do_jump) {
    static char buf[200];
    sprintf(buf, "%s jump", argument);
    do_function(ch, &do_move, buf);
  }

  int GET_NPC_SPECIAL(int dis) {
    if (dis == DIS_TELEKINESIS && number_percent() % 4 == 0)
    return SPECIAL_KNOCKBACK;
    if (dis == DIS_PISTOLS && number_percent() % 2 == 0)
    return SPECIAL_DAMAGE;
    if (dis == DIS_THROWN && number_percent() % 2 == 0)
    return SPECIAL_DAMAGE;
    if (dis == DIS_BOWS && number_percent() % 2 == 0)
    return SPECIAL_DAMAGE;
    if (dis == DIS_ENERGY && number_percent() % 4 == 0)
    return SPECIAL_ENTRAP;
    if (dis == DIS_FIRE && number_percent() % 4 == 0)
    return SPECIAL_OVERPOWER;
    if (dis == DIS_ICE && number_percent() % 4 == 0)
    return SPECIAL_ENTRAP;
    if (dis == DIS_NATURE && number_percent() % 4 == 0)
    return SPECIAL_ENTRAP;
    if (dis == DIS_LIGHTNING && number_percent() % 4 == 0)
    return SPECIAL_KNOCKBACK;
    if (dis == DIS_WEATHER && number_percent() % 4 == 0)
    return SPECIAL_KNOCKBACK;
    if (dis == DIS_DARK && number_percent() % 6 == 0)
    return SPECIAL_LIFESTEAL;
    if (dis == DIS_TELEPATHY && number_percent() % 4 == 0)
    return SPECIAL_WEAKEN;
    if (dis == DIS_SHOTGUNS && number_percent() % 4 == 0)
    return SPECIAL_KNOCKBACK;
    if (dis == DIS_SONIC && number_percent() % 4 == 0)
    return SPECIAL_KNOCKBACK;
    if (dis == DIS_RADIATION && number_percent() % 2 == 0)
    return SPECIAL_DAMAGE;
    if (dis == DIS_STONE && number_percent() % 4 == 0)
    return SPECIAL_KNOCKBACK;
    if (dis == DIS_LONGBLADE && number_percent() % 2 == 0)
    return SPECIAL_DAMAGE;
    if (dis == DIS_KNIFE && number_percent() % 2 == 0)
    return SPECIAL_GRAPPLE;
    if (dis == DIS_CLAW && number_percent() % 2 == 0)
    return SPECIAL_GRAPPLE;
    if (dis == DIS_STRIKING && number_percent() % 6 == 0)
    return SPECIAL_UNDERPOWER;
    if (dis == DIS_FIREF && number_percent() % 6 == 0)
    return SPECIAL_OVERPOWER;
    if (dis == DIS_ICEF && number_percent() % 2 == 0)
    return SPECIAL_GRAPPLE;
    if (dis == DIS_ENERGYF && number_percent() % 2 == 0)
    return SPECIAL_GRAPPLE;
    if (dis == DIS_DARKF && number_percent() % 6 == 0)
    return SPECIAL_LIFESTEAL;
    if (dis == DIS_BRUTE && number_percent() % 12 == 0)
    return SPECIAL_STUN;

    return 0;
  }

  // This seemed like handy thing to have instead of duplicating it everywhere -
  // Discordance
  void apply_marked(CHAR_DATA *ch, int length) {
    if (length == 0)
    length = 12 * 60 * 50;

    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = 0;
    af.level = 10;
    af.duration = length;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.caster = NULL;
    af.weave = FALSE;
    af.bitvector = AFF_MARKED;
    affect_to_char(ch, &af);
  }

  // Changed do_taint to use apply_mark
  _DOFUN(do_taint) {
    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (ch == victim) {
      apply_marked(ch, 12 * 60 * 50);
      act("You taint yourself for your crimes.", ch, NULL, NULL, TO_CHAR);
      act("$n acknowledges being tainted by their crime.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    if (IS_IMMORTAL(ch)) {
      if (IS_AFFECTED(victim, AFF_MARKED)) {
        AFFECT_DATA *paf = NULL, *paf_next = NULL;

        for (paf = victim->affected; paf != NULL; paf = paf_next) {
          paf_next = paf->next;

          if (paf->bitvector == AFF_MARKED || paf->bitvector == AFF_MARKED2 || paf->bitvector == AFF_MARKED3) {
            affect_remove(victim, paf);
          }
        }
        act("You remove the taint from $N.", ch, NULL, victim, TO_CHAR);
        return;
      }

      apply_marked(victim, 12 * 60 * 50);
      act("You taint $N for their crimes.", ch, NULL, victim, TO_CHAR);
      return;
    }

    send_to_char("Someone thinks you should be tainted for what you've done.\n\r", victim);
    send_to_char("Request sent.\n\r", ch);
  }

  _DOFUN(do_guards) {

    CHAR_DATA *guard;
    CHAR_DATA *victim;
    int amount = 100;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (in_fistfight(ch) && !in_fight(ch)) {
      if (!str_cmp(arg1, "high")) {
        send_to_char("You guard high.\n\r", ch);
        ch->guard = GUARD_HIGH;
      }
      else if (!str_cmp(arg1, "low")) {
        send_to_char("You guard low.\n\r", ch);
        ch->guard = GUARD_LOW;
      }
      else {
        send_to_char("Syntax: guard high/low.\n\r", ch);
      }
      return;
    }
    if (!str_cmp(arg1, "attack")) {

      if ((victim = get_char_fight(ch, argument)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        guard = *it;

        if (!in_fight(guard))
        continue;
        if (guard == NULL || is_gm(guard))
        continue;
        if (!same_fight(ch, guard))
        continue;

        if (!IS_NPC(guard))
        continue;

        if (str_cmp(guard->protecting, ch->name))
        continue;

        if (guard->hit > 0) {
          if (victim->target == guard) {
            victim->target_dam += amount;
          }
          else if (victim->target_2 == guard) {
            victim->target_dam_2 += amount;
          }
          else if (victim->target_3 == guard) {
            victim->target_dam_3 += amount;
          }
          else {
            victim->target_3 = victim->target_2;
            victim->target_dam_3 = victim->target_dam_2;
            victim->target_2 = victim->target;
            victim->target_dam_2 = victim->target_dam;
            victim->target = guard;
            victim->target_dam = amount;
          }
        }
      }

      send_to_char("Order given.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "dismiss")) {
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        guard = *it;

        if (!in_fight(guard))
        continue;
        if (guard == NULL || is_gm(guard))
        continue;
        if (!same_fight(ch, guard))
        continue;

        if (!IS_NPC(guard))
        continue;

        if (str_cmp(guard->protecting, ch->name))
        continue;

        extract_char(guard, TRUE);
        send_to_char("Order given.\n\r", ch);
        return;
      }
    }
  }

  _DOFUN(do_autoskip) {
    if (ch->pcdata->autoskip == 1) {
      ch->pcdata->autoskip = 0;
      send_to_char("You no longer auto skip your turn in combat.\n\r", ch);
      return;
    }
    else {
      ch->pcdata->autoskip = 1;
      send_to_char("You'll now autoskip your turn in combat.\n\r", ch);
      return;
    }
  }

  _DOFUN(do_showfight) {
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || IS_IMMORTAL(rch))
      continue;

      printf_to_char(ch, "Name: %s, Move: %d, Attack: %d", rch->name, rch->move_timer, rch->attack_timer);
      if (is_peaceful(rch))
      printf_to_char(ch, "Peaceful, ");
      if (will_agg(rch, ch))
      printf_to_char(ch, "Will agg, ");
      if (get_npc_target(rch) != NULL)
      printf_to_char(ch, "Target %s, AggT: %d, ", get_npc_target(rch)->name, get_agg(rch, get_npc_target(rch)));
      printf_to_char(ch, "Agg: %d, AggNew: %d\n\r", get_agg(rch, ch), 0);
      printf_to_char(ch, "ADisc: %d, ADam: %d, DDisc: %d\n\r", rch->attackdisc, rch->attackdam, rch->defensedisc);
    }
  }

  void death_cause(CHAR_DATA *ch, CHAR_DATA *victim, int disc) {
    if (IS_NPC(victim))
    return;

    switch (disc) {
    case DIS_PISTOLS:
    case DIS_RIFLES:
    case DIS_CARBINES:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Numerous bullet wounds.");
      break;
    case DIS_SHOTGUNS:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Shotgun wounds.");
      break;
    case DIS_BLUNT:
    case DIS_STRIKING:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Blunt Force Trauma.");
      break;
    case DIS_LONGBLADE:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Sword wounds.");
      break;
    case DIS_KNIFE:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Knife Wounds.");
      break;
    case DIS_GRAPPLE:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Strangulation.");
      break;
    default:
      free_string(victim->pcdata->deathcause);
      victim->pcdata->deathcause = str_dup("Physical trauma");
      break;
    }
  }

  void apply_caff(CHAR_DATA *ch, int aff, int rounds) {
    if (aff == CAFF_STUNNED && ch->fight_fast == TRUE) {
      ch->attack_timer += FIGHT_WAIT * fight_speed(ch) * rounds;
      ch->move_timer += FIGHT_WAIT * fight_speed(ch) * rounds;
      return;
    }
    if (aff == CAFF_ROOT && ch->fight_fast == TRUE) {
      ch->move_timer += FIGHT_WAIT * fight_speed(ch) * rounds;
      return;
    }

    int i;
    for (i = 0; i < 30; i++) {
      if (ch->caff[i] == 0 || ch->caff_duration[i] <= 0) {
        ch->caff[i] = aff;
        ch->caff_duration[i] = FIGHT_WAIT * rounds * fight_speed(ch) * 3 / 2;
        return;
      }
    }
  }
  void lower_caff(CHAR_DATA *ch, int aff) {
    int i;
    for (i = 0; i < 30; i++) {
      if (ch->caff[i] == aff && ch->caff_duration[i] > 0) {
        ch->caff_duration[i] -= FIGHT_WAIT * fight_speed(ch) * 3 / 2;
      }
    }
  }
  bool has_caff(CHAR_DATA *ch, int aff) {
    int i;
    for (i = 0; i < 30; i++) {
      if (ch->caff[i] == aff && ch->caff_duration[i] > 0)
      return TRUE;
    }
    return FALSE;
  }

  _DOFUN(do_caff) {
    int i;
    for (i = 0; i < 30; i++) {
      if (ch->caff[i] > 0)
      printf_to_char(ch, "%d: %d\n\r", ch->caff[i], ch->caff_duration[i]);
    }
  }

  void remove_caff(CHAR_DATA *ch, int aff) {
    int i;
    for (i = 0; i < 30; i++) {
      if (ch->caff[i] == aff && ch->caff_duration[i] > 0) {
        ch->caff[i] = 0;
        ch->caff_duration[i] = 0;
      }
    }
  }

  OBJ_DATA *get_shield_obj(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (IS_SET(obj->extra_flags, ITEM_ARMORED) && is_name("shield", obj->name))
        return obj;
      }
    }
    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (IS_SET(obj->extra_flags, ITEM_ARMORED))
        return obj;
      }
    }

    return FALSE;
  }

  OBJ_DATA *get_lmelee(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_WEAPON && obj->size >= 25 && real_weapon(obj))
    return obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_WEAPON && obj->size >= 25)
    return obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25 && real_weapon(obj))
      return obj;
    }
    return NULL;
  }

  OBJ_DATA *get_lranged(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_RANGED && obj->size >= 25)
    return obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_RANGED && obj->size >= 25)
    return obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25)
      return obj;
    }
    return NULL;
  }

  OBJ_DATA *get_smelee(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_WEAPON && obj->size < 25 && real_weapon(obj))
    return obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_WEAPON && obj->size < 25 && real_weapon(obj))
    return obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size < 25 && real_weapon(obj))
      return obj;
    }
    return NULL;
  }

  OBJ_DATA *get_sranged(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_RANGED && obj->size < 25)
    return obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_RANGED && obj->size < 25)
    return obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size < 25)
      return obj;
    }
    return NULL;
  }

  void weaken_character(CHAR_DATA *ch, int type, int disc) {
    int debuff = 0;
    if (type == AMMO_SILVER && is_werewolf(ch)) {
      debuff = 40;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_WOOD && is_vampire(ch)) {
      debuff = 35;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_IRON && is_faeborn(ch)) {
      debuff = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_QUARTZ && is_demonborn(ch)) {
      debuff = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_OBSIDIAN && is_angelborn(ch)) {
      debuff = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_RADIOACTIVE && is_demigod(ch)) {
      debuff = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (debuff > 0) {
      debuff += 10;
      if (disc == DIS_KNIFE)
      debuff -= 5;
      if (disc == DIS_BLUNT)
      debuff -= 15;
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_THROWN)
      debuff += 10;
      if (disc == DIS_CARBINES)
      debuff += 25;
      if (disc == DIS_SHOTGUNS)
      debuff += 25;
      if (disc == DIS_BOWS)
      debuff += 15;
    }
    if (get_skill(ch, SKILL_SILVERVULN) > 0 || get_skill(ch, SKILL_GOLDVULN) > 0 || get_skill(ch, SKILL_BONEVULN) > 0 || get_skill(ch, SKILL_WOODVULN) > 0)
    debuff /= 3;
    int special = 0;
    if (type == AMMO_SILVER && get_skill(ch, SKILL_SILVERVULN) > 0) {
      special = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_GOLD && get_skill(ch, SKILL_GOLDVULN) > 0) {
      special = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_BONE && get_skill(ch, SKILL_BONEVULN) > 0) {
      special = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (type == AMMO_WOOD && get_skill(ch, SKILL_WOODVULN) > 0) {
      special = 30;
      act("$n staggers.", ch, NULL, NULL, TO_ROOM);
      act("You stagger.", ch, NULL, NULL, TO_CHAR);
      dact("$n staggers.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
    if (special > 0) {
      special += 20;
      if (disc == DIS_KNIFE)
      special -= 5;
      if (disc == DIS_BLUNT)
      special -= 10;
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS)
      special += 10;
      if (disc == DIS_CARBINES || disc == DIS_THROWN)
      special += 15;
      if (disc == DIS_SHOTGUNS)
      special += 20;
      if (disc == DIS_BOWS)
      special += 15;
    }
    debuff += special;

    ch->debuff += debuff;
    if (!IS_NPC(ch) && (disc == DIS_BOWS || disc == DIS_KNIFE || disc == DIS_SPEAR)) {
      if (type == AMMO_PARALYTIC) {
        apply_caff(ch, CAFF_PARALYTICTOXIN, 10);
        act("$n is poisoned.", ch, NULL, NULL, TO_ROOM);
        act("You are poisoned.", ch, NULL, NULL, TO_CHAR);
        dact("$n is poisoned.", ch, NULL, NULL, DISTANCE_MEDIUM);
      }
      if (type == AMMO_POISON) {
        apply_caff(ch, CAFF_POISONTOXIN, 10);
        act("$n is poisoned.", ch, NULL, NULL, TO_ROOM);
        act("You are poisoned.", ch, NULL, NULL, TO_CHAR);
        dact("$n is poisoned.", ch, NULL, NULL, DISTANCE_MEDIUM);
      }
    }
  }

  int process_augments(CHAR_DATA *ch, CHAR_DATA *victim, bool lower, int disc, int dam) {
    if (forest_monster(ch) && lower == TRUE) {
      victim->debuff += 5;
      return dam;
    }

    if (get_skill(ch, SKILL_HYPERSTRENGTH) && lower && dam >= 10) {
      if (disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_SPEAR || disc == DIS_KNIFE || disc == DIS_STRIKING || disc == DIS_GRAPPLE) {
        victim->debuff += 25;
        if (disc == DIS_GRAPPLE || disc == DIS_STRIKING)
        victim->debuff += 10;
      }
    }

    if (disc == DIS_CUSTOM)
    return process_augments(ch, victim, lower, custom_vnum(ch), dam);

    if (disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_SPEAR) {
      if (get_lmelee(ch) != NULL && get_lmelee(ch)->buff > 0) {
        int atype = get_lmelee(ch)->value[2];
        if (atype == AMMO_AUGMENTED)
        dam = dam * 12 / 10;
        else
        dam = dam * 8 / 10;

        if (dam >= 8 && lower)
        weaken_character(victim, atype, disc);

        if (lower) {
          if (!is_sparring(ch) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
          get_lmelee(ch)->buff--;
        }
      }
      if (dam >= 8 && lower) {
        if (get_skill(victim, SKILL_BLADEDEFENSE) <= 0) {

          if (disc == DIS_LONGBLADE)
          victim->debuff += 15;
          else if (disc == DIS_BLUNT)
          victim->debuff += 25;
          else if (disc == DIS_SPEAR)
          victim->debuff += 10;
        }
        if (get_skill(victim, SKILL_RIPOSTE) > 0) {
          if (default_melee(victim) == disc)
          combat_damage(victim, ch, dam / 5, default_melee(victim));
          else
          combat_damage(victim, ch, dam / 10, default_melee(victim));
        }

        if (has_caff(ch, CAFF_ATHUNDER)) {
          if (ch->pcdata->divine_focus == CAFF_ATHUNDER)
          victim->debuff += 60;
          else
          victim->debuff += 30;
        }
      }
    }
    else if (disc == DIS_KNIFE || disc == DIS_THROWN) {
      if (get_smelee(ch) != NULL && get_smelee(ch)->buff > 0) {
        int atype = get_smelee(ch)->value[2];
        if (atype == AMMO_AUGMENTED)
        dam = dam * 12 / 10;
        else
        dam = dam * 8 / 10;

        if (dam >= 8 && lower)
        weaken_character(victim, atype, disc);

        if (lower) {
          if (!is_sparring(ch) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
          get_smelee(ch)->buff--;
        }
      }
      if (dam >= 8 && lower) {
        if (disc == DIS_KNIFE) {
          if (get_skill(victim, SKILL_BLADEDEFENSE) <= 0)
          victim->debuff += 5;

          if (get_skill(victim, SKILL_RIPOSTE) > 0) {
            if (default_melee(victim) == disc)
            combat_damage(victim, ch, dam / 5, default_melee(victim));
            else
            combat_damage(victim, ch, dam / 10, default_melee(victim));
          }
        }
        else if (disc == DIS_THROWN)
        victim->debuff += 5;

        if (disc == DIS_KNIFE) {
          if (has_caff(ch, CAFF_ATHUNDER)) {
            if (ch->pcdata->divine_focus == CAFF_ATHUNDER)
            victim->debuff += 60;
            else
            victim->debuff += 30;
          }
        }
      }
    }
    else if (disc == DIS_CARBINES || disc == DIS_SHOTGUNS || disc == DIS_RIFLES || disc == DIS_BOWS || disc == DIS_SPEARGUN) {
      if (get_lranged(ch) != NULL && get_lranged(ch)->buff > 0 && !IS_SET(get_lranged(ch)->extra_flags, ITEM_AMMOCHANGED)) {
        int atype = get_lranged(ch)->value[2];
        if (atype == AMMO_AUGMENTED)
        dam = dam * 12 / 10;
        else
        dam = dam * 8 / 10;

        if (dam >= 8 && lower)
        weaken_character(victim, atype, disc);

        if (lower) {
          if (!is_sparring(ch) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
          get_lranged(ch)->buff--;
        }
      }
      if (dam >= 8 && lower) {
        if (disc == DIS_CARBINES)
        victim->debuff += 15;
        else if (disc == DIS_SHOTGUNS)
        victim->debuff += 25;
        else if (disc == DIS_RIFLES)
        victim->debuff += 10;
        else if (disc == DIS_BOWS)
        victim->debuff += 10;
        else if (disc == DIS_SPEARGUN)
        victim->debuff += 10;
        if (disc != DIS_SPEARGUN && disc != DIS_BOWS) {
          if (IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR) && !invisioncone_character(victim, ch))
          victim->debuff += 20;
        }
      }
    }
    else if (disc == DIS_PISTOLS) {
      if (get_sranged(ch) != NULL && get_sranged(ch)->buff > 0 && !IS_SET(get_sranged(ch)->extra_flags, ITEM_AMMOCHANGED)) {
        int atype = get_sranged(ch)->value[2];
        if (atype == AMMO_AUGMENTED)
        dam = dam * 12 / 10;
        else
        dam = dam * 8 / 10;

        if (dam >= 8 && lower)
        weaken_character(victim, atype, disc);

        if (lower) {
          if (!is_sparring(ch) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
          get_sranged(ch)->buff--;
        }
      }
      if (dam >= 8 && lower) {
        if (disc == DIS_PISTOLS)
        victim->debuff += 10;
        if (IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR) && !invisioncone_character(victim, ch))
        victim->debuff += 20;
      }
    }
    else if (disc == DIS_PRISMATIC) {
      if (dam >= 8 && lower)
      victim->debuff += 30;
    }
    else if (disc == DIS_DARK) {
      if (dam >= 8 && lower)
      victim->debuff += 10;
    }
    else if (disc == DIS_LIGHTNING) {
      if (dam >= 8 && lower)
      victim->debuff += 30;
    }
    else if (disc == DIS_FIRE) {
      if (dam >= 8 && lower)
      victim->debuff += 20;
    }
    else if (disc == DIS_ICE) {
      if (dam >= 8 && lower)
      victim->debuff += 20;
    }
    else {
      if (dam >= 8 && lower)
      victim->debuff += 10;
    }

    return dam;
  }

  bool no_incindiary(CHAR_DATA *ch, int disc, bool lower) {
    if (disc == DIS_PISTOLS) {
      if (get_sranged(ch) != NULL && get_sranged(ch)->buff > 0 && !IS_SET(get_sranged(ch)->extra_flags, ITEM_AMMOCHANGED)) {
        if (get_sranged(ch)->value[2] == AMMO_INCENDIARY) {
          if (lower) {
            if (!is_sparring(ch) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
            get_sranged(ch)->buff--;
          }
          return FALSE;
        }
      }
    }
    else if (disc == DIS_CARBINES || disc == DIS_SHOTGUNS || disc == DIS_RIFLES || disc == DIS_BOWS) {
      if (get_lranged(ch) != NULL && get_lranged(ch)->buff > 0 && !IS_SET(get_lranged(ch)->extra_flags, ITEM_AMMOCHANGED)) {
        if (get_lranged(ch)->value[2] == AMMO_INCENDIARY) {
          if (lower) {
            if (!is_sparring(ch) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
            get_lranged(ch)->buff--;
          }
          return FALSE;
        }
      }
    }
    return TRUE;
  }

  bool ranged_disc(int disc) {
    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].vnum == disc && discipline_table[i].range > 1)
      return TRUE;
    }
    return FALSE;
  }

  int society_pop(void) {
    int count = 0;
    CHAR_DATA *to;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;

        if (is_gm(to))
        continue;
        if (to->faction != 0) {
          count += get_tier(to);
          if (get_tier(to) > 1)
          count++;
          if (IS_FLAG(to->comm, COMM_AFK))
          count--;
        }
      }
    }
    return count;
  }

  int clan_combat_mod(CHAR_DATA *ch, FACTION_TYPE *fac)
  {
    if(fac == NULL)
    return 0;
    if (fac->axes[AXES_COMBAT] == AXES_FARLEFT)
    return 5;
    else if (fac->axes[AXES_COMBAT] == AXES_MIDLEFT)
    return 3;
    else if (fac->axes[AXES_COMBAT] == AXES_NEARLEFT)
    return 1;
    else if (fac->axes[AXES_COMBAT] == AXES_FARRIGHT)
    return -5;
    else if (fac->axes[AXES_COMBAT] == AXES_MIDRIGHT)
    return -3;
    else if (fac->axes[AXES_COMBAT] == AXES_NEARRIGHT)
    return -1;
    return 0;
  }


  int soc_combat_mod(CHAR_DATA *ch)
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
      vcult = clan_combat_mod(ch, cult);
    }
    if(sect != NULL)
    {
      count++;
      vsect = clan_combat_mod(ch, sect);
    }
    if(core != NULL)
    {
      count++;
      vfac = clan_combat_mod(ch, core);
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


  int dam_caff_mod(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int disc, bool lower) {

    if (ch == NULL || victim == NULL)
    return dam;

    if (disc == DIS_CUSTOM)
    return dam_caff_mod(ch, victim, dam, custom_vnum(ch), lower);

    if (disc == DIS_BRUTE && !IS_NPC(victim) && !IS_NPC(ch))
    apply_caff(victim, CAFF_BRUTE, 2);

    if (IS_NPC(ch) && forest_monster(ch) && in_world(ch) == WORLD_EARTH)
    dam = dam * 4 / 5;

    if (ch->race == RACE_SEAMONSTER && !in_water(ch))
    dam /= 2;

    if (!IS_NPC(ch) && higher_power(ch) && ch->pcdata->in_domain != 0)
    dam = dam * 3 / 2;
    if (!IS_NPC(victim) && higher_power(victim) && victim->pcdata->in_domain != 0)
    dam = dam * 2 / 3;

    if (guestmonster(ch))
    dam = dam * UMAX(4, society_pop()) / 40;

    if (get_skill(victim, SKILL_HYPERREFLEXES) > 0) {
      int limit = UMAX(0, 20 - victim->debuff);
      dam = UMAX(0, dam - limit);
    }
    if (has_caff(ch, CAFF_BRUTE)) {
      dam = dam * 2 / 3;
      if (lower)
      lower_caff(ch, CAFF_BRUTE);
    }
    if (has_caff(ch, CAFF_WEAKEN)) {
      dam = dam / 2;
      if (lower)
      lower_caff(ch, CAFF_WEAKEN);
    }
    if (has_caff(ch, CAFF_SEMIWEAKEN)) {
      dam = dam * 2 / 3;
      if (lower)
      lower_caff(ch, CAFF_SEMIWEAKEN);
    }

    if (has_caff(ch, CAFF_DISARMED)) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_KNIFE || disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_CARBINES || disc == DIS_BLUNT || disc == DIS_SPEAR || disc == DIS_SPEARGUN) {
        dam = 0;
        if (lower)
        lower_caff(ch, CAFF_DISARMED);
      }
    }

    dam = process_augments(ch, victim, FALSE, disc, dam);
    if (get_armor(victim) != NULL && get_armor(victim)->buff != 0) {
      dam = dam * 8 / 10;
    }

    if (has_caff(ch, CAFF_MELEED)) {
      if (ranged_disc(disc))
      dam = dam * 6 / 10;
    }

    if (has_caff(ch, CAFF_REPEL)) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_KNIFE || disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_THROWN || disc == DIS_CARBINES) {
        dam = 0;
        if (lower)
        lower_caff(ch, CAFF_REPEL);
      }
    }

    if (IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR))
    dam = dam * 90 / 100;

    if (IS_FLAG(ch->fightflag, FIGHT_BIPOD)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES)
      dam = dam * 12 / 10;
    }
    if (has_caff(victim, CAFF_LASERED)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES)
      dam = dam * 15 / 10;
    }
    if (has_caff(victim, CAFF_AURA)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
      dam = dam * 120 / 100;
    }
    if (has_caff(ch, CAFF_DISCIPLINE)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN) {
        dam = dam * 14 / 10;
        if (lower)
        lower_caff(ch, CAFF_DISCIPLINE);
      }
    }
    if (has_caff(ch, CAFF_HEARTSLOW) && lower) {
      remove_caff(ch, CAFF_HEARTSLOW);
      nomove(ch);
    }
    if (has_caff(victim, CAFF_DECLOT)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_KNIFE || disc == DIS_LONGBLADE || disc == DIS_CLAW || disc == DIS_SPEAR || disc == DIS_SPEARGUN)
      dam = dam * 115 / 100;
    }
    if (has_caff(victim, CAFF_CONSTRICT)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_KNIFE || disc == DIS_LONGBLADE || disc == DIS_CLAW || disc == DIS_SPEAR || disc == DIS_SPEARGUN)
      dam = dam * 8 / 10;
    }

    if ((disc == DIS_BOWS || disc == DIS_THROWN) && has_caff(ch, CAFF_GUST)) {
      dam = 0;
      if (lower)
      lower_caff(ch, CAFF_GUST);
    }
    if (has_caff(ch, CAFF_JAM)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES)
      dam = 0;
    }
    if (has_caff(ch, CAFF_ATTRACT)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES)
      dam = dam * 12 / 10;
    }
    if (has_caff(ch, CAFF_ADRENALINE)) {
      if (disc == DIS_GRAPPLE || disc == DIS_STRIKING || disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_BRUTE || disc == DIS_CLAW || disc == DIS_SPEAR) {
        dam = dam * 120 / 100;
        if (lower)
        lower_caff(ch, CAFF_ADRENALINE);
      }
    }
    if (has_caff(ch, CAFF_CONSTRICT)) {
      if (disc == DIS_GRAPPLE || disc == DIS_STRIKING || disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_BRUTE || disc == DIS_CLAW || disc == DIS_SPEAR) {
        dam = dam * 8 / 10;
      }
    }
    if (has_caff(ch, CAFF_CHILL)) {
      if (temperature(ch->in_room) < 60)
      dam = dam / 2;
      else
      dam = dam * 3 / 4;
    }
    if (has_caff(ch, CAFF_SWEAT)) {
      if (temperature(ch->in_room) > 85)
      dam = dam / 2;
      else
      dam = dam * 3 / 4;
    }
    if (wearing_armor(victim) && has_caff(victim, CAFF_ICEARMOR)) {
      dam = dam * 7 / 10;
    }
    if (wearing_armor(victim) && has_caff(victim, CAFF_BURNARMOR)) {
      dam = dam * 12 / 10;
    }

    if (has_caff(victim, CAFF_FLUIDSHIFT)) {
      dam = dam * 6 / 10;
    }

    if (get_skill(victim, SKILL_BLADEDEFENSE) > 0) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR) {
        if (default_melee(victim) == disc)
        dam = dam * 8 / 10;
        else
        dam = dam * 9 / 10;
      }
    }
    if (has_gasmask(ch) && get_skill(ch, SKILL_RIOT) < 2) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
      dam = dam * 8 / 10;
    }
    if (has_caff(victim, CAFF_GLOWING)) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
      dam = dam * 8 / 10;
      else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
      dam = dam * 12 / 10;
    }
    if (has_caff(victim, CAFF_ASUN)) {
      if (victim->pcdata->divine_focus == CAFF_ASUN) {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 6 / 10;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 10 / 6;
      }
      else {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 8 / 10;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 10 / 8;
      }
    }
    if (has_caff(victim, CAFF_AMOON)) {
      if (victim->pcdata->divine_focus == CAFF_AMOON) {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 10 / 6;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 6 / 10;
      }
      else {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 10 / 8;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 8 / 10;
      }
    }
    if (has_caff(ch, CAFF_AWARRIOR)) {
      if (ch->pcdata->divine_focus == CAFF_AWARRIOR) {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 15 / 10;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 10 / 15;
      }
      else {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 125 / 100;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 100 / 125;
      }
    }
    if (has_caff(ch, CAFF_AHUNTER)) {
      if (ch->pcdata->divine_focus == CAFF_AHUNTER) {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 10 / 15;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 15 / 10;
      }
      else {
        if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
        dam = dam * 100 / 125;
        else if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
        dam = dam * 125 / 100;
      }
    }
    if (has_caff(ch, CAFF_AUNDERWORLD)) {
      int mult = UMAX(1, victim->hit * 100 / max_hp(victim));
      if (ch->pcdata->divine_focus == CAFF_AUNDERWORLD) {
        int val;
        if (is_undead(victim))
        val = 100 - mult;
        else
        val = 50 - mult;
        val *= 2;
        dam = dam * (100 + val) / 100;
      }
      else {
        int val;
        if (is_undead(victim))
        val = 100 - mult;
        else
        val = 50 - mult;
        dam = dam * (100 + val) / 100;
      }
    }
    if (has_caff(ch, CAFF_AHEALER)) {
      int mult = UMAX(1, victim->hit * 100 / max_hp(victim));
      if (ch->pcdata->divine_focus == CAFF_AHEALER) {
        int val = mult - 50;
        val *= 2;
        dam = dam * (100 + val) / 100;
      }
      else {
        int val = mult - 50;
        dam = dam * (100 + val) / 100;
      }
    }

    if (has_caff(victim, CAFF_PROWL)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
      dam = dam * 7 / 10;
    }

    if (get_skill(ch, SKILL_VICIOUSSTRIKES) > 0 && is_animal(ch)) {
      if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN)
      int mult = 7;
      else {
        int mult = UMAX(1, victim->hit * 100 / max_hp(victim));
        int val = 50 - mult;
        val /= 2;
        if (val > 0)
        dam = dam * (100 + val) / 100;
      }
    }
    dam = dam * (100 + soc_combat_mod(ch)) / 100;

    if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES) {
      dam = dam * (100 + soc_wealth_mod(ch, FALSE)) / 100;
    }
    if (disc == DIS_BLUNT || disc == DIS_LONGBLADE || disc == DIS_SPEAR || disc == DIS_KNIFE) {
      dam = dam * (100 + soc_wealth_mod(ch, TRUE)) / 100;
    }
    if (disc == DIS_CLAW) {
      dam = dam * (100 + soc_wealth_mod(ch, TRUE)) / 100;
    }

    if (disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_BOWS || disc == DIS_THROWN || disc == DIS_SPEARGUN) {
      CHAR_DATA *cobj;

      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        cobj = *it;

        if (cobj == NULL || is_gm(cobj))
        continue;
        if (!same_fight(ch, cobj))
        continue;

        if (!IS_FLAG(cobj->act, ACT_COMBATOBJ) || !IS_NPC(cobj))
        continue;

        if (cobj->pIndexData->vnum == COBJ_THICKAIR && combat_distance(ch, cobj, FALSE) <= 5) {
          dam /= 2;
        }
        else if (cobj->pIndexData->vnum == COBJ_THICKAIR && combat_distance(victim, cobj, FALSE) <= 5) {
          dam /= 2;
        }
      }
    }

    if (disc != ch->lastdisc && has_caff(ch, CAFF_STICK)) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_THROWN || disc == DIS_PISTOLS) {
        dam = 0;
        if (lower)
        lower_caff(ch, CAFF_STICK);
      }
    }
    if (has_caff(ch, CAFF_BURNWEAPON)) {
      if (disc == DIS_BLUNT || disc == DIS_BOWS || disc == DIS_SPEAR) {
        dam = 0;
        if (lower)
        lower_caff(ch, CAFF_BURNWEAPON);
      }
    }
    if (has_caff(ch, CAFF_HEATMETAL)) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_THROWN || disc == DIS_SPEAR) {
        dam = dam * 13 / 10;
        if (lower)
        lower_caff(ch, CAFF_HEATMETAL);
      }
      else if (disc == DIS_BOWS) {
        dam = dam * 12 / 10;
        if (lower)
        lower_caff(ch, CAFF_HEATMETAL);
      }
    }
    if (has_caff(ch, CAFF_FIREWEAPON)) {
      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR) {
        dam = dam * 13 / 10;
        if (lower)
        lower_caff(ch, CAFF_FIREWEAPON);
      }
    }
    if (has_caff(ch, CAFF_GRAVITY)) {
      if (disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_SPEAR)
      dam = dam * 8 / 10;
    }

    if (has_caff(ch, CAFF_TEAR))
    dam = dam * 8 / 10;

    if (pvp_target(ch, victim) && has_caff(victim, CAFF_ASMITH)) {
      if (victim->pcdata->divine_focus == CAFF_ASMITH) {
        if (dam + victim->damage_absorb > 40) {
          int truedam = UMAX(0, 40 - victim->damage_absorb);
          if (!lower)
          victim->to_debuff = UMAX(victim->to_debuff, dam - truedam);
          dam = truedam;
        }
      }
      else {
        if (dam + victim->damage_absorb > 80) {
          int truedam = UMAX(0, 80 - victim->damage_absorb);
          if (!lower)
          victim->to_debuff = UMAX(victim->to_debuff, dam - truedam);
          dam = truedam;
        }
      }
    }

    if (guestmonster(victim) || higher_power(victim)) {
      if (dam + victim->damage_absorb > monster_round_cap(ch, victim)) {
        int truedam =
        UMAX(0, monster_round_cap(ch, victim) - victim->damage_absorb);
        if (!lower)
        victim->to_debuff = UMAX(victim->to_debuff, dam - truedam);
        dam = truedam;
      }
    }
    else if (cardinal(victim)) {
      if (dam + victim->damage_absorb > monster_round_cap(ch, victim)) {
        int truedam =
        UMAX(0, monster_round_cap(ch, victim) - victim->damage_absorb);
        if (!lower)
        victim->to_debuff = UMAX(victim->to_debuff, dam - truedam);
        dam = truedam;
      }
    }
    else if ((!IS_NPC(ch) || combat_distance(ch, victim, FALSE) > 5) && !IS_NPC(victim) && pvp_target(ch, victim) && get_skill(victim, SKILL_IMMORTALITY) > 0 && dam > victim->hit - 1 && disc != DIS_LONGBLADE && !in_water(ch)) {
      int truedam = UMAX(0, victim->hit - 1);
      if (!str_cmp(ch->name, "Monkey"))
      printf_to_char(ch, "Dam: %d, Hit: %d, disc: %d, Truedam: %d\n\r", dam, victim->hit, disc, truedam);
      if (!lower)
      victim->to_debuff = UMAX(victim->to_debuff, dam - truedam);
      dam = truedam;
    }
    else if (pvp_target(ch, victim) && (is_super(victim) || get_skill(victim, SKILL_IMMORTALITY) > 0) && victim->shape == SHAPE_HUMAN) {
      if (disc != DIS_LONGBLADE && disc != DIS_BRUTE && disc != DIS_HELLFIRE && disc != DIS_BLUNT && disc != DIS_SHOTGUNS && no_incindiary(ch, disc, FALSE) && (disc != DIS_CLAW || ch->shape != SHAPE_WOLF || !is_werewolf(ch))) {
        if (dam + victim->damage_absorb > round_cap(ch, victim)) {
          int truedam = UMAX(0, round_cap(ch, victim) - victim->damage_absorb);
          if (!lower)
          victim->to_debuff = UMAX(victim->to_debuff, dam - truedam);
          dam = truedam;
        }
      }
      if (disc == DIS_CLAW && ch->shape == SHAPE_WOLF && is_werewolf(ch) && is_undead(victim)) {
        if (victim->debuff > 0) {
          int bonus = UMIN(dam, victim->debuff);
          dam += bonus;
        }
      }
    }
    if (ch->debuff > 0) {
      int maxreduc = 80;
      if (guestmonster(ch) || higher_power(ch)) {
        if (ch->pcdata->height_feet >= 10)
        maxreduc = 70;
        else
        maxreduc = 80;
      }
      int reduc = ch->debuff / 3;
      reduc = UMIN(reduc, maxreduc);
      dam = dam * (100 - reduc) / 100;
    }

    ch->lastdisc = disc;

    return dam;
  }

  int apply_stagger(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int disc) {
    if (victim == NULL)
    return 0;
    int init = victim->debuff;
    int throwaway = process_augments(ch, victim, TRUE, disc, dam);
    throwaway++;
    victim->to_debuff = UMIN(victim->to_debuff, 100);

    if (get_armor(victim) != NULL && get_armor(victim)->buff > 0 && dam >= 10) {
      if (!is_sparring(victim) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
      get_armor(victim)->buff--;
    }
    if (get_armor(victim) != NULL && get_armor(victim)->buff < 0 && dam >= 10) {
      if (!is_sparring(victim) && (IS_NPC(ch) || ch->pcdata->spectre == 0 || number_percent() % 2 == 0))
      get_armor(victim)->buff++;
    }

    if (has_caff(victim, CAFF_ASMITH)) {
      victim->debuff += victim->to_debuff;
      victim->damage_absorb += dam;
      victim->to_debuff = 0;
    }
    else if (get_skill(victim, SKILL_IMMORTALITY) > 0 && disc != DIS_LONGBLADE) {
      victim->debuff += victim->to_debuff;
      victim->damage_absorb += dam;
      victim->to_debuff = 0;
    }
    else if (guestmonster(victim) || higher_power(victim)) {
      victim->debuff += victim->to_debuff;
      victim->damage_absorb += dam;
      victim->to_debuff = 0;
    }
    else if (disc != DIS_LONGBLADE && disc != DIS_BRUTE && disc != DIS_HELLFIRE && disc != DIS_BLUNT && disc != DIS_SHOTGUNS && no_incindiary(ch, disc, TRUE) && (disc != DIS_CLAW || ch->shape != SHAPE_WOLF || !is_werewolf(ch))) {
      victim->debuff += victim->to_debuff;
      victim->damage_absorb += dam;
      victim->to_debuff = 0;
    }
    else if (disc == DIS_CLAW && ch->shape == SHAPE_WOLF && is_werewolf(ch) && is_undead(victim)) {
      if (victim->debuff > 0) {
        int bonus = UMIN(dam, victim->debuff);
        victim->debuff -= bonus;
      }
    }
    return UMIN(100, victim->debuff - init);
  }

  int move_caff_mod(CHAR_DATA *ch, int dist) {
    if (has_caff(ch, CAFF_SLOW)) {
      dist /= 2;
      lower_caff(ch, CAFF_SLOW);
    }
    if (has_caff(ch, CAFF_SWEAT)) {
      if (temperature(ch->in_room) > 85)
      dist /= 2;
      else
      dist = dist * 3 / 4;
    }
    if (has_caff(ch, CAFF_CHILL)) {
      if (temperature(ch->in_room) < 60)
      dist /= 2;
      else
      dist = dist * 3 / 4;
    }

    if (has_caff(ch, CAFF_SPRINTING)) {
      dist = dist * 15 / 10;
      lower_caff(ch, CAFF_SPRINTING);
    }
    if (has_caff(ch, CAFF_AMESSENGER)) {
      if (ch->pcdata->divine_focus == CAFF_AMESSENGER)
      dist = dist * 2;
      else
      dist = dist * 15 / 10;
      lower_caff(ch, CAFF_AMESSENGER);
    }
    if (has_caff(ch, CAFF_HEARTSLOW)) {
      remove_caff(ch, CAFF_HEARTSLOW);
      noattack(ch);
    }
    if (has_caff(ch, CAFF_GRAVITY)) {
      dist /= 2;
    }
    if (has_caff(ch, CAFF_PROWL)) {
      dist = dist * 2 / 3;
    }
    if (ch->in_room->sector_type == SECT_AIR)
    dist = UMIN(dist, 100);

    return dist;
  }

  void nomove(CHAR_DATA *ch) {
    if (guestmonster(ch))
    return;
    if (ch->fight_fast == TRUE && ch->move_timer <= FIGHT_WAIT * fight_speed(ch)) {
      ch->move_timer += FIGHT_WAIT * fight_speed(ch);
    }
    else {
      if (!IS_FLAG(ch->fightflag, FIGHT_NOMOVE))
      SET_FLAG(ch->fightflag, FIGHT_NOMOVE);
    }
  }
  void noattack(CHAR_DATA *ch) {
    if (guestmonster(ch))
    return;
    if (ch->fight_fast == TRUE && ch->attack_timer <= FIGHT_WAIT * fight_speed(ch)) {
      ch->attack_timer += FIGHT_WAIT * fight_speed(ch);
    }
    else {
      if (!IS_FLAG(ch->fightflag, FIGHT_NOATTACK))
      SET_FLAG(ch->fightflag, FIGHT_NOATTACK);
    }
  }

  void reset_caff(CHAR_DATA *ch, int aff) {
    int i;
    for (i = 0; i < 30; i++) {
      if (ch->caff[i] == aff && ch->caff_duration[i] > 0) {
        ch->caff_duration[i] = FIGHT_WAIT * fight_speed(ch) * 3 / 2;
      }
    }
  }

  bool shroud_monster(CHAR_DATA *ch) {
    if (ch == NULL)
    return FALSE;

    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum > 45)
    return FALSE;

    if (ch->pIndexData->vnum < 10)
    return FALSE;

    return TRUE;
  }

  _DOFUN(do_scan) {
    if (has_caff(ch, CAFF_BEWILDER)) {
      send_to_char("You can't seem to judge anything right now.\n\r", ch);
      return;
    }
    if (ch->in_room == NULL || ch->in_room->size == 0)
    return;
    scan_fight(ch, FALSE);
    WAIT_STATE(ch, PULSE_PER_SECOND * 3);
  }
  void scan_fight(CHAR_DATA *ch, bool mapcount) {
    int mindist[100] = {1000};
    CHAR_DATA *minchar[100] = {NULL};
    int t;

    for (t = 0; t < 100; t++) {
      mindist[t] = 1000;
      minchar[t] = NULL;
    }

    displaypois(ch);

    int last = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *to = *it;

      if (to == NULL || is_gm(to))
      continue;
      if (!can_see_char_distance(ch, to, DISTANCE_MEDIUM))
      continue;
      if (!can_map_see(ch, to))
      continue;

      mindist[last] = combat_distance(ch, to, FALSE);
      minchar[last] = to;
      last++;
    }
    for (int i = 0; i <= last; ++i) {
      for (int j = i + 1; j <= last; ++j) {
        if (mindist[i] > mindist[j]) {
          int dist = mindist[i];
          CHAR_DATA *point = minchar[i];
          mindist[i] = mindist[j];
          minchar[i] = minchar[j];
          mindist[j] = dist;
          minchar[j] = point;
        }
      }
    }
    for (t = 0; t <= last; t++) {
      if (mindist[t] < 1000 && minchar[t] != NULL) {
        if (mapcount == TRUE && minchar[t]->mapcount > 1) {
          if (relative_z(ch, minchar[t]->in_room) == 0)
          printf_to_char(ch, "(%s) %s%s and %d others (X:%d Y:%d D:%d)\n\r", mapname(minchar[t], ch), PERS_3(minchar[t], ch), battleflags(minchar[t], ch), minchar[t]->mapcount - 1, relative_x(ch, minchar[t]->in_room, minchar[t]->x), relative_y(ch, minchar[t]->in_room, minchar[t]->y), combat_distance(ch, minchar[t], FALSE));
          else
          printf_to_char(ch, "(%s) %s%s and %d others (X:%d Y:%d D:%d(%d))\n\r", mapname(minchar[t], ch), PERS_3(minchar[t], ch), battleflags(minchar[t], ch), minchar[t]->mapcount - 1, relative_x(ch, minchar[t]->in_room, minchar[t]->x), relative_y(ch, minchar[t]->in_room, minchar[t]->y), combat_distance(ch, minchar[t], FALSE), relative_z(ch, minchar[t]->in_room));
        }
        else {
          if (relative_z(ch, minchar[t]->in_room) == 0)
          printf_to_char(ch, "(%s) %s%s (X:%d Y:%d D:%d)\n\r", mapname(minchar[t], ch), PERS_3(minchar[t], ch), battleflags(minchar[t], ch), relative_x(ch, minchar[t]->in_room, minchar[t]->x), relative_y(ch, minchar[t]->in_room, minchar[t]->y), combat_distance(ch, minchar[t], FALSE));
          else
          printf_to_char(ch, "(%s) %s%s (X:%d Y:%d D:%d(%d))\n\r", mapname(minchar[t], ch), PERS_3(minchar[t], ch), battleflags(minchar[t], ch), relative_x(ch, minchar[t]->in_room, minchar[t]->x), relative_y(ch, minchar[t]->in_room, minchar[t]->y), combat_distance(ch, minchar[t], FALSE), relative_z(ch, minchar[t]->in_room));
        }
      }
    }
  }

  int dam_type_mod(int offense, int defense) {
    int disc = offense;

    if (defense == DIS_BARMOR) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 80;
      if (offense == DIS_SHOTGUNS)
      return 80;
      if (offense == DIS_SPEAR)
      return 100;
      if (offense == DIS_LONGBLADE)
      return 100;
      if (offense == DIS_BLUNT)
      return 100;
      if (offense == DIS_KNIFE)
      return 100;
      if (offense == DIS_BOWS)
      return 90;
      if (offense == DIS_THROWN)
      return 90;
      if (offense == DIS_SPEARGUN)
      return 90;
      if (offense == DIS_CLAW)
      return 100;
      if (offense == DIS_STRIKING)
      return 90;
      if (offense == DIS_GRAPPLE)
      return 100;
    }
    if (defense == DIS_MARMOR) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 100;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 80;
      if (offense == DIS_LONGBLADE)
      return 80;
      if (offense == DIS_BLUNT)
      return 90;
      if (offense == DIS_KNIFE)
      return 80;
      if (offense == DIS_BOWS)
      return 90;
      if (offense == DIS_THROWN)
      return 90;
      if (offense == DIS_SPEARGUN)
      return 90;
      if (offense == DIS_CLAW)
      return 80;
      if (offense == DIS_STRIKING)
      return 90;
      if (offense == DIS_GRAPPLE)
      return 100;
    }
    if (defense == DIS_BSHIELD) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 90;
      if (offense == DIS_SHOTGUNS)
      return 80;
      if (offense == DIS_SPEAR)
      return 90;
      if (offense == DIS_LONGBLADE)
      return 90;
      if (offense == DIS_BLUNT)
      return 90;
      if (offense == DIS_KNIFE)
      return 90;
      if (offense == DIS_BOWS)
      return 90;
      if (offense == DIS_THROWN)
      return 90;
      if (offense == DIS_SPEARGUN)
      return 90;
      if (offense == DIS_CLAW)
      return 90;
      if (offense == DIS_STRIKING)
      return 100;
      if (offense == DIS_GRAPPLE)
      return 90;
      if (offense == DIS_FIRE || offense == DIS_ICE || offense == DIS_LIGHTNING || offense == DIS_DARK || offense == DIS_PRISMATIC)
      return 80;
    }
    if (defense == DIS_MSHIELD) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 100;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 80;
      if (offense == DIS_LONGBLADE)
      return 80;
      if (offense == DIS_BLUNT)
      return 80;
      if (offense == DIS_KNIFE)
      return 90;
      if (offense == DIS_BOWS)
      return 90;
      if (offense == DIS_THROWN)
      return 90;
      if (offense == DIS_SPEARGUN)
      return 90;
      if (offense == DIS_CLAW)
      return 80;
      if (offense == DIS_STRIKING)
      return 100;
      if (offense == DIS_GRAPPLE)
      return 90;
      if (offense == DIS_FIRE || offense == DIS_ICE || offense == DIS_LIGHTNING || offense == DIS_DARK || offense == DIS_PRISMATIC)
      return 80;
    }
    if (defense == DIS_NARMOR) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 90;
      if (offense == DIS_SHOTGUNS)
      return 80;
      if (offense == DIS_SPEAR)
      return 100;
      if (offense == DIS_LONGBLADE)
      return 80;
      if (offense == DIS_BLUNT)
      return 90;
      if (offense == DIS_KNIFE)
      return 80;
      if (offense == DIS_BOWS)
      return 80;
      if (offense == DIS_THROWN)
      return 80;
      if (offense == DIS_SPEARGUN)
      return 100;
      if (offense == DIS_CLAW)
      return 80;
      if (offense == DIS_STRIKING)
      return 80;
      if (offense == DIS_GRAPPLE)
      return 80;
    }
    if (defense == DIS_TOUGHNESS) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 90;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 90;
      if (offense == DIS_LONGBLADE)
      return 100;
      if (offense == DIS_BLUNT)
      return 90;
      if (offense == DIS_KNIFE)
      return 80;
      if (offense == DIS_BOWS)
      return 90;
      if (offense == DIS_THROWN)
      return 80;
      if (offense == DIS_SPEARGUN)
      return 80;
      if (offense == DIS_CLAW)
      return 90;
      if (offense == DIS_STRIKING)
      return 80;
      if (offense == DIS_GRAPPLE)
      return 80;
    }
    if (defense == DIS_BONES) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 100;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 90;
      if (offense == DIS_LONGBLADE)
      return 80;
      if (offense == DIS_BLUNT)
      return 80;
      if (offense == DIS_KNIFE)
      return 100;
      if (offense == DIS_BOWS)
      return 100;
      if (offense == DIS_THROWN)
      return 100;
      if (offense == DIS_SPEARGUN)
      return 100;
      if (offense == DIS_CLAW)
      return 90;
      if (offense == DIS_STRIKING)
      return 100;
      if (offense == DIS_GRAPPLE)
      return 80;
    }
    if (defense == DIS_FORCES) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 90;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 90;
      if (offense == DIS_LONGBLADE)
      return 90;
      if (offense == DIS_BLUNT)
      return 100;
      if (offense == DIS_KNIFE)
      return 80;
      if (offense == DIS_BOWS)
      return 80;
      if (offense == DIS_THROWN)
      return 80;
      if (offense == DIS_SPEARGUN)
      return 80;
      if (offense == DIS_CLAW)
      return 100;
      if (offense == DIS_STRIKING)
      return 80;
      if (offense == DIS_GRAPPLE)
      return 100;
    }
    if (defense == DIS_UNDEAD) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 80;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 80;
      if (offense == DIS_LONGBLADE)
      return 100;
      if (offense == DIS_BLUNT)
      return 100;
      if (offense == DIS_KNIFE)
      return 80;
      if (offense == DIS_BOWS)
      return 80;
      if (offense == DIS_THROWN)
      return 80;
      if (offense == DIS_SPEARGUN)
      return 80;
      if (offense == DIS_CLAW)
      return 80;
      if (offense == DIS_STRIKING)
      return 80;
      if (offense == DIS_GRAPPLE)
      return 80;
    }
    if (defense == DIS_FATE) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 80;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 90;
      if (offense == DIS_LONGBLADE)
      return 100;
      if (offense == DIS_BLUNT)
      return 90;
      if (offense == DIS_KNIFE)
      return 100;
      if (offense == DIS_BOWS)
      return 80;
      if (offense == DIS_THROWN)
      return 80;
      if (offense == DIS_SPEARGUN)
      return 80;
      if (offense == DIS_CLAW)
      return 100;
      if (offense == DIS_STRIKING)
      return 80;
      if (offense == DIS_GRAPPLE)
      return 100;
    }
    if (defense == DIS_PUSH) {
      if (offense == DIS_RIFLES || disc == DIS_CARBINES || disc == DIS_PISTOLS)
      return 90;
      if (offense == DIS_SHOTGUNS)
      return 100;
      if (offense == DIS_SPEAR)
      return 90;
      if (offense == DIS_LONGBLADE)
      return 100;
      if (offense == DIS_BLUNT)
      return 100;
      if (offense == DIS_KNIFE)
      return 80;
      if (offense == DIS_BOWS)
      return 80;
      if (offense == DIS_THROWN)
      return 80;
      if (offense == DIS_SPEARGUN)
      return 80;
      if (offense == DIS_CLAW)
      return 100;
      if (offense == DIS_STRIKING)
      return 90;
      if (offense == DIS_GRAPPLE)
      return 100;
    }
    return 100;
  }

  int dam_type_modifier(int offense, CHAR_DATA *victim) {
    int totaldef = 0;
    int bonus = 0;

    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;

      int val = get_disc(victim, discipline_table[i].vnum, FALSE);
      if (val > 0) {
        totaldef += val;
        bonus += val * dam_type_mod(offense, discipline_table[i].vnum);
      }
    }
    if (bonus == 0 || totaldef == 0)
    return 100;
    bonus /= totaldef;
    return bonus;
  }

  int dam_cap(int damage, int cap) {
    if (damage <= cap)
    return damage;

    damage -= cap;

    damage = (int)(cbrt(damage * damage));

    damage += cap;

    return damage;
  }

  bool is_attacking(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (ch->fight_fast == TRUE) {
      if (ch->attack_timer > 0)
      return TRUE;
    }
    else {
      if (ch->attackdam > 0)
      return TRUE;

      if (IS_FLAG(ch->fightflag, FIGHT_NOATTACK))
      return TRUE;

      for (int i = 0; i < 10; i++) {
        if (ch->delays[i][1] > 0 && ch->delays[i][2] > 0)
        return TRUE;
      }
    }
    return FALSE;
  }

  int combat_lifeforce(CHAR_DATA *ch, bool used) {
    int val = get_lifeforce(ch, used, NULL);
    val = UMIN(val, 110);
    val = UMAX(val, 75);
    if (!IS_NPC(ch) && ch->fight_speed >= 4) {
      if (ch->pcdata->combat_emoted == TRUE)
      val += 5;
      else
      val -= 5;
    }
    return val;
  }

  int damage_calculate(CHAR_DATA *ch, CHAR_DATA *victim, int point, int shield) {
    int discvnum = discipline_table[point].vnum;
    int dam = 0;
    dam = get_disc(ch, discvnum, TRUE);
    int capone = 70;
    int captwo = 80;
    if (IS_NPC(ch) || IS_NPC(victim) || ch->shape != SHAPE_HUMAN || victim->shape != SHAPE_HUMAN) {
      capone += 10;
      captwo += 10;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->shape == SHAPE_WOLF && is_werewolf(ch)) {
      capone += 50;
      captwo += 50;
    }
    if (victim->last_hit_by == ch && victim->last_hit_damage > 0) {
      capone -= victim->last_hit_damage * 4 / 5;
      captwo -= victim->last_hit_damage * 4 / 5;
      capone = UMAX(0, capone);
      captwo = UMAX(0, captwo);
    }

    capone += UMIN(ch->hits_taken, 5) * 10;
    capone -= UMIN(victim->hits_taken, 5) * 5;
    captwo += UMIN(ch->hits_taken, 5) * 10;
    captwo -= UMIN(victim->hits_taken, 5) * 5;

    dam = get_disc(ch, discipline_table[point].vnum, TRUE);

    if (disc_range(ch, point) > 1) {
      dam += get_skill(ch, SKILL_COMBATTRAINING) * 3;
      if (forest_monster(ch))
      dam -= get_skill(victim, SKILL_EVADING);
      else
      dam -= get_skill(victim, SKILL_EVADING) * 3;
      if (camo_protect(victim) && dam >= 7)
      dam -= 5;
    }
    else {
      dam += get_skill(ch, SKILL_MARTIALART) * 3;
      if (forest_monster(ch))
      dam -= get_skill(victim, SKILL_DODGING);
      else
      dam -= get_skill(victim, SKILL_DODGING) * 3;
    }

    if (victim == ch->commanded)
    dam = dam * ch->command_force / 100;

    /*
dam = ch->delays[j][2]*dam/100;
if(ch->pcdata->customstats[ch->delays[j][0]][0] == SPECIAL_DELAY)
dam = dam;
else if(ch->pcdata->customstats[ch->delays[j][0]][0] ==
SPECIAL_DELAY2) dam = dam*11/10;
*/

    if (IS_NPC(ch) && ch->pIndexData->vnum == ALLY_TEMPLATE) {
      if (safe_strlen(ch->protecting) > 1 && get_char_world_pc(ch->protecting) != NULL) {
        if (is_attacking(get_char_world_pc(ch->protecting)))
        dam = dam * 2 / 3;
      }
    }

    if(!IS_NPC(victim) && !has_shield(ch)) //Hidden armor
    dam = dam * 11/10;

    dam = dam * combat_lifeforce(ch, TRUE) / combat_lifeforce(victim, FALSE);

    if (((pvp_character(ch)) && (pvp_character(victim)))) {
      int comdiff = UMIN(get_tier(ch), combat_focus(ch)) -
      UMIN(get_tier(victim), combat_focus(victim));
      comdiff *= 8;
      comdiff = UMAX(-25, comdiff);
      comdiff = UMIN(25, comdiff);
      if (get_skill(ch, SKILL_GIANTSLAYER) > 0 && comdiff < 0)
      comdiff /= 2;
      if (get_skill(victim, SKILL_GIANTSLAYER) > 0 && comdiff > 0)
      comdiff /= 2;

      dam = dam * (100 + comdiff) / 100;
    }

    if (disc_range(ch, point) > 1) {
      if (is_in_cover(victim)) {
        if (discvnum == DIS_BOWS || discvnum == DIS_SHOTGUNS || discvnum == DIS_RIFLES || discvnum == DIS_CARBINES || discvnum == DIS_SPEARGUN) {
          if (get_lranged(ch) != NULL && get_lranged(ch)->buff > 0 && !IS_SET(get_lranged(ch)->extra_flags, ITEM_AMMOCHANGED) && get_lranged(ch)->value[2] == AMMO_ARMOR)
          dam = dam * 6 / 10;
          else
          dam /= 10;
        }
        else if (discvnum == DIS_PISTOLS) {
          if (get_sranged(ch) != NULL && get_sranged(ch)->buff > 0 && !IS_SET(get_sranged(ch)->extra_flags, ITEM_AMMOCHANGED) && get_sranged(ch)->value[2] == AMMO_ARMOR)
          dam = dam * 6 / 10;
          else
          dam /= 10;
        }
        else if (discvnum == DIS_THROWN) {
          if (get_smelee(ch) != NULL && get_smelee(ch)->buff > 0 && !IS_SET(get_smelee(ch)->extra_flags, ITEM_AMMOCHANGED) && get_smelee(ch)->value[2] == AMMO_ARMOR)
          dam = dam * 6 / 10;
          else
          dam /= 10;
        }
        else
        dam /= 10;

        if (get_skill(victim, SKILL_SNIPERTRAINING) > 0)
        dam /= 2;
      }
      if (!can_see(ch, victim))
      dam /= 2;

      if(ch->in_room != victim->in_room)
      {
        if(light_level(victim->in_room) < light_level(ch->in_room)-10)
        dam = dam * 9/10;
      }

      if (victim->moved > 5) {
        int reduc = UMIN(victim->moved, 40);
        if (invisioncone_running(victim, ch) && !IS_FLAG(ch->fightflag, FIGHT_SUPPRESSOR))
        reduc += 20;
        else if (behindyou_running(victim, ch) && combat_distance(ch, victim, TRUE) <= 30)
        reduc -= 25;

        //		if(IS_NPC(ch) || IS_NPC(victim))
        //		   reduc /= 2;

        dam = dam * (100 - reduc) / 100;
      }
      else if (is_combat_flyer(victim)) {
        if (victim->in_room->sector_type == SECT_AIR)
        dam *= 2;
      }

    }
    else {
      if (!can_see(ch, victim))
      dam = dam * 2 / 3;
    }

    if (!str_cmp(ch->name, "Monkey"))
    printf_to_char(ch, "D2: %d\n\r", dam);

    if (IS_FLAG(victim->fightflag, FIGHT_DEFEND))
    dam = dam * 2 / 3;

    if (discipline_table[point].vnum == DIS_LONGBLADE && is_super(victim))
    dam = dam * 105 / 100;

    int initd = dam;
    dam = dam_cap(dam, capone);
    if (initd > dam) {
      victim->debuff += (initd - dam);
    }

    if (!str_cmp(ch->name, "Monkey"))
    printf_to_char(ch, "D3: %d\n\r", dam);

    if (disc_range(ch, point) > 1) {
      if (combat_distance(ch, victim, TRUE) < 2)
      dam =
      dam *
      damage_mod(combat_distance(ch, victim, TRUE), disc_range(ch, point)) /
      100;

      if (!str_cmp(ch->name, "Monkey"))
      printf_to_char(ch, "D4: %d DMOD: %d\n\r", dam, damage_mod(get_dist(ch->x, ch->y, victim->x, victim->y), disc_range(ch, point)));

      if (discipline_table[point].vnum == DIS_CUSTOM)
      dam = dam * dam_type_modifier(custom_vnum(ch), victim) / 100;
      else
      dam = dam * dam_type_modifier(discipline_table[point].vnum, victim) / 100;

      if (!str_cmp(ch->name, "Monkey"))
      printf_to_char(ch, "D5: %d\n\r", dam);

    }
    else {
      if (discipline_table[point].vnum == DIS_KNIFE && holding_lweapon(victim) && victim->shape == SHAPE_HUMAN && (get_disc(victim, DIS_LONGBLADE, FALSE) > 0 || get_disc(victim, DIS_BLUNT, FALSE) > 0 || get_disc(victim, DIS_SPEAR, FALSE) > 0)) {
        if (get_skill(ch, SKILL_SOLDIER) < 3 || (!holding_lgun(ch) && !has_lgun(ch)))
        dam = dam * 85 / 100;
      }

      if (discipline_table[point].vnum == DIS_CUSTOM)
      dam = dam * dam_type_modifier(custom_vnum(ch), victim) / 100;
      else
      dam = dam * dam_type_modifier(discipline_table[point].vnum, victim) / 100;
    }
    if (combat_distance(ch, victim, TRUE) > 1 && combat_distance(ch, victim, TRUE) > disc_range(ch, point))
    dam = 0;

    if (!IS_NPC(ch)) {
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->augdisc_timer[i] > 0 && ch->pcdata->augdisc_disc[i] == discvnum) {
          dam += (int)(ch->pcdata->augdisc_level[i]);
        }
      }
    }

    if (!str_cmp(ch->name, "Monkey"))
    printf_to_char(ch, "D6: %d\n\r", dam);

    dam = dam_caff_mod(ch, victim, dam, discipline_table[point].vnum, FALSE);

    if (!str_cmp(ch->name, "Monkey"))
    printf_to_char(ch, "D7: %d\n\r", dam);

    initd = dam;
    dam = dam_cap(dam, captwo);
    if (initd > dam) {
      victim->debuff += (initd - dam);
    }
    if (!IS_NPC(victim) && max_hp(victim) >= 100 && dam > max_hp(victim) * 4 / 10) {
      initd = dam;
      dam = UMIN(dam, max_hp(victim) * 4 / 10);
      if (initd > dam) {
        victim->debuff += (initd - dam);
      }
    }

    if (!str_cmp(ch->name, "Monkey"))
    printf_to_char(ch, "D8: %d\n\r", dam);

    if (!IS_NPC(ch) && !IS_NPC(victim)) {
      if (ch->diminished > 0 && ch->diminished < 100) {
        if (ch->diminish_offervalue < ch->diminished && ch->diminish_offervalue > 0)
        dam = dam * ch->diminish_offervalue / 100;
        else
        dam = dam * ch->diminished / 100;
      }
      else if (ch->diminish_offervalue > 0 && ch->diminish_offervalue < 100)
      dam = dam * ch->diminish_offervalue / 100;
      if (ch->diminish_lt > 0 && ch->diminish_lt < 100 && ch->diminish_till > current_time)
      dam = dam * ch->diminish_lt / 100;
    }
    if (is_sparring(ch))
    dam /= 2;

    ch->hits_taken = UMAX(0, ch->hits_taken - 2);
    if (dam >= 10)
    victim->hits_taken++;

    if (is_animal(victim) && get_animal_genus(victim, ANIMAL_ACTIVE) == GENUS_SWARM && get_skill(victim, SKILL_SWARMSHIFTING) > 0) {
      dam = UMIN(dam, victim->hit / swarm_count(victim, ANIMAL_ACTIVE));
    }
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_SWARM && get_skill(ch, SKILL_SWARMSHIFTING) > 0) {
      dam = dam * ch->hit / max_hp(ch);
    }
    if ((!IS_NPC(ch) || combat_distance(ch, victim, FALSE) > 5) && !IS_NPC(victim) && pvp_target(ch, victim) && get_skill(victim, SKILL_IMMORTALITY) > 0 && dam > victim->hit - 1 && discipline_table[point].vnum != DIS_LONGBLADE) {
      dam = UMAX(0, victim->hit - 1);
    }

    if (!IS_NPC(ch) && ch->pcdata->destiny_feature == DEST_FEAT_NOWIN) {
      dam = UMIN(dam, victim->hit / 4);
    }
    if (!IS_NPC(ch) && ch->pcdata->curse == CURSE_WEAKNESS && ch->pcdata->curse_timeout > current_time) {
      dam = UMIN(dam, victim->hit / 4);
    }
    if(!IS_NPC(ch) && ch->preyvictim == TRUE && number_percent() % 3 != 0)
    dam = UMIN(dam, victim->hit / 4);



    return dam;
  }

  char *battleflags(CHAR_DATA *ch, CHAR_DATA *pers) {
    if (ch == NULL || ch->in_room == NULL)
    return "";

    if (!in_fight(ch))
    return "";

    if (ch->in_room != NULL && battleground(ch->in_room) && ch->bagcarrier == 1)
    return "(`YCarrier`x)";

    if (ch->cfighting != NULL && same_fight(ch, ch->cfighting)) {
      char buf[MSL];
      if (get_dist(ch->x, ch->y, ch->cfighting->x, ch->cfighting->y) <= 5)
      sprintf(buf, " fighting %s", PERS(ch->cfighting, pers));
      else
      sprintf(buf, " shooting at %s", PERS(ch->cfighting, pers));
      return str_dup(buf);
    }

    return "";
  }

  _DOFUN(do_diminish) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];

    if (safe_strlen(argument) < 2) {
      int dim = 0;
      if (ch->diminished > 0 && ch->diminished < 100) {
        dim = ch->diminished;
      }
      if (ch->diminish_offervalue > 0 && ch->diminish_offervalue < 100)
      dim = UMIN(dim, ch->diminish_offervalue);

      if (ch->diminish_till > current_time)
      dim = UMIN(dim, ch->diminish_lt);

      if (dim > 0 && dim < 100)
      printf_to_char(ch, "You are currently diminished to %d percent.\n\r", dim);
      if (ch->diminish_offervalue > 0 && ch->diminish_offervalue < 100 && ch->diminish_offer != NULL)
      printf_to_char(ch, "You have a diminish pledge with %s for %d.\n\r", PERS(ch->diminish_offer, ch), ch->diminish_offervalue);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "longterm")) {
      if (ch->diminish_till > current_time) {
        send_to_char("You already have that in effect.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      int number = atoi(arg2);
      if (number <= 0 || number >= 100) {
        send_to_char("Syntax: diminish longterm `W(1-99)`x (number hours) (visible/invisible/combatvisible)\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg3);
      int hours = atoi(arg3);
      if (hours <= 0 || hours >= 500) {
        send_to_char("Syntax: diminish longterm (1-99) `W(number hours)`x (visible/invisible/combatvisible)\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "visible")) {
        ch->diminish_lt = number;
        ch->diminish_till = current_time + (3600 * hours);
        ch->diminish_vis = 1;
        printf_to_char(
        ch, "You diminish yourself to %d percent effectiveness for %d hours.\n\r", number, hours);
        char buf[MSL];
        sprintf(
        buf, "($n diminishes $mself to %d percent effectiveness for %d hours.)", number, hours);
        act(buf, ch, NULL, NULL, TO_ROOM);
        return;
      }
      if (!str_cmp(argument, "invisible")) {
        ch->diminish_lt = number;
        ch->diminish_till = current_time + (3600 * hours);
        ch->diminish_vis = 2;
        printf_to_char(
        ch, "You diminish yourself to %d percent effectiveness for %d hours.\n\r", number, hours);
        return;
      }
      if (!str_cmp(argument, "combatvisible")) {
        ch->diminish_lt = number;
        ch->diminish_till = current_time + (3600 * hours);
        ch->diminish_vis = 3;
        printf_to_char(
        ch, "You diminish yourself to %d percent effectiveness for %d hours.\n\r", number, hours);
        char buf[MSL];
        sprintf(
        buf, "($n diminishes $mself to %d percent effectiveness for %d hours.)", number, hours);
        act(buf, ch, NULL, NULL, TO_ROOM);
        return;
      }
      send_to_char("Syntax: diminish longterm (1-99) (number hours) (visible/invisible/combatvisible)\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "round")) {
      int number = atoi(argument);
      if (number <= 0 || number >= 100) {
        send_to_char("Syntax: diminish round (1-99)\n\r", ch);
        return;
      }
      ch->rounddiminish = number;
      printf_to_char(
      ch, "Your next source of damage will only do %d percent of normal.\n\r", number);
      char buf[MSL];
      sprintf(buf, "($n diminishes $s next attack to only do %d percent of normal.)", number);
      act(buf, ch, NULL, NULL, TO_ROOM);
      return;
    }
    if (!str_cmp(arg1, "offer")) {
      argument = one_argument_nouncap(argument, arg2);

      if ((victim = get_char_fight(ch, arg2)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      if (IS_NPC(victim))
      return;
      int amount = atoi(argument);
      if (amount <= 0 || amount >= 100) {
        send_to_char("Syntax: Diminish offer (person) (1-100)\n\r", ch);
        return;
      }
      ch->diminish_offervalue = amount * -1;
      victim->diminish_offervalue = amount * -1;
      ch->diminish_offer = victim;
      victim->diminish_offer = ch;
      printf_to_char(victim, "%s offers a diminish pact of %d percent, to agree type diminish agree.\n\r", PERS(ch, victim), amount);
      printf_to_char(ch, "You offer %s a diminish pact of %d percent.\n\r", PERS(victim, ch), amount);
      return;
    }
    else if (!str_cmp(arg1, "accept")) {
      if (ch->diminish_offervalue >= 0) {
        send_to_char("You have no pending diminish offers.\n\r", ch);
        return;
      }
      if (ch->diminish_offer == NULL || ch->diminish_offer->in_room == NULL || ch->diminish_offer->in_room != ch->in_room) {
        send_to_char("You have no pending diminish offers.\n\r", ch);
        return;
      }
      ch->diminish_offervalue *= -1;
      ch->diminish_offer->diminish_offervalue = ch->diminish_offervalue;
      printf_to_char(
      ch, "You enter into a diminishment pact of %d percent with %s.\n\r", ch->diminish_offervalue, PERS(ch->diminish_offer, ch));
      printf_to_char(
      ch->diminish_offer, "You enter in a diminishment pact of %d percent with %s.\n\r", ch->diminish_offervalue, PERS(ch, ch->diminish_offer));

      return;
    }
    else if (is_number(arg1)) {
      int amount = atoi(arg1);
      if (amount >= 100 || amount <= 0) {
        if (ch->diminish_offervalue > 0) {
          if (ch->diminish_offer != NULL && ch->diminish_offer->in_room != NULL) {
            ch->diminish_offer->diminish_offervalue = 0;
            send_to_char("Your diminish pact breaks.\n\r", ch->diminish_offer);
          }
          ch->diminish_offervalue = 0;
          send_to_char("Your diminish pact breaks.\n\r", ch);
        }
        ch->diminished = 0;
        send_to_char("You stop diminishing yourself.\n\r", ch);
        return;
      }
      else {
        if (amount > ch->diminish_offervalue && ch->diminish_offervalue > 0) {
          if (ch->diminish_offer != NULL && ch->diminish_offer->in_room != NULL) {
            ch->diminish_offer->diminish_offervalue = 0;
            send_to_char("Your diminish pact breaks.\n\r", ch->diminish_offer);
          }
          ch->diminish_offervalue = 0;
          send_to_char("Your diminish pact breaks.\n\r", ch);
        }
        ch->diminished = amount;
        printf_to_char(ch, "You will now do %d percent damage.\n\r", ch->diminished);
      }
    }
  }

  void round_process(CHAR_DATA *ch) {
    ch->moved = 0;
    if (!IS_NPC(ch))
    ch->pcdata->combat_emoted = FALSE;
    if (!is_combat_flyer(ch) && ch->in_room->sector_type == SECT_AIR) {
      fall_character(ch);
    }
    if (!water_breathe(ch) && ch->in_room->sector_type == SECT_UNDERWATER) {
      swimup_character(ch);
    }
    if (ch->debuff > 0) {
      int reduce = UMAX(10, ch->debuff / 7);
      reduce = reduce * (5 + get_skill(ch, SKILL_STAMINA)) / 5;
      if (IS_NPC(ch))
      reduce *= 2;

      reduce = reduce * (100 - UMIN(ch->moved, 100)) / 100;
      ch->debuff -= reduce;
      if(!IS_NPC(ch) && ch->pcdata->boon == BOON_VITALITY && ch->pcdata->boon_timeout > current_time && ch->pcdata->spectre == 0)
      ch->debuff = 0;

      if (ch->debuff > 1000)
      ch->debuff = 1000;
    }

    if (!IS_NPC(ch) && ch->ability_timer == 0) {
      ch->ability_dead++;
      int used = -1;
      for (int i = 0; i < 20; i++) {
        if (ch->pcdata->abilcools[i] != 0) {
          used = i;
        }
      }
      if (used == -1)
      ch->ability_dead = 0;

      if (used + ch->ability_dead >= 8) {
        resetabils(ch);
      }
    }

    ch->damage_absorb = 0;
    ch->debuff = UMAX(ch->debuff, 0);

    ch->last_hit_by = NULL;
    ch->last_hit_damage = 0;

    if (!battleground(ch->in_room)) {
      if (!IS_FLAG(ch->comm, COMM_SPARRING) || number_percent() % 4 == 0)
      psychic_feast(ch, PSYCHIC_ANGER, 1);
    }

    if (ch->wounds == 0 && get_skill(ch, SKILL_DEMONMETABOL) > 0) {
      ch->hit = UMIN(max_hp(ch), ch->hit + 10);
    }

    if (has_caff(ch, CAFF_PARALYTICTOXIN)) {
      if (is_undead(ch))
      ch->debuff += 10;
      else
      ch->debuff += 30;
      if (!IS_NPC(ch))
      send_to_char("Your body feels sluggish.\n\r", ch);
    }
    if (has_caff(ch, CAFF_POISONTOXIN)) {
      if (!is_undead(ch)) {
        combat_damage(ch, ch, max_hp(ch) / 20, DIS_RADIATION);
        if (!IS_NPC(ch))
        send_to_char("Burning pain courses through your veins.\n\r", ch);
      }
    }

    if (IS_FLAG(ch->fightflag, FIGHT_MINIONS1)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_MINIONS1);
      SET_FLAG(ch->fightflag, FIGHT_MINIONS2);
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_MINIONS2)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_MINIONS2);
      summon_support(ch, SPECIAL_MINION);
    }

    if (IS_FLAG(ch->fightflag, FIGHT_ALLIES1)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_ALLIES1);
      if (get_skill(ch, SKILL_PROTECTIONDETAIL) > 0)
      SET_FLAG(ch->fightflag, FIGHT_ALLIES4);
      else
      SET_FLAG(ch->fightflag, FIGHT_ALLIES2);
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_ALLIES2)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_ALLIES2);
      SET_FLAG(ch->fightflag, FIGHT_ALLIES4);
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_ALLIES3)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_ALLIES3);
      SET_FLAG(ch->fightflag, FIGHT_ALLIES4);
    }
    else if (IS_FLAG(ch->fightflag, FIGHT_ALLIES4)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_ALLIES4);
      summon_support(ch, SPECIAL_ALLY);
    }
  }

  int round_cap(CHAR_DATA *ch, CHAR_DATA *victim) {
    int val = 50;
    if (guestmonster(victim) || higher_power(victim)) {
      if (in_water(victim))
      val = 120;
      else
      val = 80;
    }
    else if (wearing_armor(victim))
    val = 40;
    else
    val = 60;

    val += 25;
    if(!IS_NPC(victim) && get_skill(victim, SKILL_IMMORTALITY) > 0)
    val -= 25;
    else if(is_undead(victim))
    val -= 25;

    return val;
  }

  int monster_round_cap(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (cardinal(victim)) {
      if (!sinmatch(ch, victim))
      return 75;
      else
      return 500;
    }
    int val = round_cap(ch, victim);
    if (safe_strlen(victim->pcdata->dream_origin) < 1)
    return val;
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!str_cmp(victim->pcdata->dream_origin, (*it)->name)) {
        val = val * 2 / 3;
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, (*it)->participants[i])) {
            if ((*it)->participant_exp[i] >= 10000)
            val *= 2;
            if ((*it)->participant_exp[i] >= 1000)
            val *= 2;
          }
        }
      }
    }
    return val;
  }

  bool battleground(ROOM_INDEX_DATA *room) {

    if (room == NULL)
    return FALSE;

    if (room->area->vnum == 29)
    return TRUE;

    return FALSE;
  }

  int average_muscle(CHAR_DATA *ch) {
    int total = 0;
    int count = 0;
    for (int i = 0; i < MAX_CONTACTS; i++) {
      if (ch->pcdata->contact_jobs[i] == CJOB_MUSCLE && safe_strlen(ch->pcdata->contact_names[i]) > 1 && safe_strlen(ch->pcdata->contact_descs[i]) > 1) {
        total += get_skill(ch, contacts_table[i]);
        count++;
      }
    }
    if (count == 0)
    return 0;
    int dim = total % count;
    total -= dim;
    total /= count;
    return total;
  }
  int muscle_count(CHAR_DATA *ch) {
    int count = 0;
    for (int i = 0; i < MAX_CONTACTS; i++) {
      if (ch->pcdata->contact_jobs[i] == CJOB_MUSCLE && safe_strlen(ch->pcdata->contact_names[i]) > 1 && safe_strlen(ch->pcdata->contact_descs[i]) > 1) {
        count++;
      }
    }
    if (count > 4)
    return 4;

    return count;
  }

  int muscle_exp_cap(CHAR_DATA *ch) {
    int value = 60000 * skillpoint(average_muscle(ch));

    if (get_skill(ch, SKILL_MILITARYTRAINING) > 0)
    value = value * 5 / 4;
    return value;
  }

  int minion_exp_cap(CHAR_DATA *ch) {
    return 150000 * get_skill(ch, SKILL_MINIONS);
  }

  int minion_shield_cap(CHAR_DATA *ch) { return 10 * arcane_focus(ch); }

  int built_disc_cost(int value) {
    int result = (int)(value * (float)(((value + 1) / 2)));
    result *= BASE_DISC_COST;
    return result;
  }
  int discipline_point(int vnum) {
    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].vnum == vnum)
      return i;
    }
    return -1;
  }

  // x = 0 for minion, 1 for ally.
  int current_minion_cost(CHAR_DATA *ch, int x) {
    int shield_total = 0;
    int cost = 0;

    for (int i = 0; i < 10; i += 2) {
      if (discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
          .range == -1)
      shield_total += ch->pcdata->monster_discs[i + 1][x];
      else
      cost += built_disc_cost(ch->pcdata->monster_discs[i + 1][x]);
    }
    cost += built_disc_cost(shield_total);
    return cost;
  }

  // x = 0 for minion, 1 for ally.
  int current_minion_shield(CHAR_DATA *ch, int x) {
    int shield_total = 0;
    int cost = 0;

    for (int i = 0; i < 10; i += 2) {
      if (discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
          .range == -1)
      shield_total += ch->pcdata->monster_discs[i + 1][x];
      else
      cost += built_disc_cost(ch->pcdata->monster_discs[i + 1][x]);
    }
    return shield_total;
  }

  bool valid_minion(CHAR_DATA *ch) {
    int x = 0;
    for (int i = 0; i < 10; i += 2) {
      if (ch->pcdata->monster_discs[i + 1][x] > 0) {
        if (discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .range > 0 && discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .vnum != DIS_CLAW && discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .vnum != DIS_FIRE && discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .vnum != DIS_ICE && discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .vnum != DIS_LIGHTNING)
        return FALSE;

        if (get_skill(ch, SKILL_ELEMINIONS) < 1 && discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .range > 0 && discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])]
            .vnum != DIS_CLAW)
        return FALSE;
      }
    }
    return TRUE;
  }
  bool valid_ally(CHAR_DATA *ch) {
    int x = 1;
    for (int i = 0; i < 10; i += 2) {
      if (ch->pcdata->monster_discs[i + 1][x] > 0) {

        if (discipline_table[discipline_point(ch->pcdata->monster_discs[i][x])].pc == 0) {
          return FALSE;
        }
      }
    }
    return TRUE;
  }

  _DOFUN(do_glide) {
    if (get_skill(ch, SKILL_GLIDE) <= 0) {
      send_to_char("You don't know how to do that.\n\r", ch);
      return;
    }
    if (has_caff(ch, CAFF_GLIDED)) {
      send_to_char("You've done that too recently.\n\r", ch);
      return;
    }
    if (!in_fight(ch)) {
      send_to_char("You're not in a fight.\n\r", ch);
      return;
    }
    apply_caff(ch, CAFF_GLIDING, 3);
    apply_caff(ch, CAFF_GLIDED, 7);
    send_to_char("You get ready to glide.\n\4", ch);
  }

  void add_aggro(CHAR_DATA *victim, CHAR_DATA *ch, int amount) {
    if (ch->target == victim) {
      ch->target_dam += amount;
    }
    else if (ch->target_2 == victim) {
      ch->target_dam_2 += amount;
    }
    else if (ch->target_3 == victim) {
      ch->target_dam_3 += amount;
    }
    else {
      ch->target_3 = ch->target_2;
      ch->target_dam_3 = ch->target_dam_2;
      ch->target_2 = ch->target;
      ch->target_dam_2 = ch->target_dam;
      ch->target = victim;
      ch->target_dam = amount;
    }
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || is_gm(rch) || !IS_NPC(rch))
      continue;
      if (!same_fight(rch, ch))
      continue;

      if (!str_cmp(rch->protecting, "all") || !str_cmp(rch->protecting, victim->name) || !str_cmp(rch->protecting, get_fac(victim))) {
        if (ch->target == rch) {
          ch->target_dam += amount;
        }
        else if (ch->target_2 == rch) {
          ch->target_dam_2 += amount;
        }
        else if (ch->target_3 == rch) {
          ch->target_dam_3 += amount;
        }
        else {
          ch->target_3 = ch->target_2;
          ch->target_dam_3 = ch->target_dam_2;
          ch->target_2 = ch->target;
          ch->target_dam_2 = ch->target_dam;
          ch->target = rch;
          ch->target_dam = amount;
        }
      }

      if (!str_cmp(rch->protecting, "all") || !str_cmp(rch->protecting, ch->name) || !str_cmp(rch->protecting, get_fac(ch))) {
        if (victim == NULL || !in_fight(victim))
        return;
        if (victim->target == rch) {
          victim->target_dam += amount / 3;
        }
        else if (victim->target_2 == rch) {
          victim->target_dam_2 += amount / 3;
        }
        else if (victim->target_3 == rch) {
          victim->target_dam_3 += amount / 3;
        }
        else {

          victim->target_3 = victim->target_2;
          victim->target_dam_3 = victim->target_dam_2;
          victim->target_2 = victim->target;
          victim->target_dam_2 = victim->target_dam;
          victim->target = rch;
          victim->target_dam = amount / 3;
        }
      }
    }
  }

  void forest_fight(CHAR_DATA *ch, CHAR_DATA *target) {
    if (ch == NULL || ch->in_room == NULL)
    return;
    CHAR_DATA *victim;

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;
      if (ch == victim)
      continue;
      if (!IS_NPC(victim) || !forest_monster(victim))
      continue;
      if (victim->in_room == NULL)
      continue;
      if (in_world(target) != in_world(victim))
      continue;
      if (get_dist(target->in_room->x, target->in_room->y, victim->in_room->x, victim->in_room->y) > 3)
      continue;

      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
      SET_FLAG(victim->act, PLR_SHROUD);
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD))
      REMOVE_FLAG(victim->act, PLR_SHROUD);
      victim->in_fight = TRUE;
      victim->attacking = 1;
      victim->fight_speed = ch->fight_speed;
      victim->fight_fast = ch->fight_fast;
      add_aggro(victim, target, 50);
    }
  }

  void join_to_fight(CHAR_DATA *rch) {
    if (!IS_NPC(rch) && rch->pcdata->sleeping <= 300 && rch->pcdata->sleeping > 0 && rch->pcdata->coma < current_time)
    rch->pcdata->sleeping = 0;

    if (IS_FLAG(rch->comm, COMM_HOSTILE))
    REMOVE_FLAG(rch->comm, COMM_HOSTILE);

    if (IS_FLAG(rch->act, PLR_HIDE))
    REMOVE_FLAG(rch->act, PLR_HIDE);

    if (IS_AFFECTED(rch, AFF_NEUTRALIZED))
    apply_caff(rch, CAFF_NEUTRALIZED, 1);

    if (!has_shield(rch) && !IS_FLAG(rch->fightflag, FIGHT_NOSHIELD))
    SET_FLAG(rch->fightflag, FIGHT_NOSHIELD);
    if (!holding_lweapon(rch) && !has_lweapon(rch) && !IS_FLAG(rch->fightflag, FIGHT_NOSPEAR))
    SET_FLAG(rch->fightflag, FIGHT_NOSPEAR);

    if (is_sparring_room(rch->in_room) && !IS_FLAG(rch->comm, COMM_SPARRING))
    do_function(rch, &do_spar, "");

    rch->hadturn = FALSE;
    rch->in_fight = TRUE;
    rch->attacking = 1;
  }

  void start_fight(CHAR_DATA *ch, CHAR_DATA *target) {
    if (!IS_NPC(ch) && !IS_NPC(target)) {
      if ((institute_room(ch->in_room) || institute_room(target->in_room)) && (IS_SET(ch->in_room->room_flags, ROOM_INDOORS) || IS_SET(target->in_room->room_flags, ROOM_INDOORS))) {
        if (!is_institute_taught(ch) && (ch->pcdata->institute_action != INSTITUTE_TEACH || !can_institute_teach(ch))) {
          send_to_char("Institute security moves to stop the fight before it starts.\n\r", ch);
          return;
        }
      }
    }
    if (!IS_NPC(ch) && is_prisoner(ch)) {
      send_to_char("The guards would stop you.\n\r", ch);
      return;
    }
    if (!IS_NPC(target) && target->pcdata->sleeping > 0) {
      send_to_char("They're unconcious.\n\r", ch);
      return;
    }
    if (forest_monster(ch) && target->in_room != NULL && mist_level(target->in_room) < 3 && (target->in_room->area->vnum == INNER_NORTH_FOREST || target->in_room->area->vnum == INNER_SOUTH_FOREST || target->in_room->area->vnum == INNER_WEST_FOREST || target->in_room->area->vnum == HAVEN_TOWN_VNUM || target->in_room->sector_type != SECT_FOREST)) {
      ch->ttl = 1;
      return;
    }
    if (forest_monster(target) && ch->in_room != NULL && mist_level(ch->in_room) < 3 && (ch->in_room->area->vnum == INNER_NORTH_FOREST || ch->in_room->area->vnum == INNER_SOUTH_FOREST || ch->in_room->area->vnum == INNER_WEST_FOREST || ch->in_room->area->vnum == HAVEN_TOWN_VNUM || ch->in_room->sector_type != SECT_FOREST)) {
      target->ttl = 1;
      return;
    }

    if (!IS_NPC(ch)) {
      if (ch->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && ch->pcdata->patrol_status <= PATROL_DEFENDHIDING) {
        send_to_char("You cannot find your adversary in the chaotic nightmare.\n\r", ch);
        return;
      }

      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(target->act, PLR_SHROUD)) {
        send_to_char("They're not in the nightmare.\n\r", ch);
        return;
      }
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(target->act, PLR_SHROUD)) {
        send_to_char("They're in the nightmare.\n\r", ch);
        return;
      }
    }
    else {
      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(target->act, PLR_SHROUD)) {
        if (is_invader(ch)) {
          if (can_shroud(target))
          SET_FLAG(target->act, PLR_SHROUD);
          else
          return;
        }
        else
        REMOVE_FLAG(ch->act, PLR_SHROUD);
      }
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(target->act, PLR_SHROUD)) {
        SET_FLAG(ch->act, PLR_SHROUD);
      }
    }

    if (IS_FLAG(ch->comm, COMM_PACIFIST))
    REMOVE_FLAG(ch->comm, COMM_PACIFIST);
    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->act, PLR_HIDE);

    if (!IS_NPC(ch) && !IS_NPC(target)) {
      ch->fight_fast = FALSE;
      if (ch->diminish_lt > 0 && ch->diminish_lt < 100 && ch->diminish_till > current_time) {
        if (ch->diminish_vis == 1 || (ch->diminish_vis == 3)) {
          char buf[MSL];
          sprintf(
          buf, "`W($n appears to only be fighting at %d percent effectiveness.)", ch->diminish_lt);
          act(buf, ch, NULL, NULL, TO_ROOM);
        }
      }
      if (target->diminish_lt > 0 && target->diminish_lt < 100 && target->diminish_till > current_time) {
        if (target->diminish_vis == 1 || (target->diminish_vis == 3)) {
          char buf[MSL];
          sprintf(
          buf, "`W($n appears to only be fighting at %d percent effectiveness.)", target->diminish_lt);
          act(buf, target, NULL, NULL, TO_ROOM);
        }
      }
    }
    if (guestmonster(ch) || guestmonster(target)) {
      ch->fight_fast = TRUE;
      ch->fight_speed = 10;
      target->fight_fast = TRUE;
      target->fight_speed = 10;
    }
    if ((IS_NPC(ch) && IS_FLAG(ch->act, ACT_BIGGAME)) || (IS_NPC(target) && IS_FLAG(target->act, ACT_BIGGAME))) {
      ch->fight_fast = TRUE;
      ch->fight_speed = 4;
      target->fight_fast = TRUE;
      target->fight_speed = 4;
    }

    bool starting = FALSE;
    ch->hadturn = FALSE;
    if (in_fight(target)) {
      ch->fight_fast = target->fight_fast;
      ch->fight_speed = target->fight_speed;
    }
    else {
      target->hadturn = FALSE;
      join_to_fight(target);
      send_to_char("`rYou enter combat.`x\n\r", target);
      starting = TRUE;
    }
    if (IS_NPC(ch) && forest_monster(ch)) {
      if (!IS_NPC(target) && (IS_FLAG(target->comm, COMM_AFK) || target->desc == NULL))
      ch->fight_speed = 5;
      else
      ch->fight_speed = target->fight_speed;
      add_aggro(ch, target, 50);
      forest_fight(ch, target);
    }
    else
    add_aggro(ch, target, 20);

    target->fight_fast = ch->fight_fast;
    target->fight_speed = ch->fight_speed;
    ch->attacking = 1;
    target->attacking = 1;
    int count = 1;
    ch->in_fight = TRUE;
    target->in_fight = TRUE;
    if (is_sparring_room(ch->in_room) && !IS_FLAG(ch->comm, COMM_SPARRING))
    do_function(ch, &do_spar, "");
    if (is_sparring_room(target->in_room) && !IS_FLAG(target->comm, COMM_SPARRING))
    do_function(target, &do_spar, "");

    if (ch->fight_fast == FALSE && !IS_NPC(ch) && !IS_NPC(target)) {
      ch->pcdata->autoskip = 0;
      target->pcdata->autoskip = 0;
    }
    int limit = 0;
    CHAR_DATA *part = next_fight_member_init(ch);
    for (; part != NULL && part != ch && limit < 200;) {
      limit++;
      if (!IS_NPC(part))
      count++;
      part->fight_fast = ch->fight_fast;
      part->fight_speed = ch->fight_speed;
      if (part->attacking == 0) {
        join_to_fight(part);
        send_to_char("`RA fight breaks out.`x\n\r", part);
      }
      part = next_fight_member_init(part);
    }
    if (count >= 5 && ch->fight_fast == FALSE) {
      ch->fight_fast = TRUE;
      ch->fight_speed = 3;
      part = next_fight_member(ch);
      limit = 0;
      for (; part != NULL && part != ch && limit < 200;) {
        limit++;
        part->fight_fast = ch->fight_fast;
        part->fight_speed = ch->fight_speed;
        part = next_fight_member(part);
      }
    }
    else if (ch->fight_fast == FALSE && starting == TRUE) {
      reset_turns(ch);
      next_attacker(ch, TRUE);
    }
    else if (ch->fight_fast == FALSE && (ch->fight_current == NULL || !same_fight(ch, ch->fight_current))) {
      reset_turns(ch);
      next_attacker(ch, TRUE);
    }
  }

  void start_roomfight(ROOM_INDEX_DATA *room, bool fast, int speed, bool shroud, bool deepshroud) {
    if (fight_problem > 0)
    return;
    CHAR_DATA *victim;
    CHAR_DATA *last;
    CHAR_DATA *other;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;
      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;
      if (IS_NPC(victim))
      continue;
      if (deepshroud == TRUE && IS_FLAG(victim->act, PLR_DEEPSHROUD)) {
        last = victim;
        join_to_fight(victim);
        victim->fight_speed = speed;
        victim->fight_fast = fast;
        victim->attacking = 1;
        victim->in_fight = TRUE;
        if (fast == FALSE)
        victim->pcdata->autoskip = 0;
      }
      if (deepshroud == FALSE && shroud == TRUE && IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_DEEPSHROUD)) {
        last = victim;
        join_to_fight(victim);
        victim->fight_speed = speed;
        victim->fight_fast = fast;
        victim->attacking = 1;
        victim->in_fight = TRUE;
        if (fast == FALSE)
        victim->pcdata->autoskip = 0;
      }
      if (deepshroud == FALSE && shroud == FALSE && !IS_FLAG(victim->act, PLR_SHROUD)) {
        last = victim;
        join_to_fight(victim);
        victim->fight_speed = speed;
        victim->fight_fast = fast;
        victim->attacking = 1;
        victim->in_fight = TRUE;
        if (fast == FALSE)
        victim->pcdata->autoskip = 0;
      }
      for (CharList::iterator ij = room->people->begin();
      ij != room->people->end(); ++ij) {
        other = *it;
        if (other == NULL)
        continue;
        if (other->in_room == NULL)
        continue;
        if (IS_NPC(other))
        continue;
        if (!same_faction(other, victim))
        add_aggro(other, victim, 20);
      }
    }
    int count = 0;
    CHAR_DATA *part = next_fight_member_init(last);
    int limit = 0;
    for (; part != NULL && part != last && limit < 200;) {
      limit++;
      if (!IS_NPC(part))
      count++;
      part->fight_fast = last->fight_fast;
      part->fight_speed = last->fight_speed;
      if (part->attacking == 0) {
        join_to_fight(part);
        send_to_char("`RA fight breaks out.`x\n\r", part);
      }
      part = next_fight_member_init(part);
    }
    if (count >= 5 && last->fight_fast == FALSE) {
      last->fight_fast = TRUE;
      last->fight_speed = 3;
      part = next_fight_member(last);
      limit = 0;
      for (; part != NULL && part != last && limit < 200;) {
        limit++;
        part->fight_fast = last->fight_fast;
        part->fight_speed = last->fight_speed;
        part = next_fight_member(part);
      }
    }
    else if (last->fight_fast == FALSE && (last->fight_current == NULL || !same_fight(last, last->fight_current))) {
      reset_turns(last);
      next_attacker(last, TRUE);
    }
  }
  bool is_enemy(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (fight_problem == 1)
    return FALSE;
    if (ch == NULL || victim == NULL)
    return FALSE;
    if (ch == victim)
    return FALSE;
    if (ch->in_room == NULL || victim->in_room == NULL)
    return FALSE;
    if (IS_NPC(victim) && IS_FLAG(victim->act, ACT_SENTINEL))
    return FALSE;
    if (ch->in_room->x - victim->in_room->x > 20)
    return FALSE;
    if (ch->in_room->y - victim->in_room->y > 20)
    return FALSE;
    if (ch->in_room->x - victim->in_room->x < -20)
    return FALSE;
    if (ch->in_room->y - victim->in_room->y < -20)
    return FALSE;

    if (IS_NPC(victim) && IS_FLAG(victim->act, ACT_COMBATOBJ))
    return FALSE;

    if (in_world(ch) != in_world(victim))
    return FALSE;

    if (is_gm(ch) || is_gm(victim))
    return FALSE;

    if (is_ghost(ch) || is_ghost(victim))
    return FALSE;
    if (is_helpless(ch) || is_helpless(victim))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;
    if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return FALSE;
    if (!IS_FLAG(ch->act, PLR_DEEPSHROUD) && IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return FALSE;

    if (victim->in_room != NULL && IS_SET(victim->in_room->room_flags, ROOM_PUBLIC) && !IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;
    if (ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_PUBLIC) && !IS_FLAG(ch->act, PLR_SHROUD))
    return FALSE;

    if (IS_NPC(ch) && victim->in_room != NULL && !can_get_to(ch, victim->in_room))
    return FALSE;

    if (forest_monster(ch) && victim->in_room != NULL && mist_level(victim->in_room) < 3 && (victim->in_room->area->vnum == INNER_NORTH_FOREST || victim->in_room->area->vnum == INNER_SOUTH_FOREST || victim->in_room->area->vnum == INNER_WEST_FOREST || victim->in_room->area->vnum == HAVEN_TOWN_VNUM || victim->in_room->sector_type != SECT_FOREST))
    return FALSE;
    if (forest_monster(victim) && ch->in_room != NULL && mist_level(ch->in_room) < 3 && (ch->in_room->area->vnum == INNER_NORTH_FOREST || ch->in_room->area->vnum == INNER_SOUTH_FOREST || ch->in_room->area->vnum == INNER_WEST_FOREST || ch->in_room->area->vnum == HAVEN_TOWN_VNUM || ch->in_room->sector_type != SECT_FOREST))
    return FALSE;

    int dist = init_combat_distance(ch, victim, TRUE);
    if (dist >= 250)
    return FALSE;

    if (IS_NPC(ch) && IS_FLAG(ch->act, ACT_COMBATOBJ) && !IS_NPC(victim) && victim->in_fight == TRUE)
    return TRUE;

    /*
if(!IS_NPC(ch))
printf_to_char(ch, "Enemy Check: %s, %s, ch->vic aggro: %d, vic->ch aggro
%d\n\r", ch->name, victim->name, get_agg(ch, victim), get_agg(victim, ch));
if(!IS_NPC(victim))
printf_to_char(victim, "Enemy Check: %s, %s, ch->vic aggro: %d, vic->ch aggro
%d\n\r", ch->name, victim->name, get_agg(ch, victim), get_agg(victim, ch));
*/

    if (get_agg(ch, victim) > 5 && ch->attacking != 0) {
      if (ch->in_room == victim->in_room)
      return TRUE;

      if (can_get_to(ch, victim->in_room))
      return TRUE;

      if (can_see_char_distance(ch, victim, DISTANCE_MEDIUM) && dist < 150 && get_disc(ch, default_ranged(ch), FALSE) >= 10)
      return TRUE;
    }

    if (get_agg(victim, ch) > 5 && victim->attacking != 0) {
      if (ch->in_room == victim->in_room)
      return TRUE;
      if (can_get_to(victim, ch->in_room))
      return TRUE;
      if (can_see_char_distance(victim, ch, DISTANCE_MEDIUM) && dist < 150 && get_disc(victim, default_ranged(victim), FALSE) >= 10)
      return TRUE;
    }

    return FALSE;
  }

  bool has_enemy(CHAR_DATA *ch) {
    if (fight_problem == 1)
    return FALSE;
    if (IS_NPC(ch) && IS_FLAG(ch->act, ACT_SENTINEL))
    return FALSE;
    if (!IS_NPC(ch) && is_helpless(ch))
    return FALSE;

    CHAR_DATA *wch;
    int limit = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && limit < 1000; ++it) {
      limit++;
      wch = *it;

      if (ch == wch)
      continue;

      if (is_enemy(ch, wch)) {
        return TRUE;
      }
    }
    if (limit > 800) {
      log_string("FIGHTPROBLEM: Has enemy");
      fight_problem += 1;
    }
    return FALSE;
  }

  bool same_fight(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (fight_problem > 0)
    return FALSE;
    if (ch == victim)
    return TRUE;

    if (!in_fight(ch) || !in_fight(victim))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;
    if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(victim->act, PLR_SHROUD))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return FALSE;
    if (!IS_FLAG(ch->act, PLR_DEEPSHROUD) && IS_FLAG(victim->act, PLR_DEEPSHROUD))
    return FALSE;

    if (in_world(ch) != in_world(victim))
    return FALSE;

    int dist = combat_distance(ch, victim, FALSE);
    if (dist <= 100)
    return TRUE;
    if (dist > 1000)
    return FALSE;

    if (is_enemy(ch, victim))
    return TRUE;
    CHAR_DATA *wch;
    int limit = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && limit < 2000; ++it) {
      wch = *it;
      if (ch == wch)
      continue;

      if (is_enemy(ch, wch) && combat_distance(victim, wch, FALSE) <= 250)
      return TRUE;
      if (is_enemy(victim, wch) && combat_distance(ch, wch, FALSE) <= 250)
      return TRUE;
      limit++;
    }
    if (limit > 1500) {
      log_string("FIGHTPROBLEM: Same Fight");
      fight_problem++;
    }

    return FALSE;
  }

  bool in_fight(CHAR_DATA *ch) {
    if (fight_problem > 0)
    return FALSE;
    return ch->in_fight;
  }
  bool check_fight(CHAR_DATA *ch) {
    if (fight_problem > 0)
    return FALSE;
    if (is_ghost(ch) || is_gm(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    if (IS_NPC(ch) && IS_FLAG(ch->act, ACT_SENTINEL))
    return FALSE;

    if (!IS_NPC(ch) && is_helpless(ch))
    return FALSE;

    if (ch->in_room != NULL && battleground(ch->in_room))
    return TRUE;

    if (has_enemy(ch))
    return TRUE;

    CHAR_DATA *wch;
    int limit = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && limit < 1000; ++it) {
      limit++;
      wch = *it;
      if (wch == NULL || wch->act == NULL || wch->in_room == NULL)
      continue;
      if (ch == wch)
      continue;

      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(wch->act, PLR_SHROUD))
      continue;
      if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(wch->act, PLR_SHROUD))
      continue;
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(wch->act, PLR_DEEPSHROUD))
      continue;
      if (!IS_FLAG(ch->act, PLR_DEEPSHROUD) && IS_FLAG(wch->act, PLR_DEEPSHROUD))
      continue;

      if (has_enemy(wch) && (combat_distance(ch, wch, FALSE) <= 50 || ch->in_room == wch->in_room) && (ch->in_room == wch->in_room || can_see_char_distance(ch, wch, DISTANCE_MEDIUM) || can_see_char_distance(wch, ch, DISTANCE_MEDIUM))) {
        if (ch->fight_current == NULL && wch->fight_current != NULL)
        ch->fight_current = wch->fight_current;
        else if (wch->fight_current == NULL && ch->fight_current != NULL)
        wch->fight_current = ch->fight_current;
        return TRUE;
      }
    }
    if (limit > 800) {
      log_string("FIGHTPROBLEM: Check fight");
      fight_problem += 1;
    }
    return FALSE;
  }

  CHAR_DATA *next_fight_member(CHAR_DATA *current) {
    bool found = FALSE;
    CHAR_DATA *wch;
    int count = 0;
    if (fight_problem > 0)
    return NULL;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {
      wch = *it;
      if (current == wch)
      found = TRUE;
      else if (found != TRUE)
      continue;

      if (current->fight_fast == FALSE && !IS_NPC(wch) && wch->pcdata->autoskip == 1)
      continue;

      if (in_fight(wch) && same_fight(current, wch) && wch != current)
      return wch;

      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: Next_fight_member_a");
      fight_problem++;
    }
    count = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {
      wch = *it;
      if (current == wch)
      found = TRUE;
      else if (found != TRUE)
      continue;

      if (in_fight(wch) && same_fight(current, wch) && wch != current)
      return wch;

      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: Next_fight_member_a");
      fight_problem++;
    }
    count = 0;

    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 2000; ++it) {

      wch = *it;

      if (current->fight_fast == FALSE && !IS_NPC(wch) && wch->pcdata->autoskip == 1)
      continue;

      if (wch == current)
      return current;

      if (in_fight(wch) && same_fight(current, wch))
      return wch;
      count++;
    }
    if (count > 1500) {
      log_string("FIGHTPROBLEM: Next_fight_member_b");
      fight_problem++;
    }

    return NULL;
  }

  CHAR_DATA *next_fight_member_init(CHAR_DATA *current) {
    bool found = FALSE;
    CHAR_DATA *wch;
    int count = 0;
    if (fight_problem > 0)
    return NULL;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 1000; ++it) {
      count++;
      wch = *it;
      if (current == wch)
      found = TRUE;
      else if (found != TRUE)
      continue;

      if (current->fight_fast == FALSE && !IS_NPC(wch) && wch->pcdata->autoskip == 1)
      continue;

      if (check_fight(wch) && same_fight(current, wch) && wch != current)
      return wch;
    }
    if (count > 800) {
      log_string("FIGHTPROBLEM: Next_fight_member_init_a");
      fight_problem++;
    }

    count = 0;
    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 1000; ++it) {
      count++;
      wch = *it;
      if (current == wch)
      found = TRUE;
      else if (found != TRUE)
      continue;

      if (check_fight(wch) && same_fight(current, wch) && wch != current)
      return wch;
    }
    if (count > 800) {
      log_string("FIGHTPROBLEM: Next_fight_member_init_a");
      fight_problem++;
    }

    count = 0;

    for (CharList::iterator it = char_list.begin();
    it != char_list.end() && count < 1000; ++it) {
      count++;
      wch = *it;
      if (current->fight_fast == FALSE && !IS_NPC(wch) && wch->pcdata->autoskip == 1)
      continue;

      if (wch == current)
      return current;

      if (check_fight(wch) && same_fight(current, wch))
      return wch;
    }
    if (count > 800) {
      log_string("FIGHTPROBLEM: Next_fight_member_init_b");
      fight_problem++;
    }

    return NULL;
  }

  bool room_fight(ROOM_INDEX_DATA *room, bool shroud, bool deepshroud, bool any) {
    CHAR_DATA *victim;
    if (room == NULL)
    return FALSE;
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;
      if (victim == NULL)
      continue;
      if (victim->in_room == NULL)
      continue;

      if (in_fight(victim) && any == TRUE)
      return TRUE;
      if (IS_FLAG(victim->act, PLR_DEEPSHROUD) && deepshroud == TRUE && in_fight(victim))
      return TRUE;
      if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_DEEPSHROUD) && shroud == TRUE && deepshroud == FALSE && in_fight(victim))
      return TRUE;
      if (!IS_FLAG(victim->act, PLR_SHROUD) && shroud == FALSE && deepshroud == FALSE && in_fight(victim))
      return TRUE;
    }
    return FALSE;
  }

  int fight_pop(CHAR_DATA *ch) {
    int count = 1;
    CHAR_DATA *part = next_fight_member(ch);
    if (part == NULL || part == ch)
    return count;
    int limit = 0;
    for (; part != NULL && part != ch && limit < 1000;) {
      count++;
      part = next_fight_member(part);
      limit++;
    }
    if (limit > 850) {
      log_string("FIGHTPROBLEM: fight_pop");
      fight_problem++;
    }

    return count;
  }

  void start_hostilefight(CHAR_DATA *ch) {
    if (pc_pop(ch->in_room) >= 5) {
      ch->fight_fast = TRUE;
      ch->fight_speed = 3;
    }
    else {
      ch->fight_fast = FALSE;
      ch->fight_speed = 1;
    }
    if (IS_FLAG(ch->act, PLR_DEEPSHROUD))
    start_roomfight(ch->in_room, ch->fight_fast, ch->fight_speed, TRUE, TRUE);
    else if (IS_FLAG(ch->act, PLR_SHROUD))
    start_roomfight(ch->in_room, ch->fight_fast, ch->fight_speed, TRUE, FALSE);
    else
    start_roomfight(ch->in_room, ch->fight_fast, ch->fight_speed, FALSE, FALSE);
  }

  int relative_x(CHAR_DATA *ch, ROOM_INDEX_DATA *desti, int x) {
    if (ch == NULL || ch->in_room == NULL || desti == NULL)
    return 0;
    if (x > desti->size)
    x = desti->size - 1;

    int newx = x - ch->x;
    if (desti->x != ch->in_room->x)
    newx += (desti->x - ch->in_room->x) * (ch->in_room->size + desti->size) / 2;
    return newx;
  }
  int relative_y(CHAR_DATA *ch, ROOM_INDEX_DATA *desti, int y) {
    if (ch == NULL || ch->in_room == NULL || desti == NULL)
    return 0;
    if (y > desti->size)
    y = desti->size - 1;
    int newy = y - ch->y;
    if (desti->y != ch->in_room->y)
    newy += (desti->y - ch->in_room->y) * (ch->in_room->size + desti->size) / 2;
    return newy;
  }
  int relative_z(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (ch == NULL || ch->in_room == NULL || desti == NULL)
    return 0;

    if (desti->z == ch->in_room->z)
    return 0;
    int newz = (desti->z - ch->in_room->z);
    int distance = 0;
    if (newz > 0) {
      newz--;
      distance += 25;
    }
    if (newz < 0) {
      newz++;
      distance -= 25;
    }
    distance += newz * 10;
    return distance;
  }

  int init_combat_distance(CHAR_DATA *ch, CHAR_DATA *victim, bool attacking) {
    if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
    return 1;

    if (in_world(ch) != in_world(victim))
    return 10000;
    if (battleground(ch->in_room) && !battleground(victim->in_room))
    return 10000;
    if (ch->in_room->x - victim->in_room->x > 20)
    return 10000;
    if (ch->in_room->y - victim->in_room->y > 20)
    return 10000;
    if (ch->in_room->x - victim->in_room->x < -20)
    return 10000;
    if (ch->in_room->y - victim->in_room->y < -20)
    return 10000;

    int xdiff = relative_x(ch, victim->in_room, victim->x);
    int ydiff = relative_y(ch, victim->in_room, victim->y);
    if (xdiff == 0 && ydiff == 0 && relative_z(ch, victim->in_room) == 0)
    return 1;

    int distance;
    if (xdiff > 300 || xdiff < -300 || ydiff > 300 || ydiff < -300) {
      int xdist = UMAX(xdiff, xdiff * -1);
      int ydist = UMAX(ydiff, ydiff * -1);
      if (xdist >= ydist)
      distance = xdist + ydist / 2;
      else
      distance = ydist + xdist / 2;
      return distance;
    }
    else
    distance = get_dist(0, 0, xdiff, ydiff);

    distance = UMAX(distance, 1);

    return distance;
    //     int relz = relative_z(ch, victim->in_room);
    //     if(relz < 0)
    //        relz *= -1;
    //     if(distance < relz)
    //        distance = relz;
    //     return distance;
  }

  int combat_distance(CHAR_DATA *ch, CHAR_DATA *victim, bool attacking) {
    if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
    return 1;

    if (in_world(ch) != in_world(victim))
    return 10000;
    if (battleground(ch->in_room) && !battleground(victim->in_room))
    return 10000;
    if (ch->in_room->x - victim->in_room->x > 20)
    return 10000;
    if (ch->in_room->y - victim->in_room->y > 20)
    return 10000;
    if (ch->in_room->x - victim->in_room->x < -20)
    return 10000;
    if (ch->in_room->y - victim->in_room->y < -20)
    return 10000;

    int xdiff = relative_x(ch, victim->in_room, victim->x);
    int ydiff = relative_y(ch, victim->in_room, victim->y);
    if (xdiff == 0 && ydiff == 0 && relative_z(ch, victim->in_room) == 0)
    return 1;

    int distance;
    if (xdiff > 300 || xdiff < -300 || ydiff > 300 || ydiff < -300) {
      int xdist = UMAX(xdiff, xdiff * -1);
      int ydist = UMAX(ydiff, ydiff * -1);
      if (xdist >= ydist)
      distance = xdist + ydist / 2;
      else
      distance = ydist + xdist / 2;
      return distance;
    }
    else
    distance = get_dist(0, 0, xdiff, ydiff);

    distance = UMAX(distance, 1);

    if (attacking == FALSE)
    return distance;
    if (is_combat_jumper(ch))
    return distance;

    int relz = relative_z(ch, victim->in_room);
    if (relz < 0)
    relz *= -1;
    if (distance < relz)
    distance = relz;
    return distance;
  }

  void to_combat_room(CHAR_DATA *ch, ROOM_INDEX_DATA *desti, int z) {
    ROOM_INDEX_DATA *newroom;
    if (z != 0) {
      newroom = sourced_room_by_coordinates(desti, desti->x, desti->y, desti->z + z, FALSE);
      if (can_get_to(ch, newroom)) {
        char_from_room(ch);
        char_to_room(ch, newroom);
        return;
      }
    }
    if (can_get_to(ch, desti)) {
      char_from_room(ch);
      char_to_room(ch, desti);
      return;
    }
    if (desti->exit[DIR_UP] != NULL) {
      newroom = desti->exit[DIR_UP]->u1.to_room;
      if (can_get_to(ch, newroom)) {
        char_from_room(ch);
        char_to_room(ch, newroom);
        return;
      }
      if (newroom->exit[DIR_UP] != NULL) {
        newroom = newroom->exit[DIR_UP]->u1.to_room;
        if (can_get_to(ch, newroom)) {
          char_from_room(ch);
          char_to_room(ch, newroom);
          return;
        }
        if (newroom->exit[DIR_UP] != NULL) {
          newroom = newroom->exit[DIR_UP]->u1.to_room;
          if (can_get_to(ch, newroom)) {
            char_from_room(ch);
            char_to_room(ch, newroom);
            return;
          }
        }
      }
    }
    if (desti->exit[DIR_DOWN] != NULL) {
      newroom = desti->exit[DIR_DOWN]->u1.to_room;
      if (can_get_to(ch, newroom)) {
        char_from_room(ch);
        char_to_room(ch, newroom);
        return;
      }
      if (newroom->exit[DIR_DOWN] != NULL) {
        newroom = newroom->exit[DIR_DOWN]->u1.to_room;
        if (can_get_to(ch, newroom)) {
          char_from_room(ch);
          char_to_room(ch, newroom);
          return;
        }
        if (newroom->exit[DIR_DOWN] != NULL) {
          newroom = newroom->exit[DIR_DOWN]->u1.to_room;
          if (can_get_to(ch, newroom)) {
            char_from_room(ch);
            char_to_room(ch, newroom);
            return;
          }
          if (newroom->exit[DIR_DOWN] != NULL) {
            newroom = newroom->exit[DIR_DOWN]->u1.to_room;
            if (can_get_to(ch, newroom)) {
              char_from_room(ch);
              char_to_room(ch, newroom);
              return;
            }
            if (newroom->exit[DIR_DOWN] != NULL) {
              newroom = newroom->exit[DIR_DOWN]->u1.to_room;
              if (can_get_to(ch, newroom)) {
                char_from_room(ch);
                char_to_room(ch, newroom);
                return;
              }
            }
          }
        }
      }
    }
  }

  void move_relative(CHAR_DATA *ch, int x, int y, int z) {
    ch->x += x;
    ch->y += y;
    bool breakout = FALSE;
    ROOM_INDEX_DATA *oldroom = ch->in_room;
    for (; ch->x > ch->in_room->size && ch->y > ch->in_room->size && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_NORTHEAST] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_NORTHEAST]->u1.to_room;
        ch->x -= ch->in_room->size;
        ch->y -= ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->x += ch->in_room->size;
          ch->y += ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->x > ch->in_room->size && ch->y < 0 && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_SOUTHEAST] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_SOUTHEAST]->u1.to_room;
        ch->x -= ch->in_room->size;
        ch->y += ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->x += ch->in_room->size;
          ch->y -= ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->x < 0 && ch->y < 0 && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_SOUTHWEST] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_SOUTHWEST]->u1.to_room;
        ch->x += ch->in_room->size;
        ch->y += ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->x -= ch->in_room->size;
          ch->y -= ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->x < 0 && ch->y > ch->in_room->size && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_NORTHWEST] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_NORTHWEST]->u1.to_room;
        ch->x += ch->in_room->size;
        ch->y -= ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->x -= ch->in_room->size;
          ch->y += ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->y > ch->in_room->size && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_NORTH] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_NORTH]->u1.to_room;
        ch->y -= ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->y += ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->x > ch->in_room->size && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_EAST] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_EAST]->u1.to_room;
        ch->x -= ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->x += ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->x < 0 && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_WEST] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_WEST]->u1.to_room;
        ch->x += ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->x -= ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    breakout = FALSE;
    for (; ch->y < 0 && breakout == FALSE;) {
      if (ch->in_room->exit[DIR_SOUTH] != NULL) {
        ROOM_INDEX_DATA *toroom = ch->in_room->exit[DIR_SOUTH]->u1.to_room;
        ch->y += ch->in_room->size;
        to_combat_room(ch, toroom, z);
        if (ch->in_room == oldroom) {
          ch->y -= ch->in_room->size;
          breakout = TRUE;
        }
      }
      else
      breakout = TRUE;
    }
    ch->x = UMIN(ch->x, ch->in_room->size);
    ch->y = UMIN(ch->y, ch->in_room->size);
    ch->x = UMAX(ch->x, 0);
    ch->y = UMAX(ch->y, 0);
  }

  CHAR_DATA *get_char_fight(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    char temp[MSL];
    CHAR_DATA *wch;
    int number;
    int count;
    number = number_argument(argument, arg);
    count = 0;

    if (!str_cmp(argument, "self") || !str_cmp(argument, "me"))
    return ch;
    // printf_to_char(ch, "Getting: %s: ", argument);
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      wch = *it;
      // printf_to_char(ch, ":%s:", wch->name);
      if (!can_see_char_distance(ch, wch, DISTANCE_MEDIUM))
      continue;
      // printf_to_char(ch, "1");
      if (!can_map_see(ch, wch))
      continue;
      // printf_to_char(ch, "2");
      if (!IS_NPC(wch))
      remove_color(temp, PERS(wch, ch));
      else
      remove_color(temp, wch->name);

      if (wch->in_room == NULL || !is_name(arg, temp))
      continue;
      // printf_to_char(ch, "3");
      if (++count == number)
      return wch;
      // printf_to_char(ch, "4");
    }
    return NULL;
  }

  int fight_path(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination) {
    if (in_room == destination)
    return -1;
    int zdiff = get_roomz(in_room) - get_roomz(destination);
    if (in_room->x == destination->x && in_room->y == destination->y) {
      if (zdiff > 0 && open_sound(in_room, DIR_DOWN))
      return DIR_DOWN;
      if (zdiff < 0 && open_sound(in_room, DIR_UP))
      return DIR_UP;
      return -1;
    }
    for (int i = 0; i < 10; i++) {
      if (in_room->exit[i] != NULL && in_room->exit[i]->u1.to_room == destination && open_sound(in_room, i))
      return i;
    }
    int dir =
    roomdirection(in_room->x, in_room->y, destination->x, destination->y);
    {
      if (open_sound(in_room, dir))
      return dir;
    }
    if (zdiff > 0 && open_sound(in_room, DIR_DOWN))
    return DIR_DOWN;
    if (zdiff < 0 && open_sound(in_room, DIR_UP))
    return DIR_UP;
    return -1;
  }

  bool is_combat_flyer(CHAR_DATA *ch) {
    if (has_caff(ch, CAFF_GUST))
    return FALSE;
    if (can_fly(ch))
    return TRUE;
    if (has_caff(ch, CAFF_GLIDING))
    return TRUE;
    return FALSE;
  }

  bool is_combat_jumper(CHAR_DATA *ch) {
    if (has_caff(ch, CAFF_GUST))
    return FALSE;
    if (get_skill(ch, SKILL_SUPERJUMP) > 0)
    return TRUE;
    if (is_combat_flyer(ch))
    return TRUE;
    if (get_skill(ch, SKILL_NIGHTASPECT) > 1)
    return TRUE;
    if (get_skill(ch, SKILL_TELEKINESIS) > 1)
    return TRUE;
    if (!IS_FLAG(ch->act, PLR_SHROUD) && get_skill(ch, SKILL_PATHING) >= 3)
    return TRUE;
    return FALSE;
  }

  bool get_to_path(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (ch->in_room == desti)
    return TRUE;
    ROOM_INDEX_DATA *orig = ch->in_room;
    int direction = 0;
    EXIT_DATA *pexit;

    if (orig == NULL || desti == NULL)
    return FALSE;
    int distance = 0;

    if (desti->sector_type == SECT_UNDERWATER && !water_breathe(ch))
    return FALSE;
    if (desti->sector_type == SECT_AIR && !is_combat_flyer(ch))
    return FALSE;

    if (desti->sector_type == SECT_AIR && ch->debuff >= 75) {
      send_to_char("You are too disoriented to fly.\n\r", ch);
      return FALSE;
    }

    if (IS_SET(desti->room_flags, ROOM_INDOORS) && is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) == ANIMAL_MONSTEROUS)
    return FALSE;
    if (IS_SET(desti->room_flags, ROOM_INDOORS) && guestmonster(ch) && ch->pcdata->height_feet >= 10)
    return FALSE;

    for (; orig->vnum != desti->vnum;) {
      if (orig == desti)
      return TRUE;

      direction = fight_path(orig, desti);

      if (direction == -1)
      return FALSE;
      pexit = orig->exit[direction];
      orig = pexit->u1.to_room;
      if (orig->sector_type == SECT_UNDERWATER && !water_breathe(ch))
      return FALSE;
      if (orig->sector_type == SECT_AIR && !is_combat_flyer(ch) && !is_combat_jumper(ch))
      return FALSE;
      if (IS_SET(orig->room_flags, ROOM_INDOORS) && is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) == ANIMAL_MONSTEROUS)
      return FALSE;
      if (IS_SET(orig->room_flags, ROOM_INDOORS) && guestmonster(ch) && ch->pcdata->height_feet >= 10)
      return FALSE;

      distance++;
      if (distance > 15)
      return FALSE;
    }
    if (orig->vnum == desti->vnum)
    return TRUE;
    return FALSE;
  }

  bool can_get_to(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (desti == NULL)
    return FALSE;

    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      if (in_lodge(desti))
      return FALSE;
      if (prop_from_room(desti) != NULL && prop_from_room(desti)->shroudshield >= 50)
      return FALSE;
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && no_deep_access(ch->in_room, desti))
      return FALSE;
    }
    return get_to_path(ch, desti);
  }

  void fall_character(CHAR_DATA *ch) {
    if (ch->in_room == NULL || ch->in_room->sector_type != SECT_AIR)
    return;
    ROOM_INDEX_DATA *desti = ch->in_room;
    for (int i = 0; i < 20; i++) {
      if (desti->sector_type == SECT_AIR && desti->exit[DIR_DOWN] != NULL && desti->exit[DIR_DOWN]->u1.to_room != NULL && open_sound(desti, DIR_DOWN)) {
        desti = desti->exit[DIR_DOWN]->u1.to_room;
      }
    }
    if (desti != ch->in_room) {
      act("$n falls.", ch, NULL, NULL, TO_ROOM);
      dact("$n falls.", ch, NULL, NULL, DISTANCE_MEDIUM);
      char_from_room(ch);
      char_to_room(ch, desti);
      act("$n lands.", ch, NULL, NULL, TO_ROOM);
      dact("$n lands.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
  }
  void swimup_character(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *desti = ch->in_room;
    for (int i = 0; i < 20; i++) {
      if (desti->sector_type == SECT_UNDERWATER && desti->exit[DIR_UP] != NULL && desti->exit[DIR_UP]->u1.to_room != NULL && open_sound(desti, DIR_UP)) {
        desti = desti->exit[DIR_UP]->u1.to_room;
      }
    }
    if (desti != ch->in_room) {
      act("$n swims up desperately.", ch, NULL, NULL, TO_ROOM);
      dact("$n swims up desperately.", ch, NULL, NULL, DISTANCE_MEDIUM);
      char_from_room(ch);
      char_to_room(ch, desti);
      act("$n surfaces.", ch, NULL, NULL, TO_ROOM);
      dact("$n surfaces.", ch, NULL, NULL, DISTANCE_MEDIUM);
    }
  }

  bool has_moved(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (ch->fight_fast == TRUE) {
      if (ch->attack_timer > 0)
      return TRUE;
    }
    else {
      if (ch->fight_fast == FALSE && ch->fight_current != ch) {
        return TRUE;
      }
      if (ch->fight_fast == FALSE && ch->moving == TRUE) {
        return TRUE;
      }
      if (ch->fight_fast == FALSE && IS_FLAG(ch->fightflag, FIGHT_NOMOVE)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_spar) {
    if (IS_FLAG(ch->comm, COMM_SPARRING)) {
      send_to_char("You stop pulling your blows.\n\r", ch);
      act("$n stops pulling $s blows.", ch, NULL, NULL, TO_ROOM);
      dact("$n stops pulling $s blows.", ch, NULL, NULL, DISTANCE_MEDIUM);
      REMOVE_FLAG(ch->comm, COMM_SPARRING);
      return;
    }
    else {
      send_to_char("You start pulling your blows.\n\r", ch);
      act("$n starts pulling $s blows.", ch, NULL, NULL, TO_ROOM);
      dact("$n starts pulling $s blows.", ch, NULL, NULL, DISTANCE_MEDIUM);
      SET_FLAG(ch->comm, COMM_SPARRING);
      return;
    }
  }

  bool no_deep_access(ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to) {
    CHAR_DATA *newvict;
    int mindist = 1000;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;

      if (!IS_FLAG(newvict->act, PLR_DEEPSHROUD))
      continue;

      if (newvict->in_room == NULL || newvict->in_room == from)
      continue;

      int dist = get_dist(to->x, to->y, newvict->in_room->x, newvict->in_room->y);
      if (dist < mindist)
      mindist = dist;
    }
    if (mindist > 5 && mindist <= 7)
    return TRUE;

    return FALSE;
  }

  bool nearby_deep(ROOM_INDEX_DATA *room) {
    CHAR_DATA *newvict;
    int mindist = 1000;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      newvict = *it;

      if (newvict == NULL || is_gm(newvict))
      continue;

      if (!IS_FLAG(newvict->act, PLR_DEEPSHROUD))
      continue;

      if (newvict->in_room == NULL)
      continue;

      int dist =
      get_dist(room->x, room->y, newvict->in_room->x, newvict->in_room->y);
      if (dist < mindist)
      mindist = dist;
    }
    if (mindist < 6)
    return TRUE;
    return FALSE;
  }

  bool forest_monster(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return FALSE;

    if (ch->pIndexData->vnum >= 10 && ch->pIndexData->vnum <= 50)
    return TRUE;

    return FALSE;
  }

  CHAR_DATA *find_prey(CHAR_DATA *mob) {
    int mindist = 10;
    CHAR_DATA *prey = NULL;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL)
        continue;

        if (in_public(mob, to) && !IS_FLAG(to->act, PLR_SHROUD))
        continue;
        if (IS_FLAG(to->act, PLR_HIDE)) {
          if (forest_monster(mob) && IS_FLAG(to->act, PLR_HIDE)) {
            if (get_skill(to, SKILL_STEALTH) * 40 > number_percent() || number_percent() % 2 == 0)
            continue;
          }
        }

        if (forest_monster(mob) && to->in_room != NULL && mist_level(to->in_room) < 3 && (to->in_room->area->vnum == INNER_NORTH_FOREST || to->in_room->area->vnum == INNER_SOUTH_FOREST || to->in_room->area->vnum == INNER_WEST_FOREST || to->in_room->area->vnum == HAVEN_TOWN_VNUM))
        continue;

        if (forest_monster(mob) && (in_prop(to) || to->in_room->sector_type != SECT_FOREST))
        continue;

        if (IS_FLAG(to->act, PLR_DEEPSHROUD))
        continue;

        if (is_helpless(to))
        continue;

        if (is_gm(to))
        continue;

        if (in_fight(to) && get_dist(to->in_room->x, to->in_room->y, mob->in_room->x, mob->in_room->y) > 4)
        continue;

        if (IS_FLAG(to->comm, COMM_AFK))
        continue;

        if (!can_get_to(mob, to->in_room))
        continue;

        if (get_dist(to->in_room->x, to->in_room->y, mob->in_room->x, mob->in_room->y) < mindist) {
          prey = to;
          mindist = get_dist(to->in_room->x, to->in_room->y, mob->in_room->x, mob->in_room->y);
        }
      }
    }
    if (mindist <= 8) {
      if (forest_monster(mob) && IS_FLAG(prey->act, PLR_SHROUD) && !IS_FLAG(mob->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);
      if (forest_monster(mob) && !IS_FLAG(prey->act, PLR_SHROUD) && IS_FLAG(mob->act, PLR_SHROUD))
      REMOVE_FLAG(mob->act, PLR_SHROUD);
      return prey;
    }
    return NULL;
  }

  CHAR_DATA *find_abductee(CHAR_DATA *mob) {
    int mindist = 10;
    CHAR_DATA *prey = NULL;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL)
        continue;

        if (forest_monster(mob) && in_public(mob, to) && !IS_FLAG(to->act, PLR_SHROUD))
        continue;

        if (forest_monster(mob) && to->in_room != NULL && mist_level(to->in_room) < 3 && (to->in_room->area->vnum == INNER_NORTH_FOREST || to->in_room->area->vnum == INNER_SOUTH_FOREST || to->in_room->area->vnum == INNER_WEST_FOREST || to->in_room->area->vnum == HAVEN_TOWN_VNUM))
        continue;

        if (forest_monster(mob) && (in_prop(to) || to->in_room->sector_type != SECT_FOREST))
        continue;

        if (forest_monster(mob) && (!is_helpless(to) || to->pcdata->monster_beaten == 0))
        continue;

        if (is_gm(to))
        continue;

        if (is_invader(mob) && (to->wounds >= 3 || !is_helpless(to)))
        continue;

        if (to->played / 3600 < 50)
        continue;
        if (pc_in_lair(to))
        continue;

        if (room_fight(to->in_room, FALSE, FALSE, TRUE))
        continue;

        if (get_dist(to->in_room->x, to->in_room->y, mob->in_room->x, mob->in_room->y) < mindist) {
          prey = to;
          mindist = get_dist(to->in_room->x, to->in_room->y, mob->in_room->x, mob->in_room->y);
        }
      }
    }
    if (mindist <= 5) {
      if (forest_monster(mob) && IS_FLAG(prey->act, PLR_SHROUD) && !IS_FLAG(mob->act, PLR_SHROUD))
      SET_FLAG(mob->act, PLR_SHROUD);
      if (forest_monster(mob) && !IS_FLAG(prey->act, PLR_SHROUD) && IS_FLAG(mob->act, PLR_SHROUD))
      REMOVE_FLAG(mob->act, PLR_SHROUD);
      return prey;
    }
    return NULL;
  }
  _DOFUN(do_suicide) {
    char arg1[MSL];
    if (is_roster_char(ch)) {
      send_to_char("Roster characters cannot commit suicide.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if (is_helpless(ch) || in_fight(ch) || is_pinned(ch)) {
      send_to_char("You can't do that at the moment.\n\r", ch);
      return;
    }
    if (in_lodge(ch->in_room)) {
      send_to_char("You can't bring yourself to do that in here.\n\r", ch);
      return;
    }
    if (is_ghost(ch) || IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("It's a bit late for that.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "fake")) {
      if (number_percent() % 10 == 0) {
        free_string(ch->pcdata->deathcause);
        ch->pcdata->deathcause = str_dup("apparent suicide.");

        act("You accidentally kill yourself.", ch, NULL, NULL, TO_CHAR);
        act("$n kills $mself.", ch, NULL, NULL, TO_ROOM);
        dact("$n kills $mself.", ch, NULL, NULL, DISTANCE_NEAR);
        real_kill(ch, ch);
        ch->spentexp /= 2;
        ch->spentrpexp /= 2;
        return;
      }
      if (number_percent() % 9 == 0) {
        act("You attempt to kill yourself and leave yourself critically wounded.", ch, NULL, NULL, TO_CHAR);
        act("$n attempts to kill $mself but fails leaving $m critically wounded.", ch, NULL, NULL, TO_ROOM);
        dact("$n attempts to kill $mself but fails leaving $m critically wounded.", ch, NULL, NULL, DISTANCE_NEAR);
        wound_char(ch, 3);
        return;
      }
      if (number_percent() % 2 == 0) {
        act("You attempt to kill yourself.", ch, NULL, NULL, TO_CHAR);
        act("$n attempts to kill $mself but fails.", ch, NULL, NULL, TO_ROOM);
        dact("$n attempts to kill $mself but fails.", ch, NULL, NULL, DISTANCE_NEAR);
        wound_char(ch, 2);
      }
      act("You attempt to kill yourself.", ch, NULL, NULL, TO_CHAR);
      act("$n attempts to kill $mself but fails.", ch, NULL, NULL, TO_ROOM);
      dact("$n attempts to kill $mself but fails.", ch, NULL, NULL, DISTANCE_NEAR);
      wound_char(ch, 1);
      return;
    }
    if (!str_cmp(arg1, "forreal")) {
      if (under_understanding(ch, ch)) {
        send_to_char("Your body refuses to do that.\n\r", ch);
        return;
      }
      if (number_percent() % 2 == 0) {
        act("You attempt to kill yourself but fail leaving you critically wounded.", ch, NULL, NULL, TO_CHAR);
        act("$n attempts to kill $mself but fails leaving $m critically wounded.", ch, NULL, NULL, TO_ROOM);
        dact("$n attempts to kill $mself but fails leaving $m critically wounded.", ch, NULL, NULL, DISTANCE_NEAR);
        wound_char(ch, 3);
        return;
      }
      if (number_percent() % 10 == 0) {
        act("You attempt to kill yourself but fail.", ch, NULL, NULL, TO_CHAR);
        act("$n attempts to kill $mself but fails.", ch, NULL, NULL, TO_ROOM);
        dact("$n attempts to kill $mself but fails.", ch, NULL, NULL, DISTANCE_NEAR);
        wound_char(ch, 2);
        return;
      }
      free_string(ch->pcdata->deathcause);
      ch->pcdata->deathcause = str_dup("apparent suicide.");

      act("You kill yourself.", ch, NULL, NULL, TO_CHAR);
      act("$n kills $mself.", ch, NULL, NULL, TO_ROOM);
      dact("$n kills $mself.", ch, NULL, NULL, DISTANCE_NEAR);
      real_kill(ch, ch);
      ch->spentexp /= 2;
      ch->spentrpexp /= 2;
      return;
    }
    send_to_char("Syntax: Suicide fake/forreal\n\r", ch);
  }

  void combat_damage(CHAR_DATA *victim, CHAR_DATA *ch, int amount, int disc) {
    int dam = amount;
    if (is_animal(victim) && get_animal_genus(victim, ANIMAL_ACTIVE) == GENUS_SWARM && get_skill(victim, SKILL_SWARMSHIFTING) > 0) {
      dam = UMIN(dam, victim->hit / swarm_count(victim, ANIMAL_ACTIVE));
    }

    if ((!IS_NPC(ch) || combat_distance(ch, victim, FALSE) > 5) && !IS_NPC(victim) && pvp_target(ch, victim) && get_skill(victim, SKILL_IMMORTALITY) > 0 && dam > victim->hit - 1 && disc != DIS_LONGBLADE) {
      dam = UMAX(0, victim->hit - 1);
    }

    if (IS_FLAG(ch->fightflag, FIGHT_WOUND)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_WOUND);
      if (victim->hit <= 0)
      combat_damage(victim, ch, 1, disc);
    }
    if (victim->in_room->sector_type == SECT_AIR && victim->debuff >= 75)
    fall_character(victim);

    if (IS_FLAG(ch->fightflag, FIGHT_AOE)) {
      REMOVE_FLAG(ch->fightflag, FIGHT_AOE);
      for (CharList::iterator it = char_list.begin(); it != char_list.end();
      ++it) {
        CHAR_DATA *rch = *it;

        if (rch == NULL || is_gm(rch))
        continue;
        if (!in_fight(rch))
        continue;
        if (!same_fight(rch, victim))
        continue;

        if (rch == ch || rch == victim)
        continue;

        if (combat_distance(rch, victim, FALSE) > 10)
        continue;

        if (is_in_cover(rch)) {
          combat_damage(rch, ch, amount / 4, disc);
        }
        else {
          combat_damage(rch, ch, amount, disc);
        }
        send_to_char("`rYou are hit by the aoe.`x", rch);
      }
    }
    damage(victim, ch, dam);
  }

  bool invisioncone_running(CHAR_DATA *ch, CHAR_DATA *victim) {
    bool result;
    int orig = ch->facing;
    ch->facing = ch->run_dir;
    result = invisioncone_character(ch, victim);
    ch->facing = orig;
    return result;
  }

  bool behindyou_running(CHAR_DATA *ch, CHAR_DATA *victim) {
    bool result;
    int orig = ch->facing;
    ch->facing = rev_dir[ch->run_dir];
    result = invisioncone_character(ch, victim);
    ch->facing = orig;
    return result;
  }

  bool is_vuln(CHAR_DATA *ch, int type) {
    if (type == AMMO_SILVER && is_werewolf(ch)) {
      return TRUE;
    }
    if (type == AMMO_WOOD && is_vampire(ch)) {
      return TRUE;
    }
    if (type == AMMO_IRON && is_faeborn(ch)) {
      return TRUE;
    }
    if (type == AMMO_QUARTZ && is_demonborn(ch)) {
      return TRUE;
    }
    if (type == AMMO_OBSIDIAN && is_angelborn(ch)) {
      return TRUE;
    }
    if (type == AMMO_RADIOACTIVE && is_demigod(ch)) {
      return TRUE;
    }
    if (type == AMMO_SILVER && get_skill(ch, SKILL_SILVERVULN) > 0)
    return TRUE;
    if (type == AMMO_GOLD && get_skill(ch, SKILL_GOLDVULN) > 0)
    return TRUE;
    if (type == AMMO_BONE && get_skill(ch, SKILL_BONEVULN) > 0)
    return TRUE;
    if (type == AMMO_WOOD && get_skill(ch, SKILL_WOODVULN) > 0)
    return TRUE;

    if (type == AMMO_PARALYTIC)
    return TRUE;
    if (type == AMMO_POISON)
    return TRUE;

    return FALSE;
  }

  int damage_echo(CHAR_DATA *ch, CHAR_DATA *victim, int realdisc, int disc, int dam, int range, bool damdone) {
    bool hasshield = FALSE;
    bool hasarmor = FALSE;
    bool supershield = FALSE;
    bool supertough = FALSE;

    if (is_cover(victim))
    return ECHO_COVERSTRIKE;

    if ((get_disc(victim, DIS_BARMOR, FALSE) > 2 || get_disc(victim, DIS_MARMOR, FALSE) > 2) && wearing_armor(victim))
    hasarmor = TRUE;

    if ((get_disc(victim, DIS_BSHIELD, FALSE) > 2 || get_disc(victim, DIS_MSHIELD, FALSE) > 2) && has_shield(victim))
    hasshield = TRUE;

    if (get_disc(victim, DIS_FORCES, FALSE) > 2 || get_disc(victim, DIS_FATE, FALSE) > 2 || get_disc(victim, DIS_PUSH, FALSE) > 2)
    supershield = TRUE;

    if (get_disc(victim, DIS_UNDEAD, FALSE) > 2 || get_disc(victim, DIS_BONES, FALSE) > 2 || get_disc(victim, DIS_TOUGHNESS, FALSE) > 2 || get_disc(victim, DIS_NARMOR, FALSE) > 2)
    supertough = TRUE;

    if (get_disc(victim, DIS_ICES, FALSE) > 2 || get_disc(victim, DIS_FIRES, FALSE) > 2 || get_disc(victim, DIS_ENERGYS, FALSE) > 2 || get_disc(victim, DIS_LIGHTNINGS, FALSE) > 2 || get_disc(victim, DIS_DARKS, FALSE) > 2 || get_disc(victim, DIS_WOODS, FALSE) > 2 || get_disc(victim, DIS_MENTALS, FALSE) > 2 || get_disc(victim, DIS_STONES, FALSE) > 2)
    supershield = TRUE;

    if (guestmonster(victim) || cardinal(victim) || higher_power(victim)) {
      if (victim->damage_absorb + dam >= monster_round_cap(ch, victim) && victim->debuff + victim->to_debuff >= 100)
      return ECHO_SUPERTOUGH;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim) && get_skill(victim, SKILL_IMMORTALITY) > 0 && dam > victim->hit - 1 && realdisc != DIS_LONGBLADE) {
      if (victim->debuff + victim->to_debuff >= 100)
      return ECHO_SUPERTOUGH;
    }

    if (pvp_target(ch, victim) && (is_super(victim) || get_skill(victim, SKILL_IMMORTALITY) > 0) && victim->shape == SHAPE_HUMAN) {
      if (realdisc != DIS_LONGBLADE && realdisc != DIS_BRUTE && realdisc != DIS_HELLFIRE && realdisc != DIS_BLUNT && realdisc != DIS_SHOTGUNS && no_incindiary(ch, realdisc, FALSE) && (realdisc != DIS_CLAW || ch->shape != SHAPE_WOLF || !is_werewolf(ch))) {
        if (victim->damage_absorb + dam >= round_cap(ch, victim) && victim->debuff + victim->to_debuff >= 100)
        return ECHO_SUPERTOUGH;
      }
    }

    int hitperc;
    if (damdone == TRUE)
    hitperc = (victim->hit) * 100 / max_hp(victim);
    else
    hitperc = (victim->hit - dam) * 100 / max_hp(victim);

    if (realdisc == DIS_RIFLES || realdisc == DIS_CARBINES || realdisc == DIS_BOWS || realdisc == DIS_SHOTGUNS || realdisc == DIS_SPEARGUN) {
      if (get_lranged(ch) != NULL && get_lranged(ch)->buff > 0 && !IS_SET(get_lranged(ch)->extra_flags, ITEM_AMMOCHANGED) && get_lranged(ch)->value[2] != AMMO_ARMOR && get_lranged(ch)->value[2] != AMMO_AUGMENTED && is_vuln(victim, get_lranged(ch)->value[2])) {
        if (supertough && !hasarmor && !hasshield && hitperc < 75 && victim->wound_location % 2 == 0)
        return ECHO_SUPERTOUGH;
        else
        return ECHO_GRAZE;
      }
    }
    if (realdisc == DIS_PISTOLS) {
      if (get_sranged(ch) != NULL && get_sranged(ch)->buff > 0 && !IS_SET(get_sranged(ch)->extra_flags, ITEM_AMMOCHANGED) && get_sranged(ch)->value[2] != AMMO_ARMOR && get_sranged(ch)->value[2] != AMMO_AUGMENTED && is_vuln(victim, get_sranged(ch)->value[2])) {
        if (supertough && !hasarmor && !hasshield && hitperc < 75 && victim->wound_location % 2 == 0)
        return ECHO_SUPERTOUGH;
        else
        return ECHO_GRAZE;
      }
    }
    if (realdisc == DIS_KNIFE || realdisc == DIS_THROWN) {
      if (get_smelee(ch) != NULL && get_smelee(ch)->buff > 0 && get_smelee(ch)->value[2] != AMMO_ARMOR && get_smelee(ch)->value[2] != AMMO_AUGMENTED && is_vuln(victim, get_smelee(ch)->value[2])) {
        if (supertough && !hasarmor && !hasshield && hitperc < 75 && victim->wound_location % 2 == 0)
        return ECHO_SUPERTOUGH;
        else
        return ECHO_GRAZE;
      }
    }
    if (realdisc == DIS_LONGBLADE || realdisc == DIS_BLUNT || realdisc == DIS_SPEAR) {
      if (get_lmelee(ch) != NULL && get_lmelee(ch)->buff > 0 && get_lmelee(ch)->value[2] != AMMO_ARMOR && get_lmelee(ch)->value[2] != AMMO_AUGMENTED && is_vuln(victim, get_lmelee(ch)->value[2])) {
        if (supertough && !hasarmor && !hasshield && hitperc < 75 && victim->wound_location % 2 == 0)
        return ECHO_SUPERTOUGH;
        else
        return ECHO_GRAZE;
      }
    }

    if (hitperc >= 40) {
      if (disc == DIS_STRIKING)
      return ECHO_PUNCHBLOCK;
      if (disc == DIS_GRAPPLE)
      return ECHO_GRAPPLEBLOCK;
      if (range > 1) {
        if (is_in_cover(victim))
        return ECHO_COVER;
        else
        return ECHO_MISS;
      }
      else {
        if (hasshield && victim->wound_location % 2 == 0)
        return ECHO_SHIELDBLOCK;
        else {
          if (hasshield && number_percent() % 2 == 0)
          return ECHO_SHIELDBLOCK;
          else if ((get_lmelee(victim) != NULL || get_smelee(victim) != NULL || IS_NPC(victim)) && disc != DIS_CLAW && disc != DIS_BRUTE && disc != DIS_ENERGYF && disc != DIS_DARKF && disc != DIS_ICEF && disc != DIS_FIREF)
          return ECHO_PARRY;
          else
          return ECHO_DODGE;
        }
      }
    }
    if (disc == DIS_STRIKING)
    return ECHO_PUNCH;
    if (disc == DIS_GRAPPLE)
    return ECHO_GRAPPLE;

    if (hasshield && hasarmor && supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("1", victim);
      if (hitperc >= 30)
      return ECHO_SHIELD;
      else if (hitperc >= 20)
      return ECHO_SUPERSHIELD;
      else if (hitperc >= 10)
      return ECHO_ARMOR;
      else
      return ECHO_SUPERTOUGH;
    }
    if (!hasshield && hasarmor && supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("2", victim);

      if (hitperc >= 27)
      return ECHO_SUPERSHIELD;
      else if (hitperc >= 14)
      return ECHO_ARMOR;
      else
      return ECHO_SUPERTOUGH;
    }
    if (hasshield && !hasarmor && supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("3", victim);

      if (hitperc >= 27)
      return ECHO_SHIELD;
      else if (hitperc >= 14)
      return ECHO_SUPERSHIELD;
      else
      return ECHO_SUPERTOUGH;
    }
    if (hasshield && hasarmor && !supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("4", victim);

      if (hitperc >= 27)
      return ECHO_SHIELD;
      else if (hitperc >= 14)
      return ECHO_SUPERSHIELD;
      else
      return ECHO_ARMOR;
    }
    if (!hasshield && hasarmor && supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("5", victim);

      if (hitperc >= 27)
      return ECHO_SHIELD;
      else if (hitperc >= 14)
      return ECHO_ARMOR;
      else
      return ECHO_SUPERTOUGH;
    }
    if (!hasshield && !hasarmor && supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("6", victim);

      if (hitperc >= 20)
      return ECHO_SUPERSHIELD;
      else
      return ECHO_SUPERTOUGH;
    }
    if (!hasshield && hasarmor && !supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("7", victim);

      if (hitperc >= 20)
      return ECHO_SUPERSHIELD;
      else
      return ECHO_ARMOR;
    }
    if (!hasshield && hasarmor && supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("8", victim);

      if (hitperc >= 20)
      return ECHO_ARMOR;
      else
      return ECHO_SUPERTOUGH;
    }
    if (hasshield && !hasarmor && !supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("9", victim);

      if (hitperc >= 20)
      return ECHO_SHIELD;
      else
      return ECHO_SUPERSHIELD;
    }
    if (hasshield && !hasarmor && supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("10", victim);

      if (hitperc >= 20)
      return ECHO_SHIELD;
      else
      return ECHO_SUPERTOUGH;
    }
    if (hasshield && hasarmor && !supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("11", victim);

      if (hitperc >= 20)
      return ECHO_SHIELD;
      else
      return ECHO_ARMOR;
    }
    if (hasshield && !hasarmor && !supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("12", victim);

      if (hitperc >= 20)
      return ECHO_SHIELD;
      else
      return ECHO_SHIELDBRUISE;
    }
    if (!hasshield && hasarmor && !supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("13", victim);

      if (hitperc >= 20)
      return ECHO_ARMOR;
      else
      return ECHO_ARMORBRUISE;
    }
    if (!hasshield && !hasarmor && supertough && !supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("14", victim);

      return ECHO_SUPERTOUGH;
    }
    if (!hasshield && !hasarmor && !supertough && supershield) {
      if (!str_cmp(victim->name, "Monkey"))
      send_to_char("15", victim);

      return ECHO_SUPERSHIELD;
    }
    if (!str_cmp(victim->name, "Monkey"))
    send_to_char("16", victim);

    return ECHO_GRAZE;
  }

  bool monster_fight(CHAR_DATA *ch) {
    if (!in_fight(ch))
    return FALSE;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      CHAR_DATA *rch = *it;

      if (rch == NULL || is_gm(rch))
      continue;
      if (IS_NPC(rch))
      continue;
      if (!in_fight(rch))
      continue;
      if (!same_fight(rch, ch))
      continue;
      if (guestmonster(rch))
      return TRUE;
    }
    return FALSE;
  }

  bool pvp_target(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) && IS_NPC(victim))
    return FALSE;
    if (IS_NPC(victim))
    return FALSE;

    if (IS_NPC(ch) && (ch->pIndexData->vnum == MINION_TEMPLATE || ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == HAND_SOLDIER))
    return TRUE;

    if (!IS_NPC(victim))
    return TRUE;

    return FALSE;
  }

  bool pvp_character(CHAR_DATA *ch) {
    if (!IS_NPC(ch))
    return TRUE;

    if (IS_NPC(ch) && (ch->pIndexData->vnum == MINION_TEMPLATE || ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == HAND_SOLDIER))
    return TRUE;

    return FALSE;
  }


  bool overwhelm(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    if(IS_NPC(ch) || IS_NPC(victim))
    return FALSE;
    if(is_gm(victim) || higher_power(victim) || guestmonster(victim))
    return FALSE;

    if(ch->pcdata->boon == BOON_STRENGTH && ch->pcdata->boon_timeout > current_time)
    {
      if(victim->pcdata->boon == BOON_STRENGTH && victim->pcdata->boon_timeout > current_time)
      {
        return FALSE;
      }
      else
      return TRUE;
    }

    if(victim->pcdata->curse == CURSE_WEAKNESS && ch->pcdata->curse_timeout > current_time)
    {
      if(ch->pcdata->curse == CURSE_WEAKNESS && ch->pcdata->curse_timeout > current_time)
      {
        return FALSE;
      }
      else
      return TRUE;
    }
    if(college_student(ch, FALSE) && college_student(victim, FALSE))
    {
      if(college_group(ch, FALSE) == COLLEGE_JOCK && college_group(victim, FALSE) != COLLEGE_JOCK)
      return TRUE;
    }


    return FALSE;

  }

#if defined(__cplusplus)
}
#endif

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
#include "tables.h"

#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif





  /*
* Local functions.
*/
#define CD CHAR_DATA
#define OD OBJ_DATA

  void zip_person args((CHAR_DATA * ch, char *argument));
  bool remove_obj args((CHAR_DATA * ch, int iWear, bool fReplace));
  void wear_obj args((CHAR_DATA * ch, OBJ_DATA *obj, bool fReplace, bool silent));
  int get_cost args((CHAR_DATA * keeper, OBJ_DATA *obj, bool fBuy));
  void obj_to_keeper args((OBJ_DATA * obj, CHAR_DATA *ch));
  OD *get_obj_keeper args((CHAR_DATA * ch, CHAR_DATA *keeper, char *argument));
  void do_dual args((CHAR_DATA * ch, char *argument));
  bool has_stash args((ROOM_INDEX_DATA * room));
  void clone_obj args((CHAR_DATA * ch, OBJ_DATA *base, OBJ_DATA *poi, bool colour));

#undef OD
#undef CD

  /* RT part of the corpse looting code */

  bool is_corporeal(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;
    if (is_ghost(ch))
    return FALSE;
    if (ch->pcdata->spectre > 0)
    return FALSE;
    return TRUE;
  }

  bool can_loot(CHAR_DATA *ch, OBJ_DATA *obj) {
    CHAR_DATA *owner = NULL;

    if (IS_IMMORTAL(ch))
    return TRUE;

    if (!obj->owner || obj->owner == NULL)
    return TRUE;

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it)
    if (!str_cmp((*it)->name, obj->owner))
    owner = *it;

    if (owner == NULL)
    return TRUE;

    if (!str_cmp(ch->name, owner->name))
    return TRUE;

    if (is_same_group(ch, owner))
    return TRUE;

    return FALSE;
  }

  void get_obj_load(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container) {
    /* variables for AUTOSPLIT */
    OBJ_DATA *pocketobj;
    bool hasjacket = FALSE;
    bool haspants = FALSE;

    if (!CAN_WEAR(obj, ITEM_TAKE)) {
      send_to_char("You can't take that.\n\r", ch);
      return;
    }

    if (!IS_SET(obj->extra_flags, ITEM_LARGE)) {
      send_to_char("That is too light to lift, you'll have to use the get command.\n\r", ch);
      return;
    }

    if (!can_loot(ch, obj)) {
      act("Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR);
      return;
    }

    if (obj->in_room != NULL) {
      for (CharList::iterator it = obj->in_room->people->begin();
      it != obj->in_room->people->end(); ++it) {
        if ((*it)->on == obj) {
          act("$N appears to be using $p.", ch, obj, *it, TO_CHAR);
          return;
        }
      }
    }

    if (container != NULL) {
      if (container->pIndexData->vnum == OBJ_VNUM_PIT && get_trust(ch) < obj->level) {
        send_to_char("You are not powerful enough to use it.\n\r", ch);
        return;
      }

      // added check so free_string only runs if needed - Discordance
      if (obj->adjust_string != NULL && safe_strlen(obj->adjust_string) >= 1) {
        free_string(obj->adjust_string);
      }

      obj->adjust_string = str_dup("");

      if (obj->pIndexData->vnum == 36 && obj->size >= 50) {
        WAIT_STATE(ch, PULSE_PER_SECOND * 20);
      }

      if (obj->size > 20 || (!crowded_room(ch->in_room) && !in_fight(ch)))
      act("$n gets $a $p from $P.", ch, obj, container, TO_ROOM);
      obj_from_obj(obj);

      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && !is_ghost(ch)) {
        if (get_eq_char(ch, WEAR_HOLD) == NULL || get_eq_char(ch, WEAR_HOLD_2) == NULL) {
          obj_to_char(obj, ch);
          wear_obj(ch, obj, TRUE, FALSE);
          return;
        }
        else {
          send_to_char("Your hands are full.\n\r", ch);
          obj_to_room(obj, ch->in_room);
          return;
        }
      }
      else if (CAN_WEAR(obj, ITEM_WEAR_HOLD) && !is_ghost(ch)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD);
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD_2);
      }
      if (is_big(obj)) {
        if (get_big_items(ch) + obj->size > MAX_BIG_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that on your back, so you hold it.\n\r", ch);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that on your back.\n\r", ch);
            obj_to_room(obj, ch->in_room);
            return;
          }
        }
      }
      else {
        if (get_small_items(ch) + obj->size > MAX_SMALL_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that in your pockets, so you hold it.\n\r", ch);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that in your pockets.\n\r", ch);
            obj_to_room(obj, ch->in_room);
            return;
          }
        }
      }

    }
    else {
      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && !is_ghost(ch)) {
        if (get_eq_char(ch, WEAR_HOLD) == NULL || get_eq_char(ch, WEAR_HOLD_2) == NULL) {
          obj_from_room(obj);
          obj_to_char(obj, ch);
          wear_obj(ch, obj, TRUE, FALSE);
          return;
        }
        else {
          send_to_char("Your hands are full.\n\r", ch);
          return;
        }
      }
      else if (CAN_WEAR(obj, ITEM_WEAR_HOLD) && !is_ghost(ch)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD);
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD_2);
      }
      if (is_big(obj)) {
        if (get_big_items(ch) + obj->size > MAX_BIG_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that on your back, so you hold it.\n\r", ch);
            obj_from_room(obj);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that on your back.\n\r", ch);
            return;
          }
        }
      }
      else {
        if (get_small_items(ch) + obj->size > MAX_SMALL_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that in your pockets, so you hold it.\n\r", ch);
            obj_from_room(obj);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that in your pockets.\n\r", ch);
            return;
          }
        }
      }

      pocketobj = get_jacket(ch);
      if (pocketobj == NULL || pocketobj->item_type != ITEM_CLOTHING) {
        pocketobj = get_pants(ch);
        if (pocketobj != NULL && pocketobj->item_type == ITEM_CLOTHING)
        haspants = TRUE;
      }
      else
      hasjacket = TRUE;
      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) || CAN_WEAR(obj, ITEM_WEAR_HOLD)) {
        act("You get $a $p and hold it.", ch, obj, container, TO_CHAR);
        act("$n gets $a $p and holds it.", ch, obj, container, TO_ROOM);
      }
      else if (is_big(obj)) {
        if (obj->item_type == ITEM_CLOTHING) {
          act("You get $a $p and throw it over your shoulder.", ch, obj, container, TO_CHAR);
          act("$n gets $a $p and throws it over $s shoulder.", ch, obj, container, TO_ROOM);
        }
        else if (obj->item_type == ITEM_WEAPON && obj->value[0] == WEAPON_SWORD) {
          act("You get $a $p and sheath it on your back.", ch, obj, NULL, TO_CHAR);
          act("$n gets $a $p and sheathes it on $s back.", ch, obj, NULL, TO_ROOM);
        }
        else {
          act("You get $a $p and strap it on your back.", ch, obj, container, TO_CHAR);
          act("$n gets $a $p and straps it onto $s back.", ch, obj, container, TO_ROOM);
        }
      }
      else {
        if (obj->item_type == ITEM_WEAPON) {
          act("You get $a $p and slip it into a hidden sheath.", ch, obj, NULL, TO_CHAR);
          act("$n gets $a $p and slips it into a hidden sheath.", ch, obj, NULL, TO_ROOM);
        }
        else if (hasjacket) {
          act("You get $a $p and slip it into $a $P pocket.", ch, obj, pocketobj, TO_CHAR);
          act("$n gets $a $p and slips it into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
        }
        else if (haspants) {
          act("You get $a $p and slip it into $a $P pocket.", ch, obj, pocketobj, TO_CHAR);
          act("$n gets $a $p and slips it into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
        }
        else {
          act("You get $a $p and slip it into a pocket.", ch, obj, NULL, TO_CHAR);
          act("$n gets $a $p and slips it into a pocket.", ch, obj, NULL, TO_ROOM);
        }
      }
      if (obj->in_room != NULL)
      obj_from_room(obj);
    }

    if (obj->item_type == ITEM_MONEY) {
      if (is_ghost(ch))
      return;

      char buf[MSL];
      sprintf(buf, "CASH: %s picks up %d at %d.\n\r", ch->name, obj->value[1], ch->in_room->vnum);
      log_string(buf);

      if (valid_money(obj, ch))
      ch->money += obj->value[1];

      extract_obj(obj);
    }
    else {
      obj_to_char(obj, ch);
    }

    return;
  }

  void get_obj(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container) {
    /* variables for AUTOSPLIT */
    OBJ_DATA *pocketobj;
    bool hasjacket = FALSE;
    bool haspants = FALSE;

    if (!CAN_WEAR(obj, ITEM_TAKE) && container == NULL) {
      send_to_char("You can't take that.\n\r", ch);
      return;
    }

    if (IS_SET(obj->extra_flags, ITEM_LARGE) && container == NULL) {
      send_to_char("That is too heavy to pick up, you'll have to use the lift command.\n\r", ch);
      return;
    }

    /*
    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
    &&  (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)))
    {
      act( "$d: you can't carry that much weight.", ch, NULL, obj->name, TO_CHAR );
      return;
    }
    */
    if (!can_loot(ch, obj)) {
      act("Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR);
      return;
    }

    // Anti griefer stealing items - Disco 7/30/2020
    if (obj->in_room != NULL || (obj->in_obj != NULL && obj->in_obj->carried_by != ch)) {
      if (is_griefer(ch)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
    }

    if (obj->in_room != NULL) {
      for (CharList::iterator it = obj->in_room->people->begin();
      it != obj->in_room->people->end(); ++it) {
        if ((*it)->on == obj) {
          act("$N appears to be using $p.", ch, obj, *it, TO_CHAR);
          return;
        }
      }
    }

    if (container != NULL) {
      if (container->pIndexData->vnum == OBJ_VNUM_PIT && get_trust(ch) < obj->level) {
        send_to_char("You are not powerful enough to use it.\n\r", ch);
        return;
      }

      //        added check so free_string only runs if needed - Discordance
      if (obj->adjust_string && obj->adjust_string != NULL && safe_strlen(obj->adjust_string) >= 1) {
        free_string(obj->adjust_string);
      }
      //        This line was commented out along with free_string.  Suspicious.
      //        WATCH THIS.
      obj->adjust_string = str_dup("");

      if (obj->pIndexData->vnum == 36 && obj->size >= 50) {
        WAIT_STATE(ch, PULSE_PER_SECOND * 20);
      }

      if (container->carried_by == NULL || container->pIndexData->vnum != 35) {
        act("You get $a $p from $P.", ch, obj, container, TO_CHAR);
        act("$n gets $a $p from $P.", ch, obj, container, TO_ROOM);
      }
      obj_from_obj(obj);

      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && !is_ghost(ch)) {
        if (get_eq_char(ch, WEAR_HOLD) == NULL || get_eq_char(ch, WEAR_HOLD_2) == NULL) {
          obj_to_char(obj, ch);
          wear_obj(ch, obj, TRUE, FALSE);
          return;
        }
        else {
          send_to_char("Your hands are full.\n\r", ch);
          obj_to_room(obj, ch->in_room);
          return;
        }
      }
      else if (CAN_WEAR(obj, ITEM_WEAR_HOLD) && !is_ghost(ch)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD);
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD_2);
      }

      if (is_big(obj)) {
        if (get_big_items(ch) + obj->size > MAX_BIG_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that on your back, so you hold it.\n\r", ch);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that on your back.\n\r", ch);
            obj_to_room(obj, ch->in_room);
            return;
          }
        }
      }
      else {
        if (get_small_items(ch) + obj->size > MAX_SMALL_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that in your pockets, so you hold it.\n\r", ch);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that in your pockets.\n\r", ch);
            obj_to_room(obj, ch->in_room);
            return;
          }
        }
      }

    }
    else {
      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && !is_ghost(ch)) {
        if (get_eq_char(ch, WEAR_HOLD) == NULL || get_eq_char(ch, WEAR_HOLD_2) == NULL) {
          obj_from_room(obj);
          obj_to_char(obj, ch);
          wear_obj(ch, obj, TRUE, FALSE);
          return;
        }
        else {
          send_to_char("Your hands are full.\n\r", ch);
          return;
        }
      }
      else if (CAN_WEAR(obj, ITEM_WEAR_HOLD) && !is_ghost(ch)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD);
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
        equip_char_silent(ch, obj, WEAR_HOLD_2);
      }

      if (is_big(obj)) {
        if (get_big_items(ch) + obj->size > MAX_BIG_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that on your back, so you hold it.\n\r", ch);
            obj_from_room(obj);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that on your back.\n\r", ch);
            return;
          }
        }
      }
      else {
        if (get_small_items(ch) + obj->size > MAX_SMALL_ITEMS) {
          if (get_eq_char(ch, WEAR_HOLD) == NULL && !is_ghost(ch)) {
            send_to_char("There is no space for that in your pockets, so you hold it.\n\r", ch);
            obj_from_room(obj);
            obj_to_char(obj, ch);
            wear_obj(ch, obj, TRUE, FALSE);
            return;
          }
          else {
            send_to_char("There is no space for that in your pockets.\n\r", ch);
            return;
          }
        }
      }

      pocketobj = get_jacket(ch);
      if (pocketobj == NULL || pocketobj->item_type != ITEM_CLOTHING) {
        pocketobj = get_pants(ch);
        if (pocketobj != NULL && pocketobj->item_type == ITEM_CLOTHING)
        haspants = TRUE;
      }
      else
      hasjacket = TRUE;

      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) || CAN_WEAR(obj, ITEM_WEAR_HOLD)) {
        act("You get $a $p and hold it.", ch, obj, container, TO_CHAR);
        act("$n gets $a $p and holds it.", ch, obj, container, TO_ROOM);
      }
      else if (is_big(obj)) {
        if (obj->item_type == ITEM_CLOTHING) {
          act("You get $a $p and throw it over your shoulder.", ch, obj, container, TO_CHAR);
          act("$n gets $a $p and throws it over $s shoulder.", ch, obj, container, TO_ROOM);
        }
        else if (obj->item_type == ITEM_WEAPON && obj->value[0] == WEAPON_SWORD) {
          act("You get $a $p and sheath it on your back.", ch, obj, NULL, TO_CHAR);
          act("$n gets $a $p and sheathes it on $s back.", ch, obj, NULL, TO_ROOM);
        }
        else {
          act("You get $a $p and strap it on your back.", ch, obj, container, TO_CHAR);
          act("$n gets $a $p and straps it onto $s back.", ch, obj, container, TO_ROOM);
        }
      }
      else {
        if (obj->item_type == ITEM_WEAPON) {
          act("You get $a $p and slip it into a hidden sheath.", ch, obj, NULL, TO_CHAR);
          act("$n gets $a $p and slips it into a hidden sheath.", ch, obj, NULL, TO_ROOM);
        }
        else if (hasjacket) {
          act("You get $a $p and slip it into $a $P pocket.", ch, obj, pocketobj, TO_CHAR);
          act("$n gets $a $p and slips it into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
        }
        else if (haspants) {
          act("You get $a $p and slip it into $a $P pocket.", ch, obj, pocketobj, TO_CHAR);
          act("$n gets $a $p and slips it into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
        }
        else {
          act("You get $a $p and slip it into a pocket.", ch, obj, NULL, TO_CHAR);
          act("$n gets $a $p and slips it into a pocket.", ch, obj, NULL, TO_ROOM);
        }
      }

      if (obj->in_room != NULL)
      obj_from_room(obj);
    }

    if (obj->item_type == ITEM_MONEY) {
      if (is_ghost(ch))
      return;
      char buf[MSL];
      sprintf(buf, "CASH: %s picks up %d.\n\r", ch->name, obj->value[1]);
      log_string(buf);

      if (valid_money(obj, ch))
      ch->money += obj->value[1];
      wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);

      extract_obj(obj);
    }
    else {

      obj_to_char(obj, ch);
      if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && !is_ghost(ch))
      wear_obj(ch, obj, TRUE, FALSE);
    }
    return;
  }

  bool can_access(CHAR_DATA *ch, int vnum) {
    if (vnum <= 0 || vnum == NULL)
    return FALSE;
  
    if (get_room_index(vnum) == NULL)
    return FALSE;

    if (!has_stash(get_room_index(vnum)))
    return FALSE;

    if (institute_room(get_room_index(vnum)) && !institute_room(ch->in_room))
    return FALSE;

    if (is_privatep(get_room_index(vnum)) && (!has_access(ch, get_room_index(vnum)) && !is_base(prop_from_room(get_room_index(vnum)))))
    return TRUE;

    if (vnum >= 31000 && vnum <= 40999)
    return FALSE;

    if (get_room_index(vnum)->area->vnum == DIST_MISTS)
    return FALSE;

    if (!has_exits((get_room_index(vnum))))
    return FALSE;

    if (vnum == 15108) // Narnia
    return FALSE;

    if (vnum == 15107) // Narnia
    return FALSE;

    if (vnum == 14913) // Rooms2Go
    return FALSE;

    if (vnum == 52)
    return FALSE;

    if (ch->in_room->vnum == vnum)
    return FALSE;

    if (has_active_vehicle(ch)) {
      if ((get_room_index(vnum)->sector_type == SECT_STREET || get_room_index(vnum)->sector_type == SECT_ALLEY) && (ch->pcdata->last_drove == ch->in_room->vnum || ch->pcdata->last_drove == 0))
      return FALSE;
    }

    return TRUE;
  }

  bool has_stash(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (IS_SET(room->room_flags, ROOM_STASH))
    return TRUE;

    if (IS_SET(room->room_flags, ROOM_BEDROOM))
    return TRUE;
    if (IS_SET(room->room_flags, ROOM_KITCHEN))
    return TRUE;

    if (room->sector_type == SECT_STREET)
    return TRUE;
    if (room->sector_type == SECT_ALLEY)
    return TRUE;

    if (room->area->vnum == DIST_MISTS)
    return TRUE;

    if (room->vnum >= 31000 && room->vnum < 40999)
    return TRUE;

    return FALSE;
  }

  bool stash_full(CHAR_DATA *ch, int stashroom) {
    OBJ_DATA *obj;
    int i;
    int count = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        for (i = 0; i < MAX_TAXIS; i++) {
          if (obj->stash_room == taxi_table[i].vnum)
          count++;
        }
      }
    }

    if (count < 10)
    return FALSE;

    for (i = 0; i < MAX_TAXIS; i++) {
      if (stashroom == taxi_table[i].vnum && count > 10)
      return TRUE;
    }

    return FALSE;
  }

  void stake(CHAR_DATA *ch, char name[MSL], OBJ_DATA *obj) {
    char buf[MSL];
    CHAR_DATA *victim;

    if ((victim = get_char_room(ch, NULL, name)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (ch->pcdata->bloodaura > 0) {
      send_to_char("Something compels you not to.\n\r", ch);
      return;
    }

    if (!is_helpless(victim)) {
      send_to_char("They won't hold still for that.\n\r", ch);
      return;
    }
    if (under_understanding(victim, ch)) {
      send_to_char("Your body seems unwilling to follow your commands to do that.\n\r", ch);
      return;
    }

    if (!IS_FLAG(ch->affected_by, AFF_STAKED)) {
      act("$n plunges $s $p into $N's heart.`x\n\r", ch, obj, victim, TO_ROOM);
      act("You plunge your $p into $N's heart.`x\n\r", ch, obj, victim, TO_CHAR);
      if (is_vampire(victim)) {
        send_to_char("You suffer a severe wound.\n\r", victim);
        send_to_char("Your body stops responding.`x\n\r", victim);
        wound_char_absolute(victim, 2);
        miscarriage(victim, FALSE);
        SET_FLAG(victim->affected_by, AFF_STAKED);
      }
      else {
        wound_char_absolute(victim, 3);
        send_to_char("You suffer a critical wound!\n\r", victim);
        miscarriage(victim, FALSE);
        critplayer(ch, victim);
        SET_FLAG(victim->affected_by, AFF_STAKED);
      }
      obj_from_char_silent(obj);
      obj_to_char(obj, victim);
    }
    else {
      sprintf(buf, "%s is already staked.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
      printf_to_char(ch, "%s", buf);
    }
  }

  _DOFUN(do_stake) {
    OBJ_DATA *obj;
    char arg1[MSL];
    argument = one_argument(argument, arg1);
    if (has_item_name(ch, "stake")) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        // obj_next = obj->next_content;
        if (strcasestr(obj->name, "stake")) {
          stake(ch, arg1, obj);
          return;
        }
      }
    }
    else {
      send_to_char("You don't have a stake.`x\n\r", ch);
    }
  }

  _DOFUN(do_get) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found = TRUE;

    CHAR_DATA *victim;
    if (is_gm(ch) && !IS_IMMORTAL(ch)) {
      return;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_FLAG(ch->act, PLR_DEAD) && !is_ghost(ch)) {
      send_to_char("You're dead.  Super dead.\n\r", ch);
      return;
    }
    
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && !is_hybrid(ch) && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1)) {
      send_to_char("You're an animal.  A real animal.\n\r", ch);
      return;
    }
    
    if (is_helpless(ch)) {
      send_to_char("You're helpless.  Completely helpless.\n\r", ch);
      return;
    }

    if ((victim = get_char_room(ch, NULL, arg2)) != NULL) {
      for (obj = victim->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (strcasestr(obj->name, "stake") && strcasestr(obj->name, arg1)) {
          if (IS_FLAG(victim->affected_by, AFF_STAKED)) {
            if (is_helpless(ch) || IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD))
            return;
            obj_from_char_silent(obj);
            obj_to_char(obj, ch);
            REMOVE_FLAG(victim->affected_by, AFF_STAKED);
            act("$n yanks the $p out of $N's heart.`x\n\r", ch, obj, victim, TO_ROOM);
            act("You yank the $p out of $N's heart.`x\n\r", ch, obj, victim, TO_CHAR);
            if (is_vampire(victim)) {
              send_to_char("Your body starts responding again.`x\n\r", victim);
            }
            else {
              act("The blood rushes out of $n's body, gushing onto the floor.`x\n\r", victim, NULL, NULL, TO_ROOM);
              killplayer(ch, victim);
              free_string(victim->pcdata->deathcause);
              victim->pcdata->deathcause =
              str_dup("A deep puncture in the vicinity of their heart.");
              real_kill(victim, ch);
            }
            victim->pcdata->mark_timer[0] = 12 * 60 * 5;
            free_string(victim->pcdata->mark[0]);
            sprintf(buf, "%s has a deep puncture in the vicinity of %s heart.`x\n\r", (victim->sex == SEX_MALE) ? "He" : "She", (victim->sex == SEX_MALE) ? "his" : "her");
            victim->pcdata->mark[0] = str_dup(buf);
          }
          else {
            sprintf(buf, "You don't see a stake in %s.`x\n\r", (victim->sex == SEX_MALE) ? "him" : "her");
            printf_to_char(ch, "%s", buf);
          }
          return;
        }
      }
    }
    
    if (!str_cmp(arg2, "from"))
    argument = one_argument(argument, arg2);

    // check for limit of ghost object interactions per day - Discordance
    if (is_ghost(ch)) {
      if (is_manifesting(ch)) {
        if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
          send_to_char("You're unable to muster the effort to affect that object.\n\r", ch);
          return;
        }
      }
      else {
        send_to_char("You must be prepared to manifest this power.\n\r", ch);
        return;
      }
    }

    /* Get type. */
    if (arg1[0] == '\0') {
      send_to_char("Get what?\n\r", ch);
      return;
    }

    if (arg2[0] == '\0') {
      if (in_fight(ch) || is_helpless(ch) || IS_FLAG(ch->act, PLR_SHROUD)) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
        /* 'get obj' */
        obj = get_obj_list(ch, arg1, ch->in_room->contents);
        if (obj == NULL) {
          act("I see no $T here.", ch, NULL, arg1, TO_CHAR);
          return;
        }
        if (ch->played / 3600 < 150 && ch->pcdata->account->maxhours < 150 && my_object(ch, obj) == FALSE) {
          act("You can't get that.", ch, NULL, arg1, TO_CHAR);
          return;
        }

        get_obj(ch, obj, NULL);
      }
      else {
        /* 'get all' or 'get all.obj' */
        found = FALSE;
        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;

          if (ch->played / 3600 < 150 && ch->pcdata->account->maxhours < 150 && my_object(ch, obj) == FALSE) {
            continue;
          }

          if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj)) {

            found = TRUE;
            get_obj(ch, obj, NULL);
          }
        }

        if (!found) {
          if (arg1[3] == '\0')
          send_to_char("I see nothing here.\n\r", ch);
          else
          act("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR);
        }
      }
    }
    else {
      /* 'get ... container' */
      if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if ((!str_cmp(arg2, "stash") || !str_cmp(arg2, "wardrobe"))) {
        char arg[MAX_INPUT_LENGTH];
        int number;
        int count;

        if (clinic_patient(ch)) {
          send_to_char("As a patient, you have no access to your stash.\n\r", ch);
          return;
        }
        if (!has_stash(ch->in_room)) {
          send_to_char("There's no stash here.\n\r", ch);
          return;
        }
        if (in_fight(ch)) {
          send_to_char("You're a bit busy.\n\r", ch);
          return;
        }
        if (is_helpless(ch) || IS_FLAG(ch->act, PLR_SHROUD)) {
          return;
        }

        number = number_argument(arg1, arg);
        count = 0;
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (obj->pIndexData->vnum == 35 && !has_stash(ch->in_room))
          continue;
          if (obj->stash_room != ch->in_room->vnum && can_access(ch, obj->stash_room)) {
            continue;
          }

          if (is_name(arg, obj->name) && IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
            if (++count == number) {
              REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
              act("You get $a $p from your stash.", ch, obj, NULL, TO_CHAR);
              return;
            }
          }
        }
        act("That is not in your stash", ch, NULL, NULL, TO_CHAR);
        return;
      }
      
      if ((container = get_obj_here(ch, NULL, arg2)) == NULL) {
        act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
        return;
      }

      switch (container->item_type) {
      default:
        send_to_char("That's not a container.\n\r", ch);
        return;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
        break;

      case ITEM_CORPSE_PC: {
          if (!can_loot(ch, container)) {
            send_to_char("You can't do that.\n\r", ch);
            return;
          }
        } break;
      }

      if (IS_SET(container->value[1], CONT_CLOSED)) {
        act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
        return;
      }
      
      if ((in_fight(ch) || is_helpless(ch) || IS_FLAG(ch->act, PLR_SHROUD)) && (container->carried_by == NULL || container->carried_by != ch)) {
        send_to_char("You're a bit busy.\n\r", ch);
        return;
      }

      if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
        /* 'get obj container' */
        obj = get_obj_list(ch, arg1, container->contains);
        if (obj == NULL) {
          act("I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR);
          return;
        }
        if (obj->size > 25 && in_fight(ch))
        useattack(ch);

        if ((obj->size > 25 || IS_SET(obj->extra_flags, ITEM_ARMORED)) && room_hostile(ch->in_room))
        return;

        get_obj(ch, obj, container);
        if (CAN_WEAR(obj, ITEM_WEAR_HOLD) && !is_ghost(ch)) {
          if (get_eqr_char(ch, WEAR_HOLD) == NULL)
          equip_char(ch, obj, WEAR_HOLD);
          else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
          equip_char(ch, obj, WEAR_HOLD_2);
        }

      }
      else {
        /* 'get all container' or 'get all.obj container' */
        found = FALSE;
        for (obj = container->contains; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;
          if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj)) {
            found = TRUE;
            if (container->pIndexData->vnum == OBJ_VNUM_PIT && !IS_IMMORTAL(ch)) {
              send_to_char("Don't be so greedy!\n\r", ch);
              return;
            }
            get_obj(ch, obj, container);
          }
        }
        
        if (container->pIndexData->vnum == 35 && container->contains == NULL)
        extract_obj(container);
        if (in_fight(ch))
        useattack(ch);

        if (!found) {
          if (arg1[3] == '\0')
          act("I see nothing in the $T.", ch, NULL, arg2, TO_CHAR);
          else
          act("I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR);
        }
      }
    }
    return;
  }

  _DOFUN(do_lift) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found = TRUE;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (is_ghost(ch)) {
      send_to_char("That object is too large for you to manipulate.\n\r", ch);
      return;
    }

    if(!can_manual_task(ch))
    {
      send_to_char("You cannot do manual tasks.\n\r", ch);
      return;
    }

    if (is_helpless(ch) || in_fight(ch))
    return;

    if (!str_cmp(arg2, "from"))
    argument = one_argument(argument, arg2);

    /* Get type. */
    if (arg1[0] == '\0') {
      send_to_char("Get what?\n\r", ch);
      return;
    }

    if (get_skill(ch, SKILL_STRENGTH) < 0 || is_griefer(ch)) {
      send_to_char("You're not strong enough to lift that!\n\r", ch);
      return;
    }

    if (arg2[0] == '\0') {
      if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
        /* 'get obj' */
        obj = get_obj_list(ch, arg1, ch->in_room->contents);
        if (obj == NULL) {
          act("I see no $T here.", ch, NULL, arg1, TO_CHAR);
          return;
        }
        if ((in_prop(ch) != NULL || in_house(ch) != NULL) && !can_decorate(ch, ch->in_room) && str_cmp(arg1, "corpse") && obj->pIndexData->vnum != OBJ_VNUM_CORPSE_NPC) {
          send_to_char("That's not exactly subtle thievery.\n\r", ch);
          return;
        }
        if (ch->played / 3600 < 150 && ch->pcdata->account->maxhours < 150 && my_object(ch, obj) == FALSE) {
          act("You can't get that.", ch, NULL, arg1, TO_CHAR);
          return;
        }

        get_obj_load(ch, obj, NULL);
      }
      else {
        /* 'get all' or 'get all.obj' */
        found = FALSE;
        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;

          if ((in_prop(ch) != NULL || in_house(ch) != NULL) && !can_decorate(ch, ch->in_room) && str_cmp(arg1, "corpse") && obj->pIndexData->vnum != OBJ_VNUM_CORPSE_NPC)
          continue;

          if (ch->played / 3600 < 150 && ch->pcdata->account->maxhours < 150 && my_object(ch, obj) == FALSE) {
            continue;
          }

          if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj)) {
            found = TRUE;
            get_obj_load(ch, obj, NULL);
          }
        }

        if (!found) {
          if (arg1[3] == '\0')
          send_to_char("I see nothing here.\n\r", ch);
          else
          act("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR);
        }
      }
    }
    else {
      /* 'get ... container' */
      if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2)) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if ((!str_cmp(arg2, "stash") || !str_cmp(arg2, "wardrobe"))) {
        char arg[MAX_INPUT_LENGTH];
        int number;
        int count;

        if (!has_stash(ch->in_room)) {
          send_to_char("There's no stash here.\n\r", ch);
          return;
        }

        number = number_argument(arg1, arg);
        count = 0;
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (obj->stash_room != ch->in_room->vnum && can_access(ch, obj->stash_room))
          continue;

          if (is_name(arg, obj->name) && IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
            if (++count == number) {
              REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
              act("You get $a $p from your stash.", ch, obj, NULL, TO_CHAR);
              return;
            }
          }
        }
        act("That is not in your stash", ch, NULL, NULL, TO_CHAR);
        return;
      }
      if ((container = get_obj_here(ch, NULL, arg2)) == NULL) {
        act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
        return;
      }

      switch (container->item_type) {
      default:
        send_to_char("That's not a container.\n\r", ch);
        return;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
        break;

      case ITEM_CORPSE_PC: {

          if (!can_loot(ch, container)) {
            send_to_char("You can't do that.\n\r", ch);
            return;
          }
        } break;
      }

      if (IS_SET(container->value[1], CONT_CLOSED)) {
        act("The $d is closed.", ch, NULL, container->short_descr, TO_CHAR);
        return;
      }

      if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
        /* 'get obj container' */
        obj = get_obj_list(ch, arg1, container->contains);
        if (obj == NULL) {
          act("I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR);
          return;
        }
        get_obj_load(ch, obj, container);
        if (CAN_WEAR(obj, ITEM_WEAR_HOLD)) {
          if (get_eqr_char(ch, WEAR_HOLD) == NULL)
          equip_char(ch, obj, WEAR_HOLD);
          else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
          equip_char(ch, obj, WEAR_HOLD_2);
        }

      }
      else {
        /* 'get all container' or 'get all.obj container' */
        found = FALSE;
        for (obj = container->contains; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;
          if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj)) {
            found = TRUE;
            if (container->pIndexData->vnum == OBJ_VNUM_PIT && !IS_IMMORTAL(ch)) {
              send_to_char("Don't be so greedy!\n\r", ch);
              return;
            }
            get_obj_load(ch, obj, container);
          }
        }

        if (!found) {
          if (arg1[3] == '\0')
          act("I see nothing in the $T.", ch, NULL, arg2, TO_CHAR);
          else
          act("I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR);
        }
      }
    }
    WAIT_STATE(ch, PULSE_PER_SECOND * 10);

    return;
  }

  _DOFUN(do_put) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!str_cmp(arg2, "in") || !str_cmp(arg2, "on"))
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
      send_to_char("Put what in what?\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if ((!str_cmp(arg2, "stash") || !str_cmp(arg2, "wardrobe"))) {
      if (!has_stash(ch->in_room)) {
        send_to_char("There's no stash here.\n\r", ch);
        return;
      }
      if (can_access(ch, ch->in_room->vnum) || stash_full(ch, ch->in_room->vnum)) {
        send_to_char("There's no stash here or it is out of space.\n\r", ch);
        return;
      }

      if ((obj = get_obj_carryhold(ch, arg1, NULL)) == NULL) {
        send_to_char("That isn't here.\n\r", ch);
        return;
      }
      else {

        if (obj->item_type == ITEM_CORPSE_NPC && obj->timer > 0) {
          send_to_char("You can't put that there.\n\r", ch);
          return;
        }
        if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
          send_to_char("You can't put that there.\n\r", ch);
          return;
        }
        if (obj->pIndexData->vnum == 36 && obj->size == 50) {
          send_to_char("You can't put that there.\n\r", ch);
          return;
        }

        if (obj->wear_loc != WEAR_NONE)
        unequip_char(ch, obj);

        obj->stash_room = ch->in_room->vnum;
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        act("You put $a $p in your stash", ch, obj, NULL, TO_CHAR);
        return;
      }
    }

    if ((container = get_obj_here(ch, NULL, arg2)) == NULL) {
      act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
      return;
    }

    if (container->item_type != ITEM_CONTAINER) {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }

    if (IS_SET(container->value[1], CONT_CLOSED)) {
      act("The $d is closed.", ch, NULL, container->short_descr, TO_CHAR);
      return;
    }

    if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
      /* 'put obj container' */
      if ((obj = get_obj_carryhold(ch, arg1, ch)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }

      if (obj == container) {
        send_to_char("You can't fold it into itself.\n\r", ch);
        return;
      }

      if (!can_drop_obj(ch, obj)) {
        send_to_char("You can't let go of it.\n\r", ch);
        return;
      }

      if (obj->size + get_true_weight(container) > container->value[0]) {
        send_to_char("It won't fit.\n\r", ch);
        return;
      }
      if (obj->item_type == ITEM_CORPSE_NPC && obj->timer > 0) {
        send_to_char("You can't put that there.\n\r", ch);
        return;
      }
      if (obj->item_type == ITEM_CONTAINER && obj->item_type != ITEM_BABY) {
        send_to_char("You can't put that there.\n\r", ch);
        return;
      }
      if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
        send_to_char("You can't put that there.\n\r", ch);
        return;
      }
      if (obj->pIndexData->vnum == 36 && obj->size == 50) {
        send_to_char("You can't put that there.\n\r", ch);
        return;
      }

      if (obj->size > container->value[3]) {
        send_to_char("It won't fit.\n\r", ch);
        return;
      }

      /*
      if (get_obj_weight( obj ) + get_true_weight( container ) > (container->value[0] * 10)
      ||  get_obj_weight(obj) > (container->value[3] * 10))
      {
        send_to_char( "It won't fit.\n\r", ch );
        return;
      }
      */

      obj_from_char(obj);
      obj_to_obj(obj, container);

      if (IS_SET(container->value[1], CONT_PUT_ON)) {
        act("$n puts $a $p on $P.", ch, obj, container, TO_ROOM);
        act("You put $a $p on $P.", ch, obj, container, TO_CHAR);
      }
      else {
        act("$n puts $a $p in $P.", ch, obj, container, TO_ROOM);
        act("You put $a $p in $P.", ch, obj, container, TO_CHAR);
      }
    }
    else {
      send_to_char("You don't see that here.\n\r", ch);
      return;
      /* 'put all container' or 'put all.obj container' */
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name)) && can_see_obj(ch, obj) && WEIGHT_MULT(obj) == 100 && obj->wear_loc == WEAR_NONE && obj != container && !IS_SET(obj->extra_flags, ITEM_RELIC) && obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CONTAINER && can_drop_obj(ch, obj) && (obj->pIndexData->vnum != 36 || obj->size != 50)
            //	    &&   get_obj_weight( obj ) + get_true_weight( container )
            //		 <= (container->value[0] * 10)
            //	    &&   get_obj_weight(obj) < (container->value[3] * 10))
            ) {
          obj_from_char(obj);
          obj_to_obj(obj, container);

          if (IS_SET(container->value[1], CONT_PUT_ON)) {
            act("$n puts $p on $P.", ch, obj, container, TO_ROOM);
            act("You put $p on $P.", ch, obj, container, TO_CHAR);
          }
          else {
            act("$n puts $a $p in $P.", ch, obj, container, TO_ROOM);
            act("You put $a $p in $P.", ch, obj, container, TO_CHAR);
          }
        }
      }
    }
    return;
  }

  char *dropprefix(OBJ_DATA *obj) {
    if (obj != NULL && (!str_prefix("a ", obj->short_descr) || !str_prefix("an ", obj->short_descr)))
    return "";
    else if (obj != NULL && is_pair(obj) && obj->item_type == ITEM_CLOTHING) {
      return "a pair of";
    }
    else {
      if (obj != NULL)
      return a_or_an(obj->short_descr);
      else
      return "a";
    }
  }

  _DOFUN(do_drop) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;
    char buf[MSL];

    argument = one_argument(argument, arg);

    if ((is_gm(ch) || higher_power(ch)) && !IS_IMMORTAL(ch))
    return;

    if (!is_corporeal(ch) || IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You cannot touch the physical.\n\r", ch);
      return;
    }

    if (is_griefer(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (arg[0] == '\0') {
      send_to_char("Drop what?\n\r", ch);
      return;
    }

    if (is_number(arg)) {
      /* 'drop NNNN coins' */
      int amount;
      float cash;

      cash = atof(arg);
      cash *= 100;
      amount = (int)cash;

      argument = one_argument(argument, arg);

      if (amount <= 0) {
        send_to_char("Sorry, you can't do that.\n\r", ch);
        return;
      }

      if (ch->money < amount) {
        send_to_char("You don't have that much money.\n\r", ch);
        return;
      }

      ch->money -= amount;

      obj_to_room(create_money(amount, ch), ch->in_room);
      act("$n drops some money.", ch, NULL, NULL, TO_ROOM);
      send_to_char("OK.\n\r", ch);
      alter_character(ch);
      char buf[MSL];
      sprintf(buf, "CASH: %s drops %d at %d.\n\r", ch->name, amount, ch->in_room->vnum);
      log_string(buf);
      wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);

      return;
    }

    if (str_cmp(arg, "all") && str_prefix("all.", arg)) {
      /* 'drop obj' */
      bool removing = FALSE;
      if ((obj = get_obj_carryhold(ch, arg, NULL)) == NULL) {
        if ((obj = get_obj_wear(ch, arg, TRUE)) == NULL) {
          send_to_char("That isn't here.\n\r", ch);
          return;
        }
        removing = TRUE;
      }

      if (is_helpless(ch))
      steal_object(obj, ch);
      if (!can_drop_obj(ch, obj)) {
        send_to_char("You can't let go of it.\n\r", ch);
        return;
      }
      if(removing == TRUE && IS_SET(obj->extra_flags, ITEM_CURSED) && (is_helpless(ch) || is_possessed(ch)))
      {
        send_to_char("You can't let go of it.\n\r", ch);
        return;
      }


      if (removing == TRUE) {
        if (IS_SET(obj->extra_flags, ITEM_CURSED)) {
          EXTRA_DESCR_DATA *ed;
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+imprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_CURSED) {
                  ch->pcdata->imprint_type[i] = 0;
                }
              }
            }

            if (is_name("+bimprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_BCURSED) {
                  ch->pcdata->imprint_type[i] = 0;
                }
              }
            }



          }
        }
        remove_obj(ch, obj->wear_loc, TRUE);
      }
      if (obj->item_type == ITEM_PHONE && ch->pcdata->connected_to != NULL) {
        act("The connection breaks.\n\r", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);

        ch->pcdata->connected_to->pcdata->connection_stage = CONNECT_NONE;
        ch->pcdata->connected_to->pcdata->connected_to = NULL;
        ch->pcdata->connection_stage = CONNECT_NONE;
        ch->pcdata->connected_to = NULL;
      }
      if (ch->in_room->area->vnum == DIST_MISTS) {
        if (obj->wear_loc != WEAR_NONE)
        unequip_char(ch, obj);
        obj->stash_room = ch->in_room->vnum;
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        act("You put $a $p in your stash", ch, obj, NULL, TO_CHAR);
        sprintf(buf, "$n drops %s %s.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_ROOM);
        return;
      }

      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
      alter_character(ch);
      if (in_fight(ch)) {
        sprintf(buf, "$n drops %s %s.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_ROOM);
        sprintf(buf, "You drop %s %s.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_CHAR);
      }
      else {
        sprintf(buf, "$n puts %s %s down.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_ROOM);
        sprintf(buf, "You put %s %s down.", dropprefix(obj), obj->short_descr);
        act(buf, ch, NULL, NULL, TO_CHAR);
      }

    }
    else {
      alter_character(ch);
      /* 'drop all' or 'drop all.obj' */
      found = FALSE;
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if ((arg[3] == '\0' || is_name(&arg[4], obj->name)) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE && can_drop_obj(ch, obj)) {
          found = TRUE;
          if (is_helpless(ch))
          steal_object(obj, ch);
          if (obj->item_type == ITEM_PHONE && ch->pcdata->connected_to != NULL) {
            act("The connection breaks.\n\r", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);

            ch->pcdata->connected_to->pcdata->connection_stage = CONNECT_NONE;
            ch->pcdata->connected_to->pcdata->connected_to = NULL;
            ch->pcdata->connection_stage = CONNECT_NONE;
            ch->pcdata->connected_to = NULL;
          }

          obj_from_char(obj);
          obj_to_room(obj, ch->in_room);
          sprintf(buf, "$n drops %s %s.", dropprefix(obj), obj->short_descr);
          act(buf, ch, NULL, NULL, TO_ROOM);
          sprintf(buf, "You drop %s %s.", dropprefix(obj), obj->short_descr);
          act(buf, ch, NULL, NULL, TO_CHAR);
        }
      }

      if (!found) {
        if (arg[3] == '\0')
        act("You are not carrying anything.", ch, NULL, arg, TO_CHAR);
        else
        act("You are not carrying any $T.", ch, NULL, &arg[4], TO_CHAR);
      }
    }

    return;
  }

  _DOFUN(do_give) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    // keeps storyrunners from giving people stuff
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
      send_to_char("Give what to whom?\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "intel")) {
      if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }
      if (IS_NPC(victim) || ch->faction != victim->faction) {
        send_to_char("You can only give intel to other members of your society.\n\r", ch);
        return;
      }
      victim->pcdata->intel += ch->pcdata->intel / 4;
      ch->pcdata->intel = 0;
      act("$n divulges their intel to $N", ch, NULL, victim, TO_CHAR);
      act("$n divulges their intel to $N", ch, NULL, victim, TO_ROOM);
      return;
    }

    if (is_number_float(arg1)) {
      if ((is_gm(ch) || higher_power(ch)) && !IS_IMMORTAL(ch))
      return;
      /* 'give NNNN coins victim' */
      double amount;
      amount = atof(arg1);
      amount *= 100;

      if (ch->money < amount) {
        send_to_char("You don't have that much money.\n\r", ch);
        return;
      }

      if (amount < 1) {
        send_to_char("Well that's just silly.\n\r", ch);
        return;
      }

      if (argument[0] == '\0') {
        send_to_char("Give what to whom?\n\r", ch);
        return;
      }

      if ((victim = get_char_room(ch, NULL, argument)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
      }

      if (is_corporeal(ch) != is_corporeal(victim)) {
        send_to_char("You can't touch them.\n\r", ch);
        return;
      }
      
      if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD)) {
        send_to_char("You can't touch them.\n\r", ch);
        return;
      }

      if (IS_NPC(victim)) {
        if (IS_FLAG(victim->act, ACT_BRIBEMOB) && ch->pcdata->patrol_status == PATROL_BRIBING && ch->pcdata->patrol_room == ch->in_room) {
          if (ch->pcdata->patrol_timer > 30) {
            send_to_char("There's still too many people near them for them to accept the bribe.\n\r", ch);
            return;
          }
          
          if (amount / 100 < victim->level) {
            act("$N rejects the bribe, looking offended at how little was offered. It will probably cost even more now.", ch, NULL, victim, TO_CHAR);
            act("$n rejects $n's bribe, looking offended.", ch, NULL, victim, TO_ROOM);
            victim->level = victim->level * 12 / 10;
            return;
          }
          victim->ttl = 2;
          act("$N accepts the bribe.", ch, NULL, victim, TO_CHAR);
          act("$N accepts the bribe.", ch, NULL, victim, TO_ROOM);
          sprintf(buf, "favors from %s.", victim->long_descr);
          gain_resources(victim->level / 10, ch->faction, ch, buf);
          patrol_personal_award(ch, PATROL_BRIBE);
          victim->ttl = 1;
          REMOVE_FLAG(victim->act, ACT_BRIBEMOB);
          for (CharList::iterator it = ch->in_room->people->begin();
          it != ch->in_room->people->end();) {
            CHAR_DATA *fch = *it;
            ++it;
            if (fch == NULL || IS_NPC(fch))
            continue;
            fch->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC]++;
            fch->pcdata->life_tracker[TRACK_PATROL_DIPLOMATIC]++;
          }
        }
        else if (IS_FLAG(victim->act, ACT_BRIBEMOB)) {
          send_to_char("That isn't your NPC to bribe.\n\r", ch);
          return;
        }
      }
      // Cash transaction logging
      sprintf(buf, "CASH: %s gets given %d by %s.\n\r", victim->name, (int)amount, ch->name);
      log_string(buf);
      wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);

      alter_character(ch);
      ch->money -= (int)(amount);
      victim->money += (int)(amount);

      sprintf(buf, "$n gives you %.2f dollars.", amount / 100);
      act(buf, ch, NULL, victim, TO_VICT);
      act("$n gives $N some money.", ch, NULL, victim, TO_NOTVICT);
      sprintf(buf, "You give $N %.2f dollars.", amount / 100);
      act(buf, ch, NULL, victim, TO_CHAR);

      /*
      * Bribe trigger
      */

      return;
    }

    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg1, obj->name)) {
      if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg1, obj->name))
      if ((obj = get_obj_carry(ch, arg1, ch)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }
    }

    if ((victim = get_char_room(ch, NULL, arg2)) == NULL) {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL) {
      act("$N tells you 'Sorry, you'll have to sell that.'", ch, NULL, victim, TO_CHAR);
      ch->reply = victim;
      return;
    }

    if ((is_gm(ch) || higher_power(ch)) && !IS_IMMORTAL(ch)) {
      if (!stolen_object(ch, obj) || stolen_object(victim, obj))
      return;
    }

    if (!can_drop_obj(ch, obj)) {
      send_to_char("You can't let go of it.\n\r", ch);
      return;
    }

    if (is_big(obj)) {
      if (get_big_items(victim) + obj->size > MAX_BIG_ITEMS) {
        send_to_char("They can't carry that.\n\r", ch);
        return;
      }
    }
    else {
      if (get_small_items(victim) + obj->size > MAX_SMALL_ITEMS) {
        send_to_char("They can't carry that.\n\r", ch);
        return;
      }
    }

    /*
    if (get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w( victim) )
    {
      act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
      return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
      act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
      return;
    }
    */

    if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_BABY) {
      OBJ_DATA *obj_in;

      for (obj_in = obj->contains; obj_in != NULL;
      obj_in = obj_in->next_content) {
        if (obj_in->pIndexData->vnum == 28000 || obj_in->pIndexData->vnum == 28003) {
          obj_in->timer = 5;
          obj_in->rot_timer = 5;
        }
      }
    }

    if (obj->item_type == ITEM_PHONE) {
      if (ch->pcdata->connected_to != NULL && victim->pcdata->connected_to == NULL) {
        ch->pcdata->connected_to->pcdata->connected_to = victim;
        victim->pcdata->connected_to = ch->pcdata->connected_to;
        victim->pcdata->connection_stage = ch->pcdata->connection_stage;
        ch->pcdata->connection_stage = CONNECT_NONE;
        ch->pcdata->connected_to = NULL;
      }
      else if (ch->pcdata->connected_to != NULL) {
        act("The connection breaks.\n\r", ch->pcdata->connected_to, NULL, ch->pcdata->connected_to, TO_CHAR);

        ch->pcdata->connected_to->pcdata->connection_stage = CONNECT_NONE;
        ch->pcdata->connected_to->pcdata->connected_to = NULL;
        ch->pcdata->connection_stage = CONNECT_NONE;
        ch->pcdata->connected_to = NULL;
      }
    }
    alter_character(ch);
    obj_from_char(obj);
    obj_to_char(obj, victim);
    if (obj->pIndexData->vnum == 405002 && IS_NPC(victim) && victim->pIndexData->vnum == 405010) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * 60 * 3;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_FLESHFORMING;
      affect_to_char(ch, &af);
      send_to_char("The receptionist thanks you for the payment and informs you that the fleshformer will see you now. (Use the fleshform command to fleshform yourself)\n\r", ch);
    }

    if (CAN_WEAR(obj, ITEM_WEAR_HOLD)) {
      if (get_eqr_char(victim, WEAR_HOLD) == NULL)
      equip_char_silent(victim, obj, WEAR_HOLD);
      else if (get_eqr_char(victim, WEAR_HOLD_2) == NULL)
      equip_char_silent(victim, obj, WEAR_HOLD_2);
    }
    MOBtrigger = FALSE;
    act("$n gives $a $p to $N.", ch, obj, victim, TO_NOTVICT);
    act("$n gives you $a $p.", ch, obj, victim, TO_VICT);
    act("You give $a $p to $N.", ch, obj, victim, TO_CHAR);
    MOBtrigger = TRUE;

    return;
  }

  _DOFUN(do_pour) {
    char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
      send_to_char("Pour what into what?\n\r", ch);
      return;
    }

    if ((out = get_obj_carry(ch, arg, ch)) == NULL) {
      send_to_char("You don't have that item.\n\r", ch);
      return;
    }

    if (out->item_type != ITEM_DRINK_CON) {
      send_to_char("That's not a drink container.\n\r", ch);
      return;
    }
    if (out->value[1] < 1) {
      send_to_char("It's empty.\n\r", ch);
      return;
    }

    if (!str_cmp(argument, "glass")) {
      EXTRA_DESCR_DATA *ed;
      EXTRA_DESCR_DATA *ped;

      OBJ_DATA *obj;
      obj = create_object(get_obj_index(34), 0);
      obj_to_char(obj, ch);
      amount = UMIN(out->value[1], 4);
      out->value[1] -= amount;
      obj->value[1] += amount;
      obj->value[2] = out->value[2];
      obj->cost = out->cost / 4;

      for (ed = out->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword)) {
          for (ped = obj->extra_descr; ped; ped = ped->next) {
            if (is_name("+taste", ped->keyword)) {
              free_string(ped->description);
              ped->description = str_dup(ed->description);
            }
          }
        }
      }

      act("You pour a glass from $P.", ch, out, out, TO_CHAR);
      act("$n pours a glass from $P.", ch, out, out, TO_ROOM);
      return;
    }

    if (!str_cmp(argument, "out")) {
      if (out->value[1] == 0) {
        send_to_char("It's already empty.\n\r", ch);
        return;
      }

      out->value[1] = 0;
      out->value[3] = 0;
      sprintf(buf, "You invert $a $p, spilling %s all over the ground.", liq_table[out->value[2]].liq_name);
      act(buf, ch, out, NULL, TO_CHAR);

      sprintf(buf, "$n inverts $s $p, spilling %s all over the ground.", liq_table[out->value[2]].liq_name);
      act(buf, ch, out, NULL, TO_ROOM);
      return;
    }

    if ((in = get_obj_here(ch, NULL, argument)) == NULL) {
      return;
    }

    if (IS_OBJ_STAT(out, ITEM_VBLOOD) && (in->item_type == ITEM_DRINK_CON || in->item_type == ITEM_FOOD)) {
      sprintf(buf, "You pour some red liquid from $p into $P.");
      act(buf, ch, out, in, TO_CHAR);
      if (in->carried_by == NULL) {
        sprintf(buf, "$n pours some red liquid from $p into $P.");
        act(buf, ch, out, in, TO_ROOM);
      }

      if (!IS_SET(in->extra_flags, ITEM_VBLOOD))
      SET_BIT(in->extra_flags, ITEM_VBLOOD);

      extract_obj(out);
      return;
    }

    if (IS_OBJ_STAT(out, ITEM_ROHYPNOL) && (in->item_type == ITEM_DRINK_CON || in->item_type == ITEM_FOOD)) {
      sprintf(buf, "You pour some clear liquid from $p into $P.");
      act(buf, ch, out, in, TO_CHAR);
      if (in->carried_by == NULL) {
        sprintf(buf, "$n pours some clear liquid from $p into $P.");
        act(buf, ch, out, in, TO_ROOM);
      }

      if (!IS_SET(in->extra_flags, ITEM_ROHYPNOL))
      SET_BIT(in->extra_flags, ITEM_ROHYPNOL);

      extract_obj(out);
      return;
    }

    if (IS_OBJ_STAT(out, ITEM_POISON) && (in->item_type == ITEM_DRINK_CON || in->item_type == ITEM_FOOD)) {
      sprintf(buf, "You pour some clear liquid from $p into $P.");
      act(buf, ch, out, in, TO_CHAR);
      if (in->carried_by == NULL) {
        sprintf(buf, "$n pours some clear liquid from $p into $P.");
        act(buf, ch, out, in, TO_ROOM);
      }

      if (!IS_SET(in->extra_flags, ITEM_POISON))
      SET_BIT(in->extra_flags, ITEM_POISON);

      extract_obj(out);
      return;
    }

    if (in->item_type != ITEM_DRINK_CON) {
      send_to_char("You can only pour into other drink containers.\n\r", ch);
      return;
    }

    if (in == out) {
      send_to_char("You cannot change the laws of physics!\n\r", ch);
      return;
    }

    if (in->value[1] != 0 && in->value[2] != out->value[2]) {
      send_to_char("They don't hold the same liquid.\n\r", ch);
      return;
    }

    if (out->value[1] == 0) {
      act("There's nothing in $a $p to pour.", ch, out, NULL, TO_CHAR);
      return;
    }

    if (in->value[1] >= in->value[0]) {
      act("$p is already filled to the top.", ch, in, NULL, TO_CHAR);
      return;
    }

    amount = UMIN(out->value[1], in->value[0] - in->value[1]);

    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];

    if (vch == NULL) {
      sprintf(buf, "You pour %s from $p into $P.", liq_table[out->value[2]].liq_name);
      act(buf, ch, out, in, TO_CHAR);
      sprintf(buf, "$n pours %s from $p into $P.", liq_table[out->value[2]].liq_name);
      act(buf, ch, out, in, TO_ROOM);
    }
    else {
      sprintf(buf, "You pour some %s for $N.", liq_table[out->value[2]].liq_name);
      act(buf, ch, NULL, vch, TO_CHAR);
      sprintf(buf, "$n pours you some %s.", liq_table[out->value[2]].liq_name);
      act(buf, ch, NULL, vch, TO_VICT);
      sprintf(buf, "$n pours some %s for $N.", liq_table[out->value[2]].liq_name);
      act(buf, ch, NULL, vch, TO_NOTVICT);
    }
  }

  _DOFUN(do_drink) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;
    EXTRA_DESCR_DATA *ed;

    one_argument(argument, arg);

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0)
    return;

    if (arg[0] == '\0') {

      for (obj = ch->in_room->contents; obj; obj = obj->next_content) {
        if (obj->item_type == ITEM_FOUNTAIN)
        break;
      }

      if (obj == NULL) {
        send_to_char("Drink what?\n\r", ch);
        return;
      }
    }
    else {
      if ((obj = get_obj_here(ch, NULL, arg)) == NULL) {
        act("I see no $T here.", ch, NULL, arg, TO_CHAR);
        return;
      }

      if (obj->pIndexData->vnum == 30 && is_vampire(ch)) {
        if (obj->level == 0)
        ch->pcdata->heldblood[5]++;
        else
        ch->pcdata->heldblood[obj->level - 1]++;

        act("You tear into the bag with your fangs and hungrily drink down its contents.", ch, NULL, NULL, TO_CHAR);
        act("$n tears into a blood bag with $s fangs and hungrily drinks down its contents.", ch, NULL, NULL, TO_ROOM);
        extract_obj(obj);
        return;
      }
      if (obj->item_type == ITEM_DRUGS) {
        EXTRA_DESCR_DATA *ed;
        act("$n drinks $s $p.", ch, obj, NULL, TO_ROOM);
        act("You drink your $p.", ch, obj, NULL, TO_CHAR);

        bool tastefound = FALSE;
        bool imprintfound = FALSE;
        for (ed = obj->extra_descr; ed; ed = ed->next) {
          if (is_name("+taste", ed->keyword)) {
            send_to_char(ed->description, ch);
            tastefound = TRUE;
          }
          if (is_name("+imprint", ed->keyword)) {
            auto_imprint(ch, ed->description, IMPRINT_DRUGS);
            imprintfound = TRUE;
          }
        }
        for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
          if (is_name("+taste", ed->keyword) && !tastefound) {
            send_to_char(ed->description, ch);
            tastefound = TRUE;
          }
          if (is_name("+imprint", ed->keyword) && !imprintfound) {
            auto_imprint(ch, ed->description, IMPRINT_DRUGS);
            imprintfound = TRUE;
          }
        }
        extract_obj(obj);
        return;
      }

      if (obj->item_type != ITEM_DRINK_CON) {
        if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg, obj->name) || obj->item_type != ITEM_DRINK_CON) {
          send_to_char("You can't drink from that.\n\r", ch);
          return;
        }
      }
    }

    if (!IS_NPC(ch) && ch->pcdata->conditions[COND_DRUNK] > 1000) {
      send_to_char("You fail to reach your mouth.  *Hic*\n\r", ch);
      return;
    }

    switch (obj->item_type) {
    default:
      send_to_char("You can't drink from that.\n\r", ch);
      return;

    case ITEM_FOUNTAIN:
      AFFECT_DATA *poison_aff;
      if ((liquid = obj->value[2]) < 0) {
        bug("Do_drink: bad liquid number %d.", liquid);
        liquid = obj->value[2] = 0;
      }
      amount = liq_table[liquid].liq_affect[4] * 3;
      if ((poison_aff = get_affect_by_location(obj->affected, APPLY_POISON)) !=
          NULL) {
      }
      break;

    case ITEM_DRINK_CON:
      if (obj->value[1] <= 0) {
        send_to_char("It is already empty.\n\r", ch);
        return;
      }

      if ((liquid = obj->value[2]) < 0) {
        bug("Do_drink: bad liquid number %d.", liquid);
        liquid = obj->value[2] = 0;
      }

      amount = liq_table[liquid].liq_affect[4];
      amount = UMIN(amount, obj->value[1]);
      break;
    }
    if (!IS_NPC(ch) && is_vampire(ch) && !is_name("blood", obj->name))
    ch->pcdata->vamp_fullness += 1;
    act("$n drinks from $s $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM);
    act("You drink from your $p.", ch, obj, liq_table[liquid].liq_name, TO_CHAR);

    bool tastefound = FALSE;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+taste", ed->keyword)) {
        send_to_char(ed->description, ch);
        tastefound = TRUE;
      }
    }
    if (tastefound == FALSE) {
      for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword)) {
          send_to_char(ed->description, ch);
          tastefound = TRUE;
        }
      }
    }

    if (IS_OBJ_STAT(obj, ITEM_VBLOOD) && !IS_FLAG(ch->comm, COMM_VBLOOD)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 60;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_VBLOOD;
      affect_to_char(ch, &af);

      send_to_char("You start to feel energized, aroused and powerful as the vampire blood hits your system.\n\r", ch);
      SET_FLAG(ch->comm, COMM_VBLOOD);
    }

    if (IS_OBJ_STAT(obj, ITEM_POISON) && ch->pcdata->poisontimer == 0) {
      ch->pcdata->poisontimer = number_range(200, 300);
      if (is_undead(ch))
      ch->pcdata->poisontimer = 0;
      else if (is_super(ch))
      ch->pcdata->poisontimer *= 2;
    }

    if (IS_OBJ_STAT(obj, ITEM_ROHYPNOL) && ch->pcdata->rohyptimer == 0) {
      ch->pcdata->rohyptimer = number_range(100, 150);

      if (is_undead(ch))
      ch->pcdata->rohyptimer *= 3;
      else if (is_super(ch))
      ch->pcdata->rohyptimer *= 2;
    }

    if (!IS_NPC(ch) && liq_table[obj->value[2]].liq_affect[COND_DRUNK] > 1) {
      if(college_group(ch, FALSE) == COLLEGE_BADKID)
      {
        ch->pcdata->conditions[COND_DRUNK] +=
        liq_table[obj->value[2]].liq_affect[COND_DRUNK]/2;
        ch->pcdata->atolerance += liq_table[obj->value[2]].liq_affect[COND_DRUNK]/2;
      }
      else
      {
        ch->pcdata->conditions[COND_DRUNK] +=
        liq_table[obj->value[2]].liq_affect[COND_DRUNK];
        ch->pcdata->atolerance += liq_table[obj->value[2]].liq_affect[COND_DRUNK];

      }
    }
    if (!IS_NPC(ch) && obj->pIndexData->vnum == 85) {
      ch->pcdata->other_power = obj->value[4];
      if (ch->pcdata->other_power > current_time - (3600 * 24 * 30))
      ch->pcdata->other_power =
      UMAX(ch->pcdata->other_power, current_time + (3600 * 24 * 7));
      extract_obj(obj);
      return;
    }
    if (!IS_NPC(ch) && obj->pIndexData->vnum == 86) {
      ch->pcdata->godrealm_power = obj->value[4];
      if (ch->pcdata->godrealm_power > current_time - (3600 * 24 * 30))
      ch->pcdata->godrealm_power =
      UMAX(ch->pcdata->godrealm_power, current_time + (3600 * 24 * 7));
      extract_obj(obj);
      return;
    }
    if (!IS_NPC(ch) && obj->pIndexData->vnum == 87) {
      ch->pcdata->hell_power = obj->value[4];
      if (ch->pcdata->hell_power > current_time - (3600 * 24 * 30))
      ch->pcdata->hell_power =
      UMAX(ch->pcdata->hell_power, current_time + (3600 * 24 * 7));
      extract_obj(obj);
      return;
    }

    if (liq_table[obj->value[2]].liq_affect[COND_DRUNK] == 2)
    send_to_char("You taste a small amount of alcohol.\n\r", ch);
    else if (liq_table[obj->value[2]].liq_affect[COND_DRUNK] == 10)
    send_to_char("You taste a large amount of alcohol.\n\r", ch);

    if (obj->value[0] > 0)
    obj->value[1] = UMAX(0, obj->value[1] - 1);

    if (obj->value[1] == 0) {

      if (obj->pIndexData->vnum == 405001) {
        if (!IS_FLAG(ch->comm, COMM_MANDRAKE))
        SET_FLAG(ch->comm, COMM_MANDRAKE);
      }

      if (obj->item_type == ITEM_FOUNTAIN) {
        if (obj->value[1] == 0) {
          act("$a $p dries up.", ch, obj, NULL, TO_ALL);
          obj_from_room(obj);
          extract_obj(obj);
        }
      }

      act("You finish your $p.", ch, obj, NULL, TO_CHAR);
      act("$n finishes $s $p.", ch, obj, NULL, TO_ROOM);
      if (!IS_NPC(ch))
      ch->pcdata->energy_recovery += 10;

      extract_obj(obj);
    }

    if (!IS_NPC(ch) && ch->hit >= max_hp(ch) && ch->pcdata->fatigue > 100 && ch->pcdata->fatigue_temp < 100) {
      ch->pcdata->fatigue -= 50;
      ch->pcdata->fatigue_temp += 5;

      if (ch->pcdata->habit[HABIT_EATING] == 1) {
        ch->pcdata->fatigue -= 50;
        ch->pcdata->fatigue_temp += 5;
      }

      if (ch->pcdata->habit[HABIT_EATING] == 4) {
        ch->pcdata->fatigue -= 50;
        ch->pcdata->fatigue_temp += 5;
      }
    }

    return;
  }

  _DOFUN(do_chug) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;
    EXTRA_DESCR_DATA *ed;

    one_argument(argument, arg);

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0)
    return;

    if (arg[0] == '\0') {

      for (obj = ch->in_room->contents; obj; obj = obj->next_content) {
        if (obj->item_type == ITEM_FOUNTAIN)
        break;
      }

      if (obj == NULL) {
        send_to_char("Drink what?\n\r", ch);
        return;
      }
    }
    else {
      if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg, obj->name))
      if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }

      if (obj->item_type != ITEM_DRINK_CON) {
        if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg, obj->name) || obj->item_type != ITEM_DRINK_CON) {
          send_to_char("You can't drink from that.\n\r", ch);
          return;
        }
      }
    }

    if (!IS_NPC(ch) && ch->pcdata->conditions[COND_DRUNK] > 1000) {
      send_to_char("You fail to reach your mouth.  *Hic*\n\r", ch);
      return;
    }

    switch (obj->item_type) {
    default:
      send_to_char("You can't drink from that.\n\r", ch);
      return;

    case ITEM_FOUNTAIN:
      if ((liquid = obj->value[2]) < 0) {
        bug("Do_drink: bad liquid number %d.", liquid);
        liquid = obj->value[2] = 0;
      }
      amount = liq_table[liquid].liq_affect[4] * 3;

      break;

    case ITEM_DRINK_CON:
      if (obj->value[1] <= 0) {
        send_to_char("It is already empty.\n\r", ch);
        return;
      }

      if ((liquid = obj->value[2]) < 0) {
        bug("Do_drink: bad liquid number %d.", liquid);
        liquid = obj->value[2] = 0;
      }

      amount = liq_table[liquid].liq_affect[4];
      amount = UMIN(amount, obj->value[1]);
      break;
    }

    if (IS_OBJ_STAT(obj, ITEM_VBLOOD) && !IS_FLAG(ch->comm, COMM_VBLOOD)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 60;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_VBLOOD;
      affect_to_char(ch, &af);

      send_to_char("You start to feel energized, aroused and powerful as the vampire blood hits your system.\n\r", ch);

      SET_FLAG(ch->comm, COMM_VBLOOD);
    }
    if (IS_OBJ_STAT(obj, ITEM_POISON) && ch->pcdata->poisontimer == 0) {
      ch->pcdata->poisontimer = number_range(240, 360);
      if (is_undead(ch))
      ch->pcdata->poisontimer = 0;
      else if (ch->race != RACE_CIVILIAN)
      ch->pcdata->poisontimer *= 2;
    }
    if (IS_OBJ_STAT(obj, ITEM_ROHYPNOL) && ch->pcdata->rohyptimer == 0) {
      ch->pcdata->rohyptimer = number_range(120, 180);
      if (is_undead(ch))
      ch->pcdata->rohyptimer *= 3;
      else if (is_super(ch))
      ch->pcdata->rohyptimer *= 2;
    }
    if (obj->pIndexData->vnum == 405001) {
      if (!IS_FLAG(ch->comm, COMM_MANDRAKE))
      SET_FLAG(ch->comm, COMM_MANDRAKE);
    }

    if (!IS_NPC(ch) && obj->pIndexData->vnum == 85) {
      ch->pcdata->other_power = obj->value[4];
      extract_obj(obj);
      return;
    }
    if (!IS_NPC(ch) && obj->pIndexData->vnum == 86) {
      ch->pcdata->godrealm_power = obj->value[4];
      extract_obj(obj);
      return;
    }
    if (!IS_NPC(ch) && obj->pIndexData->vnum == 87) {
      ch->pcdata->hell_power = obj->value[4];
      extract_obj(obj);
      return;
    }

    if (!IS_NPC(ch) && is_vampire(ch) && !is_name("blood", obj->name))
    ch->pcdata->vamp_fullness += 2;

    act("$n chugs $s $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM);
    act("You chug your $p.", ch, obj, liq_table[liquid].liq_name, TO_CHAR);

    bool tastefound = FALSE;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+taste", ed->keyword)) {
        send_to_char(ed->description, ch);
        tastefound = TRUE;
      }
    }
    if (tastefound == FALSE) {
      for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword)) {
          send_to_char(ed->description, ch);
          tastefound = TRUE;
        }
      }
    }

    for (; obj->value[1] > 0; obj->value[1]--) {
      if (!IS_NPC(ch) && liq_table[obj->value[2]].liq_affect[COND_DRUNK] > 1) {
        if(college_group(ch, FALSE) == COLLEGE_BADKID)
        {
          ch->pcdata->conditions[COND_DRUNK] +=
          liq_table[obj->value[2]].liq_affect[COND_DRUNK];
          ch->pcdata->atolerance += liq_table[obj->value[2]].liq_affect[COND_DRUNK];
        }
        else
        {
          ch->pcdata->conditions[COND_DRUNK] +=
          liq_table[obj->value[2]].liq_affect[COND_DRUNK]*3/2;
          ch->pcdata->atolerance += liq_table[obj->value[2]].liq_affect[COND_DRUNK];

        }
      }
    }

    if (liq_table[obj->value[2]].liq_affect[COND_DRUNK] == 2)
    send_to_char("You taste a small amount of alcohol.\n\r", ch);
    else if (liq_table[obj->value[2]].liq_affect[COND_DRUNK] == 10)
    send_to_char("You taste a large amount of alcohol.\n\r", ch);

    if (obj->value[0] > 0)
    obj->value[1] = UMAX(0, obj->value[1] - 1);

    if (obj->item_type == ITEM_FOUNTAIN) {
      if (obj->value[1] == 0) {
        act("$a $p dries up.", ch, obj, NULL, TO_ALL);
        obj_from_room(obj);
        extract_obj(obj);
      }
    }
    else if (obj->value[1] == 0) {
      extract_obj(obj);
    }

    if (!IS_NPC(ch) && ch->hit >= max_hp(ch) && ch->pcdata->fatigue > 100 && ch->pcdata->fatigue_temp < 100) {
      ch->pcdata->fatigue -= 50;
      ch->pcdata->fatigue_temp += 5;
    }

    if (!IS_NPC(ch))
    ch->pcdata->energy_recovery += 10;

    return;
  }

  _DOFUN(do_eat) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXTRA_DESCR_DATA *ed;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Eat what?\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0)
    return;

    if ((obj = get_obj_here(ch, NULL, arg)) == NULL) {
      act("I see no $T here.", ch, NULL, arg, TO_CHAR);
      return;
    }

    if (!IS_IMMORTAL(ch)) {
      if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_DRUGS) {
        if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg, obj->name) || (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_DRUGS)) {
          send_to_char("That's not edible.\n\r", ch);
          return;
        }
      }
    }

    if (obj->item_type == ITEM_DRUGS) {
      EXTRA_DESCR_DATA *ed;
      act("$n eats $s $p.", ch, obj, NULL, TO_ROOM);
      act("You eat your $p.", ch, obj, NULL, TO_CHAR);

      bool tastefound = FALSE;
      bool imprintfound = FALSE;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword)) {
          send_to_char(ed->description, ch);
          tastefound = TRUE;
        }
        if (is_name("+imprint", ed->keyword)) {
          auto_imprint(ch, ed->description, IMPRINT_DRUGS);
          imprintfound = TRUE;
        }
      }
      for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword) && !tastefound) {
          send_to_char(ed->description, ch);
          tastefound = TRUE;
        }
        if (is_name("+imprint", ed->keyword) && !imprintfound) {
          auto_imprint(ch, ed->description, IMPRINT_DRUGS);
          imprintfound = TRUE;
        }
      }

      extract_obj(obj);
      return;
    }

    if (obj->item_type == ITEM_FOOD) {
      if (IS_OBJ_STAT(obj, ITEM_VBLOOD) && !IS_FLAG(ch->comm, COMM_VBLOOD)) {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = 0;
        af.level = 10;
        af.duration = 60;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.caster = NULL;
        af.weave = FALSE;
        af.bitvector = AFF_VBLOOD;
        affect_to_char(ch, &af);

        send_to_char("You start to feel energized, aroused and powerful as the vampire blood hits your system.\n\r", ch);

        SET_FLAG(ch->comm, COMM_VBLOOD);
      }
      if (IS_OBJ_STAT(obj, ITEM_POISON) && ch->pcdata->poisontimer == 0) {
        ch->pcdata->poisontimer = number_range(240, 360);
        if (is_undead(ch))
        ch->pcdata->poisontimer = 0;
        else if (is_super(ch))
        ch->pcdata->poisontimer *= 2;
      }
      if (IS_OBJ_STAT(obj, ITEM_ROHYPNOL) && ch->pcdata->rohyptimer == 0) {
        ch->pcdata->rohyptimer = number_range(240, 360);
        if (is_undead(ch))
        ch->pcdata->rohyptimer *= 3;
        else if (is_super(ch))
        ch->pcdata->rohyptimer *= 2;
      }

      obj->value[0]--;
      if (obj->value[0] > 0) {

        act("$n eats some of $s $p.", ch, obj, NULL, TO_ROOM);
        act("You eat some of your $p.", ch, obj, NULL, TO_CHAR);

        if (!IS_NPC(ch) && is_vampire(ch))
        ch->pcdata->vamp_fullness += 2;

        bool tastefound = FALSE;
        for (ed = obj->extra_descr; ed; ed = ed->next) {
          if (is_name("+taste", ed->keyword)) {
            send_to_char(ed->description, ch);
            tastefound = TRUE;
          }
        }
        if (tastefound == FALSE) {
          for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
            if (is_name("+taste", ed->keyword)) {
              send_to_char(ed->description, ch);
              tastefound = TRUE;
            }
          }
        }
        return;
      }

      if (!IS_NPC(ch) && is_vampire(ch))
      ch->pcdata->vamp_fullness += 2;

      act("$n finishes $s $p.", ch, obj, NULL, TO_ROOM);
      act("You finish your $p.", ch, obj, NULL, TO_CHAR);
      bool tastefound = FALSE;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword)) {
          send_to_char(ed->description, ch);
          tastefound = TRUE;
        }
      }
      if (tastefound == FALSE) {
        for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
          if (is_name("+taste", ed->keyword)) {
            send_to_char(ed->description, ch);
            tastefound = TRUE;
          }
        }
      }

      if (!IS_NPC(ch) && ch->hit >= max_hp(ch) && ch->pcdata->fatigue > 100 && ch->pcdata->fatigue_temp < 100) {
        ch->pcdata->fatigue -= 50;
        ch->pcdata->fatigue_temp += 5;
      }

      if (!IS_NPC(ch))
      ch->pcdata->energy_recovery += 10;
      extract_obj(obj);
    }
    return;
  }

  _DOFUN(do_devour) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXTRA_DESCR_DATA *ed;

    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0)
    return;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
      send_to_char("Eat what?\n\r", ch);
      return;
    }

    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg, obj->name))
    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
      send_to_char("You do not have that item.\n\r", ch);
      return;
    }

    if (!IS_IMMORTAL(ch)) {
      if (obj->item_type != ITEM_FOOD) {
        if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL || !is_name(arg, obj->name) || obj->item_type != ITEM_FOOD) {
          send_to_char("That's not edible.\n\r", ch);
          return;
        }
      }
    }

    if (IS_OBJ_STAT(obj, ITEM_VBLOOD) && !IS_FLAG(ch->comm, COMM_VBLOOD)) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 60;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_VBLOOD;
      affect_to_char(ch, &af);

      send_to_char("You start to feel energized, aroused and powerful as the vampire blood hits your system.\n\r", ch);

      SET_FLAG(ch->comm, COMM_VBLOOD);
    }
    if (IS_OBJ_STAT(obj, ITEM_POISON) && ch->pcdata->poisontimer == 0) {
      ch->pcdata->poisontimer = number_range(240, 360);
      if (is_undead(ch))
      ch->pcdata->poisontimer = 0;
      else if (is_super(ch))
      ch->pcdata->poisontimer *= 2;
    }
    if (IS_OBJ_STAT(obj, ITEM_ROHYPNOL) && ch->pcdata->rohyptimer == 0) {
      ch->pcdata->rohyptimer = number_range(240, 360);
      if (is_undead(ch))
      ch->pcdata->rohyptimer *= 3;
      else if (is_super(ch))
      ch->pcdata->rohyptimer *= 2;
    }

    if (!IS_NPC(ch) && is_vampire(ch))
    ch->pcdata->vamp_fullness += 5;

    act("$n eats $s $p.", ch, obj, NULL, TO_ROOM);
    act("You eat your $p.", ch, obj, NULL, TO_CHAR);
    bool tastefound = FALSE;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+taste", ed->keyword)) {
        send_to_char(ed->description, ch);
        tastefound = TRUE;
      }
    }
    if (tastefound == FALSE) {
      for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword)) {
          send_to_char(ed->description, ch);
          tastefound = TRUE;
        }
      }
    }

    if (!IS_NPC(ch) && ch->hit >= max_hp(ch) && ch->pcdata->fatigue > 100 && ch->pcdata->fatigue_temp < 100) {
      ch->pcdata->fatigue -= 100;
      ch->pcdata->fatigue_temp += 10;
    }

    if (!IS_NPC(ch))
    ch->pcdata->energy_recovery += 10;

    extract_obj(obj);
    return;
  }

  /*
  * Remove an object.
  */
  bool remove_obj(CHAR_DATA *ch, int iWear, bool fReplace) {
    OBJ_DATA *obj;
    OBJ_DATA *pocketobj;
    bool hasjacket = FALSE;
    bool haspants = FALSE;

    if (iWear == WEAR_HOLD) {
      if ((obj = get_eqr_char(ch, WEAR_HOLD)) == NULL) {
        if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) == NULL)
        return TRUE;
      }

    }
    else if ((obj = get_eq_char(ch, iWear)) == NULL)
    return TRUE;

    if (!fReplace)
    return FALSE;

    if (obj->item_type == ITEM_KEY && ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && in_haven(get_room_index(ch->pcdata->travel_to)) && in_haven(get_room_index(ch->pcdata->travel_from)) && in_haven(ch->your_car->in_room))
    return TRUE;

    if (IS_FLAG(ch->act, PLR_BOUND) && (strcasestr(obj->name, "handcuffs") != NULL || strcasestr(obj->name, "hand-cuffs") != NULL || strcasestr(obj->name, "straightjacket") != NULL || strcasestr(obj->name, "straight-jacket") != NULL || strcasestr(obj->name, "rope") != NULL))
    return TRUE;

    if (IS_SET(obj->extra_flags, ITEM_CURSED) && (!is_helpless(ch) && !is_possessed(ch))) {
      EXTRA_DESCR_DATA *ed;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_CURSED) {
              ch->pcdata->imprint_type[i] = 0;
            }
          }
        }
        if (is_name("+bimprint", ed->keyword)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_BCURSED) {
              ch->pcdata->imprint_type[i] = 0;
            }
          }
        }

      }
    }

    if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && ch->hit > 0) {
      act("You put $a $p down.", ch, obj, NULL, TO_CHAR);
      act("$n puts $a $p down.", ch, obj, NULL, TO_ROOM);
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
      return FALSE;
    }
    if (is_big(obj)) {
      if (get_big_items(ch) + obj->size > MAX_BIG_ITEMS && ch->hit > 0 && !battleground(ch->in_room)) {
        send_to_char("There's no space for that on your back, so you drop it.\n\r", ch);
        act("$n drops $a $p.", ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        return FALSE;
      }
    }
    else {
      if (get_small_items(ch) + obj->size > MAX_SMALL_ITEMS && ch->hit > 0 && !battleground(ch->in_room)) {
        send_to_char("There's no space for that in your pockets, so you drop it.\n\r", ch);
        act("$n drops $a $p.", ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        return FALSE;
      }
    }

    pocketobj = get_jacket(ch);
    if (pocketobj == NULL || pocketobj->item_type != ITEM_CLOTHING) {
      pocketobj = get_pants(ch);
      if (pocketobj != NULL && pocketobj->item_type == ITEM_CLOTHING)
      haspants = TRUE;
    }
    else
    hasjacket = TRUE;

    if (obj->wear_loc != WEAR_NONE)
    unequip_char(ch, obj);

    if (!crowded_room(ch->in_room) && !in_fight(ch)) {
      if (iWear == WEAR_HOLD) {
        if (is_big(obj)) {
          if (obj->item_type == ITEM_CLOTHING) {
            act("You throw $a $p over your shoulder.", ch, obj, NULL, TO_CHAR);
            act("$n throws $a $p over $s shoulder.", ch, obj, NULL, TO_ROOM);
          }
          else if (obj->item_type == ITEM_WEAPON && obj->value[0] == WEAPON_SWORD) {
            act("You sheath $a $p on your back.", ch, obj, NULL, TO_CHAR);
            act("$n sheath $a $p onto $s back.", ch, obj, NULL, TO_ROOM);
          }
          else {
            act("You strap $a $p on your back.", ch, obj, NULL, TO_CHAR);
            act("$n straps $a $p onto $s back.", ch, obj, NULL, TO_ROOM);
          }
        }
        else {
          if (obj->item_type == ITEM_WEAPON) {
            act("You slip $a $p into a hidden sheath.", ch, obj, NULL, TO_CHAR);
            act("$n slips $a $p into a hidden sheath.", ch, obj, NULL, TO_ROOM);
          }
          else if (hasjacket) {
            act("You slip $a $p into $A $P pocket.", ch, obj, pocketobj, TO_CHAR);
            act("$n slips $a $p into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
          }
          else if (haspants) {
            act("You slip $a $p into $A $P pocket.", ch, obj, pocketobj, TO_CHAR);
            act("$n slips $a $p into $A $P pocket.", ch, obj, pocketobj, TO_ROOM);
          }
          else {
            act("You slip $a $p into a pocket.", ch, obj, NULL, TO_CHAR);
            act("$n slips $a $p into a pocket.", ch, obj, NULL, TO_ROOM);
          }
        }
      }
      else {
        if (is_big(obj)) {
          if (obj->item_type == ITEM_CLOTHING) {
            act("You remove $a $p and throw it over your shoulder.", ch, obj, NULL, TO_CHAR);
            act("$n removes $a $p and throws it over $s shoulder.", ch, obj, NULL, TO_ROOM);
          }
          else {
            act("You remove $a $p and strap it on your back.", ch, obj, NULL, TO_CHAR);
            act("$n removes $a $p and straps it onto $s back.", ch, obj, NULL, TO_ROOM);
          }
        }
        else {
          if (hasjacket) {
            act("You remove $a $p and slip it into $A $P pocket.", ch, obj, pocketobj, TO_CHAR);
            act("$n removes $a $p and slips it into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
          }
          else if (haspants) {
            act("You remove $a $p and slip it into $A $P pocket.", ch, obj, pocketobj, TO_CHAR);
            act("$n removes $a $p and slips it into $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
          }
          else {
            act("You remove $a $p and slip it into a pocket.", ch, obj, NULL, TO_CHAR);
            act("$n removes $a $p and slips it into a pocket.", ch, obj, NULL, TO_ROOM);
          }
        }
      }
    }

    return TRUE;
  }

  /*
  * Wear one object.
  * Optional replacement of existing objects.
  * Big repetitive code, ick.
  */
  void wear_obj(CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool silent) {
    OBJ_DATA *pocketobj;
    bool hasjacket = FALSE;
    bool haspants = FALSE;

    if (ch->in_room != NULL && (crowded_room(ch->in_room) || in_fight(ch)))
    silent = TRUE;

    pocketobj = get_jacket(ch);
    if (pocketobj == NULL || pocketobj->item_type != ITEM_CLOTHING) {
      pocketobj = get_pants(ch);
      if (pocketobj != NULL && pocketobj->item_type == ITEM_CLOTHING)
      haspants = TRUE;
    }
    else
    hasjacket = TRUE;

    if (!IS_NPC(ch) && ch->pcdata->wetness > 0 && obj->item_type == ITEM_CLOTHING) {
      obj->value[4] = UMAX(obj->value[4], ch->pcdata->wetness);
    }

    if (IS_SET(obj->extra_flags, ITEM_CURSED) && !is_helpless(ch) && !is_possessed(ch)) {
      EXTRA_DESCR_DATA *ed;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword)) {
          bool found = FALSE;
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->imprint[i], ed->description)) {
              found = TRUE;
            }
          }
          if (found == FALSE) {
            auto_imprint(ch, ed->description, IMPRINT_CURSED);
          }
        }

        if (is_name("+bimprint", ed->keyword)) {
          bool found = FALSE;
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(ch->pcdata->imprint[i], ed->description)) {
              found = TRUE;
            }
          }
          if (found == FALSE) {
            auto_imprint(ch, ed->description, IMPRINT_BCURSED);
          }
        }
      }
    }

    if (CAN_WEAR(obj, ITEM_HOLD) && get_eqr_char(ch, WEAR_HOLD) != NULL) {
      if (get_eqr_char(ch, WEAR_HOLD_2) == NULL) {
        OBJ_DATA *mainhand = get_eqr_char(ch, WEAR_HOLD);
        unequip_char_silent(ch, mainhand);
        equip_char_silent(ch, mainhand, WEAR_HOLD_2);
      }
      else {
        OBJ_DATA *mainhand = get_eqr_char(ch, WEAR_HOLD);
        OBJ_DATA *offhand = get_eqr_char(ch, WEAR_HOLD_2);
        unequip_char_silent(ch, mainhand);
        unequip_char_silent(ch, offhand);
        equip_char_silent(ch, mainhand, WEAR_HOLD_2);
      }
    }

    if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && CAN_WEAR(obj, ITEM_WEAR_HOLD)) {
      if (get_eqr_char(ch, WEAR_HOLD) != NULL && get_eqr_char(ch, WEAR_HOLD_2) != NULL && !remove_obj(ch, WEAR_HOLD, fReplace) && !remove_obj(ch, WEAR_HOLD_2, fReplace))
      return;
      if (get_eqr_char(ch, WEAR_HOLD) == NULL) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_HOLD);
        }
        else {
          equip_char(ch, obj, WEAR_HOLD);
        }
      }
      else {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_HOLD_2);
        }
        else {
          equip_char(ch, obj, WEAR_HOLD_2);
        }
      }
      return;
    }

    if (CAN_WEAR(obj, ITEM_HOLD)) {
      if (get_eqr_char(ch, WEAR_HOLD) != NULL && get_eqr_char(ch, WEAR_HOLD_2) != NULL && !remove_obj(ch, WEAR_HOLD, fReplace) && !remove_obj(ch, WEAR_HOLD_2, fReplace))
      return;

      if (get_eqr_char(ch, WEAR_HOLD) == NULL || get_eqr_char(ch, WEAR_HOLD_2) == NULL) {
        if (is_big(obj)) {
          if (silent == FALSE) {
            if (obj->item_type == ITEM_WEAPON && obj->value[0] == WEAPON_SWORD) {
              act("$n unsheathes $s $p.", ch, obj, NULL, TO_ROOM);
              act("You unsheathe your $p.", ch, obj, NULL, TO_CHAR);
            }
            else if (obj->item_type == ITEM_WEAPON) {
              act("$n wields $s $p.", ch, obj, NULL, TO_ROOM);
              act("You wield your $p.", ch, obj, NULL, TO_CHAR);
            }
            else {
              act("$n pulls $a $p off $s back.", ch, obj, NULL, TO_ROOM);
              act("You pull $a $p off your back.", ch, obj, NULL, TO_CHAR);
            }
          }
        }
        else {
          if (silent == FALSE) {
            if (obj->item_type == ITEM_WEAPON) {
              act("$n unsheathes $s $p.", ch, obj, NULL, TO_ROOM);
              act("You unsheathe your $p.", ch, obj, NULL, TO_CHAR);
            }
            else if (obj->item_type == ITEM_WEAPON) {
              act("$n wields $a $p.", ch, obj, NULL, TO_ROOM);
              act("You wield $a $p.", ch, obj, NULL, TO_CHAR);
            }
            else if (hasjacket) {
              act("You get $a $p from $A $P pocket.", ch, obj, pocketobj, TO_CHAR);
              act("$n gets $a $p from $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
            }
            else if (haspants) {
              act("You get $a $p from $A $P pocket.", ch, obj, pocketobj, TO_CHAR);
              act("$n gets $a $p from $s $P pocket.", ch, obj, pocketobj, TO_ROOM);
            }
            else {
              act("You get $a $p from a pocket.", ch, obj, NULL, TO_CHAR);
              act("$n gets $a $p from a pocket.", ch, obj, NULL, TO_ROOM);
            }
          }
        }
        if (get_eqr_char(ch, WEAR_HOLD) == NULL) {
          if (silent == TRUE) {
            equip_char_silent(ch, obj, WEAR_HOLD);
          }
          else {
            equip_char(ch, obj, WEAR_HOLD);
          }
        }
        else {
          if (silent == TRUE) {
            equip_char_silent(ch, obj, WEAR_HOLD_2);
          }
          else {
            equip_char(ch, obj, WEAR_HOLD_2);
          }
        }
        return;
      }

      bug("Wear_obj: no free hold.", 0);
      send_to_char("You already wear a hold item.\n\r", ch);
      return;
    }

    if (CAN_WEAR(obj, ITEM_WEAR_BODY)) {
      if (get_eq_char(ch, WEAR_BODY_1) != NULL && get_eq_char(ch, WEAR_BODY_2) != NULL && get_eq_char(ch, WEAR_BODY_3) != NULL && get_eq_char(ch, WEAR_BODY_4) != NULL && get_eq_char(ch, WEAR_BODY_5) != NULL && get_eq_char(ch, WEAR_BODY_6) != NULL && get_eq_char(ch, WEAR_BODY_7) != NULL && get_eq_char(ch, WEAR_BODY_8) != NULL && get_eq_char(ch, WEAR_BODY_9) != NULL && get_eq_char(ch, WEAR_BODY_10) != NULL && get_eq_char(ch, WEAR_BODY_11) != NULL && get_eq_char(ch, WEAR_BODY_12) != NULL && get_eq_char(ch, WEAR_BODY_13) != NULL && get_eq_char(ch, WEAR_BODY_14) != NULL && get_eq_char(ch, WEAR_BODY_15) != NULL && get_eq_char(ch, WEAR_BODY_16) != NULL && get_eq_char(ch, WEAR_BODY_17) != NULL && get_eq_char(ch, WEAR_BODY_18) != NULL && get_eq_char(ch, WEAR_BODY_19) != NULL && get_eq_char(ch, WEAR_BODY_20) != NULL && get_eq_char(ch, WEAR_BODY_21) != NULL && get_eq_char(ch, WEAR_BODY_22) != NULL && get_eq_char(ch, WEAR_BODY_23) != NULL && get_eq_char(ch, WEAR_BODY_24) != NULL && get_eq_char(ch, WEAR_BODY_25) != NULL && !remove_obj(ch, WEAR_BODY_1, fReplace) && !remove_obj(ch, WEAR_BODY_2, fReplace) && !remove_obj(ch, WEAR_BODY_3, fReplace) && !remove_obj(ch, WEAR_BODY_4, fReplace) && !remove_obj(ch, WEAR_BODY_5, fReplace) && !remove_obj(ch, WEAR_BODY_6, fReplace) && !remove_obj(ch, WEAR_BODY_7, fReplace) && !remove_obj(ch, WEAR_BODY_8, fReplace) && !remove_obj(ch, WEAR_BODY_9, fReplace) && !remove_obj(ch, WEAR_BODY_10, fReplace) && !remove_obj(ch, WEAR_BODY_11, fReplace) && !remove_obj(ch, WEAR_BODY_12, fReplace) && !remove_obj(ch, WEAR_BODY_13, fReplace) && !remove_obj(ch, WEAR_BODY_14, fReplace) && !remove_obj(ch, WEAR_BODY_15, fReplace) && !remove_obj(ch, WEAR_BODY_16, fReplace) && !remove_obj(ch, WEAR_BODY_17, fReplace) && !remove_obj(ch, WEAR_BODY_18, fReplace) && !remove_obj(ch, WEAR_BODY_19, fReplace) && !remove_obj(ch, WEAR_BODY_20, fReplace) && !remove_obj(ch, WEAR_BODY_21, fReplace) && !remove_obj(ch, WEAR_BODY_22, fReplace) && !remove_obj(ch, WEAR_BODY_23, fReplace) && !remove_obj(ch, WEAR_BODY_24, fReplace) && !remove_obj(ch, WEAR_BODY_25, fReplace))
      return;

      if (get_eq_char(ch, WEAR_BODY_1) == NULL && (obj->layer == 1 || (obj->layer == 0 && (obj->level == 1 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_1);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_1);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_2) == NULL && (obj->layer == 1 || (obj->layer == 0 && (obj->level == 1 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_2);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_2);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_3) == NULL && (obj->layer == 1 || (obj->layer == 0 && (obj->level == 1 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_3);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_3);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_4) == NULL && (obj->layer == 1 || (obj->layer == 0 && (obj->level == 1 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_4);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_4);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_5) == NULL && (obj->layer == 1 || (obj->layer == 0 && (obj->level == 1 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_5);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_5);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_6) == NULL && (obj->layer == 2 || (obj->layer == 0 && (obj->level == 2 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_6);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_6);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_7) == NULL && (obj->layer == 2 || (obj->layer == 0 && (obj->level == 2 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_7);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_7);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_8) == NULL && (obj->layer == 2 || (obj->layer == 0 && (obj->level == 2 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_8);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_8);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_9) == NULL && (obj->layer == 2 || (obj->layer == 0 && (obj->level == 2 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_9);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_9);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_10) == NULL && (obj->layer == 2 || (obj->layer == 0 && (obj->level == 2 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_10);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_10);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_11) == NULL && (obj->layer == 3 || (obj->layer == 0 && (obj->level == 3 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_11);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_11);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_12) == NULL && (obj->layer == 3 || (obj->layer == 0 && (obj->level == 3 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_12);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_12);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_13) == NULL && (obj->layer == 3 || (obj->layer == 0 && (obj->level == 3 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_13);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_13);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_14) == NULL && (obj->layer == 3 || (obj->layer == 0 && (obj->level == 3 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_14);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_14);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_15) == NULL && (obj->layer == 3 || (obj->layer == 0 && (obj->level == 3 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_15);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_15);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_16) == NULL && (obj->layer == 4 || (obj->layer == 0 && (obj->level == 4 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_16);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_16);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_17) == NULL && (obj->layer == 4 || (obj->layer == 0 && (obj->level == 4 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_17);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_17);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_18) == NULL && (obj->layer == 4 || (obj->layer == 0 && (obj->level == 4 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_18);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_18);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_19) == NULL && (obj->layer == 4 || (obj->layer == 0 && (obj->level == 4 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_19);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_19);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_20) == NULL && (obj->layer == 4 || (obj->layer == 0 && (obj->level == 4 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_20);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_20);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_21) == NULL && (obj->layer == 5 || (obj->layer == 0 && (obj->level == 5 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_21);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_21);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_22) == NULL && (obj->layer == 5 || (obj->layer == 0 && (obj->level == 5 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_22);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_22);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_23) == NULL && (obj->layer == 5 || (obj->layer == 0 && (obj->level == 5 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_23);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_23);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_24) == NULL && (obj->layer == 5 || (obj->layer == 0 && (obj->level == 5 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_24);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_24);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }
      if (get_eq_char(ch, WEAR_BODY_25) == NULL && (obj->layer == 5 || (obj->layer == 0 && (obj->level == 5 || obj->level == 0)))) {
        if (silent == TRUE) {
          equip_char_silent(ch, obj, WEAR_BODY_25);
        }
        else {
          equip_char(ch, obj, WEAR_BODY_25);
          act("$n wears $a $p.", ch, obj, NULL, TO_ROOM);
          act("You wear $a $p.", ch, obj, NULL, TO_CHAR);
        }
        return;
      }

      bug("Wear_obj: no free body.", 0);
      send_to_char("You already wear twenty five items.\n\r", ch);
      return;
    }

    if (fReplace)
    send_to_char("You can't wear, wield, or hold that.\n\r", ch);

    return;
  }

  int small_gun_count(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int count = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25)
      count++;
    }
    count = UMAX(0, count - 1);
    return count;
  }
  int small_melee_count(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int count = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25)
      count++;
    }
    count = UMAX(0, count - 1);
    return count;
  }

  _DOFUN(do_wear) {
    char arg[MAX_INPUT_LENGTH];
    char arg2[MSL];
    char buf[MSL];
    // char temp[MSL];
    OBJ_DATA *obj;

    smash_vector(argument);

    if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_wear);
        ch->pcdata->ci_wear = str_dup(argument);
        // send_to_char("Done.\n\r", ch);
      }
      else {
        free_string(ch->pcdata->ci_wear);
        ch->pcdata->ci_wear = str_dup("");
        // send_to_char("Wear (A short description of where the object is worn)\n\r", ch);
      }
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 1;
      return;
    }

    argument = one_argument_nouncap(argument, arg);
    argument = one_argument_nouncap(argument, arg2);

    if (arg[0] == '\0') {
      send_to_char("Wear, wield, or hold what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD) || is_ghost(ch)) {
      send_to_char("You are as the day you died.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "all")) {
      if (ch->hit < 0) {
        return;
      }

      OBJ_DATA *obj_next;
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
          continue;
        }
        if (room_hostile(ch->in_room) && IS_SET(obj->extra_flags, ITEM_ARMORED)) {
          continue;
        }

        if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj)) {
          if (strcasestr(obj->name, "blindfold")) {
            if (!IS_FLAG(ch->comm, COMM_BLINDFOLD)) {
              SET_FLAG(ch->comm, COMM_BLINDFOLD);
            }
          }

          wear_obj(ch, obj, FALSE, TRUE);
        }

        if (under_opression(ch) && obj->item_type == ITEM_CLOTHING && number_percent() % 10 == 0) {
          act("$p abruptly tears.", ch, obj, NULL, TO_CHAR);
          act("$p abruptly tears.", ch, obj, NULL, TO_ROOM);
          extract_obj(obj);
        }
      }
      return;
    }
    else {
      bool found = false;
      if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
        if (get_eqr_char(ch, WEAR_HOLD_2) != NULL && is_name(arg, get_eqr_char(ch, WEAR_HOLD_2)->name)) {
          unequip_char_silent(ch, get_eqr_char(ch, WEAR_HOLD_2));

          if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
            send_to_char("You do not have that item.\n\r", ch);
            return;
          }
        }
        else {
          // this looks for worn items to adjust the temp wear string or layer
          int worn;
          for (worn = 0; worn < MAX_WEAR; worn++) {
            for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
              if (obj->wear_loc != worn || (!can_see_obj(ch, obj))) {
                continue;
              }
              if (!is_name(arg, obj->name)) {
                continue;
              }

              found = true;
              if (is_number(arg2) && arg2[0] != '\0' && atoi(arg2) >= 0 && atoi(arg2) <= 5) {
                obj->layer = atoi(arg2);
              }
              else {
                send_to_char("You need to specify a layer.\n\r", ch);
                return;
              }

              if (argument[0] != ',') {
                sprintf(buf, "%s", argument);
              }
              else if (argument[0] != '\0') {
                sprintf(buf, " %s", argument);
              }
              else {
                strcpy(buf, " ");
              }

              free_string(obj->wear_temp);
              obj->wear_temp = str_dup(buf);

              send_to_char("You adjust your clothes.\n\r", ch);
              return;
            }
          }

          if (found == FALSE) {
            send_to_char("You do not have that item.\n\r", ch);
            return;
          }
          else {
            if (argument[0] == '\0') {
              send_to_char("You're already wearing that.\n\r", ch);
              return;
            }
          }
        }
      }

      if (room_hostile(ch->in_room) && IS_SET(obj->extra_flags, ITEM_ARMORED)) {
        start_hostilefight(ch);
      }

      if (obj->pIndexData->vnum == 75 || obj->pIndexData->vnum == ITEM_NEUTRALCOLLAR) {
        bool objectfound = FALSE;
        OBJ_DATA *obj2;
        for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
          if ((obj2 = get_eq_char(ch, iWear)) == NULL)
          continue;

          if (obj2->pIndexData->vnum == 75 || obj2->pIndexData->vnum == ITEM_NEUTRALCOLLAR) {
            if (obj2 != obj) {
              objectfound = TRUE;
              break;
            }
          }
        }
        if (objectfound) {
          send_to_char("Only one collar can be worn at a time.\n\r", ch);
          return;
        }
      }

      if (in_fight(ch) && obj->item_type == ITEM_CLOTHING) {
        nomove(ch);
      }
      if (in_fight(ch) && obj->item_type == ITEM_WEAPON && obj->size < 25) {
        ch->debuff += small_melee_count(ch) * 2;
      }
      if (in_fight(ch) && obj->item_type == ITEM_RANGED && obj->size < 25) {
        ch->debuff += small_gun_count(ch) * 2;
      }
      if (in_fight(ch) && obj->pIndexData->vnum == ITEM_GASMASK) {
        useattack(ch);
      }

      if (is_number(arg2) && arg2[0] != '\0' && atoi(arg2) >= 0 && atoi(arg2) <= 5) {
        obj->layer = atoi(arg2);
      }
      if (argument[0] != '\0') {
        sprintf(buf, "%s", argument);
        // sprintf(temp, "%-21.21s", buf);
        free_string(obj->wear_temp);
        obj->wear_temp = str_dup(buf);
      }

      if (is_name("blindfold", obj->name)) {
        if (!IS_FLAG(ch->comm, COMM_BLINDFOLD)) {
          SET_FLAG(ch->comm, COMM_BLINDFOLD);
        }
      }

      wear_obj(ch, obj, TRUE, FALSE);

      if (under_opression(ch) && obj->item_type == ITEM_CLOTHING && number_percent() % 10 == 0) {
        act("$p abruptly tears.", ch, obj, NULL, TO_CHAR);
        act("$p abruptly tears.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
      }
    }

    return;
  }

  _DOFUN(do_remove) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int toremove;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Remove what?\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD)) {
      send_to_char("You are as the day you died.\n\r", ch);
      return;
    }

    if (!strcmp(arg, "all")) {
      if (in_fight(ch)) {
        send_to_char("You're a bit busy for a strip show.\n\r", ch);
        return;
      }
      if (ch->hit < 0) {
        return;
      }

      for (toremove = 0; toremove < MAX_WEAR; toremove++) {
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (obj->wear_loc == toremove && (can_see_obj(ch, obj)) && !IS_SET(obj->extra_flags, ITEM_NOOUTFIT)) {
            remove_obj(ch, obj->wear_loc, TRUE);
          }
        }
      }
      // DISABLED - Check to see if character is leaving a computer - Discordance
      /*
      if(access_internet(ch) == FALSE){
        if(ch->pcdata->chatroom > 0){
          do_function(ch, &do_chatroom, "logoff");
        }
      }
      */
      return;
    }

    if ((obj = get_obj_wear(ch, arg, TRUE)) == NULL) {
      send_to_char("You do not have that item.\n\r", ch);
      return;
    }
    if (IS_SET(obj->extra_flags, ITEM_LOCK)) {
      send_to_char("You can't remove it.  It's locked.\n\r", ch);
      return;
    }
    if (is_helpless(ch) && obj->pIndexData->vnum == ITEM_NEUTRALCOLLAR) {
      send_to_char("You can't remove that while bound.\n\r", ch);
      return;
    }
    if (is_prisoner(ch) && obj->pIndexData->vnum == ITEM_NEUTRALCOLLAR) {
      send_to_char("The guards won't let you do that.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      if (IS_SET(obj->extra_flags, ITEM_ARMORED) || (obj->item_type == ITEM_CLOTHING && obj->value[2] > 0)) {
        send_to_char("You can't take that off in the middle of a fight, try dropping it instead.\n\r", ch);
        return;
      }
    }
    /*
    if (!IS_NPC(ch) && (ch->pcdata->patrol_status == PATROL_INIT_PREY || ch->pcdata->patrol_status == PATROL_GRABBED || ch->pcdata->patrol_status == PATROL_FLEEING || ch->pcdata->patrol_status == PATROL_HIDING)) {
      if (IS_SET(obj->extra_flags, ITEM_ARMORED) || (obj->item_type == ITEM_CLOTHING && obj->value[2] > 0)) {
        send_to_char("You can't take that off in the middle of an event, try dropping it instead.\n\r", ch);
        return;
      }
    }
    */

    if (is_name("blindfold", obj->name)) {
      if (IS_FLAG(ch->comm, COMM_BLINDFOLD)) {
        REMOVE_FLAG(ch->comm, COMM_BLINDFOLD);
      }
    }

    remove_obj(ch, obj->wear_loc, TRUE);
    // DISABLED - Check to see if character is removing a computer - Discordance
    /*
    if(access_internet(ch) == FALSE){
      if(ch->pcdata->chatroom > 0){
        do_function(ch, &do_chatroom, "logoff");
      }
    }
    */
    return;
  }

  bool trade_good(OBJ_DATA *obj) {
    if (IS_SET(obj->extra_flags, ITEM_ALTERED))
    return FALSE;
    if (is_name("coin", obj->name))
    return TRUE;
    if (is_name("currency", obj->name))
    return TRUE;
    return FALSE;
  }

  _DOFUN(do_buy) {
    char buf[MAX_STRING_LENGTH];
    int cost;
    char arg1[MSL];

    CHAR_DATA *keeper;
    OBJ_DATA *obj, *t_obj;
    char arg[MAX_INPUT_LENGTH];
    int ref = 0;
    int count = 1, bcnt = 1;
    char number[MIL];
    OBJ_DATA *lobj;
    bool found = FALSE;

    if ((!higher_power(ch) && !is_corporeal(ch)) || IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You are ignored.\n\r", ch);
      return;
    }

    if (institute_room(ch->in_room) && clinic_patient(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (argument[0] == '\0') {
      send_to_char("Buy what?\n\r", ch);
      return;
    }

    if ((keeper = find_keeper(ch)) == NULL)
    return;

    obj = NULL;

    argument = one_argument(argument, arg1);

    if (is_number(arg1)) {
      for (lobj = keeper->carrying; lobj; lobj = lobj->next_content) {
        if (lobj->wear_loc == WEAR_NONE && can_see_obj(ch, lobj) && (cost = get_cost(keeper, lobj, TRUE)) > 0) {
          ref++;
          if (ref == atoi(arg1)) {
            obj = lobj;
            found = TRUE;
          }
        }
      }
    }
    if (!found && obj != NULL) {
      argument = one_argument(argument, number);

      if (is_number(number)) {
        bcnt = atoi(number);
        argument = one_argument(argument, arg);
        obj = get_obj_keeper(ch, keeper, arg);
      }
      else
      obj = get_obj_keeper(ch, keeper, number);

      if (obj == NULL) {
        act("$n tells you 'I don't sell that -- try 'list''.", keeper, NULL, ch, TO_VICT);
        return;
      }
      cost = get_cost(keeper, obj, TRUE);

      if (cost <= 0 || !can_see_obj(ch, obj)) {
        act("$n tells you 'I don't sell that -- try 'list''.", keeper, NULL, ch, TO_VICT);
        return;
      }
    }

    if (obj == NULL) {
      act("$n tells you 'I don't sell that -- try 'list''.", keeper, NULL, ch, TO_VICT);
      return;
    }
    cost = get_cost(keeper, obj, TRUE);

    if (!IS_OBJ_STAT(obj, ITEM_INVENTORY)) {
      for (t_obj = obj->next_content; count < bcnt && t_obj != NULL;
      t_obj = t_obj->next_content) {
        if (t_obj->pIndexData == obj->pIndexData && !str_cmp(t_obj->short_descr, obj->short_descr))
        count++;
        else
        break;
      }

      if (count < bcnt) {
        act("$n tells you 'I don't have that many in stock.", keeper, NULL, ch, TO_VICT);
        ch->reply = keeper;
        return;
      }
    }
    if (trade_good(obj)) {
      if (ch->money < cost * bcnt) {
        act("$n tells you 'You can't afford to buy that.", keeper, obj, ch, TO_VICT);
        return;
      }
    }
    bool has_color = FALSE;
    if(strcasestr(obj->short_descr, "`") != NULL || strcasestr(obj->description, "`") != NULL)
    has_color = TRUE;
    if (!is_gm(ch)) {
      if (has_color == TRUE) {
        if ((ch->money + ch->pcdata->total_money < cost * bcnt)) {
          if (bcnt > 1)
          act("$n tells you 'You can't afford to buy that many.", keeper, obj, ch, TO_VICT);
          else
          act("$n tells you 'You can't afford to buy $p'.", keeper, obj, ch, TO_VICT);
          return;
        }
      }
      else if (cost > 500 && !IS_AFFECTED(ch, AFF_DISCREDIT)) {
        if ((ch->money + ch->pcdata->total_credit + ch->pcdata->total_money <
              cost * bcnt)) {
          if (bcnt > 1)
          act("$n tells you 'You can't afford to buy that many.", keeper, obj, ch, TO_VICT);
          else
          act("$n tells you 'You can't afford to buy $p'.", keeper, obj, ch, TO_VICT);
          return;
        }
      }
      else {
        if ((ch->money + ch->pcdata->total_credit < cost * bcnt)) {
          if (bcnt > 1)
          act("$n tells you 'You can't afford to buy that many.", keeper, obj, ch, TO_VICT);
          else
          act("$n tells you 'You can't afford to buy $p'.", keeper, obj, ch, TO_VICT);
          return;
        }
      }
    }

    if ((obj->item_type == ITEM_DRINK_CON && liq_table[obj->value[2]].liq_affect[COND_DRUNK] > 1)) {
      if ((get_age(ch) < 21 && get_true_age(ch) < 21) || (get_age(ch) < 21 && get_true_age(ch) > 40)) {
        if (!str_cmp(argument, "influence")) {
          if (get_skill(ch, SKILL_PERSUADE) > 0 || get_skill(ch, SKILL_HYPNOTISM) > 1)
          ch->pcdata->influence -= 100;
          else
          ch->pcdata->influence -= 500;
        }
        /*
        else if(get_hour(ch->in_room) < 17 && get_hour(ch->in_room) > 5) {
          send_to_char("The Venetians would be upset.\n\r", ch);
          return;
        }
        */
        else {
          send_to_char("Id Please.\n\r", ch);
          return;
        }
      }
    }

    if (IS_SET(obj->extra_flags, ITEM_NOINVENTORY) && get_eq_char(ch, WEAR_HOLD) != NULL && get_eq_char(ch, WEAR_HOLD_2) != NULL) {
      send_to_char("Your hands are full..\n\r", ch);
      return;
    }
    /*
    if(is_big(obj))
    {
      if(get_big_items(ch) + obj->size > MAX_BIG_ITEMS)
      {
        send_to_char("You can't carry that.\n\r", ch);
        return;
      }
    }
    else
    {
      if(get_small_items(ch) + obj->size > MAX_SMALL_ITEMS)
      {
        send_to_char("There is no more space in your pockets.\n\r", ch);
        return;
      }
    }
    */

    /* haggle */
    double price = (double)(cost);
    price /= 100;
    if (bcnt > 1) {
      sprintf(buf, "$n buys %d of $p.", bcnt);
      act(buf, ch, obj, NULL, TO_ROOM);
      sprintf(buf, "You buy %d of $p for %d dollars.", bcnt, cost * bcnt);
      act(buf, ch, obj, NULL, TO_CHAR);
    }
    else {
      act("$n buys $p.", ch, obj, NULL, TO_ROOM);
      sprintf(buf, "You buy $p for %.2f .", price);
      act(buf, ch, obj, NULL, TO_CHAR);
    }
    int tcredit = ch->pcdata->total_credit;
    if(has_color == TRUE)
    ch->pcdata->total_credit = 0;

    if (trade_good(obj)) {
      ch->money -= cost * bcnt;
    }
    else {
      ch->pcdata->total_credit -= cost * bcnt;
      if (ch->pcdata->total_credit < 0) {
        if (ch->pcdata->total_money > cost && cost > 500 && !IS_AFFECTED(ch, AFF_DISCREDIT))
        ch->pcdata->total_money += ch->pcdata->total_credit;
        else
        ch->money += ch->pcdata->total_credit;

        ch->pcdata->total_credit = 0;
      }

      if (ch->pcdata->total_credit <= 0 && !is_gm(ch) && ch->played / 3600 > 5) {
        keeper->money += cost * bcnt;
        keeper->pIndexData->pShop->proceed += (cost - obj->cost) * bcnt;
      }

      if (ch->in_room->vnum == 52) {
        ch->pcdata->total_credit += (int)((cost * bcnt) * 3 / 4);
      }
    }

    if (ch->pcdata->total_credit <= 0 && !is_gm(ch) && ch->played / 3600 > 5 && !trade_good(obj)) {
      if (in_prop(ch) != NULL && in_prop(ch)->type == PROP_SHOP) {
        in_prop(ch)->profit += (cost * 3 / 4);
      }
    }
    else if (!is_gm(ch) && ch->played / 3600 > 5 && !trade_good(obj)) {
      if (in_prop(ch) != NULL && in_prop(ch)->type == PROP_SHOP) {
        in_prop(ch)->creditprofit += (cost * 3 / 4);
      }
    }

    if(has_color == TRUE)
    ch->pcdata->total_credit = tcredit;


    for (count = 0; count < bcnt; count++) {
      if (IS_SET(obj->extra_flags, ITEM_INVENTORY)) {
        t_obj = create_object(obj->pIndexData, obj->level);
        setdescs(t_obj);
      }
      else {
        t_obj = obj;
        obj = obj->next_content;
        obj_from_char(t_obj);
      }

      if (t_obj->timer > 0)
      t_obj->timer = 0;
      obj_to_char(t_obj, ch);
      // turning flashlights off at purchase - Discordance
      if (t_obj->item_type == ITEM_FLASHLIGHT) {
        if (!IS_SET(t_obj->extra_flags, ITEM_OFF)) {
          SET_BIT(t_obj->extra_flags, ITEM_OFF);
        }
      }
      if (t_obj->item_type == ITEM_PHONE) {
        if (time_info.phone < 7000000)
        time_info.phone = 7000000;
        time_info.phone += number_range(1, 2);
        t_obj->value[0] = time_info.phone;
        if (!IS_SET(obj->extra_flags, ITEM_OFF))
        SET_BIT(obj->extra_flags, ITEM_OFF);
      }
      if (is_gm(ch))
      SET_BIT(obj->extra_flags, ITEM_NORESALE);

      if (IS_SET(t_obj->extra_flags, ITEM_NOINVENTORY)) {
        if (get_eq_char(ch, WEAR_HOLD) == NULL)
        equip_char_silent(ch, t_obj, WEAR_HOLD);
        else
        equip_char_silent(ch, t_obj, WEAR_HOLD_2);
      }
      else if (CAN_WEAR(t_obj, ITEM_WEAR_HOLD)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL)
        equip_char_silent(ch, t_obj, WEAR_HOLD);
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL)
        equip_char_silent(ch, t_obj, WEAR_HOLD_2);
      }
    }
  }

  _DOFUN(do_list) {
    char buf[MAX_STRING_LENGTH];

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost, count;
    int ref = 0;
    bool found;
    char arg[MAX_INPUT_LENGTH];

    if ((keeper = find_keeper(ch)) == NULL)
    return;
    one_argument(argument, arg);

    found = FALSE;
    for (obj = keeper->carrying; obj; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (cost = get_cost(keeper, obj, TRUE)) > 0 && (arg[0] == '\0' || is_name(arg, obj->name))) {

        ref++;
        if (!found) {
          found = TRUE;
          send_to_char("\n[Num Price ] Item\n\r", ch);
        }

        if (IS_OBJ_STAT(obj, ITEM_INVENTORY)) {
          double value = cost;
          value /= 100;
          if (trade_good(obj))
          sprintf(buf, "[`c%3d`W $`g%-7.2f`x] %s (`YCurrency`x)\n\r", ref, value, obj->description);
          else
          sprintf(buf, "[`c%3d`W $`g%-7.2f`x] %s\n\r", ref, value, obj->description);
        }
        else {
          count = 1;

          while (obj->next_content != NULL && obj->pIndexData == obj->next_content->pIndexData && !str_cmp(obj->short_descr, obj->next_content->short_descr)) {
            obj = obj->next_content;
            count++;
          }
          sprintf(buf, "[%2d %5d %2d ] %s\n\r", obj->level, cost, count, obj->short_descr);
        }
        send_to_char(buf, ch);
      }
    }

    if (!found)
    send_to_char("You can't buy anything here.\n\r", ch);
    else
    send_to_char("\nSyntax: Buy <`cNum`x>\n\r", ch);

    return;
  }

  _DOFUN(do_sacrifice) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj = NULL;

    one_argument(argument, arg);

    if (arg[0] == '\0' || !str_cmp(arg, ch->name)) {
      send_to_char("You may have some trouble doing that.\n\r", ch);
      return;
    }

    if (is_helpless(ch) || is_pinned(ch) || in_fight(ch) || room_hostile(ch->in_room) || is_ghost(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->pcdata->account != NULL) {
      if (ch->played / 3600 < 100 && ch->pcdata->account->maxhours < 200) {
        send_to_char("Your account or character is too new to destroy items.\n\r", ch);
        return;
      }
    }
    else {
      send_to_char("Your account or character is too new to destroy items.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "camp")) {
      if (!IS_SET(ch->in_room->room_flags, ROOM_CAMPSITE)) {
        send_to_char("There's no campsite here.\n\r", ch);
        return;
      }
      REMOVE_BIT(ch->in_room->room_flags, ROOM_CAMPSITE);
      act("You destroy the campsite.", ch, NULL, NULL, TO_CHAR);
      act("$n destroys the campsite.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      return;
    }
    
    if (!str_cmp(arg, "home")) {
      if (!IS_SET(ch->in_room->room_flags, ROOM_ANIMALHOME)) {
        send_to_char("There's no campsite here.\n\r", ch);
        return;
      }
      REMOVE_BIT(ch->in_room->room_flags, ROOM_ANIMALHOME);
      act("You destroy the animal home.", ch, NULL, NULL, TO_CHAR);
      act("$n destroys the animal home.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      return;
    }

    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg, obj->name)) {
      obj = get_obj_carry(ch, arg, ch);
    }
    
    if (obj == NULL) {
      obj = get_obj_list(ch, arg, ch->in_room->contents);
      if ((in_prop(ch) != NULL || in_house(ch) != NULL) && !can_decorate(ch, ch->in_room)) {
        send_to_char("That's not exactly subtle.\n\r", ch);
        return;
      }
    }
    
    if (obj == NULL) {
      send_to_char("You can't find it.\n\r", ch);
      return;
    }
    
    if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
      send_to_char("You can't destroy that.\n\r", ch);
      return;
    }
    
    if (obj->item_type == ITEM_CORPSE_PC && obj->contains) {
      send_to_char("You can't trash that.\n\r", ch);
      return;
    }
    
    if (obj->item_type == ITEM_CORPSE_NPC && obj->timer > 0) {
      send_to_char("You can't trash that.\n\r", ch);
      return;
    }
    
    if (obj->pIndexData->vnum == 45196) {
      send_to_char("You can't trash that.\n\r", ch);
      return;
    }

    if (!CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC)) {
      act("$a $p cannot be trashed.", ch, obj, 0, TO_CHAR);
      return;
    }

    if (obj->cost < 0 && !is_gm(ch)) {
      send_to_char("That's too priceless to throw away.\n\r", ch);
      return;
    }
    
    if (obj->cost > 10000 && !IS_SET(obj->extra_flags, ITEM_NORESALE)) {
      send_to_char("Perhaps you should ebay that instead.\n\r", ch);
      return;
    }
    
    if (obj->in_room != NULL) {
      for (CharList::iterator it = obj->in_room->people->begin();
      it != obj->in_room->people->end(); ++it) {
        if ((*it)->on == obj) {
          act("$N appears to be using $a $p.", ch, obj, *it, TO_CHAR);
          return;
        }
      }
    }
    alter_character(ch);
    act("You throw $a $p away.", ch, obj, NULL, TO_CHAR);
    act("$n throws $a $p away.", ch, obj, NULL, TO_ROOM);
    wiznet("$N trashes $a $p.", ch, obj, WIZ_SACCING, 0, 0);
    
    if (stolen_object(ch, obj) && obj->cost >= 2000)
    return_object(obj);
    else
    extract_obj(obj);
  
    return;
  }

  /*
  * Shopping commands.
  */
  CHAR_DATA *find_keeper(CHAR_DATA *ch) {
    /*char buf[MAX_STRING_LENGTH];*/
    CHAR_DATA *keeper = NULL;
    SHOP_DATA *pShop = NULL;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      keeper = *it;
      if (IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL)
      break;
    }

    if (pShop == NULL) {
      return NULL;
    }

    /*
    * Shop hours.
    */

    /*
    if ( time_info.hour < pShop->open_hour )
    {
      do_function(keeper, &do_say, "Sorry, I am closed. Come back later.");
      return NULL;
    }

    if ( time_info.hour > pShop->close_hour )
    {
      do_function(keeper, &do_say, "Sorry, I am closed. Come back tomorrow.");
      return NULL;
    }
    */
    return keeper;
  }

  /* insert an object at the right spot for the keeper */
  void obj_to_keeper(OBJ_DATA *obj, CHAR_DATA *ch) {
    OBJ_DATA *t_obj, *t_obj_next;

    /* see if any duplicates are found */
    for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next) {
      t_obj_next = t_obj->next_content;

      if (obj->pIndexData == t_obj->pIndexData && !str_cmp(obj->short_descr, t_obj->short_descr)) {
        /* if this is an unlimited item, destroy the new one */
        if (IS_OBJ_STAT(t_obj, ITEM_INVENTORY)) {
          extract_obj(obj);
          return;
        }
        obj->cost = t_obj->cost; /* keep it standard */
        break;
      }
    }

    if (t_obj == NULL) {
      obj->next_content = ch->carrying;
      ch->carrying = obj;
    }
    else {
      obj->next_content = t_obj->next_content;
      t_obj->next_content = obj;
    }

    obj->carried_by = ch;
    obj->in_room = NULL;
    obj->in_obj = NULL;
  }

  /* get an object from a shopkeeper's list */
  OBJ_DATA *get_obj_keeper(CHAR_DATA *ch, CHAR_DATA *keeper, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument(argument, arg);
    count = 0;
    for (obj = keeper->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && can_see_obj(keeper, obj) && can_see_obj(ch, obj) && is_name(arg, obj->name)) {
        if (++count == number)
        return obj;

        /* skip other objects of the same name */
        while (obj->next_content != NULL && obj->pIndexData == obj->next_content->pIndexData && !str_cmp(obj->short_descr, obj->next_content->short_descr))
        obj = obj->next_content;
      }
    }

    return NULL;
  }

  int get_cost(CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy) {
    SHOP_DATA *pShop;
    int cost;

    if (obj == NULL || (pShop = keeper->pIndexData->pShop) == NULL)
    return 0;

    if (fBuy) {
      cost = obj->cost * pShop->profit_buy / 100;
    }
    else {
      OBJ_DATA *obj2;
      int itype;

      cost = 0;
      for (itype = 0; itype < MAX_TRADE; itype++) {
        cost = obj->cost * pShop->profit_sell / 100;
        break;
      }

      for (obj2 = keeper->carrying; obj2; obj2 = obj2->next_content) {
        if (obj->pIndexData == obj2->pIndexData && !str_cmp(obj->short_descr, obj2->short_descr)) {
          if (IS_OBJ_STAT(obj2, ITEM_INVENTORY))
          cost /= 2;
          else
          cost = cost * 3 / 4;
        }
      }
    }

    return cost;
  }

  _DOFUN(do_ebay) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (!is_corporeal(ch) || IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You are ignored.\n\r", ch);
      return;
    }

    if (arg[0] == '\0') {
      send_to_char("Sell what?\n\r", ch);
      return;
    }
    if (is_helpless(ch) || is_pinned(ch) || in_fight(ch) || room_hostile(ch->in_room) || is_ghost(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->pcdata->account != NULL) {
      if (ch->played / 3600 < 100 && ch->pcdata->account->maxhours < 200) {
        send_to_char("Your account or character is too new to sell items.\n\r", ch);
        return;
      }
    }
    else {
      send_to_char("Your account or character is too new to sell items.\n\r", ch);
      return;
    }

    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
      send_to_char("You don't have that item.\n\r", ch);
      return;
    }

    if (obj->item_type != ITEM_BABY) {
      if (!goblin_market(ch->in_room)) {
        send_to_char("You can only sell off items at the goblin market.\n\r", ch);
        return;
      }

      if (obj->cost < 10000 && !trade_good(obj)) {
        send_to_char("That's a bit cheap for doing that.\n\r", ch);
        return;
      }
      /*
      if(trade_good(obj))
      {
        send_to_char("You could just sell that.\n\r",ch);
        return;
      }
      */
      if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
        send_to_char("There isn't a market for it.\n\r", ch);
        return;
      }

      if (obj->item_type == ITEM_DRINK_CON) {
        send_to_char("There isn't a market for it.\n\r", ch);
        return;
      }
      if (obj->pIndexData->vnum == 42000) {
        send_to_char("That might not be the best idea.\n\r", ch);
        return;
      }

      if (obj->item_type == ITEM_CORPSE_NPC && obj->timer > 0) {
        send_to_char("You can't trash that.\n\r", ch);
        return;
      }

      alter_character(ch);
      if (stolen_object(ch, obj) && !trade_good(obj))
      return_object(obj);
      else
      extract_obj(obj);

      if (IS_SET(obj->extra_flags, ITEM_NORESALE)) {
        ch->pcdata->total_credit += UMIN(200000, obj->cost / 10);
      }
      else {
        if (trade_good(obj))
        ch->money += UMIN(200000, obj->cost * 95 / 100);
        else
        ch->money += UMIN(100000, obj->cost / 10);
      }
      send_to_char("You sell it to the market.\n\r", ch);
    }
    else { // baby sales
      alter_character(ch);
      if (stolen_object(ch, obj))
      return_object(obj);
      else
      extract_obj(obj);

      if (goblin_market(ch->in_room)) {
        send_to_char("`WYou sell your baby to be `Rh`rarvested `Win the `gGoblin Market`x.\n\r", ch);
        ch->pcdata->total_credit += 50000;
      }
      else if (room_in_school(ch->in_room->vnum)) {
        send_to_char("`WYou place your baby with the Institute Orphanage.`x\n\r", ch);
        ch->pcdata->total_credit += 10000;
      }
    }
    return;
  }

  _DOFUN(do_sell) {
    do_function(ch, &do_ebay, argument);
    return;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int cost;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("Sell what?\n\r", ch);
      return;
    }
    if (is_helpless(ch) || is_pinned(ch) || in_fight(ch) || room_hostile(ch->in_room) || is_ghost(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
      send_to_char("You don't have that item.\n\r", ch);
      return;
    }

    if (!can_drop_obj(ch, obj)) {
      send_to_char("You can't let go of it.\n\r", ch);
      return;
    }

    /* haggle */

    cost = obj->cost;
    cost = UMAX(0, (int)(cost * .85));

    sprintf(buf, "You sell $a $p for %d dollars.", cost);
    act(buf, ch, obj, NULL, TO_CHAR);

    ch->money += cost;

    extract_obj(obj);

    return;
  }

  _DOFUN(do_stash) {
    OBJ_DATA *obj;
    Buffer stash(6048);
    ROOM_INDEX_DATA *room;
    char buf[MSL];
    if (ch->in_room == NULL)
    return;
    send_to_char("\n`cStash Contents:`x\n\n\r", ch);

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->stash_room == ch->in_room->vnum) {
        sprintf(buf, "%s\n\r", obj->description);
        stash.strcat(buf);
      }
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->stash_room != ch->in_room->vnum) {
        if (obj->stash_room != 0) {
          if ((room = get_room_index(obj->stash_room)) != NULL)
          sprintf(buf, "%s `W(`x%s`W)`x\n\r", obj->short_descr, room->name);
          else
          sprintf(buf, "%s `W(`xSomewhere`W)`x\n\r", obj->short_descr);
          stash.strcat(buf);
        }
      }
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->stash_room != ch->in_room->vnum) {
        if (obj->stash_room == 0) {
          if (obj->item_type == ITEM_CLOTHING) {
            if (obj->condition < 100)
            sprintf(buf, "%s `W(`xhome`W)`x %d percent clean\n\r", obj->short_descr, obj->condition);
            else
            sprintf(buf, "%s `W(`xhome`W)`x\n\r", obj->short_descr);
          }
          else
          sprintf(buf, "%s `W(`xhome`W)`x\n\r", obj->short_descr);

          stash.strcat(buf);
        }
      }
    }
    page_to_char(stash, ch);
  }

  _DOFUN(do_movein) {
    OBJ_DATA *obj;

    if (!has_stash(ch->in_room)) {
      send_to_char("There's no stash here.\n\r", ch);
      return;
    }

    if (!room_in_school(ch->in_room->vnum) && ch->in_room->vnum != 41070) {
      if (ch->money < 10000) {
        send_to_char("That costs 100 dollars.\n\r", ch);
        return;
      }
    }

    if (!room_in_school(ch->in_room->vnum)) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (institute_room(get_room_index(obj->stash_room))) {
          obj->stash_room = ch->in_room->vnum;
        }
        else if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && (obj->item_type == ITEM_CLOTHING || obj->item_type == ITEM_JEWELRY)) {
          obj->stash_room = ch->in_room->vnum;
        }
      }
    }
    else {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
          obj->stash_room = ch->in_room->vnum;
        }
      }
    }

    send_to_char("Moved in.\n\r", ch);

    if (!room_in_school(ch->in_room->vnum) && ch->in_room->vnum != 41070) {
      ch->money -= 10000;
    }
  }

  OBJ_DATA *get_jacket(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    if ((obj = get_eq_char(ch, WEAR_BODY_25)) != NULL && can_see_wear(ch, WEAR_BODY_25)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_24)) != NULL && can_see_wear(ch, WEAR_BODY_24)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_23)) != NULL && can_see_wear(ch, WEAR_BODY_23)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_22)) != NULL && can_see_wear(ch, WEAR_BODY_22)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_21)) != NULL && can_see_wear(ch, WEAR_BODY_21)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_20)) != NULL && can_see_wear(ch, WEAR_BODY_20)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_19)) != NULL && can_see_wear(ch, WEAR_BODY_19)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_18)) != NULL && can_see_wear(ch, WEAR_BODY_18)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_17)) != NULL && can_see_wear(ch, WEAR_BODY_17)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_16)) != NULL && can_see_wear(ch, WEAR_BODY_16)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_15)) != NULL && can_see_wear(ch, WEAR_BODY_15)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_14)) != NULL && can_see_wear(ch, WEAR_BODY_14)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_13)) != NULL && can_see_wear(ch, WEAR_BODY_13)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_12)) != NULL && can_see_wear(ch, WEAR_BODY_12)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_11)) != NULL && can_see_wear(ch, WEAR_BODY_11)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_10)) != NULL && can_see_wear(ch, WEAR_BODY_10)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_9)) != NULL && can_see_wear(ch, WEAR_BODY_9)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_8)) != NULL && can_see_wear(ch, WEAR_BODY_8)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_7)) != NULL && can_see_wear(ch, WEAR_BODY_7)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_6)) != NULL && can_see_wear(ch, WEAR_BODY_6)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_5)) != NULL && can_see_wear(ch, WEAR_BODY_5)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_4)) != NULL && can_see_wear(ch, WEAR_BODY_4)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_3)) != NULL && can_see_wear(ch, WEAR_BODY_3)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_2)) != NULL && can_see_wear(ch, WEAR_BODY_2)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }
    if ((obj = get_eq_char(ch, WEAR_BODY_1)) != NULL && can_see_wear(ch, WEAR_BODY_1)) {
      if (is_big(obj) && is_name("jacket", obj->name))
      return obj;
      if (is_big(obj) && is_name("pants", obj->name))
      return obj;
      if (is_big(obj) && is_name("jeans", obj->name))
      return obj;
      if (is_big(obj) && is_name("slacks", obj->name))
      return obj;
      if (is_big(obj) && is_name("trousers", obj->name))
      return obj;
      if (is_big(obj) && is_name("hoodie", obj->name))
      return obj;
      if (is_big(obj) && is_name("blazer", obj->name))
      return obj;
      if (is_big(obj) && is_name("coat", obj->name))
      return obj;
      if (is_big(obj) && is_name("trench", obj->name))
      return obj;
    }

    return NULL;
  }

  OBJ_DATA *get_pants(CHAR_DATA *ch) { return get_jacket(ch); }

  _DOFUN(do_zip) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char buf[MSL];
    char arg1[MSL];
    char *buf2 = NULL;

    one_argument(argument, arg);

    if (ch->pcdata->ci_editing == 1) {
      zip_person(ch, argument);
      return;
    }
    if (arg[0] == '\0') {
      send_to_char("Zip what?\n\r", ch);
      return;
    }

    if ((obj = get_obj_wear(ch, arg, TRUE)) == NULL) {
      if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }
    }

    if (obj->item_type != ITEM_CLOTHING) {
      send_to_char("You can't do that to that.\n\r", ch);
      return;
    }

    if (obj->value[1] <= 0) {
      send_to_char("That won't work.\n\r", ch);
      return;
    }

    int covers = obj->value[0];
    int orig = covers;
    int zips = obj->value[1];
    int mod = 0;
    int i, iWear;
    for (i = 0; i < MAX_COVERS; i++) {
      if (zips >= cover_table[i]) {
        if (covers >= cover_table[i]) {
          mod -= cover_table[i];
        }
        else
        mod += cover_table[i];
        zips -= cover_table[i];
      }

      if (covers >= cover_table[i])
      covers -= cover_table[i];
    }
    obj->value[0] += mod;
    int newval = obj->value[0];
    if (mod < 0) {
      act("You adjust your $a $p.", ch, obj, NULL, TO_CHAR);
      act("$n adjusts $s $p.", ch, obj, NULL, TO_ROOM);

      for (i = 0; i < MAX_COVERS; i++) {
        obj->value[0] = orig;
        if (is_covered(ch, cover_table[i])) {
          obj->value[0] = newval;
          if (!is_covered(ch, cover_table[i]) && safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
            //		    free_string(buf2);
            buf2 = str_dup(ch->pcdata->focused_descs[i]);
            buf2 = one_argument_nouncap(buf2, arg1);
            if (is_number(arg1))
            sprintf(buf, "Revealing that; %s", buf2);
            else
            sprintf(buf, "Revealing that; %s %s", arg1, buf2);
            act(buf, ch, NULL, NULL, TO_CHAR);
            act(buf, ch, NULL, NULL, TO_ROOM);
          }
        }
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        obj->value[0] = orig;
        if ((obj2 = get_eq_char(ch, iWear)) == NULL || !can_see_obj(ch, obj) || !can_see_wear(ch, iWear)) {
          obj->value[0] = newval;
          if ((obj2 = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && can_see_wear(ch, iWear)) {
            sprintf(buf, "Revealing;%s $o", obj2->wear_string);
            act(buf, ch, obj2, NULL, TO_CHAR);
            act(buf, ch, obj2, NULL, TO_ROOM);
          }
        }
      }

      obj->value[0] = newval;

    }
    else if (mod > 0) {
      act("You adjust your $a $p.", ch, obj, NULL, TO_CHAR);
      act("$n adjusts $s $p.", ch, obj, NULL, TO_ROOM);

      for (i = 0; i < MAX_COVERS; i++) {
        obj->value[0] = orig;
        if (!is_covered(ch, cover_table[i])) {
          obj->value[0] = newval;
          if (is_covered(ch, cover_table[i]) && safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
            //                    free_string(buf2);
            buf2 = str_dup(ch->pcdata->focused_descs[i]);
            buf2 = one_argument_nouncap(buf2, arg1);
            if (is_number(arg1))
            sprintf(buf, "Concealing that; %s", buf2);
            else
            sprintf(buf, "Concealing that; %s %s", arg1, buf2);

            act(buf, ch, NULL, NULL, TO_CHAR);
            act(buf, ch, NULL, NULL, TO_ROOM);
          }
        }
      }

      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        obj->value[0] = orig;
        if ((obj2 = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && can_see_wear(ch, iWear)) {
          obj->value[0] = newval;
          if ((obj2 = get_eq_char(ch, iWear)) == NULL || !can_see_obj(ch, obj) || !can_see_wear(ch, iWear)) {
            sprintf(buf, "Concealing;%s $o", obj2->wear_string);
            act(buf, ch, obj2, NULL, TO_CHAR);
            act(buf, ch, obj2, NULL, TO_ROOM);
          }
        }
      }

      obj->value[0] = newval;
    }
  }

  _DOFUN(do_extradesc) {
    char arg1[MSL];
    char arg2[MSL];
    char colbuf[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    EXTRA_DESCR_DATA *ed;

    if (ch->in_room->vnum != 11029) {
      send_to_char("You're not at a tailor.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    if (arg1[0] == '\0') {
      send_to_char("Adjust what?\n\r", ch);
      return;
    }

    remove_color(colbuf, argument);
    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (is_name(arg1, obj->name)) {
        send_to_char("Done.\n\r", ch);
        ed = new_extra_descr();
        ed->keyword = str_dup(arg2);
        ed->next = obj->extra_descr;
        obj->extra_descr = ed;
        string_append(ch, &obj->extra_descr->description);
        return;
      }
    }
    send_to_char("I don't see that.\n\r", ch);
  }

  void add_to_outfit(CHAR_DATA *ch, OBJ_DATA *obj, int location, int num) {
    bool found = FALSE;
    int i, ref = 0;
    /*        for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
    continue;

    for(i=0;i<10;i++)
    {
    if(obj->outfit_location[i] == location && obj->outfit[i] == num)
    {
    obj->outfit[i] = 0;
    }
    }

    }
    for(obj = ch->carrying;obj != NULL;obj = obj->next_content)
    {
    if(obj == NULL)
    continue;

    for(i=0;i<10;i++)
    {
    if(obj->outfit_location[i] == location && obj->outfit[i] == num)
    {
    obj->outfit[i] = 0;
    }
    }
    }
    */
    for (i = 0; i < 10; i++) {
      if (obj->outfit[i] == num) {
        obj->outfit[i] = 0;
        obj->outfit_location[i] = 0;
      }
    }
    for (i = 0; i < 10 && !found; i++) {
      if (obj->outfit[i] == 0) {
        ref = i;
        found = TRUE;
        break;
      }
    }
    if (found) {
      obj->outfit[ref] = num;
      obj->outfit_location[ref] = location;
    }
  }
  void remove_from_outfit(OBJ_DATA *obj, int num) {
    int i;
    for (i = 0; i < 10; i++) {
      if (obj->outfit[i] == num) {
        obj->outfit[i] = 0;
      }
    }
  }
  void credit_charge(CHAR_DATA *ch, int amount) {
    ch->pcdata->total_credit -= amount;
    if (ch->pcdata->total_credit < 0) {
      ch->pcdata->total_money += ch->pcdata->total_credit;
      ch->pcdata->total_credit = 0;
    }
  }

  int outfit_number(CHAR_DATA *ch, char *argument) {
    for (int i = 0; i < 20; i++) {
      if (!str_cmp(ch->pcdata->outfit_nicknames[i], argument))
      return ch->pcdata->outfit_nickpoint[i];
    }
    return atoi(argument);
  }

  _DOFUN(do_outfit) {
    char arg[MSL];
    char arg2[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *newobj;
    int i, iWear, number;

    argument = one_argument_nouncap(argument, arg);

    if (!str_cmp(arg, "dream")) {
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      if (val < 0 || val > 1000) {
        send_to_char("Syntax: Outfit dream (number)\n\r", ch);
        return;
      }
      ch->pcdata->dreamoutfit = val;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg, "nickname")) {
      argument = one_argument_nouncap(argument, arg2);
      int val = atoi(arg2);
      if (!str_cmp(arg2, "clear")) {
        for (int i = 0; i < 20; i++)
        ch->pcdata->outfit_nickpoint[i] = 0;
        send_to_char("Done.\n\r", ch);
        return;
      }
      if (val < 1 || val > 1000) {
        send_to_char("Outfit nickname (number) (name)\n\r", ch);
        return;
      }
      for (int i = 0; i < 20; i++) {
        if (ch->pcdata->outfit_nickpoint[i] == val) {
          free_string(ch->pcdata->outfit_nicknames[i]);
          ch->pcdata->outfit_nicknames[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      for (int i = 0; i < 20; i++) {
        if (ch->pcdata->outfit_nickpoint[i] == 0) {
          ch->pcdata->outfit_nickpoint[i] = val;
          free_string(ch->pcdata->outfit_nicknames[i]);
          ch->pcdata->outfit_nicknames[i] = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
      }
      send_to_char("Not enough slots.\n\r", ch);
      return;
    }

    if (clinic_patient(ch)) {
      send_to_char("As a patient, you have no access to your stash.\n\r", ch);
      return;
    }
    if (!has_stash(ch->in_room)) {
      send_to_char("There's no stash here.\n\r", ch);
      return;
    }
    if (is_animal(ch)) {
      send_to_char("I don't think your clothes will fit right now.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You're helpless to change.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("You cannot reach that.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "naked")) {
      bool broken = FALSE;
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;
        if (IS_SET(obj->extra_flags, ITEM_NOOUTFIT))
        continue;
        if (IS_SET(obj->extra_flags, ITEM_RELIC))
        continue;

        if (IS_SET(obj->extra_flags, ITEM_CURSED) && (!is_helpless(ch) && !is_possessed(ch))) {
          EXTRA_DESCR_DATA *ed;
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+imprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_CURSED) {
                  ch->pcdata->imprint_type[i] = 0;
                }
              }
            }

            if (is_name("+bimprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_BCURSED) {
                  ch->pcdata->imprint_type[i] = 0;
                }
              }
            }
          }
        }

        unequip_char_silent(ch, obj);
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        obj->stash_room = ch->in_room->vnum;
        if (under_opression(ch) && broken == FALSE && obj->item_type == ITEM_CLOTHING && number_percent() % 4 == 0) {
          act("$p abruptly tears.", ch, obj, NULL, TO_CHAR);
          act("$p abruptly tears.", ch, obj, NULL, TO_ROOM);
          extract_obj(obj);
          broken = TRUE;
        }
      }
      send_to_char("You strip naked.\n\r", ch);
      act("$n strips naked.", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(arg, "clone")) {
      if (!is_number(argument)) {
        send_to_char("Syntax: outfit clone <number>\n\r", ch);
        return;
      }
      number = atoi(argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;
        if (IS_SET(obj->extra_flags, ITEM_NOOUTFIT))
        continue;
        if (IS_SET(obj->extra_flags, ITEM_RELIC))
        continue;

        OBJ_DATA *newobj = NULL;
        newobj = create_object(obj->pIndexData, 0);
        obj_to_char(newobj, ch);
        credit_charge(ch, newobj->cost);
        clone_obj(ch, obj, newobj, FALSE);
        add_to_outfit(ch, newobj, iWear, number);
      }
      send_to_char("Outfit cloned.\n\r", ch);
    }
    else if (!str_cmp(arg, "create")) {
      if (!is_number(argument)) {
        send_to_char("Syntax: outfit create <number>\n\r", ch);
        return;
      }
      number = atoi(argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        for (i = 0; i < 10; i++) {
          if (obj->outfit[i] == number) {
            obj->outfit[i] = 0;
          }
        }
      }
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj == NULL)
        continue;

        for (i = 0; i < 10; i++) {
          if (obj->outfit[i] == number) {
            obj->outfit[i] = 0;
          }
        }
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        add_to_outfit(ch, obj, iWear, number);
      }
      printf_to_char(ch, "Outfit %d created.\n\r", number);
    }
    else if (!str_cmp(arg, "wear")) {
      number = outfit_number(ch, argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        if (IS_SET(obj->extra_flags, ITEM_NOOUTFIT))
        continue;
        if (IS_SET(obj->extra_flags, ITEM_RELIC))
        continue;
        if (IS_SET(obj->extra_flags, ITEM_CURSED) && (!is_helpless(ch) && !is_possessed(ch))) {
          EXTRA_DESCR_DATA *ed;
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+imprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_CURSED) {
                  ch->pcdata->imprint_type[i] = 0;
                }
              }
            }
            if (is_name("+bimprint", ed->keyword)) {
              for (int i = 0; i < 25; i++) {
                if (!str_cmp(ch->pcdata->imprint[i], ed->description) && ch->pcdata->imprint_type[i] == IMPRINT_BCURSED) {
                  ch->pcdata->imprint_type[i] = 0;
                }
              }
            }
          }
        }
        unequip_char_silent(ch, obj);
        if (!IS_SET(obj->extra_flags, ITEM_WARDROBE))
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        obj->stash_room = ch->in_room->vnum;
      }
      bool broken = FALSE;
      // discomark
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj == NULL)
        continue;

        if (IS_SET(obj->extra_flags, ITEM_NOOUTFIT))
        continue;

        if (!has_stash(ch->in_room))
        continue;

        if (obj->stash_room != ch->in_room->vnum && can_access(ch, obj->stash_room))
        continue;

        //nesting intensifies
        for (i = 0; i < 10; i++) {
          if (obj->outfit[i] == number) {
            if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            if (obj->wear_loc != WEAR_NONE)
            unequip_char_silent(ch, obj);
            if (obj != NULL) {
              if ((newobj = get_eq_char(ch, obj->outfit_location[i])) != NULL)
              unequip_char_silent(ch, newobj);
              if (get_eq_char(ch, obj->outfit_location[i]) == NULL)
              equip_char_silent(ch, obj, obj->outfit_location[i]);
              if (IS_SET(obj->extra_flags, ITEM_CURSED) && !is_helpless(ch) && !is_possessed(ch)) {
                EXTRA_DESCR_DATA *ed;
                for (ed = obj->extra_descr; ed; ed = ed->next) {
                  if (is_name("+imprint", ed->keyword)) {
                    bool found = FALSE;
                    for (int i = 0; i < 25; i++) {
                      if (!str_cmp(ch->pcdata->imprint[i], ed->description)) {
                        found = TRUE;
                      }
                    }
                    if (found == FALSE) {
                      auto_imprint(ch, ed->description, IMPRINT_CURSED);
                    }
                  }
                  if (is_name("+bimprint", ed->keyword)) {
                    bool found = FALSE;
                    for (int i = 0; i < 25; i++) {
                      if (!str_cmp(ch->pcdata->imprint[i], ed->description)) {
                        found = TRUE;
                      }
                    }
                    if (found == FALSE) {
                      auto_imprint(ch, ed->description, IMPRINT_BCURSED);
                    }
                  }
                }
              }
            }
            if (under_opression(ch) && broken == FALSE && obj->item_type == ITEM_CLOTHING && number_percent() % 4 == 0) {
              act("$p abruptly tears.", ch, obj, NULL, TO_CHAR);
              act("$p abruptly tears.", ch, obj, NULL, TO_ROOM);
              extract_obj(obj);
              broken = TRUE;
            }
          }
        }
      }
      printf_to_char(ch, "You change into outfit %d.\n\r", number);
      act("$n changes $s clothes.\n\r", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(arg, "add")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(argument)) {
        send_to_char("Syntax: outfit add <object> <number>\n\r", ch);
        return;
      }
      number = atoi(argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        if (is_name(arg2, obj->name)) {
          add_to_outfit(ch, obj, iWear, number);
          printf_to_char(ch, "Adding %s to outfit %d.\n\r", obj->short_descr, number);
          return;
        }
      }
      send_to_char("I don't see that here, only worn items may be added.\n\r", ch);
    }
    else if (!str_cmp(arg, "remove")) {
      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(argument)) {
        send_to_char("Syntax: outfit add <object> <number>\n\r", ch);
        return;
      }
      number = atoi(argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        if (is_name(arg2, obj->name)) {
          remove_from_outfit(obj, number);
          printf_to_char(ch, "Removing %s from outfit %d.\n\r", obj->short_descr, number);
          return;
        }
      }
      if (obj == NULL || !is_name(arg2, obj->name)) {
        obj = get_obj_carry(ch, arg2, ch);
        if (obj == NULL) {
          send_to_char("I don't see that here.\n\r", ch);
          return;
        }
        remove_from_outfit(obj, number);
        printf_to_char(ch, "Removing %s from outfit %d.\n\r", obj->short_descr, number);
      }
    }
    else if (!str_cmp(arg, "delete")) {
      number = outfit_number(ch, argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      for (int i = 0; i < 20; i++) {
        if (ch->pcdata->outfit_nickpoint[i] == number)
        ch->pcdata->outfit_nickpoint[i] = 0;
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        continue;

        for (i = 0; i < 10; i++) {
          if (obj->outfit[i] == number) {
            obj->outfit[i] = 0;
          }
        }
      }
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj == NULL)
        continue;

        for (i = 0; i < 10; i++) {
          if (obj->outfit[i] == number) {
            obj->outfit[i] = 0;
          }
        }
      }
      printf_to_char(ch, "You delete outfit %d.\n\r", number);
    }
    else if (!str_cmp(arg, "show")) {
      number = outfit_number(ch, argument);
      if (number <= 0 || number > 1000) {
        send_to_char("A number between 1 and 999 please.\n\r", ch);
        return;
      }
      /*
      for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
      {
      if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
      continue;

      for(i=0;i<10;i++)
      {
      if(obj->outfit[i] == number)
      {
      printf_to_char(ch, "%s\n\r", obj->short_descr);
      }
      }

      }
      */
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj == NULL)
        continue;

        for (i = 0; i < 10; i++) {
          if (obj->outfit[i] == number) {
            printf_to_char(ch, "%s\n\r", obj->short_descr);
          }
        }
      }
    }
    else
    send_to_char("Syntax: outfit naked, outfit create <number>, outfit wear <number>, outfit show <number>, outfit delete <number>, outfit add <object> <number>, outfit remove <object> <number>, outfit clone <number>, outfit nickname <number> <name>\n\r", ch);
  }

  bool is_pair(OBJ_DATA *obj) {

    if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_JEWELRY)
    return FALSE;

    if (is_name("skirt", obj->name))
    return FALSE;
    if (is_name("dress", obj->name))
    return FALSE;
    if (is_name("pants", obj->name))
    return TRUE;
    if (is_name("jeans", obj->name))
    return TRUE;
    if (is_name("trousers", obj->name))
    return TRUE;
    if (is_name("boxers", obj->name))
    return TRUE;
    if (is_name("shorts", obj->name))
    return TRUE;
    if (is_name("gloves", obj->name))
    return TRUE;
    if (is_name("gauntlets", obj->name))
    return TRUE;
    if (is_name("g-string", obj->name))
    return FALSE;
    if (is_name("gstring", obj->name))
    return FALSE;
    if (is_name("thong", obj->name))
    return FALSE;
    if (is_name("panties", obj->name))
    return TRUE;
    if (is_name("underpants", obj->name))
    return TRUE;
    if (is_name("briefs", obj->name))
    return TRUE;
    if (is_name("glasses", obj->name))
    return TRUE;
    if (is_name("sunglasses", obj->name))
    return TRUE;
    if (is_name("socks", obj->name))
    return TRUE;
    if (is_name("shoes", obj->name))
    return TRUE;
    if (is_name("boots", obj->name))
    return TRUE;
    if (is_name("glasses", obj->name))
    return TRUE;
    if (is_name("sunglasses", obj->name))
    return TRUE;

    return FALSE;
  }
  _DOFUN(do_waterproof) {
    char arg1[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (ch->money < 2000) {
      send_to_char("Waterproofing costs $20.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Waterproof what?\n\r", ch);
      return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (obj->item_type == ITEM_JEWELRY)
      continue;

      if (is_name(arg1, obj->name) && !IS_SET(obj->extra_flags, ITEM_WATERPROOF)) {
        SET_BIT(obj->extra_flags, ITEM_WATERPROOF);
        ch->money -= 2000;
        printf_to_char(ch, "You waterproof %s.\n\r", obj->short_descr);
        return;
      }
    }
  }

  _DOFUN(do_outfitproof) {
    char arg1[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    argument = one_argument_nouncap(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Outfitproof what?\n\r", ch);
      return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (is_name(arg1, obj->name)) {
        if (!IS_SET(obj->extra_flags, ITEM_NOOUTFIT)) {
          SET_BIT(obj->extra_flags, ITEM_NOOUTFIT);
          printf_to_char(ch, "You outfit proof %s.\n\r", obj->short_descr);
        }
        else {
          REMOVE_BIT(obj->extra_flags, ITEM_NOOUTFIT);
          printf_to_char(ch, "You remove the outfit proofing on %s.\n\r", obj->short_descr);
        }
        return;
      }
    }
  }
  
  _DOFUN(do_decayproof) {
    char arg1[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (ch->in_room->vnum != 11029) {
      send_to_char("You're not at a tailor.\n\r", ch);
      return;
    }

    if (ch->money < 20) {
      send_to_char("Decay proofing costs $20.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Adjust what?\n\r", ch);
      return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (is_name(arg1, obj->name)) {
        if (!IS_SET(obj->extra_flags, ITEM_NODECAY)) {
          SET_BIT(obj->extra_flags, ITEM_NODECAY);
          ch->money -= 20;
          printf_to_char(ch, "You decay proof %s.\n\r", obj->short_descr);
        }
        else {
          REMOVE_BIT(obj->extra_flags, ITEM_NODECAY);
          ch->money -= 20;
          printf_to_char(ch, "You remove the decay proofing on %s.\n\r", obj->short_descr);
        }
        return;
      }
    }
  }

  _DOFUN(do_dye) {
    char arg1[MSL];
    char buf[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (ch->in_room->vnum != 11029) {
      send_to_char("You can't do that here.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg1);

    if (safe_strlen(arg1) < 5 || safe_strlen(argument) < 5) {
      send_to_char("Syntax: dye <object> new string.\n\r", ch);
      return;
    }
    char colbuf[MSL];
    remove_color(colbuf, argument);

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (is_name(arg1, obj->name) && !str_cmp(obj->short_descr, colbuf)) {
        free_string(obj->short_descr);
        obj->short_descr = str_dup(argument);
        free_string(obj->name);
        obj->name = str_dup(argument);
        if (is_pair(obj))
        sprintf(buf, "A pair of %s", obj->short_descr);
        else {
          if (!str_cmp(a_or_an(obj->short_descr), "an"))
          sprintf(buf, "An %s", obj->short_descr);
          else
          sprintf(buf, "A %s", obj->short_descr);
        }
        free_string(obj->description);
        obj->description = str_dup(buf);
        send_to_char("Done.\n\r", ch);
        return;
      }
    }
  }

  bool does_zipcover(OBJ_DATA *obj, int selection) {
    int i;
    if (obj->item_type != ITEM_CLOTHING)
    return FALSE;
    if (obj->value[0] == 0)
    return FALSE;
    int value = obj->value[1];
    for (i = 0; i < MAX_COVERS; i++) {
      if (selection == cover_table[i]) {
        if (value >= selection)
        return TRUE;
        else
        return FALSE;
      }
      else {
        if (value >= cover_table[i])
        value -= cover_table[i];
      }
    }
    return FALSE;
  }

  bool does_cover_int(int num, int selection) {
    int i;
    if (num == 0)
    return FALSE;
    int value = num;
    for (i = 0; i < MAX_COVERS; i++) {
      if (selection == cover_table[i]) {
        if (value >= selection)
        return TRUE;
        else
        return FALSE;
      }
      else {
        if (value >= cover_table[i])
        value -= cover_table[i];
      }
    }
    return FALSE;
  }

  void zip_person(CHAR_DATA *ch, char *argument) {

    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "hands")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_HANDS)) {
          ch->pcdata->ci_zips -= COVERS_HANDS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_HANDS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerarms")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_LOWER_ARMS)) {
          ch->pcdata->ci_zips -= COVERS_LOWER_ARMS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_LOWER_ARMS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperarms")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_UPPER_ARMS)) {
          ch->pcdata->ci_zips -= COVERS_UPPER_ARMS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_UPPER_ARMS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "feet")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_FEET)) {
          ch->pcdata->ci_zips -= COVERS_FEET;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_FEET;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerlegs")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_LOWER_LEGS)) {
          ch->pcdata->ci_zips -= COVERS_LOWER_LEGS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_LOWER_LEGS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperarms")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_UPPER_ARMS)) {
          ch->pcdata->ci_zips -= COVERS_UPPER_ARMS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_UPPER_ARMS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "forehead")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_FOREHEAD)) {
          ch->pcdata->ci_zips -= COVERS_FOREHEAD;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_FOREHEAD;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "thighs")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_THIGHS)) {
          ch->pcdata->ci_zips -= COVERS_THIGHS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_THIGHS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "groin")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_GROIN)) {
          ch->pcdata->ci_zips -= COVERS_GROIN;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_GROIN;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "buttocks")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_ARSE)) {
          ch->pcdata->ci_zips -= COVERS_ARSE;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_ARSE;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerback")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_LOWER_BACK)) {
          ch->pcdata->ci_zips -= COVERS_LOWER_BACK;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_LOWER_BACK;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperback")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_UPPER_BACK)) {
          ch->pcdata->ci_zips -= COVERS_UPPER_BACK;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_UPPER_BACK;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerchest")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_LOWER_CHEST)) {
          ch->pcdata->ci_zips -= COVERS_LOWER_CHEST;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_LOWER_CHEST;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "breasts")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_BREASTS)) {
          ch->pcdata->ci_zips -= COVERS_BREASTS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_BREASTS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperchest")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_UPPER_CHEST)) {
          ch->pcdata->ci_zips -= COVERS_UPPER_CHEST;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_UPPER_CHEST;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "neck")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_NECK)) {
          ch->pcdata->ci_zips -= COVERS_NECK;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_NECK;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerface")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_LOWER_FACE)) {
          ch->pcdata->ci_zips -= COVERS_LOWER_FACE;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_LOWER_FACE;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "hair")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_HAIR)) {
          ch->pcdata->ci_zips -= COVERS_HAIR;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_HAIR;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "eyes")) {
        if (does_cover_int(ch->pcdata->ci_zips, COVERS_EYES)) {
          ch->pcdata->ci_zips -= COVERS_EYES;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_zips += COVERS_EYES;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "clear")) {
        ch->pcdata->ci_zips = 0;
        send_to_char("Coverage cleared.\n\r", ch);
      }
      else
      send_to_char("Options are: hands, lowerarms, upperarms, feet, lowerlegs, forehead, thighs, groin, buttocks, lowerback, upperback, lowerchest, breasts, upperchest, neck, lowerface, hair, eyes and clear\n\r", ch);

      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_layer) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;

      int amount = atoi(argument);

      if (amount < 0 || amount > 5) {
        send_to_char("Syntax: Layer 0-5\n\r", ch);
        ch->pcdata->ci_editing = 1;
        return;
      }
      ch->pcdata->ci_layer = amount;
      send_to_char("Layer set.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_height) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;

      int amount = atoi(argument);

      if (amount < 0 || amount > 18) {
        send_to_char("Syntax: Height 0-18\n\r", ch);
        ch->pcdata->ci_editing = 1;
        return;
      }
      ch->pcdata->ci_mod = amount;
      send_to_char("Height set.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_bust) {
    if (ch->in_room->vnum == ROOM_INDEX_GENESIS) {
      char buf[MSL];
      sprintf(buf, "bust %s", argument);
      do_function(ch, &do_change, buf);
      return;
    }
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;

      int amount = atoi(argument);

      if (amount < -8 || amount > 8) {
        send_to_char("Syntax: Bust -8 to 8\n\r", ch);
        ch->pcdata->ci_editing = 1;
        return;
      }
      ch->pcdata->ci_mod = amount + 30;
      send_to_char("Bust set.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_cover) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "hands")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_HANDS)) {
          ch->pcdata->ci_covers -= COVERS_HANDS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_HANDS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerarms")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_LOWER_ARMS)) {
          ch->pcdata->ci_covers -= COVERS_LOWER_ARMS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_LOWER_ARMS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperarms")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_UPPER_ARMS)) {
          ch->pcdata->ci_covers -= COVERS_UPPER_ARMS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_UPPER_ARMS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "feet")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_FEET)) {
          ch->pcdata->ci_covers -= COVERS_FEET;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_FEET;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerlegs")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_LOWER_LEGS)) {
          ch->pcdata->ci_covers -= COVERS_LOWER_LEGS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_LOWER_LEGS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperarms")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_UPPER_ARMS)) {
          ch->pcdata->ci_covers -= COVERS_UPPER_ARMS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_UPPER_ARMS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "forehead")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_FOREHEAD)) {
          ch->pcdata->ci_covers -= COVERS_FOREHEAD;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_FOREHEAD;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "thighs")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_THIGHS)) {
          ch->pcdata->ci_covers -= COVERS_THIGHS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_THIGHS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "groin")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_GROIN)) {
          ch->pcdata->ci_covers -= COVERS_GROIN;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_GROIN;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "buttocks")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_ARSE)) {
          ch->pcdata->ci_covers -= COVERS_ARSE;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_ARSE;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerback")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_LOWER_BACK)) {
          ch->pcdata->ci_covers -= COVERS_LOWER_BACK;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_LOWER_BACK;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperback")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_UPPER_BACK)) {
          ch->pcdata->ci_covers -= COVERS_UPPER_BACK;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_UPPER_BACK;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerchest")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_LOWER_CHEST)) {
          ch->pcdata->ci_covers -= COVERS_LOWER_CHEST;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_LOWER_CHEST;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "breasts")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_BREASTS)) {
          ch->pcdata->ci_covers -= COVERS_BREASTS;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_BREASTS;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "upperchest")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_UPPER_CHEST)) {
          ch->pcdata->ci_covers -= COVERS_UPPER_CHEST;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_UPPER_CHEST;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "neck")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_NECK)) {
          ch->pcdata->ci_covers -= COVERS_NECK;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_NECK;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "lowerface")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_LOWER_FACE)) {
          ch->pcdata->ci_covers -= COVERS_LOWER_FACE;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_LOWER_FACE;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "hair")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_HAIR)) {
          ch->pcdata->ci_covers -= COVERS_HAIR;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_HAIR;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "eyes")) {
        if (does_cover_int(ch->pcdata->ci_covers, COVERS_EYES)) {
          ch->pcdata->ci_covers -= COVERS_EYES;
          send_to_char("Coverage removed.\n\r", ch);
        }
        else {
          ch->pcdata->ci_covers += COVERS_EYES;
          send_to_char("Coverage added.\n\r", ch);
        }
      }
      else if (!str_cmp(argument, "clear")) {
        ch->pcdata->ci_covers = 0;
        send_to_char("Coverage cleared.\n\r", ch);
      }
      else
      send_to_char("Options should be added one at a time and are: hands, lowerarms, upperarms, feet, lowerlegs, forehead, thighs, groin, buttocks, lowerback, upperback, lowerchest, breasts, upperchest, neck, lowerface, hair, eyes and clear\n\r", ch);

      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_abort) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    ch->pcdata->ci_editing = 0;
    send_to_char("Edit aborted.\n\r", ch);
  }

  _DOFUN(do_taste) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      string_append(ch, &ch->pcdata->ci_taste);
    }
  }

  _DOFUN(do_scent) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      string_append(ch, &ch->pcdata->ci_taste);
    }
  }

  _DOFUN(do_effect) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      string_append(ch, &ch->pcdata->ci_taste);
    }
  }

  _DOFUN(do_long) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_long);
        ch->pcdata->ci_long = str_dup(argument);
        smash_tilde(ch->pcdata->ci_long);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Long (A complete but still shortish description of the object)\n\r", ch);
      }

      ch->pcdata->ci_editing = 1;
    }
  }
  _DOFUN(do_short) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_short);
        ch->pcdata->ci_short = str_dup(argument);
        smash_tilde(ch->pcdata->ci_short);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Short (A short description of the object)\n\r", ch);
      }

      ch->pcdata->ci_editing = 1;

    }
    else if (ch->pcdata->ci_editing == 4 || ch->pcdata->ci_editing == 5 || ch->pcdata->ci_editing == 6) {
      int temp = ch->pcdata->ci_editing;
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_short);
        ch->pcdata->ci_short = str_dup(argument);
        smash_tilde(ch->pcdata->ci_short);

        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Short (A short description of the monster)\n\r", ch);
      }

      ch->pcdata->ci_editing = temp;
    }
  }

  _DOFUN(do_names) {
    if (ch->in_room != NULL && ch->in_room->vnum == ROOM_INDEX_GENESIS) {
      char buf[MSL];
      sprintf(buf, "name %s", argument);
      do_function(ch, &do_change, buf);
      return;
    }
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(argument);
        smash_tilde(ch->pcdata->ci_name);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Names (List of names to target the object with)\n\r", ch);
      }
      ch->pcdata->ci_editing = 1;

    }
    else if (ch->pcdata->ci_editing == 2) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(argument);
        smash_tilde(ch->pcdata->ci_name);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("Name (Name of the room)\n\r", ch);
      }

    }
    else if (ch->pcdata->ci_editing == 3) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        char arg[MSL];
        one_argument_nouncap(argument, arg);

        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(arg);
        smash_tilde(ch->pcdata->ci_name);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("name (The name of the attack)\n\r", ch);
      }
    }
    else if (ch->pcdata->ci_editing == 4) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(argument);
        smash_tilde(ch->pcdata->ci_name);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("name (The name of the monster)\n\r", ch);
      }
    }
    else if (ch->pcdata->ci_editing == 5) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(argument);
        smash_tilde(ch->pcdata->ci_name);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("name (The name of the minion)\n\r", ch);
      }
    }
    else if (ch->pcdata->ci_editing == 6) {
      ch->pcdata->ci_absorb = 1;
      if (safe_strlen(argument) > 2) {
        free_string(ch->pcdata->ci_name);
        ch->pcdata->ci_name = str_dup(argument);
        smash_tilde(ch->pcdata->ci_name);
        send_to_char("Done.\n\r", ch);
      }
      else {
        send_to_char("name (The name of the ally)\n\r", ch);
      }
    }
  }

  _DOFUN(do_uses) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!is_number(argument)) {
        send_to_char("Syntax: uses (number)\n\r", ch);
        return;
      }
      if (atoi(argument) < 1 || atoi(argument) > 40) {
        send_to_char("Syntax: uses (1-40)\n\r", ch);
        ch->pcdata->ci_editing = 1;
        return;
      }
      ch->pcdata->ci_size = atoi(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_bites) {
    if (ch->pcdata->ci_editing == 0) {
      do_function(ch, &do_bite, argument);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!is_number(argument)) {
        send_to_char("Syntax: bites (number)\n\r", ch);
        return;
      }
      if (atoi(argument) < 1 || atoi(argument) > 20) {
        send_to_char("Syntax: bites (1-20)\n\r", ch);
        return;
      }
      ch->pcdata->ci_size = atoi(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_alcohol) {
    if (ch->pcdata->ci_editing == 0) {
      do_function(ch, &do_drink, argument);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "none")) {
        ch->pcdata->ci_alcohol =
        (liq_lookup("water") != -1 ? liq_lookup("water") : 0);
      }
      else if (!str_cmp(argument, "small")) {
        ch->pcdata->ci_alcohol =
        (liq_lookup("beer") != -1 ? liq_lookup("beer") : 0);
      }
      else if (!str_cmp(argument, "large")) {
        ch->pcdata->ci_alcohol =
        (liq_lookup("whisky") != -1 ? liq_lookup("whisky") : 0);
      }
      else {
        send_to_char("Syntax: alcohol none, small, large\n\r", ch);
        ch->pcdata->ci_editing = 1;
        return;
      }
      send_to_char("Done.\n\r", ch);

      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_drinks) {
    if (ch->pcdata->ci_editing == 0) {
      do_function(ch, &do_drink, argument);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!is_number(argument)) {
        send_to_char("Syntax: drinks (number)\n\r", ch);
        return;
      }
      if (atoi(argument) < 1 || atoi(argument) > 20) {
        send_to_char("Syntax: drinks (1-20)\n\r", ch);
        return;
      }
      ch->pcdata->ci_size = atoi(argument);
      send_to_char("Done.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
  }

  _DOFUN(do_size) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "Large")) {
        ch->pcdata->ci_size = 1;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Small")) {
        ch->pcdata->ci_size = 0;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Size Small/Large\n\r", ch);
      ch->pcdata->ci_editing = 1;

    }
    else if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      int num = atoi(argument);
      if (num < 300 || num > 1000) {
        send_to_char("Syntax: Size (300-1000)\n\r", ch);
        return;
      }
      if (num > max_operation_size(ch->pcdata->ci_zips)) {
        printf_to_char(
        ch, "The maximum size for an operation at that combat speed is %d.\n\r", max_operation_size(ch->pcdata->ci_zips));
        return;
      }
      ch->pcdata->ci_layer = num;
      send_to_char("Done.\n\r", ch);
    }
  }

  _DOFUN(do_cost) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      ch->pcdata->ci_editing = 0;
      ch->pcdata->ci_absorb = 1;
      if (is_number_float(argument)) {
        double temp = atof(argument);
        temp *= 100;
        if (ch->pcdata->ci_vnum == 1 && (int)(temp) <= ch->pcdata->ci_cost) {
          send_to_char("You can only use this to increase the cost.\n\r", ch);
          ch->pcdata->ci_editing = 1;
          return;
        }
        ch->pcdata->ci_cost = (int)(temp);
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Cost (Price in dollars and cents)\n\r", ch);

      ch->pcdata->ci_editing = 1;
    }
  }
  _DOFUN(do_to) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 9) {
      ch->pcdata->ci_absorb = 1;
      free_string(ch->pcdata->ci_name);
      ch->pcdata->ci_name = str_dup(argument);
      smash_tilde(ch->pcdata->ci_name);
      send_to_char("Done.\n\r", ch);
    }
  }

  _DOFUN(do_type) {
    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Prey")) {
        ch->pcdata->ci_discipline = DESTINY_PREY;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Predator")) {
        ch->pcdata->ci_discipline = DESTINY_PREDATOR;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Rival")) {
        ch->pcdata->ci_discipline = DESTINY_RIVAL;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Social")) {
        ch->pcdata->ci_discipline = DESTINY_SOCIAL;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Adventure")) {
        ch->pcdata->ci_discipline = DESTINY_ADVENTURE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Investigation")) {
        ch->pcdata->ci_discipline = DESTINY_INVESTIGATION;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Work")) {
        ch->pcdata->ci_discipline = DESTINY_WORK;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Syntax: Type Prey/Predator/Rival/social/Adventure/Investigation/Work\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
    else if (ch->pcdata->ci_editing == 23) {
      ch->pcdata->ci_absorb = 1;
      if(!str_cmp(argument, "adventure"))
      ch->pcdata->ci_discipline = PLOT_ADVENTURE;
      else if(!str_cmp(argument, "other"))
      ch->pcdata->ci_discipline = PLOT_OTHER;
      else if(!str_cmp(argument, "joint"))
      ch->pcdata->ci_discipline = PLOT_JOINT;
      else if(!str_cmp(argument, "competetive"))
      ch->pcdata->ci_discipline = PLOT_PVP;
      else if(!str_cmp(argument, "character"))
      ch->pcdata->ci_discipline = PLOT_CHARACTER;
      else if(!str_cmp(argument, "quest"))
      ch->pcdata->ci_discipline = PLOT_QUEST;
      else if(!str_cmp(argument, "personal"))
      ch->pcdata->ci_discipline = PLOT_PERSONAL;
      else if(!str_cmp(argument, "mystery"))
      ch->pcdata->ci_discipline = PLOT_MYSTERY;
      else if(!str_cmp(argument, "crisis"))
      ch->pcdata->ci_discipline = PLOT_CRISIS;
      else
      send_to_char("Valid types are: Adventure, Joint, Competetive, Character, Quest, Personal, Mystery, Crisis.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 1) {
      if (ch->pcdata->ci_vnum == 1) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Clothing")) {
        ch->pcdata->ci_type = 10;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Jewelry")) {
        ch->pcdata->ci_type = 11;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Food")) {
        ch->pcdata->ci_type = 12;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Drink")) {
        ch->pcdata->ci_type = 13;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Thing")) {
        ch->pcdata->ci_type = 14;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Bag")) {
        ch->pcdata->ci_type = 15;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Weapon")) {
        ch->pcdata->ci_type = 16;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Gun")) {
        ch->pcdata->ci_type = 17;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Phone")) {
        ch->pcdata->ci_type = 18;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Armor")) {
        ch->pcdata->ci_type = 19;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Furniture")) {
        ch->pcdata->ci_type = 20;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Drug")) {
        ch->pcdata->ci_type = 21;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Umbrella")) {
        ch->pcdata->ci_type = 22;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Perfume")) {
        ch->pcdata->ci_type = 23;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Flashlight")) {
        ch->pcdata->ci_type = 24;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Game")) {
        ch->pcdata->ci_type = 25;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Playback")) {
        ch->pcdata->ci_type = 26;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Valid types are: Clothing, Jewelry, Food, Drink, Thing, Bag, Weapon, Gun, Phone, Armor, Furniture, Drug, Umbrella, Perfume, Flashlight, Game, Playback.\n\r", ch);

      ch->pcdata->ci_editing = 1;
    }
    else if (ch->pcdata->ci_editing == 21) {
      ch->pcdata->ci_absorb = 1;
      if(!str_cmp(argument, "Lockdown"))
      {
        ch->pcdata->ci_discipline = DECREE_LOCKDOWN;
        send_to_char("Done.\n\r", ch);
      }
      else if(!str_cmp(argument, "Border Control"))
      {
        ch->pcdata->ci_discipline = DECREE_BORDER;
        send_to_char("Done.\n\r", ch);
      }
      else if(!str_cmp(argument, "Construction Grant"))
      {
        ch->pcdata->ci_discipline = DECREE_BUILDER;
        send_to_char("Done.\n\r", ch);
      }
      else if(!str_cmp(argument, "Sponsorship"))
      {
        ch->pcdata->ci_discipline = DECREE_SPONSOR;
        send_to_char("Done.\n\r", ch);
      }
      else if(!str_cmp(argument, "Overtax"))
      {
        ch->pcdata->ci_discipline = DECREE_OVERTAX;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Valid types are: Lockdown, Border Control, Construction Grant, Sponsorship, Overtax.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 16) {
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Prey")) {
        ch->pcdata->ci_discipline = DESTINY_PREY;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Predator")) {
        ch->pcdata->ci_discipline = DESTINY_PREDATOR;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Rival")) {
        ch->pcdata->ci_discipline = DESTINY_RIVAL;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Social")) {
        ch->pcdata->ci_discipline = DESTINY_SOCIAL;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Adventure")) {
        ch->pcdata->ci_discipline = DESTINY_ADVENTURE;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Investigation")) {
        ch->pcdata->ci_discipline = DESTINY_INVESTIGATION;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Work")) {
        ch->pcdata->ci_discipline = DESTINY_WORK;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Syntax: Type Prey/Predator/Rival/social/Adventure/Investigation/Work\n\r", ch);
      ch->pcdata->ci_editing = 16;
    }
    else if (ch->pcdata->ci_editing == 1) {
      if (ch->pcdata->ci_vnum == 1) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 0;
      if (!str_cmp(argument, "Clothing")) {
        ch->pcdata->ci_type = 10;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Jewelry")) {
        ch->pcdata->ci_type = 11;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Food")) {
        ch->pcdata->ci_type = 12;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Drink")) {
        ch->pcdata->ci_type = 13;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Thing")) {
        ch->pcdata->ci_type = 14;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Bag")) {
        ch->pcdata->ci_type = 15;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Weapon")) {
        ch->pcdata->ci_type = 16;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Gun")) {
        ch->pcdata->ci_type = 17;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Phone")) {
        ch->pcdata->ci_type = 18;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Armor")) {
        ch->pcdata->ci_type = 19;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Furniture")) {
        ch->pcdata->ci_type = 20;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Drug")) {
        ch->pcdata->ci_type = 21;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Umbrella")) {
        ch->pcdata->ci_type = 22;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Perfume")) {
        ch->pcdata->ci_type = 23;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Flashlight")) {
        ch->pcdata->ci_type = 24;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Game")) {
        ch->pcdata->ci_type = 25;
        send_to_char("Done.\n\r", ch);
      }
      else if (!str_cmp(argument, "Playback")) {
        ch->pcdata->ci_type = 26;
        send_to_char("Done.\n\r", ch);
      }
      else
      send_to_char("Valid types are: Clothing, Jewelry, Food, Drink, Thing, Bag, Weapon, Gun, Phone, Armor, Furniture, Drug, Umbrella, Perfume, Flashlight, Game, Playback.\n\r", ch);
      ch->pcdata->ci_editing = 1;
    }
    else if (ch->pcdata->ci_editing == 2) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "club"))
      ch->pcdata->ci_type = SECT_CLUB;
      else if (!str_cmp(argument, "restaurant"))
      ch->pcdata->ci_type = SECT_RESTERAUNT;
      else if (!str_cmp(argument, "shop"))
      ch->pcdata->ci_type = SECT_SHOP;
      else if (!str_cmp(argument, "street"))
      ch->pcdata->ci_type = SECT_STREET;
      else if (!str_cmp(argument, "alley"))
      ch->pcdata->ci_type = SECT_ALLEY;
      else if (!str_cmp(argument, "warehouse"))
      ch->pcdata->ci_type = SECT_WAREHOUSE;
      else if (!str_cmp(argument, "commercial"))
      ch->pcdata->ci_type = SECT_COMMERCIAL;
      else if (!str_cmp(argument, "park"))
      ch->pcdata->ci_type = SECT_PARK;
      else if (!str_cmp(argument, "tunnels"))
      ch->pcdata->ci_type = SECT_TUNNELS;
      else if (!str_cmp(argument, "cafe"))
      ch->pcdata->ci_type = SECT_CAFE;
      else if (!str_cmp(argument, "rooftop"))
      ch->pcdata->ci_type = SECT_ROOFTOP;
      else if (!str_cmp(argument, "basement"))
      ch->pcdata->ci_type = SECT_BASEMENT;
      else if (!str_cmp(argument, "bank"))
      ch->pcdata->ci_type = SECT_BANK;
      else if (!str_cmp(argument, "hospital"))
      ch->pcdata->ci_type = SECT_HOSPITAL;
      else if (!str_cmp(argument, "air"))
      ch->pcdata->ci_type = SECT_AIR;
      else if (!str_cmp(argument, "water"))
      ch->pcdata->ci_type = SECT_WATER;
      else if (!str_cmp(argument, "underwater"))
      ch->pcdata->ci_type = SECT_UNDERWATER;
      else if (!str_cmp(argument, "shallow"))
      ch->pcdata->ci_type = SECT_SHALLOW;
      else if (!str_cmp(argument, "house"))
      ch->pcdata->ci_type = SECT_HOUSE;
      else if (!str_cmp(argument, "forest"))
      ch->pcdata->ci_type = SECT_FOREST;
      else if (!str_cmp(argument, "beach"))
      ch->pcdata->ci_type = SECT_BEACH;
      else if (!str_cmp(argument, "swamp"))
      ch->pcdata->ci_type = SECT_SWAMP;
      else if (!str_cmp(argument, "cave"))
      ch->pcdata->ci_type = SECT_CAVE;
      else {
        send_to_char("Valid room types are: Club, restaurant, shop, street, alley, warehouse, commerical, house, park, cave, tunnels, cafe, rooftop, basement, hospital, bank, air, water, underwater, forest, beach, swamp, shallow\n\r", ch);
        return;
      }
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 9) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "Bug"))
      ch->pcdata->ci_mod = 0;
      else if (!str_cmp(argument, "Suggestion"))
      ch->pcdata->ci_mod = 1;
      else if (!str_cmp(argument, "Request"))
      ch->pcdata->ci_mod = 2;
      else if (!str_cmp(argument, "Renovate"))
      ch->pcdata->ci_mod = 3;
      else {
        send_to_char("Valid types are: Bug, Suggestion, Request, Renovate.\n\r", ch);
        return;
      }
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 10) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "Plaque"))
      ch->pcdata->ci_mod = 0;
      else if (!str_cmp(argument, "Occupied Grave")) {
        if (!corpsecarrier(ch)) {
          send_to_char("You need to have a not fresh corpse to choose that option.\n\r", ch);
          return;
        }
        ch->pcdata->ci_mod = 1;
      }
      else if (!str_cmp(argument, "Unoccupied Grave"))
      ch->pcdata->ci_mod = 2;
      else {
        send_to_char("Valid types are: Plaque, Occupied Grave, Unoccupied Grave.\n\r", ch);
        return;
      }
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 11) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "control"))
      ch->pcdata->ci_mod = MISSION_CONTROL;
      else if (!str_cmp(argument, "instigate"))
      ch->pcdata->ci_mod = MISSION_INSTIGATE;
      else if (!str_cmp(argument, "Diplomacy"))
      ch->pcdata->ci_mod = MISSION_DIPLOMACY;
      else if (!str_cmp(argument, "Liaise"))
      ch->pcdata->ci_mod = MISSION_LIAISE;
      else {
        send_to_char("Valid types are: Control, instigate, diplomacy, liaise.\n\r", ch);
        return;
      }
    }
    else if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      if (!str_cmp(argument, "intercept"))
      ch->pcdata->ci_type = OPERATION_INTERCEPT;
      else if (!str_cmp(argument, "extract"))
      ch->pcdata->ci_type = OPERATION_EXTRACT;
      else if (!str_cmp(argument, "capture"))
      ch->pcdata->ci_type = OPERATION_CAPTURE;
      else if (!str_cmp(argument, "multiplecapture"))
      ch->pcdata->ci_type = OPERATION_MULTIPLE;
      else {
        send_to_char("Valid types are: Intercept, Extract, Capture, MultipleCapture.\n\r", ch);
        return;
      }
      send_to_char("Done.\n\r", ch);
    }
  }

  // removed brackets and spacing from wear_strings - Discordance
  void custom_to_obj(CHAR_DATA *ch, OBJ_DATA *poi, bool colour) {
    EXTRA_DESCR_DATA *ed;
    //    EXTRA_DESCR_DATA *ped;
    char buf[MSL];
    // char temp[MSL];
    char nocol[MSL];

    if (poi == NULL)
    return;

    int diff = ch->pcdata->ci_cost - poi->cost;

    if (diff > ch->money + ch->pcdata->total_credit && diff > ch->pcdata->total_money && !higher_power(ch) && !is_gm(ch)) {
      send_to_char("You don't have enough money for that.\n\r", ch);
      return;
    }
    if (ch->pcdata->total_credit < diff && ch->pcdata->total_money > diff)
    ch->pcdata->total_money -= diff;
    else {
      if (diff > 50000 && !IS_SET(poi->extra_flags, ITEM_NORESALE))
      SET_BIT(poi->extra_flags, ITEM_NORESALE);
      ch->pcdata->total_credit -= diff;
      if (ch->pcdata->total_credit < 0) {
        ch->money += ch->pcdata->total_credit;
        ch->pcdata->total_credit = 0;
      }
    }

    if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 11 || ch->pcdata->ci_type == 19)
    poi->level = ch->pcdata->ci_layer;

    if (ch->pcdata->ci_type == 10)
    poi->value[2] = ch->pcdata->ci_mod;

    if (ch->pcdata->ci_type == 12) {
      poi->value[0] = ch->pcdata->ci_size;
    }
    else if (ch->pcdata->ci_type == 13) {
      poi->value[1] = ch->pcdata->ci_size;
      poi->value[0] = ch->pcdata->ci_size;
    }
    else if (ch->pcdata->ci_type == 23) {
      poi->value[0] = ch->pcdata->ci_size;
    }
    else if (ch->pcdata->ci_size > 0)
    poi->size = 30;
    else
    poi->size = 5;

    if (ch->pcdata->ci_type == 15) {
      poi->value[0] = 100;
      poi->value[1] = 1;
      poi->value[3] = 300;
    }

    free_string(poi->name);
    remove_color(nocol, ch->pcdata->ci_name);
    poi->name = str_dup(nocol);
    free_string(poi->short_descr);

    if (IS_SET(poi->extra_flags, ITEM_NOINVENTORY))
    REMOVE_BIT(poi->extra_flags, ITEM_NOINVENTORY);

    if (!is_gm(ch) && !higher_power(ch) && colour == FALSE) {
      remove_color(nocol, ch->pcdata->ci_short);
      poi->short_descr = str_dup(nocol);
      free_string(poi->description);
      remove_color(nocol, ch->pcdata->ci_long);
      poi->description = str_dup(nocol);
    }
    else {
      poi->short_descr = str_dup(ch->pcdata->ci_short);
      free_string(poi->description);
      poi->description = str_dup(ch->pcdata->ci_long);
    }

    poi->cost = ch->pcdata->ci_cost;
    sprintf(buf, "%s", ch->pcdata->ci_wear);
    free_string(poi->wear_string);
    poi->wear_string = str_dup(buf);

    if (ch->pcdata->ci_type == 11) {
      poi->value[3] = ch->pcdata->ci_covers;
    }
    if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 19) {
      poi->value[0] = ch->pcdata->ci_covers;
      poi->value[1] = ch->pcdata->ci_zips;
    }
    if (ch->pcdata->ci_type == 23) {
      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("+scent", ed->keyword)) {
          break;
        }
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+scent");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(ch->pcdata->ci_taste);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->ci_taste);
      }
    }
    if (ch->pcdata->ci_type == 21) {
      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+imprint");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(ch->pcdata->ci_imprint);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->ci_imprint);
      }
    }



    if (ch->pcdata->ci_type == 12 || ch->pcdata->ci_type == 13 || ch->pcdata->ci_type == 21) {
      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+taste");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(ch->pcdata->ci_taste);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->ci_taste);
      }
    }
    for (ed = poi->extra_descr; ed; ed = ed->next) {
      if (is_name("all", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("all");
      ed->next = poi->extra_descr;
      poi->extra_descr = ed;
      free_string(poi->extra_descr->description);
      poi->extra_descr->description = str_dup(ch->pcdata->ci_desc);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(ch->pcdata->ci_desc);
    }
    ch->pcdata->ci_editing = 0;

    if (colour == TRUE)
    ch->pcdata->account->colours--;

    send_to_char("Object customized.\n\r", ch);
  }

  void obj_to_custom(CHAR_DATA *ch, OBJ_DATA *obj) {
    EXTRA_DESCR_DATA *ed;

    if (obj == NULL)
    return;

    switch (obj->item_type) {
    case ITEM_CLOTHING:
      if (IS_SET(obj->extra_flags, ITEM_ARMORED))
      ch->pcdata->ci_type = 19;
      else
      ch->pcdata->ci_type = 10;
      break;
    case ITEM_JEWELRY:
      ch->pcdata->ci_type = 11;
      break;
    case ITEM_FOOD:
      ch->pcdata->ci_type = 12;
      break;
    case ITEM_DRINK_CON:
      ch->pcdata->ci_type = 13;
      break;
    case ITEM_TRASH:
      ch->pcdata->ci_type = 14;
      break;
    case ITEM_CONTAINER:
      ch->pcdata->ci_type = 15;
      break;
    case ITEM_WEAPON:
      ch->pcdata->ci_type = 16;
      break;
    case ITEM_RANGED:
      ch->pcdata->ci_type = 17;
      break;
    case ITEM_PHONE:
      ch->pcdata->ci_type = 18;
      break;
    case ITEM_FURNITURE:
      ch->pcdata->ci_type = 20;
      break;
    case ITEM_DRUGS:
      ch->pcdata->ci_type = 21;
      break;
    case ITEM_UMBRELLA:
      ch->pcdata->ci_type = 22;
      break;
    case ITEM_PERFUME:
      ch->pcdata->ci_type = 23;
      break;
    case ITEM_FLASHLIGHT:
      ch->pcdata->ci_type = 24;
      break;
    default:
      ch->pcdata->ci_type = 14;
      break;
    }

    if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 11 || ch->pcdata->ci_type == 19) {
      ch->pcdata->ci_layer = obj->level;
    }
    if (ch->pcdata->ci_type == 10) {
      ch->pcdata->ci_mod = obj->value[2];
    }
    if (ch->pcdata->ci_type == 12) {
      ch->pcdata->ci_size = obj->value[0];
    }
    else if (ch->pcdata->ci_type == 13) {
      ch->pcdata->ci_size = obj->value[0];
      ch->pcdata->ci_alcohol = obj->value[2];
    }
    else if (ch->pcdata->ci_type == 23) {
      ch->pcdata->ci_size = obj->value[0];
    }
    else if (obj->size > 20) {
      ch->pcdata->ci_size = 30;
    }
    else {
      ch->pcdata->ci_size = 0;
    }

    free_string(ch->pcdata->ci_name);
    ch->pcdata->ci_name = str_dup(obj->name);
    free_string(ch->pcdata->ci_short);
    ch->pcdata->ci_short = str_dup(obj->short_descr);
    free_string(ch->pcdata->ci_long);
    ch->pcdata->ci_long = str_dup(obj->description);

    ch->pcdata->ci_cost = obj->cost;

    free_string(ch->pcdata->ci_wear);
    ch->pcdata->ci_wear =
    str_dup(obj->wear_string); // changed this from "" - Disco 6/23/2019

    if (ch->pcdata->ci_type == 11) {
      ch->pcdata->ci_covers = obj->value[3];
    }
    if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 19) {
      ch->pcdata->ci_covers = obj->value[0];
      ch->pcdata->ci_zips = obj->value[1];
    }

    if (ch->pcdata->ci_type == 23) {
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+scent", ed->keyword)) {
          break;
        }
      }

      if (!ed) {
      }
      else {
        free_string(ch->pcdata->ci_taste);
        ch->pcdata->ci_taste = str_dup(ed->description);
      }
    }
    if (ch->pcdata->ci_type == 21) {
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword))
        break;
      }

      if (!ed) {
      }
      else {
        free_string(ch->pcdata->ci_imprint);
        ch->pcdata->ci_imprint = str_dup(ed->description);
      }
    }

    if (ch->pcdata->ci_type == 12 || ch->pcdata->ci_type == 13 || ch->pcdata->ci_type == 21) {
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword))
        break;
      }

      if (!ed) {
      }
      else {
        free_string(ch->pcdata->ci_taste);
        ch->pcdata->ci_taste = str_dup(ed->description);
      }
    }
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("all", ed->keyword))
      break;
    }

    if (!ed) {
    }
    else {
      free_string(ch->pcdata->ci_desc);
      ch->pcdata->ci_desc = str_dup(ed->description);
    }
  }

  int xoffset(int x, int dir) {
    if (dir == DIR_EAST || dir == DIR_NORTHEAST || dir == DIR_SOUTHEAST)
    return x + 1;
    if (dir == DIR_WEST || dir == DIR_NORTHWEST || dir == DIR_SOUTHWEST)
    return x - 1;
    return x;
  }

  int yoffset(int y, int dir) {
    if (dir == DIR_NORTH || dir == DIR_NORTHEAST || dir == DIR_NORTHWEST)
    return y + 1;
    if (dir == DIR_SOUTH || dir == DIR_SOUTHEAST || dir == DIR_SOUTHWEST)
    return y - 1;
    return y;
  }
  int zoffset(int z, int dir) {
    if (dir == DIR_UP)
    return z + 1;
    if (dir == DIR_DOWN)
    return z - 1;

    return z;
  }

  int expensive_item_level(int itype)
  {
    switch(itype) {
    default:
      return 1000;
      break;
    case ITEM_WEAPON:
      return 500;
      break;
    case ITEM_PERFUME:
      return 100;
      break;
    case ITEM_ARMOR:
      return 750;
      break;
    case ITEM_CLOTHING:
      return 350;
      break;
    case ITEM_FURNITURE:
      return 200;
      break;
    case ITEM_CONTAINER:
      return 200;
      break;
    case ITEM_FOOD:
      return 50;
      break;
    case ITEM_DRINK_CON:
      return 25;
      break;
    case ITEM_UMBRELLA:
      return 100;
      break;
    case ITEM_CORPSE_NPC:
      return 100;
      break;
    case ITEM_RANGED:
      return 500;
      break;
    case ITEM_FLASHLIGHT:
      return 100;
      break;
    case ITEM_GAME:
      return 100;
      break;
    case ITEM_PHONE:
      return 750;
      break;
    case ITEM_DRUGS:
      return 50;
      break;
    }
    return 1000;
  }

  int ci_type_to_item_type(int cit)
  {
    int item_type = 0;
    switch (cit) {
    case 10:
    case 19:
      item_type = ITEM_CLOTHING;
      break;
    case 11:
      item_type = ITEM_JEWELRY;
      break;
    case 12:
      item_type = ITEM_FOOD;
      break;
    case 13:
      item_type = ITEM_DRINK_CON;
      break;
    case 14:
      item_type = ITEM_TRASH;
      break;
    case 15:
      item_type = ITEM_CONTAINER;
      break;
    case 16:
      item_type = ITEM_WEAPON;
      break;
    case 17:
      item_type = ITEM_RANGED;
      break;
    case 18:
      item_type = ITEM_PHONE;
      break;
    case 20:
      item_type = ITEM_FURNITURE;
      break;
    case 21:
      item_type = ITEM_DRUGS;
      break;
    case 22:
      item_type = ITEM_UMBRELLA;
      break;
    case 23:
      item_type = ITEM_PERFUME;
      break;
    case 24:
      item_type = ITEM_FLASHLIGHT;
      break;
    case 25:
    case 26:
      item_type = ITEM_GAME;
      break;
    }
    return item_type;
  }

  const char *destiny_list_names_done[8] = {"", "destiny as prey", "destiny as a predator", "destiny as a rival", "social destiny", "adventuruous destiny", "investigatory destiny", "work destiny"};

  _DOFUN(do_done) {
    EXTRA_DESCR_DATA *ed;
    char buf[MSL];
    // char temp[MSL];
    EXTRA_DESCR_DATA *ped;
    PLAYERROOM_TYPE *playerroom;
    int j;
    ROOM_INDEX_DATA *room;
    NEWS_TYPE *news;
    char nocol[MSL];
    OBJ_INDEX_DATA *poi;

    if (ch->in_room->vnum == ROOM_INDEX_GENESIS) {
      if (!readychar(ch)) {
        send_to_char("You're not yet ready to move on, use look to see what you still have to configure.\n\r",ch);
        return;
      }
      if (IS_FLAG(ch->act, PLR_GUEST) && higher_power(ch)) {
        send_to_char("You cannot make that as a guest.\n\r", ch);
        return;
      }
      int cost = charcost(ch);
      int deduct = UMIN(cost, available_pkarma(ch));
      cost -= deduct;
      if (!IS_FLAG(ch->act, PLR_GUEST)) {
        if (!immaccount(ch)) {
          ch->pcdata->account->pkarma -= deduct;
          ch->spentpkarma += deduct;
          ch->pcdata->account->pkarmaspent += deduct;
        }
        if (cost > 0) {
          ch->pcdata->account->karma -= cost;
          ch->spentkarma += cost;
        }
      }
      if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type != GUEST_MONSTER) {
        ch->newexp = get_tier(ch) * 200000;
        ch->newrpexp = get_tier(ch) * 200000;
        if (ch->pcdata->guest_type == GUEST_NIGHTMARE)
        ch->pcdata->murder_cooldown = 15000;
      }
      else if (ch->pcdata->guest_type == GUEST_MONSTER && IS_FLAG(ch->act, PLR_GUEST)) {
        if (time_info.monster_hours <= 0)
        time_info.monster_hours = 30;
        time_info.monster_hours = UMAX(1, time_info.monster_hours - 1);
      }
      else if (higher_power(ch)) {
        ch->newexp += 250000;
        ch->newrpexp += 250000;
      }
      int years = get_real_age(ch) - 18;
      years = UMAX(1, years);
      years = UMIN(years, 22);

      ch->pcdata->total_credit = years * 50000;
      ch->pcdata->total_credit += 100000;
      ch->money = years * 5000;
      ch->pcdata->total_money = years * 5000;
      do_function(ch, &do_prompt, "starter");

      if (IS_FLAG(ch->act, PLR_GUEST)) {
        ch->pcdata->total_credit *= 3;
        ch->money = 0;
        ch->pcdata->total_money = 0;
      }
      ch->pcdata->total_money += ch->money;
      ch->money = 2500;
      ch->pcdata->create_date = current_time;
      ch->pcdata->last_paid = current_time;
      if (ch->modifier != MODIFIER_FLESHFORMED)
      ch->pcdata->fleshformed = 0;

      sprintf(buf, "%s %s", ch->pcdata->last_name, "the Newbie");
      set_whotitle(ch, buf);

      if (!IS_FLAG(ch->comm, COMM_PROMPT))
      SET_FLAG(ch->comm, COMM_PROMPT);

      if (!IS_FLAG(ch->comm, COMM_STORY))
      SET_FLAG(ch->comm, COMM_STORY);

      if (IS_FLAG(ch->comm, COMM_CONSENT))
      REMOVE_FLAG(ch->comm, COMM_CONSENT);

      if (!IS_FLAG(ch->comm, COMM_SUBDUE))
      SET_FLAG(ch->comm, COMM_SUBDUE);

      char_from_room(ch);
      if (ch->race == RACE_FANTASY) {
        char_to_room(ch, get_room_index(405500));
        to_spectre(ch, FALSE);
      }
      else
      char_to_room(ch, get_room_index(50));
      send_to_char("The floor opens up beneath you and you drop down into newbie school!\n\r", ch);
      ch->lifeforce = 10000;
      ch->lf_used = 0;
      ch->lf_taken = 0;
      if (!IS_FLAG(ch->act, PLR_GUEST) && !immaccount(ch))
      ch->pcdata->account->newcharcount--;
      return;
    }
    if (ch->pcdata->survey_stage == 5) {
      ch->pcdata->survey_stage = 6;
      send_to_char("Thank you, you now have the option of adding some text on how you think the player of the character you're reviewing could improve their roleplay. To do this type 'recommend' when you're finished you can type 'done' if you don't want to add an improvement recommendation, simply type 'done' now.\n\r", ch);
      return;
    }
    else if (ch->pcdata->survey_stage == 6) {
      ch->pcdata->survey_stage = 7;
      send_to_char("Thank you, you now have the option of adding a comment for the character you are reviewing. To do this type 'comment' when you're finished you can type 'done' if you don't want to add a comment, simply type 'done' now.\n\r", ch);
      return;
    }
    else if (ch->pcdata->survey_stage == 7) {
      send_to_char("Feedback completed and sent, thank you for your time.\n\r", ch);
      ch->pcdata->survey_stage = 0;
      if (silly_survey(ch))
      ch->pcdata->survey_delay += (3600 * 24 * 21);
      else if (critical_survey(ch))
      ch->pcdata->survey_delay = current_time + (3600 * 24 * 3);
      if (safe_strlen(ch->pcdata->survey_improve) > 2 || safe_strlen(ch->pcdata->survey_comment) > 2) {
        gain_rpexp(ch, 1000);
        give_karma(ch, 100, KARMA_OTHER);
      }
      send_survey(ch, ch->pcdata->surveying);
      return;
    }

    smash_tilde(ch->pcdata->ci_short);
    smash_tilde(ch->pcdata->ci_name);
    smash_tilde(ch->pcdata->ci_long);

    if (ch->pcdata->ci_editing == 0) {
      send_to_char("You're not editing anything.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 1) {
      if (ch->pcdata->ci_cost < 1 && !is_gm(ch)) {
        send_to_char("Nothing in life is free.\n\r", ch);
        return;
      }

      if (ch->pcdata->ci_vnum == 1) {
        custom_to_obj(ch, ch->pcdata->customizing, FALSE);
        return;
      }

      if (!can_decorate(ch, ch->in_room)) {
        send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
        return;
      }
      if (in_prop(ch) == NULL || in_prop(ch)->type != PROP_SHOP) {
        send_to_char("You need to own this shop or have permission to do that.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_name) < 2) {
        send_to_char("You better give it a name or two first.\n\r", ch);
        return;
      }

      int vnum = ch->pcdata->ci_vnum;

      if ((poi = get_obj_index(vnum)) == NULL) {
        send_to_char("No object couldn't be found.\n\r", ch);
        return;
      }

      /*
      for ( ed = poi->extra_descr; ed; ed = ed->next )
      {
      if(ed->next != NULL)
      ped = ed;

      if ( ed )
      {
      //Crashes here
      if ( !ped )
      poi->extra_descr = ed->next;
      else
      ped->next = ed->next;
      free_extra_descr( ed );
      }

      }
      */

      bool shop_color = FALSE;
      int min_col_cost = expensive_item_level(ci_type_to_item_type(ch->pcdata->ci_type))*100;
      if (strcasestr(ch->pcdata->ci_short, "`") != NULL || strcasestr(ch->pcdata->ci_short, "`") != NULL)
      {
        char logs[MSL];
        sprintf(logs, "EXPENSIVE: %d %d %d %d", ch->pcdata->ci_type, ci_type_to_item_type(ch->pcdata->ci_type), ch->pcdata->ci_cost, min_col_cost);
        log_string(logs);
        if(ch->pcdata->ci_cost < min_col_cost)
        {
          printf_to_char(ch, "To create that shop item as a colored item it would need to cost at least %d dollars.\n\r", min_col_cost/100);
          return;
        }
        shop_color = TRUE;
      }

      int value;
      if (IS_SET(poi->wear_flags, ITEM_WEAR_BODY)) {
        value = flag_value(wear_flags, "body");
        TOGGLE_BIT(poi->wear_flags, value);
      }
      if (IS_SET(poi->wear_flags, ITEM_HOLD)) {
        value = flag_value(wear_flags, "hold");
        TOGGLE_BIT(poi->wear_flags, value);
      }

      if (IS_SET(poi->extra_flags, ITEM_LARGE))
      REMOVE_BIT(poi->extra_flags, ITEM_LARGE);
      if (IS_SET(poi->extra_flags, ITEM_ARMORED))
      REMOVE_BIT(poi->extra_flags, ITEM_ARMORED);

      poi->value[0] = 0;
      poi->value[1] = 0;
      poi->value[2] = 0;
      poi->value[3] = 0;
      poi->value[4] = 0;

      poi->size = 5;
      poi->cost = 1;

      switch (ch->pcdata->ci_type) {
      case 10:
      case 19:
        poi->item_type = ITEM_CLOTHING;
        break;
      case 11:
        poi->item_type = ITEM_JEWELRY;
        break;
      case 12:
        poi->item_type = ITEM_FOOD;
        break;
      case 13:
        poi->item_type = ITEM_DRINK_CON;
        break;
      case 14:
        poi->item_type = ITEM_TRASH;
        break;
      case 15:
        poi->item_type = ITEM_CONTAINER;
        break;
      case 16:
        poi->item_type = ITEM_WEAPON;
        break;
      case 17:
        poi->item_type = ITEM_RANGED;
        break;
      case 18:
        poi->item_type = ITEM_PHONE;
        break;
      case 20:
        poi->item_type = ITEM_FURNITURE;
        break;
      case 21:
        poi->item_type = ITEM_DRUGS;
        break;
      case 22:
        poi->item_type = ITEM_UMBRELLA;
        break;
      case 23:
        poi->item_type = ITEM_PERFUME;
        break;
      case 24:
        poi->item_type = ITEM_FLASHLIGHT;
        break;
      case 25:
      case 26:
        poi->item_type = ITEM_GAME;
        break;
      }

      if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 11 || ch->pcdata->ci_type == 19 || ch->pcdata->ci_type == 15) {
        value = flag_value(wear_flags, "body");
        TOGGLE_BIT(poi->wear_flags, value);
      }
      else {
        value = flag_value(wear_flags, "hold");
        TOGGLE_BIT(poi->wear_flags, value);
      }
      if (ch->pcdata->ci_type == 25) {
        poi->value[0] = 0;
      }
      if (ch->pcdata->ci_type == 26) {
        poi->value[0] = 1;
      }

      if (ch->pcdata->ci_type == 19)
      SET_BIT(poi->extra_flags, ITEM_ARMORED);
      if (ch->pcdata->ci_type == 20) {
        SET_BIT(poi->extra_flags, ITEM_LARGE);
        poi->value[0] = 5;
        poi->value[2] = get_obj_index(45121)->value[2];
      }
      if (ch->pcdata->ci_type == 24)
      poi->value[0] = 250;

      if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 11 || ch->pcdata->ci_type == 19)
      poi->level = ch->pcdata->ci_layer;

      if (ch->pcdata->ci_type == 10)
      poi->value[2] = ch->pcdata->ci_mod;

      if (ch->pcdata->ci_type == 12) {
        poi->value[0] = ch->pcdata->ci_size;
      }
      else if (ch->pcdata->ci_type == 13) {
        poi->value[1] = ch->pcdata->ci_size;
        poi->value[0] = ch->pcdata->ci_size;
        poi->value[2] = ch->pcdata->ci_alcohol;
      }
      else if (ch->pcdata->ci_type == 23) {
        poi->value[0] = ch->pcdata->ci_size;
      }
      else if (ch->pcdata->ci_size > 0)
      poi->size = 30;

      if (ch->pcdata->ci_type == 15) {
        poi->value[0] = 100;
        poi->value[1] = 1;
        poi->value[3] = 300;
      }

      free_string(poi->name);
      remove_color(nocol, ch->pcdata->ci_name);
      poi->name = str_dup(nocol);
      free_string(poi->short_descr);

      if (!IS_IMMORTAL(ch)) { // vector smashing to prevent MXP tag abuse
        smash_vector(ch->pcdata->ci_short);
        smash_vector(ch->pcdata->ci_name);
        smash_vector(ch->pcdata->ci_long);
        smash_vector(ch->pcdata->ci_desc);
        smash_vector(ch->pcdata->ci_wear);
        smash_vector(ch->pcdata->ci_taste);
        smash_vector(ch->pcdata->ci_imprint);
      }

      if (!is_gm(ch) && !higher_power(ch) && shop_color == FALSE) {
        remove_color(nocol, ch->pcdata->ci_short);
        poi->short_descr = str_dup(nocol);
        free_string(poi->description);
        remove_color(nocol, ch->pcdata->ci_long);
        poi->description = str_dup(nocol);
      }
      else {
        poi->short_descr = str_dup(ch->pcdata->ci_short);
        free_string(poi->description);
        poi->description = str_dup(ch->pcdata->ci_long);
      }

      poi->cost = ch->pcdata->ci_cost;

      if (safe_strlen(ch->pcdata->ci_wear) < 2) {
        sprintf(buf, "%s", "worn");
        // sprintf(buf, "%s", "worn");
        // sprintf(buf, "%-21.21s", buf);
        free_string(poi->wear_string);
        poi->wear_string = str_dup(buf);
      }
      else {
        remove_color(nocol, ch->pcdata->ci_wear);
        sprintf(buf, "%s", nocol);
        // sprintf(buf, "%s", ch->pcdata->ci_wear);
        // sprintf(buf, "%-21.21s", buf);
        free_string(poi->wear_string);
        poi->wear_string = str_dup(buf);
      }

      if (ch->pcdata->ci_type == 11) {
        poi->value[3] = ch->pcdata->ci_covers;
      }
      if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 19) {
        poi->value[0] = ch->pcdata->ci_covers;
        poi->value[1] = ch->pcdata->ci_zips;
      }
      if (ch->pcdata->ci_type == 23) {
        for (ed = poi->extra_descr; ed; ed = ed->next) {
          if (is_name("+scent", ed->keyword))
          break;
        }

        if (!ed) {
          ed = new_extra_descr();
          ed->keyword = str_dup("+scent");
          ed->next = poi->extra_descr;
          poi->extra_descr = ed;
          free_string(poi->extra_descr->description);
          poi->extra_descr->description = str_dup(ch->pcdata->ci_taste);
        }
        else {
          free_string(ed->description);
          ed->description = str_dup(ch->pcdata->ci_taste);
        }
      }
      if (ch->pcdata->ci_type == 21) {
        for (ed = poi->extra_descr; ed; ed = ed->next) {
          if (is_name("+imprint", ed->keyword))
          break;
        }

        if (!ed) {
          ed = new_extra_descr();
          ed->keyword = str_dup("+imprint");
          ed->next = poi->extra_descr;
          poi->extra_descr = ed;
          free_string(poi->extra_descr->description);
          poi->extra_descr->description = str_dup(ch->pcdata->ci_imprint);
        }
        else {
          free_string(ed->description);
          ed->description = str_dup(ch->pcdata->ci_imprint);
        }
      }

      if (ch->pcdata->ci_type == 12 || ch->pcdata->ci_type == 13 || ch->pcdata->ci_type == 21) {
        for (ed = poi->extra_descr; ed; ed = ed->next) {
          if (is_name("+taste", ed->keyword))
          break;
        }

        if (!ed) {
          ed = new_extra_descr();
          ed->keyword = str_dup("+taste");
          ed->next = poi->extra_descr;
          poi->extra_descr = ed;
          free_string(poi->extra_descr->description);
          poi->extra_descr->description = str_dup(ch->pcdata->ci_taste);
        }
        else {
          free_string(ed->description);
          ed->description = str_dup(ch->pcdata->ci_taste);
        }
      }
      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("all", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("all");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(ch->pcdata->ci_desc);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->ci_desc);
      }

      ch->pcdata->ci_editing = 0;
      send_to_char("Object made!", ch);
    }
    else if (ch->pcdata->ci_editing == 2) {
      int distx = ch->in_room->locx - ch->pcdata->ci_x;
      int disty = ch->in_room->locy - ch->pcdata->ci_y;

      distx = distx * distx;
      disty = disty * disty;
      int dist = distx + disty;
      dist = (int)sqrt((double)dist);

      for (j = get_area_data(ch->pcdata->ci_area)->min_vnum; !freeplayerroom(j);
      j++) {
      }
      if (j > get_area_data(ch->pcdata->ci_area)->max_vnum) {
        send_to_char("No space.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_status != ROOM_PUBLIC && ch->pcdata->ci_area == DIST_DREAM) {
        send_to_char("You can't make adjacent rooms in the dreamscape, it has to be status public.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_status != ROOM_PUBLIC && ch->pcdata->ci_status != 0 && in_haven(ch->in_room) && ch->pcdata->ci_area != DIST_MISTS) {
        send_to_char("You can't do this from inside Haven.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_status != ROOM_PUBLIC && (ch->in_room->area->vnum == HELL_FOREST_VNUM || ch->in_room->area->vnum == OTHER_FOREST_VNUM || ch->in_room->area->vnum == GODREALM_FOREST_VNUM || ch->in_room->area->vnum == WILDS_FOREST_VNUM)) {
        send_to_char("You can't do that from here.\n\r", ch);
        return;
      }
      room = get_room_index(j);
      clean_room(room);
      room->locx = ch->pcdata->ci_x;
      room->locy = ch->pcdata->ci_y;
      if (IS_SET(room->room_flags, ROOM_INDOORS))
      REMOVE_BIT(room->room_flags, ROOM_INDOORS);
      room->sector_type = ch->pcdata->ci_type;

      if (ch->pcdata->ci_status != ROOM_PUBLIC && ch->pcdata->ci_status != 0 && ch->in_room->area->vnum == ch->pcdata->ci_area) {
        if (ch->in_room->exit[ch->pcdata->ci_x] != NULL) {
          send_to_char("This room already has an exit in that direction.\n\r", ch);
          return;
        }
        ch->in_room->exit[ch->pcdata->ci_x] = new_exit();
        ch->in_room->exit[ch->pcdata->ci_x]->u1.to_room = room;
        ch->in_room->exit[ch->pcdata->ci_x]->orig_door = ch->pcdata->ci_x;

        if (!room->exit[rev_dir[ch->pcdata->ci_x]]) {
          room->exit[rev_dir[ch->pcdata->ci_x]] = new_exit();

          room->x = xoffset(ch->in_room->x, ch->pcdata->ci_x);
          room->y = yoffset(ch->in_room->y, ch->pcdata->ci_x);
          room->z = zoffset(ch->in_room->z, ch->pcdata->ci_x);

          room->exit[rev_dir[ch->pcdata->ci_x]]->u1.to_room = ch->in_room;
          room->exit[rev_dir[ch->pcdata->ci_x]]->orig_door =
          rev_dir[ch->pcdata->ci_x];
        }

        room->locx = ch->in_room->locx;
        room->locy = ch->in_room->locy;
      }
      if (!IS_SET(room->room_flags, ROOM_PUBLIC))
      SET_BIT(room->room_flags, ROOM_PUBLIC);

      for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
      it != FantasyVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->active == FALSE)
        continue;

        for (int i = 0; i < 10; i++) {
          if (room->vnum == (*it)->rooms[i])
          (*it)->rooms[i] = 0;
        }
      }

      tm *ptm;
      time_t east_time;
      east_time = current_time;
      ptm = gmtime(&east_time);
      int year = ptm->tm_year + 1900;

      if (ch->pcdata->ci_cost == year)
      room->time = ch->in_room->time;
      else
      room->time = ch->pcdata->ci_cost;

      room->size = 50;
      if (room->sector_type == SECT_HOUSE) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 35;

      }
      else if (room->sector_type == SECT_CLUB) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 35;
      }
      else if (room->sector_type == SECT_RESTERAUNT) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_SHOP) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 20;
        room->entryy = 0;
      }
      else if (room->sector_type == SECT_STREET) {
        room->entryx = 100;
        room->entryy = 0;
      }
      else if (room->sector_type == SECT_ALLEY) {
        room->entryx = 50;
        room->entryy = 0;
      }
      else if (room->sector_type == SECT_WAREHOUSE) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 50;
      }
      else if (room->sector_type == SECT_COMMERCIAL) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 15;
        room->entryy = 0;
      }
      else if (room->sector_type == SECT_PARK) {
        room->entryx = 0;
        room->entryy = 100;
      }
      else if (room->sector_type == SECT_TUNNELS) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 30;
      }
      else if (room->sector_type == SECT_CAVE) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 30;
      }
      else if (room->sector_type == SECT_CAFE) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 0;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_ROOFTOP) {
        room->entryx = 50;
        room->entryy = 50;
      }
      else if (room->sector_type == SECT_BASEMENT) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 20;
        room->entryy = 20;
      }
      else if (room->sector_type == SECT_HOSPITAL) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 10;
        room->entryy = 20;
      }
      else if (room->sector_type == SECT_BANK) {
        SET_BIT(room->room_flags, ROOM_INDOORS);
        room->entryx = 15;
        room->entryy = 15;
      }
      else if (room->sector_type == SECT_AIR) {
        room->entryx = 40;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_WATER) {
        room->entryx = 40;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_UNDERWATER) {
        room->entryx = 40;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_SHALLOW) {
        room->entryx = 40;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_FOREST) {
        room->entryx = 40;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_BEACH) {
        room->entryx = 40;
        room->entryy = 40;
      }
      else if (room->sector_type == SECT_SWAMP) {
        room->entryx = 40;
        room->entryy = 40;
      }

      for (ed = room->extra_descr; ed; ed = ed->next) {
        ped = ed;

        if (!ed) {
        }
        else {
          if (!ped)
          room->extra_descr = ed->next;
          else
          ped->next = ed->next;

          free_extra_descr(ed);
        }
      }

      free_string(room->name);
      room->name = str_dup(ch->pcdata->ci_name);
      free_string(room->description);
      room->description = str_dup(ch->pcdata->ci_desc);
      room->x = 1000;
      room->y = 1000;
      room->z = 1000;
      SET_BIT(room->area->area_flags, AREA_CHANGED);
      save_area(room->area, FALSE);
      playerroom = new_playerroom();
      free_string(playerroom->author);
      playerroom->author = str_dup(ch->name);
      playerroom->cooldown = 10000;
      playerroom->vnum = room->vnum;
      playerroom->status = ch->pcdata->ci_status;
      PlayerroomVect.push_back(playerroom);
      ch->pcdata->ci_editing = 0;

      send_to_char("Room made.\n\r", ch);
      if (!IS_SET(room->area->area_flags, AREA_CHANGED))
      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (ch->pcdata->ci_editing == 3) {
      int i;
      for (i = 0; i < 25 && safe_strlen(ch->pcdata->customstrings[i][0]) > 1; i++) {
      }

      if (i == 25) {
        send_to_char("You'll have to delete an old attack first.\n\r", ch);
        return;
      }

      if (ch->pcdata->ci_special != SPECIAL_MINION && ch->pcdata->ci_special != SPECIAL_ALLY && ch->pcdata->ci_special != 0) {
        int j, k, l, m;
        int uniques[15];

        for (m = 0; m < MAX_SPECIAL; m++) {
          if (special_table[m].vnum == ch->pcdata->ci_special)
          j = m;
        }

        for (k = 0; k < 25; k++) {
          for (l = 0; l < 15; l++)
          uniques[l] = 0;

          if (ch->pcdata->ci_discipline == ch->pcdata->customstats[k][1] && ch->pcdata->customstats[k][0] == special_table[j].vnum) {
            break;
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
        /*
        for(l=0;l<15;l++)
        {
        if(uniques[l] > 0 && uniques[l] != SPECIAL_MINION && uniques[l] !=
        SPECIAL_ALLY) count+=1;
        }
        if(count >= max_specials(ch, ch->pcdata->ci_discipline))
        {
        send_to_char("Your discipline isn't high enough to support that many
        different specials.\n\r", ch); return;
        }
        */
      }
      free_string(ch->pcdata->customstrings[i][0]);
      ch->pcdata->customstrings[i][0] = str_dup(ch->pcdata->ci_name);
      free_string(ch->pcdata->customstrings[i][1]);
      ch->pcdata->customstrings[i][1] = str_dup(ch->pcdata->ci_myself);
      free_string(ch->pcdata->customstrings[i][2]);
      ch->pcdata->customstrings[i][2] = str_dup(ch->pcdata->ci_target);
      free_string(ch->pcdata->customstrings[i][3]);
      ch->pcdata->customstrings[i][3] = str_dup(ch->pcdata->ci_bystanders);
      free_string(ch->pcdata->customstrings[i][4]);
      ch->pcdata->customstrings[i][4] = str_dup(ch->pcdata->ci_myselfdelayed);
      free_string(ch->pcdata->customstrings[i][5]);
      ch->pcdata->customstrings[i][5] = str_dup(ch->pcdata->ci_targetdelayed);
      free_string(ch->pcdata->customstrings[i][6]);
      ch->pcdata->customstrings[i][6] = str_dup(ch->pcdata->ci_bystandersdelayed);

      ch->pcdata->customstats[i][1] = ch->pcdata->ci_discipline;
      ch->pcdata->customstats[i][0] = ch->pcdata->ci_special;

      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 4) {
      int i;
      for (i = 0; i < 5 && safe_strlen(ch->pcdata->monster_names[0][i]) > 1; i++) {
      }

      if (i == 5) {
        send_to_char("You'll have to delete an old monster first.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->monster_names[0][i]);
      ch->pcdata->monster_names[0][i] = str_dup(ch->pcdata->ci_name);
      free_string(ch->pcdata->monster_names[1][i]);
      ch->pcdata->monster_names[1][i] = str_dup(ch->pcdata->ci_short);
      free_string(ch->pcdata->monster_names[2][i]);
      ch->pcdata->monster_names[2][i] = str_dup(ch->pcdata->ci_desc);
      ch->pcdata->monster_discs[0][i] = ch->pcdata->ci_discipline;
      ch->pcdata->monster_discs[1][i] = ch->pcdata->ci_disclevel;
      ch->pcdata->monster_discs[2][i] = ch->pcdata->ci_discipline2;
      ch->pcdata->monster_discs[3][i] = ch->pcdata->ci_disclevel2;
      ch->pcdata->monster_discs[4][i] = ch->pcdata->ci_discipline3;
      ch->pcdata->monster_discs[5][i] = ch->pcdata->ci_disclevel3;

      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 5) {
      int i = 0;
      if (making_minion_cost(ch) > minion_exp_cap(ch)) {
        printf_to_char(ch, "The Exp cost for this minion cannot exceed %d.\n\r", minion_exp_cap(ch));
        return;
      }
      if (making_minion_shield(ch) > minion_shield_cap(ch)) {
        printf_to_char(ch, "The total defensive disciplines for this minion cannot exceed %d.\n\r", minion_shield_cap(ch));
        return;
      }

      free_string(ch->pcdata->monster_names[0][i]);
      ch->pcdata->monster_names[0][i] = str_dup(ch->pcdata->ci_name);
      free_string(ch->pcdata->monster_names[1][i]);
      ch->pcdata->monster_names[1][i] = str_dup(ch->pcdata->ci_short);
      free_string(ch->pcdata->monster_names[2][i]);
      ch->pcdata->monster_names[2][i] = str_dup(ch->pcdata->ci_desc);
      ch->pcdata->monster_discs[0][i] = ch->pcdata->ci_discipline;
      ch->pcdata->monster_discs[1][i] = ch->pcdata->ci_disclevel;
      ch->pcdata->monster_discs[2][i] = ch->pcdata->ci_discipline2;
      ch->pcdata->monster_discs[3][i] = ch->pcdata->ci_disclevel2;
      ch->pcdata->monster_discs[4][i] = ch->pcdata->ci_discipline3;
      ch->pcdata->monster_discs[5][i] = ch->pcdata->ci_disclevel3;
      ch->pcdata->monster_discs[6][i] = ch->pcdata->ci_discipline4;
      ch->pcdata->monster_discs[7][i] = ch->pcdata->ci_disclevel4;
      ch->pcdata->monster_discs[8][i] = ch->pcdata->ci_discipline5;
      ch->pcdata->monster_discs[9][i] = ch->pcdata->ci_disclevel5;

      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 6) {
      int i = 1;

      if (making_minion_cost(ch) > muscle_exp_cap(ch)) {
        printf_to_char(ch, "The Exp cost for this ally cannot exceed %d.\n\r", minion_exp_cap(ch));
        return;
      }

      free_string(ch->pcdata->monster_names[0][i]);
      ch->pcdata->monster_names[0][i] = str_dup(ch->pcdata->ci_name);
      free_string(ch->pcdata->monster_names[1][i]);
      ch->pcdata->monster_names[1][i] = str_dup(ch->pcdata->ci_short);
      free_string(ch->pcdata->monster_names[2][i]);
      ch->pcdata->monster_names[2][i] = str_dup(ch->pcdata->ci_desc);
      ch->pcdata->monster_discs[0][i] = ch->pcdata->ci_discipline;
      ch->pcdata->monster_discs[1][i] = ch->pcdata->ci_disclevel;
      ch->pcdata->monster_discs[2][i] = ch->pcdata->ci_discipline2;
      ch->pcdata->monster_discs[3][i] = ch->pcdata->ci_disclevel2;
      ch->pcdata->monster_discs[4][i] = ch->pcdata->ci_discipline3;
      ch->pcdata->monster_discs[5][i] = ch->pcdata->ci_disclevel3;
      ch->pcdata->monster_discs[6][i] = ch->pcdata->ci_discipline4;
      ch->pcdata->monster_discs[7][i] = ch->pcdata->ci_disclevel4;
      ch->pcdata->monster_discs[8][i] = ch->pcdata->ci_discipline5;
      ch->pcdata->monster_discs[9][i] = ch->pcdata->ci_disclevel5;

      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 7) {
      time_t east_time;
      east_time = current_time;
      news = new_news();
      free_string(news->author);
      if (ch->pcdata->ci_stats[0] == -2)
      news->author = str_dup("Supernatural News");
      else if (ch->pcdata->ci_stats[0] == -3)
      news->author = str_dup("Arranged Hit");
      else if (ch->pcdata->ci_stats[0] == -4)
      news->author = str_dup("Town News");
      else if (ch->pcdata->ci_stats[0] == -5)
      news->author = str_dup("Supernatural Town News");
      else
      news->author = str_dup(ch->name);
      news->timer = 2000;
      if (!str_cmp(ch->name, "Tyr"))
      news->timer = 5000;

      if (ch->pcdata->ci_stats[0] == -3) {
        ch->pcdata->ci_stats[0] = 0;
        ch->pcdata->legendary_cool = current_time + (3600 * 24 * 21);
      }
      if (ch->pcdata->ci_stats[0] == -4)
      ch->pcdata->ci_stats[0] = 0;
      if (ch->pcdata->ci_stats[0] == -5)
      ch->pcdata->ci_stats[0] = -2;
      for (int i = 0; i < 10; i++)
      news->stats[i] = ch->pcdata->ci_stats[i];
      free_string(news->message);
      sprintf(buf, "%s\n%s", ch->pcdata->ci_desc, (char *)ctime(&east_time));
      news->message = str_dup(buf);
      NewsVect.push_back(news);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

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

        if (ch == victim)
        continue;

        if (can_see_news(victim, news))
        printf_to_char(victim, "`WBreaking news!`x\n\r%s - (%s)\n\r", news->message, news->author);
      }

    }
    else if (ch->pcdata->ci_editing == 8) {
      if (safe_strlen(ch->pcdata->ci_name) < 3) {
        send_to_char("This research article requires a title.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_taste) < 3) {
        send_to_char("This research article requires some keywords.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_desc) < 3) {
        send_to_char("This research article requires some text.\n\r", ch);
        return;
      }

      long number = get_pc_id();
      ARCHIVE_INDEX *archive_index = new_archive_index();
      archive_index->number = number;
      free_string(archive_index->title);
      archive_index->title = str_dup(ch->pcdata->ci_name);
      free_string(archive_index->keywords);
      archive_index->keywords = str_dup(ch->pcdata->ci_taste);
      archive_index->valid = TRUE;
      ArchiveVect.push_back(archive_index);

      ARCHIVE_ENTRY *archive_entry = new_archive_entry();
      archive_entry->number = number;
      free_string(archive_entry->title);
      archive_entry->title = str_dup(ch->pcdata->ci_name);
      free_string(archive_entry->owner);
      archive_entry->owner = str_dup(ch->name);
      free_string(archive_entry->text);
      archive_entry->text = str_dup(ch->pcdata->ci_desc);
      archive_entry->valid = TRUE;
      save_archive_entry(archive_entry);
      research_reward(ch, ch->pcdata->ci_name, ch->pcdata->ci_desc);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 9) {
      PETITION_TYPE *petition = new_petition();
      free_string(petition->author);
      petition->author = str_dup(ch->name);
      if (ch->pcdata->account != NULL) {
        free_string(petition->account);
        petition->account = str_dup(ch->pcdata->account->name);
      }
      petition->type = ch->pcdata->ci_mod;
      petition->status = 0;
      free_string(petition->description);
      petition->description = str_dup(ch->pcdata->ci_desc);
      if (ch->in_room != NULL)
      petition->room = ch->in_room->vnum;
      petition->create_time = current_time;
      if (safe_strlen(ch->pcdata->ci_name) > 1) {
        free_string(petition->sentto);
        petition->sentto = str_dup(ch->pcdata->ci_name);
      }
      PetitionVect.push_back(petition);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 10) {
      if (!gravesite(ch->in_room)) {
        send_to_char("You can't do that here.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_mod == 1 && !corpsecarrier(ch)) {
        send_to_char("You need a not fresh corpse to do that.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_mod == 0 && ch->money < 50000 && ch->pcdata->total_money < 50000) {
        send_to_char("You'd need at least $500 to do that.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_mod == 1 && ch->money < 250000 && ch->pcdata->total_money < 250000) {
        send_to_char("You'd need at least $2500 to do that.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_mod == 2 && ch->money < 500000 && ch->pcdata->total_money < 500000) {
        send_to_char("You'd need at least $5000 to do that.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_name) < 2 || safe_strlen(ch->pcdata->ci_taste) < 2 || safe_strlen(ch->pcdata->ci_desc) < 2) {
        send_to_char("You haven't finished yet.\n\r", ch);
        return;
      }
      sprintf(buf, "%s %s", ch->pcdata->ci_name, ch->pcdata->ci_taste);
      if (gravename(ch, buf)) {
        send_to_char("Someone with that name is already commemorated here.\n\r", ch);
        return;
      }

      if (ch->pcdata->ci_mod == 0 && ch->money >= 50000)
      ch->money -= 50000;
      else if (ch->pcdata->ci_mod == 0)
      ch->pcdata->total_money -= 50000;
      if (ch->pcdata->ci_mod == 1 && ch->money >= 250000)
      ch->money -= 250000;
      else if (ch->pcdata->ci_mod == 1)
      ch->pcdata->total_money -= 250000;
      if (ch->pcdata->ci_mod == 2 && ch->money >= 500000)
      ch->money -= 500000;
      else if (ch->pcdata->ci_mod == 2)
      ch->pcdata->total_money -= 500000;

      if (ch->pcdata->ci_mod == 1)
      trashcorpse(ch);

      GRAVE_TYPE *grave = new_grave();
      free_string(grave->name);
      sprintf(buf, "%s %s", ch->pcdata->ci_name, ch->pcdata->ci_taste);
      grave->name = str_dup(buf);
      free_string(grave->description);
      grave->description = str_dup(ch->pcdata->ci_desc);
      grave->room = ch->in_room->vnum;
      grave->lastvisit = current_time;
      grave->type = ch->pcdata->ci_mod;
      GraveVect.push_back(grave);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 12) {
      if (ch->pcdata->ci_discipline2 != GOAL_PSYCHIC && (safe_strlen(ch->pcdata->ci_desc) < 20 || safe_strlen(ch->pcdata->ci_long) < 5 || safe_strlen(ch->pcdata->ci_name) < 5 || safe_strlen(ch->pcdata->ci_short) < 3)) {
        send_to_char("You haven't finished setting that up yet.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_alcohol == 7 || ch->pcdata->ci_alcohol == 17) {
        send_to_char("You can't run an operation at that hour.\n\r", ch);
        return;
      }
      int spam = 0;
      for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
      it != OpVect.end(); ++it) {
        if ((*it)->hour == 0)
        continue;

        if ((*it)->hour == ch->pcdata->ci_alcohol && (*it)->day == ch->pcdata->ci_vnum) {
          send_to_char("There's already an operation set to depart then.\n\r", ch);
          return;
        }
        if ((*it)->goal == GOAL_FOOTHOLD && (*it)->territoryvnum ==
            number_from_territory(get_loc(ch->pcdata->ci_short))) {
          send_to_char("You have to wait for the foothold battle to be resolved first.\n\r", ch);
          return;
        }
        if ((*it)->goal == GOAL_UPROOT && (*it)->territoryvnum ==
            number_from_territory(get_loc(ch->pcdata->ci_short))) {
          send_to_char("You have to wait for the uproot battle to be resolved first.\n\r", ch);
          return;
        }
        if (ch->pcdata->ci_discipline2 != GOAL_PSYCHIC && (*it)->goal == GOAL_PSYCHIC) {
          send_to_char("You have to wait for the previous psychic battle to be resolved first.\n\r", ch);
          return;
        }
        if ((*it)->faction == ch->faction && (*it)->territoryvnum ==
            number_from_territory(get_loc(ch->pcdata->ci_short)) && ch->pcdata->ci_vnum < 5)
        spam = 1;
      }
      if (ch->pcdata->ci_discipline2 != GOAL_PSYCHIC) {
        if ((ch->pcdata->ci_type == OPERATION_INTERCEPT || ch->pcdata->ci_type == OPERATION_EXTRACT) && safe_strlen(ch->pcdata->ci_myself) < 3) {
          send_to_char("You have to name the item being fought over first, this will appear as 'X has (what you enter)'\n\r", ch);
          return;
        }
        if ((ch->pcdata->ci_type == OPERATION_CAPTURE || ch->pcdata->ci_type == OPERATION_MULTIPLE) && safe_strlen(ch->pcdata->ci_target) < 3) {
          send_to_char("You have to name the process to be completed first, this will appear has 'X continues (what you enter)'\n\r", ch);
          return;
        }
      }
      if (ch->pcdata->ci_discipline == COMPETE_CLOSED)
      ch->pcdata->ci_discipline2 = GOAL_CONTROL;

      int goal = ch->pcdata->ci_discipline2;
      LOCATION_TYPE *floc;
      floc = get_loc(ch->pcdata->ci_short);
      if (goal == 0 && ch->pcdata->ci_discipline != COMPETE_CLOSED) {
        send_to_char("That operation has no goal.\n\r", ch);
        return;
      }
      if (goal == GOAL_PSYCHIC) {
        if (ch->pcdata->ci_vnum < 2) {
          send_to_char("That needs to be in at least 2 days time.\n\r", ch);
          return;
        }

        if (safe_strlen(ch->pcdata->ci_message) < 2 || !valid_psychic_target(ch->pcdata->ci_message)) {
          send_to_char("No such valid target for psychic assault, they may have been assaulted too recently.\n\r", ch);
          return;
        }
        if (ch->pcdata->ci_zips < 5) {
          send_to_char("That operation needs to be at least speed 5.\n\r", ch);
          return;
        }
        OPERATION_TYPE *op = new_operation();
        free_string(op->author);
        op->author = str_dup(ch->name);
        LOCATION_TYPE *loc = get_loc(ch->pcdata->ci_short);
        op->territoryvnum = number_from_territory(loc);
        free_string(op->room_name);
        op->room_name = str_dup("The Streets of Haven");
        free_string(op->description);
        free_string(op->preferred);
        op->preferred = str_dup(ch->pcdata->ci_bystanders);
        free_string(op->target);
        op->target = str_dup(ch->pcdata->ci_message);
        op->type = GOAL_PSYCHIC;
        op->goal = ch->pcdata->ci_discipline2;
        op->max_pcs = 10;
        op->speed = ch->pcdata->ci_zips;
        op->terrain = ch->pcdata->ci_cost;
        op->size = ch->pcdata->ci_layer;
        op->hour = ch->pcdata->ci_alcohol;
        op->day = ch->pcdata->ci_vnum;
        op->faction = ch->faction;
        op->competition = COMPETE_OPEN;
        if (op->competition != COMPETE_CLOSED) {
          ch->pcdata->week_tracker[TRACK_OPERATIONS_CREATED]++;
          ch->pcdata->life_tracker[TRACK_OPERATIONS_CREATED]++;
        }

        OpVect.push_back(op);
        ch->pcdata->ci_editing = 0;
        send_to_char("Done.\n\r", ch);

        return;
      }
      if (floc->lockout > current_time) {
        send_to_char("That territory is still locked out for operations.\n\r", ch);
        return;
      }
      if (goal == GOAL_KIDNAP) {
        if (safe_strlen(ch->pcdata->ci_message) < 2 || daysidle(ch->pcdata->ci_message) > 30) {
          send_to_char("No such target for kidnapping.\n\r", ch);
          return;
        }
        int hometerr =
        number_from_territory(offline_territory(ch->pcdata->ci_message));
        if (hometerr != number_from_territory(floc)) {
          send_to_char("Nobody close to your target lives there.\n\r", ch);
          return;
        }
      }
      if (goal == GOAL_ASSASSINATE) {
        if (safe_strlen(ch->pcdata->ci_message) < 2) {
          send_to_char("No such target for assassination provided.\n\r", ch);
          return;
        }
        int count = 0;
        for (int i = 0; i < 20; i++) {
          if (safe_strlen(floc->plant_desc[i]) > 2 && strcasestr(floc->plant_desc[i], ch->pcdata->ci_message) != NULL)
          count++;
        }
        if (count == 0) {
          send_to_char("No such target.\n\r", ch);
          return;
        }
        if (count > 1) {
          send_to_char("Multiple people were identified as possible targets, be more specific.\n\r", ch);
          return;
        }
      }
      if (goal == GOAL_RAZE) {
        if (safe_strlen(ch->pcdata->ci_message) < 2) {
          send_to_char("No such target for razing provided.\n\r", ch);
          return;
        }
        int count = 0;
        for (int i = 0; i < 20; i++) {
          if (safe_strlen(floc->place_desc[i]) > 2 && strcasestr(floc->place_desc[i], ch->pcdata->ci_message) != NULL)
          count++;
        }
        if (count == 0) {
          send_to_char("No such target.\n\r", ch);
          return;
        }
        if (count > 1) {
          send_to_char("Multiple places were identified as possible targets, be more specific.\n\r", ch);
          return;
        }
      }
      if (goal == GOAL_RESCUE) {
        if (safe_strlen(ch->pcdata->ci_message) < 2) {
          send_to_char("No target for rescuing provided.\n\r", ch);
          return;
        }
        bool found = FALSE;
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if ((*it)->vnum == 0) {
            continue;
          }
          for (int i = 0; i < 20; i++) {
            if (!str_cmp((*it)->kidnap_name[i], ch->pcdata->ci_message) && !str_cmp(floc->name, (*it)->kidnap_territory[i]))
            found = TRUE;
          }
        }
        if (found == FALSE) {
          send_to_char("No such person has a kidnap victim in that territory.\n\r", ch);
          return;
        }
      }

      if (ch->pcdata->ci_vnum < 7) {
        if (ch->pcdata->ci_discipline2 == GOAL_FOOTHOLD || ch->pcdata->ci_discipline2 == GOAL_UPROOT) {
          send_to_char("That needs to be in at least 7 days time.\n\r", ch);
          return;
        }
      }
      if (ch->pcdata->ci_zips < 5) {
        if (ch->pcdata->ci_discipline2 == GOAL_FOOTHOLD || ch->pcdata->ci_discipline2 == GOAL_UPROOT) {
          send_to_char("That operation needs to be at least speed 5.\n\r", ch);
          return;
        }
      }
      if (ch->pcdata->ci_covers < 4) {
        if (ch->pcdata->ci_discipline2 == GOAL_FOOTHOLD || ch->pcdata->ci_discipline2 == GOAL_UPROOT) {
          send_to_char("That operation needs to have a maximum pc count of at least 4.\n\r", ch);
          return;
        }
      }

      OPERATION_TYPE *op = new_operation();
      free_string(op->author);
      op->author = str_dup(ch->name);
      LOCATION_TYPE *loc = get_loc(ch->pcdata->ci_short);
      op->territoryvnum = number_from_territory(loc);
      op->adversary_type = ch->pcdata->ci_mod;
      free_string(op->adversary_name);
      op->adversary_name = str_dup(ch->pcdata->ci_long);
      free_string(op->room_name);
      op->room_name = str_dup(ch->pcdata->ci_name);
      free_string(op->description);
      op->description = str_dup(ch->pcdata->ci_desc);
      free_string(op->bag_name);
      op->bag_name = str_dup(ch->pcdata->ci_myself);
      free_string(op->upload_name);
      op->upload_name = str_dup(ch->pcdata->ci_target);
      free_string(op->preferred);
      op->preferred = str_dup(ch->pcdata->ci_bystanders);
      free_string(op->storyline);
      op->storyline = str_dup(ch->pcdata->ci_myselfdelayed);
      free_string(op->timeline);
      op->timeline = str_dup(ch->pcdata->ci_targetdelayed);
      free_string(op->storyrunners);
      op->storyrunners = str_dup(ch->pcdata->ci_taste);
      free_string(op->target);
      op->target = str_dup(ch->pcdata->ci_message);
      op->type = ch->pcdata->ci_type;
      op->goal = ch->pcdata->ci_discipline2;
      op->max_pcs = ch->pcdata->ci_covers;
      op->speed = ch->pcdata->ci_zips;
      op->terrain = ch->pcdata->ci_cost;
      op->size = ch->pcdata->ci_layer;
      op->hour = ch->pcdata->ci_alcohol;
      op->day = ch->pcdata->ci_vnum;
      op->initdays = ch->pcdata->ci_vnum;
      op->spam = spam;
      op->faction = ch->faction;
      op->competition = ch->pcdata->ci_discipline;
      op->challenge = ch->pcdata->ci_disclevel;
      if (is_leader(ch, ch->faction))
      op->bonus += 10;
      op->bonus += get_skill(ch, SKILL_GENERALFOCUS) * 4;

      if (op->competition != COMPETE_CLOSED) {
        ch->pcdata->week_tracker[TRACK_OPERATIONS_CREATED]++;
        ch->pcdata->life_tracker[TRACK_OPERATIONS_CREATED]++;
      }

      if(op->speed > 2 && strlen(op->description) > 100 && strcasestr(op->description, "\"") == NULL && strcasestr(op->upload_name, "\"") == NULL && strlen(op->upload_name) > 2 && op->competition != COMPETE_CLOSED)
      {
        char sout[MSL];
        sprintf(sout, "%s,\"%s\"", from_color(op->upload_name), from_color(op->description));
        writeLineToFile(POPS_FILE, str_dup(sout));
      }
      else if(op->speed > 2 && strlen(op->description) > 100 && strcasestr(op->description, "\"") == NULL && strcasestr(op->bag_name, "\"") == NULL && strlen(op->bag_name) > 2 && op->competition != COMPETE_CLOSED)
      {
        char sout[MSL];
        sprintf(sout, "%s,\"%s\"", from_color(op->bag_name), from_color(op->description));
        writeLineToFile(IOPS_FILE, str_dup(sout));
      }

      OpVect.push_back(op);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 16) {
      if (safe_strlen(ch->pcdata->ci_short) < 2 || safe_strlen(ch->pcdata->ci_includes[0]) < 2) {
        send_to_char("Your destiny needs a premise and a scene first.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_message) < 2 || safe_strlen(ch->pcdata->ci_bystanders) < 2) {
        send_to_char("You need to give your destiny roles names.\n\r", ch);
        return;
      }

      bool foundconc = FALSE;

      int max = 0;
      for (vector<NEWDESTINY_TYPE *>::iterator it = DestinyVect.begin();
      it != DestinyVect.end(); ++it) {
        if ((*it)->vnum > max)
        max = (*it)->vnum;
      }
      NEWDESTINY_TYPE *destiny = new_destiny();
      free_string(destiny->author);
      destiny->author = str_dup(ch->name);
      destiny->created_at = current_time;
      destiny->vnum = max + 1;
      free_string(destiny->role_one_name);
      destiny->role_one_name = str_dup(ch->pcdata->ci_message);
      free_string(destiny->role_two_name);
      destiny->role_two_name = str_dup(ch->pcdata->ci_bystanders);
      free_string(destiny->premise);
      destiny->premise = str_dup(ch->pcdata->ci_short);
      destiny->conclude_type = ch->pcdata->ci_disclevel;
      destiny->arch_one = ch->pcdata->ci_zips;
      destiny->arch_two = ch->pcdata->ci_alcohol;
      destiny->restrict_one = ch->pcdata->ci_discipline;
      destiny->restrict_two = ch->pcdata->ci_vnum;
      char arg[MSL], argtwo[MSL], argthree[MSL], argfour[MSL];
      char *argy = str_dup("");
      for (int i = 0; i < 20; i++) {
        free_string(destiny->scene_descs[i]);
        destiny->scene_descs[i] = str_dup(ch->pcdata->ci_includes[i]);
        free_string(argy);
        argy = str_dup(ch->pcdata->ci_excludes[i]);
        char arg[MSL];
        argy = one_argument_nouncap(argy, arg);
        char argtwo[MSL];
        argy = one_argument_nouncap(argy, argtwo);
        char argthree[MSL];
        argy = one_argument_nouncap(argy, argthree);
        char argfour[MSL];
        argy = one_argument_nouncap(argy, argfour);
        destiny->scene_location[i] = atoi(arg);
        destiny->scene_special_one[i] = atoi(argtwo);
        destiny->scene_special_two[i] = atoi(argthree);
        if (!str_cmp(argfour, "1")) {
          foundconc = TRUE;
          destiny->scene_conclusion[i] = TRUE;
        }
        else
        destiny->scene_conclusion[i] = FALSE;
      }
      if (foundconc == FALSE) {
        send_to_char("At least one scene must be a conclusion.\n\r", ch);
        return;
      }
      DestinyVect.push_back(destiny);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

      return;

    }
    else if (ch->pcdata->ci_editing == 18) {
      if(strlen(ch->pcdata->ci_short) < 2)
      {
        send_to_char("You have to specify the Eidolon that the encounter will be caused by.\n\r", ch);
        return;
      }
      if(strlen(ch->pcdata->ci_desc) < 100)
      {
        send_to_char("You have to specify the encounter text.\n\r", ch);
        return;
      }
      FACTION_TYPE *cult = clan_lookup(ch->fcult);
      FACTION_TYPE *sect = clan_lookup(ch->fsect);
      int fpoint = 0;
      if(cult != NULL && !str_cmp(cult->eidilon, ch->pcdata->ci_short))
      fpoint = cult->vnum;
      else if(sect != NULL && !str_cmp(sect->eidilon, ch->pcdata->ci_short))
      fpoint = sect->vnum;
      else
      {
        send_to_char("You aren't in a cult or sect with that Eidolon.\n\r", ch);
        return;
      }

      add_encounter(2, ch->pcdata->ci_discipline, ch->pcdata->ci_desc, fpoint);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (ch->pcdata->ci_editing == 19) {
      if(strlen(ch->pcdata->ci_short) < 2)
      {
        send_to_char("You have to specify the Eidolon that the patrol will be caused by.\n\r", ch);
        return;
      }
      if(strlen(ch->pcdata->ci_desc) < 100)
      {
        send_to_char("You have to specify the patrol intro.\n\r", ch);
        return;
      }
      if(strlen(ch->pcdata->ci_bystanders) < 100)
      {
        send_to_char("You have to specify the patrol conclusion.\n\r", ch);
        return;
      }
      if(strlen(ch->pcdata->ci_message) < 100)
      {
        send_to_char("You have to specify the patrol messages.\n\r", ch);
        return;
      }


      FACTION_TYPE *cult = clan_lookup(ch->fcult);
      FACTION_TYPE *sect = clan_lookup(ch->fsect);
      int fpoint = 0;
      if(cult != NULL && !str_cmp(cult->eidilon, ch->pcdata->ci_short))
      fpoint = cult->vnum;
      else if(sect != NULL && !str_cmp(sect->eidilon, ch->pcdata->ci_short))
      fpoint = sect->vnum;
      else
      {
        send_to_char("You aren't in a cult or sect with that Eidolon.\n\r", ch);
        return;
      }

      int initid = 0;
      for (vector<EXTRA_PATROL_TYPE *>::iterator it = EPatrolVect.begin();
      it != EPatrolVect.end(); ++it) {
        if ((*it)->id > initid)
        initid = (*it)->id;
      }
      EXTRA_PATROL_TYPE *pat = new_epatrol();
      pat->id = initid + 1;
      pat->clan_id = fpoint;
      free_string(pat->intro);
      pat->intro = str_dup(ch->pcdata->ci_desc);
      free_string(pat->conclusion);
      pat->conclusion = str_dup(ch->pcdata->ci_bystanders);
      free_string(pat->messages);
      pat->messages = str_dup(ch->pcdata->ci_message);
      pat->valid = TRUE;
      EPatrolVect.push_back(pat);
      save_epatrol();
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }

    else if (ch->pcdata->ci_editing == 21) {
      if(ch->pcdata->ci_discipline <= 0)
      {
        send_to_char("You have to specify the type.\n\r", ch);
        return;
      }
      if(ch->pcdata->ci_disclevel <= 0)
      {
        send_to_char("You have to specify the territory.\n\r", ch);
        return;
      }
      if(strlen(ch->pcdata->ci_desc) < 2)
      {
        send_to_char("You have to provide a description for the decree.\n\r", ch);
        return;
      }
      if(strlen(ch->pcdata->ci_short) < 2 && ch->pcdata->ci_discipline != DECREE_LOCKDOWN && ch->pcdata->ci_discipline != DECREE_BORDER)
      {
        send_to_char("You have to provide a target.\n\r", ch);
        return;
      }

      DECREE_TYPE *dec;
      for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
      it != DecreeVect.end(); ++it) {
        if((*it)->territory_vnum == ch->pcdata->ci_disclevel && (*it)->start_time > current_time - (3600*24*7))
        {
          send_to_char("There is already a been a recently passed decree in that territory.\n\r", ch);
          return;
        }
      }
      dec = new_decree();
      dec->btype = ch->pcdata->ci_discipline;
      dec->territory_vnum = ch->pcdata->ci_disclevel;
      dec->created_at = current_time;
      free_string(dec->desc);
      dec->desc = str_dup(ch->pcdata->ci_desc);
      free_string(dec->target);
      dec->target = str_dup(ch->pcdata->ci_short);
      free_string(dec->vote_1);
      dec->vote_1 = str_dup(ch->name);
      dec->valid = TRUE;
      DecreeVect.push_back(dec);
      save_decrees();
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }

    else if (ch->pcdata->ci_editing == 20) {
      ANNIVERSARY_TYPE *ann;
      for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
      it != AnniversaryVect.end(); ++it) {
        if ((*it)->id == ch->pcdata->ci_discipline) {
          ann = *it;
          break;
        }
      }
      if (ann == NULL) {
        send_to_char("No such anniversary.\n\r", ch);
        return;
      }
      if(strlen(ann->news) < 2)
      {
        send_to_char("You have to specify the news.\n\r", ch);
        return;
      }
      if(strlen(ann->summary) < 2)
      {
        send_to_char("You have to specify the summary.\n\r", ch);
        return;
      }
      NEWS_TYPE *news;
      news = new_news();
      free_string(news->author);
      if(ann->btype == 1)
      {
        sprintf(buf, "The ascension of %s", ann->eidilon_name);
      }
      else
      {
        sprintf(buf, "The defeat of %s", ann->eidilon_name);
      }
      news->author = str_dup(buf);
      news->timer = 2000;
      free_string(news->message);
      news->message = str_dup(ann->news);
      news->stats[0] = -2;
      NewsVect.push_back(news);

      save_anniversaries();
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }

    else if (ch->pcdata->ci_editing == 17) {
      if(ch->pcdata->ci_disclevel < 1 || ch->pcdata->ci_disclevel > 10 || ch->pcdata->ci_zips < 1 || ch->pcdata->ci_zips > 9)
      {
        send_to_char("Boon and curse not set.\n\r", ch);
        return;
      }
      CHAR_DATA *victim;
      if(!higher_power(ch))
      {
        send_to_char("Only Eidolons can do that.\n\r", ch);
        return;
      }
      struct stat sb;
      char buf[MIL];
      Buffer outbuf;
      DESCRIPTOR_DATA d;
      bool online = FALSE;
      d.original = NULL;
      if (safe_strlen(ch->pcdata->ci_short) < 2)
      return;
      if ((victim = get_char_world_pc(ch->pcdata->ci_short)) != NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, ch->pcdata->ci_short)) {
          return;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize(ch->pcdata->ci_short));
        stat(buf, &sb);
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        return;
      }

      if(victim->fcult != ch->pcdata->eidilon_of && victim->fsect != ch->pcdata->eidilon_of)
      {
        send_to_char("They're not a member of your cult or sect.\n\r", ch);
        if(!online)
        free_char(victim);
        return;
      }
      if (IS_FLAG(victim->act, PLR_BOUND))
      {
        send_to_char("They cannot accept your blessing while bound.\n\r", ch);
        if(!online)
        free_char(victim);
        return;


      }

      int cost = blessing_cost(ch, ch->pcdata->ci_disclevel, ch->pcdata->ci_zips);
      int faccost = cost/10;
      FACTION_TYPE *cult = clan_lookup(ch->pcdata->eidilon_of);
      if(victim->fcult == ch->pcdata->eidilon_of && cult->resource < 8000 + faccost)
      {
        send_to_char("Your cult doesn't have enough resources to gift that.\n\r", ch);
        if(!online)
        free_char(victim);
        return;
      }

      if(ch->pcdata->ci_zips == CURSE_PHOBIA || ch->pcdata->ci_zips == CURSE_IMPRINT)
      {
        free_string(victim->pcdata->curse_text);
        victim->pcdata->curse_text = str_dup(ch->pcdata->ci_message);
      }
      if(ch->pcdata->ci_disclevel == BOON_GIFT && victim->fcult == ch->pcdata->eidilon_of)
      {
        OBJ_DATA *gift = ch->pcdata->boon_gift;
        if(giftobjval(gift) > 5000)
        {
          send_to_char("That item is too powerful to gift.\n\r", ch);
          if(!online)
          free_char(victim);
          return;
        }
        if (!IS_SET(gift->extra_flags, ITEM_NORESALE))
        SET_BIT(gift->extra_flags, ITEM_NORESALE);
        obj_from_char(gift);
        obj_to_char(gift, victim);
        ch->pcdata->boon_gift = NULL;
      }

      if(victim->fcult == ch->pcdata->eidilon_of)
      {
        use_resources(faccost, cult->vnum, ch, "providing a blessing");
        victim->pcdata->boon = ch->pcdata->ci_disclevel;
        victim->pcdata->boon_timeout = current_time + (3600*24);
        if(victim->pcdata->boon == BOON_LAVISH || victim->pcdata->boon == BOON_RENOVATE)
        victim->pcdata->boon_timeout = current_time + (3600*24*7);
        if(victim->pcdata->boon == BOON_VITALITY)
        {
          if(get_age(victim) > 18)
          victim->pcdata->deaged += 180;
        }
      }
      victim->pcdata->curse = ch->pcdata->ci_zips;
      victim->pcdata->curse_timeout = current_time + (3600*24);

      save_char_obj(victim, FALSE, FALSE);
      for(int i=0;i<9;i++)
      {
        ch->pcdata->boon_history[i+1] = ch->pcdata->boon_history[i];
        ch->pcdata->curse_history[i+1] = ch->pcdata->curse_history[i];
      }
      ch->pcdata->boon_history[0] = ch->pcdata->ci_disclevel;
      ch->pcdata->curse_history[0] = ch->pcdata->ci_zips;
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);
      if(!online)
      free_char(victim);


    }
    else if (ch->pcdata->ci_editing == 15) {
      if (safe_strlen(ch->pcdata->ci_message) < 2 || safe_strlen(ch->pcdata->ci_desc) < 2) {
        send_to_char("You haven't completed the form yet.\n\r", ch);
        return;
      }
      if (!character_exists(ch->pcdata->ci_message)) {
        send_to_char("No such individual.\n\r", ch);
        return;
      }
      int ccost = commit_cost(ch, ch->pcdata->ci_message);
      if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && has_trust(ch, TRUST_RESOURCES, ch->faction)) {
        if (clan_lookup(ch->faction)->resource < 5000 + ccost / 10) {
          printf_to_char(ch, "That would cost $%d in society resources.\n\r", ccost);
          return;
        }
      }
      else {
        if (ch->pcdata->total_money < ccost * 100) {
          printf_to_char(ch, "That would cost $%d.\n\r", ccost);
          return;
        }
      }

      INSTITUTE_TYPE *ins;
      for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
      it != InVect.end(); ++it) {
        ins = (*it);
        if (!str_cmp(ins->name, ch->pcdata->ci_message)) {
          if (ins->asylum_prestige > 0 && ins->asylum_inactive == 0) {
            send_to_char("They're already a patient.\n\r", ch);
            return;
          }
          if (ins->asylum_status == ASYLUM_WANTED) {
            send_to_char("They're already wanted.\n\r", ch);
            return;
          }
          if (ins->asylum_status == ASYLUM_NEED_APPROVAL) {
            send_to_char("They're already wanted.\n\r", ch);
            return;
          }

          ins->asylum_status = ASYLUM_NEED_APPROVAL;
          free_string(ins->asylum_owner);
          if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && has_trust(ch, TRUST_RESOURCES, ch->faction))
          ins->asylum_owner = str_dup(clan_lookup(ch->faction)->name);
          else
          ins->asylum_owner = str_dup(ch->name);
          free_string(ins->arrest_notes);
          ins->arrest_notes = str_dup(ch->pcdata->ci_desc);
          ins->arrest_cost = commit_cost(ch, ch->pcdata->ci_message);
          send_to_char("Form submitted.\n\r", ch);
          return;
        }
      }
      ins = new_institute();
      free_string(ins->name);
      ins->name = str_dup(ch->pcdata->ci_message);
      ins->asylum_status = ASYLUM_NEED_APPROVAL;
      free_string(ins->asylum_owner);
      if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && has_trust(ch, TRUST_RESOURCES, ch->faction))
      ins->asylum_owner = str_dup(clan_lookup(ch->faction)->name);
      else
      ins->asylum_owner = str_dup(ch->name);
      free_string(ins->arrest_notes);
      ins->arrest_notes = str_dup(ch->pcdata->ci_desc);
      ins->arrest_cost = commit_cost(ch, ch->pcdata->ci_message);
      send_to_char("Form submitted.\n\r", ch);
      InVect.push_back(ins);
      ch->pcdata->ci_editing = 0;
      return;
    }
    else if (ch->pcdata->ci_editing == 23) {

      if(ch->pcdata->ci_discipline < 0 || ch->pcdata->ci_discipline > PLOT_CRISIS)
      {
        send_to_char("You have to specify the type.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_desc) < 2) {
        send_to_char("Your story needs a description, type storyidea description (number) to edit it\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_message) < 2 && ch->pcdata->ci_discipline == PLOT_OTHER) {
        send_to_char("Your story needs it's npcs detailed, type storyidea npcs (number) to edit it\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_bystanders) < 2 && ch->pcdata->ci_discipline == PLOT_OTHER) {
        send_to_char("Your story needs it's lore detailed, type storyidea lore (number) to edit it\n\r", ch);
        return;
      }
      STORYIDEA_TYPE *story = new_storyidea();
      story->type = ch->pcdata->ci_discipline;
      free_string(story->author);
      story->author = str_dup(ch->name);
      free_string(story->description);
      story->description = str_dup(ch->pcdata->ci_desc);
      free_string(story->npcs);
      story->npcs = str_dup(ch->pcdata->ci_message);
      free_string(story->lore);
      story->lore = str_dup(ch->pcdata->ci_bystanders);
      free_string(story->territory);
      story->territory = str_dup(ch->pcdata->ci_name);
      free_string(story->secrets);
      story->secrets = str_dup(ch->pcdata->ci_long);
      free_string(story->storyline);
      story->storyline = str_dup(ch->pcdata->ci_short);
      free_string(story->eidolon);
      story->eidolon = str_dup(ch->pcdata->ci_myselfdelayed);
      if(safe_strlen(story->eidolon) > 2)
      {
        bool found = FALSE;
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {

          if(!str_cmp((*it)->eidilon, story->eidolon))
          {
            if((*it)->type == FACTION_CULT)
            {
              found = TRUE;
              free_string(story->mastermind);
              story->mastermind = str_dup((*it)->leader);
            }
          }
        }
        if(found == FALSE)
        {
          for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
          it != FacVect.end(); ++it) {

            if(!str_cmp((*it)->eidilon, story->eidolon))
            {
              found = TRUE;
              free_string(story->mastermind);
              story->mastermind = str_dup((*it)->leader);
            }
          }
        }
      }
      if(valid_subfac(ch->pcdata->ci_myself))
      story->subfactionone = get_subfac(ch->pcdata->ci_myself);
      if(valid_subfac(ch->pcdata->ci_target))
      story->subfactiontwo = get_subfac(ch->pcdata->ci_target);
      story->status = STATUS_PENDING;
      story->time_locked = current_time + (5 * 24 * 60 * 60);
      ch->pcdata->account->storyidea_cooldown = (current_time + (3600 * 24 * 21));
      StoryIdeaVect.push_back(story);
      ch->pcdata->ci_editing = 0;
      send_to_char("Done.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 22) {
      if(higher_power(ch))
      {
        PROFILE_TYPE *profile = profile_lookup(ch->name);
        if(profile != NULL)
        {
          char * tmpphoto = str_dup(ch->pcdata->photo_msgs);
          free_string(ch->pcdata->photo_msgs);
          ch->pcdata->photo_msgs = str_dup("");
          to_photo_message(ch, profile->photo);
          free_string(profile->photo);
          profile->photo = str_dup(ch->pcdata->photo_msgs);
          free_string(ch->pcdata->photo_msgs);
          ch->pcdata->photo_msgs = str_dup(tmpphoto);
        }
      }
      send_to_char("Done.\n\r", ch);
      save_profiles();
      ch->pcdata->ci_editing = 0;
      return;
    }
    else if (ch->pcdata->ci_editing == 11) {
      if (clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->resource < 10000) {
        send_to_char("You can't do that.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_name) < 3) {
        send_to_char("Pick a territory first.\n\r", ch);
        return;
      }
      if (safe_strlen(ch->pcdata->ci_desc) < 5) {
        send_to_char("You need to describe the news  message for your mission.\n\r", ch);
        return;
      }
      LOCATION_TYPE *loc = get_loc(ch->pcdata->ci_name);
      if (loc == NULL) {
        send_to_char("Couldn't find the territory, please report this bug.\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_mod == MISSION_CONTROL) {
        if (clan_lookup(ch->faction)->parent == 0) {
          send_to_char("You need to have a parent faction to make use of this.\n\r", ch);
          return;
        }
        char newsbuf[MSL];
        sprintf(newsbuf, "%s(Control mission by %s)", ch->pcdata->ci_desc, clan_lookup(ch->faction)->name);
        super_news(newsbuf);
        ch->pcdata->ci_editing = 0;
        send_to_char("Done.\n\r", ch);
      }
      else if (ch->pcdata->ci_mod == MISSION_INSTIGATE) {
        if (loc->status == STATUS_WAR) {
          send_to_char("You probably can't escalate the violence in this territory any further.\n\r", ch);
          return;
        }
        if (loc->status == STATUS_WILD) {
          send_to_char("You can't influence that territory.\n\r", ch);
          return;
        }
        if (loc->status == STATUS_CONTEST) {
          if (number_percent() % 4 == 0) {
            loc->status = STATUS_WAR;
            clan_lookup(ch->faction)->resource -= 1500;
            char newsbuf[MSL];
            sprintf(newsbuf, "%s(Instigate mission by %s)", ch->pcdata->ci_desc, clan_lookup(ch->faction)->name);
            super_news(newsbuf);

            ch->pcdata->ci_editing = 0;
            send_to_char("You successfully instigate a war.\n\r", ch);
            return;
          }
          else {
            clan_lookup(ch->faction)->resource -= 1500;
            ch->pcdata->ci_editing = 0;
            send_to_char("You fail in your attempts to start a war.\n\r", ch);
            return;
          }
        }
        if (loc->status == STATUS_GOVERN || loc->status == STATUS_SHARED) {
          if (number_percent() % 2 == 0) {
            loc->status = STATUS_CONTEST;
            clan_lookup(ch->faction)->resource -= 1500;
            char newsbuf[MSL];
            sprintf(newsbuf, "%s(Instigate mission by %s)", ch->pcdata->ci_desc, clan_lookup(ch->faction)->name);
            super_news(newsbuf);
            ch->pcdata->ci_editing = 0;
            send_to_char("You successfully push the territory into a state of contestation.\n\r", ch);
            return;
          }
          else {
            clan_lookup(ch->faction)->resource -= 1500;
            ch->pcdata->ci_editing = 0;
            send_to_char("You fail in your attempts to push the territory into a state of contestation.\n\r", ch);
            return;
          }
        }
      }
      else if (ch->pcdata->ci_mod == MISSION_DIPLOMACY) {
        if (loc->status == STATUS_GOVERN || loc->status == STATUS_SHARED) {
          send_to_char("This territory is already quite peaceful.\n\r", ch);
          return;
        }
        if (loc->status == STATUS_WILD) {
          send_to_char("You can't influence that territory.\n\r", ch);
          return;
        }
        if (loc->status == STATUS_CONTEST) {
          if (number_percent() % 2 == 0) {
            loc->status = STATUS_GOVERN;
            if (second_terr_points(loc) + 10 >= first_terr_points(loc) && number_percent() % 3 == 0)
            loc->status = STATUS_SHARED;
            clan_lookup(ch->faction)->resource -= 1500;
            char newsbuf[MSL];
            sprintf(newsbuf, "%s(Diplomacy mission by %s)", ch->pcdata->ci_desc, clan_lookup(ch->faction)->name);
            super_news(newsbuf);
            ch->pcdata->ci_editing = 0;
            send_to_char("You have successfully calmed the region.\n\r", ch);
            return;
          }
          else {
            clan_lookup(ch->faction)->resource -= 1500;
            ch->pcdata->ci_editing = 0;
            send_to_char("You fail in your attempts to calm the region.\n\r", ch);
            return;
          }
        }
        if (loc->status == STATUS_WAR) {
          if (number_percent() % 3 == 0) {
            loc->status = STATUS_GOVERN;
            if (second_terr_points(loc) + 10 >= first_terr_points(loc) && number_percent() % 3 == 0)
            loc->status = STATUS_SHARED;
            clan_lookup(ch->faction)->resource -= 1500;
            char newsbuf[MSL];
            sprintf(newsbuf, "%s(Diplomacy mission by %s)", ch->pcdata->ci_desc, clan_lookup(ch->faction)->name);
            super_news(newsbuf);
            ch->pcdata->ci_editing = 0;
            send_to_char("You have successfully calmed the region.\n\r", ch);
            return;
          }
          else {
            clan_lookup(ch->faction)->resource -= 1500;
            ch->pcdata->ci_editing = 0;
            send_to_char("You fail in your attempts to calm the region.\n\r", ch);
            return;
          }
        }
      }
      else if (ch->pcdata->ci_mod == MISSION_LIAISE) {
        if (loc->status != STATUS_GOVERN && loc->status != STATUS_SHARED) {
          send_to_char("This territory is still being contested.\n\r", ch);
          return;
        }
        if (clan_lookup(ch->faction)->parent == 0) {
          send_to_char("You're not close enough with the government of that territory for that.\n\r", ch);
          return;
        }
        if (clan_lookup(ch->faction)->parent == FACTION_HAND) {
          if (str_cmp("The Hand", first_dom_terr(loc))) {
            if (str_cmp("The Hand", second_dom_terr(loc)) || loc->status != STATUS_SHARED) {
              send_to_char("You're not close enough with the government of that territory for that.\n\r", ch);
              return;
            }
          }
        }
        if (clan_lookup(ch->faction)->parent == FACTION_ORDER) {
          if (str_cmp("The Order", first_dom_terr(loc))) {
            if (str_cmp("The Order", second_dom_terr(loc)) || loc->status != STATUS_SHARED) {
              send_to_char("You're not close enough with the government of that territory for that.\n\r", ch);
              return;
            }
          }
        }
        if (clan_lookup(ch->faction)->parent == FACTION_TEMPLE) {
          if (str_cmp("The Temple", first_dom_terr(loc))) {
            if (str_cmp("The Temple", second_dom_terr(loc)) || loc->status != STATUS_SHARED) {
              send_to_char("You're not close enough with the government of that territory for that.\n\r", ch);
              return;
            }
          }
        }
        clan_lookup(ch->faction)->resource -= 1500;
        char newsbuf[MSL];
        sprintf(newsbuf, "%s(Liaise mission by %s)", ch->pcdata->ci_desc, clan_lookup(ch->faction)->name);
        super_news(newsbuf);
        ch->pcdata->ci_editing = 0;
        send_to_char("You successfully liaise with the government of the region.\n\r", ch);
        string_append(ch, &loc->notes);
      }
    }
  }

  _DOFUN(do_makeobject) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];
    char buf[MSL];
    // char temp[MSL];
    OBJ_DATA *obj;
    int vnum;
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);

    return;
    if (in_prop(ch) == NULL || !can_decorate(ch, ch->in_room)) {
      send_to_char("You have to be in a shop you own to do that.\n\r", ch);
      return;
    }

    ch->pcdata->ci_editing = 1;
    send_to_char("Done.\n\r", ch);
    return;

    int cost = atoi(arg2);
    if (cost < 1 || (cost > ch->money && cost > ch->pcdata->total_money)) {
      send_to_char("You can't afford that.\n\r", ch);
      send_to_char("Syntax: makeobject clothing/jewelry/food/drink/thing/bag/weapon/gun (price) large/small (name)\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "Clothing"))
    vnum = 10;
    else if (!str_cmp(arg1, "Jewelry"))
    vnum = 11;
    else if (!str_cmp(arg1, "food"))
    vnum = 12;
    else if (!str_cmp(arg1, "drink"))
    vnum = 13;
    else if (!str_cmp(arg1, "thing"))
    vnum = 14;
    else if (!str_cmp(arg1, "bag"))
    vnum = 15;
    else if (!str_cmp(arg1, "weapon"))
    vnum = 16;
    else if (!str_cmp(arg1, "gun"))
    vnum = 17;
    else if (!str_cmp(arg1, "phone"))
    vnum = 18;
    else if (!str_cmp(arg1, "armor"))
    vnum = 19;
    else {
      send_to_char("Syntax: makeobject clothing/jewelry/food/drink/thing/bag/weapon/gun (price) large/small (name)\n\r", ch);
      return;
    }

    if (safe_strlen(argument) < 3) {
      send_to_char("Syntax: makeobject clothing/jewelry/food/drink/thing/bag/weapon/gun (price) large/small (name)\n\r", ch);
      return;
    }
    if (!str_cmp(arg3, "large")) {
      obj = create_object(get_obj_index(vnum), 0);
      obj->size = 30;
    }
    else
    obj = create_object(get_obj_index(vnum), 0);

    obj->cost = cost;

    argument = one_argument_nouncap(argument, arg4);

    if (str_cmp(arg4, "a") && str_cmp(arg4, "an")) {
      sprintf(buf, "%s %s", arg4, argument);
      argument = str_dup(buf);
    }

    free_string(obj->name);
    obj->name = str_dup(argument);
    free_string(obj->short_descr);
    obj->short_descr = str_dup(argument);
    if (is_pair(obj)) {
      sprintf(buf, "A pair of %s", argument);
      free_string(obj->description);
      obj->description = str_dup(buf);
    }
    else {
      sprintf(buf, "%s %s", a_or_an(argument), argument);
      free_string(obj->description);
      obj->description = str_dup(buf);
    }

    sprintf(buf, "%s", "worn");
    // sprintf(buf, "%s", "worn");
    // sprintf(buf, "%-21.21s", buf);
    free_string(obj->wear_string);
    obj->wear_string = str_dup(buf);

    if (ch->pcdata->total_money > cost)
    ch->pcdata->total_money -= cost;
    else
    ch->money -= cost;
    obj_to_char(obj, ch);
    send_to_char("Object made!", ch);
  }

  void cover_mod_jewelry(CHAR_DATA *ch, OBJ_DATA *obj, char *argument) {
    if (!str_cmp(argument, "hands")) {
      if (does_cover(obj, COVERS_HANDS)) {
        obj->value[3] -= COVERS_HANDS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_HANDS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerarms")) {
      if (does_cover(obj, COVERS_LOWER_ARMS)) {
        obj->value[3] -= COVERS_LOWER_ARMS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_LOWER_ARMS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperarms")) {
      if (does_cover(obj, COVERS_UPPER_ARMS)) {
        obj->value[3] -= COVERS_UPPER_ARMS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_UPPER_ARMS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "feet")) {
      if (does_cover(obj, COVERS_FEET)) {
        obj->value[3] -= COVERS_FEET;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_FEET;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerlegs")) {
      if (does_cover(obj, COVERS_LOWER_LEGS)) {
        obj->value[3] -= COVERS_LOWER_LEGS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_LOWER_LEGS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "forehead")) {
      if (does_cover(obj, COVERS_FOREHEAD)) {
        obj->value[3] -= COVERS_FOREHEAD;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_FOREHEAD;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "thighs")) {
      if (does_cover(obj, COVERS_THIGHS)) {
        obj->value[3] -= COVERS_THIGHS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_THIGHS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "groin")) {
      if (does_cover(obj, COVERS_GROIN)) {
        obj->value[3] -= COVERS_GROIN;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_GROIN;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "buttocks")) {
      if (does_cover(obj, COVERS_ARSE)) {
        obj->value[3] -= COVERS_ARSE;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_ARSE;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerback")) {
      if (does_cover(obj, COVERS_LOWER_BACK)) {
        obj->value[3] -= COVERS_LOWER_BACK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_LOWER_BACK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperback")) {
      if (does_cover(obj, COVERS_UPPER_BACK)) {
        obj->value[3] -= COVERS_UPPER_BACK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_UPPER_BACK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerchest")) {
      if (does_cover(obj, COVERS_LOWER_CHEST)) {
        obj->value[3] -= COVERS_LOWER_CHEST;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_LOWER_CHEST;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "breasts")) {
      if (does_cover(obj, COVERS_BREASTS)) {
        obj->value[3] -= COVERS_BREASTS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_BREASTS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperchest")) {
      if (does_cover(obj, COVERS_UPPER_CHEST)) {
        obj->value[3] -= COVERS_UPPER_CHEST;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_UPPER_CHEST;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "neck")) {
      if (does_cover(obj, COVERS_NECK)) {
        obj->value[3] -= COVERS_NECK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_NECK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerface")) {
      if (does_cover(obj, COVERS_LOWER_FACE)) {
        obj->value[3] -= COVERS_LOWER_FACE;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_LOWER_FACE;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "hair")) {
      if (does_cover(obj, COVERS_HAIR)) {
        obj->value[3] -= COVERS_HAIR;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_HAIR;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "eyes")) {
      if (does_cover(obj, COVERS_EYES)) {
        obj->value[3] -= COVERS_EYES;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[3] += COVERS_EYES;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "clear")) {
      obj->value[3] = 0;
      send_to_char("Coverage cleared.\n\r", ch);
    }
    else
    send_to_char("Options are: hands, lowerarms, upperarms, feet, lowerlegs, forehead, thighs, groin, buttocks, lowerback, upperback,  lowerchest, breasts, upperchest, neck, lowerface, hair, eyes and clear\n\r", ch);
    return;
  }

  void zip_mod_clothing(CHAR_DATA *ch, OBJ_DATA *obj, char *argument) {
    if (!str_cmp(argument, "hands")) {
      if (does_zipcover(obj, COVERS_HANDS)) {
        obj->value[1] -= COVERS_HANDS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_HANDS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerarms")) {
      if (does_zipcover(obj, COVERS_LOWER_ARMS)) {
        obj->value[1] -= COVERS_LOWER_ARMS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_LOWER_ARMS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperarms")) {
      if (does_zipcover(obj, COVERS_UPPER_ARMS)) {
        obj->value[1] -= COVERS_UPPER_ARMS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_UPPER_ARMS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "feet")) {
      if (does_zipcover(obj, COVERS_FEET)) {
        obj->value[1] -= COVERS_FEET;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_FEET;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerlegs")) {
      if (does_zipcover(obj, COVERS_LOWER_LEGS)) {
        obj->value[1] -= COVERS_LOWER_LEGS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_LOWER_LEGS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "forehead")) {
      if (does_zipcover(obj, COVERS_FOREHEAD)) {
        obj->value[1] -= COVERS_FOREHEAD;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_FOREHEAD;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "thighs")) {
      if (does_zipcover(obj, COVERS_THIGHS)) {
        obj->value[1] -= COVERS_THIGHS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_THIGHS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "groin")) {
      if (does_zipcover(obj, COVERS_GROIN)) {
        obj->value[1] -= COVERS_GROIN;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_GROIN;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "buttocks")) {
      if (does_zipcover(obj, COVERS_ARSE)) {
        obj->value[1] -= COVERS_ARSE;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_ARSE;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerback")) {
      if (does_zipcover(obj, COVERS_LOWER_BACK)) {
        obj->value[1] -= COVERS_LOWER_BACK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_LOWER_BACK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperback")) {
      if (does_zipcover(obj, COVERS_UPPER_BACK)) {
        obj->value[1] -= COVERS_UPPER_BACK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_UPPER_BACK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerchest")) {
      if (does_zipcover(obj, COVERS_LOWER_CHEST)) {
        obj->value[1] -= COVERS_LOWER_CHEST;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_LOWER_CHEST;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "breasts")) {
      if (does_zipcover(obj, COVERS_BREASTS)) {
        obj->value[1] -= COVERS_BREASTS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_BREASTS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperchest")) {
      if (does_zipcover(obj, COVERS_UPPER_CHEST)) {
        obj->value[1] -= COVERS_UPPER_CHEST;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_UPPER_CHEST;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "neck")) {
      if (does_zipcover(obj, COVERS_NECK)) {
        obj->value[1] -= COVERS_NECK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_NECK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerface")) {
      if (does_zipcover(obj, COVERS_LOWER_FACE)) {
        obj->value[1] -= COVERS_LOWER_FACE;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_LOWER_FACE;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "hair")) {
      if (does_zipcover(obj, COVERS_HAIR)) {
        obj->value[1] -= COVERS_HAIR;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_HAIR;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "eyes")) {
      if (does_zipcover(obj, COVERS_EYES)) {
        obj->value[1] -= COVERS_EYES;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[1] += COVERS_EYES;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "clear")) {
      obj->value[1] = 0;
      send_to_char("Coverage cleared.\n\r", ch);
    }
    else
    send_to_char("Options are: hands, lowerarms, upperarms, feet, lowerlegs, forehead, thighs, groin, buttocks, lowerback, upperback,  lowerchest, breasts, upperchest, neck, lowerface, hair, eyes and clear\n\r", ch);
    return;
  }

  void cover_mod_clothing(CHAR_DATA *ch, OBJ_DATA *obj, char *argument) {
    if (!str_cmp(argument, "hands")) {
      if (does_cover(obj, COVERS_HANDS)) {
        obj->value[0] -= COVERS_HANDS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_HANDS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerarms")) {
      if (does_cover(obj, COVERS_LOWER_ARMS)) {
        obj->value[0] -= COVERS_LOWER_ARMS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_LOWER_ARMS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperarms")) {
      if (does_cover(obj, COVERS_UPPER_ARMS)) {
        obj->value[0] -= COVERS_UPPER_ARMS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_UPPER_ARMS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "feet")) {
      if (does_cover(obj, COVERS_FEET)) {
        obj->value[0] -= COVERS_FEET;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_FEET;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerlegs")) {
      if (does_cover(obj, COVERS_LOWER_LEGS)) {
        obj->value[0] -= COVERS_LOWER_LEGS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_LOWER_LEGS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "forehead")) {
      if (does_cover(obj, COVERS_FOREHEAD)) {
        obj->value[0] -= COVERS_FOREHEAD;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_FOREHEAD;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "thighs")) {
      if (does_cover(obj, COVERS_THIGHS)) {
        obj->value[0] -= COVERS_THIGHS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_THIGHS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "groin")) {
      if (does_cover(obj, COVERS_GROIN)) {
        obj->value[0] -= COVERS_GROIN;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_GROIN;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "buttocks")) {
      if (does_cover(obj, COVERS_ARSE)) {
        obj->value[0] -= COVERS_ARSE;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_ARSE;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerback")) {
      if (does_cover(obj, COVERS_LOWER_BACK)) {
        obj->value[0] -= COVERS_LOWER_BACK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_LOWER_BACK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperback")) {
      if (does_cover(obj, COVERS_UPPER_BACK)) {
        obj->value[0] -= COVERS_UPPER_BACK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_UPPER_BACK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerchest")) {
      if (does_cover(obj, COVERS_LOWER_CHEST)) {
        obj->value[0] -= COVERS_LOWER_CHEST;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_LOWER_CHEST;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "breasts")) {
      if (does_cover(obj, COVERS_BREASTS)) {
        obj->value[0] -= COVERS_BREASTS;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_BREASTS;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "upperchest")) {
      if (does_cover(obj, COVERS_UPPER_CHEST)) {
        obj->value[0] -= COVERS_UPPER_CHEST;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_UPPER_CHEST;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "neck")) {
      if (does_cover(obj, COVERS_NECK)) {
        obj->value[0] -= COVERS_NECK;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_NECK;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "lowerface")) {
      if (does_cover(obj, COVERS_LOWER_FACE)) {
        obj->value[0] -= COVERS_LOWER_FACE;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_LOWER_FACE;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "hair")) {
      if (does_cover(obj, COVERS_HAIR)) {
        obj->value[0] -= COVERS_HAIR;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_HAIR;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "eyes")) {
      if (does_cover(obj, COVERS_EYES)) {
        obj->value[0] -= COVERS_EYES;
        send_to_char("Coverage removed.\n\r", ch);
      }
      else {
        obj->value[0] += COVERS_EYES;
        send_to_char("Coverage added.\n\r", ch);
      }
    }
    else if (!str_cmp(argument, "clear")) {
      obj->value[0] = 0;
      send_to_char("Coverage cleared.\n\r", ch);
    }
    else
    send_to_char("Options are: hands, lowerarms, upperarms, feet, lowerlegs, forehead, thighs, groin, buttocks, lowerback, upperback,  lowerchest, breasts, upperchest, neck, lowerface, hair, eyes and clear\n\r", ch);
    return;
  }

  bool at_tailor(CHAR_DATA *ch) {
    if (is_gm(ch))
    return TRUE;

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;
    if (ch->in_room == NULL) {
      return FALSE;
    }

    if (ch->in_room->vnum == 24020)
    return TRUE;
    if ((keeper = find_keeper(ch)) == NULL)
    return FALSE;

    for (obj = keeper->carrying; obj; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (cost = get_cost(keeper, obj, TRUE)) > 0) {
        if (obj->item_type == ITEM_CLOTHING)
        return TRUE;

        if (obj->item_type == ITEM_CONTAINER)
        return TRUE;
      }
    }

    return FALSE;
  }
  bool at_kitchen(CHAR_DATA *ch) {
    if (is_gm(ch))
    return TRUE;

    if (IS_SET(ch->in_room->room_flags, ROOM_KITCHEN))
    return TRUE;

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;
    if (ch->in_room == NULL) {
      return FALSE;
    }

    if (ch->in_room->vnum == 24020)
    return TRUE;
    if ((keeper = find_keeper(ch)) == NULL)
    return FALSE;

    for (obj = keeper->carrying; obj; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (cost = get_cost(keeper, obj, TRUE)) > 0) {
        if (obj->item_type == ITEM_FOOD)
        return TRUE;

        if (obj->item_type == ITEM_DRINK_CON)
        return TRUE;
      }
    }

    return FALSE;
  }

  bool at_jeweler(CHAR_DATA *ch) {
    if (is_gm(ch))
    return TRUE;

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    if (ch->in_room == NULL) {
      return FALSE;
    }

    if (ch->in_room->vnum == 24029)
    return TRUE;

    if ((keeper = find_keeper(ch)) == NULL)
    return FALSE;

    for (obj = keeper->carrying; obj; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (cost = get_cost(keeper, obj, TRUE)) > 0) {
        if (obj->item_type == ITEM_JEWELRY)
        return TRUE;

        if (obj->item_type == ITEM_PHONE)
        return TRUE;
      }
    }

    return FALSE;
  }
  bool at_workshop(CHAR_DATA *ch) {
    if (is_gm(ch))
    return TRUE;

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    if (ch->in_room == NULL) {
      return FALSE;
    }

    if (ch->in_room->vnum == 19216)
    return TRUE;

    if ((keeper = find_keeper(ch)) == NULL)
    return FALSE;

    for (obj = keeper->carrying; obj; obj = obj->next_content) {
      if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (cost = get_cost(keeper, obj, TRUE)) > 0) {
        if (obj->item_type == ITEM_FURNITURE)
        return TRUE;

        if (obj->item_type == ITEM_WEAPON)
        return TRUE;
        if (obj->item_type == ITEM_RANGED)
        return TRUE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_customize) {
    char arg1[MSL];
    EXTRA_DESCR_DATA *ed;
    char arg2[MSL];
    char arg3[MSL];
    char buf[MSL];
    // char temp[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char colbuf[MSL];

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    smash_vector(argument);
    // argument = smash_MXP(argument);

    if (arg1[0] == '\0') {
      send_to_char("Customize what?\n\r", ch);
      return;
    }
    if (institute_room(ch->in_room) && clinic_patient(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        continue;
      }
      if (trade_good(obj))
      continue;

      if (is_name(arg1, obj->name)) {
        if (str_cmp(arg2, "wear")) {
          if ((obj->item_type == ITEM_CLOTHING || obj->item_type == ITEM_CONTAINER) && !at_tailor(ch) && !goblin_market(ch->in_room)) {
            send_to_char("You need to be at a tailor to do that.\n\r", ch);
            return;
          }
          else if (!at_jeweler(ch) && !goblin_market(ch->in_room) && (obj->item_type == ITEM_JEWELRY || obj->item_type == ITEM_PHONE || obj->item_type == ITEM_GAME)) {
            send_to_char("You need to be at a jeweler to do that.\n\r", ch);
            return;
          }
        }

        if (!at_workshop(ch) && !goblin_market(ch->in_room) && (obj->item_type == ITEM_FURNITURE || obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_RANGED)) {
          send_to_char("You need to be at a workshop to do that.\n\r", ch);
          return;
        }
        else if (!at_kitchen(ch) && !goblin_market(ch->in_room) && (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)) {
          send_to_char("You need to be at a kitchen or restaurant to do that.\n\r", ch);
          return;
        }
        else if (obj->item_type == ITEM_CORPSE_NPC && obj->timer > 0) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }

        if (obj->item_type == ITEM_KEY) {
          continue;
        }

        if (trade_good(obj)) {
          send_to_char("You can't alter that.\n\r", ch);
          return;
        }

        if (!IS_SET(obj->extra_flags, ITEM_ALTERED)) {
          SET_BIT(obj->extra_flags, ITEM_ALTERED);
        }

        if (!str_cmp(arg2, "")) { // enters customization editor
          ch->pcdata->customizing = obj;
          obj_to_custom(ch, obj);
          ch->pcdata->ci_vnum = 1;
          ch->pcdata->ci_editing = 1;
          return;
        }

        if (!str_cmp(arg2, "cover")) {
          if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_JEWELRY) {
            send_to_char("That can only be done on clothing or jewelry.\n\r", ch);
            return;
          }

          if (obj->item_type == ITEM_CLOTHING) {
            cover_mod_clothing(ch, obj, argument);
          }
          else if (obj->item_type == ITEM_JEWELRY) {
            cover_mod_jewelry(ch, obj, argument);
          }
          return;
        }
        else if (!str_cmp(arg2, "zips")) {
          if (obj->item_type != ITEM_CLOTHING) {
            send_to_char("That can only be done on clothing.\n\r", ch);
            return;
          }
          else if (obj->item_type == ITEM_CLOTHING) {
            zip_mod_clothing(ch, obj, argument);
          }
          return;
        }
        else if (!str_cmp(arg2, "badge")) {
          if (ch->race == RACE_DEPUTY) {
            if (obj->faction == 0) {
              obj->faction = -1;
              send_to_char("You add the emblem of the sheriff's department.\n\r", ch);
              return;
            }
            else {
              obj->faction = -0;
              send_to_char("You remove the emblem of the sheriff's department.\n\r", ch);
              return;
            }
          }
          else
          {
            send_to_char("You need to be a deputy to do that.\n\r", ch);
            return;
          }
        }
        else if (!str_cmp(arg2, "key")) {
          if(get_skill(ch, SKILL_DEMONOLOGY) > 0) {
            if (obj->faction == 0) {
              obj->faction = -2;
              send_to_char("You turn it into a key.\n\r", ch);
              return;
            }
            else {
              obj->faction = -0;
              send_to_char("You stop it being used as a key.\n\r", ch);
              return;
            }
          }
          else
          {
            send_to_char("You need to have occult knowledge to od that.\n\r", ch);
            return;
          }
        }
        else if (!str_cmp(arg2, "symbol")) {
          send_to_char("Customize (object) cultsymbol, sectsymbol, factionsymbol, badge, key.\n\r", ch);
          return;

        }
        else if (!str_cmp(arg2, "cultsymbol")) {
          if(ch->fcult == 0)
          {
            send_to_char("You're not in a cult.\n\r", ch);
            return;
          }
          if(obj->faction == 0)
          {
            obj->faction = ch->fcult;
            send_to_char("You add the symbol of your cult.\n\r", ch);
            return;
          }
          else
          {
            obj->faction = 0;
            send_to_char("You remove the symbol.\n\r", ch);
            return;
          }
        }
        else if (!str_cmp(arg2, "sectsymbol")) {
          if(ch->fsect == 0)
          {
            send_to_char("You're not in a sect.\n\r", ch);
            return;
          }
          if(obj->faction == 0)
          {
            obj->faction = ch->fsect;
            send_to_char("You add the symbol of your sect.\n\r", ch);
            return;
          }
          else
          {
            obj->faction = 0;
            send_to_char("You remove the symbol.\n\r", ch);
            return;
          }
        }
        else if (!str_cmp(arg2, "factionsymbol")) {
          if(ch->fcore == 0)
          {
            send_to_char("You're not in a faction.\n\r", ch);
            return;
          }
          if(obj->faction == 0)
          {
            obj->faction = ch->fcore;
            send_to_char("You add the symbol of your faction.\n\r", ch);
            return;
          }
          else
          {
            obj->faction = 0;
            send_to_char("You remove the symbol.\n\r", ch);
            return;
          }

        }
        else if (!str_cmp(arg2, "large")) {
          if (IS_SET(obj->extra_flags, ITEM_LARGE)) {
            send_to_char("That's already large.\n\r", ch);
            return;
          }
          SET_BIT(obj->extra_flags, ITEM_LARGE);
          obj->size = UMAX(obj->size, 35);
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "small")) {
          if (!IS_SET(obj->extra_flags, ITEM_LARGE)) {
            send_to_char("That's already small.\n\r", ch);
            return;
          }
          REMOVE_BIT(obj->extra_flags, ITEM_LARGE);
          obj->size = UMIN(obj->size, 10);
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "focus")) {
          if (!has_cash(ch, 250)) {
            send_to_char("You'd need at least $250 to make a new focus.\n\r", ch);
            return;
          }
          else if (IS_SET(obj->extra_flags, ITEM_FOCUS)) {
            send_to_char("That is already a mystical focus.\n\r", ch);
            return;
          }

          SET_BIT(obj->extra_flags, ITEM_FOCUS);
          send_to_char("You carve runes into it and turn it into a new mystical focus.\n\r", ch);
          take_cash(ch, 250);
          return;
        }
        else if (!str_cmp(arg2, "angeliccollar")) {
          if (ch->race != RACE_FREEANGEL) {
            send_to_char("You don't know the first thing about angelic collars.\n\r", ch);
            return;
          }
          else if (!has_cash(ch, 500)) {
            send_to_char("You'd need at least $500 to make a new collar.\n\r", ch);
            return;
          }
          else if (obj->item_type != ITEM_JEWELRY) {
            send_to_char("Only jewelry can be made into a collar.\n\r", ch);
            return;
          }
          else if (IS_SET(obj->extra_flags, ITEM_COLLAR)) {
            send_to_char("It already is an angelic collar.\n\r", ch);
            return;
          }

          SET_BIT(obj->extra_flags, ITEM_COLLAR);
          take_cash(ch, 500);
          send_to_char("You fashion a new angelic collar.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "relic")) {
          if (ch->modifier != MODIFIER_RELIC) {
            send_to_char("You don't have a relic.\n\r", ch);
            return;
          }
          if (IS_FLAG(ch->act, PLR_MADERELIC)) {
            send_to_char("You've already claimed your relic.\n\r", ch);
            return;
          }
          if (obj->item_type != ITEM_JEWELRY && obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_RANGED) {
            send_to_char("Only jewelry and weapons can be relics.\n\r", ch);
            return;
          }
          if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
            send_to_char("That already is a relic.\n\r", ch);
            return;
          }

          SET_BIT(obj->extra_flags, ITEM_RELIC);
          free_string(obj->material);
          obj->material = str_dup(ch->name);
          send_to_char("You make a relic.\n\r", ch);
          SET_FLAG(ch->act, PLR_MADERELIC);
          return;
        }
        else if (!str_cmp(arg2, "shortstring")) {
          argument = one_argument_nouncap(argument, arg3);
          remove_color(colbuf, argument);
          free_string(obj->short_descr);
          if (!str_cmp(arg3, "a") || !str_cmp(arg3, "an"))
          obj->short_descr = str_dup(colbuf);
          else {
            sprintf(buf, "%s %s", from_color(arg3), colbuf);
            obj->short_descr = str_dup(buf);
          }
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "longstring")) {
          remove_color(colbuf, argument);
          free_string(obj->description);
          obj->description = str_dup(colbuf);
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "names")) {
          if (safe_strlen(argument) < 2) {
            send_to_char("Syntax: customize (object) names (new names)\n\r", ch);
            return;
          }
          free_string(obj->name);
          obj->name = str_dup(argument);
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "cost")) {
          if (!is_number(argument)) {
            return;
          }

          double temp = atof(argument);

          int newcost = (int)(temp * 100);
          int diffcost = newcost - obj->cost;

          if (newcost < 1)
          return;

          if (diffcost > ch->money + ch->pcdata->total_credit) {
            send_to_char("You don't have enough for that.\n\r", ch);
            return;
          }
          obj->cost = newcost;
          ch->pcdata->total_credit -= diffcost;
          if (diffcost > 25000 && !IS_SET(obj->extra_flags, ITEM_NORESALE))
          SET_BIT(obj->extra_flags, ITEM_NORESALE);
          if (ch->pcdata->total_credit < 0) {
            ch->money += ch->pcdata->total_credit;
            ch->pcdata->total_credit = 0;
          }
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "layer")) {
          if (!is_number(argument))
          return;
          obj->level = atoi(argument);
          obj->level = UMAX(0, obj->level);
          obj->level = UMIN(5, obj->level);
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "wear")) {
          remove_color(colbuf, argument);
          sprintf(buf, "%s", colbuf);
          // sprintf(buf, "%s", argument);
          // sprintf(buf, "%-21.21s", buf);
          free_string(obj->wear_string);
          obj->wear_string = str_dup(buf);
          send_to_char("Done.\n\r", ch);
          return;
        }
        else if (!str_cmp(arg2, "description")) {
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("all", ed->keyword))
            break;
          }

          if (!ed) {
            ed = new_extra_descr();
            ed->keyword = "all";
            ed->next = obj->extra_descr;
            obj->extra_descr = ed;
            string_append(ch, &obj->extra_descr->description);
          }
          else {
            string_append(ch, &ed->description);
          }

          return;
        }
        else if (!str_cmp(arg2, "taste")) {
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+taste", ed->keyword))
            break;
          }

          if (!ed) {
            ed = new_extra_descr();
            ed->keyword = "+taste";
            ed->next = obj->extra_descr;
            obj->extra_descr = ed;
            string_append(ch, &obj->extra_descr->description);
          }
          else
          string_append(ch, &ed->description);

          return;
        }
        else if (!str_cmp(arg2, "playlist")) {
          if (obj->item_type != ITEM_GAME) {
            send_to_char("This item can't have a playlist.\n\r", ch);
            return;
          }
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+playlist", ed->keyword))
            break;
          }

          if (!ed) {
            ed = new_extra_descr();
            ed->keyword = "+playlist";
            ed->next = obj->extra_descr;
            obj->extra_descr = ed;
            string_append(ch, &obj->extra_descr->description);
          }
          else
          string_append(ch, &ed->description);

          return;
        }
        else if (!str_cmp(arg2, "extra")) {
          if (!isalpha(argument[0])) {
            send_to_char("invalid keyword.\n\r", ch);
            return;
          }
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name(argument, ed->keyword))
            break;
          }

          if (!ed) {
            ed = new_extra_descr();
            ed->keyword = str_dup(argument);
            ed->next = obj->extra_descr;
            obj->extra_descr = ed;
            string_append(ch, &obj->extra_descr->description);
          }
          else
          string_append(ch, &ed->description);

          return;
        }
        else {
          send_to_char("Syntax: customize (object) cover/zips/wear/shortstring/longstring/names/description/extra/layer/symbol/cost/taste/playlist/relic/focus/angeliccollar/badge/large/small\n\r", ch);
          return;
        }
      }
    }
    send_to_char("I don't see that.\n\r", ch);
  }

  void clone_obj(CHAR_DATA *ch, OBJ_DATA *base, OBJ_DATA *poi, bool colour) {
    EXTRA_DESCR_DATA *ed;
    //    EXTRA_DESCR_DATA *ped;
    char nocol[MSL];

    if (poi == NULL)
    return;

    int diff = base->cost - poi->cost;

    if (diff > ch->money + ch->pcdata->total_credit + ch->pcdata->total_money) {
      send_to_char("You don't have enough money for that.\n\r", ch);
      return;
    }

    credit_charge(ch, diff); // Disco 9-5-2018
    // ch->money -= diff;

    if (base->item_type == ITEM_CLOTHING || base->item_type == ITEM_JEWELRY)
    poi->level = base->level;

    if (base->item_type == ITEM_CLOTHING)
    poi->value[2] = base->value[2];

    if (base->item_type == ITEM_FOOD) {
      poi->value[0] = base->value[0];
    }
    if (base->item_type == ITEM_DRINK_CON) {
      poi->value[1] = base->value[0];
      poi->value[0] = base->value[0];
    }
    if (base->item_type == ITEM_PERFUME) {
      poi->value[0] = base->value[0];
    }
    poi->size = base->size;

    if (base->item_type == ITEM_CONTAINER) {
      poi->value[0] = 100;
      poi->value[1] = 1;
      poi->value[3] = 300;
    }

    free_string(poi->name);
    remove_color(nocol, base->name);
    poi->name = str_dup(nocol);
    free_string(poi->short_descr);

    if (IS_SET(poi->extra_flags, ITEM_NOINVENTORY))
    REMOVE_BIT(poi->extra_flags, ITEM_NOINVENTORY);

    if (!is_gm(ch) && colour == FALSE) {
      remove_color(nocol, base->short_descr);
      poi->short_descr = str_dup(nocol);
      free_string(poi->description);
      remove_color(nocol, base->description);
      poi->description = str_dup(nocol);
    }
    else {
      poi->short_descr = str_dup(base->short_descr);
      free_string(poi->description);
      poi->description = str_dup(base->description);
    }

    poi->cost = base->cost;

    free_string(poi->wear_string);
    poi->wear_string = str_dup(base->wear_string);

    if (base->item_type == ITEM_JEWELRY) {
      poi->value[3] = base->value[3];
    }
    if (base->item_type == ITEM_CLOTHING) {
      poi->value[0] = base->value[0];
      poi->value[1] = base->value[1];
    }
    if (base->item_type == ITEM_PERFUME) {
      char *temp;
      temp = str_dup("");

      for (ed = base->extra_descr; ed; ed = ed->next) {
        if (is_name("+scent", ed->keyword)) {
          free_string(temp);
          temp = str_dup(ed->description);
          break;
        }
      }

      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("+scent", ed->keyword)) {
          break;
        }
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+scent");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(temp);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(temp);
      }
    }
    if (base->item_type == ITEM_DRUGS) {
      char *temp;
      temp = str_dup("");

      for (ed = base->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword)) {
          free_string(temp);
          temp = str_dup(ed->description);
          break;
        }
      }

      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+imprint");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(temp);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(temp);
      }
    }

    if (base->item_type == ITEM_DRUGS || base->item_type == ITEM_FOOD || base->item_type == ITEM_DRINK_CON) {
      char *temp;
      temp = str_dup("");

      for (ed = base->extra_descr; ed; ed = ed->next) {
        if (is_name("+imprint", ed->keyword)) {
          free_string(temp);
          temp = str_dup(ed->description);
          break;
        }
      }

      for (ed = poi->extra_descr; ed; ed = ed->next) {
        if (is_name("+taste", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+taste");
        ed->next = poi->extra_descr;
        poi->extra_descr = ed;
        free_string(poi->extra_descr->description);
        poi->extra_descr->description = str_dup(temp);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(temp);
      }
    }
    char *newtemp;
    //            free_string(newtemp);
    newtemp = str_dup("");

    for (ed = base->extra_descr; ed; ed = ed->next) {
      if (is_name("all", ed->keyword)) {
        free_string(newtemp);
        newtemp = str_dup(ed->description);
        break;
      }
    }

    for (ed = poi->extra_descr; ed; ed = ed->next) {
      if (is_name("all", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("all");
      ed->next = poi->extra_descr;
      poi->extra_descr = ed;
      free_string(poi->extra_descr->description);
      poi->extra_descr->description = str_dup(newtemp);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(newtemp);
    }

    send_to_char("Object cloned.\n\r", ch);
  }

  _DOFUN(do_clone) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    OBJ_DATA *toclone = NULL;
    OBJ_DATA *base = NULL;

    if (ch->carrying == NULL || ch->carrying->next_content == NULL)
    return;

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (is_name(arg1, obj->name)) {
        toclone = obj;
      }
      else if (is_name(argument, obj->name)) {
        base = obj;
      }
    }
    if (base == NULL || toclone == NULL || base->item_type != toclone->item_type) {
      send_to_char("Syntax: clone (object to be cloned) (Parent object)\n\r", ch);
      return;
    }
    if ((base->item_type == ITEM_CLOTHING || base->item_type == ITEM_CONTAINER) && !at_tailor(ch)) {
      send_to_char("You need to be at a tailor to do that.\n\r", ch);
      return;
    }
    if (!at_jeweler(ch) && (base->item_type == ITEM_JEWELRY || base->item_type == ITEM_PHONE)) {
      send_to_char("You need to be at a jeweler to do that.\n\r", ch);
      return;
    }
    if (!at_workshop(ch) && (base->item_type == ITEM_FURNITURE || base->item_type == ITEM_WEAPON || base->item_type == ITEM_RANGED)) {
      send_to_char("You need to be at a workshop to do that.\n\r", ch);
      return;
    }
    if (!at_kitchen(ch) && (base->item_type == ITEM_FOOD || base->item_type == ITEM_DRINK_CON)) {
      send_to_char("You need to be at a kitchen or restaurant to do that.\n\r", ch);
      return;
    }

    clone_obj(ch, base, toclone, FALSE);
  }

  _DOFUN(do_clean) {
    char arg1[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    argument = one_argument_nouncap(argument, arg1);

    if (arg1[0] == '\0') {
      send_to_char("Clean what?\n\r", ch);
      return;
    }
    if (ch->money < 1000) {
      send_to_char("You'd need $10 for that.\n\r", ch);
      return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (obj->item_type != ITEM_CLOTHING && obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_RANGED && obj->item_type != ITEM_JEWELRY)
      continue;

      if (is_name(arg1, obj->name)) {
        if (obj->item_type == ITEM_JEWELRY)
        obj->value[4] = 0;
        else
        obj->value[3] = 0;
        obj->condition = 100;
        ch->money -= 1000;
        printf_to_char(ch, "You have %s cleaned.\n\r", obj->short_descr);
        return;
      }
    }
    send_to_char("You don't seem to have that.\n\r", ch);
  }

  _DOFUN(do_phone) {
    char arg1[MSL];
    OBJ_DATA *phone;
    OBJ_DATA *obj;
    phone = get_eq_char(ch, WEAR_HOLD);
    EXTRA_DESCR_DATA *ed;

    if (phone == NULL || phone->item_type != ITEM_PHONE)
    phone = get_eq_char(ch, WEAR_HOLD_2);

    if (phone == NULL || phone->item_type != ITEM_PHONE) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj->wear_loc != WEAR_NONE || IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (obj->item_type != ITEM_PHONE)
        continue;

        if (IS_SET(obj->extra_flags, ITEM_OFF))
        continue;

        phone = obj;
      }
    }
    if (phone == NULL) {
      send_to_char("You have no phone.\n\r", ch);
      return;
    }
    if (phone->item_type != ITEM_PHONE) {
      send_to_char("You have to hold your phone first.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);
    if (!str_cmp(arg1, "on") || !str_cmp(arg1, "off")) {
      if (IS_SET(phone->extra_flags, ITEM_OFF)) {
        printf_to_char(ch, "You turn %s on.\n\r", phone->description);
        REMOVE_BIT(phone->extra_flags, ITEM_OFF);
        return;
      }
      else {
        printf_to_char(ch, "You turn %s off.\n\r", phone->description);
        SET_BIT(phone->extra_flags, ITEM_OFF);
        if(ch->pcdata->cam_spy_char != NULL)
        ch->pcdata->cam_spy_char = NULL;
        for (vector<MATCH_TYPE *>::iterator it = MatchVect.begin();
        it != MatchVect.end(); ++it) {
          if(!str_cmp(ch->name, (*it)->nameone))
          {
            (*it)->last_msg_one_one = 0;
            (*it)->last_msg_one_two = 0;
            (*it)->last_msg_one_three = 0;
          }
          if(!str_cmp(ch->name, (*it)->nametwo))
          {
            (*it)->last_msg_two_one = 0;
            (*it)->last_msg_two_two = 0;
            (*it)->last_msg_two_three = 0;
          }
        }

        return;
      }
    }
    if (!str_cmp(arg1, "gps")) {
      if (is_town_blackout()) {
        send_to_char("You can't get a signal.\n\r", ch);
        return;
      }
      maketownmap(ch);
      if (!IS_IMMORTAL(ch)) {
        WAIT_STATE(ch, PULSE_PER_SECOND * 15);
      }
      return;
    }
    if (!str_cmp(arg1, "silent")) {
      if (IS_SET(phone->extra_flags, ITEM_SILENT)) {
        printf_to_char(ch, "You switch %s off silent.\n\r", phone->description);
        REMOVE_BIT(phone->extra_flags, ITEM_SILENT);
        return;
      }
      else {
        printf_to_char(ch, "You switch %s to silent.\n\r", phone->description);
        SET_BIT(phone->extra_flags, ITEM_SILENT);
        return;
      }
    }

    if (!str_cmp(arg1, "ringtone")) {
      if (!holding_phone(ch) && !wearing_phone(ch)) {
        send_to_char("You must be holding a phone first.\n\r", ch);
        return;
      }

      for (ed = phone->extra_descr; ed; ed = ed->next) {
        if (is_name("+ringtone", ed->keyword)) {
          break;
        }
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+ringtone");
        ed->next = phone->extra_descr;
        phone->extra_descr = ed;
        free_string(phone->extra_descr->description);
        phone->extra_descr->description = str_dup(argument);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(argument);
      }

      printf_to_char(ch, "Ringtone set to: %s\n\r", argument);
      return;
    }
    if (!str_cmp(arg1, "clone")) {
      if (!at_workshop(ch) && !at_jeweler(ch)) {
        send_to_char("You need to be at a suitable shopkeeper to do that.\n\r", ch);
        return;
      }
      if (ch->money < 20000) {
        send_to_char("That costs $200.\n\r", ch);
        return;
      }

      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      OBJ_DATA *obj_next;

      OBJ_DATA *toclone;
      OBJ_DATA *base;

      bool foundtoclone = FALSE;
      bool foundbase = FALSE;

      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (obj->item_type != ITEM_PHONE || IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
          continue;
        }

        if (is_name(arg2, obj->name)) {
          foundtoclone = TRUE;
          toclone = obj;
        }
        else if (is_name(argument, obj->name)) {
          foundbase = TRUE;
          base = obj;
        }
      }

      if (base == NULL || toclone == NULL || foundtoclone == FALSE || foundbase == FALSE) {
        send_to_char("Syntax: Phone clone (phone to be cloned) (master phone)\n\r", ch);
        return;
      }
      ch->money -= 20000;
      toclone->value[4] = base->value[0];
      printf_to_char(ch, "You clone %s, slaving it to %s.\n\r", toclone->description, base->description);
      act("$n does something technical with a phone.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    if (!str_cmp(arg1, "clean")) {
      if (!at_workshop(ch) && !at_jeweler(ch)) {
        send_to_char("There's nobody here to help you with that.\n\r", ch);
        return;
      }
      if (ch->money < 10000) {
        send_to_char("That would cosst $100.\n\r", ch);
        return;
      }
      phone->value[4] = 0;
      printf_to_char(ch, "You clean %s of any spyware.\n\r", phone->description);
      act("$n does something technical with a phone.", ch, NULL, NULL, TO_ROOM);
      ch->money -= 10000;
      return;
    }

    if (!str_cmp(arg1, "signalboost")) {
      if (get_skill(ch, SKILL_ENGINEERING) < 3) {
        send_to_char("Your engineering skill is insufficient.\n\r", ch);
        return;
      }
      if (!SET_FLAG(ch->comm, COMM_RACIAL)) {
        send_to_char("You're tired and don't want to work on another phone yet.\n\r", ch);
        return;
      }

      for (ed = phone->extra_descr; ed; ed = ed->next) {
        if (is_name("+signalboost", ed->keyword)) {
          break;
        }
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+signalboost");
        ed->next = phone->extra_descr;
        phone->extra_descr = ed;
        free_string(phone->extra_descr->description);
        phone->extra_descr->description = str_dup(ch->name);
        printf_to_char(ch, "You boost the signal for %s.\n\r", phone->description);
      }
      else {
        ed->description = str_dup("");
        free_string(ed->description);
        ed->keyword = str_dup("");
        free_string(ed->keyword);
        printf_to_char(ch, "You remove the signal boosting for %s.\n\r", phone->description);
      }

      act("$n does something technical with a phone.", ch, NULL, NULL, TO_ROOM);
      SET_FLAG(ch->comm, COMM_RACIAL);
      return;
    }

    send_to_char("Phone <on/off/silent/ringtone (string)/clone/clean/signalboost>\n\r", ch);
  }

  // Based on mult_argument for dice rolling - Discordance
  int dice_argument(char *argument, char *arg) {
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++) {
      if (*pdot == 'd') {
        *pdot = '\0';
        number = atoi(argument);
        *pdot = 'd';
        strcpy(arg, pdot + 1);
        return number;
      }
    }

    strcpy(arg, argument);
    return 1;
  }

  bool has_dice(CHAR_DATA *ch) {

    OBJ_DATA *obj;

    obj = ch->on;
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (strcasestr(ch->pcdata->place, "computer") != NULL)
    return TRUE;

    if (obj != NULL) {
      if (strcasestr(obj->name, "computer") != NULL)
      return TRUE;
    }

    if (get_held(ch, ITEM_CONTAINER) != NULL) {
      if (strcasestr(get_held(ch, ITEM_CONTAINER)->name, "computer"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_CONTAINER)->name, "laptop"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_CONTAINER)->name, "tablet"))
      return TRUE;
    }

    if (get_held(ch, ITEM_TRASH) != NULL) {
      if (strcasestr(get_held(ch, ITEM_TRASH)->name, "computer"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_TRASH)->name, "laptop"))
      return TRUE;
      if (strcasestr(get_held(ch, ITEM_TRASH)->name, "tablet"))
      return TRUE;
    }

    if (get_held(ch, ITEM_PHONE) != NULL) {
      if (get_held(ch, ITEM_PHONE)->cost >= 20000)
      return TRUE;
    }

    if (get_worn(ch, ITEM_CONTAINER) != NULL) {
      if (strcasestr(get_worn(ch, ITEM_CONTAINER)->name, "computer"))
      return TRUE;
    }

    return FALSE;
  }

  // for rolling dice objects
  _DOFUN(do_roll) {
    int chance = 0, i = 0, sides = 0, dice = 0, dice_total = 0, goal = 0;
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    //    char roll[MSL];
    char buf[MSL];

    if (!IS_IMMORTAL(ch) && is_gm(ch)) {
      send_to_char("This command is player only.\n\r", ch);
      return;
    }

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);

    dice = atoi(arg1);
    sides = atoi(arg2);
    goal = atoi(arg3);

    if (dice <= 0 || sides <= 0 || goal <= 0 || dice > 10 || sides > 100 || goal > 1000) {
      send_to_char("`gSyntax`x: `WRoll `g(`Wnumber of dice`g) (`Wnumber of sides per die`g) (`WGoal`g)`x\n\r", ch);
      return;
    }
    int lucky = 0;
    int unlucky = 0;

    if (IS_AFFECTED(ch, AFF_LUCKY) && !is_faeborn(ch))
    lucky = 1;
    if (IS_AFFECTED(ch, AFF_UNLUCKY) && !is_faeborn(ch))
    unlucky = 1;

    if (is_faeborn(ch) && number_percent() % 2 == 0)
    lucky = 1;
    else if (is_faeborn(ch) && number_percent() % 2 == 0)
    unlucky = 1;

    for (i = 0; i < dice; i++) {
      chance = number_range(1, 124672) % sides;
      chance++;
      if (i == dice - 1) {
        if (lucky == 1 && dice_total + chance < goal && chance < sides)
        chance++;
        else if (unlucky == 1 && dice_total + chance <= goal && chance > 1)
        chance--;
        else if (lucky == 1 && dice_total + chance > goal && chance > 1)
        chance--;
        else if (unlucky == 1 && dice_total + chance >= goal && chance < sides)
        chance++;
      }
      sprintf(buf, "$n rolls a %d.", chance);
      act(buf, ch, NULL, NULL, TO_ROOM);
      act(buf, ch, NULL, NULL, TO_CHAR);
      dice_total = dice_total + chance;
    }

    sprintf(
    buf, "\n\r$n gets a total of %d from rolling %d %ds and trying to get %d.\n\r", dice_total, dice, sides, goal);
    act(buf, ch, NULL, NULL, TO_ROOM);
    act(buf, ch, NULL, NULL, TO_CHAR);
  }

#define GAME_RANDOM 0
#define GAME_SEQUENTIAL 1

  void use_game(CHAR_DATA *ch, OBJ_DATA *obj) {
    char buf[MSL];

    EXTRA_DESCR_DATA *ed;
    bool tastefound = FALSE;
    for (ed = obj->extra_descr; ed && tastefound == FALSE; ed = ed->next) {
      if (is_name("+playlist", ed->keyword)) {
        tastefound = TRUE;
        if (obj->value[0] == GAME_RANDOM) {
          int value = number_range(1, linecount(ed->description));
          sprintf(buf, "$n uses %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, value));
          act(buf, ch, obj, NULL, TO_ROOM);
          sprintf(buf, "You use %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, value));
          act(buf, ch, obj, NULL, TO_CHAR);
          return;
        }
        if (obj->value[0] == GAME_SEQUENTIAL) {
          int value = linecount(ed->description);
          if (obj->value[1] == 0)
          obj->value[1] = 1;
          if (obj->value[1] > value)
          obj->value[1] = 1;
          sprintf(buf, "$n uses %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, obj->value[1]));
          act(buf, ch, obj, NULL, TO_ROOM);
          sprintf(buf, "You use %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, obj->value[1]));
          act(buf, ch, obj, NULL, TO_CHAR);
          obj->value[1]++;
          return;
        }
      }
    }
    if (tastefound == FALSE) {
      for (ed = obj->pIndexData->extra_descr; ed && tastefound == FALSE;
      ed = ed->next) {
        if (is_name("+playlist", ed->keyword)) {
          tastefound = TRUE;
          if (obj->value[0] == GAME_RANDOM) {
            int value = number_range(1, linecount(ed->description));
            sprintf(buf, "$n uses %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, value));
            act(buf, ch, obj, NULL, TO_ROOM);
            sprintf(buf, "You use %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, value));
            act(buf, ch, obj, NULL, TO_CHAR);
            return;
          }
          if (obj->value[0] == GAME_SEQUENTIAL) {
            int value = linecount(ed->description);
            if (obj->value[1] == 0)
            obj->value[1] = 1;
            if (obj->value[1] > value)
            obj->value[1] = 1;
            sprintf(buf, "$n uses %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, obj->value[1]));
            act(buf, ch, obj, NULL, TO_ROOM);
            sprintf(buf, "You use %s $p: %s", a_or_an(obj->short_descr), fetch_line(ed->description, obj->value[1]));
            act(buf, ch, obj, NULL, TO_CHAR);
            obj->value[1]++;
            return;
          }
        }
      }
    }
    if (tastefound == FALSE) {
      send_to_char("That doesn't seem to have anything to play.\n\r", ch);
      return;
    }
  }

  _DOFUN(do_use) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_INDEX_DATA *pObjIndex;
    char buf[MSL], arg1[MSL], arg2[MSL];
    int chance, target;

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return;

    if (is_ghost(ch) && !is_manifesting(ch)) {
      send_to_char("You must manifest to do that.\n\r", ch);
      return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;
      if (is_name(arg1, obj->name) && can_see_obj(ch, obj)) {
        // Umbrellas
        if (obj->item_type == ITEM_UMBRELLA) {
          if (IS_SET(obj->extra_flags, ITEM_OFF)) {
            REMOVE_BIT(obj->extra_flags, ITEM_OFF);
            act("$n opens $s umbrella.", ch, NULL, NULL, TO_ROOM);
            act("You open your umbrella.", ch, NULL, NULL, TO_CHAR);
            obj->size += 50;
          }
          else {
            SET_BIT(obj->extra_flags, ITEM_OFF);
            act("$n closes $s umbrella.", ch, NULL, NULL, TO_ROOM);
            act("You close your umbrella.", ch, NULL, NULL, TO_CHAR);
            obj->size -= 50;
            if (obj->size < 1)
            obj->size = 1;
          }
          return;
        }

        // Flashlights
        if (obj->item_type == ITEM_FLASHLIGHT) {
          if (IS_SET(obj->extra_flags, ITEM_OFF)) {
            REMOVE_BIT(obj->extra_flags, ITEM_OFF);
            act("$n switches on $s $p.", ch, obj, NULL, TO_ROOM);
            act("You switch on your $p.", ch, obj, NULL, TO_CHAR);
          }
          else {
            SET_BIT(obj->extra_flags, ITEM_OFF);
            act("$n switches off $s flashlight.", ch, NULL, NULL, TO_ROOM);
            act("You switch off your flashlight.", ch, NULL, NULL, TO_CHAR);
          }
          return;
        }

        // Stakes
        if (strcasestr(obj->name, "stake") != NULL) {
          stake(ch, arg2, obj);
          return;
        }

        /*
        if(strcasestr(obj->name, "dice") != NULL) {
        do_function(ch, &do_roll, "");
        }
        */
        // Pregnancy tests
        if (strcasestr(obj->name, "pregnancy") != NULL && strcasestr(obj->name, "test") != NULL) {
          char result[MSL];
          strcpy(result, "");
          if (obj->cost > 100) {
            act("$n urinates on $s pregnancy test.", ch, NULL, NULL, TO_ROOM);
            act("You urinate on your pregnancy test.", ch, NULL, NULL, TO_CHAR);
            obj->cost = 0;
            if (ch->pcdata->impregnated != 0 && ch->pcdata->impregnated + (3600 * 60 * 24 * 7) < current_time) {
              strcpy(result, "+");
            }
            else if (ch->pcdata->impregnated != 0) {
              // Pregnancy tests are best used at least a week after a missed
              // period
              chance = number_range(1, 100);
              // Period has been missed
              if (ch->pcdata->menstruation <= current_time) {
                if (ch->pcdata->menstruation + (3600 * 24 * 6) < current_time) {
                  target = 50;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation + (3600 * 24 * 5) <
                    current_time) {
                  target = 66;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation + (3600 * 24 * 4) <
                    current_time) {
                  target = 75;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation + (3600 * 24 * 3) <
                    current_time) {
                  target = 80;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation + (3600 * 24 * 2) <
                    current_time) {
                  target = 91;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation + (3600 * 24 * 1) <
                    current_time) {
                  target = 92;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else {
                  target = 93;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
              }
              // Period has not been missed yet
              else {
                if (ch->pcdata->menstruation - (3600 * 24 * 1) < current_time) {
                  target = 94;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation - (3600 * 24 * 2) <
                    current_time) {
                  target = 95;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation - (3600 * 24 * 3) <
                    current_time) {
                  target = 96;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation - (3600 * 24 * 4) <
                    current_time) {
                  target = 97;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation - (3600 * 24 * 5) <
                    current_time) {
                  target = 98;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else if (ch->pcdata->menstruation - (3600 * 24 * 6) <
                    current_time) {
                  target = 99;
                  if (chance >= target) {
                    strcpy(result, "+");
                  }
                }
                else {
                  strcpy(result, "-");
                }
              }
              if (chance < target) {
                strcpy(result, "-");
              }
            }
            else {
              strcpy(result, "-");
            }
          }
          else if (obj->cost > 0) {
            strcpy(result, "-");
          }
          else {
            return;
          }
          if (obj->extra_descr == NULL)
          return;
          printf_to_char(ch, "The test displays: %s\n\r", result);
          sprintf(buf, "%s\n\n\rThe test displays: %s\n\r", obj->extra_descr->description, result);
          free_string(obj->extra_descr->description);
          obj->extra_descr->description = str_dup(buf);
          return;
        }
        // Drugs
        if (obj->item_type == ITEM_GAME) {
          use_game(ch, obj);
          return;
        }
        if (obj->item_type == ITEM_DRUGS) {
          EXTRA_DESCR_DATA *ed;
          act("$n takes $s $p.", ch, obj, NULL, TO_ROOM);
          act("You take your $p.", ch, obj, NULL, TO_CHAR);

          bool tastefound = FALSE;
          bool imprintfound = FALSE;
          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+taste", ed->keyword)) {
              send_to_char(ed->description, ch);
              tastefound = TRUE;
            }
            if (is_name("+imprint", ed->keyword)) {
              auto_imprint(ch, ed->description, IMPRINT_DRUGS);
              imprintfound = TRUE;
            }
          }
          for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
            if (is_name("+taste", ed->keyword) && !tastefound) {
              send_to_char(ed->description, ch);
              tastefound = TRUE;
            }
            if (is_name("+imprint", ed->keyword) && !imprintfound) {
              auto_imprint(ch, ed->description, IMPRINT_DRUGS);
              imprintfound = TRUE;
            }
          }

          extract_obj(obj);
          return;
        }
        if (obj->item_type == ITEM_PERFUME) {
          EXTRA_DESCR_DATA *ed;
          act("$n sprays on some $p.", ch, obj, NULL, TO_ROOM);
          act("You spray on some $p.", ch, obj, NULL, TO_CHAR);
          bool scentfound = FALSE;

          for (ed = obj->extra_descr; ed; ed = ed->next) {
            if (is_name("+scent", ed->keyword) && obj->pIndexData->value[0] > 0) {
              free_string(ch->pcdata->scent);
              ch->pcdata->scent = str_dup(ed->description);
              ch->pcdata->perfume_cost =
              ((10 * obj->cost) / obj->pIndexData->value[0]);
              scentfound = TRUE;
            }
          }
          for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next) {
            if (is_name("+scent", ed->keyword) && !scentfound && obj->pIndexData->value[0] > 0) {
              free_string(ch->pcdata->scent);
              ch->pcdata->scent = str_dup(ed->description);
              ch->pcdata->perfume_cost = obj->cost / 100;
              scentfound = TRUE;
            }
          }

          obj->value[0]--;
          if (obj->value[0] <= 0)
          extract_obj(obj);
          return;
        }
      }
    }

    obj = get_obj_list(ch, arg1, ch->in_room->contents);
    if (obj != NULL && obj->item_type == ITEM_GAME) {
      use_game(ch, obj);
      return;
    }
    send_to_char("Use what?\n\r", ch);
  }

  _DOFUN(do_expose) {
    int location, i, iWear;
    char arg1[MSL];
    char buf[MSL];
    argument = one_argument_nouncap(argument, arg1);
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char *buf2 = NULL;

    if (!str_cmp(arg1, "hands"))
    location = 0;
    else if (!str_cmp(arg1, "lowerarms"))
    location = 1;
    else if (!str_cmp(arg1, "upperarms"))
    location = 2;
    else if (!str_cmp(arg1, "feet"))
    location = 3;
    else if (!str_cmp(arg1, "lowerlegs"))
    location = 4;
    else if (!str_cmp(arg1, "forehead"))
    location = 5;
    else if (!str_cmp(arg1, "thighs"))
    location = 6;
    else if (!str_cmp(arg1, "groin"))
    location = 7;
    else if (!str_cmp(arg1, "arse") || !str_cmp(arg1, "buttocks"))
    location = 8;
    else if (!str_cmp(arg1, "lowerback"))
    location = 9;
    else if (!str_cmp(arg1, "upperback"))
    location = 10;
    else if (!str_cmp(arg1, "lowerchest"))
    location = 11;
    else if (!str_cmp(arg1, "breasts"))
    location = 12;
    else if (!str_cmp(arg1, "upperchest"))
    location = 13;
    else if (!str_cmp(arg1, "neck"))
    location = 14;
    else if (!str_cmp(arg1, "lowerface"))
    location = 15;
    else if (!str_cmp(arg1, "hair"))
    location = 16;
    else if (!str_cmp(arg1, "eyes"))
    location = 17;
    else {
      if ((obj = get_obj_wear(ch, arg1, TRUE)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }

      act("You adjust your $a $p.", ch, obj, NULL, TO_CHAR);
      act("$n adjusts $s $p.", ch, obj, NULL, TO_ROOM);

      for (i = 0; i < MAX_COVERS; i++) {
        obj->exposed = 0;
        if (is_covered(ch, cover_table[i])) {
          obj->exposed = 111;
          if (!is_covered(ch, cover_table[i]) && safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
            buf2 = str_dup(ch->pcdata->focused_descs[i]);
            buf2 = one_argument_nouncap(buf2, arg1);
            if (is_number(arg1))
            sprintf(buf, "Revealing that; %s", buf2);
            else
            sprintf(buf, "Revealing that; %s %s", arg1, buf2);
            act(buf, ch, NULL, NULL, TO_CHAR);
            act(buf, ch, NULL, NULL, TO_ROOM);
          }
        }
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        obj->exposed = 0;
        if ((obj2 = get_eq_char(ch, iWear)) == NULL || !can_see_obj(ch, obj) || !can_see_wear(ch, iWear)) {
          obj->exposed = 111;
          if ((obj2 = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && can_see_wear(ch, iWear)) {
            sprintf(buf, "Revealing;%s $o", obj2->wear_string);
            act(buf, ch, obj2, NULL, TO_CHAR);
            act(buf, ch, obj2, NULL, TO_ROOM);
          }
        }
      }

      obj->exposed = 111;
      return;
    }

    act("You adjust your clothes.", ch, NULL, NULL, TO_CHAR);
    act("$n adjusts $s clothes.", ch, NULL, NULL, TO_ROOM);

    for (i = 0; i < MAX_COVERS; i++) {
      ch->pcdata->exposed[location] = 0;
      if (is_covered(ch, cover_table[i])) {
        ch->pcdata->exposed[location] = 111;
        if (!is_covered(ch, cover_table[i]) && safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
          buf2 = str_dup(ch->pcdata->focused_descs[i]);
          buf2 = one_argument_nouncap(buf2, arg1);
          if (is_number(arg1))
          sprintf(buf, "Revealing that; %s", buf2);
          else
          sprintf(buf, "Revealing that; %s %s", arg1, buf2);
          act(buf, ch, NULL, NULL, TO_CHAR);
          act(buf, ch, NULL, NULL, TO_ROOM);
        }
      }
    }

    ch->pcdata->exposed[location] = 111;
  }

  _DOFUN(do_unexpose) {
    int location, i, iWear;
    char arg1[MSL];
    char buf[MSL];

    argument = one_argument_nouncap(argument, arg1);
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char *buf2 = NULL;

    if (!str_cmp(arg1, "hands"))
    location = 0;
    else if (!str_cmp(arg1, "lowerarms"))
    location = 1;
    else if (!str_cmp(arg1, "upperarms"))
    location = 2;
    else if (!str_cmp(arg1, "feet"))
    location = 3;
    else if (!str_cmp(arg1, "lowerlegs"))
    location = 4;
    else if (!str_cmp(arg1, "forehead"))
    location = 5;
    else if (!str_cmp(arg1, "thighs"))
    location = 6;
    else if (!str_cmp(arg1, "groin"))
    location = 7;
    else if (!str_cmp(arg1, "arse") || !str_cmp(arg1, "Buttocks"))
    location = 8;
    else if (!str_cmp(arg1, "lowerback"))
    location = 9;
    else if (!str_cmp(arg1, "upperback"))
    location = 10;
    else if (!str_cmp(arg1, "lowerchest"))
    location = 11;
    else if (!str_cmp(arg1, "breasts"))
    location = 12;
    else if (!str_cmp(arg1, "upperchest"))
    location = 13;
    else if (!str_cmp(arg1, "neck"))
    location = 14;
    else if (!str_cmp(arg1, "lowerface"))
    location = 15;
    else if (!str_cmp(arg1, "hair"))
    location = 16;
    else if (!str_cmp(arg1, "eyes"))
    location = 17;
    else {
      if ((obj = get_obj_wear(ch, arg1, TRUE)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
      }

      act("You adjust your $a $p.", ch, obj, NULL, TO_CHAR);
      act("$n adjusts $s $p.", ch, obj, NULL, TO_ROOM);

      for (i = 0; i < MAX_COVERS; i++) {
        obj->exposed = 111;
        if (!is_covered(ch, cover_table[i])) {
          obj->exposed = 0;
          if (is_covered(ch, cover_table[i]) && safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
            buf2 = str_dup(ch->pcdata->focused_descs[i]);
            buf2 = one_argument_nouncap(buf2, arg1);
            if (is_number(arg1))
            sprintf(buf, "Concealing that; %s", buf2);
            else
            sprintf(buf, "Concealing that; %s %s", arg1, buf2);
            act(buf, ch, NULL, NULL, TO_CHAR);
            act(buf, ch, NULL, NULL, TO_ROOM);
          }
        }
      }
      for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        obj->exposed = 111;
        if ((obj2 = get_eq_char(ch, iWear)) == NULL || !can_see_obj(ch, obj) || can_see_wear(ch, iWear)) {
          obj->exposed = 0;
          if ((obj2 = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && !can_see_wear(ch, iWear)) {
            sprintf(buf, "Concealing;%s $o", obj2->wear_string);
            act(buf, ch, obj2, NULL, TO_CHAR);
            act(buf, ch, obj2, NULL, TO_ROOM);
          }
        }
      }

      obj->exposed = 0;
      return;
    }

    act("You adjust your clothes.", ch, NULL, NULL, TO_CHAR);
    act("$n adjusts $s clothes.", ch, NULL, NULL, TO_ROOM);

    for (i = 0; i < MAX_COVERS; i++) {
      ch->pcdata->exposed[location] = 111;
      if (!is_covered(ch, cover_table[i])) {
        ch->pcdata->exposed[location] = 0;
        if (is_covered(ch, cover_table[i]) && safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
          buf2 = str_dup(ch->pcdata->focused_descs[i]);
          buf2 = one_argument_nouncap(buf2, arg1);
          if (is_number(arg1))
          sprintf(buf, "Concealing that; %s", buf2);
          else
          sprintf(buf, "Concealing that; %s %s", arg1, buf2);
          act(buf, ch, NULL, NULL, TO_CHAR);
          act(buf, ch, NULL, NULL, TO_ROOM);
        }
      }
    }

    ch->pcdata->exposed[location] = 0;
  }

  _DOFUN(do_arrange) {
    char arg1[MSL];
    char buf[MSL];
    OBJ_DATA *obj;
    argument = one_argument(argument, arg1);

    if (get_obj_list(ch, arg1, ch->in_room->contents) == NULL) {
      send_to_char("You don't see that here.\n\r", ch);
      return;
    }

    obj = get_obj_list(ch, arg1, ch->in_room->contents);

    if (!str_cmp(argument, "")) {
      free_string(obj->adjust_string);
      obj->adjust_string = str_dup("");

      sprintf(buf, "You arrange %s so it is %s.", obj->short_descr, obj->description);
      act(buf, ch, NULL, NULL, TO_CHAR);
      sprintf(buf, "$n arranges %s so it is %s.", obj->short_descr, obj->description);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    else {
      free_string(obj->adjust_string);
      obj->adjust_string = str_dup(argument);

      sprintf(buf, "You arrange %s so it is %s.", obj->short_descr, argument);
      act(buf, ch, NULL, NULL, TO_CHAR);
      sprintf(buf, "$n arranges %s so it is %s.", obj->short_descr, argument);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
  }

  void setdescs(OBJ_DATA *obj) {
    OBJ_INDEX_DATA *pObjIndex = obj->pIndexData;

    EXTRA_DESCR_DATA *ed, *ed_new;

    for (ed = pObjIndex->extra_descr; ed != NULL; ed = ed->next) {
      ed_new = new_extra_descr();
      ed_new->keyword = str_dup(ed->keyword);
      ed_new->description = str_dup(ed->description);
      ed_new->next = obj->extra_descr;
      obj->extra_descr = ed_new;
    }
  }

  void start_vomit(CHAR_DATA *ch) {
    ch->pcdata->vomit_timer = 20;
    send_to_char("You start to feel sick, certain you're about to vomit. `W(Type 'vomit' to throw up.)`x\n\r", ch);
  }

  _DOFUN(do_vomit) {
    send_to_char("You vomit.\n\r", ch);
    act("$n suddenly vomits.", ch, NULL, NULL, TO_ROOM);
    ch->pcdata->conditions[COND_DRUNK] -= 10;

    ch->pcdata->poisontimer = 0;
    ch->pcdata->rohyptimer = 0;
    ch->pcdata->vamp_fullness = 0;
  }

  _DOFUN(do_burn) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj = NULL;
    char buf[MSL];
    one_argument(argument, arg);

    if (arg[0] == '\0') {
      send_to_char("What do you want to burn?\n\r", ch);
      return;
    }

    if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL || !is_name(arg, obj->name)) {
      obj = get_obj_carry(ch, arg, ch);
    }
    if (obj == NULL) {
      obj = get_obj_list(ch, arg, ch->in_room->contents);
    }
    if (obj == NULL) {
      send_to_char("You can't find it.\n\r", ch);
      return;
    }
    if (obj->item_type != ITEM_CORPSE_NPC) {
      send_to_char("It doesn't look flamable.\n\r", ch);
      return;
    }
    if (in_fight(ch) || is_pinned(ch) || is_helpless(ch) || is_ghost(ch)) {
      send_to_char("You can't do that right now.\n\r", ch);
      return;
    }

    if (public_room(ch->in_room) || IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
      send_to_char("You should probably take that outside and somewhere where there aren't people around.\n\r", ch);
      return;
    }

    if (!has_consume(ch, 46006)) {
      send_to_char("You'd need some gasoline first.\n\r", ch);
      return;
    }

    free_string(obj->material);
    obj->material = str_dup("");

    act("You burn $a $p.", ch, obj, NULL, TO_CHAR);
    act("$n burns $a $p.", ch, obj, NULL, TO_ROOM);

    sprintf(buf, "%s(`rbadly burned`x)", obj->short_descr);
    free_string(obj->short_descr);
    obj->short_descr = str_dup(buf);

    sprintf(buf, "%s(`rbadly burned`x)", obj->description);
    free_string(obj->description);
    obj->description = str_dup(buf);
  }

  _DOFUN(do_confiscate) {
    CHAR_DATA *victim;
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    OBJ_DATA *obj;

    if (is_helpless(ch) || in_fight(ch))
    return;

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(victim->name, ch->pcdata->relationship[i]) && (ch->pcdata->relationship_type[i] == REL_PARENT || ch->pcdata->relationship_type[i] == REL_GUARDIAN)) {
        if (is_number(arg2)) {
          act("You attempt to confiscate some of $N's money.", ch, NULL, victim, TO_CHAR);
          act("$n attempts to confiscate some of your money.", ch, NULL, victim, TO_VICT);
          sprintf(buf, "%s %s %s", arg2, argument, PERS(ch, victim));
          do_function(victim, &do_give, buf);
          return;
        }
        else {
          for (obj = victim->carrying; obj != NULL; obj = obj->next_content) {
            if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->stash_room != victim->in_room->vnum && can_access(victim, obj->stash_room))
            continue;

            if (is_name(arg2, obj->name)) {
              act("You attempt to confiscate some of $N's belongings.", ch, NULL, victim, TO_CHAR);
              act("$n attempts to confiscate some of your belongings.", ch, NULL, victim, TO_VICT);
              if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
              REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
              sprintf(buf, "%s %s", arg2, PERS(ch, victim));
              do_function(victim, &do_give, buf);
            }
          }
        }
      }
    }
  }

  _DOFUN(do_plantcamera) {
    if (is_helpless(ch) || is_ghost(ch) || is_pinned(ch) || in_fight(ch) || IS_FLAG(ch->act, PLR_SHROUD) || ch->shape != SHAPE_HUMAN) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->in_room->area->vnum == 1 || ch->in_room->area->vnum == 12 || !in_haven(ch->in_room)) {
      send_to_char("You can't plant a camera here.\n\r", ch);
      return;
    }

    if(!can_manual_task(ch))
    {
      send_to_char("You cannot do manual tasks.\n\r", ch);
      return;
    }
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char arg1[MSL];
    OBJ_DATA *phone = NULL;
    argument = one_argument_nouncap(argument, arg1);

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (obj->item_type != ITEM_PHONE)
      continue;

      if (is_name(arg1, obj->name) && can_see_obj(ch, obj)) {
        phone = obj;
      }
    }
    if (phone == NULL) {
      send_to_char("Syntax: plantcamera (phone to sync it to)\n\r", ch);
      return;
    }
    if (!has_consume(ch, 46405)) {
      send_to_char("You need to buy a spy camera from radio shack first.\n\r", ch);
      return;
    }

    EXTRA_DESCR_DATA *ed;
    char buf[MSL];

    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!bugs", ed->keyword))
      break;
    }
    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("!bugs");
      ed->next = ch->in_room->extra_descr;
      ch->in_room->extra_descr = ed;
      free_string(ch->in_room->extra_descr->description);
      sprintf(buf, "%d", phone->value[0]);
      ch->in_room->extra_descr->description = str_dup(buf);
    }
    else {
      sprintf(buf, "%s %d", ed->description, phone->value[0]);
      free_string(ed->description);
      ed->description = str_dup(buf);
    }
    printf_to_char(ch, "You discreetly plant the spycamera and sync it up to broadcast to %s.\n\r", phone->description);
    act("$n plants a spy camera.", ch, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_bugsweep) {
    if (ch->money < 5000) {
      send_to_char("You don't have enough to afford that.\n\r", ch);
      return;
    }
    if(!can_manual_task(ch))
    {
      send_to_char("You cannot do manual tasks.\n\r", ch);
      return;
    }

    if (is_helpless(ch) || is_ghost(ch) || is_pinned(ch) || in_fight(ch) || IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    ch->money -= 5000;

    EXTRA_DESCR_DATA *ed;

    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!bugs", ed->keyword))
      break;
    }
    if (!ed) {
      send_to_char("The bugsweep comes up empty.\n\r", ch);
    }
    else if (safe_strlen(ed->description) < 2)
    send_to_char("The bugsweep comes up empty.\n\r", ch);
    else {
      send_to_char("Your sweep detects and neutralizing some spyware.\n\r", ch);
      free_string(ed->description);
      ed->description = str_dup("");
    }
    act("$n sweeps the room for bugs.", ch, NULL, NULL, TO_ROOM);
  }

  bool has_cash(CHAR_DATA *ch, int amount) {
    amount *= 100;
    if (amount <= ch->money + ch->pcdata->total_credit)
    return TRUE;

    return FALSE;
  }
  void take_cash(CHAR_DATA *ch, int amount) {
    amount *= 100;
    if (amount > ch->pcdata->total_credit) {
      amount -= ch->pcdata->total_credit;
      ch->pcdata->total_credit = 0;
      ch->money -= amount;
    }
    else
    ch->pcdata->total_credit -= amount;
  }

  vector<PHONEBOOK_TYPE *> PhoneVect;
  PHONEBOOK_TYPE *nullphone;

  void fread_phonebook(FILE *fp) {

    char buf[MSL];
    const char *word;
    bool fMatch;
    PHONEBOOK_TYPE *phone;

    phone = new_phonebook();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          PhoneVect.push_back(phone);
          return;
        }
        break;
      case 'I':
        KEY("Inactivity", phone->inactivity, fread_number(fp));
        break;
      case 'O':
        KEY("Owner", phone->owner, fread_string(fp));
        break;
      case 'N':
        KEY("Number", phone->number, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_phonebook: no match: %s", word);
        bug(buf, 0);
      }
    }
  }
  void load_phonebooks() {
    nullphone = new_phonebook();
    FILE *fp;

    if ((fp = fopen("../data/phonebook.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          bug("Load_Phonebooks: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PHONEBOOK")) {
          fread_phonebook(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_phonebooks: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open phonebook.txt", 0);
      exit(0);
    }
  }
  void save_phonebooks(bool backup) {
    FILE *fpout;
    char buf[MSL];

    if (backup) {
      if (time_info.day % 7 == 0)
      sprintf(buf, "../data/back1/phonebook.txt");
      else if (time_info.day % 6 == 0)
      sprintf(buf, "../data/back2/phonebook.txt");
      else if (time_info.day % 5 == 0)
      sprintf(buf, "../data/back3/phonebook.txt");
      else if (time_info.day % 4 == 0)
      sprintf(buf, "../data/back4/phonebook.txt");
      else if (time_info.day % 3 == 0)
      sprintf(buf, "../data/back5/phonebook.txt");
      else if (time_info.day % 2 == 0)
      sprintf(buf, "../data/back6/phonebook.txt");
      else
      sprintf(buf, "../data/back7/phonebook.txt");

      if ((fpout = fopen(buf, "w")) == NULL) {
        bug("Cannot open phonebook for writing", 0);
        return;
      }
    }
    else {
      if ((fpout = fopen("../data/phonebook.txt", "w")) == NULL) {
        bug("Cannot open institute.txt for writing", 0);
        return;
      }
    }

    for (vector<PHONEBOOK_TYPE *>::iterator it = PhoneVect.begin();
    it != PhoneVect.end(); ++it) {
      (*it)->inactivity += 15;
      if ((*it)->inactivity > 5000 || (*it)->number <= 0)
      continue;

      fprintf(fpout, "#PHONEBOOK\n");
      fprintf(fpout, "Owner %s~\n", (*it)->owner);
      fprintf(fpout, "Number %d\n", (*it)->number);
      fprintf(fpout, "Inactivity %d\n", (*it)->inactivity);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    if (backup == FALSE)
    save_phonebooks(TRUE);
  }

  void update_phonebook(int number, char *owner) {
    for (vector<PHONEBOOK_TYPE *>::iterator it = PhoneVect.begin();
    it != PhoneVect.end(); ++it) {
      PHONEBOOK_TYPE *phone = (*it);
      if (phone->number == number) {
        phone->inactivity = 0;
        free_string(phone->owner);
        phone->owner = str_dup(owner);
        return;
      }
    }
    PHONEBOOK_TYPE *phone = new_phonebook();
    phone->number = number;
    free_string(phone->owner);
    phone->owner = str_dup(owner);
    PhoneVect.push_back(phone);
  }

  _DOFUN(do_imbue)
  {
    char arg1[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    argument = one_argument_nouncap(argument, arg1);

    if(ch->pcdata->last_imbue > (current_time - (3600*24*7)))
    {
      send_to_char("You have done that too recently.\n\r", ch);
      return;
    }
    if(get_tier(ch) < 2)
    {
      send_to_char("Your blood is not potent enough.\n\r", ch);
      return;
    }
    if(!is_demonborn(ch) && !is_angelborn(ch) && !is_faeborn(ch))
    {
      send_to_char("You don't have the right kind of blood for that.\n\r", ch);
      return;
    }
    char buf[MSL];
    if (arg1[0] == '\0') {
      send_to_char("Customize what?\n\r", ch);
      return;
    }
    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        continue;
      }

      if (is_name(arg1, obj->name)) {
        if(is_angelborn(ch))
        obj->faction = -10;
        else if(is_demonborn(ch))
        obj->faction = -11;
        else if(is_faeborn(ch))
        obj->faction = -12;
        sprintf(buf, "$n imbues %s with the power of $s blood", obj->short_descr);
        act(buf, ch, NULL, obj->short_descr, TO_CHAR);
        act(buf, ch, NULL, obj->short_descr, TO_ROOM);
        ch->pcdata->last_imbue = current_time;
        return;
      }
    }
  }

  bool has_imbuement(CHAR_DATA *ch, int number) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if(obj->faction == number)
        return TRUE;
      }
    }
    return FALSE;
  }



  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

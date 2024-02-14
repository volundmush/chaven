
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
#include <random>
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

  EXTRA_PATROL_TYPE* epatrol_by_id args( (int id) );
  EXTRA_PATROL_TYPE* random_epatrol args((void));
  void villain_timeout	args( (CHAR_DATA *pred, CHAR_DATA *prey) );
  void prey_timeout	args( (CHAR_DATA *prey, CHAR_DATA *pred) );
  void prey_escape	args( (CHAR_DATA *pred, CHAR_DATA *prey) );
  void prey_capture	args( (CHAR_DATA *pred, CHAR_DATA *prey) );
  void influencer_process args( (void) );


  bool free_to_act(CHAR_DATA *ch) {
    if (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || is_ghost(ch) || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room))
    return FALSE;
    if (clinic_patient(ch))
    return FALSE;
    if(ch->pcdata->summary_type > 0)
    return FALSE;
    return TRUE;
  }

  ROOM_INDEX_DATA *mist_duplicate_room(ROOM_INDEX_DATA *orig) {
    int j;
    for (j = get_area_data(DIST_MISTS)->min_vnum;
    !freeplayerroom(j) || room_pop(get_room_index(j)) > 0; j++) {
    }
    if (j > get_area_data(DIST_MISTS)->max_vnum) {
      return NULL;
    }
    ROOM_INDEX_DATA *to_room = get_room_index(j);

    to_room->room_flags = orig->room_flags;

    if (orig->sector_type == SECT_UNDERWATER)
    to_room->sector_type = SECT_WATER;
    else if (orig->sector_type == SECT_AIR || orig->sector_type == SECT_ATMOSPHERE)
    to_room->sector_type = SECT_STREET;
    else
    to_room->sector_type = orig->sector_type;
    to_room->timezone = 0;
    for (int door = 0; door < 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      int rev = rev_dir[door];
      if (to_room->exit[door] == NULL)
      continue;

      pToRoom = to_room->exit[door]->u1.to_room; /* ROM OLC */

      if (pToRoom != NULL) {
        if (pToRoom->exit[rev]) {
          free_exit(pToRoom->exit[rev]);
          pToRoom->exit[rev] = NULL;
        }
      }
      free_exit(to_room->exit[door]);
      to_room->exit[door] = NULL;
    }
    to_room->size = orig->size;
    to_room->x = orig->x;
    to_room->y = orig->y;
    to_room->z = orig->z;

    if (IS_SET(to_room->room_flags, ROOM_PUBLIC))
    REMOVE_BIT(to_room->room_flags, ROOM_PUBLIC);

    free_string(to_room->name);
    to_room->name = str_dup(orig->name);
    free_string(to_room->description);
    to_room->description = str_dup(orig->description);
    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ped;
    for (ed = to_room->extra_descr; ed; ed = ed->next) {
      ped = ed;
      if (!ed) {
      }
      else {
        if (!ped)
        to_room->extra_descr = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }
    ed = NULL;
    ped = NULL;
    for (ed = to_room->places; ed; ed = ed->next) {
      ped = ed;
      if (!ed) {
      }
      else {
        if (!ped)
        to_room->places = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }
    return to_room;
  }

  ROOM_INDEX_DATA *make_victim_room(CHAR_DATA *ch) {
    char buf[MSL];
    sprintf(buf, "STALK: Making victim room for %s, in room %d", ch->name, ch->in_room->vnum);
    log_string(buf);
    if (mist_level(ch->in_room) >= 3 || !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {

      log_string("STALK: Vuln room");
      ROOM_INDEX_DATA *room = mist_duplicate_room(ch->in_room);
      if (room != NULL) {
        sprintf(buf, "STALK: Dupe room: %d", room->vnum);
        log_string(buf);
      }
      for (int i = 0; i < 10; i++) {
        if (ch->in_room->exit[i] != NULL && ch->in_room->exit[i]->u1.to_room != NULL) {
          if (ch->in_room->exit[i]->u1.to_room->sector_type ==
              ch->in_room->sector_type) {
            sprintf(buf, "STALK: Making exit, %d to %d through %d", room->vnum, ch->in_room->exit[i]->u1.to_room->vnum, i);
            log_string(buf);
            make_exit(room->vnum, ch->in_room->exit[i]->u1.to_room->vnum, i, CONNECT_ONEWAY);
            return room;
          }
        }
      }
    }
    else {
      log_string("STALK: Unvuln room");
      ROOM_INDEX_DATA *orig;
      orig = get_room_index(16490);

      ROOM_INDEX_DATA *room = mist_duplicate_room(orig);
      sprintf(buf, "STALK: Orig: %d, dupe: %d", orig->vnum, room->vnum);
      log_string(buf);
      for (int i = 0; i < 10; i++) {
        if (orig->exit[i] != NULL && orig->exit[i]->u1.to_room != NULL) {
          if (orig->exit[i]->u1.to_room->sector_type == orig->sector_type) {
            make_exit(room->vnum, orig->exit[i]->u1.to_room->vnum, i, CONNECT_ONEWAY);
            sprintf(buf, "STALK: Making exit, %d to %d through %d", room->vnum, orig->exit[i]->u1.to_room->vnum, i);
            log_string(buf);

            return room;
          }
        }
      }
    }
    return NULL;
  }

  bool armored(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (IS_SET(obj->extra_flags, ITEM_ARMORED)) {
        if (obj->wear_loc != WEAR_NONE) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  int from_outfit(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    int values[20];
    int counts[20];
    for (int i = 0; i < 20; i++) {
      values[i] = 0;
      counts[i] = 0;
    }
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      for (int i = 0; i < 10; i++) {
        if (obj->outfit[i] > 0) {
          bool found = FALSE;
          for (int x = 0; x < 20; x++) {
            if (values[x] == obj->outfit[i]) {
              counts[x]++;
              x = 20;
              found = TRUE;
            }
          }
          if (found == FALSE) {
            for (int x = 0; x < 20; x++) {
              if (values[x] == 0) {
                values[x] = obj->outfit[i];
                counts[x]++;
                x = 20;
              }
            }
          }
        }
      }
    }
    int max = 0;
    int maxpoint = 0;
    for (int i = 0; i < 20; i++) {
      if (counts[i] > max) {
        max = counts[i];
        maxpoint = i;
      }
    }
    return values[maxpoint];
  }

  void autooutfit(CHAR_DATA *ch, int number) {
    int iWear;
    OBJ_DATA *obj;
    OBJ_DATA *newobj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (IS_SET(obj->extra_flags, ITEM_NOOUTFIT))
      continue;
      if (IS_SET(obj->extra_flags, ITEM_RELIC))
      continue;
      unequip_char_silent(ch, obj);
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE))
      SET_BIT(obj->extra_flags, ITEM_WARDROBE);
      if (obj->stash_room < 1)
      obj->stash_room = ch->in_room->vnum;
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj == NULL)
      continue;

      if (IS_SET(obj->extra_flags, ITEM_NOOUTFIT))
      continue;

      for (int i = 0; i < 10; i++) {
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
          }
        }
      }
    }
  }

  void autogear(CHAR_DATA *ch, bool dreaming) {
    bool armor = FALSE;
    bool smallmelee = FALSE;
    bool largemelee = FALSE;
    bool smallgun = FALSE;
    bool largegun = FALSE;

    if (dreaming == TRUE && ch->pcdata->dreamoutfit > 0 && !armored(ch)) {
      ch->pcdata->dreamfromoutfit = from_outfit(ch);
      autooutfit(ch, ch->pcdata->dreamoutfit);
    }
    else if (dreaming == TRUE)
    ch->pcdata->dreamfromoutfit = 0;

    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (IS_SET(obj->extra_flags, ITEM_ARMORED)) {
        if (obj->wear_loc != WEAR_NONE) {
          armor = TRUE;
        }
        else if (armor == FALSE) {
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          wear_obj(ch, obj, TRUE, TRUE);
          armor = TRUE;
        }
      }
      if (obj->item_type == ITEM_RANGED && obj->size < 25) {
        smallgun = TRUE;
      }
      if (obj->item_type == ITEM_RANGED && obj->size >= 25 && largegun == FALSE) {
        if (obj->in_obj != NULL)
        obj_from_obj(obj);
        largegun = TRUE;
      }
      if (obj->item_type == ITEM_WEAPON && obj->size < 25) {
        smallmelee = TRUE;
      }
      if (obj->item_type == ITEM_WEAPON && obj->size >= 25 && largemelee == FALSE) {
        if (obj->in_obj != NULL)
        obj_from_obj(obj);
        largemelee = TRUE;
      }
    }
    int radius;
    if (ch->pcdata->patrol_habits[PATROL_WARHABIT] > 0)
    radius = 12;
    else
    radius = 6;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (IS_SET(obj->extra_flags, ITEM_ARMORED) && armor == FALSE) {
        ROOM_INDEX_DATA *room = get_room_index(obj->stash_room);
        if (room == NULL) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          wear_obj(ch, obj, TRUE, TRUE);
        }
        else if ((room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY)) {
          ROOM_INDEX_DATA *driven = get_room_index(ch->pcdata->last_drove);
          if (driven == NULL || (get_dist(room->x, room->y, driven->x, driven->y) <= radius || dreaming)) {
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            if (obj->in_obj != NULL)
            obj_from_obj(obj);
            wear_obj(ch, obj, TRUE, TRUE);
            armor = TRUE;
          }
        }
        else if (get_dist(room->x, room->y, ch->in_room->x, ch->in_room->y) <=
            radius || dreaming) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          wear_obj(ch, obj, TRUE, TRUE);
          armor = TRUE;
        }
      }
      if (obj->item_type == ITEM_RANGED && obj->size < 25 && smallgun == FALSE) {
        ROOM_INDEX_DATA *room = get_room_index(obj->stash_room);
        if (room == NULL) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          smallgun = TRUE;
        }
        else if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY) {
          ROOM_INDEX_DATA *driven = get_room_index(ch->pcdata->last_drove);
          if (driven == NULL || (get_dist(room->x, room->y, driven->x, driven->y) <= radius || dreaming)) {
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            if (obj->in_obj != NULL)
            obj_from_obj(obj);
            smallgun = TRUE;
          }
        }
        else if (get_dist(room->x, room->y, ch->in_room->x, ch->in_room->y) <=
            radius || dreaming) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          smallgun = TRUE;
        }
      }
      if (obj->item_type == ITEM_RANGED && obj->size >= 25 && largegun == FALSE) {
        ROOM_INDEX_DATA *room = get_room_index(obj->stash_room);
        if (room == NULL) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          largegun = TRUE;
        }
        else if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY) {
          ROOM_INDEX_DATA *driven = get_room_index(ch->pcdata->last_drove);
          if (driven == NULL || (get_dist(room->x, room->y, driven->x, driven->y) <= radius || dreaming)) {
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            if (obj->in_obj != NULL)
            obj_from_obj(obj);
            largegun = TRUE;
          }
        }
        else if (get_dist(room->x, room->y, ch->in_room->x, ch->in_room->y) <=
            radius || dreaming) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          largegun = TRUE;
        }
      }
      if (obj->item_type == ITEM_WEAPON && obj->size < 25 && smallmelee == FALSE) {
        ROOM_INDEX_DATA *room = get_room_index(obj->stash_room);
        if (room == NULL) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          smallmelee = TRUE;
        }
        else if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY) {
          ROOM_INDEX_DATA *driven = get_room_index(ch->pcdata->last_drove);
          if (driven == NULL || (get_dist(room->x, room->y, driven->x, driven->y) <= radius || dreaming)) {
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            if (obj->in_obj != NULL)
            obj_from_obj(obj);
            smallmelee = TRUE;
          }
        }
        else if (get_dist(room->x, room->y, ch->in_room->x, ch->in_room->y) <=
            radius || dreaming) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          smallmelee = TRUE;
        }
      }
      if (obj->item_type == ITEM_WEAPON && obj->size >= 25 && largemelee == FALSE) {
        ROOM_INDEX_DATA *room = get_room_index(obj->stash_room);
        if (room == NULL) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          largemelee = TRUE;
        }
        else if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY) {
          ROOM_INDEX_DATA *driven = get_room_index(ch->pcdata->last_drove);
          if ((driven == NULL || (get_dist(room->x, room->y, driven->x, driven->y) <= radius) || dreaming)) {
            REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
            if (obj->in_obj != NULL)
            obj_from_obj(obj);
            largemelee = TRUE;
          }
        }
        else if (get_dist(room->x, room->y, ch->in_room->x, ch->in_room->y) <=
            radius || dreaming) {
          REMOVE_BIT(obj->extra_flags, ITEM_WARDROBE);
          if (obj->in_obj != NULL)
          obj_from_obj(obj);
          largemelee = TRUE;
        }
      }
    }
    ch->hit = max_hp(ch);
  }

  bool patrol_attackable(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_STASIS) || higher_power(ch))
    return FALSE;

    char buf[MSL];
    if (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || is_ghost(ch) || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room)) {
      sprintf(buf, "PATROLWAR: %s inelligble because fight/helpless/pinned/ghost/locked/outoftown/hostile.", ch->name);
      log_string(buf);
      return FALSE;
    }
    if (ch->wounds > 0) {
      sprintf(buf, "PATROLWAR: %s inelligble because wounded.", ch->name);
      log_string(buf);
      return FALSE;
    }
    if (institute_room(ch->in_room))
    return FALSE;
    if (!is_covered(ch, COVERS_GROIN)) {
      sprintf(buf, "PATROLWAR: %s inelligble because naked.", ch->name);
      log_string(buf);
      return FALSE;
    }

    if (ch->in_room->sector_type == SECT_AIR) {
      sprintf(buf, "PATROLWAR: %s inelligble because in the air.", ch->name);
      log_string(buf);
      return FALSE;
    }
    if (deep_water(ch) == TRUE) {
      sprintf(buf, "PATROLWAR: %s inelligble because in the water.", ch->name);
      log_string(buf);
      return FALSE;
    }
    if (IS_FLAG(ch->comm, COMM_AFK)) {
      sprintf(buf, "PATROLWAR: %s inelligble because AFK.", ch->name);
      log_string(buf);
      return FALSE;
    }
    if (ch->pcdata->availability == AVAIL_LOW) {
      sprintf(buf, "PATROLWAR: %s inelligble because unavailable.", ch->name);
      log_string(buf);
      return FALSE;
    }

    if (in_lodge(ch->in_room)) {
      sprintf(buf, "PATROLWAR: %s inelligble because in the lodge.", ch->name);
      log_string(buf);
      return FALSE;
    }
    if (in_sheriff(ch->in_room)) {
      sprintf(buf, "PATROLWAR: %s inelligble because in the prison.", ch->name);
      log_string(buf);
      return FALSE;
    }
    PROP_TYPE *prop;
    if ((prop = prop_from_room(ch->in_room)) == NULL) {
      if (ch->in_room->z < 0) {
        sprintf(buf, "PATROLWAR: %s inelligble because underground.", ch->name);
        log_string(buf);
        return FALSE;
      }

    }
    else {
      if (prop->warded > 50) {
        sprintf(buf, "PATROLWAR: %s inelligble because warded.", ch->name);
        log_string(buf);
        return FALSE;
      }
      if (prop->shroudshield >= 50 && prop->type == PROP_SHOP) {
        sprintf(buf, "PATROLWAR: %s inelligble because shroud shielded.", ch->name);
        log_string(buf);
        return FALSE;
      }
    }
    if (pc_pop(ch->in_room) > 4) {
      sprintf(buf, "PATROLWAR: %s inelligble because in populated area.", ch->name);
      log_string(buf);
      return FALSE;
    }

    return TRUE;
  }

  bool valid_syndicate_prey(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_STASIS) || higher_power(ch))
    return FALSE;

    if (newbie_level(ch) < 3)
    return FALSE;
    if (get_tier(ch) > 3)
    return FALSE;
    if (pc_pop(ch->in_room) > 1)
    return FALSE;
    if (public_room(ch->in_room) && ch->in_room->sector_type != SECT_STREET)
    return FALSE;
    if (get_tier(ch) < 2 && ch->faction == 0)
    return FALSE;
    if (institute_room(ch->in_room))
    return FALSE;
    if (has_weakness(NULL, ch))
    return FALSE;
    if (ch->pcdata->account != NULL && ch->pcdata->account->prey_cool_s > current_time)
    return FALSE;
    if (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || is_ghost(ch) || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room))
    return FALSE;
    if (ch->in_room->sector_type == SECT_AIR)
    return FALSE;
    if (IS_FLAG(ch->comm, COMM_AFK))
    return FALSE;
    if (deep_water(ch) == TRUE)
    return FALSE;
    PROP_TYPE *prop;
    if ((prop = prop_from_room(ch->in_room)) == NULL) {
      if (ch->in_room->z < 0)
      return FALSE;
    }
    else {
      if (prop->warded > 50)
      return FALSE;
    }
    return TRUE;
  }

  int syndicate_prey_score(CHAR_DATA *victim) {
    int score = 1000;
    if (IS_SET(victim->in_room->room_flags, ROOM_INDOORS))
    score = score * 2 / 3;
    if (get_tier(victim) == 2)
    score *= 2;
    if (victim->faction == 0)
    score /= 2;
    return score;
  }

  bool arcane_attackable(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_STASIS) || higher_power(ch))
    return FALSE;

    if (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || is_ghost(ch) || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room))
    return FALSE;
    if (IS_FLAG(ch->comm, COMM_AFK))
    return FALSE;
    if (ch->pcdata->availability == AVAIL_LOW)
    return FALSE;
    PROP_TYPE *prop;
    if ((prop = prop_from_room(ch->in_room)) == NULL) {
      if (ch->in_room->z < 0)
      return FALSE;
    }
    else {
      if (prop->warded > 50)
      return FALSE;
      if (prop->shroudshield >= 50)
      return FALSE;
    }
    if (pc_pop(ch->in_room) > 5)
    return FALSE;
    return TRUE;
  }

  ROOM_INDEX_DATA *patrol_attack_room(CHAR_DATA *victim) {
    if (IS_SET(victim->in_room->room_flags, ROOM_INDOORS))
    return victim->in_room;
    else {
      ROOM_INDEX_DATA *orig = victim->in_room;
      ROOM_INDEX_DATA *room;
      for (int d = 3; d > 0; d--) {
        for (int i = 0; i < 10; i++) {
          room = orig;
          bool nogood = FALSE;
          for (int j = 0; j < d; j++) {
            if (open_sound(room, i))
            room = room->exit[i]->u1.to_room;
            else
            nogood = TRUE;
          }
          if (nogood == FALSE)
          return room;
        }
      }
    }
    return victim->in_room;
  }

  bool same_faction(CHAR_DATA *ch, CHAR_DATA *victim) {

    if(ch->fcore != 0 and ch->fcore == victim->fcore)
    return TRUE;
    if(ch->fsect != 0 and ch->fsect == victim->fsect)
    return TRUE;
    if(ch->fcult != 0 and ch->fcult == victim->fcult)
    return TRUE;

    if(ch->fcore != 0 && clan_lookup(ch->fcore) != NULL && victim->fcore != 0 && clan_lookup(victim->fcore) != NULL && clan_lookup(ch->fcore)->alliance == clan_lookup(victim->fcore)->alliance && clan_lookup(ch->fcore)->alliance != 0)
    return TRUE;

    if(ch->fsect != 0 && clan_lookup(ch->fsect) != NULL && victim->fsect != 0 && clan_lookup(victim->fsect) != NULL && clan_lookup(ch->fsect)->alliance == clan_lookup(victim->fsect)->alliance && clan_lookup(ch->fsect)->alliance != 0)
    return TRUE;

    if(ch->fcult != 0 && clan_lookup(ch->fcult) != NULL && victim->fcult != 0 && clan_lookup(victim->fcult) != NULL && clan_lookup(ch->fcult)->alliance == clan_lookup(victim->fcult)->alliance && clan_lookup(ch->fcult)->alliance != 0)
    return TRUE;


    return FALSE;
  }

  int combatant_score(CHAR_DATA *ch) {
    int val = get_tier(ch) * 2;
    val += combat_focus(ch) * 2;
    val += 5;
    val += 5 * ch->pcdata->guard_number;

    if (ch->pcdata->patrol_habits[PATROL_WARHABIT] == 0 && combat_focus(ch) < 1)
    val /= 2;

    return val;
  }

  int arcanist_score(CHAR_DATA *ch) {
    int val = get_tier(ch) * 10;
    val += arcane_focus(ch) * 10;
    if (ch->race == RACE_GIFTED || ch->race == RACE_VETGIFTED)
    val += 5;
    val += get_skill(ch, SKILL_ALCHEMY);
    val += get_skill(ch, SKILL_RITUALISM);
    val += get_skill(ch, SKILL_DEMONOLOGY);
    val += get_skill(ch, SKILL_INCANTATION);
    val += get_skill(ch, SKILL_MINIONS);
    val += get_skill(ch, SKILL_ELEMINIONS);
    val = val * get_lifeforce(ch, TRUE, NULL) / 100;

    if (ch->pcdata->patrol_habits[PATROL_ARCANEWARHABIT] == 0 && arcane_focus(ch) < 1)
    val /= 2;

    return val;
  }

  bool is_patrol_enemy(CHAR_DATA *ch, CHAR_DATA *victim) {
    if(same_faction(ch, victim))
    return FALSE;
    return TRUE;
    FACTION_TYPE *cfac = clan_lookup(ch->faction);
    if (cfac != NULL) {
      FACTION_TYPE *vfac = clan_lookup(victim->faction);
      if (vfac != NULL) {
        if (((cfac->alliance <= 4) == (vfac->alliance <= 4)) && cfac->alliance != vfac->alliance && cfac->alliance != 0 && vfac->alliance != 0)
        return TRUE;
      }
      vfac = clan_lookup(victim->factiontwo);
      if (vfac != NULL) {
        if (((cfac->alliance <= 4) == (vfac->alliance <= 4)) && cfac->alliance != vfac->alliance && cfac->alliance != 0 && vfac->alliance != 0)
        return TRUE;
      }
    }
    cfac = clan_lookup(ch->factiontwo);
    if (cfac != NULL) {
      FACTION_TYPE *vfac = clan_lookup(victim->faction);
      if (vfac != NULL) {
        if (((cfac->alliance <= 4) == (vfac->alliance <= 4)) && cfac->alliance != vfac->alliance && vfac->alliance != 0 && cfac->alliance != 0)
        return TRUE;
      }
      vfac = clan_lookup(victim->factiontwo);
      if (vfac != NULL) {
        if (((cfac->alliance <= 4) == (vfac->alliance <= 4)) && cfac->alliance != vfac->alliance && vfac->alliance != 0 && cfac->alliance != 0)
        return TRUE;
      }
    }
    return FALSE;
  }

  int initial_balance(CHAR_DATA *ch, CHAR_DATA *victim) {
    int charpower = 0;
    int vicpower = 0;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (same_faction(ch, fch))
      charpower += combatant_score(fch);
    }
    for (CharList::iterator it = victim->in_room->people->begin();
    it != victim->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (same_faction(victim, fch))
      vicpower += combatant_score(fch);
    }
    int val = charpower * 100 / UMAX(vicpower, 1);
    return val;
  }

  int team_balance(CHAR_DATA *ch, CHAR_DATA *victim) {
    int charpower = 0;
    int vicpower = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
      continue;
      if (!free_to_act(to))
      continue;
      if (!same_faction(ch, to))
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      int score = combatant_score(to);
      if (IS_FLAG(to->comm, COMM_AFK) && to != ch)
      score = score * 2 / 3;
      if (to->pcdata->availability == AVAIL_LOW && to != ch)
      score = score * 2 / 3;
      if (IS_FLAG(to->comm, COMM_PRIVATE) && to != ch)
      score = score * 2 / 3;

      charpower += score;
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
      continue;
      if (!free_to_act(to))
      continue;
      if (!same_faction(victim, to))
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      int score = combatant_score(to);
      if (IS_FLAG(to->comm, COMM_AFK))
      score = score * 2 / 3;
      if (to->pcdata->availability == AVAIL_LOW && to != ch)
      score = score * 2 / 3;
      if (IS_FLAG(to->comm, COMM_PRIVATE) && to != victim)
      score = score * 2 / 3;

      vicpower += score;
    }
    int val = charpower * 100 / UMAX(vicpower, 1);
    return val;
  }

  int pc_prey_score(CHAR_DATA *ch, CHAR_DATA *pred)
  {
    int score = solidity(ch) + get_attract(ch, pred);

    if(get_tier(ch) >= 2 && is_virgin(ch))
    score *= 2;

    if (ch->pcdata->patrol_habits[PATROL_RECKLESSHABIT] > 0 || is_weakness(pred, ch))
    score *= 3;

    score *= narrative_score(ch, pred);

    if (pred->pcdata->habit[HABIT_ORIENTATION] == 0 && ch->sex == pred->sex)
    score /= 10;
    else if (pred->pcdata->habit[HABIT_ORIENTATION] == 1 && ch->sex != pred->sex)
    score /= 10;

    if (ch->pcdata->habit[HABIT_ORIENTATION] == 0 && ch->sex == pred->sex)
    score /= 10;
    else if (ch->pcdata->habit[HABIT_ORIENTATION] == 1 && ch->sex != pred->sex)
    score /= 10;

    return score;
  }


  bool valid_pc_prey(CHAR_DATA *ch, CHAR_DATA *pred, bool isauto) {

    if (IS_FLAG(ch->act, PLR_STASIS) || higher_power(ch))
    return FALSE;

    if(strlen(pred->pcdata->fixation_name) > 2 && pred->pcdata->fixation_timeout > current_time)
    return FALSE;


    bool nowhere = FALSE;
    for (int i = 0; i < 50; i++) { // nowhere characters
      if (!str_cmp(pred->pcdata->account->name, ch->pcdata->nowhere_accounts[i])) {
        nowhere = TRUE;
        i = 50;
      }
      if (nowhere == FALSE) {
        for (int n = 0; n < 25; n++) {
          if (safe_strlen(pred->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(pred->pcdata->account->characters[n], ch->pcdata->nowhere_accounts[i])) {
              nowhere = TRUE;
              i = 50;
              n = 25;
            }
          }
        }
      }
    }
    if (nowhere == TRUE) {
      return FALSE;
    }
    nowhere = FALSE;
    for (int i = 0; i < 50; i++) { // nowhere characters
      if (!str_cmp(ch->pcdata->account->name, pred->pcdata->nowhere_accounts[i])) {
        nowhere = TRUE;
        i = 50;
      }
      if (nowhere == FALSE) {
        for (int n = 0; n < 25; n++) {
          if (safe_strlen(ch->pcdata->account->characters[n]) > 2) {
            if (!str_cmp(ch->pcdata->account->characters[n], pred->pcdata->nowhere_accounts[i])) {
              nowhere = TRUE;
              i = 50;
              n = 25;
            }
          }
        }
      }
    }
    if (nowhere == TRUE) {
      return FALSE;
    }


    if (IS_NPC(ch) || ch->in_room == NULL || !in_haven(ch->in_room))
    return FALSE;
    if (ch->pcdata->patrol_status != 0 &&  ch->pcdata->patrol_status != PATROL_PATROL)
    return FALSE;

    if(ch->pcdata->victimize_difficult_time > current_time)
    return FALSE;

    if(in_lodge(ch->in_room))
    return FALSE;

    if(newbie_level(ch) < 3)
    return FALSE;

    if(!in_haven(ch->in_room))
    return FALSE;

    if(!free_to_act(ch))
    return FALSE;

    if(event_aegis == 1 && is_super(ch) != is_super(pred))
    return FALSE;

    if(has_weakness(pred, ch))
    return FALSE;


    if(ch->pcdata->account != NULL && pred->pcdata->account != NULL && !str_cmp(ch->pcdata->account->name, pred->pcdata->account->name))
    return FALSE;
    if(get_tier(ch) > get_tier(pred) && mist_level(ch->in_room) < 3)
    return FALSE;
    if(get_tier(ch) > 3)
    return FALSE;
    if(get_tier(pred) < 2)
    return FALSE;

    if(pc_pop(ch->in_room) > 3)
    return FALSE;
    if(pc_pop(ch->in_room) > 1 && mist_level(ch->in_room) < 3)
    return FALSE;

    if (IS_FLAG(ch->comm, COMM_AFK))
    return FALSE;
    if (is_gm(ch) || is_helpless(ch) || in_fight(ch) || IS_FLAG(ch->act, PLR_DEAD) || ch->in_room == NULL || locked_room(ch->in_room, ch) || room_hostile(ch->in_room) || is_pinned(ch))
    return FALSE;

    if (ch->race == RACE_FACULTY)
    return FALSE;
    if (in_fight(ch) || is_helpless(ch) || is_pinned(ch) || is_ghost(ch) || locked_room(ch->in_room, ch) || room_hostile(ch->in_room))
    return FALSE;
    if (get_gm(ch->in_room, FALSE) != NULL)
    return FALSE;
    if (ch->in_room->sector_type == SECT_AIR)
    return FALSE;
    if (ch->pcdata->availability == AVAIL_LOW)
    return FALSE;

    if(IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && mist_level(ch->in_room) < 1)
    return FALSE;


    PROP_TYPE * prop = in_prop(ch);
    if(prop != NULL && prop->warded > 50)
    return FALSE;

    if (isauto == FALSE)
    return TRUE;

    if(pc_prey_score(ch, pred) < 10)
    return FALSE;
    if(get_tier(ch) == get_tier(pred) && good_person(ch) == good_person(pred))
    return FALSE;

    if (ch->pcdata->account == NULL || ch->pcdata->account->prey_cool > current_time)
    return FALSE;


    if(ch->pcdata->victimize_difficult_time > current_time - (3600*24*4))
    return FALSE;

    if(norp_match(ch, pred))
    return FALSE;

    if(are_noncult_allies(ch, pred))
    return FALSE;
    if (!is_covered(ch, COVERS_GROIN))
    return FALSE;
    if (ch->pcdata->account == NULL || ch->pcdata->account->prey_cool > current_time)
    return FALSE;

    if (ch->pcdata->patrol_habits[PATROL_RECKLESSHABIT] > 0 || is_weakness(pred, ch))
    return TRUE;

    if(public_room(ch->in_room))
    return FALSE;

    return TRUE;
  }




  int arcane_team_balance(CHAR_DATA *ch, CHAR_DATA *victim) {
    int charpower = 0;
    int vicpower = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
      continue;
      if (!free_to_act(to))
      continue;
      if (!same_faction(ch, to))
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      int power = arcanist_score(to);
      if (IS_FLAG(to->comm, COMM_AFK))
      power = power * 2 / 3;
      if (to->pcdata->availability == AVAIL_LOW && to != ch)
      power = power * 2 / 3;

      charpower += power;
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
      continue;
      if (!free_to_act(to))
      continue;
      if (!same_faction(victim, to))
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      int power = arcanist_score(to);
      if (IS_FLAG(to->comm, COMM_AFK))
      power = power * 2 / 3;
      if (to->pcdata->availability == AVAIL_LOW && to != ch)
      power = power * 2 / 3;

      vicpower += power;
    }
    int val = charpower * 100 / UMAX(vicpower, 1);
    return val;
  }

  // Factors, sex, being in a base, being alone, patrol status, power balance, // being outdoors
  int patrol_prey_score(CHAR_DATA *ch, CHAR_DATA *victim) {
    int score = 1000;
    score -= get_dist(ch->in_room->x, ch->in_room->y, victim->in_room->x, victim->in_room->y) *
    5;

    if (pc_pop(victim->in_room) > 1 && IS_SET(victim->in_room->room_flags, ROOM_INDOORS))
    score /= 2;
    if (IS_FLAG(victim->comm, COMM_PRIVATE))
    score /= 2;
    if (IS_SET(victim->in_room->room_flags, ROOM_INDOORS))
    score = score * 2 / 3;
    if (victim->pcdata->patrol_habits[PATROL_WARHABIT] > 0)
    score *= 3;
    if (victim->pcdata->patrol_status == PATROL_PATROL)
    score *= 3;
    if (!can_shroud(victim))
    score /= 2;
    if (is_weakness(ch, victim))
    score *= 5;
    if (victim->pcdata->patrol_status == AVAIL_HIGH)
    score *= 2;


    char buf[MSL];
    sprintf(buf, "PATROLWAR: Ch: %s, Vic: %s, Score: %d, Bal: %d", ch->name, victim->name, score, team_balance(ch, victim));
    log_string(buf);

    int balance = team_balance(ch, victim);
    if (balance >= 250 && !is_weakness(ch, victim))
    return 100;
    if (balance <= 30)
    return 100;

    int diff = 0;
    if (balance > 100)
    diff = balance - 100;
    else
    diff = 100 - balance;
    // diff 0-100 with 0 being closer to balanced.

    int ndiff = 100 - diff;
    score = score * ndiff / 100;

    return score;
  }

  int arcane_prey_score(CHAR_DATA *ch, CHAR_DATA *victim) {
    int score = 1000;

    if (pc_pop(victim->in_room) > 1 && IS_SET(victim->in_room->room_flags, ROOM_INDOORS))
    score /= 2;
    if (IS_FLAG(victim->comm, COMM_PRIVATE))
    score /= 2;
    if (IS_SET(victim->in_room->room_flags, ROOM_INDOORS))
    score = score * 2 / 3;
    if (victim->pcdata->patrol_habits[PATROL_ARCANEHABIT] > 0)
    score *= 2;
    if (victim->pcdata->patrol_status == PATROL_PATROL)
    score *= 3;
    if (public_room(victim->in_room))
    score /= 2;
    if (is_weakness(ch, victim))
    score *= 5;
    if (victim->pcdata->availability == AVAIL_HIGH)
    score *= 2;

    char buf[MSL];
    sprintf(buf, "PATROLWARARCANE: Ch: %s, Vic: %s, Score: %d, Bal: %d", ch->name, victim->name, score, arcane_team_balance(ch, victim));
    log_string(buf);

    int balance = arcane_team_balance(ch, victim);
    if (balance >= 250 && !is_weakness(ch, victim))
    return 100;

    int diff = 0;
    if (balance > 100)
    diff = balance - 100;
    else
    diff = 100 - balance;
    // diff 0-100 with 0 being closer to balanced.

    int ndiff = 100 - diff;
    score = score * ndiff / 100;

    return score;
  }

  char *get_arti_material(void) {
    switch (number_percent() % 4) {
    case 3:
      return "golden";
      break;
    case 2:
      return "silver";
      break;
    case 1:
      return "wooden";
      break;
    case 0:
    default:
      return "bone";
      break;
    }
  }

  char *get_arti_type(void) {
    switch (number_percent() % 4) {
    case 3:
      return "dagger";
      break;
    case 2:
      return "necklace";
      break;
    case 1:
      return "ring";
      break;
    case 0:
    default:
      return "bracelet";
      break;
    }
  }

  ROOM_INDEX_DATA *graveyard_room(void) {
    for (int i = 0; i < 20; i++) {
      ROOM_INDEX_DATA *room =
      room_by_coordinates(number_range(56, 67), number_range(1, 23), 0);
      if (room->sector_type == SECT_CEMETARY)
      return room;
    }
    return get_room_index(10174);
  }

  void bribe_message(CHAR_DATA *ch, int npc, int est, int amount) {
    if (est == 1) {
      if (npc == 1)
      printf_to_char(
      ch, "You get a report on your phone stating that a psychiatrist needs to be paid off to back up the society's claims of a witness being insane, they can be found outside the institute. Your contact estimates they'll need to be paid somewhere around $%d. (give (amount) mob)\n\r", number_range(amount / 2, amount * 3 / 2));
      if (npc == 2)
      printf_to_char(
      ch, "You get a report on your phone stating that a medical examiner needs to be paid off to falsify an autopsy, they can be found outside the institute. Your contact estimates they'll need to be paid somewhere around $%d. (give (amount) mob)\n\r", number_range(amount / 2, amount * 3 / 2));
      if (npc == 3)
      printf_to_char(
      ch, "You get a report on your phone stating that an HSD deputy needs to be paid off to falsify a police report, they can be found outside Town Hall. Your contact estimates they'll need to be paid somewhere around $%d. (give (amount) mob)\n\r", number_range(amount / 2, amount * 3 / 2));
    }
    else {
      if (npc == 1)
      printf_to_char(
      ch, "You get a report on your phone stating that a psychiatrist needs to be paid off to back up the society's claims of a witness being insane, they can be found outside the institute. Your contact estimates they'll need to be paid at most $%d. (give (amount) mob)\n\r", number_range(amount, amount * 2));
      if (npc == 2)
      printf_to_char(
      ch, "You get a report on your phone stating that a medical examiner needs to be paid off to falsify an autopsy, they can be found outside the institute. Your contact estimates they'll need to be paid at most $%d. (give (amount) mob)\n\r", number_range(amount, amount * 2));
      if (npc == 3)
      printf_to_char(
      ch, "You get a report on your phone stating that an HSD deputy needs to be paid off to falsify a police report, they can be found outside Town Hall. Your contact estimates they'll need to be paid at most $%d. (give (amount) mob)\n\r", number_range(amount, amount * 2));
    }
  }

  void launch_patrolevent(CHAR_DATA *ch, int type) {
    char buf[MSL];
    if (event_cleanse == 1)
    return;

    if (IS_FLAG(ch->act, PLR_STASIS) || higher_power(ch))
    return;

    if (type == PATROL_WAR) {
      if (ch == NULL || ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->alliance == 0)
      return;
      int maxscore = 200;
      if (ch->pcdata->account != NULL && ch->pcdata->account->last_war != 0 && ch->pcdata->account->last_war > current_time - (3600 * 24 * 10))
      return;
      CHAR_DATA *vic = NULL;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (!free_to_act(to))
        continue;
        if (!patrol_attackable(to))
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        if (IS_FLAG(to->comm, COMM_AFK))
        continue;
        if (to->pcdata->availability == AVAIL_LOW)
        continue;

        if (event_aegis == 1 && is_super(ch) != is_super(to))
        continue;
        if (!is_patrol_enemy(ch, to))
        continue;
        sprintf(buf, "PATROLWAR: Init Bal: Ch: %s, Vic: %s, Bal: %d", ch->name, to->name, initial_balance(ch, to));
        log_string(buf);
        if (initial_balance(ch, to) < 80)
        continue;
        int score = patrol_prey_score(ch, to);
        if (score > maxscore) {
          maxscore = score;
          vic = to;
        }
      }

      if (maxscore > 200 && vic != NULL) {
        if (ch->pcdata->account != NULL)
        ch->pcdata->account->last_war = current_time;
        if (vic->your_car != NULL)
        force_park(vic->your_car);
        if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDWAR))
        SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDWAR);

        act("$n falls asleep.", ch, NULL, NULL, TO_ROOM);
        dact("$n falls asleep.", ch, NULL, NULL, DISTANCE_MEDIUM);
        printf_to_char(ch, "You fall asleep.\n\r");
        act("$n falls asleep.", vic, NULL, NULL, TO_ROOM);
        dact("$n falls asleep.", vic, NULL, NULL, DISTANCE_MEDIUM);
        send_to_char("You feel yourself being pulled into the nightmare by an unseen force. (You can use patrol attack to start the fight early.)\n\r", vic);
        to_spectre(vic, TRUE);
        if (prop_from_room(vic->in_room) != NULL && prop_from_room(vic->in_room)->shroudshield >= 50) {
          ROOM_INDEX_DATA *desti =
          get_room_index(prop_from_room(vic->in_room)->roadroom);
          char_from_room(vic);
          char_to_room(vic, desti);
          vic->walking = 0;
        }
        if (!IS_FLAG(vic->act, PLR_SHROUD))
        SET_FLAG(vic->act, PLR_SHROUD);
        if (!IS_FLAG(vic->act, PLR_DEEPSHROUD))
        SET_FLAG(vic->act, PLR_DEEPSHROUD);
        vic->pcdata->patrol_status = PATROL_DEFENDHIDING;
        vic->pcdata->patrol_target = ch;
        vic->pcdata->patrol_timer = 10;
        vic->pcdata->patrol_room = vic->in_room;
        ROOM_INDEX_DATA *atroom = patrol_attack_room(vic);
        if (ch->your_car != NULL)
        force_park(ch->your_car);
        if (!IS_FLAG(ch->act, PLR_SHROUD))
        SET_FLAG(ch->act, PLR_SHROUD);
        ch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
        ch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
        villain_mod(ch, 20, "Patrol Attack");

        ch->pcdata->patrol_did_war = TRUE;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->master == ch && fch->position > POS_SITTING && !in_fight(fch) && !is_helpless(fch) && can_shroud(fch)) {
            act("You fall asleep.", fch, NULL, ch, TO_CHAR);
            to_spectre(fch, TRUE);
            char_from_room(fch);
            char_to_room(fch, atroom);
            fch->walking = 0;
            if (!IS_FLAG(fch->act, PLR_SHROUD))
            SET_FLAG(fch->act, PLR_SHROUD);
            if (!IS_FLAG(fch->act, PLR_DEEPSHROUD))
            SET_FLAG(fch->act, PLR_DEEPSHROUD);
            fch->pcdata->patrol_status = PATROL_ATTACKASSISTING;
            fch->pcdata->patrol_target = vic;
            fch->pcdata->patrol_room = atroom;
            fch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
            fch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
            villain_mod(fch, 20, "Assisting patrol attack");
          }
        }
        to_spectre(ch, TRUE);
        char_from_room(ch);
        char_to_room(ch, atroom);
        ch->walking = 0;
        if (!IS_FLAG(ch->act, PLR_DEEPSHROUD))
        SET_FLAG(ch->act, PLR_DEEPSHROUD);
        ch->pcdata->patrol_status = PATROL_ATTACKSEARCHING;
        ch->pcdata->patrol_target = vic;
        ch->pcdata->patrol_timer = 10;
        ch->pcdata->patrol_room = atroom;

        if (pc_pop(ch->in_room) == 2) {
          if (narrative_pair(ch, vic)) {
            printf_to_char(ch, "%s", narrative_pair_rewards(ch, vic));
            printf_to_char(vic, "%s", narrative_pair_rewards(ch, vic));
          }
        }

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room))
          continue;
          if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
          continue;
          if (!free_to_act(to))
          continue;
          if (get_gm(to->in_room, FALSE) != NULL)
          continue;
          if (to->pcdata->patrol_habits[PATROL_WARHABIT] > 0 || (to->faction != 0 && combat_focus(to) > 0)) {
            if (same_faction(to, ch) && !same_faction(to, vic)) {
              printf_to_char(to, "You receive a report about an attack going on at %s. Use `gpatrol engage`x to join the fight.\n\r", roomtitle(vic->in_room, FALSE));
              to->pcdata->patrol_status = PATROL_WARMOVINGATTACK;
              to->pcdata->patrol_timer = 3;
              to->pcdata->patrol_room = atroom;
            }
            else if (same_faction(to, vic) && !same_faction(to, ch)) {
              printf_to_char(to, "You receive a report about an attack going on at %s. Use `gpatrol engage`x to join the fight.\n\r", roomtitle(vic->in_room, FALSE));
              to->pcdata->patrol_status = PATROL_WARMOVINGDEFEND;
              to->pcdata->patrol_timer = 3;
              to->pcdata->patrol_room = vic->in_room;
            }
          }
        }

        return;
      }
    }
    if (type == PATROL_PREDATOR) {
      if (ch->pcdata->account != NULL && ch->pcdata->account->last_pred != 0 && ch->pcdata->account->last_pred > current_time - (3600 * 24 * 4))
      return;
      int maxscore = 0;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;

        if(!valid_pc_prey(to, ch, TRUE))
        continue;

        if(pc_prey_score(to, ch) > maxscore)
        maxscore = pc_prey_score(to, ch);
      }

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;

        if(!valid_pc_prey(to, ch, TRUE))
        continue;

        if(number_range(1, maxscore) < pc_prey_score(to, ch))
        {
          if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD)) {
            REMOVE_FLAG(ch->act, PLR_SHROUD);
            printf_to_char(
            ch, "\n\n\nYou spot %s somewhere vulnerable, use `gpatrol stalk (message)`x to hunt them down, use `gpatrol observe`x to look at them.\n\r", PERS(to, ch));
            SET_FLAG(ch->act, PLR_SHROUD);
          }
          else if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(to->act, PLR_SHROUD)) {
            SET_FLAG(ch->act, PLR_SHROUD);
            printf_to_char(
            ch, "\n\n\nYou spot %s somewhere vulnerable, use `gpatrol stalk (message)`x to hunt them down, use `gpatrol observe`x to look at them.\n\r", PERS(to, ch));
            REMOVE_FLAG(ch->act, PLR_SHROUD);
          }
          else
          printf_to_char(
          ch, "\n\n\nYou spot %s somewhere vulnerable, use `gpatrol stalk (message)`x to hunt them down, use `gpatrol observe`x to look at them.\n\r", PERS(to, ch));
          ch->pcdata->patrol_target = to;
          ch->pcdata->patrol_status = PATROL_STALKING;
          ch->pcdata->patrol_timer = 5;
          WAIT_STATE(to, PULSE_PER_SECOND * 10);
          if (to->pcdata->account != NULL && mist_level(to->in_room) < 3)
          to->pcdata->account->prey_cool =
          UMAX(to->pcdata->account->prey_cool, current_time + 3600 * 24);
          else if(to->pcdata->account != NULL)
          to->pcdata->account->prey_cool =
          UMAX(to->pcdata->account->prey_cool, current_time + 60 * 15);

          return;
        }
      }
    }

    if (type == PATROL_BRIBE) {
      if (ch == NULL || ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->alliance == 0)
      return;

      ROOM_INDEX_DATA *room;
      int npctype;
      if (number_percent() % 3 == 0 && room_pop(get_room_index(ROOM_MEDICAL_CARPARK)) <= 0) {
        room = get_room_index(ROOM_MEDICAL_CARPARK);
        npctype = 1;
      }
      else if (number_percent() % 2 == 0 && room_pop(get_room_index(ROOM_MEDICAL_CARPARK)) <= 0) {
        room = get_room_index(ROOM_MEDICAL_CARPARK);
        npctype = 2;
      }
      else if (room_pop(get_room_index(ROOM_POLICE_CARPARK)) <= 0) {
        room = get_room_index(ROOM_POLICE_CARPARK);
        npctype = 3;
      }
      else
      return;

      MOB_INDEX_DATA *pMobIndex;
      CHAR_DATA *mob;
      pMobIndex = get_mob_index(116);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, room);
      mob->hit = max_hp(mob);
      free_string(mob->aggression);
      mob->aggression = str_dup("all");
      mob->x = 25;
      mob->y = 25;
      mob->ttl = (12 * 30);
      mob->recent_moved = 0;
      mob->level = number_range(25, 75);
      SET_FLAG(mob->act, ACT_BRIBEMOB);
      int esttype;
      if (number_percent() % 2 == 0)
      esttype = 1;
      else
      esttype = 2;
      char buf[MSL];
      if (npctype == 1) {
        sprintf(buf, "psychiatrist");
        free_string(mob->name);
        mob->name = str_dup(buf);
        free_string(mob->short_descr);
        sprintf(buf, "psychiatrist");
        free_string(mob->short_descr);
        mob->short_descr = str_dup(buf);
        free_string(mob->long_descr);
        sprintf(buf, "A psychiatrist");
        mob->long_descr = str_dup(buf);
      }
      else if (npctype == 2) {
        sprintf(buf, "medical examiner");
        free_string(mob->name);
        mob->name = str_dup(buf);
        free_string(mob->short_descr);
        sprintf(buf, "medical examiner");
        free_string(mob->short_descr);
        mob->short_descr = str_dup(buf);
        free_string(mob->long_descr);
        sprintf(buf, "A medical examiner");
        mob->long_descr = str_dup(buf);
      }
      else {
        sprintf(buf, "HSD deputy");
        free_string(mob->name);
        mob->name = str_dup(buf);
        free_string(mob->short_descr);
        sprintf(buf, "HSD deputy");
        free_string(mob->short_descr);
        mob->short_descr = str_dup(buf);
        free_string(mob->long_descr);
        sprintf(buf, "An HSD deputy");
        mob->long_descr = str_dup(buf);
      }
      ch->pcdata->patrol_status = PATROL_BRIBING;
      ch->pcdata->patrol_timer = 20;
      ch->pcdata->patrol_room = room;
      ch->pcdata->patrol_rp = 0;
      bribe_message(ch, npctype, esttype, mob->level);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room))
        continue;
        if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
        continue;
        if (!free_to_act(to))
        continue;
        if (!same_faction(to, ch))
        continue;

        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        if (to->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] > 0 || (to->faction != 0 && prof_focus(to) > 0)) {
          to->pcdata->patrol_status = PATROL_BRIBING;
          to->pcdata->patrol_timer = 20;
          to->pcdata->patrol_room = room;
          to->pcdata->patrol_rp = 0;
          bribe_message(to, npctype, esttype, mob->level);
        }
      }
    }

    if (type == PATROL_BARGAIN) {
      if (ch == NULL || ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->alliance == 0)
      return;
      ROOM_INDEX_DATA *room;
      if (pc_pop(get_room_index(ROOM_MEETING_EAST)) <= 0)
      room = get_room_index(ROOM_MEETING_EAST);
      else
      room = get_room_index(ROOM_MEETING_WEST);
      MOB_INDEX_DATA *pMobIndex;
      CHAR_DATA *mob;
      pMobIndex = get_mob_index(116);
      mob = create_mobile(pMobIndex);
      char_to_room(mob, room);
      mob->hit = max_hp(mob);
      free_string(mob->aggression);
      mob->aggression = str_dup("all");
      mob->x = 25;
      mob->y = 25;
      mob->ttl = (12 * 30);
      mob->recent_moved = 0;
      char buf[MSL];
      int subfac = number_range(1, SF_MAX - 1);
      room->level = subfac;
      sprintf(buf, "representative %s", get_subfac_name(subfac));
      free_string(mob->name);
      mob->name = str_dup(buf);
      free_string(mob->short_descr);
      sprintf(buf, "representative of %s", get_subfac_name(subfac));
      free_string(mob->short_descr);
      mob->short_descr = str_dup(buf);
      free_string(mob->long_descr);
      sprintf(buf, "A representative of %s", get_subfac_name(subfac));
      mob->long_descr = str_dup(buf);
      ch->pcdata->patrol_status = PATROL_BARGAINING;
      ch->pcdata->patrol_timer = 20;
      ch->pcdata->patrol_room = room;
      ch->pcdata->patrol_pledged = 0;
      ch->pcdata->patrol_amount = 0;
      ch->pcdata->patrol_rp = 0;
      printf_to_char(
      ch, "You receive an alert on your phone stating that a representative of %s has shown up in the %s, offering a lucrative deal to whichever faction can persuade them. (Use patrol influence (amount) to devote influence to your cause on your next emote.)\n\r", get_subfac_name(subfac), room->name);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room))
        continue;
        if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
        continue;
        if (!free_to_act(to))
        continue;
        if (to == ch)
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        if (to->faction == 0)
        continue;
        if (to == NULL || to->faction == 0 || clan_lookup(to->faction) == NULL || clan_lookup(to->faction)->alliance == 0)
        continue;

        if (to->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] > 0 || (to->faction != 0 && prof_focus(to) > 0)) {
          to->pcdata->patrol_rp = 0;
          to->pcdata->patrol_room = room;
          to->pcdata->patrol_status = PATROL_BARGAINING;
          to->pcdata->patrol_timer = 20;
          to->pcdata->patrol_pledged = 0;
          to->pcdata->patrol_amount = 0;
          printf_to_char(to, "You receive an alert on your phone stating that a representative of %s has shown up in the %s, offering a  lucrative deal to whichever society can persuade them. (Use patrol influence (amount) to devote influence to your cause on your next emote.)\n\r", get_subfac_name(subfac), room->name);
        }
      }
    }
    if (type == PATROL_KIDNAP) {
      int maxscore = 0;
      CHAR_DATA *vic = NULL;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (!valid_syndicate_prey(to))
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        int score = syndicate_prey_score(to);
        if (score > maxscore) {
          maxscore = score;
          vic = to;
        }
      }
      if (maxscore > 0 && vic != NULL) {
        ROOM_INDEX_DATA *room;
        ROOM_INDEX_DATA *prison;
        if (pc_pop(get_room_index(ROOM_MEETING_WEST)) <= 0) {
          room = get_room_index(ROOM_MEETING_WEST);
          prison = get_room_index(ROOM_PRISON_WEST);
        }
        else {
          room = get_room_index(ROOM_MEETING_EAST);
          prison = get_room_index(ROOM_PRISON_EAST);
        }
        ch->pcdata->patrol_room = room;
        ch->pcdata->patrol_status = PATROL_BIDDING;
        ch->pcdata->patrol_timer = 15;
        ch->pcdata->patrol_rp = 0;
        ch->pcdata->patrol_target = vic;
        printf_to_char(
        ch, "You receive an alert on your phone that the syndicate have kidnapped someone and is holding an auction for them in the %s.\n\r", room->name);
        char_from_room(vic);
        char_to_room(vic, prison);
        vic->walking = 0;
        if (vic->pcdata->account != NULL)
        vic->pcdata->account->prey_cool_s = current_time + 3600 * 24 * 16;
        if (!IS_FLAG(vic->act, PLR_BOUND))
        SET_FLAG(vic->act, PLR_BOUND);
        if (IS_FLAG(vic->act, PLR_SHROUD))
        REMOVE_FLAG(vic->act, PLR_SHROUD);
        if (IS_FLAG(vic->act, PLR_DEEPSHROUD))
        REMOVE_FLAG(vic->act, PLR_DEEPSHROUD);

        vic->pcdata->patrol_status = PATROL_KIDNAPPED;
        vic->pcdata->patrol_timer = 32;
        send_to_char("You feel a sharp sting in your neck, a moment later the world fades to darkness and you feel yourself being caught by a pair of arms as you slump over.\n\r", vic);

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room))
          continue;
          if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
          continue;
          if (!free_to_act(to))
          continue;
          if (get_gm(to->in_room, FALSE) != NULL)
          continue;
          if (same_faction(to, vic))
          continue;
          if (to->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] > 0 || (to->faction != 0 && prof_focus(to) > 0)) {
            to->pcdata->patrol_room = room;
            to->pcdata->patrol_status = PATROL_BIDDING;
            to->pcdata->patrol_timer = 15;
            to->pcdata->patrol_rp = 0;
            to->pcdata->patrol_target = vic;
            printf_to_char(to, "You receive an alert on your phone that the syndicate have kidnapped someone and is holding an auction for them in the %s.\n\r", room->name);
          }
        }
      }
    }
    if (type == PATROL_ARCANEWAR) {
      if (ch == NULL || ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->alliance == 0)
      return;
      if (ch->pcdata->account != NULL && ch->pcdata->account->last_awar != 0 && ch->pcdata->account->last_awar > current_time - (3600 * 24 * 10))
      return;
      int maxscore = 200;
      CHAR_DATA *vic = NULL;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (!free_to_act(to))
        continue;
        if (!arcane_attackable(to))
        continue;
        if (IS_FLAG(to->comm, COMM_AFK))
        continue;
        if (to->pcdata->availability == AVAIL_LOW)
        continue;
        if (event_aegis == 1 && is_super(ch) != is_super(to))
        continue;
        if (!is_patrol_enemy(ch, to))
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        int score = arcane_prey_score(ch, to);
        if (score > maxscore) {
          maxscore = score;
          vic = to;
        }
      }
      if (maxscore > 75 && vic != NULL) {
        if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANEWAR))
        SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANEWAR);

        if (ch->pcdata->account != NULL)
        ch->pcdata->account->last_awar = current_time;

        ch->pcdata->patrol_rp = 0;
        ch->pcdata->patrol_target = vic;
        ch->pcdata->patrol_status = PATROL_LEADING_ASSAULT;
        ch->pcdata->patrol_timer = 25;
        vic->pcdata->patrol_target = ch;
        vic->pcdata->patrol_status = PATROL_UNDER_ASSAULT;
        vic->pcdata->patrol_timer = 25;
        vic->pcdata->patrol_rp = 0;
        ch->pcdata->patrol_pledged = 0;
        vic->pcdata->patrol_pledged = 0;
        printf_to_char(
        ch, "You feel your curse take hold of %s and start to build in power. Use `gpatrol pledge (amount) (message)`x to pledge more life force to the battle and send an emit to the target's room.\n\r", PERS(vic, ch));
        send_to_char("You feel a growing pressure behind your temples as some malicious magic takes hold of you. Use `gpatrol pledge (amount) (message)`x to pledge more life force to the battle and send an emit to the caster's room.\n\r", vic);

        if (narrative_pair(ch, vic)) {
          printf_to_char(ch, "%s", narrative_pair_rewards(ch, vic));
          printf_to_char(vic, "%s", narrative_pair_rewards(ch, vic));
        }

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room))
          continue;
          if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
          continue;
          if (!free_to_act(to))
          continue;
          if (get_gm(to->in_room, FALSE) != NULL)
          continue;
          if (to->pcdata->patrol_habits[PATROL_ARCANEHABIT] > 0 || (to->faction != 0 && arcane_focus(to) > 0)) {
            if (same_faction(to, ch) && !same_faction(to, vic)) {
              printf_to_char(
              to, "You sense %s coming under an arcane assault somewhere to the %s of you.\n\r", PERS(vic, to), relspacial[get_reldirection(
              roomdirection(
              get_roomx(to->in_room), get_roomy(to->in_room), get_roomx(vic->in_room), get_roomy(vic->in_room)), to->facing)]);
            }
            else if (same_faction(to, vic) && !same_faction(to, ch)) {
              printf_to_char(
              to, "You sense %s launching an arcane assault somewhere to the %s of you.\n\r", PERS(vic, to), relspacial[get_reldirection(
              roomdirection(
              get_roomx(to->in_room), get_roomy(to->in_room), get_roomx(ch->in_room), get_roomy(ch->in_room)), to->facing)]);
            }
          }
        }
      }
      return;
    }
    if (type == PATROL_GHOST) {
      sprintf(buf, "PATROLGHOST: %s", ch->name);
      log_string(buf);
      if(ch->fsect != 0)
      ch->pcdata->account->last_ghost -= (3600*24*6);

      EXTRA_PATROL_TYPE * pat = random_epatrol();
      if(pat != NULL && pat->clan_id != ch->fcult)
      ch->pcdata->patrol_subtype = pat->id;
      else
      {
        ch->pcdata->patrol_subtype = 0;
        return;
      }
      ch->pcdata->patrol_status = PATROL_GHOST_FOCUS;
      ch->pcdata->patrol_timer = 15;
      ch->pcdata->patrol_room = graveyard_room();
      sprintf(buf, "PATROLROOM: %d", ch->pcdata->patrol_room->vnum);
      log_string(buf);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to) || to == ch)
        continue;
        if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
        continue;
        if (!free_to_act(to))
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        if (to->pcdata->patrol_habits[PATROL_SPIRITHABIT] > 0 || (to->faction != 0 && arcane_focus(to) > 0)) {
          to->pcdata->patrol_status = PATROL_SENSING_GHOST;
          to->pcdata->patrol_target = ch;
          to->pcdata->patrol_room = ch->pcdata->patrol_room;
          to->pcdata->patrol_subtype = ch->pcdata->patrol_subtype;
          to->pcdata->patrol_timer = 15;
          printf_to_char(
          to, "\n\n\nYou sense the rising of a malevolent spirit to the %s.\n\r", relspacial[get_reldirection(
          roomdirection(get_roomx(to->in_room), get_roomy(to->in_room), get_roomx(to->pcdata->patrol_room), get_roomy(to->pcdata->patrol_room)), to->facing)]);
        }
      }
      printf_to_char(
      ch, "\n\n\nYou sense the rising of a malevolent spirit to the %s.\n\r", relspacial[get_reldirection(
      roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(ch->pcdata->patrol_room), get_roomy(ch->pcdata->patrol_room)), ch->facing)]);
      return;
    }

    if (type == PATROL_ARTIFACT) {
      char objname[MSL];
      sprintf(objname, "%s %s", get_arti_material(), get_arti_type());
      OBJ_DATA *obj;
      obj = create_object(get_obj_index(36), 0);
      obj->level = 0;
      obj->size = 10;
      free_string(obj->name);
      obj->name = str_dup(objname);
      free_string(obj->short_descr);
      obj->short_descr = str_dup(objname);
      char tmp[MSL];
      sprintf(tmp, "%s %s", a_or_an(objname), objname);
      free_string(obj->description);
      obj->description = str_dup(tmp);
      obj->rot_timer = 180;
      obj->level = number_range(100, 500);
      obj->level *= -1;
      obj_to_char(obj, ch);
      ch->pcdata->patrol_status = PATROL_HOLDING_ARTIFACT;
      ch->pcdata->patrol_timer = 15;
      ch->pcdata->patrol_rp = 0;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to) || to == ch)
        continue;
        if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
        continue;
        if (!free_to_act(to))
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        if (to->pcdata->patrol_habits[PATROL_ARCANEHABIT] > 0 || (to->faction != 0 && arcane_focus(to) > 0)) {
          to->pcdata->patrol_status = PATROL_SENSING_ARTIFACT;
          to->pcdata->patrol_target = ch;
          to->pcdata->patrol_timer = 15;
          to->pcdata->patrol_rp = 0;
          printf_to_char(
          to, "\n\n\nYou sense the awakening of a cursed object to the %s.\n\r", relspacial[get_reldirection(
          roomdirection(get_roomx(to->in_room), get_roomy(to->in_room), get_roomx(ch->in_room), get_roomy(ch->in_room)), to->facing)]);
        }
      }
      printf_to_char(ch, "\n\n\nYou locate %s, you can sense its magic but as soon as you touch it you awaken a curse laid upon it. (use `gpatrol decurse`x to attempt to decurse the object, you'll need a least 2 other people to help you.)\n\r", obj->description);
      return;
    }

    if (type == PATROL_HUNT) {
      if ((ch->in_room->area->vnum == OUTER_NORTH_FOREST || ch->in_room->area->vnum == OUTER_SOUTH_FOREST || ch->in_room->area->vnum == OUTER_WEST_FOREST) && ch->in_room->sector_type == SECT_FOREST) {
        ch->pcdata->patrol_room = ch->in_room;
        send_to_char("\n\n\nYou spot some big game up ahead. Wait here for allies before approaching further.\n\r", ch);
        ch->pcdata->patrol_status = PATROL_APPROACHINGHUNT;
        ch->pcdata->patrol_timer = 5;
      }
      else {
        ROOM_INDEX_DATA *room;
        room =
        room_by_coordinates(number_range(0, 71), number_range(104, 110), 0);
        if (number_percent() % 2 == 0 || room->sector_type != SECT_FOREST)
        room =
        room_by_coordinates(number_range(0, 71), number_range(-38, -33), 0);
        if (room->sector_type != SECT_FOREST)
        room =
        room_by_coordinates(number_range(-37, -32), number_range(0, 71), 0);
        ch->pcdata->patrol_room = room;
        send_to_char("\n\n\nYou receive a report about some big game deeper in the forest. Use `gpatrol approach`x to move closer.\n\r", ch);
        ch->pcdata->patrol_status = PATROL_HUNTMOVING;
        ch->pcdata->patrol_timer = 5;
      }
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to) || to == ch)
        continue;
        if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;
        if (to->pcdata->patrol_habits[PATROL_HUNTHABIT] > 0) {
          send_to_char("\n\n\nYou receive a report about some big game being spotted in the forest. Use `gpatrol approach`x from a forest room to move closer.\n\r", to);
          to->pcdata->patrol_status = PATROL_HUNTMOVING;
          to->pcdata->patrol_timer = 5;
          to->pcdata->patrol_room = ch->pcdata->patrol_room;
        }
      }
    }
  }

  CHAR_DATA *random_person_room(ROOM_INDEX_DATA *room) {
    CHAR_DATA *ch;
    int count = 0;

    // Count the number of valid people in the room
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      ch = *it;
      if (ch != NULL && !IS_NPC(ch))
      count++;
    }

    // If there are no valid people in the room, return NULL
    if (count == 0)
    return NULL;

    // Generate a random index
    int randomIndex = number_range(0, count - 1);

    // Iterate to the randomly chosen position in the list
    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      ch = *it;
      if (ch != NULL && !IS_NPC(ch)) {
        // Check if this is the randomly chosen person
        if (randomIndex == 0)
        return ch;
        randomIndex--;
      }
    }

    // This should not be reached, but return NULL just in case
    return NULL;
  }

  void patrol_disturbance(CHAR_DATA *ch, int type) {
    CHAR_DATA *target = random_person_room(ch->in_room);

    if (type == PATROL_ARTIFACT) {
      switch (number_percent() % 9) {
      case 0:
        act("`RFor several seconds the room grows painfully hot.`x", ch, NULL, NULL, TO_ROOM);
        act("`RFor several seconds the room grows painfully hot.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`RFor several seconds the room grows painfully hot.`x");
        break;
      case 1:
        act("`CFor several seconds the room grows painfully cold.`x", ch, NULL, NULL, TO_ROOM);
        act("`CFor several seconds the room grows painfully cold.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`CFor several seconds the room grows painfully cold.`x");
        break;
      case 2:
        act("`BAn arc of lightning blasts out of the artifact to strike $N, sending $M flying.`x", ch, NULL, target, TO_NOTVICT);
        act("`BAn arc of lightning blasts out of the artifact to strike $N, sending $M flying.`x", ch, NULL, target, TO_CHAR);
        act("`BAn arc of lightning blasts out of the artifact to strike you, sending you flying.`x", ch, NULL, target, TO_VICT);
        logevent_check(ch, logact("`BAn arc of lightning blasts out of the artifact to strike $N, sending $M flying.`x", target, target));
        break;
      case 3:
        act("`cAll metal objects in the room are suddenly tugged violently towards the artifact.`x", ch, NULL, NULL, TO_ROOM);
        act("`cAll metal objects in the room are suddenly tugged violently towards the artifact.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`cAll metal objects in the room are suddenly tugged violently towards the artifact.`x");
        break;
      case 4:
        act("`CGrowing air pressure makes your ear's pop uncomfortably and deadens all sound.`x", ch, NULL, NULL, TO_ROOM);
        act("`CGrowing air pressure makes your ear's pop uncomfortably and deadens all sound.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`CGrowing air pressure makes your ear's pop uncomfortably and deadens all sound.`x");
        break;
      case 5:
        act("`WThe artifact emits a flash of blinding light.`x", ch, NULL, NULL, TO_ROOM);
        act("`WThe artifact emits a flash of blinding light.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`WThe artifact emits a flash of blinding light.`x");
        break;
      case 6:
        act("`yThe whole room shakes, making you unsteady on your feet.`x", ch, NULL, NULL, TO_ROOM);
        act("`yThe whole room shakes, making you unsteady on your feet.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`yThe whole room shakes.");
        break;
      case 7:
        act("`RA section of $N's clothing catches fire.`x", ch, NULL, target, TO_NOTVICT);
        act("`RA section of $N's clothing catches fire.`x", ch, NULL, target, TO_CHAR);
        act("`RA section of your clothing catches fire.`x", ch, NULL, target, TO_VICT);
        logevent_check(ch, logact("`RA section of $N's clothing catches fire.`x", target, target));
        break;
      case 8:
        CHAR_DATA *newt = random_person_room(ch->in_room);
        if (newt != target) {
          act("`cA blast of force sends $n flying into $N.`x", newt, NULL, target, TO_NOTVICT);
          act("`cA blast of force sends you flying into $N.`x", newt, NULL, target, TO_CHAR);
          act("`cA blast of force sends $n flying into you.`x", newt, NULL, target, TO_VICT);
          logevent_check(ch, logact("`cA blast of force sends $n flying into $N.`x", newt, target));
        }
        break;
      }
    }
    else if (type == PATROL_GHOST) {
      if(ch->pcdata->patrol_subtype > 0)
      {
        EXTRA_PATROL_TYPE *pat = epatrol_by_id(ch->pcdata->patrol_subtype);
        if(pat != NULL)
        {
          int value = number_range(1, linecount(pat->messages));
          act(fetch_line(pat->messages, value), ch, NULL, target, TO_ROOM);
          act(fetch_line(pat->messages, value), ch, NULL, target, TO_CHAR);
          logevent_check(ch, logact(fetch_line(pat->messages, value), ch, target));
          return;
        }
      }
      switch (number_percent() % 10) {
      case 0:
        act("`CAn icy cold wind comes out of nowhere to whip around the room.`x", ch, NULL, NULL, TO_ROOM);
        act("`CAn icy cold wind comes out of nowhere to whip around the room.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(
        ch, "`CAn icy cold wind comes out of nowhere to whip around the room.`x");
        break;
      case 1:
        act("`DAll the lights flicker and die for several seconds.`x", ch, NULL, NULL, TO_ROOM);
        act("`DAll the lights flicker and die for several seconds.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`DAll the lights flicker and die for several seconds.`x");
        break;
      case 2:
        act("`CA gust of icy wind sends $N flying across the room.`x", ch, NULL, target, TO_NOTVICT);
        act("`CA gust of icy wind sends $N flying across the room.`x", ch, NULL, target, TO_CHAR);
        act("`CA gust of icy wind sends you flying across the room.`x", ch, NULL, target, TO_VICT);
        logevent_check(
        ch, logact("`CA gust of icy wind sends $N flying across the room.`x", target, target));
        break;
      case 3:
        act("`yA nearby object is picked up and flung into $N.`x", ch, NULL, target, TO_NOTVICT);
        act("`yA nearby object is picked up and flung into $N.`x", ch, NULL, target, TO_CHAR);
        act("`yA nearby object is picked up and flung into you.`x", ch, NULL, target, TO_VICT);
        logevent_check(
        ch, logact("`yA nearby object is picked up and flung into $N.`x", target, target));
        break;
      case 4:
        act("`R$N's eyes start to weep blood.`x", ch, NULL, target, TO_NOTVICT);
        act("`R$N's eyes start to weep blood.`x", ch, NULL, target, TO_CHAR);
        act("`RYou lose your ability to see for several minutes as you weep blood.`x", ch, NULL, target, TO_VICT);
        logevent_check(
        ch, logact("`R$N's eyes start to weep blood.`x", target, target));
        break;
      case 5:
        act("`rYou are filled with terrible anger.`x", ch, NULL, target, TO_VICT);
        break;
      case 6:
        act("`DYou are filled with terrible sorrow.`x", ch, NULL, target, TO_VICT);
        break;
      case 7:
        act("`g$N's skin starts to fester.`x", ch, NULL, target, TO_NOTVICT);
        act("`g$N's skin starts to fester.`x", ch, NULL, target, TO_CHAR);
        act("`gYour skin starts to fester.`x", ch, NULL, target, TO_VICT);
        logevent_check(ch, logact("`g$N's skin starts to fester.`x", target, target));
        break;
      case 8:
        act("`yA swarm of locusts fills the room.`x", ch, NULL, NULL, TO_ROOM);
        act("`yA swam of locusts fills the room.`x", ch, NULL, NULL, TO_CHAR);
        logevent_check(ch, "`yA swarm of locusts fills the room.`x");
        break;
      case 9:
        act("`g$N convulses and then vomits up a serpent.`x", ch, NULL, target, TO_NOTVICT);
        act("`g$N convulses and then vomits up a serpent.`x", ch, NULL, target, TO_CHAR);
        act("`gYou convulse and then vomit up a serpent.`x", ch, NULL, target, TO_VICT);
        logevent_check(ch, logact("`g$N convulses and then vomits up a serpent.`x", target, target));
        break;
      }
    }
  }

  void finish_arcane_assault(CHAR_DATA *ch, CHAR_DATA *victim) {
    int charscore = 0;
    int vicscore = 0;
    int vicpersonal = 0;
    int charpledged = 0;
    ;
    int vicpledged = 0;
    ch->pcdata->patrol_did_arcane = TRUE;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;

      if (fch->pcdata->patrol_status == PATROL_LEADING_ASSAULT || fch->pcdata->patrol_status == PATROL_ASSISTING_ASSAULT) {
        charscore += fch->pcdata->patrol_pledged * arcanist_score(fch);
        charpledged += fch->pcdata->patrol_pledged;
        use_lifeforce(fch, 100 * fch->pcdata->patrol_pledged, "Pledge lifeforce for patrol.");
        fch->pcdata->patrol_pledged = 0;
        fch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
        fch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
        villain_mod(fch, 20, "Assisting arcane assault");
      }
    }
    for (CharList::iterator it = victim->in_room->people->begin();
    it != victim->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;

      if (fch->pcdata->patrol_status == PATROL_UNDER_ASSAULT || fch->pcdata->patrol_status == PATROL_ASSISTING_UNDER_ASSAULT) {
        vicscore += fch->pcdata->patrol_pledged * arcanist_score(fch);
        vicpledged += fch->pcdata->patrol_pledged;
        if (fch->pcdata->patrol_status == PATROL_UNDER_ASSAULT)
        vicpersonal = fch->pcdata->patrol_pledged;
        use_lifeforce(fch, 100 * fch->pcdata->patrol_pledged, "Pledge lifeforce for patrol.");
        fch->pcdata->patrol_pledged = 0;
        fch->pcdata->week_tracker[TRACK_PATROL_ARCANE]++;
        fch->pcdata->life_tracker[TRACK_PATROL_ARCANE]++;
      }
    }
    if (charscore > vicscore) {
      int val = charpledged + vicpledged - vicpersonal;
      val /= 3;
      val += vicpersonal;
      if (vicpersonal < 10) {
        val += (10 - vicpersonal);
        take_lifeforce(victim, (10 - vicpersonal) * 100, "Drained by patrol");
      }
      val *= 70;
      val = val * get_tier(victim) * get_tier(victim) / 5;
      int rplvl = 0;
      for (CharList::iterator it = ch->in_room->people->begin();
      it != ch->in_room->people->end();) {
        CHAR_DATA *fch = *it;
        ++it;
        if (fch == NULL)
        continue;
        if (!IS_NPC(fch))
        rplvl += fch->pcdata->patrol_rp;
      }
      val = val * rplvl / 1000;
      val = val * 4;
      val = UMAX(val, 10);
      char buf[MSL];

      sprintf(buf, "GEMVAL: %s, %d, %d.", victim->name, val, rplvl);
      log_string(buf);

      OBJ_DATA *obj;
      obj = create_object(get_obj_index(36), 0);
      obj->level = val;
      obj->size = 10;
      free_string(obj->name);
      obj->name = str_dup("faintly glowing red jewel");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("faintly glowing red jewel");
      free_string(obj->description);
      obj->description = str_dup("A faintly glowing red jewel");
      obj_to_char(obj, ch);
      act("$n suddenly siezes up, $s back going completly rigid and arching painfully as a stream of faintly glowing red energy flows out of $s mouth and disappears into the air.", victim, NULL, NULL, TO_ROOM);
      printf_to_char(
      victim, "You suddenly sieze up, your back going completely rigid and arching painfully as a stream of faintly glowing red energy flows out of your mouth and disappears into the air, a faint image of %s visible just for a second near where it vanishes. After your fist unclenches you discover a tiny red gem inside it.", PERS(ch, victim));
      act("All the nearby lights flicker and die for a second before a stream of faintly glowing red energy coalesces out of the air and flows into a small gem in $n's hand.", ch, NULL, NULL, TO_ROOM);
      printf_to_char(
      ch, "All the nearby lights flicker and die for a second before a small window opens in the air, you can see %s through it for a second before a stream of faintly glowing red energy flows out of them and through the window and into a small gem in your hand.\n\r", PERS(victim, ch));
      logevent_check(
      ch, logact("$n suddenly siezes up, $s back going completly rigid and arching painfully as a stream of faintly glowing red energy flows out of $s mouth and disappears into the air.", victim, victim));
      logevent_check(
      ch, logact("All the nearby lights flicker and die for a second before a stream of faintly glowing red energy coalesces out of the air and flows into a small gem in $n's hand.", ch, ch));
      obj = create_object(get_obj_index(36), 0);
      obj->level = val / 10;
      obj->size = 10;
      free_string(obj->name);
      obj->name = str_dup("faintly glowing red jewel");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("tiny, faintly glowing red jewel");
      free_string(obj->description);
      obj->description = str_dup("A tiny, faintly glowing red jewel");
      obj_to_char(obj, victim);

      return;
    }
    else {
      int val = charpledged + vicpledged;
      val /= 2;
      val *= 70;
      val = val * get_tier(ch) * get_tier(victim) / 5;
      int rplvl = 0;
      for (CharList::iterator it = victim->in_room->people->begin();
      it != victim->in_room->people->end();) {
        CHAR_DATA *fch = *it;
        ++it;
        if (fch == NULL)
        continue;
        if (!IS_NPC(fch))
        rplvl += fch->pcdata->patrol_rp;
      }
      val = val * rplvl / 1000;
      val = val * 4;

      OBJ_DATA *obj;
      obj = create_object(get_obj_index(36), 0);
      obj->level = val;
      obj->size = 10;
      free_string(obj->name);
      obj->name = str_dup("faintly glowing red jewel");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("faintly glowing red jewel");
      free_string(obj->description);
      obj->description = str_dup("A faintly glowing red jewel");
      obj_to_char(obj, victim);
      act("$n suddenly siezes up, $s back going completly rigid and arching painfully as a stream of faintly glowing red energy flows out of $s mouth and disappears into the air.", ch, NULL, NULL, TO_ROOM);
      printf_to_char(
      ch, "You suddenly sieze up, your back going completely rigid and arching painfully as a stream of faintly glowing red energy flows out of your mouth and disappears into the air, a faint image of %s visible just for a second near where it vanishes. When your fist unclenches you discover a tiny glowing red crystal inside it.", PERS(victim, ch));
      printf_to_char(
      victim, "All the nearby lights flicker and die for a second before a small window opens in the air, you can see %s through it for a second before a stream of faintly glowing red energy flows out of them and through the window and into a small gem in your hand.\n\r", PERS(ch, victim));
      act("All the nearby lights flicker and die for a second before a stream of faintly glowing red energy coalesces out of the air and flows into a small gem in $n's hand.", victim, NULL, NULL, TO_ROOM);
      logevent_check(
      ch, logact("$n suddenly siezes up, $s back going completly rigid and arching painfully as a stream of faintly glowing red energy flows out of $s mouth and disappears into the air.", ch, ch));
      logevent_check(
      ch, logact("All the nearby lights flicker and die for a second before a stream of faintly glowing red energy coalesces out of the air and flows into a small gem in $n's hand.", victim, victim));

      obj = create_object(get_obj_index(36), 0);
      obj->level = val / 10;
      obj->size = 10;
      free_string(obj->name);
      obj->name = str_dup("faintly glowing red jewel");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("tiny, faintly glowing red jewel");
      free_string(obj->description);
      obj->description = str_dup("A tiny, faintly glowing red jewel");
      obj_to_char(obj, victim);

      return;
    }
  }

  void patrol_room_award(CHAR_DATA *ch, int type) {
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;

      if (type == PATROL_GHOST) {
        if (pc_pop(fch->in_room) <= 1)
        fch->pcdata->patrol_rp = fch->pcdata->patrol_rp * 2 / 3;
        else if (only_faction(fch))
        fch->pcdata->patrol_rp /= 2;
        else if (only_allies(fch))
        fch->pcdata->patrol_rp = fch->pcdata->patrol_rp * 2 / 3;

        if (fch->fsect > 0)
        fch->pcdata->patrol_rp = fch->pcdata->patrol_rp * 2;

        if(!IS_FLAG(fch->pcdata->account->flags, ACCOUNT_DIDSPIRIT))
        {
          fch->pcdata->patrol_rp = fch->pcdata->patrol_rp / 2;
          SET_FLAG(fch->pcdata->account->flags, ACCOUNT_DIDSPIRIT);
        }

        if (fch->pcdata->account != NULL)
        fch->pcdata->account->pkarma =
        UMIN(40000 - fch->pcdata->account->pkarmaspent, fch->pcdata->account->pkarma + fch->pcdata->patrol_rp);
        char buf[MSL];
        sprintf(buf, "GHOSTXP: %s, %d.", fch->name, fch->pcdata->patrol_rp);
        log_string(buf);
        gain_rpexp(fch, fch->pcdata->patrol_rp * 2);
      }
      if (type == -1) {
        if (fch->pcdata->account != NULL)
        fch->pcdata->account->pkarma =
        UMIN(40000 - fch->pcdata->account->pkarmaspent, fch->pcdata->account->pkarma + fch->pcdata->patrol_rp);
        gain_exp(fch, fch->pcdata->patrol_rp * 20, FEXP_PATROL);
      }
    }
  }

  void patrol_personal_award(CHAR_DATA *ch, int type) {
    if (type == PATROL_BRIBE) {
      if (ch->pcdata->account != NULL)
      ch->pcdata->account->pkarma =
      UMIN(40000 - ch->pcdata->account->pkarmaspent, ch->pcdata->account->pkarma + ch->pcdata->patrol_rp * 2);
      gain_rpexp(ch, ch->pcdata->patrol_rp * 5);
    }
  }


  CHAR_DATA * find_pat_char(CHAR_DATA *ch)
  {
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
      if(victim == ch)
      continue;

      if(victim->pcdata->account != NULL && victim->pcdata->account == ch->pcdata->account)
      continue;
      if(is_gm(victim))
      continue;

      if(victim->in_room != ch->in_room)
      continue;

      if(victim->pcdata->patrol_status > 0)
      return victim;
    }
    return NULL;
  }


  void patrol_update(CHAR_DATA *ch) {

    if(ch->pcdata->patrol_timer <= 0 && ch->pcdata->patrol_status > 1 && !in_fight(ch) && ch->pcdata->patrol_status != PATROL_DEFENDASSISTING && ch->pcdata->patrol_status != PATROL_ATTACKASSISTING)
    ch->pcdata->patrol_status = 0;
    if (ch->pcdata->patrol_status == PATROL_HUNTING && in_fight(ch))
    ch->pcdata->patrol_status = PATROL_HUNTFIGHTING;
    if (ch->pcdata->patrol_status == PATROL_HUNTFIGHTING && (!in_fight(ch) || ch->in_room->area->vnum == 13)) {
      ch->pcdata->patrol_status = 0;
      ch->pcdata->patrol_timer = 0;
    }
    if(ch->pcdata->patrol_status == PATROL_HUNTING && ch->in_room->area->vnum == 13) {
      ch->pcdata->patrol_status = 0;
      ch->pcdata->patrol_timer = 0;
    }


    if (ch->pcdata->patrol_timer % 3 == 0 && ch->pcdata->patrol_timer > 0) {
      if (ch->pcdata->patrol_status == PATROL_ATTACKSEARCHING) {
        printf_to_char(
        ch, "About %d minutes until you find them in the nightmare.\n\r", ch->pcdata->patrol_timer + number_range(-1, 1));
      }
      if (ch->pcdata->patrol_status == PATROL_DEFENDHIDING) {
        printf_to_char(ch, "About %d minutes until they find you.\n\r", ch->pcdata->patrol_timer);
      }
      if (ch->pcdata->patrol_status == PATROL_CHASING) {
        printf_to_char(ch, "About %d minutes until you catch them.\n\r", ch->pcdata->patrol_timer);
        printf_to_char(ch->pcdata->patrol_target, "About %d minutes until they catch you.\n\r", ch->pcdata->patrol_timer);
      }
      if (ch->pcdata->patrol_status == PATROL_SEARCHING) {
        printf_to_char(ch, "About %d minutes until you find them.\n\r", ch->pcdata->patrol_timer + number_range(-1, 1));
        printf_to_char(ch->pcdata->patrol_target, "About %d minutes until they find you.\n\r", ch->pcdata->patrol_timer);
      }
    }
    if (ch->pcdata->patrol_timer % 5 == 0 && ch->pcdata->patrol_timer > 0) {
      if (ch->pcdata->patrol_status == PATROL_BANISHING || ch->pcdata->patrol_status == PATROL_ASSISTING_BANISHING) {
        printf_to_char(ch, "%d minutes left until the conclusion of the attempt to banish the spirit.\n\r", ch->pcdata->patrol_timer);
      }
      if (ch->pcdata->patrol_status == PATROL_DECURSING || ch->pcdata->patrol_status == PATROL_ASSISTING_DECURSING) {
        printf_to_char(
        ch, "%d minutes left until the conclusion of the decurse the item.\n\r", ch->pcdata->patrol_timer);
      }
      if (ch->pcdata->patrol_status >= PATROL_LEADING_ASSAULT && ch->pcdata->patrol_status <= PATROL_ASSISTING_UNDER_ASSAULT) {
        printf_to_char(
        ch, "%d minutes left until the conclusion of magical battle.\n\r", ch->pcdata->patrol_timer);
      }
      if (ch->pcdata->patrol_status == PATROL_BARGAINING) {
        printf_to_char(ch, "%d minutes left until the NPC makes up their mind.\n\r", ch->pcdata->patrol_timer);
      }
      if (ch->pcdata->patrol_status == PATROL_BRIBING) {
        printf_to_char(ch, "%d minutes left until the NPC leaves.\n\r", ch->pcdata->patrol_timer);
      }
    }

    if (ch->pcdata->patrol_status == PATROL_BRIBING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer == 30 && ch->in_room == ch->pcdata->patrol_room) {
        send_to_char("Enough people have moved away from the target that you can attempt the handoff.\n\r", ch);
      }
      if (ch->pcdata->patrol_timer <= 0)
      ch->pcdata->patrol_status = 0;
    }
    if (ch->pcdata->patrol_status == PATROL_GRABBING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0) {
        act("$N breaks away from you.", ch, NULL, ch->pcdata->patrol_target, TO_CHAR);
        act("You break away from $n, use flee to run or hide to hide or attack $n to start a fight.", ch, NULL, ch->pcdata->patrol_target, TO_VICT);
        ch->pcdata->patrol_status = PATROL_PREYING;
        ch->pcdata->patrol_timer = 10;
        ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_PREY;
        ch->pcdata->patrol_target->pcdata->patrol_timer = 60;
        logevent_check(
        ch, logact("$N breaks away from $n.", ch, ch->pcdata->patrol_target));
        return;
      }
    }

    if(ch->pcdata->patrol_status == PATROL_PREYING || ch->pcdata->patrol_status == PATROL_PREY)
    {
      if(ch->pcdata->patrol_target == NULL)
      ch->pcdata->patrol_target = find_pat_char(ch);
      if(ch->pcdata->patrol_target == NULL || ch->pcdata->patrol_target->in_room == NULL || ch->pcdata->patrol_target->in_room != ch->in_room)
      {
        ch->pcdata->patrol_status = 0;
        ch->pcdata->patrol_timer = 0;
        return;
      }
      if(ch->pcdata->patrol_status == PATROL_PREYING)
      {
        if(ch->pcdata->villain_option == VILLAIN_OPTION_CHOOSING)
        {
          ch->pcdata->villain_option_cooldown--;
          ch->pcdata->villain_emote_cooldown--;
          if(ch->pcdata->villain_option_cooldown <= 0)
          {
            villain_timeout(ch, ch->pcdata->patrol_target);
          }
          else if(ch->pcdata->villain_option_cooldown % 3 == 0 && ch->pcdata->villain_option_cooldown < 15)
          {
            printf_to_char(ch, "You have %d minutes left to choose an option, type 'chase' to see them.\n\r", ch->pcdata->villain_option_cooldown);
          }
        }
        else if(ch->pcdata->villain_option == VILLAIN_OPTION_EMOTING)
        {
          ch->pcdata->villain_emote_cooldown--;
          if(ch->pcdata->villain_emote_cooldown <= 0)
          {
            villain_timeout(ch, ch->pcdata->patrol_target);
          }
          else if(ch->pcdata->villain_emote_cooldown % 3 == 0 && ch->pcdata->villain_emote_cooldown < 15)
          {
            printf_to_char(ch, "You have %d minutes left to emote.\n\r", ch->pcdata->villain_emote_cooldown);
          }
        }
      }
      else if(ch->pcdata->patrol_status == PATROL_PREY)
      {
        if(ch->pcdata->prey_option == PREY_OPTION_CHOOSING)
        {
          ch->pcdata->prey_option_cooldown--;
          ch->pcdata->prey_emote_cooldown--;
          if(ch->pcdata->prey_option_cooldown <= 0)
          {
            prey_timeout(ch, ch->pcdata->patrol_target);
          }
          else if(ch->pcdata->prey_option_cooldown % 3 == 0 && ch->pcdata->prey_option_cooldown < 15)
          {
            printf_to_char(ch, "You have %d minutes left to choose an option, type 'escape' to see them.\n\r", ch->pcdata->prey_option_cooldown);
          }
        }
        else if(ch->pcdata->prey_option == PREY_OPTION_EMOTING)
        {
          ch->pcdata->prey_emote_cooldown--;
          if(ch->pcdata->prey_emote_cooldown <= 0)
          {
            prey_timeout(ch, ch->pcdata->patrol_target);
          }
          else if(ch->pcdata->prey_emote_cooldown % 3 == 0 && ch->pcdata->prey_emote_cooldown < 15)
          {
            printf_to_char(ch, "You have %d minutes left to emote.\n\r", ch->pcdata->prey_emote_cooldown);
          }
        }
      }
      return;
    }

    if (ch->pcdata->patrol_status == PATROL_CHASING || ch->pcdata->patrol_status == PATROL_SEARCHING || ch->pcdata->patrol_status == PATROL_PREYING) {
      if (ch->pcdata->patrol_target == NULL) {
        ch->pcdata->patrol_status = 0;
        ch->pcdata->patrol_timer = 0;
        return;
      }
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0) {
        if (ch->pcdata->patrol_status == PATROL_CHASING && get_speed(ch) + 5 < get_speed(ch->pcdata->patrol_target)) {
          ch->pcdata->patrol_timer = 1;
          logevent_check(ch, logact("$n escapes!", ch->pcdata->patrol_target, ch->pcdata->patrol_target));
          ch->pcdata->patrol_timer = 0;
          act("$N escapes!", ch, NULL, ch->pcdata->patrol_target, TO_CHAR);
          act("You escape!", ch, NULL, ch->pcdata->patrol_target, TO_VICT);
          char_from_room(ch->pcdata->patrol_target);
          char_to_room(ch->pcdata->patrol_target, random_inner_forest());
          ch->pcdata->patrol_target->pcdata->patrol_status = 0;
          ch->pcdata->patrol_status = 0;
          return;
        }
        ch->pcdata->patrol_status = PATROL_GRABBING;
        ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_GRABBED;
        ch->pcdata->patrol_timer = 20;
        ch->pcdata->patrol_target->pcdata->patrol_timer = 60;
        act("You sieze a hold of $N, use subdue to knock them out or let them squirm until they get free.", ch, NULL, ch->pcdata->patrol_target, TO_CHAR);
        act("$n siezes a hold of you. Use squirm to try to wriggle free.", ch, NULL, ch->pcdata->patrol_target, TO_VICT);
        logevent_check(
        ch, logact("$n siezes a hold of $N.", ch, ch->pcdata->patrol_target));
        return;
      }
    }

    if (ch->pcdata->patrol_status == PATROL_STALKING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0)
      ch->pcdata->patrol_status = 0;
    }

    if (ch->pcdata->patrol_status == PATROL_BIDDING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer == 25 && ch->in_room == ch->pcdata->patrol_room)
      send_to_char("The bidding commences, use patrol bid (amount) to bid on the victim, or patrol bribe to find out more about them for $25.\n\r", ch);
      if (ch->pcdata->patrol_timer % 10 == 0 && ch->in_room == ch->pcdata->patrol_room)
      printf_to_char(
      ch, "%d minutes remaining in the auction. Use patrol bid (amount) to bid on the victim, or patrol bribe to find out more about them.\n\r", ch->pcdata->patrol_timer);
      if (ch->pcdata->patrol_timer <= 0) {
        int maxbid = 0;
        CHAR_DATA *maxbidder = NULL;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->pcdata->patrol_amount > maxbid) {
            maxbid = fch->pcdata->patrol_amount;
            maxbidder = fch;
            fch->pcdata->patrol_status = 0;
            fch->pcdata->patrol_timer = 0;
            fch->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC]++;
            fch->pcdata->life_tracker[TRACK_PATROL_DIPLOMATIC]++;
          }
        }
        if (maxbid > 0) {
          act("The auctioneer announces that $n has won the bid and goes over to $m to discuss delivery.", maxbidder, NULL, NULL, TO_ROOM);
          maxbidder->pcdata->patrol_status = PATROL_COLLECTING;
          maxbidder->pcdata->patrol_timer = 30;
          logevent_check(ch, logact("The auctioneer announces that $n has won the bid and goes over to $m to discuss delivery.", maxbidder, maxbidder));
          if (maxbidder->in_room->vnum == ROOM_MEETING_WEST)
          maxbidder->pcdata->patrol_room = get_room_index(ROOM_PRISON_WEST);
          else
          maxbidder->pcdata->patrol_room = get_room_index(ROOM_PRISON_EAST);
          send_to_char("You have won the auction. You can use patrol collect any time in the next 25 minutes to have the victim delivered to you.\n\r", maxbidder);


          for (CharList::iterator it = maxbidder->pcdata->patrol_room->people->begin();
          it != maxbidder->pcdata->patrol_room->people->end();) {
            CHAR_DATA *fch = *it;
            ++it;
            if (fch == NULL || IS_NPC(fch))
            continue;
            fch->pcdata->patrol_timer = 30;
          }
        }
      }
    }
    if (ch->pcdata->patrol_status == PATROL_COLLECTING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0)
      ch->pcdata->patrol_status = 0;
    }
    if (ch->pcdata->patrol_status == PATROL_KIDNAPPED) {
      ch->pcdata->patrol_timer--;
    }
    if (ch->in_room != NULL && ch->pcdata->patrol_timer <= 0 && !IS_AFFECTED(ch, AFF_BADKIDNAPPEE) && (ch->in_room->vnum == ROOM_PRISON_WEST || ch->in_room->vnum == ROOM_PRISON_EAST)) {
      char_from_room(ch);
      char_to_room(ch, random_inner_forest());
      ch->walking = 0;
      if (IS_FLAG(ch->act, PLR_BOUND))
      REMOVE_FLAG(ch->act, PLR_BOUND);
      if (IS_FLAG(ch->act, PLR_BOUNDFEET))
      REMOVE_FLAG(ch->act, PLR_BOUNDFEET);

      send_to_char("You are dumped off in the forest somwhere.\n\r", ch);
    }
    if (ch->pcdata->patrol_status == PATROL_LEADING_ASSAULT) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 2) {
        finish_arcane_assault(ch, ch->pcdata->patrol_target);
        ch->pcdata->patrol_status = 0;
      }
    }
    if (ch->pcdata->patrol_status == PATROL_UNDER_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_UNDER_ASSAULT) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0)
      ch->pcdata->patrol_status = 0;
    }

    if (ch->pcdata->patrol_status == PATROL_GHOST_FOCUS || ch->pcdata->patrol_status == PATROL_SENSING_GHOST) {
      ch->pcdata->patrol_timer--;

      if (ch->pcdata->patrol_timer == 0)
      ch->pcdata->patrol_status = 0;
      if (ch->pcdata->patrol_timer == 2)
      send_to_char("Time is running out to banish the spirit, use `gpatrol banish`x to attempt it.\n\r", ch);
      else if (ch->in_room == ch->pcdata->patrol_room) {
        if (ch->pcdata->patrol_status == PATROL_GHOST_FOCUS)
        send_to_char("The spirit is here, use `gpatrol banish`x to attempt to banish it.\n\r", ch);
        else
        send_to_char("The spirit is here.\n\r", ch);
      }
      else
      printf_to_char(
      ch, "You sense the rising of a malevolent spirit to the %s.\n\r", relspacial[get_reldirection(
      roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(ch->pcdata->patrol_room), get_roomy(ch->pcdata->patrol_room)), ch->facing)]);
    }

    if (ch->pcdata->patrol_status == PATROL_HOLDING_ARTIFACT) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer == 2)
      send_to_char("Time is running out to break the curse, use `gpatrol decurse`x to attempt it.\n\r", ch);
      if (ch->pcdata->patrol_timer == 0) {
        send_to_char("The curse flares to life before fading away, rendering the object inert.\n\r", ch);
        ch->pcdata->patrol_status = 0;
      }
    }
    if (ch->pcdata->patrol_status == PATROL_SENSING_ARTIFACT) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0)
      ch->pcdata->patrol_status = 0;
      else if (ch->pcdata->patrol_target != NULL)
      printf_to_char(
      ch, "You sense the awakening of a cursed object to the %s.\n\r", relspacial[get_reldirection(
      roomdirection(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomx(ch->pcdata->patrol_target->in_room), get_roomy(ch->pcdata->patrol_target->in_room)), ch->facing)]);
    }
    if (ch->pcdata->patrol_status == PATROL_BANISHING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0) {
        ch->pcdata->patrol_timer = 1;
        ch->pcdata->patrol_timer = 0;
        ch->pcdata->patrol_did_arcane = TRUE;
        ch->pcdata->patrol_status = 0;
        patrol_room_award(ch, PATROL_GHOST);
        if(ch->pcdata->patrol_subtype > 0)
        {
          EXTRA_PATROL_TYPE *pat = epatrol_by_id(ch->pcdata->patrol_subtype);
          if(pat != NULL)
          {
            int counter = 0;
            for (vector<EXTRA_PATROL_TYPE *>::iterator it = EPatrolVect.begin();
            it != EPatrolVect.end(); ++it) {
              if ((*it)->valid == FALSE)
              continue;
              counter++;
            }

            act(pat->conclusion, ch, NULL, NULL,TO_CHAR);
            act(pat->conclusion, ch, NULL, NULL,TO_ROOM);
            logevent_check(ch, pat->conclusion);
            //            if(number_percent() % 5 == 0 && counter >= 10)
            //                 pat->valid = FALSE;
          }
        }
        else
        {
          logevent_check(ch, "With a rush of cold wind the spirit is banished.");

          act("With a rush of cold wind you banish the spirit.", ch, NULL, NULL, TO_CHAR);
          act("With a rush of cold wind you banish the spirit.", ch, NULL, NULL, TO_ROOM);
        }


      }
      else if (ch->pcdata->patrol_timer % 7 == 0)
      patrol_disturbance(ch, PATROL_GHOST);
    }
    if (ch->pcdata->patrol_status == PATROL_DECURSING) {
      ch->pcdata->patrol_timer--;
      ch->pcdata->patrol_did_arcane = TRUE;
      if (ch->pcdata->patrol_timer <= 0) {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        int rplvl = 0;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL)
          continue;

          if (!IS_NPC(fch)) {
            if (arcane_focus(fch) == 0)
            rplvl += fch->pcdata->patrol_rp * 2 / 3;
            else
            rplvl += fch->pcdata->patrol_rp;
            char buf[MSL];
            sprintf(buf, "PATROLDECURSERP: %s, %d.", fch->name, fch->pcdata->patrol_rp);
            log_string(buf);
          }
        }
        for (obj = ch->carrying; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;
          if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
          continue;

          if (obj->pIndexData->vnum != 36)
          continue;

          if (obj->level < 0) {
            obj->level = number_range(rplvl / 11, rplvl / 7);
            obj->level *= 2;
            if (pc_pop(ch->in_room) <= 1)
            obj->level = obj->level * 2 / 3;
            else if (only_faction(ch))
            obj->level /= 2;
            else if (only_allies(ch))
            obj->level = obj->level * 2 / 3;
            obj->level = UMIN(obj->level, 450);
            obj->level = obj->level * 3 / 2;
            char buf[MSL];
            sprintf(buf, "PATROLDECURSE: Final value %d", obj->level);
            log_string(buf);
            act("With a final burst of power you finish breaking the curse on $a $p.", ch, obj, NULL, TO_CHAR);
            act("With a final burst of power you finish breaking the curse on $n's $p.", ch, obj, NULL, TO_ROOM);
            char logs[MSL];
            sprintf(logs, "With a final burst of power the curse on %s is broken.", obj->description);
            ch->pcdata->patrol_timer = 1;
            logevent_check(ch, logs);
            ch->pcdata->patrol_timer = 0;

            for (CharList::iterator it = ch->in_room->people->begin();
            it != ch->in_room->people->end();) {
              CHAR_DATA *fch = *it;
              ++it;
              if (fch == NULL || IS_NPC(fch))
              continue;
              fch->pcdata->week_tracker[TRACK_PATROL_ARCANE]++;
              fch->pcdata->life_tracker[TRACK_PATROL_ARCANE]++;
            }
            return;
          }
        }
        ch->pcdata->patrol_status = 0;
      }
      else if (ch->pcdata->patrol_timer % 6 == 0)
      patrol_disturbance(ch, PATROL_ARTIFACT);
    }
    if (ch->pcdata->patrol_status == PATROL_BARGAINING) {
      int oldstat = ch->pcdata->patrol_status;
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0 && ch->pcdata->patrol_room == ch->in_room) {
        int factions[10];
        for (int i = 0; i < 10; i++)
        factions[i] = 0;

        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL)
          continue;
          if (IS_NPC(fch)) {
            fch->ttl = 1;
            continue;
          }

          if (fch->pcdata->patrol_pledged > 0 && fch->faction != 0) {
            bool found = FALSE;
            int firstzero = 0;
            for (int i = 0; i < 10; i++) {
              if (fch->faction == factions[i])
              found = TRUE;
              if (factions[i] == 0)
              firstzero = i;
            }
            if (found == FALSE)
            factions[firstzero] = fch->faction;
          }
        }
        int maxpledge = 0;
        int maxpledgevalue = 0;
        int maxfaction = 0;
        int totalpledge = 0;
        for (int i = 0; i < 10; i++) {
          if (factions[i] == 0)
          continue;
          int facpledge = 0;
          int facpledgevalue = 0;
          for (CharList::iterator it = ch->in_room->people->begin();
          it != ch->in_room->people->end();) {
            CHAR_DATA *fch = *it;
            ++it;
            if (fch == NULL || IS_NPC(fch))
            continue;

            if (fch->pcdata->patrol_pledged > 0 && fch->faction == factions[i] && fch->pcdata->patrol_rp >= 100) {
              facpledge += fch->pcdata->patrol_pledged;
              totalpledge += fch->pcdata->patrol_pledged;
              facpledgevalue += fch->pcdata->patrol_pledged *
              subfac_score(fch, fch->in_room->level) *
              get_lifeforce(fch, TRUE, NULL) / 100 *
              (UMIN(fch->pcdata->patrol_rp / 100, 10));
              if (prof_focus(fch) == 0) {
                facpledge = facpledge * 2 / 3;
                totalpledge = totalpledge * 2 / 3;
                facpledgevalue = facpledgevalue * 2 / 3;
              }
              int takinf = UMIN(2000, fch->pcdata->influence);
              takinf = UMIN(takinf, fch->pcdata->patrol_pledged);
              fch->pcdata->influence -= takinf;
              fch->pcdata->patrol_pledged -= takinf;
              fch->pcdata->super_influence -= fch->pcdata->patrol_pledged;
              fch->pcdata->patrol_pledged = 0;
              fch->pcdata->patrol_status = 0;
              fch->pcdata->patrol_timer = 0;
              fch->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC]++;
              fch->pcdata->life_tracker[TRACK_PATROL_DIPLOMATIC]++;
            }
          }
          if (facpledgevalue > maxpledgevalue) {
            maxfaction = factions[i];
            maxpledge = facpledge;
            maxpledgevalue = facpledgevalue;
          }
        }
        CHAR_DATA *pledger = NULL;

        ch->pcdata->patrol_status = 0;
        if (maxfaction > 0) {
          for (CharList::iterator it = ch->in_room->people->begin();
          it != ch->in_room->people->end();) {
            CHAR_DATA *fch = *it;
            ++it;
            if (fch == NULL || IS_NPC(fch))
            continue;
            if (fch->faction == maxfaction && fch->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC] > 0) {
              if (pledger == NULL || number_percent() % 2 == 0)
              pledger = fch;
            }
          }
          char buf[MSL];
          sprintf(buf, "The representative of %s decides to support %s.", get_subfac_name(ch->in_room->level), clan_lookup(maxfaction)->name);
          act(buf, ch, NULL, NULL, TO_CHAR);
          act(buf, ch, NULL, NULL, TO_ROOM);
          ch->pcdata->patrol_timer = 0;
          ch->pcdata->patrol_status = oldstat;
          logevent_check(ch, buf);
          ch->pcdata->patrol_timer = 0;
          ch->pcdata->patrol_status = 0;
          int val = UMAX(totalpledge, maxpledge * 2);
          val = val * 11 / 100;
          if (pledger == NULL) {
            val /= 2;
            sprintf(buf, "a deal with %s.", get_subfac_name(ch->in_room->level));
            val = UMIN(val, 1000);
            gain_resources(val, maxfaction, NULL, buf);
          }
          else {
            if (pc_pop(pledger->in_room) <= 1)
            val = val * 2 / 3;
            else if (only_faction(pledger))
            val = val / 2;
            else if (only_allies(pledger))
            val = val * 2 / 3;

            sprintf(buf, "%s making a deal with %s.", pledger->name, get_subfac_name(ch->in_room->level));

            val = UMIN(val, 1000);
            gain_resources(val, maxfaction, pledger, buf);
            pledger->pcdata->week_tracker[TRACK_CONTRIBUTED] += val * 10;
            pledger->pcdata->life_tracker[TRACK_CONTRIBUTED] += val * 10;
          }
        }
        else {
          act("The representative leaves.", ch, NULL, NULL, TO_ROOM);
          act("The representative leaves.", ch, NULL, NULL, TO_CHAR);
        }
        ch->in_room->level = 0;
      }
    }

    if (ch->pcdata->patrol_status == PATROL_ASSISTING_DECURSING || ch->pcdata->patrol_status == PATROL_ASSISTING_BANISHING) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0)
      ch->pcdata->patrol_status = 0;
    }

    if (in_fight(ch) && (ch->pcdata->patrol_status == PATROL_ATTACKASSISTING || ch->pcdata->patrol_status == PATROL_DEFENDASSISTING || ch->pcdata->patrol_status == PATROL_ATTACKSEARCHING || ch->pcdata->patrol_status == PATROL_ATTACKWAITING))
    ch->pcdata->patrol_status = PATROL_WAGINGWAR;
    if (!in_fight(ch) && ch->pcdata->patrol_status == PATROL_WAGINGWAR)
    ch->pcdata->patrol_status = 0;
    if (ch->pcdata->patrol_status == PATROL_WARMOVINGATTACK || ch->pcdata->patrol_status == PATROL_WARMOVINGDEFEND) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer == 1)
      send_to_char("You have 1 minute left to get to the conflict.\n\r", ch);
      if (ch->pcdata->patrol_timer <= 0) {
        ch->pcdata->patrol_status = 0;
        ch->pcdata->patrol_timer = 0;
      }
    }
    if (ch->pcdata->patrol_status == PATROL_ATTACKSEARCHING && !in_fight(ch)) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0) {
        if (ch->pcdata->patrol_target != NULL) {
          ch->pcdata->patrol_status = PATROL_WAGINGWAR;
          ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_WAGINGWAR;
          start_fight(ch, ch->pcdata->patrol_target);
        }
        ch->pcdata->patrol_timer = 0;
      }
    }
    if (ch->pcdata->patrol_status == PATROL_HUNTMOVING) {
      if (ch->in_room == ch->pcdata->patrol_room)
      ch->pcdata->patrol_status = PATROL_APPROACHINGHUNT;
      else {
        ch->pcdata->patrol_timer--;
        if (ch->pcdata->patrol_timer == 2)
        send_to_char("You have 2 minutes left to get to the hunt.\n\r", ch);
        if (ch->pcdata->patrol_timer <= 0) {
          ch->pcdata->patrol_status = 0;
          ch->pcdata->patrol_timer = 0;
        }
      }
    }
    if (ch->pcdata->patrol_status == PATROL_APPROACHINGHUNT) {
      ch->pcdata->patrol_timer--;
      if (ch->pcdata->patrol_timer <= 0) {
        ch->pcdata->patrol_did_hunting = TRUE;
        int pcpower = 0;
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
          continue;
          if (is_gm(to))
          continue;
          if (to->in_room == ch->in_room) {
            pcpower = pcpower * 93 / 100;
            pcpower += (char_power_score(to) / 130);
            send_to_char("You hear a rustling in the trees up ahead.\n\r", ch);
            if (to->pcdata->patrol_status == PATROL_HUNTMOVING || to->pcdata->patrol_status == PATROL_APPROACHINGHUNT) {
              to->pcdata->patrol_status = PATROL_HUNTING;
              to->pcdata->patrol_timer = 30;
              to->pcdata->week_tracker[TRACK_PATROL_HUNTING]++;
              to->pcdata->life_tracker[TRACK_PATROL_HUNTING]++;
            }
          }
        }
        int mobvnum = 0;
        int mobpower = 0;
        int humanoid = 0;
        for (int i = 0; i < 33; i++) {
          MOB_INDEX_DATA *pMobIndex;
          pMobIndex = get_mob_index(monster_table[i].vnum);
          if (mob_power_score(pMobIndex) / 100 > mobpower && mob_power_score(pMobIndex) / 100 <= pcpower && number_percent() % 2 == 0) {
            mobvnum = monster_table[i].vnum;
            mobpower = mob_power_score(pMobIndex) / 100;
            humanoid = monster_table[i].humanoid;
          }
        }
        ROOM_INDEX_DATA *spawnroom = aheadroom(ch->facing, ch->in_room);
        if (spawnroom == NULL || get_dist(ch->in_room->x, ch->in_room->y, spawnroom->x, spawnroom->y) >
            7 || ch->in_room->area->world != spawnroom->area->world) {
          int turn = turn_dir[ch->facing];
          turn = turn_dir[ch->facing];
          spawnroom = aheadroom(turn, ch->in_room);
        }
        if (spawnroom == NULL)
        spawnroom = ch->in_room;
        if (spawnroom == NULL)
        return;
        MOB_INDEX_DATA *pMobIndex;
        CHAR_DATA *mob;
        pMobIndex = get_mob_index(mobvnum);
        mob = create_mobile(pMobIndex);
        char_to_room(mob, spawnroom);
        mob->hit = max_hp(mob);
        free_string(mob->aggression);
        mob->aggression = str_dup("all");
        mob->x = 25;
        mob->y = 25;
        mob->recent_moved = 10;
        mob->facing = roomdirection(spawnroom->x, spawnroom->y, ch->x, ch->y);
        if (humanoid == 0)
        SET_FLAG(mob->act, ACT_VALUABLE);
        SET_FLAG(mob->act, ACT_BIGGAME);
        mob->valuable = TRUE;

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to))
          continue;
          if (to->in_room == ch->in_room) {
            act("$n comes into view through the trees up ahead.", mob, NULL, to, TO_VICT);
          }
        }
      }
    }
  }

  _DOFUN(do_patrol) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "patrol") || !str_cmp(arg1, "")) {
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      if (ch->pcdata->patrol_timer > 0) {
        send_to_char("You're in the middle of something.\n\r", ch);
        return;
      }
      if (ch->pcdata->patrol_status == 0) {
        if (!free_to_act(ch) || !in_haven(ch->in_room)) {
          send_to_char("You're a bit busy.\n\r", ch);
          return;
        }

        ch->pcdata->patrol_status = PATROL_PATROL;
        send_to_char("You go on patrol.\n\r", ch);
        return;
      }
      else {
        ch->pcdata->patrol_status = 0;
        send_to_char("You stop patrolling.\n\r", ch);
        return;
      }
    }

    if (!str_cmp(arg1, "prey")) {

      CHAR_DATA *to = get_char_world(ch, argument);

      if (to == NULL)
      to = get_char_world_pc(argument);


      if (ch->pcdata->account->predator_cool > current_time && (to == NULL || IS_NPC(to) || mist_level(to->in_room) < 3)) {
        send_to_char("You can't do that again yet.\n\r", ch);
        return;
      }

      if(get_tier(ch) < 2)
      {
        send_to_char("You need to be at least Tier 2 to do that.\n\r", ch);
        return;
      }

      if (to == NULL || IS_NPC(to)) {
        send_to_char("Prey not found.\n\r", ch);
        return;
      }

      if (!valid_pc_prey(to, ch, FALSE)) {
        send_to_char("Prey not found.\n\r", ch);
        return;
      }
      /*
if (ch->pcdata->patrol_habits[PATROL_RECKLESSHABIT] == 0 && !is_weakness(ch, to) && mist_level(to->in_room) < 3)
{
send_to_char("Prey not found.\n\r", ch);
return;
}
*/
      if(mist_level(to->in_room) < 3 && to->pcdata->account != NULL)
      ch->pcdata->account->predator_cool = current_time + (3600 * 24 * 6);
      else if(to->pcdata->account != NULL)
      ch->pcdata->account->predator_cool = current_time + (3600 * 4);

      if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD)) {
        REMOVE_FLAG(ch->act, PLR_SHROUD);
        printf_to_char(
        ch, "\n\n\nYou spot %s somewhere vulnerable, use `gpatrol stalk (message)`x to hunt them down, use `gpatrol observe`x to look at them.\n\r", PERS(to, ch));
        SET_FLAG(ch->act, PLR_SHROUD);
      }
      else if (!IS_FLAG(ch->act, PLR_SHROUD) && IS_FLAG(to->act, PLR_SHROUD)) {
        SET_FLAG(ch->act, PLR_SHROUD);
        printf_to_char(
        ch, "\n\n\nYou spot %s somewhere vulnerable, use `gpatrol stalk (message)`x to hunt them down, use `gpatrol observe`x to look at them.\n\r", PERS(to, ch));
        REMOVE_FLAG(ch->act, PLR_SHROUD);
      }
      else
      printf_to_char(
      ch, "\n\n\nYou spot %s somewhere vulnerable, use `gpatrol stalk (message)`x to hunt them down, use `gpatrol observe`x to look at them.\n\r", PERS(to, ch));
      ch->pcdata->patrol_target = to;
      ch->pcdata->patrol_status = PATROL_STALKING;
      ch->pcdata->patrol_timer = 5;
      WAIT_STATE(to, PULSE_PER_SECOND * 10);
      if (to->pcdata->account != NULL && mist_level(to->in_room) < 3)
      to->pcdata->account->prey_cool =
      UMAX(to->pcdata->account->prey_cool, current_time + 3600 * 24);
      else if(to->pcdata->account != NULL)
      to->pcdata->account->prey_cool =
      UMAX(to->pcdata->account->prey_cool, current_time + 60 * 15);

      return;
    }
    if (!str_cmp(arg1, "stalk")) {
      if (ch->pcdata->patrol_status == PATROL_STALKING) {
        if (ch->pcdata->patrol_target == NULL || !free_to_act(ch->pcdata->patrol_target) || pc_pop(ch->pcdata->patrol_target->in_room) > 1) {
          send_to_char("You missed your window.\n\r", ch);
          ch->pcdata->patrol_status = 0;
          return;
        }
        ROOM_INDEX_DATA *room = make_victim_room(ch->pcdata->patrol_target);
        if (room == NULL) {
          send_to_char("They are no longer vulnerable.\n\r", ch);
          ch->pcdata->patrol_status = 0;
          return;
        }
        CHAR_DATA *victim = ch->pcdata->patrol_target;
        if (victim->your_car != NULL)
        force_park(victim->your_car);

        if (ch->pcdata->account != NULL && mist_level(victim->in_room) < 3)
        ch->pcdata->account->last_pred = current_time;

        bool move = FALSE;
        if (room->vnum == 16490 || room->vnum == 90052 || room->vnum == 3438)
        move = TRUE;
        act("$n leaves.", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, room);
        act("$n leaves.", victim, NULL, NULL, TO_ROOM);
        char_from_room(victim);
        char_to_room(victim, room);
        ch->walking = 0;
        victim->walking = 0;
        if (IS_FLAG(ch->act, PLR_SHROUD))
        REMOVE_FLAG(ch->act, PLR_SHROUD);
        if (IS_FLAG(victim->act, PLR_SHROUD))
        REMOVE_FLAG(victim->act, PLR_SHROUD);

        ch->pcdata->patrol_status = PATROL_PREYING;
        ch->pcdata->patrol_timer = 10;
        victim->pcdata->patrol_status = PATROL_PREY;
        victim->pcdata->patrol_timer = 10;
        victim->pcdata->patrol_target = ch;
        ch->pcdata->patrol_target = victim;
        ch->pcdata->villain_option = VILLAIN_OPTION_WAITING;
        victim->pcdata->prey_option = PREY_OPTION_CHOOSING;
        victim->pcdata->prey_emote_cooldown = 40;
        victim->pcdata->prey_option_cooldown = 30;
        free_string(ch->pcdata->building_fixation_name);
        ch->pcdata->building_fixation_name = str_dup(victim->name);
        ch->pcdata->building_fixation_level = 0;
        char buf[MSL];
        sprintf(buf, "PATROLPRED: Ch: %s, vic: %s, chtarg: %s, victarg: %s", ch->name, victim->name, ch->pcdata->patrol_target->name, victim->pcdata->patrol_target->name);
        log_string(buf);
        if (victim->pcdata->account != NULL && mist_level(victim->in_room) < 3)
        victim->pcdata->account->prey_cool = current_time + (3600 * 24 * 5);

        act("You start to stalk $N.", ch, NULL, victim, TO_CHAR);
        act("$n starts to stalk you.", ch, NULL, victim, TO_VICT);
        send_to_char("[This is a predatory patrol event, you can RP this event anyway you please in any setting you wish as long as the goal of the predator is to capture the prey. It could be a chase through the woods, a pursuit through a nightclub, a negociation in a dark alley or anything in between. The event will end when the prey is either bound and helpess or escapes. When it is your turn for an action, you can use the chase command to pick your choices. Please allow the target first preference in setting the scene. They have up to 30 minutes to pick their first option and can use that extra time to setup the scene.]\n\r", ch);
        send_to_char("[This is a predatory patrol event, you can RP this event anyway you please in any setting you wish as long as the goal of the predator is to capture the prey. It could be a chase through the woods, a pursuit through a nightclub, a negociation in a dark alley or anything in between. The event will end when the prey is either bound and helpess or escapes. When it is your turn for an action, you can use the 'escape' command to pick your choices. Please either set the scene or make it clear you are deferring to the other person to do so. You are the first one to pick your options but have up to 30 minutes to do so and can use that extra time to setup the scene if you wish.]\n\r", victim);
        if(safe_strlen(argument) > 2)
        {
          printf_to_char(ch, "(%s)\n\r", argument);
          printf_to_char(victim, "(%s)\n\r", argument);
        }


        ch->pcdata->week_tracker[TRACK_PATROL_PREDATORY]++;
        ch->pcdata->life_tracker[TRACK_PATROL_PREDATORY]++;
        victim->pcdata->week_tracker[TRACK_PATROL_PREY]++;
        victim->pcdata->life_tracker[TRACK_PATROL_PREY]++;



        return;
      }
    }
    if (!str_cmp(arg1, "observe")) {
      if (ch->pcdata->patrol_status == PATROL_STALKING) {
        if (ch->pcdata->patrol_target == NULL || !free_to_act(ch->pcdata->patrol_target) || pc_pop(ch->pcdata->patrol_target->in_room) > 1) {
          send_to_char("You missed your window.\n\r", ch);
          ch->pcdata->patrol_status = 0;
          return;
        }
        show_char_to_char_1(ch->pcdata->patrol_target, ch, LOOK_LOOK, FALSE);
      }
    }
    if (!str_cmp(arg1, "bid")) {
      if (ch->pcdata->patrol_status == PATROL_BIDDING) {
        int amount = atoi(argument);
        if (amount <= ch->pcdata->patrol_amount) {
          send_to_char("Your bid has to be higher than your last bid.\n\r", ch);
          return;
        }
        if (amount + ch->pcdata->patrol_amount > ch->pcdata->total_money / 100) {
          send_to_char("You don't have that much cash in your account.\n\r", ch);
          return;
        }
        char buf[MSL];
        sprintf(buf, "$n bids $%d.", amount);
        act(buf, ch, NULL, NULL, TO_ROOM);
        printf_to_char(ch, "You bid $%d.\n\r", amount);
        ch->pcdata->patrol_amount = amount;
        return;
      }
    }
    if (!str_cmp(arg1, "bribe")) {
      if (ch->pcdata->patrol_status == PATROL_BIDDING && ch->pcdata->patrol_target != NULL) {
        ch->pcdata->total_money -= 2500;
        send_to_char("You slip a guard $25.\n\r", ch);
        switch (number_percent() % 3) {
        case 0:
          if (ch->pcdata->patrol_target->sex == SEX_FEMALE)
          send_to_char("They are female.\n\r", ch);
          else
          send_to_char("They are male.\n\r", ch);
          break;
        case 1:
          if (ch->pcdata->patrol_target->faction != 0)
          printf_to_char(ch, "They are part of %s.\n\r", clan_lookup(ch->pcdata->patrol_target->faction)->name);
          else if (ch->pcdata->patrol_target->sex == SEX_FEMALE)
          send_to_char("They are female.\n\r", ch);
          else
          send_to_char("They are male.\n\r", ch);
          break;
        case 2:
          printf_to_char(ch, "They have %s and %s.\n\r", get_hair(ch, ch->pcdata->patrol_target), get_eyes(ch, ch->pcdata->patrol_target));
          break;
        }
      }
    }
    if (!str_cmp(arg1, "collect")) {
      if (ch->pcdata->patrol_status == PATROL_COLLECTING && ch->pcdata->patrol_timer > 0) {
        CHAR_DATA *victim = NULL;
        for (CharList::iterator it = ch->pcdata->patrol_room->people->begin();
        it != ch->pcdata->patrol_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          victim = fch;
        }
        if (victim != NULL && in_haven(ch->in_room)) {
          char_from_room(victim);
          char_to_room(victim, ch->in_room);
          victim->walking = 0;
          ch->pcdata->total_money -= ch->pcdata->patrol_amount * 100;
          act("Some men deliver $N to you.", ch, NULL, victim, TO_CHAR);
          act("Some men deliver $N to $n.", ch, NULL, victim, TO_NOTVICT);
          act("Some men deliver you to $n.", ch, NULL, victim, TO_VICT);
        }
        victim = NULL;
        for (CharList::iterator it = ch->pcdata->patrol_room->people->begin();
        it != ch->pcdata->patrol_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          victim = fch;
        }
        if (victim != NULL) {
          char_from_room(victim);
          char_to_room(victim, ch->in_room);
          victim->walking = 0;
          act("Some men deliver $N to you as a bonus.", ch, NULL, victim, TO_CHAR);
          act("Some men deliver $N to $n.", ch, NULL, victim, TO_NOTVICT);
          act("Some men deliver you to $n.", ch, NULL, victim, TO_VICT);
        }
        ch->pcdata->patrol_status = 0;
        ch->pcdata->patrol_amount = 0;
        ch->pcdata->patrol_timer = 0;
      }
      else
      send_to_char("You haven't won any auctions recently.\n\r", ch);
    }

    if (!str_cmp(arg1, "influence")) {
      if (ch->pcdata->patrol_status == 0) {
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->pcdata->patrol_status == PATROL_BARGAINING) {
            ch->pcdata->patrol_status = PATROL_BARGAINING;
            ch->pcdata->patrol_timer = fch->pcdata->patrol_timer;
            ch->pcdata->patrol_pledged = 0;
            ch->pcdata->patrol_amount = 0;
          }
        }
      }
      if (ch->pcdata->patrol_status == PATROL_BARGAINING && ch->pcdata->patrol_timer > 0 && ch->pcdata->patrol_room == ch->in_room && ch->faction > 0) {
        int amount = atoi(argument);
        if (UMIN(2000, ch->pcdata->influence) + ch->pcdata->super_influence <
            amount + ch->pcdata->patrol_pledged) {
          send_to_char("You don't have that much influence.\n\r", ch);
          return;
        }
        ch->pcdata->patrol_amount = amount;
        printf_to_char(ch, "You pledge %d to the cause of getting the representative to make a deal with %s, you will have to do an emote before this will count.\n\r", amount, clan_lookup(ch->faction)->name);
        return;
      }
    }
    if (!str_cmp(arg1, "pledge")) {
      char arg2[MSL];
      argument = one_argument_nouncap(argument, arg2);
      if (ch->pcdata->patrol_status == 0) {
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->pcdata->patrol_status == PATROL_UNDER_ASSAULT) {
            ch->pcdata->patrol_status = PATROL_ASSISTING_UNDER_ASSAULT;
            ch->pcdata->patrol_timer = fch->pcdata->patrol_timer;
            ch->pcdata->patrol_target = fch->pcdata->patrol_target;
            ch->pcdata->patrol_pledged = 0;
          }
          else if (fch->pcdata->patrol_status == PATROL_LEADING_ASSAULT) {
            ch->pcdata->patrol_status = PATROL_ASSISTING_ASSAULT;
            ch->pcdata->patrol_timer = fch->pcdata->patrol_timer;
            ch->pcdata->patrol_target = fch->pcdata->patrol_target;
            ch->pcdata->patrol_pledged = 0;
          }
        }
      }
      if (ch->pcdata->patrol_status == PATROL_LEADING_ASSAULT || ch->pcdata->patrol_status == PATROL_UNDER_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_UNDER_ASSAULT) {
        int amount = atoi(arg2);
        if (base_lifeforce(ch) - (amount + ch->pcdata->patrol_pledged) * 100 <
            8000) {
          send_to_char("That would take you too low on life force.\n\r", ch);
          return;
        }
        ch->pcdata->patrol_pledged += amount;
        if (safe_strlen(argument) > 2) {
          act(argument, ch->pcdata->patrol_target, NULL, NULL, TO_ROOM);
          act(argument, ch->pcdata->patrol_target, NULL, NULL, TO_CHAR);
          printf_to_char(ch, "You spend %d more life force on the ritual, sending %s across to the enemy.\n\r", amount, argument);
          logevent_check(ch, argument);
        }
        else
        printf_to_char(ch, "You spend %d more life force on the ritual.\n\r", amount);
      }
    }

    if (!str_cmp(arg1, "decurse")) {
      if (ch->pcdata->patrol_status == PATROL_HOLDING_ARTIFACT && ch->pcdata->patrol_timer > 0) {
        int pop = 0;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->pcdata->patrol_status == PATROL_SENSING_ARTIFACT)
          pop++;
          fch->pcdata->patrol_rp = 0;
        }
        if (pop < 2 && ch->pcdata->patrol_timer > 3) {
          send_to_char("You need at least two other arcanists who sensed the object's awakening curse to decurse it, or you can wait longer.\n\r", ch);
          return;
        }
        ch->pcdata->patrol_status = PATROL_DECURSING;
        ch->pcdata->patrol_timer = 30;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch == ch)
          continue;
          if (free_to_act(fch)) {
            fch->pcdata->patrol_status = PATROL_ASSISTING_DECURSING;
            fch->pcdata->patrol_timer = 30;
            fch->pcdata->patrol_target = ch;
            act("You get ready to assist $N in their attempts to break the artifact's curse.", fch, NULL, ch, TO_CHAR);
          }
        }
        send_to_char("You start to decurse the item.\n\r", ch);
        return;
      }
    }
    if (!str_cmp(arg1, "banish")) {
      if (ch->pcdata->patrol_status == PATROL_GHOST_FOCUS && ch->pcdata->patrol_timer > 0 && ch->in_room == ch->pcdata->patrol_room) {
        int pop = 0;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->pcdata->patrol_status == PATROL_SENSING_GHOST)
          pop++;
          fch->pcdata->patrol_rp = 0;
        }
        if (pop < 2 && ch->pcdata->patrol_timer > 2) {
          send_to_char("You need at least two others who sensed the rising spirit to banish it or to wait longer.\n\r", ch);
          return;
        }
        ch->pcdata->patrol_status = PATROL_BANISHING;
        ch->pcdata->patrol_timer = 45;
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch == ch)
          continue;
          if (free_to_act(fch)) {
            fch->pcdata->patrol_status = PATROL_ASSISTING_BANISHING;
            fch->pcdata->patrol_timer = 45;
            fch->pcdata->patrol_target = ch;
            act("You get ready to assist $N in their attempts to banish the spirit.", fch, NULL, ch, TO_CHAR);
          }
        }
        send_to_char("You start to banish the spirit.\n\r", ch);
        if(ch->pcdata->patrol_subtype > 0)
        {
          EXTRA_PATROL_TYPE *pat = epatrol_by_id(ch->pcdata->patrol_subtype);
          if(pat != NULL)
          {
            act(pat->intro, ch, NULL, NULL, TO_ROOM);
            act(pat->intro, ch, NULL, NULL, TO_CHAR);
            logevent_check(ch, pat->intro);
          }

        }
        return;
      }
    }
    if (!str_cmp(arg1, "wait")) {
      if (ch->pcdata->patrol_status == PATROL_ATTACKSEARCHING) {
        ch->pcdata->patrol_status = PATROL_ATTACKWAITING;
        send_to_char("You stop actively searching for your target in the nightmare, content with conversing for now.\n\r", ch);
      }
    }
    if (!str_cmp(arg1, "search")) {
      if (ch->pcdata->patrol_status == PATROL_ATTACKWAITING) {
        ch->pcdata->patrol_status = PATROL_ATTACKSEARCHING;
        send_to_char("You resume searching for you target in the nightmare.\n\r", ch);
      }
    }
    if (!str_cmp(arg1, "attack")) {
      if (ch->pcdata->patrol_status == PATROL_DEFENDHIDING) {
        send_to_char("You emerge from hiding to begin the conflict.\n\r", ch);
        ch->pcdata->patrol_status = PATROL_WAGINGWAR;
        ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_WAGINGWAR;
        start_fight(ch->pcdata->patrol_target, ch);
      }
    }
    if (!str_cmp(arg1, "engage")) {
      if (ch->pcdata->patrol_status == PATROL_WARMOVINGATTACK && ch->pcdata->patrol_timer > 0) {
        if (!free_to_act(ch)) {
          send_to_char("Not now.\n\r", ch);
          return;
        }
        if (!can_shroud(ch)) {
          send_to_char("You can't enter the nightmare.\n\r", ch);
          return;
        }
        act("You fall asleep.", ch, NULL, NULL, TO_CHAR);
        act("$n falls asleep.", ch, NULL, NULL, TO_ROOM);
        ch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
        ch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
        villain_mod(ch, 10, "Defending faction mate");

        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch == ch)
          continue;
          if (fch->master == ch && fch->position > POS_SITTING && !in_fight(fch) && !is_helpless(fch) && can_shroud(fch)) {
            act("You fall asleep.", fch, NULL, ch, TO_CHAR);
            to_spectre(fch, TRUE);
            char_from_room(fch);
            char_to_room(fch, ch->pcdata->patrol_room);
            fch->walking = 0;
            if (!IS_FLAG(fch->act, PLR_SHROUD))
            SET_FLAG(fch->act, PLR_SHROUD);
            if (!IS_FLAG(fch->act, PLR_DEEPSHROUD))
            SET_FLAG(fch->act, PLR_DEEPSHROUD);
            fch->pcdata->patrol_status = PATROL_ATTACKASSISTING;
            fch->pcdata->patrol_room = ch->pcdata->patrol_room;
            fch->pcdata->patrol_timer = 0;
            fch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
            fch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
            villain_mod(fch, 5, "Assisting defense");
          }
        }
        to_spectre(ch, TRUE);
        char_from_room(ch);
        char_to_room(ch, ch->pcdata->patrol_room);
        ch->walking = 0;
        act("$n arrives.", ch, NULL, NULL, TO_CHAR);
        ch->pcdata->patrol_status = PATROL_ATTACKASSISTING;
        ch->pcdata->patrol_timer = 0;
        if (!IS_FLAG(ch->act, PLR_SHROUD))
        SET_FLAG(ch->act, PLR_SHROUD);
        if (!IS_FLAG(ch->act, PLR_DEEPSHROUD))
        SET_FLAG(ch->act, PLR_DEEPSHROUD);
      }
      if (ch->pcdata->patrol_status == PATROL_WARMOVINGDEFEND && ch->pcdata->patrol_timer > 0) {
        if (!free_to_act(ch)) {
          send_to_char("Not now.\n\r", ch);
          return;
        }
        if (!can_shroud(ch)) {
          send_to_char("You can't enter the nightmare.\n\r", ch);
          return;
        }
        dact("$n falls asleep.", ch, NULL, NULL, DISTANCE_MEDIUM);
        send_to_char("You fall asleep.\n\r", ch);
        ch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
        ch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
        villain_mod(ch, 10, "Assisting attacker");

        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->master == ch && fch->position > POS_SITTING && !in_fight(fch) && !is_helpless(fch) && can_shroud(fch)) {
            act("You fall asleep.", fch, NULL, ch, TO_CHAR);
            to_spectre(fch, TRUE);
            char_from_room(fch);
            char_to_room(fch, ch->pcdata->patrol_room);
            fch->walking = 0;

            if (!IS_FLAG(fch->act, PLR_SHROUD))
            SET_FLAG(fch->act, PLR_SHROUD);
            if (!IS_FLAG(fch->act, PLR_DEEPSHROUD))
            SET_FLAG(fch->act, PLR_DEEPSHROUD);
            fch->pcdata->patrol_status = PATROL_DEFENDASSISTING;
            fch->pcdata->patrol_room = ch->pcdata->patrol_room;
            fch->pcdata->patrol_timer = 0;
            fch->pcdata->week_tracker[TRACK_PATROL_WARFARE]++;
            fch->pcdata->life_tracker[TRACK_PATROL_WARFARE]++;
            villain_mod(fch, 3, "Following defender");
          }
        }
        to_spectre(ch, TRUE);
        char_from_room(ch);
        char_to_room(ch, ch->pcdata->patrol_room);
        ch->walking = 0;
        act("$n arrives.", ch, NULL, NULL, TO_CHAR);
        ch->pcdata->patrol_status = PATROL_DEFENDASSISTING;
        ch->pcdata->patrol_timer = 0;
        if (!IS_FLAG(ch->act, PLR_SHROUD))
        SET_FLAG(ch->act, PLR_SHROUD);
        if (!IS_FLAG(ch->act, PLR_DEEPSHROUD))
        SET_FLAG(ch->act, PLR_DEEPSHROUD);
      }
    }
    if (!str_cmp(arg1, "approach")) {
      if (ch->pcdata->patrol_status == PATROL_HUNTMOVING && ch->pcdata->patrol_timer > 0) {
        if (ch->in_room == NULL || ch->in_room->area->vnum < 16 || ch->in_room->area->vnum > 21) {
          send_to_char("You need to be in the forest first.\n\r", ch);
          return;
        }
        if (ch->in_room->sector_type != SECT_FOREST) {
          send_to_char("You need to be in the forest first.\n\r", ch);
          return;
        }

        if (is_gm(ch) || is_helpless(ch) || in_fight(ch) || IS_FLAG(ch->act, PLR_DEAD) || ch->in_room == NULL || locked_room(ch->in_room, ch) || !in_haven(ch->in_room) || room_hostile(ch->in_room) || is_pinned(ch)) {
          send_to_char("You can't do that right now.\n\r", ch);
          return;
        }
        act("$n heads deeper into the forest.", ch, NULL, NULL, TO_ROOM);
        for (CharList::iterator it = ch->in_room->people->begin();
        it != ch->in_room->people->end();) {
          CHAR_DATA *fch = *it;
          ++it;
          if (fch == NULL || IS_NPC(fch))
          continue;
          if (fch->master == ch && fch->position > POS_SITTING && !in_fight(fch) && !is_helpless(fch)) {
            act("You follow $N.", fch, NULL, ch, TO_CHAR);
            char_from_room(fch);
            char_to_room(fch, ch->pcdata->patrol_room);
            fch->facing = ch->facing;
            fch->walking = 0;
          }
        }
        char_from_room(ch);
        char_to_room(ch, ch->pcdata->patrol_room);
        ch->walking = 0;
        act("$n arrives.", ch, NULL, NULL, TO_ROOM);
        send_to_char("You arrive a little distance from where the creature is suppose to be to wait for it to show itself.\n\r", ch);
        ch->pcdata->patrol_status = PATROL_APPROACHINGHUNT;
        return;
      }
    }
  }

  bool valid_patrol_room(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    if (room->area->vnum == 1)
    return TRUE;
    if (room->area->vnum >= 22)
    return FALSE;
    if (IS_SET(room->room_flags, ROOM_INDOORS) && number_percent() % 2 == 0)
    return FALSE;
    if (prop_from_room(room) != NULL && room->sector_type != SECT_ROOFTOP && room->sector_type != SECT_AIR && number_percent() % 2 == 0)
    return FALSE;
    if (deep_water(ch))
    return FALSE;
    return TRUE;
  }

  bool predator_launch(CHAR_DATA *ch) {
    if (!valid_patrol_room(ch, ch->in_room))
    return FALSE;
    if (IS_FLAG(ch->act, PLR_SHROUD))
    return FALSE;
    if (IS_FLAG(ch->act, PLR_HIDE))
    return FALSE;
    if (is_animal(ch) && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <= ANIMAL_SMALL)
    return FALSE;
    if (clinic_patient(ch))
    return FALSE;
    if (ch->your_car != NULL) {
      if (ch->your_car->in_room == ch->pcdata->last_patrol)
      return FALSE;
      else
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if (number_percent() % 3 == 0) {
      launch_patrolevent(ch, PATROL_PREDATOR);
      return TRUE;
    }
    return FALSE;
  }
  bool war_launch(CHAR_DATA *ch) {
    if (ch->faction == 0)
    return FALSE;
    if (!valid_patrol_room(ch, ch->in_room))
    return FALSE;
    if (!can_shroud(ch))
    return FALSE;
    if (ch->your_car != NULL) {
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if (number_percent() % 8 == 0) {
      launch_patrolevent(ch, PATROL_WAR);
      return TRUE;
    }
    return FALSE;
  }

  bool hunt_launch(CHAR_DATA *ch) {
    if (ch->faction == 0 && number_percent() % 6 != 0)
    return FALSE;
    if (!valid_patrol_room(ch, ch->in_room))
    return FALSE;
    if (ch->in_room == ch->pcdata->last_patrol)
    return FALSE;
    if (ch->your_car != NULL) {
      if (ch->your_car->in_room == ch->pcdata->last_patrol)
      return FALSE;
      else
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if ((ch->in_room->area->vnum >= 16 && ch->in_room->area->vnum <= 21) || number_range(1, 10000) % 87 == 0) {
      if (number_percent() % 3 == 0) {
        int pop = 0;
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character == NULL || d->connected != CON_PLAYING)
          continue;
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to) || to == ch)
          continue;
          if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
          continue;
          if (IS_FLAG(to->comm, COMM_AFK))
          continue;
          if (to->pcdata->availability == AVAIL_LOW)
          continue;
          if (get_gm(to->in_room, FALSE) != NULL)
          continue;
          if (IS_FLAG(to->comm, COMM_PRIVATE))
          continue;
          if (is_gm(to) || is_helpless(to) || in_fight(to) || IS_FLAG(to->act, PLR_DEAD) || to->in_room == NULL || locked_room(to->in_room, to) || !in_haven(to->in_room) || room_hostile(to->in_room) || is_pinned(to))
          continue;
          if (to->pcdata->patrol_habits[PATROL_HUNTHABIT] > 0) {
            pop++;
          }
        }
        if (pop >= 1) {
          if (ch->pcdata->account != NULL && ch->pcdata->account->last_hunt != 0 && ch->pcdata->account->last_hunt > current_time - (3600 * 24 * 3))
          return FALSE;
          else if (ch->pcdata->account != NULL)
          ch->pcdata->account->last_hunt = current_time;
          launch_patrolevent(ch, PATROL_HUNT);
          if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDHUNT))
          SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDHUNT);

          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool diplomatic_launch(CHAR_DATA *ch) {
    if (ch->faction == 0 && number_percent() % 29 != 0)
    return FALSE;
    if (!cell_signal(ch))
    return FALSE;
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID)
    return FALSE;
    if (ch->your_car != NULL) {
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if (time_info.minute % 10 == 0) {
      send_to_char("You do some business on your cell phone.\n\r", ch);
      act("$n does something on $s cell phone.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
    }

    if (number_percent() % 13 == 0) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character == NULL || d->connected != CON_PLAYING)
        continue;
        to = d->character;
        if (IS_FLAG(to->comm, COMM_AFK))
        continue;
        if (!free_to_act(to))
        continue;
        if (same_faction(ch, to))
        continue;
        if (IS_FLAG(to->comm, COMM_PRIVATE))
        continue;
        if (to->pcdata->availability == AVAIL_LOW)
        continue;
        if (get_gm(to->in_room, FALSE) != NULL)
        continue;

        if (to->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] > 0 || (to->faction != 0 && prof_focus(to) > 0)) {
          if (ch->pcdata->account != NULL && ch->pcdata->account->last_bargain != 0 && ch->pcdata->account->last_bargain > current_time - (3600 * 24 * 12))
          return FALSE;
          else if (ch->pcdata->account != NULL)
          ch->pcdata->account->last_bargain = current_time;
          launch_patrolevent(ch, PATROL_BARGAIN);
          if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO))
          SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO);
          return TRUE;
        }
      }
    }
    int pop = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (!free_to_act(to))
      continue;
      if (IS_FLAG(to->comm, COMM_AFK))
      continue;
      if (IS_FLAG(to->comm, COMM_PRIVATE))
      continue;
      if (to->pcdata->availability == AVAIL_LOW)
      continue;
      if (!same_faction(ch, to))
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      if (ch == to)
      continue;
      pop++;
    }
    if (pop > 1 && number_percent() % 21 == 0) {
      if (ch->pcdata->account != NULL && ch->pcdata->account->last_bribe != 0 && ch->pcdata->account->last_bribe > current_time - (3600 * 24 * 12))
      return FALSE;
      else if (ch->pcdata->account != NULL)
      ch->pcdata->account->last_bribe = current_time;

      launch_patrolevent(ch, PATROL_BRIBE);
      if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO))
      SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO);

      return TRUE;
    }

    if (number_percent() % 76 == 0 && pop > 1) {
      if (pc_pop(get_room_index(ROOM_MEETING_EAST)) <= 0 || pc_pop(get_room_index(ROOM_MEETING_WEST)) <= 0) {
        if (prof_focus(ch) > 0) {
          launch_patrolevent(ch, PATROL_KIDNAP);
          if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO))
          SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO);
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool arcanewar_launch(CHAR_DATA *ch) {
    if (ch->faction == 0)
    return FALSE;

    if (!valid_patrol_room(ch, ch->in_room) && number_percent() % 5 != 0)
    return FALSE;

    if (ch->in_room == ch->pcdata->last_patrol && number_percent() % 2 != 0)
    return FALSE;

    if (ch->your_car != NULL) {
      if (ch->your_car->in_room == ch->pcdata->last_patrol && number_percent() % 5 != 0)
      return FALSE;
      else
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if (time_info.minute % 4 == 0 && ch->pcdata->patrol_habits[PATROL_ARCANEHABIT] == 0) {
      send_to_char("You close your eyes for a few seconds and focus on your arcane senses.\n\r", ch);
      act("$n closes $s eyes for a few seconds.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);
    }

    if (number_percent() % 5 == 0 && base_lifeforce(ch) >= 9000) {
      launch_patrolevent(ch, PATROL_ARCANEWAR);
      return TRUE;
    }

    return FALSE;
  }

  bool arcane_launch(CHAR_DATA *ch) {
    if (ch->faction == 0 && number_percent() % 29 != 0)
    return FALSE;

    if (!valid_patrol_room(ch, ch->in_room) && number_percent() % 5 != 0)
    return FALSE;

    if (ch->in_room == ch->pcdata->last_patrol && number_percent() % 2 != 0)
    return FALSE;

    if (ch->your_car != NULL) {
      if (ch->your_car->in_room == ch->pcdata->last_patrol && number_percent() % 5 != 0)
      return FALSE;
      else
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if (time_info.minute % 4 == 0) {
      send_to_char("You close your eyes for a few seconds and focus on your arcane senses.\n\r", ch);
      act("$n closes $s eyes for a few seconds.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);
    }

    int pop = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to) || to == ch)
      continue;
      if (IS_FLAG(to->comm, COMM_AFK))
      continue;
      if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      if (!free_to_act(to))
      continue;
      if (to->pcdata->availability == AVAIL_LOW)
      continue;
      if (IS_FLAG(to->comm, COMM_PRIVATE))
      continue;
      if (to->pcdata->patrol_habits[PATROL_ARCANEHABIT] > 0 || (to->faction != 0 && arcane_focus(to) > 0)) {
        pop++;
      }
    }

    if (pop > 2 && number_percent() % 3 == 0) {
      if (ch->pcdata->account != NULL && ch->pcdata->account->last_decurse != 0 && ch->pcdata->account->last_decurse > current_time - (3600 * 24 * 12))
      return FALSE;
      else if (ch->pcdata->account != NULL)
      ch->pcdata->account->last_decurse = current_time;

      launch_patrolevent(ch, PATROL_ARTIFACT);
      if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANE))
      SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANE);

      return TRUE;
    }


    return FALSE;
  }



  bool spirit_launch(CHAR_DATA *ch) {
    if (ch->faction == 0 && number_percent() % 29 != 0)
    return FALSE;

    if (!valid_patrol_room(ch, ch->in_room) && number_percent() % 5 != 0)
    return FALSE;

    if (ch->in_room == ch->pcdata->last_patrol && number_percent() % 2 != 0)
    return FALSE;

    if (ch->your_car != NULL) {
      if (ch->your_car->in_room == ch->pcdata->last_patrol && number_percent() % 5 != 0)
      return FALSE;
      else
      ch->pcdata->last_patrol = ch->your_car->in_room;
    }
    else
    ch->pcdata->last_patrol = ch->in_room;

    if (time_info.minute % 4 == 0) {
      send_to_char("You close your eyes for a few seconds to focus.\n\r", ch);
      act("$n closes $s eyes for a few seconds.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 5);
    }

    int pop = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;
      to = d->character;
      if (IS_NPC(to) || to->in_room == NULL || !in_haven(to->in_room) || is_helpless(to) || to == ch)
      continue;
      if (IS_FLAG(to->comm, COMM_AFK))
      continue;
      if (to->pcdata->patrol_status != 0 && to->pcdata->patrol_status != PATROL_PATROL)
      continue;
      if (get_gm(to->in_room, FALSE) != NULL)
      continue;
      if (!free_to_act(to))
      continue;
      if (to->pcdata->availability == AVAIL_LOW)
      continue;
      if (IS_FLAG(to->comm, COMM_PRIVATE))
      continue;
      if (to->pcdata->patrol_habits[PATROL_SPIRITHABIT] > 0 || (to->fsect != 0)) {
        pop++;
      }
    }

    if (pop > 2 && number_percent() % 3 == 0) {
      if (ch->pcdata->account != NULL && ch->pcdata->account->last_ghost != 0 && ch->pcdata->account->last_ghost > current_time - (3600 * 24 * 12))
      return FALSE;
      else if (ch->pcdata->account != NULL)
      ch->pcdata->account->last_ghost = current_time;

      launch_patrolevent(ch, PATROL_GHOST);
      if (ch->pcdata->account != NULL && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDSPIRIT))
      {
        if(number_percent() % 2 == 0 || ch->fsect == 0)
        SET_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDSPIRIT);
      }
      return TRUE;
    }

    return FALSE;
  }

  void patrol_launch_check(CHAR_DATA *ch) {
    if (guestmonster(ch) || higher_power(ch))
    return;
    if (ch->pcdata->patrol_status != PATROL_PATROL)
    return;
    if (ch->master != NULL && ch->in_room == ch->master->in_room)
    return;
    if (!free_to_act(ch))
    return;

    char buf[MSL];
    if (ch->pcdata->patrol_habits[PATROL_ARCANEHABIT] > 0 && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANE)) {
      if (arcane_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: Arcane: %s", ch->name);
        log_string(buf);
        return;
      }
    }

    if (ch->pcdata->patrol_habits[PATROL_SPIRITHABIT] > 0 && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDSPIRIT)) {
      if (spirit_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: Spirit: %s", ch->name);
        log_string(buf);
        return;
      }
    }

    if (ch->pcdata->patrol_habits[PATROL_DIPLOMATICHABIT] > 0 && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDDIPLO)) {
      if (diplomatic_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: Diplomatic: %s", ch->name);
        log_string(buf);
        return;
      }
    }
    if (ch->pcdata->patrol_habits[PATROL_HUNTHABIT] > 0 && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDHUNT)) {
      if (hunt_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: Hunt: %s", ch->name);
        log_string(buf);
        return;
      }
    }
    if (ch->pcdata->patrol_habits[PATROL_WARHABIT] > 0) {
      sprintf(buf, "PATROLCHECK: War: %s", ch->name);
      log_string(buf);
      if (war_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: War: %s", ch->name);
        log_string(buf);
        return;
      }
    }
    if (ch->pcdata->patrol_habits[PATROL_ARCANEWARHABIT] > 0 && !IS_FLAG(ch->pcdata->account->flags, ACCOUNT_DIDARCANEWAR)) {
      if (arcanewar_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: Arcanewar: %s", ch->name);
        log_string(buf);
        return;
      }
    }
    if (ch->pcdata->patrol_habits[PATROL_PREDATORHABIT] > 0) {
      if (predator_launch(ch)) {
        sprintf(buf, "PATROLLAUNCH: Predator: %s", ch->name);
        log_string(buf);
        return;
      }
    }
  }

  _DOFUN(do_squirm) {
    if (ch->pcdata->patrol_status == PATROL_GRABBED) {
      if (ch->pcdata->patrol_target->pcdata->patrol_timer > 15 || number_percent() % 2 != 0) {
        act("You squirm uselessly in $N's grasp.", ch, NULL, ch->pcdata->patrol_target, TO_CHAR);
        act("$n squirms uselessly in your grasp.", ch, NULL, ch->pcdata->patrol_target, TO_VICT);
        logevent_check(ch, logact("$n squirms uselessly in $N's grasp.", ch, ch->pcdata->patrol_target));
        WAIT_STATE(ch, PULSE_PER_SECOND * 10);
        return;
      }
      else {
        OBJ_DATA *obj;
        int iWear;
        OBJ_DATA *remove = NULL;
        for (iWear = MAX_WEAR - 1; iWear >= 0 && remove == NULL; iWear--) {
          if ((obj = get_eq_char(ch, iWear)) != NULL && obj->item_type == ITEM_CLOTHING && can_see_obj(ch->pcdata->patrol_target, obj) && (can_see_wear(ch, iWear) && number_percent() % 2 == 0)) {
            remove = obj;
          }
        }
        if (remove != NULL) {
          obj_from_char(remove);
          obj_to_char(remove, ch->pcdata->patrol_target);
          act("You squirm free of $N's grasp, losing $a $p in the process. You can attempt to either flee or hide, or attack $N.", ch, remove, ch->pcdata->patrol_target, TO_CHAR);
          act("$n's $p comes away in your hand as $e squirms free of your grasp.", ch, remove, ch->pcdata->patrol_target, TO_VICT);
        }
        else {
          act("You squirm free of $N's grasp. You can attempt to either flee or hide, or attack $N", ch, NULL, ch->pcdata->patrol_target, TO_CHAR);
          act("$n squirms free of your grasp.", ch, NULL, ch->pcdata->patrol_target, TO_VICT);
        }
        logevent_check(ch, logact("$n squirms free of $N's grasp.", ch, ch->pcdata->patrol_target));

        ch->pcdata->patrol_status = PATROL_PREY;
        ch->pcdata->patrol_timer = 60;
        ch->pcdata->patrol_target->pcdata->patrol_status = PATROL_PREYING;
        ch->pcdata->patrol_target->pcdata->patrol_timer = 5;
      }
    }
  }

  vector<WEEKLY_TYPE *> WeeklyVect;
  WEEKLY_TYPE *nullweekly;

  void save_weekly_characters() {
    FILE *fpout;
    if ((fpout = fopen("../data/weeklycharacters.txt", "w")) == NULL) {
      bug("Cannot open weeklycharacters.txt for writing", 0);
      return;
    }

    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      fprintf(fpout, "#Weekly\n");
      fprintf(fpout, "Character %s~\n", (*it)->charname);
      fprintf(fpout, "Surname %s~\n", (*it)->surname);
      fprintf(fpout, "Logon %d\n", (*it)->logon);

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void fread_weekly_character(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    WEEKLY_TYPE *weekly;

    weekly = new_weekly();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'C':
        KEY("Character", weekly->charname, fread_string(fp));
        break;
      case 'L':
        KEY("Logon", weekly->logon, fread_number(fp));
        break;
      case 'S':
        KEY("Surname", weekly->surname, fread_string(fp));
        break;

      case 'E':
        if (!str_cmp(word, "End")) {
          WeeklyVect.push_back(weekly);
          return;
        }
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_weekly_characters: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_weekly_characters() {
    nullweekly = new_weekly();
    FILE *fp;

    if ((fp = fopen("../data/weeklycharacters.txt", "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;
        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Weekly_characters: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "Weekly")) {
          fread_weekly_character(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_weekly_characters: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open weeklycharacters.txt", 0);
      exit(0);
    }
  }

  void add_to_weekly_characters(CHAR_DATA *ch) {
    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (!str_cmp((*it)->charname, ch->name))
      {
        (*it)->logon = current_time;
        return;

      }
    }
    WEEKLY_TYPE *weekly = new_weekly();
    free_string(weekly->charname);
    weekly->charname = str_dup(ch->name);
    free_string(weekly->surname);
    weekly->surname = str_dup(ch->pcdata->last_name);
    WeeklyVect.push_back(weekly);
  }

  void villain_update(char *name, int amount) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    struct stat sb;
    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {

      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    if (victim->pcdata->account == NULL)
    victim->pcdata->account = reload_account(victim->pcdata->account_name);
    victim->pcdata->account->villain_score += amount;
    victim->pcdata->account->villain_score = UMIN(amount, 15);
    victim->pcdata->account->villain_score = UMAX(amount, -15);
    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  char *focus_name(int stat) {
    if (stat == SKILL_PRIMALFOCUS)
    return "Primal Warrior";
    if (stat == SKILL_ILLUSORYMAGICFOCUS)
    return "Illusory Magic Mancer";
    if (stat == SKILL_NATURALMAGICFOCUS)
    return "Natural Magic Mancer";
    if (stat == SKILL_DARKMAGICFOCUS)
    return "Dark Magic Mancer";
    if (stat == SKILL_ELEMAGICFOCUS)
    return "Elemental Magic Mancer";
    for (int i = 0; i < SKILL_USED; i++) {
      if (skill_table[i].vnum == stat) {
        return skill_table[i].name;
      }
    }
    return "";
  }

  void weekly_process(void) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    FACTION_TYPE *fac;
    int most_char_influence = 0;
    char mcinfname[MSL];
    char mcinfmessage[MSL];
    int most_app_influence = 0;
    char mainfname[MSL];
    char mainfmessage[MSL];
    int most_rumors = 0;
    char mrumorname[MSL];
    char mrumormessage[MSL];
    int most_event_pop = 0;
    char mepopname[MSL];
    int most_event_attend = 0;
    char meattendname[MSL];
    int hottest_man_number = 0;
    char hotmanname[MSL];
    int hottest_woman_number = 0;
    char hotwomanname[MSL];
    int most_trees_chopped = 0;
    char treechoppername[MSL];
    int max_nongeneric[40];
    int max_nongeneric_point[40];
    int most_money = 0;
    char moneyname[MSL];
    int most_influence = 0;
    char mostinfluencename[MSL];
    int luckiest = 0;
    char luckiestname[MSL];
    int pstat = 0;
    int pstatlevel = 0;
    char pstatname[MSL];
    int ftype = 0;
    int ftypelevel = 0;
    int herolevel = 0;
    char heroname[MSL];
    char ftypename[MSL];
    char villains[50][30];
    char villain_praise_message[50][MSL];
    char villain_diss_message[50][MSL];
    int villain_scores[50];
    int villain_praise[50];
    int villain_tier[50];
    int day = get_day();
    int villainpraisepointer = -1;
    int villaindisspointer = -1;
    char reckless[MSL];
    char intelvalue[MSL];
    sprintf(reckless, "%s", "");
    sprintf(intelvalue, "%s", "");
    int maxf_one = 0;
    int maxf_two = 0;
    int maxf_three = 0;
    char iname_one[MSL];
    char iname_two[MSL];
    char iname_three[MSL];
    sprintf(iname_one, "A");
    sprintf(iname_two, "B");
    sprintf(iname_three, "C");
    char logs[MSL];

    time_info.lweek_tier = time_info.tweek_tier;
    time_info.lweek_total = time_info.tweek_total;
    time_info.tweek_tier = 0;
    time_info.tweek_total = 0;

    if (number_percent() % 3 == 0)
    ftype = STYPE_PROFFOCUS;
    else if (number_percent() % 2 == 0)
    ftype = STYPE_ARCANEFOCUS;
    else
    ftype = STYPE_COMBATFOCUS;

    bool statfound = FALSE;
    for (; statfound == FALSE;) {
      pstat = number_range(1, SKILL_USED);
      if (skilltype(pstat) == STYPE_ARCANEFOCUS || skilltype(pstat) == STYPE_PROFFOCUS || skilltype(pstat) == STYPE_COMBATFOCUS)
      statfound = TRUE;
    }

    for (int i = 0; i < 50; i++) {
      villains[i][0] = '\0';
      villain_praise_message[i][0] = '\0';
      villain_diss_message[i][0] = '\0';
      villain_scores[i] = 0;
      villain_praise[i] = 0;
      villain_tier[i] = 0;
    }

    char facstrings[30][MSL];
    int facpoints[30];
    for (int i = 0; i < 30; i++) {
      facstrings[i][0] = '\0';
      facpoints[i] = 0;
    }

    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      free_string((*it)->report);
      (*it)->report = str_dup("");
      for (int i = 0; i < 30; i++) {
        if (facpoints[i] == 0) {
          facpoints[i] = (*it)->vnum;
          i = 30;
        }
      }
    }

    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      online = FALSE;
      fac = NULL;
      (*it)->valid = FALSE;
      if ((victim = get_char_world_pc((*it)->charname)) !=
          NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, (*it)->charname)) {
          continue;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize((*it)->charname));
        stat(buf, &sb);
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        continue;
      }
      if (IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->guest_type != GUEST_NIGHTMARE) {
        if (!online)
        free_char(victim);
        continue;
      }

      if (IS_FLAG(victim->act, PLR_STASIS) || higher_power(victim) || IS_IMMORTAL(victim)) {
        if (!online)
        free_char(victim);
        continue;
      }
      if (IS_FLAG(victim->act, PLR_INFLUENCER))
      {
        REMOVE_FLAG(victim->act, PLR_INFLUENCER);
        save_char_obj(victim, FALSE, FALSE);
      }
      PROFILE_TYPE *prof = profile_lookup(victim->name);
      if(prof != NULL && prof->rating_count > 1)
      {
        int inf_score = influencer_score(victim);
        if(inf_score > maxf_one)
        {
          sprintf(iname_three, "%s", iname_two);
          maxf_three = maxf_two;
          sprintf(iname_two, "%s", iname_one);
          maxf_two = maxf_one;
          sprintf(iname_one, "%s", victim->name);
          maxf_one = inf_score;
        }
        else if(inf_score > maxf_two)
        {
          sprintf(iname_three, "%s", iname_two);
          maxf_three = maxf_two;
          sprintf(iname_two, "%s", victim->name);
          maxf_two = inf_score;
        }
        else if(inf_score > maxf_three)
        {
          sprintf(iname_three, "%s", victim->name);
          maxf_three = inf_score;
        }
      }

      sprintf(logs, "WCHAR: %s", victim->name);
      log_string(logs);
      fac = clan_lookup(victim->fcore);
      if (fac != NULL) {
        char facbuf[MSL];
        for (int i = 0; i < 40; i++)
        fac->total_tracked[i] += victim->pcdata->week_tracker[i];
        fac->population++;
        facbuf[0] = 0;
        char inputbuf[MSL];
        sprintf(inputbuf, "%s %s contributed $%d resources", victim->name, victim->pcdata->last_name, victim->pcdata->week_tracker[TRACK_CONTRIBUTED]);
        strcat(facbuf, inputbuf);
        if (victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED] > 0) {
          sprintf(inputbuf, ", planned %d operations", victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED] > 0) {
          sprintf(inputbuf, ", attended %d operations", victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED] > 0) {
          sprintf(inputbuf, ", contributed $%d in alchemical resources", victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_WARFARE] > 0) {
          sprintf(inputbuf, ",aided in %d society conflicts in Haven", victim->pcdata->week_tracker[TRACK_PATROL_WARFARE]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_ARCANE] > 0) {
          sprintf(inputbuf, ", aided in %d arcane tasks in Haven", victim->pcdata->week_tracker[TRACK_PATROL_ARCANE]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC] > 0) {
          sprintf(inputbuf, ", aided in %d diplomatic tasks for the society", victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_HUNTING] > 0) {
          sprintf(inputbuf, ", helped take down %d powerful forest creatures", victim->pcdata->week_tracker[TRACK_PATROL_HUNTING]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_BIGMONSTER] > 0) {
          sprintf(inputbuf, ", helped take down %d otherworldly monsters", victim->pcdata->week_tracker[TRACK_BIGMONSTER]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED] > 0) {
          sprintf(inputbuf, ", launched %d schemes", victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED] > 0) {
          sprintf(inputbuf, ", thwarted %d schemes", victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED]);
          strcat(facbuf, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_ADVENTURES] > 0) {
          sprintf(inputbuf, ", attended %d out of town missions", victim->pcdata->week_tracker[TRACK_ADVENTURES]);
          strcat(facbuf, inputbuf);
        }
        strcat(facbuf, ".");
        //	    sprintf(facbuf, "%s %s contributed $%d resources, planned %d
        //operations, attended %d operations, contributed $%d in alchemical
        //resources, aided in %d faction conflicts in Haven, aided in %d arcane
        //tasks in Haven, aided in %d diplomatic tasks for the faction, helped
        //take down %d powerful forest creatures, helped take down %d otherworldly
        //monsters, launched %d schemes, thwarted %d and attended %d out of town
        //missions.\n\r", victim->name, victim->pcdata->last_name, //victim->pcdata->week_tracker[TRACK_CONTRIBUTED], //victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED], //victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED], //victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED], //victim->pcdata->week_tracker[TRACK_PATROL_WARFARE], //victim->pcdata->week_tracker[TRACK_PATROL_ARCANE], //victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC], //victim->pcdata->week_tracker[TRACK_PATROL_HUNTING], //victim->pcdata->week_tracker[TRACK_BIGMONSTER], //victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED], //victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED], //victim->pcdata->week_tracker[TRACK_ADVENTURES]);
        char *factemp = str_dup(facbuf);
        factemp = format_string(factemp);
        sprintf(facbuf, "%s", factemp);
        for (int i = 0; i < 30; i++) {
          if (fac->vnum == facpoints[i])
          strcat(facstrings[i], facbuf);
        }
        //	    strcat(fac->report, facbuf);
        memset(facbuf, 0, MSL);
        if (victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
            victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT] >
            0) {
          if (victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] >
              victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT])
          sprintf(facbuf, "%s spent %d influence talking to the rank and file, saying things such as, '%s'\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT], victim->pcdata->last_praise_message[MESSAGE_LOYALTY]);
          else
          sprintf(facbuf, "%s spent %d influence talking to the rank and file, saying things such as, '%s'\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT], victim->pcdata->last_diss_message[MESSAGE_LOYALTY]);
        }
        else
        sprintf(facbuf, "%s spent 0 influence talking to the rank and file.\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
        free_string(factemp);
        factemp = str_dup(facbuf);
        factemp = format_string(factemp);
        sprintf(facbuf, "%s\n", factemp);

        for (int i = 0; i < 30; i++) {
          if (fac->vnum == facpoints[i])
          strcat(facstrings[i], facbuf);
        }
        //	    strcat(fac->report, facbuf);
        //	    fac->report = format_string(fac->report);
        memset(facbuf, 0, MSL);
      }
      fac = NULL;

      fac = clan_lookup(victim->fsect);
      if (fac != NULL) {
        char facbuf2[MSL];

        for (int i = 0; i < 40; i++)
        fac->total_tracked[i] += victim->pcdata->week_tracker[i];
        fac->population++;
        facbuf2[0] = 0;
        char inputbuf[MSL];
        sprintf(inputbuf, "%s %s contributed $%d resources", victim->name, victim->pcdata->last_name, victim->pcdata->week_tracker[TRACK_CONTRIBUTED]);
        strcat(facbuf2, inputbuf);
        if (victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED] > 0) {
          sprintf(inputbuf, ", planned %d operations", victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED] > 0) {
          sprintf(inputbuf, ", attended %d operations", victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED] > 0) {
          sprintf(inputbuf, ", contributed $%d in alchemical resources", victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_WARFARE] > 0) {
          sprintf(inputbuf, ",aided in %d society conflicts in Haven", victim->pcdata->week_tracker[TRACK_PATROL_WARFARE]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_ARCANE] > 0) {
          sprintf(inputbuf, ", aided in %d arcane tasks in Haven", victim->pcdata->week_tracker[TRACK_PATROL_ARCANE]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC] > 0) {
          sprintf(inputbuf, ", aided in %d diplomatic tasks for the society", victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_HUNTING] > 0) {
          sprintf(inputbuf, ", helped take down %d powerful forest creatures", victim->pcdata->week_tracker[TRACK_PATROL_HUNTING]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_BIGMONSTER] > 0) {
          sprintf(inputbuf, ", helped take down %d otherworldly monsters", victim->pcdata->week_tracker[TRACK_BIGMONSTER]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED] > 0) {
          sprintf(inputbuf, ", launched %d schemes", victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED] > 0) {
          sprintf(inputbuf, ", thwarted %d schemes", victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED]);
          strcat(facbuf2, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_ADVENTURES] > 0) {
          sprintf(inputbuf, ", attended %d out of town missions", victim->pcdata->week_tracker[TRACK_ADVENTURES]);
          strcat(facbuf2, inputbuf);
        }
        strcat(facbuf2, ".");
        //	    sprintf(facbuf2, "%s %s contributed $%d resources, planned
        //%d operations, attended %d operations, contributed $%d in alchemical
        //resources, aided in %d faction conflicts in Haven, aided in %d arcane
        //tasks in Haven, aided in %d diplomatic tasks for the faction, helped
        //take down %d powerful forest creatures, helped take down %d otherworldly
        //monsters, launched %d schemes, thwarted %d and attended %d out of town
        //missions.\n\r", victim->name, victim->pcdata->last_name, //victim->pcdata->week_tracker[TRACK_CONTRIBUTED], //victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED], //victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED], //victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED], //victim->pcdata->week_tracker[TRACK_PATROL_WARFARE], //victim->pcdata->week_tracker[TRACK_PATROL_ARCANE], //victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC], //victim->pcdata->week_tracker[TRACK_PATROL_HUNTING], //victim->pcdata->week_tracker[TRACK_BIGMONSTER], //victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED], //victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED], //victim->pcdata->week_tracker[TRACK_ADVENTURES]);
        char *factemp = str_dup(facbuf2);
        factemp = format_string(factemp);
        sprintf(facbuf2, "%s", factemp);

        for (int i = 0; i < 30; i++) {
          if (fac->vnum == facpoints[i])
          strcat(facstrings[i], facbuf2);
        }
        //	    strcat(fac->report, facbuf2);
        memset(facbuf2, 0, MSL);
        if (victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
            victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT] >
            0) {
          if (victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] >
              victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT])
          sprintf(facbuf2, "%s spent %d influence talking to the rank and file, saying things such as, '%s'\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT], victim->pcdata->last_praise_message[MESSAGE_LOYALTY]);
          else
          sprintf(facbuf2, "%s spent %d influence talking to the rank and file, saying things such as, '%s'\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT], victim->pcdata->last_diss_message[MESSAGE_LOYALTY]);
        }
        else
        sprintf(facbuf2, "%s spent 0 influence talking to the rank and file.\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
        //	    strcat(fac->report, facbuf2);
        //	    fac->report = format_string(fac->report);
        free_string(factemp);
        factemp = str_dup(facbuf2);
        factemp = format_string(factemp);
        sprintf(facbuf2, "%s\n", factemp);

        for (int i = 0; i < 30; i++) {
          if (fac->vnum == facpoints[i])
          strcat(facstrings[i], facbuf2);
        }
        memset(facbuf2, 0, MSL);
      }


      fac = NULL;

      fac = clan_lookup(victim->fcult);
      if (fac != NULL) {
        char facbuf3[MSL];

        for (int i = 0; i < 40; i++)
        fac->total_tracked[i] += victim->pcdata->week_tracker[i];
        fac->population++;
        facbuf3[0] = 0;
        char inputbuf[MSL];
        sprintf(inputbuf, "%s %s contributed $%d resources", victim->name, victim->pcdata->last_name, victim->pcdata->week_tracker[TRACK_CONTRIBUTED]);
        strcat(facbuf3, inputbuf);
        if (victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED] > 0) {
          sprintf(inputbuf, ", planned %d operations", victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED] > 0) {
          sprintf(inputbuf, ", attended %d operations", victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED] > 0) {
          sprintf(inputbuf, ", contributed $%d in alchemical resources", victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_WARFARE] > 0) {
          sprintf(inputbuf, ",aided in %d society conflicts in Haven", victim->pcdata->week_tracker[TRACK_PATROL_WARFARE]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_ARCANE] > 0) {
          sprintf(inputbuf, ", aided in %d arcane tasks in Haven", victim->pcdata->week_tracker[TRACK_PATROL_ARCANE]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC] > 0) {
          sprintf(inputbuf, ", aided in %d diplomatic tasks for the society", victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_PATROL_HUNTING] > 0) {
          sprintf(inputbuf, ", helped take down %d powerful forest creatures", victim->pcdata->week_tracker[TRACK_PATROL_HUNTING]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_BIGMONSTER] > 0) {
          sprintf(inputbuf, ", helped take down %d otherworldly monsters", victim->pcdata->week_tracker[TRACK_BIGMONSTER]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED] > 0) {
          sprintf(inputbuf, ", launched %d schemes", victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED] > 0) {
          sprintf(inputbuf, ", thwarted %d schemes", victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED]);
          strcat(facbuf3, inputbuf);
        }
        if (victim->pcdata->week_tracker[TRACK_ADVENTURES] > 0) {
          sprintf(inputbuf, ", attended %d out of town missions", victim->pcdata->week_tracker[TRACK_ADVENTURES]);
          strcat(facbuf3, inputbuf);
        }
        strcat(facbuf3, ".");
        //	    sprintf(facbuf2, "%s %s contributed $%d resources, planned
        //%d operations, attended %d operations, contributed $%d in alchemical
        //resources, aided in %d faction conflicts in Haven, aided in %d arcane
        //tasks in Haven, aided in %d diplomatic tasks for the faction, helped
        //take down %d powerful forest creatures, helped take down %d otherworldly
        //monsters, launched %d schemes, thwarted %d and attended %d out of town
        //missions.\n\r", victim->name, victim->pcdata->last_name, //victim->pcdata->week_tracker[TRACK_CONTRIBUTED], //victim->pcdata->week_tracker[TRACK_OPERATIONS_CREATED], //victim->pcdata->week_tracker[TRACK_OPERATIONS_ATTENDED], //victim->pcdata->week_tracker[TRACK_ALCH_CONTRIBUTED], //victim->pcdata->week_tracker[TRACK_PATROL_WARFARE], //victim->pcdata->week_tracker[TRACK_PATROL_ARCANE], //victim->pcdata->week_tracker[TRACK_PATROL_DIPLOMATIC], //victim->pcdata->week_tracker[TRACK_PATROL_HUNTING], //victim->pcdata->week_tracker[TRACK_BIGMONSTER], //victim->pcdata->week_tracker[TRACK_SCHEMES_LAUNCHED], //victim->pcdata->week_tracker[TRACK_SCHEMES_THWARTED], //victim->pcdata->week_tracker[TRACK_ADVENTURES]);
        char *factemp2 = str_dup(facbuf3);
        factemp2 = format_string(factemp2);
        sprintf(facbuf3, "%s", factemp2);

        for (int i = 0; i < 30; i++) {
          if (fac->vnum == facpoints[i])
          strcat(facstrings[i], facbuf3);
        }
        //	    strcat(fac->report, facbuf2);
        memset(facbuf3, 0, MSL);
        if (victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
            victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT] >
            0) {
          if (victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] >
              victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT])
          sprintf(facbuf3, "%s spent %d influence talking to the rank and file, saying things such as, '%s'\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT], victim->pcdata->last_praise_message[MESSAGE_LOYALTY]);
          else
          sprintf(facbuf3, "%s spent %d influence talking to the rank and file, saying things such as, '%s'\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She", victim->pcdata->week_tracker[TRACK_LOYALTY_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_LOYALTY_DISS_AMOUNT], victim->pcdata->last_diss_message[MESSAGE_LOYALTY]);
        }
        else
        sprintf(facbuf3, "%s spent 0 influence talking to the rank and file.\n\n\r", (victim->sex == SEX_MALE) ? "He" : "She");
        //	    strcat(fac->report, facbuf2);
        //	    fac->report = format_string(fac->report);
        free_string(factemp2);
        factemp2 = str_dup(facbuf3);
        factemp2 = format_string(factemp2);
        sprintf(facbuf3, "%s\n", factemp2);

        for (int i = 0; i < 30; i++) {
          if (fac->vnum == facpoints[i])
          strcat(facstrings[i], facbuf3);
        }
        memset(facbuf3, 0, MSL);
      }


      fac = NULL;

      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        for (int i = 0; i < 30; i++) {
          if (facpoints[i] == (*it)->vnum) {
            free_string((*it)->report);
            (*it)->report = str_dup(facstrings[i]);
            i = 30;
          }
        }
      }

      if (victim->pcdata->patrol_habits[PATROL_RECKLESSHABIT] > 0 || is_weakness(NULL, victim)) {
        if (number_percent() % 2 == 0 && !clinic_patient(victim)) {
          if (number_percent() % 3 == 0 || victim->faction != 0 || get_tier(victim) >= 2) {
            sprintf(reckless, "%s, %s", reckless, victim->name);
          }
        }
      }
      sprintf(logs, "WCHAR post Reckless: %s", victim->name);
      log_string(logs);

      if (victim->pcdata->intel >= 5000) {
        //      if (number_percent() % 2 == 0)
        sprintf(intelvalue, "%s, %s", intelvalue, victim->name);
      }
      if (victim->pcdata->heroic > herolevel) {
        memset(heroname, 0, MSL);
        sprintf(heroname, "%s %s", victim->name, victim->pcdata->last_name);
      }
      if (victim->pcdata->week_tracker[TRACK_CHARACTER_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_CHARACTER_DISS_AMOUNT] >
          most_char_influence) {
        memset(mcinfname, 0, MSL);
        memset(mcinfmessage, 0, MSL);
        most_char_influence =
        victim->pcdata->week_tracker[TRACK_CHARACTER_PRAISE_AMOUNT] +
        victim->pcdata->week_tracker[TRACK_CHARACTER_DISS_AMOUNT];
        if (victim->pcdata->week_tracker[TRACK_CHARACTER_PRAISE_AMOUNT] >
            victim->pcdata->week_tracker[TRACK_CHARACTER_DISS_AMOUNT])
        sprintf(mcinfmessage, "%s", victim->pcdata->last_praise_message[MESSAGE_CHARACTER]);
        else
        sprintf(mcinfmessage, "%s", victim->pcdata->last_diss_message[MESSAGE_CHARACTER]);
        sprintf(mcinfname, "%s %s", victim->name, victim->pcdata->last_name);
      }
      if (victim->pcdata->week_tracker[TRACK_APPEARANCE_PRAISE_AMOUNT] +
          victim->pcdata->week_tracker[TRACK_APPEARANCE_DISS_AMOUNT] >
          most_app_influence) {
        memset(mainfname, 0, MSL);
        memset(mainfmessage, 0, MSL);
        most_app_influence =
        victim->pcdata->week_tracker[TRACK_APPEARANCE_PRAISE_AMOUNT] +
        victim->pcdata->week_tracker[TRACK_APPEARANCE_DISS_AMOUNT];
        if (victim->pcdata->week_tracker[TRACK_APPEARANCE_PRAISE_AMOUNT] >
            victim->pcdata->week_tracker[TRACK_APPEARANCE_DISS_AMOUNT])
        sprintf(mainfmessage, "%s", victim->pcdata->last_praise_message[MESSAGE_APPEARANCE]);
        else
        sprintf(mainfmessage, "%s", victim->pcdata->last_diss_message[MESSAGE_APPEARANCE]);
        sprintf(mainfname, "%s %s", victim->name, victim->pcdata->last_name);
      }
      if (victim->pcdata->week_tracker[TRACK_RUMORS] > most_rumors && safe_strlen(victim->pcdata->last_rumor) > 1) {
        memset(mrumorname, 0, MSL);
        memset(mrumormessage, 0, MSL);
        most_rumors = victim->pcdata->week_tracker[TRACK_RUMORS];
        sprintf(mrumorname, "%s %s", victim->name, victim->pcdata->last_name);
        sprintf(mrumormessage, "%s", victim->pcdata->last_rumor);
      }
      if (victim->pcdata->week_tracker[TRACK_EVENT_MAXPOP] > most_event_pop) {
        memset(mepopname, 0, MSL);
        most_event_pop = victim->pcdata->week_tracker[TRACK_EVENT_MAXPOP];
        sprintf(mepopname, "%s %s", victim->name, victim->pcdata->last_name);
      }
      if (victim->pcdata->week_tracker[TRACK_EVENTS_ATTENDED] >
          most_event_attend) {
        memset(meattendname, 0, MSL);
        most_event_attend = victim->pcdata->week_tracker[TRACK_EVENTS_ATTENDED];
        sprintf(meattendname, "%s %s", victim->name, victim->pcdata->last_name);
      }
      if (victim->pcdata->week_tracker[TRACK_TREES_CHOPPED] >
          most_trees_chopped) {
        memset(treechoppername, 0, MSL);
        most_trees_chopped = victim->pcdata->week_tracker[TRACK_TREES_CHOPPED];
        sprintf(treechoppername, "%s %s", victim->name, victim->pcdata->last_name);
      }
      bool inrel = FALSE;
      for (int i = 0; i < 10; i++) {
        if (victim->pcdata->relationship_type[i] == REL_DATING || victim->pcdata->relationship_type[i] == REL_SPOUSE)
        inrel = TRUE;
      }
      if (victim->pcdata->week_tracker[TRACK_BANK] > most_money) {
        memset(moneyname, 0, MSL);
        most_money = victim->pcdata->week_tracker[TRACK_BANK];
        sprintf(moneyname, "%s %s", victim->name, victim->pcdata->last_name);
      }
      if (victim->pcdata->influence_bank > most_influence) {
        memset(mostinfluencename, 0, MSL);
        most_influence = victim->pcdata->influence_bank;
        sprintf(mostinfluencename, "%s %s", victim->name, victim->pcdata->last_name);
      }

      if (!inrel) {
        if (victim->pcdata->week_tracker[TRACK_EVENT_HOTNESS] >
            hottest_man_number && victim->sex == SEX_MALE) {
          memset(hotmanname, 0, MSL);
          hottest_man_number = victim->pcdata->week_tracker[TRACK_EVENT_HOTNESS];
          sprintf(hotmanname, "%s %s", victim->name, victim->pcdata->last_name);
        }
        if (victim->pcdata->week_tracker[TRACK_EVENT_HOTNESS] >
            hottest_woman_number && victim->sex == SEX_FEMALE && victim->pcdata->week_tracker[TRACK_SEX] == 0) {
          memset(hotwomanname, 0, MSL);
          hottest_woman_number =
          victim->pcdata->week_tracker[TRACK_EVENT_HOTNESS];
          sprintf(hotwomanname, "%s %s", victim->name, victim->pcdata->last_name);
        }
      }
      else {
        if (victim->pcdata->week_tracker[TRACK_EVENT_HOTNESS] > luckiest) {
          luckiest = victim->pcdata->week_tracker[TRACK_EVENT_HOTNESS];
          memset(luckiestname, 0, MSL);
          for (int i = 0; i < 10; i++) {
            if (victim->pcdata->relationship_type[i] == REL_DATING || victim->pcdata->relationship_type[i] == REL_SPOUSE)
            sprintf(luckiestname, "%s", victim->pcdata->relationship[i]);
          }
        }
      }
      if (ftype == STYPE_PROFFOCUS) {
        if (prof_focus(victim) > 1 && (prof_focus(victim) > ftypelevel || (prof_focus(victim) == ftypelevel && number_percent() % 3 == 0))) {
          memset(ftypename, 0, MSL);
          ftypelevel = prof_focus(victim);
          sprintf(ftypename, "%s %s", victim->name, victim->pcdata->last_name);
          log_string("Professional");
        }
      }
      if (ftype == STYPE_ARCANEFOCUS) {
        if (arcane_focus(victim) > 1 && (arcane_focus(victim) > ftypelevel || (arcane_focus(victim) == ftypelevel && number_percent() % 3 == 0))) {
          memset(ftypename, 0, MSL);
          ftypelevel = arcane_focus(victim);
          sprintf(ftypename, "%s %s", victim->name, victim->pcdata->last_name);
          log_string("Aracne");
        }
      }
      if (ftype == STYPE_COMBATFOCUS) {
        if (combat_focus(victim) > 1 && (combat_focus(victim) > ftypelevel || (combat_focus(victim) == ftypelevel && number_percent() % 3 == 0))) {
          memset(ftypename, 0, MSL);
          ftypelevel = combat_focus(victim);
          sprintf(ftypename, "%s %s", victim->name, victim->pcdata->last_name);
          log_string("Combat");
        }
      }
      if (get_skill(victim, pstat) > 1 && (get_skill(victim, pstat) > pstatlevel || (get_skill(victim, pstat) == pstatlevel && number_percent() % 3 == 0))) {
        memset(pstatname, 0, MSL);
        pstatlevel = get_skill(victim, pstat);
        sprintf(pstatname, "%s %s", victim->name, victim->pcdata->last_name);
        log_string("Pstat");
      }
      if (victim->spentkarma >= 75000 && !higher_power(victim)) {
        int loss = 100 - villain_rating(victim);
        /*
int hours = victim->played/3600;
hours = sqrt(hours);
loss = loss*hours/12;
*/
        victim->karma -= loss;
      }
      sprintf(logs, "WCHAR previllain: %s:%d", victim->name, day);
      log_string(logs);

      if ((day >= 1 && day < 7) || (day >= 14 && day < 21)) {
        if (IS_FLAG(victim->act, PLR_VILLAIN))
        REMOVE_FLAG(victim->act, PLR_VILLAIN);

        if (get_tier(victim) >= 3 && victim->played / 3600 > 100 && !higher_power(victim)) {
          int point = -1;
          for (int i = 0; i < 50; i++) {
            if (!str_cmp(villains[i], victim->name))
            point = i;
          }
          if (point == -1) {
            for (int i = 0; i < 50; i++) {
              if (safe_strlen(villains[i]) < 2) {
                point = i;
                i = 50;
                sprintf(villains[point], "%s", victim->name);
                //                    	    strcat(villains[i], victim->name);
              }
            }
          }
          villain_tier[point] = get_tier(victim);
          int mult = 100;
          if (victim->pcdata->account != NULL && victim->pcdata->account->villain_mod != 0) {
            if (victim->pcdata->account->villain_mod < 0)
            mult += victim->pcdata->account->villain_mod * 5;
            else
            mult += victim->pcdata->account->villain_mod * 10;
          }
          villain_scores[point] =
          victim->pcdata->week_tracker[TRACK_VILLAIN] +
          sqrt(victim->pcdata->life_tracker[TRACK_VILLAIN]);
          villain_scores[point] = villain_scores[point] * mult / 100;
          villain_scores[point] -= (int)(cbrt(victim->pcdata->attract[ATTRACT_PROM]*victim->pcdata->attract[ATTRACT_PROM]));
          sprintf(buf, "VILLAIN %s, Tier: %d, Score: %d, Week: %d, Life: %d", villains[point], villain_tier[point], villain_scores[point], victim->pcdata->week_tracker[TRACK_VILLAIN], victim->pcdata->life_tracker[TRACK_VILLAIN]);
          log_string(buf);
        }
        victim->pcdata->week_tracker[TRACK_VILLAIN] = 0;
        if (safe_strlen(victim->pcdata->villain_praise) > 1) {
          int point = -1;
          for (int i = 0; i < 50; i++) {
            if (!str_cmp(villains[i], victim->pcdata->villain_praise))
            point = i;
          }
          if (point == -1) {
            for (int i = 0; i < 50; i++) {
              if (safe_strlen(villains[i]) < 2) {
                point = i;
                i = 50;
                sprintf(villains[point], "%s", victim->pcdata->villain_praise);
                //                          strcat(villains[i], //                          victim->pcdata->villain_praise);
              }
            }
          }
          if (victim->pcdata->villain_type == 2 || !str_cmp(victim->name, victim->pcdata->villain_praise)) {
            villain_praise[point]--;
            if (victim->pcdata->villain_type == 2 && (number_percent() % 2 == 0 || safe_strlen(villain_diss_message[point]) < 2))
            sprintf(villain_diss_message[point], "%s", victim->pcdata->villain_message);
          }
          else {
            villain_praise[point]++;
            if (number_percent() % 2 == 0 || safe_strlen(villain_praise_message[point]) < 2)
            sprintf(villain_praise_message[point], "%s", victim->pcdata->villain_message);
          }
        }
        free_string(victim->pcdata->villain_praise);
        victim->pcdata->villain_praise = str_dup("");
        victim->pcdata->villain_type = 0;
        save_char_obj(victim, FALSE, FALSE);
      }

      for (int i = 0; i < 40; i++) {
        if (i != TRACK_VILLAIN)
        victim->pcdata->week_tracker[i] = 0;
      }
      for (int i = 0; i < 3; i++) {
        free_string(victim->pcdata->last_praise_message[i]);
        victim->pcdata->last_praise_message[i] = str_dup("");
        free_string(victim->pcdata->last_diss_message[i]);
        victim->pcdata->last_diss_message[i] = str_dup("");
      }
      free_string(victim->pcdata->last_rumor);
      victim->pcdata->last_rumor = str_dup("");

      save_char_obj(victim, FALSE, FALSE);

      if (!online)
      free_char(victim);
    }
    log_string("Weekly post characters");

    static char string[MSL];
    string[0] = '\0';

    if(strlen(iname_one) > 2)
    offline_setflag(iname_one, PLR_INFLUENCER);
    if(strlen(iname_two) > 2)
    offline_setflag(iname_two, PLR_INFLUENCER);
    if(strlen(iname_three) > 2)
    offline_setflag(iname_three, PLR_INFLUENCER);

    sprintf(buf, "The current MyHaven influencers are %s, %s, and %s.\n\r", iname_one, iname_two, iname_three);
    strcat(string, buf);

    if (most_char_influence > 1500 && safe_strlen(mcinfmessage) > 2) {
      sprintf(buf, "%s is widely thought to have been the most influential person in town this week, overheard to be saying things like, '%s'", mcinfname, mcinfmessage);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s\n", buftemp);
      strcat(string, buf);
    }
    if (most_app_influence > 1500 && safe_strlen(mainfmessage) > 2) {
      sprintf(buf, "%s was the town's main fashion critic this week, overheard to be saying things like, '%s'\n\n\r", mainfname, mainfmessage);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s\n", buftemp);
      strcat(string, buf);
    }
    if (most_rumors > 1 && safe_strlen(mrumormessage) > 2) {
      sprintf(buf, "%s was the town gossip this week, overheard spreading stories such as, '%s'\n\n\r", mrumorname, mrumormessage);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s\n", buftemp);
      strcat(string, buf);
    }
    if (most_event_pop > 2) {
      sprintf(buf, "%s supposedly held the most successful social event this week, with %d people in attendance.\n\r", mepopname, most_event_pop);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    /*
if(most_trees_chopped > 1)
{
sprintf(buf, "Rumors claim %s is quite the lumberjack, spending the
most time out in the forest chopping down trees this week.\n\r", treechoppername); char * buftemp = str_dup(buf); buftemp =
format_string(buftemp); sprintf(buf, "%s", buftemp); strcat(string, buf);
}
*/
    if (most_event_attend > 1) {
      sprintf(buf, "%s is the social butterly of the week, attending %d different social events.\n\r", meattendname, most_event_attend);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    if (hottest_man_number > 65) {
      sprintf(buf, "Gossip places %s as the most eligible bachelor in Haven.\n\r", hotmanname);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    if (hottest_woman_number > 65) {
      sprintf(buf, "Gossip places %s as the most eligible bachelorette in Haven.\n\r", hotwomanname);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    if (number_percent() % 3 == 0 && most_money > 4000) {
      sprintf(buf, "People are saying that %s might just be the richest person in Haven.\n\r", moneyname);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    if (number_percent() % 2 == 0 && most_influence > 3000) {
      sprintf(buf, "Many people are trying to get close to %s, hearing that they're both influential and very selective about their company.", mostinfluencename);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    if (number_percent() % 3 == 0 && luckiest > 65) {
      sprintf(buf, "There are many jealous whispers about %s and plans to steal their significant other away from them.\n\r", luckiestname);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(string, buf);
    }
    strcat(string, "\n\r");
    NEWS_TYPE *news;
    news = new_news();
    free_string(news->author);
    news->author = str_dup("Town News");
    news->timer = 5000;
    free_string(news->message);
    news->message = str_dup(string);
    NewsVect.push_back(news);
    static char scarystring[MSL];
    scarystring[0] = '\0';

    if ((day >= 1 && day < 7) || (day >= 14 && day < 21)) {

      int last = 0;
      for (int i = 0; i < 50; i++) {
        if (safe_strlen(villains[i]) > 1)
        last++;
        villain_scores[i] += villain_praise[i] * 50;
        if (villain_tier[i] < 3)
        villain_scores[i] = 0;
      }
      int max = 100;
      int min = 300;
      for (int i = 0; i <= last; ++i) {
        if (villain_scores[i] < min && villain_tier[i] > 2) {
          min = villain_scores[i];
          villaindisspointer = i;
        }
        if (villain_scores[i] > max && villain_tier[i] > 2) {
          max = villain_scores[i];
          villainpraisepointer = i;
        }
      }
      static char logstring2[MSL];
      logstring2[0] = '\0';
      strcat(logstring2, "VILLAINS ");
      for (int i = 0; i < 50; i++) {
        sprintf(buf, "%s:[%d] %d %d %d. ", villains[i], i, villain_scores[i], villain_praise[i], villain_tier[i]);
        strcat(logstring2, buf);
      }
      log_string(logstring2);

      int notscarypoint = -1;
      for (int i = 49; i >= 0; i--) {
        if (safe_strlen(villains[i]) > 1 && villain_tier[i] >= 3) {
          notscarypoint = i;
          i = -1;
        }
      }

      for (int i = 0; i < 50; i++) {
        if (safe_strlen(villains[i]) > 1 && villain_scores[i] >= 100 && (villain_scores[i] - villain_praise[i] * 50) >= 30) {
          villain_update(villains[i], 1);
        }
      }
      for (int i = 0; i < 50; i++) {
        if (safe_strlen(villains[i]) > 1 && villain_scores[i] < 20 && villain_tier[i] > 2 && (villain_scores[i] - villain_praise[i] * 50) <= 50) {
          villain_update(villains[i], -1);
        }
      }
      if (villainpraisepointer > -1 && villain_scores[villainpraisepointer] >= 100 && (villain_scores[villainpraisepointer] -
            villain_praise[villainpraisepointer] * 50) >= 50)
      offline_setflag(villains[villainpraisepointer], PLR_VILLAIN);

      if (villaindisspointer == notscarypoint)
      notscarypoint--;

      if (villainpraisepointer > -1 && villaindisspointer > -1 && villain_scores[villainpraisepointer] >= 100 && villain_scores[villaindisspointer] < 20)
      sprintf(buf, "In the supernatural underground people are claiming %s is particularly intimidating, while others note that for what they are, %s is particularly unintimidating.", villains[villainpraisepointer], villains[villaindisspointer]);
      else if (villainpraisepointer > -1 && villain_scores[villainpraisepointer] >= 100)
      sprintf(buf, "In the supernatural underground people are claiming %s is particularly intimidating.", villains[villainpraisepointer]);
      else if (villaindisspointer > -1 && villain_scores[villaindisspointer] < 20)
      sprintf(buf, "In the supernatural underground people are claiming %s is particularly unintimidating.", villains[villaindisspointer]);
      else
      sprintf(buf, "There are few rumors in the supernatural underworld recently.");

      char *scartemp = str_dup(buf);
      scartemp = format_string(scartemp);
      sprintf(buf, "%s\n", scartemp);
      strcat(scarystring, buf);

      sprintf(buf, "Others being discussed as having a particularly monsterous nature are");
      static char secondstring[MSL];
      secondstring[0] = '\0';
      strcat(secondstring, buf);
      for (int i = 0; i < 50; i++) {
        if (villain_tier[i] >= 3 && i != villainpraisepointer && i != villaindisspointer && i != notscarypoint) {
          sprintf(buf, ", %s", villains[i]);
          strcat(secondstring, buf);
        }
      }
      sprintf(buf, " and %s.", villains[notscarypoint]);
      strcat(secondstring, buf);
      char *scartwotemp = str_dup(secondstring);
      scartwotemp = format_string(scartwotemp);
      sprintf(buf, "%s\n", scartwotemp);
      strcat(scarystring, buf);
    }
    static char supersocialstring[MSL];
    supersocialstring[0] = '\0';
    if (pstatlevel > 0) {
      sprintf(buf, "Rumor has it %s is the best %s in the town.\n\r", pstatname, focus_name(pstat));
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(supersocialstring, buf);
    }

    if (ftypelevel > 0 && number_percent() % 2 == 0) {
      if (ftype == STYPE_PROFFOCUS)
      sprintf(
      buf, "People are claiming %s is the best connected professional in town.", ftypename);
      else if (ftype == STYPE_ARCANEFOCUS)
      sprintf(
      buf, "People are claiming that %s is the most powerful arcanist in town.", ftypename);
      else if (ftype == STYPE_COMBATFOCUS)
      sprintf(
      buf, "People are claiming that %s is the most powerful warrior in town.", ftypename);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(supersocialstring, buf);
    }
    if (safe_strlen(reckless) > 3) {
      sprintf(buf, "Supernatural predators have noticed several particularly vulnerable people this week%s.\n\r", reckless);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(supersocialstring, buf);
    }

    if (safe_strlen(intelvalue) > 3) {
      sprintf(
      buf, "Societies note several individuals with valuable intelligence%s.\n\r", intelvalue);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(supersocialstring, buf);
    }

    strcat(supersocialstring, "\n\r");
    if (herolevel >= 20) {
      sprintf(buf, "Rumor has it %s is the bravest hero in town.\n\r", heroname);
      char *buftemp = str_dup(buf);
      buftemp = format_string(buftemp);
      sprintf(buf, "%s", buftemp);
      strcat(supersocialstring, buf);
    }
    strcat(supersocialstring, "\n\r");

    for (int i = 0; i < 40; i++) {
      max_nongeneric[i] = 0;
      max_nongeneric_point[i] = 0;
    }
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      for (int i = 0; i < 40; i++) {
        if ((*it)->total_tracked[i] * 100 > max_nongeneric[i]) {
          max_nongeneric[i] = (*it)->total_tracked[i] * 100;
          max_nongeneric_point[i] = (*it)->vnum;
        }
      }
    }
    memset(buf, 0, MSL);
    static char newstring[MSL];
    newstring[0] = '\0';

    strcat(newstring, buf);
    memset(buf, 0, MSL);
    sprintf(
    buf, "Among the societies, %s has done the best at planning operations this week, %s at attending them. %s has gathered the most resources, %s the most alchemical resources. %s has engaged in the most big game hunts, %s in the most other world monster hunts. %s in the most society conflict in Haven, %s in the most arcane tasks, %s in the most diplomatic tasks, %s has had the best attendence in out of town missions, %s the best at stopping schemes inside town and %s has built the most roads.\n\r", (max_nongeneric[TRACK_OPERATIONS_CREATED] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_OPERATIONS_CREATED])->name, (max_nongeneric[TRACK_OPERATIONS_ATTENDED] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_OPERATIONS_ATTENDED])->name, (max_nongeneric[TRACK_CONTRIBUTED] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_CONTRIBUTED])->name, (max_nongeneric[TRACK_ALCH_CONTRIBUTED] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_ALCH_CONTRIBUTED])->name, (max_nongeneric[TRACK_PATROL_HUNTING] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_PATROL_HUNTING])->name, (max_nongeneric[TRACK_BIGMONSTER] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_BIGMONSTER])->name, (max_nongeneric[TRACK_PATROL_WARFARE] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_PATROL_WARFARE])->name, (max_nongeneric[TRACK_PATROL_ARCANE] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_PATROL_ARCANE])->name, (max_nongeneric[TRACK_PATROL_DIPLOMATIC] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_PATROL_DIPLOMATIC])->name, (max_nongeneric[TRACK_ADVENTURES] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_ADVENTURES])->name, (max_nongeneric[TRACK_SCHEMES_THWARTED] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_SCHEMES_THWARTED])->name, (max_nongeneric[TRACK_ROADS_BUILT] <= 0)
    ? "no one" : clan_lookup(max_nongeneric_point[TRACK_ROADS_BUILT])->name);
    char *buftemp2 = str_dup(buf);
    buftemp2 = format_string(buftemp2);
    sprintf(buf, "%s\r", buftemp2);
    strcat(newstring, buf);
    strcat(newstring, "\n\r");
    if ((day >= 1 && day < 7) || (day >= 14 && day < 21)) {
      strcat(newstring, scarystring);
      strcat(newstring, "\n\r");
    }

    strcat(newstring, supersocialstring);
    FILE *weather;
    if ((weather = fopen(WEATHER_FILE, "w")) != NULL) {
      fprintf(weather, "%d %d %d %d %d %d %d %d %d %d %d %d %ld %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", time_info.minute, time_info.hour, time_info.day, time_info.month, time_info.year, time_info.local_temp, time_info.des_temp, time_info.local_cover_total, time_info.cover_trend, time_info.density_trend, time_info.local_density_total, time_info.understanding, time_info.phone, time_info.local_mist_level, time_info.mist_timer, time_info.faction_vnum, time_info.monster_hours, time_info.mutilate_hours, time_info.cult_alliance_issue, time_info.cult_alliance_type, time_info.bloodstorm, time_info.sect_alliance_issue, time_info.sect_alliance_type, time_info.lweek_tier, time_info.lweek_total, time_info.tweek_tier, time_info.tweek_total);
      fclose(weather);
    }
    else
    bugf("[%s:%s] Could not open weather file!", __FILE__, __FUNCTION__);

    news = new_news();
    free_string(news->author);
    news->author = str_dup("Supernatural News");
    news->timer = 5000;
    free_string(news->message);
    news->message = str_dup(newstring);
    news->stats[0] = -2;
    NewsVect.push_back(news);

    if(strcasestr(string, "\"") == NULL && strcasestr(newstring, "\"") == NULL)
    {
      char scriptbuf[MSL];
      sprintf(scriptbuf, "5,0,\"%s\",\"%s\",,,", string, newstring);
      writeLineToFile(AI_IN_FILE, str_dup(scriptbuf));
    }

    save_news();
    save_weekly_characters();
    save_clans(FALSE);

    log_string("Weekly Completed");
  }

  _DOFUN(do_weekly) { weekly_process(); }
  _DOFUN(do_weekclean) {
    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      (*it)->valid = FALSE;
    }
  }

  CHAR_DATA *get_patroller(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    char buf[MSL];
    sprintf(buf, "GETPATROL: %s, %d, %d", ch->name, ch->pcdata->patrol_status, ch->pcdata->patrol_timer);
    log_string(buf);
    if (!IS_NPC(ch) && ch->pcdata->patrol_status > 1 && ch->pcdata->patrol_timer > 0 && !IS_FLAG(ch->comm, COMM_PRIVATE))
    return ch;

    int limit = 0;
    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end() && limit < 100; ++it) {
      victim = *it;
      limit++;
      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->pcdata->patrol_status <= 1 || victim->pcdata->patrol_timer <= 0)
      continue;

      if (IS_FLAG(victim->comm, COMM_PRIVATE))
      continue;

      return victim;
    }
    return NULL;
  }

  int min_hours(int type) {
    if (type == LOGEVENT_ENCOUNTER)
    return 6;
    if (type == LOGEVENT_PATROL)
    return 2;
    if (type == LOGEVENT_THWART)
    return 12;
    if (type == LOGEVENT_ADVENTURE)
    return 12;
    if (type == LOGEVENT_OTHER)
    return 3;
    if (type == LOGEVENT_MONSTER)
    return 3;
    return 2;
  }

  CHAR_DATA *random_room_pc(ROOM_INDEX_DATA *room) {
    CHAR_DATA *pledger = NULL;

    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (is_gm(fch))
      continue;
      if (pledger == NULL || number_percent() % 2 == 0)
      pledger = fch;
    }
    return pledger;
  }

  CHAR_DATA *patrol_attacker(ROOM_INDEX_DATA *room) {
    CHAR_DATA *pledger = NULL;

    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (fch->pcdata->patrol_status == PATROL_ATTACKSEARCHING || fch->pcdata->patrol_status == PATROL_ATTACKWAITING || fch->pcdata->patrol_status == PATROL_WAGINGWAR) {
        if (pledger == NULL || number_percent() % 2 == 0)
        pledger = fch;
      }
      if (fch->pcdata->patrol_status == PATROL_LEADING_ASSAULT) {
        if (pledger == NULL || number_percent() % 2 == 0)
        pledger = fch;
      }
      if (fch->pcdata->patrol_status == PATROL_PREYING || fch->pcdata->patrol_status == PATROL_CHASING || fch->pcdata->patrol_status == PATROL_SEARCHING || fch->pcdata->patrol_status == PATROL_GRABBING) {
        if (pledger == NULL || number_percent() % 2 == 0)
        pledger = fch;
      }
    }
    return pledger;
  }
  CHAR_DATA *patrol_defender(ROOM_INDEX_DATA *room) {
    CHAR_DATA *pledger = NULL;

    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (fch->pcdata->patrol_status == PATROL_DEFENDHIDING || fch->pcdata->patrol_status == PATROL_DEFENDASSISTING) {
        if (pledger == NULL || number_percent() % 2 == 0)
        pledger = fch;
      }
      if (fch->pcdata->patrol_status == PATROL_UNDER_ASSAULT) {
        if (pledger == NULL || number_percent() % 2 == 0)
        pledger = fch;
      }
      if (fch->pcdata->patrol_status == PATROL_PREY || fch->pcdata->patrol_status == PATROL_FLEEING || fch->pcdata->patrol_status == PATROL_HIDING || fch->pcdata->patrol_status == PATROL_GRABBED) {
        if (pledger == NULL || number_percent() % 2 == 0)
        pledger = fch;
      }
    }
    return pledger;
  }

  CHAR_DATA *patrol_banisher(ROOM_INDEX_DATA *room) {
    CHAR_DATA *pledger = NULL;

    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (fch->pcdata->patrol_status == PATROL_BANISHING || (pledger == NULL && fch->pcdata->patrol_status == PATROL_ASSISTING_BANISHING))
      pledger = fch;
    }
    return pledger;
  }

  char *const event_names[] = {
    "None",        "MindControl",     "Discredit",  "Steal", "Outcast",     "Brainwash",       "Heist",      "Operation", "Recruitment", "Teaching",        "Occurance",  "Condition", "Dominance",   "Aegis",           "Cleanse",    "Catastrophe", "nothing",     "RevokeSanctuary", "Rob",        "disruption", "storm",       "flood",           "blackout",   "hurricane", "malady",      "haunt",           "dreambelief"};

  char *event_title(int type, int subtype, ROOM_INDEX_DATA *room) {
    CHAR_DATA *gm = get_gm(room, FALSE);
    char buf[MSL];
    if (type == LOGEVENT_ENCOUNTER) {
      CHAR_DATA *ch = random_room_pc(room);
      if (ch != NULL) {
        sprintf(buf, "%s's odd encounter(%s)", ch->name, gm->name);
        return str_dup(buf);
      }
      return "An odd encounter in Haven";
    }
    if (type == LOGEVENT_THWART) {
      if (gm != NULL && gm->pcdata->scheme_running != NULL) {
        sprintf(buf, "An attempt to thwart %s's %s(%s)", gm->pcdata->scheme_running->author, event_names[gm->pcdata->scheme_running->type], gm->name);
        return str_dup(buf);
      }
      return "Scout report on an attempt to thwart an ongoing scheme";
    }
    if (type == LOGEVENT_ADVENTURE) {
      if (gm != NULL && get_karma_plot(gm) != NULL) {
        sprintf(buf, "%s(%s)", get_karma_plot(gm)->title, gm->name);
        return str_dup(buf);
      }
      return "An out of town mission";
    }
    if (type == LOGEVENT_MONSTER) {
      CHAR_DATA *mon = fetch_guestmonster();
      if (mon != NULL) {
        sprintf(buf, "Battle with %s", mon->name);
        return str_dup(buf);
      }
      return "Battle with a nightmare creature";
    }
    if (type == LOGEVENT_PATROL) {
      if (subtype == PATROL_HUNTING) {
        CHAR_DATA *ch = random_room_pc(room);
        if (ch != NULL) {
          sprintf(buf, "%s's big game hunt", emote_name(ch));
          return str_dup(buf);
        }
        return "A big game hunt";
      }
      if (subtype == PATROL_WAR) {
        CHAR_DATA *ch = patrol_attacker(room);
        CHAR_DATA *victim = patrol_defender(room);
        if (ch != NULL && victim != NULL) {
          sprintf(buf, "%s's nightmare battle with %s", emote_name(ch), emote_name(victim));
          return str_dup(buf);
        }
        return "A battle in the Nightmare";
      }
      if (subtype == PATROL_ARTIFACT) {
        CHAR_DATA *ch = random_room_pc(room);
        if (ch != NULL) {
          sprintf(buf, "%s's decursing attempt", emote_name(ch));
          return str_dup(buf);
        }
        return "A cursed artifact";
      }
      if (subtype == PATROL_GHOST) {
        CHAR_DATA *ch = patrol_banisher(room);
        if (ch != NULL) {
          sprintf(buf, "%s's ghost banishing", emote_name(ch));
          return str_dup(buf);
        }
        return "Spectral disturbance";
      }
      if (subtype == PATROL_ARCANEWAR) {
        CHAR_DATA *ch = patrol_attacker(room);
        CHAR_DATA *victim = patrol_defender(room);
        if (ch != NULL && victim != NULL) {
          sprintf(buf, "%s's arcane battle with %s", emote_name(ch), emote_name(victim));
          return str_dup(buf);
        }
        return "An arcane battle";
      }
      if (subtype == PATROL_KIDNAP) {
        CHAR_DATA *ch = random_room_pc(room);
        if (ch != NULL) {
          sprintf(buf, "%s's syndicate bargain", emote_name(ch));
          return str_dup(buf);
        }
        return "A syndicate kidnapping";
      }
      if (subtype == PATROL_BARGAIN) {
        CHAR_DATA *ch = random_room_pc(room);
        if (ch != NULL) {
          sprintf(buf, "%s's bargaining", emote_name(ch));
          return str_dup(buf);
        }
        return "A meeting with an out of town group";
      }
      if (subtype == PATROL_BRIBE) {
        CHAR_DATA *ch = random_room_pc(room);
        if (ch != NULL) {
          sprintf(buf, "%s's bribery attempt", emote_name(ch));
          return str_dup(buf);
        }
        return "A bribery attempt";
      }
      if (subtype == PATROL_PREDATOR) {
        CHAR_DATA *ch = patrol_attacker(room);
        CHAR_DATA *victim = patrol_defender(room);
        if (ch != NULL && victim != NULL) {
          sprintf(buf, "%s's assault on %s", emote_name(ch), emote_name(victim));
          return str_dup(buf);
        }
        return "An attack";
      }
    }
    if (type == LOGEVENT_OTHER)
    return "Confidential Scout Report";
    return "";
  }

  void logevent(ROOM_INDEX_DATA *room, int type, int subtype, char *argument, CHAR_DATA *ch) {
    char buf[MSL];
    sprintf(buf, "LOGEVENT: %d, %d, %d", room->vnum, type, subtype);
    log_string(buf);
    int limit = 0;
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end() && limit < 1000; ++it) {
      limit++;
      if ((*it)->vnum == 0 || (*it)->stasis == 1 || (*it)->antagonist == 1 || (*it)->outcast == 1 || (*it)->attributes[FACTION_SCOUTS] == 0)
      continue;
      FACTION_TYPE *fac = *it;
      bool found = FALSE;
      for (int i = 0; i < 20; i++) {
        if (found == FALSE && fac->event_type[i] == type && fac->event_subtype[i] == subtype && fac->event_time[i] > (current_time - (3600 * min_hours(type)))) {
          found = TRUE;
          int point = -1;

          if(ch != NULL && !IS_NPC(ch) && !is_gm(ch))
          {
            bool found = FALSE;
            for(int t=0;t<50;t++)
            {
              if(!str_cmp(ch->name, fac->event_participants[i][t]))
              {
                found = TRUE;
                break;
              }
            }
            if(found == FALSE)
            {
              for(int t=0;t<50;t++)
              {
                if(fac->event_participants[i][t][0] == '\0')
                {
                  free_string(fac->event_participants[i][t]);
                  fac->event_participants[i][t] = str_dup(ch->name);
                  break;
                }
              }
            }
          }


          for (int j = 0; j < 20; j++) {
            if (point == -1 && safe_strlen(fac->event_text[i][j]) < 25000)
            point = j;
          }
          if (point == -1)
          return;
          char logs[MSL];
          sprintf(logs, "%s\n%s", fac->event_text[i][point], argument);
          free_string(fac->event_text[i][point]);
          fac->event_text[i][point] = str_dup(logs);
          if (safe_strlen(fac->event_text[i][point]) >= 25000) {
            sprintf(logs, "%s\n\n`WSee society report event %d %d to continue.`x\n\r", fac->event_text[i][point], i + 1, point + 2);
            free_string(fac->event_text[i][point]);
            fac->event_text[i][point] = str_dup(logs);
          }
        }
      }
      if (found == FALSE) {
        int oldpoint = -1;
        int oldtime = current_time;
        for (int i = 0; i < 20; i++) {
          if ((i == 0 || i == 1) && type != LOGEVENT_OTHER)
          continue;
          if (i < 15 && type == LOGEVENT_ENCOUNTER)
          continue;
          if (fac->event_time[i] < oldtime) {
            oldtime = fac->event_time[i];
            oldpoint = i;
          }
        }
        if (oldpoint == -1)
        return;
        fac->event_time[oldpoint] = current_time;
        fac->event_log_sent[oldpoint] = 0;
	fac->event_plog_sent[oldpoint] = 0;
        fac->event_type[oldpoint] = type;
        fac->event_subtype[oldpoint] = subtype;
        for (int i = 0; i < 20; i++) {
          free_string(fac->event_text[oldpoint][i]);
          fac->event_text[oldpoint][i] = str_dup("");
        }
        time_t east_time = current_time;
        char tmp[MSL];
        char datestr[MSL];
        char logs[MSL];
        sprintf(tmp, "%s", (char *)ctime(&east_time));
        sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);
        CHAR_DATA *gm = get_gm(room, FALSE);
        if (gm != NULL) {
          sprintf(logs, "`g               (`x%s:%s`g)`x\n\n`c        [`x%s`c]`x\n\n%s\n%s\n%s\n\n%s", event_title(type, subtype, room), gm->name, datestr, roomtitle(room, TRUE), room->description, displaysun_room(room), argument);
        }
        else {
          sprintf(logs, "`g               (`x%s`g)`x\n\n`c        [`x%s`c]`x\n\n%s\n%s\n%s\n\n%s", event_title(type, subtype, room), datestr, roomtitle(room, TRUE), room->description, displaysun_room(room), argument);
        }
        free_string(fac->event_text[oldpoint][0]);
        fac->event_text[oldpoint][0] = str_dup(logs);
        free_string(fac->event_title[oldpoint]);
        fac->event_title[oldpoint] = str_dup(event_title(type, subtype, room));
      }
    }
  }

  void logevent_check(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *gm = get_gm(ch->in_room, FALSE);
    if (gm != NULL) {
      if (has_adventure(gm) && gm->pcdata->encounter_status != ENCOUNTER_ONGOING) {
        logevent(ch->in_room, LOGEVENT_ADVENTURE, gm->id, argument, ch);
        return;
      }
      if (gm->pcdata->karma_battery > 0 || gm->pcdata->encounter_status == ENCOUNTER_ONGOING) {
        logevent(ch->in_room, LOGEVENT_ENCOUNTER, ch->in_room->vnum, argument, ch);
        return;
      }
      else if (gm->pcdata->scheme_running != NULL) {
        logevent(ch->in_room, LOGEVENT_THWART, gm->pcdata->scheme_running->type, argument, ch);
        return;
      }
    }
    if (monster_fight(ch)) {
      logevent(ch->in_room, LOGEVENT_MONSTER, 0, argument, ch);
      return;
    }
    CHAR_DATA *pat = get_patroller(ch);
    if (pat != NULL) {
      int stat = pat->pcdata->patrol_status;
      if (stat == PATROL_HUNTING || stat == PATROL_APPROACHINGHUNT || stat == PATROL_HUNTFIGHTING) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_HUNTING, argument, ch);
        return;
      }
      if (stat == PATROL_ATTACKSEARCHING || stat == PATROL_ATTACKWAITING || stat == PATROL_ATTACKASSISTING || stat == PATROL_DEFENDASSISTING || stat == PATROL_DEFENDHIDING || stat == PATROL_WAGINGWAR) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_WAR, argument, ch);
        return;
      }
      if (stat == PATROL_DECURSING || stat == PATROL_ASSISTING_DECURSING || stat == PATROL_HOLDING_ARTIFACT) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_ARTIFACT, argument, ch);
        return;
      }
      if (stat == PATROL_BANISHING || stat == PATROL_ASSISTING_BANISHING) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_GHOST, argument, ch);
        return;
      }
      if (stat == PATROL_LEADING_ASSAULT || stat == PATROL_UNDER_ASSAULT || stat == PATROL_ASSISTING_ASSAULT || stat == PATROL_ASSISTING_UNDER_ASSAULT) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_ARCANEWAR, argument, ch);
        return;
      }
      if (stat == PATROL_KIDNAPPED || (stat == PATROL_BIDDING && (pat->in_room->vnum == ROOM_MEETING_WEST || pat->in_room->vnum == ROOM_MEETING_EAST))) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_KIDNAP, argument, ch);
        return;
      }
      if (stat == PATROL_BARGAINING && (pat->in_room->vnum == ROOM_MEETING_WEST || pat->in_room->vnum == ROOM_MEETING_EAST)) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_BARGAIN, argument, ch);
        return;
      }
      if (stat == PATROL_BRIBING && ch->in_room == ch->pcdata->patrol_room) {
        logevent(ch->in_room, LOGEVENT_PATROL, PATROL_BRIBE, argument, ch);
        return;
      }
      /*
if (stat == PATROL_INIT_PREYING || stat == PATROL_INIT_PREY || stat == PATROL_FLEEING || stat == PATROL_CHASING || stat == PATROL_HIDING || stat == PATROL_SEARCHING || stat == PATROL_GRABBING || stat == PATROL_GRABBED) {
logevent(ch->in_room, LOGEVENT_PATROL, PATROL_PREDATOR, argument);
return;
}
*/
    }
  }

  bool only_faction(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;
    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if ((fch->fcore != ch->fcore || fch->fcore == 0) && (fch->fsect != ch->fsect || fch->fsect == 0) && (fch->fcult != ch->fcult || fch->fcult == 0))
      return FALSE;
    }
    return TRUE;
  }

  bool only_allies(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;
    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;
      if (fch == NULL || IS_NPC(fch))
      continue;
      if (fch->faction != 0) {
        if(same_faction(ch, fch) == FALSE)
        return FALSE;
      }
    }
    return TRUE;
  }

  EXTRA_PATROL_TYPE* random_epatrol() {

    if (!EPatrolVect.empty()) {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> distribution(0, EPatrolVect.size() - 1);
      int randomIndex = distribution(gen);

      EXTRA_PATROL_TYPE* randomItem = EPatrolVect[randomIndex];

      return randomItem;
    }
    return NULL;
  }

  EXTRA_PATROL_TYPE* epatrol_by_id(int id) {
    for (vector<EXTRA_PATROL_TYPE *>::iterator it = EPatrolVect.begin();
    it != EPatrolVect.end(); ++it) {
      if (!(*it)->intro || (*it)->intro[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if((*it)->id == id)
      return *it;
    }
    return NULL;
  }




  void prey_capture(CHAR_DATA *pred, CHAR_DATA *prey)
  {
    act("You are captured.", pred, NULL, prey, TO_VICT);
    act("$N is captured.", pred, NULL, prey, TO_CHAR);

    if(!IS_FLAG(prey->act, PLR_BOUND))
    SET_FLAG(prey->act, PLR_BOUND);

    free_string(pred->pcdata->fixation_name);
    pred->pcdata->fixation_name = str_dup(prey->name);
    free_string(pred->pcdata->building_fixation_name);
    pred->pcdata->building_fixation_name = str_dup("");
    pred->pcdata->fixation_level = pred->pcdata->building_fixation_level;
    if(pred->pcdata->fixation_level == 1)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*3);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 2)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*9);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 3)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 4)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*2);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 5)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*3);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 6)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*5);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 7)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*7);
      pred->pcdata->fixation_charges = 1;
    }
    else if(pred->pcdata->fixation_level == 8)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*9);
      pred->pcdata->fixation_charges = 2;
    }
    else if(pred->pcdata->fixation_level == 9)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*12);
      pred->pcdata->fixation_charges = 3;
    }
    else if(pred->pcdata->fixation_level >= 10)
    {
      pred->pcdata->fixation_timeout = current_time + (3600*24*30);
      pred->pcdata->fixation_charges = 10;
    }
    prey->pcdata->patrol_status = 0;
    prey->pcdata->patrol_timer = 0;
    pred->pcdata->patrol_status = 0;
    pred->pcdata->patrol_timer = 0;
    prey->pcdata->patrol_target = NULL;
    pred->pcdata->patrol_target = NULL;
    prey->pcdata->villain_option = 0;
    pred->pcdata->villain_option = 0;
    prey->pcdata->prey_option = 0;
    pred->pcdata->prey_option = 0;
    prey->pcdata->prey_option_cooldown = 0;
    pred->pcdata->prey_option_cooldown = 0;
    prey->pcdata->villain_option_cooldown = 0;
    pred->pcdata->villain_option_cooldown = 0;
    prey->pcdata->prey_emote_cooldown = 0;
    pred->pcdata->prey_emote_cooldown = 0;
    prey->pcdata->villain_emote_cooldown = 0;
    pred->pcdata->villain_emote_cooldown = 0;
  }
  void prey_escape(CHAR_DATA *pred, CHAR_DATA *prey)
  {
    act("You escape.", pred, NULL, prey, TO_VICT);
    act("$N escapes.", pred, NULL, prey, TO_CHAR);
    if(IS_FLAG(prey->act, PLR_BOUND))
    REMOVE_FLAG(prey->act, PLR_BOUND);

    free_string(pred->pcdata->fixation_name);
    pred->pcdata->fixation_name = str_dup("");
    free_string(pred->pcdata->building_fixation_name);
    pred->pcdata->building_fixation_name = str_dup("");
    pred->pcdata->fixation_level = 0;
    pred->pcdata->fixation_timeout = 0;
    pred->pcdata->fixation_charges = 0;
    prey->pcdata->patrol_status = 0;
    prey->pcdata->patrol_timer = 0;
    pred->pcdata->patrol_status = 0;
    pred->pcdata->patrol_timer = 0;
    prey->pcdata->patrol_target = NULL;
    pred->pcdata->patrol_target = NULL;
    prey->pcdata->villain_option = 0;
    pred->pcdata->villain_option = 0;
    prey->pcdata->prey_option = 0;
    pred->pcdata->prey_option = 0;
    prey->pcdata->prey_option_cooldown = 0;
    pred->pcdata->prey_option_cooldown = 0;
    prey->pcdata->villain_option_cooldown = 0;
    pred->pcdata->villain_option_cooldown = 0;
    prey->pcdata->prey_emote_cooldown = 0;
    pred->pcdata->prey_emote_cooldown = 0;
    prey->pcdata->villain_emote_cooldown = 0;
    pred->pcdata->villain_emote_cooldown = 0;

    ROOM_INDEX_DATA *fleeroom = get_fleeroom(prey, NULL);
    if(fleeroom != NULL)
    {
      char_from_room(prey);
      char_to_room(prey, fleeroom);
    }
  }

  bool fixation_target(CHAR_DATA *ch, CHAR_DATA *victim)
  {
    if(ch == NULL || victim == NULL || IS_NPC(ch) || IS_NPC(victim))
    return FALSE;

    if(ch->pcdata->fixation_timeout > current_time && !str_cmp(ch->pcdata->fixation_name, victim->name))
    return TRUE;

    return FALSE;
  }

  bool fixation_happy_room(CHAR_DATA *ch)
  {
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

      if(ch->pcdata->account != NULL && victim->pcdata->account != NULL && ch->pcdata->account == victim->pcdata->account)
      continue;

      if(ch == victim)
      continue;

      if (victim->in_room == ch->in_room)
      pop++;

      if(fixation_target(ch, victim))
      return TRUE;
    }
    if(pop > 0)
    return FALSE;
    return TRUE;

  }

  void prey_timeout(CHAR_DATA *prey, CHAR_DATA *pred)
  {
    prey_capture(pred, prey);

  }
  void villain_timeout(CHAR_DATA *pred, CHAR_DATA *prey)
  {
    if(pred->pcdata->building_fixation_level >= 2)
    prey_capture(pred, prey);
    else
    prey_escape(pred, prey);
  }

  _DOFUN(do_preyescape)
  {
    char arg1[MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg1);
    if(!str_cmp(arg1, ""))
    {
      send_to_char("Enter `Wescape`x followed by a number or word for your selection, Options are:\n1) Giveup [Let the pursue catch you and immediately end the patrol event]\n2) Breakout [Try and run past your pursuer, almost certain they will be able to grab you but maybe not!]\n3) Injure [Inflict some superficial damage or pain on your pursuer to try and deter them. `CIncreases Fixation`x]\n4) Distance [Do something reckless to try and gain distance, chance you might be injured. `CIncreases Fixation`x]\n5) Throwoff (object) [Discard an item to try and throw your pursuer off your trail. `CIncreases Fixation`x]\n6) Tussle [Tackle the pursuer and tussle with them just long enough to try and break away. `CIncreases Fixation`x]\n7) Blockade [Cause some sort of blockade to slow the path of your pursuer. `CIncreases Fixation`x]\n8) Witnesses [Briefly go somewhere or do something so witnesses protect you from the pursuer moving in. `CIncreases Fixation`x]\n9) Charm [Try to charm the pursuer so they are less inclined to want to hurt you somehow. `CMay Increase Fixation`x\n10) Pander [Give the pursuer something you think they want in the hopes they'll be less inclined to want to hurt you. `CMay Increase Fixation`x]\nPlease ensure you have read `cHelp Fixation`x\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "1") || !str_cmp(arg1, "Giveup"))
    {
      ch->pcdata->prey_option = PREY_OPTION_GIVEUP;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "2") || !str_cmp(arg1, "Breakout"))
    {
      ch->pcdata->prey_option = PREY_OPTION_DASH;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "3") || !str_cmp(arg1, "Injure"))
    {
      ch->pcdata->prey_option = PREY_OPTION_INJURE;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "4") || !str_cmp(arg1, "Distance"))
    {
      ch->pcdata->prey_option = PREY_OPTION_DISTANCE;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "5") || !str_cmp(arg1, "Throwoff"))
    {
      OBJ_DATA *obj = NULL;
      if ((obj = get_obj_carryhold(ch, argument, NULL)) == NULL) {
        if ((obj = get_obj_wear(ch, argument, TRUE)) == NULL) {
          send_to_char("Syntax: Escape 5 (object)\n\r", ch);
          return;
        }
      }
      ch->pcdata->prey_object = obj;

      ch->pcdata->prey_option = PREY_OPTION_TRAIL;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "6") || !str_cmp(arg1, "Tussle"))
    {
      ch->pcdata->prey_option = PREY_OPTION_TUSSLE;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "7") || !str_cmp(arg1, "Blockade"))
    {
      ch->pcdata->prey_option = PREY_OPTION_BLOCKADE;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "8") || !str_cmp(arg1, "Witnesses"))
    {
      ch->pcdata->prey_option = PREY_OPTION_PUBLIC;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "9") || !str_cmp(arg1, "Charm"))
    {
      ch->pcdata->prey_option = PREY_OPTION_CHARM;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "10") || !str_cmp(arg1, "Pander"))
    {
      ch->pcdata->prey_option = PREY_OPTION_PANDER;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }
    if(!str_cmp(arg1, "11") || !str_cmp(arg1, "Drawout"))
    {
      ch->pcdata->prey_option = PREY_OPTION_DRAWOUT;
      send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
      return;
    }

    else
    {
      send_to_char("Enter a number or word for your selection, Options are:\n1) Giveup [Let the pursue catch you and immediately end the patrol event]\n2) Breakout [Try and run past your pursuer, almost certain they will be able to grab you but maybe not!]\n3) Injure [Inflict some superficial damage or pain on your pursuer to try and deter them. `CIncreases Fixation`x]\n4) Distance [Do something reckless to try and gain distance, chance you might be injured. `CIncreases Fixation`x]5) Throwoff (object) [Discard an item to try and throw your pursuer off your trail. `CIncreases Fixation`x]\n6) Tussle [Tackle the pursuer and tussle with them just long enough to try and break away. `CIncreases Fixation`x]\n 7) Blockade [Cause some sort of blockade to slow the path of your pursuer. `CIncreases Fixation`x]\n 8) Witnesses [Briefly go somewhere or do something so witnesses protect you from the pursuer moving in. `CIncreases Fixation`x]\n 9) Charm [Try to charm the pursuer so they are less inclined to want to hurt you somehow. `CMay Increase Fixation`x\n 10) Pander [Give the pursuer something you think they want in the hopes they'll be less inclined to want to hurt you. `CMay Increase Fixation`x]\n 11) Drawout [Other form of drawing out the pursuit. `CMay Increase Fixationx]\nPlease ensure you have read `cHelp Fixation`x\n\r", ch);
      return;
    }
  }

  void process_prey_emote(CHAR_DATA *ch)
  {
    CHAR_DATA *pred = ch->pcdata->patrol_target;
    ch->pcdata->prey_option_cooldown = 120;
    ch->pcdata->prey_emote_cooldown = 120;
    if(ch->pcdata->prey_option == PREY_OPTION_GIVEUP)
    {
      prey_capture(pred, ch);
      return;
    }
    if(ch->pcdata->prey_option == PREY_OPTION_DASH)
    {
      act("$N is trying to make a break for it past you, use chase capture to grab them and capture them or chase release to let them past you.", pred, NULL, ch, TO_CHAR);
      pred->pcdata->villain_option = VILLAIN_OPTION_CHOOSING_GRAB;
      pred->pcdata->villain_option_cooldown = 10;
      pred->pcdata->villain_emote_cooldown = 15;
      return;
    }
    if(ch->pcdata->prey_option == PREY_OPTION_INJURE)
    {
      if(number_percent() % 2 == 0 && pred->wounds == 0)
      {
        wound_char(pred, 1);
        pred->heal_timer /= 5;
        send_to_char("You suffer a minor injury!\n\r", pred);
        printf_to_char(ch, "%s suffers a minor injury!\n\r", PERS(pred, ch));
      }
    }
    if(ch->pcdata->prey_option == PREY_OPTION_DISTANCE)
    {
      if(number_percent() % 2 == 0 && ch->wounds == 0)
      {
        wound_char(ch, 1);
        ch->heal_timer /= 5;
        send_to_char("You suffer a minor injury!\n\r", ch);
        printf_to_char(pred, "%s suffers a minor injury!\n\r", PERS(ch, pred));
      }
    }
    if(ch->pcdata->prey_option == PREY_OPTION_TUSSLE)
    {
      pred->pcdata->villain_grab = TRUE;
      send_to_char("In the tussle you can grab objects from your target, use grab (string) to do so.\n\r", pred);
    }
    if(ch->pcdata->prey_option == PREY_OPTION_TRAIL)
    {
      if(ch->pcdata->prey_object != NULL)
      {
        obj_from_char(ch->pcdata->prey_object);
        obj_to_char(ch->pcdata->prey_object, pred);
        printf_to_char(pred, "You find %s on the ground.\n\r", ch->pcdata->prey_object->short_descr);
      }
    }
    pred->pcdata->villain_option_cooldown = 10;
    pred->pcdata->villain_emote_cooldown = 15;
    pred->pcdata->villain_option = VILLAIN_OPTION_CHOOSING;

    if(ch->pcdata->prey_option == PREY_OPTION_CHARM || ch->pcdata->prey_option == PREY_OPTION_PANDER || ch->pcdata->prey_option == PREY_OPTION_DRAWOUT)
    {
      send_to_char("Choose from these responses using the `cchase`x (response) command.\n1) Continue [Continue the chase and increase your fixation level]\n2) Abandon [Abandon the chase and let the target escape]\n3) Drawout [Continue the chase without increasing your fixation level as a potential reward for their charm or pandering]\n4) Aggressive [Make an attempt at ending the pursuit now, will succeed or fail based on RNG]\n\r", pred);
    }
    else
    {
      send_to_char("Choose from these responses using the `cchase`x (response) command.\n1) Continue [Continue the chase and increase your fixation level]\n2) Abandon [Abandon the chase and let the target escape]\n4) Aggressive [Make an attempt at ending the pursuit now, will succeed or fail based on RNG]\n\r", pred);
    }
    return;
  }

  _DOFUN(do_villainchase)
  {
    char arg1[MSL];
    argument = one_argument(argument, arg1);
    CHAR_DATA *prey = ch->pcdata->patrol_target;

    if(prey == NULL)
    {
      send_to_char("You are not currently chasing anyone.\n\r", ch);
      return;
    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_CHOOSING_GRAB)
    {
      if(!str_cmp(arg1, ""))
      {
        send_to_char("Enter `Wchase`x followed by a number or word for your selection, Options are:\n1) Capture [Capture your target]\n2) Release [Let them go]\n\r", ch);
        return;
      }
      if(!str_cmp(arg1, "1") || !str_cmp(arg1, "Capture"))
      {
        ch->pcdata->villain_option = VILLAIN_OPTION_GRAB;
        send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
        return;
      }
      if(!str_cmp(arg1, "2") || !str_cmp(arg1, "Release"))
      {
        ch->pcdata->villain_option = VILLAIN_OPTION_MISS;
        send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
        return;
      }
      else
      {
        send_to_char("Enter a number or word for your selection, Options are:\n1) Capture [Capture your target]\n2) Release [Let them go]\n\r", ch);
        return;
      }
    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_CHOOSING)
    {
      if(prey->pcdata->prey_option == PREY_OPTION_CHARM || prey->pcdata->prey_option == PREY_OPTION_PANDER || prey->pcdata->prey_option == PREY_OPTION_DRAWOUT)
      {
        if(!str_cmp(arg1, ""))
        {
          send_to_char("Enter a number or word for your selection, Options are:\n1) Continue [Continue the chase and increase your fixation level]\n2) Abandon [Abandon the chase and let the target escape]\n3) Drawout [Continue the chase without increasing your fixation level as a potential reward for their charm or pandering]\n4) Aggressive [Make an attempt at ending the pursuit now, will succeed or fail based on RNG]\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "1") || !str_cmp(arg1, "Continue"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_CHASE;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "2") || !str_cmp(arg1, "Abandon"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_ABANDON;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "3") || !str_cmp(arg1, "Drawout"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_DRAWOUT;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "4") || !str_cmp(arg1, "Aggressive"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_AGGRESSIVE;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        else
        {
          send_to_char("Enter a number or word for your selection, Options are:\n1) Continue [Continue the chase and increase your fixation level]\n2) Abandon [Abandon the chase and let the target escape]\n3) Drawout [Continue the chase without increasing your fixation level as a potential reward for their charm or pandering]\n4) Aggressive [Make an attempt at ending the pursuit now, will succeed or fail based on RNG]\n \r", ch);
          return;
        }
      }
      else
      {
        if(!str_cmp(arg1, ""))
        {
          send_to_char("Enter a number or word for your selection, Options are:\n1) Continue [Continue the chase and increase your fixation level]\n2) Abandon [Abandon the chase and let the target escape]\n4) Aggressive [Make an attempt at ending the pursuit now, will succeed or fail based on RNG]\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "1") || !str_cmp(arg1, "Continue"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_CHASE;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "2") || !str_cmp(arg1, "Abandon"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_ABANDON;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        if(!str_cmp(arg1, "4") || !str_cmp(arg1, "Aggressive"))
        {
          ch->pcdata->villain_option = VILLAIN_OPTION_AGGRESSIVE;
          send_to_char("Option chosen, will take effect after your next emote.\n\r", ch);
          return;
        }
        else
        {
          send_to_char("Enter a number or word for your selection, Options are:\n1) Continue [Continue the chase and increase your fixation level]\n2) Abandon [Abandon the chase and let the target escape]\n4) Aggressive [Make an attempt at ending the pursuit now, will succeed or fail based on RNG]\n\r", ch);
          return;
        }
      }
    }
  }

  void process_villain_emote(CHAR_DATA *ch)
  {
    CHAR_DATA *prey = ch->pcdata->patrol_target;
    ch->pcdata->villain_option_cooldown = 120;
    ch->pcdata->villain_emote_cooldown = 120;
    if(ch->pcdata->villain_option == VILLAIN_OPTION_GRAB)
    {
      prey_capture(ch, prey);
      return;
    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_MISS)
    {
      prey_escape(ch, prey);
      return;
    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_ABANDON)
    {
      prey_escape(ch, prey);
      return;
    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_DRAWOUT)
    {
      ch->pcdata->villain_option = VILLAIN_OPTION_WAITING;
      prey->pcdata->prey_option = PREY_OPTION_CHOOSING;
      prey->pcdata->prey_option_cooldown = 10;
      prey->pcdata->prey_emote_cooldown = 15;
      act("$n is still pursing you. (You have 5 minutes to choose an action, type 'escape' by itself to see your options)", ch, NULL, prey, TO_VICT);
      printf_to_char(ch, "Current Fixation Level: `c%d`x\n\r", ch->pcdata->building_fixation_level);
      printf_to_char(prey, "Current Fixation Level: `c%d`x\n\r", ch->pcdata->building_fixation_level);

    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_AGGRESSIVE)
    {
      int chance = 10 + ch->pcdata->building_fixation_level*10;
      if(number_percent() < chance)
      {
        prey_capture(ch, prey);
        return;
      }
      else
      {
        ch->pcdata->building_fixation_level++;
        ch->pcdata->villain_option = VILLAIN_OPTION_WAITING;
        prey->pcdata->prey_option = PREY_OPTION_CHOOSING;
        prey->pcdata->prey_option_cooldown = 10;
        prey->pcdata->prey_emote_cooldown = 15;
        act("$n makes an attempt to aggressively end the pursuit early but it fails. $e is still pursing you. (You have 5 minutes to choose an action, type 'escape' by itself to see your options)", ch, NULL, prey, TO_VICT);
        printf_to_char(ch, "Current Fixation Level: `c%d`x\n\r", ch->pcdata->building_fixation_level);
        printf_to_char(prey, "Current Fixation Level: `c%d`x\n\r", ch->pcdata->building_fixation_level);
      }

    }
    if(ch->pcdata->villain_option == VILLAIN_OPTION_CHASE)
    {
      if(ch->pcdata->building_fixation_level >= 10)
      {
        prey_capture(ch, prey);
        return;
      }
      else
      {
        ch->pcdata->building_fixation_level++;
        ch->pcdata->villain_option = VILLAIN_OPTION_WAITING;
        prey->pcdata->prey_option = PREY_OPTION_CHOOSING;
        prey->pcdata->prey_option_cooldown = 10;
        prey->pcdata->prey_emote_cooldown = 15;
        act("$n is still pursing you. (You have 5 minutes to choose an action, type 'escape' by itself to see your options)", ch, NULL, prey, TO_VICT);
        printf_to_char(ch, "Current Fixation Level: `c%d`x\n\r", ch->pcdata->building_fixation_level);
        printf_to_char(prey, "Current Fixation Level: `c%d`x\n\r", ch->pcdata->building_fixation_level);
      }
    }
  }

  _DOFUN(do_grab)
  {
    CHAR_DATA *prey = ch->pcdata->patrol_target;
    if(ch->pcdata->villain_grab == FALSE)
    {
      send_to_char("You are not currently tussling with your target.\n\r", ch);
      return;
    }
    if(prey == NULL)
    {
      send_to_char("You are not currently chasing anyone.\n\r", ch);
      return;
    }
    OBJ_DATA *obj = NULL;
    if ((obj = get_obj_carryhold(prey, argument, NULL)) == NULL) {
      if ((obj = get_obj_wear(prey, argument, TRUE)) == NULL) {
        send_to_char("They don't seem to have anything like that to grab.\n\r", ch);
        return;
      }
    }
    obj_from_char(obj);
    obj_to_char(obj, ch);
    printf_to_char(ch, "You grab %s.\n\r", obj->short_descr);
    printf_to_char(prey, "%s grabs %s.\n\r", PERS(ch, prey), obj->short_descr);
    if(number_percent() % 3 == 0)
    {
      ch->pcdata->villain_grab = FALSE;
      send_to_char("You cannot grab any more items.\n\r", ch);
    }
    else
    {
      send_to_char("You can grab another item.\n\r", ch);
    }

  }

  _DOFUN(do_bodycompel)
  {
    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }
    if (IS_NPC(victim)) {
      send_to_char("Not on NPCs.\n\r", ch);
      return;
    }

    if(ch->pcdata->fixation_timeout == 0 || str_cmp(ch->pcdata->fixation_name, victim->name))
    {
      send_to_char("You are not currently fixated on them.\n\r", ch);
      return;
    }
    if(ch->pcdata->fixation_charges <= 0)
    {
      send_to_char("You do not have any fixation charges left.\n\r", ch);
      return;
    }
    if(ch->pcdata->fixation_level < 1)
    {
      send_to_char("You do not have a high enough fixation level to use this ability.\n\r", ch);
      return;
    }

    auto_imprint_timed(victim, argument, IMPRINT_BODYCOMPULSION, 300*ch->pcdata->fixation_level);
    send_to_char("Done.\n\r", ch);
    ch->pcdata->fixation_charges--;

  }



  void influencer_process(void) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    d.original = NULL;
    int maxf_one = 0;
    int maxf_two = 0;
    int maxf_three = 0;
    char iname_one[MSL];
    char iname_two[MSL];
    char iname_three[MSL];
    sprintf(iname_one, "A");
    sprintf(iname_two, "B");
    sprintf(iname_three, "C");

    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      online = FALSE;
      (*it)->valid = FALSE;
      if ((victim = get_char_world_pc((*it)->charname)) !=
          NULL) // Victim is online.
      online = TRUE;
      else {
        if (!load_char_obj(&d, (*it)->charname)) {
          continue;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize((*it)->charname));
        stat(buf, &sb);
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);
        continue;
      }
      if (IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->guest_type != GUEST_NIGHTMARE) {
        if (!online)
        free_char(victim);
        continue;
      }

      if (IS_FLAG(victim->act, PLR_STASIS) || higher_power(victim)) {
        if (!online)
        free_char(victim);
        continue;
      }
      if (IS_FLAG(victim->act, PLR_INFLUENCER))
      {
        REMOVE_FLAG(victim->act, PLR_INFLUENCER);
        save_char_obj(victim, FALSE, FALSE);
      }
      PROFILE_TYPE *prof = profile_lookup(victim->name);
      if(prof == NULL || prof->rating_count < 2)
      {
        if (!online)
        free_char(victim);
        continue;
      }
      int inf_score = influencer_score(victim);
      if(inf_score > maxf_one)
      {
        sprintf(iname_three, "%s", iname_two);
        maxf_three = maxf_two;
        sprintf(iname_two, "%s", iname_one);
        maxf_two = maxf_one;
        sprintf(iname_one, "%s", victim->name);
        maxf_one = inf_score;
      }
      else if(inf_score > maxf_two)
      {
        sprintf(iname_three, "%s", iname_two);
        maxf_three = maxf_two;
        sprintf(iname_two, "%s", victim->name);
        maxf_two = inf_score;
      }
      else if(inf_score > maxf_three)
      {
        sprintf(iname_three, "%s", victim->name);
        maxf_three = inf_score;
      }
      if(!online)
      free_char(victim);
    }
    if(strlen(iname_one) > 2)
    offline_setflag(iname_one, PLR_INFLUENCER);
    if(strlen(iname_two) > 2)
    offline_setflag(iname_two, PLR_INFLUENCER);
    if(strlen(iname_three) > 2)
    offline_setflag(iname_three, PLR_INFLUENCER);
  }

  _DOFUN(do_runinfluencer)
  {
    influencer_process();

  }

  _DOFUN(do_patrolghost)
  {
    launch_patrolevent(ch, PATROL_GHOST);
    return;
  }


#if defined(__cplusplus)
}
#endif

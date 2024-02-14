/*
* This is so hot right now
*/

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


  int get_jewelry_cost(CHAR_DATA *ch) {

    int iWear;
    OBJ_DATA *obj;
    int cost = 0;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->item_type == ITEM_JEWELRY && can_see_wear(ch, iWear))
        cost += obj->cost;
        else if (obj->item_type == ITEM_JEWELRY)
        cost += obj->cost / 3;
      }
    }
    cost /= 100;
    return cost;
  }
  int get_clothes_cost(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    int cost = 0;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->item_type == ITEM_CLOTHING && can_see_wear(ch, iWear))
        cost += obj->cost;
        else if (obj->item_type == ITEM_CLOTHING)
        cost += obj->cost / 3;
      }
    }
    cost /= 100;
    return cost;
  }

  int get_obj_cover_amount(OBJ_DATA *obj, CHAR_DATA *ch, int iWear) {

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
    return points;
  }

  int get_cover_amount(CHAR_DATA *ch) {
    int iWear;
    int warmth = 0;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        warmth += get_obj_cover_amount(get_eq_char(ch, iWear), ch, iWear);
      }
    }
    return warmth;
  }

  bool has_shapewear_body(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->item_type == ITEM_CLOTHING && obj->value[2] > 30)
        return TRUE;
        if (obj->item_type == ITEM_CLOTHING && is_name("corset", obj->name))
        return TRUE;
        if (obj->item_type == ITEM_CLOTHING && is_name("shapewear", obj->name))
        return TRUE;
        if (obj->item_type == ITEM_CLOTHING && is_name("girdle", obj->name))
        return TRUE;
      }
    }
    return FALSE;
  }
  bool has_shapewear_height(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->item_type == ITEM_CLOTHING && obj->value[2] > 1 && obj->value[2] < 20)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool attract_cap(CHAR_DATA *ch)
  {
    if(get_tier(ch) < 2 || ch->pcdata->attract_count < 50.0)
    return TRUE;

    return FALSE;
  }

  float fashion_score(CHAR_DATA *ch) {
    float cost = (sqrt((float)get_jewelry_cost(ch)) / 4) +
    (sqrt((float)get_clothes_cost(ch)) / 2);
    return cost;
  }

  float skin_face_score(CHAR_DATA *ch) {
    float bval = 100 + (ch->pcdata->attract[ATTRACT_MAKEUP]*ch->pcdata->attract[ATTRACT_MAKEUP]) * 20;
    int mins = (current_time - ch->pcdata->last_shower) / 60;
    int minsq = mins*mins;
    float reduc = cbrt((float)minsq);
    bval -= reduc;
    if(bval < 0)
    bval = 0.0;
    return bval;
  }
  float phys_score(CHAR_DATA *ch) {
    int expo = 0;
    for (int j = 0; j < MAX_COVERS; j++) {
      if (!is_covered(ch, j))
      expo += 1;
    }
    float exposed = expo * expo;
    exposed -= get_cover_amount(ch);
    return exposed;
  }

  int fashion_boost(CHAR_DATA *ch, bool boosted) {
    int boost = 0;

    int val = get_skill(ch, SKILL_FASHION);

    if (val > get_skill(ch, SKILL_BEAUTY) && val > get_skill(ch, SKILL_CONDITIONING))
    boost = boost + val * 12;
    else if (val > get_skill(ch, SKILL_BEAUTY) || val > get_skill(ch, SKILL_CONDITIONING))
    boost = boost + val * 10;
    else
    boost = boost + val * 8;


    if(college_student(ch, TRUE) && (college_group(ch, FALSE) == COLLEGE_PREP || college_group(ch, FALSE) == COLLEGE_THEATRE || college_group(ch, FALSE) == COLLEGE_JOCK || college_group(ch, FALSE) == COLLEGE_GOTH))
    boost = boost - 2;

    if(college_group(ch, FALSE) == COLLEGE_PREP)
    boost = boost + 6;
    if(college_group(ch, FALSE) == COLLEGE_THEATRE)
    boost = boost + 3;



    if (has_shapewear_height(ch))
    boost = boost + 2;
    if (has_shapewear_body(ch))
    boost = boost + 2;
    ROOM_INDEX_DATA *inroom = ch->in_room;

    if (daylight_level(inroom) + light_level(inroom) < 50)
    boost = boost + 10;
    else if (daylight_level(inroom) + light_level(inroom) > 150)
    boost = boost * 0.8;

    if (boosted == FALSE && boost > 0)
    boost = (int)sqrt(boost);
    return boost;
  }

  int face_skin_boost(CHAR_DATA *ch, bool boosted) {
    int boost = 0;
    int val = get_skill(ch, SKILL_BEAUTY);
    if (val > get_skill(ch, SKILL_FASHION) && val > get_skill(ch, SKILL_CONDITIONING))
    boost = boost + val * 12;
    else if (val > get_skill(ch, SKILL_FASHION) || val > get_skill(ch, SKILL_CONDITIONING))
    boost = boost + val * 10;
    else
    boost = boost + val * 8;

    ROOM_INDEX_DATA *inroom = ch->in_room;

    if(college_student(ch, TRUE) && (college_group(ch, FALSE) == COLLEGE_PREP || college_group(ch, FALSE) == COLLEGE_THEATRE || college_group(ch, FALSE) == COLLEGE_JOCK || college_group(ch, FALSE) == COLLEGE_GOTH))
    boost = boost - 2;

    if(college_group(ch, FALSE) == COLLEGE_GOTH)
    boost = boost + 6;
    if(college_group(ch, FALSE) == COLLEGE_THEATRE)
    boost = boost + 3;

    if (ch->pcdata->perfume_cost >= 2500)
    boost = boost + 3;
    else if (ch->pcdata->perfume_cost >= 500)
    boost = boost + 1;

    if (get_tier(ch) == 3)
    boost = boost + 2;
    else if (get_tier(ch) == 4)
    boost = boost + 4;
    else if (get_tier(ch) == 5)
    boost = boost + 6;

    if (daylight_level(inroom) + light_level(inroom) < 50)
    boost = boost * 0.8;
    else if (daylight_level(inroom) + light_level(inroom) > 150)
    boost = boost * 0.8;
    else
    boost = boost + 10;

    if (boosted == FALSE && boost > 0)
    boost = (int)sqrt(boost);

    return boost;
  }

  int phys_boost(CHAR_DATA *ch, bool boosted) {
    int boost = 0;
    int val = get_skill(ch, SKILL_CONDITIONING);
    if (val > get_skill(ch, SKILL_FASHION) && val > get_skill(ch, SKILL_BEAUTY))
    boost = boost + val * 12;
    else if (val > get_skill(ch, SKILL_FASHION) || val > get_skill(ch, SKILL_BEAUTY))
    boost = boost + val * 10;
    else
    boost = boost + val * 8;

    ROOM_INDEX_DATA *inroom = ch->in_room;

    if(college_student(ch, TRUE) && (college_group(ch, FALSE) == COLLEGE_PREP || college_group(ch, FALSE) == COLLEGE_THEATRE || college_group(ch, FALSE) == COLLEGE_JOCK || college_group(ch, FALSE) == COLLEGE_GOTH))
    boost = boost - 2;

    if(college_group(ch, FALSE) == COLLEGE_JOCK)
    boost = boost + 6;

    if (has_shapewear_height(ch))
    boost = boost + 2;
    if (has_shapewear_body(ch))
    boost = boost + 2;

    if (get_tier(ch) == 3)
    boost = boost + 2;
    else if (get_tier(ch) == 4)
    boost = boost + 4;
    else if (get_tier(ch) == 5)
    boost = boost + 6;

    if (IS_AFFECTED(ch, AFF_FERTILITY))
    boost += 10;

    boost = boost * 0.8;

    if (daylight_level(inroom) + light_level(inroom) < 50)
    boost = boost * 0.8;
    else if (daylight_level(inroom) + light_level(inroom) > 150)
    boost = boost + 10;

    if (boosted == FALSE && boost > 0)
    boost = (int)sqrt(boost);

    return boost;
  }

  void attract_average_update(CHAR_DATA *ch) {
    if (!other_players(ch) || is_dreaming(ch))
    return;

    float pcs = sqrt(pc_pop(ch->in_room));
    ch->pcdata->attract_count += pcs;
    ch->pcdata->attract_count_fashion += fashion_score(ch) * pcs;
    ch->pcdata->attract_count_phys += phys_score(ch) * pcs;
    ch->pcdata->attract_count_skin += skin_face_score(ch) * pcs;
  }

  bool is_fashion_boost(CHAR_DATA *ch) {
    if (fashion_score(ch) >
        (ch->pcdata->attract_count_fashion / (UMAX(1, ch->pcdata->attract_count))))
    return TRUE;
    return FALSE;
  }

  bool is_face_skin_boost(CHAR_DATA *ch) {
    if (skin_face_score(ch) >
        (ch->pcdata->attract_count_skin / (UMAX(1, ch->pcdata->attract_count))))
    return TRUE;
    return FALSE;
  }

  bool is_phys_boost(CHAR_DATA *ch) {
    if (phys_score(ch) >
        (ch->pcdata->attract_count_phys / (UMAX(1, ch->pcdata->attract_count))))
    return TRUE;
    return FALSE;
  }

  int prom_mod(CHAR_DATA *ch) {
    int val = 0;

    int prom = ch->pcdata->attract[ATTRACT_PROM];

    if (prom >= 750)
    val = -7;
    else if (prom >= 500)
    val = -5;
    else if (prom >= 300)
    val = -3;
    else if (prom >= 200)
    val = -1;

    return val;
  }

  int praise_mod(CHAR_DATA *ch) {
    int val = 0;

    if (ch->pcdata->attract[ATTRACT_PRAISE] >= 10000)
    val = 10;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= 7500)
    val = 9;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= 5000)
    val = 7;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= 3000)
    val = 5;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= 1500)
    val = 3;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= 500)
    val = 1;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= 0)
    val = 0;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= -1000)
    val = -1;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= -2000)
    val = -2;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= -3000)
    val = -3;
    else if (ch->pcdata->attract[ATTRACT_PRAISE] >= -4000)
    val = -4;
    else
    val = -5;

    return val;
  }

  int standards_mod(CHAR_DATA *ch) {
    int val = 0;

    if (ch->pcdata->attract[ATTRACT_STANDARDS] >= 50)
    val = 5;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] >= 40)
    val = 4;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] >= 30)
    val = 3;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] >= 20)
    val = 2;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] >= 10)
    val = 1;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] <= -50)
    val = -8;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] <= -40)
    val = -6;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] <= -30)
    val = -4;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] <= -20)
    val = -2;
    else if (ch->pcdata->attract[ATTRACT_STANDARDS] <= -10)
    val = -1;

    return val;
  }

  int character_mod(CHAR_DATA *ch) {
    int mod = 0;
    mod += prom_mod(ch);
    mod += praise_mod(ch);
    mod += standards_mod(ch);
    if (status_bonus(ch) >= 5 || cruiser_bonus(ch) >= 5)
    mod += 2;

    mod = UMAX(mod, -10);
    mod = UMIN(mod, 5);
    return mod;
  }

  int attractive_rare_bonus(CHAR_DATA *ch) {
    int cond = 0;
    int beauty = 0;
    int fashion = 0;
    int maxoutfit = 0;
    int bonus = 0;
    if(is_alone(ch))
    return 0;

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
        if (higher_power(to) || is_gm(to))
        continue;
        if(to->in_room != ch->in_room)
        continue;
        if(to == ch)
        continue;

        if(fashion_score(ch) + sqrt((float)ch->pcdata->perfume_cost) > maxoutfit)
        maxoutfit = fashion_score(to) + sqrt((float)to->pcdata->perfume_cost);



        if (get_skill(to, SKILL_CONDITIONING) > get_skill(to, SKILL_BEAUTY) && get_skill(to, SKILL_CONDITIONING) > get_skill(to, SKILL_FASHION))
        cond += get_skill(to, SKILL_CONDITIONING);
        if (get_skill(to, SKILL_BEAUTY) > get_skill(to, SKILL_CONDITIONING) && get_skill(to, SKILL_BEAUTY) > get_skill(to, SKILL_FASHION))
        beauty += get_skill(to, SKILL_BEAUTY);
        if (get_skill(to, SKILL_FASHION) > get_skill(to, SKILL_CONDITIONING) && get_skill(to, SKILL_FASHION) > get_skill(to, SKILL_BEAUTY))
        fashion += get_skill(to, SKILL_FASHION);
      }
    }

    if (get_skill(ch, SKILL_CONDITIONING) > get_skill(ch, SKILL_BEAUTY) && get_skill(ch, SKILL_CONDITIONING) > get_skill(ch, SKILL_FASHION)) {

      if (cond > beauty && cond > fashion)
      bonus = -2;
      else if (cond < beauty && cond < fashion)
      bonus = 4;
      else
      bonus = 0;
    }
    if (get_skill(ch, SKILL_BEAUTY) > get_skill(ch, SKILL_CONDITIONING) && get_skill(ch, SKILL_BEAUTY) > get_skill(ch, SKILL_FASHION)) {
      if (beauty > cond && beauty > fashion)
      bonus = -2;
      else if (beauty < cond && beauty < fashion)
      bonus = 4;
      else
      bonus = 0;
    }
    if (get_skill(ch, SKILL_FASHION) > get_skill(ch, SKILL_CONDITIONING) && get_skill(ch, SKILL_FASHION) > get_skill(ch, SKILL_BEAUTY)) {

      if (fashion > cond && fashion > beauty)
      bonus = -2;
      else if (fashion < cond && fashion < beauty)
      bonus = 4;
      else
      bonus = 0;
    }
    if(fashion_score(ch) + sqrt((float)ch->pcdata->perfume_cost) > maxoutfit*9/10)
    bonus += 5;

    return bonus;
  }

  bool knows_character(CHAR_DATA *ch, CHAR_DATA *looker) {
    NameMap::const_iterator nit;
    if (ch->sex == SEX_FEMALE) {
      if ((nit = looker->pcdata->female_names->find(ch->id)) !=
          looker->pcdata->female_names->end()) {
        return TRUE;
      }
    }
    else if (ch->sex == SEX_MALE) {
      if ((nit = looker->pcdata->male_names->find(ch->id)) !=
          looker->pcdata->male_names->end()) {
        return TRUE;
      }
    }
    return FALSE;
  }

  int viewer_mods(CHAR_DATA *ch, CHAR_DATA *victim) {
    int mod = 0;

    char string[MSL];

    string[0] = '\0';

    strcat(string, from_color(get_focused(victim, victim, TRUE)));
    if (safe_strlen(string) < 200)
    mod -= 5;
    if (safe_strlen(string) < 80)
    mod -= 5;
    if (safe_strlen(string) > 3000)
    mod -= 5;

    if (get_tier(ch) >= 3 && get_tier(victim) < 3 && victim->pcdata->attract[ATTRACT_PROM] < 100) {
      if (alt_count(victim) <= 1 && victim->lf_taken <= 25 && (victim->pcdata->intel >= 2500 || victim->pcdata->heroic >= 50)) {
        mod += 5;
      }
    }

    if ((is_vampire(ch) && get_skill(ch, SKILL_OBFUSCATE) == 0) && is_werewolf(victim))
    mod -= 15;
    if (is_vampire(victim) && (is_werewolf(ch) && get_skill(ch, SKILL_OBFUSCATE) == 0))
    mod -= 15;
    if (get_drunk(ch) > 0)
    mod += get_drunk(ch) / 5;

    if (ch->pcdata->habit[HABIT_ORIENTATION] == 0 && ch->sex == victim->sex)
    mod -= 15;

    if (ch->pcdata->habit[HABIT_ORIENTATION] == 1 && ch->sex != victim->sex)
    mod -= 15;

    if (get_age(ch) < get_age(victim) / 2)
    mod -= 10;
    if (get_age(victim) < get_age(ch) / 2)
    mod -= 10;

    if (knows_character(victim, ch) && victim->played / 3600 > 100) {
      mod = mod + character_mod(victim);
      mod = mod + attractive_rare_bonus(victim);
    }
    else {
      mod = mod + 5;
      mod = mod + UMIN(0, attractive_rare_bonus(victim) * 2);
    }
    return mod;
  }

  int get_attract(CHAR_DATA *ch, CHAR_DATA *looker) {

    if(ch == NULL || IS_NPC(ch))
    return 25;
    if(ch->pcdata == NULL)
    return 25;

    if(looker != NULL && (IS_NPC(looker) || looker->pcdata == NULL))
    return 25;

    if(ch->pcdata->attract_count < 10.0)
    ch->pcdata->attract_count = 10.0;
    if(ch->pcdata->attract_count_fashion < 50.0)
    ch->pcdata->attract_count_fashion = 50.0;
    if(ch->pcdata->attract_count_phys < 2500.0)
    ch->pcdata->attract_count_phys = 2500.0;
    if(ch->pcdata->attract_count_skin < 800.0)
    ch->pcdata->attract_count_skin = 800.0;

    if(ch->pcdata->attract_count_fashion/(ch->pcdata->attract_count) < 4.5)
    ch->pcdata->attract_count_fashion = 4.5 * (ch->pcdata->attract_count);
    if(ch->pcdata->attract_count_phys/(ch->pcdata->attract_count) < 230.0)
    ch->pcdata->attract_count_phys = 230.0 * (ch->pcdata->attract_count);
    if(ch->pcdata->attract_count_skin/(ch->pcdata->attract_count) < 75.0)
    ch->pcdata->attract_count_skin = 75.0 * (ch->pcdata->attract_count);


    int base = 50;

    if(ch->pcdata->boon == BOON_BEAUTY && ch->pcdata->boon_timeout > current_time)
    return 100;

    if (get_skill(ch, SKILL_ATTRACTIVENESS) > 0)
    base = 65;
    if (get_skill(ch, SKILL_ATTRACTIVENESS) < 0)
    base = 10;

    int fash_boost = fashion_boost(ch, is_fashion_boost(ch));
    int skin_boost = face_skin_boost(ch, is_face_skin_boost(ch));
    int physical_boost = phys_boost(ch, is_phys_boost(ch));

    if (fash_boost >= skin_boost && fash_boost >= physical_boost) {
      base += fash_boost;
      base += UMAX(skin_boost / 2, (int)sqrt((float)skin_boost));
      base += UMAX(physical_boost / 2, (int)sqrt((float)physical_boost));
    }
    else if (skin_boost >= fash_boost && skin_boost >= physical_boost) {
      base += skin_boost;
      base += UMAX(fash_boost / 2, (int)sqrt((float)fash_boost));
      base += UMAX(physical_boost / 2, (int)sqrt((float)physical_boost));
    }
    else {
      base += physical_boost;
      base += UMAX(fash_boost / 2, (int)sqrt((float)fash_boost));
      base += UMAX(skin_boost / 2, (int)sqrt((float)physical_boost));
    }
    if (looker != NULL) {
      if (!same_player(ch, looker)) {
        base += viewer_mods(looker, ch);
      }
      else
      base += ch->pcdata->selfesteem;
    }
    base = base * attract_lifeforce(ch) / 100;

    if(attract_cap(ch))
    base = UMIN(base, 78);

    if(looker != NULL)
    {
      if(looker->pcdata->fixation_timeout > current_time)
      {
        if(fixation_target(looker, ch))
        {
          base += 50;
        }
        else base = UMIN(base, 50);
      }
    }

    if (base <= 75) {
      base = base / 5;
      base = base * 5;
      return base;
    }

    base = base - 75;
    base = base / 2;
    base = base + 75;
    if (base <= 90) {
      base = base / 2;
      base = base * 2;
      return base;
    }
    int bonus = 0;
    base = base - 90;
    for (int i = 2; base > 0; i = i * 3/2) {
      bonus++;
      base = base - i;
    }
    int nbase = 90 + bonus;
    nbase = UMIN(100, nbase);


    if(looker != NULL)
    {
      if(looker->pcdata->curse == CURSE_PURITANICAL && looker->pcdata->curse_timeout > current_time && (ch->pcdata->curse != CURSE_PURITANICAL || ch->pcdata->curse_timeout < current_time))
      {
        int aatt = get_attract(looker, looker);
        if(aatt > nbase)
        nbase = UMIN(nbase, 25);
      }
    }


    return nbase;
  }

  int get_attract_checkout(CHAR_DATA *ch, CHAR_DATA *looker, int ctype) {
    int base = 50;
    if (get_skill(ch, SKILL_ATTRACTIVENESS) > 0)
    base = 65;
    if (get_skill(ch, SKILL_ATTRACTIVENESS) < 0)
    base = 10;

    int fash_boost = fashion_boost(ch, is_fashion_boost(ch));
    int skin_boost = face_skin_boost(ch, is_face_skin_boost(ch));
    int physical_boost = phys_boost(ch, is_phys_boost(ch));

    if (ctype == 1) {
      base += fash_boost;
      base += UMAX(skin_boost / 2, (int)sqrt((float)skin_boost));
      base += UMAX(physical_boost / 2, (int)sqrt((float)physical_boost));
    }
    else if (ctype == 2) {
      base += skin_boost;
      base += UMAX(fash_boost / 2, (int)sqrt((float)fash_boost));
      base += UMAX(physical_boost / 2, (int)sqrt((float)physical_boost));
    }
    else {
      base += physical_boost;
      base += UMAX(fash_boost / 2, (int)sqrt((float)fash_boost));
      base += UMAX(skin_boost / 2, (int)sqrt((float)physical_boost));
    }
    if (looker != NULL) {
      if (!same_player(ch, looker)) {
        base += viewer_mods(looker, ch);
      }
      else
      base += ch->pcdata->selfesteem;
    }
    base = base * attract_lifeforce(ch) / 100;

    if(attract_cap(ch))
    base = UMIN(base, 78);


    if (base <= 75) {
      base = base / 5;
      base = base * 5;
      return base;
    }

    base = base - 75;
    base = base / 2;
    base = base + 75;
    if (base <= 90) {
      base = base / 2;
      base = base * 2;
      return base;
    }
    int bonus = 0;
    base = base - 90;
    for (int i = 2; base > 0; i = i * 3/2) {
      bonus++;
      base = base - i;
    }
    int nbase = 90 + bonus;
    nbase = UMIN(100, nbase);
    return nbase;
  }

  int cover_count(CHAR_DATA *ch) {
    int count = 0;
    for (int i = 0; i < MAX_COVERS; i++) {
      if (is_covered(ch, cover_table[i]))
      count++;
    }
    return count;
  }

  _DOFUN(do_makeup) {
    if (!str_cmp(argument, "remove")) {
      if (ch->pcdata->attract[ATTRACT_MAKEUP] == 0) {
        send_to_char("You're already not wearing makeup.\n\r", ch);
        return;
      }
      ch->pcdata->attract[ATTRACT_MAKEUP] = 0;
      send_to_char("You remove your makeup.\n\r", ch);
      act("$n removes $s makeup.\n\r", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(argument, "light")) {
      if (ch->pcdata->attract[ATTRACT_MAKEUP] == 1) {
        send_to_char("You're already wearing light makeup.\n\r", ch);
        return;
      }

      ch->pcdata->attract[ATTRACT_MAKEUP] = 1;
      send_to_char("You apply light makeup.\n\r", ch);
      act("$n applies light makeup.\n\r", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(argument, "medium")) {
      if (ch->pcdata->attract[ATTRACT_MAKEUP] == 2) {
        send_to_char("You're already wearing medium makeup.\n\r", ch);
        return;
      }
      ch->pcdata->attract[ATTRACT_MAKEUP] = 2;
      send_to_char("You apply medium makeup.\n\r", ch);
      act("$n applies medium makeup.\n\r", ch, NULL, NULL, TO_ROOM);
    }
    else if (!str_cmp(argument, "heavy")) {
      if (ch->pcdata->attract[ATTRACT_MAKEUP] == 3) {
        send_to_char("You're already wearing heavy makeup.\n\r", ch);
        return;
      }
      ch->pcdata->attract[ATTRACT_MAKEUP] = 3;
      send_to_char("You apply heavy makeup.\n\r", ch);
      act("$n applies heavy makeup.\n\r", ch, NULL, NULL, TO_ROOM);
    }
    else
    send_to_char("Syntax: makeup remove/light/medium/heavy.\n\r", ch);
  }

  int clothes_count(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int count = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && (obj->item_type == ITEM_CLOTHING || obj->item_type == ITEM_JEWELRY)) {
        count++;
      }
    }
    return count;
  }
  bool has_detail(CHAR_DATA *ch) {
    int i;
    for (i = 0; i < MAX_COVERS; i++) {
      if (safe_strlen(ch->pcdata->detail_over[i]) > 10)
      return TRUE;
      if (safe_strlen(ch->pcdata->detail_under[i]) > 10)
      return TRUE;
    }
    return FALSE;
  }

  bool has_focused(CHAR_DATA *ch) {
    int i;
    for (i = 0; i < MAX_COVERS; i++) {
      if (safe_strlen(ch->pcdata->focused_descs[i]) > 10)
      return TRUE;
    }
    return FALSE;
  }

  _DOFUN(do_icheckout) {


    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);
    victim = get_char_world_pc(arg1);
    printf_to_char(ch, "%s%0.1f`x out of 10\n", attract_color(get_attract(victim, ch)), ((float)get_attract(victim, ch))/10.0);

    if (is_fashion_boost(victim))
    printf_to_char(ch, "*Clothes: %s%0.1f`x out of 10\n", attract_color(get_attract_checkout(victim, ch, 1)), ((float)get_attract_checkout(victim, ch, 1))/10.0);
    else
    printf_to_char(ch, "Clothes: %s%0.1f`x out of 10\n", attract_color(get_attract_checkout(victim, ch, 1)), ((float)get_attract_checkout(victim, ch, 1))/10.0);
    if ( is_face_skin_boost(victim))
    printf_to_char(ch, "*Skin/Face/Hair: %s%0.1f`x out of 10\n", attract_color(get_attract_checkout(victim, ch, 2)), ((float)get_attract_checkout(victim, ch, 2))/10.0);
    else
    printf_to_char(ch, "Skin/Face/Hair: %s%0.1f`x out of 10\n", attract_color(get_attract_checkout(victim, ch, 2)), ((float)get_attract_checkout(victim, ch, 2))/10.0);
    if (is_phys_boost(victim))
    printf_to_char(ch, "*Body: %s%0.1f`x out of 10\n", attract_color(get_attract_checkout(victim, ch, 3)), ((float)get_attract_checkout(victim, ch, 3))/10.0);
    else
    printf_to_char(ch, "Body: %0.1f out of 10\n", ((float)get_attract_checkout(victim, ch, 3))/10.0);

    printf_to_char(ch, "Scount %f\n\r", victim->pcdata->attract_count_skin);

    printf_to_char(ch, "Current skin: %f, avg: %f\n\r", skin_face_score(victim), (victim->pcdata->attract_count_skin / UMAX(1, victim->pcdata->attract_count)));

    printf_to_char(ch, "Current fash: %f, avg: %f\n\r", fashion_score(victim), (victim->pcdata->attract_count_fashion / UMAX(1, victim->pcdata->attract_count)));


    printf_to_char(ch, "Current phys: %f, avg: %f\n\r", phys_score(victim), (victim->pcdata->attract_count_phys / UMAX(1, victim->pcdata->attract_count) ));

    float bval = victim->pcdata->attract[ATTRACT_MAKEUP] * 30;
    int mins = (current_time - victim->pcdata->last_shower) / 60;
    int minsq = mins*mins;
    float reduc = cbrt((float)minsq);

    printf_to_char(ch, "Skin bval: %f, mins: %d, minsq: %d, reduc: %f\n\r", bval, mins, minsq, reduc);

    printf_to_char(ch, "FBoost Base: %d, FBoost Boost: %d\n", fashion_boost(victim, FALSE), fashion_boost(victim, TRUE));
    printf_to_char(ch, "SBoost Base: %d, SBoost Boost: %d\n", face_skin_boost(victim, FALSE), face_skin_boost(victim, TRUE));
    printf_to_char(ch, "PBoost Base: %d, PBoost Boost: %d\n", phys_boost(victim, FALSE), phys_boost(victim, TRUE));
    int base = 50;

    if (get_skill(victim, SKILL_ATTRACTIVENESS) > 0)
    base = 65;
    if (get_skill(victim, SKILL_ATTRACTIVENESS) < 0)
    base = 10;
    printf_to_char(ch, "Base: %d\n", base);

    int fash_boost = fashion_boost(victim, is_fashion_boost(victim));
    int skin_boost = face_skin_boost(victim, is_face_skin_boost(victim));
    int physical_boost = phys_boost(victim, is_phys_boost(victim));

    if (fash_boost >= skin_boost && fash_boost >= physical_boost) {
      base += fash_boost;
      base += UMAX(skin_boost / 2, (int)sqrt((float)skin_boost));
      base += UMAX(physical_boost / 2, (int)sqrt((float)physical_boost));
      printf_to_char(ch, "After Fashion Boost: %d\n", base);
    }
    else if (skin_boost >= fash_boost && skin_boost >= physical_boost) {
      base += skin_boost;
      base += UMAX(fash_boost / 2, (int)sqrt((float)fash_boost));
      base += UMAX(physical_boost / 2, (int)sqrt((float)physical_boost));
      printf_to_char(ch, "After Skin Boost: %d\n", base);
    }
    else {
      base += physical_boost;
      base += UMAX(fash_boost / 2, (int)sqrt((float)fash_boost));
      base += UMAX(skin_boost / 2, (int)sqrt((float)physical_boost));
      printf_to_char(ch, "After Phys Boost: %d\n", base);
    }

    printf_to_char(ch, "Viewer mods: %d\n", viewer_mods(ch, victim));
    printf_to_char(ch, "ALF %d\n", attract_lifeforce(victim));
    printf_to_char(ch, "CMOD %d\n", character_mod(victim));
    printf_to_char(ch, "Rare %d\n", attractive_rare_bonus(victim));

    char string[MSL];

    string[0] = '\0';

    strcat(string, from_color(get_focused(victim, victim, TRUE)));
    if (safe_strlen(string) < 200)
    send_to_char("Short desc\n\r", ch);
    if (safe_strlen(string) < 80)
    send_to_char("Shorter desc\n\r", ch);
    if (safe_strlen(string) > 3000)
    send_to_char("Wall of text\n\r", ch);

  }

  _DOFUN(do_checkout) {

    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    victim = get_char_room(ch, NULL, arg1);
    if(victim == NULL)
    victim = ch;

    printf_to_char(ch, "%s%0.1f`x out of 10\n", attract_color(get_attract(victim, ch)), ((float)get_attract(victim, ch))/10.0);
    if (ch == victim && is_fashion_boost(ch))
    printf_to_char(ch, "`W*%sClothes: %0.1f out of 10`x\n", attract_color(get_attract_checkout(victim, ch, 1)), ((float)get_attract_checkout(victim, ch, 1))/10.0);
    else
    printf_to_char(ch, "%sClothes: %0.1f out of 10`x\n", attract_color(get_attract_checkout(victim, ch, 1)), ((float)get_attract_checkout(victim, ch, 1))/10.0);
    if (ch == victim && is_face_skin_boost(ch))
    printf_to_char(ch, "`W*%sSkin/Face/Hair: %0.1f out of 10`x\n", attract_color(get_attract_checkout(victim, ch, 2)), ((float)get_attract_checkout(victim, ch, 2))/10.0);
    else
    printf_to_char(ch, "%sSkin/Face/Hair: %0.1f out of 10`x\n", attract_color(get_attract_checkout(victim, ch, 2)), ((float)get_attract_checkout(victim, ch, 2))/10.0);
    if (ch == victim && is_phys_boost(ch))
    printf_to_char(ch, "`W*%sBody: %0.1f out of 10`x\n", attract_color(get_attract_checkout(victim, ch, 3)), ((float)get_attract_checkout(victim, ch, 3))/10.0);
    else
    printf_to_char(ch, "%sBody: %0.1f out of 10`x\n", attract_color(get_attract_checkout(victim, ch, 3)), ((float)get_attract_checkout(victim, ch, 3))/10.0);

    if (ch == victim) {
      if (victim->pcdata->attract[ATTRACT_PROM] >= 2000) {
        send_to_char("You have a reputation as the town bicycle.\n\r", ch);
      }
      else if (victim->pcdata->attract[ATTRACT_PROM] >= 1000) {
        send_to_char("You have a very poor sexual reputation.\n\r", ch);
      }
      else if (victim->pcdata->attract[ATTRACT_PROM] >= 500) {
        send_to_char("You have a poor sexual reputation.\n\r", ch);
      }
      else if (victim->pcdata->attract[ATTRACT_PROM] >= 150) {
        send_to_char("You have a mild poor sexual reputation.\n\r", ch);
      }
    }
    else {
      act("You checkout $N.", ch, NULL, victim, TO_CHAR);
      act("$n checks you out.", ch, NULL, victim, TO_VICT);
    }
  }

  _DOFUN(do_bloodbath)
  {
    if(!has_nonconsume(ch, ITEM_BLOOD))
    {
      send_to_char("You don't have any blood.\n\r", ch);
      return;
    }
    if(!IS_SET(ch->in_room->room_flags, ROOM_BATHROOM))
    {
      send_to_char("You have to be in a bathroom to do that.\n\r", ch);
      return;
    }
    int level = 0;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (obj->pIndexData->vnum == ITEM_BLOOD) {
        level += obj->level;
      }
    }
    if(level < 250)
    {
      send_to_char("You don't have enough blood to do that.\n\r", ch);
      return;
    }
    level = 0;
    int count = 0;
    while (level < 250 && count < 50) {
      count++;
      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;

        if (IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
          continue;
        }

        if (obj->pIndexData != NULL && obj->pIndexData->vnum == ITEM_BLOOD) {
          level += obj->level;
          obj->level = 0;
          extract_obj(obj);
          break;
        }
      }
    }
    ch->pcdata->attract_count_skin = ch->pcdata->attract_count_fashion*5/10;
    ch->pcdata->attract_count_phys = ch->pcdata->attract_count_fashion*8/10;
    ch->pcdata->attract_count_fashion = ch->pcdata->attract_count_fashion*9/10;
    ch->pcdata->attract_count = ch->pcdata->attract_count*8/10;
    act("You bathe in blood.", ch, NULL, NULL, TO_CHAR);
    act("$n bathes in blood.", ch, NULL, NULL, TO_ROOM);
    ch->pcdata->blood[0] += 50;

  }

  char * attract_color(int val)
  {
    if(val < 40)
    return "`003";
    if(val < 60)
    return "`w";
    if(val < 70)
    return "`028";
    if(val < 74)
    return "`029";
    if(val < 77)
    return "`030";
    if(val < 80)
    return "`025";
    if(val < 83)
    return "`027";
    if(val < 86)
    return "`033";
    if(val < 89)
    return "`039";
    if(val < 92)
    return "`045";
    if(val < 95)
    return "`051";
    if(val < 98)
    return "`014";
    return "`015";
  }



#if defined(__cplusplus)
}
#endif

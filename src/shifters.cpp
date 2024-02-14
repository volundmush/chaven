/*
* Werewolves, shapeshifters, and shifters
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


  char *get_animal_description(CHAR_DATA *ch, int number) {
    if (number >= 0 && number <= 5)
    return ch->pcdata->animal_descs[number];

    if (number == ANIMAL_WOLF)
    return ch->pcdata->wolfdesc;

    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_description(ch, ANIMAL_WOLF);
      else
      return get_animal_description(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_description(ch, ANIMAL_WOLF);
      else
      return get_animal_description(ch, 0);
    }
    return "";
  }

  char *get_animal_species(CHAR_DATA *ch, int number) {
    if (number >= 0 && number <= 5) {
      if (ch->pcdata->animal_genus[number] == GENUS_SWARM) {
        char buf[MSL];
        sprintf(buf, "swarm of %d %s", swarm_count(ch, number), ch->pcdata->animal_species[number]);
        return str_dup(buf);
      }
      return ch->pcdata->animal_species[number];
    }
    if (number == ANIMAL_WOLF) {
      if (get_skill(ch, SKILL_HYBRIDSHIFTING) > 0)
      return "hybrid wolf";
      return "wolf";
    }

    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_species(ch, ANIMAL_WOLF);
      else
      return get_animal_species(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_species(ch, ANIMAL_WOLF);
      else
      return get_animal_species(ch, 0);
    }
    return "";
  }

  char *get_animal_name(CHAR_DATA *ch, int number) {
    if (number >= 0 && number <= 5)
    return ch->pcdata->animal_names[number];

    if (number == ANIMAL_WOLF)
    return "wolf";

    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_name(ch, ANIMAL_WOLF);
      else
      return get_animal_name(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_name(ch, ANIMAL_WOLF);
      else
      return get_animal_name(ch, 0);
    }
    return "";
  }

  char *get_animal_intro(CHAR_DATA *ch, int number) {
    if (number >= 0 && number <= 5)
    return ch->pcdata->animal_intros[number];

    if (number == ANIMAL_WOLF) {
      return ch->pcdata->wolfintro;
    }

    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_intro(ch, ANIMAL_WOLF);
      else
      return get_animal_intro(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_intro(ch, ANIMAL_WOLF);
      else
      return get_animal_intro(ch, 0);
    }
    return "";
  }


  char *get_animal_changeto(CHAR_DATA *ch, int number) {

    if (number == SHAPE_WOLF) {
      return ch->pcdata->wolf_change_to;
    }

    if (number >= 2 && number <= 7)
    return ch->pcdata->animal_change_to[number-2];


    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_changeto(ch, SHAPE_WOLF);
      else
      return get_animal_changeto(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_changeto(ch, SHAPE_WOLF);
      else
      return get_animal_changeto(ch, 0);
    }
    return "";
  }


  char *get_animal_changefrom(CHAR_DATA *ch, int number) {

    if (number == SHAPE_WOLF) {
      return ch->pcdata->wolf_change_from;
    }

    if (number >= 2 && number <= 7)
    return ch->pcdata->animal_change_from[number-2];


    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_changefrom(ch, ANIMAL_WOLF);
      else
      return get_animal_changefrom(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_changefrom(ch, ANIMAL_WOLF);
      else
      return get_animal_changefrom(ch, 0);
    }
    return "";
  }

  int wolf_stat(CHAR_DATA *ch, int stat) {
    if (get_skill(ch, SKILL_MONSTERSHIFT) > 1) {
      if (stat == ANIMAL_POWER)
      return 4;
      if (stat == ANIMAL_TOUGHNESS)
      return 3;
    }

    if (stat == ANIMAL_SPEED)
    return 2;
    if (stat == ANIMAL_AGILITY)
    return 1;
    if (stat == ANIMAL_POWER) {
      if (get_skill(ch, SKILL_PRIMALFOCUS) >= 2)
      return 2;
      return 1;
    }
    if (stat == ANIMAL_TOUGHNESS) {
      if (get_skill(ch, SKILL_PRIMALFOCUS) >= 1)
      return 1;
    }
    if (stat == ANIMAL_SMELL)
    return 1;
    if (stat == ANIMAL_HEARING)
    return 1;
    if (stat == ANIMAL_SIGHT)
    return 1;
    if (stat == ANIMAL_NOCTURNAL)
    return 1;

    return 0;
  }

  int animal_stat(CHAR_DATA *ch, int number, int stat) {

    int astat = 0;
    if (number >= 0 && number <= 5)
    astat = ch->pcdata->animal_stats[number][stat];
    else if (number == ANIMAL_WOLF)
    astat = wolf_stat(ch, stat);
    else if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      astat = animal_stat(ch, ANIMAL_WOLF, stat);
      else
      astat = animal_stat(ch, (ch->shape - SHAPE_ANIMALONE), stat);
    }
    else if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      astat = animal_stat(ch, ANIMAL_WOLF, stat);
      else
      astat = animal_stat(ch, 0, stat);
    }
    if (get_animal_genus(ch, number) != GENUS_MYTHOLOGICAL)
    stat = UMIN(astat, 6);

    return astat;
  }
  int get_animal_genus(CHAR_DATA *ch, int number) {
    if (number >= 0 && number <= 5)
    return ch->pcdata->animal_genus[number];

    if (number == ANIMAL_WOLF)
    return GENUS_MAMMAL;

    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_genus(ch, ANIMAL_WOLF);
      else
      return get_animal_genus(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_genus(ch, ANIMAL_WOLF);
      else
      return get_animal_genus(ch, 0);
    }
    return 0;
  }
  int wolf_weight(CHAR_DATA *ch) {
    int weight = 230;
    if (get_skill(ch, SKILL_HYBRIDSHIFTING) > 0)
    weight = 155;
    weight += (ch->pcdata->height_feet - 5) * 12 * 5;
    weight += ch->pcdata->height_inches * 5;
    if (ch->pcdata->penis > 0)
    weight += 20;

    if (ch->race == RACE_ANCIENTWEREWOLF && get_skill(ch, SKILL_HYBRIDSHIFTING) == 0)
    weight *= 2;

    if (get_skill(ch, SKILL_MONSTERSHIFT) > 1 && get_skill(ch, SKILL_HYBRIDSHIFTING) == 0)
    weight += 1000;

    return weight * 10;
  }

  int get_animal_weight(CHAR_DATA *ch, int number) {
    if (number >= 0 && number <= 5)
    return ch->pcdata->animal_weights[number];

    if (number == ANIMAL_WOLF)
    return wolf_weight(ch);

    if (number == ANIMAL_ACTIVE) {
      if (ch->shape == SHAPE_WOLF)
      return get_animal_weight(ch, ANIMAL_WOLF);
      else
      return get_animal_weight(ch, (ch->shape - SHAPE_ANIMALONE));
    }

    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      return get_animal_weight(ch, ANIMAL_WOLF);
      else
      return get_animal_weight(ch, 0);
    }
    return 0;
  }

  int animalpoint(CHAR_DATA *ch, char *name) {
    if (safe_strlen(name) < 2)
    return -1;

    for (int i = 0; i < 6; i++) {
      if (!str_cmp(ch->pcdata->animal_names[i], name))
      return i;
    }
    return -1;
  }

  int kgfrompounds(int pounds) {
    int val = pounds * 454 / 1000;
    return val;
  }

  int animal_size(int weight) // weight pounds * 10
  {
    if (weight <= 2)
    return ANIMAL_TINY;
    if (weight <= 110)
    return ANIMAL_SMALL;
    if (weight <= 880)
    return ANIMAL_SMALLMED;
    if (weight <= 1540)
    return ANIMAL_MEDIUM;
    if (weight <= 2200)
    return ANIMAL_MANSIZED;
    if (weight <= 4410)
    return ANIMAL_LARGE;
    if (weight <= 10000)
    return ANIMAL_VLARGE;
    return ANIMAL_MONSTEROUS;
  }

  int swarm_count(CHAR_DATA *ch, int number) {
    int size = animal_size(get_animal_weight(ch, number));
    int val = 1;
    if (size >= ANIMAL_MANSIZED)
    val = 1;
    if (size == ANIMAL_MEDIUM)
    val = 2;
    if (size == ANIMAL_SMALLMED)
    val = 4;
    if (size == ANIMAL_SMALL)
    val = 8;
    if (size == ANIMAL_TINY)
    val = 16;

    val = val * UMAX(1, ch->hit) / UMAX(1, max_hp(ch));
    val = UMAX(val, 1);
    return val;
  }

  int animal_pool_total(CHAR_DATA *ch) {
    int val = 0;

    val += get_skill(ch, SKILL_SHAPESHIFTING) * 5;

    if (get_skill(ch, SKILL_HYPERSHIFTING) > 0)
    val = UMAX(val + 5, 15);
    if (get_skill(ch, SKILL_HYBRIDSHIFTING) > 0)
    val = UMAX(val + 5, 15);
    if (get_skill(ch, SKILL_SWARMSHIFTING) > 0)
    val = UMAX(val + 5, 15);
    if (get_skill(ch, SKILL_MONSTERSHIFT) > 0)
    val = UMAX(val + 5, 15);

    return val;
  }

  int animal_count(CHAR_DATA *ch) {
    int count = 0;
    for (int i = 0; i < 6; i++) {
      if (safe_strlen(ch->pcdata->animal_names[i]) > 1)
      count++;
    }
    return count;
  }
  int animal_pool_per_animal(CHAR_DATA *ch, int number) {
    int pool = animal_pool_total(ch);

    number--;

    if (number > 0) {
      for (int i = 0; i < number; i++)
      pool = UMAX(pool * 8 / 10, pool - 3);
    }
    return pool;
  }

  int animal_pool(CHAR_DATA *ch, bool primary) {
    int amount = animal_pool_per_animal(ch, animal_count(ch));

    if (primary)
    amount += 5;

    return amount;
  }

  int animal_trainpool(CHAR_DATA *ch, int number) {
    if (is_werewolf(ch))
    return animal_pool(ch, FALSE) - 2;

    if (is_werewolf(ch) || number != 0)
    return animal_pool(ch, FALSE);

    return animal_pool(ch, TRUE);
  }

  int get_animal_cost(CHAR_DATA *ch, int number) {
    //    int genus = get_animal_genus(ch, number);
    int size = animal_size(get_animal_weight(ch, number));

    int cost = 0;

    cost += animal_stat(ch, number, ANIMAL_POWER) * 5;
    cost += animal_stat(ch, number, ANIMAL_TOUGHNESS) * 5;
    cost += animal_stat(ch, number, ANIMAL_SENSES);
    cost += animal_stat(ch, number, ANIMAL_NOCTURNAL) * 2;
    cost += animal_stat(ch, number, ANIMAL_AMPHIBIOUS) * 4;
    cost += animal_stat(ch, number, ANIMAL_ARMORED) * 5;
    cost += animal_stat(ch, number, ANIMAL_SMELL);
    cost += animal_stat(ch, number, ANIMAL_HEARING);
    cost += animal_stat(ch, number, ANIMAL_SIGHT);
    cost += animal_stat(ch, number, ANIMAL_POISONOUS) * 5;
    cost += animal_stat(ch, number, ANIMAL_FLIGHT) * 5;

    switch (size) {
    case ANIMAL_VLARGE:
    case ANIMAL_MONSTEROUS:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 4;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 5;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 5;
      break;
    case ANIMAL_LARGE:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 3;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 4;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 4;
      break;
    case ANIMAL_MANSIZED:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 2;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 2;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 3;
      break;
    case ANIMAL_MEDIUM:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 1;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 1;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 2;
      break;
    case ANIMAL_SMALLMED:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 2;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 1;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 2;
      break;
    case ANIMAL_SMALL:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 3;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 2;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 1;
      break;
    case ANIMAL_TINY:
      cost += animal_stat(ch, number, ANIMAL_SPEED) * 4;
      cost += animal_stat(ch, number, ANIMAL_AGILITY) * 2;
      cost += animal_stat(ch, number, ANIMAL_CAMOUFLAGE) * 1;
      break;
    }
    return cost;
  }

  int get_animal_claw(CHAR_DATA *ch, int number) {
    int base = (UMAX(UMAX(UMAX(UMAX(UMAX(UMAX(ch->disciplines[DIS_BOWS], ch->disciplines[DIS_PISTOLS]), ch->disciplines[DIS_RIFLES]), ch->disciplines[DIS_CARBINES]), ch->disciplines[DIS_THROWN]), ch->disciplines[DIS_SHOTGUNS]), ch->disciplines[DIS_SPEARGUN])) /
    2;

    base =
    base + UMAX(ch->disciplines[DIS_STRIKING], ch->disciplines[DIS_GRAPPLE]) +
    UMAX(
    UMAX(UMAX(ch->disciplines[DIS_LONGBLADE], ch->disciplines[DIS_BLUNT]), ch->disciplines[DIS_KNIFE]), ch->disciplines[DIS_SPEAR]);

    int size = animal_size(get_animal_weight(ch, number));

    if (get_animal_genus(ch, number) == GENUS_REPTILE) {
      if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && temperature(ch->in_room) < 50) {
        base = base * 9 / 10;
      }
    }

    switch (animal_stat(ch, number, ANIMAL_POWER)) {
    case 1:
      base += 20;
      break;
    case 2:
      base += 20;
      base += 18;
      break;
    case 3:
      base += 20;
      base += 18;
      base += 16;
      break;
    case 4:
      base += 20;
      base += 18;
      base += 16;
      base += 14;
      break;
    case 5:
      base += 20;
      base += 18;
      base += 16;
      base += 14;
      base += 12;
      break;
    }
    switch (animal_stat(ch, number, ANIMAL_TOUGHNESS)) {
    case 1:
      base -= 10;
      break;
    case 2:
      base -= 10;
      base -= 9;
      break;
    case 3:
      base -= 10;
      base -= 9;
      base -= 8;
      break;
    case 4:
      base -= 10;
      base -= 9;
      base -= 8;
      base -= 7;
      break;
    case 5:
      base -= 10;
      base -= 9;
      base -= 8;
      base -= 7;
      base -= 6;
      break;
    }

    if (size == ANIMAL_MONSTEROUS)
    base = base * 150 / 100;
    if (size == ANIMAL_VLARGE)
    base = base * 120 / 100;
    else if (size == ANIMAL_LARGE)
    base = base * 110 / 100;
    else if (size == ANIMAL_MANSIZED || (size == ANIMAL_SMALL && animal_stat(ch, number, ANIMAL_POISONOUS) > 0))
    base = base * 1;
    else if (size == ANIMAL_MEDIUM)
    base = base * 90 / 100;
    else if (size == ANIMAL_SMALLMED)
    base = base * 75 / 100;
    else if (size == ANIMAL_SMALL)
    base = base * 40 / 100;
    else if (size == ANIMAL_TINY && animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
    base = base * 50 / 100;
    else if (size == ANIMAL_TINY)
    base = base * 10 / 100;

    return base;
  }

  int get_animal_tough(CHAR_DATA *ch, int number) {
    int base = ch->disciplines[DIS_TOUGHNESS] + ch->disciplines[DIS_BARMOR] +
    ch->disciplines[DIS_MARMOR] + ch->disciplines[DIS_BSHIELD] +
    ch->disciplines[DIS_MSHIELD] + ch->disciplines[DIS_FORCES] +
    ch->disciplines[DIS_BONES] + ch->disciplines[DIS_UNDEAD] +
    ch->disciplines[DIS_PUSH] + ch->disciplines[DIS_FATE];

    int size = animal_size(get_animal_weight(ch, number));

    switch (animal_stat(ch, number, ANIMAL_TOUGHNESS)) {
    case 1:
      base += 20;
      break;
    case 2:
      base += 20;
      base += 18;
      break;
    case 3:
      base += 20;
      base += 18;
      base += 16;
      break;
    case 4:
      base += 20;
      base += 18;
      base += 16;
      base += 14;
      break;
    case 5:
      base += 20;
      base += 18;
      base += 16;
      base += 14;
      base += 12;
      break;
    }
    switch (animal_stat(ch, number, ANIMAL_POWER)) {
    case 1:
      base -= 10;
      break;
    case 2:
      base -= 10;
      base -= 9;
      break;
    case 3:
      base -= 10;
      base -= 9;
      base -= 8;
      break;
    case 4:
      base -= 10;
      base -= 9;
      base -= 8;
      base -= 7;
      break;
    case 5:
      base -= 10;
      base -= 9;
      base -= 8;
      base -= 7;
      base -= 6;
      break;
    }

    if (size == ANIMAL_MONSTEROUS)
    base = base * 150 / 100;
    else if (size == ANIMAL_VLARGE)
    base = base * 120 / 100;
    else if (size == ANIMAL_LARGE)
    base = base * 110 / 100;
    else if (size == ANIMAL_MANSIZED)
    base = base * 1;
    else if (size == ANIMAL_MEDIUM)
    base = base * 80 / 100;
    else if (size == ANIMAL_SMALLMED)
    base = base * 50 / 100;
    else if (size == ANIMAL_SMALL)
    base = base * 25 / 100;
    else if (size == ANIMAL_TINY)
    base = base * 1 / 100;

    return base;
  }

  int animal_skill(CHAR_DATA *ch, int number, int skill) {
    int val = 0;

    if (skill == SKILL_PERCEPTION) {
      val += animal_stat(ch, number, ANIMAL_SENSES);
      val += animal_stat(ch, number, ANIMAL_HEARING);
      val += animal_stat(ch, number, ANIMAL_SMELL);
      val += animal_stat(ch, number, ANIMAL_SIGHT);
      if (nighttime(ch->in_room) && animal_stat(ch, number, ANIMAL_NOCTURNAL) > 0)
      val++;
    }
    if (skill == SKILL_STEALTH) {
      if (is_natural(ch->in_room) || light_level(ch->in_room) < 50) {
        val += animal_stat(ch, number, ANIMAL_CAMOUFLAGE);
      }
    }
    if (skill == SKILL_STRENGTH) {
      val += animal_stat(ch, number, ANIMAL_POWER) * 2;
    }
    if (skill == SKILL_STAMINA) {
      val += animal_stat(ch, number, ANIMAL_TOUGHNESS) * 2;
    }
    if (skill == SKILL_RUNNING) {
      val += animal_stat(ch, number, ANIMAL_SPEED);
      if (get_animal_genus(ch, number) == GENUS_REPTILE) {
        if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && temperature(ch->in_room) < 50) {
          val--;
        }
      }
    }
    if (skill == SKILL_ACROBATICS) {
      val += animal_stat(ch, number, ANIMAL_AGILITY) / 2;
    }
    if (skill == SKILL_DEXTERITY) {
      val += animal_stat(ch, number, ANIMAL_AGILITY);
    }
    if (skill == SKILL_SUPERJUMP) {
      if (animal_stat(ch, number, ANIMAL_AGILITY) > 3 && animal_size(get_animal_weight(ch, number)) != ANIMAL_TINY && animal_size(get_animal_weight(ch, number)) != ANIMAL_SMALL)
      val += 2;
    }
    if (skill == SKILL_FASTREFLEXES) {
      val += animal_stat(ch, number, ANIMAL_AGILITY) / 2;
    }
    if (skill == SKILL_EVADING) {
      val += animal_stat(ch, number, ANIMAL_AGILITY);
      if (animal_size(get_animal_weight(ch, number)) == ANIMAL_TINY)
      val += 2;
      else if (animal_size(get_animal_weight(ch, number)) == ANIMAL_SMALL)
      val += 1;
    }
    if (skill == SKILL_ACUTEHEAR) {
      val += animal_stat(ch, number, ANIMAL_HEARING);
    }
    if (skill == SKILL_ACUTESMELL) {
      val += animal_stat(ch, number, ANIMAL_SMELL);
    }
    if (skill == SKILL_ACUTESIGHT) {
      val += animal_stat(ch, number, ANIMAL_SIGHT);
    }
    if (skill == SKILL_NIGHTVISION) {
      val += animal_stat(ch, number, ANIMAL_NOCTURNAL) * 3;
    }

    if (val > 0 && !viable_form(ch, ANIMAL_PRIMARY)) {
      //	printf_to_char(ch, "Nonviable %d %d %d\n\r", val, skill, number);
      val = 0;
    }

    return val;
  }

  bool legal_size(CHAR_DATA *ch, int size) {
    if (size == ANIMAL_MANSIZED) {
      if (get_skill(ch, SKILL_FAEKNIGHTFOCUS) > 0 || get_skill(ch, SKILL_PRIMALFOCUS) > 0 || get_skill(ch, SKILL_HYBRIDSHIFTING) > 0)
      return TRUE;
      return FALSE;
    }
    if (size == ANIMAL_MEDIUM || size == ANIMAL_LARGE) {
      if (get_skill(ch, SKILL_FAEKNIGHTFOCUS) > 1 || get_skill(ch, SKILL_PRIMALFOCUS) > 1)
      return TRUE;
      return FALSE;
    }
    if (size == ANIMAL_VLARGE) {
      if (get_skill(ch, SKILL_FAEKNIGHTFOCUS) > 3 || get_skill(ch, SKILL_PRIMALFOCUS) > 3)
      return TRUE;
      return FALSE;
    }
    if (size == ANIMAL_MONSTEROUS) {
      if (get_skill(ch, SKILL_MONSTERSHIFT) > 1)
      return TRUE;
      return FALSE;
    }
    if (size == ANIMAL_SMALLMED) {
      if (get_skill(ch, SKILL_FAEKNIGHTFOCUS) > 2 || get_skill(ch, SKILL_PRIMALFOCUS) > 2)
      return TRUE;
      if (get_skill(ch, SKILL_NATURALMAGICFOCUS) > 2 || get_skill(ch, SKILL_ILLUSORYMAGICFOCUS) > 3 || get_skill(ch, SKILL_DARKMAGICFOCUS) > 3)
      return TRUE;
      return FALSE;
    }
    if (size == ANIMAL_SMALL) {
      if (get_skill(ch, SKILL_FAEKNIGHTFOCUS) > 2 || get_skill(ch, SKILL_PRIMALFOCUS) > 2)
      return TRUE;
      if (get_skill(ch, SKILL_NATURALMAGICFOCUS) > 1 || get_skill(ch, SKILL_ILLUSORYMAGICFOCUS) > 2 || get_skill(ch, SKILL_DARKMAGICFOCUS) > 3)
      return TRUE;
      return FALSE;
    }
    if (size == ANIMAL_TINY) {
      if (get_skill(ch, SKILL_FAEKNIGHTFOCUS) > 3 || get_skill(ch, SKILL_PRIMALFOCUS) > 3)
      return TRUE;
      if (get_skill(ch, SKILL_NATURALMAGICFOCUS) > 3 || get_skill(ch, SKILL_ILLUSORYMAGICFOCUS) > 3 || get_skill(ch, SKILL_DARKMAGICFOCUS) > 2)
      return TRUE;
      return FALSE;
    }
    return FALSE;
  }

  bool viable_form(CHAR_DATA *ch, int number) {
    if (number == ANIMAL_PRIMARY) {
      if (is_werewolf(ch))
      number = ANIMAL_WOLF;
      else
      number = 0;
    }

    if (get_animal_genus(ch, number) == 0)
    return FALSE;
    if (get_animal_weight(ch, number) == 0)
    return FALSE;

    if (safe_strlen(get_animal_name(ch, number)) < 3)
    return FALSE;
    if (safe_strlen(get_animal_intro(ch, number)) < 3)
    return FALSE;
    if (safe_strlen(get_animal_species(ch, number)) < 3)
    return FALSE;

    if (number == ANIMAL_WOLF)
    return TRUE;

    if (is_werewolf(ch) || number != 0) {
      if (get_animal_cost(ch, number) > animal_pool(ch, FALSE))
      return FALSE;
    }
    else {
      if (get_animal_cost(ch, number) > animal_pool(ch, TRUE))
      return FALSE;
    }

    for (int x = 0; x < 5; x++) {
      for (int y = 0; y < 21; y++) {
        if (ch->pcdata->animal_stats[x][y] < 0)
        ch->pcdata->animal_stats[x][y] = 0;
      }
    }

    if (!legal_size(ch, (animal_size(get_animal_weight(ch, number)))))
    return FALSE;

    if (get_animal_genus(ch, number) == GENUS_INSECT) {
      if (animal_size(get_animal_weight(ch, number)) != ANIMAL_TINY)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_AMPHIBIOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_RODENT) {
      if (animal_size(get_animal_weight(ch, number)) > ANIMAL_SMALL)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_BIRD) {
      if (animal_size(get_animal_weight(ch, number)) > ANIMAL_SMALLMED)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_AMPHIBIOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_AQUATIC) {
      if (animal_stat(ch, number, ANIMAL_AMPHIBIOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_FLIGHT) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_REPTILE) {
      if (animal_size(get_animal_weight(ch, number)) > ANIMAL_SMALL && animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_FLIGHT) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_MAMMAL) {
      if (animal_stat(ch, number, ANIMAL_AMPHIBIOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_FLIGHT) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_HYBRID) {
      if (animal_size(get_animal_weight(ch, number)) != ANIMAL_MANSIZED)
      return FALSE;
      if (get_skill(ch, SKILL_HYBRIDSHIFTING) < 1)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0 && get_skill(ch, SKILL_HYPERSHIFTING) < 1)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_FLIGHT) > 0)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_SWARM) {
      if (get_skill(ch, SKILL_SWARMSHIFTING) < 1)
      return FALSE;
      if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
      return FALSE;
    }
    if (get_animal_genus(ch, number) == GENUS_MYTHOLOGICAL) {
      if (get_skill(ch, SKILL_HYPERSHIFTING) < 1)
      return FALSE;
    }
    return TRUE;
  }

  char *const genus_names[] = {"", "insects", "rodents", "birds", "aquatic life", "reptiles", "mammals", "mythological creatures", "hybrid", "swarm"};

  char *genus_name(int genus) { return genus_names[genus]; }

  void view_animal(CHAR_DATA *ch, int number) {

    if (viable_form(ch, number))
    printf_to_char(ch, "`gName`x: `W%s\t\t\t`xViable: `GYes`x\n\r", get_animal_name(ch, number));
    else
    printf_to_char(ch, "`gName`x: `W%s\t\t\t`xViable: `RNo`x\n\r", get_animal_name(ch, number));

    printf_to_char(ch, "A %.1f pound", (float)(get_animal_weight(ch, number) / 10));
    printf_to_char(ch, "(%.1f KG)", (float)(kgfrompounds(get_animal_weight(ch, number)) / 10));
    printf_to_char(ch, " %s from the genetic family of", get_animal_species(ch, number));
    printf_to_char(ch, " %s.\n\r", genus_name(get_animal_genus(ch, number)));

    printf_to_char(ch, "\nThey are %s\n\r", lowercase_clause(get_animal_intro(ch, number)));
    printf_to_char(ch, "Change To: %s\nChange From: %s\n\r", get_animal_changeto(ch, number), get_animal_changefrom(ch, number));
    if (!is_werewolf(ch) || number > 0) {
      if (is_werewolf(ch) || number != 0)
      printf_to_char(ch, "\nStats: %d(%d)\n\r", get_animal_cost(ch, number), animal_pool(ch, FALSE));
      else
      printf_to_char(ch, "\nStats: %d(%d)\n\r", get_animal_cost(ch, number), animal_pool(ch, TRUE));
    }
    printf_to_char(ch, "\nPower: %d \tToughness: %d \t Speed: %d\n\r", animal_stat(ch, number, ANIMAL_POWER), animal_stat(ch, number, ANIMAL_TOUGHNESS), animal_stat(ch, number, ANIMAL_SPEED));
    printf_to_char(ch, "Agility: %d \tCamouflage: %d \t Senses: %d\n\r", animal_stat(ch, number, ANIMAL_AGILITY), animal_stat(ch, number, ANIMAL_CAMOUFLAGE), animal_stat(ch, number, ANIMAL_SENSES));
    if (animal_stat(ch, number, ANIMAL_FLIGHT) > 0)
    send_to_char("Flying ", ch);
    if (animal_stat(ch, number, ANIMAL_NOCTURNAL) > 0)
    send_to_char("Nocturnal ", ch);
    if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
    send_to_char("Venomous ", ch);
    if (animal_stat(ch, number, ANIMAL_AMPHIBIOUS) > 0)
    send_to_char("Amphibious ", ch);
    if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
    send_to_char("Armored ", ch);
    if (animal_stat(ch, number, ANIMAL_SMELL) > 0)
    send_to_char("Acute smell ", ch);
    if (animal_stat(ch, number, ANIMAL_HEARING) > 0)
    send_to_char("Acute hearing ", ch);
    if (animal_stat(ch, number, ANIMAL_SIGHT) > 0)
    send_to_char("Acute sight ", ch);

    printf_to_char(ch, "\n\n%s\n\n\r", get_animal_description(ch, number));
  }

  void show_animal_to_char(CHAR_DATA *ch, CHAR_DATA *looker, int type, bool photo) {
    char buf[MSL];
    char otext[MSL];
    strcpy(otext, "");

    sprintf(buf, "%s %s\n\n\r", get_animal_intro(ch, ANIMAL_ACTIVE), relfacing[get_reldirection(ch->facing, looker->facing)]);
    strcat(otext, buf);
    if (get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_INSECT && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_RODENT && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_REPTILE) {
      sprintf(
      buf, "%s looks to be an approximately %.1f pound(%.1f KG) %s with %s.\n\r", (ch->sex == SEX_MALE) ? "He" : "She", (float)(get_animal_weight(ch, ANIMAL_ACTIVE) / 10), (float)(kgfrompounds(get_animal_weight(ch, ANIMAL_ACTIVE)) / 10), get_animal_species(ch, ANIMAL_ACTIVE), get_natural_eyes(ch));
      strcat(otext, buf);
    }
    else {
      sprintf(buf, "%s looks to be an approximately %.1f pound(%.1f KG) %s.\n\r", (ch->sex == SEX_MALE) ? "He" : "She", (float)(get_animal_weight(ch, ANIMAL_ACTIVE) / 10), (float)(kgfrompounds(get_animal_weight(ch, ANIMAL_ACTIVE)) / 10), get_animal_species(ch, ANIMAL_ACTIVE));
      strcat(otext, buf);
    }
    if (get_skill(looker, SKILL_SENSEWEAKNESS) > 0) {
      if (IS_AFFECTED(ch, AFF_WEAKEN) || IS_AFFECTED(ch, AFF_PENALTY)) {
        strcat(otext, "You sense a mystical weakness.\n\r");
      }
      if (ch->wounds == 1) {
        strcat(otext, "You sense a faint weakness caused by injury.\n\r");
      }
      if (ch->wounds >= 2) {
        strcat(otext, "You sense a potent weakness caused by injury.\n\r");
      }
    }

    if (type == LOOK_LOOK) {
      sprintf(buf, "\n%s\n\r", get_animal_description(ch, ANIMAL_ACTIVE));
      strcat(otext, buf);
    }

    if (get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_HYBRID || (ch->shape == SHAPE_WOLF && get_skill(ch, SKILL_HYBRIDSHIFTING) > 0)) {
      strcat(otext, equip_string(looker, ch));
    }

    if (photo == FALSE)
    page_to_char(wrap_string(otext, get_wordwrap(looker)), looker);
    else
    to_photo_message(looker, otext);
  }

  void view_animal_to(CHAR_DATA *ch, int number, CHAR_DATA *to) {
    if (viable_form(ch, number))
    printf_to_char(to, "`gName`x: `W%s\t\t\t`xViable: `GYes`x\n\r", get_animal_name(ch, number));
    else
    printf_to_char(to, "`gName`x: `W%s\t\t\t`xViable: `RNo`x\n\r", get_animal_name(ch, number));
    printf_to_char(to, "A %.1f pound", (float)(get_animal_weight(ch, number) / 10));
    printf_to_char(to, "(%.1f KG)", (float)(kgfrompounds(get_animal_weight(ch, number)) / 10));
    printf_to_char(to, " %s from the genetic family of", get_animal_species(ch, number));
    printf_to_char(to, " %s.\n\r", genus_name(get_animal_genus(ch, number)));

    printf_to_char(to, "\nThey are %s\n\r", lowercase_clause(get_animal_intro(ch, number)));
    if (!is_werewolf(ch) || number > 0) {
      if (is_werewolf(ch) || number != 0)
      printf_to_char(to, "\nStats: %d(%d)\n\r", get_animal_cost(ch, number), animal_pool(ch, FALSE));
      else
      printf_to_char(to, "\nStats: %d(%d)\n\r", get_animal_cost(ch, number), animal_pool(ch, TRUE));
    }
    printf_to_char(to, "\nPower: %d \tToughness: %d \t Speed: %d\n\r", animal_stat(ch, number, ANIMAL_POWER), animal_stat(ch, number, ANIMAL_TOUGHNESS), animal_stat(ch, number, ANIMAL_SPEED));
    printf_to_char(to, "Agility: %d \tCamouflage: %d \t Senses: %d\n\r", animal_stat(ch, number, ANIMAL_AGILITY), animal_stat(ch, number, ANIMAL_CAMOUFLAGE), animal_stat(ch, number, ANIMAL_SENSES));
    if (animal_stat(ch, number, ANIMAL_FLIGHT) > 0)
    send_to_char("Flying ", to);
    if (animal_stat(ch, number, ANIMAL_NOCTURNAL) > 0)
    send_to_char("Nocturnal ", to);
    if (animal_stat(ch, number, ANIMAL_POISONOUS) > 0)
    send_to_char("Venomous ", to);
    if (animal_stat(ch, number, ANIMAL_AMPHIBIOUS) > 0)
    send_to_char("Amphibious ", to);
    if (animal_stat(ch, number, ANIMAL_ARMORED) > 0)
    send_to_char("Armored ", to);
    if (animal_stat(ch, number, ANIMAL_SMELL) > 0)
    send_to_char("Acute smell ", to);
    if (animal_stat(ch, number, ANIMAL_HEARING) > 0)
    send_to_char("Acute hearing ", to);
    if (animal_stat(ch, number, ANIMAL_SIGHT) > 0)
    send_to_char("Acute sight ", to);

    printf_to_char(to, "\n\n%s\n\n\r", get_animal_description(ch, number));
  }

  int item_count(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int toremove;
    int count = 0;

    for (toremove = 0; toremove < MAX_WEAR; toremove++) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj->wear_loc == toremove && (can_see_obj(ch, obj)) && (obj->item_type == ITEM_CLOTHING)) {
          count++;
        }
      }
    }

    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL) {
      count++;
    }
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL) {
      count++;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        count++;
      }
    }
    return count;
  }

  void remove_clothing(CHAR_DATA *ch) {
    int toremove;
    OBJ_DATA *obj;
    OBJ_DATA *container;
    OBJ_DATA *obj_next;

    if (item_count(ch) < 1)
    return;
    if (higher_power(ch))
    return;

    if (IS_FLAG(ch->act, PLR_SHIFTSTASH)) {
      for (toremove = 0; toremove < MAX_WEAR; toremove++) {
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (obj->wear_loc == toremove && !IS_SET(obj->extra_flags, ITEM_RELIC) && (obj->item_type == ITEM_CLOTHING)) {
            if (obj->wear_loc != WEAR_NONE)
            unequip_char(ch, obj);

            SET_BIT(obj->extra_flags, ITEM_WARDROBE);
            obj->stash_room = 52;
          }
        }
      }
      if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL) {
        if (obj->wear_loc != WEAR_NONE && !IS_SET(obj->extra_flags, ITEM_RELIC))
        unequip_char(ch, obj);
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        obj->stash_room = 52;
      }
      if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL) {
        if (obj->wear_loc != WEAR_NONE && !IS_SET(obj->extra_flags, ITEM_RELIC))
        unequip_char(ch, obj);
        SET_BIT(obj->extra_flags, ITEM_WARDROBE);
        obj->stash_room = 52;
      }

      for (obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (!IS_SET(obj->extra_flags, ITEM_WARDROBE) && !IS_SET(obj->extra_flags, ITEM_RELIC)) {
          SET_BIT(obj->extra_flags, ITEM_WARDROBE);
          obj->stash_room = 52;
        }
      }

      return;
    }

    container = create_object(get_obj_index(35), 0);
    char buf[MSL];

    sprintf(buf, "A pile of %s's belongings", ch->pcdata->intro_desc);
    free_string(container->short_descr);
    container->short_descr = str_dup(buf);

    sprintf(buf, "A pile of %s's belongings", ch->pcdata->intro_desc);
    free_string(container->description);
    container->description = str_dup(buf);

    sprintf(buf, "pile belongings %s %s", ch->pcdata->intro_desc, ch->name);
    free_string(container->name);
    container->name = str_dup(buf);

    obj_to_room(container, ch->in_room);

    for (toremove = 0; toremove < MAX_WEAR; toremove++) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj->wear_loc == toremove && !IS_SET(obj->extra_flags, ITEM_RELIC) && (obj->item_type == ITEM_CLOTHING)) {
          obj_from_char_silent(obj);
          obj_to_obj(obj, container);
        }
      }
    }

    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && !IS_SET(obj->extra_flags, ITEM_RELIC)) {
      obj_from_char_silent(obj);
      obj_to_obj(obj, container);
    }
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && !IS_SET(obj->extra_flags, ITEM_RELIC)) {
      obj_from_char_silent(obj);
      obj_to_obj(obj, container);
    }

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE) && !IS_SET(obj->extra_flags, ITEM_RELIC)) {
        obj_from_char_silent(obj);
        obj_to_obj(obj, container);
      }
    }

    save_ground_objects();
  }

  void pants_off(CHAR_DATA *ch) {
    int toremove;
    OBJ_DATA *obj;

    if (IS_FLAG(ch->act, PLR_SHIFTSTASH)) {
      for (toremove = 0; toremove < MAX_WEAR; toremove++) {
        for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
          if (obj->wear_loc == toremove && (obj->item_type == ITEM_CLOTHING) && !is_name("skirt", obj->short_descr) && !is_name("miniskirt", obj->short_descr) && !is_name("mini-skirt", obj->short_descr) && !is_name("kilt", obj->short_descr) && !is_name("towel", obj->short_descr) && !is_name("dress", obj->short_descr) && (does_cover(obj, COVERS_GROIN) || does_cover(obj, COVERS_ARSE) || does_cover(obj, COVERS_THIGHS) || does_cover(obj, COVERS_LOWER_LEGS) || does_cover(obj, COVERS_FEET))) {
            if (obj->wear_loc != WEAR_NONE)
            unequip_char(ch, obj);
          }
        }
      }
      return;
    }

    OBJ_DATA *container;
    container = create_object(get_obj_index(35), 0);
    char buf[MSL];

    sprintf(buf, "pile of %s's clothes", ch->pcdata->intro_desc);
    free_string(container->short_descr);
    container->short_descr = str_dup(buf);

    sprintf(buf, "A pile of %s's clothes", ch->pcdata->intro_desc);
    free_string(container->description);
    container->description = str_dup(buf);

    sprintf(buf, "pile clothes %s %s", ch->pcdata->intro_desc, ch->name);
    free_string(container->name);
    container->name = str_dup(buf);

    obj_to_room(container, ch->in_room);
    bool found = FALSE;
    for (toremove = 0; toremove < MAX_WEAR; toremove++) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj->wear_loc == toremove && (obj->item_type == ITEM_CLOTHING) && !is_name("skirt", obj->short_descr) && !is_name("miniskirt", obj->short_descr) && !is_name("mini-skirt", obj->short_descr) && !is_name("kilt", obj->short_descr) && !is_name("towel", obj->short_descr) && !is_name("dress", obj->short_descr) && (does_cover(obj, COVERS_GROIN) || does_cover(obj, COVERS_ARSE) || does_cover(obj, COVERS_THIGHS) || does_cover(obj, COVERS_LOWER_LEGS) || does_cover(obj, COVERS_FEET))) {
          found = TRUE;
          obj_from_char_silent(obj);
          obj_to_obj(obj, container);
        }
      }
    }
    if (found == FALSE)
    extract_obj(container);
    save_ground_objects();
  }

  void tear_clothing(CHAR_DATA *ch, int cover_location) {
    char buf[MSL];
    char arg1[MSL];
    int toremove;
    OBJ_DATA *obj;

    for (toremove = 0; toremove < MAX_WEAR; toremove++) {
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj->wear_loc == toremove && (can_see_obj(ch, obj)) && (obj->item_type == ITEM_CLOTHING) && (does_cover(obj, cover_location) || number_percent() % 3 == 0)) {

          if (is_name("torn", obj->short_descr) || is_name("loose", obj->short_descr) || is_name("baggy", obj->short_descr))
          continue;

          sprintf(buf, "torn %s", obj->short_descr);
          free_string(obj->short_descr);
          obj->short_descr = str_dup(buf);

          int val = number_range(0, MAX_COVERS - 1);
          if (does_cover(obj, cover_table[val]))
          obj->value[0] -= cover_table[val];

          obj->description = one_argument_nouncap(obj->description, arg1);
          sprintf(buf, "%s torn %s", arg1, obj->description);

          free_string(obj->description);
          obj->description = str_dup(buf);
          val = number_range(0, MAX_COVERS - 1);
          if (does_cover(obj, cover_table[val]))
          obj->value[0] -= cover_table[val];
        }
      }
    }
  }

  _DOFUN(do_howl) {
    CHAR_DATA *victim;

    if (IS_FLAG(ch->comm, COMM_RACIAL)) {
      send_to_char("You've done that too recently.\n\r", ch);
      return;
    }

    if (!is_werewolf(ch) || (ch->shape != SHAPE_WOLF)) {
      send_to_char("You lack the capacity to howl.\n\r", ch);
      return;
    }

    // ghostie check and limit for howl -Discordance
    if (is_ghost(ch)) {
      if (is_manifesting(ch)) {
        if (deplete_ghostpool(ch, GHOST_MANIFESTATION) == FALSE) {
          send_to_char("You can't muster the strength to project your voice any more today.\n\r", ch);
          return;
        }
      }
      else {
        send_to_char("You must be prepared to manifest this power.\n\r", ch);
        return;
      }
    }

    act("$n tilts $s head back and howls.", ch, NULL, NULL, TO_ROOM);
    dact("$n tilts $s head back and howls.", ch, NULL, NULL, DISTANCE_MEDIUM);
    act("You tilt your head back and howl.", ch, NULL, NULL, TO_CHAR);

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

      if (!in_haven(victim->in_room))
      continue;

      if (victim->in_room == ch->in_room)
      continue;

      if (is_deaf(victim))
      continue;

      if (!is_werewolf(victim))
      printf_to_char(victim, "You hear a wolf howl somewhere in the distance.");
      else
      printf_to_char(
      victim, "You hear a howl coming from %s.\n\r", relspacial[get_reldirection(roomdirection(get_roomx(victim->in_room), get_roomy(victim->in_room), get_roomx(ch->in_room), get_roomy(ch->in_room)), victim->facing)]);
    }
    SET_FLAG(ch->comm, COMM_RACIAL);
  }

  void fightshift(CHAR_DATA *ch) {
    if (in_fight(ch)) {
      if (ch->fight_fast == TRUE) {
        if (ch->move_timer > 0 && ch->attack_timer == 0)
        ch->attack_timer += FIGHT_WAIT * fight_speed(ch);
        else if (ch->move_timer == 0 && ch->attack_timer > 0)
        ch->move_timer += FIGHT_WAIT * fight_speed(ch);
        else
        apply_caff(ch, CAFF_HEARTSLOW, 1);
      }
      else {
        if (ch->moving == TRUE || IS_FLAG(ch->fightflag, FIGHT_NOMOVE)) {
          if ((ch->attackdam > 0 && ch->actiontimer > 0) || IS_FLAG(ch->fightflag, FIGHT_NOATTACK)) {
            apply_caff(ch, CAFF_HEARTSLOW, 2);
          }
          else
          useattack(ch);
        }
        else if ((ch->attackdam > 0 && ch->actiontimer > 0) || IS_FLAG(ch->fightflag, FIGHT_NOATTACK)) {
          usemove(ch);
        }
        else
        apply_caff(ch, CAFF_HEARTSLOW, 1);
      }
    }
    else if (get_skill(ch, SKILL_FLUIDSHIFT) <= 0) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 4;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_STUCKSHIFTER;
      affect_to_char(ch, &af);
    }
  }

  // transformation to human
  void human_transformation(CHAR_DATA *ch) {
    char buf[MSL];
    char cto[MSL];
    sprintf(cto, "%s", get_animal_changefrom(ch, ANIMAL_ACTIVE));
    if(strlen(cto) > 2)
    {
      act(cto, ch, NULL, NULL, TO_CHAR);
      act(cto, ch, NULL, NULL, TO_ROOM);
    }
    if (is_masked(ch) || is_cloaked(ch))
    sprintf(buf, "$n turns into someone.");
    else
    sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->intro_desc));
    visualact(buf, ch, NULL, NULL, TO_ROOM);
    if (get_skill(ch, SKILL_FLUIDSHIFT) > 0 && in_fight(ch) && SHAPE_HUMAN != ch->shape && SHAPE_HUMAN != ch->lastshape) {
      if (ch->fight_fast == TRUE)
      apply_caff(ch, CAFF_FLUIDSHIFT, 1);
      else
      apply_caff(ch, CAFF_FLUIDSHIFT, 2);
    }
    int hitperc = ch->hit * 1000 / max_hp(ch);
    ch->id = ch->id - ch->shape;
    ch->lastshape = ch->shape;
    ch->shape = SHAPE_HUMAN;
    fightshift(ch);
    ch->hit = max_hp(ch) * hitperc / 1000;
    send_to_char("You turn back into a human.\n\r", ch);
  }

  void animal_transformation(CHAR_DATA *ch, int newshape) {
    char buf[MSL];
    char oldintro[MSL];
    bool isanimal = FALSE;

    char cto[MSL];
    sprintf(cto, "%s", get_animal_changeto(ch, newshape));

    if (ch->shape == SHAPE_MERMAID)
    ch->shape = SHAPE_HUMAN;

    if (ch->shape != SHAPE_HUMAN && ch->shape != SHAPE_MERMAID) {
      isanimal = TRUE;
    }

    sprintf(oldintro, "%s", get_intro(ch));
    ch->id = ch->id - ch->shape + newshape;
    if (get_skill(ch, SKILL_FLUIDSHIFT) > 0 && in_fight(ch) && newshape != ch->shape && newshape != ch->lastshape) {
      if (ch->fight_fast == TRUE) {
        apply_caff(ch, CAFF_FLUIDSHIFT, 1);
      }
      else {
        apply_caff(ch, CAFF_FLUIDSHIFT, 2);
      }
    }

    ch->lastshape = ch->shape;
    ch->shape = newshape;
    if (is_masked(ch) || is_cloaked(ch)) {
      sprintf(oldintro, "someone");
    }

    if(strlen(cto) > 2)
    {
      act(cto, ch, NULL, NULL, TO_CHAR);
      act(cto, ch, NULL, NULL, TO_ROOM);
    }
    sprintf(buf, "%s turns into $n.", lowercase_clause(oldintro));
    visualact(buf, ch, NULL, NULL, TO_ROOM);
    printf_to_char(ch, "You turn into %s.\n\r", lowercase_clause(get_intro(ch)));

    if (get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1)) {
      if (!isanimal && animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) > ANIMAL_MANSIZED && number_percent() % 3 == 0) {
        tear_clothing(ch, COVERS_UPPER_BACK);
      }

      if ((get_skill(ch, SKILL_AURASHIFTING) <= 0 || shipment_carrier(ch) || animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) >=
            ANIMAL_MANSIZED)) {
        remove_clothing(ch);
      }
    }

    fightshift(ch);
  }

  // redid shift to account for new shapeshifting stat amoungst other things -
  // Discordance
  _DOFUN(do_shift) {
    char arg1[MSL];
    //    char string[MSL], buf[MSL];
    int point = -1;
    argument = one_argument_nouncap(argument, arg1);

    if (ch->your_car != NULL && ch->your_car->in_room != NULL && !is_animal(ch) && ch->pcdata->travel_to != -1)
    have_accident(ch);
    // errors for nonshifters
    if ((animal_pool_total(ch) < 8 && !is_werewolf(ch)) || event_cleanse == 1) {
      send_to_char("You aren't a shapeshifter.\n\r", ch);
      return;
    }
    else if (is_gm(ch) && !IS_IMMORTAL(ch)) {
      send_to_char("Storyrunners can't shift.\n\r", ch);
      return;
    }
    else if (is_ghost(ch)) {
      send_to_char("You have no body.\n\r", ch);
      return;
    }
    else if (is_animal(ch) && IS_AFFECTED(ch, AFF_ANIMALMINDED) && get_skill(ch, SKILL_FLUIDSHIFT) <= 0) {
      send_to_char("You feel like staying in this form.\n\r", ch);
      return;
    }
    else if (is_neutralized(ch)) {
      if (is_animal(ch)) {
        send_to_char("You don't understand what that is.\n\r", ch);
        return;
      }

      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    // errors for circumstances
    else if (is_helpless(ch)) {
      send_to_char("You can't move around enough to do that.\n\r", ch);
      return;
    }
    else if (is_animal(ch) && (IS_FLAG(ch->act, PLR_DEAD) || (ch->in_room != NULL && ch->in_room->vnum == 98))) {
      send_to_char("You are as the day you died.\n\r", ch);
      return;
    }
    else if (IS_FLAG(ch->affected_by, AFF_STUCKSHIFTER)) {
      send_to_char("You can't seem to shift.\n\r", ch);
      return;
    }
    else if (has_caff(ch, CAFF_NOSHIFTING)) {
      send_to_char("You can't seem to shift.\n\r", ch);
      return;
    }
    // else if(ch->pcdata->impregnated > 0 && ch->pcdata->impregnated <
    // (current_time-(3600*24*30)) && !is_animal(ch)) {send_to_char("You can't
    // manage to shift into your animal form.\n\r", ch);return;}
    else if (in_fight(ch) && ch->fight_fast == FALSE && ch->fight_current != ch) {
      send_to_char("It's not your turn yet.\n\r", ch);
      return;
    }

    if (room_hostile(ch->in_room)) {
      start_hostilefight(ch);
      return;
    }

    if (is_werewolf(ch)) {
      ch->pcdata->lastshift = current_time;
    }

    if (ch->in_room != NULL && battleground(ch->in_room) && ch->bagcarrier == 1) {
      CHAR_DATA *close;
      close = closest_character(ch);
      char buf[MSL];
      if (close != NULL) {
        close->bagcarrier = 1;
        sprintf(buf, "$n has %s.", bag_name());
        act(buf, close, NULL, NULL, TO_ROOM);
      }
      ch->bagcarrier = 0;
    }
    if (is_animal(ch)) {
      ch->pcdata->nightmare_shifted = 0;
      if (!str_cmp(arg1, "wolf") && ch->shape != SHAPE_WOLF && is_werewolf(ch)) {
        int hitperc = ch->hit * 1000 / max_hp(ch);
        animal_transformation(ch, SHAPE_WOLF);
        ch->hit = max_hp(ch) * hitperc / 1000;
        return;
      }
      point = animalpoint(ch, arg1);
      if (point == -1 || safe_strlen(arg1) < 1) {
        int hitperc = ch->hit * 1000 / max_hp(ch);
        human_transformation(ch);
        ch->hit = max_hp(ch) * hitperc / 1000;
      }
      else {
        if (!viable_form(ch, point)) {
          send_to_char("That form isn't viable.\n\r", ch);
          return;
        }
        if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) <=
            ANIMAL_SMALLMED && animal_size(get_animal_weight(ch, point)) > ANIMAL_MANSIZED) {
          send_to_char("You cannot make that large a direct shift.\n\r", ch);
          return;
        }
        if (animal_size(get_animal_weight(ch, ANIMAL_ACTIVE)) > ANIMAL_MANSIZED && animal_size(get_animal_weight(ch, point)) <= ANIMAL_SMALLMED) {
          send_to_char("You cannot make that large a direct shift.\n\r", ch);
          return;
        }
        int hitperc = ch->hit * 1000 / max_hp(ch);
        animal_transformation(ch, SHAPE_ANIMALONE + point);
        ch->hit = max_hp(ch) * hitperc / 1000;
      }
    }
    else {
      if (has_shroudmanip(ch)) {
        ch->pcdata->nightmare_shifted = 1;
      }
      point = animalpoint(ch, arg1);

      if (is_werewolf(ch) && (!str_cmp(arg1, "wolf") || point == -1)) {
        if (animal_size(get_animal_weight(ch, SHAPE_WOLF)) == ANIMAL_MONSTEROUS && IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
          send_to_char("You wouldn't fit.\n\r", ch);
          return;
        }

        int hitperc = ch->hit * 1000 / max_hp(ch);
        animal_transformation(ch, SHAPE_WOLF);
        ch->hit = max_hp(ch) * hitperc / 1000;
        return;
      }
      if (point == -1) {
        point = 0;
      }

      if (!viable_form(ch, point)) {
        send_to_char("That form isn't viable.\n\r", ch);
        return;
      }
      if (animal_size(get_animal_weight(ch, SHAPE_ANIMALONE + point)) ==
          ANIMAL_MONSTEROUS && IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        send_to_char("You wouldn't fit.\n\r", ch);
        return;
      }

      int hitperc = ch->hit * 1000 / max_hp(ch);
      animal_transformation(ch, SHAPE_ANIMALONE + point);
      ch->hit = max_hp(ch) * hitperc / 1000;
    }
  }

  /*  Old shifting lines
if(ch->race == RACE_FLIER)
{
if(ch->shape == SHAPE_HUMAN)
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->wolfintro)); act(buf, ch, NULL, NULL, TO_ROOM);
ch->id = ch->id - ch->shape + SHAPE_BIRD;
ch->shape = SHAPE_BIRD;
remove_clothing(ch);
printf_to_char(ch, "You turn into %s.\n\r", lowercase_clause(ch->pcdata->wolfintro)); return;
}
else
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->intro_desc)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape;
ch->shape = SHAPE_HUMAN;
send_to_char("You turn back into a human.\n\r", ch);
return;
}
}
else if(ch->race == RACE_SWIMMER)
{
if(ch->shape == SHAPE_HUMAN)
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->wolfintro)); act(buf, ch, NULL, NULL, TO_ROOM);
ch->id = ch->id - ch->shape + SHAPE_FISH;
ch->shape = SHAPE_FISH;
remove_clothing(ch);
printf_to_char(ch, "You turn into %s.\n\r", lowercase_clause(ch->pcdata->wolfintro)); return;
}
else
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->intro_desc)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape;
ch->shape = SHAPE_HUMAN;
send_to_char("You turn back into a human.\n\r", ch);
return;
}
}
else if(ch->race == RACE_FIGHTER)
{
if(ch->shape == SHAPE_HUMAN)
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->wolfintro)); act(buf, ch, NULL, NULL, TO_ROOM);
ch->id = ch->id - ch->shape + SHAPE_FIGHTER;
ch->shape = SHAPE_FIGHTER;
makeremove_clothing(ch);
printf_to_char(ch, "You turn into %s.\n\r", lowercase_clause(ch->pcdata->wolfintro)); return;
}
else
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->intro_desc)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape;
ch->shape = SHAPE_HUMAN;
send_to_char("You turn back into a human.\n\r", ch);
return;
}
}
else if(ch->race == RACE_RUNNER)
{
if(ch->shape == SHAPE_HUMAN)
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->wolfintro)); act(buf, ch, NULL, NULL, TO_ROOM);
ch->id = ch->id - ch->shape + SHAPE_RUNNER;
ch->shape = SHAPE_RUNNER;
remove_clothing(ch);
printf_to_char(ch, "You turn into %s.\n\r", lowercase_clause(ch->pcdata->wolfintro)); return;
}
else
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->intro_desc)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape;
ch->shape = SHAPE_HUMAN;
send_to_char("You turn back into a human.\n\r", ch);
return;
}
}

if(ch->race != RACE_WEREWOLF)
return;
if(!str_cmp(argument, "human"))
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->intro_desc)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape;
ch->shape = SHAPE_HUMAN;
send_to_char("You turn back into a human.\n\r", ch);
}
else if(!str_cmp(argument, "wolf"))
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->wolfintro)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape + SHAPE_WOLF;
ch->shape = SHAPE_WOLF;
if(in_fight(ch))
{
if(ch->fight_fast == TRUE)
{
ch->attack_timer += FIGHT_WAIT*fight_speed(ch);
}
else
SET_FLAG(ch->fightflag, FIGHT_NOATTACK);
}
send_to_char("You turn into a wolf.\n\r", ch);
remove_clothing(ch);

}
else if(!str_cmp(argument, "wolfman"))
{
sprintf(buf, "$n turns into %s.", lowercase_clause(ch->pcdata->wolfmanintro)); act(buf, ch, NULL, NULL, TO_ROOM);

ch->id = ch->id - ch->shape + SHAPE_WOLFMAN;
ch->shape = SHAPE_WOLFMAN;
if(in_fight(ch))
{
if(ch->fight_fast == TRUE)
{
ch->move_timer += FIGHT_WAIT*fight_speed(ch);
}
else
SET_FLAG(ch->fightflag, FIGHT_NOMOVE);
}
send_to_char("You turn into a wolfman.\n\r", ch);
tear_clothing(ch);

}
else
send_to_char("Syntax: shift human/wolf/wolfman\n\r", ch);
*/

  char *const astat_names[] = {
    "empty",      "power",   "toughness", "camouflage", "speed", "agility",    "senses",  "flying",    "nocturnal",  "venomous", "amphibious", "armored", "smelling",  "seeing",     "hearing", "fifteen",    "sixteen", "seventeen", "eighteen",   "nineteen", "twenty"};

  _DOFUN(do_form) {
    char arg1[MSL];
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    int point = -1;

    if (!str_cmp(arg1, "create")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      if (safe_strlen(arg2) < 3) {
        send_to_char("Syntax: form create (name)\n\r", ch);
        return;
      }
      if (animal_pool_per_animal(ch, animal_count(ch) + 1) < 8) {
        send_to_char("You don't have a large enough shapeshifting pool to support that many shapes.\n\r", ch);
        return;
      }

      for (int i = 0; i < 6; i++) {
        if (safe_strlen(ch->pcdata->animal_names[i]) < 3) {
          free_string(ch->pcdata->animal_names[i]);
          ch->pcdata->animal_names[i] = str_dup(arg2);
          printf_to_char(ch, "You create a new shapeshifting form named: %s\n\r", arg2);
          return;
        }
      }
      send_to_char("You can't create any more forms.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "genus")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      if (!str_cmp(argument, "Insect"))
      ch->pcdata->animal_genus[point] = GENUS_INSECT;
      else if (!str_cmp(argument, "rodent"))
      ch->pcdata->animal_genus[point] = GENUS_RODENT;
      else if (!str_cmp(argument, "bird"))
      ch->pcdata->animal_genus[point] = GENUS_BIRD;
      else if (!str_cmp(argument, "aquatic"))
      ch->pcdata->animal_genus[point] = GENUS_AQUATIC;
      else if (!str_cmp(argument, "reptile"))
      ch->pcdata->animal_genus[point] = GENUS_REPTILE;
      else if (!str_cmp(argument, "Mammal"))
      ch->pcdata->animal_genus[point] = GENUS_MAMMAL;
      else if (!str_cmp(argument, "mythological"))
      ch->pcdata->animal_genus[point] = GENUS_MYTHOLOGICAL;
      else if (!str_cmp(argument, "swarm"))
      ch->pcdata->animal_genus[point] = GENUS_SWARM;
      else if (!str_cmp(argument, "hybrid"))
      ch->pcdata->animal_genus[point] = GENUS_HYBRID;
      else {
        send_to_char("Valid genetic familes are: insect, rodent, bird, aquatic, reptile, mammal, mythological, swarm, hybrid.\n\r", ch);
        return;
      }
      send_to_char("Done.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "species")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: form species (name) (species)\n\r", ch);
        return;
      }
      free_string(ch->pcdata->animal_species[point]);
      ch->pcdata->animal_species[point] = str_dup(argument);
      printf_to_char(ch, "%s is now of the species %s.\n\r", ch->pcdata->animal_names[point], ch->pcdata->animal_species[point]);
      return;
    }
    else if (!str_cmp(arg1, "weight")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      float val = atof(argument);
      if (val < 0.1) // || val > 150000)
      {
        send_to_char("Valid weights are between 0.1 and 150000 pounds.\n\r", ch);
        return;
      }
      int newval = (int)((val * 10));
      ch->pcdata->animal_weights[point] = newval;
      printf_to_char(ch, "%s now weighs %f.\n\r", ch->pcdata->animal_names[point], val);
      return;

    }
    else if (!str_cmp(arg1, "intro")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: form intro (name) (introduction)\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "wolf") && is_werewolf(ch)) {
        free_string(ch->pcdata->wolfintro);
        ch->pcdata->wolfintro = str_dup(argument);
        printf_to_char(ch, "Your wolf form now appears as: %s.\n\r", ch->pcdata->wolfintro);
        return;
      }
      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->animal_intros[point]);
      ch->pcdata->animal_intros[point] = str_dup(argument);
      printf_to_char(ch, "%s now appears as: %s.\n\r", ch->pcdata->animal_names[point], ch->pcdata->animal_intros[point]);
      return;

    }
    else if (!str_cmp(arg1, "describe")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "wolf") && is_werewolf(ch)) {
        string_append(ch, &ch->pcdata->wolfdesc);
        return;
      }
      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      string_append(ch, &ch->pcdata->animal_descs[point]);
      return;
    }
    else if (!str_cmp(arg1, "changeto")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "wolf") && is_werewolf(ch)) {
        string_append(ch, &ch->pcdata->wolf_change_to);
        return;
      }
      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      string_append(ch, &ch->pcdata->animal_change_to[point]);
      return;

    }
    else if (!str_cmp(arg1, "changefrom")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      if (!str_cmp(arg2, "wolf") && is_werewolf(ch)) {
        string_append(ch, &ch->pcdata->wolf_change_from);
        return;
      }
      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      string_append(ch, &ch->pcdata->animal_change_from[point]);
      return;
    }
    else if (!str_cmp(arg1, "view")) {

      if (!str_cmp(arg2, "wolf") && is_werewolf(ch)) {
        view_animal(ch, ANIMAL_WOLF);
        return;
      }
      point = animalpoint(ch, arg2);
      if (point == -1) {
        if (is_werewolf(ch))
        view_animal(ch, ANIMAL_WOLF);
        for (int i = 0; i < 6; i++) {
          if (safe_strlen(ch->pcdata->animal_names[i]) > 2)
          view_animal(ch, i);
        }
      }
      else
      view_animal(ch, point);

    }
    else if (!str_cmp(arg1, "delete")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->animal_names[point]);
      ch->pcdata->animal_names[point] = str_dup("");
      free_string(ch->pcdata->animal_intros[point]);
      ch->pcdata->animal_intros[point] = str_dup("");
      free_string(ch->pcdata->animal_descs[point]);
      ch->pcdata->animal_descs[point] = str_dup("");
      free_string(ch->pcdata->animal_species[point]);
      ch->pcdata->animal_species[point] = str_dup("");
      ch->pcdata->animal_genus[point] = 0;
      ch->pcdata->animal_weights[point] = 0;
      for (int y = 0; y < 25; y++)
      ch->pcdata->animal_stats[point][y] = 0;
      send_to_char("Form deleted.\n\r", ch);
      if (ch->played / 3600 > 50)
      ch->pcdata->form_change_date = current_time + (3600 * 24 * 7);

      return;
    }
    else if (!str_cmp(arg1, "rename")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 2) {
        send_to_char("Syntax: form rename (name) (newname)\n\r", ch);
        return;
      }
      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg3);

      free_string(ch->pcdata->animal_names[point]);
      ch->pcdata->animal_names[point] = str_dup(arg3);
      printf_to_char(ch, "Form renamed to %s.\n\r", ch->pcdata->animal_names[point]);
      return;

    }
    else if (!str_cmp(arg1, "train")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }
      if (ch->pcdata->form_change_date > current_time + (3600 * 24 * 20))
      ch->pcdata->form_change_date = 0;
      if (ch->pcdata->form_change_date > current_time && ch->in_room != NULL && ch->in_room->vnum > 300) {
        send_to_char("You need to wait at least seven days since you last untrained to train again.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      for (int y = 0; y < 21; y++) {
        if (!str_cmp(argument, astat_names[y])) {
          if (ch->pcdata->animal_stats[point][y] >= 5 && (!str_cmp(argument, "power") || !str_cmp(argument, "toughness"))) {
            send_to_char("You can't raise that any further.\n\r", ch);
            return;
          }
          if (ch->pcdata->animal_stats[point][y] >= 1 && (!str_cmp(argument, "flying") || !str_cmp(argument, "nocturnal") || !str_cmp(argument, "venomous") || !str_cmp(argument, "amphibious") || !str_cmp(argument, "armored"))) {
            send_to_char("You can't raise that any further.\n\r", ch);
            return;
          }

          ch->pcdata->animal_stats[point][y]++;
          if (viable_form(ch, point))
          printf_to_char(ch, "You raise your form's %s.\n\r", astat_names[y]);
          else
          printf_to_char(
          ch, "You raise your form's %s. This form is `rnonviable.`x\n\r", astat_names[y]);
          return;
        }
      }
      send_to_char("Possible options are: power, toughness, camouflage, speed, agility, senses, flying, nocturnal, venomous, amphibious, armored, smelling, seeing, hearing.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg1, "untrain") || !str_cmp(arg1, "negtrain")) {
      if (is_animal(ch) || in_fight(ch)) {
        send_to_char("You need to be in human shape to do that.\n\r", ch);
        return;
      }

      point = animalpoint(ch, arg2);
      if (point == -1) {
        send_to_char("No such animal, try form view to see all your shapes.\n\r", ch);
        return;
      }
      for (int y = 0; y < 21; y++) {
        if (!str_cmp(argument, astat_names[y])) {
          if (ch->pcdata->animal_stats[point][y] <= 0) {
            send_to_char("You don't have any stat points in that.\n\r", ch);
            return;
          }
          ch->pcdata->animal_stats[point][y]--;
          if (viable_form(ch, point))
          printf_to_char(ch, "You lower your form's %s.\n\r", astat_names[y]);
          else
          printf_to_char(
          ch, "You lower your form's %s. This form is `rnonviable.`x\n\r", astat_names[y]);

          if (ch->played / 3600 > 50)
          ch->pcdata->form_change_date = current_time + (3600 * 24 * 7);
          return;
        }
      }
      send_to_char("Possible options are: power, toughness, camouflage, speed, agility, senses, flying, nocturnal, venomous, amphibious, armored,smelling, seeing, hearing.\n\r", ch);
      return;
    }
    else
    send_to_char("Syntax: Form Create/View/Genus/Species/Weight/Intro/Describe/Changeto/ChangeFrom/Rename/Delete/Train/Untrain.\n\r", ch);
  }

  _DOFUN(do_shifterstash) {
    if (IS_FLAG(ch->act, PLR_SHIFTSTASH)) {
      send_to_char("Your belongings will no longer be stashed.\n\r", ch);
      REMOVE_FLAG(ch->act, PLR_SHIFTSTASH);
      return;
    }
    else {
      send_to_char("Your belongings will be stashed when you shift.\n\r", ch);
      SET_FLAG(ch->act, PLR_SHIFTSTASH);
      return;
    }
  }

  _DOFUN(do_swat) {
    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if ((victim = get_char_room(ch, NULL, arg1)) == NULL) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }


    if (IS_FLAG(ch->act, PLR_SHROUD) != IS_FLAG(victim->act, PLR_SHROUD)) {
      send_to_char("They're not here.\n\r", ch);
      return;
    }

    if (is_ghost(ch)) {
      send_to_char("Your fist would find only air.\n\r", ch);
      return;
    }

    if(!is_animal(victim) || animal_size(get_animal_weight(victim, ANIMAL_ACTIVE)) != ANIMAL_TINY)
    {
      send_to_char("They seem a little large for that.\n\r", ch);
      return;
    }
    victim->pcdata->sleeping = 300;
    act("You swat $N", ch, NULL, victim, TO_CHAR);
    act("$n swats you.", ch, NULL, victim, TO_VICT);
    act("$n swats $N.", ch, NULL, victim, TO_NOTVICT);
  }

#if defined(__cplusplus)
}
#endif

#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include "merc.h"
#include "tables.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif

  /*
  * Lookup stats by name
  */
  int stat_lookup(const char *name) { return -1; }
  int sstat_lookup(const char *name) { return -1; }

  vector<char *> MaleNames;
  vector<char *> FemaleNames;
#define MNAME_FILE "../data/mname.txt"
#define FNAME_FILE "../data/fname.txt"

  void load_names() {
    FILE *fp;
    char buf[MSL];
    if ((fp = fopen(MNAME_FILE, "r")) != NULL) {
      for (;;) {
        char *word;
        /*
        letter = fread_letter( fp );
        if ( letter == '*' )
        {
        fread_to_eol( fp );
        continue;
        }
        */
        word = fread_word(fp);
        if (!str_cmp(word, "END")) {
          break;
        }
        else {
          sprintf(buf, "%s", word);
          MaleNames.push_back(str_dup(buf));
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open mname.txt", 0);
      exit(0);
    }
    if ((fp = fopen(FNAME_FILE, "r")) != NULL) {
      for (;;) {
        char *word;
        /*
        letter = fread_letter( fp );
        if ( letter == '*' )
        {
        fread_to_eol( fp );
        continue;
        }
        */
        word = fread_word(fp);
        if (!str_cmp(word, "END")) {
          break;
        }
        else {
          sprintf(buf, "%s", word);
          FemaleNames.push_back(str_dup(buf));
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open fname.txt", 0);
      exit(0);
    }
  }

  int get_eca(CHAR_DATA *ch) {
    if (!ch || ch == NULL)
    return 0;

    if (IS_NPC(ch))
    return ch->level;

    return 0;
  }

  int get_cslvl(CHAR_DATA *ch) {
    if (!ch || ch == NULL)
    return 0;
    return 0;
  }
  int get_pslvl(CHAR_DATA *ch) {
    if (!ch || ch == NULL)
    return 0;
    return 0;
  }

  bool defense_aug(CHAR_DATA *ch)
  {
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->augdisc_timer[i] > 0)
      {
        for (int j = 0; j < DIS_USED; j++) {
          if(discipline_table[j].vnum == ch->pcdata->augdisc_disc[i] && discipline_table[j].range == -1)
          return TRUE;
        }
      }
    }
    return FALSE;
  }


  int max_hp(CHAR_DATA *ch) {
    if (is_helpless(ch)) {
      if (IS_NPC(ch) || ch->pcdata->trance <= 0)
      return 1;
    }
    if (guestmonster(ch) && ch->pcdata->height_feet >= 10)
    return UMAX(shield_total(ch) * 10, 1);

    int val = UMAX(25, shield_total(ch) * 5);
    if (!IS_NPC(ch)) {
      int tier = get_tier(ch);
      int cap = 300;
      if(tier == 1)
      cap = 200;
      if(tier == 2)
      cap = 250;
      if(tier == 3)
      cap = 350;
      if(tier == 4)
      cap = 500;
      if(tier == 5)
      cap = 750;
      if (IS_AFFECTED(ch, AFF_PROTECT))
      cap = cap *11/10;
      if(in_cult_domain(ch))
      cap += 100;
      if(defense_aug(ch))
      cap = cap *11/10;

      if(val <= cap)
      return val;

      int rem = val - cap;
      float frem = (float)rem;
      float fmod = cbrt(frem*frem);
      val = cap + (int)fmod;
    }
    val = UMAX(val, 1);
    return val;
  }

  int shield_lowtotal(CHAR_DATA *ch) {
    int i, val = 0;
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      val += ch->disciplines[discipline_table[i].vnum];
    }
    return val;
  }
  
  int flag_lookup(const char *name, const struct flag_type *flag_table) {
    int flag;

    for (flag = 0; flag_table[flag].name != NULL; flag++) {
      if (LOWER(name[0]) == LOWER(flag_table[flag].name[0]) && !str_prefix(name, flag_table[flag].name))
      return flag_table[flag].bit;
    }

    return NO_FLAG;
  }

  int position_lookup(const char *name) {
    int pos;

    for (pos = 0; position_table[pos].name != NULL; pos++) {
      if (LOWER(name[0]) == LOWER(position_table[pos].name[0]) && !str_prefix(name, position_table[pos].name))
      return pos;
    }

    return -1;
  }

  int sex_lookup(const char *name) {
    int sex;

    for (sex = 0; sex_table[sex].name != NULL; sex++) {
      if (LOWER(name[0]) == LOWER(sex_table[sex].name[0]) && !str_prefix(name, sex_table[sex].name))
      return sex;
    }

    return -1;
  }

  int size_lookup(const char *name) { return 1; }

  /* returns race number */
  int bonus_lookup(const char *name) { return -1; }

  /* returns race number */
  int race_lookup(const char *name) {
    int race;

    for (race = 0; race < MAX_RACES; race++) {
      if (LOWER(name[0]) == LOWER(race_table[race].name[0]) && !str_cmp(name, race_table[race].name))
      return race;
    }
    for (race = 0; race < MAX_RACES; race++) {
      if (LOWER(name[0]) == LOWER(race_table[race].name[0]) && !str_prefix(name, race_table[race].name))
      return race;
    }

    return 0;
  }

  int item_lookup(const char *name) {
    int type;

    for (type = 0; item_table[type].name != NULL; type++) {
      if (LOWER(name[0]) == LOWER(item_table[type].name[0]) && !str_prefix(name, item_table[type].name))
      return item_table[type].type;
    }

    return -1;
  }

  int liq_lookup(const char *name) {
    int liq;

    for (liq = 0; liq_table[liq].liq_name != NULL; liq++) {
      if (LOWER(name[0]) == LOWER(liq_table[liq].liq_name[0]) && !str_prefix(name, liq_table[liq].liq_name))
      return liq;
    }

    return -1;
  }

  HELP_DATA *help_lookup(char *keyword) {
    HELP_DATA *pHelp;
    char temp[MIL], argall[MIL];

    argall[0] = '\0';

    while (keyword[0] != '\0') {
      keyword = one_argument(keyword, temp);
      if (argall[0] != '\0')
      strcat(argall, " ");
      strcat(argall, temp);
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    if (is_name(argall, pHelp->keyword))
    return pHelp;

    return NULL;
  }

  HELP_AREA *had_lookup(char *arg) {
    HELP_AREA *temp;
    extern HELP_AREA *had_list;

    for (temp = had_list; temp; temp = temp->next)
    if (!str_cmp(arg, temp->filename))
    return temp;

    return NULL;
  }

  /* returns material number */
  int material_lookup(const char *name) { return 0; }

  bool is_mute(CHAR_DATA *ch) {

    if (!higher_power(ch) && is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1))
    return TRUE;

    if (ch->pcdata->sleeping > 0)
    return TRUE;

    if (IS_FLAG(ch->affected_by, AFF_GAG))
    return TRUE;

    if (IS_FLAG(ch->affected_by, AFF_MUTE))
    return TRUE;

    if (IS_FLAG(ch->affected_by, AFF_STAKED))
    return TRUE;

    if (!IS_NPC(ch) && ch->pcdata->trance > 0)
    return TRUE;

    if (has_scuba(ch))
    return TRUE;

    return FALSE;
  }

  bool is_evil(CHAR_DATA *ch) { return FALSE; }

  bool is_good(CHAR_DATA *ch) { return FALSE; }

  bool is_forcibly_helpless(CHAR_DATA *ch) {
    if (IS_FLAG(ch->affected_by, AFF_PARALYZED))
    return TRUE;
    if (IS_FLAG(ch->affected_by, AFF_STAKED) && is_vampire(ch))
    return TRUE;
    if (IS_FLAG(ch->act, PLR_BOUND) && !is_ghost(ch))
    return TRUE;
    if (!IS_NPC(ch) && ch->pcdata->trance > 0)
    return TRUE;
    if (!IS_NPC(ch) && ch->pcdata->sleeping > 0)
    return TRUE;
    if (!IS_NPC(ch) && ch->in_room != NULL && ch->in_room->vnum == 4854 && ch->pcdata->detention_time > 0 && ch->pcdata->clique_role != CLIQUEROLE_BULLY)
    return TRUE;
    if (ch->wounds >= 3 && !is_undead(ch) && !guestmonster(ch) && !higher_power(ch))
    return TRUE;

    return FALSE;
  }

  bool is_helpless(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (is_forcibly_helpless(ch))
    return TRUE;
    if (IS_FLAG(ch->comm, COMM_HELPLESS))
    return TRUE;

    if (ch->pcdata->destiny_feature == DEST_FEAT_HELPLESS && pc_pop(ch->in_room) > 1)
    return TRUE;

    if (strcasestr(ch->pcdata->title, "sleeping") || strcasestr(ch->pcdata->title, "sleeps") || strcasestr(ch->pcdata->title, "asleep") || strcasestr(ch->pcdata->title, "unconcious") || strcasestr(ch->pcdata->title, "unconscious")) {
      if (!IS_FLAG(ch->act, PLR_SHROUD)) {
        return TRUE;
      }
    }

    return FALSE;
  }

  bool is_pinned(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    if (IS_NPC(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    if (room_ambushing(ch->in_room))
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      if (IS_NPC(victim))
      continue;

      if (!str_cmp(ch->pcdata->pinned_by, victim->name))
      return TRUE;
    }

    return FALSE;
  }

  bool is_pinning(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    if (IS_NPC(ch))
    return FALSE;

    if (room_ambushing(ch->in_room))
    return FALSE;

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      if (IS_NPC(victim))
      continue;

      if (!str_cmp(victim->pcdata->pinned_by, ch->name))
      return TRUE;
    }

    return FALSE;
  }

  bool is_same_align(CHAR_DATA *ch, CHAR_DATA *victim) { return TRUE; }

  bool is_prey(CHAR_DATA *ch) { return FALSE; }
  
  bool is_monster(CHAR_DATA *ch) { return FALSE; }

  bool is_human(CHAR_DATA *ch) { return FALSE; }

  int dstatus_lookup(const char *name) { return -1; }

  int weapon_lookup(const char *name) { return -1; }

  int weapon_type(CHAR_DATA *ch) {

    OBJ_DATA *obj;
    obj = get_weapon(ch);
    if (obj == NULL || obj->item_type != ITEM_WEAPON)
    return WEAPON_MACE;

    return obj->value[0];
  }

  bool check_for_color(char *word) {
    int i = 0;

    while (word[i] != '\0') {
      if (word[i] == '`')
      return TRUE;
      i++;
    }
    return FALSE;
  }

  bool string_alphanum(char *word) {
    int i = 0;

    while (word[i] != '\0') {
      if (!isalnum(word[i]))
      return FALSE;
      i++;
    }
    return TRUE;
  }
  bool string_alpha(char *word) {
    int i = 0;

    while (word[i] != '\0') {
      if (!isalpha(word[i]))
      return FALSE;
      i++;
    }
    return TRUE;
  }

  int attack_lookup(const char *name) { return 0; }

  /* returns a flag for wiznet */
  long wiznet_lookup(const char *name) {
    int flag;

    for (flag = 0; wiznet_table[flag].name != NULL; flag++) {
      if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0]) && !str_prefix(name, wiznet_table[flag].name))
      return flag;
    }

    return -1;
  }

  /* returns class number */
  int class_lookup(const char *name) { return -1; }

  int coin_lookup(const char *name) { return -1; }

  int metal_lookup(const char *name) { return -1; }

  int armor_lookup(const char *name) { return -1; }
  int hfile_type_lookup(const char *name) {
    int i;

    for (i = 0; i < MAX_HFILE_TYPE; i++) {
      if (LOWER(name[0]) == LOWER(hfile_type_table[i][0]) && !str_prefix(name, hfile_type_table[i]))
      return i;
    }
    return -1;
  }

  bool did_quest(CHAR_DATA *ch, int quest_value) { return FALSE; }

  bool has_echarm(CHAR_DATA *ch) { return FALSE; }
  OBJ_DATA *get_armor(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    int iWear;
    bool found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR && !found; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (IS_SET(obj->extra_flags, ITEM_ARMORED))
      found = TRUE;
    }
    if (found)
    return obj;
    else
    return NULL;
  }

  bool is_big(OBJ_DATA *obj) {
    if (obj->size > 25)
    return TRUE;

    return FALSE;
  }

  int get_big_items(CHAR_DATA *ch) {
    int count = 0;
    OBJ_DATA *obj;

    if (ch == NULL || ch->carrying == NULL)
    return 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc != WEAR_NONE)
      continue;
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;
      if (!is_big(obj))
      continue;

      count += obj->size;
    }
    return count;
  }

  int get_small_items(CHAR_DATA *ch) {
    int count = 0;
    OBJ_DATA *obj;

    if (ch == NULL || ch->carrying == NULL)
    return 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc != WEAR_NONE)
      continue;
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;
      if (is_big(obj))
      continue;

      count += obj->size;
    }
    return count;
  }
  bool carry_objects(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    if (ch == NULL || ch->carrying == NULL)
    return 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->wear_loc != WEAR_NONE)
      continue;
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      return TRUE;
    }
    return FALSE;
  }
  char *a_or_an(char *arg) {

    switch (arg[0]) {
    case 'a':
      return "an";
      break;
    case 'e':
      return "an";
      break;
    case 'i':
      return "an";
      break;
    case 'o':
      return "an";
      break;
    case 'u':
      return "an";
      break;
    case 'A':
      return "an";
      break;
    case 'E':
      return "an";
      break;
    case 'I':
      return "an";
      break;
    case 'O':
      return "an";
      break;
    case 'U':
      return "an";
      break;
    default:
      return "a";
      break;
    }
  }

  char *smart_a_or_an(char *arg) {
    if (!str_prefix("a ", arg) || !str_prefix("an ", arg))
    return "";
    char buf[MSL];
    sprintf(buf, "%s ", a_or_an(arg));
    return str_dup(buf);
  }

  /*
  bool does_cover(OBJ_DATA *obj, int selection)
  {
  int i;
  if(obj->item_type != ITEM_CLOTHING)
  return FALSE;
  if(obj->value[0] == 0)
  return FALSE;
  if(is_transperant(obj))
  return FALSE;

  int value = obj->value[0];
  for(i=0;i<MAX_COVERS;i++)
  {
  if(selection == cover_table[i])
  {
  if(value >= selection)
  return TRUE;
  else
  return FALSE;
  }
  else
  {
  if(value >= cover_table[i])
  value -= cover_table[i];
  }
  }
  return FALSE;
  }

  bool does_conceal(OBJ_DATA *obj_over, OBJ_DATA *obj_under)
  {
  int i;
  for(i=0;i<MAX_COVERS;i++)
  {
  if(does_cover(obj_under, i) && !does_cover(obj_over, i))
  return FALSE;
  }
  return TRUE;
  }
  */

  char *const vowel_anywhere[] = {
    "a",  "e",  "i",  "o",  "u",  "a",  "e",  "i",  "o",   "u",  "a",  "e", "i",  "o",  "u",  "a",  "e",  "i",  "o",  "u",  "a",   "e",  "i",  "o", "u",  "a",  "e",  "i",  "o",  "u",  "a",  "e",  "i",   "o",  "u",  "a", "e",  "i",  "o",  "u",  "a",  "e",  "i",  "o",  "u",   "a",  "e",  "i", "o",  "u",  "ae", "ai", "ao", "au", "aa", "ea", "eo",  "eu", "ee", "ia", "io", "iu", "ii", "oa", "oe", "oi", "ou", "oo", "eau", "y"};

  char *const const_anywhere[] = {
    "b",  "c",  "d",  "f",  "g", "h", "g", "h", "j", "k",  "l",  "m",  "n", "p",  "r",  "s",  "t",  "v", "w", "x", "y", "z", "sc", "ch", "gh", "ph", "sh", "th", "sk", "st", "b", "c", "d", "f", "g", "h",  "j",  "k",  "l", "m",  "n",  "p",  "r",  "s", "t", "v", "w", "b", "c",  "d",  "f",  "g", "h",  "j",  "k",  "l",  "m", "n", "p", "r", "s", "t",  "v",  "w"};

  char *const const_bigormid[] = {
    "qu", "wh", "cl", "fl", "gl", "kl", "ll",  "pl", "sl", "br", "cr", "dr", "fr", "gr", "kr", "pr", "sr", "tr", "str", "br", "dr", "fr", "gr", "kr"};
  char *const const_midorend[] = {"ck", "nk", "rk", "ss"};

  char *random_monster_name(void) {
    int length = number_range(2, 5);
    char name[MSL];
    char buf[MSL];
    name[0] = 0;
    int i, j;
    bool vowel = FALSE;

    for (i = 0; i <= length; i++) {
      if (i == 0) {
        j = number_percent() % 4;
        switch (j) {
        case 0:
        case 2:
          sprintf(buf, "%s", vowel_anywhere[number_range(1, 1000) % 70]);
          strcat(name, buf);
          vowel = TRUE;
          break;
        case 1:
          sprintf(buf, "%s", const_anywhere[number_range(1, 1000) % 64]);
          strcat(name, buf);
          vowel = FALSE;
          break;
        case 3:
          sprintf(buf, "%s", const_midorend[number_range(1, 1000) % 4]);
          strcat(name, buf);
          vowel = FALSE;
          break;
        }
      }
      else if (i >= length) {
        if (vowel == TRUE) {
          j = number_percent() % 3;
          switch (j) {
          case 0:
          case 2:
            sprintf(buf, "%s", const_anywhere[number_range(1, 1000) % 64]);
            strcat(name, buf);
            vowel = FALSE;
            break;
          case 1:
            sprintf(buf, "%s", const_bigormid[number_range(1, 1000) % 24]);
            strcat(name, buf);
            vowel = FALSE;
            break;
          }
        }
        else {
          sprintf(buf, "%s", vowel_anywhere[number_range(1, 1000) % 70]);
          strcat(name, buf);
          vowel = TRUE;
        }
      }
      else {
        if (vowel == TRUE) {
          j = number_percent() % 10;
          switch (j) {
          case 0:
          case 2:
          case 4:
          case 5:
          case 9:
          case 7:
            sprintf(buf, "%s", const_anywhere[number_range(1, 1000) % 64]);
            strcat(name, buf);
            vowel = FALSE;
            break;
          case 1:
          case 6:
          case 8:
            sprintf(buf, "%s", const_bigormid[number_range(1, 1000) % 24]);
            strcat(name, buf);
            vowel = FALSE;
            break;
          case 3:
            sprintf(buf, "%s", const_midorend[number_range(1, 1000) % 4]);
            strcat(name, buf);
            vowel = FALSE;
          }
        }
        else {
          sprintf(buf, "%s", vowel_anywhere[number_range(1, 1000) % 70]);
          strcat(name, buf);
          vowel = TRUE;
        }
      }
    }
    name[0] += 'A' - 'a';
    return str_dup(name);
  };

  char *random_male_name() {
    int i;
    i = number_range(1, 1000) % MaleNames.size();
    return MaleNames.at(i);
  }
  char *random_female_name() {
    int i;
    i = number_range(1, 1000) % FemaleNames.size();
    return FemaleNames.at(i);
  }
  _DOFUN(do_random) {
    int i;
    i = number_range(1, 1000) % MaleNames.size();
    printf_to_char(ch, "Random Male Name: %s\n\r", MaleNames.at(i));
    i = number_range(1, 1000) % FemaleNames.size();
    printf_to_char(ch, "Random Female Name: %s\n\r", FemaleNames.at(i));
    printf_to_char(ch, "Random Monster Name: %s\n\r", random_monster_name());
  }

  int get_diminish(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return 100;
    return 100;
  }

  bool has_vnum(CHAR_DATA *ch, int vnum) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->pIndexData->vnum == vnum)
        return TRUE;
      }
    }
    return FALSE;
  }
  int level_vnum(CHAR_DATA *ch, int vnum) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->pIndexData->vnum == vnum)
        return obj->level;
      }
    }
    return 0;
  }

  int get_num_weapons(CHAR_DATA *ch) { return 1; }

  int wallnumber(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room;
    EXIT_DATA *pexit;
    int dir = DIR_NORTH;
    int count = 0;
    if ((room = ch->in_room) == NULL)
    return 0;

    for (; dir <= DIR_SOUTHWEST; dir++) {
      pexit = room->exit[dir];
      if (dir != DIR_UP && dir != DIR_DOWN) {
        if (pexit == NULL)
        count++;
        else if (pexit->wall > WALL_NONE && pexit->wallcondition != WALLCOND_HOLE)
        count++;
        else if (IS_SET(pexit->exit_info, EX_CLOSED))
        count++;
      }
    }
    return count;
  }

  int get_lvl(CHAR_DATA *ch) {
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
    int total = total_disc(ch) - ch->disciplines[maxpoint] -
    ch->disciplines[nextmaxpoint];
    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;
      level += ch->disciplines[discipline_table[i].vnum];
      total -= ch->disciplines[discipline_table[i].vnum];
    }
    level += total / 5;
    level = UMAX(30, level);

    if (ch->spentexp >= 750000)
    level += 10;

    level += get_skill(ch, SKILL_MARTIALART);
    level += get_skill(ch, SKILL_COMBATTRAINING);
    level += get_skill(ch, SKILL_DODGING);
    level += get_skill(ch, SKILL_EVADING);

    level += get_tier(ch) * 50;
    level += (combat_focus(ch) + prof_focus(ch) + arcane_focus(ch))*10;

    return level;
  }

  bool holding_lgun(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_RANGED && obj->size >= 25)
    return TRUE;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_RANGED && obj->size >= 25)
    return TRUE;

    return FALSE;
  }
  bool holding_sgun(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_RANGED && obj->size < 25)
    return TRUE;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_RANGED && obj->size < 25)
    return TRUE;

    return FALSE;
  }

  bool dual_pistols(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) == NULL || obj->item_type != ITEM_RANGED || obj->size >= 25)
    return FALSE;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) == NULL || obj->item_type != ITEM_RANGED || obj->size >= 25)
    return FALSE;

    return TRUE;
  }

  bool has_lgun(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25)
      return TRUE;
    }
    return FALSE;
  }
  bool has_sgun(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size < 25)
      return TRUE;
    }
    return FALSE;
  }

  void equip_lgun(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL) {
          equip_char(ch, obj, WEAR_HOLD);
          if (under_opression(ch) && number_percent() % 25 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL) {
          equip_char(ch, obj, WEAR_HOLD_2);
          if (under_opression(ch) && number_percent() % 25 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
        else {
          unequip_char(ch, get_eqr_char(ch, WEAR_HOLD));
          equip_char(ch, obj, WEAR_HOLD);
          if (under_opression(ch) && number_percent() % 25 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
      }
    }
  }
  
  void equip_sgun(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_RANGED && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size < 25) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL) {
          equip_char(ch, obj, WEAR_HOLD);
          if (under_opression(ch) && number_percent() % 25 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL) {
          equip_char(ch, obj, WEAR_HOLD_2);
          if (under_opression(ch) && number_percent() % 25 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
        else {
          unequip_char(ch, get_eqr_char(ch, WEAR_HOLD));
          equip_char(ch, obj, WEAR_HOLD);
          if (under_opression(ch) && number_percent() % 25 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
      }
    }
  }
 
  bool real_weapon(OBJ_DATA *obj)
  {
    if(strcasestr(from_color(obj->short_descr), "club") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "dagger") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "axe") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "hammer") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "mace") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "staff") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "sickle") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "spear") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "flail") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "glaive") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "halberd") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "lance") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "sword") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "maul") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "morningstar") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "pike") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "rapier") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "knife") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "scimitar") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "trident") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "pick") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "saber") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "sabre") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "cutlass") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "scimitar") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "bayonet") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "katana") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "wakizashi") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "falchion") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "khopesh") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "gladius") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "kopis") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "stiletto") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "poignard") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "dirk") != NULL)
    return TRUE;
    if(strcasestr(from_color(obj->short_descr), "blade") != NULL)
    return TRUE;
  
    return FALSE;
  }
  
  bool real_armor(OBJ_DATA *obj)
  {
    if(strcasestr(from_color(obj->short_descr), "leather bodysuit") != NULL)
    return FALSE;
    if(strcasestr(from_color(obj->short_descr), "catsuit") != NULL)
    return FALSE;
  
    return TRUE;
  }

  bool holding_lweapon(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_WEAPON && obj->size >= 25 && real_weapon(obj))
    return TRUE;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_WEAPON && obj->size >= 25 && real_weapon(obj))
    return TRUE;

    return FALSE;
  }
  bool holding_sweapon(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if ((obj = get_eqr_char(ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_WEAPON && obj->size < 25 && real_weapon(obj))
    return TRUE;
    if ((obj = get_eqr_char(ch, WEAR_HOLD_2)) != NULL && obj->item_type == ITEM_WEAPON && obj->size < 25 && real_weapon(obj))
    return TRUE;

    return FALSE;
  }

  bool has_lweapon(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25 && real_weapon(obj))
      return TRUE;
    }
    return FALSE;
  }
  bool has_sweapon(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size < 25 && real_weapon(obj))
      return TRUE;
    }
    return FALSE;
  }

  void equip_lweapon(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size >= 25 && real_weapon(obj)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL) {
          equip_char(ch, obj, WEAR_HOLD);
          if (under_opression(ch) && number_percent() % 30 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL) {
          equip_char(ch, obj, WEAR_HOLD_2);
          if (under_opression(ch) && number_percent() % 30 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
        else {
          unequip_char(ch, get_eqr_char(ch, WEAR_HOLD));
          equip_char(ch, obj, WEAR_HOLD);
          if (under_opression(ch) && number_percent() % 30 == 0) {
            act("Your $p breaks.", ch, obj, NULL, TO_CHAR);
            extract_obj(obj);
          }
          return;
        }
      }
    }
  }

  void equip_sweapon(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (obj->item_type == ITEM_WEAPON && !IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->size < 25 && real_weapon(obj)) {
        if (get_eqr_char(ch, WEAR_HOLD) == NULL) {
          equip_char(ch, obj, WEAR_HOLD);
          return;
        }
        else if (get_eqr_char(ch, WEAR_HOLD_2) == NULL) {
          equip_char(ch, obj, WEAR_HOLD_2);
          return;
        }
        else {
          unequip_char(ch, get_eqr_char(ch, WEAR_HOLD));
          equip_char(ch, obj, WEAR_HOLD);
          return;
        }
      }
    }
  }

  bool has_shield(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (IS_SET(obj->extra_flags, ITEM_ARMORED))
        return TRUE;
      }
    }
    return FALSE;
  }

  bool wearing_armor(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;
    OBJ_DATA *obj;
    int iWear;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && IS_SET(obj->extra_flags, ITEM_ARMORED) && real_armor(obj))
      return TRUE;
    }
    return FALSE;
  }

  bool institute_contraband(OBJ_DATA *obj) {
    if (obj->item_type == ITEM_WEAPON && !strcasestr(obj->description, "practice") && obj->size >= 25)
    return TRUE;
    if (obj->item_type == ITEM_RANGED && !strcasestr(obj->description, "practice"))
    return TRUE;
    if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !strcasestr(obj->description, "practice"))
    return TRUE;

    return FALSE;
  }

  bool has_institute_contraband(CHAR_DATA *ch) {
    if (ch->race == RACE_DEPUTY) {
      return FALSE;
    }

    if (is_ghost(ch)) {
      return FALSE;
    }

    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      return FALSE;
    }

    if (ch->race == RACE_FACULTY) {
      return FALSE;
    }

    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (institute_contraband(obj)) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  void remove_institute_contraband(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    char buf[MSL];
    ch->pcdata->institute_contraband_violations++;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (institute_contraband(obj)) {
          if (obj->wear_loc != WEAR_NONE) {
            unequip_char(ch, obj);
            obj_from_char(obj);
            obj_to_room(obj, get_room_index(ROOM_INDEX_SHERIFFCAGE));
          }
          // obj->stash_room = ROOM_INDEX_SHERIFFCAGE;
          // SET_BIT(obj->extra_flags,ITEM_WARDROBE);
          act("A deputy frisks you and confiscates $a $p.", ch, obj, NULL, TO_CHAR);
          sprintf(buf, "A deputy frisks $n and confiscates %s %s.", dropprefix(obj), obj->short_descr);
          act(buf, ch, NULL, NULL, TO_ROOM);
        }
      }
    }

    return;
  }

  bool is_practice_arm(OBJ_DATA *obj) {
    if (obj->item_type == ITEM_WEAPON && strcasestr(obj->description, "practice"))
    return TRUE;
    if (obj->item_type == ITEM_RANGED && strcasestr(obj->description, "practice"))
    return TRUE;
    if (IS_SET(obj->extra_flags, ITEM_ARMORED) && strcasestr(obj->description, "practice"))
    return TRUE;

    return FALSE;
  }

  bool has_practice_arms(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (!IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
        if (is_practice_arm(obj)) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool wearing_collar(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;
    OBJ_DATA *obj;
    int iWear;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && IS_SET(obj->extra_flags, ITEM_COLLAR))
      return TRUE;
    }
    return FALSE;
  }

  bool is_clothed(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;
    OBJ_DATA *obj;
    int iWear;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && obj->item_type == ITEM_CLOTHING)
      return TRUE;
    }
    return FALSE;
  }

  bool has_scuba(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (IS_SET(obj->extra_flags, ITEM_SCUBA))
        return TRUE;
      }
    }
    return FALSE;
  }

  bool water_breathe(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;

    if (is_undead(ch))
    return TRUE;

    if (ch->race == RACE_SEAMONSTER)
    return TRUE;

    if (is_animal(ch)) {
      if (animal_stat(ch, ANIMAL_ACTIVE, ANIMAL_AMPHIBIOUS) > 0)
      return TRUE;
      if (get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_AQUATIC)
      return TRUE;
    }
    else if (is_metashifter(ch) && !is_neutralized(ch)) {
      if (animal_stat(ch, ANIMAL_PRIMARY, ANIMAL_AMPHIBIOUS) > 0)
      return TRUE;
      if (get_animal_genus(ch, ANIMAL_PRIMARY) == GENUS_AQUATIC)
      return TRUE;
    }

    if (IS_AFFECTED(ch, AFF_WATERBLESS))
    return TRUE;

    if (get_skill(ch, SKILL_MERSHIFTER) > 0)
    return TRUE;

    if (has_scuba(ch))
    return TRUE;

    return FALSE;
  }

  bool water_fighter(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;

    if (is_animal(ch)) {
      if (animal_stat(ch, ANIMAL_ACTIVE, ANIMAL_AMPHIBIOUS) > 0)
      return TRUE;
      if (get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_AQUATIC)
      return TRUE;
    }

    if (ch->race == RACE_SEAMONSTER)
    return TRUE;
    if (get_skill(ch, SKILL_MERSHIFTER) > 0)
    return TRUE;

    return FALSE;
  }

  bool is_swimmer(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;

    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_AQUATIC)
    return TRUE;

    if (get_skill(ch, SKILL_MERSHIFTER) > 0)
    return TRUE;

    if (ch->race == RACE_SEAMONSTER)
    return TRUE;

    return FALSE;
  }

  int pack_count(CHAR_DATA *ch) {
    CHAR_DATA *victim;
    int count = 0;
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

      if (same_pack(ch, victim))
      count++;
    }
    return count;
  }

  bool super_disc(int disc) {
    if (disc == DIS_FIRE || disc == DIS_ICE || disc == DIS_LIGHTNING || disc == DIS_DARK || disc == DIS_TOUGHNESS || disc == DIS_CUSTOM || disc == DIS_HELLFIRE || disc == DIS_SHADOWL || disc == DIS_BONES || disc == DIS_UNDEAD || disc == DIS_FORCES || disc == DIS_FATE || disc == DIS_PUSH || disc == DIS_PRISMATIC)
    return TRUE;

    return FALSE;
  }

  bool is_sorcerer(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (get_skill(ch, SKILL_DARKSORC) > 0)
    return TRUE;
    if (get_skill(ch, SKILL_PRISSORC) > 0)
    return TRUE;
    if (get_skill(ch, SKILL_LIGHTNINGSORC) > 0)
    return TRUE;
    if (get_skill(ch, SKILL_FIRESORC) > 0)
    return TRUE;
    if (get_skill(ch, SKILL_ICESORC) > 0)
    return TRUE;
  
    return FALSE;
  }

  int get_disc(CHAR_DATA *ch, int disc, bool usingit) {
    if (ch->race == RACE_CIVILIAN)
    return 0;

    int i, range = 0;
    int val = ch->disciplines[disc];

    for (i = 0; i < DIS_USED; i++) {
      if (disc == discipline_table[i].vnum)
      range = discipline_table[i].range;
    }

    int bonus = 0;

    if (IS_FLAG(ch->act, PLR_SINSPIRIT)) {
      if (disc == DIS_GRAPPLE)
      return 50;
      if (ch->pcdata->spirit_type == SPIRIT_JUSTICARUM) {
        if (disc == DIS_RADIATION)
        return 100;
      }
      else {
        if (disc == DIS_HELLFIRE)
        return 100;
      }
      if (disc == DIS_ENERGYS)
      return 100;
      return 0;
    }

    if (IS_NPC(ch) && pvp_character(ch)) {
      if (disc != DIS_SPEARGUN && disc != DIS_SPEAR) {
        if (deep_water(ch))
        val = val / 3;
        else if (is_water(ch->in_room))
        val = val * 3 / 4;
      }
    }

    if (!is_npcanimal(ch)) {
      if (disc == DIS_LONGBLADE)
      bonus = UMAX(ch->disciplines[DIS_SPEAR], UMAX(ch->disciplines[DIS_BLUNT], ch->disciplines[DIS_KNIFE])) / 4;
      if (disc == DIS_KNIFE)
      bonus = UMAX(ch->disciplines[DIS_SPEAR], UMAX(ch->disciplines[DIS_BLUNT], ch->disciplines[DIS_LONGBLADE])) / 4;
      if (disc == DIS_BLUNT)
      bonus = UMAX(ch->disciplines[DIS_SPEAR], UMAX(ch->disciplines[DIS_LONGBLADE], ch->disciplines[DIS_KNIFE])) / 4;
      if (disc == DIS_SPEAR)
      bonus = UMAX(ch->disciplines[DIS_BLUNT], UMAX(ch->disciplines[DIS_LONGBLADE], ch->disciplines[DIS_KNIFE])) / 4;

      if (disc == DIS_PISTOLS)
      bonus = UMAX(ch->disciplines[DIS_CARBINES], UMAX(ch->disciplines[DIS_RIFLES], ch->disciplines[DIS_SHOTGUNS])) / 3;
      if (disc == DIS_RIFLES)
      bonus = UMAX(ch->disciplines[DIS_CARBINES], UMAX(ch->disciplines[DIS_PISTOLS], ch->disciplines[DIS_SHOTGUNS])) / 3;
      if (disc == DIS_CARBINES)
      bonus = UMAX(ch->disciplines[DIS_PISTOLS], UMAX(ch->disciplines[DIS_RIFLES], ch->disciplines[DIS_SHOTGUNS])) / 3;
      if (disc == DIS_SHOTGUNS)
      bonus = UMAX(ch->disciplines[DIS_CARBINES], UMAX(ch->disciplines[DIS_RIFLES], ch->disciplines[DIS_PISTOLS])) / 3;

      if (disc == DIS_THROWN)
      bonus = UMAX(ch->disciplines[DIS_BOWS], ch->disciplines[DIS_SPEARGUN]) / 3;
      if (disc == DIS_BOWS)
      bonus = UMAX(ch->disciplines[DIS_THROWN], ch->disciplines[DIS_SPEARGUN]) / 3;
      if (disc == DIS_SPEARGUN)
      bonus = UMAX(ch->disciplines[DIS_THROWN], ch->disciplines[DIS_BOWS]) / 3;

      bonus -= val / 2;
      if (bonus > 0)
      val += bonus;

      if (disc == DIS_LONGBLADE || disc == DIS_KNIFE || disc == DIS_BLUNT || disc == DIS_SPEAR)
      val = val +
      UMAX(ch->disciplines[DIS_STRIKING], ch->disciplines[DIS_GRAPPLE]);
    }
    
    if(disc == DIS_STRIKING)
    val = UMAX(ch->disciplines[DIS_STRIKING], val + ch->disciplines[DIS_GRAPPLE]/2);
    if(disc == DIS_GRAPPLE)
    val = UMAX(ch->disciplines[DIS_GRAPPLE], val + ch->disciplines[DIS_STRIKING]/2);

    if (event_cleanse == 1 && (disc == DIS_STRIKING || disc == DIS_GRAPPLE) && val > 10)
    return 10;

    if (event_cleanse == 1 && (disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_KNIFE || disc == DIS_THROWN) && val > 25)
    return 25;

    if (event_cleanse == 1 && super_disc(disc))
    return 0;

    if (disc == DIS_CUSTOM && get_skill(ch, SKILL_CUSTOM) > 0)
    val = ch->disciplines[custom_vnum(ch)];

    if (is_super(ch) && (disc == DIS_PISTOLS || disc == DIS_CARBINES || disc == DIS_RIFLES || disc == DIS_SHOTGUNS)) {
      val = UMAX(0, val * (100 - (get_tier(ch) * (5 - get_skill(ch, SKILL_GUNDISCFOCUS)))) / 100);
    }
    if (is_super(ch) && disc == DIS_CUSTOM && (custom_vnum(ch) == DIS_PISTOLS || custom_vnum(ch) == DIS_CARBINES || custom_vnum(ch) == DIS_RIFLES || custom_vnum(ch) == DIS_SHOTGUNS)) {
      val = UMAX(0, val * (100 - (get_tier(ch) * (5 - get_skill(ch, SKILL_GUNDISCFOCUS)))) / 100);
    }
    if (disc == DIS_PISTOLS || disc == DIS_CARBINES || disc == DIS_RIFLES || disc == DIS_SHOTGUNS) {
      if (default_base_ranged(ch) == DIS_BOWS || default_base_ranged(ch) == DIS_THROWN)
      val = val * 4 / 5;
    }

    if (arcane_circle(ch->in_room) && range > -1 && disc != DIS_DARK && disc != DIS_PRISMATIC && disc != DIS_LIGHTNING && disc != DIS_FIRE && disc != DIS_ICE) {
      if (disc == DIS_CUSTOM)
      val = val / 2;
      else
      return 0;
    }

    if (disc == DIS_STRIKING || disc == DIS_GRAPPLE || disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_KNIFE || disc == DIS_SPEAR || disc == DIS_CUSTOM) {
      if (is_neutralized(ch)) {
        if ((disc == DIS_STRIKING || disc == DIS_GRAPPLE) && val > 10)
        return 10;
        if ((disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_KNIFE || disc == DIS_SPEAR) && val > 25)
        return 25;
        if (range > -1 && super_disc(disc))
        return 0;
        if (disc == DIS_CUSTOM && is_super(ch))
        return 0;
      }
    }

    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_DARKSORC) > 0) {
      if (disc == DIS_DARK) {
        val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
        val = UMAX(val, ch->disciplines[DIS_THROWN]);
        val = UMAX(val, ch->disciplines[DIS_CARBINES]);
        val = UMAX(val, ch->disciplines[DIS_RIFLES]);
        val = UMAX(val, ch->disciplines[DIS_BOWS]);
        val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
        val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);
        val = val * 130 / 100;
        if (in_world(ch) == WORLD_OTHER)
        val = val * 160 / 100;
        else if (in_world(ch) == WORLD_HELL || in_world(ch) == WORLD_WILDS || in_world(ch) == WORLD_GODREALM)
        val = val * 140 / 100;
        else if (!in_haven(ch->in_room))
        val = val * 4 / 5;
        if (!has_focus(ch))
        val = 0;
      }
    }
    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_PRISSORC) > 0) {
      if (disc == DIS_PRISMATIC) {
        val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
        val = UMAX(val, ch->disciplines[DIS_THROWN]);
        val = UMAX(val, ch->disciplines[DIS_CARBINES]);
        val = UMAX(val, ch->disciplines[DIS_RIFLES]);
        val = UMAX(val, ch->disciplines[DIS_BOWS]);
        val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
        val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);
        val = val * 130 / 100;
        if (in_world(ch) == WORLD_OTHER)
        val = val * 160 / 100;
        else if (in_world(ch) == WORLD_HELL || in_world(ch) == WORLD_WILDS || in_world(ch) == WORLD_GODREALM)
        val = val * 140 / 100;
        else if (!in_haven(ch->in_room))
        val = val * 4 / 5;
        if (!has_focus(ch))
        val = 0;
      }
    }
    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_LIGHTNINGSORC) > 0) {
      if (disc == DIS_LIGHTNING) {
        val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
        val = UMAX(val, ch->disciplines[DIS_THROWN]);
        val = UMAX(val, ch->disciplines[DIS_CARBINES]);
        val = UMAX(val, ch->disciplines[DIS_RIFLES]);
        val = UMAX(val, ch->disciplines[DIS_BOWS]);
        val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
        val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);
        val = val * 130 / 100;
        if (in_world(ch) == WORLD_OTHER)
        val = val * 160 / 100;
        else if (in_world(ch) == WORLD_HELL || in_world(ch) == WORLD_WILDS || in_world(ch) == WORLD_GODREALM)
        val = val * 140 / 100;
        else if (!in_haven(ch->in_room))
        val = val * 4 / 5;
        if (!has_focus(ch))
        val = 0;
      }
    }
    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_FIRESORC) > 0) {
      if (disc == DIS_FIRE) {
        val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
        val = UMAX(val, ch->disciplines[DIS_THROWN]);
        val = UMAX(val, ch->disciplines[DIS_CARBINES]);
        val = UMAX(val, ch->disciplines[DIS_RIFLES]);
        val = UMAX(val, ch->disciplines[DIS_BOWS]);
        val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
        val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);
        val = val * 130 / 100;
        if (in_world(ch) == WORLD_OTHER)
        val = val * 160 / 100;
        else if (in_world(ch) == WORLD_HELL || in_world(ch) == WORLD_WILDS || in_world(ch) == WORLD_GODREALM)
        val = val * 140 / 100;
        else if (!in_haven(ch->in_room))
        val = val * 4 / 5;
        if (!has_focus(ch))
        val = 0;
      }
    }
    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_ICESORC) > 0) {
      if (disc == DIS_ICE) {
        val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
        val = UMAX(val, ch->disciplines[DIS_THROWN]);
        val = UMAX(val, ch->disciplines[DIS_CARBINES]);
        val = UMAX(val, ch->disciplines[DIS_RIFLES]);
        val = UMAX(val, ch->disciplines[DIS_BOWS]);
        val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
        val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);
        val = val * 130 / 100;
        if (in_world(ch) == WORLD_OTHER)
        val = val * 160 / 100;
        else if (in_world(ch) == WORLD_HELL || in_world(ch) == WORLD_WILDS || in_world(ch) == WORLD_GODREALM)
        val = val * 140 / 100;
        else if (!in_haven(ch->in_room))
        val = val * 4 / 5;
        if (!has_focus(ch))
        val = 0;
      }
    }
    if (ch->shape == SHAPE_HUMAN && !IS_NPC(ch) && ch->pcdata->hell_power > current_time && ch->pcdata->hell_power < current_time + (3600 * 24 * 60)) {
      if (disc == DIS_HELLFIRE) {
        val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
        val = UMAX(val, ch->disciplines[DIS_THROWN]);
        val = UMAX(val, ch->disciplines[DIS_CARBINES]);
        val = UMAX(val, ch->disciplines[DIS_RIFLES]);
        val = UMAX(val, ch->disciplines[DIS_BOWS]);
        val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
        val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);
        val = val * 140 / 100;
        if (in_world(ch) == WORLD_HELL)
        val = val * 150 / 100;
      }
    }
    if (is_sorcerer(ch)) {
      if (disc == DIS_PISTOLS || disc == DIS_THROWN || disc == DIS_CARBINES || disc == DIS_RIFLES || disc == DIS_BOWS || disc == DIS_SHOTGUNS || disc == DIS_SPEARGUN)
      val = val * 3 / 4;
    }
    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_ARMORED) > 0) {
      if (disc == DIS_NARMOR)
      val = val + ch->disciplines[DIS_TOUGHNESS] + ch->disciplines[DIS_BONES] +
      ch->disciplines[DIS_FATE] + ch->disciplines[DIS_FORCES] +
      ch->disciplines[DIS_PUSH];
      if (disc == DIS_TOUGHNESS || disc == DIS_BONES || disc == DIS_FATE || disc == DIS_FORCES || disc == DIS_PUSH)
      val = 0;
    }

    if (is_animal(ch)) {
      if (disc == DIS_CLAW)
      val = get_animal_claw(ch, ANIMAL_ACTIVE);
      else if (disc == DIS_TOUGHNESS)
      val = get_animal_tough(ch, ANIMAL_ACTIVE);
      else if (get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1)) {
        val = 0;
      }
      else if (range == -1) {
        val = 0;
      }

      if (disc == DIS_NARMOR && animal_stat(ch, ANIMAL_ACTIVE, ANIMAL_ARMORED) > 0)
      val += 30;

      if (disc == DIS_CLAW && is_neutralized(ch))
      val = UMIN(val, 30);
    }

    if (!IS_NPC(ch) && (!usingit || range == -1)) {
      for (i = 0; i < 10; i++) {
        if (ch->pcdata->augdisc_timer[i] > 0 && ch->pcdata->augdisc_disc[i] == disc) {
          val += (int)(ch->pcdata->augdisc_level[i]);
        }
      }
    }

    if ((IS_AFFECTED(ch, AFF_PROTECT) || in_cult_domain(ch)) && disc == DIS_FATE)
    val += 20;

    if (has_caff(ch, CAFF_RESISTPAIN) && disc == DIS_TOUGHNESS)
    val += 20;

    if (val > 0 && !IS_NPC(ch)) {
      if (disc == DIS_RIFLES || disc == DIS_BOWS || disc == DIS_SHOTGUNS || disc == DIS_CARBINES || disc == DIS_SPEARGUN) {
        if (!holding_lgun(ch) && has_lgun(ch) && usingit)
        equip_lgun(ch);
        else if (!holding_lgun(ch) && !has_lgun(ch))
        val = 0;
      }
      
      if (disc == DIS_PISTOLS) {
        if (!holding_sgun(ch) && has_sgun(ch) && usingit)
        equip_sgun(ch);
        else if (!holding_sgun(ch) && !has_sgun(ch))
        val = 0;
      }
      
      if (disc == DIS_LONGBLADE || disc == DIS_BLUNT || disc == DIS_SPEAR) {
        if (!holding_lweapon(ch) && has_lweapon(ch) && usingit)
        equip_lweapon(ch);
        else if (!holding_lweapon(ch) && !has_lweapon(ch))
        val = 0;
      }
      
      if (disc == DIS_KNIFE || disc == DIS_THROWN) {
        if (!holding_sweapon(ch) && has_sweapon(ch) && usingit)
        equip_sweapon(ch);
        else if (!holding_sweapon(ch) && !has_sweapon(ch)) {
          if (get_skill(ch, SKILL_SOLDIER) < 2 || (!holding_lgun(ch) && !has_lgun(ch)) || disc == DIS_THROWN)
          val = 0;
        }
      }
      
      if (disc == DIS_BARMOR || disc == DIS_MARMOR) {
        if (!wearing_armor(ch))
        val = 0;
      }
      
      if (disc == DIS_BSHIELD || disc == DIS_MSHIELD) {
        if (!has_shield(ch))
        val = 0;
      }
      
      if ((disc == DIS_MSHIELD || disc == DIS_BSHIELD) && IS_FLAG(ch->fightflag, FIGHT_NOSHIELD))
      val /= 2;
      if (disc == DIS_SPEAR && IS_FLAG(ch->fightflag, FIGHT_NOSPEAR))
      val /= 2;

      if (ch->in_room != NULL) {
        if (deep_water(ch)) {
          if (disc == DIS_THROWN || disc == DIS_BOWS || disc == DIS_CARBINES || disc == DIS_RIFLES || disc == DIS_PISTOLS || disc == DIS_SHOTGUNS)
          val = 0;
          if (disc == DIS_LONGBLADE || disc == DIS_BLUNT)
          val = val / 5;
          if (disc == DIS_CUSTOM && (custom_vnum(ch) == DIS_BOWS || custom_vnum(ch) == DIS_CARBINES || custom_vnum(ch) == DIS_RIFLES || custom_vnum(ch) == DIS_PISTOLS || custom_vnum(ch) == DIS_SHOTGUNS))
          val = 0;
          if (is_underwater(ch->in_room)) {
            if (disc == DIS_LIGHTNING || disc == DIS_FIRE)
            val = 0;
          }
        }
        else {
          if (disc == DIS_SPEARGUN)
          val = val * 5 / 6;
          if (disc == DIS_CUSTOM && custom_vnum(ch) == DIS_SPEARGUN)
          val = val * 5 / 6;
        }
      }
    }
    if (shield_lowtotal(ch) <= 5 && wearing_armor(ch) && disc == DIS_BARMOR && !IS_NPC(ch))
    val = 10;

    if (IS_AFFECTED(ch, AFF_BONUS))
    val = val * 6 / 5;
  
    if (IS_AFFECTED(ch, AFF_PENALTY))
    val = val * 4 / 6;

    if (!IS_NPC(ch) && ch->pcdata->without_sleep > 6000)
    val = val / 2;

    if (disc == DIS_CLAW && is_werewolf(ch) && ch->shape != SHAPE_HUMAN)
    val += pack_count(ch) * 2;

    if (!IS_NPC(ch) && !IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->bloodaura > 0)
    val = val / 3;

    if (!IS_NPC(ch) && available_donated(ch) < 1750 && disc == DIS_CUSTOM)
    val = 0;

    if (IS_AFFECTED(ch, AFF_WARBLESS) && range >= 0 && val >= 10)
    val = UMIN(val * 12 / 10, val + 5);

    if (IS_AFFECTED(ch, AFF_BERSERK)) {
      if (discipline_table[disc].range == -1 && val > 0)
      val = UMIN(val - 1, val * 4 / 5);
      else if (ch->disciplines[disc] > 0 && range >= 0 && val > 0)
      val = UMAX(val * 14 / 10, val + 5);
    }
    
    if (IS_AFFECTED(ch, AFF_WEAKEN))
    val = UMIN(val - 1, val * 15 / 20);

    if (!IS_NPC(ch)) {
      if (ch->spentexp >= 750000 && ch->pcdata->recent_exp <= 0)
      val = UMIN(val - 1, val * 15 / 20);
    }

    if (!IS_NPC(ch) && get_drunk(ch) >= 50)
    val = UMIN(val - 1, val * 18 / 20);

    if (ch->in_room != NULL) {
      if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && disc == DIS_WEATHER && ch->disciplines[disc] > 0 && (raining == 1 || snowing == 1 || hailing == 1)) {
        val = UMAX(val * 11 / 10, val + 5);
      }
      else if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS) && disc == DIS_WEATHER && ch->disciplines[disc] > 0)
      val = UMAX(val - 5, val * 9 / 10);
    }

    if (!IS_NPC(ch) && has_con(ch, SCON_WEAKEN))
    val = UMIN(val - 1, val * 7 / 10);

    if (!IS_NPC(ch) && range >= 0 && is_animal(ch) && (!water_fighter(ch)) && (is_water(ch->in_room) || is_underwater(ch->in_room)))
    val = UMIN(val - 1, val * 5 / 10);

    if (!IS_NPC(ch) && range >= 0 && is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) == GENUS_AQUATIC && (is_water(ch->in_room) || is_underwater(ch->in_room)))
    val = (val * 13 / 10);

    if (!IS_NPC(ch) && get_energy(ch) >= 100)
    val = UMIN(val - 1, val * 3 / 5);

    val = UMAX(val, 0);

    return val;
  }

  bool is_metashifter(CHAR_DATA *ch) {
    if (ch->skills[SKILL_METASHIFTING] > 0)
    return TRUE;

    return FALSE;
  }

  bool illegal_augmentation(int skill) {
    if (skill == SKILL_WEALTH)
    return TRUE;
    if (skill == SKILL_POWERARMOR)
    return TRUE;
    if (skilltype(skill) == -1)
    return TRUE;
    if (skilltype(skill) == STYPE_ARCANEFOCUS)
    return TRUE;
    if (skilltype(skill) == STYPE_COMBATFOCUS)
    return TRUE;
    if (skilltype(skill) == STYPE_PROFFOCUS)
    return TRUE;
    if (skilltype(skill) == STYPE_ABILITIES)
    return TRUE;
    if (skilltype(skill) == STYPE_SABILITIES)
    return TRUE;
    if (skilltype(skill) == STYPE_ABOMINATION)
    return TRUE;
    if (skill == SKILL_LARCENY)
    return TRUE;
    if (skill == SKILL_ENGINEERING)
    return TRUE;
    if (skill == SKILL_RITUALISM)
    return TRUE;
    if (skill == SKILL_RUNNING)
    return TRUE;
    if (skill == SKILL_PARALYTIC)
    return TRUE;
    if (skill == SKILL_POISONTOXIN)
    return TRUE;

    return FALSE;
  }

  int get_skill(CHAR_DATA *ch, int skill) {
    int val = ch->skills[skill];
    int i;
    if (IS_NPC(ch)) {
      return val;
    }

    if (event_cleanse == 1 && (skilltype(skill) == STYPE_SUPERNATURAL || skilltype(skill) == STYPE_DEFENSE))
    return 0;
    if (event_cleanse == 1 && skilltype(skill) == STYPE_SABILITIES)
    return 0;
    if (event_cleanse == 1 && skilltype(skill) == STYPE_ABOMINATION)
    return 0;


    // other worlds - Disco 9/30/2020
    if (in_world(ch) == WORLD_WILDS) {
      if (skill == ch->pcdata->wilds_legendary) {
        return 3;
      }
      else if (val < ch->wilds_skills[skill]) {
        val = ch->wilds_skills[skill];
        return val;
      }
    }
    if (in_world(ch) == WORLD_OTHER) {
      if (skill == ch->pcdata->other_legendary) {
        return 3;
      }
      else if (val < ch->other_skills[skill]) {
        val = ch->other_skills[skill];
        return val;
      }
    }
    if (in_world(ch) == WORLD_GODREALM) {
      if (skill == ch->pcdata->godrealm_legendary) {
        return 3;
      }
      else if (val < ch->godrealm_skills[skill]) {
        val = ch->godrealm_skills[skill];
        return val;
      }
    }
    if (in_world(ch) == WORLD_HELL) {
      if (skill == ch->pcdata->hell_legendary) {
        return 3;
      }
      else if (val < ch->hell_skills[skill]) {
        val = ch->hell_skills[skill];
        return val;
      }
    }

    if (!IS_NPC(ch)) {
      for (i = 0; i < 10; i++) {
        if (!illegal_augmentation(skill)) {
          if (ch->pcdata->augskill_timer[i] > 0 && ch->pcdata->augskill_skill[i] == skill)
          val += ch->pcdata->augskill_level[i];
        }
      }
    }

    if(IS_FLAG(ch->act, PLR_INVIS) && skill == SKILL_STEALTH)
    return 5;
    if(IS_FLAG(ch->act, PLR_INVIS) && skill == SKILL_LARCENY)
    val += 2;


    if (skilltype(skill) == STYPE_ARCANEFOCUS || skilltype(skill) == STYPE_PROFFOCUS || skilltype(skill) == STYPE_COMBATFOCUS) {
      val += regress_mod(ch, skill);
      return val;
    }

    if (skill == SKILL_AGELESS) {
      if (ch->race == RACE_OLDWEREWOLF || ch->race == RACE_OLDFAEBORN || ch->race == RACE_OLDDEMONBORN || ch->race == RACE_OLDANGELBORN || ch->race == RACE_OLDDEMIGOD || ch->race == RACE_ANCIENTWEREWOLF || ch->race == RACE_DEMIDEMON || ch->race == RACE_FREEANGEL || ch->race == RACE_GODCHILD || ch->race == RACE_FAECHILD || ch->race == RACE_SPIRIT_FAE || ch->race == RACE_SPIRIT_DIVINE || ch->race == RACE_SPIRIT_DEMON || ch->race == RACE_SPIRIT_GHOST
          || ch->race == RACE_SPIRIT_CTHULIAN || ch->race == RACE_SPIRIT_PRIMAL) {
        return 4;
      }
    }
    if (skill == SKILL_NIGHTVISION && (higher_power(ch) || is_gm(ch)))
    return 5;

    if(skill == ch->pcdata->bonus_origin)
    return 1;

    if(skill == SKILL_ART || skill == SKILL_ART2 || skill == SKILL_ART3)
    {
      if(college_group(ch, FALSE) == COLLEGE_THEATRE)
      {
        if(institute_room(ch->in_room))
        val = UMAX(val, 3);
        else
        val = UMAX(val, 1);
      }
    }

    if (skill == SKILL_STRENGTH && IS_FLAG(ch->act, PLR_SINSPIRIT))
    val = UMAX(val, 8);
    if ((skill == SKILL_MINIONS || skill == SKILL_RITUALISM || skill == SKILL_INCANTATION) && IS_FLAG(ch->act, PLR_SINSPIRIT))
    return 5;
    if ((skill == SKILL_CLAIRAUDIENCE || skill == SKILL_CLAIRVOYANCE) && IS_FLAG(ch->act, PLR_SINSPIRIT))
    return 2;
    if (skill == SKILL_HYPNOTISM && IS_FLAG(ch->act, PLR_SINSPIRIT))
    return 4;
    if (skill == SKILL_BIOMANCY && ch->skills[SKILL_FAEKNIGHTFOCUS] < 1 && ch->shape != SHAPE_HUMAN)
    return 0;
    if (skill == SKILL_BIOMANCY && ch->skills[SKILL_DEMONWARRIORFOCUS] < 1 && ch->shape == SHAPE_HUMAN)
    return 0;

    if (skill == SKILL_PRIMALCOMBAT && !is_animal(ch))
    return 0;

    if (skill == SKILL_IMMORTALITY && !IS_NPC(ch) && ch->pcdata->godrealm_power > current_time && ch->pcdata->godrealm_power < current_time + (3600 * 24 * 60))
    return 2;

    if (skill == SKILL_SUPERJUMP && has_caff(ch, CAFF_AWIND))
    return 3;
    if (skill == SKILL_SUPERJUMP && has_caff(ch, CAFF_TKJUMP))
    return 3;

    if (skill == SKILL_GLIDE && !IS_NPC(ch) && has_caff(ch, CAFF_AWIND) && ch->pcdata->divine_focus == CAFF_AWIND)
    return 3;

    for (int i = 0; i < 10; i++) {
      if (skill == ch->pcdata->deactivated_stats[i])
      return 0;
    }

    if (skilltype(skill) == STYPE_SUPERNATURAL || skilltype(skill) == STYPE_DEFENSE || skilltype(skill) == STYPE_SABILITIES) {
      if (is_neutralized(ch)) {
        if (skilltype(skill) == STYPE_SUPERNATURAL || skilltype(skill) == STYPE_DEFENSE) {
          if (skill != SKILL_MIMIC && skill != SKILL_OBFUSCATE)
          return 0;
        }
        if (skilltype(skill) == STYPE_SABILITIES)
        return 0;
      }
    }

    if ((skill == SKILL_CLAIRVOYANCE || skill == SKILL_CLAIRAUDIENCE) && ch->skills[SKILL_TOUCHED] > 0) {
      if (number_percent() % 7 == 0)
      return 2;
      else if (number_percent() % 6 == 0)
      return 1;
    }

    if (is_gm(ch) && skilltype(skill) == STYPE_ABILITIES)
    return 5;
    if (is_gm(ch) && skilltype(skill) == STYPE_SABILITIES)
    return 5;

    if (is_gm(ch) && skill == SKILL_CLAIRVOYANCE)
    return 1;

    if (skill == SKILL_STEALTH)
    val += stakeout_bonus(ch);

    if (is_animal(ch)) {
      if (skilltype(skill) == STYPE_ABILITIES)
      return 0;
      if (skilltype(skill) == STYPE_SABILITIES) {
        if (skill == SKILL_COMBAT_ILLUSIONS && ch->skills[SKILL_ILLUSORYAFFINITY] <= 0)
        return 0;
        if ((skill == SKILL_NATURE || skill == SKILL_WEATHER) && ch->skills[SKILL_NATURALAFFINITY] <= 0)
        return 0;

        if (skill != SKILL_BIOMANCY && skill != SKILL_PRIMALCOMBAT && skill != SKILL_NATURE && skill != SKILL_WEATHER && skill != SKILL_COMBAT_ILLUSIONS) {
          return 0;
        }
      }

      if (skill == SKILL_ANIMALS)
      val++;
      if (skill == SKILL_PERCEPTION)
      return animal_skill(ch, ANIMAL_ACTIVE, SKILL_PERCEPTION);
      if (skill == SKILL_STEALTH)
      return animal_skill(ch, ANIMAL_ACTIVE, SKILL_STEALTH);
      if (skill == SKILL_ACUTESIGHT)
      return animal_skill(ch, ANIMAL_ACTIVE, skill);
      if (skill == SKILL_ACUTEHEAR)
      return animal_skill(ch, ANIMAL_ACTIVE, skill);
      if (skill == SKILL_ACUTESMELL)
      return animal_skill(ch, ANIMAL_ACTIVE, skill);
      if (skill == SKILL_NIGHTVISION)
      return animal_skill(ch, ANIMAL_ACTIVE, skill);
      if (skill == SKILL_SUPERJUMP)
      return animal_skill(ch, ANIMAL_ACTIVE, skill);

      if (skilltype(skill) == STYPE_PHYSICAL || skilltype(skill) == STYPE_SPECIALIZATION)
      return animal_skill(ch, ANIMAL_ACTIVE, skill);
    }
    else if (is_metashifter(ch) && !is_neutralized(ch)) {
      if (skill == SKILL_ACROBATICS)
      val += animal_skill(ch, ANIMAL_PRIMARY, SKILL_ACROBATICS);
      if (skill == SKILL_ACUTESIGHT)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill);
      if (skill == SKILL_ACUTEHEAR)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill);
      if (skill == SKILL_ACUTESMELL)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill);
      if (skill == SKILL_NIGHTVISION)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill);
      if (skill == SKILL_RUNNING)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_STRENGTH)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill);
      if (skill == SKILL_DEXTERITY)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_STAMINA)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_FASTREFLEXES)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_EVADING)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_PERCEPTION)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_STEALTH)
      val += animal_skill(ch, ANIMAL_PRIMARY, skill) / 2;
      if (skill == SKILL_MARTIALART)
      val += animal_skill(ch, ANIMAL_PRIMARY, SKILL_STRENGTH) / 2;
      val = UMIN(val, 5);
    }

    if (skill == SKILL_CAR) {
      val = vehicle_quality(ch);
    }
    if (skill == SKILL_BIKE) {
      val = vehicle_quality(ch);
    }

    if (!IS_NPC(ch)) {
      if (skilltype(skill) == STYPE_ABOMINATION && ch->pcdata->bloodaura > 0)
      return 0;
    }

    if (skilltype(skill) == STYPE_SOCIAL || skilltype(skill) == STYPE_PHYSICAL || skilltype(skill) == STYPE_SPECIALIZATION || skilltype(skill) == STYPE_INTELLECTUAL) {

      if (IS_AFFECTED(ch, AFF_LUCKY) && number_percent() % 5 == 0)
      val++;
      if (IS_AFFECTED(ch, AFF_UNLUCKY) && number_percent() % 5 == 0)
      val = UMAX(0, val - 1);
    }

    if (skill == SKILL_SUPERJUMP || skilltype(skill) == STYPE_PHYSICAL) {
      if (!IS_NPC(ch) && get_armor(ch) != NULL) {
        if (get_armor(ch)->buff < 0) {
          if (skilltype(skill) == STYPE_PHYSICAL && skill != SKILL_VIRGIN)
          val++;
          if (skill == SKILL_SUPERJUMP)
          val = UMAX(val, 2);
        }
      }
    }

    if (is_ghost(ch) && ch->possessing != NULL && skill == SKILL_FLIGHT)
    val = 5;
    if (is_ghost(ch) && skill == SKILL_NIGHTVISION)
    val = 5;

    if (val == -1)
    return val;

    int j, newval = 0;
    for (i = 0; i < SKILL_USED; i++) {
      if (skill_table[i].vnum == skill) {
        for (j = 0; j < 6; j++) {
          if (skill_table[i].levels[j] <= val) {
            if (j == 5 && skill_table[i].levels[j] > 0 && skill_table[i].levels[j] >= 0) {
              newval = skill_table[i].levels[j];
            }
            else if (j < 5 && skill_table[i].levels[j + 1] > val && skill_table[i].levels[j] >= 0) {
              newval = skill_table[i].levels[j];
            }
            else if (j < 5 && skill_table[i].levels[j + 1] == 0 && skill_table[i].levels[j] > 0) {
              newval = skill_table[i].levels[j];
            }
          }
        }
      }
    }
    val = newval;

    if (skill == SKILL_STRENGTH && get_skill(ch, SKILL_HYPERSTRENGTH) > 0)
    val *= 2;
    if (skill == SKILL_RUNNING && get_skill(ch, SKILL_HYPERSPEED) > 0)
    val = val * 2;
    if (skill == SKILL_PERCEPTION && get_skill(ch, SKILL_HYPERPERCEPTION) > 0)
    val *= 2;

    return val;
  }

  bool is_unlit(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (IS_SET(room->room_flags, ROOM_INDOORS) && crisis_blackout == 1 && in_haven(room))
    return TRUE;

    if (supernatural_darkness == 1 && in_haven(room))
    return TRUE;

    if (room->area->vnum == 13 && room->sector_type == SECT_STREET && crisis_blackout == 1)
    return TRUE;

    if (room_house(room) != NULL && room_house(room)->blackout > 0) {
      if (IS_SET(room->room_flags, ROOM_INDOORS) && (sunphase(room) < 3 || sunphase(room) > 5))
      return TRUE;
    }
    if (room_prop(room) != NULL && room_prop(room)->blackout > 0) {
      if (IS_SET(room->room_flags, ROOM_INDOORS) && (sunphase(room) < 3 || sunphase(room) > 5))
      return TRUE;
    }

    if (shadowcloaked(room)) {
      if (IS_SET(room->room_flags, ROOM_INDOORS))
      return TRUE;
      if (sunphase(room) <= 1 || crisis_darkness == 1 || sunphase(room) == 7)
      return TRUE;
    }

    if (IS_SET(room->room_flags, ROOM_LIGHTON)) {
      return FALSE;
    }
    if (IS_SET(room->room_flags, ROOM_DARK)) {
      return TRUE;
    }
    if (institute_room(room)) {
      if (IS_SET(room->room_flags, ROOM_INDOORS) || room->sector_type == SECT_STREET || room->sector_type == SECT_SIDEWALK || room->sector_type == SECT_PARK)
      return FALSE;
    }

    if (IS_SET(room->room_flags, ROOM_UNLIT)) {
      if (crisis_light == 1)
      return FALSE;
      else if (sunphase(room) <= 1 || crisis_darkness == 1 || sunphase(room) == 7) {
        return TRUE;
      }
      else
      return FALSE;
    }

    return FALSE;
  }

  bool is_dark(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (light_level(room) < 10)
    return TRUE;

    if (IS_SET(room->room_flags, ROOM_UNLIT)) {
      if (crisis_light == 1 && in_haven(room))
      return FALSE;
      else if (sunphase(room) <= 1 || crisis_darkness == 1 || sunphase(room) == 7) {
        tm *ptm;
        time_t east_time;

        east_time = current_time;
        ptm = gmtime(&east_time);

        if (moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL) != 0 && (cloud_cover(room) < 80 || cloud_density(room) < 60))
        return FALSE;
      }
      else
      return FALSE;
    }
    if (!is_unlit(room))
    return FALSE;

    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *temp_room;
    for (int i = 0; i < 10; i++) {
      if ((pexit = room->exit[i]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (pexit->wall != WALL_GLASS || !IS_SET(pexit->exit_info, EX_CURTAINS)) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU))) {
        if (!is_unlit(temp_room) && !shadowcloaked(room))
        return FALSE;
      }
    }
    return TRUE;
  }
  
  /*
  * True if room is dark.
  */
  bool room_is_dark(ROOM_INDEX_DATA *pRoomIndex) {
    return FALSE;

    if (pRoomIndex->light > 0)
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags, ROOM_DARK))
    return TRUE;

    if (IS_AFFECTED(pRoomIndex, AFF_XDARKEN))
    return TRUE;

    return FALSE;
  }

  bool is_outside(CHAR_DATA *ch) {
    if (ch->in_room->sector_type == SECT_STREET || ch->in_room->sector_type == SECT_ROOFTOP || ch->in_room->sector_type == SECT_UNDERWATER || ch->in_room->sector_type == SECT_SWAMP || ch->in_room->sector_type == SECT_PARK || ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_ATMOSPHERE || ch->in_room->sector_type == SECT_BEACH || ch->in_room->sector_type == SECT_FOREST || ch->in_room->sector_type == SECT_ROCKY || ch->in_room->sector_type == SECT_WATER || ch->in_room->sector_type == SECT_SHALLOW || ch->in_room->sector_type == SECT_CAVE) {
      return TRUE;
    }
    return FALSE;
  }

  bool is_dark_outside() {
    if (supernatural_darkness == 1) {
      return TRUE;
    }

    if (crisis_light == 1) {
      return FALSE;
    }
    else if (sunphase(get_room_index(105495492)) <= 1 || crisis_darkness == 1 || sunphase(get_room_index(105495492)) == 7) {
      return TRUE;
      /*
      tm * ptm;
      time_t east_time;

      east_time = current_time;
      ptm = gmtime ( &east_time );

      if(moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL) == 0) {
      return TRUE;
      }

      if(cloud_cover(get_room_index(105495492)) > 80) {
      return TRUE;
      }

      return FALSE;
      */
    }
    else {
      return FALSE;
    }

    return FALSE;
  }

  bool can_see_dark(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;
    if (light_level(ch->in_room) + skillpoint(get_skill(ch, SKILL_NIGHTVISION)) * 5 > 0)
    return TRUE;

    if (is_gm(ch))
    return TRUE;

    if (higher_power(ch))
    return TRUE;

    if (ch->in_room != NULL && battleground(ch->in_room))
    return TRUE;

    if (is_ghost(ch))
    return TRUE;
    if (ch->in_room != NULL && is_underwater(ch->in_room) && !IS_NPC(ch)) {
      if (get_skill(ch, SKILL_MERSHIFTER) > 0)
      return TRUE;
    }
    if (IS_IMMORTAL(ch))
    return TRUE;

    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for (CharList::iterator it = ch->in_room->people->begin(); it != ch->in_room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != ch->in_room)
      continue;

      if (!IS_NPC(victim) && event_cleanse == 0 && victim->pcdata->litup == 1)
      return TRUE;
      // Changed get_eq_char lines to get_held and get_hold instead to allow for
      // worn flashlights. - Discordance
      obj = get_held(victim, ITEM_FLASHLIGHT);

      if (obj != NULL) {
        if (!IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 1 && !shadowcloaked(victim->in_room))
        return TRUE;
      }

      obj = get_worn(victim, ITEM_FLASHLIGHT);

      if (obj != NULL) {
        if (!IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 1 && !shadowcloaked(victim->in_room))
        return TRUE;
      }
      // Adding a check to see if there's a light object that's illuminating the
      // room. - Discordance
      if (ch->in_room->contents != NULL && ch->in_room->contents->next_content == ch->in_room->contents) {
        if (ch->in_room->contents->item_type == ITEM_FLASHLIGHT && !IS_SET(ch->in_room->contents->extra_flags, ITEM_OFF) && ch->in_room->contents->value[0] > 1 && !shadowcloaked(victim->in_room))
        return TRUE;
      }
      else {
        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
          obj_next = obj->next_content;

          if (obj->item_type == ITEM_FLASHLIGHT && !IS_SET(obj->extra_flags, ITEM_OFF) && obj->value[0] > 1 && !shadowcloaked(victim->in_room))
          return TRUE;
        }
      }
    }

    return FALSE;
  }
  bool is_fighting(CHAR_DATA *ch) { return FALSE; }

  bool same_player(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (ch == NULL || victim == NULL)
    return FALSE;
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;
    if (ch->pcdata->job_type_one == JOB_EMPLOYEE && !str_cmp(ch->pcdata->job_title_one, victim->name))
    return FALSE;
    if (victim->pcdata->job_type_one == JOB_EMPLOYEE && !str_cmp(victim->pcdata->job_title_one, ch->name))
    return FALSE;

    if (ch->pcdata->account != NULL && victim->pcdata->account != NULL) {
      if (!str_cmp(ch->pcdata->account->name, victim->pcdata->account->name))
      return TRUE;
    }

    if (!str_cmp(ch->last_ip, victim->last_ip))
    return TRUE;

    if (str_cmp(ch->pcdata->email, victim->pcdata->email) && str_cmp(ch->pcdata->email, "None") && str_cmp(victim->pcdata->email, "None"))
    return FALSE;

    return FALSE;
  }

  bool timetraveler(CHAR_DATA *ch) {
    if (is_gm(ch))
    return TRUE;

    return FALSE;
  }

  bool is_smelly(CHAR_DATA *ch) {
    int iWear;
    OBJ_DATA *obj;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) == NULL)
      continue;

      if (obj->item_type != ITEM_CLOTHING)
      continue;

      if (obj->condition < 30)
      return TRUE;
    }
    return FALSE;
  }

  bool is_mage(CHAR_DATA *ch) {
    int i;
    for (i = DIS_ENERGY; i <= DIS_DARK; i++) {
      if (ch->disciplines[i] > 0)
      return TRUE;
    }
    if (ch->disciplines[DIS_STONE] > 0)
    return TRUE;

    for (i = DIS_ENERGYF; i <= DIS_DARKF; i++) {
      if (ch->disciplines[i] > 0)
      return TRUE;
    }
    for (i = DIS_ENERGYS; i <= DIS_DARKS; i++) {
      if (ch->disciplines[i] > 0)
      return TRUE;
    }
    return FALSE;
  }

  // shifter qualifiers - Discordance
  bool is_shifter(CHAR_DATA *ch) {
    if (is_werewolf(ch)) {
      return TRUE;
    }

    return FALSE;
  }

  // if character is in hybrid form
  bool is_hybrid(CHAR_DATA *ch) {
    if (ch->shape == SHAPE_MERMAID)
    return TRUE;
    if (ch->shape == SHAPE_WOLF && get_skill(ch, SKILL_HYBRIDSHIFTING) > 0)
    return TRUE;

    return FALSE;
  }

  // if character is in animal form
  bool is_animal(CHAR_DATA *ch) {

    if (ch->shape >= SHAPE_ANIMALONE && ch->shape <= SHAPE_ANIMALSIX)
    return TRUE;

    if (ch->shape == SHAPE_WOLF)
    return TRUE;

    return FALSE;
  }

  bool is_npcanimal(CHAR_DATA *ch) {
    if (IS_NPC(ch)) {
      if (ch->pIndexData->vnum == MINION_TEMPLATE)
      return TRUE;
    }
    return FALSE;
  }

  bool is_vampire(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;
    if (ch->race == RACE_NEWVAMPIRE)
    return TRUE;
    if (ch->race == RACE_VETVAMPIRE)
    return TRUE;
    if (ch->race == RACE_OLDVAMPIRE)
    return TRUE;
    if (ch->race == RACE_ANCIENTVAMPIRE)
    return TRUE;

    return FALSE;
  }
  bool is_werewolf(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;
    if (ch->race == RACE_NEWWEREWOLF)
    return TRUE;
    if (ch->race == RACE_VETWEREWOLF)
    return TRUE;
    if (ch->race == RACE_OLDWEREWOLF)
    return TRUE;
    if (ch->race == RACE_ANCIENTWEREWOLF)
    return TRUE;

    return FALSE;
  }

  bool is_demonborn(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;

    if (get_skill(ch, SKILL_DEMONBORN) > 0)
    return TRUE;
    if (ch->race == RACE_NEWDEMONBORN)
    return TRUE;
    if (ch->race == RACE_VETDEMONBORN)
    return TRUE;
    if (ch->race == RACE_OLDDEMONBORN)
    return TRUE;
    if (ch->race == RACE_DEMIDEMON)
    return TRUE;
    if (ch->race == RACE_SPIRIT_DEMON)
    return TRUE;
    if (pact_holder(ch))
    return TRUE;

    return FALSE;
  }

  bool is_angelborn(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;

    if (get_skill(ch, SKILL_ANGELBORN) > 0)
    return TRUE;

    if (ch->race == RACE_NEWANGELBORN)
    return TRUE;
    if (ch->race == RACE_VETANGELBORN)
    return TRUE;
    if (ch->race == RACE_OLDANGELBORN)
    return TRUE;
    if (ch->race == RACE_FREEANGEL)
    return TRUE;

    return FALSE;
  }

  bool is_demigod(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;

    if (get_skill(ch, SKILL_DEMIGOD) > 0)
    return TRUE;

    if (ch->race == RACE_NEWDEMIGOD)
    return TRUE;
    if (ch->race == RACE_VETDEMIGOD)
    return TRUE;
    if (ch->race == RACE_OLDDEMIGOD)
    return TRUE;
    if (ch->race == RACE_GODCHILD)
    return TRUE;
    if (ch->race == RACE_SPIRIT_DIVINE)
    return TRUE;

    return FALSE;
  }

  bool is_faeborn(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;

    if (get_skill(ch, SKILL_FAEBORN) > 0)
    return TRUE;

    if (ch->race == RACE_NEWFAEBORN)
    return TRUE;

    if (ch->race == RACE_VETFAEBORN)
    return TRUE;

    if (ch->race == RACE_OLDFAEBORN)
    return TRUE;

    if (ch->race == RACE_FAECHILD)
    return TRUE;
    if (ch->race == RACE_SPIRIT_FAE)
    return TRUE;

    return FALSE;
  }

  bool is_wildling(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;

    if (ch->race == RACE_WILDLING)
    return TRUE;

    return FALSE;
  }

  // superjumping preference toggle - Discordance
  bool is_superjumping(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_SUPERJUMPING)) {
      return FALSE;
    }

    return TRUE;
  }

  // flying preference toggle - Discordance
  bool is_flying(CHAR_DATA *ch) {

    if (ch == NULL)
    return FALSE;

    if (is_ghost(ch) && is_flying(ch->possessing))
    return TRUE;

    if (IS_FLAG(ch->act, PLR_FLYING)) {
      return FALSE;
    }
    if (!can_fly(ch)) {
      return FALSE;
    }

    if (ch->shape == SHAPE_HUMAN && get_skill(ch, SKILL_FLIGHT) > 0 && ch->pcdata->blackeyes == 0)
    apply_blackeyes(ch);

    return TRUE;
  }

  bool is_gm(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GUEST))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_GM))
    return TRUE;

    if (IS_IMMORTAL(ch))
    return TRUE;

    return FALSE;
  }

  bool is_guest(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GUEST))
    return TRUE;

    return FALSE;
  }

  bool senior_gm(CHAR_DATA *ch) {
    if (!is_gm(ch))
    return FALSE;

    if (IS_IMMORTAL(ch))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_DEPUTY))
    return TRUE;

    if (ch->played < (3600 * 100))
    return FALSE;

    if (ch->pcdata->earned_karma < 20000)
    return FALSE;

    return TRUE;
  }

  bool phoneroom(ROOM_INDEX_DATA *room) {
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
        if (get_phone(victim) != NULL)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool is_superspyproof(CHAR_DATA *victim) {
    if (room_prop(victim->in_room) != NULL && room_prop(victim->in_room)->type == PROP_HOUSE) {
      PROP_TYPE *prop = room_prop(victim->in_room);

      if (prop->warded >= 50)
      return TRUE;
    }

    return FALSE;
  }

  void npc_spy(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!IS_IMMORTAL(ch))
    return;

    if (victim == NULL || victim->in_room == NULL)
    return;

    if (is_superspyproof(victim))
    send_to_char("`B!`x", ch);

    if (!phoneroom(victim->in_room))
    send_to_char("`G!`x", ch);
  }

  bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (higher_power(ch) && power_bound(ch) && !IS_NPC(victim) && str_cmp(ch->pcdata->place, victim->pcdata->place)) {
      return TRUE;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim) && !same_player(ch, victim) && victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return TRUE;

    if (battleground(ch->in_room))
    return FALSE;

    if(victim->in_room != NULL && newbie_school(victim->in_room))
    return TRUE;

    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0')
      continue;
      if ((*it)->valid == FALSE)
      continue;
      if((*it)->isprotected == 1 && (*it)->time <= current_time && (*it)->time >= (current_time - (3600 * 6))) {
        CHAR_DATA * author = get_char_world_pc((*it)->author);
        if(author != NULL && (author->in_room == victim->in_room || author->in_room == ch->in_room))
        return TRUE;
      }

    }


    if (victim->in_room != NULL && (victim->in_room->vnum == ROOM_MEETING_WEST || victim->in_room->vnum == ROOM_MEETING_EAST))
    return TRUE;

    if (event_aegis == 1) {
      if (is_super(ch) && !is_super(victim))
      return TRUE;

      if (!is_super(ch) && is_super(victim))
      return TRUE;
    }

    if (IS_FLAG(ch->comm, COMM_FORCEDPACIFIST))
    return TRUE;

    if (victim->in_room != NULL && in_lodge(victim->in_room))
    return TRUE;

    if (victim->in_room != NULL && victim->in_room->vnum == 98)
    return TRUE;

    if (victim->in_room != NULL && victim->in_room->vnum >= 50 && victim->in_room->vnum <= 100)
    return TRUE;

    if (institute_room(victim->in_room) && (clinic_patient(victim) || college_student(victim, TRUE))) {
      for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
      it != EventVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;

        if (current_time < (*it)->active_time || current_time > (*it)->deactive_time)
        continue;

        if ((*it)->type == EVENT_UNDERSTANDINGMINUS) {
          return TRUE;
        }
        if ((*it)->type == EVENT_CLEANSE) {
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool spyroom(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    int yescount = 0;
    int nocount = 0;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL)
      continue;
      if (vch->in_room != room)
      continue;
      if (vch == ch)
      yescount += 5;
      else if (is_gm(vch) && !IS_IMMORTAL(ch))
      nocount += 5;
      if (vch->pcdata->in_domain > 0 && higher_power(ch) && vch->pcdata->domain_timer >= 5) {
        DOMAIN_TYPE *domain = vnum_domain(vch->pcdata->in_domain);
        if (domain != NULL && !str_cmp(ch->name, domain->domain_of)) {
          yescount++;
          continue;
        }
      }
      if (get_snooptrust(ch, vch) >= 3 && !IS_FLAG(vch->comm, COMM_PRIVATE))
      yescount++;
      else
      nocount++;
    }
    if (nocount >= yescount)
    return FALSE;

    return TRUE;
  }

  bool can_spy_normal(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
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

      if (is_gm(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room == room || get_room_index(victim->pcdata->dream_room) == room) {
        if (IS_FLAG(victim->act, PLR_SPYSHIELD))
        return FALSE;
      }
    }

    for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (is_gm(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room == room || get_room_index(victim->pcdata->dream_room) == room) {
        if (get_snooptrust(ch, victim) >= 3 && !IS_FLAG(victim->comm, COMM_PRIVATE))
        return TRUE;
      }
      if (get_room_index(victim->pcdata->dream_room) == room && !IS_FLAG(victim->comm, COMM_PRIVATE)) {
        FANTASY_TYPE *fant;
        if ((fant = in_fantasy(victim)) != NULL) {
          if (!str_cmp(fant->author, ch->name))
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  bool can_spy(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!is_gm(ch) && !higher_power(ch))
    return FALSE;

    if (victim->in_room == NULL)
    return FALSE;

    if (is_dreaming(victim) && higher_power(ch)) {
      FANTASY_TYPE *fant;
      if ((fant = in_fantasy(victim)) != NULL) {
        if (!str_cmp(ch->name, fant->author) && !IS_FLAG(victim->comm, COMM_PRIVATE))
        return TRUE;
      }
    }
    if ((base_room(victim->in_room)) && !IS_IMMORTAL(ch))
    return FALSE;

    if (silenced(victim) && !IS_IMMORTAL(ch))
    return FALSE;

    if (is_town_blackout() && !IS_IMMORTAL(ch))
    return FALSE;

    if (!spyroom(ch, victim->in_room) && !is_dreaming(victim) && (ch->level < 106 || !str_cmp(ch->name, "Ouroboros"))) {
      return FALSE;
    }

    if (can_spy_normal(ch, victim->in_room)) {
      npc_spy(ch, victim);
      return TRUE;
    }
    else {
      if (ch->level >= 106 && IS_IMMORTAL(ch) && !is_spyshield(victim)) {
        send_to_char("`R!`x", ch);
        npc_spy(ch, victim);
        return TRUE;
      }
      else
      return FALSE;
    }
  }

  int get_energy(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return 100;

    return ch->pcdata->fatigue / 10;
  }

  bool has_floor(ROOM_INDEX_DATA *room) {
    EXIT_DATA *pexit;
    if ((pexit = room->exit[DIR_DOWN]) == NULL || (pexit->wall != WALL_NONE && pexit->wallcondition != WALLCOND_HOLE) || (pexit->u1.to_room) == NULL)
    return TRUE;

    if (IS_SET(pexit->exit_info, EX_CLOSED))
    return TRUE;

    return FALSE;
  }

  bool is_air(ROOM_INDEX_DATA *room) {
    if (room == NULL || !room)
    return FALSE;

    if (has_floor(room))
    return FALSE;

    if (room->sector_type == SECT_AIR)
    return TRUE;

    if (room->sector_type == SECT_ATMOSPHERE)
    return TRUE;

    return FALSE;
  }

  bool is_underwater(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (room->sector_type == SECT_UNDERWATER)
    return TRUE;

    if (room->sector_type != SECT_WATER && room->sector_type != SECT_SHALLOW && room->sector_type != SECT_TUNNELS && in_haven(room)) {
      if (district_room(room) == DISTRICT_TOURIST && room->z < -2)
      return TRUE;

      if (district_room(room) == DISTRICT_REDLIGHT && room->z < -2)
      return TRUE;

      if (district_room(room) == DISTRICT_URBAN && room->z < -3)
      return TRUE;

      if (district_room(room) == DISTRICT_HISTORIC && room->z < -2)
      return TRUE;
    }

    if (crisis_flood == 1 && in_haven(room)) {
      if (room->z < 0) {
        return TRUE;
      }
    }

    return FALSE;
  }

  bool is_water(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (crisis_flood == 1 && in_haven(room)) {
      if (!IS_SET(room->room_flags, ROOM_INDOORS) && room->z < 1) {
        return TRUE;
      }

      if (room->z < 0) {
        return TRUE;
      }
    }

    if (room->sector_type == SECT_WATER)
    return TRUE;

    if (room->sector_type == SECT_UNDERWATER)
    return TRUE;

    if (room->sector_type == SECT_SHALLOW)
    return TRUE;

    if (in_haven(room)) {
      if (district_room(room) == DISTRICT_TOURIST && room->z < -1)
      return TRUE;

      if (district_room(room) == DISTRICT_REDLIGHT && room->z < -1)
      return TRUE;

      if (district_room(room) == DISTRICT_URBAN && room->z < -2)
      return TRUE;

      if (district_room(room) == DISTRICT_HISTORIC && room->z < -1 && prop_from_room(room) != NULL)
      return TRUE;
    }

    return FALSE;
  }

  bool airborne(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (ch->in_room->sector_type == SECT_AIR)
    return TRUE;

    if (ch->in_room->sector_type == SECT_ATMOSPHERE)
    return TRUE;

    return FALSE;
  }

  // Checks for medical facility - Discordance
  bool in_medical_facility(CHAR_DATA *ch) {
    if (in_medcent(ch->in_room)) {
      return TRUE;
    }

    return FALSE;
  }

  bool in_cosmetic(CHAR_DATA *ch) {
    if (in_medical_facility(ch)) {
      return TRUE;
    }

    if (is_helpless(ch)) {
      return TRUE;
    }

    return FALSE;
  }

  bool in_police_station(CHAR_DATA *ch) {
    if (strcasestr(ch->in_room->subarea, "police") != NULL || strcasestr(ch->in_room->subarea, "sheriff") != NULL
    || strcasestr(ch->in_room->subarea, "HSD") != NULL) {

      return TRUE;
    }

    return FALSE;
  }

  bool deep_water(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (ch->in_room->sector_type == SECT_WATER)
    return TRUE;

    if (ch->in_room->sector_type == SECT_UNDERWATER)
    return TRUE;

    if (crisis_flood == 1 && in_haven(ch->in_room)) {
      if (ch->in_room->z < 0)
      return TRUE;
      if (ch->in_room->z == 0) {
        if (ch->in_room->sector_type == SECT_SHALLOW)
        return TRUE;
        if (ch->in_room->sector_type == SECT_BEACH)
        return TRUE;
      }
    }

    if (in_bath(ch))
    return TRUE;
    if (in_stream(ch))
    return TRUE;

    return FALSE;
  }

  bool in_water(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (IS_NPC(ch))
    return FALSE;

    if (is_water(ch->in_room))
    return TRUE;

    if (in_shower(ch))
    return TRUE;
    if (in_stream(ch))
    return TRUE;

    return FALSE;
  }

  bool nearby_water(CHAR_DATA *ch) {
    if (in_water(ch))
    return TRUE;
    int door = 0;
    EXIT_DATA *pexit;

    for (door = 0; door <= 9; door++) {
      if ((pexit = ch->in_room->exit[door]) != NULL && pexit->u1.to_room != NULL) {

        if (is_water(pexit->u1.to_room))
        return TRUE;
      }
    }
    return FALSE;
  }

  int dollars(int amount) {
    int remainder = amount % 100;

    amount -= remainder;

    amount /= 100;

    return amount;
  }

  int cents(int amount) {
    int remainder = amount % 100;

    return remainder;
  }

  // determines if character can discern who another character is based on face exposure
  bool is_identifiable(CHAR_DATA *viewer, CHAR_DATA *victim) {
    int covered_locations = 0;
    bool occult = FALSE;

    // animals
    if (is_animal(victim))
    return FALSE;

    if (is_covered(victim, COVERS_HAIR)) {
      covered_locations = covered_locations + 1;
    }

    if (is_covered(victim, COVERS_FOREHEAD)) {
      covered_locations = covered_locations + 1;
    }

    if (is_covered(victim, COVERS_EYES)) {
      covered_locations = covered_locations + 1;
    }

    if (is_covered(victim, COVERS_LOWER_FACE)) {
      covered_locations = covered_locations + 1;
    }

    // no skin is showing
    if (occult == TRUE) {
      // lower legs, feet, thighs, arse, groin don't count
      if (victim->shape == SHAPE_MERMAID) {
        if (is_covered(victim, COVERS_HANDS) && is_covered(victim, COVERS_LOWER_ARMS) && is_covered(victim, COVERS_UPPER_ARMS) && is_covered(victim, COVERS_FEET) && is_covered(victim, COVERS_FOREHEAD) && is_covered(victim, COVERS_LOWER_BACK) && is_covered(victim, COVERS_UPPER_BACK) && is_covered(victim, COVERS_LOWER_CHEST) && is_covered(victim, COVERS_BREASTS) && is_covered(victim, COVERS_UPPER_CHEST) && is_covered(victim, COVERS_NECK) && is_covered(victim, COVERS_LOWER_FACE) && is_covered(victim, COVERS_HAIR)) {
          covered_locations = covered_locations + 1;
        }
      }
      else {
        if (is_covered(victim, COVERS_HANDS) && is_covered(victim, COVERS_LOWER_ARMS) && is_covered(victim, COVERS_UPPER_ARMS) && is_covered(victim, COVERS_FEET) && is_covered(victim, COVERS_LOWER_LEGS) && is_covered(victim, COVERS_FOREHEAD) && is_covered(victim, COVERS_THIGHS) && is_covered(victim, COVERS_GROIN) && is_covered(victim, COVERS_ARSE) && is_covered(victim, COVERS_LOWER_BACK) && is_covered(victim, COVERS_UPPER_BACK) && is_covered(victim, COVERS_LOWER_CHEST) && is_covered(victim, COVERS_BREASTS) && is_covered(victim, COVERS_UPPER_CHEST) && is_covered(victim, COVERS_NECK) && is_covered(victim, COVERS_LOWER_FACE) && is_covered(victim, COVERS_HAIR)) {
          covered_locations = covered_locations + 1;
        }

        // compares total covered locations vs perception + occult knowledge
        if (covered_locations > (get_skill(viewer, SKILL_PERCEPTION) +
              get_skill(viewer, SKILL_DEMONOLOGY))) {
          return FALSE;
        }
      }
    }
    else {
      if (is_covered(victim, COVERS_HANDS) && is_covered(victim, COVERS_LOWER_ARMS) && is_covered(victim, COVERS_UPPER_ARMS) && is_covered(victim, COVERS_FEET) && is_covered(victim, COVERS_LOWER_LEGS) && is_covered(victim, COVERS_FOREHEAD) && is_covered(victim, COVERS_THIGHS) && is_covered(victim, COVERS_GROIN) && is_covered(victim, COVERS_ARSE) && is_covered(victim, COVERS_LOWER_BACK) && is_covered(victim, COVERS_UPPER_BACK) && is_covered(victim, COVERS_LOWER_CHEST) && is_covered(victim, COVERS_BREASTS) && is_covered(victim, COVERS_UPPER_CHEST) && is_covered(victim, COVERS_NECK) && is_covered(victim, COVERS_LOWER_FACE) && is_covered(victim, COVERS_HAIR)) {
        covered_locations = covered_locations + 1;
      }

      // compares total covered locations vs perception + occult knowledge
      if (covered_locations > get_skill(viewer, SKILL_PERCEPTION)) {
        return FALSE;
      }
    }

    return TRUE;
  }

  bool is_super(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return TRUE;

    if (event_cleanse == 1)
    return FALSE;

    if (is_ghost(ch))
    return TRUE;

    if (ch->pcdata->tier_raised > 0)
    return TRUE;

    if (ch->race == RACE_WILDLING && arcane_focus(ch) > 0)
    return TRUE;

    if (ch->race == RACE_TIMESWEPT && arcane_focus(ch) > 0)
    return TRUE;

    if (ch->race == RACE_DREAMCHILD && arcane_focus(ch) > 0)
    return TRUE;


    if (ch->race == RACE_ELSEBORN && arcane_focus(ch) > 0)
    return TRUE;

    if (ch->race == RACE_FACULTY) {
      if (get_tier(ch) == 1)
      return FALSE;
      else
      return TRUE;
    }
    return race_table[ch->race].super;

    return FALSE;
  }

  bool seems_super(CHAR_DATA *ch) {
    if (!is_super(ch))
    return FALSE;

    if (is_ghost(ch))
    return TRUE;

    if (get_skill(ch, SKILL_OBFUSCATE) > 0)
    return FALSE;

    if (get_skill(ch, SKILL_MIMIC) > 0) {
      return race_table[get_app_race(ch)].super;
    }

    return is_super(ch);
  }

  int get_app_race(CHAR_DATA *ch) {
    if (get_skill(ch, SKILL_MIMIC) > 0)
    return ch->pcdata->mimic;

    return ch->race;
  }
  
  int get_race(CHAR_DATA *ch) { return ch->race; }

  bool has_shroudmanip(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (IS_SET(obj->extra_flags, ITEM_SHROUD))
        return TRUE;
      }
    }
    return FALSE;
  }

  bool can_shroud(CHAR_DATA *ch) {
    if (event_cleanse == 1)
    return FALSE;

    if (has_shroudmanip(ch))
    return TRUE;

    if (ch->shape != SHAPE_HUMAN && !IS_NPC(ch) && ch->pcdata->nightmare_shifted == 1)
    return TRUE;

    if (is_gm(ch))
    return TRUE;

    if (IS_FLAG(ch->act, PLR_SHROUD))
    return TRUE;

    return FALSE;
  }

  bool can_blood(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_DEAD))
    return FALSE;
    if (is_super(ch))
    return FALSE;

    return TRUE;
  }

  bool seems_under_understanding(CHAR_DATA *ch, CHAR_DATA *pers) {
    if (IS_NPC(ch))
    return FALSE;

    if (IS_NPC(pers))
    return FALSE;

    if (guestmonster(ch) || guestmonster(pers))
    return FALSE;

    if(!str_cmp(pers->pcdata->fixation_name, ch->name) && pers->pcdata->fixation_level >= 10)
    return FALSE;

    if (ch->pcdata->destiny_feature == DEST_FEAT_NOSANC)
    return FALSE;
    if (ch->pcdata->destiny_feature == DEST_FEAT_LIMITED)
    return FALSE;

    if (!str_cmp(ch->pcdata->understanding, "None"))
    return FALSE;
    if (!str_cmp(ch->pcdata->understanding, "Limited"))
    return FALSE;

    if (institute_room(ch->in_room)) {
      if (clinic_patient(ch) || college_student(ch, FALSE) || college_staff(ch, FALSE) || clinic_staff(ch, FALSE)) {
        return TRUE;
      }
    }

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (current_time < (*it)->active_time || current_time > (*it)->deactive_time)
      continue;

      if ((*it)->type == EVENT_UNDERSTANDINGMINUS || (*it)->typetwo == EVENT_UNDERSTANDINGMINUS) {
        return FALSE;
      }
    }

    if (cardinal(pers) && sinmatch(ch, pers))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_NOUNDERSTANDING))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_UNDERSTANDING))
    return TRUE;

    if (!in_haven(ch->in_room) && ch->in_room != NULL && ch->in_room->vnum != 18999 && ch->in_room->area->vnum != 31) {
      if (in_world(ch) != WORLD_EARTH) {
        if (IS_NPC(ch) || IS_NPC(pers))
        return FALSE;
        if (ch->pcdata->resident != 0 && ch->pcdata->resident == in_world(ch))
        return FALSE;
        if (pers->pcdata->resident != 0 && pers->pcdata->resident == in_world(pers))
        return FALSE;
      }
      else
      return FALSE;
    }

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(ch->pcdata->relationship[i], pers->name) && ch != pers && ch->pcdata->relationship_type[i] != REL_WEAKNESS_OF && ch->pcdata->relationship_type[i] != REL_PRAESTES && ch->pcdata->relationship_type[i] != REL_PRAESTES_OF)
      return FALSE;
    }

    if (get_skill(ch, SKILL_OBFUSCATE) > 0)
    return TRUE;

    if (IS_AFFECTED(ch, AFF_COMMUTE))
    return FALSE;

    if (ch->race == RACE_FACULTY) {
      if (college_staff(ch, FALSE) || clinic_staff(ch, FALSE)) {
        return TRUE;
      }
    }

    if (college_student(ch, FALSE)) {
      return TRUE;
    }

    if (!seems_super(ch) && !IS_AFFECTED(ch, AFF_SUPERTAINT))
    return TRUE;

    if (ch->played / 3600 < 125)
    return TRUE;

    if (ch->fcore != 0 && clan_lookup(ch->fcore) != NULL && clan_lookup(ch->fcore)->attributes[FACTION_UNDERSTANDING] > 0)
    return TRUE;

    if (ch->fsect != 0 && clan_lookup(ch->fsect) != NULL && clan_lookup(ch->fsect)->attributes[FACTION_UNDERSTANDING] > 0)
    return TRUE;

    if (ch->fcult != 0 && clan_lookup(ch->fcult) != NULL && clan_lookup(ch->fcult)->attributes[FACTION_UNDERSTANDING] > 0)
    return TRUE;

    return FALSE;
  }

  bool under_understanding(CHAR_DATA *ch, CHAR_DATA *pers) {
    if (IS_NPC(ch))
    return FALSE;

    if (IS_NPC(pers) && forest_monster(pers))
    return FALSE;

    if (guestmonster(ch) || guestmonster(pers))
    return FALSE;

    if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_NIGHTMARE)
    return FALSE;

    if(!str_cmp(pers->pcdata->fixation_name, ch->name) && pers->pcdata->fixation_level >= 10)
    return FALSE;

    if (!str_cmp(ch->pcdata->understanding, "None"))
    return FALSE;
    if (!str_cmp(ch->pcdata->understanding, "Limited"))
    return FALSE;

    if (ch->pcdata->destiny_feature == DEST_FEAT_NOSANC)
    return FALSE;
    if (ch->pcdata->destiny_feature == DEST_FEAT_LIMITED)
    return FALSE;

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin(); it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (current_time < (*it)->active_time || current_time > (*it)->deactive_time)
      continue;

      if ((*it)->type == EVENT_UNDERSTANDINGMINUS || (*it)->typetwo == EVENT_UNDERSTANDINGMINUS) {
        if (str_cmp(ch->name, (*it)->author))
        return FALSE;
      }
      if ((!in_haven(ch->in_room) || deepforest(ch->in_room)) && ch->in_room->area->vnum != 12) {
        if ((*it)->type == EVENT_CLEANSE || (*it)->typetwo == EVENT_CLEANSE) {
          return FALSE;
        }
      }
    }

    if (cardinal(pers) && sinmatch(ch, pers))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_NOUNDERSTANDING))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_UNDERSTANDING))
    return TRUE;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(ch->pcdata->relationship[i], pers->name) && ch != pers && ch->pcdata->relationship_type[i] != REL_WEAKNESS_OF && ch->pcdata->relationship_type[i] != REL_PRAESTES && ch->pcdata->relationship_type[i] != REL_PRAESTES_OF)
      return FALSE;
    }

    // institute rules here
    if (institute_room(ch->in_room) || ch->in_room->area->vnum == 12) // mists
    {
      if (clinic_patient(ch) || college_student(ch, FALSE)
      || college_staff(ch, FALSE) || clinic_staff(ch, FALSE))
      return TRUE;
    }

    if (get_skill(ch, SKILL_FLESHFORMED) > 0)
    return FALSE;

    if (ch->modifier == MODIFIER_FLESHFORMED)
    return FALSE;

    if (IS_AFFECTED(ch, AFF_COMMUTE))
    return FALSE;

    if(college_student(ch, FALSE))
    return TRUE;

    if (!in_haven(ch->in_room) && ch->in_room != NULL && ch->in_room->vnum != 18999 && ch->in_room->area->vnum != 31) {
      if (in_world(ch) != WORLD_EARTH) {
        if (IS_NPC(ch) || IS_NPC(pers))
        return FALSE;
        if (ch->pcdata->resident != 0 && ch->pcdata->resident == in_world(ch))
        return FALSE;
        if (pers->pcdata->resident != 0 && pers->pcdata->resident == in_world(pers))
        return FALSE;
      }
      else
      return FALSE;
    }
    if (ch->race == RACE_FACULTY) {
      if (college_staff(ch, FALSE) || clinic_staff(ch, FALSE))
      return TRUE;
    }

    if (IS_NPC(ch) && (ch->pIndexData->vnum == MINION_TEMPLATE || ch->pIndexData->vnum == ALLY_TEMPLATE || ch->pIndexData->vnum == HAND_SOLDIER))
    return TRUE;

    if (!is_super(ch) && !IS_AFFECTED(ch, AFF_SUPERTAINT))
    return TRUE;

    if (ch->played / 3600 < 125 && !is_guest(ch))
    return TRUE;

    if (ch->fcore != 0 && clan_lookup(ch->fcore) != NULL && clan_lookup(ch->fcore)->attributes[FACTION_UNDERSTANDING] > 0)
    return TRUE;

    if (ch->fcult != 0 && clan_lookup(ch->fcult) != NULL && clan_lookup(ch->fcult)->attributes[FACTION_UNDERSTANDING] > 0)
    return TRUE;


    if (ch->fsect != 0 && clan_lookup(ch->fsect) != NULL && clan_lookup(ch->fsect)->attributes[FACTION_UNDERSTANDING] > 0)
    return TRUE;


    return FALSE;
  }

  int fight_speed(CHAR_DATA *ch) {
    int val = ch->fight_speed;
    if (val < 2 || val > 20)
    return 1;

    return UMAX(1, val);
  }

  bool offworld(CHAR_DATA *ch) {
    if (ch == NULL || ch->in_room == NULL)
    return FALSE;

    if (get_world(ch) != WORLD_EARTH)
    return TRUE;

    return FALSE;
  }

  bool visiblyarmed(CHAR_DATA *ch) {
    if (holding_lgun(ch))
    return TRUE;
    if (holding_sgun(ch))
    return TRUE;
    if (has_lgun(ch))
    return TRUE;

    if (holding_lweapon(ch))
    return TRUE;
    if (holding_sweapon(ch))
    return TRUE;
    if (has_lweapon(ch))
    return TRUE;

    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (IS_SET(obj->extra_flags, ITEM_ARMORED))
        return TRUE;
      }
    }

    return FALSE;
  }

  bool visiblybloody(CHAR_DATA *ch) {
    if (ch->pcdata->blood[0] > 200)
    return TRUE;

    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->item_type == ITEM_CLOTHING && obj->value[3] > 100)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool largeweapons(CHAR_DATA *ch) {
    if (holding_lgun(ch))
    return TRUE;
    if (holding_sgun(ch))
    return TRUE;
    if (has_lgun(ch))
    return TRUE;

    if (holding_lweapon(ch))
    return TRUE;
    if (holding_sweapon(ch))
    return TRUE;
    if (has_lweapon(ch))
    return TRUE;

    OBJ_DATA *obj;
    OBJ_DATA *objcont;
    OBJ_DATA *objcont_next;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (IS_SET(obj->extra_flags, ITEM_ARMORED))
        return TRUE;
      }
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !IS_SET(obj->extra_flags, ITEM_WARDROBE))
      return TRUE;

      if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {

        for (objcont = obj->contains; objcont != NULL; objcont = objcont_next) {
          objcont_next = objcont->next_content;

          if (IS_SET(objcont->extra_flags, ITEM_ARMORED))
          return TRUE;
          if (objcont->item_type == ITEM_WEAPON && objcont->size >= 25)
          return TRUE;
          if (objcont->item_type == ITEM_RANGED && objcont->size >= 25)
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool heavyweapons(CHAR_DATA *ch) {
    if (holding_lgun(ch))
    return FALSE;
    if (has_lgun(ch))
    return FALSE;

    if (holding_lweapon(ch))
    return FALSE;
    if (has_lweapon(ch))
    return FALSE;

    OBJ_DATA *obj;
    OBJ_DATA *objcont;
    OBJ_DATA *objcont_next;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && (!can_see_obj(ch, obj) || (!can_see_wear(ch, iWear)))) {
        if (IS_SET(obj->extra_flags, ITEM_ARMORED))
        return TRUE;
      }
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !IS_SET(obj->extra_flags, ITEM_WARDROBE))
      return TRUE;

      if (obj->item_type == ITEM_CONTAINER && !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {

        for (objcont = obj->contains; objcont != NULL; objcont = objcont_next) {
          objcont_next = objcont->next_content;

          if (IS_SET(objcont->extra_flags, ITEM_ARMORED))
          return TRUE;
          if (objcont->item_type == ITEM_WEAPON && objcont->size >= 25)
          return TRUE;
          if (objcont->item_type == ITEM_RANGED && objcont->size >= 25)
          return TRUE;
        }
      }
    }

    return FALSE;
  }

  bool silenced(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return TRUE;
    if (in_house(ch) != NULL && in_house(ch)->blackout > 0)
    return TRUE;

    if (in_prop(ch) != NULL && in_prop(ch)->blackout > 0)
    return TRUE;

    if (IS_SET(ch->in_room->room_flags, ROOM_SILENCED))
    return TRUE;

    if (encounter_room(ch->in_room))
    return TRUE;

    return FALSE;
  }

  int skillinvested(CHAR_DATA *ch, int type) {
    int i, count = 0;
    for (i = 0; i < SKILL_USED; i++) {
      if (skilltype(skill_table[i].vnum) != type)
      continue;
      if (ch->skills[skill_table[i].vnum] == 0)
      continue;

      if (type == STYPE_ABILITIES || type == STYPE_SABILITIES) {
        if (ch->pcdata->ability_trees[0] == skill_table[i].vnum)
        count += ch->skills[skill_table[i].vnum];
        else
        count += (ch->skills[skill_table[i].vnum]) * 2;
      }
      else
      count += skillpoint(ch->skills[skill_table[i].vnum]);
    }
    return count;
  }

  int effective_trains(CHAR_DATA *ch) {
    int trains = ch->pcdata->strains;

    int penalty;
    penalty = 5 - skillinvested(ch, STYPE_SKILLS) -
    skillinvested(ch, STYPE_INTELLECTUAL);
    if (penalty > 0)
    trains -= penalty;
    penalty = 5 -
    (skillinvested(ch, STYPE_SPECIALIZATION) +
    skillinvested(ch, STYPE_PHYSICAL)) -
    skillinvested(ch, STYPE_CONTACTS);
    if (penalty > 0)
    trains -= penalty;
    penalty = 3 - skillinvested(ch, STYPE_ABILITIES) -
    skillinvested(ch, STYPE_SABILITIES);
    if (penalty > 0)
    trains -= penalty;

    return trains;
  }

  int neweffective_trains(CHAR_DATA *ch, int type, int cost) {
    int trains = ch->pcdata->strains;

    int penalty;
    penalty = 5 - skillinvested(ch, STYPE_SKILLS) -
    skillinvested(ch, STYPE_INTELLECTUAL);
    if (type == STYPE_SKILLS || type == STYPE_INTELLECTUAL)
    penalty -= cost;
    if (penalty > 0)
    trains -= penalty;
    penalty = 5 -
    (skillinvested(ch, STYPE_SPECIALIZATION) +
    skillinvested(ch, STYPE_PHYSICAL)) -
    skillinvested(ch, STYPE_CONTACTS);
    if (type == STYPE_PHYSICAL || type == STYPE_CONTACTS)
    penalty -= cost;
    if (penalty > 0)
    trains -= penalty;
    penalty = 3 - skillinvested(ch, STYPE_ABILITIES) -
    skillinvested(ch, STYPE_SABILITIES);
    if (type == STYPE_ABILITIES || type == STYPE_SABILITIES)
    penalty -= cost;
    if (penalty > 0)
    trains -= penalty;

    return trains;
  }

  bool is_dreaming(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (ch->pcdata->sleeping <= 0)
    return FALSE;
    if (ch->pcdata->dream_room > 0)
    return TRUE;
    if (!ch->pcdata->dream_link)
    return FALSE;
    if (ch->pcdata->dream_link == NULL)
    return FALSE;
    if (IS_NPC(ch->pcdata->dream_link))
    return FALSE;
    if (ch->pcdata->dream_link->in_room == NULL)
    return FALSE;

    return TRUE;
  }

  bool can_remoteinvade(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (ch->pcdata->dream_cooldown > 0)
    return FALSE;
    if (is_dreaming(ch))
    return FALSE;
    if (ch->in_room == NULL)
    return FALSE;
    if (in_water(ch))
    return FALSE;
    if (room_hostile(ch->in_room))
    return FALSE;
    if (public_room(ch->in_room))
    return FALSE;
    if (in_fight(ch))
    return FALSE;
    if (pc_pop(ch->in_room) > 1)
    return FALSE;
    if (get_gm(ch->in_room, FALSE) != NULL)
    return FALSE;

    return TRUE;
  }

  int newbie_level(CHAR_DATA *ch) {
    if (ch->played / 3600 < 60)
    return 1;
    else if (ch->played / 3600 < 65)
    return 2;
    else if (ch->played / 3600 < 70)
    return 3;

    return 10;
  }

  bool same_pack(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (!is_werewolf(ch) || !is_werewolf(victim))
    return FALSE;

    int count = 0;
    int count2 = 0;
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(ch->pcdata->relationship[i], victim->name) && ch->pcdata->relationship_type[i] == REL_PACKMATE)
      count = 1;
    }
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(victim->pcdata->relationship[i], ch->name) && victim->pcdata->relationship_type[i] == REL_PACKMATE)
      count2 = 1;
    }

    if (count == 1 && count2 == 1)
    return TRUE;

    return FALSE;
  }

  bool mindwarded(CHAR_DATA *ch) {
    if (IS_AFFECTED(ch, AFF_MINDWARD))
    return TRUE;

    if (get_skill(ch, SKILL_GUARDEDMIND) > 0)
    return TRUE;

    return FALSE;
  }

#define PROPERTY_TOWNHOUSE 1
#define PROPERTY_TOWNSHOP 2
#define PROPERTY_UNDERSHOP 3
#define PROPERTY_OVERFLAT 4
#define PROPERTY_LONGHOUSE 5
#define PROPERTY_LONGSHOP 6
#define PROPERTY_INNERFOREST 7
#define PROPERTY_OUTERFOREST 8
#define PROPERTY_TRAILER 9

  int town_minz(ROOM_INDEX_DATA *room, PROP_TYPE *prop) {

    if (prop == NULL) {
      if ((prop = prop_from_room(room)) == NULL) {
        return 0;
      }
    }
    if (prop == NULL) {
      return 0;
    }
    if (room == NULL) {
      room = first_room_in_property(prop);
    }
    if (room == NULL) {
      return 0;
    }
    if (prop->type_special == PROPERTY_SMALLFLAT)
    return 0;
    if (prop->type_special == PROPERTY_MEDFLAT)
    return 1;
    if (prop->type_special == PROPERTY_BIGFLAT)
    return 2;

    if (district_room(room) == DISTRICT_HISTORIC) {
      if (prop->type_special == PROPERTY_UNDERSHOP) {
        return 0;
      }
      else if (prop->type_special == PROPERTY_TRAILER) {
        return 0;
      }
      else if (prop->type_special == PROPERTY_OVERFLAT) {
        return 1;
      }
    }
    else if (district_room(room) == DISTRICT_TOURIST) {
      return -3;
    }
    else if (district_room(room) == DISTRICT_REDLIGHT) {
      return -3;
    }
    else if (district_room(room) == DISTRICT_URBAN) {
      if (prop->type == PROP_HOUSE) {
        return -3;

      }
      else {
        return -3;
      }
    }
    else if (prop->type_special == PROPERTY_INNERFOREST) {
      return -5;
    }
    else if (prop->type_special == PROPERTY_OUTERFOREST) {
      return -10;
    }

    return -3;
  }

  int town_maxz(ROOM_INDEX_DATA *room, PROP_TYPE *prop) {
    if (prop == NULL) {
      if ((prop = prop_from_room(room)) == NULL) {
        return 0;
      }
    }
    if (prop == NULL) {
      return 0;
    }
    if (room == NULL) {
      room = first_room_in_property(prop);
    }
    if (room == NULL) {
      return 0;
    }
    if (prop->type_special == PROPERTY_SMALLFLAT)
    return 0;
    if (prop->type_special == PROPERTY_MEDFLAT)
    return 1;
    if (prop->type_special == PROPERTY_BIGFLAT)
    return 2;

    if (district_room(room) == DISTRICT_HISTORIC) {
      if (prop->type_special == PROPERTY_UNDERSHOP) {
        return 0;
      }
      else if (prop->type_special == PROPERTY_TRAILER) {
        return 1;
      }
      else if (prop->type_special == PROPERTY_OVERFLAT) {
        return 2;
      }
    }
    else if (district_room(room) == DISTRICT_TOURIST) {
      return 1;
    }
    else if (district_room(room) == DISTRICT_REDLIGHT) {
      return 1;
    }
    else if (district_room(room) == DISTRICT_URBAN) {
      return 3;
    }
    else if (prop->type_special == PROPERTY_INNERFOREST) {
      return 6;
    }
    else if (prop->type_special == PROPERTY_OUTERFOREST) {
      return 15;
    }

    return 3;
  }

  bool in_haven(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (room->area == NULL)
    return FALSE;

    if (room->area->vnum >= 2 && room->area->vnum <= 11)
    return FALSE;

    if (room->area->vnum == 0)
    return FALSE;

    if (battleground(room))
    return FALSE;

    if (room->vnum == 18999)
    return FALSE;

    if (room->area->vnum >= 13 && room->area->vnum <= 21)
    return TRUE;

    if (room->area->vnum == 12 || room->area->vnum == 1) {
      if (room->x >= -60 && room->z <= 100 && room->y >= -30 && room->y <= 130)
      return TRUE;
    }
    return FALSE;
  }

  int get_roomx(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return -1000;
    return room->x;
  }
  
  int get_roomy(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return -1000;
    return room->y;
  }
  
  int get_roomz(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return -1000;
    return room->z;
  }

  // this returns the center of a currently occupied plot - Discordance
  int get_plotcenter(CHAR_DATA *ch) { return 0; }

  int get_roomplot(ROOM_INDEX_DATA *room) {
    int vnum = room->vnum;
    int value = vnum % 100000000;
    return value;
  }

  int get_coord_vnum(int x, int y, int z) {
    int vnum = 0;
    vnum += y;
    vnum += x * 1000;
    vnum += z * 1000000;
    return vnum;
  }

  int get_dist3d(int xone, int yone, int zone, int xtwo, int ytwo, int ztwo) {
    int totalx, totaly, totalz;

    if (xone > xtwo)
    totalx = xone - xtwo;
    else
    totalx = xtwo - xone;

    if (yone > ytwo)
    totaly = yone - ytwo;
    else
    totaly = ytwo - yone;

    if (zone > ztwo)
    totalz = zone - ztwo;
    else
    totalz = ztwo - zone;

    int total = totalx * totalx + totaly * totaly + totalz * totalz;

    total = (int)sqrt((double)total);
    return total;
  }

  // Locations
  bool in_sheriff(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return FALSE;
    }

    if (strcasestr(room->subarea, "HSD")) {
      return TRUE;
    }

    return FALSE;
  }

  bool in_lodge(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return FALSE;
    }

    if (!str_cmp(room->subarea, "lodge")) {
      return TRUE;
    }

    return FALSE;
  }

  bool builder_helper(CHAR_DATA *ch) {
    if (ch->pcdata->account != NULL) {
    }

    return FALSE;
  }

  bool in_medcent(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (!str_cmp(room->subarea, "doctor") || !str_cmp(room->subarea, "hospital") || !str_cmp(room->subarea, "surgery")
    || !str_cmp(room->subarea, "operating") || !str_cmp(room->subarea, "clinic") || room->sector_type == SECT_HOSPITAL) {
      return TRUE;
    }

    return FALSE;
  }

  int skillpointcost(CHAR_DATA *ch, int skill) {
    int val = ch->skills[skill];

    return skillpoint(val);
  }

  bool is_masked(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->shape != SHAPE_HUMAN && ch->shape != SHAPE_MERMAID)
    return FALSE;

    if (ch->pcdata->maskednumber == 1 && safe_strlen(ch->pcdata->mask_intro_one) > 3)
    return TRUE;

    if (ch->pcdata->maskednumber == 2 && safe_strlen(ch->pcdata->mask_intro_two) > 3)
    return TRUE;

    return FALSE;
  }

  bool is_cloaked(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (ch->shape != SHAPE_HUMAN)
    return FALSE;

    if (is_masked(ch))
    return FALSE;

    if (!IS_FLAG(ch->comm, COMM_CLOAKED))
    return FALSE;

    if (!is_covered(ch, COVERS_GROIN))
    return FALSE;

    return TRUE;
  }

  bool all_covered(CHAR_DATA *ch) {
    for (int j = 0; j < MAX_COVERS; j++) {
      if (!is_covered(ch, cover_table[j]))
      return FALSE;
    }

    return TRUE;
  }

  bool is_undead(CHAR_DATA *ch) {
    if (IS_NPC(ch)) {
      if (is_name("skeleton", ch->short_descr))
      return TRUE;
      if (is_name("ghost", ch->short_descr))
      return TRUE;
      if (is_name("zombie", ch->short_descr))
      return TRUE;
      if (is_name("revenant", ch->short_descr))
      return TRUE;

      return FALSE;
    }
    if (event_cleanse == 1)
    return FALSE;

    if (is_vampire(ch))
    return TRUE;
    if (ch->race == RACE_WIGHT)
    return TRUE;

    return FALSE;
  }

  int alt_count(CHAR_DATA *ch) {
    int count = 0;
    if(ch == NULL || IS_NPC(ch) || ch->pcdata == NULL || ch->pcdata->account == NULL)
    return 0;
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(ch->pcdata->account->characters[i]) > 2) {
        if (active_character(ch->pcdata->account->characters[i]))
        count++;
      }
    }
    return count;
  }

  int solidity(CHAR_DATA *ch) {
    if (IS_NPC(ch) || is_gm(ch))
    return 0;
    int starting = 160;
    int val = (int)sqrt(ch->pcdata->attract[ATTRACT_PROM]);
    starting -= val;
    starting -= (ch->pcdata->adventure_fatigue / 10);
    if (ch->faction != 0) {
      if (get_rank(ch, ch->faction) <= 2)
      starting -= 20;
      if (get_rank(ch, ch->faction) <= 1)
      starting -= 10;
    }
    else if (is_super(ch))
    starting -= 15;
    if (ch->played / 3600 < 15)
    starting -= 25;
    if (ch->pcdata->account != NULL) {
      if (ch->played / 3600 < 100 && ch->pcdata->account->maxhours > ch->played / 3600 + 10)
      starting -= 25;
    }
    int acount = alt_count(ch);
    starting -= acount * acount * 10;

    if (get_true_age(ch) < 19)
    starting -= 10;
    if (clothes_count(ch) < 30 && ch->played/3600 > 50)
    starting -= 15;
    if (clothes_count(ch) < 50 && ch->played/3600 > 100)
    starting -= 10;
    if (!housesleeper(ch) && !college_student(ch, FALSE))
    starting -= 15;
    if (ch->spentrpexp < 10000)
    starting -= 10;
    if (ch->spentrpexp < 50000)
    starting -= 10;
    if (ch->spentrpexp < 150000)
    starting -= 10;

    starting += (int)sqrt(clothes_count(ch));
    starting += (int)sqrt(ch->spentkarma);
    starting = UMAX(20, starting);
    starting = UMIN(200, starting);
    return starting;
  }

  bool animal_feeder(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;

    if (!is_vampire(ch))
    return FALSE;

    if (ch->pcdata->habit[HABIT_FEEDING] == 1)
    return TRUE;

    if (event_aegis == 1)
    return TRUE;

    return FALSE;
  }

  bool can_fly(CHAR_DATA *ch) {
    if (get_skill(ch, SKILL_FLIGHT) > 0)
    return TRUE;

    if (get_skill(ch, SKILL_WINGS) > 0 && !is_covered(ch, COVERS_UPPER_BACK))
    return TRUE;

    if (is_animal(ch) && animal_stat(ch, ANIMAL_ACTIVE, ANIMAL_FLIGHT) > 0)
    return TRUE;

    if (has_caff(ch, CAFF_GLIDING))
    return TRUE;

    return FALSE;
  }

  bool nighttime(ROOM_INDEX_DATA *room) {
    int sun = sunphase(room);

    if (sun == 0)
    return TRUE;
    if (sun == 1)
    return TRUE;
    if (sun == 7)
    return TRUE;

    return FALSE;
  }

  bool spammer(CHAR_DATA *ch) {
    if (ch->pcdata->account != NULL) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SPAMMER))
      return TRUE;
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
      return TRUE;

      if (is_griefer(ch))
      return TRUE;
    }
    
    if (ch->desc != NULL && ch->desc->account != NULL) {
      if (IS_FLAG(ch->desc->account->flags, ACCOUNT_SPAMMER))
      return TRUE;
      if (IS_FLAG(ch->desc->account->flags, ACCOUNT_SHADOWBAN))
      return TRUE;
      if (IS_FLAG(ch->desc->account->flags, ACCOUNT_GRIEFER))
      return TRUE;
    }
    if(ch->in_room == NULL)
    return FALSE;

    if(ch->in_room->vnum == ROOM_VNUM_CREATION || (ch->in_room->vnum >= 50 && ch->in_room->vnum <= 65 && ch->in_room->vnum != GMHOME))
    {
      if(ch->played/3600 > 20 && ch->pcdata->emotes[EMOTE_TOTAL] < 20)
      return TRUE;
    }

    return FALSE;
  }

  bool valid_money(OBJ_DATA *obj, CHAR_DATA *ch) {
    if (!str_cmp(obj->material, ch->name))
    return TRUE;

    if (obj->level == 99)
    return FALSE;

    if (ch->pcdata->account != NULL) {
      for (int i = 0; i < 10; i++) {
        if (!str_cmp(obj->material, ch->pcdata->account->characters[i]) && safe_strlen(obj->material) > 1)
        return FALSE;
      }
    }
    return TRUE;
  }

  bool crowded_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->sector_type == SECT_FOREST)
    return FALSE;
    if (battleground(room))
    return FALSE;
    if (room_fight(room, FALSE, FALSE, TRUE))
    return FALSE;
    if (room_level(room) > 0)
    return FALSE;

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
    
      if (victim->recent_moved > -300)
      continue;

      if (victim->in_room == room)
      pop++;
    }

    if (pop >= 8)
    return TRUE;

    return FALSE;
  }

  bool gravesite(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (room->sector_type == SECT_CEMETARY)
    return TRUE;

    return FALSE;
  }

  bool shadowcloaked(ROOM_INDEX_DATA *room) {
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end(); ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;
    
      if (IS_NPC(victim))
      continue;
    
      if (IS_FLAG(victim->comm, COMM_SHADOWCLOAK))
      return TRUE;
    }
    return FALSE;
  }

  bool can_see_outside(ROOM_INDEX_DATA *room) {
    if (!IS_SET(room->room_flags, ROOM_INDOORS))
    return TRUE;

    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *temp_room;
    for (int i = 0; i < 10; i++) {
      if ((pexit = room->exit[i]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE)
      && (temp_room = pexit->u1.to_room) != NULL && (pexit->wall != WALL_GLASS || !IS_SET(pexit->exit_info, EX_CURTAINS))
      && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU))) {
        if (!IS_SET(temp_room->room_flags, ROOM_INDOORS))
        return TRUE;
      }
    }
    return FALSE;
  }

  bool is_neutralized(CHAR_DATA *ch) {
    if (IS_NPC(ch)) {
      return FALSE;
    }

    if (is_gm(ch)) {
      return FALSE;
    }

    if (is_werewolf(ch) && full_moon() == TRUE && (sunphase(NULL) < 2 || sunphase(NULL) == 7)) {
      return FALSE;
    }

    if (ch->fighting == TRUE || ch->attacking > 0) {
      if (has_caff(ch, CAFF_NEUTRALIZED))
      return TRUE;
    }
    else {
      if (IS_AFFECTED(ch, AFF_NEUTRALIZED))
      return TRUE;
    }

    if (ch->pcdata->neutralized == 1) {
      return TRUE;
    }

    return FALSE;
  }

  int moon_phase(void) {
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    return moon_pointer(ptm->tm_mday, ptm->tm_mon, ptm->tm_year, NULL);
  }

  int armor_cap(CHAR_DATA *ch) {
    int cap = 30;
    if (ch == NULL)
    return 30;

    return cap;
  }

  int human_cap(int stat) {
    switch (stat) {
    case SKILL_STRENGTH:
    case SKILL_ACROBATICS:
    case SKILL_FASTREFLEXES:
    case SKILL_DEXTERITY:
    case SKILL_RUNNING:
      return 2;
      break;
    case SKILL_PERCEPTION:
      return 3;
      break;
    case SKILL_STAMINA:
      return 4;
      break;
    case SKILL_DODGING:
    case SKILL_EVADING:
    case SKILL_MARTIALART:
    case SKILL_COMBATTRAINING:
      return 3;
      break;
    }
    return 5;
  }

  int available_exp(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GUEST))
    return ch->newexp;
    int amount = 0;
    if (ch->pcdata->account != NULL)
    amount = ch->pcdata->account->xp;
    else
    amount = ch->exp;
    amount += ch->newexp;
    return amount;
  }
  
  int available_rpexp(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GUEST))
    return ch->newrpexp;
    int amount = 0;
    if (ch->pcdata->account != NULL)
    amount = ch->pcdata->account->rpxp;
    else
    amount = ch->rpexp;

    amount += ch->newrpexp;
    return amount;
  }
  
  int available_karma(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GUEST))
    return 0;
    int amount = 0;
    if (ch->pcdata->account != NULL)
    amount = ch->pcdata->account->karma;
    else
    amount = ch->karma;

    amount += 5000;
    return amount;
  }
  
  int available_pkarma(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_GUEST))
    return 0;

    int amount = 0;
    if (ch->pcdata->account != NULL)
    amount = ch->pcdata->account->pkarma;
    else
    amount = ch->pkarma;

    amount = UMIN(amount, 25000 - ch->spentpkarma);
    amount = UMAX(amount, 0);

    return amount;
  }
  
  int available_donated(CHAR_DATA *ch) {
    if (ch->pcdata->account != NULL)
    return ch->pcdata->account->donated;
    return ch->donated;
  }
  
  int available_karmabank(CHAR_DATA *ch) {
    if (ch->pcdata->account != NULL)
    return ch->pcdata->account->karmabank;
    return 0;
  }

  int get_tier(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return 2;
    if (IS_FLAG(ch->act, PLR_SINSPIRIT))
    return 5;
    int base = 1;

    base = race_table[ch->race].tier;
    if (ch->modifier >= 0 && ch->modifier <= 100)
    base += modifier_table[ch->modifier].amount;
    if (ch->modifier == MODIFIER_TEMPLE && base > 3)
    base--;
    if (ch->pcdata->tier_raised != 0)
    base += ch->pcdata->tier_raised;

    base = UMIN(base, 6);
    base = UMAX(base, 1);
    return base;
  }

  //race_table age reference functions
  int min_age(CHAR_DATA *ch) {
    if (race_table[ch->race].min_age >= 18) {
      return race_table[ch->race].min_age;
    }
    return 18;
  }
  
  int max_age(CHAR_DATA *ch) {
    if (race_table[ch->race].max_age >= 18) {
      return race_table[ch->race].max_age;
    }
    return 18;
  }
  
  int min_aage(CHAR_DATA *ch) {
    if (race_table[ch->race].min_aage >= 18) {
      return race_table[ch->race].min_aage;
    }
    return 18;
  }
  
  int max_aage(CHAR_DATA *ch) {
    if (race_table[ch->race].max_aage >= 18) {
      return race_table[ch->race].max_aage;
    }
    return 18;
  }

  //reformated and condensed this because I like pretty columns and brackets - Disco
  bool elligible_modifier(CHAR_DATA *ch, int mod) {
    if (mod == 0)                        {return TRUE;}
    if (ch->race == RACE_FACULTY)        {return FALSE;}
    if (higher_power(ch))                {return FALSE;}

    if (mod == MODIFIER_ADDICTION) {
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_IMPORTANT)    {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_SFORCES)      {return TRUE;}
      if (ch->race == RACE_CELEBRITY)    {return TRUE;}
      if (ch->race == RACE_PILLAR)       {return TRUE;}
      if (ch->race == RACE_DABBLER)      {return TRUE;}
    }
    else if (mod == MODIFIER_INJURY) {
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_NEWWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_SFORCES)      {return TRUE;}
    }
    else if (mod == MODIFIER_ILLNESS) {
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_IMPORTANT)    {return TRUE;}
      if (ch->race == RACE_NEWWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_SFORCES)      {return TRUE;}
      if (ch->race == RACE_PILLAR)       {return TRUE;}
      if (ch->race == RACE_DABBLER)      {return TRUE;}
    }
    else if (mod == MODIFIER_CRIPPLED) {
      if (ch->race == RACE_VETWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_VETFAEBORN)   {return TRUE;}
      if (ch->race == RACE_VETDEMONBORN) {return TRUE;}
      if (ch->race == RACE_VETANGELBORN) {return TRUE;}
      if (ch->race == RACE_VETDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_VETGIFTED)    {return TRUE;}
      if (ch->race == RACE_OLDWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_OLDFAEBORN)   {return TRUE;}
      if (ch->race == RACE_OLDDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_OLDDEMONBORN) {return TRUE;}
      if (ch->race == RACE_OLDANGELBORN) {return TRUE;}
    }
    else if (mod == MODIFIER_DISFIGURED) {
      if (ch->race == RACE_VETVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_OLDVAMPIRE)   {return TRUE;}
    }
    else if (mod == MODIFIER_CURSED) {
      if (ch->race == RACE_VETVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_VETWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_VETFAEBORN)   {return TRUE;}
      if (ch->race == RACE_VETDEMONBORN) {return TRUE;}
      if (ch->race == RACE_VETANGELBORN) {return TRUE;}
      if (ch->race == RACE_VETDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_VETGIFTED)    {return TRUE;}
    }
    else if (mod == MODIFIER_INSANE) {
      if (ch->race == RACE_NEWVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_NEWWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
    }
    else if (mod == MODIFIER_CATATONIC) {
      if (ch->race == RACE_OLDVAMPIRE)   {return TRUE;}
    }
    else if (mod == MODIFIER_RELIC) {
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_NEWVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_VETVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_VETFAEBORN)   {return TRUE;}
      if (ch->race == RACE_VETDEMONBORN) {return TRUE;}
      if (ch->race == RACE_VETANGELBORN) {return TRUE;}
      if (ch->race == RACE_VETDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_VETGIFTED)    {return TRUE;}
      if (ch->race == RACE_OLDDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_DABBLER)      {return TRUE;}
    }
    else if (mod == MODIFIER_PACT) {
      if (ch->race == RACE_LOCAL)        {return TRUE;}
      if (ch->race == RACE_VISITOR)      {return TRUE;}
      if (ch->race == RACE_STUDENT)      {return TRUE;}
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_BROWN)        {return TRUE;}
      if (ch->race == RACE_IMPORTANT)    {return TRUE;}
      if (ch->race == RACE_DEPUTY)       {return TRUE;}
      if (ch->race == RACE_NEWVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_NEWWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_VETVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_VETWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_VETFAEBORN)   {return TRUE;}
      if (ch->race == RACE_VETDEMONBORN) {return TRUE;}
      if (ch->race == RACE_VETANGELBORN) {return TRUE;}
      if (ch->race == RACE_VETDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_VETGIFTED)    {return TRUE;}
      if (ch->race == RACE_OLDVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_OLDWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_OLDFAEBORN)   {return TRUE;}
      if (ch->race == RACE_OLDDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_OLDDEMONBORN) {return TRUE;}
      if (ch->race == RACE_OLDANGELBORN) {return TRUE;}
      if (ch->race == RACE_DABBLER)      {return TRUE;}
    }
    else if (mod == MODIFIER_CHEMICAL) {
      if (ch->race == RACE_LOCAL)        {return TRUE;}
      if (ch->race == RACE_VISITOR)      {return TRUE;}
      if (ch->race == RACE_STUDENT)      {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_IMPORTANT)    {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_DEPUTY)       {return TRUE;}
    }
    else if (mod == MODIFIER_TEMPLE) {
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_SFORCES)      {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
    }
    else if (mod == MODIFIER_FLESHFORMED) {
      if (ch->race == RACE_LOCAL)        {return TRUE;}
      if (ch->race == RACE_VISITOR)      {return TRUE;}
      if (ch->race == RACE_STUDENT)      {return TRUE;}
      if (ch->race == RACE_SOLDIER)      {return TRUE;}
      if (ch->race == RACE_TIMESWEPT)    {return TRUE;}
      if (ch->race == RACE_WILDLING)     {return TRUE;}
      if (ch->race == RACE_DREAMCHILD)   {return TRUE;}
      if (ch->race == RACE_ELSEBORN)     {return TRUE;}
      if (ch->race == RACE_BROWN)        {return TRUE;}
      if (ch->race == RACE_IMPORTANT)    {return TRUE;}
      if (ch->race == RACE_DEPUTY)       {return TRUE;}
      if (ch->race == RACE_NEWWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_NEWFAEBORN)   {return TRUE;}
      if (ch->race == RACE_NEWDEMONBORN) {return TRUE;}
      if (ch->race == RACE_NEWANGELBORN) {return TRUE;}
      if (ch->race == RACE_NEWDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_GIFTED)       {return TRUE;}
      if (ch->race == RACE_VETWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_VETFAEBORN)   {return TRUE;}
      if (ch->race == RACE_VETDEMONBORN) {return TRUE;}
      if (ch->race == RACE_VETANGELBORN) {return TRUE;}
      if (ch->race == RACE_VETDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_VETGIFTED)    {return TRUE;}
      if (ch->race == RACE_OLDWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_OLDFAEBORN)   {return TRUE;}
      if (ch->race == RACE_OLDDEMIGOD)   {return TRUE;}
      if (ch->race == RACE_OLDDEMONBORN) {return TRUE;}
      if (ch->race == RACE_OLDANGELBORN) {return TRUE;}
      if (ch->race == RACE_DABBLER)      {return TRUE;}
      if (ch->race == RACE_FACULTY)      {return TRUE;}
    }
    else if (mod == MODIFIER_UNLIVING) {
      if (ch->race == RACE_OLDVAMPIRE)   {return TRUE;}
    }
    else if (mod == MODIFIER_LUNACY) {
      if (ch->race == RACE_NEWWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_VETWEREWOLF)  {return TRUE;}
      if (ch->race == RACE_OLDWEREWOLF)  {return TRUE;}
    }
    else if (mod == MODIFIER_DRINKING) {
      if (ch->race == RACE_NEWVAMPIRE)   {return TRUE;}
      if (ch->race == RACE_VETVAMPIRE)   {return TRUE;}
    }

    return FALSE;
  }

  int focuscount(CHAR_DATA *ch, int type) {
    int i, count = 0;
    for (i = 0; i < SKILL_USED; i++) {
      if (skilltype(skill_table[i].vnum) != type)
      continue;
      if (ch->skills[skill_table[i].vnum] == 0)
      continue;
      count += ch->skills[skill_table[i].vnum];
      count += regress_mod(ch, skill_table[i].vnum);
    }
    return count;
  }

  int npc_cost(CHAR_DATA *ch) {
    int shield_total = 0;
    int cost = 0;

    for (int i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range == -1)
      shield_total += ch->disciplines[discipline_table[i].vnum];
      else
      cost += built_disc_cost(ch->disciplines[discipline_table[i].vnum]);
    }
    cost += built_disc_cost(shield_total);
    return cost;
  }

  int combat_focus(CHAR_DATA *ch) {
    int amount;
    if (IS_NPC(ch)) {
      amount = npc_cost(ch) / 200000;
      return amount;
    }
    amount = focuscount(ch, STYPE_COMBATFOCUS);
    return amount;
  }
  int arcane_focus(CHAR_DATA *ch) {
    int amount = focuscount(ch, STYPE_ARCANEFOCUS);
    return amount;
  }
  int prof_focus(CHAR_DATA *ch) {
    int amount = focuscount(ch, STYPE_PROFFOCUS);
    if (ch->race == RACE_BROWN)
    amount++;
    return amount;
  }

  bool nowindows(ROOM_INDEX_DATA *room) {
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *temp_room;
    for (int i = 0; i < 10; i++) {
      if ((pexit = room->exit[i]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU))) {
        if (!IS_SET(temp_room->room_flags, ROOM_INDOORS))
        return FALSE;
      }
    }
    return TRUE;
  }

  int shade_level(ROOM_INDEX_DATA *room) {
    int shade = 0;
    if (room == NULL)
    return 0;
    if (IS_SET(room->room_flags, ROOM_INDOORS)) {
      shade += 2;
      if (nowindows(room))
      shade++;
    }
    
    if (room->sector_type == SECT_FOREST)
    shade++;
    if (room->sector_type == SECT_UNDERWATER)
    shade += 2;
    if (mist_level(room) == 3)
    shade++;
    else if (mist_level(room) >= 2 && sunphase(room) != 4)
    shade++;
    if (cloud_density(room) >= 50 && cloud_cover(room) >= 75)
    shade++;

    shade = UMIN(3, shade);
    return shade;
  }

  int sunlight_debuff(CHAR_DATA *ch) {
    int sun = sunphase(ch->in_room);
    int debuff = 0;
    if (sun == 4)
    debuff = 60;
    else if (sun == 3 || sun == 5)
    debuff = 32;
    else if (sun == 2 || sun == 6)
    debuff = 22;

    if (get_skill(ch, SKILL_SUNRESIST) > 0)
    debuff /= 2;

    int reduc = shade_level(ch->in_room) * 10;
    reduc += cover_count(ch);
    reduc = UMIN(reduc, 30);

    debuff -= reduc;
    return UMAX(debuff, 0);
  }

  bool has_relic(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *objcont;
    OBJ_DATA *objcont_next;

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
      continue;

      if (IS_SET(obj->extra_flags, ITEM_RELIC) && !str_cmp(ch->name, obj->material))
      return TRUE;

      if (obj->item_type == ITEM_CONTAINER) {

        for (objcont = obj->contains; objcont != NULL; objcont = objcont_next) {
          objcont_next = objcont->next_content;

          if (IS_SET(objcont->extra_flags, ITEM_RELIC) && !str_cmp(ch->name, objcont->material))
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  int monster_lf_mod(CHAR_DATA *ch, CHAR_DATA *view) {
    int val = 100;
    if (is_vampire(ch) && ch->in_room != NULL) {
      val = val * (100 - sunlight_debuff(ch)) / 100;
      if (view != NULL)
      printf_to_char(view, "Vampire sunlight: -%d ", sunlight_debuff(ch));

      if (ch->modifier == MODIFIER_DRINKING
      && ch->pcdata->last_drained_person + (3600 * 24 * 60) < current_time && ch->played / 3600 > 100) {
        if (view != NULL)
        printf_to_char(view, "Exang debuff: -%d ", val * 2 / 10);
        val = val * 8 / 10;
      }
    }
    if (is_werewolf(ch)) {
      if (new_moon()) {
        if (view != NULL)
        printf_to_char(view, "Newmoon: -%d ", val * 1 / 5);
        val = val * 4 / 5;
      }

      if (full_moon()) {
        if (ch->pcdata->habit[HABIT_LUNACY] == 1) {
          if (view != NULL)
          printf_to_char(view, "Chained wolf: -%d ", val * 1 / 5);
          val = val * 4 / 5;
        }
        if (sunphase(NULL) < 2 || sunphase(NULL) == 7) {
          if (ch->pcdata->habit[HABIT_LUNACY] == 2) {
            if (view != NULL)
            printf_to_char(view, "Lunacy: %d ", val * 1 / 10);
            val = val * 11 / 10;
          }
          if (ch->pcdata->habit[HABIT_LUNACY] == 3) {
            if (view != NULL)
            printf_to_char(view, "Lunacy: %d ", val * 3 / 10);
            val = val * 13 / 10;
          }
        }
      }
      if (ch->modifier == MODIFIER_LUNACY
      && ch->pcdata->last_outside_fullmoon < current_time - (3600 * 24 * 60) && ch->played / 3600 > 100) {
        if (view != NULL)
        printf_to_char(view, "No Lunacy: -%d ", val * 2 / 10);
        val = val * 8 / 10;
      }
    }
    val = UMAX(val, 20);
    return val;
  }


  int clan_lifeforce_mod(CHAR_DATA *ch, CHAR_DATA *view, FACTION_TYPE *fac)
  {
    int val = 0;
    int bonus = 0;
    if(fac == NULL)
    return 0;

    if (!str_cmp(ch->name, fac->leader)) {
      if (fac->axes[AXES_SUPERNATURAL] == AXES_FARLEFT && !seems_super(ch) && event_cleanse == 0)
      val -= 10;
      if (fac->axes[AXES_SUPERNATURAL] == AXES_FARRIGHT && seems_super(ch) && event_cleanse == 0)
      val -= 10;
      if (fac->axes[AXES_CORRUPT] <= AXES_FARLEFT && get_tier(ch) <= 2 && fac->axes[AXES_CORRUPT] > 0)
      val -= 10;
      if (fac->axes[AXES_CORRUPT] <= AXES_NEARLEFT && get_tier(ch) <= 1 && fac->axes[AXES_CORRUPT] > 0)
      val -= 10;

      if (!trust_elligible(ch, fac, FALSE, NULL))
      val -= 5;

      if (college_student(ch, TRUE) && fac->college == 0)
      val = -5;
      if(view != NULL)
      printf_to_char(view, "Leader effect: %d\n\r", val);
    }

    if (fac->axes[AXES_SUPERNATURAL] == AXES_FARLEFT) {
      if (seems_super(ch))
      bonus += 4;
      else
      bonus -= 4;
    }
    if (fac->axes[AXES_SUPERNATURAL] == AXES_MIDLEFT) {
      if (seems_super(ch))
      bonus += 3;
      else
      bonus -= 3;
    }
    if (fac->axes[AXES_SUPERNATURAL] == AXES_NEARLEFT) {
      if (seems_super(ch))
      bonus += 1;
      else
      bonus -= 1;
    }
    if (fac->axes[AXES_SUPERNATURAL] == AXES_FARRIGHT) {
      if (seems_super(ch))
      bonus -= 4;
      else
      bonus += 4;
    }
    if (fac->axes[AXES_SUPERNATURAL] == AXES_MIDRIGHT) {
      if (seems_super(ch))
      bonus -= 3;
      else
      bonus += 3;
    }
    if (fac->axes[AXES_SUPERNATURAL] == AXES_NEARRIGHT) {
      if (seems_super(ch))
      bonus -= 1;
      else
      bonus += 1;
    }

    if (view != NULL && bonus != 0)
    printf_to_char(view, "Supernatural Position: %d ", bonus);

    if (fac->axes[AXES_CORRUPT] == AXES_FARLEFT) {
      if (get_tier(ch) >= 3)
      bonus += 6;
      else if (get_tier(ch) == 4)
      bonus += 4;
      else if (get_tier(ch) == 3)
      bonus += 3;
      else if (get_tier(ch) == 2 && fac->type != FACTION_CULT)
      bonus -= 3;
      else if(fac->type != FACTION_CULT)
      bonus -= 4;
    }
    if (fac->axes[AXES_CORRUPT] == AXES_MIDLEFT) {
      if (get_tier(ch) >= 3)
      bonus += 3;
      else if (get_tier(ch) == 4)
      bonus += 2;
      else if (get_tier(ch) == 3)
      bonus += 2;
      else if (get_tier(ch) == 2 && fac->type != FACTION_CULT)
      bonus -= 1;
      else if(fac->type != FACTION_CULT)
      bonus -= 2;
    }
    if (fac->axes[AXES_CORRUPT] == AXES_NEUTRAL) {
      if (get_tier(ch) == 1 || get_tier(ch) == 3)
      bonus -= 3;
      else if (get_tier(ch) == 4)
      bonus -= 9;
      else if (get_tier(ch) == 5)
      bonus -= 12;
    }
    if (fac->axes[AXES_CORRUPT] == AXES_NEARLEFT) {
      if (get_tier(ch) == 5)
      bonus += 1;
      else if (get_tier(ch) == 4)
      bonus += 1;
      else if (get_tier(ch) == 3)
      bonus += 1;
      else if (get_tier(ch) == 2)
      bonus -= 0;
      else
      bonus -= 1;
    }
    if (fac->axes[AXES_CORRUPT] == AXES_FARRIGHT) {
      if (get_tier(ch) < 3)
      bonus += 10;
      else if (get_tier(ch) > 2)
      bonus -= 20;
    }
    if (fac->axes[AXES_CORRUPT] == AXES_MIDRIGHT) {
      if (get_tier(ch) < 3)
      bonus += 6;

      else if (get_tier(ch) > 3)
      bonus -= 20;
      else if (get_tier(ch) > 2)
      bonus -= 12;
    }
    if (fac->axes[AXES_CORRUPT] == AXES_NEARRIGHT) {
      if (get_tier(ch) < 3)
      bonus += 3;
      else if (get_tier(ch) > 3)
      bonus -= 10;
      else if (get_tier(ch) > 2)
      bonus -= 8;
    }

    if (view != NULL && bonus != 0)
    printf_to_char(view, "Corrupt Position: %d ", bonus);

    int rval = val + bonus;
    return rval;
  }

  int soc_lf_mod(CHAR_DATA *ch, CHAR_DATA *view)
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
      vcult = clan_lifeforce_mod(ch, view, cult);
    }
    
    if(sect != NULL)
    {
      count++;
      vsect = clan_lifeforce_mod(ch, view, sect);
    }
    
    if(core != NULL)
    {
      count++;
      vfac = clan_lifeforce_mod(ch, view, core);
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
    if(count == 3)
    rval += 3;
    if(count == 2)
    rval += 1;
    return rval;
  }




  int lifeforce_mod(CHAR_DATA *ch, CHAR_DATA *view) {
    int val = 100;
    if (ch->modifier == MODIFIER_TEMPLE && no_tech(ch)) {
      val -= 10;
      if (view != NULL)
      printf_to_char(view, "No tech: %d ", -10);
    }

    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (current_time < (*it)->active_time || current_time > (*it)->deactive_time)
      continue;

      if ((*it)->type == EVENT_DREAMBELIEF) {
        if (safe_strlen(dream_detail(ch, ch->pcdata->identity_world, DREAM_DETAIL_SHORT)) < 1) {
          val -= 15;
          if (view != NULL)
          send_to_char("-15% dream belief.\n\r", view);
        }
      }
    }
    int weather = weather_bonus(ch);
    if (weather != 0) {
      val += weather;
      if (view != NULL)
      printf_to_char(view, "Weather Spec: %d\n\r", weather);
    }

    if (!IS_FLAG(ch->act, PLR_GUEST)) //exempting institute PCs and guests -- Disco
    {
      if (is_super(ch) && !under_understanding(ch, ch) && !under_limited(ch, ch)
      && (ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->resource < 8000)
      && str_cmp(ch->name, "Ritualist")) {
        if (ch->in_room != NULL && in_haven(ch->in_room)
        && (!clinic_staff(ch, FALSE) && !college_staff(ch, FALSE))) {
          val -= 15;
          if (view != NULL) {
            printf_to_char(view, "No Sanc: %d ", -15);
          }
        }
      }

      if (get_tier(ch) == 2 && ch->faction == 0 && ch->vassal == 0 && !has_weakness(NULL, ch)) {
        val -= 2;
        if (view != NULL)
        printf_to_char(view, "Unvassaled: %d ", -2);
      }
      else if (get_tier(ch) >= 3 && ch->faction == 0 && ch->vassal == 0 && !has_weakness(NULL, ch)) {
        val -= 4;
        if (view != NULL)
        printf_to_char(view, "Unvassaled: %d ", -4);
      }
      else if (ch->faction == 0 && ch->vassal != 0 && !has_weakness(NULL, ch)) {
        val += 5;
        if (view != NULL)
        printf_to_char(view, "Vassaled: %d ", 5);
      }
    }
    
    if (under_opression(ch)) {
      val -= 15;
      if (view != NULL)
      printf_to_char(view, "Oppressed: %d ", -15);
    }

    int tbonus = soc_lf_mod(ch, view);
    
    if(tbonus != 0 && view != NULL)
    printf_to_char(view, "Society: %d ", tbonus);

    val += tbonus;

    if (IS_AFFECTED(ch, AFF_GRIEF)) {
      if (get_true_age(ch) >= 100) {
        val -= 25;
        if (view != NULL)
        printf_to_char(view, "Grief: %d ", -25);
      }
      else {
        val -= 50;
        if (view != NULL)
        printf_to_char(view, "Grief: %d ", -50);
      }
    }
    
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0)
      continue;
      for (int i = 0; i < 20; i++) {
        if (!str_cmp((*it)->kidnap_name[i], ch->name)) {
          if (get_true_age(ch) >= 100) {
            val -= 5;
            if (view != NULL)
            printf_to_char(view, "Kidnapping: %d ", -5);
          }
          else {
            val -= 10;
            if (view != NULL)
            printf_to_char(view, "Kidnapping: %d ", -10);
          }
        }
      }
    }

    if (ch->played / 3600 > 50 && safe_strlen(ch->pcdata->home_territory) < 2 && ch->race != RACE_LOCAL && ch->race != RACE_IMPORTANT && ch->race != RACE_CELEBRITY && ch->race != RACE_PILLAR && ch->race != RACE_FACULTY) {
      val -= 10;
      if (view != NULL)
      printf_to_char(view, "No Territory: %d ", -10);
    }

    if (!IS_NPC(ch) && ch->pcdata->overworked > 0) {
      val -= 25;
      if (view != NULL)
      printf_to_char(view, "Overworked: %d ", -25);
    }

    if (!IS_NPC(ch)) {
      int weak = 0;
      if (ch->pcdata->weakness_status == WEAKNESS_CRITICAL)
      weak -= 30;
      else if (ch->pcdata->weakness_status == WEAKNESS_SEVERE)
      weak -= 20;
      else if (ch->pcdata->weakness_status == WEAKNESS_PRISONER)
      weak -= 20;
      else if (ch->pcdata->weakness_status == WEAKNESS_COMMITTED)
      weak -= 15;
      else if (ch->pcdata->weakness_status == WEAKNESS_DREAMSNARED)
      weak -= 10;
      else if (ch->pcdata->weakness_status == WEAKNESS_MILD)
      weak -= 10;
      val += weak;
      if (view != NULL && weak != 0)
      printf_to_char(view, "Weakness: %d ", weak);
    }

    if (get_age(ch) > 60) {
      if (view != NULL)
      printf_to_char(view, "Oldage: %d ", UMIN(20, (get_age(ch) - 60)));

      val -= UMIN(20, (get_age(ch) - 60));
    }
    val = UMAX(30, val);

    if (!is_undead(ch)) {
      if (is_ill(ch)) {
        val -= 5;
        if (view != NULL)
        printf_to_char(view, "Illness: %d ", -5);
      }
    }
    
    if (ch->wounds == 3) {
      if (view != NULL)
      printf_to_char(view, "Critical wound: -%d ", val / 2);
      val /= 2;
    }
    else if (ch->wounds == 2) {
      if (view != NULL)
      printf_to_char(view, "Severe wound: -%d ", val * 2 / 10);
      val = val * 8 / 10;
    }
    else if (ch->wounds == 1) {
      if (view != NULL)
      printf_to_char(view, "Mild wound: -%d ", val * 5 / 100);
      val = val * 95 / 100;
    }

    return val;
  }

  int get_lifeforce(CHAR_DATA *ch, bool used, CHAR_DATA *view) {
    if (ch->in_room != NULL && ch->in_room->vnum < 300 && ch->in_room->vnum > 1) {
      ch->lifeforce = 10000;
      ch->lf_taken = 0;
      ch->lf_used = 0;
      return 100;
    }
    if (higher_power(ch))
    return 100;

    if (guestmonster(ch))
    return (base_lifeforce(ch) / 100 + 100) / 2;

    if (is_gm(ch) && !IS_IMMORTAL(ch))
    return 100;

    if (ch->lifeforce > 30000 || ch->lifeforce < -1000)
    ch->lifeforce = 9000;

    if (ch->lifeforce < 1000)
    ch->lifeforce = 1000;

    if (!IS_NPC(ch) && base_lifeforce(ch) < 3000) {
      if (ch->pcdata->sleeping == 0) {
        act("$n slumps sideways to the ground.", ch, NULL, NULL, TO_ROOM);
        act("You pass out.", ch, NULL, NULL, TO_CHAR);
        ch->pcdata->coma = current_time + (3600 * number_range(1, 5));
        ch->pcdata->sleeping = 10;
      }
      else
      ch->pcdata->sleeping = 10;
    }

    if (IS_FLAG(ch->act, PLR_SINSPIRIT))
    return 120;

    if (IS_NPC(ch)) {
      if (ch->in_room != NULL && battleground(ch->in_room))
      return ch->lifeforce / 100;
      if (ch->pIndexData->vnum == MINION_TEMPLATE)
      return ch->lifeforce / 100;
      if (ch->pIndexData->vnum == ALLY_TEMPLATE)
      return ch->lifeforce / 100;

      return 100;
    }
    int val = base_lifeforce(ch);

    if (view != NULL)
    printf_to_char(view, "Base: %d ", val);

    if (!used)
    val += UMIN(2000, ch->lf_used);

    val = val * lifeforce_mod(ch, view) / 100;
    val /= 100;

    if (ch->pcdata->deluded_duration > current_time)
    val -= ch->pcdata->deluded_cost;

    if (IS_FLAG(ch->act, PLR_VILLAIN))
    val += 25;
    else if (cardinal(ch) && sincount(ch) > 0)
    val += 25;
    else if (cardinal(ch))
    val -= 30;

    if(ch->pcdata->fixation_mourning > current_time)
    val -= 20;

    if (ch->pcdata->dream_sex > 2) {
      val -= ch->pcdata->dream_sex;
      if (view != NULL)
      printf_to_char(view, "Dream sex: -%d ", ch->pcdata->dream_sex);
    }

    if (is_vampire(ch)) {
      int hour = get_hour(ch->in_room);
      int bonus = 0;
      if (hour == 5 || hour == 19)
      bonus += 1;
      else if (hour == 4 || hour == 20)
      bonus += 1;
      else if (hour == 3 || hour == 21)
      bonus += 2;
      else if (hour == 2 || hour == 22)
      bonus += 2;
      else if (hour == 1 || hour == 23)
      bonus += 2;
      else if (hour == 0 || hour == 24)
      bonus += 3;
      val += bonus;
      if (view != NULL && bonus != 0)
      printf_to_char(view, "Vampire Nighttime: %d ", bonus);

      if (IS_AFFECTED(ch, AFF_NEUTRALIZED)) {
        val -= 25;
        if (view != NULL)
        printf_to_char(view, "Neutralized: -%d ", 25);
      }
    }

    val += ch->pcdata->lf_modifier;
    if (view != NULL)
    printf_to_char(view, "Lf_modifier: %d ", ch->pcdata->lf_modifier);

    if (is_vampire(ch) || is_werewolf(ch))
    val = val * monster_lf_mod(ch, view) / 100;

    if (pact_holder(ch) && ch->pcdata->emotes[EMOTE_PACTANTAG] < 10 && ch->played / 3600 > 100) {
      if (view != NULL)
      printf_to_char(view, "Pact debuff: %d ", val * 2 / 10);
      val = val * 8 / 10;
    }
    if (ch->modifier == MODIFIER_RELIC && !has_relic(ch)) {
      if (view != NULL)
      printf_to_char(view, "Relic debuff: %d ", val * 2 / 10);
      val = val * 8 / 10;
    }
    if (!IS_NPC(ch) && safe_strlen(ch->pcdata->maim) > 3) {
      if (view != NULL)
      printf_to_char(view, "Maim: %d ", val * 1 / 10);
      val = val * 90 / 100;
    }

    int sun = sunphase(ch->in_room);

    if (IS_AFFECTED(ch, AFF_SUNBLESS) && sun >= 2 && sun <= 6) {
      if (view != NULL)
      printf_to_char(view, "Sunbless: %d ", val * 15 / 100);
      val = val * 115 / 100;
    }
    if (IS_AFFECTED(ch, AFF_MOONBLESS) && (sun < 2 || sun > 6)) {
      if (view != NULL)
      printf_to_char(view, "Moonbless: %d ", val * 15 / 100);
      val = val * 115 / 100;
    }
    if (IS_AFFECTED(ch, AFF_WATERBLESS) && nearby_water(ch)) {
      if (view != NULL)
      printf_to_char(view, "Waterbless: %d ", val * 15 / 100);
      val = val * 115 / 100;
    }
    if (safe_strlen(ch->pcdata->maintained_target) > 1 && str_cmp(ch->pcdata->maintained_target, "casting")) {
      if (view != NULL)
      printf_to_char(view, "Maintained ritual: -%d ", ch->pcdata->maintain_cost);
      val -= ch->pcdata->maintain_cost;
    }
    if (ch->pcdata->ill_time > current_time) {
      if (view != NULL)
      printf_to_char(view, "Ill: %d ", 80);
      val = UMIN(val, 80);
    }

    if(in_cult_domain(ch))
    {
      val = UMAX(val, 80);
      val = val*120/100;
      if (view != NULL)
      printf_to_char(view, "Sanctified domain: %d ", val * 20 / 100);
    }

    val = UMAX(val, 10);
    return val;
  }

  int get_display_lifeforce(CHAR_DATA *ch) {
    int val = get_lifeforce(ch, TRUE, NULL);
    if (IS_AFFECTED(ch, AFF_BITTEN) && ch->pcdata->bittenloss > 0)
    val = UMIN(110, val + ch->pcdata->bittenloss / 100);
    return val;
  }

  int get_display_lfmod(CHAR_DATA *ch) {
    int base = get_lifeforce(ch, TRUE, NULL);
    int mod = base - base_lifeforce(ch) / 100;
    return mod;
  }

  void lfstat(CHAR_DATA *ch, CHAR_DATA *view) {
    if(ch == NULL || IS_NPC(ch))
    return;

    printf_to_char(view, "Used: %d, Taken: %d, SUsed: %d\n\r", ch->lf_used, ch->lf_taken, ch->lf_sused);
    printf_to_char(view, "Result: %d\n\r", get_lifeforce(ch, TRUE, view));
    return;

    if (ch == NULL || view == NULL)
    return;
    if (is_vampire(ch) && ch->in_room != NULL) {
      printf_to_char(view, "Sunlight: -%d%%\n\r", sunlight_debuff(ch));
      if (ch->modifier == MODIFIER_DRINKING && ch->pcdata->last_drained_person + (3600 * 24 * 60) < current_time && ch->played / 3600 > 100)
      send_to_char("Exsanguinating: -20%\n\r", view);
    }
    if (is_werewolf(ch)) {
      if (new_moon() && get_world(ch) == WORLD_EARTH)
      send_to_char("New Moon: -20%\n\r", view);

      if (full_moon()) {
        if (ch->pcdata->habit[HABIT_LUNACY] == 1)
        send_to_char("Chemical restraint: -20%\n\r", view);
        if (sunphase(NULL) < 2 || sunphase(NULL) == 7) {
          if (ch->pcdata->habit[HABIT_LUNACY] == 2)
          send_to_char("Lunacy: +10%\n\r", view);
          if (ch->pcdata->habit[HABIT_LUNACY] == 3)
          send_to_char("Lunacy: +30%\n\r", view);
        }
      }
      if (ch->modifier == MODIFIER_LUNACY && ch->pcdata->last_outside_fullmoon < current_time - (3600 * 24 * 60) && ch->played / 3600 > 100)
      send_to_char("Too long since Lunacy: -20%\n\r", view);
    }
    if (ch->modifier == MODIFIER_TEMPLE && no_tech(ch))
    send_to_char("Temple Augmented, No Tech Curse: -10%\n\r", view);

    if (!IS_FLAG(ch->act, PLR_GUEST)) {
      if (is_super(ch) && !under_understanding(ch, ch) && (ch->faction == 0 || clan_lookup(ch->faction) == NULL || clan_lookup(ch->faction)->resource < 10000) && str_cmp(ch->name, "Ritualist"))
      send_to_char("Unprotected: -15%\n\r", view);

      if (get_tier(ch) == 2 && ch->faction == 0 && ch->vassal == 0)
      send_to_char("Unvassaled: -3%\n\r", view);
      else if (get_tier(ch) == 3 && ch->faction == 0 && ch->vassal == 0)
      send_to_char("Unvassaled: -10%\n\r", view);
    }
    if (under_opression(ch))
    send_to_char("Oppression: -15%\n\r", view);

    if (is_super(ch) && get_tier(ch) >= 3) {
      FACTION_TYPE *fac;
      if (ch->faction != 0 && (fac = clan_lookup(ch->faction)) != NULL) {
        int leadertier = 0;
        int membertier = 0;
        bool memberfive = FALSE;
        bool onehigh = FALSE;
        for (int i = 0; i < 100 && fac->member_tier[i] > 0; i++) {
          if (!str_cmp(fac->member_names[i], fac->leader))
          leadertier = fac->member_tier[i];
          else {
            if (fac->member_tier[i] > 2) {
              if (onehigh == FALSE && fac->member_tier[i] > 3) {
                membertier += (fac->member_tier[i] - 3);
                onehigh = TRUE;
              }
              else
              membertier += (fac->member_tier[i] - 2);
            }
            if (fac->member_tier[i] == 5)
            memberfive = TRUE;
          }
        }
        
        if (memberfive == TRUE && leadertier == 5)
        send_to_char("Two 5s: -5%\n\r", view);
      
        if (membertier > 3) {
          membertier -= 3;
          printf_to_char(view, "Membertiers: -%d\n\r", 5 * membertier);
        }
      }
      if (ch->factiontwo != 0 && (fac = clan_lookup(ch->factiontwo)) != NULL) {
        int leadertier = 0;
        int membertier = 0;
        bool memberfive = FALSE;
        bool onehigh = FALSE;
        for (int i = 0; i < 100 && fac->member_tier[i] > 0; i++) {
          if (!str_cmp(fac->member_names[i], fac->leader))
          leadertier = fac->member_tier[i];
          else {
            if (fac->member_tier[i] > 2) {
              if (onehigh == FALSE && fac->member_tier[i] > 3) {
                membertier += (fac->member_tier[i] - 3);
                onehigh = TRUE;
              }
              else
              membertier += (fac->member_tier[i] - 2);
            }
            if (fac->member_tier[i] == 5)
            memberfive = TRUE;
          }
        }
        if (memberfive == TRUE && leadertier == 5)
        send_to_char("Two 5s: -5%\n\r", view);
      
        if (membertier > 3) {
          membertier -= 3;
          printf_to_char(view, "Membertiers: -%d\n\r", 5 * membertier);
        }
      }
    }

    if (IS_AFFECTED(ch, AFF_GRIEF)) {
      if (get_true_age(ch) >= 100)
      send_to_char("Grief: -25%\n\r", view);
      else
      send_to_char("Grief: -50%\n\r", view);
    }
    
    for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
    it != FacVect.end(); ++it) {
      if ((*it)->vnum == 0)
      continue;
    
      for (int i = 0; i < 20; i++) {
        if (!str_cmp((*it)->kidnap_name[i], ch->name)) {
          if (get_true_age(ch) >= 100)
          send_to_char("Kidnap: -5%\n\r", view);
          else
          send_to_char("Kidnap: -10%\n\r", view);
        }
      }
    }

    if (ch->played / 3600 > 50 && safe_strlen(ch->pcdata->home_territory) < 2 && ch->race != RACE_LOCAL && ch->race != RACE_IMPORTANT && ch->race != RACE_CELEBRITY && ch->race != RACE_PILLAR && ch->race != RACE_FACULTY)
    send_to_char("No territory: -10%\n\r", view);

    if (get_age(ch) > 60) {
      printf_to_char(view, "Old Body: -%d%%\n\r", (get_age(ch) - 60));
    }

    if (!is_undead(ch)) {
      if (is_ill(ch))
      send_to_char("Illness: -5%\n\r", view);
    }
    
    if (ch->wounds == 3)
    send_to_char("Critical Wound: -50%\n\r", view);
    else if (ch->wounds == 2)
    send_to_char("Severe Wound: -20%\n\r", view);
    else if (ch->wounds == 1)
    send_to_char("Mild Wound: -5%\n\r", view);

    if (ch->pcdata->dream_sex > 2)
    printf_to_char(view, "Dream Sex: -%d%%\n\r", ch->pcdata->dream_sex);

    if (ch->pcdata->lf_modifier != 0)
    printf_to_char(view, "Wellness Modifier: %d%%\n\r", ch->pcdata->lf_modifier);

    if (pact_holder(ch) && ch->pcdata->emotes[EMOTE_PACTANTAG] < 10 && ch->played / 3600 > 100)
    send_to_char("Unhappy Demon: -20%\n\r", view);
  
    if (ch->modifier == MODIFIER_RELIC && !has_relic(ch))
    send_to_char("No Relic: -20%\n\r", view);

    if (!IS_NPC(ch) && safe_strlen(ch->pcdata->maim) > 3)
    send_to_char("Maimed: -10%n\r", view);

    int sun = sunphase(ch->in_room);

    if (IS_AFFECTED(ch, AFF_SUNBLESS) && sun >= 2 && sun <= 6)
    send_to_char("Sunbless: +20%\n\r", view);
    if (IS_AFFECTED(ch, AFF_MOONBLESS) && (sun < 2 || sun > 6))
    send_to_char("Moonbless: +20%\n\r", view);
    if (IS_AFFECTED(ch, AFF_WATERBLESS) && nearby_water(ch))
    send_to_char("Waterbless: +10%\n\r", view);
  }

  int in_world(CHAR_DATA *ch) {
    if (ch == NULL)
    return WORLD_ELSEWHERE;
    ROOM_INDEX_DATA *room;

    if (!IS_NPC(ch)) {
      if (ch->pcdata->travel_time > -1 && ch->pcdata->travel_to != -1 && ch->pcdata->travel_type != -1) {
        room = get_room_index(ch->pcdata->travel_to);
        if (room != NULL)
        return room->area->world;
      }
    }
    if (battleground(ch->in_room))
    return battle_world();
    if (ch->in_room != NULL)
    return ch->in_room->area->world;

    return WORLD_EARTH;
  }
  
  int get_world(CHAR_DATA *ch) { return in_world(ch); }
  
  bool no_tech(CHAR_DATA *ch) {
    if (has_caff(ch, CAFF_OVERLOAD))
    return TRUE;

    if (IS_AFFECTED(ch, AFF_NOTECH))
    return TRUE;

    return FALSE;
  }
  
  bool cell_signal(CHAR_DATA *ch) {
    if (ch == NULL || ch->in_room == NULL)
    return FALSE;

    if(crisis_prologue == 1)
    return FALSE;
    if (higher_power(ch))
    return TRUE;
    PROP_TYPE *prop;
    prop = room_prop(ch->in_room);

    if (IS_NPC(ch))
    return FALSE;

    if (ch->in_room == NULL)
    return FALSE;

    if (get_world(ch) != WORLD_EARTH) {
      if (prop_from_room(ch->in_room) == NULL)
      return FALSE;
      if (prop_from_room(ch->in_room)->utilities == 0)
      return FALSE;
      if (ch->in_room->encroachment >= 600)
      return FALSE;
    }
    if (no_tech(ch))
    return FALSE;

    if (silenced(ch))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_SILENCED))
    return FALSE;

    if (is_ghost(ch))
    return FALSE;

    if (ch->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && ch->pcdata->patrol_status <= PATROL_DEFENDHIDING)
    return FALSE;

    if (ch->pcdata->patrol_status >= PATROL_PREY && ch->pcdata->patrol_status <= PATROL_GRABBED)
    return FALSE;

    if (ch->in_room->area->vnum == OUTER_NORTH_FOREST || ch->in_room->area->vnum == OUTER_SOUTH_FOREST || ch->in_room->area->vnum == OUTER_WEST_FOREST) {
      if (ch->in_room->sector_type != SECT_STREET) {
        if (prop_from_room(ch->in_room) == NULL)
        return FALSE;
        if (prop_from_room(ch->in_room)->utilities == 0)
        return FALSE;
      }
      if (ch->in_room->encroachment >= 600)
      return FALSE;
    }

    if (get_skill(ch, SKILL_HACKING) < 3) {
      if (clinic_patient(ch)) {
        INSTITUTE_TYPE *ins;
        for (vector<INSTITUTE_TYPE *>::iterator it = InVect.begin();
        it != InVect.end(); ++it) {
          ins = (*it);
          if (!str_cmp(ch->name, ins->name)) {
            if (ins->nophone == 1)
            return FALSE;
          }
        }
      }
    }

    return TRUE;
  }

  bool radio_signal(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return FALSE;

    if (get_world(ch) != WORLD_EARTH) {
      if (prop_from_room(ch->in_room) == NULL)
      return FALSE;
      if (prop_from_room(ch->in_room)->utilities == 0)
      return FALSE;
      if (ch->in_room->encroachment >= 600)
      return FALSE;
    }

    if (no_tech(ch))
    return FALSE;

    if (silenced(ch))
    return FALSE;

    if (IS_AFFECTED(ch, AFF_SILENCED))
    return FALSE;

    if (ch->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && ch->pcdata->patrol_status <= PATROL_DEFENDHIDING)
    return FALSE;

    if (ch->pcdata->patrol_status >= PATROL_PREY && ch->pcdata->patrol_status <= PATROL_GRABBED)
    return FALSE;

    return TRUE;
  }

  int custom_vnum(CHAR_DATA *ch) {
    int val = 0;
    val = UMAX(val, ch->disciplines[DIS_PISTOLS]);
    val = UMAX(val, ch->disciplines[DIS_THROWN]);
    val = UMAX(val, ch->disciplines[DIS_CARBINES]);
    val = UMAX(val, ch->disciplines[DIS_RIFLES]);
    val = UMAX(val, ch->disciplines[DIS_BOWS]);
    val = UMAX(val, ch->disciplines[DIS_SHOTGUNS]);
    val = UMAX(val, ch->disciplines[DIS_SPEARGUN]);

    if (val <= ch->disciplines[DIS_PISTOLS])
    return DIS_PISTOLS;
    if (val <= ch->disciplines[DIS_THROWN])
    return DIS_THROWN;
    if (val <= ch->disciplines[DIS_CARBINES])
    return DIS_CARBINES;
    if (val <= ch->disciplines[DIS_RIFLES])
    return DIS_RIFLES;
    if (val <= ch->disciplines[DIS_BOWS])
    return DIS_BOWS;
    if (val <= ch->disciplines[DIS_SHOTGUNS])
    return DIS_SHOTGUNS;
    if (val <= ch->disciplines[DIS_SPEARGUN])
    return DIS_SPEARGUN;
    return DIS_BOWS;
  }

  bool guestmonster(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_MONSTER)
    return TRUE;
    return FALSE;
  }

  bool cardinal(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (IS_FLAG(ch->act, PLR_GUEST) && ch->pcdata->guest_type == GUEST_CARDINAL)
    return TRUE;
    return FALSE;
  }
  
  int sincount(CHAR_DATA *ch) {
    int count = 0;
    for (int i = 0; i < 7; i++) {
      if (ch->pcdata->sins[i] == 1)
      count++;
    }
    return count;
  }
  
  bool sinmatch(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;
    for (int i = 0; i < 7; i++) {
      if (ch->pcdata->sins[i] == 1 && victim->pcdata->sins[i] == 1)
      return TRUE;
    }
    return FALSE;
  }

  bool clique_is_rebel(int clique_role) {

    if (clique_role == CLIQUEROLE_BULLY)
    return TRUE;
    if (clique_role == CLIQUEROLE_DRUGGIE)
    return TRUE;

    return FALSE;
  }

  bool clique_is_nerd(int clique_role) {

    if (clique_role == CLIQUEROLE_ARTIST)
    return TRUE;
    if (clique_role == CLIQUEROLE_HACKER)
    return TRUE;
    if (clique_role == CLIQUEROLE_OVERACHIEVER)
    return TRUE;
    if (clique_role == CLIQUEROLE_GOTH)
    return TRUE;

    return FALSE;
  }

  bool clique_is_popular(int clique_role) {

    if (clique_role == CLIQUEROLE_PREP)
    return TRUE;
    if (clique_role == CLIQUEROLE_JOCK)
    return TRUE;

    return FALSE;
  }

  bool in_school(CHAR_DATA *ch) {

    if (ch->in_room == NULL) {
      return FALSE;
    }

    if (district_room(ch->in_room) == DISTRICT_INSTITUTE) {
      return TRUE;
    }

    return FALSE;
  }

  bool room_in_school(int vnum) {
    ROOM_INDEX_DATA *room;

    room = get_room_index(vnum);

    if (room == NULL) {
      return FALSE;
    }

    if (district_room(room) == DISTRICT_INSTITUTE) {
      return TRUE;
    }

    return FALSE;
  }

  bool is_lockable(int vnum) { // vnum whitelist for do_lock
    return FALSE;
  }

  int regress_mod(CHAR_DATA *ch, int stat) { return 0; }
  bool pact_holder(CHAR_DATA *ch) {
    if (ch->modifier == MODIFIER_PACT)
    return TRUE;
    if (IS_FLAG(ch->act, PLR_DEMONPACT))
    return TRUE;

    return FALSE;
  }

  bool in_wilds(int areavnum) {
    if (areavnum == WILDS_FOREST_VNUM)
    return TRUE;

    return FALSE;
  }

  bool is_wildsapproved(CHAR_DATA *ch) {
    if (ch->pcdata->account != NULL) {
      if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOOFFWORLD))
      return FALSE;
      if (ch->pcdata->account->karmaearned >= 10000)
      return TRUE;
    }
    return FALSE;
  }

  int villain_rating(CHAR_DATA *ch) {
    int val = 80;
    val += ch->pcdata->account->villain_score;
    val += ch->pcdata->account->villain_mod;
    val = UMIN(val, 100);
    val = UMIN(val, 60);
    return val;
  }

  bool under_opression(CHAR_DATA *ch) {
    if (is_gm(ch))
    return FALSE;
    if (is_ghost(ch))
    return FALSE;
    if (ch->oppress != 0 && clan_lookup(ch->oppress) != NULL)
    return TRUE;
  
    return FALSE;
  }

  bool super_aware(CHAR_DATA *ch) {
    if (ch->faction != 0)
    return TRUE;

    if (is_super(ch) || ch->race == RACE_WILDLING) {
      if (ch->played / 3600 > 100)
      return TRUE;
    }
    if (get_skill(ch, SKILL_DEMONOLOGY) > 0)
    return TRUE;

    return FALSE;
  }

  int unused_lifeforce(CHAR_DATA *ch) {
    int amount = ch->lifeforce;
    amount -= ch->lf_taken;
    amount -= ch->lf_sused;
    return amount;
  }

  int base_lifeforce(CHAR_DATA *ch) {
    int amount = ch->lifeforce;
    amount -= ch->lf_used;
    amount -= ch->lf_taken;
    amount -= ch->lf_sused;
    return amount;
  }

  void give_lifeforce_nouse(CHAR_DATA *ch, int amount, char *reason) {
    char buf[MSL];
    sprintf(buf, "LIFEFORCE %d given to %s for %s.", amount, ch->name, reason);
    log_string(buf);
    if (ch->lf_taken >= amount) {
      ch->lf_taken -= amount;
      return;
    }
    else if (ch->lf_taken > 0) {
      amount -= ch->lf_taken;
      ch->lf_taken = 0;
    }
    if (amount != 0) {
      ch->lf_taken -= amount;
    }
    guest_match(ch);
  }

  void give_lifeforce(CHAR_DATA *ch, int amount, char *reason) {
    char buf[MSL];
    sprintf(buf, "LIFEFORCE %d given to %s for %s.", amount, ch->name, reason);
    log_string(buf);
    if (ch->lf_taken >= amount) {
      ch->lf_taken -= amount;
      return;
    }
    else if (ch->lf_taken > 0) {
      amount -= ch->lf_taken;
      ch->lf_taken = 0;
    }
    if (ch->lf_used >= amount) {
      ch->lf_used -= amount;
      return;
    }
    else if (ch->lf_used > 0) {
      amount -= ch->lf_used;
      ch->lf_used = 0;
    }
    if (amount != 0) {
      ch->lf_used -= amount / 2;
      ch->lf_taken -= amount / 2;
    }
    guest_match(ch);
  }
  
  void use_lifeforce(CHAR_DATA *ch, int amount, char *reason) {
    ch->lf_used += amount;
    char buf[MSL];
    sprintf(buf, "LIFEFORCE %d taken from %s for %s.", amount, ch->name, reason);
    log_string(buf);
    guest_match(ch);
  }
  
  void take_lifeforce(CHAR_DATA *ch, int amount, char *reason) {
    ch->lf_taken += amount;
    char buf[MSL];
    sprintf(buf, "LIFEFORCE %d taken from %s for %s.", amount, ch->name, reason);
    log_string(buf);
    guest_match(ch);
  }

  bool is_ill(CHAR_DATA *ch) {
    if (IS_AFFECTED(ch, AFF_ILLNESS))
    return TRUE;
    if (IS_AFFECTED(ch, AFF_MALADY))
    return TRUE;

    return FALSE;
  }

  bool story_on(CHAR_DATA *ch) {
    if (IS_FLAG(ch->comm, COMM_STORY)) {
      return TRUE;
    }

    return FALSE;
  }

  bool move_helpless(CHAR_DATA *ch) {
    if (IS_FLAG(ch->act, PLR_BOUNDFEET))
    return TRUE;

    bool orig = is_helpless(ch);
    if (orig == TRUE && !IS_FLAG(ch->act, PLR_BOUND))
    return TRUE;

    if (orig == FALSE)
    return FALSE;

    REMOVE_FLAG(ch->act, PLR_BOUND);
    bool after = is_helpless(ch);
    SET_FLAG(ch->act, PLR_BOUND);
    return after;
  }

  bool mirror_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (IS_SET(room->room_flags, ROOM_BATHROOM))
    return TRUE;

    if (strcasestr(room->description, "mirror"))
    return TRUE;

    if (room->vnum == 15108 || room->vnum == 15107)
    return TRUE;

    if (room->vnum == 15035 || room->vnum == 18163)
    return TRUE;

    if (room->area->vnum >= 2 && room->area->vnum <= 12)
    return TRUE;
    if (room->area->vnum >= 26 && room->area->vnum <= 28)
    return TRUE;

    return FALSE;
  }

  bool staff_account(ACCOUNT_TYPE *account) {
    if (account == NULL)
    return FALSE;
  
    if (!str_cmp(account->name, "Tyr"))
    return TRUE;
    if (!str_cmp(account->name, "Daed"))
    return TRUE;
    if (!str_cmp(account->name, "Discordance"))
    return TRUE;

    return FALSE;
  }
  bool staff_char(CHAR_DATA *ch) {
    if (ch == NULL || ch->pcdata->account == NULL)
    return FALSE;
    return staff_account(ch->pcdata->account);
  }

  bool workshop_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->vnum == 405006)
    return TRUE;
    if (!strcmp(room->subarea, "technology"))
    return TRUE;

    return FALSE;
  }
  bool engineering_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->vnum == 405015)
    return TRUE;
    if (!strcmp(room->subarea, "technology"))
    return TRUE;

    return FALSE;
  }
  bool alchemy_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->vnum == 405009)
    return TRUE;
    if (!strcmp(room->subarea, "laboratory"))
    return TRUE;

    return FALSE;
  }
  bool library_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->vnum == 405005)
    return TRUE;
    if (!strcmp(room->subarea, "library"))
    return TRUE;

    return FALSE;
  }
  bool goblin_market(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->vnum == 405007)
    return TRUE;
    if (room->vnum == 405004)
    return TRUE;
    return FALSE;
  }

  bool arcane_circle(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->vnum == 405025)
    return TRUE;
    if (room->vnum == 405026)
    return TRUE;
    if (room->vnum == 405027)
    return TRUE;
    if (room->vnum == 405028)
    return TRUE;
    if (room->vnum == 405029)
    return TRUE;

    return FALSE;
  }

  bool brainwash_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (!strcmp(room->subarea, "operating"))
    return TRUE;
    return FALSE;
  }
  
  bool operating_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (!strcmp(room->subarea, "operating"))
    return TRUE;

    return FALSE;
  }
  
  bool harvesting_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (!strcmp(room->subarea, "operating"))
    return TRUE;

    return FALSE;
  }
  
  bool college_dorms(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    if (room->x >= 3 && room->x <= 7 && room->y >= 57 && room->y <= 62 && room->z >= 0)
    return TRUE;

    return FALSE;
  }

  bool institute_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;

    if (district_room(room) == DISTRICT_INSTITUTE) {
      return TRUE;
    }

    return FALSE;
  }

  bool visitors_area(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return FALSE;
    }
    if (!strcmp(room->subarea, "visitor")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "courtyard")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "union")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "clinic")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "hospital")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "operating")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "HSD")) {
      return TRUE;
    }
    if (!strcmp(room->subarea, "mainoffice")) {
      return TRUE;
    }

    return FALSE;
  }
  
  bool school_room(ROOM_INDEX_DATA *room) { return FALSE; }
  
  bool college_room(ROOM_INDEX_DATA *room) { return FALSE; }
  
  bool asylum_room(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return FALSE;
    }
    if (!strcmp(room->subarea, "clinic")) {
      return TRUE;
    }

    return FALSE;
  }

  bool clinic_visitors_area(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return FALSE;
    }

    if (visitors_area(room)) {
      return TRUE;
    }

    return FALSE;
  }

  bool has_clinic_visitors_pass(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL && can_see_obj(ch, obj) && (can_see_wear(ch, iWear))) {
        if (obj->pIndexData->vnum == 45194)
        return TRUE;
      }
    }
    return FALSE;
  }

  bool private_school_rooms(ROOM_INDEX_DATA *room) {
    if (institute_room(room)) {
      if (IS_SET(room->room_flags, ROOM_BATHROOM)) {
        return TRUE;
      }

      if (is_name("closet", room->name)) {
        return TRUE;
      }

      if (is_name("bathroom", room->name)) {
        return TRUE;
      }

      if (is_name("restroom", room->name)) {
        return TRUE;
      }

      if (is_name("locker", room->name)) {
        return TRUE;
      }
    }

    return FALSE;
  }

  bool private_school_room(ROOM_INDEX_DATA *room) {
    return FALSE;
    if (IS_SET(room->room_flags, ROOM_PUBLIC)) {
      return FALSE;
    }

    if (institute_room(room)) {
      if (!strcmp(room->subarea, "secret")) {
        return TRUE;
      }
    }
    
    if (private_school_rooms(room)) {
      int count = 0;

      for (CharList::iterator it = room->people->begin();
      it != room->people->end(); ++it) {
        CHAR_DATA *vch = *it;
        if (vch == NULL)
        continue;

        if (college_student(vch, FALSE) || college_staff(vch, FALSE) || clinic_staff(vch, FALSE))
        count++;
      }

      if (count >= 2) {
        return TRUE;
      }
    }

    return FALSE;
  }

  bool private_clinic_room(ROOM_INDEX_DATA *room) { return FALSE; }

  bool private_college_room(ROOM_INDEX_DATA *room) { return FALSE; }

  bool victim_faction(FACTION_TYPE *fac, FACTION_TYPE *opp) {
    if (fac == NULL || opp == NULL)
    return FALSE;
    if (fac->alliance == opp->alliance && fac->type == opp->type)
    return FALSE;

    if(safe_strlen(fac->eidilon) > 2 && !str_cmp(fac->eidilon, opp->eidilon))
    {
      if(fac->type == FACTION_CULT || fac->subtype == FACTION_CULT)
      {
        if(opp->type == FACTION_SECT || opp->subtype == FACTION_SECT)
        return TRUE;
      }
      if(fac->type == FACTION_SECT || fac->subtype == FACTION_SECT)
      {
        if(opp->type == FACTION_CULT || opp->subtype == FACTION_CULT)
        return TRUE;
      }
    }

    for (int i = 0; i < 10; i++) {
      if (fac->enemies[i][0] == opp->vnum && fac->enemies[i][1] >= 2)
      return TRUE;
    }
    for (int i = 0; i < 10; i++) {
      if (opp->enemies[i][0] == fac->vnum && opp->enemies[i][1] == 2)
      return TRUE;
    }
    return FALSE;
  }

  bool has_weakness(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (is_guest(victim))
    return FALSE;
    if (IS_FLAG(victim->act, PLR_VILLAIN))
    return TRUE;
    if (is_weakness(ch, victim))
    return FALSE;
    if (get_skill(victim, SKILL_DREAMSIGHT) > 0)
    return FALSE;
    if (get_skill(victim, SKILL_TOUCHED) > 0)
    return FALSE;
    if (victim->pcdata->weakness_status >= WEAKNESS_HAS)
    return TRUE;
    return FALSE;
  }
  bool is_weakness(CHAR_DATA *ch, CHAR_DATA *victim) {
    for (int i = 0; i < 10; i++) {
      if (victim->pcdata->relationship_type[i] == REL_WEAKNESS_OF)
      return TRUE;
      if (victim->pcdata->relationship_type[i] == REL_PRAESTES_OF)
      return TRUE;
    }
    if (ch == NULL)
    return FALSE;
    if (get_skill(victim, SKILL_TOUCHED) > 0)
    return TRUE;

    if (victim->pcdata->intel >= 8500)
    return TRUE;

    if (victim_faction(clan_lookup(victim->faction), clan_lookup(ch->faction)))
    return TRUE;

    return FALSE;
  }

  bool is_griefer(CHAR_DATA *ch) {
    if (ch->desc == NULL)                                    {return FALSE;}
    if (ch->desc->host == NULL)                              {return FALSE;}
    if (ch->desc->account == NULL)                           {return FALSE;}
    if (ch->pcdata->account != NULL
    && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_GRIEFER)) {return TRUE;}

    return FALSE;
  }

  bool physical_dreamer(CHAR_DATA *ch) {
    if (!is_dreaming(ch))
    return FALSE;
    if (ch->race == RACE_FANTASY)
    return FALSE;
    if (ch->pcdata->ghost_room > 300 && get_room_index(ch->pcdata->ghost_room) != NULL && get_room_index(ch->pcdata->ghost_room)->area->vnum == 30)
    return TRUE;
  
    return FALSE;
  }

  bool has_gasmask(CHAR_DATA *ch) {
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID && (ch->shape != SHAPE_WOLF || get_skill(ch, SKILL_HYBRIDSHIFTING) < 1))
    return FALSE;

    OBJ_DATA *obj;
    for (int iWear = 0; iWear < MAX_WEAR; iWear++) {
      if ((obj = get_eq_char(ch, iWear)) != NULL) {
        if (obj->pIndexData->vnum == ITEM_GASMASK && can_see_wear(ch, iWear))
        return TRUE;
      }
    }
    return FALSE;
  }

  bool higher_power(CHAR_DATA *ch) {
    if (IS_NPC(ch))
    return FALSE;
    if (ch->race == RACE_SPIRIT_DEMON || ch->race == RACE_SPIRIT_FAE || ch->race == RACE_SPIRIT_DIVINE || ch->race == RACE_SPIRIT_GHOST || ch->race == RACE_SPIRIT_CTHULIAN || ch->race == RACE_SPIRIT_PRIMAL)
    return TRUE;
  
    return FALSE;
  }

  bool can_manual_task(CHAR_DATA *ch)
  {
    if (is_animal(ch) && get_animal_genus(ch, ANIMAL_ACTIVE) != GENUS_HYBRID)
    return FALSE;
    if(is_ghost(ch) && !is_manifesting(ch))
    return FALSE;
  
    return TRUE;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

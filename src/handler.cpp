#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <map>
#include <ext/hash_map>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "gsn.h"

#if defined(__cplusplus)
extern "C" {
#endif


/*
 * Local functions.
 */
void affect_modify args((CHAR_DATA * ch, AFFECT_DATA *paf, bool fAdd));
void mod_agility args((CHAR_DATA * ch));

// makes any string start with a lowercase letter - Discordance
char *lowercase_clause(char *clause) {
  if (clause[0] >= 'A' && clause[0] <= 'Z')
    clause[0] = clause[0] + 'a' - 'A';

  return clause;
}

char *indefinite(char *str) {
  char first;

  /* Get first character in lower case */
  first = tolower(str[0]);
  switch (first) {
  /* If it's a vowel, return "an" */
  case 'a':
  case 'e':
  case 'i':
  case 'o':
  case 'u':
    return "an";
  /* Otherwise return "a" */
  default:
    return "a";
  }

  /* To prevent warnings from dumb compilers */
  return "a";
}

char *NAME2(CHAR_DATA *ch) {
  if (IS_IMMORTAL(ch))
    return ch->short_descr;

  if (IS_NPC(ch))
    return ch->short_descr;

  if (is_cloaked(ch)) {
    if (ch->sex == SEX_MALE)
      return "A nondescript man";
    else
      return "A nondescript woman";
  }
  if (is_masked(ch)) {
    return mask_intro(ch, ch->pcdata->maskednumber);
  }
  return ch->name;
}

// Changed a bunch of stuff in the PERS functions where commented - Discordance
char *PERS(CHAR_DATA *ch, CHAR_DATA *looker) {
  /* Buffer to contain final string */
  static char buf[MAX_INPUT_LENGTH];
  NameMap::const_iterator nit;

  sprintf(buf, " ");
  buf[0] = '\0';
  bool found = FALSE;

  if (looker == NULL) {
    sprintf(buf, "%s", ch->name);
    return buf;
  }
  // NPC's just show their short descriptions
  if (IS_NPC(ch))
    return ch->short_descr;

  if (!IS_NPC(ch) && !IS_NPC(looker) && is_dreaming(ch) && is_dreaming(looker))
    return dream_name(ch);

  if(higher_power(ch))
      return ch->name;
  if(higher_power(looker) && !IS_NPC(looker) && (looker->pcdata->eidilon_of == ch->fsect || looker->pcdata->eidilon_of == ch->fcult || get_gmtrust(looker, ch) >= 1))
	return ch->name;

  if (IS_FLAG(looker->act, PLR_SINSPIRIT))
    return ch->name;

  if (is_dreaming(ch) && is_dreaming(looker)) {
    if (safe_strlen(ch->pcdata->dream_intro) > 2)
      return ch->pcdata->dream_intro;
    else
      return ch->name;
  }
  // Enthrall and enrapture
  if (is_cloaked(ch) && !IS_IMMORTAL(looker) && can_see(looker, ch)) {
    if (!IS_NPC(looker) && !str_cmp(looker->pcdata->enthralled, ch->name))
      return "Your master";
    if (!IS_NPC(looker) && !str_cmp(looker->pcdata->enraptured, ch->name))
      return "Your adored";

    if (ch->sex == SEX_MALE)
      return "A nondescript man";
    else
      return "A nondescript woman";
  }

  // Everyone knows an immortal and immortals know everyone.  GM's can see
  // everyone's names except for immortals.
  if ((IS_IMMORTAL(ch) && !IS_FLAG(ch->act, PLR_ROOMINVIS) &&
       !IS_FLAG(ch->act, PLR_WHOINVIS)) ||
      (IS_IMMORTAL(looker)) || // IS_FLAG(ch->act, PLR_GM) ||
      (IS_FLAG(looker->act, PLR_GM) && !IS_IMMORTAL(ch)) || IS_NPC(looker) ||
      ch == looker || (ch->desc && ch->desc->connected == CON_CREATION)) {
    if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
      sprintf(buf, "The ghost of %s", NAME(ch));
    } else {
      sprintf(buf, "%s", NAME(ch));
    }
    return buf;
  }

  // Special more thematic lines for ghosts
  if (is_ghost(ch) && is_visible(ch)) {
    if (!can_see(looker, ch)) {
      sprintf(buf, "%s", NAME(ch));
      return buf;
    }
  } else if (is_ghost(ch)) {
    // Rephrased to be less specific
    if (!can_see(looker, ch)) {
      return "Something unseen";
    }
  }

  // Hidden targets
  if (!can_see(looker, ch) && IS_IMMORTAL(ch)) {
    sprintf(buf, "%s", capitalize(get_intro(ch)));
    return buf; // Hidden immortals
  }
  if (!can_see(looker, ch))
    return "Someone unseen"; // Hidden players
  // Naturals (also characters with obsfuscate to avoid cheatery) for shrouder's
  // viewpoint
  //    if((!can_shroud(ch)) && !IS_FLAG(ch->act, PLR_SHROUD) &&
  //    IS_FLAG(looker->act, PLR_SHROUD))
  //        return "A shadowy figure";

  // Changed storyrunner appearance because storyrunners names are revealed when
  // looking at them and when voting anyhow
  if (is_gm(ch) && !IS_IMMORTAL(ch)) {
    /*
            sprintf(buf, "Story runner %s", ch->name);
    */
    return "A lingering, invisible spirit";
  }
  if (IS_AFFECTED(looker, AFF_MAD) && number_percent() % 9 == 0) {
    if (ch->sex == SEX_FEMALE) {
      for (NameMap::iterator it = looker->pcdata->female_names->begin();
           it != looker->pcdata->female_names->end(); ++it) {
        if (number_percent() % 17 == 0) {
          sprintf(buf, "%s", it->second);
          return buf;
        }
      }
    } else {
      for (NameMap::iterator it = looker->pcdata->male_names->begin();
           it != looker->pcdata->male_names->end(); ++it) {
        if (number_percent() % 17 == 0) {
          sprintf(buf, "%s", it->second);
          return buf;
        }
      }
    }
  }

  // Gender specific
  // Presenting Female
  if (ch->sex == SEX_FEMALE) {
    if ((nit = looker->pcdata->female_names->find(ch->id)) !=
        looker->pcdata->female_names->end()) {
      if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
        sprintf(buf, "The ghost of %s", nit->second);
      } else {
        sprintf(buf, "%s", nit->second);
      }
      found = TRUE;
    }
  }
  // Presenting Male
  else if (ch->sex == SEX_MALE) {
    if ((nit = looker->pcdata->male_names->find(ch->id)) !=
        looker->pcdata->male_names->end()) {
      if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
        sprintf(buf, "The ghost of %s", nit->second);
      } else {
        sprintf(buf, "%s", nit->second);
      }
      found = TRUE;
    }
  }

  // Intros
  if (found == FALSE) {
    if (is_masked(ch))
      sprintf(buf, "%s", capitalize(mask_intro(ch, ch->pcdata->maskednumber)));
    else if (IS_FLAG(ch->act, PLR_DEAD))
      sprintf(buf, "The ghost of %s", capitalize(get_intro(ch)));
    else
      sprintf(buf, "%s", capitalize(get_intro(ch)));
  }
  if (!IS_NPC(looker) && !IS_NPC(ch) &&
      looker->pcdata->dream_identity_timer > 0 &&
      safe_strlen(looker->pcdata->identity_world) > 1 &&
      safe_strlen(dream_detail(looker, looker->pcdata->identity_world,
                          DREAM_DETAIL_SHORT)) > 1) {
    if (found == TRUE && safe_strlen(dream_detail(ch, looker->pcdata->identity_world,
                                             DREAM_DETAIL_NAME)) > 1)
      return dream_detail(ch, looker->pcdata->identity_world,
                          DREAM_DETAIL_NAME);
    if (found == FALSE &&
        safe_strlen(dream_detail(ch, looker->pcdata->identity_world,
                            DREAM_DETAIL_SHORT)) > 1)
      return dream_detail(ch, looker->pcdata->identity_world,
                          DREAM_DETAIL_SHORT);
  }
  return buf;
}

char *PERS_2(CHAR_DATA *ch, CHAR_DATA *looker) {
  /* Buffer to contain final string */
  static char buf[MAX_INPUT_LENGTH];
  NameMap::const_iterator nit;
  sprintf(buf, " ");
  buf[0] = '\0';
  bool found = FALSE;

  if (looker == NULL) {
    sprintf(buf, "%s", ch->name);
    return buf;
  }

  // NPC's just show their short descriptions
  if (IS_NPC(ch))
    return ch->short_descr;

  if (!IS_NPC(ch) && !IS_NPC(looker) && is_dreaming(ch) && is_dreaming(looker))
    return dream_name(ch);

  if (IS_FLAG(looker->act, PLR_SINSPIRIT))
    return ch->name;

  if (is_dreaming(ch) && is_dreaming(looker)) {
    if (safe_strlen(ch->pcdata->dream_intro) > 2)
      return ch->pcdata->dream_intro;
    else
      return ch->name;
  }

  if(higher_power(ch))
      return ch->name;
  if(higher_power(looker) && !IS_NPC(looker) && (looker->pcdata->eidilon_of == ch->fsect || looker->pcdata->eidilon_of == ch->fcult || get_gmtrust(looker, ch) >= 1))
        return ch->name;


  if (is_cloaked(ch) && can_see(looker, ch)) {
    if (!IS_NPC(looker) && !str_cmp(looker->pcdata->enthralled, ch->name))
      return "Your master";
    if (!IS_NPC(looker) && !str_cmp(looker->pcdata->enraptured, ch->name))
      return "Your adored";

    if (ch->sex == SEX_MALE)
      return "A nondescript man";
    else
      return "A nondescript woman";
  }

  // Everyone knows an immortal, and immortals know everyone
  if ((IS_IMMORTAL(ch) && !IS_FLAG(ch->act, PLR_ROOMINVIS) &&
       !IS_FLAG(ch->act, PLR_WHOINVIS)) ||
      (IS_IMMORTAL(looker)) ||
      // IS_FLAG(ch->act, PLR_GM) ||
      IS_FLAG(looker->act, PLR_GM) || IS_NPC(looker) || ch == looker ||
      (ch->desc && ch->desc->connected == CON_CREATION)) {
    if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
      sprintf(buf, "The ghost of %s", NAME(ch));
    } else {
      sprintf(buf, "%s", NAME(ch));
    }
    return buf;
  }

  // Special more thematic lines for ghosts
  if (is_ghost(ch) && is_visible(ch)) {
    // Rephrased to be less specific
    if (!can_see(looker, ch)) {
      sprintf(buf, "%s", NAME(ch));
      return buf;
    }
  } else if (is_ghost(ch)) {
    // Rephrased to be less specific
    if (!can_see(looker, ch)) {
      return "Something unseen";
    }
  }

  // Hidden targets
  if (!can_see(looker, ch) && IS_IMMORTAL(ch)) {
    sprintf(buf, "%s", capitalize(get_intro(ch)));
    return buf; // Hidden immortals
  }

  if (!can_see(looker, ch))
    return "Someone unseen"; // Hidden players

  // Naturals (also characters with obsfuscate to avoid cheatery) for shrouder's
  // viewpoint
  //    if((!can_shroud(ch) || !seems_super(ch)) && IS_FLAG(looker->act,
  //    PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD))
  //        return "A shadowy figure";

  // Changed storyrunner appearance because storyrunners names are revealed when
  // looking at them and when voting anyhow
  if (is_gm(ch) && !IS_IMMORTAL(ch)) {
    /*
            sprintf(buf, "Story runner %s", ch->name);
    */
    return "A lingering, invisible spirit";
  }

  if (IS_AFFECTED(looker, AFF_MAD) && number_percent() % 9 == 0) {
    if (ch->sex == SEX_FEMALE) {
      for (NameMap::iterator it = looker->pcdata->female_names->begin();
           it != looker->pcdata->female_names->end(); ++it) {
        if (number_percent() % 17 == 0) {
          sprintf(buf, "%s", it->second);
          return buf;
        }
      }
    } else {
      for (NameMap::iterator it = looker->pcdata->male_names->begin();
           it != looker->pcdata->male_names->end(); ++it) {
        if (number_percent() % 17 == 0) {
          sprintf(buf, "%s", it->second);
          return buf;
        }
      }
    }
  }

  // Gender specific
  // Presenting Female
  if (ch->sex == SEX_FEMALE) {
    if ((nit = looker->pcdata->female_names->find(ch->id)) !=
        looker->pcdata->female_names->end()) {
      if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
        sprintf(buf, "The ghost of %s", nit->second);
      } else {
        sprintf(buf, "%s", nit->second);
      }
      found = TRUE;
    }
  }
  // Presenting Male
  else if (ch->sex == SEX_MALE) {
    if ((nit = looker->pcdata->male_names->find(ch->id)) !=
        looker->pcdata->male_names->end()) {
      if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
        sprintf(buf, "The ghost of %s", nit->second);
      } else {
        sprintf(buf, "%s", nit->second);
      }
      found = TRUE;
    }
  }

  // Intros
  if (found == FALSE) {
    if (is_masked(ch))
      sprintf(buf, "%s", capitalize(mask_intro(ch, ch->pcdata->maskednumber)));
    else if (IS_FLAG(ch->act, PLR_DEAD))
      sprintf(buf, "The ghost of %s", capitalize(get_intro(ch)));
    else
      sprintf(buf, "%s", capitalize(get_intro(ch)));
  }
  if (!IS_NPC(looker) && !IS_NPC(ch) &&
      looker->pcdata->dream_identity_timer > 0 &&
      safe_strlen(looker->pcdata->identity_world) > 1 &&
      safe_strlen(dream_detail(looker, looker->pcdata->identity_world,
                          DREAM_DETAIL_SHORT)) > 1) {
    if (found == TRUE && safe_strlen(dream_detail(ch, looker->pcdata->identity_world,
                                             DREAM_DETAIL_NAME)) > 1)
      return dream_detail(ch, looker->pcdata->identity_world,
                          DREAM_DETAIL_NAME);
    if (found == FALSE &&
        safe_strlen(dream_detail(ch, looker->pcdata->identity_world,
                            DREAM_DETAIL_SHORT)) > 1)
      return dream_detail(ch, looker->pcdata->identity_world,
                          DREAM_DETAIL_SHORT);
  }
  return buf;
}

char *PERS_3(CHAR_DATA *ch, CHAR_DATA *looker) {
  /* Buffer to contain final string */
  static char buf[MAX_INPUT_LENGTH];
  NameMap::const_iterator nit;
  sprintf(buf, " ");
  buf[0] = '\0';
  bool found = FALSE;

  if (looker == NULL) {
    sprintf(buf, "%s", ch->name);
    return buf;
  }

  /* NPC's just show their short descriptions */
  if (IS_NPC(ch)) {
    return lowercase_clause(ch->short_descr);
  }

  if (!IS_NPC(ch) && !IS_NPC(looker) && is_dreaming(ch) && is_dreaming(looker))
    return dream_name(ch);

  if (IS_FLAG(looker->act, PLR_SINSPIRIT))
    return ch->name;

  if(higher_power(ch))
      return ch->name;
  if(higher_power(looker) && !IS_NPC(looker) && (looker->pcdata->eidilon_of == ch->fsect || looker->pcdata->eidilon_of == ch->fcult || get_gmtrust(looker, ch) >= 1))
        return ch->name;



  if (is_dreaming(ch) && is_dreaming(looker)) {
    if (safe_strlen(ch->pcdata->dream_intro) > 2)
      return ch->pcdata->dream_intro;
    else
      return ch->name;
  }

  if (is_cloaked(ch) && can_see(looker, ch)) {
    if (!IS_NPC(looker) && !str_cmp(looker->pcdata->enthralled, ch->name))
      return "Your master";
    if (!IS_NPC(looker) && !str_cmp(looker->pcdata->enraptured, ch->name))
      return "Your adored";

    if (ch->sex == SEX_MALE)
      return "A nondescript man";
    else
      return "A nondescript woman";
  }

  // Everyone knows an immortal, and immortals know everyone
  if ((IS_IMMORTAL(ch) && !IS_FLAG(ch->act, PLR_ROOMINVIS) &&
       !IS_FLAG(ch->act, PLR_WHOINVIS)) ||
      (IS_IMMORTAL(looker)) ||
      // IS_FLAG(ch->act, PLR_GM) ||
      IS_FLAG(looker->act, PLR_GM) || IS_NPC(looker) || ch == looker ||
      (ch->desc && ch->desc->connected == CON_CREATION)) {
    if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
      sprintf(buf, "the ghost of %s", NAME(ch));
    } else {
      sprintf(buf, "%s", NAME(ch));
    }
    return buf;
  }

  // Special more thematic lines for ghosts
  if (is_ghost(ch) && is_visible(ch)) {
    // Rephrased to be less specific
    if (!can_see(looker, ch)) {
      sprintf(buf, "%s", NAME(ch));
      return buf;
    }
  } else if (is_ghost(ch)) {
    // Rephrased to be less specific
    if (!can_see(looker, ch)) {
      return "something unseen";
    }
  }

  // Hidden targets
  if (!can_see(looker, ch) && IS_IMMORTAL(ch)) {
    sprintf(buf, "%s", capitalize(get_intro(ch)));
    return buf; // Hidden immortals
  }

  if (!can_see(looker, ch))
    return "Someone unseen"; // Hidden players

  // Naturals (also characters with obsfuscate to avoid cheatery) for shrouder's
  // viewpoint
  //    if((!can_shroud(ch) || !seems_super(ch)) && IS_FLAG(looker->act,
  //    PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD))
  //        return "a shadowy figure";

  // Changed storyrunner appearance because storyrunners names are revealed when
  // looking at them and when voting anyhow
  if (is_gm(ch) && !IS_IMMORTAL(ch)) {
    /*
            sprintf(buf, "story runner %s", ch->name);
    */
    return "A lingering, invisible spirit";
  }

  if (IS_AFFECTED(looker, AFF_MAD) && number_percent() % 9 == 0) {
    if (ch->sex == SEX_FEMALE) {
      for (NameMap::iterator it = looker->pcdata->female_names->begin();
           it != looker->pcdata->female_names->end(); ++it) {
        if (number_percent() % 17 == 0) {
          sprintf(buf, "%s", it->second);
          return buf;
        }
      }
    } else {
      for (NameMap::iterator it = looker->pcdata->male_names->begin();
           it != looker->pcdata->male_names->end(); ++it) {
        if (number_percent() % 17 == 0) {
          sprintf(buf, "%s", it->second);
          return buf;
        }
      }
    }
  }

  // Gender specific
  // Presenting Female
  if (ch->sex == SEX_FEMALE) {
    if ((nit = looker->pcdata->female_names->find(ch->id)) !=
        looker->pcdata->female_names->end()) {
      if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
        sprintf(buf, "The ghost of %s", nit->second);
      } else {
        sprintf(buf, "%s", nit->second);
      }
      found = TRUE;
    }
  }
  // Presenting Male
  else if (ch->sex == SEX_MALE) {
    if ((nit = looker->pcdata->male_names->find(ch->id)) !=
        looker->pcdata->male_names->end()) {
      if (IS_FLAG(ch->act, PLR_DEAD) && can_see(looker, ch)) {
        sprintf(buf, "The ghost of %s", nit->second);
      } else {
        sprintf(buf, "%s", nit->second);
      }
      found = TRUE;
    }
  }

  if (found == FALSE) {
    if (is_masked(ch))
      sprintf(buf, "%s", capitalize(mask_intro(ch, ch->pcdata->maskednumber)));
    else if (IS_FLAG(ch->act, PLR_DEAD))
      sprintf(buf, "The ghost of %s", capitalize(get_intro(ch)));
    else
      sprintf(buf, "%s", capitalize(get_intro(ch)));
  }
  if (!IS_NPC(looker) && !IS_NPC(ch) &&
      looker->pcdata->dream_identity_timer > 0 &&
      safe_strlen(looker->pcdata->identity_world) > 1 &&
      safe_strlen(dream_detail(looker, looker->pcdata->identity_world,
                          DREAM_DETAIL_SHORT)) > 1) {
    if (found == TRUE && safe_strlen(dream_detail(ch, looker->pcdata->identity_world,
                                             DREAM_DETAIL_NAME)) > 1)
      return dream_detail(ch, looker->pcdata->identity_world,
                          DREAM_DETAIL_NAME);
    if (found == FALSE &&
        safe_strlen(dream_detail(ch, looker->pcdata->identity_world,
                            DREAM_DETAIL_SHORT)) > 1)
      return dream_detail(ch, looker->pcdata->identity_world,
                          DREAM_DETAIL_SHORT);
  }

  return buf;
}

// Added two different namemaps for gender - Discordance
void add_intro(CHAR_DATA *ch, long id, int sex, char *name) {
  NameMap::iterator nit;
  if (name == NULL) {
    return;
  }

  // Gender Specific
  if (sex == SEX_FEMALE) {
    // Presenting female

    nit = ch->pcdata->female_names->find(id);

    if (nit != ch->pcdata->female_names->end()) {
      free_string(nit->second);
      nit->second = str_dup(name);
    } else {
      ch->pcdata->female_names->insert(make_pair(id, str_dup(name)));
    }
  }
  if (sex == SEX_MALE) {
    // Presenting male

    nit = ch->pcdata->male_names->find(id);

    if (nit != ch->pcdata->male_names->end()) {
      free_string(nit->second);
      nit->second = str_dup(name);
    } else {
      ch->pcdata->male_names->insert(make_pair(id, str_dup(name)));
    }
  }

  return;
}

/*     Already defined in lookup.c, left here in case of future issues.   Kuval
1-27-04

bool check_for_color( char * word )
{
    int i = 0;

    while( word[i] != '\0' )
    {
        if (word[i] == '`')
            return TRUE;
        i++;
    }
    return FALSE;
}

*/

/* returns number of people on an object */
int count_users(OBJ_DATA *obj) {
  int count = 0;

  if (obj->in_room == NULL)
    return 0;

  for (CharList::iterator it = obj->in_room->people->begin();
       it != obj->in_room->people->end(); ++it) {
    if ((*it)->on == obj)
      count++;
  }
  return count;
}

char *item_name(int item_type) {
  int type;

  for (type = 0; item_table[type].name != NULL; type++)
    if (item_type == item_table[type].type)
      return item_table[type].name;
  return "none";
}

char *weapon_name(int weapon_type) { return "exotic"; }

/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust(CHAR_DATA *ch) {
  if (ch == NULL)
    return 0;

  if (ch->name == NULL) // Preventing crash - Discordance 9-9-2015
    return 0;

  if (IS_NPC(ch))
    return 1;

  /*
      if ( ch->desc != NULL && ch->desc->original != NULL )
          ch = ch->desc->original;
  */

  if (ch->trust == -1)
    return 0;

  if (ch->trust)
    return ch->trust;

  if (IS_NPC(ch) && ch->level >= LEVEL_HERO)
    return LEVEL_HERO - 1;
  else
    return ch->level;
}

/*
 * Name: get_affect_by_type
 * Parameters:
 * 	AFFECT_DATA *paf - list of affects to search
 *	sh_int type - type of affect to get
 * Returns:
 * 	AFFECT_DATA - pointer to the affect or NULL
 *
 * Purpose: Get the first instance of an affect from an affect list
 * Author: Cameron Matthews-Dickson
 */
AFFECT_DATA *get_affect_by_type(AFFECT_DATA *paf, sh_int type) {
  for (; paf; paf = paf->next) {
    if (paf->type == type)
      return paf;
  }

  return NULL;
}

/*
 * Name: get_affect_by_location
 * Parameters:
 * 	AFFECT_DATA *paf - list of affects to search
 *	sh_int type - type of affect to get
 * Returns:
 * 	AFFECT_DATA - pointer to the affect or NULL
 *
 * Purpose: Get the first instance of an affect from an affect list
 * Author: Cameron Matthews-Dickson
 */
AFFECT_DATA *get_affect_by_location(AFFECT_DATA *paf, sh_int type) {
  for (; paf; paf = paf->next) {
    if (paf->location == type)
      return paf;
  }

  return NULL;
}

/*
 * Name: get_affect_by_bit
 * Parameters:
 *      AFFECT_DATA *paf - list of affects to search
 *      long bit - bitvector of affect to get
 * Returns:
 *      AFFECT_DATA - pointer to the affect or NULL
 *
 * Purpose: Get the first instance of an affect from an affect list
 * Author: Cameron Matthews-Dickson
 */
AFFECT_DATA *get_affect_by_bit(AFFECT_DATA *paf, long bit) {
  for (; paf; paf = paf->next) {
    if (paf->bitvector == bit)
      return paf;
  }

  return NULL;
}

bool raw_vampire(CHAR_DATA *ch) {
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

/*
 * Retrieve a character's age.
 */
int get_age(CHAR_DATA *ch) {
  int age = 0;
  tm *ptm;
  time_t east_time;

  east_time = current_time;

  if (ch->pcdata->deaged != 0 && ch->pcdata->deaged < 10000 &&
      ch->pcdata->deaged > -10000) {
    east_time -= ch->pcdata->deaged * 3600 * 24;
  }

  ptm = gmtime(&east_time);
  int year = ptm->tm_year + 1900;
  // Modified this to allow setting of apparent age without requiring ageless
  // stat - Discordance if(!IS_NPC(ch) && get_skill(ch, SKILL_AGELESS) > 0 &&
  // ch->pcdata->apparant_age != 0 && ch->pcdata->apparant_age > -10000 &&
  // ch->pcdata->apparant_age < 10000)
  if (raw_vampire(ch)) {
    age = ch->pcdata->sire_year - ch->pcdata->birth_year;
    if (ch->pcdata->birth_month > ch->pcdata->sire_month)
      age--;
    if (ch->pcdata->birth_month == ch->pcdata->sire_month &&
        ch->pcdata->birth_day > ch->pcdata->sire_day)
      age--;

    int yearsdead = year - ch->pcdata->sire_year;
    if (yearsdead > 50) {
      age += yearsdead / 100;
    }

    int days = ch->pcdata->deaged;
    days /= 364;
    age -= (int)(days);

    if (ch->pcdata->timeswept != 0) {
      age -= ch->pcdata->timeswept;
    }
  } else {
    age = year - ch->pcdata->birth_year;
    if (ptm->tm_mon + 1 < ch->pcdata->birth_month)
      age--;
    if (ptm->tm_mon + 1 == ch->pcdata->birth_month &&
        ptm->tm_mday < ch->pcdata->birth_day)
      age--;

    if (ch->pcdata->timeswept != 0)
      age -= ch->pcdata->timeswept;

    if (!IS_NPC(ch) && ch->pcdata->apparant_age != 0 &&
        ch->pcdata->apparant_age > -10000 && ch->pcdata->apparant_age < 10000) {
      if (ch->pcdata->apparant_age > 5)
        age = ch->pcdata->apparant_age;
    }

    if (ch->pcdata->deaged > 10000 || ch->pcdata->deaged < -10000)
      age -= (ch->pcdata->deaged / 1461) * 4;

/*
    if (age > 21) {
      age -= 21;
      age = age * (10 - get_skill(ch, SKILL_BEAUTY)) / 10;
      age += 21;
      age = UMAX(age, 21);
    }
*/
  }

  return age;
}

int get_true_age(CHAR_DATA *ch) {
  tm *ptm;
  time_t east_time;

  east_time = current_time;
  ptm = gmtime(&east_time);
  int year = ptm->tm_year + 1900;
  int age = year - ch->pcdata->birth_year;
  if (ptm->tm_mon + 1 < ch->pcdata->birth_month)
    age--;
  if (ptm->tm_mon + 1 == ch->pcdata->birth_month &&
      ptm->tm_mday < ch->pcdata->birth_day)
    age--;

  return age;
}

int get_real_age(CHAR_DATA *ch) {
  tm *ptm;
  time_t east_time;

  east_time = current_time;

  if (ch->pcdata->agemod != 0)
    east_time += ch->pcdata->agemod * 3600 * 24;

  ptm = gmtime(&east_time);
  int year = ptm->tm_year + 1900;

  int age = year - ch->pcdata->birth_year;
  if (ptm->tm_mon + 1 < ch->pcdata->birth_month)
    age--;
  if (ptm->tm_mon + 1 == ch->pcdata->birth_month &&
      ptm->tm_mday < ch->pcdata->birth_day)
    age--;

  if (ch->pcdata->timeswept != 0)
    age -= ch->pcdata->timeswept;

  return age;
}

/*
 * See if a string is one of the names of an object.
 */
bool is_name_nocol(char *str, char *namelist) {
  return is_name(str, namelist);

  char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
  char *list, *string;

  /* fix crash on NULL namelist */
  if (namelist == NULL || namelist[0] == '\0')
    return FALSE;

  /* fixed to prevent is_name on "" returning TRUE */
  if (str[0] == '\0')
    return FALSE;

  string = str;

  /*
      char colbuf[MSL];
      remove_color(colbuf, namelist);
      char *collist = str_dup(colbuf);
  */

  /* we need ALL parts of string to match part of namelist */
  for (;;) /* start parsing string */
  {
    str = one_argument(str, part);

    if (part[0] == '\0')
      return TRUE;

    /* check to see if this is part of namelist */
    list = from_color(namelist);
    for (;;) /* start parsing namelist */
    {
      list = one_argument(list, name);
      if (name[0] == '\0') /* this name was not found */
        return FALSE;

      if (!str_prefix(string, name))
        return TRUE; /* full pattern match */

      if (!str_prefix(part, name))
        break;
    }
  }
}

bool is_exact_name(char *str, char *namelist) {
  char name[MAX_INPUT_LENGTH];

  if (namelist == NULL)
    return FALSE;

  for (;;) {
    namelist = one_argument(namelist, name);
    if (name[0] == '\0')
      return FALSE;
    if (!str_cmp(str, name))
      return TRUE;
  }
}

bool is_name(char *str, char *namelist) {
  char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
  char *list, *string;

  /* fix crash on NULL namelist */
  if (namelist == NULL || namelist[0] == '\0')
    return FALSE;

  /* fixed to prevent is_name on "" returning TRUE */
  if (str[0] == '\0')
    return FALSE;

  if (safe_strlen(str) < 1 || safe_strlen(namelist) < 1)
    return FALSE;

  string = str;
  /* we need ALL parts of string to match part of namelist */
  for (;;) /* start parsing string */
  {
    str = one_argument(str, part);

    if (part[0] == '\0')
      return TRUE;

    /* check to see if this is part of namelist */
    list = namelist;
    for (;;) /* start parsing namelist */
    {
      list = one_argument(list, name);
      if (name[0] == '\0') /* this name was not found */
        return FALSE;

      if (!str_prefix(string, name))
        return TRUE; /* full pattern match */

      if (!str_prefix(part, name))
        break;
    }
  }
}

/* enchanted stuff for eq */
void affect_enchant(OBJ_DATA *obj) {
  /* okay, move all the old flags into new vectors if we have to */
  AFFECT_DATA *paf, *af_new;

  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
    af_new = new_affect();

    af_new->next = obj->affected;
    obj->affected = af_new;

    af_new->where = paf->where;
    af_new->type = UMAX(0, paf->type);
    af_new->level = paf->level;
    af_new->duration = paf->duration;
    af_new->location = paf->location;
    af_new->modifier = paf->modifier;
    af_new->bitvector = paf->bitvector;
  }
}

/*
 * Apply or remove an affect to a character.
 */
void affect_modify(CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd) {
  int mod;

  mod = paf->modifier;

  if (fAdd) {
    switch (paf->where) {
    case TO_AFFECTS:
      SET_FLAG(ch->affected_by, paf->bitvector);
      break;
    }
  } else {
    switch (paf->where) {
    case TO_AFFECTS:
      REMOVE_FLAG(ch->affected_by, paf->bitvector);
      break;
    }
    mod = 0 - mod;
  }

  if (paf->where != TO_OBJECT || paf->level <= (ch->level + 10)) {
    switch (paf->location) {
    default:
      bug("Affect_modify: unknown location %d.", paf->location);
      return;

    case APPLY_NONE:
      break;
    }
  }

  return;
}

/* find an effect in an affect list */
AFFECT_DATA *affect_find(AFFECT_DATA *paf, int sn) {
  AFFECT_DATA *paf_find;

  for (paf_find = paf; paf_find != NULL; paf_find = paf_find->next) {
    if (paf_find->type == sn)
      return paf_find;
  }

  return NULL;
}

/* fix object affects when removing one */
void affect_check(CHAR_DATA *ch, int where, int vector) {
  AFFECT_DATA *paf;
  OBJ_DATA *obj;

  if (where == TO_OBJECT || where == TO_WEAPON || vector == 0)
    return;

  for (paf = ch->affected; paf != NULL; paf = paf->next)
    if (paf->where == where && paf->bitvector == vector) {
      switch (where) {
      case TO_AFFECTS:
        SET_FLAG(ch->affected_by, vector);
        break;
      }
      return;
    }

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
    if (obj->wear_loc == -1)
      continue;

    for (paf = obj->affected; paf != NULL; paf = paf->next)
      if (paf->where == where && paf->bitvector == vector) {
        switch (where) {
        case TO_AFFECTS:
          SET_FLAG(ch->affected_by, vector);
          break;
        }
        return;
      }

    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      if (paf->where == where && paf->bitvector == vector) {
        switch (where) {
        case TO_AFFECTS:
          SET_FLAG(ch->affected_by, vector);
          break;
        }
        return;
      }
  }
}

/*
 * Give an affect to a char.
 */
void affect_to_char(CHAR_DATA *ch, AFFECT_DATA *paf) {
  AFFECT_DATA *paf_new;

  paf_new = new_affect();

  *paf_new = *paf;
  paf_new->woven = FALSE;
  paf_new->inverted = FALSE;
  if (paf_new->held != FALSE)
    paf_new->held = TRUE;
  VALIDATE(paf_new); /* in case we missed it when we set up paf */
  paf_new->next = ch->affected;

  if (paf_new->weave == FALSE)
    paf_new->caster = NULL;

  if (paf_new->held && paf_new->caster != NULL && paf_new->weave == TRUE) {
    if (IS_NPC(paf_new->caster))
      paf_new->duration = ch->level / 50;
  }

  ch->affected = paf_new;

  affect_modify(ch, paf_new, TRUE);

  if (paf_new->caster != NULL && !IS_NPC(paf_new->caster))
    return;
}

/******************************************************************
 * Name: affect_to_room
 * Parameters:
 *      ROOM_INDEX_DATA *pRoom - the exit to which to add the affect
 *      AFFECT_DATA *paf - the affect to add
 * Returns:
 *      void
 *
 * Purpose:  Adds a new affect to an room.  Add the room to the
 *	list of rooms to update if necessary.
 * Author: Cameron Matthews-Dickson (Scaelorn)
 *****************************************************************/
void affect_to_room(ROOM_INDEX_DATA *pRoom, AFFECT_DATA *paf) {
  AFFECT_DATA *paf_new;

  paf_new = new_affect();

  *paf_new = *paf;
  paf_new->woven = FALSE;
  paf_new->inverted = FALSE;
  if (paf_new->held != FALSE)
    paf_new->held = TRUE;
  VALIDATE(paf_new); /* in case we missed it when we set up paf */
  paf_new->next = pRoom->affected;

  if (paf_new->weave == FALSE)
    paf_new->caster = NULL;
  if (paf_new->held && paf_new->caster != NULL && paf_new->weave == TRUE) {
    if (IS_NPC(paf_new->caster))
      paf_new->duration = paf_new->caster->level / 50;
  }

  pRoom->affected = paf_new;

  switch (paf->where) {
  case TO_AFFECTS:
    SET_FLAG(pRoom->affected_by, paf->bitvector);
    break;
  default:
    bug("Affect_to_room: bad paf->where value", 0);
    return;
  }

  /*
   * If this is the first affect on this room add it to
   * the update list
   */
  if (pRoom->affected->next == NULL) {
    pRoom->next_upd = room_upd_list;
    room_upd_list = pRoom;
    return;
  }

  return;
}

/*****************************************************************
 * Name: affect_remove_room
 * Parameters:
 *      ROOM_INDEX_DATA *pRoom - the room from which to remove the
 *		affect
 *      AFFECT_DATA *paf - the affect to remove
 * Returns:
 *       void
 *
 * Purpose:  Removes an affect from an room.  Also removes it
 *      from the list of rooms to update if no more effects on the
 *      room.
 * Author: Cameron Matthews-Dickson (Scaelorn)
 *****************************************************************/
void affect_remove_room(ROOM_INDEX_DATA *pRoom, AFFECT_DATA *paf) {

  if (pRoom->affected == NULL) {
    bug("Affect_remove_room: no affect.", 0);
    return;
  }

  switch (paf->where) {
  case TO_AFFECTS:
    REMOVE_FLAG(pRoom->affected_by, paf->bitvector);
    break;
  default:
    bug("Affect_remove_room: bad paf->where value", 0);
    return;
  }

  if (paf == pRoom->affected) {
    pRoom->affected = paf->next;
  } else {
    AFFECT_DATA *prev;

    for (prev = pRoom->affected; prev != NULL; prev = prev->next) {
      if (prev->next == paf) {
        prev->next = paf->next;
        break;
      }
    }

    if (prev == NULL) {
      bug("Affect_remove_room: cannot find paf.", 0);
      return;
    }
  }

  /*
   * If this is the last affect on this room remove it
   * from the update list
   */
  if (pRoom->affected == NULL) {
    if (pRoom == room_upd_list)
      room_upd_list = pRoom->next;
    else {
      ROOM_INDEX_DATA *room;
      for (room = room_upd_list; room; room = room->next_upd) {
        if (room->next_upd == pRoom) {
          room->next_upd = pRoom->next_upd;
          break;
        }
      }
    }
  }

  free_affect(paf);

  return;
}

/******************************************************************
 * Name: affect_to_exit
 * Parameters:
 *	EXIT_DATA *pExit - the exit to which to add the affect
 *	AFFECT_DATA *paf - the affect to add
 *	bool reverse - should the reverse exit be affected too?
 * Returns:
 *	void
 *
 * Purpose:  Adds a new affect to an exit.  If the 'reverse'
 *	parameter is TRUE then the exit on the other side will
 *	also have the affect added.  Add the exit to the list
 *	of exits to update if necessary.
 * Author: Cameron Matthews-Dickson (Scaelorn)
 *****************************************************************/
void affect_to_exit(EXIT_DATA *pExit, AFFECT_DATA *paf, bool reverse) {
  AFFECT_DATA *paf_new;
  EXIT_DATA *eRev = NULL;

  paf_new = new_affect();

  *paf_new = *paf;
  paf_new->woven = FALSE;
  paf_new->inverted = FALSE;
  if (paf_new->held != FALSE)
    paf_new->held = TRUE;
  VALIDATE(paf_new); /* in case we missed it when we set up paf */
  paf_new->next = pExit->affected;

  if (paf_new->weave == FALSE)
    paf_new->caster = NULL;

  if (paf_new->held && paf_new->caster != NULL && paf_new->weave == TRUE) {
    if (IS_NPC(paf_new->caster))
      paf_new->duration = paf_new->caster->level / 50;
  }

  pExit->affected = paf_new;

  switch (paf->where) {
  case TO_AFFECTS:
    SET_FLAG(pExit->affected_by, paf->bitvector);
    if (eRev != NULL)
      SET_FLAG(eRev->affected_by, paf->bitvector);
    break;
  default:
    bug("Affect_to_exit: bad paf->where value", 0);
    return;
  }

  /*
   * If this is the first affect on this exit add it to
   * the update list
   */
  if (pExit->affected->next == NULL) {
    pExit->next_upd = exit_upd_list;
    exit_upd_list = pExit;
    return;
  }

  return;
}

/* give an affect to an object */
void affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf) {
  AFFECT_DATA *paf_new;

  paf_new = new_affect();

  *paf_new = *paf;

  VALIDATE(paf_new); /* in case we missed it when we set up paf */
  paf_new->next = obj->affected;
  obj->affected = paf_new;

  /* apply any affect vectors to the object's extra_flags */
  if (paf->bitvector)
    switch (paf->where) {
    case TO_OBJECT:
      SET_BIT(obj->extra_flags, paf->bitvector);
      break;
    case TO_WEAPON:
      if (obj->item_type == ITEM_WEAPON)
        SET_BIT(obj->value[4], paf->bitvector);
      break;
    }

  return;
}

/*
 * Remove an affect from a char.
 */
void affect_remove(CHAR_DATA *ch, AFFECT_DATA *paf) {
  int where;
  int vector;

  if (ch->affected == NULL) {
    bug("Affect_remove: no affect.", 0);
    return;
  }

  affect_modify(ch, paf, FALSE);
  where = paf->where;
  vector = paf->bitvector;

  if (paf == ch->affected) {
    ch->affected = paf->next;
  } else {
    AFFECT_DATA *prev;

    for (prev = ch->affected; prev != NULL; prev = prev->next) {
      if (prev->next == paf) {
        prev->next = paf->next;
        break;
      }
    }

    if (prev == NULL) {
      bug("Affect_remove: cannot find paf.", 0);
      return;
    }
  }

  free_affect(paf);

  affect_check(ch, where, vector);
  return;
}

void affect_remove_obj(OBJ_DATA *obj, AFFECT_DATA *paf) {
  int where, vector;
  if (obj->affected == NULL) {
    bug("Affect_remove_object: no affect.", 0);
    return;
  }

  if (obj->carried_by != NULL && obj->wear_loc != -1)
    affect_modify(obj->carried_by, paf, FALSE);

  where = paf->where;
  vector = paf->bitvector;

  /* remove flags from the object if needed */
  if (paf->bitvector)
    switch (paf->where) {
    case TO_OBJECT:
      REMOVE_BIT(obj->extra_flags, paf->bitvector);
      break;
    case TO_WEAPON:
      if (obj->item_type == ITEM_WEAPON)
        REMOVE_BIT(obj->value[4], paf->bitvector);
      break;
    }

  if (paf == obj->affected) {
    obj->affected = paf->next;
  } else {
    AFFECT_DATA *prev;

    for (prev = obj->affected; prev != NULL; prev = prev->next) {
      if (prev->next == paf) {
        prev->next = paf->next;
        break;
      }
    }

    if (prev == NULL) {
      bug("Affect_remove_object: cannot find paf.", 0);
      return;
    }
  }

  free_affect(paf);

  if (obj->carried_by != NULL && obj->wear_loc != -1)
    affect_check(obj->carried_by, where, vector);
  return;
}

/*
 * Made this check to see if an object is affected by something.
 */
bool is_obj_affected(OBJ_DATA *obj, int bitvec) {
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  for (paf = obj->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    if (paf->bitvector == bitvec)
      return TRUE;
  }

  return FALSE;
}

int get_affect_bonus(CHAR_DATA *ch, int apply_mod) {
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;
  int bonus = 0;

  for (paf = ch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    if (paf->location == apply_mod)
      bonus += paf->modifier;
  }

  return bonus;
}

/*
 * Strip all affects of a given sn.
 */

void affect_strip(CHAR_DATA *ch, int sn) {
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  for (paf = ch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    if (paf->type == sn) {
      affect_remove(ch, paf);
    }
  }

  return;
}

void affect_strip_room(ROOM_INDEX_DATA *room, int sn) {
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  for (paf = room->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    if (paf->type == sn) {
      affect_remove_room(room, paf);
    }
  }

  return;
}

/*
 * Return true if a char is affected by a spell.
 */
bool is_affected(CHAR_DATA *ch, int sn) {
  AFFECT_DATA *paf;

  for (paf = ch->affected; paf != NULL; paf = paf->next) {
    if (paf->type == sn)
      return TRUE;
  }

  return FALSE;
}

/*
 * Add or enhance an affect.
 */
void affect_join(CHAR_DATA *ch, AFFECT_DATA *paf) {
  AFFECT_DATA *paf_old;
  for (paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next) {
    if (paf_old->type == paf->type) {
      paf->level = (paf->level + paf_old->level) / 2;
      paf->duration += paf_old->duration;
      paf->modifier += paf_old->modifier;
      affect_remove(ch, paf_old);
      break;
    }
  }

  affect_to_char(ch, paf);
  return;
}

/*
 * Move a char out of a room.
 */
void char_from_room(CHAR_DATA *ch) {
  char buf[MSL];
  if (ch->in_room == NULL) {
    sprintf(buf, "Char_from_room %s: NULL", ch->name);
    bug(buf, 0);
    return;
  } else {
    //	sprintf(buf, "Char_from_room %s: %s", ch->name, ch->in_room->name);
    //	bug(buf, 0);
  }

  if (!IS_NPC(ch)) {
    --ch->in_room->area->nplayer;
  }

  ch->in_room->people->remove(ch);
  ch->in_room = NULL;
  //    ch->on 	     = NULL;  /* sanity check! */
  return;
}

/*
 * Move a char into a room.
 */
void char_to_room(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) {
  char buf[MSL];
  if (ch == NULL)
    return;
  if (pRoomIndex == NULL) {
    ROOM_INDEX_DATA *room;
    sprintf(buf, "Char_to_room %s: NULL", ch->name);
    bug(buf, 0);

    if ((room = get_room_index(ROOM_VNUM_LIMBO)) != NULL)
      char_to_room(ch, room);

    return;
  }
  //        sprintf(buf, "Char_to_room %s: %s", ch->name, pRoomIndex->name);
  //        bug( buf, 0 );

  if (!IS_NPC(ch)) {
    /* Let's run the check for securities here.
    if ( !check_security ( ch, pRoomIndex->area ) && !ch->pcdata->spec_trust )
    {
        ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
        send_to_char( "You're not permitted into that area.\n\r", ch );
        pRoomIndex = ch->in_room;
    }*/
  }

  ch->in_room = pRoomIndex;
  pRoomIndex->people->push_front(ch);

  if (!IS_NPC(ch)) {
    /*  This was interfering with room titles loading from pfile.  I'm sure this
     is for something like taxis and summons, but
     *  but will have to address those independently - Discordance
            set_title(ch, "");
    */
    if (ch->in_room->area->empty) {
      ch->in_room->area->empty = FALSE;
      ch->in_room->area->age = 0;
    }
    ++ch->in_room->area->nplayer;

    ch->pcdata->tertiary_timer = UMAX(0, ch->pcdata->tertiary_timer - 2);
  }

  /*
      if(!IS_NPC(ch) && is_vampire(ch) && ch->in_room != NULL &&
     ch->in_room->area->vnum != 20 && ch->pcdata->travel_time > 2)
      {
          int sun = sunphase(ch->in_room);
          if((sun == 2 || sun == 6) && !IS_SET(ch->in_room->room_flags,
     ROOM_INDOORS))
          {
              send_to_char("With the glow of the sun just visible on the horizon
     you feel a little uncomfortable as you travel outside.\n\r", ch);
          }
          else if(sun == 3 || sun == 5)
          {
              if(!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
              {
                  send_to_char("Thesunlight burns your skin and stabs into your
     eyes as you travel outside.\n\r", ch);
              }

          }
          else if(sun == 4)
          {
              if(!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
              {
                  send_to_char("The head of the midday sun lashes your skin and
     sears your vision as you travel outside.\n\r", ch);
              }
          }
      }
  */
  /*
      if(!IS_NPC(ch))
      {
          FIGHT_TYPE *Fight;
          if((Fight = get_fight(ch)) != NULL)
          {

          PARTICIPANT_TYPE* participant = new_participant();
          participant->ch = ch;
          ch->attacking = 1;
          participant->attack_timer = FIGHT_WAIT;
          participant->move_timer = FIGHT_WAIT;
          Fight->Participants.push_back(participant);

          }
      }
  */
  return;
}

/*
 * Give an obj to a char.
 */
void obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch) {
  obj->next_content = ch->carrying;
  ch->carrying = obj;
  obj->carried_by = ch;
  obj->in_room = NULL;
  obj->in_obj = NULL;

  // added check so free_string only runs if needed - Discordance
  if (obj->adjust_string && obj->adjust_string != NULL &&
      safe_strlen(obj->adjust_string) >= 1) {
    free_string(obj->adjust_string);
  }
  obj->adjust_string = str_dup("");

  return;
}

/*
 * Take an obj from its character.
 */
void obj_from_char(OBJ_DATA *obj) {
  CHAR_DATA *ch;

  if ((ch = obj->carried_by) == NULL) {
    //	bug( "Obj_from_char: null ch.", 0 );
    return;
  }

  if (obj->wear_loc != WEAR_NONE) {
    unequip_char(ch, obj);
  }

  if (ch->carrying == obj) {
    ch->carrying = obj->next_content;
  } else {
    OBJ_DATA *prev;

    for (prev = ch->carrying; prev != NULL; prev = prev->next_content) {
      if (prev->next_content == obj) {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL)
      bug("Obj_from_char: obj not in list.", 0);
  }

  obj->carried_by = NULL;
  obj->next_content = NULL;
  return;
}

void obj_from_char_silent(OBJ_DATA *obj) {
  CHAR_DATA *ch;

  if ((ch = obj->carried_by) == NULL) {
    bug("Obj_from_char: null ch.", 0);
    return;
  }

  if (obj->wear_loc != WEAR_NONE) {
    unequip_char_silent(ch, obj);
  }

  if (ch->carrying == obj) {
    ch->carrying = obj->next_content;
  } else {
    OBJ_DATA *prev;

    for (prev = ch->carrying; prev != NULL; prev = prev->next_content) {
      if (prev->next_content == obj) {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL)
      bug("Obj_from_char: obj not in list.", 0);
  }

  obj->carried_by = NULL;
  obj->next_content = NULL;
  return;
}

OBJ_DATA *get_eqr_char(CHAR_DATA *ch, int iWear) {
  OBJ_DATA *obj;

  if (ch == NULL)
    return NULL;
  int limit = 0;
  for (obj = ch->carrying; obj != NULL && limit < 200;
       obj = obj->next_content) {
    limit++;
    if (obj->wear_loc == iWear)
      return obj;
  }
  return NULL;
}

/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char(CHAR_DATA *ch, int iWear) {
  OBJ_DATA *obj;

  if (ch == NULL)
    return NULL;
  int limit = 0;
  for (obj = ch->carrying; obj != NULL && limit < 200;
       obj = obj->next_content) {
    limit++;
    if (obj->wear_loc == iWear)
      return obj;
  }
  limit = 0;
  if (iWear == WEAR_HOLD) {
    for (obj = ch->carrying; obj != NULL && limit < 200;
         obj = obj->next_content) {
      limit++;
      if (obj->wear_loc == WEAR_HOLD_2)
        return obj;
    }
  }

  return NULL;
}

OBJ_DATA *get_held(CHAR_DATA *ch, int item_type) {
  OBJ_DATA *obj;
  if ((obj = get_eq_char(ch, WEAR_HOLD)) == NULL ||
      obj->item_type != item_type) {
    if ((obj = get_eq_char(ch, WEAR_HOLD_2)) == NULL ||
        obj->item_type != item_type)
      return NULL;
    else
      return obj;
  } else
    return obj;

  return NULL;
}

// New function to check whether or not an item of a specific type is currently
// being worn - Discordance
OBJ_DATA *get_worn(CHAR_DATA *ch, int item_type) {
  int i;
  OBJ_DATA *obj;
  for (i = 0; i < 25; i++) {
    if ((obj = get_eq_char(ch, i)) != NULL && obj->item_type == item_type) {
      return obj;
    }
  }
  return NULL;
}

/*
 * Equip a char with an obj.
 */
void equip_char(CHAR_DATA *ch, OBJ_DATA *obj, int iWear) {
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj2;
  int i, intWear;
  char *buf2 = NULL;
  char arg1[MSL];

  if (get_eqr_char(ch, iWear) != NULL) {
    sprintf(buf, "Equip_char: [%d] in [%d] %s already equipped (%d).",
            ch->pIndexData->vnum, ch->in_room->vnum, ch->short_descr, iWear);
    bug(buf, 0);
    return;
  }
  int hitperc;
  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    hitperc = ch->hit * 1000 / max_hp(ch);
  }

  int orig = obj->wear_loc;
  obj->wear_loc = iWear;
  int newval = obj->wear_loc;

  for (i = 0; i < MAX_COVERS; i++) {
    obj->wear_loc = orig;
    if (!is_covered(ch, cover_table[i])) {
      obj->wear_loc = newval;
      if (is_covered(ch, cover_table[i]) &&
          safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
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

  for (intWear = 0; intWear < MAX_WEAR; intWear++) {
    obj->wear_loc = orig;
    if ((obj2 = get_eq_char(ch, intWear)) != NULL && can_see_obj(ch, obj) &&
        can_see_wear(ch, intWear)) {
      obj->wear_loc = newval;
      if ((get_eq_char(ch, intWear)) == NULL || !can_see_obj(ch, obj) ||
          !can_see_wear(ch, intWear)) {
        sprintf(buf, "Concealing;%s $o", obj2->wear_string);
        act(buf, ch, obj2, NULL, TO_CHAR);
        act(buf, ch, obj2, NULL, TO_ROOM);
      }
    }
  }

  obj->wear_loc = newval;
  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    ch->hit = max_hp(ch) * hitperc / 1000;
  }

  return;
}

void equip_char_silent(CHAR_DATA *ch, OBJ_DATA *obj, int iWear) {
  char buf[MAX_STRING_LENGTH];

  if (get_eqr_char(ch, iWear) != NULL) {
    sprintf(buf, "Equip_char: [%d] in [%d] %s already equipped (%d).",
            ch->pIndexData->vnum, ch->in_room->vnum, ch->short_descr, iWear);
    bug(buf, 0);
    return;
  }
  int hitperc;
  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    hitperc = ch->hit * 1000 / max_hp(ch);
  }

  obj->wear_loc = iWear;
  int newval = obj->wear_loc;

  obj->wear_loc = newval;

  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    ch->hit = max_hp(ch) * hitperc / 1000;
  }

  return;
}

/*
 * Unequip a char with an obj.
 */
void unequip_char(CHAR_DATA *ch, OBJ_DATA *obj) {
  OBJ_DATA *obj2;
  char buf[MSL];
  char arg1[MSL];
  int i, intWear;
  char *buf2 = NULL;
  if (obj->wear_loc == WEAR_NONE) {
    bug("Unequip_char: already unequipped.", 0);
    return;
  }
  int hitperc;
  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    hitperc = ch->hit * 1000 / max_hp(ch);
  }

  obj->layer = 0;
  obj->exposed = 0;
  free_string(obj->wear_temp);
  obj->wear_temp = str_dup("");

  int orig = obj->wear_loc;
  obj->wear_loc = -1;
  int newval = obj->wear_loc;

  for (i = 0; i < MAX_COVERS; i++) {
    obj->wear_loc = orig;
    if (is_covered(ch, cover_table[i])) {
      obj->wear_loc = newval;
      if (!is_covered(ch, cover_table[i]) &&
          safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
        //                    free_string(buf2);
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
  for (intWear = 0; intWear < MAX_WEAR; intWear++) {
    obj->wear_loc = orig;
    if ((obj2 = get_eq_char(ch, intWear)) == NULL || !can_see_obj(ch, obj) ||
        !can_see_wear(ch, intWear)) {
      obj->wear_loc = newval;
      if ((obj2 = get_eq_char(ch, intWear)) != NULL && can_see_obj(ch, obj) &&
          can_see_wear(ch, intWear)) {
        sprintf(buf, "Revealing;%s $o", obj2->wear_string);
        act(buf, ch, obj2, NULL, TO_CHAR);
        act(buf, ch, obj2, NULL, TO_ROOM);
      }
    }
  }

  obj->wear_loc = newval;
  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    ch->hit = max_hp(ch) * hitperc / 1000;
  }

  return;
}

void unequip_char_silent(CHAR_DATA *ch, OBJ_DATA *obj) {
  OBJ_DATA *obj2;
  int i, intWear;
  if (obj->wear_loc == WEAR_NONE) {
    bug("Unequip_char: already unequipped.", 0);
    return;
  }
  int hitperc;
  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    hitperc = ch->hit * 1000 / max_hp(ch);
  }

  obj->layer = 0;
  free_string(obj->wear_temp);
  obj->wear_temp = str_dup("");

  int orig = obj->wear_loc;
  obj->wear_loc = -1;
  int newval = obj->wear_loc;

  for (i = 0; i < MAX_COVERS; i++) {
    obj->wear_loc = orig;
    if (is_covered(ch, cover_table[i])) {
      obj->wear_loc = newval;
      if (!is_covered(ch, cover_table[i]) &&
          safe_strlen(ch->pcdata->focused_descs[i]) > 5) {
      }
    }
  }
  for (intWear = 0; intWear < MAX_WEAR; intWear++) {
    obj->wear_loc = orig;
    if ((obj2 = get_eq_char(ch, intWear)) == NULL || !can_see_obj(ch, obj) ||
        !can_see_wear(ch, intWear)) {
      obj->wear_loc = newval;
      if ((obj2 = get_eq_char(ch, intWear)) != NULL && can_see_obj(ch, obj) &&
          can_see_wear(ch, intWear)) {
      }
    }
  }

  obj->wear_loc = newval;

  if (IS_SET(obj->extra_flags, ITEM_ARMORED) && !in_fight(ch)) {
    ch->hit = max_hp(ch) * hitperc / 1000;
  }

  return;
}

/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list(OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list) {
  OBJ_DATA *obj;
  int nMatch;

  nMatch = 0;
  for (obj = list; obj != NULL; obj = obj->next_content) {
    if (obj->pIndexData == pObjIndex)
      nMatch++;
  }

  return nMatch;
}

/*
 * Move an obj out of a room.
 */
void obj_from_room(OBJ_DATA *obj) {
  ROOM_INDEX_DATA *in_room;

  if ((in_room = obj->in_room) == NULL) {
    bug("obj_from_room: NULL.", 0);
    return;
  }

  // free_string(obj->adjust_string);
  // obj->adjust_string = str_dup("");

  for (CharList::iterator it = in_room->people->begin();
       it != in_room->people->end(); ++it) {
    if ((*it)->on == obj)
      (*it)->on = NULL;
  }

  if (obj == in_room->contents) {
    in_room->contents = obj->next_content;
  } else {
    OBJ_DATA *prev;

    for (prev = in_room->contents; prev; prev = prev->next_content) {
      if (prev->next_content == obj) {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL) {
      bug("Obj_from_room: obj not found.", 0);
      return;
    }
  }

  obj->in_room = NULL;
  obj->next_content = NULL;
  return;
}

/*
 * Move an obj into a room.
 */
void obj_to_room(OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex) {

  // free_string(obj->adjust_string);

  // obj->adjust_string = str_dup("");

  obj->next_content = pRoomIndex->contents;
  pRoomIndex->contents = obj;
  if (obj->next_content == obj)
    obj->next_content = NULL;
  obj->in_room = pRoomIndex;
  obj->carried_by = NULL;
  obj->in_obj = NULL;
  return;
}

/*
 * Move an object into an object.
 */
void obj_to_obj(OBJ_DATA *obj, OBJ_DATA *obj_to) {
  obj->next_content = obj_to->contains;
  obj_to->contains = obj;
  obj->in_obj = obj_to;
  if (obj->in_obj == obj) {
    obj->in_obj = NULL;
    obj->contains = NULL;
  }
  if (obj->next_content == obj)
    obj->next_content = NULL;

  obj->in_room = NULL;
  obj->carried_by = NULL;
  if (obj_to->pIndexData->vnum == OBJ_VNUM_PIT)
    obj->cost = 0;

  for (; obj_to != NULL; obj_to = obj_to->in_obj) {
    if (obj_to->carried_by != NULL) {
      obj_to->carried_by->carry_number += get_obj_number(obj);
    }
  }

  return;
}

/*
 * Move an object out of an object.
 */
void obj_from_obj(OBJ_DATA *obj) {
  OBJ_DATA *obj_from;

  if ((obj_from = obj->in_obj) == NULL) {
    bug("Obj_from_obj: null obj_from.", 0);
    return;
  }

  if (obj == obj_from->contains) {
    obj_from->contains = obj->next_content;
  } else {
    OBJ_DATA *prev;

    for (prev = obj_from->contains; prev; prev = prev->next_content) {
      if (prev->next_content == obj) {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if (prev == NULL) {
      bug("Obj_from_obj: obj not found.", 0);
      bug(obj->description, 0);
      return;
    }
  }

  obj->next_content = NULL;
  obj->in_obj = NULL;

  for (; obj_from != NULL; obj_from = obj_from->in_obj) {
    if (obj_from->carried_by != NULL) {
      obj_from->carried_by->carry_number -= get_obj_number(obj);
    }
  }

  return;
}

/*
 * Extract an obj from the world.
 */
void extract_obj(OBJ_DATA *obj) {
  OBJ_DATA *obj_content;
  OBJ_DATA *obj_next;

  if (obj->pIndexData == NULL) {
    object_list.remove(obj);
    free_obj(obj);
  }

  if (obj->in_room != NULL)
    obj_from_room(obj);
  else if (obj->carried_by != NULL)
    obj_from_char(obj);
  else if (obj->in_obj != NULL)
    obj_from_obj(obj);

  for (obj_content = obj->contains; obj_content; obj_content = obj_next) {
    obj_next = obj_content->next_content;
    extract_obj(obj_content);
  }

  object_list.remove(obj);

  --obj->pIndexData->count;
  free_obj(obj);
  return;
}

void extract_obj_silent(OBJ_DATA *obj) {
  OBJ_DATA *obj_content;
  OBJ_DATA *obj_next;

  if (obj->pIndexData == NULL) {
    object_list.remove(obj);
    free_obj(obj);
  }

  if (obj->in_room != NULL)
    obj_from_room(obj);
  else if (obj->carried_by != NULL)
    obj_from_char_silent(obj);
  else if (obj->in_obj != NULL)
    obj_from_obj(obj);

  for (obj_content = obj->contains; obj_content; obj_content = obj_next) {
    obj_next = obj_content->next_content;
    extract_obj_silent(obj_content);
  }

  object_list.remove(obj);

  --obj->pIndexData->count;
  free_obj(obj);
  return;
}

void clear_pointers(CHAR_DATA *ch) {

  for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
       it != FacVect.end(); ++it) {
    if ((*it)->ritual_orderer == ch)
      (*it)->ritual_orderer = NULL;
  }

  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    CHAR_DATA *victim = *it;

    if (victim == NULL || victim == ch)
      continue;

    if (victim->next_observing_room == ch)
      victim->next_observing_room = NULL;
    if (victim->master == ch)
      victim->master = NULL;
    if (victim->reply == ch)
      victim->reply = NULL;
    if (victim->possessing == ch)
      victim->possessing = NULL;
    if (victim->fight_current == ch)
      victim->fight_current = NULL;
    if (victim->fight_next == ch)
      victim->fight_next = NULL;
    if (victim->diminish_offer == ch)
      victim->diminish_offer = NULL;
    if (victim->dragging == ch)
      victim->dragging = NULL;
    if (victim->cfighting == ch)
      victim->cfighting = NULL;
    if (victim->lifting == ch)
      victim->lifting = NULL;
    if (victim->dream_dragging == ch)
      victim->dream_dragging = NULL;
    if (victim->delay_target == ch)
      victim->delay_target = NULL;
    if (victim->afraid_of == ch)
      victim->afraid_of = NULL;
    if (victim->underpowering == ch)
      victim->underpowering = NULL;
    if (victim->last_hit_by == ch)
      victim->last_hit_by = NULL;
    if (victim->mimic == ch)
      victim->mimic = NULL;
    if (victim->cloaked == ch)
      victim->cloaked = NULL;
    if (victim->reflect == ch)
      victim->reflect = NULL;
    if (victim->commanded == ch)
      victim->commanded = NULL;
    if (victim->your_car == ch)
      victim->your_car = NULL;
    if (victim->fistfighting == ch)
      victim->fistfighting = NULL;
    if (victim->target == ch)
      victim->target = NULL;
    if (victim->target_2 == ch)
      victim->target_2 = NULL;
    if (victim->target_3 == ch)
      victim->target_3 = NULL;
    if (victim->chattacking == ch)
      victim->chattacking = NULL;

    if (!IS_NPC(victim)) {
      if (victim->pcdata->destiny_offer_char == ch)
        victim->pcdata->destiny_offer_char = NULL;
      if (victim->pcdata->victimize_vic_point == ch)
        victim->pcdata->victimize_vic_point = NULL;
      if (victim->pcdata->victimize_char_point == ch)
        victim->pcdata->victimize_char_point = NULL;
      if (victim->pcdata->patrol_target == ch)
        victim->pcdata->patrol_target = NULL;
      if (victim->pcdata->prep_target == ch)
        victim->pcdata->prep_target = NULL;
      if (victim->pcdata->scheme_requester == ch)
        victim->pcdata->scheme_requester = NULL;
      if (victim->pcdata->wardrobe_pointer == ch)
        victim->pcdata->wardrobe_pointer = NULL;
      if (victim->pcdata->tracing == ch)
        victim->pcdata->tracing = NULL;
      if (victim->pcdata->process_target == ch)
        victim->pcdata->process_target = NULL;
      if (victim->pcdata->dream_link == ch)
        victim->pcdata->dream_link = NULL;
      if (victim->pcdata->bond_offer == ch)
        victim->pcdata->bond_offer = NULL;
      if (victim->pcdata->offering == ch)
        victim->pcdata->offering = NULL;
      if (victim->pcdata->imprinter == ch)
        victim->pcdata->imprinter = NULL;
      if (victim->pcdata->persuade_target == ch)
        victim->pcdata->persuade_target = NULL;
      if (victim->pcdata->encounter_sr == ch)
        victim->pcdata->encounter_sr = NULL;
      if (victim->pcdata->protecting == ch)
        victim->pcdata->protecting = NULL;
      if (victim->pcdata->sexing == ch)
        victim->pcdata->sexing = NULL;
      if (victim->pcdata->connected_to == ch)
        victim->pcdata->connected_to = NULL;
      if (victim->pcdata->sr_connection == ch)
        victim->pcdata->sr_connection = NULL;
      if (victim->pcdata->cansee == ch)
        victim->pcdata->cansee = NULL;
      if (victim->pcdata->cansee2 == ch)
        victim->pcdata->cansee2 = NULL;
      if (victim->pcdata->cansee3 == ch)
        victim->pcdata->cansee3 = NULL;
      if (victim->pcdata->patrol_target == ch)
        victim->pcdata->patrol_target = NULL;
      if (victim->pcdata->luck_character == ch)
        victim->pcdata->luck_character = NULL;
      if (victim->pcdata->attempt_character == ch)
        victim->pcdata->attempt_character = NULL;
      if(victim->pcdata->cam_spy_char == ch)
        victim->pcdata->cam_spy_char = NULL;
      if(victim->pcdata->narrative_query_char == ch)
        victim->pcdata->narrative_query_char = NULL;
      if(victim->pcdata->summary_target == ch)
        victim->pcdata->summary_target = NULL;


    }
  }
}

/*
 * Extract a char from the world.
 */
void extract_char(CHAR_DATA *ch, bool fPull) {
  char buf[MSL];
  CHAR_DATA *wch;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  clear_pointers(ch);

  /* doesn't seem to be necessary */
  if (ch->in_room == NULL) {
    bug("Extract_char: NULL.", 0);
    sprintf(buf, "%s", ch->name);
    bug(buf, 0);
    return;
  }

  if (fPull)
    die_follower(ch);

  stop_fighting(ch, TRUE);

  for (obj = ch->carrying; obj != NULL; obj = obj_next) {
    obj_next = obj->next_content;
    extract_obj_silent(obj);
  }

  if (ch->in_room != NULL)
    char_from_room(ch);

  char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));

  if ((*(char_list.end())) == ch) {
    ch->ttl = 0;
    return;
  }

  if (ch->in_room != NULL)
    char_from_room(ch);

  /* Death room */
  if (!fPull) {
    char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
    return;
  }

  if (IS_NPC(ch))
    --ch->pIndexData->count;

  if (ch->desc != NULL && ch->desc->original != NULL) {
    do_function(ch, &do_return, "");
    ch->desc = NULL;
  }

  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->reply == ch)
      wch->reply = NULL;
  }

  char_list.remove(ch);

  if (ch->desc != NULL)
    ch->desc->character = NULL;
  free_char(ch);
  return;
}

/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room(CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MSL];
  CharList *l;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;

  if (!str_cmp(arg, "self") || !str_cmp(arg, "me"))
    return ch;

  if (room && ch) {
    bug("get_char_room received room AND ch", 0);
    return NULL;
  }

  if (room == NULL && ch->in_room == NULL)
    return NULL;

  if (room)
    l = room->people;
  else
    l = ch->in_room->people;

  for (CharList::iterator it = l->begin(); it != l->end(); ++it) {
    CHAR_DATA *rch = *it;

    //	if( !IS_NPC(rch))
    remove_color(temp, PERS(rch, ch));

    if (ch &&
        ((!can_see(ch, rch) && in_fight(ch) == FALSE) || (!is_name(arg, temp))))
      continue;
    if (++count == number)
      return rch;
  }
  if (!IS_NPC(ch))
    ch->pcdata->paranoid += 100;
  return NULL;
}

CHAR_DATA *get_char_id(long id) {
  CHAR_DATA *ch;

  if (id < 0) {
    bug("get_char_id: bad ID", 0);
    return NULL;
  }

  for (DescList::iterator it = descriptor_list.begin();
       it != descriptor_list.end(); ++it) {
    if ((*it)->connected != CON_PLAYING)
      continue;

    ch = CH(*it);

    if (ch != NULL && ch->id == id)
      return ch;
  }

  return NULL;
}

// variation of get_char_world that doesn't count self - Discordance
CHAR_DATA *get_victim_world(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;
  int number;
  int count;

  if (ch && (wch = get_char_room(ch, NULL, argument)) != NULL && ch != wch &&
      !IS_NPC(wch))
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (IS_NPC(wch))
      continue;

    if (ch == wch)
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp) || str_cmp(arg, wch->name))
      continue;

    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (IS_NPC(wch))
      continue;

    if (ch == wch)
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp))
      continue;

    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

    if (++count == number)
      return wch;
  }
  /*
      count = 0;
      for(CharList::iterator it = char_list.begin(); it != char_list.end();
     ++it)
      {
          wch = *it;

          if(wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
     { continue;
          }

              if(!IS_NPC(wch))
                  continue;
          if( ch )
              remove_color(temp, PERS(wch, ch));
          else
              sprintf(temp, "%s", wch->name);

          if ( wch->in_room == NULL || ( ch && !can_see( ch, wch ) ) ||
     !is_name( arg, temp ) ) continue;

          if(IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
              continue;

          if ( ++count == number )
              return wch;
      }
  */
  return NULL;
}

/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;
  int number;
  int count;
  if (ch && (wch = get_char_room(ch, NULL, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (IS_NPC(wch))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || !is_name(arg, temp) || str_cmp(arg, wch->name))
      continue;

    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (IS_NPC(wch))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (!IS_NPC(wch))
      continue;
    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

    if (++count == number)
      return wch;
  }
  return NULL;
}

CHAR_DATA *get_char_world_pc_noname(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch = NULL;
  int number;
  int count;
  if (ch == NULL)
    return NULL;

  if (ch && (wch = get_char_room(ch, NULL, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (IS_NPC(wch))
      continue;
    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp) || str_cmp(arg, wch->name))
      continue;

    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;
    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;

    if (IS_NPC(wch))
      continue;
    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);
    if (wch->in_room == NULL || !is_name(arg, temp))
      continue;
    if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;
    if (++count == number)
      return wch;
  }

  return NULL;
}

CHAR_DATA *get_char_world_true(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;
  int number;
  int count;
  if (ch && (wch = get_char_room(ch, NULL, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp) || str_cmp(arg, wch->name))
      continue;

    if (IS_FLAG(wch->act, PLR_DEAD)) {
      if (!IS_IMMORTAL(ch))
        continue;
    }
    if (wch->in_room != NULL && wch->in_room->vnum < 100)
      continue;
    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;

    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp))
      continue;

    if (IS_FLAG(wch->act, PLR_DEAD)) {
      if (!IS_IMMORTAL(ch))
        continue;
    }
    if (wch->in_room != NULL && wch->in_room->vnum < 100)
      continue;
    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (!IS_NPC(wch))
      continue;
    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp))
      continue;

    if (IS_FLAG(wch->act, PLR_DEAD)) {
      if (!IS_IMMORTAL(ch))
        continue;
    }
    if (wch->in_room != NULL && wch->in_room->vnum < 100)
      continue;

    if (++count == number)
      return wch;
  }
  return NULL;
}

CHAR_DATA *get_char_world_new(char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;

  argument = one_argument_nouncap(argument, arg);
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || str_cmp(arg, temp))
      continue;

    return wch;
  }
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (!IS_NPC(wch))
      continue;
    sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || str_cmp(arg, temp))
      continue;

    return wch;
  }
  return NULL;
}

CHAR_DATA *get_char_world_pc(char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;

  argument = one_argument_nouncap(argument, arg);
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (!str_cmp(wch->name, arg))
      return wch;

    sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || str_cmp(arg, temp))
      continue;

    return wch;
  }
  return NULL;
}

CHAR_DATA *get_char_world_account(char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;

  argument = one_argument_nouncap(argument, arg);
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (wch->pcdata->account == NULL)
      continue;

    sprintf(temp, "%s", wch->pcdata->account->name);

    if (wch->in_room == NULL || str_cmp(arg, temp))
      continue;
    return wch;
  }
  return NULL;
}

CHAR_DATA *get_char_haven(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;
  int number;
  int count;
  if (ch && (wch = get_char_room(ch, NULL, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || !is_name(arg, temp) || str_cmp(arg, wch->name))
      continue;

    if (IS_FLAG(wch->act, PLR_DEAD)) {
      if (!IS_IMMORTAL(ch))
        continue;
    }
    if (wch->in_room != NULL && wch->in_room->vnum < 100)
      continue;

    if (wch->in_room != NULL && !in_haven(wch->in_room))
      continue;

    if (++count == number)
      return wch;
  }
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

    if (IS_FLAG(wch->act, PLR_DEAD)) {
      if (!IS_IMMORTAL(ch))
        continue;
    }
    if (wch->in_room != NULL && wch->in_room->vnum < 100)
      continue;

    if (wch->in_room != NULL && !in_haven(wch->in_room))
      continue;

    if (++count == number)
      return wch;
  }
  return NULL;
}

CHAR_DATA *get_char_vision(CHAR_DATA *ch, ROOM_INDEX_DATA *room,
                           char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *wch;
  int number;
  int count;

  if (is_dreaming(ch)) {
    return get_char_dream(ch, argument);
  }

  if (ch && (wch = get_char_room(ch, NULL, argument)) != NULL)
    return wch;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (IS_NPC(wch))
      continue;

    if (wch->in_room != ch->in_room &&
        !can_see_char_distance(ch, wch, DISTANCE_MEDIUM))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp))
      continue;

    if (++count == number)
      return wch;
  }
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    wch = *it;
    if (wch->race < 0 || wch->race > 200 || wch->sex < 0 || wch->sex > 10)
      continue;
    if (!IS_NPC(wch))
      continue;

    if (wch->in_room != ch->in_room &&
        !can_see_char_distance(ch, wch, DISTANCE_MEDIUM))
      continue;

    if (ch)
      remove_color(temp, PERS(wch, ch));
    else
      sprintf(temp, "%s", wch->name);

    if (wch->in_room == NULL || (ch && !can_see(ch, wch)) ||
        !is_name(arg, temp))
      continue;

    if (++count == number)
      return wch;
  }
  return NULL;
}

/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type(OBJ_INDEX_DATA *pObjIndex) {
  for (ObjList::iterator it = object_list.begin(); it != object_list.end();
       ++it) {
    if ((*it)->pIndexData == pObjIndex)
      return *it;
  }

  return NULL;
}

/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list(CHAR_DATA *ch, char *argument, OBJ_DATA *list) {
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = list; obj != NULL; obj = obj->next_content) {
    if (can_see_obj(ch, obj) && is_name(arg, obj->name)) {
      if (++count == number)
        return obj;
    } else if (arg[0] == '\0' && ++count == number)
      return obj;
  }

  return NULL;
}

/*
 * Find an obj in player's inventory.
 */

OBJ_DATA *get_obj_carry(CHAR_DATA *ch, char *argument, CHAR_DATA *viewer) {
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
    if (obj->wear_loc == WEAR_NONE &&
        (viewer ? can_see_obj(viewer, obj) : TRUE) && is_name(arg, obj->name) &&
        !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

OBJ_DATA *get_obj_carryhold(CHAR_DATA *ch, char *argument, CHAR_DATA *viewer) {
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
    if ((obj->wear_loc == WEAR_NONE || obj->wear_loc == WEAR_HOLD ||
         obj->wear_loc == WEAR_HOLD_2) &&
        (viewer ? can_see_obj(viewer, obj) : TRUE) && is_name(arg, obj->name) &&
        !IS_SET(obj->extra_flags, ITEM_WARDROBE)) {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear(CHAR_DATA *ch, char *argument, bool character) {
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument(argument, arg);
  count = 0;
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
    if (obj->wear_loc != WEAR_NONE &&
        (character ? can_see_obj(ch, obj) : TRUE) && is_name(arg, obj->name)) {
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Find an obj in the room or in inventory.
 */
/*
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    obj = get_obj_list( ch, argument, ch->in_room->contents );
    if ( obj != NULL )
        return obj;

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL )
        return obj;

    if ( ( obj = get_obj_wear( ch, argument, TRUE ) ) != NULL )
        return obj;

    return NULL;
}*/

OBJ_DATA *get_obj_here(CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument) {
  OBJ_DATA *obj;
  int number, count;
  char arg[MAX_INPUT_LENGTH];

  if (ch && room) {
    bug("get_obj_here received a ch and a room", 0);
    return NULL;
  }

  number = number_argument(argument, arg);
  count = 0;

  if (ch) {
    obj = get_obj_list(ch, argument, ch->in_room->contents);
    if (obj != NULL)
      return obj;

    if ((obj = get_obj_carry(ch, argument, ch)) != NULL)
      return obj;

    if ((obj = get_obj_wear(ch, argument, TRUE)) != NULL)
      return obj;
  } else {
    for (obj = room->contents; obj; obj = obj->next_content) {
      if (!is_name(arg, obj->name))
        continue;
      if (++count == number)
        return obj;
    }
  }

  return NULL;
}

/*
 * Find an obj in the world.
 */
/*
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int number;
    int count;

    if ( ( obj = get_obj_here( ch, NULL, argument ) ) != NULL )
        return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for (ObjList::iterator it = object_list.begin();
        it != object_list.end(); ++it)
    {
        if ( can_see_obj( ch, *it ) && is_name( arg, (*it)->name ) )
        {
            if ( ++count == number )
                return *it;
        }
    }

    return NULL;
}
*/

OBJ_DATA *get_obj_world(CHAR_DATA *ch, char *argument) {
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int number;
  int count;

  if (ch && (obj = get_obj_here(ch, NULL, argument)) != NULL)
    return obj;

  number = number_argument(argument, arg);
  count = 0;
  for (ObjList::iterator it = object_list.begin(); it != object_list.end();
       ++it) {
    if ((ch && !can_see_obj(ch, *it)) || !is_name(arg, (*it)->name))
      continue;
    if (++count == number)
      return *it;
  }

  return NULL;
}

/* deduct cost from a character */

void deduct_cost(CHAR_DATA *ch, int cost, int country) {
  ch->money -= cost;

  return;
}

/*
 * Create a 'money' obj.
 *
 * Modified on December 26th, 2001 by Palin elaurin@umich.edu
 *
 * This function should correctly split the individual coins into their
 * respective types(silver, gold, copper, andoran marks etc...) as well as
 * give the new coin the proper 'Nation' setting for the value[1] coordinate.
 *
 * This nation value is used to find the weights/values of the respective coins.
 */
OBJ_DATA *create_money(int amount, CHAR_DATA *ch) {
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj = NULL;

  if (amount <= 0) {
    //	bug( "Create_money: zero or negative money.",amount);
    amount = UMAX(1, amount);
  }

  if (amount == 1) {
    obj = create_object(get_obj_index(OBJ_VNUM_MONEY), 0);
    obj->value[1] = 1;
  } else {
    obj = create_object(get_obj_index(OBJ_VNUM_MONEY_S), 0);
    obj->value[1] = amount;
    sprintf(buf, obj->short_descr, amount / 100);
    free_string(obj->short_descr);
    obj->short_descr = str_dup(buf);
    obj->cost = amount;
  }
  if (ch->played / 3600 < 5) {
    obj->level = 99;
  }
  free_string(obj->material);
  obj->material = str_dup(ch->name);
  return obj;
}

/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number(OBJ_DATA *obj) {
  int number;

  if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_MONEY ||
      obj->item_type == ITEM_JEWELRY)
    number = 0;
  else
    number = 1;

  for (obj = obj->contains; obj != NULL; obj = obj->next_content)
    number += get_obj_number(obj);

  return number;
}

bool is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
  if (room->owner == NULL || room->owner[0] == '\0')
    return FALSE;

  return is_name(ch->name, room->owner);
}

/*
 * True if room is private.
 */
bool room_is_private(ROOM_INDEX_DATA *pRoomIndex) {
  return FALSE;
  int count;

  if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0')
    return TRUE;

  count = pRoomIndex->people->size();

  if (IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE) && count >= 2)
    return TRUE;

  return FALSE;
}

/* visibility on a room -- for entering and exits */
bool can_see_room(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) {
  if (pRoomIndex->area->vnum == 31 &&
      (ch->in_room == NULL || ch->in_room->area->vnum != 31)) {
    if (!has_symbol_core(ch) && !has_symbol_cult(ch) && !has_symbol_sect(ch) && !has_other_symbol(ch) &&
        !has_goblinkey(ch)) {
      if (!college_staff(ch, FALSE) && !clinic_staff(ch, FALSE))
        return FALSE;
    }
  }
  if (pRoomIndex->area->world != ch->in_room->area->world &&
      pRoomIndex->area->world != WORLD_ELSEWHERE &&
      ch->in_room->area->world != WORLD_ELSEWHERE) {
    if (!has_symbol_core(ch) && !has_symbol_cult(ch) && !has_symbol_sect(ch) && !has_other_symbol(ch) &&
        !has_goblinkey(ch)) {
      if (prop_from_room(ch->in_room) != NULL)
        return FALSE;
    }
  }
  return TRUE;
}

/*
 * True if char can see victim.
 */
bool can_see(CHAR_DATA *ch, CHAR_DATA *victim) {
  /* RT changed so that WIZ_INVIS has levels */
  if (ch == victim)
    return TRUE;
  if (!strcmp(ch->name, "Discordance")) {
    return TRUE;
  }
  if(!IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) && victim->in_room != NULL && ch != victim && newbie_school(victim->in_room))
	return FALSE;


  if (!IS_NPC(ch) && !IS_NPC(victim) && !same_player(ch, victim) &&
      victim->pcdata->account != NULL &&
      IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return FALSE;

  if (!IS_NPC(ch) && !IS_NPC(victim) && !same_player(ch, victim) &&
      ch->pcdata->account != NULL &&
      IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return FALSE;

  if (ch->level < victim->invis_level && IS_FLAG(victim->act, PLR_ROOMINVIS) &&
      ch->in_room == victim->in_room)
    return FALSE;
  if (ch->level < victim->invis_level && IS_FLAG(victim->act, PLR_WHOINVIS) &&
      ch->in_room != victim->in_room)
    return FALSE;

  if (IS_IMMORTAL(ch))
    return TRUE;
  if (!IS_IMMORTAL(victim) && is_gm(victim))
    return TRUE;

  if (higher_power(ch) && pc_in_lair(victim))
    return FALSE;

  if (!IS_IMMORTAL(ch) && is_gm(ch) && pc_in_lair(victim))
    return FALSE;

   if(ch->in_room->vnum == GMHOME && victim->in_room->vnum == GMHOME)
	return FALSE;


  if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(ch->comm, COMM_MANDRAKE) &&
      !IS_FLAG(ch->act, PLR_SHROUD) && !guestmonster(ch))
    return FALSE;
  if (IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD) &&
      IS_NPC(victim))
    return FALSE;
  if (IS_FLAG(victim->act, PLR_DEEPSHROUD) && !IS_IMMORTAL(ch) &&
      !IS_FLAG(ch->act, PLR_DEEPSHROUD))
    return FALSE;

  if (!IS_NPC(ch) && !IS_NPC(victim) && !in_fight(ch) && !in_fight(victim)) {
    if (ch->pcdata->patrol_status >= 9 && ch->pcdata->patrol_status <= 11) {
      if (victim->pcdata->patrol_status < 9 ||
          victim->pcdata->patrol_status > 11)
        return FALSE;
    }
    if (ch->pcdata->patrol_status == 13 || ch->pcdata->patrol_status == 12) {
      if (victim->pcdata->patrol_status != 12 &&
          victim->pcdata->patrol_status != 13)
        return FALSE;
    }

    if (victim->pcdata->patrol_status == PATROL_HIDING &&
        ch->pcdata->patrol_status == PATROL_SEARCHING)
      return FALSE;
  }

  if (!IS_FLAG(ch->act, PLR_SHROUD) && is_sprite(victim))
    return FALSE;

  if (!IS_NPC(ch) && !str_cmp(victim->name, "unicorn") &&
      IS_FLAG(victim->affected_by, AFF_INVISIBLE) &&
      ((ch->pcdata->virginity_lost > 0) || get_skill(ch, SKILL_VIRGIN) == 0))
    return FALSE;

  if (is_dark(ch->in_room) && !can_see_dark(ch))
    return FALSE;

  if (is_blind(ch))
    return FALSE;

  if (!IS_NPC(ch) && IS_FLAG(victim->act, PLR_INVIS) && !IS_IMMORTAL(ch) &&
      !in_fight(victim))
    return FALSE;

  if (!IS_NPC(ch) && is_cloaked(victim) && ch->pcdata->paranoid >= 1000 &&
      public_room(ch->in_room) && !IS_IMMORTAL(ch))
    return FALSE;

  if (in_fight(ch) && !can_map_see(ch, victim))
    return FALSE;

  // added this so SRs can see ghosts and hidden people who trust them to
  // advanced - Discordance
  if (is_gm(ch) && !IS_IMMORTAL(ch) && get_gmtrust(ch, victim) > 0) {
    return TRUE;
  }

  if ((!IS_NPC(ch) && IS_FLAG(ch->act, PLR_HOLYLIGHT)) ||
      (IS_NPC(ch) && IS_IMMORTAL(ch)))
    return TRUE;

  if (is_ghost(victim) && is_gm(ch) && get_snooptrust(ch, victim) < 3 &&
      ch->in_room != victim->in_room)
    return FALSE;

  if (is_ghost(victim) && get_skill(ch, SKILL_CLAIRVOYANCE) < 2 &&
      !IS_FLAG(victim->comm, COMM_PRIVATE))
    return FALSE;

  if (is_ghost(victim) && is_possessing(victim))
    return FALSE;

  if (IS_AFFECTED(ch, AFF_BLIND) || IS_AFFECTED(ch, AFF_FLASH))
    return FALSE;

  if (has_caff(ch, CAFF_BLIND))
    return FALSE;
  if (has_caff(ch, CAFF_TEAR))
    return FALSE;

  if (room_is_dark(ch->in_room))
    return FALSE;

  if (IS_FLAG(victim->act, PLR_HIDE)) {
    if (get_skill(ch, SKILL_PERCEPTION) > get_skill(victim, SKILL_STEALTH))
      return TRUE;
    else if (!IS_NPC(ch) && ch->pcdata->cansee == victim)
      return TRUE;
    else if (!IS_NPC(ch) && ch->pcdata->cansee2 == victim)
      return TRUE;
    else if (!IS_NPC(ch) && ch->pcdata->cansee3 == victim)
      return TRUE;
    else
      return FALSE;
  }

  return TRUE;
}

bool room_visible(CHAR_DATA *ch) {
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
    if (victim->in_room != ch->in_room)
      continue;

    if (is_gm(victim))
      continue;

    if (higher_power(victim))
      continue;

    return can_see(victim, ch);
  }
  return TRUE;
}

/*
 * True if char can see obj.
 */
bool can_see_obj(CHAR_DATA *ch, OBJ_DATA *obj) {

  if (!IS_NPC(ch) && IS_FLAG(ch->act, PLR_HOLYLIGHT)) {
    return TRUE;
  }
  if (obj->item_type == ITEM_WEAPON && ch->in_room != NULL &&
      in_lodge(ch->in_room)) {
    return FALSE;
  }
  if (obj->item_type == ITEM_RANGED && ch->in_room != NULL &&
      in_lodge(ch->in_room)) {
    return FALSE;
  }
  if (obj->carried_by == ch) {
    return TRUE;
  }
  if (obj->pIndexData->vnum == 32 && obj->in_room != NULL &&
      !IS_FLAG(ch->act, PLR_SHROUD)) {
    return FALSE;
  }
  if (is_dark(ch->in_room) && !can_see_dark(ch) &&
      obj->item_type != ITEM_FLASHLIGHT) {
    return FALSE;
  }
  if (is_blind(ch)) {
    return FALSE;
  }
  if (IS_AFFECTED(ch, AFF_BLIND) && obj->item_type != ITEM_POTION) {
    return FALSE;
  }
  if (IS_AFFECTED(ch, AFF_FLASH) && obj->item_type != ITEM_POTION) {
    return FALSE;
  }
  if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0) {
    return TRUE;
  }
  if (IS_SET(obj->extra_flags, ITEM_INVIS)) {
    return FALSE;
  }
  if (IS_SET(obj->extra_flags, ITEM_HIDDEN)) {
    return FALSE;
  }

  return TRUE;
}

/*
 * True if char can drop obj.
 */
bool can_drop_obj(CHAR_DATA *ch, OBJ_DATA *obj) {
  if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
    return FALSE;
  if (!IS_SET(obj->extra_flags, ITEM_NODROP))
    return TRUE;

  if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL)
    return TRUE;

  return FALSE;
}

/*
 * Return ascii name of an affect location.
 */
const char *affect_loc_name(int location) {
  switch (location) {
  case APPLY_NONE:
    return "none";
  }

  bug("Affect_location_name: unknown location %d.", location);
  return "(unknown)";
}

/*
 * Return ascii name of an affect bit vector.
 * REMEBER! If your AFF_BIT is in AFFECTED2 status put it
 * in the table directly following this one.
 * This is only a temp thing until a better sol is avaliable.
 * Palin
 */
const char *affect_bit_name(int vector) {
  static char buf[512];

  buf[0] = '\0';
  if (vector & AFF_BLIND)
    strcat(buf, " blind");
  if (vector & AFF_FLASH)
    strcat(buf, " flash");
  if (vector & AFF_PHYS_BOOST)
    strcat(buf, " physical boost");
  if (vector & AFF_PENALTY)
    strcat(buf, " penalty");
  if (vector & AFF_BONUS)
    strcat(buf, " bonus");
  if (vector & AFF_FLESHFORMING)
    strcat(buf, " fleshforming");
  if (vector & AFF_PATHED)
    strcat(buf, " pathed");
  if (vector & AFF_MARKED)
    strcat(buf, " marked");
  if (vector & AFF_WET)
    strcat(buf, " wet");
  if (vector & AFF_REROLLED)
    strcat(buf, " rerolled");
  if (vector & AFF_MARKED2)
    strcat(buf, " marked2");
  if (vector & AFF_DSENSITIVE)
    strcat(buf, " desire sensitive");
  if (vector & AFF_UNBANISHED)
    strcat(buf, " unbanished");
  if (vector & AFF_SLOW)
    strcat(buf, " slow");
  if (vector & AFF_SHIELD)
    strcat(buf, " shield");
  if (vector & AFF_HEAL)
    strcat(buf, " heal");
  if (vector & AFF_DISORIENTATION)
    strcat(buf, " disorientation");
  if (vector & AFF_NOHEAL)
    strcat(buf, " noheal");
  if (vector & AFF_MUTE)
    strcat(buf, " mute");

  return (buf[0] != '\0') ? buf + 1 : "none";
}

/******************************************************************
 * Name: bit_array_name_lookup
 * Parameters:
 *      flag_type *table - table containing bit numbers and
 *		associated names
 *      int bit - the bit for which to find the name
 * Returns:
 *      char * - name of the specified bit
 *
 * Purpose:  Returns the name of the specified bit
 *****************************************************************/
const char *bit_array_name_lookup(const struct flag_type *table, int bit) {
  int x;

  for (x = 0; table[x].name != NULL; x++)
    if (table[x].bit == bit)
      return table[x].name;

  return "NotFound";
}

/******************************************************************
 * Name: act_bit_name_array
 * Parameters:
 *      SET s - SET containing the ACT bits that have been set
 * Returns:
 *      char * - space-seperated list of the names of all the bits
 *              in the set that have been set
 *
 * Purpose:  Returns a space-seperated list of the names of all
 *      the bits in the set that have been set
 *****************************************************************/
const char *act_bit_name_array(SET s) {
  static char buf[512];
  int x, y;

  buf[0] = '\0';

  for (x = 0; x < SETSIZE; x++) {
    for (y = 0; y < 32; y++) {
      if ((unsigned)s[x] >> y & 01) {
        if (IS_FLAG(s, ACT_IS_NPC))
          strcat(buf, bit_array_name_lookup(act_flags, (x * 32) + y));
        else
          strcat(buf, bit_array_name_lookup(plr_flags, (x * 32) + y));
        strcat(buf, " ");
      }
    }
  }

  return (buf[0] != '\0') ? buf : "none";
}

/******************************************************************
 * Name: comm_bit_name_array
 * Parameters:
 *      SET s - SET containing the COMM bits that have been set
 * Returns:
 *      char * - space-seperated list of the names of all the bits
 *		in the set that have been set
 *
 * Purpose:  Returns a space-seperated list of the names of all
 *	the bits in the set that have been set
 *****************************************************************/
const char *comm_bit_name_array(SET s) {
  static char buf[512];
  int x, y;

  buf[0] = '\0';

  for (x = 0; x < SETSIZE; x++) {
    for (y = 0; y < 32; y++) {
      if ((unsigned)s[x] >> y & 01) {
        strcat(buf, bit_array_name_lookup(comm_flags, (x * 32) + y));
        strcat(buf, " ");
      }
    }
  }

  return (buf[0] != '\0') ? buf : "none";
}

/******************************************************************
 * Name: affect_bit_name_array
 * Parameters:
 *      SET s - SET containing the AFFECT bits that have been set
 * Returns:
 *      char * - space-seperated list of the names of all the bits
 *              in the set that have been set
 *
 * Purpose:  Returns a space-seperated list of the names of all
 *      the bits in the set that have been set
 *****************************************************************/
const char *affect_bit_name_array(SET s) {
  static char buf[512];
  int x, y;

  buf[0] = '\0';

  for (x = 0; x < SETSIZE; x++) {
    for (y = 0; y < 32; y++) {
      if ((unsigned)s[x] >> y & 01) {
        strcat(buf, bit_array_name_lookup(affect_flags_bit, (x * 32) + y));
        strcat(buf, " ");
      }
    }
  }

  return (buf[0] != '\0') ? buf : "none";
}

/*
 * Return ascii name of extra flags vector.
 */
const char *extra_bit_name(int extra_flags) {
  static char buf[512];

  buf[0] = '\0';
  if (extra_flags & ITEM_LOCK)
    strcat(buf, " lock");
  if (extra_flags & ITEM_INVIS)
    strcat(buf, " invis");
  if (extra_flags & ITEM_NODROP)
    strcat(buf, " nodrop");
  if (extra_flags & ITEM_WARDROBE)
    strcat(buf, " wardrobe");
  if (extra_flags & ITEM_SCUBA)
    strcat(buf, " scuba");
  if (extra_flags & ITEM_COLLAR)
    strcat(buf, " collar");
  if (extra_flags & ITEM_FOCUS)
    strcat(buf, " focus");
  if (extra_flags & ITEM_SILENT)
    strcat(buf, " silent");
  if (extra_flags & ITEM_INVENTORY)
    strcat(buf, " inventory");
  if (extra_flags & ITEM_RELIC)
    strcat(buf, " relic");
  if (extra_flags & ITEM_POISON)
    strcat(buf, " poison");
  if (extra_flags & ITEM_ROHYPNOL)
    strcat(buf, " rohypnol");
  if (extra_flags & ITEM_SHROUD)
    strcat(buf, " shroud");
  if (extra_flags & ITEM_LARGE)
    strcat(buf, " large");
  if (extra_flags & ITEM_CURSED)
    strcat(buf, " cursed");
  if (extra_flags & ITEM_AMMOCHANGED)
    strcat(buf, " ammochanged");
  if (extra_flags & ITEM_ROT_TIMER)
    strcat(buf, " rot_timer");
  if (extra_flags & ITEM_WATERPROOF)
    strcat(buf, " waterproof");
  if (extra_flags & ITEM_VBLOOD)
    strcat(buf, " vblood");
  if (extra_flags & ITEM_HIDDEN)
    strcat(buf, " hidden");
  if (extra_flags & ITEM_ARMORED)
    strcat(buf, " armored");
  return (buf[0] != '\0') ? buf + 1 : "none";
}

const char *wear_bit_name(int wear_flags) {
  static char buf[512];

  buf[0] = '\0';
  if (wear_flags & ITEM_TAKE)
    strcat(buf, " take");
  if (wear_flags & ITEM_WEAR_BODY)
    strcat(buf, " torso");
  if (wear_flags & ITEM_HOLD)
    strcat(buf, " hold");
  if (wear_flags & ITEM_NO_SAC)
    strcat(buf, " nosac");
  return (buf[0] != '\0') ? buf + 1 : "none";
}

const char *form_bit_name(int form_flags) {
  static char buf[512];

  buf[0] = '\0';
  if (form_flags & FORM_POISON)
    strcat(buf, " poison");
  else if (form_flags & FORM_EDIBLE)
    strcat(buf, " edible");
  if (form_flags & FORM_MAGICAL)
    strcat(buf, " magical");
  if (form_flags & FORM_INSTANT_DECAY)
    strcat(buf, " instant_rot");
  if (form_flags & FORM_OTHER)
    strcat(buf, " other");
  if (form_flags & FORM_ANIMAL)
    strcat(buf, " animal");
  if (form_flags & FORM_SENTIENT)
    strcat(buf, " sentient");
  if (form_flags & FORM_UNDEAD)
    strcat(buf, " undead");
  if (form_flags & FORM_CONSTRUCT)
    strcat(buf, " construct");
  if (form_flags & FORM_MIST)
    strcat(buf, " mist");
  if (form_flags & FORM_INTANGIBLE)
    strcat(buf, " intangible");
  if (form_flags & FORM_BIPED)
    strcat(buf, " biped");
  if (form_flags & FORM_CENTAUR)
    strcat(buf, " centaur");
  if (form_flags & FORM_INSECT)
    strcat(buf, " insect");
  if (form_flags & FORM_SPIDER)
    strcat(buf, " spider");
  if (form_flags & FORM_CRUSTACEAN)
    strcat(buf, " crustacean");
  if (form_flags & FORM_WORM)
    strcat(buf, " worm");
  if (form_flags & FORM_BLOB)
    strcat(buf, " blob");
  if (form_flags & FORM_MAMMAL)
    strcat(buf, " mammal");
  if (form_flags & FORM_BIRD)
    strcat(buf, " bird");
  if (form_flags & FORM_REPTILE)
    strcat(buf, " reptile");
  if (form_flags & FORM_SNAKE)
    strcat(buf, " snake");
  if (form_flags & FORM_DRAGON)
    strcat(buf, " dragon");
  if (form_flags & FORM_AMPHIBIAN)
    strcat(buf, " amphibian");
  if (form_flags & FORM_FISH)
    strcat(buf, " fish");
  if (form_flags & FORM_COLD_BLOOD)
    strcat(buf, " cold_blooded");

  return (buf[0] != '\0') ? buf + 1 : "none";
}

const char *part_bit_name(int part_flags) {
  static char buf[512];

  buf[0] = '\0';
  if (part_flags & PART_HEAD)
    strcat(buf, " head");
  if (part_flags & PART_ARMS)
    strcat(buf, " arms");
  if (part_flags & PART_LEGS)
    strcat(buf, " legs");
  if (part_flags & PART_HEART)
    strcat(buf, " heart");
  if (part_flags & PART_BRAINS)
    strcat(buf, " brains");
  if (part_flags & PART_GUTS)
    strcat(buf, " guts");
  if (part_flags & PART_HANDS)
    strcat(buf, " hands");
  if (part_flags & PART_FEET)
    strcat(buf, " feet");
  if (part_flags & PART_FINGERS)
    strcat(buf, " fingers");
  if (part_flags & PART_EAR)
    strcat(buf, " ears");
  if (part_flags & PART_EYE)
    strcat(buf, " eyes");
  if (part_flags & PART_LONG_TONGUE)
    strcat(buf, " long_tongue");
  if (part_flags & PART_EYESTALKS)
    strcat(buf, " eyestalks");
  if (part_flags & PART_TENTACLES)
    strcat(buf, " tentacles");
  if (part_flags & PART_FINS)
    strcat(buf, " fins");
  if (part_flags & PART_WINGS)
    strcat(buf, " wings");
  if (part_flags & PART_TAIL)
    strcat(buf, " tail");
  if (part_flags & PART_CLAWS)
    strcat(buf, " claws");
  if (part_flags & PART_FANGS)
    strcat(buf, " fangs");
  if (part_flags & PART_HORNS)
    strcat(buf, " horns");
  if (part_flags & PART_SCALES)
    strcat(buf, " scales");

  return (buf[0] != '\0') ? buf + 1 : "none";
}

const char *weapon_bit_name(int weapon_flags) {
  static char buf[512];

  buf[0] = '\0';
  if (weapon_flags & WEAPON_FLAMING)
    strcat(buf, " flaming");
  if (weapon_flags & WEAPON_FROST)
    strcat(buf, " frost");
  if (weapon_flags & WEAPON_VAMPIRIC)
    strcat(buf, " vampiric");
  if (weapon_flags & WEAPON_SHARP)
    strcat(buf, " sharp");
  if (weapon_flags & WEAPON_VORPAL)
    strcat(buf, " vorpal");
  if (weapon_flags & WEAPON_TWO_HANDS)
    strcat(buf, " two-handed");
  if (weapon_flags & WEAPON_SHOCKING)
    strcat(buf, " shocking");
  if (weapon_flags & WEAPON_POISON)
    strcat(buf, " poison");

  return (buf[0] != '\0') ? buf + 1 : "none";
}

const char *cont_bit_name(int cont_flags) {
  static char buf[512];

  buf[0] = '\0';

  if (cont_flags & CONT_CLOSEABLE)
    strcat(buf, " closable");
  if (cont_flags & CONT_PICKPROOF)
    strcat(buf, " pickproof");
  if (cont_flags & CONT_CLOSED)
    strcat(buf, " closed");
  if (cont_flags & CONT_LOCKED)
    strcat(buf, " locked");

  return (buf[0] != '\0') ? buf + 1 : "none";
}

CHAR_DATA *get_char_range(CHAR_DATA *ch, char *argument, int range) {
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *scan_room;
  EXIT_DATA *pExit;
  int depth, door;

  if ((victim = get_char_room(ch, NULL, argument)) != NULL)
    return victim;

  for (door = 0; door < MAX_DIR; door++) {
    scan_room = ch->in_room;
    for (depth = 0; depth < range; depth++) {
      if ((pExit = scan_room->exit[door]) == NULL)
        break;

      if ((scan_room = pExit->u1.to_room) == NULL)
        break;

      //		if ( scan_room->vnum == ROOM_VNUM_VMAP )
      //		    break;

      if (scan_room->people == NULL)
        continue;

      if ((victim = get_char_room(NULL, scan_room, argument)) != NULL)
        return victim;
    }
  }
  return NULL;
}

int get_char_range_int(CHAR_DATA *ch, char *argument, int range) {
  CHAR_DATA *ach;
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *scan_room;
  EXIT_DATA *pExit;
  int depth, door;

  if ((ach = get_char_room(ch, NULL, argument)) != NULL)
    return 1;

  for (door = 0; door < 10; door++) {
    scan_room = ch->in_room;
    for (depth = 1; depth < range + 1; depth++) {
      if ((pExit = scan_room->exit[door]) == NULL)
        break;

      if ((scan_room = pExit->u1.to_room) == NULL)
        break;

      if (scan_room->people == NULL)
        continue;

      if ((victim = get_char_room(NULL, scan_room, argument)) == NULL)
        continue;

      return depth;
    }
  }
  return 0;
}

CHAR_DATA *get_char_area(CHAR_DATA *ch, char *argument) {
  char arg[MAX_INPUT_LENGTH];
  char temp[MIL];
  CHAR_DATA *ach;
  int number, count;

  if ((ach = get_char_room(ch, NULL, argument)) != NULL)
    return ach;

  number = number_argument(argument, arg);
  count = 0;
  for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
    ach = *it;

    if (ch && ach)
      remove_color(temp, PERS(ach, ch));

    if (ach->in_room == NULL)
      continue;
    if (ach->in_room->area->empty)
      continue;

    if (ach->in_room->area != ch->in_room->area || !can_see(ch, ach) ||
        !is_name(arg, temp))
      continue;

    if (++count == number)
      return ach;
  }

  return NULL;
}

/*
 * Config Color stuff
 */
void default_color(CHAR_DATA *ch) {
  if (IS_NPC(ch))
    return;

  if (!ch->pcdata)
    return;

  ch->pcdata->text[1] = (WHITE);
  ch->pcdata->auction[1] = (YELLOW);
  ch->pcdata->irl[1] = (YELLOW);
  ch->pcdata->implementor[1] = (YELLOW);
  ch->pcdata->ooc[1] = (YELLOW);
  ch->pcdata->pray[1] = (YELLOW);
  ch->pcdata->hero[1] = (YELLOW);
  ch->pcdata->hint[1] = (YELLOW);
  ch->pcdata->minioncolor[1] = (YELLOW);
  ch->pcdata->gossip[1] = (MAGENTA);
  ch->pcdata->immortal[1] = (WHITE);
  ch->pcdata->info[1] = (YELLOW);
  ch->pcdata->tells[1] = (GREEN);
  ch->pcdata->say[1] = (GREEN);
  ch->pcdata->osay[1] = (GREEN);
  ch->pcdata->newbie[1] = (YELLOW);
  ch->pcdata->yells[1] = (GREEN);
  ch->pcdata->reply[1] = (GREEN);
  ch->pcdata->gtell[1] = (GREEN);
  ch->pcdata->wiznet[1] = (GREEN);
  ch->pcdata->room_exits[1] = (GREEN);
  ch->pcdata->room_things[1] = (CYAN);
  ch->pcdata->prompt[1] = (CYAN);
  ch->pcdata->fight_death[1] = (RED);
  ch->pcdata->fight_yhit[1] = (GREEN);
  ch->pcdata->fight_ohit[1] = (YELLOW);
  ch->pcdata->fight_thit[1] = (RED);
  ch->pcdata->fight_skill[1] = (WHITE);
  ch->pcdata->text[0] = (NORMAL);
  ch->pcdata->auction[0] = (BRIGHT);
  ch->pcdata->irl[0] = (BRIGHT);
  ch->pcdata->implementor[0] = (BRIGHT);
  ch->pcdata->newbie[0] = (BRIGHT);
  ch->pcdata->ooc[0] = (BRIGHT);
  ch->pcdata->pray[0] = (BRIGHT);
  ch->pcdata->hint[0] = (BRIGHT);
  ch->pcdata->minioncolor[0] = (BRIGHT);
  ch->pcdata->hero[0] = (BRIGHT);
  ch->pcdata->yells[0] = (BRIGHT);
  ch->pcdata->osay[0] = (BRIGHT);
  ch->pcdata->gossip[0] = (NORMAL);
  ch->pcdata->info[0] = (NORMAL);
  ch->pcdata->say[0] = (NORMAL);
  ch->pcdata->tells[0] = (NORMAL);
  ch->pcdata->reply[0] = (NORMAL);
  ch->pcdata->gtell[0] = (NORMAL);
  ch->pcdata->wiznet[0] = (NORMAL);
  ch->pcdata->room_exits[0] = (NORMAL);
  ch->pcdata->room_things[0] = (NORMAL);
  ch->pcdata->prompt[0] = (NORMAL);
  ch->pcdata->fight_death[0] = (NORMAL);
  ch->pcdata->fight_yhit[0] = (NORMAL);
  ch->pcdata->fight_ohit[0] = (NORMAL);
  ch->pcdata->fight_thit[0] = (NORMAL);
  ch->pcdata->fight_skill[0] = (NORMAL);
  ch->pcdata->text[2] = 0;
  ch->pcdata->auction[2] = 0;
  ch->pcdata->irl[2] = 0;
  ch->pcdata->implementor[2] = 0;
  ch->pcdata->newbie[2] = 0;
  ch->pcdata->ooc[2] = 0;
  ch->pcdata->pray[2] = 0;
  ch->pcdata->hero[2] = 0;
  ch->pcdata->hint[2] = 0;
  ch->pcdata->minioncolor[2] = 0;
  ch->pcdata->yells[2] = 0;
  ch->pcdata->osay[2] = 0;
  ch->pcdata->gossip[2] = 0;
  ch->pcdata->immortal[2] = 0;
  ch->pcdata->info[2] = 1;
  ch->pcdata->say[2] = 0;
  ch->pcdata->tells[2] = 0;
  ch->pcdata->reply[2] = 0;
  ch->pcdata->gtell[2] = 0;
  ch->pcdata->wiznet[2] = 0;
  ch->pcdata->room_exits[2] = 0;
  ch->pcdata->room_things[2] = 0;
  ch->pcdata->prompt[2] = 0;
  ch->pcdata->fight_death[2] = 0;
  ch->pcdata->fight_yhit[2] = 0;
  ch->pcdata->fight_ohit[2] = 0;
  ch->pcdata->fight_thit[2] = 0;
  ch->pcdata->fight_skill[2] = 0;

  return;
}

void all_color(CHAR_DATA *ch, char *argument) {
  char buf[100];
  char buf2[100];
  int color;
  int bright;

  if (IS_NPC(ch) || !ch->pcdata)
    return;

  if (!*argument)
    return;

  if (!str_prefix(argument, "red")) {
    color = (RED);
    bright = NORMAL;
    sprintf(buf2, "Red");
  }
  if (!str_prefix(argument, "hi-red")) {
    color = (RED);
    bright = BRIGHT;
    sprintf(buf2, "Red");
  } else if (!str_prefix(argument, "green")) {
    color = (GREEN);
    bright = NORMAL;
    sprintf(buf2, "Green");
  } else if (!str_prefix(argument, "hi-green")) {
    color = (GREEN);
    bright = BRIGHT;
    sprintf(buf2, "Green");
  } else if (!str_prefix(argument, "yellow")) {
    color = (YELLOW);
    bright = NORMAL;
    sprintf(buf2, "Yellow");
  } else if (!str_prefix(argument, "hi-yellow")) {
    color = (YELLOW);
    bright = BRIGHT;
    sprintf(buf2, "Yellow");
  } else if (!str_prefix(argument, "blue")) {
    color = (BLUE);
    bright = NORMAL;
    sprintf(buf2, "Blue");
  } else if (!str_prefix(argument, "hi-blue")) {
    color = (BLUE);
    bright = BRIGHT;
    sprintf(buf2, "Blue");
  } else if (!str_prefix(argument, "magenta")) {
    color = (MAGENTA);
    bright = NORMAL;
    sprintf(buf2, "Magenta");
  } else if (!str_prefix(argument, "hi-magenta")) {
    color = (MAGENTA);
    bright = BRIGHT;
    sprintf(buf2, "Magenta");
  } else if (!str_prefix(argument, "cyan")) {
    color = (CYAN);
    bright = NORMAL;
    sprintf(buf2, "Cyan");
  } else if (!str_prefix(argument, "hi-cyan")) {
    color = (CYAN);
    bright = BRIGHT;
    sprintf(buf2, "Cyan");
  } else if (!str_prefix(argument, "grey")) {
    color = (WHITE);
    bright = NORMAL;
    sprintf(buf2, "White");
  } else if (!str_prefix(argument, "white")) {
    color = (WHITE);
    bright = BRIGHT;
    sprintf(buf2, "White");
  } else if (!str_prefix(argument, "black")) {
    color = (BLACK);
    bright = BRIGHT;
    sprintf(buf2, "White");
  } else {
    send_to_char("Unrecognized color, unchanged.\n\r", ch);
    return;
  }

  ch->pcdata->text[1] = color;
  ch->pcdata->auction[1] = color;
  ch->pcdata->gossip[1] = color;
  ch->pcdata->info[1] = color;
  ch->pcdata->say[1] = color;
  ch->pcdata->tells[1] = color;
  ch->pcdata->reply[1] = color;
  ch->pcdata->irl[1] = color;
  ch->pcdata->immortal[1] = color;
  ch->pcdata->implementor[1] = color;
  ch->pcdata->newbie[1] = color;
  ch->pcdata->ooc[1] = color;
  ch->pcdata->pray[1] = color;
  ch->pcdata->hint[1] = color;
  ch->pcdata->minioncolor[1] = color;
  ch->pcdata->hero[1] = color;
  ch->pcdata->yells[1] = color;
  ch->pcdata->osay[1] = color;
  ch->pcdata->gtell[1] = color;
  ch->pcdata->wiznet[1] = color;
  ch->pcdata->room_exits[1] = color;
  ch->pcdata->room_things[1] = color;
  ch->pcdata->prompt[1] = color;
  ch->pcdata->fight_death[1] = color;
  ch->pcdata->fight_yhit[1] = color;
  ch->pcdata->fight_ohit[1] = color;
  ch->pcdata->fight_thit[1] = color;
  ch->pcdata->fight_skill[1] = color;
  ch->pcdata->text[0] = bright;
  ch->pcdata->auction[0] = bright;
  ch->pcdata->gossip[0] = bright;
  ch->pcdata->info[0] = bright;
  ch->pcdata->say[0] = bright;
  ch->pcdata->irl[0] = bright;
  ch->pcdata->immortal[0] = bright;
  ch->pcdata->implementor[0] = bright;
  ch->pcdata->newbie[0] = bright;
  ch->pcdata->ooc[0] = bright;
  ch->pcdata->pray[0] = bright;
  ch->pcdata->hero[0] = bright;
  ch->pcdata->hint[0] = bright;
  ch->pcdata->minioncolor[0] = bright;
  ch->pcdata->yells[0] = bright;
  ch->pcdata->osay[0] = bright;
  ch->pcdata->tells[0] = bright;
  ch->pcdata->reply[0] = bright;
  ch->pcdata->gtell[0] = bright;
  ch->pcdata->wiznet[0] = bright;
  ch->pcdata->room_exits[0] = bright;
  ch->pcdata->room_things[0] = bright;
  ch->pcdata->prompt[0] = bright;
  ch->pcdata->fight_death[0] = bright;
  ch->pcdata->fight_yhit[0] = bright;
  ch->pcdata->fight_ohit[0] = bright;
  ch->pcdata->fight_thit[0] = bright;
  ch->pcdata->fight_skill[0] = bright;

  sprintf(buf, "All color settings set to %s.\n\r", buf2);
  send_to_char(buf, ch);

  return;
}

int sec_stat(CHAR_DATA *ch, int stat) { return 0; }

/**
 * This function returns how much money a person has in a specific type of
 * coin.  Usually it is used to find out how much copper a person has.
 *
 * @param *ch - Character whose money is being counted.
 * @param type - Type of coin we're seeing how much the character has of.
 * @param nation - Particular nation we're weighing the persons money in.
 *
 * @return How much money the character has of coin type.
 * @author Edward E. Laurin (Palin)
 */
long total_money(CHAR_DATA *ch, int type, int nation) { return ch->money; }

/**
 * This functions decides if a string is actually a type of coin.
 * Type being the following list.. not specific nations.
 *
 * @param argument - Incoming string to decide if its a type or not.
 * @return True/False - If the argument is a coin return true else return false.
 * @author Edward E. Laurin (Palin)
 */
bool is_coin_type(char *argument) { return TRUE; }

/**
 * This function takes a specific amount of cash away from a character.
 *
 * @param *ch - Character money is being taken from.
 * @param amount - Amount of money being taken away from character.
 * @param type_pay - Type of coin the character is paying in.
 * @param nation - Nation of the person character is paying.
 *
 * @return void
 * @author Edward E. Laurin (Palin)
 */
void pay_specific(CHAR_DATA *ch, long amount, int type_pay, int nation) {
  ch->money -= amount;
}

/*
 * This function lowers the condition of an object when it is 'hit'
 * or damaged.
 *
 * @param *obj - Object being damanged.
 * @param mod - Integer value, higher the easier it is to damage the object.
 *
 * @return void
 * @author Edward E. Laurin (Palin)
 */
void mod_eq_hp(OBJ_DATA *obj, int mod) {
  int chance = 0;
  if (mod == 0 || obj == NULL)
    return;

  /* You cannot harm a power-wrought item. */

  chance = number_range(1, (300 - obj->level));

  if (chance < (mod * 3))
    obj->condition = UMAX(1, obj->condition - 1);

  return;
}

/**
 * This function damages specific seletions of equipment.  It can either
 * damage all of a person's equipment or specific sections(such as the
 * equipment worn on the head).  This does not actually do the damage, but
 * directs the object to the mod_eq_hp function which does.
 *
 * @param *victim - Person who's armor is being damaged.
 * @param body_part - Location on the body where the damage is being taken.
 * @param dam - Amount of damage being taken.
 * @param mod - Modifier to determine how critical the damage is to that body
 * part.
 *
 * @return void
 * @author Edward E. Laurin (Palin)
 */
void damage_eq_body(CHAR_DATA *victim, int body_part, int dam, int mod) {
  return;
}

int calc_cfield(int size, char *str) {
  return size + (safe_strlen(str) - safe_strlen_color(str));
}

bool check_security(CHAR_DATA *ch, AREA_DATA *area) {

  if (IS_NPC(ch))
    return FALSE;

  if (ch->level < LEVEL_IMMORTAL)
    return FALSE;
  else
    return TRUE;

  return FALSE;
}

int get_true_weight(OBJ_DATA *obj) {
  int weight = 0;

  for (obj = obj->contains; obj != NULL; obj = obj->next_content)
    weight += obj->size;

  return weight;
}

OBJ_DATA *get_weapon(CHAR_DATA *ch) {
  if (get_held(ch, ITEM_WEAPON) == NULL) {
    return get_eq_char(ch, WEAR_HOLD);
  } else
    return get_held(ch, ITEM_WEAPON);
}

void declass(CHAR_DATA *ch) {
  if (UMAX(ch->pcdata->class_type, ch->pcdata->class_oldlevel) == 2) {
    ch->pcdata->classpoints = 30000;
    ch->pcdata->classtotal = 30000;
  }
  if (UMAX(ch->pcdata->class_type, ch->pcdata->class_oldlevel) == 3) {
    ch->pcdata->classpoints = 30000;
    ch->pcdata->classtotal = 30000;
  }
  if (UMAX(ch->pcdata->class_type, ch->pcdata->class_oldlevel) == 4) {
    ch->pcdata->classpoints = 30000;
    ch->pcdata->classtotal = 30000;
  }
  ch->pcdata->classtotal = ch->pcdata->classpoints;
  ch->pcdata->class_type = 1;
  ch->pcdata->exp_cap = 500000;
}

void re_race(CHAR_DATA *ch) {}

void logon_spirit(CHAR_DATA *victim) {
  if (!IS_FLAG(victim->act, PLR_NOSAVE))
    SET_FLAG(victim->act, PLR_NOSAVE);
  if (!IS_FLAG(victim->act, PLR_GHOST))
    SET_FLAG(victim->act, PLR_GHOST);
  if (IS_FLAG(victim->act, PLR_SINSPIRIT))
    REMOVE_FLAG(victim->act, PLR_SINSPIRIT);

  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  for (obj = victim->carrying; obj != NULL; obj = obj_next) {
    obj_next = obj->next_content;
    if (obj->wear_loc == WEAR_NONE)
      extract_obj(obj);
  }
}

bool no_doors(ROOM_INDEX_DATA *room) {
  int door;
  EXIT_DATA *pexit;
  for (door = 0; door <= 9; door++) {
    if ((pexit = room->exit[door]) != NULL && pexit->u1.to_room != NULL) {
      if (pexit->wall == WALL_NONE || pexit->wallcondition != WALLCOND_HOLE)
        return FALSE;
      if (IS_SET(pexit->exit_info, EX_ISDOOR))
        return FALSE;
    }
  }
  return TRUE;
}

void logon_char(CHAR_DATA *ch) {

  int minoffline;

  if (ch->activeat > 0)
    minoffline = (current_time - ch->activeat) / 60;
  else
    minoffline = (current_time - ch->lastlogoff) / 60;

  if (ch->pcdata->account == NULL && ch->desc->account != NULL)
    ch->pcdata->account = ch->desc->account;

  if (ch->pcdata->solitary_time > 0)
    ch->pcdata->solitary_time -= minoffline;

  char buf[MSL];


  re_race(ch);

  CHAR_DATA *original;
  for (DescList::iterator it = descriptor_list.begin();
       it != descriptor_list.end(); ++it) {
    DESCRIPTOR_DATA *d = *it;

    if (d->connected != CON_PLAYING)
      continue;
    original = CH(d);
    if (original == ch)
      continue;
    if (original == NULL || IS_NPC(original))
      continue;
    if (!str_cmp(original->name, ch->name) &&
        IS_FLAG(original->act, PLR_SINSPIRIT))
      logon_spirit(ch);
  }

  if (ch->pcdata->account != NULL) {
    if (ch->played / 3600 > ch->pcdata->account->maxhours)
      ch->pcdata->account->maxhours = ch->played / 3600;
  }

  if (ch->in_room != NULL && no_doors(ch->in_room) &&
      ch->in_room->area->vnum != 1 && ch->in_room->area->vnum > 12 &&
      ch->in_room->area->vnum < 26) {
    char buf[MSL];
    sprintf(buf, "No doors wakeup: %d", ch->in_room->vnum);
    if (is_undead(ch))
      take_lifeforce(ch, 600, buf);
    else
      take_lifeforce(ch, 1200, buf);
  }
  if(minoffline > 30)
     add_to_weekly_characters(ch);


  if (ch->race == RACE_WIGHT && minoffline > 240) {
    int decay = minoffline / 120;
    decay = UMIN(decay, 10);
    ch->pcdata->decay_timer = UMAX(4, ch->pcdata->decay_timer - decay);
    if (ch->pcdata->max_decay_timer > 10)
      ch->pcdata->max_decay_timer -= 2;
  }

  if (minoffline >= 5 && ch->pcdata->account != NULL &&
      IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NOTIFY)) {
    for (DescList::iterator it = descriptor_list.begin();
         it != descriptor_list.end(); ++it) {
      if ((*it)->connected != CON_PLAYING)
        continue;
      CHAR_DATA *to = CH(*it);
      if (are_friends(to, ch))
        printf_to_char(to, "%s has logged on.\n\r", ch->name);
    }
  }

  if (guestmonster(ch))
    ch->played = UMAX(0, ch->played - (3600 * 1));

  logonkeyscheck(ch);
  garage_update(ch, minoffline);

  place_car(ch, vehicle_location(ch));

  if (minoffline > 240) {
    ch->pcdata->last_rp_room = 0;
    if (IS_FLAG(ch->comm, COMM_MANDRAKE))
      REMOVE_FLAG(ch->comm, COMM_MANDRAKE);
    if (in_haven(ch->in_room))
      ch->pcdata->resident = 0;
    for (int i = 0; i < 25; i++) {
      if (ch->pcdata->imprint_type[i] == IMPRINT_COMPULSION &&
          ch->pcdata->imprint_pending[i] == 1) {
        ch->pcdata->imprint_type[i] = 0;
      }
      if (ch->pcdata->imprint_type[i] == IMPRINT_BODYCOMPULSION &&
          ch->pcdata->imprint_pending[i] == 1) {
        ch->pcdata->imprint_type[i] = 0;
      }
    }

    if (IS_FLAG(ch->comm, COMM_CLOAKED))
      REMOVE_FLAG(ch->comm, COMM_CLOAKED);
    if (IS_FLAG(ch->act, PLR_INVIS))
      REMOVE_FLAG(ch->act, PLR_INVIS);

    ch->pcdata->breach_timer = 0;
    ch->pcdata->breach_origin = 0;
    ch->pcdata->breach_destination = 0;


   if(ch->factiontrue < 1 && ch->faction != 0)
   {
	FACTION_TYPE *tfac = clan_lookup(ch->faction);
        if(tfac != NULL)
	{
	    if(tfac->type == FACTION_CULT && ch->fcult == 0)
	        ch->fcult = ch->faction;
	    if(tfac->type == FACTION_SECT && ch->fsect == 0)
	        ch->fsect = ch->faction;
	    if(tfac->type == FACTION_CORE && ch->fcore == 0)
	        ch->fcore = ch->faction;
	}
    }

    if (IS_FLAG(ch->comm, COMM_RESEARCHED))
      REMOVE_FLAG(ch->comm, COMM_RESEARCHED);

    if (IS_FLAG(ch->comm, COMM_EMPOWERED))
      REMOVE_FLAG(ch->comm, COMM_EMPOWERED);

    if (IS_FLAG(ch->comm, COMM_SHADOWCLOAK))
      REMOVE_FLAG(ch->comm, COMM_SHADOWCLOAK);
  }

  if (minoffline > 180 && !IS_FLAG(ch->act, PLR_BOUND) &&
      !trapped_room(ch->in_room, ch)) {
    ch->diminished = 0;
    if ((ch->pcdata->job_type_one == JOB_FULLCOMMUTE ||
         ch->pcdata->job_type_two == JOB_FULLCOMMUTE) &&
        number_percent() % 3 == 0) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * number_range(60, 150);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_COMMUTE;
      affect_to_char(ch, &af);
    } else if ((ch->pcdata->job_type_one == JOB_PARTCOMMUTE ||
                ch->pcdata->job_type_two == JOB_PARTCOMMUTE) &&
               number_percent() % 8 == 0) {
      AFFECT_DATA af;
      af.where = TO_AFFECTS;
      af.type = 0;
      af.level = 10;
      af.duration = 12 * number_range(60, 150);
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.caster = NULL;
      af.weave = FALSE;
      af.bitvector = AFF_COMMUTE;
      affect_to_char(ch, &af);
    }
  }

  if (ch->pcdata->job_type_one == JOB_FULLEMPLOY ||
      ch->pcdata->job_type_one == JOB_PARTEMPLOY) {
    bool found = FALSE;
    if (get_room_index(ch->pcdata->job_room_one) != NULL &&
        prop_from_room(get_room_index(ch->pcdata->job_room_one)) != NULL) {
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(ch->name,
                     prop_from_room(get_room_index(ch->pcdata->job_room_one))
                         ->workers[x]))
          found = TRUE;
      }
    }
    if (found == FALSE) {
      free_string(ch->pcdata->job_title_one);
      ch->pcdata->job_title_one = str_dup("");
      ch->pcdata->job_type_one = JOB_UNEMPLOYED;
    }
  }
  if (ch->pcdata->job_type_two == JOB_FULLEMPLOY ||
      ch->pcdata->job_type_two == JOB_PARTEMPLOY) {
    bool found = FALSE;
    if (get_room_index(ch->pcdata->job_room_two) != NULL &&
        prop_from_room(get_room_index(ch->pcdata->job_room_two)) != NULL) {
      for (int x = 0; x < 20; x++) {
        if (!str_cmp(ch->name,
                     prop_from_room(get_room_index(ch->pcdata->job_room_two))
                         ->workers[x]))
          found = TRUE;
      }
    }
    if (found == FALSE) {
      free_string(ch->pcdata->job_title_two);
      ch->pcdata->job_title_two = str_dup("");
      ch->pcdata->job_type_two = JOB_UNEMPLOYED;
    }
  }

  if (minoffline > 120 && ch->in_room != NULL && !higher_power(ch)) {
    if (ch->in_room->sector_type == SECT_FOREST && in_haven(ch->in_room)) {
      if (!IS_SET(ch->in_room->room_flags, ROOM_CAMPSITE) && !is_animal(ch) &&
          number_percent() % 3 == 0 && !IS_AFFECTED(ch, AFF_ABDUCTED) &&
          !is_helpless(ch)) {
        wound_char(ch, 1);
        message_to_char(ch->name,
                        "You are attacked by predators in the night.");
      }
      if (!IS_SET(ch->in_room->room_flags, ROOM_ANIMALHOME) && is_animal(ch) &&
          number_percent() % 5 == 0 && !IS_AFFECTED(ch, AFF_ABDUCTED) &&
          !is_helpless(ch)) {
        wound_char(ch, 1);
        message_to_char(ch->name,
                        "You are attacked by predators in the night.");
      }
    }
  }

  if (ch->in_room != NULL) {

    if (in_prop(ch) != NULL && in_prop(ch)->logoffs > 0) {
      in_prop(ch)->logoffs--;
      from_sleepers(ch, in_prop(ch));
    }

    EXTRA_DESCR_DATA *ed;
    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!sleepers", ed->keyword))
        break;
    }
    if (ed) {
      free_string(ed->description);
      ed->description = str_dup("");
    }
  }

  if (minoffline >= 25) {
    ch->hit = max_hp(ch);

    if (IS_FLAG(ch->comm, COMM_NONEWBIE))
      REMOVE_FLAG(ch->comm, COMM_NONEWBIE);
    if (IS_FLAG(ch->comm, COMM_NOGUIDE))
      REMOVE_FLAG(ch->comm, COMM_NOGUIDE);

    if (event_catastrophe == 1 && ch->wounds == 0) {
      ch->wounds = 1;
      ch->heal_timer = 30000;
    }
  }

  if (ch->pcdata->captive_timer > 0)
    ch->pcdata->captive_timer += minoffline * 6;

  if (minoffline >= 240) {
    ch->pcdata->availability = AVAIL_NORMAL;
    if (ch->pcdata->understandtoggle == 1) {
      ch->pcdata->understandtoggle = 0;
      free_string(ch->pcdata->understanding);
      ch->pcdata->understanding = str_dup("All");
    }
    ch->pcdata->fatigue = 0;
    ch->pcdata->fatigue_temp = 0;
    ch->pcdata->fatigue_temp = 0;

    ch->pcdata->without_sleep = 0;

    if (IS_FLAG(ch->comm, COMM_VBLOOD))
      REMOVE_FLAG(ch->comm, COMM_VBLOOD);

    if (IS_FLAG(ch->act, PLR_NOTICED))
      REMOVE_FLAG(ch->act, PLR_NOTICED);

    free_string(ch->pcdata->scent);
    ch->pcdata->scent = str_dup("");
    ch->pcdata->perfume_cost = 0;

    if (!IS_NPC(ch) && ch->in_room != NULL &&
        (ch->in_room->sector_type == SECT_PARK ||
         ch->in_room->sector_type == SECT_FOREST ||
         ch->in_room->sector_type == SECT_TUNNELS ||
         ch->in_room->sector_type == SECT_CAVE ||
         ch->in_room->sector_type == SECT_BASEMENT ||
         ch->in_room->sector_type == SECT_ROCKY)) {
      ch->pcdata->dirt[0] += 300;
    } else
      ch->pcdata->dirt[0] += 50;

  } else if (minoffline < 15)
    ch->pcdata->fatigue_temp = 10;

  if (ch->pcdata->account != NULL &&
      (ch->pcdata->account->xp > 200000 || ch->spentexp > 300000))
    sprintf(buf,
            "LOGONCOUNTMILLIONAIRE: %s MinOff: %d, Esteem: %d, RPXP %d+%d XP "
            "%d+%d, Influence: %d, Karma: %d, Cash: %ld+%ld\n\r",
            ch->name, minoffline, ch->esteem_faction, ch->pcdata->account->rpxp,
            ch->spentrpexp, ch->pcdata->account->xp, ch->spentexp,
            ch->pcdata->influence, available_karma(ch), ch->money / 100,
            ch->pcdata->total_money / 100);
  else
    sprintf(buf,
            "LOGONCOUNT: %s MinOff: %d, Esteem: %d, RPXP %d+%d XP %d+%d, "
            "Influence: %d, Karma: %d, Cash: %ld+%ld\n\r",
            ch->name, minoffline, ch->esteem_faction, ch->rpexp, ch->spentrpexp,
            ch->exp, ch->spentexp, ch->pcdata->influence, available_karma(ch),
            ch->money / 100, ch->pcdata->total_money / 100);
  log_string(buf);

  if (ch->fcore != 0 && minoffline > 360 && ch->esteem_faction > 0) {

    int pen = UMIN(20000, minoffline);
    if (get_age(ch) < 20)
      pen = pen * 3;
    if (get_age(ch) < 25)
      pen = pen * 3;
    pen /= 2;
    if (!in_haven(ch->in_room)) {
      give_respect(ch, -1 * pen / 45, "Logon Flat", ch->fcore);
      give_respect(ch, ch->esteem_faction * -1 / 300, "Logon Percent", ch->fcore);
    } else {
      give_respect(ch, -1 * pen / 75, "Logon Flat", ch->fcore);
      give_respect(ch, ch->esteem_faction * -1 / 300, "Logon Percent", ch->fcore);
    }
  }
  if (ch->fsect != 0 && minoffline > 360 && ch->esteem_sect > 0) {

    int pen = UMIN(20000, minoffline);
    if (get_age(ch) < 20)
      pen = pen * 3;
    if (get_age(ch) < 25)
      pen = pen * 3;
    pen /= 2;
    if (!in_haven(ch->in_room)) {
      give_respect(ch, -1 * pen / 45, "Logon Flat", ch->fsect);
      give_respect(ch, ch->esteem_sect * -1 / 300, "Logon Percent", ch->fsect);
    } else {
      give_respect(ch, -1 * pen / 75, "Logon Flat", ch->fsect);
      give_respect(ch, ch->esteem_sect * -1 / 300, "Logon Percent", ch->fsect);
    }
  }
  if (ch->fcult != 0 && minoffline > 360 && ch->esteem_cult > 0) {

    int pen = UMIN(20000, minoffline);
    if (get_age(ch) < 20)
      pen = pen * 3;
    if (get_age(ch) < 25)
      pen = pen * 3;
    pen /= 2;
    if (!in_haven(ch->in_room)) {
      give_respect(ch, -1 * pen / 45, "Logon Flat", ch->fcult);
      give_respect(ch, ch->esteem_cult * -1 / 300, "Logon Percent", ch->fcult);
    } else {
      give_respect(ch, -1 * pen / 75, "Logon Flat", ch->fcult);
      give_respect(ch, ch->esteem_cult * -1 / 300, "Logon Percent", ch->fcult);
    }
  }


  if (ch->esteem_faction < 0) {
    give_respect(ch, ch->esteem_faction * -1, "", ch->fcore);
  }
 if (ch->esteem_sect < 0) {
    give_respect(ch, ch->esteem_sect * -1, "", ch->fsect);
  }
 if (ch->esteem_cult < 0) {
    give_respect(ch, ch->esteem_cult * -1, "", ch->fcult);
  }


  if (minoffline > 240) {
    influence_update(ch);
    ch->pcdata->habits_done = 1;
  }

  if (ch->pcdata->stories_countdown > 0)
    ch->pcdata->stories_countdown =
        UMAX(1, ch->pcdata->stories_countdown - minoffline * 12);
  if (ch->pcdata->rooms_countdown > 0)
    ch->pcdata->rooms_countdown =
        UMAX(1, ch->pcdata->rooms_countdown - minoffline * 12);

  ch->pcdata->conditions[COND_DRUNK] -= minoffline / 2;
  ch->pcdata->conditions[COND_DRUNK] =
      UMAX(0, ch->pcdata->conditions[COND_DRUNK]);

  if (ch->pcdata->order_timer > 0) {
    ch->pcdata->order_timer -= minoffline * 12;
    ch->pcdata->order_timer = UMAX(ch->pcdata->order_timer, 1);
  }
  // Added some commented GODLYSEX checks in here - Discordance
  if (ch->wounds == 1) {
    if (get_skill(ch, SKILL_REGEN) >= 3) {
      ch->heal_timer = UMAX(1, ch->heal_timer - minoffline * 12 * 84);
    }
    // This raises the effective regen of regen 1 to regen 3 if the character
    // recently had sex with a demigod Added check incase regen < 1, but
    // character had sex with a demigod recently
    else if (get_skill(ch, SKILL_REGEN) >= 1) {
      ch->heal_timer = UMAX(1, ch->heal_timer - minoffline * 12 * 24);
    } else
      ch->heal_timer = UMAX(1, ch->heal_timer - minoffline * 24);
  }
  if (ch->wounds == 2 && (in_hospital(ch) || get_skill(ch, SKILL_REGEN) >= 1)) {
    if (get_skill(ch, SKILL_REGEN) >= 3) {
      ch->heal_timer = UMAX(1, ch->heal_timer - minoffline * 12 * 37);
    }
    // Regen 1 and demigod sex
    // Regen 1 or demigod sex
    else if (get_skill(ch, SKILL_REGEN) >= 1) {
      ch->heal_timer = UMAX(1, ch->heal_timer - minoffline * 12 * 7);
    } else
      ch->heal_timer = UMAX(1, ch->heal_timer - minoffline * 48);
  }
  if (ch->wounds == 3 && (in_hospital(ch) || is_undead(ch))) {
    ch->heal_timer = UMAX(1, ch->heal_timer - (minoffline * 20));
  }
  if (minoffline > 120) {
    ch->pcdata->minion_limit = 0;
    ch->pcdata->ally_limit = 0;
  }
  if (ch->pcdata->travel_time > 0)
    ch->pcdata->travel_time =
        UMAX(1, ch->pcdata->travel_time - minoffline * 60);

  ch->pcdata->desclock -= minoffline * 12;
  if (ch->pcdata->desclock < 0)
    ch->pcdata->desclock = 0;

  if (ch->vassal > 0) {
    if (clan_lookup(ch->vassal) != NULL && clan_lookup(ch->vassal)->stasis == 0)
      vassal_to_clan(ch, ch->vassal);
    else
      ch->vassal = 0;
  }
  if (!in_fac(ch, ch->faction) && ch->faction > 0) {
    if (clan_lookup(ch->faction) != NULL &&
        clan_lookup(ch->faction)->stasis == 0)
      join_to_clan(ch, ch->faction);
    else
      ch->faction = 0;
  }
  if (!in_fac(ch, ch->fcore) && ch->fcore > 0) {
    if (clan_lookup(ch->fcore) != NULL && clan_lookup(ch->fcore)->stasis == 0)
      join_to_clan(ch, ch->fcore);
    else
      ch->fcore = 0;
  }
  if (!in_fac(ch, ch->fsect) && ch->fsect > 0) {
    if (clan_lookup(ch->fsect) != NULL && clan_lookup(ch->fsect)->stasis == 0)
      join_to_clan(ch, ch->fsect);
    else
      ch->fsect = 0;
  }
  if (!in_fac(ch, ch->fcult) && ch->fcult > 0) {
    if (clan_lookup(ch->fcult) != NULL && clan_lookup(ch->fcult)->stasis == 0)
      join_to_clan(ch, ch->fcult);
    else
      ch->fcult = 0;
  }

  ch->pcdata->conditions[COND_DRUNK] =
      UMAX(0, ch->pcdata->conditions[COND_DRUNK] - minoffline / 30);

  //                if(ch->pcdata->pubic_hair[0] == '\0' ||
  //                safe_strlen(ch->pcdata->pubic_hair) < 2 ||
  //! str_cmp(ch->pcdata->pubic_hair, "") || !str_cmp(ch->pcdata->pubic_hair, "
  //! "))
  //                {
  //                    free_string(ch->pcdata->pubic_hair);
  //                    ch->pcdata->pubic_hair = str_dup("wild and bushy");
  //                }
}

char *nospaces(char *begining) {
  char endstring[MSL];
  strcpy(endstring, "");
  char buf[MSL];
  char temp[MSL];
  sprintf(buf, "%s", begining);
  for (int i = 0; buf[i] != '\0'; i++) {
    if (buf[i] != ' ' && buf[i] != '\n') {
      sprintf(temp, "%c", buf[i]);
      strcat(endstring, temp);
    }
  }
  return str_dup(endstring);
}

void account_sync(CHAR_DATA *ch) {
  if (ch->desc == NULL)
    return;

  if (ch->pcdata->account == NULL && ch->desc->account != NULL)
    ch->pcdata->account = ch->desc->account;
  if (ch->desc->account == NULL && ch->pcdata->account != NULL)
    ch->desc->account = ch->pcdata->account;
  if (ch->pcdata->account == NULL)
    ch->pcdata->account = reload_account(ch->pcdata->account_name);
  ch->desc->account = ch->pcdata->account;

  DESCRIPTOR_DATA *d;
  for (DescList::iterator it = descriptor_list.begin();
       it != descriptor_list.end(); ++it) {
    d = *it;
    if (d->character == NULL)
      continue;
    if (d->character == ch)
      continue;
    if ((d->account != NULL &&
         !str_cmp(d->account->name, ch->pcdata->account->name)) ||
        (d->character->pcdata->account != NULL &&
         !str_cmp(d->character->pcdata->account->name,
                  ch->pcdata->account_name))) {
      d->account = ch->pcdata->account;
      d->character->pcdata->account = ch->pcdata->account;
    }
  }
}

CHAR_DATA *get_char_by_id(int id) {
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

    if (victim->id == id)
      return victim;
  }
  return NULL;
}

CHAR_DATA *get_char_by_account(char *argument) {
  CHAR_DATA *ch;
  for (DescList::iterator it = descriptor_list.begin();
       it != descriptor_list.end(); ++it) {
    if ((*it)->connected != CON_PLAYING)
      continue;

    ch = CH(*it);

    if (ch != NULL && ch->pcdata != NULL && ch->pcdata->account != NULL &&
        !str_cmp(ch->pcdata->account->name, argument))
      return ch;
  }

  return NULL;
}

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
